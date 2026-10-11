#include "MXCopyPasteManager.h"
#include "../Files/MXLargeCoWFileWindow.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"

#include <Base/LSWBase.h>
#include <Helpers/LSWHelpers.h>


namespace mx {

	static_assert( size_t( CCopyPasteManager::TotalClipboards ) == size_t( CLargeCoWFileWindow::TotalClipboards ),
		"CCopyPasteManager and CLargeCoWFileWindow must agree on the number of clipboards." );
	static_assert( size_t( CCopyPasteManager::WindowsClipboard ) < size_t( CCopyPasteManager::TotalClipboards ),
		"The Windows clipboard slot must be a valid slot." );


	// == Members.
	/** The data source held by each slot. */
	std::shared_ptr<CHexEditorInterface> CCopyPasteManager::m_spSlots[CCopyPasteManager::TotalClipboards];

	/** Guards m_spSlots and m_bShutDown. */
	std::mutex CCopyPasteManager::m_mMutex;

	/** Set by ShutDown(); once set, slots cannot be set. */
	bool CCopyPasteManager::m_bShutDown = false;

	/** The hidden window that owns the Windows clipboard. */
	HWND CCopyPasteManager::m_hWnd = NULL;

	/** The window class of m_hWnd. */
	ATOM CCopyPasteManager::m_aClass = 0;

	/** The formats advertised by the last deferred Copy (empty if the clipboard holds rendered data). */
	std::vector<UINT> CCopyPasteManager::m_vDeferredFormats;

	/** The number of bytes in the last deferred Copy. */
	uint64_t CCopyPasteManager::m_ui64DeferredBytes = 0;

	/** Set while this class empties the clipboard itself, so that its own WM_DESTROYCLIPBOARD does not clear slot 0. */
	bool CCopyPasteManager::m_bEmptying = false;


	// == Functions.
	/**
	 * \brief Registers the data source for a Copy operation into the given slot.
	 *
	 * The slot takes a reference to _spInterface and holds it until the slot is overwritten, cleared, or ShutDown() is called.  Any
	 *	different interface previously held by the slot has its data for the slot released (and is destroyed if this was its last
	 *	reference).  For slot 0, use SetWindowsClipboard() or SetWindowsClipboardDeferred() instead, which also update the Windows
	 *	clipboard.
	 *
	 * \param _sIdx The index of the slot to fill.  Must be less than TotalClipboards.
	 * \param _spInterface The interface from which the data was copied.  Must not be empty.
	 * \return Returns false if _sIdx is out of range, _spInterface is empty, or ShutDown() has already been called.
	 */
	bool CCopyPasteManager::SetSlot( size_t _sIdx, const std::shared_ptr<CHexEditorInterface> &_spInterface ) {
		if ( _sIdx >= TotalClipboards || !_spInterface ) { return false; }
		return ReplaceSlot( _sIdx, _spInterface, true );
	}

	/**
	 * \brief Gets the data source held by the given slot for a Paste operation.
	 *
	 * The slot is not modified.  The returned pointer is a new reference, so the interface remains alive for as long as the caller
	 *	holds it, even if the slot is overwritten or cleared in the meantime (for example, during a long streaming Paste).
	 *
	 * \param _sIdx The index of the slot to read.
	 * \return Returns the interface held by the slot, or an empty pointer if _sIdx is out of range or the slot is empty.
	 */
	std::shared_ptr<CHexEditorInterface> CCopyPasteManager::GetSlot( size_t _sIdx ) {
		if ( _sIdx >= TotalClipboards ) { return std::shared_ptr<CHexEditorInterface>(); }
		std::lock_guard<std::mutex> lgLock( m_mMutex );
		return m_spSlots[_sIdx];
	}

	/**
	 * \brief Determines whether the given slot currently holds a data source.
	 *
	 * \param _sIdx The index of the slot to check.
	 * \return Returns true if _sIdx is in range and the slot holds an interface.
	 */
	bool CCopyPasteManager::SlotIsSet( size_t _sIdx ) {
		if ( _sIdx >= TotalClipboards ) { return false; }
		std::lock_guard<std::mutex> lgLock( m_mMutex );
		return static_cast<bool>(m_spSlots[_sIdx]);
	}

	/**
	 * \brief Releases the data source held by the given slot.
	 *
	 * Intended for cases in which a slot's contents become invalid without a new Copy operation, such as another application taking
	 *	ownership of the Windows clipboard.
	 *
	 * \param _sIdx The index of the slot to clear.
	 * \return Returns true if the slot held an interface that was released.
	 */
	bool CCopyPasteManager::ClearSlot( size_t _sIdx ) {
		if ( _sIdx >= TotalClipboards ) { return false; }

		std::shared_ptr<CHexEditorInterface> spReleaseMe;
		{
			std::lock_guard<std::mutex> lgLock( m_mMutex );
			spReleaseMe = std::move( m_spSlots[_sIdx] );
		}
		// Outside of the lock: the interface may do work, or be destroyed, here.
		if ( !spReleaseMe ) { return false; }
		spReleaseMe->ReleaseClipboard( _sIdx );
		return true;
	}

	/**
	 * \brief Renders a Copy operation now and places it on the Windows clipboard, then registers _spInterface in slot 0.
	 *
	 * For data sources whose contents can change after the Copy (processes).  Uses lsw::LSW_CLIPBOARD.  UI thread only.
	 *
	 * \param _spInterface The interface from which the data was copied.
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _swsMsg Receives an error message on failure.
	 * \return Returns true if the clipboard was updated.
	 */
	bool CCopyPasteManager::SetWindowsClipboard( const std::shared_ptr<CHexEditorInterface> &_spInterface, const CCopyFormatter::MX_COPY_SOURCE &_csSource, CSecureWString &_swsMsg ) {
		if ( !_spInterface || IsShutDown() ) {
			_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		try {
			// Render everything before opening the clipboard so that it is held open only briefly.
			std::vector<UINT> vFormats;
			if ( !CCopyFormatter::Formats( _csSource.caCopyAs, vFormats ) ) {
				_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
				return false;
			}
			std::vector<CCopyVectorSink> vSinks( vFormats.size() );
			for ( size_t I = 0; I < vFormats.size(); ++I ) {
				if ( !CCopyFormatter::Render( _csSource, vFormats[I], vSinks[I], _swsMsg ) ) { return false; }
			}

			HWND hWnd = ClipboardWindow();
			if ( !hWnd ) {
				_swsMsg = _DEC_WS_C6498829_Failed_to_open_the_clipboard_;
				return false;
			}

			m_bEmptying = true;
			lsw::LSW_CLIPBOARD cbClip( hWnd, true );
			m_bEmptying = false;
			if ( !cbClip.IsOpen() ) {
				_swsMsg = _DEC_WS_C6498829_Failed_to_open_the_clipboard_;
				return false;
			}
			// The clipboard now holds rendered data, so nothing is left to render at exit.
			m_vDeferredFormats.clear();
			m_ui64DeferredBytes = 0;

			bool bAny = false;
			for ( size_t I = 0; I < vFormats.size(); ++I ) {
				try {
					if ( cbClip.SetData( vFormats[I], vSinks[I].Data() ) ) { bAny = true; }
				}
				catch ( ... ) {}
			}
			if ( !bAny ) {
				// The clipboard was emptied, so slot 0 no longer matches it.
				ReplaceSlot( WindowsClipboard, std::shared_ptr<CHexEditorInterface>(), false );
				_swsMsg = _DEC_WS_F90B4E11_Failed_to_place_the_data_on_the_clipboard_;
				return false;
			}
			ReplaceSlot( WindowsClipboard, _spInterface, true );
			return true;
		}
		catch ( ... ) {
			m_bEmptying = false;
			_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * \brief Advertises a Copy operation on the Windows clipboard without rendering it, then registers _spInterface in slot 0.
	 *
	 * _spInterface must already hold the Copy for slot 0 and be able to render it later through
	 *	CHexEditorInterface::RenderClipboard().  On failure, _spInterface's data for slot 0 is released (and slot 0 is cleared if it
	 *	referred to _spInterface, since its data was replaced).  UI thread only.
	 *
	 * \param _spInterface The interface from which the data was copied.
	 * \param _caCopyAs The copy parameters (determines the advertised formats).
	 * \param _ui64Bytes The number of bytes copied (determines whether the data is rendered at exit).
	 * \param _swsMsg Receives an error message on failure.
	 * \return Returns true if the clipboard was updated.
	 */
	bool CCopyPasteManager::SetWindowsClipboardDeferred( const std::shared_ptr<CHexEditorInterface> &_spInterface, const MX_COPYAS &_caCopyAs, uint64_t _ui64Bytes, CSecureWString &_swsMsg ) {
		if ( !_spInterface ) {
			_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		// The interface's slot-0 data has already been replaced, so on failure it must not stay reachable through slot 0.
		auto Fail = [&]( const CSecureWString &_swsWhy, bool _bClipboardEmptied ) -> bool {
			_swsMsg = _swsWhy;
			if ( _bClipboardEmptied || GetSlot( WindowsClipboard ) == _spInterface ) {
				ReplaceSlot( WindowsClipboard, std::shared_ptr<CHexEditorInterface>(), false );
			}
			_spInterface->ReleaseClipboard( WindowsClipboard );
			return false;
		};
		if ( IsShutDown() ) { return Fail( _DEC_WS_1468A1DF_Internal_error_, false ); }

		std::vector<UINT> vFormats, vAdvertised;
		try {
			if ( !CCopyFormatter::Formats( _caCopyAs, vFormats ) ) { return Fail( _DEC_WS_F39F91A5_Out_of_memory_, false ); }
			// Reserve now so that nothing can throw while the clipboard is open.
			vAdvertised.reserve( vFormats.size() );
		}
		catch ( ... ) { return Fail( _DEC_WS_F39F91A5_Out_of_memory_, false ); }

		HWND hWnd = ClipboardWindow();
		if ( !hWnd ) { return Fail( _DEC_WS_C6498829_Failed_to_open_the_clipboard_, false ); }

		m_bEmptying = true;
		lsw::LSW_CLIPBOARD cbClip( hWnd, true );
		m_bEmptying = false;
		if ( !cbClip.IsOpen() ) { return Fail( _DEC_WS_C6498829_Failed_to_open_the_clipboard_, false ); }

		for ( UINT uiFormat : vFormats ) {
			if ( cbClip.SetDeferredData( uiFormat ) ) { vAdvertised.push_back( uiFormat ); }
		}
		if ( vAdvertised.empty() ) {
			m_vDeferredFormats.clear();
			m_ui64DeferredBytes = 0;
			return Fail( _DEC_WS_F90B4E11_Failed_to_place_the_data_on_the_clipboard_, true );
		}
		m_vDeferredFormats.swap( vAdvertised );
		m_ui64DeferredBytes = _ui64Bytes;
		if ( !ReplaceSlot( WindowsClipboard, _spInterface, true ) ) { return Fail( _DEC_WS_1468A1DF_Internal_error_, true ); }
		return true;
	}

	/**
	 * \brief Completes a Copy from a data source whose contents can change after the Copy (a process).
	 *
	 * Slot 0 is rendered onto the Windows clipboard immediately (SetWindowsClipboard()).  For other slots the selected bytes are
	 *	captured into _ccCapture, which the interface renders from later.  _spInterface is then registered in the slot.  UI thread only.
	 *
	 * \param _spInterface The interface from which the data was copied.
	 * \param _csSource The data and parameters of the Copy operation, reading live from the interface.
	 * \param _sClipIdx The clipboard slot.
	 * \param _ccCapture The interface's capture for _sClipIdx.
	 * \param _swsMsg Receives an error message on failure.
	 * \return Returns true if the Copy completed.
	 */
	bool CCopyPasteManager::CopyLiveSource( const std::shared_ptr<CHexEditorInterface> &_spInterface, const CCopyFormatter::MX_COPY_SOURCE &_csSource,
		size_t _sClipIdx, CCopyCapture &_ccCapture, CSecureWString &_swsMsg ) {
		if ( !_spInterface || _sClipIdx >= TotalClipboards ) {
			_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		if ( _sClipIdx == WindowsClipboard ) {
			// The data goes to Windows now, so nothing is kept for slot 0.
			_ccCapture.Reset();
			return SetWindowsClipboard( _spInterface, _csSource, _swsMsg );
		}

		if ( !_ccCapture.Capture( _csSource, _swsMsg ) ) {
			// The previous capture for this slot is gone, so the slot cannot keep referring to this interface.
			if ( GetSlot( _sClipIdx ) == _spInterface ) { ClearSlot( _sClipIdx ); }
			return false;
		}
		if ( !SetSlot( _sClipIdx, _spInterface ) ) {
			_ccCapture.Reset();
			_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		return true;
	}

	/**
	 * \brief Determines whether the Windows clipboard still holds the data of the Copy in slot 0.
	 *
	 * When true, a Paste from slot 0 can read straight from GetSlot( WindowsClipboard ) instead of the Windows clipboard.  UI thread only.
	 *
	 * \return Returns true if this application still owns the Windows clipboard and slot 0 is set.
	 */
	bool CCopyPasteManager::OwnsWindowsClipboard() {
		return m_hWnd && ::GetClipboardOwner() == m_hWnd && SlotIsSet( WindowsClipboard );
	}

	/**
	 * \brief Gets the hidden message-only window that owns the Windows clipboard, creating it if necessary.  UI thread only.
	 *
	 * \return Returns the window, or NULL if it could not be created or ShutDown() has been called.
	 */
	HWND CCopyPasteManager::ClipboardWindow() {
		if ( m_hWnd ) { return m_hWnd; }
		if ( IsShutDown() ) { return NULL; }

		HINSTANCE hInstance = lsw::CBase::GetThisHandle();
		if ( !m_aClass ) {
			// Like every other window class in the application, the class name is random.
			WCHAR szName[24];
			CUtilities::RandomString( szName, std::size( szName ) );
			WNDCLASSEXW wcClass = {};
			wcClass.cbSize = sizeof( wcClass );
			wcClass.lpfnWndProc = WindowProc;
			wcClass.hInstance = hInstance;
			wcClass.lpszClassName = szName;
			m_aClass = ::RegisterClassExW( &wcClass );
			if ( !m_aClass ) { return NULL; }
		}
		m_hWnd = ::CreateWindowExW( 0, MAKEINTATOM( m_aClass ), L"", 0,
			0, 0, 0, 0,
			HWND_MESSAGE, NULL, hInstance, NULL );
		return m_hWnd;
	}

	/**
	 * \brief Releases all slots and prevents any further slots from being set.
	 *
	 * Call once from the UI thread as the application shuts down, before the Hex Editor and the libraries its interfaces depend upon
	 *	are torn down.  A small deferred copy on the Windows clipboard is rendered first (see MaxRenderOnExit) so that it survives the
	 *	application; interfaces kept alive only by this class are then destroyed in an orderly manner rather than during static
	 *	destruction.  Calling it more than once is harmless.
	 */
	void CCopyPasteManager::ShutDown() {
		// Destroying the clipboard owner sends WM_RENDERALLFORMATS while slot 0 is still alive.
		if ( m_hWnd ) {
			HWND hWnd = m_hWnd;
			::DestroyWindow( hWnd );
			m_hWnd = NULL;
		}
		if ( m_aClass ) {
			::UnregisterClassW( MAKEINTATOM( m_aClass ), lsw::CBase::GetThisHandle() );
			m_aClass = 0;
		}
		m_vDeferredFormats = std::vector<UINT>();
		m_ui64DeferredBytes = 0;

		// Declared before the lock so that the released interfaces are destroyed after the lock is released.
		std::shared_ptr<CHexEditorInterface> spReleaseMe[TotalClipboards];
		{
			std::lock_guard<std::mutex> lgLock( m_mMutex );
			m_bShutDown = true;
			for ( size_t I = 0; I < TotalClipboards; ++I ) {
				spReleaseMe[I] = std::move( m_spSlots[I] );
			}
		}
		for ( size_t I = 0; I < TotalClipboards; ++I ) {
			if ( spReleaseMe[I] ) { spReleaseMe[I]->ReleaseClipboard( I ); }
		}
	}

	/**
	 * \brief Determines whether ShutDown() has been called.
	 *
	 * \return Returns true if ShutDown() has been called.
	 */
	bool CCopyPasteManager::IsShutDown() {
		std::lock_guard<std::mutex> lgLock( m_mMutex );
		return m_bShutDown;
	}

	/**
	 * \brief Replaces the interface in a slot and releases the outgoing interface's data for the slot if it is a different interface.
	 *
	 * \param _sIdx The index of the slot.  Must be less than TotalClipboards.
	 * \param _spInterface The new interface (may be empty to clear the slot).
	 * \param _bRespectShutDown If true, fails once ShutDown() has been called.
	 * \return Returns false only if _bRespectShutDown is true and ShutDown() has been called.
	 */
	bool CCopyPasteManager::ReplaceSlot( size_t _sIdx, const std::shared_ptr<CHexEditorInterface> &_spInterface, bool _bRespectShutDown ) {
		std::shared_ptr<CHexEditorInterface> spOld;
		{
			std::lock_guard<std::mutex> lgLock( m_mMutex );
			if ( _bRespectShutDown && m_bShutDown ) { return false; }
			spOld = std::move( m_spSlots[_sIdx] );
			m_spSlots[_sIdx] = _spInterface;
		}
		// Outside of the lock.  If the same interface copied again, its new data for the slot is already in place.
		if ( spOld && spOld != _spInterface ) { spOld->ReleaseClipboard( _sIdx ); }
		return true;
	}

	/**
	 * \brief The window procedure of m_hWnd.  Handles WM_RENDERFORMAT, WM_RENDERALLFORMATS, and WM_DESTROYCLIPBOARD.
	 *
	 * \param _hWnd The window.
	 * \param _uMsg The message.
	 * \param _wParam The message's WPARAM.
	 * \param _lParam The message's LPARAM.
	 * \return Returns the result of the message.
	 */
	LRESULT CALLBACK CCopyPasteManager::WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		switch ( _uMsg ) {
			case WM_RENDERFORMAT : {
				// Another application wants one of the deferred formats.  The clipboard is already open on its behalf.
				RenderFormat( static_cast<UINT>(_wParam) );
				return 0;
			}
			case WM_RENDERALLFORMATS : {
				RenderAllFormats( _hWnd );
				return 0;
			}
			case WM_DESTROYCLIPBOARD : {
				// Another application emptied the clipboard, so slot 0 no longer describes it.
				if ( !m_bEmptying ) {
					m_vDeferredFormats.clear();
					m_ui64DeferredBytes = 0;
					ClearSlot( WindowsClipboard );
				}
				return 0;
			}
			default : {}
		}
		return ::DefWindowProcW( _hWnd, _uMsg, _wParam, _lParam );
	}

	/**
	 * \brief Renders one deferred format from slot 0 and passes it to ::SetClipboardData().  The clipboard must already be open.
	 *
	 * \param _uiFormat The clipboard format requested.
	 */
	void CCopyPasteManager::RenderFormat( UINT _uiFormat ) {
		auto spInterface = GetSlot( WindowsClipboard );
		if ( !spInterface ) { return; }

		CCopyGlobalSink cgsSink;
		CSecureWString swsMsg;
		if ( !spInterface->RenderClipboard( WindowsClipboard, _uiFormat, cgsSink, swsMsg ) ) { return; }
		HGLOBAL hGlobal = cgsSink.Release();
		if ( hGlobal && NULL == ::SetClipboardData( _uiFormat, hGlobal ) ) {
			// Ownership was not transferred.
			::GlobalFree( hGlobal );
		}
	}

	/**
	 * \brief Renders every deferred format before the clipboard owner is destroyed (WM_RENDERALLFORMATS).
	 *
	 * \param _hWnd The clipboard owner.
	 */
	void CCopyPasteManager::RenderAllFormats( HWND _hWnd ) {
		// A very large copy is dropped rather than stalling the exit; Windows removes formats that were never rendered.
		if ( m_vDeferredFormats.empty() || m_ui64DeferredBytes > MaxRenderOnExit ) { return; }

		lsw::LSW_CLIPBOARD cbClip( _hWnd, false );
		if ( !cbClip.IsOpen() || ::GetClipboardOwner() != _hWnd ) { return; }
		for ( UINT uiFormat : m_vDeferredFormats ) {
			RenderFormat( uiFormat );
		}
	}

}	// namespace mx

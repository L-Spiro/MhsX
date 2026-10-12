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

	/** The active slot. */
	size_t CCopyPasteManager::m_sActive = CCopyPasteManager::WindowsClipboard;


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
			// Rendered directly into global memory, which is handed to the clipboard without another copy.  A format that fails to
			//	render is left out rather than abandoning the whole Copy.
			std::vector<std::unique_ptr<CCopyGlobalSink>> vSinks;
			bool bRendered = false;
			for ( size_t I = 0; I < vFormats.size(); ++I ) {
				vSinks.push_back( std::make_unique<CCopyGlobalSink>() );
				if ( CCopyFormatter::Render( _csSource, vFormats[I], (*vSinks[I]), _swsMsg ) ) { bRendered = true; }
				else { vSinks[I].reset(); }
			}
			if ( !bRendered ) { return false; }

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

			// Every format is placed; the clipboard holds several at once.
			bool bAny = false;
			for ( size_t I = 0; I < vFormats.size(); ++I ) {
				if ( vSinks[I] && cbClip.SetHandle( vFormats[I], (*vSinks[I]).Release() ) ) { bAny = true; }
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
	 * \brief Sets the active clipboard slot, used by Cut, Copy, and Paste.  UI thread only.
	 *
	 * \param _sIdx The slot to make active.
	 * \return Returns false if _sIdx is out of range.
	 */
	bool CCopyPasteManager::SetActiveClipboard( size_t _sIdx ) {
		if ( _sIdx >= TotalClipboards ) { return false; }
		m_sActive = _sIdx;
		return true;
	}

	/**
	 * \brief Determines whether a Paste from the given slot would have anything to paste.  UI thread only.
	 *
	 * \param _sIdx The slot.
	 * \return Returns true if the slot holds a Copy, or for slot 0, if the Windows clipboard holds binary data or text.
	 */
	bool CCopyPasteManager::HasPasteData( size_t _sIdx ) {
		if ( _sIdx >= TotalClipboards ) { return false; }
		if ( _sIdx != WindowsClipboard ) { return SlotIsSet( _sIdx ); }
		if ( OwnsWindowsClipboard() ) { return true; }
		return (CCopyFormatter::BinaryFormat() && ::IsClipboardFormatAvailable( CCopyFormatter::BinaryFormat() )) ||
			::IsClipboardFormatAvailable( CF_UNICODETEXT ) || ::IsClipboardFormatAvailable( CF_TEXT );
	}

	/**
	 * \brief Resolves what a Paste from the given slot inserts.  UI thread only.
	 *
	 * - A raw Copy (MX_CA_BINARY) held by an interface is pasted from that interface (_pdData.spSource), so any amount of data can
	 *	be streamed, or spliced instantly when pasting into the file it was copied from.
	 * - A Copy As text format pastes its text.
	 * - For slot 0, data placed on the Windows clipboard by another application pastes its "BinaryData" if present, otherwise its
	 *	text.
	 * Text is converted to bytes in the given character set (UTF-16 for Unicode, otherwise the character set's code page).
	 *
	 * \param _sIdx The slot.
	 * \param _csCharSet The character set of the view being pasted into.
	 * \param _pdData Receives the data to paste.
	 * \param _swsMsg Receives an error message on failure, including when there is nothing to paste.
	 * \return Returns true if there is data to paste.
	 */
	bool CCopyPasteManager::GetPasteData( size_t _sIdx, CCharSets::MX_CHAR_SETS _csCharSet, MX_PASTE_DATA &_pdData, CSecureWString &_swsMsg ) {
		try {
			_pdData = MX_PASTE_DATA();
			if ( _sIdx >= TotalClipboards ) {
				_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
				return false;
			}

			std::shared_ptr<CHexEditorInterface> spSource = GetSlot( _sIdx );
			// Slot 0 only describes the Windows clipboard while this application still owns it.
			if ( _sIdx == WindowsClipboard && !OwnsWindowsClipboard() ) { spSource.reset(); }

			MX_COPYAS caParms;
			if ( spSource && spSource->ClipboardCopyAs( _sIdx, caParms ) ) {
				if ( caParms.caCopyFmt == MX_CA_BINARY ) {
					_pdData.spSource = spSource;
					_pdData.sSourceSlot = _sIdx;
					_pdData.ui64Size = spSource->ClipboardBytes( _sIdx );
				}
				else {
					// A Copy As format pastes as the text it produced.
					CCopyVectorSink cvsText;
					if ( !spSource->RenderClipboard( _sIdx, CF_UNICODETEXT, cvsText, _swsMsg ) ) { return false; }
					const wchar_t * pwcText = reinterpret_cast<const wchar_t *>(cvsText.Data().data());
					const size_t sLen = ::wcsnlen( pwcText, cvsText.Data().size() / sizeof( wchar_t ) );
					if ( !TextToBytes( pwcText, sLen, _csCharSet, _pdData.vBytes ) ) {
						_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
						return false;
					}
					_pdData.ui64Size = _pdData.vBytes.size();
				}
			}
			else if ( _sIdx == WindowsClipboard ) {
				// Another application's data, or a process's Copy that was placed on the clipboard directly.
				if ( !ReadWindowsClipboard( _csCharSet, _pdData.vBytes, _swsMsg ) ) { return false; }
				_pdData.ui64Size = _pdData.vBytes.size();
			}

			if ( !_pdData.ui64Size ) {
				_pdData = MX_PASTE_DATA();
				_swsMsg = _DEC_WS_3D9DD0BF_The_clipboard_is_empty_;
				return false;
			}
			return true;
		}
		catch ( ... ) {
			_pdData = MX_PASTE_DATA();
			_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * \brief Empties every slot.  The Windows clipboard is emptied only if it holds this application's data.  UI thread only.
	 */
	void CCopyPasteManager::ClearAll() {
		for ( size_t I = 0; I < TotalClipboards; ++I ) {
			if ( I != WindowsClipboard ) { ClearSlot( I ); }
		}
		if ( m_hWnd && ::GetClipboardOwner() == m_hWnd ) {
			// Emptying sends this window WM_DESTROYCLIPBOARD, which releases slot 0 and the deferred formats.
			lsw::LSW_CLIPBOARD cbClip( m_hWnd, true );
		}
		ClearSlot( WindowsClipboard );
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
	 * \brief Reads the Windows clipboard data of another application for pasting.  UI thread only.
	 *
	 * \param _csCharSet The character set into which text is converted.
	 * \param _vBytes Receives the bytes.
	 * \param _swsMsg Receives an error message on failure.
	 * \return Returns true if there was binary data or text to read.
	 */
	bool CCopyPasteManager::ReadWindowsClipboard( CCharSets::MX_CHAR_SETS _csCharSet, std::vector<uint8_t> &_vBytes, CSecureWString &_swsMsg ) {
		_vBytes.clear();
		try {
			lsw::LSW_CLIPBOARD cbClip( ClipboardWindow(), false );
			if ( !cbClip.IsOpen() ) {
				_swsMsg = _DEC_WS_C6498829_Failed_to_open_the_clipboard_;
				return false;
			}
			// Raw bytes first.
			if ( CCopyFormatter::BinaryFormat() && cbClip.GetData( CCopyFormatter::BinaryFormat(), _vBytes ) ) { return true; }

			std::vector<uint8_t> vTmp;
			if ( cbClip.GetData( CF_UNICODETEXT, vTmp ) ) {
				const wchar_t * pwcText = reinterpret_cast<const wchar_t *>(vTmp.data());
				const size_t sLen = ::wcsnlen( pwcText, vTmp.size() / sizeof( wchar_t ) );
				if ( !TextToBytes( pwcText, sLen, _csCharSet, _vBytes ) ) {
					_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
					return false;
				}
				return true;
			}
			if ( cbClip.GetData( CF_TEXT, vTmp ) ) {
				const size_t sLen = ::strnlen( reinterpret_cast<const char *>(vTmp.data()), vTmp.size() );
				_vBytes.assign( vTmp.begin(), vTmp.begin() + sLen );
				return true;
			}
			_swsMsg = _DEC_WS_3D9DD0BF_The_clipboard_is_empty_;
			return false;
		}
		catch ( ... ) {
			_vBytes.clear();
			_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * \brief Converts text to bytes in a character set: UTF-16 for Unicode, otherwise the character set's code page (falling back
	 *	to the ANSI code page if that code page is not installed).
	 *
	 * \param _pwcText The text.
	 * \param _sLen The number of characters to which _pwcText points.
	 * \param _csCharSet The character set.
	 * \param _vBytes Receives the bytes.
	 * \return Returns false on a memory or conversion failure.
	 */
	bool CCopyPasteManager::TextToBytes( const wchar_t * _pwcText, size_t _sLen, CCharSets::MX_CHAR_SETS _csCharSet, std::vector<uint8_t> &_vBytes ) {
		try {
			_vBytes.clear();
			if ( !_sLen ) { return true; }
			UINT uiCodePage = static_cast<UINT>(CCharSets::m_csSets[_csCharSet].ui32Code);
			if ( _csCharSet == CCharSets::MX_CS_UNICODE || uiCodePage == CCodePages::MX_utf_16 ) {
				const uint8_t * pui8Text = reinterpret_cast<const uint8_t *>(_pwcText);
				_vBytes.assign( pui8Text, pui8Text + _sLen * sizeof( wchar_t ) );
				return true;
			}
			// Probe the code page; it might not be installed.
			if ( !::WideCharToMultiByte( uiCodePage, 0, _pwcText, 1, nullptr, 0, nullptr, nullptr ) ) { uiCodePage = CP_ACP; }

			// Convert in pieces that fit in an int, never splitting a surrogate pair.
			constexpr size_t sPiece = 16 * 1024 * 1024;
			for ( size_t sDone = 0; sDone < _sLen; ) {
				size_t sThis = std::min( _sLen - sDone, sPiece );
				if ( sDone + sThis < _sLen && _pwcText[sDone+sThis-1] >= 0xD800 && _pwcText[sDone+sThis-1] <= 0xDBFF ) { --sThis; }
				const int iNeeded = ::WideCharToMultiByte( uiCodePage, 0, _pwcText + sDone, int( sThis ), nullptr, 0, nullptr, nullptr );
				if ( iNeeded <= 0 ) { return false; }
				const size_t sOld = _vBytes.size();
				_vBytes.resize( sOld + size_t( iNeeded ) );
				if ( ::WideCharToMultiByte( uiCodePage, 0, _pwcText + sDone, int( sThis ), reinterpret_cast<char *>(_vBytes.data() + sOld), iNeeded, nullptr, nullptr ) != iNeeded ) {
					return false;
				}
				sDone += sThis;
			}
			return true;
		}
		catch ( ... ) { return false; }
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

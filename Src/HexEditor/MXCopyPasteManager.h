#pragma once

#include "../MXMhsX.h"
#include "MXCopyFormatter.h"
#include "MXHexEditorInterface.h"

#include <memory>
#include <mutex>
#include <vector>


namespace mx {

	/**
	 * Class CCopyPasteManager
	 * \brief Owns references to the data sources behind each of the Copy/Paste slots, and owns the Windows clipboard on their behalf.
	 *
	 * A Copy operation registers the CHexEditorInterface it copied from into one of TotalClipboards slots.  The slot keeps that
	 *	interface alive (even after its Hex Editor tab or the whole Hex Editor window has been closed) so that the copied data can be
	 *	produced later, on demand, when a Paste operation occurs.
	 *
	 * Slot 0 (WindowsClipboard) is shared with other applications through the Windows clipboard, which is owned by a hidden
	 *	message-only window created by this class:
	 *	- SetWindowsClipboard() renders the data immediately (for sources such as processes whose contents can change).
	 *	- SetWindowsClipboardDeferred() only advertises the formats; the interface renders them when another application asks for
	 *		them (WM_RENDERFORMAT), so copying gigabytes is instant and costs nothing until a Paste.
	 *	When another application takes the clipboard, slot 0 is released.
	 *
	 * Pasting does not remove anything from a slot, so a single Copy can be pasted any number of times.  A slot is released only when a
	 *	later Copy operation overwrites it, when ClearSlot() is called, or when ShutDown() is called as the application closes.  When a
	 *	slot stops referring to an interface, that interface's CHexEditorInterface::ReleaseClipboard() is called for the slot.
	 *
	 * The slot functions are thread-safe.  The Windows-clipboard functions and ShutDown() must be called from the UI thread.  Interfaces
	 *	released by this class are always destroyed outside of its lock.
	 */
	class CCopyPasteManager {
	public :
		CCopyPasteManager() = delete;


		// == Constants.
		/**
		 * The number of Copy/Paste slots, including the Windows clipboard (index 0).  Must match CLargeCoWFileWindow::TotalClipboards.
		 */
		enum : size_t { TotalClipboards = 10 };

		/**
		 * The index of the slot that is backed by the Windows clipboard.
		 */
		enum : size_t { WindowsClipboard = 0 };

		/**
		 * The largest deferred copy (in source bytes) that is rendered onto the clipboard when the application exits.  Larger copies are
		 *	dropped from the clipboard at exit rather than stalling the shut-down.
		 */
		static constexpr uint64_t								MaxRenderOnExit = 64ULL * 1024ULL * 1024ULL;


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
		static bool												SetSlot( size_t _sIdx, const std::shared_ptr<CHexEditorInterface> &_spInterface );

		/**
		 * \brief Gets the data source held by the given slot for a Paste operation.
		 *
		 * The slot is not modified.  The returned pointer is a new reference, so the interface remains alive for as long as the caller
		 *	holds it, even if the slot is overwritten or cleared in the meantime (for example, during a long streaming Paste).
		 *
		 * \param _sIdx The index of the slot to read.
		 * \return Returns the interface held by the slot, or an empty pointer if _sIdx is out of range or the slot is empty.
		 */
		static std::shared_ptr<CHexEditorInterface>				GetSlot( size_t _sIdx );

		/**
		 * \brief Determines whether the given slot currently holds a data source.
		 *
		 * \param _sIdx The index of the slot to check.
		 * \return Returns true if _sIdx is in range and the slot holds an interface.
		 */
		static bool												SlotIsSet( size_t _sIdx );

		/**
		 * \brief Releases the data source held by the given slot.
		 *
		 * Intended for cases in which a slot's contents become invalid without a new Copy operation, such as another application taking
		 *	ownership of the Windows clipboard.
		 *
		 * \param _sIdx The index of the slot to clear.
		 * \return Returns true if the slot held an interface that was released.
		 */
		static bool												ClearSlot( size_t _sIdx );

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
		static bool												SetWindowsClipboard( const std::shared_ptr<CHexEditorInterface> &_spInterface, const CCopyFormatter::MX_COPY_SOURCE &_csSource, CSecureWString &_swsMsg );

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
		static bool												SetWindowsClipboardDeferred( const std::shared_ptr<CHexEditorInterface> &_spInterface, const MX_COPYAS &_caCopyAs, uint64_t _ui64Bytes, CSecureWString &_swsMsg );

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
		static bool												CopyLiveSource( const std::shared_ptr<CHexEditorInterface> &_spInterface, const CCopyFormatter::MX_COPY_SOURCE &_csSource,
			size_t _sClipIdx, CCopyCapture &_ccCapture, CSecureWString &_swsMsg );

		/**
		 * \brief Gets the active clipboard slot, used by Cut, Copy, and Paste.  UI thread only.
		 *
		 * \return Returns the active slot (WindowsClipboard by default).
		 */
		static size_t											ActiveClipboard() { return m_sActive; }

		/**
		 * \brief Sets the active clipboard slot, used by Cut, Copy, and Paste.  UI thread only.
		 *
		 * \param _sIdx The slot to make active.
		 * \return Returns false if _sIdx is out of range.
		 */
		static bool												SetActiveClipboard( size_t _sIdx );

		/**
		 * \brief Determines whether a Paste from the given slot would have anything to paste.  UI thread only.
		 *
		 * \param _sIdx The slot.
		 * \return Returns true if the slot holds a Copy, or for slot 0, if the Windows clipboard holds binary data or text.
		 */
		static bool												HasPasteData( size_t _sIdx );

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
		static bool												GetPasteData( size_t _sIdx, CCharSets::MX_CHAR_SETS _csCharSet, MX_PASTE_DATA &_pdData, CSecureWString &_swsMsg );

		/**
		 * \brief Empties every slot.  The Windows clipboard is emptied only if it holds this application's data.  UI thread only.
		 */
		static void												ClearAll();

		/**
		 * \brief Determines whether the Windows clipboard still holds the data of the Copy in slot 0.
		 *
		 * When true, a Paste from slot 0 can read straight from GetSlot( WindowsClipboard ) instead of the Windows clipboard.  UI thread only.
		 *
		 * \return Returns true if this application still owns the Windows clipboard and slot 0 is set.
		 */
		static bool												OwnsWindowsClipboard();

		/**
		 * \brief Gets the hidden message-only window that owns the Windows clipboard, creating it if necessary.  UI thread only.
		 *
		 * \return Returns the window, or NULL if it could not be created or ShutDown() has been called.
		 */
		static HWND												ClipboardWindow();

		/**
		 * \brief Releases all slots and prevents any further slots from being set.
		 *
		 * Call once from the UI thread as the application shuts down, before the Hex Editor and the libraries its interfaces depend upon
		 *	are torn down.  A small deferred copy on the Windows clipboard is rendered first (see MaxRenderOnExit) so that it survives the
		 *	application; interfaces kept alive only by this class are then destroyed in an orderly manner rather than during static
		 *	destruction.  Calling it more than once is harmless.
		 */
		static void												ShutDown();

		/**
		 * \brief Determines whether ShutDown() has been called.
		 *
		 * \return Returns true if ShutDown() has been called.
		 */
		static bool												IsShutDown();


	protected :
		// == Members.
		/** The data source held by each slot. */
		static std::shared_ptr<CHexEditorInterface>				m_spSlots[TotalClipboards];
		/** Guards m_spSlots and m_bShutDown. */
		static std::mutex										m_mMutex;
		/** Set by ShutDown(); once set, slots cannot be set. */
		static bool												m_bShutDown;

		/** The hidden window that owns the Windows clipboard. */
		static HWND												m_hWnd;
		/** The window class of m_hWnd. */
		static ATOM												m_aClass;
		/** The formats advertised by the last deferred Copy (empty if the clipboard holds rendered data). */
		static std::vector<UINT>								m_vDeferredFormats;
		/** The number of bytes in the last deferred Copy. */
		static uint64_t											m_ui64DeferredBytes;
		/** Set while this class empties the clipboard itself, so that its own WM_DESTROYCLIPBOARD does not clear slot 0. */
		static bool												m_bEmptying;
		/** The active slot. */
		static size_t											m_sActive;


		// == Functions.
		/**
		 * \brief Replaces the interface in a slot and releases the outgoing interface's data for the slot if it is a different interface.
		 *
		 * \param _sIdx The index of the slot.  Must be less than TotalClipboards.
		 * \param _spInterface The new interface (may be empty to clear the slot).
		 * \param _bRespectShutDown If true, fails once ShutDown() has been called.
		 * \return Returns false only if _bRespectShutDown is true and ShutDown() has been called.
		 */
		static bool												ReplaceSlot( size_t _sIdx, const std::shared_ptr<CHexEditorInterface> &_spInterface, bool _bRespectShutDown );

		/**
		 * \brief Reads the Windows clipboard data of another application for pasting.  UI thread only.
		 *
		 * \param _csCharSet The character set into which text is converted.
		 * \param _vBytes Receives the bytes.
		 * \param _swsMsg Receives an error message on failure.
		 * \return Returns true if there was binary data or text to read.
		 */
		static bool												ReadWindowsClipboard( CCharSets::MX_CHAR_SETS _csCharSet, std::vector<uint8_t> &_vBytes, CSecureWString &_swsMsg );

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
		static bool												TextToBytes( const wchar_t * _pwcText, size_t _sLen, CCharSets::MX_CHAR_SETS _csCharSet, std::vector<uint8_t> &_vBytes );

		/**
		 * \brief The window procedure of m_hWnd.  Handles WM_RENDERFORMAT, WM_RENDERALLFORMATS, and WM_DESTROYCLIPBOARD.
		 *
		 * \param _hWnd The window.
		 * \param _uMsg The message.
		 * \param _wParam The message's WPARAM.
		 * \param _lParam The message's LPARAM.
		 * \return Returns the result of the message.
		 */
		static LRESULT CALLBACK									WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

		/**
		 * \brief Renders one deferred format from slot 0 and passes it to ::SetClipboardData().  The clipboard must already be open.
		 *
		 * \param _uiFormat The clipboard format requested.
		 */
		static void												RenderFormat( UINT _uiFormat );

		/**
		 * \brief Renders every deferred format before the clipboard owner is destroyed (WM_RENDERALLFORMATS).
		 *
		 * \param _hWnd The clipboard owner.
		 */
		static void												RenderAllFormats( HWND _hWnd );
	};

}	// namespace mx

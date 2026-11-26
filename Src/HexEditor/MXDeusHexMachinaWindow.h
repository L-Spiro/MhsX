#pragma once

#include "../MXMhsX.h"
#include "../ExpEval/MXExpEvalWindow.h"
#include "../Layouts/MXDeuHexMachinaWindowLayout.h"
#include "../Layouts/MXExportHexLayout.h"
#include "../Layouts/MXFindLayout.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Layouts/MXNumberInputLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "MXFindResults.h"
#include "MXHexEditorControl.h"
#include "MXHexEditorControlHost.h"
#include "MXHexEditorInterface.h"

#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>
#include <MultiSplitter/LSWMultiSplitter.h>
#include <StatusBar/LSWStatusBar.h>
#include <Tab/LSWTab.h>
#include <TreeListView/LSWTreeListView.h>

#include <filesystem>
#include <functional>

using namespace lsw;

namespace mx {

	class CHexEditorFile;
	
	class CWindowMemHack;

	class CDeusHexMachinaWindow : public lsw::CMainWindow, CHexEditorControlHost {
	public :
		CDeusHexMachinaWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CDeusHexMachinaWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_NEW,
			MX_I_OPENFILE,
			MX_I_CURPROC,
			MX_I_SAVE,
			MX_I_SAVEAS,
			MX_I_SAVEALL,
			MX_I_CLOSE,

			MX_I_CUT,
			MX_I_COPY,
			MX_I_PASTE,
			MX_I_UNDO,
			MX_I_REDO,

			MX_I_FIND,
			MX_I_GOTO,

			MX_I_EXP,
			MX_I_COMPARE,
			MX_I_HISTOGRAM,

			MX_I_TOTAL,
		};

		// Menus.
		enum MX_MENUS {
			MX_M_CONTEXT_MENU						= 200,
			MX_M_CONTEXT_SELECT,																	/**< The context menu's Select submenu. */
			MX_M_CONTEXT_SELECT_ITEM,																/**< The context menu's item that opens the Select submenu. */
		};

		// Status bar parts.
		enum MX_STATUS_BAR {
			MX_SB_MESSAGE,
			MX_SB_POSITION_START,
			MX_SB_VALUE_SEL,
			MX_SB_FILE_SIZE,
			MX_SB_VIEW_TYPE,
			MX_SB_CHAR_SET,
			MX_SB_ENDIAN,
			MX_SB_CLIPBOARD,
			MX_SB_INSERT_OVERWRITE,

			MX_SB_TOTAL
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED							InitDialog();

		/**
		 * Handles WM_COMMAND dispatch.
		 * \brief Routes command notifications from menus, accelerators, or child controls.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
		 * \param _wId The identifier for the command or control.
		 * \param _pwSrc The source control for control notifications; otherwise nullptr.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Custom user-defined message handler.
		 * \brief Handles WM_USER and other private messages.
		 *
		 * \param _uMsg Message identifier.
		 * \param _wParam WPARAM data.
		 * \param _lParam LPARAM data.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							CustomPrivateMsg( UINT _uMsg, WPARAM _wParam, LPARAM /*_lParam*/ );

		// WM_COMMAND from menu.
		virtual LSW_HANDLED							MenuCommand( WORD _wId );

		/**
		 * Handles WM_CONTEXTMENU.  Right-clicking the hex editor shows its context menu.
		 *
		 * \param _pwControl The control that was right-clicked.
		 * \param _iX The horizontal position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
		 * \param _iY The vertical position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							ContextMenu( CWidget * _pwControl, INT _iX, INT _iY );

		/**
		 * Shows the hex editor's context menu.  Called by the hex control itself when it gets WM_CONTEXTMENU.
		 *
		 * \param _pwControl The hex control.
		 * \param _iX The horizontal position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
		 * \param _iY The vertical position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
		 * \return Returns true if a menu was shown.
		 **/
		virtual bool								ShowHexContextMenu( lsw::CWidget * _pwControl, INT _iX, INT _iY );

		// WM_CLOSE.
		virtual LSW_HANDLED							Close();

		// WM_ERASEBKGND.
		virtual LSW_HANDLED							EraseBkgnd( HDC _hDc );

		/**
		 * Handles WM_SETFOCUS.
		 * \brief Notified when the window receives keyboard focus.
		 *
		 * \param _hPrevFocus The window that previously had focus.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							SetFocus( HWND /*_hPrevFocus*/ ) override;

		// Determines if the control is focused or not.
		virtual bool								IsFocused() { return GetFocus(); }

		/**
		 * Handles WM_KILLFOCUS.
		 * \brief Notified when the window loses keyboard focus.
		 *
		 * \param _hNewFocus The window receiving focus (may be NULL).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							KillFocus( HWND /*_hNewFocus*/ ) override;

		/**
		 * Notification for when a child tab changes its selection.
		 * 
		 * \param param _pwTab Pointer to the tab control.
		 * \param _iTab Zero-based tab index that was selected.
		 * \param _lpnHdr Contains information about the notification message.
		 */
		virtual void								TabSelChanged( CWidget * _pwTab, int _iTab, LPNMHDR _lpnHdr ) override;

		/**
		 * The WM_KEYDOWN handler.
		 *
		 * \param _uiKeyCode The virtual-key code of the nonsystem key.
		 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							KeyDown( UINT _uiKeyCode, UINT _uiFlags );

		// Translate a child's tooltip text.
		virtual std::wstring						TranslateTooltip( const std::string &_sText ) { 
			return CStringDecoder::DecodeToWString( _sText.c_str(), _sText.size() );
		}

		// Gets the status bar.
		CStatusBar *								StatusBar();

		// Gets the status bar.
		const CStatusBar *							StatusBar() const;

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const LSW_RECT						VirtualClientRect( const CWidget * pwChild ) const;

		// Informs tha a child tab closed one of it headers.
		virtual void								ChildTabClosed( CWidget * _pwChild, INT _iTab );

		/**
		 * Asked before a tab is closed with its close box.  Offers to save a modified file.
		 *
		 * \param _pwChild The tab control.
		 * \param _iTab The tab being closed.
		 * \return Returns false to keep the tab open.
		 */
		virtual bool								ChildTabClosing( CWidget * _pwChild, INT _iTab ) override;

		// Gets the multi-splitter.
		CMultiSplitter *							MultiSplitter() {
			return static_cast<CMultiSplitter *>(FindChild( Layout::MX_W_MAIN_SPLITTER ));
		}

		// Gets the main tab.
		CTab *										Tab() {
			return static_cast<CTab *>(FindChild( Layout::MX_W_TABS ));
		}

		/** The kinds of file File > New creates. */
		enum MX_NEW_FILE {
			MX_NF_TEXT,																		/**< A text file. */
			MX_NF_HEX,																		/**< A binary file, edited as hex. */
			MX_NF_UNICODE,																	/**< A UTF-16 (little-endian) text file, starting with a byte-order mark. */
			MX_NF_UTF8,																		/**< A UTF-8 text file. */
			MX_NF_C,																		/**< A C/C++ source file. */
			MX_NF_HTML,																		/**< An HTML file. */
			MX_NF_PHP,																		/**< A PHP file. */
			MX_NF_XML,																		/**< An XML file. */
		};

		/**
		 * Creates a new, untitled file in a new tab.
		 *
		 * \param _nfType The kind of file.
		 * \return Returns true if the file was created.
		 **/
		bool										NewFile( MX_NEW_FILE _nfType );

		/**
		 * Performs a Save As operation on the current file.
		 **/
		void										SaveAs();

		/**
		 * Performs a Save operation on the current file (Save As if it has never been saved).
		 **/
		void										Save();

		/**
		 * Saves a file.  An untitled file, or Save As, asks for a path.  A read-only file can only be saved under another name, after
		 *	which the tab edits the new file.
		 *
		 * \param _sIdx The tab.
		 * \param _bAs If true, asks for a new path (Save As).
		 * \return Returns true if the file was saved; false if it failed or was cancelled.
		 **/
		bool										SaveTab( size_t _sIdx, bool _bAs );

		/**
		 * Writes a copy of the current file without changing which file is being edited.
		 **/
		void										SaveACopy();

		/**
		 * Writes the selected bytes (a file's or a process's) to a new file.
		 **/
		void										SaveSelection();

		/**
		 * Saves every modified file (read-only files are skipped).
		 **/
		void										SaveAll();

		/**
		 * Discards the current file's unsaved changes and reloads it from disk.
		 **/
		void										RevertRefresh();

		/**
		 * Renames the current file on disk.
		 **/
		void										RenameFile();

		/**
		 * Closes the current file and permanently deletes it from disk.
		 **/
		void										DeleteCurrentFile();

		/**
		 * Offers to save every modified file, as before closing them all.
		 *
		 * \return Returns false if the user cancelled (or a save failed), in which case nothing should be closed.
		 **/
		bool										QueryCloseAll();

		// Performs an Open operation.
		void										Open();

		// Performs an Open operation.
		void										Open( const std::filesystem::path &_pPath );

		// Handles opening a process via the Open Process dialog (returns true if a process was actually opened).
		bool										OpenProcess();

		// Handles opening the current process (opens the Open Process dialog if necessary) (returns true if a process was actually opened).
		bool										OpenCurProcess( uint64_t _ui64StartAddress = UINT64_MAX );

		// Closes the active tab.
		void										CloseTab();

		/**
		 * Offers to save a tab's file if it is modified, as before closing it.
		 *
		 * \param _sIdx The tab.
		 * \return Returns false if the user cancelled (or the save failed), in which case the tab should stay open.
		 **/
		bool										QueryCloseTab( size_t _sIdx );

		/**
		 * Closes a tab without asking anything.
		 *
		 * \param _sIdx The tab.
		 **/
		void										RemoveTab( size_t _sIdx );

		// Close all tabs.
		void										CloseAllTabs();

		// Performs an Undo.
		void										Undo();

		// Performs a Redo.
		void										Redo();

		// Performs a Cut.
		void										Cut();

		// Performs a Copy As.
		void										Copy( MX_COPY_AS _caFormat = MX_CA_BINARY );

		/**
		 * Performs a Paste from the active clipboard into the current editor.
		 **/
		void										Paste();

		/**
		 * Makes a clipboard the active clipboard for Cut, Copy, and Paste.
		 * 
		 * \param _sIdx The clipboard index (0 is the Windows clipboard).
		 **/
		void										SelectClipboard( size_t _sIdx );

		/**
		 * Empties all clipboards (the Windows clipboard only if it holds this application's data).
		 **/
		void										ClearClipboards();

		// Delete the selection.
		void										DeleteSelectedOrCaret() override;

		// Deletes the character prior to the caret (backspace).
		void										DeletePriorToCaret() override;

		// Select all.
		void										SelectAll();

		// Select word.
		void										SelectWord();

		// Select line.
		void										SelectLine();

		// Reverse the selection.
		void										ReverseSelection();

		// Sets the caret to the beginning of the selection.
		void										MarkSelectionStart();

		// Sets the caret to the end of the selection.
		void										MarkSelectionEnd();

		// Moves the selection back.
		void										MoveSelectionBack();

		// Moves the selection forward.
		void										MoveSelectionForward();

		// Toggle column mode.
		void										ToggleColumnSelection();

		// Enlarge font.
		void										EnlargeFont();

		// Ensmall font.
		void										EnsmallFont();

		// Reset font.
		void										ResetFont();

		// Sets the font size back to normal.
		void										ResetFontSize();

		// Shows the Go To dialog.
		void										ShowGoTo( uint64_t _ui64DefaultAddress = UINT64_MAX );

		/**
		 * Shows the Select Range dialog and selects the range entered.
		 **/
		void										ShowSelectRange();

		/**
		 * Shows the Mini Map Options dialog for the current hex editor.
		 **/
		void										ShowMiniMapOptions();

		/**
		 * Shows the Set Starting Address dialog and sets the address shown for the first byte of the current tab.
		 **/
		void										ShowSetStartingAddress();

		/**
		 * Shows the Find dialog, which searches the current tab while it is open.
		 **/
		void										ShowFind();

		/**
		 * Finds the next or previous match of the last search (Find -> Find Next/Find Previous).  With Quick Find Next/Previous,
		 *	the selected bytes are searched for instead.
		 *
		 * \param _bUp Searches backward.
		 **/
		void										FindAgain( bool _bUp );

		/**
		 * Searches the current tab.  Find Next/Previous selects the match; Find All stores the matches in the Find results.
		 *
		 * \param _fpParms The search.
		 * \param _faAction What to do.
		 * \param _swsStatus Receives a description of the result.
		 * \return Returns true if a match was selected (Find Next/Previous) or if the search was done (Find All, even if nothing
		 *	was found).
		 **/
		bool										DoFind( const CHexFind::MX_FIND_PARMS &_fpParms, CFindLayout::MX_FIND_ACTION _faAction, CSecureWString &_swsStatus );

		/**
		 * Gets the results of the Find All searches.
		 *
		 * \return Returns the results.
		 **/
		inline const CFindResults &					FindResults() const { return m_frFindResults; }

		/**
		 * Shows the Hex Editor's Expression Evaluator (Tools -> Expression Evaluator), creating it if needed.  If it is docked
		 *	behind other dockables, its tab is selected.
		 **/
		void										ShowExpEval();

		/**
		 * Informs that a child was removed from a child control (or from this one).  Forgets the Expression Evaluator when it is
		 *	closed.
		 *
		 * \param _pwChild The child that was removed.
		 **/
		virtual void								ChildWasRemoved( const CWidget * _pwChild );

		/**
		 * Reads values for the Expression Evaluator's [address] expressions from the current tab.  Addresses are those shown (they
		 *	include the starting address), and values are read in the byte order of the view.
		 *
		 * \param _ui64Address The address.
		 * \param _ctType The type to read.
		 * \param _uiptrData The CDeusHexMachinaWindow.
		 * \param _peecContainer The expression container.
		 * \param _rResult Receives the value.
		 * \return Returns true if the value was read.
		 **/
		static bool __stdcall						ExpAddressHandler( uint64_t _ui64Address, ee::EE_CAST_TYPES _ctType, uintptr_t _uiptrData,
			ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );

		/**
		 * Shows the Export Hex dialog and writes the current file, process, or selection to a text file in the format chosen.
		 **/
		void										ExportHex();

		/**
		 * Asks for a file of hex text (or Intel HEX, Motorola, Base64, etc.), converts it to bytes, and opens the bytes as a new file.
		 **/
		void										ImportHex();

		/**
		 * Shows the Change Font dialog and applies the font chosen to the Hex Editor (all tabs share it).
		 **/
		void										ShowChangeFont();

		/**
		 * Shows the Windows Properties dialog of the current file (or of the executable of the current process).
		 **/
		void										ShowProperties();

		/**
		 * Turns Read Only mode on or off for the current tab (not available if the data source itself cannot be written).
		 **/
		void										ToggleReadOnly();

		/**
		 * Turns Keep File Time on or off for the current file.
		 **/
		void										ToggleKeepFileTime();

		/**
		 * Gets the path of the file whose properties are shown for a tab: the file, or the executable of the process.
		 *
		 * \param _sIdx The tab.
		 * \param _pPath Receives the path.
		 * \return Returns true if the tab has such a file.
		 **/
		bool										PropertiesPath( size_t _sIdx, std::filesystem::path &_pPath );

		// Shows the Set Line SPacing dialog.
		void										ShowSetLineSpacing();

		// Shows the Set Custom Line Width dialog.
		void										ShowSetCustomLineWidth();

		// Shows the Set Group Width dialog.
		void										ShowSetGroupWidth();

		// Shows the Set Division Lines dialog.
		void										ShowSetDivisionLines();

		// Sets a status-bar item’s text and "warning" status.
		void										SetStatusBarText( const wchar_t * _pwcText, bool _bWarning = false, size_t _sIdx = 0, bool _bUpdateStore = true ) override;

		// Toggles between Overwrite and Insert.
		void										ToggleOverwriteInsert() override { CHexEditorControlHost::ToggleOverwriteInsert(); UpdateStatusBar_InsertOverwrite(); }

		// Update the status-bar Insert/Overwrite part.
		void										UpdateStatusBar_InsertOverwrite();

		/**
		 * Updates the status-bar Clipboard part to show the active clipboard.
		 **/
		void										UpdateStatusBar_Clipboard();

		// Update the status-bar Little-Endian/Big-Endian part.
		void										UpdateStatusBar_Endian();

		// Update the status-bar Character-Set part.
		void										UpdateStatusBar_CharSet() override;

		// Update the status-bar View Type part.
		void										UpdateStatusBar_ViewType();

		// Update the status-bar Size part.
		void										UpdateStatusBar_Size() override;

		// Updates the status-bar selection ranges/current position/value.
		void										UpdateStatusBar_PosValue_StartSize() override;

		/**
		 * Called when the mouse enters a tab.
		 * 
		 * \param _iIdx The tab the mouse began to hover over.
		 **/
		virtual void								TabMouseEnterTab( INT _iIdx ) override;

		/**
		 * Called when the mouse leaves a tab.
		 * 
		 * \param _iIdx INdex of the tab the mouse left.
		 **/
		virtual void								TabMouseLeftTab( INT _iIdx ) override;

		/**
		 * Gets the widget type identifier.
		 * \brief Returns one of LSW_LT_* constants.
		 *
		 * \return Returns LSW_LT_WIDGET in the base class.
		 */
		virtual uint32_t							WidgetType() const { return MX_DEUS_HEX_MACHINA; }

		// Gets the currently active hex-editor control.
		CHexEditorControl *							CurrentEditor() {
			auto ptTab = Tab();
			if ( !ptTab ) { return nullptr; }
			size_t sIdx = size_t( ptTab->GetCurSel() );
			if ( sIdx >= m_vTabs.size() ) { return nullptr; }
			return m_vTabs[sIdx].phecWidget;
		}

		// Called by the main window before the application closes.
		bool										WillClose();

		/**
		 * Gets the file being edited in a tab.
		 *
		 * \param _sIdx The tab.
		 * \return Returns the file, or nullptr if the tab does not exist or does not edit a file (a process).
		 **/
		CHexEditorFile *							FileAt( size_t _sIdx );

		/**
		 * Gets the file being edited in the current tab.
		 *
		 * \return Returns the file, or nullptr if there is no tab or it does not edit a file (a process).
		 **/
		CHexEditorFile *							CurrentFile();

		/**
		 * Gets the index of the current tab.
		 *
		 * \return Returns the index, or SIZE_MAX if there is no tab.
		 **/
		size_t										CurrentTab();
		
		// Prepares to create the window.  Creates the atom if necessary.
		static void									PrepareWindow();


	protected :
		// == Types.
		/** Data for a tab. */
		struct MX_HEX_TAB {
			CHexEditorControl *						phecWidget = nullptr;
			//CHexEditorInterface *					pheiInterface = nullptr;
			std::shared_ptr<CHexEditorInterface>	pheiInterface;
		};


		// == Members.
		// The shared options.
		CHexEditorControl::MX_STYLE					m_sOptions[CHexEditorControl::MX_EA_TOTAL];
		// The control foreground colors.
		MX_HEX_EDITOR_COLORS						m_hecFgColors;
		// The control background colors.
		MX_HEX_EDITOR_COLORS						m_hecBgColors;
		// Fixed-width view font.
		CHexEditorControl::MX_FONT_SET				m_fsFixedRowFont;
		// Variable-width view font.
		CHexEditorControl::MX_FONT_SET				m_fsTextViewFont;
		// Image list.
		CImageList									m_iImages;
		// Images.
		CBitmap										m_bBitmaps[MX_I_TOTAL];
		// Image mapping.
		INT											m_iImageMap[MX_I_TOTAL];
		// The hex-editor controls.
		std::vector<MX_HEX_TAB>						m_vTabs;
		// The tast text put into status-bat index 0.
		CSecureWString								m_swsLastStatusText;
		// Whether the last index-0 status-bar text was a warning or not.
		bool										m_bLastStatusTextIsWarning = false;

		// The MHS object where we can access the current process and our options.
		CWindowMemHack *							m_pmhMemHack = nullptr;
		/** The kind of file the toolbar's New button creates: the last one chosen. */
		MX_NEW_FILE									m_nfLastNew = MX_NF_HEX;
		/** If true, the Select Range dialog last took an end address rather than a size (it starts that way next time). */
		bool										m_bSelectRangeEnd = false;
		/** The Export Hex settings last used (the path is not kept). */
		CExportHexLayout::MX_EXPORT_HEX_PARMS		m_ehpLastExport;
		/** The filter last chosen in Import Hex. */
		DWORD										m_dwLastImportFilter = 1;
		/** The Find settings last used. */
		CHexFind::MX_FIND_PARMS						m_fpFindParms;
		/** What was searched for (most recent first). */
		std::vector<CSecureWString>					m_vFindHistory;
		/** The results of Find All. */
		CFindResults								m_frFindResults;
		/** The data source the Find dialog's locked range belongs to. */
		const CHexEditorInterface *					m_pheiFindLock = nullptr;
		/** Where Incremental Search searches from (the caret when the Find dialog was opened). */
		uint64_t									m_ui64FindAnchor = 0;
		/** The Hex Editor's Expression Evaluator (nullptr until it is shown, and after it is closed). */
		CExpEvalWindow *							m_peeExpEval = nullptr;
		/** The number of untitled files created (for naming them). */
		uint32_t									m_ui32Untitled = 0;

		// The main window class.
		static ATOM									m_aAtom;


		// == Functions.
		/**
		 * The WM_INITMENUPOPUP handler.
		 *
		 * \param _hMenu A handle to the drop-down menu or submenu.
		 * \param _wPos The zero-based relative position of the menu item that opens the drop-down menu or submenu.
		 * \param _bIsWindowMenu Indicates whether the drop-down menu is the window menu. If the menu is the window menu, this parameter is TRUE; otherwise, it is FALSE.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							InitMenuPopup( HMENU _hMenu, WORD /*_wPos*/, BOOL _bIsWindowMenu );

		/**
		 * Handles TBN_GETINFOTIPW: supplies the tooltips of the toolbar buttons.  The text is decrypted only when it is needed.
		 *
		 * \param _pgitTip The notification data.  iItem is the command ID of the button.
		 * \return Returns LSW_H_HANDLED if text was supplied.
		 */
		virtual LSW_HANDLED							TbnGetInfoTip( LPNMTBGETINFOTIPW _pgitTip ) override;
		
		// Recalc all but the given control.
		void										RecalcAllBut( const CHexEditorControl * _phecSkipMe );

		// Adds a tab.
		bool										AddTab( const std::shared_ptr<CHexEditorInterface> &_spInterface );

		/**
		 * Makes a tab edit a file freshly opened from disk in place of what it edits now (Revert/Refresh, or after saving a read-only
		 *	file under a new name).  The tab is closed if the file cannot be opened.
		 *
		 * \param _sIdx The tab.
		 * \param _pPath The file to open.
		 * \return Returns true if the file was opened.
		 **/
		bool										ReopenTab( size_t _sIdx, const std::filesystem::path &_pPath );

		/**
		 * Updates a tab's text from its data source.
		 *
		 * \param _sIdx The tab.
		 **/
		void										UpdateTabText( size_t _sIdx );

		/**
		 * Enables and disables the toolbar buttons for the current tab (Save, Cut/Copy with a selection, Undo/Redo, etc.)
		 **/
		void										UpdateToolBar();

		/**
		 * Determines whether the current file can be saved under its own name (it is a file, not read-only).
		 *
		 * \return Returns true if Save is available.
		 **/
		bool										CanSave();

		/**
		 * Determines whether any open file can be saved (Save All).
		 *
		 * \return Returns true if at least one file is not read-only.
		 **/
		bool										CanSaveAll();

		/**
		 * Asks for the path of a file to write.
		 *
		 * \param _pwcTitle The title of the dialog.
		 * \param _pSuggested The path (or just the name) to suggest.
		 * \param _bOverwritePrompt If true, asks before an existing file is chosen.
		 * \param _pChosen Receives the chosen path.
		 * \return Returns true if a path was chosen.
		 **/
		bool										AskSavePath( LPCWSTR _pwcTitle, const std::filesystem::path &_pSuggested, bool _bOverwritePrompt, std::filesystem::path &_pChosen );

		/**
		 * Determines whether a path is a file open in any tab (which must not be overwritten except by saving that tab).
		 *
		 * \param _pPath The path.
		 * \param _sSkip A tab to ignore, or SIZE_MAX.
		 * \return Returns true if another tab edits the file.
		 **/
		bool										IsOpenInTab( const std::filesystem::path &_pPath, size_t _sSkip );

		/**
		 * Writes bytes read from a data source to a file (through a temporary file that then replaces it).
		 *
		 * \param _pheiSource The data source.
		 * \param _fGather Gets the ranges to write, a group at a time (filling an empty vector after the last group), in order.  Returns
		 *	false on a memory failure.
		 * \param _pDst The file to write.
		 * \param _dwError Receives the Windows error code on failure.
		 * \return Returns true if the file was written.
		 **/
		static bool									WriteRangesTo( CHexEditorInterface * _pheiSource, const std::function<bool ( std::vector<MX_SEL_RANGE> &, uint64_t )> &_fGather,
			const std::filesystem::path &_pDst, DWORD &_dwError );

		/**
		 * Shows an error about a file.
		 *
		 * \param _swsMsg What failed.
		 * \param _pPath The file.
		 * \param _dwError The Windows error code.
		 **/
		void										FileError( const CSecureWString &_swsMsg, const std::filesystem::path &_pPath, DWORD _dwError );

		// Verification function for the Go To dialog.
		static bool									GoTo_Verify( void * _pvThis, const mx::CNumberInputLayout::MX_NUMBER_DIALOG_PARMS &_ndpParms );

		/**
		 * Checks the address entered in the Set Starting Address dialog.
		 *
		 * \param _pvThis The CDeusHexMachinaWindow.
		 * \param _ndpParms The dialog's parameters, holding the result.
		 * \return Returns true if the result is an unsigned integer.
		 **/
		static bool									StartingAddress_Verify( void * _pvThis, const mx::CNumberInputLayout::MX_NUMBER_DIALOG_PARMS &_ndpParms );

		// Verification of a value fitting into a constant range.
		template <uint64_t _ui64Max = 0xFFFFFFFF>
		static bool									ConstantRange_Verify( void * _pvThis, const mx::CNumberInputLayout::MX_NUMBER_DIALOG_PARMS &_ndpParms ) {
			auto pdhmwThis = static_cast<CDeusHexMachinaWindow *>(_pvThis);
			auto rCastResult = ee::CExpEvalContainer::ConvertResult( _ndpParms.rResult, ee::EE_NC_UNSIGNED );
			if ( rCastResult.ncType != ee::EE_NC_UNSIGNED ) {
				auto swsTitle = _DEC_WS_9607671B_Invalid_Expression;
				auto swsError = _DEC_WS_16D063D0_The_given_expression_must_evaluate_to_an_unsigned_integer_;
				lsw::CBase::MessageBoxError( _ndpParms.pwThis->Wnd(), swsError.c_str(), swsTitle.c_str() );
				return false;
			}
			auto phecControl = pdhmwThis->CurrentEditor();
			if ( !phecControl ) { return true; }	// Internal error but unrecoverable here.
			if constexpr ( _ui64Max != UINT64_MAX ) {
				if ( rCastResult.u.ui64Val > _ui64Max ) {
					auto swsTitle = _DEC_WS_9607671B_Invalid_Expression;
					auto swsError = _DEC_WS_D08434A5_Address_out_of_range_;
					lsw::CBase::MessageBoxError( _ndpParms.pwThis->Wnd(), swsError.c_str(), swsTitle.c_str() );
					return false;
				}
			}
			return true;
		}

	private :
		typedef CDeusHexMachinaLayout				Layout;
	};


}	// namespace mx

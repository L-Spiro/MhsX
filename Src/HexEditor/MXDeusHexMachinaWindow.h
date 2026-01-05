#pragma once

#include "../MXMhsX.h"
#include "../Layouts/MXDeuHexMachinaWindowLayout.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Strings/MXStringDecoder.h"
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

using namespace lsw;

namespace mx {
	
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
		// Gets the base tab control.
		CTab *										GetTab() const;

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

		// Gets the multi-splitter.
		CMultiSplitter *							MultiSplitter() {
			return static_cast<CMultiSplitter *>(FindChild( Layout::MX_W_MAIN_SPLITTER ));
		}

		// Gets the main tab.
		CTab *										Tab() {
			return static_cast<CTab *>(FindChild( Layout::MX_W_TABS ));
		}

		// Performs a Save As operation.
		void										SaveAs();

		// Performs a Save operation.
		void										Save();

		// Performs an Open operation.
		void										Open();

		// Performs an Open operation.
		void										Open( const std::filesystem::path &_pPath );

		// Handles opening a process via the Open Process dialog (returns true if a process was actually opened).
		bool										OpenProcess();

		// Handles opening the current process (opens the Open Process dialog if necessary) (returns true if a process was actually opened).
		bool										OpenCurProcess( uint64_t _ui64StartAddress = 0 );

		// Closes the active tab.
		void										CloseTab();

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

		// Sets a status-bar item’s text and "warning" status.
		void										SetStatusBarText( const wchar_t * _pwcText, bool _bWarning = false, size_t _sIdx = 0, bool _bUpdateStore = true ) override;

		// Toggles between Overwrite and Insert.
		void										ToggleOverwriteInsert() override { CHexEditorControlHost::ToggleOverwriteInsert(); UpdateStatusBar_InsertOverwrite(); }

		// Update the status-bar Insert/Overwrite part.
		void										UpdateStatusBar_InsertOverwrite();

		// Update the status-bar Little-Endian/Big-Endian part.
		void										UpdateStatusBar_Endian();

		// Update the status-bar Character-Set part.
		void										UpdateStatusBar_CharSet() override;

		// Update the status-bar View Type part.
		void										UpdateStatusBar_ViewType();

		// Update the status-bar Size part.
		void										UpdateStatusBar_Size();

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
		
		// Prepares to create the window.  Creates the atom if necessary.
		static void									PrepareWindow();


	protected :
		// == Types.
		/** Data for a tab. */
		struct MX_HEX_TAB {
			CHexEditorControl *						phecWidget = nullptr;
			CHexEditorInterface *					pheiInterface = nullptr;
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
		/** Overwrite Mode. */
		bool										m_bOverwrite = true;
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
		
		// Recalc all but the given control.
		void										RecalcAllBut( const CHexEditorControl * _phecSkipMe );

		// Adds a tab.
		bool										AddTab( CHexEditorInterface * _pheiInterface );

	private :
		typedef CDeusHexMachinaLayout				Layout;
	};


}	// namespace mx

#pragma once

#include "../Converter/MXConverterWindow.h"
#include "../ExpEval/MXExpEvalWindow.h"
#include "../FloatingPointStudio/MXFloatingPointStudioWindow.h"
#include "../FoundAddresses/MXFoundAddressesWindow.h"
#include "../HexEditor/MXDeusHexMachinaWindow.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../MemHack/MXWindowMemHack.h"
#include "../Options/MXOptionsWindow.h"
#include "../PE/MXPeWorksWindow.h"
#include "../Search/MXSearcher.h"
#include "../StringTheory/MXStringTheoryWindow.h"

#include <MainWindow/LSWMainWindow.h>
#include <MultiSplitter/LSWMultiSplitter.h>
#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <ListView/LSWListView.h>

using namespace lsw;

namespace lsw {
	class CStatusBar;
}

namespace mx {
	
	class CMhsMainWindow : public lsw::CMainWindow {
	public :
		CMhsMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64MemHack = 0 );
		~CMhsMainWindow();


		// == Enumerations.
		// Recent Processes history size.
		enum MX_RECENT {
			MX_R_RECENT_PROCESSES_SIZE		= 25,
		};


		// Images.
		enum MX_IMAGES {
			MX_I_OPENPROCESS,
			MX_I_OPENFORDEBUG,

			MX_I_ADDENTRY,
			MX_I_OPENENTRY,
			MX_I_SAVEENTRY,
			MX_I_SAVEENTRYAS,

			MX_I_DELETEENTRY,
			MX_I_DELETEALL,
			MX_I_LOCK,
			MX_I_UNLOCK,
			MX_I_LOCKONCE,

			MX_I_EDIT,

			MX_I_OPTIONS,

			MX_I_TOTAL,
		};

		// Custom messages.
		enum MX_CUSTOM_MESSAGE : uint16_t {
			MX_CM_UPDATE_FA					= LSW_USER + 0x0000,
			MX_CM_HOTKEY,
			MX_CM_HOOKKEY,
			MX_CM_DETATCHED,
			MX_CM_UPDATE_HEX_SIBLINGS,
		};

		// Our hotkeys.  Ranging from 100-200.
		enum MX_WINDOW_HOTKEYS : uint32_t {
			MX_WH_SHOW_OPTIONS				= CHotkeyManBase::MX_F_USER + 100,
			MX_WH_SHOW_FOUND_ADDRESSES,
			MX_WH_SHOW_EXPRESSION_EVALUATOR,
			MX_WH_SHOW_CONVERTER,
			MX_WH_SHOW_PE_WORKS,
			MX_WH_SHOW_STRING_THEORY,
			MX_WH_SHOW_FLOATING_POINT_STUDIO,
			MX_WH_LOCK_SELECTED,
			MX_WH_UNLOCK_SELECTED,
			MX_WH_UNLOCK_ALL,
			MX_WH_SHOW_EDIT,
			MX_WH_SHOW_ADD_ENTRY,
		};

		// Menu.
		enum MX_MENU {
			MX_M_CONTEXT_MENU				= 0x4100
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		// WM_COMMAND from menu.
		virtual LSW_HANDLED					MenuCommand( WORD _wId );

		// WM_ERASEBKGND.
		virtual LSW_HANDLED					EraseBkgnd( HDC _hDc );

		// WM_DESTROY.
		virtual LSW_HANDLED					Destroy();

		// WM_NCDESTROY.
		virtual LSW_HANDLED					NcDestroy();

		// WM_KEYDOWN
		virtual LSW_HANDLED					KeyDown( UINT _uiKeyCode, UINT _uiFlags );

		// WM_KEYUP
		virtual LSW_HANDLED					KeyUp( UINT _uiKeyCode, UINT _uiFlags );

		// WM_SYSKEYDOWN
		virtual LSW_HANDLED					SysKeyDown( UINT _uiKeyCode, UINT _uiFlags );

		// WM_SYSKEYUP
		virtual LSW_HANDLED					SysKeyUp( UINT _uiKeyCode, UINT _uiFlags );

		// WM_HOTKEY.
		virtual LSW_HANDLED					Hotkey( INT _iIdentifier, INT _iVirtualKey, INT _iMod );

		// Gets the status bar.
		CStatusBar *						StatusBar();

		// Gets the status bar.
		const CStatusBar *					StatusBar() const;

		// Gets the list view.
		CTreeListView *						MainTreeView();

		// Gets the multi-splitter.
		CMultiSplitter *					MultiSplitter();

		// Gets the multi-splitter.
		const CMultiSplitter *				MultiSplitter() const;

		// Gets the Expression Evaluator.
		CExpEvalWindow *					ExpressionEvaluator() { return m_peeExpEval; }

		// Gets the Found Addresses.
		CFoundAddressesWindow *				FoundAddresses() { return m_pfaFoundAddresses; }

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const LSW_RECT				VirtualClientRect( const CWidget * pwChild ) const;

		// WM_USER/custom messages.
		virtual LSW_HANDLED					CustomPrivateMsg( UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

		// Shows the Found Address dockable.
		void								ShowFoundAddress();

		// Shows the Expression Evaluator dockable.
		void								ShowExpEval();

		// Shows the Converter dockable.
		void								ShowConverter();

		// Shows the options dialog with a page to be selected by default.
		void								ShowOptions( int32_t _i32Page );

		// Deletes the selected Found Addresses.
		void								DeleteSelectedOrCaret();

		// Deletes all Found Addresses.
		void								DeleteAll();

		// Gets the last options page shown.
		int32_t								LastOptionsShown() const { return m_i32LastOptionsPage; }

		// Gets the search page, keeping either page if one was already selected, otherwise showing COptionsWindow::MX_P_GENERAL_SEARCH.
		int32_t								SearchOptionsPage() const {
			if ( LastOptionsShown() == COptionsWindow::MX_P_GENERAL_SEARCH || LastOptionsShown() == COptionsWindow::MX_P_SEARCH_EX ) { return LastOptionsShown(); }
			return COptionsWindow::MX_P_GENERAL_SEARCH;
		}

		// Takes the last option page and returns either the last option page if it is one of the search options or one of the search options.
		int32_t								SearchOptionsPageClamp() const {
			if ( LastOptionsShown() == COptionsWindow::MX_P_GENERAL_SEARCH || LastOptionsShown() == COptionsWindow::MX_P_SEARCH_EX ) { return LastOptionsShown(); }
			return COptionsWindow::MX_P_GENERAL_SEARCH;
		}

		// Shows the PE Works window.  If _pwcFile is not nullptr, it points to a default file to
		//	show in a new tab after opening the window.  If the window already exists, it is brought
		//	to the front and the tab with the existing provided file (if any) is activated, or a new
		//	tab with the given file (if any) is created.
		void								ShowPeWorks( const wchar_t * _pwcFile = nullptr );

		// Shows the String Theory window.
		void								ShowStringTheory( const wchar_t * _pwcDefaultText = nullptr );

		// Shows the Floating-Point Studio window with a given default value or std::nan() for no default value.
		void								ShowFloatingPointStudio( double _dDefault,
			bool _bHasSign = true, bool _bHasExplicitBit = true,
			uint16_t _uiExpBits = EE_FLOATX_DBL_EXP_BITS, uint16_t _uiManBits = EE_FLOATX_DBL_MAN_BITS );

		// Shows the Deus Hex Machina window.
		void								ShowDeusHexMachina();

		// Shows a new search.
		uint32_t							ShowNewDataTypeSearch();

		// Shows a new pointer search.
		uint32_t							ShowNewPointerSearch();

		// Shows a new string search.
		uint32_t							ShowNewStringSearch();

		// Shows a new expression search.
		uint32_t							ShowNewExpressionSearch();

		// Shows the last search dialog.
		uint32_t							ShowLastSearch() {
			switch ( m_stLastSearchType ) {
				case MX_ST_DATA_TYPE : { return ShowNewDataTypeSearch(); }
				case MX_ST_POINTER : { return ShowNewPointerSearch(); }
				case MX_ST_STRING : { return ShowNewStringSearch(); }
				case MX_ST_EXPRESSION : { return ShowNewExpressionSearch(); }
			}
			return 0;
		}

		// Shows the standard subsearch dialog.
		uint32_t							ShowStandardSubsearch();

		// Shows the Edit dialog.
		uint32_t							ShowEdit();

		// Locks the selected items.
		void								LockSelected();

		// Unlocks the selected items.
		void								UnlockSelected();

		// Unlocks all items.
		void								UnlockAll();

		// Writes the lock value once.
		void								WriteOnce();

		// Move selected items up.
		void								MoveUp();

		// Move selected items down.
		void								MoveDown();

		// Sets the source address for the Move Address dialog.
		void								SetMoveAddressSource();

		// Move selected addresses.
		void								MoveSelectedAddresses();

		// Copy the selected addresses to the clipboard.
		void								CopySelectedAddresses();

		// Handles opening a process via the Open Process dialog (returns true if a process was actually opened).
		bool								OpenProcess();

		// Opens a process given its path.
		bool								OpenProcessByPath( const CSecureWString &_swsPath );

		// Updates the Open Recent menu.
		void								UpdateOpenRecent();

		// Starts a search on the current thread (when activated via script or plug-in) or on a new thread (normal).
		bool								Search( CSearcher::MX_SEARCH_PARMS * _pspParms, CWidget * _pwParent, bool _bUseSepThread = true, CWidget * _pwProgress = nullptr );

		// Starts a subsearch on the current thread (when activated via script or plug-in) or on a new thread (normal).
		bool								Subsearch( CSearcher::MX_SUBSEARCH_PARMS * _pspParms, CWidget * _pwParent, bool _bUseSepThread = true, CWidget * _pwProgress = nullptr );

		// Gets the MemHack object.
		CWindowMemHack *					MemHack() { return m_pmhMemHack; }

		// Gets the stored "Move From" address.
		UINT64								MoveFromAddr() const { return m_ui64MoveAddressesSourceAddr; }

		// Saves all program settings.
		virtual bool						SaveSettings( const std::wstring &_wsPath, bool _bAsJson ) const;

		// Loads all program settings.
		virtual bool						LoadSettings( const std::wstring &_wsPath, bool _bAsJson );

		// Translate a child's tooltip text.
		virtual std::wstring				TranslateTooltip( const std::string &_sText ) { 
			return CStringDecoder::DecodeToWString( _sText.c_str(), _sText.size() );
		}

		// Update timer speed.
		void								UpdateTimer();

		// WM_TIMER.
		CWidget::LSW_HANDLED				Timer( UINT_PTR _uiptrId, TIMERPROC _tpProc );

		// WM_NOTIFY->NM_DBLCLK on the main list to edit the currently selected values.
		virtual LSW_HANDLED					DblClk( const LPNMITEMACTIVATE _phHdr );

		// WM_NOTIFY->NM_DBLCLK for the owning window if the child either could not be resolved or returned LSW_HANDLED::LSW_H_CONTINUE.
		//virtual LSW_HANDLED					DblClk( const LPNMITEMACTIVATE /*_phHdr*/, WORD /*_wControlId*/, CWidget * /*_pwWidget*/ );


	protected :
		// == Enumerations.
		// The last search types.
		enum MX_SEARCH_TYPES {
			MX_ST_DATA_TYPE,
			MX_ST_POINTER,
			MX_ST_STRING,
			MX_ST_EXPRESSION,
		};

		// The last subsearch types.
		enum MX_SUBSEARCH_TYPES {
			MX_SST_STANDARD,
		};


		// == Members.
		// The Move Address source address.
		UINT64								m_ui64MoveAddressesSourceAddr = 0;

		// Image list.
		CImageList							m_iImages;

		// Images.
		CBitmap								m_bBitmaps[MX_I_TOTAL];

		// Image mapping.
		INT									m_iImageMap[MX_I_TOTAL];

		// The MHS object.
		CWindowMemHack *					m_pmhMemHack;

		// The Found Address dockable.
		CFoundAddressesWindow *				m_pfaFoundAddresses;

		// The Expression Evaluator dockable.
		CExpEvalWindow *					m_peeExpEval = nullptr;

		// The PE Works window.
		CPeWorksWindow *					m_ppwPeWorks = nullptr;

		// The String Theory window.
		CStringTheoryWindow	*				m_pstwStringTheory = nullptr;

		// The Floating-Point Studio window.
		CFloatingPointStudioWindow *		m_pfpsFloatingPointStudioWindow = nullptr;

		// The Deus Hex Machina window.
		CDeusHexMachinaWindow *				m_pdhmwDeusHexMachinaWindow = nullptr;

		// The Converter window.
		CConverterWindow *					m_cwConverter;

		// The last search that was performed.
		MX_SEARCH_TYPES						m_stLastSearchType;

		// The last subsearch that was performed.
		MX_SUBSEARCH_TYPES					m_sstLastSubsearchType;

		// The last options page shown.
		int32_t								m_i32LastOptionsPage;

		// List timer.
		CUtilities::MX_TIMER				m_tUpdateListTimer;

		// Locks timer.
		CUtilities::MX_TIMER				m_tUpdateLocksTimer;

		// Last context-menu hit-test;
		int									m_iContextHitTest = -1;


		// == Functions.
		// Informs that a child was removed from a child control (IE this control's child had a child control removed from it).
		// Is also called on the control from which a child was removed for convenience.
		virtual void						ChildWasRemoved( const CWidget * _pwChild );

		// Update the "Window" checks.
		void								UpdateWindowChecks();

		// Updates the Found Address window text.
		void								UpdateFoundAddressWindowText();

		// Do polling-based hotkeys.
		void								PollHotkeys( INT _iId );

		// Do hook-based hotkeys.
		void								PollHookKeys( INT _iId );

		// WM_CONTEXTMENU.
		virtual LSW_HANDLED					ContextMenu( CWidget * _pwControl, INT _iX, INT _iY );

		
		// Performs a Save As operation.
		void								SaveAs();

		// Performs a Save Selected As operation.
		void								SaveSelectedAs();

		// Performs a Save operation.
		void								Save();

		// Performs an Open operation.
		void								Open();

		// Add Entry.
		void								AddEntry();

		/**
		 * Called when a CTreeListView wants text for an item.  Can be used to produce real-time or dynamically changing text for items in the tree.
		 * 
		 * \param _pwSrc A pointer to the widget calling the function.
		 * \param _iItem Index of the item whose text is being drawn.
		 * \param _iSubItem Index of the column for which to retreive text.
		 * \param _lpParam The parameter associated with the item.
		 * \param _wsOptionalBuffer An optional buffer for storing text to make it easier to return a persistent std::wstring pointer.  Not necessary if you already have an std::wstring ready to go.
		 * \return Return a pointer to a wide-string result containing the text to display for the given item.  If it is convenient, _wsOptionalBuffer can be used to store the text and &_wsOptionalBuffer returned, otherwise you can return a pointer to an existing std::wstring.
		 *	Return nullptr to use the item's text set by SetItemText().
		 **/
		virtual std::wstring *				TreeListView_ItemText( CWidget * _pwSrc, int _iItem, int _iSubItem, LPARAM _lpParam, std::wstring &_wsOptionalBuffer );

		/**
		 * Hotkey handler for opening the options window.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_OpenOptions( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );
		
		/**
		 * Hotkey handler for showing the Found Addresses window.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_ShowFoundAddresses( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );

		/**
		 * Hotkey handler for showing the Expression Evaluator window.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_ShowExpressionEvaluator( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );

		/**
		 * Hotkey handler for showing the Converter window.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_ShowConverter( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );

		/**
		 * Hotkey handler for showing the PE Works window.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_ShowPeWorks( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );

		/**
		 * Hotkey handler for showing the String Theory window.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_ShowStringTheory( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );

		/**
		 * Hotkey handler for showing the Floating-Point Studio window.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_ShowFloatingPointStudio( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );

		/**
		 * Hotkey handler for showing the Add Entry dialog.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_ShowAddFoundAddress( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );

		/**
		 * Hotkey handler for showing the Modify Entry dialog.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_ShowEdit( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );

		/**
		 * Hotkey handler for locking selected entries.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_LockSelected( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );

		/**
		 * Hotkey handler for unlocking selected entries.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_UnLockSelected( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );

		/**
		 * Hotkey handler for unlocking all entries.
		 *
		 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
		 * \param _uiParm1 Unused.
		 * \param _uiParm2 Unused.
		 * \param _uiParm3 Unused.
		 */
		static void __stdcall				Hotkey_UnLockAll( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ );

	private :
		typedef CMainWindowLayout			Layout;
	};

}	// namespace mx

#include "MXMainWindowLayout.h"
#include "../MemHack/MXMemHack.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <Layout/LSWLayoutManager.h>


namespace mx {

	// == Members.
	LSW_WIDGET_LAYOUT CMainWindowLayout::m_wlMainWindow[] = {
		{
			MX_MAIN_WINDOW,							// ltType
			MX_MWI_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			64,										// iLeft
			64,										// iTop
			MX_MAIN_WINDOW_W,						// dwWidth
			MX_MAIN_WINDOW_H,						// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_BA5DABD6_L___Spiro__MHS__X ),								// pwcText
			_LEN_BA5DABD6,							// sTextLen
			MX_MWI_NONE,							// dwParentId
		},
		{
			LSW_LT_STATUSBAR,						// ltType
			MX_MWI_STATUSBAR,						// wId
			STATUSCLASSNAMEW,						// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			0,										// dwWidth
			0,										// dwHeight
			WS_VISIBLE | WS_CHILD | SBARS_SIZEGRIP,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MWI_MAINWINDOW,						// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},

		{
			LSW_LT_REBAR,							// ltType
			MX_MWI_REBAR0,							// wId
			REBARCLASSNAMEW,						// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			m_wlMainWindow[0].dwWidth,				// dwWidth
			150,									// dwHeight
			//WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TBSTYLE_TRANSPARENT | TBSTYLE_REGISTERDROP | TBSTYLE_LIST | TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_NOMOVEY,
			WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN |
				RBS_VARHEIGHT | RBS_BANDBORDERS,	// dwStyle
			0,//WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW,// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MWI_MAINWINDOW,						// dwParentId

			MX_PARENT_CLEFT,//MX_LOCK_LEFT,			// pcLeftSizeExp
			MX_PARENT_CRIGHT,						// pcRightSizeExp
			MX_PARENT_CTOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_TOOLBAR,							// ltType
			MX_MWI_TOOLBAR0,						// wId
			TOOLBARCLASSNAMEW,						// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			0,										// dwWidth
			0,										// dwHeight
			WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | WS_VISIBLE | CCS_ADJUSTABLE,		// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MWI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_MULTISPLITTER,					// ltType
			MX_MWI_SPLITTER,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			255,									// dwWidth
			110,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE,			// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MWI_MAINWINDOW,						// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		//{
		//	LSW_LT_SPLITTER,						// ltType
		//	MX_MWI_SPLITTER,						// wId
		//	nullptr,								// lpwcClass
		//	TRUE,									// bEnabled
		//	FALSE,									// bActive
		//	0,										// iLeft
		//	0,										// iTop
		//	255,									// dwWidth
		//	110,									// dwHeight
		//	WS_CHILDWINDOW | WS_VISIBLE,			// dwStyle
		//	0,										// dwStyleEx
		//	nullptr,								// pwcText
		//	0,										// sTextLen
		//	MX_MWI_MAINWINDOW,						// dwParentId

		//	MX_PARENT_VCLEFT,						// pcLeftSizeExp
		//	MX_PARENT_VCRIGHT,						// pcRightSizeExp
		//	MX_PARENT_VCTOP,						// pcTopSizeExp
		//	MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
		//	nullptr, 0,								// pcWidthSizeExp
		//	nullptr, 0,								// pcHeightSizeExp
		//},
		{
			LSW_LT_TREELISTVIEW,					// ltType
			MX_MWI_STOREDADDRESSES,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			255,									// dwWidth
			110,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT,		// dwStyle
			WS_EX_CLIENTEDGE | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MWI_SPLITTER,						// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
	};

	// Menu bar items for the main window.
	LSW_MENU_ITEM CMainWindowLayout::m_miMenuBar[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_MWMI_FILE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_854A6B8B__File, _LEN_854A6B8B ) },
		{ FALSE,		MX_MWMI_SEARCH,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_339FACEA__Search, _LEN_339FACEA ) },
		{ FALSE,		MX_MWMI_TOOLS,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_8D2E41E6__Tools, _LEN_8D2E41E6 ) },
		{ FALSE,		MX_MWMI_WINDOW,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0C8B8E90__Window, _LEN_0C8B8E90 ) },
		{ FALSE,		MX_MWMI_HELP,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_01520137__Help, _LEN_01520137 ) },
	};

	// File menu.
	LSW_MENU_ITEM CMainWindowLayout::m_miFileMenu[] {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_MWMI_OPENPROCESS,		FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_364A9A57_Open__Process, _LEN_364A9A57 ) },
		{ FALSE,		MX_MWMI_OPENRECENT,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EA79B404_Open__Recent, _LEN_EA79B404 ) },
		{ FALSE,		MX_MWMI_OPENFORDEBUG,		FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_33B4BD68_Open_for__Debug, _LEN_33B4BD68 ) },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_OPENSAVEFILE,		FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_610CBCA6__Open_Save_File, _LEN_610CBCA6 ) },
		{ FALSE,		MX_MWMI_SAVE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5CB36745__Save, _LEN_5CB36745 ) },
		{ FALSE,		MX_MWMI_SAVEAS,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_A37FB152_Save__As, _LEN_A37FB152 ) },
		{ FALSE,		MX_MWMI_SAVE_SELECTED_AS,	FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_05CC6AD3_Save_Se_lected_As, _LEN_05CC6AD3 ) },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_EXIT,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6ADD8CE5_E_xit, _LEN_6ADD8CE5 ) },
	};

	// Search menu.
	LSW_MENU_ITEM CMainWindowLayout::m_miSearchMenu[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_MWMI_DATATYPE,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_807A9EFA__Data_Type_Search, _LEN_807A9EFA ) },
		{ FALSE,		MX_MWMI_POINTER,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0E3DF790__Pointer_Search, _LEN_0E3DF790 ) },
		{ FALSE,		MX_MWMI_STRING,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_7F01DA2F__String_Search, _LEN_7F01DA2F ) },
		{ FALSE,		MX_MWMI_GROUP,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_05F169BE__Group_Search, _LEN_05F169BE ) },
		{ FALSE,		MX_MWMI_EXPRESSION,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_BD4358C1__Expression_Search, _LEN_BD4358C1 ) },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_SUB,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_BDE91628_S_ubsearch, _LEN_BDE91628 ) },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_ADDENTRY,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_ED5B72E3__Insert_Address, _LEN_ED5B72E3 ) },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_SEARCHOPTIONS,		FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6F5731F2_Search__Options, _LEN_6F5731F2 ) },
	};

	// Options menu.
	LSW_MENU_ITEM CMainWindowLayout::m_miOptionsMenu[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_MWMI_PEWORKS,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_1A689CBC_PE__Works, _LEN_1A689CBC ) },
		{ FALSE,		MX_MWMI_STRINGTHEORY,		FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_38618AB0__String_Theory, _LEN_38618AB0 ) },
		{ FALSE,		MX_MWMI_FLOATINGPOINTSTUDIO,FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_904CFF9B__Floating_Point_Studio, _LEN_904CFF9B ) },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_DEUS_HEX_MACHINA,	FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EAEFED39_Deus__Hex_Machina, _LEN_EAEFED39 ) },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_OPTIONS,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D8DFA8DD__Options, _LEN_D8DFA8DD ) },
	};

	// Windows menu.
	LSW_MENU_ITEM CMainWindowLayout::m_miWindowsMenu[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_MWMI_SHOW_EXPEVAL,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_7B97062A__Expression_Evaluator, _LEN_7B97062A ) },
		{ FALSE,		MX_MWMI_SHOWFOUNDADDR,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5887B86C__Found_Addresses, _LEN_5887B86C ) },
		{ FALSE,		MX_MWMI_SHOW_CONVERTER,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2C729004__Converter, _LEN_2C729004 ) },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_SHOW_ALL,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_7D8DE2AF_Show__All, _LEN_7D8DE2AF ) },
	};

	// Menus.
	LSW_MENU_LAYOUT CMainWindowLayout::m_miMenus[] = {
		{
			MX_MWMI_MENU_BAR,
			0,
			0,
			std::size( m_miMenuBar ),
			m_miMenuBar
		},
		{
			MX_MWMI_MENU_FILE,
			MX_MWMI_MENU_BAR,
			MX_MWMI_FILE,
			std::size( m_miFileMenu ),
			m_miFileMenu
		},
		{
			MX_MWMI_MENU_SEARCH,
			MX_MWMI_MENU_BAR,
			MX_MWMI_SEARCH,
			std::size( m_miSearchMenu ),
			m_miSearchMenu
		},
		{
			MX_MWMI_MENU_TOOLS,
			MX_MWMI_MENU_BAR,
			MX_MWMI_TOOLS,
			std::size( m_miOptionsMenu ),
			m_miOptionsMenu
		},
		{
			MX_MWMI_MENU_WINDOW,
			MX_MWMI_MENU_BAR,
			MX_MWMI_WINDOW,
			std::size( m_miWindowsMenu ),
			m_miWindowsMenu
		},
	};

	// The class for the main window.
	ATOM CMainWindowLayout::m_aMainClass = 0;

	// The main window.
	CWidget * CMainWindowLayout::m_pwMainWindow = nullptr;

	// == Functions.
	// Creates the main window.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc., and registers the main window class.
	CWidget * CMainWindowLayout::CreateMainWindow( CMemHack * _pmhMemHack ) {
		if ( !m_aMainClass ) {
			// Register the window classes we need.
			WCHAR szTemp[5];
			CUtilities::RandomString( szTemp, std::size( szTemp ) );
			lsw::CWndClassEx wceEx( lsw::CWidget::WindowProc, szTemp );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1) );
			m_aMainClass = lsw::CBase::RegisterClassExW( wceEx.Obj() );	
		}

		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		CLayoutManager::UnencryptLayouts( m_wlMainWindow, std::size( m_wlMainWindow ),
			vLayouts,
			sStringsW,
			sStrings );
		std::vector<LSW_MENU_LAYOUT> vMenus;
		std::vector<std::vector<LSW_MENU_ITEM> *> vMenuItems;
		std::vector<std::vector<CSecureWString> *> vMenuStrings;
		CLayoutManager::UnencryptMenu( m_miMenus, std::size( m_miMenus ),
			vMenus,
			vMenuItems,
			vMenuStrings );

		LSW_WIDGET_LAYOUT * _pwMain = CHelpers::FindLayout( &vLayouts[0], vLayouts.size(), MX_MWI_MAINWINDOW );
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());

		_pwMain->lpwcClass = reinterpret_cast<LPCWSTR>(m_aMainClass);
		m_pwMainWindow = lsw::CBase::LayoutManager()->CreateWindowX( &vLayouts[0], vLayouts.size(),
			vMenus.size() ? &vMenus[0] : nullptr, vMenus.size(),
			nullptr, reinterpret_cast<uint64_t>(_pmhMemHack) );
		_pwMain->lpwcClass = nullptr;
		_pwMain->pwcText = nullptr;
		CLayoutManager::CleanEncryptedMenu( vMenuItems, vMenuStrings );
		return m_pwMainWindow;
	}

	// Creates the main menu and adds it to the window.
	BOOL CMainWindowLayout::CreateMenu( CWidget * _pwMainWindow ) {
		if ( !_pwMainWindow ) { return FALSE; }
		std::vector<ACCEL> vHotkeys;
		HMENU hMenu = lsw::CBase::LayoutManager()->CreateMenu( m_miMenus, std::size( m_miMenus ), vHotkeys );
		if ( !hMenu ) { return FALSE; }
		if ( vHotkeys.size() ) {
			CBase::GetAccelHandler().CreateAndRegister( _pwMainWindow->Wnd(), vHotkeys.data(), int( vHotkeys.size() ) );
		}
		return ::SetMenu( _pwMainWindow->Wnd(), hMenu );
	}

}	// namespace mx

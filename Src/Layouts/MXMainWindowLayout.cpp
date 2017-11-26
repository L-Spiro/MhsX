#include "MXMainWindowLayout.h"
#include "../Utilities/MXUtilities.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
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
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,		// dwStyle
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
#if 1
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
				RBS_VARHEIGHT | RBS_BANDBORDERS,		// dwStyle
			0,//WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MWI_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
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
			0,																						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MWI_MAINWINDOW,						// dwParentId
		},
#endif
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
			LSW_LT_LISTVIEW,						// ltType
			MX_MWI_STOREDADDRESSES,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			255,									// dwWidth
			110,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT,		// dwStyle
			WS_EX_CLIENTEDGE | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,																				// dwStyleEx
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


#if 1
		{
			LSW_LT_LISTVIEW,						// ltType
			MX_MWI_TEST0,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			255,									// dwWidth
			110,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT,		// dwStyle
			WS_EX_CLIENTEDGE | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,																				// dwStyleEx
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


		{
			LSW_LT_LISTVIEW,						// ltType
			MX_MWI_TEST1,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			255,									// dwWidth
			110,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT,		// dwStyle
			WS_EX_CLIENTEDGE | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,																				// dwStyleEx
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
#endif	// #if 0
		/*{
			LSW_LT_COMBOBOXEX,						// ltType
			50,										// wId
			WC_COMBOBOXEXW,						// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			10,										// iLeft
			30,										// iTop
			250,										// dwWidth
			150,										// dwHeight
			WS_VISIBLE | WS_CHILD | WS_TABSTOP |
				WS_VSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NORESIZE |
				CBS_AUTOHSCROLL | CBS_DROPDOWNLIST,		// dwStyle
			0,																		// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MWI_REBAR0,						// dwParentId
		},*/
		/*{
			LSW_LT_LISTVIEW,						// ltType
			MX_MWI_STOREDADDRESSES,					// wId
			WC_LISTVIEWW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			64,										// iLeft
			64,										// iTop
			50,									// dwWidth
			50,									// dwHeight
			WS_CHILD | LVS_REPORT | LVS_EDITLABELS | WS_VISIBLE,		// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			MX_MWI_MAINWINDOW,						// dwParentId
		},*/
	};

	// Menu bar items for the main window.
	LSW_MENU_ITEM CMainWindowLayout::m_miMenuBar[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_MWMI_FILE,				FALSE,		FALSE,		TRUE,		L"&File" },
		{ FALSE,		MX_MWMI_SEARCH,				FALSE,		FALSE,		TRUE,		L"&Search" },
		{ FALSE,		MX_MWMI_TOOLS,				FALSE,		FALSE,		TRUE,		L"&Tools" },
		{ FALSE,		MX_MWMI_WINDOW,				FALSE,		FALSE,		TRUE,		L"&Window" },
		{ FALSE,		MX_MWMI_HELP,				FALSE,		FALSE,		TRUE,		L"&Help" },
	};

	// File menu.
	LSW_MENU_ITEM CMainWindowLayout::m_miFileMenu[] {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_MWMI_OPENPROCESS,		FALSE,		FALSE,		TRUE,		L"Open &Process" },
		{ FALSE,		MX_MWMI_OPENRECENT,			FALSE,		FALSE,		TRUE,		L"Open &Recent" },
		{ FALSE,		MX_MWMI_OPENFORDEBUG,		FALSE,		FALSE,		TRUE,		L"Open for &Debug" },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_OPENSAVEFILE,		FALSE,		FALSE,		TRUE,		L"&Open Save File" },
		{ FALSE,		MX_MWMI_SAVE,				FALSE,		FALSE,		TRUE,		L"&Save" },
		{ FALSE,		MX_MWMI_SAVEAS,				FALSE,		FALSE,		TRUE,		L"Save &As" },
	};

	// Search menu.
	LSW_MENU_ITEM CMainWindowLayout::m_miSearchMenu[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_MWMI_DATATYPE,			FALSE,		FALSE,		TRUE,		L"&Data-Type Search" },
		{ FALSE,		MX_MWMI_POINTER,			FALSE,		FALSE,		TRUE,		L"&Pointer Search" },
		{ FALSE,		MX_MWMI_STRING,				FALSE,		FALSE,		TRUE,		L"&String Search" },
		{ FALSE,		MX_MWMI_GROUP,				FALSE,		FALSE,		TRUE,		L"&Group Search" },
		{ FALSE,		MX_MWMI_EXPRESSION,			FALSE,		FALSE,		TRUE,		L"&Expression Search" },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_SUB,				FALSE,		FALSE,		TRUE,		L"S&ub Search" },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_INSERT,				FALSE,		FALSE,		TRUE,		L"&Insert Address" },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_SEARCHOPTIONS,		FALSE,		FALSE,		TRUE,		L"Search &Options" },
	};

	// Options menu.
	LSW_MENU_ITEM CMainWindowLayout::m_miOptionsMenu[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_MWMI_OPTIONS,			FALSE,		FALSE,		TRUE,		L"&Options" },
	};

	// Windows menu.
	LSW_MENU_ITEM CMainWindowLayout::m_miWindowsMenu[]= {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_MWMI_SHOW_EXPEVAL,		TRUE,		FALSE,		TRUE,		L"&Expression Evaluator" },
		{ FALSE,		MX_MWMI_SHOWFOUNDADDR,		TRUE,		FALSE,		TRUE,		L"&Found Addresses" },
		{ TRUE,			0,							FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_MWMI_SHOW_ALL,			TRUE,		FALSE,		TRUE,		L"Show &All" },
	};

	// Menus.
	LSW_MENU_LAYOUT CMainWindowLayout::m_miMenus[] = {
		{
			MX_MWMI_MENU_BAR,
			0,
			0,
			MX_ELEMENTS( m_miMenuBar ),
			m_miMenuBar
		},
		{
			MX_MWMI_MENU_FILE,
			MX_MWMI_MENU_BAR,
			MX_MWMI_FILE,
			MX_ELEMENTS( m_miFileMenu ),
			m_miFileMenu
		},
		{
			MX_MWMI_MENU_SEARCH,
			MX_MWMI_MENU_BAR,
			MX_MWMI_SEARCH,
			MX_ELEMENTS( m_miSearchMenu ),
			m_miSearchMenu
		},
		{
			MX_MWMI_MENU_TOOLS,
			MX_MWMI_MENU_BAR,
			MX_MWMI_TOOLS,
			MX_ELEMENTS( m_miOptionsMenu ),
			m_miOptionsMenu
		},
		{
			MX_MWMI_MENU_WINDOW,
			MX_MWMI_MENU_BAR,
			MX_MWMI_WINDOW,
			MX_ELEMENTS( m_miWindowsMenu ),
			m_miWindowsMenu
		},
	};

	// The class for the main window.
	ATOM CMainWindowLayout::m_aMainClass = 0;

	// The main window.
	CWidget * CMainWindowLayout::m_pwMainWindow = nullptr;

	// == Functions.
	// Creates the main window.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc., and registers the main window class.
	BOOL CMainWindowLayout::CreateMainWindow() {
		if ( !m_aMainClass ) {
			// Register the window classes we need.
			WCHAR szTemp[5];
			CUtilities::RandomString( szTemp, MX_ELEMENTS( szTemp ) );
			lsw::CWndClassEx wceEx( lsw::CWidget::WindowProc, szTemp );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1) );
			m_aMainClass = lsw::CBase::RegisterClassExW( wceEx.Obj() );	
		}

		std::vector<std::string> sStrings;
		std::vector<std::wstring> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ),
			vLayouts,
			sStringsW,
			sStrings );

		LSW_WIDGET_LAYOUT * _pwMain = CHelpers::FindLayout( &vLayouts[0], vLayouts.size(), MX_MWI_MAINWINDOW );

		_pwMain->lpwcClass = reinterpret_cast<LPCWSTR>(m_aMainClass);
		m_pwMainWindow = lsw::CBase::LayoutManager()->CreateWindowX( &vLayouts[0], vLayouts.size(), m_miMenus, MX_ELEMENTS( m_miMenus ) );
		_pwMain->lpwcClass = nullptr;
		_pwMain->pwcText = nullptr;
		//::ZeroMemory( szTempTitle, MX_ELEMENTS( szTempTitle ) );


		// *** TEMP ***
		/*HMENU hMenu = lsw::CBase::LayoutManager()->CreateMenu( m_miMenus, MX_ELEMENTS( m_miMenus ) );
		::SetMenu( m_pwMainWindow->Wnd(), hMenu );
		::RedrawWindow( m_pwMainWindow->Wnd(), NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT | RDW_ERASE );*/

		/*HMENU hMenu = ::CreateMenu();
		AppendMenuW(
			hMenu,
			MF_ENABLED | MF_STRING,
			3,
			L"&File" );
		AppendMenuW(
			hMenu,
			MF_ENABLED | MF_STRING,
			4,
			L"&Blah" );

		MENUITEMINFOW miItem = {
			sizeof( MENUITEMINFOW ),
		};
		miItem.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
		miItem.fType = MFT_STRING;
		miItem.fState = MFS_GRAYED;
		miItem.wID = 15;
		miItem.dwTypeData = L"Coo&l";
		miItem.cch = lstrlenW( miItem.dwTypeData );

		InsertMenuItem( hMenu, 4, FALSE, &miItem );
		//SetMenuItemInfo( 

		::SetMenu( m_pwMainWindow->Wnd(), hMenu );*/
		return TRUE;
	}

	// Creates the main menu and adds it to the window.
	BOOL CMainWindowLayout::CreateMenu( CWidget * _pwMainWindow ) {
		if ( !_pwMainWindow ) { return FALSE; }
		HMENU hMenu = lsw::CBase::LayoutManager()->CreateMenu( m_miMenus, MX_ELEMENTS( m_miMenus ) );
		if ( !hMenu ) { return FALSE; }
		return ::SetMenu( _pwMainWindow->Wnd(), hMenu );
	}

}	// namespace mx

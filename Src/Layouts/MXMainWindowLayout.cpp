#include "MXMainWindowLayout.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"

#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <Layout/LSWLayoutManager.h>
#include <random>

namespace mx {

	// == Members.
	LSW_WIDGET_LAYOUT CMainWindowLayout::m_wlMainWindow[] = {
		{
			MX_MAIN_WINDOW,							// ltType
			MX_MWI_MAINWINDOW,						// dwId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			64,										// iLeft
			64,										// iTop
			772,									// dwWidth
			605,									// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,		// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			MX_MWI_NONE,							// dwParentId
		},
		/*{
			LSW_LT_LISTVIEW,						// ltType
			MX_MWI_STOREDADDRESSES,					// dwId
			WC_LISTVIEWW,							// lpwcClass
			TRUE,									// bEnabled
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
			std::random_device rdRand;
			std::mt19937 mGen( rdRand() );
			std::uniform_int_distribution<> uidDist( 'a', 'z' );

			for ( SIZE_T I = 0; I < MX_ELEMENTS( szTemp ); ++I ) {
				szTemp[I] = uidDist( mGen );
			}
			szTemp[MX_ELEMENTS( szTemp )-1] = L'\0';
			lsw::CWndClassEx wceEx( lsw::CWidget::WindowProc, szTemp );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1) );
			m_aMainClass = lsw::CBase::RegisterClassExW( wceEx.Obj() );

			
		}
		WCHAR szTempTitle[_LEN_F8A1A494+1];
		_DEC_F8A1A494_L__Spiro_MHS_X( szTempTitle );
		LSW_WIDGET_LAYOUT * _pwMain = CHelpers::FindLayout( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ), MX_MWI_MAINWINDOW );

		_pwMain->lpwcClass = reinterpret_cast<LPCWSTR>(m_aMainClass);
		_pwMain->pwcText = szTempTitle;
		m_pwMainWindow = lsw::CBase::LayoutManager()->CreateWindowX( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ) );
		_pwMain->lpwcClass = nullptr;
		_pwMain->pwcText = nullptr;
		::ZeroMemory( szTempTitle, MX_ELEMENTS( szTempTitle ) );


		// *** TEMP ***
		HMENU hMenu = lsw::CBase::LayoutManager()->CreateMenu( m_miMenus, MX_ELEMENTS( m_miMenus ) );
		::SetMenu( m_pwMainWindow->Wnd(), hMenu );
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

	}

}	// namespace mx

#include "MXDeuHexMachinaWindowLayout.h"
//#include "../PE/MXDeusHexMachinaWindow.h"
#include "../Utilities/MXUtilities.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>
//#include <Base/LSWWndClassEx.h>
//#include <Layout/LSWLayoutManager.h>

namespace mx {

#define MX_W_W				1460
#define MX_W_H				940
	// == Members.
	// The layout for the PE Works window.
	LSW_WIDGET_LAYOUT CDeusHexMachinaLayout::m_wlMainWindow[] = {
		{
			MX_PE_WORKS,									// ltType
			MX_W_MAINWINDOW,								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			135,											// iLeft
			150,											// iTop
			MX_W_W,											// dwWidth
			MX_W_H,											// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | DS_3DLOOK | DS_SETFONT,				// dwStyle
			WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,														// dwStyleEx
			MX_MAKE_WCHAR( _T_D09DA91D_PE_Works ),			// pwcText
			_LEN_D09DA91D,									// sTextLen
			MX_W_NONE,										// dwParentId
			/*
			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
			*/
		},
		{
			LSW_LT_STATUSBAR,								// ltType
			MX_W_STATUSBAR,									// wId
			STATUSCLASSNAMEW,								// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			0,												// dwWidth
			0,												// dwHeight
			WS_VISIBLE | WS_CHILD | SBARS_SIZEGRIP,			// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_W_MAINWINDOW,								// dwParentId

			nullptr, 0,										// pcLeftSizeExp
			nullptr, 0,										// pcRightSizeExp
			nullptr, 0,										// pcTopSizeExp
			nullptr, 0,										// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			nullptr, 0,										// pcHeightSizeExp
		},
#if 1
		{
			LSW_LT_REBAR,									// ltType
			MX_W_REBAR0,									// wId
			REBARCLASSNAMEW,								// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			m_wlMainWindow[0].dwWidth,						// dwWidth
			150,											// dwHeight
			WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN |
				RBS_VARHEIGHT | RBS_BANDBORDERS,			// dwStyle
			0,//WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW,										// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_W_MAINWINDOW,								// dwParentId

			MX_PARENT_CLEFT,//MX_LOCK_LEFT,					// pcLeftSizeExp
			MX_PARENT_CRIGHT,								// pcRightSizeExp
			MX_PARENT_CTOP,									// pcTopSizeExp
			nullptr, 0,										// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			MX_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_TOOLBAR,									// ltType
			MX_W_TOOLBAR0,									// wId
			TOOLBARCLASSNAMEW,								// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			0,												// dwWidth
			0,												// dwHeight
			WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | WS_VISIBLE | CCS_ADJUSTABLE,		// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_W_MAINWINDOW,								// dwParentId
		},
#endif
		{
			LSW_LT_TAB,										// ltType
			MX_W_TABS,										// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			255,											// dwWidth
			110,											// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK,				// dwStyle
			WS_EX_ACCEPTFILES,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_W_MAINWINDOW,								// dwParentId

			MX_PARENT_VCLEFT,								// pcLeftSizeExp
			MX_PARENT_VCRIGHT,								// pcRightSizeExp
			MX_PARENT_VCTOP,								// pcTopSizeExp
			MX_PARENT_VCBOTTOM,								// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			nullptr, 0,										// pcHeightSizeExp
		},
		/*{
			LSW_LT_TREELIST,						// ltType
			MX_W_TESTTREELIST,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			255,									// dwWidth
			110,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP,								// dwStyle
			0,																		// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_W_MAINWINDOW,						// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},*/
	};

#undef MX_W_W
#undef MX_W_H

	// Menu bar items for the main window.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miMenuBar[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_MENU_FILE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_854A6B8B__File, _LEN_854A6B8B ) },
		/*{ FALSE,		MX_M_MENU_SEARCH,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_339FACEA__Search, _LEN_339FACEA ) },
		{ FALSE,		MX_M_MENU_TOOLS,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_8D2E41E6__Tools, _LEN_8D2E41E6 ) },
		{ FALSE,		MX_M_MENU_WINDOW,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0C8B8E90__Window, _LEN_0C8B8E90 ) },
		{ FALSE,		MX_M_MENU_HELP,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_01520137__Help, _LEN_01520137 ) },*/
	};

	// File menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miFileMenu[] {
		//bIsSeperator	dwId							bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_FILE_OPENFILE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2BEA22F9__Open_File, _LEN_2BEA22F9 ) },
		{ FALSE,		MX_M_FILE_OPENCURPROCESS,		FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_3E14AB9A_Open__Current_Process, _LEN_3E14AB9A ) },
		{ FALSE,		MX_M_FILE_OPENFILE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_364A9A57_Open__Process, _LEN_364A9A57 ) },
		{ FALSE,		MX_M_FILE_OPENRECENT,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EA79B404_Open__Recent, _LEN_EA79B404 ) },
	};

	// Menus.
	LSW_MENU_LAYOUT CDeusHexMachinaLayout::m_miMenus[] = {
		{
			MX_M_MENU_BAR,
			0,
			0,
			MX_ELEMENTS( m_miMenuBar ),
			m_miMenuBar
		},
		{
			MX_M_FILE,
			MX_M_MENU_BAR,
			MX_M_MENU_FILE,
			MX_ELEMENTS( m_miFileMenu ),
			m_miFileMenu
		},
	};

	// == Functions.
	// Creates the PE Works window.
	CWidget * CDeusHexMachinaLayout::CreateDeusHexMachinaWindow( CWidget * _pwParent ) {
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		CLayoutManager::UnencryptLayouts( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ),
			vLayouts,
			sStringsW,
			sStrings );

		std::vector<LSW_MENU_LAYOUT> vMenus;
		std::vector<std::vector<LSW_MENU_ITEM> *> vMenuItems;
		std::vector<std::vector<CSecureWString> *> vMenuStrings;
		CLayoutManager::UnencryptMenu( m_miMenus, MX_ELEMENTS( m_miMenus ),
			vMenus,
			vMenuItems,
			vMenuStrings );


		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		//CDeusHexMachinaWindow::PrepareDeusHexMachina();
		CWidget * pwThis = plmLayout->CreateWindowX( &vLayouts[0], MX_ELEMENTS( m_wlMainWindow ),
			vMenus.size() ? &vMenus[0] : nullptr, vMenus.size(),
			_pwParent );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		CLayoutManager::CleanEncryptedMenu( vMenuItems, vMenuStrings );

		return pwThis;
	}

}	// namespace mx

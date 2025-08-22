#include "MXPeWorksLayout.h"
#include "../PE/MXPeWorksWindow.h"
#include "../Utilities/MXUtilities.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>
//#include <Base/LSWWndClassEx.h>
//#include <Layout/LSWLayoutManager.h>

namespace mx {

#define MX_PEW_W				1460
#define MX_PEW_H				940
	// == Members.
	// The layout for the PE Works window.
	LSW_WIDGET_LAYOUT CPeWorksLayout::m_wlMainWindow[] = {
		{
			MX_PE_WORKS,							// ltType
			MX_PEW_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			135,									// iLeft
			150,									// iTop
			MX_PEW_W,								// dwWidth
			MX_PEW_H,								// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | DS_3DLOOK | DS_SETFONT,				// dwStyle
			WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,														// dwStyleEx
			MX_MAKE_WCHAR( _T_D09DA91D_PE_Works ),	// pwcText
			_LEN_D09DA91D,							// sTextLen
			MX_PEW_NONE,							// dwParentId
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
			LSW_LT_STATUSBAR,						// ltType
			MX_PEW_STATUSBAR,						// wId
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
			MX_PEW_MAINWINDOW,						// dwParentId

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
			MX_PEW_REBAR0,							// wId
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
			MX_PEW_MAINWINDOW,						// dwParentId

			MX_PARENT_CLEFT,//MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_PARENT_CRIGHT,							// pcRightSizeExp
			MX_PARENT_CTOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_TOOLBAR,							// ltType
			MX_PEW_TOOLBAR0,						// wId
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
			MX_PEW_MAINWINDOW,						// dwParentId
		},
#endif
		{
			LSW_LT_TAB,								// ltType
			MX_PEW_TABS,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			255,									// dwWidth
			110,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK,				// dwStyle
			WS_EX_ACCEPTFILES,														// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_PEW_MAINWINDOW,						// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		/*{
			LSW_LT_TREELIST,						// ltType
			MX_PEW_TESTTREELIST,					// wId
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
			MX_PEW_MAINWINDOW,						// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},*/
	};

#undef MX_PEW_W
#undef MX_PEW_H

	// Menu bar items for the main window.
	LSW_MENU_ITEM CPeWorksLayout::m_miMenuBar[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_PEW_MENU_FILE,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_854A6B8B__File, _LEN_854A6B8B ) },
		/*{ FALSE,		MX_PEW_MENU_SEARCH,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_339FACEA__Search, _LEN_339FACEA ) },
		{ FALSE,		MX_PEW_MENU_TOOLS,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_8D2E41E6__Tools, _LEN_8D2E41E6 ) },
		{ FALSE,		MX_PEW_MENU_WINDOW,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0C8B8E90__Window, _LEN_0C8B8E90 ) },
		{ FALSE,		MX_PEW_MENU_HELP,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_01520137__Help, _LEN_01520137 ) },*/
	};

	// File menu.
	LSW_MENU_ITEM CPeWorksLayout::m_miFileMenu[] {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_PEW_OPENPE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2D715017_Open_PE_File, _LEN_2D715017 ) },
		{ FALSE,		MX_PEW_OPENRECENT,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EA79B404_Open__Recent, _LEN_EA79B404 ) },
	};

	// Menus.
	LSW_MENU_LAYOUT CPeWorksLayout::m_miMenus[] = {
		{
			MX_PEW_MENU_BAR,
			0,
			0,
			MX_ELEMENTS( m_miMenuBar ),
			m_miMenuBar
		},
		{
			MX_PEW_FILE,
			MX_PEW_MENU_BAR,
			MX_PEW_MENU_FILE,
			MX_ELEMENTS( m_miFileMenu ),
			m_miFileMenu
		},
	};

	// == Functions.
	// Creates the PE Works window.
	CWidget * CPeWorksLayout::CreatePeWorksWindow( CWidget * _pwParent ) {
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
		CPeWorksWindow::PreparePeWorks();
		CWidget * pwThis = plmLayout->CreateWindowX( &vLayouts[0], MX_ELEMENTS( m_wlMainWindow ),
			vMenus.size() ? &vMenus[0] : nullptr, vMenus.size(),
			_pwParent );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		CLayoutManager::CleanEncryptedMenu( vMenuItems, vMenuStrings );

		return pwThis;
	}

}	// namespace mx

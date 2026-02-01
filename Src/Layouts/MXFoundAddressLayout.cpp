#include "MXFoundAddressLayout.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../Utilities/MXUtilities.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <Layout/LSWLayoutManager.h>


namespace mx {

	// == Members.
	// The layout for the Found Addresses dockable window.
	LSW_WIDGET_LAYOUT CFoundAddressLayout::m_wlMainWindow[] = {
		{
			MX_FOUND_ADDRESSES,						// ltType
			MX_FAW_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			154,									// iLeft
			153,									// iTop
			340,									// dwWidth
			491,									// dwHeight
			WS_VISIBLE,								// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_2741E247_Found_Addresses ),		// pwcText
			_LEN_2741E247,										// sTextLen
			0,										// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_REBAR,							// ltType
			MX_FAW_REBAR0,							// wId
			nullptr,								// lpwcClass
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
			0,//WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FAW_MAINWINDOW,						// dwParentId

			MX_PARENT_CLEFT,						// pcLeftSizeExp
			MX_PARENT_CRIGHT,						// pcRightSizeExp
			MX_PARENT_CTOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_TOOLBAR,							// ltType
			MX_FAW_TOOLBAR0,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			0,										// dwWidth
			0,										// dwHeight
			WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | WS_VISIBLE | CCS_ADJUSTABLE | TBSTYLE_LIST,		// dwStyle
			0,																		// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FAW_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_FAW_PADDING_EDIT_LEFT,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			0,										// dwWidth
			0,										// dwHeight
			MX_EDITSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FAW_MAINWINDOW,						// dwParentId
		},
		/*{
			LSW_LT_UPDOWN,							// ltType
			MX_FAW_PADDINGLEFT,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			0,										// dwWidth
			0,										// dwHeight
			MX_UPDOWN_STYLE,						// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FAW_MAINWINDOW,						// dwParentId
		},*/
		{
			MX_FOUND_ADDRESSES_LISTVIEW,			// ltType
			MX_FAW_LIST,							// wId
			WC_LISTVIEWW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			255,									// dwWidth
			110,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_OWNERDATA | LVS_NOSORTHEADER | WS_TABSTOP,		// dwStyle
			WS_EX_CLIENTEDGE | LVS_EX_FULLROWSELECT /*| LVS_EX_DOUBLEBUFFER*/,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FAW_MAINWINDOW,						// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
	};

	// == Functions.
	// Creates the Found Addresses dockable window.
	CWidget * CFoundAddressLayout::CreateFoundAddressesWindow( CWidget * _pwParent, CMhsMainWindow * _pmmwMhsWindow ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlMainWindow, std::size( m_wlMainWindow ),
			vLayouts,
			sStringsW,
			sStrings );
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwThis = plmLayout->CreateWindowX( &vLayouts[0], std::size( m_wlMainWindow ), nullptr, 0, _pwParent, reinterpret_cast<uint64_t>(_pmmwMhsWindow) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );

		return pwThis;
	}

}	// namespace mx

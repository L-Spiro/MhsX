#include "MXFindResultsLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>
#include <Layout/LSWLayoutManager.h>


namespace mx {

	// == Members.
	// The layout of the Find Results window.  The map and the list are placed by the window.
	LSW_WIDGET_LAYOUT CFindResultsLayout::m_wlFindResults[] = {
		{
			MX_FIND_RESULTS,						// ltType
			MX_FRI_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			0,										// iLeft
			0,										// iTop
			600,									// dwWidth
			200,									// dwHeight
			WS_VISIBLE,								// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_75B7F809_Find_Results ),		// pwcText
			_LEN_75B7F809,					// sTextLen
			0,										// dwParentId

			// The dock target places the window through these (its part of the dock target is the parent's virtual client area).
			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FRI_MAP,								// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			24,										// dwWidth
			100,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | SS_NOTIFY,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FRI_MAINWINDOW,						// dwParentId
		},
		{
			MX_FIND_RESULTS_LIST,					// ltType
			MX_FRI_LIST,							// wId
			WC_LISTVIEWW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			26,										// iLeft
			0,										// iTop
			500,									// dwWidth
			100,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS,	// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FRI_MAINWINDOW,						// dwParentId
		},
	};

	// == Functions.
	/**
	 * Creates the Find Results window.
	 *
	 * \param _pwParent The parent (the dock target it is created in).
	 * \return Returns the window, or nullptr on failure.
	 **/
	CWidget * CFindResultsLayout::CreateFindResultsWindow( CWidget * _pwParent ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlFindResults, std::size( m_wlFindResults ),
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwThis = plmLayout->CreateWindowX( &vLayouts[0], std::size( m_wlFindResults ), nullptr, 0, _pwParent );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		return pwThis;
	}

}	// namespace mx

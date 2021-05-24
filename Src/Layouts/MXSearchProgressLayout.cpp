#include "MXSearchProgressLayout.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>
#include <Layout/LSWLayoutManager.h>

namespace mx {

	// == Members.
	// The layout for the Open Process dialog.
	LSW_WIDGET_LAYOUT CSearchProgressLayout::m_wlSearchProgressDialog[] = {
		{
			MX_SEARCH_PROGRESS,						// ltType
			MX_SPW_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			(MX_DEF_PROGRESS_BAR_WIDTH + 6) + MX_DEF_BUTTON_WIDTH + 4,																		// dwWidth
			static_cast<DWORD>(4 + std::max( MX_DEF_PROGRESS_BAR_HEIGHT, MX_DEF_BUTTON_HEIGHT ) + 4),										// dwHeight
			WS_CAPTION | WS_CLIPCHILDREN | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME,									// dwStyleEx
			MX_MAKE_WCHAR( _T_16A69772_Searching___ ),																						// pwcText
			_LEN_16A69772,							// sTextLen
			MX_SPW_NONE,							// dwParentId
		},
		{
			LSW_LT_PROGRESSBAR,						// ltType
			MX_SPW_PROGRESSBAR,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			4,										// iLeft
			4,										// iTop
			MX_DEF_PROGRESS_BAR_WIDTH,				// dwWidth
			MX_DEF_PROGRESS_BAR_HEIGHT,				// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | PBS_SMOOTH,															// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,		// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SPW_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_SPW_CANCEL,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_DEF_PROGRESS_BAR_WIDTH + 6,			// iLeft
			4,										// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),																// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_SPW_MAINWINDOW,						// dwParentId
		},
	};

	// == Functions.
	// Creates the search progress dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	BOOL CSearchProgressLayout::CreateSearchProgressDialog( CWidget * _pwParent, const MX_SEARCH_DLG_PARMS * _psdpParms ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlSearchProgressDialog, MX_ELEMENTS( m_wlSearchProgressDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], MX_ELEMENTS( m_wlSearchProgressDialog ), _pwParent, reinterpret_cast<uint64_t>(_psdpParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

}	// namespace mx

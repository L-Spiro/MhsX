#include "MXOpenProcessLayout.h"
#include "../System/MXSystem.h"

#include <Base/LSWBase.h>
#include <Layout/LSWLayoutManager.h>

namespace mx {

	// == Members.
	// The layout for the Open Process dialog.
	LSW_WIDGET_LAYOUT COpenProcessLayout::m_wlOpenProcessDialog[] = {
		{
			LSW_LT_MAINWINDOW,						// ltType
			MX_OPI_DIALOG,							// dwId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			64,										// iLeft
			64,										// iTop
			271,									// dwWidth
			198,									// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | WS_THICKFRAME | DS_3DLOOK | DS_FIXEDSYS | DS_CENTER,		// dwStyle
			//WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION,
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,										// dwStyleEx
			L"Open Process",						// pwcText
			MX_OPI_NONE,							// dwParentId
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OPI_GROUP,							// dwId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			4,										// iLeft
			4,										// iTop
			263,									// dwWidth
			174,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,										// dwStyleEx
			L"Select Process",						// pwcText
			MX_OPI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LISTVIEW,						// ltType
			MX_OPI_LISTVIEW,						// dwId
			L"SysListView32",						// lpwcClass
			TRUE,									// bEnabled
			8,										// iLeft
			13,										// iTop
			255,									// dwWidth
			110,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE | LVS_EX_FULLROWSELECT,										// dwStyleEx
			L"L",									// pwcText
			MX_OPI_DIALOG,							// dwParentId
		},
	};


	// == Functions.
	// Creates the Open Process dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	BOOL COpenProcessLayout::CreateOpenProcessDialog( CWidget * _pwParent ) {
		lsw::CBase::LayoutManager()->CreateDialogX( m_wlOpenProcessDialog, MX_ELEMENTS( m_wlOpenProcessDialog ), TRUE, _pwParent );

		return TRUE;
	}

}	// namespace mx

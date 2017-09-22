#include "MXOpenProcessLayout.h"
#include "../Layouts/MXLayoutManager.h"
#include "../System/MXSystem.h"

#include <Base/LSWBase.h>
#include <Layout/LSWLayoutManager.h>

namespace mx {

	// == Members.
	// The layout for the Open Process dialog.
	LSW_WIDGET_LAYOUT COpenProcessLayout::m_wlOpenProcessDialog[] = {
		{
			MX_OPEN_PROCESS_WINDOW,					// ltType
			MX_OPI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			64,										// iLeft
			64,										// iTop
			271,									// dwWidth
			198,									// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | WS_THICKFRAME | DS_3DLOOK | DS_FIXEDSYS | DS_CENTER,					// dwStyle
			//WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION,
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			L"Open Process",						// pwcText
			MX_OPI_NONE,							// dwParentId
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OPI_GROUP,							// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			4,										// iLeft
			4,										// iTop
			263,									// dwWidth
			174,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																// dwStyleEx
			L"Select Process",						// pwcText
			MX_OPI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LISTVIEW,						// ltType
			MX_OPI_LISTVIEW,						// wId
			L"SysListView32",						// lpwcClass
			TRUE,									// bEnabled
			8,										// iLeft
			13,										// iTop
			255,									// dwWidth
			110,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE | LVS_EX_FULLROWSELECT,						// dwStyleEx
			nullptr,								// pwcText
			MX_OPI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OPI_BUTTON_OK,						// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			4,										// iLeft
			182,									// iTop
			43,										// dwWidth
			12,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			L"OK",									// pwcText
			MX_OPI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OPI_BUTTON_CANCEL,					// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			224,									// iLeft
			182,									// iTop
			43,										// dwWidth
			12,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			L"Cancel",								// pwcText
			MX_OPI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OPI_GROUPSHOW,						// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			9,										// iLeft
			128,									// iTop
			253,									// dwWidth
			20,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																// dwStyleEx
			L"Show",								// pwcText
			MX_OPI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_OPI_RADIO_MAIN,						// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			14,										// iLeft
			136,									// iTop
			29,										// dwWidth
			9,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																// dwStyleEx
			L"Main",								// pwcText
			MX_OPI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_OPI_RADIO_ALL,						// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			238,										// iLeft
			136,									// iTop
			20,										// dwWidth
			9,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																// dwStyleEx
			L"All",								// pwcText
			MX_OPI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OPI_CHECK_RESTR,						// wId
			L"Button",								// lpwcClass
			FALSE,									// bEnabled
			9,										// iLeft
			150,									// iTop
			253,										// dwWidth
			9,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																// dwStyleEx
			L"Open in Restricted Mode (Improves Undetectability)",								// pwcText
			MX_OPI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OPI_BUTTON_REFRESH,					// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			9,									// iLeft
			161,									// iTop
			253,									// dwWidth
			12,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			L"Refresh",								// pwcText
			MX_OPI_DIALOG,							// dwParentId
		},
	};


	// == Functions.
	// Creates the Open Process dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	BOOL COpenProcessLayout::CreateOpenProcessDialog( CWidget * _pwParent ) {
		if ( lsw::CBase::LayoutManager()->CreateDialogX( m_wlOpenProcessDialog, MX_ELEMENTS( m_wlOpenProcessDialog ), _pwParent ) ) {
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

}	// namespace mx

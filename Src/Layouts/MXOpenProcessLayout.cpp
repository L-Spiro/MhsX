#include "MXOpenProcessLayout.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>
#include <Layout/LSWLayoutManager.h>

#define MX_OPL_W									598
#define MX_OPL_SELECT_GROUP_TOP						MX_TOP_JUST
#define MX_OPL_VIEW_HEIGHT							350
#define MX_OPL_SELECT_GROUP_HEIGHT					(MX_OPL_VIEW_HEIGHT + MX_GROUP_TOP + MX_GROUP_BOTTOM)
#define MX_OPL_SHOW_GROUP_TOP						(MX_OPL_SELECT_GROUP_TOP + MX_OPL_SELECT_GROUP_HEIGHT + MX_TOP_JUST)
#define MX_OPL_SHOW_GROUP_HEIGHT					(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_DEF_RADIO_HEIGHT)
#define MX_OPL_MAIN_CHECK_W							29
#define MX_OPL_ALL_CHECK_W							17
#define MX_OPL_LAST_SECTION_TOP						(MX_OPL_SHOW_GROUP_TOP + MX_OPL_SHOW_GROUP_HEIGHT + MX_TOP_JUST)
#define MX_OPL_LAST_SECTION_H						(MX_DEF_CHECK_HEIGHT + MX_TOP_JUST + MX_DEF_BUTTON_HEIGHT)
#define MX_OPL_H									(MX_OPL_LAST_SECTION_TOP + MX_OPL_LAST_SECTION_H + MX_TOP_JUST * 4 + MX_DEF_BUTTON_HEIGHT)

namespace mx {

	// == Members.
	// The layout for the Open Process dialog.
	LSW_WIDGET_LAYOUT COpenProcessLayout::m_wlOpenProcessDialog[] = {
		{
			MX_OPEN_PROCESS_WINDOW,					// ltType
			MX_OPI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			64,										// iLeft
			64,										// iTop
			MX_OPL_W,								// dwWidth
			MX_OPL_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | WS_THICKFRAME | DS_3DLOOK | DS_FIXEDSYS | DS_CENTER,					// dwStyle
			//WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION,
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_49D5B53D_Open_Process ),																									// pwcText
			_LEN_49D5B53D,							// sTextLen
			MX_OPI_NONE,							// dwParentId
		},


		// Select Process.
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OPI_GROUP,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_OPL_SELECT_GROUP_TOP,				// iTop
			MX_OPL_W - (MX_LEFT_JUST * 2),			// dwWidth
			MX_OPL_SELECT_GROUP_HEIGHT,				// dwHeight
			MX_GROUPSTYLE,																																// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																// dwStyleEx
			MX_MAKE_WCHAR( _T_2FF354B7_Select_Process ),																								// pwcText
			_LEN_2FF354B7,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_LISTVIEW,						// ltType
			MX_OPI_LISTVIEW,						// wId
			WC_LISTVIEWW,							// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT,			// iLeft
			MX_OPL_SELECT_GROUP_TOP + MX_GROUP_TOP,	// iTop
			MX_OPL_W - (MX_LEFT_JUST + MX_GROUP_LEFT) * 2,																								// dwWidth
			MX_OPL_VIEW_HEIGHT,						// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT,		// dwStyle
			WS_EX_CLIENTEDGE | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OPI_BUTTON_OK,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_OPL_H - MX_DEF_BUTTON_HEIGHT - MX_TOP_JUST,														// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),																	// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OPI_BUTTON_CANCEL,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OPL_W - MX_DEF_BUTTON_WIDTH - MX_LEFT_JUST,														// iLeft
			MX_OPL_H - MX_DEF_BUTTON_HEIGHT - MX_TOP_JUST,														// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),																// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		// Show.
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OPI_GROUPSHOW,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_OPL_SHOW_GROUP_TOP,					// iTop
			MX_OPL_W - (MX_LEFT_JUST * 2),			// dwWidth
			MX_OPL_SHOW_GROUP_HEIGHT,				// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_923C763F_Show ),		// pwcText
			_LEN_923C763F,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_OPI_RADIO_MAIN,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT,			// iLeft
			MX_OPL_SHOW_GROUP_TOP + MX_GROUP_TOP,	// iTop
			MX_OPL_MAIN_CHECK_W,					// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_1F1A625A_Main ),		// pwcText
			_LEN_1F1A625A,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_OPI_RADIO_ALL,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OPL_W - MX_LEFT_JUST - MX_GROUP_RIGHT - MX_OPL_ALL_CHECK_W,										// iLeft
			MX_OPL_SHOW_GROUP_TOP + MX_GROUP_TOP,	// iTop
			MX_OPL_ALL_CHECK_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_0355373D_All ),		// pwcText
			_LEN_0355373D,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_CHECK,							// ltType
			MX_OPI_CHECK_RESTR,						// wId
			WC_BUTTONW,								// lpwcClass
			FALSE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_OPL_LAST_SECTION_TOP,				// iTop
			MX_OPL_W - MX_LEFT_JUST * 2,			// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_B563255B_Open_in_Restricted_Mode__Improves_Undetectability_ ),					// pwcText
			_LEN_B563255B,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OPI_BUTTON_REFRESH,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_OPL_LAST_SECTION_TOP + MX_TOP_JUST + MX_DEF_CHECK_HEIGHT,										// iTop
			MX_OPL_W - MX_LEFT_JUST * 2,			// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_AF3C9967_Refresh ),	// pwcText
			_LEN_AF3C9967,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
	};

#undef MX_OPL_H
#undef MX_OPL_LAST_SECTION_H
#undef MX_OPL_LAST_SECTION_TOP
#undef MX_OPL_ALL_CHECK_W
#undef MX_OPL_MAIN_CHECK_W
#undef MX_OPL_SHOW_GROUP_HEIGHT
#undef MX_OPL_SHOW_GROUP_TOP
#undef MX_OPL_SELECT_GROUP_HEIGHT
#undef MX_OPL_VIEW_HEIGHT
#undef MX_OPL_SELECT_GROUP_TOP
#undef MX_OPL_W

	// == Functions.
	// Creates the Open Process dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	DWORD COpenProcessLayout::CreateOpenProcessDialog( CWidget * _pwParent, MX_OPTIONS * _poOptions ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlOpenProcessDialog, MX_ELEMENTS( m_wlOpenProcessDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], MX_ELEMENTS( m_wlOpenProcessDialog ), _pwParent, reinterpret_cast<uint64_t>(_poOptions) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );

		return static_cast<DWORD>(ipProc);
	}

}	// namespace mx

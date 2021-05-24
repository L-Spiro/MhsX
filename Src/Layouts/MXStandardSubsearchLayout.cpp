#include "MXStandardSubsearchLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>

namespace mx {

#define MX_SSL_W									220
#define MX_SSL_INNER_W								(MX_SSL_W - (MX_LEFT_JUST + MX_GROUP_RIGHT) * 2)
#define MX_SSL_INFO_OFFSET							35
#define MX_SSL_INFO_GROUP_TOP						MX_TOP_JUST
#define MX_SSL_INFO_GROUP_HEIGHT					(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_DEF_STATIC_HEIGHT + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST) * 2)
#define MX_SSL_SEARCH_GROUP_TOP						(MX_SSL_INFO_GROUP_TOP + MX_SSL_INFO_GROUP_HEIGHT + MX_TOP_JUST)
#define MX_SSL_SEARCH_GROUP_HEIGHT					(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_DEF_COMBO_HEIGHT + (MX_TOP_JUST + MX_DEF_STATIC_HEIGHT) + MX_DEF_COMBO_HEIGHT + (MX_TOP_JUST + MX_DEF_STATIC_HEIGHT) + (MX_TOP_JUST + MX_DEF_COMBO_HEIGHT))
#define MX_SSL_OPTIONS_GROUP_TOP					(MX_SSL_SEARCH_GROUP_TOP + MX_SSL_SEARCH_GROUP_HEIGHT + MX_TOP_JUST)
#define MX_SSL_OPTIONS_GROUP_HEIGHT					(MX_GROUP_TOP + MX_GROUP_BOTTOM + std::max( MX_DEF_CHECK_HEIGHT + MX_CHECK_JUST, MX_DEF_BUTTON_HEIGHT ))
#define MX_SSL_H									(MX_SSL_OPTIONS_GROUP_TOP + MX_SSL_OPTIONS_GROUP_HEIGHT) + (MX_DEF_STATIC_HEIGHT * 3) + (MX_TOP_JUST * 4 + MX_DEF_BUTTON_HEIGHT)

	// == Members.
	// The layout for the window.
	LSW_WIDGET_LAYOUT CStandardSubsearchLayout::m_wlMainWindow[] = {
		{
			MX_STANDARD_SUBSEARCH,					// ltType
			MX_C_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			35,										// iLeft
			25,										// iTop
			MX_SSL_W,								// dwWidth
			static_cast<DWORD>(MX_SSL_H),			// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_507DD121_Subsearch ),	// pwcText
			_LEN_507DD121,							// sTextLen
			MX_C_NONE,								// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_C_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_JUST,																					// iLeft
			MX_SSL_H - MX_TOP_JUST - MX_DEF_BUTTON_HEIGHT,													// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,					// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),																// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_C_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SSL_W - MX_DEF_BUTTON_WIDTH - MX_LEFT_JUST,													// iLeft
			MX_SSL_H - MX_TOP_JUST - MX_DEF_BUTTON_HEIGHT,													// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,					// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),															// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},


		// Information.
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_C_INFO_GROUP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_SSL_INFO_GROUP_TOP,					// iTop
			MX_SSL_W - (MX_LEFT_JUST * 2),			// dwWidth
			MX_SSL_INFO_GROUP_HEIGHT,				// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_AFE59754_Information ),														// pwcText
			_LEN_AFE59754,																					// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_C_INFO_ORIG_SEARCH_TYPE_LABEL,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ),			// iLeft
			MX_SSL_INFO_GROUP_TOP + MX_GROUP_TOP,	// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ) - MX_SSL_INFO_OFFSET,	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_9081B25A_Original_Search_Type_ ),												// pwcText
			_LEN_9081B25A,																					// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_C_INFO_ORIG_SEARCH_TYPE_VAL_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ) - MX_SSL_INFO_OFFSET,		// iLeft
			MX_SSL_INFO_GROUP_TOP + MX_GROUP_TOP,	// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ) + MX_SSL_INFO_OFFSET,	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_E6183621_Previous_Search_Type_ ),												// pwcText
			_LEN_E6183621,																					// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_C_INFO_SEARCH_TYPE_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ),			// iLeft
			MX_SSL_INFO_GROUP_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT + MX_TOP_JUST,						// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ) - MX_SSL_INFO_OFFSET,	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_E6183621_Previous_Search_Type_ ),												// pwcText
			_LEN_E6183621,																					// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_C_INFO_SEARCH_TYPE_VAL_LABEL,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ) - MX_SSL_INFO_OFFSET,		// iLeft
			MX_SSL_INFO_GROUP_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT + MX_TOP_JUST,						// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ) + MX_SSL_INFO_OFFSET,	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_E6183621_Previous_Search_Type_ ),												// pwcText
			_LEN_E6183621,																					// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_C_INFO_RESULTS_LABEL,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ),			// iLeft
			MX_SSL_INFO_GROUP_TOP + MX_GROUP_TOP + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST) * 2,				// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ) - MX_SSL_INFO_OFFSET,	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_ABBC5B9D_Total_Results_ ),													// pwcText
			_LEN_ABBC5B9D,																					// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_C_INFO_RESULTS_VAL_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ) - MX_SSL_INFO_OFFSET,		// iLeft
			MX_SSL_INFO_GROUP_TOP + MX_GROUP_TOP + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST) * 2,				// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ) + MX_SSL_INFO_OFFSET,	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_E6183621_Previous_Search_Type_ ),												// pwcText
			_LEN_E6183621,																					// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},


		// Search.
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_C_SEARCH_GROUP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_SSL_SEARCH_GROUP_TOP,				// iTop
			MX_SSL_W - (MX_LEFT_JUST * 2),			// dwWidth
			MX_SSL_SEARCH_GROUP_HEIGHT,				// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_507DD121_Subsearch ),	// pwcText
			_LEN_507DD121,							// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_C_SEARCH_TYPE_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ),			// iLeft
			MX_SSL_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_LABEL_JUST,											// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_6A31DA03_Subsearch_Type_ ),													// pwcText
			_LEN_6A31DA03,																					// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_C_SEARCH_TYPE_COMBO,					// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ),			// iLeft
			MX_SSL_SEARCH_GROUP_TOP + MX_GROUP_TOP,															// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_C_SEARCH_LEFT_DESC_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ),			// iLeft
			MX_SSL_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_TOP_JUST,						// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8C044A93_Value_to_Find_ ),													// pwcText
			_LEN_8C044A93,																					// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_C_SEARCH_RIGHT_DESC_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ),			// iLeft
			MX_SSL_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_TOP_JUST,						// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8C044A93_Value_to_Find_ ),													// pwcText
			_LEN_8C044A93,																					// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_COMBOBOX,						// ltType
			MX_C_SEARCH_LEFT_VALUE_COMBO,			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ),			// iLeft
			MX_SSL_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_TOP_JUST + MX_DEF_STATIC_HEIGHT,
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_C_SEARCH_RIGHT_VALUE_COMBO,			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ),			// iLeft
			MX_SSL_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_TOP_JUST + MX_DEF_STATIC_HEIGHT,
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_C_SEARCH_EXP_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_RIGHT,			// iLeft
			MX_SSL_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_TOP_JUST + (MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT) + MX_TOP_JUST,
			MX_SSL_INNER_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_68C09196_Value_____ ),// pwcText
			_LEN_68C09196,							// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_C_SEARCH_EXP_COMBO,					// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_RIGHT,			// iLeft
			MX_SSL_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_TOP_JUST + (MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT) + MX_TOP_JUST + MX_DEF_STATIC_HEIGHT + MX_TOP_JUST,
			MX_SSL_INNER_W,							// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},


		// Options.
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_C_OPTIONS_GROUP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_SSL_OPTIONS_GROUP_TOP,				// iTop
			MX_SSL_W - (MX_LEFT_JUST * 2),			// dwWidth
			static_cast<DWORD>(MX_SSL_OPTIONS_GROUP_HEIGHT),			// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_1F88C31B_Options ),	// pwcText
			_LEN_1F88C31B,							// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{	// Invert Results.
			LSW_LT_CHECK,							// ltType
			MX_C_OPTIONS_INVERT_CHECK,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ),			// iLeft
			MX_SSL_OPTIONS_GROUP_TOP + MX_GROUP_TOP + MX_CHECK_JUST,										// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 0, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_EE3A07C4_Invert_Search ),														// pwcText
			_LEN_EE3A07C4,							// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_C_OPTIONS_OPTIONS_BUTTON,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ),			// iLeft
			MX_SSL_OPTIONS_GROUP_TOP + MX_GROUP_TOP,// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SSL_INNER_W, MX_LEFT_JUST + MX_GROUP_RIGHT, 2, 1, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,					// dwStyleEx
			MX_MAKE_WCHAR( _T_5FE14262_General_Search_Options ),											// pwcText
			_LEN_5FE14262,							// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},


		// ==== Description. ==== //
		{
			LSW_LT_LABEL,							// ltType
			MX_C_DESC_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			static_cast<INT>(MX_SSL_OPTIONS_GROUP_TOP + MX_SSL_OPTIONS_GROUP_HEIGHT),	// iTop
			MX_SSL_W - (MX_LEFT_JUST * 2),			// dwWidth
			MX_DEF_STATIC_HEIGHT * 3,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			L"",									// pwcText
			0,										// sTextLen
			MX_C_MAINWINDOW,						// dwParentId
		},
	};

#undef MX_SSL_OPTIONS_GROUP_HEIGHT
#undef MX_SSL_OPTIONS_GROUP_TOP
#undef MX_SSL_SEARCH_GROUP_HEIGHT
#undef MX_SSL_SEARCH_GROUP_TOP
#undef MX_SSL_INFO_GROUP_HEIGHT
#undef MX_SSL_INFO_GROUP_TOP
#undef MX_SSL_INFO_OFFSET
#undef MX_SSL_INNER_W
#undef MX_SSL_W
#undef MX_SSL_H

	// == Functions.
	// Creates the Standard Subsearch dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	DWORD CStandardSubsearchLayout::CreateStandardSubsearchDialog( CWidget * _pwParent, CMemHack * _pmhMemHack ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], MX_ELEMENTS( m_wlMainWindow ), _pwParent, reinterpret_cast<uint64_t>(_pmhMemHack) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

}	// namespace mx

#include "MXOptionsLayout.h"
#include "../System/MXSystem.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>

namespace mx {

#define MX_OPTIONS_W		(260 + 0)
#define MX_OPTIONS_H		(183 + 0)
#define MX_OPTIONS_LIST_W	(52)
#define MX_OD_L				(MX_OPTIONS_LIST_W + 5)
#define MX_OD_T				(2)
#define MX_OD_W				(MX_OPTIONS_W - MX_OD_L - 4)//(204)
#define MX_OD_H				(MX_OPTIONS_H - 26)//(156)
#define MX_OD_COORD			MX_OD_L, MX_OD_T, MX_OD_W, MX_OD_H

	// == Members.
	// The layout for the Options dialog.
	LSW_WIDGET_LAYOUT COptionsLayout::m_wlOptionsDialog[] = {
		{
			MX_OPTIONS_WINDOW,						// ltType
			MX_OI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_OPTIONS_W,							// dwWidth
			MX_OPTIONS_H,							// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_1F88C31B_Options ),																										// pwcText
			_LEN_1F88C31B,							// sTextLen
			MX_OI_NONE,								// dwParentId
		},
		{
			LSW_LT_LISTBOX,							// ltType
			MX_OI_LIST,								// wId
			WC_LISTBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			2,										// iLeft
			MX_OD_T,								// iTop
			MX_OPTIONS_LIST_W,						// dwWidth
			MX_OD_H,								// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,																// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,												// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_OPTIONS_W - MX_DEF_BUTTON_WIDTH * 2 - 2 - 2,	// iLeft
			MX_OPTIONS_H - MX_DEF_BUTTON_HEIGHT - 2,		// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),																	// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_OI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OPTIONS_W - MX_DEF_BUTTON_WIDTH * 1 - 2,		// iLeft
			MX_OPTIONS_H - MX_DEF_BUTTON_HEIGHT - 2,		// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),																// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_OI_DIALOG,							// dwParentId
		},
	};

	// General section.
#define MX_ROW_HEIGHT					(MX_DEF_COMBO_HEIGHT + 3)
#define MX_REFRESH_RATES_GROUP_HEIGHT	(MX_GROUP_TOP + (MX_ROW_HEIGHT * 4) + MX_GROUP_BOTTOM - 2)
#define MX_TPS_W						54
#define MX_TPS_L						(MX_OD_W - MX_GROUP_RIGHT - MX_TPS_W)
#define MX_TPS_CB_L						(MX_OD_W / 2)
#define MX_TPS_CB_W						(MX_TPS_L - MX_TPS_CB_L - 2)
	LSW_WIDGET_LAYOUT COptionsLayout::m_wlOptionsGeneral[] = {
		{
			MX_OPTIONS_GENERAL,						// ltType
			MX_OI_GENERAL,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_COORD,
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,															// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,																	// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_NONE,								// dwParentId
		},
		// ==== Refresh Rates ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_GENERAL_REFRESH_RATE,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_OD_W,								// dwWidth
			MX_REFRESH_RATES_GROUP_HEIGHT,			// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_5467F077_Refresh_Rates ),				// pwcText
			_LEN_5467F077,											// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		// ==== Refresh Found Addresses ==== //
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_REFRESH_FOUND_ADDRESSES,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + 2 + MX_ROW_HEIGHT * 0,	// iTop
			90,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_067EB2AF_Refresh_Found_Addresses ),				// pwcText
			_LEN_067EB2AF,														// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_GENERAL_REFRESH_FOUND_ADDRESSES_CB,	// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TPS_CB_L,							// iLeft
			MX_GROUP_TOP + MX_ROW_HEIGHT * 0,		// iTop
			MX_TPS_CB_W,							// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_TPS_0,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TPS_L,								// iLeft
			MX_GROUP_TOP + 2 + MX_ROW_HEIGHT * 0,	// iTop
			MX_TPS_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B7758FF8_Times_Per_Second ),						// pwcText
			_LEN_B7758FF8,														// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		// ==== Refresh Main List ==== //
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_REFRESH_MAIN_LIST,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + 2 + MX_ROW_HEIGHT * 1,	// iTop
			90,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8168E128_Refresh_Main_List ),						// pwcText
			_LEN_8168E128,														// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_GENERAL_REFRESH_MAIN_LIST_CB,		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TPS_CB_L,							// iLeft
			MX_GROUP_TOP + MX_ROW_HEIGHT * 1,		// iTop
			MX_TPS_CB_W,							// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_TPS_1,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TPS_L,								// iLeft
			MX_GROUP_TOP + 2 + MX_ROW_HEIGHT * 1,	// iTop
			MX_TPS_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B7758FF8_Times_Per_Second ),						// pwcText
			_LEN_B7758FF8,														// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		// ==== Update Locked Values ==== //
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_UPDDATE_LOCKED_VALUE,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + 2 + MX_ROW_HEIGHT * 2,	// iTop
			90,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_BAE47170_Update_Locked_Values ),					// pwcText
			_LEN_BAE47170,														// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_GENERAL_UPDDATE_LOCKED_VALUE_CB,	// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TPS_CB_L,							// iLeft
			MX_GROUP_TOP + MX_ROW_HEIGHT * 2,		// iTop
			MX_TPS_CB_W,							// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_TPS_2,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TPS_L,								// iLeft
			MX_GROUP_TOP + 2 + MX_ROW_HEIGHT * 2,	// iTop
			MX_TPS_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B7758FF8_Times_Per_Second ),						// pwcText
			_LEN_B7758FF8,														// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		// ==== Update Expression Evaluator ==== //
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_UPDDATE_EXP_EVAL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + 2 + MX_ROW_HEIGHT * 3,	// iTop
			90,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_625BE2FF_Update_Expression_Evaluator ),			// pwcText
			_LEN_625BE2FF,														// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_GENERAL_UPDDATE_EXP_EVAL_CB,		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TPS_CB_L,							// iLeft
			MX_GROUP_TOP + MX_ROW_HEIGHT * 3,		// iTop
			MX_TPS_CB_W,							// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_TPS_3,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TPS_L,								// iLeft
			MX_GROUP_TOP + 2 + MX_ROW_HEIGHT * 3,	// iTop
			MX_TPS_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B7758FF8_Times_Per_Second ),						// pwcText
			_LEN_B7758FF8,														// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},

		// ==== View Settings ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_GENERAL_VIEW_SETTINGS,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_REFRESH_RATES_GROUP_HEIGHT,			// iTop
			MX_OD_W,								// dwWidth
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2 + MX_GROUP_BOTTOM/*29*/,										// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_DD16C375_View_Settings ),				// pwcText
			_LEN_DD16C375,											// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_VIEW_CODE_NAMES,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_REFRESH_RATES_GROUP_HEIGHT + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 0,		// iTop
			91,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_D546F677_Data_Types_as_Code_Names ),	// pwcText
			_LEN_D546F677,											// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_VIEW_SIZES,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_W / 2,							// iLeft
			MX_REFRESH_RATES_GROUP_HEIGHT + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 0,		// iTop
			58,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_2DC47C2B_Data_Type_Sizes ),			// pwcText
			_LEN_2DC47C2B,											// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_VIEW_RANGES,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_REFRESH_RATES_GROUP_HEIGHT + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 1,		// iTop
			91,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8CD4CB22_Data_Type_Ranges ),	// pwcText
			_LEN_8CD4CB22,											// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},

	};
#undef MX_TPS_CB_W
#undef MX_TPS_CB_L
#undef MX_TPS_L
#undef MX_TPS_W
#undef MX_ROW_HEIGHT
#undef MX_REFRESH_RATES_GROUP_HEIGHT

	// General search section.
#define MX_SEARCH_RANGE_GROUP_HEIGHT	(MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 4 + MX_GROUP_BOTTOM)
#define MX_THREAD_PRIORITY_GROUP_HEIGHT	(MX_GROUP_TOP + MX_DEF_RADIO_HEIGHT + MX_GROUP_BOTTOM)
#define MX_BUFFER_GROUP_TOP				(MX_SEARCH_RANGE_GROUP_HEIGHT + MX_THREAD_PRIORITY_GROUP_HEIGHT)
#define MX_BUFFER_GROUP_HEIGHT			(MX_GROUP_TOP + MX_DEF_EDIT_HEIGHT + MX_GROUP_BOTTOM)
#define MX_MISC_GROUP_TOP				(MX_BUFFER_GROUP_TOP + MX_BUFFER_GROUP_HEIGHT)
#define MX_MISC_GROUP_HEIGHT			(MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2 + MX_DEF_STATIC_HEIGHT + MX_DEF_STATIC_HEIGHT + 2 + MX_DEF_EDIT_HEIGHT + MX_GROUP_BOTTOM)
#define MX_CRIT_W						30
#define MX_TP_L( IDX )					((MX_OD_W - MX_GROUP_LEFT - MX_GROUP_RIGHT - MX_CRIT_W) * IDX / 2 + MX_GROUP_LEFT)
	LSW_WIDGET_LAYOUT COptionsLayout::m_wlOptionsGeneralSearch[] = {
		{
			MX_OPTIONS_GENSEARCH,					// ltType
			MX_OI_GENERAL_SEARCH,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_COORD,
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,															// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,																	// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_NONE,								// dwParentId
		},
		// ==== Search Ranges ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_GENERAL_SEARCH_SEARCH_RANGES,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_OD_W,								// dwWidth
			MX_SEARCH_RANGE_GROUP_HEIGHT,			// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_638D307B_Search_Ranges ),				// pwcText
			_LEN_638D307B,											// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_SEARCH_MEM_IMAGE,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 0,	// iTop
			194,									// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_67CAB38F_MEM_IMAGE__Static_Data_is_Usually_Here_ ),	// pwcText
			_LEN_67CAB38F,															// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_SEARCH_MEM_PRIVATE,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 1,	// iTop
			194,									// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,																						// dwStyleEx
			MX_MAKE_WCHAR( _T_6B02F769_MEM_PRIVATE__Sections_Exclusive_to_the_Target_Process_ ),	// pwcText
			_LEN_6B02F769,																			// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_SEARCH_MEM_MAPPED,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2,	// iTop
			194,									// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_37D0905A_MEM_MAPPED__Data_Loaded_by_the_Target_Process__Slow_ ),	// pwcText
			_LEN_37D0905A,																		// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_SEARCH_ANYTHING,			// wId
			WC_BUTTONW,								// lpwcClass
			FALSE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 3,	// iTop
			194,									// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_39FDDE4C_Anything_Readable__Slower_but_Undetectable_ ),	// pwcText
			_LEN_39FDDE4C,																// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		// ==== Thread Priority ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_GENERAL_SEARCH_THREAD_PRIORITY,	// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_SEARCH_RANGE_GROUP_HEIGHT,			// iTop
			MX_OD_W,								// dwWidth
			MX_THREAD_PRIORITY_GROUP_HEIGHT,		// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_33BECF37_Thread_Priority ),			// pwcText
			_LEN_33BECF37,											// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_OI_GENERAL_SEARCH_TP_NORMAL,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TP_L( 0 ),							// iLeft
			MX_SEARCH_RANGE_GROUP_HEIGHT + MX_GROUP_TOP,			// iTop
			60,										// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_33095CDA_Normal__Default_ ),			// pwcText
			_LEN_33095CDA,											// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_OI_GENERAL_SEARCH_TP_HIGH,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TP_L( 1 ),							// iLeft
			MX_SEARCH_RANGE_GROUP_HEIGHT + MX_GROUP_TOP,			// iTop
			60,										// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_51374B6B_High ),		// pwcText
			_LEN_51374B6B,							// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_OI_GENERAL_SEARCH_TP_CRIT,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TP_L( 2 ),							// iLeft
			MX_SEARCH_RANGE_GROUP_HEIGHT + MX_GROUP_TOP,			// iTop
			MX_CRIT_W,								// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_AE33799D_Critical ),	// pwcText
			_LEN_AE33799D,							// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		// ==== Buffer ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_GENERAL_SEARCH_BUFFER,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_BUFFER_GROUP_TOP,					// iTop
			MX_OD_W,								// dwWidth
			MX_BUFFER_GROUP_HEIGHT,					// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_36F6F5C4_Buffer ),	// pwcText
			_LEN_36F6F5C4,							// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_SEARCH_BUFFER_SIZE,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_BUFFER_GROUP_TOP + MX_GROUP_TOP + 2,	// iTop
			96,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_E2091EC1_Buffer_Size__Megabytes__ ),				// pwcText
			_LEN_E2091EC1,														// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_OI_GENERAL_SEARCH_BUFFER_SIZE_EDIT,	// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_W / 2,							// iLeft
			MX_BUFFER_GROUP_TOP + MX_GROUP_TOP,		// iTop
			(MX_OD_W - MX_GROUP_RIGHT) - (MX_OD_W / 2),																									// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_GROUP | ES_LEFT | ES_AUTOHSCROLL,																// dwStyle
			WS_EX_CLIENTEDGE,																															// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		// ==== Miscellaneous ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_GENERAL_SEARCH_MISC,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_MISC_GROUP_TOP,						// iTop
			MX_OD_W,								// dwWidth
			MX_MISC_GROUP_HEIGHT,					// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_F1050E0B_Miscellaneous ),				// pwcText
			_LEN_F1050E0B,											// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_SEARCH_POSTFLUSH,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_MISC_GROUP_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 0,				// iTop
			90,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_5FAC969F_Postflushing__More_RAM_ ),					// pwcText
			_LEN_5FAC969F,															// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_SEARCH_PRECACHE,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_MISC_GROUP_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 1,				// iTop
			90,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_4959D96B_Precaching__More_RAM_ ),						// pwcText
			_LEN_4959D96B,															// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_SEARCH_PAUSE,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_W / 2,							// iLeft
			MX_MISC_GROUP_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 1,				// iTop
			90,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_5C839ECF_Pause_Target_Process ),						// pwcText
			_LEN_5C839ECF,															// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_SEARCH_DOTS,				// wId
			WC_STATICW,								// lpwcClass
			FALSE,									// bEnabled
			FALSE,									// bActive
			4,										// iLeft
			MX_MISC_GROUP_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2,				// iTop
			MX_OD_W - 4 * 2,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_9D174B2D_________________________________________________________________________________________________________________________________________________________________________________________________________________ ),				// pwcText
			_LEN_9D174B2D,															// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_SEARCH_USE_EPSILON,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_MISC_GROUP_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2 + MX_DEF_STATIC_HEIGHT,									// iTop
			117,									// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_A4845F29_Use_Epsilon__Float_Double_Searches_ ),		// pwcText
			_LEN_A4845F29,															// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_SEARCH_SMART_EPS,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_W - MX_GROUP_RIGHT - 50,			// iLeft
			MX_MISC_GROUP_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2 + MX_DEF_STATIC_HEIGHT,									// iTop
			50,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_D6F9E0F3_Smart_Epsilon ),								// pwcText
			_LEN_D6F9E0F3,															// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_SEARCH_EPSILON,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			4,										// iLeft
			MX_MISC_GROUP_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2 + MX_DEF_STATIC_HEIGHT + MX_DEF_STATIC_HEIGHT + 2 + 2,		// iTop
			30 - 4,									// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_FEAAF2A5_Epsilon_ ),									// pwcText
			_LEN_FEAAF2A5,															// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_OI_GENERAL_SEARCH_EPSILON_EDIT,		// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			30,										// iLeft
			MX_MISC_GROUP_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2 + MX_DEF_STATIC_HEIGHT + MX_DEF_STATIC_HEIGHT + 2,			// iTop
			(MX_OD_W - MX_GROUP_RIGHT) - (30),		// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
	};
#undef MX_TP_L
#undef MX_CRIT_W
#undef MX_MISC_GROUP_HEIGHT
#undef MX_MISC_GROUP_TOP
#undef MX_BUFFER_GROUP_HEIGHT
#undef MX_BUFFER_GROUP_TOP
#undef MX_THREAD_PRIORITY_GROUP_HEIGHT
#undef MX_SEARCH_RANGE_GROUP_HEIGHT

	// Open-process section.
	LSW_WIDGET_LAYOUT COptionsLayout::m_wlOptionsOpenProc[] = {
		{
			MX_OPTIONS_OPENPROC,					// ltType
			MX_OI_OPEN_PROCESS,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_COORD,
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,															// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,																	// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_NONE,								// dwParentId
		},
		// ==== Show ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_OPEN_PROCESS_SHOW_GROUP,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_OD_W,								// dwWidth
			MX_GROUP_TOP + (MX_DEF_CHECK_HEIGHT * 7) + MX_DEF_STATIC_HEIGHT * 2 + MX_GROUP_BOTTOM,	// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_923C763F_Show ),		// pwcText
			_LEN_923C763F,							// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_PROCESS,				// wId
			WC_BUTTONW,								// lpwcClass
			FALSE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 0,	// iTop
			(MX_OD_W / 2 - MX_GROUP_LEFT),			// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_49A0210A_Process ),	// pwcText
			_LEN_49A0210A,							// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_PROCESSID,			// wId
			WC_BUTTONW,								// lpwcClass
			FALSE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 1,	// iTop
			(MX_OD_W / 2 - MX_GROUP_LEFT),			// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_C0D8DDA3_Process_ID ),// pwcText
			_LEN_C0D8DDA3,							// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_PATH,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2,	// iTop
			(MX_OD_W / 2 - MX_GROUP_LEFT),			// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_AB662431_Path ),		// pwcText
			_LEN_AB662431,							// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_WINDOWS,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 3,	// iTop
			(MX_OD_W / 2 - MX_GROUP_LEFT),			// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_2C5ABC07_Windows ),	// pwcText
			_LEN_2C5ABC07,							// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_CHILDWINDOWS,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT * 2,						// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 4,	// iTop
			(MX_OD_W / 2 - MX_GROUP_LEFT),			// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_4CFA0E3F_Include_Child_Windows ),	// pwcText
			_LEN_4CFA0E3F,										// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_PARENT,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 5,	// iTop
			(MX_OD_W / 2 - MX_GROUP_LEFT),			// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_3A226579_Parent ),	// pwcText
			_LEN_3A226579,							// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_X86,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 6,	// iTop
			(MX_OD_W / 2 - MX_GROUP_LEFT),			// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_6E9684E9_Mark_x86_Processes ),	// pwcText
			_LEN_6E9684E9,							// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_ALL,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_W / 2,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 0,	// iTop
			(MX_OD_W - MX_GROUP_RIGHT) - MX_OD_W / 2,			// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0355373D_All ),		// pwcText
			_LEN_0355373D,							// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_NONE,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_W / 2,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 1,	// iTop
			(MX_OD_W - MX_GROUP_RIGHT) - MX_OD_W / 2,			// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_DFA2AFF1_None ),		// pwcText
			_LEN_DFA2AFF1,							// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_OPEN_PROCESS_EXPLAIN,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 7,	// iTop
			MX_OD_W - MX_GROUP_LEFT - MX_GROUP_RIGHT,	// dwWidth
			MX_DEF_STATIC_HEIGHT * 2,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_CF10E6E7_Gathering_more_data_about_a_process_increases_the_risk_of_an_anti_cheat_being_triggered_ ),				// pwcText
			_LEN_CF10E6E7,																														// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
	};

#undef MX_OD_COORD
#undef MX_OD_H
#undef MX_OD_W
#undef MX_OD_T
#undef MX_OD_L
#undef MX_OPTIONS_LIST_W
#undef MX_OPTIONS_H
#undef MX_OPTIONS_W

	// == Functions.
	// Creates the Options dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	BOOL COptionsLayout::CreateOptionsDialog( CWidget * _pwParent, MX_OPTIONS * _poOptions ) {
		std::vector<std::string> sStrings;
		std::vector<std::wstring> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlOptionsDialog, MX_ELEMENTS( m_wlOptionsDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		plmLayout->m_poOptions = _poOptions;
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], MX_ELEMENTS( m_wlOptionsDialog ), _pwParent );
		plmLayout->m_poOptions = nullptr;	// Don't keep the pointer around.
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

	// Creates the general options page.
	CWidget * COptionsLayout::CreateGeneralPage( CWidget * _pwParent ) {
		return CreatePage( _pwParent, m_wlOptionsGeneral, MX_ELEMENTS( m_wlOptionsGeneral ) );
	}

	// Creates the open-process options page.
	CWidget * COptionsLayout::CreateOpenProcPage( CWidget * _pwParent ) {
		return CreatePage( _pwParent, m_wlOptionsOpenProc, MX_ELEMENTS( m_wlOptionsOpenProc ) );
	}

	// Creates the general search options page.
	CWidget * COptionsLayout::CreateGeneralSearchPage( CWidget * _pwParent ) {
		return CreatePage( _pwParent, m_wlOptionsGeneralSearch, MX_ELEMENTS( m_wlOptionsGeneralSearch ) );
	}

	// Default window-creation.
	CWidget * COptionsLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal ) {
		std::vector<std::string> sStrings;
		std::vector<std::wstring> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( _pwlLayout, _sTotal,
			vLayouts,
			sStringsW,
			sStrings );
		
		CWidget * pwWidget = lsw::CBase::LayoutManager()->CreateDialogX( &vLayouts[0], _sTotal, _pwParent );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( pwWidget ) {
			
			// Success.  Do stuff.
		}

		return pwWidget;
	}

}	// namespace mx

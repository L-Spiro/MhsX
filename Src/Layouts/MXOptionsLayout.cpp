#include "MXOptionsLayout.h"
#include "../Options/MXOptionsWindow.h"
#include "../System/MXSystem.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>

namespace mx {

#define MX_OPTIONS_W								(260 + 0)
#define MX_OPTIONS_H								(183 + 0)
#define MX_OPTIONS_LIST_W							(52)
#define MX_PADDING									4
#define MX_THIN_PADDING								2
#define MX_OD_L										(MX_OPTIONS_LIST_W + 5)
#define MX_OD_T										(2)
#define MX_OD_W										(MX_OPTIONS_W - MX_OD_L - MX_PADDING)
#define MX_OD_H										(MX_OPTIONS_H - 26)
#define MX_OD_COORD									MX_OD_L, MX_OD_T, MX_OD_W, MX_OD_H


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
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,		// dwStyleEx
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
#define MX_ROW_HEIGHT								(MX_DEF_COMBO_HEIGHT + 2)
#define MX_REFRESH_RATES_GROUP_HEIGHT				(MX_GROUP_TOP + (MX_ROW_HEIGHT * 4) + MX_GROUP_BOTTOM - 2)
#define MX_VIEW_SETTINGS_GROUP_HEIGHT				(MX_REFRESH_RATES_GROUP_HEIGHT + (MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2 + MX_GROUP_BOTTOM + MX_DEF_STATIC_HEIGHT/*29*/))
#define MX_TPS_W									54
#define MX_TPS_L									(MX_OD_W - MX_GROUP_RIGHT - MX_TPS_W)
#define MX_TPS_CB_L									(MX_OD_W / 2)
#define MX_TPS_CB_W									(MX_TPS_L - MX_TPS_CB_L - 2)
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
			MX_OI_GENERAL_REFRESH_FOUND_ADDRESSES_CB,							// wId
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
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2 + MX_GROUP_BOTTOM + MX_DEF_STATIC_HEIGHT/*29*/,										// dwHeight
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
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_VIEW_EXAMPLE,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_REFRESH_RATES_GROUP_HEIGHT + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2,		// iTop
			MX_OD_W - MX_GROUP_RIGHT - MX_GROUP_LEFT,									// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_4CEBDC18_Example__ ),	// pwcText
			_LEN_4CEBDC18,							// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},

		// ==== Format Settings ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_GENERAL_FORMAT_SETTINGS,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_VIEW_SETTINGS_GROUP_HEIGHT,			// iTop
			MX_OD_W,								// dwWidth
			MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 3 + MX_GROUP_BOTTOM,										// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_1CD6C0DD_Format_Settings ),													// pwcText
			_LEN_1CD6C0DD,																					// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_FORMAT_HEX,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_VIEW_SETTINGS_GROUP_HEIGHT + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 0,							// iTop
			MX_OD_W - MX_GROUP_RIGHT - MX_GROUP_LEFT,														// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8A119D1D_Use____0x____Prefix_for_Hexadecimal_Numbers__0x_____vs______h_ ),	// pwcText
			_LEN_8A119D1D,																					// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_FORMAT_OCT,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_VIEW_SETTINGS_GROUP_HEIGHT + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 1,							// iTop
			MX_OD_W - MX_GROUP_RIGHT - MX_GROUP_LEFT,														// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_612B8F87_Use____0o____Prefix_for_Octadecimal_Numbers__0o_____vs__0_____ ),	// pwcText
			_LEN_612B8F87,																					// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_GENERAL_FORMAT_SHORT_ENUMS,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_VIEW_SETTINGS_GROUP_HEIGHT + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2,							// iTop
			MX_OD_W - MX_GROUP_RIGHT - MX_GROUP_LEFT,														// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_7566E6A4_Shorten_Enums__IMAGE_SCN_MEM_LOCKED_vs__LOCKED_ ),	// pwcText
			_LEN_7566E6A4,																					// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},
		/*{
			LSW_LT_LABEL,							// ltType
			MX_OI_GENERAL_FORMAT_HEX_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT + 134,					// iLeft
			MX_VIEW_SETTINGS_GROUP_HEIGHT + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 0 + 1,	// iTop
			(MX_OD_W - MX_GROUP_RIGHT - MX_GROUP_LEFT) - 134,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_1DBE083A_0x_____vs______h ),	// pwcText
			_LEN_1DBE083A,							// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},*/
	};
#undef MX_TPS_CB_W
#undef MX_TPS_CB_L
#undef MX_TPS_L
#undef MX_TPS_W
#undef MX_ROW_HEIGHT
#undef MX_REFRESH_RATES_GROUP_HEIGHT

	// General search section.
#define MX_SEARCH_RANGE_GROUP_HEIGHT				(MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 4 + MX_GROUP_BOTTOM)
#define MX_THREAD_PRIORITY_GROUP_HEIGHT				(MX_GROUP_TOP + MX_DEF_RADIO_HEIGHT + MX_GROUP_BOTTOM)
#define MX_BUFFER_GROUP_TOP							(MX_SEARCH_RANGE_GROUP_HEIGHT + MX_THREAD_PRIORITY_GROUP_HEIGHT)
#define MX_BUFFER_GROUP_HEIGHT						(MX_GROUP_TOP + MX_DEF_EDIT_HEIGHT + MX_GROUP_BOTTOM)
#define MX_MISC_GROUP_TOP							(MX_BUFFER_GROUP_TOP + MX_BUFFER_GROUP_HEIGHT)
#define MX_MISC_GROUP_HEIGHT						(MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2 + MX_DEF_STATIC_HEIGHT + MX_DEF_STATIC_HEIGHT + 2 + MX_DEF_EDIT_HEIGHT + MX_GROUP_BOTTOM)
#define MX_CRIT_W									30
#define MX_TP_L( IDX )								((MX_OD_W - MX_GROUP_LEFT - MX_GROUP_RIGHT - MX_CRIT_W) * IDX / 2 + MX_GROUP_LEFT)
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
			MX_OD_W - MX_GROUP_LEFT - MX_GROUP_RIGHT,												// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_67CAB38F_MEM_IMAGE__Static_Data_is_Usually_Here_ ),					// pwcText
			_LEN_67CAB38F,																			// sTextLen
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
			MX_OD_W - MX_GROUP_LEFT - MX_GROUP_RIGHT,												// dwWidth
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
			MX_OD_W - MX_GROUP_LEFT - MX_GROUP_RIGHT,												// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_37D0905A_MEM_MAPPED__Data_Loaded_by_the_Target_Process__Slow_ ),		// pwcText
			_LEN_37D0905A,																			// sTextLen
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
			MX_OD_W - MX_GROUP_LEFT - MX_GROUP_RIGHT,												// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_39FDDE4C_Anything_Readable__Slower_but_Undetectable_ ),				// pwcText
			_LEN_39FDDE4C,																			// sTextLen
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
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
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
			FALSE,									// bEnabled
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
			FALSE,									// bEnabled
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
			nullptr,								// pwcText
			0,										// sTextLen
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
			MX_OD_W - MX_GROUP_RIGHT - 34,			// iLeft
			MX_MISC_GROUP_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2 + MX_DEF_STATIC_HEIGHT,									// iTop
			34,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_972D558F_Relative ),									// pwcText
			_LEN_972D558F,															// sTextLen
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

#undef MX_MISC_GROUP_HEIGHT
#undef MX_MISC_GROUP_TOP
#undef MX_BUFFER_GROUP_HEIGHT
#undef MX_BUFFER_GROUP_TOP
#undef MX_THREAD_PRIORITY_GROUP_HEIGHT
#undef MX_SEARCH_RANGE_GROUP_HEIGHT


#define MX_SEARCH_EX_BYTESWAP_GROUP_HEIGHT			(MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_GROUP_BOTTOM)
#define MX_SEARCH_EX_RANGE_GROUP_HEIGHT				(MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + MX_DEF_BUTTON_HEIGHT + MX_LEFT_ALIGN + MX_GROUP_BOTTOM)
	// Extended search section.
	LSW_WIDGET_LAYOUT COptionsLayout::m_wlOptionsSearchEx[] = {
		{
			MX_OPTIONS_SEARCHEX,					// ltType
			MX_OI_SEARCH_EX,						// wId
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
		// ==== Byteswapping ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_SEARCH_EX_BYTESWAP,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_OD_W,								// dwWidth
			MX_SEARCH_EX_BYTESWAP_GROUP_HEIGHT,		// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_4391F303_Search_Preprocessing ),																							// pwcText
			_LEN_4391F303,																																// sTextLen
			MX_OI_SEARCH_EX,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_SEARCH_EX_BYTESWAP_TYPE_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_GROUP_TOP + 2,						// iTop
			31,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0DD0DC2C_Byteswap_ ),	// pwcText
			_LEN_0DD0DC2C,							// sTextLen
			MX_OI_SEARCH_EX,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_SEARCH_EX_BYTESWAP_TYPE_COMBO,	// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT + 32,						// iLeft
			MX_GROUP_TOP,							// iTop
			MX_OD_W - 32 - MX_GROUP_LEFT - MX_GROUP_RIGHT,																								// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_SEARCH_EX,						// dwParentId
		},

		// ==== Range ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_SEARCH_EX_SEARCH_RANGE,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_SEARCH_EX_BYTESWAP_GROUP_HEIGHT,		// iTop
			MX_OD_W,								// dwWidth
			MX_SEARCH_EX_RANGE_GROUP_HEIGHT,		// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_638D307B_Search_Ranges ),																									// pwcText
			_LEN_638D307B,																																// sTextLen
			MX_OI_SEARCH_EX,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_SEARCH_EX_SEARCH_RANGE_LOW_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_SEARCH_EX_BYTESWAP_GROUP_HEIGHT + MX_GROUP_TOP,															// iTop
			(MX_OD_W - MX_GROUP_LEFT * 2) / 2 - (MX_LEFT_ALIGN),														// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_857372A6_From_ ),		// pwcText
			_LEN_857372A6,							// sTextLen
			MX_OI_SEARCH_EX,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_SEARCH_EX_SEARCH_RANGE_HIGH_LABEL,// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_W - MX_GROUP_LEFT - ((MX_OD_W - MX_GROUP_LEFT * 2) / 2) + MX_LEFT_ALIGN,								// iLeft
			MX_SEARCH_EX_BYTESWAP_GROUP_HEIGHT + MX_GROUP_TOP,															// iTop
			(MX_OD_W - MX_GROUP_LEFT * 2) / 2 - (MX_LEFT_ALIGN),														// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B09DF1A4_To_ ),		// pwcText
			_LEN_B09DF1A4,							// sTextLen
			MX_OI_SEARCH_EX,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_SEARCH_EX_SEARCH_RANGE_LOW_COMBO,	// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_SEARCH_EX_BYTESWAP_GROUP_HEIGHT + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT,									// iTop
			(MX_OD_W - MX_GROUP_LEFT * 2) / 2 - (MX_LEFT_ALIGN),														// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_SEARCH_EX,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_SEARCH_EX_SEARCH_RANGE_HIGH_COMBO,// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_W - MX_GROUP_LEFT - ((MX_OD_W - MX_GROUP_LEFT * 2) / 2) + MX_LEFT_ALIGN,								// iLeft
			MX_SEARCH_EX_BYTESWAP_GROUP_HEIGHT + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT,									// iTop
			(MX_OD_W - MX_GROUP_LEFT * 2) / 2 - (MX_LEFT_ALIGN),// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_SEARCH_EX,						// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_SEARCH_EX_SEARCH_RANGE_SET_BUTTON,// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_GROUP_LEFT,							// iLeft
			MX_SEARCH_EX_BYTESWAP_GROUP_HEIGHT + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + MX_LEFT_ALIGN,				// iTop
			MX_OD_W - MX_GROUP_LEFT * 2,			// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_A07EA06F_Snap_to_Full_Range ),		// pwcText
			_LEN_A07EA06F,							// sTextLen
			MX_OI_SEARCH_EX,						// dwParentId
		},
	};

#undef MX_SEARCH_EX_RANGE_GROUP_HEIGHT
#undef MX_SEARCH_EX_BYTESWAP_GROUP_HEIGHT
#define MX_SHOW_HEIGHT								(MX_GROUP_TOP + (MX_DEF_CHECK_HEIGHT * 7) + MX_DEF_STATIC_HEIGHT * 2 + MX_GROUP_BOTTOM)
#define MX_TITLE_BAR_HEIGHT							(MX_GROUP_TOP + (MX_DEF_CHECK_HEIGHT * 2) + MX_GROUP_BOTTOM)
#undef MX_CRIT_W
#define MX_CRIT_W									33
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
			MX_SHOW_HEIGHT,							// dwHeight
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

		// ==== Title Bar ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_OPEN_PROCESS_TITLE_BAR,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_SHOW_HEIGHT,							// iTop
			MX_OD_W,								// dwWidth
			MX_TITLE_BAR_HEIGHT,					// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_2111C006_Title_Bar ),	// pwcText
			_LEN_2111C006,							// sTextLen
			MX_OI_OPEN_PROCESS,						// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_PROCESS_NAME,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TP_L( 0 ),							// iLeft
			MX_SHOW_HEIGHT + MX_GROUP_TOP,			// iTop
			60,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_724BAFEB_Process_Name ),			// pwcText
			_LEN_724BAFEB,										// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_PROCESS_ID,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TP_L( 1 ),							// iLeft
			MX_SHOW_HEIGHT + MX_GROUP_TOP,			// iTop
			60,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_C0D8DDA3_Process_ID ),// pwcText
			_LEN_C0D8DDA3,							// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_TITLE_BAR_ALL,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TP_L( 2 ),							// iLeft
			MX_SHOW_HEIGHT + MX_GROUP_TOP,			// iTop
			MX_CRIT_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0355373D_All ),		// pwcText
			_LEN_0355373D,							// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_32_BIT_FLAG,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TP_L( 0 ),							// iLeft
			MX_SHOW_HEIGHT + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT,			// iTop
			60,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_C27BEE75_x86_Flag ),	// pwcText
			_LEN_C27BEE75,							// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_INSERT_RANDOM,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TP_L( 1 ),							// iLeft
			MX_SHOW_HEIGHT + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT,			// iTop
			60,										// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_3DA6AAF4_Insert_Random ),						// pwcText
			_LEN_3DA6AAF4,							// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_OPEN_PROCESS_TITLE_BAR_NONE,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_TP_L( 2 ),							// iLeft
			MX_SHOW_HEIGHT + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT,			// iTop
			MX_CRIT_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_DFA2AFF1_None ),		// pwcText
			_LEN_DFA2AFF1,							// sTextLen
			MX_OPTIONS_GENSEARCH,					// dwParentId
		},
	};

#undef MX_TP_L
#undef MX_CRIT_W
#undef MX_TITLE_BAR_HEIGHT
#undef MX_SHOW_HEIGHT

#define MX_HOTKEY_TOP_HEIGHT						48
#define MX_HOTKEY_TOP_WIDTH							151
#define MX_UPDOWN_BUTTON_WIDTHS						(((MX_OD_W - (MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING)) - (MX_THIN_PADDING)) / 2)
#define MX_OPTIONS_PARM_PART_HEIGHT					(MX_THIN_PADDING + MX_DEF_STATIC_HEIGHT + MX_DEF_EDIT_HEIGHT)
#define MX_HOTKEY_OPTIONS_PARMS_TOP					(MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_THIN_PADDING + MX_DEF_COMBO_HEIGHT + MX_THIN_PADDING * 2)
#define MX_HOTKEY_OPTIONS_GROUP_HIEGHT				(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_DEF_COMBO_HEIGHT + MX_THIN_PADDING + MX_DEF_COMBO_HEIGHT + MX_THIN_PADDING * 2 + (MX_OPTIONS_PARM_PART_HEIGHT * 3))
#define MX_HOTKEY_OPTIONS_LEFT						(MX_GROUP_LEFT)
#define MX_HOTKEY_OPTIONS_LEFT_WIDTH				(30)
#define MX_HOTKEY_OPTIONS_MOD_WIDTH					(17)
#define MX_HOTKEY_OPTIONS_COMBO_LEFT				(MX_HOTKEY_OPTIONS_LEFT + MX_HOTKEY_OPTIONS_LEFT_WIDTH)
	// Hotkeys section.
	LSW_WIDGET_LAYOUT COptionsLayout::m_wlOptionsHotkeys[] = {
		{
			MX_OPTIONS_HOTKEYS,						// ltType
			MX_OI_HOTKEYS,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_COORD,
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,							// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_NONE,								// dwParentId
		},

		{
			LSW_LT_LISTBOX,							// ltType
			MX_OI_HOTKEYS_LIST,						// wId
			WC_LISTBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_HOTKEY_TOP_WIDTH,					// dwWidth
			MX_HOTKEY_TOP_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_EXTENDEDSEL,			// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,							// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_2C6375EC_Hold_Ctrl_while_selecting_to_copy_hotkey_settings_,														// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_HOTKEYS_ADD,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,	// iLeft
			0,										// iTop
			MX_OD_W - (MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING),													// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_53AE02A5_New ),		// pwcText
			_LEN_53AE02A5,							// sTextLen
			MX_OI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_69EAC485_Adds_a_new_hotkey_using_the_current_settings_,										// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_HOTKEYS_DELETE,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,	// iLeft
			MX_DEF_BUTTON_HEIGHT + MX_PADDING,		// iTop
			MX_OD_W - (MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING),													// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_3DBE79B1_Delete ),	// pwcText
			_LEN_3DBE79B1,							// sTextLen
			MX_OI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_E0769D45_Deletes_all_selected_hotkeys_,														// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},

		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_HOTKEYS_UP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,	// iLeft
			MX_HOTKEY_TOP_HEIGHT - MX_DEF_BUTTON_HEIGHT,														// iTop
			MX_UPDOWN_BUTTON_WIDTHS,				// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_D610CAD2_Up ),		// pwcText
			_LEN_D610CAD2,							// sTextLen
			MX_OI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_F88A30AA_Moves_all_selected_hotkeys_up__The_order_can_matter_in_methods_that_allow_duplicate_hotkeys_,														// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_HOTKEYS_DOWN,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING * 2 + MX_UPDOWN_BUTTON_WIDTHS,								// iLeft
			MX_HOTKEY_TOP_HEIGHT - MX_DEF_BUTTON_HEIGHT,														// iTop
			MX_UPDOWN_BUTTON_WIDTHS,				// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_BCCD3F05_Down ),		// pwcText
			_LEN_BCCD3F05,							// sTextLen
			MX_OI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_3E151175_Moves_all_selected_hotkeys_down__The_order_can_matter_in_methods_that_allow_duplicate_hotkeys_,													// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},

		// ==== Hotkey Options ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_HOTKEYS_OPTIONS_GROUP,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_HOTKEY_TOP_HEIGHT,					// iTop
			MX_OD_W,								// dwWidth
			MX_HOTKEY_OPTIONS_GROUP_HIEGHT,			// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_CE3DF015_Hotkey_Options ),														// pwcText
			_LEN_CE3DF015,							// sTextLen
			MX_OI_HOTKEYS,						// dwParentId
		},
		
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_HOTKEYS_OPTIONS_KEY_LABEL,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_LEFT,					// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + 2,// iTop
			MX_HOTKEY_OPTIONS_LEFT_WIDTH,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_1BBC12E6_Key_ ),		// pwcText
			_LEN_1BBC12E6,							// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_HOTKEYS_OPTIONS_KEY_BUTTON,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP,	// iTop
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),//(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_MOD_WIDTH) - MX_HOTKEY_OPTIONS_COMBO_LEFT - MX_THIN_PADDING,
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_971A9E73_Press_to_set_hotkey_ ),													// pwcText
			_LEN_971A9E73,																						// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_OI_HOTKEYS_OPTIONS_METHOD_LABEL,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,																// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + 2,//MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + 2 + MX_DEF_COMBO_HEIGHT + MX_THIN_PADDING,					// iTop
			MX_OD_W - (MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING + MX_GROUP_LEFT),									// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_965F37C1_Method_ ),	// pwcText
			_LEN_965F37C1,							// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_HOTKEYS_OPTIONS_METHOD_COMBO,		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,																// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + 2 + MX_THIN_PADDING + MX_DEF_STATIC_HEIGHT,//MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + 2 + MX_DEF_COMBO_HEIGHT + MX_THIN_PADDING + MX_DEF_STATIC_HEIGHT,
			MX_OD_W - (MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING + MX_GROUP_LEFT),									// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},

		/*{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_HOTKEYS_OPTIONS_KEY_COMBO,		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP,	// iTop
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_MOD_WIDTH) - MX_HOTKEY_OPTIONS_COMBO_LEFT - MX_THIN_PADDING,
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},*/
		
		/*{
			LSW_LT_LABEL,							// ltType
			MX_OI_HOTKEYS_OPTIONS_MOD_LABEL,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_MOD_WIDTH + MX_THIN_PADDING,									// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + 2,// iTop
			MX_HOTKEY_OPTIONS_MOD_WIDTH,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_CC2CA4F0_Mod_ ),		// pwcText
			_LEN_CC2CA4F0,							// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_HOTKEYS_OPTIONS_MOD_COMBO,		// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,	// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP,	// iTop
			MX_OD_W - MX_HOTKEY_TOP_WIDTH - MX_GROUP_LEFT - MX_THIN_PADDING,
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},*/

		{
			LSW_LT_LABEL,							// ltType
			MX_OI_HOTKEYS_OPTIONS_FUNCTION_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_LEFT,					// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + 2 + MX_DEF_COMBO_HEIGHT + MX_THIN_PADDING,					// iTop
			MX_HOTKEY_OPTIONS_LEFT_WIDTH,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_CE5D569B_Function_ ),	// pwcText
			_LEN_CE5D569B,							// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_HOTKEYS_OPTIONS_FUNCTION_COMBO,	// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_THIN_PADDING,// iTop
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},


		


		{
			LSW_LT_LABEL,							// ltType
			MX_OI_HOTKEYS_OPTIONS_PARM1_DESC_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_OPTIONS_PARMS_TOP + (MX_OPTIONS_PARM_PART_HEIGHT * 0),									// iTop
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_12A629FB_Parameter_Not_Used ),													// pwcText
			_LEN_12A629FB,							// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_HOTKEYS_OPTIONS_PARM1_LABEL,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_LEFT,					// iLeft
			MX_HOTKEY_OPTIONS_PARMS_TOP + MX_DEF_STATIC_HEIGHT + 2 + (MX_OPTIONS_PARM_PART_HEIGHT * 0),					// iTop
			MX_HOTKEY_OPTIONS_LEFT_WIDTH,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0F53EEA7_Parm_1_ ),	// pwcText
			_LEN_0F53EEA7,							// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_HOTKEYS_OPTIONS_PARM1_EDIT,		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_OPTIONS_PARMS_TOP + MX_DEF_STATIC_HEIGHT + (MX_OPTIONS_PARM_PART_HEIGHT * 0),						// iTop
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_OI_HOTKEYS_OPTIONS_PARM2_DESC_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_OPTIONS_PARMS_TOP + (MX_OPTIONS_PARM_PART_HEIGHT * 1),
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_12A629FB_Parameter_Not_Used ),													// pwcText
			_LEN_12A629FB,							// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_HOTKEYS_OPTIONS_PARM2_LABEL,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_LEFT,					// iLeft
			MX_HOTKEY_OPTIONS_PARMS_TOP + MX_DEF_STATIC_HEIGHT + 2 + (MX_OPTIONS_PARM_PART_HEIGHT * 1),
			MX_HOTKEY_OPTIONS_LEFT_WIDTH,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_247EBD64_Parm_2_ ),	// pwcText
			_LEN_247EBD64,							// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_HOTKEYS_OPTIONS_PARM2_EDIT,		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_OPTIONS_PARMS_TOP + MX_DEF_STATIC_HEIGHT + (MX_OPTIONS_PARM_PART_HEIGHT * 1),
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_OI_HOTKEYS_OPTIONS_PARM3_DESC_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_OPTIONS_PARMS_TOP + (MX_OPTIONS_PARM_PART_HEIGHT * 2),
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_12A629FB_Parameter_Not_Used ),													// pwcText
			_LEN_12A629FB,							// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_HOTKEYS_OPTIONS_PARM3_LABEL,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_LEFT,					// iLeft
			MX_HOTKEY_OPTIONS_PARMS_TOP + MX_DEF_STATIC_HEIGHT + 2 + (MX_OPTIONS_PARM_PART_HEIGHT * 2),
			MX_HOTKEY_OPTIONS_LEFT_WIDTH,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_3D658C25_Parm_3_ ),	// pwcText
			_LEN_3D658C25,							// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_HOTKEYS_OPTIONS_PARM3_EDIT,		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_OPTIONS_PARMS_TOP + MX_DEF_STATIC_HEIGHT + (MX_OPTIONS_PARM_PART_HEIGHT * 2),
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_HOTKEYS,							// dwParentId
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_OI_HOTKEYS_OPTIONS_WARNING_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,																// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + 2 + MX_THIN_PADDING + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + MX_THIN_PADDING,
			MX_OD_W - (MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING + MX_GROUP_LEFT),									// dwWidth
			24,										// dwHeight
			MX_STATICSTYLE | SS_BITMAP | SS_CENTERIMAGE,				// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_HOTKEYS,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_EF7346DB_The____Hotkey____method_does_not_allow_duplicate_hotkeys_,							// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},

	};

#undef MX_HOTKEY_OPTIONS_LEFT_WIDTH
#undef MX_HOTKEY_OPTIONS_COMBO_LEFT
#undef MX_HOTKEY_OPTIONS_MOD_WIDTH
#undef MX_HOTKEY_OPTIONS_GROUP_HIEGHT
#undef MX_HOTKEY_OPTIONS_PARMS_TOP
#undef MX_OPTIONS_PARM_PART_HEIGHT

#define MX_HOTKEY_OPTIONS_LEFT_WIDTH				(40)
#define MX_HOTKEY_OPTIONS_COMBO_LEFT				(MX_HOTKEY_OPTIONS_LEFT + MX_HOTKEY_OPTIONS_LEFT_WIDTH)
#define MX_OPTIONS_PARM_PART_HEIGHT					(MX_THIN_PADDING + MX_DEF_EDIT_HEIGHT)
#define MX_PROGRAMS_OPTIONS_GROUP_HIEGHT			(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_THIN_PADDING * 0 + (MX_OPTIONS_PARM_PART_HEIGHT * 4) + MX_DEF_COMBO_HEIGHT)

	// Programs section.
	LSW_WIDGET_LAYOUT COptionsLayout::m_wlOptionsPrograms[] = {
		{
			MX_OPTIONS_PROGRAMS,					// ltType
			MX_OI_PROGRAMS,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OD_COORD,
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,										// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,												// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_NONE,								// dwParentId
		},

		{
			LSW_LT_LISTBOX,							// ltType
			MX_OI_PROGRAMS_LIST,					// wId
			WC_LISTBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_HOTKEY_TOP_WIDTH,					// dwWidth
			MX_HOTKEY_TOP_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,							// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_PROGRAMS,							// dwParentId

			LSW_NO_SIZE_EXP,
			nullptr, 0,								// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_PROGRAMS_ADD,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,	// iLeft
			0,										// iTop
			MX_OD_W - (MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING),																		// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_53AE02A5_New ),		// pwcText
			_LEN_53AE02A5,							// sTextLen
			MX_OI_PROGRAMS,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_C2E8ADD2_Adds_a_new_program_using_default_settings_,																// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_PROGRAMS_DELETE,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,	// iLeft
			MX_DEF_BUTTON_HEIGHT + MX_PADDING,		// iTop
			MX_OD_W - (MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING),																		// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_3DBE79B1_Delete ),	// pwcText
			_LEN_3DBE79B1,							// sTextLen
			MX_OI_PROGRAMS,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_B4FF749D_Deletes_all_selected_programs_,																			// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},

		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_PROGRAMS_UP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,	// iLeft
			MX_HOTKEY_TOP_HEIGHT - MX_DEF_BUTTON_HEIGHT,																			// iTop
			MX_UPDOWN_BUTTON_WIDTHS,				// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_D610CAD2_Up ),		// pwcText
			_LEN_D610CAD2,							// sTextLen
			MX_OI_PROGRAMS,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_74F2386F_Moves_all_selected_programs_up_,																		// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_PROGRAMS_DOWN,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING * 2 + MX_UPDOWN_BUTTON_WIDTHS,													// iLeft
			MX_HOTKEY_TOP_HEIGHT - MX_DEF_BUTTON_HEIGHT,																			// iTop
			MX_UPDOWN_BUTTON_WIDTHS,				// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_BCCD3F05_Down ),		// pwcText
			_LEN_BCCD3F05,							// sTextLen
			MX_OI_PROGRAMS,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_9A810ED9_Moves_all_selected_programs_down_,																		// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},


		// ==== Program Options ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OI_PROGRAMS_OPTIONS_GROUP,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_HOTKEY_TOP_HEIGHT,					// iTop
			MX_OD_W,								// dwWidth
			MX_PROGRAMS_OPTIONS_GROUP_HIEGHT,		// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_69D7866D_Program_Options ),																			// pwcText
			_LEN_69D7866D,							// sTextLen
			MX_OI_PROGRAMS,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_OI_PROGRAMS_OPTIONS_NAME_LABEL,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_LEFT,					// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_DEF_EDIT_HEIGHT-MX_DEF_STATIC_HEIGHT) / 2 + (MX_OPTIONS_PARM_PART_HEIGHT * 0),// iTop
			MX_HOTKEY_OPTIONS_LEFT_WIDTH,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_3CF29F70_Name_ ),		// pwcText
			_LEN_3CF29F70,							// sTextLen
			MX_OI_PROGRAMS,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_OI_PROGRAMS_OPTIONS_NAME_EDIT,		// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_OPTIONS_PARM_PART_HEIGHT * 0),												// iTop
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_PROGRAMS,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OI_PROGRAMS_OPTIONS_HIDDEN_CHECK,	// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,	// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_DEF_EDIT_HEIGHT-MX_DEF_CHECK_HEIGHT) / 2,										// iTop
			MX_OD_W - (MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING) - MX_GROUP_LEFT,														// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8FF1EC8B_Hidden ),	// pwcText
			_LEN_8FF1EC8B,							// sTextLen
			MX_OI_PROGRAMS,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_OI_PROGRAMS_OPTIONS_PROGRAM_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_LEFT,					// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_DEF_EDIT_HEIGHT-MX_DEF_STATIC_HEIGHT) / 2 + (MX_OPTIONS_PARM_PART_HEIGHT * 1),// iTop
			MX_HOTKEY_OPTIONS_LEFT_WIDTH,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_073FFE27_Program_ ),	// pwcText
			_LEN_073FFE27,							// sTextLen
			MX_OI_PROGRAMS,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_OI_PROGRAMS_OPTIONS_PROGRAM_EDIT,	// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_OPTIONS_PARM_PART_HEIGHT * 1),												// iTop
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_PROGRAMS,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_PROGRAMS_OPTIONS_PROGRAM_BUTTON,	// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,	// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_OPTIONS_PARM_PART_HEIGHT * 1),												// iTop
			MX_UPDOWN_BUTTON_WIDTHS,				// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			L"…",									// pwcText
			0,										// sTextLen
			MX_OI_PROGRAMS,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_F5BE626B_Find_the_program_to_launch_,																			// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_OI_PROGRAMS_OPTIONS_ARGUMENTS_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_LEFT,					// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_DEF_EDIT_HEIGHT-MX_DEF_STATIC_HEIGHT) / 2 + (MX_OPTIONS_PARM_PART_HEIGHT * 2),// iTop
			MX_HOTKEY_OPTIONS_LEFT_WIDTH,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_877EEE17_Arguments_ ),// pwcText
			_LEN_877EEE17,							// sTextLen
			MX_OI_PROGRAMS,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_OI_PROGRAMS_OPTIONS_ARGUMENTS_EDIT,	// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_OPTIONS_PARM_PART_HEIGHT * 2),												// iTop
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_PROGRAMS,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_OI_PROGRAMS_OPTIONS_WORKINGDIR_LABEL,// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_LEFT,					// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_DEF_EDIT_HEIGHT-MX_DEF_STATIC_HEIGHT) / 2 + (MX_OPTIONS_PARM_PART_HEIGHT * 3),// iTop
			MX_HOTKEY_OPTIONS_LEFT_WIDTH,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_94F83C03_Working_Dir_ ),																				// pwcText
			_LEN_94F83C03,							// sTextLen
			MX_OI_PROGRAMS,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_OI_PROGRAMS_OPTIONS_WORKINGDIR_EDIT,	// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_OPTIONS_PARM_PART_HEIGHT * 3),												// iTop
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_PROGRAMS,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_PROGRAMS_OPTIONS_WORKINGDIR_BUTTON,																				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_TOP_WIDTH + MX_THIN_PADDING,	// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_OPTIONS_PARM_PART_HEIGHT * 3),												// iTop
			MX_UPDOWN_BUTTON_WIDTHS,				// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			L"…",									// pwcText
			0,										// sTextLen
			MX_OI_PROGRAMS,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_66D72F7C_Browse_to_the_desired_working_directory_,																// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},

		{
			LSW_LT_COMBOBOX,						// ltType
			MX_OI_PROGRAMS_OPTIONS_SHOW_CMD_COMBO,	// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_HOTKEY_OPTIONS_COMBO_LEFT,			// iLeft
			MX_HOTKEY_TOP_HEIGHT + MX_GROUP_TOP + (MX_OPTIONS_PARM_PART_HEIGHT * 4),												// iTop
			(MX_HOTKEY_TOP_WIDTH - MX_HOTKEY_OPTIONS_COMBO_LEFT),
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_PROGRAMS,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_497C0C91_The_flags_that_specify_how_an_application_is_to_be_displayed_when_it_is_opened_,						// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
	};

#undef MX_HOTKEY_OPTIONS_COMBO_LEFT
#undef MX_HOTKEY_OPTIONS_LEFT_WIDTH
#undef MX_HOTKEY_OPTIONS_LEFT
#undef MX_UPDOWN_BUTTON_WIDTHS
#undef MX_HOTKEY_TOP_WIDTH
#undef MX_HOTKEY_TOP_HEIGHT
#undef MX_OPTIONS_PARM_PART_HEIGHT
#undef MX_PROGRAMS_OPTIONS_GROUP_HIEGHT


#undef MX_OD_COORD
#undef MX_OD_H
#undef MX_OD_W
#undef MX_OD_T
#undef MX_OD_L
#undef MX_THIN_PADDING
#undef MX_PADDING
#undef MX_OPTIONS_LIST_W
#undef MX_OPTIONS_H
#undef MX_OPTIONS_W
	

	// == Functions.
	// Creates the Options dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	BOOL COptionsLayout::CreateOptionsDialog( CWidget * _pwParent, MX_OPTIONS * _poOptions, int32_t _i32Page ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlOptionsDialog, MX_ELEMENTS( m_wlOptionsDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		COptionsWindow::MX_PARMS pParms = {
			_poOptions,
			_i32Page
		};

		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], MX_ELEMENTS( m_wlOptionsDialog ), _pwParent, reinterpret_cast<uint64_t>(&pParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

	// Creates the general options page.
	CWidget * COptionsLayout::CreateGeneralPage( CWidget * _pwParent, MX_OPTIONS * _poOptions ) {
		return CreatePage( _pwParent, m_wlOptionsGeneral, MX_ELEMENTS( m_wlOptionsGeneral ), _poOptions );
	}

	// Creates the open-process options page.
	CWidget * COptionsLayout::CreateOpenProcPage( CWidget * _pwParent, MX_OPTIONS * _poOptions ) {
		return CreatePage( _pwParent, m_wlOptionsOpenProc, MX_ELEMENTS( m_wlOptionsOpenProc ), _poOptions );
	}

	// Creates the general search options page.
	CWidget * COptionsLayout::CreateGeneralSearchPage( CWidget * _pwParent, MX_OPTIONS * _poOptions ) {
		return CreatePage( _pwParent, m_wlOptionsGeneralSearch, MX_ELEMENTS( m_wlOptionsGeneralSearch ), _poOptions );
	}

	// Creates the extended search options page.
	CWidget * COptionsLayout::CreateSearchExPage( CWidget * _pwParent, MX_OPTIONS * _poOptions ) {
		return CreatePage( _pwParent, m_wlOptionsSearchEx, MX_ELEMENTS( m_wlOptionsSearchEx ), _poOptions );
	}

	// Creates the hotkeys options page.
	CWidget * COptionsLayout::CreateHotkeysPage( CWidget * _pwParent, MX_OPTIONS * _poOptions ) {
		return CreatePage( _pwParent, m_wlOptionsHotkeys, MX_ELEMENTS( m_wlOptionsHotkeys ), _poOptions );
	}

		// Creates the programs options page.
	CWidget * COptionsLayout::CreateProgramsPage( CWidget * _pwParent, MX_OPTIONS * _poOptions ) {
		return CreatePage( _pwParent, m_wlOptionsPrograms, MX_ELEMENTS( m_wlOptionsPrograms ), _poOptions );
	}

	// Default window-creation.
	CWidget * COptionsLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, MX_OPTIONS * _poOptions ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( _pwlLayout, _sTotal,
			vLayouts,
			sStringsW,
			sStrings );
		
		CWidget * pwWidget = lsw::CBase::LayoutManager()->CreateDialogX( &vLayouts[0], _sTotal, _pwParent, reinterpret_cast<uint64_t>(_poOptions) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( pwWidget ) {
			
			// Success.  Do stuff.
		}

		return pwWidget;
	}

}	// namespace mx

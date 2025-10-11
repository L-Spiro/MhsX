#include "MXFoundAddressEditLayout.h"
#include "../MemHack/MXMemHack.h"
#include "../FoundAddresses/MXFoundAddressEditWindow.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>

//#define LSN_SEQUE_LOOP_1_4_L								LSN_EVEN_DIVIDE_EX( LSN_INNER_GROUP_W - LSN_GROUP_LEFT - LSN_GROUP_RIGHT, LSN_INNER_GROUP_L + LSN_GROUP_LEFT, 4, 0, LSN_LEFT_JUST )

namespace mx {

#define MX_MAIN_LABEL_W										50
#define MX_MAIN_EDIT_W										98
#define MX_SECTION1_W										(MX_MAIN_LABEL_W + MX_LEFT_JUST + MX_MAIN_EDIT_W)
#define MX_INNER_W											(MX_SECTION1_W + MX_LEFT_JUST + MX_SECTION1_W)
#define MX_INNER_GROUP_W									(MX_GROUP_LEFT + MX_INNER_W + MX_GROUP_LEFT)
#define MX_EDIT_W											(MX_GROUP_L + MX_INNER_GROUP_W + MX_GROUP_L)
#define MX_MID_L											(MX_SECTION1_L + MX_SECTION1_W + MX_LEFT_JUST)

#define MX_GROUP_L											MX_LEFT_JUST
#define MX_SECTION1_L										(MX_GROUP_L + MX_GROUP_LEFT)
#define MX_SECTION2_L										(MX_SECTION1_L + MX_MAIN_LABEL_W + MX_LEFT_JUST)

#define MX_GEN_GROUP_T										MX_TOP_JUST
#define MX_GEN_GROUP_H										(MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_GROUP_BOTTOM + MX_TOP_JUST)

#define MX_VAL_GROUP_T										(MX_GEN_GROUP_T + MX_GEN_GROUP_H)
#define MX_VAL_GROUP_H										(MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) * 2 + MX_GROUP_BOTTOM + MX_TOP_JUST)

#define MX_QL_GROUP_T										(MX_VAL_GROUP_T + MX_VAL_GROUP_H)
#define MX_QL_GROUP_H										(MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_GROUP_BOTTOM + MX_TOP_JUST + MX_TOP_JUST + MX_VWL_GROUP_H)

#define MX_VWL_GROUP_T										(MX_QL_GROUP_T + MX_GROUP_TOP + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST))
#define MX_VWL_GROUP_H										(MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_GROUP_BOTTOM + MX_TOP_JUST + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST))

#define MX_INFO_GROUP_T										(MX_QL_GROUP_T + MX_QL_GROUP_H)
#define MX_INFO_GROUP_H										(MX_GROUP_TOP + MX_DEF_EDIT_HEIGHT + (MX_TOP_JUST + MX_DEF_STATIC_HEIGHT * 6) + MX_GROUP_BOTTOM + MX_TOP_JUST)

#define MX_EDIT_H											(MX_INFO_GROUP_T + MX_INFO_GROUP_H + MX_TOP_JUST)

	// == Members.
	// The layout for the edit dialog.
	LSW_WIDGET_LAYOUT CFoundAddressEditLayout::m_wlMainDialog[] = {
		{
			MX_FOUND_ADDRESSES_EDIT,						// ltType
			MX_FAEI_DIALOG,									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			MX_EDIT_W,										// dwWidth
			MX_EDIT_H,										// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,								// dwStyleEx
			MX_MAKE_WCHAR( _T_7AE3FDE1_Modify_Entry ),		// pwcText
			_LEN_7AE3FDE1,									// sTextLen
			MX_FAEI_NONE,									// dwParentId
		},
		{
			LSW_LT_TAB,										// ltType
			MX_FAEI_TAB,									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			MX_EDIT_W,										// dwWidth
			MX_EDIT_H - 26,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,				// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_DIALOG,									// dwParentId
		},

		{
			LSW_LT_BUTTON,									// ltType
			MX_FAEI_OK,										// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			MX_LEFT_JUST,									// iLeft
			MX_EDIT_H - MX_DEF_BUTTON_HEIGHT - MX_TOP_JUST,	// iTop
			MX_DEF_BUTTON_WIDTH,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,							// dwHeight
			MX_DEFBUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,												// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),				// pwcText
			_LEN_D736D92D,									// sTextLen
			MX_FAEI_DIALOG,									// dwParentId

			MX_LOCK_LEFT,									// pcLeftSizeExp
			nullptr, 0,										// pcRightSizeExp
			nullptr, 0,										// pcTopSizeExp
			MX_LOCK_BOTTOM,									// pcBottomSizeExp
			MX_FIXED_WIDTH,									// pcWidthSizeExp
			MX_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,									// ltType
			MX_FAEI_CANCEL,									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_EDIT_W - MX_DEF_BUTTON_WIDTH * 1 - MX_LEFT_JUST,		// iLeft
			MX_EDIT_H - MX_DEF_BUTTON_HEIGHT - MX_TOP_JUST,			// iTop
			MX_DEF_BUTTON_WIDTH,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,							// dwHeight
			MX_BUTTONSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,												// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),			// pwcText
			_LEN_51BAC044,									// sTextLen
			MX_FAEI_DIALOG,									// dwParentId

			nullptr, 0,										// pcLeftSizeExp
			MX_LOCK_RIGHT,									// pcRightSizeExp
			nullptr, 0,										// pcTopSizeExp
			MX_LOCK_BOTTOM,									// pcBottomSizeExp
			MX_FIXED_WIDTH,									// pcWidthSizeExp
			MX_FIXED_HEIGHT,								// pcHeightSizeExp
		},
	};

	// General section.
	LSW_WIDGET_LAYOUT CFoundAddressEditLayout::m_wlOptionsGeneral[] = {
		{
			MX_FOUND_ADDRESSES_MAIN_PAGE,					// ltType
			MX_FAEI_GENERAL_PAGE,							// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			MX_EDIT_W,										// dwWidth
			MX_EDIT_H,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL,															// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CONTROLPARENT,																	// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_NONE,									// dwParentId
		},
		// ==== General ==== //
		{
			LSW_LT_GROUPBOX,								// ltType
			MX_FAEI_GENERAL_GROUP,							// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_GROUP_L,										// iLeft
			MX_GEN_GROUP_T,									// iTop
			MX_INNER_GROUP_W,								// dwWidth
			MX_GEN_GROUP_H,									// dwHeight
			MX_GROUPSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_01940FD6_General ),			// pwcText
			_LEN_01940FD6,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_GENERAL_DESC_LABEL,						// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION1_L,									// iLeft
			(MX_GEN_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),										// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_DE5F8C7A_Description_ ),		// pwcText
			_LEN_DE5F8C7A,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_FAEI_GENERAL_DESC_COMBO,						// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION2_L,									// iLeft
			MX_GEN_GROUP_T + MX_GROUP_TOP,					// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			MX_FAEI_GENERAL_ARRAY_CHECK,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_MID_L,										// iLeft
			(MX_GEN_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_CHECK_HEIGHT) >> 1),										// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_CHECK_HEIGHT,							// dwHeight
			MX_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_3CC8FF2A_Add_Array_Indices ),	// pwcText
			_LEN_3CC8FF2A,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COLORBUTTON,								// ltType
			MX_FAEI_GENERAL_COLOR_BUTTON,					// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_MID_L + MX_MAIN_EDIT_W + MX_LEFT_JUST,		// iLeft
			(MX_GEN_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_BUTTON_HEIGHT) >> 1),										// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_BUTTON_HEIGHT,							// dwHeight
			MX_BUTTONSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_A79767ED_Color ),				// pwcText
			_LEN_A79767ED,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},

		// ==== Value ==== //
		{
			LSW_LT_GROUPBOX,								// ltType
			MX_FAEI_VALUE_GROUP,							// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_GROUP_L,										// iLeft
			MX_VAL_GROUP_T,									// iTop
			MX_INNER_GROUP_W,								// dwWidth
			MX_VAL_GROUP_H,									// dwHeight
			MX_GROUPSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_DCB67730_Value ),				// pwcText
			_LEN_DCB67730,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_VALUE_CUR_VAL_LABEL,					// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION1_L,									// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),										// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_E286B290_Current_Value_ ),	// pwcText
			_LEN_E286B290,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_FAEI_VALUE_CUR_VAL_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION2_L,									// iLeft
			MX_VAL_GROUP_T + MX_GROUP_TOP,					// iTop
			MX_INNER_W - MX_SECTION2_L + MX_SECTION1_L,		// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_VALUE_CUR_VAL_DESC_LABEL,				// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION2_L,									// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT),
			MX_INNER_W - MX_SECTION2_L + MX_SECTION1_L,		// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_A4774F9B_If_not_locked__this_value_will_be_written_to_the_target_process_once_when_you_hit_OK_ ),			// pwcText
			_LEN_A4774F9B,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},

		// Type.
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_VALUE_TYPE_LABEL,						// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION1_L,									// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_97F15F00_Type_ ),				// pwcText
			_LEN_97F15F00,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_FAEI_VALUE_TYPE_COMBO,						// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION2_L,									// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST),
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE_LIST,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_FAEI_VALUE_TYPE_STRING_COMBO,				// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_MID_L,										// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST),
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE_LIST,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		//{
		//	LSW_LT_COMBOBOX,								// ltType
		//	MX_FAEI_VALUE_TYPE_DATA_COMBO,					// wId
		//	WC_COMBOBOXW,									// lpwcClass
		//	TRUE,											// bEnabled
		//	FALSE,											// bActive
		//	MX_MID_L,										// iLeft
		//	(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST),
		//	MX_MAIN_EDIT_W,									// dwWidth
		//	MX_DEF_COMBO_HEIGHT,							// dwHeight
		//	MX_COMBOSTYLE_LIST,								// dwStyle
		//	0,												// dwStyleEx
		//	nullptr,										// pwcText
		//	0,												// sTextLen
		//	MX_FAEI_GENERAL_PAGE,							// dwParentId
		//},
		{
			LSW_LT_CHECK,									// ltType
			MX_FAEI_VALUE_TYPE_SHOW_AS_HEX_CHECK,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_MID_L + MX_MAIN_EDIT_W + MX_LEFT_JUST,		// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_CHECK_HEIGHT) >> 1),
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,							// dwHeight
			MX_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_A0692B77_Hexadecimal ),		// pwcText
			_LEN_A0692B77,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			MX_FAEI_VALUE_TYPE_SCIENTIFIC_CHECK,			// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_MID_L + MX_MAIN_EDIT_W + MX_LEFT_JUST,		// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_CHECK_HEIGHT) >> 1),
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,							// dwHeight
			MX_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_9631F7BC_Scientific ),		// pwcText
			_LEN_9631F7BC,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			MX_FAEI_VALUE_TYPE_ESCAPE_CHECK,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_MID_L + MX_MAIN_EDIT_W + MX_LEFT_JUST,		// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_CHECK_HEIGHT) >> 1),
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,							// dwHeight
			MX_CHECKSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_2244B5B8_Escape ),			// pwcText
			_LEN_2244B5B8,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		

		// Array Length.
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_VALUE_ARRAY_LEN_LABEL,					// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION1_L,									// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) * 2 + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_F9B2FD80_Array_Length_ ),		// pwcText
			_LEN_F9B2FD80,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_FAEI_VALUE_ARRAY_LEN_COMBO,					// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION2_L,									// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) * 2 + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST),
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},

		// Array Stride.
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_VALUE_ARRAY_STRIDE_LABEL,				// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_MID_L,										// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) * 2 + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_4CE91014_Array_Stride_ ),		// pwcText
			_LEN_4CE91014,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_FAEI_VALUE_ARRAY_STRIDE_COMBO,				// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_MID_L + (MX_MAIN_LABEL_W + MX_LEFT_JUST),	// iLeft
			(MX_VAL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) * 2 + (MX_DEF_STATIC_HEIGHT + MX_TOP_JUST),
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},

		// ==== Quick Lock ==== //
		{
			LSW_LT_GROUPBOX,								// ltType
			MX_FAEI_QL_GROUP,								// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_GROUP_L,										// iLeft
			MX_QL_GROUP_T,									// iTop
			MX_INNER_GROUP_W,								// dwWidth
			MX_QL_GROUP_H,									// dwHeight
			MX_GROUPSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_DF99D5C3_Quick_Lock ),		// pwcText
			_LEN_DF99D5C3,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_QL_LOCK_TYPE_LABEL,						// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION1_L,									// iLeft
			(MX_QL_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),										// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_657CD341_Lock_Type_ ),		// pwcText
			_LEN_657CD341,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_FAEI_QL_LOCK_TYPE_COMBO,						// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION2_L,									// iLeft
			MX_QL_GROUP_T + MX_GROUP_TOP,					// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE_LIST,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_FAEI_QL_STRING_LOCK_TYPE_COMBO,				// wId
			WC_COMBOBOXW,									// lpwcClass
			FALSE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION2_L,									// iLeft
			MX_QL_GROUP_T + MX_GROUP_TOP,					// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE_LIST,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_CHECK,									// ltType
			MX_FAEI_QL_LOCKED_CHECK,						// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_MID_L,										// iLeft
			(MX_QL_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_COMBO_HEIGHT - (MX_DEF_CHECK_HEIGHT * 2)) >> 1),									// iTop
			MX_SECTION1_W,									// dwWidth
			MX_DEF_CHECK_HEIGHT * 2,						// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_MULTILINE | BS_AUTO3STATE,
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_F52E2EF1_Locked__Intermediate_Check_Leaves_Locks_As_They_Are_ ),											// pwcText
			_LEN_F52E2EF1,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},

		// Value When Locked.
		{
			LSW_LT_GROUPBOX,								// ltType
			MX_FAEI_QL_VALUE_WHEN_LOCKED_GROUP,				// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION1_L,									// iLeft
			MX_VWL_GROUP_T,									// iTop
			MX_INNER_W,										// dwWidth
			MX_VWL_GROUP_H,									// dwHeight
			MX_GROUPSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_022E8A69_Value_When_Locked ),	// pwcText
			_LEN_022E8A69,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_LABEL,		// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION1_L + MX_GROUP_LEFT,					// iLeft
			(MX_VWL_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),
			MX_MAIN_LABEL_W - MX_GROUP_LEFT,				// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_320BB0E4_Value_ ),			// pwcText
			_LEN_320BB0E4,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_QL_VALUE_WHEN_LOCKED_MIN_LABEL,			// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION1_L + MX_GROUP_LEFT,					// iLeft
			(MX_VWL_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),
			MX_MAIN_LABEL_W - MX_GROUP_LEFT,				// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_324E30C8_Min_ ),				// pwcText
			_LEN_324E30C8,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO,		// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION2_L,									// iLeft
			(MX_VWL_GROUP_T + MX_GROUP_TOP),				// iTop
			MX_INNER_W - MX_SECTION2_L + MX_SECTION1_L - MX_GROUP_LEFT,
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_LABEL,		// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION1_L + MX_GROUP_LEFT,					// iLeft
			(MX_VWL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),
			MX_MAIN_LABEL_W - MX_GROUP_LEFT,				// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_20C5D4A7_Max_ ),				// pwcText
			_LEN_20C5D4A7,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO,		// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION2_L,									// iLeft
			(MX_VWL_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST),
			MX_INNER_W - MX_SECTION2_L + MX_SECTION1_L - MX_GROUP_LEFT,
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},

		// ==== Info ==== //
		{
			LSW_LT_GROUPBOX,								// ltType
			MX_FAEI_INFO_GROUP,								// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_GROUP_L,										// iLeft
			MX_INFO_GROUP_T,								// iTop
			MX_INNER_GROUP_W,								// dwWidth
			MX_INFO_GROUP_H,								// dwHeight
			MX_GROUPSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_6BBB9E69_Info ),				// pwcText
			_LEN_6BBB9E69,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_INFO_ADDRESS_LABEL,						// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION1_L,									// iLeft
			(MX_INFO_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_EDIT_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_77CAA9B0_Address_ ),			// pwcText
			_LEN_77CAA9B0,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			MX_FAEI_INFO_ADDRESS_EDIT,						// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION2_L,									// iLeft
			(MX_INFO_GROUP_T + MX_GROUP_TOP),				// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_EDIT_HEIGHT,								// dwHeight
			MX_EDITSTYLE | ES_READONLY,						// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_INFO_MODULE_LABEL,						// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_MID_L,										// iLeft
			(MX_INFO_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_EDIT_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_EE0A837F_Module_ ),			// pwcText
			_LEN_EE0A837F,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_EDIT,									// ltType
			MX_FAEI_INFO_MODULE_EDIT,						// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_MID_L + MX_MAIN_LABEL_W + MX_LEFT_JUST,		// iLeft
			(MX_INFO_GROUP_T + MX_GROUP_TOP),				// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_EDIT_HEIGHT,								// dwHeight
			MX_EDITSTYLE | ES_READONLY,						// dwStyle
			WS_EX_CLIENTEDGE,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_FAEI_INFO_DESC_LABEL,						// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION1_L,									// iLeft
			(MX_INFO_GROUP_T + MX_GROUP_TOP) + MX_DEF_EDIT_HEIGHT + MX_TOP_JUST,
			MX_INNER_W,										// dwWidth
			MX_DEF_STATIC_HEIGHT * 6,						// dwHeight
			MX_STATICSTYLE | SS_EDITCONTROL | SS_NOPREFIX,	// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
	};

	// == Functions.
	// Creates the Edit dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	BOOL CFoundAddressEditLayout::CreateEditDialog( CWidget * _pwParent, CMemHack * _poMemHack, std::vector<LPARAM> &_vSelected ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlMainDialog, MX_ELEMENTS( m_wlMainDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		CFoundAddressEditPageBase::MX_PARMS pParms = { .pmhMemHack = _poMemHack, .vSelection = _vSelected };

		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], MX_ELEMENTS( m_wlMainDialog ), _pwParent, reinterpret_cast<uint64_t>(&pParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

	// Creates the general edit page.
	CWidget * CFoundAddressEditLayout::CreateGeneralPage( CWidget * _pwParent, CMemHack * _poMemHack, std::vector<LPARAM> &_vSelected ) {
		return CreatePage( _pwParent, m_wlOptionsGeneral, MX_ELEMENTS( m_wlOptionsGeneral ), _poMemHack, _vSelected );
	}

	// Default window-creation.
	CWidget * CFoundAddressEditLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, CMemHack * _poMemHack, std::vector<LPARAM> &_vSelected ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( _pwlLayout, _sTotal,
			vLayouts,
			sStringsW,
			sStrings );
		
		CFoundAddressEditPageBase::MX_PARMS pParms = { .pmhMemHack = _poMemHack, .vSelection = _vSelected };

		CWidget * pwWidget = lsw::CBase::LayoutManager()->CreateDialogX( &vLayouts[0], _sTotal, _pwParent, reinterpret_cast<uint64_t>(&pParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );

		return pwWidget;
	}

}	// namespace mx

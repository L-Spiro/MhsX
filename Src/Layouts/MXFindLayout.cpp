#include "MXFindLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>


namespace mx {

#define MX_FD_L														7
#define MX_FD_W														300
#define MX_FD_MID													(MX_FD_W / 2)
#define MX_FD_LABEL_W												24
#define MX_FD_CTRL_L												(MX_FD_L + MX_FD_LABEL_W + 2)
#define MX_FD_TYPE_W												(MX_FD_MID - 4 - MX_FD_CTRL_L)
#define MX_FD_TOL_L													MX_FD_MID
#define MX_FD_TOL_LABEL_W											56
#define MX_FD_ROW0_T												7
#define MX_FD_ROW1_T												(MX_FD_ROW0_T + 17)
#define MX_FD_ROW2_T												(MX_FD_ROW1_T + 17)
#define MX_FD_IN_L													(MX_FD_L + MX_GROUP_LEFT)
#define MX_FD_COL0_W												(MX_FD_MID - 4 - MX_FD_IN_L)
#define MX_FD_COL1_L												MX_FD_MID
#define MX_FD_COL1_W												(MX_FD_W - MX_FD_L - MX_GROUP_RIGHT - MX_FD_COL1_L)
#define MX_FD_ROW_H													12
#define MX_FD_OPT_T													(MX_FD_ROW2_T + 13)
#define MX_FD_OPT_ROW( ROW )										(MX_FD_OPT_T + MX_GROUP_TOP + 1 + (ROW) * MX_FD_ROW_H)
#define MX_FD_OPT_H													(MX_FD_OPT_ROW( 6 ) - MX_FD_ROW_H + MX_DEF_CHECK_HEIGHT + MX_GROUP_BOTTOM + 2 - MX_FD_OPT_T)
#define MX_FD_ADV_T													(MX_FD_OPT_T + MX_FD_OPT_H + 5)
#define MX_FD_ADV_ROW( ROW )										(MX_FD_ADV_T + MX_GROUP_TOP + 1 + (((ROW) <= 3) ? ((ROW) * MX_FD_ROW_H) : (3 * MX_FD_ROW_H + ((ROW) - 3) * 16)))
#define MX_FD_ADV_H													(MX_FD_ADV_ROW( 5 ) + 2 + MX_DEF_EDIT_HEIGHT + MX_GROUP_BOTTOM + 2 - MX_FD_ADV_T)
#define MX_FD_CUSTOM_W												70
#define MX_FD_COLOR_W												34
#define MX_FD_FORE_W												(MX_FD_MID - 4 - MX_FD_COLOR_W - MX_FD_IN_L - MX_FD_CUSTOM_W)
#define MX_FD_BACK_L												MX_FD_MID
#define MX_FD_BACK_W												26
#define MX_FD_NUM_LABEL_W											(MX_FD_MID - MX_FD_IN_L)
#define MX_FD_NUM_W													60
#define MX_FD_BUTTON_W												52
#define MX_FD_BUTTON_GAP											4
#define MX_FD_BUTTON_L( IDX )										((MX_FD_W - (5 * MX_FD_BUTTON_W + 4 * MX_FD_BUTTON_GAP)) / 2 + (IDX) * (MX_FD_BUTTON_W + MX_FD_BUTTON_GAP))
#define MX_FD_BUTTON_T												(MX_FD_ADV_T + MX_FD_ADV_H + 7)
#define MX_FD_H														(MX_FD_BUTTON_T + MX_DEF_BUTTON_HEIGHT + MX_FD_L)

#define MX_FP_W														200
#define MX_FP_BAR_T													(MX_FD_L + MX_DEF_STATIC_HEIGHT + 4)
#define MX_FP_BUTTON_T												(MX_FP_BAR_T + MX_DEF_PROGRESS_BAR_HEIGHT + 7)
#define MX_FP_H														(MX_FP_BUTTON_T + MX_DEF_BUTTON_HEIGHT + MX_FD_L)

	// == Members.
	// The layout of the Find dialog.
	LSW_WIDGET_LAYOUT CFindLayout::m_wlFindDialog[] = {
		{
			MX_FIND_DIALOG,							// ltType
			MX_FI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_FD_W,								// dwWidth
			MX_FD_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			MX_MAKE_WCHAR( _T_699CCB3A_Find ),		// pwcText
			_LEN_699CCB3A,							// sTextLen
			MX_FI_NONE,								// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FI_FIND_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_L,								// iLeft
			MX_FD_ROW0_T + 2,						// iTop
			MX_FD_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_E5B8BBC4_F_ind_ ),	// pwcText
			_LEN_E5B8BBC4,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_FI_FIND_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_FD_CTRL_L,							// iLeft
			MX_FD_ROW0_T,							// iTop
			MX_FD_W - MX_FD_L - MX_FD_CTRL_L,		// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE | WS_VSCROLL,				// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FI_TYPE_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_L,								// iLeft
			MX_FD_ROW1_T + 2,						// iTop
			MX_FD_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_31E4EF95__Type_ ),	// pwcText
			_LEN_31E4EF95,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_FI_TYPE_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_CTRL_L,							// iLeft
			MX_FD_ROW1_T,							// iTop
			MX_FD_TYPE_W,							// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FI_TOLERANCE_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_TOL_L,							// iLeft
			MX_FD_ROW1_T + 2,						// iTop
			MX_FD_TOL_LABEL_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_FEDACACB_Float_Tolerance_ ),	// pwcText
			_LEN_FEDACACB,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_FI_TOLERANCE_EDIT,					// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_TOL_L + MX_FD_TOL_LABEL_W,		// iLeft
			MX_FD_ROW1_T,							// iTop
			MX_FD_W - MX_FD_L - MX_FD_TOL_L - MX_FD_TOL_LABEL_W,	// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FI_STATUS,							// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_L,								// iLeft
			MX_FD_ROW2_T,							// iTop
			MX_FD_W - MX_FD_L * 2,					// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE | SS_ENDELLIPSIS,		// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_FI_OPTIONS_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_L,								// iLeft
			MX_FD_OPT_T,							// iTop
			MX_FD_W - MX_FD_L * 2,					// dwWidth
			MX_FD_OPT_H,							// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_1F88C31B_Options ),	// pwcText
			_LEN_1F88C31B,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_FIND_ALL_OCCURRENCES,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_OPT_ROW( 0 ),						// iTop
			MX_FD_COL0_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE | WS_GROUP,				// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_469EA709__Find_All_Occurrences ),	// pwcText
			_LEN_469EA709,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_MATCH_CASE,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_OPT_ROW( 1 ),						// iTop
			MX_FD_COL0_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B35F8BFD_Match__Case ),	// pwcText
			_LEN_B35F8BFD,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_WHOLE_WORD,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_OPT_ROW( 2 ),						// iTop
			MX_FD_COL0_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_6D9B37FA_Match__Whole_Word ),	// pwcText
			_LEN_6D9B37FA,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_REGEX,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_OPT_ROW( 3 ),						// iTop
			MX_FD_COL0_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_37F88B22_Search_with_Regular_E_xpressions ),	// pwcText
			_LEN_37F88B22,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_WILDCARDS,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_OPT_ROW( 4 ),						// iTop
			MX_FD_COL0_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_698157B5_Search_with_Wil_dcards ),	// pwcText
			_LEN_698157B5,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_MULTIPLE_RANGES,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_OPT_ROW( 5 ),						// iTop
			MX_FD_COL0_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_44423777_Allow__Multiple_Find_Ranges ),	// pwcText
			_LEN_44423777,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FI_DIRECTION_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_COL1_L,							// iLeft
			MX_FD_OPT_ROW( 0 ) + 1,					// iTop
			MX_FD_COL1_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_BCBB5310_Direction ),	// pwcText
			_LEN_BCBB5310,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_FI_UP,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_COL1_L + 6,						// iLeft
			MX_FD_OPT_ROW( 1 ),						// iTop
			MX_FD_COL1_W - 6,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_54483B6D__Up ),		// pwcText
			_LEN_54483B6D,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_FI_DOWN,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_COL1_L + 6,						// iLeft
			MX_FD_OPT_ROW( 2 ),						// iTop
			MX_FD_COL1_W - 6,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_152ACDA0__Down ),		// pwcText
			_LEN_152ACDA0,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FI_RANGE_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_COL1_L,							// iLeft
			MX_FD_OPT_ROW( 3 ) + 1,					// iTop
			MX_FD_COL1_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_5246754D_Range ),		// pwcText
			_LEN_5246754D,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_LOCK_TO_SELECTION,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_COL1_L + 6,						// iLeft
			MX_FD_OPT_ROW( 4 ),						// iTop
			MX_FD_COL1_W - 6,						// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE | WS_GROUP,				// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_316CBD34__Lock_to_Selection ),	// pwcText
			_LEN_316CBD34,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_FI_ADVANCED_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_L,								// iLeft
			MX_FD_ADV_T,							// iTop
			MX_FD_W - MX_FD_L * 2,					// dwWidth
			MX_FD_ADV_H,							// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_20A1D916_Advanced_Options ),	// pwcText
			_LEN_20A1D916,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_INCREMENTAL,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_ADV_ROW( 0 ),						// iTop
			MX_FD_COL0_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE | WS_GROUP,				// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_C7C92C39_Incremen_tal_Search ),	// pwcText
			_LEN_C7C92C39,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_WRAP,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_ADV_ROW( 1 ),						// iTop
			MX_FD_COL0_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_AD01700C_Allow_Wra_pping ),	// pwcText
			_LEN_AD01700C,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_TYPE_SPECIFIERS,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_ADV_ROW( 2 ),						// iTop
			MX_FD_COL0_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_752FA363_Allow_Type__Specifiers ),	// pwcText
			_LEN_752FA363,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_HIDE_AFTER,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_COL1_L,							// iLeft
			MX_FD_ADV_ROW( 0 ),						// iTop
			MX_FD_COL1_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_7C10650B_Close_after_Find_A_ll ),	// pwcText
			_LEN_7C10650B,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_QUICK_FIND,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_COL1_L,							// iLeft
			MX_FD_ADV_ROW( 1 ),						// iTop
			MX_FD_COL1_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8DE85102__Quick_Find_Next_Previous ),	// pwcText
			_LEN_8DE85102,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_CUSTOM_COLOR,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_ADV_ROW( 3 ) + 2,					// iTop
			MX_FD_CUSTOM_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_520A39C8_Use_Cust_om_Color_ ),	// pwcText
			_LEN_520A39C8,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_FORE_CHECK,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L + MX_FD_CUSTOM_W,			// iLeft
			MX_FD_ADV_ROW( 3 ) + 2,					// iTop
			MX_FD_FORE_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_7F0BA2C2_Fo_re_ ),	// pwcText
			_LEN_7F0BA2C2,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COLORBUTTON,						// ltType
			MX_FI_FORE_COLOR,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L + MX_FD_CUSTOM_W + MX_FD_FORE_W,	// iLeft
			MX_FD_ADV_ROW( 3 ),						// iTop
			MX_FD_COLOR_W,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FI_BACK_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_BACK_L,							// iLeft
			MX_FD_ADV_ROW( 3 ) + 2,					// iTop
			MX_FD_BACK_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_CB0AC260__Back_ ),	// pwcText
			_LEN_CB0AC260,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COLORBUTTON,						// ltType
			MX_FI_BACK_COLOR,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_BACK_L + MX_FD_BACK_W,			// iLeft
			MX_FD_ADV_ROW( 3 ),						// iTop
			MX_FD_COLOR_W,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FI_LIMIT,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_ADV_ROW( 4 ) + 2,					// iTop
			MX_FD_NUM_LABEL_W,						// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_5A3A0D34_Li_mit_Find_Occurrences_ ),	// pwcText
			_LEN_5A3A0D34,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_FI_LIMIT_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L + MX_FD_NUM_LABEL_W,			// iLeft
			MX_FD_ADV_ROW( 4 ),						// iTop
			MX_FD_NUM_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | ES_NUMBER,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FI_MAX_WILDCARD_LABEL,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L,								// iLeft
			MX_FD_ADV_ROW( 5 ) + 4,					// iTop
			MX_FD_NUM_LABEL_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_36F89C92_Maximum_Wildcard_Match_Len_gth_ ),	// pwcText
			_LEN_36F89C92,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_FI_MAX_WILDCARD_EDIT,				// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_IN_L + MX_FD_NUM_LABEL_W,			// iLeft
			MX_FD_ADV_ROW( 5 ) + 2,					// iTop
			MX_FD_NUM_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | ES_NUMBER,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_FI_FIND,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_BUTTON_L( 0 ),					// iLeft
			MX_FD_BUTTON_T,							// iTop
			MX_FD_BUTTON_W,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE | WS_GROUP,			// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_C07B399F__Find ),		// pwcText
			_LEN_C07B399F,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_FI_FIND_NEXT,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_BUTTON_L( 1 ),					// iLeft
			MX_FD_BUTTON_T,							// iTop
			MX_FD_BUTTON_W,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_F6024D9D_Find__Next ),	// pwcText
			_LEN_F6024D9D,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_FI_FIND_PREV,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_BUTTON_L( 2 ),					// iLeft
			MX_FD_BUTTON_T,							// iTop
			MX_FD_BUTTON_W,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_E0E59780_Find__Previous ),	// pwcText
			_LEN_E0E59780,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_FI_FIND_ALL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_BUTTON_L( 3 ),					// iLeft
			MX_FD_BUTTON_T,							// iTop
			MX_FD_BUTTON_W,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_74459E8E_Find__All ),	// pwcText
			_LEN_74459E8E,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_FI_CLOSE,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_BUTTON_L( 4 ),					// iLeft
			MX_FD_BUTTON_T,							// iTop
			MX_FD_BUTTON_W,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_D2C0AEC0_Close ),		// pwcText
			_LEN_D2C0AEC0,							// sTextLen
			MX_FI_DIALOG,							// dwParentId
		},
	};

	// The layout of the dialog shown while a long search runs.
	LSW_WIDGET_LAYOUT CFindLayout::m_wlFindProgress[] = {
		{
			MX_FIND_PROGRESS,						// ltType
			MX_FI_PROGRESS_DIALOG,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_FP_W,								// dwWidth
			MX_FP_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME,							// dwStyleEx
			MX_MAKE_WCHAR( _T_699CCB3A_Find ),		// pwcText
			_LEN_699CCB3A,							// sTextLen
			MX_FI_NONE,								// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FI_PROGRESS_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_L,								// iLeft
			MX_FD_L,								// iTop
			MX_FP_W - MX_FD_L * 2,					// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_16A69772_Searching___ ),	// pwcText
			_LEN_16A69772,							// sTextLen
			MX_FI_PROGRESS_DIALOG,					// dwParentId
		},
		{
			LSW_LT_PROGRESSBAR,						// ltType
			MX_FI_PROGRESS_BAR,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_L,								// iLeft
			MX_FP_BAR_T,							// iTop
			MX_FP_W - MX_FD_L * 2,					// dwWidth
			MX_DEF_PROGRESS_BAR_HEIGHT,				// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | PBS_SMOOTH,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,	// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FI_PROGRESS_DIALOG,					// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_FI_PROGRESS_CANCEL,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			(MX_FP_W - MX_DEF_BUTTON_WIDTH) / 2,	// iLeft
			MX_FP_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),	// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_FI_PROGRESS_DIALOG,					// dwParentId
		},
	};

	// == Functions.
	/**
	 * Creates the modeless dialog shown while a long search runs (it has a progress bar and a Cancel button).  The caller
	 *	destroys its window and deletes the returned object when the search ends.
	 *
	 * \param _pwParent The owner window.
	 * \param _fppParms The parameters, which must remain valid while the dialog exists.
	 * \return Returns the dialog, or nullptr on failure.
	 **/
	CWidget * CFindLayout::CreateFindProgress( CWidget * _pwParent, MX_FIND_PROGRESS_PARMS &_fppParms ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlFindProgress, std::size( m_wlFindProgress ),
			vLayouts,
			sStringsW,
			sStrings );

		CWidget * pwWidget = lsw::CBase::LayoutManager()->CreateDialogX( &vLayouts[0], std::size( m_wlFindProgress ), _pwParent, reinterpret_cast<uint64_t>(&_fppParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		return pwWidget;
	}

	/**
	 * Shows the Find dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	 *
	 * \param _pwParent The parent window.
	 * \param _fdpParms The settings and the function that searches.
	 * \return Returns the dialog result.
	 **/
	DWORD CFindLayout::CreateFindDialog( CWidget * _pwParent, MX_FIND_DIALOG_PARMS &_fdpParms ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlFindDialog, std::size( m_wlFindDialog ),
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], std::size( m_wlFindDialog ), _pwParent, reinterpret_cast<uint64_t>(&_fdpParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		return ipProc > 0 ? TRUE : FALSE;
	}

}	// namespace mx

#include "MXNewStringSearchLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "MXLayoutManager.h"
#include <Base/LSWBase.h>


namespace mx {

#define MX_SEARCH_W									280
//#define MX_SEARCH_H									(200)
#define MX_SEARCH_GROUP_TOP							MX_TOP_JUST
#define MX_CENTER									(MX_SEARCH_W / 2)
#define MX_1_3RD_W									((MX_SEARCH_W - (MX_LEFT_ALIGN + MX_GROUP_LEFT) - (MX_LEFT_ALIGN + MX_GROUP_RIGHT)) / 3)
#define MX_1_3RD_LEFT								(MX_LEFT_ALIGN + MX_GROUP_LEFT)
#define MX_2_3RD_LEFT								(MX_1_3RD_LEFT + MX_1_3RD_W)
#define MX_3_3RD_LEFT								(MX_1_3RD_LEFT + MX_1_3RD_W * 2)
#define MX_WIDTH_TO_CENTER( L )						((MX_SEARCH_W / 2) - (L))
#define MX_SEARCH_GROUP_HEIGHT						((MX_DEF_COMBO_HEIGHT + 2) * 2 + ((MX_DEF_EDIT_HEIGHT + 2) * 3) + MX_GROUP_TOP + MX_GROUP_BOTTOM + 2)
#define MX_COMBOBOXES_LEFT							(MX_LEFT_ALIGN + MX_GROUP_LEFT + 50)
#define MX_OPTIONS_TOP								(MX_SEARCH_GROUP_HEIGHT + MX_SEARCH_GROUP_TOP)
#define MX_OPTIONS_H								(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_DEF_CHECK_HEIGHT * 4 + MX_DEF_BUTTON_HEIGHT)
#define MX_SEARCH_RANGE_TOP							(MX_OPTIONS_TOP + MX_OPTIONS_H)
#define MX_SEARCH_RANGE_H							(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT)
#define MX_SEARCH_H									(MX_SEARCH_RANGE_TOP + MX_SEARCH_RANGE_H + (MX_TOP_JUST * 4 + MX_DEF_BUTTON_HEIGHT))

	// == Members.
	// The layout for the New Data-Type Search dialog.
	LSW_WIDGET_LAYOUT CNewStringSearchLayout::m_wlNewStringSearchDialog[] = {
		{
			MX_NEW_STRING_SEARCH,					// ltType
			MX_SSI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_SEARCH_W,							// dwWidth
			MX_SEARCH_H,							// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_9468D604_String_Search ),																									// pwcText
			_LEN_9468D604,																																// sTextLen
			MX_SSI_NONE,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_SSI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_ALIGN,																					// iLeft
			MX_SEARCH_H - MX_TOP_JUST - MX_DEF_BUTTON_HEIGHT,												// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,					// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),																// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_SSI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SEARCH_W - MX_DEF_BUTTON_WIDTH - MX_LEFT_ALIGN,												// iLeft
			MX_SEARCH_H - MX_TOP_JUST - MX_DEF_BUTTON_HEIGHT,												// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,					// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),															// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		
		// ==== Search ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_SSI_SEARCH_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_SEARCH_GROUP_TOP,					// iTop
			MX_SEARCH_W - (MX_LEFT_ALIGN * 2),		// dwWidth
			MX_SEARCH_GROUP_HEIGHT,					// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B35CDE91_Search ),	// pwcText
			_LEN_B35CDE91,							// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_SSI_TYPE_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			(MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + 2),														// iTop
			50,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_97F15F00_Type_ ),		// pwcText
			_LEN_97F15F00,							// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_SSI_STRING_TYPE_COMBO,				// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_COMBOBOXES_LEFT,						// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP,		// iTop
			MX_SEARCH_W - MX_LEFT_JUST - MX_GROUP_RIGHT - MX_COMBOBOXES_LEFT,								// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_SSI_CHAR_TYPE_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER,								// iLeft
			(MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + 2),														// iTop
			50,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_1B8339E8_Character_Type_ ),													// pwcText
			_LEN_1B8339E8,							// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_SSI_REGEX_ENCODING_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER,								// iLeft
			(MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + 2),														// iTop
			33,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_70F4A064_Encoding_ ),															// pwcText
			_LEN_70F4A064,							// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_SSI_CHAR_TYPE_COMBO,					// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER + 50,							// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP,		// iTop
			MX_SEARCH_W - MX_LEFT_JUST - MX_GROUP_RIGHT - (MX_CENTER + 50),									// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_SSI_REGEX_ENCODING_COMBO,			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER + 33,							// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP,		// iTop
			MX_SEARCH_W - MX_LEFT_JUST - MX_GROUP_RIGHT - (MX_CENTER + 33),								// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_SSI_STRING_TO_FIND_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			(MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + 2) + MX_DEF_COMBO_HEIGHT + 2,								// iTop
			50,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_21CE2C3B_String_to_Find_ ),													// pwcText
			_LEN_21CE2C3B,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_SSI_STRING_TO_FIND_COMBO,			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_COMBOBOXES_LEFT,						// iLeft
			(MX_SEARCH_GROUP_TOP + MX_GROUP_TOP) + MX_DEF_COMBO_HEIGHT + 2,									// iTop
			MX_SEARCH_W - MX_LEFT_JUST - MX_GROUP_RIGHT - MX_COMBOBOXES_LEFT,								// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_SSI_PREVIEW_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			((MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + 2) + MX_DEF_COMBO_HEIGHT + 2) + MX_DEF_COMBO_HEIGHT + 2,	// iTop
			50,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_CB18E1EA_Preview_ ),	// pwcText
			_LEN_CB18E1EA,							// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_SSI_REGEX_FLAVOR_LABEL,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			((MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + 2) + MX_DEF_COMBO_HEIGHT + 2) + MX_DEF_COMBO_HEIGHT + 2,	// iTop
			50,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B709BF7A_Flavor_ ),	// pwcText
			_LEN_B709BF7A,							// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_SSI_PREVIEW_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_COMBOBOXES_LEFT,						// iLeft
			((MX_SEARCH_GROUP_TOP + MX_GROUP_TOP) + MX_DEF_COMBO_HEIGHT + 2) + MX_DEF_COMBO_HEIGHT + 2,		// iTop
			MX_SEARCH_W - MX_LEFT_JUST - MX_GROUP_RIGHT - MX_COMBOBOXES_LEFT,								// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,										// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_SSI_REGEX_FLAVOR_COMBO,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_COMBOBOXES_LEFT,						// iLeft
			((MX_SEARCH_GROUP_TOP + MX_GROUP_TOP) + MX_DEF_COMBO_HEIGHT + 2) + MX_DEF_COMBO_HEIGHT + 2,		// iTop
			MX_SEARCH_W - MX_LEFT_JUST - MX_GROUP_RIGHT - MX_COMBOBOXES_LEFT,								// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_SSI_PREVIEW_HEX_EDIT,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_COMBOBOXES_LEFT,						// iLeft
			(((MX_SEARCH_GROUP_TOP + MX_GROUP_TOP) + MX_DEF_COMBO_HEIGHT + 2) + MX_DEF_COMBO_HEIGHT + 2) + (MX_DEF_EDIT_HEIGHT) + 2,	// iTop
			MX_SEARCH_W - MX_LEFT_JUST - MX_GROUP_RIGHT - MX_COMBOBOXES_LEFT,															// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,																	// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_SSI_BYTE_HELPER_LABEL,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			(MX_SEARCH_GROUP_TOP + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + 2) * 4,																							// iTop
			MX_SEARCH_W - MX_LEFT_JUST - MX_GROUP_RIGHT - (MX_LEFT_ALIGN + MX_GROUP_LEFT),																					// dwWidth
			MX_DEF_STATIC_HEIGHT * 2,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},


		//
		{
			LSW_LT_LABEL,							// ltType
			MX_SSI_UTF_HELPER_LABEL,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			(MX_SEARCH_GROUP_TOP + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + 2) * 4 + 2,													// iTop
			50,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_BA612FFE_Unicode_Chars_ ),																				// pwcText
			_LEN_BA612FFE,																												// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_SSI_UTF_HELPER_COMBO,				// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_COMBOBOXES_LEFT,						// iLeft
			(MX_SEARCH_GROUP_TOP + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + 2) * 4,														// iTop
			MX_SEARCH_W - MX_LEFT_JUST - MX_GROUP_RIGHT - MX_COMBOBOXES_LEFT,															// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		//

		//
		{
			LSW_LT_LABEL,							// ltType
			MX_SSI_REGEX_HELPER_LABEL,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			(MX_SEARCH_GROUP_TOP + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + 2) * 4 + 2,													// iTop
			150,									// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_F3327DB2_Regex_Sheet_ ),																					// pwcText
			_LEN_F3327DB2,																												// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_SSI_REGEX_HELPER_COMBO,				// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_COMBOBOXES_LEFT,						// iLeft
			(MX_SEARCH_GROUP_TOP + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + 2) * 4,														// iTop
			MX_SEARCH_W - MX_LEFT_JUST - MX_GROUP_RIGHT - MX_COMBOBOXES_LEFT,															// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		//
		
		
		// ==== Options ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_SSI_OPTIONS_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_OPTIONS_TOP,							// iTop
			MX_SEARCH_W - (MX_LEFT_ALIGN * 2),		// dwWidth
			MX_OPTIONS_H,							// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_1F88C31B_Options ),	// pwcText
			_LEN_1F88C31B,							// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_IGNORE_CASE_CHECK,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_1_3RD_LEFT,							// iLeft
			MX_OPTIONS_TOP + MX_GROUP_TOP,			// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_73A2F867_Ignore_Case ),														// pwcText
			_LEN_73A2F867,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId
			
			LSW_NO_SIZE_EXP,
			_T_LEN_73A2F867_Ignore_Case,			// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_ALIGNED_CHECK,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_2_3RD_LEFT,							// iLeft
			MX_OPTIONS_TOP + MX_GROUP_TOP,			// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_22E9689D_Aligned ),															// pwcText
			_LEN_22E9689D,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_EA556272_Characters_in_the_string_are_type_aligned_,										// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_ESCAPES_CHECK,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_3_3RD_LEFT,							// iLeft
			MX_OPTIONS_TOP + MX_GROUP_TOP,			// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_78F75E9B_Resolve_Escapes_____ ),												// pwcText
			_LEN_78F75E9B,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_A9CD0C35_Resolves_escape_sequences____a__b__f__n__r__t__v____________nnn__xnn__unnnn__Unnnnnnnn____,
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_WHOLE_WORD_CHECK,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_1_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT,											// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_40CE1008_Whole_Word_Only ),													// pwcText
			_LEN_40CE1008,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_8A60E2C0_Requires_matches_to_be_surrounded_by_word_boundaries_,							// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_HEX_CHECK,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_2_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT,											// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_800B4890_Array_is_in_Hex ),													// pwcText
			_LEN_800B4890,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_9C4FF69C_Indicates_that_each_element_in_the_array_is_a_hexadecimal_value_,				// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_WILDCARD_CHECK,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_3_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT,											// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_003B75E3_Wildcard ),															// pwcText
			_LEN_003B75E3,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_05F7A799___and___characters_are_treated_as_wildcards_,									// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_LINGUISTIC_IGNORECASE_CHECK,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_1_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT * 2,										// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_9D23A539_Linguistic_Ignore_Case ),											// pwcText
			_LEN_9D23A539,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_A8F450F1_Ignore_case__as_linguistically_appropriate____________,							// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_REGEX_SINGLELINE,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_1_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT * 2,										// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_6C2A854F_Single_Line ),														// pwcText
			_LEN_6C2A854F,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_98F37FD9___________A_____________Z_,														// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_LINGUISTIC_IGNOREDIACRITIC_CHECK,// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_2_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT * 2,										// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_899EFB13_Linguistic_Ignore_Diacritic ),										// pwcText
			_LEN_899EFB13,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_645BF5AE_Ignore_nonspacing_characters__as_linguistically_appropriate__________A,			// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_REGEX_MULTILINE,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_2_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT * 2,										// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_95B3789A_Multiline ),															// pwcText
			_LEN_95B3789A,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_0474EC48_____matches_with_new_line_characters_,											// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_NORM_IGNOREKANATYPE_CHECK,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_3_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT * 2,										// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_4E17788D_Ignore_Kana ),														// pwcText
			_LEN_4E17788D,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_2165F244_Do_not_differentiate_between_hiragana_and_katakana_characters______________,	// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_REGEX_EXTENDED,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_3_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT * 2,										// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_DCCBE329_Extended ),															// pwcText
			_LEN_DCCBE329,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_77AB5C2E_Use_Extended_Regex_,	// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},

		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_NORM_IGNORENONSPACE_CHECK,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_1_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT * 3,										// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_5F5A587A_Ignore_Non_Space ),													// pwcText
			_LEN_5F5A587A,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_44CE7F22_Ignore_nonspacing_characters_,													// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_REGEX_FIND_LONGEST,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_1_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT * 3,										// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_5F59C830_Find_Longest ),														// pwcText
			_LEN_5F59C830,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_6A177D05_Finds_the_longest_match_,														// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_NORM_IGNORESYMBOLS_CHECK,		// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_2_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT * 3,										// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_DFF728E7_Ignore_Symbols ),													// pwcText
			_LEN_DFF728E7,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_EDA70C5F_Ignore_symbols_and_punctuation__________,										// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_NORM_IGNOREWIDTH_CHECK,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_3_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT * 3,										// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_58AADFAC_Ignore_Width ),														// pwcText
			_LEN_58AADFAC,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_339AE0D6_Ignore_the_difference_between_half_width_and_full_width_characters___________A,	// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_SSI_REGEX_NEGATE_SINGLELINE,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_3_3RD_LEFT,							// iLeft
			(MX_OPTIONS_TOP + MX_GROUP_TOP) + MX_DEF_CHECK_HEIGHT * 3,										// iTop
			MX_1_3RD_W,								// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_7B9E8326_Negate_Single_Line ),												// pwcText
			_LEN_7B9E8326,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId

			LSW_NO_SIZE_EXP,
			_T_LEN_A893C9A7_Negates_the_single_line_attribute_enabled_in_Java__POSIX__and_Perl_,			// pcToolTip, sToolTipLen
			WS_EX_TOPMOST,							// dwToolTipStyleEx
			MX_TOOLTIP_STYLE,						// dwToolTipStyle
		},

		{
			LSW_LT_BUTTON,							// ltType
			MX_SSI_GENERAL_SEARCH_OPTIONS_BUTTON,	// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_OPTIONS_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 4,										// iTop
			MX_SEARCH_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT) * 2),											// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,					// dwStyleEx
			MX_MAKE_WCHAR( _T_5FE14262_General_Search_Options ),											// pwcText
			_LEN_5FE14262,																					// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},

		// ==== Search Range ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_SSI_SEARCH_RANGE_GROUP,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_SEARCH_RANGE_TOP,					// iTop
			MX_SEARCH_W - (MX_LEFT_ALIGN * 2),		// dwWidth
			MX_SEARCH_RANGE_H,						// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8436D248_Search_Range ),	// pwcText
			_LEN_8436D248,								// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_SSI_FROM_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SEARCH_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT) * 2), MX_LEFT_ALIGN + MX_GROUP_LEFT, 2, 0, MX_LEFT_JUST ),			// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP,		// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT) * 2), MX_LEFT_ALIGN + MX_GROUP_LEFT, 2, 0, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_857372A6_From_ ),		// pwcText
			_LEN_857372A6,							// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_SSI_TO_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SEARCH_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT) * 2), MX_LEFT_ALIGN + MX_GROUP_LEFT, 2, 1, MX_LEFT_JUST ),			// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP,		// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT) * 2), MX_LEFT_ALIGN + MX_GROUP_LEFT, 2, 1, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B09DF1A4_To_ ),		// pwcText
			_LEN_B09DF1A4,							// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_SSI_FROM_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SEARCH_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT) * 2), MX_LEFT_ALIGN + MX_GROUP_LEFT, 2, 0, MX_LEFT_JUST ),			// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT,									// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT) * 2), MX_LEFT_ALIGN + MX_GROUP_LEFT, 2, 0, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_SSI_TO_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE_EX( MX_SEARCH_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT) * 2), MX_LEFT_ALIGN + MX_GROUP_LEFT, 2, 1, MX_LEFT_JUST ),			// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT,									// iTop
			MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT) * 2), MX_LEFT_ALIGN + MX_GROUP_LEFT, 2, 1, MX_LEFT_JUST ),	// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SSI_DIALOG,							// dwParentId
		},

	};

	// == Functions.
	// Creates the New String Search dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	DWORD CNewStringSearchLayout::CreateNewStringSearchDialog( CWidget * _pwParent, CMemHack * _pmhMemHack ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlNewStringSearchDialog, std::size( m_wlNewStringSearchDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], std::size( m_wlNewStringSearchDialog ), _pwParent, reinterpret_cast<uint64_t>(_pmhMemHack) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

#undef MX_SEARCH_RANGE_H
#undef MX_SEARCH_RANGE_TOP
#undef MX_OPTIONS_TOP
#undef MX_COMBOBOXES_LEFT
#undef MX_SEARCH_GROUP_HEIGHT
#undef MX_WIDTH_TO_CENTER
#undef MX_3_3RD_LEFT
#undef MX_2_3RD_LEFT
#undef MX_1_3RD_LEFT
#undef MX_1_3RD_W
#undef MX_CENTER
#undef MX_LEFT_ALIGN
#undef MX_SEARCH_GROUP_TOP
#undef MX_SEARCH_H
#undef MX_SEARCH_W

}	// namespace mx



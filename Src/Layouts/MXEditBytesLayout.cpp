#include "MXEditBytesLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>


namespace mx {

#define MX_EB_L														7
#define MX_EB_CTRL_W												196
#define MX_EB_W														(MX_EB_L * 2 + MX_EB_CTRL_W)
#define MX_EB_START_LABEL_T											7
#define MX_EB_START_EDIT_T											(MX_EB_START_LABEL_T + MX_DEF_STATIC_HEIGHT + MX_TOP_JUST)
#define MX_EB_RADIO_T												(MX_EB_START_EDIT_T + MX_DEF_EDIT_HEIGHT + 6)
#define MX_EB_SIZE_RADIO_W											50
#define MX_EB_END_RADIO_L											(MX_EB_L + MX_EB_SIZE_RADIO_W + 6)
#define MX_EB_END_RADIO_W											(MX_EB_CTRL_W - MX_EB_SIZE_RADIO_W - 6)
#define MX_EB_VALUE_EDIT_T											(MX_EB_RADIO_T + MX_DEF_RADIO_HEIGHT + MX_TOP_JUST)
#define MX_EB_NUM_LABEL_W											36
#define MX_EB_NUM_RADIO_W											50
#define MX_EB_NUM_HEX_L												(MX_EB_L + MX_EB_NUM_LABEL_W + 4)
#define MX_EB_NUM_DEC_L												(MX_EB_NUM_HEX_L + MX_EB_NUM_RADIO_W + 4)
#define MX_EB_NUM_T													(MX_EB_VALUE_EDIT_T + MX_DEF_EDIT_HEIGHT + 8)
#define MX_EB_BYTE_LABEL_T											(MX_EB_NUM_T + MX_DEF_RADIO_HEIGHT + 6)
#define MX_EB_BYTE_T												(MX_EB_BYTE_LABEL_T + MX_DEF_STATIC_HEIGHT + MX_TOP_JUST)
#define MX_EB_BUTTON_T												(MX_EB_BYTE_T + MX_DEF_EDIT_HEIGHT + 9)
#define MX_EB_BUTTON_GAP											4
#define MX_EB_H														(MX_EB_BUTTON_T + MX_DEF_BUTTON_HEIGHT + MX_EB_L)
#define MX_EB_BV_LBL_W												20
#define MX_EB_BV_EDIT_W												40
#define MX_EB_BV_STEP												(MX_EB_BV_LBL_W + 2 + MX_EB_BV_EDIT_W + 8)
#define MX_EB_BV_HEX_LBL_L											MX_EB_L
#define MX_EB_BV_HEX_L												(MX_EB_BV_HEX_LBL_L + MX_EB_BV_LBL_W + 2)
#define MX_EB_BV_DEC_LBL_L											(MX_EB_BV_HEX_LBL_L + MX_EB_BV_STEP)
#define MX_EB_BV_DEC_L												(MX_EB_BV_DEC_LBL_L + MX_EB_BV_LBL_W + 2)
#define MX_EB_BV_CHAR_LBL_L											(MX_EB_BV_DEC_LBL_L + MX_EB_BV_STEP)
#define MX_EB_BV_CHAR_L												(MX_EB_BV_CHAR_LBL_L + MX_EB_BV_LBL_W + 2)
// Set File Size.
#define MX_SS_INFO_T												(MX_EB_START_EDIT_T + MX_DEF_EDIT_HEIGHT + 4)
#define MX_SS_NUM_T													(MX_SS_INFO_T + MX_DEF_STATIC_HEIGHT + 6)
#define MX_SS_BYTE_LABEL_T											(MX_SS_NUM_T + MX_DEF_RADIO_HEIGHT + 6)
#define MX_SS_BYTE_T												(MX_SS_BYTE_LABEL_T + MX_DEF_STATIC_HEIGHT + MX_TOP_JUST)
#define MX_SS_BUTTON_T												(MX_SS_BYTE_T + MX_DEF_EDIT_HEIGHT + 9)
#define MX_SS_H														(MX_SS_BUTTON_T + MX_DEF_BUTTON_HEIGHT + MX_EB_L)

	// == Members.
	// The layout of the Insert Bytes and Overwrite Bytes dialogs.
	LSW_WIDGET_LAYOUT CEditBytesLayout::m_wlEditBytesDialog[] = {
		{
			MX_EDIT_BYTES,							// ltType
			MX_EBI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_EB_W,								// dwWidth
			MX_EB_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			MX_MAKE_WCHAR( _T_98B3F074_Insert_Bytes ),		// pwcText
			_LEN_98B3F074,					// sTextLen
			MX_EBI_NONE,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_START_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_L,								// iLeft
			MX_EB_START_LABEL_T,					// iTop
			MX_EB_CTRL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_26C90B12__Start_Address_ ),	// pwcText
			_LEN_26C90B12,					// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EBI_START_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_L,								// iLeft
			MX_EB_START_EDIT_T,						// iTop
			MX_EB_CTRL_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EBI_SIZE_RADIO,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_EB_L,								// iLeft
			MX_EB_RADIO_T,							// iTop
			MX_EB_SIZE_RADIO_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_2B77C476_Si_ze ),			// pwcText
			_LEN_2B77C476,							// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EBI_END_RADIO,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_END_RADIO_L,						// iLeft
			MX_EB_RADIO_T,							// iTop
			MX_EB_END_RADIO_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_364EC236__End_Address__Inclusive_ ),	// pwcText
			_LEN_364EC236,		// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EBI_VALUE_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_L,								// iLeft
			MX_EB_VALUE_EDIT_T,						// iTop
			MX_EB_CTRL_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | WS_GROUP,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_NUMBERS_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_L,								// iLeft
			MX_EB_NUM_T + 2,						// iTop
			MX_EB_NUM_LABEL_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_296E801B_Numbers_ ),			// pwcText
			_LEN_296E801B,						// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EBI_HEX_RADIO,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_NUM_HEX_L,						// iLeft
			MX_EB_NUM_T,							// iTop
			MX_EB_NUM_RADIO_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_ACF8DC2F_He_x ),				// pwcText
			_LEN_ACF8DC2F,							// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EBI_DEC_RADIO,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_NUM_DEC_L,						// iLeft
			MX_EB_NUM_T,							// iTop
			MX_EB_NUM_RADIO_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_5217E1C9__Decimal ),			// pwcText
			_LEN_5217E1C9,						// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_BYTE_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_L,								// iLeft
			MX_EB_BYTE_LABEL_T,						// iTop
			MX_EB_CTRL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_1ACA18FE_Byte__value_ ),		// pwcText
			_LEN_1ACA18FE,					// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_BV_HEX_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_HEX_LBL_L,						// iLeft
			MX_EB_BYTE_T + 2,						// iTop
			MX_EB_BV_LBL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_10128C49_Hex_ ),				// pwcText
			_LEN_10128C49,							// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EBI_BV_HEX_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_HEX_L,							// iLeft
			MX_EB_BYTE_T,							// iTop
			MX_EB_BV_EDIT_W,						// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | WS_GROUP,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_BV_DEC_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_DEC_LBL_L,						// iLeft
			MX_EB_BYTE_T + 2,						// iTop
			MX_EB_BV_LBL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_F3F2F86B_Dec_ ),				// pwcText
			_LEN_F3F2F86B,							// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EBI_BV_DEC_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_DEC_L,							// iLeft
			MX_EB_BYTE_T,							// iTop
			MX_EB_BV_EDIT_W,						// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_BV_CHAR_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_CHAR_LBL_L,					// iLeft
			MX_EB_BYTE_T + 2,						// iTop
			MX_EB_BV_LBL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B5F369B9_Char_ ),			// pwcText
			_LEN_B5F369B9,							// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EBI_BV_CHAR_EDIT,					// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_CHAR_L,						// iLeft
			MX_EB_BYTE_T,							// iTop
			MX_EB_BV_EDIT_W,						// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_EBI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_EB_W - MX_EB_L - MX_DEF_BUTTON_WIDTH * 2 - MX_EB_BUTTON_GAP,	// iLeft
			MX_EB_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),				// pwcText
			_LEN_D736D92D,								// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_EBI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_W - MX_EB_L - MX_DEF_BUTTON_WIDTH,	// iLeft
			MX_EB_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),			// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
	};

	// The layout of the Set File Size dialog.
	LSW_WIDGET_LAYOUT CEditBytesLayout::m_wlSetSizeDialog[] = {
		{
			MX_EDIT_BYTES,							// ltType
			MX_EBI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_EB_W,								// dwWidth
			MX_SS_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			MX_MAKE_WCHAR( _T_CB1726AD_Set_File_Size ),	// pwcText
			_LEN_CB1726AD,					// sTextLen
			MX_EBI_NONE,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_START_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_L,								// iLeft
			MX_EB_START_LABEL_T,					// iTop
			MX_EB_CTRL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_73AD7AF4_New__Size_ ),		// pwcText
			_LEN_73AD7AF4,						// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EBI_START_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_L,								// iLeft
			MX_EB_START_EDIT_T,						// iTop
			MX_EB_CTRL_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_INFO_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_L,								// iLeft
			MX_SS_INFO_T,							// iTop
			MX_EB_CTRL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_NUMBERS_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_L,								// iLeft
			MX_SS_NUM_T + 2,						// iTop
			MX_EB_NUM_LABEL_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_296E801B_Numbers_ ),			// pwcText
			_LEN_296E801B,						// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EBI_HEX_RADIO,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_NUM_HEX_L,						// iLeft
			MX_SS_NUM_T,							// iTop
			MX_EB_NUM_RADIO_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_ACF8DC2F_He_x ),				// pwcText
			_LEN_ACF8DC2F,							// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EBI_DEC_RADIO,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_NUM_DEC_L,						// iLeft
			MX_SS_NUM_T,							// iTop
			MX_EB_NUM_RADIO_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_5217E1C9__Decimal ),			// pwcText
			_LEN_5217E1C9,						// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_BYTE_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_L,								// iLeft
			MX_SS_BYTE_LABEL_T,						// iTop
			MX_EB_CTRL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_FAFB9AF7_Byte__value_of_added_bytes_ ),	// pwcText
			_LEN_FAFB9AF7,		// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_BV_HEX_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_HEX_LBL_L,						// iLeft
			MX_SS_BYTE_T + 2,						// iTop
			MX_EB_BV_LBL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_10128C49_Hex_ ),				// pwcText
			_LEN_10128C49,							// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EBI_BV_HEX_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_HEX_L,							// iLeft
			MX_SS_BYTE_T,							// iTop
			MX_EB_BV_EDIT_W,						// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | WS_GROUP,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_BV_DEC_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_DEC_LBL_L,						// iLeft
			MX_SS_BYTE_T + 2,						// iTop
			MX_EB_BV_LBL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_F3F2F86B_Dec_ ),				// pwcText
			_LEN_F3F2F86B,							// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EBI_BV_DEC_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_DEC_L,							// iLeft
			MX_SS_BYTE_T,							// iTop
			MX_EB_BV_EDIT_W,						// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EBI_BV_CHAR_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_CHAR_LBL_L,					// iLeft
			MX_SS_BYTE_T + 2,						// iTop
			MX_EB_BV_LBL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B5F369B9_Char_ ),			// pwcText
			_LEN_B5F369B9,							// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EBI_BV_CHAR_EDIT,					// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_BV_CHAR_L,						// iLeft
			MX_SS_BYTE_T,							// iTop
			MX_EB_BV_EDIT_W,						// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_EBI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_EB_W - MX_EB_L - MX_DEF_BUTTON_WIDTH * 2 - MX_EB_BUTTON_GAP,	// iLeft
			MX_SS_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),				// pwcText
			_LEN_D736D92D,								// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_EBI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EB_W - MX_EB_L - MX_DEF_BUTTON_WIDTH,	// iLeft
			MX_SS_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),			// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_EBI_DIALOG,							// dwParentId
		},
	};

	// == Functions.
	/**
	 * Shows the Insert Bytes, Overwrite Bytes, or Set File Size dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it
	 *	can decode strings etc.
	 *
	 * \param _pwParent The parent window.
	 * \param _ebpParms The default values on input and the values entered on output.
	 * \return Returns TRUE if the user pressed OK.
	 **/
	DWORD CEditBytesLayout::CreateEditBytesDialog( CWidget * _pwParent, MX_EDIT_BYTES_PARMS &_ebpParms ) {
		const bool bSetSize = _ebpParms.ebmMode == MX_EBM_SET_SIZE;
		const LSW_WIDGET_LAYOUT * pwlLayout = bSetSize ? m_wlSetSizeDialog : m_wlEditBytesDialog;
		const size_t sTotal = bSetSize ? std::size( m_wlSetSizeDialog ) : std::size( m_wlEditBytesDialog );
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( pwlLayout, sTotal,
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], sTotal, _pwParent, reinterpret_cast<uint64_t>(&_ebpParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		return ipProc > 0 ? TRUE : FALSE;
	}

}	// namespace mx

#include "MXExportHexLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>


namespace mx {

#define MX_EH_L														7
#define MX_EH_W														280
#define MX_EH_LABEL_W												55
#define MX_EH_LABEL_DROP											2
#define MX_EH_CTRL_L												(MX_EH_L + MX_EH_LABEL_W + 4)
#define MX_EH_BROWSE_W												16
#define MX_EH_FILE_EDIT_W											(MX_EH_W - MX_EH_L - MX_EH_BROWSE_W - 3 - MX_EH_CTRL_L)
#define MX_EH_BPR_W													40
#define MX_EH_ROW0_T												7
#define MX_EH_ROW1_T												(MX_EH_ROW0_T + 18)
#define MX_EH_ROW2_T												(MX_EH_ROW1_T + 18)
#define MX_EH_GROUP_T												(MX_EH_ROW2_T + 20)
#define MX_EH_OPT_L													(MX_EH_L + MX_GROUP_LEFT)
#define MX_EH_OPT_LABEL_W											52
#define MX_EH_OPT_C0												(MX_EH_OPT_L + MX_EH_OPT_LABEL_W + 4)
#define MX_EH_OPT_COL_W												62
#define MX_EH_OPT_C1												(MX_EH_OPT_C0 + MX_EH_OPT_COL_W + 4)
#define MX_EH_OPT_C2												(MX_EH_OPT_C1 + MX_EH_OPT_COL_W + 4)
#define MX_EH_OPT_CUSTOM_W											36
#define MX_EH_OPT_ROW0_T											(MX_EH_GROUP_T + MX_GROUP_TOP + 2)
#define MX_EH_OPT_ROW1_T											(MX_EH_OPT_ROW0_T + 15)
#define MX_EH_OPT_ROW2_T											(MX_EH_OPT_ROW1_T + 15)
#define MX_EH_OPT_ROW3_T											(MX_EH_OPT_ROW2_T + 15)
#define MX_EH_GROUP_H												(MX_EH_OPT_ROW3_T + MX_DEF_CHECK_HEIGHT + MX_GROUP_BOTTOM + 3 - MX_EH_GROUP_T)
#define MX_EH_BUTTON_T												(MX_EH_GROUP_T + MX_EH_GROUP_H + 8)
#define MX_EH_H														(MX_EH_BUTTON_T + MX_DEF_BUTTON_HEIGHT + MX_EH_L)

	// == Members.
	// The layout of the Export Hex dialog.
	LSW_WIDGET_LAYOUT CExportHexLayout::m_wlExportHexDialog[] = {
		{
			MX_EXPORT_HEX,							// ltType
			MX_EHI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_EH_W,								// dwWidth
			MX_EH_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			MX_MAKE_WCHAR( _T_55F71713_Export_Hex___ ),	// pwcText
			_LEN_55F71713,							// sTextLen
			MX_EHI_NONE,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EHI_FILE_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_L,								// iLeft
			MX_EH_ROW0_T + MX_EH_LABEL_DROP,		// iTop
			MX_EH_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B58F54C4_Export__File_ ),	// pwcText
			_LEN_B58F54C4,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EHI_FILE_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_CTRL_L,							// iLeft
			MX_EH_ROW0_T,							// iTop
			MX_EH_FILE_EDIT_W,						// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | ES_AUTOHSCROLL,			// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_EHI_BROWSE,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_W - MX_EH_L - MX_EH_BROWSE_W,		// iLeft
			MX_EH_ROW0_T - 1,						// iTop
			MX_EH_BROWSE_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT + 2,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_5111434A____ ),		// pwcText
			_LEN_5111434A,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EHI_TYPE_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_L,								// iLeft
			MX_EH_ROW1_T + MX_EH_LABEL_DROP,		// iTop
			MX_EH_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_EA8A9DAD_Export__Type_ ),	// pwcText
			_LEN_EA8A9DAD,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_EHI_TYPE_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_CTRL_L,							// iLeft
			MX_EH_ROW1_T,							// iTop
			MX_EH_W - MX_EH_L - MX_EH_CTRL_L,		// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EHI_BPR_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_L,								// iLeft
			MX_EH_ROW2_T + MX_EH_LABEL_DROP,		// iTop
			MX_EH_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_F664FE8F__Bytes_Per_Row_ ),	// pwcText
			_LEN_F664FE8F,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EHI_BPR_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_CTRL_L,							// iLeft
			MX_EH_ROW2_T,							// iTop
			MX_EH_BPR_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | ES_NUMBER,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_UPDOWN,							// ltType
			MX_EHI_BPR_UPDOWN,						// wId
			UPDOWN_CLASSW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_CTRL_L + MX_EH_BPR_W,				// iLeft
			MX_EH_ROW2_T,							// iTop
			10,										// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_UPDOWN_STYLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_NOTHOUSANDS,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_EHI_OPTIONS_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_L,								// iLeft
			MX_EH_GROUP_T,							// iTop
			MX_EH_W - MX_EH_L * 2,					// dwWidth
			MX_EH_GROUP_H,							// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_1F88C31B_Options ),	// pwcText
			_LEN_1F88C31B,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EHI_RANGE_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_L,							// iLeft
			MX_EH_OPT_ROW0_T + 1,					// iTop
			MX_EH_OPT_LABEL_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_5246754D_Range ),		// pwcText
			_LEN_5246754D,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EHI_RANGE_ALL,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_C0,							// iLeft
			MX_EH_OPT_ROW0_T,						// iTop
			MX_EH_OPT_COL_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_69E5268D__Entire_File ),	// pwcText
			_LEN_69E5268D,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EHI_RANGE_SEL,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_C1,							// iLeft
			MX_EH_OPT_ROW0_T,						// iTop
			MX_EH_OPT_COL_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_B9CDB3B1_Se_lection ),	// pwcText
			_LEN_B9CDB3B1,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EHI_START_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_L,							// iLeft
			MX_EH_OPT_ROW1_T + 1,					// iTop
			MX_EH_OPT_LABEL_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_F438C207_Start_Address ),	// pwcText
			_LEN_F438C207,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EHI_START_ZERO,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_C0,							// iLeft
			MX_EH_OPT_ROW1_T,						// iTop
			MX_EH_OPT_COL_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_D84A2C26_Always__Zero ),	// pwcText
			_LEN_D84A2C26,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EHI_START_RANGE,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_C1,							// iLeft
			MX_EH_OPT_ROW1_T,						// iTop
			MX_EH_OPT_COL_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_BE34354D_Start_of__Range ),	// pwcText
			_LEN_BE34354D,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EHI_START_CUSTOM,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_C2,							// iLeft
			MX_EH_OPT_ROW1_T,						// iTop
			MX_EH_OPT_CUSTOM_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_3A496722_C_ustom_ ),	// pwcText
			_LEN_3A496722,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EHI_START_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_C2 + MX_EH_OPT_CUSTOM_W,		// iLeft
			MX_EH_OPT_ROW1_T - 2,					// iTop
			MX_EH_W - MX_EH_L - MX_GROUP_RIGHT - MX_EH_OPT_C2 - MX_EH_OPT_CUSTOM_W,	// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | WS_GROUP,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EHI_UNITS_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_L,							// iLeft
			MX_EH_OPT_ROW2_T + 1,					// iTop
			MX_EH_OPT_LABEL_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_56DED88C_Address_Units ),	// pwcText
			_LEN_56DED88C,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EHI_UNITS_BYTES,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_C0,							// iLeft
			MX_EH_OPT_ROW2_T,						// iTop
			MX_EH_OPT_COL_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_299A409E_B_ytes ),	// pwcText
			_LEN_299A409E,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_EHI_UNITS_WORDS,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_C1,							// iLeft
			MX_EH_OPT_ROW2_T,						// iTop
			MX_EH_OPT_COL_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_16A9811D__Words ),	// pwcText
			_LEN_16A9811D,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_EHI_SPACES,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_C0,							// iLeft
			MX_EH_OPT_ROW3_T,						// iTop
			MX_EH_OPT_COL_W,						// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE | WS_GROUP,				// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_CDAE1208_Include__Spaces ),	// pwcText
			_LEN_CDAE1208,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_EHI_LINEFEEDS,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_OPT_C1,							// iLeft
			MX_EH_OPT_ROW3_T,						// iTop
			MX_EH_OPT_COL_W + 20,					// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B3AAC106_Include_Line_feeds ),	// pwcText
			_LEN_B3AAC106,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_EHI_EXPORT,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_EH_W - MX_EH_L - MX_DEF_BUTTON_WIDTH * 2 - 4,	// iLeft
			MX_EH_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE | WS_GROUP,			// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_C5E361D9__Export ),	// pwcText
			_LEN_C5E361D9,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_EHI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EH_W - MX_EH_L - MX_DEF_BUTTON_WIDTH,	// iLeft
			MX_EH_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),	// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_EHI_DIALOG,							// dwParentId
		},
	};

	// The export types, in the order in which they are listed.
	const CExportHexLayout::MX_EXPORT_TYPE CExportHexLayout::m_etTypes[] = {
		{ MX_CA_HEX_TEXT,			_T_LEN_96B4BB97_Hex_Text____txt_,						_T_LEN_82DF8146__txt },
		{ MX_CA_DECIMAL_TEXT,		_T_LEN_DCD38118_Decimal_Text____txt_,					_T_LEN_82DF8146__txt },
		{ MX_CA_BINARY_TEXT,		_T_LEN_3C9AA8D1_Binary_Text____txt_,					_T_LEN_82DF8146__txt },
		{ MX_CA_C_CODE,				_T_LEN_26DDF563_C_Code____cpp___c_,						_T_LEN_54027AA2__cpp },
		{ MX_CA_JAVA_CODE,			_T_LEN_CC7D2E65_Java_Code____java_,						_T_LEN_E68D5FB6__java },
		{ MX_CA_PYTHON_CODE,		_T_LEN_B718D420_Python_Code____py_,						_T_LEN_CB740296__py },
		{ MX_CA_INTEL_8,			_T_LEN_BBB91553_Intel_8_Bit_Hex_Code____hex_,			_T_LEN_6139FB65__hex },
		{ MX_CA_INTEL_16,			_T_LEN_F0042879_Intel_16_Bit_Hex_Code____hex_,			_T_LEN_6139FB65__hex },
		{ MX_CA_INTEL_32,			_T_LEN_41336890_Intel_32_Bit_Hex_Code____hex_,			_T_LEN_6139FB65__hex },
		{ MX_CA_MOTOROLA_S19,		_T_LEN_16D2FB28_Motorola_S19_Records____s19_,			_T_LEN_AE465EA2__s19 },
		{ MX_CA_MOTOROLA_S28,		_T_LEN_60AF6211_Motorola_S28_Records____s28_,			_T_LEN_F26C3DF7__s28 },
		{ MX_CA_MOTOROLA_S37,		_T_LEN_CF2591DD_Motorola_S37_Records____s37_,			_T_LEN_7BC81127__s37 },
		{ MX_CA_HEX_EDITOR_AREAS,	_T_LEN_5A14F893_Hex_Editor_Areas____txt_,				_T_LEN_82DF8146__txt },
		{ MX_CA_WEB_PAGE,			_T_LEN_0AD665DA_Web_Page__HTML_____html___htm_,			_T_LEN_81EE4181__html },
		{ MX_CA_RICH_TEXT,			_T_LEN_E9B3EE28_Rich_Text_Format__RTF_____rtf_,			_T_LEN_D95EC3B0__rtf },
		{ MX_CA_BASE64,				_T_LEN_53DD2C36_Base64____b64_,							_T_LEN_82527D9F__b64 },
		{ MX_CA_UUENCODING,			_T_LEN_16858C8B_Uuencoding____uue___uu_,				_T_LEN_5C03B5CE__uue },
	};

	// == Functions.
	/**
	 * Shows the Export Hex dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	 *
	 * \param _pwParent The parent window.
	 * \param _ehpParms The defaults on input and the choices made on output.
	 * \return Returns TRUE if the user pressed Export.
	 **/
	DWORD CExportHexLayout::CreateExportHexDialog( CWidget * _pwParent, MX_EXPORT_HEX_PARMS &_ehpParms ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlExportHexDialog, std::size( m_wlExportHexDialog ),
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], std::size( m_wlExportHexDialog ), _pwParent, reinterpret_cast<uint64_t>(&_ehpParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		return ipProc > 0 ? TRUE : FALSE;
	}

	/**
	 * Gets the export types, in the order in which they are listed.
	 *
	 * \param _sTotal Receives the number of types.
	 * \return Returns the types.
	 **/
	const CExportHexLayout::MX_EXPORT_TYPE * CExportHexLayout::Types( size_t &_sTotal ) {
		_sTotal = std::size( m_etTypes );
		return m_etTypes;
	}

	/**
	 * Determines whether an export type is made of address records (Intel HEX or Motorola), which use the start-address and
	 *	address-unit options.
	 *
	 * \param _caFormat The Copy As format.
	 * \return Returns true for the Intel HEX and Motorola formats.
	 **/
	bool CExportHexLayout::IsRecordFormat( MX_COPY_AS _caFormat ) {
		switch ( _caFormat ) {
			case MX_CA_INTEL_8 : {}						MX_FALLTHROUGH
			case MX_CA_INTEL_16 : {}					MX_FALLTHROUGH
			case MX_CA_INTEL_32 : {}					MX_FALLTHROUGH
			case MX_CA_MOTOROLA_S19 : {}				MX_FALLTHROUGH
			case MX_CA_MOTOROLA_S28 : {}				MX_FALLTHROUGH
			case MX_CA_MOTOROLA_S37 : { return true; }
			default : { return false; }
		}
	}

}	// namespace mx

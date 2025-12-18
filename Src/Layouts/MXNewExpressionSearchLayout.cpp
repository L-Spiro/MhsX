#include "MXNewExpressionSearchLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "MXLayoutManager.h"
#include <Base/LSWBase.h>


namespace mx {

#define MX_SEARCH_W									410
//#define MX_SEARCH_H									275
#define MX_JUSTIFY									MX_TOP_JUST
#define MX_LEFT_POS									(MX_LEFT_ALIGN + MX_GROUP_LEFT)
#define MX_CENTER_POS								(MX_SEARCH_W / 2)
#define MX_ONE_FOURTH_POS							(MX_LEFT_POS + ((MX_CENTER_POS - MX_LEFT_POS) / 2))
#define MX_SETTINGS_GROUP_TOP						MX_TOP_JUST
#define MX_SETTINGS_GROUP_HEIGHT					(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_DEF_COMBO_HEIGHT + (MX_DEF_STATIC_HEIGHT * 2) + MX_JUSTIFY)
#define MX_SETTINGS_QQ_W							14
#define MX_SETTINGS_QQ_TYPE_COMBO_W					110
#define MX_SETTINGS_QQ_TYPE_W						31
#define MX_BYTES_LABEL_W							16
#define MX_SIZE_GROUP_TOP							(MX_SETTINGS_GROUP_TOP + MX_SETTINGS_GROUP_HEIGHT)//(2)
#define MX_SIZE_GROUP_HEIGHT						(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_DEF_COMBO_HEIGHT + MX_DEF_STATIC_HEIGHT + MX_JUSTIFY)
#define MX_SIZE_DYN_W								MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 3, 0, MX_LEFT_JUST )
#define MX_SIZE_DYN_L								MX_EVEN_DIVIDE_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 3, 0, MX_LEFT_JUST )
#define MX_SIZE_FIXED_W								MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 3, 1, MX_LEFT_JUST )
#define MX_SIZE_FIXED_L								MX_EVEN_DIVIDE_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 3, 1, MX_LEFT_JUST )
#define MX_SIZE_COMBO_W								(MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 3, 2, MX_LEFT_JUST ) - MX_BYTES_LABEL_W - MX_LEFT_ALIGN)
#define MX_SIZE_COMBO_L								MX_EVEN_DIVIDE_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 3, 2, MX_LEFT_JUST )

#define MX_EXP_EDIT_LINES							8
#define MX_EXPRESSION_GROUP_TOP						(MX_SIZE_GROUP_TOP + MX_SIZE_GROUP_HEIGHT)
#define MX_EXPRESSION_GROUP_HEIGHT					(MX_GROUP_TOP + MX_GROUP_BOTTOM + (MX_DEF_EDIT_HEIGHT + 8 * MX_EXP_EDIT_LINES) + (MX_DEF_COMBO_HEIGHT + MX_JUSTIFY) + (MX_DEF_EDIT_HEIGHT + MX_JUSTIFY))
#define MX_EXPRESSION_DUMMY_W						63
#define MX_EXPRESSION_RESULT_LABEL_W				21
#define MX_EXPRESSION_DUMMY_COMBO_W_OFF				(MX_EXPRESSION_DUMMY_W - MX_EXPRESSION_RESULT_LABEL_W)
#define MX_EXPRESSION_DUMMY_COMBO_L					(MX_EVEN_DIVIDE_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 2, 0, MX_LEFT_JUST ) + MX_EXPRESSION_DUMMY_W)
#define MX_EXPRESSION_DUMMY_COMBO_W					(MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 2, 0, MX_LEFT_JUST ) - MX_EXPRESSION_DUMMY_W)
#define MX_EXPRESSION_DUMMY_LABEL_W					(MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 2, 1, MX_LEFT_JUST ))
#define MX_EXPRESSION_TEST_LABEL_L					MX_EVEN_DIVIDE_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 2, 1, MX_LEFT_JUST )
#define MX_EXPRESSION_TEST_LABEL_W					(MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 2, 1, MX_LEFT_JUST ) + MX_EXPRESSION_DUMMY_COMBO_W_OFF)
#define MX_EXPRESSION_RESULT_LABEL_L				(MX_EVEN_DIVIDE_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 2, 1, MX_LEFT_JUST ) - MX_EXPRESSION_DUMMY_COMBO_W_OFF)
#define MX_EXPRESSION_RESULT_COMBO_L				(MX_EVEN_DIVIDE_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 2, 0, MX_LEFT_JUST ) + MX_EXPRESSION_RESULT_LABEL_W)
#define MX_EXPRESSION_RESULT_COMBO_W				(MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - (MX_LEFT_POS) * 2, MX_LEFT_POS, 2, 0, MX_LEFT_JUST ) - MX_EXPRESSION_RESULT_LABEL_W - MX_EXPRESSION_DUMMY_COMBO_W_OFF)

#define MX_OPTIONS_GROUP_TOP						(MX_EXPRESSION_GROUP_TOP + MX_EXPRESSION_GROUP_HEIGHT)
#define MX_OPTIONS_GROUP_HEIGHT						(MX_GROUP_TOP + MX_GROUP_BOTTOM + (MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT))
#define MX_OPTIONS_GROUP_L							MX_EVEN_DIVIDE_EX( MX_SEARCH_W - (MX_LEFT_ALIGN) * 2, MX_LEFT_JUST, 2, 0, MX_LEFT_JUST )
#define MX_OPTIONS_GROUP_INNER_L					(MX_OPTIONS_GROUP_L + MX_GROUP_LEFT)
#define MX_OPTIONS_GROUP_W							MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - (MX_LEFT_ALIGN) * 2, MX_LEFT_JUST, 2, 0, MX_LEFT_JUST )//(MX_CENTER_POS - MX_LEFT_ALIGN - 1)
#define MX_OPTIONS_GROUP_INNER_W					(MX_OPTIONS_GROUP_W - (MX_GROUP_LEFT * 2))
#define MX_OPTIONS_GROUP_L0							MX_EVEN_DIVIDE_EX( MX_OPTIONS_GROUP_INNER_W, MX_OPTIONS_GROUP_INNER_L, 2, 0, MX_LEFT_JUST )
#define MX_OPTIONS_GROUP_W0							MX_EVEN_DIVIDE_WIDTH_EX( MX_OPTIONS_GROUP_INNER_W, MX_OPTIONS_GROUP_INNER_L, 2, 0, MX_LEFT_JUST )
#define MX_OPTIONS_GROUP_L1							MX_EVEN_DIVIDE_EX( MX_OPTIONS_GROUP_INNER_W, MX_OPTIONS_GROUP_INNER_L, 2, 1, MX_LEFT_JUST )
#define MX_OPTIONS_GROUP_W1							MX_EVEN_DIVIDE_WIDTH_EX( MX_OPTIONS_GROUP_INNER_W, MX_OPTIONS_GROUP_INNER_L, 2, 1, MX_LEFT_JUST )
#define MX_OPTIONS_ALIGN_W							41
#define MX_OPTIONS_ALIGN_COMBO_W					(MX_CENTER_POS - (MX_LEFT_POS + MX_OPTIONS_ALIGN_W) - MX_LEFT_ALIGN)

#define MX_SEARCH_RANGE_TOP							(MX_EXPRESSION_GROUP_TOP + MX_EXPRESSION_GROUP_HEIGHT)//(MX_OPTIONS_GROUP_TOP + MX_OPTIONS_GROUP_HEIGHT)
#define MX_SEARCH_RANGE_H							(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT)
#define MX_SEARCH_RANGE_L							MX_EVEN_DIVIDE_EX( MX_SEARCH_W - (MX_LEFT_ALIGN) * 2, MX_LEFT_JUST, 2, 1, MX_LEFT_JUST )//(MX_CENTER_POS + 1/* + (MX_LEFT_ALIGN / 2)*/)
#define MX_SEARCH_RANGE_INNER_L						(MX_SEARCH_RANGE_L + MX_GROUP_LEFT)
#define MX_SEARCH_RANGE_W							MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_W - (MX_LEFT_ALIGN) * 2, MX_LEFT_JUST, 2, 1, MX_LEFT_JUST )//(MX_CENTER_POS - MX_LEFT_ALIGN - 1)
#define MX_SEARCH_RANGE_INNER_W						(MX_SEARCH_RANGE_W - (MX_GROUP_LEFT * 2))
#define MX_SEARCH_RANGE_L0							MX_EVEN_DIVIDE_EX( MX_SEARCH_RANGE_INNER_W, MX_SEARCH_RANGE_INNER_L, 2, 0, MX_LEFT_JUST )
#define MX_SEARCH_RANGE_W0							MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_RANGE_INNER_W, MX_SEARCH_RANGE_INNER_L, 2, 0, MX_LEFT_JUST )
#define MX_SEARCH_RANGE_L1							MX_EVEN_DIVIDE_EX( MX_SEARCH_RANGE_INNER_W, MX_SEARCH_RANGE_INNER_L, 2, 1, MX_LEFT_JUST )
#define MX_SEARCH_RANGE_W1							MX_EVEN_DIVIDE_WIDTH_EX( MX_SEARCH_RANGE_INNER_W, MX_SEARCH_RANGE_INNER_L, 2, 1, MX_LEFT_JUST )


#define MX_SEARCH_H									(MX_SEARCH_RANGE_TOP + MX_SEARCH_RANGE_H + (MX_TOP_JUST * 4 + MX_DEF_BUTTON_HEIGHT))


	// == Members.
	// The layout for the New Expression Search dialog.
	LSW_WIDGET_LAYOUT CNewExpressionSearchLayout::m_wlNewExpressionSearchDialog[] = {
		{
			MX_NEW_EXPRESSION_TYPE_SEARCH,			// ltType
			MX_NESI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_SEARCH_W,							// dwWidth
			MX_SEARCH_H,							// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_408D6284_Expression_Search ),																								// pwcText
			_LEN_408D6284,							// sTextLen
			MX_NESI_NONE,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_NESI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_SEARCH_H - MX_TOP_JUST - MX_DEF_BUTTON_HEIGHT,											// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,				// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),		// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_NESI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SEARCH_W - MX_DEF_BUTTON_WIDTH - MX_LEFT_ALIGN,											// iLeft
			MX_SEARCH_H - MX_TOP_JUST - MX_DEF_BUTTON_HEIGHT,											// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,				// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),	// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},

		
		// ==== Settings ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_NESI_SETTINGS_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_SETTINGS_GROUP_TOP,					// iTop
			MX_SEARCH_W - (MX_LEFT_ALIGN * 2),		// dwWidth
			MX_SETTINGS_GROUP_HEIGHT,				// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_1C33C293_Settings ),	// pwcText
			_LEN_1C33C293,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_SETTINGS_QQ_LABEL,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_POS,							// iLeft
			MX_SETTINGS_GROUP_TOP + MX_GROUP_TOP + MX_LABEL_JUST,											// iTop
			MX_SETTINGS_QQ_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_6BD7FDF5_____ ),		// pwcText
			_LEN_6BD7FDF5,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NESI_SETTINGS_QQ_COMBO,				// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_POS + MX_SETTINGS_QQ_W,			// iLeft
			MX_SETTINGS_GROUP_TOP + MX_GROUP_TOP,	// iTop
			MX_CENTER_POS - (MX_LEFT_POS + MX_SETTINGS_QQ_W) - MX_LEFT_ALIGN,								// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_SETTINGS_QQ_TYPE_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SEARCH_W / 2,						// iLeft
			MX_SETTINGS_GROUP_TOP + MX_GROUP_TOP + MX_LABEL_JUST,											// iTop
			MX_SETTINGS_QQ_TYPE_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0FC36A5B____Type__ ),	// pwcText
			_LEN_0FC36A5B,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NESI_SETTINGS_QQ_TYPE_COMBO,			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SEARCH_W / 2 + MX_SETTINGS_QQ_TYPE_W,// iLeft
			MX_SETTINGS_GROUP_TOP + MX_GROUP_TOP,	// iTop
			MX_SEARCH_W - MX_LEFT_POS - (MX_SEARCH_W / 2 + MX_SETTINGS_QQ_TYPE_W),//MX_SETTINGS_QQ_TYPE_COMBO_W,			// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_SETTINGS_DESC_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_POS,							// iLeft
			MX_SETTINGS_GROUP_TOP + MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_JUSTIFY,						// iTop
			MX_SEARCH_W - ((MX_LEFT_POS) * 2),		// dwWidth
			MX_DEF_STATIC_HEIGHT * 2,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_D54B3E50__________will_be_the_address_currently_being_searched___The_dereference_operator_______address_______can_be_used_to_access_this_and_neighboring_addresses__u32____4____u32_____1_ ),	// pwcText
			_LEN_D54B3E50,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},


		// ==== Size ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_NESI_SIZE_GROUP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_SIZE_GROUP_TOP,						// iTop
			MX_SEARCH_W - (MX_LEFT_ALIGN * 2),		// dwWidth
			MX_SIZE_GROUP_HEIGHT,					// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_57F28B54_Size ),		// pwcText
			_LEN_57F28B54,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},

		{
			LSW_LT_RADIO,							// ltType
			MX_NESI_SIZE_DYN_SIZE_RADIO,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_SIZE_DYN_L,							// iLeft
			MX_SIZE_GROUP_TOP + MX_GROUP_TOP + MX_CHECK_JUST,												// iTop
			//MX_ONE_FOURTH_POS - MX_LEFT_POS - 1,	// dwWidth
			MX_SIZE_DYN_W,							// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,					// dwStyleEx
			MX_MAKE_WCHAR( _T_253E3D56_Dynamic_Size ),														// pwcText
			_LEN_253E3D56,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_NESI_SIZE_FIXED_SIZE_RADIO,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SIZE_FIXED_L,						// iLeft
			MX_SIZE_GROUP_TOP + MX_GROUP_TOP + MX_CHECK_JUST,												// iTop
			//MX_CENTER_POS - MX_ONE_FOURTH_POS - 1,	// dwWidth
			MX_SIZE_FIXED_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,					// dwStyleEx
			MX_MAKE_WCHAR( _T_6DD0496C_Fixed_Size_ ),														// pwcText
			_LEN_6DD0496C,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NESI_SIZE_FIXED_SIZE_COMBO,			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SIZE_COMBO_L,						// iLeft
			MX_SIZE_GROUP_TOP + MX_GROUP_TOP,		// iTop
			MX_SIZE_COMBO_W,						// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_SIZE_FIXED_SIZE_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SIZE_COMBO_L + MX_SIZE_COMBO_W + MX_LEFT_ALIGN,												// iLeft
			MX_SIZE_GROUP_TOP + MX_GROUP_TOP + MX_LABEL_JUST,												// iTop
			MX_BYTES_LABEL_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_70586CCA_Bytes ),		// pwcText
			_LEN_70586CCA,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_SIZE_SIZE_LABEL,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_POS,							// iLeft
			MX_SIZE_GROUP_TOP + MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_JUSTIFY,							// iTop
			MX_SEARCH_W - ((MX_LEFT_POS) * 2),		// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8D0B139B_Expression_returns_a_positive_value_to_indicate_the_data_size_ ),	// pwcText
			_LEN_8D0B139B,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},


		// ==== Expression ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_NESI_EXPRESSION_GROUP,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_EXPRESSION_GROUP_TOP,				// iTop
			MX_SEARCH_W - (MX_LEFT_ALIGN * 2),		// dwWidth
			MX_EXPRESSION_GROUP_HEIGHT,				// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_976D55D1_Expression ),// pwcText
			_LEN_976D55D1,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},

		{
			LSW_LT_EDIT,							// ltType
			MX_NESI_EXPRESSION_EDIT,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_POS,							// iLeft
			MX_EXPRESSION_GROUP_TOP + MX_GROUP_TOP,	// iTop
			MX_SEARCH_W - ((MX_LEFT_POS) * 2),		// dwWidth
			MX_DEF_EDIT_HEIGHT + 8 * MX_EXP_EDIT_LINES,														// dwHeight
			MX_EDITSTYLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_WANTRETURN,						// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_EXPRESSION_DUMMY_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_POS,							// iLeft
			MX_EXPRESSION_GROUP_TOP + MX_GROUP_TOP + (MX_DEF_EDIT_HEIGHT + 8 * MX_EXP_EDIT_LINES) + MX_LABEL_JUST + MX_JUSTIFY,	// iTop
			MX_EXPRESSION_DUMMY_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8355FE64_Dummy_Value_for____ ),												// pwcText
			_LEN_8355FE64,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NESI_EXPRESSION_DUMMY_COMBO,			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EXPRESSION_DUMMY_COMBO_L,			// iLeft
			MX_EXPRESSION_GROUP_TOP + MX_GROUP_TOP + (MX_DEF_EDIT_HEIGHT + 8 * MX_EXP_EDIT_LINES) + MX_JUSTIFY,			// iTop
			MX_EXPRESSION_DUMMY_COMBO_W,			// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_EXPRESSION_DUMMY_HELP_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EXPRESSION_TEST_LABEL_L,				// iLeft
			MX_EXPRESSION_GROUP_TOP + MX_GROUP_TOP + (MX_DEF_EDIT_HEIGHT + 8 * MX_EXP_EDIT_LINES) + MX_LABEL_JUST + MX_JUSTIFY,	// iTop
			MX_EXPRESSION_DUMMY_LABEL_W,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_A0D42CEF_Use_to_test_your_expression_ ),										// pwcText
			_LEN_A0D42CEF,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_EXPRESSION_RESULT_LABEL,		// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_POS,							// iLeft
			MX_EXPRESSION_GROUP_TOP + MX_GROUP_TOP + (MX_DEF_EDIT_HEIGHT + 8 * MX_EXP_EDIT_LINES) + MX_JUSTIFY + MX_DEF_COMBO_HEIGHT + MX_LABEL_JUST + MX_JUSTIFY,	// iTop
			MX_EXPRESSION_RESULT_LABEL_W,			// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_5F1E24BC_Result_ ),	// pwcText
			_LEN_5F1E24BC,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_NESI_EXPRESSION_RESULT_EDIT,			// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EXPRESSION_RESULT_COMBO_L,			// iLeft
			MX_EXPRESSION_GROUP_TOP + MX_GROUP_TOP + (MX_DEF_EDIT_HEIGHT + 8 * MX_EXP_EDIT_LINES) + MX_JUSTIFY + MX_DEF_COMBO_HEIGHT + MX_JUSTIFY,	// iTop
			MX_EXPRESSION_RESULT_COMBO_W,			// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_EXPRESSION_RESULT_HELP_LABEL,	// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EXPRESSION_RESULT_LABEL_L,			// iLeft
			MX_EXPRESSION_GROUP_TOP + MX_GROUP_TOP + (MX_DEF_EDIT_HEIGHT + 8 * MX_EXP_EDIT_LINES) + MX_JUSTIFY + MX_DEF_COMBO_HEIGHT + MX_LABEL_JUST + MX_JUSTIFY,	// iTop
			MX_EXPRESSION_TEST_LABEL_W,				// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},


		// ==== Options ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_NESI_OPTIONS_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OPTIONS_GROUP_L,						// iLeft
			MX_OPTIONS_GROUP_TOP,					// iTop
			MX_OPTIONS_GROUP_W,						// dwWidth
			MX_OPTIONS_GROUP_HEIGHT,				// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_1F88C31B_Options ),	// pwcText
			_LEN_1F88C31B,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_OPTIONS_ALIGN_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OPTIONS_GROUP_L0,					// iLeft
			MX_OPTIONS_GROUP_TOP + MX_GROUP_TOP,	// iTop
			MX_OPTIONS_GROUP_W0,					// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_39A2E62E_Alignment_ ),// pwcText
			_LEN_39A2E62E,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NESI_OPTIONS_ALIGN_COMBO,			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OPTIONS_GROUP_L0,					// iLeft
			MX_OPTIONS_GROUP_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT,						// iTop
			MX_OPTIONS_GROUP_W0,					// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_NESI_OPTIONS_GENERAL_OPTIONS_BUTTON,	// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OPTIONS_GROUP_L1,					// iLeft
			MX_OPTIONS_GROUP_TOP + MX_GROUP_TOP,	// iTop
			MX_OPTIONS_GROUP_W1,					// dwWidth
			MX_DEF_STATIC_HEIGHT + MX_DEF_BUTTON_HEIGHT,									// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_2A4DD765_Search_Options ),									// pwcText
			_LEN_2A4DD765,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},


		// ==== Search Range ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_NESI_SEARCH_RANGE_GROUP,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SEARCH_RANGE_L,						// iLeft
			MX_SEARCH_RANGE_TOP,					// iTop
			MX_SEARCH_RANGE_W,						// dwWidth
			MX_SEARCH_RANGE_H,						// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8436D248_Search_Range ),										// pwcText
			_LEN_8436D248,																	// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_FROM_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SEARCH_RANGE_L0,						// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP,		// iTop
			MX_SEARCH_RANGE_W0,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_857372A6_From_ ),		// pwcText
			_LEN_857372A6,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NESI_TO_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SEARCH_RANGE_L1,						// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP,		// iTop
			MX_SEARCH_RANGE_W1,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B09DF1A4_To_ ),		// pwcText
			_LEN_B09DF1A4,							// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NESI_FROM_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SEARCH_RANGE_L0,						// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT,						// iTop
			MX_SEARCH_RANGE_W0,						// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NESI_TO_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SEARCH_RANGE_L1,						// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT,						// iTop
			MX_SEARCH_RANGE_W1,						// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NESI_DIALOG,							// dwParentId
		},

		
	};

#undef MX_SEARCH_H

#undef MX_SEARCH_RANGE_W1
#undef MX_SEARCH_RANGE_L1
#undef MX_SEARCH_RANGE_W0
#undef MX_SEARCH_RANGE_L0
//#undef MX_SEARCH_RANGE_C
#undef MX_SEARCH_RANGE_INNER_W
#undef MX_SEARCH_RANGE_W
//#undef MX_SEARCH_RANGE_R
#undef MX_SEARCH_RANGE_INNER_L
#undef MX_SEARCH_RANGE_L
#undef MX_SEARCH_RANGE_H
#undef MX_SEARCH_RANGE_TOP
#undef MX_BYTES_LABEL_W
#undef MX_OPTIONS_ALIGN_COMBO_W
#undef MX_OPTIONS_ALIGN_W
#undef MX_OPTIONS_GROUP_W1
#undef MX_OPTIONS_GROUP_L1
#undef MX_OPTIONS_GROUP_W0
#undef MX_OPTIONS_GROUP_L0
//#undef MX_OPTIONS_GROUP_C_R
//#undef MX_OPTIONS_GROUP_C_L
//#undef MX_OPTIONS_GROUP_C
#undef MX_OPTIONS_GROUP_W
#undef MX_OPTIONS_GROUP_HEIGHT
#undef MX_OPTIONS_GROUP_TOP
#undef MX_EXPRESSION_RESULT_COMBO_W
#undef MX_EXPRESSION_RESULT_COMBO_L
#undef MX_EXPRESSION_RESULT_LABEL_W
#undef MX_EXPRESSION_TEST_LABEL_W
#undef MX_EXPRESSION_TEST_LABEL_L
#undef MX_EXPRESSION_DUMMY_LABEL_W
#undef MX_EXPRESSION_DUMMY_COMBO_W
#undef MX_EXPRESSION_DUMMY_COMBO_L
#undef MX_EXPRESSION_DUMMY_W
#undef MX_EXPRESSION_GROUP_HEIGHT
#undef MX_EXPRESSION_GROUP_TOP
#undef MX_EXP_EDIT_LINES
#undef MX_SIZE_COMBO_L
#undef MX_SIZE_COMBO_W
#undef MX_SIZE_FIXED_L
#undef MX_SIZE_FIXED_W
#undef MX_SIZE_DYN_L
#undef MX_SIZE_DYN_W
#undef MX_SIZE_GROUP_HEIGHT
#undef MX_SETTINGS_QQ_TYPE_W
#undef MX_SETTINGS_QQ_TYPE_COMBO_W
#undef MX_SETTINGS_QQ_COMBO_W
#undef MX_SETTINGS_QQ_W
#undef MX_SETTINGS_GROUP_HEIGHT
#undef MX_SETTINGS_GROUP_TOP
#undef MX_SIZE_GROUP_TOP
#undef MX_ONE_FOURTH_POS
#undef MX_CENTER_POS
#undef MX_LEFT_POS
#undef MX_JUSTIFY
#undef MX_LEFT_ALIGN
#undef MX_SEARCH_W


	// == Functions.
	// Creates the New Data-Type Search dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	DWORD CNewExpressionSearchLayout::CreateNewExpressionSearchDialog( CWidget * _pwParent, CMemHack * _pmhMemHack ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlNewExpressionSearchDialog, std::size( m_wlNewExpressionSearchDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], std::size( m_wlNewExpressionSearchDialog ), _pwParent, reinterpret_cast<uint64_t>(_pmhMemHack) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

}	// namespace mx

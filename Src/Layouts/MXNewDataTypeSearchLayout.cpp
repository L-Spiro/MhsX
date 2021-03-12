#include "MXNewDataTypeSearchLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "MXLayoutManager.h"
#include <Base/LSWBase.h>


namespace mx {

#define MX_SEARCH_W									200
#define MX_SEARCH_H									(175 + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + 2)
#define MX_SEARCH_GROUP_TOP							2
#define MX_LEFT_ALIGN								4
#define MX_CENTER									(MX_SEARCH_W / 2)
#define MX_WIDTH_TO_CENTER( L )						((MX_SEARCH_W / 2) - (L))
#define MX_SEARCH_GROUP_HEIGHT						(MX_DEF_COMBO_HEIGHT * 2 + MX_DEF_STATIC_HEIGHT * 2 + MX_GROUP_TOP + MX_GROUP_BOTTOM + 2 + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + 2)
#define MX_OPTIONS_TOP								(MX_SEARCH_GROUP_HEIGHT + MX_SEARCH_GROUP_TOP)
#define MX_OPTIONS_H								(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_DEF_CHECK_HEIGHT * 2 + MX_DEF_BUTTON_HEIGHT)
#define MX_SEARCH_RANGE_TOP							(MX_OPTIONS_TOP + MX_OPTIONS_H)
#define MX_SEARCH_RANGE_H							(MX_GROUP_TOP + MX_GROUP_BOTTOM + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT)

	// == Members.
	// The layout for the New Data-Type Search dialog.
	LSW_WIDGET_LAYOUT CNewDataTypeSearchLayout::m_wlNewDataTypeSearchDialog[] = {
		{
			MX_NEW_DATA_TYPE_SEARCH,				// ltType
			MX_NDSI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_SEARCH_W,							// dwWidth
			MX_SEARCH_H,							// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_2CB9A1E0_Data_Type_Search ),																								// pwcText
			_LEN_2CB9A1E0,							// sTextLen
			MX_NDSI_NONE,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_NDSI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_ALIGN,																				// iLeft
			MX_SEARCH_H - MX_LEFT_ALIGN - MX_DEF_BUTTON_HEIGHT,											// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,				// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),															// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_NDSI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SEARCH_W - MX_DEF_BUTTON_WIDTH - MX_LEFT_ALIGN,											// iLeft
			MX_SEARCH_H - MX_LEFT_ALIGN - MX_DEF_BUTTON_HEIGHT,											// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,				// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),														// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},

		// ==== Search ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_NDSI_SEARCH_GROUP,					// wId
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
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NDSI_DATA_TYPE_LABEL,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP,		// iTop
			MX_WIDTH_TO_CENTER( MX_LEFT_ALIGN + MX_GROUP_LEFT ) - MX_GROUP_RIGHT,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_6ED0C648_Data_Type ),	// pwcText
			_LEN_6ED0C648,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NDSI_EVALUATION_TYPE_LABEL,			// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER,								// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP,		// iTop
			(MX_SEARCH_W - MX_LEFT_ALIGN - MX_GROUP_RIGHT - MX_CENTER),									// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_92EDFE2F_Evaluation_Type ),												// pwcText
			_LEN_92EDFE2F,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NDSI_DATA_TYPE_COMBO,				// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT,									// iTop
			MX_WIDTH_TO_CENTER( MX_LEFT_ALIGN + MX_GROUP_LEFT ) - MX_GROUP_RIGHT,						// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NDSI_EVALUATION_TYPE_COMBO,			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER,								// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT,									// iTop
			(MX_SEARCH_W - MX_LEFT_ALIGN - MX_GROUP_RIGHT - MX_CENTER),									// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_NDSI_LVAL_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + 2,		// iTop
			MX_WIDTH_TO_CENTER( MX_LEFT_ALIGN + MX_GROUP_LEFT ) - MX_GROUP_RIGHT,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8C044A93_Value_to_Find_ ),												// pwcText
			_LEN_8C044A93,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NDSI_RVAL_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER,								// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + 2,		// iTop
			(MX_SEARCH_W - MX_LEFT_ALIGN - MX_GROUP_RIGHT - MX_CENTER),									// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B09DF1A4_To_ ),															// pwcText
			_LEN_B09DF1A4,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NDSI_LVAL_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + 2 + MX_DEF_STATIC_HEIGHT,		// iTop
			MX_WIDTH_TO_CENTER( MX_LEFT_ALIGN + MX_GROUP_LEFT ) - MX_GROUP_RIGHT,											// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NDSI_RVAL_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER,								// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + 2 + MX_DEF_STATIC_HEIGHT,		// iTop
			(MX_SEARCH_W - MX_LEFT_ALIGN - MX_GROUP_RIGHT - MX_CENTER),														// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_NDSI_EXP_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + (MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + 2) * 2,	// iTop
			MX_SEARCH_W - (MX_GROUP_RIGHT + MX_LEFT_ALIGN + MX_LEFT_ALIGN + MX_GROUP_LEFT),				// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_68C09196_Value_____ ),// pwcText
			_LEN_68C09196,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NDSI_EXP_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_SEARCH_GROUP_TOP + MX_GROUP_TOP + (MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + 2) * 2 + MX_DEF_STATIC_HEIGHT,	// iTop
			MX_SEARCH_W - (MX_GROUP_RIGHT + MX_LEFT_ALIGN + MX_LEFT_ALIGN + MX_GROUP_LEFT),										// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},

		// ==== Options ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_NDSI_OPTIONS_GROUP,					// wId
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
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_NDSI_ALIGNED,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_OPTIONS_TOP + MX_GROUP_TOP,			// iTop
			MX_CENTER - (MX_LEFT_ALIGN + MX_GROUP_LEFT),				// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_22E9689D_Aligned ),	// pwcText
			_LEN_22E9689D,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{	// Invert Results.
			LSW_LT_CHECK,							// ltType
			MX_NDSI_INVERT,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER,								// iLeft
			MX_OPTIONS_TOP + MX_GROUP_TOP,			// iTop
			(MX_SEARCH_W - MX_LEFT_ALIGN - MX_GROUP_RIGHT - MX_CENTER),	// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_EE3A07C4_Invert_Search ),	// pwcText
			_LEN_EE3A07C4,								// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_NDSI_SAO_CHECK,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_OPTIONS_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT,					// iTop
			MX_SEARCH_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT) * 2),					// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_7422F11D_Enable____Same_as_Original____Sub_Search ),	// pwcText
			_LEN_7422F11D,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_NDSI_GENERAL_SEARCH_OPTIONS_BUTTON,	// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_OPTIONS_TOP + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2,						// iTop
			MX_SEARCH_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT) * 2),							// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_5FE14262_General_Search_Options ),							// pwcText
			_LEN_5FE14262,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},


		// ==== Search Range ==== //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_NDSI_SEARCH_RANGE_GROUP,				// wId
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
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NDSI_FROM_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP,		// iTop
			MX_WIDTH_TO_CENTER( MX_LEFT_ALIGN + MX_GROUP_LEFT ) - MX_GROUP_RIGHT,						// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_857372A6_From_ ),		// pwcText
			_LEN_857372A6,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_NDSI_TO_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER,								// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP,		// iTop
			(MX_SEARCH_W - MX_LEFT_ALIGN - MX_GROUP_RIGHT - MX_CENTER),									// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_B09DF1A4_To_ ),		// pwcText
			_LEN_B09DF1A4,							// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NDSI_FROM_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT,			// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT,									// iTop
			MX_WIDTH_TO_CENTER( MX_LEFT_ALIGN + MX_GROUP_LEFT ) - MX_GROUP_RIGHT,						// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_NDSI_TO_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER,								// iLeft
			MX_SEARCH_RANGE_TOP + MX_GROUP_TOP + MX_DEF_STATIC_HEIGHT,									// iTop
			(MX_SEARCH_W - MX_LEFT_ALIGN - MX_GROUP_RIGHT - MX_CENTER),									// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},

		// ==== Description. ==== //
		{
			LSW_LT_LABEL,							// ltType
			MX_NDSI_DESC_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_SEARCH_RANGE_TOP + MX_SEARCH_RANGE_H,// iTop
			MX_SEARCH_W - ((MX_LEFT_ALIGN) * 2),	// dwWidth
			MX_DEF_STATIC_HEIGHT * 3,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			L"",									// pwcText
			0,										// sTextLen
			MX_NDSI_DIALOG,							// dwParentId
		},
	};
#undef MX_SEARCH_RANGE_H
#undef MX_SEARCH_RANGE_TOP
#undef MX_OPTIONS_H
#undef MX_OPTIONS_TOP
#undef MX_SEARCH_GROUP_HEIGHT
#undef MX_WIDTH_TO_CENTER
#undef MX_CENTER
#undef MX_LEFT_ALIGN
#undef MX_SEARCH_GROUP_TOP
#undef MX_SEARCH_H
#undef MX_SEARCH_W

	// == Functions.
	// Creates the New Data-Type Search dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	DWORD CNewDataTypeSearchLayout::CreateNewDataTypeSearchDialog( CWidget * _pwParent, CMemHack * _pmhMemHack ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlNewDataTypeSearchDialog, MX_ELEMENTS( m_wlNewDataTypeSearchDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], MX_ELEMENTS( m_wlNewDataTypeSearchDialog ), _pwParent, reinterpret_cast<uint64_t>(_pmhMemHack) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

}	// namespace mx

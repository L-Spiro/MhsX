#include "MXOptionsLayout.h"
#include "../System/MXSystem.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>

namespace mx {

	// == Members.
	// The layout for the Open Process dialog.
	LSW_WIDGET_LAYOUT COptionsLayout::m_wlOptionsDialog[] = {
		{
			MX_OPTIONS_WINDOW,						// ltType
			MX_OI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			260,									// dwWidth
			182,									// dwHeight
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
			2,										// iTop
			48,									// dwWidth
			156,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,																// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,												// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_DIALOG,							// dwParentId
		},
		
	};

	// General section.
#define MX_ROW_HEIGHT 14
	LSW_WIDGET_LAYOUT COptionsLayout::m_wlOptionsGeneral[] = {
		{
			MX_OPTIONS_GENERAL,						// ltType
			MX_OI_GENERAL,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			53,										// iLeft
			2,										// iTop
			204,									// dwWidth
			156,									// dwHeight
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
			204,									// dwWidth
			67,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,																									// dwStyle
			0,																																			// dwStyleEx
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
			6,										// iLeft
			11 + MX_ROW_HEIGHT * 0,					// iTop
			90,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | SS_LEFT,																							// dwStyle
			0,																																			// dwStyleEx
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
			102,										// iLeft
			9 + MX_ROW_HEIGHT * 0,					// iTop
			36,										// dwWidth
			11,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN | CBS_HASSTRINGS,																	// dwStyle
			0,																																			// dwStyleEx
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
			144,									// iLeft
			11 + MX_ROW_HEIGHT * 0,					// iTop
			55,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | SS_LEFT,																							// dwStyle
			0,																																			// dwStyleEx
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
			6,										// iLeft
			11 + MX_ROW_HEIGHT * 1,					// iTop
			90,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | SS_LEFT,																							// dwStyle
			0,																																			// dwStyleEx
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
			102,									// iLeft
			9 + MX_ROW_HEIGHT * 1,					// iTop
			36,										// dwWidth
			11,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN | CBS_HASSTRINGS,																	// dwStyle
			0,																																			// dwStyleEx
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
			144,									// iLeft
			11 + MX_ROW_HEIGHT * 1,					// iTop
			55,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | SS_LEFT,																							// dwStyle
			0,																																			// dwStyleEx
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
			6,										// iLeft
			11 + MX_ROW_HEIGHT * 2,					// iTop
			90,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | SS_LEFT,																							// dwStyle
			0,																																			// dwStyleEx
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
			102,										// iLeft
			9 + MX_ROW_HEIGHT * 2,					// iTop
			36,										// dwWidth
			11,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN | CBS_HASSTRINGS,																	// dwStyle
			0,																																			// dwStyleEx
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
			144,									// iLeft
			11 + MX_ROW_HEIGHT * 2,					// iTop
			55,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | SS_LEFT,																							// dwStyle
			0,																																			// dwStyleEx
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
			6,										// iLeft
			11 + MX_ROW_HEIGHT * 3,					// iTop
			90,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | SS_LEFT,																							// dwStyle
			0,																																			// dwStyleEx
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
			102,										// iLeft
			9 + MX_ROW_HEIGHT * 3,					// iTop
			36,										// dwWidth
			11,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN | CBS_HASSTRINGS,																	// dwStyle
			0,																																			// dwStyleEx
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
			144,									// iLeft
			11 + MX_ROW_HEIGHT * 3,					// iTop
			55,										// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | SS_LEFT,																							// dwStyle
			0,																																			// dwStyleEx
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
			63 + 4,									// iTop
			204,									// dwWidth
			29,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,																									// dwStyle
			0,																																			// dwStyleEx
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
			6,										// iLeft
			71 + 4,									// iTop
			91,										// dwWidth
			9,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | BS_AUTOCHECKBOX,																								// dwStyle
			0,																																			// dwStyleEx
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
			102,									// iLeft
			71 + 4,									// iTop
			58,										// dwWidth
			9,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | BS_AUTOCHECKBOX,																								// dwStyle
			0,																																			// dwStyleEx
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
			6,										// iLeft
			80 + 4,									// iTop
			91,										// dwWidth
			9,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | BS_AUTOCHECKBOX,																								// dwStyle
			0,																																			// dwStyleEx
			MX_MAKE_WCHAR( _T_8CD4CB22_Data_Type_Ranges ),	// pwcText
			_LEN_8CD4CB22,											// sTextLen
			MX_OI_GENERAL,							// dwParentId
		},

	};
#undef MX_ROW_HEIGHT

	// == Functions.
	// Creates the Options dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	BOOL COptionsLayout::CreateOptionsDialog( CWidget * _pwParent ) {
		std::vector<std::string> sStrings;
		std::vector<std::wstring> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlOptionsDialog, MX_ELEMENTS( m_wlOptionsDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		INT_PTR ipProc = lsw::CBase::LayoutManager()->DialogBoxX( &vLayouts[0], MX_ELEMENTS( m_wlOptionsDialog ), _pwParent );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != -1 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

	// Creates the general options page.
	CWidget * COptionsLayout::CreateGeneralPage( CWidget * _pwParent ) {
		return CreatePage( _pwParent, m_wlOptionsGeneral, MX_ELEMENTS( m_wlOptionsGeneral ) );
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

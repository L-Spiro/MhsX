#include "MXAddFoundAddressWindowLayout.h"
#include "../MemHack/MXMemHack.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>


namespace mx {

#define MX_MAIN_LABEL_W										50
#define MX_MAIN_EDIT_W										98
#define MX_INNER_L											(MX_GROUP_L + MX_GROUP_LEFT)
#define MX_INNER_W											(MX_MAIN_LABEL_W + MX_LEFT_JUST + MX_MAIN_EDIT_W)
#define MX_COMBO_L											(MX_INNER_L + MX_MAIN_LABEL_W + MX_LEFT_JUST)
#define MX_GROUP_W											(MX_GROUP_LEFT + MX_INNER_W + MX_GROUP_LEFT)
#define MX_DLG_W											(MX_GROUP_L + MX_GROUP_W + MX_GROUP_L)

#define MX_GROUP_L											MX_LEFT_JUST
#define MX_GROUP_T											MX_TOP_JUST
#define MX_GROUP_H											(MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + (MX_TOP_JUST + MX_DEF_COMBO_HEIGHT) * 3 + MX_GROUP_BOTTOM + MX_TOP_JUST)

#define MX_DLG_H											(MX_GROUP_T + MX_GROUP_H + 26)

	// == Members.
	// The layout for the Options dialog.
	LSW_WIDGET_LAYOUT CAddFoundAddressWindowLayout::m_wlDialog[] = {
		{
			MX_ADD_FOUND_ADDRESS,							// ltType
			MX_ADI_DIALOG,									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			MX_DLG_W,										// dwWidth
			MX_DLG_H,										// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_2FD68502_Add_Entry ),			// pwcText
			_LEN_2FD68502,									// sTextLen
			MX_ADI_NONE,									// dwParentId
		},
		{
			LSW_LT_GROUPBOX,								// ltType
			MX_ADI_GROUP,									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_GROUP_L,										// iLeft
			MX_GROUP_T,										// iTop
			MX_GROUP_W,										// dwWidth
			MX_GROUP_H,										// dwHeight
			MX_GROUPSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_5CA5095A_New_Value ),			// pwcText
			_LEN_5CA5095A,									// sTextLen
			MX_ADI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_ADI_DESC_LABEL,								// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_INNER_L,										// iLeft
			(MX_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),															// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_DE5F8C7A_Description_ ),		// pwcText
			_LEN_DE5F8C7A,									// sTextLen
			MX_ADI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_ADI_DESC_COMBO,								// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_COMBO_L,										// iLeft
			MX_GROUP_T + MX_GROUP_TOP,						// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_ADI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_ADI_ADDRESS_LABEL,							// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_INNER_L,										// iLeft
			(MX_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),					// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_77CAA9B0_Address_ ),			// pwcText
			_LEN_77CAA9B0,									// sTextLen
			MX_ADI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_ADI_ADDRESS_COMBO,							// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_COMBO_L,										// iLeft
			(MX_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST),																			// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_ADI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_ADI_TYPE_LABEL,								// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_INNER_L,										// iLeft
			(MX_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) * 2 + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),				// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_97F15F00_Type_ ),				// pwcText
			_LEN_97F15F00,									// sTextLen
			MX_ADI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_ADI_TYPE_COMBO,								// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_COMBO_L,										// iLeft
			(MX_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) * 2,																		// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE_LIST,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_ADI_DIALOG,									// dwParentId
		},

		// Code page.
		{
			LSW_LT_LABEL,									// ltType
			MX_ADI_CODEPAGE_LABEL,							// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_INNER_L,										// iLeft
			(MX_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) * 3 + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),				// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_B7C996CD_Code_Page_ ),		// pwcText
			_LEN_B7C996CD,									// sTextLen
			MX_ADI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_ADI_CODEPAGE_COMBO,							// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_COMBO_L,										// iLeft
			(MX_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) * 3,																		// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE_LIST,								// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_ADI_DIALOG,									// dwParentId
		},


		// OK/Cancel.
		{
			LSW_LT_BUTTON,									// ltType
			MX_ADI_OK,										// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			MX_LEFT_JUST,									// iLeft
			MX_DLG_H - MX_DEF_BUTTON_HEIGHT - MX_TOP_JUST,	// iTop
			MX_DEF_BUTTON_WIDTH,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,							// dwHeight
			MX_DEFBUTTONSTYLE,								// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),				// pwcText
			_LEN_D736D92D,									// sTextLen
			MX_ADI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			MX_ADI_CANCEL,									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_DLG_W - MX_DEF_BUTTON_WIDTH * 1 - MX_LEFT_JUST,																							// iLeft
			MX_DLG_H - MX_DEF_BUTTON_HEIGHT - MX_TOP_JUST,	// iTop
			MX_DEF_BUTTON_WIDTH,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,							// dwHeight
			MX_BUTTONSTYLE,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),			// pwcText
			_LEN_51BAC044,									// sTextLen
			MX_ADI_DIALOG,									// dwParentId
		},
	};

	// == Functions.
	// Creates the Add Found Address dialog.
	BOOL CAddFoundAddressWindowLayout::CreateAddEntryDialog( CWidget * _pwParent, CMemHack * _pmhMemHack ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlDialog, std::size( m_wlDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());

		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], std::size( m_wlDialog ), _pwParent, reinterpret_cast<uint64_t>(_pmhMemHack) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

}	// namespace mx

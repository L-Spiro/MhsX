#include "MXMoveAddressesWindowLayout.h"
#include "../MemHack/MXWindowMemHack.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>


namespace mx {

#define MX_MAIN_LABEL_W										(50 + 25)
#define MX_MAIN_EDIT_W										98
#define MX_INNER_L											(MX_GROUP_L + MX_GROUP_LEFT)
#define MX_INNER_W											(MX_MAIN_LABEL_W + MX_LEFT_JUST + MX_MAIN_EDIT_W)
#define MX_COMBO_L											(MX_INNER_L + MX_MAIN_LABEL_W + MX_LEFT_JUST)
#define MX_GROUP_W											(MX_GROUP_LEFT + MX_INNER_W + MX_GROUP_LEFT)
#define MX_DLG_W											(MX_GROUP_L + MX_GROUP_W + MX_GROUP_L)

#define MX_GROUP_L											MX_LEFT_JUST
#define MX_GROUP_T											MX_TOP_JUST
#define MX_GROUP_H											(MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + (MX_TOP_JUST + MX_DEF_COMBO_HEIGHT) * 1 + MX_GROUP_BOTTOM + MX_TOP_JUST)

#define MX_BY_GROUP_T										(MX_GROUP_T + MX_GROUP_H)
#define MX_BY_GROUP_H										(MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_GROUP_BOTTOM + MX_TOP_JUST)

#define MX_DLG_H											(MX_BY_GROUP_T + MX_BY_GROUP_H + 26)

	// == Members.
	// The layout for the Options dialog.
	LSW_WIDGET_LAYOUT CMoveAddressesWindowLayout::m_wlDialog[] = {
		{
			MX_MOVE_ADDRESSES,								// ltType
			MX_MAI_DIALOG,									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			MX_DLG_W,										// dwWidth
			MX_DLG_H,										// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_0616F872_Move_Selected_Addresses ),																						// pwcText
			_LEN_0616F872,									// sTextLen
			MX_MAI_NONE,									// dwParentId
		},

		// From -> To
		{
			LSW_LT_GROUPBOX,								// ltType
			MX_MAI_GROUP,									// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_GROUP_L,										// iLeft
			MX_GROUP_T,										// iTop
			MX_GROUP_W,										// dwWidth
			MX_GROUP_H,										// dwHeight
			MX_GROUPSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_584FA898_From____To ),		// pwcText
			_LEN_584FA898,									// sTextLen
			MX_MAI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_MAI_ADDR_START_LABEL,						// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_INNER_L,										// iLeft
			(MX_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),															// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_69AB474F_Where_the_Address_Was_ ),																						// pwcText
			_LEN_69AB474F,									// sTextLen
			MX_MAI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_MAI_ADDR_START_COMBO,						// wId
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
			MX_MAI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_MAI_ADDR_END_LABEL,							// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_INNER_L,										// iLeft
			(MX_GROUP_T + MX_GROUP_TOP) + (MX_DEF_COMBO_HEIGHT + MX_TOP_JUST) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),					// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_C3A3C612_Where_the_Address_Is_ ),																							// pwcText
			_LEN_C3A3C612,									// sTextLen
			MX_MAI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_MAI_ADDR_END_COMBO,							// wId
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
			MX_MAI_DIALOG,									// dwParentId
		},

		// By.
		{
			LSW_LT_GROUPBOX,								// ltType
			MX_MAI_MOVE_BY_GROUP,							// wId
			WC_BUTTONW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_GROUP_L,										// iLeft
			MX_BY_GROUP_T,									// iTop
			MX_GROUP_W,										// dwWidth
			MX_BY_GROUP_H,									// dwHeight
			MX_GROUPSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_AA4FF6E0_By ),				// pwcText
			_LEN_AA4FF6E0,									// sTextLen
			MX_MAI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_LABEL,									// ltType
			MX_MAI_ADDR_END_LABEL,							// wId
			WC_STATICW,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_INNER_L,										// iLeft
			(MX_BY_GROUP_T + MX_GROUP_TOP) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),														// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_BF31E825_Move_Address_es__By_ ),																							// pwcText
			_LEN_BF31E825,									// sTextLen
			MX_MAI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_MAI_MOVE_BY_COMBO,							// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_COMBO_L,										// iLeft
			(MX_BY_GROUP_T + MX_GROUP_TOP),					// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_MAI_DIALOG,									// dwParentId
		},

		{
			LSW_LT_BUTTON,									// ltType
			MX_MAI_OK,										// wId
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
			MX_MAI_DIALOG,									// dwParentId
		},
		{
			LSW_LT_BUTTON,									// ltType
			MX_MAI_CANCEL,									// wId
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
			MX_MAI_DIALOG,									// dwParentId
		},
	};

	// == Functions.
	// Creates the Move Addresses dialog.
	BOOL CMoveAddressesWindowLayout::CreateMoveAddressDialog( CWidget * _pwParent, CWindowMemHack * _pmhMemHack ) {
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

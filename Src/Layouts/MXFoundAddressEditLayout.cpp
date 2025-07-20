#include "MXFoundAddressEditLayout.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>

namespace mx {

#define MX_MAIN_LABEL_W										60
#define MX_MAIN_EDIT_W										126
#define MX_SECTION1_W										(MX_MAIN_LABEL_W + MX_LEFT_JUST + MX_MAIN_EDIT_W)
#define MX_INNER_W											(MX_SECTION1_W + MX_LEFT_JUST + MX_SECTION1_W)
#define MX_INNER_GROUP_W									(MX_GROUP_LEFT + MX_INNER_W + MX_GROUP_LEFT)
#define MX_EDIT_W											(MX_GROUP_L + MX_INNER_GROUP_W + MX_GROUP_L)

#define MX_GROUP_L											MX_LEFT_JUST
#define MX_SECTION1_L										(MX_GROUP_L + MX_GROUP_LEFT)
#define MX_SECTION2_L										(MX_SECTION1_L + MX_MAIN_LABEL_W + MX_LEFT_JUST)
#define MX_GEN_GROUP_T										MX_TOP_JUST
#define MX_GEN_GROUP_H										(MX_GROUP_TOP + MX_DEF_COMBO_HEIGHT + MX_GROUP_BOTTOM)
#define MX_EDIT_H											550

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
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
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
			MX_EDIT_H,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK,																					// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_DIALOG,									// dwParentId

			//MX_PARENT_VCLEFT,								// pcLeftSizeExp
			//MX_PARENT_VCRIGHT,								// pcRightSizeExp
			//MX_PARENT_VCTOP,								// pcTopSizeExp
			//MX_PARENT_VCBOTTOM,								// pcBottomSizeExp
			//nullptr, 0,										// pcWidthSizeExp
			//nullptr, 0,										// pcHeightSizeExp
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
			(MX_GEN_GROUP_T + MX_GROUP_TOP + MX_TOP_JUST) + ((MX_DEF_COMBO_HEIGHT - MX_DEF_STATIC_HEIGHT) >> 1),										// iTop
			MX_MAIN_LABEL_W,								// dwWidth
			MX_DEF_STATIC_HEIGHT,							// dwHeight
			MX_STATICSTYLE,									// dwStyle
			0,												// dwStyleEx
			MX_MAKE_WCHAR( _T_EB78CFF1_Description ),		// pwcText
			_LEN_EB78CFF1,									// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,								// ltType
			MX_FAEI_GENERAL_DESC_COMBO,						// wId
			WC_COMBOBOXW,									// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			MX_SECTION2_L,									// iLeft
			MX_GEN_GROUP_T + MX_GROUP_TOP + MX_TOP_JUST,	// iTop
			MX_MAIN_EDIT_W,									// dwWidth
			MX_DEF_COMBO_HEIGHT,							// dwHeight
			MX_COMBOSTYLE,									// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_FAEI_GENERAL_PAGE,							// dwParentId
		},
	};

	// == Functions.
	// Creates the Edit dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	BOOL CFoundAddressEditLayout::CreateEditDialog( CWidget * _pwParent, CMemHack * _poEdit, int32_t _i32Page ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlMainDialog, MX_ELEMENTS( m_wlMainDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());

		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], MX_ELEMENTS( m_wlMainDialog ), _pwParent, reinterpret_cast<uint64_t>(_poEdit) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

	// Creates the general edit page.
	CWidget * CFoundAddressEditLayout::CreateGeneralPage( CWidget * _pwParent, CMemHack * _poEdit ) {
		return CreatePage( _pwParent, m_wlOptionsGeneral, MX_ELEMENTS( m_wlOptionsGeneral ), _poEdit );
	}

	// Default window-creation.
	CWidget * CFoundAddressEditLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, CMemHack * _poEdit ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( _pwlLayout, _sTotal,
			vLayouts,
			sStringsW,
			sStrings );
		
		CWidget * pwWidget = lsw::CBase::LayoutManager()->CreateDialogX( &vLayouts[0], _sTotal, _pwParent, reinterpret_cast<uint64_t>(_poEdit) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		//if ( pwWidget ) {
		//	
		//	// Success.  Do stuff.
		//}

		return pwWidget;
	}

}	// namespace mx

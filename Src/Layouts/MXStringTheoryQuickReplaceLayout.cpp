#include "MXStringTheoryQuickReplaceLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "MXLayoutManager.h"
#include <Base/LSWBase.h>


namespace mx {

#define MX_W											220
#define MX_H											(175 + MX_DEF_STATIC_HEIGHT + MX_DEF_COMBO_HEIGHT + 2)

	// == Members.
	// The layout for the New Data-Type Search dialog.
	LSW_WIDGET_LAYOUT CStringTheoryQuickReplaceLayout::m_wlQuickReplaceDialog[] = {
		{
			MX_NEW_DATA_TYPE_SEARCH,				// ltType
			MX_QRI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_W,									// dwWidth
			MX_H,									// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_BFF9F43A_Quick_String_Replace ),																							// pwcText
			_LEN_BFF9F43A,							// sTextLen
			MX_QRI_NONE,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_QRI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_H - MX_TOP_JUST - MX_DEF_BUTTON_HEIGHT,											// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,		// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),													// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_QRI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_QRI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_W - MX_DEF_BUTTON_WIDTH - MX_LEFT_JUST,											// iLeft
			MX_H - MX_TOP_JUST - MX_DEF_BUTTON_HEIGHT,											// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,		// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),												// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_QRI_DIALOG,							// dwParentId
		},
	};

	// == Functions.
	// Creates the New Data-Type Search dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	DWORD CStringTheoryQuickReplaceLayout::CreateQuickReplaceDialog( CWidget * _pwParent, CMemHack * _pmhMemHack ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlQuickReplaceDialog, MX_ELEMENTS( m_wlQuickReplaceDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], MX_ELEMENTS( m_wlQuickReplaceDialog ), _pwParent, reinterpret_cast<uint64_t>(_pmhMemHack) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

}	// namespace mx

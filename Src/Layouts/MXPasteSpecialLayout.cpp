#include "MXPasteSpecialLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>


namespace mx {

#define MX_PS_L														7
#define MX_PS_CTRL_W												220
#define MX_PS_W														(MX_PS_L * 2 + MX_PS_CTRL_W)
#define MX_PS_LIST_T												(MX_PS_L + MX_DEF_STATIC_HEIGHT + 3)
#define MX_PS_LIST_H												130
#define MX_PS_BUTTON_T												(MX_PS_LIST_T + MX_PS_LIST_H + 8)
#define MX_PS_H														(MX_PS_BUTTON_T + MX_DEF_BUTTON_HEIGHT + MX_PS_L)

	// == Members.
	// The layout of the Paste Special dialog.
	LSW_WIDGET_LAYOUT CPasteSpecialLayout::m_wlPasteSpecialDialog[] = {
		{
			MX_PASTE_SPECIAL,						// ltType
			MX_PSI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_PS_W,								// dwWidth
			MX_PS_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			MX_MAKE_WCHAR( _T_DC533F38_Paste_Special ),	// pwcText
			_LEN_DC533F38,					// sTextLen
			MX_PSI_NONE,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_PSI_FORMATS_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_PS_L,								// iLeft
			MX_PS_L,								// iTop
			MX_PS_CTRL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0F84FCEC__Formats_on_the_clipboard_ ),	// pwcText
			_LEN_0F84FCEC,		// sTextLen
			MX_PSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LISTBOX,							// ltType
			MX_PSI_FORMATS_LIST,					// wId
			WC_LISTBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_PS_L,								// iLeft
			MX_PS_LIST_T,							// iTop
			MX_PS_CTRL_W,							// dwWidth
			MX_PS_LIST_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,	// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_PSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_PSI_PASTE,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_PS_W - MX_PS_L - MX_DEF_BUTTON_WIDTH * 2 - 4,	// iLeft
			MX_PS_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_FB82E709__Paste ),			// pwcText
			_LEN_FB82E709,							// sTextLen
			MX_PSI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_PSI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_PS_W - MX_PS_L - MX_DEF_BUTTON_WIDTH,	// iLeft
			MX_PS_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),			// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_PSI_DIALOG,							// dwParentId
		},
	};

	// == Functions.
	/**
	 * Shows the Paste Special dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	 *
	 * \param _pwParent The parent window.
	 * \param _pspParms Receives the bytes of the chosen format.
	 * \return Returns TRUE if the user pressed Paste and bytes were read.
	 **/
	DWORD CPasteSpecialLayout::CreatePasteSpecialDialog( CWidget * _pwParent, MX_PASTE_SPECIAL_PARMS &_pspParms ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlPasteSpecialDialog, std::size( m_wlPasteSpecialDialog ),
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], std::size( m_wlPasteSpecialDialog ), _pwParent, reinterpret_cast<uint64_t>(&_pspParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		return ipProc > 0 ? TRUE : FALSE;
	}

}	// namespace mx

#include "MXSelectRangeLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>


namespace mx {

#define MX_SR_L														7
#define MX_SR_CTRL_W												176
#define MX_SR_W														(MX_SR_L * 2 + MX_SR_CTRL_W)
#define MX_SR_START_LABEL_T											7
#define MX_SR_START_EDIT_T											(MX_SR_START_LABEL_T + MX_DEF_STATIC_HEIGHT + MX_TOP_JUST)
#define MX_SR_RADIO_T												(MX_SR_START_EDIT_T + MX_DEF_EDIT_HEIGHT + 6)
#define MX_SR_SIZE_RADIO_W											50
#define MX_SR_END_RADIO_L											(MX_SR_L + MX_SR_SIZE_RADIO_W + 6)
#define MX_SR_END_RADIO_W											(MX_SR_CTRL_W - MX_SR_SIZE_RADIO_W - 6)
#define MX_SR_VALUE_EDIT_T											(MX_SR_RADIO_T + MX_DEF_RADIO_HEIGHT + MX_TOP_JUST)
#define MX_SR_BUTTON_T												(MX_SR_VALUE_EDIT_T + MX_DEF_EDIT_HEIGHT + 9)
#define MX_SR_BUTTON_GAP											4
#define MX_SR_H														(MX_SR_BUTTON_T + MX_DEF_BUTTON_HEIGHT + MX_SR_L)

	// == Members.
	// The layout of the Select Range dialog.
	LSW_WIDGET_LAYOUT CSelectRangeLayout::m_wlSelectRangeDialog[] = {
		{
			MX_SELECT_RANGE,						// ltType
			MX_SRI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_SR_W,								// dwWidth
			MX_SR_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			MX_MAKE_WCHAR( _T_191780FA_Select_Range ),	// pwcText
			_LEN_191780FA,							// sTextLen
			MX_SRI_NONE,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_SRI_START_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SR_L,								// iLeft
			MX_SR_START_LABEL_T,					// iTop
			MX_SR_CTRL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_26C90B12__Start_Address_ ),	// pwcText
			_LEN_26C90B12,							// sTextLen
			MX_SRI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_SRI_START_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SR_L,								// iLeft
			MX_SR_START_EDIT_T,						// iTop
			MX_SR_CTRL_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SRI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_SRI_SIZE_RADIO,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_SR_L,								// iLeft
			MX_SR_RADIO_T,							// iTop
			MX_SR_SIZE_RADIO_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_2B77C476_Si_ze ),		// pwcText
			_LEN_2B77C476,							// sTextLen
			MX_SRI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_SRI_END_RADIO,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SR_END_RADIO_L,						// iLeft
			MX_SR_RADIO_T,							// iTop
			MX_SR_END_RADIO_W,						// dwWidth
			MX_DEF_RADIO_HEIGHT,					// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_364EC236__End_Address__Inclusive_ ),	// pwcText
			_LEN_364EC236,							// sTextLen
			MX_SRI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_SRI_VALUE_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SR_L,								// iLeft
			MX_SR_VALUE_EDIT_T,						// iTop
			MX_SR_CTRL_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | WS_GROUP,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_SRI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_SRI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_SR_W - MX_SR_L - MX_DEF_BUTTON_WIDTH * 2 - MX_SR_BUTTON_GAP,	// iLeft
			MX_SR_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),		// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_SRI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_SRI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_SR_W - MX_SR_L - MX_DEF_BUTTON_WIDTH,	// iLeft
			MX_SR_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),	// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_SRI_DIALOG,							// dwParentId
		},
	};

	// == Functions.
	/**
	 * Shows the Select Range dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	 *
	 * \param _pwParent The parent window.
	 * \param _srpParms The default values on input and the values entered on output.
	 * \return Returns TRUE if the user pressed OK.
	 **/
	DWORD CSelectRangeLayout::CreateSelectRangeDialog( CWidget * _pwParent, MX_SELECT_RANGE_PARMS &_srpParms ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlSelectRangeDialog, std::size( m_wlSelectRangeDialog ),
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], std::size( m_wlSelectRangeDialog ), _pwParent, reinterpret_cast<uint64_t>(&_srpParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		return ipProc > 0 ? TRUE : FALSE;
	}

}	// namespace mx

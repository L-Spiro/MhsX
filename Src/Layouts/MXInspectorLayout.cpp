#include "MXInspectorLayout.h"
#include "../HexEditor/MXInspectorCustomizeDialog.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>
#include <Layout/LSWLayoutManager.h>


namespace mx {

#define MX_ICU_L													7
#define MX_ICU_W													260
#define MX_ICU_BTN_W												60
#define MX_ICU_BTN_L												(MX_ICU_W - MX_ICU_L - MX_ICU_BTN_W)
#define MX_ICU_LIST_T												(MX_ICU_L + MX_DEF_STATIC_HEIGHT + 3)
#define MX_ICU_LIST_W												(MX_ICU_BTN_L - 6 - MX_ICU_L)
#define MX_ICU_LIST_H												150
#define MX_ICU_LABEL_W												60
#define MX_ICU_CTRL_L												(MX_ICU_L + MX_ICU_LABEL_W + 2)
#define MX_ICU_ROW2_T												(MX_ICU_LIST_T + MX_ICU_LIST_H + 8)
#define MX_ICU_ROW3_T												(MX_ICU_ROW2_T + MX_DEF_COMBO_HEIGHT + 4)
#define MX_ICU_BUTTON_T												(MX_ICU_ROW3_T + MX_DEF_COMBO_HEIGHT + 10)
#define MX_ICU_H													(MX_ICU_BUTTON_T + MX_DEF_BUTTON_HEIGHT + MX_ICU_L)

	// == Members.
	// The layout of the Inspector.
	LSW_WIDGET_LAYOUT CInspectorLayout::m_wlInspector[] = {
		{
			MX_INSPECTOR,							// ltType
			MX_II_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			200,									// iLeft
			150,									// iTop
			300,									// dwWidth
			420,									// dwHeight
			WS_VISIBLE,								// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_F02CD328_Inspector ),		// pwcText
			_LEN_F02CD328,						// sTextLen
			0,										// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_LISTVIEW,						// ltType
			MX_II_LIST,								// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			0,										// dwWidth
			0,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,	// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_II_MAINWINDOW,						// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
	};

	// The layout of the Customize dialog.
	LSW_WIDGET_LAYOUT CInspectorLayout::m_wlCustomize[] = {
		{
			MX_INSPECTOR_CUSTOMIZE,					// ltType
			MX_ICI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_ICU_W,								// dwWidth
			MX_ICU_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			MX_MAKE_WCHAR( _T_37CC022B_Customize_Inspector ),	// pwcText
			_LEN_37CC022B,				// sTextLen
			MX_ICI_NONE,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_ICI_TYPES_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_ICU_L,								// iLeft
			MX_ICU_L,								// iTop
			MX_ICU_W - MX_ICU_L * 2,				// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_3017F406__Data_types__checked_types_are_shown__ ),	// pwcText
			_LEN_3017F406,	// sTextLen
			MX_ICI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LISTVIEW,						// ltType
			MX_ICI_TYPES_LIST,						// wId
			WC_LISTVIEWW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_ICU_L,								// iLeft
			MX_ICU_LIST_T,							// iTop
			MX_ICU_LIST_W,							// dwWidth
			MX_ICU_LIST_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | WS_BORDER | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_ICI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_ICI_MOVE_UP,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_ICU_BTN_L,							// iLeft
			MX_ICU_LIST_T,							// iTop
			MX_ICU_BTN_W,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_2DD66FF9_Move__Up ),			// pwcText
			_LEN_2DD66FF9,						// sTextLen
			MX_ICI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_ICI_MOVE_DOWN,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_ICU_BTN_L,							// iLeft
			MX_ICU_LIST_T + MX_DEF_BUTTON_HEIGHT + 4,	// iTop
			MX_ICU_BTN_W,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_6C016ECD_Move__Down ),		// pwcText
			_LEN_6C016ECD,						// sTextLen
			MX_ICI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_ICI_RESET,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_ICU_BTN_L,							// iLeft
			MX_ICU_LIST_T + MX_ICU_LIST_H - MX_DEF_BUTTON_HEIGHT,	// iTop
			MX_ICU_BTN_W,							// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_374920DC__Reset ),			// pwcText
			_LEN_374920DC,							// sTextLen
			MX_ICI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_ICI_DATE_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_ICU_L,								// iLeft
			MX_ICU_ROW2_T + 2,						// iTop
			MX_ICU_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_9BE69576_D_ate_format_ ),	// pwcText
			_LEN_9BE69576,					// sTextLen
			MX_ICI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_ICI_DATE_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_ICU_CTRL_L,							// iLeft
			MX_ICU_ROW2_T,							// iTop
			MX_ICU_W - MX_ICU_L - MX_ICU_CTRL_L,	// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_ICI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_ICI_TIME_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_ICU_L,								// iLeft
			MX_ICU_ROW3_T + 2,						// iTop
			MX_ICU_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_5C40AE6F__Time_format_ ),	// pwcText
			_LEN_5C40AE6F,					// sTextLen
			MX_ICI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_ICI_TIME_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_ICU_CTRL_L,							// iLeft
			MX_ICU_ROW3_T,							// iTop
			MX_ICU_W - MX_ICU_L - MX_ICU_CTRL_L,	// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_ICI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_ICI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_ICU_W - MX_ICU_L - MX_DEF_BUTTON_WIDTH * 2 - 4,	// iLeft
			MX_ICU_BUTTON_T,						// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),				// pwcText
			_LEN_D736D92D,								// sTextLen
			MX_ICI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_ICI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_ICU_W - MX_ICU_L - MX_DEF_BUTTON_WIDTH,	// iLeft
			MX_ICU_BUTTON_T,						// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),			// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_ICI_DIALOG,							// dwParentId
		},
	};

	// == Functions.
	/**
	 * Creates the Inspector.
	 *
	 * \param _pwParent The parent (the dock target it is created in).
	 * \return Returns the Inspector, or nullptr on failure.
	 **/
	CWidget * CInspectorLayout::CreateInspectorWindow( CWidget * _pwParent ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlInspector, std::size( m_wlInspector ),
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwThis = plmLayout->CreateWindowX( &vLayouts[0], std::size( m_wlInspector ), nullptr, 0, _pwParent );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		return pwThis;
	}

	/**
	 * Shows the Customize dialog of the Inspector.
	 *
	 * \param _pwParent The parent window.
	 * \param _cpParms The current settings on input and the chosen settings on output.
	 * \return Returns true if OK was pressed.
	 **/
	bool CInspectorLayout::CreateCustomizeDialog( CWidget * _pwParent, MX_CUSTOMIZE_PARMS &_cpParms ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlCustomize, std::size( m_wlCustomize ),
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], std::size( m_wlCustomize ), _pwParent, reinterpret_cast<uint64_t>(&_cpParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		return ipProc > 0;
	}

#undef MX_ICU_H
#undef MX_ICU_BUTTON_T
#undef MX_ICU_ROW3_T
#undef MX_ICU_ROW2_T
#undef MX_ICU_CTRL_L
#undef MX_ICU_LABEL_W
#undef MX_ICU_LIST_H
#undef MX_ICU_LIST_W
#undef MX_ICU_LIST_T
#undef MX_ICU_BTN_L
#undef MX_ICU_BTN_W
#undef MX_ICU_W
#undef MX_ICU_L

}	// namespace mx

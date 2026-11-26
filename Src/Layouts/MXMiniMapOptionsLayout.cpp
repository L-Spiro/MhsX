#include "MXMiniMapOptionsLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>


namespace mx {

#define MX_MMO_L													7
#define MX_MMO_W													250
#define MX_MMO_LABEL_W												66
#define MX_MMO_LABEL_DROP											2
#define MX_MMO_CHECK_DROP											1
#define MX_MMO_CTRL_L												(MX_MMO_L + MX_MMO_LABEL_W + 2)
#define MX_MMO_POS_COMBO_W											70
#define MX_MMO_COLOR_COMBO_W										(MX_MMO_W - MX_MMO_L - MX_DEF_BUTTON_WIDTH - MX_MMO_BUTTON_GAP - MX_MMO_CTRL_L)
#define MX_MMO_NUM_EDIT_W											40
#define MX_MMO_SNAP_CHECK_W											56
#define MX_MMO_SNAP_CHECK_L											(MX_MMO_W - MX_MMO_L - MX_MMO_NUM_EDIT_W - 3 - MX_MMO_SNAP_CHECK_W)
#define MX_MMO_TRACK_H												15
#define MX_MMO_VALUE_W												28
#define MX_MMO_VALUE_L												(MX_MMO_W - MX_MMO_L - MX_MMO_VALUE_W)
#define MX_MMO_TRACK_W												(MX_MMO_VALUE_L - 2 - MX_MMO_CTRL_L)
#define MX_MMO_SLIDER_LABEL_DROP									4
#define MX_MMO_BUTTON_GAP											4
#define MX_MMO_ROW1_T												7
#define MX_MMO_ROW2_T												(MX_MMO_ROW1_T + MX_DEF_COMBO_HEIGHT + 5)
#define MX_MMO_ROW3_T												(MX_MMO_ROW2_T + MX_DEF_COMBO_HEIGHT + 5)
#define MX_MMO_ROW4_T												(MX_MMO_ROW3_T + MX_DEF_CHECK_HEIGHT + 6)
#define MX_MMO_ROW5_T												(MX_MMO_ROW4_T + MX_DEF_EDIT_HEIGHT + 7)
#define MX_MMO_ROW6_T												(MX_MMO_ROW5_T + MX_MMO_TRACK_H + 3)
#define MX_MMO_ROW7_T												(MX_MMO_ROW6_T + MX_MMO_TRACK_H + 3)
#define MX_MMO_BUTTON_T												(MX_MMO_ROW7_T + MX_MMO_TRACK_H + 8)
#define MX_MMO_H													(MX_MMO_BUTTON_T + MX_DEF_BUTTON_HEIGHT + MX_MMO_L)

	// == Members.
	// The layout of the Mini Map Options dialog.
	LSW_WIDGET_LAYOUT CMiniMapOptionsLayout::m_wlMiniMapOptionsDialog[] = {
		{
			MX_MINI_MAP_OPTIONS,					// ltType
			MX_MMOI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_MMO_W,								// dwWidth
			MX_MMO_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			MX_MAKE_WCHAR( _T_811AFADD_Mini_Map_Options ),	// pwcText
			_LEN_811AFADD,							// sTextLen
			MX_MMOI_NONE,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_MMOI_POSITION_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_L,								// iLeft
			MX_MMO_ROW1_T + MX_MMO_LABEL_DROP,		// iTop
			MX_MMO_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_9909041A_Mini_Map__Position_ ),	// pwcText
			_LEN_9909041A,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_MMOI_POSITION_COMBO,					// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_CTRL_L,							// iLeft
			MX_MMO_ROW1_T,							// iTop
			MX_MMO_POS_COMBO_W,						// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_MMOI_HIDE,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_W - MX_MMO_L - MX_DEF_BUTTON_WIDTH * 2 - MX_MMO_BUTTON_GAP,	// iLeft
			MX_MMO_ROW1_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_AD4C96B0__Hide ),		// pwcText
			_LEN_AD4C96B0,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_MMOI_RESET,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_W - MX_MMO_L - MX_DEF_BUTTON_WIDTH,	// iLeft
			MX_MMO_ROW1_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_374920DC__Reset ),	// pwcText
			_LEN_374920DC,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_MMOI_COLORIZE_CHECK,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_L,								// iLeft
			MX_MMO_ROW2_T + MX_MMO_CHECK_DROP,		// iTop
			MX_MMO_LABEL_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_B875ADDA__Colorize_ ),	// pwcText
			_LEN_B875ADDA,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_MMOI_COLORIZE_COMBO,					// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_CTRL_L,							// iLeft
			MX_MMO_ROW2_T,							// iTop
			MX_MMO_COLOR_COMBO_W,					// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_MMOI_HIGHLIGHT_STRUCTS,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_L,								// iLeft
			MX_MMO_ROW3_T,							// iTop
			MX_MMO_W - MX_MMO_L * 2,				// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_DFF1784D_Highlight__Structs ),	// pwcText
			_LEN_DFF1784D,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_MMOI_BPR_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_L,								// iLeft
			MX_MMO_ROW4_T + MX_MMO_LABEL_DROP,		// iTop
			MX_MMO_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_18237A8D_Bytes_Per__Line_ ),	// pwcText
			_LEN_18237A8D,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_MMOI_BPR_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_CTRL_L,							// iLeft
			MX_MMO_ROW4_T,							// iTop
			MX_MMO_NUM_EDIT_W,						// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | ES_NUMBER,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_MMOI_AUTO_SNAP_CHECK,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_SNAP_CHECK_L,					// iLeft
			MX_MMO_ROW4_T + MX_MMO_CHECK_DROP,		// iTop
			MX_MMO_SNAP_CHECK_W,					// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_98C68C54__Auto_Snap_ ),	// pwcText
			_LEN_98C68C54,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_MMOI_AUTO_SNAP_EDIT,					// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_W - MX_MMO_L - MX_MMO_NUM_EDIT_W,	// iLeft
			MX_MMO_ROW4_T,							// iTop
			MX_MMO_NUM_EDIT_W,						// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE | ES_NUMBER,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_MMOI_BRIGHTNESS_LABEL,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_L,								// iLeft
			MX_MMO_ROW5_T + MX_MMO_SLIDER_LABEL_DROP,	// iTop
			MX_MMO_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_4C167567__Brightness_ ),	// pwcText
			_LEN_4C167567,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,						// ltType
			MX_MMOI_BRIGHTNESS_TRACK,				// wId
			TRACKBAR_CLASSW,						// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_CTRL_L,							// iLeft
			MX_MMO_ROW5_T,							// iTop
			MX_MMO_TRACK_W,							// dwWidth
			MX_MMO_TRACK_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TBS_HORZ | TBS_BOTTOM | TBS_AUTOTICKS,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_MMOI_BRIGHTNESS_VALUE,				// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_VALUE_L,							// iLeft
			MX_MMO_ROW5_T + MX_MMO_SLIDER_LABEL_DROP,	// iTop
			MX_MMO_W - MX_MMO_L - MX_MMO_VALUE_L,	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | SS_RIGHT,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_MMOI_CONTRAST_LABEL,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_L,								// iLeft
			MX_MMO_ROW6_T + MX_MMO_SLIDER_LABEL_DROP,	// iTop
			MX_MMO_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_FE7D7504_C_ontrast_ ),	// pwcText
			_LEN_FE7D7504,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,						// ltType
			MX_MMOI_CONTRAST_TRACK,					// wId
			TRACKBAR_CLASSW,						// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_CTRL_L,							// iLeft
			MX_MMO_ROW6_T,							// iTop
			MX_MMO_TRACK_W,							// dwWidth
			MX_MMO_TRACK_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TBS_HORZ | TBS_BOTTOM | TBS_AUTOTICKS,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_MMOI_CONTRAST_VALUE,					// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_VALUE_L,							// iLeft
			MX_MMO_ROW6_T + MX_MMO_SLIDER_LABEL_DROP,	// iTop
			MX_MMO_W - MX_MMO_L - MX_MMO_VALUE_L,	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | SS_RIGHT,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_MMOI_ZOOM_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_L,								// iLeft
			MX_MMO_ROW7_T + MX_MMO_SLIDER_LABEL_DROP,	// iTop
			MX_MMO_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_240537F2__Zoom_ ),	// pwcText
			_LEN_240537F2,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_TRACKBAR,						// ltType
			MX_MMOI_ZOOM_TRACK,						// wId
			TRACKBAR_CLASSW,						// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_CTRL_L,							// iLeft
			MX_MMO_ROW7_T,							// iTop
			MX_MMO_TRACK_W,							// dwWidth
			MX_MMO_TRACK_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TBS_HORZ | TBS_BOTTOM | TBS_AUTOTICKS,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_MMOI_ZOOM_VALUE,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_VALUE_L,							// iLeft
			MX_MMO_ROW7_T + MX_MMO_SLIDER_LABEL_DROP,	// iTop
			MX_MMO_W - MX_MMO_L - MX_MMO_VALUE_L,	// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | SS_RIGHT,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_MMOI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_MMO_W - MX_MMO_L - MX_DEF_BUTTON_WIDTH * 2 - MX_MMO_BUTTON_GAP,	// iLeft
			MX_MMO_BUTTON_T,						// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),		// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_MMOI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_MMO_W - MX_MMO_L - MX_DEF_BUTTON_WIDTH,	// iLeft
			MX_MMO_BUTTON_T,						// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),	// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_MMOI_DIALOG,							// dwParentId
		},
	};

	// == Functions.
	/**
	 * Shows the Mini Map Options dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	 *
	 * \param _pwParent The parent window.
	 * \param _mmopParms The current options on input and the chosen options on output.
	 * \return Returns TRUE if the user pressed OK or Hide.
	 **/
	DWORD CMiniMapOptionsLayout::CreateMiniMapOptionsDialog( CWidget * _pwParent, MX_MINI_MAP_OPTIONS_PARMS &_mmopParms ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlMiniMapOptionsDialog, std::size( m_wlMiniMapOptionsDialog ),
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], std::size( m_wlMiniMapOptionsDialog ), _pwParent, reinterpret_cast<uint64_t>(&_mmopParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		return ipProc > 0 ? TRUE : FALSE;
	}

}	// namespace mx

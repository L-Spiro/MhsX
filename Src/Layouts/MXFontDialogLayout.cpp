#include "MXFontDialogLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>


namespace mx {

#define MX_FD_PAD													7
#define MX_FD_COL_GAP												6
#define MX_FD_FONT_L												MX_FD_PAD
#define MX_FD_FONT_W												132
#define MX_FD_STYLE_L												(MX_FD_FONT_L + MX_FD_FONT_W + MX_FD_COL_GAP)
#define MX_FD_STYLE_W												92
#define MX_FD_SIZE_L												(MX_FD_STYLE_L + MX_FD_STYLE_W + MX_FD_COL_GAP)
#define MX_FD_SIZE_W												40
#define MX_FD_W														(MX_FD_SIZE_L + MX_FD_SIZE_W + MX_FD_PAD)
#define MX_FD_LABEL_T												MX_FD_PAD
#define MX_FD_EDIT_T												(MX_FD_LABEL_T + MX_DEF_STATIC_HEIGHT + MX_TOP_JUST)
#define MX_FD_LIST_T												(MX_FD_EDIT_T + MX_DEF_EDIT_HEIGHT + 1)
#define MX_FD_LIST_H												92
#define MX_FD_BOTTOM_T												(MX_FD_LIST_T + MX_FD_LIST_H + 6)
#define MX_FD_CHECK_W												100
#define MX_FD_EFFECTS_H												(MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT * 2 + MX_TOP_JUST + MX_GROUP_BOTTOM + 2)
#define MX_FD_WS_LABEL_T											(MX_FD_BOTTOM_T + MX_FD_EFFECTS_H + 5)
#define MX_FD_WS_COMBO_T											(MX_FD_WS_LABEL_T + MX_DEF_STATIC_HEIGHT + MX_TOP_JUST)
#define MX_FD_SAMPLE_W												(MX_FD_W - MX_FD_PAD - MX_FD_STYLE_L)
#define MX_FD_SAMPLE_H												(MX_FD_WS_COMBO_T + MX_DEF_COMBO_HEIGHT - MX_FD_BOTTOM_T)
#define MX_FD_BUTTON_GAP											4
#define MX_FD_BUTTON_T												(MX_FD_WS_COMBO_T + MX_DEF_COMBO_HEIGHT + 9)
#define MX_FD_H														(MX_FD_BUTTON_T + MX_DEF_BUTTON_HEIGHT + MX_FD_PAD)

// The Font column gets 3/5 of any change in width and the Font Style column gets the rest.
#define MX_FD_RIGHT_3_5												_T_LEN_F202C542_____SR______P__L_____P__SL________P__W_____P__SW__3_5_
#define MX_FD_LEFT_3_5												_T_LEN_5E689AC9_____SL______P__L_____P__SL________P__W_____P__SW__3_5_
// OK stays beside Cancel.
#define MX_FD_OK_RIGHT												MX_LOCK_RIGHT

	// == Members.
	// The layout of the Change Font dialog.
	LSW_WIDGET_LAYOUT CFontDialogLayout::m_wlFontDialog[] = {
		{
			MX_FONT_DIALOG,							// ltType
			MX_FDI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_FD_W,								// dwWidth
			MX_FD_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | WS_THICKFRAME | DS_3DLOOK | DS_FIXEDSYS | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,	// dwStyleEx
			MX_MAKE_WCHAR( _T_C7F347AD_Change_Hex_Editor_Font ),	// pwcText
			_LEN_C7F347AD,							// sTextLen
			MX_FDI_NONE,							// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FDI_FONT_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_FONT_L,							// iLeft
			MX_FD_LABEL_T,							// iTop
			MX_FD_FONT_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_D9415549__Font ),		// pwcText
			_LEN_D9415549,							// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_FD_RIGHT_3_5,						// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_FDI_FONT_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_FONT_L,							// iLeft
			MX_FD_EDIT_T,							// iTop
			MX_FD_FONT_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_FD_RIGHT_3_5,						// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LISTBOX,							// ltType
			MX_FDI_FONT_LIST,						// wId
			WC_LISTBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_FONT_L,							// iLeft
			MX_FD_LIST_T,							// iTop
			MX_FD_FONT_W,							// dwWidth
			MX_FD_LIST_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,	// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_FD_RIGHT_3_5,						// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FDI_STYLE_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_STYLE_L,							// iLeft
			MX_FD_LABEL_T,							// iTop
			MX_FD_STYLE_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_AD9DEA5C_Font_st_yle ),	// pwcText
			_LEN_AD9DEA5C,							// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_FD_LEFT_3_5,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_FDI_STYLE_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_STYLE_L,							// iLeft
			MX_FD_EDIT_T,							// iTop
			MX_FD_STYLE_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_FD_LEFT_3_5,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LISTBOX,							// ltType
			MX_FDI_STYLE_LIST,						// wId
			WC_LISTBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_STYLE_L,							// iLeft
			MX_FD_LIST_T,							// iTop
			MX_FD_STYLE_W,							// dwWidth
			MX_FD_LIST_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,	// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_FD_LEFT_3_5,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FDI_SIZE_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_SIZE_L,							// iLeft
			MX_FD_LABEL_T,							// iTop
			MX_FD_SIZE_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_FE1579F1__Size ),		// pwcText
			_LEN_FE1579F1,							// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_FDI_SIZE_EDIT,						// wId
			WC_EDITW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_SIZE_L,							// iLeft
			MX_FD_EDIT_T,							// iTop
			MX_FD_SIZE_W,							// dwWidth
			MX_DEF_EDIT_HEIGHT,						// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LISTBOX,							// ltType
			MX_FDI_SIZE_LIST,						// wId
			WC_LISTBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_SIZE_L,							// iLeft
			MX_FD_LIST_T,							// iTop
			MX_FD_SIZE_W,							// dwWidth
			MX_FD_LIST_H,							// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,	// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_FDI_EFFECTS_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_FONT_L,							// iLeft
			MX_FD_BOTTOM_T,							// iTop
			MX_FD_FONT_W,							// dwWidth
			MX_FD_EFFECTS_H,						// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_87B60536_Effects ),	// pwcText
			_LEN_87B60536,							// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_FD_RIGHT_3_5,						// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FDI_STRIKEOUT,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_FONT_L + MX_GROUP_LEFT,			// iLeft
			MX_FD_BOTTOM_T + MX_GROUP_TOP,			// iTop
			MX_FD_CHECK_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_99F94BF2_Stri_keout ),	// pwcText
			_LEN_99F94BF2,							// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_FDI_UNDERLINE,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_FONT_L + MX_GROUP_LEFT,			// iLeft
			MX_FD_BOTTOM_T + MX_GROUP_TOP + MX_DEF_CHECK_HEIGHT + MX_TOP_JUST,	// iTop
			MX_FD_CHECK_W,							// dwWidth
			MX_DEF_CHECK_HEIGHT,					// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_08EF830B__Underline ),	// pwcText
			_LEN_08EF830B,							// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FDI_WS_LABEL,						// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_FONT_L,							// iLeft
			MX_FD_WS_LABEL_T,						// iTop
			MX_FD_FONT_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT,					// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_CD61665D__Writing_System ),	// pwcText
			_LEN_CD61665D,							// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_FD_RIGHT_3_5,						// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_FDI_WS_COMBO,						// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_FONT_L,							// iLeft
			MX_FD_WS_COMBO_T,						// iTop
			MX_FD_FONT_W,							// dwWidth
			MX_DEF_COMBO_HEIGHT,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_FD_RIGHT_3_5,						// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_FDI_SAMPLE_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_STYLE_L,							// iLeft
			MX_FD_BOTTOM_T,							// iTop
			MX_FD_SAMPLE_W,							// dwWidth
			MX_FD_SAMPLE_H,							// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_F6A773F5_Sample ),	// pwcText
			_LEN_F6A773F5,							// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_FD_LEFT_3_5,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_FDI_SAMPLE,							// wId
			WC_STATICW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_STYLE_L + MX_GROUP_LEFT,			// iLeft
			MX_FD_BOTTOM_T + MX_GROUP_TOP,			// iTop
			MX_FD_SAMPLE_W - MX_GROUP_LEFT - MX_GROUP_RIGHT,	// dwWidth
			MX_FD_SAMPLE_H - MX_GROUP_TOP - MX_GROUP_BOTTOM,	// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | SS_NOPREFIX | SS_ENDELLIPSIS,	// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			MX_FD_LEFT_3_5,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_FDI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_FD_W - MX_FD_PAD - MX_DEF_BUTTON_WIDTH * 2 - MX_FD_BUTTON_GAP,	// iLeft
			MX_FD_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),		// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			MX_FD_OK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_FDI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FD_W - MX_FD_PAD - MX_DEF_BUTTON_WIDTH,	// iLeft
			MX_FD_BUTTON_T,							// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,	// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),	// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_FDI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
	};

	// == Functions.
	/**
	 * Shows the Change Font dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	 *
	 * \param _pwParent The parent window.
	 * \param _fdpParms The current font on input and the chosen font on output.
	 * \return Returns TRUE if the user pressed OK.
	 **/
	DWORD CFontDialogLayout::CreateFontDialog( CWidget * _pwParent, MX_FONT_DIALOG_PARMS &_fdpParms ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlFontDialog, std::size( m_wlFontDialog ),
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], std::size( m_wlFontDialog ), _pwParent, reinterpret_cast<uint64_t>(&_fdpParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		return ipProc > 0 ? TRUE : FALSE;
	}

}	// namespace mx

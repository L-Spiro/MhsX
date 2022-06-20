#include "MXConverterLayout.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>

namespace mx {

	// == Members.

#define MX_ROW_HEIGHT								(MX_TOP_JUST + MX_DEF_STATIC_HEIGHT_PXL + MX_DEF_EDIT_HEIGHT_PXL)
#define MX_GROUP_HEIGHT								(MX_GROUP_TOP_PXL + MX_GROUP_BOTTOM_PXL + (MX_ROW_HEIGHT * 17))
#define MX_GROUPS_TOP								(MX_DEF_STATIC_HEIGHT_PXL + MX_TOP_JUST * 2 + MX_DEF_EDIT_HEIGHT_PXL + MX_ROW_HEIGHT)
#define MX_LAYOUT_W									340
#define MX_LAYOUT_H									(MX_GROUP_HEIGHT + MX_GROUPS_TOP + 34 + MX_ROW_HEIGHT)

	// The layout for the Converter dockable window.
	LSW_WIDGET_LAYOUT CConverterLayout::m_wlMainWindow[] = {
		{
			MX_CONVERTER,							// ltType
			MX_CWI_MAIN_WINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			654 + 130 * 2,							// iLeft
			153,									// iTop
			MX_LAYOUT_W,							// dwWidth
			MX_LAYOUT_H,							// dwHeight
			WS_VISIBLE | WS_CLIPSIBLINGS,			// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_6BB29F4B_Converter ),	// pwcText
			_LEN_6BB29F4B,							// sTextLen
			0,										// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_REBAR,							// ltType
			MX_CWI_REBAR0,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			m_wlMainWindow[0].dwWidth,				// dwWidth
			150,									// dwHeight
			WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN |
				RBS_VARHEIGHT | RBS_BANDBORDERS,																	// dwStyle
			0,//WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW,							// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			MX_PARENT_CLEFT,						// pcLeftSizeExp
			MX_PARENT_CRIGHT,						// pcRightSizeExp
			MX_PARENT_CTOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_TOOLBAR,							// ltType
			MX_CWI_TOOLBAR0,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			0,										// dwWidth
			0,										// dwHeight
			WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | WS_VISIBLE | CCS_ADJUSTABLE | TBSTYLE_LIST,		// dwStyle
			0,																										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId
		},

		// == 1 byte.
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_INT8_LABEL,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_TOP_JUST,							// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_1C554140_int8_t ),	// pwcText
			_LEN_1C554140,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_C5DC90BF_____P__CL_2,			// pcLeftSizeExp
			_T_LEN_F23BFBC4_____P__CL______P__CW_2__2_2__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_INT8,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_DEF_STATIC_HEIGHT_PXL + MX_TOP_JUST,	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_C5DC90BF_____P__CL_2,			// pcLeftSizeExp
			_T_LEN_F23BFBC4_____P__CL______P__CW_2__2_2__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UINT8_LABEL,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LAYOUT_W / 2,						// iLeft
			MX_TOP_JUST,							// iTop
			(MX_LAYOUT_W - (MX_LAYOUT_W / 2)) - MX_LEFT_JUST,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8EABC011_uint8_t ),	// pwcText
			_LEN_8EABC011,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_91EBCEFE_____P__CL______P__CW_2__2_2__,			// pcLeftSizeExp
			_T_LEN_AE7677BD_____P__CL______P__CW_2_,				// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UINT8,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LAYOUT_W / 2,						// iLeft
			MX_DEF_STATIC_HEIGHT_PXL + MX_TOP_JUST,	// iTop
			(MX_LAYOUT_W / 2) - MX_LEFT_JUST,		// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_91EBCEFE_____P__CL______P__CW_2__2_2__,			// pcLeftSizeExp
			_T_LEN_AE7677BD_____P__CL______P__CW_2_,				// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_CHAR_LABEL,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_TOP_JUST + MX_ROW_HEIGHT,			// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8CFE579F_char ),		// pwcText
			_LEN_8CFE579F,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_C5DC90BF_____P__CL_2,			// pcLeftSizeExp
			_T_LEN_F23BFBC4_____P__CL______P__CW_2__2_2__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_CHAR,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_DEF_STATIC_HEIGHT_PXL + MX_TOP_JUST + MX_ROW_HEIGHT,	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_C5DC90BF_____P__CL_2,			// pcLeftSizeExp
			_T_LEN_F23BFBC4_____P__CL______P__CW_2__2_2__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UTF8_LABEL,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LAYOUT_W / 2,						// iLeft
			MX_TOP_JUST + MX_ROW_HEIGHT,			// iTop
			(MX_LAYOUT_W - (MX_LAYOUT_W / 2)) - MX_LEFT_JUST,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0E813C50_UTF_8 ),		// pwcText
			_LEN_0E813C50,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_91EBCEFE_____P__CL______P__CW_2__2_2__,			// pcLeftSizeExp
			_T_LEN_AE7677BD_____P__CL______P__CW_2_,				// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UTF8,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LAYOUT_W / 2,						// iLeft
			MX_DEF_STATIC_HEIGHT_PXL + MX_TOP_JUST + MX_ROW_HEIGHT,	// iTop
			(MX_LAYOUT_W / 2) - MX_LEFT_JUST,		// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_91EBCEFE_____P__CL______P__CW_2__2_2__,			// pcLeftSizeExp
			_T_LEN_AE7677BD_____P__CL______P__CW_2_,				// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		// == 2+ bytes.
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_CWI_GROUP_LE,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_GROUPS_TOP,							// iTop
			0,										// dwWidth
			MX_GROUP_HEIGHT,						// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_9D390D9A_Little_Endian ),															// pwcText
			_LEN_9D390D9A,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_C5DC90BF_____P__CL_2,			// pcLeftSizeExp
			_T_LEN_F23BFBC4_____P__CL______P__CW_2__2_2__,														// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_CWI_GROUP_BE,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LAYOUT_W / 2,						// iLeft
			MX_GROUPS_TOP,							// iTop
			(MX_LAYOUT_W / 2) - MX_LEFT_JUST,		// dwWidth
			MX_GROUP_HEIGHT,						// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_C0AFCC9B_Big_Endian ),															// pwcText
			_LEN_C0AFCC9B,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_91EBCEFE_____P__CL______P__CW_2__2_2__,			// pcLeftSizeExp
			_T_LEN_AE7677BD_____P__CL______P__CW_2_,// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_INT16_LE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL,	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_BFD40C35_int16_t ),	// pwcText
			_LEN_BFD40C35,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_INT16_BE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL,	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_BFD40C35_int16_t ),	// pwcText
			_LEN_BFD40C35,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_INT16_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL,	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_INT16_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL,	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UINT16_LE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 1),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_A32A93D6_uint16_t ),	// pwcText
			_LEN_A32A93D6,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UINT16_BE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 1),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_A32A93D6_uint16_t ),	// pwcText
			_LEN_A32A93D6,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UINT16_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 1),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UINT16_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 1),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_INT32_LE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 2),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_12D46C62_int32_t ),	// pwcText
			_LEN_12D46C62,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_INT32_BE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 2),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_12D46C62_int32_t ),	// pwcText
			_LEN_12D46C62,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_INT32_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 2),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_INT32_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 2),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UINT32_LE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 3),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0E2AF381_uint32_t ),	// pwcText
			_LEN_0E2AF381,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UINT32_BE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 3),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0E2AF381_uint32_t ),	// pwcText
			_LEN_0E2AF381,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UINT32_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 3),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UINT32_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 3),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_INT64_LE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 4),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_2187E0E2_int64_t ),	// pwcText
			_LEN_2187E0E2,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_INT64_BE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 4),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_2187E0E2_int64_t ),	// pwcText
			_LEN_2187E0E2,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_INT64_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 4),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_INT64_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 4),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UINT64_LE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 5),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_3D797F01_uint64_t ),	// pwcText
			_LEN_3D797F01,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UINT64_BE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 5),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_3D797F01_uint64_t ),	// pwcText
			_LEN_3D797F01,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UINT64_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 5),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UINT64_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 5),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		// -=-=-=-=-=-=-=-
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UTF16_LE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 6),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_A71F1195_UTF_16 ),	// pwcText
			_LEN_A71F1195,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UTF16_BE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 6),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_A71F1195_UTF_16 ),	// pwcText
			_LEN_A71F1195,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UTF16_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 6),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UTF16_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 6),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UTF32_LE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 7),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_9244B70E_UTF_32 ),	// pwcText
			_LEN_9244B70E,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_UTF32_BE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 7),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_9244B70E_UTF_32 ),	// pwcText
			_LEN_9244B70E,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UTF32_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 7),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_UTF32_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 7),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		// -=-=-=-=-=-=

		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT10_LE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 8),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_21B8EEE8_float10 ),	// pwcText
			_LEN_21B8EEE8,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT10_BE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 8),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_21B8EEE8_float10 ),	// pwcText
			_LEN_21B8EEE8,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT10_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 8),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT10_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 8),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT11_LE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 9),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_56BFDE7E_float11 ),	// pwcText
			_LEN_56BFDE7E,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT11_BE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 9),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_56BFDE7E_float11 ),	// pwcText
			_LEN_56BFDE7E,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT11_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 9),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT11_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 9),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT14_LE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 10),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_26D52AF1_float14 ),	// pwcText
			_LEN_26D52AF1,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT14_BE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 10),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_26D52AF1_float14 ),	// pwcText
			_LEN_26D52AF1,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT14_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 10),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT14_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 10),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT16_LE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 11),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_C8DB4BDD_float16 ),	// pwcText
			_LEN_C8DB4BDD,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT16_BE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 11),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_C8DB4BDD_float16 ),	// pwcText
			_LEN_C8DB4BDD,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT16_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 11),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT16_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 11),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		/*
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT24_LE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 12),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0DF87932_float24 ),	// pwcText
			_LEN_0DF87932,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT24_BE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 12),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0DF87932_float24 ),	// pwcText
			_LEN_0DF87932,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT24_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 12),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT24_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 12),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		*/

		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT32_LE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 12),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_FD80ED46_float32 ),	// pwcText
			_LEN_FD80ED46,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT32_BE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 12),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_FD80ED46_float32 ),	// pwcText
			_LEN_FD80ED46,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT32_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 12),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT32_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 12),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT64_LE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 13),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_6994BC36_float64 ),	// pwcText
			_LEN_6994BC36,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_FLOAT64_BE_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 13),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_6994BC36_float64 ),	// pwcText
			_LEN_6994BC36,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT64_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 13),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_FLOAT64_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 13),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_HEX_LE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 14),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_06A8BD67_Base_16 ),		// pwcText
			_LEN_06A8BD67,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_HEX_BE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 14),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_06A8BD67_Base_16 ),		// pwcText
			_LEN_06A8BD67,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_HEX_LE,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 14),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_HEX_BE,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 14),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_OCT_LE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 15),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_68BEB0F1_Base_8 ),	// pwcText
			_LEN_68BEB0F1,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_OCT_BE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 15),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_68BEB0F1_Base_8 ),	// pwcText
			_LEN_68BEB0F1,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_OCT_LE,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 15),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_OCT_BE,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 15),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_BINARY_LE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 16),	// iTop
			0,										// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_886B59EF_Base_2 ),	// pwcText
			_LEN_886B59EF,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CWI_BINARY_BE_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,	// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + (MX_ROW_HEIGHT * 16),	// iTop
			0,		// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_886B59EF_Base_2 ),	// pwcText
			_LEN_886B59EF,							// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_BINARY_LE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 16),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_FBFE8371_____P__CL_2_9,			// pcLeftSizeExp
			_T_LEN_74FBB566_____P__CL______P__CW_2__2_2_9__,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CWI_BINARY_BE,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			MX_GROUPS_TOP + MX_GROUP_TOP_PXL + MX_DEF_STATIC_HEIGHT_PXL + (MX_ROW_HEIGHT * 16),	// iTop
			0,										// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CWI_MAIN_WINDOW,						// dwParentId

			_T_LEN_07E1CCEC_____P__CL______P__CW_2__2_2_9__,	// pcLeftSizeExp
			_T_LEN_95EC35E9_____P__CL______P__CW_2_9_,			// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
	};

#undef MX_GROUPS_TOP
#undef MX_GROUP_HEIGHT
#undef MX_ROW_HEIGHT

	// == Functions.
	// Creates the Converter dockable window.
	CWidget * CConverterLayout::CreateConverterWindow( CWidget * _pwParent ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ),
			vLayouts,
			sStringsW,
			sStrings );
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwThis = plmLayout->CreateWindowX( &vLayouts[0], MX_ELEMENTS( m_wlMainWindow ), nullptr, 0, _pwParent );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );

		return pwThis;
	}


#undef MX_LAYOUT_H
#undef MX_LAYOUT_W
}	// namespace mx

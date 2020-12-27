#include "MXStringTheoryLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../StringTheory/MXStringTheoryWindow.h"
#include "../Utilities/MXUtilities.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>
/*#include <Base/LSWWndClassEx.h>
#include <Layout/LSWLayoutManager.h>*/

namespace mx {

#define MX_EDIT_LINES								12
#define MX_HEX_LINES								3
#define MX_EDIT_LINE_HEIGHT							15
#define MX_EDIT_HEIGHT( LINES )						(MX_DEF_EDIT_HEIGHT_PXL + ((LINES - 1) * MX_EDIT_LINE_HEIGHT))
#define MX_HEX_EDIT_LINES							3
#define MX_STW_W									760
#define MX_STW_H									800
#define MX_LEFT_ALIGN								4
#define MX_PADDING									2
#define MX_INPUT_ENCODING_GROUP_HEIGHT				(MX_GROUP_TOP_PXL + MX_GROUP_BOTTOM_PXL + MX_DEF_COMBO_HEIGHT_PXL)
#define MX_INPUT_GROUP_TOP							2
#define MX_INPUT_GROUP_HEIGHT						((((MX_EDIT_HEIGHT( MX_EDIT_LINES )) + (MX_DEF_CHECK_HEIGHT_PXL) + MX_PADDING * 2) + MX_DEF_COMBO_HEIGHT_PXL + MX_DEF_RADIO_HEIGHT_PXL) + MX_INPUT_ENCODING_GROUP_HEIGHT + MX_GROUP_TOP_PXL + MX_GROUP_BOTTOM_PXL)
#define MX_CENTER									(MX_STW_W / 2)
#define MX_MODIFIERS_GROUP_TOP						(MX_INPUT_GROUP_TOP + MX_INPUT_GROUP_HEIGHT)
#define MX_MODIFIERS_PREDEF_HEIGHT					((MX_DEF_RADIO_HEIGHT_PXL * 2 + MX_DEF_CHECK_HEIGHT_PXL + (MX_DEF_RADIO_HEIGHT_PXL * 2)) + MX_GROUP_TOP_PXL + MX_GROUP_BOTTOM_PXL)
#define MX_MODIFIERS_GROUP_HEIGHT					((MX_MODIFIERS_PREDEF_HEIGHT) + MX_GROUP_TOP_PXL + MX_GROUP_BOTTOM_PXL)
#define MX_MODIFIERS_PREDEF_TOP						(MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL)
#define MX_MODIFIERS_PREDEF_W						350
#define MX_1_3RD									(((MX_STW_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) * 2)) * 1 / 3) + (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL))
#define MX_2_3RD									(((MX_STW_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) * 2)) * 2 / 3) + (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL))
#define MX_1_3RD_2									(((MX_STW_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL * 2) * 2)) * 1 / 3) + (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL * 2))
#define MX_2_3RD_2									(((MX_STW_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL * 2) * 2)) * 2 / 3) + (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL * 2))
#define MX_FIXED_GROUP_SIZE_1_3RD					(((MX_STW_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) * 2) - (MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN)) * 1 / 3) + (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) + (MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN))
#define MX_FIXED_GROUP_SIZE_2_3RD					(((MX_STW_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) * 2) - (MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN)) * 2 / 3) + (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) + (MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN))
#define MX_FIXED_GROUP_SIZE_1_2ND					(((MX_STW_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) * 2) - (MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN)) * 1 / 2) + (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) + (MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN))
#define MX_ENCODING_GROUP_TOP						(MX_MODIFIERS_GROUP_TOP + MX_MODIFIERS_GROUP_HEIGHT)
#define MX_ENCODING_GROUP_HEIGHT					(MX_DEF_COMBO_HEIGHT_PXL + MX_DEF_CHECK_HEIGHT_PXL + MX_DEF_RADIO_HEIGHT_PXL + (MX_GROUP_TOP_PXL + MX_GROUP_BOTTOM_PXL))
#define MX_ESCAPE_GROUP_HEIGHT						(MX_GROUP_TOP_PXL + MX_GROUP_BOTTOM_PXL + MX_DEF_RADIO_HEIGHT_PXL * 2)
#define MX_OUTPUT_GROUP_TOP							(MX_ENCODING_GROUP_TOP + MX_ENCODING_GROUP_HEIGHT)
#define MX_OUTPUT_GROUP_HEIGHT						((MX_GROUP_TOP_PXL + MX_GROUP_BOTTOM_PXL) + MX_ESCAPE_GROUP_HEIGHT)


	// == Members.
	// The layout for the PE Works window.
	LSW_WIDGET_LAYOUT CStringTheoryLayout::m_wlMainWindow[] = {
		{
			MX_STRING_THEORY,						// ltType
			MX_STW_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			75,										// iLeft
			25,										// iTop
			MX_STW_W + 16,							// dwWidth
			MX_STW_H + 26,							// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | DS_3DLOOK | DS_SETFONT,							// dwStyle
			WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,																	// dwStyleEx
			MX_MAKE_WCHAR( _T_D308869B_String_Theory ),																// pwcText
			_LEN_D308869B,							// sTextLen
			MX_STW_NONE,							// dwParentId
			/*
			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
			*/
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_STW_INPUT_GROUP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_INPUT_GROUP_TOP,						// iTop
			(MX_STW_W - (MX_LEFT_ALIGN * 2)),		// dwWidth
			MX_INPUT_GROUP_HEIGHT,					// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_57B745F8_Input_String ),																// pwcText
			_LEN_57B745F8,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,      // pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_STW_INPUT_UTF8,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL,		// iLeft
			MX_INPUT_GROUP_TOP + MX_GROUP_TOP_PXL,	// iTop
			(MX_STW_W - (MX_LEFT_ALIGN * 2)) - MX_GROUP_LEFT_PXL - MX_GROUP_RIGHT_PXL,								// dwWidth
			MX_EDIT_HEIGHT( MX_EDIT_LINES ),		// dwHeight
			MX_EDITSTYLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL,					// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_STW_UNICODE_CHARS_COMBO,				// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL,		// iLeft
			MX_INPUT_GROUP_TOP + MX_GROUP_TOP_PXL + (MX_EDIT_HEIGHT( MX_EDIT_LINES )) + MX_PADDING,					// iTop
			(MX_STW_W - (MX_LEFT_ALIGN * 2)) - MX_GROUP_LEFT_PXL - MX_GROUP_RIGHT_PXL,								// dwWidth
			MX_DEF_COMBO_HEIGHT_PXL,				// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_RADIO,							// ltType
			MX_STW_UNICODE_NAME,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL,		// iLeft
			MX_INPUT_GROUP_TOP + MX_GROUP_TOP_PXL + (MX_EDIT_HEIGHT( MX_EDIT_LINES )) + MX_PADDING * 2 + MX_DEF_COMBO_HEIGHT_PXL,
			MX_1_3RD - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL),															// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_D69E2039___N_name_ ),																	// pwcText
			_LEN_D69E2039,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			_T_LEN_5CBCB252_____P__CL________P__CW___4_9__2___1_3___4_9__,											// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_UNICODE_4_ESCAPE,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_1_3RD,								// iLeft
			MX_INPUT_GROUP_TOP + MX_GROUP_TOP_PXL + (MX_EDIT_HEIGHT( MX_EDIT_LINES )) + MX_PADDING * 2 + MX_DEF_COMBO_HEIGHT_PXL,
			MX_2_3RD - MX_1_3RD,																					// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_7E3640E5___uxxxx ),																	// pwcText
			_LEN_7E3640E5,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_5CBCB252_____P__CL________P__CW___4_9__2___1_3___4_9__,											// pcLeftSizeExp
			_T_LEN_C55ED453_____P__CL________P__CW___4_9__2___2_3___4_9__,											// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_UNICODE_8_ESCAPE,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_2_3RD,								// iLeft
			MX_INPUT_GROUP_TOP + MX_GROUP_TOP_PXL + (MX_EDIT_HEIGHT( MX_EDIT_LINES )) + MX_PADDING * 2 + MX_DEF_COMBO_HEIGHT_PXL,
			MX_STW_W - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) - MX_2_3RD,												// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_417E1750___Uxxxxxxxx ),																// pwcText
			_LEN_417E1750,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_C55ED453_____P__CL________P__CW___4_9__2___2_3___4_9__,											// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_CHECK,							// ltType
			MX_STW_RESOLVE_ESCAPES_CHECK,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL,		// iLeft
			MX_INPUT_GROUP_TOP + MX_GROUP_TOP_PXL + (MX_EDIT_HEIGHT( MX_EDIT_LINES )) + MX_PADDING * 2 + MX_DEF_COMBO_HEIGHT_PXL + MX_DEF_RADIO_HEIGHT_PXL,
			MX_1_3RD - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL),															// dwWidth
			MX_DEF_CHECK_HEIGHT_PXL,				// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_78F75E9B_Resolve_Escapes_____ ),														// pwcText
			_LEN_78F75E9B,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			_T_LEN_5CBCB252_____P__CL________P__CW___4_9__2___1_3___4_9__,											// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_STW_RESOLVE_HTML_CHECK,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_1_3RD,								// iLeft
			MX_INPUT_GROUP_TOP + MX_GROUP_TOP_PXL + (MX_EDIT_HEIGHT( MX_EDIT_LINES )) + MX_PADDING * 2 + MX_DEF_COMBO_HEIGHT_PXL + MX_DEF_RADIO_HEIGHT_PXL,
			MX_2_3RD - MX_1_3RD,																					// dwWidth
			MX_DEF_CHECK_HEIGHT_PXL,				// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_AAC469C1_Resolve_HTML_Characters ),													// pwcText
			_LEN_AAC469C1,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_5CBCB252_____P__CL________P__CW___4_9__2___1_3___4_9__,											// pcLeftSizeExp
			_T_LEN_C55ED453_____P__CL________P__CW___4_9__2___2_3___4_9__,											// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_STW_INPUT_IS_HEX_CHECK,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_2_3RD,								// iLeft
			MX_INPUT_GROUP_TOP + MX_GROUP_TOP_PXL + (MX_EDIT_HEIGHT( MX_EDIT_LINES )) + MX_PADDING * 2 + MX_DEF_COMBO_HEIGHT_PXL + MX_DEF_RADIO_HEIGHT_PXL,
			MX_STW_W - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) - MX_2_3RD,												// dwWidth
			MX_DEF_CHECK_HEIGHT_PXL,				// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_A3B76B62_Input_is_Hex_Array ),														// pwcText
			_LEN_A3B76B62,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_C55ED453_____P__CL________P__CW___4_9__2___2_3___4_9__,											// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_GROUPBOX,						// ltType
			MX_STW_INPUT_ENCODING_GROUP,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL,		// iLeft
			MX_INPUT_GROUP_TOP + MX_GROUP_TOP_PXL + (MX_EDIT_HEIGHT( MX_EDIT_LINES )) + MX_PADDING * 2 + MX_DEF_COMBO_HEIGHT_PXL + MX_DEF_RADIO_HEIGHT_PXL + MX_DEF_CHECK_HEIGHT_PXL,
			(MX_STW_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) * 2)),													// dwWidth
			MX_INPUT_ENCODING_GROUP_HEIGHT,			// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_913E9B19_Encoding ),																	// pwcText
			_LEN_913E9B19,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_STW_INPUT_ENCODING_COMBO,			// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL * 2,	// iLeft
			MX_INPUT_GROUP_TOP + MX_GROUP_TOP_PXL + (MX_EDIT_HEIGHT( MX_EDIT_LINES )) + MX_PADDING * 2 + MX_DEF_COMBO_HEIGHT_PXL + MX_DEF_RADIO_HEIGHT_PXL + MX_DEF_CHECK_HEIGHT_PXL + MX_GROUP_TOP_PXL,
			(MX_STW_W - (MX_LEFT_ALIGN * 2)) - (MX_GROUP_LEFT_PXL * 2) - (MX_GROUP_RIGHT_PXL * 2),					// dwWidth
			MX_DEF_COMBO_HEIGHT_PXL,				// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		// == Modifiers group == //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_STW_MODIFIERS_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_MODIFIERS_GROUP_TOP,					// iTop
			(MX_STW_W - (MX_LEFT_ALIGN * 2)),		// dwWidth
			MX_MODIFIERS_GROUP_HEIGHT,				// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_5A587CD0_Modifiers ),																	// pwcText
			_LEN_5A587CD0,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		// == Predefined group. == //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_STW_PREDEF_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL,		// iLeft
			MX_MODIFIERS_PREDEF_TOP,				// iTop
			MX_MODIFIERS_PREDEF_W,					// dwWidth
			MX_MODIFIERS_PREDEF_HEIGHT,				// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_0007626D_Sorted_Operations ),															// pwcText
			_LEN_0007626D,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_RADIO,							// ltType
			MX_STW_NO_CASE,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL + MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN,								// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL,																// iTop
			MX_FIXED_GROUP_SIZE_1_3RD - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL + MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN),// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_73052D63_No_Case_Change ),															// pwcText
			_LEN_73052D63,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			_T_LEN_6B1DDAC0_____P__CL________P__CW___4_9__2___350_4___1_3___4_9___350_4__,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_TOUPPER,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FIXED_GROUP_SIZE_1_3RD,								// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL,																// iTop
			MX_FIXED_GROUP_SIZE_2_3RD - MX_FIXED_GROUP_SIZE_1_3RD,													// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_A7A01CEA_To_Upper ),																	// pwcText
			_LEN_A7A01CEA,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_6B1DDAC0_____P__CL________P__CW___4_9__2___350_4___1_3___4_9___350_4__,							// pcLeftSizeExp
			_T_LEN_C84B5C69_____P__CL________P__CW___4_9__2___350_4___2_3___4_9___350_4__,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_TOLOWER,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FIXED_GROUP_SIZE_2_3RD,				// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL,																// iTop
			MX_STW_W - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) - MX_FIXED_GROUP_SIZE_2_3RD,								// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_C765B855_To_Lower ),																	// pwcText
			_LEN_C765B855,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_C84B5C69_____P__CL________P__CW___4_9__2___350_4___2_3___4_9___350_4__,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_RADIO,							// ltType
			MX_STW_NO_KANA,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL + MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN,								// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_RADIO_HEIGHT_PXL,									// iTop
			MX_FIXED_GROUP_SIZE_1_3RD - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL + MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN),// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_100367FA_No_Kana_Change ),															// pwcText
			_LEN_100367FA,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			_T_LEN_6B1DDAC0_____P__CL________P__CW___4_9__2___350_4___1_3___4_9___350_4__,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_TO_HIRAGANA,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FIXED_GROUP_SIZE_1_3RD,				// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_RADIO_HEIGHT_PXL,									// iTop
			MX_FIXED_GROUP_SIZE_2_3RD - MX_FIXED_GROUP_SIZE_1_3RD,													// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_293C9907_To_Hiragana ),																// pwcText
			_LEN_293C9907,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_6B1DDAC0_____P__CL________P__CW___4_9__2___350_4___1_3___4_9___350_4__,							// pcLeftSizeExp
			_T_LEN_C84B5C69_____P__CL________P__CW___4_9__2___350_4___2_3___4_9___350_4__,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_TO_KATAKANA,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FIXED_GROUP_SIZE_2_3RD,				// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_RADIO_HEIGHT_PXL,									// iTop
			MX_STW_W - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) - MX_FIXED_GROUP_SIZE_2_3RD,								// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_08D38C26_To_Katakana ),																// pwcText
			_LEN_08D38C26,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_C84B5C69_____P__CL________P__CW___4_9__2___350_4___2_3___4_9___350_4__,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_CHECK,							// ltType
			MX_STW_FULL_WIDTH,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL + MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN,								// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_RADIO_HEIGHT_PXL * 2,								// iTop
			MX_FIXED_GROUP_SIZE_1_2ND - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL + MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN),// dwWidth
			MX_DEF_CHECK_HEIGHT_PXL,				// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_B81AB459_Full_Width____ASCII ),														// pwcText
			_LEN_B81AB459,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			_T_LEN_ECBB1183_____P__CL________P__CW___4_9__2___350_4___1_2___4_9___350_4__,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_STW_STD_DIGITS,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FIXED_GROUP_SIZE_1_2ND,				// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_RADIO_HEIGHT_PXL * 2,								// iTop
			MX_STW_W - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) - (MX_FIXED_GROUP_SIZE_1_2ND),							// dwWidth
			MX_DEF_CHECK_HEIGHT_PXL,				// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_248960C6_Standardize_Digits ),														// pwcText
			_LEN_248960C6,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_ECBB1183_____P__CL________P__CW___4_9__2___350_4___1_2___4_9___350_4__,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_RADIO,							// ltType
			MX_STW_NORMALIZE,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL + MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN,								// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_RADIO_HEIGHT_PXL * 2 + MX_DEF_CHECK_HEIGHT_PXL,		// iTop
			MX_FIXED_GROUP_SIZE_1_3RD - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL + MX_MODIFIERS_PREDEF_W + MX_LEFT_ALIGN),// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_91C21A1E_No_Normalization ),															// pwcText
			_LEN_91C21A1E,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			_T_LEN_6B1DDAC0_____P__CL________P__CW___4_9__2___350_4___1_3___4_9___350_4__,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_NORMALIZEC,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FIXED_GROUP_SIZE_1_3RD,				// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_RADIO_HEIGHT_PXL * 2 + MX_DEF_CHECK_HEIGHT_PXL,		// iTop
			MX_FIXED_GROUP_SIZE_2_3RD - MX_FIXED_GROUP_SIZE_1_3RD,													// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_BBA4ACA5_NFC__form_C_ ),																// pwcText
			_LEN_BBA4ACA5,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_6B1DDAC0_____P__CL________P__CW___4_9__2___350_4___1_3___4_9___350_4__,							// pcLeftSizeExp
			_T_LEN_C84B5C69_____P__CL________P__CW___4_9__2___350_4___2_3___4_9___350_4__,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_NORMALIZED,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FIXED_GROUP_SIZE_2_3RD,				// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_RADIO_HEIGHT_PXL * 2 + MX_DEF_CHECK_HEIGHT_PXL,		// iTop
			MX_STW_W - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) - MX_FIXED_GROUP_SIZE_2_3RD,								// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_1639211B_NFD__form_D_ ),																// pwcText
			_LEN_1639211B,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_C84B5C69_____P__CL________P__CW___4_9__2___350_4___2_3___4_9___350_4__,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_RADIO,							// ltType
			MX_STW_NORMALIZEKC,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FIXED_GROUP_SIZE_1_3RD,				// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_RADIO_HEIGHT_PXL * 2 + MX_DEF_CHECK_HEIGHT_PXL + MX_DEF_RADIO_HEIGHT_PXL,
			MX_FIXED_GROUP_SIZE_2_3RD - MX_FIXED_GROUP_SIZE_1_3RD,													// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_37F71908_NFKC__form_KC_ ),															// pwcText
			_LEN_37F71908,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_6B1DDAC0_____P__CL________P__CW___4_9__2___350_4___1_3___4_9___350_4__,							// pcLeftSizeExp
			_T_LEN_C84B5C69_____P__CL________P__CW___4_9__2___350_4___2_3___4_9___350_4__,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_NORMALIZEKD,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FIXED_GROUP_SIZE_2_3RD,				// iLeft
			MX_MODIFIERS_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_RADIO_HEIGHT_PXL * 2 + MX_DEF_CHECK_HEIGHT_PXL + MX_DEF_RADIO_HEIGHT_PXL,
			MX_STW_W - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) - MX_FIXED_GROUP_SIZE_2_3RD,								// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_518D9A4C_NFKD__form_KD_ ),															// pwcText
			_LEN_518D9A4C,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_C84B5C69_____P__CL________P__CW___4_9__2___350_4___2_3___4_9___350_4__,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		// == Encoding group. == //
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_STW_ENCODING_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_ENCODING_GROUP_TOP,					// iTop
			(MX_STW_W - (MX_LEFT_ALIGN * 2)),		// dwWidth
			MX_ENCODING_GROUP_HEIGHT,				// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_913E9B19_Encoding ),																	// pwcText
			_LEN_913E9B19,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_STW_ENCODING_COMBO,					// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL,		// iLeft
			MX_ENCODING_GROUP_TOP + MX_GROUP_TOP_PXL,																// iTop
			(MX_STW_W - (MX_LEFT_ALIGN * 2)) - MX_GROUP_LEFT_PXL - MX_GROUP_RIGHT_PXL,								// dwWidth
			MX_DEF_COMBO_HEIGHT_PXL,				// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_CHECK,							// ltType
			MX_STW_COMPOSITE,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL,		// iLeft
			MX_ENCODING_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_COMBO_HEIGHT_PXL,										// iTop
			MX_CENTER - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL),														// dwWidth
			MX_DEF_CHECK_HEIGHT_PXL,				// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_584EF466_Composite_Check ),															// pwcText
			_LEN_584EF466,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_CENTER_H,						// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_STW_NO_BEST_FIT,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_CENTER,								// iLeft
			MX_ENCODING_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_COMBO_HEIGHT_PXL,										// iTop
			MX_CENTER - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL),														// dwWidth
			MX_DEF_CHECK_HEIGHT_PXL,				// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_FF124F7B_No_Best_Fit_Chars ),															// pwcText
			_LEN_FF124F7B,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_CENTER_H,						// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_RADIO,							// ltType
			MX_STW_DEFAULT_CHAR,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL,		// iLeft
			MX_ENCODING_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_COMBO_HEIGHT_PXL + MX_DEF_CHECK_HEIGHT_PXL,			// iTop
			MX_1_3RD - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL),															// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_0A8E6F41_Default_Char ),																// pwcText
			_LEN_0A8E6F41,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			_T_LEN_5CBCB252_____P__CL________P__CW___4_9__2___1_3___4_9__,											// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_DISCARD_NONSPACING,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_1_3RD,								// iLeft
			MX_ENCODING_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_COMBO_HEIGHT_PXL + MX_DEF_CHECK_HEIGHT_PXL,			// iTop
			MX_2_3RD - MX_1_3RD,																					// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_F537A44C_Discard_Nonspacing ),														// pwcText
			_LEN_F537A44C,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_5CBCB252_____P__CL________P__CW___4_9__2___1_3___4_9__,											// pcLeftSizeExp
			_T_LEN_C55ED453_____P__CL________P__CW___4_9__2___2_3___4_9__,											// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_SEP_CHARS,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_2_3RD,								// iLeft
			MX_ENCODING_GROUP_TOP + MX_GROUP_TOP_PXL + MX_DEF_COMBO_HEIGHT_PXL + MX_DEF_CHECK_HEIGHT_PXL,			// iTop
			MX_STW_W - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) - MX_2_3RD,												// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_1F50680C_Separate_Chars ),															// pwcText
			_LEN_1F50680C,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_C55ED453_____P__CL________P__CW___4_9__2___2_3___4_9__,											// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_GROUPBOX,						// ltType
			MX_STW_OUTPUT_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_OUTPUT_GROUP_TOP,					// iTop
			(MX_STW_W - (MX_LEFT_ALIGN * 2)),		// dwWidth
			MX_OUTPUT_GROUP_HEIGHT,					// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_E4916641_Output_String ),																// pwcText
			_LEN_E4916641,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_GROUPBOX,						// ltType
			MX_STW_ESCAPE_GROUP,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL,		// iLeft
			MX_OUTPUT_GROUP_TOP + MX_GROUP_TOP_PXL,	// iTop
			(MX_STW_W - ((MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL) * 2)),													// dwWidth
			MX_ESCAPE_GROUP_HEIGHT,					// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_2244B5B8_Escape ),																	// pwcText
			_LEN_2244B5B8,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},


		{
			LSW_LT_RADIO,							// ltType
			MX_STW_ESPACE_ONLY_NUL,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL * 2,	// iLeft
			MX_OUTPUT_GROUP_TOP + MX_GROUP_TOP_PXL * 2,																// iTop
			MX_1_3RD_2 - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL * 2),													// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE | WS_GROUP,				// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_F02C6FC7_NUL_Only ),																	// pwcText
			_LEN_F02C6FC7,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			_T_LEN_5657B212_____P__CL________P__CW___4_9_2__2___1_3___4_9_2__,										// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_ESPACE_ALL_UNPRINTABLE,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_1_3RD_2,								// iLeft
			MX_OUTPUT_GROUP_TOP + MX_GROUP_TOP_PXL * 2,																// iTop
			MX_2_3RD_2 - MX_1_3RD_2,																				// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_4A490AF1_All_Unprintable ),															// pwcText
			_LEN_4A490AF1,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_5657B212_____P__CL________P__CW___4_9_2__2___1_3___4_9_2__,										// pcLeftSizeExp
			_T_LEN_EB9DDEDC_____P__CL________P__CW___4_9_2__2___2_3___4_9_2__,										// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_ESPACE_ALL,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_2_3RD_2,								// iLeft
			MX_OUTPUT_GROUP_TOP + MX_GROUP_TOP_PXL * 2,																// iTop
			MX_STW_W - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL * 2) - MX_2_3RD_2,										// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_0355373D_All ),																		// pwcText
			_LEN_0355373D,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_EB9DDEDC_____P__CL________P__CW___4_9_2__2___2_3___4_9_2__,										// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_ESPACE_ONLY_NUL_AND_BS,			// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL * 2,	// iLeft
			MX_OUTPUT_GROUP_TOP + MX_GROUP_TOP_PXL * 2 + MX_DEF_RADIO_HEIGHT_PXL,									// iTop
			MX_1_3RD_2 - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL * 2),													// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_F7C38982_NUL_and___ ),																// pwcText
			_LEN_F7C38982,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			_T_LEN_5657B212_____P__CL________P__CW___4_9_2__2___1_3___4_9_2__,										// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_ESPACE_ALL_UNPRINTABLE_AND_BS,	// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_1_3RD_2,								// iLeft
			MX_OUTPUT_GROUP_TOP + MX_GROUP_TOP_PXL * 2 + MX_DEF_RADIO_HEIGHT_PXL,									// iTop
			MX_2_3RD_2 - MX_1_3RD_2,																				// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_8D794A68_All_Unprintable_and___ ),													// pwcText
			_LEN_8D794A68,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_5657B212_____P__CL________P__CW___4_9_2__2___1_3___4_9_2__,										// pcLeftSizeExp
			_T_LEN_EB9DDEDC_____P__CL________P__CW___4_9_2__2___2_3___4_9_2__,										// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_STW_ESPACE_STANDARD,					// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_2_3RD_2,								// iLeft
			MX_OUTPUT_GROUP_TOP + MX_GROUP_TOP_PXL * 2 + MX_DEF_RADIO_HEIGHT_PXL,									// iTop
			MX_STW_W - (MX_LEFT_ALIGN + MX_GROUP_LEFT_PXL * 2) - MX_2_3RD_2,										// dwWidth
			MX_DEF_RADIO_HEIGHT_PXL,				// dwHeight
			MX_RADIOSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,							// dwStyleEx
			MX_MAKE_WCHAR( _T_977CA617_Standard_Escapes ),															// pwcText
			_LEN_977CA617,							// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			_T_LEN_EB9DDEDC_____P__CL________P__CW___4_9_2__2___2_3___4_9_2__,										// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_EDIT,							// ltType
			MX_STW_OUTPUT_HEX_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_OUTPUT_GROUP_TOP + MX_OUTPUT_GROUP_HEIGHT,															// iTop
			(MX_STW_W - (MX_LEFT_ALIGN * 2)),		// dwWidth
			MX_EDIT_HEIGHT( MX_HEX_LINES ),			// dwHeight
			MX_EDITSTYLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL | WS_HSCROLL,					// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_STW_OUTPUT_EDIT,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_ALIGN,							// iLeft
			MX_OUTPUT_GROUP_TOP + MX_OUTPUT_GROUP_HEIGHT + MX_EDIT_HEIGHT( MX_HEX_LINES ),							// iTop
			(MX_STW_W - (MX_LEFT_ALIGN * 2)),		// dwWidth
			MX_STW_H - (MX_OUTPUT_GROUP_TOP + MX_OUTPUT_GROUP_HEIGHT + MX_EDIT_HEIGHT( MX_HEX_LINES )) - MX_LEFT_ALIGN * 4,
			MX_EDITSTYLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL | WS_HSCROLL,					// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_STW_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
	};

#undef MX_OUTPUT_GROUP_HEIGHT
#undef MX_OUTPUT_GROUP_TOP
#undef MX_ESCAPE_GROUP_HEIGHT
#undef MX_ENCODING_GROUP_HEIGHT
#undef MX_ENCODING_GROUP_TOP
#undef MX_FIXED_GROUP_SIZE_1_2ND
#undef MX_FIXED_GROUP_SIZE_2_3RD
#undef MX_FIXED_GROUP_SIZE_1_3RD
#undef MX_2_3RD_2
#undef MX_1_3RD_2
#undef MX_2_3RD
#undef MX_1_3RD
#undef MX_MODIFIERS_PREDEF_TOP
#undef MX_MODIFIERS_GROUP_HEIGHT
#undef MX_MODIFIERS_PREDEF_HEIGHT
#undef MX_MODIFIERS_GROUP_TOP
#undef MX_CENTER
#undef MX_INPUT_GROUP_HEIGHT
#undef MX_INPUT_GROUP_TOP
#undef MX_INPUT_ENCODING_GROUP_HEIGHT
#undef MX_PADDING
#undef MX_LEFT_ALIGN
#undef MX_STW_W
#undef MX_STW_H
#undef MX_HEX_EDIT_LINES
#undef MX_EDIT_LINE_HEIGHT
#undef MX_EDIT_LINES

#if 0
	// Menu bar items for the main window.
	LSW_MENU_ITEM CStringTheoryLayout::m_miMenuBar[] = {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_STW_MENU_FILE,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_854A6B8B__File, _LEN_854A6B8B ) },
		/*{ FALSE,		MX_STW_MENU_SEARCH,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_339FACEA__Search, _LEN_339FACEA ) },
		{ FALSE,		MX_STW_MENU_TOOLS,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_8D2E41E6__Tools, _LEN_8D2E41E6 ) },
		{ FALSE,		MX_STW_MENU_WINDOW,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0C8B8E90__Window, _LEN_0C8B8E90 ) },
		{ FALSE,		MX_STW_MENU_HELP,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_01520137__Help, _LEN_01520137 ) },*/
	};

	// File menu.
	LSW_MENU_ITEM CStringTheoryLayout::m_miFileMenu[] {
		//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_STW_OPENPE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2D715017_Open_PE_File, _LEN_2D715017 ) },
		{ FALSE,		MX_STW_OPENRECENT,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EA79B404_Open__Recent, _LEN_EA79B404 ) },
	};

	// Menus.
	LSW_MENU_LAYOUT CStringTheoryLayout::m_miMenus[] = {
		{
			MX_STW_MENU_BAR,
			0,
			0,
			MX_ELEMENTS( m_miMenuBar ),
			m_miMenuBar
		},
		{
			MX_STW_FILE,
			MX_STW_MENU_BAR,
			MX_STW_MENU_FILE,
			MX_ELEMENTS( m_miFileMenu ),
			m_miFileMenu
		},
	};
#endif

	// == Functions.
	// Creates the PE Works window.
	CWidget * CStringTheoryLayout::CreateStringTheoryWindow( CWidget * _pwParent ) {
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		CLayoutManager::UnencryptLayouts( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ),
			vLayouts,
			sStringsW,
			sStrings );

		std::vector<LSW_MENU_LAYOUT> vMenus;
		std::vector<std::vector<LSW_MENU_ITEM> *> vMenuItems;
		std::vector<std::vector<CSecureWString> *> vMenuStrings;
		/*CLayoutManager::UnencryptMenu( m_miMenus, MX_ELEMENTS( m_miMenus ),
			vMenus,
			vMenuItems,
			vMenuStrings );*/


		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		CStringTheoryWindow::PrepareStringTheory();
		CWidget * pwThis = plmLayout->CreateWindowX( &vLayouts[0], MX_ELEMENTS( m_wlMainWindow ),
			vMenus.size() ? &vMenus[0] : nullptr, vMenus.size(),
			_pwParent );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		CLayoutManager::CleanEncryptedMenu( vMenuItems, vMenuStrings );

		return pwThis;
	}

}	// namespace mx

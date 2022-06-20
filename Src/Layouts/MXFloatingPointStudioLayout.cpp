#include "MXFloatingPointStudioLayout.h"
#include "../FloatingPointStudio/MXFloatingPointStudioWindow.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>

namespace mx {

#define MX_FPS_W				(65*(15-MX_FPS_CHECKBOX_TRIM*2)+2)
#define MX_FPS_IN_GROUP_TOP		(MX_TOP_JUST + MX_FPS_PARMS_GROUP_HEIGHT)
#define MX_FPS_INPUT_LABEL_W	75
#define MX_FPS_PARM_INTERIOR_W	(MX_FPS_W - (MX_LEFT_JUST + MX_GROUP_LEFT_PXL) * 2 - 200)
#define MX_FPS_1_5TH			MX_EVEN_DIVIDE( MX_FPS_PARM_INTERIOR_W, MX_LEFT_JUST + MX_GROUP_LEFT_PXL, 5, 0 ) //((MX_FPS_PARM_INTERIOR_W * 0) / 5 + MX_LEFT_JUST + MX_GROUP_LEFT_PXL)
#define MX_FPS_2_5TH			MX_EVEN_DIVIDE( MX_FPS_PARM_INTERIOR_W, MX_LEFT_JUST + MX_GROUP_LEFT_PXL, 5, 1 ) //((MX_FPS_PARM_INTERIOR_W * 1) / 5 + MX_LEFT_JUST + MX_GROUP_LEFT_PXL)
#define MX_FPS_3_5TH			MX_EVEN_DIVIDE( MX_FPS_PARM_INTERIOR_W, MX_LEFT_JUST + MX_GROUP_LEFT_PXL, 5, 2 ) //((MX_FPS_PARM_INTERIOR_W * 2) / 5 + MX_LEFT_JUST + MX_GROUP_LEFT_PXL)
#define MX_FPS_4_5TH			MX_EVEN_DIVIDE( MX_FPS_PARM_INTERIOR_W, MX_LEFT_JUST + MX_GROUP_LEFT_PXL, 5, 3 ) //((MX_FPS_PARM_INTERIOR_W * 3) / 5 + MX_LEFT_JUST + MX_GROUP_LEFT_PXL)
#define MX_FPS_5_5TH			MX_EVEN_DIVIDE( MX_FPS_PARM_INTERIOR_W, MX_LEFT_JUST + MX_GROUP_LEFT_PXL, 5, 4 ) //((MX_FPS_PARM_INTERIOR_W * 4) / 5 + MX_LEFT_JUST + MX_GROUP_LEFT_PXL)
#define MX_FPS_PARM_SIGN_W		70
#define MX_FPS_PARM_EXP_W		76
#define MX_FPS_PARM_IMP_W		110
#define MX_FPS_PARM_MAN_W		74
#define MX_FPS_PARM_PRE_W		37
#define MX_FPS_LIMITS_TOP		(((MX_FPS_IN_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3 + MX_FPS_INPUT_LABEL_W) + MX_DEF_STATIC_HEIGHT_PXL + (::GetSystemMetrics( SM_CYMENUCHECK ) - MX_FPS_CHECKBOX_TRIM * 2) + MX_DEF_STATIC_HEIGHT_PXL * 2)
#define MX_FPS_LIMITS_H			(MX_GROUP_TOP_PXL + MX_GROUP_BOTTOM_PXL + MX_DEF_EDIT_HEIGHT_PXL + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 7)
#define MX_FPS_LIMITS_LABEL_W	75
#define MX_FPS_LIMITS_HEX_W		123//((((MX_FPS_W - (MX_LEFT_JUST + MX_GROUP_LEFT_PXL) * 4) / 5) - MX_FPS_LIMITS_LABEL_W) / 2 - 2)
#define MX_FPS_LIMITS_BIN_L		(MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_LIMITS_LABEL_W + MX_FPS_LIMITS_HEX_W + 2)
#define MX_FPS_LIMITS_BIN_W		(MX_FPS_W - (MX_LEFT_JUST + MX_GROUP_LEFT_PXL) - MX_FPS_LIMITS_BIN_L)
#define MX_FPS_BIAS_RIGHT		(MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_LIMITS_LABEL_W + MX_FPS_LIMITS_HEX_W)
#define MX_FPS_MISC_GROUP_TOP	(MX_FPS_LIMITS_TOP + MX_FPS_LIMITS_H)
#define MX_FPS_MISC_GROUP_H		(MX_GROUP_TOP_PXL + MX_GROUP_BOTTOM_PXL + MX_DEF_EDIT_HEIGHT_PXL + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3)
#define MX_FPS_MISC_LABEL_W		75
#define MX_FPS_MISC_INTERIOR_W	(MX_FPS_W - (MX_LEFT_JUST + MX_GROUP_LEFT_PXL) * 1)
#define MX_FPS_MISC_1_5TH		((MX_FPS_MISC_INTERIOR_W * 0) / 4 + MX_LEFT_JUST + MX_GROUP_LEFT_PXL)
#define MX_FPS_MISC_2_5TH		((MX_FPS_MISC_INTERIOR_W * 1) / 4 + MX_LEFT_JUST + MX_GROUP_LEFT_PXL)
#define MX_FPS_MISC_3_5TH		((MX_FPS_MISC_INTERIOR_W * 2) / 4 + MX_LEFT_JUST + MX_GROUP_LEFT_PXL)
#define MX_FPS_MISC_4_5TH		((MX_FPS_MISC_INTERIOR_W * 3) / 4 + MX_LEFT_JUST + MX_GROUP_LEFT_PXL)
#define MX_FPS_MISC_5_5TH		((MX_FPS_MISC_INTERIOR_W * 4) / 4 + MX_LEFT_JUST + MX_GROUP_LEFT_PXL)
#define MX_FPS_H				(MX_FPS_MISC_GROUP_TOP + MX_FPS_MISC_GROUP_H + MX_TOP_JUST)

	// == Members.
	// The layout for the PE Works window.
	LSW_WIDGET_LAYOUT CFloatingPointStudioLayout::m_wlMainWindow[] = {
		{
			MX_FLOATING_POINT_STUDIO,				// ltType
			MX_CI_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			350,									// iLeft
			250,									// iTop
			MX_FPS_W,								// dwWidth
			static_cast<DWORD>(MX_FPS_H),			// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,	// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_830F43B3_Floating_Point_Studio ),																							// pwcText
			_LEN_830F43B3,							// sTextLen
			MX_CI_NONE,								// dwParentId
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
			MX_CI_PARM_GROUP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_TOP_JUST,							// iTop
			MX_FPS_W - (MX_LEFT_JUST * 2),			// dwWidth
			MX_FPS_PARMS_GROUP_HEIGHT,				// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_49CE4B2E_Parameters ),// pwcText
			_LEN_49CE4B2E,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_CI_PARM_SIGNBIT_CHECK,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_1_5TH,							// iLeft
			MX_GROUP_TOP_PXL + MX_TOP_JUST + 2,		// iTop
			MX_FPS_PARM_SIGN_W,						// dwWidth
			MX_DEF_CHECK_HEIGHT_PXL,				// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_A32D47EF_Sign_Bit ),	// pwcText
			_LEN_A32D47EF,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CI_PARM_EXP_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_2_5TH,							// iLeft
			MX_GROUP_TOP_PXL + MX_TOP_JUST + 3,		// iTop
			MX_FPS_PARM_EXP_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_E5FB9FBC_Exponent_Bits_ ),											// pwcText
			_LEN_E5FB9FBC,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_PARM_EXP_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_2_5TH + MX_FPS_PARM_EXP_W,		// iLeft
			MX_GROUP_TOP_PXL + MX_TOP_JUST,			// iTop
			MX_FPS_3_5TH - (MX_FPS_2_5TH + MX_FPS_PARM_EXP_W) - MX_GROUP_LEFT_PXL,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		
		{
			LSW_LT_LABEL,							// ltType
			MX_CI_PARM_MAN_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_3_5TH,							// iLeft
			MX_GROUP_TOP_PXL + MX_TOP_JUST + 3,		// iTop
			MX_FPS_PARM_MAN_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_6290E7C4_Mantissa_Bits_ ),											// pwcText
			_LEN_6290E7C4,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_PARM_MAN_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_3_5TH + MX_FPS_PARM_MAN_W,		// iLeft
			MX_GROUP_TOP_PXL + MX_TOP_JUST,			// iTop
			(MX_FPS_4_5TH - (MX_LEFT_JUST + MX_GROUP_LEFT_PXL)) - (MX_FPS_3_5TH + MX_FPS_PARM_MAN_W),	// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_CHECK,							// ltType
			MX_CI_PARM_IMPBIT_CHECK,				// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_4_5TH,							// iLeft
			MX_GROUP_TOP_PXL + MX_TOP_JUST + 2,		// iTop
			MX_FPS_PARM_IMP_W,						// dwWidth
			MX_DEF_CHECK_HEIGHT_PXL,				// dwHeight
			MX_CHECKSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_2085F958_Imp__Mantissa_Bit ),	// pwcText
			_LEN_2085F958,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CI_PARM_PRESET_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_5_5TH,							// iLeft
			MX_GROUP_TOP_PXL + MX_TOP_JUST + 3,		// iTop
			MX_FPS_PARM_PRE_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_134801C7_Preset_ ),	// pwcText
			_LEN_134801C7,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_CI_PARM_PRESET_COMBO,				// wId
			WC_COMBOBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_5_5TH + MX_FPS_PARM_PRE_W,		// iLeft
			MX_GROUP_TOP_PXL + MX_TOP_JUST,			// iTop
			(MX_FPS_W - (MX_LEFT_JUST + MX_GROUP_LEFT_PXL)) - (MX_FPS_5_5TH + MX_FPS_PARM_PRE_W),	// dwWidth
			MX_DEF_COMBO_HEIGHT_PXL,				// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			MX_COMBOSTYLEEX_LIST,					// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_GROUPBOX,						// ltType
			MX_CI_INPUT_GROUP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_FPS_IN_GROUP_TOP,					// iTop
			MX_FPS_W - (MX_LEFT_JUST * 2),			// dwWidth
			MX_FPS_INPUT_AREA,						// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_19E91DD3_Input ),		// pwcText
			_LEN_19E91DD3,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CI_VALUE_LABEL,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			(MX_FPS_IN_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3,						// iTop
			MX_FPS_INPUT_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_E54DB928_Expression_ ),										// pwcText
			_LEN_E54DB928,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_VALUE_EDIT,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_INPUT_LABEL_W,						// iLeft
			(MX_FPS_IN_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST),							// iTop
			MX_FPS_W - ((MX_LEFT_JUST + MX_GROUP_LEFT_PXL) * 2) - MX_FPS_INPUT_LABEL_W,		// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CI_VALUE_HEX_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			(MX_FPS_IN_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3 + (MX_DEF_EDIT_HEIGHT_PXL + 2),
			MX_FPS_INPUT_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_C7DE6A79_Base_16_ ),	// pwcText
			_LEN_C7DE6A79,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_VALUE_HEX_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_INPUT_LABEL_W,								// iLeft
			(MX_FPS_IN_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2),
			MX_FPS_W - ((MX_LEFT_JUST + MX_GROUP_LEFT_PXL) * 2) - MX_FPS_INPUT_LABEL_W,			// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CI_VALUE_BIN_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			(MX_FPS_IN_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3 + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 2,
			MX_FPS_INPUT_LABEL_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_2433A28F_Base_2_ ),	// pwcText
			_LEN_2433A28F,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_VALUE_BIN_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_INPUT_LABEL_W,								// iLeft
			(MX_FPS_IN_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 2,
			MX_FPS_W - ((MX_LEFT_JUST + MX_GROUP_LEFT_PXL) * 2) - MX_FPS_INPUT_LABEL_W,			// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE,							// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},


		{
			LSW_LT_GROUPBOX,						// ltType
			MX_CI_LIMITS_GROUP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_FPS_LIMITS_TOP,						// iTop
			MX_FPS_W - (MX_LEFT_JUST * 2),			// dwWidth
			MX_FPS_LIMITS_H,						// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_516F3D8D_Limits ),	// pwcText
			_LEN_516F3D8D,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CI_OUT_PINF_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3,						// iTop
			MX_FPS_LIMITS_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_2FDF1AED__Infinity_ ),// pwcText
			_LEN_2FDF1AED,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_PINF_EDIT_HEX,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_LIMITS_LABEL_W,						// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST),							// iTop
			MX_FPS_LIMITS_HEX_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_PINF_EDIT_BIN,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_LIMITS_BIN_L,					// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST),							// iTop
			MX_FPS_LIMITS_BIN_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CI_OUT_NINF_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			((MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + MX_DEF_EDIT_HEIGHT_PXL + 2,						// iTop
			MX_FPS_LIMITS_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_22C16AAA__Infinity_ ),// pwcText
			_LEN_22C16AAA,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_NINF_EDIT_HEX,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_LIMITS_LABEL_W,						// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + MX_DEF_EDIT_HEIGHT_PXL + 2,							// iTop
			MX_FPS_LIMITS_HEX_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_NINF_EDIT_BIN,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_LIMITS_BIN_L,					// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + MX_DEF_EDIT_HEIGHT_PXL + 2,							// iTop
			MX_FPS_LIMITS_BIN_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CI_OUT_NAN_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			((MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 2,					// iTop
			MX_FPS_LIMITS_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_BB6CEA3C_NaN_ ),		// pwcText
			_LEN_BB6CEA3C,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_NAN_EDIT_HEX,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_LIMITS_LABEL_W,														// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 2,						// iTop
			MX_FPS_LIMITS_HEX_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_NAN_EDIT_BIN,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_LIMITS_BIN_L,					// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 2,						// iTop
			MX_FPS_LIMITS_BIN_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CI_OUT_MAX_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			((MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3,					// iTop
			MX_FPS_LIMITS_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_71B2E2B3__Max_ ),		// pwcText
			_LEN_71B2E2B3,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_MAX_EDIT_HEX,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_LIMITS_LABEL_W,														// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3,						// iTop
			MX_FPS_LIMITS_HEX_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_MAX_EDIT_FLT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_LIMITS_BIN_L,					// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3,						// iTop
			MX_FPS_LIMITS_BIN_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CI_OUT_MIN_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			((MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 4,					// iTop
			MX_FPS_LIMITS_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_3353A0EC_Non_0__Min_ ),																		// pwcText
			_LEN_3353A0EC,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_MIN_EDIT_HEX,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_LIMITS_LABEL_W,														// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 4,						// iTop
			MX_FPS_LIMITS_HEX_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_MIN_EDIT_FLT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_LIMITS_BIN_L,					// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 4,						// iTop
			MX_FPS_LIMITS_BIN_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CI_OUT_NMIN_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			((MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 5,					// iTop
			MX_FPS_LIMITS_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0B5A0E4E_Norm__Min_ ),	// pwcText
			_LEN_0B5A0E4E,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_NMIN_EDIT_HEX,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_LIMITS_LABEL_W,														// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 5,						// iTop
			MX_FPS_LIMITS_HEX_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_NMIN_EDIT_FLT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_LIMITS_BIN_L,					// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 5,						// iTop
			MX_FPS_LIMITS_BIN_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CI_OUT_EPS_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			((MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 6,					// iTop
			MX_FPS_LIMITS_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_FEAAF2A5_Epsilon_ ),	// pwcText
			_LEN_FEAAF2A5,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_EPS_EDIT_HEX,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_LIMITS_LABEL_W,														// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 6,						// iTop
			MX_FPS_LIMITS_HEX_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_EPS_EDIT_FLT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_LIMITS_BIN_L,					// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 6,						// iTop
			MX_FPS_LIMITS_BIN_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},


		{
			LSW_LT_LABEL,							// ltType
			MX_CI_OUT_BIAS_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			((MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 7,					// iTop
			MX_FPS_LIMITS_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_2934162D_Bias_ ),		// pwcText
			_LEN_2934162D,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_BIAS_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_LIMITS_LABEL_W,														// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 7,						// iTop
			MX_FPS_LIMITS_HEX_W,					// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CI_OUT_PREC_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE( MX_FPS_W - (MX_LEFT_JUST + MX_FPS_BIAS_RIGHT), MX_LEFT_JUST + MX_FPS_BIAS_RIGHT, 1, 0 ),	// iLeft
			((MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 7,					// iTop
			MX_FPS_LIMITS_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_0FC09248_Precision_ ),// pwcText
			_LEN_0FC09248,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_PREC_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE( MX_FPS_W - (MX_LEFT_JUST + MX_FPS_BIAS_RIGHT), MX_LEFT_JUST + MX_FPS_BIAS_RIGHT, 1, 0 ) + MX_FPS_LIMITS_LABEL_W,														// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 7,						// iTop
			MX_EVEN_DIVIDE( MX_FPS_W - (MX_LEFT_JUST + MX_FPS_BIAS_RIGHT), MX_LEFT_JUST + MX_FPS_BIAS_RIGHT, 1, 1 ) - 
				(MX_EVEN_DIVIDE( MX_FPS_W - (MX_LEFT_JUST + MX_FPS_BIAS_RIGHT), MX_LEFT_JUST + MX_FPS_BIAS_RIGHT, 1, 0 ) + MX_FPS_LIMITS_LABEL_W) -
				(MX_LEFT_JUST + MX_GROUP_LEFT_PXL),																			// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		/*{
			LSW_LT_LABEL,							// ltType
			MX_CI_OUT_PREC_LOW_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE( MX_FPS_W - (MX_LEFT_JUST + MX_FPS_BIAS_RIGHT), MX_LEFT_JUST + MX_FPS_BIAS_RIGHT, 2, 1 ),	// iLeft
			((MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 7,					// iTop
			MX_FPS_LIMITS_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_72875955_Precision___1_ ),		// pwcText
			_LEN_72875955,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_OUT_PREC_LOW_EDIT,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EVEN_DIVIDE( MX_FPS_W - (MX_LEFT_JUST + MX_FPS_BIAS_RIGHT), MX_LEFT_JUST + MX_FPS_BIAS_RIGHT, 2, 1 ) + MX_FPS_LIMITS_LABEL_W,														// iLeft
			(MX_FPS_LIMITS_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 7,						// iTop
			MX_EVEN_DIVIDE( MX_FPS_W - (MX_LEFT_JUST + MX_FPS_BIAS_RIGHT), MX_LEFT_JUST + MX_FPS_BIAS_RIGHT, 2, 2 ) - 
				(MX_EVEN_DIVIDE( MX_FPS_W - (MX_LEFT_JUST + MX_FPS_BIAS_RIGHT), MX_LEFT_JUST + MX_FPS_BIAS_RIGHT, 2, 1 ) + MX_FPS_LIMITS_LABEL_W) -
				(MX_LEFT_JUST + MX_GROUP_LEFT_PXL),																			// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},*/


		{
			LSW_LT_GROUPBOX,						// ltType
			MX_CI_MISC_GROUP,						// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST,							// iLeft
			MX_FPS_MISC_GROUP_TOP,					// iTop
			MX_FPS_W - (MX_LEFT_JUST * 2),			// dwWidth
			MX_FPS_MISC_GROUP_H,					// dwHeight
			MX_GROUPSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_E4DE30CE_Misc_ ),		// pwcText
			_LEN_E4DE30CE,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_CI_MISC_NEXT_ABOVE_LABEL,			// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			(MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3,					// iTop
			MX_FPS_MISC_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_BEE2C309_Next_Above_ ),										// pwcText
			_LEN_BEE2C309,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_MISC_NEXT_ABOVE_EDIT,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_MISC_LABEL_W,							// iLeft
			(MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST),						// iTop
			MX_FPS_W - ((MX_LEFT_JUST + MX_GROUP_LEFT_PXL) * 2) - MX_FPS_MISC_LABEL_W,		// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CI_MISC_ACTUAL_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2),
			MX_FPS_MISC_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_EE10D731_Actual_Value_ ),										// pwcText
			_LEN_EE10D731,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_MISC_ACTUAL_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_MISC_LABEL_W,							// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST)) + (MX_DEF_EDIT_HEIGHT_PXL + 2),
			MX_FPS_W - ((MX_LEFT_JUST + MX_GROUP_LEFT_PXL) * 2) - MX_FPS_MISC_LABEL_W,		// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CI_MISC_NEXT_BELOW_LABEL,			// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL,		// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 2,
			MX_FPS_MISC_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_F3E02A75_Next_Below_ ),										// pwcText
			_LEN_F3E02A75,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_MISC_NEXT_BELOW_EDIT,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_LEFT_JUST + MX_GROUP_LEFT_PXL + MX_FPS_MISC_LABEL_W,							// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST)) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 2,
			MX_FPS_W - ((MX_LEFT_JUST + MX_GROUP_LEFT_PXL) * 2) - MX_FPS_MISC_LABEL_W,		// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CI_MISC_TOTAL_BITS_LABEL,			// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_MISC_1_5TH,						// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3,
			MX_FPS_MISC_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_8C43D147_Total_Bits_ ),										// pwcText
			_LEN_8C43D147,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_MISC_TOTAL_BITS_EDIT,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_MISC_1_5TH + MX_FPS_MISC_LABEL_W,// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST)) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3,
			(MX_FPS_MISC_2_5TH - MX_FPS_MISC_1_5TH) - MX_FPS_MISC_LABEL_W - (MX_LEFT_JUST + MX_GROUP_LEFT_PXL),// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CI_MISC_EXPONENT_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_MISC_2_5TH,						// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3,
			MX_FPS_MISC_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_FD9FF810_Exponent_ ),											// pwcText
			_LEN_FD9FF810,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_MISC_EXPONENT_EDIT,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_MISC_2_5TH + MX_FPS_MISC_LABEL_W,// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST)) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3,
			(MX_FPS_MISC_3_5TH - MX_FPS_MISC_2_5TH) - MX_FPS_MISC_LABEL_W - (MX_LEFT_JUST + MX_GROUP_LEFT_PXL),// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CI_MISC_MANTISSA_LABEL,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_MISC_3_5TH,						// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3,
			MX_FPS_MISC_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_9F5A67A7_Mantissa_ ),											// pwcText
			_LEN_9F5A67A7,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_MISC_MANTISSA_EDIT,				// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_MISC_3_5TH + MX_FPS_MISC_LABEL_W,// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST)) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3,
			(MX_FPS_MISC_4_5TH - MX_FPS_MISC_3_5TH) - MX_FPS_MISC_LABEL_W - (MX_LEFT_JUST + MX_GROUP_LEFT_PXL),// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_CI_MISC_BIAS_LABEL,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_MISC_4_5TH,						// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST) + 3) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3,
			MX_FPS_MISC_LABEL_W,					// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_2F7DD938_Biased_Exp_ ),										// pwcText
			_LEN_2F7DD938,							// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_CI_MISC_BIAS_EDIT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_FPS_MISC_4_5TH + MX_FPS_MISC_LABEL_W,// iLeft
			((MX_FPS_MISC_GROUP_TOP + MX_GROUP_TOP_PXL + MX_TOP_JUST)) + (MX_DEF_EDIT_HEIGHT_PXL + 2) * 3,
			(MX_FPS_MISC_5_5TH - MX_FPS_MISC_4_5TH) - MX_FPS_MISC_LABEL_W - (MX_LEFT_JUST + MX_GROUP_LEFT_PXL),// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_CI_MAINWINDOW,						// dwParentId
		},
	};

#undef MX_FPS_H
#undef MX_FPS_MISC_5_5TH
#undef MX_FPS_MISC_4_5TH
#undef MX_FPS_MISC_3_5TH
#undef MX_FPS_MISC_2_5TH
#undef MX_FPS_MISC_1_5TH
#undef MX_FPS_MISC_LABEL_W
#undef MX_FPS_MISC_INTERIOR_W
#undef MX_FPS_MISC_GROUP_H
#undef MX_FPS_MISC_GROUP_TOP
#undef MX_FPS_BIAS_RIGHT
#undef MX_FPS_LIMITS_BIN_W
#undef MX_FPS_LIMITS_BIN_L
#undef MX_FPS_LIMITS_HEX_W
#undef MX_FPS_LIMITS_LABEL_W
#undef MX_FPS_LIMITS_H
#undef MX_FPS_LIMITS_TOP
#undef MX_FPS_PARM_PRE_W
#undef MX_FPS_PARM_MAN_W
#undef MX_FPS_PARM_IMP_W
#undef MX_FPS_PARM_EXP_W
#undef MX_FPS_PARM_SIGN_W
#undef MX_FPS_5_5TH
#undef MX_FPS_4_5TH
#undef MX_FPS_3_5TH
#undef MX_FPS_2_5TH
#undef MX_FPS_1_5TH
#undef MX_FPS_PARM_INTERIOR_W
#undef MX_FPS_INPUT_LABEL_W
#undef MX_FPS_IN_GROUP_TOP
#undef MX_FPS_W

	// == Functions.
	// Creates the window.
	CWidget * CFloatingPointStudioLayout::CreateFloatingPointStudioWindow( CWidget * _pwParent ) {
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		CLayoutManager::UnencryptLayouts( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ),
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		CFloatingPointStudioWindow::PrepareFloatingPointStudio();
		CWidget * pwThis = plmLayout->CreateWindowX( &vLayouts[0], MX_ELEMENTS( m_wlMainWindow ),
			nullptr, 0,
			_pwParent );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );

		return pwThis;
	}

}	// namespace mx

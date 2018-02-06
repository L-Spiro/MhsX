#include "MXExpressionEvaluatorLayout.h"
#include "../Utilities/MXUtilities.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <Layout/LSWLayoutManager.h>


namespace mx {

	// == Members.
#define MX_EXP_W				340
#define MX_EXP_H				490
#define MX_EXP_TEXT_W			57
#define MX_OUTPU_TEXT_W			40
#define MX_T_MARGIN				2
#define MX_RESULT_TOP			(MX_T_MARGIN + MX_DEF_COMBO_HEIGHT_PXL + 2)
	// The layout for the Found Addresses dockable window.
	LSW_WIDGET_LAYOUT CExpressionEvaluatorLayout::m_wlMainWindow[] = {
		{
			MX_EXP_EVAL,							// ltType
			MX_EE_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			654 - 130,								// iLeft
			153,									// iTop
			MX_EXP_W,								// dwWidth
			MX_EXP_H,								// dwHeight
			WS_VISIBLE,								// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_54B37832_Expression_Evaluator ),		// pwcText
			_LEN_54B37832,											// sTextLen
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
			MX_EE_REBAR0,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			m_wlMainWindow[0].dwWidth,				// dwWidth
			150,									// dwHeight
			//WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TBSTYLE_TRANSPARENT | TBSTYLE_REGISTERDROP | TBSTYLE_LIST | TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_NOMOVEY,
			WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN |
				RBS_VARHEIGHT | RBS_BANDBORDERS,		// dwStyle
			0,//WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EE_MAINWINDOW,						// dwParentId

			MX_PARENT_CLEFT,						// pcLeftSizeExp
			MX_PARENT_CRIGHT,						// pcRightSizeExp
			MX_PARENT_CTOP,							// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_TOOLBAR,							// ltType
			MX_EE_TOOLBAR0,							// wId
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
			MX_EE_MAINWINDOW,						// dwParentId
		},
		{
			LSW_LT_LABEL,							// ltType
			MX_EE_EXP_TEXT,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_T_MARGIN + 4,						// iTop
			MX_EXP_TEXT_W,							// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_E54DB928_Expression_ ),																													// pwcText
			_LEN_E54DB928,																																				// sTextLen
			MX_EE_MAINWINDOW,						// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_EE_EXP_COMBO,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_EXP_TEXT_W + 2,						// iLeft
			MX_T_MARGIN,							// iTop
			MX_EXP_W - (MX_EXP_TEXT_W + 2),			// dwWidth
			MX_DEF_COMBO_HEIGHT_PXL,				// dwHeight
			MX_COMBOSTYLE,							// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EE_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_EDIT,							// ltType
			MX_EE_EXP_RESULT,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_RESULT_TOP,							// iTop
			MX_EXP_W,								// dwWidth
			MX_DEF_EDIT_HEIGHT_PXL,					// dwHeight
			MX_EDITSTYLE | ES_READONLY,				// dwStyle
			WS_EX_CLIENTEDGE,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EE_MAINWINDOW,						// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},

		{
			LSW_LT_LABEL,							// ltType
			MX_EE_EXP_OUTPUTTEXT,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_RESULT_TOP + MX_DEF_EDIT_HEIGHT_PXL + 2 + 4,	// iTop
			MX_OUTPU_TEXT_W,						// dwWidth
			MX_DEF_STATIC_HEIGHT_PXL,				// dwHeight
			MX_STATICSTYLE,							// dwStyle
			0,										// dwStyleEx
			MX_MAKE_WCHAR( _T_CCC86FF4_Output_ ),	// pwcText
			_LEN_CCC86FF4,							// sTextLen
			MX_EE_MAINWINDOW,						// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_COMBOBOX,						// ltType
			MX_EE_EXP_OUTPUTCOMBO,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OUTPU_TEXT_W + 2,						// iLeft
			MX_RESULT_TOP + MX_DEF_EDIT_HEIGHT_PXL + 2,	// iTop
			MX_EXP_W - (MX_OUTPU_TEXT_W + 2),			// dwWidth
			MX_DEF_COMBO_HEIGHT_PXL,					// dwHeight
			MX_COMBOSTYLE_LIST,						// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EE_MAINWINDOW,						// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			nullptr, 0,								// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_LISTVIEW,						// ltType
			MX_EE_LIST,								// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			MX_RESULT_TOP + MX_DEF_EDIT_HEIGHT_PXL + 2 + MX_DEF_COMBO_HEIGHT_PXL + 2,	// iTop
			0,										// dwWidth
			0,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_OWNERDATA | LVS_NOSORTHEADER | WS_TABSTOP,		// dwStyle
			WS_EX_CLIENTEDGE | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,																								// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_EE_MAINWINDOW,						// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_TOP_VCOFFSET,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
	};
#undef MX_RESULT_TOP
#undef MX_EXP_TEXT_W
#undef MX_EXP_H
#undef MX_EXP_W


	// == Functions.
	// Creates the Expression Evaluator dockable window.
	CWidget * CExpressionEvaluatorLayout::CreateExpEvalWindow( CWidget * _pwParent ) {
		std::vector<std::string> sStrings;
		std::vector<std::wstring> sStringsW;
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

}	// namespace mx

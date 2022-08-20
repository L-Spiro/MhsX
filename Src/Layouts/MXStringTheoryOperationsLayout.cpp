#include "MXStringTheoryOperationsLayout.h"
#include "../System/MXSystem.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>

namespace mx {

#define MX_OPTIONS_W								(260 + 0)
#define MX_OPTIONS_H								(183 + 0)
#define MX_OPTIONS_LIST_W							(150)
#define MX_PADDING									4
#define MX_THIN_PADDING								2
#define MX_OD_L										(MX_OPTIONS_LIST_W + 5)
#define MX_OD_T										(2)
#define MX_OD_W										(MX_OPTIONS_W - MX_OD_L - MX_PADDING)
#define MX_OD_H										(MX_OPTIONS_H - 26)
#define MX_OD_COORD									MX_OD_L, MX_OD_T, MX_OD_W, MX_OD_H


	// == Members.
	// The layout for the Options dialog.
	LSW_WIDGET_LAYOUT CStringTheoryOperationsLayout::m_wlOperationsDialog[] = {
		{
			MX_STRING_THEORY_OPERATIONS,			// ltType
			MX_OI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_OPTIONS_W,							// dwWidth
			MX_OPTIONS_H,							// dwHeight
			//WS_CAPTION | WS_POPUP | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_CAPTION | WS_POPUP | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | WS_THICKFRAME | DS_3DLOOK | DS_FIXEDSYS | DS_CENTER,
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_FDE4562A_String_Operations ),																								// pwcText
			_LEN_FDE4562A,							// sTextLen
			MX_OI_NONE,								// dwParentId
		},
		{
			LSW_LT_LISTBOX,							// ltType
			MX_OI_LIST,								// wId
			WC_LISTBOXW,							// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			2,										// iLeft
			MX_OD_T,								// iTop
			MX_OPTIONS_LIST_W,						// dwWidth
			MX_OD_H,								// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,		// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_OK,								// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			MX_OPTIONS_W - MX_DEF_BUTTON_WIDTH * 2 - 2 - 2,	// iLeft
			MX_OPTIONS_H - MX_DEF_BUTTON_HEIGHT - 2,		// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_DEFBUTTONSTYLE,						// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),																	// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_OI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OI_CANCEL,							// wId
			WC_BUTTONW,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			MX_OPTIONS_W - MX_DEF_BUTTON_WIDTH * 1 - 2,		// iLeft
			MX_OPTIONS_H - MX_DEF_BUTTON_HEIGHT - 2,		// iTop
			MX_DEF_BUTTON_WIDTH,					// dwWidth
			MX_DEF_BUTTON_HEIGHT,					// dwHeight
			MX_BUTTONSTYLE,							// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),																// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_OI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
	};

	// String-replace.
	LSW_WIDGET_LAYOUT CStringTheoryOperationsLayout::m_wlOperationsReplace[] = {
		{
			MX_STRING_THEORY_OPERATIONS,			// ltType
			MX_OI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_OPTIONS_W,							// dwWidth
			MX_OPTIONS_H,							// dwHeight
			WS_CAPTION | WS_POPUP | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_FDE4562A_String_Operations ),																								// pwcText
			_LEN_FDE4562A,							// sTextLen
			MX_OI_NONE,								// dwParentId
		},
	};

	// == Functions.
	// Creates the Options dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	BOOL CStringTheoryOperationsLayout::CreateOperationsDialog( CWidget * _pwParent, MX_OPTIONS * _poOptions, int32_t _i32Page ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlOperationsDialog, MX_ELEMENTS( m_wlOperationsDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		/*COptionsWindow::MX_PARMS pParms = {
			_poOptions,
			_i32Page
		};*/

		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], MX_ELEMENTS( m_wlOperationsDialog ), _pwParent, static_cast<uint64_t>(0) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

	// Creates the general options page.
	CWidget * CStringTheoryOperationsLayout::CreateStringReplacePage( CWidget * _pwParent, MX_OPTIONS * _poOptions ) {
		return CreatePage( _pwParent, m_wlOperationsReplace, MX_ELEMENTS( m_wlOperationsReplace ), _poOptions );
	}

	// Default window-creation.
	CWidget * CStringTheoryOperationsLayout::CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, MX_OPTIONS * _poOptions ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( _pwlLayout, _sTotal,
			vLayouts,
			sStringsW,
			sStrings );
		
		CWidget * pwWidget = lsw::CBase::LayoutManager()->CreateDialogX( &vLayouts[0], _sTotal, _pwParent, reinterpret_cast<uint64_t>(_poOptions) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( pwWidget ) {
			
			// Success.  Do stuff.
		}

		return pwWidget;
	}

}	// namespace mx

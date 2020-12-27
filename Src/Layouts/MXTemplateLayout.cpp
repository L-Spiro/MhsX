#include "MXTemplateLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>

namespace mx {

#define MX_FPS_W				1460
#define MX_FPS_H				940
	// == Members.
	// The layout for the window.
	LSW_WIDGET_LAYOUT CTemplateLayout::m_wlMainWindow[] = {
		{
			MX_PE_WORKS,							// ltType
			MX_C_MAINWINDOW,						// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			35,										// iLeft
			25,										// iTop
			MX_FPS_W,								// dwWidth
			MX_FPS_H,								// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | DS_3DLOOK | DS_SETFONT,				// dwStyle
			WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,														// dwStyleEx
			MX_MAKE_WCHAR( _T_D09DA91D_PE_Works ),	// pwcText
			_LEN_D09DA91D,							// sTextLen
			MX_C_NONE,								// dwParentId
			/*
			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
			*/
		},
	};

#undef MX_FPS_W
#undef MX_FPS_H

	// == Functions.
	// Creates the Template window.
	CWidget * CTemplateLayout::CreateTemplateWindow( CWidget * _pwParent ) {
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		CLayoutManager::UnencryptLayouts( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ),
			vLayouts,
			sStringsW,
			sStrings );

		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		CWidget * pwThis = plmLayout->CreateWindowX( &vLayouts[0], MX_ELEMENTS( m_wlMainWindow ),
			nullptr, 0,
			_pwParent );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );

		return pwThis;
	}

}	// namespace mx

#include "MXFoundAddressEditLayout.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>

namespace mx {

#define MX_EDIT_W							550
#define MX_EDIT_H							550

	// == Members.
	// The layout for the edit dialog.
	LSW_WIDGET_LAYOUT CFoundAddressEditLayout::m_wlMainDialog[] = {
		{
			MX_OPTIONS_WINDOW,						// ltType
			MX_FAEI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			MX_EDIT_W,								// dwWidth
			MX_EDIT_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_7AE3FDE1_Modify_Entry ),																									// pwcText
			_LEN_7AE3FDE1,							// sTextLen
			MX_FAEI_NONE,							// dwParentId
		},
	};

}	// namespace mx

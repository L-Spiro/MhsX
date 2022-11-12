#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CToolTip : public CWidget {
	public :
		CToolTip( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_TOOLTIP; }


	protected :
		// == Members.
		// The tooltip text.
		std::wstring						m_wsTooltipText;


		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

	};

}	// namespace lsw

#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CChildWindow : public CWidget {
	public :
		CChildWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			lsw::CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		}

	};

}	// namespace lsw

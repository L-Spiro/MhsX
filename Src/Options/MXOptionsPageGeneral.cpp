#include "MXOptionsPageGeneral.h"

namespace mx {

	COptionsPageGeneral::COptionsPageGeneral( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		COptionsPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	/*CWidget::LSW_HANDLED COptionsPage::InitDialog() {
		CWidget * pwWidget = COptionsLayout::CreateGeneralPage( this );
		return LSW_H_CONTINUE;
	}*/

}	// namespace mx

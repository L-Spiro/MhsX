#include "LSWGroupBox.h"

namespace lsw {

	CGroupBox::CGroupBox( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
	}

}	// namespace lsw

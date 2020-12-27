#include "LSWButton.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CButton::CButton( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

}	// namespace lsw

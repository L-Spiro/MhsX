#include "LSWUpDown.h"


namespace lsw {

	CUpDown::CUpDown( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	// == Functions.
	// Setting the HWND after the control has been created.
	void CUpDown::InitControl( HWND _hWnd ) {
	}

}	// namespace lsw

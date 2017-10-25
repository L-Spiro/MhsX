#include "LSWDockable.h"

namespace lsw {

	CDockable::CDockable( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
	}

	// == Functions.
	// The dockable message handler.
	LRESULT CALLBACK CDockable::WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		switch ( _uMsg ) {
			case WM_NCACTIVATE : {
				return ::DefWindowProcW( _hWnd, _uMsg, TRUE, _lParam );
			}
		}
		return CWidget::WindowProc( _hWnd, _uMsg, _wParam, _lParam );
	}

}	// namespace lsw

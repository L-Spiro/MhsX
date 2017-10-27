#include "LSWDockable.h"

namespace lsw {

	CDockable::CDockable( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
		if ( Parent() ) {
			Parent()->AddDockable( this );
		}
	}
	CDockable::~CDockable() {
		if ( Parent() ) {
			Parent()->RemDockable( this );
		}
	}

	// == Functions.
	// The dockable message handler.
	LRESULT CALLBACK CDockable::WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CWidget * pmwThis = reinterpret_cast<CWidget *>(::GetWindowLongPtrW( _hWnd, GWLP_USERDATA ));
		switch ( _uMsg ) {
			case WM_NCACTIVATE : {
				if ( pmwThis ) {
					if ( pmwThis->Parent() ) {
						return pmwThis->Parent()->DockNcActivate( pmwThis, _wParam, _lParam, TRUE );
					}
				}
				break;
			}
			case WM_ENABLE : {
				if ( pmwThis ) {
					if ( pmwThis->Parent() ) {
						return pmwThis->Parent()->DockEnable( pmwThis, _wParam, _lParam, TRUE );
					}
				}
				break;
			}
			/*case WM_ACTIVATE : {
				if ( pmwThis ) {
					if ( pmwThis->Parent() ) {
						return pmwThis->Parent()->DockActivate( pmwThis, _wParam, _lParam, TRUE );
					}
				}
				break;
			}*/
			/*case WM_NCACTIVATE : {
				return CWidget::WindowProc( _hWnd, _uMsg, TRUE, _lParam );
			}*/
		}
		return CWidget::WindowProc( _hWnd, _uMsg, _wParam, _lParam );
	}

}	// namespace lsw

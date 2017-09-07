#include "LSWWidget.h"

namespace lsw {

	CWidget::CWidget() :
		m_hWnd( NULL ) {
	}
	CWidget::~CWidget() {
	}

	
	// == Functions.
	// The default message handler.
	LRESULT CALLBACK CWidget::WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CWidget * pmwThis = reinterpret_cast<CWidget *>(::GetWindowLongPtrW( _hWnd, GWLP_USERDATA ));
		switch ( _uMsg ) {
			// =======================================
			// Create/Destroy.
			// =======================================
			case WM_NCCREATE : {
				CREATESTRUCTW * pcsCreate = reinterpret_cast<CREATESTRUCTW *>(_lParam);
				::SetWindowLongPtrW( _hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pcsCreate->lpCreateParams) );
				pmwThis = reinterpret_cast<CWidget *>(pcsCreate->lpCreateParams);
				pmwThis->m_hWnd = _hWnd;
				LSW_HANDLED hHandled = pmwThis->NcCreate( (*pcsCreate) );
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
			case WM_CREATE : {
				CREATESTRUCTW * pcsCreate = reinterpret_cast<CREATESTRUCTW *>(_lParam);
				LSW_HANDLED hHandled = pmwThis->Create( (*pcsCreate) );
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
			case WM_DESTROY : {
				LSW_HANDLED hHandled = pmwThis->Destroy();
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
			case WM_NCDESTROY : {
				LSW_HANDLED hHandled = pmwThis->NcDestroy();
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
			case WM_CLOSE : {
				LSW_HANDLED hHandled = pmwThis->Close();
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}

			// =======================================
			// Sizing.
			// =======================================
			case WM_SIZE : {
				LSW_HANDLED hHandled;
				switch ( _wParam ) {
					case SIZE_MINIMIZED : {
						// Width and height are the sizes in the task bar.
						hHandled = pmwThis->Minimized( LOWORD( _lParam ), HIWORD( _lParam ) );
						break;
					}
					default : {
						::GetWindowRect( _hWnd, &pmwThis->m_rRect );
						::GetClientRect( _hWnd, &pmwThis->m_rClientRect );
						hHandled = pmwThis->Size( _wParam, pmwThis->m_rRect.Width(), pmwThis->m_rRect.Height() );
					}
				}
				
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
			case WM_MOVE : {
				if ( !::IsIconic( _hWnd ) ) {
					::GetWindowRect( _hWnd, &pmwThis->m_rRect );
					::GetClientRect( _hWnd, &pmwThis->m_rClientRect );
					POINTS pPoint = MAKEPOINTS( _lParam );
					LSW_HANDLED hHandled = pmwThis->Move( pPoint.x, pPoint.y );
					if ( hHandled == LSW_H_HANDLED ) { return 0; }
				}
				break;
			}
		}
		return ::DefWindowProcW( _hWnd, _uMsg, _wParam, _lParam );
	}

	// == Message Handlers.
	// WM_NCCREATE.
	CWidget::LSW_HANDLED CWidget::NcCreate( const CREATESTRUCTW &_csCreateParms ) {
		return LSW_H_CONTINUE;
	}

	// WM_CREATE.
	CWidget::LSW_HANDLED CWidget::Create( const CREATESTRUCTW &_csCreateParms ) {
		return LSW_H_CONTINUE;
	}

	// WM_DESTROY.
	CWidget::LSW_HANDLED CWidget::Destroy() {
		return LSW_H_CONTINUE;
	}

	// WM_NCDESTROY.
	CWidget::LSW_HANDLED CWidget::NcDestroy() {
		return LSW_H_CONTINUE;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CWidget::Close() {
		return LSW_H_CONTINUE;
	}

	// WM_SIZE.
	CWidget::LSW_HANDLED CWidget::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		return LSW_H_CONTINUE;
	}

	// WM_SIZE, SIZE_MINIMIZED.
	CWidget::LSW_HANDLED CWidget::Minimized( LONG _lWidth, LONG _lHeight ) {
		return LSW_H_CONTINUE;
	}

	// WM_MOVE.
	CWidget::LSW_HANDLED CWidget::Move( LONG _lX, LONG _lY ) {
		return LSW_H_CONTINUE;
	}

}	// namespace lsw

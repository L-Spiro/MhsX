#include "LSWWidget.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CWidget::CWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget ) :
		m_hWnd( NULL ),
		m_dwId( _wlLayout.dwId ),
		m_pwParent( _pwParent ) {

		m_rStartingRect.left = _wlLayout.iLeft;
		m_rStartingRect.top = _wlLayout.iTop;
		m_rStartingRect.SetWidth( _wlLayout.dwWidth );
		m_rStartingRect.SetHeight( _wlLayout.dwHeight );

		if ( _bCreateWidget ) {
			m_hWnd = ::CreateWindowExW( 0,
				_wlLayout.lpwcClass,
				_wlLayout.pwcText,
				_wlLayout.dwStyle,
				_wlLayout.iLeft, _wlLayout.iTop,
				_wlLayout.dwWidth, _wlLayout.dwHeight,
				_pwParent ? _pwParent->Wnd() : NULL,
				(_wlLayout.dwStyle & WS_CHILD) ? reinterpret_cast<HMENU>(_wlLayout.dwId) : NULL,
				CBase::GetThisHandle(),
				static_cast<CWidget *>(this) );
		}

		if ( _pwParent != nullptr ) {
			_pwParent->AddChild( this );
		}
		else {
			m_pwParent = nullptr;
		}
	}
	CWidget::~CWidget() {
		if ( m_pwParent ) {
			m_pwParent->RemoveChild( this );
		}
		for ( size_t I = 0; I < m_vChildren.size(); ++I ) {
			CWidget * pwTemp = m_vChildren[I];
			m_vChildren[I] = nullptr;
			delete m_vChildren[I];			
		}
		m_vChildren.clear();
		
		if ( m_hWnd ) {
			::DestroyWindow( m_hWnd );
			m_hWnd = NULL;
		}
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

			// =======================================
			// Commands
			// =======================================
			case WM_COMMAND : {
				LSW_HANDLED hHandled;
				WORD wId = LOWORD( _wParam );
				if ( _lParam ) {
					// Sent by a control.
					hHandled = pmwThis->Command( wId, reinterpret_cast<HWND>(_lParam) );
				}
				else {
					// Sent by a menu or accelerator.
					switch ( HIWORD( _wParam ) ) {
						case 0 : {
							hHandled = pmwThis->MenuCommand( wId );
							break;
						}
						case 1 : {
							hHandled = pmwThis->AcceleratorCommand( wId );
							break;
						}
						default : {
							// Error.
						}
					}
				}
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
		}
		return ::DefWindowProcW( _hWnd, _uMsg, _wParam, _lParam );
	}

	// The default dialog message handler.
	INT_PTR CALLBACK CWidget::DialogProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CWidget * pmwThis = reinterpret_cast<CWidget *>(::GetWindowLongPtrW( _hWnd, DWLP_USER ));

		switch ( _uMsg ) {
			// =======================================
			// Create/Destroy.
			// =======================================
			case WM_INITDIALOG : {
				std::vector<CWidget *> * pvWidgets = reinterpret_cast<std::vector<CWidget *> *>(_lParam);
				pmwThis = (*pvWidgets)[0];
				::SetWindowLongPtrW( _hWnd, DWLP_USER, reinterpret_cast<LONG_PTR>(pmwThis) );
				pmwThis->m_hWnd = _hWnd;

				//SetWindowTheme(hwnd, L" ", L" ");


				// Attach all controls to their CWidget counterparts.
				::EnumChildWindows( _hWnd, EnumChildWindows_AttachWindowToWidget, _lParam );
				POINT pConvOrg = PixelsToDialogUnits( _hWnd, 628 - 605, 550 - 496 - 30 );
				POINT pConv = PixelsToDialogUnits( _hWnd, 443, 206 );
				POINT pConvClient = PixelsToDialogUnits( _hWnd, 422, 185 );
				
				/*NONCLIENTMETRICSW ncmMetrics;
				ncmMetrics.cbSize = sizeof( ncmMetrics );
				::SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, ncmMetrics.cbSize, &ncmMetrics, 0 );
				HFONT hFont = ::CreateFontIndirectW( &ncmMetrics.lfMessageFont );*/


				// ::SetWindowLong( _hWnd, DWL_MSGRESULT, lResult );
				//FONT 8, "MS Shell Dlg", 400, 0, 0x1
				//SendMessage( _hWnd, WM_SETFONT,  (WPARAM)GetStockObject(DEFAULT_GUI_FONT), true );
				//EnumChildWindows(_hWnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(SYSTEM_FIXED_FONT));
				//EnumChildWindows(_hWnd, (WNDENUMPROC)SetFont, (LPARAM)hFont);
				return TRUE;	// Return TRUE to pass focus on to the control specified by _wParam.
			}
			case WM_CLOSE : {
				LSW_HANDLED hHandled = pmwThis->Close();
				if ( hHandled == LSW_H_HANDLED ) { return FALSE; }
				break;
			}
			case WM_SETFONT : {
				/*HFONT hFont = (HFONT)_wParam;
				::EnumChildWindows( _hWnd, EnumChildWindows_SetFont, (LPARAM)hFont );*/
				return TRUE;
			}
		}
		return FALSE;
	}

	// Do we have a given child widget?
	bool CWidget::HasChild( const CWidget * _pwChild ) const {
		for ( size_t I = m_vChildren.size(); I--; ) {
			if ( m_vChildren[I] == _pwChild ) {
				return true;
			}
		}
		return false;
	}

	// Sets a given font on all children of a window.
	BOOL CALLBACK CWidget::EnumChildWindows_SetFont( HWND _hWnd, LPARAM _lParam ) {
		::SendMessageW( _hWnd, WM_SETFONT, static_cast<WPARAM>(_lParam), TRUE );
		return TRUE;
	}

	// Converts a point from pixels to dialog units.
	POINT CWidget::PixelsToDialogUnits( HWND _hWnd, LONG _lX, LONG _lY ) {
		POINT pRet;
		for ( LONG I = 0; I < 5000; ++I ) {
			RECT rX;
			rX.left = I;
			::MapDialogRect( _hWnd, &rX );
			if ( rX.left >= _lX ) {
				pRet.x = I;
				break;
			}
		}
		for ( LONG I = 0; I < 5000; ++I ) {
			RECT rY;
			rY.top = I;
			::MapDialogRect( _hWnd, &rY );
			if ( rY.top >= _lY ) {
				pRet.y = I;
				break;
			}
		}
		return pRet;
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

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CWidget::Command( WORD _Id, HWND _hControl ) {
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from menu.
	CWidget::LSW_HANDLED CWidget::MenuCommand( WORD _Id ) {
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from accelerator.
	CWidget::LSW_HANDLED CWidget::AcceleratorCommand( WORD _Id ) {
		return LSW_H_CONTINUE;
	}

	// == Functions.
	// Remove a child.
	void CWidget::RemoveChild( const CWidget * _pwChild ) {
		for ( size_t I = m_vChildren.size(); I--; ) {
			if ( m_vChildren[I] == _pwChild ) {
				m_vChildren.erase( m_vChildren.begin() + I );
			}
		}
	}

	// Add a chld.
	void CWidget::AddChild( CWidget * _pwChild ) {
		if ( !HasChild( _pwChild ) ) {
			m_vChildren.push_back( _pwChild );
		}
	}

	// Set the parent.
	void CWidget::SetParent( CWidget * _pwParent ) {
		if ( m_pwParent ) {
			m_pwParent->RemoveChild( this );
		}
		m_pwParent = _pwParent;
		if ( m_pwParent ) {
			m_pwParent->AddChild( this );
		}
	}

	// Attaches a control/window to its CWidget.
	BOOL CALLBACK CWidget::EnumChildWindows_AttachWindowToWidget( HWND _hWnd, LPARAM _lParam ) {
		std::vector<CWidget *> * pvWidgets = reinterpret_cast<std::vector<CWidget *> *>(_lParam);
		int iId = ::GetDlgCtrlID( _hWnd );
		for ( size_t I = 0; I < pvWidgets->size(); ++I ) {
			if ( (*pvWidgets)[I]->Id() == iId ) {
				(*pvWidgets)[I]->m_hWnd = _hWnd;
				::SetWindowLongPtrW( _hWnd, DWLP_USER, reinterpret_cast<LONG_PTR>((*pvWidgets)[I]) );
			}
		}
		return TRUE;
	}

}	// namespace lsw

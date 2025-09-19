#include "LSWColorButton.h"
#include "../Base/LSWBase.h"
#include "../Brush/LSWBrush.h"

#include <strsafe.h>
#include <windowsx.h>

namespace lsw {

	CColorButton::CColorButton( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}
	CColorButton::~CColorButton() {
		return;
	}

	// == Functions.
	// Setting the HWND after the control has been created.
	void CColorButton::InitControl( HWND _hWnd ) {
		Parent::InitControl( _hWnd );

		m_hFont = reinterpret_cast<HFONT>(::SendMessageW( _hWnd, WM_GETFONT, 0L, 0L ));

		constexpr const UINT_PTR k_uSubclassId = 0xC0105BEEU;
		if ( ::SetWindowSubclass( _hWnd, ColorButtonSubclassProc, k_uSubclassId, reinterpret_cast<DWORD_PTR>(this) ) ) {
			std::memset( m_wsText, 0, sizeof( m_wsText ) );
			m_crColor = RGB( 20, 20, 20 );
		}
	}

	/**
	 * \brief Draws the colored button face, border, text, and focus cues.
	 *
	 * \param _hDcPaint The HDC obtained during painting.
	 * \param _rcClient Client rectangle of the button.
	 */
	inline void CColorButton::DrawColorButton( HDC _hDcPaint, const RECT &_rcClient ) {
		const COLORREF crFace = m_crColor;
		const COLORREF crText = CHelpers::ContrastingTextColor( crFace );

		//{
		//	LSW_SELECTOBJECT soPenOuter( _hDcPaint, reinterpret_cast<HGDIOBJ>(::CreatePen( PS_SOLID, 1, RGB( 60, 60, 60 ) )), true );	// Destructor sets the original pen back and deletes the new one.
		//	::Rectangle( _hDcPaint, _rcClient.left, _rcClient.top, _rcClient.right, _rcClient.bottom );

		//	LSW_SELECTOBJECT soPenInner( _hDcPaint, reinterpret_cast<HGDIOBJ>(::CreatePen( PS_SOLID, 1, m_bPressed ? RGB( 20, 20, 20 ) : RGB( 200, 200, 200 ) )), true );	// Destructor sets the original pen back and deletes the new one.
		//	::Rectangle( _hDcPaint, _rcClient.left + 1, _rcClient.top + 1, _rcClient.right - 1, _rcClient.bottom - 1 );
		//}
		{
			CBrush bBrush;
			bBrush.CreateSolidBrush( crFace );
			::FillRect( _hDcPaint, &_rcClient, bBrush.Handle() );
		}
		

		LSW_SELECTOBJECT soFontOrig( _hDcPaint, m_hFont );																				// Destructor sets the original font back.
		int iOldBkMode = ::SetBkMode( _hDcPaint, TRANSPARENT );
		COLORREF crOldText = ::SetTextColor( _hDcPaint, crText );
		RECT rcText = _rcClient;
		rcText.left += 6; rcText.right -= 6; rcText.top += 2; rcText.bottom -= 2;
		::DrawTextW( _hDcPaint, m_wsText, -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );
		::SetTextColor( _hDcPaint, crOldText );
		::SetBkMode( _hDcPaint, iOldBkMode );

		if ( m_bFocus && !m_bPressed ) {
			RECT rcFocus = _rcClient;
			::InflateRect( &rcFocus, -4, -4 );
			::DrawFocusRect( _hDcPaint, &rcFocus );
		}
		/*if ( m_bFocus && !m_bPressed ) {
			RECT rcFocus = _rcClient;
			::InflateRect( &rcFocus, -4, -4 );
			::DrawFocusRect( _hDcPaint, &rcFocus );
		}*/
	}

	/**
	 * \brief Opens the system ChooseColor dialog and updates the given color on success.
	 *
	 * \param _hOwner Owner window for the modal dialog.
	 * \param _crIo In/out color. Supplies initial color; receives the chosen color when TRUE is returned.
	 * \return Returns TRUE if the user selected a color; otherwise FALSE.
	 */
	inline BOOL CColorButton::DoPickColor( COLORREF &_crIo ) {
		if ( !Wnd() ) { return FALSE; }

		auto hWnd = ::GetParent( Wnd() ) ? ::GetParent( Wnd() ) : Wnd();
		CHOOSECOLORW ccPick{};
		ccPick.lStructSize = sizeof( ccPick );
		ccPick.hwndOwner = hWnd;
		ccPick.rgbResult = _crIo;
		ccPick.lpCustColors = m_acrCust;
		ccPick.Flags = CC_FULLOPEN | CC_RGBINIT;
		if ( ::ChooseColorW( &ccPick ) ) {
			_crIo = ccPick.rgbResult;
			m_bColorWasSelected = TRUE;
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	/**
	 * \brief Subclass procedure for the color-picker button.
	 *
	 * \param _hWnd The subclassed button HWND.
	 * \param _uiMsg The message identifier.
	 * \param _wParam The WPARAM for the message.
	 * \param _lParam The LPARAM for the message.
	 * \param _uId Subclass ID used when installing the subclass.
	 * \param _dwRef Reference data passed to SetWindowSubclass (holds SColorButtonState *).
	 * \return Returns an LRESULT per message semantics.
	 */
	LRESULT CALLBACK CColorButton::ColorButtonSubclassProc( HWND _hWnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uId, DWORD_PTR _dwRef ) {
		//SColorButtonState * pmwThis = reinterpret_cast<SColorButtonState *>(_dwRef);
//		CColorButton * pmwThis = static_cast<CColorButton *>(LSW_WIN2CLASS( _hWnd ));
		CColorButton * pmwThis = reinterpret_cast<CColorButton *>(_dwRef);

		switch ( _uiMsg ) {
			case WM_NCDESTROY : {
				::RemoveWindowSubclass( _hWnd, ColorButtonSubclassProc, _uId );
				//::SetWindowSubclass( _hWnd, CWidget::WindowProc, _uId, _dwRef );
				////delete pmwThis;
				//return ::DefSubclassProc( _hWnd, _uiMsg, _wParam, _lParam );
				
				return CWidget::WindowProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
			case WM_SETTEXT : {
				/*LRESULT lrResult = */::DefSubclassProc( _hWnd, _uiMsg, _wParam, _lParam );
				::GetWindowTextW( _hWnd, pmwThis->m_wsText, int( sizeof( pmwThis->m_wsText ) / sizeof( pmwThis->m_wsText[0] ) ) );
				::InvalidateRect( _hWnd, nullptr, TRUE );
				//return lrResult;
				return CWidget::WindowProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
			case WM_SETFONT : {
				pmwThis->m_hFont = reinterpret_cast<HFONT>(_wParam);
				/*LRESULT lrResult = */::DefSubclassProc( _hWnd, _uiMsg, _wParam, _lParam );
				if ( _lParam ) { ::InvalidateRect( _hWnd, nullptr, TRUE ); }
				//return lrResult;
				return CWidget::WindowProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
			case WM_ENABLE : {
				::InvalidateRect( _hWnd, nullptr, TRUE );
				//return ::DefSubclassProc( _hWnd, _uiMsg, _wParam, _lParam );
				return CWidget::WindowProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
			case WM_SETFOCUS : {
				pmwThis->m_bFocus = TRUE;
				::InvalidateRect( _hWnd, nullptr, TRUE );
				//return ::DefSubclassProc( _hWnd, _uiMsg, _wParam, _lParam );
				return CWidget::WindowProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
			case WM_KILLFOCUS : {
				pmwThis->m_bFocus = FALSE;
				::InvalidateRect( _hWnd, nullptr, TRUE );
				//return ::DefSubclassProc( _hWnd, _uiMsg, _wParam, _lParam );
				return CWidget::WindowProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
			case WM_MOUSEMOVE : {
				if ( !pmwThis->m_bHot ) {
					pmwThis->m_bHot = TRUE;
					TRACKMOUSEEVENT tme{ sizeof( TRACKMOUSEEVENT ), TME_LEAVE, _hWnd, 0 };
					::TrackMouseEvent( &tme );
					::InvalidateRect( _hWnd, nullptr, FALSE );
				}
				//return ::DefSubclassProc( _hWnd, _uiMsg, _wParam, _lParam );
				return CWidget::WindowProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
			case WM_MOUSELEAVE : {
				pmwThis->m_bHot = FALSE;
				::InvalidateRect( _hWnd, nullptr, FALSE );
				//return 0;
				return CWidget::WindowProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
			case WM_LBUTTONDOWN : {
				pmwThis->m_bPressed = TRUE;
				::SetCapture( _hWnd );
				::InvalidateRect( _hWnd, nullptr, FALSE );
				//return 0;
				return CWidget::WindowProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
			case WM_LBUTTONUP : {
				if ( pmwThis->m_bPressed ) {
					pmwThis->m_bPressed = FALSE;
					::ReleaseCapture();
					::InvalidateRect( _hWnd, nullptr, FALSE );
					POINT ptClient{ GET_X_LPARAM( _lParam ), GET_Y_LPARAM( _lParam ) };
					RECT rcClient{}; ::GetClientRect( _hWnd, &rcClient );
					if ( ::PtInRect( &rcClient, ptClient ) ) {
						COLORREF crOld = pmwThis->m_crColor;
						if ( pmwThis->DoPickColor( pmwThis->m_crColor ) ) {
							if ( pmwThis->m_crColor != crOld ) {
								::InvalidateRect( _hWnd, nullptr, TRUE );
								HWND hParent = ::GetParent( _hWnd );
								if ( hParent ) {
									UINT uiId = UINT(::GetDlgCtrlID( _hWnd ));
									::SendMessageW( hParent, WM_COMMAND, MAKEWPARAM( uiId, LSW_BN_COLOR_CHANGED ), reinterpret_cast<LPARAM>(_hWnd) );
								}
							}
						}
					}
				}
				//return 0;
				return CWidget::WindowProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
			case WM_KEYDOWN : {
				if ( _wParam == VK_SPACE || _wParam == VK_RETURN ) {
					COLORREF crOld = pmwThis->m_crColor;
					if ( pmwThis->DoPickColor( pmwThis->m_crColor ) ) {
						if ( pmwThis->m_crColor != crOld ) {
							::InvalidateRect( _hWnd, nullptr, TRUE );
							HWND hParent = ::GetParent( _hWnd );
							if ( hParent ) {
								UINT uiId = (UINT)::GetDlgCtrlID( _hWnd );
								::SendMessageW( hParent, WM_COMMAND, MAKEWPARAM( uiId, LSW_BN_COLOR_CHANGED ), reinterpret_cast<LPARAM>(_hWnd) );
							}
						}
					}
					return 0;
				}
				//return ::DefSubclassProc( _hWnd, _uiMsg, _wParam, _lParam );
				return CWidget::WindowProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
			case WM_ERASEBKGND : {
				return 1;
			}
			case WM_PAINT : {
				LSW_BEGINPAINT bpPaint( _hWnd );
				RECT rcClient{}; ::GetClientRect( _hWnd, &rcClient );
				if ( pmwThis->m_wsText[0] == L'\0' ) {
					::GetWindowTextW( _hWnd, pmwThis->m_wsText, int( sizeof( pmwThis->m_wsText ) / sizeof( pmwThis->m_wsText[0] ) ) );
				}
				pmwThis->DrawColorButton( bpPaint.hDc, rcClient );
				return 0;
			}
			default : {
				return ::DefSubclassProc( _hWnd, _uiMsg, _wParam, _lParam );
			}
		}
	}

}	// namespace lsw

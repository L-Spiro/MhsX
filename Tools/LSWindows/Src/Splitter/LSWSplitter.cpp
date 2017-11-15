#include "LSWSplitter.h"
#include "../Base/LSWBase.h"
#include "../Brush/LSWBrush.h"
#include "../Images/LSWBitmap.h"

namespace lsw {

	CSplitter::CSplitter( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		CWidget( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(CBase::SplitterAtom()) ), _pwParent, _bCreateWidget, _hMenu ),
		m_bDragging( FALSE ),
		m_dwBarWidth( 4 ),
		m_dwSplitterType( LSW_SS_HORIZONTAL ) {
		
	}
	CSplitter::~CSplitter() {
	}

	// == Functions.
	// Draw the XOR bar.
	void CSplitter::DrawXorBar( HDC _hDc, INT _iX1, INT _iY1, INT _iWidth, INT _iHeight ) {
		static WORD wDotPattern[8] = {
			0x00AA, 0x0055, 0x00AA, 0x0055,
			0x00AA, 0x0055, 0x00AA, 0x0055
		};

		CBitmap bBitmap;
		bBitmap.CreateBitmap( 8, 8, 1, 1, wDotPattern );
		CBrush bBrush;
		bBrush.CreatePatternBrush( bBitmap );
		::SetBrushOrgEx( _hDc, _iX1, _iY1, NULL );

		LSW_SELECTOBJECT soBrushOrig( _hDc, bBrush.Handle() );	// Destructor sets the original brush back.

		::PatBlt( _hDc, _iX1, _iY1, _iWidth, _iHeight, PATINVERT );
	}

	// Draw the bar for this control.
	void CSplitter::DrawBar( POINT &_pPoint, RECT _rClient, BOOL _bEnforceNew ) {
		HDC hDc = ::GetDC( Wnd() );
		//HDC hDc = ::GetWindowDC( Wnd() );

		INT iLinesDrawn = 0;
		if ( m_dwSplitterType & LSW_SS_VERTICAL ) {
			if ( !_bEnforceNew || _pPoint.x != m_pLastPoint.x ) {
				DrawXorBar( hDc, _pPoint.x, _rClient.top, m_dwBarWidth, _rClient.bottom );
				iLinesDrawn++;
			}
		}
		if ( m_dwSplitterType & LSW_SS_HORIZONTAL ) {
			if ( !_bEnforceNew || _pPoint.y != m_pLastPoint.y ) {
				DrawXorBar( hDc, _rClient.left, _pPoint.y, _rClient.right, m_dwBarWidth );
				iLinesDrawn++;
			}
		}
		if ( iLinesDrawn == 2 ) {
			DrawXorBar( hDc, _pPoint.x, _pPoint.y, m_dwBarWidth, m_dwBarWidth );
		}
		ReleaseDC( m_hWnd, hDc );

		m_pLastPoint = _pPoint;
	}

	// WM_LBUTTONDOWN.
	CWidget::LSW_HANDLED CSplitter::LButtonDown( DWORD _dwVirtKeys, const POINTS &_pCursorPos ) {
		m_bDragging = TRUE;
		POINT pCurPos;
		::GetCursorPos( &pCurPos );
		::SetCapture( Wnd() );

		LSW_RECT rRect = ClientRect();
		DrawBar( pCurPos, rRect, FALSE );

		return LSW_H_CONTINUE;
	}

	// WM_MOUSEMOVE.
	CWidget::LSW_HANDLED CSplitter::MouseMove( DWORD _dwVirtKeys, const POINTS &_pCursorPos ) {
		if ( !m_bDragging ) { return LSW_H_CONTINUE; }

		POINT pCurPos;
		::GetCursorPos( &pCurPos );

		LSW_RECT rRect = ClientRect();
		DrawBar( pCurPos, rRect, FALSE );

		return LSW_H_CONTINUE;
	}

	// WM_LBUTTONUP.
	CWidget::LSW_HANDLED CSplitter::LButtonUp( DWORD _dwVirtKeys, const POINTS &_pCursorPos ) {
		if ( !m_bDragging ) { return LSW_H_CONTINUE; }

		m_bDragging = FALSE;
		::ReleaseCapture();

		POINT pCurPos;
		::GetCursorPos( &pCurPos );

		LSW_RECT rRect = ClientRect();
		DrawBar( pCurPos, rRect, FALSE );

		return LSW_H_CONTINUE;
	}

}	// namespace lsw

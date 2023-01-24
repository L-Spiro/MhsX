#include "LSWSplitter.h"
#include "../Base/LSWBase.h"
#include "../Brush/LSWBrush.h"
#include "../Images/LSWBitmap.h"

namespace lsw {

	CSplitter::CSplitter( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CWidget( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(CBase::SplitterAtom()) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_bDragging( FALSE ),
		m_dwBarWidth( 4 ),
		m_dwSplitterType( LSW_SS_HORIZONTAL ),
		m_iHorPos( 10 ),
		m_iVertPos( 10 ) {
		
	}
	CSplitter::~CSplitter() {
	}

	// == Functions.
	// Window rectangle.
	LSW_RECT CSplitter::WindowRect( const CWidget * pwChild ) const {
		LSW_RECT rWindow;
		::GetWindowRect( Wnd(), &rWindow );
		LSW_RECT rRet = ClientRect( pwChild );
		rRet.MoveBy( rWindow.left, rWindow.top );
		return rRet;
	}

	// Client rectangle.
	LSW_RECT CSplitter::ClientRect( const CWidget * pwChild ) const {
		// If we only have 0 or 1 child, use the full rectangle.
		if ( m_vAttachments.size() > 1 ) {
			switch ( FindAttachment( pwChild ) ) {
				case LSW_A_UPPERLEFT : { return UpperLeftRect(); }
				case LSW_A_UPPERRIGHT : { return UpperRightRect(); }
				case LSW_A_LOWERLEFT : { return LowerLeftRect(); }
				case LSW_A_LOWERRIGHT : { return LowerRightRect(); }
				case LSW_A_UPPER : { return UpperRect(); }
				case LSW_A_BOTTOM : { return LowerRect(); }
				case LSW_A_LEFT : { return LeftRect(); }
				case LSW_A_RIGHT : { return RightRect(); }
			}
		}
		LSW_RECT rClient;
		::GetClientRect( Wnd(), &rClient );
		return rClient;
	}

	// Attach a control to the splitter.
	void CSplitter::Attach( CWidget * _pwWidget, LSW_ATTACHMENT _aAttachment ) {
		size_t sAtt = IndexOfAttachment( _pwWidget );
		if ( sAtt != LSW_S_NOATTACH ) {
			m_vAttachments[sAtt].aAttachment = _aAttachment;
		}
		else {
			m_vAttachments.push_back( { _pwWidget, _aAttachment } );
		}
	}

	// Set splitter styles.
	void CSplitter::SetSplitterType( LSW_SPLITTER_STYLES _ssType ) {
		m_dwSplitterType = _ssType;
	}

	// Sets the bar width.
	void CSplitter::SetBarWidth( DWORD _dwWidth ) {
		m_dwBarWidth = _dwWidth;
	}

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
	void CSplitter::DrawBar( POINT &_pPoint, LSW_RECT &_rClient, BOOL _bEnforceNew ) {
		//HDC hDc = ::GetDC( Wnd() );
		HDC hDc = ::GetWindowDC( Wnd() );

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
		::ReleaseDC( Wnd(), hDc );

		m_pLastPoint = _pPoint;
	}

	// Hit test.  _pPoint should be in client coordinates.
	bool CSplitter::HitTest( const POINT &_pPoint, DWORD &_dwHit ) const {
		_dwHit = 0;
		if ( m_dwSplitterType & LSW_SS_HORIZONTAL ) {
			INT iY = GetHorBarY();
			if ( _pPoint.y >= iY && _pPoint.y < iY + static_cast<INT>(m_dwBarWidth) ) {
				_dwHit |= LSW_SS_HORIZONTAL;
			}
		}
		if ( m_dwSplitterType & LSW_SS_VERTICAL ) {
			INT iX = GetVertBarX();
			if ( _pPoint.x >= iX && _pPoint.x < iX + static_cast<INT>(m_dwBarWidth) ) {
				_dwHit |= LSW_SS_VERTICAL;
			}
		}

		return _dwHit != 0;
	}

	// Gets the Y position of the horizontal bar.
	INT CSplitter::GetHorBarY() const {
		INT iY = std::max( 0, m_iHorPos );
		LSW_RECT rClient = ClientRect( this );
		return std::min( iY, static_cast<INT>(rClient.bottom - m_dwBarWidth) );
	}

	// Gets the X position of the vertical bar.
	INT CSplitter::GetVertBarX() const {
		INT iX = std::max( 0, m_iVertPos );
		LSW_RECT rClient = ClientRect( this );
		return std::min( iX, static_cast<INT>(rClient.right - m_dwBarWidth) );
	}

	// Gets the upper-left rectangle in a horizontal and vertical splitter.
	LSW_RECT CSplitter::UpperLeftRect() const {
		LSW_RECT rRet;
		rRet.left = rRet.top = 0;
		rRet.right = GetVertBarX();
		rRet.bottom = GetHorBarY();
		return rRet;
	}

	// Gets the upper-right rectangle in a horizontal and vertical splitter.
	LSW_RECT CSplitter::UpperRightRect() const {
		LSW_RECT rRet = ClientRect( this );
		rRet.left = GetVertBarX() + m_dwBarWidth;
		rRet.bottom = GetHorBarY();
		return rRet;
	}

	// Gets the lower-left rectangle in a horizontal and vertical splitter.
	LSW_RECT CSplitter::LowerLeftRect() const {
		LSW_RECT rRet = ClientRect( this );
		rRet.right = GetVertBarX();
		rRet.top = GetHorBarY() + m_dwBarWidth;
		return rRet;
	}

	// Gets the lower-right rectangle in a horizontal and vertical splitter.
	LSW_RECT CSplitter::LowerRightRect() const {
		LSW_RECT rRet = ClientRect( this );
		rRet.left = GetVertBarX() + m_dwBarWidth;
		rRet.top = GetHorBarY() + m_dwBarWidth;
		return rRet;
	}

	// Gets the top rectangle in a horizontal splitter.
	LSW_RECT CSplitter::UpperRect() const {
		LSW_RECT rRet = ClientRect( this );
		rRet.bottom = GetHorBarY();
		return rRet;
	}

	// Gets the bottom rectangle in a horizontal splitter.
	LSW_RECT CSplitter::LowerRect() const {
		LSW_RECT rRet = ClientRect( this );
		rRet.top = GetHorBarY() + m_dwBarWidth;
		return rRet;
	}

	// Gets the left rectangle in a vertical splitter.
	LSW_RECT CSplitter::LeftRect() const {
		LSW_RECT rRet = ClientRect( this );
		rRet.right = GetVertBarX();
		return rRet;
	}

	// Gets the right rectangle in a vertical splitter.
	LSW_RECT CSplitter::RightRect() const {
		LSW_RECT rRet = ClientRect( this );
		rRet.left = GetVertBarX() + m_dwBarWidth;
		return rRet;
	}

	// Gets a control's attachment by pointer.
	DWORD CSplitter::FindAttachment( const CWidget * _pwWidget ) const {
		size_t sIndex = IndexOfAttachment( _pwWidget );
		if ( sIndex != LSW_S_NOATTACH ) { return m_vAttachments[sIndex].aAttachment; }
		return LSW_A_NONE;
	}

	// Gets the index of a control or LSW_S_NOATTACH.
	size_t CSplitter::IndexOfAttachment( const CWidget * _pwWidget ) const {
		for ( size_t I = 0; I < m_vAttachments.size(); ++I ) {
			if ( m_vAttachments[I].pwWidget == _pwWidget ) { return I; }
		}
		return LSW_S_NOATTACH;
	}

	// Remove a child.
	void CSplitter::RemoveChild( const CWidget * _pwChild ) {
		for ( size_t I = m_vAttachments.size(); I--; ) {
			if ( m_vAttachments[I].pwWidget == _pwChild ) {
				m_vAttachments.erase( m_vAttachments.begin() + I );
			}
		}

		CWidget::RemoveChild( _pwChild );
	}

	// Size all attachments.
	void CSplitter::SizeAttachments() const {
		for ( size_t I = 0; I < m_vAttachments.size(); ++I ) {
			LSW_RECT rRect = ClientRect( m_vAttachments[I].pwWidget );
			::SetWindowPos( m_vAttachments[I].pwWidget->Wnd(), NULL, rRect.left, rRect.top, rRect.Width(), rRect.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOSENDCHANGING );
		}
	}

	// WM_LBUTTONDOWN.
	CWidget::LSW_HANDLED CSplitter::LButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		// If we only have 0 or 1 child, do nothing.
		if ( m_vAttachments.size() <= 1 ) { return LSW_H_CONTINUE; }
		m_bDragging = TRUE;
		POINT pCurPos = { _pCursorPos.x, _pCursorPos.y };
		::SetCapture( Wnd() );

		LSW_RECT rRect = ClientRect( this );
		DrawBar( pCurPos, rRect, FALSE );

		return LSW_H_CONTINUE;
	}

	// WM_MOUSEMOVE.
	CWidget::LSW_HANDLED CSplitter::MouseMove( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		if ( !m_bDragging ) { return LSW_H_CONTINUE; }

		POINT pCurPos = { _pCursorPos.x, _pCursorPos.y };

		LSW_RECT rRect = ClientRect( this );
		DrawBar( m_pLastPoint, rRect, FALSE );
		DrawBar( pCurPos, rRect, FALSE );

		return LSW_H_CONTINUE;
	}

	// WM_LBUTTONUP.
	CWidget::LSW_HANDLED CSplitter::LButtonUp( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		if ( !m_bDragging ) { return LSW_H_CONTINUE; }

		m_bDragging = FALSE;
		::ReleaseCapture();

		POINT pCurPos = { _pCursorPos.x, _pCursorPos.y };

		LSW_RECT rRect = ClientRect( this );
		DrawBar( m_pLastPoint, rRect, FALSE );

		if ( m_dwSplitterType & LSW_SS_VERTICAL ) {
			m_iVertPos = pCurPos.x;
		}
		if ( m_dwSplitterType & LSW_SS_HORIZONTAL ) {
			m_iHorPos = pCurPos.y;
		}
		//CWidget::Size( 0, 0, 0 );
		SizeAttachments();

		return LSW_H_CONTINUE;
	}

}	// namespace lsw

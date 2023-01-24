#include "LSWDockable.h"
#include "../Base/LSWBase.h"
#include "../Brush/LSWBrush.h"
#include "../Helpers/LSWHelpers.h"
#include "../Images/LSWBitmap.h"
#include "../MultiSplitter/LSWMultiSplitter.h"

namespace lsw {

	// == Members.
	// To keep track of window movement.  Only one window can move via the mouse at a time.
	// The dragging rectangle.
	LSW_RECT CDockable::m_rDragPlacementRect;

	// The original rectangle of the window at the time drawgging began.
	LSW_RECT CDockable::m_rDragStartRect;

	// The widget being dragged.
	CWidget * CDockable::m_pwDraggingDockWnd = nullptr;

	// The hook procedure.
	HHOOK CDockable::m_hOrigDockHookProc = NULL;

	// The mouse position.
	POINT CDockable::m_pMousePos = { 0, 0 };

	// The position of starting a drag.
	POINT CDockable::m_pDragStartPos = { 0, 0 };

	// Has the mouse moved?
	BOOL CDockable::m_bMouseMoved = FALSE;

	// Drag rectangle type.
	CDockable::LSW_DOCK_DRAG_RECT_TYPE CDockable::m_ddrtDragRectType = CDockable::LSW_DDRT_CHECKERED;

	CDockable::CDockable( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CWidget( _wlLayout.ChangeStyle( LSW_POPUP_STYLES | (_wlLayout.dwStyle & WS_VISIBLE) ).ChangeStyleEx( LSW_POPUP_STYLESEX ).ChangeClass( reinterpret_cast<LPCWSTR>(CBase::DockableAtom()) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_dwState( LSW_DS_FLOATING ),
		m_dwDockStyle( LSW_DS_ALLOW_DOCKALL ),
		m_iFrameWidth( 0 ),
		m_iFrameHeight( 0 ),
		m_dwDockSize( 0 ),
		m_bUseGradient( TRUE ),
		m_pdtDockedControl( nullptr ) {
		if ( Ancestor() ) {
			Ancestor()->AddDockable( this );
		}
		::GetClientRect( Wnd(), (m_dwState & LSW_DS_FLOATING) ? &m_rFloatRect : &m_rDockedRect );
		CalcFloatFrame();
	}
	CDockable::~CDockable() {
		if ( m_pdtDockedControl ) {
			m_pdtDockedControl->Detach( Id() );
			m_pdtDockedControl = nullptr;
		}

		if ( Ancestor() ) {
			Ancestor()->RemDockable( this );
		}
	}

	// == Functions.
	// Client rectangle.
	LSW_RECT CDockable::ClientRect( const CWidget * pwChild ) const {
		if ( Floating() ) { return CWidget::ClientRect( pwChild ); }
		LSW_RECT rRect;
		::GetClientRect( Wnd(), &rRect );
		rRect.top += ::GetSystemMetrics( SM_CYSMCAPTION );
		return rRect;
	}

	// Adds a dock target to be queried during drags.
	void CDockable::AddDockTarget( CDockTarget * _pdtTarget ) {
		if ( !HasDockTarget( _pdtTarget ) ) {
			m_vDockingTargets.push_back( _pdtTarget );
		}
	}

	// Determines if the given target is already in the list of dock targets.
	bool CDockable::HasDockTarget( CDockTarget * _pdtTarget ) {
		return std::find( m_vDockingTargets.begin(), m_vDockingTargets.end(), _pdtTarget ) != m_vDockingTargets.end();
	}

	// The dockable message handler.
	LRESULT CALLBACK CDockable::WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CWidget * pmwThis = LSW_WIN2CLASS( _hWnd );
		switch ( _uMsg ) {
			case WM_NCACTIVATE : {
				if ( pmwThis ) {
					CWidget * pwPar = pmwThis->Ancestor();
					if ( pwPar ) {
						return pwPar->DockNcActivate( pmwThis, _wParam, _lParam, TRUE );
					}
				}
				break;
			}
			case WM_ENABLE : {
				if ( pmwThis ) {
					CWidget * pwPar = pmwThis->Ancestor();
					if ( pwPar ) {
						return pwPar->DockEnable( pmwThis, _wParam, _lParam, TRUE );
					}
				}
				break;
			}
		}
		return CWidget::WindowProc( _hWnd, _uMsg, _wParam, _lParam );
	}

	// Converts a CDockTarget::LSW_DT_ATTACH object to a LSW_DOCK_STATE state.
	CDockable::LSW_DOCK_STATE CDockable::AttachToDockState( const CDockTarget::LSW_DT_ATTACH &_daAttach ) {
		if ( !_daAttach.pwWidget ) { return LSW_DS_FLOATING; }
		switch ( _daAttach.atAttachTo ) {
			case CDockTarget::LSW_AT_LEFT : { return LSW_DS_DOCKED_LEFT; }
			case CDockTarget::LSW_AT_RIGHT : { return LSW_DS_DOCKED_RIGHT; }
			case CDockTarget::LSW_AT_TOP : { return LSW_DS_DOCKED_TOP; }
			case CDockTarget::LSW_AT_BOTTOM : { return LSW_DS_DOCKED_BOTTOM; }
		}
		return LSW_DS_FLOATING;
	}

	// WM_SIZE.
	CWidget::LSW_HANDLED CDockable::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		CWidget::Size( _wParam, _lWidth, _lHeight );
		::GetClientRect( Wnd(), (m_dwState & LSW_DS_FLOATING) ? &m_rFloatRect : &m_rDockedRect );
		CalcFloatFrame();
		return LSW_H_CONTINUE;
	}

	// WM_CANCELMODE.
	CWidget::LSW_HANDLED CDockable::CancelMode() {
		CancelDrag( TRUE );
		return LSW_H_CONTINUE;
	}

	// WM_LBUTTONUP.
	CWidget::LSW_HANDLED CDockable::LButtonUp( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) {
		if ( m_pwDraggingDockWnd ) {
			CancelDrag( FALSE );
		}
		{
			POINT pPnt;
			::GetCursorPos( &pPnt );
			LRESULT lrHit = ::SendMessageW( Wnd(), WM_NCHITTEST, 0, MAKELPARAM( pPnt.x, pPnt.y ) );
			if ( lrHit == HTCLOSE ) {
				return Close();
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_MOUSEMOVE.
	CWidget::LSW_HANDLED CDockable::MouseMove( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) {
		if ( m_pwDraggingDockWnd ) {
			// Yes. Move the drag rectangle.
			POINT pMousePos;
			LSW_DOCK_STATE dsDockSide;

			// Get mouse pointer position in screen coords
			::GetCursorPos( &pMousePos );

			// Did the mouse move?
			if ( pMousePos.x != m_pMousePos.x || pMousePos.y != m_pMousePos.y ) {
				// Indicate the drag rectangle (and therefore potentially the Docking Frame) has moved
				m_bMouseMoved = TRUE;

				// Save new mouse pointer position
				m_pMousePos = pMousePos;
			}

			// Did the mouse move or CTRL key state change?
			if ( m_bMouseMoved ) {
				// Clear CTRL key status change.
				m_bMouseMoved &= ~LSW_DS_FLOATING;
				CDockTarget * pdtTarget;
				CDockTarget::LSW_DT_ATTACH adAttach = CheckDockingPos( m_rDragPlacementRect, pdtTarget );
				dsDockSide = AttachToDockState( adAttach );

				// If we have moved over a dockable area, then temporarily change the size
				// of the drag rectangle to fill that dockable area
				if ( m_ddrtDragRectType == LSW_DDRT_SOLID ) {
					/*
					LSW_RECT rParCient = Parent()->VirtualClientRect( this ).MapToDeskTop( Parent()->Wnd() );

					switch ( dsDockSide ) {
						case LSW_DS_DOCKED_LEFT : {
							m_rDragPlacementRect.right = rParCient.left + m_dwDockSize;
							m_rDragPlacementRect.left = rParCient.left;
							m_rDragPlacementRect.top = rParCient.top;
							m_rDragPlacementRect.bottom = rParCient.bottom;
							break;
						}
						case LSW_DS_DOCKED_RIGHT : {
							m_rDragPlacementRect.left = rParCient.right - m_dwDockSize;
							m_rDragPlacementRect.right = rParCient.right;
							m_rDragPlacementRect.top = rParCient.top;
							m_rDragPlacementRect.bottom = rParCient.bottom;
							break;
						}
						case LSW_DS_DOCKED_TOP : {
							m_rDragPlacementRect.bottom = rParCient.top + m_dwDockSize;
							m_rDragPlacementRect.top = rParCient.top;
							m_rDragPlacementRect.left = rParCient.left;
							m_rDragPlacementRect.right = rParCient.right;
							break;
						}
						case LSW_DS_DOCKED_BOTTOM : {
							m_rDragPlacementRect.top = rParCient.bottom - m_dwDockSize;
							m_rDragPlacementRect.bottom = rParCient.bottom;
							m_rDragPlacementRect.left = rParCient.left;
							m_rDragPlacementRect.right = rParCient.right;
							break;
						}
					}*/
				}
				DrawDragRect();
			}
		}

		return LSW_H_CONTINUE;
	}

	// WM_NCLBUTTONDOWN.
	CWidget::LSW_HANDLED CDockable::NcLButtonDown( INT _iHitTest, const POINTS &/*_pCursorPos*/ ) {
		if ( _iHitTest == HTCAPTION && (m_dwDockStyle & LSW_DS_ALLOW_DOCKALL) && m_vDockingTargets.size() ) {
			if ( (m_dwDockStyle & LSW_DS_KEEPORIGSTATE) == 0 ) {
					//BOOL bControlKeyDown = (::GetKeyState( VK_CONTROL ) & 0x8000) ? TRUE : FALSE;

					::GetCursorPos( &m_pMousePos );
					m_pDragStartPos = m_pMousePos;

					// Bring to the top.
					::SetWindowPos( Wnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

					// Capture the mouse.
					::SetCapture( Wnd() );

					// Indicate we're dragging now
					m_pwDraggingDockWnd = this;

					// Install the keyboard hook to check for user pressing CTRL or ESC keys
					m_hOrigDockHookProc = ::SetWindowsHookExW( WH_KEYBOARD, KeyboardProc, lsw::CBase::GetThisHandle(), 0 ); 

					::GetWindowRect( Wnd(), &m_rDragPlacementRect );
					m_rDragStartRect = m_rDragPlacementRect;

					// TODO: Check this logic.
					m_ddrtDragRectType = (m_dwState > 0) ? LSW_DDRT_SOLID : LSW_DDRT_CHECKERED;

					DrawDragRect();

					m_bMouseMoved = FALSE;
			}
		}
		if ( _iHitTest == HTCLOSE ) {
			return Close();
		}
		return LSW_H_CONTINUE;
	}

	// WM_NCMOUSEMOVE.
	CWidget::LSW_HANDLED CDockable::NcMouseMove( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) {
		/*if ( _iHitTest != m_iLastHit ) {
			::InvalidateRect( Wnd(), NULL, FALSE );
			::UpdateWindow( Wnd() );
		}*/
		return LSW_H_CONTINUE;
	}

	// WM_NCMOUSELEAVE.
	CWidget::LSW_HANDLED CDockable::NcMouseLeave() {
		INT iLast = m_iLastHit;
		m_iLastHit = HTNOWHERE;
		if ( iLast == HTCLOSE ) {
			LSW_RECT rCaption = GetCloseRect();
			::InvalidateRect( Wnd(), &rCaption, FALSE );
			::UpdateWindow( Wnd() );
		}
		return LSW_H_CONTINUE;
	}

	// WM_WINDOWPOSCHANGED.
	CWidget::LSW_HANDLED CDockable::WindowPosChanged( const WINDOWPOS * _pwpPos ) {
		if ( (m_dwDockStyle & LSW_DS_DONTSAVEPOS) == 0 ) {
			LSW_RECT rBorder = ClientRect( this );
			// Floating?
			if ( Floating() ) {
				if ( (_pwpPos->flags & SWP_NOMOVE) == 0 ) {
					// Moving.
					m_iFrameX = _pwpPos->x;
					m_iFrameY = _pwpPos->y;
				}
				if ( (_pwpPos->flags & SWP_NOSIZE) == 0 ) {
					// Width and height.
					m_iFrameWidth = _pwpPos->cx;
					m_iFrameWidth = _pwpPos->cy;

					//m_rFloatRect.SetWidth( 
					m_rFloatRect = rBorder;
				}
			}
			// Docked.
			else {
				if ( m_dwState & LSW_DS_DOCKED_LEFT | LSW_DS_DOCKED_RIGHT ) {
					rBorder.left = _pwpPos->cx;
				}
				else {
					rBorder.left = _pwpPos->cy;
				}
				if ( m_dwDockSize != DWORD( rBorder.left ) ) {
					m_dwDockSize = static_cast<DWORD>(rBorder.left);
					UpdateLayout( Parent() );
				}
			}

			if ( Floating() ) {
				// Floating or docked but not to LSW_DS_DOCKED_LEFT, LSW_DS_DOCKED_RIGHT, LSW_DS_DOCKED_TOP, or LSW_DS_DOCKED_BOTTOM.
				rBorder.left = rBorder.top = 0L;
				if ( m_dwDockStyle & LSW_DS_DRAWGRIPPERFLOATING ) { rBorder.left = 3; }
				rBorder.right = m_rFloatRect.Width();
				rBorder.bottom = m_rFloatRect.Height() - rBorder.left;
			}
			else {
				// It is docked. 
				::GetClientRect( Wnd(), &rBorder );

				if ( m_dwDockStyle & LSW_DS_DRAWGRIPPERDOCKED ) {
					// TODO: Don't hard-code these numbers.
					rBorder.left += 7;
					rBorder.right -= 7;
				}
			}


			if ( (_pwpPos->flags & SWP_NOSIZE) == 0 ) {
				::InvalidateRect( Wnd(), NULL, TRUE );
			}

			/*
				// Position/size the child window(s) which are inside of the
				// Docking Frame
				(*dwp->DockResize)(dwp, &border);
				*/
		}
		return LSW_H_CONTINUE;
	}

	// WM_NCHITTEST.
	CWidget::LSW_HANDLED CDockable::NcHitTest( const POINTS &_pCursorPos, INT &_iReturnHitTest ) {
		if ( Floating() ) { return LSW_H_CONTINUE; }

		LRESULT lRes = ::DefWindowProcW( Wnd(), WM_NCHITTEST, 0, MAKEWPARAM( _pCursorPos.x, _pCursorPos.y ) );
		if ( lRes == HTCLIENT ) { lRes = HTCAPTION; }
		if ( lRes == HTCAPTION ) {
			LSW_RECT rClose = GetCloseRect();
			POINT pPosBig = { _pCursorPos.x, _pCursorPos.y };
			::ScreenToClient( Wnd(), &pPosBig );
			if ( rClose.PointIsIn_ULInclusive( pPosBig ) ) {
				lRes = HTCLOSE;
			}
		}
		_iReturnHitTest = static_cast<INT>(lRes);
		if ( _iReturnHitTest != m_iLastHit ) {
			m_iLastHit = _iReturnHitTest;
			LSW_RECT rCaption = GetCloseRect();
			::InvalidateRect( Wnd(), &rCaption, FALSE );
			::UpdateWindow( Wnd() );
		}
		return LSW_H_HANDLED;
	}

	// WM_NCACTIVATE.
	CWidget::LSW_HANDLED CDockable::NcActivate( BOOL _bTitleBarActive, LPARAM _lParam ) {
		LSW_HANDLED hHand = CWidget::NcActivate( _bTitleBarActive, _lParam );
		if ( !Floating() ) {
			::RedrawWindow( Wnd(), NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME );
		}
		return hHand;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CDockable::Close() {
		::DestroyWindow( Wnd() );
		return LSW_H_HANDLED;
	}

	// WM_PAINT.
	CWidget::LSW_HANDLED CDockable::Paint() {
		if ( Floating() ) { return LSW_H_CONTINUE; }

		LSW_BEGINPAINT bpPaint( Wnd() );

		int iH = ::GetSystemMetrics( SM_CYSMCAPTION );
		LSW_RECT rRect;
		::GetClientRect( Wnd(), &rRect );
		rRect.bottom = rRect.top + iH;
		LONG rCliRight = rRect.right;
		BOOL bActive = /*Enabled() && */m_bShowAsActive;

		LSW_RECT rClose = GetCloseRect();
		rRect.right = rClose.left;

		::DrawCaption( Wnd(), bpPaint.hDc, &rRect, DC_BUTTONS | DC_TEXT | (bActive ? DC_ACTIVE : 0) | (m_bUseGradient ? DC_GRADIENT : 0) );



		LOGFONTW lf =  { 0 };
		lf.lfCharSet = SYMBOL_CHARSET;
		// Avoiding searchable strings.
		//::wcscpy_s( lf.lfFaceName, LSW_COUNT_OF( lf.lfFaceName ), L"Webdings" );
		lf.lfFaceName[7] = L's';
		lf.lfFaceName[4] = L'i';
		lf.lfFaceName[1] = L'e';
		lf.lfFaceName[6] = L'g';
		lf.lfFaceName[2] = L'b';
		lf.lfFaceName[0] = L'W';
		lf.lfFaceName[3] = L'd';
		lf.lfFaceName[5] = L'n';
		lf.lfHeight = -::MulDiv( iH / 2, ::GetDeviceCaps( bpPaint.hDc, LOGPIXELSY ), 72 );
		lf.lfWeight = FW_NORMAL;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfQuality = DRAFT_QUALITY;
		lf.lfOutPrecision = OUT_STROKE_PRECIS;
		lf.lfClipPrecision = CLIP_STROKE_PRECIS;
		lf.lfItalic = FALSE;
		lf.lfStrikeOut = FALSE;
		lf.lfUnderline = FALSE;
		lf.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
		HFONT hFont = ::CreateFontIndirectW( &lf );

		{
			LSW_SELECTOBJECT soBrushOrig( bpPaint.hDc, hFont );	// Destructor sets the original brush back.
			ABCFLOAT abcfWidth;
			::GetCharABCWidthsFloatA( bpPaint.hDc, 'r', 'r', &abcfWidth );
			INT iCharWidth = static_cast<INT>(abcfWidth.abcfA + abcfWidth.abcfB + abcfWidth.abcfC);

			LSW_RECT rChar = rClose;
			rChar.left = rClose.left + ((rChar.Width() - iCharWidth) >> 1);
			rChar.bottom = iH;


			COLORREF crTextColor = ::GetSysColor( bActive ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT );
			if ( m_iLastHit == HTCLOSE ) {
				crTextColor = RGB( 0xFF - GetRValue( crTextColor ),
					0xFF - GetGValue( crTextColor ),
					0xFF - GetBValue( crTextColor ) );
			}
			COLORREF crBackColor = ::GetSysColor( GetRightCaptionColor() );
			LSW_SETTEXTCOLOR stcText( bpPaint.hDc, crTextColor );
			LSW_SETBKCOLOR sbcB( bpPaint.hDc, crBackColor );
			
			::ExtTextOutA( bpPaint.hDc, rChar.left, rChar.top, ETO_OPAQUE | ETO_CLIPPED, &rChar, "r", 1, NULL );
			LONG lRight = rChar.right;


			rChar.right = rChar.left;
			rChar.left = rClose.left;
			::ExtTextOutA( bpPaint.hDc, rChar.left, rChar.top, ETO_OPAQUE | ETO_CLIPPED, &rChar, "", 0, NULL );


			rChar.left = lRight;
			rChar.right = rCliRight;
			::ExtTextOutA( bpPaint.hDc, rChar.left, rChar.top, ETO_OPAQUE | ETO_CLIPPED, &rChar, "", 0, NULL );
		}
		::DeleteObject( hFont );

		return LSW_H_HANDLED;
	}

	// Setting the HWND after the control has been created.
	void CDockable::InitControl( HWND _hWnd ) {
		CWidget::InitControl( _hWnd );
		CalcFloatFrame();
	}

	// Become a floating window.
	void CDockable::BecomeFloat() {
		SetStyle( (GetStyle() & ~LSW_CHILD_STYLES) | LSW_POPUP_STYLES );
		SetStyleEx( (GetStyleEx() & ~LSW_CHILD_STYLESEX) | LSW_POPUP_STYLESEX );
		SetParent( nullptr );
	}

	// Become a normal child window.
	void CDockable::BecomeChild() {
		SetStyle( (GetStyle() & ~LSW_POPUP_STYLES) | LSW_CHILD_STYLES );
		SetStyleEx( (GetStyleEx() & ~LSW_POPUP_STYLESEX) | LSW_CHILD_STYLESEX );
		SetParent( Parent() );
		m_bShowAsActive = TRUE;
	}

	// Check if the rectangle have moved to adocking or non-docking area.
	CDockTarget::LSW_DT_ATTACH CDockable::CheckDockingPos( LSW_RECT &_rRect, CDockTarget * &_pdtTarget ) const {
		// Should have been drawn once already.  Erase it.
		DrawDragRect();

		_pdtTarget = nullptr;
		CDockTarget::LSW_DT_ATTACH daAttach;
		daAttach.atAttachTo = CDockTarget::LSW_AT_LEFT;
		daAttach.dwId = 0;
		daAttach.pwWidget = 0;	// If this is nullptr, not attachment could be made.

		// The size of the drag rectangle should now be the size of the Docking
		// Frame when it is not docked (ie, floating). The drag rectangle should
		// be centered around the current position of the mouse pointer
		_rRect = m_rDragStartRect;
		_rRect.MoveBy( { m_pMousePos.x - m_pDragStartPos.x, m_pMousePos.y - m_pDragStartPos.y } );
	
		// ===========================================================
		// Check if the drag rectangle has moved into a dockable area.
		// ===========================================================
		BOOL bControlKeyDown = (::GetKeyState( VK_CONTROL ) & 0x8000) ? TRUE : FALSE;
		m_ddrtDragRectType = LSW_DDRT_SOLID;
		if ( !bControlKeyDown ) {
			LONG lDistWin = LONG_MAX;
			for ( size_t I = 0; I < m_vDockingTargets.size(); ++I ) {
				CDockTarget::LSW_DT_ATTACH daTemp;
				LSW_RECT rRectTemp;
				LONG lDist = m_vDockingTargets[I]->GetAttachPoint( m_pMousePos, daTemp, 300, rRectTemp );
				if ( lDist < lDistWin && lDist < 250 * 250 ) {
					lDistWin = lDist;
					m_rDragPlacementRect = rRectTemp;
					daAttach = daTemp;
					daAttach.pwWidget = const_cast<CDockable *>(this);
					m_ddrtDragRectType = LSW_DDRT_CHECKERED;
					_pdtTarget = m_vDockingTargets[I];
				}
			}
		}
		return daAttach;
#if 0
		const CWidget * pwContainer = Parent();
		if ( !pwContainer ) { return daAttach; }
		LSW_RECT rPrc2 = pwContainer->VirtualClientRect( this ).MapToDeskTop( pwContainer->Wnd() );
	

		LSW_DOCK_STATE dsDockSide = LSW_DS_FLOATING;

#define LSW_CLAMP		7
		// Check based on the mouse position.
		// Is it near the top?
		if ( std::abs( m_pMousePos.y - rPrc2.top ) <= LSW_CLAMP &&
			m_pMousePos.x >= rPrc2.left && m_pMousePos.x <= rPrc2.right ) {
			dsDockSide = LSW_DS_DOCKED_TOP;
		}
		// Is it near the bottom?
		if ( std::abs( m_pMousePos.y - rPrc2.bottom ) <= LSW_CLAMP &&
			m_pMousePos.x >= rPrc2.left && m_pMousePos.x <= rPrc2.right ) {
			dsDockSide = LSW_DS_DOCKED_BOTTOM;
		}
		// Is it near the left?
		if ( std::abs( m_pMousePos.x - rPrc2.left ) <= LSW_CLAMP &&
			m_pMousePos.y >= rPrc2.top && m_pMousePos.y <= rPrc2.bottom ) {
			dsDockSide = LSW_DS_DOCKED_LEFT;
		}
		// Is it near the right?
		if ( std::abs( m_pMousePos.x - rPrc2.right ) <= LSW_CLAMP &&
			m_pMousePos.y >= rPrc2.top && m_pMousePos.y <= rPrc2.bottom ) {
			dsDockSide = LSW_DS_DOCKED_RIGHT;
		}
#undef LSW_CLAMP


		BOOL bControlKeyDown = (::GetKeyState( VK_CONTROL ) & 0x8000) ? TRUE : FALSE;
		m_ddrtDragRectType = ((dsDockSide & LSW_DS_ALL_DOCKS) && !bControlKeyDown) ? LSW_DDRT_SOLID : LSW_DDRT_CHECKERED;
		//return LSW_DS_DOCKED_BOTTOM;
		return dsDockSide;
#endif	// #if 0
	}

	// Determine the size of the floating frame.
	void CDockable::CalcFloatFrame() {
		LSW_RECT rRect;
		rRect.left = rRect.top = 0;
		rRect.right = m_rFloatRect.Width() + rRect.left;
		rRect.bottom = m_rFloatRect.Height() + rRect.top;
		::AdjustWindowRectEx( &rRect, GetStyle(), FALSE, GetStyleEx() );

		m_iFrameWidth = rRect.Width();
		m_iFrameHeight = rRect.Height();
	}

	// Updates the layout.
	void CDockable::UpdateLayout( CWidget * _pwParent ) {
		if ( !_pwParent ) { return; }
		LSW_RECT rRect;

		// Send a fake WM_SIZE message to cause the window to recalculate and redraw its layout.
		rRect = _pwParent->ClientRect( this );
		::SendMessageW( _pwParent->Wnd(), WM_SIZE, SIZE_RESTORED, MAKELPARAM( rRect.Width(), rRect.Height() ) );
	}

	// Cancels the drag.
	void CDockable::CancelDrag( BOOL _bBecauseOfCancelMode ) {
		if ( m_pwDraggingDockWnd ) {
			// Remove keyboard hook.
			if ( m_hOrigDockHookProc ) {
				::UnhookWindowsHookEx( m_hOrigDockHookProc );
				m_hOrigDockHookProc = nullptr;
			}

			// NULL window indicates dragging is done.
			m_pwDraggingDockWnd = nullptr;

			if ( !_bBecauseOfCancelMode && m_bMouseMoved ) {
				::GetCursorPos( &m_pMousePos );

				CDockTarget * pdtTarget;
				CDockTarget::LSW_DT_ATTACH adAttach = CheckDockingPos( m_rDragPlacementRect, pdtTarget );
				LSW_DOCK_STATE dsDockSide = AttachToDockState( adAttach );


				if ( !Floating() ) {
					// Was docked, now floating.
					if ( Floating( dsDockSide ) ) {

						// TODO: Make a routine to adjust the placement based on the boundaries of the screen.
						m_rDragPlacementRect.left = std::max( m_rDragPlacementRect.left, 0L );
						m_iFrameX = static_cast<INT>(m_rDragPlacementRect.left);
						m_rDragPlacementRect.top = std::max( m_rDragPlacementRect.top, 0L );
						m_iFrameY = static_cast<INT>(m_rDragPlacementRect.top);

						// Set the docking mode to floating and redraw the window. Also
						// force the container window to update its layout
						m_dwState |= LSW_DS_FLOATING;
						RedrawDockingState( adAttach, pdtTarget );
					}
					// Was docked, still docked.
					else {
						// TODO: Make a routine to adjust the placement based on the boundaries of the screen.
						m_rDragPlacementRect.left = std::max( m_rDragPlacementRect.left, 0L );
						m_iFrameX = static_cast<INT>(m_rDragPlacementRect.left);
						m_rDragPlacementRect.top = std::max( m_rDragPlacementRect.top, 0L );
						m_iFrameY = static_cast<INT>(m_rDragPlacementRect.top);

						// Set the docking mode to floating and redraw the window. Also
						// force the container window to update its layout
						m_dwState |= LSW_DS_FLOATING;
						RedrawDockingState( adAttach, pdtTarget );

						adAttach = CheckDockingPos( m_rDragPlacementRect, pdtTarget );
						dsDockSide = AttachToDockState( adAttach );

						m_dwState = dsDockSide;

						//if (dwp->focusWindow) InvalidateRect(dwp->focusWindow, 0, 1);

						// Send WM_SIZE to parent window to reposition and redraws everything.
						//UpdateLayout( Parent() );
						RedrawDockingState( adAttach, pdtTarget );
					}
				}
				else {
					// Is floating.
					if ( !Floating( dsDockSide ) ) {
						// Became docked.
						m_dwState = dsDockSide;
						RedrawDockingState( adAttach, pdtTarget );
					}
					else {
						// Still floating.
						::SetWindowPos( Wnd(), NULL, m_rDragPlacementRect.left, m_rDragPlacementRect.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOSENDCHANGING );
					}
				}
			}
			else {
				DrawDragRect();
			}


			::ReleaseCapture();
			m_bMouseMoved = 0;
		}
#if 0
				

			// Sizing a docked window?
			else if (SizingDockWnd)
			{
				// Indicate we're done with the sizing
				SizingDockWnd = 0;

				goto mousedone;
			}
#endif
	}

	// Redraws the docking state.
	void CDockable::RedrawDockingState( CDockTarget::LSW_DT_ATTACH &_daAttach, CDockTarget * _pdtTarget ) {
		DWORD dwStyle = m_dwDockStyle;

		// LSW_DS_DONTSAVEPOS used inside WM_WINDOWPOSCHANGED to store the position of the window.
		// Temporarily disable that storage.
		m_dwDockStyle |= LSW_DS_DONTSAVEPOS;

		// Hide the docking frame so it will not be in the wrong position when docked/floated.
		SetVisible( FALSE );

		if ( Floating() ) {
			BecomeFloat();
			if ( m_pdtDockedControl ) {
				m_pdtDockedControl->Detach( Id() );
				m_pdtDockedControl = nullptr;
			}
		}
		else {
			BecomeChild();
			if ( _pdtTarget ) {
				if ( _pdtTarget->Attach( _daAttach ) ) {
					m_pdtDockedControl = _pdtTarget;
				}
				else {
					m_pdtDockedControl = nullptr;
					m_dwState |= LSW_DS_FLOATING;
				}
			}
		}

		// Send WM_NCCALCSIZE.
		::SetWindowPos( Wnd(), 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED );

		// if (dwp->focusWindow) SetFocus(dwp->focusWindow);

		m_dwDockStyle = dwStyle;

		UpdateLayout( Parent() );

		if ( Floating() ) {
			SetFloatingPos( SWP_SHOWWINDOW );
		}
		else {
			// UpdateLayout() will have sent WM_SiZE to the parent to size the docking control.
			::SetWindowPos( Wnd(), HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE );
		}
	}

	// Sets the floating window position.
	void CDockable::SetFloatingPos( DWORD _dwSetWindowPosFlags ) {
		CalcFloatFrame();
		::SetWindowPos( Wnd(), HWND_TOP, m_iFrameX, m_iFrameY, m_iFrameWidth, m_iFrameHeight, _dwSetWindowPosFlags );
	}

	// Gets the close rectangle.
	LSW_RECT CDockable::GetCloseRect() const {
		LSW_RECT rCaption = GetCaptionRect();
		int iCxSmSize = ::GetSystemMetrics( SM_CXSMSIZE );
		int iCySmSize = ::GetSystemMetrics( SM_CYSMSIZE );
		int iCxFrame = ::GetSystemMetrics( SM_CXFRAME );

		LSW_RECT rRet;
		rRet.left = std::max( 0L, rCaption.right - iCxSmSize - iCxFrame );
		rRet.right = rRet.left + iCxSmSize;

		rRet.top = std::max( 0L, ((rCaption.Height()) - iCySmSize) >> 1 );
		rRet.bottom = rRet.top + iCySmSize;
		return rRet;
	}

	// Gets the caption rectangle.
	LSW_RECT CDockable::GetCaptionRect() const {
		LSW_RECT rRet;
		::GetClientRect( Wnd(), &rRet );
		rRet.bottom = rRet.top + ::GetSystemMetrics( SM_CYSMCAPTION );
		return rRet;
	}

	// Keyboard hook.
	LRESULT CALLBACK CDockable::KeyboardProc( int _iCode, WPARAM _wParam, LPARAM _lParam ) {
		return ::CallNextHookEx( m_hOrigDockHookProc, _iCode, _wParam, _lParam );
	}

	// Draw the drag rectangle.
	void CDockable::DrawDragRect() {
		// Used by drawDragFrame() to draw the drag rectangle
		static const WORD wDotPatternBmp1[] = {
			0x00AA, 0x0055, 0x00AA, 0x0055, 0x00AA, 0x0055, 0x00AA, 0x0055
			//0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555
		};
		static const WORD wDotPatternBmp2[] = {
			0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
		};
		HDC hDc = ::GetDC( NULL );
		const WORD * pwBitmap = (m_ddrtDragRectType == CDockable::LSW_DDRT_CHECKERED) ? wDotPatternBmp2 : wDotPatternBmp1;
		INT iBorder = (m_ddrtDragRectType == CDockable::LSW_DDRT_CHECKERED) ? 1 : 3;

		CBitmap bBitmap;
		bBitmap.CreateBitmap( 8, 8, 1, 1, pwBitmap );
		CBrush bBrush;
		bBrush.CreatePatternBrush( bBitmap );
		::SetBrushOrgEx( hDc, m_rDragPlacementRect.left, m_rDragPlacementRect.top, NULL );

		LSW_SELECTOBJECT soBrushOrig( hDc, bBrush.Handle() );	// Destructor sets the original brush back.

		LONG lWidth = m_rDragPlacementRect.right - m_rDragPlacementRect.left;
		LONG lHeight = m_rDragPlacementRect.bottom - m_rDragPlacementRect.top;
		::PatBlt( hDc, m_rDragPlacementRect.left + iBorder, m_rDragPlacementRect.top, lWidth - iBorder, iBorder, PATINVERT );
		::PatBlt( hDc, m_rDragPlacementRect.left + lWidth - iBorder, m_rDragPlacementRect.top + iBorder, iBorder, lHeight - iBorder, PATINVERT );
		::PatBlt( hDc, m_rDragPlacementRect.left, m_rDragPlacementRect.top + lHeight - iBorder, lWidth - iBorder,  iBorder, PATINVERT );
		::PatBlt( hDc, m_rDragPlacementRect.left, m_rDragPlacementRect.top, iBorder, lHeight - iBorder, PATINVERT );

		::ReleaseDC( 0, hDc );
	}

}	// namespace lsw

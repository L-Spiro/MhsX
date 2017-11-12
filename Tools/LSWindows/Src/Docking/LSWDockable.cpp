#include "LSWDockable.h"
#include "../Base/LSWBase.h"
#include "../Brush/LSWBrush.h"
#include "../Helpers/LSWHelpers.h"
#include "../Images/LSWBitmap.h"

namespace lsw {

	// == Members.
	// To keep track of window movement.  Only one window can move via the mouse at a time.
	// The dragging rectangle.
	LSW_RECT CDockable::m_rDragPlacementRect;

	// The widget being dragged.
	CWidget * CDockable::m_pwDraggingDockWnd = nullptr;

	// The hook procedure.
	HHOOK CDockable::m_hOrigDockHookProc = NULL;

	// The mouse position.
	POINT CDockable::m_pMousePos = { 0, 0 };

	// Has the mouse moved?
	BOOL CDockable::m_bMouseMoved = FALSE;

	// Drag rectangle type.
	CDockable::LSW_DOCK_DRAG_RECT_TYPE CDockable::m_ddrtDragRectType = CDockable::LSW_DDRT_CHECKERED;

	CDockable::CDockable( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		CWidget( _wlLayout.ChangeStyle( LSW_POPUP_STYLES | (_wlLayout.dwStyle & WS_VISIBLE) ).ChangeStyleEx( LSW_POPUP_STYLESEX ).ChangeClass( reinterpret_cast<LPCWSTR>(CBase::DockableAtom()) ), _pwParent, _bCreateWidget, _hMenu ),
		m_dwState( LSW_DS_FLOATING ),
		m_dwDockStyle( 0/*LSW_DS_ALLOW_DOCKALL*/ ),
		m_iFrameWidth( 0 ),
		m_iFrameHeight( 0 ),
		m_dwDockSize( 0 ) {
		if ( Parent() ) {
			Parent()->AddDockable( this );
		}
		::GetClientRect( Wnd(), (m_dwState & LSW_DS_FLOATING) ? &m_rFloatRect : &m_rDockedRect );
		CalcFloatFrame();
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
		}
		return CWidget::WindowProc( _hWnd, _uMsg, _wParam, _lParam );
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
	CWidget::LSW_HANDLED CDockable::LButtonUp( DWORD _dwVirtKeys, const POINTS &_pCursorPos ) {
		CancelDrag( FALSE );
		return LSW_H_CONTINUE;
	}

	// WM_MOUSEMOVE.
	CWidget::LSW_HANDLED CDockable::MouseMove( DWORD _dwVirtKeys, const POINTS &_pCursorPos ) {
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

					dsDockSide = CheckDockingPos();

					// If we have moved over a dockable area, then temporarily change the size
					// of the drag rectangle to fill that dockable area
					if ( m_ddrtDragRectType == LSW_DDRT_SOLID ) {

						LSW_RECT rParCient = Parent()->ClientRect().MapToDeskTop( Parent()->Wnd() );

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
						}
					}
					DrawDragRect();
				}
			}

		return LSW_H_CONTINUE;
	}

	// WM_NCLBUTTONDOWN.
	CWidget::LSW_HANDLED CDockable::NcLButtonDown( INT _iHitTest, const POINTS &_pCursorPos ) {
		if ( _iHitTest == HTCAPTION && (m_dwDockStyle & LSW_DS_ALLOW_DOCKALL) ) {
			if ( (m_dwDockStyle & LSW_DS_KEEPORIGSTATE) == 0 ) {
					BOOL bControlKeyDown = (::GetKeyState( VK_CONTROL ) & 0x8000) ? TRUE : FALSE;

					::GetCursorPos( &m_pMousePos );

					// Bring to the top.
					::SetWindowPos( Wnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

					// Capture the mouse.
					::SetCapture( Wnd() );

					// Indicate we're dragging now
					m_pwDraggingDockWnd = this;

					// Install the keyboard hook to check for user pressing CTRL or ESC keys
					m_hOrigDockHookProc = ::SetWindowsHookExW( WH_KEYBOARD, KeyboardProc, lsw::CBase::GetThisHandle(), 0 ); 

					::GetWindowRect( Wnd(), &m_rDragPlacementRect );

					// TODO: Check this logic.
					m_ddrtDragRectType = (m_dwState > 0) ? LSW_DDRT_SOLID : LSW_DDRT_CHECKERED;

					DrawDragRect();

					m_bMouseMoved = FALSE;
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_WINDOWPOSCHANGED.
	CWidget::LSW_HANDLED CDockable::WindowPosChanged( const WINDOWPOS * _pwpPos ) {
		if ( (m_dwDockStyle & LSW_DS_DONTSAVEPOS) == 0 ) {
			RECT rBorder;
			::GetClientRect( Wnd(), &rBorder );
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
				if ( m_dwDockSize != rBorder.left ) {
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
	}

	// Toggle between floating and being a child.
	void CDockable::ToggleFloatAndChild() {
		if ( GetStyle() & WS_CHILD ) {
			BecomeFloat();
		}
		else {
			BecomeChild();
		}
	}

	// Check if the rectangle have moved to adocking or non-docking area.
	CDockable::LSW_DOCK_STATE CDockable::CheckDockingPos() const {
		// Should have been drawn once already.  Erase it.
		DrawDragRect();

		// The size of the drag rectangle should now be the size of the Docking
		// Frame when it is not docked (ie, floating). The drag rectangle should
		// be centered around the current position of the mouse pointer
		m_rDragPlacementRect.left = m_pMousePos.x - (m_iFrameWidth >> 1);
		m_rDragPlacementRect.left = std::max( m_rDragPlacementRect.left, 0L );

		m_rDragPlacementRect.top = m_pMousePos.y - (m_iFrameHeight >> 1);
		m_rDragPlacementRect.top = std::max( m_rDragPlacementRect.top, 0L );

		m_rDragPlacementRect.right = m_rDragPlacementRect.left + m_iFrameWidth;
		m_rDragPlacementRect.bottom = m_rDragPlacementRect.top + m_iFrameHeight;
	
		// ===========================================================
		// Check if the drag rectangle has moved into a dockable area.
		// ===========================================================
		const CWidget * pwContainer = Parent();
		if ( !pwContainer ) { return static_cast<LSW_DOCK_STATE>(0); }
		LSW_RECT rPrc1 = pwContainer->WindowRect();
		LSW_RECT rPrc2 = pwContainer->ClientRect().MapToDeskTop( pwContainer->Wnd() );
		::InflateRect( &rPrc2, -2 , -2 );
	

		LSW_DOCK_STATE dsDockSide = LSW_DS_FLOATING;

		// If outside of the container frame, then floating.
		if ( m_rDragPlacementRect.left >= rPrc1.left && m_rDragPlacementRect.right <= rPrc1.right &&
			m_rDragPlacementRect.top >= rPrc1.top && m_rDragPlacementRect.bottom <= rPrc1.bottom ) {
			// Check intersection at bottom
			if ( m_rDragPlacementRect.bottom > rPrc2.bottom && (m_dwDockStyle & LSW_DS_ALLOW_DOCKBOTTOM) ) {
				dsDockSide = LSW_DS_DOCKED_BOTTOM;
			}

			// Check intersection at top
			if ( m_rDragPlacementRect.top < rPrc2.top && (m_dwDockStyle & LSW_DS_ALLOW_DOCKTOP) ) {
				dsDockSide = LSW_DS_DOCKED_TOP;
			}

			// Check intersection at left
			if ( m_rDragPlacementRect.left < rPrc2.left && (m_dwDockStyle & LSW_DS_ALLOW_DOCKLEFT) ) {
				dsDockSide = LSW_DS_DOCKED_LEFT;
			}

			// Check intersection at right
			if ( m_rDragPlacementRect.right > rPrc2.right && (m_dwDockStyle & LSW_DS_ALLOW_DOCKRIGHT) ) {
				dsDockSide = LSW_DS_DOCKED_RIGHT;
			}
		}

		BOOL bControlKeyDown = (::GetKeyState( VK_CONTROL ) & 0x8000) ? TRUE : FALSE;
		m_ddrtDragRectType = ((dsDockSide & LSW_DS_ALL_DOCKS) && !bControlKeyDown) ? LSW_DDRT_SOLID : LSW_DDRT_CHECKERED;
		//return LSW_DS_DOCKED_BOTTOM;
		return dsDockSide;
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
		::GetClientRect( _pwParent->Wnd(), &rRect );
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

				LSW_DOCK_STATE dsDockSide = CheckDockingPos();


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
						RedrawDockingState();
					}
					// Was docked, still docked.
					else {
						m_dwState = dsDockSide;

						//if (dwp->focusWindow) InvalidateRect(dwp->focusWindow, 0, 1);

						// Send WM_SIZE to parent window to reposition and redraws everything.
						UpdateLayout( Parent() );
					}
				}
				else {
					// Is floating.
					if ( !Floating( dsDockSide ) ) {
						// Became docked.
						m_dwState = dsDockSide;
						RedrawDockingState();
					}
					else {
						// Still floating.
						::SetWindowPos( Wnd(), 0, m_rDragPlacementRect.left, m_rDragPlacementRect.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOSENDCHANGING );
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
				// Did the user actually move the Docking Frame, and not cancel the operation?
				if (msg != WM_CANCELMODE && MouseMoved)
				{
					POINT	pt;
					char	uDockSide;

					// Get final cursor position
					GetCursorPos(&pt);
					MousePos.x = (short)pt.x;
					MousePos.y = (short)pt.y;

					// Erase the drag rectangle by redrawing it at the same position as before,
					// and check if it was moved into a dockable area
					uDockSide = checkDockingPos(dwp);

					// If the window was docked, see if it is now floating
					if (m_dwState > 0)
					{
						// If he held the control key down, or moved it to a non-dockable
						// area, then it is now floating
						if (uDockSide < 0)
						{
							// Set the XY position where the Docking Frame will float. The
							// position is where the last mouse position is
							if (m_rDragPlacementRect.left < 0) m_rDragPlacementRect.left = 0;
							m_iFrameX = (unsigned short)m_rDragPlacementRect.left;
							if (m_rDragPlacementRect.top < 0) m_rDragPlacementRect.top = 0;
							m_iFrameY = (unsigned short)m_rDragPlacementRect.top;

							// Set the docking mode to floating and redraw the window. Also
							// force the container window to update its layout
							m_dwState |= (char)0x80;
							redrawDockingState(dwp);
						}

						// The window is still docked (but may have changed to different
						// side of the container window)
						else
						{
							m_dwState = uDockSide;

							// Invalidate any child window so that it redraws itself, just in case
							// the frame moved
							if (dwp->focusWindow) InvalidateRect(dwp->focusWindow, 0, 1);

							// Send a WM_SIZE message to the container window so it repositions and
							// redraws everything docked inside of it
							updateLayout(dwp->container);
						}
					}

					// It was floating, so see if it is now docked
					else
					{
						// If he didn't hold the control key down, and moved it to a dockable
						// area, then it is now docked
						if (uDockSide > 0)
						{
							m_dwState = uDockSide;
							redrawDockingState(dwp);
						}

						// It is still floating. Just change the Docking Frame position to the last
						// position of the mouse
						else
							SetWindowPos(hwnd, 0, m_rDragPlacementRect.left, m_rDragPlacementRect.top, 0, 0, SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER|SWP_DRAWFRAME|SWP_NOSENDCHANGING);
					}
				}

				// Erase the drag rectangle if user cancelled
				else
					drawDragFrame();

				// No need to trap the mouse any further since we're done
mousedone:		ReleaseCapture();
				MouseMoved = 0;
			}

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
	void CDockable::RedrawDockingState() {
		DWORD dwStyle = m_dwDockStyle;

		// LSW_DS_DONTSAVEPOS used inside WM_WINDOWPOSCHANGED to store the position of the window.
		// Temporarily disable that storage.
		m_dwDockStyle |= LSW_DS_DONTSAVEPOS;

		// Hide the docking frame so it will not be in the wrong position when docked/floated.
		SetVisible( FALSE );

		if ( Floating() ) {
			BecomeFloat();
		}
		else {
			BecomeChild();
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

	// Keyboard hook.
	LRESULT CALLBACK CDockable::KeyboardProc( int _iCode, WPARAM _wParam, LPARAM _lParam ) {
		return CallNextHookEx( m_hOrigDockHookProc, _iCode, _wParam, _lParam );
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
		const WORD * pwBitmap = (m_ddrtDragRectType = CDockable::LSW_DDRT_CHECKERED) ? wDotPatternBmp2 : wDotPatternBmp1;
		INT iBorder = (m_ddrtDragRectType = CDockable::LSW_DDRT_CHECKERED) ? 1 : 3;

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

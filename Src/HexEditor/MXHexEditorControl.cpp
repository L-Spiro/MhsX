#include "MXHexEditorControl.h"
#include "../Utilities/MXUtilities.h"

#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <Helpers/LSWHelpers.h>


namespace mx {

	// == Members.
	// The main window class.
	ATOM CHexEditorControl::m_aAtom = 0;

	CHexEditorControl::CHexEditorControl( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, lsw::CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CParent( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ).AddStyle( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL )/*.AddStyleEx( WS_EX_CLIENTEDGE ).RemoveStyleEx( WS_EX_WINDOWEDGE )*/, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	// == Functions.
	// // WM_NCCREATE.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::NcCreate( const CREATESTRUCTW &_csCreateParms ) {
		

		return CParent::NcCreate( _csCreateParms );
	}

	// WM_NCDESTROY.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::NcDestroy() {
		return CParent::NcDestroy();
	}

	// Setting the HWND after the control has been created.
	void CHexEditorControl::InitControl( HWND _hWnd ) {
		m_iCxChar = m_iCyChar = 0;
		m_iClientW = m_iClientH = 0;
		//m_hHex.sStyle.uiBytesPerRow = 16;
		/*m_pData = nullptr;
		m_ui64Size = 0ULL;*/
		m_iVPos = 0;
		m_iHPos = 0;
		m_iPageLines = m_iPageCols = 1;
		m_iAddrDigits = 8;
		//SetState( _hWnd, ps );

		ChooseDefaultFont();
		ComputeFontMetrics();

		CParent::InitControl( _hWnd );
	}

	/**
	 * The WM_GETDLGCODE handler.
	 * 
	 * \param _wKey The virtual key, pressed by the user, that prompted Windows to issue this notification. The handler must selectively handle these keys. For instance, the handler might accept and process VK_RETURN but delegate VK_TAB to the owner window.
	 * \return Returns an LSW_HANDLED code.
	 **/
	WORD CHexEditorControl::GetDlgCode( WORD _wKey ) {
		return DLGC_WANTARROWS | DLGC_WANTTAB | DLGC_WANTALLKEYS | DLGC_WANTCHARS;
	}

	// WM_PAINT.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::Paint() {
		lsw::LSW_RECT rRect = ClientRect();
		lsw::LSW_BEGINPAINT bpPaint( Wnd() );
		
		HDC hDcMem = ::CreateCompatibleDC( bpPaint.hDc );
		HBITMAP hBmp = ::CreateCompatibleBitmap( bpPaint.hDc, int( rRect.Width() ), int( rRect.Height() ) );
		{
			lsw::LSW_SELECTOBJECT soBmp( hDcMem, hBmp );
			PaintHex( hDcMem, rRect );


			::BitBlt( bpPaint.hDc, 0, 0, int( rRect.Width() ), int( rRect.Height() ), hDcMem, 0, 0, SRCCOPY );
		}
		::DeleteObject( hBmp );
		::DeleteDC( hDcMem );

		return lsw::CWidget::LSW_H_HANDLED;
	}

	/**
	 * The WM_SIZE handler.
	 *
	 * \param _wParam The type of resizing requested.
	 * \param _lWidth The new width of the client area.
	 * \param _lHeight The new height of the client area.
	 * \return Returns a LSW_HANDLED enumeration.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		UpdateScrollbars();
		return CParent::Size( _wParam, _lWidth, _lHeight );
	}

	/**
	 * Handles the WM_DPICHANGED message.
	 * 
	 * \param _wX The X-axis value of the new dpi of the window.
	 * \param _wY The Y-axis value of the new dpi of the window.
	 * \param _pRect A pointer to a RECT structure that provides a suggested size and position of the current window scaled for the new DPI. The expectation is that apps will reposition and resize windows based on the suggestions provided by lParam when handling this message.
	 * \return Returns an LSW_HANDLED code.
	 **/
	lsw::CWidget::LSW_HANDLED CHexEditorControl::DpiChanged( WORD _wX, WORD _wY, LPRECT _pRect ) {
		ChooseDefaultFont();
		RecalcAndInvalidate();
		return CParent::DpiChanged( _wX, _wY, _pRect );
	}

	// WM_LBUTTONDOWN.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::LButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) {
		this->SetFocus();                   // ensure keyboard focus lands here
		//::SetCapture( Wnd() );
		return lsw::CWidget::LSW_H_HANDLED;
	}

	/**
	 * The WM_KEYDOWN handler.
	 *
	 * \param _uiKeyCode The virtual-key code of the nonsystem key.
	 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
	 * \return Returns an LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::KeyDown( UINT _uiKeyCode, UINT /*_uiFlags*/ ) {
		const bool bCtrl = (::GetKeyState( VK_CONTROL ) & 0x8000) != 0;

		switch ( _uiKeyCode ) {
			case VK_UP :		{ ::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( SB_LINEUP, 0 ), 0 ); break; }
			case VK_DOWN :		{ ::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( SB_LINEDOWN, 0 ), 0 ); break; }
			case VK_PRIOR :		{ ::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( SB_PAGEUP, 0 ), 0 ); break; }
			case VK_NEXT :		{ ::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( SB_PAGEDOWN, 0 ), 0 ); break; }

			case VK_HOME : {
				if ( bCtrl ) { ::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( SB_TOP, 0 ), 0 ); }
				else { ::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_LEFT, 0 ), 0 ); }
				break;
			}

			case VK_END : {
				if ( bCtrl ) { ::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( SB_BOTTOM, 0 ), 0 ); }
				else { ::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_RIGHT, 0 ), 0 ); }
				break;
			}

			case VK_LEFT :		{ ::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_LINELEFT, 0 ), 0 ); break; }
			case VK_RIGHT :		{ ::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_LINERIGHT, 0 ), 0 ); break; }
		}


		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_HSCROLL
	lsw::CWidget::LSW_HANDLED CHexEditorControl::HScroll( USHORT /*_uScrollPos*/, USHORT _uScrollType, CWidget * /*_pwWidget*/ ) {
		SCROLLINFO siInfo { sizeof( siInfo ), SIF_ALL };
		::GetScrollInfo( Wnd(), SB_HORZ, &siInfo );
		int iPos = siInfo.nPos;

		switch ( _uScrollType ) {
			case SB_LINELEFT :		{ iPos -= 1; break; }
			case SB_LINERIGHT :		{ iPos += 1; break; }
			case SB_PAGELEFT :		{ iPos -= static_cast<int>(siInfo.nPage); break; }
			case SB_PAGERIGHT :		{ iPos += static_cast<int>(siInfo.nPage); break; }
			case SB_THUMBPOSITION :	{}		MX_FALLTHROUGH
			case SB_THUMBTRACK :	{ iPos = siInfo.nTrackPos; break; }
			case SB_LEFT :			{ iPos = siInfo.nMin; break; }
			case SB_RIGHT :			{ iPos = siInfo.nMax; break; }
		}

		if ( iPos < siInfo.nMin ) { iPos = siInfo.nMin; }
		if ( iPos > siInfo.nMax ) { iPos = siInfo.nMax; }
		if ( iPos != siInfo.nPos ) {
			m_iHPos = iPos;
			siInfo.fMask = SIF_POS;
			siInfo.nPos = iPos;
			::SetScrollInfo( Wnd(), SB_HORZ, &siInfo, TRUE );
			::InvalidateRect( Wnd(), NULL, FALSE );
		}
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_VSCROLL
	lsw::CWidget::LSW_HANDLED CHexEditorControl::VScroll( USHORT /*_uScrollPos*/, USHORT _uScrollType, CWidget * /*_pwWidget*/ ) {
		SCROLLINFO siInfo { sizeof( siInfo ), SIF_ALL };
		::GetScrollInfo( Wnd(), SB_VERT, &siInfo );
		int iPos = siInfo.nPos;

		switch ( _uScrollType ) {
			case SB_TOP :			{ iPos = siInfo.nMin; break; }
			case SB_BOTTOM :		{ iPos = siInfo.nMax; break; }
			case SB_LINEUP :		{ iPos -= 1; break; }
			case SB_LINEDOWN :		{ iPos += 1; break; }
			case SB_PAGEUP :		{ iPos -= static_cast<int>(siInfo.nPage); break; }
			case SB_PAGEDOWN :		{ iPos += static_cast<int>(siInfo.nPage); break; }
			case SB_THUMBPOSITION :	{}		MX_FALLTHROUGH
			case SB_THUMBTRACK :	{ iPos = siInfo.nTrackPos; break; }
		}

		if ( iPos < siInfo.nMin ) { iPos = siInfo.nMin; }
		if ( iPos > siInfo.nMax ) { iPos = siInfo.nMax; }
		if ( iPos != siInfo.nPos ) {
			m_iVPos = iPos;
			siInfo.fMask = SIF_POS;
			siInfo.nPos = iPos;
			::SetScrollInfo( Wnd(), SB_VERT, &siInfo, TRUE );
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_MOUSEWHEEL.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::MouseWheel( DWORD _dwVirtKeys, const POINTS &/*_pCursorPos*/ ) {
		UINT uiLines = 3;
		::SystemParametersInfoW( SPI_GETWHEELSCROLLLINES, 0, &uiLines, 0 );
		short sDelta = GET_WHEEL_DELTA_WPARAM( _dwVirtKeys );
		int iSteps = static_cast<int>((sDelta / WHEEL_DELTA) * static_cast<int>(uiLines));
		::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( (iSteps < 0) ? SB_LINEDOWN : SB_LINEUP, 0 ), 0 );
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_MOUSEHWHEEL.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::MouseHWheel( DWORD _dwVirtKeys, const POINTS &/*_pCursorPos*/ ) {
		short sDelta = GET_WHEEL_DELTA_WPARAM( _dwVirtKeys );
		int iSteps = (sDelta / WHEEL_DELTA);
		::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( (iSteps < 0) ? SB_LINERIGHT : SB_LINELEFT, 0 ), 0 );
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// Registers the control if it has not been registered already.  Redundant calls have no effect.  Must be called before creating this control.
	void CHexEditorControl::PrepareControl() {
		if ( !m_aAtom ) {
			lsw::CWndClassEx wceEx;
			wceEx.SetInstance( lsw::CBase::GetThisHandle() );
			WCHAR szStr[15];
			mx::CUtilities::RandomString( szStr, MX_ELEMENTS( szStr ) );
			wceEx.SetClassName( szStr );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(/*CTLCOLOR_DLG*/CTLCOLOR_STATIC + 1) );
			wceEx.SetWindPro( CWidget::WindowProc );
			m_aAtom = lsw::CBase::RegisterClassExW( wceEx.Obj() );
		}
	}

	// Draws the hex-editor view.
	bool CHexEditorControl::PaintHex( HDC _hDc, const lsw::LSW_RECT &_rRect ) {

		::FillRect( _hDc, &_rRect, reinterpret_cast<HBRUSH>(::GetStockObject( DKGRAY_BRUSH )) );

		return true;
	}

	// Updates the scrollbars.
	void CHexEditorControl::UpdateScrollbars() {
		m_iPageLines = (m_iCyChar ? (m_iClientH / m_iCyChar) : 0);
		if ( m_iPageLines < 1 ) { m_iPageLines = 1; }

		m_iPageCols = (m_iCxChar ? (m_iClientW / m_iCxChar) : 0);
		if ( m_iPageCols < 1 ) { m_iPageCols = 1; }

		// Vertical.
		SCROLLINFO siV {};
		siV.cbSize = sizeof( siV );
		siV.fMask  = SIF_PAGE | SIF_RANGE | SIF_POS;
		const uint64_t ullLines = TotalLines_Hex();
		const int iMaxV = ullLines > 0 ? static_cast<int>((ullLines - 1ULL) > 0x7FFFFFFFULL ? 0x7FFFFFFF : (ullLines - 1ULL)) : 0;
		if ( m_iVPos > iMaxV ) { m_iVPos = iMaxV; }
		siV.nMin	= 0;
		siV.nMax	= iMaxV;
		siV.nPos	= m_iVPos;
		siV.nPage	= static_cast<UINT>(m_iPageLines);
		::SetScrollInfo( m_hWnd, SB_VERT, &siV, TRUE );

		// Horizontal (columns).
		SCROLLINFO siH {};
		siH.cbSize	= sizeof( siH );
		siH.fMask	= SIF_PAGE | SIF_RANGE | SIF_POS;
		const int iCols = TotalColumns_Hex();
		const int iMaxH = iCols > 0 ? (iCols - 1) : 0;
		if ( m_iHPos > iMaxH ) { m_iHPos = iMaxH; }
		siH.nMin	= 0;
		siH.nMax	= iMaxH;
		siH.nPos	= m_iHPos;
		siH.nPage	= static_cast<UINT>(m_iPageCols);
		::SetScrollInfo( m_hWnd, SB_HORZ, &siH, TRUE );
	}

}	// namespace mx

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
	// WM_NCDESTROY.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::NcDestroy() {
		return CParent::NcDestroy();
	}

	// Setting the HWND after the control has been created.
	void CHexEditorControl::InitControl( HWND _hWnd ) {
		//m_iCxChar = m_iCyChar = 0;
		//m_iClientW = m_iClientH = 0;
		//m_hHex.sStyle.uiBytesPerRow = 16;
		/*m_pData = nullptr;
		m_ui64Size = 0ULL;*/
		/*m_iVPos = 0;
		m_iHPos = 0;*/
		m_iPageLines = m_iPageCols = 1;
		//m_iAddrDigits = 8;
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
				if ( bCtrl )	{ ::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( SB_TOP, 0 ), 0 ); }
				else			{ ::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_LEFT, 0 ), 0 ); }
				break;
			}

			case VK_END : {
				if ( bCtrl )	{ ::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( SB_BOTTOM, 0 ), 0 ); }
				else			{ ::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_RIGHT, 0 ), 0 ); }
				break;
			}

			case VK_LEFT :		{ ::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_LINELEFT, 0 ), 0 ); break; }
			case VK_RIGHT :		{ ::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_LINERIGHT, 0 ), 0 ); break; }
		}


		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_HSCROLL
	lsw::CWidget::LSW_HANDLED CHexEditorControl::HScroll( USHORT /*_uScrollPos*/, USHORT _uScrollType, CWidget * /*_pwWidget*/ ) {
		const int32_t iLineStepPx = (CurStyle()->iCharCx > 0) ? CurStyle()->iCharCx : 1;
		const int32_t iPagePx     = GetClientWidth();

		const uint64_t ui64ContentPx = static_cast<uint64_t>(TotalContentWidthPx());
		const uint64_t ui64MaxH      = ui64ContentPx ? (ui64ContentPx - 1ULL) : 0ULL;

		uint64_t ui64Pos = m_ui64HPx;

		switch ( _uScrollType ) {
			case SB_LINELEFT : {
				ui64Pos = (ui64Pos >= static_cast<uint64_t>(iLineStepPx)) ? (ui64Pos - static_cast<uint64_t>(iLineStepPx)) : 0ULL;
				break;
			}
			case SB_LINERIGHT : {
				const uint64_t ui64Add = static_cast<uint64_t>(iLineStepPx);
				ui64Pos = (ui64MaxH - ui64Pos > ui64Add) ? (ui64Pos + ui64Add) : ui64MaxH;
				break;
			}
			case SB_PAGELEFT : {
				ui64Pos = (ui64Pos >= static_cast<uint64_t>(iPagePx)) ? (ui64Pos - static_cast<uint64_t>(iPagePx)) : 0ULL;
				break;
			}
			case SB_PAGERIGHT : {
				const uint64_t ui64Add = static_cast<uint64_t>(iPagePx);
				ui64Pos = (ui64MaxH - ui64Pos > ui64Add) ? (ui64Pos + ui64Add) : ui64MaxH;
				break;
			}
			case SB_LEFT : {
				ui64Pos = 0ULL; break;
			}
			case SB_RIGHT : {
				ui64Pos = ui64MaxH; break;
			}
			case SB_THUMBPOSITION : {} MX_FALLTHROUGH
			case SB_THUMBTRACK : {
				ui64Pos = lsw::CBase::GetScrollPos64( Wnd(), SB_HORZ, SIF_TRACKPOS, ui64MaxH );
				break;
			}
		}

		if ( ui64Pos > ui64MaxH ) { ui64Pos = ui64MaxH; }
		if ( ui64Pos != m_ui64HPx ) {
			m_ui64HPx = ui64Pos;
			lsw::CBase::SetScrollInfo64( Wnd(), SB_HORZ, SIF_POS, ui64MaxH, m_ui64HPx, 1, TRUE );
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_VSCROLL
	lsw::CWidget::LSW_HANDLED CHexEditorControl::VScroll( USHORT /*_uScrollPos*/, USHORT _uScrollType, CWidget * /*_pwWidget*/ ) {
		const int32_t iPageLines = m_iPageLines;
		const int32_t iStep      = 1;

		const uint64_t ui64Lines = TotalLines_Hex();
		const uint64_t ui64MaxV  = ui64Lines ? (ui64Lines - 1ULL) : 0ULL;

		uint64_t ui64Pos = m_ui64VPos;

		switch ( _uScrollType ) {
			case SB_TOP :			{ ui64Pos = 0ULL; break; }
			case SB_BOTTOM :		{ ui64Pos = ui64MaxV; break; }
			case SB_LINEUP :		{ ui64Pos = (ui64Pos >= static_cast<uint64_t>(iStep)) ? (ui64Pos - static_cast<uint64_t>(iStep)) : 0ULL; break; }
			case SB_LINEDOWN :		{
				const uint64_t ui64Add = static_cast<uint64_t>(iStep);
				ui64Pos = (ui64MaxV - ui64Pos > ui64Add) ? (ui64Pos + ui64Add) : ui64MaxV;
				break;
			}
			case SB_PAGEUP :		{
				ui64Pos = (ui64Pos >= static_cast<uint64_t>(iPageLines)) ? (ui64Pos - static_cast<uint64_t>(iPageLines)) : 0ULL;
				break;
			}
			case SB_PAGEDOWN :		{
				const uint64_t ui64Add = static_cast<uint64_t>(iPageLines);
				ui64Pos = (ui64MaxV - ui64Pos > ui64Add) ? (ui64Pos + ui64Add) : ui64MaxV;
				break;
			}
			case SB_THUMBPOSITION :	{} MX_FALLTHROUGH
			case SB_THUMBTRACK :	{
				ui64Pos = lsw::CBase::GetScrollPos64( Wnd(), SB_VERT, SIF_TRACKPOS, ui64MaxV );
				break;
			}
		}

		if ( ui64Pos > ui64MaxV ) { ui64Pos = ui64MaxV; }
		if ( ui64Pos != m_ui64VPos ) {
			m_ui64VPos = ui64Pos;

			// Keep style in sync with the new top visible line.
			const_cast<MX_STYLE *>(CurStyle())->daAddressStyle.ui64FirstVisibleLine = m_ui64VPos;

			lsw::CBase::SetScrollInfo64( Wnd(), SB_VERT, SIF_POS, ui64MaxV, m_ui64VPos, 1, TRUE );
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}
		return lsw::CWidget::LSW_H_HANDLED;
	}


	// WM_MOUSEWHEEL.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::MouseWheel( DWORD _dwVirtKeys, const POINTS &/*_pCursorPos*/ ) {
		UINT uiLines = 3;
		::SystemParametersInfoW( SPI_GETWHEELSCROLLLINES, 0, &uiLines, 0 );
		int16_t sDelta = GET_WHEEL_DELTA_WPARAM( _dwVirtKeys );
		int32_t iSteps = static_cast<int32_t>((sDelta / WHEEL_DELTA) * static_cast<int32_t>(uiLines));
		::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( (iSteps < 0) ? SB_LINEDOWN : SB_LINEUP, 0 ), 0 );
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_MOUSEHWHEEL.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::MouseHWheel( DWORD _dwVirtKeys, const POINTS &/*_pCursorPos*/ ) {
		int16_t sDelta = GET_WHEEL_DELTA_WPARAM( _dwVirtKeys );
		int32_t iSteps = (sDelta / WHEEL_DELTA);
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

		lsw::LSW_RECT rTmp = _rRect;
		rTmp.right = rTmp.left + ComputeAddressGutterWidthPx() + 3;
		::FillRect( _hDc, &rTmp,
			lsw::CBase::BrushCache().Brush( RGB( 0x23, 0x22, 0x20 ) )
			/*reinterpret_cast<HBRUSH>(::GetStockObject( DKGRAY_BRUSH ))*/ );	// RGB( 0x23, 0x22, 0x20 )

		DrawAddressGutter( _hDc, 3, 0, m_iPageLines + 1 );
		return true;
	}

	/**
	 * Draws the address gutter starting at a given Y position.
	 *
	 * \param _hDc The destination HDC. The caller does not need to select the font; this function selects CurStyle()->fFont.hFont.
	 * \param _iXLeft The pixel X of the gutterÅfs left edge.
	 * \param _iYTop The pixel Y at which to draw the first visible line.
	 * \param _ui32LinesToDraw The number of rows to draw (typically page lines).
	 *
	 * Description: Renders the left address area using the current MX_STYLE obtained via CurStyle().
	 *  Honors address format (bytes/shorts/line-number; hex/dec/oct), lowercase hex, internal grouping
	 *  colons every 4 hex digits, optional trailing colon, optional type suffix ('h'/'o'), and optional
	 *  short suffix ('w'). Pads hex/oct to the minimum digits with '0'; pads decimal/line-number on the
	 *  left with spaces to align to the same minimum digit count.
	 *  The gutter is fixed (non-scrolling horizontally); vertical positioning uses the controlÅfs line
	 *  height (m_iCyChar if set, otherwise the measured font metrics). Text is drawn with transparent
	 *  background using CurStyle()->daAddressStyle.crText.
	 */
	void CHexEditorControl::DrawAddressGutter( HDC _hDc, int _iXLeft, int _iYTop, uint32_t _ui32LinesToDraw ) {
		const MX_STYLE & stAll = (*CurStyle());
		if ( !stAll.bShowAddressGutter || _ui32LinesToDraw == 0 ) { return; }

		const MX_ADDR_STYLE & st = stAll.daAddressStyle;

		// Select font and ensure glyph metrics.
		lsw::LSW_SELECTOBJECT soFont( _hDc, stAll.fFont.hFont );
		EnsureAddrGlyphs( _hDc );
		const MX_ADDR_GLYPHS & g = stAll.agGlyphs;

		::SetBkMode( _hDc, TRANSPARENT );
		::SetTextColor( _hDc, st.crText );

		// Helpers: convert value to string in base, then apply options.
		auto ToBase = [&]( uint64_t _ui64V, uint32_t _ui32Base, bool _bLower ) -> std::wstring {
			if ( _ui64V == 0 ) { return L"0"; }
			std::wstring s;
			while ( _ui64V ) {
				const uint32_t d = static_cast<uint32_t>(_ui64V % _ui32Base);
				wchar_t c = 0;
				if ( d < 10 ) { c = wchar_t( L'0' + d ); }
				else { c = wchar_t( (_bLower ? L'a' : L'A') + (d - 10) ); }
				s.push_back( c );
				_ui64V /= _ui32Base;
			}
			std::reverse( s.begin(), s.end() );
			return s;
		};

		auto InsertHexGroupingColons = [&]( std::wstring & _s ) {
			if ( !st.bShowColonIn || _s.size() <= 4 ) { return; }
			std::wstring grouped;
			size_t r = _s.size();
			int cnt = 0;
			while ( r-- ) {
				grouped.push_back( _s[r] );
				if ( (++cnt % 4) == 0 && r != SIZE_MAX ) {
					grouped.push_back( L':' );
				}
			}
			std::reverse( grouped.begin(), grouped.end() );
			_s.swap( grouped );
		};

		// Per-line formatter: computes the address string shown for a given line index.
		auto FormatAddressForLine = [&]( uint64_t _ui64Line ) -> std::wstring {
			// Base byte index for the start of this line.
			const uint64_t ui64Bpr = stAll.uiBytesPerRow ? stAll.uiBytesPerRow : 16;
			const uint64_t ui64ByteBase = st.ui64StartAddress + _ui64Line * ui64Bpr;

			// Map to displayed scalar according to format.
			uint64_t v = 0ULL;
			bool bHex = false, bOct = false, bDec = false;
			switch( st.afFormat ) {
				case MX_AF_BYTES_HEX :		{ v = ui64ByteBase; bHex = true; break; }
				case MX_AF_BYTES_DEC :		{ v = ui64ByteBase; bDec = true; break; }
				case MX_AF_BYTES_OCT :		{ v = ui64ByteBase; bOct = true; break; }
				case MX_AF_LINE_NUMBER :	{ v = _ui64Line;    bDec = true; break; }
				case MX_AF_SHORT_HEX :		{ v = (ui64ByteBase >> 1); bHex = true; break; }
				case MX_AF_SHORT_DEC :		{ v = (ui64ByteBase >> 1); bDec = true; break; }
				default :					{ v = ui64ByteBase; bHex = true; break; }
			}

			// Convert to string in the selected base.
			std::wstring core;
			if ( bHex ) { core = ToBase( v, 16, st.bLowercaseHex ); }
			else if ( bOct ) { core = ToBase( v, 8, false ); }
			else { core = ToBase( v, 10, false ); }

			// Minimum digits: hex/oct padded with '0'; decimal padded with spaces on the left.
			uint32_t ui32MinDigits = MinAddrDigits();
			if ( ui32MinDigits && core.size() < ui32MinDigits ) {
				const size_t pad = ui32MinDigits - core.size();
				if ( bHex || bOct ) {
					core.insert( core.begin(), pad, L'0' );
				}
				else { // decimal / line-number
					core.insert( core.begin(), pad, L' ' );
				}
			}

			// Internal grouping with ':' every 4 hex digits (after zero-padding).
			if ( bHex ) {
				InsertHexGroupingColons( core );
			}

			// Trailing colon after the address if enabled.
			if ( st.bShowColonAfter ) {
				core.push_back( L':' );
			}

			// Optional type specifier.
			if ( st.bShowTypeSpec ) {
				if ( bHex ) { core.push_back( L'h' ); }
				else if ( bOct ) { core.push_back( L'o' ); }
			}

			// Optional short suffix 'w' for Short addressing.
			if ( st.bUseShortSuffixW && (st.afFormat == MX_AF_SHORT_HEX || st.afFormat == MX_AF_SHORT_DEC) ) {
				core.push_back( L'w' );
			}

			return core;
		};

		// Line height.
		const int iLineAdv = CurStyle()->iCharCy + stAll.iLineSpacingPx;

		// First visible line (top) comes from style; draw consecutive lines.
		uint64_t ui64Line = st.ui64FirstVisibleLine;

		auto ui64MaxLines = TotalLines() - ui64Line;
		_ui32LinesToDraw = uint32_t( std::min<uint64_t>( _ui32LinesToDraw, ui64MaxLines ) );
		std::wstring ws;
		for( uint32_t i = 0; i < _ui32LinesToDraw; ++i, ++ui64Line ) {
			ws = FormatAddressForLine( ui64Line );
			const int iY = _iYTop + int( i ) * iLineAdv;

			// Decimal is right-aligned by spaces we injected above; just draw at left edge.
			::TextOutW( _hDc, _iXLeft, iY, ws.c_str(), static_cast<int>(ws.size()) );
		}
	}

	/**
	 * Computes the pixel width of the address gutter for the current options and font.
	 *
	 * \return Returns the pixel width of the address gutter. Returns 0 if the gutter is hidden.
	 */
	int CHexEditorControl::ComputeAddressGutterWidthPx() {
		if ( !CurStyle()->bShowAddressGutter ) { return 0; }

		const MX_STYLE & stAll = (*CurStyle());
		const MX_ADDR_STYLE & asAddrStyle = stAll.daAddressStyle;
		const uint64_t ui64FileSize = Size();
		int iWidth = 0;
		{
			lsw::LSW_HDC hDc( Wnd() );
			{
				lsw::LSW_SELECTOBJECT soFont( hDc.hDc, stAll.fFont.hFont );
				EnsureAddrGlyphs( hDc.hDc );
				const MX_ADDR_GLYPHS & agGlyphs = stAll.agGlyphs;

				// File/page bound (last visible byte vs last file byte).
				const uint64_t ui64Bpr       = stAll.uiBytesPerRow ? stAll.uiBytesPerRow : 16;
				const uint64_t ui64FileLast  = (ui64FileSize ? (ui64FileSize - 1ULL) : 0ULL);
				const uint64_t ui64FileMax   = asAddrStyle.ui64StartAddress + ui64FileLast;

				const uint64_t ui64TopLine   = asAddrStyle.ui64FirstVisibleLine;
				const uint64_t ui64LastLine  = ui64TopLine + (asAddrStyle.ui32VisibleLines ? (asAddrStyle.ui32VisibleLines - 1ULL) : 0ULL);
				const uint64_t ui64PageLast  = asAddrStyle.ui64StartAddress + (ui64LastLine * ui64Bpr + (ui64Bpr ? (ui64Bpr - 1ULL) : 0ULL));
				const uint64_t ui64BoundByte = asAddrStyle.bMinimizeDigits ? std::min( ui64PageLast, ui64FileMax ) : ui64FileMax;

				// Map bound to displayed scalar by format.
				auto MapToDisplayed = [&]( uint64_t _ui64Byte ) -> uint64_t {
					switch ( asAddrStyle.afFormat ) {
						case MX_AF_BYTES_HEX :
						case MX_AF_BYTES_DEC :
						case MX_AF_BYTES_OCT : return _ui64Byte;
						case MX_AF_LINE_NUMBER : {
							if ( stAll.uiBytesPerRow == 0 ) { return 0ULL; }
							const uint64_t ui64Lines = (ui64FileSize + stAll.uiBytesPerRow - 1ULL) / stAll.uiBytesPerRow;
							return ui64Lines ? (ui64Lines - 1ULL) : 0ULL;
						}
						case MX_AF_SHORT_HEX :
						case MX_AF_SHORT_DEC : return (_ui64Byte >> 1);
						default : return _ui64Byte;
					}
				};

				const uint64_t ui64Disp = MapToDisplayed( ui64BoundByte );

				// Base/digits.
				bool bHex = false, bOct = false, bDec = false;
				uint32_t ui32Base = 10;
				switch ( asAddrStyle.afFormat ) {
					case MX_AF_BYTES_HEX :
					case MX_AF_SHORT_HEX : bHex = true; ui32Base = 16; break;
					case MX_AF_BYTES_OCT : bOct = true; ui32Base = 8;  break;
					case MX_AF_BYTES_DEC :
					case MX_AF_SHORT_DEC :
					case MX_AF_LINE_NUMBER : default : bDec = true; ui32Base = 10; break;
				}
				uint32_t ui32Digits = DigitCount( ui64Disp, ui32Base );
				uint32_t ui32MinDigits = MinAddrDigits();
				if ( ui32MinDigits ) { ui32Digits = std::max( ui32Digits, ui32MinDigits ); }

				iWidth = int( ui32Digits ) * agGlyphs.iDigitMaxCx;

				// Internal grouping (HEX): ':' every 4 hex digits.
				if ( bHex && asAddrStyle.bShowColonIn && ui32Digits > 4 ) {
					const uint32_t ui32Groups = (ui32Digits - 1U) / 4U;
					iWidth += int( ui32Groups ) * agGlyphs.iColonCx;
				}
				// Trailing colon.
				if ( asAddrStyle.bShowColonAfter ) { iWidth += agGlyphs.iColonCx; }
				// Type spec 'h'/'o'.
				if ( asAddrStyle.bShowTypeSpec ) {
					if ( bHex ) { iWidth += agGlyphs.iSpecHexCx; }
					else if ( bOct ) { iWidth += agGlyphs.iSpecOctCx; }
				}
				// Short suffix 'w'.
				if ( asAddrStyle.bUseShortSuffixW && (asAddrStyle.afFormat == MX_AF_SHORT_HEX || asAddrStyle.afFormat == MX_AF_SHORT_DEC) ) {
					iWidth += agGlyphs.iShortWcx;
				}
				// Optional left-space padding for right-aligned DEC up to MinDigits (if you do that).
				if ( bDec && ui32MinDigits && ui32Digits < ui32MinDigits ) {
					const uint32_t ui32Pads = ui32MinDigits - ui32Digits;
					iWidth += int( ui32Pads ) * agGlyphs.iSpaceCx;
				}
			}
		}

		return iWidth + CurStyle()->iPadAfterGutterPx;
	}

	/**
	 * Computes the pixel width of the left numeric (hex/dec/oct/bin/char) column for one row.
	 *
	 * \return Returns the pixel width of the left numbers block; 0 if hidden.
	 */
	int CHexEditorControl::ComputeLeftNumbersWidthPx() {
		const MX_STYLE & asAddrStyle = (*CurStyle());
		if ( !asAddrStyle.bShowLeftNumbers ) { return 0; }
			
		int iCx = 0;
		{
			lsw::LSW_HDC hDc( Wnd() );
			{
				lsw::LSW_SELECTOBJECT soFont( hDc.hDc, asAddrStyle.fFont.hFont );
				EnsureAddrGlyphs( hDc.hDc );
				const MX_ADDR_GLYPHS & agGlyphs = asAddrStyle.agGlyphs;

				auto CellWidthForFmt = [&]( MX_DATA_FMT _df ) -> int {
					switch ( _df ) {
						case MX_DF_HEX :	{ return 2 * agGlyphs.iDigitMaxCx; }	// "FF"
						case MX_DF_DEC :	{ return 3 * agGlyphs.iDigitMaxCx; }	// "255"
						case MX_DF_OCT :	{ return 3 * agGlyphs.iDigitMaxCx; }	// "377"
						case MX_DF_BIN :	{ return 8 * agGlyphs.iDigitMaxCx; }	// "11111111"
						case MX_DF_CHAR :	{ return agGlyphs.iDigitMaxCx; }		// fixed cell (refine if you later measure ASCII separately)
						default :			{ return 2 * agGlyphs.iDigitMaxCx; }
					}
				};

				const uint32_t ui32Bpr		= asAddrStyle.uiBytesPerRow ? asAddrStyle.uiBytesPerRow : 16;
				const uint32_t ui32GroupSz	= asAddrStyle.uiGroupSize ? asAddrStyle.uiGroupSize : 1;

				const int iCellCx			= CellWidthForFmt( asAddrStyle.dfLeftNumbersFmt );
				const int iSpaceB			= int( asAddrStyle.uiSpacesBetweenBytes ) * agGlyphs.iSpaceCx;
				const int iSpaceGrp			= int( asAddrStyle.uiExtraSpacesBetweenGroups ) * agGlyphs.iSpaceCx;

				int iTotal = asAddrStyle.iPadNumbersLeftPx;
				for ( uint32_t I = 0; I < ui32Bpr; ++I ) {
					iTotal += iCellCx;
					if ( I + 1 < ui32Bpr ) {
						iTotal += iSpaceB;
						if ( ui32GroupSz > 1 && ((I + 1) % ui32GroupSz) == 0 ) {
							iTotal += iSpaceGrp;
						}
					}
				}
				iTotal += asAddrStyle.iPadNumbersRightPx;

				iCx = iTotal;
			}
		}
		return iCx;
	}

	/**
	 * Computes the pixel width of the right ASCII/text column for one row.
	 *
	 * \return Returns the pixel width of the text block; 0 if hidden.
	 */
	int CHexEditorControl::ComputeRightTextWidthPx() {
		const MX_STYLE & asAddrStyle = (*CurStyle());
		if ( !asAddrStyle.bShowRightText ) { return 0; }

		int iCx = 0;
		{
			lsw::LSW_HDC hDc( Wnd() );
			lsw::LSW_SELECTOBJECT soFont( hDc.hDc, asAddrStyle.fFont.hFont );
			EnsureAddrGlyphs( hDc.hDc );
			const MX_ADDR_GLYPHS & agGlyphs = asAddrStyle.agGlyphs;

			const uint32_t ui32Bpr = asAddrStyle.uiBytesPerRow ? asAddrStyle.uiBytesPerRow : 16;

			int iTotal = 0;
			iTotal += asAddrStyle.iPadBetweenNumbersAndTextPx;     // gap from left block
			iTotal += static_cast<int>(ui32Bpr) * agGlyphs.iDigitMaxCx; // fixed per-char cell
			iTotal += asAddrStyle.iPadTextRightPx;

			iCx = iTotal;
		}
		return iCx;
	}

	// Gets the total number of lines based on which view type is active.
	uint64_t CHexEditorControl::TotalLines() const {
		switch ( m_eaEditAs ) {
			case MX_ES_HEX : { return TotalLines_Hex(); }
		}
		return 0;
	}

	// Updates the scrollbars.
	void CHexEditorControl::UpdateScrollbars() {
		// Compute visible page sizes (lines / columns-on-screen).
		int iClientH = int( ClientRect().Height() );

		const int iLineAdv = LineAdvanceCy();
		m_iPageLines = (iLineAdv ? (iClientH / iLineAdv) : 0);
		if ( m_iPageLines < 1 ) { m_iPageLines = 1; }

		m_iPageCols = (CurStyle()->iCharCx ? (iClientH / CurStyle()->iCharCx) : 0);
		if ( m_iPageCols < 1 ) { m_iPageCols = 1; }

		// == Vertical (lines; virtual units are line indices 0..N-1).
		const uint64_t ui64Lines = TotalLines();
		const uint64_t ui64MaxV  = ui64Lines ? (ui64Lines - 1ULL) : 0ULL;	// inclusive; allow last line at top
		if ( m_ui64VPos > ui64MaxV ) { m_ui64VPos = ui64MaxV; }

		// Keep style in sync.
		const_cast<MX_STYLE *>(CurStyle())->daAddressStyle.ui64FirstVisibleLine = m_ui64VPos;
		const_cast<MX_STYLE *>(CurStyle())->daAddressStyle.ui32VisibleLines     = static_cast<uint32_t>(m_iPageLines);

		// Physical page = 1 so the thumb can reach the end.
		lsw::CBase::SetScrollInfo64( m_hWnd, SB_VERT, SIF_PAGE | SIF_RANGE | SIF_POS,
			ui64MaxV, m_ui64VPos, 1, TRUE );

		// == Horizontal (pixels; virtual units are pixel offsets 0..contentPx-1).
		const uint64_t ui64ContentPx = static_cast<uint64_t>(TotalContentWidthPx());
		const uint64_t ui64MaxH      = ui64ContentPx ? (ui64ContentPx - 1ULL) : 0ULL; // inclusive; allow last pixel at left
		if ( m_ui64HPx > ui64MaxH ) { m_ui64HPx = ui64MaxH; }

		// Physical page = 1 for the same reason; weÅfll use client width for paging behavior.
		lsw::CBase::SetScrollInfo64( m_hWnd, SB_HORZ, SIF_PAGE | SIF_RANGE | SIF_POS,
			ui64MaxH, m_ui64HPx, 1, TRUE );
	}


}	// namespace mx

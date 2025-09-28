#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXStringDecoder.h"
#include "MXHexEditorInterface.h"

#include <LSWWin.h>
#include <Helpers/LSWHelpers.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>


namespace mx {

	class CHexEditorControl : public lsw::CWidget {
	public :
		CHexEditorControl( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, lsw::CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Enumerations.
		/** View types. */
		enum MX_EDIT_AS {
			MX_ES_TEXT,
			MX_ES_HEX,
			MX_ES_BINARY,
			MX_ES_SCRIPT,
			MX_ES_TEMPLATE,
			MX_ES_EBCDIC,
			MX_ES_UTF16,
			MX_ES_UTF8,
			MX_ES_PROCESS,
			MX_ES_CUR_PROCESS,
			MX_ES_CODE,
			MX_ES_TAGGED,
		};

		/** Address formats. */
		enum MX_ADDRESS_FMT {
			MX_AF_BYTES_HEX,						// Example: 1:1D9E:53C8
			MX_AF_BYTES_DEC,						// Example: 6562337568 (right-aligned, padded with spaces on the left)
			MX_AF_BYTES_OCT,						// Example: 60411251440
			MX_AF_LINE_NUMBER,						// Example: 820292197 (right-aligned, padded with spaces on the left)
			MX_AF_SHORT_HEX,						// Example: C392:A990w
			MX_AF_SHORT_DEC,						// Example: 2395961428w (right-aligned, padded with spaces on the left)
		};

		/** Data display for the left (numbers) and right (text) columns. */
		enum MX_DATA_FMT {
			MX_DF_HEX,								// "00 FF 2A"
			MX_DF_DEC,								// "000 255 042"
			MX_DF_OCT,								// "000 377 052"
			MX_DF_BIN,								// Optional: "00000000"
			MX_DF_CHAR								// Textual: printable byte or '.'
		};



		// == Functions.
		// Sets the view type.
		void										SetViewType( MX_EDIT_AS _eaType ) {
			m_ui64VPos = 0;
			m_ui64HPx = 0;
			m_eaEditAs = _eaType;
		}

		// WM_NCDESTROY.
		virtual LSW_HANDLED							NcDestroy();

		// Setting the HWND after the control has been created.
		virtual void								InitControl( HWND _hWnd );

		/**
		 * The WM_GETDLGCODE handler.
		 * 
		 * \param _wKey The virtual key, pressed by the user, that prompted Windows to issue this notification. The handler must selectively handle these keys. For instance, the handler might accept and process VK_RETURN but delegate VK_TAB to the owner window.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual WORD								GetDlgCode( WORD _wKey );

		// WM_ERASEBKGND.
		virtual LSW_HANDLED							EraseBkgnd( HDC _hDc ) { return LSW_H_HANDLED; }

		// WM_PAINT.
		virtual LSW_HANDLED							Paint();

		/**
		 * The WM_SIZE handler.
		 *
		 * \param _wParam The type of resizing requested.
		 * \param _lWidth The new width of the client area.
		 * \param _lHeight The new height of the client area.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED							Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		/**
		 * Handles the WM_DPICHANGED message.
		 * 
		 * \param _wX The X-axis value of the new dpi of the window.
		 * \param _wY The Y-axis value of the new dpi of the window.
		 * \param _pRect A pointer to a RECT structure that provides a suggested size and position of the current window scaled for the new DPI. The expectation is that apps will reposition and resize windows based on the suggestions provided by lParam when handling this message.
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED							DpiChanged( WORD _wX, WORD _wY, LPRECT _pRect );

		/**
		 * The WM_GETFONT handler.
		 *
		 * \param Returns -1 to perform the default routine, otherwise NULL or a handle to a font.
		 */
		virtual HFONT								GetFont() { return CurStyle()->fFont.hFont; }

		// WM_LBUTTONDOWN.
		virtual LSW_HANDLED							LButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ );

		/**
		 * The WM_KEYDOWN handler.
		 *
		 * \param _uiKeyCode The virtual-key code of the nonsystem key.
		 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							KeyDown( UINT _uiKeyCode, UINT /*_uiFlags*/ );

		// WM_HSCROLL
		virtual LSW_HANDLED							HScroll( USHORT /*_uScrollPos*/, USHORT _uScrollType, CWidget * /*_pwWidget*/ );

		// WM_VSCROLL
		virtual LSW_HANDLED							VScroll( USHORT /*_uScrollPos*/, USHORT _uScrollType, CWidget * /*_pwWidget*/ );

		// WM_MOUSEWHEEL.
		virtual LSW_HANDLED							MouseWheel( DWORD _dwVirtKeys, const POINTS &/*_pCursorPos*/ );

		// WM_MOUSEHWHEEL.
		virtual LSW_HANDLED							MouseHWheel( DWORD _dwVirtKeys, const POINTS &/*_pCursorPos*/ );

		// Translate a child's tooltip text.
		virtual std::wstring						TranslateTooltip( const std::string &_sText ) { 
			return CStringDecoder::DecodeToWString( _sText.c_str(), _sText.size() );
		}

		// Sets the target stream.
		inline void									SetStream( CHexEditorInterface * _pediStream ) { m_pheiTarget = _pediStream; RecalcAndInvalidate(); }

		// Gets the size of the handled data.
		inline uint64_t								Size() const { return m_pheiTarget ? m_pheiTarget->Size() : 0; }

		// Registers the control if it has not been registered already.  Redundant calls have no effect.  Must be called before creating this control.
		static void									PrepareControl();


	protected :
		// == Types.
		/** What to draw and how to size the address gutter. */
		struct MX_ADDR_STYLE {
			uint64_t								ui64StartAddress = 0;				// Starting address bias (Set Starting Address).
			uint64_t								ui64FirstVisibleLine = 0;			// Top visible line index (0-based).
			//uint32_t								ui32MinAddrDigits = 4;				// Clamp minimum digits (HEX: 4..16 typical).

			uint32_t								ui32VisibleLines = 0;				// Count of visible lines.

			COLORREF								crText = RGB( 0x92, 0x92, 0x92 );	// Text color (use your theme).
			MX_ADDRESS_FMT							afFormat = MX_AF_BYTES_HEX;			// Address format.
			bool									bLowercaseHex = false;				// Use lowercase a..f for HEX.
			bool									bShowColonIn = true;				// Insert ':' every 4 HEX digits (>4 only).
			bool									bShowColonAfter = false;			// Append ':' after the address.
			bool									bShowTypeSpec = false;				// Append 'h' (hex) or 'o' (oct).
			bool									bMinimizeDigits = false;			// Size digits for current page, not whole file.
			bool									bUseShortSuffixW = true;			// Append 'w' for Short addressing (matches your examples).
		};

		/** Measured glyph metrics for the address gutter font. */
		struct MX_ADDR_GLYPHS {
			int										iDigitMaxCx = 0;					// Max over '0'..'9' (and 'a'..'f' or 'A'..'F' when HEX).
			int										iColonCx = 0;						// Width of ':'.
			int										iSpaceCx = 0;						// Width of ' '.
			int										iSpecHexCx = 0;						// Width of 'h'.
			int										iSpecOctCx = 0;						// Width of 'o'.
			int										iShortWcx = 0;						// Width of 'w'.
		};

		/** General style settings. */
		struct MX_STYLE {
			lsw::LSW_FONT							fFont;								// Current font.
			MX_ADDR_STYLE							daAddressStyle;						// Address style.
			MX_ADDR_GLYPHS							agGlyphs;							// Glyph settings (filled on demand).
			uint32_t								uiBytesPerRow = 16;					// Bytes per displayed row.
			uint64_t								ui64FileSize = 0;					// File size in bytes (needed for address digits).

			// Visibility.
			bool									bShowAddressGutter = true;
			bool									bShowLeftNumbers   = true;			// Hex/Dec/Oct/Bin column.
			bool									bShowRightText     = true;			// ASCII column.
			bool									bShowMiniMap       = false;			// Fixed panel on right.

			// Left numbers formatting.
			MX_DATA_FMT								dfLeftNumbersFmt = MX_DF_HEX;
			uint32_t								uiGroupSize = 8;                    // Extra spacing after every N bytes.
			uint32_t								uiSpacesBetweenBytes = 1;           // Count of ' ' between adjacent bytes.
			uint32_t								uiExtraSpacesBetweenGroups = 1;     // Extra ' ' at group boundary.

			// Paddings/gaps (pixels).
			int										iPadAfterGutterPx = 8;
			int										iPadBetweenNumbersAndTextPx = 16;
			int										iPadNumbersLeftPx = 0;
			int										iPadNumbersRightPx = 0;
			int										iPadTextRightPx = 0;
			int										iPadScrollableLeftPx = 0;
			int										iPadScrollableRightPx = 0;
			int										iPadBeforeMiniMapPx = 8;
			int										iLineSpacingPx = 2;
			int										iCharCx = 0;
			int										iCharCy = 0;						// Baseline advance

			// Mini-map geometry (pixels).
			int										iMiniMapWidthPx = 140;
		};


		/** The hex-view information. */
		struct MX_HEX {
			MX_STYLE								sStyle;								// Style settings for the Hex View.
		};


		// == Members.
		uint64_t									m_ui64VPos = 0;						// First visible line (virtual).
		uint64_t									m_ui64HPx = 0;						// First visible column (virtual).
		MX_EDIT_AS									m_eaEditAs = MX_ES_HEX;				// The view type.

		CHexEditorInterface *						m_pheiTarget = nullptr;				// The stream of data to handle.
		MX_HEX										m_hHex;								// Hex view settings.
		MX_STYLE *									m_pwCurStyle = nullptr;				// The current style.
		//int											m_iCxChar;							// Char width in pixels.
		//int											m_iCyChar;							// Char height in pixels.
		//int											m_iClientW;							// Client width.
		//int											m_iClientH;							// Client height.
		int											m_iPageLines;						// How many text rows fit.
		int											m_iPageCols;						// How many text columns fit.
		//int											m_iAddrDigits;						// 8 or 16.


		// The main window class.
		static ATOM									m_aAtom;


		// == Functions.
		// Gets the current style settings.
		MX_STYLE *									CurStyle() { return m_pwCurStyle ? m_pwCurStyle : &m_hHex.sStyle; }

		// Gets the current style settings.
		const MX_STYLE *							CurStyle() const { return m_pwCurStyle ? m_pwCurStyle : &m_hHex.sStyle; }

		// Draws the hex-editor view.
		bool										PaintHex( HDC _hDc, const lsw::LSW_RECT &_rRect );

		/**
		 * Draws the address gutter starting at a given Y position.
		 *
		 * \param _hDc The destination HDC. The caller does not need to select the font; this function selects CurStyle()->fFont.hFont.
		 * \param _iXLeft The pixel X of the gutterfs left edge.
		 * \param _iYTop The pixel Y at which to draw the first visible line.
		 * \param _ui32LinesToDraw The number of rows to draw (typically page lines).
		 *
		 * Description: Renders the left address area using the current MX_STYLE obtained via CurStyle().
		 *  Honors address format (bytes/shorts/line-number; hex/dec/oct), lowercase hex, internal grouping
		 *  colons every 4 hex digits, optional trailing colon, optional type suffix ('h'/'o'), and optional
		 *  short suffix ('w'). Pads hex/oct to the minimum digits with '0'; pads decimal/line-number on the
		 *  left with spaces to align to the same minimum digit count.
		 *  The gutter is fixed (non-scrolling horizontally); vertical positioning uses the controlfs line
		 *  height (m_iCyChar if set, otherwise the measured font metrics). Text is drawn with transparent
		 *  background using CurStyle()->daAddressStyle.crText.
		 */
		void										DrawAddressGutter( HDC _hDc, int _iXLeft, int _iYTop, uint32_t _ui32LinesToDraw );

		// Computes font metrics.
		void										ComputeFontMetrics() {
			lsw::LSW_HDC hDc( Wnd() );
			{
				lsw::LSW_SELECTOBJECT soFontOrig( hDc.hDc, CurStyle()->fFont.hFont );	// Destructor sets the original font back.
				TEXTMETRICW tmMetrics {};
				::GetTextMetricsW( hDc.hDc, &tmMetrics );
				CurStyle()->iCharCx = tmMetrics.tmAveCharWidth;
				CurStyle()->iCharCy = tmMetrics.tmHeight + tmMetrics.tmExternalLeading;
			}
		}

		// Sets a default font.
		void										ChooseDefaultFont() {
			static const wchar_t * s_ppwszFaces[] = {
				L"Droid Sans Mono",   // preferred
				L"Consolas",          // modern Windows TT
				L"Lucida Console"     // older Windows TT
			};

			const int iPt = 10;

			LOGFONTW lfFont {};
			lfFont = lsw::CBase().NonClientMetrics().lfMessageFont;
			lfFont.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
			lfFont.lfWeight = FW_NORMAL;//FW_BOLD;
			lfFont.lfItalic = FALSE;
			lfFont.lfOutPrecision = OUT_TT_PRECIS;

			lfFont.lfQuality = PROOF_QUALITY;
#if ( WINVER >= 0x0400 )
			lfFont.lfQuality = ANTIALIASED_QUALITY;
#endif	// #if ( WINVER >= 0x0400 )
#if ( _WIN32_WINNT >= _WIN32_WINNT_WINXP )
			lfFont.lfQuality = CLEARTYPE_NATURAL_QUALITY;
#endif	// #if ( _WIN32_WINNT >= _WIN32_WINNT_WINXP )

			lfFont.lfCharSet = DEFAULT_CHARSET;
			lfFont.lfHeight = -::MulDiv( iPt, static_cast<int>(m_wDpiY), 72 );

			HDC hDc = ::GetDC( Wnd() );
			for ( size_t i = 0; i < _countof( s_ppwszFaces ); ++i ) {
				::wcscpy_s( lfFont.lfFaceName, s_ppwszFaces[i] );

				// Create the candidate.
				//lsw::LSW_FONT fTry;
				if ( CurStyle()->fFont.CreateFontIndirectW( &lfFont ) ) {
					// Verify it is TrueType by asking for outline metrics.
					lsw::LSW_SELECTOBJECT soFont( hDc, CurStyle()->fFont.hFont );

					// Two-call pattern to see if OTM exists (TrueType/OpenType only).
					UINT uNeed = ::GetOutlineTextMetricsW( hDc, 0, NULL );
					if ( uNeed != 0 ) {
						// Success: keep this one.
						/*CurStyle()->fFont.Reset();
						CurStyle()->fFont = fTry;
						CurStyle()->fFont.bDeleteIt = true;*/
						break;
					}
					// else: not TrueType, try next fallback.
				}
			}
			::ReleaseDC( Wnd(), hDc );
		}

		/**
		 * \brief Returns the vertical advance, in pixels, between consecutive rows.
		 *
		 * Description: Base line height (m_iCyChar or measured glyph height) plus CurStyle()->iLineSpacingPx.
		 */
		int											LineAdvanceCy() const {
			return CurStyle()->iCharCy + CurStyle()->iLineSpacingPx;
		}

		// Updates font and other render settings and invalidates the control for redrawing.
		void										RecalcAndInvalidate() {
			ComputeFontMetrics();
			//CurStyle()->ui64FileSize = Size();
			//m_iAddrDigits = AddrDigitsForSize( Size() );
			
			UpdateScrollbars();
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}

		// Gets the total number of lines in the display.
		inline uint64_t								TotalLines_Hex() const {
			if ( !m_hHex.sStyle.uiBytesPerRow ) { return 0ULL; }
			return (Size() + m_hHex.sStyle.uiBytesPerRow - 1ULL) / m_hHex.sStyle.uiBytesPerRow;
		}

		// Gets the minimum address digits.
		uint32_t									MinAddrDigits() const {
			uint64_t ui64TotalLines = Size() / CurStyle()->uiBytesPerRow * CurStyle()->uiBytesPerRow;
			switch( CurStyle()->daAddressStyle.afFormat ) {
				case MX_AF_BYTES_HEX :		{ return uint32_t( DigitCount( ui64TotalLines, 16 ) ); }
				case MX_AF_BYTES_DEC :		{ return uint32_t( DigitCount( ui64TotalLines, 10 ) ); }
				case MX_AF_BYTES_OCT :		{ return uint32_t( DigitCount( ui64TotalLines, 8 ) ); }
				case MX_AF_LINE_NUMBER :	{ return 1; }
				case MX_AF_SHORT_HEX :		{ return uint32_t( DigitCount( (ui64TotalLines) >> 1, 16 ) ); }
				case MX_AF_SHORT_DEC :		{ return uint32_t( DigitCount( (ui64TotalLines) >> 1, 10 ) ); }
				default :					{ return uint32_t( DigitCount( ui64TotalLines, 16 ) ); }
			}
			//CurStyle()->daAddressStyle.ui32MinAddrDigits
		}

		// Gets the address size (number of digits).
		inline int									AddrDigitsForSize( uint64_t _ui64Size ) const {
			return (_ui64Size > UINT_MAX) ? 16 : 8;
		}

		/**
		 * Computes the pixel width of the address gutter for the current options and font.
		 *
		 * \return Returns the pixel width of the address gutter. Returns 0 if the gutter is hidden.
		 */
		int											ComputeAddressGutterWidthPx();

		/**
		 * Computes the pixel width of the left numeric (hex/dec/oct/bin/char) column for one row.
		 *
		 * \return Returns the pixel width of the left numbers block; 0 if hidden.
		 */
		int											ComputeLeftNumbersWidthPx();

		/**
		 * Computes the pixel width of the right ASCII/text column for one row.
		 *
		 * \return Returns the pixel width of the text block; 0 if hidden.
		 */
		int											ComputeRightTextWidthPx();

		/**
		 * Computes the total width, in pixels, of the horizontally scrollable content (one row).
		 *
		 * \return Returns the pixel width of the scrollable content area (numbers + ASCII). Always >= 1.
		 */
		inline int									TotalContentWidthPx() {
			int iCx = 0;
			iCx += ComputeLeftNumbersWidthPx();
			iCx += ComputeRightTextWidthPx();
			return max( iCx, 1 );
		}

		// Compute how many character columns our content needs.
		//inline int									TotalColumns_Hex() {
		//	// Address: digits + ':' + space.
		//	const int iAddr  = m_iAddrDigits + 2;
		//	// Hex area: each byte = "XX " (3 chars), extra spacer after 8 bytes.
		//	const int iHex   = static_cast<int>(m_hHex.sStyle.uiBytesPerRow) * 3 + ((m_hHex.sStyle.uiBytesPerRow > 8) ? 1 : 0);
		//	// Gap + ASCII area: 2 + bytes.
		//	const int iAscii = 2 + static_cast<int>(m_hHex.sStyle.uiBytesPerRow);
		//	return iAddr + iHex + iAscii;
		//}

		// Gets the total number of lines based on which view type is active.
		uint64_t									TotalLines() const;

		/**
		 * Gets the viewport width, in pixels, available to the horizontally scrollable area.
		 *
		 * \return Returns client width minus fixed panels (address gutter, mini-map) and outer paddings. Always >= 1.
		 */
		int											GetClientWidth() {
			const MX_STYLE & asAddrStyle = (*CurStyle());

			int iW = int( ClientRect().Width()); //m_iClientW;
			if ( asAddrStyle.bShowAddressGutter ) {
				iW -= ComputeAddressGutterWidthPx();          // Includes iPadAfterGutterPx.
			}
			iW -= asAddrStyle.iPadScrollableLeftPx;
			iW -= asAddrStyle.iPadScrollableRightPx;

			if ( asAddrStyle.bShowMiniMap ) {
				iW -= (asAddrStyle.iMiniMapWidthPx + asAddrStyle.iPadBeforeMiniMapPx);
			}
			return (iW > 0) ? iW : 1;
		}


		// Updates the scrollbars.
		void										UpdateScrollbars();

		/** Ensures CurStyle()->agGlyphs is populated for the current font/options. */
		void										EnsureAddrGlyphs( HDC _hDc ) {
			MX_ADDR_GLYPHS & agGlyphs = CurStyle()->agGlyphs;
			if ( agGlyphs.iDigitMaxCx > 0 && CurStyle()->iCharCy > 0 ) { return; }

			TEXTMETRICW tmMetrics {};
			::GetTextMetricsW( _hDc, &tmMetrics );
			CurStyle()->iCharCy = tmMetrics.tmHeight + tmMetrics.tmExternalLeading;

			agGlyphs.iDigitMaxCx = lsw::CHelpers::MeasureMax( _hDc, L"0123456789" );
			const bool bLower = CurStyle()->daAddressStyle.bLowercaseHex;
			const int iHexCx  = lsw::CHelpers::MeasureMax( _hDc, bLower ? L"abcdef" : L"ABCDEF" );
			if ( iHexCx > agGlyphs.iDigitMaxCx ) { agGlyphs.iDigitMaxCx = iHexCx; }

			SIZE sSize {};
			::GetTextExtentPoint32W( _hDc, L":", 1, &sSize ); agGlyphs.iColonCx = sSize.cx;
			::GetTextExtentPoint32W( _hDc, L" ", 1, &sSize ); agGlyphs.iSpaceCx = sSize.cx;
			::GetTextExtentPoint32W( _hDc, L"h", 1, &sSize ); agGlyphs.iSpecHexCx = sSize.cx;
			::GetTextExtentPoint32W( _hDc, L"o", 1, &sSize ); agGlyphs.iSpecOctCx = sSize.cx;
			::GetTextExtentPoint32W( _hDc, L"w", 1, &sSize ); agGlyphs.iShortWcx  = sSize.cx;
		}

		/**
		 * Counts the number of digits needed to represent _ui64N in base _ui32Base.
		 *
		 * \param _ui64N The value whose digit count is requested.
		 * \param _ui32Base The base, typically 10, 16, or 8.
		 * \return Returns the number of digits (at least 1).
		 */
		static uint32_t								DigitCount( uint64_t _ui64N, uint32_t _ui32Base ) {
			uint32_t ui32Val = 1;
			while ( _ui64N >= _ui32Base ) {
				_ui64N /= _ui32Base;
				++ui32Val;
			}
			return ui32Val;
		}
	private :
		typedef lsw::CWidget						CParent;
	};

}	// namespace mx

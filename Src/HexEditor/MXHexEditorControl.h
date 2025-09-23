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
		/** Address formats. */
		enum MX_ADDRESS_FMT {
			MX_AF_BYTES_HEX,						// Example: 1:1D9E:53C8
			MX_AF_BYTES_DEC,						// Example: 6562337568 (right-aligned, padded with spaces on the left)
			MX_AF_BYTES_OCT,						// Example: 60411251440
			MX_AF_LINE_NUMBER,						// Example: 820292197 (right-aligned, padded with spaces on the left)
			MX_AF_SHORT_HEX,						// Example: C392:A990w
			MX_AF_SHORT_DEC,						// Example: 2395961428w (right-aligned, padded with spaces on the left)
		};


		// == Functions.
		// WM_NCCREATE.
		virtual LSW_HANDLED							NcCreate( const CREATESTRUCTW &_csCreateParms );

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
		virtual HFONT								GetFont() { return m_fFont.hFont; }

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
			uint64_t								ui64StartAddress = 0;			// Starting address bias (Set Starting Address).
			//uint64_t								ui64FileSize;					// File size in bytes.
			uint64_t								ui64FirstVisibleLine = 0;		// Top visible line index (0-based).
			uint32_t								ui32MinAddrDigits = 4;			// Clamp minimum digits (HEX: 4..16 typical).
			//uint32_t								ui32BytesPerRow;				// Bytes per displayed row.

			uint32_t								ui32VisibleLines = 0;			// Count of visible lines.

			//HFONT									hFont = NULL;					// Font used for drawing.
			COLORREF								crText;							// Text color (use your theme).
			MX_ADDRESS_FMT							afFormat = MX_AF_BYTES_HEX;     // Address format.
			bool									bLowercaseHex = false;			// Use lowercase a..f for HEX.
			bool									bShowColonIn = true;			// Insert ':' every 4 HEX digits (>4 only).
			bool									bShowColonAfter = false;		// Append ':' after the address.
			bool									bShowTypeSpec = false;			// Append 'h' (hex) or 'o' (oct).
			bool									bMinimizeDigits = false;		// Size digits for current page, not whole file.
			bool									bUseShortSuffixW = true;		// Append 'w' for Short addressing (matches your examples).
		};

		/** Measured glyph metrics for the address gutter font. */
		struct MX_ADDR_GLYPHS {
			int										iDigitMaxCx = 0;				// Max over '0'..'9' (and 'a'..'f' or 'A'..'F' when HEX).
			int										iColonCx = 0;					// Width of ':'.
			int										iSpaceCx = 0;					// Width of ' '.
			int										iSpecHexCx = 0;					// Width of 'h'.
			int										iSpecOctCx = 0;					// Width of 'o'.
			int										iShortWcx = 0;					// Width of 'w'.
			int										iCharCy = 0;					// Baseline advance (line height; you already have m_iCyChar).
		};

		/** General style settings. */
		struct MX_STYLE {
			MX_ADDR_STYLE							daAddressStyle;					// Address style.
			MX_ADDR_GLYPHS							agGlyphs;						// Glyph settings.
			uint32_t								uiBytesPerRow = 16;				// Bytes per displayed row (default 16).
		};

		/** The hex-view information. */
		struct MX_HEX {
			MX_STYLE								sStyle;							// Style settings for the Hex View.
		};


		// == Members.
		CHexEditorInterface *						m_pheiTarget = nullptr;			// The stream of data to handle.
		lsw::LSW_FONT								m_fFont;						// Current font (monospace).
		MX_HEX										m_hHex;							// Hex view settings.
		int											m_iCxChar;						// Char width in pixels.
		int											m_iCyChar;						// Char height in pixels.
		int											m_iClientW;						// Client width.
		int											m_iClientH;						// Client height.
		//UINT										m_uiDpi;						// Window DPI.
		//const uint8_t *								m_pData;					// Data pointer (not owned).
		//uint64_t									m_ui64Size = 0;					// Data size in bytes.  TMP.
		int											m_iVPos;						// Vertical position (first visible line index).
		int											m_iHPos;						// Horizontal position (columns).
		int											m_iPageLines;					// How many text rows fit.
		int											m_iPageCols;					// How many text columns fit.
		int											m_iAddrDigits;					// 8 or 16.


		// The main window class.
		static ATOM									m_aAtom;


		// == Functions.
		// Draws the hex-editor view.
		bool										PaintHex( HDC _hDc, const lsw::LSW_RECT &_rRect );

		// Computes font metrics.
		void										ComputeFontMetrics() {
			HDC hDc = ::GetDC( Wnd() );
			{
				lsw::LSW_SELECTOBJECT soFontOrig( hDc, m_fFont.hFont );	// Destructor sets the original font back.
				TEXTMETRICW tmMetrics {};
				::GetTextMetricsW( hDc, &tmMetrics );
				m_iCxChar = tmMetrics.tmAveCharWidth;
				m_iCyChar = tmMetrics.tmHeight + tmMetrics.tmExternalLeading;
			}
			::ReleaseDC( Wnd(), hDc );
		}

		// Sets a default font.
		void										ChooseDefaultFont() {
			LOGFONTW lfFont {};
			lfFont = lsw::CBase().NonClientMetrics().lfMessageFont;
			lfFont.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
			::wcscpy_s( lfFont.lfFaceName, L"Consolas" );
			int iPt = 10;

			lfFont.lfHeight = -::MulDiv( iPt, static_cast<int>(m_wDpiY), 72 );
			m_fFont.CreateFontIndirectW( &lfFont );
		}

		// Updates font and other render settings and invalidates the control for redrawing.
		void										RecalcAndInvalidate() {
			ComputeFontMetrics();
			m_iAddrDigits = AddrDigitsForSize( Size() );
			UpdateScrollbars();
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}

		// Gets the total number of lines in the display.
		inline uint64_t								TotalLines_Hex() {
			if ( !m_hHex.sStyle.uiBytesPerRow ) { return 0ULL; }
			return (Size() + m_hHex.sStyle.uiBytesPerRow - 1ULL) / m_hHex.sStyle.uiBytesPerRow;
		}

		// Gets the address size (number of digits).
		inline int									AddrDigitsForSize( uint64_t _ui64Size ) {
			return (_ui64Size > UINT_MAX) ? 16 : 8;
		}

		// Compute how many character columns our content needs.
		inline int									TotalColumns_Hex() {
			// Address: digits + ':' + space.
			const int iAddr  = m_iAddrDigits + 2;
			// Hex area: each byte = "XX " (3 chars), extra spacer after 8 bytes.
			const int iHex   = static_cast<int>(m_hHex.sStyle.uiBytesPerRow) * 3 + ((m_hHex.sStyle.uiBytesPerRow > 8) ? 1 : 0);
			// Gap + ASCII area: 2 + bytes.
			const int iAscii = 2 + static_cast<int>(m_hHex.sStyle.uiBytesPerRow);
			return iAddr + iHex + iAscii;
		}

		// Updates the scrollbars.
		void										UpdateScrollbars();
	private :
		typedef lsw::CWidget						CParent;
	};

}	// namespace mx

#pragma once

#include "../Strings/MXStringDecoder.h"

#include <LSWWin.h>
#include <Helpers/LSWHelpers.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>


namespace mx {

	class CHexEditorControl : public lsw::CWidget {
	public :
		CHexEditorControl( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, lsw::CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// WM_NCCREATE.
		virtual LSW_HANDLED							NcCreate( const CREATESTRUCTW &_csCreateParms );

		// WM_NCDESTROY.
		virtual LSW_HANDLED							NcDestroy();

		// Setting the HWND after the control has been created.
		virtual void								InitControl( HWND _hWnd );

		// WM_ERASEBKGND.
		virtual LSW_HANDLED							EraseBkgnd( HDC _hDc ) { return LSW_H_HANDLED; }

		/**
		 * The WM_GETFONT handler.
		 *
		 * \param Returns -1 to perform the default routine, otherwise NULL or a handle to a font.
		 */
		virtual HFONT								GetFont() { return m_fFont.hFont; }

		// Translate a child's tooltip text.
		virtual std::wstring						TranslateTooltip( const std::string &_sText ) { 
			return CStringDecoder::DecodeToWString( _sText.c_str(), _sText.size() );
		}

		// Registers the control if it has not been registered already.  Redundant calls have no effect.  Must be called before creating this control.
		static void									PrepareControl();


	protected :
		// == Members.
		lsw::LSW_FONT								m_fFont;						// Current font (monospace).
		int											m_iCxChar;						// Char width in pixels.
		int											m_iCyChar;						// Char height in pixels.
		int											m_iClientW;						// Client width.
		int											m_iClientH;						// Client height.
		//UINT										m_uiDpi;						// Window DPI.
		UINT										m_uiBytesPerRow;				// Bytes per displayed row (default 16).
		//const uint8_t *								m_pData;					// Data pointer (not owned).
		uint64_t									m_ui64Size = 0;					// Data size in bytes.  TMP.
		int											m_iVPos;						// Vertical position (first visible line index).
		int											m_iHPos;						// Horizontal position (columns).
		int											m_iPageLines;					// How many text rows fit.
		int											m_iPageCols;					// How many text columns fit.
		int											m_iAddrDigits;					// 8 or 16.


		// The main window class.
		static ATOM									m_aAtom;


		// == Functions.
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
			m_iAddrDigits = AddrDigitsForSize( m_ui64Size );
			//UpdateScrollbars();
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}

		// Gets the total number of lines in the display.
		inline uint64_t								TotalLines() {
			if ( !m_uiBytesPerRow ) { return 0ULL; }
			return (m_ui64Size + m_uiBytesPerRow - 1ULL) / m_uiBytesPerRow;
		}

		// Gets the address size (number of digits).
		inline int									AddrDigitsForSize( uint64_t _ui64Size ) {
			return (_ui64Size > UINT_MAX) ? 16 : 8;
		}

		// Compute how many character columns our content needs.
		inline int									TotalColumns() {
			// Address: digits + ':' + space.
			const int iAddr  = m_iAddrDigits + 2;
			// Hex area: each byte = "XX " (3 chars), extra spacer after 8 bytes.
			const int iHex   = static_cast<int>( m_uiBytesPerRow ) * 3 + ((m_uiBytesPerRow > 8) ? 1 : 0);
			// Gap + ASCII area: 2 + bytes.
			const int iAscii = 2 + static_cast<int>( m_uiBytesPerRow );
			return iAddr + iHex + iAscii;
		}


	private :
		typedef lsw::CWidget						CParent;
	};

}	// namespace mx

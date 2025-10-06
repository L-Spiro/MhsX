#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
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
		/** Shared fonts. */
		enum MX_FONT_TYPE {
			MX_FT_FIXED_ROW,
			MX_FT_TEXT_VIEW,
		};

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

			MX_ES_TOTAL
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


		// == Types.
		/** Measured glyph metrics for the address gutter font. */
		struct MX_ADDR_GLYPHS {
			int										iDigitMaxCx = 0;										// Max over '0'..'9' (and 'A'..'F' when HEX).
			int										iDigitMaxCxLower = 0;									// Max over '0'..'9' (and 'a'..'f' when HEX).
			int										iColonCx = 0;											// Width of ':'.
			int										iSpaceCx = 0;											// Width of ' '.
			int										iSpecHexCx = 0;											// Width of 'h'.
			int										iSpecOctCx = 0;											// Width of 'o'.
			int										iShortWcx = 0;											// Width of 'w'.
		};

		/** A set of font data. */
		struct MX_FONT_SET {
			lsw::LSW_FONT							fFont;													// Current font.
			LOGFONTW								lfFontParms {};											// The current font parameters.
			TEXTMETRICW								tmMetrics {};											// The current font's metrics.
			MX_ADDR_GLYPHS							agGlyphs;												// Glyph settings (filled on demand).
			int32_t									i32PointSize = DefaultPointSize();						// The font point size.

			int										iCharCx = 0;
			int										iCharCy = 0;											// Baseline advance.
		};

		/** Creation parameters. */
		struct MX_CREATION_PARMS {
			MX_FONT_SET *							pfsFixedRowFont = nullptr;								/**< The shared font for fixed-row views. */
			MX_FONT_SET *							pfsDynamicRowFont = nullptr;							/**< The shared font for text views. */
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
		virtual HFONT								GetFont() { return Font()->fFont.hFont; }

		/**
		 * Increases the font size.  Returns the new font parameters and the view type.  Hex, Octal, and Bindary displays are linked, etc.
		 * 
		 * \param _lfFont Holds the returned font for applying to other tabs
		 * \return Returns true if the view type is Hex, Octal, or Binary.
		 **/
		bool										IncreaseFontSize( LOGFONTW &_lfFont );

		/**
		 * Decreases the font size.  Returns the new font parameters and the view type.  Hex, Octal, and Bindary displays are linked, etc.
		 * 
		 * \param _lfFont Holds the returned font for applying to other tabs
		 * \return Returns true if the view type is Hex, Octal, or Binary.
		 **/
		bool										DecreaseFontSize( LOGFONTW &_lfFont );

		/**
		 * Sets the font size.
		 * 
		 * \param _i32PtSize The font size to set.
		 * \return Returns true if the font was changed.
		 **/
		bool										SetFontSize( int32_t _i32PtSize );

		/**
		 * Sets the current font.
		 * 
		 * \param _lfFont The font to set.
		 * \param _bFixedRowLength If true, the fixed-sized fonts are updated, otherwise the variable-row-length fonts are updated.
		 * \return Returns true if setting the font succeeded,
		 **/
		bool										SetFont( const LOGFONTW &_lfFont, bool _bFixedRowLength );

		/**
		 * Gets the font for either fixed-row- or dynamic-row- length.
		 * 
		 * \param _bFixedRowLength Whether to return the fixed-row font or the dynamic-row font.
		 * \return Returns the selected font.
		 **/
		inline LOGFONTW								GetThisFont() {
			return Font()->lfFontParms;
		}

		/**
		 * Sets the default font size.
		 * 
		 * \param _lfFont Holds the returned font for applying to other tabs
		 * \return Returns true if the view type is Hex, Octal, or Binary.
		 **/
		bool										DefaultFontSize( LOGFONTW &_lfFont );

		// Gets the current font.
		MX_FONT_SET *								Font() { return m_pfsFonts[m_sStyles[m_eaEditAs].ftFont]; }

		// Gets the current font.
		const MX_FONT_SET *							Font() const { return m_pfsFonts[m_sStyles[m_eaEditAs].ftFont]; }

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
		

		// Updates font and other render settings and invalidates the control for redrawing.
		void										RecalcAndInvalidate() {
			ComputeFontMetrics();
			
			UpdateScrollbars();
			::InvalidateRect( Wnd(), NULL, FALSE );
		}

		// Registers the control if it has not been registered already.  Redundant calls have no effect.  Must be called before creating this control.
		static void									PrepareControl();

		// Sets a default font.
		static bool									ChooseDefaultFont( MX_FONT_SET &_fsFont, WORD _wDpi, HWND _hWnd );

		// Computes font metrics.
		static void									ComputeFontMetrics( MX_FONT_SET &_fsFont, WORD _wDpi, HWND _hWnd ) {
			lsw::LSW_HDC hDc( _hWnd );
			{
				lsw::LSW_SELECTOBJECT soFontOrig( hDc.hDc, _fsFont.fFont.hFont );	// Destructor sets the original font back.
				TEXTMETRICW tmMetrics {};
				::GetTextMetricsW( hDc.hDc, &tmMetrics );
				_fsFont.iCharCx = tmMetrics.tmAveCharWidth;
				_fsFont.iCharCy = tmMetrics.tmHeight + tmMetrics.tmExternalLeading;
				_fsFont.tmMetrics = tmMetrics;
				int32_t i32EmPx = static_cast<int32_t>(tmMetrics.tmHeight - tmMetrics.tmInternalLeading);
				if ( i32EmPx <= 0 ) {
					i32EmPx = static_cast<int32_t>(tmMetrics.tmHeight);
				}
				float fPoints = static_cast<float>(i32EmPx) * 72.0f / static_cast<float>(_wDpi);
				_fsFont.i32PointSize = std::round( fPoints );
				// To be set by EnsureAddrGlyphs().
				_fsFont.agGlyphs.iDigitMaxCx = 0;
				//_fsFont.iCharCy = 0;
			}
		}

		// Gets the default font size (later to be replaced with a settings value).
		static int32_t								DefaultPointSize() { return 10; }


	protected :
		// == Types.
		/** What to draw and how to size the address gutter. */
		struct MX_ADDR_STYLE {
			uint64_t								ui64StartAddress = 0;									// Starting address bias (Set Starting Address).
			uint64_t								ui64FirstVisibleLine = 0;								// Top visible line index (0-based).

			uint32_t								ui32VisibleLines = 0;									// Count of visible lines.

			MX_ADDRESS_FMT							afFormat = MX_AF_BYTES_HEX;								// Address format.
			bool									bLowercaseHex = false;									// Use lowercase a..f for HEX.
			bool									bShowColonIn = true;									// Insert ':' every 4 HEX digits (>4 only).
			bool									bShowColonAfter = false;								// Append ':' after the address.
			bool									bShowTypeSpec = false;									// Append 'h' (hex) or 'o' (oct).
			bool									bMinimizeDigits = false;								// Size digits for current page, not whole file.
			bool									bUseShortSuffixW = true;								// Append 'w' for Short addressing (matches your examples).
		};

		/** General style settings. */
		struct MX_STYLE {
			MX_FONT_TYPE							ftFont = MX_FT_FIXED_ROW;								// The shared font to use.
			MX_ADDR_STYLE							daAddressStyle;											// Address style.
			uint32_t								uiBytesPerRow = 16;										// Bytes per displayed row.
			uint64_t								ui64FileSize = 0;										// File size in bytes (needed for address digits).

			// Visibility.
			bool									bShowAddressGutter	= true;
			bool									bShowLeftNumbers	= true;								// Hex/Dec/Oct/Bin column.
			bool									bShowRightArea		= true;								// ASCII column.
			bool									bShowMiniMap		= false;							// Fixed panel on right.
			bool									bShowRuler			= true;								// Show/hide ruler row.

			// Left numbers formatting.
			MX_DATA_FMT								dfLeftNumbersFmt = MX_DF_HEX;
			uint32_t								uiGroupSize = 2;                   						// Extra spacing after every N bytes.
			uint32_t								uiSpacesBetweenBytes = 1;          						// Count of ' ' between adjacent bytes.
			uint32_t								uiExtraSpacesBetweenGroups = 1;     					// Extra ' ' at group boundary.

			// Right formatting.
			MX_DATA_FMT								dfRightNumbersFmt = MX_DF_CHAR;

			// Paddings/gaps (pixels).
			int32_t									i32LeftAddrPadding = 3;										// Left padding.
			int32_t									iPadAfterGutterPx = 8;
			int32_t									iPadBetweenNumbersAndTextPx = 16;
			int32_t									iPadNumbersLeftPx = 0;
			int32_t									iPadNumbersRightPx = 0;
			int32_t									iPadTextRightPx = 0;
			int32_t									iPadScrollableLeftPx = 0;
			int32_t									iPadScrollableRightPx = 0;
			int32_t									iPadBeforeMiniMapPx = 8;
			int32_t									iLineSpacingPx = 2;

			// Ruler.

			// Mini-map geometry (pixels).
			int32_t									iMiniMapWidthPx = 140;
		};


		// == Members.
		uint64_t									m_ui64VPos = 0;											// First visible line (virtual).
		uint64_t									m_ui64HPx = 0;											// Horizontal scroll offset.
		MX_FONT_SET *								m_pfsFonts[2] = {};										// Shared fonts.
		MX_EDIT_AS									m_eaEditAs = MX_ES_HEX;									// The view type.
		COLORREF									m_crText = MX_RGBA( 0x92, 0x92, 0x92, 0xFF );			// Text color.
		COLORREF									m_crAddSepLine = MX_RGBA( 0x42, 0x42, 0x42, 0x00 );		// Address separator line.
		COLORREF									m_crRulerText = MX_RGBA( 0x57, 0x57, 0x57, 0xFF );		// Ruler text color.
		COLORREF									m_crRulerLine = MX_RGBA( 0x57, 0x57, 0x57, 0x00 );		// Ruler line.
		COLORREF									m_crRulerMarker = MX_RGBA( 0x81, 0x81, 0x81, 0xFF );	// Ruler marker color.
		COLORREF									m_crEditorBk = MX_RGBA( 0x23, 0x22, 0x20, 0xFF );		// Background color for the whole editor.

		CHexEditorInterface *						m_pheiTarget = nullptr;									// The stream of data to handle.
		MX_STYLE									m_sStyles[MX_ES_TOTAL];									// View settings.
		int											m_iPageLines;											// How many text rows fit.
		int											m_iPageCols;											// How many text columns fit.

		// The main window class.
		static ATOM									m_aAtom;


		// == Functions.
		// Gets the current style settings.
		MX_STYLE *									CurStyle() { return &m_sStyles[m_eaEditAs]; }

		// Gets the current style settings.
		const MX_STYLE *							CurStyle() const { return &m_sStyles[m_eaEditAs]; }

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

		/**
		 * \brief Draws the ruler at the given position.
		 *
		 * \param _hDc Destination HDC.
		 * \param _iXLeft Pixel X where the scrollable content begins (after gutter; already includes horizontal scroll offset).
		 * \param _iYTop Pixel Y top where the ruler should be drawn.
		 * \param _dfLeftFmt Format for the left area.
		 * \param _dfRightFmt Format for the right area.
		 *
		 * Description:
		 *  - The ruler width equals the sum of the left numbers block and the right text block (if visible),
		 *    including the inter-block gap. The mini-map and gutter do not scroll and are not part of the ruler.
		 *  - Each group label is centered within its group rect, computed by GetGroupRectForIndex().
		 *  - The rulerfs height equals the base character height for the font; line spacing is ignored.
		 */
		void										DrawRuler( HDC _hDc, int _iXLeft, int _iYTop, MX_DATA_FMT _dfLeftFmt, MX_DATA_FMT _dfRightFmt );

		// Computes font metrics.
		void										ComputeFontMetrics() {
			ComputeFontMetrics( (*Font()), m_wDpiY, Wnd() );
		}

		/**
		 * \brief Returns the vertical advance, in pixels, between consecutive rows.
		 *
		 * Description: Base line height (m_iCyChar or measured glyph height) plus CurStyle()->iLineSpacingPx.
		 */
		int											LineAdvanceCy() const {
			return Font()->iCharCy + CurStyle()->iLineSpacingPx;
		}

		// Gets the total number of lines in the display.
		inline uint64_t								TotalLines_FixedWidth() const {
			if ( !&m_sStyles[MX_ES_HEX].uiBytesPerRow ) { return 0ULL; }
			return (Size() + m_sStyles[MX_ES_HEX].uiBytesPerRow - 1ULL) / m_sStyles[MX_ES_HEX].uiBytesPerRow;
		}

		// Gets the minimum address digits.
		uint32_t									MinAddrDigits() const {
			uint64_t ui64TotalLines = Size() / CurStyle()->uiBytesPerRow * CurStyle()->uiBytesPerRow;
			uint32_t ui32Result = 4;
			switch( CurStyle()->daAddressStyle.afFormat ) {
				case MX_AF_BYTES_HEX :		{ ui32Result = uint32_t( DigitCount( ui64TotalLines, 16 ) ); break; }
				case MX_AF_BYTES_DEC :		{ return uint32_t( DigitCount( ui64TotalLines, 10 ) ); break; }
				case MX_AF_BYTES_OCT :		{ return uint32_t( DigitCount( ui64TotalLines, 8 ) ); break; }
				case MX_AF_LINE_NUMBER :	{ return 1; break; }
				case MX_AF_SHORT_HEX :		{ return uint32_t( DigitCount( (ui64TotalLines) >> 1, 16 ) ); break; }
				case MX_AF_SHORT_DEC :		{ return uint32_t( DigitCount( (ui64TotalLines) >> 1, 10 ) ); break; }
				default :					{ return uint32_t( DigitCount( ui64TotalLines, 16 ) ); break; }
			}
			return std::max( ui32Result, 4U );
		}

		// Gets the address size (number of digits).
		inline int									AddrDigitsForSize( uint64_t _ui64Size ) const {
			return (_ui64Size > UINT_MAX) ? 16 : 8;
		}

		/**
		 * Determines whether the current Edit As type is a fixed-width type (hex, octal, or binary).
		 * 
		 * \return Returns true if the view type is Hex, Binary, Code, or a process.
		 **/
		inline bool									IsFixedRowLength() const { return m_eaEditAs == MX_ES_HEX || m_eaEditAs == MX_ES_BINARY || m_eaEditAs == MX_ES_CUR_PROCESS || m_eaEditAs == MX_ES_PROCESS; }

		/**
		 * Computes the pixel width of the address gutter for the current options and font.
		 *
		 * \return Returns the pixel width of the address gutter. Returns 0 if the gutter is hidden.
		 */
		int											ComputeAddressGutterWidthPx();

		/**
		 * \brief Returns the ruler height in pixels.
		 *
		 * Description: The ruler height equals the base character height for the current font
		 *  (line spacing does not apply to the ruler). Returns 0 if the ruler is hidden.
		 */
		int											GetRulerHeightPx() const {
			if ( !CurStyle()->bShowRuler ) { return 0; }
			return Font()->iCharCy;
		}

		/**
		 * \brief Computes the left X and width (in pixels) of a group at index for an area.
		 *
		 * \param _dfDataFmt Data format of the area (HEX/DEC/OCT/BIN/CHAR).
		 * \param _ui32Index Zero-based group index within the row (e.g., 0..(groups-1)).
		 * \param _bRightArea False for the left numbers area; true for the right text area.
		 * \param _iXBase The pixel X of the beginning of the area (not including gutter; includes horizontal scroll offset).
		 * \param _iXLeft [out] Receives the pixel X of the groupfs left edge.
		 * \param _iWidth [out] Receives the pixel width of the group (internal bytes/chars and normal intra-byte spacing).
		 * \return Returns true if the index is valid for the current layout; false otherwise.
		 *
		 * Description:
		 *  - A ggrouph is composed of N adjacent cells (bytes for HEX/DEC/OCT/BIN, characters for CHAR),
		 *    where N is the areafs grouping size. The width includes intra-cell spacing inside the group.
		 *  - The extra spacing that separates two adjacent groups (group gap) is not included in the width
		 *    of the current group; it is placed after it.
		 *  - The caller can center text within the returned [left,width] using a text measurement.
		 */
		bool										GetGroupRectForIndex(
			MX_DATA_FMT _dfDataFmt,
			uint32_t _ui32Index,
			bool _bRightArea,
			int _iXBase,
			int & _iXLeft,
			int & _iWidth
		) const;

		/**
		 * Computes the pixel width of the left numeric (hex/dec/oct/bin/char) column for one row.
		 *
		 * \param _dfDataFmt Data format of the area (HEX/DEC/OCT/BIN/CHAR).
		 * \return Returns the pixel width of the left numbers block; 0 if hidden.
		 */
		int											ComputeAreaWidthPx( MX_DATA_FMT _dfDataFmt );

		/**
		 * Computes the total width, in pixels, of the horizontally scrollable content (one row).
		 *
		 * \return Returns the pixel width of the scrollable content area (numbers + ASCII). Always >= 1.
		 */
		inline int									TotalContentWidthPx() {
			const MX_STYLE & stAll = (*CurStyle());
			int iCx = 0;
			iCx += stAll.iPadNumbersLeftPx + ComputeAreaWidthPx( stAll.dfLeftNumbersFmt ) + stAll.iPadNumbersRightPx;
			if ( CurStyle()->bShowRightArea ) {
				iCx += stAll.iPadBetweenNumbersAndTextPx + ComputeAreaWidthPx( stAll.dfRightNumbersFmt );
			}
			return max( iCx, 1 );
		}

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
			MX_ADDR_GLYPHS & agGlyphs = Font()->agGlyphs;
			if ( agGlyphs.iDigitMaxCx > 0 && Font()->iCharCy > 0 ) { return; }

			TEXTMETRICW tmMetrics {};
			::GetTextMetricsW( _hDc, &tmMetrics );
			Font()->iCharCy = tmMetrics.tmHeight + tmMetrics.tmExternalLeading;

			agGlyphs.iDigitMaxCx = lsw::CHelpers::MeasureMax( _hDc, L"0123456789" );
			agGlyphs.iDigitMaxCx = std::max( agGlyphs.iDigitMaxCx, lsw::CHelpers::MeasureMax( _hDc, L"ABCDEF" ) );
			agGlyphs.iDigitMaxCxLower = std::max( agGlyphs.iDigitMaxCx, lsw::CHelpers::MeasureMax( _hDc, L"abcdef" ) );

			SIZE sSize {};
			::GetTextExtentPoint32W( _hDc, L":", 1, &sSize ); agGlyphs.iColonCx = sSize.cx;
			::GetTextExtentPoint32W( _hDc, L" ", 1, &sSize ); agGlyphs.iSpaceCx = (sSize.cx + 1) / 2 * 2;	// Round up to the nearest even number.
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

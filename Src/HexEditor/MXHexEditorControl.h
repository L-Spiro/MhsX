#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include "MXCharSets.h"
#include "MXHexEditorColors.h"
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
			MX_EA_TEXT,
			MX_EA_HEX,
			MX_EA_BINARY,
			MX_EA_SCRIPT,
			MX_EA_TEMPLATE,
			MX_EA_EBCDIC,
			MX_EA_UTF16,
			MX_EA_UTF8,
			MX_EA_PROCESS,
			MX_EA_CUR_PROCESS,
			MX_EA_CODE,
			MX_EA_TAGGED,

			MX_EA_TOTAL
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

		/** Data display for the left and right columns. */
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
			int32_t									iDigitMaxCx = 0;										// Max over '0'..'9' (and 'A'..'F' when HEX).
			int32_t									iDigitMaxCxLower = 0;									// Max over '0'..'9' (and 'a'..'f' when HEX).
			int32_t									i32ColonCx = 0;											// Width of ':'.
			int32_t									i32SpaceCx = 0;											// Width of ' '.
			int32_t									i32SpecHexCx = 0;										// Width of 'h'.
			int32_t									i32SpecOctCx = 0;										// Width of 'o'.
			int32_t									i32ShortWcx = 0;										// Width of 'w'.
			int32_t									i32MaxAscii = 0;										// Maximum width of ASCII characters.
			int32_t									i32MaxAsciiAnsi = 0;									// Maximum width of ASCII+ANSI characters.
		};

		/** A set of font data. */
		struct MX_FONT_SET {
			lsw::LSW_FONT							fFont;													// Current font.
			LOGFONTW								lfFontParms {};											// The current font parameters.
			TEXTMETRICW								tmMetrics {};											// The current font's metrics.
			MX_ADDR_GLYPHS							agGlyphs;												// Glyph settings (filled on demand).
			int32_t									i32PointSize = DefaultPointSize();						// The font point size.

			int32_t									iCharCx = 0;
			int32_t									iCharCy = 0;											// Baseline advance.
		};

		/** What to draw and how to size the address gutter. */
		struct MX_ADDR_STYLE {
			MX_ADDRESS_FMT							afFormat			= MX_AF_BYTES_HEX;					// Address format.
			bool									bLowercaseHex		= false;							// Use lowercase a..f for HEX.
			bool									bShowColonIn		= true;								// Insert ':' every 4 HEX digits (>4 only).
			bool									bShowColonAfter		= false;							// Append ':' after the address.
			bool									bShowTypeSpec		= false;							// Append 'h' (hex) or 'o' (oct).
			bool									bMinimizeDigits		= false;							// Size digits for current page, not whole file.
			bool									bUseShortSuffixW	= true;								// Append 'w' for Short addressing.
		};

		/** Per-view-type style settings. */
		struct MX_STYLE {
			MX_FONT_SET								fsFonts[2]						= {};						// Shared fonts.
			MX_FONT_TYPE							ftFont							= MX_FT_FIXED_ROW;			// The shared font to use.
			CCharSets::MX_CHAR_SETS					csCharSet						= CCharSets::MX_CS_ASCII;	// the current character set.
			MX_ADDR_STYLE							daAddressStyle;												// Address style.
			uint32_t								uiBytesPerRow					= 16;						// Bytes per displayed row.
			uint64_t								ui64DivisionSpacing				= 4;						// Spacing between division lines.

			// Visibility.
			bool									bShowAddressGutter				= true;
			bool									bShowLeftNumbers				= true;						// Hex/Dec/Oct/Bin column.
			bool									bShowRightArea					= true;						// ASCII column.
			bool									bShowMiniMap					= true;						// Fixed panel on right.
			bool									bShowRuler						= true;						// Show/hide ruler row.

			// Left numbers formatting.
			MX_DATA_FMT								dfLeftNumbersFmt				= MX_DF_HEX;
			uint32_t								uiGroupSize						= 1;                   		// Extra spacing after every N bytes.
			uint32_t								uiSpacesBetweenBytes			= 1;          				// Count of ' ' between adjacent bytes.
			uint32_t								uiExtraSpacesBetweenGroups		= 1;     					// Extra ' ' at group boundary.

			// Right formatting.
			MX_DATA_FMT								dfRightNumbersFmt				= MX_DF_CHAR;

			// Paddings/gaps (pixels).
			int32_t									i32LeftAddrPadding				= 3;						// Left padding.
			int32_t									i32PadAfterGutterPx				= 8;
			int32_t									i32PadBetweenNumbersAndTextPx	= 3;
			int32_t									i32PadNumbersLeftPx				= 3;
			int32_t									i32PadNumbersRightPx			= 3;
			int32_t									i32PadScrollableLeftPx			= 0;
			int32_t									i32PadScrollableRightPx			= 0;
			int32_t									i32PadBeforeMiniMapPx			= 8;
			int32_t									i32LineSpacingPx				= 2;

			// Ruler.

			// Mini-map geometry (pixels).
			int32_t									i32MiniMapWidthPx				= 140;

		};

		/** Creation parameters. */
		struct MX_CREATION_PARMS {
			MX_STYLE *								psOptions = nullptr;									/**< Shared options. */
			MX_FONT_SET *							pfsFixedRowFont = nullptr;								/**< The shared font for fixed-row views. */
			MX_FONT_SET *							pfsDynamicRowFont = nullptr;							/**< The shared font for text views. */
			MX_HEX_EDITOR_COLORS *					phecFg = nullptr;										/**< Foreground colors. */
			MX_HEX_EDITOR_COLORS *					phecBg = nullptr;										/**< Background colors. */
		};


		// == Functions.
		// Sets the view type.
		void										SetViewType( MX_EDIT_AS _eaType ) {
			if ( m_eaEditAs != _eaType && _eaType >= MX_EA_TEXT && _eaType < MX_EA_TOTAL ) {
				m_eaEditAs = _eaType;
				m_sdScrollView[_eaType].ui64VPos = 0;
				m_sdScrollView[_eaType].ui64HPx = 0;

				RecalcAndInvalidate();
			}
		}

		// Gets the editing type of the control.
		inline MX_EDIT_AS							GetViewType() const { return m_eaEditAs; }

		// Sets show-addresses.
		void										SetShowAddressGutter( bool _bShowAddress ) {
			if ( CurStyle()->bShowAddressGutter != _bShowAddress ) {
				CurStyle()->bShowAddressGutter = _bShowAddress;
				RecalcAndInvalidate();
			}
		}

		// Gets the show-addresses flag.
		inline bool									GetShowAddressGutter() const { return CurStyle()->bShowAddressGutter; }

		// Sets the address-gutter format.
		void										SetAddressGutterFmt( MX_ADDRESS_FMT _afFmt ) {
			if ( CurStyle()->daAddressStyle.afFormat != _afFmt ) {
				CurStyle()->daAddressStyle.afFormat = _afFmt;
				RecalcAndInvalidate();
			}
		}

		// Gets the address-gutter format.
		inline MX_ADDRESS_FMT						GetAddressGutterFmr() const { return CurStyle()->daAddressStyle.afFormat; }

		// Sets the row width.
		void										SetLineWidth( uint32_t _ui32Width ) {
			// TODO: Set Auto Line Width to false.
			if ( CurStyle()->uiBytesPerRow != _ui32Width && _ui32Width >= 1 ) {
				CurStyle()->uiBytesPerRow = _ui32Width;
				// Group size must be a multiple of the line size.
				if ( _ui32Width % CurStyle()->uiGroupSize ) {
					uint32_t I;
					for ( I = 1; I != _ui32Width; ++I ) {
						if ( I ) {
							if ( (_ui32Width % I) == 0 ) {
								if ( (_ui32Width / I) <= CurStyle()->uiGroupSize ) {
									break;
								}
							}
						}
					}
					CurStyle()->uiGroupSize = _ui32Width / I;
				}

				RecalcAndInvalidate();
			}
		}

		// Gets the row width.
		inline uint32_t								GetLineWidth() const { return CurStyle()->uiBytesPerRow; }

		// Sets thr row width to Auto.
		void										SetLineWidthdAuto( bool /*_bSet*/ ) {
		}

		// Gets the Auto status of the row width.
		inline bool									GetLineWidthAuto() const { return false; }

		// Sets the group size.
		void										SetGroupSize( uint32_t _ui32GroupSize ) {
			if ( CurStyle()->uiGroupSize != _ui32GroupSize && _ui32GroupSize >= 1 ) {
				CurStyle()->uiGroupSize = _ui32GroupSize;
				// Line width umust be a multiple of group size.
				if ( CurStyle()->uiBytesPerRow % _ui32GroupSize ) {
					CurStyle()->uiBytesPerRow = (CurStyle()->uiBytesPerRow + (_ui32GroupSize - 1)) / _ui32GroupSize * _ui32GroupSize;
				}
				RecalcAndInvalidate();
			}
		}

		// Gets the group size.
		inline uint32_t								GetGroupSize() const { return CurStyle()->uiGroupSize; }

		// Sets the division lines.
		void										SetDivionLines( uint64_t _ui64Spacing ) {
			if ( CurStyle()->ui64DivisionSpacing != _ui64Spacing ) {
				CurStyle()->ui64DivisionSpacing = _ui64Spacing;
				RecalcAndInvalidate();
			}
		}

		// Gets the division-line spacing.
		inline uint64_t								GetDivisionLines() const { return CurStyle()->ui64DivisionSpacing; }

		// Sets the left area format.
		void										SetLeftAreaFormat( MX_DATA_FMT _fdfFmt ) {
			if ( CurStyle()->dfLeftNumbersFmt != _fdfFmt ) {
				CurStyle()->dfLeftNumbersFmt = _fdfFmt;
				RecalcAndInvalidate();
			}
		}

		// Gets the left-area type.
		inline MX_DATA_FMT							GetLeftAreaFormat() const { return CurStyle()->dfLeftNumbersFmt; }

		// Sets the right area format.
		void										SetRightAreaFormat( MX_DATA_FMT _fdfFmt ) {
			if ( CurStyle()->dfRightNumbersFmt != _fdfFmt || RightAreaHidden() ) {
				CurStyle()->dfRightNumbersFmt = _fdfFmt;
				CurStyle()->bShowRightArea = true;
				RecalcAndInvalidate();
			}
		}

		// Gets the right-area type.
		inline MX_DATA_FMT							GetRightAreaFormat() const { return CurStyle()->dfRightNumbersFmt; }

		// Hides the right area.
		void										HideRightArea() {
			if ( CurStyle()->bShowRightArea ) {
				CurStyle()->bShowRightArea = false;
				RecalcAndInvalidate();
			}
		}

		// Gets the right-area-hidden setting.
		inline bool									RightAreaHidden() const { return !CurStyle()->bShowRightArea; }

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
		inline MX_FONT_SET *						Font() { return m_pfsFonts[m_psOptions[m_eaEditAs].ftFont]; }

		// Gets the current font.
		const inline MX_FONT_SET *					Font() const { return m_pfsFonts[m_psOptions[m_eaEditAs].ftFont]; }

		// Gets a foreground color.
		inline MX_HEX_EDITOR_COLORS *				ForeColors() { return m_phecForeColors; }

		// Gets a foreground color.
		inline const MX_HEX_EDITOR_COLORS *			ForeColors() const { return m_phecForeColors; }

		// Gets a background color.
		inline MX_HEX_EDITOR_COLORS *				BackColors() { return m_phecBackColors; }

		// Gets a background color.
		inline const MX_HEX_EDITOR_COLORS *			BackColors() const { return m_phecBackColors; }

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

		// Gets the area separator width in pixels.
		int32_t										AreaSeparatorWidth() const { return CurStyle()->i32PadBetweenNumbersAndTextPx + 6; }

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
		/** Scroll/view data. */
		struct MX_SCROLL_DATA {
			uint64_t								ui64StartAddress	= 0;								// Starting address bias (Set Starting Address).
			uint64_t								ui64FirstVisibleLine= 0;								// Top visible line index (0-based).

			uint32_t								ui32VisibleLines	= 0;								// Count of visible lines.

			uint64_t								ui64VPos			= 0;								// First visible line (virtual).
			uint64_t								ui64HPx				= 0;								// Horizontal scroll offset.

			int32_t									i32PageLines		= 0;								// How many text rows fit.
			int32_t									i32PageCols			= 0;								// How many text columns fit.
		};

		// == Members.
		MX_STYLE *									m_psOptions = nullptr;									// Pointer to the shared options.
		MX_FONT_SET *								m_pfsFonts[2] = {};										// Shared fonts.
		
		MX_HEX_EDITOR_COLORS *						m_phecForeColors = nullptr;								// Pointer to shared foreground colors.
		MX_HEX_EDITOR_COLORS *						m_phecBackColors = nullptr;								// Pointer to shared background colors.
		MX_EDIT_AS									m_eaEditAs = MX_EA_HEX;									// The view type.
		MX_SCROLL_DATA								m_sdScrollView[MX_EA_TOTAL];							// The scroll/view data for each view type.
		std::map<uint64_t, COLORREF>				m_mColorLookup;											// Quick mixing of colors.

		CHexEditorInterface::CBuffer				m_bCurBuffer;											// The current data buffer being displayed.

		CHexEditorInterface *						m_pheiTarget = nullptr;									// The stream of data to handle.

		// The main window class.
		static ATOM									m_aAtom;


		// == Functions.
		// Gets the current style settings.
		MX_STYLE *									CurStyle() { return &m_psOptions[m_eaEditAs]; }

		// Gets the current style settings.
		const MX_STYLE *							CurStyle() const { return &m_psOptions[m_eaEditAs]; }

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
		void										DrawAddressGutter( HDC _hDc, int32_t _iXLeft, int32_t _iYTop, uint32_t _ui32LinesToDraw );

		/**
		 * Draws the ruler at the given position.
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
		 *  - Each group label is centered within its group rect, computed by GetTextRectForIndex().
		 *  - The rulerfs height equals the base character height for the font; line spacing is ignored.
		 */
		void										DrawRuler( HDC _hDc, int32_t _iXLeft, int32_t _iYTop, MX_DATA_FMT _dfLeftFmt, MX_DATA_FMT _dfRightFmt );

		/**
		 * Draws an area at a given position.
		 * 
		 * \param _hDc Destination HDC.
		 * \param _iXLeft Pixel X where the scrollable content begins (after gutter; already includes horizontal scroll offset).
		 * \param _iYTop Pixel Y top where the area should be drawn.
		 * \param _dfFmt Format of the area.
		 * \param _ui32LinesToDraw The number of rows to draw (typically page lines).
		 * \param _bData The actual values at the addresses to render.
		 **/
		void										DrawArea( HDC _hDc, int32_t _iXLeft, int32_t _iYTop, MX_DATA_FMT _dfFmt, uint32_t _ui32LinesToDraw, const CHexEditorInterface::CBuffer &_bData );

		// Computes font metrics.
		void										ComputeFontMetrics() {
			ComputeFontMetrics( (*Font()), m_wDpiY, Wnd() );
		}

		/**
		 * \brief Returns the vertical advance, in pixels, between consecutive rows.
		 *
		 * Description: Base line height (m_iCyChar or measured glyph height) plus CurStyle()->i32LineSpacingPx.
		 */
		int32_t										LineAdvanceCy() const {
			return Font()->iCharCy + CurStyle()->i32LineSpacingPx;
		}

		// Gets the total number of lines in the display.
		inline uint64_t								TotalLines_FixedWidth() const {
			if ( !CurStyle()->uiBytesPerRow ) { return 0ULL; }
			return (Size() + CurStyle()->uiBytesPerRow - 1ULL) / CurStyle()->uiBytesPerRow;
		}

		/**
		 * Gets the color of a hex/octal/binary cell by address.
		 * 
		 * \param _ui64Address The address of the data in the cell.
		 * \param _pui8Value The address of the data in the cell.
		 * \param _sSize The address of the data in the cell.
		 * \return Returns the background color for the given cell.
		 **/
		COLORREF									CellBgColor( uint64_t _ui64Address, const uint8_t * _pui8Value, size_t _sSize );

		// Gets the minimum address digits.
		uint32_t									MinAddrDigits() const {
			uint64_t ui64Rows = Size() / CurStyle()->uiBytesPerRow;
			uint64_t ui64TotalLines = ui64Rows * CurStyle()->uiBytesPerRow;
			uint32_t ui32Result = 4;
			switch( CurStyle()->daAddressStyle.afFormat ) {
				case MX_AF_BYTES_HEX :		{ ui32Result = uint32_t( DigitCount( ui64TotalLines, 16 ) ); break; }
				case MX_AF_BYTES_DEC :		{ ui32Result = uint32_t( DigitCount( ui64TotalLines, 10 ) ); break; }
				case MX_AF_BYTES_OCT :		{ ui32Result = uint32_t( DigitCount( ui64TotalLines, 8 ) ); break; }
				case MX_AF_LINE_NUMBER :	{ ui32Result = uint32_t( DigitCount( ui64Rows + 1, 10 ) ); break; }
				case MX_AF_SHORT_HEX :		{ ui32Result = uint32_t( DigitCount( (ui64TotalLines) >> 1, 16 ) ); break; }
				case MX_AF_SHORT_DEC :		{ ui32Result = uint32_t( DigitCount( (ui64TotalLines) >> 1, 10 ) ); break; }
				default :					{ ui32Result = uint32_t( DigitCount( ui64TotalLines, 16 ) ); break; }
			}
			return std::max( ui32Result, 4U );
		}

		// Gets the address size (number of digits).
		inline int32_t								AddrDigitsForSize( uint64_t _ui64Size ) const {
			return (_ui64Size > UINT_MAX) ? 16 : 8;
		}

		/**
		 * Determines whether the current Edit As type is a fixed-width type (hex, octal, or binary).
		 * 
		 * \return Returns true if the view type is Hex, Binary, Code, or a process.
		 **/
		inline bool									IsFixedRowLength() const { return m_eaEditAs == MX_EA_HEX || m_eaEditAs == MX_EA_BINARY || m_eaEditAs == MX_EA_CUR_PROCESS || m_eaEditAs == MX_EA_PROCESS; }

		/**
		 * Computes the pixel width of the address gutter for the current options and font.
		 *
		 * \return Returns the pixel width of the address gutter. Returns 0 if the gutter is hidden.
		 */
		int32_t										ComputeAddressGutterWidthPx();

		/**
		 * \brief Returns the ruler height in pixels.
		 *
		 * Description: The ruler height equals the base character height for the current font
		 *  (line spacing does not apply to the ruler). Returns 0 if the ruler is hidden.
		 */
		int32_t										GetRulerHeightPx() const {
			if ( !CurStyle()->bShowRuler ) { return 0; }
			return Font()->iCharCy;
		}

		/**
		 * \brief Computes the left X and width (in pixels) of a text cell at index for an area.
		 *
		 * \param _dfDataFmt Data format of the area (HEX/DEC/OCT/BIN/CHAR).
		 * \param _ui32Index Zero-based cell index within the row.
		 * \param _iXBase The pixel X of the beginning of the area (not including gutter; includes horizontal scroll offset).
		 * \param _iXLeft [out] Receives the pixel X of the groupfs left edge.
		 * \param _iWidth [out] Receives the pixel width of the group (internal bytes/chars and normal intra-byte spacing).
		 * \return Returns true if the index is valid for the current layout; false otherwise.
		 */
		bool										GetTextRectForIndex(
			MX_DATA_FMT _dfDataFmt,
			uint32_t _ui32Index,
			int32_t _iXBase,
			int32_t &_iXLeft,
			int32_t &_iWidth ) const;

		/**
		 * \brief Computes the left X and width (in pixels) of a background cell at index for an area.
		 *
		 * \param _dfDataFmt Data format of the area (HEX/DEC/OCT/BIN/CHAR).
		 * \param _ui32Index Zero-based cell index within the row.
		 * \param _iXBase The pixel X of the beginning of the area (not including gutter; includes horizontal scroll offset).
		 * \param _iXLeft [out] Receives the pixel X of the groupfs left edge.
		 * \param _iWidth [out] Receives the pixel width of the group (internal bytes/chars and normal intra-byte spacing).
		 * \return Returns true if the index is valid for the current layout; false otherwise.
		 */
		bool										GetBackgrondRectForIndex(
			MX_DATA_FMT _dfDataFmt,
			uint32_t _ui32Index,
			int32_t _iXBase,
			int32_t &_iXLeft,
			int32_t &_iWidth ) const;

		/**
		 * Computes the pixel width of the left numeric (hex/dec/oct/bin/char) column for one row.
		 *
		 * \param _dfDataFmt Data format of the area (HEX/DEC/OCT/BIN/CHAR).
		 * \return Returns the pixel width of the left numbers block; 0 if hidden.
		 */
		int32_t										ComputeAreaWidthPx( MX_DATA_FMT _dfDataFmt );

		/**
		 * Computes the total width, in pixels, of the horizontally scrollable content (one row).
		 *
		 * \return Returns the pixel width of the scrollable content area (numbers + ASCII). Always >= 1.
		 */
		inline int32_t								TotalContentWidthPx() {
			const MX_STYLE & stAll = (*CurStyle());
			int32_t iCx = 0;
			iCx += ComputeAreaWidthPx( stAll.dfLeftNumbersFmt );
			if ( CurStyle()->bShowRightArea ) {
				iCx += stAll.i32PadNumbersLeftPx + stAll.i32PadNumbersRightPx;
				iCx += stAll.i32PadBetweenNumbersAndTextPx + ComputeAreaWidthPx( stAll.dfRightNumbersFmt );
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
		int32_t										GetClientWidth() {
			const MX_STYLE & asAddrStyle = (*CurStyle());

			int32_t iW = int32_t( ClientRect().Width()); //m_iClientW;
			if ( asAddrStyle.bShowAddressGutter ) {
				iW -= ComputeAddressGutterWidthPx();          // Includes i32PadAfterGutterPx.
			}
			iW -= asAddrStyle.i32PadScrollableLeftPx;
			iW -= asAddrStyle.i32PadScrollableRightPx;

			if ( asAddrStyle.bShowMiniMap ) {
				iW -= (asAddrStyle.i32MiniMapWidthPx + asAddrStyle.i32PadBeforeMiniMapPx);
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
			::GetTextExtentPoint32W( _hDc, L":", 1, &sSize ); agGlyphs.i32ColonCx = sSize.cx;
			::GetTextExtentPoint32W( _hDc, L" ", 1, &sSize ); agGlyphs.i32SpaceCx = (sSize.cx + 1) / 2 * 2;	// Round up to the nearest even number.
			::GetTextExtentPoint32W( _hDc, L"h", 1, &sSize ); agGlyphs.i32SpecHexCx = sSize.cx;
			::GetTextExtentPoint32W( _hDc, L"o", 1, &sSize ); agGlyphs.i32SpecOctCx = sSize.cx;
			::GetTextExtentPoint32W( _hDc, L"w", 1, &sSize ); agGlyphs.i32ShortWcx  = sSize.cx;

			agGlyphs.i32MaxAscii = 0;
			agGlyphs.i32MaxAsciiAnsi = 0;
			for ( wchar_t I = 32; I < 256; ++I ) {
				if ( CUtilities::ByteIsPrintable( I, false ) ) {
					::GetTextExtentPoint32W( _hDc, &I, 1, &sSize );
					agGlyphs.i32MaxAscii = std::max<int32_t>( agGlyphs.i32MaxAscii, sSize.cx );
				}
				if ( CUtilities::ByteIsPrintable( I, true ) ) {
					::GetTextExtentPoint32W( _hDc, &I, 1, &sSize );
					agGlyphs.i32MaxAsciiAnsi = std::max<int32_t>( agGlyphs.i32MaxAsciiAnsi, sSize.cx );
				}
			}
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

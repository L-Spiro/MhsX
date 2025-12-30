#pragma once

#include "../MXMhsX.h"
#include "../Layouts/MXUiIds.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include "MXCharSets.h"
#include "MXHexEditorColors.h"
#include "MXHexEditorInterface.h"
#include "MXMiniMap.h"

#include <LSWWin.h>
#include <Helpers/LSWHelpers.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>


namespace mx {

	class CHexEditorControl : public lsw::CWidget {
	public :
		CHexEditorControl( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, lsw::CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Enumerations.
		/** Selection mode. */
		enum MX_SELECTION_MODE : uint32_t {
			MX_SM_NORMAL = 0,						/**< Normal mode. */
			MX_SM_COLUMN = 1						/**< Column mode. */
		};

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
			MX_AF_BYTES_HEX,						/**< Example: 1:1D9E:53C8 */
			MX_AF_BYTES_DEC,						/**< Example: 6562337568 (right-aligned, padded with spaces on the left) */
			MX_AF_BYTES_OCT,						/**< Example: 60411251440 */
			MX_AF_LINE_NUMBER,						/**< Example: 820292197 (right-aligned, padded with spaces on the left) */
			MX_AF_SHORT_HEX,						/**< Example: C392:A990w */
			MX_AF_SHORT_DEC,						/**< Example: 2395961428w (right-aligned, padded with spaces on the left) */
		};

		/** Data display for the left and right columns. */
		enum MX_DATA_FMT {
			MX_DF_HEX,								/**< "00 FF 2A" */
			MX_DF_DEC,								/**< "000 255 042" */
			MX_DF_OCT,								/**< "000 377 052" */
			MX_DF_BIN,								/**< "00000000" */
			MX_DF_CHAR,								/**< Textual: printable byte or '.' */
			MX_DF_UINT8,							/**< "  0 255  42" */
			MX_DF_INT8,								/**< "  0  -1  42" */
			MX_DF_UINT16,							/**< "    0 65535    42" */
			MX_DF_INT16,							/**< "    0    -1    42" */
			MX_DF_UINT32,							/**< "         0 4294967295         42" */
			MX_DF_INT32,							/**< "         0         -1         42" */
			MX_DF_UINT64,							/**< "                   0 18446744073709551615                   42" */
			MX_DF_INT64,							/**< "                   0                   -1                   42" */
			MX_DF_POINTER16,						/**< "FFFF" */
			MX_DF_POINTER32,						/**< "FFFFFFFF" */
			MX_DF_POINTER64,						/**< "FFFFFFFFFFFFFFFF" */
			MX_DF_FLOAT10,							/**< "0 3.125 -42" */
			MX_DF_FLOAT11,							/**< "0 3.125 -42" */
			MX_DF_BFLOAT16,							/**< "0 3.141 -42" */
			MX_DF_FLOAT16,							/**< "0 3.1406 -42" */
			MX_DF_FLOAT32,							/**< "0 3.1415925 -42" */
			MX_DF_FLOAT64,							/**< "0 3.1415926535897931 -42" */

			MX_DF_PCM8,								/**< "0 -1 0.1647" */
			MX_DF_PCM16,							/**< "0 -1 0.1647" */
			MX_DF_PCM24,							/**< "0 -1 0.1647" */
			MX_DF_PCM32,							/**< "0 -1 0.1647" */
		};

		/** Ruler formats. */
		enum MX_RULER_FMT {
			MX_RF_HEX,
			MX_RF_DEC,
		};

		// == Types.
		/** Measured glyph metrics for the address gutter font. */
		struct MX_ADDR_GLYPHS {
			int32_t									i320_9DigitMaxCx				= 0;						// Max over '0'..'9'.
			int32_t									i32DigitMaxCx					= 0;						// Max over '0'..'9' (and 'A'..'F' when HEX).
			int32_t									i32DigitMaxCxLower				= 0;						// Max over '0'..'9' (and 'a'..'f' when HEX).
			int32_t									i32ColonCx						= 0;						// Width of ':'.
			int32_t									i32SpaceCx						= 0;						// Width of ' '.
			int32_t									i32SpecHexCx					= 0;						// Width of 'h'.
			int32_t									i32SpecOctCx					= 0;						// Width of 'o'.
			int32_t									i32ShortWcx						= 0;						// Width of 'w'.
			int32_t									i32PeriodCx						= 0;						// Width of '.'.
			int32_t									i32Ecx							= 0;						// Width of 'e'.
			int32_t									i32MinusCx						= 0;						// Width of '-'.
			int32_t									i32PlusMinusSpaceMaxCx			= 0;						// Max width between ' ', '+', and '-'.
			int32_t									i32MaxAscii						= 0;						// Maximum width of ASCII characters.
			int32_t									i32MaxAsciiAnsi					= 0;						// Maximum width of ASCII+ANSI characters.
		};

		/** A set of font data. */
		struct MX_FONT_SET {
			lsw::LSW_FONT							fFont;														/**< Current font. */
			LOGFONTW								lfFontParms {};												/**< The current font parameters. */
			TEXTMETRICW								tmMetrics {};												/**< The current font's metrics. */
			MX_ADDR_GLYPHS							agGlyphs;													/**< Glyph settings (filled on demand). */
			int32_t									i32PointSize					= DefaultPointSize();		/**< The font point size. */

			int32_t									iCharCx							= 0;
			int32_t									iCharCy							= 0;						/**< Baseline advance. */
		};

		/** What to draw and how to size the address gutter. */
		struct MX_ADDR_STYLE {
			MX_ADDRESS_FMT							afFormat						= MX_AF_BYTES_HEX;			/**< Address format. */
			bool									bLowercaseHex					= false;					/**< Use lowercase a..f for HEX. */
			bool									bShowColonIn					= true;						/**< Insert ':' every 4 HEX digits (>4 only). */
			bool									bShowColonAfter					= false;					/**< Append ':' after the address. */
			bool									bShowTypeSpec					= false;					/**< Append 'h' (hex) or 'o' (oct). */
			bool									bMinimizeDigits					= false;					/**< Size digits for current page, not whole file. */
			bool									bUseShortSuffixW				= true;						/**< Append 'w' for Short addressing. */
		};

		/** Per-view-type style settings. */
		struct MX_STYLE {
			MX_FONT_SET								fsFonts[2]						= {};						/**< Shared fonts. */
			MX_FONT_TYPE							ftFont							= MX_FT_FIXED_ROW;			/**< The shared font to use. */
			CCharSets::MX_CHAR_SETS					csCharSet						= CCharSets::MX_CS_ASCII;	/**< The current character set. */
			MX_ADDR_STYLE							daAddressStyle;												/**< Address style. */
			uint32_t								uiBytesPerRow					= 16;						/**< Bytes per displayed row. */
			uint32_t								uiDesiredBytesPerRow			= 16;						/**< Desired bytes per row. */
			uint64_t								ui64DivisionSpacing				= 4;						/**< Spacing between division lines. */
			uint32_t								ui32FloatDigits					= 18;						/**< Number of digits to display for floating-point numbers. */

			// Visibility.
			bool									bShowAddressGutter				= true;
			bool									bShowLeftNumbers				= true;						/**< Hex/Dec/Oct/Bin column. */
			bool									bShowRightArea					= true;						/**< ASCII column. */
			bool									bShowMiniMap					= true;						/**< Fixed panel on right. */
			bool									bShowRuler						= true;						/**< Show/hide ruler row. */
			bool									bShowRulerLabels				= true;						/**< Show/hide ruler labels. */
			bool									bShowRulerArrows				= true;						/**< Show/hide ruler arrow. */
			bool									bSelectColumnMode				= false;					/**< Global selection mode. */

			// Left numbers formatting.
			MX_DATA_FMT								dfLeftNumbersFmt				= MX_DF_HEX;				/**< Left area view type. */
			uint32_t								uiGroupSize						= 1;                   		/**< Extra spacing after every N bytes. */
			uint32_t								uiSpacesBetweenBytes			= 1;          				/**< Count of ' ' between adjacent bytes. */
			uint32_t								uiExtraSpacesBetweenGroups		= 1;     					/**< Extra ' ' at group boundary. */

			// Right formatting.
			MX_DATA_FMT								dfRightNumbersFmt				= MX_DF_CHAR;				/**< Right area view type. */

			// Paddings/gaps (pixels).
			int32_t									i32LeftAddrPadding				= 3;						/**< Left padding. */
			int32_t									i32PadAfterGutterPx				= 8;
			int32_t									i32PadBetweenNumbersAndTextPx	= 3;
			int32_t									i32PadNumbersLeftPx				= 3;
			int32_t									i32PadNumbersRightPx			= 3;
			int32_t									i32PadScrollableLeftPx			= 0;
			int32_t									i32PadScrollableRightPx			= 0;
			int32_t									i32PadBeforeMiniMapPx			= 1;
			int32_t									i32LineSpacingPx				= 2;

			// Ruler.
			MX_RULER_FMT							rfRulerFmt						= MX_RF_HEX;				/**< The ruler format. */
			bool									bDecimal						= false;					/**< Decimal numbers instead of hex. */
			bool									bNumbers						= true;						/**< Show numbers in the ruler. */

			// Mini-map geometry (pixels).
			int32_t									i32MiniMapWidthPx				= 16 * 4 + 1;
			CMiniMap::MX_MINI_MAP					mmMiniMap;													/**< Mini Map options. */

			// Highlighting.
			bool									bHighlightNewLines				= false;					/**< Highlight new-line characters. */
			bool									bHighlightAlphaNumeric			= false;					/**< Highlight alphanumeric characters. */
			bool									bHighlightControl				= false;					/**< Highlight control characters. */
			bool									bHighlightNonAscii				= false;					/**< Highilght characters above 127. */
			bool									bHighlightZeros					= false;					/**< Highlight zeros. */
			bool									bHighlightPointers				= false;					/**< Highlight pointers. */
		};

		/** Creation parameters. */
		struct MX_CREATION_PARMS {
			MX_STYLE *								psOptions						= nullptr;					/**< Shared options. */
			MX_FONT_SET *							pfsFixedRowFont					= nullptr;					/**< The shared font for fixed-row views. */
			MX_FONT_SET *							pfsDynamicRowFont				= nullptr;					/**< The shared font for text views. */
			MX_HEX_EDITOR_COLORS *					phecFg							= nullptr;					/**< Foreground colors. */
			MX_HEX_EDITOR_COLORS *					phecBg							= nullptr;					/**< Background colors. */
		};

		/** Selection ranges. */
		struct MX_SEL_RANGE {
			uint64_t								ui64Start						= 0;						/**< The start of a selection range. */
			uint64_t								ui64Size						= 0;						/**< The size of the selection range. */
		};


		// == Functions.
		// Sets the view type.
		void										SetViewType( MX_EDIT_AS _eaType );

		// Gets the editing type of the control.
		inline MX_EDIT_AS							GetViewType() const { return m_eaEditAs; }

		// Sets the character set.
		void										SetCharacterSet( CCharSets::MX_CHAR_SETS _csSet ) {
			if ( CurStyle()->csCharSet != _csSet ) {
				CurStyle()->csCharSet = _csSet;

				RecalcAndInvalidate();
			}
		};

		// Gets the current character set.
		inline CCharSets::MX_CHAR_SETS				GetCharacterSet() const { return CurStyle()->csCharSet; }

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
			auto & stAll = (*CurStyle());
			stAll.uiDesiredBytesPerRow = _ui32Width;
			// TODO: Set Auto Line Width to false.
				
			auto ui32PrimSize = StrideForFormat( stAll.dfLeftNumbersFmt );
			if ( stAll.bShowRightArea ) {
				ui32PrimSize = ee::CExpEval::Lcm( ui32PrimSize, StrideForFormat( stAll.dfRightNumbersFmt ) );
			}
			// Effective width must be a common denominator of the view strides.
			uint32_t ui32LcdWidth = ee::CExpEval::Lcm( ui32PrimSize, _ui32Width );
			uint32_t ui32LcdWidth2 = ui32LcdWidth << 1;
			// Set effective width to the nearest multiple.
			_ui32Width = ((_ui32Width * 2) + ui32LcdWidth) / ui32LcdWidth2 * ui32LcdWidth;
			_ui32Width = std::max( _ui32Width, ui32LcdWidth );
			if ( stAll.uiBytesPerRow != _ui32Width && _ui32Width >= 1 ) {
				stAll.uiBytesPerRow = _ui32Width;
				// Group size must be a multiple of the line size.
				if ( _ui32Width % stAll.uiGroupSize ) {
					uint32_t I;
					for ( I = 1; I != _ui32Width; ++I ) {
						if ( I ) {
							if ( (_ui32Width % I) == 0 ) {
								if ( (_ui32Width / I) <= stAll.uiGroupSize ) {
									break;
								}
							}
						}
					}
					stAll.uiGroupSize = _ui32Width / I;
				}

				RecalcAndInvalidate();
			}
		}

		// Gets the row width.
		inline uint32_t								GetLineWidth() const { return CurStyle()->uiBytesPerRow; }

		// Sets thr row width to Auto.
		void										SetLineWidthdAuto( bool /*_bSet*/ ) {
			//CurStyle()->bAuto
		}

		// Gets the Auto status of the row width.
		inline bool									GetLineWidthAuto() const { return false; }

		// Sets the group size.
		void										SetGroupSize( uint32_t _ui32GroupSize ) {
			auto & stAll = (*CurStyle());
			if ( _ui32GroupSize != 1 ) {
				auto ui32PrimSize = StrideForFormat( stAll.dfLeftNumbersFmt );
				if ( stAll.bShowRightArea ) {
					ui32PrimSize = ee::CExpEval::Lcm( ui32PrimSize, StrideForFormat( stAll.dfRightNumbersFmt ) );
				}
				if ( std::max( ui32PrimSize, _ui32GroupSize ) % std::min( ui32PrimSize, _ui32GroupSize ) ) {
					// Effective width must be a common denominator of the view strides.
					uint32_t ui32LcdWidth = ee::CExpEval::Lcm( ui32PrimSize, _ui32GroupSize );
					uint32_t ui32LcdWidth2 = ui32LcdWidth << 1;
					// Set effective width to the nearest multiple.
					_ui32GroupSize = ((_ui32GroupSize * 2) + ui32LcdWidth) / ui32LcdWidth2 * ui32LcdWidth;
					_ui32GroupSize = std::max( _ui32GroupSize, ui32LcdWidth );
				}
			}
			if ( stAll.uiGroupSize != _ui32GroupSize && _ui32GroupSize >= 1 ) {
				stAll.uiGroupSize = _ui32GroupSize;
				// Line width umust be a multiple of group size.
				if ( stAll.uiBytesPerRow % _ui32GroupSize ) {
					stAll.uiBytesPerRow = (stAll.uiBytesPerRow + (_ui32GroupSize - 1)) / _ui32GroupSize * _ui32GroupSize;
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
				SetLineWidth( CurStyle()->uiDesiredBytesPerRow );
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
				SetLineWidth( CurStyle()->uiDesiredBytesPerRow );
				RecalcAndInvalidate();
			}
		}

		// Gets the right-area type.
		inline MX_DATA_FMT							GetRightAreaFormat() const { return CurStyle()->dfRightNumbersFmt; }

		// Hides the right area.
		void										HideRightArea() {
			if ( CurStyle()->bShowRightArea ) {
				CurStyle()->bShowRightArea = false;
				SetLineWidth( CurStyle()->uiDesiredBytesPerRow );
				RecalcAndInvalidate();
			}
		}

		// Gets the right-area-hidden setting.
		inline bool									RightAreaHidden() const { return !CurStyle()->bShowRightArea; }

		// Sets line-feed highlighting.
		void										SetHighlightingLinefeed( bool _bEnable ) {
			CurStyle()->bHighlightNewLines = _bEnable;
			::InvalidateRect( Wnd(), NULL, FALSE );
		}

		// Gets line-feed highlighing.
		inline bool									GetHighlightingLinefeed() const { return CurStyle()->bHighlightNewLines; }

		// Sets alphanumerica highlighting.
		void										SetHighlightingAlphaNumeric( bool _bEnable ) {
			CurStyle()->bHighlightAlphaNumeric = _bEnable;
			::InvalidateRect( Wnd(), NULL, FALSE );
		}

		// Gets alphanumerica highlighing.
		inline bool									GetHighlightingAlphaNumeric() const { return CurStyle()->bHighlightAlphaNumeric; }

		// Sets control highlighting.
		void										SetHighlightingControl( bool _bEnable ) {
			CurStyle()->bHighlightControl = _bEnable;
			::InvalidateRect( Wnd(), NULL, FALSE );
		}

		// Gets control highlighing.
		inline bool									GetHighlightingControl() const { return CurStyle()->bHighlightControl; }

		// Sets non-ASCII highlighting.
		void										SetHighlightingNonAscii( bool _bEnable ) {
			CurStyle()->bHighlightNonAscii = _bEnable;
			::InvalidateRect( Wnd(), NULL, FALSE );
		}

		// Gets non-ASCII highlighing.
		inline bool									GetHighlightingNonAscii() const { return CurStyle()->bHighlightNonAscii; }

		// Sets zeros highlighting.
		void										SetHighlightingZeros( bool _bEnable ) {
			CurStyle()->bHighlightZeros = _bEnable;
			::InvalidateRect( Wnd(), NULL, FALSE );
		}

		// Gets zeros highlighing.
		inline bool									GetHighlightingZeros() const { return CurStyle()->bHighlightZeros; }

		// Sets pointer highlighting.
		void										SetHighlightingPointers( bool _bEnable ) {
			CurStyle()->bHighlightPointers = _bEnable;
			::InvalidateRect( Wnd(), NULL, FALSE );
		}

		// Gets pointer highlighing.
		inline bool									GetHighlightingPointers() const { return CurStyle()->bHighlightPointers; }

		// Sets the showing of the ruler.
		void										SetShowRuler( bool _bEnable ) {
			CurStyle()->bShowRuler = _bEnable;
			::InvalidateRect( Wnd(), NULL, FALSE );
		}
		
		// Gets the showing of the ruler.
		inline bool									GetShowRuler() const { return CurStyle()->bShowRuler; }

		// Sets the showing of the ruler caret.
		void										SetShowRulerCaret( bool _bEnable ) {
			CurStyle()->bShowRulerArrows = _bEnable;
			::InvalidateRect( Wnd(), NULL, FALSE );
		}
		
		// Gets the showing of the ruler caret.
		inline bool									GetShowRulerCaret() const { return CurStyle()->bShowRulerArrows; }

		// Sets the showing of the ruler labels.
		void										SetShowRulerLabels( bool _bEnable ) {
			CurStyle()->bShowRulerLabels = _bEnable;
			if ( CurStyle()->bShowRuler ) {
				::InvalidateRect( Wnd(), NULL, FALSE );
			}
		}
		
		// Gets the showing of the ruler labels.
		inline bool									GetShowRulerLabels() const { return CurStyle()->bShowRulerLabels; }

		// Sets the ruler format.
		void										SetRulerFormat( MX_RULER_FMT _rfFmt ) {
			CurStyle()->rfRulerFmt = _rfFmt;
			if ( CurStyle()->bShowRuler ) {
				::InvalidateRect( Wnd(), NULL, FALSE );
			}
		}

		// Gets the ruler format.
		inline MX_RULER_FMT							GetRulerFormat() const { return CurStyle()->rfRulerFmt; }

		// Sets the showing of the ruler caret.
		void										SetShowMiniMap( bool _bEnable ) {
			CurStyle()->bShowMiniMap = _bEnable;
			::InvalidateRect( Wnd(), NULL, FALSE );
		}
		
		// Gets the showing of the ruler caret.
		inline bool									GetShowMiniMap() const { return CurStyle()->bShowMiniMap; }

		/**
		 * Determines if a process is being viewed.
		 * 
		 * \return Returns true if the current view is to a process.
		 **/
		inline bool									IsProcess() const {
			if ( m_pheiTarget ) {
				return m_pheiTarget->Type() == CHexEditorInterface::MX_HET_PROCESS ||
					m_pheiTarget->Type() == CHexEditorInterface::MX_HET_CUR_PROCESS;
			}
			return false;
		}

		/**
		 * Gets the caret address.
		 * 
		 * \return Returns the caret address.
		 **/
		inline uint64_t								GetCaretAddr() const { return m_sgSelGesture.ui64CaretAddr; }

		/**
		 * Sets the caret address.
		 * 
		 * \param _ui64Addr The new address to set as the caret position.
		 * \param _i32SubCaret The caret subindex.
		 * \param _bUpdateSelection If true, the selection is updated.  If false, the selection is cleared.
		 * \param _bRedraw If true the screen is redrawn immediately.
		 **/
		void										SetCaretAddr( uint64_t _ui64Addr, int32_t _i32SubCaret = 0, bool _bUpdateSelection = false, bool _bRedraw = true );

		/**
		 * Moves the caret forward (to a higher address) by the given amount.
		 * 
		 * \param _ui64Amount The amount by which to increase the caret position.
		 * \param _i32SubCaret The caret subindex.
		 * \param _bUpdateSelection If true, the selection is updated.  If false, the selection is cleared.
		 * \param _bRedraw If true the screen is redrawn immediately.
		 **/
		inline void									AddCaret( uint64_t _ui64Amount, int32_t _i32SubCaret = 0, bool _bUpdateSelection = false, bool _bRedraw = true ) {
			bool bRight = m_sgSelGesture.bRightArea && CurStyle()->bShowRightArea;
			auto dfFmt = bRight ? CurStyle()->dfRightNumbersFmt : CurStyle()->dfLeftNumbersFmt;
			auto ui32Stride = StrideForFormat( dfFmt );
			// ui32Stride - (m_sgSelGesture.ui64CaretAddr % ui32Stride)
			if ( _ui64Amount % ui32Stride ) {
				_ui64Amount = ((_ui64Amount / ui32Stride) + 1) * ui32Stride;
			}

			auto ui64NewAddr = GetCaretAddr() + _ui64Amount;
			if ( ui64NewAddr < GetCaretAddr() ) { SetCaretAddr( Size(), _i32SubCaret, _bUpdateSelection, _bRedraw ); }
			else { SetCaretAddr( ui64NewAddr, _i32SubCaret, _bUpdateSelection, _bRedraw ); }
		}

		/**
		 * Moves the caret backward (to a lower address) by the given amount.
		 * 
		 * \param _ui64Amount The amount by which to decrease the caret position.
		 * \param _i32SubCaret The caret subindex.
		 * \param _bUpdateSelection If true, the selection is updated.  If false, the selection is cleared.
		 * \param _bRedraw If true the screen is redrawn immediately.
		 **/
		inline void									SubCaret( uint64_t _ui64Amount, int32_t _i32SubCaret = 0, bool _bUpdateSelection = false, bool _bRedraw = true ) {
			bool bRight = m_sgSelGesture.bRightArea && CurStyle()->bShowRightArea;
			auto dfFmt = bRight ? CurStyle()->dfRightNumbersFmt : CurStyle()->dfLeftNumbersFmt;
			auto ui32Stride = StrideForFormat( dfFmt );
			// ui32Stride - (m_sgSelGesture.ui64CaretAddr % ui32Stride)
			if ( _ui64Amount % ui32Stride ) {
				_ui64Amount = ((_ui64Amount / ui32Stride) + 1) * ui32Stride;
			}

			if ( _ui64Amount >= GetCaretAddr() ) { SetCaretAddr( 0, _i32SubCaret, _bUpdateSelection, _bRedraw ); }
			else { SetCaretAddr( GetCaretAddr() - _ui64Amount, _i32SubCaret, _bUpdateSelection, _bRedraw ); }
		}

		/**
		 * Increases the sub-caret by 1.
		 * 
		 * \param _bRedraw If true the screen is redrawn immediately.
		 **/
		inline void									IncSubCaret( bool _bRedraw = true ) {
			if ( HasSelection() ) {
				SetCaretAddr( m_sSel.UpperRightAddress( CurStyle()->uiBytesPerRow ), 0, false, _bRedraw );
			}
			else {
				bool bRight = m_sgSelGesture.bRightArea && CurStyle()->bShowRightArea;
				auto dfFmt = bRight ? CurStyle()->dfRightNumbersFmt : CurStyle()->dfLeftNumbersFmt;
				int32_t i32Max = SubCaretCountForFormat( dfFmt );
				auto ui32Stride = StrideForFormat( dfFmt );
				if ( ui32Stride > 1 ) {
					AddCaret( ui32Stride - (m_sgSelGesture.ui64CaretAddr % ui32Stride), 0, false, _bRedraw );
				}
				else if ( std::clamp( m_sgSelGesture.i32CaretIdx, 0, i32Max ) == i32Max - 1 ) {
					AddCaret( 1, 0, false, _bRedraw );
				}
				else {
					if ( m_sgSelGesture.ui64CaretAddr < Size() ) {
						++m_sgSelGesture.i32CaretIdx;
						StartCaretBlink();
						if ( _bRedraw ) {
							::InvalidateRect( Wnd(), nullptr, FALSE );
						}
					}
				}
			}
		}

		/**
		 * Decreases the sub-caret by 1.
		 * 
		 * \param _bRedraw If true the screen is redrawn immediately.
		 **/
		inline void									DecSubCaret( bool _bRedraw = true ) {
			if ( HasSelection() ) {
				SetCaretAddr( m_sSel.LowerLeftAddress(), 0, false, _bRedraw );
			}
			else {
				bool bRight = m_sgSelGesture.bRightArea && CurStyle()->bShowRightArea;
				auto dfFmt = bRight ? CurStyle()->dfRightNumbersFmt : CurStyle()->dfLeftNumbersFmt;
				int32_t i32Max = SubCaretCountForFormat( dfFmt );
				auto ui32Stride = StrideForFormat( dfFmt );
				if ( ui32Stride > 1 || HasSelection() ) {
					SubCaret( (m_sgSelGesture.ui64CaretAddr % ui32Stride) + ui32Stride, 0, false, _bRedraw );
				}
				else if ( std::clamp( m_sgSelGesture.i32CaretIdx, 0, i32Max ) == 0 ) {
					if ( m_sgSelGesture.ui64CaretAddr ) {
						SubCaret( 1, i32Max - 1, false, _bRedraw );
					}
					else {
						SubCaret( 1, 0, false, _bRedraw );
					}
				}
				else {
					--m_sgSelGesture.i32CaretIdx;
					StartCaretBlink();
					if ( _bRedraw ) {
						::InvalidateRect( Wnd(), nullptr, FALSE );
					}
				}
			}
		}

		/**
		 * Returns true if the selection mode is set to COLUMN.
		 * 
		 * \return Returns true if the selection mode is set to COLUMN.
		 **/
		inline bool									SelectionIsColumn() const { return CurStyle()->bSelectColumnMode; }

		/**
		 * Sets the default selection mode.
		 * 
		 * \param _bColumnMode If true, the default selection mode is COLUMN, otherwise it is NORMAL.
		 **/
		inline void									SetSelectionIsColumn( bool _bColumnMode ) { CurStyle()->bSelectColumnMode = _bColumnMode; }

		/**
		 * Determines if there are selected bytes or not.
		 * 
		 * \return Return strue if both the file size is not 0 and there is an active selection.
		 **/
		inline bool									HasSelection() const { return Size() > 0 && m_sSel.bHas; }

		/**
		 * Selects the whole file.  Not available when opening processes.
		 * 
		 * \return Returns true if the current view is not into a process.
		 **/
		bool										SelectAll();

		/**
		 * \brief Select the "word" according to improved Normal-mode semantics.
		 *
		 * Normal Mode (with an existing selection):
		 *  - Start of selection:
		 *      * If the start byte is a word character, expand toward 0 until a word boundary
		 *        (first non-word byte when scanning backward).
		 *      * Otherwise, move upward (toward the end of the selection) until a word boundary
		 *        is found or the end of the selection is reached. The word boundary in this case
		 *        is the first word byte encountered when scanning forward.
		 *  - End of selection:
		 *      * If the end byte is a word character, move toward the end of the file until a
		 *        word boundary is found (first non-word byte scanning forward).
		 *      * Otherwise, move toward 0 until a word boundary is found or the beginning of the
		 *        selection is reached. The word boundary here is the first word byte encountered
		 *        when scanning backward.
		 *
		 * Column Mode or no Normal selection:
		 *  - Falls back to caret-based behavior: If the caret is on a word byte, expand left/right
		 *    to cover that word; otherwise select the single caret byte.
		 */
		void										SelectWord();

		/**
		 * \brief Select the entire line containing the current selection or caret.
		 *
		 * In Hex View:
		 *  - NORMAL mode:
		 *      * Takes the low address of the selection (min of start/end) as the reference.
		 *  - COLUMN mode:
		 *      * Takes the lower-left corner of the selection (last row, leftmost column) as the reference.
		 *  - No selection:
		 *      * Uses the current caret address as the reference.
		 *
		 * The reference address determines the line (row) as address / bytes-per-row.
		 * The resulting selection is always a NORMAL selection from the beginning of that line
		 * (row * bytes-per-row) to the end of that line (clamped to the end of the file).
		 */
		void										SelectLine();

		/**
		 * Reverses the selection (swaps the caret and anchor points).
		 **/
		void										ReverseSelection();

		/**
		 * Sets the caret to the beginning of the selection.
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 **/
		bool										MarkSelectionStart( CSecureWString &_swsStatus );

		/**
		 * Sets the caret to the end of the selection.
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 **/
		bool										MarkSelectionEnd( CSecureWString &_swsStatus );

		/**
		 * Moves the selection back.
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 **/
		bool										MoveSelectionBack( CSecureWString &_swsStatus );

		/**
		 * Moves the selection forward.
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 **/
		bool										MoveSelectionForward( CSecureWString &_swsStatus );

		/**
		 * Determines if deleting is possible.  To delete, the internal state must be valid and the file must not be read-only.
		 * 
		 * \return Returns true if the internal state is valid and the file is not read-only.
		 **/
		inline bool									CanDelete() const {
			return m_pheiTarget && !m_pheiTarget->ReadOnly();
		}

		/**
		 * Performs an Undo operation.
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 **/
		bool										Undo( CSecureWString &_swsStatus );

		/**
		 * Performs an Redo operation.
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 * \return Returns
		 **/
		bool										Redo( CSecureWString &_swsStatus );

		/**
		 * Deletes the selected bytes.
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 * \return Returns false if _swsStatus contains an error-state string.  True indicates that the process completed normally, regardless of whether anything was deleted or not.
		 **/
		bool										DeleteSelectedOrCaret( CSecureWString &_swsStatus );

		/**
		 * Deletes the character prior to the caret (backspace).
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 * \return Returns false if _swsStatus contains an error-state string.  True indicates that the process completed normally, regardless of whether anything was deleted or not.
		 **/
		bool										DeletePriorToCaret( CSecureWString &_swsStatus );

		/**
		 * Is Undo available?
		 * 
		 * \return Returns true if Undo is available.
		 **/
		inline bool									CanUndo() const {
			return m_pheiTarget && m_pheiTarget->HasUndo();
		}

		/**
		 * Is Redo available?
		 * 
		 * \return Returns true if Redo is available.
		 **/
		inline bool									CanRedo() const {
			return m_pheiTarget && m_pheiTarget->HasRedo();
		}

		/**
		 * Goes to a given address.
		 * 
		 * \param _ui64Addr The address to which to go.
		 * \param _bShowAtTop If true, the scroll be aligned such that the address is on the top of the screen, otherwise it will be in the center of the screen.
		 * \param _bOnlyIfNotVisible If true, the view is only moved if the value is not currently in the visible area.
		 * \param _bRefresh If true, the view is updated.
		 **/
		void										GoTo( uint64_t _ui64Addr, bool _bShowAtTop = false, bool _bOnlyIfNotVisible = false, bool _bRefresh = true );

		/**
		 * Ensures a given address is visible.  If the address is already visible, no scrolling is done, otherwise it scrolls just enough to put the address on the top
		 *	or bottom of the screen.
		 * 
		 * \param _ui64Addr The address to make visible.
		 * \param _bRefresh If true, the control is redrawn.
		 **/
		void										EnsureVisible( uint64_t _ui64Addr, bool _bRefresh = true );

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

		/**
		 * Handles WM_ERASEBKGND.
		 * \brief Allows custom background erasing.
		 *
		 * \param _hDc Device context provided for erasing.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							EraseBkgnd( HDC /*_hDc*/ ) { return LSW_H_HANDLED; }

		/**
		 * Handles WM_SETFOCUS.
		 * \brief Notified when the window receives keyboard focus.
		 *
		 * \param _hPrevFocus The window that previously had focus.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							SetFocus( HWND /*_hPrevFocus*/ ) override;

		/**
		 * Handles WM_KILLFOCUS.
		 * \brief Notified when the window loses keyboard focus.
		 *
		 * \param _hNewFocus The window receiving focus (may be NULL).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							KillFocus( HWND /*_hNewFocus*/ ) override;

		/**
		 * Handles WM_SETCURSOR.
		 * \brief Sets the cursor for the window or its child controls.
		 *
		 * \param _pwControl The control under the cursor, or nullptr.
		 * \param _wHitTest Hit-test code (HT*).
		 * \param _wIdent Mouse message identifier (e.g., WM_MOUSEMOVE).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							SetCursor( CWidget * /*_pwControl*/, WORD /*_wHitTest*/, WORD /*_wIdent*/ ) override { return LSW_H_HANDLED; }

		/**
		 * Handles WM_PAINT.
		 * \brief Performs painting for the client area.
		 *
		 * \return Returns a LSW_HANDLED code.
		 */
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

		// Gets the current character set.
		inline const CCharSets::MX_CHAR_SET &		CharSet() const { return CCharSets::m_csSets[CurStyle()->csCharSet]; }

		// Gets a foreground color.
		inline MX_HEX_EDITOR_COLORS *				ForeColors() { return m_phecForeColors; }

		// Gets a foreground color.
		inline const MX_HEX_EDITOR_COLORS *			ForeColors() const { return m_phecForeColors; }

		// Gets a background color.
		inline MX_HEX_EDITOR_COLORS *				BackColors() { return m_phecBackColors; }

		// Gets a background color.
		inline const MX_HEX_EDITOR_COLORS *			BackColors() const { return m_phecBackColors; }

		/**
		 * Handles WM_TIMER.
		 * \brief Notified when a timer elapses.
		 *
		 * \param _uiptrId Timer identifier.
		 * \param _tpProc Optional callback associated with the timer.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							Timer( UINT_PTR _uiptrId, TIMERPROC /*_tpProc*/ );

		// WM_LBUTTONDOWN.
		virtual LSW_HANDLED							LButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos );

		/** \brief Handles WM_MOUSEMOVE. */
		virtual LSW_HANDLED							MouseMove( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos );

		/** \brief Handles WM_LBUTTONUP. */
		virtual LSW_HANDLED							LButtonUp( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos );

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
		void										SetStream( CHexEditorInterface * _pediStream, CWidget * _pwMainCtrl );

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

		/**
		 * Gets the address under the given mouse point.
		 * 
		 * \param _pPoint The client-space point under which to find the address.
		 * \param _ui64Addr The return address in the case that the function returns true.
		 * \param _i32SubCaret The subcaret.
		 * \param _bRightArea If true, the click happened in the right area.
		 * \return Returns true if there is a cell representing an address under the given point.  The address will always be valid (in the range of the opened file, memory space, etc.) if the function returns true.
		 **/
		bool										PointToAddress( const POINT &_pPoint, uint64_t &_ui64Addr, int32_t &_i32SubCaret, bool &_bRightArea );

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
				_fsFont.agGlyphs.i32DigitMaxCx = 0;
				//_fsFont.iCharCy = 0;
			}
		}

		// Gets the default font size (later to be replaced with a settings value).
		static int32_t								DefaultPointSize() { return 10; }


	protected :
		// == Types.
		/** Scroll/view data. */
		struct MX_SCROLL_DATA {
			uint64_t								ui64StartAddress	= 0;								/**< Starting address bias (Set Starting Address). */
			uint64_t								ui64FirstVisibleLine= 0;								/**< Top visible line index (0-based). */

			uint32_t								ui32VisibleLines	= 0;								/**< Count of visible lines. */

			uint64_t								ui64VPos			= 0;								/**< First visible line (virtual). */
			uint64_t								ui64HPx				= 0;								/**< Horizontal scroll offset. */

			int32_t									i32PageLines		= 0;								/**< How many text rows fit. */
			int32_t									i32PageCols			= 0;								/**< How many text columns fit. */
		};

		/** A Normal selection: [startAddr, endAddr] inclusive, normalized. */
		struct MX_SEL_NORMAL {
			uint64_t								ui64Start			= 0;								/**< Inclusive start address. */
			uint64_t								ui64End				= 0;								/**< Exclusive end address. */


			// == Functions.
			/** Normalize start/end so start <= end. */
			inline void								Normalize() {
				if ( ui64Start > ui64End ) { std::swap( ui64Start, ui64End ); }
			}

			/** True if a single-point selection. */
			inline bool								Empty() const { return ui64Start == ui64End; }

			/**
			 * Determines if a given address is in the selection.
			 * 
			 * \param _ui64Address The address to test for being selected.
			 * \return Returns true if there is a selection and if the given address is part of that selection.
			 **/
			inline bool								IsSelected( uint64_t _ui64Address ) {
				return _ui64Address >= ui64Start && _ui64Address < ui64End;
			}

		};

		/**
		 * Column selection anchored to a top-left address.
		 * Width is in columns (cells), height is in lines (rows). Both are >= 1 when valid.
		 * This form survives any bytes-per-row changes; visualization is realized per current layout.
		 */
		struct MX_SEL_COLUMN {
			uint64_t								ui64AnchorAddr		= 0;								/**< Absolute address of the top-left cell. */
			uint32_t								ui32Cols			= 1;								/**< Width in byte columns. */
			uint64_t								ui64Lines			= 1;								/**< Height in rows (lines). */


			// == Functions.
			/** Returns true if degenerate (treated as empty on end-gesture). */
			inline bool								Empty() const { return ui32Cols == 0 /*|| ui64Lines == 0*/; }

			/**
			 * Determines if a given address is in the selection.
			 * 
			 * \param _ui64Address The address to test for being selected.
			 * \param _ui32BytesPerRow The number of bytes in a row.
			 * \return Returns true if there is a selection and if the given address is part of that selection.
			 **/
			inline bool								IsSelected( uint64_t _ui64Address, uint32_t _ui32BytesPerRow ) {
				uint64_t ui64AnchorRow = ui64AnchorAddr / _ui32BytesPerRow;
				uint64_t ui64AnchorCol = ui64AnchorAddr % _ui32BytesPerRow;

				uint64_t ui64AddrRow = _ui64Address / _ui32BytesPerRow;
				if ( ui64AddrRow < ui64AnchorRow || ui64AddrRow > ui64AnchorRow + ui64Lines ) { return false; }

				uint64_t ui64AddrCol = _ui64Address % _ui32BytesPerRow;
				return ui64AddrCol >= ui64AnchorCol && ui64AddrCol < ui64AnchorCol + ui32Cols;
			}
		};

		/**
		 * The active selection (either Normal or Column).
		 * Stores payload for the active mode and exposes helpers shared by both.
		 */
		struct MX_SELECTION {
			MX_SELECTION_MODE						smMode				= MX_SM_NORMAL;						/**< Active selection mode. */
			bool									bHas				= false;							/**< True if there is an active selection. */
			MX_SEL_NORMAL							sn;														/**< Normal selection payload. */
			MX_SEL_COLUMN							sc;														/**< Column selection payload. */


			// == Functions.
			/**
			 * Is there an actual selection?
			 * 
			 * \return Returns true if both bHas is true and the selection region has a non-0 area.
			 **/
			inline bool								HasSelection() const {
				if ( !bHas ) { return false; }
				if ( smMode == MX_SM_NORMAL ) { return !sn.Empty(); }
				return !sc.Empty();
			}

			/**
			 * True if selection is limited to a single logical row (for Normal) or single row (for Column).
			 * Used for conversion logic.
			 *
			 * \param _ui32BytesPerRow Current bytes-per-row.
			 * \return Returns true if the selection occupies exactly one logical row.
			 */
			inline bool								IsSingleRow( uint32_t _ui32BytesPerRow ) const {
				if ( smMode == MX_SM_COLUMN ) {
					return sc.ui64Lines == 1;
				}
				const uint64_t ui64StartLine = sn.ui64Start / _ui32BytesPerRow;
				const uint64_t ui64EndLine   = sn.ui64End   / _ui32BytesPerRow;
				return ui64StartLine == ui64EndLine;
			}

			/**
			 * Determines if a given address is in the selection.
			 * 
			 * \param _ui64Address The address to test for being selected.
			 * \param _ui32BytesPerRow The number of bytes in a row.
			 * \return Returns true if there is a selection and if the given address is part of that selection.
			 **/
			inline bool								IsSelected( uint64_t _ui64Address, uint32_t _ui32BytesPerRow ) {
				if ( !bHas ) { return false; }
				if ( smMode == MX_SM_NORMAL ) {
					return sn.IsSelected( _ui64Address );
				}
				return sc.IsSelected( _ui64Address, _ui32BytesPerRow );
			}

			/**
			 * Gathers the selected ranges.  Addresses will be sorted in descanding order (higher addresses first).
			 * 
			 * \param _ui32BytesPerRow The number of bytes in a row.
			 * \param _vSelections Holds the returned selection ranges sorted from high-to-low.
			 * \return Returns true if allocating all of the selection ranges succeeded or there is no selection.  False always indicates a memory failure.
			 **/
			inline bool								GatherSelected_HighToLow( uint32_t _ui32BytesPerRow, std::vector<MX_SEL_RANGE> &_vSelections ) {
				_vSelections.clear();
				if ( !HasSelection() ) { return true; }
				try {
					if ( smMode == MX_SM_NORMAL ) {	
						// Known not to be empty.
						// There is only one selection range.
						MX_SEL_RANGE srRange = { .ui64Start = sn.ui64Start, .ui64Size = sn.ui64End - sn.ui64Start };
						_vSelections.push_back( srRange );
						return true;
					}

					// Column Mode.
					// Known not to be empty.
					for ( size_t I = sc.ui64Lines + 1; I--; ) {
						MX_SEL_RANGE srRange = { .ui64Start = sc.ui64AnchorAddr + I * _ui32BytesPerRow, .ui64Size = sc.ui32Cols };
						_vSelections.push_back( srRange );
					}
					return true;
				}
				catch ( ... ) { return false; }
			 }

			/**
			 * Gathers the selected ranges.  Addresses will be sorted in ascanding order (lower addresses first).
			 * 
			 * \param _ui32BytesPerRow The number of bytes in a row.
			 * \param _vSelections Holds the returned selection ranges sorted from low-to-high.
			 * \return Returns true if allocating all of the selection ranges succeeded or there is no selection.  False always indicates a memory failure.
			 **/
			inline bool								GatherSelected_LowToHigh( uint32_t _ui32BytesPerRow, std::vector<MX_SEL_RANGE> &_vSelections ) {
				_vSelections.clear();
				if ( !HasSelection() ) { return true; }
				try {
					if ( smMode == MX_SM_NORMAL ) {	
						// Known not to be empty.
						// There is only one selection range.
						MX_SEL_RANGE srRange = { .ui64Start = sn.ui64Start, .ui64Size = sn.ui64End - sn.ui64Start };
						_vSelections.push_back( srRange );
						return true;
					}

					// Column Mode.
					// Known not to be empty.
					for ( size_t I = 0; I <= sc.ui64Lines; ++I ) {
						MX_SEL_RANGE srRange = { .ui64Start = sc.ui64AnchorAddr + I * _ui32BytesPerRow, .ui64Size = sc.ui32Cols };
						_vSelections.push_back( srRange );
					}
					return true;
				}
				catch ( ... ) { return false; }
			 }

			 /**
			 * Gathers a part of the selected ranges.  Addresses will be grouped into chunks of _ui64GroupSize, and you can request an index into those groupings to be returned via _ui64GroupIdx.
			 *	This prevents long freezes or memory failures when millions of rows have been selected in column mode.
			 *	Addresses will be sorted in descanding order (higher addresses first).
			 * 
			 * \param _ui32BytesPerRow The number of bytes in a row.
			 * \param _vSelections Holds the returned selection ranges sorted from high-to-low.
			 * \param _ui64GroupSize The size of each group to gather.
			 * \param _ui64GroupIdx The index of the group to gather.
			 * \return Returns true if allocating all of the selection ranges succeeded or there is no selection.  False always indicates a memory failure.
			 **/
			inline bool								GatherSelected_HighToLow( uint32_t _ui32BytesPerRow, std::vector<MX_SEL_RANGE> &_vSelections, uint64_t _ui64GroupSize, uint64_t _ui64GroupIdx ) {
				_vSelections.clear();
				if ( !HasSelection() ) { return true; }
				try {
					if ( !_ui64GroupSize ) { return false; }
					if ( smMode == MX_SM_NORMAL ) {
						if ( _ui64GroupIdx != 0 ) { return true; }
						// Known not to be empty.
						// There is only one selection range.
						MX_SEL_RANGE srRange = { .ui64Start = sn.ui64Start, .ui64Size = sn.ui64End - sn.ui64Start };
						_vSelections.push_back( srRange );
						return true;
					}

					// Column Mode.
					// Known not to be empty.
					
					uint64_t ui64Total = sc.ui64Lines + 1;
					uint64_t ui64TotalGroups = (ui64Total % _ui64GroupSize == 0) ? (ui64Total / _ui64GroupSize) : (ui64Total / _ui64GroupSize + 1);
					if ( _ui64GroupIdx >= ui64TotalGroups ) { return true; }
					uint64_t ui64NewTotal = std::min( _ui64GroupSize, ui64Total - _ui64GroupSize * _ui64GroupIdx );
					uint64_t ui64Offset = ui64Total - _ui64GroupSize * _ui64GroupIdx - ui64NewTotal;
					for ( size_t I = ui64NewTotal; I--; ) {
						MX_SEL_RANGE srRange = { .ui64Start = sc.ui64AnchorAddr + (I + ui64Offset) * _ui32BytesPerRow, .ui64Size = sc.ui32Cols };
						_vSelections.push_back( srRange );
					}
					return true;
				}
				catch ( ... ) { return false; }
			 }

			/**
			 * Gathers a part of the selected ranges.  Addresses will be grouped into chunks of _ui64GroupSize, and you can request an index into those groupings to be returned via _ui64GroupIdx.
			 *	This prevents long freezes or memory failures when millions of rows have been selected in column mode.
			 *	Addresses will be sorted in ascanding order (lower addresses first).
			 * 
			 * \param _ui32BytesPerRow The number of bytes in a row.
			 * \param _vSelections Holds the returned selection ranges sorted from low-to-high.
			 * \param _ui32BytesPerRow The number of bytes in a row.
			 * \param _ui32BytesPerRow The number of bytes in a row.
			 * \return Returns true if allocating all of the selection ranges succeeded or there is no selection.  False always indicates a memory failure.
			 **/
			inline bool								GatherSelected_LowToHigh( uint32_t _ui32BytesPerRow, std::vector<MX_SEL_RANGE> &_vSelections, uint64_t _ui64GroupSize, uint64_t _ui64GroupIdx ) {
				_vSelections.clear();
				if ( !HasSelection() ) { return true; }
				try {
					if ( !_ui64GroupSize ) { return false; }
					if ( smMode == MX_SM_NORMAL ) {
						if ( _ui64GroupIdx != 0 ) { return true; }
						// Known not to be empty.
						// There is only one selection range.
						MX_SEL_RANGE srRange = { .ui64Start = sn.ui64Start, .ui64Size = sn.ui64End - sn.ui64Start };
						_vSelections.push_back( srRange );
						return true;
					}

					// Column Mode.
					// Known not to be empty.
					uint64_t ui64Total = sc.ui64Lines + 1;
					uint64_t ui64TotalGroups = (ui64Total % _ui64GroupSize == 0) ? (ui64Total / _ui64GroupSize) : (ui64Total / _ui64GroupSize + 1);
					if ( _ui64GroupIdx >= ui64TotalGroups ) { return true; }
					uint64_t ui64NewTotal = std::min( _ui64GroupSize, ui64Total - _ui64GroupSize * _ui64GroupIdx );
					uint64_t ui64Offset = _ui64GroupSize * _ui64GroupIdx;
					for ( size_t I = 0; I <= ui64NewTotal; ++I ) {
						MX_SEL_RANGE srRange = { .ui64Start = sc.ui64AnchorAddr + (I + ui64Offset) * _ui32BytesPerRow, .ui64Size = sc.ui32Cols };
						_vSelections.push_back( srRange );
					}
					return true;
				}
				catch ( ... ) { return false; }
			 }

			/**
			 * Gets the lower-left address of the selection.  Lower refers to the address, which means visually this is the upper-left address.
			 * 
			 * \return Returns the lower-left selection address or 0.
			 **/
			inline uint64_t							LowerLeftAddress() const {
				if ( !HasSelection() ) { return 0; }
				if ( smMode == MX_SM_NORMAL ) { return sn.ui64Start; }
				return sc.ui64AnchorAddr;
			}

			/**
			 * Gets the upper-right address of the selection.  Upper refers to the address, which means visually this is the lower-left address.
			 * 
			 * \param _ui32BytesPerRow The number of bytes in a row.
			 * \return Returns the upper-right selection address or 0.
			 **/
			inline uint64_t							UpperRightAddress( uint32_t _ui32BytesPerRow ) const {
				if ( !HasSelection() ) { return 0; }
				if ( smMode == MX_SM_NORMAL ) { return sn.ui64End; }
				return sc.ui64AnchorAddr + (sc.ui64Lines * _ui32BytesPerRow) + sc.ui32Cols;
			}
		};

		/**
		 * \brief Drag/gesture state captured on mouse down.
		 *
		 * Stores only what is required to later start/drive a selection:
		 * - The clicked address (anchor) and current address (caret), both absolute.
		 * - Whether the click landed in the right (text) area.
		 * - The half-cell horizontal threshold used to decide when a drag begins.
		 * - The per-gesture CurrentMode decided at mouse-down.
		 * - Flags for “pending threshold” and “actively selecting”.
		 */
		struct MX_SELECT_GESTURE {
			MX_SELECTION_MODE						smCurrent			= MX_SM_NORMAL;						/**< CurrentMode for this gesture. */

			uint64_t								ui64AnchorAddr		= 0;								/**< Selection "from" address. */
			uint64_t								ui64MouseAnchorAddr	= 0;								/**< Original anchor address when beginning a mouse drag. */
			uint64_t								ui64CaretAddr		= 0;								/**< Selection "to" address. */
			int32_t									i32CaretIdx			= -1;								/**< The index into the text where the caret is (IE if a value at an address is E7, this could be 0 to put the caret under the E, or 1 to put it under the 7). */

			POINT									ptDown				= { 0, 0 };							/**< Client-space mouse-down point. */
			//int32_t									i32HalfCellW		= 0;								/**< Half of the clicked cell’s width in pixels. */

			bool									bSelecting			= false;							/**< True after threshold is exceeded. */
			bool									bPendingThreshold	= false;							/**< True between mouse-down and threshold. */
			bool									bRightArea			= false;							/**< True if click hit the right (text) area. */
		};

		/** The selection Undo/Redo stack item. **/
		struct MX_SELECT_UNDO_REDO_ITEM {
			MX_SELECTION							sUndoSelection;											/**< The selection to restore during an Undo. */
			MX_SELECTION							sRedoSelection;											/**< The selection to restore during a Redo. */

			uint64_t								ui64UndoAnchorAddr	= 0;								/**< Selection "from" address. */
			uint64_t								ui64UndoMouseAnchorAddr	= 0;							/**< Original anchor address when beginning a mouse drag. */
			uint64_t								ui64UndoCaretAddr	= 0;								/**< Selection "to" address. */
			int32_t									i32UndoCaretIdx		= -1;								/**< The index into the text where the caret is (IE if a value at an address is E7, this could be 0 to put the caret under the E, or 1 to put it under the 7). */

			uint64_t								ui64RedoAnchorAddr	= 0;								/**< Selection "from" address. */
			uint64_t								ui64RedoMouseAnchorAddr	= 0;							/**< Original anchor address when beginning a mouse drag. */
			uint64_t								ui64RedoCaretAddr	= 0;								/**< Selection "to" address. */
			int32_t									i32RedoCaretIdx		= -1;								/**< The index into the text where the caret is (IE if a value at an address is E7, this could be 0 to put the caret under the E, or 1 to put it under the 7). */
		};


		// == Members.
		CWidget *									m_pwHexParent = nullptr;								/**< The main hex-editor window. */
		MX_STYLE *									m_psOptions = nullptr;									/**< Pointer to the shared options. */
		MX_FONT_SET *								m_pfsFonts[2] = {};										/**< Shared fonts. */
		
		MX_HEX_EDITOR_COLORS *						m_phecForeColors = nullptr;								/**< Pointer to shared foreground colors. */
		MX_HEX_EDITOR_COLORS *						m_phecBackColors = nullptr;								/**< Pointer to shared background colors. */
		MX_EDIT_AS									m_eaEditAs = MX_EA_HEX;									/**< The view type. */
		MX_SCROLL_DATA								m_sdScrollView[MX_EA_TOTAL];							/**< The scroll/view data for each view type. */
		CMiniMap::MX_MINI_MAP_STATE					m_mmsMiniMap;											/**< The mini-map. */

		std::map<uint64_t, COLORREF>				m_mColorLookup;											/**< Quick mixing of colors. */

		CHexEditorInterface::CBuffer				m_bCurBuffer;											/**< The current data buffer being displayed. */

		CHexEditorInterface *						m_pheiTarget = nullptr;									/**< The stream of data to handle. */
		MX_SELECT_GESTURE							m_sgSelGesture {};										/**< Selection action. */
		MX_SELECTION								m_sSel {};												/**< Actual selection. */
		std::vector<MX_SELECT_UNDO_REDO_ITEM>		m_vSelectionStack;										/**< The Undo/Redo selection stack. */
		size_t										m_sSelStackIdx = size_t( -1 );							/**< The selection-stack current index. */
		CUtilities::MX_TIMER						m_tCaretBlink;											/**< The blinking of the caret. */
		bool										m_bCaretOn = true;										/**< Caret on or off during active blink. */

		// The main window class.
		static ATOM									m_aAtom;


		// == Functions.
		// Gets the current style settings.
		MX_STYLE *									CurStyle() { return &m_psOptions[m_eaEditAs]; }

		// Gets the current style settings.
		const MX_STYLE *							CurStyle() const { return &m_psOptions[m_eaEditAs]; }

		/**
		 * Tells the parent to update the other hex controls due to the changing of some shared values.  Typically scroll must be updated.
		 **/
		void										UpdateSiblings() const;

		// Draws the hex-editor view.
		bool										PaintHex( HDC _hDc, const lsw::LSW_RECT &_rRect );

		/**
		 * \brief Draws the mini-map.
		 * 
		 * \param _hDc The device context to which to draw.
		 * \param _iXLeft The left X coordinate of the mini-map.
		 * \param _iYTop The top Y coordinate of the mini-map.
		 * \param _rRect The destination rectangle for the mini-map.
		 */
		void										DrawMiniMap( HDC _hDc, int32_t _iXLeft, int32_t _iYTop, const lsw::LSW_RECT &_rRect );

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
		 * \param _bRightArea The right or left area.
		 * \param _bData The actual values at the addresses to render.
		 **/
		void										DrawArea( HDC _hDc, int32_t _iXLeft, int32_t _iYTop, MX_DATA_FMT _dfFmt, uint32_t _ui32LinesToDraw, const CHexEditorInterface::CBuffer &_bData, bool _bRightArea );

		/**
		 * \brief Starts the caret blink timer.
		 *
		 * If the caret blink timer is not already running, this function creates a
		 * Win32 timer using the system caret blink time (or a reasonable default
		 * if the system value is disabled). The caret is set to the "on" phase and
		 * its rectangle is invalidated when the timer begins.
		 */
		inline void									StartCaretBlink() {
			UINT uBlink = ::GetCaretBlinkTime();
			if ( uBlink == 0 || uBlink == INFINITE ) {
				uBlink = 500;
			}
			m_bCaretOn = true;
			m_tCaretBlink.Start( Wnd(), MX_TI_HEX_CARET, uBlink, nullptr );
		}

		/**
		 * \brief Stops the caret blink timer and hides the caret.
		 *
		 * If a caret timer is active, this function cancels the timer and forces
		 * the caret to the "off" phase. The caret rectangle is invalidated so that
		 * the caret is erased from the display.
		 */
		void										StopCaretBlink() {
			m_tCaretBlink.Stop();
			m_bCaretOn = false;
				
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}

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
		 * Gets the text color of a hex/octal/binary cell by address.
		 * 
		 * \param _ui64Address The address of the data in the cell.
		 * \param _pui8Value The address of the data in the cell.
		 * \param _sSize The address of the data in the cell.
		 * \param _bRightArea The right area or the left area.
		 * \return Returns the background color for the given cell.
		 **/
		COLORREF									CellFgColor( uint64_t _ui64Address, const uint8_t * _pui8Value, size_t _sSize, bool _bRightArea );

		/**
		 * Gets the color of a hex/octal/binary cell by address.
		 * 
		 * \param _ui64Address The address of the data in the cell.
		 * \param _pui8Value The address of the data in the cell.
		 * \param _sSize The address of the data in the cell.
		 * \param _bRightArea The right area or the left area.
		 * \return Returns the background color for the given cell.
		 **/
		COLORREF									CellBgColor( uint64_t _ui64Address, const uint8_t * _pui8Value, size_t _sSize, bool _bRightArea );

		/**
		 * Gets the color of a Mini-Map pixel.
		 * 
		 * \param _ui64Address The address of the data in the cell.
		 * \param _pui8Value The address of the data in the cell.
		 * \param _sSize The address of the data in the cell.
		 * \return Returns the color for the given Mini-Map pixel.
		 **/
		COLORREF									MiniMapColor( uint64_t _ui64Address, const uint8_t * _pui8Value, size_t _sSize );

		/**
		 * Gets the rectangle for a cell's text.
		 * 
		 * \param _ui64Address The address whose cell is to be gotten.
		 * \param _bRightArea If true, the cell is in the right area, otherwise it is in the left area.
		 * \return Returns the client rectable for the given text cell.
		 **/
		lsw::LSW_RECT								GetCellTextRect( uint64_t _ui64Address, bool _bRightArea );

		/**
		 * Computes the start and end addresses for the Mini-Map.
		 * 
		 * \param _i32ScreenHeight The height of the display area.
		 * \param _ui64StartAddr Holds the returned start address.
		 * \param _ui64EndAddr Holds the returned end address.
		 * \param _ui32BytesPerRow The number of bytes per row.
		 **/
		void										MiniMapAddressRange( int32_t _i32ScreenHeight, uint64_t &_ui64StartAddr, uint64_t &_ui64EndAddr, uint32_t _ui32BytesPerRow );

		/**
		 * \brief Determines if the cursor is on the mini-map drag line.
		 *
		 * \param _pCursorPos The cursor position in client coordinates.
		 * \param _i32DragX Holds the returned drag-line X coordinate if the hit-test succeeds.
		 * \return Returns true if the cursor is on the drag line.
		 */
		bool										MiniMapHitDragLine( const POINTS &_pCursorPos, int32_t &_i32DragX ) const;

		/**
		 * \brief Snaps and clamps a proposed mini-map width.
		 *
		 * \param _i32Width The proposed width.
		 * \return Returns the snapped and clamped width.
		 */
		int32_t										MiniMapClampWidth( int32_t _i32Width ) const;

		/**
		 * \brief Gets the mini-map rectangle in client coordinates.
		 *
		 * \param _rOut Holds the returned rectangle.
		 * \return Returns true if the mini-map is visible.
		 */
		inline bool									MiniMapRect( lsw::LSW_RECT &_rOut ) const {
			if ( !CurStyle()->bShowMiniMap ) { return false; }
			const lsw::LSW_RECT rClient = ClientRect();
			const int32_t i32Left = MiniMapLeft( rClient );
			_rOut = lsw::LSW_RECT( i32Left, 0, i32Left + CurStyle()->i32MiniMapWidthPx, rClient.Height() );
			return true;
		}

		/**
		 * \brief Converts a cursor position to a mini-map source line and a byte offset within that line.
		 *
		 * \param _pCursorPos Cursor position in client coordinates.
		 * \param _ui64MiniLine Holds the returned mini-map source line.
		 * \param _ui32ByteOff Holds the returned byte offset within the mini-map row.
		 * \return Returns true if the cursor is inside the mini-map rectangle.
		 */
		bool										MiniMapCursorToLineByte( const POINTS &_pCursorPos, uint64_t &_ui64MiniLine, uint32_t &_ui32ByteOff ) const;

		/**
		 * \brief Converts a cursor position inside the mini-map to an absolute mini-map line index.
		 *
		 * \param _pCursorPos Cursor position in client coordinates.
		 * \param _rClient The client rectangle.
		 * \param _ui32BytesPerRow Mini-map bytes-per-row.
		 * \param _ui64MiniLine Holds the returned absolute mini-line.
		 * \return Returns true if the cursor is inside the mini-map rectangle.
		 */
		bool										MiniMapCursorToMiniLine( const POINTS &_pCursorPos, const lsw::LSW_RECT &_rClient, uint32_t _ui32BytesPerRow, uint64_t &_ui64MiniLine ) const {
			if ( !CurStyle()->bShowMiniMap || !_ui32BytesPerRow ) { return false; }

			const CMiniMap::MX_MINI_MAP & mmMiniMap = CurStyle()->mmMiniMap;

			const int32_t i32Left = MiniMapLeft( _rClient );
			const int32_t i32Right = i32Left + CurStyle()->i32MiniMapWidthPx;
			const int32_t i32Top = 0;
			const int32_t i32Bottom = _rClient.Height();

			if ( _pCursorPos.x < i32Left || _pCursorPos.x >= i32Right ) { return false; }
			if ( _pCursorPos.y < i32Top || _pCursorPos.y >= i32Bottom ) { return false; }

			const uint32_t ui32Zoom = mmMiniMap.ui32Zoom ? mmMiniMap.ui32Zoom : 1U;

			const uint64_t ui64SpanMiniLines = static_cast<uint64_t>( i32Bottom - i32Top ) / static_cast<uint64_t>( ui32Zoom ) + 1ULL;
			if ( ui64SpanMiniLines == 0 ) { return false; }

			const uint64_t ui64DispFirstLine = m_mmsMiniMap.ui64Address / static_cast<uint64_t>( _ui32BytesPerRow );

			// Line within the displayed span.
			const uint64_t ui64RelY = static_cast<uint64_t>( _pCursorPos.y - i32Top );
			const uint64_t ui64H = static_cast<uint64_t>( i32Bottom - i32Top );

			uint64_t ui64LineInSpan = (ui64RelY * ui64SpanMiniLines) / ui64H;
			if ( ui64LineInSpan >= ui64SpanMiniLines ) { ui64LineInSpan = ui64SpanMiniLines - 1ULL; }

			_ui64MiniLine = ui64DispFirstLine + ui64LineInSpan;
			return true;
		}

		/**
		 * \brief Converts a mini-map source line to the corresponding address in the file.
		 *
		 * \param _ui64MiniLine Source line in the mini-map bitmap.
		 * \return Returns the file address clamped to Size().
		 */
		uint64_t									MiniMapLineToAddress( uint64_t _ui64MiniLine ) const;

		/**
		 * \brief Gets mini-map rectangle and thumb metrics for scroll dragging.
		 *
		 * \param _rClient The client rectangle.
		 * \param _i32MiniLeft Holds the mini-map left.
		 * \param _i32MiniRight Holds the mini-map right (exclusive).
		 * \param _i32MiniTop Holds the mini-map top.
		 * \param _i32MiniBottom Holds the mini-map bottom (exclusive).
		 * \param _i32ThumbTop Holds the thumb top.
		 * \param _i32ThumbHeight Holds the thumb height.
		 * \param _i32Track Holds the track height (miniHeight - thumbHeight).
		 * \param _ui64MaxFirstLine Holds the maximum ui64FirstVisibleLine.
		 * \return Returns true if mini-map is visible and metrics are valid.
		 */
		bool										MiniMapThumbMetrics( const lsw::LSW_RECT &_rClient,
			int32_t &_i32MiniLeft, int32_t &_i32MiniRight, int32_t &_i32MiniTop, int32_t &_i32MiniBottom,
			int32_t &_i32ThumbTop, int32_t &_i32ThumbHeight, int32_t &_i32Track, uint64_t &_ui64MaxFirstLine ) const;

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
			return CurStyle()->bShowRulerLabels ? std::max( Font()->iCharCy, 4 ) : 4;
		}

		/**
		 * \brief Computes the left X and width (in pixels) of a text cell at index _ui32Index for an area.
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
		 * Gets the Mini-Map left.
		 * 
		 * \param _rRect The client rectangle.
		 * \return Returns the left position of the Mini-Map.
		 **/
		int32_t										MiniMapLeft( const lsw::LSW_RECT &_rRect ) const {
			const MX_STYLE & stAll = (*CurStyle());
			return stAll.mmMiniMap.bRightSize ?
				_rRect.Width() - (stAll.i32MiniMapWidthPx) :
				(stAll.i32MiniMapWidthPx);
		}

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
			if ( agGlyphs.i32DigitMaxCx > 0 && Font()->iCharCy > 0 ) { return; }

			TEXTMETRICW tmMetrics {};
			::GetTextMetricsW( _hDc, &tmMetrics );
			Font()->iCharCy = tmMetrics.tmHeight + tmMetrics.tmExternalLeading;

			agGlyphs.i320_9DigitMaxCx = lsw::CHelpers::MeasureMax( _hDc, L"0123456789" );
			agGlyphs.i32DigitMaxCx = std::max( agGlyphs.i320_9DigitMaxCx, lsw::CHelpers::MeasureMax( _hDc, L"ABCDEF" ) );
			agGlyphs.i32DigitMaxCxLower = std::max( agGlyphs.i32DigitMaxCx, lsw::CHelpers::MeasureMax( _hDc, L"abcdef" ) );

			SIZE sSize {};
			::GetTextExtentPoint32W( _hDc, L":", 1, &sSize ); agGlyphs.i32ColonCx				= sSize.cx;
			::GetTextExtentPoint32W( _hDc, L" ", 1, &sSize ); agGlyphs.i32SpaceCx				= (sSize.cx + 1) / 2 * 2;	// Round up to the nearest even number.
			::GetTextExtentPoint32W( _hDc, L"h", 1, &sSize ); agGlyphs.i32SpecHexCx				= sSize.cx;
			::GetTextExtentPoint32W( _hDc, L"o", 1, &sSize ); agGlyphs.i32SpecOctCx				= sSize.cx;
			::GetTextExtentPoint32W( _hDc, L"w", 1, &sSize ); agGlyphs.i32ShortWcx				= sSize.cx;
			::GetTextExtentPoint32W( _hDc, L"+", 1, &sSize ); agGlyphs.i32PlusMinusSpaceMaxCx	= std::max<int32_t>( agGlyphs.i32SpaceCx, sSize.cx );
			::GetTextExtentPoint32W( _hDc, L"-", 1, &sSize ); agGlyphs.i32MinusCx				= sSize.cx;
			::GetTextExtentPoint32W( _hDc, L".", 1, &sSize ); agGlyphs.i32PeriodCx				= sSize.cx;
			::GetTextExtentPoint32W( _hDc, L"e", 1, &sSize ); agGlyphs.i32Ecx					= sSize.cx;
			agGlyphs.i32PlusMinusSpaceMaxCx	= std::max<int32_t>( agGlyphs.i32PlusMinusSpaceMaxCx, agGlyphs.i32MinusCx );
			

			if ( CharSet().bSinglePage ) {
				agGlyphs.i32MaxAscii = 0;
				agGlyphs.i32MaxAsciiAnsi = 0;
				for ( wchar_t I = 0; I < 256; ++I ) {
					wchar_t wcChar = I;
					CharSet().pfDisplayable( reinterpret_cast<uint8_t *>(&wcChar), 2, wcChar );
					::GetTextExtentPoint32W( _hDc, &wcChar, 1, &sSize );
					agGlyphs.i32MaxAsciiAnsi = std::max<int32_t>( agGlyphs.i32MaxAsciiAnsi, sSize.cx );
					if ( wcChar <= 0x7F ) {
						agGlyphs.i32MaxAscii = std::max<int32_t>( agGlyphs.i32MaxAscii, sSize.cx );
					}
				}
			}
			else {
				agGlyphs.i32MaxAscii = 0;
				agGlyphs.i32MaxAsciiAnsi = tmMetrics.tmMaxCharWidth;
				if ( CurStyle()->csCharSet == CCharSets::MX_CS_UNICODE ) {
					//agGlyphs.i32MaxAsciiAnsi = ((agGlyphs.i32MaxAsciiAnsi + 1) / 2) + (agGlyphs.i32MaxAsciiAnsi / 8);
					agGlyphs.i32MaxAsciiAnsi = ((agGlyphs.i32MaxAsciiAnsi + 1) / 2 * 2);// + (agGlyphs.i32MaxAsciiAnsi / 12);
				}
				for ( wchar_t I = 0; I < 0x80; ++I ) {
					wchar_t wcChar = I;
					CharSet().pfDisplayable( reinterpret_cast<uint8_t *>(&wcChar), 2, wcChar );
					::GetTextExtentPoint32W( _hDc, &wcChar, 1, &sSize );
					agGlyphs.i32MaxAscii = std::max<int32_t>( agGlyphs.i32MaxAscii, sSize.cx );
				}
			}
		}

		/**
		 * Begin a selection gesture (mouse down). Applies Column/Normal rules and Shift/Ctrl modifiers.
		 *
		 * \param _ptClient Client-space mouse location.
		 * \param _bShift True if Shift is pressed.
		 * \param _bCtrl True if Ctrl is pressed.
		 * \param _bCtrlShift True if Ctrl+Shift are pressed.
		 */
		void										SelectionBeginGesture( const POINT &_ptClient, bool _bShift, bool _bCtrl, bool _bCtrlShift );

		/**
		 * Update selection during mouse move. Starts a new selection after threshold when appropriate.
		 *
		 * \param _ptClient Client-space mouse location.
		 * \param _bShift True if Shift is pressed.
		 * \param _bCtrl True if Ctrl is pressed.
		 * \param _bCtrlShift True if Ctrl+Shift are pressed.
		 */
		void										SelectionUpdateGesture( const POINT &_ptClient, bool _bShift, bool _bCtrl, bool _bCtrlShift );

		/**
		 * End a selection gesture (mouse up). Collapses zero-length selections created by clicks without drag.
		 */
		void										SelectionEndGesture();

		/**
		 * Adds the current selection to the Undo/Redo stack.
		 * 
		 * \param _sOldSelection The old selection being set.  Automatically saves the current selection.
		 * \param _sgOldGesture The previous caret and anchors.
		 * \return Returns true if the current selection was added to the selection Undo/Redo stack.
		 **/
		bool										AddSelectionUndo( const MX_SELECTION &_sOldSelection, const MX_SELECT_GESTURE &_sgOldGesture );

		/**
		 * \brief Initialize a Shift-extend gesture on an existing Normal selection.
		 *
		 * The endpoint (start or end) whose address is closest to the clicked address is moved to the clicked
		 * address. The other endpoint remains fixed. We implement this by setting the gesture anchor to the
		 * fixed endpoint and the caret to the clicked address, then calling SelectionUpdateGesture().
		 *
		 * \param _ui64ClickAddr Address that was Shift-clicked.
		 * \param _ptClient Client-space mouse location.
		 * \param _bShift True if Shift is pressed.
		 * \param _bCtrl True if Ctrl is pressed.
		 * \param _bCtrlShift True if Ctrl+Shift are pressed.
		 */
		void										InitShiftExtendNormal( uint64_t _ui64ClickAddr,
			const POINT &_ptClient,
			bool _bShift,
			bool _bCtrl,
			bool _bCtrlShift ) {

			// Normalized local copies.
			uint64_t ui64Start = m_sSel.sn.ui64Start;
			uint64_t ui64End   = m_sSel.sn.ui64End;
			if ( ui64Start > ui64End ) {
				std::swap( ui64Start, ui64End );
			}

			const uint64_t ui64DistStart = (ui64Start > _ui64ClickAddr) ? (ui64Start - _ui64ClickAddr) : (_ui64ClickAddr - ui64Start);
			const uint64_t ui64DistEnd   = (ui64End   > _ui64ClickAddr) ? (ui64End   - _ui64ClickAddr) : (_ui64ClickAddr - ui64End);

			// If click is closer to Start, move Start; anchor is End.
			// If click is closer to End, move End; anchor is Start.
			const uint64_t ui64AnchorAddr = (ui64DistStart <= ui64DistEnd) ? ui64End : ui64Start;

			m_sgSelGesture.ptDown = _ptClient;
			m_sgSelGesture.bPendingThreshold = false;			// We are already selecting.
			m_sgSelGesture.bSelecting = true;
			m_sgSelGesture.smCurrent = MX_SM_NORMAL;			// Shift-extend is always Normal here.

			m_sgSelGesture.ui64AnchorAddr = ui64AnchorAddr;		// Fixed endpoint.
			m_sgSelGesture.ui64CaretAddr  = _ui64ClickAddr;		// Moving endpoint at click.

			// Let the normal update logic build m_sSel from anchor/caret.
			SelectionUpdateGesture( _ptClient, _bShift, _bCtrl, _bCtrlShift );
		}

		/**
		 * \brief Initialize a Shift-extend gesture on an existing Column selection.
		 *
		 * The corner (top-left or bottom-right) whose grid position is closest to the clicked
		 * address is moved to the clicked address. The opposite corner remains fixed.
		 * We implement this by:
		 *  - Choosing the fixed corner's address as the gesture anchor.
		 *  - Using the clicked address as the gesture caret.
		 *  - Forcing Column mode and immediately calling SelectionUpdateGesture().
		 *
		 * \param _ui64ClickAddr Address that was Shift-clicked.
		 * \param _ptClient Client-space mouse location.
		 * \param _bShift True if Shift is pressed.
		 * \param _bCtrl True if Ctrl is pressed.
		 * \param _bCtrlShift True if Ctrl+Shift are pressed.
		 */
		void										InitShiftExtendColumn( uint64_t _ui64ClickAddr,
			const POINT &_ptClient,
			bool _bShift,
			bool _bCtrl,
			bool _bCtrlShift );

		/**
		 * \brief Convert the current single-row Normal selection into a Column selection.
		 *
		 * Precondition: NormalSelectionIsSingleRow() is true.
		 */
		void										ConvertSingleRowNormalToColumn();

		/**
		 * \brief Returns true if the active Normal selection lies entirely on one logical row.
		 */
		bool										NormalSelectionIsSingleRow() const;

		/**
		 * \brief Finds a word boundary relative to a starting address, scanning toward a given endpoint.
		 *
		 * A word boundary is defined as the first address whose word/non-word classification differs
		 * from the classification of the byte at _ui64StartAddr (using MX_IsWordByte()).
		 *
		 * For forward scans:
		 *  - Scans ( _ui64StartAddr + 1 ) up to and including _ui64EndAddr.
		 *  - _ui64EndAddr must be >= _ui64StartAddr (after clamping).
		 *
		 * For backward scans:
		 *  - Scans ( _ui64StartAddr - 1 ) down to and including _ui64EndAddr.
		 *  - _ui64EndAddr must be <= _ui64StartAddr (after clamping).
		 *
		 * The function reads data in chunks to minimize per-byte overhead.
		 *
		 * \param _ui64StartAddr Starting address whose classification is treated as the base.
		 * \param _bForward True to scan forward (toward increasing addresses), false to scan backward.
		 * \param _ui64EndAddr Inclusive endpoint in the scan direction. It will be clamped to the file range.
		 * \param _ui64FoundAddr [out] Receives the address of the first boundary if one is found.
		 * \return Returns true if a boundary is found; false if none is found in the given range or on read failure.
		 */
		bool										FindWordBoundary( uint64_t _ui64StartAddr,
			bool _bForward,
			uint64_t _ui64EndAddr,
			uint64_t &_ui64FoundAddr ) const;

		/**
		 * Clears the current selection.
		 **/
		void										ClearSelection() {
			m_sgSelGesture.bSelecting = false;
			m_sSel.bHas = false;
		}

		/**
		 * Gets the pixel width of a cell given its format.
		 * 
		 * \param _dfFmt The format of the cell.
		 * \return Returns the pixel width for the text in the cell.
		 **/
		inline int32_t								CellWidthForFmt( MX_DATA_FMT _dfFmt ) const {
			const MX_STYLE & stStyle = (*CurStyle());
			const MX_FONT_SET & fsFont = (*Font());
			const MX_ADDR_GLYPHS & agGlyphs = fsFont.agGlyphs;
			switch( _dfFmt ) {
				case MX_DF_HEX :		{ return 2 * agGlyphs.i32DigitMaxCx; }																																				/**< "FF" */
				case MX_DF_DEC :		{ return 3 * agGlyphs.i320_9DigitMaxCx; }																																			/**< "255" */
				case MX_DF_OCT :		{ return 3 * agGlyphs.i320_9DigitMaxCx; }																																			/**< "377" */
				case MX_DF_BIN :		{ return 8 * agGlyphs.i320_9DigitMaxCx; }																																			/**< "11111111" */
				case MX_DF_CHAR :		{ return CCharSets::m_csSets[stStyle.csCharSet].bHideOver127 ? agGlyphs.i32MaxAscii : agGlyphs.i32MaxAsciiAnsi; }																	/**< Printable cell. */
				case MX_DF_UINT8 :		{ return 3 * agGlyphs.i320_9DigitMaxCx; }																																			/**< "255" */
				case MX_DF_INT8 :		{ return 3 * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx; }																														/**< "-128" */
				case MX_DF_UINT16 :		{ return (5 * agGlyphs.i320_9DigitMaxCx) / 2; }																																		/**< "65535" */
				case MX_DF_INT16 :		{ return (5 * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx) / 2; }																												/**< "-32768" */
				case MX_DF_UINT32 :		{ return (10 * agGlyphs.i320_9DigitMaxCx) / 4; }																																	/**< "4294967295" */
				case MX_DF_INT32 :		{ return (10 * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx) / 4; }																												/**< "-2147483648" */
				case MX_DF_UINT64 :		{ return (20 * agGlyphs.i320_9DigitMaxCx) / 8; }																																	/**< "18446744073709551615" */
				case MX_DF_INT64 :		{ return (19 * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx) / 8; }																												/**< "-9223372036854775808" */
				case MX_DF_POINTER16 :	{ return (4 * agGlyphs.i320_9DigitMaxCx) / 2; }																																		/**< "FFFF" */
				case MX_DF_POINTER32 :	{ return (8 * agGlyphs.i320_9DigitMaxCx) / 4; }																																		/**< "FFFFFFFF" */
				case MX_DF_POINTER64 :	{ return (16 * agGlyphs.i320_9DigitMaxCx) / 8; }																																	/**< "FFFFFFFFFFFFFFFF" */
				case MX_DF_FLOAT10 :	{ return ((stStyle.ui32FloatDigits + 2) * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx + agGlyphs.i32PlusMinusSpaceMaxCx + agGlyphs.i32PeriodCx + agGlyphs.i32Ecx) / 2; }		/**< "0 3.125 -42" */
				case MX_DF_FLOAT11 :	{ return ((stStyle.ui32FloatDigits + 2) * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx + agGlyphs.i32PlusMinusSpaceMaxCx + agGlyphs.i32PeriodCx + agGlyphs.i32Ecx) / 2; }		/**< "0 3.125 -42" */
				case MX_DF_BFLOAT16 :	{ return ((stStyle.ui32FloatDigits + 2) * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx + agGlyphs.i32PlusMinusSpaceMaxCx + agGlyphs.i32PeriodCx + agGlyphs.i32Ecx) / 2; }		/**< "0 3.141 -42" */
				case MX_DF_FLOAT16 :	{ return ((stStyle.ui32FloatDigits + 2) * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx + agGlyphs.i32PlusMinusSpaceMaxCx + agGlyphs.i32PeriodCx + agGlyphs.i32Ecx) / 2; }		/**< "0 3.1406 -42" */
				case MX_DF_FLOAT32 :	{ return ((stStyle.ui32FloatDigits + 3) * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx + agGlyphs.i32PlusMinusSpaceMaxCx + agGlyphs.i32PeriodCx + agGlyphs.i32Ecx) / 4; }		/**< "0 3.1415925 -42" */
				case MX_DF_FLOAT64 :	{ return ((stStyle.ui32FloatDigits + 4) * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx + agGlyphs.i32PlusMinusSpaceMaxCx + agGlyphs.i32PeriodCx + agGlyphs.i32Ecx) / 8; }		/**< "0 3.1415926535897931 -42" */

				case MX_DF_PCM8 :		{ return ((stStyle.ui32FloatDigits + 1) * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx + agGlyphs.i32PeriodCx) / 1; }															/**< "0 -1 0.1647" */
				case MX_DF_PCM16 :		{ return ((stStyle.ui32FloatDigits + 1) * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx + agGlyphs.i32PeriodCx) / 2; }															/**< "0 -1 0.1647" */
				case MX_DF_PCM24 :		{ return ((stStyle.ui32FloatDigits + 1) * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx + agGlyphs.i32PeriodCx) / 3; }															/**< "0 -1 0.1647" */
				case MX_DF_PCM32 :		{ return ((stStyle.ui32FloatDigits + 1) * agGlyphs.i320_9DigitMaxCx + agGlyphs.i32MinusCx + agGlyphs.i32PeriodCx) / 4; }															/**< "0 -1 0.1647" */
				default :				{ return 2 * agGlyphs.i32DigitMaxCx; }
			}
		}

		/**
		 * Mixes two colors.  Alpha is taken from crRight.
		 * 
		 * \param crLeft The left color.
		 * \param crRight The right color.
		 * \return Returns the mixed color.
		 **/
		COLORREF									Mix( COLORREF crLeft, COLORREF crRight ) {
			uint64_t ui64Key = (static_cast<uint64_t>(*reinterpret_cast<uint32_t *>(&crLeft)) << 32ULL) | (*reinterpret_cast<uint32_t *>(&crRight));
			auto aKey = m_mColorLookup.find( ui64Key );
			if ( aKey == m_mColorLookup.end() ) {
				COLORREF cdRes = lsw::CHelpers::MixColorRef( GetRValue( crLeft ), GetRValue( crRight ),
					GetGValue( crLeft ), GetGValue( crRight ),
					GetBValue( crLeft ), GetBValue( crRight ),
					MX_GetAValue( crRight ) / 255.0f );
				m_mColorLookup[ui64Key] = cdRes;
				return cdRes;
			}
			return aKey->second;
		}

		/**
		 * Gets a format’s stride (how many cells in a row it consumes).
		 * 
		 * \param _dfFmt The format whose sub-caret count is to be returned.
		 * \return Returns the sub-caret count.
		 **/
		inline uint32_t								StrideForFormat( MX_DATA_FMT _dfFmt ) const {
			switch ( _dfFmt ) {
				case MX_DF_HEX : {}								MX_FALLTHROUGH
				case MX_DF_DEC : {}								MX_FALLTHROUGH
				case MX_DF_OCT : {}								MX_FALLTHROUGH
				case MX_DF_BIN : { return 1; }
				case MX_DF_CHAR : { return (CurStyle()->csCharSet == CCharSets::MX_CS_UNICODE) ? 2 : 1; }

				case MX_DF_UINT8 : { return sizeof( uint8_t ); }
				case MX_DF_INT8 : { return sizeof( int8_t ); }
				case MX_DF_UINT16 : { return sizeof( uint16_t ); }
				case MX_DF_INT16 : { return sizeof( int16_t ); }
				case MX_DF_UINT32 : { return sizeof( uint32_t ); }
				case MX_DF_INT32 : { return sizeof( int32_t ); }
				case MX_DF_UINT64 : { return sizeof( uint64_t ); }
				case MX_DF_INT64 : { return sizeof( int64_t ); }

				case MX_DF_POINTER16 : { return sizeof( uint16_t ); }
				case MX_DF_POINTER32 : { return sizeof( uint32_t ); }
				case MX_DF_POINTER64 : { return sizeof( uint64_t ); }

				case MX_DF_FLOAT11 : { return sizeof( uint16_t ); }
				case MX_DF_FLOAT10 : { return sizeof( uint16_t ); }
				case MX_DF_BFLOAT16 : { return sizeof( uint16_t ); }
				case MX_DF_FLOAT16 : { return sizeof( uint16_t ); }
				case MX_DF_FLOAT32 : { return sizeof( float ); }
				case MX_DF_FLOAT64 : { return sizeof( double ); }

				case MX_DF_PCM8 : { return sizeof( uint8_t ); }
				case MX_DF_PCM16 : { return sizeof( uint16_t ); }
				case MX_DF_PCM24 : { return sizeof( uint8_t ) * 3; }
				case MX_DF_PCM32 : { return sizeof( uint32_t ); }
				default : { return 1; }
			}
		}

		/**
		 * \brief Computes the number of source mini-map lines displayed for a given height and zoom.
		 *
		 * \param _i32Height The destination height in pixels.
		 * \param _ui32Zoom The zoom (pixel size), clamped to at least 1.
		 * \return Returns the number of source lines (at least 1).
		 */
		static inline uint64_t						MiniMapSpanLines( int32_t _i32Height, uint32_t _ui32Zoom ) {
			const uint32_t ui32Zoom = _ui32Zoom ? _ui32Zoom : 1U;
			if ( _i32Height <= 0 ) { return 1ULL; }
			// CeilDiv(height, zoom).
			return (static_cast<uint64_t>(_i32Height) + static_cast<uint64_t>(ui32Zoom) - 1ULL) / static_cast<uint64_t>(ui32Zoom);
		}

		/**
		 * \brief Computes a rounded (A * B) / Div using 128-bit intermediate math.
		 *
		 * \param _ui64A The first value.
		 * \param _ui64B The second value.
		 * \param _ui64Div The divisor.
		 * \return Returns round((A * B) / Div). Returns 0 if _ui64Div is 0 or on failure.
		 */
		static inline uint64_t						MiniMapMulDivU64Round( uint64_t _ui64A, uint64_t _ui64B, uint64_t _ui64Div ) {
			if ( _ui64Div == 0 ) { return 0; }
			try {
				uint64_t ui64Rem;
				return _umuldiv128_rounded( _ui64A, _ui64B, _ui64Div, &ui64Rem );
			}
			catch ( ... ) { return 0; }
		}

		/**
		 * \brief Tests if a point is inside the mini-map rectangle.
		 */
		static inline bool							MiniMapPtInRect( int32_t _i32X, int32_t _i32Y, int32_t _i32L, int32_t _i32T, int32_t _i32R, int32_t _i32B ) {
			return (_i32X >= _i32L && _i32X < _i32R && _i32Y >= _i32T && _i32Y < _i32B);
		}

		/**
		 * \brief Computes the maximum top-of-view line index (the same clamp used by the main view).
		 *
		 * \param _ui64TotalBytes Total file size in bytes.
		 * \param _ui32MainBpr Main view bytes-per-row.
		 * \param _i32PageLines Number of visible lines.
		 * \return Returns the maximum value for ui64FirstVisibleLine.
		 */
		static inline uint64_t						MiniMapMaxFirstVisibleLine( uint64_t _ui64TotalBytes, uint32_t _ui32MainBpr, int32_t _i32PageLines ) {
			const uint64_t ui64PageBytesMain = static_cast<uint64_t>((_i32PageLines > 0) ? _i32PageLines : 1) * static_cast<uint64_t>(_ui32MainBpr ? _ui32MainBpr : 1U);
			if ( _ui64TotalBytes <= ui64PageBytesMain ) { return 0ULL; }
			return (_ui64TotalBytes - ui64PageBytesMain) / static_cast<uint64_t>(_ui32MainBpr ? _ui32MainBpr : 1U);
		}

		/**
		 * Gets the number of sub-carets in a cell given its format.
		 * 
		 * \param _dfFmt The format whose sub-caret count is to be returned.
		 * \return Returns the sub-caret count.
		 **/
		static inline uint32_t						SubCaretCountForFormat( MX_DATA_FMT _dfFmt ) {
			switch ( _dfFmt ) {
				case MX_DF_HEX : { return 2; }
				case MX_DF_DEC : { return 3; }
				case MX_DF_OCT : { return 3; }
				case MX_DF_BIN : { return 8; }
				case MX_DF_CHAR : { return 1; }
				case MX_DF_UINT8 : {}				MX_FALLTHROUGH							/**< "  0 255  42" */
				case MX_DF_INT8 : {}				MX_FALLTHROUGH							/**< "  0  -1  42" */
				case MX_DF_UINT16 : {}				MX_FALLTHROUGH							/**< "    0 65535    42" */
				case MX_DF_INT16 : {}				MX_FALLTHROUGH							/**< "    0    -1    42" */
				case MX_DF_UINT32 : {}				MX_FALLTHROUGH							/**< "         0 4294967295         42" */
				case MX_DF_INT32 : {}				MX_FALLTHROUGH							/**< "         0         -1         42" */
				case MX_DF_UINT64 : {}				MX_FALLTHROUGH							/**< "                   0 18446744073709551615                   42" */
				case MX_DF_INT64 : {}				MX_FALLTHROUGH							/**< "                   0                   -1                   42" */
				case MX_DF_POINTER16 : {}			MX_FALLTHROUGH							/**< "FFFF" */
				case MX_DF_POINTER32 : {}			MX_FALLTHROUGH							/**< "FFFFFFFF" */
				case MX_DF_POINTER64 : {}			MX_FALLTHROUGH							/**< "FFFFFFFFFFFFFFFF" */
				case MX_DF_FLOAT10 : {}				MX_FALLTHROUGH							/**< "0 3.125 -42" */
				case MX_DF_FLOAT11 : {}				MX_FALLTHROUGH							/**< "0 3.125 -42" */
				case MX_DF_BFLOAT16 : {}			MX_FALLTHROUGH							/**< "0 3.141 -42" */
				case MX_DF_FLOAT16 : {}				MX_FALLTHROUGH							/**< "0 3.1406 -42" */
				case MX_DF_FLOAT32 : {}				MX_FALLTHROUGH							/**< "0 3.1415925 -42" */
				case MX_DF_FLOAT64 : {}				MX_FALLTHROUGH							/**< "0 3.1415926535897931 -42" */
				case MX_DF_PCM8 : {}				MX_FALLTHROUGH							/**< "0 -1 0.1647" */
				case MX_DF_PCM16 : {}				MX_FALLTHROUGH							/**< "0 -1 0.1647" */
				case MX_DF_PCM24 : {}				MX_FALLTHROUGH							/**< "0 -1 0.1647" */
				case MX_DF_PCM32 : {}				MX_FALLTHROUGH							/**< "0 -1 0.1647" */
				default : { return 1; }
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

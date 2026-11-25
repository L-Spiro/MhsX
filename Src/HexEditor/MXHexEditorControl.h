#pragma once

#include "../MXMhsX.h"
#include "../Layouts/MXUiIds.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include "MXCharSets.h"
#include "MXCopyAs.h"
#include "MXDataFmt.h"
#include "MXHexEditorColors.h"
#include "MXHexEditorControlHost.h"
#include "MXHexEditorInterface.h"
#include "MXMiniMap.h"
#include "MXSelection.h"

#include <LSWWin.h>
#include <Helpers/LSWHelpers.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

#include <map>


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
			MX_AF_BYTES_HEX,						/**< Example: 1:1D9E:53C8 */
			MX_AF_BYTES_DEC,						/**< Example: 6562337568 (right-aligned, padded with spaces on the left) */
			MX_AF_BYTES_OCT,						/**< Example: 60411251440 */
			MX_AF_LINE_NUMBER,						/**< Example: 820292197 (right-aligned, padded with spaces on the left) */
			MX_AF_SHORT_HEX,						/**< Example: C392:A990w */
			MX_AF_SHORT_DEC,						/**< Example: 2395961428w (right-aligned, padded with spaces on the left) */
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


		// == Functions.
		// Gets the text displayed in the tab.
		virtual CSecureWString						TabString() const { return m_pheiTarget ? m_pheiTarget->TabString() : CSecureWString(); }

		// Gets the text displayed in the status bar.
		virtual CSecureWString						StatusString() const { return m_pheiTarget ? m_pheiTarget->StatusString() : CSecureWString(); }

		// Sets the view type.
		void										SetViewType( MX_EDIT_AS _eaType );

		// Gets the editing type of the control.
		inline MX_EDIT_AS							GetViewType() const { return m_eaEditAs; }

		// Gets the view type as a string.
		CSecureWString								GetViewTypeAsString() const;

		// Sets the character set.
		void										SetCharacterSet( CCharSets::MX_CHAR_SETS _csSet );

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
		inline MX_ADDRESS_FMT						GetAddressGutterFmt() const { return CurStyle()->daAddressStyle.afFormat; }

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
				if ( m_pwHexParent ) { m_pwHexParent->UpdateStatusBar_PosValue_StartSize(); }
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
				if ( m_pwHexParent ) { m_pwHexParent->UpdateStatusBar_PosValue_StartSize(); }
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
				CancelEdit();
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
				CancelEdit();
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
				CancelEdit();
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
		 * Gets the starting address of the selection, if there is one, otherwise returns the caret position.
		 * 
		 * \return Returns the starting address of the selection, if there is one.  Otherwise returns the caret position.
		 **/
		uint64_t									GetSelctStartOrCaret() const;

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
		inline bool									HasSelection() const { return Size() > 0 && m_sSel.HasSelection(); }

		/**
		 * Gets the selected ranges, lowest address first, a group at a time (a Column selection has one range per row; see
		 *	MX_SELECTION::GatherSelected_LowToHigh()).  Ranges can run past the end of the data.
		 *
		 * \param _vRanges Receives the ranges of the group (empty after the last group).
		 * \param _ui64GroupSize The number of ranges per group.
		 * \param _ui64GroupIdx The group.
		 * \return Returns false on a memory failure.
		 **/
		inline bool									GatherSelection( std::vector<MX_SEL_RANGE> &_vRanges, uint64_t _ui64GroupSize, uint64_t _ui64GroupIdx ) {
			return m_sSel.GatherSelected_LowToHigh( CurStyle()->uiBytesPerRow, _vRanges, _ui64GroupSize, _ui64GroupIdx );
		}

		/**
		 * Gets the address of the start of the selection (the lowest selected address), or the caret if nothing is selected.
		 *
		 * \return Returns the address.
		 **/
		inline uint64_t								SelectionStart() const { return HasSelection() ? m_sSel.LowerLeftAddress() : m_sgSelGesture.ui64CaretAddr; }

		/**
		 * Gets the size of the selection.
		 * 
		 * \return Returns the number of bytes selected.
		 **/
		inline uint64_t								GetTotalSelectedBytes() const { return m_sSel.TotalSelected( CurStyle()->uiBytesPerRow ); }

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
		 * \return Returns false if the message should be treated as a warning.
		 **/
		bool										MarkSelectionStart( CSecureWString &_swsStatus );

		/**
		 * Sets the caret to the end of the selection.
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 * \return Returns false if the message should be treated as a warning.
		 **/
		bool										MarkSelectionEnd( CSecureWString &_swsStatus );

		/**
		 * Moves the selection back.
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 * \return Returns false if the message should be treated as a warning.
		 **/
		bool										MoveSelectionBack( CSecureWString &_swsStatus );

		/**
		 * Moves the selection forward.
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 * \return Returns false if the message should be treated as a warning.
		 **/
		bool										MoveSelectionForward( CSecureWString &_swsStatus );

		/**
		 * Performs a Cut operation.
		 * 
		 * \param _wsStatus The string to print in the status bar.
		 * \return Returns false if the message should be treated as a warning.
		 **/
		bool										Cut( CSecureWString &_swsStatus );

		/**
		 * Performs a Copy operation.
		 * 
		 * \param _caFormat The Copy format.
		 * \param _wsStatus The string to print in the status bar.
		 * \return Returns false if the message should be treated as a warning.
		 **/
		bool										CopyAs( MX_COPY_AS _caFormat, CSecureWString &_swsStatus );

		/**
		 * Gathers the parameters of a Copy operation from the current view: the format, the left/right areas, the character set, the
		 *	endianness, and the layout and address-gutter settings needed to reproduce the view later (deferred rendering may happen
		 *	after this control is gone).
		 * 
		 * \param _caFormat The Copy format.
		 * \return Returns the Copy parameters.
		 **/
		MX_COPYAS									CopyParms( MX_COPY_AS _caFormat ) const;

		/**
		 * Performs a Paste operation from the active clipboard.  Any selection is deleted first, then the clipboard data is inserted
		 *	(Insert mode) or written over the following bytes (Overwrite mode) at the start of the selection or the caret.  The caret
		 *	ends up after the pasted data.  The whole operation is one Undo step.
		 * 
		 * \param _swsStatus The string to print in the status bar.
		 * \return Returns false if the message should be treated as a warning.
		 **/
		bool										Paste( CSecureWString &_swsStatus );

		/**
		 * Determines whether a Paste operation can be performed now.
		 * 
		 * \return Returns true if the data source supports pasting, is writable, and the active clipboard has data.
		 **/
		bool										CanPaste() const;

		/**
		 * Are we in OVR mode?
		 * 
		 * \return Returns true if in Overwrite mode.
		 **/
		bool										IsOverwriteMode() const;

		/**
		 * Are we in Big-Endian mode?
		 * 
		 * \return Returns true if we are in Big Endian mode (impacts how Unicode characters are displayed).
		 **/
		bool										IsBigEndian() const { return m_bBigEnd; }

		/**
		 * Gets the character-set Status-Bar string.
		 * 
		 * \return Returns the Status Bar string for the given character set.
		 **/
		const CSecureString							CharSetStatusBarCode() const;

		/**
		 * Determines if deleting is possible.  To delete, the internal state must be valid and the file must not be read-only.
		 * 
		 * \return Returns true if the internal state is valid and the file is not read-only.
		 **/
		inline bool									CanDelete() const {
			return m_pheiTarget && !m_pheiTarget->ReadOnly();
		}

		/**
		 * Determines whether the edit box is open (values are being typed but have not been written yet).
		 * 
		 * \return Returns true if the edit box is open.
		 **/
		inline bool									IsEditing() const { return m_esEdit.bActive; }

		/**
		 * Closes the edit box without writing anything.  The caret returns to where the edit began.
		 **/
		void										CancelEdit();

		/**
		 * Writes what was typed into the edit box and closes it.
		 * 
		 * \param _swsStatus Receives an error message on failure.
		 * \return Returns false if a typed value is invalid (the edit box stays open so that it can be corrected) or could not be written.
		 **/
		bool										CommitEdit( CSecureWString &_swsStatus );

		/**
		 * Ends the Undo group that collects real-time edits.  It normally ends 0.2 seconds after the last keystroke; call this before
		 *	anything else changes the data or the Undo stack.
		 **/
		void										FlushRealTimeUndo();

		/**
		 * Clears the highlighting of every changed value.
		 **/
		void										ClearAllChanges();

		/**
		 * Clears the highlighting of the changed values in the selection, or of the byte at the caret if nothing is selected.
		 **/
		void										ClearSelectedChanges();

		/**
		 * Clears the highlighting of the changed value at the address that was right-clicked to open the context menu (the whole
		 *	value in the area that was clicked), or at the caret if the menu was not opened by a click on an address.
		 **/
		void										ClearClickedChange();

		/**
		 * Forgets the address that was right-clicked, so that ClearClickedChange() uses the caret (for a context menu opened with
		 *	the keyboard).
		 **/
		inline void									ForgetContextAddress() { m_bContextAddr = false; }

		/**
		 * Selects a range of bytes, with the caret after the last one.
		 *
		 * \param _ui64Start The first byte to select.
		 * \param _ui64Size The number of bytes to select.  If 0, the selection is removed and the caret is put at _ui64Start.
		 * \return Returns false if the range starts beyond the end of the data.  A range that runs past the end is cut short.
		 **/
		bool										SelectRange( uint64_t _ui64Start, uint64_t _ui64Size );

		/**
		 * Determines whether any values are highlighted as changed.
		 *
		 * \return Returns true if at least one byte is highlighted as changed.
		 **/
		inline bool									HasChanges() const { return !m_mChanges.empty(); }

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

		/**
		 * Reads from a given address in the hew control.
		 * 
		 * \param _ui64Addr The address from which to read.
		 * \param _vReturn Holds the read values.
		 * \param _sSize The number of bytes to read.
		 * \return Returns true if all bytes were read.
		 **/
		bool										Read( uint64_t _ui64Addr, std::vector<uint8_t> &_vReturn, size_t _sSize );

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

		/**
		 * Sets the number of pixels between lines in the hex/binary/octal view.
		 * 
		 * \param _ui16Pixels The number of pixels between lines to set.
		 **/
		void										SetHexLineSpacing( uint16_t _ui16Pixels ) {
			MX_STYLE & stAll = (*CurStyle());
			stAll.i32LineSpacingPx = int32_t( _ui16Pixels );

			RecalcAndInvalidate();
		}

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

		/**
		 * Handles WM_RBUTTONDOWN.  Right-clicking a byte outside the selection moves the caret to it, so that the context menu applies to
		 *	it.
		 *
		 * \param _iHitTest The virtual keys that are down.
		 * \param _pCursorPos The cursor position in client coordinates.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							RButtonDown( INT _iHitTest, const POINTS &_pCursorPos );

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

		/**
		 * The WM_SYSKEYDOWN handler.  Collects the digits of an Alt code (Alt held while typing on the numeric keypad).
		 *
		 * \param _uiKeyCode The virtual-key code.
		 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							SysKeyDown( UINT _uiKeyCode, UINT _uiFlags );

		/**
		 * The WM_SYSKEYUP handler.  When Alt is released after an Alt code above 255, types the Unicode character with that code (Windows
		 *	itself would produce the character for the code modulo 256).
		 *
		 * \param _uiKeyCode The virtual-key code.
		 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							SysKeyUp( UINT _uiKeyCode, UINT _uiFlags );

		/**
		 * The WM_KEYUP handler.  Alt can be released as a WM_KEYUP rather than a WM_SYSKEYUP after typing an Alt code.
		 *
		 * \param _uiKeyCode The virtual-key code.
		 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED							KeyUp( UINT _uiKeyCode, UINT _uiFlags );

		/**
		 * \brief Receives committed text input as a Unicode code point.
		 *
		 * \param _c32Cp The committed Unicode code point.
		 * \param _uiRepeat The repeat count (normally 1).
		 * \return Returns LSW_H_HANDLED if consumed.
		 */
		virtual LSW_HANDLED							OnTextInputCodePoint( char32_t _c32Cp, uint32_t _uiRepeat );

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
		void										SetStream( CHexEditorInterface * _pediStream, CHexEditorControlHost * _pwMainCtrl );

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

		/** How the edit box interprets what is typed into it, by data format. */
		enum MX_EDIT_KIND {
			MX_EK_DIGITS,																					/**< Hex, binary, octal, decimal: a fixed number of digits per byte, typed over. */
			MX_EK_TEXT,																						/**< Characters, encoded with the view's character set. */
			MX_EK_VALUES,																					/**< Multi-byte numbers: one typed string per value, separated by spaces, commas or tabs. */
		};

		/** Cursor movements within the edit box. */
		enum MX_EDIT_NAV {
			MX_EN_LEFT,																						/**< Left arrow. */
			MX_EN_RIGHT,																					/**< Right arrow. */
			MX_EN_UP,																						/**< Up arrow: one row up. */
			MX_EN_DOWN,																						/**< Down arrow: one row down. */
			MX_EN_HOME,																						/**< Home: the start of the value (or of the edit box for digits). */
			MX_EN_END,																						/**< End: the end of the value (or of the edit box for digits). */
			MX_EN_NEXT_ITEM,																				/**< Tab or Space: the next cell or value. */
			MX_EN_PREV_ITEM,																				/**< Shift+Tab: the previous cell or value. */
		};

		/** The edit box. */
		struct MX_EDIT_STATE {
			bool									bActive				= false;							/**< The edit box is open. */
			bool									bRightArea			= false;							/**< The edit box is in the right area. */
			bool									bInsert				= false;							/**< Writing inserts the bytes rather than replacing existing ones. */
			bool									bAutoCommit			= false;							/**< Writes itself as soon as the text no longer ends in an unfinished escape (real-time text editing). */
			bool									bEscapeOpen			= false;							/**< MX_EK_TEXT: the text ends in an unfinished escape. */
			MX_DATA_FMT								dfFmt				= MX_DF_HEX;						/**< The format being typed. */
			MX_EDIT_KIND							ekKind				= MX_EK_DIGITS;						/**< How typing is interpreted. */
			uint64_t								ui64Start			= 0;								/**< The address of the first edited cell. */

			std::vector<uint8_t>					vDigits;												/**< MX_EK_DIGITS: every digit of every covered cell, most-significant first. */
			std::vector<uint8_t>					vOrigDigits;											/**< MX_EK_DIGITS: the digits of every covered cell before editing. */
			std::vector<uint8_t>					vTypedMask;												/**< MX_EK_DIGITS: per covered cell, a bit for each digit that was typed. */
			size_t									sDigitPos			= 0;								/**< MX_EK_DIGITS: the next digit to type (cell * digits-per-cell + digit). */

			std::u32string							u32Text;												/**< MX_EK_TEXT: the typed text, escapes unresolved. */
			std::vector<uint8_t>					vPreview;												/**< MX_EK_TEXT: the bytes the text encodes to now. */

			std::vector<std::wstring>				vTexts;													/**< MX_EK_VALUES: the text of each value (empty if skipped over). */
			size_t									sItem				= 0;								/**< MX_EK_VALUES: the value under the cursor. */
			size_t									sCursor				= 0;								/**< MX_EK_VALUES: the cursor within the value under it.  MX_EK_TEXT: the cursor within u32Text. */

			MX_SELECTION							sOldSel;												/**< The selection before the edit began. */
			MX_SELECT_GESTURE						sgOldGesture;											/**< The caret and anchors before the edit began. */
		};

		/** A run of bytes that changed at the same time. */
		struct MX_CHANGE_RUN {
			uint64_t								ui64End				= 0;								/**< The end of the run (exclusive). */
			uint32_t								ui32Time			= 0;								/**< When the bytes changed: milliseconds after m_ui64ChangeEpoch. */
		};

		/** How often to look for changed values while nothing is changing (milliseconds). */
		static constexpr uint32_t					MX_WATCH_SLOW_MS	= 100;
		/** How often to look for changed values right after a change (milliseconds). */
		static constexpr uint32_t					MX_WATCH_FAST_MS	= 20;
		/** How much longer the wait becomes after each look that finds nothing new (milliseconds). */
		static constexpr uint32_t					MX_WATCH_SLOWDOWN_MS = 4;
		/** How long a changed value takes to fade from the Modified color to its normal color (milliseconds).  0 = it does not fade. */
		static constexpr uint32_t					MX_CHANGE_FADE_MS	= 0;


		// == Members.
		CHexEditorControlHost *						m_pwHexParent = nullptr;								/**< The main hex-editor window. */
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
		CUtilities::MX_TIMER						m_tUpdateSelection;										/**< Updates the status-bar selection text. */
		uint8_t										m_ui8UpdateSelectCnt = 0;								/**< Every few mouse drags the selection data is forcibly updated. */
		CUtilities::MX_TIMER						m_tCaretBlink;											/**< The blinking of the caret. */
		bool										m_bCaretOn = true;										/**< Caret on or off during active blink. */

		bool										m_bBigEnd = false;										/**< True for displaying Unicode as Big-Endian. */

		MX_EDIT_STATE								m_esEdit;												/**< The edit box. */
		CHexEditorInterface::CBuffer				m_bEditBuffer;											/**< The displayed data with the edit box's bytes in place. */
		bool										m_bRtUndoOpen = false;									/**< An Undo group collecting real-time edits is open. */
		MX_SELECTION								m_sRtOldSel {};											/**< The selection before the real-time edits in the open Undo group. */
		MX_SELECT_GESTURE							m_sgRtOldGesture {};									/**< The caret and anchors before the real-time edits in the open Undo group. */
		CUtilities::MX_TIMER						m_tRtUndo;												/**< Ends the real-time Undo group shortly after typing stops. */
		uint32_t									m_ui32AltCode = 0;										/**< The number typed so far on the numeric keypad with Alt held. */
		uint32_t									m_ui32AltDigits = 0;									/**< The number of digits in m_ui32AltCode. */
		bool										m_bAltLeadingZero = false;								/**< The Alt code began with 0 (ANSI code page rather than OEM). */
		char32_t									m_c32AltEmitted = 0;									/**< A character this control typed for an Alt code above 255. */
		wchar_t										m_wcAltSwallow = 0;										/**< The character Windows produces for that Alt code, which is discarded. */

		std::map<uint64_t, MX_CHANGE_RUN>			m_mChanges;												/**< Changed bytes: runs that do not overlap, by start address. */
		uint64_t									m_ui64ContextAddr = 0;									/**< The address last right-clicked (for the context menu). */
		bool										m_bContextAddr = false;									/**< m_ui64ContextAddr is valid. */
		bool										m_bContextRight = false;								/**< m_ui64ContextAddr was clicked in the right (text) area. */
		uint64_t									m_ui64ChangeEpoch = 0;									/**< The tick count (milliseconds) to which change times are relative. */
		CHexEditorInterface::CBuffer				m_bWatchPrev;											/**< The bytes in view the last time changes were looked for. */
		uint64_t									m_ui64WatchAddr = 0;									/**< The address of the first byte in m_bWatchPrev. */
		uint64_t									m_ui64WatchSize = 0;									/**< The size of the data when m_bWatchPrev was read. */
		bool										m_bWatchValid = false;									/**< m_bWatchPrev can be compared against. */
		uint32_t									m_ui32WatchPeriod = MX_WATCH_SLOW_MS;					/**< The current wait between looks for changed values (milliseconds). */
		CUtilities::MX_TIMER						m_tWatch;												/**< Looks for changed values in view. */

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
		 * \param _bEditFade If true, the text of a cell inside the edit box that has not been typed over is faded.
		 * \return Returns the text color for the given cell.
		 **/
		COLORREF									CellFgColor( uint64_t _ui64Address, const uint8_t * _pui8Value, size_t _sSize, bool _bRightArea, bool _bEditFade = true );

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
		 * Types one character, the way a WM_CHAR would.  Starts or continues an edit, or edits in real time.
		 * 
		 * \param _c32Cp The character.
		 * \param _uiRepeat How many times to type it.
		 **/
		void										TypeCodePoint( char32_t _c32Cp, uint32_t _uiRepeat );

		/**
		 * Types one character when no edit box is open: decides between real-time editing and opening the edit box.
		 * 
		 * \param _c32Cp The character.
		 **/
		void										TypeFirst( char32_t _c32Cp );

		/**
		 * Determines whether typing in a given format changes the data as each character is typed.
		 * 
		 * \param _dfFmt The format being typed.
		 * \return Returns true for real-time editing, false if the edit box collects the typing.
		 **/
		bool										RealTimeFor( MX_DATA_FMT _dfFmt ) const;

		/**
		 * Opens the edit box.
		 * 
		 * \param _dfFmt The format being typed.
		 * \param _bRightArea True if the edit box is in the right area.
		 * \param _ui64Addr The address of the first edited cell.
		 * \param _bAutoCommit If true, the edit writes itself as soon as its text no longer ends in an unfinished escape.
		 **/
		void										BeginEdit( MX_DATA_FMT _dfFmt, bool _bRightArea, uint64_t _ui64Addr, bool _bAutoCommit );

		/**
		 * Types one character into the open edit box.  Characters that are not valid for the format are skipped.
		 * 
		 * \param _c32Cp The character.
		 **/
		void										EditType( char32_t _c32Cp );

		/**
		 * Removes what is typed before the cursor in the edit box (a digit goes back to what it was).  At the start of a value, moves to
		 *	the end of the previous one.  Closes the edit box if nothing typed remains.
		 **/
		void										EditBackspace();

		/**
		 * Removes what is typed under the cursor in the edit box (a digit goes back to what it was).  Closes the edit box if nothing typed
		 *	remains.
		 **/
		void										EditDelete();

		/**
		 * Finishes a change made by EditBackspace() or EditDelete(): closes the edit box if nothing typed remains, otherwise trims it and
		 *	redraws it.
		 **/
		void										EditChanged();

		/**
		 * Determines whether anything typed remains in the edit box.
		 *
		 * \return Returns true if at least one digit, character or value is typed.
		 **/
		bool										EditHasTyping() const;

		/**
		 * Moves the cursor in the edit box.  Digits: by digit, by cell (Tab, Space, Shift+Tab) or by row; values: by character within a
		 *	value, by value (Tab, Space, Shift+Tab) or by row; text: by character.  Moving past either end of the edit box extends it,
		 *	except before its start in Insert mode and past the end of process memory.
		 *
		 * \param _enNav The movement.
		 **/
		void										EditNavigate( MX_EDIT_NAV _enNav );

		/**
		 * Grows the edit box backward by whole cells (digits) or values.  Not possible in Insert mode (the edit box is new data inserted
		 *	at its start) or before address 0.
		 *
		 * \param _sCount The number of cells or values to add before the start.
		 * \return Returns true if the edit box grew.
		 **/
		bool										EditPrepend( size_t _sCount );

		/**
		 * Determines whether the edit box can cover a cell (digits) or value.  Process memory cannot grow, so nothing past its end can be
		 *	covered.
		 *
		 * \param _sIdx The index of the cell or value relative to the start of the edit box.
		 * \return Returns true if the cell or value can be part of the edit box.
		 **/
		bool										EditCanCover( size_t _sIdx ) const;

		/**
		 * Removes untyped cells (digits) or empty values from the ends of the edit box.
		 *
		 * \param _bKeepCursor If true, the cell or value under the cursor is kept.  If false and nothing is typed, everything is removed.
		 **/
		void										TrimEdit( bool _bKeepCursor );

		/**
		 * Writes an automatically committing edit (real-time text editing ending in an escape) as part of the real-time Undo group.
		 **/
		void										CommitAutoEdit();

		/**
		 * Makes sure the edit box holds the digits of a cell (MX_EK_DIGITS), reading its original value if needed.
		 * 
		 * \param _sCell The index of the cell relative to the start of the edit.
		 **/
		void										EnsureEditCell( size_t _sCell );

		/**
		 * Recomputes the bytes the edit box's text encodes to (MX_EK_TEXT).
		 **/
		void										UpdateEditPreview();

		/**
		 * Moves the caret to the edit box's cursor (where the next character typed goes) and scrolls it into view.
		 **/
		void										UpdateEditCaret();

		/**
		 * Gets the address just past the edit box.  The edit box covers what was typed plus the cell where the next character goes.
		 * 
		 * \return Returns the end of the edit box (exclusive).
		 **/
		uint64_t									EditEnd() const;

		/**
		 * Determines whether a cell is inside the edit box.
		 * 
		 * \param _ui64Address The address of the cell.
		 * \param _bRightArea The right area or the left area.
		 * \return Returns true if the cell is inside the edit box.
		 **/
		bool										EditCovers( uint64_t _ui64Address, bool _bRightArea ) const;

		/**
		 * Determines whether anything was typed into a cell inside the edit box.
		 * 
		 * \param _ui64Address The address of the cell.
		 * \return Returns true if the cell was typed into (wholly or partly).
		 **/
		bool										EditCellTyped( uint64_t _ui64Address ) const;

		/**
		 * Gets the bytes the edit box would write.
		 * 
		 * \param _vBytes Receives the bytes.
		 * \param _swsStatus Receives an error message if a typed value is invalid.
		 * \return Returns false if a typed value is invalid.
		 **/
		bool										EditBytes( std::vector<uint8_t> &_vBytes, CSecureWString &_swsStatus ) const;

		/**
		 * Gets the buffer to display: the data read for the screen, with the bytes the edit box would write in place (and, in Insert
		 *	mode, the data after the edit box pushed along).
		 * 
		 * \param _ui64BufAddr The address of the first byte in m_bCurBuffer.
		 * \param _sBufCap The number of bytes the screen shows.
		 * \return Returns the buffer to display.
		 **/
		const CHexEditorInterface::CBuffer &		DisplayBuffer( uint64_t _ui64BufAddr, size_t _sBufCap );

		/**
		 * Draws the text of a cell inside the edit box when it differs from the normal drawing: typed values (with the text cursor in the
		 *	value under it), digits typed over in part (typed digits normal, the rest faded), and empty new cells.
		 * 
		 * \param _hDc The device context.
		 * \param _ui64Address The address of the cell.
		 * \param _dfFmt The format of the area.
		 * \param _iX The left of the cell's text.
		 * \param _iY The top of the cell's text.
		 * \param _wsText The text that would normally be drawn.
		 * \param _pui8Value The data at the cell.
		 * \param _sSize The number of bytes at _pui8Value.
		 * \return Returns true if the cell's text was drawn (or deliberately left blank).
		 **/
		bool										DrawEditCellText( HDC _hDc, uint64_t _ui64Address, MX_DATA_FMT _dfFmt, int32_t _iX, int32_t _iY, const std::wstring &_wsText, const uint8_t * _pui8Value, size_t _sSize );

		/**
		 * Draws the parts of the edit box's sunken border that fall in one row of an area, and fills any gaps between its cells.
		 *	Called for each row during the main drawing pass, after the row's backgrounds and before its text, so each row draws only its
		 *	own pixels: the row above the edit box draws its top border, the row below draws its bottom border.
		 * 
		 * \param _hDc The device context.
		 * \param _dfFmt The format of the area.
		 * \param _iXLeft The left of the area.
		 * \param _iAreaW The width of the area.
		 * \param _iYTop The top of the first row.
		 * \param _ui32Row The row (relative to the first visible line).
		 * \param _iLineAdv The height of a row.
		 **/
		void										DrawEditBoxRow( HDC _hDc, MX_DATA_FMT _dfFmt, int32_t _iXLeft, int32_t _iAreaW, int32_t _iYTop, uint32_t _ui32Row, int32_t _iLineAdv );

		/**
		 * Gets the color of the edit box from the color of the cell beneath it: a little whiter, or a little darker if the cell is white.
		 * 
		 * \param _crBase The cell's background color.
		 * \return Returns the edit box's color.
		 **/
		static COLORREF								EditBoxColor( COLORREF _crBase );

		/**
		 * Fades a text color as if a translucent pane in the edit box's color lay over it.
		 * 
		 * \param _crText The text color.
		 * \param _crBox The edit box's color.
		 * \return Returns the faded text color.
		 **/
		static COLORREF								EditFadeColor( COLORREF _crText, COLORREF _crBox );

		/**
		 * (Re)starts the timer that looks for changed values in view, at the current rate.
		 **/
		void										StartWatch();

		/**
		 * Compares the bytes in view with those seen the last time and highlights the ones that changed.  Looks more often right after a
		 *	change (MX_WATCH_FAST_MS), then less and less often while nothing more changes, back down to MX_WATCH_SLOW_MS.
		 **/
		void										WatchForChanges();

		/**
		 * Gets the time to record for a change made now.
		 *
		 * \return Returns the number of milliseconds since m_ui64ChangeEpoch.
		 **/
		uint32_t									ChangeTimeNow();

		/**
		 * Highlights bytes as changed now.
		 *
		 * \param _ui64Addr The address of the first byte.
		 * \param _ui64Size The number of bytes.
		 **/
		void										MarkChanged( uint64_t _ui64Addr, uint64_t _ui64Size );

		/**
		 * Records a run of bytes as changed at a given time, replacing what was recorded for them.
		 *
		 * \param _ui64Start The address of the first byte.
		 * \param _ui64End The address after the last byte.
		 * \param _ui32Time When they changed (see ChangeTimeNow()).
		 **/
		void										SetChanged( uint64_t _ui64Start, uint64_t _ui64End, uint32_t _ui32Time );

		/**
		 * Removes the change highlighting of a range of bytes.
		 *
		 * \param _ui64Start The address of the first byte.
		 * \param _ui64End The address after the last byte.
		 **/
		void										ClearChanges( uint64_t _ui64Start, uint64_t _ui64End );

		/**
		 * Moves the change highlighting along after bytes are inserted or deleted, so that it stays with the bytes it belongs to.
		 *
		 * \param _ui64Addr Where the bytes were inserted or deleted.
		 * \param _ui64Size The number of bytes.
		 * \param _bInserted True if the bytes were inserted (and are highlighted as changed), false if they were deleted.
		 **/
		void										ShiftChanges( uint64_t _ui64Addr, uint64_t _ui64Size, bool _bInserted );

		/**
		 * Determines whether any of a range of bytes is highlighted as changed.
		 *
		 * \param _ui64Addr The address of the first byte.
		 * \param _ui64Size The number of bytes (a value's size).
		 * \param _ui32Time Receives when the most recent change among them happened (see ChangeTimeNow()).
		 * \return Returns true if at least one of the bytes is highlighted as changed.
		 **/
		bool										ChangedIn( uint64_t _ui64Addr, uint64_t _ui64Size, uint32_t &_ui32Time ) const;

		/**
		 * Gets the text color of a changed value: the Modified color, fading to the normal color over MX_CHANGE_FADE_MS if that is
		 *	not 0.
		 *
		 * \param _crNormal The value's normal text color.
		 * \param _ui32Time When the value changed (see ChangeTimeNow()).
		 * \return Returns the text color.
		 **/
		COLORREF									ChangedColor( COLORREF _crNormal, uint32_t _ui32Time ) const;

		/**
		 * Writes bytes to the data source: inserted or written over existing bytes.
		 * 
		 * \param _ui64Addr The address at which to write.
		 * \param _vBytes The bytes to write.
		 * \param _bInsert If true and the data source allows it, the bytes are inserted.
		 * \param _swsStatus Receives an error message on failure.
		 * \return Returns true if the bytes were written.
		 **/
		bool										WriteBytes( uint64_t _ui64Addr, const std::vector<uint8_t> &_vBytes, bool _bInsert, CSecureWString &_swsStatus );

		/**
		 * Opens the Undo group that collects real-time edits, if it is not open, and restarts the timer that ends it.
		 * 
		 * \param _sOldSel The selection before the edit.
		 * \param _sgOldGesture The caret and anchors before the edit.
		 **/
		void										BeginRealTimeUndo( const MX_SELECTION &_sOldSel, const MX_SELECT_GESTURE &_sgOldGesture );

		/**
		 * Edits one digit in real time (hex and binary).
		 * 
		 * \param _ui64Addr The address of the byte.
		 * \param _i32Idx The digit within the byte (0 is the most significant).
		 * \param _dfFmt The format being typed.
		 * \param _ui32Digit The digit's value.
		 **/
		void										RealTimeDigit( uint64_t _ui64Addr, int32_t _i32Idx, MX_DATA_FMT _dfFmt, uint32_t _ui32Digit );

		/**
		 * Edits one character in real time.
		 * 
		 * \param _ui64Addr The address of the character.
		 * \param _c32Cp The character.
		 **/
		void										RealTimeText( uint64_t _ui64Addr, char32_t _c32Cp );

		/**
		 * Puts the caret after a real-time edit.
		 * 
		 * \param _ui64Addr The new caret address.
		 * \param _i32Idx The new sub-caret.
		 **/
		void										SetEditedCaret( uint64_t _ui64Addr, int32_t _i32Idx );

		/**
		 * Reports a problem in the status bar.
		 * 
		 * \param _swsStatus The message.
		 **/
		void										EditWarning( const CSecureWString &_swsStatus );

		/**
		 * Encodes text with the view's character set.  Characters the character set lacks become their closest match (best fit).
		 * 
		 * \param _u32Text The text.
		 * \param _vBytes Receives the encoded bytes (appended).
		 **/
		void										EncodeText( const std::u32string &_u32Text, std::vector<uint8_t> &_vBytes ) const;

		/**
		 * Resolves escape sequences in typed text.  Each is a backslash followed by one of: a backslash, ', ", ?, a, b, f, n, r, t, v,
		 *	space, xHH, uHHHH (two in a row for a surrogate pair), UHHHHHHHH, up to three octal digits, or N{name}.  An invalid escape is
		 *	kept as typed.
		 * 
		 * \param _u32Raw The typed text.
		 * \param _u32Out Receives the resolved text.  An unfinished escape at the end is kept as typed.
		 * \return Returns true if the text ends in an unfinished escape (more typing could change its meaning).
		 **/
		static bool									ResolveEditEscapes( const std::u32string &_u32Raw, std::u32string &_u32Out );

		/**
		 * Converts typed text to the bytes of a multi-byte value.
		 * 
		 * \param _dfFmt The format of the value.
		 * \param _wsText The typed text.
		 * \param _vBytes Receives the bytes (appended, little-endian).
		 * \return Returns false if the text is not a valid value of the format.
		 **/
		static bool									ParseValue( MX_DATA_FMT _dfFmt, const std::wstring &_wsText, std::vector<uint8_t> &_vBytes );

		/**
		 * Determines whether a character can be part of a typed value of a multi-byte format.
		 * 
		 * \param _dfFmt The format of the value.
		 * \param _c32Cp The character.
		 * \param _bSignAllowed True if a sign can be typed here (at the start of a value that has none).
		 * \return Returns true if the character can be typed.
		 **/
		static bool									ValueCharValid( MX_DATA_FMT _dfFmt, char32_t _c32Cp, bool _bSignAllowed );

		/**
		 * Gets how the edit box treats a format.
		 * 
		 * \param _dfFmt The format.
		 * \return Returns the edit kind.
		 **/
		static MX_EDIT_KIND							EditKindForFormat( MX_DATA_FMT _dfFmt ) {
			switch ( _dfFmt ) {
				case MX_DF_HEX : {}							MX_FALLTHROUGH
				case MX_DF_BIN : {}							MX_FALLTHROUGH
				case MX_DF_OCT : {}							MX_FALLTHROUGH
				case MX_DF_DEC : { return MX_EK_DIGITS; }
				case MX_DF_CHAR : { return MX_EK_TEXT; }
				default : { return MX_EK_VALUES; }
			}
		}

		/**
		 * Gets the radix of a digit format.
		 * 
		 * \param _dfFmt The format.
		 * \return Returns 16, 10, 8 or 2.
		 **/
		static uint32_t								RadixForFormat( MX_DATA_FMT _dfFmt ) {
			switch ( _dfFmt ) {
				case MX_DF_BIN : { return 2; }
				case MX_DF_OCT : { return 8; }
				case MX_DF_DEC : { return 10; }
				default : { return 16; }
			}
		}

		/**
		 * Gets the value of a digit character.
		 * 
		 * \param _c32Cp The character.
		 * \param _ui32Radix The radix.
		 * \return Returns the digit's value, or -1 if the character is not a digit of the radix.
		 **/
		static int32_t								DigitValue( char32_t _c32Cp, uint32_t _ui32Radix ) {
			int32_t i32Val = -1;
			if ( _c32Cp >= U'0' && _c32Cp <= U'9' ) { i32Val = int32_t( _c32Cp - U'0' ); }
			else if ( _c32Cp >= U'a' && _c32Cp <= U'f' ) { i32Val = int32_t( _c32Cp - U'a' ) + 10; }
			else if ( _c32Cp >= U'A' && _c32Cp <= U'F' ) { i32Val = int32_t( _c32Cp - U'A' ) + 10; }
			return (i32Val >= 0 && uint32_t( i32Val ) < _ui32Radix) ? i32Val : -1;
		}

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
		 * \brief Computes the first mini-map line shown when the main view starts at a given line.
		 *
		 * This is the single source of truth for where the mini-map content starts; MiniMapAddressRange() and the view-box drag both
		 *	use it.
		 *
		 * \param _ui64TopLineMain The main view's first visible line.
		 * \param _i32ScreenHeight The height of the mini-map.
		 * \param _ui32BytesPerRow The number of bytes per mini-map row.
		 * \return Returns the first mini-map line (0 if the whole file fits in the mini-map).
		 */
		uint64_t									MiniMapFirstLine( uint64_t _ui64TopLineMain, int32_t _i32ScreenHeight, uint32_t _ui32BytesPerRow ) const;

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
		 * \brief Gets the drawn top of the "current view" box in the mini-map, as it would be if the main view started at a given line.
		 *
		 * Uses the geometry of the last DrawMiniMap() and the same content placement (MiniMapFirstLine()), so the result matches what
		 *	is on screen.  The box can start part-way through a mini-map row, so the result is fractional.
		 *
		 * \param _ui64TopLineMain The main view's first visible line.
		 * \return Returns the box top in pixels, relative to the top of the mini-map.
		 */
		double										MiniMapViewBoxTopPx( uint64_t _ui64TopLineMain ) const;

		/**
		 * \brief Gets how far the "current view" box moves in the mini-map per line the main view scrolls.
		 *
		 * The mini-map content scrolls while the box moves, so the box moves at
		 *	zoom * (mainBytesPerRow / miniBytesPerRow) * (1 - (miniLines - miniSpan) / maxTopMiniLine) pixels per line.
		 *
		 * \param _dPxPerLine Holds the returned pixels per main-view line.
		 * \param _ui64MaxFirstLine Holds the largest first line a drag can reach (totalLines - pageLines).
		 * \param _bExact Holds whether the box can follow the mouse.  If false (the box is at least as tall as the mini-map),
		 *	_dPxPerLine spreads the whole scroll range over the mini-map's height instead.
		 * \return Returns false if the mini-map has not been drawn or there is nothing to scroll.
		 */
		bool										MiniMapViewBoxModel( double &_dPxPerLine, uint64_t &_ui64MaxFirstLine, bool &_bExact ) const;

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

#include "MXHexEditorControl.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Layouts/MXUiIds.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../Utilities/MXUtilities.h"
#include "MXHexEditorCurProcess.h"
#include "MXHexEditorProcess.h"

#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <Helpers/LSWHelpers.h>


namespace mx {

	// == Members.
	// The main window class.
	ATOM CHexEditorControl::m_aAtom = 0;

	CHexEditorControl::CHexEditorControl( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, lsw::CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CParent( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ).AddStyle( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL )/*.AddStyleEx( WS_EX_CLIENTEDGE ).RemoveStyleEx( WS_EX_WINDOWEDGE )*/, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		MX_CREATION_PARMS * pcpParms = reinterpret_cast<MX_CREATION_PARMS *>(_ui64Data);
		m_pfsFonts[MX_FT_FIXED_ROW] = pcpParms->pfsFixedRowFont;
		m_pfsFonts[MX_FT_TEXT_VIEW] = pcpParms->pfsDynamicRowFont;
		m_phecForeColors = pcpParms->phecFg;
		m_phecBackColors = pcpParms->phecBg;
		m_psOptions = pcpParms->psOptions;
		StartCaretBlink();
	}

	// == Functions.
	// Sets the view type.
	void CHexEditorControl::SetViewType( MX_EDIT_AS _eaType ) {
		if ( m_eaEditAs != _eaType && _eaType >= MX_EA_TEXT && _eaType < MX_EA_TOTAL ) {
			m_eaEditAs = _eaType;
				
			GoTo( m_pheiTarget->DefaultAddress() );

			RecalcAndInvalidate();
		}
	}

	/**
	 * Sets the caret address.
	 * 
	 * \param _ui64Addr The new address to set as the caret position.
	 * \param _i32SubCaret The caret subindex.
	 * \param _bUpdateSelection If true, the selection is updated.  If false, the selection is cleared.
	 * \param _bRedraw If true the screen is redrawn immediately.
	 **/
	void CHexEditorControl::SetCaretAddr( uint64_t _ui64Addr, int32_t _i32SubCaret, bool _bUpdateSelection, bool _bRedraw ) {
		m_sgSelGesture.ui64CaretAddr = std::min( _ui64Addr, Size() );
		m_sgSelGesture.i32CaretIdx = _i32SubCaret;

		if ( _bUpdateSelection ) {
			m_sSel.bHas = true;

			if ( m_sgSelGesture.smCurrent == LSN_SM_NORMAL ) {
				// ---- Normal selection: linear address range.
				m_sSel.smMode = LSN_SM_NORMAL;
				m_sSel.sn.ui64Start = std::min( m_sgSelGesture.ui64AnchorAddr, m_sgSelGesture.ui64CaretAddr );
				m_sSel.sn.ui64End   = std::max( m_sgSelGesture.ui64AnchorAddr, m_sgSelGesture.ui64CaretAddr );
			}
			else {
				const uint32_t ui32Bpr = CurStyle()->uiBytesPerRow;
				const uint64_t ui64A = m_sgSelGesture.ui64AnchorAddr;
				const uint64_t ui64B = m_sgSelGesture.ui64CaretAddr;

				const uint64_t ui64RowA = ui64A / ui32Bpr;
				const uint32_t ui32ColA = static_cast<uint32_t>(ui64A % ui32Bpr);
				const uint64_t ui64RowB = ui64B / ui32Bpr;
				const uint32_t ui32ColB = static_cast<uint32_t>(ui64B % ui32Bpr);

				const uint64_t ui64TlRow = std::min( ui64RowA, ui64RowB );
				const uint32_t ui32TlCol = std::min( ui32ColA, ui32ColB );
				const uint64_t ui64BrRow = std::max( ui64RowA, ui64RowB );
				const uint32_t ui32BrCol = std::max( ui32ColA, ui32ColB );

				m_sSel.smMode = LSN_SM_COLUMN;
				m_sSel.sc.ui64AnchorAddr = ui64TlRow * ui32Bpr + ui32TlCol;
				m_sSel.sc.ui32Cols       = (ui32BrCol - ui32TlCol);
				m_sSel.sc.ui64Lines      = (ui64BrRow - ui64TlRow);

			}
		}
		else {
			ClearSelection();
			m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = _ui64Addr;
			m_sgSelGesture.smCurrent = CurStyle()->bSelectColumnMode ? LSN_SM_COLUMN : LSN_SM_NORMAL;
		}

		StartCaretBlink();
		EnsureVisible( m_sgSelGesture.ui64CaretAddr, _bRedraw );
	}

	/**
	 * Selects the whole file.  Not available when opening processes.
	 * 
	 * \return Returns true if the current view is not into a process.
	 **/
	bool CHexEditorControl::SelectAll() {
		if ( IsProcess() ) { return false; }

		m_sSel.bHas = Size() != 0;
		m_sSel.smMode = m_sgSelGesture.smCurrent = LSN_SM_NORMAL;
		m_sSel.sn.ui64Start = 0;
		m_sSel.sn.ui64End   = Size() - 1;
		m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sSel.sn.ui64Start;
		SetCaretAddr( Size(), 0, true );
		return true;
	}

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
	void CHexEditorControl::SelectWord() {
		const uint64_t ui64TotalBytes = Size();
		if ( ui64TotalBytes == 0 || !m_pheiTarget ) {
			ClearSelection();
			return;
		}
		try {
			bool bIsFirst = false;
			CHexEditorInterface::CBuffer bTmp;
			bTmp.resize( 1 );

			// ===== Column mode or no selection: caret-based behavior. =====
			if ( !m_sSel.HasSelection() || m_sSel.smMode != LSN_SM_NORMAL ) {
				const uint64_t ui64CaretAddr = GetCaretAddr();
				if ( ui64CaretAddr >= ui64TotalBytes ) {
					return;
				}

				if ( !m_pheiTarget->Read( ui64CaretAddr, bTmp, 1 ) ) { return; }

				wchar_t wcHold = bTmp[0];
				if ( int32_t( CCharSets::m_csSets[CurStyle()->csCharSet].pfDisplayable( bTmp.data(), 1, wcHold ) ) == 1 ) {}
				else { wcHold = L'\0'; }

				// If caret is on a non-word character, unselect.
				if ( !ee::CExpEval::IsIdentifier( char( wcHold ), bIsFirst ) ) {
					ClearSelection();
					::InvalidateRect( Wnd(), nullptr, FALSE );
					return;
				}

				uint64_t ui64SelStart = ui64CaretAddr;
				uint64_t ui64SelEnd   = ui64CaretAddr;

				// Expand left using FindWordBoundary (backward).
				uint64_t ui64Boundary = 0;
				if ( FindWordBoundary( ui64CaretAddr, false, 0, ui64Boundary ) ) {
					// Boundary is first non-word; word starts at boundary+1.
					ui64SelStart = ui64Boundary + 1;
				}
				else { ui64SelStart = 0; }

				// Expand right using FindWordBoundary (forward).
				if ( FindWordBoundary( ui64CaretAddr, true, ui64TotalBytes - 1, ui64Boundary ) ) {
					// Boundary is first non-word; word ends at boundary-1.
					if ( ui64Boundary > 0 ) {
						ui64SelEnd = ui64Boundary - 1;
					}
					else { ui64SelEnd = 0; }
				}
				else { ui64SelEnd = ui64TotalBytes - 1; }

				m_sSel.bHas = true;
				m_sSel.smMode = m_sgSelGesture.smCurrent = LSN_SM_NORMAL;
				m_sSel.sn.ui64Start = ui64SelStart;
				m_sSel.sn.ui64End   = ui64SelEnd;
				m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sSel.sn.ui64Start;
				SetCaretAddr( m_sSel.sn.ui64End + 1, 0, true );
				return;
			}

			// ===== Normal mode with an existing selection: improved semantics. =====
			// Work on a normalized copy of the selection range.
			uint64_t ui64SelStart = m_sSel.sn.ui64Start;
			uint64_t ui64SelEnd   = m_sSel.sn.ui64End ? (m_sSel.sn.ui64End - 1) : m_sSel.sn.ui64End;
			if ( ui64SelStart > ui64SelEnd ) {
				std::swap( ui64SelStart, ui64SelEnd );
			}
			if ( ui64SelStart >= ui64TotalBytes ) { return; }
			if ( ui64SelEnd >= ui64TotalBytes ) {
				ui64SelEnd = ui64TotalBytes - 1;
			}

			uint64_t ui64NewStart = ui64SelStart;
			uint64_t ui64NewEnd   = ui64SelEnd;

			// ----- Adjust start side. -----
			bool bStartIsWord = m_pheiTarget->Read( ui64NewStart, bTmp, 1 );
			wchar_t wcHold = bTmp[0];
			if ( int32_t( CCharSets::m_csSets[CurStyle()->csCharSet].pfDisplayable( bTmp.data(), 1, wcHold ) ) == 1 ) {
					
			}
			else {
				wcHold = L'\0';
			}
			bStartIsWord = ee::CExpEval::IsIdentifier( char( wcHold ), bIsFirst );

			if ( bStartIsWord ) {
				// Start is inside a word: expand toward 0 until first non-word.
				uint64_t ui64Boundary = 0;
				if ( FindWordBoundary( ui64NewStart, false, 0, ui64Boundary ) ) {
					ui64NewStart = ui64Boundary + 1;
				}
				else {
					ui64NewStart = 0;
				}
			}
			else {
				// Start is not a word: move upward toward the end of the selection until
				// we encounter a word boundary (first word byte) or reach ui64SelEnd.
				uint64_t ui64Boundary = 0;
				if ( FindWordBoundary( ui64NewStart, true, ui64SelEnd, ui64Boundary ) ) {
					// Boundary is first word byte in that direction.
					ui64NewStart = ui64Boundary;
				}
				// If no boundary found, we leave ui64NewStart where it is (no word in that span).
			}

			// ----- Adjust end side. -----
			bool bEndIsWord = m_pheiTarget->Read( ui64NewEnd, bTmp, 1 );
			wcHold = bTmp[0];
			if ( int32_t( CCharSets::m_csSets[CurStyle()->csCharSet].pfDisplayable( bTmp.data(), 1, wcHold ) ) == 1 ) {}
			else { wcHold = L'\0'; }
			bEndIsWord = ee::CExpEval::IsIdentifier( char( wcHold ), bIsFirst );

			if ( bEndIsWord ) {
				// End is inside a word: move toward EOF until first non-word.
				uint64_t ui64Boundary = 0;
				if ( FindWordBoundary( ui64NewEnd, true, ui64TotalBytes - 1, ui64Boundary ) ) {
					if ( ui64Boundary > 0 ) {
						ui64NewEnd = ui64Boundary - 1;
					}
					else {
						ui64NewEnd = 0;
					}
				}
				else {
					ui64NewEnd = ui64TotalBytes - 1;
				}
			}
			else {
				// End is not a word: move toward 0 until a word boundary or beginning of selection.
				uint64_t ui64Boundary = 0;
				if ( ui64NewEnd > ui64NewStart &&
					FindWordBoundary( ui64NewEnd, false, ui64NewStart, ui64Boundary ) ) {
					// Boundary is first word byte when scanning backward; treat it as the new end.
					ui64NewEnd = ui64Boundary;
				}
				// If no boundary found, ui64NewEnd stays where it is (no word on that side).
			}

			// Ensure new range is sane.
			if ( ui64NewStart > ui64NewEnd ) {
				// No usable word inside the selection; vanish the selection.
				ClearSelection();
				::InvalidateRect( Wnd(), nullptr, FALSE );
				return;
			}

			// Final safety: if both endpoints are non-word, treat as "no word in selection" and vanish.
			bool bStartFinalWord = false;
			bool bEndFinalWord = false;
			if ( m_pheiTarget->Read( ui64NewStart, bTmp, 1 ) ) {
				wchar_t wcHold = bTmp[0];
				if ( int32_t( CCharSets::m_csSets[CurStyle()->csCharSet].pfDisplayable( bTmp.data(), 1, wcHold ) ) == 1 ) {}
				else { wcHold = L'\0'; }
				bStartFinalWord = ee::CExpEval::IsIdentifier( char( wcHold ), bIsFirst );
			}
			if ( m_pheiTarget->Read( ui64NewEnd, bTmp, 1 ) ) {
				wchar_t wcHold = bTmp[0];
				if ( int32_t( CCharSets::m_csSets[CurStyle()->csCharSet].pfDisplayable( bTmp.data(), 1, wcHold ) ) == 1 ) {}
				else { wcHold = L'\0'; }
				bEndFinalWord = ee::CExpEval::IsIdentifier( char( wcHold ), bIsFirst );
			}
			if ( !bStartFinalWord && !bEndFinalWord ) {
				ClearSelection();
				::InvalidateRect( Wnd(), nullptr, FALSE );
				return;
			}

			m_sSel.bHas = true;
			m_sSel.smMode = m_sgSelGesture.smCurrent = LSN_SM_NORMAL;
			m_sSel.sn.ui64Start = ui64NewStart;
			m_sSel.sn.ui64End   = ui64NewEnd;
			m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sSel.sn.ui64Start;
			SetCaretAddr( m_sSel.sn.ui64End + 1, 0, true );
		}
		catch ( ... ) {}
	}

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
	void CHexEditorControl::SelectLine() {
		const uint64_t ui64TotalBytes = Size();
		if ( ui64TotalBytes == 0 ) {
			ClearSelection();
			return;
		}

		const uint32_t ui32Bpr = CurStyle()->uiBytesPerRow;
		if ( ui32Bpr == 0 ) {
			ClearSelection();
			return;
		}

		uint64_t ui64StartRef = 0;
		uint64_t ui64EndRef   = 0;

		if ( m_sSel.bHas ) {
			if ( m_sSel.smMode == LSN_SM_NORMAL ) {
				// Low/high of the Normal selection.
				const uint64_t ui64SelStart = m_sSel.sn.ui64Start;
				const uint64_t ui64SelEnd   = m_sSel.sn.ui64End;
				ui64StartRef = (ui64SelStart <= ui64SelEnd) ? ui64SelStart : ui64SelEnd;
				ui64EndRef   = (ui64SelStart <= ui64SelEnd) ? ui64SelEnd   : ui64SelStart;
			}
			else {
				// COLUMN mode: lower-left and upper-right corners.
				const uint64_t ui64AnchorAddr = m_sSel.sc.ui64AnchorAddr;
				const uint64_t ui64AnchorRow  = ui64AnchorAddr / ui32Bpr;
				const uint32_t ui32AnchorCol  = static_cast<uint32_t>(ui64AnchorAddr % ui32Bpr);

				const uint64_t ui64LastRow = ui64AnchorRow + m_sSel.sc.ui64Lines;
				const uint32_t ui32LastCol = ui32AnchorCol + m_sSel.sc.ui32Cols;

				// Lower-left: last row, leftmost column.
				const uint64_t ui64LowerLeftAddr = ui64LastRow * ui32Bpr + ui32AnchorCol;
				// Upper-right: first row, rightmost column.
				const uint64_t ui64UpperRightAddr = ui64AnchorRow * ui32Bpr + ui32LastCol;

				ui64StartRef = ui64UpperRightAddr;
				ui64EndRef   = ui64LowerLeftAddr;
			}
		}
		else {
			// No selection: use caret as both start and end reference.
			const uint64_t ui64CaretAddr = GetCaretAddr();
			if ( ui64CaretAddr >= ui64TotalBytes ) { return; }
			ui64StartRef = ui64CaretAddr;
			ui64EndRef   = ui64CaretAddr;
		}

		// Clamp into file range.
		if ( ui64StartRef >= ui64TotalBytes ) {
			ui64StartRef = ui64TotalBytes - 1;
		}
		if ( ui64EndRef >= ui64TotalBytes ) {
			ui64EndRef = ui64TotalBytes - 1;
		}


		const uint64_t ui64StartRow     = ui64StartRef / ui32Bpr;
		const uint64_t ui64EndRow       = ui64EndRef   / ui32Bpr;
		const uint64_t ui64LineStartAdr = ui64StartRow * ui32Bpr;
		uint64_t ui64LineEndAdr         = (ui64EndRow + 1) * ui32Bpr - 1;

		if ( ui64LineEndAdr >= ui64TotalBytes ) {
			ui64LineEndAdr = ui64TotalBytes - 1;
		}

		
		m_sSel.bHas = true;
		m_sSel.smMode = m_sgSelGesture.smCurrent = LSN_SM_NORMAL;
		m_sSel.sn.ui64Start = ui64LineStartAdr;
		m_sSel.sn.ui64End   = ui64LineEndAdr;
		m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sSel.sn.ui64Start;

		SetCaretAddr( m_sSel.sn.ui64End + 1, 0, true );
	}

	/**
	 * Goes to a given address.
	 * 
	 * \param _ui64Addr The address to which to go.
	 * \param _bShowAtTop If true, the scroll be aligned such that the address is on the top of the screen, otherwise it will be in the center of the screen.
	 **/
	void CHexEditorControl::GoTo( uint64_t _ui64Addr, bool _bShowAtTop ) {
		_ui64Addr = std::min( _ui64Addr, Size() );


		// Compute visible page sizes (lines / columns-on-screen).
		int32_t iClientH = int32_t( ClientRect().Height() );

		const int32_t iLineAdv = LineAdvanceCy();
		m_sdScrollView[m_eaEditAs].i32PageLines = (iLineAdv ? (iClientH / iLineAdv) : 0);
		if ( m_sdScrollView[m_eaEditAs].i32PageLines < 1 ) { m_sdScrollView[m_eaEditAs].i32PageLines = 1; }


		const uint64_t ui64Lines = TotalLines_FixedWidth();
		const uint64_t ui64MaxV  = ui64Lines ? (ui64Lines - 1ULL) : 0ULL;

		uint64_t ui64Pos = _ui64Addr / CurStyle()->uiBytesPerRow;
		if ( !_bShowAtTop ) {
			if ( ui64Pos <= m_sdScrollView[m_eaEditAs].i32PageLines / 2 ) {
				ui64Pos = 0;
			}
			else {
				ui64Pos -= m_sdScrollView[m_eaEditAs].i32PageLines / 2;
			}
		}
		m_sdScrollView[m_eaEditAs].ui64VPos = ui64Pos;

		// Keep style in sync with the new top visible line.
		m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine = m_sdScrollView[m_eaEditAs].ui64VPos;

		lsw::CBase::SetScrollInfo64( Wnd(), SB_VERT, SIF_POS, ui64MaxV, m_sdScrollView[m_eaEditAs].ui64VPos, 1, TRUE );
		::InvalidateRect( Wnd(), nullptr, FALSE );
	}

	/**
	 * Ensures a given address is visible.  If the address is already visible, no scrolling is done, otherwise it scrolls just enough to put the address on the top
	 *	or bottom of the screen.
	 * 
	 * \param _ui64Addr The address to make visible.
	 * \param _bRefresh If true, the control is redrawn.
	 **/
	void CHexEditorControl::EnsureVisible( uint64_t _ui64Addr, bool _bRefresh ) {
		_ui64Addr = std::min( _ui64Addr, Size() );


		// Compute visible page sizes (lines / columns-on-screen).
		int32_t iClientH = int32_t( ClientRect().Height() );

		const int32_t iLineAdv = LineAdvanceCy();
		m_sdScrollView[m_eaEditAs].i32PageLines = (iLineAdv ? (iClientH / iLineAdv) : 0);
		if ( m_sdScrollView[m_eaEditAs].i32PageLines < 1 ) { m_sdScrollView[m_eaEditAs].i32PageLines = 1; }

		const uint64_t ui64Lines = TotalLines_FixedWidth();
		const uint64_t ui64MaxV  = ui64Lines ? (ui64Lines - 1ULL) : 0ULL;

		uint64_t ui64Pos = _ui64Addr / CurStyle()->uiBytesPerRow;

		if ( ui64Pos >= m_sdScrollView[m_eaEditAs].ui64VPos && ui64Pos < m_sdScrollView[m_eaEditAs].ui64VPos + m_sdScrollView[m_eaEditAs].i32PageLines ) {
			if ( _bRefresh ) {
				::InvalidateRect( Wnd(), nullptr, FALSE );
			}
			return;
		}
		if ( ui64Pos >= m_sdScrollView[m_eaEditAs].ui64VPos + m_sdScrollView[m_eaEditAs].i32PageLines ) {
			ui64Pos = (ui64Pos - m_sdScrollView[m_eaEditAs].i32PageLines) + 1;
		}

		m_sdScrollView[m_eaEditAs].ui64VPos = ui64Pos;
		// Keep style in sync with the new top visible line.
		m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine = m_sdScrollView[m_eaEditAs].ui64VPos;

		lsw::CBase::SetScrollInfo64( Wnd(), SB_VERT, SIF_POS, ui64MaxV, m_sdScrollView[m_eaEditAs].ui64VPos, 1, TRUE );
		if ( _bRefresh ) {
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}
	}

	// WM_NCDESTROY.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::NcDestroy() {
		return CParent::NcDestroy();
	}

	// Setting the HWND after the control has been created.
	void CHexEditorControl::InitControl( HWND _hWnd ) {
		//m_iCxChar = m_iCyChar = 0;
		//m_iClientW = m_iClientH = 0;
		//m_sStyles[MX_EA_HEX].uiBytesPerRow = 16;
		/*m_pData = nullptr;
		m_ui64Size = 0ULL;*/
		/*m_iVPos = 0;
		m_iHPos = 0;*/
		m_sdScrollView[m_eaEditAs].i32PageLines = m_sdScrollView[m_eaEditAs].i32PageCols = 1;
		//m_iAddrDigits = 8;
		//SetState( _hWnd, ps );

		//ChooseDefaultFont();
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

	/**
	 * Handles WM_SETFOCUS.
	 * \brief Notified when the window receives keyboard focus.
	 *
	 * \param _hPrevFocus The window that previously had focus.
	 * \return Returns a LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::SetFocus( HWND /*_hPrevFocus*/ ) {
		StartCaretBlink();
		::InvalidateRect( Wnd(), nullptr, FALSE );
		return lsw::CWidget::LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_KILLFOCUS.
	 * \brief Notified when the window loses keyboard focus.
	 *
	 * \param _hNewFocus The window receiving focus (may be NULL).
	 * \return Returns a LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::KillFocus( HWND /*_hNewFocus*/ ) {
		StopCaretBlink();
		::InvalidateRect( Wnd(), nullptr, FALSE );
		return lsw::CWidget::LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_PAINT.
	 * \brief Performs painting for the client area.
	 *
	 * \return Returns a LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::Paint() {
		lsw::LSW_RECT rRect = ClientRect();
		lsw::LSW_BEGINPAINT bpPaint( Wnd() );
		
		HDC hDcMem = ::CreateCompatibleDC( bpPaint.hDc );
		HBITMAP hBmp = ::CreateCompatibleBitmap( bpPaint.hDc, int32_t( rRect.Width() ), int32_t( rRect.Height() ) );
		{
			lsw::LSW_SELECTOBJECT soBmp( hDcMem, hBmp );
			PaintHex( hDcMem, rRect );


			::BitBlt( bpPaint.hDc, 0, 0, int32_t( rRect.Width() ), int32_t( rRect.Height() ), hDcMem, 0, 0, SRCCOPY );
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
		//ChooseDefaultFont();
		Font()->lfFontParms.lfHeight = -::MulDiv( Font()->i32PointSize, static_cast<int32_t>(m_wDpiY), 72 );
		Font()->fFont.CreateFontIndirectW( &Font()->lfFontParms );
		RecalcAndInvalidate();
		return CParent::DpiChanged( _wX, _wY, _pRect );
	}

	/**
	 * Increases the font size.  Returns the new font parameters and the view type.  Hex, Octal, and Bindary displays are linked, etc.
	 * 
	 * \param _lfFont Holds the returned font for applying to other tabs
	 * \return Returns true if the view type is Hex, Octal, or Binary.
	 **/
	bool CHexEditorControl::IncreaseFontSize( LOGFONTW &_lfFont ) {
		_lfFont = Font()->lfFontParms;
		if ( Font()->i32PointSize >= 72 ) { return IsFixedRowLength(); }
		_lfFont.lfHeight = -::MulDiv( Font()->i32PointSize + 1, static_cast<int32_t>(m_wDpiY), 72 );

		if ( Font()->fFont.CreateFontIndirectW( &_lfFont ) ) {
			Font()->i32PointSize++;
			Font()->lfFontParms = _lfFont;
			RecalcAndInvalidate();
		}
		else {
			_lfFont = Font()->lfFontParms;
		}
		return IsFixedRowLength();
	}

	/**
	 * Decreases the font size.  Returns the new font parameters and the view type.  Hex, Octal, and Bindary displays are linked, etc.
	 * 
	 * \param _lfFont Holds the returned font for applying to other tabs
	 * \return Returns true if the view type is Hex, Octal, or Binary.
	 **/
	bool CHexEditorControl::DecreaseFontSize( LOGFONTW &_lfFont ) {
		_lfFont = Font()->lfFontParms;
		if ( Font()->i32PointSize <= 2 ) { return IsFixedRowLength(); }
		_lfFont.lfHeight = -::MulDiv( Font()->i32PointSize - 1, static_cast<int32_t>(m_wDpiY), 72 );

		if ( Font()->fFont.CreateFontIndirectW( &_lfFont ) ) {
			Font()->i32PointSize--;
			Font()->lfFontParms = _lfFont;
			RecalcAndInvalidate();
		}
		else {
			_lfFont = Font()->lfFontParms;
		}
		return IsFixedRowLength();
	}

	/**
	 * Sets the font size.
	 * 
	 * \param _i32PtSize The font size to set.
	 * \return Returns true if the font was changed.
	 **/
	bool CHexEditorControl::SetFontSize( int32_t _i32PtSize ) {
		if ( _i32PtSize <= 0 || _i32PtSize > 72 ) { return false; }
		Font()->i32PointSize = _i32PtSize;
		auto lfTmp = GetThisFont();
		lfTmp.lfHeight = -::MulDiv( Font()->i32PointSize, static_cast<int32_t>(m_wDpiY), 72 );
		if ( Font()->fFont.CreateFontIndirectW( &lfTmp ) ) {
			Font()->lfFontParms = lfTmp;
			RecalcAndInvalidate();
			return true;
		}
		else {
			return false;
		}
	}

	/**
	 * Sets the current font.
	 * 
	 * \param _lfFont The font to set.
	 * \param _bFixedRowLength If true, the fixed-sized fonts are updated, otherwise the variable-row-length fonts are updated.
	 * \return Returns true if setting the font succeeded,
	 **/
	bool CHexEditorControl::SetFont( const LOGFONTW &_lfFont, bool _bFixedRowLength ) {
		 LOGFONTW lfTmp = _lfFont;
		 bool bRet = false;
		 MX_FONT_TYPE ftType = _bFixedRowLength ? MX_FT_FIXED_ROW : MX_FT_TEXT_VIEW;

		if ( m_pfsFonts[ftType]->fFont.CreateFontIndirectW( &_lfFont ) ) {
			m_pfsFonts[ftType]->lfFontParms = _lfFont;
			ComputeFontMetrics( (*m_pfsFonts[ftType]), m_wDpiY, Wnd() );
			bRet = true;
		}

		 if ( bRet ) {
			 RecalcAndInvalidate();
		}
		return bRet;
	}

	/**
	 * Sets the default font size.
	 * 
	 * \param _lfFont Holds the returned font for applying to other tabs
	 * \return Returns true if the view type is Hex, Octal, or Binary.
	 **/
	bool CHexEditorControl::DefaultFontSize( LOGFONTW &_lfFont ) {
		_lfFont = Font()->lfFontParms;
		_lfFont.lfHeight = -::MulDiv( 10, static_cast<int32_t>(m_wDpiY), 72 );

		if ( Font()->fFont.CreateFontIndirectW( &_lfFont ) ) {
			Font()->i32PointSize--;
			Font()->lfFontParms = _lfFont;
			RecalcAndInvalidate();
		}
		else {
			_lfFont = Font()->lfFontParms;
		}
		return IsFixedRowLength();
	}

	/**
	 * Handles WM_TIMER.
	 * \brief Notified when a timer elapses.
	 *
	 * \param _uiptrId Timer identifier.
	 * \param _tpProc Optional callback associated with the timer.
	 * \return Returns a LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::Timer( UINT_PTR _uiptrId, TIMERPROC /*_tpProc*/ ) {
		if ( _uiptrId == m_tCaretBlink.uiptrActiveId ) {
			m_bCaretOn = !m_bCaretOn;
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_LBUTTONDOWN.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::LButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		this->CWidget::SetFocus();
		::SetCapture( Wnd() );

		const bool bShift = ((::GetKeyState( VK_SHIFT ) & 0x8000) != 0);
		const bool bCtrl  = ((::GetKeyState( VK_CONTROL ) & 0x8000) != 0);
		const bool bCtrlShift = bCtrl && bShift;

		POINT ptPoint { _pCursorPos.x, _pCursorPos.y };

		SelectionBeginGesture( ptPoint, bShift, bCtrl, bCtrlShift );

		::InvalidateRect( Wnd(), NULL, FALSE );
		return lsw::CWidget::LSW_H_HANDLED;
	}

	/** \brief Handles WM_MOUSEMOVE. */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::MouseMove( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		if ( ::GetCapture() != Wnd() ) { return lsw::CWidget::LSW_H_CONTINUE; }

		const bool bShift = ((::GetKeyState( VK_SHIFT ) & 0x8000) != 0);
		const bool bCtrl  = ((::GetKeyState( VK_CONTROL ) & 0x8000) != 0);
		const bool bCtrlShift = bCtrl && bShift;

		POINT ptPoint { _pCursorPos.x, _pCursorPos.y };
		SelectionUpdateGesture( ptPoint, bShift, bCtrl, bCtrlShift );
		::InvalidateRect( Wnd(), NULL, FALSE );
		return lsw::CWidget::LSW_H_HANDLED;
	}

	/** \brief Handles WM_LBUTTONUP. */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::LButtonUp( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		if ( ::GetCapture() == Wnd() ) { ::ReleaseCapture(); }
		SelectionEndGesture();
		::InvalidateRect( Wnd(), NULL, FALSE );
		return LSW_H_HANDLED;
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
		const bool bShift = (::GetKeyState( VK_SHIFT ) & 0x8000) != 0;

		const int32_t iPageLines = std::max( 1, m_sdScrollView[m_eaEditAs].i32PageLines - 1 );

		switch ( _uiKeyCode ) {
			case VK_UP : {
				if ( bCtrl ) {
					::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( SB_LINEUP, 0 ), 0 );
				}
				else {
					SubCaret( CurStyle()->uiBytesPerRow, m_sgSelGesture.i32CaretIdx, bShift );
				}
				break;
			}
			case VK_DOWN : {
				if ( bCtrl ) {
					::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( SB_LINEDOWN, 0 ), 0 );
				}
				else {
					AddCaret( CurStyle()->uiBytesPerRow, m_sgSelGesture.i32CaretIdx, bShift );
				}
				break;
			}
			case VK_PRIOR : {
				if ( !bCtrl ) {
					uint64_t ui64Jump = static_cast<uint64_t>(iPageLines) * CurStyle()->uiBytesPerRow;
					SubCaret( ui64Jump, m_sgSelGesture.i32CaretIdx, bShift );
				}
				break;
			}
			case VK_NEXT : {
				if ( !bCtrl ) {
					uint64_t ui64Jump = static_cast<uint64_t>(iPageLines) * CurStyle()->uiBytesPerRow;
					AddCaret( ui64Jump, m_sgSelGesture.i32CaretIdx, bShift );
				}
				break;
			}

			case VK_HOME : {
				if ( bCtrl ) {
					// Go to the beginning of the document.
					SetCaretAddr( 0, 0, bShift );
				}
				else			{
					// Go to the beginning of the line.
					SetCaretAddr( m_sgSelGesture.ui64CaretAddr / CurStyle()->uiBytesPerRow * CurStyle()->uiBytesPerRow, 0, bShift );
				}
				break;
			}
			case VK_END : {
				if ( bCtrl ) {
					// Go to the end of the document.
					SetCaretAddr( Size(), 0, bShift );
				}
				else {
					// Go to the end of the line.
					SetCaretAddr( (m_sgSelGesture.ui64CaretAddr / CurStyle()->uiBytesPerRow * CurStyle()->uiBytesPerRow) + CurStyle()->uiBytesPerRow, 0, bShift );
				}
				break;
			}

			case VK_LEFT : {
				if ( !bCtrl )	{
					// TODO: CTRL = move whole byte at a time, otherwise move i32CaretIdx.
					SubCaret( 1, m_sgSelGesture.i32CaretIdx, bShift );
				}
				else {
					::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_LINELEFT, 0 ), 0 );
				}
				/*::InvalidateRect( Wnd(), nullptr, FALSE );
				*/
				break;
			}
			case VK_RIGHT : {
				if ( !bCtrl )	{
					// TODO: CTRL = move whole byte at a time, otherwise move i32CaretIdx.
					AddCaret( 1, m_sgSelGesture.i32CaretIdx, bShift );
				}
				else {
					::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_LINERIGHT, 0 ), 0 );
				}
				break;
			}
		}


		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_HSCROLL
	lsw::CWidget::LSW_HANDLED CHexEditorControl::HScroll( USHORT /*_uScrollPos*/, USHORT _uScrollType, CWidget * /*_pwWidget*/ ) {
		const int32_t iLineStepPx = (Font()->iCharCx > 0) ? Font()->iCharCx : 1;
		const int32_t iPagePx     = GetClientWidth();

		const uint64_t ui64ContentPx = static_cast<uint64_t>(TotalContentWidthPx());
		const uint64_t ui64MaxH      = ui64ContentPx ? (ui64ContentPx - 1ULL) : 0ULL;

		uint64_t ui64Pos = m_sdScrollView[m_eaEditAs].ui64HPx;

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
		if ( ui64Pos != m_sdScrollView[m_eaEditAs].ui64HPx ) {
			m_sdScrollView[m_eaEditAs].ui64HPx = ui64Pos;
			lsw::CBase::SetScrollInfo64( Wnd(), SB_HORZ, SIF_POS, ui64MaxH, m_sdScrollView[m_eaEditAs].ui64HPx, 1, TRUE );
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_VSCROLL
	lsw::CWidget::LSW_HANDLED CHexEditorControl::VScroll( USHORT /*_uScrollPos*/, USHORT _uScrollType, CWidget * /*_pwWidget*/ ) {
		const int32_t iPageLines = std::max( 1, m_sdScrollView[m_eaEditAs].i32PageLines - 1 );
		const int32_t iStep      = 1;

		const uint64_t ui64Lines = TotalLines_FixedWidth();
		const uint64_t ui64MaxV  = ui64Lines ? (ui64Lines - 1ULL) : 0ULL;

		uint64_t ui64Pos = m_sdScrollView[m_eaEditAs].ui64VPos;

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
		if ( ui64Pos != m_sdScrollView[m_eaEditAs].ui64VPos ) {
			m_sdScrollView[m_eaEditAs].ui64VPos = ui64Pos;

			// Keep style in sync with the new top visible line.
			m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine = m_sdScrollView[m_eaEditAs].ui64VPos;

			lsw::CBase::SetScrollInfo64( Wnd(), SB_VERT, SIF_POS, ui64MaxV, m_sdScrollView[m_eaEditAs].ui64VPos, 1, TRUE );
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_MOUSEWHEEL.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::MouseWheel( DWORD _dwVirtKeys, const POINTS &/*_pCursorPos*/ ) {
		const bool bCtrl = (::GetKeyState( VK_CONTROL ) & 0x8000) != 0;
		UINT uiLines = 3;
		::SystemParametersInfoW( SPI_GETWHEELSCROLLLINES, 0, &uiLines, 0 );
		int16_t sDelta = GET_WHEEL_DELTA_WPARAM( _dwVirtKeys );
		int32_t iSteps = static_cast<int32_t>((sDelta / WHEEL_DELTA) * static_cast<int32_t>(uiLines));
		if ( !bCtrl ) {
			
			::SendMessageW( Wnd(), WM_VSCROLL, MAKELONG( (iSteps < 0) ? SB_LINEDOWN : SB_LINEUP, 0 ), 0 );
		}
		else {
			// Zoom in/out.
			LOGFONTW lfFont;
			if ( iSteps > 0 ) {
				while ( iSteps != 0 ) {
					IncreaseFontSize( lfFont );
					--iSteps;
				}
			}
			else {
				while ( iSteps != 0 ) {
					DecreaseFontSize( lfFont );
					++iSteps;
				}
			}
			UpdateSiblings();
		}
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_MOUSEHWHEEL.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::MouseHWheel( DWORD _dwVirtKeys, const POINTS &/*_pCursorPos*/ ) {
		int16_t sDelta = GET_WHEEL_DELTA_WPARAM( _dwVirtKeys );
		int32_t iSteps = (sDelta / WHEEL_DELTA);
		::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( (iSteps < 0) ? SB_LINERIGHT : SB_LINELEFT, 0 ), 0 );
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// Sets the target stream.
	void CHexEditorControl::SetStream( CHexEditorInterface * _pediStream, CWidget * _pwMainCtrl ) {
		m_pwHexParent = _pwMainCtrl;
		m_pheiTarget = _pediStream;

		GoTo( m_pheiTarget->DefaultAddress() );
		
		RecalcAndInvalidate();
	}

	/**
	 * Gets the address under the given mouse point.
	 * 
	 * \param _pPoint The client-space point under which to find the address.
	 * \param _ui64Addr The return address in the case that the function returns true.
	 * \param _bRightArea If true, the click happened in the right area.
	 * \return Returns true if there is a cell representing an address under the given point.  The address will always be valid (in the range of the opened file, memory space, etc.) if the function returns true.
	 **/
	bool CHexEditorControl::PointToAddress( const POINT &_pPoint, uint64_t &_ui64Addr, bool &_bRightArea ) {
		uint64_t ui64Max = Size();
		if MX_UNLIKELY( ui64Max == 0 ) { return false; }
		if MX_LIKELY( m_pheiTarget ) {
			const MX_STYLE & stAll = (*CurStyle());
			const int32_t iGutterW = ComputeAddressGutterWidthPx();

			int32_t i32Top = GetRulerHeightPx();
			if MX_UNLIKELY( _pPoint.y < i32Top ) { return false; }
			if MX_UNLIKELY( _pPoint.x < iGutterW ) { return false; }

			POINT pThis = { _pPoint.x - iGutterW, _pPoint.y - i32Top };


			// Line height.
			const MX_FONT_SET & fsFont = (*Font());
			const int32_t iLineAdv = fsFont.iCharCy + stAll.i32LineSpacingPx;
			uint64_t ui64TopAddress = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine * stAll.uiBytesPerRow;
			ui64TopAddress += pThis.y / iLineAdv * stAll.uiBytesPerRow;

			auto ui32WidthL = ComputeAreaWidthPx( stAll.dfLeftNumbersFmt ) + stAll.i32PadNumbersLeftPx + stAll.i32PadBetweenNumbersAndTextPx + stAll.i32PadNumbersRightPx;
			for ( size_t I = 0; I < stAll.uiBytesPerRow; ++I ) {
				lsw::LSW_RECT rRect;

				int32_t iGX = 0, iGW = 0;
				if ( GetBackgrondRectForIndex( stAll.dfLeftNumbersFmt, I, int32_t( -int64_t( m_sdScrollView[m_eaEditAs].ui64HPx ) ), iGX, iGW ) ) {
					if ( pThis.x >= iGX && pThis.x < (iGX + iGW) ) {
						_bRightArea = false;
						ui64TopAddress += I;
						_ui64Addr = std::min( ui64Max, ui64TopAddress );
						return true;
					}
				}
				if ( stAll.bShowRightArea ) {
					if ( GetBackgrondRectForIndex( stAll.dfRightNumbersFmt, I, int32_t( -int64_t( m_sdScrollView[m_eaEditAs].ui64HPx ) ) + ui32WidthL, iGX, iGW ) ) {
						if ( pThis.x >= iGX && pThis.x < (iGX + iGW) ) {
							_bRightArea = true;
							ui64TopAddress += I;
							_ui64Addr = std::min( ui64Max, ui64TopAddress );
							return true;
						}
					}
				}

			}

			return false;
		}
		return false;
	}

	// Registers the control if it has not been registered already.  Redundant calls have no effect.  Must be called before creating this control.
	void CHexEditorControl::PrepareControl() {
		if ( !m_aAtom ) {
			lsw::CWndClassEx wceEx;
			wceEx.SetInstance( lsw::CBase::GetThisHandle() );
			WCHAR szStr[15];
			mx::CUtilities::RandomString( szStr, std::size( szStr ) );
			wceEx.SetClassName( szStr );
			wceEx.SetStyle( CS_OWNDC );
			wceEx.SetBackgroundBrush( /*reinterpret_cast<HBRUSH>(CTLCOLOR_STATIC + 1)*/NULL );
			wceEx.SetWindPro( CWidget::WindowProc );
			m_aAtom = lsw::CBase::RegisterClassExW( wceEx.Obj() );
		}
	}

	// Sets a default font.
	bool CHexEditorControl::ChooseDefaultFont( MX_FONT_SET &_fsFont, WORD _wDpi, HWND _hWnd ) {
		static const wchar_t * s_ppwszFaces[] = {
			//L"Brush Script MT",
			L"Droid Sans Mono",   // Preferred.
			L"Consolas",          // Modern Windows TT.
			L"Lucida Console"     // Older Windows TT.
		};

		const int32_t iPt = _fsFont.i32PointSize;

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
		lfFont.lfHeight = -::MulDiv( iPt, static_cast<int32_t>(_wDpi), 72 );

		lsw::LSW_HDC hDc( _hWnd );
		for ( size_t i = 0; i < _countof( s_ppwszFaces ); ++i ) {
			::wcscpy_s( lfFont.lfFaceName, s_ppwszFaces[i] );

			if ( _fsFont.fFont.CreateFontIndirectW( &lfFont ) ) {
				// Verify it is TrueType by asking for outline metrics.
				lsw::LSW_SELECTOBJECT soFont( hDc.hDc, _fsFont.fFont.hFont );

				// Two-call pattern to see if OTM exists (TrueType/OpenType only).
				UINT uNeed = ::GetOutlineTextMetricsW( hDc.hDc, 0, NULL );
				if ( uNeed != 0 ) {
					// Success: keep this one.
					_fsFont.lfFontParms = lfFont;
					return true;
				}
				// else: not TrueType, try next fallback.
			}
		}
		_fsFont.fFont.CreateFontIndirectW( &_fsFont.lfFontParms );
		return false;
	}

	/**
	 * Tells the parent to update the other hex controls due to the changing of some shared values.  Typically scroll must be updated.
	 **/
	void CHexEditorControl::UpdateSiblings() const {
		auto pwTmp = m_pwParent;
		while ( pwTmp ) {
			if ( pwTmp->WidgetType() == MX_DEUS_HEX_MACHINA ) {
				::PostMessageW( pwTmp->Wnd(), CMhsMainWindow::MX_CUSTOM_MESSAGE::MX_CM_UPDATE_HEX_SIBLINGS, reinterpret_cast<WPARAM>(this), 0 );
				return;
			}
			pwTmp = pwTmp->Parent();
		}
	};

	// Draws the hex-editor view.
	bool CHexEditorControl::PaintHex( HDC _hDc, const lsw::LSW_RECT &_rRect ) {
		const MX_STYLE & stAll = (*CurStyle());
		const int32_t iGutterW = ComputeAddressGutterWidthPx();
		lsw::LSW_RECT rTmp = _rRect;
		int32_t iX = iGutterW - m_sdScrollView[m_eaEditAs].ui64HPx, iY = 0;
		lsw::LSW_RECT rBlankArea = _rRect;
		{
			if MX_LIKELY( m_pheiTarget ) {
				if MX_LIKELY( m_pheiTarget->Read( m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine * stAll.uiBytesPerRow, m_bCurBuffer, (m_sdScrollView[m_eaEditAs].i32PageLines + 1) * stAll.uiBytesPerRow ) ) {
					int32_t i32Top = GetRulerHeightPx();
					DrawArea( _hDc, iX, i32Top, stAll.dfLeftNumbersFmt, m_sdScrollView[m_eaEditAs].i32PageLines + 1, m_bCurBuffer, false );
					auto ui32WidthL = ComputeAreaWidthPx( stAll.dfLeftNumbersFmt );
					rBlankArea.top = i32Top;
					rBlankArea.left = iX + ui32WidthL;

					if ( stAll.bShowRightArea ) {
						DrawArea( _hDc, iX + ui32WidthL + stAll.i32PadNumbersLeftPx + stAll.i32PadBetweenNumbersAndTextPx + stAll.i32PadNumbersRightPx,
							i32Top, stAll.dfRightNumbersFmt, m_sdScrollView[m_eaEditAs].i32PageLines + 1, m_bCurBuffer, true );

						rBlankArea.left += stAll.i32PadNumbersLeftPx + stAll.i32PadBetweenNumbersAndTextPx + stAll.i32PadNumbersRightPx + ComputeAreaWidthPx( stAll.dfRightNumbersFmt );

						// Draw the separator.
						lsw::LSW_RECT rSep;
						rSep.left = iX + ui32WidthL;
						rSep.right = rSep.left + stAll.i32PadNumbersLeftPx + stAll.i32PadBetweenNumbersAndTextPx + stAll.i32PadNumbersRightPx;
						rSep.top = i32Top;
						rSep.bottom = rTmp.bottom;
						::FillRect( _hDc, &rSep,
							lsw::CBase::BrushCache().Brush( MX_GetRgbValue( BackColors()->crEditor ) ) );
						if ( MX_GetAValue( ForeColors()->crAreaSeparator ) ) {
							rSep.left += stAll.i32PadNumbersLeftPx;
							rSep.right -= stAll.i32PadNumbersRightPx;
							::FillRect( _hDc, &rSep,
								lsw::CBase::BrushCache().Brush( MX_GetRgbValue( ForeColors()->crAreaSeparator ) ) );
						}

					}
				}
			}
			if ( rBlankArea.left < rBlankArea.right ) {
				COLORREF crBlankArea = MX_GetAValue( BackColors()->crEmptyArea ) ? MX_GetRgbValue( BackColors()->crEmptyArea ) : MX_GetRgbValue( BackColors()->crEditor );
				::FillRect( _hDc, &rBlankArea,
					lsw::CBase::BrushCache().Brush( crBlankArea ) );
			}
		}
		
		
		if ( stAll.bShowRuler ) {
			rTmp.bottom = rTmp.top + GetRulerHeightPx();
			if ( MX_GetAValue( BackColors()->crRulerFileBar ) ) {
				::FillRect( _hDc, &rTmp,
					lsw::CBase::BrushCache().Brush( MX_GetRgbValue( BackColors()->crRulerFileBar ) ) );
			}
			else {
				::FillRect( _hDc, &rTmp,
					lsw::CBase::BrushCache().Brush( MX_GetRgbValue( BackColors()->crEditor ) ) );
			}
			if ( MX_GetAValue( ForeColors()->crRulerLine ) ) {
				const int32_t iThisY = rTmp.bottom - 1;
				lsw::LSW_HPEN pPen( PS_SOLID, 1, ForeColors()->crRulerLine );
				lsw::LSW_SELECTOBJECT soPen( _hDc, pPen.hPen );
				::MoveToEx( _hDc, rTmp.left, iThisY, NULL );
				::LineTo( _hDc, rTmp.right, iThisY );
			}
			DrawRuler( _hDc, iX, 0, stAll.dfLeftNumbersFmt, stAll.dfRightNumbersFmt );
			iY += GetRulerHeightPx();
		}


		if ( stAll.bShowAddressGutter ) {
			rTmp = _rRect;
			rTmp.right = rTmp.left + iGutterW;
			if ( MX_GetAValue( BackColors()->crAddresses ) ) {
				::FillRect( _hDc, &rTmp,
					lsw::CBase::BrushCache().Brush( MX_GetRgbValue( BackColors()->crAddresses ) ) );
			}
			else {
				::FillRect( _hDc, &rTmp,
					lsw::CBase::BrushCache().Brush( MX_GetRgbValue( BackColors()->crEditor ) ) );
			}
			if ( MX_GetAValue( ForeColors()->crAddressSeparatorLine ) || m_sdScrollView[m_eaEditAs].ui64HPx != 0 ) {
				const int32_t iThisX = rTmp.right - 1;
				lsw::LSW_HPEN pPen( PS_SOLID, 1, ForeColors()->crAddressSeparatorLine );
				lsw::LSW_SELECTOBJECT soPen( _hDc, pPen.hPen );
				::MoveToEx( _hDc, iThisX, iY, NULL );
				::LineTo( _hDc, iThisX, rTmp.bottom );
			}
			DrawAddressGutter( _hDc, stAll.i32LeftAddrPadding, iY, m_sdScrollView[m_eaEditAs].i32PageLines + 1 );
		}
		return true;
	}

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
	void CHexEditorControl::DrawAddressGutter( HDC _hDc, int32_t _iXLeft, int32_t _iYTop, uint32_t _ui32LinesToDraw ) {
		const MX_STYLE & stAll = (*CurStyle());
		if ( !stAll.bShowAddressGutter || _ui32LinesToDraw == 0 || nullptr == Font() ) { return; }
		const MX_FONT_SET & fsFont = (*Font());

		const MX_ADDR_STYLE & asAddrStyle = stAll.daAddressStyle;

		// Select font and ensure glyph metrics.
		lsw::LSW_SELECTOBJECT soFont( _hDc, fsFont.fFont.hFont );
		EnsureAddrGlyphs( _hDc );
		const MX_ADDR_GLYPHS & agGlyphs = fsFont.agGlyphs;

		
		lsw::LSW_SETBKMODE sbmBkMode( _hDc, TRANSPARENT );
		lsw::LSW_SETTEXTCOLOR stcTextColor( _hDc, MX_GetRgbValue( ForeColors()->crLineNumbers ) );

		// Helpers: convert value to string in base, then apply options.
		auto ToBase = [&]( uint64_t _ui64V, uint32_t _ui32Base, bool _bLower ) -> std::wstring {
			if ( _ui64V == 0 ) { return L"0"; }
			std::wstring wsStr;
			while ( _ui64V ) {
				const uint32_t ui32D = static_cast<uint32_t>(_ui64V % _ui32Base);
				wchar_t wcC = 0;
				if ( ui32D < 10 ) { wcC = wchar_t( L'0' + ui32D ); }
				else { wcC = wchar_t( (_bLower ? (L'a' - 10) : (L'A' - 10)) + ui32D ); }
				wsStr.push_back( wcC );
				_ui64V /= _ui32Base;
			}
			std::reverse( wsStr.begin(), wsStr.end() );
			return wsStr;
		};

		auto InsertHexGroupingColons = [&]( std::wstring & _wsStr ) {
			if ( !asAddrStyle.bShowColonIn || _wsStr.size() <= 4 ) { return; }
			std::wstring wsGrouped;
			size_t sR = _wsStr.size();
			int32_t i32Cnt = 0;
			while ( sR-- ) {
				wsGrouped.push_back( _wsStr[sR] );
				if ( (++i32Cnt % 4) == 0 && sR != SIZE_MAX && sR != 0 ) {
					wsGrouped.push_back( L':' );
				}
			}
			std::reverse( wsGrouped.begin(), wsGrouped.end() );
			_wsStr.swap( wsGrouped );
		};

		// Per-line formatter: computes the address string shown for a given line index.
		auto FormatAddressForLine = [&]( uint64_t _ui64Line ) -> std::wstring {
			// Base byte index for the start of this line.
			const uint64_t ui64Bpr = stAll.uiBytesPerRow ? stAll.uiBytesPerRow : 16;
			const uint64_t ui64ByteBase = m_sdScrollView[m_eaEditAs].ui64StartAddress + _ui64Line * ui64Bpr;

			// Map to displayed scalar according to format.
			uint64_t v = 0ULL;
			bool bHex = false, bOct = false, bDec = false;
			switch( asAddrStyle.afFormat ) {
				case MX_AF_BYTES_HEX :		{ v = ui64ByteBase;			bHex = true; break; }
				case MX_AF_BYTES_DEC :		{ v = ui64ByteBase;			bDec = true; break; }
				case MX_AF_BYTES_OCT :		{ v = ui64ByteBase;			bOct = true; break; }
				case MX_AF_LINE_NUMBER :	{ v = _ui64Line + 1;		bDec = true; break; }
				case MX_AF_SHORT_HEX :		{ v = (ui64ByteBase >> 1);	bHex = true; break; }
				case MX_AF_SHORT_DEC :		{ v = (ui64ByteBase >> 1);	bDec = true; break; }
				default :					{ v = ui64ByteBase;			bHex = true; break; }
			}

			// Convert to string in the selected base.
			std::wstring wsCore;
			if ( bHex ) { wsCore = ToBase( v, 16, asAddrStyle.bLowercaseHex ); }
			else if ( bOct ) { wsCore = ToBase( v, 8, false ); }
			else { wsCore = ToBase( v, 10, false ); }

			// Minimum digits: hex/oct padded with '0'; decimal padded with spaces on the left.
			uint32_t ui32MinDigits = MinAddrDigits();
			if ( ui32MinDigits && wsCore.size() < ui32MinDigits ) {
				const size_t pad = ui32MinDigits - wsCore.size();
				if ( bHex ) {
					wsCore.insert( wsCore.begin(), pad, L'0' );
				}
				else {
					wsCore.insert( wsCore.begin(), pad, L' ' );
				}
			}

			// Internal grouping with ':' every 4 hex digits (after zero-padding).
			if ( bHex ) {
				InsertHexGroupingColons( wsCore );
			}

			// Trailing colon after the address if enabled.
			if ( asAddrStyle.bShowColonAfter ) {
				wsCore.push_back( L':' );
			}

			// Optional type specifier.
			if ( asAddrStyle.bShowTypeSpec ) {
				if ( bHex ) { wsCore.push_back( L'h' ); }
				else if ( bOct ) { wsCore.push_back( L'o' ); }
			}

			// Optional short suffix 'w' for Short addressing.
			if ( asAddrStyle.bUseShortSuffixW && (asAddrStyle.afFormat == MX_AF_SHORT_HEX || asAddrStyle.afFormat == MX_AF_SHORT_DEC) ) {
				wsCore.push_back( L'w' );
			}

			return wsCore;
		};

		// Line height.
		const int32_t iLineAdv = fsFont.iCharCy + stAll.i32LineSpacingPx;

		// First visible line (top) comes from style; draw consecutive lines.
		uint64_t ui64Line = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine;

		auto ui64MaxLines = TotalLines() - ui64Line;
		_ui32LinesToDraw = uint32_t( std::min<uint64_t>( _ui32LinesToDraw, ui64MaxLines ) );
		std::wstring wsTmp;
		for ( uint32_t I = 0; I < _ui32LinesToDraw; ++I, ++ui64Line ) {
			wsTmp = FormatAddressForLine( ui64Line );
			const int32_t iY = _iYTop + int32_t( I ) * iLineAdv;

			// Decimal is right-aligned by spaces we injected above; just draw at left edge.
			::TextOutW( _hDc, _iXLeft, iY + stAll.i32LineSpacingPx / 2, wsTmp.c_str(), static_cast<int32_t>(wsTmp.size()) );
		}
	}

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
	void CHexEditorControl::DrawRuler( HDC _hDc, int32_t _iXLeft, int32_t _iYTop, MX_DATA_FMT _dfLeftFmt, MX_DATA_FMT _dfRightFmt ) {
		const MX_STYLE & stAll = (*CurStyle());
		if ( !stAll.bShowRuler || !Font() ) { return; }
		const MX_FONT_SET & fsFont = (*Font());

		// Select font, set colors.
		lsw::LSW_SELECTOBJECT soFont( _hDc, fsFont.fFont.hFont );
		EnsureAddrGlyphs( _hDc );
		const int32_t iRulerCy = GetRulerHeightPx();

		lsw::LSW_SETBKMODE sbmBkMode( _hDc, TRANSPARENT );
		lsw::LSW_SETTEXTCOLOR stcTextColor( _hDc, MX_GetRgbValue( ForeColors()->crRulerFileBar ) );

		uint32_t ui32Digits = DigitCount( stAll.uiBytesPerRow - 1, 16 );

		// Helper: hex label for a group start index (byte index within row).
		auto HexLabel = []( uint32_t _ui32V, uint32_t _ui32Digits ) -> std::wstring {
			static const wchar_t * pwsS = L"0123456789ABCDEF";
			std::wstring wsOut;
			if MX_LIKELY( _ui32V < 16U && _ui32Digits == 1 ) { wsOut.push_back( pwsS[_ui32V] ); }
			else {
				// Wider rows: use more digits.
				uint32_t ui32N = _ui32V;
				std::wstring wsTmp;
				while ( ui32N ) { wsTmp.push_back( pwsS[ui32N&0xF] ); ui32N >>= 4; }
				while ( wsTmp.size() < _ui32Digits ) { wsTmp.push_back( L'0' ); }
				std::reverse( wsTmp.begin(), wsTmp.end() );
				wsOut.swap( wsTmp );
			}
			return wsOut;
		};

		
		// Draw one area (left or right) using its data format and group rules.
		auto DrawArea = [&]( MX_DATA_FMT _dfFmt, int32_t _iAreaXBase, uint32_t _ui32Digits ) {
			if ( _dfFmt == MX_DF_CHAR ) { _ui32Digits = 1; }
			// Number of groups in this area.
			const uint32_t ui32Bpr		= stAll.uiBytesPerRow;
			const uint32_t ui32GroupSz	= stAll.uiGroupSize ? stAll.uiGroupSize : 1;
			if ( ui32GroupSz == 0U ) { return; }

			const uint32_t ui32Groups = (ui32Bpr + ui32GroupSz - 1U) / ui32GroupSz;

			uint32_t ui32Stride = std::min<uint32_t>( _ui32Digits, 2 );
			if ( _dfFmt == MX_DF_CHAR && CurStyle()->csCharSet == CCharSets::MX_CS_UNICODE ) {
				ui32Stride = 2;
			}
			for ( uint32_t I = 0; I < ui32Bpr; I += ui32Stride ) {
				int32_t iGX = 0, iGW = 0;
				if ( !GetTextRectForIndex( _dfFmt, I, _iAreaXBase, iGX, iGW ) ) { break; }

				if ( stAll.bShowRulerArrows ) {
					// TODO: Get caret position.
					//ForeColors()->crRulerMarker
				}

				if ( stAll.bShowRulerLabels ) {
					// Label is the byte index of the first item in the group within the row.
					const uint32_t ui32ByteIndexInRow = I;
					std::wstring wsTmp = HexLabel( _dfFmt == MX_DF_CHAR ? (ui32ByteIndexInRow & 0x0F) : ui32ByteIndexInRow, _ui32Digits );

					// Center horizontally inside [iGX, iGW].
					SIZE sSize {};
					::GetTextExtentPoint32W( _hDc, wsTmp.c_str(), int32_t( wsTmp.size() ), &sSize );
					const int32_t iTextX = iGX + (iGW - sSize.cx) / 2;
					const int32_t iTextY = (_iYTop + (iRulerCy - fsFont.iCharCy) / 2);

					::TextOutW( _hDc, iTextX, iTextY, wsTmp.c_str(), int32_t( wsTmp.size() ) );
				}
				else {
					// Ruler Display Format == Hex.
					// Draw small centered lines with lengths: 3 1 1 1 2 1 1 1 [repeat].
					static const uint32_t ui32Lens[] = { 3, 1, 1, 1, 2, 1, 1, 1 };
					auto i32X = iGX + (iGW / 2);
					lsw::CHelpers::DrawLineSinglePixel_Inclusive( _hDc, i32X, _iYTop, i32X, _iYTop + ui32Lens[I%std::size( ui32Lens )], ForeColors()->crRulerFileBar );
				}
			}
		};

		{
			const int32_t i32LeftBase = _iXLeft;
			DrawArea( _dfLeftFmt, i32LeftBase, ui32Digits );
		}
		if ( stAll.bShowRightArea ) {
			const int32_t i32RightBase = _iXLeft + ComputeAreaWidthPx( _dfLeftFmt ) + stAll.i32PadNumbersLeftPx + stAll.i32PadBetweenNumbersAndTextPx + stAll.i32PadNumbersRightPx;
			DrawArea( _dfRightFmt, i32RightBase, ui32Digits );
		}
	}

	/**
	 * Draws an area at a given position.
	 * 
	 * \param _hDc Destination HDC.
	 * \param _iXLeft Pixel X where the scrollable content begins (after gutter; already includes horizontal scroll offset).
	 * \param _iYTop Pixel Y top where the area should be drawn.
	 * \param _dfFmt Format of the area.
	 * \param _bRightArea False for the left numbers area; true for the right text area.
	 * \param _ui32LinesToDraw The number of rows to draw (typically page lines).
	 * \param _bRightArea The right or left area.
	 * \param _bData The actual values at the addresses to render.
	 **/
	void CHexEditorControl::DrawArea( HDC _hDc, int32_t _iXLeft, int32_t _iYTop, MX_DATA_FMT _dfFmt, uint32_t _ui32LinesToDraw, const CHexEditorInterface::CBuffer &_bData, bool _bRightArea ) {
		const MX_STYLE & stAll = (*CurStyle());
		if ( _ui32LinesToDraw == 0 || nullptr == Font() ) { return; }
		const MX_FONT_SET & fsFont = (*Font());
		const MX_ADDR_STYLE & asAddrStyle = stAll.daAddressStyle;

		// Select font and ensure glyph metrics.
		lsw::LSW_SELECTOBJECT soFont( _hDc, fsFont.fFont.hFont );
		EnsureAddrGlyphs( _hDc );
		const MX_ADDR_GLYPHS & agGlyphs = fsFont.agGlyphs;

		const uint32_t ui32Bpr = stAll.uiBytesPerRow ? stAll.uiBytesPerRow : 16;

		// Line height.
		const int32_t iLineAdv = fsFont.iCharCy + stAll.i32LineSpacingPx;

		// Fill in the most likely color for each cell.
		COLORREF crBackColor = MX_GetRgbValue( BackColors()->crEditor );
		{
			lsw::LSW_RECT rRect;
			rRect.top = _iYTop;
			rRect.left = _iXLeft;
			rRect.right = rRect.left + ComputeAreaWidthPx( _dfFmt );
			rRect.bottom = rRect.top + iLineAdv * _ui32LinesToDraw;
			::FillRect( _hDc, &rRect,
				lsw::CBase::BrushCache().Brush( crBackColor ) );
		}

		// First visible line (top) comes from style; draw consecutive lines.
		uint64_t ui64Line = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine;
		uint64_t ui64StartAddress = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine * ui32Bpr;

		auto ui64MaxLines = TotalLines() - ui64Line;
		_ui32LinesToDraw = uint32_t( std::min<uint64_t>( _ui32LinesToDraw, ui64MaxLines ) );

		// The values to render.
		const uint8_t * pui8Data = _bData.data();
		size_t sDataSize = _bData.size();

		lsw::LSW_SETBKMODE sbmBkMode( _hDc, TRANSPARENT );
		lsw::LSW_SETTEXTCOLOR stcTextColor( _hDc, MX_GetRgbValue( ForeColors()->crEditor ) );
		std::wstring wsTmp;
		wchar_t wcDefChar = L'\0', wcHold;
		bool bHolding = false;
		int32_t i32CharSizeCounter = 0;
		uint32_t ui32Stride = (_dfFmt == MX_DF_CHAR && CurStyle()->csCharSet == CCharSets::MX_CS_UNICODE) ? 2 : 1;
		std::vector<COLORREF> vBgColors;
		try {
			vBgColors.resize( ui32Bpr );
		} catch ( ... ) {}
		lsw::LSW_RECT rCarect;
		bool bDrawCaret = false;
		COLORREF crCaretColor;
		for ( uint32_t I = 0; I < _ui32LinesToDraw; ++I, ui64Line += ui32Bpr ) {
			const int32_t iY = _iYTop + int32_t( I ) * iLineAdv;

			// Draw the background.  Try to reduce to as few calls as possible for a row.
			int32_t iL, iW;
			uint64_t ui64ThisAddr = ui64StartAddress + I * ui32Bpr;
			auto crColor = CellBgColor( ui64ThisAddr, pui8Data, sDataSize, _bRightArea );
			if ( 0 < vBgColors.size() ) {
				vBgColors[0] = crColor;
			}
			GetBackgrondRectForIndex( _dfFmt, 0, _iXLeft, iL, iW );
			for ( int32_t J = 1; J < ui32Bpr; ++J ) {
				int32_t iL2, iW2;
				GetBackgrondRectForIndex( _dfFmt, J, _iXLeft, iL2, iW2 );
				ui64ThisAddr = ui64StartAddress + I * ui32Bpr + J;
				auto crThisColor = CellBgColor( ui64ThisAddr, pui8Data + J, sDataSize - J, _bRightArea );
				if ( J < vBgColors.size() ) {
					vBgColors[J] = crThisColor;
				}
				if ( crThisColor != crColor || (iL + iW) == iL2 ) {
					if ( crBackColor != MX_GetRgbValue( crColor ) ) {
						lsw::LSW_RECT rRect;
						rRect.top = iY;
						rRect.left = iL;
						rRect.right = iL + iW;
						rRect.bottom = rRect.top + iLineAdv;
						::FillRect( _hDc, &rRect,
							lsw::CBase::BrushCache().Brush( MX_GetRgbValue( crColor ) ) );
					}
					iL = iL2, iW = iW2;
					crColor = crThisColor;
				}
				else {
					iW += iW2;
				}
			}
			{
				if ( crBackColor != MX_GetRgbValue( crColor ) ) {
					lsw::LSW_RECT rRect;
					rRect.top = iY;
					rRect.left = iL;
					rRect.right = iL + iW;
					rRect.bottom = rRect.top + iLineAdv;
					::FillRect( _hDc, &rRect,
						lsw::CBase::BrushCache().Brush( MX_GetRgbValue( crColor ) ) );
				}
			}

			for ( int32_t J = 0; J < ui32Bpr; J += ui32Stride ) {
				if ( J >= sDataSize ) { break; }

				ui64ThisAddr = ui64StartAddress + I * ui32Bpr + J;
				size_t sBuffSize = J < sDataSize ? sDataSize - J : 0;
				const uint8_t * pui8Value = sBuffSize ? &pui8Data[J] : reinterpret_cast<const uint8_t *>(&wcDefChar);
				uint8_t ui8Value = (*pui8Value);
				GetTextRectForIndex( _dfFmt, J, _iXLeft, iL, iW );
				
				if ( MX_DF_CHAR != _dfFmt && stAll.ui64DivisionSpacing ) {
					// Draw division lines.
					int32_t iL2, iW2;
					GetBackgrondRectForIndex( _dfFmt, J, _iXLeft, iL2, iW2 );
					lsw::LSW_RECT rRect;
					rRect.top = iY - 1;
					rRect.left = iL2;
					rRect.right = iL2 + iW2 + 1;
					rRect.bottom = iY + iLineAdv;

					uint64_t ui64ThisMod = ui64ThisAddr % stAll.ui64DivisionSpacing;
					uint64_t ui64ThisDiv = ui64ThisAddr / stAll.ui64DivisionSpacing;
					lsw::CHelpers::DrawRectOutlineMasked( _hDc, rRect,
						//(ui64ThisAddr >= ui32Bpr && ((ui64ThisAddr - ui32Bpr) % stAll.ui64DivisionSpacing != ui64ThisMod) ? lsw::CHelpers::LSW_RS_TOP : 0) |																						// Cell above this one.
						(((ui64ThisAddr + ui32Bpr) % stAll.ui64DivisionSpacing != ui64ThisMod && (ui64ThisAddr + ui32Bpr) / stAll.ui64DivisionSpacing != ui64ThisDiv) ? lsw::CHelpers::LSW_RS_BOTTOM : 0) |											// Cell below this one.
						((((ui64ThisAddr + 1) % stAll.ui64DivisionSpacing) == 0) ? lsw::CHelpers::LSW_RS_RIGHT : 0),																																// Cell to the right.
						MX_GetRgbValue( ForeColors()->crDivisionLines ) );
				}
				
				switch ( _dfFmt ) {
					case MX_DF_OCT : {
						try {
							wsTmp.resize( 3 );
							for ( size_t K = 0; K < 3; ++K ) {
								wsTmp[2-K] = (ui8Value & 0x07) + L'0';
								ui8Value >>= 3;
							}

							
						}
						catch ( ... ) {}
						break;
					}
					case MX_DF_BIN : {
						try {
							wsTmp.resize( 8 );
							for ( size_t K = 0; K < 8; ++K ) {
								wsTmp[7-K] = (ui8Value & 0x01) + L'0';
								ui8Value >>= 1;
							}
						}
						catch ( ... ) {}
						break;
					}
					case MX_DF_DEC : {
						try {
							wsTmp.resize( 3 );
							for ( size_t K = 0; K < 3; ++K ) {
								wsTmp[2-K] = (ui8Value % 10) + L'0';
								ui8Value /= 10;
							}
						}
						catch ( ... ) {}
						break;
					}
					case MX_DF_CHAR : {
						try {
							wsTmp.resize( 1 );
							i32CharSizeCounter -= ui32Stride;
							if ( i32CharSizeCounter <= 1 ) {
								if ( bHolding ) {
									wsTmp[0] = wcHold;
									bHolding = false;
								}
								else {
									i32CharSizeCounter = int32_t( CCharSets::m_csSets[stAll.csCharSet].pfDisplayable( pui8Value, sBuffSize, wcHold ) );
									if MX_UNLIKELY( (wcHold >= 0x80 && CCharSets::m_csSets[stAll.csCharSet].bHideOver127) ) {
										wcHold = L'.';
									}
									if ( i32CharSizeCounter > ui32Stride ) {
										wsTmp[0] = L'\0';
										bHolding = true;
									}
									else {
										wsTmp[0] = wcHold;
									}
								}
							}
							else { wsTmp[0] = L'\0'; }
						}
						catch ( ... ) {}
						break;
					}
					default : {
						try {
							wsTmp.resize( 2 );
							for ( size_t K = 0; K < 2; ++K ) {
								uint8_t uiThis = (ui8Value & 0x0F);
								if ( uiThis >= 10 ) {
									wsTmp[1-K] = uiThis + (L'A' - 10);
								}
								else {
									wsTmp[1-K] = uiThis + L'0';
								}
								ui8Value >>= 4;
							}
						}
						catch ( ... ) {}
						break;
					}
				}
				auto crThisColor = CellFgColor( ui64ThisAddr, pui8Data + J, sDataSize - J, _bRightArea );
				if ( crThisColor != MX_GetRgbValue( ForeColors()->crEditor ) ) {
					lsw::LSW_SETTEXTCOLOR stcTextColor2( _hDc, MX_GetRgbValue( crThisColor ) );
					::TextOutW( _hDc, iL, iY + stAll.i32LineSpacingPx / 2, wsTmp.c_str(), static_cast<int>(wsTmp.size()) );
				}
				else {
					::TextOutW( _hDc, iL, iY + stAll.i32LineSpacingPx / 2, wsTmp.c_str(), static_cast<int>(wsTmp.size()) );
				}

				// Caret.
				if ( ui64ThisAddr == m_sgSelGesture.ui64CaretAddr ) {
					rCarect.top = iY + iLineAdv - 1;
					rCarect.left = iL;
					rCarect.right = iL + iW;
					rCarect.bottom = rCarect.top + 2;

					auto pwAncestor = m_pwHexParent;
					bool bFocus = (pwAncestor && pwAncestor->GetFocus()) || m_bHasFocus || (m_pwParent && m_pwParent->GetFocus()) ||
						(m_pwParent && m_pwParent->Parent() && m_pwParent->Parent()->GetFocus());
					
					bool bRightCaret = m_sgSelGesture.bRightArea && stAll.bShowRightArea;
					if ( _bRightArea == bRightCaret ) {
						if ( !bFocus ) {
							COLORREF crColor = ForeColors()->crInactiveCaret;
							if ( J < vBgColors.size() ) {
								crColor = Mix( vBgColors[J], ForeColors()->crInactiveCaret );
							}
							bDrawCaret = true;
							crCaretColor = MX_GetRgbValue( crColor );
						}
						else if ( m_bCaretOn ) {
							bDrawCaret = true;
							crCaretColor = MX_GetRgbValue( ForeColors()->crCaret );
							
						}
					}
				}
			}
		
			if ( ui32Bpr >= sDataSize ) {
				pui8Data = nullptr;
				sDataSize = 0;
			}
			else {
				pui8Data += ui32Bpr;
				sDataSize -= ui32Bpr;
			}
		}
		if ( bDrawCaret ) {
			::FillRect( _hDc, &rCarect,
				lsw::CBase::BrushCache().Brush( crCaretColor ) );
		}
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
	COLORREF CHexEditorControl::CellFgColor( uint64_t _ui64Address, const uint8_t * _pui8Value, size_t _sSize, bool /*_bRightArea*/ ) {
		if ( !_sSize ) {
			return ForeColors()->crEditor;
		}
		const MX_STYLE & stAll = (*CurStyle());

		COLORREF crBaseColor = ForeColors()->crEditor;
		bool bDefaultBaseColor = true;

		if ( stAll.bHighlightNewLines && ((*_pui8Value) == '\r' || (*_pui8Value) == '\n') ) {
			crBaseColor = ForeColors()->crHighlightingHex;
			bDefaultBaseColor = false;
		}
		else if ( stAll.bHighlightAlphaNumeric && std::isalnum( (*_pui8Value) ) ) {
			crBaseColor = ForeColors()->crHighlightingHex;
			bDefaultBaseColor = false;
		}
		else if ( stAll.bHighlightControl && std::iscntrl( (*_pui8Value) ) ) {
			crBaseColor = ForeColors()->crHighlightingHex;
			bDefaultBaseColor = false;
		}
		else if ( stAll.bHighlightNonAscii && (*_pui8Value) >= 0x80 ) {
			crBaseColor = ForeColors()->crHighlightingHex;
			bDefaultBaseColor = false;
		}
		else if ( stAll.bHighlightZeros && (*_pui8Value) == 0x00 ) {
			crBaseColor = ForeColors()->crHighlightingHex;
			bDefaultBaseColor = false;
		}
		/*else if ( stAll.bHighlightPointers && (*_pui8Value) == 0x00 ) {
			crBaseColor = ForeColors()->crHighlightingHex;
			bDefaultBaseColor = false;
		}*/


		if ( !m_sSel.HasSelection() ) {
			if ( _ui64Address < Size() && m_sSel.IsSelected( _ui64Address, CurStyle()->uiBytesPerRow ) && (MX_GetAValue( ForeColors()->crSelected ) || bDefaultBaseColor) ) {
				crBaseColor = bDefaultBaseColor ? ForeColors()->crSelected : Mix( crBaseColor, ForeColors()->crSelected );
			}
		}

		return crBaseColor;
	}

	/**
	 * Gets the color of a hex/octal/binary cell by address.
	 * 
	 * \param _ui64Address The address of the data in the cell.
	 * \param _pui8Value The address of the data in the cell.
	 * \param _sSize The address of the data in the cell.
	 * \param _bRightArea The right area or the left area.
	 * \return Returns the background color for the given cell.
	 **/
	COLORREF CHexEditorControl::CellBgColor( uint64_t _ui64Address, const uint8_t * /*_pui8Value*/, size_t /*_sSize*/, bool _bRightArea ) {
		COLORREF crBaseColor;
		bool bDefaultBaseColor = false;
#define MX_CHECK_PROC( CAST, ENUM )																																	\
	if ( m_pheiTarget && m_pheiTarget->Type() == CHexEditorInterface::ENUM ) {																						\
		CAST * phepProc = static_cast<CAST *>(m_pheiTarget);																										\
		if ( !phepProc->Process().IsReadableByQuery( _ui64Address ) ) {																								\
			crBaseColor = BackColors()->crUnreadable; goto PostBase;																								\
		}																																							\
		if ( phepProc->Process().IsExecutableByQuery( _ui64Address ) ) {																							\
			crBaseColor = BackColors()->crExecutable; goto PostBase;																								\
		}																																							\
		if ( phepProc->Process().AddressIsInModule( _ui64Address ) ) {																								\
			crBaseColor = BackColors()->crStaticAddress; goto PostBase;																								\
		}																																							\
	}

		MX_CHECK_PROC( CHexEditorProcess, MX_HET_PROCESS )
		MX_CHECK_PROC( CHexEditorCurProcess, MX_HET_CUR_PROCESS )
#undef MX_CHECK_PROC
		

		bDefaultBaseColor = true;
		if ( MX_GetAValue( BackColors()->crAlternatingHexLines ) && ((_ui64Address / CurStyle()->uiBytesPerRow) & 1) == 0 ) {
			crBaseColor = BackColors()->crAlternatingHexLines;
			goto PostBase;
		}

		crBaseColor = BackColors()->crEditor;
	PostBase :
		// Mix colors.
		uint64_t ui64Caret = m_sgSelGesture.ui64CaretAddr;
		if ( !m_sSel.HasSelection() ) {
			if ( _bRightArea != (m_sgSelGesture.bRightArea && CurStyle()->bShowRightArea) && _ui64Address < Size() && ui64Caret == _ui64Address ) {
				crBaseColor = BackColors()->crHighlightByte;
			}
			else {
				
				if ( MX_GetAValue( BackColors()->crHighlightLineHexEditor ) && ui64Caret / CurStyle()->uiBytesPerRow == (_ui64Address / CurStyle()->uiBytesPerRow) ) {
					crBaseColor = Mix( crBaseColor, BackColors()->crHighlightLineHexEditor );
				}
			}
		}
		else {
			if ( _ui64Address < Size() && m_sSel.IsSelected( _ui64Address, CurStyle()->uiBytesPerRow ) && (MX_GetAValue( BackColors()->crSelected ) || bDefaultBaseColor) ) {
				crBaseColor = bDefaultBaseColor ? BackColors()->crSelected : Mix( crBaseColor, BackColors()->crSelected );
			}
		}

		return crBaseColor;
	}

	/**
	 * Gets the rectangle for a cell's text.
	 * 
	 * \param _ui64Address The address whose cell is to be gotten.
	 * \param _bRightArea If true, the cell is in the right area, otherwise it is in the left area.
	 * \return Returns the client rectable for the given text cell.
	 **/
	lsw::LSW_RECT CHexEditorControl::GetCellTextRect( uint64_t _ui64Address, bool _bRightArea ) {
		lsw::LSW_RECT rRet = lsw::LSW_RECT().Zero();
		const MX_STYLE & stAll = (*CurStyle());
		const int32_t iGutterW = ComputeAddressGutterWidthPx();
		rRet.left = iGutterW - m_sdScrollView[m_eaEditAs].ui64HPx;
		const MX_FONT_SET & fsFont = (*Font());
		rRet.top = GetRulerHeightPx() + ((fsFont.iCharCy + stAll.i32LineSpacingPx) * ((_ui64Address / stAll.uiBytesPerRow) - m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine));
		rRet.bottom = rRet.top + (fsFont.iCharCy + stAll.i32LineSpacingPx);

		int32_t iL, iW;
		_bRightArea = _bRightArea && stAll.bShowRightArea;
		if ( GetTextRectForIndex( _bRightArea ? stAll.dfRightNumbersFmt : stAll.dfLeftNumbersFmt,
			_ui64Address % stAll.uiBytesPerRow, int32_t( rRet.left ), iL, iW ) ) {
			if ( _bRightArea ) {
				rRet.left += ComputeAreaWidthPx( stAll.dfLeftNumbersFmt ) + stAll.i32PadNumbersLeftPx + stAll.i32PadBetweenNumbersAndTextPx + stAll.i32PadNumbersRightPx;
			}
			rRet.right = rRet.left + iW;
		}
		return rRet;
	}

	/**
	 * Computes the pixel width of the address gutter for the current options and font.
	 *
	 * \return Returns the pixel width of the address gutter. Returns 0 if the gutter is hidden.
	 */
	int32_t CHexEditorControl::ComputeAddressGutterWidthPx() {
		if ( !CurStyle()->bShowAddressGutter || !Font() ) { return 0; }
		MX_FONT_SET & fsFont = (*Font());

		const MX_STYLE & stAll = (*CurStyle());
		const MX_ADDR_STYLE & asAddrStyle = stAll.daAddressStyle;
		const uint64_t ui64FileSize = Size();
		int32_t iWidth = 0;
		{
			lsw::LSW_HDC hDc( Wnd() );
			{
				lsw::LSW_SELECTOBJECT soFont( hDc.hDc, fsFont.fFont.hFont );
				EnsureAddrGlyphs( hDc.hDc );
				const MX_ADDR_GLYPHS & agGlyphs = fsFont.agGlyphs;

				// File/page bound (last visible byte vs last file byte).
				const uint64_t ui64Bpr			= stAll.uiBytesPerRow ? stAll.uiBytesPerRow : 16;
				const uint64_t ui64FileLast		= (ui64FileSize ? (ui64FileSize - 1ULL) : 0ULL);
				const uint64_t ui64FileMax		= m_sdScrollView[m_eaEditAs].ui64StartAddress + ui64FileLast;

				const uint64_t ui64TopLine		= m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine;
				const uint64_t ui64LastLine		= ui64TopLine + (m_sdScrollView[m_eaEditAs].ui32VisibleLines ? (m_sdScrollView[m_eaEditAs].ui32VisibleLines - 1ULL) : 0ULL);
				const uint64_t ui64PageLast		= m_sdScrollView[m_eaEditAs].ui64StartAddress + (ui64LastLine * ui64Bpr + (ui64Bpr ? (ui64Bpr - 1ULL) : 0ULL));
				const uint64_t ui64BoundByte	= asAddrStyle.bMinimizeDigits ? std::min( ui64PageLast, ui64FileMax ) : ui64FileMax;

				// Map bound to displayed scalar by format.
				auto MapToDisplayed = [&]( uint64_t _ui64Byte ) -> uint64_t {
					switch ( asAddrStyle.afFormat ) {
						case MX_AF_BYTES_HEX : {}	MX_FALLTHROUGH
						case MX_AF_BYTES_DEC : {}	MX_FALLTHROUGH
						case MX_AF_BYTES_OCT : { return _ui64Byte; }
						case MX_AF_LINE_NUMBER : {
							if ( stAll.uiBytesPerRow == 0 ) { return 0ULL; }
							const uint64_t ui64Lines = (ui64FileSize + stAll.uiBytesPerRow - 1ULL) / stAll.uiBytesPerRow;
							return ui64Lines ? (ui64Lines - 1ULL) : 0ULL;
						}
						case MX_AF_SHORT_HEX : {}	MX_FALLTHROUGH
						case MX_AF_SHORT_DEC : { return (_ui64Byte >> 1); }
						default : { return _ui64Byte; }
					}
				};

				const uint64_t ui64Disp = MapToDisplayed( ui64BoundByte );

				// Base/digits.
				bool bHex = false, bOct = false, bDec = false;
				uint32_t ui32Base = 10;
				switch ( asAddrStyle.afFormat ) {
					case MX_AF_BYTES_HEX : {}	MX_FALLTHROUGH
					case MX_AF_SHORT_HEX : { bHex = true; ui32Base = 16; break; }
					case MX_AF_BYTES_OCT : { bOct = true; ui32Base = 8; break; }
					case MX_AF_BYTES_DEC : {}	MX_FALLTHROUGH
					case MX_AF_SHORT_DEC : {}	MX_FALLTHROUGH
					case MX_AF_LINE_NUMBER : {}	MX_FALLTHROUGH
					default : { bDec = true; ui32Base = 10; break; }
				}
				uint32_t ui32Digits = DigitCount( ui64Disp, ui32Base );
				uint32_t ui32MinDigits = MinAddrDigits();
				if ( ui32MinDigits ) { ui32Digits = std::max( ui32Digits, ui32MinDigits ); }

				iWidth = int32_t( ui32Digits ) * agGlyphs.iDigitMaxCx;

				// Internal grouping (HEX): ':' every 4 hex digits.
				if ( bHex && asAddrStyle.bShowColonIn && ui32Digits > 4 ) {
					const uint32_t ui32Groups = (ui32Digits - 1U) / 4U;
					iWidth += int32_t( ui32Groups ) * agGlyphs.i32ColonCx;
				}
				// Trailing colon.
				if ( asAddrStyle.bShowColonAfter ) { iWidth += agGlyphs.i32ColonCx; }
				// Type spec 'h'/'o'.
				if ( asAddrStyle.bShowTypeSpec ) {
					if ( bHex ) { iWidth += agGlyphs.i32SpecHexCx; }
					else if ( bOct ) { iWidth += agGlyphs.i32SpecOctCx; }
				}
				// Short suffix 'w'.
				if ( asAddrStyle.bUseShortSuffixW && (asAddrStyle.afFormat == MX_AF_SHORT_HEX || asAddrStyle.afFormat == MX_AF_SHORT_DEC) ) {
					iWidth += agGlyphs.i32ShortWcx;
				}
				// Optional left-space padding for right-aligned DEC up to MinDigits (if you do that).
				if ( bDec && ui32MinDigits && ui32Digits < ui32MinDigits ) {
					const uint32_t ui32Pads = ui32MinDigits - ui32Digits;
					iWidth += int32_t( ui32Pads ) * agGlyphs.i32SpaceCx;
				}
			}
		}

		return stAll.i32LeftAddrPadding + iWidth + stAll.i32PadAfterGutterPx;
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
	bool CHexEditorControl::GetTextRectForIndex(
		MX_DATA_FMT _dfDataFmt,
		uint32_t _ui32Index,
		int32_t _iXBase,
		int32_t &_iXLeft,
		int32_t &_iWidth ) const {
		const MX_STYLE & stStyle = (*CurStyle());
		const MX_FONT_SET & fsFont = (*Font());
		const MX_ADDR_GLYPHS & agGlyphs = fsFont.agGlyphs;

		// Bytes per row for both areas (ruler uses the row layout).
		//const uint32_t ui32Bpr = stStyle.uiBytesPerRow ? stStyle.uiBytesPerRow : 16;

		// Cell width by format (worst-case stable cell).
		auto CellWidthForFmt = [&]( MX_DATA_FMT _dfFmt ) -> int32_t {
			switch( _dfFmt ) {
				case MX_DF_HEX :	{ return 2 * agGlyphs.iDigitMaxCx; }																							// "FF"
				case MX_DF_DEC :	{ return 3 * agGlyphs.iDigitMaxCx; }																							// "255"
				case MX_DF_OCT :	{ return 3 * agGlyphs.iDigitMaxCx; }																							// "377"
				case MX_DF_BIN :	{ return 8 * agGlyphs.iDigitMaxCx; }																							// "11111111"
				case MX_DF_CHAR :	{ return CCharSets::m_csSets[stStyle.csCharSet].bHideOver127 ? agGlyphs.i32MaxAscii : agGlyphs.i32MaxAsciiAnsi; }				// Printable cell.
				default :			{ return 2 * agGlyphs.iDigitMaxCx; }
			}
		};

		// Choose per-area layout knobs.
		const uint32_t ui32Bpr		= stStyle.uiBytesPerRow ? stStyle.uiBytesPerRow : 16;
		const uint32_t ui32GroupSz	= (stStyle.uiGroupSize && _dfDataFmt != MX_DF_CHAR) ? stStyle.uiGroupSize : 1;

		const int32_t iCellCx			= CellWidthForFmt( _dfDataFmt );
		const int32_t iSpaceB			= int32_t( _dfDataFmt == MX_DF_CHAR ? 1 : stStyle.uiSpacesBetweenBytes ) * agGlyphs.i32SpaceCx;

		// Prepend area-local leading pad (for left numbers or right text; we baked pads into our area widths).
		int32_t iX = _iXBase;
		if ( _dfDataFmt == MX_DF_CHAR ) {
			iX += (iCellCx * _ui32Index);
			_iXLeft = iX;
		}
		else {
			iX += (iCellCx * _ui32Index) + (((_ui32Index / ui32GroupSz)) * iSpaceB);
			_iXLeft = iX + (agGlyphs.i32SpaceCx / 2);
		}

		
		_iWidth = iCellCx;
		return true;
	}

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
	bool CHexEditorControl::GetBackgrondRectForIndex(
		MX_DATA_FMT _dfDataFmt,
		uint32_t _ui32Index,
		int32_t _iXBase,
		int32_t &_iXLeft,
		int32_t &_iWidth ) const {
		if MX_UNLIKELY( !GetTextRectForIndex( _dfDataFmt, _ui32Index, _iXBase, _iXLeft, _iWidth ) ) { return false; }
		if ( _dfDataFmt == MX_DF_CHAR ) { return true; }

		const MX_STYLE & stStyle = (*CurStyle());
		const MX_ADDR_GLYPHS & agGlyphs = Font()->agGlyphs;

		const uint32_t ui32GroupSz	= (stStyle.uiGroupSize && _dfDataFmt != MX_DF_CHAR) ? stStyle.uiGroupSize : 1;
		const uint32_t ui32Mod = _ui32Index % ui32GroupSz;
		const int32_t i32Adj = agGlyphs.i32SpaceCx / 2;
		// If at the group start, increase size to the left.
		if ( ui32Mod == 0 ) {
			_iXLeft -= i32Adj;
			_iWidth += i32Adj;
		}
		// If at the end of a group, increase size to the right.
		if ( ui32Mod == ui32GroupSz - 1 ) {
			_iWidth += i32Adj;
		}
		return true;
	}

	/**
	 * Computes the pixel width of the left numeric (hex/dec/oct/bin/char) column for one row.
	 *
	 * \param _dfDataFmt Data format of the area (HEX/DEC/OCT/BIN/CHAR).
	 * \return Returns the pixel width of the left numbers block; 0 if hidden.
	 */
	int32_t CHexEditorControl::ComputeAreaWidthPx( MX_DATA_FMT _dfDataFmt ) {
		const MX_STYLE & stStyle = (*CurStyle());
		if ( !stStyle.bShowLeftNumbers || !Font() ) { return 0; }
		const MX_FONT_SET & fsFont = (*Font());
			
		int32_t iCx = 0;
		{
			lsw::LSW_HDC hDc( Wnd() );
			{
				lsw::LSW_SELECTOBJECT soFont( hDc.hDc, fsFont.fFont.hFont );
				EnsureAddrGlyphs( hDc.hDc );
				const MX_ADDR_GLYPHS & agGlyphs = fsFont.agGlyphs;

				auto CellWidthForFmt = [&]( MX_DATA_FMT _dfDataFmt ) -> int32_t {
					switch ( _dfDataFmt ) {
						case MX_DF_HEX :	{ return 2 * agGlyphs.iDigitMaxCx; }																								// "FF"
						case MX_DF_DEC :	{ return 3 * agGlyphs.iDigitMaxCx; }																								// "255"
						case MX_DF_OCT :	{ return 3 * agGlyphs.iDigitMaxCx; }																								// "377"
						case MX_DF_BIN :	{ return 8 * agGlyphs.iDigitMaxCx; }																								// "11111111"
						case MX_DF_CHAR :	{ return CCharSets::m_csSets[stStyle.csCharSet].bHideOver127 ? agGlyphs.i32MaxAscii : agGlyphs.i32MaxAsciiAnsi; }					// Fixed cell.
						default :			{ return 2 * agGlyphs.iDigitMaxCx; }
					}
				};

				const uint32_t ui32Bpr		= stStyle.uiBytesPerRow ? stStyle.uiBytesPerRow : 16;
				const uint32_t ui32GroupSz	= (stStyle.uiGroupSize && _dfDataFmt != MX_DF_CHAR) ? stStyle.uiGroupSize : 1;		// Items per group.

				const int32_t iCellCx			= CellWidthForFmt( _dfDataFmt );
				const int32_t iSpaceB			= int32_t( _dfDataFmt == MX_DF_CHAR ? 1 : stStyle.uiSpacesBetweenBytes ) * agGlyphs.i32SpaceCx;

				int32_t iTotal = 0;//stStyle.i32PadNumbersLeftPx;

				if ( _dfDataFmt == MX_DF_CHAR ) {
					iTotal += (iCellCx * ui32Bpr);
				}
				else {
					iTotal += (iCellCx * ui32Bpr) + (((ui32Bpr / ui32GroupSz) - 0) * iSpaceB);
				}

				//iTotal += stStyle.i32PadNumbersRightPx;

				iCx = iTotal;// + (iCellCx / 1);
			}
		}
		return iCx;
	}

	// Gets the total number of lines based on which view type is active.
	uint64_t CHexEditorControl::TotalLines() const {
		if ( IsFixedRowLength() ) { return TotalLines_FixedWidth(); }
		return 0;
	}

	// Updates the scrollbars.
	void CHexEditorControl::UpdateScrollbars() {
		// Compute visible page sizes (lines / columns-on-screen).
		int32_t iClientH = int32_t( ClientRect().Height() );

		const int32_t iLineAdv = LineAdvanceCy();
		m_sdScrollView[m_eaEditAs].i32PageLines = (iLineAdv ? (iClientH / iLineAdv) : 0);
		if ( m_sdScrollView[m_eaEditAs].i32PageLines < 1 ) { m_sdScrollView[m_eaEditAs].i32PageLines = 1; }

		m_sdScrollView[m_eaEditAs].i32PageCols = (Font()->iCharCx ? (iClientH / Font()->iCharCx) : 0);
		if ( m_sdScrollView[m_eaEditAs].i32PageCols < 1 ) { m_sdScrollView[m_eaEditAs].i32PageCols = 1; }

		// == Vertical (lines; virtual units are line indices 0..N-1).
		const uint64_t ui64Lines = TotalLines();
		const uint64_t ui64MaxV  = ui64Lines ? (ui64Lines - 1ULL) : 0ULL;	// inclusive; allow last line at top
		if ( m_sdScrollView[m_eaEditAs].ui64VPos > ui64MaxV ) { m_sdScrollView[m_eaEditAs].ui64VPos = ui64MaxV; }

		// Keep style in sync.
		m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine = m_sdScrollView[m_eaEditAs].ui64VPos;
		m_sdScrollView[m_eaEditAs].ui32VisibleLines     = static_cast<uint32_t>(m_sdScrollView[m_eaEditAs].i32PageLines);

		// Physical page = 1 so the thumb can reach the end.
		lsw::CBase::SetScrollInfo64( m_hWnd, SB_VERT, SIF_PAGE | SIF_RANGE | SIF_POS,
			ui64MaxV, m_sdScrollView[m_eaEditAs].ui64VPos, 1, TRUE );

		// == Horizontal (pixels; virtual units are pixel offsets 0..contentPx-1).
		const uint64_t ui64ContentPx = static_cast<uint64_t>(TotalContentWidthPx());
		const uint64_t ui64MaxH      = ui64ContentPx ? (ui64ContentPx - 1ULL) : 0ULL; // inclusive; allow last pixel at left
		if ( m_sdScrollView[m_eaEditAs].ui64HPx > ui64MaxH ) { m_sdScrollView[m_eaEditAs].ui64HPx = ui64MaxH; }

		// Physical page = 1 for the same reason; wefll use client width for paging behavior.
		lsw::CBase::SetScrollInfo64( m_hWnd, SB_HORZ, SIF_PAGE | SIF_RANGE | SIF_POS,
			ui64MaxH, m_sdScrollView[m_eaEditAs].ui64HPx, 1, TRUE );
	}

	/**
	 * Begin a selection gesture (mouse down). Applies Column/Normal rules and Shift/Ctrl modifiers.
	 *
	 * \param _ptClient Client-space mouse location.
	 * \param _bShift True if Shift is pressed.
	 * \param _bCtrl True if Ctrl is pressed.
	 * \param _bCtrlShift True if Ctrl+Shift are pressed.
	 */
	void CHexEditorControl::SelectionBeginGesture( const POINT &_ptClient, bool _bShift, bool _bCtrl, bool _bCtrlShift ) {
		bool bRightArea = false;
		uint64_t ui64Addr = 0;
		if ( !PointToAddress( _ptClient, ui64Addr, bRightArea ) ) { return; }
		

		m_sgSelGesture.bRightArea = bRightArea;

		if ( _bCtrlShift && m_sSel.bHas && m_sSel.smMode == LSN_SM_NORMAL && NormalSelectionIsSingleRow() ) {
			//m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64AnchorAddr;
			ConvertSingleRowNormalToColumn();
			SelectionUpdateGesture( _ptClient, _bShift, _bCtrl, _bCtrlShift );
			return;
		}

		if ( _bShift && m_sSel.bHas && m_sSel.smMode == LSN_SM_NORMAL ) {
			//m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64AnchorAddr;
			InitShiftExtendNormal( ui64Addr, _ptClient, _bShift, _bCtrl, _bCtrlShift );
			return;
		}
		if ( _bShift && m_sSel.bHas && m_sSel.smMode == LSN_SM_COLUMN ) {
			//m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64AnchorAddr;
			InitShiftExtendColumn( ui64Addr, _ptClient, _bShift, _bCtrl, _bCtrlShift );
			return;
		}
		if ( _bShift ) {
			//m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64AnchorAddr;
			if ( m_sgSelGesture.smCurrent == LSN_SM_NORMAL ) {
				if ( ui64Addr >= m_sgSelGesture.ui64MouseAnchorAddr ) {
					m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr;
					SetCaretAddr( ui64Addr + 1, 0, true, false );
				}
				else {
					m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr + 1;
					SetCaretAddr( ui64Addr, 0, true, false );
				}
			}
			else {
				if ( ui64Addr % CurStyle()->uiBytesPerRow >= m_sgSelGesture.ui64MouseAnchorAddr % CurStyle()->uiBytesPerRow ) {
					m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr;
					SetCaretAddr( ui64Addr + 1, 0, true, false );
				}
				else {
					m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr + 1;
					SetCaretAddr( ui64Addr, 0, true, false );
				}
			}
			m_sgSelGesture.bPendingThreshold = false;
			m_sgSelGesture.bSelecting = true;
			return;
		}

		m_sgSelGesture.bSelecting = false;
		m_sgSelGesture.bPendingThreshold = true;
		m_sgSelGesture.ptDown = _ptClient;
		m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64CaretAddr = ui64Addr;
		

		m_sgSelGesture.smCurrent = _bCtrl ? LSN_SM_COLUMN : (CurStyle()->bSelectColumnMode ? LSN_SM_COLUMN : LSN_SM_NORMAL);

		m_sSel.bHas = false;
	}

	/**
	 * Update selection during mouse move. Starts a new selection after threshold when appropriate.
	 *
	 * \param _ptClient Client-space mouse location.
	 * \param _bShift True if Shift is pressed.
	 * \param _bCtrl True if Ctrl is pressed.
	 * \param _bCtrlShift True if Ctrl+Shift are pressed.
	 */
	void CHexEditorControl::SelectionUpdateGesture( const POINT &_ptClient, bool _bShift, bool _bCtrl, bool _bCtrlShift ) {
		if ( !m_sgSelGesture.bSelecting && !m_sgSelGesture.bPendingThreshold ) { return; }
		bool bRightArea = false;
		uint64_t ui64Addr = 0;
		if ( !PointToAddress( _ptClient, ui64Addr, bRightArea ) ) {
			return;
		}

		//m_sgSelGesture.ui64CaretAddr = ui64Addr;

		if ( m_sgSelGesture.bPendingThreshold ) {
			const int32_t iDx = std::abs( _ptClient.x - m_sgSelGesture.ptDown.x );
			const int32_t iDy = std::abs( _ptClient.y - m_sgSelGesture.ptDown.y );
			const int32_t i32Range = 4;
			if ( iDx >= i32Range || iDy >= i32Range ) {
				m_sgSelGesture.bPendingThreshold = false;
				m_sgSelGesture.bSelecting = true;
			}
		}


		if ( !m_sgSelGesture.bSelecting ) { return; }

		if ( m_sgSelGesture.smCurrent == LSN_SM_NORMAL ) {
			if ( ui64Addr >= m_sgSelGesture.ui64MouseAnchorAddr ) {
				m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr;
				SetCaretAddr( ui64Addr + 1, 0, true, false );
			}
			else {
				m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr + 1;
				SetCaretAddr( ui64Addr, 0, true, false );
			}
		}
		else {
			if ( ui64Addr % CurStyle()->uiBytesPerRow >= m_sgSelGesture.ui64MouseAnchorAddr % CurStyle()->uiBytesPerRow ) {
				m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr;
				SetCaretAddr( ui64Addr + 1, 0, true, false );
			}
			else {
				m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr + 1;
				SetCaretAddr( ui64Addr, 0, true, false );
			}
		}
	}

	/**
	 * End a selection gesture (mouse up). Collapses zero-length selections created by clicks without drag.
	 */
	void CHexEditorControl::SelectionEndGesture() {
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
	void CHexEditorControl::InitShiftExtendColumn( uint64_t _ui64ClickAddr,
		const POINT &_ptClient,
		bool _bShift,
		bool _bCtrl,
		bool _bCtrlShift ) {

		if ( !m_sSel.bHas || m_sSel.smMode != LSN_SM_COLUMN ) { return; }

		const uint32_t ui32Bpr = CurStyle()->uiBytesPerRow;
		if ( ui32Bpr == 0 ) { return; }

		// Current column selection corners under this BPR.
		const uint64_t ui64AnchorAddr = m_sSel.sc.ui64AnchorAddr;		// Top-left.
		const uint64_t ui64AnchorRow  = ui64AnchorAddr / ui32Bpr;
		const uint32_t ui32AnchorCol  = static_cast<uint32_t>(ui64AnchorAddr % ui32Bpr);

		const uint64_t ui64BrRow = ui64AnchorRow + m_sSel.sc.ui64Lines;
		const uint32_t ui32BrCol = ui32AnchorCol + m_sSel.sc.ui32Cols;
		const uint64_t ui64BrAddr = ui64BrRow * ui32Bpr + ui32BrCol;		// Bottom-right.

		// Click position in grid space.
		const uint64_t ui64ClickRow = _ui64ClickAddr / ui32Bpr;
		const uint32_t ui32ClickCol = static_cast<uint32_t>(_ui64ClickAddr % ui32Bpr);

		// Distances to TL and BR in grid space (row distance weighted by bytes-per-row).
		const int64_t i64DrTl = static_cast<int64_t>(ui64AnchorRow) - static_cast<int64_t>(ui64ClickRow);
		const int64_t i64DcTl = static_cast<int64_t>(ui32AnchorCol) - static_cast<int64_t>(ui32ClickCol);
		const int64_t i64DrBr = static_cast<int64_t>(ui64BrRow)     - static_cast<int64_t>(ui64ClickRow);
		const int64_t i64DcBr = static_cast<int64_t>(ui32BrCol)     - static_cast<int64_t>(ui32ClickCol);

		const uint64_t ui64DistTl = static_cast<uint64_t>(std::llabs( i64DrTl )) * ui32Bpr +
			static_cast<uint64_t>(std::llabs( i64DcTl ));
		const uint64_t ui64DistBr = static_cast<uint64_t>(std::llabs( i64DrBr )) * ui32Bpr +
			static_cast<uint64_t>(std::llabs( i64DcBr ));

		// If click is closer to TL, move TL (fix BR). Otherwise move BR (fix TL).
		const uint64_t ui64FixedAddr = (ui64DistTl <= ui64DistBr) ? ui64BrAddr : ui64AnchorAddr;

		m_sgSelGesture.ptDown = _ptClient;
		m_sgSelGesture.bPendingThreshold = false;		// Already selecting.
		m_sgSelGesture.bSelecting = true;
		m_sgSelGesture.smCurrent = LSN_SM_COLUMN;

		m_sgSelGesture.ui64AnchorAddr = ui64FixedAddr;
		m_sgSelGesture.ui64CaretAddr  = _ui64ClickAddr;

		// Let the common update logic rebuild m_sSel.sc from anchor/caret.
		SelectionUpdateGesture( _ptClient, _bShift, _bCtrl, _bCtrlShift );
	}

	/**
	 * \brief Convert the current single-row Normal selection into a Column selection.
	 *
	 * Precondition: NormalSelectionIsSingleRow() is true.
	 */
	void CHexEditorControl::ConvertSingleRowNormalToColumn() {
		if ( !m_sSel.bHas || m_sSel.smMode != LSN_SM_NORMAL ) {
			return;
		}

		const uint32_t ui32Bpr = CurStyle()->uiBytesPerRow;
		if ( ui32Bpr == 0 ) { return; }

		uint64_t ui64Start = m_sSel.sn.ui64Start;
		uint64_t ui64End   = m_sSel.sn.ui64End;
		if ( ui64Start > ui64End ) { std::swap( ui64Start, ui64End ); }

		const uint64_t ui64Row     = ui64Start / ui32Bpr;
		const uint32_t ui32ColBeg  = static_cast<uint32_t>(ui64Start % ui32Bpr);
		const uint32_t ui32ColEnd  = static_cast<uint32_t>(ui64End   % ui32Bpr);
		const uint32_t ui32Cols    = (ui32ColEnd - ui32ColBeg) + 1;

		m_sSel.smMode = LSN_SM_COLUMN;
		m_sSel.sc.ui64AnchorAddr = ui64Row * ui32Bpr + ui32ColBeg;
		m_sSel.sc.ui32Cols       = ui32Cols;
		m_sSel.sc.ui64Lines      = 1;

		m_sgSelGesture.smCurrent = LSN_SM_COLUMN;
	}

	/**
	 * \brief Returns true if the active Normal selection lies entirely on one logical row.
	 */
	bool CHexEditorControl::NormalSelectionIsSingleRow() const {
		if ( !m_sSel.bHas || m_sSel.smMode != LSN_SM_NORMAL ) { return false; }

		const uint32_t ui32Bpr = CurStyle()->uiBytesPerRow;
		if ( ui32Bpr == 0 ) { return false; }

		uint64_t ui64Start = m_sSel.sn.ui64Start;
		uint64_t ui64End   = m_sSel.sn.ui64End;
		if ( ui64Start > ui64End ) { std::swap( ui64Start, ui64End ); }

		const uint64_t ui64RowStart = ui64Start / ui32Bpr;
		const uint64_t ui64RowEnd   = ui64End   / ui32Bpr;
		return ui64RowStart == ui64RowEnd;
	}

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
	bool CHexEditorControl::FindWordBoundary( uint64_t _ui64StartAddr,
		bool _bForward,
		uint64_t _ui64EndAddr,
		uint64_t &_ui64FoundAddr ) const {

		_ui64FoundAddr = 0;

		if ( !m_pheiTarget ) { return false; }

		const uint64_t ui64TotalBytes = Size();
		if ( ui64TotalBytes == 0 ) {
			return false;
		}
		if ( _ui64StartAddr >= ui64TotalBytes ) {
			return false;
		}

		// Clamp endpoint into file range.
		if ( _ui64EndAddr >= ui64TotalBytes ) {
			_ui64EndAddr = ui64TotalBytes - 1;
		}

		// If there is no range to scan in the given direction, bail out.
		if ( _bForward ) {
			if ( _ui64EndAddr <= _ui64StartAddr ) { return false; }
		}
		else {
			if ( _ui64EndAddr >= _ui64StartAddr ) { return false; }
		}

		// Read classification at the starting address.
		try {
			bool bFirst = false;

			CHexEditorInterface::CBuffer bStart;
			bStart.resize( 1 );
			if ( !m_pheiTarget->Read( _ui64StartAddr, bStart, 1 ) ) {
				return false;
			}
			/*const uint8_t * pui8Start = static_cast<const uint8_t *>(bStart.data());
			if ( !pui8Start ) { return false; }*/
			wchar_t wcHold = bStart[0];
			if ( int32_t( CCharSets::m_csSets[CurStyle()->csCharSet].pfDisplayable( bStart.data(), 1, wcHold ) ) == 1 ) {}
			else { wcHold = L'\0'; }
			const bool bStartIsWord = ee::CExpEval::IsIdentifier( char( wcHold ), bFirst );

			//const bool bStartIsWord = ee::CExpEval::IsIdentifier( pui8Start[0], bFirst );

			// Shared buffer for chunked reads.
			const size_t sChunkSize = 4096 * 100;
			CHexEditorInterface::CBuffer bChunk;
			bChunk.resize( sChunkSize );
			uint8_t * pui8Chunk = static_cast<uint8_t *>(bChunk.data());
			if ( !pui8Chunk ) { return false; }

			if ( _bForward ) {
				// Forward: scan from start+1 up to end.
				uint64_t ui64Cur = _ui64StartAddr + 1;
				while ( ui64Cur <= _ui64EndAddr ) {
					const uint64_t ui64BytesRemaining = (_ui64EndAddr - ui64Cur) + 1;
					const size_t sThisChunk = static_cast<size_t>(std::min<uint64_t>( ui64BytesRemaining, static_cast<uint64_t>(sChunkSize) ));

					if ( sThisChunk == 0 ) { break; }

					if ( !m_pheiTarget->Read( ui64Cur, bChunk, sThisChunk ) ) { return false; }

					for ( size_t sIdx = 0; sIdx < sThisChunk; ++sIdx ) {
						//const uint8_t ui8Byte = pui8Chunk[sIdx];
						wchar_t wcHold = pui8Chunk[sIdx];
						if ( int32_t( CCharSets::m_csSets[CurStyle()->csCharSet].pfDisplayable( &pui8Chunk[sIdx], 1, wcHold ) ) == 1 ) {}
						else { wcHold = L'\0'; }
						const bool bIsWord = ee::CExpEval::IsIdentifier( char( wcHold ), bFirst );
						if ( bIsWord != bStartIsWord ) {
							_ui64FoundAddr = ui64Cur + static_cast<uint64_t>(sIdx);
							return true;
						}
					}

					ui64Cur += sThisChunk;
				}
			}
			else {
				// Backward: scan from start-1 down to end.
				uint64_t ui64CurEnd = _ui64StartAddr - 1;
				while ( ui64CurEnd >= _ui64EndAddr ) {
					const uint64_t ui64Span = ( ui64CurEnd - _ui64EndAddr ) + 1;
					const size_t sThisChunk = static_cast<size_t>(std::min<uint64_t>( ui64Span, static_cast<uint64_t>(sChunkSize) ));

					const uint64_t ui64ChunkStart = ui64CurEnd - static_cast<uint64_t>(sThisChunk - 1);

					if ( !m_pheiTarget->Read( ui64ChunkStart, bChunk, sThisChunk ) ) { return false; }

					// Scan this chunk backwards in memory.
					for ( size_t sIdx = 0; sIdx < sThisChunk; ++sIdx ) {
						const size_t sRevIdx = sThisChunk - 1 - sIdx;
						wchar_t wcHold = pui8Chunk[sRevIdx];
						if ( int32_t( CCharSets::m_csSets[CurStyle()->csCharSet].pfDisplayable( &pui8Chunk[sRevIdx], 1, wcHold ) ) == 1 ) {}
						else { wcHold = L'\0'; }
						const bool bIsWord = ee::CExpEval::IsIdentifier( char( wcHold ), bFirst );
						/*const uint8_t ui8Byte = pui8Chunk[sRevIdx];
						const bool bIsWord = ee::CExpEval::IsIdentifier( ui8Byte, bFirst );*/
						if ( bIsWord != bStartIsWord ) {
							_ui64FoundAddr = ui64ChunkStart + static_cast<uint64_t>(sRevIdx);
							return true;
						}
					}

					// Move to the previous chunk.
					if ( ui64ChunkStart == 0 ) { break; }
					ui64CurEnd = ui64ChunkStart - 1;
					if ( ui64CurEnd < _ui64EndAddr ) { break; }
				}
			}
		}
		catch ( ... ) { return false; }

		return false;
	}

}	// namespace mx

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
		auto psbStatus = reinterpret_cast<CDeusHexMachinaWindow *>(m_pwHexParent)->StatusBar();
		auto ui64Size = Size();
		m_sgSelGesture.ui64CaretAddr = std::min( _ui64Addr, ui64Size );
		if ( m_sgSelGesture.ui64CaretAddr == ui64Size ) {
			_i32SubCaret = 0;
		}
		m_sgSelGesture.i32CaretIdx = _bUpdateSelection ? 0 : _i32SubCaret;

		CSecureWString swsStatus;
		try {
			if ( _bUpdateSelection ) {
				m_sSel.bHas = true;

				if ( m_sgSelGesture.smCurrent == MX_SM_NORMAL ) {
					m_sSel.smMode = MX_SM_NORMAL;
					m_sSel.sn.ui64Start = std::min( m_sgSelGesture.ui64AnchorAddr, m_sgSelGesture.ui64CaretAddr );
					m_sSel.sn.ui64End   = std::max( m_sgSelGesture.ui64AnchorAddr, m_sgSelGesture.ui64CaretAddr );

					swsStatus = _DEC_WS_2644E6E7_Selected__;
					uint64_t ui64SelLen = m_sSel.sn.ui64End - m_sSel.sn.ui64Start;
					swsStatus += std::to_wstring( ui64SelLen );
					if ( ui64SelLen >= 0xA ) {
						swsStatus.push_back( L' ' );
						swsStatus.push_back( L'[' );
						CUtilities::ToHex( ui64SelLen, swsStatus, 1 );
						swsStatus.push_back( L']' );
					}
					if ( ui64SelLen == 1 ) {
						swsStatus += _DEC_WS_BB679D6B__byte;
					}
					else {
						swsStatus += _DEC_WS_C1D51046__bytes;
					}
					swsStatus += _DEC_WS_E9A13A63___Range__;
					swsStatus += std::to_wstring( m_sSel.sn.ui64Start );
					if ( m_sSel.sn.ui64Start >= 0xA ) {
						swsStatus.push_back( L' ' );
						swsStatus.push_back( L'[' );
						CUtilities::ToHex( m_sSel.sn.ui64Start, swsStatus, 1 );
						swsStatus.push_back( L']' );
					}

					swsStatus += _DEC_WS_0B85E70E__to_;
					swsStatus += std::to_wstring( m_sSel.sn.ui64End - 1 );
					if ( m_sSel.sn.ui64End - 1 >= 0xA ) {
						swsStatus.push_back( L' ' );
						swsStatus.push_back( L'[' );
						CUtilities::ToHex( m_sSel.sn.ui64End - 1, swsStatus, 1 );
						swsStatus.push_back( L']' );
					}
					swsStatus.push_back( L')' );
				}
				else {
					const uint32_t ui32Bpr	= CurStyle()->uiBytesPerRow;
					const uint64_t ui64A	= m_sgSelGesture.ui64AnchorAddr;
					const uint64_t ui64B	= m_sgSelGesture.ui64CaretAddr - 1;

					const uint64_t ui64RowA = ui64A / ui32Bpr;
					const uint32_t ui32ColA = static_cast<uint32_t>(ui64A % ui32Bpr);
					const uint64_t ui64RowB = ui64B / ui32Bpr;
					const uint32_t ui32ColB = static_cast<uint32_t>(ui64B % ui32Bpr) + 1;

					const uint64_t ui64TlRow = std::min( ui64RowA, ui64RowB );
					const uint32_t ui32TlCol = std::min( ui32ColA, ui32ColB );
					const uint64_t ui64BrRow = std::max( ui64RowA, ui64RowB );
					const uint32_t ui32BrCol = std::max( ui32ColA, ui32ColB );

					m_sSel.smMode = MX_SM_COLUMN;
					m_sSel.sc.ui64AnchorAddr = ui64TlRow * ui32Bpr + ui32TlCol;
					m_sSel.sc.ui32Cols       = (ui32BrCol - ui32TlCol);
					m_sSel.sc.ui64Lines      = (ui64BrRow - ui64TlRow);

					swsStatus = _DEC_WS_2644E6E7_Selected__;
					swsStatus += std::to_wstring( m_sSel.sc.ui64Lines + 1 );
					swsStatus += _DEC_WS_66452C3E___lines__;
					swsStatus += std::to_wstring( m_sSel.sc.ui32Cols );
					swsStatus += _DEC_WS_6DBF08C4__cols;
				}
			}
			else {
				ClearSelection();
				m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = _ui64Addr;
				m_sgSelGesture.smCurrent = CurStyle()->bSelectColumnMode ? MX_SM_COLUMN : MX_SM_NORMAL;
			}

			if ( psbStatus ) { psbStatus->SetTextW( swsStatus.c_str() ); }
		}
		catch ( ... ) {}
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
		m_sSel.smMode = m_sgSelGesture.smCurrent = MX_SM_NORMAL;
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
			if ( !m_sSel.HasSelection() || m_sSel.smMode != MX_SM_NORMAL ) {
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
				m_sSel.smMode = m_sgSelGesture.smCurrent = MX_SM_NORMAL;
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
			m_sSel.smMode = m_sgSelGesture.smCurrent = MX_SM_NORMAL;
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
			if ( m_sSel.smMode == MX_SM_NORMAL ) {
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
		m_sSel.smMode = m_sgSelGesture.smCurrent = MX_SM_NORMAL;
		m_sSel.sn.ui64Start = ui64LineStartAdr;
		m_sSel.sn.ui64End   = ui64LineEndAdr;
		m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sSel.sn.ui64Start;

		SetCaretAddr( m_sSel.sn.ui64End + 1, 0, true );
	}

	/**
	 * Reverses the selection (swaps the caret and anchor points).
	 **/
	void CHexEditorControl::ReverseSelection() {
		if ( !m_sSel.HasSelection() ) { return; }

		// Normal selection mode.
		auto ui64Tmp = m_sgSelGesture.ui64AnchorAddr;
			
		if ( m_sgSelGesture.ui64CaretAddr > m_sgSelGesture.ui64AnchorAddr ) {
			// A..C  -> C..A
			m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64CaretAddr;
			m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64AnchorAddr - 1;
		}
		else {
			// C..A  -> A..C
			m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64CaretAddr;
			m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64AnchorAddr;
		}
		SetCaretAddr( ui64Tmp, 0, true );
	}

	/**
	 * Sets the caret to the beginning of the selection.
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 **/
	void CHexEditorControl::MarkSelectionStart( CSecureWString &_swsStatus ) {
		try {
			_swsStatus = _DEC_WS_59D5AB36_Marked_start_of_selection_;
		}
		catch ( ... ) {}
		if ( !m_sSel.HasSelection() ) { return; }

		if ( m_sgSelGesture.ui64CaretAddr > m_sgSelGesture.ui64AnchorAddr ) {
			ReverseSelection();
		}
	}

	/**
	 * Sets the caret to the end of the selection.
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 **/
	void CHexEditorControl::MarkSelectionEnd( CSecureWString &_swsStatus ) {
		try {
			_swsStatus = _DEC_WS_06516F57_Marked_end_of_selection_;
		}
		catch ( ... ) {}
		if ( !m_sSel.HasSelection() ) { return; }

		if ( m_sgSelGesture.ui64CaretAddr < m_sgSelGesture.ui64AnchorAddr ) {
			ReverseSelection();
		}
	}

	/**
	 * Performs an Undo operation.
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 **/
	bool CHexEditorControl::Undo( CSecureWString &_swsStatus ) {
		if ( !m_pheiTarget ) {
			_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}

		if ( m_pheiTarget->Undo( _swsStatus ) ) {
			if ( m_sSelStackIdx != size_t( -1 ) && m_sSelStackIdx < m_vSelectionStack.size() ) {
				m_sSel = m_vSelectionStack[m_sSelStackIdx].sUndoSelection;
				m_sgSelGesture.i32CaretIdx = m_vSelectionStack[m_sSelStackIdx].i32UndoCaretIdx;
				m_sgSelGesture.ui64AnchorAddr = m_vSelectionStack[m_sSelStackIdx].ui64UndoAnchorAddr;
				m_sgSelGesture.ui64MouseAnchorAddr = m_vSelectionStack[m_sSelStackIdx].ui64UndoMouseAnchorAddr;
				m_sgSelGesture.ui64CaretAddr = m_vSelectionStack[m_sSelStackIdx].ui64UndoCaretAddr;
				
				--m_sSelStackIdx;
				EnsureVisible( m_sgSelGesture.ui64CaretAddr, FALSE );
			}
			::InvalidateRect( Wnd(), nullptr, FALSE );
			return true;
		}
		return false;
	}

	/**
	 * Performs an Redo operation.
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 **/
	bool CHexEditorControl::Redo( CSecureWString &_swsStatus ) {
		if ( !m_pheiTarget ) {
			_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}

		if ( m_pheiTarget->Redo( _swsStatus ) ) {
			size_t sTmp = m_sSelStackIdx + 1;
			if ( sTmp < m_vSelectionStack.size() ) {
				m_sSel = m_vSelectionStack[sTmp].sRedoSelection;
				m_sgSelGesture.i32CaretIdx = m_vSelectionStack[sTmp].i32RedoCaretIdx;
				m_sgSelGesture.ui64AnchorAddr = m_vSelectionStack[sTmp].ui64RedoAnchorAddr;
				m_sgSelGesture.ui64MouseAnchorAddr = m_vSelectionStack[sTmp].ui64RedoMouseAnchorAddr;
				m_sgSelGesture.ui64CaretAddr = m_vSelectionStack[sTmp].ui64RedoCaretAddr;

				m_sSelStackIdx = sTmp;
				EnsureVisible( m_sgSelGesture.ui64CaretAddr, FALSE );
			}

			::InvalidateRect( Wnd(), nullptr, FALSE );
			return true;
		}
		return false;
	}

	/**
	 * Deletes the selected bytes.
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 **/
	bool CHexEditorControl::DeleteSelectedOrCaret( CSecureWString &_swsStatus ) {
		try {
			if ( !m_pheiTarget ) {
				_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
				return false;
			}
			if ( m_pheiTarget->ReadOnly() ) {
				_swsStatus = _DEC_WS_D4FFE2E0_File_is_Read_Only;
				return false;
			}

			uint64_t ui64Total = 0, ui64TotaltDeleted = 0, ui64Deleted;
			if ( !m_sSel.HasSelection() ) {
				// Delete a single byte at the caret.
				if ( m_sgSelGesture.ui64CaretAddr < Size() ) {
					if ( m_pheiTarget->Delete( m_sgSelGesture.ui64CaretAddr, 1, ui64Deleted ) && AddSelectionUndo( m_sSel, m_sgSelGesture ) ) {
						CSecureWString swsTmp;
						_swsStatus = _DEC_WS_B8E08C71_Deleted__ + CUtilities::SizeString<CSecureWString>( ui64Deleted, swsTmp );
						::InvalidateRect( Wnd(), nullptr, FALSE );
						return true;
					}
					else {
						_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
						return false;
					}
				}
				return true;
			}
			
		
			uint64_t ui64Caret = m_sgSelGesture.ui64CaretAddr;
			std::vector<MX_SEL_RANGE> vSelections;
			
			if ( m_sSel.GatherSelected_HighToLow( CurStyle()->uiBytesPerRow, vSelections ) ) {
				CHexEditorInterface::MX_UNDO uUndo( m_pheiTarget );
				if ( uUndo.Ready() ) {
					for ( const auto & srThis : vSelections ) {
						if ( m_pheiTarget->Delete( srThis.ui64Start, srThis.ui64Size, ui64Deleted ) ) {
							if ( ui64Caret > srThis.ui64Start ) {
								// If caret is above the start then it needs to be updated.
								if ( ui64Caret >= srThis.ui64Start + srThis.ui64Size ) {
									// Caret is beyond the end of the selection range.
									ui64Caret -= srThis.ui64Size;
								}
								else {
									// Caret is inside the selection range.
									ui64Caret = srThis.ui64Start;
								}
							}
							ui64TotaltDeleted += srThis.ui64Size;
						}
						ui64Total += ui64Deleted;
					}
				}
				else {
					_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
					return false;
				}
			}
			

			auto sOldSel = m_sSel;
			auto sgOldGesture = m_sgSelGesture;
			SetCaretAddr( ui64Caret );
			AddSelectionUndo( sOldSel, sgOldGesture );
			CSecureWString swsTmp;
			_swsStatus = _DEC_WS_B8E08C71_Deleted__ + CUtilities::SizeString<CSecureWString>( ui64TotaltDeleted, swsTmp );
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Deletes the character prior to the caret (backspace).
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 * \return Returns false if _swsStatus contains an error-state string.  True indicates that the process completed normally, regardless of whether anything was deleted or not.
	 **/
	bool CHexEditorControl::DeletePriorToCaret( CSecureWString &_swsStatus ) {
		try {
			if ( !m_pheiTarget ) {
				_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
				return false;
			}
			if ( m_pheiTarget->ReadOnly() ) {
				_swsStatus = _DEC_WS_D4FFE2E0_File_is_Read_Only;
				return false;
			}

			uint64_t ui64Total = 0, ui64TotaltDeleted = 0, ui64Deleted;
			if ( !m_sSel.HasSelection() ) {
				// Delete a single byte just before the caret.
				if ( m_sgSelGesture.ui64CaretAddr != 0 ) {
					if ( m_pheiTarget->Delete( m_sgSelGesture.ui64CaretAddr - 1, 1, ui64Deleted ) ) {
						auto sOldSel = m_sSel;
						auto sgOldGesture = m_sgSelGesture;
						SetCaretAddr( m_sgSelGesture.ui64CaretAddr - 1 );
						AddSelectionUndo( sOldSel, sgOldGesture );	// Failure here is non-critical.
						CSecureWString swsTmp;
						_swsStatus = _DEC_WS_B8E08C71_Deleted__ + CUtilities::SizeString<CSecureWString>( ui64Deleted, swsTmp );
						
						::InvalidateRect( Wnd(), nullptr, FALSE );
						return true;
					}
					else {
						_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
						return false;
					}
				}
				return true;
			}
			return DeleteSelectedOrCaret( _swsStatus );
		}
		catch ( ... ) { return false; }
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

		// If clicking on the drag line, begin dragging mini-map width and avoid selection logic.
		int32_t i32DragX = 0;
		if ( CurStyle()->bShowMiniMap && MiniMapHitDragLine( _pCursorPos, i32DragX ) ) {
			m_mmsMiniMap.bDraggingWidth = true;
			m_mmsMiniMap.i32DragStartX = _pCursorPos.x;
			m_mmsMiniMap.i32DragStartWidth = CurStyle()->i32MiniMapWidthPx;

			::SetCapture( Wnd() );
			::SetCursor( ::LoadCursorW( NULL, IDC_SIZEWE ) );
			return lsw::CWidget::LSW_H_HANDLED;
		}

		// Mini-map click handling.
		if ( CurStyle()->bShowMiniMap ) {
			uint64_t ui64MiniLine = 0;
			uint32_t ui32ByteOff = 0;
			if ( MiniMapCursorToLineByte( _pCursorPos, ui64MiniLine, ui32ByteOff ) ) {
				CMiniMap::MX_MINI_MAP & mmMiniMap = CurStyle()->mmMiniMap;

				const uint64_t ui64TotalBytes = Size();
				const uint32_t ui32MiniBpr = mmMiniMap.ui32BytesPerRow ? mmMiniMap.ui32BytesPerRow : 1U;

				uint64_t ui64ClickAddr = MiniMapLineToAddress( ui64MiniLine ) + static_cast<uint64_t>( ui32ByteOff );
				if ( ui64ClickAddr > ui64TotalBytes ) { ui64ClickAddr = ui64TotalBytes; }

				// Current main-view address range.
				const uint64_t ui64TopLineMain = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine;
				const int32_t i32PageLines = (m_sdScrollView[m_eaEditAs].i32PageLines > 0) ? m_sdScrollView[m_eaEditAs].i32PageLines : 1;
				const uint32_t ui32MainBpr = CurStyle()->uiBytesPerRow ? CurStyle()->uiBytesPerRow : 1U;

				const uint64_t ui64ViewStartAddr = ui64TopLineMain * static_cast<uint64_t>(ui32MainBpr);
				uint64_t ui64ViewEndAddr = ui64ViewStartAddr + (static_cast<uint64_t>(i32PageLines) * static_cast<uint64_t>(ui32MainBpr));
				if ( ui64ViewEndAddr > ui64TotalBytes ) { ui64ViewEndAddr = ui64TotalBytes; }

				if ( ui64ClickAddr < ui64ViewStartAddr || ui64ClickAddr >= ui64ViewEndAddr ) {
					// Click outside current view.
					const int64_t i64Diff = static_cast<int64_t>(ui64ClickAddr) - (static_cast<int64_t>(ui64ViewStartAddr / 2) + static_cast<int64_t>(ui64ViewEndAddr / 2));
					int64_t i64Delta = std::max( std::abs( i64Diff / int64_t( ui32MainBpr ) / i32PageLines ), 1LL );
					if ( i64Diff < 0 ) { i64Delta = -i64Delta; }

					int64_t i64NewFirst = (ui64ClickAddr / ui32MainBpr) - i32PageLines / 2;
					if ( i64NewFirst < 0 ) { i64NewFirst = 0; }

					const uint64_t ui64TotalMainLines = (ui64TotalBytes + ui32MainBpr - 1ULL) / ui32MainBpr;
					const uint64_t ui64MaxFirst = ui64TotalMainLines ? (ui64TotalMainLines - 1ULL) : 0ULL;

					uint64_t ui64NewFirst = static_cast<uint64_t>(i64NewFirst);
					if ( ui64NewFirst > ui64MaxFirst ) { ui64NewFirst = ui64MaxFirst; }

					m_sdScrollView[m_eaEditAs].ui64VPos = ui64NewFirst;
					m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine = ui64NewFirst;
					const uint64_t ui64Lines = TotalLines_FixedWidth();
					const uint64_t ui64MaxV  = ui64Lines ? (ui64Lines - 1ULL) : 0ULL;
					lsw::CBase::SetScrollInfo64( Wnd(), SB_VERT, SIF_POS, ui64MaxV, m_sdScrollView[m_eaEditAs].ui64VPos, 1, TRUE );
					::InvalidateRect( Wnd(), NULL, FALSE );
					return lsw::CWidget::LSW_H_HANDLED;
				}

				// Click inside current view: begin drag-scroll.
				int32_t i32MiniLeft, i32MiniRight, i32MiniTop, i32MiniBottom;
				int32_t i32ThumbTop, i32ThumbHeight, i32Track;
				uint64_t ui64MaxFirstLine;

				if ( MiniMapThumbMetrics( ClientRect(), i32MiniLeft, i32MiniRight, i32MiniTop, i32MiniBottom, i32ThumbTop, i32ThumbHeight, i32Track, ui64MaxFirstLine ) ) {
					const int32_t i32X = _pCursorPos.x;
					const int32_t i32Y = _pCursorPos.y;
					m_mmsMiniMap.bDraggingScroll = true;
					m_mmsMiniMap.i32ScrollGrabY = i32Y - i32ThumbTop;

					// Freeze geometry to avoid drift.
					m_mmsMiniMap.i32DragMiniTop = i32MiniTop;
					m_mmsMiniMap.i32DragTrack = i32Track;
					m_mmsMiniMap.i32DragThumbHeight = i32ThumbHeight;
					m_mmsMiniMap.i32DragThumbTop = i32ThumbTop;
					m_mmsMiniMap.ui64DragMaxFirstLine = ui64MaxFirstLine;

					::SetCapture( Wnd() );
					return lsw::CWidget::LSW_H_HANDLED;

				}
			}
		}

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
		bool bRefresh = false;
		if ( CurStyle()->bShowMiniMap ) {
			int32_t i32Left = MiniMapLeft( ClientRect() );
			if ( _pCursorPos.x >= i32Left && _pCursorPos.x <= (i32Left + CurStyle()->i32MiniMapWidthPx) ) {
				if ( !m_mmsMiniMap.bHighlight ) {
					m_mmsMiniMap.bHighlight = true;
					bRefresh = true;
				}
			}
			else {
				if ( m_mmsMiniMap.bHighlight ) {
					m_mmsMiniMap.bHighlight = false;
					bRefresh = true;
				}
			}

			// If we are dragging the mini-map width, update it and skip selection logic.
			if ( m_mmsMiniMap.bDraggingWidth ) {
				CMiniMap::MX_MINI_MAP & mmMiniMap = CurStyle()->mmMiniMap;

				int32_t i32Delta;
				if ( mmMiniMap.bRightSize ) {
					// Drag bar is on the left edge; moving left increases width.
					i32Delta = m_mmsMiniMap.i32DragStartX - _pCursorPos.x;
				}
				else {
					// Drag bar is on the right edge; moving right increases width.
					i32Delta = _pCursorPos.x - m_mmsMiniMap.i32DragStartX;
				}

				const int32_t i32NewW = MiniMapClampWidth( m_mmsMiniMap.i32DragStartWidth + i32Delta );
				if ( i32NewW != CurStyle()->i32MiniMapWidthPx ) {
					CurStyle()->i32MiniMapWidthPx = i32NewW;
					::InvalidateRect( Wnd(), NULL, FALSE );
				}

				::SetCursor( ::LoadCursorW( NULL, IDC_SIZEWE ) );
				return lsw::CWidget::LSW_H_HANDLED;
			}


			// Mini-map scroll drag bypasses selection logic.
			if ( m_mmsMiniMap.bDraggingScroll && ::GetCapture() == Wnd() ) {
				const lsw::LSW_RECT rClient = ClientRect();

				int32_t i32MiniLeft, i32MiniRight, i32MiniTop, i32MiniBottom;
				int32_t i32ThumbTop, i32ThumbHeight, i32Track;
				uint64_t ui64MaxFirstLine;

				if ( MiniMapThumbMetrics( rClient, i32MiniLeft, i32MiniRight, i32MiniTop, i32MiniBottom,
					i32ThumbTop, i32ThumbHeight, i32Track, ui64MaxFirstLine ) ) {

					// Thumb-top bounds.
					const int32_t i32MinThumbTop = i32MiniTop;
					const int32_t i32MaxThumbTop = i32MiniBottom - i32ThumbHeight;

					int32_t i32GrabY = m_mmsMiniMap.i32ScrollGrabY;
					if ( i32GrabY < 0 ) { i32GrabY = 0; }
					if ( i32GrabY >= i32ThumbHeight ) { i32GrabY = i32ThumbHeight - 1; }

					const int32_t i32MouseY = _pCursorPos.y;

					// Pass 1: clamp thumb top using the current grab.
					int32_t i32NewThumbTop = i32MouseY - i32GrabY;
					if ( i32NewThumbTop < i32MinThumbTop ) { i32NewThumbTop = i32MinThumbTop; }
					if ( i32NewThumbTop > i32MaxThumbTop ) { i32NewThumbTop = i32MaxThumbTop; }

					// Update grab so the mouse stays on the thumb after clamping.
					int32_t i32NewGrabY = i32MouseY - i32NewThumbTop;
					if ( i32NewGrabY < 0 ) { i32NewGrabY = 0; }
					if ( i32NewGrabY >= i32ThumbHeight ) { i32NewGrabY = i32ThumbHeight - 1; }

					// Pass 2: recompute thumb top with the adjusted grab (prevents thumb-height drift).
					i32NewThumbTop = i32MouseY - i32NewGrabY;
					if ( i32NewThumbTop < i32MinThumbTop ) { i32NewThumbTop = i32MinThumbTop; }
					if ( i32NewThumbTop > i32MaxThumbTop ) { i32NewThumbTop = i32MaxThumbTop; }

					m_mmsMiniMap.i32ScrollGrabY = i32NewGrabY;

					// Authoritative thumb position while dragging (draw should use these).
					m_mmsMiniMap.i32DragThumbTop = i32NewThumbTop;
					m_mmsMiniMap.i32DragThumbHeight = i32ThumbHeight;

					const uint64_t ui64ThumbTopPx = static_cast<uint64_t>( i32NewThumbTop - i32MiniTop );

					uint64_t ui64NewFirst = 0ULL;
					if ( ui64MaxFirstLine != 0ULL && i32Track != 0 ) {
						ui64NewFirst = MiniMapMulDivU64Round( ui64ThumbTopPx, ui64MaxFirstLine, static_cast<uint64_t>( i32Track ) );
						if ( ui64NewFirst > ui64MaxFirstLine ) { ui64NewFirst = ui64MaxFirstLine; }
					}

					if ( ui64NewFirst != m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine ) {
						m_sdScrollView[m_eaEditAs].ui64VPos = ui64NewFirst;
						m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine = ui64NewFirst;

						// Keep OS scrollbar max unchanged (your scrollbar still allows last line at top).
						const uint64_t ui64Lines = TotalLines_FixedWidth();
						const uint64_t ui64MaxV = ui64Lines ? (ui64Lines - 1ULL) : 0ULL;
						lsw::CBase::SetScrollInfo64( Wnd(), SB_VERT, SIF_POS, ui64MaxV, m_sdScrollView[m_eaEditAs].ui64VPos, 1, TRUE );
					}

					::InvalidateRect( Wnd(), NULL, FALSE );
					return lsw::CWidget::LSW_H_HANDLED;
				}

				return lsw::CWidget::LSW_H_HANDLED;
			}

			
			// Not dragging: if hovering on the drag line, show horizontal resize cursor.
			int32_t i32DragX = 0;
			if ( MiniMapHitDragLine( _pCursorPos, i32DragX ) && ::GetCapture() != Wnd() ) {
				::SetCursor( ::LoadCursorW( NULL, IDC_SIZEWE ) );
				if ( bRefresh ) {
					::InvalidateRect( Wnd(), NULL, FALSE );
				}
				return lsw::CWidget::LSW_H_HANDLED;
			}
		}

		
		if ( ::GetCapture() != Wnd() ) {
			if ( bRefresh ) {
				::InvalidateRect( Wnd(), NULL, FALSE );
			}
			::SetCursor( ::LoadCursorW( NULL, m_mmsMiniMap.bHighlight ? IDC_ARROW : IDC_IBEAM ) );
			return lsw::CWidget::LSW_H_CONTINUE;
		}
		::SetCursor( ::LoadCursorW( NULL, IDC_IBEAM ) );

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
		m_mmsMiniMap.bDraggingWidth = false;
		m_mmsMiniMap.bDraggingScroll = false;
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
				if ( bCtrl || bShift) {
					SubCaret( 1, 0, bShift );
					//::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_LINELEFT, 0 ), 0 );
				}
				else {
					DecSubCaret();
				}
				break;
			}
			case VK_RIGHT : {
				if ( bCtrl || bShift) {
					AddCaret( 1, 0, bShift );
					//::SendMessageW( Wnd(), WM_HSCROLL, MAKELONG( SB_LINERIGHT, 0 ), 0 );
				}
				else {
					IncSubCaret();
				}
				break;
			}


			case VK_BACK : {
				//if ( CanDelete() ) {
					reinterpret_cast<CDeusHexMachinaWindow *>(m_pwHexParent)->DeletePriorToCaret();
				//}
				break;
			}
			case VK_DELETE : {
				//if ( CanDelete() ) {
					reinterpret_cast<CDeusHexMachinaWindow *>(m_pwHexParent)->DeleteSelectedOrCaret();
				//}
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

		uint64_t ui64Addr = m_pheiTarget->DefaultAddress();
		GoTo( ui64Addr );
		SetCaretAddr( ui64Addr );
		
		RecalcAndInvalidate();
	}

	/**
	 * Gets the address under the given mouse point.
	 * 
	 * \param _pPoint The client-space point under which to find the address.
	 * \param _ui64Addr The return address in the case that the function returns true.
	 * \param _i32SubCaret The subcaret.
	 * \param _bRightArea If true, the click happened in the right area.
	 * \return Returns true if there is a cell representing an address under the given point.  The address will always be valid (in the range of the opened file, memory space, etc.) if the function returns true.
	 **/
	bool CHexEditorControl::PointToAddress( const POINT &_pPoint, uint64_t &_ui64Addr, int32_t &_i32SubCaret, bool &_bRightArea ) {
		uint64_t ui64Max = Size();
		if MX_UNLIKELY( ui64Max == 0 ) { return false; }
		if MX_LIKELY( m_pheiTarget ) {
			const MX_STYLE & stAll = (*CurStyle());
			const int32_t iGutterW = ComputeAddressGutterWidthPx();

			int32_t i32Top = GetRulerHeightPx();
			//if MX_UNLIKELY( _pPoint.y < i32Top ) { return false; }
			if MX_UNLIKELY( _pPoint.x < iGutterW ) { return false; }

			POINT pThis = { _pPoint.x - iGutterW, _pPoint.y - i32Top };


			// Line height.
			const MX_FONT_SET & fsFont = (*Font());
			const int32_t iLineAdv = fsFont.iCharCy + stAll.i32LineSpacingPx;
			uint64_t ui64TopAddress = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine * stAll.uiBytesPerRow;
			int64_t i64Change = pThis.y / iLineAdv * int64_t( stAll.uiBytesPerRow );
			if ( i64Change < 0 ) {
				if ( -i64Change > ui64TopAddress ) { ui64TopAddress = 0; }
				else {
					ui64TopAddress += i64Change;
				}
			}
			else { ui64TopAddress += i64Change; }
			

			auto ui32WidthL = ComputeAreaWidthPx( stAll.dfLeftNumbersFmt ) + stAll.i32PadNumbersLeftPx + stAll.i32PadBetweenNumbersAndTextPx + stAll.i32PadNumbersRightPx;
			for ( size_t I = 0; I < stAll.uiBytesPerRow; ++I ) {
				lsw::LSW_RECT rRect;

				int32_t iGX = 0, iGW = 0;
				if ( GetBackgrondRectForIndex( stAll.dfLeftNumbersFmt, I, int32_t( -int64_t( m_sdScrollView[m_eaEditAs].ui64HPx ) ), iGX, iGW ) ) {
					if ( pThis.x >= iGX && pThis.x < (iGX + iGW) ) {
						_bRightArea = false;
						ui64TopAddress += I;
						_ui64Addr = std::min( ui64Max, ui64TopAddress );

						auto ui32Stride = StrideForFormat( CurStyle()->dfLeftNumbersFmt );
						_ui64Addr = _ui64Addr / ui32Stride * ui32Stride;

						if ( Size() <= _ui64Addr ) {
							_i32SubCaret = 0;
						}
						else {
							GetTextRectForIndex( stAll.dfLeftNumbersFmt, I, int32_t( -int64_t( m_sdScrollView[m_eaEditAs].ui64HPx ) ), iGX, iGW );
							double dSubCarets = double( SubCaretCountForFormat( stAll.dfLeftNumbersFmt ) );
							double dSubWidth = iGW / dSubCarets;
							_i32SubCaret = int32_t( std::clamp( (pThis.x - iGX) / dSubWidth, 0.0, dSubCarets - 1.0 ) );
						}
						return true;
					}
				}
				if ( stAll.bShowRightArea ) {
					if ( GetBackgrondRectForIndex( stAll.dfRightNumbersFmt, I, int32_t( -int64_t( m_sdScrollView[m_eaEditAs].ui64HPx ) ) + ui32WidthL, iGX, iGW ) ) {
						if ( pThis.x >= iGX && pThis.x < (iGX + iGW) ) {
							_bRightArea = true;
							ui64TopAddress += I;
							_ui64Addr = std::min( ui64Max, ui64TopAddress );

							auto ui32Stride = StrideForFormat( CurStyle()->dfRightNumbersFmt );
							_ui64Addr = _ui64Addr / ui32Stride * ui32Stride;

							if ( Size() <= _ui64Addr ) {
								_i32SubCaret = 0;
							}
							else {
								GetTextRectForIndex( stAll.dfRightNumbersFmt, I, int32_t( -int64_t( m_sdScrollView[m_eaEditAs].ui64HPx ) ) + ui32WidthL, iGX, iGW );
								double dSubCarets = double( SubCaretCountForFormat( stAll.dfRightNumbersFmt ) );
								double dSubWidth = iGW / dSubCarets;
								_i32SubCaret = int32_t( std::clamp( (pThis.x - iGX) / dSubWidth, 0.0, dSubCarets - 1.0 ) );
							}
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
			wceEx.SetCursor( ::LoadCursorW( NULL, IDC_IBEAM ) );
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

		if ( stAll.bShowMiniMap ) {
			DrawMiniMap( _hDc,
				MiniMapLeft( _rRect ),
				0, _rRect );
		}
		return true;
	}

	/**
	 * \brief Draws the mini-map.
	 * 
	 * \param _hDc The device context to which to draw.
	 * \param _iXLeft The left X coordinate of the mini-map.
	 * \param _iYTop The top Y coordinate of the mini-map.
	 * \param _rRect The destination rectangle for the mini-map.
	 */
	void CHexEditorControl::DrawMiniMap( HDC _hDc, int32_t _iXLeft, int32_t _iYTop, const lsw::LSW_RECT &_rRect ) {
		if MX_UNLIKELY( !_hDc || !m_pheiTarget ) { return; }

		CMiniMap::MX_MINI_MAP & mmMiniMap = CurStyle()->mmMiniMap;

		const int32_t i32RectH = _rRect.Height();
		uint64_t ui64AddrStart, ui64AddrEnd;
		uint32_t ui32SrcW = mmMiniMap.ui32BytesPerRow;
		int32_t i32VisualWidth;
		{
			lsw::LSW_RECT rBg = _rRect;
			
			if ( mmMiniMap.bRightSize ) {
				rBg.left = _iXLeft + 1;
				rBg.right = _rRect.right;
				i32VisualWidth = int32_t( _rRect.right - rBg.left );
			}
			else {
				rBg.left = 0;
				rBg.right = _iXLeft - 1;
				i32VisualWidth = int32_t( rBg.left );
			}
			if ( rBg.Width() >= 0 ) {
				::FillRect( _hDc, &rBg,
					lsw::CBase::BrushCache().Brush( MX_GetRgbValue( BackColors()->crEditor ) ) );
			}
			if ( mmMiniMap.bRightSize ) {
				rBg.left--;
				rBg.right = rBg.left + 1;
			}
			else {
				rBg.left = rBg.right;
				rBg.right++;
			}
			::FillRect( _hDc, &rBg,
				lsw::CBase::BrushCache().Brush( MX_GetRgbValue( ForeColors()->crMiniMapSeparatorLine ) ) );
		}
		if ( mmMiniMap.bAutoSnap ) {
			ui32SrcW = i32VisualWidth / (mmMiniMap.ui32SnapLines * mmMiniMap.ui32Zoom) * mmMiniMap.ui32SnapLines;
			ui32SrcW = std::max( ui32SrcW, mmMiniMap.ui32SnapLines );
		}
		MiniMapAddressRange( i32RectH, ui64AddrStart, ui64AddrEnd, ui32SrcW );

		m_mmsMiniMap.ui64Address = ui64AddrStart;
		m_mmsMiniMap.ui64End = ui64AddrEnd;
		size_t sTotal = size_t( m_mmsMiniMap.ui64End - m_mmsMiniMap.ui64Address );
		if ( !sTotal || !ui32SrcW ) { return; }

		
		const size_t sSrcW = static_cast<size_t>(ui32SrcW);
		const size_t sSrcH = (sTotal + (sSrcW - 1U)) / sSrcW;
		
		

		try {
			CHexEditorInterface::CBuffer bBuffer;
			if ( !m_pheiTarget->Read( m_mmsMiniMap.ui64Address, bBuffer, sTotal ) ) { return; }

			m_mmsMiniMap.vBuffer.resize( sSrcW * sSrcH );
			if ( mmMiniMap.cColors == CMiniMap::MX_C_NONE ) {
				CMiniMap::ContrastEndpoints( mmMiniMap.ui32Contrast, mmMiniMap.ui8Lo, mmMiniMap.ui8Hi );
			}

			for ( size_t I = 0; I < m_mmsMiniMap.vBuffer.size(); ++I ) {
				if ( I < bBuffer.size() ) {
					m_mmsMiniMap.vBuffer[I] = MiniMapColor( m_mmsMiniMap.ui64Address + I, bBuffer.data() + I, bBuffer.size() - I );
				}
				else {
					m_mmsMiniMap.vBuffer[I] = BackColors()->crEditor;
				}
			}

			struct MX_BMI32 {
				BITMAPINFOHEADER					bih;
				DWORD								dwMasks[3];
			} bmi = {};

			bmi.bih.biSize = sizeof( bmi.bih );
			bmi.bih.biWidth = static_cast<LONG>(ui32SrcW);
			bmi.bih.biHeight = -static_cast<LONG>(sSrcH);				// Top-down.
			bmi.bih.biPlanes = 1;
			bmi.bih.biBitCount = 32;
			bmi.bih.biCompression = BI_BITFIELDS;

			// COLORREF is 0x00BBGGRR, so interpret DWORD pixels using these masks.
			bmi.dwMasks[0] = 0x000000FF;								// Red.
			bmi.dwMasks[1] = 0x0000FF00;								// Green.
			bmi.dwMasks[2] = 0x00FF0000;								// Blue.

			if ( mmMiniMap.bRightSize ) {
				++_iXLeft;
			}
			else {
				_iXLeft = _iXLeft - 1 - (sSrcW * mmMiniMap.ui32Zoom);
			}

			::StretchDIBits(
				_hDc,
				_iXLeft,
				_iYTop,
				sSrcW * mmMiniMap.ui32Zoom,
				sSrcH * mmMiniMap.ui32Zoom,
				0,
				0,
				static_cast<int32_t>(ui32SrcW),
				static_cast<int32_t>(sSrcH),
				m_mmsMiniMap.vBuffer.data(),
				reinterpret_cast<const BITMAPINFO *>(&bmi),
				DIB_RGB_COLORS,
				SRCCOPY
			);

		}
		catch ( ... ) {}
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
		if ( Size() % stAll.uiBytesPerRow == 0 ) {
			++ui64MaxLines;
		}
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
	 *  - The rulerÅfs height equals the base character height for the font; line spacing is ignored.
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
		auto HexLabel = [&stAll]( uint32_t _ui32V, uint32_t _ui32Digits ) -> std::wstring {
			static const wchar_t * pwsS = L"0123456789ABCDEF";
			std::wstring wsOut;

			uint32_t ui32MaxDig = 16;
			if ( stAll.rfRulerFmt == MX_RF_DEC ) {
				ui32MaxDig = 10;
				_ui32Digits = 1;
			}

			if MX_LIKELY( _ui32V < ui32MaxDig && _ui32Digits == 1 ) { wsOut.push_back( pwsS[_ui32V] ); }
			else {
				// Wider rows: use more digits.
				uint32_t ui32N = _ui32V;
				std::wstring wsTmp;
				while ( ui32N ) {
					wsTmp.push_back( pwsS[ui32N%ui32MaxDig] );
					if ( stAll.rfRulerFmt == MX_RF_DEC ) { break; }
					ui32N /= ui32MaxDig;
				}
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
			uint32_t ui32Stride	= StrideForFormat( _dfFmt );
			const uint32_t ui32GroupSz	= stAll.uiGroupSize ? stAll.uiGroupSize : 1;
			if ( ui32GroupSz == 0U ) { return; }

			const uint32_t ui32Groups = (ui32Bpr + ui32GroupSz - 1U) / ui32GroupSz;
			const bool bCenter = (_dfFmt == MX_DF_CHAR && CurStyle()->csCharSet == CCharSets::MX_CS_UNICODE) || ui32Stride == 1;
			ui32Stride = std::max<uint32_t>( std::min<uint32_t>( _ui32Digits, 2 ), ui32Stride );
			/*if ( _dfFmt == MX_DF_CHAR && CurStyle()->csCharSet == CCharSets::MX_CS_UNICODE ) {
				ui32Stride = 2;
			}*/
			for ( uint32_t I = 0; I < ui32Bpr; I += ui32Stride ) {
				int32_t iGX = 0, iGW = 0;
				if ( !GetTextRectForIndex( _dfFmt, I, _iAreaXBase, iGX, iGW ) ) { break; }

				if ( stAll.bShowRulerArrows && (I / ui32Stride == (m_sgSelGesture.ui64CaretAddr % ui32Bpr) / ui32Stride) ) {
					auto i32X = iGX + (bCenter ? (iGW / 2) : 0);
					lsw::CHelpers::DrawLineSinglePixel_Inclusive( _hDc, i32X - 2, _iYTop, i32X + 2, _iYTop, ForeColors()->crRulerMarker );
					lsw::CHelpers::DrawLineSinglePixel_Inclusive( _hDc, i32X - 1, _iYTop + 1, i32X + 1, _iYTop + 1, ForeColors()->crRulerMarker );
					lsw::CHelpers::DrawLineSinglePixel_Inclusive( _hDc, i32X - 0, _iYTop + 2, i32X + 0, _iYTop + 2, ForeColors()->crRulerMarker );
				}

				if ( stAll.bShowRulerLabels ) {
					// Label is the byte index of the first item in the group within the row.
					const uint32_t ui32ByteIndexInRow = I;
					std::wstring wsTmp = HexLabel( _dfFmt == MX_DF_CHAR ? (ui32ByteIndexInRow & 0x0F) : ui32ByteIndexInRow, _ui32Digits );

					// Center horizontally inside [iGX, iGW].
					SIZE sSize {};
					::GetTextExtentPoint32W( _hDc, wsTmp.c_str(), int32_t( wsTmp.size() ), &sSize );
					const int32_t iTextX = iGX + (bCenter ? (iGW - sSize.cx) / 2 : 0);
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

		auto ui64MaxLines = TotalLines() - ui64Line + 1;
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
		uint32_t ui32Stride = StrideForFormat( _dfFmt );
		bool bDivLines = stAll.ui64DivisionSpacing >= ui32Stride && (stAll.ui64DivisionSpacing % ui32Stride == 0);
		switch ( _dfFmt ) {
			case MX_DF_CHAR : { bDivLines = false; break; }
		}
		
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

			//for ( int32_t J = 0; J < ui32Bpr; J += ui32Stride ) {
			for ( int32_t J = 0; J < ui32Bpr; ++J ) {
				ui64ThisAddr = ui64StartAddress + I * ui32Bpr + J;
				GetTextRectForIndex( _dfFmt, J, _iXLeft, iL, iW );
				if ( J < sDataSize ) {
					size_t sBuffSize = J < sDataSize ? sDataSize - J : 0;
					const uint8_t * pui8Value = sBuffSize ? &pui8Data[J] : reinterpret_cast<const uint8_t *>(&wcDefChar);
					uint8_t ui8Value = (*pui8Value);
				
					if ( bDivLines && stAll.ui64DivisionSpacing ) {
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
					if ( J % ui32Stride != 0 ) { continue; }
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

#define MX_PRINT_PRIM( CASE, TYPE )																	\
	case CASE : {																					\
		try {																						\
			wsTmp.resize( 1 );																		\
			if ( sBuffSize < sizeof( TYPE ) ) { wsTmp[0] = L'\0'; }									\
			else {																					\
				i32CharSizeCounter -= ui32Stride;													\
				if ( i32CharSizeCounter <= 1 ) {													\
					i32CharSizeCounter = sizeof( TYPE ) + 1;										\
					wsTmp = std::to_wstring( (*reinterpret_cast<const TYPE *>(pui8Value)) );		\
				}																					\
				else { wsTmp[0] = L'\0'; }															\
			}																						\
		}																							\
		catch ( ... ) {}																			\
		break;																						\
	}

						MX_PRINT_PRIM( MX_DF_UINT8, uint8_t )
						MX_PRINT_PRIM( MX_DF_INT8, int8_t )
						MX_PRINT_PRIM( MX_DF_UINT16, uint16_t )
						MX_PRINT_PRIM( MX_DF_INT16, int16_t )
						MX_PRINT_PRIM( MX_DF_UINT32, uint32_t )
						MX_PRINT_PRIM( MX_DF_INT32, int32_t )
						MX_PRINT_PRIM( MX_DF_UINT64, uint64_t )
						MX_PRINT_PRIM( MX_DF_INT64, int64_t )
#undef MX_PRINT_PRIM

						case MX_DF_POINTER16 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( uint16_t ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( uint16_t ) + 1;
										wsTmp = std::format( L"{:04X}", (*reinterpret_cast<const uint16_t *>(pui8Value)) );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}
						case MX_DF_POINTER32 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( uint32_t ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( uint32_t ) + 1;
										wsTmp = std::format( L"{:08X}", (*reinterpret_cast<const uint32_t *>(pui8Value)) );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}
						case MX_DF_POINTER64 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( uint64_t ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( uint64_t ) + 1;
										wsTmp = std::format( L"{:016X}", (*reinterpret_cast<const uint64_t *>(pui8Value)) );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}
						case MX_DF_FLOAT10 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( uint16_t ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( uint16_t ) + 1;
										ee::CFloatX fFloat;
										fFloat.CreateFromBits( (*reinterpret_cast<const uint16_t *>(pui8Value)), 5, 6, true, false );
										wsTmp = std::format( L"{1:.{0}g}", stAll.ui32FloatDigits, fFloat.AsDouble() );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}
						case MX_DF_FLOAT11 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( uint16_t ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( uint16_t ) + 1;
										ee::CFloatX fFloat;
										fFloat.CreateFromBits( (*reinterpret_cast<const uint16_t *>(pui8Value)), 5, 7, true, false );
										wsTmp = std::format( L"{1:.{0}g}", stAll.ui32FloatDigits, fFloat.AsDouble() );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}
						case MX_DF_BFLOAT16 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( uint16_t ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( uint16_t ) + 1;
										ee::CFloatX fFloat;
										fFloat.CreateFromBits( (*reinterpret_cast<const uint16_t *>(pui8Value)), 8, 8, true, true );
										wsTmp = std::format( L"{1:.{0}g}", stAll.ui32FloatDigits, fFloat.AsDouble() );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}
						case MX_DF_FLOAT16 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( uint16_t ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( uint16_t ) + 1;
										ee::CFloat16 fFloat( (*reinterpret_cast<const uint16_t *>(pui8Value)) );
										wsTmp = std::format( L"{1:.{0}g}", stAll.ui32FloatDigits, double( fFloat ) );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}
						case MX_DF_FLOAT32 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( float ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( float ) + 1;
										wsTmp = std::format( L"{1:.{0}g}", stAll.ui32FloatDigits, (*reinterpret_cast<const float *>(pui8Value)) );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}
						case MX_DF_FLOAT64 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( double ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( double ) + 1;
										wsTmp = std::format( L"{1:.{0}g}", stAll.ui32FloatDigits, (*reinterpret_cast<const double *>(pui8Value)) );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}

						case MX_DF_PCM8 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( int8_t ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( int8_t ) + 1;
										double dV = std::clamp( (static_cast<int32_t>((*pui8Value)) - 128) / 127.0, -1.0, 1.0 );
										wsTmp = std::format( L"{1:.{0}f}", stAll.ui32FloatDigits, dV );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}
						case MX_DF_PCM16 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( int16_t ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( int16_t ) + 1;
										constexpr double dFactor = 32767.0;				//std::pow( 2.0, 16.0 - 1.0 ) - 1.0;
										double dV = std::clamp( double( (*reinterpret_cast<const int16_t *>(pui8Value)) ) / dFactor, -1.0, 1.0 );
										wsTmp = std::format( L"{1:.{0}f}", stAll.ui32FloatDigits, dV );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}
						case MX_DF_PCM24 : {
							try {
								wsTmp.resize( 1 );
								constexpr size_t sPcm24Size = sizeof( int8_t ) * 3;
								if ( sBuffSize < sPcm24Size ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sPcm24Size + 1;
										constexpr double dFactor = 2147483392.0;		//(std::pow( 2.0, 24.0 - 1.0 ) - 1.0) * 256.0;
										int32_t i32Tmp = 0;
										std::memcpy( &i32Tmp, pui8Value, sPcm24Size );
										i32Tmp <<= 8;
										double dV = std::clamp( double( i32Tmp ) / dFactor, -1.0, 1.0 );
										wsTmp = std::format( L"{1:.{0}f}", stAll.ui32FloatDigits, dV );
									}
									else { wsTmp[0] = L'\0'; }
								}
							}
							catch ( ... ) {}
							break;
						}
						case MX_DF_PCM32 : {
							try {
								wsTmp.resize( 1 );
								if ( sBuffSize < sizeof( int32_t ) ) { wsTmp[0] = L'\0'; }
								else {
									i32CharSizeCounter -= ui32Stride;
									if ( i32CharSizeCounter <= 1 ) {
										i32CharSizeCounter = sizeof( int32_t ) + 1;
										constexpr double dFactor = 2147483647.0;		//std::pow( 2.0, 32.0 - 1.0 ) - 1.0;
										double dV = std::clamp( double( (*reinterpret_cast<const int32_t *>(pui8Value)) ) / dFactor, -1.0, 1.0 );
										wsTmp = std::format( L"{1:.{0}f}", stAll.ui32FloatDigits, dV );
									}
									else { wsTmp[0] = L'\0'; }
								}
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
				}

				// Caret.
				if ( ui64ThisAddr / ui32Stride == m_sgSelGesture.ui64CaretAddr / ui32Stride && !bDrawCaret ) {
					int32_t i32MaxCaretIdx = SubCaretCountForFormat( _dfFmt );
					double dSubWidth = double( iW ) / i32MaxCaretIdx;
					rCarect.top = iY + iLineAdv - 1;
					rCarect.left = iL + LONG( std::round( dSubWidth * std::clamp( m_sgSelGesture.i32CaretIdx, 0, i32MaxCaretIdx - 1 ) ) );
					rCarect.right = iL + LONG( std::round( dSubWidth * (std::clamp( m_sgSelGesture.i32CaretIdx, 0, i32MaxCaretIdx - 1 ) + 1.0) ) ) * ui32Stride;
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
	 * Gets the color of a Mini-Map pixel.
	 * 
	 * \param _ui64Address The address of the data in the cell.
	 * \param _pui8Value The address of the data in the cell.
	 * \param _sSize The address of the data in the cell.
	 * \return Returns the color for the given Mini-Map pixel.
	 **/
	COLORREF CHexEditorControl::MiniMapColor( uint64_t _ui64Address, const uint8_t * _pui8Value, size_t _sSize ) {
		const MX_STYLE & stAll = (*CurStyle());
		COLORREF crBaseColor;
		bool bDefaultBaseColor = false;
		bool bReadable = true;
#define MX_CHECK_PROC( CAST, ENUM )																																	\
	if ( m_pheiTarget && m_pheiTarget->Type() == CHexEditorInterface::ENUM ) {																						\
		CAST * phepProc = static_cast<CAST *>(m_pheiTarget);																										\
		if ( !phepProc->Process().IsReadableByQuery( _ui64Address ) ) {																								\
			bReadable = false; crBaseColor = BackColors()->crUnreadable; goto PostBase;																				\
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
		crBaseColor = BackColors()->crEditor;
	PostBase :

		
		if ( stAll.bHighlightNewLines && ((*_pui8Value) == '\r' || (*_pui8Value) == '\n') ) {
			crBaseColor = bDefaultBaseColor ? ForeColors()->crHighlightingHex : Mix( crBaseColor, ForeColors()->crHighlightingHex );
			bDefaultBaseColor = false;
		}
		else if ( stAll.bHighlightAlphaNumeric && std::isalnum( (*_pui8Value) ) ) {
			crBaseColor = bDefaultBaseColor ? ForeColors()->crHighlightingHex : Mix( crBaseColor, ForeColors()->crHighlightingHex );
			bDefaultBaseColor = false;
		}
		else if ( stAll.bHighlightControl && std::iscntrl( (*_pui8Value) ) ) {
			crBaseColor = bDefaultBaseColor ? ForeColors()->crHighlightingHex : Mix( crBaseColor, ForeColors()->crHighlightingHex );
			bDefaultBaseColor = false;
		}
		else if ( stAll.bHighlightNonAscii && (*_pui8Value) >= 0x80 ) {
			crBaseColor = bDefaultBaseColor ? ForeColors()->crHighlightingHex : Mix( crBaseColor, ForeColors()->crHighlightingHex );
			bDefaultBaseColor = false;
		}
		else if ( stAll.bHighlightZeros && (*_pui8Value) == 0x00 ) {
			crBaseColor = bDefaultBaseColor ? ForeColors()->crHighlightingHex : Mix( crBaseColor, ForeColors()->crHighlightingHex );
			bDefaultBaseColor = false;
		}


		// Mix colors.
		uint64_t ui64Caret = m_sgSelGesture.ui64CaretAddr;
		if ( !m_sSel.HasSelection() ) {
			{
				if ( MX_GetAValue( BackColors()->crHighlightLineHexEditor ) && ui64Caret / stAll.uiBytesPerRow == (_ui64Address / stAll.uiBytesPerRow) ) {
					crBaseColor = Mix( crBaseColor, BackColors()->crHighlightLineHexEditor );
					bDefaultBaseColor = false;
				}
			}
		}
		else {
			if ( _ui64Address < Size() && m_sSel.IsSelected( _ui64Address, stAll.uiBytesPerRow ) && (MX_GetAValue( BackColors()->crSelected ) || bDefaultBaseColor) ) {
				crBaseColor = bDefaultBaseColor ? BackColors()->crSelected : Mix( crBaseColor, BackColors()->crSelected );
				bDefaultBaseColor = false;
			}
		}

		uint8_t ui8Value = (_pui8Value && _sSize) ? (*_pui8Value) : 0;
		COLORREF crRampColor = 0;
		switch ( stAll.mmMiniMap.cColors ) {
			case CMiniMap::MX_C_NONE : {
				crRampColor = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, CMiniMap::ApplyContrast( ui8Value, stAll.mmMiniMap.ui8Lo, stAll.mmMiniMap.ui8Hi ) );
				crRampColor = MX_RGBA( crRampColor, crRampColor, crRampColor, 0x7F );
				break;
			}
			case CMiniMap::MX_C_ASCII : {
				auto crColor = CMiniMap::m_crAscii[ui8Value];
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_RAINBOW : {
				auto crColor = CMiniMap::m_crRainbow[ui8Value];
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_HEAT : {
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, 0xBF );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, CMiniMap::ApplyContrast( ui8Value, 0x01, 0xBF ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, 0x01 );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_OCEAN : {
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, CMiniMap::ApplyContrast( ui8Value, 0x07, 0x6F ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, CMiniMap::ApplyContrast( ui8Value, 0x3E, 0xA4 ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, CMiniMap::ApplyContrast( ui8Value, 0x75, 0xBF ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_GREY : {
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, CMiniMap::ApplyContrast( ui8Value, 0x01, 0xBF ) );
				crRampColor = MX_RGBA( ui8R, ui8R, ui8R, 0x7F );
				break;
			}
			case CMiniMap::MX_C_CUBEHELIX : {
				auto crColor = CMiniMap::m_crCubehelix[ui8Value];
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_VIRIDIS : {
				auto crColor = CMiniMap::Palette16( CMiniMap::m_crViridis16, ui8Value );
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_PLASMA : {
				auto crColor = CMiniMap::Palette16( CMiniMap::m_crPlasma16, ui8Value );
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_MAGMA : {
				auto crColor = CMiniMap::Palette16( CMiniMap::m_crMagma16, ui8Value );
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_INFERNO : {
				auto crColor = CMiniMap::Palette16( CMiniMap::m_crInferno16, ui8Value );
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_CIVIDIS : {
				auto crColor = CMiniMap::Palette16( CMiniMap::m_crCividis16, ui8Value );
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_TURBO : {
				auto crColor = CMiniMap::Palette16( CMiniMap::m_crTurbo16, ui8Value );
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_COOLWARM : {
				auto crColor = CMiniMap::Palette16( CMiniMap::m_crCoolWarm16, ui8Value );
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_BLUEWHITERED : {
				auto crColor = CMiniMap::Palette16( CMiniMap::m_crBlueWhiteRed16, ui8Value );
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_TWILIGHT : {
				auto crColor = CMiniMap::Palette16( CMiniMap::m_crTwilight16, ui8Value );
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
			case CMiniMap::MX_C_TWILIGHTSHIFTED : {
				auto crColor = CMiniMap::Palette16( CMiniMap::m_crTwilightShifted16, ui8Value );
				uint8_t ui8R = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetRValue( crColor ) );
				uint8_t ui8G = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetGValue( crColor ) );
				uint8_t ui8B = CMiniMap::BrightnessValue( stAll.mmMiniMap.i32Brightness, GetBValue( crColor ) );
				crRampColor = MX_RGBA( ui8R, ui8G, ui8B, 0x7F );
				break;
			}
		}

		if ( !bReadable ) {
			crRampColor = crBaseColor;
		}
		
		// Mini-Map post-processing.
		COLORREF crFinalMix = bDefaultBaseColor ? crRampColor : Mix( crBaseColor, crRampColor );
		bool bHighlightView = m_mmsMiniMap.bHighlight || m_mmsMiniMap.bDraggingScroll || m_mmsMiniMap.bDraggingWidth;
		if ( MX_GetAValue( BackColors()->crMiniMapHexSelectionBox )  || ((bHighlightView) && MX_GetAValue( BackColors()->crMiniMapHexSelectionBoxHighlight )) ) {
			const MX_FONT_SET & fsFont = (*Font());
			const int32_t iLineAdv = fsFont.iCharCy + stAll.i32LineSpacingPx;
			uint64_t ui64TopAddress = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine * stAll.uiBytesPerRow;
			uint64_t ui64BottomAddress = ui64TopAddress + (m_sdScrollView[m_eaEditAs].i32PageLines) * stAll.uiBytesPerRow;
			//uint64_t ui64TopViewRow = ui64TopAddress / stAll.mmMiniMap.

			if ( _ui64Address >= ui64TopAddress && _ui64Address < ui64BottomAddress ) {
				crFinalMix = Mix( crFinalMix, bHighlightView ? BackColors()->crMiniMapHexSelectionBoxHighlight : BackColors()->crMiniMapHexSelectionBox );
			}
		}
		return crFinalMix;
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
	 * Computes the start and end addresses for the Mini-Map.
	 * 
	 * \param _i32ScreenHeight The height of the display area.
	 * \param _ui64StartAddr Holds the returned start address.
	 * \param _ui64EndAddr Holds the returned end address.
	 * \param _ui32BytesPerRow The number of bytes per row.
	 **/
	void CHexEditorControl::MiniMapAddressRange( int32_t _i32ScreenHeight, uint64_t &_ui64StartAddr, uint64_t &_ui64EndAddr, uint32_t _ui32BytesPerRow ) {
#if 1
		CMiniMap::MX_MINI_MAP & mmMiniMap = CurStyle()->mmMiniMap;

		if ( _i32ScreenHeight <= 0 || !_ui32BytesPerRow ) {
			_ui64StartAddr = 0;
			_ui64EndAddr = 0;
			return;
		}

		const uint64_t ui64TotalBytes = Size();
		if ( ui64TotalBytes == 0 ) {
			_ui64StartAddr = 0;
			_ui64EndAddr = 0;
			return;
		}

		const uint32_t ui32Zoom = mmMiniMap.ui32Zoom ? mmMiniMap.ui32Zoom : 1U;

		// Total mini-map lines in the file based on mini-map bytes-per-row.
		const uint64_t ui64TotalMiniLines = (ui64TotalBytes + _ui32BytesPerRow - 1ULL) / _ui32BytesPerRow;
		if ( ui64TotalMiniLines == 0 ) {
			_ui64StartAddr = 0;
			_ui64EndAddr = 0;
			return;
		}

		// Number of mini-map lines visible on screen at this zoom.
		//uint64_t ui64SpanMiniLines = static_cast<uint64_t>(_i32ScreenHeight) / static_cast<uint64_t>(ui32Zoom) + 1ULL;
		//if ( ui64SpanMiniLines < 1ULL ) { ui64SpanMiniLines = 1ULL; }
		uint64_t ui64SpanMiniLines = MiniMapSpanLines( _i32ScreenHeight, ui32Zoom );
		

		// If the whole file fits, show it all.
		if ( ui64SpanMiniLines >= ui64TotalMiniLines ) {
			_ui64StartAddr = 0;
			_ui64EndAddr = ui64TotalBytes;
			return;
		}

		// Main editor visible range (in main lines).
		const uint64_t ui64TopLineMain = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine;
		const int32_t i32PageLines = (m_sdScrollView[m_eaEditAs].i32PageLines > 0) ?
			m_sdScrollView[m_eaEditAs].i32PageLines : 1;

		const uint32_t ui32MainBpr = CurStyle()->uiBytesPerRow ? CurStyle()->uiBytesPerRow : 1U;

		const uint64_t ui64PageBytesMain = static_cast<uint64_t>(i32PageLines) * static_cast<uint64_t>(ui32MainBpr);
		const uint64_t ui64TopAddrMain = ui64TopLineMain * static_cast<uint64_t>(ui32MainBpr);

		// Maximum top-of-view address (clamped).
		uint64_t ui64MaxTopAddrMain = 0;
		if ( ui64TotalBytes > ui64PageBytesMain ) {
			ui64MaxTopAddrMain = ui64TotalBytes - ui64PageBytesMain;
		}

		// Convert to mini-map line space.
		uint64_t ui64TopLineMini = ui64TopAddrMain / static_cast<uint64_t>(_ui32BytesPerRow);
		uint64_t ui64MaxTopLineMini = ui64MaxTopAddrMain / static_cast<uint64_t>(_ui32BytesPerRow);

		// Map top line to a start line that moves continuously from 0..(total-span).
		const uint64_t ui64MaxStart = ui64TotalMiniLines - ui64SpanMiniLines;

		uint64_t ui64FirstLineMini = 0;
		if ( ui64MaxTopLineMini ) {
			try {
				uint64_t ui64Rem;
				ui64FirstLineMini = _umuldiv128_rounded( ui64TopLineMini, ui64MaxStart, ui64MaxTopLineMini, &ui64Rem );
			}
			catch ( ... ) {
				ui64FirstLineMini = 0;
			}
		}

		if ( ui64FirstLineMini > ui64MaxStart ) {
			ui64FirstLineMini = ui64MaxStart;
		}

		const uint64_t ui64LastLineMini = ui64FirstLineMini + ui64SpanMiniLines;

		_ui64StartAddr = ui64FirstLineMini * static_cast<uint64_t>(_ui32BytesPerRow);
		_ui64EndAddr = ui64LastLineMini * static_cast<uint64_t>(_ui32BytesPerRow);

		if ( _ui64EndAddr > ui64TotalBytes ) {
			_ui64EndAddr = ui64TotalBytes;
		}

#else
		CMiniMap::MX_MINI_MAP & mmMiniMap = CurStyle()->mmMiniMap;

		if ( _i32ScreenHeight <= 0 || !_ui32BytesPerRow ) {
			_ui64StartAddr = 0;
			_ui64EndAddr = 0;
			return;
		}

		const uint64_t ui64TotalBytes = Size();
		if ( ui64TotalBytes == 0 ) {
			_ui64StartAddr = 0;
			_ui64EndAddr = 0;
			return;
		}

		// Mini-map bytes-per-row and zoom (pixel scale 1..10).
		const uint32_t ui32Zoom = mmMiniMap.ui32Zoom ? mmMiniMap.ui32Zoom : 1U;

		// Total mini-map "lines" in the file based on mini-map bytes-per-row.
		const uint64_t ui64TotalMiniLines = (ui64TotalBytes + _ui32BytesPerRow - 1ULL) / _ui32BytesPerRow;

		// Number of mini-map lines we can display in the given rectangle at this zoom.
		// Higher zoom -> fewer lines (zoomed in).
		uint64_t ui64SpanMiniLines = static_cast<uint64_t>(_i32ScreenHeight) / static_cast<uint64_t>(ui32Zoom) + 1ULL;

		// Main editor visible range in lines (using main bytes-per-row).
		const uint64_t ui64TopLineMain = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine;
		const int32_t i32PageLines = (m_sdScrollView[m_eaEditAs].i32PageLines > 0) ?
			m_sdScrollView[m_eaEditAs].i32PageLines : 1;

		const uint32_t ui32MainBpr = CurStyle()->uiBytesPerRow ? CurStyle()->uiBytesPerRow : 1U;

		// Compute the midpoint address of the main window:
		// midAddr = topAddr + (pageLines * mainBpr) / 2.
		const uint64_t ui64TopAddrMain = ui64TopLineMain * static_cast<uint64_t>(ui32MainBpr);
		const uint64_t ui64PageBytesMain = static_cast<uint64_t>(i32PageLines) * static_cast<uint64_t>(ui32MainBpr);
		const uint64_t ui64MidAddrMain = ui64TopAddrMain + (ui64PageBytesMain >> 1);

		// Map that midpoint address into mini-map line space.
		const uint64_t ui64MidLineMini = ui64MidAddrMain / static_cast<uint64_t>(_ui32BytesPerRow);

		// Center the mini-map span around ui64MidLineMini.
		const uint64_t ui64HalfSpanMini = ui64SpanMiniLines >> 1;

		uint64_t ui64FirstLineMini;
		if ( ui64MidLineMini > ui64HalfSpanMini ) {
			ui64FirstLineMini = ui64MidLineMini - ui64HalfSpanMini;
		}
		else {
			ui64FirstLineMini = 0;
		}

		uint64_t ui64LastLineMini = ui64FirstLineMini + ui64SpanMiniLines;
		if ( ui64LastLineMini > ui64TotalMiniLines ) {
			ui64LastLineMini = ui64TotalMiniLines;

			// Keep the span size when clamping at the bottom if possible.
			if ( ui64LastLineMini > ui64SpanMiniLines ) {
				ui64FirstLineMini = ui64LastLineMini - ui64SpanMiniLines;
			}
			else {
				ui64FirstLineMini = 0;
			}
		}

		// Convert mini-map line range to byte addresses and clamp to file size.
		_ui64StartAddr = ui64FirstLineMini * static_cast<uint64_t>(_ui32BytesPerRow);
		_ui64EndAddr = ui64LastLineMini * static_cast<uint64_t>(_ui32BytesPerRow);
		if ( _ui64EndAddr > ui64TotalBytes ) {
			_ui64EndAddr = ui64TotalBytes;
		}
#endif
	}

	/**
	 * \brief Determines if the cursor is on the mini-map drag line.
	 *
	 * \param _pCursorPos The cursor position in client coordinates.
	 * \param _i32DragX Holds the returned drag-line X coordinate if the hit-test succeeds.
	 * \return Returns true if the cursor is on the drag line.
	 */
	bool CHexEditorControl::MiniMapHitDragLine( const POINTS &_pCursorPos, int32_t &_i32DragX ) const {
		if ( !CurStyle()->bShowMiniMap ) { return false; }

		const lsw::LSW_RECT & rClient = ClientRect();
		const int32_t i32Left = MiniMapLeft( rClient );
		const int32_t i32Right = i32Left + CurStyle()->i32MiniMapWidthPx;

		const CMiniMap::MX_MINI_MAP & mmMiniMap = CurStyle()->mmMiniMap;

		// Drag bar is on the left when bRightSize is true; otherwise it is on the right.
		const int32_t i32DragX = mmMiniMap.bRightSize ? i32Left : i32Right;

		// A thin vertical hit zone.
		const int32_t i32Grip = 3;
		if ( _pCursorPos.x < (i32DragX - i32Grip) || _pCursorPos.x > (i32DragX + i32Grip) ) { return false; }
		if ( _pCursorPos.y < rClient.top || _pCursorPos.y > rClient.bottom ) { return false; }

		_i32DragX = i32DragX;
		return true;
	}

	/**
	 * \brief Snaps and clamps a proposed mini-map width.
	 *
	 * \param _i32Width The proposed width.
	 * \return Returns the snapped and clamped width.
	 */
	int32_t CHexEditorControl::MiniMapClampWidth( int32_t _i32Width ) const {
		const lsw::LSW_RECT & rClient = ClientRect();

		constexpr int32_t i32MinW = 1;
		const int32_t i32MaxW = std::max<int32_t>( rClient.Width() - 1, i32MinW );

		int32_t i32W = _i32Width;
		if ( i32W < i32MinW ) { i32W = i32MinW; }
		if ( i32W > i32MaxW ) { i32W = i32MaxW; }

		const CMiniMap::MX_MINI_MAP & mmMiniMap = CurStyle()->mmMiniMap;
		if ( mmMiniMap.bAutoSnap ) {
			// Note: ui32SnapLines is used as the snap granularity for width (pixels) here.
			const int32_t i32Snap = std::max<int32_t>( static_cast<int32_t>(mmMiniMap.ui32SnapLines), 1 );
			i32W = ((i32W + (i32Snap >> 1)) / i32Snap) * i32Snap;
			if ( i32W < i32MinW ) { i32W = i32MinW; }
			if ( i32W > i32MaxW ) { i32W = i32MaxW; }
		}

		return i32W + 1;
	}

	/**
	 * \brief Converts a cursor position to a mini-map source line and a byte offset within that line.
	 *
	 * \param _pCursorPos Cursor position in client coordinates.
	 * \param _ui64MiniLine Holds the returned mini-map source line.
	 * \param _ui32ByteOff Holds the returned byte offset within the mini-map row.
	 * \return Returns true if the cursor is inside the mini-map rectangle.
	 */
	bool CHexEditorControl::MiniMapCursorToLineByte( const POINTS &_pCursorPos, uint64_t &_ui64MiniLine, uint32_t &_ui32ByteOff ) const {
		lsw::LSW_RECT rMini;
		if ( !MiniMapRect( rMini ) ) { return false; }

		if ( _pCursorPos.x < rMini.left || _pCursorPos.x >= rMini.right ) { return false; }
		if ( _pCursorPos.y < rMini.top || _pCursorPos.y >= rMini.bottom ) { return false; }

		const CMiniMap::MX_MINI_MAP & mmMiniMap = CurStyle()->mmMiniMap;

		const uint32_t ui32MiniBpr = mmMiniMap.ui32BytesPerRow ? mmMiniMap.ui32BytesPerRow : 1U;
		const uint32_t ui32Zoom = mmMiniMap.ui32Zoom ? mmMiniMap.ui32Zoom : 1U;

		const uint64_t ui64TotalBytes = Size();
		if ( ui64TotalBytes == 0 ) { return false; }

		const int32_t i32DstW = rMini.Width();
		const int32_t i32DstH = rMini.Height();
		if ( i32DstW <= 0 || i32DstH <= 0 ) { return false; }

		// Source bitmap height in "mini-lines" for the current view.
		const uint64_t ui64SpanMiniLines = std::max( static_cast<uint64_t>(i32DstH) / static_cast<uint64_t>(ui32Zoom), 1ULL );

		const int32_t i32RelX = _pCursorPos.x - rMini.left;
		const int32_t i32RelY = _pCursorPos.y - rMini.top;

		// Map destination pixel to source line.
		uint64_t ui64Y = (static_cast<uint64_t>(i32RelY) * ui64SpanMiniLines) / static_cast<uint64_t>(i32DstH);
		if ( ui64Y >= ui64SpanMiniLines ) { ui64Y = ui64SpanMiniLines - 1ULL; }

		// Map destination pixel to source byte within row.
		uint64_t ui64X = (static_cast<uint64_t>(i32RelX) * static_cast<uint64_t>(ui32MiniBpr)) / static_cast<uint64_t>(i32DstW);
		if ( ui64X >= ui32MiniBpr ) { ui64X = ui32MiniBpr - 1ULL; }

		_ui64MiniLine = ui64Y;
		_ui32ByteOff = static_cast<uint32_t>( ui64X );
		return true;
	}

	/**
	 * \brief Converts a mini-map source line to the corresponding address in the file.
	 *
	 * \param _ui64MiniLine Source line in the mini-map bitmap.
	 * \return Returns the file address clamped to Size().
	 */
	uint64_t CHexEditorControl::MiniMapLineToAddress( uint64_t _ui64MiniLine ) const {
		const CMiniMap::MX_MINI_MAP & mmMiniMap = CurStyle()->mmMiniMap;

		uint32_t ui32MiniBpr = mmMiniMap.bAutoSnap ?
			std::max<uint32_t>( (CurStyle()->i32MiniMapWidthPx - 1) / (mmMiniMap.ui32SnapLines * mmMiniMap.ui32Zoom) * mmMiniMap.ui32SnapLines, mmMiniMap.ui32SnapLines ) :
			(mmMiniMap.ui32BytesPerRow);
		ui32MiniBpr = std::max( ui32MiniBpr, 1U );
		const uint64_t ui64TotalBytes = Size();

		uint64_t ui64Addr = m_mmsMiniMap.ui64Address + (_ui64MiniLine * static_cast<uint64_t>(ui32MiniBpr));
		if ( ui64Addr > ui64TotalBytes ) { ui64Addr = ui64TotalBytes; }
		return ui64Addr;
	}

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
	bool CHexEditorControl::MiniMapThumbMetrics( const lsw::LSW_RECT &_rClient,
		int32_t &_i32MiniLeft, int32_t &_i32MiniRight, int32_t &_i32MiniTop, int32_t &_i32MiniBottom,
		int32_t &_i32ThumbTop, int32_t &_i32ThumbHeight, int32_t &_i32Track, uint64_t &_ui64MaxFirstLine ) const {

		if ( !CurStyle()->bShowMiniMap ) { return false; }

		_i32MiniLeft = MiniMapLeft( _rClient );
		_i32MiniRight = _i32MiniLeft + CurStyle()->i32MiniMapWidthPx;
		_i32MiniTop = 0;
		_i32MiniBottom = _rClient.Height();

		const int32_t i32MiniHeight = _i32MiniBottom - _i32MiniTop;
		if ( i32MiniHeight <= 0 ) { return false; }

		const uint64_t ui64Lines = TotalLines_FixedWidth();
		if ( ui64Lines == 0 ) { return false; }

		const uint64_t ui64PageLines = (m_sdScrollView[m_eaEditAs].i32PageLines > 0) ?
			static_cast<uint64_t>(m_sdScrollView[m_eaEditAs].i32PageLines) : 1ULL;

		// Mini-map drag/overlay range: last line reaches the BOTTOM of the viewport.
		_ui64MaxFirstLine = 0;
		if ( ui64Lines > ui64PageLines ) {
			_ui64MaxFirstLine = ui64Lines - ui64PageLines;
		}

		// Thumb height proportional to visible lines. Clamp to [1..miniHeight].
		uint64_t ui64ThumbH = MiniMapMulDivU64Round( ui64PageLines, static_cast<uint64_t>(i32MiniHeight), ui64Lines );
		if ( ui64ThumbH < 1ULL ) { ui64ThumbH = 1ULL; }
		if ( ui64ThumbH > static_cast<uint64_t>(i32MiniHeight) ) { ui64ThumbH = static_cast<uint64_t>(i32MiniHeight); }
		_i32ThumbHeight = static_cast<int32_t>(ui64ThumbH);

		_i32Track = i32MiniHeight - _i32ThumbHeight;
		if ( _i32Track < 0 ) { _i32Track = 0; }

		const uint64_t ui64First = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine;

		if ( _ui64MaxFirstLine == 0ULL || _i32Track == 0 ) {
			_i32ThumbTop = _i32MiniTop;
		}
		else {
			uint64_t ui64TopPx = MiniMapMulDivU64Round( ui64First, static_cast<uint64_t>(_i32Track), _ui64MaxFirstLine );
			if ( ui64TopPx > static_cast<uint64_t>(_i32Track) ) { ui64TopPx = static_cast<uint64_t>(_i32Track); }
			_i32ThumbTop = _i32MiniTop + static_cast<int32_t>(ui64TopPx);
		}

		return true;
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

				iWidth = int32_t( ui32Digits ) * agGlyphs.i32DigitMaxCx;

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
	 * \param _iXLeft [out] Receives the pixel X of the groupÅfs left edge.
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


		// Choose per-area layout knobs.
		const uint32_t ui32Bpr			= stStyle.uiBytesPerRow ? stStyle.uiBytesPerRow : 16;

		const uint32_t ui32Stride		= StrideForFormat( _dfDataFmt );
		const uint32_t ui32GroupSz		= (stStyle.uiGroupSize && _dfDataFmt != MX_DF_CHAR && (stStyle.uiGroupSize >= ui32Stride && stStyle.uiGroupSize % ui32Stride == 0)) ? stStyle.uiGroupSize : ui32Stride;

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
	 * \param _iXLeft [out] Receives the pixel X of the groupÅfs left edge.
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

		const uint32_t ui32Stride = StrideForFormat( _dfDataFmt );
		const uint32_t ui32GroupSz	= (stStyle.uiGroupSize && _dfDataFmt != MX_DF_CHAR && (stStyle.uiGroupSize >= ui32Stride && stStyle.uiGroupSize % ui32Stride == 0)) ? stStyle.uiGroupSize : ui32Stride;
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

				const uint32_t ui32Bpr		= stStyle.uiBytesPerRow ? stStyle.uiBytesPerRow : 16;
				const uint32_t ui32Stride	= StrideForFormat( _dfDataFmt );
				const uint32_t ui32GroupSz	= (stStyle.uiGroupSize && _dfDataFmt != MX_DF_CHAR && (stStyle.uiGroupSize >= ui32Stride && stStyle.uiGroupSize % ui32Stride == 0)) ? stStyle.uiGroupSize : ui32Stride;

				const int32_t iCellCx		= CellWidthForFmt( _dfDataFmt );
				const int32_t iSpaceB		= int32_t( _dfDataFmt == MX_DF_CHAR ? 1 : stStyle.uiSpacesBetweenBytes ) * agGlyphs.i32SpaceCx;

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

		// Physical page = 1 for the same reason; weÅfll use client width for paging behavior.
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
		int32_t i32Sub = 0;
		if MX_UNLIKELY( _ptClient.y < GetRulerHeightPx() ) {
			m_sgSelGesture.bSelecting = false;
			m_sgSelGesture.bPendingThreshold = false;
			return;
		}
		if ( !PointToAddress( _ptClient, ui64Addr, i32Sub, bRightArea ) ) {
			m_sgSelGesture.bSelecting = false;
			m_sgSelGesture.bPendingThreshold = true;
			return;
		}
		

		m_sgSelGesture.bRightArea = bRightArea;

		if ( _bCtrlShift && m_sSel.bHas && m_sSel.smMode == MX_SM_NORMAL && NormalSelectionIsSingleRow() ) {
			ConvertSingleRowNormalToColumn();
			SelectionUpdateGesture( _ptClient, _bShift, _bCtrl, _bCtrlShift );
			return;
		}

		if ( _bShift && m_sSel.bHas && m_sSel.smMode == MX_SM_NORMAL ) {
			InitShiftExtendNormal( ui64Addr, _ptClient, _bShift, _bCtrl, _bCtrlShift );
			return;
		}
		if ( _bShift && m_sSel.bHas && m_sSel.smMode == MX_SM_COLUMN ) {
			InitShiftExtendColumn( ui64Addr, _ptClient, _bShift, _bCtrl, _bCtrlShift );
			return;
		}
		if ( _bShift ) {
			if ( m_sgSelGesture.smCurrent == MX_SM_NORMAL ) {
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

		auto psbStatus = reinterpret_cast<CDeusHexMachinaWindow *>(m_pwHexParent)->StatusBar();
		if ( psbStatus ) {
			psbStatus->SetTextW( L"" );
		}
		m_sgSelGesture.bSelecting = false;
		m_sgSelGesture.bPendingThreshold = true;
		m_sgSelGesture.ptDown = _ptClient;
		m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64CaretAddr = ui64Addr;
		m_sgSelGesture.i32CaretIdx = i32Sub;
		StartCaretBlink();

		m_sgSelGesture.smCurrent = _bCtrl ? MX_SM_COLUMN : (CurStyle()->bSelectColumnMode ? MX_SM_COLUMN : MX_SM_NORMAL);

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
		int32_t i32Sub = 0;
		if ( !PointToAddress( _ptClient, ui64Addr, i32Sub, bRightArea ) ) {
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

		auto ui32Stride = StrideForFormat( m_sgSelGesture.bRightArea ? CurStyle()->dfRightNumbersFmt : CurStyle()->dfLeftNumbersFmt );
		if ( m_sgSelGesture.smCurrent == MX_SM_NORMAL ) {
			if ( ui64Addr >= m_sgSelGesture.ui64MouseAnchorAddr ) {
				m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr;
				SetCaretAddr( ui64Addr + ui32Stride, 0, true, false );
			}
			else {
				m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr + ui32Stride;
				SetCaretAddr( ui64Addr, 0, true, false );
			}
		}
		else {
			if ( ui64Addr % CurStyle()->uiBytesPerRow >= m_sgSelGesture.ui64MouseAnchorAddr % CurStyle()->uiBytesPerRow ) {
				m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr;
				SetCaretAddr( ui64Addr + ui32Stride, 0, true, false );
			}
			else {
				m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr + ui32Stride;
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
	 * Adds the current selection to the Undo/Redo stack.
	 * 
	 * \param _sOldSelection The old selection being set.  Automatically saves the current selection.
	 * \param _sgOldGesture The previous caret and anchors.
	 * \return Returns true if the current selection was added to the selection Undo/Redo stack.
	 **/
	bool CHexEditorControl::AddSelectionUndo( const MX_SELECTION &_sOldSelection, const MX_SELECT_GESTURE &_sgOldGesture ) {
		try {
			MX_SELECT_UNDO_REDO_ITEM suriItem;
			suriItem.sRedoSelection = m_sSel;
			suriItem.i32RedoCaretIdx = m_sgSelGesture.i32CaretIdx;
			suriItem.ui64RedoAnchorAddr = m_sgSelGesture.ui64AnchorAddr;
			suriItem.ui64RedoCaretAddr = m_sgSelGesture.ui64CaretAddr;
			suriItem.ui64RedoMouseAnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr;

			suriItem.sUndoSelection = _sOldSelection;
			suriItem.i32UndoCaretIdx = _sgOldGesture.i32CaretIdx;
			suriItem.ui64UndoAnchorAddr = _sgOldGesture.ui64AnchorAddr;
			suriItem.ui64UndoCaretAddr = _sgOldGesture.ui64CaretAddr;
			suriItem.ui64UndoMouseAnchorAddr = _sgOldGesture.ui64MouseAnchorAddr;
			m_vSelectionStack.resize( m_sSelStackIdx + 2 );

			m_vSelectionStack[m_sSelStackIdx+1] = suriItem;
			++m_sSelStackIdx;
			return true;
		}
		catch ( ... ) { return false; }
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

		if ( !m_sSel.bHas || m_sSel.smMode != MX_SM_COLUMN ) { return; }

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
		m_sgSelGesture.smCurrent = MX_SM_COLUMN;

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
		if ( !m_sSel.bHas || m_sSel.smMode != MX_SM_NORMAL ) {
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

		m_sSel.smMode = MX_SM_COLUMN;
		m_sSel.sc.ui64AnchorAddr = ui64Row * ui32Bpr + ui32ColBeg;
		m_sSel.sc.ui32Cols       = ui32Cols;
		m_sSel.sc.ui64Lines      = 1;

		m_sgSelGesture.smCurrent = MX_SM_COLUMN;
	}

	/**
	 * \brief Returns true if the active Normal selection lies entirely on one logical row.
	 */
	bool CHexEditorControl::NormalSelectionIsSingleRow() const {
		if ( !m_sSel.bHas || m_sSel.smMode != MX_SM_NORMAL ) { return false; }

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

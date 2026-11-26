#include "MXHexEditorControl.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Layouts/MXUiIds.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../Utilities/MXUtilities.h"
#include "MXCopyPasteManager.h"
#include "MXHexEditorCurProcess.h"
#include "MXHexEditorProcess.h"

#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <Helpers/LSWHelpers.h>

#include <algorithm>
#include <cmath>
#include <cwchar>
#include <limits>


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
			CancelEdit();
			m_eaEditAs = _eaType;
				
			GoTo( m_pheiTarget->DefaultAddress() );

			RecalcAndInvalidate();
			if ( m_pwHexParent ) {
				m_pwHexParent->UpdateStatusBar_CharSet();
			}
		}
	}

	// Gets the view type as a string.
	CSecureWString CHexEditorControl::GetViewTypeAsString() const {
		switch ( m_eaEditAs ) {
			case MX_EA_TEXT : { return _DEC_WS_9BB908F9_Text; }
			case MX_EA_BINARY : { return _DEC_WS_C14CEC33_Binary; }
			case MX_EA_SCRIPT : { return _DEC_WS_1B2D820C_Script; }
			case MX_EA_TEMPLATE : { return _DEC_WS_6E167DD5_Template; }
			case MX_EA_EBCDIC : { return _DEC_WS_FC3807FC_EBCDIC; }
			case MX_EA_UTF16 : { return _DEC_WS_1C0C8DFC_Unicode; }
			case MX_EA_UTF8 : { return _DEC_WS_0E813C50_UTF_8; }
			case MX_EA_PROCESS : { return _DEC_WS_49A0210A_Process; }
			case MX_EA_CUR_PROCESS : { return _DEC_WS_49A0210A_Process; }
			case MX_EA_CODE : { return _DEC_WS_D7279FA6_Code; }
			case MX_EA_TAGGED : { return _DEC_WS_3BB39D03_Tagged; }
			default : { return _DEC_WS_C79C6386_Hex; }
		}
	}

	// Sets the character set.
	void CHexEditorControl::SetCharacterSet( CCharSets::MX_CHAR_SETS _csSet ) {
		if ( CurStyle()->csCharSet != _csSet ) {
			CancelEdit();
			CurStyle()->csCharSet = _csSet;

			RecalcAndInvalidate();
			if ( m_pwHexParent ) {
				m_pwHexParent->UpdateStatusBar_CharSet();
			}
		}
	}

	/**
	 * Gets the starting address of the selection, if there is one, otherwise returns the caret position.
	 * 
	 * \return Returns the starting address of the selection, if there is one.  Otherwise returns the caret position.
	 **/
	uint64_t CHexEditorControl::GetSelctStartOrCaret() const {
		if ( !HasSelection() ) { return GetCaretAddr(); }
		return m_sSel.smMode == MX_SM_NORMAL ? m_sSel.sn.ui64Start : m_sSel.sc.ui64AnchorAddr;
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
			
			if ( m_pwHexParent ) {
				LSW_SETREDRAW srRedraw( m_pwHexParent->StatusBar() );
				m_pwHexParent->SetStatusBarText( swsStatus.c_str(), false );
				if ( (m_ui8UpdateSelectCnt++ & 0x0F) == 0 ) {
					m_pwHexParent->UpdateStatusBar_PosValue_StartSize();
					m_tUpdateSelection.Stop();
				}
				else {
					m_tUpdateSelection.Start( Wnd(), MX_TI_HEX_UPDATE_STATUS, 16, nullptr );
				}
			}
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
				const uint32_t ui32LastCol = ui32AnchorCol + std::min( m_sSel.sc.ui32Cols, ui32Bpr );

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

		if ( m_sSel.smMode == MX_SM_NORMAL ) {
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
		else {
			auto uiBpr = CurStyle()->uiBytesPerRow;
			auto ui64Tmp = m_sSel.sc.ui64AnchorAddr;
			uint64_t ui64Right = ui64Tmp + std::min( uiBpr, m_sSel.sc.ui32Cols );
			if ( ui64Right % CurStyle()->uiBytesPerRow <= ui64Tmp % CurStyle()->uiBytesPerRow ) {
				uint64_t ui64ThisSize = (m_sSel.sc.ui64Lines * CurStyle()->uiBytesPerRow) + std::min( uiBpr, m_sSel.sc.ui32Cols );
				if ( m_sgSelGesture.ui64CaretAddr > ui64Tmp ) {
					m_sgSelGesture.ui64CaretAddr = ui64Tmp;
					m_sgSelGesture.ui64AnchorAddr = ui64Tmp + ui64ThisSize;
					m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64AnchorAddr - 1;
				}
				else {
					m_sgSelGesture.ui64CaretAddr = ui64Tmp + ui64ThisSize;
					m_sgSelGesture.ui64AnchorAddr = ui64Tmp;
					m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64AnchorAddr;
				}
				
				StartCaretBlink();
				EnsureVisible( m_sgSelGesture.ui64CaretAddr, true );
			}
			else {
				ui64Tmp = m_sgSelGesture.ui64AnchorAddr;
			
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
		}
		if ( m_pwHexParent ) {
			m_pwHexParent->UpdateStatusBar_PosValue_StartSize();
		}
	}

	/**
	 * Sets the caret to the beginning of the selection.
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 **/
	bool CHexEditorControl::MarkSelectionStart( CSecureWString &_swsStatus ) {
		try {
			_swsStatus = _DEC_WS_59D5AB36_Marked_start_of_selection_;
		}
		catch ( ... ) {}
		if ( !m_sSel.HasSelection() ) { return true; }

		if ( m_sgSelGesture.ui64CaretAddr > m_sgSelGesture.ui64AnchorAddr ) {
			ReverseSelection();
		}
		return true;
	}

	/**
	 * Sets the caret to the end of the selection.
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 **/
	bool CHexEditorControl::MarkSelectionEnd( CSecureWString &_swsStatus ) {
		try {
			_swsStatus = _DEC_WS_06516F57_Marked_end_of_selection_;
		}
		catch ( ... ) {}
		if ( !m_sSel.HasSelection() ) { return true; }

		if ( m_sgSelGesture.ui64CaretAddr < m_sgSelGesture.ui64AnchorAddr ) {
			ReverseSelection();
		}
		return true;
	}

	/**
	 * Moves the selection back.
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 **/
	bool CHexEditorControl::MoveSelectionBack( CSecureWString &_swsStatus ) {
		if ( !m_sSel.HasSelection() ) { return true; }
		
		uint64_t uiAddr;
		if ( m_sSel.smMode == MX_SM_NORMAL ) {
			uint64_t ui64Len = m_sSel.sn.ui64End - m_sSel.sn.ui64Start;
			if ( ui64Len >= m_sSel.sn.ui64Start ) {
				m_sSel.sn.ui64Start = 0;
			}
			else {
				m_sSel.sn.ui64Start -= ui64Len;
			}
			m_sSel.sn.ui64End = m_sSel.sn.ui64Start + ui64Len;
			m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sSel.sn.ui64Start;
			m_sgSelGesture.ui64CaretAddr = m_sSel.sn.ui64End;

			uiAddr = m_sSel.sn.ui64Start;
		}
		else {
			auto uiBpr = CurStyle()->uiBytesPerRow;
			if ( (m_sSel.sc.ui64AnchorAddr <= std::min( uiBpr, m_sSel.sc.ui32Cols )) ) {
				m_sSel.sc.ui64AnchorAddr = 0;
			}
			else {
				m_sSel.sc.ui64AnchorAddr -= std::min( uiBpr, m_sSel.sc.ui32Cols );
			}

			uint64_t ui64ThisSize = (m_sSel.sc.ui64Lines * CurStyle()->uiBytesPerRow) + std::min( uiBpr, m_sSel.sc.ui32Cols );
			m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sSel.sc.ui64AnchorAddr;
			m_sgSelGesture.ui64CaretAddr = m_sgSelGesture.ui64AnchorAddr + ui64ThisSize;

			uiAddr = m_sSel.sc.ui64AnchorAddr;
		}

		GoTo( uiAddr, false, true );

		try {
			_swsStatus = _DEC_WS_3843E4C1_Moved_selection_to_address_;
			std::wstring wsTmp;
			_swsStatus += std::format( L"{} [{}]", uiAddr, CUtilities::ToHex( uiAddr, wsTmp, 0 ) );

			if ( m_pwHexParent ) {
				m_pwHexParent->UpdateStatusBar_PosValue_StartSize();
			}
		}
		catch ( ... ) { return false; }
		return true;
	}

	/**
	 * Moves the selection forward.
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 **/
	bool CHexEditorControl::MoveSelectionForward( CSecureWString &_swsStatus ) {
		if ( !m_sSel.HasSelection() ) { return true; }
		
		uint64_t uiAddr;
		auto ui64Size = Size();
		if ( m_sSel.smMode == MX_SM_NORMAL ) {
			uint64_t ui64Len = m_sSel.sn.ui64End - m_sSel.sn.ui64Start;
			if ( ui64Len >= ui64Size - m_sSel.sn.ui64End ) {
				m_sSel.sn.ui64End = ui64Size;
			}
			else {
				m_sSel.sn.ui64End += ui64Len;
			}
			m_sSel.sn.ui64Start = m_sSel.sn.ui64End - ui64Len;
			m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sSel.sn.ui64Start;
			m_sgSelGesture.ui64CaretAddr = m_sSel.sn.ui64End;

			uiAddr = m_sSel.sn.ui64Start;
		}
		else {
			auto uiBpr = CurStyle()->uiBytesPerRow;
			uint64_t ui64ThisSize = (m_sSel.sc.ui64Lines * uiBpr) + std::min( uiBpr, m_sSel.sc.ui32Cols );
			if ( (m_sSel.sc.ui64AnchorAddr + std::min( uiBpr, m_sSel.sc.ui32Cols )) >= ui64Size - ui64ThisSize ) {
				m_sSel.sc.ui64AnchorAddr = ui64Size - ui64ThisSize;
			}
			else {
				m_sSel.sc.ui64AnchorAddr += std::min( uiBpr, m_sSel.sc.ui32Cols );
			}

			m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sSel.sc.ui64AnchorAddr;
			m_sgSelGesture.ui64CaretAddr = m_sgSelGesture.ui64AnchorAddr + ui64ThisSize;

			uiAddr = m_sgSelGesture.ui64CaretAddr;
		}

		GoTo( uiAddr, false, true );

		try {
			_swsStatus = _DEC_WS_3843E4C1_Moved_selection_to_address_;
			std::wstring wsTmp;
			_swsStatus += std::format( L"{} [{}]", uiAddr, CUtilities::ToHex( uiAddr, wsTmp, 0 ) );

			if ( m_pwHexParent ) {
				m_pwHexParent->UpdateStatusBar_PosValue_StartSize();
			}
		}
		catch ( ... ) { return false; }
		return true;
	}

	/**
	 * Performs a Cut operation.
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 * \return Returns false if the message should be treated as a warning.
	 **/
	bool CHexEditorControl::Cut( CSecureWString &_swsStatus ) {
		try {
			CancelEdit();
			FlushRealTimeUndo();
			if ( !m_pheiTarget ) {
				_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
				return false;
			}
			if ( m_pheiTarget->ReadOnly() ) {
				_swsStatus = _DEC_WS_D4FFE2E0_File_is_Read_Only;
				return false;
			}
			if ( !m_sSel.HasSelection() ) { return true; }
			/*
			std::vector<MX_SEL_RANGE> vSelections;
			constexpr uint64_t ui64GroupSize = 1024ULL * 1024ULL;
			uint64_t ui64Idx = 0;
			std::vector<uint8_t> vClip;
			{
				CHexEditorInterface::CBuffer bTmp;
				do {
					if ( m_sSel.GatherSelected_LowToHigh( CurStyle()->uiBytesPerRow, vSelections, ui64GroupSize, ui64Idx++ ) ) {
						for ( const auto & srThis : vSelections ) {
							size_t sBufSize;
							for ( uint64_t I = 0; I < srThis.ui64Size; I += sBufSize ) {
								sBufSize = size_t( std::min<uint64_t>( srThis.ui64Size - I, MAXSIZE_T ) );
								if ( !m_pheiTarget->Read( srThis.ui64Start, bTmp, sBufSize ) ) {
								}
								const uint8_t * pData = reinterpret_cast<const uint8_t *>(bTmp.data());
								vClip.insert( vClip.end(), pData, pData + sBufSize );
							}
						}
					}
				} while ( vSelections.size() );
			}

			LSW_CLIPBOARD cbClip( Wnd(), true );
			UINT uiId = ::RegisterClipboardFormatW( L"BinaryData" );
			if ( !cbClip.SetData( uiId, vClip ) ) {
				// FUCK!!!
			}*/
			MX_COPYAS caCopyData = CopyParms( MX_CA_BINARY );
			if ( !m_pheiTarget->Copy( m_sSel, CurStyle()->uiBytesPerRow, CCopyPasteManager::ActiveClipboard(), caCopyData, _swsStatus ) ) { return false; }

			auto ui64TotalSel = m_sSel.TotalSelected( CurStyle()->uiBytesPerRow );
			if ( !DeleteSelectedOrCaret( _swsStatus ) ) {
				return false;
			}

			_swsStatus = _DEC_WS_C8AFBB17_Cut__;
			std::wstring wsTmp;
			_swsStatus += std::format( L"{} [{}]", ui64TotalSel, CUtilities::ToHex( ui64TotalSel, wsTmp, 0 ) );
			if ( ui64TotalSel != 1 ) {
				_swsStatus += _DEC_WS_C1D51046__bytes;
			}
			else {
				_swsStatus += _DEC_WS_BB679D6B__byte;
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Performs a Copy operation.
	 * 
	 * \param _caFormat The Copy format.
	 * \param _wsStatus The string to print in the status bar.
	 * \return Returns false if the message should be treated as a warning.
	 **/
	bool CHexEditorControl::CopyAs( MX_COPY_AS _caFormat, CSecureWString &_swsStatus ) {
		try {
			if ( !m_pheiTarget ) {
				_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
				return false;
			}
			// Copying does not modify the source, so read-only sources can be copied.
			if ( !m_sSel.HasSelection() ) { return true; }

			MX_COPYAS caCopyData = CopyParms( _caFormat );
			if ( !m_pheiTarget->Copy( m_sSel, CurStyle()->uiBytesPerRow, CCopyPasteManager::ActiveClipboard(), caCopyData, _swsStatus ) ) { return false; }

			auto ui64TotalSel = m_sSel.TotalSelected( CurStyle()->uiBytesPerRow );

			_swsStatus = _DEC_WS_7D2FA4EA_Copied__;
			std::wstring wsTmp;
			_swsStatus += std::format( L"{} [{}]", ui64TotalSel, CUtilities::ToHex( ui64TotalSel, wsTmp, 0 ) );
			if ( ui64TotalSel != 1 ) {
				_swsStatus += _DEC_WS_C1D51046__bytes;
			}
			else {
				_swsStatus += _DEC_WS_BB679D6B__byte;
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Gathers the parameters of a Copy operation from the current view: the format, the left/right areas, the character set, the
	 *	endianness, and the layout and address-gutter settings needed to reproduce the view later (deferred rendering may happen
	 *	after this control is gone).
	 * 
	 * \param _caFormat The Copy format.
	 * \return Returns the Copy parameters.
	 **/
	MX_COPYAS CHexEditorControl::CopyParms( MX_COPY_AS _caFormat ) const {
		const MX_STYLE &stStyle = (*CurStyle());
		MX_COPYAS caCopyData;
		caCopyData.caCopyFmt = _caFormat;
		caCopyData.dfLeft = stStyle.dfLeftNumbersFmt;
		caCopyData.dfRight = stStyle.dfRightNumbersFmt;
		caCopyData.bBigEndian = IsBigEndian();
		caCopyData.csCharSet = stStyle.csCharSet;

		caCopyData.ui32FloatDigits = stStyle.ui32FloatDigits;
		caCopyData.bShowAddress = stStyle.bShowAddressGutter;
		caCopyData.bShowLeft = stStyle.bShowLeftNumbers;
		caCopyData.bShowRight = stStyle.bShowRightArea;
		caCopyData.ui32GroupSize = stStyle.uiGroupSize;
		caCopyData.ui32SpacesBetweenGroups = stStyle.uiSpacesBetweenBytes;

		const MX_ADDR_STYLE &asAddr = stStyle.daAddressStyle;
		caCopyData.caAddress.ui64Bias = m_sdScrollView[m_eaEditAs].ui64StartAddress;
		caCopyData.caAddress.ui32Format = static_cast<uint32_t>(asAddr.afFormat);
		caCopyData.caAddress.ui32MinDigits = MinAddrDigits();
		caCopyData.caAddress.bLowercaseHex = asAddr.bLowercaseHex;
		caCopyData.caAddress.bShowColonIn = asAddr.bShowColonIn;
		caCopyData.caAddress.bShowColonAfter = asAddr.bShowColonAfter;
		caCopyData.caAddress.bShowTypeSpec = asAddr.bShowTypeSpec;
		caCopyData.caAddress.bUseShortSuffixW = asAddr.bUseShortSuffixW;
		return caCopyData;
	}

	/**
	 * Performs a Paste operation from the active clipboard.  Any selection is deleted first, then the clipboard data is inserted
	 *	(Insert mode) or written over the following bytes (Overwrite mode) at the start of the selection or the caret.  The caret
	 *	ends up after the pasted data.  The whole operation is one Undo step.
	 * 
	 * \param _swsStatus The string to print in the status bar.
	 * \return Returns false if the message should be treated as a warning.
	 **/
	bool CHexEditorControl::Paste( CSecureWString &_swsStatus ) {
		try {
			CancelEdit();
			FlushRealTimeUndo();
			if ( !m_pheiTarget ) {
				_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
				return false;
			}
			if ( m_pheiTarget->ReadOnly() ) {
				_swsStatus = _DEC_WS_D4FFE2E0_File_is_Read_Only;
				return false;
			}
			if ( !m_pheiTarget->CanPaste() ) {
				_swsStatus = _DEC_WS_0388A331_Pasting_is_not_supported_for_this_data_source_;
				return false;
			}

			// Resolve the data first so that nothing is changed when there is nothing to paste.
			MX_PASTE_DATA pdData;
			if ( !CCopyPasteManager::GetPasteData( CCopyPasteManager::ActiveClipboard(), CurStyle()->csCharSet, pdData, _swsStatus ) ) { return false; }

			const bool bOverwrite = IsOverwriteMode() || m_pheiTarget->OverwriteOnly();
			const MX_SELECTION sOldSel = m_sSel;
			const MX_SELECT_GESTURE sgOldGesture = m_sgSelGesture;
			// Deleting the selection (highest ranges first) leaves its lowest address where the data goes.
			const uint64_t ui64Addr = m_sSel.HasSelection() ? m_sSel.LowerLeftAddress() : m_sgSelGesture.ui64CaretAddr;
			uint64_t ui64Pasted = 0;
			bool bOk = true;
			{
				CHexEditorInterface::MX_UNDO uUndo( m_pheiTarget );
				if ( !uUndo.Ready() ) {
					_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
					return false;
				}
				if ( m_sSel.HasSelection() ) {
					std::vector<MX_SEL_RANGE> vSelections;
					constexpr uint64_t ui64GroupSize = 1024ULL * 1024ULL;
					uint64_t ui64Idx = 0;
					do {
						if ( !m_sSel.GatherSelected_HighToLow( CurStyle()->uiBytesPerRow, vSelections, ui64GroupSize, ui64Idx++ ) ) {
							bOk = false;
							break;
						}
						for ( const auto &srThis : vSelections ) {
							uint64_t ui64Deleted = 0;
							if ( !m_pheiTarget->Delete( srThis.ui64Start, srThis.ui64Size, ui64Deleted ) ) {
								bOk = false;
								break;
							}
							ShiftChanges( srThis.ui64Start, ui64Deleted, false );
						}
					} while ( bOk && vSelections.size() );
					if ( !bOk ) { _swsStatus = _DEC_WS_1468A1DF_Internal_error_; }
				}
				if ( bOk ) {
					bOk = m_pheiTarget->Paste( ui64Addr, pdData, bOverwrite, ui64Pasted, _swsStatus );
					// The pasted bytes are highlighted as changed.
					if ( bOverwrite ) { MarkChanged( ui64Addr, ui64Pasted ); }
					else { ShiftChanges( ui64Addr, ui64Pasted, true ); }
				}
			}

			// One selection-Undo item per Undo group, even if the paste failed part-way (the group is still on the stack).
			SetCaretAddr( ui64Addr + ui64Pasted );
			AddSelectionUndo( sOldSel, sgOldGesture );
			EnsureVisible( m_sgSelGesture.ui64CaretAddr, FALSE );
			::InvalidateRect( Wnd(), nullptr, FALSE );
			if ( !bOk ) { return false; }

			_swsStatus = _DEC_WS_C1C0C8E2_Pasted__;
			std::wstring wsTmp;
			_swsStatus += std::format( L"{} [{}]", ui64Pasted, CUtilities::ToHex( ui64Pasted, wsTmp, 0 ) );
			if ( ui64Pasted != 1 ) {
				_swsStatus += _DEC_WS_C1D51046__bytes;
			}
			else {
				_swsStatus += _DEC_WS_BB679D6B__byte;
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Determines whether a Paste operation can be performed now.
	 * 
	 * \return Returns true if the data source supports pasting, is writable, and the active clipboard has data.
	 **/
	bool CHexEditorControl::CanPaste() const {
		return m_pheiTarget && !m_pheiTarget->ReadOnly() && m_pheiTarget->CanPaste() &&
			CCopyPasteManager::HasPasteData( CCopyPasteManager::ActiveClipboard() );
	}

	/**
	 * Are we in OVR mode?
	 * 
	 * \return Returns true if in Overwrite mode.
	 **/
	bool CHexEditorControl::IsOverwriteMode() const {
		if ( m_pwHexParent ) { return m_pwHexParent->IsOverwriteMode(); }
		return true;
	}

	/**
	 * Gets the character-set Status-Bar string.
	 * 
	 * \return Returns the Status Bar string for the given character set.
	 **/
	const CSecureString CHexEditorControl::CharSetStatusBarCode() const {
		return mx::CStringDecoder::DecodeToString( CCharSets::m_csSets[CurStyle()->csCharSet].pcStatusBar, CCharSets::m_csSets[CurStyle()->csCharSet].sStatusLen );
	}

	/**
	 * Performs an Undo operation.
	 * 
	 * \param _wsStatus The string to print in the status bar.
	 **/
	bool CHexEditorControl::Undo( CSecureWString &_swsStatus ) {
		// Undoing while the edit box is open discards the edit.
		if ( IsEditing() ) {
			CancelEdit();
			return true;
		}
		FlushRealTimeUndo();
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
		CancelEdit();
		FlushRealTimeUndo();
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
			CancelEdit();
			FlushRealTimeUndo();
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
						ShiftChanges( m_sgSelGesture.ui64CaretAddr, ui64Deleted, false );
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
			{
				CHexEditorInterface::MX_UNDO uUndo( m_pheiTarget );
				std::vector<MX_SEL_RANGE> vSelections;
				if ( uUndo.Ready() ) {
					constexpr uint64_t ui64GroupSize = 1024ULL * 1024ULL;
					uint64_t ui64Idx = 0;
					do {
						if ( m_sSel.GatherSelected_HighToLow( CurStyle()->uiBytesPerRow, vSelections, ui64GroupSize, ui64Idx++ ) ) {
				
							for ( const auto & srThis : vSelections ) {
								if ( m_pheiTarget->Delete( srThis.ui64Start, srThis.ui64Size, ui64Deleted ) ) {
									ShiftChanges( srThis.ui64Start, ui64Deleted, false );
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
							if ( ui64Caret != m_sgSelGesture.ui64CaretAddr ) {
								auto sOldSel = m_sSel;
								auto sgOldGesture = m_sgSelGesture;
								SetCaretAddr( ui64Caret );
								AddSelectionUndo( sOldSel, sgOldGesture );
							}
							_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
							return false;
						}
					} while ( vSelections.size() );
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
			CancelEdit();
			FlushRealTimeUndo();
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
						ShiftChanges( m_sgSelGesture.ui64CaretAddr - 1, ui64Deleted, false );
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
	 * \param _bOnlyIfNotVisible If true, the view is only moved if the value is not currently in the visible area.
	 * \param _bRefresh If true, the view is updated.
	 **/
	void CHexEditorControl::GoTo( uint64_t _ui64Addr, bool _bShowAtTop, bool _bOnlyIfNotVisible, bool _bRefresh ) {
		_ui64Addr = std::min( _ui64Addr, Size() );


		// Compute visible page sizes (lines / columns-on-screen).
		int32_t iClientH = int32_t( ClientRect().Height() );

		const int32_t iLineAdv = LineAdvanceCy();
		m_sdScrollView[m_eaEditAs].i32PageLines = (iLineAdv ? (iClientH / iLineAdv) : 0);
		if ( m_sdScrollView[m_eaEditAs].i32PageLines < 1 ) { m_sdScrollView[m_eaEditAs].i32PageLines = 1; }

		uint64_t ui64Pos = _ui64Addr / CurStyle()->uiBytesPerRow;
		if ( _bOnlyIfNotVisible ) {
			if ( ui64Pos >= m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine &&
				ui64Pos < m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine + m_sdScrollView[m_eaEditAs].i32PageLines ) {
				if ( _bRefresh ) {
					::InvalidateRect( Wnd(), nullptr, FALSE );
				}
				return;
			}
		}


		const uint64_t ui64Lines = TotalLines_FixedWidth();
		const uint64_t ui64MaxV  = ui64Lines ? (ui64Lines - 1ULL) : 0ULL;

		
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
		if ( _bRefresh ) {
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}
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

	/**
	 * Reads from a given address in the hew control.
	 * 
	 * \param _ui64Addr The address from which to read.
	 * \param _vReturn Holds the read values.
	 * \param _sSize The number of bytes to read.
	 * \return Returns true if all bytes were read.
	 **/
	bool CHexEditorControl::Read( uint64_t _ui64Addr, std::vector<uint8_t> &_vReturn, size_t _sSize ) {
		if MX_UNLIKELY( !m_pheiTarget ) { return false; }
		if MX_UNLIKELY( UINT64_MAX - _sSize < _ui64Addr ) { return false; }
		if MX_UNLIKELY( !m_pheiTarget->Read( _ui64Addr, _vReturn, _sSize ) ) { return false; }
		return _vReturn.size() == _sSize;
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
			const int32_t i32Off = MainViewOffsetX();
			if ( i32Off ) {
				// The Mini-Map is on the left: the main view is drawn shifted to its right, then the Mini-Map is drawn.
				lsw::LSW_RECT rGap = rRect;
				rGap.left = MiniMapBarX( rRect );
				rGap.right = std::min<LONG>( rRect.right, rRect.left + i32Off );
				if ( rGap.right > rGap.left ) {
					::FillRect( hDcMem, &rGap, lsw::CBase::BrushCache().Brush( MX_GetRgbValue( BackColors()->crEditor ) ) );
				}

				POINT ptOrg;
				::SetViewportOrgEx( hDcMem, i32Off, 0, &ptOrg );
				lsw::LSW_RECT rMain = rRect;
				rMain.right = std::max<LONG>( rMain.left, rMain.right - i32Off );
				PaintHex( hDcMem, rMain, false );
				::SetViewportOrgEx( hDcMem, ptOrg.x, ptOrg.y, nullptr );
				DrawMiniMap( hDcMem, MiniMapBarX( rRect ), 0, rRect );
			}
			else {
				PaintHex( hDcMem, rRect );
			}


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
		else if ( _uiptrId == m_tUpdateSelection.uiptrActiveId ) {
			m_tUpdateSelection.Stop();
			if ( m_pwHexParent ) {
				m_pwHexParent->UpdateStatusBar_PosValue_StartSize();
			}
		}
		else if ( _uiptrId == m_tRtUndo.uiptrActiveId ) {
			// Typing stopped: the real-time edits so far become one Undo item.
			FlushRealTimeUndo();
		}
		else if ( _uiptrId == m_tWatch.uiptrActiveId ) {
			WatchForChanges();
		}
		return lsw::CWidget::LSW_H_HANDLED;
	}

	// WM_LBUTTONDOWN.
	lsw::CWidget::LSW_HANDLED CHexEditorControl::LButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		this->CWidget::SetFocus();
		// Clicking away from the edit box discards it (Enter writes it), and ends any real-time Undo item.
		CancelEdit();
		FlushRealTimeUndo();

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
				double dPxPerLine;
				uint64_t ui64MaxFirstLine;
				bool bExact;
				if ( MiniMapViewBoxModel( dPxPerLine, ui64MaxFirstLine, bExact ) ) {
					const uint64_t ui64First = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine;
					m_mmsMiniMap.bDraggingScroll = true;
					m_mmsMiniMap.bDragExact = bExact;
					m_mmsMiniMap.i32DragStartY = _pCursorPos.y;
					m_mmsMiniMap.ui64DragStartFirstLine = ui64First;
					m_mmsMiniMap.ui64DragMaxFirstLine = ui64MaxFirstLine;
					m_mmsMiniMap.dDragPxPerLine = dPxPerLine;
					m_mmsMiniMap.dDragGrabOffset = double( _pCursorPos.y - m_mmsMiniMap.i32DrawnTop ) - MiniMapViewBoxTopPx( ui64First );

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
				const uint64_t ui64MaxFirstLine = m_mmsMiniMap.ui64DragMaxFirstLine;
				const uint64_t ui64StartFirst = std::min( m_mmsMiniMap.ui64DragStartFirstLine, ui64MaxFirstLine );

				if ( ui64MaxFirstLine > 0 && m_mmsMiniMap.dDragPxPerLine > 0.0 ) {
					// Estimate from the box's linear rate of movement.
					double dDelta = double( _pCursorPos.y - m_mmsMiniMap.i32DragStartY ) / m_mmsMiniMap.dDragPxPerLine;
					dDelta = std::clamp( dDelta, -double( ui64StartFirst ), double( ui64MaxFirstLine - ui64StartFirst ) );
					uint64_t ui64NewFirst;
					if ( dDelta < 0.0 ) {
						ui64NewFirst = ui64StartFirst - std::min( ui64StartFirst, static_cast<uint64_t>(std::llround( -dDelta )) );
					}
					else {
						ui64NewFirst = ui64StartFirst + std::min( ui64MaxFirstLine - ui64StartFirst, static_cast<uint64_t>(std::llround( dDelta )) );
					}

					if ( m_mmsMiniMap.bDragExact ) {
						// The content only moves by whole mini-map rows, so refine against the box's drawn position: pick the nearby line
						//	that puts the box top closest to where the grab point says it belongs.
						const double dTarget = double( _pCursorPos.y - m_mmsMiniMap.i32DrawnTop ) - m_mmsMiniMap.dDragGrabOffset;
						const uint64_t ui64MainBpr = CurStyle()->uiBytesPerRow ? CurStyle()->uiBytesPerRow : 1ULL;
						const uint64_t ui64MiniBpr = m_mmsMiniMap.ui32DrawnBytesPerRow ? m_mmsMiniMap.ui32DrawnBytesPerRow : 1ULL;
						const uint64_t ui64Window = 4ULL * ((ui64MiniBpr + ui64MainBpr - 1ULL) / ui64MainBpr) + 4ULL;
						const uint64_t ui64Lo = ui64NewFirst - std::min( ui64NewFirst, ui64Window );
						const uint64_t ui64Hi = ui64NewFirst + std::min( ui64MaxFirstLine - ui64NewFirst, ui64Window );
						const uint64_t ui64Estimate = ui64NewFirst;
						double dBestErr = -1.0;
						for ( uint64_t I = ui64Lo; I <= ui64Hi; ++I ) {
							const double dErr = std::abs( MiniMapViewBoxTopPx( I ) - dTarget );
							const uint64_t ui64Dist = (I > ui64Estimate) ? (I - ui64Estimate) : (ui64Estimate - I);
							const uint64_t ui64BestDist = (ui64NewFirst > ui64Estimate) ? (ui64NewFirst - ui64Estimate) : (ui64Estimate - ui64NewFirst);
							if ( dBestErr < 0.0 || dErr < dBestErr || (dErr == dBestErr && ui64Dist < ui64BestDist) ) {
								dBestErr = dErr;
								ui64NewFirst = I;
							}
						}
					}

					if ( m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine != ui64NewFirst ) {
						m_sdScrollView[m_eaEditAs].ui64VPos = ui64NewFirst;
						m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine = ui64NewFirst;

						const uint64_t ui64Lines = TotalLines_FixedWidth();
						const uint64_t ui64MaxV = ui64Lines ? (ui64Lines - 1ULL) : 0ULL;
						lsw::CBase::SetScrollInfo64( Wnd(), SB_VERT, SIF_POS, ui64MaxV, m_sdScrollView[m_eaEditAs].ui64VPos, 1, TRUE );

						::InvalidateRect( Wnd(), NULL, FALSE );
					}
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

		// A new key means the character Windows produces for an Alt code above 255 has either arrived or will not.
		m_wcAltSwallow = 0;
		m_c32AltEmitted = 0;

		if ( IsEditing() ) {
			switch ( _uiKeyCode ) {
				case VK_RETURN : {
					CSecureWString swsStatus;
					if ( !CommitEdit( swsStatus ) ) { EditWarning( swsStatus ); }
					return lsw::CWidget::LSW_H_HANDLED;
				}
				case VK_ESCAPE : {
					CancelEdit();
					return lsw::CWidget::LSW_H_HANDLED;
				}
				case VK_BACK : {
					EditBackspace();
					return lsw::CWidget::LSW_H_HANDLED;
				}
				case VK_DELETE : {
					EditDelete();
					return lsw::CWidget::LSW_H_HANDLED;
				}
				// The arrow keys, Home and End move within the edit box (Tab, Space and Shift+Tab arrive as characters).
				case VK_LEFT : { EditNavigate( MX_EN_LEFT ); return lsw::CWidget::LSW_H_HANDLED; }
				case VK_RIGHT : { EditNavigate( MX_EN_RIGHT ); return lsw::CWidget::LSW_H_HANDLED; }
				case VK_UP : { EditNavigate( MX_EN_UP ); return lsw::CWidget::LSW_H_HANDLED; }
				case VK_DOWN : { EditNavigate( MX_EN_DOWN ); return lsw::CWidget::LSW_H_HANDLED; }
				case VK_HOME : { EditNavigate( MX_EN_HOME ); return lsw::CWidget::LSW_H_HANDLED; }
				case VK_END : { EditNavigate( MX_EN_END ); return lsw::CWidget::LSW_H_HANDLED; }
				case VK_INSERT : { break; }		// Applies to the next edit.
				default : {
					// Anything else (Page Up/Down, etc.) would leave the edit box behind.
					return lsw::CWidget::LSW_H_HANDLED;
				}
			}
		}
		else {
			switch ( _uiKeyCode ) {
				case VK_UP : {}							MX_FALLTHROUGH
				case VK_DOWN : {}						MX_FALLTHROUGH
				case VK_PRIOR : {}						MX_FALLTHROUGH
				case VK_NEXT : {}						MX_FALLTHROUGH
				case VK_HOME : {}						MX_FALLTHROUGH
				case VK_END : {}						MX_FALLTHROUGH
				case VK_LEFT : {}						MX_FALLTHROUGH
				case VK_RIGHT : {
					// Real-time edits after moving the caret are a separate Undo item.
					FlushRealTimeUndo();
					break;
				}
				default : {}
			}
		}

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
				if ( m_pwHexParent ) {
					m_pwHexParent->DeletePriorToCaret();
				}
				else {
					CSecureWString wsMsg;
					DeletePriorToCaret( wsMsg );
				}
				break;
			}
			case VK_DELETE : {
				if ( m_pwHexParent ) {
					m_pwHexParent->DeleteSelectedOrCaret();
				}
				else {
					CSecureWString wsMsg;
					DeleteSelectedOrCaret( wsMsg );
				}
				break;
			}
			case VK_INSERT : {
				if ( m_pwHexParent ) {
					m_pwHexParent->ToggleOverwriteInsert();
				}
				break;
			}
		}


		return lsw::CWidget::LSW_H_HANDLED;
	}

	/**
	 * \brief Receives committed text input as a Unicode code point.
	 *
	 * \param _c32Cp The committed Unicode code point.
	 * \param _uiRepeat The repeat count (normally 1).
	 * \return Returns LSW_H_HANDLED if consumed.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::OnTextInputCodePoint( char32_t _c32Cp, uint32_t _uiRepeat ) {
		if ( m_wcAltSwallow ) {
			// The character Windows produced for an Alt code above 255 (which SysKeyUp() already typed correctly).
			const wchar_t wcSwallow = m_wcAltSwallow;
			m_wcAltSwallow = 0;
			m_c32AltEmitted = 0;
			if ( _c32Cp == char32_t( wcSwallow ) ) { return lsw::CWidget::LSW_H_HANDLED; }
		}
		TypeCodePoint( _c32Cp, _uiRepeat );
		return lsw::CWidget::LSW_H_HANDLED;
	}

	/**
	 * The WM_KEYUP handler.  Alt can be released as a WM_KEYUP rather than a WM_SYSKEYUP after typing an Alt code.
	 *
	 * \param _uiKeyCode The virtual-key code.
	 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
	 * \return Returns an LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::KeyUp( UINT _uiKeyCode, UINT _uiFlags ) {
		if ( _uiKeyCode == VK_MENU ) { return SysKeyUp( _uiKeyCode, _uiFlags ); }
		return lsw::CWidget::LSW_H_CONTINUE;
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
	void CHexEditorControl::SetStream( CHexEditorInterface * _pediStream, CHexEditorControlHost * _pwMainCtrl ) {
		// Finish with the old stream first.
		CancelEdit();
		FlushRealTimeUndo();
		m_pwHexParent = _pwMainCtrl;
		m_pheiTarget = _pediStream;

		// Changes are tracked per data source.
		m_mChanges.clear();
		m_bWatchValid = false;
		m_ui32WatchPeriod = MX_WATCH_SLOW_MS;
		StartWatch();

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
		// The main view is shifted right when the Mini-Map is on the left.
		const POINT ptPoint = { _pPoint.x - MainViewOffsetX(), _pPoint.y };
		uint64_t ui64Max = Size();
		if MX_UNLIKELY( ui64Max == 0 ) { return false; }
		if MX_LIKELY( m_pheiTarget ) {
			const MX_STYLE & stAll = (*CurStyle());
			const int32_t iGutterW = ComputeAddressGutterWidthPx();

			int32_t i32Top = GetRulerHeightPx();
			//if MX_UNLIKELY( ptPoint.y < i32Top ) { return false; }
			if MX_UNLIKELY( ptPoint.x < iGutterW ) { return false; }

			POINT pThis = { ptPoint.x - iGutterW, ptPoint.y - i32Top };


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
	bool CHexEditorControl::PaintHex( HDC _hDc, const lsw::LSW_RECT &_rRect, bool _bDrawMiniMap ) {
		const MX_STYLE & stAll = (*CurStyle());
		const int32_t iGutterW = ComputeAddressGutterWidthPx();
		lsw::LSW_RECT rTmp = _rRect;
		int32_t iX = iGutterW - m_sdScrollView[m_eaEditAs].ui64HPx, iY = 0;
		lsw::LSW_RECT rBlankArea = _rRect;
		{
			if MX_LIKELY( m_pheiTarget ) {
				const uint64_t ui64BufAddr = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine * stAll.uiBytesPerRow;
				const size_t sBufCap = size_t( (m_sdScrollView[m_eaEditAs].i32PageLines + 1) * stAll.uiBytesPerRow );
				if MX_LIKELY( m_pheiTarget->Read( ui64BufAddr, m_bCurBuffer, sBufCap ) ) {
					// What the edit box would write is shown in both areas.
					const CHexEditorInterface::CBuffer &bShown = DisplayBuffer( ui64BufAddr, sBufCap );
					int32_t i32Top = GetRulerHeightPx();
					DrawArea( _hDc, iX, i32Top, stAll.dfLeftNumbersFmt, m_sdScrollView[m_eaEditAs].i32PageLines + 1, bShown, false );
					auto ui32WidthL = ComputeAreaWidthPx( stAll.dfLeftNumbersFmt );
					rBlankArea.top = i32Top;
					rBlankArea.left = iX + ui32WidthL;

					if ( stAll.bShowRightArea ) {
						DrawArea( _hDc, iX + ui32WidthL + stAll.i32PadNumbersLeftPx + stAll.i32PadBetweenNumbersAndTextPx + stAll.i32PadNumbersRightPx,
							i32Top, stAll.dfRightNumbersFmt, m_sdScrollView[m_eaEditAs].i32PageLines + 1, bShown, true );

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

		if ( stAll.bShowMiniMap && _bDrawMiniMap ) {
			DrawMiniMap( _hDc,
				MiniMapBarX( _rRect ),
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
				i32VisualWidth = int32_t( rBg.Width() );
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
		// The view-box drag must use exactly this geometry.
		m_mmsMiniMap.ui32DrawnBytesPerRow = ui32SrcW;
		m_mmsMiniMap.i32DrawnTop = _iYTop;
		m_mmsMiniMap.i32DrawnHeight = i32RectH;

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

			// On the left, the bitmap starts at the left edge and is kept out of the drag bar and the main view.
			const int32_t i32ClipRight = _iXLeft - 1;
			const bool bClip = !mmMiniMap.bRightSize;
			if ( mmMiniMap.bRightSize ) {
				++_iXLeft;
			}
			else {
				_iXLeft = 0;
			}
			int iSavedDc = 0;
			if ( bClip ) {
				iSavedDc = ::SaveDC( _hDc );
				::IntersectClipRect( _hDc, 0, _rRect.top, std::max( i32ClipRight, 0 ), _rRect.bottom );
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
			if ( iSavedDc ) { ::RestoreDC( _hDc, iSavedDc ); }

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

		uint64_t ui64TotalLines = TotalLines();
		if ( m_esEdit.bActive && ui32Bpr ) {
			// The edit box can run past the end of the data (and inserted cells push the data along), so draw its rows too.
			const uint64_t ui64EditEnd = EditEnd();
			const uint64_t ui64Shown = m_esEdit.bInsert ? (Size() + (ui64EditEnd - m_esEdit.ui64Start)) : std::max( Size(), ui64EditEnd );
			ui64TotalLines = std::max( ui64TotalLines, (ui64Shown + ui32Bpr - 1) / ui32Bpr );
		}
		auto ui64MaxLines = ui64TotalLines - ui64Line + 1;
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

			// The parts of the edit box's border that fall in this row.  Drawn over this row's backgrounds and under its text.
			if ( m_esEdit.bActive && m_esEdit.bRightArea == _bRightArea ) {
				DrawEditBoxRow( _hDc, _dfFmt, _iXLeft, ComputeAreaWidthPx( _dfFmt ), _iYTop, I, iLineAdv );
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
					// Cells in the edit box that are not drawn normally (typed values, partly typed digits, empty cells).
					const bool bEditDrawn = EditCovers( ui64ThisAddr, _bRightArea ) &&
						DrawEditCellText( _hDc, ui64ThisAddr, _dfFmt, iL, iY + stAll.i32LineSpacingPx / 2, wsTmp, pui8Data + J, sDataSize - J );
					if ( !bEditDrawn ) {
						auto crThisColor = CellFgColor( ui64ThisAddr, pui8Data + J, sDataSize - J, _bRightArea );
						if ( crThisColor != MX_GetRgbValue( ForeColors()->crEditor ) ) {
							lsw::LSW_SETTEXTCOLOR stcTextColor2( _hDc, MX_GetRgbValue( crThisColor ) );
							::TextOutW( _hDc, iL, iY + stAll.i32LineSpacingPx / 2, wsTmp.c_str(), static_cast<int>(wsTmp.size()) );
						}
						else {
							::TextOutW( _hDc, iL, iY + stAll.i32LineSpacingPx / 2, wsTmp.c_str(), static_cast<int>(wsTmp.size()) );
						}
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
					bool bFocus = (pwAncestor && pwAncestor->IsFocused()) || m_bHasFocus || (m_pwParent && m_pwParent->GetFocus()) ||
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
	 * \param _bEditFade If true, the text of a cell inside the edit box that has not been typed over is faded.
	 * \return Returns the text color for the given cell.
	 **/
	COLORREF CHexEditorControl::CellFgColor( uint64_t _ui64Address, const uint8_t * _pui8Value, size_t _sSize, bool _bRightArea, bool _bEditFade ) {
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

		// Values that changed (any of their bytes).
		if ( _ui64Address < Size() ) {
			const uint32_t ui32Stride = StrideForFormat( _bRightArea ? stAll.dfRightNumbersFmt : stAll.dfLeftNumbersFmt );
			if ( m_pheiTarget && m_pheiTarget->TracksChangedBytes() ) {
				if ( m_pheiTarget->BytesChanged( _ui64Address, std::max<uint32_t>( ui32Stride, 1 ) ) ) {
					crBaseColor = ForeColors()->crModified;
					bDefaultBaseColor = false;
				}
			}
			else if ( !m_mChanges.empty() ) {
				uint32_t ui32Time;
				if ( ChangedIn( _ui64Address, ui32Stride, ui32Time ) ) {
					crBaseColor = ChangedColor( crBaseColor, ui32Time );
					bDefaultBaseColor = false;
				}
			}
		}


		if ( !m_sSel.HasSelection() ) {
			if ( _ui64Address < Size() && m_sSel.IsSelected( _ui64Address, CurStyle()->uiBytesPerRow ) && (MX_GetAValue( ForeColors()->crSelected ) || bDefaultBaseColor) ) {
				crBaseColor = bDefaultBaseColor ? ForeColors()->crSelected : Mix( crBaseColor, ForeColors()->crSelected );
			}
		}

		// Inside the edit box, what has not been typed over is faded, as if seen through a translucent pane in the edit box's color.
		if ( _bEditFade && EditCovers( _ui64Address, _bRightArea ) && !EditCellTyped( _ui64Address ) ) {
			crBaseColor = EditFadeColor( MX_GetRgbValue( crBaseColor ), MX_GetRgbValue( CellBgColor( _ui64Address, _pui8Value, _sSize, _bRightArea ) ) );
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

		// The edit box: a little whiter than what lies beneath it (or a little darker if that is white).
		if ( EditCovers( _ui64Address, _bRightArea ) ) {
			crBaseColor = EditBoxColor( crBaseColor );
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

		const uint64_t ui64FirstLineMini = MiniMapFirstLine( m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine, _i32ScreenHeight, _ui32BytesPerRow );
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
	uint64_t CHexEditorControl::MiniMapFirstLine( uint64_t _ui64TopLineMain, int32_t _i32ScreenHeight, uint32_t _ui32BytesPerRow ) const {
		const CMiniMap::MX_MINI_MAP &mmMiniMap = CurStyle()->mmMiniMap;
		if ( _i32ScreenHeight <= 0 || !_ui32BytesPerRow ) { return 0; }

		const uint64_t ui64TotalBytes = Size();
		if ( ui64TotalBytes == 0 ) { return 0; }

		const uint32_t ui32Zoom = mmMiniMap.ui32Zoom ? mmMiniMap.ui32Zoom : 1U;

		// Total mini-map lines in the file based on mini-map bytes-per-row.
		const uint64_t ui64TotalMiniLines = (ui64TotalBytes + _ui32BytesPerRow - 1ULL) / _ui32BytesPerRow;

		// Number of mini-map lines visible on screen at this zoom.
		const uint64_t ui64SpanMiniLines = MiniMapSpanLines( _i32ScreenHeight, ui32Zoom );

		// If the whole file fits, it is shown from the start.
		if ( ui64SpanMiniLines >= ui64TotalMiniLines ) { return 0; }

		// Main editor visible range (in main lines).
		const int32_t i32PageLines = (m_sdScrollView[m_eaEditAs].i32PageLines > 0) ?
			m_sdScrollView[m_eaEditAs].i32PageLines : 1;

		const uint32_t ui32MainBpr = CurStyle()->uiBytesPerRow ? CurStyle()->uiBytesPerRow : 1U;

		const uint64_t ui64PageBytesMain = static_cast<uint64_t>(i32PageLines) * static_cast<uint64_t>(ui32MainBpr);
		const uint64_t ui64TopAddrMain = _ui64TopLineMain * static_cast<uint64_t>(ui32MainBpr);

		// Maximum top-of-view address (clamped).
		uint64_t ui64MaxTopAddrMain = 0;
		if ( ui64TotalBytes > ui64PageBytesMain ) {
			ui64MaxTopAddrMain = ui64TotalBytes - ui64PageBytesMain;
		}

		// Convert to mini-map line space.
		const uint64_t ui64TopLineMini = ui64TopAddrMain / static_cast<uint64_t>(_ui32BytesPerRow);
		const uint64_t ui64MaxTopLineMini = ui64MaxTopAddrMain / static_cast<uint64_t>(_ui32BytesPerRow);

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

		return std::min( ui64FirstLineMini, ui64MaxStart );
	}

	/**
	 * \brief Gets the drawn top of the "current view" box in the mini-map, as it would be if the main view started at a given line.
	 *
	 * Uses the geometry of the last DrawMiniMap() and the same content placement (MiniMapFirstLine()), so the result matches what
	 *	is on screen.  The box can start part-way through a mini-map row, so the result is fractional.
	 *
	 * \param _ui64TopLineMain The main view's first visible line.
	 * \return Returns the box top in pixels, relative to the top of the mini-map.
	 */
	double CHexEditorControl::MiniMapViewBoxTopPx( uint64_t _ui64TopLineMain ) const {
		const uint32_t ui32MiniBpr = m_mmsMiniMap.ui32DrawnBytesPerRow;
		if ( !ui32MiniBpr ) { return 0.0; }
		const uint32_t ui32Zoom = CurStyle()->mmMiniMap.ui32Zoom ? CurStyle()->mmMiniMap.ui32Zoom : 1U;
		const uint64_t ui64MainBpr = CurStyle()->uiBytesPerRow ? CurStyle()->uiBytesPerRow : 1ULL;

		const uint64_t ui64FirstMini = MiniMapFirstLine( _ui64TopLineMain, m_mmsMiniMap.i32DrawnHeight, ui32MiniBpr );
		// Bytes between the top of the mini-map and the top of the box (negative if the box starts above the mini-map).
		const int64_t i64Bytes = static_cast<int64_t>(_ui64TopLineMain * ui64MainBpr) - static_cast<int64_t>(ui64FirstMini * ui32MiniBpr);
		return double( ui32Zoom ) * double( i64Bytes ) / double( ui32MiniBpr );
	}

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
	bool CHexEditorControl::MiniMapViewBoxModel( double &_dPxPerLine, uint64_t &_ui64MaxFirstLine, bool &_bExact ) const {
		const uint32_t ui32MiniBpr = m_mmsMiniMap.ui32DrawnBytesPerRow;
		const int32_t i32Height = m_mmsMiniMap.i32DrawnHeight;
		if ( !CurStyle()->bShowMiniMap || !ui32MiniBpr || i32Height <= 0 ) { return false; }

		const uint64_t ui64TotalBytes = Size();
		if ( ui64TotalBytes == 0 ) { return false; }

		const uint32_t ui32Zoom = CurStyle()->mmMiniMap.ui32Zoom ? CurStyle()->mmMiniMap.ui32Zoom : 1U;
		const uint64_t ui64MainBpr = CurStyle()->uiBytesPerRow ? CurStyle()->uiBytesPerRow : 1ULL;
		const uint64_t ui64PageLines = (m_sdScrollView[m_eaEditAs].i32PageLines > 0) ?
			static_cast<uint64_t>(m_sdScrollView[m_eaEditAs].i32PageLines) : 1ULL;

		const uint64_t ui64Lines = TotalLines_FixedWidth();
		_ui64MaxFirstLine = (ui64Lines > ui64PageLines) ? (ui64Lines - ui64PageLines) : 0ULL;
		if ( _ui64MaxFirstLine == 0 ) { return false; }

		// The fraction of the box's movement that is cancelled by the content scrolling underneath it (see MiniMapFirstLine()).
		double dScrollRatio = 0.0;
		const uint64_t ui64TotalMiniLines = (ui64TotalBytes + ui32MiniBpr - 1ULL) / ui32MiniBpr;
		const uint64_t ui64SpanMiniLines = MiniMapSpanLines( i32Height, ui32Zoom );
		if ( ui64SpanMiniLines < ui64TotalMiniLines ) {
			const uint64_t ui64PageBytes = ui64PageLines * ui64MainBpr;
			const uint64_t ui64MaxTopAddr = (ui64TotalBytes > ui64PageBytes) ? (ui64TotalBytes - ui64PageBytes) : 0ULL;
			const uint64_t ui64MaxTopMini = ui64MaxTopAddr / ui32MiniBpr;
			if ( ui64MaxTopMini ) {
				dScrollRatio = double( ui64TotalMiniLines - ui64SpanMiniLines ) / double( ui64MaxTopMini );
			}
		}

		_dPxPerLine = double( ui32Zoom ) * double( ui64MainBpr ) / double( ui32MiniBpr ) * (1.0 - dScrollRatio);
		_bExact = _dPxPerLine > 1.0e-9;
		if ( !_bExact ) {
			// The box is at least as tall as the mini-map and cannot follow the mouse; map the mini-map's height to the scroll range.
			_dPxPerLine = double( i32Height ) / double( _ui64MaxFirstLine );
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
		
		/*if ( m_pwHexParent ) {
			auto psbStatus = m_pwHexParent->StatusBar();
			if ( psbStatus ) {
				psbStatus->SetTextW( L"" );
			}
		}*/
		
		m_sgSelGesture.bSelecting = false;
		m_sgSelGesture.bPendingThreshold = true;
		m_sgSelGesture.ptDown = _ptClient;
		m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64CaretAddr = ui64Addr;
		m_sgSelGesture.i32CaretIdx = i32Sub;
		StartCaretBlink();

		m_sgSelGesture.smCurrent = _bCtrl ? MX_SM_COLUMN : (CurStyle()->bSelectColumnMode ? MX_SM_COLUMN : MX_SM_NORMAL);

		m_sSel.bHas = false;
		if ( m_pwHexParent ) {
			m_pwHexParent->SetStatusBarText( L"", false );
			m_pwHexParent->UpdateStatusBar_PosValue_StartSize();
		}
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
		const uint32_t ui32BrCol = ui32AnchorCol + std::min( ui32Bpr, m_sSel.sc.ui32Cols );
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

	// == Editing.
	/**
	 * Closes the edit box without writing anything.  The caret returns to where the edit began.
	 **/
	void CHexEditorControl::CancelEdit() {
		if ( !m_esEdit.bActive ) { return; }
		m_sSel = m_esEdit.sOldSel;
		m_sgSelGesture = m_esEdit.sgOldGesture;
		m_esEdit = MX_EDIT_STATE();
		m_bEditBuffer = CHexEditorInterface::CBuffer();
		StartCaretBlink();
		::InvalidateRect( Wnd(), nullptr, FALSE );
	}

	/**
	 * Writes what was typed into the edit box and closes it.
	 *
	 * \param _swsStatus Receives an error message on failure.
	 * \return Returns false if a typed value is invalid (the edit box stays open so that it can be corrected) or could not be written.
	 **/
	bool CHexEditorControl::CommitEdit( CSecureWString &_swsStatus ) {
		if ( !m_esEdit.bActive ) { return true; }
		if ( m_esEdit.bAutoCommit ) {
			CommitAutoEdit();
			return true;
		}
		if ( !m_pheiTarget ) {
			CancelEdit();
			_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		try {
			// Where the caret goes afterward: where it is in the edit box, or after the last value if it is at the end of it.
			uint64_t ui64Caret = m_sgSelGesture.ui64CaretAddr;
			int32_t i32CaretIdx = m_sgSelGesture.i32CaretIdx;
			if ( m_esEdit.ekKind == MX_EK_VALUES && m_esEdit.sItem + 1 == m_esEdit.vTexts.size() &&
				m_esEdit.vTexts[m_esEdit.sItem].size() && m_esEdit.sCursor == m_esEdit.vTexts[m_esEdit.sItem].size() ) {
				ui64Caret = m_esEdit.ui64Start + uint64_t( m_esEdit.sItem + 1 ) * StrideForFormat( m_esEdit.dfFmt );
				i32CaretIdx = 0;
			}

			// Only what lies between the first and last things typed is written.
			const MX_EDIT_STATE esSaved = m_esEdit;
			TrimEdit( false );
			std::vector<uint8_t> vBytes;
			if ( !EditBytes( vBytes, _swsStatus ) ) {
				// An invalid value leaves the edit box open so that it can be corrected.
				m_esEdit = esSaved;
				return false;
			}
			if ( vBytes.empty() ) {
				CancelEdit();
				return true;
			}

			// Close the edit box first so that the display shows the real data from here on.
			const MX_SELECTION sOldSel = m_esEdit.sOldSel;
			const MX_SELECT_GESTURE sgOldGesture = m_esEdit.sgOldGesture;
			const uint64_t ui64Start = m_esEdit.ui64Start;
			const bool bInsert = m_esEdit.bInsert;
			const bool bRight = m_esEdit.bRightArea;
			m_esEdit = MX_EDIT_STATE();
			m_bEditBuffer = CHexEditorInterface::CBuffer();

			FlushRealTimeUndo();
			bool bOk;
			{
				CHexEditorInterface::MX_UNDO uUndo( m_pheiTarget );
				bOk = WriteBytes( ui64Start, vBytes, bInsert, _swsStatus );
			}

			ClearSelection();
			m_sgSelGesture.bRightArea = bRight;
			m_sgSelGesture.ui64CaretAddr = std::min( ui64Caret, Size() );
			m_sgSelGesture.i32CaretIdx = (m_sgSelGesture.ui64CaretAddr == ui64Caret) ? i32CaretIdx : 0;
			m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64CaretAddr;
			// One selection-Undo item per data-Undo group.
			AddSelectionUndo( sOldSel, sgOldGesture );

			StartCaretBlink();
			EnsureVisible( m_sgSelGesture.ui64CaretAddr, FALSE );
			::InvalidateRect( Wnd(), nullptr, FALSE );
			if ( m_pwHexParent ) { m_pwHexParent->UpdateStatusBar_PosValue_StartSize(); }
			return bOk;
		}
		catch ( ... ) {
			CancelEdit();
			_swsStatus = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * Ends the Undo group that collects real-time edits.  It normally ends 0.2 seconds after the last keystroke; call this before
	 *	anything else changes the data or the Undo stack.
	 **/
	void CHexEditorControl::FlushRealTimeUndo() {
		m_tRtUndo.Stop();
		if ( !m_bRtUndoOpen ) { return; }
		m_bRtUndoOpen = false;
		if ( m_pheiTarget ) {
			m_pheiTarget->EndUndo();
			// One selection-Undo item for the whole group, restoring the caret from before the first edit in it.
			AddSelectionUndo( m_sRtOldSel, m_sgRtOldGesture );
		}
	}

	/**
	 * The WM_SYSKEYDOWN handler.  Collects the digits of an Alt code (Alt held while typing on the numeric keypad).
	 *
	 * \param _uiKeyCode The virtual-key code.
	 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
	 * \return Returns an LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::SysKeyDown( UINT _uiKeyCode, UINT _uiFlags ) {
		const bool bRepeat = (_uiFlags & (1U << 30)) != 0;
		if ( _uiKeyCode == VK_MENU ) {
			// A fresh press of Alt starts a new Alt code.
			if ( !bRepeat ) {
				m_ui32AltCode = 0;
				m_ui32AltDigits = 0;
				m_bAltLeadingZero = false;
			}
			return lsw::CWidget::LSW_H_CONTINUE;
		}

		// The numeric keypad's digits.  Without Num Lock they arrive as navigation keys, which the keypad sends as non-extended keys.
		int32_t i32Digit = -1;
		if ( _uiKeyCode >= VK_NUMPAD0 && _uiKeyCode <= VK_NUMPAD9 ) {
			i32Digit = int32_t( _uiKeyCode - VK_NUMPAD0 );
		}
		else if ( (_uiFlags & (1U << 24)) == 0 ) {
			switch ( _uiKeyCode ) {
				case VK_INSERT : { i32Digit = 0; break; }
				case VK_END : { i32Digit = 1; break; }
				case VK_DOWN : { i32Digit = 2; break; }
				case VK_NEXT : { i32Digit = 3; break; }
				case VK_LEFT : { i32Digit = 4; break; }
				case VK_CLEAR : { i32Digit = 5; break; }
				case VK_RIGHT : { i32Digit = 6; break; }
				case VK_HOME : { i32Digit = 7; break; }
				case VK_UP : { i32Digit = 8; break; }
				case VK_PRIOR : { i32Digit = 9; break; }
			}
		}
		if ( i32Digit >= 0 && (::GetKeyState( VK_MENU ) & 0x8000) ) {
			if ( !bRepeat ) {
				if ( m_ui32AltDigits == 0 && i32Digit == 0 ) { m_bAltLeadingZero = true; }
				if ( m_ui32AltCode <= 0x10FFFF ) { m_ui32AltCode = m_ui32AltCode * 10 + uint32_t( i32Digit ); }
				++m_ui32AltDigits;
			}
		}
		else {
			m_ui32AltCode = 0;
			m_ui32AltDigits = 0;
			m_bAltLeadingZero = false;
		}
		return lsw::CWidget::LSW_H_CONTINUE;
	}

	/**
	 * The WM_SYSKEYUP handler.  When Alt is released after an Alt code above 255, types the Unicode character with that code (Windows
	 *	itself would produce the character for the code modulo 256).
	 *
	 * \param _uiKeyCode The virtual-key code.
	 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
	 * \return Returns an LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::SysKeyUp( UINT _uiKeyCode, UINT /*_uiFlags*/ ) {
		if ( _uiKeyCode == VK_MENU && m_ui32AltDigits ) {
			const uint32_t ui32Code = m_ui32AltCode;
			const bool bLeadingZero = m_bAltLeadingZero;
			m_ui32AltCode = 0;
			m_ui32AltDigits = 0;
			m_bAltLeadingZero = false;
			if ( ui32Code > 0xFF && ui32Code <= 0x10FFFF && !(ui32Code >= 0xD800 && ui32Code <= 0xDFFF) ) {
				// Windows has already queued its own character for this Alt code (the code modulo 256 in the ANSI code page, or the OEM
				//	code page without the leading 0).  Remember it so that OnTextInputCodePoint() discards it.
				char cByte = char( ui32Code & 0xFF );
				wchar_t wcWindows = 0;
				if ( cByte == 0 || ::MultiByteToWideChar( bLeadingZero ? CP_ACP : CP_OEMCP, 0, &cByte, 1, &wcWindows, 1 ) != 1 ) {
					wcWindows = 0;
				}
				TypeCodePoint( char32_t( ui32Code ), 1 );
				m_wcAltSwallow = wcWindows;
				m_c32AltEmitted = char32_t( ui32Code );
			}
		}
		return lsw::CWidget::LSW_H_CONTINUE;
	}

	/**
	 * Types one character, the way a WM_CHAR would.  Starts or continues an edit, or edits in real time.
	 *
	 * \param _c32Cp The character.
	 * \param _uiRepeat How many times to type it.
	 **/
	void CHexEditorControl::TypeCodePoint( char32_t _c32Cp, uint32_t _uiRepeat ) {
		if ( !m_pheiTarget ) { return; }
		// Enter, Escape, Backspace and Ctrl+letter arrive here too; KeyDown() handles the keys that mean anything.  Tab separates values.
		if ( (_c32Cp < 0x20 && _c32Cp != U'\t') || _c32Cp == 0x7F ) { return; }
		if ( m_pheiTarget->ReadOnly() ) {
			EditWarning( _DEC_WS_D4FFE2E0_File_is_Read_Only );
			return;
		}
		for ( uint32_t I = 0; I < std::max( _uiRepeat, 1U ); ++I ) {
			if ( m_esEdit.bActive ) { EditType( _c32Cp ); }
			else { TypeFirst( _c32Cp ); }
		}
	}

	/**
	 * Types one character when no edit box is open: decides between real-time editing and opening the edit box.
	 *
	 * \param _c32Cp The character.
	 **/
	void CHexEditorControl::TypeFirst( char32_t _c32Cp ) {
		const MX_STYLE &stAll = (*CurStyle());
		const bool bRight = m_sgSelGesture.bRightArea && stAll.bShowRightArea;
		const MX_DATA_FMT dfFmt = bRight ? stAll.dfRightNumbersFmt : stAll.dfLeftNumbersFmt;
		const MX_EDIT_KIND ekKind = EditKindForFormat( dfFmt );

		// Typing starts at the start of the selection, or at the caret, aligned to the format's values.
		uint64_t ui64Addr = HasSelection() ? m_sSel.LowerLeftAddress() : m_sgSelGesture.ui64CaretAddr;
		int32_t i32Idx = HasSelection() ? 0 : m_sgSelGesture.i32CaretIdx;
		const uint32_t ui32Stride = StrideForFormat( dfFmt );
		if ( ui32Stride > 1 ) { ui64Addr -= ui64Addr % ui32Stride; }
		ui64Addr = std::min( ui64Addr, Size() );
		// Process memory cannot grow.
		if ( m_pheiTarget->OverwriteOnly() && ui64Addr >= Size() ) { return; }

		const bool bRealTime = RealTimeFor( dfFmt );
		switch ( ekKind ) {
			case MX_EK_DIGITS : {
				const int32_t i32Digit = DigitValue( _c32Cp, RadixForFormat( dfFmt ) );
				if ( i32Digit < 0 ) { return; }		// Skipped.
				if ( bRealTime ) {
					RealTimeDigit( ui64Addr, i32Idx, dfFmt, uint32_t( i32Digit ) );
					return;
				}
				BeginEdit( dfFmt, bRight, ui64Addr, false );
				// Type over from the digit under the caret (an inserted byte starts at its first digit).
				if ( !m_esEdit.bInsert ) {
					m_esEdit.sDigitPos = size_t( std::clamp<int32_t>( i32Idx, 0, int32_t( SubCaretCountForFormat( dfFmt ) ) - 1 ) );
				}
				EditType( _c32Cp );
				break;
			}
			case MX_EK_TEXT : {
				// An escape needs more than one character, so even in real time it is collected in the edit box until it is complete.
				if ( bRealTime && _c32Cp != U'\\' ) {
					RealTimeText( ui64Addr, _c32Cp );
					return;
				}
				BeginEdit( dfFmt, bRight, ui64Addr, bRealTime );
				EditType( _c32Cp );
				break;
			}
			case MX_EK_VALUES : {
				if ( !ValueCharValid( dfFmt, _c32Cp, true ) ) { return; }		// Skipped (separators too: nothing to separate yet).
				BeginEdit( dfFmt, bRight, ui64Addr, false );
				EditType( _c32Cp );
				break;
			}
		}
	}

	/**
	 * Determines whether typing in a given format changes the data as each character is typed.
	 *
	 * \param _dfFmt The format being typed.
	 * \return Returns true for real-time editing, false if the edit box collects the typing.
	 **/
	bool CHexEditorControl::RealTimeFor( MX_DATA_FMT _dfFmt ) const {
		const bool bText = _dfFmt == MX_DF_CHAR;
		if ( IsProcess() ) {
			// RAM is only written once a value is complete, except text, which can optionally be written character-by-character.
			return bText && (m_pwHexParent ? m_pwHexParent->RealTimeRamTextEditing() : false);
		}
		// Only formats where each character stands alone can change as each is typed; other values need the edit box.
		const bool bDigits = _dfFmt == MX_DF_HEX || _dfFmt == MX_DF_BIN;
		return (bText || bDigits) && (m_pwHexParent ? m_pwHexParent->RealTimeFileEditing() : true);
	}

	/**
	 * Opens the edit box.
	 *
	 * \param _dfFmt The format being typed.
	 * \param _bRightArea True if the edit box is in the right area.
	 * \param _ui64Addr The address of the first edited cell.
	 * \param _bAutoCommit If true, the edit writes itself as soon as its text no longer ends in an unfinished escape.
	 **/
	void CHexEditorControl::BeginEdit( MX_DATA_FMT _dfFmt, bool _bRightArea, uint64_t _ui64Addr, bool _bAutoCommit ) {
		// An automatically committing edit belongs to the real-time Undo group; anything else starts after it.
		if ( !_bAutoCommit ) { FlushRealTimeUndo(); }
		try {
			m_esEdit = MX_EDIT_STATE();
			m_esEdit.bActive = true;
			m_esEdit.bRightArea = _bRightArea;
			m_esEdit.bAutoCommit = _bAutoCommit;
			m_esEdit.dfFmt = _dfFmt;
			m_esEdit.ekKind = EditKindForFormat( _dfFmt );
			m_esEdit.ui64Start = _ui64Addr;
			m_esEdit.bInsert = !IsOverwriteMode() && m_pheiTarget && !m_pheiTarget->OverwriteOnly();
			m_esEdit.sOldSel = m_sSel;
			m_esEdit.sgOldGesture = m_sgSelGesture;
			ClearSelection();

			switch ( m_esEdit.ekKind ) {
				case MX_EK_DIGITS : {
					EnsureEditCell( 0 );
					break;
				}
				case MX_EK_VALUES : {
					m_esEdit.vTexts.push_back( std::wstring() );
					break;
				}
				default : {}
			}
		}
		catch ( ... ) {
			m_esEdit = MX_EDIT_STATE();
		}
	}

	/**
	 * Types one character into the open edit box.  Characters that are not valid for the format are skipped.
	 *
	 * \param _c32Cp The character.
	 **/
	void CHexEditorControl::EditType( char32_t _c32Cp ) {
		if ( !m_esEdit.bActive ) { return; }
		try {
			// Outside of text, Tab, Space, comma and semicolon move between cells or values (Shift+Tab moves back).
			if ( m_esEdit.ekKind != MX_EK_TEXT &&
				(_c32Cp == U'\t' || _c32Cp == U' ' || (m_esEdit.ekKind == MX_EK_VALUES && (_c32Cp == U',' || _c32Cp == U';'))) ) {
				const bool bBack = _c32Cp == U'\t' && (::GetKeyState( VK_SHIFT ) & 0x8000) != 0;
				EditNavigate( bBack ? MX_EN_PREV_ITEM : MX_EN_NEXT_ITEM );
				return;
			}
			switch ( m_esEdit.ekKind ) {
				case MX_EK_DIGITS : {
					const int32_t i32Digit = DigitValue( _c32Cp, RadixForFormat( m_esEdit.dfFmt ) );
					if ( i32Digit < 0 ) { return; }
					const size_t sDigits = SubCaretCountForFormat( m_esEdit.dfFmt );
					const size_t sCell = m_esEdit.sDigitPos / sDigits;
					if ( !EditCanCover( sCell ) ) { return; }
					EnsureEditCell( sCell );
					m_esEdit.vDigits[m_esEdit.sDigitPos] = uint8_t( i32Digit );
					m_esEdit.vTypedMask[sCell] |= uint8_t( 1U << (m_esEdit.sDigitPos % sDigits) );
					// The edit box covers the cell where the next digit goes (unless that is past the end of process memory).
					const size_t sNextCell = (m_esEdit.sDigitPos + 1) / sDigits;
					if ( EditCanCover( sNextCell ) ) {
						++m_esEdit.sDigitPos;
						EnsureEditCell( sNextCell );
					}
					break;
				}
				case MX_EK_TEXT : {
					// Every character is typed, Tab and Space included.
					m_esEdit.sCursor = std::min( m_esEdit.sCursor, m_esEdit.u32Text.size() );
					m_esEdit.u32Text.insert( m_esEdit.u32Text.begin() + m_esEdit.sCursor, _c32Cp );
					++m_esEdit.sCursor;
					UpdateEditPreview();
					if ( m_esEdit.bAutoCommit && !m_esEdit.bEscapeOpen ) {
						CommitAutoEdit();
						return;
					}
					break;
				}
				case MX_EK_VALUES : {
					std::wstring &wsItem = m_esEdit.vTexts[m_esEdit.sItem];
					m_esEdit.sCursor = std::min( m_esEdit.sCursor, wsItem.size() );
					// A sign can only go at the start, and only once.
					const bool bSignAllowed = m_esEdit.sCursor == 0 && (wsItem.empty() || (wsItem[0] != L'-' && wsItem[0] != L'+'));
					if ( !ValueCharValid( m_esEdit.dfFmt, _c32Cp, bSignAllowed ) ) { return; }
					wsItem.insert( wsItem.begin() + m_esEdit.sCursor, wchar_t( _c32Cp ) );
					++m_esEdit.sCursor;
					break;
				}
			}
			UpdateEditCaret();
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}
		catch ( ... ) {}
	}

	/**
	 * Removes what is typed before the cursor in the edit box (a digit goes back to what it was).  At the start of a value, moves to
	 *	the end of the previous one.  Closes the edit box if nothing typed remains.
	 **/
	void CHexEditorControl::EditBackspace() {
		if ( !m_esEdit.bActive ) { return; }
		try {
			switch ( m_esEdit.ekKind ) {
				case MX_EK_DIGITS : {
					if ( m_esEdit.sDigitPos ) {
						// Back one digit, putting back the digit that was there before.
						const size_t sDigits = SubCaretCountForFormat( m_esEdit.dfFmt );
						--m_esEdit.sDigitPos;
						const size_t sCell = m_esEdit.sDigitPos / sDigits;
						m_esEdit.vDigits[m_esEdit.sDigitPos] = m_esEdit.vOrigDigits[m_esEdit.sDigitPos];
						m_esEdit.vTypedMask[sCell] &= uint8_t( ~(1U << (m_esEdit.sDigitPos % sDigits)) );
					}
					break;
				}
				case MX_EK_TEXT : {
					m_esEdit.sCursor = std::min( m_esEdit.sCursor, m_esEdit.u32Text.size() );
					if ( m_esEdit.sCursor ) {
						m_esEdit.u32Text.erase( m_esEdit.u32Text.begin() + (m_esEdit.sCursor - 1) );
						--m_esEdit.sCursor;
					}
					break;
				}
				case MX_EK_VALUES : {
					std::wstring &wsItem = m_esEdit.vTexts[m_esEdit.sItem];
					m_esEdit.sCursor = std::min( m_esEdit.sCursor, wsItem.size() );
					if ( m_esEdit.sCursor ) {
						wsItem.erase( wsItem.begin() + (m_esEdit.sCursor - 1) );
						--m_esEdit.sCursor;
					}
					else if ( m_esEdit.sItem ) {
						// Back to the end of the previous value.
						--m_esEdit.sItem;
						m_esEdit.sCursor = m_esEdit.vTexts[m_esEdit.sItem].size();
					}
					break;
				}
			}
			EditChanged();
		}
		catch ( ... ) {}
	}

	/**
	 * Removes what is typed under the cursor in the edit box (a digit goes back to what it was).  Closes the edit box if nothing typed
	 *	remains.
	 **/
	void CHexEditorControl::EditDelete() {
		if ( !m_esEdit.bActive ) { return; }
		try {
			switch ( m_esEdit.ekKind ) {
				case MX_EK_DIGITS : {
					const size_t sDigits = SubCaretCountForFormat( m_esEdit.dfFmt );
					const size_t sCell = m_esEdit.sDigitPos / sDigits;
					if ( sCell < m_esEdit.vTypedMask.size() ) {
						m_esEdit.vDigits[m_esEdit.sDigitPos] = m_esEdit.vOrigDigits[m_esEdit.sDigitPos];
						m_esEdit.vTypedMask[sCell] &= uint8_t( ~(1U << (m_esEdit.sDigitPos % sDigits)) );
					}
					break;
				}
				case MX_EK_TEXT : {
					if ( m_esEdit.sCursor < m_esEdit.u32Text.size() ) {
						m_esEdit.u32Text.erase( m_esEdit.u32Text.begin() + m_esEdit.sCursor );
					}
					break;
				}
				case MX_EK_VALUES : {
					std::wstring &wsItem = m_esEdit.vTexts[m_esEdit.sItem];
					if ( m_esEdit.sCursor < wsItem.size() ) {
						wsItem.erase( wsItem.begin() + m_esEdit.sCursor );
					}
					break;
				}
			}
			EditChanged();
		}
		catch ( ... ) {}
	}

	/**
	 * Finishes a change made by EditBackspace() or EditDelete(): closes the edit box if nothing typed remains, otherwise trims it and
	 *	redraws it.
	 **/
	void CHexEditorControl::EditChanged() {
		if ( !EditHasTyping() ) {
			CancelEdit();
			return;
		}
		if ( m_esEdit.ekKind == MX_EK_TEXT ) { UpdateEditPreview(); }
		TrimEdit( true );
		UpdateEditCaret();
		::InvalidateRect( Wnd(), nullptr, FALSE );
	}

	/**
	 * Determines whether anything typed remains in the edit box.
	 *
	 * \return Returns true if at least one digit, character or value is typed.
	 **/
	bool CHexEditorControl::EditHasTyping() const {
		switch ( m_esEdit.ekKind ) {
			case MX_EK_DIGITS : {
				for ( auto ui8Mask : m_esEdit.vTypedMask ) {
					if ( ui8Mask ) { return true; }
				}
				return false;
			}
			case MX_EK_TEXT : { return !m_esEdit.u32Text.empty(); }
			case MX_EK_VALUES : {
				for ( const auto &wsThis : m_esEdit.vTexts ) {
					if ( !wsThis.empty() ) { return true; }
				}
				return false;
			}
			default : { return false; }
		}
	}

	/**
	 * Moves the cursor in the edit box.  Digits: by digit, by cell (Tab, Space, Shift+Tab) or by row; values: by character within a
	 *	value, by value (Tab, Space, Shift+Tab) or by row; text: by character.  Moving past either end of the edit box extends it,
	 *	except before its start in Insert mode and past the end of process memory.
	 *
	 * \param _enNav The movement.
	 **/
	void CHexEditorControl::EditNavigate( MX_EDIT_NAV _enNav ) {
		if ( !m_esEdit.bActive ) { return; }
		try {
			const uint32_t ui32Bpr = CurStyle()->uiBytesPerRow ? CurStyle()->uiBytesPerRow : 16;
			switch ( m_esEdit.ekKind ) {
				case MX_EK_DIGITS : {
					const int64_t i64Digits = int64_t( SubCaretCountForFormat( m_esEdit.dfFmt ) );
					const int64_t i64Cells = int64_t( m_esEdit.vTypedMask.size() );
					const int64_t i64Cur = int64_t( m_esEdit.sDigitPos );
					// Floor division (the position can go negative, before the edit box).
					auto CellOf = [i64Digits]( int64_t _i64Pos ) -> int64_t { return (_i64Pos >= 0) ? (_i64Pos / i64Digits) : -((-_i64Pos + i64Digits - 1) / i64Digits); };
					int64_t i64Pos = i64Cur;
					switch ( _enNav ) {
						case MX_EN_LEFT : { --i64Pos; break; }
						case MX_EN_RIGHT : { ++i64Pos; break; }
						case MX_EN_UP : { i64Pos -= int64_t( ui32Bpr ) * i64Digits; break; }
						case MX_EN_DOWN : { i64Pos += int64_t( ui32Bpr ) * i64Digits; break; }
						case MX_EN_HOME : { i64Pos = 0; break; }
						case MX_EN_END : { i64Pos = i64Cells * i64Digits - 1; break; }
						case MX_EN_NEXT_ITEM : { i64Pos = (CellOf( i64Cur ) + 1) * i64Digits; break; }
						case MX_EN_PREV_ITEM : { i64Pos = (CellOf( i64Cur ) - 1) * i64Digits; break; }
						default : { return; }
					}
					if ( i64Pos < 0 ) {
						// Before the start: the edit box grows backward.
						const int64_t i64Add = -CellOf( i64Pos );
						if ( !EditPrepend( size_t( i64Add ) ) ) { return; }
						i64Pos += i64Add * i64Digits;
					}
					const size_t sCell = size_t( i64Pos / i64Digits );
					if ( !EditCanCover( sCell ) ) { return; }
					EnsureEditCell( sCell );
					m_esEdit.sDigitPos = size_t( i64Pos );
					break;
				}
				case MX_EK_TEXT : {
					m_esEdit.sCursor = std::min( m_esEdit.sCursor, m_esEdit.u32Text.size() );
					switch ( _enNav ) {
						case MX_EN_LEFT : {
							if ( m_esEdit.sCursor ) { --m_esEdit.sCursor; }
							break;
						}
						case MX_EN_RIGHT : {
							if ( m_esEdit.sCursor < m_esEdit.u32Text.size() ) { ++m_esEdit.sCursor; }
							break;
						}
						case MX_EN_HOME : { m_esEdit.sCursor = 0; break; }
						case MX_EN_END : { m_esEdit.sCursor = m_esEdit.u32Text.size(); break; }
						default : { return; }
					}
					break;
				}
				case MX_EK_VALUES : {
					const size_t sLen = m_esEdit.vTexts[m_esEdit.sItem].size();
					m_esEdit.sCursor = std::min( m_esEdit.sCursor, sLen );
					const int64_t i64RowItems = std::max<int64_t>( 1, int64_t( ui32Bpr / StrideForFormat( m_esEdit.dfFmt ) ) );
					int64_t i64Item = int64_t( m_esEdit.sItem );
					// Where the cursor goes in the new value: 0 = its start, 1 = its end, 2 = the same column.
					int32_t i32Place = 0;
					// False if the cursor only moves within the current value.
					bool bNewItem = true;
					switch ( _enNav ) {
						case MX_EN_LEFT : {
							if ( m_esEdit.sCursor ) {
								--m_esEdit.sCursor;
								bNewItem = false;
							}
							else {
								--i64Item;
								i32Place = 1;
							}
							break;
						}
						case MX_EN_RIGHT : {
							if ( m_esEdit.sCursor < sLen ) {
								++m_esEdit.sCursor;
								bNewItem = false;
							}
							else { ++i64Item; }
							break;
						}
						case MX_EN_UP : { i64Item -= i64RowItems; i32Place = 2; break; }
						case MX_EN_DOWN : { i64Item += i64RowItems; i32Place = 2; break; }
						case MX_EN_HOME : { m_esEdit.sCursor = 0; bNewItem = false; break; }
						case MX_EN_END : { m_esEdit.sCursor = sLen; bNewItem = false; break; }
						case MX_EN_NEXT_ITEM : { ++i64Item; i32Place = 1; break; }
						case MX_EN_PREV_ITEM : { --i64Item; i32Place = 1; break; }
						default : { return; }
					}
					if ( bNewItem ) {
						if ( i64Item < 0 ) {
							// Before the start: the edit box grows backward.
							if ( !EditPrepend( size_t( -i64Item ) ) ) { return; }
							i64Item = 0;
						}
						if ( !EditCanCover( size_t( i64Item ) ) ) { return; }
						while ( m_esEdit.vTexts.size() <= size_t( i64Item ) ) { m_esEdit.vTexts.push_back( std::wstring() ); }
						const size_t sOldCursor = m_esEdit.sCursor;
						m_esEdit.sItem = size_t( i64Item );
						const size_t sNewLen = m_esEdit.vTexts[m_esEdit.sItem].size();
						m_esEdit.sCursor = (i32Place == 0) ? 0 : ((i32Place == 1) ? sNewLen : std::min( sOldCursor, sNewLen ));
					}
					break;
				}
			}
			TrimEdit( true );
			UpdateEditCaret();
			::InvalidateRect( Wnd(), nullptr, FALSE );
		}
		catch ( ... ) {}
	}

	/**
	 * Grows the edit box backward by whole cells (digits) or values.  Not possible in Insert mode (the edit box is new data inserted
	 *	at its start) or before address 0.
	 *
	 * \param _sCount The number of cells or values to add before the start.
	 * \return Returns true if the edit box grew.
	 **/
	bool CHexEditorControl::EditPrepend( size_t _sCount ) {
		if ( !_sCount ) { return true; }
		if ( m_esEdit.bInsert || m_esEdit.ekKind == MX_EK_TEXT ) { return false; }
		const uint64_t ui64Stride = (m_esEdit.ekKind == MX_EK_DIGITS) ? 1 : StrideForFormat( m_esEdit.dfFmt );
		if ( m_esEdit.ui64Start < uint64_t( _sCount ) * ui64Stride ) { return false; }
		if ( m_esEdit.ekKind == MX_EK_DIGITS ) {
			const size_t sDigits = SubCaretCountForFormat( m_esEdit.dfFmt );
			const uint32_t ui32Radix = RadixForFormat( m_esEdit.dfFmt );
			for ( size_t I = 0; I < _sCount; ++I ) {
				--m_esEdit.ui64Start;
				uint32_t ui32Val = 0;
				if ( m_esEdit.ui64Start < Size() && m_pheiTarget ) {
					CHexEditorInterface::CBuffer bTmp;
					if ( m_pheiTarget->Read( m_esEdit.ui64Start, bTmp, 1 ) && bTmp.size() ) { ui32Val = bTmp[0]; }
				}
				uint8_t ui8Digits[8] = {};
				for ( size_t J = sDigits; J--; ) {
					ui8Digits[J] = uint8_t( ui32Val % ui32Radix );
					ui32Val /= ui32Radix;
				}
				m_esEdit.vDigits.insert( m_esEdit.vDigits.begin(), ui8Digits, ui8Digits + sDigits );
				m_esEdit.vOrigDigits.insert( m_esEdit.vOrigDigits.begin(), ui8Digits, ui8Digits + sDigits );
				m_esEdit.vTypedMask.insert( m_esEdit.vTypedMask.begin(), uint8_t( 0 ) );
				m_esEdit.sDigitPos += sDigits;
			}
		}
		else {
			m_esEdit.vTexts.insert( m_esEdit.vTexts.begin(), _sCount, std::wstring() );
			m_esEdit.ui64Start -= uint64_t( _sCount ) * ui64Stride;
			m_esEdit.sItem += _sCount;
		}
		return true;
	}

	/**
	 * Determines whether the edit box can cover a cell (digits) or value.  Process memory cannot grow, so nothing past its end can be
	 *	covered.
	 *
	 * \param _sIdx The index of the cell or value relative to the start of the edit box.
	 * \return Returns true if the cell or value can be part of the edit box.
	 **/
	bool CHexEditorControl::EditCanCover( size_t _sIdx ) const {
		if ( !m_pheiTarget || !m_pheiTarget->OverwriteOnly() ) { return true; }
		const uint64_t ui64Stride = (m_esEdit.ekKind == MX_EK_DIGITS) ? 1 : StrideForFormat( m_esEdit.dfFmt );
		return m_esEdit.ui64Start + (uint64_t( _sIdx ) + 1) * ui64Stride <= Size();
	}

	/**
	 * Removes untyped cells (digits) or empty values from the ends of the edit box.
	 *
	 * \param _bKeepCursor If true, the cell or value under the cursor is kept.  If false and nothing is typed, everything is removed.
	 **/
	void CHexEditorControl::TrimEdit( bool _bKeepCursor ) {
		switch ( m_esEdit.ekKind ) {
			case MX_EK_DIGITS : {
				const size_t sDigits = SubCaretCountForFormat( m_esEdit.dfFmt );
				const size_t sCells = m_esEdit.vTypedMask.size();
				size_t sFirst = sCells, sLast = 0;
				for ( size_t I = 0; I < sCells; ++I ) {
					if ( m_esEdit.vTypedMask[I] ) {
						sFirst = std::min( sFirst, I );
						sLast = I;
					}
				}
				if ( _bKeepCursor ) {
					const size_t sCur = std::min( m_esEdit.sDigitPos / sDigits, sCells ? sCells - 1 : 0 );
					sFirst = std::min( sFirst, sCur );
					sLast = std::max( sLast, sCur );
				}
				if ( sFirst >= sCells ) {
					m_esEdit.vTypedMask.clear();
					m_esEdit.vDigits.clear();
					m_esEdit.vOrigDigits.clear();
					m_esEdit.sDigitPos = 0;
					break;
				}
				m_esEdit.vTypedMask.resize( sLast + 1 );
				m_esEdit.vDigits.resize( (sLast + 1) * sDigits );
				m_esEdit.vOrigDigits.resize( (sLast + 1) * sDigits );
				if ( sFirst ) {
					m_esEdit.vTypedMask.erase( m_esEdit.vTypedMask.begin(), m_esEdit.vTypedMask.begin() + sFirst );
					m_esEdit.vDigits.erase( m_esEdit.vDigits.begin(), m_esEdit.vDigits.begin() + sFirst * sDigits );
					m_esEdit.vOrigDigits.erase( m_esEdit.vOrigDigits.begin(), m_esEdit.vOrigDigits.begin() + sFirst * sDigits );
					m_esEdit.ui64Start += sFirst;
					m_esEdit.sDigitPos = (m_esEdit.sDigitPos >= sFirst * sDigits) ? (m_esEdit.sDigitPos - sFirst * sDigits) : 0;
				}
				m_esEdit.sDigitPos = std::min( m_esEdit.sDigitPos, m_esEdit.vTypedMask.size() * sDigits - 1 );
				break;
			}
			case MX_EK_VALUES : {
				const size_t sItems = m_esEdit.vTexts.size();
				size_t sFirst = sItems, sLast = 0;
				for ( size_t I = 0; I < sItems; ++I ) {
					if ( !m_esEdit.vTexts[I].empty() ) {
						sFirst = std::min( sFirst, I );
						sLast = I;
					}
				}
				if ( _bKeepCursor ) {
					const size_t sCur = std::min( m_esEdit.sItem, sItems ? sItems - 1 : 0 );
					sFirst = std::min( sFirst, sCur );
					sLast = std::max( sLast, sCur );
				}
				if ( sFirst >= sItems ) {
					m_esEdit.vTexts.clear();
					m_esEdit.sItem = 0;
					m_esEdit.sCursor = 0;
					break;
				}
				m_esEdit.vTexts.resize( sLast + 1 );
				if ( sFirst ) {
					m_esEdit.vTexts.erase( m_esEdit.vTexts.begin(), m_esEdit.vTexts.begin() + sFirst );
					m_esEdit.ui64Start += uint64_t( sFirst ) * StrideForFormat( m_esEdit.dfFmt );
					if ( m_esEdit.sItem >= sFirst ) { m_esEdit.sItem -= sFirst; }
					else {
						m_esEdit.sItem = 0;
						m_esEdit.sCursor = 0;
					}
				}
				if ( m_esEdit.sItem >= m_esEdit.vTexts.size() ) {
					m_esEdit.sItem = m_esEdit.vTexts.size() - 1;
					m_esEdit.sCursor = m_esEdit.vTexts[m_esEdit.sItem].size();
				}
				break;
			}
			default : {}
		}
	}

	/**
	 * Writes an automatically committing edit (real-time text editing ending in an escape) as part of the real-time Undo group.
	 **/
	void CHexEditorControl::CommitAutoEdit() {
		if ( !m_esEdit.bActive ) { return; }
		try {
			const std::vector<uint8_t> vBytes = m_esEdit.vPreview;
			const MX_SELECTION sOldSel = m_esEdit.sOldSel;
			const MX_SELECT_GESTURE sgOldGesture = m_esEdit.sgOldGesture;
			const uint64_t ui64Start = m_esEdit.ui64Start;
			const bool bInsert = m_esEdit.bInsert;
			const bool bRight = m_esEdit.bRightArea;
			m_esEdit = MX_EDIT_STATE();
			m_bEditBuffer = CHexEditorInterface::CBuffer();
			if ( vBytes.empty() ) {
				m_sSel = sOldSel;
				m_sgSelGesture = sgOldGesture;
				::InvalidateRect( Wnd(), nullptr, FALSE );
				return;
			}

			BeginRealTimeUndo( sOldSel, sgOldGesture );
			CSecureWString swsStatus;
			if ( !WriteBytes( ui64Start, vBytes, bInsert, swsStatus ) ) {
				EditWarning( swsStatus );
			}
			m_sgSelGesture.bRightArea = bRight;
			SetEditedCaret( ui64Start + vBytes.size(), 0 );
		}
		catch ( ... ) {}
	}

	/**
	 * Makes sure the edit box holds the digits of a cell (MX_EK_DIGITS), reading its original value if needed.
	 *
	 * \param _sCell The index of the cell relative to the start of the edit.
	 **/
	void CHexEditorControl::EnsureEditCell( size_t _sCell ) {
		const size_t sDigits = SubCaretCountForFormat( m_esEdit.dfFmt );
		const uint32_t ui32Radix = RadixForFormat( m_esEdit.dfFmt );
		while ( m_esEdit.vTypedMask.size() <= _sCell ) {
			const uint64_t ui64Addr = m_esEdit.ui64Start + m_esEdit.vTypedMask.size();
			// Inserted bytes, and bytes past the end, start at 0.
			uint32_t ui32Val = 0;
			if ( !m_esEdit.bInsert && ui64Addr < Size() && m_pheiTarget ) {
				CHexEditorInterface::CBuffer bTmp;
				if ( m_pheiTarget->Read( ui64Addr, bTmp, 1 ) && bTmp.size() ) { ui32Val = bTmp[0]; }
			}
			uint8_t ui8Digits[8] = {};
			for ( size_t I = sDigits; I--; ) {
				ui8Digits[I] = uint8_t( ui32Val % ui32Radix );
				ui32Val /= ui32Radix;
			}
			for ( size_t I = 0; I < sDigits; ++I ) {
				m_esEdit.vDigits.push_back( ui8Digits[I] );
				m_esEdit.vOrigDigits.push_back( ui8Digits[I] );
			}
			m_esEdit.vTypedMask.push_back( 0 );
		}
	}

	/**
	 * Recomputes the bytes the edit box's text encodes to (MX_EK_TEXT).
	 **/
	void CHexEditorControl::UpdateEditPreview() {
		std::u32string u32Resolved;
		m_esEdit.bEscapeOpen = ResolveEditEscapes( m_esEdit.u32Text, u32Resolved );
		m_esEdit.vPreview.clear();
		EncodeText( u32Resolved, m_esEdit.vPreview );
	}

	/**
	 * Moves the caret to the edit box's cursor (where the next character typed goes) and scrolls it into view.
	 **/
	void CHexEditorControl::UpdateEditCaret() {
		if ( !m_esEdit.bActive ) { return; }
		uint64_t ui64Caret = m_esEdit.ui64Start;
		int32_t i32Idx = 0;
		switch ( m_esEdit.ekKind ) {
			case MX_EK_DIGITS : {
				const size_t sDigits = SubCaretCountForFormat( m_esEdit.dfFmt );
				ui64Caret += m_esEdit.sDigitPos / sDigits;
				i32Idx = int32_t( m_esEdit.sDigitPos % sDigits );
				break;
			}
			case MX_EK_TEXT : {
				// Under the first byte of the character at the cursor.
				size_t sBytes = m_esEdit.vPreview.size();
				if ( m_esEdit.sCursor < m_esEdit.u32Text.size() ) {
					std::u32string u32Resolved;
					std::vector<uint8_t> vPrefix;
					ResolveEditEscapes( m_esEdit.u32Text.substr( 0, m_esEdit.sCursor ), u32Resolved );
					EncodeText( u32Resolved, vPrefix );
					sBytes = std::min( sBytes, vPrefix.size() );
				}
				ui64Caret += sBytes;
				break;
			}
			case MX_EK_VALUES : {
				ui64Caret += uint64_t( m_esEdit.sItem ) * StrideForFormat( m_esEdit.dfFmt );
				break;
			}
		}
		m_sgSelGesture.bRightArea = m_esEdit.bRightArea;
		m_sgSelGesture.ui64CaretAddr = ui64Caret;
		m_sgSelGesture.i32CaretIdx = i32Idx;
		m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = ui64Caret;
		StartCaretBlink();
		EnsureVisible( ui64Caret, FALSE );
	}

	/**
	 * Gets the address just past the edit box.  The edit box covers what was typed plus the cell where the next character goes.
	 *
	 * \return Returns the end of the edit box (exclusive).
	 **/
	uint64_t CHexEditorControl::EditEnd() const {
		if ( !m_esEdit.bActive ) { return 0; }
		switch ( m_esEdit.ekKind ) {
			case MX_EK_DIGITS : { return m_esEdit.ui64Start + m_esEdit.vTypedMask.size(); }
			case MX_EK_TEXT : { return m_esEdit.ui64Start + m_esEdit.vPreview.size() + StrideForFormat( m_esEdit.dfFmt ); }
			case MX_EK_VALUES : { return m_esEdit.ui64Start + uint64_t( m_esEdit.vTexts.size() ) * StrideForFormat( m_esEdit.dfFmt ); }
			default : { return m_esEdit.ui64Start; }
		}
	}

	/**
	 * Determines whether a cell is inside the edit box.
	 *
	 * \param _ui64Address The address of the cell.
	 * \param _bRightArea The right area or the left area.
	 * \return Returns true if the cell is inside the edit box.
	 **/
	bool CHexEditorControl::EditCovers( uint64_t _ui64Address, bool _bRightArea ) const {
		return m_esEdit.bActive && m_esEdit.bRightArea == _bRightArea &&
			_ui64Address >= m_esEdit.ui64Start && _ui64Address < EditEnd();
	}

	/**
	 * Determines whether anything was typed into a cell inside the edit box.
	 *
	 * \param _ui64Address The address of the cell.
	 * \return Returns true if the cell was typed into (wholly or partly).
	 **/
	bool CHexEditorControl::EditCellTyped( uint64_t _ui64Address ) const {
		if ( !m_esEdit.bActive || _ui64Address < m_esEdit.ui64Start ) { return false; }
		const uint64_t ui64Off = _ui64Address - m_esEdit.ui64Start;
		switch ( m_esEdit.ekKind ) {
			case MX_EK_DIGITS : { return ui64Off < m_esEdit.vTypedMask.size() && m_esEdit.vTypedMask[size_t( ui64Off )] != 0; }
			case MX_EK_TEXT : { return ui64Off < m_esEdit.vPreview.size(); }
			case MX_EK_VALUES : {
				const uint64_t ui64Idx = ui64Off / StrideForFormat( m_esEdit.dfFmt );
				return ui64Idx < m_esEdit.vTexts.size() && !m_esEdit.vTexts[size_t( ui64Idx )].empty();
			}
			default : { return false; }
		}
	}

	/**
	 * Gets the bytes the edit box would write.
	 *
	 * \param _vBytes Receives the bytes.
	 * \param _swsStatus Receives an error message if a typed value is invalid.
	 * \return Returns false if a typed value is invalid.
	 **/
	bool CHexEditorControl::EditBytes( std::vector<uint8_t> &_vBytes, CSecureWString &_swsStatus ) const {
		_vBytes.clear();
		if ( !m_esEdit.bActive ) { return true; }
		switch ( m_esEdit.ekKind ) {
			case MX_EK_DIGITS : {
				const size_t sDigits = SubCaretCountForFormat( m_esEdit.dfFmt );
				const uint32_t ui32Radix = RadixForFormat( m_esEdit.dfFmt );
				// Every cell up to the last one typed into (the cell after it only shows where the next digit goes).
				size_t sCells = m_esEdit.vTypedMask.size();
				while ( sCells && m_esEdit.vTypedMask[sCells-1] == 0 ) { --sCells; }
				for ( size_t I = 0; I < sCells; ++I ) {
					uint32_t ui32Val = 0;
					for ( size_t J = 0; J < sDigits; ++J ) {
						ui32Val = ui32Val * ui32Radix + m_esEdit.vDigits[I*sDigits+J];
					}
					if ( ui32Val > 0xFF ) {
						// Octal and decimal digits can describe values that do not fit in a byte.
						_swsStatus = _DEC_WS_CEE34699_Invalid_input__;
						for ( size_t J = 0; J < sDigits; ++J ) {
							_swsStatus.push_back( wchar_t( L'0' + m_esEdit.vDigits[I*sDigits+J] ) );
						}
						return false;
					}
					_vBytes.push_back( uint8_t( ui32Val ) );
				}
				return true;
			}
			case MX_EK_TEXT : {
				_vBytes = m_esEdit.vPreview;
				return true;
			}
			case MX_EK_VALUES : {
				const uint32_t ui32Stride = StrideForFormat( m_esEdit.dfFmt );
				for ( size_t I = 0; I < m_esEdit.vTexts.size(); ++I ) {
					const std::wstring &wsThis = m_esEdit.vTexts[I];
					if ( wsThis.empty() ) {
						// A value skipped over stays as it is (an inserted one is 0).
						const size_t sOff = _vBytes.size();
						_vBytes.resize( sOff + ui32Stride, 0 );
						const uint64_t ui64Addr = m_esEdit.ui64Start + uint64_t( I ) * ui32Stride;
						if ( !m_esEdit.bInsert && ui64Addr < Size() && m_pheiTarget ) {
							CHexEditorInterface::CBuffer bTmp;
							if ( m_pheiTarget->Read( ui64Addr, bTmp, ui32Stride ) ) {
								for ( size_t J = 0; J < std::min<size_t>( bTmp.size(), ui32Stride ); ++J ) { _vBytes[sOff+J] = bTmp[J]; }
							}
						}
						continue;
					}
					if ( !ParseValue( m_esEdit.dfFmt, wsThis, _vBytes ) ) {
						_swsStatus = _DEC_WS_CEE34699_Invalid_input__;
						_swsStatus += wsThis;
						return false;
					}
				}
				return true;
			}
			default : { return true; }
		}
	}

	/**
	 * Gets the buffer to display: the data read for the screen, with the bytes the edit box would write in place (and, in Insert
	 *	mode, the data after the edit box pushed along).
	 * 
	 * \param _ui64BufAddr The address of the first byte in m_bCurBuffer.
	 * \param _sBufCap The number of bytes the screen shows.
	 * \return Returns the buffer to display.
	 **/
	const CHexEditorInterface::CBuffer & CHexEditorControl::DisplayBuffer( uint64_t _ui64BufAddr, size_t _sBufCap ) {
		if ( !m_esEdit.bActive ) { return m_bCurBuffer; }
		try {
			const uint64_t ui64BufEnd = _ui64BufAddr + _sBufCap;
			const uint64_t ui64EditEnd = EditEnd();
			const uint64_t ui64EditLen = ui64EditEnd - m_esEdit.ui64Start;
			if ( m_esEdit.bInsert ) {
				// The edit box's cells are new, so the data from its start onward comes after them.
				m_bEditBuffer.clear();
				const uint64_t ui64End = std::min( ui64BufEnd, Size() + ui64EditLen );
				if ( ui64End > _ui64BufAddr ) {
					m_bEditBuffer.resize( size_t( ui64End - _ui64BufAddr ), 0 );
					const uint64_t ui64Before = std::min( ui64End, m_esEdit.ui64Start );
					for ( uint64_t A = _ui64BufAddr; A < ui64Before; ++A ) {
						const size_t sIdx = size_t( A - _ui64BufAddr );
						if ( sIdx < m_bCurBuffer.size() ) { m_bEditBuffer[sIdx] = m_bCurBuffer[sIdx]; }
					}
					const uint64_t ui64After = std::max( _ui64BufAddr, ui64EditEnd );
					if ( ui64End > ui64After && m_pheiTarget ) {
						CHexEditorInterface::CBuffer bTmp;
						if ( m_pheiTarget->Read( ui64After - ui64EditLen, bTmp, size_t( ui64End - ui64After ) ) ) {
							const size_t sCopy = std::min( bTmp.size(), size_t( ui64End - ui64After ) );
							for ( size_t I = 0; I < sCopy; ++I ) {
								m_bEditBuffer[size_t( ui64After - _ui64BufAddr )+I] = bTmp[I];
							}
						}
					}
				}
			}
			else {
				m_bEditBuffer = m_bCurBuffer;
				// Make room for the edit box where it runs past the end of the data (so that its cells are drawn).
				const uint64_t ui64Need = std::min( ui64EditEnd, ui64BufEnd );
				if ( ui64Need > _ui64BufAddr && ui64Need - _ui64BufAddr > m_bEditBuffer.size() ) {
					m_bEditBuffer.resize( size_t( ui64Need - _ui64BufAddr ), 0 );
				}
			}

			// The bytes the edit box would write.
			auto Put = [&]( uint64_t _ui64Addr, uint8_t _ui8Val ) {
				if ( _ui64Addr >= _ui64BufAddr && _ui64Addr - _ui64BufAddr < m_bEditBuffer.size() ) {
					m_bEditBuffer[size_t( _ui64Addr - _ui64BufAddr )] = _ui8Val;
				}
			};
			const uint64_t ui64From = std::max( m_esEdit.ui64Start, _ui64BufAddr );
			switch ( m_esEdit.ekKind ) {
				case MX_EK_DIGITS : {
					const size_t sDigits = SubCaretCountForFormat( m_esEdit.dfFmt );
					const uint32_t ui32Radix = RadixForFormat( m_esEdit.dfFmt );
					const uint64_t ui64To = std::min( m_esEdit.ui64Start + m_esEdit.vTypedMask.size(), ui64BufEnd );
					for ( uint64_t A = ui64From; A < ui64To; ++A ) {
						const size_t sCell = size_t( A - m_esEdit.ui64Start );
						uint32_t ui32Val = 0;
						for ( size_t J = 0; J < sDigits; ++J ) {
							ui32Val = ui32Val * ui32Radix + m_esEdit.vDigits[sCell*sDigits+J];
						}
						Put( A, uint8_t( std::min<uint32_t>( ui32Val, 0xFF ) ) );
					}
					break;
				}
				case MX_EK_TEXT : {
					const uint64_t ui64To = std::min( m_esEdit.ui64Start + m_esEdit.vPreview.size(), ui64BufEnd );
					for ( uint64_t A = ui64From; A < ui64To; ++A ) {
						Put( A, m_esEdit.vPreview[size_t( A - m_esEdit.ui64Start )] );
					}
					break;
				}
				case MX_EK_VALUES : {
					// Each value typed so far that is valid (the area being typed in shows the typed text instead).
					const uint32_t ui32Stride = StrideForFormat( m_esEdit.dfFmt );
					std::vector<uint8_t> vValue;
					for ( size_t I = 0; I < m_esEdit.vTexts.size(); ++I ) {
						const uint64_t ui64Addr = m_esEdit.ui64Start + uint64_t( I ) * ui32Stride;
						if ( ui64Addr >= ui64BufEnd ) { break; }
						if ( ui64Addr + ui32Stride <= _ui64BufAddr || m_esEdit.vTexts[I].empty() ) { continue; }
						vValue.clear();
						if ( !ParseValue( m_esEdit.dfFmt, m_esEdit.vTexts[I], vValue ) ) { continue; }
						for ( size_t J = 0; J < vValue.size(); ++J ) { Put( ui64Addr + J, vValue[J] ); }
					}
					break;
				}
				default : {}
			}
			return m_bEditBuffer;
		}
		catch ( ... ) { return m_bCurBuffer; }
	}

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
	bool CHexEditorControl::DrawEditCellText( HDC _hDc, uint64_t _ui64Address, MX_DATA_FMT _dfFmt, int32_t _iX, int32_t _iY, const std::wstring &_wsText, const uint8_t * _pui8Value, size_t _sSize ) {
		const bool bTyped = EditCellTyped( _ui64Address );
		// New cells (inserted, or past the end of the data) show only what was typed.
		const bool bNew = m_esEdit.bInsert || _ui64Address >= Size();
		switch ( m_esEdit.ekKind ) {
			case MX_EK_DIGITS : {
				if ( !bTyped ) { return bNew; }		// Otherwise wholly faded: drawn normally with the color CellFgColor() gives.
				const size_t sDigits = SubCaretCountForFormat( m_esEdit.dfFmt );
				const size_t sCell = size_t( _ui64Address - m_esEdit.ui64Start );
				const uint8_t ui8Mask = m_esEdit.vTypedMask[sCell];
				const COLORREF crNormal = MX_GetRgbValue( CellFgColor( _ui64Address, _pui8Value, _sSize, m_esEdit.bRightArea, false ) );
				const COLORREF crFaded = EditFadeColor( crNormal, MX_GetRgbValue( CellBgColor( _ui64Address, _pui8Value, _sSize, m_esEdit.bRightArea ) ) );
				wchar_t wcDigits[8];
				for ( size_t J = 0; J < sDigits; ++J ) {
					const uint8_t ui8Digit = m_esEdit.vDigits[sCell*sDigits+J];
					wcDigits[J] = wchar_t( ui8Digit < 10 ? (L'0' + ui8Digit) : (L'A' + (ui8Digit - 10)) );
				}
				// Each digit goes where it would be if the digits were drawn together, the way cells are normally drawn.
				for ( size_t J = 0; J < sDigits; ++J ) {
					SIZE sPrefix = { 0, 0 };
					if ( J ) { ::GetTextExtentPoint32W( _hDc, wcDigits, int( J ), &sPrefix ); }
					lsw::LSW_SETTEXTCOLOR stcColor( _hDc, ((ui8Mask >> J) & 1) ? crNormal : crFaded );
					::TextOutW( _hDc, _iX + sPrefix.cx, _iY, &wcDigits[J], 1 );
				}
				return true;
			}
			case MX_EK_VALUES : {
				const size_t sIdx = size_t( (_ui64Address - m_esEdit.ui64Start) / StrideForFormat( m_esEdit.dfFmt ) );
				const bool bCurrent = sIdx == m_esEdit.sItem;
				if ( !bTyped && !bCurrent ) { return bNew; }
				// The typed text, or (for the value under the cursor before anything is typed into it) what is there now, faded.
				const std::wstring &wsShown = bTyped ? m_esEdit.vTexts[sIdx] : _wsText;
				if ( bTyped || !bNew ) {
					lsw::LSW_SETTEXTCOLOR stcColor( _hDc, MX_GetRgbValue( CellFgColor( _ui64Address, _pui8Value, _sSize, m_esEdit.bRightArea ) ) );
					::TextOutW( _hDc, _iX, _iY, wsShown.c_str(), static_cast<int>(wsShown.size()) );
				}
				if ( bCurrent && m_bCaretOn ) {
					// The text cursor.
					SIZE sPrefix = { 0, 0 };
					if ( bTyped && m_esEdit.sCursor ) {
						::GetTextExtentPoint32W( _hDc, wsShown.c_str(), int( std::min( m_esEdit.sCursor, wsShown.size() ) ), &sPrefix );
					}
					TEXTMETRICW tmMetrics = {};
					::GetTextMetricsW( _hDc, &tmMetrics );
					lsw::LSW_RECT rBar;
					rBar.left = _iX + sPrefix.cx;
					rBar.right = rBar.left + 2;
					rBar.top = _iY;
					rBar.bottom = _iY + (tmMetrics.tmHeight ? tmMetrics.tmHeight : 1);
					::FillRect( _hDc, &rBar, lsw::CBase::BrushCache().Brush( MX_GetRgbValue( ForeColors()->crCaret ) ) );
				}
				return true;
			}
			default : { return !bTyped && bNew; }		// Text: the preview bytes are already in the displayed buffer.
		}
	}

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
	void CHexEditorControl::DrawEditBoxRow( HDC _hDc, MX_DATA_FMT _dfFmt, int32_t _iXLeft, int32_t _iAreaW, int32_t _iYTop, uint32_t _ui32Row, int32_t _iLineAdv ) {
		if ( !m_esEdit.bActive || _iLineAdv <= 0 ) { return; }
		const uint32_t ui32Bpr = CurStyle()->uiBytesPerRow ? CurStyle()->uiBytesPerRow : 16;
		const uint64_t ui64End = EditEnd();
		if ( ui64End <= m_esEdit.ui64Start ) { return; }
		const uint64_t ui64FirstRow = m_esEdit.ui64Start / ui32Bpr;
		const uint64_t ui64LastRow = (ui64End - 1) / ui32Bpr;
		const uint64_t ui64TopLine = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine;
		const uint64_t ui64Line = ui64TopLine + _ui32Row;
		// Only the rows of the edit box and the rows just above and below it have anything to draw.
		if ( ui64Line + 1 < ui64FirstRow || ui64Line > ui64LastRow + 1 ) { return; }

		const int32_t i32BandT = _iYTop + int32_t( _ui32Row ) * _iLineAdv;
		const int32_t i32BandB = i32BandT + _iLineAdv;
		const int32_t i32AreaL = _iXLeft;
		const int32_t i32AreaR = _iXLeft + _iAreaW;

		// The edit box's extent on its rows next to this one.
		/** The edit box's rectangle on one row. */
		struct MX_BOX_ROW {
			int32_t									i32L;												/**< The left (inclusive). */
			int32_t									i32R;												/**< The right (exclusive). */
			int32_t									i32T;												/**< The top (inclusive). */
			int32_t									i32B;												/**< The bottom (exclusive). */
		};
		MX_BOX_ROW brRows[3];
		size_t sRows = 0;
		for ( uint64_t K = (ui64Line ? ui64Line - 1 : 0); K <= ui64Line + 1; ++K ) {
			if ( K < ui64FirstRow || K > ui64LastRow ) { continue; }
			const uint32_t ui32C0 = (K == ui64FirstRow) ? uint32_t( m_esEdit.ui64Start % ui32Bpr ) : 0;
			const uint32_t ui32C1 = (K == ui64LastRow) ? uint32_t( (ui64End - 1) % ui32Bpr ) : (ui32Bpr - 1);
			int32_t i32L0, i32W0, i32L1, i32W1;
			GetBackgrondRectForIndex( _dfFmt, ui32C0, _iXLeft, i32L0, i32W0 );
			GetBackgrondRectForIndex( _dfFmt, ui32C1, _iXLeft, i32L1, i32W1 );

			MX_BOX_ROW &brRow = brRows[sRows++];
			// Nothing outside the area belongs to it, so a border there is moved inside the edit box.
			brRow.i32L = std::max( i32L0, i32AreaL + 2 );
			brRow.i32R = std::min( i32L1 + i32W1, i32AreaR - 2 );
			brRow.i32T = _iYTop + int32_t( int64_t( K ) - int64_t( ui64TopLine ) ) * _iLineAdv;
			brRow.i32B = brRow.i32T + _iLineAdv;
			// Nothing is drawn above the first visible row, so its top border is moved inside it.
			if ( K == ui64FirstRow && K == ui64TopLine ) { brRow.i32T += 2; }

			// Wide spacing between groups leaves gaps between cells; fill those inside the edit box.
			if ( K == ui64Line ) {
				for ( uint32_t C = ui32C0; C < ui32C1; ++C ) {
					int32_t i32La, i32Wa, i32Lb, i32Wb;
					GetBackgrondRectForIndex( _dfFmt, C, _iXLeft, i32La, i32Wa );
					GetBackgrondRectForIndex( _dfFmt, C + 1, _iXLeft, i32Lb, i32Wb );
					if ( i32La + i32Wa < i32Lb ) {
						lsw::LSW_RECT rGap;
						rGap.left = i32La + i32Wa;
						rGap.right = i32Lb;
						rGap.top = i32BandT;
						rGap.bottom = i32BandB;
						const COLORREF crGap = CellBgColor( K * ui32Bpr + C, nullptr, 0, m_esEdit.bRightArea );
						::FillRect( _hDc, &rGap, lsw::CBase::BrushCache().Brush( MX_GetRgbValue( crGap ) ) );
					}
				}
			}
		}
		if ( !sRows ) { return; }

		// A sunken border: dark above and to the left, light below and to the right, two pixels deep.
		const COLORREF crBox = MX_GetRgbValue( CellBgColor( m_esEdit.ui64Start, nullptr, 0, m_esEdit.bRightArea ) );
		auto Blend = [crBox]( COLORREF _crTo, uint32_t _ui32Amount ) -> COLORREF {
			return RGB( (GetRValue( crBox ) * (255 - _ui32Amount) + GetRValue( _crTo ) * _ui32Amount) / 255,
				(GetGValue( crBox ) * (255 - _ui32Amount) + GetGValue( _crTo ) * _ui32Amount) / 255,
				(GetBValue( crBox ) * (255 - _ui32Amount) + GetBValue( _crTo ) * _ui32Amount) / 255 );
		};
		const COLORREF crOuterDark = Blend( RGB( 0, 0, 0 ), 90 );
		const COLORREF crInnerDark = Blend( RGB( 0, 0, 0 ), 150 );
		const COLORREF crOuterLight = Blend( RGB( 0xFF, 0xFF, 0xFF ), 230 );
		const COLORREF crInnerLight = Blend( RGB( 0xFF, 0xFF, 0xFF ), 120 );

		int32_t i32Pts[3*6+2];
		for ( int32_t Y = i32BandT; Y < i32BandB; ++Y ) {
			// Where the border can begin or end along this line of pixels.
			size_t sPts = 0;
			i32Pts[sPts++] = i32AreaL;
			i32Pts[sPts++] = i32AreaR;
			for ( size_t R = 0; R < sRows; ++R ) {
				i32Pts[sPts++] = brRows[R].i32L - 2;
				i32Pts[sPts++] = brRows[R].i32L - 1;
				i32Pts[sPts++] = brRows[R].i32L;
				i32Pts[sPts++] = brRows[R].i32R;
				i32Pts[sPts++] = brRows[R].i32R + 1;
				i32Pts[sPts++] = brRows[R].i32R + 2;
			}
			std::sort( i32Pts, i32Pts + sPts );
			sPts = size_t( std::unique( i32Pts, i32Pts + sPts ) - i32Pts );

			int32_t i32RunL = 0, i32RunR = 0;
			COLORREF crRun = 0;
			bool bRun = false;
			auto Flush = [&]() {
				if ( bRun && i32RunL < i32RunR ) {
					lsw::LSW_RECT rLine;
					rLine.left = i32RunL;
					rLine.right = i32RunR;
					rLine.top = Y;
					rLine.bottom = Y + 1;
					::FillRect( _hDc, &rLine, lsw::CBase::BrushCache().Brush( crRun ) );
				}
				bRun = false;
			};
			for ( size_t P = 0; P + 1 < sPts; ++P ) {
				const int32_t X = i32Pts[P];
				const int32_t XEnd = i32Pts[P+1];
				if ( X < i32AreaL || XEnd > i32AreaR ) { Flush(); continue; }

				// The closest box row (Chebyshev distance) decides what this stretch of pixels is.  On a tie, a straight edge wins over
				//	a corner, so that the border runs on unbroken where two box rows meet.
				int32_t i32Best = 3, i32BestMin = 3;
				bool bDark = false;
				for ( size_t R = 0; R < sRows; ++R ) {
					const MX_BOX_ROW &brRow = brRows[R];
					const int32_t i32Dx = (X < brRow.i32L) ? (brRow.i32L - X) : ((X >= brRow.i32R) ? (X - brRow.i32R + 1) : 0);
					const int32_t i32Dy = (Y < brRow.i32T) ? (brRow.i32T - Y) : ((Y >= brRow.i32B) ? (Y - brRow.i32B + 1) : 0);
					const int32_t i32D = std::max( i32Dx, i32Dy );
					const int32_t i32Min = std::min( i32Dx, i32Dy );
					if ( i32D < i32Best || (i32D == i32Best && i32Min < i32BestMin) ) {
						i32Best = i32D;
						i32BestMin = i32Min;
						if ( i32Dy > i32Dx ) { bDark = Y < brRow.i32T; }
						else if ( i32Dx > i32Dy ) { bDark = X < brRow.i32L; }
						else { bDark = (Y < brRow.i32T) && (X < brRow.i32L); }
					}
				}
				if ( i32Best == 0 || i32Best > 2 ) { Flush(); continue; }
				const COLORREF crThis = bDark ? (i32Best == 1 ? crInnerDark : crOuterDark) : (i32Best == 1 ? crInnerLight : crOuterLight);
				if ( bRun && crThis == crRun && i32RunR == X ) {
					i32RunR = XEnd;
				}
				else {
					Flush();
					bRun = true;
					crRun = crThis;
					i32RunL = X;
					i32RunR = XEnd;
				}
			}
			Flush();
		}
	}

	/**
	 * Gets the color of the edit box from the color of the cell beneath it: a little whiter, or a little darker if the cell is white.
	 *
	 * \param _crBase The cell's background color.
	 * \return Returns the edit box's color.
	 **/
	COLORREF CHexEditorControl::EditBoxColor( COLORREF _crBase ) {
		const uint32_t ui32R = GetRValue( _crBase ), ui32G = GetGValue( _crBase ), ui32B = GetBValue( _crBase );
		const uint32_t ui32Luma = (ui32R * 299 + ui32G * 587 + ui32B * 114) / 1000;
		COLORREF crRet;
		if ( ui32Luma >= 0xF0 ) {
			// White (or nearly): a little darker.
			constexpr uint32_t ui32Keep = 235;
			crRet = RGB( ui32R * ui32Keep / 255, ui32G * ui32Keep / 255, ui32B * ui32Keep / 255 );
		}
		else {
			// A little whiter.
			constexpr uint32_t ui32White = 95;
			crRet = RGB( ui32R + (255 - ui32R) * ui32White / 255, ui32G + (255 - ui32G) * ui32White / 255, ui32B + (255 - ui32B) * ui32White / 255 );
		}
		return crRet | (_crBase & 0xFF000000);
	}

	/**
	 * Fades a text color as if a translucent pane in the edit box's color lay over it.
	 *
	 * \param _crText The text color.
	 * \param _crBox The edit box's color.
	 * \return Returns the faded text color.
	 **/
	COLORREF CHexEditorControl::EditFadeColor( COLORREF _crText, COLORREF _crBox ) {
		auto Blend = []( uint32_t _ui32Text, uint32_t _ui32Box ) -> uint32_t {
			constexpr uint32_t ui32Pane = 140;		// How opaque the pane is (of 255).
			return (_ui32Text * (255 - ui32Pane) + _ui32Box * ui32Pane) / 255;
		};
		return RGB( Blend( GetRValue( _crText ), GetRValue( _crBox ) ),
			Blend( GetGValue( _crText ), GetGValue( _crBox ) ),
			Blend( GetBValue( _crText ), GetBValue( _crBox ) ) ) | (_crText & 0xFF000000);
	}

	/**
	 * Writes bytes to the data source: inserted or written over existing bytes.
	 *
	 * \param _ui64Addr The address at which to write.
	 * \param _vBytes The bytes to write.
	 * \param _bInsert If true and the data source allows it, the bytes are inserted.
	 * \param _swsStatus Receives an error message on failure.
	 * \return Returns true if the bytes were written.
	 **/
	bool CHexEditorControl::WriteBytes( uint64_t _ui64Addr, const std::vector<uint8_t> &_vBytes, bool _bInsert, CSecureWString &_swsStatus ) {
		if ( !m_pheiTarget ) {
			_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		if ( m_pheiTarget->ReadOnly() ) {
			_swsStatus = _DEC_WS_D4FFE2E0_File_is_Read_Only;
			return false;
		}
		if ( _vBytes.empty() ) { return true; }
		const uint64_t ui64OldSize = Size();
		bool bOk;
		if ( _bInsert && !m_pheiTarget->OverwriteOnly() ) {
			uint64_t ui64Inserted = 0;
			bOk = m_pheiTarget->Insert( _ui64Addr, _vBytes, ui64Inserted ) && ui64Inserted == _vBytes.size();
		}
		else {
			bOk = m_pheiTarget->Overwrite( _ui64Addr, _vBytes.data(), _vBytes.size() );
		}
		if ( !bOk ) { _swsStatus = _DEC_WS_58763A46_Failed_to_write_to_the_target_; }
		else if ( _bInsert && !m_pheiTarget->OverwriteOnly() ) { ShiftChanges( _ui64Addr, _vBytes.size(), true ); }
		else { MarkChanged( _ui64Addr, _vBytes.size() ); }
		if ( Size() != ui64OldSize ) {
			UpdateScrollbars();
			if ( m_pwHexParent ) { m_pwHexParent->UpdateStatusBar_Size(); }
		}
		::InvalidateRect( Wnd(), nullptr, FALSE );
		return bOk;
	}

	/**
	 * Opens the Undo group that collects real-time edits, if it is not open, and restarts the timer that ends it.
	 *
	 * \param _sOldSel The selection before the edit.
	 * \param _sgOldGesture The caret and anchors before the edit.
	 **/
	void CHexEditorControl::BeginRealTimeUndo( const MX_SELECTION &_sOldSel, const MX_SELECT_GESTURE &_sgOldGesture ) {
		if ( !m_pheiTarget ) { return; }
		if ( !m_bRtUndoOpen ) {
			// If the data source has no Undo, BeginUndo() fails and nothing is collected.
			if ( !m_pheiTarget->BeginUndo() ) { return; }
			m_bRtUndoOpen = true;
			m_sRtOldSel = _sOldSel;
			m_sgRtOldGesture = _sgOldGesture;
		}
		// The Undo item is completed 0.2 seconds after typing stops.
		m_tRtUndo.Start( Wnd(), MX_TI_HEX_EDIT_UNDO, 200, nullptr );
	}

	/**
	 * Edits one digit in real time (hex and binary).
	 *
	 * \param _ui64Addr The address of the byte.
	 * \param _i32Idx The digit within the byte (0 is the most significant).
	 * \param _dfFmt The format being typed.
	 * \param _ui32Digit The digit's value.
	 **/
	void CHexEditorControl::RealTimeDigit( uint64_t _ui64Addr, int32_t _i32Idx, MX_DATA_FMT _dfFmt, uint32_t _ui32Digit ) {
		try {
			const uint32_t ui32Digits = SubCaretCountForFormat( _dfFmt );
			const uint32_t ui32Radix = RadixForFormat( _dfFmt );
			const uint32_t ui32Idx = uint32_t( std::clamp<int32_t>( _i32Idx, 0, int32_t( ui32Digits ) - 1 ) );
			// A new byte is inserted when typing its first digit in Insert mode, or past the end of the data.
			const bool bInsert = _ui64Addr >= Size() || (!IsOverwriteMode() && !m_pheiTarget->OverwriteOnly() && ui32Idx == 0);
			uint32_t ui32Val = 0;
			if ( !bInsert ) {
				CHexEditorInterface::CBuffer bTmp;
				if ( !m_pheiTarget->Read( _ui64Addr, bTmp, 1 ) || bTmp.empty() ) { return; }
				ui32Val = bTmp[0];
			}
			// Replace the digit (radix 16 and 2 always fit in a byte).
			uint32_t ui32Weight = 1;
			for ( uint32_t I = ui32Idx + 1; I < ui32Digits; ++I ) { ui32Weight *= ui32Radix; }
			ui32Val = ui32Val - ((ui32Val / ui32Weight) % ui32Radix) * ui32Weight + _ui32Digit * ui32Weight;

			const MX_SELECTION sOldSel = m_sSel;
			const MX_SELECT_GESTURE sgOldGesture = m_sgSelGesture;
			BeginRealTimeUndo( sOldSel, sgOldGesture );
			ClearSelection();
			CSecureWString swsStatus;
			if ( !WriteBytes( _ui64Addr, std::vector<uint8_t>( 1, uint8_t( ui32Val ) ), bInsert, swsStatus ) ) {
				EditWarning( swsStatus );
				return;
			}
			if ( ui32Idx + 1 < ui32Digits ) { SetEditedCaret( _ui64Addr, int32_t( ui32Idx + 1 ) ); }
			else { SetEditedCaret( _ui64Addr + 1, 0 ); }
		}
		catch ( ... ) {}
	}

	/**
	 * Edits one character in real time.
	 *
	 * \param _ui64Addr The address of the character.
	 * \param _c32Cp The character.
	 **/
	void CHexEditorControl::RealTimeText( uint64_t _ui64Addr, char32_t _c32Cp ) {
		try {
			std::vector<uint8_t> vBytes;
			EncodeText( std::u32string( 1, _c32Cp ), vBytes );
			if ( vBytes.empty() ) { return; }
			const bool bInsert = _ui64Addr >= Size() || (!IsOverwriteMode() && !m_pheiTarget->OverwriteOnly());

			const MX_SELECTION sOldSel = m_sSel;
			const MX_SELECT_GESTURE sgOldGesture = m_sgSelGesture;
			BeginRealTimeUndo( sOldSel, sgOldGesture );
			ClearSelection();
			CSecureWString swsStatus;
			if ( !WriteBytes( _ui64Addr, vBytes, bInsert, swsStatus ) ) {
				EditWarning( swsStatus );
				return;
			}
			SetEditedCaret( _ui64Addr + vBytes.size(), 0 );
		}
		catch ( ... ) {}
	}

	/**
	 * Puts the caret after a real-time edit.
	 *
	 * \param _ui64Addr The new caret address.
	 * \param _i32Idx The new sub-caret.
	 **/
	void CHexEditorControl::SetEditedCaret( uint64_t _ui64Addr, int32_t _i32Idx ) {
		ClearSelection();
		m_sgSelGesture.ui64CaretAddr = std::min( _ui64Addr, Size() );
		m_sgSelGesture.i32CaretIdx = (m_sgSelGesture.ui64CaretAddr == _ui64Addr) ? _i32Idx : 0;
		m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64CaretAddr;
		StartCaretBlink();
		EnsureVisible( m_sgSelGesture.ui64CaretAddr, FALSE );
		::InvalidateRect( Wnd(), nullptr, FALSE );
		if ( m_pwHexParent ) { m_pwHexParent->UpdateStatusBar_PosValue_StartSize(); }
	}

	/**
	 * Reports a problem in the status bar.
	 *
	 * \param _swsStatus The message.
	 **/
	void CHexEditorControl::EditWarning( const CSecureWString &_swsStatus ) {
		if ( m_pwHexParent && _swsStatus.size() ) {
			m_pwHexParent->SetStatusBarText( _swsStatus.c_str(), true );
		}
	}

	/**
	 * Encodes text with the view's character set.  Characters the character set lacks become their closest match (best fit).
	 *
	 * \param _u32Text The text.
	 * \param _vBytes Receives the encoded bytes (appended).
	 **/
	void CHexEditorControl::EncodeText( const std::u32string &_u32Text, std::vector<uint8_t> &_vBytes ) const {
		const CCharSets::MX_CHAR_SETS csSet = CurStyle()->csCharSet;
		auto Valid = []( char32_t _c32Cp ) -> char32_t {
			return (_c32Cp > 0x10FFFF || (_c32Cp >= 0xD800 && _c32Cp <= 0xDFFF)) ? char32_t( 0xFFFD ) : _c32Cp;
		};
		switch ( csSet ) {
			case CCharSets::MX_CS_UNICODE : {
				for ( char32_t c32Cp : _u32Text ) {
					c32Cp = Valid( c32Cp );
					uint16_t ui16Units[2];
					size_t sUnits = 1;
					if ( c32Cp >= 0x10000 ) {
						c32Cp -= 0x10000;
						ui16Units[0] = uint16_t( 0xD800 + (c32Cp >> 10) );
						ui16Units[1] = uint16_t( 0xDC00 + (c32Cp & 0x3FF) );
						sUnits = 2;
					}
					else { ui16Units[0] = uint16_t( c32Cp ); }
					for ( size_t I = 0; I < sUnits; ++I ) {
						if ( m_bBigEnd ) {
							_vBytes.push_back( uint8_t( ui16Units[I] >> 8 ) );
							_vBytes.push_back( uint8_t( ui16Units[I] ) );
						}
						else {
							_vBytes.push_back( uint8_t( ui16Units[I] ) );
							_vBytes.push_back( uint8_t( ui16Units[I] >> 8 ) );
						}
					}
				}
				break;
			}
			case CCharSets::MX_CS_UTF8 : {
				for ( char32_t c32Cp : _u32Text ) {
					c32Cp = Valid( c32Cp );
					if ( c32Cp < 0x80 ) {
						_vBytes.push_back( uint8_t( c32Cp ) );
					}
					else if ( c32Cp < 0x800 ) {
						_vBytes.push_back( uint8_t( 0xC0 | (c32Cp >> 6) ) );
						_vBytes.push_back( uint8_t( 0x80 | (c32Cp & 0x3F) ) );
					}
					else if ( c32Cp < 0x10000 ) {
						_vBytes.push_back( uint8_t( 0xE0 | (c32Cp >> 12) ) );
						_vBytes.push_back( uint8_t( 0x80 | ((c32Cp >> 6) & 0x3F) ) );
						_vBytes.push_back( uint8_t( 0x80 | (c32Cp & 0x3F) ) );
					}
					else {
						_vBytes.push_back( uint8_t( 0xF0 | (c32Cp >> 18) ) );
						_vBytes.push_back( uint8_t( 0x80 | ((c32Cp >> 12) & 0x3F) ) );
						_vBytes.push_back( uint8_t( 0x80 | ((c32Cp >> 6) & 0x3F) ) );
						_vBytes.push_back( uint8_t( 0x80 | (c32Cp & 0x3F) ) );
					}
				}
				break;
			}
			default : {
				// The ASCII view shows only 7-bit characters, so it gets the 7-bit code page (where best fit turns, for example, (R) into R).
				const UINT uiCodePage = (csSet == CCharSets::MX_CS_ASCII) ? CCodePages::MX_us_ascii : UINT( CharSet().ui32Code );
				for ( char32_t c32Cp : _u32Text ) {
					c32Cp = Valid( c32Cp );
					wchar_t wcUnits[2];
					int iUnits = 1;
					if ( c32Cp >= 0x10000 ) {
						const char32_t c32Tmp = c32Cp - 0x10000;
						wcUnits[0] = wchar_t( 0xD800 + (c32Tmp >> 10) );
						wcUnits[1] = wchar_t( 0xDC00 + (c32Tmp & 0x3FF) );
						iUnits = 2;
					}
					else { wcUnits[0] = wchar_t( c32Cp ); }
					// No WC_NO_BEST_FIT_CHARS: characters the code page lacks become their best fit, or the code page's default character.
					char cBytes[16];
					int iLen = ::WideCharToMultiByte( uiCodePage, 0, wcUnits, iUnits, cBytes, int( sizeof( cBytes ) ), nullptr, nullptr );
					if ( iLen > 0 ) {
						for ( int I = 0; I < iLen; ++I ) { _vBytes.push_back( uint8_t( cBytes[I] ) ); }
					}
					else {
						// The code page is not available.
						_vBytes.push_back( uint8_t( c32Cp < 0x80 ? c32Cp : U'?' ) );
					}
				}
			}
		}
	}

	/**
	 * Resolves escape sequences in typed text.  Each is a backslash followed by one of: a backslash, ', ", ?, a, b, f, n, r, t, v,
	 *	space, xHH, uHHHH (two in a row for a surrogate pair), UHHHHHHHH, up to three octal digits, or N{name}.  An invalid escape is
	 *	kept as typed.
	 *
	 * \param _u32Raw The typed text.
	 * \param _u32Out Receives the resolved text.  An unfinished escape at the end is kept as typed.
	 * \return Returns true if the text ends in an unfinished escape (more typing could change its meaning).
	 **/
	bool CHexEditorControl::ResolveEditEscapes( const std::u32string &_u32Raw, std::u32string &_u32Out ) {
		const size_t sLen = _u32Raw.size();
		auto Hex = []( char32_t _c32Cp ) -> int32_t { return DigitValue( _c32Cp, 16 ); };
		// Counts up to _sMax hex digits at _sPos.
		auto CountHex = [&]( size_t _sPos, size_t _sMax, uint32_t &_ui32Val ) -> size_t {
			size_t sCnt = 0;
			_ui32Val = 0;
			while ( sCnt < _sMax && _sPos + sCnt < sLen && Hex( _u32Raw[_sPos+sCnt] ) >= 0 ) {
				_ui32Val = (_ui32Val << 4) | uint32_t( Hex( _u32Raw[_sPos+sCnt] ) );
				++sCnt;
			}
			return sCnt;
		};
		auto Valid = []( uint32_t _ui32Cp ) -> char32_t {
			return (_ui32Cp > 0x10FFFF || (_ui32Cp >= 0xD800 && _ui32Cp <= 0xDFFF)) ? char32_t( 0xFFFD ) : char32_t( _ui32Cp );
		};
		// The rest of the text is an unfinished escape: keep it as typed.
		auto Open = [&]( size_t _sFrom ) -> bool {
			_u32Out.append( _u32Raw, _sFrom, std::u32string::npos );
			return true;
		};

		for ( size_t I = 0; I < sLen; ) {
			const char32_t c32This = _u32Raw[I];
			if ( c32This != U'\\' ) {
				_u32Out.push_back( c32This );
				++I;
				continue;
			}
			if ( I + 1 >= sLen ) { return Open( I ); }
			const char32_t c32Next = _u32Raw[I+1];
			switch ( c32Next ) {
				case U'\\' : { _u32Out.push_back( U'\\' ); I += 2; continue; }
				case U'\'' : { _u32Out.push_back( U'\'' ); I += 2; continue; }
				case U'"' : { _u32Out.push_back( U'"' ); I += 2; continue; }
				case U'?' : { _u32Out.push_back( U'?' ); I += 2; continue; }
				case U' ' : { _u32Out.push_back( U' ' ); I += 2; continue; }
				case U'a' : { _u32Out.push_back( U'\a' ); I += 2; continue; }
				case U'b' : { _u32Out.push_back( U'\b' ); I += 2; continue; }
				case U'f' : { _u32Out.push_back( U'\f' ); I += 2; continue; }
				case U'n' : { _u32Out.push_back( U'\n' ); I += 2; continue; }
				case U'r' : { _u32Out.push_back( U'\r' ); I += 2; continue; }
				case U't' : { _u32Out.push_back( U'\t' ); I += 2; continue; }
				case U'v' : { _u32Out.push_back( U'\v' ); I += 2; continue; }
				case U'x' : {
					uint32_t ui32Val;
					const size_t sCnt = CountHex( I + 2, 2, ui32Val );
					// \x takes one or two hex digits; with one at the end a second may still come.
					if ( I + 2 + sCnt >= sLen && sCnt < 2 ) { return Open( I ); }
					if ( !sCnt ) { break; }		// Invalid.
					_u32Out.push_back( char32_t( ui32Val ) );
					I += 2 + sCnt;
					continue;
				}
				case U'u' : {
					uint32_t ui32Val;
					const size_t sCnt = CountHex( I + 2, 4, ui32Val );
					if ( sCnt < 4 ) {
						if ( I + 2 + sCnt >= sLen ) { return Open( I ); }
						break;		// Invalid.
					}
					size_t sNext = I + 6;
					if ( ui32Val >= 0xD800 && ui32Val <= 0xDBFF ) {
						// A high surrogate wants a low surrogate in another \u right after it.
						const char32_t c32Pat[2] = { U'\\', U'u' };
						size_t sMatch = 0;
						while ( sMatch < 2 && sNext + sMatch < sLen && _u32Raw[sNext+sMatch] == c32Pat[sMatch] ) { ++sMatch; }
						if ( sNext + sMatch >= sLen && sMatch < 2 ) { return Open( I ); }
						if ( sMatch == 2 ) {
							uint32_t ui32Low;
							const size_t sLowCnt = CountHex( sNext + 2, 4, ui32Low );
							if ( sLowCnt < 4 && sNext + 2 + sLowCnt >= sLen ) { return Open( I ); }
							if ( sLowCnt == 4 && ui32Low >= 0xDC00 && ui32Low <= 0xDFFF ) {
								_u32Out.push_back( char32_t( 0x10000 + ((ui32Val - 0xD800) << 10) + (ui32Low - 0xDC00) ) );
								I = sNext + 6;
								continue;
							}
						}
					}
					_u32Out.push_back( Valid( ui32Val ) );
					I = sNext;
					continue;
				}
				case U'U' : {
					uint32_t ui32Val;
					const size_t sCnt = CountHex( I + 2, 8, ui32Val );
					if ( sCnt < 8 ) {
						if ( I + 2 + sCnt >= sLen ) { return Open( I ); }
						break;		// Invalid.
					}
					_u32Out.push_back( Valid( ui32Val ) );
					I += 10;
					continue;
				}
				case U'N' : {
					if ( I + 2 >= sLen ) { return Open( I ); }
					if ( _u32Raw[I+2] != U'{' ) { break; }		// Invalid.
					const size_t sClose = _u32Raw.find( U'}', I + 3 );
					if ( sClose == std::u32string::npos ) { return Open( I ); }
					// Let the expression evaluator look the name up.
					std::string sEsc = "\\N{";
					bool bAscii = true;
					for ( size_t J = I + 3; J < sClose; ++J ) {
						if ( _u32Raw[J] >= 0x80 ) { bAscii = false; break; }
						sEsc.push_back( char( _u32Raw[J] ) );
					}
					sEsc.push_back( '}' );
					if ( bAscii ) {
						size_t sCharLen = 0;
						bool bFound = false;
						const uint64_t ui64Val = ee::CExpEval::ResolveEscape( sEsc.c_str(), sEsc.size(), sCharLen, false, &bFound );
						if ( bFound && sCharLen == sEsc.size() ) {
							_u32Out.push_back( Valid( uint32_t( ui64Val ) ) );
							I = sClose + 1;
							continue;
						}
					}
					break;		// Invalid.
				}
				default : {
					if ( c32Next >= U'0' && c32Next <= U'7' ) {
						// Up to three octal digits; with fewer at the end, more may still come.
						size_t sCnt = 0;
						uint32_t ui32Val = 0;
						while ( sCnt < 3 && I + 1 + sCnt < sLen && _u32Raw[I+1+sCnt] >= U'0' && _u32Raw[I+1+sCnt] <= U'7' ) {
							ui32Val = (ui32Val << 3) | uint32_t( _u32Raw[I+1+sCnt] - U'0' );
							++sCnt;
						}
						if ( sCnt < 3 && I + 1 + sCnt >= sLen ) { return Open( I ); }
						_u32Out.push_back( char32_t( ui32Val ) );
						I += 1 + sCnt;
						continue;
					}
				}
			}
			// An invalid escape: the backslash is kept as typed.
			_u32Out.push_back( U'\\' );
			++I;
		}
		return false;
	}

	/**
	 * Converts typed text to the bytes of a multi-byte value.
	 *
	 * \param _dfFmt The format of the value.
	 * \param _wsText The typed text.
	 * \param _vBytes Receives the bytes (appended, little-endian).
	 * \return Returns false if the text is not a valid value of the format.
	 **/
	bool CHexEditorControl::ParseValue( MX_DATA_FMT _dfFmt, const std::wstring &_wsText, std::vector<uint8_t> &_vBytes ) {
		auto Append = [&]( const void * _pvData, size_t _sSize ) {
			const uint8_t * pui8Data = static_cast<const uint8_t *>(_pvData);
			_vBytes.insert( _vBytes.end(), pui8Data, pui8Data + _sSize );
		};
		// Unsigned decimal or hexadecimal digits, with overflow checking.
		auto ParseUnsigned = [&]( size_t _sFrom, uint32_t _ui32Radix, uint64_t &_ui64Val ) -> bool {
			if ( _sFrom >= _wsText.size() ) { return false; }
			_ui64Val = 0;
			for ( size_t I = _sFrom; I < _wsText.size(); ++I ) {
				const int32_t i32Digit = DigitValue( char32_t( _wsText[I] ), _ui32Radix );
				if ( i32Digit < 0 ) { return false; }
				if ( _ui64Val > (UINT64_MAX - uint64_t( i32Digit )) / _ui32Radix ) { return false; }
				_ui64Val = _ui64Val * _ui32Radix + uint64_t( i32Digit );
			}
			return true;
		};
		auto ParseSigned = [&]( int64_t _i64Min, int64_t _i64Max, int64_t &_i64Val ) -> bool {
			const bool bNeg = !_wsText.empty() && _wsText[0] == L'-';
			const size_t sFrom = (!_wsText.empty() && (_wsText[0] == L'-' || _wsText[0] == L'+')) ? 1 : 0;
			uint64_t ui64Mag;
			if ( !ParseUnsigned( sFrom, 10, ui64Mag ) ) { return false; }
			if ( bNeg ) {
				if ( ui64Mag > uint64_t( -(_i64Min + 1) ) + 1 ) { return false; }
				_i64Val = (ui64Mag == 0) ? 0 : -int64_t( ui64Mag - 1 ) - 1;
			}
			else {
				if ( ui64Mag > uint64_t( _i64Max ) ) { return false; }
				_i64Val = int64_t( ui64Mag );
			}
			return true;
		};
		auto ParseDouble = [&]( double &_dVal ) -> bool {
			if ( _wsText.empty() ) { return false; }
			wchar_t * pwcEnd = nullptr;
			_dVal = std::wcstod( _wsText.c_str(), &pwcEnd );
			return pwcEnd == _wsText.c_str() + _wsText.size();
		};
		auto Pcm = [&]( double _dScale, double _dBias, int64_t _i64Min, int64_t _i64Max, size_t _sBytes ) -> bool {
			double dVal;
			if ( !ParseDouble( dVal ) || !(dVal >= -1.0 && dVal <= 1.0) ) { return false; }
			const int64_t i64Val = std::clamp<int64_t>( int64_t( std::llround( dVal * _dScale + _dBias ) ), _i64Min, _i64Max );
			Append( &i64Val, _sBytes );
			return true;
		};

		switch ( _dfFmt ) {
#define MX_UNSIGNED( CASE, TYPE )																	\
			case CASE : {																			\
				uint64_t ui64Val;																	\
				if ( !ParseUnsigned( 0, 10, ui64Val ) || ui64Val > std::numeric_limits<TYPE>::max() ) { return false; }	\
				const TYPE tVal = TYPE( ui64Val );													\
				Append( &tVal, sizeof( tVal ) );													\
				return true;																		\
			}
#define MX_SIGNED( CASE, TYPE )																		\
			case CASE : {																			\
				int64_t i64Val;																		\
				if ( !ParseSigned( std::numeric_limits<TYPE>::min(), std::numeric_limits<TYPE>::max(), i64Val ) ) { return false; }	\
				const TYPE tVal = TYPE( i64Val );													\
				Append( &tVal, sizeof( tVal ) );													\
				return true;																		\
			}
#define MX_POINTER( CASE, TYPE )																	\
			case CASE : {																			\
				uint64_t ui64Val;																	\
				if ( !ParseUnsigned( 0, 16, ui64Val ) || ui64Val > std::numeric_limits<TYPE>::max() ) { return false; }	\
				const TYPE tVal = TYPE( ui64Val );													\
				Append( &tVal, sizeof( tVal ) );													\
				return true;																		\
			}
			MX_UNSIGNED( MX_DF_UINT8, uint8_t )
			MX_UNSIGNED( MX_DF_UINT16, uint16_t )
			MX_UNSIGNED( MX_DF_UINT32, uint32_t )
			MX_UNSIGNED( MX_DF_UINT64, uint64_t )
			MX_SIGNED( MX_DF_INT8, int8_t )
			MX_SIGNED( MX_DF_INT16, int16_t )
			MX_SIGNED( MX_DF_INT32, int32_t )
			MX_SIGNED( MX_DF_INT64, int64_t )
			MX_POINTER( MX_DF_POINTER16, uint16_t )
			MX_POINTER( MX_DF_POINTER32, uint32_t )
			MX_POINTER( MX_DF_POINTER64, uint64_t )
#undef MX_POINTER
#undef MX_SIGNED
#undef MX_UNSIGNED
			case MX_DF_FLOAT32 : {
				double dVal;
				if ( !ParseDouble( dVal ) ) { return false; }
				const float fVal = float( dVal );
				Append( &fVal, sizeof( fVal ) );
				return true;
			}
			case MX_DF_FLOAT64 : {
				double dVal;
				if ( !ParseDouble( dVal ) ) { return false; }
				Append( &dVal, sizeof( dVal ) );
				return true;
			}
			case MX_DF_FLOAT16 : {
				double dVal;
				if ( !ParseDouble( dVal ) ) { return false; }
				const uint16_t ui16Val = ee::CFloat16( dVal ).RawValue();
				Append( &ui16Val, sizeof( ui16Val ) );
				return true;
			}
			// The same layouts DrawArea() displays.
			case MX_DF_FLOAT10 : {}				MX_FALLTHROUGH
			case MX_DF_FLOAT11 : {}				MX_FALLTHROUGH
			case MX_DF_BFLOAT16 : {
				double dVal;
				if ( !ParseDouble( dVal ) ) { return false; }
				ee::CFloatX fFloat;
				switch ( _dfFmt ) {
					case MX_DF_FLOAT10 : { fFloat.CreateFromDouble( dVal, 5, 6, true, false ); break; }
					case MX_DF_FLOAT11 : { fFloat.CreateFromDouble( dVal, 5, 7, true, false ); break; }
					default : { fFloat.CreateFromDouble( dVal, 8, 8, true, true ); break; }
				}
				const uint16_t ui16Val = uint16_t( fFloat.AsUint64() );
				Append( &ui16Val, sizeof( ui16Val ) );
				return true;
			}
			// The inverses of DrawArea()'s PCM conversions.
			case MX_DF_PCM8 : { return Pcm( 127.0, 128.0, 0, 255, 1 ); }
			case MX_DF_PCM16 : { return Pcm( 32767.0, 0.0, INT16_MIN, INT16_MAX, 2 ); }
			case MX_DF_PCM24 : { return Pcm( 8388607.0, 0.0, -8388608, 8388607, 3 ); }
			case MX_DF_PCM32 : { return Pcm( 2147483647.0, 0.0, INT32_MIN, INT32_MAX, 4 ); }
			default : { return false; }
		}
	}

	/**
	 * Determines whether a character can be part of a typed value of a multi-byte format.
	 *
	 * \param _dfFmt The format of the value.
	 * \param _c32Cp The character.
	 * \param _bSignAllowed True if a sign can be typed here (at the start of a value that has none).
	 * \return Returns true if the character can be typed.
	 **/
	bool CHexEditorControl::ValueCharValid( MX_DATA_FMT _dfFmt, char32_t _c32Cp, bool _bSignAllowed ) {
		const bool bDigit = _c32Cp >= U'0' && _c32Cp <= U'9';
		switch ( _dfFmt ) {
			case MX_DF_UINT8 : {}				MX_FALLTHROUGH
			case MX_DF_UINT16 : {}				MX_FALLTHROUGH
			case MX_DF_UINT32 : {}				MX_FALLTHROUGH
			case MX_DF_UINT64 : { return bDigit; }
			case MX_DF_INT8 : {}				MX_FALLTHROUGH
			case MX_DF_INT16 : {}				MX_FALLTHROUGH
			case MX_DF_INT32 : {}				MX_FALLTHROUGH
			case MX_DF_INT64 : { return bDigit || ((_c32Cp == U'-' || _c32Cp == U'+') && _bSignAllowed); }
			case MX_DF_POINTER16 : {}			MX_FALLTHROUGH
			case MX_DF_POINTER32 : {}			MX_FALLTHROUGH
			case MX_DF_POINTER64 : { return DigitValue( _c32Cp, 16 ) >= 0; }
			default : {
				// Floating-point and PCM values.
				return bDigit || _c32Cp == U'.' || _c32Cp == U'-' || _c32Cp == U'+' || _c32Cp == U'e' || _c32Cp == U'E';
			}
		}
	}

	// == Changed values.
	/**
	 * Clears the highlighting of every changed value.
	 **/
	void CHexEditorControl::ClearAllChanges() {
		m_mChanges.clear();
		::InvalidateRect( Wnd(), nullptr, FALSE );
	}

	/**
	 * Clears the highlighting of the changed values in the selection, or of the byte at the caret if nothing is selected.
	 **/
	void CHexEditorControl::ClearSelectedChanges() {
		try {
			if ( m_sSel.HasSelection() ) {
				std::vector<MX_SEL_RANGE> vSelections;
				constexpr uint64_t ui64GroupSize = 1024ULL * 1024ULL;
				uint64_t ui64Idx = 0;
				do {
					if ( !m_sSel.GatherSelected_LowToHigh( CurStyle()->uiBytesPerRow, vSelections, ui64GroupSize, ui64Idx++ ) ) { break; }
					for ( const auto &srThis : vSelections ) {
						ClearChanges( srThis.ui64Start, srThis.ui64Start + srThis.ui64Size );
					}
				} while ( vSelections.size() );
			}
			else {
				// The whole value under the caret.
				const MX_STYLE &stAll = (*CurStyle());
				const bool bRight = m_sgSelGesture.bRightArea && stAll.bShowRightArea;
				const uint32_t ui32Stride = StrideForFormat( bRight ? stAll.dfRightNumbersFmt : stAll.dfLeftNumbersFmt );
				const uint64_t ui64Start = m_sgSelGesture.ui64CaretAddr - m_sgSelGesture.ui64CaretAddr % ui32Stride;
				ClearChanges( ui64Start, ui64Start + ui32Stride );
			}
		}
		catch ( ... ) {}
		::InvalidateRect( Wnd(), nullptr, FALSE );
	}

	/**
	 * Clears the highlighting of the changed value at the address that was right-clicked to open the context menu (the whole
	 *	value in the area that was clicked), or at the caret if the menu was not opened by a click on an address.
	 **/
	void CHexEditorControl::ClearClickedChange() {
		const MX_STYLE &stAll = (*CurStyle());
		const uint64_t ui64Addr = m_bContextAddr ? m_ui64ContextAddr : m_sgSelGesture.ui64CaretAddr;
		const bool bRight = (m_bContextAddr ? m_bContextRight : m_sgSelGesture.bRightArea) && stAll.bShowRightArea;
		const uint32_t ui32Stride = std::max<uint32_t>( StrideForFormat( bRight ? stAll.dfRightNumbersFmt : stAll.dfLeftNumbersFmt ), 1 );
		const uint64_t ui64Start = ui64Addr - ui64Addr % ui32Stride;
		ClearChanges( ui64Start, ui64Start + ui32Stride );
		::InvalidateRect( Wnd(), nullptr, FALSE );
	}

	/**
	 * Selects a range of bytes, with the caret after the last one.
	 *
	 * \param _ui64Start The first byte to select.
	 * \param _ui64Size The number of bytes to select.  If 0, the selection is removed and the caret is put at _ui64Start.
	 * \return Returns false if the range starts beyond the end of the data.  A range that runs past the end is cut short.
	 **/
	bool CHexEditorControl::SelectRange( uint64_t _ui64Start, uint64_t _ui64Size ) {
		const uint64_t ui64Size = Size();
		if ( _ui64Start > ui64Size ) { return false; }
		FlushRealTimeUndo();
		_ui64Size = std::min( _ui64Size, ui64Size - _ui64Start );
		if ( !_ui64Size ) {
			ClearSelection();
			SetCaretAddr( _ui64Start );
			m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64CaretAddr;
			return true;
		}
		m_sSel.bHas = true;
		m_sSel.smMode = m_sgSelGesture.smCurrent = MX_SM_NORMAL;
		m_sSel.sn.ui64Start = _ui64Start;
		m_sSel.sn.ui64End = _ui64Start + _ui64Size - 1;
		m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sSel.sn.ui64Start;
		SetCaretAddr( _ui64Start + _ui64Size, 0, true );
		return true;
	}

	/**
	 * Sets the custom starting address for every view of the data.
	 *
	 * \param _ui64Addr The address to show for the first byte.
	 **/
	void CHexEditorControl::SetStartingAddress( uint64_t _ui64Addr ) {
		for ( auto &sdView : m_sdScrollView ) {
			sdView.ui64StartAddress = _ui64Addr;
		}
		// The address column may need a different width.
		RecalcAndInvalidate();
	}

	/**
	 * Handles WM_RBUTTONDOWN.  Right-clicking a byte outside the selection moves the caret to it, so that the context menu applies to
	 *	it.
	 *
	 * \param _iHitTest The virtual keys that are down.
	 * \param _pCursorPos The cursor position in client coordinates.
	 * \return Returns a LSW_HANDLED code.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::RButtonDown( INT /*_iHitTest*/, const POINTS &_pCursorPos ) {
		this->CWidget::SetFocus();
		// The edit box stays as it is (its own keys and the context menu's commands decide what happens to it).
		if ( IsEditing() ) { return lsw::CWidget::LSW_H_CONTINUE; }
		POINT ptPoint { _pCursorPos.x, _pCursorPos.y };
		uint64_t ui64Addr = 0;
		int32_t i32Sub = 0;
		bool bRight = false;
		m_bContextAddr = false;
		if ( ptPoint.y >= GetRulerHeightPx() && PointToAddress( ptPoint, ui64Addr, i32Sub, bRight ) ) {
			// Remembered for the context menu's Clear Change Highlight at Clicked Address, even inside the selection.
			m_ui64ContextAddr = ui64Addr;
			m_bContextRight = bRight;
			m_bContextAddr = true;
			if ( !(m_sSel.HasSelection() && m_sSel.IsSelected( ui64Addr, CurStyle()->uiBytesPerRow )) ) {
				FlushRealTimeUndo();
				ClearSelection();
				m_sgSelGesture.bRightArea = bRight;
				SetCaretAddr( ui64Addr, i32Sub, false, false );
				m_sgSelGesture.ui64AnchorAddr = m_sgSelGesture.ui64MouseAnchorAddr = m_sgSelGesture.ui64CaretAddr;
				StartCaretBlink();
				::InvalidateRect( Wnd(), nullptr, FALSE );
				if ( m_pwHexParent ) { m_pwHexParent->UpdateStatusBar_PosValue_StartSize(); }
			}
		}
		// Windows sends WM_CONTEXTMENU when the button is released.
		return lsw::CWidget::LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_CONTEXTMENU.  The hex editor window shows the context menu directly (the message is not left to travel up
	 *	through the tab control and the splitter, where it was being lost).
	 *
	 * \param _pwControl The control that was right-clicked.
	 * \param _iX The horizontal position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
	 * \param _iY The vertical position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
	 * \return Returns LSW_H_HANDLED if the menu was shown.
	 */
	lsw::CWidget::LSW_HANDLED CHexEditorControl::ContextMenu( CWidget * /*_pwControl*/, INT _iX, INT _iY ) {
		if ( m_pwHexParent && m_pwHexParent->ShowHexContextMenu( this, _iX, _iY ) ) { return lsw::CWidget::LSW_H_HANDLED; }
		return lsw::CWidget::LSW_H_CONTINUE;
	}

	/**
	 * (Re)starts the timer that looks for changed values in view, at the current rate.
	 **/
	void CHexEditorControl::StartWatch() {
		if ( Wnd() ) {
			m_tWatch.Start( Wnd(), MX_TI_HEX_WATCH_CHANGES, m_ui32WatchPeriod, nullptr );
		}
	}

	/**
	 * Compares the bytes in view with those seen the last time and highlights the ones that changed.  Looks more often right after a
	 *	change (MX_WATCH_FAST_MS), then less and less often while nothing more changes, back down to MX_WATCH_SLOW_MS.
	 **/
	void CHexEditorControl::WatchForChanges() {
		// A hidden view keeps what it saw last, so changes made meanwhile show when it is shown again.
		if ( !m_pheiTarget || !Wnd() || !::IsWindowVisible( Wnd() ) ) { return; }
		// Data sources that know their own changes (files) are shown from those instead.
		if ( m_pheiTarget->TracksChangedBytes() ) { return; }
		try {
			const uint32_t ui32Bpr = CurStyle()->uiBytesPerRow ? CurStyle()->uiBytesPerRow : 16;
			const uint64_t ui64Addr = m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine * ui32Bpr;
			const size_t sCap = size_t( std::max( m_sdScrollView[m_eaEditAs].i32PageLines + 1, 1 ) ) * ui32Bpr;
			const uint64_t ui64Size = Size();
			CHexEditorInterface::CBuffer bNow;
			if ( ui64Addr >= ui64Size || !m_pheiTarget->Read( ui64Addr, bNow, size_t( std::min<uint64_t>( sCap, ui64Size - ui64Addr ) ) ) ) {
				m_bWatchValid = false;
				return;
			}

			bool bChanged = false;
			// Insertions and deletions move the data, so after one the bytes can only be compared from the next look onward.
			if ( m_bWatchValid && m_ui64WatchSize == ui64Size ) {
				const uint64_t ui64From = std::max( ui64Addr, m_ui64WatchAddr );
				const uint64_t ui64To = std::min( ui64Addr + bNow.size(), m_ui64WatchAddr + m_bWatchPrev.size() );
				const uint32_t ui32Now = ChangeTimeNow();
				uint64_t ui64RunStart = 0;
				bool bInRun = false;
				for ( uint64_t A = ui64From; A < ui64To; ++A ) {
					const bool bDiff = bNow[size_t( A - ui64Addr )] != m_bWatchPrev[size_t( A - m_ui64WatchAddr )];
					if ( bDiff && !bInRun ) {
						ui64RunStart = A;
						bInRun = true;
					}
					else if ( !bDiff && bInRun ) {
						SetChanged( ui64RunStart, A, ui32Now );
						bInRun = false;
						bChanged = true;
					}
				}
				if ( bInRun ) {
					SetChanged( ui64RunStart, ui64To, ui32Now );
					bChanged = true;
				}
			}
			m_bWatchPrev.swap( bNow );
			m_ui64WatchAddr = ui64Addr;
			m_ui64WatchSize = ui64Size;
			m_bWatchValid = true;

			bool bRedraw = bChanged;
			if ( !bRedraw && MX_CHANGE_FADE_MS && !m_mChanges.empty() ) {
				// Values still fading in view need to be drawn again.
				const uint64_t ui64Now = ::GetTickCount64() - m_ui64ChangeEpoch;
				auto aIt = m_mChanges.upper_bound( ui64Addr );
				if ( aIt != m_mChanges.begin() ) { --aIt; }
				for ( ; aIt != m_mChanges.end() && aIt->first < ui64Addr + m_bWatchPrev.size() && !bRedraw; ++aIt ) {
					bRedraw = aIt->second.ui64End > ui64Addr && ui64Now - aIt->second.ui32Time < MX_CHANGE_FADE_MS;
				}
			}
			if ( bRedraw ) { ::InvalidateRect( Wnd(), nullptr, FALSE ); }

			// Faster right after a change, then gradually slower while nothing more changes.
			const uint32_t ui32Period = bChanged ? MX_WATCH_FAST_MS : std::min( m_ui32WatchPeriod + MX_WATCH_SLOWDOWN_MS, MX_WATCH_SLOW_MS );
			if ( ui32Period != m_ui32WatchPeriod ) {
				m_ui32WatchPeriod = ui32Period;
				StartWatch();
			}
		}
		catch ( ... ) { m_bWatchValid = false; }
	}

	/**
	 * Gets the time to record for a change made now.
	 *
	 * \return Returns the number of milliseconds since m_ui64ChangeEpoch.
	 **/
	uint32_t CHexEditorControl::ChangeTimeNow() {
		const uint64_t ui64Now = ::GetTickCount64();
		if ( !m_ui64ChangeEpoch ) { m_ui64ChangeEpoch = ui64Now; }
		return uint32_t( std::min<uint64_t>( ui64Now - m_ui64ChangeEpoch, UINT32_MAX ) );
	}

	/**
	 * Highlights bytes as changed now.
	 *
	 * \param _ui64Addr The address of the first byte.
	 * \param _ui64Size The number of bytes.
	 **/
	void CHexEditorControl::MarkChanged( uint64_t _ui64Addr, uint64_t _ui64Size ) {
		SetChanged( _ui64Addr, _ui64Addr + _ui64Size, ChangeTimeNow() );
	}

	/**
	 * Records a run of bytes as changed at a given time, replacing what was recorded for them.
	 *
	 * \param _ui64Start The address of the first byte.
	 * \param _ui64End The address after the last byte.
	 * \param _ui32Time When they changed (see ChangeTimeNow()).
	 **/
	void CHexEditorControl::SetChanged( uint64_t _ui64Start, uint64_t _ui64End, uint32_t _ui32Time ) {
		if ( _ui64Start >= _ui64End ) { return; }
		try {
			ClearChanges( _ui64Start, _ui64End );
			uint64_t ui64End = _ui64End;
			// Join the neighbors that changed at the same time.
			auto aNext = m_mChanges.lower_bound( _ui64Start );
			if ( aNext != m_mChanges.end() && aNext->first == _ui64End && aNext->second.ui32Time == _ui32Time ) {
				ui64End = aNext->second.ui64End;
				aNext = m_mChanges.erase( aNext );
			}
			if ( aNext != m_mChanges.begin() ) {
				auto aPrev = std::prev( aNext );
				if ( aPrev->second.ui64End == _ui64Start && aPrev->second.ui32Time == _ui32Time ) {
					aPrev->second.ui64End = ui64End;
					return;
				}
			}
			MX_CHANGE_RUN crRun;
			crRun.ui64End = ui64End;
			crRun.ui32Time = _ui32Time;
			m_mChanges.emplace_hint( aNext, _ui64Start, crRun );
		}
		catch ( ... ) {}
	}

	/**
	 * Removes the change highlighting of a range of bytes.
	 *
	 * \param _ui64Start The address of the first byte.
	 * \param _ui64End The address after the last byte.
	 **/
	void CHexEditorControl::ClearChanges( uint64_t _ui64Start, uint64_t _ui64End ) {
		if ( _ui64Start >= _ui64End || m_mChanges.empty() ) { return; }
		try {
			auto aIt = m_mChanges.lower_bound( _ui64Start );
			if ( aIt != m_mChanges.begin() ) {
				// A run that starts before the range is cut short (and split if it runs past the range).
				auto aPrev = std::prev( aIt );
				if ( aPrev->second.ui64End > _ui64Start ) {
					const MX_CHANGE_RUN crOld = aPrev->second;
					aPrev->second.ui64End = _ui64Start;
					if ( crOld.ui64End > _ui64End ) {
						m_mChanges.emplace_hint( aIt, _ui64End, crOld );
						return;
					}
				}
			}
			while ( aIt != m_mChanges.end() && aIt->first < _ui64End ) {
				if ( aIt->second.ui64End > _ui64End ) {
					// The last run keeps its part after the range.
					const MX_CHANGE_RUN crTail = aIt->second;
					aIt = m_mChanges.erase( aIt );
					m_mChanges.emplace_hint( aIt, _ui64End, crTail );
					break;
				}
				aIt = m_mChanges.erase( aIt );
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Moves the change highlighting along after bytes are inserted or deleted, so that it stays with the bytes it belongs to.
	 *
	 * \param _ui64Addr Where the bytes were inserted or deleted.
	 * \param _ui64Size The number of bytes.
	 * \param _bInserted True if the bytes were inserted (and are highlighted as changed), false if they were deleted.
	 **/
	void CHexEditorControl::ShiftChanges( uint64_t _ui64Addr, uint64_t _ui64Size, bool _bInserted ) {
		// The data moved, so the next look for changes starts over.
		m_bWatchValid = false;
		if ( !_ui64Size ) { return; }
		try {
			if ( _bInserted ) {
				// A run that straddles the insertion point is split around it.
				auto aIt = m_mChanges.lower_bound( _ui64Addr );
				if ( aIt != m_mChanges.begin() ) {
					auto aPrev = std::prev( aIt );
					if ( aPrev->second.ui64End > _ui64Addr ) {
						const MX_CHANGE_RUN crTail = aPrev->second;
						aPrev->second.ui64End = _ui64Addr;
						m_mChanges.emplace_hint( aIt, _ui64Addr, crTail );
					}
				}
			}
			else {
				ClearChanges( _ui64Addr, _ui64Addr + _ui64Size );
			}

			// Everything after the point moves (in order, so each can be put back at the end).
			std::vector<std::pair<uint64_t, MX_CHANGE_RUN>> vMoved;
			for ( auto aIt = m_mChanges.lower_bound( _bInserted ? _ui64Addr : (_ui64Addr + _ui64Size) ); aIt != m_mChanges.end(); ) {
				MX_CHANGE_RUN crRun = aIt->second;
				uint64_t ui64Start = aIt->first;
				if ( _bInserted ) {
					ui64Start += _ui64Size;
					crRun.ui64End += _ui64Size;
				}
				else {
					ui64Start -= _ui64Size;
					crRun.ui64End -= _ui64Size;
				}
				vMoved.emplace_back( ui64Start, crRun );
				aIt = m_mChanges.erase( aIt );
			}
			for ( const auto &pThis : vMoved ) { m_mChanges.emplace_hint( m_mChanges.end(), pThis.first, pThis.second ); }

			if ( _bInserted ) { MarkChanged( _ui64Addr, _ui64Size ); }
		}
		catch ( ... ) {}
	}

	/**
	 * Determines whether any of a range of bytes is highlighted as changed.
	 *
	 * \param _ui64Addr The address of the first byte.
	 * \param _ui64Size The number of bytes (a value's size).
	 * \param _ui32Time Receives when the most recent change among them happened (see ChangeTimeNow()).
	 * \return Returns true if at least one of the bytes is highlighted as changed.
	 **/
	bool CHexEditorControl::ChangedIn( uint64_t _ui64Addr, uint64_t _ui64Size, uint32_t &_ui32Time ) const {
		if ( m_mChanges.empty() ) { return false; }
		const uint64_t ui64End = _ui64Addr + std::max<uint64_t>( _ui64Size, 1 );
		bool bFound = false;
		_ui32Time = 0;
		auto aIt = m_mChanges.upper_bound( _ui64Addr );
		if ( aIt != m_mChanges.begin() ) {
			auto aPrev = std::prev( aIt );
			if ( aPrev->second.ui64End > _ui64Addr ) {
				bFound = true;
				_ui32Time = aPrev->second.ui32Time;
			}
		}
		for ( ; aIt != m_mChanges.end() && aIt->first < ui64End; ++aIt ) {
			if ( !bFound || aIt->second.ui32Time > _ui32Time ) { _ui32Time = aIt->second.ui32Time; }
			bFound = true;
		}
		return bFound;
	}

	/**
	 * Gets the text color of a changed value: the Modified color, fading to the normal color over MX_CHANGE_FADE_MS if that is
	 *	not 0.
	 *
	 * \param _crNormal The value's normal text color.
	 * \param _ui32Time When the value changed (see ChangeTimeNow()).
	 * \return Returns the text color.
	 **/
	COLORREF CHexEditorControl::ChangedColor( COLORREF _crNormal, uint32_t _ui32Time ) const {
		const COLORREF crChanged = ForeColors()->crModified;
		if ( !MX_CHANGE_FADE_MS ) { return crChanged; }
		const uint64_t ui64Age = (::GetTickCount64() - m_ui64ChangeEpoch) - _ui32Time;
		if ( ui64Age >= MX_CHANGE_FADE_MS ) { return _crNormal; }
		// 0 = still wholly the Modified color, 255 = back to normal.
		const uint32_t ui32Amount = uint32_t( ui64Age * 255 / std::max<uint64_t>( MX_CHANGE_FADE_MS, 1 ) );
		auto Blend = [ui32Amount]( uint32_t _ui32From, uint32_t _ui32To ) -> uint32_t {
			return (_ui32From * (255 - ui32Amount) + _ui32To * ui32Amount) / 255;
		};
		return RGB( Blend( GetRValue( crChanged ), GetRValue( _crNormal ) ),
			Blend( GetGValue( crChanged ), GetGValue( _crNormal ) ),
			Blend( GetBValue( crChanged ), GetBValue( _crNormal ) ) ) | (crChanged & 0xFF000000);
	}

}	// namespace mx

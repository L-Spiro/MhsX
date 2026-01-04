#include "LSWStatusBar.h"

#include <commctrl.h>
#include <strsafe.h>
#include <uxtheme.h>
#include <vssym32.h>
#pragma comment( lib, "UxTheme.lib" )



namespace lsw {

	// == Members.
	WCHAR CStatusBar::m_szProp[2] = { 0 };							/** Window property. */

	CStatusBar::CStatusBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		if ( !m_szProp[0] ) {
			m_szProp[0] = L'F' + ((reinterpret_cast<UINT_PTR>(_pwParent) >> 2) & 0x0F) + 16;
		}
	}

	// == Functions.
	/**
	 * \brief Sets the parts for the status bar.
	 *
	 * \param _vParts The parts to apply.
	 * \return TRUE if the parts were set successfully; FALSE otherwise.
	 */
	BOOL CStatusBar::SetParts( const std::vector<LSW_STATUS_PART> &_vParts ) {
		if ( !_vParts.size() ) { return TRUE; }
		return SetParts( &_vParts[0], _vParts.size() );
	}

	/**
	 * \brief Sets the parts for the status bar.
	 *
	 * \param _pspParts Pointer to an array of parts.
	 * \param _sTotal Total number of parts in the array.
	 * \return TRUE if the parts were set successfully; FALSE otherwise.
	 */
	BOOL CStatusBar::SetParts( const LSW_STATUS_PART * _pspParts, size_t _sTotal ) {
		m_vParts.clear();
		m_vParts.reserve( _sTotal );
		m_vItems.clear();
		m_vItems.resize( _sTotal );
		std::vector<INT> vInts;
		vInts.reserve( _sTotal );
		for ( size_t I = 0; I < _sTotal; ++I ) {
			vInts.push_back( _pspParts[I].iRightCoord );
			m_vParts.push_back( _pspParts[I] );
			m_vItems[I].uiType = 0;
		}
		if ( !vInts.size() ) { return TRUE; }
		if ( !static_cast<BOOL>(::SendMessageW( Wnd(), SB_SETPARTS, static_cast<WPARAM>(vInts.size()), reinterpret_cast<LPARAM>(&vInts[0]) )) ) { return FALSE; }
		m_bAllOwnerDrawApplied = FALSE;
		m_vLastAutoFitRights.clear();

		// Force SBT_OWNERDRAW for every part.
		for ( size_t I = 0; I < _sTotal; ++I ) {
			::SendMessageW( Wnd(), SB_SETTEXTW, static_cast<WPARAM>(static_cast<INT>(I) | (m_vItems[I].uiType | SBT_OWNERDRAW)), static_cast<LPARAM>(I) );
		}
		return TRUE;
	}

	/**
	 * \brief Retrieves the text for a part as ANSI.
	 *
	 * \param _iIdx The zero-based part index.
	 * \param _lpString Destination buffer for the text.
	 * \param _nMaxCount Maximum number of characters to copy (including terminator, if applicable).
	 * \return A 32-bit value containing two 16-bit values:
	 *	The low word specifies the length, in characters, of the text.
	 *	The high word specifies the type of operation used to draw the text.
	 */
	DWORD CStatusBar::GetTextA( INT _iIdx, LPSTR _lpString, INT _nMaxCount ) const {
		if ( !_lpString || _nMaxCount <= 0 ) { return 0; }
		std::string sText = GetTextA( _iIdx );
		UINT uiType = 0;
		if ( _iIdx >= 0 && static_cast<size_t>(_iIdx) < m_vItems.size() ) { uiType = m_vItems[_iIdx].uiType; }
		::StringCbCopyNA( _lpString, static_cast<size_t>(_nMaxCount) * sizeof( _lpString[0] ), sText.c_str(), static_cast<size_t>(sText.size()) * sizeof( sText[0] ) );
		return MAKELONG( static_cast<WORD>(sText.size()), static_cast<WORD>(uiType | SBT_OWNERDRAW) );
	}

	/**
	 * \brief Retrieves the text for a part as UTF-16.
	 *
	 * \param _iIdx The zero-based part index.
	 * \param _lpString Destination buffer for the text.
	 * \param _nMaxCount Maximum number of characters to copy (including terminator, if applicable).
	 * \return A 32-bit value containing two 16-bit values:
	 *	The low word specifies the length, in characters, of the text.
	 *	The high word specifies the type of operation used to draw the text.
	 */
	DWORD CStatusBar::GetTextW( INT _iIdx, LPWSTR _lpString, INT _nMaxCount ) const {
		if ( !_lpString || _nMaxCount <= 0 ) { return 0; }
		std::wstring wsText = GetTextW( _iIdx );
		UINT uiType = 0;
		if ( _iIdx >= 0 && static_cast<size_t>(_iIdx) < m_vItems.size() ) { uiType = m_vItems[_iIdx].uiType; }
		::StringCbCopyNW( _lpString, static_cast<size_t>(_nMaxCount) * sizeof( _lpString[0] ), wsText.c_str(), static_cast<size_t>(wsText.size()) * sizeof( wsText[0] ) );
		return MAKELONG( static_cast<WORD>(wsText.size()), static_cast<WORD>(uiType | SBT_OWNERDRAW) );
	}

	/**
	 * \brief Gets the text of a part as ANSI.
	 *
	 * \param _iIdx The zero-based part index.
	 * \return The part text.
	 */
	std::string CStatusBar::GetTextA( INT _iIdx ) const {
		if ( _iIdx < 0 || static_cast<size_t>(_iIdx) >= m_vItems.size() ) { return std::string(); }
		return ee::CExpEval::ToUtf8( m_vItems[_iIdx].wsText );
	}

	/**
	 * \brief Gets the text of a part as UTF-16.
	 *
	 * \param _iIdx The zero-based part index.
	 * \return The part text.
	 */
	std::wstring CStatusBar::GetTextW( INT _iIdx ) const {
		if ( _iIdx < 0 || static_cast<size_t>(_iIdx) >= m_vItems.size() ) { return std::wstring(); }
		return m_vItems[_iIdx].wsText;
	}

	/**
	 * \brief Gets the text length of the given part as ANSI.
	 *
	 * \param _iIdx The zero-based part index.
	 * \return The length, in characters, of the part text.
	 */
	INT CStatusBar::GetTextLengthA( INT _iIdx ) const {
		if ( _iIdx < 0 || static_cast<size_t>(_iIdx) >= m_vItems.size() ) { return 0; }
		return static_cast<INT>(ee::CExpEval::ToUtf8( m_vItems[_iIdx].wsText ).size());
	}

	/**
	 * \brief Gets the text length of the given part as UTF-16.
	 *
	 * \param _iIdx The zero-based part index.
	 * \return The length, in characters, of the part text.
	 */
	INT CStatusBar::GetTextLengthW( INT _iIdx ) const {
		if ( _iIdx < 0 || static_cast<size_t>(_iIdx) >= m_vItems.size() ) { return 0; }
		return static_cast<INT>(m_vItems[_iIdx].wsText.size());
	}

	/**
	 * \brief Sets the text in the specified part as ANSI, forcing SBT_OWNERDRAW.
	 *
	 * \param _iIdx The zero-based part index.
	 * \param _uiType SB_SETTEXT high-word flags (SBT_*) excluding SBT_OWNERDRAW, which is always forced.
	 * \param _lpString The text to set.
	 * \return TRUE if the text was set successfully; FALSE otherwise.
	 */
	BOOL CStatusBar::SetTextA( INT _iIdx, UINT _uiType, LPCSTR _lpString ) {
		return SetTextW( _iIdx, _uiType, ee::CExpEval::ToUtf16( _lpString ).c_str() );
	}

	/**
	 * \brief Sets the text in the specified part as UTF-16, forcing SBT_OWNERDRAW.
	 *
	 * \param _iIdx The zero-based part index.
	 * \param _uiType SB_SETTEXT high-word flags (SBT_*) excluding SBT_OWNERDRAW, which is always forced.
	 * \param _lpString The text to set.
	 * \return TRUE if the text was set successfully; FALSE otherwise.
	 */
	BOOL CStatusBar::SetTextW( INT _iIdx, UINT _uiType, LPCWSTR _lpString ) {
		if ( _iIdx < 0 ) { return FALSE; }
		if ( static_cast<size_t>(_iIdx) >= m_vItems.size() ) {
			m_vItems.resize( static_cast<size_t>(_iIdx) + 1 );
		}
		m_vItems[_iIdx].uiType = _uiType;
		m_vItems[_iIdx].wsText = _lpString ? _lpString : L"";
		if ( !Wnd() ) { return FALSE; }
		LSW_SETREDRAW srRedraw( this );
		if ( static_cast<BOOL>(::SendMessageW( Wnd(), SB_SETTEXTW, static_cast<WPARAM>(_iIdx | (_uiType | SBT_OWNERDRAW)), static_cast<LPARAM>(_iIdx) )) ) {
			AutoFitPartsToTextLocked( TRUE );
			return TRUE;
		}
		return FALSE;
	}

	/**
	 * Sets the text colors for a given item.
	 * 
	 * \param _crColorNormal The normal text color.  The default is ::GetSysColor( COLOR_WINDOWTEXT ).
	 * \param _crColorDisabled The disabled text color.  Default is ::GetSysColor( COLOR_GRAYTEXT ).
	 * \param _iIdx The index of the item whose colors are to be set.
	 **/
	void CStatusBar::SetItemTextColors( COLORREF _crColorNormal, COLORREF _crColorDisabled, INT _iIdx ) {
		if ( _iIdx < 0 ) { return; }
		if ( static_cast<size_t>(_iIdx) >= m_vItems.size() ) { return; }
		m_vItems[_iIdx].crTextColor = _crColorNormal;
		m_vItems[_iIdx].crDisabledTextColor = _crColorDisabled;
	}

	/**
	 * Sets the background color for a given item.
	 * 
	 * \param _coColor The color to set.
	 * \param _iIdx The index of the item whose background color is to be set.
	 **/
	void CStatusBar::SetItemBkColor( COLORREF _coColor, INT _iIdx ) {
		if ( _iIdx < 0 ) { return; }
		if ( static_cast<size_t>(_iIdx) >= m_vItems.size() ) { return; }
		m_vItems[_iIdx].crBkColor = _coColor;
	}

	/**
	 * \brief Handles WM_DRAWITEM for this status bar.
	 *
	 * \param _pDis The DRAWITEMSTRUCT pointer received from WM_DRAWITEM.
	 * \return TRUE if the item was drawn; FALSE if the message was not for this control.
	 */
	BOOL CStatusBar::HandleDrawItem( const DRAWITEMSTRUCT * _pDis ) const {
		if ( !_pDis || !_pDis->hwndItem || _pDis->hwndItem != Wnd() ) { return FALSE; }
		if ( _pDis->itemID == static_cast<UINT>(-1) ) { return FALSE; }

		UINT uiIdx = static_cast<UINT>(_pDis->itemID);
		if ( uiIdx >= m_vItems.size() ) { return FALSE; }

		const RECT & rItem = _pDis->rcItem;
		const int iW = rItem.right - rItem.left;
		const int iH = rItem.bottom - rItem.top;
		if ( iW <= 0 || iH <= 0 ) { return TRUE; }

		HDC hMemDc = ::CreateCompatibleDC( _pDis->hDC );
		if ( !hMemDc ) { return FALSE; }

		HBITMAP hBmp = ::CreateCompatibleBitmap( _pDis->hDC, iW, iH );
		if ( !hBmp ) {
			::DeleteDC( hMemDc );
			return FALSE;
		}
		
		{
			LSW_SELECTOBJECT soBmp( hMemDc, hBmp );

			// Draw into local coordinates (0..W, 0..H).
			RECT rLocal{ 0, 0, iW, iH };

			HFONT hFont = Font();
			LSW_SELECTOBJECT soFont( hMemDc, hFont, false, NULL != hFont );

			// Copy target DC settings that might matter.
			//::SetBkMode( hMemDc, ::GetBkMode( _pDis->hDC ) );
			LSW_SETBKMODE sbmBkMode( hMemDc, ::GetBkMode( _pDis->hDC ) );
			LSW_SETTEXTCOLOR stcTextColor( hMemDc, ::GetTextColor( _pDis->hDC ) );
			LSW_SETBKCOLOR sbcBkColor( hMemDc, ::GetBkColor( _pDis->hDC ) );

			// Now do your normal draw into the backbuffer.
			DefaultDrawStatusPart(
				hMemDc,
				rLocal,
				m_vItems[uiIdx].uiType,
				m_vItems[uiIdx].wsText.c_str(),
				m_bEnabled != FALSE,
				uiIdx
			);

			// One blit to the real DC.
			::BitBlt( _pDis->hDC, rItem.left, rItem.top, iW, iH, hMemDc, 0, 0, SRCCOPY );
		}
		::DeleteObject( hBmp );
		::DeleteDC( hMemDc );

		return TRUE;
	}

	/**
	 * Gets the font used by this control.
	 * 
	 * \return Returns the font used by this control.
	 **/
	HFONT CStatusBar::Font() const {
		HFONT hFont = GetFont();
		if ( !hFont ) {
			NONCLIENTMETRICSW ncm{};
			ncm.cbSize = sizeof( ncm );
			if ( ::SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0 ) ) {
				hFont = ::CreateFontIndirectW( &ncm.lfStatusFont );
			}
			if ( !hFont ) {
				hFont = reinterpret_cast<HFONT>(::GetStockObject( DEFAULT_GUI_FONT ));
			}
		}
		return hFont;
	}

	/**
	 * \brief Automatically sizes parts to their text and anchors a right-locked suffix to the right edge.
	 *
	 * All parts before the first bLockToRight part are laid out from left-to-right. All parts from the first
	 * bLockToRight part to the end are laid out from right-to-left (anchored to the right edge).
	 *
	 * By default, the last non-locked part becomes a “filler” that expands to consume remaining space up to
	 * the left edge of the right-locked suffix (or to the end if no suffix exists).
	 *
	 * \param _bFillLastNonLocked If TRUE, the last non-locked part is expanded to fill remaining space.
	 * \return TRUE on success; FALSE otherwise.
	 */
	BOOL CStatusBar::AutoFitPartsToTextLocked( BOOL _bFillLastNonLocked ) {
		if ( !Wnd() ) { return FALSE; }
		try {
			RECT rClient{};
			::GetClientRect( Wnd(), &rClient );
			int iClientW = rClient.right - rClient.left;
			if ( iClientW <= 0 ) { return TRUE; }

			// If the size grip is enabled, the native control visually occupies the bottom-right corner.
			// Reserving its width prevents “filler” text from ending under the grip.
			LONG_PTR lpStyle = ::GetWindowLongPtrW( Wnd(), GWL_STYLE );
			int iGripReserve = 0;
			if ( (lpStyle & SBARS_SIZEGRIP) != 0 ) {
				iGripReserve = ::GetSystemMetrics( SM_CXVSCROLL );
				if ( iGripReserve < 0 ) { iGripReserve = 0; }
			}

			const size_t sParts = m_vItems.size();
			if ( sParts == 0 ) { return TRUE; }

			// Determine the right-locked suffix start (must be a contiguous suffix for SB_SETPARTS ordering).
			size_t sRightStart = sParts;
			for ( size_t I = 0; I < sParts; ++I ) {
				if ( I < m_vParts.size() && m_vParts[I].bLockToRight ) {
					sRightStart = I;
					break;
				}
			}

			// Determine filler index: last non-locked part (just before the right-locked suffix).
			INT iFill = -1;
			if ( _bFillLastNonLocked && sRightStart > 0 ) {
				iFill = static_cast<INT>(sRightStart - 1);
			}

			// Prepare a DC for measuring using the status bar’s font.
			std::vector<int> vWidths;
			vWidths.resize( sParts );
			{
				LSW_HDC hDc( Wnd() );
				if ( !hDc.hDc ) { return FALSE; }
			
				{
					HFONT hFont = GetFont();
					LSW_SELECTOBJECT soFont( hDc.hDc, hFont, hFont != NULL );


					const int iPad = StatusPanePaddingX();

					for ( size_t I = 0; I < sParts; ++I ) {
						const UINT uiType = m_vItems[I].uiType;
						const LPCWSTR pwcText = m_vItems[I].wsText.c_str();

						int iTextW = MeasureStatusTextW( hDc.hDc, pwcText, uiType );
						int iW = iTextW + iPad;

						// Keep empty/small items sane.
						if ( iW < 24 ) { iW = 24; }

						vWidths[I] = iW;
					}
				}
			}

			// Layout.
			std::vector<INT> vRight;
			vRight.resize( sParts );

			// Right suffix placed from right-to-left.
			int iRightEdge = iClientW;
			int iRightTextEdge = iClientW - iGripReserve;
			if ( iRightTextEdge < 0 ) { iRightTextEdge = 0; }

			// Place the right-locked suffix (including the last part).
			// Note: the rectangles still go to iClientW; the reserve is only used for the *filler* boundary.
			for ( size_t I = sParts; I-- > sRightStart; ) {
				int iW = vWidths[I];
				int iLeft = iRightEdge - iW;
				if ( iLeft < 0 ) { iLeft = 0; }

				vRight[I] = iRightEdge;
				iRightEdge = iLeft;
			}

			// Now iRightEdge is the left boundary of the right-locked suffix.
			int iSuffixLeft = iRightEdge;

			// Left side placed from left-to-right, skipping filler for now.
			int iX = 0;
			for ( size_t I = 0; I < sRightStart; ++I ) {
				if ( static_cast<INT>(I) == iFill ) { continue; }

				int iW = vWidths[I];
				int iNext = iX + iW;

				// Don’t overlap into the suffix.
				if ( iNext > iSuffixLeft ) { iNext = iSuffixLeft; }
				if ( iNext < iX ) { iNext = iX; }

				vRight[I] = iNext;
				iX = iNext;
			}

			// Filler expands to the left edge of suffix (or end if no suffix).
			if ( iFill >= 0 ) {
				int iFillRight = iSuffixLeft;

				// If there is no right-locked suffix, keep text away from grip.
				if ( sRightStart == sParts ) {
					iFillRight = iRightTextEdge;
				}

				if ( iFillRight < iX ) { iFillRight = iX; }
				vRight[static_cast<size_t>(iFill)] = iFillRight;
			}

			// Ensure the last part extends to the end (lets the control handle exact edge quirks).
			vRight.back() = -1;

			// Skip redundant SB_SETPARTS calls.
			bool bSame = false;
			if ( m_vLastAutoFitRights.size() == vRight.size() ) {
				bSame = true;
				for ( size_t I = 0; I < vRight.size(); ++I ) {
					if ( m_vLastAutoFitRights[I] != vRight[I] ) { bSame = false; break; }
				}
			}

			if ( bSame ) {
				// Layout didn’t change, but parts may have lost owner-draw due to an SB_SETPARTS elsewhere.
				if ( !m_bAllOwnerDrawApplied ) {
					for ( size_t I = 0; I < m_vItems.size(); ++I ) {
						::SendMessageW( Wnd(), SB_SETTEXTW,
							static_cast<WPARAM>(static_cast<INT>(I) | (m_vItems[I].uiType | SBT_OWNERDRAW)),
							static_cast<LPARAM>(I) );
					}
					m_bAllOwnerDrawApplied = TRUE;
				}
				return TRUE;
			}

			if ( !static_cast<BOOL>(::SendMessageW( Wnd(), SB_SETPARTS, static_cast<WPARAM>(vRight.size()), reinterpret_cast<LPARAM>(vRight.data()) )) ) {
				return FALSE;
			}

			m_vLastAutoFitRights = vRight;

			// Re-apply ownerdraw flags after SB_SETPARTS (some setups lose the type).
			for ( size_t I = 0; I < m_vItems.size(); ++I ) {
				::SendMessageW( Wnd(), SB_SETTEXTW, static_cast<WPARAM>(static_cast<INT>(I) | (m_vItems[I].uiType | SBT_OWNERDRAW)), static_cast<LPARAM>(I) );
			}

			// Optionally keep m_vParts’ iRightCoord in sync (useful if you still use it elsewhere).
			if ( m_vParts.size() == vRight.size() ) {
				for ( size_t I = 0; I < vRight.size(); ++I ) {
					m_vParts[I].iRightCoord = vRight[I];
				}
			}

			// Re-assert ownerdraw after SB_SETPARTS.
			for ( size_t I = 0; I < m_vItems.size(); ++I ) {
				::SendMessageW( Wnd(), SB_SETTEXTW,
					static_cast<WPARAM>(static_cast<INT>(I) | (m_vItems[I].uiType | SBT_OWNERDRAW)),
					static_cast<LPARAM>(I) );
			}
			m_bAllOwnerDrawApplied = TRUE;

			return TRUE;
		}
		catch ( ... ) { return FALSE; }
	}

	/**
	 * Attaches an HWND to this widget after creation.
	 * \brief Finalizes control initialization once the window handle exists.
	 *
	 * \param _hWnd The created window handle.
	 */
	void CStatusBar::InitControl( HWND /*_hWnd*/ ) {
		m_wpOrigProc = CHelpers::SetWndProc( Wnd(), StatusOverride );
		::SetPropW( Wnd(), m_szProp, reinterpret_cast<HANDLE>(this) );
	}

	/**
	 * \brief Evaluates expressions to determine a new rectangle for the control.
	 *
	 * Called during layout/resizing to compute and apply an updated size/position.
	 */
	void CStatusBar::EvalNewSize() {
		::SendMessageW( Wnd(), WM_SIZE, 0L, 0L );
		/*::GetWindowRect( Wnd(), &m_rRect );
		::GetClientRect( Wnd(), &m_rClientRect );*/

		if ( m_pwParent && m_vParts.size() ) {
			LONG lChange = m_pwParent->WindowRect( this ).Width() - m_pwParent->StartRect().Width();
			std::vector<INT> vInts;
			vInts.reserve( m_vParts.size() );
			for ( size_t I = 0; I < m_vParts.size(); ++I ) {
				INT iPos = m_vParts[I].bLockToRight ? (m_vParts[I].iRightCoord + lChange) : m_vParts[I].iRightCoord;
				vInts.push_back( iPos );
			}
			::SendMessageW( Wnd(), SB_SETPARTS, static_cast<WPARAM>(vInts.size()), reinterpret_cast<LPARAM>(&vInts[0]) );
			// SB_SETPARTS can clear/rebuild internal state. Re-apply owner-draw flags for all parts.
			for ( size_t I = 0; I < m_vItems.size(); ++I ) {
				::SendMessageW( Wnd(), SB_SETTEXTW, static_cast<WPARAM>(static_cast<INT>(I) | (m_vItems[I].uiType | SBT_OWNERDRAW)), static_cast<LPARAM>(I) );
			}
			m_bAllOwnerDrawApplied = FALSE;
			m_vLastAutoFitRights.clear();
		}
		AutoFitPartsToTextLocked( TRUE );
	}

	/**
	 * Handles WM_PAINT.
	 * \brief Performs painting for the client area.
	 *
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CStatusBar::Paint() {
		LSW_BEGINPAINT bpPaint( Wnd() );
		if ( !bpPaint.hDc ) { return LSW_H_HANDLED; }


		RECT rClient{};
		::GetClientRect( Wnd(), &rClient );

		const int iW = rClient.right - rClient.left;
		const int iH = rClient.bottom - rClient.top;

		if ( iW <= 0 || iH <= 0 ) {
			return LSW_H_HANDLED;
		}

		// Backbuffer for the full status bar.
		HDC hMemDc = ::CreateCompatibleDC( bpPaint.hDc );
		if ( !hMemDc ) {
			return LSW_H_HANDLED;
		}

		HBITMAP hBmp = ::CreateCompatibleBitmap( bpPaint.hDc, iW, iH );
		if ( !hBmp ) {
			::DeleteDC( hMemDc );
			return LSW_H_HANDLED;
		}

		{
			LSW_SELECTOBJECT soBmp( hMemDc, hBmp );

			// Match the paint DC font into the backbuffer.
			HGDIOBJ hFont = ::GetCurrentObject( bpPaint.hDc, OBJ_FONT );
			LSW_SELECTOBJECT soFont( hMemDc, hFont, false, NULL != hFont );

			// Fill full background like a normal status bar.
			::FillRect( hMemDc, &rClient, ::GetSysColorBrush( COLOR_3DFACE ) );

			const BOOL bEnabled = ::IsWindowEnabled( Wnd() );
			const INT iCtrlId = ::GetDlgCtrlID( Wnd() );
			HWND hParent = ::GetParent( Wnd() );

			const BOOL bSimple = static_cast<BOOL>(::SendMessageW( Wnd(), SB_ISSIMPLE, 0, 0 ));
			INT iParts = 1;
			if ( !bSimple ) {
				iParts = static_cast<INT>(::SendMessageW( Wnd(), SB_GETPARTS, 0, 0 ));
				if ( iParts <= 0 ) { iParts = 1; }
			}

			for ( INT I = 0; I < iParts; ++I ) {
				RECT rPart{};
				if ( bSimple ) {
					rPart = rClient;
				}
				else {
					if ( !static_cast<BOOL>(::SendMessageW( Wnd(), SB_GETRECT, static_cast<WPARAM>(I), reinterpret_cast<LPARAM>(&rPart) )) ) {
						continue;
					}
				}

				// Build a DRAWITEMSTRUCT and send WM_DRAWITEM even if not owner-draw.
				DRAWITEMSTRUCT disItem{};
				disItem.CtlType = 0;					// Status bars don’t reliably set this; hwndItem is the reliable identifier.
				disItem.CtlID = static_cast<UINT>(iCtrlId);
				disItem.itemID = static_cast<UINT>(I);
				disItem.itemAction = ODA_DRAWENTIRE;
				disItem.itemState = bEnabled ? 0 : ODS_DISABLED;
				disItem.hwndItem = Wnd();
				disItem.hDC = hMemDc;
				disItem.rcItem = rPart;
				disItem.itemData = static_cast<ULONG_PTR>(I);

				LRESULT lrHandled = 0;
				if ( hParent ) {
					lrHandled = ::SendMessageW( hParent, WM_DRAWITEM, static_cast<WPARAM>(iCtrlId), reinterpret_cast<LPARAM>(&disItem) );
				}

				// If parent didn’t handle it, draw the default look.
				if ( !lrHandled ) {
					UINT uiType = 0;
					LPCWSTR pwcText = L"";

					if ( static_cast<size_t>(I) < m_vItems.size() ) {
						uiType = m_vItems[I].uiType;
						pwcText = m_vItems[I].wsText.c_str();
					}
					else {
						// Fallback: ask the control for its current text/type if you haven’t stored it.
						WCHAR wszTmp[512]{};
						DWORD dw = static_cast<DWORD>(::SendMessageW( Wnd(), SB_GETTEXTW, static_cast<WPARAM>(I), reinterpret_cast<LPARAM>(wszTmp) ));
						uiType = HIWORD( dw );
						pwcText = wszTmp;
					}

					DefaultDrawStatusPart( hMemDc, rPart, uiType, pwcText, bEnabled, size_t( I ) );
				}
			}

			// Optional: size grip (matches normal status bar behavior).
			LONG_PTR lpStyle = ::GetWindowLongPtrW( Wnd(), GWL_STYLE );
			if ( (lpStyle & SBARS_SIZEGRIP) && !bSimple ) {
				RECT rGrip = rClient;
				const int iGripW = ::GetSystemMetrics( SM_CXVSCROLL );
				const int iGripH = ::GetSystemMetrics( SM_CYHSCROLL );
				rGrip.left = rGrip.right - iGripW;
				rGrip.top = rGrip.bottom - iGripH;
				DrawStatusGrip( Wnd(), hMemDc, rGrip );
			}

			// Blit only the invalid region (reduces work).
			const RECT & rPaint = bpPaint.psPaint.rcPaint;
			::BitBlt(
				bpPaint.hDc,
				rPaint.left,
				rPaint.top,
				rPaint.right - rPaint.left,
				rPaint.bottom - rPaint.top,
				hMemDc,
				rPaint.left,
				rPaint.top,
				SRCCOPY
			);
		}
		::DeleteObject( hBmp );
		::DeleteDC( hMemDc );

		return LSW_H_HANDLED;
	}

	/**
	 * \brief Handles WM_DRAWITEM for an owner-drawn control.
	 *
	 * \param _wControlId The control identifier (DRAWITEMSTRUCT::CtlID).
	 * \param _disItem The DRAWITEMSTRUCT describing what to draw.
	 * \return LSW_HANDLED::LSW_H_HANDLED if the message was handled and drawing was performed; otherwise
	 * returns LSW_HANDLED::LSW_H_NOT_HANDLED to allow default/other handling.
	 *
	 * \note This is typically called from the parent window's WM_DRAWITEM handler.
	 *
	 * \note For SBT_OWNERDRAW status-bar parts, DRAWITEMSTRUCT::itemID is the zero-based part index.
	 * The status-bar window handle is in DRAWITEMSTRUCT::hwndItem.
	 */
	CWidget::LSW_HANDLED CStatusBar::DrawItem( WORD _wControlId, const DRAWITEMSTRUCT &_disItem ) {
		if ( _wControlId != Id() ) { return LSW_H_CONTINUE; }

		HandleDrawItem( &_disItem );

		return LSW_H_HANDLED;
	}

	/**
	 * \brief Draws one status-bar part the same way the common control typically does.
	 *
	 * \param _hDc The target device context.
	 * \param _rRc The part rectangle.
	 * \param _uiType The stored SB_SETTEXT flags for the part (excluding SBT_OWNERDRAW).
	 * \param _pwcText The text to draw.
	 * \param _bEnabled TRUE if enabled; FALSE otherwise.
	 * \param _sIdx The index of the item being drawn.
	 */
	void CStatusBar::DefaultDrawStatusPart( HDC _hDc, const RECT &_rRc, UINT _uiType, LPCWSTR _pwcText, BOOL _bEnabled, size_t _sIdx ) const {
		RECT rItem = _rRc;

		// Background.
		::FillRect( _hDc, &rItem, lsw::CBase::BrushCache().Brush( m_vItems[_sIdx].crBkColor ) );

		LSW_THEME_DATA hTheme( Wnd(), L"STATUS", ::IsThemeActive() && ::IsAppThemed() );

		do {
			if ( hTheme.Handle() ) {
				// Draw themed pane background (includes the “bars” between items on modern Windows).
				if ( FAILED( ::DrawThemeBackground( hTheme.Handle(), _hDc, SP_PANE, 0, &rItem, NULL ) ) ) { break; }

				RECT rText = rItem;

				// Use theme margins if available, otherwise small inset.
				MARGINS m{};
				if ( SUCCEEDED( ::GetThemeMargins( hTheme.Handle(), _hDc, SP_PANE, 0, TMT_CONTENTMARGINS, &rItem, &m ) ) ) {
					rText.left += m.cxLeftWidth;
					rText.right -= m.cxRightWidth;
					rText.top += m.cyTopHeight;
					rText.bottom -= m.cyBottomHeight;
				}
				else {
					rText.left += 2;
					rText.right -= 2;
				}

				lsw::LSW_SETBKMODE sbmBkMode( _hDc, TRANSPARENT );

				COLORREF crText = _bEnabled ? m_vItems[_sIdx].crTextColor : m_vItems[_sIdx].crDisabledTextColor;
				/*COLORREF crThemeText{};
				if ( SUCCEEDED( ::GetThemeColor( hTheme.Handle(), SP_PANE, 0, TMT_TEXTCOLOR, &crThemeText ) ) ) {
					if ( _bEnabled ) { crText = crThemeText; }
				}*/
				lsw::LSW_SETTEXTCOLOR stcTextColor( _hDc, crText );

				UINT uiBase = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX;
				if ( _uiType & SBT_RTLREADING ) { uiBase |= DT_RTLREADING; }

				if ( (_uiType & SBT_NOTABPARSING) != 0 || !_pwcText ) {
					::DrawThemeText( hTheme.Handle(), _hDc, SP_PANE, 0, _pwcText ? _pwcText : L"", -1, uiBase | DT_LEFT, 0, &rText );
				}
				else {
					// Tab parsing: left\tcenter\tright.
					try {
						std::wstring wsTmp = _pwcText;
						size_t s0 = wsTmp.find( L'\t' );
						if ( s0 == std::wstring::npos ) {
							::DrawThemeText( hTheme.Handle(), _hDc, SP_PANE, 0, wsTmp.c_str(), -1, uiBase | DT_LEFT, 0, &rText );
						}
						else {
							std::wstring wsLeft = wsTmp.substr( 0, s0 );
							size_t s1 = wsTmp.find( L'\t', s0 + 1 );
							std::wstring wsMid;
							std::wstring wsRight;
							if ( s1 == std::wstring::npos ) {
								wsRight = wsTmp.substr( s0 + 1 );
							}
							else {
								wsMid = wsTmp.substr( s0 + 1, s1 - (s0 + 1) );
								wsRight = wsTmp.substr( s1 + 1 );
							}

							if ( wsLeft.size() ) { ::DrawThemeText( hTheme.Handle(), _hDc, SP_PANE, 0, wsLeft.c_str(), -1, uiBase | DT_LEFT, 0, &rText ); }
							if ( wsMid.size() ) { ::DrawThemeText( hTheme.Handle(), _hDc, SP_PANE, 0, wsMid.c_str(), -1, uiBase | DT_CENTER, 0, &rText ); }
							if ( wsRight.size() ) { ::DrawThemeText( hTheme.Handle(), _hDc, SP_PANE, 0, wsRight.c_str(), -1, uiBase | DT_RIGHT, 0, &rText ); }
						}
					}
					catch ( ... ) {}
				}

				return;
			}
		} while ( false );

		// Border.
		if ( (_uiType & SBT_NOBORDERS) == 0 ) {
			UINT uiEdge = (_uiType & SBT_POPOUT) ? EDGE_RAISED : EDGE_SUNKEN;
			::DrawEdge( _hDc, &rItem, uiEdge, BF_RECT | BF_ADJUST );
		}

		// Text inset.
		rItem.left += 2;
		rItem.right -= 2;

		
		lsw::LSW_SETBKMODE sbmBkMode( _hDc, TRANSPARENT );
		lsw::LSW_SETTEXTCOLOR stcTextColor( _hDc, _bEnabled ? m_vItems[_sIdx].crTextColor : m_vItems[_sIdx].crDisabledTextColor );

		UINT uiBase = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX;
		if ( _uiType & SBT_RTLREADING ) { uiBase |= DT_RTLREADING; }

		if ( (_uiType & SBT_NOTABPARSING) != 0 || !_pwcText ) {
			::DrawTextW( _hDc, _pwcText ? _pwcText : L"", -1, &rItem, uiBase | DT_LEFT );
		}
		else {
			try {
				// Tab parsing: left\tcenter\tright.
				std::wstring wsTmp = _pwcText;
				size_t s0 = wsTmp.find( L'\t' );
				if ( s0 == std::wstring::npos ) {
					::DrawTextW( _hDc, wsTmp.c_str(), -1, &rItem, uiBase | DT_LEFT );
				}
				else {
					std::wstring wsLeft = wsTmp.substr( 0, s0 );
					size_t s1 = wsTmp.find( L'\t', s0 + 1 );
					std::wstring wsMid;
					std::wstring wsRight;
					if ( s1 == std::wstring::npos ) {
						wsRight = wsTmp.substr( s0 + 1 );
					}
					else {
						wsMid = wsTmp.substr( s0 + 1, s1 - (s0 + 1) );
						wsRight = wsTmp.substr( s1 + 1 );
					}

					if ( wsLeft.size() ) {
						::DrawTextW( _hDc, wsLeft.c_str(), -1, &rItem, uiBase | DT_LEFT );
					}
					if ( wsMid.size() ) {
						::DrawTextW( _hDc, wsMid.c_str(), -1, &rItem, uiBase | DT_CENTER );
					}
					if ( wsRight.size() ) {
						::DrawTextW( _hDc, wsRight.c_str(), -1, &rItem, uiBase | DT_RIGHT );
					}
				}
			}
			catch ( ... ) {}
		}
	}

	/**
	 * The message handler for the Status Bar (implements custom drawing).
	 * 
	 * \param _hWnd Window receiving the message.
	 * \param _uMsg Message identifier (WM_*).
	 * \param _wParam WPARAM message data.
	 * \param _lParam LPARAM message data.
	 * \return Returns an LRESULT as defined by the message semantics.
	 **/
	LRESULT CALLBACK CStatusBar::StatusOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CStatusBar * ptlThis = reinterpret_cast<CStatusBar *>(::GetPropW( _hWnd, m_szProp ));
		WNDPROC wpOrig = ptlThis->m_wpOrigProc;
		switch ( _uMsg ) {
			// =======================================
			// Painting.
			// =======================================
			case WM_PAINT : {
				ptlThis->Paint();
				return 0;
			}
		}

		return ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
	}

	/**
	 * \brief Draws the standard status-bar size grip in the lower-right corner.
	 *
	 * This routine draws a size grip only when the status-bar has the SBARS_SIZEGRIP style.
	 * If visual styles/themes are active, it draws the themed gripper (STATUS class, SP_GRIPPER).
	 * Otherwise it falls back to the classic non-themed size grip using DrawFrameControl().
	 *
	 * \param _hWnd The status-bar window handle.
	 * \param _hDc The device context to draw into.
	 * \param _rClient The full client rectangle of the status bar in the coordinate space of \p _hDc.
	 *
	 * \note This should be called after the status-bar background and panes have been drawn so the
	 * gripper is rendered on top.
	 *
	 * \note The grip rectangle size is derived from SM_CXVSCROLL and SM_CYHSCROLL to match the system’s
	 * standard grip metrics.
	 */
	void CStatusBar::DrawStatusGrip( HWND _hWnd, HDC _hDc, const RECT &_rClient ) {
		if ( !(::GetWindowLongPtrW( _hWnd, GWL_STYLE ) & SBARS_SIZEGRIP) ) { return; }

		RECT rGrip = _rClient;
		const int iGripW = ::GetSystemMetrics( SM_CXVSCROLL );
		const int iGripH = ::GetSystemMetrics( SM_CYHSCROLL );
		rGrip.left = rGrip.right - iGripW;
		rGrip.top = rGrip.bottom - iGripH;

		LSW_THEME_DATA hTheme( _hWnd, L"STATUS", ::IsThemeActive() && ::IsAppThemed() );
		if ( hTheme.Handle() ) {
			::DrawThemeBackground( hTheme.Handle(), _hDc, SP_GRIPPER, 0, &rGrip, NULL );
			return;
		}

		::DrawFrameControl( _hDc, &rGrip, DFC_SCROLL, DFCS_SCROLLSIZEGRIP );
	}

	/**
	 * \brief Measures the pixel width of a single UTF-16 text segment using DrawTextW() in DT_CALCRECT mode.
	 *
	 * This helper assumes the caller has already selected the desired font into \p _hDc.
	 *
	 * \param _hDc The device context used for measurement.
	 * \param _pwc A NUL-terminated UTF-16 string to measure. If NULL or empty, returns 0.
	 * \param _uiDrawFlags The DrawTextW() flags used for measurement. This function always ORs DT_CALCRECT
	 * into the call, so \p _uiDrawFlags should include alignment and single-line flags as desired.
	 * \return The width, in pixels, of the rendered text.
	 */
	int CStatusBar::MeasureTextSegmentW( HDC _hDc, const wchar_t * _pwc, UINT _uiDrawFlags ) {
		if ( !_pwc || !_pwc[0] ) { return 0; }
		LSW_RECT rRect { 0, 0, 0, 0 };
		::DrawTextW( _hDc, _pwc, -1, &rRect, _uiDrawFlags | DT_CALCRECT );
		return int( rRect.Width() );
	}

	/**
	 * \brief Measures the pixel width of status-bar text as it would be rendered for a pane.
	 *
	 * If tab parsing is enabled (i.e. SBT_NOTABPARSING is not set) and the string contains one or two tab
	 * characters, the status bar can display up to 3 segments (left, center, right). This helper measures
	 * each segment independently and returns the maximum segment width, which is sufficient for sizing a
	 * pane intended to contain that text without truncation for any of the three alignments.
	 *
	 * This helper assumes the caller has already selected the desired font into \p _hDc.
	 *
	 * \param _hDc The device context used for measurement.
	 * \param _pwcText A NUL-terminated UTF-16 string to measure. If NULL or empty, returns 0.
	 * \param _uiType The status-bar text type flags (SBT_*). Only SBT_NOTABPARSING and SBT_RTLREADING
	 * are considered for measurement.
	 * \return The width, in pixels, required for the widest segment of the text as rendered by a status bar.
	 */
	int CStatusBar::MeasureStatusTextW( HDC _hDc, LPCWSTR _pwcText, UINT _uiType ) {
		if ( !_pwcText || !_pwcText[0] ) { return 0; }

		UINT uiBase = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX;
		if ( _uiType & SBT_RTLREADING ) { uiBase |= DT_RTLREADING; }

		// If tab parsing is enabled, measure the widest of left/mid/right segments.
		if ( (_uiType & SBT_NOTABPARSING) == 0 ) {
			const wchar_t * pTab0 = std::wcschr( _pwcText, L'\t' );
			if ( pTab0 ) {
				try {
					std::wstring ws( _pwcText );
					size_t s0 = ws.find( L'\t' );
					size_t s1 = ws.find( L'\t', s0 + 1 );

					std::wstring wsLeft = ws.substr( 0, s0 );
					std::wstring wsMid;
					std::wstring wsRight;

					if ( s1 == std::wstring::npos ) {
						wsRight = ws.substr( s0 + 1 );
					}
					else {
						wsMid = ws.substr( s0 + 1, s1 - (s0 + 1) );
						wsRight = ws.substr( s1 + 1 );
					}

					int iL = wsLeft.size() ? MeasureTextSegmentW( _hDc, wsLeft.c_str(), uiBase ) : 0;
					int iM = wsMid.size() ? MeasureTextSegmentW( _hDc, wsMid.c_str(), uiBase ) : 0;
					int iR = wsRight.size() ? MeasureTextSegmentW( _hDc, wsRight.c_str(), uiBase ) : 0;

					int iMax = (iL > iM) ? ((iL > iR) ? iL : iR) : ((iM > iR) ? iM : iR);
					return iMax;
				}
				catch ( ... ) {}
			}
		}

		return MeasureTextSegmentW( _hDc, _pwcText, uiBase );
	}

	/**
	 * \brief Returns the default horizontal padding used when converting measured text width into a pane width.
	 *
	 * The returned value represents extra horizontal space added to the measured text width to account for
	 * typical status-bar insets, separators, and visual breathing room so text does not touch pane edges.
	 *
	 * \return The horizontal padding, in pixels, to add to the measured text width when computing pane width.
	 */
	int CStatusBar::StatusPanePaddingX() {
		return 4;
	}

}	// namespace lsw

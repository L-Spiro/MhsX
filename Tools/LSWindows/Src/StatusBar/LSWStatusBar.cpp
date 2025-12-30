#include "LSWStatusBar.h"
#include <commctrl.h>
#include <strsafe.h>


namespace lsw {

	CStatusBar::CStatusBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
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

		// Force SBT_OWNERDRAW for every part.
		for ( size_t I = 0; I < _sTotal; ++I ) {
			::SendMessageW( Wnd(), SB_SETTEXTW, static_cast<WPARAM>( static_cast<INT>(I), (m_vItems[I].uiType | SBT_OWNERDRAW) ), static_cast<LPARAM>(I) );
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
		return static_cast<BOOL>(::SendMessageW( Wnd(), SB_SETTEXTW, static_cast<WPARAM>( _iIdx, (_uiType | SBT_OWNERDRAW) ), static_cast<LPARAM>(_iIdx) ));
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

		LSW_SELECTOBJECT soBmp( hMemDc, hBmp );

		// Draw into local coordinates (0..W, 0..H).
		RECT rLocal{ 0, 0, iW, iH };

		// Match font/state from the target DC (status bars often rely on this).
		HGDIOBJ hFont = ::GetCurrentObject( _pDis->hDC, OBJ_FONT );
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

		::DeleteObject( hBmp );
		::DeleteDC( hMemDc );

		return TRUE;
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
				::SendMessageW( Wnd(), SB_SETTEXTW, static_cast<WPARAM>( static_cast<INT>(I), (m_vItems[I].uiType | SBT_OWNERDRAW) ), static_cast<LPARAM>(I) );
			}
		}
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

		return LSW_H_HANDLED;

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
				::DrawFrameControl( hMemDc, &rGrip, DFC_SCROLL, DFCS_SCROLLSIZEGRIP );
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

		

		// Border.
		/*if ( (_uiType & SBT_NOBORDERS) == 0 ) {
			UINT uiEdge = (_uiType & SBT_POPOUT) ? EDGE_RAISED : EDGE_SUNKEN;
			::DrawEdge( _hDc, &rItem, uiEdge, BF_RECT | BF_ADJUST );
		}*/

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

}	// namespace lsw

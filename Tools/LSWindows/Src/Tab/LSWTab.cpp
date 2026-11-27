#include "LSWTab.h"
#include "../Base/LSWBase.h"
#include "../Helpers/LSWHelpers.h"
#include <codecvt>
#include <uxtheme.h>
#include <vssym32.h>
#include <windowsx.h>
#pragma comment( lib, "UxTheme.lib" )

#define LSW_TAB_MARGIN		2								// Space to the left and right of the row of tabs (room for the larger selected tab).
#define LSW_TAB_PADDING		6								// Space to the left and right of the text in a tab.
#define LSW_TAB_BOX_GAP		4								// Space on both sides of a close box or check box.

namespace lsw {

	CTab::CTab( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}
	CTab::~CTab() {
		/*for ( auto I = m_vTabs.size(); I--; ) {
			CWidget * pwDeleteMe = m_vTabs[I].pwWidget;
			m_vTabs[I].pwWidget = nullptr;
			delete pwDeleteMe;
		}*/
		m_vTabs.clear();
	}

	// == Functions.
	/**
	 * Gets the virtual client rectangle.
	 * \brief Allows parents to adjust available space for toolbars, status bars, or splitters.
	 *
	 * \param pwChild Optional child for which the virtual rect is requested.
	 * \return Returns the virtual client rectangle.
	 */
	const LSW_RECT CTab::VirtualClientRect( const CWidget * /*pwChild*/ ) const {
		LSW_RECT rRect = ClientRect();
		AdjustRect( FALSE, &rRect );

		return rRect;
	}

	// Calculates a tab control's display area given a window rectangle, or calculates the window rectangle that would correspond to a specified display area.
	void CTab::AdjustRect( BOOL _bLarger, RECT * prRect ) const {
		if ( !Wnd() ) { return; }
		::SendMessageW( Wnd(), TCM_ADJUSTRECT, static_cast<WPARAM>(_bLarger), reinterpret_cast<LPARAM>(prRect) );
	}

	// Inserts a new tab in a tab control. Returns the index of the new tab if successful, or -1 otherwise.
	int CTab::InsertItem( int _iItem, LPTCITEMW _piItem, CWidget * _pwWidget ) {
		if ( !Wnd() ) { return -1; }
		int iRet = static_cast<int>(::SendMessageW( Wnd(), TCM_INSERTITEMW, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(_piItem) ));
		if ( iRet != -1 ) {
			LSW_TAB tNewTab = { _pwWidget };
			if ( _piItem->mask & TCIF_TEXT && _piItem->pszText ) {
				tNewTab.sTextLen = std::wcslen( _piItem->pszText );
			}
			m_vTabs.insert( m_vTabs.begin() + iRet, tNewTab );
			UpdateLayout();
			SetCurSel( iRet );
		}

		return iRet;
	}

	// Inserts a new tab in a tab control. Returns the index of the new tab if successful, or -1 otherwise.
	int CTab::InsertItem( int _iItem, LPTCITEMA _piItem, CWidget * _pwWidget ) {
		if ( !Wnd() ) { return -1; }
		int iRet = static_cast<int>(::SendMessageA( Wnd(), TCM_INSERTITEMA, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(_piItem) ));
		if ( iRet != -1 ) {
			LSW_TAB tNewTab = { _pwWidget };
			if ( _piItem->mask & TCIF_TEXT && _piItem->pszText ) {
				tNewTab.sTextLen = std::strlen( _piItem->pszText );
			}
			m_vTabs.insert( m_vTabs.begin() + iRet, tNewTab );
			UpdateLayout();
			SetCurSel( iRet );
		}

		return iRet;
	}

	// Removes an item from a tab control.
	void CTab::DeleteItem( int _iItem, bool _bDestroyChildWindow ) {
		if ( !Wnd() ) { return; }
		int iCur = GetCurSel();
		if ( ::SendMessageW( Wnd(), TCM_DELETEITEM, static_cast<WPARAM>(_iItem), 0L ) ) {
			if ( _bDestroyChildWindow ) {
				CWidget * pwDeleteMe = m_vTabs[_iItem].pwWidget;
				m_vTabs[_iItem].pwWidget = nullptr;
				delete pwDeleteMe;
			}
			m_vTabs.erase( m_vTabs.begin() + _iItem );
			if ( m_iHotTab >= int( m_vTabs.size() ) ) { m_iHotTab = -1; }
			UpdateLayout();

			int iCount = GetItemCount();
			if ( iCount && iCur == _iItem ) {
				// The currently selected item was deleted.  Select the next in line.
				SetCurSel( std::min( iCur, iCount - 1 ) );
			}
		}
	}

	// Retrieves the number of tabs in the tab control.
	int CTab::GetItemCount() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<int>(::SendMessageW( Wnd(), TCM_GETITEMCOUNT, 0L, 0L ));
	}

	// Selects a tab in a tab control.  Returns the index of the previously selected tab if successful, or -1 otherwise.
	int CTab::SetCurSel( int _iItem ) {
		if ( !Wnd() ) { return -1; }
		int iPrev = static_cast<int>(::SendMessageW( Wnd(), TCM_SETCURSEL, static_cast<WPARAM>(_iItem), 0L ));

		// Gather target + count.
		size_t stCount = m_vTabs.size();
		if ( _iItem < 0 || static_cast<size_t>(_iItem) >= stCount ) { return iPrev; }

		HWND hParent = ::GetParent( Wnd() );
		HWND hTarget = m_vTabs[static_cast<size_t>(_iItem)].pwWidget ? m_vTabs[static_cast<size_t>(_iItem)].pwWidget->Wnd() : nullptr;

		// Disable redraw on parent to avoid intermediate paints.
		::SendMessageW( hParent, WM_SETREDRAW, FALSE, 0 );
		::SendMessageW( Wnd(), WM_SETREDRAW, FALSE, 0 );

		// Batch Z-order + visibility changes.
		HDWP hDwp = ::BeginDeferWindowPos( static_cast<int>(stCount) );

		// 1) Bring the target to the top and show it.
		if ( hTarget ) {
			hDwp = ::DeferWindowPos(
				hDwp,
				hTarget,
				HWND_TOP,						// Ensure it is above siblings so WS_CLIPSIBLINGS wonÅft occlude it
				0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW
			);
		}

		// 2) Hide all other pages.
		for ( size_t I = 0; I < stCount; ++I ) {
			if ( I == static_cast<size_t>(_iItem) ) { continue; }
			if ( m_vTabs[I].pwWidget ) {
				hDwp = ::DeferWindowPos(
					hDwp,
					m_vTabs[I].pwWidget->Wnd(),
					nullptr,
					0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW
				);
			}
		}

		if ( hDwp ) { ::EndDeferWindowPos( hDwp ); }

		// Re-enable redraw.
		::SendMessageW( Wnd(), WM_SETREDRAW, TRUE, 0 );
		::SendMessageW( hParent, WM_SETREDRAW, TRUE, 0 );

		if ( hTarget ) {
			::RedrawWindow(
				hTarget,
				nullptr, nullptr,
				RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW
			);
		}

		// Also refresh the container frame once (no NOERASE to avoid stale buffer).
		::RedrawWindow(
			hParent,
			nullptr, nullptr,
			RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME
		);

		return iPrev;
	}

	// Determines the currently selected tab in a tab control.
	INT CTab::GetCurSel() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<int>(::SendMessageW( Wnd(), TCM_GETCURSEL, 0L, 0L ));
	}

	// Determines which tab, if any, is at a specified screen position.  Returns the index of the tab, or -1 if no tab is at the specified position.
	int CTab::HitTest( LPTCHITTESTINFO _lphtiInfo ) const {
		if ( !Wnd() ) { return -1; }
		return static_cast<int>(::SendMessageW( Wnd(), TCM_HITTEST, 0L, reinterpret_cast<LPARAM>(_lphtiInfo) ));
	}

	// Gets the rectangle of a given tab.
	LSW_RECT CTab::GetItemRect( INT _iTab ) const {
		LSW_RECT rTemp = LSW_RECT().Zero();
		if ( !Wnd() ) { return rTemp; }
		/*BOOL bRet = */static_cast<BOOL>(::SendMessageW( Wnd(), TCM_GETITEMRECT, static_cast<WPARAM>(_iTab), reinterpret_cast<LPARAM>(&rTemp) ));
		return rTemp;
	}

	/**
	 * Gets the width a tab needs for its text (the text plus the padding on both sides, not counting a close box or check box).
	 *
	 * \param _iTab The index of the tab.
	 * \return Returns the width in pixels.
	 **/
	int CTab::GetTabTextWidth( INT _iTab ) const {
		if ( !Wnd() || _iTab < 0 || _iTab >= static_cast<INT>(m_vTabs.size()) ) { return 0; }
		std::wstring wsText = GetItemTextW( _iTab );
		SIZE sSize = {};
		{
			LSW_HDC hDc( Wnd() );
			if ( !hDc.hDc ) { return LSW_TAB_PADDING * 2; }
			LSW_SELECTOBJECT soFont( hDc.hDc, TabFont() );
			if ( wsText.size() ) { ::GetTextExtentPoint32W( hDc.hDc, wsText.c_str(), static_cast<int>(wsText.size()), &sSize ); }
		}
		return sSize.cx + LSW_TAB_PADDING * 2;

#if 0
		// Neither method is great.  Trying ::DrawTextW() returns bad values and ::SetWindowLongPtrW() changes the window style back and forth, potentially causing flicker?
		//	It is only used when inserting and removing items so flicker, if it exists, can't be noticed, but it is still a hack.
		// Any better way?
		DWORD dwStyle = static_cast<DWORD>(::GetWindowLongPtrW( Wnd(), GWL_STYLE ));
		// Go back to normal style that allows tabs to have different widths.
		::SetWindowLongPtrW( Wnd(), GWL_STYLE, dwStyle & ~(TCS_FIXEDWIDTH | TCS_FORCELABELLEFT | TCS_SINGLELINE) );
		// With this style, the system will do the math for us.
		LSW_RECT rItem = GetItemRect( _iTab );
		// Put the styles back.
		::SetWindowLongPtrW( Wnd(), GWL_STYLE, dwStyle );
		return rItem.Width();

		/*HDC hDc = ::GetDC( Wnd() );
		LSW_RECT rRet;
		rRet.Zero();
		HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW( Wnd(), WM_GETFONT, 0L, 0L ));
		{
			LSW_SELECTOBJECT soFontOrig( hDc, hFont );	// Destructor sets the original font back.

			std::wstring wString = GetItemTextW( _iTab );
			::DrawTextW( hDc, wString.c_str(), static_cast<int>(wString.size()), &rRet, DT_CALCRECT );
		}

		::ReleaseDC( Wnd(), hDc );

		return rRet.Width();*/
#endif	// #if 0
	}

	// Gets the width of the longest text in any tab.
	int CTab::GetLongestTextWidth() const {
		int iMax = 0;
		for ( auto I = m_vTabs.size(); I--; ) {
			iMax = max( iMax, GetTabTextWidth( static_cast<int>(I) ) );
		}
		return iMax;
	}

	// Sets the rectangle of a given tab.
	DWORD CTab::SetItemSize( int _iW, int _iH ) {
		if ( !Wnd() ) { return 0; }
		return static_cast<DWORD>(::SendMessageW( Wnd(), TCM_SETITEMSIZE, 0L, MAKELPARAM( _iW, _iH ) ));
	}

	// Gets item text.
	std::string CTab::GetItemTextA( int _iItem ) const {
		return ee::CExpEval::ToUtf8( GetItemTextW( _iItem ) );
	}

	// Gets item text.
	std::wstring CTab::GetItemTextW( int _iItem ) const {
		if ( _iItem >= static_cast<int>(m_vTabs.size()) ) { return std::wstring(); }
		std::wstring sTemp;
		TCITEMW iItem = { TCIF_TEXT };
		size_t sAllocLen = m_vTabs[_iItem].sTextLen + 2;
		iItem.pszText = new( std::nothrow ) WCHAR[sAllocLen];
		if ( !iItem.pszText ) { return std::wstring(); }
		::ZeroMemory( iItem.pszText, sAllocLen * sizeof( WCHAR ) );
		iItem.cchTextMax = static_cast<int>(sAllocLen - 1);
		BOOL bSuccess = static_cast<BOOL>(::SendMessageW( Wnd(), TCM_GETITEMW, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&iItem) ));
		if ( bSuccess ) {
			sTemp = iItem.pszText;
		}
		delete [] iItem.pszText;		
		return sTemp;
	}

	/**
	 * Sets item text.
	 *
	 * \param _iItem The index of the tab.
	 * \param _pwcText The new text.
	 * \return Returns true if successful.
	 **/
	bool CTab::SetItemTextW( int _iItem, LPCWSTR _pwcText ) {
		if ( !Wnd() || !_pwcText || _iItem < 0 || _iItem >= static_cast<int>(m_vTabs.size()) ) { return false; }
		TCITEMW iItem = { TCIF_TEXT };
		iItem.pszText = const_cast<LPWSTR>(_pwcText);
		if ( !::SendMessageW( Wnd(), TCM_SETITEMW, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&iItem) ) ) { return false; }
		m_vTabs[_iItem].sTextLen = std::wcslen( _pwcText );
		UpdateLayout();
		return true;
	}

	/**
	 * Attaches an HWND to this widget after creation.
	 * \brief Finalizes control initialization once the window handle exists.
	 *
	 * \param _hWnd The created window handle.
	 */
	void CTab::InitControl( HWND _hWnd ) {
		CWidget::InitControl( _hWnd );
		m_lpOriginProc = ::GetWindowLongPtrW( Wnd(), GWLP_WNDPROC );
		::SetWindowLongPtrW( _hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(CustomTabProc) );

		// The native control keeps narrow fixed-width tabs (so it never shows scroll arrows); where the tabs are shown is decided by
		//	UpdateLayout().
		DWORD dwStyle = static_cast<DWORD>(::GetWindowLongPtrW( Wnd(), GWL_STYLE ));
		::SetWindowLongPtrW( Wnd(), GWL_STYLE, (dwStyle | TCS_FIXEDWIDTH | TCS_FORCELABELLEFT | TCS_SINGLELINE) & ~TCS_MULTILINE );
		::SendMessageW( Wnd(), TCM_SETMINTABWIDTH, 0, 1 );
		UpdateLayout();
	}

	/**
	 * Sets a tab to be checkable.
	 * 
	 * \param _iItem The index of the tab to set as checkable.
	 **/
	void CTab::SetCheckable( int _iItem ) {
		if ( _iItem < 0 ) {
			for ( auto I = m_vTabs.size(); I--; ) {
				m_vTabs[I].bCheckable = true;
			}
		}
		else {
			if ( _iItem < int( m_vTabs.size() ) ) { m_vTabs[_iItem].bCheckable = true; }
		}
		// Check boxes take room.
		UpdateLayout();
	}

	/**
	 * Sets a tab as checked or not.
	 * 
	 * \param _iItem The index of the tab to set or unset as checked.
	 * \param _bChecked Checked or unchecked.
	 * \return Returns true if the given tab exists.
	 **/
	bool CTab::SetChecked( int _iItem, bool _bChecked ) {
		if ( _iItem < 0 ) {
			for ( auto I = m_vTabs.size(); I--; ) {
				m_vTabs[I].bChecked = _bChecked;
			}
			return true;
		}
		else {
			if ( _iItem < int( m_vTabs.size() ) ) {
				m_vTabs[_iItem].bChecked = _bChecked;
				return true;
			}
		}
		return false;
	}

	/**
	 * Toggles the check status of a given tab.
	 * 
	 * \param _iItem The tab whose check status is to be toggled.  The tab must be marked as checkable.
	 **/
	void CTab::ToggleCheck( int _iItem ) {
		if ( _iItem < 0 ) {
			for ( auto I = m_vTabs.size(); I--; ) {
				if ( m_vTabs[I].bCheckable ) {
					m_vTabs[I].bChecked = !m_vTabs[I].bChecked;
				}
			}
		}
		else if ( _iItem < int( m_vTabs.size() ) ) {
			if ( m_vTabs[_iItem].bCheckable ) {
				m_vTabs[_iItem].bChecked = !m_vTabs[_iItem].bChecked;
			}
		}
		if ( _iItem < 0 ) {
			for ( auto I = m_vTabs.size(); I--; ) { InvalidateTab( static_cast<INT>(I) ); }
		}
		else { InvalidateTab( _iItem ); }
	}

	/**
	 * Returns true if any tabs are checkable.
	 * 
	 * \return Returns true if any tabs are checkable.
	 **/
	bool CTab::AnyAreCheckable() const {
		for ( auto I = m_vTabs.size(); I--; ) {
			if ( m_vTabs[I].bCheckable ) { return true; }
		}
		return false;
	}

	/**
	 * Checks for the given tab being checked or not.  It must also be checkable.
	 * 
	 * \param _iItem The index of the tab to check for being both checkable and checked.
	 * \return Returns true if the given tab exists, is checkable, and is checked.
	 **/
	bool CTab::IsChecked( int _iItem ) const {
		if ( _iItem < int( m_vTabs.size() ) ) {
			return m_vTabs[_iItem].bCheckable && m_vTabs[_iItem].bChecked;
		}
		return false;
	}

	/**
	 * \brief Sets the current "hot" (mouse-over) tab index, updating state only when it changes.
	 *
	 * When the hot tab changes, this function updates internal state and triggers any necessary UI refresh.
	 * Typical uses include invalidating the control for custom hover rendering and/or notifying the parent.
	 *
	 * \param _iNewHotTab The new 0-based hot tab index, or -1 for "no hot tab."
	 */
	void CTab::SetHotTab( int _iNewHotTab ) {
		if ( _iNewHotTab == m_iHotTab ) { return; }
		if ( -1 != m_iHotTab ) {
			TabMouseLeftTab( m_iHotTab );
		}
		if ( -1 != _iNewHotTab ) {
			TabMouseEnterTab( _iNewHotTab );
		}
		const int iOld = m_iHotTab;
		m_iHotTab = _iNewHotTab;

		// Only the two tabs are redrawn (into a buffer, without erasing), so nothing flickers.
		InvalidateTab( iOld );
		InvalidateTab( _iNewHotTab );
	}

	// == Functions.
	/**
	 * Handles WM_SIZE.
	 * \brief Responds to client-area size changes.
	 *
	 * Called after the window's client size changes. Override to update layouts,
	 * reposition children, or cache new sizes.
	 *
	 * \param _wParam The requested sizing type (e.g., SIZE_RESTORED, SIZE_MINIMIZED).
	 * \param _lWidth The new client width, in pixels.
	 * \param _lHeight The new client height, in pixels.
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CTab::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		CWidget::Size( _wParam, _lWidth, _lHeight );
		UpdateLayout();
		//::ShowScrollBar( Wnd(), SB_VERT, FALSE );
		//ResizeControls( VirtualClientRect( nullptr ) );

		/*for ( auto I = m_vTabs.size(); I--; ) {
			m_vTabs[I].pwWidget->EvalNewSize();
		}*/

		return LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_MOVE.
	 * \brief Notified when the window is moved.
	 *
	 * \param _lX New x-position of the window (screen coordinates).
	 * \param _lY New y-position of the window (screen coordinates).
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CTab::Move( LONG _lX, LONG _lY ) {
		CWidget::Move( _lX, _lY );
		//ResizeControls( VirtualClientRect( nullptr ) );
		/*for ( auto I = m_vTabs.size(); I--; ) {
			m_vTabs[I].pwWidget->Move( _lX, _lY );
		}*/

		return LSW_H_CONTINUE;
	}

	/**
	 * \brief Begins tracking for WM_MOUSELEAVE so we can clear the hot tab when the mouse exits the tab control.
	 *
	 * Uses TrackMouseEvent() with TME_LEAVE. Call this when receiving WM_MOUSEMOVE, but only once per hover session.
	 * WM_MOUSELEAVE will reset the tracking flag, allowing tracking to be re-armed on the next WM_MOUSEMOVE.
	 */
	void CTab::BeginMouseLeaveTracking() {
		if ( m_bTrackMouseLeave ) { return; }
		m_bTrackMouseLeave = true;

		TRACKMOUSEEVENT tmeTrackMe{};
		tmeTrackMe.cbSize = sizeof( tmeTrackMe );
		tmeTrackMe.dwFlags = TME_LEAVE;
		tmeTrackMe.hwndTrack = m_hWnd;
		::TrackMouseEvent( &tmeTrackMe );
	}

	/**
	 * Handles WM_ERASEBKGND.
	 * \brief Allows custom background erasing.
	 *
	 * \param _hDc Device context provided for erasing.
	 * \return Returns a LSW_HANDLED code.
	 */
	//CWidget::LSW_HANDLED CTab::EraseBkgnd( HDC /*_hDc*/ ) { return LSW_H_HANDLED; }

	// Determines the close rectangle.
	LSW_RECT CTab::GetCloseRect( const LSW_RECT &_rTabRect, bool /*_bHasFocus*/ ) {
		// The same for selected and unselected tabs (_rTabRect is never the larger rectangle of a selected tab).
		LSW_RECT rReturn;
		INT iHeight = _rTabRect.Height();
		INT iXW = ::GetSystemMetrics( SM_CXSMSIZE ) - 1;
		INT iXH = std::min<INT>( ::GetSystemMetrics( SM_CYSMSIZE ) - 1, iHeight );
		rReturn.left = _rTabRect.right - iXW - LSW_TAB_BOX_GAP;
		rReturn.right = rReturn.left + iXW;

		rReturn.top = _rTabRect.top + ((iHeight - iXH) >> 1) + 1;
		rReturn.bottom = rReturn.top + iXH;
		return rReturn;
	}

	/**
	 * Works out where every tab goes (from the width mode, the text, and the size of the control) and redraws the tabs.
	 **/
	void CTab::UpdateLayout() {
		if ( !Wnd() ) { return; }
		RECT rClient = {};
		::GetClientRect( Wnd(), &rClient );
		const INT iCount = static_cast<INT>(m_vTabs.size());
		const INT iAvail = std::max<INT>( 0, (rClient.right - rClient.left) - LSW_TAB_MARGIN * 2 );
		if ( !iCount ) {
			::InvalidateRect( Wnd(), nullptr, FALSE );
			return;
		}

		// The native tabs are kept narrow enough that they never need scroll arrows and leave empty space at the end (used when a
		//	click is not on a tab; see ToNativePoint()).
		SetItemSize( std::max<INT>( 1, iAvail / (iCount + 1) ), 0 );
		RECT rNative = NativeItemRect( 0 );
		m_iRowTop = rNative.top;
		m_iRowBottom = rNative.bottom;

		std::vector<INT> vWidths( static_cast<size_t>(iCount) );
		bool bFill = m_twmWidthMode == LSW_TWM_FILL;
		if ( !bFill ) {
			int64_t i64Total = 0;
			for ( INT I = 0; I < iCount; ++I ) {
				const INT iBox = BoxWidth( I );
				vWidths[I] = GetTabTextWidth( I ) + (iBox ? iBox - LSW_TAB_PADDING : 0);
				i64Total += vWidths[I];
			}
			// If they do not fit, they share the space.
			bFill = i64Total > iAvail;
		}
		if ( bFill ) {
			const INT iEach = iAvail / iCount;
			const INT iExtra = iAvail % iCount;
			for ( INT I = 0; I < iCount; ++I ) {
				vWidths[I] = iEach + ((I < iExtra) ? 1 : 0);
			}
		}

		INT iX = LSW_TAB_MARGIN;
		for ( INT I = 0; I < iCount; ++I ) {
			m_vTabs[I].rRect.left = iX;
			m_vTabs[I].rRect.right = iX + vWidths[I];
			m_vTabs[I].rRect.top = m_iRowTop;
			m_vTabs[I].rRect.bottom = m_iRowBottom;
			iX += vWidths[I];
		}

		// Only the row of tabs changes.
		RECT rStrip = { 0, 0, rClient.right, m_iRowBottom + 1 };
		::InvalidateRect( Wnd(), &rStrip, FALSE );
	}

	/**
	 * Gets the font used for the tab text.
	 *
	 * \return Returns the control's font, or the default GUI font.
	 **/
	HFONT CTab::TabFont() const {
		HFONT hFont = Wnd() ? reinterpret_cast<HFONT>(::SendMessageW( Wnd(), WM_GETFONT, 0, 0 )) : NULL;
		return hFont ? hFont : reinterpret_cast<HFONT>(::GetStockObject( DEFAULT_GUI_FONT ));
	}

	/**
	 * Gets the extra width a tab needs for its close box or check box.
	 *
	 * \param _iTab The index of the tab.
	 * \return Returns the extra width in pixels, or 0 if the tab has neither.
	 **/
	int CTab::BoxWidth( INT _iTab ) const {
		if ( _iTab < 0 || _iTab >= static_cast<INT>(m_vTabs.size()) ) { return 0; }
		if ( !m_bShowClose && !m_vTabs[_iTab].bCheckable ) { return 0; }
		return (::GetSystemMetrics( SM_CXSMSIZE ) - 1) + LSW_TAB_BOX_GAP * 2;
	}

	/**
	 * Gets the rectangle in which the native control keeps a tab (not where it is shown).
	 *
	 * \param _iTab The index of the tab.
	 * \return Returns the native rectangle.
	 **/
	RECT CTab::NativeItemRect( INT _iTab ) const {
		RECT rRect = {};
		if ( Wnd() && m_lpOriginProc ) {
			::CallWindowProcW( OriginalProc(), Wnd(), TCM_GETITEMRECT, static_cast<WPARAM>(_iTab), reinterpret_cast<LPARAM>(&rRect) );
		}
		return rRect;
	}

	/**
	 * Gets the tab at a point.
	 *
	 * \param _pPoint The point in client coordinates.
	 * \return Returns the index of the tab, or -1.
	 **/
	INT CTab::TabFromPoint( const POINT &_pPoint ) const {
		// The selected tab is drawn larger, so it is tested first.
		const INT iSel = GetCurSel();
		if ( iSel >= 0 && iSel < static_cast<INT>(m_vTabs.size()) ) {
			const RECT rSel = DrawRect( iSel, true );
			if ( ::PtInRect( &rSel, _pPoint ) ) { return iSel; }
		}
		for ( size_t I = 0; I < m_vTabs.size(); ++I ) {
			if ( ::PtInRect( &m_vTabs[I].rRect, _pPoint ) ) { return static_cast<INT>(I); }
		}
		return -1;
	}

	/**
	 * Converts the mouse position of a mouse message to the matching place in the native control's own layout, so that it
	 *	selects the tab that is shown under the mouse.
	 *
	 * \param _lParam The client coordinates from the mouse message.
	 * \return Returns the coordinates to give the native control.
	 **/
	LPARAM CTab::ToNativePoint( LPARAM _lParam ) const {
		if ( m_vTabs.empty() ) { return _lParam; }
		const POINT pPoint = { GET_X_LPARAM( _lParam ), GET_Y_LPARAM( _lParam ) };
		// Only the row of tabs is laid out differently.
		if ( pPoint.y < m_iRowTop - 2 || pPoint.y >= m_iRowBottom ) { return _lParam; }
		const INT iTab = TabFromPoint( pPoint );
		RECT rNative;
		INT iX;
		if ( iTab >= 0 ) {
			rNative = NativeItemRect( iTab );
			iX = (rNative.left + rNative.right) / 2;
		}
		else {
			// Past the native tabs, where there is no tab.
			rNative = NativeItemRect( static_cast<INT>(m_vTabs.size()) - 1 );
			iX = rNative.right + 1;
		}
		const INT iY = std::max<INT>( rNative.top, std::min<INT>( pPoint.y, rNative.bottom - 1 ) );
		return MAKELPARAM( static_cast<SHORT>(iX), static_cast<SHORT>(iY) );
	}

	/**
	 * Gets the rectangle a tab is drawn in (a selected tab is larger).
	 *
	 * \param _iTab The index of the tab.
	 * \param _bSelected Whether the tab is selected.
	 * \return Returns the rectangle in client coordinates.
	 **/
	RECT CTab::DrawRect( INT _iTab, bool _bSelected ) const {
		RECT rRect = m_vTabs[_iTab].rRect;
		if ( _bSelected ) {
			rRect.left -= 2;
			rRect.right += 2;
			rRect.top = std::max<LONG>( 0, rRect.top - 2 );
			rRect.bottom += 1;
		}
		return rRect;
	}

	/**
	 * Redraws one tab (without erasing, so there is no flicker).
	 *
	 * \param _iTab The index of the tab, or -1 to do nothing.
	 **/
	void CTab::InvalidateTab( INT _iTab ) {
		if ( !Wnd() || _iTab < 0 || _iTab >= static_cast<INT>(m_vTabs.size()) ) { return; }
		RECT rRect = DrawRect( _iTab, true );
		::InvalidateRect( Wnd(), &rRect, FALSE );
	}

	/**
	 * Draws the whole control: the background, the page area and its border, and the row of tabs.
	 *
	 * \param _hDc The device context.
	 * \param _rClient The client rectangle.
	 **/
	void CTab::PaintTabs( HDC _hDc, const RECT &_rClient ) {
		const INT iCount = static_cast<INT>(m_vTabs.size());
		LSW_THEME_DATA tdTheme( Wnd(), L"TAB", ::IsThemeActive() && ::IsAppThemed() );
		HTHEME htTheme = tdTheme.Handle();

		::FillRect( _hDc, &_rClient, ::GetSysColorBrush( COLOR_BTNFACE ) );
		RECT rPane = { _rClient.left, iCount ? m_iRowBottom : _rClient.top, _rClient.right, _rClient.bottom };
		if ( rPane.bottom > rPane.top ) {
			if ( htTheme ) { ::DrawThemeBackground( htTheme, _hDc, TABP_PANE, 0, &rPane, nullptr ); }
			else { ::DrawEdge( _hDc, &rPane, EDGE_RAISED, BF_RECT ); }
		}
		if ( !iCount ) { return; }

		const INT iSel = GetCurSel();
		const bool bEnabled = ::IsWindowEnabled( Wnd() ) != FALSE;
		const bool bFocus = ::GetFocus() == Wnd() &&
			!(::SendMessageW( Wnd(), WM_QUERYUISTATE, 0, 0 ) & UISF_HIDEFOCUS);
		LSW_SELECTOBJECT soFont( _hDc, TabFont() );
		LSW_SETBKMODE sbmMode( _hDc, TRANSPARENT );

		// Unselected tabs first; the selected one is drawn last, over its neighbors.
		for ( INT J = 0; J <= iCount; ++J ) {
			const INT I = (J == iCount) ? iSel : J;
			if ( I < 0 || I >= iCount ) { continue; }
			const bool bSel = I == iSel;
			if ( J != iCount && bSel ) { continue; }

			RECT rTab = DrawRect( I, bSel );
			const int iState = !bEnabled ? TIS_DISABLED : (bSel ? TIS_SELECTED : ((I == m_iHotTab) ? TIS_HOT : TIS_NORMAL));
			const int iPart = bSel ? ((I == 0) ? TABP_TOPTABITEMLEFTEDGE : TABP_TOPTABITEM) :
				((I == 0) ? TABP_TABITEMLEFTEDGE : TABP_TABITEM);
			COLORREF crText = ::GetSysColor( bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT );
			if ( htTheme ) {
				::DrawThemeBackground( htTheme, _hDc, iPart, iState, &rTab, nullptr );
				COLORREF crTheme;
				if ( SUCCEEDED( ::GetThemeColor( htTheme, iPart, iState, TMT_TEXTCOLOR, &crTheme ) ) ) { crText = crTheme; }
			}
			else {
				::FillRect( _hDc, &rTab, ::GetSysColorBrush( COLOR_BTNFACE ) );
				RECT rEdge = rTab;
				::DrawEdge( _hDc, &rEdge, EDGE_RAISED, BF_LEFT | BF_TOP | BF_RIGHT | BF_SOFT );
			}

			// The text (left-aligned if there is a box to its right).
			const LSW_RECT rItem = m_vTabs[I].rRect;
			const INT iBox = BoxWidth( I );
			RECT rText = rItem;
			rText.left += LSW_TAB_PADDING;
			rText.right -= iBox ? iBox : LSW_TAB_PADDING;
			if ( bSel ) { ::OffsetRect( &rText, 0, -1 ); }
			const std::wstring wsText = GetItemTextW( I );
			LSW_SETTEXTCOLOR stcText( _hDc, crText );
			if ( rText.right > rText.left ) {
				// Text starts at the reading edge of every tab: the left, or the right for right-to-left text.
				const DWORD dwExStyle = static_cast<DWORD>(::GetWindowLongPtrW( Wnd(), GWL_EXSTYLE ));
				const UINT uiAlign = (dwExStyle & (WS_EX_RTLREADING | WS_EX_RIGHT)) ? (DT_RIGHT | ((dwExStyle & WS_EX_RTLREADING) ? DT_RTLREADING : 0)) : DT_LEFT;
				::DrawTextW( _hDc, wsText.c_str(), static_cast<int>(wsText.size()), &rText,
					DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX | uiAlign );
			}

			// The close box or check box.
			if ( iBox ) {
				RECT rBox = GetCloseRect( rItem, bSel );
				if ( m_bShowClose ) { ::DrawFrameControl( _hDc, &rBox, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_FLAT | (bEnabled ? 0 : DFCS_INACTIVE) ); }
				else { ::DrawFrameControl( _hDc, &rBox, DFC_BUTTON, DFCS_BUTTONCHECK | (m_vTabs[I].bChecked ? DFCS_CHECKED : 0) | (bEnabled ? 0 : DFCS_INACTIVE) ); }
			}

			if ( bSel && bFocus ) {
				RECT rFocus = rItem;
				::InflateRect( &rFocus, -2, -2 );
				::OffsetRect( &rFocus, 0, -1 );
				::DrawFocusRect( _hDc, &rFocus );
			}
		}
	}

	/**
	 * Keeps painting off the child windows and the pages that cover the control.
	 *
	 * \param _hDc The device context to clip.
	 **/
	void CTab::ExcludeCovered( HDC _hDc ) const {
		auto fExclude = [&]( HWND _hCover ) {
			if ( !_hCover || _hCover == Wnd() || !::IsWindowVisible( _hCover ) ) { return; }
			RECT rCover;
			if ( !::GetWindowRect( _hCover, &rCover ) ) { return; }
			::MapWindowPoints( HWND_DESKTOP, Wnd(), reinterpret_cast<POINT *>(&rCover), 2 );
			::ExcludeClipRect( _hDc, rCover.left, rCover.top, rCover.right, rCover.bottom );
		};
		for ( HWND hChild = ::GetWindow( Wnd(), GW_CHILD ); hChild; hChild = ::GetWindow( hChild, GW_HWNDNEXT ) ) {
			fExclude( hChild );
		}
		// Pages may be siblings placed over the control.
		for ( size_t I = 0; I < m_vTabs.size(); ++I ) {
			if ( m_vTabs[I].pwWidget && ::GetParent( m_vTabs[I].pwWidget->Wnd() ) != Wnd() ) { fExclude( m_vTabs[I].pwWidget->Wnd() ); }
		}
	}

	// The custom tab message handler.
	LRESULT CALLBACK CTab::CustomTabProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CTab * pmwThis = static_cast<CTab *>(LSW_WIN2CLASS( _hWnd ));
		WNDPROC pOld = pmwThis->OriginalProc();
		switch ( _uMsg ) {
			case WM_NCHITTEST : {
				if ( pmwThis->m_bShowClose || pmwThis->AnyAreCheckable() ) {
					POINTS pPos = MAKEPOINTS( _lParam );
					LRESULT lrHit = ::CallWindowProcW( reinterpret_cast<WNDPROC>(pOld), _hWnd, _uMsg, _wParam, _lParam );
					TCHITTESTINFO tTest;
					tTest.pt.x = pPos.x;
					tTest.pt.y = pPos.y;
					::ScreenToClient( _hWnd, &tTest.pt );
					INT iHit = pmwThis->HitTest( &tTest );
					if ( iHit == -1 ) {
						return lrHit;
					}
					LSW_RECT rItem = pmwThis->GetItemRect( iHit );
					LSW_RECT rClose;
					rClose = GetCloseRect( rItem, iHit == pmwThis->GetCurSel() );
			
					if ( rClose.PtInRect( tTest.pt ) ) {
						if ( pmwThis->m_bShowClose ) {
							lrHit = HTCLOSE;
						}
						else if ( pmwThis->m_vTabs[iHit].bCheckable ) {
							lrHit = HTOBJECT;
						}
					}
					return lrHit;
				}
				break;
			}
			case WM_NCLBUTTONDOWN : {}
			case WM_NCLBUTTONDBLCLK : {
				// The close box and check box act on button-up.  The default handling of HTCLOSE tracks a caption button and redraws.
				if ( _wParam == HTCLOSE || _wParam == HTOBJECT ) { return 0; }
				break;
			}
			case WM_NCLBUTTONUP : {
				if ( _wParam == HTCLOSE ) {
					LSW_NMTABCLOSE hCloser;
					INT iID = pmwThis->Id();
					POINTS pPos = MAKEPOINTS( _lParam );
					TCHITTESTINFO tTest;
					tTest.pt.x = pPos.x;
					tTest.pt.y = pPos.y;
					::ScreenToClient( pmwThis->Wnd(), &tTest.pt );
					hCloser.hdr.code		= static_cast<UINT>(LSW_TAB_NM_CLOSE);
					hCloser.hdr.hwndFrom	= pmwThis->Wnd();
					hCloser.hdr.idFrom		= iID;
					hCloser.iTab			= pmwThis->HitTest( &tTest );
					hCloser.pwWidget		= pmwThis;

					::SendMessageW( pmwThis->Parent()->Wnd(), WM_NOTIFY, iID, reinterpret_cast<LPARAM>(&hCloser) );
					return 0;
				}
				if ( _wParam == HTOBJECT ) {
					LSW_NMTABCLOSE hCloser;
					INT iID = pmwThis->Id();
					POINTS pPos = MAKEPOINTS( _lParam );
					TCHITTESTINFO tTest;
					tTest.pt.x = pPos.x;
					tTest.pt.y = pPos.y;
					::ScreenToClient( pmwThis->Wnd(), &tTest.pt );
					hCloser.hdr.code		= static_cast<UINT>(LSW_TAB_NM_CHECK);
					hCloser.hdr.hwndFrom	= pmwThis->Wnd();
					hCloser.hdr.idFrom		= iID;
					hCloser.iTab			= pmwThis->HitTest( &tTest );
					hCloser.pwWidget		= pmwThis;

					::SendMessageW( pmwThis->Parent()->Wnd(), WM_NOTIFY, iID, reinterpret_cast<LPARAM>(&hCloser) );
				}
				break;
			}
			case WM_PAINT : {
				// Everything is drawn into a buffer and copied to the screen once, without drawing over the pages, so nothing flickers.
				LSW_BEGINPAINT bpPaint( _hWnd );
				if ( !bpPaint.hDc ) { return 0; }
				RECT rClient = {};
				::GetClientRect( _hWnd, &rClient );
				const int iW = rClient.right - rClient.left, iH = rClient.bottom - rClient.top;
				if ( iW <= 0 || iH <= 0 ) { return 0; }

				HDC hMem = ::CreateCompatibleDC( bpPaint.hDc );
				HBITMAP hBmp = hMem ? ::CreateCompatibleBitmap( bpPaint.hDc, iW, iH ) : NULL;
				if ( !hBmp ) {
					// No buffer: draw directly.
					if ( hMem ) { ::DeleteDC( hMem ); }
					pmwThis->ExcludeCovered( bpPaint.hDc );
					pmwThis->PaintTabs( bpPaint.hDc, rClient );
					return 0;
				}
				{
					LSW_SELECTOBJECT soBmp( hMem, hBmp );
					pmwThis->PaintTabs( hMem, rClient );
					pmwThis->ExcludeCovered( bpPaint.hDc );
					::BitBlt( bpPaint.hDc, 0, 0, iW, iH, hMem, 0, 0, SRCCOPY );
				}
				::DeleteObject( hBmp );
				::DeleteDC( hMem );
				// ::EndPaint() called by the destructor of bpPaint.
				return 0;
			}
			case WM_ERASEBKGND : {
				// WM_PAINT draws everything.  Other windows' DCs (a child asking for its parent's background) are left to the control.
				if ( ::WindowFromDC( reinterpret_cast<HDC>(_wParam) ) == _hWnd ) { return 1; }
				break;
			}
			case WM_SIZE : {}
			case WM_SETFONT : {
				LRESULT lrRet = ::CallWindowProcW( reinterpret_cast<WNDPROC>(pOld), _hWnd, _uMsg, _wParam, _lParam );
				pmwThis->UpdateLayout();
				return lrRet;
			}
			case TCM_HITTEST : {
				// Tabs are where this class puts them.
				LPTCHITTESTINFO phtiInfo = reinterpret_cast<LPTCHITTESTINFO>(_lParam);
				if ( !phtiInfo ) { return -1; }
				const INT iTab = pmwThis->TabFromPoint( phtiInfo->pt );
				phtiInfo->flags = (iTab >= 0) ? TCHT_ONITEM : TCHT_NOWHERE;
				return iTab;
			}
			case TCM_GETITEMRECT : {
				const INT iTab = static_cast<INT>(_wParam);
				if ( _lParam && iTab >= 0 && iTab < static_cast<INT>(pmwThis->m_vTabs.size()) ) {
					(*reinterpret_cast<RECT *>(_lParam)) = pmwThis->m_vTabs[iTab].rRect;
					return TRUE;
				}
				break;
			}
			case WM_LBUTTONDOWN : {}
			case WM_LBUTTONUP : {}
			case WM_LBUTTONDBLCLK : {}
			case WM_RBUTTONDOWN : {}
			case WM_RBUTTONUP : {}
			case WM_RBUTTONDBLCLK : {}
			case WM_MBUTTONDOWN : {}
			case WM_MBUTTONUP : {
				// The native control finds the tab under the mouse in its own layout.
				return ::CallWindowProcW( reinterpret_cast<WNDPROC>(pOld), _hWnd, _uMsg, _wParam, pmwThis->ToNativePoint( _lParam ) );
			}
			case WM_MOUSEACTIVATE : {
				if ( LOWORD( _lParam ) == HTCLOSE && HIWORD( _lParam ) == WM_LBUTTONDOWN ) { return MA_ACTIVATEANDEAT; }
				break;
			}
			case WM_ACTIVATE : {
				break;
			}
			case WM_MOUSEMOVE : {
				pmwThis->BeginMouseLeaveTracking();

				const int iX = GET_X_LPARAM( _lParam );
				const int iY = GET_Y_LPARAM( _lParam );
				pmwThis->SetHotTab( pmwThis->HitTestTab( iX, iY ) );
				return ::CallWindowProcW( reinterpret_cast<WNDPROC>(pOld), _hWnd, _uMsg, _wParam, pmwThis->ToNativePoint( _lParam ) );
			}
			case WM_MOUSELEAVE : {
				pmwThis->m_bTrackMouseLeave = false;
				// Moving onto a close box or check box (non-client) also "leaves", but the tab stays hot (see WM_NCMOUSEMOVE).
				pmwThis->SetHotTab( -1 );
				break;
			}
			case WM_NCMOUSEMOVE : {
				// Over a close box or check box.
				if ( !pmwThis->m_bTrackNcLeave ) {
					pmwThis->m_bTrackNcLeave = true;
					TRACKMOUSEEVENT tmeTrackMe{};
					tmeTrackMe.cbSize = sizeof( tmeTrackMe );
					tmeTrackMe.dwFlags = TME_LEAVE | TME_NONCLIENT;
					tmeTrackMe.hwndTrack = _hWnd;
					::TrackMouseEvent( &tmeTrackMe );
				}
				POINT pPos = { GET_X_LPARAM( _lParam ), GET_Y_LPARAM( _lParam ) };
				::ScreenToClient( _hWnd, &pPos );
				pmwThis->SetHotTab( pmwThis->TabFromPoint( pPos ) );
				break;
			}
			case WM_NCMOUSELEAVE : {
				pmwThis->m_bTrackNcLeave = false;
				// If the mouse went back over the tab, WM_MOUSEMOVE makes it hot again before anything is redrawn.
				pmwThis->SetHotTab( -1 );
				break;
			}
			/*case WM_MOUSEWHEEL : {
				POINTS pPos = {
					GET_X_LPARAM( _lParam ),
					GET_Y_LPARAM( _lParam ),
				};
				for ( size_t I = 0; I < pmwThis->m_vChildren.size(); ++I ) {
					LSW_HANDLED hHandled = pmwThis->m_vChildren[I]->MouseWheel( static_cast<DWORD>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { return 0; }
					break;
				}
				break;
			}*/
			case WM_NOTIFY : {
				LPNMHDR lpHdr = reinterpret_cast<LPNMHDR>(_lParam);
				switch ( lpHdr->code ) {
					case NM_CUSTOMDRAW : {
						return CWidget::WindowProc( _hWnd, _uMsg, _wParam, _lParam );
					}
				}
				//LVN_ODSTATECHANGED
				break;
			}
		}

		return ::CallWindowProcW( reinterpret_cast<WNDPROC>(pOld), _hWnd, _uMsg, _wParam, _lParam );
	}

}	// namespace lsw

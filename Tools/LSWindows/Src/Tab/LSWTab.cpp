#include "LSWTab.h"
#include "../Base/LSWBase.h"
#include "../Helpers/LSWHelpers.h"
#include <codecvt>
#include <windowsx.h>

#define LSW_CLOSE_SPACING	21

namespace lsw {

	CTab::CTab( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_lpOriginProc( 0 ),
		m_bShowClose( true ) {
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
	// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
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
			
			int iLen = GetLongestTextWidth();
			SetItemSize( iLen + LSW_CLOSE_SPACING, 0 );
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
			int iLen = GetLongestTextWidth();
			SetItemSize( iLen + (m_bShowClose ? LSW_CLOSE_SPACING : 0), 0 );
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
			int iLen = GetLongestTextWidth();
			SetItemSize( iLen + (m_bShowClose ? LSW_CLOSE_SPACING : 0), 0 );

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
		int iRet = static_cast<int>(::SendMessageW( Wnd(), TCM_SETCURSEL, static_cast<WPARAM>(_iItem), 0L ));
		// Do not check iRet for -1.
		for ( size_t I = 0; I < m_vTabs.size(); ++I ) {
			if ( m_vTabs[I].pwWidget ) {
				m_vTabs[I].pwWidget->SetVisible( I == static_cast<size_t>(_iItem) );
			}
		}
		return iRet;
	}

	// Determines the currently selected tab in a tab control.
	int CTab::GetCurSel() const {
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

	// Gets the width of the text in a given tab.
	int CTab::GetTabTextWidth( INT _iTab ) const {
		if ( !Wnd() ) { return 0; }
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

	// Setting the HWND after the control has been created.
	void CTab::InitControl( HWND _hWnd ) {
		CWidget::InitControl( _hWnd );
		m_lpOriginProc = ::GetWindowLongPtrW( Wnd(), GWLP_WNDPROC );
		::SetWindowLongPtrW( _hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(CustomTabProc) );

		DWORD dwStyle = static_cast<DWORD>(::GetWindowLongPtrW( Wnd(), GWL_STYLE ));
		::SetWindowLongPtrW( Wnd(), GWL_STYLE, dwStyle | TCS_FIXEDWIDTH | TCS_FORCELABELLEFT | TCS_SINGLELINE );
	}

	// == Functions.
	// WM_SIZE.
	CWidget::LSW_HANDLED CTab::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		CWidget::Size( _wParam, _lWidth, _lHeight );
		//::ShowScrollBar( Wnd(), SB_VERT, FALSE );
		//ResizeControls( VirtualClientRect( nullptr ) );

		/*for ( auto I = m_vTabs.size(); I--; ) {
			m_vTabs[I].pwWidget->EvalNewSize();
		}*/

		return LSW_H_CONTINUE;
	}

	// WM_MOVE.
	CWidget::LSW_HANDLED CTab::Move( LONG _lX, LONG _lY ) {
		CWidget::Move( _lX, _lY );
		//ResizeControls( VirtualClientRect( nullptr ) );
		/*for ( auto I = m_vTabs.size(); I--; ) {
			m_vTabs[I].pwWidget->Move( _lX, _lY );
		}*/

		return LSW_H_CONTINUE;
	}

	// Determines the close rectangle.
	LSW_RECT CTab::GetCloseRect( const LSW_RECT &_rTabRect, bool _bHasFocus ) {
		LSW_RECT rReturn;
		INT iHeight = _rTabRect.Height();
		INT iXW = ::GetSystemMetrics( SM_CXSMSIZE ) - 1;
		INT iXH = ::GetSystemMetrics( SM_CYSMSIZE ) - 1;
		rReturn.left = _rTabRect.right - iXW - (_bHasFocus ? 2 : ::GetSystemMetrics( SM_CXFRAME ));
		rReturn.right = rReturn.left + iXW;

		rReturn.top = _rTabRect.top + ((iHeight - iXH) >> 1);
		rReturn.bottom = rReturn.top + iXH;
		return rReturn;
	}

	// The custom tab message handler.
	LRESULT CALLBACK CTab::CustomTabProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CTab * pmwThis = static_cast<CTab *>(LSW_WIN2CLASS( _hWnd ));
		WNDPROC pOld = pmwThis->OriginalProc();
		switch ( _uMsg ) {
			case WM_NCHITTEST : {
				if ( pmwThis->m_bShowClose ) {
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
						lrHit = HTCLOSE;
					}
					return lrHit;
				}
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
				break;
			}
			case WM_PAINT : {
				if ( pmwThis->m_bShowClose ) {
					/*LONG_PTR hObj = */::GetWindowLongPtrW( _hWnd, 0 );
					LSW_BEGINPAINT bpPaint( _hWnd );

					{
						//LSW_SELECTOBJECT soPrev( bpPaint.hDc, reinterpret_cast<HGDIOBJ>(hObjSet) );

						INT iTotal = pmwThis->GetItemCount();
						INT iSel = pmwThis->GetCurSel();

						::CallWindowProcW( pOld, _hWnd, _uMsg, reinterpret_cast<WPARAM>(bpPaint.hDc), 0L );

						for ( INT I = 0; I < iTotal; I++ ) {
							LSW_RECT rItem, rClose;
							rItem = pmwThis->GetItemRect( I );
							rClose = GetCloseRect( rItem, I == iSel );
							::DrawFrameControl( bpPaint.hDc, &rClose, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_FLAT );
						}
					}

					// ::EndPaint() called by the destructor of bpPaint.
					return 0;
				}
				break;
			}
			case WM_MOUSEACTIVATE : {
				if ( LOWORD( _lParam ) == HTCLOSE && HIWORD( _lParam ) == WM_LBUTTONDOWN ) { return MA_ACTIVATEANDEAT; }
				break;
			}
			case WM_ACTIVATE : {
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

#include "LSWTreeListView.h"
#include "../Base/LSWBase.h"
#include <strsafe.h>
#include <windowsx.h>

namespace lsw {

	// == Members.
	// Window property.
	WCHAR CTreeListView::m_szProp[2] = { 0 };

	CTreeListView::CTreeListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CListView( _wlLayout.ChangeClass( WC_LISTVIEWW ).AddStyleEx( LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER ).AddStyle( LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT ),
			_pwParent, _bCreateWidget, _hMenu ),
		m_hListView( NULL ),
		m_wpListViewProc( nullptr ) {

		if ( !m_szProp[0] ) {
			m_szProp[0] = L'A' + ((reinterpret_cast<UINT_PTR>(_pwParent) >> 2) & 0x0F);
		}
	}
	CTreeListView::~CTreeListView() {
		::DestroyWindow( m_hListView );
	}

	// == Functions.
#if 0
	// WM_SIZE.
	CWidget::LSW_HANDLED CTreeListView::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		//CWidget::Size( _wParam, _lWidth, _lHeight );
		/*LSW_RECT rRect = VirtualClientRect( nullptr );
		//::MoveWindow( Wnd(), rRect.left, rRect.top, rRect.Width(), rRect.Height(), TRUE );
		
		::MoveWindow( m_hListView, ee::Time() % 10, ee::Time() % 20, rRect.Width(), rRect.Height(), TRUE );
		::RedrawWindow( m_hListView, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME );
		::RedrawWindow( Wnd(), NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME );*/
		//::SetWindowPos( m_hListView, NULL, 0, 0, _lWidth, _lHeight, SWP_NOOWNERZORDER | SWP_NOZORDER );
		/*char szBuffer[64];
		std::sprintf( szBuffer, "%d %d\r\n", rRect.Width(), rRect.Height() );
		::OutputDebugStringA( szBuffer );*/
		return LSW_H_CONTINUE;
	}

	// WM_MOVE.
	CWidget::LSW_HANDLED CTreeListView::Move( LONG _lX, LONG _lY ) {
		//CWidget::Move( _lX, _lY );
		//ResizeControls( VirtualClientRect( nullptr ) );
		/*LSW_RECT rRect = VirtualClientRect( nullptr );
		//::MoveWindow( Wnd(), rRect.left, rRect.top, rRect.Width(), rRect.Height(), TRUE );
		
		::MoveWindow( m_hListView, ee::Time() % 10, ee::Time() % 20, rRect.Width(), rRect.Height(), TRUE );
		::RedrawWindow( m_hListView, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME );*/

		return LSW_H_CONTINUE;
	}
#endif

	// Setting the HWND after the control has been created.
	void CTreeListView::InitControl( HWND _hWnd ) {
		CListView::InitControl( _hWnd );
		m_wpListViewProc = CHelpers::SetWndProc( Wnd(), ListViewOverride );
		::SetPropW( Wnd(), m_szProp, reinterpret_cast<HANDLE>(this) );

		// By default there is already a header.
		InsertColumn( L"", 150, 0 );
		/*LV_COLUMNW lvColumn;
		lvColumn.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
		lvColumn.pszText = const_cast<LPWSTR>(L"");
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 150;
		INT iInserted = static_cast<INT>(::SendMessageW( Wnd(), LVM_INSERTCOLUMNW, static_cast<WPARAM>(0), reinterpret_cast<LPARAM>(&lvColumn)));*/
		//::SendMessageW( Wnd(), LVM_SETCOLUMNWIDTH, static_cast<WPARAM>(0), MAKELPARAM( (150), 0 ) );
#if 0
		if ( !m_hListView ) {
			m_hListView = ::CreateWindowExW( WS_EX_CLIENTEDGE,
				WC_LISTVIEWW,
				L"",
				WS_VISIBLE | WS_CLIPSIBLINGS | WS_CHILD | LVS_REPORT | LVS_SINGLESEL/* | WS_VSCROLL | WS_HSCROLL*/,
				150, 20,
				350, 122,
				Wnd(), NULL,
				CBase::GetThisHandle(),
				NULL );
			//::SetWindowPos( m_hListView, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

			LV_COLUMNW lvColumn;
			lvColumn.mask = LVCF_TEXT | LVCF_FMT;
			lvColumn.pszText = const_cast<LPWSTR>(L"Header");
			lvColumn.fmt = LVCFMT_LEFT;
			INT iInserted = static_cast<INT>(::SendMessageW( m_hListView, LVM_INSERTCOLUMNW, static_cast<WPARAM>(0), reinterpret_cast<LPARAM>(&lvColumn)));
			::SendMessageW( m_hListView, LVM_SETCOLUMNWIDTH, static_cast<WPARAM>(0), MAKELPARAM( (150), 0 ) );
			//ListView_SetColumnWidth( m_hListView, _iCol, _iWidth );

			/*HWND hHeader = ListView_GetHeader( m_hListView );
			HGDIOBJ hFont = reinterpret_cast<HGDIOBJ>(::SendMessageW( hHeader, WM_GETFONT, 0L, 0L ));
			LSW_RECT rRect;
			::GetClientRect( hHeader, &rRect );*/

			

			// The list view has to stay in memory because we are using its font.
			m_wpListViewProc = CHelpers::SetWndProc( m_hListView, ListViewOverride );
			::SetPropW( m_hListView, m_szProp, reinterpret_cast<HANDLE>(this) );
			//::ShowWindow( m_hListView, SW_HIDE );

			::ShowScrollBar( Wnd(), SB_VERT, FALSE );
			::ShowScrollBar( Wnd(), SB_HORZ, FALSE );
		}
#endif
	}

	// Inserts an item.
	HTREEITEM CTreeListView::InsertItem( const TVINSERTSTRUCTW * _pisStruct ) {
		return nullptr;
	}

	// Sets the text for an item.
	BOOL CTreeListView::SetItemText( HTREEITEM _tiItem, const WCHAR * _pwcText, INT _iColumn ) {
		return FALSE;
	}

	// Gets the text of an item.
	BOOL CTreeListView::GetItemText( HTREEITEM _tiItem, INT _iColumn, std::wstring &_sRet ) const {
		return FALSE;
	}

	// Sets the lParam of an item.
	BOOL CTreeListView::SetItemLParam( HTREEITEM _tiItem, LPARAM _lpParam ) {
		return FALSE;
	}

	// Gets the lParam of an item.
	LPARAM CTreeListView::GetItemLParam( HTREEITEM _tiItem ) const {
		return 0;
	}

	// Returns true if any of the selected items have children and are not in expanded view.
	bool CTreeListView::AnySelectedHasUnexpandedChildren() const {
		return false;
	}

	// Returns true if any of the selected items have children and are in expanded view.
	bool CTreeListView::AnySelectedHasExpandedChildren() const {
		return false;
	}

	// Returns true if any of the items have children and are not in expanded view.
	bool CTreeListView::AnyHasUnexpandedChildren() const {
		return false;
	}

	// Returns true if any of the items have children and are in expanded view.
	bool CTreeListView::AnyHasExpandedChildren() const {
		return false;
	}

	// Expands selected items.
	void CTreeListView::ExpandSelected() const {
	}

	// Expands all items.
	void CTreeListView::ExpandAll() const {
	}

	// Collapses selected items.
	void CTreeListView::CollapseSelected() const {
	}

	// Collapses all items.
	void CTreeListView::CollapseAll() const {
	}

	// Gathers the selected items into a vector.
	size_t CTreeListView::GatherSelected( std::vector<HTREEITEM> &_vReturn, bool _bIncludeNonVisible ) const {
		return 0;
	}

	// A helper to easily create a tree view item to be inserted with only text.
	TVINSERTSTRUCTW CTreeListView::DefaultItem( const WCHAR * _pwcText, HTREEITEM _tiParent, HTREEITEM _tiInsertAfter ) {
		TVINSERTSTRUCTW isItem = { 0 };
		isItem.hParent = _tiParent;
		isItem.hInsertAfter = _tiInsertAfter;
		isItem.item.mask = TVIF_TEXT;
		isItem.item.pszText = const_cast<LPWSTR>(_pwcText);
		isItem.item.cchTextMax = static_cast<int>(std::wcslen( _pwcText ));
		return isItem;
	}

	// A helper to easily create a tree view item to be inserted with only text and data.
	TVINSERTSTRUCTW CTreeListView::DefaultItemLParam( const WCHAR * _pwcText, LPARAM _lpParm, HTREEITEM _tiParent, HTREEITEM _tiInsertAfter ) {
		TVINSERTSTRUCTW isItem = { 0 };
		isItem.hParent = _tiParent;
		isItem.hInsertAfter = _tiInsertAfter;
		isItem.item.mask = TVIF_TEXT | TVIF_PARAM;
		isItem.item.pszText = const_cast<LPWSTR>(_pwcText);
		isItem.item.cchTextMax = static_cast<int>(std::wcslen( _pwcText ));
		isItem.item.lParam = _lpParm;
		return isItem;
	}


	// List-view window procedure.  The list-view is hidden.
	LRESULT CALLBACK CTreeListView::ListViewOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CTreeListView * ptlThis = reinterpret_cast<CTreeListView *>(::GetPropW( _hWnd, m_szProp ));
		WNDPROC wpOrig = ptlThis ? ptlThis->m_wpListViewProc : NULL;

		switch ( _uMsg ) {
			case WM_NCDESTROY : {
				::RemovePropW( _hWnd, m_szProp );
				break;
			}
			case WM_LBUTTONDBLCLK : {
				volatile int ghjg = 0;
				//LSW_RECT rRect = ptlThis->VirtualClientRect( nullptr );
				//::MoveWindow( _hWnd, rRect.left, rRect.top, rRect.Width(), rRect.Height(), TRUE );
				break;
			}
			case WM_NCLBUTTONDBLCLK : {
				volatile int ghjg = 0;
				//::MoveWindow( _hWnd, ee::Time() % 10, ee::Time() % 20, ee::Time() % 10 + 350, ee::Time() % 10 + 250, TRUE );
				break;
			}
		}
		if ( wpOrig ) {
			return ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
		}
		else {
			return 0;
		}
	}

}	// namespace lsw

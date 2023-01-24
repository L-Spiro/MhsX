#include "LSWTreeList.h"
#include "../Base/LSWBase.h"
#include <strsafe.h>
#include <windowsx.h>

namespace lsw {

	// == Members.
	// Window property.
	WCHAR CTreeList::m_szProp[2] = { 0 };

	CTreeList::CTreeList( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CWidget( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(CBase::TreeListAtom()) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_hListView( NULL ),
		m_hTreeView( NULL ),
		m_hHeader( NULL ),
		m_iHScroll( 0 ),
		m_iVScroll( 0 ),
		m_wpListViewProc( nullptr ),
		m_wpTreeViewProc( nullptr ),
		m_wpTreeListProc( nullptr ),
		m_wpHeaderProc( nullptr ),
		m_stLastSel( LSW_TREELIST_INVALID ),
		m_bLocked( false ) {

		if ( !m_szProp[0] ) {
			m_szProp[0] = L'A' + ((reinterpret_cast<UINT_PTR>(_pwParent) >> 2) & 0x0F);
		}
	}
	CTreeList::~CTreeList() {
		::DestroyWindow( m_hListView );
		::DestroyWindow( m_hTreeView );
		::DestroyWindow( m_hHeader );
	}

	// == Functions.
	// Gets the content width.
	INT CTreeList::ContentWidth() const {
		HDITEM hdItem;
		hdItem.mask = HDI_WIDTH;
		int iTotal = GetColumnCount();
		INT iWidth = 0;
		for ( INT I = 0; I < iTotal; I++ ) {
			Header_GetItem( m_hHeader, I, &hdItem );
			iWidth += hdItem.cxy;
		}
		return iWidth;
	}

	// Gets the content height.
	INT CTreeList::ContentHeight() const {
		int iItemHeight = TreeView_GetItemHeight( m_hTreeView );
		return static_cast<INT>(GetTreeHeight( TreeView_GetRoot( m_hTreeView ) ) * iItemHeight + m_uiHeaderHeight);
	}

	// Inserts a column.
	BOOL CTreeList::InsertColumn( const WCHAR * _pwcText, INT _iWidth, INT _iIdx ) {
		if ( _iIdx < 0 ) { _iIdx = GetColumnCount(); }
		HDITEMW hdItem;
		hdItem.mask = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
		hdItem.pszText = const_cast<LPWSTR>(_pwcText);
		hdItem.cchTextMax = static_cast<int>(std::wcslen( _pwcText ));
		hdItem.cxy = _iWidth;
		hdItem.fmt = HDF_LEFT | HDF_STRING;
		INT iIdx = static_cast<INT>(::SendMessageW( m_hHeader, HDM_INSERTITEMW, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(&hdItem) ));
		if ( iIdx == -1 ) { return FALSE; }
		//Header_InsertItem
		//(int)SNDMSG((hwndHD), HDM_INSERTITEMW, (WPARAM)(int)(i), (LPARAM)(const HD_ITEM *)(phdi))
		//TVINSERTSTRUCTW isInsertMe = { 0 };
		for ( size_t I = m_vRows.size(); I--; ) {
			m_vRows[I].vStrings.insert( m_vRows[I].vStrings.begin() + _iIdx, std::wstring() );
		}

		return TRUE;
	}

	// Sets the text of a column.
	BOOL CTreeList::SetColumnText( const WCHAR * _pwcText, INT _iIdx ) {
		if ( _iIdx >= 0 && _iIdx < GetColumnCount() ) {
			HDITEMW hdItem;
			hdItem.mask = HDI_TEXT;
			hdItem.pszText = const_cast<LPWSTR>(_pwcText);
			hdItem.cchTextMax = static_cast<int>(std::wcslen( _pwcText ));
			return static_cast<BOOL>(::SendMessageW( m_hHeader, HDM_SETITEMW, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(&hdItem) ));
		}
		return FALSE;
	}

	// Inserts an item.
	HTREEITEM CTreeList::InsertItem( const TVINSERTSTRUCTW * _pisStruct ) {
		size_t sIdx = m_vRows.size();
		TVINSERTSTRUCTW isCopy;
		try {
			m_vRows.push_back( LSW_TREE_ROW() );
			isCopy = (*_pisStruct);
			isCopy.item.pszText = LPSTR_TEXTCALLBACKW;
			//isCopy.item.mask = isCopy.item.mask & ~TVIF_TEXT;
		}
		catch ( const std::bad_alloc & /*_eE*/ ) { return NULL; }

		isCopy.item.lParam = sIdx;
		isCopy.item.mask |= TVIF_PARAM;
		HTREEITEM hItem = TreeView_InsertItem( m_hTreeView, &isCopy );
		if ( !hItem ) {
			m_vRows.pop_back();
		}
		else {
			// Add 1 string for each column.
			int iColumns = GetColumnCount();
			m_vRows[sIdx].lpParam = _pisStruct->item.lParam;
			m_vRows[sIdx].uiStateEx = 0;
			m_vRows[sIdx].htiItem = hItem;
			try {
				for ( int I = 0; I < iColumns; ++I ) {
					m_vRows[sIdx].vStrings.insert( m_vRows[sIdx].vStrings.begin(), std::wstring() );
				}
				if ( iColumns && _pisStruct->item.pszText ) {
					m_vRows[sIdx].vStrings[0] = _pisStruct->item.pszText;
				}
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {
				m_vRows.pop_back();
				TreeView_DeleteItem( m_hTreeView, hItem );
				return NULL;
			}
		}
		return hItem;
	}

	// Sets the text for an item.
	BOOL CTreeList::SetItemText( HTREEITEM _tiItem, const WCHAR * _pwcText, INT _iColumn ) {
		if ( !_tiItem ) { return FALSE; }
		TVITEMW iItem = { 0 };
		iItem.mask = TVIF_HANDLE | TVIF_PARAM;
		iItem.hItem = _tiItem;
		if ( !TreeView_GetItem( m_hTreeView, &iItem ) ) { return FALSE; }
		if ( static_cast<size_t>(iItem.lParam) >= m_vRows.size() || iItem.lParam < 0 ) { return FALSE; }
		if ( static_cast<size_t>(_iColumn) >= m_vRows[iItem.lParam].vStrings.size() || _iColumn < 0 ) { return FALSE; }
		m_vRows[iItem.lParam].vStrings[_iColumn] = _pwcText;
		return TRUE;
	}

	// Gets the text of an item.
	BOOL CTreeList::GetItemText( HTREEITEM _tiItem, INT _iColumn, std::wstring &_sRet ) const {
		TVITEMW iItem = { 0 };
		iItem.mask = TVIF_HANDLE | TVIF_PARAM;
		iItem.hItem = _tiItem;
		if ( !TreeView_GetItem( m_hTreeView, &iItem ) ) { return FALSE; }
		if ( static_cast<size_t>(iItem.lParam) >= m_vRows.size() || iItem.lParam < 0 ) { return FALSE; }
		if ( static_cast<size_t>(_iColumn) >= m_vRows[iItem.lParam].vStrings.size() || _iColumn < 0 ) { return FALSE; }
		_sRet = m_vRows[iItem.lParam].vStrings[_iColumn];
		return TRUE;
	}

	// Sets the lParam of an item.
	BOOL CTreeList::SetItemLParam( HTREEITEM _tiItem, LPARAM _lpParam ) {
		TVITEMW iItem = { 0 };
		iItem.mask = TVIF_HANDLE | TVIF_PARAM;
		iItem.hItem = _tiItem;
		if ( !TreeView_GetItem( m_hTreeView, &iItem ) ) { return FALSE; }
		if ( static_cast<size_t>(iItem.lParam) >= m_vRows.size() || iItem.lParam < 0 ) { return FALSE; }
		m_vRows[iItem.lParam].lpParam = _lpParam;
		return TRUE;
	}

	// Gets the lParam of an item.
	LPARAM CTreeList::GetItemLParam( HTREEITEM _tiItem ) const {
		TVITEMW iItem = { 0 };
		iItem.mask = TVIF_HANDLE | TVIF_PARAM;
		iItem.hItem = _tiItem;
		if ( !TreeView_GetItem( m_hTreeView, &iItem ) ) { return FALSE; }
		if ( static_cast<size_t>(iItem.lParam) >= m_vRows.size() || iItem.lParam < 0 ) { return FALSE; }
		return m_vRows[iItem.lParam].lpParam;
	}

	// Sets the width of a column.
	BOOL CTreeList::SetColumnWidth( INT _iIdx, INT _iWidth ) {
		HDITEMW hdItem;
		hdItem.mask = HDI_WIDTH;
		hdItem.cxy = _iWidth;
		return static_cast<BOOL>(::SendMessageW( m_hHeader, HDM_SETITEMW, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(reinterpret_cast<const HD_ITEMW *>(&hdItem)) ));
	}

	// Gets the width of a column or -1.
	INT CTreeList::GetColumnWidth( INT _iIdx ) {
		HDITEMW hdItem;
		hdItem.mask = HDI_WIDTH;
		if ( !::SendMessageW( m_hHeader, HDM_GETITEMW, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(reinterpret_cast<const HD_ITEMW *>(&hdItem)) ) ) { return -1; }
		return hdItem.cxy;
	}

	// Setting the HWND after the control has been created.
	void CTreeList::InitControl( HWND _hWnd ) {
		CWidget::InitControl( _hWnd );

		if ( !m_hListView ) {
			m_hListView = ::CreateWindowExW( WS_EX_CLIENTEDGE,
				WC_LISTVIEWW,
				L"",
				WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL/* | WS_VSCROLL | WS_HSCROLL*/,
				150, 0,
				50, 122,
				Wnd(), NULL,
				CBase::GetThisHandle(),
				NULL );

			HWND hHeader = ListView_GetHeader( m_hListView );
			HGDIOBJ hFont = reinterpret_cast<HGDIOBJ>(::SendMessageW( hHeader, WM_GETFONT, 0L, 0L ));
			LSW_RECT rRect;
			::GetClientRect( hHeader, &rRect );

			if ( !m_hHeader ) {
				m_hHeader = ::CreateWindowExW( 0,
					WC_HEADERW,
					L"",
					WS_VISIBLE | WS_CHILDWINDOW | HDS_FULLDRAG | HDS_HOTTRACK | HDS_HORZ,
					0, 0,
					450, rRect.bottom,
					Wnd(), NULL,
					CBase::GetThisHandle(),
					NULL );
				::SendMessageW( m_hHeader, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), FALSE );
				/*HGDIOBJ hNewFont = */reinterpret_cast<HGDIOBJ>(::SendMessageW( m_hHeader, WM_GETFONT, 0L, 0L ));
				LSW_RECT rHeaderRect;
				::GetClientRect( m_hHeader, &rHeaderRect );
				m_uiHeaderHeight = rHeaderRect.Height();


				// The header always has at least one column.
				HDITEMW hdItem;
				hdItem.mask = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
				hdItem.cxy = 160;
				hdItem.fmt = HDF_LEFT;
				hdItem.pszText = const_cast<LPWSTR>(L"");
				::SendMessageW( m_hHeader, HDM_INSERTITEMW, 0L, reinterpret_cast<LPARAM>(reinterpret_cast<const HD_ITEMW *>(&hdItem)) );

				if ( !m_hTreeView ) {
/*#if (NTDDI_VERSION >= NTDDI_VISTA)
					DWORD dwExStyle = TVS_EX_MULTISELECT;
#else
					DWORD dwExStyle = 0;
#endif*/
					m_hTreeView = ::CreateWindowExW( 0/*dwExStyle*/,
					WC_TREEVIEWW,
					L"",
					WS_VISIBLE | WS_CHILD | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_NOHSCROLL | /*TVS_NOSCROLL | *//*TVS_HASLINES | */TVS_LINESATROOT |
						/*WS_VSCROLL | WS_HSCROLL | */TVS_FULLROWSELECT | TVS_DISABLEDRAGDROP,
					250, 150,
					150, 450,
					Wnd(), NULL,
					CBase::GetThisHandle(),
					NULL );

					// The list view has to stay in memory because we are using its font.
					m_wpListViewProc = CHelpers::SetWndProc( m_hListView, ListViewOverride );
					::SetPropW( m_hListView, m_szProp, reinterpret_cast<HANDLE>(this) );
					::ShowWindow( m_hListView, SW_HIDE );

					m_wpTreeViewProc = CHelpers::SetWndProc( m_hTreeView, TreeViewOverride );
					::SetPropW( m_hTreeView, m_szProp, reinterpret_cast<HANDLE>(this) );

					m_wpTreeListProc = CHelpers::SetWndProc( Wnd(), TreeListOverride );
					::SetPropW( Wnd(), m_szProp, reinterpret_cast<HANDLE>(this) );

					m_wpHeaderProc = reinterpret_cast<WNDPROC>(::GetWindowLongPtrW( m_hHeader, GWLP_WNDPROC ));
					::SetWindowPos( m_hHeader, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

					::ShowScrollBar( m_hTreeView, SB_HORZ, FALSE );
					::ShowScrollBar( Wnd(), SB_VERT, FALSE );
					//::ShowScrollBar( Wnd(), SB_BOTH, TRUE );

					//::ShowScrollBar( m_hTreeView, SB_BOTH, FALSE );
					//::EnableScrollBar( m_hTreeView, SB_BOTH, ESB_DISABLE_BOTH );
					/*::ShowScrollBar( m_hTreeView, SB_BOTH, TRUE );
					::EnableScrollBar( m_hTreeView, SB_BOTH, ESB_ENABLE_BOTH );*/
	
					//::ShowWindow( m_hListView, SW_HIDE );
					//::DestroyWindow( m_hListView );
				}
			}
		}
	}

	// Gets the maximum width in pixels of a column.
	INT CTreeList::GetColumnTextWidth( INT _iIdx ) const {
		if ( _iIdx >= GetColumnCount() ) { return -1; }
		if ( !m_hTreeView ) { return -1; }
		HDC hDc = ::GetDC( m_hTreeView );
		if ( !hDc ) { return -1; }

		INT iRet = 0;

		HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW( m_hTreeView, WM_GETFONT, 0L, 0L ));
		{
			LSW_SELECTOBJECT soFontOrig( hDc, hFont );	// Destructor sets the original font back.
			LSW_RECT rRet;
			TVITEM tvItem;
			tvItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_PARAM;
			tvItem.hItem = TreeView_GetRoot( m_hTreeView );
			tvItem.state = 0;
			tvItem.stateMask = 0xFF;
			while ( true ) {
				if ( !TreeView_GetItem( m_hTreeView, &tvItem ) ) { break; }
				const std::wstring * pwsText = &m_vRows[tvItem.lParam].vStrings[_iIdx];
				rRet.Zero();
				if ( _iIdx == 0 ) {
					TreeView_GetItemRect( m_hTreeView, tvItem.hItem, &rRet, TRUE );
					rRet.left = 0;
				}
				else {
					::DrawTextW( hDc, pwsText->c_str(), static_cast<int>(pwsText->size()), &rRet, DT_CALCRECT );
				}
				iRet = std::max( iRet, static_cast<INT>(rRet.Width()) );
				tvItem.hItem = TreeView_GetNextVisible( m_hTreeView, tvItem.hItem );
			}
		}


		::ReleaseDC( m_hTreeView, hDc );
		if ( _iIdx > 0 ) { return iRet + LSW_TREELIST_LEFT_MARGIN + 1; }
		return iRet;
	}

	// Gathers the selected items into a vector.
	size_t CTreeList::GatherSelected( std::vector<HTREEITEM> &_vReturn, bool _bIncludeNonVisible ) const {
		_vReturn.clear();
		GatherSelected( TreeView_GetRoot( m_hTreeView ), _vReturn, _bIncludeNonVisible );
		return _vReturn.size();
	}

	// Returns true if any of the selected items have children and are not in expanded view.
	bool CTreeList::AnySelectedHasUnexpandedChildren() const {
		for ( size_t I = 0; I < m_vRows.size(); ++I ) {
			TVITEM tvItem;
			tvItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_CHILDREN;
			tvItem.hItem = m_vRows[I].htiItem;
			tvItem.stateMask = static_cast<UINT>(~0);
			if ( !TreeView_GetItem( m_hTreeView, &tvItem ) ) { return false; }
			// Is it selected?
			if ( (tvItem.state | m_vRows[I].uiStateEx) & TVIS_SELECTED ) {
				// Does it have children and is unexpanded?
				if ( !(tvItem.state & TVIS_EXPANDED) && tvItem.cChildren != 0 ) { return true; }
			}
		}
		return false;
	}

	// Returns true if any of the selected items have children and are in expanded view.
	bool CTreeList::AnySelectedHasExpandedChildren() const {
		for ( size_t I = 0; I < m_vRows.size(); ++I ) {
			TVITEM tvItem;
			tvItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_CHILDREN;
			tvItem.hItem = m_vRows[I].htiItem;
			tvItem.stateMask = static_cast<UINT>(~0);
			if ( !TreeView_GetItem( m_hTreeView, &tvItem ) ) { return false; }
			// Is it selected?
			if ( (tvItem.state | m_vRows[I].uiStateEx) & TVIS_SELECTED ) {
				// Does it have children and is expanded?
				if ( (tvItem.state & TVIS_EXPANDED) && tvItem.cChildren != 0 ) { return true; }
			}
		}
		return false;
	}

	// Returns true if any of the items have children and are not in expanded view.
	bool CTreeList::AnyHasUnexpandedChildren() const {
		for ( size_t I = 0; I < m_vRows.size(); ++I ) {
			TVITEM tvItem;
			tvItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_CHILDREN;
			tvItem.hItem = m_vRows[I].htiItem;
			tvItem.stateMask = static_cast<UINT>(~0);
			if ( !TreeView_GetItem( m_hTreeView, &tvItem ) ) { return false; }
			// Does it have children and is unexpanded?
			if ( !(tvItem.state & TVIS_EXPANDED) && tvItem.cChildren != 0 ) { return true; }
		}
		return false;
	}

	// Returns true if any of the items have children and are in expanded view.
	bool CTreeList::AnyHasExpandedChildren() const {
		for ( size_t I = 0; I < m_vRows.size(); ++I ) {
			TVITEM tvItem;
			tvItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_CHILDREN;
			tvItem.hItem = m_vRows[I].htiItem;
			tvItem.stateMask = static_cast<UINT>(~0);
			if ( !TreeView_GetItem( m_hTreeView, &tvItem ) ) { return false; }
			// Does it have children and is expanded?
			if ( (tvItem.state & TVIS_EXPANDED) && tvItem.cChildren != 0 ) { return true; }
		}
		return false;
	}

	// Expands selected items.
	void CTreeList::ExpandSelected() const {
		for ( size_t I = 0; I < m_vRows.size(); ++I ) {
			TVITEM tvItem;
			tvItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_CHILDREN;
			tvItem.hItem = m_vRows[I].htiItem;
			tvItem.stateMask = static_cast<UINT>(~0);
			if ( TreeView_GetItem( m_hTreeView, &tvItem ) ) {
				// Is it selected?
				if ( (tvItem.state | m_vRows[I].uiStateEx) & TVIS_SELECTED ) {
					// Does it have children and is unexpanded?
					if ( !(tvItem.state & TVIS_EXPANDED) && tvItem.cChildren != 0 ) {
						TreeView_Expand( m_hTreeView, tvItem.hItem, TVE_EXPAND );
					}
				}
			}
		}
	}

	// Expands all items.
	void CTreeList::ExpandAll() const {
		for ( size_t I = 0; I < m_vRows.size(); ++I ) {
			TVITEM tvItem;
			tvItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_CHILDREN;
			tvItem.hItem = m_vRows[I].htiItem;
			tvItem.stateMask = static_cast<UINT>(~0);
			if ( TreeView_GetItem( m_hTreeView, &tvItem ) ) {
				// Does it have children and is unexpanded?
				if ( !(tvItem.state & TVIS_EXPANDED) && tvItem.cChildren != 0 ) {
					TreeView_Expand( m_hTreeView, tvItem.hItem, TVE_EXPAND );
				}
			}
		}
	}

	// Collapses selected items.
	void CTreeList::CollapseSelected() const {
		for ( size_t I = 0; I < m_vRows.size(); ++I ) {
			TVITEM tvItem;
			tvItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_CHILDREN;
			tvItem.hItem = m_vRows[I].htiItem;
			tvItem.stateMask = static_cast<UINT>(~0);
			if ( TreeView_GetItem( m_hTreeView, &tvItem ) ) {
				// Is it selected?
				if ( (tvItem.state | m_vRows[I].uiStateEx) & TVIS_SELECTED ) {
					// Does it have children and is expanded?
					if ( (tvItem.state & TVIS_EXPANDED) && tvItem.cChildren != 0 ) {
						TreeView_Expand( m_hTreeView, tvItem.hItem, TVE_COLLAPSE );
					}
				}
			}
		}
	}

	// Collapses all items.
	void CTreeList::CollapseAll() const {
		for ( size_t I = 0; I < m_vRows.size(); ++I ) {
			TVITEM tvItem;
			tvItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_CHILDREN;
			tvItem.hItem = m_vRows[I].htiItem;
			tvItem.stateMask = static_cast<UINT>(~0);
			if ( TreeView_GetItem( m_hTreeView, &tvItem ) ) {
				// Does it have children and is expanded?
				if ( (tvItem.state & TVIS_EXPANDED) && tvItem.cChildren != 0 ) {
					TreeView_Expand( m_hTreeView, tvItem.hItem, TVE_COLLAPSE );
				}
			}
		}
	}

	// A helper to easily create a tree view item to be inserted with only text.
	TVINSERTSTRUCTW CTreeList::DefaultItem( const WCHAR * _pwcText, HTREEITEM _tiParent, HTREEITEM _tiInsertAfter ) {
		TVINSERTSTRUCTW isItem = { 0 };
		isItem.hParent = _tiParent;
		isItem.hInsertAfter = _tiInsertAfter;
		isItem.item.mask = TVIF_TEXT;
		isItem.item.pszText = const_cast<LPWSTR>(_pwcText);
		isItem.item.cchTextMax = static_cast<int>(std::wcslen( _pwcText ));
		return isItem;
	}

	// A helper to easily create a tree view item to be inserted with only text and data.
	TVINSERTSTRUCTW CTreeList::DefaultItemLParam( const WCHAR * _pwcText, LPARAM _lpParm, HTREEITEM _tiParent, HTREEITEM _tiInsertAfter ) {
		TVINSERTSTRUCTW isItem = { 0 };
		isItem.hParent = _tiParent;
		isItem.hInsertAfter = _tiInsertAfter;
		isItem.item.mask = TVIF_TEXT | TVIF_PARAM;
		isItem.item.pszText = const_cast<LPWSTR>(_pwcText);
		isItem.item.cchTextMax = static_cast<int>(std::wcslen( _pwcText ));
		isItem.item.lParam = _lpParm;
		return isItem;
	}

	// WM_SIZE.
	CWidget::LSW_HANDLED CTreeList::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		CWidget::Size( _wParam, _lWidth, _lHeight );
		//::ShowScrollBar( Wnd(), SB_VERT, FALSE );
		ResizeControls( VirtualClientRect( nullptr ) );

		return LSW_H_CONTINUE;
	}

	// WM_MOVE.
	CWidget::LSW_HANDLED CTreeList::Move( LONG _lX, LONG _lY ) {
		CWidget::Move( _lX, _lY );
		ResizeControls( VirtualClientRect( nullptr ) );

		return LSW_H_CONTINUE;
	}

	// WM_HSCROLL
	CWidget::LSW_HANDLED CTreeList::HScroll( USHORT _uScrollPos, USHORT _uScrollType, HWND /*_hSender*/ ) {
		//return LSW_H_CONTINUE;
		int xDelta;     // xDelta = new_pos - current_pos
		int xNewPos;    // new position
		int yDelta = 0;
 
		switch ( _uScrollType )  {
			// User clicked the scroll bar shaft left of the scroll box.
			case SB_PAGEUP :  {
				xNewPos = m_iHScroll - 50; 
				break; 
			}
			// User clicked the scroll bar shaft right of the scroll box.
			case SB_PAGEDOWN : {
				xNewPos = m_iHScroll + 50;
				break;
			}
			// User clicked the left arrow.
			case SB_LINEUP : {
				xNewPos = m_iHScroll - 5;
				break;
			}
			// User clicked the right arrow.
			case SB_LINEDOWN : {
				xNewPos = m_iHScroll + 5;
				break;
			} 
			// User dragged the scroll box.
			case SB_THUMBPOSITION : {
				xNewPos = _uScrollPos;
				break;
			}
			default : {
				xNewPos = m_iHScroll;
			}
		}
 
		// New position must be between 0 and the screen width.
		xNewPos = std::max( 0, xNewPos );
		xNewPos = std::min( ContentWidth(), xNewPos );
 
		// If the current position does not change, do not scroll.
		if ( xNewPos == m_iHScroll ) { return LSW_H_CONTINUE; }
 
		// Set the scroll flag to TRUE.
		//fScroll = TRUE;
 
		// Determine the amount scrolled (in pixels).
		xDelta = xNewPos - m_iHScroll;
 
		// Reset the current scroll position.
		m_iHScroll = xNewPos;

		// Move the header.
		LSW_RECT rClient = VirtualClientRect( nullptr );
		LSW_RECT rRect;
		Header_GetItemRect( m_hHeader, 0, &rRect );
		INT iLeft = rClient.left - m_iHScroll;
		INT iWidth = rClient.right - iLeft;
		::MoveWindow( m_hHeader, iLeft, rClient.top, iWidth, rRect.Height(), FALSE );
 
		// Scroll the window. (The system repaints most of the
		// client area when ScrollWindowEx is called; however, it is
		// necessary to call UpdateWindow in order to repaint the
		// rectangle of pixels that were invalidated.)
		::ScrollWindowEx( Wnd(), -xDelta, -yDelta, NULL,
			NULL, NULL, NULL,
			MAKELONG( SW_INVALIDATE, 0 )
		/*| SW_SCROLLCHILDREN*/ );
		//::UpdateWindow( Wnd() );
		
 
		// Reset the scroll bar.
		SCROLLINFO siScroll;
		siScroll.cbSize = sizeof( siScroll );
		siScroll.fMask  = SIF_POS;
		siScroll.nPos   = m_iHScroll;
		::SetScrollInfo( Wnd(), SB_HORZ, &siScroll, FALSE );
		INT iTop = rClient.top + rRect.Height() - m_iVScroll;
		INT iHeight = (rClient.bottom - iTop);
		iWidth = (rClient.right - iLeft);
		::MoveWindow( m_hTreeView, iLeft, iTop, iWidth, iHeight, FALSE );

		Redraw();
		return LSW_H_CONTINUE;
	}

	// WM_LBUTTONDOWN.
	CWidget::LSW_HANDLED CTreeList::LButtonDown( DWORD _dwVirtKeys, const POINTS &_pCursorPos ) {
		POINT pPos = { _pCursorPos.x, _pCursorPos.y };
		::ClientToScreen( Wnd(), &pPos );
		::ScreenToClient( m_hTreeView, &pPos );
		HTREEITEM tiItem = TreeItemAt( static_cast<SHORT>(pPos.x), static_cast<SHORT>(pPos.y) );
		if ( !tiItem ) { return LSW_H_CONTINUE; }

		// Get the thing that was hit.
		TVHITTESTINFO hiHitTest = { 0 };
		hiHitTest.hItem = tiItem;
		hiHitTest.pt = pPos;
		TreeView_HitTest( m_hTreeView, &hiHitTest );
		if ( hiHitTest.flags == TVHT_ONITEMBUTTON ) {
			if ( TreeView_Expand( m_hTreeView, tiItem, TVE_TOGGLE ) ) {
				UpdateVertScroll();
				Redraw( TRUE );
				return LSW_H_HANDLED;
			}
		}
		else {
			LPARAM lpParm;
			UINT uiState = GetTreeItemState( tiItem, &lpParm );
			if ( lpParm >= 0 ) {
				uiState |= m_vRows[lpParm].uiStateEx;
				//if ( TreeView_Select( m_hTreeView, ((_dwVirtKeys & MK_CONTROL) && uiState & TVIS_SELECTED) ? NULL : tiItem, TVGN_CARET ) ) {
				if ( _dwVirtKeys & MK_CONTROL ) {
					if ( ToggleSelect( tiItem, TVGN_CARET, lpParm ) ) {
						Redraw( TRUE );
						return LSW_H_HANDLED;
					}
				}
				else if ( _dwVirtKeys & MK_SHIFT ) {
					//if ( Select( tiItem, TVGN_CARET, lpParm ) ) {
					if ( DragSelect( tiItem, TVGN_CARET, true, lpParm ) ) {
						Redraw( TRUE );
						return LSW_H_HANDLED;
					}
				}
				else {
					if ( SelectOnly( tiItem, TVGN_CARET, lpParm ) ) {
						Redraw( TRUE );
						return LSW_H_HANDLED;
					}
				}
			}
		}
		return CWidget::LButtonDown( _dwVirtKeys, _pCursorPos );
	}

	// Notification that a header is being dragged.
	BOOL CTreeList::NotifyHeaderDrag( LPNMHEADERW /*_lphHeader*/ ) {
		UpdateHorScroll();
		Redraw( true );
		return FALSE;
	}

	// Notification that a header bar was double-clicked (for automatic resizing).
	BOOL CTreeList::NotifyHeaderDividerDblClick( LPNMHEADERW _lphHeader ) {
		//if ( _lphHeader->
		INT iNewWidth = GetColumnTextWidth( _lphHeader->iItem );
		if ( iNewWidth >= 0 ) {
			SetColumnWidth( _lphHeader->iItem, iNewWidth );
		}

		return FALSE;
	}

	// Updates the horizontal scroll settings.
	void CTreeList::UpdateHorScroll() {
		LSW_RECT rClient = VirtualClientRect( nullptr );
		int iLimit = rClient.Width();

		SCROLLINFO siScroll = { sizeof( SCROLLINFO ) };
		// The horizontal scrolling range is defined by
		// (bitmap_width) - (client_width). The current horizontal
		// scroll value remains within the horizontal scrolling range.
		INT xMaxScroll = std::max( ContentWidth() - iLimit, 0 );
		m_iHScroll = std::min( m_iHScroll, xMaxScroll );
		siScroll.cbSize = sizeof( siScroll );
		siScroll.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
		siScroll.nMin   = 0;//xMinScroll;
		siScroll.nMax   = ContentWidth();
		siScroll.nPage  = iLimit;
		siScroll.nPos   = m_iHScroll;
		::SetScrollInfo( Wnd(), SB_HORZ, &siScroll, FALSE );

	}

	// Updates the vertical scroll settings.
	void CTreeList::UpdateVertScroll() {
#if 0
		LSW_RECT rClient = VirtualClientRect( nullptr );
		int iLimit = rClient.Height();

		SCROLLINFO siScroll = { sizeof( SCROLLINFO ) };
		// The vertical scrolling range is defined by
		// (bitmap_height) - (client_height). The current vertical
		// scroll value remains within the vertical scrolling range.
		INT yMaxScroll = std::max( ContentHeight() - iLimit, 0 );
		m_iVScroll = std::min( m_iVScroll, yMaxScroll );
		siScroll.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
		siScroll.nMin   = 0;//yMinScroll;
		siScroll.nMax   = ContentHeight();
		siScroll.nPage  = iLimit;
		siScroll.nPos   = m_iVScroll;
		::SetScrollInfo( Wnd(), SB_VERT, &siScroll, FALSE );
#endif
	}

	// Resizes the inner controls.
	void CTreeList::ResizeControls( const LSW_RECT &_rRect ) {
#if 1
		HDITEM hdItem;
		hdItem.mask = HDI_HEIGHT;
		Header_GetItem( m_hHeader, 0, &hdItem );

		int xNewSize = _rRect.Width();
		int yNewSize = _rRect.Height();


		LSW_RECT rRect;
		Header_GetItemRect( m_hHeader, 0, &rRect );
		INT iLeft = _rRect.left - m_iHScroll;
		INT iWidth = _rRect.right - iLeft;
		::MoveWindow( m_hHeader, iLeft, _rRect.top, iWidth, rRect.Height(), FALSE );
      
		SCROLLINFO siScroll = { sizeof( SCROLLINFO ) };
		// The horizontal scrolling range is defined by
		// (bitmap_width) - (client_width). The current horizontal
		// scroll value remains within the horizontal scrolling range.
		INT iContentWidth = ContentWidth();
		INT xMaxScroll = std::max( iContentWidth - xNewSize, 0 );
		m_iHScroll = std::min( m_iHScroll, xMaxScroll );
		siScroll.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
		siScroll.nMin   = 0;//xMinScroll;
		siScroll.nMax   = iContentWidth;
		siScroll.nPage  = xNewSize;
		siScroll.nPos   = m_iHScroll;
		::SetScrollInfo( Wnd(), SB_HORZ, &siScroll, FALSE );
 
		// The vertical scrolling range is defined by
		// (bitmap_height) - (client_height). The current vertical
		// scroll value remains within the vertical scrolling range.
		INT yMaxScroll = std::max( ContentHeight() - yNewSize, 0 );
		m_iVScroll = std::min( m_iVScroll, yMaxScroll );
		/*siScroll.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
		siScroll.nMin   = 0;//yMinScroll;
		siScroll.nMax   = ContentHeight();
		siScroll.nPage  = yNewSize;
		siScroll.nPos   = m_iVScroll;
		::SetScrollInfo( Wnd(), SB_VERT, &siScroll, FALSE );*/
		//::ShowScrollBar( Wnd(), SB_VERT, FALSE );

		INT iTop = _rRect.top + rRect.Height() - m_iVScroll;
		INT iBottom = (_rRect.bottom - iTop);
		INT iRight = (_rRect.right - iLeft);
		::MoveWindow( m_hTreeView, iLeft, iTop, iRight, iBottom, FALSE );
		//Redraw( false );
		//::MoveWindow( Wnd(), _rRect.left, _rRect.top, _rRect.Width(), _rRect.Height(), FALSE );
#else
		INT iContentWidth = ContentWidth();
		BOOL bNeedsHor = iContentWidth > _rRect.Width();
		if ( !bNeedsHor ) {
			m_iHScroll = 0;
		}

		LSW_RECT rRect;
		Header_GetItemRect( m_hHeader, 0, &rRect );

		HDITEM hdItem;
		hdItem.mask = HDI_HEIGHT;
		Header_GetItem( m_hHeader, 0, &hdItem );
	
		INT iLeft = _rRect.left - m_iHScroll;
		INT iWidth = _rRect.right - iLeft;

		::MoveWindow( m_hHeader, iLeft, _rRect.top, iWidth, rRect.Height(), FALSE );


		// Position the tree under the header and match its width to the width of the first column in the header.
		hdItem.mask = HDI_WIDTH;
		Header_GetItem( m_hHeader, 0, &hdItem );
		int iItemHeight = TreeView_GetItemHeight( m_hTreeView );
		INT iScreenTreeHeight = _rRect.Height() - rRect.Height();
		UINT uiActualHeight = TreeView_GetCount( m_hTreeView ) * iItemHeight;
		BOOL bNeedsVer = static_cast<INT>(uiActualHeight) > iScreenTreeHeight;
	
		INT iTop = _rRect.top + rRect.Height() - m_iVScroll;
	
		INT iBottom = (_rRect.bottom - iTop)/* + GetSystemMetrics( SM_CYHSCROLL )*/;
		INT iRight = (_rRect.right - iLeft)/* + ::GetSystemMetrics( SM_CXVSCROLL )*/;
	

		// TODO:
		SCROLLINFO siScroll;
		siScroll.cbSize = sizeof( siScroll );
		if ( bNeedsHor ) {
			siScroll.fMask = SIF_RANGE | SIF_PAGE;
			BOOL bGot = ::GetScrollInfo( m_hTreeView, SB_HORZ, &siScroll );
			::ShowScrollBar( m_hTreeView, SB_HORZ, TRUE );
			//::MoveWindow( m_hTreeView, iLeft, iTop, iRight/*hdItem.cxy*/, iBottom/* + ::GetSystemMetrics( SM_CYHSCROLL )*/, FALSE );
		}
		else {
			::ShowScrollBar( m_hTreeView, SB_HORZ, FALSE );
		}
		siScroll.fMask = SIF_POS;
		int iHeight = TreeView_GetItemHeight( m_hTreeView );
		if ( bNeedsVer ) {
			siScroll.nPos = 100/*m_iVScroll*/ / iHeight;
			::SetScrollInfo( m_hTreeView, SB_VERT, &siScroll, TRUE );
			::ShowScrollBar( m_hTreeView, SB_VERT, TRUE );
		}
		else {
			::ShowScrollBar( m_hTreeView, SB_VERT, FALSE );
		}
		::MoveWindow( m_hTreeView, iLeft, iTop, iRight, iBottom, FALSE );
#endif
	}

	// Gets the height of the tree view counting only expanded items.
	DWORD CTreeList::GetTreeHeight( HTREEITEM _tiItem ) const {
		DWORD dwHeight = 0;
		TVITEM tvItem;
		tvItem.mask = TVIF_HANDLE | TVIF_STATE;
		while ( TRUE ) {
			tvItem.hItem = _tiItem;
			tvItem.state = 0;
			tvItem.stateMask = 0xFF;
			if ( !TreeView_GetItem( m_hTreeView, &tvItem ) ) { return dwHeight; }
			if ( tvItem.state & TVIS_EXPANDED ) {
				dwHeight += GetTreeHeight( TreeView_GetChild( m_hTreeView, _tiItem ) );
			}
			dwHeight++;
			_tiItem = TreeView_GetNextSibling( m_hTreeView, _tiItem );
		}
		return dwHeight;
	}

	// Gets the index of a tree-view item or -1.
	INT CTreeList::GetIndexOf( HTREEITEM _tiItemToFind, HTREEITEM tiItemStart, INT &_iPos ) const {
		TVITEM tvItem;
		tvItem.mask = TVIF_HANDLE | TVIF_STATE;
		while ( TRUE ) {
			if ( tiItemStart == _tiItemToFind ) { return _iPos; }
			tvItem.hItem = tiItemStart;
			tvItem.state = 0;
			tvItem.stateMask = 0xFF;
			if ( !TreeView_GetItem( m_hTreeView, &tvItem ) ) { return -1; }
			_iPos++;
			if ( tvItem.state & TVIS_EXPANDED ) {
				INT iTemp = GetIndexOf( _tiItemToFind, TreeView_GetChild( m_hTreeView, tiItemStart ), _iPos );
				if ( iTemp != -1 ) { return iTemp; }
			}
		
			tiItemStart = TreeView_GetNextSibling( m_hTreeView, tiItemStart );
		}
		return -1;
	}

	// Gets the tree item under the given mouse position.
	HTREEITEM CTreeList::TreeItemAt( SHORT /*_sX*/, SHORT _sY ) const {
		TVITEM tvItem;
		tvItem.mask = TVIF_HANDLE;
		LSW_RECT rRect, rTree;
		tvItem.hItem = TreeView_GetFirstVisible( m_hTreeView );
		::GetClientRect( m_hTreeView, &rTree );
		while ( TreeView_GetItem( m_hTreeView, &tvItem ) ) {
			TreeView_GetItemRect( m_hTreeView, tvItem.hItem, &rRect, FALSE );
		
			if ( rRect.top > rTree.bottom ) { return NULL; }
			if ( rRect.bottom > _sY ) { return tvItem.hItem; }
		
			tvItem.hItem = TreeView_GetNextVisible( m_hTreeView, tvItem.hItem );
		}
		return NULL;
	}

	// Handle painting.
	BOOL CTreeList::OnPaint( WPARAM /*_wParam*/, LPARAM /*_lParam*/ ) {
		LSW_BEGINPAINT bpPaint( Wnd() );
		if ( m_bLocked ) { return 0; }

		LSW_SETBKCOLOR sbcB( bpPaint.hDc, RGB( 0xFF, 0xFF, 0xFF ) );
		INT iPos = 0;
		HTREEITEM hStart = TreeView_GetFirstVisible( m_hTreeView );
		INT iStartIndex = GetIndexOf( hStart, TreeView_GetRoot( m_hTreeView ), iPos );
		LSW_SELECTOBJECT soFontOrig( bpPaint.hDc, reinterpret_cast<HGDIOBJ>(::SendMessageW( m_hTreeView, WM_GETFONT, 0, 0 )) );	// Destructor sets the original font back.
		LSW_SELECTOBJECT soPenOrig( bpPaint.hDc, reinterpret_cast<HGDIOBJ>(::CreatePen( PS_SOLID, 1, ::GetSysColor( COLOR_3DFACE ) )), true );	// Destructor sets the original pen back and deletes it.

		LSW_PAINTPARMS ppParms;
		ppParms.hStart = hStart;
		ppParms.iHeaderCount = GetColumnCount();
		ppParms.iIndex = iStartIndex;
		ppParms.rClient = VirtualClientRect( nullptr );
		DrawTreeItems( bpPaint.hDc, ppParms, bpPaint.psPaint );
	
		// ::EndPaint() called by the destructor of bpPaint.
		return 0;
	}

	// Draw the tree items.
	void CTreeList::DrawTreeItems( HDC _hDc, LSW_PAINTPARMS &_ppParms, PAINTSTRUCT &/*_psPaintStruct*/ ) {
		/*{
			WNDPROC wpOrig = m_wpTreeViewProc;
			::CallWindowProc( wpOrig, m_hTreeView, WM_PAINT, 0, 0 );
			return;
		}*/

		/*POINT origin;
		::GetWindowOrgEx( _hDc, &origin );
		::SetWindowOrgEx( _hDc, origin.x + m_iHScroll, origin.y + m_iVScroll, 0 );

		// Move the paint rectangle into the new coordinate system
		::OffsetRect( &_psPaintStruct.rcPaint, m_iHScroll, m_iVScroll );
		*/
//#define MX_DRAW

		SCROLLBARINFO sbiInfo = { sizeof( SCROLLBARINFO ) };
		::GetScrollBarInfo( m_hTreeView, OBJID_VSCROLL, &sbiInfo );
		LSW_RECT rThisClient = VirtualClientRect( nullptr );
		LSW_RECT rThisVertScrollRect = rThisClient;

		
		if ( sbiInfo.rgstate[0] & STATE_SYSTEM_INVISIBLE ) {
			rThisVertScrollRect.left = rThisVertScrollRect.right;
		}
		else {
			/*LSW_RECT rTmp = sbiInfo.rcScrollBar;
			LSW_RECT rScrollInWnd = rTmp.ClientToScreen( m_hTreeView ).ScreenToClient( Wnd() );
			::InvalidateRect( Wnd(), &rScrollInWnd, FALSE );
			//::InvalidateRect( m_hTreeView, &rThisVertScrollRect.ClientToScreen( Wnd() ).ScreenToClient( m_hTreeView ), FALSE );
			*/
			/*int iWidth = */::GetSystemMetrics( SM_CXVSCROLL );
			rThisVertScrollRect.left = rThisVertScrollRect.right - ::GetSystemMetrics( SM_CXVSCROLL );
			LSW_RECT rTemp = sbiInfo.rcScrollBar;
#if 0
			::ValidateRect( Wnd(), &rThisVertScrollRect );
			::InvalidateRect( Wnd(), &rThisVertScrollRect, FALSE );
			::InvalidateRect( m_hTreeView, &rThisVertScrollRect.ClientToScreen( Wnd() ).ScreenToClient( m_hTreeView ), FALSE );
#endif
			
			//::InvalidateRect( Wnd(), &rTemp.ScreenToClient( Wnd() ), FALSE );
			LSW_RECT rTmp = rTemp.ScreenToClient( m_hTreeView );
			::InvalidateRect( m_hTreeView, &rTmp, FALSE );
			rTmp = rTemp.ScreenToClient( Wnd() );
			::ValidateRect( Wnd(), &rTmp );
		}
		
		LSW_RECT rValidateMe;
		::GetClientRect( m_hTreeView, &rValidateMe );
		// Validate the header out.
		LSW_RECT rHeaderRect;
		::GetClientRect( m_hHeader, &rHeaderRect );
		//rHeaderRect.ScreenToClient( Wnd() );
		LSW_RECT rTmp = rHeaderRect.ClientToScreen( m_hHeader ).ScreenToClient( Wnd() );
		::ValidateRect( Wnd(), &rTmp );

		TVITEM tvItem;
		tvItem.mask = TVIF_HANDLE | TVIF_STATE | TVIF_PARAM;
		bool bRenderedTree = false;
		LSW_RECT rItem;
		rItem.Zero();
		while ( TRUE ) {
			tvItem.hItem = _ppParms.hStart;
			tvItem.state = 0;
			tvItem.stateMask = 0xFF;
			//tvItem.stateMask  = TVIS_STATEIMAGEMASK;
			if ( !TreeView_GetItem( m_hTreeView, &tvItem ) ) { break; }
			// Draw it.
			if ( (tvItem.state | m_vRows[tvItem.lParam].uiStateEx) & TVIS_SELECTED ) {
				::SetBkColor( _hDc, ::GetSysColor( COLOR_HIGHLIGHT ) );
				::SetTextColor( _hDc, ::GetSysColor( COLOR_HIGHLIGHTTEXT ) );
			}
			else {
				::SetBkColor( _hDc, _ppParms.iIndex % 2 == 0 ? RGB( 0xF0, 0xF0, 0xF0 ) : ::GetSysColor( COLOR_WINDOW ) );
				::SetTextColor( _hDc, ::GetSysColor( COLOR_WINDOWTEXT ) );
			}

			TreeView_GetItemRect( m_hTreeView, tvItem.hItem, &rItem, FALSE );
			
			rItem = rItem.ClientToScreen( m_hTreeView ).ScreenToClient( Wnd() );
			//rItem.MoveBy( { 0, static_cast<LONG>(m_uiHeaderHeight) } );

			if ( rItem.bottom >= _ppParms.rClient.top ) {
				//rItem = GetHeaderLandR( 0 );
				//ExtTextOut( _hDc, rItem.left, 0, ETO_OPAQUE | ETO_CLIPPED, &rItem, "", 0, NULL );
				INT iLastRight = rItem.right;
				if ( tvItem.lParam >= 0 && static_cast<size_t>(tvItem.lParam) < m_vRows.size() ) {
					INT iColumns = GetColumnCount();
					for ( INT I = 0; I < iColumns; ++I ) {
						LSW_RECT rThisItem = GetHeaderLandR( I, false );
						rThisItem.right = min( rThisVertScrollRect.left, rThisItem.right );
						rItem.left = rThisItem.left;
						rItem.right = rThisItem.right;
						if ( rItem.Width() > 0 ) { 
							if ( I == 0 ) {
								if ( bRenderedTree ) { continue; }
								// Invalidate the whole thing.
								//::InvalidateRect( m_hTreeView, &rValidateMe, FALSE );
								rValidateMe.left = rThisItem.right + m_iHScroll;
								//LSW_RECT rIcon = GetExpandIconRectFromItemRect( rItem );
								if ( (sbiInfo.rgstate[0] & STATE_SYSTEM_INVISIBLE) == 0 ) {
									// Retain the tree-view's vertical scrollbar.
									//LSW_RECT rTreeViewScroll = rThisVertScrollRect.ClientToScreen( Wnd() ).ScreenToClient( m_hTreeView );
									
									//::InvalidateRect( m_hTreeView, &rThisVertScrollRect, FALSE );
									rValidateMe.right = min( rThisVertScrollRect.left, rValidateMe.right );
									//::ValidateRect( m_hTreeView, &sbiInfo.rcScrollBar );
								}

								// Validate the portion over which we will draw.
								::ValidateRect( m_hTreeView, &rValidateMe );

								WNDPROC wpOrig = m_wpTreeViewProc;
								::CallWindowProc( wpOrig, m_hTreeView, WM_PAINT, 0, 0 );
								bRenderedTree = true;
								continue;
							}

							INT iDrawLeft = rItem.left;
					
							::MoveToEx( _hDc, iDrawLeft, rItem.top, NULL );
							::LineTo( _hDc, iDrawLeft, rItem.bottom );
							rItem.left++;
							LSW_RECT rBounds = rItem;
							// Space between the front bar and the text.
							rBounds.right = rBounds.left + LSW_TREELIST_LEFT_MARGIN;
							::ExtTextOutW( _hDc, 0, 0, ETO_OPAQUE | ETO_CLIPPED, &rBounds, L"", 0, NULL );
							rItem.left += LSW_TREELIST_LEFT_MARGIN;
							const std::wstring * pwsText = &m_vRows[tvItem.lParam].vStrings[I];
							rBounds = rItem;
							::DrawTextW( _hDc, pwsText->c_str(), static_cast<int>(pwsText->size()), &rBounds, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT
#ifdef DT_END_ELLIPSIS
								| DT_END_ELLIPSIS
#endif	// DT_END_ELLIPSIS
							);

							// Space from after the text to the end of the box.
							rBounds.left = rBounds.right;
							rBounds.right = rItem.right;
							::ExtTextOutW( _hDc, 0, 0, ETO_OPAQUE | ETO_CLIPPED, &rItem, L"", 0, NULL );

							// Draw the text between the starting space and the end-of-the-box space.
							::DrawTextW( _hDc, pwsText->c_str(), static_cast<int>(pwsText->size()), &rItem, DT_VCENTER | DT_SINGLELINE
#ifdef DT_END_ELLIPSIS
								| DT_END_ELLIPSIS
#endif	// DT_END_ELLIPSIS
							);
							iLastRight = rItem.right;
						}
					}
				}
				rItem.left = iLastRight;
				rItem.right = _ppParms.rClient.right - rThisVertScrollRect.Width();
				if ( rItem.Width() > 0 ) {
					::ExtTextOutW( _hDc, 0, 0, ETO_OPAQUE | ETO_CLIPPED, &rItem, L"", 0, NULL );
				}


				_ppParms.iIndex++;
				if ( rItem.top > _ppParms.rClient.bottom ) {
					break;
				}
			}
			_ppParms.hStart = TreeView_GetNextVisible( m_hTreeView, tvItem.hItem );
		}
		if ( rItem.bottom + rItem.Height() < _ppParms.rClient.bottom ) {
			::SetBkColor( _hDc, ::GetSysColor( COLOR_WINDOW ) );
			::SetTextColor( _hDc, ::GetSysColor( COLOR_WINDOWTEXT ) );
			//LSW_RECT rThisItem = GetHeaderLandR( 1, false );
			LSW_RECT rBounds = rItem;
			rBounds.left = _ppParms.rClient.left;//rThisItem.left;
			rBounds.right = _ppParms.rClient.right - rThisVertScrollRect.Width();
			if ( rBounds.Width() > 0 ) {
				rBounds.bottom = _ppParms.rClient.bottom;
				rBounds.top += rItem.Height();
				::ExtTextOutW( _hDc, 0, 0, ETO_OPAQUE | ETO_CLIPPED, &rBounds, L"", 0, NULL );
			}
		}

		//::SetWindowOrgEx( _hDc, origin.x, origin.y, 0 );
		//WNDPROC wpOrig = m_wpTreeViewProc;
		::InvalidateRect( m_hHeader, NULL, FALSE );
		::CallWindowProc( m_wpHeaderProc, m_hHeader, WM_PAINT, 0, 0 );


#if 1
		if ( !(sbiInfo.rgstate[0] & STATE_SYSTEM_INVISIBLE) ) {
			// Tree-view has a vertical sroll bar.
#if 0
			LSW_RECT rItem = rThisVertScrollRect.ClientToScreen( Wnd() ).ScreenToClient( m_hTreeView );
			LSW_RECT rTmp = sbiInfo.rcScrollBar;
			//rTmp = rTmp.ClientToScreen( Wnd() ).ScreenToClient( m_hTreeView );
			::InvalidateRect( m_hTreeView, &rItem, FALSE );
#endif	// 0
			LSW_RECT rTemp = sbiInfo.rcScrollBar;
			LSW_RECT rTmp2 = rTemp.ScreenToClient( m_hTreeView );
			::InvalidateRect( m_hTreeView, &rTmp2, TRUE );
			WNDPROC wpOrig2 = m_wpTreeViewProc;
			::CallWindowProc( wpOrig2, m_hTreeView, WM_NCPAINT, 0, 0 );
		}
#endif
	}

	// Gets the header left and right pixel coordinates in Wnd() client space.
	LSW_RECT CTreeList::GetHeaderLandR( INT _iColumn, bool _bApplyScroll ) const {
		LSW_RECT rRect;
		rRect.Zero();
		if ( !Header_GetItemRect( m_hHeader, _iColumn, &rRect ) ) { return rRect; }
		rRect = rRect.ClientToScreen( m_hHeader ).ScreenToClient( Wnd() );
		rRect.left -= 1;
		rRect.right -= 1;
		if ( _bApplyScroll ) {
			rRect.left -= m_iHScroll;
			rRect.right -= m_iHScroll;
		}
		return rRect;
	}

	// Redraws the tree-list.
	void CTreeList::Redraw( bool _bUpdateNow ) {
		//_bUpdateNow = true;
		
		/*::InvalidateRect( m_hHeader, NULL, FALSE );
		::InvalidateRect( m_hTreeView, NULL, FALSE );*/
		::RedrawWindow( Wnd(), NULL, NULL, RDW_INVALIDATE /*| RDW_ERASENOW | RDW_ERASE */| RDW_ALLCHILDREN | (_bUpdateNow ? RDW_UPDATENOW : 0) | RDW_FRAME );
		
		/*::RedrawWindow( m_hHeader, NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW | RDW_ERASE| RDW_ALLCHILDREN | (_bUpdateNow ? RDW_UPDATENOW : 0) | RDW_FRAME );
		::RedrawWindow( m_hTreeView, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE | (_bUpdateNow ? RDW_UPDATENOW : 0) | RDW_FRAME );*/
		
		/*::InvalidateRect( Wnd(), NULL, TRUE );
		::InvalidateRect( m_hHeader, NULL, TRUE );
		::InvalidateRect( m_hTreeView, NULL, TRUE );
		::UpdateWindow( Wnd() );
		::UpdateWindow( m_hHeader );
		::UpdateWindow( m_hTreeView );*/
		/*::RedrawWindow( Wnd(), NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW | RDW_ERASE | RDW_ALLCHILDREN | (_bUpdateNow ? RDW_UPDATENOW : 0) | RDW_FRAME );
		::RedrawWindow( m_hHeader, NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW | RDW_ERASE| RDW_ALLCHILDREN | (_bUpdateNow ? RDW_UPDATENOW : 0) | RDW_FRAME );
		::RedrawWindow( m_hTreeView, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE | (_bUpdateNow ? RDW_UPDATENOW : 0) | RDW_FRAME );*/
	}

	// Gets a tree item's state.
	UINT CTreeList::GetTreeItemState( HTREEITEM _tiItem, LPARAM * _lpParm ) const {
		TVITEM tvItem;
		tvItem.mask = TVIF_HANDLE | TVIF_STATE | (_lpParm ? TVIF_PARAM : 0);
		tvItem.hItem = _tiItem;
		tvItem.state = 0;
		tvItem.stateMask = 0xFF;
		tvItem.lParam = -1;
		TreeView_GetItem( m_hTreeView, &tvItem );	// Failure keeps tvItem.state at 0.
		if ( _lpParm ) {
			(*_lpParm) = tvItem.lParam;
		}
		return tvItem.state;
	}

	// Selects a tree item (or NULL to unselect all tree items).
	BOOL CTreeList::Select( HTREEITEM _htiItem, UINT /*_uiCode*/, LPARAM _lpParm ) {
		if ( !_htiItem ) {
			//if ( TreeView_Select( m_hTreeView, NULL, _uiCode ) ) {
			{
				for ( auto I = m_vRows.size(); I--; ) {
					if ( (GetTreeItemState( m_vRows[I].htiItem ) | m_vRows[I].uiStateEx) & TVIS_SELECTED ) {
						TreeView_SetItemState( m_hTreeView, m_vRows[I].htiItem, 0, TVIS_SELECTED );
						m_vRows[I].uiStateEx = m_vRows[I].uiStateEx & ~TVIS_SELECTED;
					}
				}
				m_stLastSel = LSW_TREELIST_INVALID;
				return TRUE;
			}
			return FALSE;
		}
		if ( _lpParm == -1 ) {
			/*UINT uiState = */GetTreeItemState( _htiItem, &_lpParm );
		}
		if ( _lpParm >= 0 ) {
			TreeView_SetItemState( m_hTreeView, _htiItem, TVIS_SELECTED, TVIS_SELECTED );
			TreeView_EnsureVisible( m_hTreeView, _htiItem );
			//if ( TreeView_Select( m_hTreeView, _htiItem, _uiCode ) ) {
				m_vRows[_lpParm].uiStateEx |= TVIS_SELECTED;
				m_stLastSel = _lpParm;
				return TRUE;
			//}
		}
		return FALSE;
	}

	// Toggles the selection of a given tree item (or NULL to unselect all tree items).
	BOOL CTreeList::ToggleSelect( HTREEITEM _htiItem, UINT _uiCode, LPARAM _lpParm ) {
		if ( !_htiItem ) { return Select( _htiItem, _uiCode, _lpParm ); }

		if ( _lpParm == -1 ) {
			/*UINT uiState = */GetTreeItemState( _htiItem, &_lpParm );
		}
		if ( _lpParm >= 0 ) {
			if ( m_vRows[_lpParm].uiStateEx & TVIS_SELECTED ) {
				// Unselect it.
				TreeView_SetItemState( m_hTreeView, _htiItem, 0, TVIS_SELECTED );
				m_vRows[_lpParm].uiStateEx = m_vRows[_lpParm].uiStateEx & ~TVIS_SELECTED;
			}
			else {
				// Select it.
				TreeView_SetItemState( m_hTreeView, _htiItem, TVIS_SELECTED, TVIS_SELECTED );
				TreeView_EnsureVisible( m_hTreeView, _htiItem );
				m_vRows[_lpParm].uiStateEx |= TVIS_SELECTED;
			}
			m_stLastSel = _lpParm;
			return TRUE;
		}
		return FALSE;
	}

	// Selects only the given item, removing selection from all other items (or NULL to unselect all tree items).
	BOOL CTreeList::SelectOnly( HTREEITEM _htiItem, UINT _uiCode, LPARAM _lpParm ) {
		if ( !_htiItem ) { return Select( _htiItem, _uiCode, _lpParm ); }

		if ( _lpParm == -1 ) {
			/*UINT uiState = */GetTreeItemState( _htiItem, &_lpParm );
		}
		for ( auto I = m_vRows.size(); I--; ) {
			if ( static_cast<LPARAM>(I) == _lpParm ) {
				// Select it.
				TreeView_SetItemState( m_hTreeView, _htiItem, TVIS_SELECTED, TVIS_SELECTED );
				TreeView_EnsureVisible( m_hTreeView, _htiItem );
				m_vRows[_lpParm].uiStateEx |= TVIS_SELECTED;
				m_stLastSel = _lpParm;
			}
			else {
				// Unselect it.
				if ( (GetTreeItemState( m_vRows[I].htiItem ) | m_vRows[I].uiStateEx) & TVIS_SELECTED ) {
					// Unselect it.
					TreeView_SetItemState( m_hTreeView, m_vRows[I].htiItem, 0, TVIS_SELECTED );
					m_vRows[I].uiStateEx = m_vRows[I].uiStateEx & ~TVIS_SELECTED;
				}
			}
		}
		return TRUE;
	}

	// Drags a selection from the item to which m_stLastSel points until (and including) _htiItem, unless _htiItem is NULL, in which case
	//	all items are unselected.
	BOOL CTreeList::DragSelect( HTREEITEM _htiItem, UINT _uiCode, bool _bAddToSelection, LPARAM _lpParm ) {
		if ( !_htiItem || !m_vRows.size() ) { return Select( _htiItem, _uiCode, _lpParm ); }

		if ( _lpParm == -1 ) {
			_lpParm = GetItemLParam( _htiItem );
		}
		size_t stDragFrom = m_stLastSel == LSW_TREELIST_INVALID ? 0 : m_stLastSel;
		//LPARAM lpDragFromParm = GetItemLParam( _htiItem );
		HTREEITEM htiThis = TreeView_GetRoot( m_hTreeView );
		bool bInSelection = false;
		m_stLastSel = _lpParm;
		std::vector<HTREEITEM> vStack;
		vStack.push_back( htiThis );
		while ( htiThis || vStack.size() ) {
			if ( !htiThis ) {
				vStack.pop_back();
				if ( !vStack.size() ) { break; }
				htiThis = vStack[vStack.size()-1];
				htiThis = TreeView_GetNextSibling( m_hTreeView, htiThis );
				continue;
			}
			LPARAM lpThis = LSW_TREELIST_INVALID;
			UINT uiState = GetTreeItemState( htiThis, &lpThis );
			if ( htiThis == _htiItem || htiThis == m_vRows[stDragFrom].htiItem ) {
				// Select it.
				TreeView_SetItemState( m_hTreeView, htiThis, TVIS_SELECTED, TVIS_SELECTED );
				//TreeView_EnsureVisible( m_hTreeView, _htiItem );
				m_vRows[lpThis].uiStateEx |= TVIS_SELECTED;

				if ( bInSelection && _bAddToSelection ) {
					// We are ending the selection here, and since we don't need to remove the selection
					//	flag from any furter items we can stop.
					return TRUE;
				}
				bInSelection = !bInSelection;
			}
			else if ( bInSelection ) {
				// Select it.
				TreeView_SetItemState( m_hTreeView, htiThis, TVIS_SELECTED, TVIS_SELECTED );
				//TreeView_EnsureVisible( m_hTreeView, htiThis );
				m_vRows[lpThis].uiStateEx |= TVIS_SELECTED;
			}
			else if ( !_bAddToSelection ) {
				// Unselect it.
				TreeView_SetItemState( m_hTreeView, htiThis, 0, TVIS_SELECTED );
				m_vRows[lpThis].uiStateEx = m_vRows[lpThis].uiStateEx & ~TVIS_SELECTED;
			}
			if ( uiState & TVIS_EXPANDED ) {
				vStack[vStack.size()-1] = htiThis;
				htiThis = TreeView_GetChild( m_hTreeView, htiThis );
				vStack.push_back( htiThis );
			}
			else {
				htiThis = TreeView_GetNextSibling( m_hTreeView, htiThis );
			}
		}
		return TRUE;
	}

	// Gathers the selected items into a vector.
	void CTreeList::GatherSelected( HTREEITEM _htiFrom, std::vector<HTREEITEM> &_vReturn, bool _bIncludeNonVisible ) const {
		while ( _htiFrom ) {
			LPARAM lpParm = LSW_TREELIST_INVALID;
			UINT uiState = GetTreeItemState( _htiFrom, &lpParm );
			if ( lpParm == LSW_TREELIST_INVALID ) { return; }
			if ( (uiState | m_vRows[lpParm].uiStateEx) & TVIS_SELECTED ) {
				_vReturn.push_back( _htiFrom );
			}
			if ( _bIncludeNonVisible || (uiState & TVIS_EXPANDED) ) {
				GatherSelected( TreeView_GetChild( m_hTreeView, _htiFrom ), _vReturn, _bIncludeNonVisible );
			}
			_htiFrom = TreeView_GetNextSibling( m_hTreeView, _htiFrom );
		}
	}

	// Takes an item's rectangle and produces a new rectangle where an expand/contract icon should be displayed.
	LSW_RECT CTreeList::GetExpandIconRectFromItemRect( const LSW_RECT &_rItem ) {
		LSW_RECT rRet = _rItem;
		rRet.left += 2;
		rRet.right = rRet.left + _rItem.Height();	// Square.

		rRet.ConstrainBy( _rItem );
		return rRet;
	}

	// List-view window procedure.  The list-view is hidden.
	LRESULT CALLBACK CTreeList::ListViewOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CTreeList * ptlThis = reinterpret_cast<CTreeList *>(::GetPropW( _hWnd, m_szProp ));
		WNDPROC wpOrig = ptlThis->m_wpListViewProc;

		switch ( _uMsg ) {
			case WM_PAINT : {
				/*PAINTSTRUCT psPaintMe;
				HDC hDc;
				if ( _wParam ) { hDc = reinterpret_cast<HDC>(_wParam); }
				else { hDc = ::BeginPaint( _hWnd, &psPaintMe ); }
				::EndPaint( _hWnd, &psPaintMe );*/
				return 0;
			}
			case WM_ERASEBKGND : { return 0; }
			case WM_NCHITTEST : { return HTNOWHERE; }
			case WM_NCDESTROY : {
				::RemovePropW( _hWnd, m_szProp );
				break;
			}
			case WM_LBUTTONDBLCLK : {
				break;
			}
			case WM_NCLBUTTONDBLCLK : {
				break;
			}
		}
		return ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
	}

	// Tree-view window procedure.  Overrides the Win32 tree-view control.
	LRESULT CALLBACK CTreeList::TreeViewOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CTreeList * ptlThis = reinterpret_cast<CTreeList *>(::GetPropW( _hWnd, m_szProp ));
		WNDPROC wpOrig = ptlThis->m_wpTreeViewProc;

		switch ( _uMsg ) {
			// =======================================
			// Drawing.
			// =======================================
			case WM_NCPAINT : { return 0; }
			case WM_ERASEBKGND : { return 0; }
			case WM_PAINT : { return 0; }

			// =======================================
			// Scrolling.
			// =======================================
			case WM_HSCROLL : {}
			case WM_VSCROLL : {
				//return ::SendMessageW( ptlThis->Wnd(), _uMsg, _wParam, _lParam );
				LRESULT lrRes = ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
				ptlThis->Redraw( true );
				ptlThis->SetFocus();
				return lrRes;
			}
			/*case WM_ACTIVATE : {
				volatile int gjhg = 0;
				break;
			}*/
			

			// =======================================
			// Mouse.
			// =======================================
#ifdef WM_MOUSEWHEEL
			case WM_MOUSEWHEEL : {
				LRESULT lrRes = ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
				ptlThis->Redraw( true );
				ptlThis->SetFocus();
				return lrRes;
			}
#endif	// #ifdef WM_MOUSEWHEEL

			case WM_NCLBUTTONDBLCLK : {}
			case WM_LBUTTONDOWN : {}
			case WM_LBUTTONDBLCLK : {}
			case WM_MBUTTONDOWN : {}
			case WM_MBUTTONDBLCLK : {}
			case WM_RBUTTONDOWN : {}
			case WM_RBUTTONDBLCLK : { ptlThis->SetFocus(); }
			case WM_MOUSEHOVER : {}
			case WM_MOUSEMOVE : {
				POINT pPos = {
					GET_X_LPARAM( _lParam ),
					GET_Y_LPARAM( _lParam ),
				};
				::ClientToScreen( _hWnd, &pPos );
				::ScreenToClient( ptlThis->Wnd(), &pPos );
				return ::SendMessageW( ptlThis->Wnd(), _uMsg, _wParam, MAKELPARAM( pPos.x, pPos.y ) );
			}

			case TVM_EXPAND : {
				LRESULT lrRes = ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
				ptlThis->Redraw( true );
				ptlThis->SetFocus();
				return lrRes;
			}
		}

		return ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
	}

	// Tree-list window procedure.  Represents this actual final control.
	LRESULT CALLBACK CTreeList::TreeListOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CTreeList * ptlThis = reinterpret_cast<CTreeList *>(::GetPropW( _hWnd, m_szProp ));
		WNDPROC wpOrig = ptlThis->m_wpTreeListProc;
		switch ( _uMsg ) {
			// =======================================
			// Drawing.
			// =======================================
			case WM_PAINT : { return ptlThis->OnPaint( _wParam, _lParam ); }
			case WM_ERASEBKGND : { return 0; }
#if 0
			case WM_NCPAINT : {
				//LSW_BEGINPAINT bpPaint( ptlThis->Wnd() );
				HDC hDc = /*bpPaint.hDc;*///::GetDCEx( ptlThis->Wnd(), reinterpret_cast<HRGN>(_wParam), DCX_WINDOW/* | DCX_INTERSECTRGN*/ );
					::GetWindowDC( ptlThis->Wnd() );
				LSW_RECT rRect = ptlThis->WindowRect();
				rRect = rRect.ScreenToClient( ptlThis->Wnd() );
				rRect.MoveBy( { -rRect.left, -rRect.top } );
				BOOL bYay = ::DrawEdge( hDc, &rRect, EDGE_ETCHED, BF_RECT );
				//::FrameRect( hDc

				::ReleaseDC( ptlThis->Wnd(), hDc );
				return 0;
			}
#endif
			// =======================================
			// Notifications.
			// =======================================
			case WM_NOTIFY : {
				LPNMHDR lpHdr = reinterpret_cast<LPNMHDR>(_lParam);
				switch ( lpHdr->code ) {
					case NM_CUSTOMDRAW : {
						LPNMTVCUSTOMDRAW lpNMCustomDraw = reinterpret_cast<LPNMTVCUSTOMDRAW>(_lParam);
						if ( lpNMCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT ) {
							return CDRF_NOTIFYITEMDRAW;
						}
						if ( lpNMCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT ) {
							if ( lpNMCustomDraw->nmcd.uItemState & CDIS_SELECTED ) {
								lpNMCustomDraw->clrText = ::GetSysColor( COLOR_HIGHLIGHTTEXT );
								lpNMCustomDraw->clrTextBk = ::GetSysColor( COLOR_HIGHLIGHT );
							}
							return 0;
						}
						break;
					}
					case HDN_DIVIDERDBLCLICK : {
						LPNMHEADERW phdr = reinterpret_cast<LPNMHEADERW>(_lParam);
						return ptlThis->NotifyHeaderDividerDblClick( phdr );
					}
					case HDN_ITEMCHANGED : {}
					case HDN_BEGINTRACKW : {}
					case HDN_ENDTRACKW : {}
					case HDN_TRACKW : {
						LPNMHEADERW phdr = reinterpret_cast<LPNMHEADERW>(_lParam);
						return ptlThis->NotifyHeaderDrag( phdr );
					}
					case TVN_GETDISPINFOW : {
						// Return value is ignored.
						LPNMTVDISPINFOW lpdiInfo = reinterpret_cast<LPNMTVDISPINFOW>(_lParam);
						if ( lpdiInfo->item.mask & TVIF_TEXT ) {
							TVITEMW iItem = { 0 };
							iItem.mask = TVIF_HANDLE | TVIF_PARAM;
							iItem.hItem = lpdiInfo->item.hItem;
							if ( !TreeView_GetItem( ptlThis->m_hTreeView, &iItem ) ) { return FALSE; }
							if ( static_cast<size_t>(iItem.lParam) >= ptlThis->m_vRows.size() || iItem.lParam < 0 ) { return FALSE; }
							if ( !ptlThis->m_vRows[iItem.lParam].vStrings.size() ) { return FALSE; }
							return SUCCEEDED( ::StringCchCopyW( lpdiInfo->item.pszText, lpdiInfo->item.cchTextMax, ptlThis->m_vRows[iItem.lParam].vStrings[0].c_str() ) );
						}
						return TRUE;
					}
				}
				break;
			}
			// =======================================
			// Tree-view Forwarding.
			// =======================================
			// These messages should be handled by the tree-view control we manage.
			//case WM_VSCROLL : {}
			case TVM_CREATEDRAGIMAGE : {}
			case TVM_EDITLABELW : {}
			case TVM_ENDEDITLABELNOW : {}
			case TVM_ENSUREVISIBLE : {}
			case TVM_EXPAND : {}
			case TVM_GETBKCOLOR : {}
			case TVM_GETCOUNT : {}
			case TVM_GETEDITCONTROL : {}
			case TVM_GETIMAGELIST : {}
			case TVM_GETINDENT : {}
			case TVM_GETINSERTMARKCOLOR : {}
			case TVM_GETISEARCHSTRINGW : {}
			case TVM_GETITEMHEIGHT : {}
			case TVM_GETITEMRECT : {}
			case TVM_GETITEMSTATE : {}
			case TVM_GETLINECOLOR : {}
			case TVM_GETNEXTITEM : {}
			case TVM_GETSCROLLTIME : {}
			case TVM_GETTEXTCOLOR : {}
			case TVM_GETTOOLTIPS : {}
			case TVM_GETUNICODEFORMAT : {}
			case TVM_GETVISIBLECOUNT : {}
			case TVM_HITTEST : {}
			case WM_MOUSEWHEEL : {}
			case TVM_SELECTITEM : {}
			case TVM_SETBKCOLOR : {}
			case TVM_SETIMAGELIST : {}
			case TVM_SETINDENT : {}
			case TVM_SETINSERTMARK : {}
			case TVM_SETINSERTMARKCOLOR : {}
			case TVM_SETITEMHEIGHT : {}
			case TVM_SETLINECOLOR : {}
			case TVM_SETTEXTCOLOR : {}
			case TVM_SETTOOLTIPS : {}
			case TVM_SETUNICODEFORMAT : {}
			case TVM_SORTCHILDREN : { return ::SendMessageW( ptlThis->m_hTreeView, _uMsg, _wParam, _lParam ); }
			case TVM_EDITLABELA : {}
			case TVM_GETISEARCHSTRINGA : { return ::SendMessageA( ptlThis->m_hTreeView, _uMsg, _wParam, _lParam ); }


			/*case WM_LBUTTONDBLCLK : {
				volatile int ghjg = 0;
				break;
			}
			case WM_NCLBUTTONDBLCLK : {
				volatile int ghjg = 0;
				break;
			}*/
			/*case WM_ACTIVATE : {
				volatile int gjhg = 0;
				break;
			}*/
		}

		return ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
	}

}	// namespace lsw

#include "LSWTreeListView.h"
#include "../Base/LSWBase.h"
#include <strsafe.h>
#include <uxtheme.h>
#include <windowsx.h>

namespace lsw {

	// == Members.
	// Window property.
	WCHAR CTreeListView::m_szProp[2] = { 0 };

	CTreeListView::CTreeListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CListView( _wlLayout.ChangeClass( WC_LISTVIEWW ).AddStyleEx( LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER ).AddStyle( LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_OWNERDATA ),
			_pwParent, _bCreateWidget, _hMenu ),
		/*m_hListView( NULL ),*/
		m_wpListViewProc( nullptr ),
		m_ptIndexCache( nullptr ),
		m_stIndexCache( 0 ) {
		
		if ( !m_szProp[0] ) {
			m_szProp[0] = L'A' + ((reinterpret_cast<UINT_PTR>(_pwParent) >> 2) & 0x0F);
		}
	}
	CTreeListView::~CTreeListView() {
		//::DestroyWindow( m_hListView );
	}

	// == Functions.
	// Setting the HWND after the control has been created.
	void CTreeListView::InitControl( HWND _hWnd ) {
		::SetWindowLongPtrW( Wnd(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this) );
		CListView::InitControl( _hWnd );
		//m_wpListViewProc = CHelpers::SetWndProc( Wnd(), ListViewOverride );
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
	}

	// Inserts an item.
	HTREEITEM CTreeListView::InsertItem( const TVINSERTSTRUCTW * _pisStruct ) {
		ee::CTree<LSW_TREE_ROW> * pntParent = TreeItemToPointer( _pisStruct->hParent );
		if ( !pntParent ) { return nullptr; }
		size_t stIdx = TreeItemToIndex( pntParent, _pisStruct->hInsertAfter, _pisStruct->itemex.pszText );

		if ( pntParent->InsertChild( ItemExToTreeRow( _pisStruct->itemex ), stIdx ) ) {
			SetItemCount( static_cast<INT>(CountExpanded()) );
			m_ptIndexCache = nullptr;
			m_stIndexCache = 0;
			return PointerToTreeItem( pntParent->GetChild( stIdx ) );
		}
		return nullptr;
	}

	// Sets the text for an item.
	BOOL CTreeListView::SetItemText( HTREEITEM _tiItem, const WCHAR * _pwcText, INT _iColumn ) {
		if ( _iColumn >= GetColumnCount() ) { return FALSE; }
		ee::CTree<LSW_TREE_ROW> * pntItem = TreeItemToPointer( _tiItem );
		if ( !pntItem ) { return FALSE; }
		while ( pntItem->Value().vStrings.size() <= _iColumn ) {
			pntItem->Value().vStrings.push_back( L"" );
		}
		pntItem->Value().vStrings[_iColumn] = _pwcText;
		return TRUE;
	}

	// Gets the text of an item.
	BOOL CTreeListView::GetItemText( HTREEITEM _tiItem, INT _iColumn, std::wstring &_sRet ) const {
		if ( _iColumn >= GetColumnCount() ) { return FALSE; }
		const ee::CTree<LSW_TREE_ROW> * pntItem = TreeItemToPointer( _tiItem );
		if ( !pntItem ) { return FALSE; }
		if ( _iColumn < pntItem->Value().vStrings.size() ) {
			_sRet = pntItem->Value().vStrings[_iColumn];
		}
		else {
			_sRet = L"";
		}
		return TRUE;
	}

	// Sets the lParam of an item.
	BOOL CTreeListView::SetItemLParam( HTREEITEM _tiItem, LPARAM _lpParam ) {
		ee::CTree<LSW_TREE_ROW> * pntItem = TreeItemToPointer( _tiItem );
		if ( !pntItem ) { return FALSE; }
		pntItem->Value().lpParam = _lpParam;
		return TRUE;
	}

	// Gets the lParam of an item.
	LPARAM CTreeListView::GetItemLParam( HTREEITEM _tiItem ) const {
		const ee::CTree<LSW_TREE_ROW> * pntItem = TreeItemToPointer( _tiItem );
		if ( !pntItem ) { return 0; }
		return pntItem->Value().lpParam;
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

	// Requesting information (notification responder).
	BOOL CTreeListView::GetDispInfoNotify( NMLVDISPINFOW * _plvdiInfo ) {
		//m_ptIndexCache = ItemByIndex( _plvdiInfo->item.iItem );
		if ( m_ptIndexCache == nullptr ) {
			m_ptIndexCache = ItemByIndex( _plvdiInfo->item.iItem );
			m_stIndexCache = _plvdiInfo->item.iItem;
		}
		else if ( _plvdiInfo->item.iItem == m_stIndexCache ) {
			
		}
		else if ( _plvdiInfo->item.iItem < m_stIndexCache ) {
			m_ptIndexCache = ItemByIndex( _plvdiInfo->item.iItem );
			m_stIndexCache = _plvdiInfo->item.iItem;
		}
		else {
			m_ptIndexCache = ItemByIndex( m_ptIndexCache, _plvdiInfo->item.iItem, m_stIndexCache );
			m_stIndexCache = _plvdiInfo->item.iItem;
		}

		if ( !m_ptIndexCache ) { return FALSE; }

		INT iMask = _plvdiInfo->item.mask;
		if ( (iMask & LVIF_TEXT) && _plvdiInfo->item.cchTextMax >= 1 ) {
			if ( _plvdiInfo->item.iSubItem >= m_ptIndexCache->Value().vStrings.size() ) {
				std::swprintf( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax, L"" );
			}
			else {
				//std::swprintf( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax, L"%s", m_ptIndexCache->Value().vStrings[_plvdiInfo->item.iSubItem] );
				//::wcsncat_s( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax, m_ptIndexCache->Value().vStrings[_plvdiInfo->item.iSubItem].c_str(), m_ptIndexCache->Value().vStrings[_plvdiInfo->item.iSubItem].size() );
				LPWSTR strCopy = _plvdiInfo->item.pszText;
				if ( _plvdiInfo->item.iSubItem == 0 ) {
					LPWSTR strEnd = _plvdiInfo->item.pszText + _plvdiInfo->item.cchTextMax - 1;
					for ( INT I = (GetIndent( m_ptIndexCache ) + 1) * 4; strCopy < strEnd && I--; ) {
						(*strCopy++) = L' ';
						--_plvdiInfo->item.cchTextMax;
					}
				}
				std::wcsncpy( strCopy, m_ptIndexCache->Value().vStrings[_plvdiInfo->item.iSubItem].c_str(), _plvdiInfo->item.cchTextMax );
				strCopy[_plvdiInfo->item.cchTextMax-1] = L'\0';
			}
		}
		if ( iMask & LVIF_INDENT ) {
			_plvdiInfo->item.iIndent = GetIndent( m_ptIndexCache ) + 1;
		}
		return TRUE;
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

	// Converts a TVITEMEXW to a LSW_TREE_ROW.
	CTreeListView::LSW_TREE_ROW CTreeListView::ItemExToTreeRow( const TVITEMEXW &_ieItemEx ) {
		LSW_TREE_ROW trRow;
		if ( _ieItemEx.mask & TVIF_IMAGE ) {
			trRow.iImage = _ieItemEx.iImage;
		}
		if ( _ieItemEx.mask & TVIF_PARAM ) {
			trRow.lpParam = _ieItemEx.lParam;
		}
		if ( _ieItemEx.mask & TVIF_SELECTEDIMAGE ) {
			trRow.iSelectedImage = _ieItemEx.iSelectedImage;
		}
		if ( _ieItemEx.mask & TVIF_STATE ) {
			trRow.uiState = (_ieItemEx.state & _ieItemEx.stateMask) | (trRow.uiState & ~_ieItemEx.stateMask);
		}
		if ( _ieItemEx.mask & TVIF_STATEEX ) {
			trRow.uiStateEx = _ieItemEx.uStateEx;
		}
		if ( _ieItemEx.mask & TVIF_TEXT ) {
			trRow.vStrings.push_back( std::wstring( _ieItemEx.pszText, _ieItemEx.cchTextMax ) );
		}
		return trRow;
	}

	// Converts an HTREEITEM to an object pointer.
	ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::TreeItemToPointer( HTREEITEM _htiItem ) {
		if ( _htiItem == TVI_ROOT ) { return &m_tRoot; }
		return reinterpret_cast<ee::CTree<LSW_TREE_ROW> *>(_htiItem);
	}

	// Converts an HTREEITEM to an object pointer.
	const ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::TreeItemToPointer( HTREEITEM _htiItem ) const {
		if ( _htiItem == TVI_ROOT ) { return &m_tRoot; }
		return reinterpret_cast<ee::CTree<LSW_TREE_ROW> *>(_htiItem);
	}

	// Converts an HTREEITEM to an index.
	size_t CTreeListView::TreeItemToIndex( ee::CTree<LSW_TREE_ROW> * _ptParent, HTREEITEM _htiItem, const wchar_t * _pwcSortText ) {
		if ( _htiItem == TVI_FIRST ) { return 0; }
		if ( _htiItem == TVI_LAST ) { return _ptParent->Size(); }
		if ( _htiItem == TVI_SORT ) {
			if ( nullptr == _pwcSortText ) { return 0; }
			for ( size_t I = 0; I < _ptParent->Size(); ++I ) {
				if ( _ptParent->GetChild( I )->Value() < _pwcSortText ) { continue; }
				return I;
			}
			return _ptParent->Size();
		}
		// _htiItem must be an item parented by _ptParent.
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptItem = TreeItemToPointer( _htiItem );
		for ( auto I = _ptParent->Size(); I--; ) {
			if ( _ptParent->GetChild( I ) == ptItem ) { return I; }
		}
		// Hmm.
		return _ptParent->Size();
	}

	// Converts an object pointer to an HTREEITEM.
	HTREEITEM CTreeListView::PointerToTreeItem( ee::CTree<LSW_TREE_ROW> * _ptObj ) {
		if ( _ptObj == &m_tRoot ) { return TVI_ROOT; }
		return reinterpret_cast<HTREEITEM>(_ptObj);
	}

	// Gets an item by index accounting for children being expanded or not.
	ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::ItemByIndex( size_t _stIdx ) {
		return ItemByIndex( nullptr, _stIdx, 0 );
	}

	// Gets an item by index accounting for children being expanded or not.
	ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::ItemByIndex( ee::CTree<LSW_TREE_ROW> * _ptStart, size_t _stIdx, size_t _stStartIdx ) {
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptThis = _ptStart;
		if ( !ptThis ) {
			ptThis = NextByExpansion( ptThis );
			_stStartIdx = 0;
		}
		while ( _stStartIdx++ < _stIdx && ptThis ) {
			ptThis = NextByExpansion( ptThis );
		}
		return ptThis;
	}

	// Counts the total number of expanded items.
	size_t CTreeListView::CountExpanded() const {
		size_t stCnt = 0;
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptTmp = nullptr;
		while ( (ptTmp = NextByExpansion( ptTmp )) ) {
			++stCnt;
		}
		return stCnt;
	}

	// Gets the indentation level for an item.
	INT CTreeListView::GetIndent( ee::CTree<CTreeListView::LSW_TREE_ROW> * _ptThis ) const {
		INT iLevel = 0;
		while ( _ptThis->Parent() ) {
			_ptThis = _ptThis->Parent();
			++iLevel;
		}
		return iLevel;
	}

	// Gets the next item based on expansion.
	ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::NextByExpansion( ee::CTree<CTreeListView::LSW_TREE_ROW> * _ptThis ) const {
		if ( !_ptThis ) {
			if ( !m_tRoot.Size() ) { return nullptr; }
			return const_cast<ee::CTree<CTreeListView::LSW_TREE_ROW> *>(m_tRoot.GetChild( 0 ));
		}
		if ( (_ptThis->Value().uiState & TVIS_EXPANDED) && _ptThis->Size() ) {
			// Go to its children.
			return _ptThis->GetChild( 0 );
		}
		// Item not expanded.  Go to the next in line.
		if ( _ptThis->Next() ) { return _ptThis->Next(); }

		// Nothing after this.  Go to the parent.
		while ( _ptThis->Parent() ) {
			_ptThis = _ptThis->Parent();
			if ( _ptThis->Next() ) { return _ptThis->Next(); }
		}
		// That's the end.
		return nullptr;
	}

	// WM_SIZE.
	CWidget::LSW_HANDLED CTreeListView::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		CWidget::Size( _wParam, _lWidth, _lHeight );
		//::ShowScrollBar( Wnd(), SB_VERT, FALSE );
		//ResizeControls( VirtualClientRect( nullptr ) );

		return LSW_H_CONTINUE;
	}

	// WM_MOVE.
	CWidget::LSW_HANDLED CTreeListView::Move( LONG _lX, LONG _lY ) {
		CWidget::Move( _lX, _lY );
		//ResizeControls( VirtualClientRect( nullptr ) );

		return LSW_H_CONTINUE;
	}

	// Evaluates expressions to determine a new rectangle for the control.
	void CTreeListView::EvalNewSize() {
		CWidget::EvalNewSize();
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
			/*case WM_PAINT : {
				HTHEME hTheme = ::OpenThemeData( _hWnd, L"TREEVIEW" );
				if ( NULL != hTheme ) {
					::CloseThemeData( hTheme );
				}
				break;
			}*/
		}
		if ( wpOrig ) {
			return ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
		}
		else {
			return 0;
		}
	}

}	// namespace lsw

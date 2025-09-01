#include "LSWTreeListView.h"
#include "../Base/LSWBase.h"

#include <set>
#include <strsafe.h>
#include <vsstyle.h>
#include <uxtheme.h>
#include <windowsx.h>

namespace lsw {

	// == Members.
	// Window property.
	WCHAR CTreeListView::m_szProp[2] = { 0 };


	CTreeListView::CTreeListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CListView( _wlLayout.ChangeClass( WC_LISTVIEWW ).AddStyleEx( LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER ).AddStyle( LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_OWNERDATA ),
			_pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		/*m_hListView( NULL ),*/
		m_wpListViewProc( nullptr ),
		m_ptIndexCache( nullptr ),
		m_stIndexCache( 0 ),
		m_stHotItem( size_t( -1 ) ),
		m_lSpaceWidth( 0 ),
		m_bDontUpdate( false ) {
		
		if ( !m_szProp[0] ) {
			m_szProp[0] = L'A' + ((reinterpret_cast<UINT_PTR>(_pwParent) >> 2) & 0x0F);
		}
	}
	CTreeListView::~CTreeListView() {
		//::DestroyWindow( m_hListView );
	}

	// == Functions.
	/**
	 * Setting the HWND after the control has been created.
	 *
	 * \param _hWnd The handle to the window/control being initialized.
	 */
	void CTreeListView::InitControl( HWND _hWnd ) {
		CListView::InitControl( _hWnd );
		//::SetWindowLongPtrW( Wnd(), GWL_STYLE, LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT | LVS_OWNERDATA );
		::SetWindowLongPtrW( Wnd(), GWL_EXSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER );
		::SendMessageW( Wnd(), LVM_SETVIEW, LV_VIEW_DETAILS, 0 );
		m_wpListViewProc = CHelpers::SetWndProc( Wnd(), ListViewOverride );
		::SetPropW( Wnd(), m_szProp, reinterpret_cast<HANDLE>(this) );

		// By default there is already a header.
		InsertColumn( L"", 150, 0 );
	}

	/**
	 * Inserts an item.
	 *
	 * \param _pisStruct The insertion data for the item being inserted.
	 * \return Returns a handle to the inserted item.
	 */
	HTREEITEM CTreeListView::InsertItem( const TVINSERTSTRUCTW * _pisStruct ) {
		ee::CTree<LSW_TREE_ROW> * pntParent = TreeItemToPointer( _pisStruct->hParent );
		if ( !pntParent ) { return nullptr; }
		size_t stIdx = TreeItemToIndex( pntParent, _pisStruct->hInsertAfter, _pisStruct->itemex.pszText );

		if ( pntParent->InsertChild( ItemExToTreeRow( _pisStruct->itemex ), stIdx ) ) {
			if ( !m_bDontUpdate ) {
				UpdateListView();
			}
			ClearCache();
			return PointerToTreeItem( pntParent->GetChild( stIdx ) );
		}
		return nullptr;
	}

	/**
	 * Sets the text for an item.
	 *
	 * \param _tiItem Handle to the item to modify.
	 * \param _pwcText The new text value if the item.
	 * \param _iColumn The cloumn of text to set.
	 * \return Returns TRUE if the item's text was set.  FALSE indicates that the column was out-of-range or that the item was invalid.
	 */
	BOOL CTreeListView::SetItemText( HTREEITEM _tiItem, const WCHAR * _pwcText, INT _iColumn ) {
		if ( _iColumn >= GetColumnCount() ) { return FALSE; }
		ee::CTree<LSW_TREE_ROW> * pntItem = TreeItemToPointer( _tiItem );
		if ( !pntItem ) { return FALSE; }
		while ( pntItem->Value().vStrings.size() <= size_t( _iColumn ) && _iColumn >= 0 ) {
			pntItem->Value().vStrings.push_back( L"" );
		}
		pntItem->Value().vStrings[_iColumn] = _pwcText;
		return TRUE;
	}

	/**
	 * Gets the text of an item.
	 *
	 * \param _tiItem Handle to the item.
	 * \param _iColumn The index of the column whose text is to be gotten.
	 * \param _sRet Holds the returned text value.
	 * \return Returns TRUE if the item's text was obtained.  FALSE indicates that the column was out-of-range or that the item was invalid.
	 */
	BOOL CTreeListView::GetItemText( HTREEITEM _tiItem, INT _iColumn, std::wstring &_sRet ) const {
		if ( _iColumn >= GetColumnCount() ) { return FALSE; }
		const ee::CTree<LSW_TREE_ROW> * pntItem = TreeItemToPointer( _tiItem );
		if ( !pntItem ) { return FALSE; }
		if ( size_t( _iColumn ) < pntItem->Value().vStrings.size() ) {
			_sRet = pntItem->Value().vStrings[_iColumn];
		}
		else {
			_sRet = L"";
		}
		return TRUE;
	}

	/**
	 * Sets the lParam of an item.
	 *
	 * \param _tiItem Handle to the item to modify.
	 * \param _lpParam The value to set.
	 * \return Returns TRUE if the item's LPARAM was set.  FALSE indicates that the item was invalid.
	 */
	BOOL CTreeListView::SetItemLParam( HTREEITEM _tiItem, LPARAM _lpParam ) {
		ee::CTree<LSW_TREE_ROW> * pntItem = TreeItemToPointer( _tiItem );
		if ( !pntItem ) { return FALSE; }
		pntItem->Value().lpParam = _lpParam;
		return TRUE;
	}

	/**
	 * Gets the lParam of an item.
	 *
	 * \param _tiItem Handle to the item.
	 * \return Returns the item's LPARAM value or 0 if the item is invalid.
	 */
	LPARAM CTreeListView::GetItemLParam( HTREEITEM _tiItem ) const {
		const ee::CTree<LSW_TREE_ROW> * pntItem = TreeItemToPointer( _tiItem );
		if ( !pntItem ) { return 0; }
		return pntItem->Value().lpParam;
	}

	/**
	 * Sets an itemÅfs color.
	 * 
	 * \param _tiItem The item whose color is to be updated.
	 * \param _rgbColor The color to apply to the item (alpha respected).
	 * \return Returns TRUE if the itemÅfs color was set.  FALSE indicates that the item was invalid.
	 **/
	BOOL CTreeListView::SetItemColor( HTREEITEM _tiItem, RGBQUAD _rgbColor ) {
		ee::CTree<LSW_TREE_ROW> * pntItem = TreeItemToPointer( _tiItem );
		if ( !pntItem ) { return FALSE; }
		pntItem->Value().rgbColor = _rgbColor;
		return TRUE;
	}

	/**
	 * Gets an item given an LPARAM.  Only the first item with the given LPARAM is returned.
	 * 
	 * \param _lpValue The LPARAM value to find on an item.
	 * \return Returns the item with the given LPARAM value or NULL.
	 **/
	HTREEITEM CTreeListView::GetByLParam( LPARAM _lpValue ) {
		for ( auto I = m_tRoot.Size(); I--; ) {
			if ( m_tRoot.GetChild( I )->Value().lpParam == _lpValue ) {
				return PointerToTreeItem( m_tRoot.GetChild( I ) );
			}
		}
		return NULL;
	}

	/**
	 * Counts the total number of expanded items.
	 *
	 * \return Returns the total number of items, accounting for expandedness.
	 */
	size_t CTreeListView::CountExpanded() const {
		size_t stCnt = 0;
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptTmp = nullptr;
		while ( (ptTmp = NextByExpansion( ptTmp )) != nullptr ) {
			++stCnt;
		}
		return stCnt;
	}

	/**
	 * Deletes all items with the given LPARAM value.
	 * 
	 * \param _lpParm The LPARAM value to find and delete.
	 **/
	VOID CTreeListView::DeleteByLParam( LPARAM _lpParm ) {
		ClearCache();
		for ( auto I = m_tRoot.Size(); I--; ) {
			if ( m_tRoot.GetChild( I )->Value().lpParam == _lpParm ) {
				m_tRoot.RemoveChild( I );
			}
		}
		UpdateListView();
	}

	/**
	 * Deletes all items.
	 */
	VOID CTreeListView::DeleteAll() {
		ClearCache();
		CListView::DeleteAll();
		m_tRoot.RemoveAllChildren();
	}

	/**
	 * Moves items up one index.  Items are found by their LPARAM values.
	 * 
	 * \param _vItems The array of LPARAM items to move up 1 index.
	 **/
	VOID CTreeListView::MoveUp( const std::vector<LPARAM> &_vItems ) {
		auto sSet = std::set<LPARAM>( _vItems.begin(), _vItems.end() );

		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = &m_tRoot;
		ClearCache();
		MoveUp( ptNode, sSet );
		UpdateListView();
	}

	/**
	 * Moves items down one index.  Items are found by their LPARAM values.
	 * 
	 * \param _vItems The array of LPARAM items to move down 1 index.
	 **/
	VOID CTreeListView::MoveDown( const std::vector<LPARAM> &_vItems ) {
		auto sSet = std::set<LPARAM>( _vItems.begin(), _vItems.end() );

		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = &m_tRoot;
		ClearCache();
		MoveDown( ptNode, sSet );
		UpdateListView();
	}

	/**
	 * Selects all root-level items.
	 * 
	 * \return Returns the number of items selected.
	 **/
	size_t CTreeListView::SelectRootItems() {
		ClearCache();
		size_t sCnt = 0;
		for ( auto I = m_tRoot.Size(); I--; ) {
			m_tRoot.GetChild( I )->Value().uiState |= TVIS_SELECTED;
			++sCnt;
		}

		UpdateListView();
		return sCnt;
	}

	/**
	 * Sets the selection based on item data.
	 * 
	 * \param _pData The LPARAM value to use to decide on selection.
	 * \return Returns the number of items selected.
	 **/
	INT CTreeListView::SetCurSelByItemData( LPARAM _pData ) {
		ClearCache();
		size_t sCnt = 0;
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		while ( ptNode ) {
			if ( ptNode->Value().lpParam == _pData ) {
				ptNode->Value().uiState |= TVIS_SELECTED;
				++sCnt;
			}
			ptNode = Next( ptNode );
		}

		UpdateListView();
		return INT( sCnt );
	}

	/**
	 * Unselects all items.
	 **/
	void CTreeListView::UnselectAll() {
		ClearCache();
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		while ( ptNode ) {
			ptNode->Value().uiState &= ~TVIS_SELECTED;
			ptNode = Next( ptNode );
		}
		UpdateListView();
	}

	/**
	 * Returns true if any of the selected items have children and are not in expanded view.
	 *
	 * \return Returns true if any of the selected items have children and are not in expanded view.
	 */
	bool CTreeListView::AnySelectedHasUnexpandedChildren() const {
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		while ( ptNode ) {
			if ( ptNode->Value().uiState & TVIS_SELECTED ) {
				if ( !(ptNode->Value().uiState & TVIS_EXPANDED) && ptNode->Size() ) { return true; }
			}
			ptNode = Next( ptNode );
		}
		return false;
	}

	/**
	 * Returns true if any of the selected items have children and are in expanded view.
	 *
	 * \return Returns true if any of the selected items have children and are in expanded view.
	 */
	bool CTreeListView::AnySelectedHasExpandedChildren() const {
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		while ( ptNode ) {
			if ( ptNode->Value().uiState & TVIS_SELECTED ) {
				if ( (ptNode->Value().uiState & TVIS_EXPANDED) && ptNode->Size() ) { return true; }
			}
			ptNode = Next( ptNode );
		}
		return false;
	}

	/**
	 * Returns true if any of the items have children and are not in expanded view.
	 *
	 * \return Returns true if any of the items have children and are not in expanded view.
	 */
	bool CTreeListView::AnyHasUnexpandedChildren() const {
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		while ( ptNode ) {
			if ( !(ptNode->Value().uiState & TVIS_EXPANDED) && ptNode->Size() ) { return true; }
			ptNode = Next( ptNode );
		}
		return false;
	}

	/**
	 * Returns true if any of the items have children and are in expanded view.
	 *
	 * \return Returns true if any of the items have children and are in expanded view.
	 */
	bool CTreeListView::AnyHasExpandedChildren() const {
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		while ( ptNode ) {
			if ( (ptNode->Value().uiState & TVIS_EXPANDED) && ptNode->Size() ) { return true; }
			ptNode = Next( ptNode );
		}
		return false;
	}

	/**
	 * Expands selected items.
	 */
	void CTreeListView::ExpandSelected() {
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		bool bFound = false;
		while ( ptNode ) {
			if ( ptNode->Value().uiState & TVIS_SELECTED ) {
				if ( !(ptNode->Value().uiState & TVIS_EXPANDED) ) {
					if ( ptNode->Size() ) {
						bFound = true;
					}
					ptNode->Value().uiState |= TVIS_EXPANDED;
				}
			}
			ptNode = Next( ptNode );
		}
		if ( bFound ) {
			UpdateListView();
		}
	}

	/**
	 * Expands all items.
	 */
	void CTreeListView::ExpandAll() {
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		bool bFound = false;
		while ( ptNode ) {
			if ( !(ptNode->Value().uiState & TVIS_EXPANDED) ) {
				if ( ptNode->Size() ) {
					bFound = true;
				}
				ptNode->Value().uiState |= TVIS_EXPANDED;
			}
			ptNode = Next( ptNode );
		}
		if ( bFound ) {
			UpdateListView();
		}
	}

	/**
	 * Collapses selected items.
	 *
	 * \return Collapses selected items.
	 */
	void CTreeListView::CollapseSelected() {
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		bool bFound = false;
		while ( ptNode ) {
			if ( ptNode->Value().uiState & TVIS_SELECTED ) {
				if ( (ptNode->Value().uiState & TVIS_EXPANDED) ) {
					if ( ptNode->Size() ) {
						bFound = true;
					}
					ptNode->Value().uiState &= ~TVIS_EXPANDED;
				}
			}
			ptNode = Next( ptNode );
		}
		if ( bFound ) {
			UpdateListView();
		}
	}

	/**
	 * Collapses all items.
	 *
	 * \return Collapses all items.
	 */
	void CTreeListView::CollapseAll() {
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		bool bFound = false;
		while ( ptNode ) {
			if ( (ptNode->Value().uiState & TVIS_EXPANDED) ) {
				if ( ptNode->Size() ) {
					bFound = true;
				}
				ptNode->Value().uiState &= ~TVIS_EXPANDED;
			}
			ptNode = Next( ptNode );
		}
		if ( bFound ) {
			UpdateListView();
		}
	}

	/**
	 * Gathers the selected items into a vector.
	 *
	 * \param _vReturn The array into which to gather the selected items.
	 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
	 * \return Returns the number of items gathered.
	 */
	size_t CTreeListView::GatherSelected( std::vector<HTREEITEM> &_vReturn, bool _bIncludeNonVisible ) const {
		_vReturn.clear();
		GatherSelected( TVI_ROOT, _vReturn, _bIncludeNonVisible );
		return _vReturn.size();
	}

	/**
	 * Gathers the selected item indices into a vector.
	 *
	 * \param _vReturn The array into which to gather the selected items.
	 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
	 * \return Returns the number of items gathered.
	 */
	size_t CTreeListView::GatherSelected( std::vector<size_t> &_vReturn, bool _bIncludeNonVisible ) const {
		_vReturn.clear();
		size_t stIdx = size_t( -1 );
		GatherSelected( TVI_ROOT, _vReturn, _bIncludeNonVisible, stIdx );
		return _vReturn.size();
	}

	/**
	 * Gathers the selected item LPARAM values into a vector.
	 *
	 * \param _vReturn The array into which to gather the selected items.
	 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
	 * \return Returns the number of items gathered.
	 */
	size_t CTreeListView::GatherSelectedLParam( std::vector<LPARAM> &_vReturn, bool _bIncludeNonVisible ) const {
		_vReturn.clear();
		size_t stIdx = size_t( -1 );
		GatherSelectedLParam( TVI_ROOT, _vReturn, _bIncludeNonVisible, stIdx );
		return _vReturn.size();
	}

	/**
	 * Gathers all LPARAM values of every tree item into an array.
	 * 
	 * \param _vReturn The array into which to gather the return values.
	 * \param _bIncludeNonVisible If true, non-expanded items are also searched.
	 * \return Returns the total number of items gathered (_vReturn.size()).
	 **/
	size_t CTreeListView::GatherAllLParam( std::vector<LPARAM> &_vReturn, bool _bIncludeNonVisible ) const {
		_vReturn.clear();
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		while ( ptNode ) {
			_vReturn.push_back( ptNode->Value().lpParam );
			ptNode = _bIncludeNonVisible ? Next( ptNode ) : NextByExpansion( ptNode );
		}
		return _vReturn.size();
	}

	/**
	 * Gathers all LPARAM values of every tree item visible within the control view space into an array.
	 * 
	 * \param _vReturn The array into which to gather the return values.
	 * \return Returns the total number of items gathered (_vReturn.size()).
	 **/
	size_t CTreeListView::GatherInViewLParam( std::vector<LPARAM> &_vReturn ) const {
		try {
			_vReturn.clear();
			int I = -1;
			while ( (I = ListView_GetNextItem(
				Wnd(),
				I,
				LVNI_ALL | LVNI_VISIBLEONLY )) != -1 ) {
				ee::CTree<LSW_TREE_ROW> * ptNode = ItemByIndex_Cached( I );
				if ( ptNode ) {
					_vReturn.push_back( ptNode->Value().lpParam );
				}
			}
		}
		catch ( ... ) {}
		return _vReturn.size();
	}

	/**
	 * Gets the index of the highlighted item or returns size_t( -1 ).
	 *
	 * \return Returs the index of the highlighted item or size_t( -1 ) if there is none.
	 */
	size_t CTreeListView::FindHighlighted() const {
		ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = Next( nullptr );
		size_t stIdx = 0;
		while ( ptNode ) {
			if ( ptNode->Value().uiState & LSW_TREELIST_HIGHLIGHTED ) {
				return stIdx;
			}
			ptNode = NextByExpansion( ptNode );
			++stIdx;
		}
		return size_t( -1 );
	}

	/**
	 * Determines if the given tree item is hidden.
	 *
	 * \param _hiItem The item to check for being hidden
	 * \return Returns true if any parent of the item is collapsed.
	 */
	bool CTreeListView::IsHidden( HTREEITEM _hiItem ) const {
		const ee::CTree<CTreeListView::LSW_TREE_ROW> * ptNode = TreeItemToPointer( _hiItem );
		if ( ptNode ) { ptNode = ptNode->Parent(); }
		while ( ptNode ) {
			// Children of the root can't be hidden.
			if ( !ptNode->Parent() ) { return false; }
			// This parent is not directly under the root.  If it is collapsed then everything under it is hidden.
			if ( !(ptNode->Value().uiState & TVIS_EXPANDED) ) { return true; }
			ptNode = ptNode->Parent();
		}
		return false;
	}

	/**
	 * Requesting information (notification responder).
	 *
	 * \param _plvdiInfo Pointer to the display-info structure.
	 * \return Returns TRUE if the requested information could be obtained.  FALSE indicates an internal error.
	 */
	BOOL CTreeListView::GetDispInfoNotify( NMLVDISPINFOW * _plvdiInfo ) {
		try {
			m_ptIndexCache = ItemByIndex_Cached( _plvdiInfo->item.iItem );
			if ( !m_ptIndexCache ) { return FALSE; }

			INT iMask = _plvdiInfo->item.mask;
			auto iMax = _plvdiInfo->item.cchTextMax;
			if ( (iMask & LVIF_TEXT) && iMax >= 1 ) {
				LPWSTR strCopy = _plvdiInfo->item.pszText;
				if ( _plvdiInfo->item.iSubItem == 0 ) {
					for ( INT I = (GetIndent( m_ptIndexCache ) + 1) * 4; iMax > 1 && I--; ) {
						(*strCopy++) = L' ';
						--iMax;
					}
				}
				if ( iMax ) {
					std::wstring wsTmp = GetCellText( _plvdiInfo->item.iItem, _plvdiInfo->item.iSubItem, m_ptIndexCache );
					std::wcsncpy( strCopy, wsTmp.c_str(), iMax );
					strCopy[iMax-1] = L'\0';
					/*auto pwsSrc = CWidget::Parent() ? CWidget::Parent()->TreeListView_ItemText( this, _plvdiInfo->item.iItem, _plvdiInfo->item.iSubItem, m_ptIndexCache->Value().lpParam, wsTmp ) : nullptr;
					if ( !pwsSrc && size_t( _plvdiInfo->item.iSubItem ) < m_ptIndexCache->Value().vStrings.size() ) {
						pwsSrc = &m_ptIndexCache->Value().vStrings[_plvdiInfo->item.iSubItem];
					}
					if ( !pwsSrc ) {
						strCopy[0] = L'\0';
					}
					else {
						std::wcsncpy( strCopy, (*pwsSrc).c_str(), iMax );
						strCopy[iMax-1] = L'\0';
					}*/
				}
			}
			if ( iMask & LVIF_INDENT ) {
				_plvdiInfo->item.iIndent = GetIndent( m_ptIndexCache ) + 1;
			}
			if ( iMask & LVIF_PARAM ) {
				_plvdiInfo->item.lParam = reinterpret_cast<LPARAM>(m_ptIndexCache);
			}
			/*if ( iMask & LVIF_STATE ) {
				_plvdiInfo->item.state = 2;
			}*/
			return TRUE;
		}
		catch ( ... ) { return FALSE; }
	}

	/**
	 * Sorts items.
	 * 
	 * \param _iSubItem The index of the sub-item.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CTreeListView::SortItems( INT _iSubItem ) {
		if ( !m_bSort ) { return FALSE; }
		try {
			if ( _iSubItem >= m_tRoot.Value().vSortAscNext.size() ) {
				m_tRoot.Value().vSortAscNext.resize( _iSubItem + 1 );
			}
			auto eSortMethod = (_iSubItem < m_vSortMethod.size()) ? m_vSortMethod[_iSubItem] : ee::EE_TP_ORDINAL_CI;
			SortChildren( &m_tRoot, _iSubItem, m_tRoot.Value().vSortAscNext[_iSubItem], eSortMethod );
			m_tRoot.Value().vSortAscNext[_iSubItem] = !m_tRoot.Value().vSortAscNext[_iSubItem];
			UpdateListView();
			return TRUE;
		}
		catch ( ... ) { return FALSE; }
	}

	/**
	 * A helper to easily create a tree view item to be inserted with only text.
	 *
	 * \param _pwcText The item text.
	 * \param _tiParent The item parent.
	 * \param _tiInsertAfter The item's insertion point
	 * \return Returns a default tree item with the given common parameters set.
	 */
	TVINSERTSTRUCTW CTreeListView::DefaultItem( const WCHAR * _pwcText, HTREEITEM _tiParent, HTREEITEM _tiInsertAfter ) {
		TVINSERTSTRUCTW isItem = { 0 };
		isItem.hParent = _tiParent;
		isItem.hInsertAfter = _tiInsertAfter;
		isItem.item.mask = TVIF_TEXT;
		isItem.item.pszText = const_cast<LPWSTR>(_pwcText);
		isItem.item.cchTextMax = static_cast<int>(std::wcslen( _pwcText ));
		return isItem;
	}

	/**
	 * A helper to easily create a tree view item to be inserted with only text and data.
	 *
	 * \param _pwcText The item text.
	 * \param _lpParm The item's LPARAM.
	 * \param _tiParent The item parent.
	 * \param _tiInsertAfter The item's insertion point
	 * \return Returns a default tree item with the given common parameters set.
	 */
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

	/**
	 * Converts an HTREEITEM to an object pointer.
	 *
	 * \param _htiItem The item handle to convert.
	 * \return Returns the converted pointer.
	 */
	ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::TreeItemToPointer( HTREEITEM _htiItem ) {
		if ( _htiItem == TVI_ROOT ) { return &m_tRoot; }
		return reinterpret_cast<ee::CTree<LSW_TREE_ROW> *>(_htiItem);
	}

	/**
	 * Converts an HTREEITEM to a constant object pointer.
	 *
	 * \param _htiItem The item handle to convert.
	 * \return Returns the converted pointer.
	 */
	const ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::TreeItemToPointer( HTREEITEM _htiItem ) const {
		if ( _htiItem == TVI_ROOT ) { return &m_tRoot; }
		return reinterpret_cast<ee::CTree<LSW_TREE_ROW> *>(_htiItem);
	}

	/**
	 * Converts an HTREEITEM to an index.
	 *
	 * \param _ptParent The parent item.
	 * \param _htiItem The item whose index into _ptParent's children is to be obtained.
	 * \param _pwcSortText Sort text if _htiItem is TVI_SORT.
	 * \return Returns the inde of the item.
	 */
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

	/**
	 * Converts an object pointer to an HTREEITEM.
	 *
	 * \param _ptObj The object pointer to cnvert to a handle.
	 * \return Returns the obect pointer converted to a handle.
	 */
	HTREEITEM CTreeListView::PointerToTreeItem( ee::CTree<LSW_TREE_ROW> * _ptObj ) const {
		if ( _ptObj == &m_tRoot ) { return TVI_ROOT; }
		return reinterpret_cast<HTREEITEM>(_ptObj);
	}

	/**
	 * Converts an object pointer to an HTREEITEM.
	 *
	 * \param _ptObj The object pointer to cnvert to a handle.
	 * \return Returns the obect pointer converted to a handle.
	 */
	HTREEITEM CTreeListView::PointerToTreeItem( const ee::CTree<LSW_TREE_ROW> * _ptObj ) const {
		if ( _ptObj == &m_tRoot ) { return TVI_ROOT; }
		return reinterpret_cast<HTREEITEM>(const_cast<ee::CTree<LSW_TREE_ROW> *>(_ptObj));
	}

	/**
	 * Gets an item by index accounting for children being expanded or not.
	 *
	 * \param _stIdx The index of the item to get with collapsed items being taken into account.
	 * \return Returns the _stIdx'th item in the tree, accounting for expandedness.
	 */
	ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::ItemByIndex( size_t _stIdx ) const {
		return ItemByIndex( nullptr, _stIdx, 0 );
	}

	/**
	 * Gets an item by index accounting for children being expanded or not.
	 *
	 * \param _stIdx The index of the item to get with collapsed items being taken into account.
	 * \return Returns the _stIdx'th item in the tree, accounting for expandedness.
	 */
	ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::ItemByIndex( ee::CTree<LSW_TREE_ROW> * _ptStart, size_t _stIdx, size_t _stStartIdx ) const {
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

	/**
	 * Gets an item by index accounting for children being expanded or not, using cached internal values to speed up the process.
	 *
	 * \param _stIdx The index of the item to get with collapsed items being taken into account.
	 * \return Returns the _stIdx'th item in the tree, accounting for expandedness.
	 */
	ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::ItemByIndex_Cached( size_t _stIdx ) const {
		if ( m_ptIndexCache == nullptr ) {
			m_ptIndexCache = ItemByIndex( _stIdx );
			m_stIndexCache = _stIdx;
		}
		else if ( _stIdx == m_stIndexCache ) {
			
		}
		else if ( _stIdx < m_stIndexCache ) {
			m_ptIndexCache = ItemByIndex( _stIdx );
			m_stIndexCache = _stIdx;
		}
		else {
			m_ptIndexCache = ItemByIndex( m_ptIndexCache, _stIdx, m_stIndexCache );
			m_stIndexCache = _stIdx;
		}
		return m_ptIndexCache;
	}

	/**
	 * Deletes a column from the given element and all of its children.
	 * 
	 * \param PARM The element from which to delete a column.
	 * \param PARM the column to delete.
	 **/
	void CTreeListView::DeleteColumn( ee::CTree<CTreeListView::LSW_TREE_ROW> * _ptThis, INT _iCol ) {
		if ( _ptThis ) {
			if ( _iCol < _ptThis->Value().vSortAscNext.size() ) {
				_ptThis->Value().vSortAscNext.erase( _ptThis->Value().vSortAscNext.begin() + _iCol );
			}
			if ( _iCol < _ptThis->Value().vStrings.size() ) {
				_ptThis->Value().vStrings.erase( _ptThis->Value().vStrings.begin() + _iCol );
			}
			for ( auto I = _ptThis->Size(); I--; ) {
				DeleteColumn( _ptThis->GetChild( I ), _iCol );
			}
		}
	}

	/**
	 * Gets the text for a given item.  The row is counted by skipping over collapsed items, and can refer to different items depending on the collapsed/expanded state of the tree.
	 * 
	 * \param _sRow The index of the item.
	 * \param _Column The column of the item.
	 * \param _ptrRow A pointer to the _sRow'th item.
	 * \return Returns the text of the given cell.
	 **/
	std::wstring CTreeListView::GetCellText( size_t _sRow, size_t _Column, ee::CTree<LSW_TREE_ROW> * _ptrRow ) {
		if ( !_ptrRow ) { return std::wstring(); }
		if ( _sRow > INT_MAX || _Column > INT_MAX ) { return std::wstring(); }

		std::wstring wsTmp;
		auto pwsSrc = CWidget::Parent() ? CWidget::Parent()->TreeListView_ItemText( this, int( _sRow ), int( _Column ), _ptrRow->Value().lpParam, wsTmp ) : nullptr;
		if ( !pwsSrc && size_t( _Column ) < _ptrRow->Value().vStrings.size() ) {
			pwsSrc = &_ptrRow->Value().vStrings[_Column];
		}
		if ( !pwsSrc ) { return std::wstring(); }
		return (*pwsSrc);
	}

	/**
	 * Gets the indentation level for an item.
	 *
	 * \param _ptThis The item whose indentation level is to be obtained.
	 * \return Returns the number of parents on the item.
	 */
	INT CTreeListView::GetIndent( ee::CTree<CTreeListView::LSW_TREE_ROW> * _ptThis ) const {
		INT iLevel = 0;
		while ( _ptThis->Parent() ) {
			_ptThis = _ptThis->Parent();
			++iLevel;
		}
		return iLevel;
	}

	/**
	 * Gets the next item based on expansion.
	 *
	 * \param _ptThis The items whose next item is to be obtained, accounting for expandedness.
	 * \return Returns the next item, accounting for expandedness.
	 */
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

	/**
	 * Gets the next item.
	 *
	 * \param _ptThis The items whose next item is to be obtained.
	 * \return Returns the next item.
	 */
	ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::Next( ee::CTree<CTreeListView::LSW_TREE_ROW> * _ptThis ) const {
		if ( !_ptThis ) {
			if ( !m_tRoot.Size() ) { return nullptr; }
			return const_cast<ee::CTree<CTreeListView::LSW_TREE_ROW> *>(m_tRoot.GetChild( 0 ));
		}
		return ee::CTree<CTreeListView::LSW_TREE_ROW>::Next( _ptThis );
	}

	/**
	 * Moves items up one index.  Items are found by their LPARAM values.
	 * 
	 * \param _ptThis The group of items to possibly move up by 1.
	 * \param _sItems The array of LPARAM items to move up 1 index.
	 **/
	VOID CTreeListView::MoveUp( ee::CTree<LSW_TREE_ROW> * _ptThis, const std::set<LPARAM> &_sItems ) {
		int64_t i64Idx = -2;
		int64_t i64ThisIdx = 0;
		while ( _ptThis ) {
			if ( _ptThis->Size() ) { MoveUp( _ptThis->GetChild( 0 ), _sItems ); }

			if ( std::find( _sItems.begin(), _sItems.end(), _ptThis->Value().lpParam ) != _sItems.end() ) {
				// Can we move this one?  If the previous item was just moved or there is nothing before this one, it canÅft be moved.
				if ( _ptThis->Prev() && i64ThisIdx - i64Idx > 1 ) {
					ee::CTree<LSW_TREE_ROW>::MoveUp( _ptThis );
				}
				i64Idx = i64ThisIdx;
			}

			_ptThis = _ptThis->Next();
			++i64ThisIdx;
		}
	}

	/**
	 * Moves items down one index.  Items are found by their LPARAM values.
	 * 
	 * \param _ptThis The group of items to possibly move down by 1.
	 * \param _sItems The array of LPARAM items to move down 1 index.
	 **/
	VOID CTreeListView::MoveDown( ee::CTree<LSW_TREE_ROW> * _ptThis, const std::set<LPARAM> &_sItems ) {
		int64_t i64Idx = -2;
		int64_t i64ThisIdx = 0;
		// Go to the end of the list.
		while ( _ptThis ) {
			if ( !_ptThis->Next() ) { break; }
			_ptThis = _ptThis->Next();
		}

		while ( _ptThis ) {
			if ( _ptThis->Size() ) { MoveDown( _ptThis->GetChild( 0 ), _sItems ); }

			if ( std::find( _sItems.begin(), _sItems.end(), _ptThis->Value().lpParam ) != _sItems.end() ) {
				// Can we move this one?  If the previous item was just moved or there is nothing before this one, it canÅft be moved.
				if ( _ptThis->Next() && i64ThisIdx - i64Idx > 1 ) {
					ee::CTree<LSW_TREE_ROW>::MoveDown( _ptThis );
				}
				i64Idx = i64ThisIdx;
			}

			_ptThis = _ptThis->Prev();
			++i64ThisIdx;
		}
	}

	/**
	 * Sorts the children of a given item.
	 * 
	 * \param _ptObj The object whose children are to be sorted.
	 * \param _sColumn The column by which to sort rows.
	 * \param _bAsc Ascending or descending sort.
	 * \param _tpCmpPolicy The text-compare policy.
	 **/
	void CTreeListView::SortChildren( ee::CTree<LSW_TREE_ROW> * _ptObj, size_t _sColumn, bool _bAsc, ee::EE_TEXTPOLICY _tpCmpPolicy ) {
		if ( !_ptObj ) { return; }
		if ( !_ptObj->Size() ) { return; }

		auto GetItemText = [this]( size_t _sRow, size_t _sColumn )->const std::wstring {
            return GetCellText( _sRow, _sColumn );
        };

		auto vOrder = BuildOrderByColumnGeneric( _ptObj->Size(), GetItemText, _sColumn, _bAsc, _tpCmpPolicy );
		_ptObj->ArrangeChildren( vOrder );
	}

	/**
	 * WM_SIZE.
	 *
	 * \param _wParam The type of resizing requested.
	 * \param _lWidth The new width of the client area.
	 * \param _lHeight The new height of the client area.
	 * \return Returns a HANDLED code.
	 */
	CWidget::LSW_HANDLED CTreeListView::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		CWidget::Size( _wParam, _lWidth, _lHeight );
		//::ShowScrollBar( Wnd(), SB_VERT, FALSE );
		//ResizeControls( VirtualClientRect( nullptr ) );

		return LSW_H_CONTINUE;
	}

	/**
	 * WM_MOVE.
	 *
	 * \param _lX The x coordinate of the upper-left corner of the client area of the window.
	 * \param _lY The y coordinate of the upper-left corner of the client area of the window.
	 * \return Returns a HANDLED code.
	 */
	CWidget::LSW_HANDLED CTreeListView::Move( LONG _lX, LONG _lY ) {
		CWidget::Move( _lX, _lY );
		//ResizeControls( VirtualClientRect( nullptr ) );

		return LSW_H_CONTINUE;
	}

	/**
	 * WM_KEYDOWN.
	 *
	 * \param _uiKeyCode The virtual-key code of the nonsystem key.
	 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
	 * \return Returns a HANDLED code.
	 */
	CWidget::LSW_HANDLED CTreeListView::KeyDown( UINT _uiKeyCode, UINT _uiFlags ) {
		switch ( _uiKeyCode ) {
			case VK_LEFT : {
				if ( (::GetAsyncKeyState( VK_CONTROL ) & 0x8000) == 0 ) {
					LSW_KEY_FLAGS kfFlags = { _uiFlags };
					ee::CTree<LSW_TREE_ROW> * ptNode = GetHighlighted();
					if ( ptNode ) {
						if ( (ptNode->Value().uiState & TVIS_EXPANDED) && ptNode->Size() ) {
							ptNode->Value().uiState &= ~TVIS_EXPANDED;
							UpdateListView();
							return LSW_H_HANDLED;
						}
						else {
							// Move up to parent.  If Shift is held, select all the children between here and there, otherwise set the selection to just the parent.
							if ( (::GetAsyncKeyState( VK_SHIFT ) & 0x8000) == 0 ) {
								// No shift, Sherlock!
								if ( ptNode->Parent() && ptNode->Parent() != &m_tRoot ) {
									UnselectAll();
									UnfocusAll();
									ptNode->Parent()->Value().uiState |= (TVIS_SELECTED | LSW_TREELIST_HIGHLIGHTED);
									UpdateListView();
									return LSW_H_HANDLED;
								}
							}
							else {
								// Holy shift!
								if ( ptNode->Parent() && ptNode->Parent() != &m_tRoot ) {
									UnfocusAll();
									ptNode->Value().uiState |= (TVIS_SELECTED | LSW_TREELIST_HIGHLIGHTED);
									ee::CTree<LSW_TREE_ROW> * ptPrev = ptNode->Prev();
									while ( ptPrev ) {
										ptPrev->Value().uiState |= (TVIS_SELECTED | LSW_TREELIST_HIGHLIGHTED);
										ptPrev = ptPrev->Prev();
									}
									ptNode->Parent()->Value().uiState |= (TVIS_SELECTED | LSW_TREELIST_HIGHLIGHTED);
									UpdateListView();
									return LSW_H_HANDLED;
								}
							}
						}
					}
				}
				break;
			}
			case VK_RIGHT : {
				if ( (::GetAsyncKeyState( VK_CONTROL ) & 0x8000) == 0 ) {
					LSW_KEY_FLAGS kfFlags = { _uiFlags };
					ee::CTree<LSW_TREE_ROW> * ptNode = GetHighlighted();
					if ( ptNode ) {
						if ( !(ptNode->Value().uiState & TVIS_EXPANDED) && ptNode->Size() ) {
							ptNode->Value().uiState |= TVIS_EXPANDED;
							UpdateListView();
							return LSW_H_HANDLED;
						}
					}
				}
				break;
			}
		}
		//
		return LSW_H_CONTINUE;
	}

	/**
	 * WM_LBUTTONDBLCLK.
	 * 
	 * \param _dwVirtKeys Indicates whether various virtual keys are down. This parameter can be one or more of the following values: MK_CONTROL, MK_LBUTTON, MK_MBUTTON, MK_RBUTTON, MK_SHIFT, MK_XBUTTON1, MK_XBUTTON2.
	 * \param _pCursorPos The coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area.
	 * \return Returns a HANDLED code.
	 **/
	CWidget::LSW_HANDLED CTreeListView::LButtonDblClk( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		LVHITTESTINFO hitTestInfo;
		hitTestInfo.pt.x = _pCursorPos.x;
		hitTestInfo.pt.y = _pCursorPos.y;
		ListView_HitTest( Wnd(), &hitTestInfo );
		if ( hitTestInfo.iItem >= 0 ) {
			auto aItem = ItemByIndex( hitTestInfo.iItem );
			if ( aItem ) {
				if ( (aItem->Value().uiState & TVIS_EXPANDED) && aItem->Size() ) {
					aItem->Value().uiState &= ~TVIS_EXPANDED;
					UpdateListView();
					return LSW_H_HANDLED;
				}
				else if ( !(aItem->Value().uiState & TVIS_EXPANDED) && aItem->Size() ) {
					aItem->Value().uiState |= TVIS_EXPANDED;
					UpdateListView();
					return LSW_H_HANDLED;
				}
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * WM_LBUTTONDOWN.
	 * 
	 * \param _dwVirtKeys Indicates whether various virtual keys are down.
	 * \param _pCursorPos The coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area.
	 * \return Returns a HANDLED code.
	 **/
	CWidget::LSW_HANDLED CTreeListView::LButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		LVHITTESTINFO hitTestInfo;
		hitTestInfo.pt.x = _pCursorPos.x;
		hitTestInfo.pt.y = _pCursorPos.y;
		ListView_HitTest( Wnd(), &hitTestInfo );
		if ( hitTestInfo.iItem >= 0 ) {
			HDC hHdc = ::GetDC( Wnd() );
			if ( hHdc ) {
				LSW_RECT rItem = GetItemRect( hitTestInfo.iItem, LVIR_BOUNDS );
				auto aItem = ItemByIndex( hitTestInfo.iItem );
				LSW_RECT rRect = ExpansionBox( rItem, aItem, hHdc );
				::ReleaseDC( Wnd(), hHdc );

				if ( rRect.PtInRect( hitTestInfo.pt ) ) {
					if ( aItem->Value().uiState & TVIS_EXPANDED ) {
						aItem->Value().uiState &= ~TVIS_EXPANDED;
					}
					else {
						aItem->Value().uiState |= TVIS_EXPANDED;
					}

					UpdateListView();
					return LSW_H_HANDLED;
				}
			}
		}


		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_NOTIFY -> LVN_ITEMCHANGED handler.
	 *
	 * \param _lplvParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CTreeListView::Notify_ItemChanged( LPNMLISTVIEW _lplvParm ) {
		if ( _lplvParm->uChanged & LVIF_STATE ) {
			if ( _lplvParm->iItem == -1 ) {
				// All items.
				if ( (_lplvParm->uOldState & LVIS_SELECTED) && !(_lplvParm->uNewState & LVIS_SELECTED) ) {
					// All items were deselected.
					UnselectAll();
				}
			}
			else {
				if ( (_lplvParm->uOldState & LVIS_SELECTED) && !(_lplvParm->uNewState & LVIS_SELECTED) ) {
					// Single node unselected.
					ee::CTree<LSW_TREE_ROW> * ptNode = ItemByIndex_Cached( _lplvParm->iItem );
					if ( ptNode ) {
						ptNode->Value().uiState &= ~TVIS_SELECTED;
					}
				}
				else if ( (_lplvParm->uOldState & LVIS_SELECTED) && (_lplvParm->uNewState & LVIS_SELECTED) ) {
					// Single node selected.
					ee::CTree<LSW_TREE_ROW> * ptNode = ItemByIndex_Cached( _lplvParm->iItem );
					if ( ptNode ) {
						ptNode->Value().uiState |= TVIS_SELECTED;
					}
				}
				else if ( !(_lplvParm->uOldState & LVIS_SELECTED) && (_lplvParm->uNewState & LVIS_SELECTED) ) {
					// Single node selected.
					ee::CTree<LSW_TREE_ROW> * ptNode = ItemByIndex_Cached( _lplvParm->iItem );
					if ( ptNode ) {
						ptNode->Value().uiState |= TVIS_SELECTED;
					}
				}

				if ( (_lplvParm->uOldState & LVIS_FOCUSED) && !(_lplvParm->uNewState & LVIS_FOCUSED) ) {
					// Single node un-hot.
					UnfocusAll();
					m_stHotItem = size_t( -1 );
				}
				else if ( (_lplvParm->uOldState & LVIS_FOCUSED) && (_lplvParm->uNewState & LVIS_FOCUSED) ) {
					// Single node hot.
					UnfocusAll();
					ee::CTree<LSW_TREE_ROW> * ptNode = ItemByIndex_Cached( _lplvParm->iItem );
					if ( ptNode ) {
						ptNode->Value().uiState |= LSW_TREELIST_HIGHLIGHTED;
					}
					m_stHotItem = size_t( _lplvParm->iItem );
				}
				else if ( !(_lplvParm->uOldState & LVIS_FOCUSED) && (_lplvParm->uNewState & LVIS_FOCUSED) ) {
					// Single node hot.
					UnfocusAll();
					ee::CTree<LSW_TREE_ROW> * ptNode = ItemByIndex_Cached( _lplvParm->iItem );
					if ( ptNode ) {
						ptNode->Value().uiState |= LSW_TREELIST_HIGHLIGHTED;
					}
					m_stHotItem = size_t( _lplvParm->iItem );
				}
			}
		}

		if ( m_pwParent ) {
			if ( LSW_H_HANDLED == m_pwParent->Notify_ItemChanged( _lplvParm ) ) {
				return LSW_H_HANDLED;
			}
		}

		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_NOTIFY -> LVN_ODSTATECHANGED handler.
	 *
	 * \param _lposcParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CTreeListView::Notify_OdStateChange( LPNMLVODSTATECHANGE _lposcParm ) {
		for ( int I = _lposcParm->iFrom; I <= _lposcParm->iTo; ++I ) {
			if ( (_lposcParm->uOldState & LVIS_SELECTED) && !(_lposcParm->uNewState & LVIS_SELECTED) ) {
				// Single node unselected.
				ee::CTree<LSW_TREE_ROW> * ptNode = ItemByIndex_Cached( I );
				if ( ptNode ) {
					ptNode->Value().uiState &= ~TVIS_SELECTED;
				}
			}
			else if ( (_lposcParm->uOldState & LVIS_SELECTED) && (_lposcParm->uNewState & LVIS_SELECTED) ) {
				// Single node selected.
				ee::CTree<LSW_TREE_ROW> * ptNode = ItemByIndex_Cached( I );
				if ( ptNode ) {
					ptNode->Value().uiState |= TVIS_SELECTED;
				}
			}
			else if ( !(_lposcParm->uOldState & LVIS_SELECTED) && (_lposcParm->uNewState & LVIS_SELECTED) ) {
				// Single node selected.
				ee::CTree<LSW_TREE_ROW> * ptNode = ItemByIndex_Cached( I );
				if ( ptNode ) {
					ptNode->Value().uiState |= TVIS_SELECTED;
				}
			}
		}
		
		if ( m_pwParent ) {
			if ( LSW_H_HANDLED == m_pwParent->Notify_OdStateChange( _lposcParm ) ) {
				return LSW_H_HANDLED;
			}
		}

		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_NOTIFY -> NM_CUSTOMDRAW -> CDDS_ITEMPREPAINT handler.
	 *
	 * \param _lpcdParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	DWORD CTreeListView::Notify_CustomDraw_ItemPrePaint( LPNMLVCUSTOMDRAW _lpcdParm ) {
		constexpr BYTE bDefR = 0xF2;
		constexpr BYTE bDefG = 0xFA;
		constexpr BYTE bDefB = 0xFF;

		auto ptiItem = ItemByIndex( _lpcdParm->nmcd.dwItemSpec );
		if ( ptiItem ) {
			RGBQUAD rgbColor;
			if ( ptiItem->Value().rgbColor.rgbReserved == 0xFF ) {
				_lpcdParm->clrTextBk = RGB( ptiItem->Value().rgbColor.rgbRed, ptiItem->Value().rgbColor.rgbGreen, ptiItem->Value().rgbColor.rgbBlue );
				_lpcdParm->clrText = CHelpers::ContrastingTextColor( _lpcdParm->clrTextBk );
			}
			else if ( ptiItem->Value().rgbColor.rgbReserved == 0x00 ) {
				if ( _lpcdParm->nmcd.dwItemSpec % 2 == 0 ) {
					_lpcdParm->clrText = ::GetSysColor( COLOR_WINDOWTEXT );
					_lpcdParm->clrTextBk = RGB( bDefR, bDefG, bDefB );
				}
			}
			else {
				if ( _lpcdParm->nmcd.dwItemSpec % 2 == 0 ) {
					//_lpcdParm->clrText = ::GetSysColor( COLOR_WINDOWTEXT );
					//_lpcdParm->clrTextBk = RGB( 0xF2, 0xFA, 0xFF );
					rgbColor.rgbRed = bDefR;
					rgbColor.rgbGreen = bDefG;
					rgbColor.rgbBlue = bDefB;
				}
				else {
					auto aBg = ::GetSysColor( COLOR_WINDOW );
					rgbColor.rgbRed = GetRValue( aBg );
					rgbColor.rgbGreen = GetGValue( aBg );
					rgbColor.rgbBlue = GetBValue( aBg );
				}

				uint64_t ui64Key = (static_cast<uint64_t>(*reinterpret_cast<uint32_t *>(&rgbColor)) << 32ULL) | (*reinterpret_cast<uint32_t *>(&ptiItem->Value().rgbColor));
				auto aKey = m_mColorLookup.find( ui64Key );
				if ( aKey == m_mColorLookup.end() ) {
					_lpcdParm->clrTextBk = CHelpers::MixColorRef( rgbColor.rgbRed, ptiItem->Value().rgbColor.rgbRed,
						rgbColor.rgbGreen, ptiItem->Value().rgbColor.rgbGreen,
						rgbColor.rgbBlue, ptiItem->Value().rgbColor.rgbBlue,
						ptiItem->Value().rgbColor.rgbReserved / 255.0f );
					m_mColorLookup[ui64Key] = _lpcdParm->clrTextBk;
				}
				else {
					_lpcdParm->clrTextBk = aKey->second;
				}
				_lpcdParm->clrText = CHelpers::ContrastingTextColor( _lpcdParm->clrTextBk );
			}
		}
		return (CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT);
	}

	/**
	 * The WM_NOTIFY -> NM_CUSTOMDRAW -> CDDS_ITEMPOSTPAINT handler.
	 * 
	 * \param _lpcdParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	DWORD CTreeListView::Notify_CustomDraw_ItemPostPaint( LPNMLVCUSTOMDRAW _lpcdParm ) {
		m_ptIndexCache = ItemByIndex_Cached( _lpcdParm->nmcd.dwItemSpec );
		if ( m_ptIndexCache ) {
			if ( m_ptIndexCache->Size() ) {
				HTHEME hTheme = ::OpenThemeData( _lpcdParm->nmcd.hdr.hwndFrom, L"TREEVIEW" );
				if ( hTheme ) {
					int iState = GLPS_OPENED;
					if ( m_ptIndexCache->Value().uiState & TVIS_EXPANDED ) {
						iState = GLPS_OPENED;
					}
					else {
						iState = GLPS_CLOSED;
					}

					m_lSpaceWidth = 0;
					LSW_RECT rRect = ExpansionBox( _lpcdParm->nmcd.rc, ItemByIndex( _lpcdParm->nmcd.dwItemSpec ), _lpcdParm->nmcd.hdc );


					::DrawThemeBackground( hTheme, _lpcdParm->nmcd.hdc, TVP_GLYPH, iState, &rRect, NULL );

					::CloseThemeData( hTheme );
				}
			}
		}
		return CDRF_SKIPDEFAULT;
	}

	/**
	 * Evaluates expressions to determine a new rectangle for the control.
	 */
	void CTreeListView::EvalNewSize() {
		CWidget::EvalNewSize();
	}

	/**
	 * Gathers the selected items into a vector.
	 *
	 * \param _htiFrom The item from which to start gathering.
	 * \param _vReturn The array into which to gather the selected items.
	 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
	 */
	void CTreeListView::GatherSelected( HTREEITEM _htiFrom, std::vector<HTREEITEM> &_vReturn, bool _bIncludeNonVisible ) const {
		const ee::CTree<LSW_TREE_ROW> * ptFrom = TreeItemToPointer( _htiFrom );
		while ( ptFrom ) {
			UINT uiState = ptFrom->Value().uiState;
			if ( uiState & TVIS_SELECTED ) {
				_vReturn.push_back( PointerToTreeItem( ptFrom ) );
			}
			if ( _bIncludeNonVisible || (uiState & TVIS_EXPANDED) && ptFrom->Size() ) {
				GatherSelected( PointerToTreeItem( ptFrom->GetChild( 0 ) ), _vReturn, _bIncludeNonVisible );
			}
			ptFrom = ptFrom->Next();
		}
	}

	/**
	 * Gathers the selected item indices into a vector.
	 *
	 * \param _htiFrom The item from which to start gathering.
	 * \param _vReturn The array into which to gather the selected items.
	 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
	 * \param _stIndex Tracks the current item's index.
	 */
	void CTreeListView::GatherSelected( HTREEITEM _htiFrom, std::vector<size_t> &_vReturn, bool _bIncludeNonVisible, size_t &_stIndex ) const {
		const ee::CTree<LSW_TREE_ROW> * ptFrom = TreeItemToPointer( _htiFrom );
		while ( ptFrom ) {
			UINT uiState = ptFrom->Value().uiState;
			if ( uiState & TVIS_SELECTED ) {
				_vReturn.push_back( _stIndex );
			}
			++_stIndex;
			if ( _bIncludeNonVisible || (uiState & TVIS_EXPANDED) && ptFrom->Size() ) {
				GatherSelected( PointerToTreeItem( ptFrom->GetChild( 0 ) ), _vReturn, _bIncludeNonVisible, _stIndex );
			}
			ptFrom = ptFrom->Next();
		}
	}

	/**
	 * Gathers the selected item LPARAM values into a vector.
	 *
	 * \param _htiFrom The item from which to start gathering.
	 * \param _vReturn The array into which to gather the selected items.
	 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
	 * \param _stIndex Tracks the current item's index.
	 */
	void CTreeListView::GatherSelectedLParam( HTREEITEM _htiFrom, std::vector<LPARAM> &_vReturn, bool _bIncludeNonVisible, size_t &_stIndex ) const {
		const ee::CTree<LSW_TREE_ROW> * ptFrom = TreeItemToPointer( _htiFrom );
		while ( ptFrom ) {
			UINT uiState = ptFrom->Value().uiState;
			if ( uiState & TVIS_SELECTED ) {
				_vReturn.push_back( ptFrom->Value().lpParam );
			}
			++_stIndex;
			if ( _bIncludeNonVisible || (uiState & TVIS_EXPANDED) && ptFrom->Size() ) {
				GatherSelectedLParam( PointerToTreeItem( ptFrom->GetChild( 0 ) ), _vReturn, _bIncludeNonVisible, _stIndex );
			}
			ptFrom = ptFrom->Next();
		}
	}

	/**
	 * Gets the highlighted item or returns nullptr.
	 *
	 * \return Returs the highlighted item or nullptr if there is none.
	 */
	ee::CTree<CTreeListView::LSW_TREE_ROW> * CTreeListView::GetHighlighted() {
		ee::CTree<LSW_TREE_ROW> * ptNode = Next( nullptr );
		while ( ptNode ) {
			if ( ptNode->Value().uiState & LSW_TREELIST_HIGHLIGHTED ) {
				return ptNode;
			}
			ptNode = NextByExpansion( ptNode );
		}
		return nullptr;
	}

	/**
	 * Updates the list view (clears the cache, sets the size, and updates selections/hot).
	 */
	void CTreeListView::UpdateListView() {
		ClearCache();
		size_t stTotal = CountExpanded();
		const_cast<CTreeListView *>(this)->SetItemCount( static_cast<INT>(stTotal) );

		UnfocusCollapsed();
		size_t stHighlighted = FindHighlighted();
		if ( stHighlighted == size_t( -1 ) ) {
			SetItemHighlight( -1, FALSE );
		}
		else {
			SetItemHighlight( INT( stHighlighted ), TRUE );
		}

		// Get the selected indices based on the tree.
		std::vector<size_t> vSelection;
		GatherSelected( vSelection );

		// Get the selected indices in the listview.
		std::vector<int> vListSelection;
		GetSelectedItems( vListSelection );

		// Unselect from the listview.
		auto aTreeSelctIdx = vSelection.begin();
		for ( size_t I = 0; I < vListSelection.size(); ++I ) {
			auto aTmp = std::find( aTreeSelctIdx, vSelection.end(), size_t( vListSelection[I] ) );
			if ( aTmp != vSelection.end() ) {
				// It is still selected.
				aTreeSelctIdx = aTmp;
			}
			else {
				// It is unselected.
				SetItemSelection( static_cast<INT>(vListSelection[I]), FALSE );
			}
		}

		// Select from the tree-listview.
		auto aSelctIdx = vListSelection.begin();
		for ( size_t I = 0; I < vSelection.size(); ++I ) {
			auto aTmp = std::find( aSelctIdx, vListSelection.end(), static_cast<int>(vSelection[I]) );
			if ( aTmp != vListSelection.end() ) {
				// It is still selected.
				aSelctIdx = aTmp;
			}
			else {
				// It is selected.
				SetItemSelection( static_cast<INT>(vSelection[I]), TRUE );
			}
		}
	}

	/**
	 * Removes focus from any hidden items.
	 */
	void CTreeListView::UnfocusCollapsed() {
		ee::CTree<LSW_TREE_ROW> * ptFrom = Next( nullptr );
		while ( ptFrom ) {
			UINT uiState = ptFrom->Value().uiState;
			if ( uiState & LSW_TREELIST_HIGHLIGHTED ) {
				if ( IsHidden( PointerToTreeItem( ptFrom ) ) ) {
					ptFrom->Value().uiState &= ~LSW_TREELIST_HIGHLIGHTED;
				}
			}
			ptFrom = Next( ptFrom );
		}
	}

	/**
	 * Removes focus from all items.
	 */
	void CTreeListView::UnfocusAll() {
		ee::CTree<LSW_TREE_ROW> * ptFrom = Next( nullptr );
		while ( ptFrom ) {
			ptFrom->Value().uiState &= ~LSW_TREELIST_HIGHLIGHTED;
			ptFrom = Next( ptFrom );
		}
	}

	/**
	 * Takes an item RECT and returns the expanding/collapsing box rectangle that goes with it.
	 * 
	 * \param _rItemRect The item's RECT.
	 * \param _ptItem The item.
	 * \param _hHdc The listview's HDC.
	 * \return Returns the item's expanding/collapsing box rectangle.
	 **/
	LSW_RECT CTreeListView::ExpansionBox( const RECT &_rItemRect, const ee::CTree<LSW_TREE_ROW> * _ptItem, HDC _hHdc ) {
		LSW_RECT rRet;
		rRet.Zero();
		if ( _ptItem ) {
			if ( _ptItem->Size() ) {
				if ( !m_lSpaceWidth ) {
					SIZE sSpaceSize;
					::GetTextExtentPoint32A( _hHdc, " ", 1, &sSpaceSize );
					m_lSpaceWidth = sSpaceSize.cx;
				}

				const LSW_RECT & rTmp = _rItemRect;
				LONG lLeft = rTmp.left + 2;
				lLeft += m_lSpaceWidth * 4 * GetIndent( const_cast<ee::CTree<LSW_TREE_ROW> *>(_ptItem) );
				return { lLeft,
					rTmp.top + 1,
					lLeft + rTmp.Height() - 2,
					rTmp.top + 1 + rTmp.Height() - 2 };
			}
		}
		return rRet;
	}

	/**
	 * List-view window procedure.  The list-view is hidden.
	 *
	 * \param _hWnd A handle to the window.
	 * \param _uMsg The message.
	 * \param _wParam Additional message information.
	 * \param _lParam Additional message information.
	 * \return The return value is the result of the message processing, and depends on the message sent.
	 */
	LRESULT CALLBACK CTreeListView::ListViewOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CTreeListView * ptlThis = reinterpret_cast<CTreeListView *>(::GetPropW( _hWnd, m_szProp ));
		WNDPROC wpOrig = ptlThis ? ptlThis->m_wpListViewProc : NULL;

		switch ( _uMsg ) {
			case WM_NCDESTROY : {
				::RemovePropW( _hWnd, m_szProp );
				break;
			}
			case WM_LBUTTONDBLCLK : {
				if ( ptlThis ) {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = ptlThis->LButtonDblClk( static_cast<DWORD>(_wParam), pPos );

					// Return value
					//	An application should return zero if it processes this message.
					if ( hHandled == LSW_H_HANDLED ) { return 0; }
				}
				break;
			}
			case WM_NCLBUTTONDBLCLK : {
				//::MoveWindow( _hWnd, ee::CExpEval::Time() % 10, ee::CExpEval::Time() % 20, ee::CExpEval::Time() % 10 + 350, ee::CExpEval::Time() % 10 + 250, TRUE );
				break;
			}
			case WM_LBUTTONDOWN : {
				if ( ptlThis ) {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = ptlThis->LButtonDown( static_cast<DWORD>(_wParam), pPos );

					// Return value
					//	An application should return zero if it processes this message.
					if ( hHandled == LSW_H_HANDLED ) { return 0; }
				}
				break;
			}
			case WM_KEYDOWN : {
				if ( ptlThis ) {
					LSW_HANDLED hHandled = ptlThis->KeyDown( static_cast<UINT>(_wParam), static_cast<UINT>(_lParam) );

					// Return value
					//	An application should return zero if it processes this message.
					if ( hHandled == LSW_H_HANDLED ) { return 0; }
				}
				break;
			}
#if 0
			case WM_PAINT : {
				RECT clientRect;
				::GetClientRect( _hWnd, &clientRect );

				// Call the original window procedure to draw into memory DC
				//LRESULT lrRes = ::CallWindowProcW( wpOrig, _hWnd, _uMsg, _wParam, _lParam );

				/*PAINTSTRUCT ps;
				HDC hdc = ::BeginPaint( _hWnd, &ps );

				RECT rcButton = { 5, 50, 30, 70 };
				::InvalidateRect( _hWnd, &rcButton, TRUE );
				::DrawFrameControl( hdc, &rcButton, DFC_BUTTON, DFCS_BUTTONCHECK );

				::EndPaint( _hWnd, &ps );*/

				
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(_hWnd, &ps);

				// Create a compatible memory DC and bitmap
				HDC memDC = CreateCompatibleDC(hdc);
				HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
				HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

    // Fill the bitmap with a background color (e.g., white)
    //FillRect(memDC, &clientRect, (HBRUSH)(COLOR_WINDOW+1));

				LRESULT lrRes = CallWindowProcW( wpOrig, _hWnd, _uMsg, (WPARAM)memDC, _lParam );
				//LRESULT lrRes = CallWindowProcW( wpOrig, _hWnd, _uMsg, _wParam, (LPARAM)memDC );

				// Now do your custom drawing on memory DC
				RECT rcButton = {10, 50, 30, 70};
				::InvalidateRect( _hWnd, &rcButton, FALSE );
				DrawFrameControl(memDC, &rcButton, DFC_BUTTON, DFCS_BUTTONCHECK);
				// Call the original window procedure to draw into memory DC
				

				// Blit the memory DC to screen
				BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, (ps.rcPaint.right - ps.rcPaint.left) / 2,
					   (ps.rcPaint.bottom - ps.rcPaint.top) / 2, memDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

				

				// Clean up
    SelectObject(memDC, oldBitmap);

				DeleteObject(memBitmap);
				DeleteDC(memDC);

				EndPaint(_hWnd, &ps);
				
				return lrRes;

			}
#endif
#if 0
			case WM_NOTIFY : {
				LPNMHDR lpnmhdr = (LPNMHDR)_lParam;
				if (lpnmhdr->code == NM_CUSTOMDRAW) {
					LPNMLVCUSTOMDRAW lpLVCD = (LPNMLVCUSTOMDRAW)_lParam;

					switch (lpLVCD->nmcd.dwDrawStage) {
						case CDDS_PREPAINT:
							return CDRF_NOTIFYITEMDRAW;

						case CDDS_ITEMPREPAINT: {
							/*static bool _bInPrePaint = false;

							if ( !_bInPrePaint ) {
								_bInPrePaint = true;
								::InvalidateRect( _hWnd, &lpLVCD->nmcd.rc, FALSE );
								::SendMessageW( lpLVCD->nmcd.hdr.hwndFrom, WM_PAINT, 0, 0 );
								::ValidateRect( _hWnd, &lpLVCD->nmcd.rc );
								_bInPrePaint = false;
							}
							else {
								return CDRF_DODEFAULT;
							}*/
							HTHEME hTheme = ::OpenThemeData(_hWnd, L"TREEVIEW");
							if (hTheme) {
								
								RECT rc = lpLVCD->nmcd.rc;
								rc.right = rc.left + 20;
								int partId = TVP_GLYPH;
								int stateId = GLPS_OPENED; // or GLPS_CLOSED for "+"

								//::InvalidateRect( _hWnd, &rc, FALSE );
								//::DrawThemeBackground(hTheme, lpLVCD->nmcd.hdc, partId, stateId, &rc, NULL);
								//::ValidateRect( _hWnd, &rc );



								::CloseThemeData(hTheme);
							}
							return CDRF_DODEFAULT;
							//return CDRF_SKIPDEFAULT;
						}
					}
				}
				break;
			}
#endif
						  
			/*case WM_PAINT : {
				HTHEME hTheme = ::OpenThemeData( _hWnd, L"TREEVIEW" );
				if ( NULL != hTheme ) {
					::CloseThemeData( hTheme );
				}
				break;
			}*/

			// =======================================
			// List-View Messages.
			// =======================================
			case LVM_SETITEMSTATE : {
				break;
			}
		}
		if ( wpOrig ) {
			return ::CallWindowProcW( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
		}
		else {
			return 0;
		}
	}

	/**
	 * Converts a TVITEMEXW to a LSW_TREE_ROW.
	 *
	 * \param _ieItemEx The TVITEMEXW object to convert.
	 * \return Returns the equivalent LSW_TREE_ROW object for the given TVITEMEXW object.
	 */
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

}	// namespace lsw

#pragma once
#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../ListView/LSWListView.h"
#include "../Widget/LSWWidget.h"

#include <Tree/EETree.h>

#include <numeric>

#ifndef LSW_TREELIST_INVALID
#define LSW_TREELIST_INVALID				static_cast<size_t>(~static_cast<size_t>(0))
#endif	// #ifndef LSW_TREELIST_INVALID

#ifndef LSW_TREELIST_HIGHLIGHTED
#define LSW_TREELIST_HIGHLIGHTED			0x08000000
#endif	// #ifndef LSW_TREELIST_HIGHLIGHTED

namespace lsw {

	class CTreeListView : public CListView {
	public :
		CTreeListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CTreeListView();


		// == Functions.
		/**
		 * Setting the HWND after the control has been created.
		 *
		 * \param _hWnd The handle to the window/control being initialized.
		 */
		virtual void						InitControl( HWND _hWnd );

		/**
		 * Inserts an item.
		 *
		 * \param _pisStruct The insertion data for the item being inserted.
		 * \return Returns a handle to the inserted item.
		 */
		HTREEITEM							InsertItem( const TVINSERTSTRUCTW * _pisStruct );

		/**
		 * Sets the text for an item.
		 *
		 * \param _tiItem Handle to the item to modify.
		 * \param _pwcText The new text value if the item.
		 * \param _iColumn The cloumn of text to set.
		 * \return Returns TRUE if the item's text was set.  FALSE indicates that the column was out-of-range or that the item was invalid.
		 */
		BOOL								SetItemText( HTREEITEM _tiItem, const WCHAR * _pwcText, INT _iColumn );

		/**
		 * Gets the text of an item.
		 *
		 * \param _tiItem Handle to the item.
		 * \param _iColumn The index of the column whose text is to be gotten.
		 * \param _sRet Holds the returned text value.
		 * \return Returns TRUE if the item's text was obtained.  FALSE indicates that the column was out-of-range or that the item was invalid.
		 */
		BOOL								GetItemText( HTREEITEM _tiItem, INT _iColumn, std::wstring &_sRet ) const;

		/**
		 * Sets the lParam of an item.
		 *
		 * \param _tiItem Handle to the item to modify.
		 * \param _lpParam The value to set.
		 * \return Returns TRUE if the item's LPARAM was set.  FALSE indicates that the item was invalid.
		 */
		BOOL								SetItemLParam( HTREEITEM _tiItem, LPARAM _lpParam );

		/**
		 * Gets the lParam of an item.
		 *
		 * \param _tiItem Handle to the item.
		 * \return Returns the item's LPARAM value or 0 if the item is invalid.
		 */
		LPARAM								GetItemLParam( HTREEITEM _tiItem ) const;

		/**
		 * Sets an itemÅfs color.
		 * 
		 * \param _tiItem The item whose color is to be updated.
		 * \param _rgbColor The color to apply to the item (alpha respected).
		 * \return Returns TRUE if the itemÅfs color was set.  FALSE indicates that the item was invalid.
		 **/
		BOOL								SetItemColor( HTREEITEM _tiItem, RGBQUAD _rgbColor );

		/**
		 * Gets an item by index accounting for children being expanded or not.
		 *
		 * \param _stIdx The index of the item to get with collapsed items being taken into account.
		 * \return Returns the _stIdx'th item in the tree, accounting for expandedness.
		 */
		HTREEITEM							GetItemByIndex( size_t _stIdx ) { return PointerToTreeItem( ItemByIndex( _stIdx ) ); }

		/**
		 * Gets an item given an LPARAM.  Only the first item with the given LPARAM is returned.
		 * 
		 * \param _lpValue The LPARAM value to find on an item.
		 * \return Returns the item with the given LPARAM value or NULL.
		 **/
		HTREEITEM							GetByLParam( LPARAM _lpValue );
									
		/**
		 * Counts the total number of expanded items.
		 *
		 * \return Returns the total number of items, accounting for expandedness.
		 */
		size_t								CountExpanded() const;

		/**
		 * Gets the next item based on expansion.
		 *
		 * \param _ptThis The items whose next item is to be obtained, accounting for expandedness.
		 * \return Returns the next item, accounting for expandedness.
		 */
		HTREEITEM							GetNextByExpansion( HTREEITEM _ptThis ) const { return PointerToTreeItem( NextByExpansion( const_cast<ee::CTree<LSW_TREE_ROW> *>(TreeItemToPointer( _ptThis )) ) ); }

		/**
		 * Gets the next item.
		 *
		 * \param _ptThis The items whose next item is to be obtained.
		 * \return Returns the next item.
		 */
		HTREEITEM							GetNext( HTREEITEM _ptThis ) const { return PointerToTreeItem( Next( const_cast<ee::CTree<LSW_TREE_ROW> *>(TreeItemToPointer( _ptThis )) ) ); }

		/**
		 * Deletes all items with the given LPARAM value.
		 * 
		 * \param _lpParm The LPARAM value to find and delete.
		 **/
		virtual VOID						DeleteByLParam( LPARAM _lpParm );

		/**
		 * Deletes all items.
		 */
		virtual VOID						DeleteAll();

		/**
		 * Moves items up one index.  Items are found by their LPARAM values.
		 * 
		 * \param _vItems The array of LPARAM items to move up 1 index.
		 **/
		virtual VOID						MoveUp( const std::vector<LPARAM> &_vItems );

		/**
		 * Moves items down one index.  Items are found by their LPARAM values.
		 * 
		 * \param _vItems The array of LPARAM items to move down 1 index.
		 **/
		virtual VOID						MoveDown( const std::vector<LPARAM> &_vItems );

		/**
		 * Returns true if this is a CTreeListView class.
		 *
		 * \return Returns true.
		 */
		virtual bool						IsTreeListView() const { return true; }

		/**
		 * Selects all root-level items.
		 * 
		 * \return Returns the number of items selected.
		 **/
		virtual size_t						SelectRootItems();

		/**
		 * Sets the selection based on item data.
		 * 
		 * \param _pData The LPARAM value to use to decide on selection.
		 * \return Returns the number of items selected.
		 **/
		virtual INT							SetCurSelByItemData( LPARAM _pData );

		/**
		 * Unselects all items.
		 **/
		virtual void						UnselectAll();

		/**
		 * Returns true if any of the selected items have children and are not in expanded view.
		 *
		 * \return Returns true if any of the selected items have children and are not in expanded view.
		 */
		bool								AnySelectedHasUnexpandedChildren() const;

		/**
		 * Returns true if any of the selected items have children and are in expanded view.
		 *
		 * \return Returns true if any of the selected items have children and are in expanded view.
		 */
		bool								AnySelectedHasExpandedChildren() const;

		/**
		 * Returns true if any of the items have children and are not in expanded view.
		 *
		 * \return Returns true if any of the items have children and are not in expanded view.
		 */
		bool								AnyHasUnexpandedChildren() const;

		/**
		 * Returns true if any of the items have children and are in expanded view.
		 *
		 * \return Returns true if any of the items have children and are in expanded view.
		 */
		bool								AnyHasExpandedChildren() const;

		/**
		 * Expands selected items.
		 */
		void								ExpandSelected();

		/**
		 * Expands all items.
		 */
		void								ExpandAll();

		/**
		 * Collapses selected items.
		 *
		 * \return Collapses selected items.
		 */
		void								CollapseSelected();

		/**
		 * Collapses all items.
		 *
		 * \return Collapses all items.
		 */
		void								CollapseAll();

		/**
		 * Gathers the selected items into a vector.
		 *
		 * \param _vReturn The array into which to gather the selected items.
		 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
		 * \return Returns the number of items gathered.
		 */
		size_t								GatherSelected( std::vector<HTREEITEM> &_vReturn, bool _bIncludeNonVisible = false ) const;

		/**
		 * Gathers the selected item indices into a vector.
		 *
		 * \param _vReturn The array into which to gather the selected items.
		 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
		 * \return Returns the number of items gathered.
		 */
		size_t								GatherSelected( std::vector<size_t> &_vReturn, bool _bIncludeNonVisible = false ) const;

		/**
		 * Gathers the selected item LPARAM values into a vector.
		 *
		 * \param _vReturn The array into which to gather the selected items.
		 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
		 * \return Returns the number of items gathered.
		 */
		size_t								GatherSelectedLParam( std::vector<LPARAM> &_vReturn, bool _bIncludeNonVisible = false ) const;

		/**
		 * Gathers all LPARAM values of every tree item into an array.
		 * 
		 * \param _vReturn The array into which to gather the return values.
		 * \param _bIncludeNonVisible If true, non-expanded items are also searched.
		 * \return Returns the total number of items gathered (_vReturn.size()).
		 **/
		size_t								GatherAllLParam( std::vector<LPARAM> &_vReturn, bool _bIncludeNonVisible = false ) const;

		/**
		 * Gathers all LPARAM values of every tree item visible within the control view space into an array.
		 * 
		 * \param _vReturn The array into which to gather the return values.
		 * \return Returns the total number of items gathered (_vReturn.size()).
		 **/
		size_t								GatherInViewLParam( std::vector<LPARAM> &_vReturn ) const;

		/**
		 * Gets the index of the highlighted item or returns size_t( -1 ).
		 *
		 * \return Returs the index of the highlighted item or size_t( -1 ) if there is none.
		 */
		size_t								FindHighlighted() const;

		/**
		 * Allows quickly updating the tree without causing visual updates to the controls.  Must be paired with a call to FinishUpdate().
		 */
		void								BeginLargeUpdate() {
			m_bDontUpdate = true;
		}

		/**
		 * Call to update the control after making major changes to the tree.
		 */
		void								FinishUpdate() {
			m_bDontUpdate = false;
			UpdateListView();
		}

		/**
		 * Determines if the given tree item is hidden.
		 *
		 * \param _hiItem The item to check for being hidden
		 * \return Returns true if any parent of the item is collapsed.
		 */
		bool								IsHidden( HTREEITEM _hiItem ) const;

		/**
		 * Determines if there is at least one item in the tree list view.
		 **/
		bool								HasItem() const { return const_cast<CTreeListView *>(this)->ItemByIndex( 0 ) != nullptr; }

		/**
		 * Requesting information (notification responder).
		 *
		 * \param _plvdiInfo Pointer to the display-info structure.
		 * \return Returns TRUE if the requested information could be obtained.  FALSE indicates an internal error.
		 */
		virtual BOOL						GetDispInfoNotify( NMLVDISPINFOW * _plvdiInfo );

		/**
		 * Gets the text for a given item.  The row is counted by skipping over collapsed items, and can refer to different items depending on the collapsed/expanded state of the tree.
		 * 
		 * \param _sRow The index of the item.
		 * \param _Column The column of the item.
		 * \return Returns the text of the given cell.
		 **/
		std::wstring						GetCellText( size_t _sRow, size_t _Column ) {
			return GetCellText( _sRow, _Column, ItemByIndex_Cached( _sRow ) );
		}

		/**
		 * Sorts items.
		 * 
		 * \param _iSubItem The index of the sub-item.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		virtual BOOL						SortItems( INT _iSubItem );

		/**
		 * Sets the sort method for a given column.
		 * 
		 * \param _iColumn The column by which to sort.
		 * \param _eMethod The sorting text-compare method.
		 **/
		void								SetSortMethod( INT _iColumn, ee::EE_TEXTPOLICY _eMethod ) {
			if ( _iColumn >= m_vSortMethod.size() && _iColumn < GetColumnCount() ) {
				try {
					m_vSortMethod.resize( _iColumn + 1 );
				}
				catch ( ... ) { return; }
			}
			m_vSortMethod[_iColumn] = _eMethod;
		}

		/**
		 * Sets sorting enabled or disabled.
		 * 
		 * \param PARM Whether sorting of columns is enabled or disabled.
		 **/
		void								SetSortable( bool _bSortable ) { m_bSort = _bSortable; }

		/**
		 * Deletes a column.
		 * 
		 * \param _iCol An index of the column to delete.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		virtual BOOL						DeleteColumn( INT _iCol ) {
			if ( CListView::DeleteColumn( _iCol ) ) {
				if ( _iCol < m_vSortMethod.size() ) {
					m_vSortMethod.erase( m_vSortMethod.begin() + _iCol );
				}
				DeleteColumn( &m_tRoot, _iCol );
				return TRUE;
			}
			return FALSE;
		}

		/**
		 * Deletes all columns.
		 **/
		virtual VOID						DeleteAllColumns() {
			for ( auto I = GetColumnCount(); I--; ) {
				DeleteColumn( &m_tRoot, I );
			}
			CListView::DeleteAllColumns();
			m_vSortMethod.clear();
		}

		/**
		 * A helper to easily create a tree view item to be inserted with only text.
		 *
		 * \param _pwcText The item text.
		 * \param _tiParent The item parent.
		 * \param _tiInsertAfter The item's insertion point
		 * \return Returns a default tree item with the given common parameters set.
		 */
		static TVINSERTSTRUCTW				DefaultItem( const WCHAR * _pwcText, HTREEITEM _tiParent = TVI_ROOT, HTREEITEM _tiInsertAfter = TVI_LAST );

		/**
		 * A helper to easily create a tree view item to be inserted with only text and data.
		 *
		 * \param _pwcText The item text.
		 * \param _lpParm The item's LPARAM.
		 * \param _tiParent The item parent.
		 * \param _tiInsertAfter The item's insertion point
		 * \return Returns a default tree item with the given common parameters set.
		 */
		static TVINSERTSTRUCTW				DefaultItemLParam( const WCHAR * _pwcText, LPARAM _lpParm, HTREEITEM _tiParent = TVI_ROOT, HTREEITEM _tiInsertAfter = TVI_LAST );


	protected :
		// == Types.
		/**
		 * Internal book-keeping for each row.
		 */
		struct LSW_TREE_ROW {
			std::vector<std::wstring>		vStrings;								// Length matches GetColumnCount().
			LPARAM							lpParam			= 0;					// The lParam originally associated with the tree item for this row.
			UINT							uiState			= TVIS_EXPANDED;		// State information.
			UINT							uiStateEx		= 0;					// Additional state information.  Manages multi-select.
			int								iImage			= 0;					// Index in the tree-view control's image list of the icon image to use when the item is in the nonselected state.
			int								iSelectedImage	= 0;					// Index in the tree-view control's image list of the icon image to use when the item is in the selected state.
			int								iExpandedImage	= 0;					// Index of the image in the control's image list to display when the item is in the expanded state.
			RGBQUAD							rgbColor		= { .rgbReserved = 0 };	// Color of the item.
			std::vector<bool>				vSortAscNext;							// Tobble between ascending sort and descending sort for a given column.

			// == Operators.
			// Less-than against another object.
			bool							operator < ( const LSW_TREE_ROW &_trOther ) const {
				if ( vStrings.size() == 0 || _trOther.vStrings.size() == 0 ) { return false; }
				return vStrings[0].compare( _trOther.vStrings[0] ) < 0;
			}

			// Less-than against a string.
			bool							operator < ( const wchar_t * _pwcOther ) const {
				if ( vStrings.size() == 0 ) { return false; }
				return std::wcscmp( vStrings[0].c_str(), _pwcOther ) < 0;
			}
		};


		// == Members.
		/** Column sorting method. */
		std::vector<ee::EE_TEXTPOLICY>		m_vSortMethod;
		/** Original list-view message handler. */
		WNDPROC								m_wpListViewProc;
		/** Book keeping.  One entry per row. */
		ee::CTree<LSW_TREE_ROW>				m_tRoot;
		/** Optimiation: Cache the last ee::CTree<CTreeListView::LSW_TREE_ROW> * returned by ItemByIndex(). */
		mutable ee::CTree<LSW_TREE_ROW> *	m_ptIndexCache;
		/** Optimiation: Cache the last index returned by ItemByIndex(). */
		mutable size_t						m_stIndexCache;
		/** The hot item index. */
		size_t								m_stHotItem;
		/** The width of the space character inside items of the ListView. */
		LONG								m_lSpaceWidth;
		/** If set, the listview is not updated when inserting/removing an item.  FinishUpdate() must be called to update the listview after the tree is modified. */
		bool								m_bDontUpdate;
		/** Set whether sorting is enabled. */
		bool								m_bSort = true;
		/** Window property. */
		static WCHAR						m_szProp[2];
		


		// == Functions.
		/**
		 * Converts an HTREEITEM to an object pointer.
		 *
		 * \param _htiItem The item handle to convert.
		 * \return Returns the converted pointer.
		 */
		ee::CTree<LSW_TREE_ROW> *			TreeItemToPointer( HTREEITEM _htiItem );

		/**
		 * Converts an HTREEITEM to a constant object pointer.
		 *
		 * \param _htiItem The item handle to convert.
		 * \return Returns the converted pointer.
		 */
		const ee::CTree<LSW_TREE_ROW> *		TreeItemToPointer( HTREEITEM _htiItem ) const;

		/**
		 * Converts an HTREEITEM to an index.
		 *
		 * \param _ptParent The parent item.
		 * \param _htiItem The item whose index into _ptParent's children is to be obtained.
		 * \param _pwcSortText Sort text if _htiItem is TVI_SORT.
		 * \return Returns the inde of the item.
		 */
		size_t								TreeItemToIndex( ee::CTree<LSW_TREE_ROW> * _ptParent, HTREEITEM _htiItem, const wchar_t * _pwcSortText );

		/**
		 * Converts an object pointer to an HTREEITEM.
		 *
		 * \param _ptObj The object pointer to cnvert to a handle.
		 * \return Returns the obect pointer converted to a handle.
		 */
		HTREEITEM							PointerToTreeItem( ee::CTree<LSW_TREE_ROW> * _ptObj ) const;

		/**
		 * Converts an object pointer to an HTREEITEM.
		 *
		 * \param _ptObj The object pointer to cnvert to a handle.
		 * \return Returns the obect pointer converted to a handle.
		 */
		HTREEITEM							PointerToTreeItem( const ee::CTree<LSW_TREE_ROW> * _ptObj ) const;

		/**
		 * Gets an item by index accounting for children being expanded or not.
		 *
		 * \param _stIdx The index of the item to get with collapsed items being taken into account.
		 * \return Returns the _stIdx'th item in the tree, accounting for expandedness.
		 */
		ee::CTree<LSW_TREE_ROW> *			ItemByIndex( size_t _stIdx ) const;

		/**
		 * Gets an item by index accounting for children being expanded or not.
		 *
		 * \param _stIdx The index of the item to get with collapsed items being taken into account.
		 * \return Returns the _stIdx'th item in the tree, accounting for expandedness.
		 */
		ee::CTree<LSW_TREE_ROW> *			ItemByIndex( ee::CTree<LSW_TREE_ROW> * _ptStart, size_t _stIdx, size_t _stStartIdx ) const;

		/**
		 * Gets an item by index accounting for children being expanded or not, using cached internal values to speed up the process.
		 *
		 * \param _stIdx The index of the item to get with collapsed items being taken into account.
		 * \return Returns the _stIdx'th item in the tree, accounting for expandedness.
		 */
		ee::CTree<LSW_TREE_ROW> *			ItemByIndex_Cached( size_t _stIdx ) const;

		/**
		 * Clears the cached tree item and index.
		 */
		void								ClearCache() const {
			m_ptIndexCache = nullptr;
			m_stIndexCache = 0;
		}

		/**
		 * Deletes a column from the given element and all of its children.
		 * 
		 * \param PARM The element from which to delete a column.
		 * \param PARM the column to delete.
		 **/
		void								DeleteColumn( ee::CTree<CTreeListView::LSW_TREE_ROW> * _ptThis, INT _iCol );

		/**
		 * Gets the text for a given item.  The row is counted by skipping over collapsed items, and can refer to different items depending on the collapsed/expanded state of the tree.
		 * 
		 * \param _sRow The index of the item.
		 * \param _Column The column of the item.
		 * \param _ptrRow A pointer to the _sRow'th item.
		 * \return Returns the text of the given cell.
		 **/
		std::wstring						GetCellText( size_t _sRow, size_t _Column, ee::CTree<LSW_TREE_ROW> * _ptrRow );
		
		/**
		 * Gets the indentation level for an item.
		 *
		 * \param _ptThis The item whose indentation level is to be obtained.
		 * \return Returns the number of parents on the item.
		 */
		INT									GetIndent( ee::CTree<CTreeListView::LSW_TREE_ROW> * _ptThis ) const;

		/**
		 * Gets the next item based on expansion.
		 *
		 * \param _ptThis The items whose next item is to be obtained, accounting for expandedness.
		 * \return Returns the next item, accounting for expandedness.
		 */
		ee::CTree<LSW_TREE_ROW> *			NextByExpansion( ee::CTree<LSW_TREE_ROW> * _ptThis ) const;

		/**
		 * Gets the next item.
		 *
		 * \param _ptThis The items whose next item is to be obtained.
		 * \return Returns the next item.
		 */
		ee::CTree<LSW_TREE_ROW> *			Next( ee::CTree<LSW_TREE_ROW> * _ptThis ) const;

		/**
		 * Moves items up one index.  Items are found by their LPARAM values.
		 * 
		 * \param _ptThis The group of items to possibly move up by 1.
		 * \param _sItems The array of LPARAM items to move up 1 index.
		 **/
		virtual VOID						MoveUp( ee::CTree<LSW_TREE_ROW> * _ptThis, const std::set<LPARAM> &_sItems );

		/**
		 * Moves items down one index.  Items are found by their LPARAM values.
		 * 
		 * \param _ptThis The group of items to possibly move down by 1.
		 * \param _sItems The array of LPARAM items to move down 1 index.
		 **/
		virtual VOID						MoveDown( ee::CTree<LSW_TREE_ROW> * _ptThis, const std::set<LPARAM> &_sItems );

		/**
		 * Sorts the children of a given item.
		 * 
		 * \param _ptObj The object whose children are to be sorted.
		 * \param _sColumn The column by which to sort rows.
		 * \param _bAsc Ascending or descending sort.
		 * \param _tpCmpPolicy The text-compare policy.
		 **/
		void								SortChildren( ee::CTree<LSW_TREE_ROW> * _ptObj, size_t _sColumn, bool _bAsc, ee::EE_TEXTPOLICY _tpCmpPolicy );

		/**
		 * Build a stable permutation for a flat list given a (row, col)->text accessor.
		 * 
		 * \tparam _tGetText A function for getting item texts based on row and column.
		 * \param _rRows The number of rows to process.
		 * \param _pfGetText A pointer to a function of type _tGetText.
		 * \param _sCol The column by which to sort.
		 * \param _bAsc Ascend or descend.
		 * \param _tpCmpPolicy The text-compare policy.
		 * \return Returns an ordered list with the new order of the children following the sort.
		 **/
		template <typename _tGetText>
		static inline std::vector<size_t>	BuildOrderByColumnGeneric( size_t _rRows, const _tGetText &_pfGetText, size_t _sCol, bool _bAsc, ee::EE_TEXTPOLICY _tpCmpPolicy ) {
			std::vector<size_t> vOrder( _rRows );
			std::iota( vOrder.begin(), vOrder.end(), size_t( 0 ) );
			std::stable_sort(
				vOrder.begin(), vOrder.end(),
				MakeIndexComparerText( _pfGetText, _sCol, _bAsc, _tpCmpPolicy )
			);
			return vOrder;
		}

		/**
		 * Creates an index comparator for text cells using a user-provided accessor.
		 * 
		 * The accessor must have the signature:
		 *     std::wstring GetText( size_t _iRow, size_t _sCol )
		 * 
		 * The returned comparator captures _sCol, _bAsc, and _eKind, fetches cell strings via the accessor,
		 * then applies numeric-aware parsing (int/float) or CmpTextNoWin32() for text.
		 * 
		 * \tparam _tGetText Callable type of the accessor.
		 * \param _GetText Accessor returning a const std::wstring & for (_iRow, _sCol).
		 * \param _sCol Column index to sort by.
		 * \param _bAsc Sort direction; true for ascending, false for descending.
		 * \param _tpCmpPolicy The text-compare policy.
		 * \return Returns a comparator suitable for std::(stable_)sort over row indices.
		 **/
		template <typename _tGetText>
		static inline auto MakeIndexComparerText(
			const _tGetText &_GetText,
			size_t _sCol,
			bool _bAsc,
			ee::EE_TEXTPOLICY _tpCmpPolicy ) {
			return [&_GetText, _sCol, _bAsc, _tpCmpPolicy]( size_t _sA, size_t _sB ) {
				std::wstring wsA = _GetText( _sA, _sCol );
				std::wstring wsB = _GetText( _sB, _sCol );


				// Fallback to text compare.
				int32_t i32C = ee::CExpEval::CmpText( wsA, wsB, _tpCmpPolicy );
				if( _bAsc ) { return i32C < 0; }
				else { return i32C > 0; }
			};
		}

		/**
		 * WM_SIZE.
		 *
		 * \param _wParam The type of resizing requested.
		 * \param _lWidth The new width of the client area.
		 * \param _lHeight The new height of the client area.
		 * \return Returns a HANDLED code.
		 */
		virtual LSW_HANDLED					Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		/**
		 * WM_MOVE.
		 *
		 * \param _lX The x coordinate of the upper-left corner of the client area of the window.
		 * \param _lY The y coordinate of the upper-left corner of the client area of the window.
		 * \return Returns a HANDLED code.
		 */
		virtual LSW_HANDLED					Move( LONG _lX, LONG _lY );

		/**
		 * WM_KEYDOWN.
		 *
		 * \param _uiKeyCode The virtual-key code of the nonsystem key.
		 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
		 * \return Returns a HANDLED code.
		 */
		virtual LSW_HANDLED					KeyDown( UINT _uiKeyCode, UINT _uiFlags );

		/**
		 * WM_LBUTTONDBLCLK.
		 * 
		 * \param _dwVirtKeys Indicates whether various virtual keys are down. This parameter can be one or more of the following values: MK_CONTROL, MK_LBUTTON, MK_MBUTTON, MK_RBUTTON, MK_SHIFT, MK_XBUTTON1, MK_XBUTTON2.
		 * \param _pCursorPos The coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area.
		 * \return Returns a HANDLED code.
		 **/
		virtual LSW_HANDLED					LButtonDblClk( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos );

		/**
		 * WM_LBUTTONDOWN.
		 * 
		 * \param _dwVirtKeys Indicates whether various virtual keys are down.
		 * \param _pCursorPos The coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area.
		 * \return Returns a HANDLED code.
		 **/
		virtual LSW_HANDLED					LButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos );

		/**
		 * The WM_NOTIFY -> LVN_ITEMCHANGED handler.
		 *
		 * \param _lplvParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Notify_ItemChanged( LPNMLISTVIEW /*_lplvParm*/ );

		/**
		 * The WM_NOTIFY -> LVN_ODSTATECHANGED handler.
		 *
		 * \param _lposcParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Notify_OdStateChange( LPNMLVODSTATECHANGE /*_lposcParm*/ );

		/**
		 * The WM_NOTIFY -> NM_CUSTOMDRAW -> CDDS_PREPAINT handler.
		 *
		 * \param _lpcdParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual DWORD						Notify_CustomDraw_PrePaint( LPNMLVCUSTOMDRAW /*_lpcdParm*/ ) { return CDRF_NOTIFYITEMDRAW; }

		/**
		 * The WM_NOTIFY -> NM_CUSTOMDRAW -> CDDS_ITEMPREPAINT handler.
		 *
		 * \param _lpcdParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual DWORD						Notify_CustomDraw_ItemPrePaint( LPNMLVCUSTOMDRAW /*_lpcdParm*/ );

		/**
		 * The WM_NOTIFY -> NM_CUSTOMDRAW -> CDDS_ITEMPOSTPAINT handler.
		 * 
		 * \param _lpcdParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual DWORD						Notify_CustomDraw_ItemPostPaint( LPNMLVCUSTOMDRAW /*_lpcdParm*/ );

		/**
		 * Evaluates expressions to determine a new rectangle for the control.
		 */
		virtual void						EvalNewSize();

		/**
		 * Gathers the selected items into a vector.
		 *
		 * \param _htiFrom The item from which to start gathering.
		 * \param _vReturn The array into which to gather the selected items.
		 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
		 */
		void								GatherSelected( HTREEITEM _htiFrom, std::vector<HTREEITEM> &_vReturn, bool _bIncludeNonVisible ) const;

		/**
		 * Gathers the selected item indices into a vector.
		 *
		 * \param _htiFrom The item from which to start gathering.
		 * \param _vReturn The array into which to gather the selected items.
		 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
		 * \param _stIndex Tracks the current item's index.
		 */
		void								GatherSelected( HTREEITEM _htiFrom, std::vector<size_t> &_vReturn, bool _bIncludeNonVisible, size_t &_stIndex ) const;

		/**
		 * Gathers the selected item LPARAM values into a vector.
		 *
		 * \param _htiFrom The item from which to start gathering.
		 * \param _vReturn The array into which to gather the selected items.
		 * \param _bIncludeNonVisible If true, selected items from collapsed nodes are gathered as well.
		 * \param _stIndex Tracks the current item's index.
		 */
		void								GatherSelectedLParam( HTREEITEM _htiFrom, std::vector<LPARAM> &_vReturn, bool _bIncludeNonVisible, size_t &_stIndex ) const;

		/**
		 * Gets the highlighted item or returns nullptr.
		 *
		 * \return Returs the highlighted item or nullptr if there is none.
		 */
		ee::CTree<LSW_TREE_ROW> *			GetHighlighted();

		/**
		 * Updates the list view (clears the cache, sets the size, and updates selections/hot).
		 */
		void								UpdateListView();

		/**
		 * Removes focus from any hidden items.
		 */
		void								UnfocusCollapsed();

		/**
		 * Removes focus from all items.
		 */
		void								UnfocusAll();

		/**
		 * Takes an item RECT and returns the expanding/collapsing box rectangle that goes with it.
		 * 
		 * \param _rItemRect The item's RECT.
		 * \param _ptItem The item.
		 * \param _hHdc The listview's HDC.
		 * \return Returns the item's expanding/collapsing box rectangle.
		 **/
		LSW_RECT							ExpansionBox( const RECT &_rItemRect, const ee::CTree<LSW_TREE_ROW> * _ptItem, HDC _hHdc );

		/**
		 * List-view window procedure.  The list-view is hidden.
		 *
		 * \param _hWnd A handle to the window.
		 * \param _uMsg The message.
		 * \param _wParam Additional message information.
		 * \param _lParam Additional message information.
		 * \return The return value is the result of the message processing, and depends on the message sent.
		 */
		static LRESULT CALLBACK				ListViewOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

		/**
		 * Converts a TVITEMEXW to a LSW_TREE_ROW.
		 *
		 * \param _ieItemEx The TVITEMEXW object to convert.
		 * \return Returns the equivalent LSW_TREE_ROW object for the given TVITEMEXW object.
		 */
		static LSW_TREE_ROW					ItemExToTreeRow( const TVITEMEXW &_ieItemEx );
	};

}	// namespace lsw

#pragma once
#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../ListView/LSWListView.h"
#include <Tree/EETree.h>
#include "../Widget/LSWWidget.h"

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
		 * Returns true if this is a CTreeListView class.
		 *
		 * \return Returns true.
		 */
		virtual bool						IsTreeListView() const { return true; }

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
		 * Gets the index of the highlighted item or returns size_t( -1 ).
		 *
		 * \return Returs the index of the highlighted item or size_t( -1 ) if there is none.
		 */
		size_t								FindHighlighted() const;

		/**
		 * Unselects all.
		 */
		void								UnselectAll();

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
		 * Requesting information (notification responder).
		 *
		 * \param _plvdiInfo Pointer to the display-info structure.
		 * \return Returns TRUE if the requested information could be obtained.  FALSE indicates an internal error.
		 */
		virtual BOOL						GetDispInfoNotify( NMLVDISPINFOW * _plvdiInfo );

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
			std::vector<std::wstring>		vStrings;					// Length matches GetColumnCount().
			LPARAM							lpParam			= 0;		// The lParam originally associated with the tree item for this row.
			UINT							uiState			= TVIS_EXPANDED;		// State information.
			UINT							uiStateEx		= 0;		// Additional state information.  Manages multi-select.
			int								iImage			= 0;		// Index in the tree-view control's image list of the icon image to use when the item is in the nonselected state.
			int								iSelectedImage	= 0;		// Index in the tree-view control's image list of the icon image to use when the item is in the selected state.
			int								iExpandedImage	= 0;		// Index of the image in the control's image list to display when the item is in the expanded state.

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
		/** If set, the listview is not updated when inserting/removing an item.  FinishUpdate() must be called to update the listview after the tree is modified. */
		bool								m_bDontUpdate;
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
		ee::CTree<LSW_TREE_ROW> *			ItemByIndex( size_t _stIdx );

		/**
		 * Gets an item by index accounting for children being expanded or not.
		 *
		 * \param _stIdx The index of the item to get with collapsed items being taken into account.
		 * \return Returns the _stIdx'th item in the tree, accounting for expandedness.
		 */
		ee::CTree<LSW_TREE_ROW> *			ItemByIndex( ee::CTree<LSW_TREE_ROW> * _ptStart, size_t _stIdx, size_t _stStartIdx );

		/**
		 * Gets an item by index accounting for children being expanded or not, using cached internal values to speed up the process.
		 *
		 * \param _stIdx The index of the item to get with collapsed items being taken into account.
		 * \return Returns the _stIdx'th item in the tree, accounting for expandedness.
		 */
		ee::CTree<LSW_TREE_ROW> *			ItemByIndex_Cached( size_t _stIdx );

		/**
		 * Clears the cached tree item and index.
		 */
		void								ClearCache() const {
			m_ptIndexCache = nullptr;
			m_stIndexCache = 0;
		}


		/**
		 * Counts the total number of expanded items.
		 *
		 * \return Returns the total number of items, accounting for expandedness.
		 */
		size_t								CountExpanded() const;

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

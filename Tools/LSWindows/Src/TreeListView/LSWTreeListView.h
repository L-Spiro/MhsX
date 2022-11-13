#pragma once
#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../ListView/LSWListView.h"
#include <Tree/EETree.h>
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CTreeListView : public CListView {
	public :
		CTreeListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CTreeListView();


		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

		// Inserts an item.
		HTREEITEM							InsertItem( const TVINSERTSTRUCTW * _pisStruct );

		// Sets the text for an item.
		BOOL								SetItemText( HTREEITEM _tiItem, const WCHAR * _pwcText, INT _iColumn );

		// Gets the text of an item.
		BOOL								GetItemText( HTREEITEM _tiItem, INT _iColumn, std::wstring &_sRet ) const;

		// Sets the lParam of an item.
		BOOL								SetItemLParam( HTREEITEM _tiItem, LPARAM _lpParam );

		// Gets the lParam of an item.
		LPARAM								GetItemLParam( HTREEITEM _tiItem ) const;

		// Returns true if this is a CTreeListView class.
		virtual bool						IsTreeListView() const { return true; }

		// Returns true if any of the selected items have children and are not in expanded view.
		bool								AnySelectedHasUnexpandedChildren() const;

		// Returns true if any of the selected items have children and are in expanded view.
		bool								AnySelectedHasExpandedChildren() const;

		// Returns true if any of the items have children and are not in expanded view.
		bool								AnyHasUnexpandedChildren() const;

		// Returns true if any of the items have children and are in expanded view.
		bool								AnyHasExpandedChildren() const;

		// Expands selected items.
		void								ExpandSelected() const;

		// Expands all items.
		void								ExpandAll() const;

		// Collapses selected items.
		void								CollapseSelected() const;

		// Collapses all items.
		void								CollapseAll() const;

		// Gathers the selected items into a vector.
		size_t								GatherSelected( std::vector<HTREEITEM> &_vReturn, bool _bIncludeNonVisible = false ) const;

		// Requesting information (notification responder).
		virtual BOOL						GetDispInfoNotify( NMLVDISPINFOW * _plvdiInfo );

		// A helper to easily create a tree view item to be inserted with only text.
		static TVINSERTSTRUCTW				DefaultItem( const WCHAR * _pwcText, HTREEITEM _tiParent = TVI_ROOT, HTREEITEM _tiInsertAfter = TVI_LAST );

		// A helper to easily create a tree view item to be inserted with only text and data.
		static TVINSERTSTRUCTW				DefaultItemLParam( const WCHAR * _pwcText, LPARAM _lpParm, HTREEITEM _tiParent = TVI_ROOT, HTREEITEM _tiInsertAfter = TVI_LAST );


	protected :
		// == Types.
		// Internal book-keeping for each row.
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
		// The list view.
		//HWND								m_hListView;
		// Original list-view message handler.
		WNDPROC								m_wpListViewProc;
		// Book keeping.  One entry per row.
		//std::vector<LSW_TREE_ROW>			m_vRows;
		ee::CTree<LSW_TREE_ROW>				m_tRoot;
		// Optimiation: Cache the last ee::CTree<CTreeListView::LSW_TREE_ROW> * returned by ItemByIndex().
		mutable ee::CTree<LSW_TREE_ROW> *	m_ptIndexCache;
		// Optimiation: Cache the last index returned by ItemByIndex().
		mutable size_t						m_stIndexCache;
		// Window property.
		static WCHAR						m_szProp[2];


		// == Functions.
#if 0
		// WM_SIZE.
		virtual LSW_HANDLED					Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		// WM_MOVE.
		virtual LSW_HANDLED					Move( LONG _lX, LONG _lY );
#endif

		// Converts an HTREEITEM to an object pointer.
		ee::CTree<LSW_TREE_ROW> *			TreeItemToPointer( HTREEITEM _htiItem );

		// Converts an HTREEITEM to an object pointer.
		const ee::CTree<LSW_TREE_ROW> *		TreeItemToPointer( HTREEITEM _htiItem ) const;

		// Converts an HTREEITEM to an index.
		size_t								TreeItemToIndex( ee::CTree<LSW_TREE_ROW> * _ptParent, HTREEITEM _htiItem, const wchar_t * _pwcSortText );

		// Converts an object pointer to an HTREEITEM.
		HTREEITEM							PointerToTreeItem( ee::CTree<LSW_TREE_ROW> * _ptObj );

		// Gets an item by index accounting for children being expanded or not.
		ee::CTree<LSW_TREE_ROW> *			ItemByIndex( size_t _stIdx );

		// Gets an item by index accounting for children being expanded or not.
		ee::CTree<LSW_TREE_ROW> *			ItemByIndex( ee::CTree<LSW_TREE_ROW> * _ptStart, size_t _stIdx, size_t _stStartIdx );

		// Counts the total number of expanded items.
		size_t								CountExpanded() const;

		// Gets the indentation level for an item.
		INT									GetIndent( ee::CTree<CTreeListView::LSW_TREE_ROW> * _ptThis ) const;

		// Gets the next item based on expansion.
		ee::CTree<CTreeListView::LSW_TREE_ROW> *
											NextByExpansion( ee::CTree<CTreeListView::LSW_TREE_ROW> * _ptThis ) const;

		// WM_SIZE.
		virtual LSW_HANDLED					Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		// WM_MOVE.
		virtual LSW_HANDLED					Move( LONG _lX, LONG _lY );

		// Evaluates expressions to determine a new rectangle for the control.
		virtual void						EvalNewSize();

		// List-view window procedure.  The list-view is hidden.
		static LRESULT CALLBACK				ListViewOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

		// Converts a TVITEMEXW to a LSW_TREE_ROW.
		static LSW_TREE_ROW					ItemExToTreeRow( const TVITEMEXW &_ieItemEx );
	};

}	// namespace lsw

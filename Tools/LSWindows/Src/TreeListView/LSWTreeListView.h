#pragma once
#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../ListView/LSWListView.h"
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

		// A helper to easily create a tree view item to be inserted with only text.
		static TVINSERTSTRUCTW				DefaultItem( const WCHAR * _pwcText, HTREEITEM _tiParent = TVI_ROOT, HTREEITEM _tiInsertAfter = TVI_LAST );

		// A helper to easily create a tree view item to be inserted with only text and data.
		static TVINSERTSTRUCTW				DefaultItemLParam( const WCHAR * _pwcText, LPARAM _lpParm, HTREEITEM _tiParent = TVI_ROOT, HTREEITEM _tiInsertAfter = TVI_LAST );


	protected :
		// == Types.
		// Internal book-keeping for each row.
		struct LSW_TREE_ROW {
			std::vector<std::wstring>		vStrings;		// Length matches GetColumnCount().
			LPARAM							lpParam;		// The lParam originally associated with the tree item for this row.
			//HTREEITEM						htiItem;		// The tree item associated with this data.
			UINT							uiState;		// State information.
			UINT							uiStateEx;		// Additional state information.  Manages multi-select.
			int								iImage;			// Index in the tree-view control's image list of the icon image to use when the item is in the nonselected state.
			int								iSelectedImage;	// Index in the tree-view control's image list of the icon image to use when the item is in the selected state.
			int								iExpandedImage;	// Index of the image in the control's image list to display when the item is in the expanded state.
			//std::vector<LSW_TREE_ROW>		vChildren;		// Its children.
			//LSW_TREE_ROW *					ptiParent;		// Its parent.
		};


		// == Members.
		// The list view.
		HWND								m_hListView;
		// Original list-view message handler.
		WNDPROC								m_wpListViewProc;
		// Book keeping.  One entry per row.
		std::vector<LSW_TREE_ROW>			m_vRows;
		// Window property.
		static WCHAR						m_szProp[2];


		// == Functions.
#if 0
		// WM_SIZE.
		virtual LSW_HANDLED					Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		// WM_MOVE.
		virtual LSW_HANDLED					Move( LONG _lX, LONG _lY );
#endif

		// List-view window procedure.  The list-view is hidden.
		static LRESULT CALLBACK				ListViewOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );
	};

}	// namespace lsw

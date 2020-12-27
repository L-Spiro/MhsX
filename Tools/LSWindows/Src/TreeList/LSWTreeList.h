#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"


#define LSW_TREELIST_LEFT_MARGIN			3


namespace lsw {

	class CTreeList : public CWidget {
	public :
		CTreeList( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CTreeList();

		// == Functions.
		// Gets the content width.
		INT									ContentWidth() const;

		// Gets the content height.
		INT									ContentHeight() const;

		// Inserts a column.
		BOOL								InsertColumn( const WCHAR * _pwcText, INT _iWidth, INT _iIdx );

		// Gets the total number of columns in the tree.
		int									GetColumnCount() const { return Header_GetItemCount( m_hHeader ); }

		// Sets the text of a column.
		BOOL								SetColumnText( const WCHAR * _pwcText, INT _iIdx );

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

		// Sets the width of a column.
		BOOL								SetColumnWidth( INT _iIdx, INT _iWidth );

		// Gets the width of a column or -1.
		INT									GetColumnWidth( INT _iIdx );

		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_TREELIST; }

		// Returns true if this is a CTreeList class.
		virtual bool						IsTreeList() const { return true; }

		// Tree-view notification for an expanded item.
		virtual BOOL						ItemExpandedNotify( NMTREEVIEWW * _ptvNotif ) {
			Redraw( true );
			return 0; /* Return value is ignored. */
		}

		// Gets the maximum width in pixels of a column.
		virtual INT							GetColumnTextWidth( INT _iIdx );

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
		};

		struct LSW_PAINTPARMS {
			RECT							rClient;
			INT								iHeaderCount;
			HTREEITEM						hStart;
			INT								iIndex;
		};

		// == Members.
		// The list view.
		HWND								m_hListView;
		// The tree view.
		HWND								m_hTreeView;
		// The header.
		HWND								m_hHeader;
		// Horizontal scroll.
		INT									m_iHScroll;
		// Vertical scroll.
		INT									m_iVScroll;
		// Header height.
		UINT								m_uiHeaderHeight;
		// Original list-view message handler.
		WNDPROC								m_wpListViewProc;
		// Original tree-view message handler.
		WNDPROC								m_wpTreeViewProc;
		// Original tree-list message handler.
		WNDPROC								m_wpTreeListProc;
		// Original header message handler.
		WNDPROC								m_wpHeaderProc;
		// Book keeping.  One entry per row.
		std::vector<LSW_TREE_ROW>			m_vRows;
		// Lock for drawing.
		bool								m_bLocked;
		// Window property.
		static WCHAR						m_szProp[2];


		// == Functions.
		// WM_SIZE.
		virtual LSW_HANDLED					Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		// WM_MOVE.
		virtual LSW_HANDLED					Move( LONG _lX, LONG _lY );

		// WM_HSCROLL
		virtual LSW_HANDLED					HScroll( USHORT _uScrollPos, USHORT _uScrollType, HWND _hSender );

		// WM_LBUTTONDOWN.
		virtual LSW_HANDLED					LButtonDown( DWORD _dwVirtKeys, const POINTS &_pCursorPos );

		// Notification that a header is being dragged.
		virtual BOOL						NotifyHeaderDrag( LPNMHEADERW _lphHeader );

		// Notification that a header bar was double-clicked (for automatic resizing).
		virtual BOOL						NotifyHeaderDividerDblClick( LPNMHEADERW _lphHeader );

		// Updates the horizontal scroll settings.
		void								UpdateHorScroll();

		// Updates the vertical scroll settings.
		void								UpdateVertScroll();

		// Resizes the inner controls.
		void								ResizeControls( const LSW_RECT &_rRect );

		// Gets the height of the tree view counting only expanded items.
		DWORD								GetTreeHeight( HTREEITEM _tiItem ) const;

		// Gets the index of a tree-view item or -1.
		INT									GetIndexOf( HTREEITEM _tiItemToFind, HTREEITEM tiItemStart, INT &_iPos ) const;

		// Gets the tree item under the given mouse position.
		HTREEITEM							TreeItemAt( SHORT _sX, SHORT _sY ) const;

		// Handle painting.
		BOOL								OnPaint( WPARAM _wParam, LPARAM _lParam );

		// Draw the tree items.
		void								DrawTreeItems( HDC _hDc, LSW_PAINTPARMS &_ppParms, PAINTSTRUCT &_psPaintStruct );

		// Gets the header left and right pixel coordinates in Wnd() client space.
		LSW_RECT							GetHeaderLandR( INT _iColumn, bool _bApplyScroll = true ) const;

		// Redraws the tree-list.
		void								Redraw( bool _bUpdateNow = false );

		// Gets a tree item's state.
		UINT								GetTreeItemState( HTREEITEM _tiItem );

		// Takes an item's rectangle and produces a new rectangle where an expand/contract icon should be displayed.
		static LSW_RECT						GetExpandIconRectFromItemRect( const LSW_RECT &_rItem );

		// List-view window procedure.  The list-view is hidden.
		static LRESULT CALLBACK				ListViewOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );
		
		// Tree-view window procedure.  Overrides the Win32 tree-view control.
		static LRESULT CALLBACK				TreeViewOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

		// Tree-list window procedure.  Represents this actual final control.
		static LRESULT CALLBACK				TreeListOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );
	};

}	// namespace lsw

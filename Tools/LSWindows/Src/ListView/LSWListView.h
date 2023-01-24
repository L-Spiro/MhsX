#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CListView : public CWidget {
	public :
		CListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Set sort case sensitivity.
		VOID								SetSortCaseSensitivity( BOOL _bVal ) { m_bSortWithCase = (_bVal != FALSE); }

		// If the list-view control was created without the LVS_OWNERDATA style, this macro causes the control to allocate its internal data structures for
		//	the specified number of items. This prevents the control from having to allocate the data structures every time an item is added.
		VOID								SetItemCount( INT _cItems );

		// Sets the virtual number of items in a virtual list view.
		VOID								SetItemCountEx( INT _cItems, DWORD _dwFlags );

		// Gets the number of items in a list-view control.
		int									GetItemCount() const;

		// Determines the number of selected items in a list-view control.
		UINT								GetSelectedCount() const;

		// Removes an item from a list-view control.
		BOOL								DeleteItem( int _iItem );

		// Gets the number of columns.
		INT									GetTotalColumns() const;

		// Inserts a column at the given index.
		INT									InsertColumn( INT _iIndex, const WCHAR * _pwcText, INT _iFormat = LVCFMT_LEFT );

		// Inserts a column at the given index.
		INT									InsertColumn( INT _iIndex, const CHAR * _pcText, INT _iFormat = LVCFMT_LEFT );

		// Inserts a column.
		BOOL								InsertColumn( const WCHAR * _pwcText, INT _iWidth, INT _iIdx );

		// Adds a column with the given text.
		INT									AddColumn( const CHAR * _pcText, INT _iFormat = LVCFMT_LEFT );

		// Adds a column with the given text.
		INT									AddColumn( const WCHAR * _pwcText, INT _iFormat = LVCFMT_LEFT );

		// Sets the width of a column.
		BOOL								SetColumnWidth( INT _iCol, INT _iWidth );

		// Gets the width of a column.
		INT									GetColumnWidth( INT _iCol ) const;

		// Sets the text of a column.
		BOOL								SetColumnText( const WCHAR * _pwcText, INT _iIdx );

		// Deletes a column.
		BOOL								DeleteColumn( INT _iCol );

		// Deletes all columns.
		VOID								DeleteAllColumns();

		// Gets the total number of columns in the list view.
		int									GetColumnCount() const { return Header_GetItemCount( ListView_GetHeader( Wnd() ) ); }

		// Inserts an item.  Returns the index of the item.
		INT									InsertItem( const LVITEMW &_iItem );

		// Inserts an item.  Returns the index of the item.
		INT									InsertItem( const LVITEMA &_iItem );

		// Inserts an item that consistes of text and a parameter.
		INT									InsertItem( const WCHAR * _pwcText, LPARAM _lParam );

		// Inserts an item that consistes of text and a parameter.
		INT									InsertItem( const CHAR * _pcText, LPARAM _lParam );

		// Sets the text for an item.
		VOID								SetItemText( INT _iItem, INT _iSubItem, const WCHAR * _pwcText );

		// Sets the text for an item.
		VOID								SetItemText( INT _iItem, INT _iSubItem, const CHAR * _pcText );

		// Gets the length of an item's text in WCHAR units.
		INT									GetItemTextLenW( INT _iItem, INT _iSubItem );

		// Gets the length of an item's text in CHAR units.
		INT									GetItemTextLenA( INT _iItem, INT _iSubItem );

		// Gets the text of an item.
		VOID								GetItemText( INT _iItem, INT _iSubItem, std::wstring &_sRet );

		// Gets the text of an item.
		VOID								GetItemText( INT _iItem, INT _iSubItem, std::string &_sRet );

		// Gets the width of a string.
		int									GetStringWidth( const std::wstring &_wsStr ) const {
			return static_cast<int>(::SendMessageW( Wnd(), LVM_GETSTRINGWIDTHW, 0, reinterpret_cast<LPARAM>(_wsStr.c_str()) ));
		}

		// Gets the width of a string.
		int									GetStringWidth( const std::string &_sStr ) const {
			return static_cast<int>(::SendMessageA( Wnd(), LVM_GETSTRINGWIDTHA, 0, reinterpret_cast<LPARAM>(_sStr.c_str()) ));
		}

		// Gets the width of a string.
		int									GetStringWidth( const WCHAR * _pwcText ) const {
			return static_cast<int>(::SendMessageW( Wnd(), LVM_GETSTRINGWIDTHW, 0, reinterpret_cast<LPARAM>(_pwcText) ));
		}

		// Gets the width of a string.
		int									GetStringWidth( const CHAR * _pcText ) const {
			return static_cast<int>(::SendMessageA( Wnd(), LVM_GETSTRINGWIDTHA, 0, reinterpret_cast<LPARAM>(_pcText) ));
		}

		// Gets the index of the (first) selected item or -1.
		INT									GetFirstSelectedItem() const;

		// Gets the data of the selected item or returns -1.
		LPARAM								GetSelData() const;

		/**
		 * Sets selection on an item by index.
		 *
		 * \param _iItem The item to update.
		 * \param _bSelected Whether the item is selected or not.
		 */
		void								SetItemSelection( INT _iItem, BOOL _bSelected );

		/**
		 * Sets highlight on an item by index.
		 *
		 * \param _iItem The item to update.
		 * \param _bSelected Whether the item is highlighted or not.
		 */
		void								SetItemHighlight( INT _iItem, BOOL _bHighlighted );


		// Gets an item.  _iItm is input and output.
		BOOL								GetItem( INT _iItem, INT _iSubItem, LVITEMW &_iItm ) const;

		// Gets an item.  _iItm is input and output.
		BOOL								GetItem( INT _iItem, INT _iSubItem, LVITEMA &_iItm ) const;

		// Creates an array of indices that correspond to the selected items in the list-view.
		bool								GetSelectedItems( std::vector<int> &_vArray ) const;

		// Gets the item's state.
		UINT								GetItemState( INT _iItem, UINT _uiMask ) const;

		// Indicates whether an item in the list-view control is visible.
		BOOL								IsItemVisible( INT _iItem ) const;

		// Sort items.
		virtual BOOL						SortItems( INT _iSubItem );

		// Sort comparison function.  Override to change how items compare against each other.
		virtual int							SortCompare( INT _iLeft, INT _iRight, INT _iSub );

		/**
		 * Deletes all items.
		 */
		virtual VOID						DeleteAll();

		// Delete all items (notification responder).
		virtual BOOL						DeleteAllNotify() { return 1; }

		// Requesting information (notification responder).
		virtual BOOL						GetDispInfoNotify( NMLVDISPINFOW * /*_plvdiInfo*/ ) { return 1; }

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
		 * Determines the type of control this is.
		 *
		 * \return Returns LSW_LT_LISTVIEW.
		 */
		virtual uint32_t					WidgetType() const { return LSW_LT_LISTVIEW; }

		/**
		 * Returns true if this is a CListView class.
		 *
		 * \return Returns true.
		 */
		virtual bool						IsListView() const { return true; }


	protected :
		// == Types.
		struct LSW_LISTSORT {
			CListView *						plvListView;
			INT								iSubItem;
		};

		// == Members.
		// The number of columns.
		SIZE_T								m_sColumns;

		// Sort with case-sensitivity or not.
		BOOL								m_bSortWithCase;


		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

		// Sort routine.
		static int CALLBACK					CompareFunc( LPARAM _lParam1, LPARAM _lParam2, LPARAM _lParamSort );


	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw

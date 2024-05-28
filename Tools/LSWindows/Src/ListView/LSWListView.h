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

		/**
		 * If the list-view control was created without the LVS_OWNERDATA style, this macro causes the control to allocate its internal data structures for
		 *	the specified number of items. This prevents the control from having to allocate the data structures every time an item is added.
		 * 
		 * \param _cItems The number of items for which the list-view control should allocate memory.
		 **/
		VOID								SetItemCount( INT _cItems );

		/**
		 * Sets the virtual number of items in a virtual list view.
		 * 
		 * \param _cItems The number of items that the list-view control will contain
		 * \param _dwFlags Values that specify the behavior of the list-view control after resetting the item count. This value can be a combination of the following: LVSICF_NOINVALIDATEALL, LVSICF_NOSCROLL.
		 **/
		VOID								SetItemCountEx( INT _cItems, DWORD _dwFlags );

		/**
		 * Gets the number of items in a list-view control.
		 * 
		 * \return Returns the number of items in a list-view control
		 **/
		int									GetItemCount() const;

		/**
		 * Determines the number of selected items in a list-view control.
		 * 
		 * \return Returns the number of selected items in a list-view control.
		 **/
		UINT								GetSelectedCount() const;

		/**
		 * Removes an item from a list-view control.
		 * 
		 * \param _iItem An index of the list-view item to delete.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								DeleteItem( int _iItem );

		/**
		 * Gets the number of columns.
		 * 
		 * \return Returns the number of columns.
		 **/
		INT									GetTotalColumns() const;

		/**
		 * Inserts a column at the given index.
		 * 
		 * \param _iIndex The index of the item after which the new item is to be inserted. The new item is inserted at the end of the header control if _iIndex is greater than or equal to the number of items in the control. If _iIndex is zero, the new item is inserted at the beginning of the header control.
		 * \param _pwcText A pointer to an item string.
		 * \param _iFormat Flags that specify the item's format: [HDF_CENTER, HDF_LEFT, HDF_RIGHT], [HDF_BITMAP, HDF_BITMAP_ON_RIGHT, HDF_OWNERDRAW, HDF_STRING], [HDF_IMAGE, HDF_JUSTIFYMASK, HDF_RTLREADING, HDF_SORTDOWN, HDF_SORTUP, HDF_CHECKBOX, HDF_CHECKED, HDF_FIXEDWIDTH, HDF_SPLITBUTTON].
		 * \return Returns the index of the inserted item or -1.
		 **/
		INT									InsertColumn( INT _iIndex, const WCHAR * _pwcText, INT _iFormat = LVCFMT_LEFT );

		/**
		 * Inserts a column at the given index.
		 * 
		 * \param _iIndex The index of the item after which the new item is to be inserted. The new item is inserted at the end of the header control if _iIndex is greater than or equal to the number of items in the control. If _iIndex is zero, the new item is inserted at the beginning of the header control.
		 * \param _pcText A pointer to an item string.
		 * \param _iFormat Flags that specify the item's format: [HDF_CENTER, HDF_LEFT, HDF_RIGHT], [HDF_BITMAP, HDF_BITMAP_ON_RIGHT, HDF_OWNERDRAW, HDF_STRING], [HDF_IMAGE, HDF_JUSTIFYMASK, HDF_RTLREADING, HDF_SORTDOWN, HDF_SORTUP, HDF_CHECKBOX, HDF_CHECKED, HDF_FIXEDWIDTH, HDF_SPLITBUTTON].
		 * \return Returns the index of the inserted item or -1.
		 **/
		INT									InsertColumn( INT _iIndex, const CHAR * _pcText, INT _iFormat = LVCFMT_LEFT );

		/**
		 * Inserts a column at the given index.
		 *
		 * \param _pwcText A pointer to an item string.
		 * \param _iWidth The width of the item.
		 * \param _iIdx The index of the item after which the new item is to be inserted. The new item is inserted at the end of the header control if _iIndex is greater than or equal to the number of items in the control. If _iIndex is zero, the new item is inserted at the beginning of the header control.
		 * \return Returns TRUE if the item was inserted.
		 **/
		BOOL								InsertColumn( const WCHAR * _pwcText, INT _iWidth, INT _iIdx );

		/**
		 * Adds a column with the given text.
		 * 
		 * \param _pcText A pointer to an item string.
		 * \param _iFormat Flags that specify the item's format: [HDF_CENTER, HDF_LEFT, HDF_RIGHT], [HDF_BITMAP, HDF_BITMAP_ON_RIGHT, HDF_OWNERDRAW, HDF_STRING], [HDF_IMAGE, HDF_JUSTIFYMASK, HDF_RTLREADING, HDF_SORTDOWN, HDF_SORTUP, HDF_CHECKBOX, HDF_CHECKED, HDF_FIXEDWIDTH, HDF_SPLITBUTTON].
		 * \return Returns the index of the inserted item or -1.
		 **/
		INT									AddColumn( const CHAR * _pcText, INT _iFormat = LVCFMT_LEFT );

		/**
		 * Adds a column with the given text.
		 * 
		 * \param _pwcText A pointer to an item string.
		 * \param _iFormat Flags that specify the item's format: [HDF_CENTER, HDF_LEFT, HDF_RIGHT], [HDF_BITMAP, HDF_BITMAP_ON_RIGHT, HDF_OWNERDRAW, HDF_STRING], [HDF_IMAGE, HDF_JUSTIFYMASK, HDF_RTLREADING, HDF_SORTDOWN, HDF_SORTUP, HDF_CHECKBOX, HDF_CHECKED, HDF_FIXEDWIDTH, HDF_SPLITBUTTON].
		 * \return Returns the index of the inserted item or -1.
		 **/
		INT									AddColumn( const WCHAR * _pwcText, INT _iFormat = LVCFMT_LEFT );

		/**
		 * Sets the width of a column.
		 * 
		 * \param _iCol The zero-based index of a valid column. For list-view mode, this parameter must be set to zero.
		 * \param _iWidth The new width of the column, in pixels. For report-view mode, the following special values are supported: LVSCW_AUTOSIZE, LVSCW_AUTOSIZE_USEHEADER.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								SetColumnWidth( INT _iCol, INT _iWidth );

		/**
		 * Gets the width of a column.
		 * 
		 * \param _iCol The index of the column. This parameter is ignored in list view.
		 * \return Returns the column width if successful, or zero otherwise. If this message is sent to a list-view control with the LVS_REPORT style and the specified column does not exist, the return value is undefined.
		 **/
		INT									GetColumnWidth( INT _iCol ) const;

		/**
		 * Sets the text of a column.
		 * 
		 * \param _pwcText The address of a null-terminated string that contains the column header text.
		 * \param _iIdx Index of the column.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								SetColumnText( const WCHAR * _pwcText, INT _iIdx );

		/**
		 * Deletes a column.
		 * 
		 * \param _iCol An index of the column to delete.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								DeleteColumn( INT _iCol );

		/**
		 * Deletes all columns.
		 **/
		VOID								DeleteAllColumns();

		/**
		 * Gets the total number of columns in the list view.
		 * 
		 * \return Returns the total number of columns in the list view.
		 **/
		int									GetColumnCount() const { return Header_GetItemCount( ListView_GetHeader( Wnd() ) ); }

		/**
		 * Inserts an item.
		 * 
		 * \param _iItem A pointer to an LVITEMW structure that specifies the attributes of the list-view item. Use the iItem member to specify the zero-based index at which the new item should be inserted. If this value is greater than the number of items currently contained by the listview control, the new item will be appended to the end of the list and assigned the correct index.
		 * \return Returns the index of the inserted item.
		 **/
		INT									InsertItem( const LVITEMW &_iItem );

		/**
		 * Inserts an item.
		 * 
		 * \param _iItem A pointer to an LVITEMA structure that specifies the attributes of the list-view item. Use the iItem member to specify the zero-based index at which the new item should be inserted. If this value is greater than the number of items currently contained by the listview control, the new item will be appended to the end of the list and assigned the correct index.
		 * \return Returns the index of the inserted item.
		 **/
		INT									InsertItem( const LVITEMA &_iItem );

		/**
		 * Inserts an item that consistes of text and a parameter.
		 * 
		 * \param _pwcText The text of the item.
		 * \param _lParam The LPARAM to associate with the item.
		 * \return Returns the index of the inserted item.
		 **/
		INT									InsertItem( const WCHAR * _pwcText, LPARAM _lParam );

		/**
		 * Inserts an item that consistes of text and a parameter.
		 * 
		 * \param _pcText The text of the item.
		 * \param _lParam The LPARAM to associate with the item.
		 * \return Returns the index of the inserted item.
		 **/
		INT									InsertItem( const CHAR * _pcText, LPARAM _lParam );

		/**
		 * Sets the text for an item.
		 * 
		 * \param _iItem The zero-based index of the list-view item
		 * \param _iSubItem The one-based index of the subitem. To set the item label, set _iSubItem to zero.
		 * \param _pwcText A pointer to a null-terminated string that contains the new text. This parameter can be LPSTR_TEXTCALLBACK to indicate a callback item for which the parent window stores the text. In this case, the list-view control sends the parent an LVN_GETDISPINFO notification code when it needs the text. This parameter can be NULL.
		 **/
		VOID								SetItemText( INT _iItem, INT _iSubItem, const WCHAR * _pwcText );

		/**
		 * Sets the text for an item.
		 * 
		 * \param _iItem The zero-based index of the list-view item
		 * \param _iSubItem The one-based index of the subitem. To set the item label, set _iSubItem to zero.
		 * \param _pcText A pointer to a null-terminated string that contains the new text. This parameter can be LPSTR_TEXTCALLBACK to indicate a callback item for which the parent window stores the text. In this case, the list-view control sends the parent an LVN_GETDISPINFO notification code when it needs the text. This parameter can be NULL.
		 **/
		VOID								SetItemText( INT _iItem, INT _iSubItem, const CHAR * _pcText );

		/**
		 * Gets the length of an item's text in WCHAR units.
		 * 
		 * \param _iItem The zero-based index of the list-view item
		 * \param _iSubItem The one-based index of the subitem. To set the item label, set _iSubItem to zero.
		 * \return Returns the length of the item's string, including the terminating NULL.
		 **/
		INT									GetItemTextLenW( INT _iItem, INT _iSubItem );

		/**
		 * Gets the length of an item's text in CHAR units.
		 * 
		 * \param _iItem The zero-based index of the list-view item
		 * \param _iSubItem The one-based index of the subitem. To set the item label, set _iSubItem to zero.
		 * \return Returns the length of the item's string, including the terminating NULL.
		 **/
		INT									GetItemTextLenA( INT _iItem, INT _iSubItem );

		/**
		 * Gets the text of an item.
		 * 
		 * \param _iItem The index of the list-view item
		 * \param _iSubItem The index of the subitem. To retrieve the item text, set _iSubItem to zero.
		 * \param _sRet Holds the returned string.
		 **/
		VOID								GetItemText( INT _iItem, INT _iSubItem, std::wstring &_sRet );

		/**
		 * Gets the text of an item.
		 * 
		 * \param _iItem The index of the list-view item
		 * \param _iSubItem The index of the subitem. To retrieve the item text, set _iSubItem to zero.
		 * \param _sRet Holds the returned string.
		 **/
		VOID								GetItemText( INT _iItem, INT _iSubItem, std::string &_sRet );

		/**
		 * Determines the width of a specified string using the specified list-view control's current font.
		 * 
		 * \param _wsStr The string to width-test.
		 * \return Returns the string width if successful, or zero otherwise
		 **/
		int									GetStringWidth( const std::wstring &_wsStr ) const {
			return static_cast<int>(::SendMessageW( Wnd(), LVM_GETSTRINGWIDTHW, 0, reinterpret_cast<LPARAM>(_wsStr.c_str()) ));
		}

		/**
		 * Determines the width of a specified string using the specified list-view control's current font.
		 * 
		 * \param _sStr The string to width-test.
		 * \return Returns the string width if successful, or zero otherwise
		 **/
		int									GetStringWidth( const std::string &_sStr ) const {
			return static_cast<int>(::SendMessageA( Wnd(), LVM_GETSTRINGWIDTHA, 0, reinterpret_cast<LPARAM>(_sStr.c_str()) ));
		}

		/**
		 * Determines the width of a specified string using the specified list-view control's current font.
		 * 
		 * \param _pwcText The string to width-test.
		 * \return Returns the string width if successful, or zero otherwise
		 **/
		int									GetStringWidth( const WCHAR * _pwcText ) const {
			return static_cast<int>(::SendMessageW( Wnd(), LVM_GETSTRINGWIDTHW, 0, reinterpret_cast<LPARAM>(_pwcText) ));
		}

		/**
		 * Determines the width of a specified string using the specified list-view control's current font.
		 * 
		 * \param _pcText The string to width-test.
		 * \return Returns the string width if successful, or zero otherwise
		 **/
		int									GetStringWidth( const CHAR * _pcText ) const {
			return static_cast<int>(::SendMessageA( Wnd(), LVM_GETSTRINGWIDTHA, 0, reinterpret_cast<LPARAM>(_pcText) ));
		}

		/**
		 * Gets the bounding rectangle for all or part of an item in the current view.
		 * 
		 * \param _iItem The index of the list-view item.
		 * \param _iCode The portion of the list-view item from which to retrieve the bounding rectangle. This parameter must be one of the following values: LVIR_BOUNDS, LVIR_ICON, LVIR_LABEL, LVIR_SELECTBOUNDS.
		 * \return Returns the item's RECT.
		 **/
		LSW_RECT							GetItemRect( int _iItem, int _iCode );

		/**
		 * Gets the index of the (first) selected item or -1.
		 *
		 * \return Returns the index of the (first) selected item or -1.
		 **/
		INT									GetFirstSelectedItem() const;

		/**
		 * Gets the data of the selected item or returns -1.
		 *
		 * \return Returns the data of the selected item or returns -1.
		 **/
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


		/**
		 * Gets an item.  _iItm is input and output.
		 * 
		 * \param _iItem The index of the list-view item.
		 * \param _iSubItem The index of the subitem. To retrieve the item text, set _iSubItem to zero.
		 * \param _iItm A pointer to an LVITEMW structure that specifies the information to retrieve and receives information about the list-view item.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								GetItem( INT _iItem, INT _iSubItem, LVITEMW &_iItm ) const;

		/**
		 * Gets an item.  _iItm is input and output.
		 * 
		 * \param _iItem The index of the list-view item.
		 * \param _iSubItem The index of the subitem. To retrieve the item text, set _iSubItem to zero.
		 * \param _iItm A pointer to an LVITEMA structure that specifies the information to retrieve and receives information about the list-view item.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								GetItem( INT _iItem, INT _iSubItem, LVITEMA &_iItm ) const;

		/**
		 * Creates an array of indices that correspond to the selected items in the list-view.
		 * 
		 * \param _vArray Holds the returned items, appended to end of the input vector.
		 * \return Returns true if no allocation issues were encountered during the operation and the control is valid.
		 **/
		bool								GetSelectedItems( std::vector<int> &_vArray ) const;

		/**
		 * Gets the item's state.
		 * 
		 * \param _iItem The index of the list-view item.
		 * \param _uiMask The state information to retrieve. This parameter can be a combination of the following values: LVIS_CUT, LVIS_DROPHILITED, LVIS_FOCUSED, LVIS_SELECTED, LVIS_OVERLAYMASK, LVIS_STATEIMAGEMASK.
		 * \return Returns the current state for the specified item. The only valid bits in the return value are those that correspond to the bits set in the _uiMask parameter.
		 **/
		UINT								GetItemState( INT _iItem, UINT _uiMask ) const;

		/**
		 * Indicates whether an item in the list-view control is visible.
		 * 
		 * \param _iItem The index of the list-view item.
		 * \return Returns TRUE if the control and item are valid and the item is visible.
		 **/
		BOOL								IsItemVisible( INT _iItem ) const;

		/**
		 * Sorts items.
		 * 
		 * \param _iSubItem The index of the sub-item.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		virtual BOOL						SortItems( INT _iSubItem );

		/**
		 * Sort comparison function.  Override to change how items compare against each other.
		 * 
		 * \param _iLeft Left operand.
		 * \param _iRight Right operand.
		 * \param _iSub Sub-item to use for text compare.
		 * \return Returns a lexicographical comparison value.
		 **/
		virtual int							SortCompare( INT _iLeft, INT _iRight, INT _iSub );

		/**
		 * Deletes all items.
		 */
		virtual VOID						DeleteAll();

		/**
		 * Delete all items (notification responder).
		 *
		 * \return Returns TRUE;
		 **/
		virtual BOOL						DeleteAllNotify() { return TRUE; }

		/**
		 * Requesting information (notification responder).
		 *
		 * \return Returns TRUE.
		 **/
		virtual BOOL						GetDispInfoNotify( NMLVDISPINFOW * /*_plvdiInfo*/ ) { return TRUE; }

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
		SIZE_T								m_sColumns;						/**< The number of columns. */
		BOOL								m_bSortWithCase;				/**< Sort with case-sensitivity or not. */


		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

		// Sort routine.
		static int CALLBACK					CompareFunc( LPARAM _lParam1, LPARAM _lParam2, LPARAM _lParamSort );


	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw

#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	/**
	 * A list-view (SysListView32) control.
	 * 
	 * Wraps the Win32 list-view control with helpers for columns, items, selection, and sorting.
	 *
	 * In report view the right-most column snaps to the control's right edge: it stretches to fill the control whenever the columns are
	 *	narrower than the control, but never gets narrower than its internal width, so a control narrower than that gets a horizontal scroll
	 *	bar instead.  See SetRightColumnSnap().
	 **/
	class CListView : public CWidget {
	public :
		CListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		virtual ~CListView();


		// == Functions.
		/**
		 * Sets whether sorting is case-sensitive.
		 *
		 * \param _bVal TRUE to sort with case sensitivity, FALSE to sort without it.
		 **/
		VOID								SetSortCaseSensitivity( BOOL _bVal ) { m_bSortWithCase = (_bVal != FALSE); }

		/**
		 * Enables or disables snapping the right-most column to the control's right edge (enabled by default).
		 *
		 * While enabled, the right-most column's width is the larger of its internal width and the width the other columns leave free.  Its
		 *	internal width is the width the application or the user last gave it (LVSCW_AUTOSIZE_USEHEADER sizes it to its header and items,
		 *	as it does any other column).
		 *
		 * \param _bEnable TRUE to snap the right-most column to the right edge, FALSE to return it to its internal width and leave it alone.
		 **/
		VOID								SetRightColumnSnap( BOOL _bEnable );

		/**
		 * Determines whether the right-most column snaps to the control's right edge.
		 *
		 * \return Returns TRUE if the right-most column snaps to the control's right edge.
		 **/
		BOOL								RightColumnSnap() const { return m_bRightColumnSnap; }

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
		 * Gets the caret (focused) item index from a SysListView32 control.
		 * 
		 * Uses LVNI_FOCUSED. Returns -1 if no item has the focus state.
		 * 
		 * \return Returns the zero-based caret index, or -1 if none.
		 **/
		inline int							GetCaretIndex() {
			return static_cast<int>(::SendMessageW( Wnd(), LVM_GETNEXTITEM, static_cast<WPARAM>(-1), MAKELPARAM( LVNI_FOCUSED, 0 ) ));
		}
		
		/**
		 * Gets the selection anchor (gselection markh) index.
		 * 
		 * The selection mark is the anchor used for Shift+click range selection and
		 * can differ from the focused item. Returns -1 if not set.
		 * 
		 * \return Returns the zero-based selection mark index, or -1 if none.
		 **/
		inline int							GetListViewSelectionMark() {
			return static_cast<int>(::SendMessageW( Wnd(), LVM_GETSELECTIONMARK, 0, 0 ));
		}

		/**
		 * Gets information about the rectangle that surrounds a subitem in a list-view control.
		 * 
		 * \param _iItem The index of the subitem's parent item.
		 * \param _iSubItem The one-based index of the subitem.
		 * \param _iCode A portion of the list-view subitem for which to retrieve the bounding rectangle information. This value can be one of the following: LVIR_BOUNDS, LVIR_ICON, or LVIR_LABEL.
		 * \param _pRc Pointer to a RECT structure that receives the subitem bounding rectangle information
		 * \return Returns nonzero if successful, or zero otherwise.
		 **/
		inline BOOL							GetSubItemRect( int _iItem, int _iSubItem, int _iCode, LPRECT _pRc ) {
			return static_cast<BOOL>(::SendMessageW( Wnd(), LVM_GETSUBITEMRECT, static_cast<WPARAM>(_iItem),
				(_pRc ? ((_pRc->top = _iSubItem), (_pRc->left = _iCode), reinterpret_cast<LPARAM>(_pRc)) :
				reinterpret_cast<LPARAM>(nullptr))));
		}

		/**
		 * Determines which list-view item or subitem is located at a given position.
		 * 
		 * \param _plvhtiInfo A pointer to an LVHITTESTINFO structure. The POINT structure within LVHITTESTINFO must be set to the client coordinates to be hit-tested.
		 * \return Returns the index of the item or subitem tested, if any, or -1 otherwise. If an item or subitem is at the given coordinates, the fields of the LVHITTESTINFO structure will be filled with the applicable hit information.
		 **/
		inline INT							SubItemHitTest( LPLVHITTESTINFO _plvhtiInfo ) {
			return ListView_SubItemHitTest( Wnd(), _plvhtiInfo );
		}

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
		virtual BOOL						DeleteColumn( INT _iCol );

		/**
		 * Deletes all columns.
		 **/
		virtual VOID						DeleteAllColumns();

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
		INT									GetItemTextLenW( INT _iItem, INT _iSubItem ) const;

		/**
		 * Gets the length of an item's text in CHAR units.
		 * 
		 * \param _iItem The zero-based index of the list-view item
		 * \param _iSubItem The one-based index of the subitem. To set the item label, set _iSubItem to zero.
		 * \return Returns the length of the item's string, including the terminating NULL.
		 **/
		INT									GetItemTextLenA( INT _iItem, INT _iSubItem ) const;

		/**
		 * Gets the text of an item.
		 * 
		 * \param _iItem The index of the list-view item
		 * \param _iSubItem The index of the subitem. To retrieve the item text, set _iSubItem to zero.
		 * \param _sRet Holds the returned string.
		 **/
		VOID								GetItemText( INT _iItem, INT _iSubItem, std::wstring &_sRet ) const;

		/**
		 * Gets the text of an item.
		 * 
		 * \param _iItem The index of the list-view item
		 * \param _iSubItem The index of the subitem. To retrieve the item text, set _iSubItem to zero.
		 * \param _sRet Holds the returned string.
		 **/
		VOID								GetItemText( INT _iItem, INT _iSubItem, std::string &_sRet ) const;

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
		 * Gets the data of an item by index.
		 * 
		 * \param _iItem The index of the item whose data is to be obtained.
		 * \return Returns the item's data or -1.
		 **/
		LPARAM								GetItemData( INT _iItem ) const;

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
		 * \param _bHighlighted Whether the item is highlighted or not.
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
		 * Snaps the column widths to the control width bi adjusting the given column's width.
		 * 
		 * \param _iCol The index of the column whose width is to be adjusted to make the columns fit perfectly into the control width.
		 * \return Returns the new width of the given column if it exists or -1 if it does not.
		 **/
		virtual INT							FitColumndsToControlWidth( INT _iCol );

		/**
		 * Requesting information (notification responder).
		 *
		 * \param _plvdiInfo A pointer to an NMLVDISPINFOW structure that specifies the item or subitem and the information requested, and receives that information.
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
		/**
		 * Sort data passed to CompareFunc() through its _lParamSort parameter.
		 **/
		struct LSW_LISTSORT {
			CListView *						plvListView;					/**< The list-view control whose items are being sorted. */
			INT								iSubItem;						/**< The index of the sub-item to use for comparisons. */
		};

		// == Members.
		SIZE_T								m_sColumns;						/**< The number of columns. */
		BOOL								m_bSortWithCase;				/**< Sort with case-sensitivity or not. */
		INT									m_iSnapCol;						/**< The index of the right-most column, which snaps to the right edge, or -1. */
		INT									m_iSnapColWidth;				/**< The right-most column's internal width: its width when it is not stretched. */
		INT									m_iSnapTrackCol;				/**< The column whose divider the user is dragging, or -1. */
		INT									m_iSnapInternal;				/**< Non-zero while this class sets a width that must not become the internal width. */
		INT									m_iSnapMsgDepth;				/**< How many of the control's messages are being processed (they nest). */
		BOOL								m_bRightColumnSnap;				/**< Whether the right-most column snaps to the right edge. */
		bool								m_bSnapPending;					/**< A snap has been requested but not performed. */
		bool								m_bSnapPosted;					/**< SnapMessage() has been posted but not received. */
		bool								m_bSnapSubclassed;				/**< The list-view control is subclassed. */


		// == Functions.
		/**
		 * Setting the HWND after the control has been created.
		 * 
		 * \param _hWnd The handle to the control.
		 **/
		virtual void						InitControl( HWND _hWnd );

		/**
		 * Sort routine.  The comparison callback that SortItems() passes to the list-view control.
		 * 
		 * \param _lParam1 The first item to compare.
		 * \param _lParam2 The second item to compare.
		 * \param _lParamSort A pointer to the LSW_LISTSORT structure for the sort in progress.
		 * \return Returns a negative value if the first item should precede the second, a positive value if the first item should follow the second, or zero if the two items are equivalent.
		 **/
		static int CALLBACK					CompareFunc( LPARAM _lParam1, LPARAM _lParam2, LPARAM _lParamSort );

		/**
		 * Subclasses the list-view control so that the right-most column can snap to its right edge.  Does nothing if it is already subclassed.
		 **/
		VOID								InstallSnapSubclass();

		/**
		 * Removes the subclass installed by InstallSnapSubclass().
		 *
		 * \param _hWnd The list-view control's window handle.
		 **/
		VOID								RemoveSnapSubclass( HWND _hWnd );

		/**
		 * Determines whether the right-most column is being snapped: snapping is enabled, the control is subclassed, and it is in report view.
		 *
		 * \return Returns TRUE if the right-most column is being snapped to the control's right edge.
		 **/
		BOOL								SnapActive() const;

		/**
		 * Requests a snap without performing it.  The snap happens when the posted SnapMessage() arrives, by which time the control has finished
		 *	whatever it was doing.  Requests made in the meantime are merged into one.
		 **/
		VOID								RequestSnap();

		/**
		 * Snaps the right-most column immediately, repeating (a few times at most) while doing so adds or removes a scroll bar and so requests
		 *	another snap.  Only call this while the control is not processing any of its own messages.
		 **/
		VOID								SnapNow();

		/**
		 * Snaps the right-most column once: its width becomes the larger of its internal width and the width the other columns leave free.
		 **/
		VOID								SnapOnce();

		/**
		 * Keeps track of which column is right-most.  When that changes, the previous right-most column goes back to its internal width and the
		 *	new one is adopted with its current width as its internal width.
		 **/
		VOID								UpdateSnapColumn();

		/**
		 * Returns the right-most column to its internal width.
		 **/
		VOID								RestoreSnapColumn();

		/**
		 * Sets a column's width without that width becoming the column's internal width.
		 *
		 * \param _iCol The index of the column.
		 * \param _iWidth The new width in pixels, or LVSCW_AUTOSIZE.
		 **/
		VOID								SetColumnWidthInternal( INT _iCol, INT _iWidth );

		/**
		 * Measures the width that LVSCW_AUTOSIZE_USEHEADER gives any column but the last one: wide enough for its header text and its items.
		 *
		 * \param _iCol The index of the column to measure.  As a side effect, the column is sized to fit its items.
		 * \return Returns the width, in pixels.
		 **/
		INT									UseHeaderWidth( INT _iCol );

		/**
		 * Called by SnapSubclassProc() before the list-view control processes a message.
		 *
		 * \param _uMsg The message.
		 * \param _wParam The message's WPARAM.
		 * \param _lParam The message's LPARAM, which can be changed before the control receives it.
		 **/
		VOID								SnapBeforeMessage( UINT _uMsg, WPARAM _wParam, LPARAM &_lParam );

		/**
		 * Called by SnapSubclassProc() after the list-view control has processed a message.
		 *
		 * \param _uMsg The message.
		 * \param _wParam The message's WPARAM.
		 * \param _lParam The message's LPARAM.
		 * \param _lrResult The control's result for the message.
		 * \param _iDepth How many of the control's messages were already being processed when this one arrived.
		 **/
		VOID								SnapAfterMessage( UINT _uMsg, WPARAM _wParam, LPARAM _lParam, LRESULT _lrResult, INT _iDepth );

		/**
		 * Tracks the header notifications that affect the right-most column.
		 *
		 * \param _pnhHdr The notification.
		 * \param _lrResult The control's result for the notification.
		 **/
		VOID								SnapHeaderNotify( const NMHDR * _pnhHdr, LRESULT _lrResult );

		/**
		 * Gets the private message that RequestSnap() posts to the list-view control.
		 *
		 * \return Returns the registered message.
		 **/
		static UINT							SnapMessage();

		/**
		 * The list-view control's subclass procedure.
		 *
		 * \param _hWnd The list-view control.
		 * \param _uMsg The message.
		 * \param _wParam The message's WPARAM.
		 * \param _lParam The message's LPARAM.
		 * \param _uiptrId The subclass ID.
		 * \param _dwpRefData The CListView that installed the subclass.
		 * \return Returns the message's result.
		 **/
		static LRESULT CALLBACK				SnapSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uiptrId, DWORD_PTR _dwpRefData );


	private :
		/**
		 * The parent class.
		 **/
		typedef CWidget						Parent;
	};

}	// namespace lsw

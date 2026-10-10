#include "LSWListView.h"
#include <codecvt>
#include <CommCtrl.h>
#include <locale>

#pragma comment( lib, "Comctl32.lib" )

namespace lsw {

	/** The ID under which CListView subclasses its list-view control. */
	static const UINT_PTR					LSW_LV_SNAP_SUBCLASS_ID = 0x4C565253;

	CListView::CListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_sColumns( 0 ),
		m_bSortWithCase( FALSE ),
		m_iSnapCol( -1 ),
		m_iSnapColWidth( 0 ),
		m_iSnapTrackCol( -1 ),
		m_iSnapInternal( 0 ),
		m_iSnapMsgDepth( 0 ),
		m_bRightColumnSnap( TRUE ),
		m_bSnapPending( false ),
		m_bSnapPosted( false ),
		m_bSnapSubclassed( false ) {
	}
	/**
	 * Removes the subclass that snaps the right-most column.
	 **/
	CListView::~CListView() {
		// CWidget's destructor destroys the window, and this object must not see those messages.
		RemoveSnapSubclass( Wnd() );
	}

	// == Functions.
	/**
	 * Enables or disables snapping the right-most column to the control's right edge (enabled by default).
	 *
	 * While enabled, the right-most column's width is the larger of its internal width and the width the other columns leave free.  Its
	 *	internal width is the width the application or the user last gave it (LVSCW_AUTOSIZE_USEHEADER sizes it to its header and items,
	 *	as it does any other column).
	 *
	 * \param _bEnable TRUE to snap the right-most column to the right edge, FALSE to return it to its internal width and leave it alone.
	 **/
	VOID CListView::SetRightColumnSnap( BOOL _bEnable ) {
		BOOL bEnable = (_bEnable != FALSE) ? TRUE : FALSE;
		if ( bEnable == m_bRightColumnSnap ) { return; }
		if ( !bEnable ) {
			RestoreSnapColumn();
			m_bRightColumnSnap = FALSE;
			m_iSnapCol = -1;
			return;
		}
		m_bRightColumnSnap = TRUE;
		m_iSnapCol = -1;	// The next snap adopts whichever column is right-most at its current width.
		RequestSnap();
	}

	/**
	 * If the list-view control was created without the LVS_OWNERDATA style, this macro causes the control to allocate its internal data structures for
	 *	the specified number of items. This prevents the control from having to allocate the data structures every time an item is added.
	 * 
	 * \param _cItems The number of items for which the list-view control should allocate memory.
	 **/
	VOID CListView::SetItemCount( INT _cItems ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), LVM_SETITEMCOUNT, static_cast<WPARAM>(_cItems), 0 );
		}
	}

	/**
	 * Sets the virtual number of items in a virtual list view.
	 * 
	 * \param _cItems The number of items that the list-view control will contain
	 * \param _dwFlags Values that specify the behavior of the list-view control after resetting the item count. This value can be a combination of the following: LVSICF_NOINVALIDATEALL, LVSICF_NOSCROLL.
	 **/
	VOID CListView::SetItemCountEx( INT _cItems, DWORD _dwFlags ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), LVM_SETITEMCOUNT, static_cast<WPARAM>(_cItems), static_cast<LPARAM>(_dwFlags) );
		}
	}

	/**
	 * Gets the number of items in a list-view control.
	 * 
	 * \return Returns the number of items in a list-view control
	 **/
	int CListView::GetItemCount() const {
		return static_cast<int>(::SendMessageW( Wnd(), LVM_GETITEMCOUNT, 0L, 0L ));
	}

	/**
	 * Determines the number of selected items in a list-view control.
	 * 
	 * \return Returns the number of selected items in a list-view control.
	 **/
	UINT CListView::GetSelectedCount() const {
		return static_cast<UINT>(::SendMessageW( Wnd(), LVM_GETSELECTEDCOUNT, 0, 0L ));
	}

	/**
	 * Removes an item from a list-view control.
	 * 
	 * \param _iItem An index of the list-view item to delete.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CListView::DeleteItem( int _iItem ) {
		return static_cast<BOOL>(::SendMessageW( Wnd(), LVM_DELETEITEM, static_cast<WPARAM>(_iItem), 0L ));
	}

	/**
	 * Gets the number of columns.
	 * 
	 * \return Returns the number of columns.
	 **/
	INT CListView::GetTotalColumns() const {
		if ( Wnd() ) {
			HWND hHeader = reinterpret_cast<HWND>(::SendMessageW( Wnd(), LVM_GETHEADER, 0L, 0L ));
			if ( hHeader ) {
				return static_cast<INT>(::SendMessageW( hHeader, HDM_GETITEMCOUNT, 0L, 0L ));
			}
		}
		return 0;
	}

	/**
	 * Inserts a column at the given index.
	 * 
	 * \param _iIndex The index of the item after which the new item is to be inserted. The new item is inserted at the end of the header control if _iIndex is greater than or equal to the number of items in the control. If _iIndex is zero, the new item is inserted at the beginning of the header control.
	 * \param _pwcText A pointer to an item string.
	 * \param _iFormat Flags that specify the item's format: [HDF_CENTER, HDF_LEFT, HDF_RIGHT], [HDF_BITMAP, HDF_BITMAP_ON_RIGHT, HDF_OWNERDRAW, HDF_STRING], [HDF_IMAGE, HDF_JUSTIFYMASK, HDF_RTLREADING, HDF_SORTDOWN, HDF_SORTUP, HDF_CHECKBOX, HDF_CHECKED, HDF_FIXEDWIDTH, HDF_SPLITBUTTON].
	 * \return Returns the index of the inserted item or -1.
	 **/
	INT CListView::InsertColumn( INT _iIndex, const WCHAR * _pwcText, INT _iFormat ) {
		InstallSnapSubclass();	// In case InitControl() was not called.
		LV_COLUMNW lvColumn;
		lvColumn.mask = LVCF_TEXT | LVCF_FMT;
		lvColumn.pszText = const_cast<LPWSTR>(_pwcText);
		lvColumn.fmt = _iFormat;
		INT iInserted = static_cast<INT>(::SendMessageW( Wnd(), LVM_INSERTCOLUMNW, static_cast<WPARAM>(_iIndex), reinterpret_cast<LPARAM>(&lvColumn)));
		if ( iInserted != -1 ) {
			++m_sColumns;
		}
		return iInserted;
	}

	/**
	 * Inserts a column at the given index.
	 * 
	 * \param _iIndex The index of the item after which the new item is to be inserted. The new item is inserted at the end of the header control if _iIndex is greater than or equal to the number of items in the control. If _iIndex is zero, the new item is inserted at the beginning of the header control.
	 * \param _pcText A pointer to an item string.
	 * \param _iFormat Flags that specify the item's format: [HDF_CENTER, HDF_LEFT, HDF_RIGHT], [HDF_BITMAP, HDF_BITMAP_ON_RIGHT, HDF_OWNERDRAW, HDF_STRING], [HDF_IMAGE, HDF_JUSTIFYMASK, HDF_RTLREADING, HDF_SORTDOWN, HDF_SORTUP, HDF_CHECKBOX, HDF_CHECKED, HDF_FIXEDWIDTH, HDF_SPLITBUTTON].
	 * \return Returns the index of the inserted item or -1.
	 **/
	INT CListView::InsertColumn( INT _iIndex, const CHAR * _pcText, INT _iFormat ) {
		InstallSnapSubclass();	// In case InitControl() was not called.
		LV_COLUMNA lvColumn;
		lvColumn.mask = LVCF_TEXT | LVCF_FMT;
		lvColumn.pszText = const_cast<LPSTR>(_pcText);
		lvColumn.fmt = _iFormat;
		if ( _iIndex < 0 ) {
			_iIndex = static_cast<INT>(m_sColumns);
		}
		INT iInserted = static_cast<INT>(::SendMessageA( Wnd(), LVM_INSERTCOLUMNA, static_cast<WPARAM>(_iIndex), reinterpret_cast<LPARAM>(&lvColumn)));
		if ( iInserted != -1 ) {
			++m_sColumns;
		}
		return iInserted;
	}

	/**
	 * Inserts a column at the given index.
	 *
	 * \param _pwcText A pointer to an item string.
	 * \param _iWidth The width of the item.
	 * \param _iIdx The index of the item after which the new item is to be inserted. The new item is inserted at the end of the header control if _iIndex is greater than or equal to the number of items in the control. If _iIndex is zero, the new item is inserted at the beginning of the header control.
	 * \return Returns TRUE if the item was inserted.
	 **/
	BOOL CListView::InsertColumn( const WCHAR * _pwcText, INT _iWidth, INT _iIdx ) {
		InstallSnapSubclass();	// In case InitControl() was not called.
		LV_COLUMNW lvColumn;
		lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		lvColumn.pszText = const_cast<LPWSTR>(_pwcText);
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = _iWidth;
		if ( _iIdx < 0 ) {
			_iIdx = static_cast<INT>(m_sColumns);
		}
		INT iInserted = static_cast<INT>(::SendMessageW( Wnd(), LVM_INSERTCOLUMNW, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(&lvColumn)));
		if ( iInserted != -1 ) {
			++m_sColumns;
		}
		return iInserted;
	}

	/**
	 * Adds a column with the given text.
	 * 
	 * \param _pcText A pointer to an item string.
	 * \param _iFormat Flags that specify the item's format: [HDF_CENTER, HDF_LEFT, HDF_RIGHT], [HDF_BITMAP, HDF_BITMAP_ON_RIGHT, HDF_OWNERDRAW, HDF_STRING], [HDF_IMAGE, HDF_JUSTIFYMASK, HDF_RTLREADING, HDF_SORTDOWN, HDF_SORTUP, HDF_CHECKBOX, HDF_CHECKED, HDF_FIXEDWIDTH, HDF_SPLITBUTTON].
	 * \return Returns the index of the inserted item or -1.
	 **/
	INT CListView::AddColumn( const CHAR * _pcText, INT _iFormat ) {
		return AddColumn( ee::CExpEval::ToUtf16( _pcText ).c_str(), _iFormat );
	}

	/**
	 * Adds a column with the given text.
	 * 
	 * \param _pwcText A pointer to an item string.
	 * \param _iFormat Flags that specify the item's format: [HDF_CENTER, HDF_LEFT, HDF_RIGHT], [HDF_BITMAP, HDF_BITMAP_ON_RIGHT, HDF_OWNERDRAW, HDF_STRING], [HDF_IMAGE, HDF_JUSTIFYMASK, HDF_RTLREADING, HDF_SORTDOWN, HDF_SORTUP, HDF_CHECKBOX, HDF_CHECKED, HDF_FIXEDWIDTH, HDF_SPLITBUTTON].
	 * \return Returns the index of the inserted item or -1.
	 **/
	INT CListView::AddColumn( const WCHAR * _pwcText, INT _iFormat ) {
		return InsertColumn( static_cast<INT>(m_sColumns), _pwcText, _iFormat );
	}

	/**
	 * Sets the width of a column.
	 * 
	 * \param _iCol The zero-based index of a valid column. For list-view mode, this parameter must be set to zero.
	 * \param _iWidth The new width of the column, in pixels. For report-view mode, the following special values are supported: LVSCW_AUTOSIZE, LVSCW_AUTOSIZE_USEHEADER.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CListView::SetColumnWidth( INT _iCol, INT _iWidth ) {
		return ListView_SetColumnWidth( Wnd(), _iCol, _iWidth );
	}

	/**
	 * Gets the width of a column.
	 * 
	 * \param _iCol The index of the column. This parameter is ignored in list view.
	 * \return Returns the column width if successful, or zero otherwise. If this message is sent to a list-view control with the LVS_REPORT style and the specified column does not exist, the return value is undefined.
	 **/
	INT CListView::GetColumnWidth( INT _iCol ) const {
		return static_cast<INT>(::SendMessageW( Wnd(), LVM_GETCOLUMNWIDTH, static_cast<WPARAM>(_iCol), 0 ));
	}

	/**
	 * Sets the text of a column.
	 * 
	 * \param _pwcText The address of a null-terminated string that contains the column header text.
	 * \param _iIdx Index of the column.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CListView::SetColumnText( const WCHAR * _pwcText, INT _iIdx ) {
		if ( _iIdx >= 0 && _iIdx < GetColumnCount() ) {
			LV_COLUMNW lvColumn;
			lvColumn.mask = LVCF_TEXT;
			lvColumn.pszText = const_cast<LPWSTR>(_pwcText);
			INT iInserted = static_cast<INT>(::SendMessageW( Wnd(), LVM_SETCOLUMN, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(&lvColumn)));
			return iInserted;
		}
		return FALSE;
	}

	/**
	 * Deletes a column.
	 * 
	 * \param _iCol An index of the column to delete.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CListView::DeleteColumn( INT _iCol ) {
		if ( static_cast<BOOL>(::SendMessageW( Wnd(), LVM_DELETECOLUMN, static_cast<WPARAM>(_iCol), 0 )) ) {
			--m_sColumns;
			return TRUE;
		}
		return FALSE;
	}

	/**
	 * Deletes all columns.
	 **/
	VOID CListView::DeleteAllColumns() {
		while ( GetTotalColumns() ) {
			DeleteColumn( 0 );
		}
	}

	/**
	 * Inserts an item.
	 * 
	 * \param _iItem A pointer to an LVITEMW structure that specifies the attributes of the list-view item. Use the iItem member to specify the zero-based index at which the new item should be inserted. If this value is greater than the number of items currently contained by the listview control, the new item will be appended to the end of the list and assigned the correct index.
	 * \return Returns the index of the inserted item.
	 **/
	INT CListView::InsertItem( const LVITEMW &_iItem ) {
		return static_cast<INT>(::SendMessageW( Wnd(), LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&_iItem)));
	}

	/**
	 * Inserts an item.
	 * 
	 * \param _iItem A pointer to an LVITEMA structure that specifies the attributes of the list-view item. Use the iItem member to specify the zero-based index at which the new item should be inserted. If this value is greater than the number of items currently contained by the listview control, the new item will be appended to the end of the list and assigned the correct index.
	 * \return Returns the index of the inserted item.
	 **/
	INT CListView::InsertItem( const LVITEMA &_iItem ) {
		return static_cast<INT>(::SendMessageA( Wnd(), LVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&_iItem)));
	}

	/**
	 * Inserts an item that consistes of text and a parameter.
	 * 
	 * \param _pwcText The text of the item.
	 * \param _lParam The LPARAM to associate with the item.
	 * \return Returns the index of the inserted item.
	 **/
	INT CListView::InsertItem( const WCHAR * _pwcText, LPARAM _lParam ) {
		LVITEMW iItem = { 0 };
		iItem.mask = LVIF_TEXT | LVIF_PARAM;
		iItem.iItem = 0x7FFFFFFF;
		iItem.lParam = _lParam;
		iItem.pszText = const_cast<LPWSTR>(_pwcText);
		return InsertItem( iItem );
	}

	/**
	 * Inserts an item that consistes of text and a parameter.
	 * 
	 * \param _pcText The text of the item.
	 * \param _lParam The LPARAM to associate with the item.
	 * \return Returns the index of the inserted item.
	 **/
	INT CListView::InsertItem( const CHAR * _pcText, LPARAM _lParam ) {
		LVITEMA iItem = { 0 };
		iItem.mask = LVIF_TEXT | LVIF_PARAM;
		iItem.iItem = 0x7FFFFFFF;
		iItem.lParam = _lParam;
		iItem.pszText = const_cast<LPSTR>(_pcText);
		return InsertItem( iItem );
	}

	/**
	 * Sets the text for an item.
	 * 
	 * \param _iItem The zero-based index of the list-view item
	 * \param _iSubItem The one-based index of the subitem. To set the item label, set _iSubItem to zero.
	 * \param _pwcText A pointer to a null-terminated string that contains the new text. This parameter can be LPSTR_TEXTCALLBACK to indicate a callback item for which the parent window stores the text. In this case, the list-view control sends the parent an LVN_GETDISPINFO notification code when it needs the text. This parameter can be NULL.
	 **/
	VOID CListView::SetItemText( INT _iItem, INT _iSubItem, const WCHAR * _pwcText ) {
		LVITEMW iItem = { 0 };
		iItem.iItem = _iItem;
		iItem.iSubItem = _iSubItem;
		iItem.pszText = const_cast<LPWSTR>(_pwcText);
		::SendMessageW( Wnd(), LVM_SETITEMTEXTW, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&iItem) );
	}

	/**
	 * Sets the text for an item.
	 * 
	 * \param _iItem The zero-based index of the list-view item
	 * \param _iSubItem The one-based index of the subitem. To set the item label, set _iSubItem to zero.
	 * \param _pcText A pointer to a null-terminated string that contains the new text. This parameter can be LPSTR_TEXTCALLBACK to indicate a callback item for which the parent window stores the text. In this case, the list-view control sends the parent an LVN_GETDISPINFO notification code when it needs the text. This parameter can be NULL.
	 **/
	VOID CListView::SetItemText( INT _iItem, INT _iSubItem, const CHAR * _pcText ) {
		LVITEMA iItem = { 0 };
		iItem.iItem = _iItem;
		iItem.iSubItem = _iSubItem;
		iItem.pszText = const_cast<LPSTR>(_pcText);
		::SendMessageA( Wnd(), LVM_SETITEMTEXTA, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&iItem) );
	}

	/**
	 * Gets the length of an item's text in WCHAR units.
	 * 
	 * \param _iItem The zero-based index of the list-view item
	 * \param _iSubItem The one-based index of the subitem. To set the item label, set _iSubItem to zero.
	 * \return Returns the length of the item's string, including the terminating NULL.
	 **/
	INT CListView::GetItemTextLenW( INT _iItem, INT _iSubItem ) const {
		INT iSize = 128;
		INT iRet = 0;
		LVITEMW liItem = { 0 };
		liItem.iSubItem = _iSubItem;
		liItem.pszText = nullptr;
		do {
			iSize *= 2;
			liItem.pszText = new( std::nothrow ) WCHAR[iSize];
			liItem.cchTextMax = iSize;
			iRet = static_cast<INT>(::SendMessageW( Wnd(), LVM_GETITEMTEXTW, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&liItem) ));
			delete [] liItem.pszText;
		} while ( iRet >= iSize -1 );
		return iRet + 1;
	}

	/**
	 * Gets the length of an item's text in CHAR units.
	 * 
	 * \param _iItem The zero-based index of the list-view item
	 * \param _iSubItem The one-based index of the subitem. To set the item label, set _iSubItem to zero.
	 * \return Returns the length of the item's string, including the terminating NULL.
	 **/
	INT CListView::GetItemTextLenA( INT _iItem, INT _iSubItem ) const {
		INT iSize = 128;
		INT iRet = 0;
		LVITEMA liItem = { 0 };
		liItem.iSubItem = _iSubItem;
		liItem.pszText = nullptr;
		do {
			iSize *= 2;
			liItem.pszText = new( std::nothrow ) CHAR[iSize];
			liItem.cchTextMax = iSize;
			iRet = static_cast<INT>(::SendMessageA( Wnd(), LVM_GETITEMTEXTA, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&liItem) ));
			delete [] liItem.pszText;
		} while ( iRet >= iSize - 1 );
		return iRet + 1;
	}

	/**
	 * Gets the text of an item.
	 * 
	 * \param _iItem The index of the list-view item
	 * \param _iSubItem The index of the subitem. To retrieve the item text, set _iSubItem to zero.
	 * \param _sRet Holds the returned string.
	 **/
	VOID CListView::GetItemText( INT _iItem, INT _iSubItem, std::wstring &_sRet ) const {
		INT iLen = GetItemTextLenW( _iItem,  _iSubItem );
		LVITEMW liItem = { 0 };
		liItem.iSubItem = _iSubItem;
		liItem.pszText = new( std::nothrow ) WCHAR[iLen];
		liItem.cchTextMax = iLen;
		::SendMessageA( Wnd(), LVM_GETITEMTEXTW, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&liItem) );
		_sRet = liItem.pszText;
		delete [] liItem.pszText;
	}

	/**
	 * Gets the text of an item.
	 * 
	 * \param _iItem The index of the list-view item
	 * \param _iSubItem The index of the subitem. To retrieve the item text, set _iSubItem to zero.
	 * \param _sRet Holds the returned string.
	 **/
	VOID CListView::GetItemText( INT _iItem, INT _iSubItem, std::string &_sRet ) const {
		INT iLen = GetItemTextLenA( _iItem,  _iSubItem );
		LVITEMA liItem = { 0 };
		liItem.iSubItem = _iSubItem;
		liItem.pszText = new( std::nothrow ) CHAR[iLen];
		liItem.cchTextMax = iLen;
		::SendMessageA( Wnd(), LVM_GETITEMTEXTA, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&liItem) );
		_sRet = liItem.pszText;
		delete [] liItem.pszText;
	}

	/**
	 * Gets the bounding rectangle for all or part of an item in the current view.
	 * 
	 * \param _iItem The index of the list-view item.
	 * \param _iCode The portion of the list-view item from which to retrieve the bounding rectangle. This parameter must be one of the following values: LVIR_BOUNDS, LVIR_ICON, LVIR_LABEL, LVIR_SELECTBOUNDS.
	 * \return Returns the item's RECT.
	 **/
	LSW_RECT CListView::GetItemRect( int _iItem, int _iCode ) {
		LSW_RECT rRet;
		rRet.Zero();
		if ( !Wnd() ) { return rRet; }
		rRet.left = _iCode;
		::SendMessageW( Wnd(), LVM_GETITEMRECT, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&rRet) );
		return rRet;
	}

	/**
	 * Gets the index of the (first) selected item or -1.
	 * 
	 * \return Returns the index of the (first) selected item or -1.
	 **/
	INT CListView::GetFirstSelectedItem() const {
		if ( !Wnd() ) { return -1; }
		return static_cast<INT>(::SendMessageW( Wnd(), LVM_GETNEXTITEM, static_cast<WPARAM>(-1), MAKELPARAM( LVNI_SELECTED, 0 ) ));
	}

	/**
	 * Gets the data of the selected item or returns -1.
	 *
	 * \return Returns the data of the selected item or returns -1.
	 **/
	LPARAM CListView::GetSelData() const {
		INT iSel = GetFirstSelectedItem();
		if ( iSel == -1 ) { return -1; }

		LVITEMW lvItem;
		lvItem.mask = LVIF_PARAM;
		if ( GetItem( iSel, 0, lvItem ) ) {
			return lvItem.lParam;
		}

		return -1;
	}

	/**
	 * Gets the data of an item by index.
	 * 
	 * \param _iItem The index of the item whose data is to be obtained.
	 * \return Returns the item's data or -1.
	 **/
	LPARAM CListView::GetItemData( INT _iItem ) const {
		LVITEMW lvItem;
		lvItem.mask = LVIF_PARAM;
		if ( GetItem( _iItem, 0, lvItem ) ) {
			return lvItem.lParam;
		}

		return -1;
	}

	/**
	 * Sets selection on an item by index.
	 *
	 * \param _iItem The item to update.
	 * \param _bSelected Whether the item is selected or not.
	 */
	void CListView::SetItemSelection( INT _iItem, BOOL _bSelected ) {
		LVITEMW iItem = {};
		iItem.stateMask = LVIS_SELECTED;
		iItem.state = _bSelected ? LVIS_SELECTED : 0;
		::SendMessageW( Wnd(), LVM_SETITEMSTATE, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(reinterpret_cast<const LV_ITEM *>(&iItem) ));
	}

	/**
	 * Sets highlight on an item by index.
	 *
	 * \param _iItem The item to update.
	 * \param _bHighlighted Whether the item is highlighted or not.
	 */
	void CListView::SetItemHighlight( INT _iItem, BOOL _bHighlighted ) {
		LVITEMW iItem = {};
		iItem.stateMask = LVIS_FOCUSED;
		iItem.state = _bHighlighted ? LVIS_FOCUSED : 0;
		::SendMessageW( Wnd(), LVM_SETITEMSTATE, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(reinterpret_cast<const LV_ITEM *>(&iItem) ));
	}

	/**
	 * Gets an item.  _iItm is input and output.
	 * 
	 * \param _iItem The index of the list-view item.
	 * \param _iSubItem The index of the subitem. To retrieve the item text, set _iSubItem to zero.
	 * \param _iItm A pointer to an LVITEMW structure that specifies the information to retrieve and receives information about the list-view item.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CListView::GetItem( INT _iItem, INT _iSubItem, LVITEMW &_iItm ) const {
		if ( !Wnd() ) { return FALSE; }
		_iItm.iItem = _iItem;
		_iItm.iSubItem = _iSubItem;
		return static_cast<BOOL>(::SendMessageW( Wnd(), LVM_GETITEMW, 0, reinterpret_cast<LPARAM>(&_iItm) ));
	}

	/**
	 * Gets an item.  _iItm is input and output.
	 * 
	 * \param _iItem The index of the list-view item.
	 * \param _iSubItem The index of the subitem. To retrieve the item text, set _iSubItem to zero.
	 * \param _iItm A pointer to an LVITEMA structure that specifies the information to retrieve and receives information about the list-view item.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CListView::GetItem( INT _iItem, INT _iSubItem, LVITEMA &_iItm ) const {
		if ( !Wnd() ) { return FALSE; }
		_iItm.iItem = _iItem;
		_iItm.iSubItem = _iSubItem;
		return static_cast<BOOL>(::SendMessageA( Wnd(), LVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&_iItm) ));
	}

	/**
	 * Creates an array of indices that correspond to the selected items in the list-view.
	 * 
	 * \param _vArray Holds the returned items, appended to end of the input vector.
	 * \return Returns true if no allocation issues were encountered during the operation and the control is valid.
	 **/
	bool CListView::GetSelectedItems( std::vector<int> &_vArray ) const {
		if ( !Wnd() ) { return false; }
		int iIdx = -1;
		while ( (iIdx = static_cast<int>(::SendMessageW( Wnd(), LVM_GETNEXTITEM, static_cast<WPARAM>(iIdx), MAKELPARAM( LVNI_SELECTED, 0 ) ))) != -1 ) {
			try {
				_vArray.push_back( iIdx );
			}
			catch ( const std::bad_alloc /*& _eE*/ ) { return false; }
		}
		return true;
	}

	/**
	 * Gets the item's state.
	 * 
	 * \param _iItem The index of the list-view item.
	 * \param _uiMask The state information to retrieve. This parameter can be a combination of the following values: LVIS_CUT, LVIS_DROPHILITED, LVIS_FOCUSED, LVIS_SELECTED, LVIS_OVERLAYMASK, LVIS_STATEIMAGEMASK.
	 * \return Returns the current state for the specified item. The only valid bits in the return value are those that correspond to the bits set in the _uiMask parameter.
	 **/
	UINT CListView::GetItemState( INT _iItem, UINT _uiMask ) const {
		if ( !Wnd() ) { return 0; }
		return static_cast<UINT>(::SendMessageW( Wnd(), LVM_GETITEMSTATE, static_cast<WPARAM>(_iItem), static_cast<LPARAM>(_uiMask) ));
	}

	/**
	 * Indicates whether an item in the list-view control is visible.
	 * 
	 * \param _iItem The index of the list-view item.
	 * \return Returns TRUE if the control and item are valid and the item is visible.
	 **/
	BOOL CListView::IsItemVisible( INT _iItem ) const {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageA( Wnd(), LVM_ISITEMVISIBLE, static_cast<WPARAM>(_iItem), 0L ));
	}

	/**
	 * Sorts items.
	 * 
	 * \param _iSubItem The index of the sub-item.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CListView::SortItems( INT _iSubItem ) {
		LSW_LISTSORT lsSort = {
			this,
			_iSubItem,
		};
		return static_cast<BOOL>(::SendMessageA( Wnd(), LVM_SORTITEMSEX, reinterpret_cast<WPARAM>(&lsSort), reinterpret_cast<LPARAM>(CompareFunc) ));
	}

	/**
	 * Sort comparison function.  Override to change how items compare against each other.
	 * 
	 * \param _iLeft Left operand.
	 * \param _iRight Right operand.
	 * \param _iSub Sub-item to use for text compare.
	 * \return Returns a lexicographical comparison value.
	 **/
	int CListView::SortCompare( INT _iLeft, INT _iRight, INT _iSub ) {
		std::wstring sLeft, sRight;
		GetItemText( _iLeft, _iSub, sLeft );
		GetItemText( _iRight, _iSub, sRight );
		return m_bSortWithCase ? std::wcscmp( sLeft.c_str(), sRight.c_str() ) : _wcsicmp( sLeft.c_str(), sRight.c_str() );
	}

	/**
	 * Deletes all items.
	 */
	VOID CListView::DeleteAll() {
		::SendMessageW( Wnd(), LVM_DELETEALLITEMS, 0, 0 );
	}

	/**
	 * Snaps the column widths to the control width bi adjusting the given column's width.
	 * 
	 * \param _iCol The index of the column whose width is to be adjusted to make the columns fit perfectly into the control width.
	 * \return Returns the new width of the given column if it exists or -1 if it does not.
	 **/
	INT CListView::FitColumndsToControlWidth( INT _iCol ) {
		auto iColumns = GetTotalColumns();
		if ( _iCol >= iColumns ) { return -1; }
		BOOL bSnap = SnapActive();
		INT iWidthExcludingCol = 0;
		for ( auto I = iColumns; I--; ) {
			if ( I != _iCol ) {
				// A snapped right-most column counts at its internal width; it gives up whatever it was stretched into.
				iWidthExcludingCol += (bSnap && I == m_iSnapCol) ? m_iSnapColWidth : GetColumnWidth( I );
			}
		}

		LSW_RECT rClient;
		::GetClientRect( Wnd(), &rClient );

		INT iNewWidth = std::max<INT>( 0, rClient.Width() - iWidthExcludingCol );
		if ( bSnap ) {
			if ( _iCol == m_iSnapCol ) {
				// The snapped column already fits itself to the control.  Setting its width here would make the fitted width its internal
				//	width, and then it could no longer shrink with the control.
				if ( m_iSnapMsgDepth == 0 ) { SnapNow(); }
				else { RequestSnap(); }
				return std::max<INT>( m_iSnapColWidth, iNewWidth );
			}
			RestoreSnapColumn();
		}
		SetColumnWidth( _iCol, iNewWidth );
		return iNewWidth;
	}

	/**
	 * The WM_NOTIFY -> NM_CUSTOMDRAW -> CDDS_ITEMPREPAINT handler.
	 *
	 * \param _lpcdParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	DWORD CListView::Notify_CustomDraw_ItemPrePaint( LPNMLVCUSTOMDRAW _lpcdParm ) {
		if ( _lpcdParm->nmcd.dwItemSpec % 2 == 0 ) {
			_lpcdParm->clrText = ::GetSysColor( COLOR_WINDOWTEXT );//RGB( 0, 0, 0 );
			_lpcdParm->clrTextBk = RGB( 0xE5, 0xF5, 0xFF );
			return CDRF_NEWFONT;
		}
		return CDRF_DODEFAULT;
	}

	/**
	 * Setting the HWND after the control has been created.
	 * 
	 * \param _hWnd The handle to the control.
	 **/
	void CListView::InitControl( HWND _hWnd ) {
		CWidget::InitControl( _hWnd );
		ListView_SetExtendedListViewStyleEx( Wnd(), m_dwExtendedStyles, 0xFFFFFFFF );
		InstallSnapSubclass();
	}

	/**
	 * Sort routine.  The comparison callback that SortItems() passes to the list-view control.
	 * 
	 * \param _lParam1 The first item to compare.
	 * \param _lParam2 The second item to compare.
	 * \param _lParamSort A pointer to the LSW_LISTSORT structure for the sort in progress.
	 * \return Returns a negative value if the first item should precede the second, a positive value if the first item should follow the second, or zero if the two items are equivalent.
	 **/
	int CALLBACK CListView::CompareFunc( LPARAM _lParam1, LPARAM _lParam2, LPARAM _lParamSort ) {
		LSW_LISTSORT * plsSort = reinterpret_cast<LSW_LISTSORT *>(_lParamSort);
		return plsSort->plvListView->SortCompare( static_cast<INT>(_lParam1), static_cast<INT>(_lParam2), plsSort->iSubItem );
	}

	/**
	 * Subclasses the list-view control so that the right-most column can snap to its right edge.  Does nothing if it is already subclassed.
	 **/
	VOID CListView::InstallSnapSubclass() {
		if ( m_bSnapSubclassed || !Wnd() ) { return; }
		if ( ::SetWindowSubclass( Wnd(), SnapSubclassProc, LSW_LV_SNAP_SUBCLASS_ID, reinterpret_cast<DWORD_PTR>(this) ) ) {
			m_bSnapSubclassed = true;
			RequestSnap();
		}
	}

	/**
	 * Removes the subclass installed by InstallSnapSubclass().
	 *
	 * \param _hWnd The list-view control's window handle.
	 **/
	VOID CListView::RemoveSnapSubclass( HWND _hWnd ) {
		if ( !m_bSnapSubclassed ) { return; }
		if ( _hWnd ) {
			::RemoveWindowSubclass( _hWnd, SnapSubclassProc, LSW_LV_SNAP_SUBCLASS_ID );
		}
		m_bSnapSubclassed = false;
		m_bSnapPending = false;
		m_bSnapPosted = false;		// If the posted message still arrives, it goes straight to the control, which ignores it.
	}

	/**
	 * Determines whether the right-most column is being snapped: snapping is enabled, the control is subclassed, and it is in report view.
	 *
	 * \return Returns TRUE if the right-most column is being snapped to the control's right edge.
	 **/
	BOOL CListView::SnapActive() const {
		return (m_bRightColumnSnap && m_bSnapSubclassed && Wnd() && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT) ? TRUE : FALSE;
	}

	/**
	 * Requests a snap without performing it.  The snap happens when the posted SnapMessage() arrives, by which time the control has finished
	 *	whatever it was doing.  Requests made in the meantime are merged into one.
	 **/
	VOID CListView::RequestSnap() {
		if ( !m_bRightColumnSnap || !m_bSnapSubclassed || !Wnd() ) { return; }
		m_bSnapPending = true;
		if ( !m_bSnapPosted ) {
			m_bSnapPosted = (::PostMessageW( Wnd(), SnapMessage(), 0, 0 ) != FALSE);
		}
	}

	/**
	 * Snaps the right-most column immediately, repeating (a few times at most) while doing so adds or removes a scroll bar and so requests
	 *	another snap.  Only call this while the control is not processing any of its own messages.
	 **/
	VOID CListView::SnapNow() {
		for ( INT I = 0; I < 4; ++I ) {
			m_bSnapPending = false;
			SnapOnce();
			if ( !m_bSnapPending ) { break; }
		}
	}

	/**
	 * Snaps the right-most column once: its width becomes the larger of its internal width and the width the other columns leave free.
	 **/
	VOID CListView::SnapOnce() {
		if ( !SnapActive() ) { return; }
		UpdateSnapColumn();
		if ( m_iSnapCol < 0 ) { return; }
		// If a divider drag ended without an HDN_ENDTRACK, stop waiting for one.
		if ( m_iSnapTrackCol != -1 && ::GetCapture() != ListView_GetHeader( Wnd() ) ) { m_iSnapTrackCol = -1; }
		// While the user drags the right-most column's divider, the column goes where the mouse takes it.  HDN_ENDTRACK snaps it afterwards.
		if ( m_iSnapTrackCol == m_iSnapCol ) { return; }

		INT iOthers = 0;
		for ( INT I = GetTotalColumns(); I--; ) {
			if ( I != m_iSnapCol ) { iOthers += GetColumnWidth( I ); }
		}
		// The client area excludes the vertical scroll bar.  Nothing here depends on the horizontal scroll position or on where the header is.
		LSW_RECT rClient;
		::GetClientRect( Wnd(), &rClient );
		INT iWidth = std::max<INT>( m_iSnapColWidth, rClient.Width() - iOthers );
		if ( GetColumnWidth( m_iSnapCol ) != iWidth ) {
			SetColumnWidthInternal( m_iSnapCol, iWidth );
		}
	}

	/**
	 * Keeps track of which column is right-most.  When that changes, the previous right-most column goes back to its internal width and the
	 *	new one is adopted with its current width as its internal width.
	 **/
	VOID CListView::UpdateSnapColumn() {
		INT iTotal = GetTotalColumns();
		INT iRight = -1;
		if ( iTotal > 0 ) {
			// Right-most on the screen, which is not the last index once the user has dragged the columns into another order.
			iRight = Header_OrderToIndex( ListView_GetHeader( Wnd() ), iTotal - 1 );
			if ( iRight < 0 || iRight >= iTotal ) { iRight = iTotal - 1; }
		}
		if ( iRight == m_iSnapCol ) { return; }
		RestoreSnapColumn();
		m_iSnapCol = iRight;
		m_iSnapColWidth = (iRight >= 0) ? GetColumnWidth( iRight ) : 0;
	}

	/**
	 * Returns the right-most column to its internal width.
	 **/
	VOID CListView::RestoreSnapColumn() {
		if ( m_iSnapCol < 0 || m_iSnapCol >= GetTotalColumns() || !SnapActive() ) { return; }
		if ( GetColumnWidth( m_iSnapCol ) != m_iSnapColWidth ) {
			SetColumnWidthInternal( m_iSnapCol, m_iSnapColWidth );
		}
	}

	/**
	 * Sets a column's width without that width becoming the column's internal width.
	 *
	 * \param _iCol The index of the column.
	 * \param _iWidth The new width in pixels, or LVSCW_AUTOSIZE.
	 **/
	VOID CListView::SetColumnWidthInternal( INT _iCol, INT _iWidth ) {
		++m_iSnapInternal;
		::SendMessageW( Wnd(), LVM_SETCOLUMNWIDTH, static_cast<WPARAM>(_iCol), MAKELPARAM( _iWidth, 0 ) );
		--m_iSnapInternal;
	}

	/**
	 * Measures the width that LVSCW_AUTOSIZE_USEHEADER gives any column but the last one: wide enough for its header text and its items.
	 *
	 * \param _iCol The index of the column to measure.  As a side effect, the column is sized to fit its items.
	 * \return Returns the width, in pixels.
	 **/
	INT CListView::UseHeaderWidth( INT _iCol ) {
		// The items, measured by the control itself.
		SetColumnWidthInternal( _iCol, LVSCW_AUTOSIZE );
		INT iWidth = GetColumnWidth( _iCol );

		// The header text, plus the margins the header leaves on either side of it.
		WCHAR wcText[256] = {};
		LV_COLUMNW lvColumn = {};
		lvColumn.mask = LVCF_TEXT;
		lvColumn.pszText = wcText;
		lvColumn.cchTextMax = static_cast<int>(std::size( wcText ));
		if ( ::SendMessageW( Wnd(), LVM_GETCOLUMNW, static_cast<WPARAM>(_iCol), reinterpret_cast<LPARAM>(&lvColumn) ) && lvColumn.pszText ) {
			INT iText = static_cast<INT>(::SendMessageW( Wnd(), LVM_GETSTRINGWIDTHW, 0, reinterpret_cast<LPARAM>(lvColumn.pszText) ));
			iWidth = std::max<INT>( iWidth, iText + 6 * ::GetSystemMetrics( SM_CXEDGE ) );
		}
		return iWidth;
	}

	/**
	 * Called by SnapSubclassProc() before the list-view control processes a message.
	 *
	 * \param _uMsg The message.
	 * \param _wParam The message's WPARAM.
	 * \param _lParam The message's LPARAM, which can be changed before the control receives it.
	 **/
	VOID CListView::SnapBeforeMessage( UINT _uMsg, WPARAM _wParam, LPARAM &_lParam ) {
		switch ( _uMsg ) {
			case LVM_INSERTCOLUMNA : {}
			case LVM_INSERTCOLUMNW : {
				// A column added at the end becomes the new right-most column, so the current right-most column goes back to its internal
				//	width first.  Otherwise the control briefly holds the stretched column plus the new one.
				if ( SnapActive() && _wParam >= static_cast<WPARAM>(GetTotalColumns()) ) {
					RestoreSnapColumn();
				}
				break;
			}
			case LVM_SETCOLUMNWIDTH : {
				// On the last column, the control's LVSCW_AUTOSIZE_USEHEADER fills the remaining width, which would become the column's internal
				//	width and keep it from ever shrinking below that.  Snapping already does the filling, so size the column to its header and
				//	items instead, as LVSCW_AUTOSIZE_USEHEADER does for every other column.
				if ( !m_iSnapInternal && m_iSnapCol >= 0 && static_cast<INT>(_wParam) == m_iSnapCol &&
					static_cast<SHORT>(LOWORD( _lParam )) == LVSCW_AUTOSIZE_USEHEADER && SnapActive() ) {
					_lParam = MAKELPARAM( UseHeaderWidth( m_iSnapCol ), 0 );
				}
				break;
			}
		}
	}

	/**
	 * Called by SnapSubclassProc() after the list-view control has processed a message.
	 *
	 * \param _uMsg The message.
	 * \param _wParam The message's WPARAM.
	 * \param _lParam The message's LPARAM.
	 * \param _lrResult The control's result for the message.
	 * \param _iDepth How many of the control's messages were already being processed when this one arrived.
	 **/
	VOID CListView::SnapAfterMessage( UINT _uMsg, WPARAM _wParam, LPARAM _lParam, LRESULT _lrResult, INT _iDepth ) {
		switch ( _uMsg ) {
			case LVM_INSERTCOLUMNA : {}
			case LVM_INSERTCOLUMNW : {
				INT iNew = static_cast<INT>(_lrResult);
				if ( iNew >= 0 ) {
					if ( m_iSnapCol >= iNew ) { ++m_iSnapCol; }
					if ( SnapActive() ) { UpdateSnapColumn(); }
				}
				RequestSnap();
				break;
			}
			case LVM_DELETECOLUMN : {
				if ( _lrResult ) {
					INT iCol = static_cast<INT>(_wParam);
					if ( iCol == m_iSnapCol ) { m_iSnapCol = -1; }
					else if ( iCol < m_iSnapCol ) { --m_iSnapCol; }
					if ( SnapActive() ) { UpdateSnapColumn(); }
				}
				RequestSnap();
				break;
			}
			case LVM_SETCOLUMNORDERARRAY : {}
			case LVM_SETVIEW : {}
			case WM_STYLECHANGED : {
				RequestSnap();
				break;
			}
			case WM_NOTIFY : {
				SnapHeaderNotify( reinterpret_cast<const NMHDR *>(_lParam), _lrResult );
				break;
			}
			case WM_WINDOWPOSCHANGED : {
				const WINDOWPOS * pwpPos = reinterpret_cast<const WINDOWPOS *>(_lParam);
				if ( !pwpPos || ((pwpPos->flags & SWP_NOSIZE) && !(pwpPos->flags & SWP_FRAMECHANGED)) ) { break; }
				// A resize from outside (a layout, a splitter) is snapped right away, so the repaint that follows it is already right.  A resize
				//	nested inside one of the control's own messages is the control adding or removing a scroll bar partway through its own
				//	update.  Changing columns then is what leaves the control drawn detached from its left edge, so that one waits for the
				//	posted message.
				if ( _iDepth == 0 ) { SnapNow(); }
				else { RequestSnap(); }
				break;
			}
			case WM_SIZE : {
				// As WM_WINDOWPOSCHANGED.
				if ( _iDepth == 0 ) { SnapNow(); }
				else { RequestSnap(); }
				break;
			}
		}
	}

	/**
	 * Tracks the header notifications that affect the right-most column.
	 *
	 * \param _pnhHdr The notification.
	 * \param _lrResult The control's result for the notification.
	 **/
	VOID CListView::SnapHeaderNotify( const NMHDR * _pnhHdr, LRESULT _lrResult ) {
		if ( !_pnhHdr || _pnhHdr->code < HDN_LAST || _pnhHdr->code > HDN_FIRST ) { return; }
		if ( _pnhHdr->hwndFrom != ListView_GetHeader( Wnd() ) ) { return; }
		// NMHEADERA/NMHEADERW and HDITEMA/HDITEMW share a layout, and only members that are not text are read.
		const NMHEADERW * pNmHdr = reinterpret_cast<const NMHEADERW *>(_pnhHdr);
		switch ( _pnhHdr->code ) {
			case HDN_BEGINTRACKA : {}
			case HDN_BEGINTRACKW : {
				if ( !_lrResult ) { m_iSnapTrackCol = pNmHdr->iItem; }	// Unless the drag was vetoed.
				break;
			}
			case HDN_ENDTRACKA : {}
			case HDN_ENDTRACKW : {
				m_iSnapTrackCol = -1;
				RequestSnap();
				break;
			}
			case HDN_ITEMCHANGEDA : {}
			case HDN_ITEMCHANGEDW : {
				// Sent for every width change: dragging or double-clicking a divider, the application, and this class (m_iSnapInternal).
				UINT uiMask = pNmHdr->pitem ? pNmHdr->pitem->mask : (HDI_WIDTH | HDI_ORDER);
				if ( m_iSnapInternal || !(uiMask & (HDI_WIDTH | HDI_ORDER)) ) { break; }
				if ( pNmHdr->iItem == m_iSnapCol && (uiMask & HDI_WIDTH) ) {
					// Whoever set it, this is now the right-most column's internal width.
					m_iSnapColWidth = GetColumnWidth( m_iSnapCol );
				}
				// Deferred: this arrives in the middle of the control's own update (after a divider double-click, for example).
				RequestSnap();
				break;
			}
			case HDN_ENDDRAG : {
				// The new column order is applied after this notification returns.
				RequestSnap();
				break;
			}
		}
	}

	/**
	 * Builds the name passed to RegisterWindowMessageW() for the snap message.
	 *
	 * The name is generated at run time rather than stored as a literal, so the binary holds no fixed, searchable string by which the message
	 *	(or this class) could be identified.  The first character is fixed so the name belongs to this class and the rest are random, giving a
	 *	name well over 16 characters long that will not collide with another registered message while matching nothing searchable.  Only single
	 *	character constants and generic arithmetic constants reach the binary; no string does.
	 *
	 * \return Returns the generated message name.
	 **/
	static std::wstring MakeSnapMessageName() {
		// Mix entropy that differs between runs and machines into a 64-bit seed.
		uint64_t ui64Seed = 0x9E3779B97F4A7C15ULL;
		LARGE_INTEGER liCounter = {};
		::QueryPerformanceCounter( &liCounter );
		ui64Seed ^= static_cast<uint64_t>(liCounter.QuadPart);
		ui64Seed ^= static_cast<uint64_t>(::GetTickCount64()) * 0x100000001B3ULL;
		ui64Seed ^= static_cast<uint64_t>(::GetCurrentProcessId()) << 16;
		ui64Seed ^= static_cast<uint64_t>(::GetCurrentThreadId()) << 32;
		ui64Seed ^= static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&liCounter));				// Stack address (ASLR).
		ui64Seed ^= static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&MakeSnapMessageName));	// Code address (ASLR).

		// SplitMix64 keeps the generator self-contained: its constants are generic and shared by countless projects, so nothing here is a
		//	fingerprint, and no string-bearing library facility is pulled in.
		auto aNext = [&ui64Seed]() -> uint64_t {
			ui64Seed += 0x9E3779B97F4A7C15ULL;
			uint64_t ui64Z = ui64Seed;
			ui64Z = (ui64Z ^ (ui64Z >> 30)) * 0xBF58476D1CE4E5B9ULL;
			ui64Z = (ui64Z ^ (ui64Z >> 27)) * 0x94D049BB133111EBULL;
			return ui64Z ^ (ui64Z >> 31);
		};

		// A fixed first character, then 23 random letters: 24 characters in all, comfortably over the 16 that keep collisions negligible.
		std::wstring wsName;
		wsName.push_back( L'L' );
		for ( INT I = 0; I < 23; ++I ) {
			wsName.push_back( static_cast<wchar_t>(L'a' + static_cast<wchar_t>(aNext() % 26)) );
		}
		return wsName;
	}

	/**
	 * Gets the private message that RequestSnap() posts to the list-view control.
	 *
	 * The name registered is generated once, at run time, by MakeSnapMessageName(); every instance in the process shares the message it
	 *	returns.
	 *
	 * \return Returns the registered message.
	 **/
	UINT CListView::SnapMessage() {
		static const UINT uiMsg = ::RegisterWindowMessageW( MakeSnapMessageName().c_str() );
		return uiMsg;
	}

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
	LRESULT CALLBACK CListView::SnapSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uiptrId, DWORD_PTR _dwpRefData ) {
		CListView * plvThis = reinterpret_cast<CListView *>(_dwpRefData);
		if ( _uMsg == SnapMessage() ) {
			// Posted by RequestSnap().  A modal loop running inside one of the control's messages (a context menu, a drag) can deliver it
			//	early.  Then it stays pending and is posted again when the outermost message returns.
			plvThis->m_bSnapPosted = false;
			if ( plvThis->m_iSnapMsgDepth == 0 && plvThis->m_bSnapPending ) { plvThis->SnapNow(); }
			return 0;
		}
		if ( _uMsg == WM_NCDESTROY ) {
			plvThis->RemoveSnapSubclass( _hWnd );
			return ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam );
		}

		const INT iDepth = plvThis->m_iSnapMsgDepth++;
		plvThis->SnapBeforeMessage( _uMsg, _wParam, _lParam );
		LRESULT lrRet = ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam );

		// The message may have destroyed the control, and this object with it.  Either one removes the subclass.
		DWORD_PTR dwpRefData = 0;
		if ( !::GetWindowSubclass( _hWnd, SnapSubclassProc, _uiptrId, &dwpRefData ) || dwpRefData != _dwpRefData ) { return lrRet; }

		plvThis->SnapAfterMessage( _uMsg, _wParam, _lParam, lrRet, iDepth );
		if ( --plvThis->m_iSnapMsgDepth == 0 && plvThis->m_bSnapPending && !plvThis->m_bSnapPosted ) {
			plvThis->RequestSnap();
		}
		return lrRet;
	}

}	// namespace lsw

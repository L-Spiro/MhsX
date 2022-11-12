#include "LSWListView.h"
#include <codecvt>
#include <CommCtrl.h>
#include <locale>

namespace lsw {

	CListView::CListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_sColumns( 0 ),
		m_bSortWithCase( FALSE ) {
	}

	// == Functions.
	// If the list-view control was created without the LVS_OWNERDATA style, this macro causes the control to allocate its internal data structures for
	//	the specified number of items. This prevents the control from having to allocate the data structures every time an item is added.
	VOID CListView::SetItemCount( INT _cItems ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), LVM_SETITEMCOUNT, static_cast<WPARAM>(_cItems), 0 );
		}
	}

	// Sets the virtual number of items in a virtual list view.
	VOID CListView::SetItemCountEx( INT _cItems, DWORD _dwFlags ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), LVM_SETITEMCOUNT, static_cast<WPARAM>(_cItems), static_cast<LPARAM>(_dwFlags) );
		}
	}

	// Gets the number of items in a list-view control.
	int CListView::GetItemCount() const {
		return static_cast<int>(::SendMessageW( Wnd(), LVM_GETITEMCOUNT, 0L, 0L ));
	}

	// Determines the number of selected items in a list-view control.
	UINT CListView::GetSelectedCount() const {
		return static_cast<UINT>(::SendMessageW( Wnd(), LVM_GETSELECTEDCOUNT, 0, 0L ));
	}

	// Removes an item from a list-view control.
	BOOL CListView::DeleteItem( int _iItem ) {
		return static_cast<BOOL>(::SendMessageW( Wnd(), LVM_DELETEITEM, static_cast<WPARAM>(_iItem), 0L ));
	}

	// Gets the number of columns.
	INT CListView::GetTotalColumns() const {
		if ( Wnd() ) {
			HWND hHeader = reinterpret_cast<HWND>(::SendMessageW( Wnd(), LVM_GETHEADER, 0L, 0L ));
			if ( hHeader ) {
				return static_cast<INT>(::SendMessageW( hHeader, HDM_GETITEMCOUNT, 0L, 0L ));
			}
		}
		return 0;
	}

	// Inserts a column at the given index.
	INT CListView::InsertColumn( INT _iIndex, const WCHAR * _pwcText, INT _iFormat ) {
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

	// Inserts a column at the given index.
	INT CListView::InsertColumn( INT _iIndex, const CHAR * _pcText, INT _iFormat ) {
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

	// Inserts a column.
	BOOL CListView::InsertColumn( const WCHAR * _pwcText, INT _iWidth, INT _iIdx ) {
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

	// Adds a column with the given text.
	INT CListView::AddColumn( const CHAR * _pcText, INT _iFormat ) {
		return AddColumn( ee::ToUtf16( _pcText ).c_str(), _iFormat );
	}

	// Adds a column with the given text.
	INT CListView::AddColumn( const WCHAR * _pwcText, INT _iFormat ) {
		return InsertColumn( static_cast<INT>(m_sColumns), _pwcText, _iFormat );
	}

	// Sets the width of a column.
	BOOL CListView::SetColumnWidth( INT _iCol, INT _iWidth ) {
		return ListView_SetColumnWidth( Wnd(), _iCol, _iWidth );
	}

	// Gets the width of a column.
	INT CListView::GetColumnWidth( INT _iCol ) const {
		return static_cast<INT>(::SendMessageW( Wnd(), LVM_GETCOLUMNWIDTH, static_cast<WPARAM>(_iCol), 0 ));
	}

	// Sets the text of a column.
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

	// Deletes a column.
	BOOL CListView::DeleteColumn( INT _iCol ) {
		if ( static_cast<BOOL>(::SendMessageW( Wnd(), LVM_DELETECOLUMN, static_cast<WPARAM>(_iCol), 0 )) ) {
			--m_sColumns;
			return TRUE;
		}
		return FALSE;
	}

	// Deletes all columns.
	VOID CListView::DeleteAllColumns() {
		while ( GetTotalColumns() ) {
			DeleteColumn( 0 );
		}
	}

	// Inserts an item.  Returns the index of the item.
	INT CListView::InsertItem( const LVITEMW &_iItem ) {
		return static_cast<INT>(::SendMessageW( Wnd(), LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&_iItem)));
	}

	// Inserts an item.  Returns the index of the item.
	INT CListView::InsertItem( const LVITEMA &_iItem ) {
		return static_cast<INT>(::SendMessageA( Wnd(), LVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&_iItem)));
	}

	// Inserts an item that consistes of text and a parameter.
	INT CListView::InsertItem( const WCHAR * _pwcText, LPARAM _lParam ) {
		LVITEMW iItem = { 0 };
		iItem.mask = LVIF_TEXT | LVIF_PARAM;
		iItem.iItem = 0x7FFFFFFF;
		iItem.lParam = _lParam;
		iItem.pszText = const_cast<LPWSTR>(_pwcText);
		return InsertItem( iItem );
	}

	// Inserts an item that consistes of text and a parameter.
	INT CListView::InsertItem( const CHAR * _pcText, LPARAM _lParam ) {
		LVITEMA iItem = { 0 };
		iItem.mask = LVIF_TEXT | LVIF_PARAM;
		iItem.iItem = 0x7FFFFFFF;
		iItem.lParam = _lParam;
		iItem.pszText = const_cast<LPSTR>(_pcText);
		return InsertItem( iItem );
	}

	// Sets the text for an item.
	VOID CListView::SetItemText( INT _iItem, INT _iSubItem, const WCHAR * _pwcText ) {
		LVITEMW iItem = { 0 };
		iItem.iItem = _iItem;
		iItem.iSubItem = _iSubItem;
		iItem.pszText = const_cast<LPWSTR>(_pwcText);
		::SendMessageW( Wnd(), LVM_SETITEMTEXTW, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&iItem) );
	}

	// Sets the text for an item.
	VOID CListView::SetItemText( INT _iItem, INT _iSubItem, const CHAR * _pcText ) {
		LVITEMA iItem = { 0 };
		iItem.iItem = _iItem;
		iItem.iSubItem = _iSubItem;
		iItem.pszText = const_cast<LPSTR>(_pcText);
		::SendMessageA( Wnd(), LVM_SETITEMTEXTA, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&iItem) );
	}

	// Gets the length of an item's text in WCHAR units.
	INT CListView::GetItemTextLenW( INT _iItem, INT _iSubItem ) {
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

	// Gets the length of an item's text in CHAR units.
	INT CListView::GetItemTextLenA( INT _iItem, INT _iSubItem ) {
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

	// Gets the text of an item.
	VOID CListView::GetItemText( INT _iItem, INT _iSubItem, std::wstring &_sRet ) {
		INT iLen = GetItemTextLenW( _iItem,  _iSubItem );
		LVITEMW liItem = { 0 };
		liItem.iSubItem = _iSubItem;
		liItem.pszText = new( std::nothrow ) WCHAR[iLen];
		liItem.cchTextMax = iLen;
		::SendMessageA( Wnd(), LVM_GETITEMTEXTW, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&liItem) );
		_sRet = liItem.pszText;
		delete [] liItem.pszText;
	}

	// Gets the text of an item.
	VOID CListView::GetItemText( INT _iItem, INT _iSubItem, std::string &_sRet ) {
		INT iLen = GetItemTextLenA( _iItem,  _iSubItem );
		LVITEMA liItem = { 0 };
		liItem.iSubItem = _iSubItem;
		liItem.pszText = new( std::nothrow ) CHAR[iLen];
		liItem.cchTextMax = iLen;
		::SendMessageA( Wnd(), LVM_GETITEMTEXTA, static_cast<WPARAM>(_iItem), reinterpret_cast<LPARAM>(&liItem) );
		_sRet = liItem.pszText;
		delete [] liItem.pszText;
	}

	// Gets the index of the (first) selected item or -1.
	INT CListView::GetFirstSelectedItem() const {
		if ( !Wnd() ) { return -1; }
		return static_cast<INT>(::SendMessageW( Wnd(), LVM_GETNEXTITEM, static_cast<WPARAM>(-1), MAKELPARAM( LVNI_SELECTED, 0 ) ));
	}

	// Gets the data of the selected item or returns -1.
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

	// Gets an item.  _iItm is input and output.
	BOOL CListView::GetItem( INT _iItem, INT _iSubItem, LVITEMW &_iItm ) const {
		if ( !Wnd() ) { return FALSE; }
		_iItm.iItem = _iItem;
		_iItm.iSubItem = _iSubItem;
		return static_cast<BOOL>(::SendMessageW( Wnd(), LVM_GETITEMW, 0, reinterpret_cast<LPARAM>(&_iItm) ));
	}

	// Gets an item.  _iItm is input and output.
	BOOL CListView::GetItem( INT _iItem, INT _iSubItem, LVITEMA &_iItm ) const {
		if ( !Wnd() ) { return FALSE; }
		_iItm.iItem = _iItem;
		_iItm.iSubItem = _iSubItem;
		return static_cast<BOOL>(::SendMessageA( Wnd(), LVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&_iItm) ));
	}

	// Creates an array of indices that correspond to the selected items in the list-view.
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

	// Gets the item's state.
	UINT CListView::GetItemState( INT _iItem, UINT _uiMask ) const {
		if ( !Wnd() ) { return 0; }
		return static_cast<UINT>(::SendMessageW( Wnd(), LVM_GETITEMSTATE, static_cast<WPARAM>(_iItem), static_cast<LPARAM>(_uiMask) ));
	}

	// Indicates whether an item in the list-view control is visible.
	BOOL CListView::IsItemVisible( INT _iItem ) const {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageA( Wnd(), LVM_ISITEMVISIBLE, static_cast<WPARAM>(_iItem), 0L ));
	}

	// Sort items.
	BOOL CListView::SortItems( INT _iSubItem ) {
		LSW_LISTSORT lsSort = {
			this,
			_iSubItem,
		};
		return static_cast<BOOL>(::SendMessageA( Wnd(), LVM_SORTITEMSEX, reinterpret_cast<WPARAM>(&lsSort), reinterpret_cast<LPARAM>(CompareFunc) ));
	}

	// Sort comparison function.  Override to change how items compare against each other.
	int CListView::SortCompare( INT _iLeft, INT _iRight, INT _iSub ) {
		std::wstring sLeft, sRight;
		GetItemText( _iLeft, _iSub, sLeft );
		GetItemText( _iRight, _iSub, sRight );
		return m_bSortWithCase ? std::wcscmp( sLeft.c_str(), sRight.c_str() ) : _wcsicmp( sLeft.c_str(), sRight.c_str() );
	}

	// Delete all items.
	VOID CListView::DeleteAll() {
		::SendMessageW( Wnd(), LVM_DELETEALLITEMS, 0, 0 );
	}

	// Setting the HWND after the control has been created.
	void CListView::InitControl( HWND _hWnd ) {
		CWidget::InitControl( _hWnd );
		ListView_SetExtendedListViewStyleEx( Wnd(), m_dwExtendedStyles, 0xFFFFFFFF );
	}

	// Sort routine.
	int CALLBACK CListView::CompareFunc( LPARAM _lParam1, LPARAM _lParam2, LPARAM _lParamSort ) {
		LSW_LISTSORT * plsSort = reinterpret_cast<LSW_LISTSORT *>(_lParamSort);
		return plsSort->plvListView->SortCompare( static_cast<INT>(_lParam1), static_cast<INT>(_lParam2), plsSort->iSubItem );
	}

}	// namespace lsw

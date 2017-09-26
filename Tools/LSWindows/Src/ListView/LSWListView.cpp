#include "LSWListView.h"
#include <CommCtrl.h>

namespace lsw {

	CListView::CListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget ),
		m_sColumns( 0 ) {
	}

	// == Functions.
	// Inserts a column at the given index.
	INT CListView::InsertColumn( INT _iIndex, const CHAR * _pcText, INT _iFormat ) {
		LV_COLUMNA lvColumn;
		lvColumn.mask = LVCF_TEXT | LVCF_FMT;
		lvColumn.pszText = const_cast<LPSTR>(_pcText);
		lvColumn.fmt = _iFormat;
		INT iInserted = static_cast<INT>(::SendMessageA( Wnd(), LVM_INSERTCOLUMNA, static_cast<WPARAM>(_iIndex), reinterpret_cast<LPARAM>(&lvColumn)));
		if ( iInserted != -1 ) {
			++m_sColumns;
		}
		return iInserted;
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

	// Adds a column with the given text.
	INT CListView::AddColumn( const CHAR * _pcText, INT _iFormat ) {
		return InsertColumn( static_cast<INT>(m_sColumns), _pcText, _iFormat );
	}

	// Adds a column with the given text.
	INT CListView::AddColumn( const WCHAR * _pwcText, INT _iFormat ) {
		return InsertColumn( static_cast<INT>(m_sColumns), _pwcText, _iFormat );
	}

	// Sets the width of a column.
	BOOL CListView::SetColumnWidth( INT _iCol, INT _iWidth ) {
		return ListView_SetColumnWidth( Wnd(), _iCol, _iWidth );
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

}	// namespace lsw

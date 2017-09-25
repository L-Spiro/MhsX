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

}	// namespace lsw

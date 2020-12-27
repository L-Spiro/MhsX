#include "LSWListBox.h"
#include "../Base/LSWBase.h"
#include <Windowsx.h>

namespace lsw {

	CListBox::CListBox( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	// == Functions.
	// Adds a string to the list box.  The return value is the zero-based index of the string in the list box. If an error occurs,
	//	the return value is LB_ERR. If there is insufficient space to store the new string, the return value is LB_ERRSPACE.
	INT CListBox::AddString( LPCSTR _lpStr ) {
		if ( !Wnd() ) { return LB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageA( Wnd(), LB_ADDSTRING, 0L, reinterpret_cast<LPARAM>(_lpStr) )));
	}

	// Adds a string to the list box.  The return value is the zero-based index of the string in the list box. If an error occurs,
	//	the return value is LB_ERR. If there is insufficient space to store the new string, the return value is LB_ERRSPACE.
	INT CListBox::AddString( LPCWSTR _lpStr ) {
		if ( !Wnd() ) { return LB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), LB_ADDSTRING, 0L, reinterpret_cast<LPARAM>(_lpStr) )));
	}

	// Gets the number of items in the list box.
	INT CListBox::GetCount() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), LB_GETCOUNT, 0L, 0L )));
	}

	// Gets the total number of selected items in a multiple-selection list box.
	INT CListBox::GetSelCount() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), LB_GETSELCOUNT, 0L, 0L )));
	}

	// Gets the index of the currently selected item in a single-selection list box.
	INT CListBox::GetCurSel() const {
		if ( !Wnd() ) { return LB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), LB_GETCURSEL, 0L, 0L )));
	}

	// Sets the currently selected item in a single-selection list box
	INT CListBox::SetCurSel( INT _iIndex ) {
		if ( !Wnd() ) { return LB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), LB_SETCURSEL, static_cast<WPARAM>(static_cast<INT>(_iIndex)), 0L )));
	}

	// Fills a buffer with an array of integers that specify the item numbers of selected items in a multiple-selection list box.
	bool CListBox::GetSelItems( std::vector<INT> &_vResult ) const {
		_vResult.clear();
		INT iTotal = GetSelCount();
		if ( iTotal == LB_ERR ) { return false; }
		if ( !iTotal ) { return true; }
		_vResult.resize( iTotal );
		return static_cast<INT>(::SendMessageW( Wnd(), LB_GETSELITEMS, static_cast<WPARAM>(iTotal), reinterpret_cast<LPARAM>(&_vResult[0]) )) != LB_ERR;
	}

	// Selects an item in a multiple-selection list box and, if necessary, scrolls the item into view.
	bool CListBox::SetSel( BOOL _bSelected, INT _iIndex ) {
		if ( !Wnd() ) { return false; }
		return static_cast<INT>(::SendMessageW( Wnd(), LB_SETSEL, static_cast<WPARAM>(_bSelected), static_cast<LPARAM>(_iIndex) )) != LB_ERR;
	}

	// Gets the application-defined value associated with the specified list item in a list box.
	LRESULT CListBox::GetItemData( INT _iIndex ) const {
		if ( !Wnd() || _iIndex < 0 ) { return 0; }
		return static_cast<LRESULT>(static_cast<ULONG_PTR>(::SendMessageW( Wnd(), LB_GETITEMDATA, static_cast<WPARAM>(_iIndex), 0L )));
	}

	// Sets the application-defined value associated with the specified list item in a list box.
	INT CListBox::SetItemData( INT _iIndex, LPARAM _pData ) {
		if ( !Wnd() ) { return LB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), LB_SETITEMDATA, static_cast<WPARAM>(_iIndex), _pData )));
	}

	// Sets the height of list items or the selection field in a combo box.  If the index or height is invalid, the return value is CB_ERR.
	INT CListBox::SetItemHeight( INT _iIndex, INT _cyItem ) {
		if ( !Wnd() ) { return LB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), LB_SETITEMHEIGHT, static_cast<WPARAM>(_iIndex), static_cast<LPARAM>(_cyItem) )));
	}

	// Sets the selection based on item data.
	INT CListBox::SetCurSelByItemData( LPARAM _pData ) {
		INT iTotal = GetCount();
		for ( INT I = 0; I < iTotal; ++I ) {
			if ( GetItemData( I ) == _pData ) {
				return SetCurSel( I );
			}
		}
		return SetCurSel( -1 );
	}

	// Gets the currently selected items data.
	LPARAM CListBox::GetCurSelItemData() const {
		return GetItemData( GetCurSel() );
	}

	// Removes all items from the list box.
	void CListBox::ResetContent() {
		if ( !Wnd() ) { return; }
		::SendMessageW( Wnd(), LB_RESETCONTENT, 0, 0 );
	}

}	// namespace lsw

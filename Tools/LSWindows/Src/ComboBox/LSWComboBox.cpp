#include "LSWComboBox.h"
#include "../Helpers/LSWHelpers.h"
#include <windowsx.h>

namespace lsw {

	CComboBox::CComboBox( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
		if ( Wnd() ) {
			COMBOBOXINFO cbiInfo = { sizeof( cbiInfo ) };
			::GetComboBoxInfo( Wnd(), &cbiInfo );

			WCHAR szName[128];
			::GetClassNameW( cbiInfo.hwndList, szName, 128 );
			HWND hWnd = cbiInfo.hwndList;
		}
	}


	// == Functions.
	// Adds a string to a list in a combo box. If the combo box does not have the CBS_SORT style, the
	//	string is added to the end of the list. Otherwise, the string is inserted into the list and the list is sorted.
	INT CComboBox::AddString( LPCSTR _psString ) {
		if ( !Wnd() ) { return CB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageA( Wnd(), CB_ADDSTRING, 0L, reinterpret_cast<LPARAM>(_psString) )));
	}

	// Adds a string to a list in a combo box. If the combo box does not have the CBS_SORT style, the
	//	string is added to the end of the list. Otherwise, the string is inserted into the list and the list is sorted.
	INT CComboBox::AddString( LPCWSTR _pwsString ) {
		if ( !Wnd() ) { return CB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_ADDSTRING, 0L, reinterpret_cast<LPARAM>(_pwsString) )));
	}

	// Deletes the item at the specified location in a list in a combo box.
	INT CComboBox::DeleteString( INT  _iIndex ) {
		if ( !Wnd() ) { return CB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_DELETESTRING, static_cast<WPARAM>(_iIndex), 0L )));
	}

	// Gets the number of items in the list box of a combo box.
	INT CComboBox::GetCount() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_GETCOUNT, 0L, 0L )));
	}

	// Gets the length of an item's text.
	INT CComboBox::GetLBTextLen( INT _iIndex ) const {
		if ( !Wnd() ) { return 0; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_GETLBTEXTLEN, static_cast<WPARAM>(_iIndex), 0L )));
	}

	// Gets a string from a list in a combo box.
	INT CComboBox::GetLBText( INT _iIndex, std::wstring &_wString ) const {
		_wString.clear();
		if ( !Wnd() ) { return CB_ERR; }

		INT iLen = GetLBTextLen( _iIndex );
		if ( !iLen ) { return CB_ERR; }

		LPWSTR lpszBuffer = new WCHAR[iLen+1];
		INT iRet = static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_GETLBTEXT, static_cast<WPARAM>(_iIndex), reinterpret_cast<LPARAM>(lpszBuffer) )));
		if ( iRet != CB_ERR ) {
			_wString = lpszBuffer;
		}
		delete [] lpszBuffer;
		return iRet;
	}

	// Gets the index of the currently selected item in a combo box.
	INT CComboBox::GetCurSel() const {
		if ( !Wnd() ) { return CB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_GETCURSEL, 0L, 0L )));
	}

	// Gets the application-defined value associated with the specified list item in a combo box.
	LRESULT CComboBox::GetItemData( INT _iIndex ) const {
		if ( !Wnd() ) { return 0; }
		return static_cast<LRESULT>(static_cast<ULONG_PTR>(::SendMessageW( Wnd(), CB_GETITEMDATA, static_cast<WPARAM>(_iIndex), 0L )));
	}

	// Retrieves the height of list items in a combo box.
	INT CComboBox::GetItemHeight() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_GETITEMHEIGHT, 0L, 0L )));
	}

	// Gets the minimum number of visible items in the drop-down list of a combo box.
	INT CComboBox::GetMinVisible() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<INT>(::SendMessageW( Wnd(), CB_GETMINVISIBLE, 0L, 0L ));
	}

	// Adds a string to a list in a combo box at the specified location.
	INT CComboBox::InsertString( INT _iIndex, LPCSTR _lpStr ) {
		if ( !Wnd() ) { return CB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageA( Wnd(), CB_INSERTSTRING, static_cast<WPARAM>(_iIndex), reinterpret_cast<LPARAM>(_lpStr) )));
	}

	// Adds a string to a list in a combo box at the specified location.
	INT CComboBox::InsertString( INT _iIndex, LPCWSTR _lpStr ) {
		if ( !Wnd() ) { return CB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_INSERTSTRING, static_cast<WPARAM>(_iIndex), reinterpret_cast<LPARAM>(_lpStr) )));
	}

	// Removes all items from the list box and edit control of a combo box.
	VOID CComboBox::ResetContent() {
		if ( Wnd() ) { ::SendMessageW( Wnd(), CB_RESETCONTENT, 0L, 0L ); }
	}

	// Sets the application-defined value associated with the specified list item in a combo box.
	INT CComboBox::SetItemData( INT _iIndex, LPARAM _pData ) {
		if ( !Wnd() ) { return CB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_SETITEMDATA, static_cast<WPARAM>(_iIndex), _pData )));
	}

	// Sets the height of list items or the selection field in a combo box.  If the index or height is invalid, the return value is CB_ERR.
	INT CComboBox::SetItemHeight( INT _iIndex, INT _cyItem ) {
		if ( !Wnd() ) { return CB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_SETITEMHEIGHT, static_cast<WPARAM>(_iIndex), static_cast<LPARAM>(_cyItem) )));
	}

	// Sets the minimum number of visible items in the drop-down list of a combo box.
	BOOL CComboBox::SetMinVisible( INT _iMinVisible ) {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageW( Wnd(), CB_SETMINVISIBLE, static_cast<WPARAM>(_iMinVisible), 0L ));
	}

	// Shows or hides the list in a combo box.
	BOOL CComboBox::ShowDropdown( BOOL _fShow ) {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageW( Wnd(), CB_SHOWDROPDOWN, static_cast<WPARAM>(_fShow), 0L ));
	}

	// Gets the rectangle for a string in the combo bod.
	LSW_RECT CComboBox::GetItemRect( INT _iIndex ) const {
		LSW_RECT rRet;
		rRet.Zero();
		if ( !Wnd() || _iIndex < 0 || _iIndex >= GetCount() ) {
			// Originally had rRet.Zero(); here but moved it out to possibly ensure certain behavior inside ::DrawTextW().
		}
		else {
			COMBOBOXINFO cbiInfo = { sizeof( cbiInfo ) };
			::GetComboBoxInfo( Wnd(), &cbiInfo );
			HDC hDc = ::GetDC( cbiInfo.hwndList );

			std::wstring wString;
			GetLBText( _iIndex, wString );
			::DrawTextW( hDc, wString.c_str(), wString.size(), &rRet, DT_CALCRECT );

			::ReleaseDC( cbiInfo.hwndList, hDc );
			//CHelpers::
		}
		return rRet;
	}

	// Gets the minimum width for the list box.
	LONG CComboBox::GetListMinWidth() const {
		LONG lRet = 0L;
		INT iTotal = GetCount();
		for ( INT I = 0; I < iTotal; ++I ) {
			LSW_RECT rTemp = GetItemRect( I );
			lRet = std::max( lRet, rTemp.Width() );
		}
		return lRet;
	}

	// Auto-sets the minimum width of the list box after adding all the strings.
	LONG CComboBox::AutoSetMinListWidth() {
		//LONG lRet = GetListMinWidth();
		//return static_cast<LONG>(::SendMessageW( Wnd(), CB_SETDROPPEDWIDTH, static_cast<WPARAM>(lRet), 0L ));
		return static_cast<LONG>(::SendMessageW( Wnd(), CB_SETDROPPEDWIDTH, 1L, 0L ));
	}

}	// namespace lsw

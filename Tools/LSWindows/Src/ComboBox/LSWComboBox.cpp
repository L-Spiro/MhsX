#include "LSWComboBox.h"
#include "../Helpers/LSWHelpers.h"
#include <codecvt>
//#include <windowsx.h>

namespace lsw {

	CComboBox::CComboBox( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		/*if ( Wnd() ) {
			COMBOBOXINFO cbiInfo = { sizeof( cbiInfo ) };
			::GetComboBoxInfo( Wnd(), &cbiInfo );

			WCHAR szName[128];
			::GetClassNameW( cbiInfo.hwndList, szName, 128 );
			HWND hWnd = cbiInfo.hwndList;
		}*/
	}


	// == Functions.
	// Prepares the combo box to receive a large number of items on init.
	INT CComboBox::InitStorage( WPARAM _wpTotalItems, LPARAM _lpStringBytes ) {
		if ( !Wnd() ) { return CB_ERRSPACE; }
		return static_cast<INT>(::SendMessageW( Wnd(), CB_INITSTORAGE, _wpTotalItems, _lpStringBytes ));
	}

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

		LPWSTR lpszBuffer = new( std::nothrow ) WCHAR[iLen+1];
		INT iRet = static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_GETLBTEXT, static_cast<WPARAM>(_iIndex), reinterpret_cast<LPARAM>(lpszBuffer) )));
		if ( iRet != CB_ERR ) {
			_wString = lpszBuffer;
		}
		delete [] lpszBuffer;
		return iRet;
	}

	// Copies the text of the specified window's title bar (if it has one) into a buffer. If the specified window is a control, the text of the control is copied.
	INT CComboBox::GetTextA( LPSTR _lpString, INT _nMaxCount ) const {
		INT iSel = GetCurSel();
		if ( iSel < 0 ) { return CWidget::GetTextA( _lpString, _nMaxCount ); }
		std::wstring wsTemp;
		GetLBText( iSel, wsTemp );
		std::string sTemp = ee::CExpEval::ToUtf8( wsTemp );
		if ( !_lpString ) { return static_cast<INT>(sTemp.size() + 1); }
		const CHAR * pcOrig = sTemp.c_str();
		char * pcNew = std::strncpy( _lpString, sTemp.c_str(), _nMaxCount );
		INT iRet = static_cast<INT>(pcNew - pcOrig);
		return iRet;
	}

	// Copies the text of the specified window's title bar (if it has one) into a buffer. If the specified window is a control, the text of the control is copied.
	INT CComboBox::GetTextW( LPWSTR _lpString, INT _nMaxCount ) const {
		INT iSel = GetCurSel();
		if ( iSel < 0 ) { return CWidget::GetTextW( _lpString, _nMaxCount ); }
		std::wstring wsTemp;
		GetLBText( iSel, wsTemp );
		if ( !_lpString ) { return static_cast<INT>(wsTemp.size() + 1); }
		const WCHAR * pcOrig = wsTemp.c_str();
		wchar_t * pcNew = std::wcsncpy( _lpString, wsTemp.c_str(), _nMaxCount );
		INT iRet = static_cast<INT>(pcNew - pcOrig);
		return iRet;
	}

	// Gets the window text.
	std::string CComboBox::GetTextA() const {
		INT iSel = GetCurSel();
		if ( iSel < 0 ) { return CWidget::GetTextA(); }
		std::wstring wsTemp;
		GetLBText( iSel, wsTemp );
		return ee::CExpEval::ToUtf8( wsTemp );
	}

	// Gets the window text.
	std::wstring CComboBox::GetTextW() const {
		INT iSel = GetCurSel();
		if ( iSel < 0 ) { return CWidget::GetTextW(); }
		std::wstring wsTemp;
		GetLBText( iSel, wsTemp );
		return wsTemp;
	}

	// Retrieves the length, in characters, of the specified window's title bar text (if the window has a title bar).
	//	If the specified window is a control, the function retrieves the length of the text within the control
	INT CComboBox::GetTextLengthA() const {
		INT iSel = GetCurSel();
		if ( iSel < 0 ) { return CWidget::GetTextLengthA(); }
		return GetLBTextLen( iSel );
	}

	// Retrieves the length, in characters, of the specified window's title bar text (if the window has a title bar).
	//	If the specified window is a control, the function retrieves the length of the text within the control
	INT CComboBox::GetTextLengthW() const {
		INT iSel = GetCurSel();
		if ( iSel < 0 ) { return CWidget::GetTextLengthW(); }
		return GetLBTextLen( iSel );
	}

	// Gets the index of the currently selected item in a combo box.
	INT CComboBox::GetCurSel() const {
		if ( !Wnd() ) { return CB_ERR; }
		return static_cast<INT>(static_cast<DWORD>(::SendMessageW( Wnd(), CB_GETCURSEL, 0L, 0L )));
	}

	// Gets the application-defined value associated with the specified list item in a combo box.
	LRESULT CComboBox::GetItemData( INT _iIndex ) const {
		if ( !Wnd() || _iIndex < 0 ) { return 0; }
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
#ifndef CB_GETMINVISIBLE
		return 30;	// I guess?
#else
		return static_cast<INT>(::SendMessageW( Wnd(), CB_GETMINVISIBLE, 0L, 0L ));
#endif	// #ifndef CB_GETMINVISIBLE
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
#ifndef CB_SETMINVISIBLE
		return FALSE;
#else
		return static_cast<BOOL>(::SendMessageW( Wnd(), CB_SETMINVISIBLE, static_cast<WPARAM>(_iMinVisible), 0L ));
#endif	// #ifndef CB_SETMINVISIBLE
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
#if ( WINVER >= 0x0500 )
			COMBOBOXINFO cbiInfo = { sizeof( cbiInfo ) };
			::GetComboBoxInfo( Wnd(), &cbiInfo );
			HDC hDc = ::GetDC( cbiInfo.hwndList );

			HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW( cbiInfo.hwndList, WM_GETFONT, 0L, 0L ));
			{
				LSW_SELECTOBJECT soFontOrig( hDc, hFont );	// Destructor sets the original font back.

				std::wstring wString;
				GetLBText( _iIndex, wString );
				::DrawTextW( hDc, wString.c_str(), static_cast<int>(wString.size()), &rRet, DT_CALCRECT );
			}

			::ReleaseDC( cbiInfo.hwndList, hDc );
#else
#endif	// #if ( WINVER >= 0x0500 )
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
		lRet += ::GetSystemMetrics( SM_CXEDGE ) * 2;
		if ( GetCount() > GetMinVisible() ) {
			lRet += ::GetSystemMetrics( SM_CXVSCROLL );
		}
		return lRet;
	}

	// Auto-sets the minimum width of the list box after adding all the strings.
	LONG CComboBox::AutoSetMinListWidth() {
		LONG lRet = GetListMinWidth();
		return static_cast<LONG>(::SendMessageW( Wnd(), CB_SETDROPPEDWIDTH, static_cast<WPARAM>(lRet), 0L ));
		//return static_cast<LONG>(::SendMessageW( Wnd(), CB_SETDROPPEDWIDTH, 200L, 0L ));
	}

	// Sets the selection based on index.
	INT CComboBox::SetCurSel( INT _iIndex ) {
		if ( !Wnd() || _iIndex >= GetCount() ) { return CB_ERR; }	// Don't handle (_iIndex < 0) here.
		return static_cast<INT>(::SendMessageW( Wnd(), CB_SETCURSEL, static_cast<WPARAM>(_iIndex), 0L ));
	}

	// Sets the selection based on item data.
	INT CComboBox::SetCurSelByItemData( LPARAM _pData ) {
		INT iTotal = GetCount();
		for ( INT I = 0; I < iTotal; ++I ) {
			if ( GetItemData( I ) == _pData ) {
				return SetCurSel( I );
			}
		}
		return SetCurSel( -1 );
	}

	// Gets the currently selected items data.
	LPARAM CComboBox::GetCurSelItemData() const {
		return GetItemData( GetCurSel() );
	}

	// Selects a range of text.  Implemented by CEdit and CComboBox.
	VOID CComboBox::SetSel( INT _iStart, INT _iEnd ) const {
		/*LRESULT lrRet = */::SendMessageW( Wnd(), CB_SETEDITSEL, 0L, MAKELPARAM( _iStart, _iEnd ) );
	}

	// Replaces the selected text in an edit control or a rich edit control with the specified text.
	void CComboBox::ReplaceSel( bool _bCanUndo, const wchar_t * _pwcText ) {
		COMBOBOXINFO cbiInfo = { sizeof( cbiInfo ) };
		if ( Wnd() && ::GetComboBoxInfo( Wnd(), &cbiInfo ) ) {
			::SendMessageW( cbiInfo.hwndItem, EM_REPLACESEL, _bCanUndo, reinterpret_cast<LPARAM>(_pwcText) );
		}
	}

}	// namespace lsw

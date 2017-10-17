#include "LSWListBox.h"
#include "../Base/LSWBase.h"
#include <Windowsx.h>

namespace lsw {

	CListBox::CListBox( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
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

}	// namespace lsw

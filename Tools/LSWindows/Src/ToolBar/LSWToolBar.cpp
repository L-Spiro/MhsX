#include "LSWToolBar.h"

namespace lsw {

	CToolBar::CToolBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
	}


	// == Functions.
	// Sets the image list.
	HIMAGELIST CToolBar::SetImageList( INT _iIndex, CImageList &_ilList ) {
		if ( !Wnd() ) { return NULL; }
		return reinterpret_cast<HIMAGELIST>(::SendMessageW( Wnd(), TB_SETIMAGELIST, static_cast<WPARAM>(_iIndex), reinterpret_cast<LPARAM>(_ilList.Handle()) ));
	}

	// Checks a button.
	BOOL CToolBar::CheckButton( WORD _wId, BOOL _bChecked ) {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageW( Wnd(), TB_CHECKBUTTON, static_cast<WPARAM>(_wId), MAKELPARAM( _bChecked, 0 ) ));
	}

	// Auto-sizes.
	VOID CToolBar::AutoSize() {
		if ( !Wnd() ) { return; }
		::SendMessageW( Wnd(), TB_AUTOSIZE, 0L, 0L );
	}

	// Gets the button count.
	DWORD CToolBar::GetButtonCount() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<DWORD>(::SendMessageW( Wnd(), TB_BUTTONCOUNT, 0L, 0L ));
	}

	// Deletes a button.
	BOOL CToolBar::DeleteButton( DWORD _dwIndex ) {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageW( Wnd(), TB_DELETEBUTTON, static_cast<WPARAM>(_dwIndex), 0L ));
	}

	// Enables or disables a button.
	BOOL CToolBar::EnableButton( WORD _wId, BOOL _bEnabled ) {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageW( Wnd(), TB_ENABLEBUTTON, static_cast<WPARAM>(_wId), MAKELPARAM( _bEnabled, 0 ) ));
	}

	// Gets a button's data.
	BOOL CToolBar::GetButton( DWORD _dwIndex, TBBUTTON &_tButton ) {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageW( Wnd(), TB_GETBUTTON, static_cast<WPARAM>(_dwIndex), reinterpret_cast<LPARAM>(&_tButton) ));
	}

	// Returns a DWORD value that contains the width and height values in the low word and high word, respectively.
	DWORD CToolBar::GetButtonSize() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<DWORD>(::SendMessageW( Wnd(), TB_GETBUTTONSIZE, 0L, 0L ));
	}

	// Gets the button text length in wchar_t units, including the NULL character.
	DWORD CToolBar::GetButtonTextLenW( WORD _wId ) const {
		if ( !Wnd() ) { return 1; }
		INT iRet = static_cast<INT>(::SendMessageW( Wnd(), TB_GETBUTTONTEXTW, static_cast<WPARAM>(_wId), NULL ));
		return static_cast<DWORD>(max( iRet + 1, 1 ));
	}

	// Gets the button text length in char units, including the NULL character.
	DWORD CToolBar::GetButtonTextLenA( WORD _wId ) const {
		if ( !Wnd() ) { return 1; }
		INT iRet = static_cast<INT>(::SendMessageA( Wnd(), TB_GETBUTTONTEXTA, static_cast<WPARAM>(_wId), NULL ));
		return static_cast<DWORD>(max( iRet + 1, 1 ));
	}

	// Gets the button text.  _pwcBuffer should be at least GetButtonTextLenW() characters in length.
	DWORD CToolBar::GetButtonTextW( WORD _wId, WCHAR * _pwcBuffer ) const {
		if ( !Wnd() ) { return -1; }
		return static_cast<DWORD>(::SendMessageW( Wnd(), TB_GETBUTTONTEXTW, static_cast<WPARAM>(_wId), reinterpret_cast<LPARAM>(_pwcBuffer) ));
	}

	// Gets the button text.  _pcBuffer should be at least GetButtonTextLenA() characters in length.
	DWORD CToolBar::GetButtonTextA( WORD _wId, CHAR * _pcBuffer ) const {
		if ( !Wnd() ) { return -1; }
		return static_cast<DWORD>(::SendMessageA( Wnd(), TB_GETBUTTONTEXTA, static_cast<WPARAM>(_wId), reinterpret_cast<LPARAM>(_pcBuffer) ));
	}

	// Gets a button's rectangle.
	LSW_RECT CToolBar::GetButtonRect( DWORD _dwIndex ) const {
		LSW_RECT rRet;
		rRet.left = rRet.top = rRet.right = rRet.bottom = 0;
		if ( !Wnd() ) { return rRet; }
		::SendMessageW( Wnd(), TB_GETITEMRECT, static_cast<WPARAM>(_dwIndex), reinterpret_cast<LPARAM>(&rRet) );
		return rRet;
	}

	// Gets a rectangle large enough to fit any of the buttons inside.
	LSW_RECT CToolBar::GetMinBoundingRect() const {
		LONG lW = 0, lH = 0;
		LSW_RECT rRet;
		DWORD dwTotal = GetButtonCount();
		for ( DWORD I = 0; I < dwTotal; ++I ) {
			rRet = GetButtonRect( I );
			lW = max( lW, rRet.Width() );
			lH = max( lH, rRet.Height() );
		}
		rRet.left = rRet.top = 0;
		rRet.SetWidth( lW );
		rRet.SetHeight( lH );
		return rRet;
	}

	// Adds buttons.
	BOOL CToolBar::AddButtons( const TBBUTTON * _pbButtons, size_t _sTotal ) {
		if ( !Wnd() ) { return FALSE; }
		::SendMessageW( Wnd(), TB_BUTTONSTRUCTSIZE, sizeof( (*_pbButtons) ), 0L );
		return static_cast<BOOL>(::SendMessageW( Wnd(), TB_ADDBUTTONS, static_cast<WPARAM>(_sTotal), reinterpret_cast<LPARAM>(_pbButtons) ));
	}

}	// namespace lsw

#include "LSWToolBar.h"

namespace lsw {

	CToolBar::CToolBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
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
		BOOL bRet = static_cast<BOOL>(::SendMessageW( Wnd(), TB_CHECKBUTTON, static_cast<WPARAM>(_wId), MAKELPARAM( _bChecked, 0 ) ));
		if ( bRet ) {
			size_t sIndex = IndexOfButton( _wId );
			if ( sIndex != static_cast<size_t>(-1) ) {
				if ( _bChecked ) {
					// Add TBSTATE_CHECKED.
					m_vButtons[sIndex].fsState |= TBSTATE_CHECKED;
				}
				else {
					// Remove TBSTATE_CHECKED.
					m_vButtons[sIndex].fsState &= ~TBSTATE_CHECKED;
				}
			}
		}
		return bRet;
	}

	// Auto-sizes.
	VOID CToolBar::AutoSize() {
		if ( !Wnd() ) { return; }
		::SendMessageW( Wnd(), TB_AUTOSIZE, 0L, 0L );
	}

	// Determines if a button is checked.
	BOOL CToolBar::IsChecked( WORD _wId ) const {
		TBBUTTONINFOW tbiButton = { 0 };
		tbiButton.cbSize = sizeof( tbiButton );
		tbiButton.dwMask = TBIF_STATE;
		if ( -1 != static_cast<BOOL>(::SendMessageW( Wnd(), TB_GETBUTTONINFOW, static_cast<WPARAM>(_wId), reinterpret_cast<LPARAM>(&tbiButton) )) ) {
			return (tbiButton.fsState & TBSTATE_CHECKED) ? TRUE : FALSE;
		}
		return FALSE;
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
		BOOL bRet = static_cast<BOOL>(::SendMessageW( Wnd(), TB_ENABLEBUTTON, static_cast<WPARAM>(_wId), MAKELPARAM( _bEnabled, 0 ) ));
		if ( bRet ) {
			size_t sIndex = IndexOfButton( _wId );
			if ( sIndex != static_cast<size_t>(-1) ) {
				if ( _bEnabled ) {
					// Add TBSTATE_ENABLED.
					m_vButtons[sIndex].fsState |= TBSTATE_ENABLED;
				}
				else {
					// Remove TBSTATE_ENABLED.
					m_vButtons[sIndex].fsState &= ~TBSTATE_ENABLED;
				}
			}
		}
		return bRet;
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
		if ( !Wnd() ) { return static_cast<DWORD>(-1); }
		return static_cast<DWORD>(::SendMessageW( Wnd(), TB_GETBUTTONTEXTW, static_cast<WPARAM>(_wId), reinterpret_cast<LPARAM>(_pwcBuffer) ));
	}

	// Gets the button text.  _pcBuffer should be at least GetButtonTextLenA() characters in length.
	DWORD CToolBar::GetButtonTextA( WORD _wId, CHAR * _pcBuffer ) const {
		if ( !Wnd() ) { return static_cast<DWORD>(-1); }
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
		BOOL bRet = static_cast<BOOL>(::SendMessageW( Wnd(), TB_ADDBUTTONSW, static_cast<WPARAM>(_sTotal), reinterpret_cast<LPARAM>(_pbButtons) ));
		if ( bRet ) {
			for ( size_t I = 0; I < _sTotal; ++I ) {
				m_vButtons.push_back( (*_pbButtons++) );
			}
			return TRUE;
		}
		return FALSE;
	}

	// TBN_QUERYINSERT.
	CWidget::LSW_HANDLED CToolBar::TbnQueryInsert( const LPNMTOOLBARW /*_lptbToolBar*/ ) {
		return LSW_H_HANDLED;
	}

	// TBN_QUERYDELETE.
	CWidget::LSW_HANDLED CToolBar::TbnQueryDelete( const LPNMTOOLBARW /*_lptbToolBar*/ ) {
		return LSW_H_HANDLED;
	}

	// TBN_RESET.
	CWidget::LSW_HANDLED CToolBar::TbnReset() {
		if ( !m_vButtons.size() ) { return LSW_H_CONTINUE; }
		std::vector<TBBUTTON> vCopy = m_vButtons;

		for ( DWORD I = GetButtonCount(); I--; ) {
			DeleteButton( 0 );
		}
		m_vButtons.clear();
		AddButtons( &vCopy[0], vCopy.size() );
		return LSW_H_CONTINUE;	// To keep the dialog open.
		//return LSW_H_HANDLED;	// To close the dialog.
	}

	// TBN_GETBUTTONINFO.
	CWidget::LSW_HANDLED CToolBar::TbnGetButtonInfo( LPNMTOOLBARW _lptbToolBar ) {
		if ( static_cast<size_t>(_lptbToolBar->iItem) >= m_vButtons.size() ) { return LSW_H_CONTINUE; }
		_lptbToolBar->tbButton = m_vButtons[_lptbToolBar->iItem];
		return LSW_H_HANDLED;
	}

	// Gets the index of a button given its command ID.
	size_t CToolBar::IndexOfButton( WORD _wId ) const {
		for ( size_t I = m_vButtons.size(); I--; ) {
			if ( m_vButtons[I].idCommand == _wId ) { return I; }
		}
		return static_cast<size_t>(-1);
	}

}	// namespace lsw

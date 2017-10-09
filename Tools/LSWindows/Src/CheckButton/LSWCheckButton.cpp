#include "LSWCheckButton.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CCheckButton::CCheckButton( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
		if ( _bCreateWidget ) {
			::SendMessageW( Wnd(), BM_SETCHECK, m_bActive ? BST_CHECKED : BST_UNCHECKED, 0 );
		}
	}

	// == Functions.
	// Setting the HWND after the control has been created.
	void CCheckButton::InitControl( HWND _hWnd ) {
		Parent::InitControl( _hWnd );
		::SendMessageW( Wnd(), BM_SETCHECK, m_bActive ? BST_CHECKED : BST_UNCHECKED, 0 );
	}

}	// namespace lsw

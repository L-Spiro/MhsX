#include "LSWEdit.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CEdit::CEdit( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
	}

// == Functions.
	// Selects a range of text.
	VOID CEdit::SetSel( INT _iStart, INT _iEnd ) const {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), EM_SETSEL, static_cast<WPARAM>(_iStart), static_cast<LPARAM>(_iEnd) );
		}
	}

}	// namespace lsw

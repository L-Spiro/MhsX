#include "LSWStatusBar.h"

namespace lsw {

	CStatusBar::CStatusBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
	}

	// == Functions.
	// Evaluates expressions to determine a new rectangle for the control.
	void CStatusBar::EvalNewSize() {
		::SendMessageW( Wnd(), WM_SIZE, 0L, 0L );
		::GetWindowRect( Wnd(), &m_rRect );
		::GetClientRect( Wnd(), &m_rClientRect );
	}

}	// namespace lsw

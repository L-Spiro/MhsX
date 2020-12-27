#include "LSWProgressBar.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CProgressBar::CProgressBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	// == Functions.
	// Sets the range for the progress bar.
	VOID CProgressBar::SetRange( INT _iLow, INT _iHi ) {
		if ( !Wnd() ) { return; }
		::SendMessageW( Wnd(), PBM_SETRANGE32, static_cast<WPARAM>(std::min( _iLow, _iHi )), static_cast<LPARAM>(std::max( _iLow, _iHi )) );
	}

	// Sets the position of the progress bar.
	INT CProgressBar::SetPos( INT _iPos ) {
		if ( !Wnd() ) { return 0; }
		return static_cast<INT>(::SendMessageW( Wnd(), PBM_SETPOS, static_cast<WPARAM>(_iPos), static_cast<LPARAM>(0) ));
	}

}	// namespace lsw

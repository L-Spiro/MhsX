#include "LSWStatic.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CStatic::CStatic( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	// == Functions.
	// Associates a new image with the static control.
	bool CStatic::SetImage( INT _iType, CBitmap &_bImage ) {
		if ( !Wnd() ) { return false; }
		::SendMessageW( Wnd(), STM_SETIMAGE, static_cast<WPARAM>(_iType), reinterpret_cast<LPARAM>(_bImage.Handle()) );
		return true;
	}

}	// namespace lsw

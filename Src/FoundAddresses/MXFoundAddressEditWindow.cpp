#include "MXFoundAddressEditWindow.h"


namespace mx {

	CFoundAddressEditWindow::CFoundAddressEditWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_ppParms( reinterpret_cast<MX_PARMS *>(_ui64Data) ) {
	}

}	// namespace mx

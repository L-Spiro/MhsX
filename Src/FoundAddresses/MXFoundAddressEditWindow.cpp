#include "MXFoundAddressEditWindow.h"


namespace mx {

	CFoundAddressEditWindow::CFoundAddressEditWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pParms( (*reinterpret_cast<MX_PARMS *>(_ui64Data)) ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CFoundAddressEditWindow::InitDialog() {


		return CParent::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CFoundAddressEditWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		
		return CParent::Command( _wCtrlCode, _Id, _pwSrc );
	}

}	// namespace mx

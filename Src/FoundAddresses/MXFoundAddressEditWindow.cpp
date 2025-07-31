#include "MXFoundAddressEditWindow.h"
#include "../Layouts/MXFoundAddressEditLayout.h"

#include <Tab/LSWTab.h>


namespace mx {

	CFoundAddressEditWindow::CFoundAddressEditWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pParms( (*reinterpret_cast<MX_PARMS *>(_ui64Data)) ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CFoundAddressEditWindow::InitDialog() {
		auto ptTab = reinterpret_cast<lsw::CTab *>(FindChild( CFoundAddressEditLayout::MX_FAEI_TAB ));
		if ( ptTab ) {
		}

		return CParent::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CFoundAddressEditWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {

		

		
		return CParent::Command( _wCtrlCode, _Id, _pwSrc );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CFoundAddressEditWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

}	// namespace mx

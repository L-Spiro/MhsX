#include "MXOptionsWindow.h"
#include "../Layouts/MXOptionsLayout.h"

namespace mx {

	COptionsWindow::COptionsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COptionsWindow::InitDialog() {
		CWidget * pwWidget = COptionsLayout::CreateGeneralPage( this );
		return LSW_H_CONTINUE;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED COptionsWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

}	// namespace mx
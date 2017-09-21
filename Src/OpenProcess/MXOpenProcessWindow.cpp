#include "MXOpenProcessWindow.h"
#include "../Layouts/MXOpenProcessLayout.h"

namespace mx {

	COpenProcessWindow::~COpenProcessWindow() {
		int i =0;
	}

	// == Functions.
	// WM_CLOSE.
	CWidget::LSW_HANDLED COpenProcessWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED COpenProcessWindow::Command( WORD _Id, HWND _hControl ) {
		switch ( _Id ) {
			/*case COpenProcessLayout::MX_OP: {
				COpenProcessLayout::CreateOpenProcessDialog( this );
				break;
			}*/
		}
		return LSW_H_CONTINUE;
	}

}	// namespace mx

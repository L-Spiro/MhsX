#include "MXMhsMainWindow.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXOpenProcessLayout.h"

namespace mx {

	// == Functions.
	// WM_COMMAND from menu.
	CWidget::LSW_HANDLED CMhsMainWindow::MenuCommand( WORD _Id ) {
		switch ( _Id ) {
			case CMainWindowLayout::MX_MWMI_OPENPROCESS : {
				COpenProcessLayout::CreateOpenProcessDialog( this );
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

}	// namespace mx

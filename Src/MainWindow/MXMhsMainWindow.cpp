#include "MXMhsMainWindow.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXOpenProcessLayout.h"

namespace mx {

	CMhsMainWindow::CMhsMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget ) {

		/*::CreateWindowExW( WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW,
			L"ReBarWindow32",
			NULL,
			WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TBSTYLE_TRANSPARENT | TBSTYLE_REGISTERDROP | TBSTYLE_LIST | TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_NOMOVEY,
			0, 20,
			_wlLayout.dwWidth, 150,
			Wnd(),
			(HMENU)2,
			::GetModuleHandleA( NULL ),
			NULL );*/

		/*
		::CreateWindowExA( WS_EX_LEFT | WS_EX_LTRREADING| WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW | TBSTYLE_EX_DOUBLEBUFFER,
			"ToolbarWindow32",
			"",
			WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TBSTYLE_TRANSPARENT | TBSTYLE_REGISTERDROP | TBSTYLE_LIST | TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_TOP,
			0, 0,
			_wlLayout.dwWidth, 50,
			Wnd(),
			NULL,
			::GetModuleHandleA( NULL ),
			NULL );
		*/

	}

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

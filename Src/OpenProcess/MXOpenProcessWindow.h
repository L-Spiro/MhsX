#pragma once

#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {
	
	class COpenProcessWindow : public lsw::CMainWindow {
	public :
		COpenProcessWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true ) :
			lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget ) {
		}
		~COpenProcessWindow();


		// == Functions.
		// WM_CLOSE.
		virtual LSW_HANDLED					Close();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _Id, HWND _hControl );
	};

}	// namespace mx

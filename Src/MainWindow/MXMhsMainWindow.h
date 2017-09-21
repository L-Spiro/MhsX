#pragma once

#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {
	
	class CMhsMainWindow : public lsw::CMainWindow {
	public :
		CMhsMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true ) :
			lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget ) {
		}


		// == Functions.
		// WM_COMMAND from menu.
		virtual LSW_HANDLED					MenuCommand( WORD _Id );
	};

}	// namespace mx

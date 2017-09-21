#include "MXLayoutManager.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../OpenProcess/MXOpenProcessWindow.h"

namespace mx {

	// == Functions.
	// Creates a class based on its type.
	CWidget * CLayoutManager::CreateWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget ) {
		switch ( _wlLayout.ltType ) {
			case MX_MAIN_WINDOW : {
				return new CMhsMainWindow( _wlLayout, _pwParent,  _bCreateWidget );
			}
			case MX_OPEN_PROCESS_WINDOW : {
				return new COpenProcessWindow( _wlLayout, _pwParent,  _bCreateWidget );
			}
		}
		return lsw::CLayoutManager::CreateWidget( _wlLayout, _pwParent, _bCreateWidget );
	}

}	// namespace mx

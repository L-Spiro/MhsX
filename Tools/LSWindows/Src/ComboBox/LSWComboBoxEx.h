#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CComboBoxEx : public CWidget {
	public :
		CComboBoxEx( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );


		// == Functions.


	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw

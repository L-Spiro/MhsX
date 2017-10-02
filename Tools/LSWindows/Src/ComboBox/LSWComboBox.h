#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CComboBox : public CWidget {
	public :
		CComboBox( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true );


		// == Functions.


	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw

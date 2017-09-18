#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CGroupBox : public CWidget {
	public :
		CGroupBox( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true );


	protected :
		// == Functions.


	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw

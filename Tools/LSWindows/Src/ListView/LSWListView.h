#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CListView : public CWidget {
	public :
		CListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true );


	protected :
		// == Functions.


	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw

#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CMainWindow : public CWidget {
	public :
		CMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );


	protected :
		// == Functions.
		// WM_NCDESTROY.
		virtual LSW_HANDLED					NcDestroy();

	private :
		typedef CWidget						Parent;
	};
		
}	// namespace lsw

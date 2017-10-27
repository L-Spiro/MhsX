#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CDockable : public CWidget {
	public :
		CDockable( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );
		~CDockable();


		// == Functions.
		// The dockable message handler.
		static LRESULT CALLBACK				WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );


	protected :
		// == Functions.

	private :
		//typedef CWidget						Parent;
	};

}	// namespace lsw

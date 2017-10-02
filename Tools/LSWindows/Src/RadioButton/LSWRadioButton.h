#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CRadioButton : public CWidget {
	public :
		CRadioButton( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true );

		// == Functions.
		// Are we checked?
		virtual BOOL						IsChecked() const { return ::SendMessageW( Wnd(), BM_GETCHECK, 0, 0 ) == BST_CHECKED; }

	protected :
		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

	private :
		typedef CWidget						Parent;
	};
		
}	// namespace lsw

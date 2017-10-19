#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CCheckButton : public CWidget {
	public :
		CCheckButton( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );

		// == Functions.
		// Are we checked?
		virtual BOOL						IsChecked() const { return ::SendMessageW( Wnd(), BM_GETCHECK, 0, 0 ) == BST_CHECKED; }

		// Changes the check state of a button control.
		virtual VOID						CheckButton( UINT _uCheck ) { ::SendMessageW( Wnd(), BM_SETCHECK, static_cast<WPARAM>(_uCheck), 0L ); }

		// Sets the button state to BST_CHECKED.
		virtual VOID						Check() { CheckButton( BST_CHECKED ); }

		// Sets the button state to BST_UNCHECKED.
		virtual VOID						UnCheck() { CheckButton( BST_UNCHECKED ); }

		// Sets the button to BST_CHECKED or BST_UNCHECKED.
		virtual VOID						SetCheck( BOOL _bChecked ) { CheckButton( _bChecked ? BST_CHECKED : BST_UNCHECKED ); }


	protected :
		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

	private :
		typedef CWidget						Parent;
	};
		
}	// namespace lsw

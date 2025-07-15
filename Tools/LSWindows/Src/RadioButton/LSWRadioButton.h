#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CRadioButton : public CWidget {
	public :
		CRadioButton( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );

		// == Functions.
		// Are we checked?
		virtual BOOL						IsChecked() const { return CheckState() == BST_CHECKED; }

		// Changes the check state of a button control.
		virtual VOID						CheckButton( UINT _uCheck ) { ::SendMessageW( Wnd(), BM_SETCHECK, static_cast<WPARAM>(_uCheck), 0L ); }

		/**
		 * Gets the check state.
		 * 
		 * \return Returns the check state.
		 **/
		virtual INT							CheckState() const { return static_cast<INT>(::SendMessageW( Wnd(), BM_GETCHECK, 0, 0 )); }

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_RADIO; }

		// Returns true if this is a CRadioButton class.
		virtual bool						IsRadioButton() const { return true; }

	protected :
		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

	private :
		typedef CWidget						Parent;
	};
		
}	// namespace lsw

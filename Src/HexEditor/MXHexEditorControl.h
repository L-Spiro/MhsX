#pragma once

#include <LSWWin.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>


namespace mx {

	class CHexEditorControl : public lsw::CWidget {
	public :
		CHexEditorControl( const lsw::LSW_WIDGET_LAYOUT &_wlLayout, lsw::CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void								InitControl( HWND _hWnd );

		// Registers the control if it has not been registered already.  Redundant calls have no effect.  Must be called before creating this control.
		static void									PrepareControl();


	protected :
		// == Members.

		// The main window class.
		static ATOM									m_aAtom;

	private :
		typedef lsw::CWidget						CParent;
	};

}	// namespace mx

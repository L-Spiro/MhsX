#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CStatusBar : public CWidget {
	public :
		CStatusBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );


		// == Functions.


	protected :
		// == Functions.
		// Evaluates expressions to determine a new rectangle for the control.
		virtual void						EvalNewSize();

	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw

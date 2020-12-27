#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CButton : public CWidget {
	public :
		CButton( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_BUTTON; }

		// Returns true if this is a CButton class.
		virtual bool						IsButton() const { return true; }


	protected :
		// == Functions.

	private :
		typedef CWidget						Parent;
	};
		
}	// namespace lsw

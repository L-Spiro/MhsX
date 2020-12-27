#pragma once

#include "../LSWWin.h"
#include "../Images/LSWBitmap.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CStatic : public CWidget {
	public :
		CStatic( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_LABEL; }

		// Returns true if this is a CStatic class.
		virtual bool						IsStatic() const { return true; }

		// Associates a new image with the static control.
		virtual bool						SetImage( INT _iType, CBitmap &_bImage );


	protected :
		// == Functions.

	private :
		typedef CWidget						Parent;
	};
		
}	// namespace lsw

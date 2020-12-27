#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CProgressBar : public CWidget {
	public :
		CProgressBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Sets the range for the progress bar.
		VOID								SetRange( INT _iLow, INT _iHi );

		// Sets the position of the progress bar.
		INT									SetPos( INT _iPos );

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_PROGRESSBAR; }

		// Returns true if this is a CProgressBar class.
		virtual bool						IsProgressBar() const { return true; }

	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw

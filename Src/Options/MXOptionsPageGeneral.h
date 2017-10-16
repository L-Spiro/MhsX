#pragma once
#include "MXOptionsPage.h"
#include <LSWWin.h>

using namespace lsw;

namespace mx {

	class COptionsPageGeneral : public COptionsPage {
	public :
		COptionsPageGeneral( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );


		// == Functions.
		// WM_INITDIALOG.
		//virtual LSW_HANDLED					InitDialog();
	};

}	// namespace mx

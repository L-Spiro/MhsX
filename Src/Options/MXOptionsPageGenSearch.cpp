#include "MXOptionsPageGenSearch.h"
#include "../Strings/MXStringDecoder.h"

namespace mx {

	COptionsPageGenSearch::COptionsPageGenSearch( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		COptionsPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
	}

	// == Functions.
	// Get the name of the page.
	std::wstring COptionsPageGenSearch::GetName() const {
		return _DEC_WS_F530FBCF_General_Search;
	}

	// WM_INITDIALOG.
	/*CWidget::LSW_HANDLED COptionsPage::InitDialog() {
		return LSW_H_CONTINUE;
	}*/

}	// namespace mx

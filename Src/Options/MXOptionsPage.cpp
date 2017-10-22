#include "MXOptionsPage.h"

namespace mx {

	COptionsPage::COptionsPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		lsw::CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
	}

	// == Functions.
	// Verifies the options, returning an error string in case of error.
	BOOL COptionsPage::Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
		_pwWidget = nullptr;
		return FALSE;
	}

	// Copies all the settings to the MX_OPTIONS structure.
	BOOL COptionsPage::Finalize() {
		return TRUE;
	}

	// WM_INITDIALOG.
	/*CWidget::LSW_HANDLED COptionsPage::InitDialog() {
		return LSW_H_CONTINUE;
	}*/

}	// namespace mx

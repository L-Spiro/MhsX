#include "MXFoundAddressEditMainPage.h"
#include "../Strings/MXStringDecoder.h"


namespace mx {

	CFoundAddressEditMainPage::CFoundAddressEditMainPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CFoundAddressEditPageBase( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data )/*,
		m_poOptions( reinterpret_cast<MX_OPTIONS *>(_ui64Data) )*/ {
	}

	// == Functions.
	// Get the name of the page.
	std::wstring CFoundAddressEditMainPage::GetName() const {
		return _DEC_WS_01940FD6_General;
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CFoundAddressEditMainPage::InitDialog() {
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CFoundAddressEditMainPage::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		return LSW_H_CONTINUE;
	}

	// Verifies the options, returning an error string in case of error.
	BOOL CFoundAddressEditMainPage::Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
		return FALSE;
	}

	// Copies all the settings to the MX_OPTIONS structure.
	BOOL CFoundAddressEditMainPage::Finalize() {
		return FALSE;
	}

}	// namespace mx

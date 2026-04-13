#include "MXNumberInputDialog.h"


namespace mx {

	CNumberInputDialog::CNumberInputDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pndpParms( reinterpret_cast<CNumberInputLayout::MX_NUMBER_DIALOG_PARMS *>(_ui64Data) ) {
	}
	CNumberInputDialog::~CNumberInputDialog() {
	}

}	// namespace mx

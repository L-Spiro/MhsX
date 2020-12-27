#include "LSWToolTip.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CToolTip::CToolTip( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		m_wsTooltipText( _wlLayout.pwcText ? _wlLayout.pwcText : L"" ),
		CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	// Setting the HWND after the control has been created.
	void CToolTip::InitControl( HWND _hWnd ) {
		CWidget::InitControl( _hWnd );
	}

}	// namespace lsw

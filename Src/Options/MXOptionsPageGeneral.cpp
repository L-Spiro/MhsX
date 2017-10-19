#include "MXOptionsPageGeneral.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <CheckButton/LSWCheckButton.h>
#include <ComboBox/LSWComboBox.h>

namespace mx {

	COptionsPageGeneral::COptionsPageGeneral( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, MX_OPTIONS * _poOptions, bool _bCreateWidget, HMENU _hMenu ) :
		COptionsPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu ),
		m_poOptions( _poOptions ) {
	}

	// == Functions.
	// Get the name of the page.
	std::wstring COptionsPageGeneral::GetName() const {
		return _DEC_WS_01940FD6_General;
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COptionsPageGeneral::InitDialog() {
		if ( !m_poOptions ) { return LSW_H_CONTINUE; }
		CCheckButton * pcbCheck = nullptr;
		CComboBox * pcbCombo = nullptr;

#define MX_CHECK( ID, MEMBER )																					\
	pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::ID ));									\
	if ( pcbCheck ) {																							\
		pcbCheck->SetCheck( m_poOptions->MEMBER );																\
	}
		/*COptionsLayout::MX_OI_GENERAL_REFRESH_MAIN_LIST_CB;
		m_poOptions->dwFoundAddressRefresh;*/
		MX_CHECK( MX_OI_GENERAL_VIEW_CODE_NAMES, bDataTypesAsCodeNames );
		MX_CHECK( MX_OI_GENERAL_VIEW_SIZES, bDataTypeSizes );
		MX_CHECK( MX_OI_GENERAL_VIEW_RANGES, bDataTypeRanges );
#undef MX_CHECK

#define MX_TEXT( ID, MEMBER )																					\
	pcbCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::ID ));										\
	if ( pcbCombo ) {																							\
		pcbCombo->SetTextA( CUtilities::ToUnsigned( m_poOptions->MEMBER ) );									\
	}
		MX_TEXT( MX_OI_GENERAL_REFRESH_FOUND_ADDRESSES_CB, dwFoundAddressRefresh );
		MX_TEXT( MX_OI_GENERAL_REFRESH_MAIN_LIST_CB, dwMainRefresh );
		MX_TEXT( MX_OI_GENERAL_UPDDATE_LOCKED_VALUE_CB, dwLockedRefresh );
		MX_TEXT( MX_OI_GENERAL_UPDDATE_EXP_EVAL_CB, dwExpressionRefresh );
#undef MX_TEXT

		return LSW_H_CONTINUE;
	}

}	// namespace mx

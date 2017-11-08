#include "MXOptionsPageGeneral.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <CheckButton/LSWCheckButton.h>
#include <ComboBox/LSWComboBox.h>
#include <limits>

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

		UpdateExample();
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED COptionsPageGeneral::Command( WORD _Id, HWND _hControl ) {
		UpdateExample();
		return LSW_H_CONTINUE;
	}

	// Verifies the options, returning an error string in case of error.
	BOOL COptionsPageGeneral::Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
		if ( !m_poOptions ) { return TRUE; }
		ee::CExpEvalContainer::EE_RESULT eRes;

		CWidget * pwTemp = nullptr;
		pwTemp = FindChild( COptionsLayout::MX_OI_GENERAL_REFRESH_FOUND_ADDRESSES_CB );
		if ( pwTemp ) {
			if ( !pwTemp->GetTextAsUInt64Expression( eRes ) ) {
				_pwWidget = pwTemp;
				_wsError = _DEC_WS_32DCB4CD_Invalid_update_rate_;
				return FALSE;
			}
		}

		pwTemp = FindChild( COptionsLayout::MX_OI_GENERAL_REFRESH_MAIN_LIST_CB );
		if ( pwTemp ) {
			if ( !pwTemp->GetTextAsUInt64Expression( eRes ) ) {
				_pwWidget = pwTemp;
				_wsError = _DEC_WS_32DCB4CD_Invalid_update_rate_;
				return FALSE;
			}
		}

		pwTemp = FindChild( COptionsLayout::MX_OI_GENERAL_UPDDATE_LOCKED_VALUE_CB );
		if ( pwTemp ) {
			if ( !pwTemp->GetTextAsUInt64Expression( eRes ) ) {
				_pwWidget = pwTemp;
				_wsError = _DEC_WS_32DCB4CD_Invalid_update_rate_;
				return FALSE;
			}
		}

		pwTemp = FindChild( COptionsLayout::MX_OI_GENERAL_UPDDATE_EXP_EVAL_CB );
		if ( pwTemp ) {
			if ( !pwTemp->GetTextAsUInt64Expression( eRes ) ) {
				_pwWidget = pwTemp;
				_wsError = _DEC_WS_32DCB4CD_Invalid_update_rate_;
				return FALSE;
			}
		}


		return TRUE;
	}

	// Copies all the settings to the MX_OPTIONS structure.
	BOOL COptionsPageGeneral::Finalize() {
		if ( !m_poOptions ) { return TRUE; }
		CCheckButton * pcbCheck = nullptr;
		DWORD dwOpts = 0;

		// ==== BOOLEANS ==== //
#define MX_GETCHECK( ID, MEMBER )																				\
	pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::ID ));									\
	if ( pcbCheck ) {																							\
		m_poOptions->MEMBER = pcbCheck->IsChecked();															\
	}

		MX_GETCHECK( MX_OI_GENERAL_VIEW_CODE_NAMES, bDataTypesAsCodeNames );
		MX_GETCHECK( MX_OI_GENERAL_VIEW_SIZES, bDataTypeSizes );
		MX_GETCHECK( MX_OI_GENERAL_VIEW_RANGES, bDataTypeRanges );
#undef MX_GETCHECK

		ee::CExpEvalContainer::EE_RESULT eRes;
		CWidget * pwTemp = nullptr;
#define MX_GETVAL( ID, MEMBER )																														\
	pwTemp = FindChild( COptionsLayout::ID );																										\
	if ( pwTemp ) {																																	\
		if ( !pwTemp->GetTextAsUInt64Expression( eRes ) ) {																							\
			return FALSE;																															\
		}																																			\
		m_poOptions->MEMBER = static_cast<DWORD>(std::min( eRes.u.ui64Val, static_cast<UINT64>(numeric_limits<DWORD>::max()) ));					\
	}

		MX_GETVAL( MX_OI_GENERAL_REFRESH_FOUND_ADDRESSES_CB, dwFoundAddressRefresh );
		MX_GETVAL( MX_OI_GENERAL_REFRESH_MAIN_LIST_CB, dwMainRefresh );
		MX_GETVAL( MX_OI_GENERAL_UPDDATE_LOCKED_VALUE_CB, dwLockedRefresh );
		MX_GETVAL( MX_OI_GENERAL_UPDDATE_EXP_EVAL_CB, dwExpressionRefresh );

#undef MX_GETVAL

		return TRUE;
	}

	// Updates the text example of data-type settings.
	void COptionsPageGeneral::UpdateExample() {
		CWidget * pwExample = FindChild( COptionsLayout::MX_OI_GENERAL_VIEW_EXAMPLE );
		if ( pwExample ) {
			std::string sText = _DEC_S_4CEBDC18_Example__;
			DWORD dwOptions = 0;
			dwOptions |= FindChild( COptionsLayout::MX_OI_GENERAL_VIEW_CODE_NAMES )->IsChecked() ? CUtilities::MX_DTO_CODENAMES : 0;
			dwOptions |= FindChild( COptionsLayout::MX_OI_GENERAL_VIEW_SIZES )->IsChecked() ? CUtilities::MX_DTO_SHOWSIZES : 0;
			dwOptions |= FindChild( COptionsLayout::MX_OI_GENERAL_VIEW_RANGES )->IsChecked() ? CUtilities::MX_DTO_SHOWRANGES : 0;

			sText.push_back( ' ' );
			CUtilities::PrintDataType( sText, CUtilities::MX_DT_INT32, dwOptions );
			pwExample->SetTextA( sText.c_str() );
		}
	}

}	// namespace mx

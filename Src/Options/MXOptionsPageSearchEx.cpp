#include "MXOptionsPageSearchEx.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <CheckButton/LSWCheckButton.h>
#include <ComboBox/LSWComboBox.h>
#include <Edit/LSWEdit.h>
#include <RadioButton/LSWRadioButton.h>

namespace mx {

	COptionsPageSearchEx::COptionsPageSearchEx( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		COptionsPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<MX_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
	// Get the name of the page.
	std::wstring COptionsPageSearchEx::GetName() const {
		return _DEC_WS_956BFA86_Search_Ex;
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COptionsPageSearchEx::InitDialog() {
		if ( !m_poOptions ) { return LSW_H_CONTINUE; }

		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_SEARCH_EX_BYTESWAP_TYPE_COMBO ));
		if ( pcbCombo ) {
			const struct {
				const char *			pcText;
				size_t					sLen;
				CUtilities::MX_BYTESWAP	dParam;
			} aTitles[] = {
				{ _T_LEN_DFA2AFF1_None, CUtilities::MX_BS_NONE },
				{ _T_LEN_9CC212B3_Every_2_Bytes, CUtilities::MX_BS_2BYTE },
				{ _T_LEN_FF122789_Every_4_Bytes, CUtilities::MX_BS_4BYTE },
				{ _T_LEN_38B24DFD_Every_8_Bytes, CUtilities::MX_BS_8BYTE },
				{ _T_LEN_E5665240_By_Data_Type, CUtilities::MX_BS_BY_TYPE },
			};
			for ( size_t I = 0; I < MX_ELEMENTS( aTitles ); I++ ) {
				INT iIndex = pcbCombo->AddString( mx::CStringDecoder::DecodeToWString( aTitles[I].pcText, aTitles[I].sLen ).c_str() );
				if ( iIndex != -1 ) {
					pcbCombo->SetItemData( iIndex, static_cast<LPARAM>(aTitles[I].dParam) );
				}
			}
			pcbCombo->SetCurSel( m_poOptions->bsByteswap );
			pcbCombo->AutoSetMinListWidth();
		}

		CUtilities::FillAddressRangeComboBoxes( static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_SEARCH_EX_SEARCH_RANGE_LOW_COMBO )),
			static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_SEARCH_EX_SEARCH_RANGE_HIGH_COMBO )),
			m_poOptions );
		ApplySettings();
		
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED COptionsPageSearchEx::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		if ( !m_poOptions ) { return LSW_H_CONTINUE; }
		ApplySettings();
		return LSW_H_CONTINUE;
	}

	// Verifies the options, returning an error string in case of error.
	BOOL COptionsPageSearchEx::Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
		return TRUE;
	}

	// Copies all the settings to the MX_OPTIONS structure.
	BOOL COptionsPageSearchEx::Finalize() {
		if ( !m_poOptions ) { return TRUE; }
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_SEARCH_EX_BYTESWAP_TYPE_COMBO ));
		if ( !pcbCombo ) { return TRUE; }
		m_poOptions->bsByteswap = static_cast<CUtilities::MX_BYTESWAP>(pcbCombo->GetCurSelItemData());
		return TRUE;
	}

	// Apply dialog settings.
	VOID COptionsPageSearchEx::ApplySettings() {
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_SEARCH_EX_BYTESWAP_TYPE_COMBO ));
		if ( pcbCombo ) {

		}
	}

}	// namespace mx

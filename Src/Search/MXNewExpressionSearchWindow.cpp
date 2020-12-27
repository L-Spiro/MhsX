#include "MXNewExpressionSearchWindow.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXNewExpressionSearchLayout.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include <Base/LSWBase.h>
#include <Button/LSWButton.h>
#include <CheckButton/LSWCheckButton.h>
#include <ComboBox/LSWComboBox.h>
#include <Edit/LSWEdit.h>
#include <RadioButton/LSWRadioButton.h>
#include <Static/LSWStatic.h>


namespace mx {

	// == Members.
	CNewExpressionSearchWindow::MX_EXP_SEARCH_WIN_DATA CNewExpressionSearchWindow::m_eswdData = {
		CUtilities::MX_DT_UINT32,				// dtDataType
		CUtilities::MX_ESQM_ADDRESS,			// esqmEvalType
		false,									// bDynamic
	};

	CNewExpressionSearchWindow::CNewExpressionSearchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmhMemHack( reinterpret_cast<CMemHack *>(_ui64Data) ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CNewExpressionSearchWindow::InitDialog() {
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_TYPE_COMBO ));
		
		if ( pcbCombo ) {
			static const CUtilities::MX_DATA_TYPES dtTypes[] = {
				CUtilities::MX_DT_INT8,
				CUtilities::MX_DT_UINT8,
				CUtilities::MX_DT_INT16,
				CUtilities::MX_DT_UINT16,
				CUtilities::MX_DT_INT32,
				CUtilities::MX_DT_UINT32,
				CUtilities::MX_DT_INT64,
				CUtilities::MX_DT_UINT64,
				CUtilities::MX_DT_FLOAT16,
				CUtilities::MX_DT_FLOAT,
				CUtilities::MX_DT_DOUBLE
			};
			for ( size_t I = 0; I < MX_ELEMENTS( dtTypes ); ++I ) {
				CSecureWString sTemp;
				CUtilities::PrintDataType( sTemp, dtTypes[I] );
				INT iIndex = pcbCombo->AddString( sTemp.c_str() );
				pcbCombo->SetItemData( iIndex, dtTypes[I] );
			}
			pcbCombo->AutoSetMinListWidth();

			pcbCombo->SetCurSelByItemData( m_eswdData.dtDataType );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_COMBO ));
		if ( pcbCombo ) {
			struct {
				const char *							pcText;
				size_t									sLen;
				CUtilities::MX_EXP_SEARCH_QQ_MEANING	esqmType;
			} sTable[] = {
				{ _T_LEN_A27B4CE4_Current_Address_____Val___u32_________, CUtilities::MX_ESQM_ADDRESS },
				{ _T_LEN_141B20E3_Current_Value_____Val__________, CUtilities::MX_ESQM_VALUE },
			};

			for ( size_t I = 0; I < MX_ELEMENTS( sTable ); ++I ) {
				//CSecureWString sTemp;
				INT iIndex = pcbCombo->AddString( mx::CStringDecoder::DecodeToWString( sTable[I].pcText, sTable[I].sLen ).c_str() );
				pcbCombo->SetItemData( iIndex, sTable[I].esqmType );
			}
			pcbCombo->AutoSetMinListWidth();

			pcbCombo->SetCurSelByItemData( m_eswdData.esqmEvalType );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_FROM_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_eswdData.vFromHistory.size(); ++I ) {
				pcbCombo->AddString( m_eswdData.vFromHistory[I].c_str() );
			}
			if ( m_eswdData.wsFromText.size() == 0 ) {
				uintptr_t uiptrTemp = reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMinimumApplicationAddress);
				std::string sTemp = CUtilities::ToHex( uiptrTemp, 4 );
				pcbCombo->SetTextA( sTemp.c_str() );
			}
			else {
				pcbCombo->SetTextW( m_eswdData.wsFromText.c_str() );
			}
			pcbCombo->SetTreatAsHex( TRUE );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_TO_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_eswdData.vToHistory.size(); ++I ) {
				pcbCombo->AddString( m_eswdData.vToHistory[I].c_str() );
			}
			if ( m_eswdData.wsToText.size() == 0 ) {
				uintptr_t uiptrTemp = reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMaximumApplicationAddress);
				std::string sTemp = CUtilities::ToHex( uiptrTemp + 1ULL, 4 );
				pcbCombo->SetTextA( sTemp.c_str() );
			}
			else {
				pcbCombo->SetTextW( m_eswdData.wsToText.c_str() );
			}
			pcbCombo->SetTreatAsHex( TRUE );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}


		pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_OPTIONS_ALIGN_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_eswdData.vAlignmentHistory.size(); ++I ) {
				pcbCombo->AddString( m_eswdData.vAlignmentHistory[I].c_str() );
			}
			if ( m_eswdData.wsAlignmentText.size() == 0 ) {
				std::string sTemp = CUtilities::ToUnsigned( 4, 0 );
				pcbCombo->SetTextA( sTemp.c_str() );
			}
			else {
				pcbCombo->SetTextW( m_eswdData.wsAlignmentText.c_str() );
			}
			pcbCombo->SetTreatAsHex( TRUE );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_eswdData.vFixedSizeHistory.size(); ++I ) {
				pcbCombo->AddString( m_eswdData.vFixedSizeHistory[I].c_str() );
			}
			if ( m_eswdData.wsFixedSizeText.size() == 0 ) {
				std::string sTemp = CUtilities::ToUnsigned( 4, 0 );
				pcbCombo->SetTextA( sTemp.c_str() );
			}
			else {
				pcbCombo->SetTextW( m_eswdData.wsFixedSizeText.c_str() );
			}
			pcbCombo->SetTreatAsHex( TRUE );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_DUMMY_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_eswdData.vDummyHistory.size(); ++I ) {
				pcbCombo->AddString( m_eswdData.vDummyHistory[I].c_str() );
			}
			if ( m_eswdData.wsDummyText.size() == 0 ) {
				std::string sTemp = CUtilities::ToUnsigned( 4, 0 );
				pcbCombo->SetTextA( sTemp.c_str() );
			}
			else {
				pcbCombo->SetTextW( m_eswdData.wsDummyText.c_str() );
			}
			pcbCombo->SetTreatAsHex( TRUE );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		CButton * pbButton = static_cast<CButton *>(FindChild( CNewExpressionSearchLayout::MX_NESI_OPTIONS_GENERAL_OPTIONS_BUTTON ));
		if ( pbButton ) {
			pbButton->SetEnabled( m_pmhMemHack != nullptr );
		}


		CRadioButton * prbDynRadio = static_cast<CRadioButton *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_DYN_SIZE_RADIO ));
		CRadioButton * prbFixRadio = static_cast<CRadioButton *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_RADIO ));
		if ( prbDynRadio ) { prbDynRadio->SetCheck( m_eswdData.bDynamic ); }
		if ( prbFixRadio ) { prbFixRadio->SetCheck( !m_eswdData.bDynamic ); }

		CEdit * peEdit = static_cast<CEdit *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_EDIT ));
		if ( peEdit ) {
			peEdit->SetTextA( m_eswdData.ssExpression.c_str() );
		}

		UpdateDialog();
		return CMainWindow::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CNewExpressionSearchWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CNewExpressionSearchLayout::MX_NESI_CANCEL : {
				return Close();
			}
			case CNewExpressionSearchLayout::MX_NESI_SIZE_DYN_SIZE_RADIO : {}
			case CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_RADIO : {
				if ( _wCtrlCode == STN_CLICKED ) {
					UpdateDialog();
				}
				break;
			}
			case CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_COMBO : {}
			case CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_COMBO : {}
			case CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_TYPE_COMBO : {}
			case CNewExpressionSearchLayout::MX_NESI_EXPRESSION_DUMMY_COMBO : {}
			case CNewExpressionSearchLayout::MX_NESI_FROM_COMBO : {}
			case CNewExpressionSearchLayout::MX_NESI_TO_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CNewExpressionSearchLayout::MX_NESI_EXPRESSION_EDIT : {
				switch ( _wCtrlCode ) {
					case EN_CHANGE : {
						UpdateDialog();
						break;
					}
				}
				break;
			}
			case CNewExpressionSearchLayout::MX_NESI_OK : {
				CSecureWString wsError;
				CSearcher::MX_SEARCH_PARMS spParms;
				if ( !DialogContentsAreValid( wsError, spParms ) ) {
					lsw::CBase::MessageBoxError( Wnd(), wsError.c_str() );
					break;
				}
				SaveDialogData( m_eswdData );
				CMhsMainWindow * pmmwMain = static_cast<CMhsMainWindow *>(FindParent( mx::CMainWindowLayout::MX_MWI_MAINWINDOW ));
				if ( pmmwMain ) {
					pmmwMain->Search( &spParms, this, true, nullptr );
				}
				return Close();
			}
			case CNewExpressionSearchLayout::MX_NESI_OPTIONS_GENERAL_OPTIONS_BUTTON : {
				CMhsMainWindow * pmmwMain = static_cast<CMhsMainWindow *>(FindParent( mx::CMainWindowLayout::MX_MWI_MAINWINDOW ));
				if ( pmmwMain ) {
					pmmwMain->ShowOptions( pmmwMain->SearchOptionsPage() );
					UpdateDialog();
				}
				break;
			}
		}
		return CMainWindow::Command( _wCtrlCode, _Id, _pwSrc );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CNewExpressionSearchWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	// Saves the current dialog values to an MX_SEARCH_WIN_DATA structure.
	void CNewExpressionSearchWindow::SaveDialogData( MX_EXP_SEARCH_WIN_DATA &_eswdData ) {
		if ( m_pmhMemHack ) {
			MX_OPTIONS oOpts = m_pmhMemHack->Options();





		}


		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_FROM_COMBO ));
		if ( pcbCombo ) {
			_eswdData.wsFromText = pcbCombo->GetTextW();
			if ( _eswdData.wsFromText.size() ) {
				auto aFound = std::find( _eswdData.vFromHistory.begin(), _eswdData.vFromHistory.end(), _eswdData.wsFromText );
				if ( aFound != _eswdData.vFromHistory.end() ) {
					_eswdData.vFromHistory.erase( aFound );
				}
				_eswdData.vFromHistory.insert( _eswdData.vFromHistory.begin(), _eswdData.wsFromText );
			}
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_TO_COMBO ));
		if ( pcbCombo ) {
			_eswdData.wsToText = pcbCombo->GetTextW();
			if ( _eswdData.wsToText.size() ) {
				auto aFound = std::find( _eswdData.vToHistory.begin(), _eswdData.vToHistory.end(), _eswdData.wsToText );
				if ( aFound != _eswdData.vToHistory.end() ) {
					_eswdData.vToHistory.erase( aFound );
				}
				_eswdData.vToHistory.insert( _eswdData.vToHistory.begin(), _eswdData.wsToText );
			}
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_OPTIONS_ALIGN_COMBO ));
		if ( pcbCombo ) {
			_eswdData.wsAlignmentText = pcbCombo->GetTextW();
			if ( _eswdData.wsAlignmentText.size() ) {
				auto aFound = std::find( _eswdData.vAlignmentHistory.begin(), _eswdData.vAlignmentHistory.end(), _eswdData.wsAlignmentText );
				if ( aFound != _eswdData.vAlignmentHistory.end() ) {
					_eswdData.vAlignmentHistory.erase( aFound );
				}
				_eswdData.vAlignmentHistory.insert( _eswdData.vAlignmentHistory.begin(), _eswdData.wsAlignmentText );
			}
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_COMBO ));
		if ( pcbCombo ) {
			_eswdData.wsFixedSizeText = pcbCombo->GetTextW();
			if ( _eswdData.wsFixedSizeText.size() ) {
				auto aFound = std::find( _eswdData.vFixedSizeHistory.begin(), _eswdData.vFixedSizeHistory.end(), _eswdData.wsFixedSizeText );
				if ( aFound != _eswdData.vFixedSizeHistory.end() ) {
					_eswdData.vFixedSizeHistory.erase( aFound );
				}
				_eswdData.vFixedSizeHistory.insert( _eswdData.vFixedSizeHistory.begin(), _eswdData.wsFixedSizeText );
			}
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_DUMMY_COMBO ));
		if ( pcbCombo ) {
			_eswdData.wsDummyText = pcbCombo->GetTextW();
			if ( _eswdData.wsDummyText.size() ) {
				auto aFound = std::find( _eswdData.vDummyHistory.begin(), _eswdData.vDummyHistory.end(), _eswdData.wsDummyText );
				if ( aFound != _eswdData.vDummyHistory.end() ) {
					_eswdData.vDummyHistory.erase( aFound );
				}
				_eswdData.vDummyHistory.insert( _eswdData.vDummyHistory.begin(), _eswdData.wsDummyText );
			}
		}



		CComboBox * pcbQqCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_COMBO ));
		_eswdData.esqmEvalType = pcbQqCombo ? static_cast<CUtilities::MX_EXP_SEARCH_QQ_MEANING>(pcbQqCombo->GetCurSelItemData()) : CUtilities::MX_ESQM_ADDRESS;

		CComboBox * pcbQqTypeCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_TYPE_COMBO ));
		_eswdData.dtDataType = pcbQqTypeCombo ? static_cast<CUtilities::MX_DATA_TYPES>(pcbQqTypeCombo->GetCurSelItemData()) : CUtilities::MX_DT_UINT32;

		_eswdData.ssExpression = FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_EDIT )->GetTextA();

		CRadioButton * prbDynRadio = static_cast<CRadioButton *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_DYN_SIZE_RADIO ));
		_eswdData.bDynamic = prbDynRadio && prbDynRadio->IsChecked();

	}

	// Validates the dialog box.
	bool CNewExpressionSearchWindow::DialogContentsAreValid( CSecureWString &_wsError, CSearcher::MX_SEARCH_PARMS &_spParmsIfValid ) {
		_spParmsIfValid.stType = CUtilities::MX_ST_EXP_SEARCH;
		_spParmsIfValid.pbAbort = nullptr;
		_spParmsIfValid.iThreadPriority = THREAD_PRIORITY_NORMAL;

		ee::CExpEvalContainer::EE_RESULT rRes;
		CComboBox * pcbAlignCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_OPTIONS_ALIGN_COMBO ));
		if ( !pcbAlignCombo || !pcbAlignCombo->GetTextAsUInt64Expression( rRes ) ) {
			_wsError = _DEC_WS_6D4F0349_Invalid_alignment_;
			return false;
		}
		_spParmsIfValid.bAligned = true;
		_spParmsIfValid.ui32Alignment = static_cast<uint64_t>(rRes.u.ui64Val);
		if ( _spParmsIfValid.ui32Alignment < 1 ) {
			_wsError = _DEC_WS_6D4F0349_Invalid_alignment_;
			return false;
		}


		// Starting address.
		BOOL bIsValid;
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_FROM_COMBO ));
		if ( !pcbCombo || !pcbCombo->GetTextAsExpression( rRes, &bIsValid ) ) {
			_wsError = mx::CStringDecoder::DecodeToWString( _T_LEN_F94C43F0_The_specified_search_range_is_invalid_ );
			return false;
		}
		_spParmsIfValid.ui64AddressFrom = CUtilities::ExpEvalResultToDataType( rRes, CUtilities::MX_DT_UINT64 ).u.UInt64;
		// Ending search address.
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_TO_COMBO ));
		if ( !pcbCombo || !pcbCombo->GetTextAsExpression( rRes, &bIsValid ) ) {
			_wsError = mx::CStringDecoder::DecodeToWString( _T_LEN_F94C43F0_The_specified_search_range_is_invalid_ );
			return false;
		}
		_spParmsIfValid.ui64AddressTo = CUtilities::ExpEvalResultToDataType( rRes, CUtilities::MX_DT_UINT64 ).u.UInt64;



		CComboBox * pcbQqCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_COMBO ));
		_spParmsIfValid.esqmExpSearchType = pcbQqCombo ? static_cast<CUtilities::MX_EXP_SEARCH_QQ_MEANING>(pcbQqCombo->GetCurSelItemData()) : CUtilities::MX_ESQM_ADDRESS;
		if ( _spParmsIfValid.esqmExpSearchType == CUtilities::MX_ESQM_ADDRESS ) {
			// Can be fixed size or dynamic.
			CRadioButton * prbDynRadio = static_cast<CRadioButton *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_DYN_SIZE_RADIO ));
			bool bDynamic = prbDynRadio && prbDynRadio->IsChecked();
			if ( !bDynamic ) {
				pcbCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_COMBO ));
				if ( !pcbCombo || !pcbCombo->GetTextAsExpression( rRes, &bIsValid ) ) {
					_wsError = _DEC_WS_7B4DE404_The_specified_fixed_size_is_invalid_;
					return false;
				}
				_spParmsIfValid.ui32ExpSearchSize = static_cast<uint64_t>(rRes.u.ui64Val);
			}
			else {
				_spParmsIfValid.ui32ExpSearchSize = 0;
			}
		}
		else {
			// Size comes from type.
			CComboBox * pcbQqTypeCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_TYPE_COMBO ));
			if ( !pcbQqTypeCombo ) {
				_wsError = _DEC_WS_2AEF955A_Failed_to_find_data_type_control_;
				return false;
			}
			_spParmsIfValid.dtLVal.dtType = pcbQqTypeCombo ? static_cast<CUtilities::MX_DATA_TYPES>(pcbQqTypeCombo->GetCurSelItemData()) : CUtilities::MX_DT_UINT32;
			_spParmsIfValid.ui32ExpSearchSize = CUtilities::DataTypeSize( _spParmsIfValid.dtLVal.dtType );
		}



		CSecureWString wFinalEvaluation;
		CSecureWString wResult;
		bool bExpIsValid = VerifyExpression( _spParmsIfValid.ssExpression, wResult, wFinalEvaluation, _spParmsIfValid.esqmExpSearchType, _spParmsIfValid.dtLVal.dtType );
		if ( !bExpIsValid ) {
			if ( !wFinalEvaluation.size() ) {
				_wsError = _DEC_WS_69153D9B_No_expression_has_been_entered_;
			}
			else {
				_wsError = _DEC_WS_097E0A48_Expression_Error__ + wFinalEvaluation;
			}
			return false;
		}
		

		return true;
	}

	// Updates the dialog.
	void CNewExpressionSearchWindow::UpdateDialog() {
		CComboBox * pcbDummyCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_DUMMY_COMBO ));
		if ( pcbDummyCombo && !pcbDummyCombo->GetTextW().size() ) {
			pcbDummyCombo->SetTextA( "0" );
		}
		CComboBox * pcbFixedSizeCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_COMBO ));
		if ( pcbFixedSizeCombo && !pcbFixedSizeCombo->GetTextW().size() ) {
			pcbFixedSizeCombo->SetTextA( "4" );
		}
		CComboBox * pcbAlignCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_OPTIONS_ALIGN_COMBO ));
		if ( pcbAlignCombo && !pcbAlignCombo->GetTextW().size() ) {
			pcbAlignCombo->SetTextA( "4" );
		}


		CStatic * plLabel = static_cast<CStatic *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_SIZE_LABEL ));
		CRadioButton * prbDynRadio = static_cast<CRadioButton *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_DYN_SIZE_RADIO ));
		bool bDynamic = prbDynRadio && prbDynRadio->IsChecked();
		if ( plLabel ) {
			CSecureWString wText;
			if ( bDynamic ) {
				wText = _DEC_WS_8D0B139B_Expression_returns_a_positive_value_to_indicate_the_data_size_;
			}
			else {
				wText = _DEC_WS_5E160389_Searches_for_data_known_to_be_of_a_given_size__Subsearches_support____previous_value____;
			}
			plLabel->SetTextW( wText.c_str() );
		}

		CComboBox * pcbQqCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_COMBO ));
		CUtilities::MX_EXP_SEARCH_QQ_MEANING esqmEvalType = pcbQqCombo ? static_cast<CUtilities::MX_EXP_SEARCH_QQ_MEANING>(pcbQqCombo->GetCurSelItemData()) : CUtilities::MX_ESQM_ADDRESS;

		


		CRadioButton * prbFixedRadio = static_cast<CRadioButton *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_RADIO ));
		CComboBox * pcbSizeCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_COMBO ));
		CStatic * plBytesLabel = static_cast<CStatic *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_LABEL ));
		CStatic * plHelpLabel = static_cast<CStatic *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_SIZE_LABEL ));
		if ( prbDynRadio ) { prbDynRadio->SetEnabled( esqmEvalType == CUtilities::MX_ESQM_ADDRESS ); }
		if ( prbFixedRadio ) { prbFixedRadio->SetEnabled( esqmEvalType == CUtilities::MX_ESQM_ADDRESS ); }
		if ( pcbSizeCombo ) { pcbSizeCombo->SetEnabled( esqmEvalType == CUtilities::MX_ESQM_ADDRESS && !bDynamic ); }
		if ( plBytesLabel ) { plBytesLabel->SetEnabled( esqmEvalType == CUtilities::MX_ESQM_ADDRESS && !bDynamic ); }
		if ( plHelpLabel ) { plHelpLabel->SetEnabled( esqmEvalType == CUtilities::MX_ESQM_ADDRESS ); }

		CComboBox * pcbQqTypeCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_TYPE_COMBO ));
		if ( pcbQqTypeCombo ) { pcbQqTypeCombo->SetEnabled( esqmEvalType == CUtilities::MX_ESQM_VALUE ); }
		CUtilities::MX_DATA_TYPES dtDataType = pcbQqTypeCombo ? static_cast<CUtilities::MX_DATA_TYPES>(pcbQqTypeCombo->GetCurSelItemData()) : CUtilities::MX_DT_UINT32;


		CStatic * plDescLabel = static_cast<CStatic *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SETTINGS_DESC_LABEL ));
		if ( plDescLabel ) {
			plDescLabel->SetTextW( esqmEvalType == CUtilities::MX_ESQM_ADDRESS ? _DEC_WS_D54B3E50__________will_be_the_address_currently_being_searched___The_dereference_operator_______address_______can_be_used_to_access_this_and_neighboring_addresses__u32____4____u32_____1_.c_str() :
				_DEC_WS_B985D578__________will_be_the_value_at_the_address_being_searched___The_search_can_be_a_bit_faster_but_surrounding_values_cannot_be_examined________32____________89.c_str() );
		}


		CSecureWString wFinalEvaluation;
#if 0
		if ( pcbDummyCombo ) {
			ee::CExpEvalContainer::EE_RESULT rDummy;
			BOOL bIsValid;
			pcbDummyCombo->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
			BOOL bParsed = pcbDummyCombo->GetTextAsExpression( rDummy, &bIsValid );
			if ( !bIsValid ) {
				wFinalEvaluation = _DEC_WS_32F9AB23_Invalid____Value;
			}
			else if ( !bParsed ) {
				wFinalEvaluation = _DEC_WS_275B53EA____Value_Unresolvable;
			}
			else {
				if ( esqmEvalType == CUtilities::MX_ESQM_ADDRESS ) {
					rDummy = ee::CExpEvalContainer::ConvertResult( rDummy, ee::EE_NC_UNSIGNED );
				}
				else {
					switch ( dtDataType ) {
#define MX_CHECK( CASE, BROAD_CAST, MEMBER, CAST )										\
	case CUtilities::CASE : {															\
		rDummy = ee::CExpEvalContainer::ConvertResult( rDummy, ee::BROAD_CAST );		\
		rDummy.u.MEMBER = static_cast<CAST>(rDummy.u.MEMBER);							\
		break;																			\
	}

						MX_CHECK( MX_DT_INT8, EE_NC_SIGNED, i64Val, int8_t )
						MX_CHECK( MX_DT_INT16, EE_NC_SIGNED, i64Val, int16_t )
						MX_CHECK( MX_DT_INT32, EE_NC_SIGNED, i64Val, int32_t )
						MX_CHECK( MX_DT_INT64, EE_NC_SIGNED, i64Val, int64_t )
						MX_CHECK( MX_DT_UINT8, EE_NC_UNSIGNED, ui64Val, uint8_t )
						MX_CHECK( MX_DT_UINT16, EE_NC_UNSIGNED, ui64Val, uint16_t )
						MX_CHECK( MX_DT_UINT32, EE_NC_UNSIGNED, ui64Val, uint32_t )
						MX_CHECK( MX_DT_UINT64, EE_NC_UNSIGNED, ui64Val, uint64_t )
						MX_CHECK( MX_DT_FLOAT, EE_NC_FLOATING, dVal, float )
						MX_CHECK( MX_DT_DOUBLE, EE_NC_FLOATING, dVal, double )
						case CUtilities::MX_DT_FLOAT16 : {
							rDummy = ee::CExpEvalContainer::ConvertResult( rDummy, ee::EE_NC_FLOATING );
							rDummy.u.dVal = CFloat16( rDummy.u.dVal ).Value();
							break;
						}
						default : {
							wFinalEvaluation = _DEC_WS_1468A1DF_Internal_error_;
						}
#undef MX_CHECK
					}
				}

				CEdit * peExpEdit = static_cast<CEdit *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_EDIT ));
				CExpression eExp;
				std::string sText = peExpEdit->GetTextA();
				if ( sText.size() != 0 ) {
					if ( !eExp.SetExpression( sText.c_str() ) ) { wFinalEvaluation = _DEC_WS_3424431C_Invalid; }
					else {
						if ( !eExp.GetContainer() ) { wFinalEvaluation = _DEC_WS_3424431C_Invalid; }
						else {
							eExp.GetContainer()->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
							eExp.GetContainer()->SetUserHandler( UserVarHandler, reinterpret_cast<uintptr_t>(&rDummy) );
							ee::CExpEvalContainer::EE_RESULT rRes;
							if ( !eExp.GetContainer()->Resolve( rRes ) ) { wFinalEvaluation = _DEC_WS_3C583DF0_Address_rejected_due_to_being_unresolvable_; }
							else {
								if ( !eExp.GetContainer()->HasAccessToUserData() ) { wFinalEvaluation = _DEC_WS_3CF0B31C_Expression_Must_Use___; }
								else {
									CSecureWString wResult;
									CUtilities::PrintExpResult( rRes, wResult, 0, CUtilities::MX_D_STANDARD );
									CEdit * peResEdit = static_cast<CEdit *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_RESULT_EDIT ));
									if ( peResEdit ) {
										peResEdit->SetTextW( wResult.c_str() );
									}
									bool bAccept = false;
									size_t sSize = 0;
									switch ( rRes.ncType ) {
										case ee::EE_NC_SIGNED : {
											if ( esqmEvalType == CUtilities::MX_ESQM_ADDRESS ) {
												if ( prbDynRadio && prbDynRadio->IsChecked() ) {
													if ( rRes.u.i64Val ) {
														bAccept = true;
														sSize = static_cast<size_t>(rRes.u.i64Val);
													}
												}
												else {
													ee::CExpEvalContainer::EE_RESULT rSize;
													CComboBox * pcbSizeCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_COMBO ));
													pcbSizeCombo->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
													bParsed = pcbSizeCombo->GetTextAsExpression( rSize, &bIsValid );
													if ( !bIsValid ) {
														wFinalEvaluation = _DEC_WS_32F9AB23_Invalid____Value;
													}
													else if ( !bParsed ) {
														wFinalEvaluation = _DEC_WS_275B53EA____Value_Unresolvable;
													}
													else {
														bAccept = true;
														sSize = static_cast<size_t>(ee::CExpEvalContainer::ConvertResult( rSize, ee::EE_NC_UNSIGNED ).u.ui64Val);
													}
												}
											}
											else {
												if ( rRes.u.i64Val ) {
													bAccept = true;
													sSize = static_cast<size_t>(CUtilities::DataTypeSize( dtDataType ));
												}
											}
											break;
										}
										case ee::EE_NC_UNSIGNED : {
											if ( esqmEvalType == CUtilities::MX_ESQM_ADDRESS ) {
												if ( prbDynRadio && prbDynRadio->IsChecked() ) {
													if ( rRes.u.ui64Val ) {
														bAccept = true;
														sSize = static_cast<size_t>(rRes.u.ui64Val);
													}
												}
												else {
													ee::CExpEvalContainer::EE_RESULT rSize;
													CComboBox * pcbSizeCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_COMBO ));
													pcbSizeCombo->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
													bParsed = pcbSizeCombo->GetTextAsExpression( rSize, &bIsValid );
													if ( !bIsValid ) {
														wFinalEvaluation = _DEC_WS_32F9AB23_Invalid____Value;
													}
													else if ( !bParsed ) {
														wFinalEvaluation = _DEC_WS_275B53EA____Value_Unresolvable;
													}
													else {
														bAccept = true;
														sSize = static_cast<size_t>(ee::CExpEvalContainer::ConvertResult( rSize, ee::EE_NC_UNSIGNED ).u.ui64Val);
													}
												}
											}
											else {
												if ( rRes.u.ui64Val ) {
													bAccept = true;
													sSize = static_cast<size_t>(CUtilities::DataTypeSize( dtDataType ));
												}
											}
											break;
										}
										case ee::EE_NC_FLOATING : {
											if ( esqmEvalType == CUtilities::MX_ESQM_ADDRESS ) {
												if ( prbDynRadio && prbDynRadio->IsChecked() ) {
													if ( rRes.u.dVal ) {
														bAccept = true;
														sSize = static_cast<size_t>(rRes.u.dVal);
													}
												}
												else {
													ee::CExpEvalContainer::EE_RESULT rSize;
													CComboBox * pcbSizeCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_COMBO ));
													pcbSizeCombo->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
													bParsed = pcbSizeCombo->GetTextAsExpression( rSize, &bIsValid );
													if ( !bIsValid ) {
														wFinalEvaluation = _DEC_WS_32F9AB23_Invalid____Value;
													}
													else if ( !bParsed ) {
														wFinalEvaluation = _DEC_WS_275B53EA____Value_Unresolvable;
													}
													else {
														bAccept = true;
														sSize = static_cast<size_t>(ee::CExpEvalContainer::ConvertResult( rSize, ee::EE_NC_UNSIGNED ).u.ui64Val);
													}
												}
											}
											else {
												if ( static_cast<size_t>(rRes.u.dVal) ) {
													bAccept = true;
													sSize = static_cast<size_t>(CUtilities::DataTypeSize( dtDataType ));
												}
												
											}
											break;
										}
									}

									CStatic * plFinalDescLabel = static_cast<CStatic *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_RESULT_HELP_LABEL ));
									if ( plFinalDescLabel ) {
										if ( !bAccept ) {
											wFinalEvaluation = _DEC_WS_F55FD24A_Address_is_rejected_;
										}
										else if ( sSize > 0 ) {
											WCHAR szBuffer[_LEN_76C91756+16];
											::swprintf_s( szBuffer, _DEC_WS_76C91756__u_byte_s_matched_at_current_address_.c_str(), static_cast<uint32_t>(sSize),
												static_cast<uint32_t>(sSize) != 1 ? L"s" : L"" );
											wFinalEvaluation = szBuffer;
											::ZeroMemory( szBuffer, sizeof( szBuffer ) );
										}
										else {
											if ( !wFinalEvaluation.size() ) {
												wFinalEvaluation = _DEC_WS_75BC43F0_Invalid_Size_;
											}
										}
									}
									else { wFinalEvaluation = _DEC_WS_1468A1DF_Internal_error_; }
								}
							}
						}
					}
				}
				else {
					//wFinalEvaluation = L"";
					// Leave blank.
				}

				 /*CExpression eExp;
		std::string sText = GetTextA();
		if ( sText.size() == 0 ) { return FALSE; }
		if ( !eExp.SetExpression( sText.c_str() ) ) { return FALSE; }
		if ( !eExp.GetContainer() ) { return FALSE; }
		eExp.GetContainer()->SetAddressHandler( m_pfahAddressHandler, m_uiptrAddressHandlerData );
		if ( _pbExpIsValid ) { (*_pbExpIsValid) = TRUE; }
		
		if ( !eExp.GetContainer()->Resolve( _eResult ) ) { return FALSE; }*/
			}
		}
		else {
			wFinalEvaluation = _DEC_WS_1468A1DF_Internal_error_;
		}
#else
		CSecureString ssExp;
		CSecureWString wResult;
		bool bExpIsValid = VerifyExpression( ssExp, wResult, wFinalEvaluation, esqmEvalType, dtDataType );
		CEdit * peResEdit = static_cast<CEdit *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_RESULT_EDIT ));
		if ( peResEdit ) {
			peResEdit->SetTextW( wResult.c_str() );
		}
#endif
		plDescLabel = static_cast<CStatic *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_RESULT_HELP_LABEL ));
		if ( plDescLabel ) {
			plDescLabel->SetTextW( wFinalEvaluation.c_str() );
		}
	}

	// Verifies that the expression is valid and returns descriptive texts.
	bool CNewExpressionSearchWindow::VerifyExpression( CSecureString &_sExpression, CSecureWString &_wsResult, CSecureWString &_wsHelp, CUtilities::MX_EXP_SEARCH_QQ_MEANING _esqmEvalType, CUtilities::MX_DATA_TYPES _dtDataType ) {
		CComboBox * pcbDummyCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_DUMMY_COMBO ));
		CRadioButton * prbDynRadio = static_cast<CRadioButton *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_DYN_SIZE_RADIO ));
		if ( !pcbDummyCombo || !prbDynRadio ) {
			_wsHelp = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}


		ee::CExpEvalContainer::EE_RESULT rDummy;
		BOOL bIsValid;
		pcbDummyCombo->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
		BOOL bParsed = pcbDummyCombo->GetTextAsExpression( rDummy, &bIsValid );
		if ( !bIsValid ) {
			_wsHelp = _DEC_WS_32F9AB23_Invalid____Value;
		}
		else if ( !bParsed ) {
			_wsHelp = _DEC_WS_275B53EA____Value_Unresolvable;
		}
		else {
			if ( _esqmEvalType == CUtilities::MX_ESQM_ADDRESS ) {
				rDummy = ee::CExpEvalContainer::ConvertResult( rDummy, ee::EE_NC_UNSIGNED );
			}
			else {
				switch ( _dtDataType ) {
#define MX_CHECK( CASE, BROAD_CAST, MEMBER, CAST )									\
case CUtilities::CASE : {															\
	rDummy = ee::CExpEvalContainer::ConvertResult( rDummy, ee::BROAD_CAST );		\
	rDummy.u.MEMBER = static_cast<CAST>(rDummy.u.MEMBER);							\
	break;																			\
}

					MX_CHECK( MX_DT_INT8, EE_NC_SIGNED, i64Val, int8_t )
					MX_CHECK( MX_DT_INT16, EE_NC_SIGNED, i64Val, int16_t )
					MX_CHECK( MX_DT_INT32, EE_NC_SIGNED, i64Val, int32_t )
					MX_CHECK( MX_DT_INT64, EE_NC_SIGNED, i64Val, int64_t )
					MX_CHECK( MX_DT_UINT8, EE_NC_UNSIGNED, ui64Val, uint8_t )
					MX_CHECK( MX_DT_UINT16, EE_NC_UNSIGNED, ui64Val, uint16_t )
					MX_CHECK( MX_DT_UINT32, EE_NC_UNSIGNED, ui64Val, uint32_t )
					MX_CHECK( MX_DT_UINT64, EE_NC_UNSIGNED, ui64Val, uint64_t )
					MX_CHECK( MX_DT_FLOAT, EE_NC_FLOATING, dVal, float )
					MX_CHECK( MX_DT_DOUBLE, EE_NC_FLOATING, dVal, double )
					case CUtilities::MX_DT_FLOAT16 : {
						rDummy = ee::CExpEvalContainer::ConvertResult( rDummy, ee::EE_NC_FLOATING );
						rDummy.u.dVal = CFloat16( rDummy.u.dVal ).Value();
						break;
					}
					default : {
						_wsHelp = _DEC_WS_1468A1DF_Internal_error_;
					}
#undef MX_CHECK
				}
			}

			CEdit * peExpEdit = static_cast<CEdit *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_EDIT ));
			CExpression eExp;
			std::string sText = peExpEdit->GetTextA();
			if ( sText.size() != 0 ) {
				if ( !eExp.SetExpression( sText.c_str() ) ) { _wsHelp = _DEC_WS_3424431C_Invalid; }
				else {
					if ( !eExp.GetContainer() ) { _wsHelp = _DEC_WS_3424431C_Invalid; }
					else {
						if ( !eExp.GetContainer()->HasAccessToUserData() ) { _wsHelp = _DEC_WS_3CF0B31C_Expression_Must_Use___; }
						else {
							if ( _esqmEvalType == CUtilities::MX_ESQM_ADDRESS && !eExp.GetContainer()->HasAddressRead() ) { _wsHelp = _DEC_WS_C6E278CF_Expression_Must_Use_a_Dereference__i8__addr____u32__addr____f16__addr____etc__; }
							else {
								
								size_t sSize = 0;
								bool bSizeIsValid = true;
								if ( _esqmEvalType == CUtilities::MX_ESQM_ADDRESS ) {
									if ( !prbDynRadio->IsChecked() ) {
										ee::CExpEvalContainer::EE_RESULT rSize;
										CComboBox * pcbSizeCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SIZE_FIXED_SIZE_COMBO ));
										pcbSizeCombo->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
										bParsed = pcbSizeCombo->GetTextAsExpression( rSize, &bIsValid );
										if ( !bIsValid ) {
											_wsHelp = _DEC_WS_32F9AB23_Invalid____Value;
											bSizeIsValid = false;
										}
										else if ( !bParsed ) {
											_wsHelp = _DEC_WS_275B53EA____Value_Unresolvable;
											bSizeIsValid = false;
										}
										else {
											bSizeIsValid = true;
											sSize = static_cast<size_t>(ee::CExpEvalContainer::ConvertResult( rSize, ee::EE_NC_UNSIGNED ).u.ui64Val);
										}
									}
								}
								if ( bSizeIsValid ) {
									ee::CExpEvalContainer::EE_RESULT rRes;
									eExp.GetContainer()->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
									eExp.GetContainer()->SetUserHandler( UserVarHandler, reinterpret_cast<uintptr_t>(&rDummy) );
									_sExpression = sText;
									if ( !eExp.GetContainer()->Resolve( rRes ) ) {
										_wsHelp = _DEC_WS_3C583DF0_Address_rejected_due_to_being_unresolvable_;
										_wsResult = _DEC_WS_3AF901F6_Valid_But_Unresolvable;
										return true;
										// _sExpression
									}
									else {
										CUtilities::PrintExpResult( rRes, _wsResult, 0, CUtilities::MX_D_STANDARD );
								
										bool bAccept = false;
									
										switch ( rRes.ncType ) {
											case ee::EE_NC_SIGNED : {
												if ( _esqmEvalType == CUtilities::MX_ESQM_ADDRESS ) {
													if ( prbDynRadio->IsChecked() ) {
														if ( rRes.u.i64Val > 0 ) {
															bAccept = true;
															sSize = static_cast<size_t>(rRes.u.i64Val);
														}
													}
													else {
														bAccept = rRes.u.i64Val > 0 ? true : false;
													}
												}
												else {
													if ( rRes.u.i64Val ) {
														bAccept = true;
														sSize = static_cast<size_t>(CUtilities::DataTypeSize( _dtDataType ));
													}
												}
												break;
											}
											case ee::EE_NC_UNSIGNED : {
												if ( _esqmEvalType == CUtilities::MX_ESQM_ADDRESS ) {
													if ( prbDynRadio->IsChecked() ) {
														if ( rRes.u.ui64Val > 0 ) {
															bAccept = true;
															sSize = static_cast<size_t>(rRes.u.ui64Val);
														}
													}
													else {
														bAccept = rRes.u.ui64Val > 0 ? true : false;
													}
												}
												else {
													if ( rRes.u.ui64Val ) {
														bAccept = true;
														sSize = static_cast<size_t>(CUtilities::DataTypeSize( _dtDataType ));
													}
												}
												break;
											}
											case ee::EE_NC_FLOATING : {
												if ( _esqmEvalType == CUtilities::MX_ESQM_ADDRESS ) {
													if ( prbDynRadio->IsChecked() ) {
														if ( rRes.u.dVal > 0.0 ) {
															bAccept = true;
															sSize = static_cast<size_t>(rRes.u.dVal);
														}
													}
													else {
														bAccept = rRes.u.dVal ? true : false;
													}
												}
												else {
													if ( rRes.u.dVal ) {
														bAccept = true;
														sSize = static_cast<size_t>(CUtilities::DataTypeSize( _dtDataType ));
													}
												}
												break;
											}
										}

										CStatic * plFinalDescLabel = static_cast<CStatic *>(FindChild( CNewExpressionSearchLayout::MX_NESI_EXPRESSION_RESULT_HELP_LABEL ));
										if ( plFinalDescLabel ) {
											if ( !bAccept ) {
												_wsHelp = _DEC_WS_F55FD24A_Address_is_rejected_;
												return true;
											}
											else if ( sSize > 0 ) {
												WCHAR szBuffer[_LEN_76C91756+16];
												::swprintf_s( szBuffer, _DEC_WS_76C91756__u_byte_s_matched_at_current_address_.c_str(), static_cast<uint32_t>(sSize),
													static_cast<uint32_t>(sSize) != 1 ? L"s" : L"" );
												_wsHelp = szBuffer;
												::ZeroMemory( szBuffer, sizeof( szBuffer ) );
												return true;
											}
											else {
												if ( !_wsHelp.size() ) {
													_wsHelp = _DEC_WS_75BC43F0_Invalid_Size_;
												}
												else { return true; }
											}
										}
										else { _wsHelp = _DEC_WS_1468A1DF_Internal_error_; }
									}
								}
							}
						}
					}
				}
			}
			else {
				//_wsHelp = L"";
				// Leave blank.
			}
		}
		return false;
	}

	// WM_CTLCOLORSTATIC.
	CWidget::LSW_HANDLED CNewExpressionSearchWindow::CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush ) {
		if ( _pwControl && _pwControl->Id() == CNewExpressionSearchLayout::MX_NESI_EXPRESSION_RESULT_EDIT ) {
			CComboBox * pcbQqCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_COMBO ));
			CUtilities::MX_EXP_SEARCH_QQ_MEANING esqmEvalType = pcbQqCombo ? static_cast<CUtilities::MX_EXP_SEARCH_QQ_MEANING>(pcbQqCombo->GetCurSelItemData()) : CUtilities::MX_ESQM_ADDRESS;
			CComboBox * pcbQqTypeCombo = static_cast<CComboBox *>(FindChild( CNewExpressionSearchLayout::MX_NESI_SETTINGS_QQ_TYPE_COMBO ));
			CUtilities::MX_DATA_TYPES dtDataType = pcbQqTypeCombo ? static_cast<CUtilities::MX_DATA_TYPES>(pcbQqTypeCombo->GetCurSelItemData()) : CUtilities::MX_DT_UINT32;
			CSecureWString wFinalEvaluation;
			CSecureString ssExp;
			CSecureWString wResult;
			bool bExpIsValid = VerifyExpression( ssExp, wResult, wFinalEvaluation, esqmEvalType, dtDataType );
			if ( bExpIsValid ) {
				_hBrush = ::GetSysColorBrush( COLOR_HIGHLIGHT );
				return LSW_H_HANDLED;
			}
		}
		return LSW_H_CONTINUE;
	}

	// User-variable function handler.
	bool __stdcall CNewExpressionSearchWindow::UserVarHandler( uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		::memcpy( &_rResult, reinterpret_cast<const void *>(_uiptrData), sizeof( _rResult ) );
		return true;
	}

}	// namespace mx

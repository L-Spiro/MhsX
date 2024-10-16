#include "MXNewDataTypeSearchWindow.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXNewDataTypeSearchLayout.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"

#include <algorithm>
#include <Base/LSWBase.h>
#include <Button/LSWButton.h>
#include <CheckButton/LSWCheckButton.h>
#include <ComboBox/LSWComboBox.h>
#include <Object/EEObject.h>
#include <Static/LSWStatic.h>


namespace mx {

	// == Members.
	CNewDataTypeSearchWindow::MX_SEARCH_WIN_DATA CNewDataTypeSearchWindow::m_swdData = {
		CUtilities::MX_DT_UINT32,				// uiDataType
		CUtilities::MX_ET_EXACT,				// uiEvalType
	};

	CNewDataTypeSearchWindow::CNewDataTypeSearchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmhMemHack( reinterpret_cast<CMemHack *>(_ui64Data) ),
		m_bLastTypeExpressionWasEmpty( true ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CNewDataTypeSearchWindow::InitDialog() {
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_DATA_TYPE_COMBO ));
		
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

			pcbCombo->SetCurSelByItemData( m_swdData.uiDataType );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EVALUATION_TYPE_COMBO ));
		if ( pcbCombo ) {
			static const CUtilities::MX_EVAL_TYPES stTypes[] = {
				CUtilities::MX_ET_EXACT,
				CUtilities::MX_ET_GREATER_THAN,
				CUtilities::MX_ET_LESS_THAN,
				CUtilities::MX_ET_RANGE,
				CUtilities::MX_ET_UNKNOWN,
				CUtilities::MX_ET_QUICK_EXP,
			};
			for ( size_t I = 0; I < MX_ELEMENTS( stTypes ); ++I ) {
				CSecureWString sTemp;
				CUtilities::EvaluationTypeToString( stTypes[I], sTemp );
				INT iIndex = pcbCombo->AddString( sTemp.c_str() );
				pcbCombo->SetItemData( iIndex, stTypes[I] );
			}
			pcbCombo->AutoSetMinListWidth();

			pcbCombo->SetCurSelByItemData( m_swdData.uiEvalType );
		}
		
		if ( m_pmhMemHack ) {
			CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_ALIGNED ));
			if ( pcbCheck ) {
				pcbCheck->CheckButton( m_pmhMemHack->Options().bAligned ? BST_CHECKED : BST_UNCHECKED );
			}

			pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_SAO_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->CheckButton( m_pmhMemHack->Options().bSameAsOriginal ? BST_CHECKED : BST_UNCHECKED );
			}


			pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_FROM_COMBO ));
			if ( pcbCombo ) {
				for ( size_t I = 0; I < m_pmhMemHack->Options().vFromHistory.size(); ++I ) {
					pcbCombo->AddString( m_pmhMemHack->Options().vFromHistory[I].c_str() );
				}
				if ( m_pmhMemHack->Options().wsFromText.size() == 0 ) {
					uintptr_t uiptrTemp = reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMinimumApplicationAddress);
					std::string sTemp = CUtilities::ToHex( uiptrTemp, 4 );
					pcbCombo->SetTextA( sTemp.c_str() );
				}
				else {
					pcbCombo->SetTextW( m_pmhMemHack->Options().wsFromText.c_str() );
				}
				pcbCombo->SetTreatAsHex( TRUE );
				pcbCombo->SetFocus();

				pcbCombo->AutoSetMinListWidth();
			}

			pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_TO_COMBO ));
			if ( pcbCombo ) {
				for ( size_t I = 0; I < m_pmhMemHack->Options().vToHistory.size(); ++I ) {
					pcbCombo->AddString( m_pmhMemHack->Options().vToHistory[I].c_str() );
				}
				if ( m_pmhMemHack->Options().wsToText.size() == 0 ) {
					uintptr_t uiptrTemp = reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMaximumApplicationAddress);
					std::string sTemp = CUtilities::ToHex( uiptrTemp + 1ULL, 4 );
					pcbCombo->SetTextA( sTemp.c_str() );
				}
				else {
					pcbCombo->SetTextW( m_pmhMemHack->Options().wsToText.c_str() );
				}
				pcbCombo->SetTreatAsHex( TRUE );
				pcbCombo->SetFocus();

				pcbCombo->AutoSetMinListWidth();
			}
		}

		CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_INVERT ));
		if ( pcbCheck ) {
			pcbCheck->CheckButton( m_swdData.bInvertResults ? BST_CHECKED : BST_UNCHECKED );
		}
		
		
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_LVAL_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vLValHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vLValHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsLValText.c_str() );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_RVAL_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vRValHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vRValHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsRValText.c_str() );
			//pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EXP_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vExpHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vExpHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsExpText.c_str() );
			//pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		


		CButton * pbButton = static_cast<CButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_GENERAL_SEARCH_OPTIONS_BUTTON ));
		if ( pbButton ) {
			pbButton->SetEnabled( m_pmhMemHack != nullptr );
		}

		UpdateDialog();
		return CMainWindow::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CNewDataTypeSearchWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CNewDataTypeSearchLayout::MX_NDSI_CANCEL : {
				return Close();
			}
			case CNewDataTypeSearchLayout::MX_NDSI_OK : {
				CSecureWString wsError;
				CSearcher::MX_SEARCH_PARMS spParms;
				if ( !DialogContentsAreValid( wsError, spParms ) ) {
					lsw::CBase::MessageBoxError( Wnd(), wsError.c_str() );
					break;
				}
				SaveDialogData( m_swdData );
				CMhsMainWindow * pmmwMain = static_cast<CMhsMainWindow *>(FindParent( mx::CMainWindowLayout::MX_MWI_MAINWINDOW ));
				if ( pmmwMain ) {
					pmmwMain->Search( &spParms, this, true, nullptr );
				}
				return Close();
			}
			case CNewDataTypeSearchLayout::MX_NDSI_GENERAL_SEARCH_OPTIONS_BUTTON : {
				CMhsMainWindow * pmmwMain = static_cast<CMhsMainWindow *>(FindParent( mx::CMainWindowLayout::MX_MWI_MAINWINDOW ));
				if ( pmmwMain ) {
					pmmwMain->ShowOptions( pmmwMain->SearchOptionsPage() );
					UpdateDialog();
				}
				break;
			}
			case CNewDataTypeSearchLayout::MX_NDSI_ALIGNED : {
				if ( _wCtrlCode == STN_CLICKED ) {
					UpdateDialog();
				}
				break;
			}
			case CNewDataTypeSearchLayout::MX_NDSI_INVERT : {
				if ( _wCtrlCode == STN_CLICKED ) {
					UpdateDialog();
				}
				break;
			}
			case CNewDataTypeSearchLayout::MX_NDSI_LVAL_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CNewDataTypeSearchLayout::MX_NDSI_RVAL_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CNewDataTypeSearchLayout::MX_NDSI_DATA_TYPE_COMBO : {
				if ( _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CNewDataTypeSearchLayout::MX_NDSI_EVALUATION_TYPE_COMBO : {
				if ( _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CNewDataTypeSearchLayout::MX_NDSI_EXP_COMBO : {
				CComboBox * pcbExpCombo = static_cast<CComboBox *>(FindChild( _Id ));
				if ( pcbExpCombo ) {
					std::string sText = pcbExpCombo->GetTextA();
					m_bLastTypeExpressionWasEmpty = sText.size() == 0;
				}
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CNewDataTypeSearchLayout::MX_NDSI_FROM_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CNewDataTypeSearchLayout::MX_NDSI_TO_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
		}
		return CMainWindow::Command( _wCtrlCode, _Id, _pwSrc );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CNewDataTypeSearchWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	// Saves the current dialog values to an MX_SEARCH_WIN_DATA structure.
	void CNewDataTypeSearchWindow::SaveDialogData( MX_SEARCH_WIN_DATA &_swdData ) {
		CComboBox * pcbCombo;
		if ( m_pmhMemHack ) {
			MX_OPTIONS oOpts = m_pmhMemHack->Options();
			CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_ALIGNED ));
			if ( pcbCheck ) {
				oOpts.bAligned = pcbCheck->IsChecked();
			}
			pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_SAO_CHECK ));
			if ( pcbCheck ) {
				oOpts.bSameAsOriginal = pcbCheck->IsChecked();
			}

			pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_FROM_COMBO ));
			if ( pcbCombo ) {
				oOpts.wsFromText = pcbCombo->GetTextW();
				if ( oOpts.wsFromText.size() ) {
					auto aFound = std::find( oOpts.vFromHistory.begin(), oOpts.vFromHistory.end(), oOpts.wsFromText );
					if ( aFound != oOpts.vFromHistory.end() ) {
						oOpts.vFromHistory.erase( aFound );
					}
					oOpts.vFromHistory.insert( oOpts.vFromHistory.begin(), oOpts.wsFromText );
				}
			}
			pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_TO_COMBO ));
			if ( pcbCombo ) {
				oOpts.wsToText = pcbCombo->GetTextW();
				if ( oOpts.wsToText.size() ) {
					auto aFound = std::find( oOpts.vToHistory.begin(), oOpts.vToHistory.end(), oOpts.wsToText );
					if ( aFound != oOpts.vToHistory.end() ) {
						oOpts.vToHistory.erase( aFound );
					}
					oOpts.vToHistory.insert( oOpts.vToHistory.begin(), oOpts.wsToText );
				}
			}

			m_pmhMemHack->SetOptions( oOpts );
		}

		CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_INVERT ));
		_swdData.bInvertResults = pcbCheck && pcbCheck->IsChecked();


		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_DATA_TYPE_COMBO ));
		if ( pcbCombo ) {
			_swdData.uiDataType = pcbCombo->GetItemData( pcbCombo->GetCurSel() );
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EVALUATION_TYPE_COMBO ));
		LPARAM lpEval = 0;
		if ( pcbCombo ) {
			lpEval = pcbCombo->GetCurSelItemData();
			_swdData.uiEvalType = pcbCombo->GetItemData( pcbCombo->GetCurSel() );
		}


		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_LVAL_COMBO ));
		if ( pcbCombo ) {
			_swdData.wsLValText = pcbCombo->GetTextW();
			if ( _swdData.wsLValText.size() ) {
				auto aFound = std::find( _swdData.vLValHistory.begin(), _swdData.vLValHistory.end(), _swdData.wsLValText );
				if ( aFound != _swdData.vLValHistory.end() ) {
					_swdData.vLValHistory.erase( aFound );
				}
				_swdData.vLValHistory.insert( _swdData.vLValHistory.begin(), _swdData.wsLValText );
			}
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_RVAL_COMBO ));
		if ( pcbCombo ) {
			_swdData.wsRValText = pcbCombo->GetTextW();
			if ( _swdData.wsRValText.size() ) {
				auto aFound = std::find( _swdData.vRValHistory.begin(), _swdData.vRValHistory.end(), _swdData.wsRValText );
				if ( aFound != _swdData.vRValHistory.end() ) {
					_swdData.vRValHistory.erase( aFound );
				}
				_swdData.vRValHistory.insert( _swdData.vRValHistory.begin(), _swdData.wsRValText );
			}
		}		
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EXP_COMBO ));
		if ( pcbCombo ) {
			_swdData.wsExpText = pcbCombo->GetTextW();
			if ( !m_bLastTypeExpressionWasEmpty || lpEval == CUtilities::MX_ET_QUICK_EXP ) {
				if ( _swdData.wsExpText.size() ) {
					auto aFound = std::find( _swdData.vExpHistory.begin(), _swdData.vExpHistory.end(), _swdData.wsExpText );
					if ( aFound != _swdData.vExpHistory.end() ) {
						_swdData.vExpHistory.erase( aFound );
					}
					_swdData.vExpHistory.insert( _swdData.vExpHistory.begin(), _swdData.wsExpText );
				}
			}
		}
		
	}

	// Validates the dialog box.
	bool CNewDataTypeSearchWindow::DialogContentsAreValid( CSecureWString &_wsError, CSearcher::MX_SEARCH_PARMS &_spParmsIfValid ) {
		_spParmsIfValid.stType = CUtilities::MX_ST_DATATYPE_SEARCH;
		_spParmsIfValid.pbAbort = nullptr;
		_spParmsIfValid.iThreadPriority = THREAD_PRIORITY_NORMAL;
		::ZeroMemory( &_spParmsIfValid.dtLVal, sizeof( _spParmsIfValid.dtLVal ) );
		::ZeroMemory( &_spParmsIfValid.dtRVal, sizeof( _spParmsIfValid.dtRVal ) );
		_spParmsIfValid.ssExpression.clear();



		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_DATA_TYPE_COMBO ));
		if ( !pcbCombo || pcbCombo->GetCurSel() == -1 ) {
			_wsError = mx::CStringDecoder::DecodeToWString( _T_LEN_7F534810_No_data_type_has_been_selected_ );
			return false;
		}
		CUtilities::MX_DATA_TYPES dtDataType = static_cast<CUtilities::MX_DATA_TYPES>(pcbCombo->GetCurSelItemData());
		_spParmsIfValid.dtLVal.dtType = dtDataType;

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EVALUATION_TYPE_COMBO ));
		if ( !pcbCombo || pcbCombo->GetCurSel() == -1 ) {
			_wsError = mx::CStringDecoder::DecodeToWString( _T_LEN_9A641D40_No_evaluation_type_has_been_selected_ );
			return false;
		}
		LRESULT lrEvalType = pcbCombo->GetCurSelItemData();
		_spParmsIfValid.i32EvalType = static_cast<int32_t>(lrEvalType);


		BOOL bIsValid;
		ee::CExpEvalContainer::EE_RESULT rRes;
		// Check for valid left value.
		switch ( lrEvalType ) {
			case CUtilities::MX_ET_EXACT : {}
			case CUtilities::MX_ET_GREATER_THAN : {}
			case CUtilities::MX_ET_LESS_THAN : {}
			case CUtilities::MX_ET_RANGE : {
				pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_LVAL_COMBO ));
				if ( !pcbCombo ) {
					_wsError = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
					return false;
				}
				
				BOOL bParsed = pcbCombo->GetTextAsExpression( rRes, ee::CObject::EE_TF_NONE, &bIsValid );
				if ( !bParsed ) {
					_wsError = bIsValid ? mx::CStringDecoder::DecodeToWString( _T_LEN_BDC62AA7_The_specified_value_is_invalid_ ) :
						mx::CStringDecoder::DecodeToWString( _T_LEN_3A9BBB6B_No_search_value_has_been_specified_ );
					return false;
				}
				_spParmsIfValid.dtLVal = CUtilities::ExpEvalResultToDataType( rRes, dtDataType );
				break;
			}
			case CUtilities::MX_ET_QUICK_EXP : {
				CSecureString ssExp;
				CSecureWString wResult;
				CSecureWString wFinalEvaluation;
				if ( !VerifyExpression( ssExp, wResult, wFinalEvaluation, dtDataType ) ) {
					_wsError = wFinalEvaluation;
					return false;
				}
				_spParmsIfValid.ssExpression = ssExp;
				break;
			}
		}


		// Check for valid right value.
		switch ( lrEvalType ) {
			case CUtilities::MX_ET_RANGE : {
				pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_RVAL_COMBO ));
				if ( !pcbCombo ) {
					_wsError = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
					return false;
				}
				BOOL bParsed = pcbCombo->GetTextAsExpression( rRes, ee::CObject::EE_TF_NONE, &bIsValid );
				if ( !bParsed ) {
					_wsError = bIsValid ? mx::CStringDecoder::DecodeToWString( _T_LEN_BDC62AA7_The_specified_value_is_invalid_ ) :
						mx::CStringDecoder::DecodeToWString( _T_LEN_14F91708_The_range_of_values_to_find_is_incomplete_ );
					return false;
				}
				_spParmsIfValid.dtRVal = CUtilities::ExpEvalResultToDataType( rRes, dtDataType );
			}
		}


		// Starting address.
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_FROM_COMBO ));
		if ( !pcbCombo || !pcbCombo->GetTextAsExpression( rRes, ee::CObject::EE_TF_NONE, &bIsValid ) ) {
			_wsError = mx::CStringDecoder::DecodeToWString( _T_LEN_F94C43F0_The_specified_search_range_is_invalid_ );
			return false;
		}
		_spParmsIfValid.ui64AddressFrom = CUtilities::ExpEvalResultToDataType( rRes, CUtilities::MX_DT_UINT64 ).u.UInt64;
		// Ending search address.
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_TO_COMBO ));
		if ( !pcbCombo || !pcbCombo->GetTextAsExpression( rRes, ee::CObject::EE_TF_NONE, &bIsValid ) ) {
			_wsError = mx::CStringDecoder::DecodeToWString( _T_LEN_F94C43F0_The_specified_search_range_is_invalid_ );
			return false;
		}
		_spParmsIfValid.ui64AddressTo = CUtilities::ExpEvalResultToDataType( rRes, CUtilities::MX_DT_UINT64 ).u.UInt64;


		CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_ALIGNED ));
		if ( pcbCheck ) {
			_spParmsIfValid.bAligned = pcbCheck->IsChecked();
		}
		else { _spParmsIfValid.bAligned = false; }

		pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_INVERT ));
		if ( pcbCheck ) {
			_spParmsIfValid.bInvertResults = pcbCheck->IsChecked() && lrEvalType != CUtilities::MX_ET_UNKNOWN;
		}
		else { _spParmsIfValid.bInvertResults = false; }

		return true;
	}

	// Verifies that the expression is valid and returns descriptive texts.
	bool CNewDataTypeSearchWindow::VerifyExpression( CSecureString &_sExpression, CSecureWString &_wsResult, CSecureWString &_wsHelp, CUtilities::MX_DATA_TYPES _dtDataType ) {
		BOOL bIsValid;
		{
			CComboBox * pcbExpCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EXP_COMBO ));
			CExpression eExp;
			std::string sText = pcbExpCombo->GetTextA();
			if ( sText.size() != 0 ) {
				if ( !eExp.SetExpression( sText.c_str() ) ) { _wsHelp = _DEC_WS_02BBA31E_Invalid_expression_; }
				else {
					if ( !eExp.GetContainer() ) { _wsHelp = _DEC_WS_02BBA31E_Invalid_expression_; }
					else {
						if ( !eExp.GetContainer()->HasAccessToUserData() ) { _wsHelp = _DEC_WS_3CF0B31C_Expression_Must_Use___; }
						else {
							{	
								size_t sSize = 0;
								bool bSizeIsValid = true;
								if ( bSizeIsValid ) {
									ee::CExpEvalContainer::EE_RESULT rRes, rDummy;
									rDummy.ncType = ee::EE_NC_UNSIGNED;
									rDummy.u.ui64Val = 0;
									eExp.GetContainer()->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
									eExp.GetContainer()->SetUserHandler( UserVarHandler, reinterpret_cast<uintptr_t>(&rDummy) );
									_sExpression = sText;
									if ( !eExp.GetContainer()->Resolve( rRes ) ) {
										return true;
										// _sExpression
									}
									else {
										return true;
#if 0
										CUtilities::PrintExpResult( rRes, _wsResult, 0, CUtilities::MX_D_STANDARD );
								
										bool bAccept = false;
									
										switch ( rRes.ncType ) {
											case ee::EE_NC_SIGNED : {
												{
													if ( rRes.u.i64Val ) {
														bAccept = true;
														sSize = static_cast<size_t>(CUtilities::DataTypeSize( _dtDataType ));
													}
												}
												break;
											}
											case ee::EE_NC_UNSIGNED : {
												{
													if ( rRes.u.ui64Val ) {
														bAccept = true;
														sSize = static_cast<size_t>(CUtilities::DataTypeSize( _dtDataType ));
													}
												}
												break;
											}
											case ee::EE_NC_FLOATING : {
												{
													if ( rRes.u.dVal ) {
														bAccept = true;
														sSize = static_cast<size_t>(CUtilities::DataTypeSize( _dtDataType ));
													}
												}
												break;
											}
										}
										CStatic * plFinalDescLabel = static_cast<CStatic *>(FindChild( CNewDataTypeSearchLayout::MX_NESI_EXPRESSION_RESULT_HELP_LABEL ));
										if ( plFinalDescLabel ) {
											if ( !bAccept ) {
												_wsHelp = _DEC_WS_F55FD24A_Address_is_rejected_;
												return true;
											}
											else if ( sSize > 0 ) {
												WCHAR szBuffer[_LEN_76C91756+16];
												std::swprintf( szBuffer, _DEC_WS_76C91756__u_byte_s_matched_at_current_address_.c_str(), static_cast<uint32_t>(sSize),
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
#endif
									}
								}
							}
						}
					}
				}
			}
			else {
				_wsHelp = _DEC_WS_02BBA31E_Invalid_expression_;
			}
		}
		return false;
	}

	// Updates the dialog.
	void CNewDataTypeSearchWindow::UpdateDialog() {
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EVALUATION_TYPE_COMBO ));
		
		CSecureWString wsTemp;
		if ( pcbCombo ) {
			LRESULT lrRes = pcbCombo->GetCurSelItemData();
			CWidget * pcbLabel = static_cast<CWidget *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_RVAL_LABEL ));
			if ( pcbLabel ) {				
				bool bEnabled = false;
				if ( lrRes == CUtilities::MX_ET_RANGE ) {
					wsTemp = mx::CStringDecoder::DecodeToWString( _T_B09DF1A4_To_, _LEN_B09DF1A4 );
					bEnabled = true;
				}
				pcbLabel->SetTextW( wsTemp.c_str() );

				CComboBox * pcbRValCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_RVAL_COMBO ));
				if ( pcbRValCombo ) {
					pcbRValCombo->SetEnabled( bEnabled );
				}
			}

			CComboBox * pcbLValCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_LVAL_COMBO ));
			if ( pcbLValCombo ) {
				pcbLValCombo->SetEnabled( lrRes != CUtilities::MX_ET_UNKNOWN && lrRes != CUtilities::MX_ET_QUICK_EXP );
				pcbLabel = static_cast<CWidget *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_LVAL_LABEL ));
				if ( pcbLabel ) {
					switch ( lrRes ) {
						case CUtilities::MX_ET_EXACT : {
							wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_8C044A93_Value_to_Find_ );
							break;
						}
						case CUtilities::MX_ET_GREATER_THAN : {
							wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_D7A9ADE0_Find_Values_Greater_Than_ );
							break;
						}
						case CUtilities::MX_ET_LESS_THAN : {
							wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_98033164_Find_Values_Lower_Than_ );
							break;
						}
						case CUtilities::MX_ET_RANGE : {
							wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_857372A6_From_ );
							break;
						}
						case CUtilities::MX_ET_UNKNOWN : {
							wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_00C08F50_Let_Me_Handle_This );
							break;
						}
					}
					pcbLabel ->SetTextW( wsTemp.c_str() );
				}
			}

			CComboBox * pcbLExpCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EXP_COMBO ));
			if ( pcbLExpCombo ) {
				pcbLExpCombo->SetEnabled( lrRes == CUtilities::MX_ET_QUICK_EXP );
			}

			CCheckButton * pcbCheck;
			pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_INVERT ));
			if ( pcbCheck ) {
				pcbCheck->SetEnabled( lrRes != CUtilities::MX_ET_UNKNOWN );
			}
		}


		// Update the description label.
		CStatic * plLabel = static_cast<CStatic *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_DESC_LABEL ));
		CCheckButton * pcbInvertCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_INVERT ));
		if ( plLabel ) {
			CUtilities::MX_DATA_TYPES dtDataType = CUtilities::MX_DT_VOID;
			CSecureWString wText;
			CComboBox * pcbEvalCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EVALUATION_TYPE_COMBO ));
			if ( pcbEvalCombo ) {
				LRESULT lrEvalType = pcbEvalCombo->GetCurSelItemData();
				CComboBox * pcbDataTypeCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_DATA_TYPE_COMBO ));
				if ( pcbDataTypeCombo ) {
					dtDataType = static_cast<CUtilities::MX_DATA_TYPES>(pcbDataTypeCombo->GetCurSelItemData());

					CComboBox * pcbLValCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_LVAL_COMBO ));
					INT iSel = pcbLValCombo->GetCurSel();
					CComboBox * pcbRValCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_RVAL_COMBO ));
					ee::CExpEvalContainer::EE_RESULT rRes, rResLow, rResHi;
					BOOL bIsValid;
					
					switch ( lrEvalType ) {
						case CUtilities::MX_ET_EXACT : {
							CSecureWString wsStr0, wsStr1;
							BOOL bParsed = pcbLValCombo->GetTextAsExpression( rRes, ee::CObject::EE_TF_NONE, &bIsValid );
							if ( !bIsValid ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
							}
							else if ( !bParsed ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_3A9BBB6B_No_search_value_has_been_specified_ );
							}
							else {
								CSecureWString wsNegation = pcbInvertCheck && pcbInvertCheck->IsChecked() ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
								wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_3048B35F_Find__s_values__sequal_to__s_ );
								CSecureWString wsLeft, wsRight;
								CUtilities::PrintDataType( wsLeft, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );

								CUtilities::ToDataTypeString( rRes, dtDataType, wsRight );
								WCHAR wszBuffer[512];
								std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsLeft.c_str(), wsNegation.c_str(), wsRight.c_str() );

								wText = wszBuffer;

								if ( m_bLastTypeExpressionWasEmpty ) {
									CComboBox * pcbExpCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EXP_COMBO ));
									if ( pcbExpCombo ) {
										CSecureString ssExact;
										CreateExactValueExpression( ssExact, rRes, dtDataType, pcbInvertCheck->IsChecked() );
										pcbExpCombo->SetTextA( ssExact.c_str() );
									}
								}
							}
							break;
						}
						case CUtilities::MX_ET_GREATER_THAN : {
							CSecureWString wsStr0, wsStr1;
							BOOL bParsed = pcbLValCombo->GetTextAsExpression( rRes, ee::CObject::EE_TF_NONE, &bIsValid );
							if ( !bIsValid ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
							}
							else if ( !bParsed ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_3A9BBB6B_No_search_value_has_been_specified_ );
							}
							else {
								CSecureWString wsNegation = pcbInvertCheck && pcbInvertCheck->IsChecked() ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
								wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_9E378ADB_Find__s_values__sgreater_than__s_ );
								CSecureWString wsLeft, wsRight;
								CUtilities::PrintDataType( wsLeft, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );

								CUtilities::ToDataTypeString( rRes, dtDataType, wsRight );
								WCHAR wszBuffer[512];
								std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsLeft.c_str(), wsNegation.c_str(), wsRight.c_str() );

								wText = wszBuffer;

								if ( m_bLastTypeExpressionWasEmpty ) {
									CComboBox * pcbExpCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EXP_COMBO ));
									if ( pcbExpCombo ) {
										CSecureString ssExact;
										CreateGreaterThanExpression( ssExact, rRes, dtDataType, pcbInvertCheck->IsChecked() );
										pcbExpCombo->SetTextA( ssExact.c_str() );
									}
								}
							}
							break;
						}
						case CUtilities::MX_ET_LESS_THAN : {
							CSecureWString wsStr0, wsStr1;
							BOOL bParsed = pcbLValCombo->GetTextAsExpression( rRes, ee::CObject::EE_TF_NONE, &bIsValid );
							if ( !bIsValid ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
							}
							else if ( !bParsed ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_3A9BBB6B_No_search_value_has_been_specified_ );
							}
							else {
								CSecureWString wsNegation = pcbInvertCheck && pcbInvertCheck->IsChecked() ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
								wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_88096E3E_Find__s_values__slower_than__s_ );
								CSecureWString wsLeft, wsRight;
								CUtilities::PrintDataType( wsLeft, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );

								CUtilities::ToDataTypeString( rRes, dtDataType, wsRight );
								WCHAR wszBuffer[512];
								std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsLeft.c_str(), wsNegation.c_str(), wsRight.c_str() );

								wText = wszBuffer;

								if ( m_bLastTypeExpressionWasEmpty ) {
									CComboBox * pcbExpCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EXP_COMBO ));
									if ( pcbExpCombo ) {
										CSecureString ssExact;
										CreateLessThanExpression( ssExact, rRes, dtDataType, pcbInvertCheck->IsChecked() );
										pcbExpCombo->SetTextA( ssExact.c_str() );
									}
								}
							}
							break;
						}
						case CUtilities::MX_ET_RANGE : {
							CSecureWString wsStr0, wsStr1, wsStr2;
							BOOL bParsed = pcbLValCombo->GetTextAsExpression( rRes, ee::CObject::EE_TF_NONE, &bIsValid );
							if ( !bIsValid ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_3A9BBB6B_No_search_value_has_been_specified_ );
							}
							else if ( !bParsed ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
							}
							else {
								CUtilities::ToDataTypeString( rRes, dtDataType, wsStr1 );
								rResLow = rRes;

								if ( !pcbRValCombo ) {
									wText = mx::CStringDecoder::DecodeToWString( _T_LEN_14F91708_The_range_of_values_to_find_is_incomplete_ );
								}
								else {
									bParsed = pcbRValCombo->GetTextAsExpression( rRes, ee::CObject::EE_TF_NONE, &bIsValid );
									if ( !bIsValid ) {
										wText = mx::CStringDecoder::DecodeToWString( _T_LEN_14F91708_The_range_of_values_to_find_is_incomplete_ );
									}
									else if ( !bParsed ) {
										wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
									}
									else {
										rResHi = rRes;
										CSecureWString wsNegation = pcbInvertCheck && pcbInvertCheck->IsChecked() ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
										wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_837BF7A2_Find__s_values__sequal_to_or_between__s_and__s_ );
										CUtilities::PrintDataType( wsStr0, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );
										CUtilities::ToDataTypeString( rRes, dtDataType, wsStr2 );

										WCHAR wszBuffer[512];
										std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsStr0.c_str(), wsNegation.c_str(), wsStr1.c_str(), wsStr2.c_str() );

										wText = wszBuffer;

										if ( m_bLastTypeExpressionWasEmpty ) {
											CComboBox * pcbExpCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EXP_COMBO ));
											if ( pcbExpCombo ) {
												CSecureString ssExact;
												CreateRangeExpression( ssExact, rResLow, rResHi, dtDataType, pcbInvertCheck->IsChecked() );
												pcbExpCombo->SetTextA( ssExact.c_str() );
											}
										}
									}
								}
							}
							break;
						}
						case CUtilities::MX_ET_UNKNOWN : {
							wsTemp = ((pcbInvertCheck && pcbInvertCheck->IsChecked()) && false) ? mx::CStringDecoder::DecodeToWString( _T_LEN_B6D4D1A0_Find_no__s_values_ ) :
								mx::CStringDecoder::DecodeToWString( _T_LEN_2FF885A4_Find_all__s_values_ );
							CSecureWString wsLeft;
							CUtilities::PrintDataType( wsLeft, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );

							WCHAR wszBuffer[512];
							std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsLeft.c_str() );

							wText = wszBuffer;
							break;
						}
						case CUtilities::MX_ET_QUICK_EXP : {
							CSecureString ssExp;
							CSecureWString wResult;
							CSecureWString wFinalEvaluation;
							if ( !VerifyExpression( ssExp, wResult, wFinalEvaluation, dtDataType ) ) {
								wsTemp = wFinalEvaluation;
							}
							else {
								wsTemp = pcbInvertCheck && pcbInvertCheck->IsChecked() ? mx::CStringDecoder::DecodeToWString( _T_LEN_4D9B69FA_Expression_returns_0_to_add_an_address_ ) :
								mx::CStringDecoder::DecodeToWString( _T_LEN_EB543D9B_Expression_returns_non_0_to_add_an_address_ );
							}
							wText = wsTemp;
							break;
						}
					}
				}
			}


			CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_ALIGNED ));
			if ( pcbCheck ) {
				WCHAR wszBuffer[512];
				DWORD dwSearchStride = CUtilities::DataTypeAlign( dtDataType );
				if ( dwSearchStride != 1 && pcbCheck->IsChecked() ) {
					wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_3646A145_Searches_every__u_bytes__aligned_ );
					std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), dwSearchStride );
					wsTemp = wszBuffer;
				}
				else {
					wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_A4101CB6_Searches_every_byte );
				}
				
				if ( wText.size() ) {
					wText += L"\r\n";
				}
				wText += wsTemp;


				// Address.
				ee::CExpEvalContainer::EE_RESULT rFrom, rTo;
				pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_FROM_COMBO ));
				bool bCapped = false;
				if ( pcbCombo && pcbCombo->GetTextAsExpression( rFrom, ee::CObject::EE_TF_NONE ) ) {
					pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_TO_COMBO ));
					if ( pcbCombo && pcbCombo->GetTextAsExpression( rTo, ee::CObject::EE_TF_NONE ) ) {
						rFrom = ee::CExpEvalContainer::ConvertResult( rFrom, ee::EE_NC_UNSIGNED );
						rTo = ee::CExpEvalContainer::ConvertResult( rTo, ee::EE_NC_UNSIGNED );
						CSecureWString wFrom, wTo;
						CUtilities::ToHex( rFrom.u.ui64Val, wFrom, 1 );
						CUtilities::ToHex( rTo.u.ui64Val, wTo, 1 );
						//wText.pop_back();
						wText += L" ";
						wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_19729486_from__s_to__s_ );
						std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wFrom.c_str(), wTo.c_str() );
						wText += wszBuffer;
						bCapped = true;
					}
				}
				if ( !bCapped ) {
					wText.push_back( L'.' );
				}
			}

			plLabel->SetTextW( wText.c_str() );
		}

		
	}

	// Creates an expression for searching for exact values.
	void CNewDataTypeSearchWindow::CreateExactValueExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		int32_t iIsInf = CUtilities::DataTypeIsInf( _rLVal, _dtDataType );
		bool bIsFloat = CUtilities::DataTypeIsFloat( _dtDataType );
		bool bUsedNan = false;
		if ( m_pmhMemHack->Options().bUseEpsilon && bIsFloat ) {
			if ( _dtDataType == CUtilities::MX_DT_DOUBLE ) {
				if ( m_pmhMemHack->Options().bSmartEpsilon ) {
					_sExpression = _DEC_S_6015A3A2_repsilon______;
				}
				else {
					_sExpression = _DEC_S_B5DBEB0F_epsilon______;
				}
			}
			else {
				if ( m_pmhMemHack->Options().bSmartEpsilon ) {
					_sExpression = _DEC_S_79D6C61D_repsilonf______;
				}
				else {
					_sExpression = _DEC_S_D164D700_epsilonf______;
				}
			}
			CSecureWString swsLeft;
			CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft, true );
			_sExpression.append( ee::CExpEval::WStringToString( swsLeft ) );
			_sExpression.push_back( ',' );
			_sExpression.push_back( ' ' );

			swsLeft.SecureClear();
			CUtilities::ToDouble( m_pmhMemHack->Options().dEpsilon, swsLeft );
			_sExpression.append( ee::CExpEval::WStringToString( swsLeft ) );

			_sExpression.push_back( ' ' );
			_sExpression.push_back( ')' );
		}
		else {
			if ( bIsFloat && _bInv ) {
				if ( iIsInf > 0 ) {
					_sExpression = _DEC_S_FAD085A8_isinf______;
				}
				else if ( iIsInf < 0 ) {
					_sExpression = _DEC_S_94DF1758_isinf_______;
				}
				else if ( CUtilities::DataTypeIsNan( _rLVal, _dtDataType ) ) {
					_sExpression = _DEC_S_747EB4AA_isnan______;
					bUsedNan = true;
				}
				else {
					_sExpression = _DEC_S_BA988A07_______;
					CSecureWString swsLeft;
					CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft );
					_sExpression.append( ee::CExpEval::WStringToString( swsLeft ) );
				}
			}
			else {
				if ( iIsInf > 0 ) {
					_sExpression = _DEC_S_FAD085A8_isinf______;
				}
				else if ( iIsInf < 0 ) {
					_sExpression = _DEC_S_94DF1758_isinf_______;
				}
				else if ( CUtilities::DataTypeIsNan( _rLVal, _dtDataType ) ) {
					_sExpression = _DEC_S_747EB4AA_isnan______;
				}
				else {
					CSecureWString swsLeft;
					CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft );
					_sExpression = _DEC_S_BA988A07_______;
					_sExpression.append( ee::CExpEval::WStringToString( swsLeft ) );
				}
			}
		}

		if ( bIsFloat && _bInv && !bUsedNan ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// Creates an expression for searching for greater-than values.
	void CNewDataTypeSearchWindow::CreateGreaterThanExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		
		CSecureWString swsLeft;
		CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft, true );
		if ( CUtilities::DataTypeIsFloat( _dtDataType ) && _bInv ) {
			bool bUsedNan = false;
			/*if ( CUtilities::DataTypeIsNan( _rLVal, _dtDataType ) ) {
				bUsedNan = true;
			}*/
			_sExpression = _DEC_S_0FFD4DE8______;
			_sExpression.append( ee::CExpEval::WStringToString( swsLeft ) );
			if ( !bUsedNan ) {
				_sExpression.push_back( ' ' );
				_sExpression.append( _DEC_S_36A7406E____isnan______ );
			}
		}
		else {
			_sExpression = _DEC_S_0FFD4DE8______;
			_sExpression.append( ee::CExpEval::WStringToString( swsLeft ) );
		}
	}

	// Creates an expression for searching for less-than values.
	void CNewDataTypeSearchWindow::CreateLessThanExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		CSecureWString swsLeft;
		CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft, true );
		if ( CUtilities::DataTypeIsFloat( _dtDataType ) && _bInv ) {
			bool bUsedNan = false;
			/*if ( CUtilities::DataTypeIsNan( _rLVal, _dtDataType ) ) {
				bUsedNan = true;
			}*/
			_sExpression = _DEC_S_3DCB2F6A______;
			_sExpression.append( ee::CExpEval::WStringToString( swsLeft ) );
			if ( !bUsedNan ) {
				_sExpression.push_back( ' ' );
				_sExpression.append( _DEC_S_36A7406E____isnan______ );
			}
		}
		else {
			_sExpression = _DEC_S_3DCB2F6A______;
			_sExpression.append( ee::CExpEval::WStringToString( swsLeft ) );
		}
	}

	// Creates an expression for searching for ranged values.
	void CNewDataTypeSearchWindow::CreateRangeExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal,
		const ee::CExpEvalContainer::EE_RESULT &_rRVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		ee::CExpEvalContainer::EE_RESULT rLow;
		ee::CExpEvalContainer::EE_RESULT rHi;
		ee::CExpEvalContainer::EE_RESULT rTempL, rTempH;
		if ( CUtilities::DataTypeIsFloat( _dtDataType ) ) {
			rLow = ee::CExpEvalContainer::ConvertResult( _rLVal, ee::EE_NC_FLOATING );
			rHi = ee::CExpEvalContainer::ConvertResult( _rRVal, ee::EE_NC_FLOATING );

			rTempL.ncType = ee::EE_NC_FLOATING;
			rTempL.u.dVal = std::min( rLow.u.dVal, rHi.u.dVal );
			rTempH.ncType = ee::EE_NC_FLOATING;
			rTempH.u.dVal = std::max( rLow.u.dVal, rHi.u.dVal );
		}
		else if ( CUtilities::DataTypeIsSigned( _dtDataType ) ) {
			rLow = ee::CExpEvalContainer::ConvertResult( _rLVal, ee::EE_NC_SIGNED );
			rHi = ee::CExpEvalContainer::ConvertResult( _rRVal, ee::EE_NC_SIGNED );

			rTempL.ncType = ee::EE_NC_SIGNED;
			rTempL.u.i64Val = std::min( rLow.u.i64Val, rHi.u.i64Val );
			rTempH.ncType = ee::EE_NC_SIGNED;
			rTempH.u.i64Val = std::max( rLow.u.i64Val, rHi.u.i64Val );
		}
		else {
			rLow = ee::CExpEvalContainer::ConvertResult( _rLVal, ee::EE_NC_UNSIGNED );
			rHi = ee::CExpEvalContainer::ConvertResult( _rRVal, ee::EE_NC_UNSIGNED );

			rTempL.ncType = ee::EE_NC_UNSIGNED;
			rTempL.u.ui64Val = std::min( rLow.u.ui64Val, rHi.u.ui64Val );
			rTempH.ncType = ee::EE_NC_UNSIGNED;
			rTempH.u.ui64Val = std::max( rLow.u.ui64Val, rHi.u.ui64Val );
		}

		CSecureWString swsLeft, swsRight;
		CUtilities::ToDataTypeString( rTempL, _dtDataType, swsLeft, true );
		CUtilities::ToDataTypeString( rTempH, _dtDataType, swsRight, true );
		
		_sExpression = _DEC_S_68223D78________;
		_sExpression.append( ee::CExpEval::WStringToString( swsLeft ) );
		_sExpression.append( _DEC_S_C4C57451_____________ );
		_sExpression.append( ee::CExpEval::WStringToString( swsRight ) );
		_sExpression.push_back( ')' );
		if ( CUtilities::DataTypeIsFloat( _dtDataType ) && _bInv ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// User-variable function handler.
	bool __stdcall CNewDataTypeSearchWindow::UserVarHandler( uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		::memcpy( &_rResult, reinterpret_cast<const void *>(_uiptrData), sizeof( _rResult ) );
		return true;
	}

}	// namespace mx

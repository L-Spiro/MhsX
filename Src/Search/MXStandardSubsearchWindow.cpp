#include "MXStandardSubsearchWindow.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXStandardSubsearchLayout.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include <CheckButton/LSWCheckButton.h>
#include <Static/LSWStatic.h>


namespace mx {

	// == Members.
	CStandardSubsearchWindow::MX_SEARCH_WIN_DATA CStandardSubsearchWindow::m_swdData = {
		CUtilities::MX_ET_EXACT,				// uiEvalType
	};

	CStandardSubsearchWindow::CStandardSubsearchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmhMemHack( reinterpret_cast<CMemHack *>(_ui64Data) ),
		m_bLastTypeExpressionWasEmpty( true ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CStandardSubsearchWindow::InitDialog() {
		CComboBox * pcbCombo;
		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_TYPE_COMBO ));
		if ( pcbCombo ) {
			const CUtilities::MX_SUB_EVAL_TYPES stTypes[] = {
				CUtilities::MX_SET_EXACT,
				CUtilities::MX_SET_NOT_EQUAL_TO,
				CUtilities::MX_SET_INCREASED,
				CUtilities::MX_SET_DECREASED,
				CUtilities::MX_SET_RANGE,
				CUtilities::MX_SET_SAME_AS_BEFORE,
				CUtilities::MX_SET_DIFFERENT_FROM_BEFORE,
				CUtilities::MX_SET_CHANGED_BY,
				CUtilities::MX_SET_CHANGED_BY_PERCENT,
				CUtilities::MX_SET_CHANGED_BY_RANGE,
				CUtilities::MX_SET_CHANGED_BY_RANGE_PERCENT,
				//CUtilities::MX_SET_SAME_AS_ORIGINAL,
				CUtilities::MX_SET_QUICK_EXP,
			};
			for ( size_t I = 0; I < MX_ELEMENTS( stTypes ); ++I ) {
				CSecureWString sTemp;
				CUtilities::SubEvaluationTypeToString( stTypes[I], sTemp );
				INT iIndex = pcbCombo->AddString( sTemp.c_str() );
				pcbCombo->SetItemData( iIndex, stTypes[I] );
			}
			pcbCombo->AutoSetMinListWidth();

			pcbCombo->SetCurSelByItemData( m_swdData.uiEvalType );
		}
		
		CStatic * pcbStatic;
		pcbStatic = static_cast<CStatic *>(FindChild( CStandardSubsearchLayout::MX_C_INFO_ORIG_SEARCH_TYPE_VAL_LABEL ));
		if ( pcbStatic ) {
			CSecureWString sTemp;
			CUtilities::SearchTypeToString( m_pmhMemHack->Searcher().LastSearchParms().stType, sTemp );
			switch ( m_pmhMemHack->Searcher().LastSearchParms().stType ) {
				case CUtilities::MX_ST_DATATYPE_SEARCH : {
					sTemp += L", ";
					CUtilities::EvaluationTypeToString( static_cast<CUtilities::MX_EVAL_TYPES>(m_pmhMemHack->Searcher().LastSearchParms().i32EvalType), sTemp );
					sTemp += L", ";
					CUtilities::PrintDataType( sTemp, m_pmhMemHack->Searcher().LastSearchParms().dtLVal.dtType, m_pmhMemHack->Options().bDataTypesAsCodeNames ? CUtilities::MX_DTO_CODENAMES : 0 );
					if ( !m_pmhMemHack->Searcher().LastSearchTotalResults() ) {
						CStatic * pcbStaticPrev = static_cast<CStatic *>(FindChild( CStandardSubsearchLayout::MX_C_INFO_SEARCH_TYPE_VAL_LABEL ));
						if ( pcbStaticPrev ) {
							//pcbStaticPrev->SetTextW( sTemp.c_str() );
							pcbStaticPrev->SetTextW( L"" );
						}
					}
					break;
				}
				case CUtilities::MX_ST_STRING_SEARCH : {
					break;
				}
			}

			pcbStatic->SetTextW( sTemp.c_str() );
		}

		pcbStatic = static_cast<CStatic *>(FindChild( CStandardSubsearchLayout::MX_C_INFO_SEARCH_TYPE_VAL_LABEL ));
		if ( pcbStatic ) {
			CSecureWString sTemp;
			switch ( m_pmhMemHack->Searcher().LastSearchParms().stType ) {
				case CUtilities::MX_ST_DATATYPE_SEARCH : {
					if ( m_pmhMemHack->Searcher().LastSearchWasSubsearch() ) {
						// A subsearch has been performed.  Previous results differ from current results.
						CUtilities::SubEvaluationTypeToString( static_cast<CUtilities::MX_SUB_EVAL_TYPES>(m_pmhMemHack->Searcher().LastSubsearchParms().i32EvalType), sTemp );
					}
					break;
				}
			}
			pcbStatic->SetTextW( sTemp.c_str() );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_LEFT_VALUE_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vLValHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vLValHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsLValText.c_str() );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_RIGHT_VALUE_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vRValHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vRValHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsRValText.c_str() );
			//pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_EXP_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vExpHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vExpHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsExpText.c_str() );
			//pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}
		

		if ( m_pmhMemHack ) {
			CCheckButton * pcbCheck;
			CStatic * pcbExpStatic = static_cast<CStatic *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_EXP_LABEL ));
			if ( pcbExpStatic ) {
				CSearcher::MX_SEARCH_LOCK slSearchLock( &m_pmhMemHack->Searcher() );
				if ( !m_pmhMemHack->Searcher().SearchResults() || !m_pmhMemHack->Searcher().SearchResults()->StoresValues() ) {
					pcbExpStatic->SetTextW( _DEC_WS_68C09196_Value_____.c_str() );
				}
				else {
					pcbExpStatic->SetTextW( _DEC_WS_289E3DA9_Value_______Previous_Value____0.c_str() );
				}
			}
		}

		CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CStandardSubsearchLayout::MX_C_OPTIONS_INVERT_CHECK ));
		if ( pcbCheck ) {
			pcbCheck->CheckButton( m_swdData.bInvertResults ? BST_CHECKED : BST_UNCHECKED );
		}


		pcbStatic = static_cast<CStatic *>(FindChild( CStandardSubsearchLayout::MX_C_INFO_RESULTS_VAL_LABEL ));
		if ( pcbStatic ) {
			char szBuffer[64];
			CUtilities::ToUnsigned( m_pmhMemHack->Searcher().TotalResultsQuick(), szBuffer, MX_ELEMENTS( szBuffer ) );
			pcbStatic->SetTextA( szBuffer );
		}
		UpdateDialog();
		return CMainWindow::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CStandardSubsearchWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CStandardSubsearchLayout::MX_C_CANCEL : {
				return Close();
			}
			case CStandardSubsearchLayout::MX_C_OK : {
				CSecureWString wsError;
				CSearcher::MX_SUBSEARCH_PARMS spParms;
				if ( !DialogContentsAreValid( wsError, spParms ) ) {
					lsw::CBase::MessageBoxError( Wnd(), wsError.c_str() );
					break;
				}
				SaveDialogData( m_swdData );
				CMhsMainWindow * pmmwMain = static_cast<CMhsMainWindow *>(FindParent( mx::CMainWindowLayout::MX_MWI_MAINWINDOW ));
				if ( pmmwMain ) {
					pmmwMain->Subsearch( &spParms, this, true, nullptr );
				}
				return Close();
			}
			case CStandardSubsearchLayout::MX_C_SEARCH_LEFT_VALUE_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CStandardSubsearchLayout::MX_C_SEARCH_RIGHT_VALUE_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CStandardSubsearchLayout::MX_C_SEARCH_TYPE_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CStandardSubsearchLayout::MX_C_SEARCH_EXP_COMBO : {
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
			case CStandardSubsearchLayout::MX_C_OPTIONS_OPTIONS_BUTTON : {
				CMhsMainWindow * pmmwMain = static_cast<CMhsMainWindow *>(FindParent( mx::CMainWindowLayout::MX_MWI_MAINWINDOW ));
				if ( pmmwMain ) {
					pmmwMain->ShowOptions( pmmwMain->SearchOptionsPage() );
					UpdateDialog();
				}
				break;
			}
			case CStandardSubsearchLayout::MX_C_OPTIONS_INVERT_CHECK : {
				if ( _wCtrlCode == STN_CLICKED ) {
					UpdateDialog();
				}
				break;
			}
		}
		return CMainWindow::Command( _wCtrlCode, _Id, _pwSrc );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CStandardSubsearchWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	// Saves the current dialog values to an MX_SEARCH_WIN_DATA structure.
	void CStandardSubsearchWindow::SaveDialogData( MX_SEARCH_WIN_DATA &_swdData ) {
		CComboBox * pcbCombo;
		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_TYPE_COMBO ));
		LPARAM lpEval = 0;
		if ( pcbCombo ) {
			lpEval = pcbCombo->GetCurSelItemData();
			_swdData.uiEvalType = pcbCombo->GetItemData( pcbCombo->GetCurSel() );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_LEFT_VALUE_COMBO ));
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
		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_RIGHT_VALUE_COMBO ));
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
		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_EXP_COMBO ));
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
		CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CStandardSubsearchLayout::MX_C_OPTIONS_INVERT_CHECK ));
		_swdData.bInvertResults = pcbCheck && pcbCheck->IsChecked();

		if ( m_pmhMemHack ) {
			MX_OPTIONS oOpts = m_pmhMemHack->Options();
			CCheckButton * pcbCheck;
			
			m_pmhMemHack->SetOptions( oOpts );
		}
	}

	// Validates the dialog box.
	bool CStandardSubsearchWindow::DialogContentsAreValid( CSecureWString &_wsError, CSearcher::MX_SUBSEARCH_PARMS &_spParmsIfValid ) {
		//_spParmsIfValid.stType = CUtilities::MX_ST_DATATYPE_SEARCH;
		_spParmsIfValid.pbAbort = nullptr;
		_spParmsIfValid.iThreadPriority = THREAD_PRIORITY_NORMAL;
		::ZeroMemory( &_spParmsIfValid.dtLVal, sizeof( _spParmsIfValid.dtLVal ) );
		::ZeroMemory( &_spParmsIfValid.dtRVal, sizeof( _spParmsIfValid.dtRVal ) );
		_spParmsIfValid.ssExpression.clear();

		_spParmsIfValid.stInitialType = m_pmhMemHack->Searcher().LastSearchParms().stType;

		CUtilities::MX_DATA_TYPES dtDataType = m_pmhMemHack->Searcher().LastSearchParms().dtLVal.dtType;
		_spParmsIfValid.dtLVal.dtType = dtDataType;

		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_TYPE_COMBO ));
		LRESULT lrEvalType = pcbCombo->GetCurSelItemData();
		_spParmsIfValid.i32EvalType = static_cast<int32_t>(lrEvalType);

		BOOL bIsValid;
		ee::CExpEvalContainer::EE_RESULT rRes;
		// Left values.
		switch ( lrEvalType ) {
			case CUtilities::MX_SET_EXACT : {}
			case CUtilities::MX_SET_NOT_EQUAL_TO : {}
			case CUtilities::MX_SET_RANGE : {}
			case CUtilities::MX_SET_CHANGED_BY : {}
			case CUtilities::MX_SET_CHANGED_BY_PERCENT : {}
			case CUtilities::MX_SET_CHANGED_BY_RANGE : {}
			case CUtilities::MX_SET_CHANGED_BY_RANGE_PERCENT : {
				pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_LEFT_VALUE_COMBO ));
				if ( !pcbCombo ) {
					_wsError = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
					return false;
				}
				
				BOOL bParsed = pcbCombo->GetTextAsExpression( rRes, &bIsValid );
				if ( !bParsed ) {
					_wsError = bIsValid ? mx::CStringDecoder::DecodeToWString( _T_LEN_BDC62AA7_The_specified_value_is_invalid_ ) :
						mx::CStringDecoder::DecodeToWString( _T_LEN_3A9BBB6B_No_search_value_has_been_specified_ );
					return false;
				}
				if ( lrEvalType == CUtilities::MX_SET_CHANGED_BY_PERCENT || lrEvalType == CUtilities::MX_SET_CHANGED_BY_RANGE_PERCENT ) {
					_spParmsIfValid.dtLVal.u.Float64 = CUtilities::ExpEvalResultToDataType( rRes, CUtilities::MX_DT_DOUBLE ).u.Float64;
				}
				else {
					_spParmsIfValid.dtLVal = CUtilities::ExpEvalResultToDataType( rRes, dtDataType );
				}
				break;
			}
			case CUtilities::MX_SET_QUICK_EXP : {
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

		// Right values.
		switch ( lrEvalType ) {
			case CUtilities::MX_SET_RANGE : {}
			case CUtilities::MX_SET_CHANGED_BY_RANGE : {}
			case CUtilities::MX_SET_CHANGED_BY_RANGE_PERCENT : {
				pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_RIGHT_VALUE_COMBO ));
				if ( !pcbCombo ) {
					_wsError = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
					return false;
				}
				BOOL bParsed = pcbCombo->GetTextAsExpression( rRes, &bIsValid );
				if ( !bParsed ) {
					_wsError = bIsValid ? mx::CStringDecoder::DecodeToWString( _T_LEN_BDC62AA7_The_specified_value_is_invalid_ ) :
						mx::CStringDecoder::DecodeToWString( _T_LEN_14F91708_The_range_of_values_to_find_is_incomplete_ );
					return false;
				}
				if ( lrEvalType == CUtilities::MX_SET_CHANGED_BY_PERCENT || lrEvalType == CUtilities::MX_SET_CHANGED_BY_RANGE_PERCENT ) {
					_spParmsIfValid.dtRVal.u.Float64 = CUtilities::ExpEvalResultToDataType( rRes, CUtilities::MX_DT_DOUBLE ).u.Float64;
				}
				else {
					_spParmsIfValid.dtRVal = CUtilities::ExpEvalResultToDataType( rRes, dtDataType );
				}
				break;
			}
		}

		CCheckButton * pcbCheck;
		pcbCheck = static_cast<CCheckButton *>(FindChild( CStandardSubsearchLayout::MX_C_OPTIONS_INVERT_CHECK ));
		if ( pcbCheck ) { _spParmsIfValid.bInvertResults = pcbCheck->IsChecked(); }
		else { _spParmsIfValid.bInvertResults = false; }
		return true;
	}

	// Verifies that the expression is valid and returns descriptive texts.
	bool CStandardSubsearchWindow::VerifyExpression( CSecureString &_sExpression, CSecureWString &_wsResult, CSecureWString &_wsHelp, CUtilities::MX_DATA_TYPES _dtDataType ) {
		BOOL bIsValid;
		{
			CComboBox * pcbExpCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_EXP_COMBO ));
			CExpression eExp;
			std::string sText = pcbExpCombo->GetTextA();
			if ( sText.size() != 0 ) {
				if ( !eExp.SetExpression( sText.c_str() ) ) { _wsHelp = _DEC_WS_02BBA31E_Invalid_expression_; return false; }

				if ( !eExp.GetContainer() ) { _wsHelp = _DEC_WS_02BBA31E_Invalid_expression_; return false; }
				
				bool bCanHaveNumbered = false;
				{
					CSearcher::MX_SEARCH_LOCK slSearchLock( &m_pmhMemHack->Searcher() );
					bCanHaveNumbered = m_pmhMemHack->Searcher().SearchResults() && m_pmhMemHack->Searcher().SearchResults()->StoresValues();
				}
				bool bNumberedParms = eExp.GetContainer()->IsNumberedParmAccessed( 0 );
				if ( bNumberedParms && !bCanHaveNumbered ) { _wsHelp = _DEC_WS_ED845380_Expression_uses___integer__but_there_are_no_previous_results_to_access_; return false; }

				bool bUserData = eExp.GetContainer()->HasAccessToUserData();
				// Either ?? must be used or $0 must be valid and used.
				if ( !bUserData && !(bCanHaveNumbered && bNumberedParms) ) { _wsHelp = _DEC_WS_0B220183_Expression_Must_Use____or__0; return false; }

				_sExpression = sText;
				return true;
			}
			else {
				_wsHelp = _DEC_WS_02BBA31E_Invalid_expression_;
			}
		}
		return false;
	}

	// Updates the dialog.
	void CStandardSubsearchWindow::UpdateDialog() {
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_TYPE_COMBO ));
		CComboBox * pcbLCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_LEFT_VALUE_COMBO ));
		CComboBox * pcbRCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_RIGHT_VALUE_COMBO ));
		CComboBox * pcbExpCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_EXP_COMBO ));

		CStatic * pcbLStatic = static_cast<CStatic *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_LEFT_DESC_LABEL ));
		CStatic * pcbRStatic = static_cast<CStatic *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_RIGHT_DESC_LABEL ));
		CStatic * pcbExpStatic = static_cast<CStatic *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_EXP_LABEL ));

		CCheckButton * pcbInvertCheck = static_cast<CCheckButton *>(FindChild( CStandardSubsearchLayout::MX_C_OPTIONS_INVERT_CHECK ));
		if ( !pcbCombo || !pcbLCombo || !pcbRCombo || !pcbExpCombo || !pcbLStatic || !pcbRStatic || !pcbExpStatic || !pcbInvertCheck ) { return; }
		LPARAM lpSearchType = pcbCombo->GetCurSelItemData();
		pcbExpCombo->SetEnabled( lpSearchType == CUtilities::MX_SET_QUICK_EXP );
		switch ( lpSearchType ) {
			case CUtilities::MX_SET_EXACT : {
				pcbLCombo->SetEnabled( TRUE );
				pcbRCombo->SetEnabled( FALSE );

				pcbLStatic->SetTextW( _DEC_WS_8C044A93_Value_to_Find_.c_str() );
				pcbRStatic->SetTextW( L"" );
				break;
			}
			case CUtilities::MX_SET_NOT_EQUAL_TO : {
				pcbLCombo->SetEnabled( TRUE );
				pcbRCombo->SetEnabled( FALSE );

				pcbLStatic->SetTextW( _DEC_WS_5BA7EED2_Find_Values_not_Equal_To_.c_str() );
				pcbRStatic->SetTextW( L"" );
				break;
			}
			case CUtilities::MX_SET_INCREASED : {
				pcbLCombo->SetEnabled( FALSE );
				pcbRCombo->SetEnabled( FALSE );

				pcbLStatic->SetTextW( _DEC_WS_373DBC5B_Find_Values_that_Increased.c_str() );
				pcbRStatic->SetTextW( L"" );
				break;
			}
			case CUtilities::MX_SET_DECREASED : {
				pcbLCombo->SetEnabled( FALSE );
				pcbRCombo->SetEnabled( FALSE );

				pcbLStatic->SetTextW( _DEC_WS_9B18475A_Find_Values_that_Decreased.c_str() );
				pcbRStatic->SetTextW( L"" );
				break;
			}
			case CUtilities::MX_SET_RANGE : {
				pcbLCombo->SetEnabled( TRUE );
				pcbRCombo->SetEnabled( TRUE );

				pcbLStatic->SetTextW( _DEC_WS_DF0979ED_Find_Values_Between_.c_str() );
				pcbRStatic->SetTextW( _DEC_WS_8738717F_And_.c_str() );
				break;
			}
			case CUtilities::MX_SET_SAME_AS_BEFORE : {
				pcbLCombo->SetEnabled( FALSE );
				pcbRCombo->SetEnabled( FALSE );

				pcbLStatic->SetTextW( _DEC_WS_78DEC9F8_Find_Values_that_did_not_Change.c_str() );
				pcbRStatic->SetTextW( L"" );
				break;
			}
			case CUtilities::MX_SET_DIFFERENT_FROM_BEFORE : {
				pcbLCombo->SetEnabled( FALSE );
				pcbRCombo->SetEnabled( FALSE );

				pcbLStatic->SetTextW( _DEC_WS_C1EB737E_Find_Values_that_Changed.c_str() );
				pcbRStatic->SetTextW( L"" );
				break;
			}
			case CUtilities::MX_SET_CHANGED_BY : {
				pcbLCombo->SetEnabled( TRUE );
				pcbRCombo->SetEnabled( FALSE );

				pcbLStatic->SetTextW( _DEC_WS_4627EB26_Find_Values_that_Changed_By_.c_str() );
				pcbRStatic->SetTextW( L"" );
				break;
			}
			case CUtilities::MX_SET_CHANGED_BY_PERCENT : {
				pcbLCombo->SetEnabled( TRUE );
				pcbRCombo->SetEnabled( FALSE );

				pcbLStatic->SetTextW( _DEC_WS_4627EB26_Find_Values_that_Changed_By_.c_str() );
				pcbRStatic->SetTextW( L"" );
				break;
			}
			case CUtilities::MX_SET_CHANGED_BY_RANGE : {
				pcbLCombo->SetEnabled( TRUE );
				pcbRCombo->SetEnabled( TRUE );

				pcbLStatic->SetTextW( _DEC_WS_4627EB26_Find_Values_that_Changed_By_.c_str() );
				pcbRStatic->SetTextW( L"" );
				break;
			}
			case CUtilities::MX_SET_CHANGED_BY_RANGE_PERCENT : {
				pcbLCombo->SetEnabled( TRUE );
				pcbRCombo->SetEnabled( TRUE );

				pcbLStatic->SetTextW( _DEC_WS_4627EB26_Find_Values_that_Changed_By_.c_str() );
				pcbRStatic->SetTextW( L"" );
				break;
			}
			case CUtilities::MX_SET_SAME_AS_ORIGINAL : {
				pcbLCombo->SetEnabled( FALSE );
				pcbRCombo->SetEnabled( FALSE );

				pcbLStatic->SetTextW( _DEC_WS_7BD6DC75_Same_as_Original.c_str() );
				pcbRStatic->SetTextW( L"" );
				break;
			}
			case CUtilities::MX_SET_QUICK_EXP : {
				pcbLCombo->SetEnabled( FALSE );
				pcbRCombo->SetEnabled( FALSE );

				pcbLStatic->SetTextW( _DEC_WS_1A50E6C7_Quick_Expression_.c_str() );
				pcbRStatic->SetTextW( L"" );
				
				break;
			}
		}

		{

			// Update the description label.
			CStatic * plLabel = static_cast<CStatic *>(FindChild( CStandardSubsearchLayout::MX_C_DESC_LABEL ));
			CUtilities::MX_DATA_TYPES dtDataType = m_pmhMemHack->Searcher().LastSearchParms().dtLVal.dtType;
			ee::CExpEvalContainer::EE_RESULT rRes, rResLow, rResHi;
			BOOL bIsValid;
			CSecureWString wText;
			CSecureWString wsTemp;
			switch ( lpSearchType ) {
				case CUtilities::MX_SET_EXACT : {
					BOOL bParsed = pcbLCombo->GetTextAsExpression( rRes, &bIsValid );
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
							CSecureString ssExact;
							CreateExactValueExpression( ssExact, rRes, dtDataType, pcbInvertCheck->IsChecked() );
							pcbExpCombo->SetTextA( ssExact.c_str() );
						}
					}
					break;
				}
				case CUtilities::MX_SET_NOT_EQUAL_TO : {
					BOOL bParsed = pcbLCombo->GetTextAsExpression( rRes, &bIsValid );
					if ( !bIsValid ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
					}
					else if ( !bParsed ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_3A9BBB6B_No_search_value_has_been_specified_ );
					}
					else {
						CSecureWString wsNegation = !(pcbInvertCheck && pcbInvertCheck->IsChecked()) ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
						wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_3048B35F_Find__s_values__sequal_to__s_ );
						CSecureWString wsLeft, wsRight;
						CUtilities::PrintDataType( wsLeft, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );

						CUtilities::ToDataTypeString( rRes, dtDataType, wsRight );
						WCHAR wszBuffer[512];
						std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsLeft.c_str(), wsNegation.c_str(), wsRight.c_str() );

						wText = wszBuffer;

						if ( m_bLastTypeExpressionWasEmpty ) {
							CSecureString ssExact;
							CreateNotEqualToExpression( ssExact, rRes, dtDataType, pcbInvertCheck->IsChecked() );
							pcbExpCombo->SetTextA( ssExact.c_str() );
						}
					}
					break;
				}
				case CUtilities::MX_SET_INCREASED : {
					CSecureWString wsNegation = (pcbInvertCheck && pcbInvertCheck->IsChecked()) ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
					wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_ABF9FAF0_Find__s_values_that_have__sincreased_ );
					CSecureWString wsLeft;
					CUtilities::PrintDataType( wsLeft, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );

					WCHAR wszBuffer[512];
					std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsLeft.c_str(), wsNegation.c_str() );

					wText = wszBuffer;

					if ( m_bLastTypeExpressionWasEmpty ) {
						CSecureString ssExact;
						CreateIncreasedExpression( ssExact, rRes, dtDataType, pcbInvertCheck->IsChecked() );
						pcbExpCombo->SetTextA( ssExact.c_str() );
					}
					break;
				}
				case CUtilities::MX_SET_DECREASED : {
					CSecureWString wsNegation = (pcbInvertCheck && pcbInvertCheck->IsChecked()) ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
					wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_DC52EF9D_Find__s_values_that_have__sdecreased_ );
					CSecureWString wsLeft;
					CUtilities::PrintDataType( wsLeft, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );

					WCHAR wszBuffer[512];
					std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsLeft.c_str(), wsNegation.c_str() );

					wText = wszBuffer;

					if ( m_bLastTypeExpressionWasEmpty ) {
						CSecureString ssExact;
						CreateDecreasedExpression( ssExact, rRes, dtDataType, pcbInvertCheck->IsChecked() );
						pcbExpCombo->SetTextA( ssExact.c_str() );
					}
					break;
				}
				case CUtilities::MX_SET_RANGE : {
					CSecureWString wsStr0, wsStr1, wsStr2;
					BOOL bParsed = pcbLCombo->GetTextAsExpression( rRes, &bIsValid );
					if ( !bIsValid ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_3A9BBB6B_No_search_value_has_been_specified_ );
					}
					else if ( !bParsed ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
					}
					else {
						CUtilities::ToDataTypeString( rRes, dtDataType, wsStr1, true );
						rResLow = rRes;

						if ( !pcbRCombo ) {
							wText = mx::CStringDecoder::DecodeToWString( _T_LEN_14F91708_The_range_of_values_to_find_is_incomplete_ );
						}
						else {
							bParsed = pcbRCombo->GetTextAsExpression( rRes, &bIsValid );
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
									CSecureString ssExact;
									CreateRangeExpression( ssExact, rResLow, rResHi, dtDataType, pcbInvertCheck->IsChecked() );
									pcbExpCombo->SetTextA( ssExact.c_str() );
								}
							}
						}
					}
					break;
				}
				case CUtilities::MX_SET_SAME_AS_BEFORE : {
					CSecureWString wsNegation = (pcbInvertCheck && pcbInvertCheck->IsChecked()) ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
					wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_BC7D1B47_Find__s_values_that_are__sthe_same_as_before_ );
					CSecureWString wsLeft;
					CUtilities::PrintDataType( wsLeft, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );

					WCHAR wszBuffer[512];
					std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsLeft.c_str(), wsNegation.c_str() );

					wText = wszBuffer;

					if ( m_bLastTypeExpressionWasEmpty ) {
						CSecureString ssExact;
						CreateSameAsBeforeExpression( ssExact, rRes, dtDataType, pcbInvertCheck->IsChecked() );
						pcbExpCombo->SetTextA( ssExact.c_str() );
					}
					break;
				}
				case CUtilities::MX_SET_DIFFERENT_FROM_BEFORE : {
					CSecureWString wsNegation = (pcbInvertCheck && pcbInvertCheck->IsChecked()) ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
					wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_4E91D410_Find__s_values_that_are__sdifferent_from_before_ );
					CSecureWString wsLeft;
					CUtilities::PrintDataType( wsLeft, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );

					WCHAR wszBuffer[512];
					std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsLeft.c_str(), wsNegation.c_str() );

					wText = wszBuffer;

					if ( m_bLastTypeExpressionWasEmpty ) {
						CSecureString ssExact;
						CreateDifferentFromBeforeExpression( ssExact, rRes, dtDataType, pcbInvertCheck->IsChecked() );
						pcbExpCombo->SetTextA( ssExact.c_str() );
					}
					break;
				}
				case CUtilities::MX_SET_CHANGED_BY : {
					BOOL bParsed = pcbLCombo->GetTextAsExpression( rRes, &bIsValid );
					if ( !bIsValid ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
					}
					else if ( !bParsed ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_78D93AF7_No_changed_by_value_has_been_specified_ );
					}
					else {
						CSecureWString wsNegation = pcbInvertCheck && pcbInvertCheck->IsChecked() ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
						wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_8240CD28_Find__s_values_that_have__schanged_by__s_ );
						CSecureWString wsLeft, wsRight;
						CUtilities::PrintDataType( wsLeft, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );

						CUtilities::ToDataTypeString( rRes, dtDataType, wsRight );
						WCHAR wszBuffer[512];
						std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsLeft.c_str(), wsNegation.c_str(), wsRight.c_str() );

						wText = wszBuffer;

						if ( m_bLastTypeExpressionWasEmpty ) {
							CSecureString ssExact;
							CreateChangedByExpression( ssExact, rRes, dtDataType, pcbInvertCheck->IsChecked() );
							pcbExpCombo->SetTextA( ssExact.c_str() );
						}
					}
					break;
				}
				case CUtilities::MX_SET_CHANGED_BY_PERCENT : {
					BOOL bParsed = pcbLCombo->GetTextAsExpression( rRes, &bIsValid );
					if ( !bIsValid ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
					}
					else if ( !bParsed ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_78D93AF7_No_changed_by_value_has_been_specified_ );
					}
					else {
						CSecureWString wsNegation = pcbInvertCheck && pcbInvertCheck->IsChecked() ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
						wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_16336FF9_Find__s_values_that_have__sbeen_multiplied_by__s_ );
						CSecureWString wsLeft, wsRight;
						CUtilities::PrintDataType( wsLeft, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );

						CUtilities::ToDataTypeString( rRes, CUtilities::MX_DT_DOUBLE, wsRight );
						WCHAR wszBuffer[512];
						std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsLeft.c_str(), wsNegation.c_str(), wsRight.c_str() );

						wText = wszBuffer;

						if ( m_bLastTypeExpressionWasEmpty ) {
							rRes = ee::CExpEvalContainer::ConvertResult( rRes, ee::EE_NC_FLOATING );
							CSecureString ssExact;
							CreateChangedByPercentExpression( ssExact, rRes, dtDataType, pcbInvertCheck->IsChecked() );
							pcbExpCombo->SetTextA( ssExact.c_str() );
						}
					}
					break;
				}
				case CUtilities::MX_SET_CHANGED_BY_RANGE : {
					CSecureWString wsStr0, wsStr1, wsStr2;
					BOOL bParsed = pcbLCombo->GetTextAsExpression( rRes, &bIsValid );
					if ( !bIsValid ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_78D93AF7_No_changed_by_value_has_been_specified_ );
					}
					else if ( !bParsed ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
					}
					else {
						CUtilities::ToDataTypeString( rRes, dtDataType, wsStr1, true );
						rResLow = rRes;

						if ( !pcbRCombo ) {
							wText = mx::CStringDecoder::DecodeToWString( _T_LEN_14F91708_The_range_of_values_to_find_is_incomplete_ );
						}
						else {
							bParsed = pcbRCombo->GetTextAsExpression( rRes, &bIsValid );
							if ( !bIsValid ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_14F91708_The_range_of_values_to_find_is_incomplete_ );
							}
							else if ( !bParsed ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
							}
							else {
								rResHi = rRes;

								CSecureWString wsNegation = pcbInvertCheck && pcbInvertCheck->IsChecked() ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
								wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_6E2313CE_Find__s_values_that_have__schanged_by_between__s_and__s_ );
								CUtilities::PrintDataType( wsStr0, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );
								CUtilities::ToDataTypeString( rRes, dtDataType, wsStr2 );

								WCHAR wszBuffer[512];
								std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsStr0.c_str(), wsNegation.c_str(), wsStr1.c_str(), wsStr2.c_str() );

								wText = wszBuffer;

								if ( m_bLastTypeExpressionWasEmpty ) {
									CSecureString ssExact;
									CreateChangedByRangeExpression( ssExact, rResLow, rResHi, dtDataType, pcbInvertCheck->IsChecked() );
									pcbExpCombo->SetTextA( ssExact.c_str() );
								}
							}
						}
					}
					break;
				}
				case CUtilities::MX_SET_CHANGED_BY_RANGE_PERCENT : {
					CSecureWString wsStr0, wsStr1, wsStr2;
					BOOL bParsed = pcbLCombo->GetTextAsExpression( rRes, &bIsValid );
					if ( !bIsValid ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_78D93AF7_No_changed_by_value_has_been_specified_ );
					}
					else if ( !bParsed ) {
						wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
					}
					else {
						CUtilities::ToDataTypeString( rRes, CUtilities::MX_DT_DOUBLE, wsStr1, true );
						rResLow = rRes;

						if ( !pcbRCombo ) {
							wText = mx::CStringDecoder::DecodeToWString( _T_LEN_14F91708_The_range_of_values_to_find_is_incomplete_ );
						}
						else {
							bParsed = pcbRCombo->GetTextAsExpression( rRes, &bIsValid );
							if ( !bIsValid ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_14F91708_The_range_of_values_to_find_is_incomplete_ );
							}
							else if ( !bParsed ) {
								wText = mx::CStringDecoder::DecodeToWString( _T_LEN_83B9CBC3_Invalid_Data );
							}
							else {
								rResHi = rRes;

								CSecureWString wsNegation = pcbInvertCheck && pcbInvertCheck->IsChecked() ? mx::CStringDecoder::DecodeToWString( _T_LEN_C90B4F0B_not_ ) : L"";
								wsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_D0582611_Find__s_values_that_have__sbeen_multiplied_by_between__s_and__s_ );
								CUtilities::PrintDataType( wsStr0, dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );
								CUtilities::ToDataTypeString( rRes, CUtilities::MX_DT_DOUBLE, wsStr2 );

								WCHAR wszBuffer[512];
								//std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsStr0.c_str(), wsNegation.c_str(), wsStr1.c_str(), wsStr2.c_str() );
								std::swprintf( wszBuffer, MX_ELEMENTS( wszBuffer ), wsTemp.c_str(), wsStr0.c_str(), wsNegation.c_str(), wsStr1.c_str(), wsStr2.c_str() );

								wText = wszBuffer;

								if ( m_bLastTypeExpressionWasEmpty ) {
									CSecureString ssExact;
									rResLow = ee::CExpEvalContainer::ConvertResult( rResLow, ee::EE_NC_FLOATING );
									rResHi = ee::CExpEvalContainer::ConvertResult( rResHi, ee::EE_NC_FLOATING );
									CreateChangedByRangePercentExpression( ssExact, rResLow, rResHi, dtDataType, pcbInvertCheck->IsChecked() );
									pcbExpCombo->SetTextA( ssExact.c_str() );
								}
							}
						}
					}
					break;
				}
				case CUtilities::MX_SET_QUICK_EXP : {
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
				}

			}

			if ( plLabel ) {
				plLabel->SetTextW( wText.c_str() );
			}
		}
	}

	// Creates an expression for searching for exact values.
	void CStandardSubsearchWindow::CreateExactValueExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
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
			_sExpression.append( CUtilities::WStringToString( swsLeft ) );
			_sExpression.push_back( ',' );
			_sExpression.push_back( ' ' );

			swsLeft.SecureClear();
			CUtilities::ToDouble( m_pmhMemHack->Options().dEpsilon, swsLeft );
			_sExpression.append( CUtilities::WStringToString( swsLeft ) );

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
					CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft, true );
					_sExpression.append( CUtilities::WStringToString( swsLeft ) );
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
					CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft, true );
					_sExpression = _DEC_S_BA988A07_______;
					_sExpression.append( CUtilities::WStringToString( swsLeft ) );
				}
			}
		}

		if ( bIsFloat && _bInv && !bUsedNan ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// Creates an expression for searching for not-equal-to values.
	void CStandardSubsearchWindow::CreateNotEqualToExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		int32_t iIsInf = CUtilities::DataTypeIsInf( _rLVal, _dtDataType );
		bool bIsFloat = CUtilities::DataTypeIsFloat( _dtDataType );
		bool bUsedNan = false;
		if ( m_pmhMemHack->Options().bUseEpsilon && bIsFloat ) {
			_sExpression = "!";
			if ( _dtDataType == CUtilities::MX_DT_DOUBLE ) {
				if ( m_pmhMemHack->Options().bSmartEpsilon ) {
					_sExpression += _DEC_S_6015A3A2_repsilon______;
				}
				else {
					_sExpression += _DEC_S_B5DBEB0F_epsilon______;
				}
			}
			else {
				if ( m_pmhMemHack->Options().bSmartEpsilon ) {
					_sExpression += _DEC_S_79D6C61D_repsilonf______;
				}
				else {
					_sExpression += _DEC_S_D164D700_epsilonf______;
				}
			}
			CSecureWString swsLeft;
			CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft, true );
			_sExpression.append( CUtilities::WStringToString( swsLeft ) );
			_sExpression.push_back( ',' );
			_sExpression.push_back( ' ' );

			swsLeft.SecureClear();
			CUtilities::ToDouble( m_pmhMemHack->Options().dEpsilon, swsLeft );
			_sExpression.append( CUtilities::WStringToString( swsLeft ) );

			_sExpression.push_back( ' ' );
			_sExpression.push_back( ')' );
		}
		else {
			if ( bIsFloat && _bInv ) {
				if ( iIsInf > 0 ) {
					_sExpression = "!";
					_sExpression += _DEC_S_FAD085A8_isinf______;
				}
				else if ( iIsInf < 0 ) {
					_sExpression = "!";
					_sExpression += _DEC_S_94DF1758_isinf_______;
				}
				else if ( CUtilities::DataTypeIsNan( _rLVal, _dtDataType ) ) {
					_sExpression = "!";
					_sExpression += _DEC_S_747EB4AA_isnan______;
					bUsedNan = true;
				}
				else {
					_sExpression = _DEC_S_AFA4D013_______;
					CSecureWString swsLeft;
					CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft, true );
					_sExpression.append( CUtilities::WStringToString( swsLeft ) );
				}
			}
			else {
				if ( iIsInf > 0 ) {
					_sExpression = "!";
					_sExpression += _DEC_S_FAD085A8_isinf______;
				}
				else if ( iIsInf < 0 ) {
					_sExpression = "!";
					_sExpression += _DEC_S_94DF1758_isinf_______;
				}
				else if ( CUtilities::DataTypeIsNan( _rLVal, _dtDataType ) ) {
					_sExpression = "!";
					_sExpression += _DEC_S_747EB4AA_isnan______;
				}
				else {
					CSecureWString swsLeft;
					CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft, true );
					_sExpression = _DEC_S_AFA4D013_______;
					_sExpression.append( CUtilities::WStringToString( swsLeft ) );
				}
			}
		}

		if ( bIsFloat && _bInv && !bUsedNan ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// Creates an expression for searching for increased values.
	void CStandardSubsearchWindow::CreateIncreasedExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &/*_rLVal*/, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		bool bIsFloat = CUtilities::DataTypeIsFloat( _dtDataType );
		bool bUsedNan = false;
		_sExpression = _DEC_S_082288C1_______0;

		if ( bIsFloat && _bInv && !bUsedNan ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// Creates an expression for searching for decreased values.
	void CStandardSubsearchWindow::CreateDecreasedExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &/*_rLVal*/, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		bool bIsFloat = CUtilities::DataTypeIsFloat( _dtDataType );
		bool bUsedNan = false;
		_sExpression = _DEC_S_A22B404A_______0;

		if ( bIsFloat && _bInv && !bUsedNan ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// Creates an expression for searching for ranged values.
	void CStandardSubsearchWindow::CreateRangeExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal,
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
		_sExpression.append( CUtilities::WStringToString( swsLeft ) );
		_sExpression.append( _DEC_S_C4C57451_____________ );
		_sExpression.append( CUtilities::WStringToString( swsRight ) );
		_sExpression.push_back( ')' );
		if ( CUtilities::DataTypeIsFloat( _dtDataType ) && _bInv ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// Creates an expression for searching for same-as-before values.
	void CStandardSubsearchWindow::CreateSameAsBeforeExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &/*_rLVal*/, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		bool bIsFloat = CUtilities::DataTypeIsFloat( _dtDataType );
		bool bUsedNan = false;
		_sExpression = _DEC_S_D4C48594________0;

		if ( bIsFloat && _bInv && !bUsedNan ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// Creates an expression for searching for same-as-before values.
	void CStandardSubsearchWindow::CreateDifferentFromBeforeExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &/*_rLVal*/, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		bool bIsFloat = CUtilities::DataTypeIsFloat( _dtDataType );
		bool bUsedNan = false;
		_sExpression = _DEC_S_71D4FF17________0;

		if ( bIsFloat && _bInv && !bUsedNan ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// Creates an expression for searching for changed-by values.
	void CStandardSubsearchWindow::CreateChangedByExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		int32_t iIsInf = CUtilities::DataTypeIsInf( _rLVal, _dtDataType );
		bool bIsFloat = CUtilities::DataTypeIsFloat( _dtDataType );
		bool bUsedNan = false;
		if ( m_pmhMemHack->Options().bUseEpsilon && bIsFloat ) {
			if ( _dtDataType == CUtilities::MX_DT_DOUBLE ) {
				if ( m_pmhMemHack->Options().bSmartEpsilon ) {
					_sExpression = _DEC_S_0B192472_repsilon_________0___;
				}
				else {
					_sExpression = _DEC_S_89C101CC_epsilon_________0___;
				}
			}
			else {
				if ( m_pmhMemHack->Options().bSmartEpsilon ) {
					_sExpression = _DEC_S_06B90FCC_repsilonf_________0___;
				}
				else {
					_sExpression = _DEC_S_2AE24962_epsilonf_________0___;
				}
			}
			CSecureWString swsLeft;
			CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft, true );
			_sExpression.append( CUtilities::WStringToString( swsLeft ) );
			_sExpression.push_back( ',' );
			_sExpression.push_back( ' ' );

			swsLeft.SecureClear();
			CUtilities::ToDouble( m_pmhMemHack->Options().dEpsilon, swsLeft );
			_sExpression.append( CUtilities::WStringToString( swsLeft ) );

			_sExpression.push_back( ' ' );
			_sExpression.push_back( ')' );
		}
		else {
			_sExpression = _DEC_S_D0ED2A1B________0_____;
			CSecureWString swsLeft;
			CUtilities::ToDataTypeString( _rLVal, _dtDataType, swsLeft, true );
			_sExpression.append( CUtilities::WStringToString( swsLeft ) );
		}

		if ( bIsFloat && _bInv && !bUsedNan ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// Creates an expression for searching for changed-by-percent values.
	void CStandardSubsearchWindow::CreateChangedByPercentExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		int32_t iIsInf = CUtilities::DataTypeIsInf( _rLVal, CUtilities::MX_DT_DOUBLE );
		bool bIsFloat = CUtilities::DataTypeIsFloat( _dtDataType );
		bool bUsedNan = false;
		if ( m_pmhMemHack->Options().bUseEpsilon && bIsFloat ) {
			if ( _dtDataType == CUtilities::MX_DT_DOUBLE ) {
				if ( m_pmhMemHack->Options().bSmartEpsilon ) {
					_sExpression = _DEC_S_68331048__0____repsilon________0__;
				}
				else {
					_sExpression = _DEC_S_A29DA168__0____epsilon________0__;
				}
			}
			else {
				if ( m_pmhMemHack->Options().bSmartEpsilon ) {
					_sExpression = _DEC_S_15C0F892__0____repsilonf________0__;
				}
				else {
					_sExpression = _DEC_S_2E6476EB__0____epsilonf________0__;
				}
			}
			CSecureWString swsLeft;
			CUtilities::ToDataTypeString( _rLVal, CUtilities::MX_DT_DOUBLE, swsLeft, true );
			_sExpression.append( CUtilities::WStringToString( swsLeft ) );
			_sExpression.push_back( ',' );
			_sExpression.push_back( ' ' );

			swsLeft.SecureClear();
			CUtilities::ToDouble( m_pmhMemHack->Options().dEpsilon, swsLeft );
			_sExpression.append( CUtilities::WStringToString( swsLeft ) );

			_sExpression.push_back( ' ' );
			_sExpression.push_back( ')' );
		}
		else {
			// 1 cast will cause the whole division to be done in double.
			// 2 casts will have the same result but add 1 extra node of processing, so ever-so-slightly slower.
			_sExpression = _DEC_S_D0DAC171__0______double_______0_____;
			CSecureWString swsLeft;
			CUtilities::ToDataTypeString( _rLVal, CUtilities::MX_DT_DOUBLE, swsLeft, true );
			_sExpression.append( CUtilities::WStringToString( swsLeft ) );
		}

		if ( bIsFloat && _bInv && !bUsedNan ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// Creates an expression for searching for changed-by-range values.
	void CStandardSubsearchWindow::CreateChangedByRangeExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal,
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
		
		_sExpression = _DEC_S_EFDD3592_Tmp__________0____Tmp____;
		_sExpression.append( CUtilities::WStringToString( swsLeft ) );
		_sExpression.append( _DEC_S_E1223E80_____Tmp____ );
		_sExpression.append( CUtilities::WStringToString( swsRight ) );
		_sExpression.push_back( ')' );
		if ( CUtilities::DataTypeIsFloat( _dtDataType ) && _bInv ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_36A7406E____isnan______ );
		}
	}

	// Creates an expression for searching for changed-by-range-percent values.
	void CStandardSubsearchWindow::CreateChangedByRangePercentExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal,
		const ee::CExpEvalContainer::EE_RESULT &_rRVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv ) {
		ee::CExpEvalContainer::EE_RESULT rLow;
		ee::CExpEvalContainer::EE_RESULT rHi;
		ee::CExpEvalContainer::EE_RESULT rTempL, rTempH;
		rLow = ee::CExpEvalContainer::ConvertResult( _rLVal, ee::EE_NC_FLOATING );
		rHi = ee::CExpEvalContainer::ConvertResult( _rRVal, ee::EE_NC_FLOATING );

		rTempL.ncType = ee::EE_NC_FLOATING;
		rTempL.u.dVal = std::min( rLow.u.dVal, rHi.u.dVal );
		rTempH.ncType = ee::EE_NC_FLOATING;
		rTempH.u.dVal = std::max( rLow.u.dVal, rHi.u.dVal );

		CSecureWString swsLeft, swsRight;
		CUtilities::ToDataTypeString( rTempL, CUtilities::MX_DT_DOUBLE, swsLeft, true );
		CUtilities::ToDataTypeString( rTempH, CUtilities::MX_DT_DOUBLE, swsRight, true );
		
		if ( CUtilities::DataTypeIsFloat( _dtDataType ) ) {
			// No casting necessary if it is already a floating-point type (which will be stored internally as doubles).
			_sExpression = _DEC_S_2D02F861_if____0_____Tmp_________0___Tmp____;
		}
		else {
			// 1 cast will cause the whole division to be done in double.
			// 2 casts will have the same result but add 1 extra node of processing, so ever-so-slightly slower.
			_sExpression = _DEC_S_B7B76940_if____0_____Tmp____double_______0___Tmp____;
		}
		_sExpression.append( CUtilities::WStringToString( swsLeft ) );
		_sExpression.append( _DEC_S_E1223E80_____Tmp____ );
		_sExpression.append( CUtilities::WStringToString( swsRight ) );
		_sExpression.push_back( ')' );
		if ( CUtilities::DataTypeIsFloat( _dtDataType ) && _bInv ) {
			_sExpression.push_back( ' ' );
			_sExpression.append( _DEC_S_08148872____isnan__Tmp__ );
			_sExpression.append( _DEC_S_44D7E135_____else___isnan_________ );
		}
		else {
			_sExpression.append( _DEC_S_7627B364_____else___false___ );
		}
	}

}	// namespace mx

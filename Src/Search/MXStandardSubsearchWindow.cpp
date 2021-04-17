#include "MXStandardSubsearchWindow.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXStandardSubsearchLayout.h"
#include "../MainWindow/MXMhsMainWindow.h"


namespace mx {

	// == Members.
	CStandardSubsearchWindow::MX_SEARCH_WIN_DATA CStandardSubsearchWindow::m_swdData = {
		CUtilities::MX_ET_EXACT,				// uiEvalType
	};

	CStandardSubsearchWindow::CStandardSubsearchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmhMemHack( reinterpret_cast<CMemHack *>(_ui64Data) ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CStandardSubsearchWindow::InitDialog() {
		CComboBox * pcbCombo;
		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_TYPE_COMBO ));
		if ( pcbCombo ) {
			static const CUtilities::MX_SUB_EVAL_TYPES stTypes[] = {
				CUtilities::MX_SET_EXACT,
				CUtilities::MX_SET_INCREASED,
				CUtilities::MX_SET_DECREASED,
				CUtilities::MX_SET_RANGE,
				CUtilities::MX_SET_SAME_AS_BEFORE,
				CUtilities::MX_SET_DIFFERENT_FROM_BEFORE,
				CUtilities::MX_SET_CHANGED_BY,
				CUtilities::MX_SET_CHANGED_BY_PERCENT,
				CUtilities::MX_SET_SAME_AS_ORIGINAL,
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
		
		/*
		if ( m_pmhMemHack ) {
			CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CStandardSubsearchLayout::MX_C_ALIGNED ));
			if ( pcbCheck ) {
				pcbCheck->CheckButton( m_pmhMemHack->Options().bAligned ? BST_CHECKED : BST_UNCHECKED );
			}

			pcbCheck = static_cast<CCheckButton *>(FindChild( CStandardSubsearchLayout::MX_C_INVERT ));
			if ( pcbCheck ) {
				pcbCheck->CheckButton( m_pmhMemHack->Options().bInvertResults ? BST_CHECKED : BST_UNCHECKED );
			}

			pcbCheck = static_cast<CCheckButton *>(FindChild( CStandardSubsearchLayout::MX_C_SAO_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->CheckButton( m_pmhMemHack->Options().bSameAsOriginal ? BST_CHECKED : BST_UNCHECKED );
			}
		}
		
		
		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_LVAL_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vLValHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vLValHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsLValText.c_str() );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_RVAL_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vRValHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vRValHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsRValText.c_str() );
			//pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_EXP_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vExpHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vExpHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsExpText.c_str() );
			//pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_FROM_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vFromHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vFromHistory[I].c_str() );
			}
			if ( m_swdData.wsFromText.size() == 0 ) {
				uintptr_t uiptrTemp = reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMinimumApplicationAddress);
				std::string sTemp = CUtilities::ToHex( uiptrTemp, 4 );
				pcbCombo->SetTextA( sTemp.c_str() );
			}
			else {
				pcbCombo->SetTextW( m_swdData.wsFromText.c_str() );
			}
			pcbCombo->SetTreatAsHex( TRUE );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_TO_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vToHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vToHistory[I].c_str() );
			}
			if ( m_swdData.wsToText.size() == 0 ) {
				uintptr_t uiptrTemp = reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMaximumApplicationAddress);
				std::string sTemp = CUtilities::ToHex( uiptrTemp + 1ULL, 4 );
				pcbCombo->SetTextA( sTemp.c_str() );
			}
			else {
				pcbCombo->SetTextW( m_swdData.wsToText.c_str() );
			}
			pcbCombo->SetTreatAsHex( TRUE );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}


		CButton * pbButton = static_cast<CButton *>(FindChild( CStandardSubsearchLayout::MX_C_GENERAL_SEARCH_OPTIONS_BUTTON ));
		if ( pbButton ) {
			pbButton->SetEnabled( m_pmhMemHack != nullptr );
		}
		*/
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
	}

	// Validates the dialog box.
	bool CStandardSubsearchWindow::DialogContentsAreValid( CSecureWString &_wsError, CSearcher::MX_SEARCH_PARMS &_spParmsIfValid ) {
		_spParmsIfValid.stType = CUtilities::MX_ST_DATATYPE_SEARCH;
		_spParmsIfValid.pbAbort = nullptr;
		_spParmsIfValid.iThreadPriority = THREAD_PRIORITY_NORMAL;
		::ZeroMemory( &_spParmsIfValid.dtLVal, sizeof( _spParmsIfValid.dtLVal ) );
		::ZeroMemory( &_spParmsIfValid.dtRVal, sizeof( _spParmsIfValid.dtRVal ) );
		_spParmsIfValid.ssExpression.clear();

		return false;
	}

	// Updates the dialog.
	void CStandardSubsearchWindow::UpdateDialog() {
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CStandardSubsearchLayout::MX_C_SEARCH_TYPE_COMBO ));
	}

}	// namespace mx

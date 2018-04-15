#include "MXNewDataTypeSearchWindow.h"
#include "../Layouts/MXNewDataTypeSearchLayout.h"
#include "../Utilities/MXUtilities.h"
#include <algorithm>
#include <Button/LSWButton.h>
#include <CheckButton/LSWCheckButton.h>
#include <ComboBox/LSWComboBox.h>


namespace mx {

	// == Members.
	CNewDataTypeSearchWindow::MX_SEARCH_WIN_DATA CNewDataTypeSearchWindow::m_swdData = {
		CUtilities::MX_DT_UINT32,				// uiDataType
		CUtilities::MX_ET_EXACT,				// uiEvalType
	};

	CNewDataTypeSearchWindow::CNewDataTypeSearchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, MX_OPTIONS * _poOptions, bool _bCreateWidget, HMENU _hMenu ) :
		CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu ),
		m_poOptions( _poOptions ) {
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
				CUtilities::MX_DT_FLOAT,
				CUtilities::MX_DT_DOUBLE
			};
			for ( size_t I = 0; I < MX_ELEMENTS( dtTypes ); ++I ) {
				std::wstring sTemp;
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
				CUtilities::MX_ET_NOT_EQUAL_TO,
				CUtilities::MX_ET_GREATER_THAN,
				CUtilities::MX_ET_LESS_THAN,
				CUtilities::MX_ET_RANGE,
				CUtilities::MX_ET_UNKNOWN,
			};
			for ( size_t I = 0; I < MX_ELEMENTS( stTypes ); ++I ) {
				std::wstring sTemp;
				CUtilities::EvaluationTypeToString( stTypes[I], sTemp );
				INT iIndex = pcbCombo->AddString( sTemp.c_str() );
				pcbCombo->SetItemData( iIndex, stTypes[I] );
			}
			pcbCombo->AutoSetMinListWidth();

			pcbCombo->SetCurSelByItemData( m_swdData.uiEvalType );
		}
		
		if ( m_poOptions ) {
			CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_ALIGNED ));
			if ( pcbCheck ) {
				pcbCheck->CheckButton( m_poOptions->bAligned ? BST_CHECKED : BST_UNCHECKED );
			}

			pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_SAO_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->CheckButton( m_poOptions->bSameAsOriginal ? BST_CHECKED : BST_UNCHECKED );
			}
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
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_FROM_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vFromHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vFromHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsFromText.c_str() );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_TO_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vToHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vToHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsToText.c_str() );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}


		CButton * pbButton = static_cast<CButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_GENERAL_SEARCH_OPTIONS_BUTTON ));
		if ( pbButton ) {
			pbButton->SetEnabled( m_poOptions != nullptr );
		}


		return CMainWindow::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CNewDataTypeSearchWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CNewDataTypeSearchLayout::MX_NDSI_CANCEL : {
				return Close();
			}
			case CNewDataTypeSearchLayout::MX_NDSI_OK : {
				SaveDialogData( m_swdData );
				return Close();
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
		if ( m_poOptions ) {
			CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_ALIGNED ));
			if ( pcbCheck ) {
				m_poOptions->bAligned = pcbCheck->IsChecked();
			}
			pcbCheck = static_cast<CCheckButton *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_SAO_CHECK ));
			if ( pcbCheck ) {
				m_poOptions->bSameAsOriginal = pcbCheck->IsChecked();
			}
		}


		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_DATA_TYPE_COMBO ));
		if ( pcbCombo ) {
			_swdData.uiDataType = pcbCombo->GetItemData( pcbCombo->GetCurSel() );
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_EVALUATION_TYPE_COMBO ));
		if ( pcbCombo ) {
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
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_FROM_COMBO ));
		if ( pcbCombo ) {
			_swdData.wsFromText = pcbCombo->GetTextW();
			if ( _swdData.wsFromText.size() ) {
				auto aFound = std::find( _swdData.vFromHistory.begin(), _swdData.vFromHistory.end(), _swdData.wsFromText );
				if ( aFound != _swdData.vFromHistory.end() ) {
					_swdData.vFromHistory.erase( aFound );
				}
				_swdData.vFromHistory.insert( _swdData.vFromHistory.begin(), _swdData.wsFromText );
			}
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewDataTypeSearchLayout::MX_NDSI_TO_COMBO ));
		if ( pcbCombo ) {
			_swdData.wsToText = pcbCombo->GetTextW();
			if ( _swdData.wsToText.size() ) {
				auto aFound = std::find( _swdData.vToHistory.begin(), _swdData.vToHistory.end(), _swdData.wsToText );
				if ( aFound != _swdData.vToHistory.end() ) {
					_swdData.vToHistory.erase( aFound );
				}
				_swdData.vToHistory.insert( _swdData.vToHistory.begin(), _swdData.wsToText );
			}
		}
	}

}	// namespace mx

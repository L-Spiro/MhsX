#include "MXNumberInputDialog.h"

#include <Object/EEObject.h>


namespace mx {

	CNumberInputDialog::CNumberInputDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pndpParms( reinterpret_cast<CNumberInputLayout::MX_NUMBER_DIALOG_PARMS *>(_ui64Data) ) {
	}
	CNumberInputDialog::~CNumberInputDialog() {
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.
	 * \brief Initializes a dialog after creation and before it is shown.
	 *
	 * Use to set control defaults, attach data bindings, and position the dialog.
	 *
	 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
	 */
	CWidget::LSW_HANDLED CNumberInputDialog::InitDialog() {
		if ( m_pndpParms ) {
			SetTextW( m_pndpParms->swsHeaderText.c_str() );
			auto pwLabel = FindChild( CNumberInputLayout::MX_NI_LABEL );
			if ( pwLabel ) {
				pwLabel->SetTextW( m_pndpParms->swsLabelText.c_str() );
			}
			auto pwCombo = FindChild( CNumberInputLayout::MX_NI_COMBO );
			if ( pwCombo ) {
				if ( m_pndpParms->vComboItems.size() ) {
					CUtilities::FillComboBox( pwCombo, m_pndpParms->vComboItems.data(), m_pndpParms->vComboItems.size(),
						m_pndpParms->vComboItems[0].lpParm, 0 );
				}
				pwCombo->SetTextW( m_pndpParms->swsDefaultText.c_str() );
			}
		}
		return lsw::CMainWindow::InitDialog();
	}

	/**
	 * Handles WM_CLOSE.
	 * \brief Requests window closure and gives a chance to veto or prompt the user.
	 *
	 * Use to confirm unsaved changes or to cancel the close. Post WM_DESTROY if approved.
	 *
	 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
	 */
	CWidget::LSW_HANDLED CNumberInputDialog::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND dispatch.
	 * \brief Routes command notifications from menus, accelerators, or child controls.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
	 * \param _wId The identifier for the command or control.
	 * \param _pwSrc The source control for control notifications; otherwise nullptr.
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CNumberInputDialog::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case CNumberInputLayout::MX_NI_OK : {
				if ( !m_pndpParms ) {
					// Return simply OK/Cancel information.
					::EndDialog( Wnd(), TRUE );
				}
				else {
					// Evaluate the expression.
					BOOL bValid = FALSE;
					auto pwCombo = FindChild( CNumberInputLayout::MX_NI_COMBO );
					if ( !pwCombo ) {
						auto swsTitle = _DEC_WS_A756CF10_Internal_Error;
						auto swsError = _DEC_WS_1468A1DF_Internal_error_;
						lsw::CBase::MessageBoxError( Wnd(), swsError.c_str(), swsTitle.c_str() );
						::EndDialog( Wnd(), -1 );
						break;
					}
					pwCombo->SetTreatAsHex( m_pndpParms->bHexInput );
					if ( !pwCombo->GetTextAsExpression( m_pndpParms->rResult, ee::CObject::EE_TF_NONE, &bValid, m_pndpParms->psStringReturn ) ) {
						auto swsTitle = _DEC_WS_9607671B_Invalid_Expression;
						auto swsError = _DEC_WS_7605BFA8_The_given_expression_is_invalid_;
						lsw::CBase::MessageBoxError( Wnd(), swsError.c_str(), swsTitle.c_str() );
						break;
					}
					if ( !bValid ) {
						auto swsTitle = _DEC_WS_EBFDD36E_Unable_to_Resolve;
						auto swsError = _DEC_WS_26C9377F_The_given_expression_is_valid_but_unable_to_be_resolved_;
						lsw::CBase::MessageBoxError( Wnd(), swsError.c_str(), swsTitle.c_str() );
						break;
					}
					if ( m_pndpParms->pfVerifyFunc ) {
						m_pndpParms->pwThis = this;
						if ( !m_pndpParms->pfVerifyFunc( m_pndpParms->pvVerifyParm, (*m_pndpParms) ) ) {
							break;
						}
					}
					m_pndpParms->swsExpression = pwCombo->GetTextW();
					::EndDialog( Wnd(), TRUE );
				}
				break;
			}
			case CNumberInputLayout::MX_NI_CANCEL : {
				::EndDialog( Wnd(), FALSE );
				break;
			}
		}
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND from a menu.
	 * \brief Invoked for menu command selections.
	 *
	 * \param _wId The menu command identifier.
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CNumberInputDialog::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {}								MX_FALLTHROUGH
			case CNumberInputLayout::MX_NI_CANCEL : {
				return Close();
			}
		}
		
		return CParent::MenuCommand( _wId );
	}

}	// namespace mx

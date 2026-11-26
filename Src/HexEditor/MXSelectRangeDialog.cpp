#include "MXSelectRangeDialog.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"

#include <Object/EEObject.h>


namespace mx {

	CSelectRangeDialog::CSelectRangeDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_psrpParms( reinterpret_cast<CSelectRangeLayout::MX_SELECT_RANGE_PARMS *>(_ui64Data) ) {
	}
	CSelectRangeDialog::~CSelectRangeDialog() {
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.  Fills in the default start address and size.
	 *
	 * \return Returns LSW_H_CONTINUE to allow default processing.
	 **/
	CWidget::LSW_HANDLED CSelectRangeDialog::InitDialog() {
		if ( m_psrpParms ) {
			m_bEnd = m_psrpParms->bEnd;
			SetValue( CSelectRangeLayout::MX_SRI_START_EDIT, m_psrpParms->ui64Start );
			// The end address of an empty range is shown as the start address.
			SetValue( CSelectRangeLayout::MX_SRI_VALUE_EDIT, m_bEnd ?
				(m_psrpParms->ui64Start + (m_psrpParms->ui64Size ? m_psrpParms->ui64Size - 1 : 0)) :
				m_psrpParms->ui64Size );
			auto pwSize = FindChild( CSelectRangeLayout::MX_SRI_SIZE_RADIO );
			auto pwEnd = FindChild( CSelectRangeLayout::MX_SRI_END_RADIO );
			if ( pwSize ) { pwSize->SetCheck( !m_bEnd ); }
			if ( pwEnd ) { pwEnd->SetCheck( m_bEnd ); }
		}
		auto pwStart = FindChild( CSelectRangeLayout::MX_SRI_START_EDIT );
		if ( pwStart ) {
			pwStart->SetFocus();
			::SendMessageW( pwStart->Wnd(), EM_SETSEL, 0, -1 );
		}
		CParent::InitDialog();
		// The focus was set here.
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_CLOSE.  Closes the dialog as Cancel.
	 *
	 * \return Returns LSW_H_HANDLED.
	 **/
	CWidget::LSW_HANDLED CSelectRangeDialog::Close() {
		::EndDialog( Wnd(), FALSE );
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND.  OK checks and returns the values; Cancel closes the dialog; the radio buttons change the meaning of the
	 *	second value (converting it).
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
	 * \param _wId The identifier for the command or control.
	 * \param _pwSrc The source control for control notifications; otherwise nullptr.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CSelectRangeDialog::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case CSelectRangeLayout::MX_SRI_SIZE_RADIO : {
				SetEndMode( false );
				break;
			}
			case CSelectRangeLayout::MX_SRI_END_RADIO : {
				SetEndMode( true );
				break;
			}
			case CSelectRangeLayout::MX_SRI_OK : {
				if ( !m_psrpParms ) {
					::EndDialog( Wnd(), FALSE );
					break;
				}
				uint64_t ui64Start = 0, ui64Value = 0;
				if ( !GetValue( CSelectRangeLayout::MX_SRI_START_EDIT, ui64Start, true ) ) { break; }
				if ( !GetValue( CSelectRangeLayout::MX_SRI_VALUE_EDIT, ui64Value, true ) ) { break; }
				if ( ui64Start < m_psrpParms->ui64Bias || ui64Start - m_psrpParms->ui64Bias > m_psrpParms->ui64DataSize ) {
					lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_D08434A5_Address_out_of_range_.c_str(), _DEC_WS_191780FA_Select_Range.c_str() );
					auto pwStart = FindChild( CSelectRangeLayout::MX_SRI_START_EDIT );
					if ( pwStart ) { pwStart->SetFocus(); }
					break;
				}
				uint64_t ui64Size = ui64Value;
				if ( m_bEnd ) {
					if ( ui64Value < ui64Start ) {
						lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_D647650C_The_end_address_must_not_be_less_than_the_start_address_.c_str(), _DEC_WS_191780FA_Select_Range.c_str() );
						auto pwValue = FindChild( CSelectRangeLayout::MX_SRI_VALUE_EDIT );
						if ( pwValue ) { pwValue->SetFocus(); }
						break;
					}
					// Inclusive (selecting up to the last address possible cannot overflow, since the range is cut short at the end of the data).
					ui64Size = (ui64Value - ui64Start == UINT64_MAX) ? UINT64_MAX : (ui64Value - ui64Start + 1);
				}
				m_psrpParms->ui64Start = ui64Start;
				m_psrpParms->ui64Size = ui64Size;
				m_psrpParms->bEnd = m_bEnd;
				::EndDialog( Wnd(), TRUE );
				break;
			}
			case CSelectRangeLayout::MX_SRI_CANCEL : {
				::EndDialog( Wnd(), FALSE );
				break;
			}
		}
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND from a menu (Escape).
	 *
	 * \param _wId The menu command identifier.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CSelectRangeDialog::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {}								MX_FALLTHROUGH
			case CSelectRangeLayout::MX_SRI_CANCEL : {
				return Close();
			}
		}
		return CParent::MenuCommand( _wId );
	}

	/**
	 * Evaluates the expression in an edit control as an unsigned number, showing an error if it is not one.
	 *
	 * \param _wId The edit control.
	 * \param _ui64Value Receives the value.
	 * \param _bShowErrors If true, errors are shown in a message box and the control is given the focus.
	 * \return Returns true if the text is an unsigned number.
	 **/
	bool CSelectRangeDialog::GetValue( WORD _wId, uint64_t &_ui64Value, bool _bShowErrors ) {
		auto pwEdit = FindChild( _wId );
		if ( !pwEdit ) { return false; }
		pwEdit->SetTreatAsHex( m_psrpParms && m_psrpParms->bHex );
		ee::CExpEvalContainer::EE_RESULT rResult;
		BOOL bValid = FALSE;
		CSecureWString swsTitle, swsError;
		if ( !pwEdit->GetTextAsExpression( rResult, ee::CObject::EE_TF_NONE, &bValid ) ) {
			swsTitle = _DEC_WS_9607671B_Invalid_Expression;
			swsError = _DEC_WS_7605BFA8_The_given_expression_is_invalid_;
		}
		else if ( !bValid ) {
			swsTitle = _DEC_WS_EBFDD36E_Unable_to_Resolve;
			swsError = _DEC_WS_26C9377F_The_given_expression_is_valid_but_unable_to_be_resolved_;
		}
		else {
			auto rCast = ee::CExpEvalContainer::ConvertResult( rResult, ee::EE_NC_UNSIGNED );
			if ( rCast.ncType == ee::EE_NC_UNSIGNED ) {
				_ui64Value = rCast.u.ui64Val;
				return true;
			}
			swsTitle = _DEC_WS_9607671B_Invalid_Expression;
			swsError = _DEC_WS_16D063D0_The_given_expression_must_evaluate_to_an_unsigned_integer_;
		}
		if ( _bShowErrors ) {
			lsw::CBase::MessageBoxError( Wnd(), swsError.c_str(), swsTitle.c_str() );
			pwEdit->SetFocus();
			::SendMessageW( pwEdit->Wnd(), EM_SETSEL, 0, -1 );
		}
		return false;
	}

	/**
	 * Puts a number in an edit control, in hexadecimal or decimal.
	 *
	 * \param _wId The edit control.
	 * \param _ui64Value The value.
	 **/
	void CSelectRangeDialog::SetValue( WORD _wId, uint64_t _ui64Value ) {
		auto pwEdit = FindChild( _wId );
		if ( !pwEdit ) { return; }
		try {
			std::wstring wsText;
			if ( m_psrpParms && m_psrpParms->bHex ) {
				// Without a prefix, since the edit treats numbers as hexadecimal.
				const char * pcHex = CUtilities::ToHex( _ui64Value, 1, false );
				for ( const char * pcThis = pcHex; pcThis && (*pcThis); ++pcThis ) { wsText.push_back( wchar_t( (*pcThis) ) ); }
			}
			else {
				wsText = std::to_wstring( _ui64Value );
			}
			pwEdit->SetTextW( wsText.c_str() );
		}
		catch ( ... ) {}
	}

	/**
	 * Switches the second value between a size and an end address, converting what has been entered if it can be read.
	 *
	 * \param _bEnd If true, the second value becomes the (inclusive) end address.
	 **/
	void CSelectRangeDialog::SetEndMode( bool _bEnd ) {
		if ( _bEnd == m_bEnd ) { return; }
		uint64_t ui64Start = 0, ui64Value = 0;
		if ( GetValue( CSelectRangeLayout::MX_SRI_START_EDIT, ui64Start, false ) && GetValue( CSelectRangeLayout::MX_SRI_VALUE_EDIT, ui64Value, false ) ) {
			if ( _bEnd ) {
				// Size -> end address.
				SetValue( CSelectRangeLayout::MX_SRI_VALUE_EDIT, ui64Start + (ui64Value ? ui64Value - 1 : 0) );
			}
			else if ( ui64Value >= ui64Start ) {
				// End address -> size.
				SetValue( CSelectRangeLayout::MX_SRI_VALUE_EDIT, ui64Value - ui64Start + 1 );
			}
		}
		m_bEnd = _bEnd;
	}

}	// namespace mx

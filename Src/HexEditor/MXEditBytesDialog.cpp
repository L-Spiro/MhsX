#include "MXEditBytesDialog.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"

#include <Object/EEObject.h>

#include <cwchar>
#include <format>


namespace mx {

	CEditBytesDialog::CEditBytesDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pebpParms( reinterpret_cast<CEditBytesLayout::MX_EDIT_BYTES_PARMS *>(_ui64Data) ) {
	}
	CEditBytesDialog::~CEditBytesDialog() {
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.  Fills in the default values.
	 *
	 * \return Returns LSW_H_HANDLED (the focus is set here).
	 **/
	CWidget::LSW_HANDLED CEditBytesDialog::InitDialog() {
		if ( m_pebpParms ) {
			m_bEnd = m_pebpParms->bEnd;
			m_bHex = m_pebpParms->bHex;
			if ( m_pebpParms->ebmMode == CEditBytesLayout::MX_EBM_OVERWRITE ) {
				SetTextW( _DEC_WS_15877231_Overwrite_Bytes.c_str() );
			}
			auto pwHex = FindChild( CEditBytesLayout::MX_EBI_HEX_RADIO );
			auto pwDec = FindChild( CEditBytesLayout::MX_EBI_DEC_RADIO );
			if ( pwHex ) { pwHex->SetCheck( m_bHex ); }
			if ( pwDec ) { pwDec->SetCheck( !m_bHex ); }
			if ( m_pebpParms->ebmMode == CEditBytesLayout::MX_EBM_SET_SIZE ) {
				SetValue( CEditBytesLayout::MX_EBI_START_EDIT, m_pebpParms->ui64DataSize );
				auto pwInfo = FindChild( CEditBytesLayout::MX_EBI_INFO_LABEL );
				if ( pwInfo ) {
					try {
						std::wstring wsTmp;
						std::wstring wsInfo = _DEC_WS_2C827E06_Current_size__;
						wsInfo += std::to_wstring( m_pebpParms->ui64DataSize );
						wsInfo += L' ';
						wsInfo += L'[';
						wsInfo += CUtilities::ToHex( m_pebpParms->ui64DataSize, wsTmp, 0 );
						wsInfo += L']';
						wsInfo += (m_pebpParms->ui64DataSize != 1) ? _DEC_WS_C1D51046__bytes : _DEC_WS_BB679D6B__byte;
						pwInfo->SetTextW( wsInfo.c_str() );
					}
					catch ( ... ) {}
				}
			}
			else {
				SetValue( CEditBytesLayout::MX_EBI_START_EDIT, m_pebpParms->ui64Start + m_pebpParms->ui64Bias );
				SetValue( CEditBytesLayout::MX_EBI_VALUE_EDIT, m_bEnd ?
					(m_pebpParms->ui64Start + m_pebpParms->ui64Bias + (m_pebpParms->ui64Size ? m_pebpParms->ui64Size - 1 : 0)) :
					m_pebpParms->ui64Size );
				auto pwSize = FindChild( CEditBytesLayout::MX_EBI_SIZE_RADIO );
				auto pwEnd = FindChild( CEditBytesLayout::MX_EBI_END_RADIO );
				if ( pwSize ) { pwSize->SetCheck( !m_bEnd ); }
				if ( pwEnd ) { pwEnd->SetCheck( m_bEnd ); }
			}
			WriteByteValue( m_pebpParms->ui8Value, 0 );
		}
		auto pwFirst = FindChild( (m_pebpParms && m_pebpParms->ebmMode != CEditBytesLayout::MX_EBM_SET_SIZE) ?
			CEditBytesLayout::MX_EBI_VALUE_EDIT : CEditBytesLayout::MX_EBI_START_EDIT );
		if ( pwFirst ) {
			pwFirst->SetFocus();
			::SendMessageW( pwFirst->Wnd(), EM_SETSEL, 0, -1 );
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
	CWidget::LSW_HANDLED CEditBytesDialog::Close() {
		::EndDialog( Wnd(), FALSE );
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND.  OK checks and returns the values; Cancel closes the dialog; the radio buttons convert what has been
	 *	entered; the byte-value edits update each other.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
	 * \param _wId The identifier for the command or control.
	 * \param _pwSrc The source control for control notifications; otherwise nullptr.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CEditBytesDialog::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case CEditBytesLayout::MX_EBI_SIZE_RADIO : {
				SetEndMode( false );
				break;
			}
			case CEditBytesLayout::MX_EBI_END_RADIO : {
				SetEndMode( true );
				break;
			}
			case CEditBytesLayout::MX_EBI_HEX_RADIO : {
				SetHexMode( true );
				break;
			}
			case CEditBytesLayout::MX_EBI_DEC_RADIO : {
				SetHexMode( false );
				break;
			}
			case CEditBytesLayout::MX_EBI_BV_HEX_EDIT : {}				MX_FALLTHROUGH
			case CEditBytesLayout::MX_EBI_BV_DEC_EDIT : {}				MX_FALLTHROUGH
			case CEditBytesLayout::MX_EBI_BV_CHAR_EDIT : {
				if ( _wCtrlCode == EN_CHANGE && !m_bSyncing ) {
					uint8_t ui8Value;
					if ( ReadByteValue( _wId, ui8Value ) ) { WriteByteValue( ui8Value, _wId ); }
				}
				break;
			}
			case CEditBytesLayout::MX_EBI_OK : {
				if ( !m_pebpParms ) {
					::EndDialog( Wnd(), FALSE );
					break;
				}
				uint8_t ui8Value = 0;
				if ( !ReadByteValue( CEditBytesLayout::MX_EBI_BV_HEX_EDIT, ui8Value ) ) {
					ShowError( _DEC_WS_D116139C_The_byte_value_must_be_from_0_to_255__0_to_FF_in_hexadecimal__, CEditBytesLayout::MX_EBI_BV_HEX_EDIT );
					break;
				}
				if ( m_pebpParms->ebmMode == CEditBytesLayout::MX_EBM_SET_SIZE ) {
					uint64_t ui64Size = 0;
					if ( !GetValue( CEditBytesLayout::MX_EBI_START_EDIT, ui64Size, true ) ) { break; }
					// Growing by a huge amount is most likely a typing mistake.
					constexpr uint64_t ui64Large = 256ULL * 1024ULL * 1024ULL;
					if ( ui64Size > m_pebpParms->ui64DataSize && ui64Size - m_pebpParms->ui64DataSize > ui64Large ) {
						try {
							std::wstring wsMsg = _DEC_WS_C1F24E40_This_adds_;
							wsMsg += std::to_wstring( ui64Size - m_pebpParms->ui64DataSize );
							wsMsg += _DEC_WS_F089B95D__bytes_to_the_file___Continue_;
							if ( !lsw::CBase::PromptYesNo( Wnd(), wsMsg.c_str(), GetTextW().c_str() ) ) { break; }
						}
						catch ( ... ) { break; }
					}
					m_pebpParms->ui64Size = ui64Size;
				}
				else {
					uint64_t ui64Start = 0, ui64Value = 0;
					if ( !GetValue( CEditBytesLayout::MX_EBI_START_EDIT, ui64Start, true ) ) { break; }
					if ( !GetValue( CEditBytesLayout::MX_EBI_VALUE_EDIT, ui64Value, true ) ) { break; }
					if ( ui64Start < m_pebpParms->ui64Bias || ui64Start - m_pebpParms->ui64Bias > m_pebpParms->ui64DataSize ) {
						ShowError( _DEC_WS_D08434A5_Address_out_of_range_, CEditBytesLayout::MX_EBI_START_EDIT );
						break;
					}
					uint64_t ui64Size = ui64Value;
					if ( m_bEnd ) {
						if ( ui64Value < ui64Start ) {
							ShowError( _DEC_WS_D647650C_The_end_address_must_not_be_less_than_the_start_address_, CEditBytesLayout::MX_EBI_VALUE_EDIT );
							break;
						}
						if ( ui64Value - ui64Start == UINT64_MAX ) {
							ShowError( _DEC_WS_42D1E83E_The_range_is_too_large_, CEditBytesLayout::MX_EBI_VALUE_EDIT );
							break;
						}
						ui64Size = ui64Value - ui64Start + 1;
					}
					if ( !ui64Size ) {
						ShowError( _DEC_WS_B88DBC00_The_size_must_not_be_0_, CEditBytesLayout::MX_EBI_VALUE_EDIT );
						break;
					}
					const uint64_t ui64Offset = ui64Start - m_pebpParms->ui64Bias;
					if ( m_pebpParms->ebmMode == CEditBytesLayout::MX_EBM_OVERWRITE && !m_pebpParms->bCanGrow &&
						ui64Size > m_pebpParms->ui64DataSize - ui64Offset ) {
						ShowError( _DEC_WS_88F4BBE1_The_range_goes_past_the_end_of_the_data_, CEditBytesLayout::MX_EBI_VALUE_EDIT );
						break;
					}
					constexpr uint64_t ui64Large = 256ULL * 1024ULL * 1024ULL;
					const uint64_t ui64Added = (m_pebpParms->ebmMode == CEditBytesLayout::MX_EBM_INSERT) ? ui64Size :
						((ui64Size > m_pebpParms->ui64DataSize - ui64Offset) ? ui64Size - (m_pebpParms->ui64DataSize - ui64Offset) : 0);
					if ( ui64Added > ui64Large ) {
						try {
							std::wstring wsMsg = _DEC_WS_C1F24E40_This_adds_;
							wsMsg += std::to_wstring( ui64Added );
							wsMsg += _DEC_WS_F089B95D__bytes_to_the_file___Continue_;
							if ( !lsw::CBase::PromptYesNo( Wnd(), wsMsg.c_str(), GetTextW().c_str() ) ) { break; }
						}
						catch ( ... ) { break; }
					}
					m_pebpParms->ui64Start = ui64Offset;
					m_pebpParms->ui64Size = ui64Size;
					m_pebpParms->bEnd = m_bEnd;
				}
				m_pebpParms->bHex = m_bHex;
				m_pebpParms->ui8Value = ui8Value;
				::EndDialog( Wnd(), TRUE );
				break;
			}
			case CEditBytesLayout::MX_EBI_CANCEL : {
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
	CWidget::LSW_HANDLED CEditBytesDialog::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {}								MX_FALLTHROUGH
			case CEditBytesLayout::MX_EBI_CANCEL : {
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
	bool CEditBytesDialog::GetValue( WORD _wId, uint64_t &_ui64Value, bool _bShowErrors ) {
		auto pwEdit = FindChild( _wId );
		if ( !pwEdit ) { return false; }
		pwEdit->SetTreatAsHex( m_bHex );
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
	void CEditBytesDialog::SetValue( WORD _wId, uint64_t _ui64Value ) {
		auto pwEdit = FindChild( _wId );
		if ( !pwEdit ) { return; }
		try {
			std::wstring wsText;
			if ( m_bHex ) {
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
	void CEditBytesDialog::SetEndMode( bool _bEnd ) {
		if ( _bEnd == m_bEnd ) { return; }
		uint64_t ui64Start = 0, ui64Value = 0;
		if ( GetValue( CEditBytesLayout::MX_EBI_START_EDIT, ui64Start, false ) && GetValue( CEditBytesLayout::MX_EBI_VALUE_EDIT, ui64Value, false ) ) {
			if ( _bEnd ) {
				// Size -> end address.
				SetValue( CEditBytesLayout::MX_EBI_VALUE_EDIT, ui64Start + (ui64Value ? ui64Value - 1 : 0) );
			}
			else if ( ui64Value >= ui64Start ) {
				// End address -> size.
				SetValue( CEditBytesLayout::MX_EBI_VALUE_EDIT, ui64Value - ui64Start + 1 );
			}
		}
		m_bEnd = _bEnd;
	}

	/**
	 * Switches numbers between hexadecimal and decimal, converting what has been entered if it can be read.
	 *
	 * \param _bHex If true, numbers are entered in hexadecimal.
	 **/
	void CEditBytesDialog::SetHexMode( bool _bHex ) {
		if ( _bHex == m_bHex ) { return; }
		const WORD wIds[] = { CEditBytesLayout::MX_EBI_START_EDIT, CEditBytesLayout::MX_EBI_VALUE_EDIT };
		uint64_t ui64Values[std::size( wIds )];
		bool bRead[std::size( wIds )];
		for ( size_t I = 0; I < std::size( wIds ); ++I ) {
			bRead[I] = FindChild( wIds[I] ) && GetValue( wIds[I], ui64Values[I], false );
		}
		m_bHex = _bHex;
		for ( size_t I = 0; I < std::size( wIds ); ++I ) {
			if ( bRead[I] ) { SetValue( wIds[I], ui64Values[I] ); }
		}
	}

	/**
	 * Reads the byte value from one of its edits.
	 *
	 * \param _wId The edit (hexadecimal, decimal, or character).
	 * \param _ui8Value Receives the value.
	 * \return Returns true if the edit holds a byte value.
	 **/
	bool CEditBytesDialog::ReadByteValue( WORD _wId, uint8_t &_ui8Value ) const {
		const CWidget * pwEdit = const_cast<CEditBytesDialog *>(this)->FindChild( _wId );
		if ( !pwEdit ) { return false; }
		try {
			std::wstring wsText = pwEdit->GetTextW();
			if ( _wId == CEditBytesLayout::MX_EBI_BV_CHAR_EDIT ) {
				// The first character, as a Latin-1 code.
				if ( wsText.empty() || uint32_t( wsText[0] ) > 0xFF ) { return false; }
				_ui8Value = uint8_t( wsText[0] );
				return true;
			}
			// Spaces around the number are allowed.
			while ( wsText.size() && ::iswspace( wsText.back() ) ) { wsText.pop_back(); }
			size_t sStart = 0;
			while ( sStart < wsText.size() && ::iswspace( wsText[sStart] ) ) { ++sStart; }
			if ( sStart == wsText.size() ) { return false; }
			const int iBase = (_wId == CEditBytesLayout::MX_EBI_BV_HEX_EDIT) ? 16 : 10;
			uint32_t ui32Value = 0;
			for ( size_t I = sStart; I < wsText.size(); ++I ) {
				const wchar_t wcThis = wsText[I];
				uint32_t ui32Digit;
				if ( wcThis >= L'0' && wcThis <= L'9' ) { ui32Digit = uint32_t( wcThis - L'0' ); }
				else if ( iBase == 16 && wcThis >= L'a' && wcThis <= L'f' ) { ui32Digit = uint32_t( wcThis - L'a' + 10 ); }
				else if ( iBase == 16 && wcThis >= L'A' && wcThis <= L'F' ) { ui32Digit = uint32_t( wcThis - L'A' + 10 ); }
				else { return false; }
				ui32Value = ui32Value * uint32_t( iBase ) + ui32Digit;
				if ( ui32Value > 0xFF ) { return false; }
			}
			_ui8Value = uint8_t( ui32Value );
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Puts the byte value into its edits, except the one being typed into.
	 *
	 * \param _ui8Value The value.
	 * \param _wSkip The edit not to change, or 0.
	 **/
	void CEditBytesDialog::WriteByteValue( uint8_t _ui8Value, WORD _wSkip ) {
		m_bSyncing = true;
		try {
			CWidget * pwEdit;
			if ( _wSkip != CEditBytesLayout::MX_EBI_BV_HEX_EDIT && (pwEdit = FindChild( CEditBytesLayout::MX_EBI_BV_HEX_EDIT )) ) {
				const wchar_t wcDigits[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };
				const wchar_t wcText[] = { wcDigits[_ui8Value>>4], wcDigits[_ui8Value&0xF], L'\0' };
				pwEdit->SetTextW( wcText );
			}
			if ( _wSkip != CEditBytesLayout::MX_EBI_BV_DEC_EDIT && (pwEdit = FindChild( CEditBytesLayout::MX_EBI_BV_DEC_EDIT )) ) {
				pwEdit->SetTextW( std::to_wstring( _ui8Value ).c_str() );
			}
			if ( _wSkip != CEditBytesLayout::MX_EBI_BV_CHAR_EDIT && (pwEdit = FindChild( CEditBytesLayout::MX_EBI_BV_CHAR_EDIT )) ) {
				const bool bPrintable = (_ui8Value >= 0x20 && _ui8Value < 0x7F) || _ui8Value >= 0xA0;
				const wchar_t wcText[] = { bPrintable ? wchar_t( _ui8Value ) : L'\0', L'\0' };
				pwEdit->SetTextW( wcText );
			}
		}
		catch ( ... ) {}
		m_bSyncing = false;
	}

	/**
	 * Shows an error and gives a control the focus.
	 *
	 * \param _wsMsg The message.
	 * \param _wId The control.
	 **/
	void CEditBytesDialog::ShowError( const std::wstring &_wsMsg, WORD _wId ) {
		try {
			lsw::CBase::MessageBoxError( Wnd(), _wsMsg.c_str(), GetTextW().c_str() );
		}
		catch ( ... ) {}
		auto pwThis = FindChild( _wId );
		if ( pwThis ) {
			pwThis->SetFocus();
			::SendMessageW( pwThis->Wnd(), EM_SETSEL, 0, -1 );
		}
	}

}	// namespace mx

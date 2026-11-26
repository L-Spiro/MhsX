#include "MXFindDialog.h"
#include "../Strings/MXStringMacros.h"

#include <Button/LSWColorButton.h>
#include <ComboBox/LSWComboBox.h>

#include <cmath>
#include <sstream>


namespace mx {

	/** The most search texts kept in the history. */
	#define MX_FD_HISTORY											32

	CFindDialog::CFindDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pfdpParms( reinterpret_cast<CFindLayout::MX_FIND_DIALOG_PARMS *>(_ui64Data) ) {
	}
	CFindDialog::~CFindDialog() {
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.  Fills in the settings.
	 *
	 * \return Returns LSW_H_CONTINUE to allow default processing.
	 **/
	CWidget::LSW_HANDLED CFindDialog::InitDialog() {
		if ( m_pfdpParms && m_pfdpParms->pfpParms ) {
			m_bFilling = true;
			try {
				const CHexFind::MX_FIND_PARMS & fpParms = (*m_pfdpParms->pfpParms);

				auto pcbFind = static_cast<lsw::CComboBox *>(FindChild( CFindLayout::MX_FI_FIND_COMBO ));
				if ( pcbFind ) {
					if ( m_pfdpParms->pvHistory ) {
						for ( const auto & swsThis : (*m_pfdpParms->pvHistory) ) { pcbFind->AddString( swsThis.c_str() ); }
					}
					pcbFind->SetTextW( fpParms.swsQuery.c_str() );
					pcbFind->SetSel( 0, -1 );
				}

				auto pcbTypes = static_cast<lsw::CComboBox *>(FindChild( CFindLayout::MX_FI_TYPE_COMBO ));
				if ( pcbTypes ) {
					for ( uint32_t I = 0; I < CHexFind::MX_FT_TOTAL; ++I ) {
						const INT iIdx = pcbTypes->AddString( CHexFind::TypeName( CHexFind::MX_FIND_TYPE( I ) ).c_str() );
						if ( iIdx >= 0 ) { pcbTypes->SetItemData( iIdx, static_cast<LPARAM>(I) ); }
					}
					pcbTypes->SetCurSelByItemData( static_cast<LPARAM>(fpParms.ftType) );
					::SendMessageW( pcbTypes->Wnd(), CB_SETMINVISIBLE, static_cast<WPARAM>(CHexFind::MX_FT_TOTAL), 0 );
				}

				auto pwTol = FindChild( CFindLayout::MX_FI_TOLERANCE_EDIT );
				if ( pwTol ) {
					std::wostringstream wossTol;
					wossTol << fpParms.dTolerance;
					pwTol->SetTextW( wossTol.str().c_str() );
				}

				SetChecked( CFindLayout::MX_FI_FIND_ALL_OCCURRENCES, fpParms.bFindAll );
				SetChecked( CFindLayout::MX_FI_MATCH_CASE, fpParms.bMatchCase );
				SetChecked( CFindLayout::MX_FI_WHOLE_WORD, fpParms.bWholeWord );
				SetChecked( CFindLayout::MX_FI_REGEX, fpParms.bRegex );
				SetChecked( CFindLayout::MX_FI_WILDCARDS, fpParms.bWildcards );
				SetChecked( CFindLayout::MX_FI_MULTIPLE_RANGES, fpParms.bMultipleRanges );
				SetChecked( CFindLayout::MX_FI_UP, fpParms.bUp );
				SetChecked( CFindLayout::MX_FI_DOWN, !fpParms.bUp );
				SetChecked( CFindLayout::MX_FI_LOCK_TO_SELECTION, fpParms.bLockToSelection );
				SetChecked( CFindLayout::MX_FI_INCREMENTAL, fpParms.bIncremental );
				SetChecked( CFindLayout::MX_FI_WRAP, fpParms.bWrap );
				SetChecked( CFindLayout::MX_FI_TYPE_SPECIFIERS, fpParms.bTypeSpecifiers );
				SetChecked( CFindLayout::MX_FI_HIDE_AFTER, fpParms.bHideAfterSearch );
				SetChecked( CFindLayout::MX_FI_QUICK_FIND, fpParms.bQuickFind );
				SetChecked( CFindLayout::MX_FI_CUSTOM_COLOR, fpParms.bCustomColor );
				SetChecked( CFindLayout::MX_FI_FORE_CHECK, fpParms.bCustomFore );
				SetChecked( CFindLayout::MX_FI_LIMIT, fpParms.bLimit );

				auto pcbFore = static_cast<lsw::CColorButton *>(FindChild( CFindLayout::MX_FI_FORE_COLOR ));
				if ( pcbFore ) { pcbFore->SetColor( fpParms.crFore ); }
				auto pcbBack = static_cast<lsw::CColorButton *>(FindChild( CFindLayout::MX_FI_BACK_COLOR ));
				if ( pcbBack ) { pcbBack->SetColor( fpParms.crBack ); }

				auto pwLimit = FindChild( CFindLayout::MX_FI_LIMIT_EDIT );
				if ( pwLimit ) { pwLimit->SetTextW( std::to_wstring( fpParms.ui64Limit ).c_str() ); }
				auto pwWild = FindChild( CFindLayout::MX_FI_MAX_WILDCARD_EDIT );
				if ( pwWild ) { pwWild->SetTextW( std::to_wstring( fpParms.ui32MaxWildcard ).c_str() ); }
			}
			catch ( ... ) {}
			m_bFilling = false;
			UpdateControls();
		}
		return CParent::InitDialog();
	}

	/**
	 * Handles WM_CLOSE.  Keeps the settings and closes the dialog.
	 *
	 * \return Returns LSW_H_HANDLED.
	 **/
	CWidget::LSW_HANDLED CFindDialog::Close() {
		CSecureWString swsError;
		Gather( swsError );
		::EndDialog( Wnd(), FALSE );
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
	 * \param _wId The identifier for the command or control.
	 * \param _pwSrc The source control for control notifications; otherwise nullptr.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CFindDialog::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		if ( m_bFilling ) { return LSW_H_CONTINUE; }
		switch ( _wId ) {
			case CFindLayout::MX_FI_FIND_COMBO : {
				// Incremental Search: searching starts again from where it began each time the text changes.
				if ( _wCtrlCode == CBN_EDITCHANGE && Checked( CFindLayout::MX_FI_INCREMENTAL ) ) { DoAction( CFindLayout::MX_FA_INCREMENTAL ); }
				break;
			}
			case CFindLayout::MX_FI_TYPE_COMBO : {
				if ( _wCtrlCode == CBN_SELCHANGE ) { UpdateControls(); }
				break;
			}
			case CFindLayout::MX_FI_REGEX : {}							MX_FALLTHROUGH
			case CFindLayout::MX_FI_CUSTOM_COLOR : {}					MX_FALLTHROUGH
			case CFindLayout::MX_FI_FORE_CHECK : {}						MX_FALLTHROUGH
			case CFindLayout::MX_FI_LIMIT : {}							MX_FALLTHROUGH
			case CFindLayout::MX_FI_LOCK_TO_SELECTION : {
				UpdateControls();
				break;
			}
			case IDOK : {}												MX_FALLTHROUGH
			case CFindLayout::MX_FI_FIND : {
				// The same as pressing Enter in 010 Editor's Find Bar.
				if ( Checked( CFindLayout::MX_FI_FIND_ALL_OCCURRENCES ) ) { DoAction( CFindLayout::MX_FA_ALL ); }
				else { DoAction( Checked( CFindLayout::MX_FI_UP ) ? CFindLayout::MX_FA_PREV : CFindLayout::MX_FA_NEXT ); }
				break;
			}
			case CFindLayout::MX_FI_FIND_NEXT : {
				DoAction( CFindLayout::MX_FA_NEXT );
				break;
			}
			case CFindLayout::MX_FI_FIND_PREV : {
				DoAction( CFindLayout::MX_FA_PREV );
				break;
			}
			case CFindLayout::MX_FI_FIND_ALL : {
				DoAction( CFindLayout::MX_FA_ALL );
				break;
			}
			case IDCANCEL : {}											MX_FALLTHROUGH
			case CFindLayout::MX_FI_CLOSE : {
				return Close();
			}
		}
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND from a menu (Enter and Escape).
	 *
	 * \param _wId The menu command identifier.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CFindDialog::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDOK : {
				return Command( BN_CLICKED, CFindLayout::MX_FI_FIND, nullptr );
			}
			case IDCANCEL : {}											MX_FALLTHROUGH
			case CFindLayout::MX_FI_CLOSE : {
				return Close();
			}
		}
		return CParent::MenuCommand( _wId );
	}

	/**
	 * Does a search.
	 *
	 * \param _faAction What to do.
	 **/
	void CFindDialog::DoAction( CFindLayout::MX_FIND_ACTION _faAction ) {
		if ( !m_pfdpParms || !m_pfdpParms->pfpParms || !m_pfdpParms->fAction ) { return; }
		CSecureWString swsStatus;
		if ( !Gather( swsStatus ) ) {
			SetStatus( swsStatus );
			return;
		}
		// Incremental searches are not added to the history (every letter typed would be).
		if ( _faAction != CFindLayout::MX_FA_INCREMENTAL ) { AddHistory( m_pfdpParms->pfpParms->swsQuery ); }
		if ( _faAction == CFindLayout::MX_FA_INCREMENTAL && m_pfdpParms->pfpParms->swsQuery.empty() ) {
			SetStatus( CSecureWString() );
			return;
		}
		const bool bFound = m_pfdpParms->fAction( _faAction, swsStatus );
		SetStatus( swsStatus );
		if ( bFound && _faAction == CFindLayout::MX_FA_ALL && m_pfdpParms->pfpParms->bHideAfterSearch ) {
			::EndDialog( Wnd(), TRUE );
		}
	}

	/**
	 * Copies the settings from the controls to the parameters.
	 *
	 * \param _swsError Receives a description of the problem on failure.
	 * \return Returns true if every setting is valid.
	 **/
	bool CFindDialog::Gather( CSecureWString &_swsError ) {
		if ( !m_pfdpParms || !m_pfdpParms->pfpParms ) { return false; }
		CHexFind::MX_FIND_PARMS & fpParms = (*m_pfdpParms->pfpParms);
		try {
			auto pwFind = FindChild( CFindLayout::MX_FI_FIND_COMBO );
			if ( pwFind ) { fpParms.swsQuery = pwFind->GetTextW(); }
			fpParms.ftType = SelectedType();
			fpParms.bFindAll = Checked( CFindLayout::MX_FI_FIND_ALL_OCCURRENCES );
			fpParms.bMatchCase = Checked( CFindLayout::MX_FI_MATCH_CASE );
			fpParms.bWholeWord = Checked( CFindLayout::MX_FI_WHOLE_WORD );
			fpParms.bRegex = Checked( CFindLayout::MX_FI_REGEX );
			fpParms.bWildcards = Checked( CFindLayout::MX_FI_WILDCARDS );
			fpParms.bMultipleRanges = Checked( CFindLayout::MX_FI_MULTIPLE_RANGES );
			fpParms.bUp = Checked( CFindLayout::MX_FI_UP );
			fpParms.bIncremental = Checked( CFindLayout::MX_FI_INCREMENTAL );
			fpParms.bWrap = Checked( CFindLayout::MX_FI_WRAP );
			fpParms.bTypeSpecifiers = Checked( CFindLayout::MX_FI_TYPE_SPECIFIERS );
			fpParms.bHideAfterSearch = Checked( CFindLayout::MX_FI_HIDE_AFTER );
			fpParms.bQuickFind = Checked( CFindLayout::MX_FI_QUICK_FIND );
			fpParms.bCustomColor = Checked( CFindLayout::MX_FI_CUSTOM_COLOR );
			fpParms.bCustomFore = Checked( CFindLayout::MX_FI_FORE_CHECK );
			fpParms.bLimit = Checked( CFindLayout::MX_FI_LIMIT );
			auto pcbFore = static_cast<lsw::CColorButton *>(FindChild( CFindLayout::MX_FI_FORE_COLOR ));
			if ( pcbFore ) { fpParms.crFore = pcbFore->GetColor(); }
			auto pcbBack = static_cast<lsw::CColorButton *>(FindChild( CFindLayout::MX_FI_BACK_COLOR ));
			if ( pcbBack ) { fpParms.crBack = pcbBack->GetColor(); }

			// Locking uses the selection the dialog was opened with.
			const bool bLock = Checked( CFindLayout::MX_FI_LOCK_TO_SELECTION );
			if ( bLock && !fpParms.bLockToSelection && m_pfdpParms->bHasSelection ) {
				fpParms.ui64LockStart = m_pfdpParms->ui64SelStart;
				fpParms.ui64LockSize = m_pfdpParms->ui64SelSize;
			}
			fpParms.bLockToSelection = bLock && fpParms.ui64LockSize;

			uint64_t ui64Val;
			if ( ReadNumber( CFindLayout::MX_FI_MAX_WILDCARD_EDIT, ui64Val ) && ui64Val ) {
				fpParms.ui32MaxWildcard = uint32_t( std::min<uint64_t>( ui64Val, 0xFFFF ) );
			}
			if ( fpParms.bLimit ) {
				if ( !ReadNumber( CFindLayout::MX_FI_LIMIT_EDIT, ui64Val ) || !ui64Val ) {
					_swsError = _DEC_WS_C977E3B3_The_limit_must_be_at_least_1_;
					return false;
				}
				fpParms.ui64Limit = ui64Val;
			}
			if ( fpParms.ftType == CHexFind::MX_FT_FLOAT || fpParms.ftType == CHexFind::MX_FT_DOUBLE ) {
				auto pwTol = FindChild( CFindLayout::MX_FI_TOLERANCE_EDIT );
				if ( pwTol ) {
					const std::wstring wsTol = pwTol->GetTextW();
					wchar_t * pwcEnd = nullptr;
					const double dTol = wsTol.empty() ? 0.0 : std::wcstod( wsTol.c_str(), &pwcEnd );
					if ( (!wsTol.empty() && (!pwcEnd || (*pwcEnd) != L'\0')) || !std::isfinite( dTol ) || dTol < 0.0 ) {
						_swsError = _DEC_WS_055C5BB0_The_tolerance_is_not_valid_;
						return false;
					}
					fpParms.dTolerance = dTol;
				}
			}
			return true;
		}
		catch ( ... ) {
			_swsError = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * Adds the search text to the top of the history.
	 *
	 * \param _swsText The text.
	 **/
	void CFindDialog::AddHistory( const CSecureWString &_swsText ) {
		if ( _swsText.empty() || !m_pfdpParms || !m_pfdpParms->pvHistory ) { return; }
		try {
			auto & vHistory = (*m_pfdpParms->pvHistory);
			auto aFound = std::find( vHistory.begin(), vHistory.end(), _swsText );
			if ( aFound == vHistory.begin() ) { return; }
			if ( aFound != vHistory.end() ) { vHistory.erase( aFound ); }
			vHistory.insert( vHistory.begin(), _swsText );
			if ( vHistory.size() > MX_FD_HISTORY ) { vHistory.resize( MX_FD_HISTORY ); }

			// The list shows the new order; the text being edited stays as it is.
			auto pcbFind = static_cast<lsw::CComboBox *>(FindChild( CFindLayout::MX_FI_FIND_COMBO ));
			if ( pcbFind ) {
				const std::wstring wsText = pcbFind->GetTextW();
				m_bFilling = true;
				pcbFind->ResetContent();
				for ( const auto & swsThis : vHistory ) { pcbFind->AddString( swsThis.c_str() ); }
				pcbFind->SetTextW( wsText.c_str() );
				m_bFilling = false;
			}
		}
		catch ( ... ) { m_bFilling = false; }
	}

	/**
	 * Enables the options that apply to the selected type and settings and disables the rest.
	 **/
	void CFindDialog::UpdateControls() {
		const CHexFind::MX_FIND_TYPE ftType = SelectedType();
		const bool bText = CHexFind::IsText( ftType );
		const bool bRegex = bText && Checked( CFindLayout::MX_FI_REGEX );
		Enable( CFindLayout::MX_FI_MATCH_CASE, bText );
		Enable( CFindLayout::MX_FI_WHOLE_WORD, bText );
		Enable( CFindLayout::MX_FI_REGEX, bText && ftType != CHexFind::MX_FT_EBCDIC );
		Enable( CFindLayout::MX_FI_WILDCARDS, (bText || ftType == CHexFind::MX_FT_HEX) && !bRegex );
		Enable( CFindLayout::MX_FI_MAX_WILDCARD_LABEL, (bText || ftType == CHexFind::MX_FT_HEX) && !bRegex );
		Enable( CFindLayout::MX_FI_MAX_WILDCARD_EDIT, (bText || ftType == CHexFind::MX_FT_HEX) && !bRegex );

		const bool bFloat = ftType == CHexFind::MX_FT_FLOAT || ftType == CHexFind::MX_FT_DOUBLE;
		Enable( CFindLayout::MX_FI_TOLERANCE_LABEL, bFloat );
		Enable( CFindLayout::MX_FI_TOLERANCE_EDIT, bFloat );

		const bool bCustom = Checked( CFindLayout::MX_FI_CUSTOM_COLOR );
		Enable( CFindLayout::MX_FI_FORE_CHECK, bCustom );
		Enable( CFindLayout::MX_FI_FORE_COLOR, bCustom && Checked( CFindLayout::MX_FI_FORE_CHECK ) );
		Enable( CFindLayout::MX_FI_BACK_LABEL, bCustom );
		Enable( CFindLayout::MX_FI_BACK_COLOR, bCustom );
		Enable( CFindLayout::MX_FI_LIMIT_EDIT, Checked( CFindLayout::MX_FI_LIMIT ) );

		// Lock to Selection needs a selection (or a lock already in place).
		const bool bLocked = m_pfdpParms && m_pfdpParms->pfpParms && m_pfdpParms->pfpParms->bLockToSelection;
		Enable( CFindLayout::MX_FI_LOCK_TO_SELECTION, (m_pfdpParms && m_pfdpParms->bHasSelection) || bLocked );
	}

	/**
	 * Gets the type selected in the list.
	 *
	 * \return Returns the type.
	 **/
	CHexFind::MX_FIND_TYPE CFindDialog::SelectedType() {
		auto pcbTypes = static_cast<lsw::CComboBox *>(FindChild( CFindLayout::MX_FI_TYPE_COMBO ));
		if ( !pcbTypes ) { return CHexFind::MX_FT_HEX; }
		const LPARAM lpType = pcbTypes->GetCurSelItemData();
		return (lpType >= 0 && lpType < LPARAM( CHexFind::MX_FT_TOTAL )) ? CHexFind::MX_FIND_TYPE( lpType ) : CHexFind::MX_FT_HEX;
	}

	/**
	 * Shows text under the search text.
	 *
	 * \param _swsText The text.
	 **/
	void CFindDialog::SetStatus( const CSecureWString &_swsText ) {
		auto pwStatus = FindChild( CFindLayout::MX_FI_STATUS );
		if ( pwStatus ) { pwStatus->SetTextW( _swsText.c_str() ); }
	}

	/**
	 * Determines whether a radio button or check box is checked.
	 *
	 * \param _wId The ID of the control.
	 * \return Returns true if the control exists and is checked.
	 **/
	bool CFindDialog::Checked( WORD _wId ) {
		auto pwThis = FindChild( _wId );
		return pwThis && pwThis->IsChecked();
	}

	/**
	 * Checks or unchecks a radio button or check box.
	 *
	 * \param _wId The ID of the control.
	 * \param _bCheck Whether to check it.
	 **/
	void CFindDialog::SetChecked( WORD _wId, bool _bCheck ) {
		auto pwThis = FindChild( _wId );
		if ( pwThis ) { pwThis->SetCheck( _bCheck ); }
	}

	/**
	 * Enables or disables a control.
	 *
	 * \param _wId The ID of the control.
	 * \param _bEnable If true, the control is enabled.
	 **/
	void CFindDialog::Enable( WORD _wId, bool _bEnable ) {
		auto pwThis = FindChild( _wId );
		if ( pwThis ) { ::EnableWindow( pwThis->Wnd(), _bEnable ? TRUE : FALSE ); }
	}

	/**
	 * Reads a whole number from an edit control.
	 *
	 * \param _wId The ID of the control.
	 * \param _ui64Val Receives the number.
	 * \return Returns true if the control holds a valid number.
	 **/
	bool CFindDialog::ReadNumber( WORD _wId, uint64_t &_ui64Val ) {
		auto pwThis = FindChild( _wId );
		if ( !pwThis ) { return false; }
		const std::wstring wsText = pwThis->GetTextW();
		if ( wsText.empty() ) { return false; }
		_ui64Val = 0;
		for ( wchar_t wcChar : wsText ) {
			if ( wcChar < L'0' || wcChar > L'9' ) { return false; }
			if ( _ui64Val > (UINT64_MAX - 9) / 10 ) { return false; }
			_ui64Val = _ui64Val * 10 + uint64_t( wcChar - L'0' );
		}
		return true;
	}

}	// namespace mx

#include "MXExportHexDialog.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"

#include <ComboBox/LSWComboBox.h>
#include <Object/EEObject.h>

#include <commdlg.h>
#include <filesystem>


namespace mx {

	CExportHexDialog::CExportHexDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pehpParms( reinterpret_cast<CExportHexLayout::MX_EXPORT_HEX_PARMS *>(_ui64Data) ) {
	}
	CExportHexDialog::~CExportHexDialog() {
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.  Fills in the defaults.
	 *
	 * \return Returns LSW_H_CONTINUE to allow default processing.
	 **/
	CWidget::LSW_HANDLED CExportHexDialog::InitDialog() {
		if ( m_pehpParms ) {
			try {
				// "Export Hex - <name>".
				SetTextW( (_DEC_WS_55F71713_Export_Hex___ + m_pehpParms->wsSourceName).c_str() );

				auto pwFile = FindChild( CExportHexLayout::MX_EHI_FILE_EDIT );
				if ( pwFile ) { pwFile->SetTextW( m_pehpParms->wsPath.c_str() ); }

				auto pcbTypes = static_cast<lsw::CComboBox *>(FindChild( CExportHexLayout::MX_EHI_TYPE_COMBO ));
				if ( pcbTypes ) {
					size_t sTotal = 0;
					const CExportHexLayout::MX_EXPORT_TYPE * petTypes = CExportHexLayout::Types( sTotal );
					for ( size_t I = 0; I < sTotal; ++I ) {
						const INT iIdx = pcbTypes->AddString( mx::CStringDecoder::DecodeToWString( petTypes[I].pcName, petTypes[I].sNameLen ).c_str() );
						if ( iIdx >= 0 ) { pcbTypes->SetItemData( iIdx, static_cast<LPARAM>(I) ); }
					}
					pcbTypes->SetCurSelByItemData( static_cast<LPARAM>(m_pehpParms->ui32Type < sTotal ? m_pehpParms->ui32Type : 0) );
					::SendMessageW( pcbTypes->Wnd(), CB_SETMINVISIBLE, static_cast<WPARAM>(sTotal), 0 );
				}

				auto pwBpr = FindChild( CExportHexLayout::MX_EHI_BPR_EDIT );
				if ( pwBpr ) { pwBpr->SetTextW( std::to_wstring( m_pehpParms->ui32BytesPerRow ).c_str() ); }
				auto pwSpin = FindChild( CExportHexLayout::MX_EHI_BPR_UPDOWN );
				if ( pwSpin ) {
					::SendMessageW( pwSpin->Wnd(), UDM_SETRANGE32, 1, 65535 );
					::SendMessageW( pwSpin->Wnd(), UDM_SETPOS32, 0, static_cast<LPARAM>(m_pehpParms->ui32BytesPerRow) );
				}

				auto SetCheck = [this]( WORD _wId, bool _bCheck ) {
					auto pwThis = FindChild( _wId );
					if ( pwThis ) { pwThis->SetCheck( _bCheck ); }
				};
				const bool bSel = m_pehpParms->bHasSelection && m_pehpParms->bSelection;
				SetCheck( CExportHexLayout::MX_EHI_RANGE_ALL, !bSel );
				SetCheck( CExportHexLayout::MX_EHI_RANGE_SEL, bSel );
				Enable( CExportHexLayout::MX_EHI_RANGE_SEL, m_pehpParms->bHasSelection );
				SetCheck( CExportHexLayout::MX_EHI_START_ZERO, m_pehpParms->smStart == CExportHexLayout::MX_SM_ZERO );
				SetCheck( CExportHexLayout::MX_EHI_START_RANGE, m_pehpParms->smStart == CExportHexLayout::MX_SM_RANGE );
				SetCheck( CExportHexLayout::MX_EHI_START_CUSTOM, m_pehpParms->smStart == CExportHexLayout::MX_SM_CUSTOM );
				SetCheck( CExportHexLayout::MX_EHI_UNITS_BYTES, !m_pehpParms->bWords );
				SetCheck( CExportHexLayout::MX_EHI_UNITS_WORDS, m_pehpParms->bWords );
				SetCheck( CExportHexLayout::MX_EHI_SPACES, m_pehpParms->bSpaces );
				SetCheck( CExportHexLayout::MX_EHI_LINEFEEDS, m_pehpParms->bLineFeeds );

				auto pwStart = FindChild( CExportHexLayout::MX_EHI_START_EDIT );
				if ( pwStart ) {
					std::wstring wsStart;
					if ( m_pehpParms->bHexInput ) {
						const char * pcHex = CUtilities::ToHex( m_pehpParms->ui64CustomStart, 1, false );
						for ( const char * pcThis = pcHex; pcThis && (*pcThis); ++pcThis ) { wsStart.push_back( wchar_t( (*pcThis) ) ); }
					}
					else {
						wsStart = std::to_wstring( m_pehpParms->ui64CustomStart );
					}
					pwStart->SetTextW( wsStart.c_str() );
				}
			}
			catch ( ... ) {}
			UpdateOptions();
		}
		return CParent::InitDialog();
	}

	/**
	 * Handles WM_CLOSE.  Closes the dialog as Cancel.
	 *
	 * \return Returns LSW_H_HANDLED.
	 **/
	CWidget::LSW_HANDLED CExportHexDialog::Close() {
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
	CWidget::LSW_HANDLED CExportHexDialog::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case CExportHexLayout::MX_EHI_TYPE_COMBO : {
				if ( _wCtrlCode == CBN_SELCHANGE ) {
					UpdateExtension();
					UpdateOptions();
				}
				break;
			}
			case CExportHexLayout::MX_EHI_START_ZERO : {}				MX_FALLTHROUGH
			case CExportHexLayout::MX_EHI_START_RANGE : {}				MX_FALLTHROUGH
			case CExportHexLayout::MX_EHI_START_CUSTOM : {
				UpdateOptions();
				break;
			}
			case CExportHexLayout::MX_EHI_BROWSE : {
				Browse();
				break;
			}
			case CExportHexLayout::MX_EHI_EXPORT : {
				if ( !m_pehpParms ) {
					::EndDialog( Wnd(), FALSE );
					break;
				}
				if ( Finish() ) { ::EndDialog( Wnd(), TRUE ); }
				break;
			}
			case CExportHexLayout::MX_EHI_CANCEL : {
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
	CWidget::LSW_HANDLED CExportHexDialog::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {}								MX_FALLTHROUGH
			case CExportHexLayout::MX_EHI_CANCEL : {
				return Close();
			}
		}
		return CParent::MenuCommand( _wId );
	}

	/**
	 * Gets the index of the export type selected in the list.
	 *
	 * \return Returns the index into CExportHexLayout::Types().
	 **/
	uint32_t CExportHexDialog::SelectedType() {
		size_t sTotal = 0;
		CExportHexLayout::Types( sTotal );
		auto pcbTypes = static_cast<lsw::CComboBox *>(FindChild( CExportHexLayout::MX_EHI_TYPE_COMBO ));
		if ( !pcbTypes ) { return 0; }
		const LPARAM lpType = pcbTypes->GetCurSelItemData();
		return (lpType >= 0 && size_t( lpType ) < sTotal) ? uint32_t( lpType ) : 0;
	}

	/**
	 * Enables the options that apply to the selected export type and disables the rest.
	 **/
	void CExportHexDialog::UpdateOptions() {
		size_t sTotal = 0;
		const CExportHexLayout::MX_EXPORT_TYPE * petTypes = CExportHexLayout::Types( sTotal );
		const MX_COPY_AS caFmt = petTypes[SelectedType()].caFormat;
		const bool bRecords = CExportHexLayout::IsRecordFormat( caFmt );
		for ( WORD wId : { WORD( CExportHexLayout::MX_EHI_START_LABEL ), WORD( CExportHexLayout::MX_EHI_START_ZERO ), WORD( CExportHexLayout::MX_EHI_START_RANGE ),
			WORD( CExportHexLayout::MX_EHI_START_CUSTOM ), WORD( CExportHexLayout::MX_EHI_UNITS_LABEL ), WORD( CExportHexLayout::MX_EHI_UNITS_BYTES ),
			WORD( CExportHexLayout::MX_EHI_UNITS_WORDS ) } ) {
			Enable( wId, bRecords );
		}
		Enable( CExportHexLayout::MX_EHI_START_EDIT, bRecords && Checked( CExportHexLayout::MX_EHI_START_CUSTOM ) );
		Enable( CExportHexLayout::MX_EHI_SPACES, caFmt == MX_CA_HEX_TEXT );
		Enable( CExportHexLayout::MX_EHI_LINEFEEDS, caFmt == MX_CA_HEX_TEXT );
		// Base64 and uuencoding have fixed line lengths.
		const bool bRows = caFmt != MX_CA_BASE64 && caFmt != MX_CA_UUENCODING;
		Enable( CExportHexLayout::MX_EHI_BPR_LABEL, bRows );
		Enable( CExportHexLayout::MX_EHI_BPR_EDIT, bRows );
		Enable( CExportHexLayout::MX_EHI_BPR_UPDOWN, bRows );
	}

	/**
	 * Gives the file in the Export File box the extension of the selected export type.
	 **/
	void CExportHexDialog::UpdateExtension() {
		auto pwFile = FindChild( CExportHexLayout::MX_EHI_FILE_EDIT );
		if ( !pwFile ) { return; }
		try {
			std::filesystem::path pPath( pwFile->GetTextW() );
			if ( pPath.empty() || !pPath.has_filename() ) { return; }
			size_t sTotal = 0;
			const CExportHexLayout::MX_EXPORT_TYPE * petTypes = CExportHexLayout::Types( sTotal );
			const uint32_t ui32Type = SelectedType();
			pPath.replace_extension( std::filesystem::path( static_cast<const std::wstring &>(mx::CStringDecoder::DecodeToWString( petTypes[ui32Type].pcExt, petTypes[ui32Type].sExtLen )) ) );
			pwFile->SetTextW( pPath.wstring().c_str() );
		}
		catch ( ... ) {}
	}

	/**
	 * Asks for the file to write (the Browse button).
	 **/
	void CExportHexDialog::Browse() {
		auto pwFile = FindChild( CExportHexLayout::MX_EHI_FILE_EDIT );
		if ( !pwFile ) { return; }
		try {
			const std::filesystem::path pCurrent( pwFile->GetTextW() );
			OPENFILENAMEW ofnSaveFile = { sizeof( ofnSaveFile ) };
			CSecureWString szFileName;
			szFileName.resize( 0xFFFF + 2 );
			const std::wstring wsName = pCurrent.filename().wstring();
			std::memcpy( szFileName.data(), wsName.c_str(), std::min( wsName.size(), szFileName.size() - 2 ) * sizeof( wchar_t ) );
			const std::wstring wsDir = pCurrent.has_parent_path() ? pCurrent.parent_path().wstring() : std::wstring();

			CSecureWString wsFilter = _DEC_WS_6331FDE9_All_Files_______0____0;
			ofnSaveFile.hwndOwner = Wnd();
			ofnSaveFile.lpstrFilter = wsFilter.c_str();
			ofnSaveFile.lpstrFile = szFileName.data();
			ofnSaveFile.nMaxFile = DWORD( szFileName.size() );
			ofnSaveFile.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_NOREADONLYRETURN | OFN_HIDEREADONLY;
			ofnSaveFile.lpstrInitialDir = wsDir.size() ? wsDir.c_str() : nullptr;
			if ( ::GetSaveFileNameW( &ofnSaveFile ) ) {
				pwFile->SetTextW( ofnSaveFile.lpstrFile );
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Checks what was entered and fills in the parameters.
	 *
	 * \return Returns true if everything is valid.
	 **/
	bool CExportHexDialog::Finish() {
		try {
			auto pwFile = FindChild( CExportHexLayout::MX_EHI_FILE_EDIT );
			std::wstring wsPath = pwFile ? pwFile->GetTextW() : std::wstring();
			while ( wsPath.size() && (wsPath.back() == L' ' || wsPath.back() == L'\t') ) { wsPath.pop_back(); }
			if ( wsPath.empty() ) {
				lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_948E74E5_Choose_the_file_to_export_to_.c_str(), _DEC_WS_D39B97AC_Export_Hex.c_str() );
				if ( pwFile ) { pwFile->SetFocus(); }
				return false;
			}

			const uint32_t ui32Type = SelectedType();
			size_t sTotal = 0;
			const CExportHexLayout::MX_EXPORT_TYPE * petTypes = CExportHexLayout::Types( sTotal );
			const MX_COPY_AS caFmt = petTypes[ui32Type].caFormat;

			// Bytes per row (not used by Base64 or uuencoding).
			uint32_t ui32Bpr = m_pehpParms->ui32BytesPerRow;
			if ( caFmt != MX_CA_BASE64 && caFmt != MX_CA_UUENCODING ) {
				auto pwBpr = FindChild( CExportHexLayout::MX_EHI_BPR_EDIT );
				const std::wstring wsBpr = pwBpr ? pwBpr->GetTextW() : std::wstring();
				uint64_t ui64Bpr = 0;
				bool bValid = !wsBpr.empty() && wsBpr.size() <= 5;
				for ( auto wcThis : wsBpr ) {
					if ( wcThis < L'0' || wcThis > L'9' ) { bValid = false; break; }
					ui64Bpr = ui64Bpr * 10 + uint64_t( wcThis - L'0' );
				}
				if ( !bValid || ui64Bpr < 1 || ui64Bpr > 65535 ) {
					lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_F076F729_The_bytes_per_row_must_be_from_1_to_65535_.c_str(), _DEC_WS_D39B97AC_Export_Hex.c_str() );
					if ( pwBpr ) { pwBpr->SetFocus(); }
					return false;
				}
				ui32Bpr = uint32_t( ui64Bpr );
			}

			// The custom start address.
			CExportHexLayout::MX_START_MODE smStart = CExportHexLayout::MX_SM_ZERO;
			if ( Checked( CExportHexLayout::MX_EHI_START_RANGE ) ) { smStart = CExportHexLayout::MX_SM_RANGE; }
			else if ( Checked( CExportHexLayout::MX_EHI_START_CUSTOM ) ) { smStart = CExportHexLayout::MX_SM_CUSTOM; }
			uint64_t ui64Custom = m_pehpParms->ui64CustomStart;
			if ( CExportHexLayout::IsRecordFormat( caFmt ) && smStart == CExportHexLayout::MX_SM_CUSTOM ) {
				auto pwStart = FindChild( CExportHexLayout::MX_EHI_START_EDIT );
				if ( pwStart ) {
					pwStart->SetTreatAsHex( m_pehpParms->bHexInput );
					ee::CExpEvalContainer::EE_RESULT rResult;
					BOOL bValid = FALSE;
					CSecureWString swsTitle, swsError;
					if ( !pwStart->GetTextAsExpression( rResult, ee::CObject::EE_TF_NONE, &bValid ) ) {
						swsTitle = _DEC_WS_9607671B_Invalid_Expression;
						swsError = _DEC_WS_7605BFA8_The_given_expression_is_invalid_;
					}
					else if ( !bValid ) {
						swsTitle = _DEC_WS_EBFDD36E_Unable_to_Resolve;
						swsError = _DEC_WS_26C9377F_The_given_expression_is_valid_but_unable_to_be_resolved_;
					}
					else {
						auto rCast = ee::CExpEvalContainer::ConvertResult( rResult, ee::EE_NC_UNSIGNED );
						if ( rCast.ncType == ee::EE_NC_UNSIGNED ) { ui64Custom = rCast.u.ui64Val; }
						else {
							swsTitle = _DEC_WS_9607671B_Invalid_Expression;
							swsError = _DEC_WS_16D063D0_The_given_expression_must_evaluate_to_an_unsigned_integer_;
						}
					}
					if ( swsError.size() ) {
						lsw::CBase::MessageBoxError( Wnd(), swsError.c_str(), swsTitle.c_str() );
						pwStart->SetFocus();
						return false;
					}
				}
			}

			m_pehpParms->wsPath = wsPath;
			m_pehpParms->ui32Type = ui32Type;
			m_pehpParms->ui32BytesPerRow = ui32Bpr;
			m_pehpParms->bSelection = m_pehpParms->bHasSelection && Checked( CExportHexLayout::MX_EHI_RANGE_SEL );
			m_pehpParms->smStart = smStart;
			m_pehpParms->ui64CustomStart = ui64Custom;
			m_pehpParms->bWords = Checked( CExportHexLayout::MX_EHI_UNITS_WORDS );
			m_pehpParms->bSpaces = Checked( CExportHexLayout::MX_EHI_SPACES );
			m_pehpParms->bLineFeeds = Checked( CExportHexLayout::MX_EHI_LINEFEEDS );
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Determines whether a radio button or check box is checked.
	 *
	 * \param _wId The ID of the control.
	 * \return Returns true if the control exists and is checked.
	 **/
	bool CExportHexDialog::Checked( WORD _wId ) {
		auto pwThis = FindChild( _wId );
		return pwThis && pwThis->IsChecked();
	}

	/**
	 * Enables or disables a control.
	 *
	 * \param _wId The ID of the control.
	 * \param _bEnable If true, the control is enabled.
	 **/
	void CExportHexDialog::Enable( WORD _wId, bool _bEnable ) {
		auto pwThis = FindChild( _wId );
		if ( pwThis ) { ::EnableWindow( pwThis->Wnd(), _bEnable ? TRUE : FALSE ); }
	}

}	// namespace mx

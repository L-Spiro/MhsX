#include "MXFontDialog.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"

#include <ComboBox/LSWComboBox.h>

#include <algorithm>
#include <cwctype>


namespace mx {

	CFontDialog::CFontDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pfdpParms( reinterpret_cast<CFontDialogLayout::MX_FONT_DIALOG_PARMS *>(_ui64Data) ) {
	}
	CFontDialog::~CFontDialog() {
		if ( m_hSampleFont ) {
			::DeleteObject( m_hSampleFont );
			m_hSampleFont = NULL;
		}
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.  Fills the lists and selects the current font.
	 *
	 * \return Returns LSW_H_CONTINUE to allow default processing.
	 **/
	CWidget::LSW_HANDLED CFontDialog::InitDialog() {
		LSW_RECT rWindow;
		if ( ::GetWindowRect( Wnd(), &rWindow ) ) {
			m_sStartSize.cx = rWindow.Width();
			m_sStartSize.cy = rWindow.Height();
		}
		m_bUpdating = true;
		try {
			// Writing systems.
			auto pcbCombo = static_cast<lsw::CComboBox *>(FindChild( CFontDialogLayout::MX_FDI_WS_COMBO ));
			if ( pcbCombo ) {
				for ( size_t I = 0; I < lsw::CFonts::WritingSystems(); ++I ) {
					INT iIdx = pcbCombo->AddString( lsw::CFonts::WritingSystem( static_cast<lsw::CFonts::LSW_WS_ID>(I) ).pwszName );
					if ( iIdx >= 0 ) { pcbCombo->SetItemData( iIdx, static_cast<LPARAM>(I) ); }
				}
				pcbCombo->SetCurSelByItemData( lsw::CFonts::LSW_WS_ANY );
			}

			if ( m_pfdpParms ) {
				const LOGFONTW &lfFont = m_pfdpParms->lfFont;
				auto pwStrike = FindChild( CFontDialogLayout::MX_FDI_STRIKEOUT );
				auto pwUnder = FindChild( CFontDialogLayout::MX_FDI_UNDERLINE );
				if ( pwStrike ) { pwStrike->SetCheck( lfFont.lfStrikeOut != 0 ); }
				if ( pwUnder ) { pwUnder->SetCheck( lfFont.lfUnderline != 0 ); }

				SetTextQuietly( CFontDialogLayout::MX_FDI_SIZE_EDIT, std::to_wstring( m_pfdpParms->i32PointSize ).c_str() );
				FillFonts( lfFont.lfFaceName );
				FillStyles( lfFont.lfWeight ? lfFont.lfWeight : FW_NORMAL, lfFont.lfItalic, true );
				FillSizes();
			}
		}
		catch ( ... ) {}
		m_bUpdating = false;
		UpdateSample();

		return CParent::InitDialog();
	}

	/**
	 * Handles WM_CLOSE.  Closes the dialog as Cancel.
	 *
	 * \return Returns LSW_H_HANDLED.
	 **/
	CWidget::LSW_HANDLED CFontDialog::Close() {
		::EndDialog( Wnd(), FALSE );
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND.  Keeps the edit boxes, lists, and sample in step, and checks and returns the font on OK.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
	 * \param _wId The identifier for the command or control.
	 * \param _pwSrc The source control for control notifications; otherwise nullptr.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CFontDialog::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		if ( m_bUpdating ) { return LSW_H_HANDLED; }
		try {
			switch ( _wId ) {
				case CFontDialogLayout::MX_FDI_FONT_LIST : {
					if ( _wCtrlCode == LBN_SELCHANGE ) {
						auto plbList = List( CFontDialogLayout::MX_FDI_FONT_LIST );
						const INT iSel = plbList ? plbList->GetCurSel() : LB_ERR;
						if ( iSel >= 0 && size_t( iSel ) < m_vFonts.size() ) {
							SetTextQuietly( CFontDialogLayout::MX_FDI_FONT_EDIT, m_vFonts[iSel].lfLogFont.lfFaceName );
						}
						// Keep the style as close as possible to the one chosen.
						LOGFONTW lfTmp {};
						int32_t i32Size;
						BuildFont( lfTmp, i32Size );
						FillStyles( lfTmp.lfWeight, lfTmp.lfItalic, true );
						FillSizes();
						UpdateSample();
					}
					break;
				}
				case CFontDialogLayout::MX_FDI_FONT_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						auto plbList = List( CFontDialogLayout::MX_FDI_FONT_LIST );
						const INT iIdx = FindInList( CFontDialogLayout::MX_FDI_FONT_LIST, CFontDialogLayout::MX_FDI_FONT_EDIT, false );
						if ( plbList && iIdx >= 0 && iIdx != plbList->GetCurSel() ) {
							m_bUpdating = true;
							plbList->SetCurSel( iIdx );
							m_bUpdating = false;
							LOGFONTW lfTmp {};
							int32_t i32Size;
							BuildFont( lfTmp, i32Size );
							FillStyles( lfTmp.lfWeight, lfTmp.lfItalic, true );
							FillSizes();
						}
						UpdateSample();
					}
					break;
				}
				case CFontDialogLayout::MX_FDI_STYLE_LIST : {
					if ( _wCtrlCode == LBN_SELCHANGE ) {
						auto plbList = List( CFontDialogLayout::MX_FDI_STYLE_LIST );
						const INT iSel = plbList ? plbList->GetCurSel() : LB_ERR;
						if ( iSel >= 0 && size_t( iSel ) < m_vStyles.size() ) {
							SetTextQuietly( CFontDialogLayout::MX_FDI_STYLE_EDIT, m_vStyles[iSel].wsStyleName.c_str() );
						}
						FillSizes();
						UpdateSample();
					}
					break;
				}
				case CFontDialogLayout::MX_FDI_STYLE_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						auto plbList = List( CFontDialogLayout::MX_FDI_STYLE_LIST );
						const INT iIdx = FindInList( CFontDialogLayout::MX_FDI_STYLE_LIST, CFontDialogLayout::MX_FDI_STYLE_EDIT, false );
						if ( plbList && iIdx >= 0 && iIdx != plbList->GetCurSel() ) {
							m_bUpdating = true;
							plbList->SetCurSel( iIdx );
							m_bUpdating = false;
							FillSizes();
						}
						UpdateSample();
					}
					break;
				}
				case CFontDialogLayout::MX_FDI_SIZE_LIST : {
					if ( _wCtrlCode == LBN_SELCHANGE ) {
						auto plbList = List( CFontDialogLayout::MX_FDI_SIZE_LIST );
						const INT iSel = plbList ? plbList->GetCurSel() : LB_ERR;
						if ( iSel >= 0 ) {
							SetTextQuietly( CFontDialogLayout::MX_FDI_SIZE_EDIT, std::to_wstring( plbList->GetItemData( iSel ) ).c_str() );
						}
						UpdateSample();
					}
					break;
				}
				case CFontDialogLayout::MX_FDI_SIZE_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						auto plbList = List( CFontDialogLayout::MX_FDI_SIZE_LIST );
						const INT iIdx = FindInList( CFontDialogLayout::MX_FDI_SIZE_LIST, CFontDialogLayout::MX_FDI_SIZE_EDIT, true );
						if ( plbList ) {
							m_bUpdating = true;
							plbList->SetCurSel( iIdx );
							m_bUpdating = false;
						}
						UpdateSample();
					}
					break;
				}
				case CFontDialogLayout::MX_FDI_STRIKEOUT : {}		MX_FALLTHROUGH
				case CFontDialogLayout::MX_FDI_UNDERLINE : {
					UpdateSample();
					break;
				}
				case CFontDialogLayout::MX_FDI_WS_COMBO : {
					if ( _wCtrlCode == CBN_SELCHANGE ) {
						auto pcbCombo = static_cast<lsw::CComboBox *>(FindChild( CFontDialogLayout::MX_FDI_WS_COMBO ));
						if ( pcbCombo ) {
							const LPARAM lpWs = pcbCombo->GetCurSelItemData();
							if ( lpWs >= 0 && size_t( lpWs ) < lsw::CFonts::WritingSystems() ) {
								m_wiWritingSystem = static_cast<lsw::CFonts::LSW_WS_ID>(lpWs);
							}
						}
						// Keep the font if it is still listed.
						LOGFONTW lfTmp {};
						int32_t i32Size;
						BuildFont( lfTmp, i32Size );
						auto pwEdit = FindChild( CFontDialogLayout::MX_FDI_FONT_EDIT );
						HCURSOR hOld = ::SetCursor( ::LoadCursorW( NULL, IDC_WAIT ) );
						FillFonts( pwEdit ? pwEdit->GetTextW() : std::wstring() );
						::SetCursor( hOld );
						FillStyles( lfTmp.lfWeight, lfTmp.lfItalic, true );
						FillSizes();
						UpdateSample();
					}
					break;
				}
				case CFontDialogLayout::MX_FDI_OK : {
					if ( !m_pfdpParms ) {
						::EndDialog( Wnd(), FALSE );
						break;
					}
					// The font must be installed (and listed for the writing system).
					if ( FindInList( CFontDialogLayout::MX_FDI_FONT_LIST, CFontDialogLayout::MX_FDI_FONT_EDIT, true ) < 0 ) {
						lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_3547FFEB_Choose_a_font_from_the_list_or_type_the_name_of_an_installed_font_.c_str(), _DEC_WS_EE6F169E_Change_Font.c_str() );
						auto pwEdit = FindChild( CFontDialogLayout::MX_FDI_FONT_EDIT );
						if ( pwEdit ) { pwEdit->SetFocus(); }
						break;
					}
					LOGFONTW lfFont {};
					int32_t i32Size = 0;
					if ( !BuildFont( lfFont, i32Size ) ) {
						lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_F229696B_The_size_must_be_a_whole_number_from_1_to_1638_.c_str(), _DEC_WS_EE6F169E_Change_Font.c_str() );
						auto pwEdit = FindChild( CFontDialogLayout::MX_FDI_SIZE_EDIT );
						if ( pwEdit ) {
							pwEdit->SetFocus();
							::SendMessageW( pwEdit->Wnd(), EM_SETSEL, 0, -1 );
						}
						break;
					}
					m_pfdpParms->lfFont = lfFont;
					m_pfdpParms->i32PointSize = i32Size;
					::EndDialog( Wnd(), TRUE );
					break;
				}
				case CFontDialogLayout::MX_FDI_CANCEL : {
					::EndDialog( Wnd(), FALSE );
					break;
				}
			}
		}
		catch ( ... ) {}
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND from a menu (Escape).
	 *
	 * \param _wId The menu command identifier.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CFontDialog::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {}								MX_FALLTHROUGH
			case CFontDialogLayout::MX_FDI_CANCEL : {
				return Close();
			}
		}
		return CParent::MenuCommand( _wId );
	}

	/**
	 * Handles WM_GETMINMAXINFO.  The dialog cannot be made much smaller than it starts.
	 *
	 * \param _pmmiInfo The size limits to fill.
	 * \return Returns LSW_H_HANDLED.
	 **/
	CWidget::LSW_HANDLED CFontDialog::GetMinMaxInfo( MINMAXINFO * _pmmiInfo ) {
		if ( !_pmmiInfo || !m_sStartSize.cx ) { return LSW_H_CONTINUE; }
		// The Size column and the buttons keep their size, so the rest can only shrink so far.
		_pmmiInfo->ptMinTrackSize.x = m_sStartSize.cx * 3 / 5;
		_pmmiInfo->ptMinTrackSize.y = m_sStartSize.cy * 3 / 4;
		return LSW_H_HANDLED;
	}

	/**
	 * Gets a list box.
	 *
	 * \param _wId The ID of the list box.
	 * \return Returns the list box, or nullptr.
	 **/
	lsw::CListBox * CFontDialog::List( WORD _wId ) {
		CWidget * pwList = FindChild( _wId );
		if ( !pwList || !pwList->IsListBox() ) { return nullptr; }
		return static_cast<lsw::CListBox *>(pwList);
	}

	/**
	 * Sets the text of a control without reacting to its change notification.
	 *
	 * \param _wId The ID of the control.
	 * \param _pwcText The text.
	 **/
	void CFontDialog::SetTextQuietly( WORD _wId, const wchar_t * _pwcText ) {
		auto pwThis = FindChild( _wId );
		if ( !pwThis ) { return; }
		const bool bOld = m_bUpdating;
		m_bUpdating = true;
		pwThis->SetTextW( _pwcText );
		m_bUpdating = bOld;
	}

	/**
	 * Finds the first item in a list box that starts with the text of an edit box (ignoring case), or that matches it exactly.
	 *
	 * \param _wList The ID of the list box.
	 * \param _wEdit The ID of the edit box.
	 * \param _bExact If true, the whole item must match.
	 * \return Returns the index of the item, or LB_ERR.
	 **/
	INT CFontDialog::FindInList( WORD _wList, WORD _wEdit, bool _bExact ) {
		auto plbList = List( _wList );
		auto pwEdit = FindChild( _wEdit );
		if ( !plbList || !pwEdit ) { return LB_ERR; }
		const std::wstring wsText = pwEdit->GetTextW();
		if ( wsText.empty() ) { return LB_ERR; }
		return static_cast<INT>(::SendMessageW( plbList->Wnd(), _bExact ? LB_FINDSTRINGEXACT : LB_FINDSTRING, WPARAM( -1 ), reinterpret_cast<LPARAM>(wsText.c_str()) ));
	}

	/**
	 * Fills the Font list with the fonts for the current writing system and selects a font.
	 *
	 * \param _wsSelect The name of the font to select (the first font if it is not listed).
	 **/
	void CFontDialog::FillFonts( const std::wstring &_wsSelect ) {
		auto plbList = List( CFontDialogLayout::MX_FDI_FONT_LIST );
		if ( !plbList ) { return; }
		const bool bOld = m_bUpdating;
		m_bUpdating = true;
		try {
			m_vFonts.clear();
			const lsw::CFonts::CSystemFonts sfFonts = lsw::CFonts::EnumAllFaces( m_pfdpParms ? m_pfdpParms->ui32Dpi : 96, m_wiWritingSystem );
			m_vFonts.assign( sfFonts.begin(), sfFonts.end() );

			::SendMessageW( plbList->Wnd(), WM_SETREDRAW, FALSE, 0 );
			plbList->ResetContent();
			INT iSelect = LB_ERR;
			for ( size_t I = 0; I < m_vFonts.size(); ++I ) {
				const INT iIdx = plbList->AddString( m_vFonts[I].lfLogFont.lfFaceName );
				if ( iIdx < 0 ) { continue; }
				if ( iSelect == LB_ERR && ::_wcsicmp( m_vFonts[I].lfLogFont.lfFaceName, _wsSelect.c_str() ) == 0 ) { iSelect = iIdx; }
			}
			::SendMessageW( plbList->Wnd(), WM_SETREDRAW, TRUE, 0 );
			::InvalidateRect( plbList->Wnd(), nullptr, TRUE );

			if ( iSelect == LB_ERR && m_vFonts.size() ) { iSelect = 0; }
			plbList->SetCurSel( iSelect );
			if ( iSelect >= 0 && size_t( iSelect ) < m_vFonts.size() ) {
				auto pwEdit = FindChild( CFontDialogLayout::MX_FDI_FONT_EDIT );
				if ( pwEdit ) { pwEdit->SetTextW( m_vFonts[iSelect].lfLogFont.lfFaceName ); }
			}
		}
		catch ( ... ) {}
		m_bUpdating = bOld;
	}

	/**
	 * Fills the Font Style list with the styles of the selected font and selects the style closest to the one wanted.
	 *
	 * \param _lWeight The weight wanted.
	 * \param _bItalic The italic setting wanted.
	 * \param _bSetText If true, the Font Style edit box gets the name of the style selected.
	 **/
	void CFontDialog::FillStyles( LONG _lWeight, BYTE _bItalic, bool _bSetText ) {
		auto plbFonts = List( CFontDialogLayout::MX_FDI_FONT_LIST );
		auto plbList = List( CFontDialogLayout::MX_FDI_STYLE_LIST );
		if ( !plbFonts || !plbList ) { return; }
		const bool bOld = m_bUpdating;
		m_bUpdating = true;
		try {
			m_vStyles.clear();
			plbList->ResetContent();
			const INT iFont = plbFonts->GetCurSel();
			if ( iFont >= 0 && size_t( iFont ) < m_vFonts.size() ) {
				m_vStyles.assign( m_vFonts[iFont].fsStyles.begin(), m_vFonts[iFont].fsStyles.end() );
				// Lightest to heaviest, upright before italic.
				std::stable_sort( m_vStyles.begin(), m_vStyles.end(), []( const lsw::CFonts::LSW_FONT_STYLE &_fsA, const lsw::CFonts::LSW_FONT_STYLE &_fsB ) {
					return lsw::CFonts::RankStyle( _fsA.wsStyleName ) < lsw::CFonts::RankStyle( _fsB.wsStyleName );
				} );
			}

			INT iBest = LB_ERR;
			LONG lBestScore = LONG_MAX;
			for ( size_t I = 0; I < m_vStyles.size(); ++I ) {
				const INT iIdx = plbList->AddString( m_vStyles[I].wsStyleName.c_str() );
				if ( iIdx < 0 ) { continue; }
				// Matching the italic setting matters most, then the weight.
				const LONG lWeight = m_vStyles[I].lWeight ? m_vStyles[I].lWeight : FW_NORMAL;
				const LONG lScore = ((m_vStyles[I].bItalic != 0) != (_bItalic != 0) ? 10000 : 0) + std::abs( lWeight - _lWeight );
				if ( lScore < lBestScore ) {
					lBestScore = lScore;
					iBest = iIdx;
				}
			}
			plbList->SetCurSel( iBest );
			if ( _bSetText && iBest >= 0 && size_t( iBest ) < m_vStyles.size() ) {
				auto pwEdit = FindChild( CFontDialogLayout::MX_FDI_STYLE_EDIT );
				if ( pwEdit ) { pwEdit->SetTextW( m_vStyles[iBest].wsStyleName.c_str() ); }
			}
		}
		catch ( ... ) {}
		m_bUpdating = bOld;
	}

	/**
	 * Fills the Size list with the sizes of the selected style and selects the size in the Size edit box, if it is listed.
	 **/
	void CFontDialog::FillSizes() {
		auto plbStyles = List( CFontDialogLayout::MX_FDI_STYLE_LIST );
		auto plbList = List( CFontDialogLayout::MX_FDI_SIZE_LIST );
		if ( !plbStyles || !plbList ) { return; }
		const bool bOld = m_bUpdating;
		m_bUpdating = true;
		try {
			plbList->ResetContent();
			const INT iStyle = plbStyles->GetCurSel();
			if ( iStyle >= 0 && size_t( iStyle ) < m_vStyles.size() ) {
				for ( auto ui32Size : m_vStyles[iStyle].sSizes ) {
					const INT iIdx = plbList->AddString( std::to_wstring( ui32Size ).c_str() );
					if ( iIdx >= 0 ) { plbList->SetItemData( iIdx, static_cast<LPARAM>(ui32Size) ); }
				}
			}
			plbList->SetCurSel( FindInList( CFontDialogLayout::MX_FDI_SIZE_LIST, CFontDialogLayout::MX_FDI_SIZE_EDIT, true ) );
		}
		catch ( ... ) {}
		m_bUpdating = bOld;
	}

	/**
	 * Gets the size typed in the Size edit box.
	 *
	 * \param _i32Size Receives the size in points.
	 * \return Returns true if the text is a whole number from 1 to 1638.
	 **/
	bool CFontDialog::GetSize( int32_t &_i32Size ) {
		auto pwEdit = FindChild( CFontDialogLayout::MX_FDI_SIZE_EDIT );
		if ( !pwEdit ) { return false; }
		try {
			const std::wstring wsText = pwEdit->GetTextW();
			size_t sStart = 0, sEnd = wsText.size();
			while ( sStart < sEnd && std::iswspace( wsText[sStart] ) ) { ++sStart; }
			while ( sEnd > sStart && std::iswspace( wsText[sEnd-1] ) ) { --sEnd; }
			if ( sStart == sEnd || sEnd - sStart > 4 ) { return false; }
			int32_t i32Size = 0;
			for ( size_t I = sStart; I < sEnd; ++I ) {
				if ( wsText[I] < L'0' || wsText[I] > L'9' ) { return false; }
				i32Size = i32Size * 10 + int32_t( wsText[I] - L'0' );
			}
			// 1638 points is the largest size that fits a font height (16,384 device units) at 720 DPI.
			if ( i32Size < 1 || i32Size > 1638 ) { return false; }
			_i32Size = i32Size;
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Builds the font described by the controls.
	 *
	 * \param _lfFont Receives the font (its height for the DPI in the parameters).
	 * \param _i32Size Receives the size in points.
	 * \return Returns false if the size is not valid.
	 **/
	bool CFontDialog::BuildFont( LOGFONTW &_lfFont, int32_t &_i32Size ) {
		// Everything not set here (quality, pitch, etc.) stays as it was.
		_lfFont = m_pfdpParms ? m_pfdpParms->lfFont : LOGFONTW {};
		auto pwFace = FindChild( CFontDialogLayout::MX_FDI_FONT_EDIT );
		if ( pwFace ) {
			::wcsncpy_s( _lfFont.lfFaceName, pwFace->GetTextW().c_str(), _TRUNCATE );
		}
		_lfFont.lfCharSet = DEFAULT_CHARSET;

		// The style selected in the list if its name was typed (or chosen), otherwise whatever the typed name suggests.
		auto plbStyles = List( CFontDialogLayout::MX_FDI_STYLE_LIST );
		auto pwStyle = FindChild( CFontDialogLayout::MX_FDI_STYLE_EDIT );
		const std::wstring wsStyle = pwStyle ? pwStyle->GetTextW() : std::wstring();
		const INT iStyle = plbStyles ? plbStyles->GetCurSel() : LB_ERR;
		if ( iStyle >= 0 && size_t( iStyle ) < m_vStyles.size() && ::_wcsicmp( m_vStyles[iStyle].wsStyleName.c_str(), wsStyle.c_str() ) == 0 ) {
			_lfFont.lfWeight = m_vStyles[iStyle].lWeight ? m_vStyles[iStyle].lWeight : FW_NORMAL;
			_lfFont.lfItalic = m_vStyles[iStyle].bItalic;
		}
		else {
			lsw::CFonts::ApplyStyleToLogFont( wsStyle, _lfFont );
		}

		auto pwStrike = FindChild( CFontDialogLayout::MX_FDI_STRIKEOUT );
		auto pwUnder = FindChild( CFontDialogLayout::MX_FDI_UNDERLINE );
		_lfFont.lfStrikeOut = (pwStrike && pwStrike->IsChecked()) ? TRUE : FALSE;
		_lfFont.lfUnderline = (pwUnder && pwUnder->IsChecked()) ? TRUE : FALSE;

		_lfFont.lfWidth = 0;
		_lfFont.lfEscapement = _lfFont.lfOrientation = 0;
		if ( !GetSize( _i32Size ) ) { return false; }
		_lfFont.lfHeight = -::MulDiv( _i32Size, static_cast<int>(m_pfdpParms ? m_pfdpParms->ui32Dpi : 96), 72 );
		return true;
	}

	/**
	 * Shows the sample text in the font described by the controls.
	 **/
	void CFontDialog::UpdateSample() {
		auto pwSample = FindChild( CFontDialogLayout::MX_FDI_SAMPLE );
		if ( !pwSample ) { return; }
		LOGFONTW lfFont {};
		int32_t i32Size = 0;
		// An invalid size shows the sample at the last size that was valid.
		if ( !BuildFont( lfFont, i32Size ) ) {
			if ( !m_hSampleFont ) { return; }
			LOGFONTW lfOld {};
			if ( ::GetObjectW( m_hSampleFont, sizeof( lfOld ), &lfOld ) ) { lfFont.lfHeight = lfOld.lfHeight; }
		}
		HFONT hNew = ::CreateFontIndirectW( &lfFont );
		if ( !hNew ) { return; }
		::SendMessageW( pwSample->Wnd(), WM_SETFONT, reinterpret_cast<WPARAM>(hNew), TRUE );
		if ( m_hSampleFont ) { ::DeleteObject( m_hSampleFont ); }
		m_hSampleFont = hNew;
		pwSample->SetTextW( lsw::CFonts::WritingSystem( m_wiWritingSystem ).pwszSample );
	}

}	// namespace mx

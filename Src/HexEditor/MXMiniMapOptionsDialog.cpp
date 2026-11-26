#include "MXMiniMapOptionsDialog.h"
#include "MXHexEditorControl.h"
#include "../Strings/MXStringMacros.h"

#include <ComboBox/LSWComboBox.h>
#include <TrackBar/LSWTrackBar.h>


namespace mx {

	/** A palette that can be chosen in the Colorize combo box. */
	struct MX_MINI_MAP_PALETTE {
		CMiniMap::MX_COLORS											cColors;							/**< The palette. */
		const char *												pcName;								/**< The encrypted name. */
		size_t														sNameLen;							/**< The length of the encrypted name. */
	};

	/** The palettes, in the order in which they are listed. */
	static const MX_MINI_MAP_PALETTE g_mmpPalettes[] = {
		{ CMiniMap::MX_C_ASCII,				_T_LEN_042926FB_ASCII_Sections },
		{ CMiniMap::MX_C_RAINBOW,			_T_LEN_C202E2AA_Rainbow },
		{ CMiniMap::MX_C_HEAT,				_T_LEN_1A7649C6_Heat },
		{ CMiniMap::MX_C_OCEAN,				_T_LEN_ADE27F1A_Ocean },
		{ CMiniMap::MX_C_GREY,				_T_LEN_41A155DC_Grey },
		{ CMiniMap::MX_C_CUBEHELIX,			_T_LEN_AF2057A4_Cubehelix },
		{ CMiniMap::MX_C_VIRIDIS,			_T_LEN_0543C5D3_Viridis },
		{ CMiniMap::MX_C_PLASMA,			_T_LEN_DE453153_Plasma },
		{ CMiniMap::MX_C_MAGMA,				_T_LEN_42A56202_Magma },
		{ CMiniMap::MX_C_INFERNO,			_T_LEN_158D82FC_Inferno },
		{ CMiniMap::MX_C_CIVIDIS,			_T_LEN_C525D07A_Cividis },
		{ CMiniMap::MX_C_TURBO,				_T_LEN_8AE3076A_Turbo },
		{ CMiniMap::MX_C_COOLWARM,			_T_LEN_0281117E_Cool_Warm },
		{ CMiniMap::MX_C_BLUEWHITERED,		_T_LEN_67ABF333_Blue_White_Red },
		{ CMiniMap::MX_C_TWILIGHT,			_T_LEN_1F423988_Twilight },
		{ CMiniMap::MX_C_TWILIGHTSHIFTED,	_T_LEN_FC095134_Twilight_Shifted },
	};

	CMiniMapOptionsDialog::CMiniMapOptionsDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmmopParms( reinterpret_cast<CMiniMapOptionsLayout::MX_MINI_MAP_OPTIONS_PARMS *>(_ui64Data) ) {
		if ( m_pmmopParms ) { m_mmOriginal = m_pmmopParms->mmOptions; }
	}
	CMiniMapOptionsDialog::~CMiniMapOptionsDialog() {
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.  Fills in the controls from the current options.
	 *
	 * \return Returns LSW_H_HANDLED (the focus is set here).
	 **/
	CWidget::LSW_HANDLED CMiniMapOptionsDialog::InitDialog() {
		try {
			auto pcbPos = static_cast<lsw::CComboBox *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_POSITION_COMBO ));
			if ( pcbPos ) {
				INT iIdx = pcbPos->AddString( _DEC_WS_3F59A998_Right_Side.c_str() );
				if ( iIdx >= 0 ) { pcbPos->SetItemData( iIdx, static_cast<LPARAM>(TRUE) ); }
				iIdx = pcbPos->AddString( _DEC_WS_5D4738F9_Left_Side.c_str() );
				if ( iIdx >= 0 ) { pcbPos->SetItemData( iIdx, static_cast<LPARAM>(FALSE) ); }
			}
			auto pcbColors = static_cast<lsw::CComboBox *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_COLORIZE_COMBO ));
			if ( pcbColors ) {
				for ( size_t I = 0; I < std::size( g_mmpPalettes ); ++I ) {
					const INT iIdx = pcbColors->AddString( mx::CStringDecoder::DecodeToWString( g_mmpPalettes[I].pcName, g_mmpPalettes[I].sNameLen ).c_str() );
					if ( iIdx >= 0 ) { pcbColors->SetItemData( iIdx, static_cast<LPARAM>(g_mmpPalettes[I].cColors) ); }
				}
				::SendMessageW( pcbColors->Wnd(), CB_SETMINVISIBLE, static_cast<WPARAM>(std::size( g_mmpPalettes )), 0 );
			}
		}
		catch ( ... ) {}

		auto ptbBright = static_cast<lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_BRIGHTNESS_TRACK ));
		if ( ptbBright ) {
			// -100..100 is shown as 0..200.
			ptbBright->SetRangeMin( FALSE, 0 );
			ptbBright->SetRangeMax( FALSE, 200 );
			ptbBright->SetTicFreq( 20 );
			ptbBright->SetPageSize( 10 );
		}
		auto ptbContrast = static_cast<lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_CONTRAST_TRACK ));
		if ( ptbContrast ) {
			ptbContrast->SetRangeMin( FALSE, 0 );
			ptbContrast->SetRangeMax( FALSE, 100 );
			ptbContrast->SetTicFreq( 10 );
			ptbContrast->SetPageSize( 10 );
		}
		auto ptbZoom = static_cast<lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_ZOOM_TRACK ));
		if ( ptbZoom ) {
			ptbZoom->SetRangeMin( FALSE, 1 );
			ptbZoom->SetRangeMax( FALSE, 10 );
			ptbZoom->SetTicFreq( 1 );
			ptbZoom->SetPageSize( 1 );
		}

		SetControls( m_mmOriginal );
		m_bReady = true;

		CParent::InitDialog();
		auto pwOk = FindChild( CMiniMapOptionsLayout::MX_MMOI_OK );
		if ( pwOk ) { pwOk->SetFocus(); }
		// The focus was set here.
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_CLOSE.  Closes the dialog as Cancel.
	 *
	 * \return Returns LSW_H_HANDLED.
	 **/
	CWidget::LSW_HANDLED CMiniMapOptionsDialog::Close() {
		Cancel();
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND.  Any change to a control is shown in the hex editor right away; OK keeps the changes, Cancel undoes them,
	 *	Hide hides the Mini Map, and Reset puts back the default options.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
	 * \param _wId The identifier for the command or control.
	 * \param _pwSrc The source control for control notifications; otherwise nullptr.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CMiniMapOptionsDialog::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case CMiniMapOptionsLayout::MX_MMOI_POSITION_COMBO : {}		MX_FALLTHROUGH
			case CMiniMapOptionsLayout::MX_MMOI_COLORIZE_COMBO : {
				if ( _wCtrlCode == CBN_SELCHANGE ) { Apply(); }
				break;
			}
			case CMiniMapOptionsLayout::MX_MMOI_COLORIZE_CHECK : {}		MX_FALLTHROUGH
			case CMiniMapOptionsLayout::MX_MMOI_HIGHLIGHT_STRUCTS : {}	MX_FALLTHROUGH
			case CMiniMapOptionsLayout::MX_MMOI_AUTO_SNAP_CHECK : {
				if ( _wCtrlCode == BN_CLICKED ) {
					UpdateEnabled();
					Apply();
				}
				break;
			}
			case CMiniMapOptionsLayout::MX_MMOI_BPR_EDIT : {}			MX_FALLTHROUGH
			case CMiniMapOptionsLayout::MX_MMOI_AUTO_SNAP_EDIT : {
				if ( _wCtrlCode == EN_CHANGE ) { Apply(); }
				break;
			}
			case CMiniMapOptionsLayout::MX_MMOI_RESET : {
				m_bReady = false;
				SetControls( CMiniMap::MX_MINI_MAP() );
				m_bReady = true;
				Apply();
				break;
			}
			case CMiniMapOptionsLayout::MX_MMOI_HIDE : {
				if ( m_pmmopParms ) {
					GetControls( m_pmmopParms->mmOptions );
					m_pmmopParms->bHide = true;
				}
				::EndDialog( Wnd(), TRUE );
				break;
			}
			case CMiniMapOptionsLayout::MX_MMOI_OK : {
				if ( m_pmmopParms ) {
					GetControls( m_pmmopParms->mmOptions );
					m_pmmopParms->bHide = false;
				}
				::EndDialog( Wnd(), TRUE );
				break;
			}
			case IDCANCEL : {}											MX_FALLTHROUGH
			case CMiniMapOptionsLayout::MX_MMOI_CANCEL : {
				Cancel();
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
	CWidget::LSW_HANDLED CMiniMapOptionsDialog::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {}											MX_FALLTHROUGH
			case CMiniMapOptionsLayout::MX_MMOI_CANCEL : {
				return Close();
			}
		}
		return CParent::MenuCommand( _wId );
	}

	/**
	 * Handles WM_HSCROLL.  Moving a slider updates its value label and the hex editor.
	 *
	 * \param _uScrollPos The current position of the scroll box if the scroll type is SB_THUMBPOSITION or SB_THUMBTRACK.
	 * \param _uScrollType The type of scroll request.
	 * \param _pwWidget The control sending the message (a slider), or nullptr.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CMiniMapOptionsDialog::HScroll( USHORT /*_uScrollPos*/, USHORT /*_uScrollType*/, CWidget * _pwWidget ) {
		// Only the sliders send WM_HSCROLL to this dialog.
		if ( !_pwWidget || _pwWidget->WidgetType() == LSW_LT_TRACKBAR ) {
			UpdateValueLabels();
			Apply();
			return LSW_H_HANDLED;
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Fills in the controls from a set of options.
	 *
	 * \param _mmOptions The options to show.
	 **/
	void CMiniMapOptionsDialog::SetControls( const CMiniMap::MX_MINI_MAP &_mmOptions ) {
		auto pcbPos = static_cast<lsw::CComboBox *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_POSITION_COMBO ));
		if ( pcbPos ) { pcbPos->SetCurSelByItemData( static_cast<LPARAM>(_mmOptions.bRightSize ? TRUE : FALSE) ); }

		const bool bColorize = _mmOptions.cColors != CMiniMap::MX_C_NONE;
		auto pwColorize = FindChild( CMiniMapOptionsLayout::MX_MMOI_COLORIZE_CHECK );
		if ( pwColorize ) { pwColorize->SetCheck( bColorize ); }
		auto pcbColors = static_cast<lsw::CComboBox *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_COLORIZE_COMBO ));
		if ( pcbColors ) {
			// When not colorizing, the list still shows a palette (the one Colorize would use).
			pcbColors->SetCurSelByItemData( static_cast<LPARAM>(bColorize ? _mmOptions.cColors : g_mmpPalettes[0].cColors) );
			if ( ::SendMessageW( pcbColors->Wnd(), CB_GETCURSEL, 0, 0 ) == CB_ERR ) { pcbColors->SetCurSel( 0 ); }
		}

		auto pwStructs = FindChild( CMiniMapOptionsLayout::MX_MMOI_HIGHLIGHT_STRUCTS );
		if ( pwStructs ) { pwStructs->SetCheck( _mmOptions.bHighlightStructs ); }

		try {
			auto pwBpr = FindChild( CMiniMapOptionsLayout::MX_MMOI_BPR_EDIT );
			if ( pwBpr ) { pwBpr->SetTextW( std::to_wstring( _mmOptions.ui32BytesPerRow ).c_str() ); }
			auto pwSnap = FindChild( CMiniMapOptionsLayout::MX_MMOI_AUTO_SNAP_EDIT );
			if ( pwSnap ) { pwSnap->SetTextW( std::to_wstring( _mmOptions.ui32SnapLines ).c_str() ); }
		}
		catch ( ... ) {}
		auto pwAutoSnap = FindChild( CMiniMapOptionsLayout::MX_MMOI_AUTO_SNAP_CHECK );
		if ( pwAutoSnap ) { pwAutoSnap->SetCheck( _mmOptions.bAutoSnap ); }

		auto ptbBright = static_cast<lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_BRIGHTNESS_TRACK ));
		if ( ptbBright ) { ptbBright->SetPos( TRUE, static_cast<LPARAM>(std::clamp<int32_t>( _mmOptions.i32Brightness, -100, 100 ) + 100) ); }
		auto ptbContrast = static_cast<lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_CONTRAST_TRACK ));
		if ( ptbContrast ) { ptbContrast->SetPos( TRUE, static_cast<LPARAM>(std::min<uint32_t>( _mmOptions.ui32Contrast, 100 )) ); }
		auto ptbZoom = static_cast<lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_ZOOM_TRACK ));
		if ( ptbZoom ) { ptbZoom->SetPos( TRUE, static_cast<LPARAM>(std::clamp<uint32_t>( _mmOptions.ui32Zoom, 1, 10 )) ); }

		UpdateEnabled();
		UpdateValueLabels();
	}

	/**
	 * Reads the options from the controls.  Values that cannot be read are kept from the options the dialog was given.
	 *
	 * \param _mmOptions Receives the options.
	 **/
	void CMiniMapOptionsDialog::GetControls( CMiniMap::MX_MINI_MAP &_mmOptions ) const {
		_mmOptions = m_mmOriginal;

		auto pcbPos = static_cast<const lsw::CComboBox *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_POSITION_COMBO ));
		if ( pcbPos ) { _mmOptions.bRightSize = pcbPos->GetCurSelItemData() != FALSE; }

		auto pwColorize = FindChild( CMiniMapOptionsLayout::MX_MMOI_COLORIZE_CHECK );
		auto pcbColors = static_cast<const lsw::CComboBox *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_COLORIZE_COMBO ));
		if ( pwColorize && pwColorize->IsChecked() ) {
			_mmOptions.cColors = pcbColors ? static_cast<CMiniMap::MX_COLORS>(pcbColors->GetCurSelItemData()) : g_mmpPalettes[0].cColors;
			if ( _mmOptions.cColors == CMiniMap::MX_C_NONE ) { _mmOptions.cColors = g_mmpPalettes[0].cColors; }
		}
		else {
			_mmOptions.cColors = CMiniMap::MX_C_NONE;
		}

		auto pwStructs = FindChild( CMiniMapOptionsLayout::MX_MMOI_HIGHLIGHT_STRUCTS );
		if ( pwStructs ) { _mmOptions.bHighlightStructs = pwStructs->IsChecked() != FALSE; }

		GetUInt( CMiniMapOptionsLayout::MX_MMOI_BPR_EDIT, _mmOptions.ui32BytesPerRow );
		GetUInt( CMiniMapOptionsLayout::MX_MMOI_AUTO_SNAP_EDIT, _mmOptions.ui32SnapLines );
		auto pwAutoSnap = FindChild( CMiniMapOptionsLayout::MX_MMOI_AUTO_SNAP_CHECK );
		if ( pwAutoSnap ) { _mmOptions.bAutoSnap = pwAutoSnap->IsChecked() != FALSE; }

		auto ptbBright = static_cast<const lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_BRIGHTNESS_TRACK ));
		if ( ptbBright ) { _mmOptions.i32Brightness = std::clamp<int32_t>( static_cast<int32_t>(ptbBright->GetPos()) - 100, -100, 100 ); }
		auto ptbContrast = static_cast<const lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_CONTRAST_TRACK ));
		if ( ptbContrast ) { _mmOptions.ui32Contrast = std::min<uint32_t>( ptbContrast->GetPos(), 100 ); }
		auto ptbZoom = static_cast<const lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_ZOOM_TRACK ));
		if ( ptbZoom ) { _mmOptions.ui32Zoom = std::clamp<uint32_t>( ptbZoom->GetPos(), 1, 10 ); }
	}

	/**
	 * Enables or disables the controls whose use depends on other controls (the palette, Bytes Per Line, and Auto Snap).
	 **/
	void CMiniMapOptionsDialog::UpdateEnabled() {
		auto pwColorize = FindChild( CMiniMapOptionsLayout::MX_MMOI_COLORIZE_CHECK );
		EnableCtrl( CMiniMapOptionsLayout::MX_MMOI_COLORIZE_COMBO, pwColorize && pwColorize->IsChecked() );

		// Auto Snap chooses the bytes per line from the width of the Mini Map.
		auto pwAutoSnap = FindChild( CMiniMapOptionsLayout::MX_MMOI_AUTO_SNAP_CHECK );
		const bool bAutoSnap = pwAutoSnap && pwAutoSnap->IsChecked();
		EnableCtrl( CMiniMapOptionsLayout::MX_MMOI_BPR_EDIT, !bAutoSnap );
		EnableCtrl( CMiniMapOptionsLayout::MX_MMOI_BPR_LABEL, !bAutoSnap );
		EnableCtrl( CMiniMapOptionsLayout::MX_MMOI_AUTO_SNAP_EDIT, bAutoSnap );
	}

	/**
	 * Updates the value labels beside the sliders.
	 **/
	void CMiniMapOptionsDialog::UpdateValueLabels() {
		try {
			auto ptbBright = static_cast<lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_BRIGHTNESS_TRACK ));
			auto pwBright = FindChild( CMiniMapOptionsLayout::MX_MMOI_BRIGHTNESS_VALUE );
			if ( ptbBright && pwBright ) {
				std::wstring wsText = std::to_wstring( static_cast<int32_t>(ptbBright->GetPos()) - 100 );
				wsText.push_back( L'%' );
				pwBright->SetTextW( wsText.c_str() );
			}
			auto ptbContrast = static_cast<lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_CONTRAST_TRACK ));
			auto pwContrast = FindChild( CMiniMapOptionsLayout::MX_MMOI_CONTRAST_VALUE );
			if ( ptbContrast && pwContrast ) {
				std::wstring wsText = std::to_wstring( ptbContrast->GetPos() );
				wsText.push_back( L'%' );
				pwContrast->SetTextW( wsText.c_str() );
			}
			auto ptbZoom = static_cast<lsw::CTrackBar *>(FindChild( CMiniMapOptionsLayout::MX_MMOI_ZOOM_TRACK ));
			auto pwZoom = FindChild( CMiniMapOptionsLayout::MX_MMOI_ZOOM_VALUE );
			if ( ptbZoom && pwZoom ) {
				pwZoom->SetTextW( std::to_wstring( ptbZoom->GetPos() ).c_str() );
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Reads the options from the controls and shows them in the hex editor.
	 **/
	void CMiniMapOptionsDialog::Apply() {
		if ( !m_bReady || !m_pmmopParms ) { return; }
		GetControls( m_pmmopParms->mmOptions );
		if ( m_pmmopParms->phecControl ) {
			m_pmmopParms->phecControl->SetMiniMapOptions( m_pmmopParms->mmOptions );
		}
	}

	/**
	 * Puts back the original options and closes the dialog.
	 **/
	void CMiniMapOptionsDialog::Cancel() {
		if ( m_pmmopParms ) {
			m_pmmopParms->mmOptions = m_mmOriginal;
			m_pmmopParms->bHide = false;
			if ( m_pmmopParms->phecControl ) {
				m_pmmopParms->phecControl->SetMiniMapOptions( m_mmOriginal );
			}
		}
		::EndDialog( Wnd(), FALSE );
	}

	/**
	 * Enables or disables a control.
	 *
	 * \param _wId The control to enable or disable.
	 * \param _bEnable If true, the control is enabled.
	 **/
	void CMiniMapOptionsDialog::EnableCtrl( WORD _wId, bool _bEnable ) {
		auto pwThis = FindChild( _wId );
		if ( pwThis ) { ::EnableWindow( pwThis->Wnd(), _bEnable ? TRUE : FALSE ); }
	}

	/**
	 * Reads an unsigned number from an edit control.
	 *
	 * \param _wId The edit control.
	 * \param _ui32Value Receives the value if it can be read.
	 * \return Returns true if the edit holds a number greater than 0.
	 **/
	bool CMiniMapOptionsDialog::GetUInt( WORD _wId, uint32_t &_ui32Value ) const {
		auto pwEdit = FindChild( _wId );
		if ( !pwEdit ) { return false; }
		try {
			const std::wstring wsText = pwEdit->GetTextW();
			if ( wsText.empty() ) { return false; }
			wchar_t * pwcEnd = nullptr;
			const unsigned long ulValue = std::wcstoul( wsText.c_str(), &pwcEnd, 10 );
			if ( !ulValue || ulValue > 0xFFFF ) { return false; }
			_ui32Value = static_cast<uint32_t>(ulValue);
			return true;
		}
		catch ( ... ) { return false; }
	}

}	// namespace mx

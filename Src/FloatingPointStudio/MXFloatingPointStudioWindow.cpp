#include "MXFloatingPointStudioWindow.h"
#include "../Layouts/MXFloatingPointStudioLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"

#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <CheckButton/LSWCheckButton.h>
#include <EEExpEval.h>
#include <Static/LSWStatic.h>

#include <format>

namespace mx {

	// == Members.
	// The main window class.
	ATOM CFloatingPointStudioWindow::m_aAtom = 0;

	// The database of float types.
	CFloatingPointStudioWindow::MX_PRESET CFloatingPointStudioWindow::m_pPresets[] = {
		//	bSign	bImpBit	ui8ExpBits	ui8ManBits	pId
		{ _T_LEN_7D9829FB_half_float16_IEEE_754_2008_binary16_s10e5_fp16,
			true,	true,	5,			11,			MX_P_FLOAT16 },
		{ _T_LEN_D119A670_float_float32_IEEE_754_single_precision_IEEE_754_2008_binary32,
			true,	true,	8,			24,			MX_P_FLOAT32 },
		{ _T_LEN_C8A406E9_double_float64_IEEE_754_double_precision_IEEE_754_2008_binary64,
			true,	true,	11,			53,			MX_P_FLOAT64 },
		{ _T_LEN_4228F19E_6_bit__Study_Material_,
			true,	false,	3,			2,			MX_P_6_BIT },
		{ _T_LEN_899A8D1E_8_bit__Study_Material_,
			true,	false,	4,			3,			MX_P_8_BIT },
		{ _T_LEN_1D2DCE46_10_bit__Graphics__EG__GL_R11F_G11F_B10F_,
			false,	false,	5,			5,			MX_P_10_BIT },
		{ _T_LEN_53A4C5EF_11_bit__Graphics__EG__GL_R11F_G11F_B10F_,
			false,	false,	5,			6,			MX_P_11_BIT },
		{ _T_LEN_A25BEC0C_bfloat16,
			true,	true,	8,			8,			MX_P_BFLOAT16 },
		{ _T_LEN_1AD9610B_Nvidia_TensorFloat_32,
			true,	false,	8,			10,			MX_P_TENSORFLOAT32 },
		{ _T_LEN_4109FEE0_AMD_f24,
			true,	true,	7,			17,			MX_P_AMD_F24 },
		{ _T_LEN_80FCF432_Pixar_PXR24,
			true,	false,	8,			15,			MX_P_PXR24 },
		{ _T_LEN_C277C193_AMD_9511__1979_,
			false,	false,	7,			23,			MX_P_AMD_9511 },
		{ _T_LEN_F22813AD_Custom,
			false,	false,	0,			0,			MX_P_CUSTOM },
	};

	CFloatingPointStudioWindow::CFloatingPointStudioWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pwLastInput( nullptr ),
		m_bUpdatingInputs( false ),
		m_pmhMemHack( reinterpret_cast<CMemHack *>(_ui64Data) ) {
		
	}
	CFloatingPointStudioWindow::~CFloatingPointStudioWindow() {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CFloatingPointStudioWindow::InitDialog() {
		int iX = ::GetSystemMetrics( SM_CXMENUCHECK );
		int iY = ::GetSystemMetrics( SM_CYMENUCHECK );
		const int iShave = MX_FPS_CHECKBOX_TRIM;

		LONG lTop = MX_FPS_INPUT_AREA + MX_FPS_PARMS_GROUP_HEIGHT;
		LONG lLeft = 0;

		lLeft = 0;
		for ( WORD I = 0; I < 65; ++I ) {
			const WCHAR * pwcLabel = L"S";
			if ( I == 0 ) {}
			else if ( I >= 1 && I < ee::CFloatX::MaxExpBits() + 1 ) {
				pwcLabel = L"E";
			}
			else if ( I == ee::CFloatX::MaxExpBits() + 1 ) {
				pwcLabel = L"I";
			}
			else {
				pwcLabel = L"M";
			}
			LSW_WIDGET_LAYOUT wlLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->FixLayout( LSW_WIDGET_LAYOUT{
				LSW_LT_LABEL,							// ltType
				static_cast<WORD>(CFloatingPointStudioLayout::MX_CI_KEY_LABEL_START + I),	// wId
				nullptr,								// lpwcClass
				TRUE,									// bEnabled
				TRUE,									// bActive
				lLeft + iShave,							// iLeft
				lTop + iShave,							// iTop
				static_cast<DWORD>(iX - iShave * 2),	// dwWidth
				static_cast<DWORD>(MX_DEF_STATIC_HEIGHT_PXL),								// dwHeight
				MX_STATICSTYLE | SS_CENTER,				// dwStyle
				0,										// dwStyleEx
				pwcLabel,								// pwcText
				0,										// sTextLen
				static_cast<DWORD>(CFloatingPointStudioLayout::MX_CI_MAINWINDOW),			// dwParentId

			} );
			CStatic * psStatic = static_cast<CStatic *>(static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->CreateWidget( wlLayout, this, TRUE, NULL, 0 ));
			if ( psStatic ) {
				::SendMessageW( psStatic->Wnd(), WM_SETFONT, reinterpret_cast<WPARAM>(CBase::MessageBoxFont()), TRUE );
				/*LSW_RECT rRect;
				rRect.Zero();
				Button_GetIdealSize( psStatic->Wnd(), &rRect );
				if ( rRect.Width() ) {
				}*/
			}
			/*if ( pbButton ) {
				LSW_RECT rRect;
				Button_GetIdealSize( pbButton->Wnd(), &rRect );
				if ( rRect.Width() ) {
				}
			}*/
			lLeft += iX - iShave * 2;
		}
		lTop += MX_DEF_STATIC_HEIGHT_PXL;
		lLeft = 0;
		for ( WORD I = 0; I < 65; ++I ) {
			LSW_WIDGET_LAYOUT wlLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->FixLayout( LSW_WIDGET_LAYOUT{
				LSW_LT_CHECK,							// ltType
				static_cast<WORD>(CFloatingPointStudioLayout::MX_CI_CHECK_START + I),	// wId
				nullptr,								// lpwcClass
				TRUE,									// bEnabled
				FALSE,									// bActive
				lLeft + iShave,							// iLeft
				lTop + iShave,							// iTop
				static_cast<DWORD>(iX - iShave * 2),	// dwWidth
				static_cast<DWORD>(iY - iShave * 2),	// dwHeight
				MX_CHECKSTYLE,							// dwStyle
				0,										// dwStyleEx
				nullptr,								// pwcText
				0,										// sTextLen
				static_cast<DWORD>(CFloatingPointStudioLayout::MX_CI_MAINWINDOW),		// dwParentId
			} );
			CCheckButton * pbButton = static_cast<CCheckButton *>(static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->CreateWidget( wlLayout, this, TRUE, NULL, 0 ));
			/*if ( pbButton ) {
				LSW_RECT rRect;
				Button_GetIdealSize( pbButton->Wnd(), &rRect );
				if ( rRect.Width() ) {
				}
			}*/
			lLeft += iX - iShave * 2;
		}
		lTop += iY - iShave * 2;
		lLeft = 0;
		for ( WORD I = 0; I < 65; ++I ) {
			WCHAR szBuffer[8];
			if ( I == 0 ) {
				std::swprintf( szBuffer, L"00" );
			}
			else if ( I >= 1 && I < ee::CFloatX::MaxExpBits() + 1 ) {
				std::swprintf( szBuffer, L"%.2u", I - 1 );
			}
			else if ( I == ee::CFloatX::MaxExpBits() + 1 ) {
				std::swprintf( szBuffer, L"XX" );
			}
			else {
				std::swprintf( szBuffer, L"%.2u", I - (ee::CFloatX::MaxExpBits() + 1) - 1 );
			}
			// Insert the line break.
			szBuffer[3] = szBuffer[1];
			szBuffer[1] = L'\r';
			szBuffer[2] = L'\n';
			szBuffer[4] = L'\0';

			LSW_WIDGET_LAYOUT wlLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->FixLayout( LSW_WIDGET_LAYOUT{
				LSW_LT_LABEL,							// ltType
				static_cast<WORD>(CFloatingPointStudioLayout::MX_CI_LABEL_START + I),	// wId
				nullptr,								// lpwcClass
				TRUE,									// bEnabled
				TRUE,									// bActive
				lLeft + iShave,							// iLeft
				lTop + iShave,							// iTop
				static_cast<DWORD>(iX - iShave * 2),	// dwWidth
				static_cast<DWORD>(MX_DEF_STATIC_HEIGHT_PXL * 2),						// dwHeight
				MX_STATICSTYLE | SS_CENTER,				// dwStyle
				0,										// dwStyleEx
				szBuffer,								// pwcText
				0,										// sTextLen
				static_cast<DWORD>(CFloatingPointStudioLayout::MX_CI_MAINWINDOW),		// dwParentId
			} );
			CStatic * psStatic = static_cast<CStatic *>(static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->CreateWidget( wlLayout, this, TRUE, NULL, 0 ));
			if ( psStatic ) {
				::SendMessageW( psStatic->Wnd(), WM_SETFONT, reinterpret_cast<WPARAM>(CBase::MessageBoxFont()), TRUE );
				/*LSW_RECT rRect;
				rRect.Zero();
				Button_GetIdealSize( psStatic->Wnd(), &rRect );
				if ( rRect.Width() ) {
				}*/
			}
			/*if ( pbButton ) {
				LSW_RECT rRect;
				Button_GetIdealSize( pbButton->Wnd(), &rRect );
				if ( rRect.Width() ) {
				}
			}*/
			lLeft += iX - iShave * 2;
		}
		//::EnumChildWindows( Wnd(), CWidget::EnumChildWindows_SetFont, reinterpret_cast<LPARAM>(CBase::MessageBoxFont()) );


		CWidget * pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_SIGNBIT_CHECK );
		if ( pwWidget ) {
			pwWidget->SetCheck( TRUE );
		}
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_EXP_EDIT );
		if ( pwWidget ) {
			pwWidget->SetTextW( std::to_wstring( EE_FLOATX_DBL_EXP_BITS ).c_str() );
		}
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_IMPBIT_CHECK );
		if ( pwWidget ) {
			pwWidget->SetCheck( TRUE );
		}
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_MAN_EDIT );
		if ( pwWidget ) {
			pwWidget->SetTextW( std::to_wstring( EE_FLOATX_DBL_MAN_BITS ).c_str() );
		}

		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CFloatingPointStudioLayout::MX_CI_PARM_PRESET_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < MX_ELEMENTS( m_pPresets ); ++I ) {
				INT iIndex = pcbCombo->AddString( mx::CStringDecoder::DecodeToWString( m_pPresets[I].pcEncryptedName, m_pPresets[I].sNameLen ).c_str() );
				if ( iIndex != -1 ) {
					pcbCombo->SetItemData( iIndex, static_cast<LPARAM>(m_pPresets[I].pId) );
				}
			}
			pcbCombo->SetCurSelByItemData( MX_P_FLOAT64 );
			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CFloatingPointStudioLayout::MX_CI_OPTIONS_OUT_FORMAT_COMBO ));
		if ( pcbCombo ) {
			static struct MX_FMT_COMB {
				const char *									pcStr;
				size_t											sLen;
				int												iValue;
			} fcStrings[] = {
				{ _T_LEN_2CE33943_Default,						MX_DEFAULT },
				{ _T_LEN_9631F7BC_Scientific,					MX_SCIENTIFIC },
				//{ _T_LEN_A3A5D702_sprintf____17f_,				MX_SPRINTF },
				{ _T_LEN_78A47C44_Python_____17f__,				MX_PYTHON },
			};
			for ( size_t I = 0; I < MX_ELEMENTS( fcStrings ); ++I ) {
				INT iIndex = pcbCombo->AddString( mx::CStringDecoder::DecodeToWString( fcStrings[I].pcStr, fcStrings[I].sLen ).c_str() );
				if ( iIndex != -1 ) {
					pcbCombo->SetItemData( iIndex, static_cast<LPARAM>(fcStrings[I].iValue) );
				}
			}
			pcbCombo->SetCurSelByItemData( MX_DEFAULT );
			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CFloatingPointStudioLayout::MX_CI_OPTIONS_ROUND_COMBO ));
		if ( pcbCombo ) {
			static struct MX_FMT_COMB {
				const char *									pcStr;
				size_t											sLen;
				int												iValue;
			} fcStrings[] = {
				{ _T_LEN_1DD6529B_To_Nearest,					FE_TONEAREST },
				{ _T_LEN_FC785F2F_Upward,						FE_UPWARD },
				{ _T_LEN_6D055C20_Downward,						FE_DOWNWARD },
				{ _T_LEN_08746C74_Toward_Zero,					FE_TOWARDZERO },
			};
			for ( size_t I = 0; I < MX_ELEMENTS( fcStrings ); ++I ) {
				INT iIndex = pcbCombo->AddString( mx::CStringDecoder::DecodeToWString( fcStrings[I].pcStr, fcStrings[I].sLen ).c_str() );
				if ( iIndex != -1 ) {
					pcbCombo->SetItemData( iIndex, static_cast<LPARAM>(fcStrings[I].iValue) );
				}
			}
			if ( m_pmhMemHack && (m_pmhMemHack->FloatStudioOptions().iFloatStudioRoundMode == FE_TONEAREST ||
				m_pmhMemHack->FloatStudioOptions().iFloatStudioRoundMode == FE_UPWARD ||
				m_pmhMemHack->FloatStudioOptions().iFloatStudioRoundMode == FE_DOWNWARD ||
				m_pmhMemHack->FloatStudioOptions().iFloatStudioRoundMode == FE_TOWARDZERO) ) {
				pcbCombo->SetCurSelByItemData( m_pmhMemHack->FloatStudioOptions().iFloatStudioRoundMode );
			}
			else { pcbCombo->SetCurSelByItemData( FE_TONEAREST ); }
			pcbCombo->AutoSetMinListWidth();
		}
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CFloatingPointStudioWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		if ( _Id >= CFloatingPointStudioLayout::MX_CI_CHECK_START + 0 &&
			_Id < CFloatingPointStudioLayout::MX_CI_CHECK_START + 65 ) {
			UpdateChecks();
		}
		else {
			switch ( _Id ) {
				case CFloatingPointStudioLayout::MX_CI_PARM_PRESET_COMBO : {
					switch ( _wCtrlCode ) {
						case LBN_SELCHANGE : {
							CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( _Id ));
							if ( pcbCombo ) {
								LPARAM lpSel = pcbCombo->GetCurSelItemData();
								if ( lpSel != MX_P_CUSTOM ) {
									for ( size_t I = 0; I < MX_ELEMENTS( m_pPresets ); ++I ) {
										if ( m_pPresets[I].pId == lpSel ) {

											CWidget * pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_SIGNBIT_CHECK );
											if ( pwWidget ) {
												pwWidget->SetCheck( m_pPresets[I].bSign );
											}
											pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_EXP_EDIT );
											if ( pwWidget ) {
												pwWidget->SetTextW( std::to_wstring( m_pPresets[I].ui8ExpBits ).c_str() );
											}
											pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_IMPBIT_CHECK );
											if ( pwWidget ) {
												pwWidget->SetCheck( m_pPresets[I].bImpBit );
											}
											pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_MAN_EDIT );
											if ( pwWidget ) {
												pwWidget->SetTextW( std::to_wstring( m_pPresets[I].ui8ManBits ).c_str() );
											}

											UpdateByLast();
											break;
										}
									}
								}
							}
							break;
						}
					}
					break;
				}
				case CFloatingPointStudioLayout::MX_CI_PARM_EXP_EDIT : {}
				case CFloatingPointStudioLayout::MX_CI_PARM_MAN_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						UpdateByLast();
					}
					break;
				}
				case CFloatingPointStudioLayout::MX_CI_PARM_SIGNBIT_CHECK : {}
				case CFloatingPointStudioLayout::MX_CI_PARM_IMPBIT_CHECK : {
					if ( _wCtrlCode == STN_CLICKED ) {
						UpdateByLast();
					}
					break;
				}

				case CFloatingPointStudioLayout::MX_CI_VALUE_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						if ( m_bUpdatingInputs ) { break; }
						CWidget * pwWidget = FindChild( _Id );
						m_pwLastInput = pwWidget;
						UpdateDefault( pwWidget );
					}
					break;
				}
				case CFloatingPointStudioLayout::MX_CI_VALUE_HEX_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						if ( m_bUpdatingInputs ) { break; }
						CWidget * pwWidget = FindChild( _Id );
						m_pwLastInput = pwWidget;
						UpdateHex( pwWidget );
					}
					break;
				}
				case CFloatingPointStudioLayout::MX_CI_VALUE_BIN_EDIT : {
					if ( _wCtrlCode == EN_CHANGE ) {
						if ( m_bUpdatingInputs ) { break; }
						CWidget * pwWidget = FindChild( _Id );
						m_pwLastInput = pwWidget;
						UpdateBinary( pwWidget );
					}
					break;
				}
				case CFloatingPointStudioLayout::MX_CI_OPTIONS_OUT_FORMAT_COMBO : {
					switch ( _wCtrlCode ) {
						case LBN_SELCHANGE : {
							CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( _Id ));
							if ( pcbCombo ) {
								LPARAM lpSel = pcbCombo->GetCurSelItemData();
								UpdateByLast();
							}
						}
					}
					break;
				}
				case CFloatingPointStudioLayout::MX_CI_OPTIONS_OUT_FORMAT_EDIT_COMBO : {
					if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
						UpdateByLast();
					}
					break;
				}
				case CFloatingPointStudioLayout::MX_CI_OPTIONS_ROUND_COMBO : {
					if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
						UpdateByLast();
					}
					break;
				}
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CFloatingPointStudioWindow::Close() {
		MX_CUR_SETTINGS csSettings;
		GatherSettings( csSettings );
		if ( m_pmhMemHack ) {
			m_pmhMemHack->FloatStudioOptions().iFloatStudioRoundMode = csSettings.iRoundingMode;
		}
		::DestroyWindow( Wnd() );
		//CUtilities::PrintTotalGuiObjects( GR_GDIOBJECTS );
		return LSW_H_HANDLED;
	}

	// WM_ERASEBKGND.
	CWidget::LSW_HANDLED CFloatingPointStudioWindow::EraseBkgnd( HDC _hDc ) {
		return LSW_H_HANDLED;
	}

	// Prepares to create the window.  Creates the atom if necessary.
	void CFloatingPointStudioWindow::PrepareFloatingPointStudio() {
		if ( !m_aAtom ) {
			lsw::CWndClassEx wceEx;
			wceEx.SetInstance( lsw::CBase::GetThisHandle() );
			WCHAR szStr[9];
			mx::CUtilities::RandomString( szStr, MX_ELEMENTS( szStr ) );
			wceEx.SetClassName( szStr );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(CTLCOLOR_DLG + 1) );
			wceEx.SetWindPro( CWidget::WindowProc );
			m_aAtom = lsw::CBase::RegisterClassExW( wceEx.Obj() );
		}
	}

	// Gathers the floating-point settings.
	bool CFloatingPointStudioWindow::GatherSettings( MX_CUR_SETTINGS &_csSettings ) {
		CWidget * pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_SIGNBIT_CHECK );
		if ( pwWidget ) {
			_csSettings.bSign = pwWidget->IsChecked();
		}
		else { return false; }
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_EXP_EDIT );
		if ( pwWidget ) {
			ee::CExpEvalContainer::EE_RESULT rRes;
			if ( !pwWidget->GetTextAsUInt64Expression( rRes ) ) { return false; }
			if ( rRes.u.ui64Val > ee::CFloatX::MaxExpBits() ) { return false; }
			_csSettings.ui8ExpBits = rRes.u.ui64Val;
		}
		else { return false; }
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_IMPBIT_CHECK );
		if ( pwWidget ) {
			_csSettings.bImpBit = pwWidget->IsChecked();
		}
		else { return false; }
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_PARM_MAN_EDIT );
		if ( pwWidget ) {
			ee::CExpEvalContainer::EE_RESULT rRes;
			if ( !pwWidget->GetTextAsUInt64Expression( rRes ) ) { return false; }
			if ( rRes.u.ui64Val - _csSettings.bImpBit > ee::CFloatX::MaxManBits() - 1/*- _csSettings.bImpBit*/ ) { return false; }
			_csSettings.ui8ManBits = rRes.u.ui64Val;
		}
		else { return false; }

		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CFloatingPointStudioLayout::MX_CI_OPTIONS_OUT_FORMAT_COMBO ));
		if ( pcbCombo ) {
			_csSettings.iOutFormat = static_cast<int>(pcbCombo->GetCurSelItemData());
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CFloatingPointStudioLayout::MX_CI_OPTIONS_OUT_FORMAT_EDIT_COMBO ));
		if ( pcbCombo ) {
			_csSettings.wsFormatString = pcbCombo->GetTextW();
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CFloatingPointStudioLayout::MX_CI_OPTIONS_ROUND_COMBO ));
		if ( pcbCombo ) {
			_csSettings.iRoundingMode = static_cast<int>(pcbCombo->GetCurSelItemData());
		}

		return true;
	}

	// Updates the inputs based on the current settings and a value, skipping m_pwLastInput.
	void CFloatingPointStudioWindow::UpdateInputs( const ee::CFloatX &_fVal, const MX_CUR_SETTINGS &_csSettings ) {
		CWidget * pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_VALUE_EDIT );
		if ( pwWidget && m_pwLastInput != pwWidget ) {
			SetFloatText( pwWidget, _fVal.AsDouble(), _csSettings );
		}

		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_VALUE_HEX_EDIT );
		if ( pwWidget && m_pwLastInput != pwWidget ) {
			std::string sTemp;
			CUtilities::ToHex( _fVal.AsUint64(), sTemp, (ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits /*- _csSettings.bImpBit*/, _csSettings.bImpBit, _csSettings.bSign ) + 3) / 4 );
			pwWidget->SetTextA( sTemp.c_str() );
		}

		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_VALUE_BIN_EDIT );
		if ( pwWidget && m_pwLastInput != pwWidget ) {
			std::string sTemp;
			CUtilities::ToBinary( _fVal.AsUint64(), sTemp, ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits /*- _csSettings.bImpBit*/, _csSettings.bImpBit, _csSettings.bSign ) );
			pwWidget->SetTextA( sTemp.c_str() );
		}

		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OPTIONS_OUT_FORMAT_EDIT_COMBO );
		if ( pwWidget ) {
			pwWidget->SetEnabled( _csSettings.iOutFormat != MX_DEFAULT && _csSettings.iOutFormat != MX_SCIENTIFIC );
		}

		UpdateChecks( _fVal, _csSettings );
		UpdateLimits( _csSettings );
		UpdatePreset( _csSettings );
		UpdateMisc( _fVal, _csSettings );
	}

	// Updates the inputs based on whatever was last.
	void CFloatingPointStudioWindow::UpdateByLast() {
		if ( m_pwLastInput == FindChild( CFloatingPointStudioLayout::MX_CI_VALUE_EDIT ) ) {
			UpdateDefault( m_pwLastInput );
		}
		else if ( m_pwLastInput == FindChild( CFloatingPointStudioLayout::MX_CI_VALUE_HEX_EDIT ) ) {
			UpdateHex( m_pwLastInput );
		}
		else if ( m_pwLastInput == FindChild( CFloatingPointStudioLayout::MX_CI_VALUE_BIN_EDIT ) ) {
			UpdateBinary( m_pwLastInput );
		}
		else {
			UpdateChecks();
		}
	}

	// Updates the inputs based on the expression input.
	void CFloatingPointStudioWindow::UpdateDefault( CWidget * _pwWidget ) {
		MX_CUR_SETTINGS csSettings;
		if ( !GatherSettings( csSettings ) ) { return; }

		ee::CExpEvalContainer::EE_RESULT rRes;
		{
			ee::CExpEval::EE_FEROUNDMODE rmRound( csSettings.iRoundingMode );
		
			if ( !_pwWidget->GetTextAsDoubleExpression( rRes ) ) { return; }
		}
		
		ee::CFloatX fFloat;
		fFloat.CreateFromDouble( rRes.u.dVal,
			csSettings.ui8ExpBits, csSettings.ui8ManBits /*+ csSettings.bImpBit*/, csSettings.bImpBit, csSettings.bSign );
		m_bUpdatingInputs = true;
		UpdateInputs( fFloat, csSettings );
		m_bUpdatingInputs = false;
	}

	// Updates the inputs based on the hex input.
	void CFloatingPointStudioWindow::UpdateHex( CWidget * _pwWidget ) {
		MX_CUR_SETTINGS csSettings;
		if ( !GatherSettings( csSettings ) ) { return; }

		ee::CExpEvalContainer::EE_RESULT rRes;
		_pwWidget->SetTreatAsHex( TRUE );
		if ( !_pwWidget->GetTextAsUInt64Expression( rRes ) ) { return; }
		
		ee::CFloatX fFloat;
		fFloat.CreateFromBits( rRes.u.ui64Val,
			csSettings.ui8ExpBits, csSettings.ui8ManBits /*+ csSettings.bImpBit*/, csSettings.bImpBit, csSettings.bSign );
		m_bUpdatingInputs = true;
		UpdateInputs( fFloat, csSettings );
		m_bUpdatingInputs = false;
	}

	// Updates the inputs based on the binary input.
	void CFloatingPointStudioWindow::UpdateBinary( CWidget * _pwWidget ) {
		MX_CUR_SETTINGS csSettings;
		if ( !GatherSettings( csSettings ) ) { return; }

		uint64_t ui64Val = ee::CExpEval::StoULL( _pwWidget->GetTextA().c_str(), 2 );
		
		ee::CFloatX fFloat;
		fFloat.CreateFromBits( ui64Val,
			csSettings.ui8ExpBits, csSettings.ui8ManBits /*+ csSettings.bImpBit*/, csSettings.bImpBit, csSettings.bSign );
		m_bUpdatingInputs = true;
		UpdateInputs( fFloat, csSettings );
		m_bUpdatingInputs = false;
	}

	// Updates the inputs based on the checkboxes.
	void CFloatingPointStudioWindow::UpdateChecks() {
		MX_CUR_SETTINGS csSettings;
		if ( !GatherSettings( csSettings ) ) { return; }

		ee::CFloatX fFloat;
		uint64_t uiVal = 0;
		CWidget * pwWidget;
		if ( csSettings.bSign ) {
			pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_CHECK_START + 0 );
			if ( pwWidget ) {
				uiVal |= pwWidget->IsChecked() ? 1 : 0;
			}
		}

		for ( WORD I = 0; I < csSettings.ui8ExpBits; ++I ) {
			pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_CHECK_START + 1 + I );
			uiVal <<= 1;
			if ( pwWidget ) {
				uiVal |= pwWidget->IsChecked() ? 1 : 0;
			}
		}

		for ( WORD I = 0; I < csSettings.ui8ManBits - csSettings.bImpBit; ++I ) {
			pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_CHECK_START + 1 + ee::CFloatX::MaxExpBits() + 1 + I );
			uiVal <<= 1;
			if ( pwWidget ) {
				uiVal |= pwWidget->IsChecked() ? 1 : 0;
			}
		}

		fFloat.CreateFromBits( uiVal,
			csSettings.ui8ExpBits, csSettings.ui8ManBits /*+ csSettings.bImpBit*/, csSettings.bImpBit, csSettings.bSign );
		m_bUpdatingInputs = true;
		m_pwLastInput = nullptr;
		UpdateInputs( fFloat, csSettings );
		m_bUpdatingInputs = false;
	}

	// Updates the check boxes.
	void CFloatingPointStudioWindow::UpdateChecks( const ee::CFloatX &_fVal, const MX_CUR_SETTINGS &_csSettings ) {
		UpdateChecks( _csSettings );
		CWidget * pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_CHECK_START + 0 );
		if ( pwWidget ) {
			pwWidget->SetCheck( _csSettings.bSign && _fVal.SignBit() );
		}

		for ( WORD I = 0; I < ee::CFloatX::MaxExpBits(); ++I ) {
			pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_CHECK_START + 1 + I );
			if ( pwWidget ) {
				pwWidget->SetCheck( I < _csSettings.ui8ExpBits && (_fVal.ExpBits() & (1ULL << (_csSettings.ui8ExpBits - I - 1))) );
			}
		}

		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_CHECK_START + 1 + ee::CFloatX::MaxExpBits() );
		if ( pwWidget ) {
			pwWidget->SetCheck( _csSettings.bImpBit && !_fVal.IsDenormalized() );
		}

		// MaxManBits() includes the implicit bit.
		for ( WORD I = 0; I < ee::CFloatX::MaxManBits() - 1; ++I ) {
			pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_CHECK_START + 1 + ee::CFloatX::MaxExpBits() + 1 + I );
			if ( pwWidget ) {
				pwWidget->SetCheck( I < _csSettings.ui8ManBits - _csSettings.bImpBit && (_fVal.ManBits() & (1ULL << (_csSettings.ui8ManBits - I - 1 - _csSettings.bImpBit))) );
			}
		}
	}

	// Updates the check boxes (only enable/disable).
	void CFloatingPointStudioWindow::UpdateChecks( const MX_CUR_SETTINGS &_csSettings ) {
		CWidget * pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_CHECK_START + 0 );
		if ( pwWidget ) {
			pwWidget->SetEnabled( _csSettings.bSign );
			if ( !_csSettings.bSign ) {
				pwWidget->SetCheck( false );
			}
		}
		for ( WORD I = 0; I < ee::CFloatX::MaxExpBits(); ++I ) {
			pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_CHECK_START + 1 + I );
			if ( pwWidget ) {
				pwWidget->SetEnabled( I < _csSettings.ui8ExpBits );
				if ( I >= _csSettings.ui8ExpBits ) {
					pwWidget->SetCheck( false );
				}
			}
		}

		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_CHECK_START + 1 + ee::CFloatX::MaxExpBits() );
		if ( pwWidget ) {
			pwWidget->SetEnabled( _csSettings.bImpBit );
			if ( !_csSettings.bImpBit ) {
				pwWidget->SetCheck( false );
			}
		}

		// MaxManBits() includes the implicit bit.
		for ( WORD I = 0; I < ee::CFloatX::MaxManBits() - 1; ++I ) {
			pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_CHECK_START + 1 + ee::CFloatX::MaxExpBits() + 1 + I );
			if ( pwWidget ) {
				pwWidget->SetEnabled( I < _csSettings.ui8ManBits - _csSettings.bImpBit );
				if ( I >= _csSettings.ui8ManBits ) {
					pwWidget->SetCheck( false );
				}
			}
		}
	}

	// Updates the limit edits.
	void CFloatingPointStudioWindow::UpdateLimits( const MX_CUR_SETTINGS &_csSettings ) {
		ee::CFloatX fThis;
		fThis.CreateInfP( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign );

		CWidget * pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_PINF_EDIT_HEX );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToHex( fThis.AsUint64(), sTemp, (ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign ) + 3) / 4 );
			pwWidget->SetTextA( sTemp.c_str() );
		}
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_PINF_EDIT_BIN );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToBinary( fThis.AsUint64(), sTemp, ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign ) );
			pwWidget->SetTextA( sTemp.c_str() );
		}


		fThis.CreateInfN( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign );
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_NINF_EDIT_HEX );
		if ( pwWidget ) {
			if ( !_csSettings.bSign ) {
				pwWidget->SetTextA( "" );
			}
			else {
				std::string sTemp;
				CUtilities::ToHex( fThis.AsUint64(), sTemp, (ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign ) + 3) / 4 );
				pwWidget->SetTextA( sTemp.c_str() );
			}
		}
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_NINF_EDIT_BIN );
		if ( pwWidget ) {
			if ( !_csSettings.bSign ) {
				pwWidget->SetTextA( "" );
			}
			else {
				std::string sTemp;
				CUtilities::ToBinary( fThis.AsUint64(), sTemp, ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign ) );
				pwWidget->SetTextA( sTemp.c_str() );
			}
		}


		fThis.CreateNaN( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign );
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_NAN_EDIT_HEX );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToHex( fThis.AsUint64(), sTemp, (ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign ) + 3) / 4 );
			pwWidget->SetTextA( sTemp.c_str() );
		}
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_NAN_EDIT_BIN );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToBinary( fThis.AsUint64(), sTemp, ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign ) );
			pwWidget->SetTextA( sTemp.c_str() );
		}


		fThis.CreateMax( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign );
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_MAX_EDIT_HEX );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToHex( fThis.AsUint64(), sTemp, (ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign ) + 3) / 4 );
			pwWidget->SetTextA( sTemp.c_str() );
		}
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_MAX_EDIT_FLT );
		if ( pwWidget ) {
			SetFloatText( pwWidget, fThis.AsDouble(), _csSettings );
		}


		fThis.CreateMin( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign );
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_MIN_EDIT_HEX );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToHex( fThis.AsUint64(), sTemp, (ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign ) + 3) / 4 );
			pwWidget->SetTextA( sTemp.c_str() );
		}
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_MIN_EDIT_FLT );
		if ( pwWidget ) {
			SetFloatText( pwWidget, fThis.AsDouble(), _csSettings );
		}


		fThis.CreateMinNormalized( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign );
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_NMIN_EDIT_HEX );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToHex( fThis.AsUint64(), sTemp, (ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign ) + 3) / 4 );
			pwWidget->SetTextA( sTemp.c_str() );
		}
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_NMIN_EDIT_FLT );
		if ( pwWidget ) {
			SetFloatText( pwWidget, fThis.AsDouble(), _csSettings );
		}


		fThis.CreateEpsilon( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign );
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_EPS_EDIT_HEX );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToHex( fThis.AsUint64(), sTemp, (ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign ) + 3) / 4 );
			pwWidget->SetTextA( sTemp.c_str() );
		}
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_EPS_EDIT_FLT );
		if ( pwWidget ) {
			SetFloatText( pwWidget, fThis.AsDouble(), _csSettings );
		}


		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_BIAS_EDIT );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToSigned( static_cast<int64_t>(ee::CFloatX::ExpBias( _csSettings.ui8ExpBits )), sTemp );
			pwWidget->SetTextA( sTemp.c_str() );
		}

		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_OUT_PREC_EDIT );
		if ( pwWidget ) {
			SetFloatText( pwWidget, ee::CFloatX::Precision( _csSettings.ui8ManBits ), _csSettings );
		}
	}

	// Updates the Misc. section.
	void CFloatingPointStudioWindow::UpdateMisc( const ee::CFloatX &_fVal, const MX_CUR_SETTINGS &_csSettings ) {
		double dVal = _fVal.AsDouble();
		ee::CFloatX fCopy;
		fCopy.CreateNextAfterUp( dVal, _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign );

		CWidget * pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_MISC_NEXT_ABOVE_EDIT );
		if ( pwWidget ) {
			SetFloatText( pwWidget, fCopy.AsDouble(), _csSettings );
		}

		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_MISC_ACTUAL_EDIT );
		if ( pwWidget ) {
			SetFloatText( pwWidget, dVal, _csSettings );
		}

		fCopy.CreateNextAfterDown( dVal, _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign );
		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_MISC_NEXT_BELOW_EDIT );
		if ( pwWidget ) {
			SetFloatText( pwWidget, fCopy.AsDouble(), _csSettings );
		}

		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_MISC_TOTAL_BITS_EDIT );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToUnsigned( ee::CFloatX::TotalBits( _csSettings.ui8ExpBits, _csSettings.ui8ManBits, _csSettings.bImpBit, _csSettings.bSign ), sTemp );
			pwWidget->SetTextA( sTemp.c_str() );
		}

		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_MISC_EXPONENT_EDIT );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToHex( _fVal.ExpBits(), sTemp, (_csSettings.ui8ExpBits + 3) / 4 );
			pwWidget->SetTextA( sTemp.c_str() );
		}

		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_MISC_MANTISSA_EDIT );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToHex( _fVal.ManBits(), sTemp, (ee::CFloatX::RealMantissaBits( _csSettings.ui8ManBits, _csSettings.bImpBit ) + 3) / 4 );
			pwWidget->SetTextA( sTemp.c_str() );
		}

		pwWidget = FindChild( CFloatingPointStudioLayout::MX_CI_MISC_BIAS_EDIT );
		if ( pwWidget ) {
			std::string sTemp;
			CUtilities::ToSigned( static_cast<int64_t>(_fVal.ExpBits()) - _fVal.ExpBias(), sTemp );
			pwWidget->SetTextA( sTemp.c_str() );
		}
	}

	// WM_CTLCOLORSTATIC.
	CWidget::LSW_HANDLED CFloatingPointStudioWindow::CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush ) {
		WORD wList[] = {
			CFloatingPointStudioLayout::MX_CI_OUT_PINF_EDIT_HEX,
			CFloatingPointStudioLayout::MX_CI_OUT_PINF_EDIT_BIN,
			CFloatingPointStudioLayout::MX_CI_OUT_NINF_EDIT_HEX,
			CFloatingPointStudioLayout::MX_CI_OUT_NINF_EDIT_BIN,
			CFloatingPointStudioLayout::MX_CI_OUT_NAN_EDIT_HEX,
			CFloatingPointStudioLayout::MX_CI_OUT_NAN_EDIT_BIN,
			CFloatingPointStudioLayout::MX_CI_OUT_MAX_EDIT_HEX,
			CFloatingPointStudioLayout::MX_CI_OUT_MAX_EDIT_FLT,
			CFloatingPointStudioLayout::MX_CI_OUT_MIN_EDIT_HEX,
			CFloatingPointStudioLayout::MX_CI_OUT_MIN_EDIT_FLT,
			CFloatingPointStudioLayout::MX_CI_OUT_NMIN_EDIT_HEX,
			CFloatingPointStudioLayout::MX_CI_OUT_NMIN_EDIT_FLT,
			CFloatingPointStudioLayout::MX_CI_OUT_EPS_EDIT_HEX,
			CFloatingPointStudioLayout::MX_CI_OUT_EPS_EDIT_FLT,
			CFloatingPointStudioLayout::MX_CI_OUT_BIAS_EDIT,
			CFloatingPointStudioLayout::MX_CI_OUT_PREC_EDIT,
			CFloatingPointStudioLayout::MX_CI_MISC_NEXT_ABOVE_EDIT,
			CFloatingPointStudioLayout::MX_CI_MISC_ACTUAL_EDIT,
			CFloatingPointStudioLayout::MX_CI_MISC_NEXT_BELOW_EDIT,
			CFloatingPointStudioLayout::MX_CI_MISC_TOTAL_BITS_EDIT,
			CFloatingPointStudioLayout::MX_CI_MISC_EXPONENT_EDIT,
			CFloatingPointStudioLayout::MX_CI_MISC_MANTISSA_EDIT,
			CFloatingPointStudioLayout::MX_CI_MISC_BIAS_EDIT,
		};
		if ( _pwControl ) {
			for ( size_t I = 0; I < MX_ELEMENTS( wList ); ++I ) {
				if ( wList[I] == _pwControl->Id() ) {
					_hBrush = ::GetSysColorBrush( COLOR_WINDOW );
					return LSW_H_HANDLED;
				}
			}
		}
		return LSW_H_CONTINUE;
	}

	// Updates the preset list selection or text.
	void CFloatingPointStudioWindow::UpdatePreset( const MX_CUR_SETTINGS &_csSettings ) {
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CFloatingPointStudioLayout::MX_CI_PARM_PRESET_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < MX_ELEMENTS( m_pPresets ); ++I ) {
				if ( m_pPresets[I].bSign == _csSettings.bSign &&
					m_pPresets[I].bImpBit == _csSettings.bImpBit &&
					m_pPresets[I].ui8ExpBits == _csSettings.ui8ExpBits &&
					m_pPresets[I].ui8ManBits == _csSettings.ui8ManBits ) {
					pcbCombo->SetCurSel( I );
					return;
				}
			}
			pcbCombo->SetCurSelByItemData( MX_P_CUSTOM );
		}
	}

	// Sets a floating-point string on a widget, applying the selected format.
	void CFloatingPointStudioWindow::SetFloatText( CWidget * _pwWidget, double _dValue, const MX_CUR_SETTINGS &_csSettings ) {
		if ( !_pwWidget ) { return; }

		switch ( _csSettings.iOutFormat ) {
			case MX_SCIENTIFIC : {
				auto aPrec = std::ceil( ee::CFloatX::Precision( _csSettings.ui8ManBits ) ) + 1.0;
				aPrec = std::max( aPrec, 4.0 );
				std::string sTemp;
				CUtilities::ToDouble( _dValue, sTemp, static_cast<int32_t>(-aPrec) );
				_pwWidget->SetTextA( sTemp.c_str() );
				break;
			}
			case MX_SPRINTF : {
				try {
					std::wstring wsTmp;
					size_t sLen = 1200;
					wsTmp.resize( sLen );
					auto iLen = std::swprintf( wsTmp.data(), _csSettings.wsFormatString.c_str(), wsTmp.size(), _dValue, _dValue, _dValue, _dValue );
					while ( iLen >= sLen - 3 ) {
						sLen = iLen + 50;
						wsTmp.resize( sLen );
						iLen = std::swprintf( wsTmp.data(), _csSettings.wsFormatString.c_str(), wsTmp.size(), _dValue, _dValue, _dValue, _dValue );
					}
					_pwWidget->SetTextW( wsTmp.c_str() );
				}
				catch ( ... ) { break; }
				break;
			}
			case MX_PYTHON : {
				try {
					auto fFormatArgs = std::make_wformat_args( _dValue, _dValue, _dValue, _dValue );
					std::wstring sTmp = std::vformat( _csSettings.wsFormatString, fFormatArgs );
					_pwWidget->SetTextW( sTmp.c_str() );
				}
				catch ( ... ) { break; }
				break;
			}
			default : {
				std::string sTemp;
				CUtilities::ToDouble( _dValue, sTemp );
				_pwWidget->SetTextA( sTemp.c_str() );
			}
		}
		
	}

}	// namespace mx

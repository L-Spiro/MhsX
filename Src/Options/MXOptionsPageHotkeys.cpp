#include "MXOptionsPageHotkeys.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../MemHack/MXMemHack.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <Button/LSWButton.h>
#include <CheckButton/LSWCheckButton.h>
#include <Edit/LSWEdit.h>
#include <EEExpEvalContainer.h>
#include <RadioButton/LSWRadioButton.h>
#include <Static/LSWStatic.h>

namespace mx {

	// Window property.
	WCHAR COptionsPageHotkeys::m_szProp[2] = { 0 };

	COptionsPageHotkeys::COptionsPageHotkeys( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		COptionsPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<MX_OPTIONS *>(_ui64Data) ),
		m_lsListenState( MX_LS_WAITING ),
		m_bKeyCode( 0 ),
		m_bKeyModifier( 0 ),
		m_wpOrigProc( nullptr ),
		m_dwScanCode( 0 ),
		m_bAddAfterSelectingButtons( false ) {

		if ( !m_szProp[0] ) {
			m_szProp[0] = L'H' + ((reinterpret_cast<UINT_PTR>(_pwParent) >> 2) & 0x0F);
		}

		m_bBitmap.LoadFromFile( (CSystem::GetResourcesPathW() + std::to_wstring( 58 ) + _DEC_WS_AAAC7C89__bmp).c_str(), 0, 0, LR_CREATEDIBSECTION );
	}

	// == Functions.
	// Get the name of the page.
	std::wstring COptionsPageHotkeys::GetName() const {
		return _DEC_WS_8AB28410_Hotkeys;
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COptionsPageHotkeys::InitDialog() {
		if ( !m_poOptions ) { return LSW_H_CONTINUE; }

		if ( m_poOptions->pmhMemHackObj ) {
			m_hmManager.Copy( m_poOptions->pmhMemHackObj->HotkeyManager() );
		}

		CListBox * plbList = nullptr;
		CCheckButton * pcbCheck = nullptr;
		CEdit * peEdit = nullptr;
		CRadioButton * prbRadio = nullptr;
		CComboBox * pcbCombo = nullptr;


		plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_LIST ));
		if ( plbList ) {
			std::set<LPARAM> sSelected;
			m_hmManager.FillListBoxWithHotkeys( plbList, sSelected );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_KEY_COMBO ));
		if ( pcbCombo ) {
			CHotkeyManager::FillComboWithCodes( pcbCombo );
			pcbCombo->AutoSetMinListWidth();
			pcbCombo->SetCurSelByItemData( VK_RETURN );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_MOD_COMBO ));
		if ( pcbCombo ) {
			CHotkeyManager::FillComboWithMods( pcbCombo );
			pcbCombo->AutoSetMinListWidth();
			pcbCombo->SetCurSelByItemData( 0 );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_METHOD_COMBO ));
		if ( pcbCombo ) {
			CHotkeyManager::FillComboWithMethods( pcbCombo );
			pcbCombo->AutoSetMinListWidth();
			pcbCombo->SetCurSelByItemData( CHotkeyManager::MX_M_HOTKEY );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_FUNCTION_COMBO ));
		if ( pcbCombo ) {
			CHotkeyManager::FillComboWithFunctions( pcbCombo );
			pcbCombo->AutoSetMinListWidth();
			pcbCombo->SetCurSelByItemData( 0 );
		}



#define MX_CHECK( ID, MEMBER )																					\
	pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::ID ));									\
	if ( pcbCheck ) {																							\
		pcbCheck->SetCheck( m_poOptions->MEMBER );																\
	}
		/*MX_CHECK( MX_OI_GENERAL_SEARCH_MEM_IMAGE, bMemImage );
		MX_CHECK( MX_OI_GENERAL_SEARCH_MEM_PRIVATE, bMemPrivate );
		MX_CHECK( MX_OI_GENERAL_SEARCH_MEM_MAPPED, bMemMapped );

		MX_CHECK( MX_OI_GENERAL_SEARCH_POSTFLUSH, bPostFlush );
		MX_CHECK( MX_OI_GENERAL_SEARCH_PRECACHE, bPreCache );
		MX_CHECK( MX_OI_GENERAL_SEARCH_PAUSE, bPauseTarget );

		MX_CHECK( MX_OI_GENERAL_SEARCH_USE_EPSILON, bUseEpsilon );
		MX_CHECK( MX_OI_GENERAL_SEARCH_SMART_EPS, bSmartEpsilon );*/
#undef MX_CHECK

#define MX_CHECK( ID, MEMBER )																					\
	prbRadio = static_cast<CRadioButton *>(FindChild( COptionsLayout::ID ));									\
	if ( prbRadio ) {																							\
		prbRadio->SetCheck( m_poOptions->MEMBER );																\
	}
		/*MX_CHECK( MX_OI_GENERAL_SEARCH_TP_NORMAL, iThreadPriority == THREAD_PRIORITY_NORMAL );
		MX_CHECK( MX_OI_GENERAL_SEARCH_TP_HIGH, iThreadPriority == THREAD_PRIORITY_HIGHEST );
		MX_CHECK( MX_OI_GENERAL_SEARCH_TP_CRIT, iThreadPriority == THREAD_PRIORITY_TIME_CRITICAL );*/
#undef MX_CHECK

		/*peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_BUFFER_SIZE_EDIT ));
		if ( peEdit ) {
			peEdit->SetTextA( CUtilities::ToUnsigned( m_poOptions->ui64BufferSize / 1024 / 1024 ) );
		}

		peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_EPSILON_EDIT ));
		if ( peEdit ) {
			std::string sTemp;
			peEdit->SetTextA( CUtilities::ToDouble( m_poOptions->dEpsilon, sTemp ) );
		}*/

		CStatic * psStatic = static_cast<CStatic *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_WARNING_LABEL ));
		if ( psStatic ) {
			psStatic->SetImage( IMAGE_BITMAP, m_bBitmap );
		}

		ApplySettings();
		
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED COptionsPageHotkeys::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		if ( !m_poOptions ) { return LSW_H_CONTINUE; }
		//ApplySettings();
		switch ( _Id ) {
			case COptionsLayout::MX_OI_HOTKEYS_OPTIONS_KEY_BUTTON : {
				BeginListening();
				break;
			}
			case COptionsLayout::MX_OI_HOTKEYS_ADD : {
				if ( !AddButton() ) {
				}
				break;
			}
			case COptionsLayout::MX_OI_HOTKEYS_DELETE : {
				if ( !DeleteButton() ) {
				}
				break;
			}
			case COptionsLayout::MX_OI_HOTKEYS_UP : {
				if ( !UpButton() ) {
				}
				break;
			}
			case COptionsLayout::MX_OI_HOTKEYS_DOWN : {
				if ( !DownButton() ) {
				}
				break;
			}
			case COptionsLayout::MX_OI_HOTKEYS_OPTIONS_METHOD_COMBO : {}
			case COptionsLayout::MX_OI_HOTKEYS_OPTIONS_FUNCTION_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					ApplySettings();
				}
				break;
			}
			case COptionsLayout::MX_OI_HOTKEYS_LIST : {
				switch ( _wCtrlCode ) {
					case LBN_SELCHANGE : {
						SHORT sKey = CSystem::GetAsyncKeyState( VK_CONTROL );
						if ( sKey & 0x8000 ) {
							FillInfoBySelection();
						}
						break;
					}
				}
			}
		}
		return LSW_H_CONTINUE;
	}

	// Verifies the options, returning an error string in case of error.
	BOOL COptionsPageHotkeys::Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
		ee::CExpEvalContainer::EE_RESULT eRes;
		CWidget * pwTemp = nullptr;
		/*pwTemp = FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_BUFFER_SIZE_EDIT );
		if ( pwTemp ) {
			if ( !pwTemp->GetTextAsUInt64Expression( eRes ) ) {
				_pwWidget = pwTemp;
				_wsError = _DEC_WS_C6AC1231_Invalid_search_buffer_size_;
				return FALSE;
			}
		}

		pwTemp = FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_EPSILON_EDIT );
		if ( pwTemp ) {
			if ( !pwTemp->GetTextAsDoubleExpression( eRes ) ) {
				_pwWidget = pwTemp;
				_wsError = _DEC_WS_552321B5_Invalid_epsilon_;
				return FALSE;
			}
		}*/
		return TRUE;
	}

	// WM_KEYDOWN
	CWidget::LSW_HANDLED COptionsPageHotkeys::KeyDown( UINT _uiKeyCode, UINT _lParam ) {
		return COptionsPage::KeyDown( _uiKeyCode, _lParam );
	}

	// Are we ignoring left and right modifier keys?
	bool COptionsPageHotkeys::IgnoringLeftRight() const {
		const CComboBox * pcbMethodCombo = static_cast<const CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_METHOD_COMBO ));
		return pcbMethodCombo && pcbMethodCombo->GetCurSelItemData() == CHotkeyManager::MX_M_HOTKEY;
	}

	// Translate a child's tooltip text.
	std::wstring COptionsPageHotkeys::TranslateTooltip( const std::string &_sText ) { 
		return CStringDecoder::DecodeToWString( _sText.c_str(), _sText.size() );
	}

	// Copies all the settings to the MX_OPTIONS structure.
	BOOL COptionsPageHotkeys::Finalize() {
		if ( !m_poOptions ) { return TRUE; }
		CCheckButton * pcbCheck = nullptr;
		CRadioButton * prbRadio = nullptr;
		CEdit * peEdit = nullptr;


		if ( m_poOptions->pmhMemHackObj ) {
			m_poOptions->pmhMemHackObj->HotkeyManager().Copy( m_hmManager );
		}

		// ==== SEARCH BOOLEANS ==== //
#if 0
#define MX_GETCHECK( ID, MEMBER )																				\
	pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::ID ));									\
	if ( pcbCheck ) {																							\
		m_poOptions->MEMBER = pcbCheck->IsChecked();															\
	}

		/*COptionsLayout::MX_OI_GENERAL_SEARCH_MEM_IMAGE;
		m_poOptions->bMemImage;*/
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_MEM_IMAGE, bMemImage );
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_MEM_PRIVATE, bMemPrivate );
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_MEM_MAPPED, bMemMapped );

		MX_GETCHECK( MX_OI_GENERAL_SEARCH_POSTFLUSH, bPostFlush );
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_PRECACHE, bPreCache );
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_PAUSE, bPauseTarget );

		MX_GETCHECK( MX_OI_GENERAL_SEARCH_USE_EPSILON, bUseEpsilon );
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_SMART_EPS, bSmartEpsilon );
#undef MX_GETCHECK

		// ==== THREAD PRIORITY ==== //
		static const struct {
			COptionsLayout::MX_OPTIONS_IDS	iId;
			INT								iPri;
		} aTemp [] = {
			{ COptionsLayout::MX_OI_GENERAL_SEARCH_TP_NORMAL, THREAD_PRIORITY_NORMAL },
			{ COptionsLayout::MX_OI_GENERAL_SEARCH_TP_HIGH, THREAD_PRIORITY_HIGHEST },
			{ COptionsLayout::MX_OI_GENERAL_SEARCH_TP_CRIT, THREAD_PRIORITY_TIME_CRITICAL },
		};
		for ( size_t I = 0; I < MX_ELEMENTS( aTemp ); ++I ) {
			prbRadio = static_cast<CRadioButton *>(FindChild( aTemp[I].iId ));
			if ( prbRadio ) {
				if ( prbRadio->IsChecked() ) {
					m_poOptions->iThreadPriority = aTemp[I].iPri;
					break;
				}
			}
		}

		// ==== EDIT BOXES ==== //
		ee::CExpEvalContainer::EE_RESULT eRes;
		peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_BUFFER_SIZE_EDIT ));
		if ( peEdit ) {
			if ( peEdit->GetTextAsUInt64Expression( eRes ) ) {
				m_poOptions->ui64BufferSize = eRes.u.ui64Val * 1024ULL * 1024ULL;
			}
		}

		peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_EPSILON_EDIT ));
		if ( peEdit ) {
			if ( peEdit->GetTextAsDoubleExpression( eRes ) ) {
				m_poOptions->dEpsilon = eRes.u.dVal;
			}
		}
#endif

		return TRUE;
	}

	// Apply dialog settings.
	VOID COptionsPageHotkeys::ApplySettings() {
		CComboBox * pcbFuncCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_FUNCTION_COMBO ));
		if ( !pcbFuncCombo ) { return; }
		INT iFunc = static_cast<INT>(pcbFuncCombo->GetCurSelItemData());
		const CHotkeyManBase::MX_HOTKEY_HANDLER * phhHandler = CHotkeyManBase::HandlerByFunctionId( static_cast<CHotkeyManBase::MX_FUNCTION>(iFunc) );
		if ( !phhHandler ) { return; }

		CEdit * peParms[3] = {
			static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM1_EDIT )),
			static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM2_EDIT )),
			static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM3_EDIT )),
		};
		CStatic * psLabels[3] = {
			static_cast<CStatic *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM1_DESC_LABEL )),
			static_cast<CStatic *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM2_DESC_LABEL )),
			static_cast<CStatic *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM3_DESC_LABEL )),
		};

		for ( size_t I = 0; I < MX_ELEMENTS( peParms ); ++I ) {
			if ( peParms[I] ) {
				peParms[I]->SetEnabled( I < phhHandler->sParms );
			}
			if ( psLabels[I] ) {
				CSecureString ssTmp;
				const char * pcTxt = (I < phhHandler->sParms) ?
					(phhHandler->hdParms[I].sLen == 0 ? phhHandler->hdParms[I].pcDesc :
						(ssTmp = mx::CStringDecoder::DecodeToString( phhHandler->hdParms[I].pcDesc, phhHandler->hdParms[I].sLen )).c_str()) :
						(ssTmp = _DEC_S_12A629FB_Parameter_Not_Used).c_str();
				psLabels[I]->SetTextW( CUtilities::StringToWString( pcTxt ).c_str() );
			}
		}

		CStatic * psStatic = static_cast<CStatic *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_WARNING_LABEL ));
		BOOL bShow = FALSE;
		if ( m_lsListenState == MX_LS_SUCCESS ) {
			CButton * pbButton = static_cast<CButton *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_KEY_BUTTON ));
			if ( pbButton ) {
				bool bIgnoreLeftRight = IgnoringLeftRight();

				CSecureWString wTmp;
				if ( m_bKeyModifier ) {
					CHotkeyManager::ModifierToString( m_bKeyModifier, wTmp, bIgnoreLeftRight );
					wTmp += L"+";
				}
				wTmp += CHotkeyManager::ScanCodeToString( m_dwScanCode );
				
				pbButton->SetTextW( wTmp.c_str() );

				CComboBox * pcbMethodCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_METHOD_COMBO ));
				
				if ( pcbMethodCombo ) {
					INT iMethod = static_cast<INT>(pcbMethodCombo->GetCurSelItemData());
					if ( iMethod == CHotkeyManager::MX_M_HOTKEY ) {
						INT iKey = m_bKeyCode;
						INT iMod = m_bKeyModifier;
						bShow = m_hmManager.GetRegisteredHotkeyManager()->HasKey( iKey, iMod );
					}
				}
			}
		}
		if ( psStatic ) { psStatic->SetVisible( bShow ); }
	}

	// Handles the Add button.
	bool COptionsPageHotkeys::AddButton() {
		/*CComboBox * pcbKeyCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_KEY_COMBO ));
		CComboBox * pcbModCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_MOD_COMBO ));*/
		CComboBox * pcbFuncCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_FUNCTION_COMBO ));
		CComboBox * pcbMethodCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_METHOD_COMBO ));
		if ( /*!pcbKeyCombo || !pcbModCombo || */!pcbFuncCombo || !pcbMethodCombo ) {
			CSystem::MessageBoxError( Wnd(), _T_LEN_A756CF10_Internal_Error );
			return false;
		}

		/*INT iKey = static_cast<INT>(pcbKeyCombo->GetCurSelItemData());
		INT iMod = static_cast<INT>(pcbModCombo->GetCurSelItemData());*/
		INT iKey = m_bKeyCode;
		INT iMod = m_bKeyModifier;
		INT iFunc = static_cast<INT>(pcbFuncCombo->GetCurSelItemData());
		INT iMethod = static_cast<INT>(pcbMethodCombo->GetCurSelItemData());
		const CHotkeyManBase::MX_HOTKEY_HANDLER * phhHandler = CHotkeyManBase::HandlerByFunctionId( static_cast<CHotkeyManBase::MX_FUNCTION>(iFunc) );
		if ( !phhHandler ) {
			CSystem::MessageBoxError( Wnd(), _T_LEN_E7B32FBC_Internal_Error__Unrecognized_function_ );
			return false;
		}
		CEdit * peParms[3] = {
			static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM1_EDIT )),
			static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM2_EDIT )),
			static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM3_EDIT )),
		};
		ee::CExpEvalContainer::EE_RESULT rRes[3] = {};
		/*for ( auto I = MX_ELEMENTS( peParms ); I--; ) {
			if ( !peParms[I] ) {
				CSystem::MessageBoxError( Wnd(), _T_LEN_A756CF10_Internal_Error );
				return false;
			}
		}*/
		for ( auto I = phhHandler->sParms; I--; ) {
			if ( !peParms[I] ) {
				CSystem::MessageBoxError( Wnd(), _T_LEN_A756CF10_Internal_Error );
				return false;
			}
			if ( !peParms[I]->GetTextLengthA() ) {
				CSystem::MessageBoxError( Wnd(), _T_LEN_20573FE6_No_expression_entered_ );
				peParms[I]->SetFocus();
				return false;
			}
			if ( !peParms[I]->GetTextAsInt64Expression( rRes[I] ) ) {
				/*char szBuffer[64];
				int iLen = ::_snprintf( szBuffer, MX_ELEMENTS( szBuffer ), "%s (%u)", _DEC_S_02BBA31E_Invalid_expression_.c_str(), static_cast<uint32_t>(I) );
				CSystem::MessageBoxError( Wnd(), szBuffer );*/
				CSystem::MessageBoxError( Wnd(), _T_LEN_02BBA31E_Invalid_expression_ );
				peParms[I]->SetFocus();
				return false;
			}
		}


		if ( m_lsListenState != MX_LS_SUCCESS ) {
			m_bAddAfterSelectingButtons = true;
			return BeginListening();
		}

		switch ( iMethod ) {
			case CHotkeyManager::MX_M_HOOK : {}
			case CHotkeyManager::MX_M_POLLING : {}
			case CHotkeyManager::MX_M_HOTKEY : {
				CSecureString ssError;
				if ( !m_hmManager.AddHotkey( 0, iKey, iMod, m_dwScanCode, iMethod, iFunc, rRes[0].u.ui64Val, rRes[1].u.ui64Val, rRes[2].u.ui64Val, ssError ) ) {
					lsw::CBase::MessageBoxError( Wnd(), (_DEC_WS_9C1C9375_Error + L": " + CUtilities::StringToWString( ssError )).c_str(), _DEC_WS_9C1C9375_Error.c_str() );
					return false;
				}
				break;
			}
		}

		CListBox * plbList = nullptr;
		plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_LIST ));
		if ( plbList ) {
			std::set<LPARAM> sSelected;
			GatherSelected( sSelected );
			plbList->ResetContent();
			m_hmManager.FillListBoxWithHotkeys( plbList, sSelected );
		}
		ApplySettings();
		return true;
	}

	// Handles the Delete button.
	bool COptionsPageHotkeys::DeleteButton() {
		bool bPass = true;
		std::set<LPARAM> sSelected;
		GatherSelected( sSelected );
		if ( !sSelected.size() ) { return true; }
		for ( auto I = sSelected.begin(); I != sSelected.end(); ++I ) {
			if ( !m_hmManager.DeleteByLParam( (*I) ) ) {
				bPass = false;
			}
		}

		CListBox * plbList = nullptr;
		plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_LIST ));
		if ( plbList ) {
			std::set<LPARAM> sSelected;
			plbList->ResetContent();
			m_hmManager.FillListBoxWithHotkeys( plbList, sSelected );
		}
		ApplySettings();
		return bPass;
	}

	// Handles the Up button.
	bool COptionsPageHotkeys::UpButton() {
		CListBox * plbList = nullptr;
		plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_LIST ));
		if ( plbList ) {
			std::vector<INT> vSelections;
			if ( plbList->GetSelItems( vSelections ) ) {
				if ( vSelections.size() > 0 ) {
					// Store the selections for reconstruction the list box later.
					std::set<LPARAM> sSels;
					std::vector<LPARAM> vSels;
					for ( size_t I = 0; I < vSelections.size(); ++I ) {
						LPARAM lpItem = plbList->GetItemData( vSelections[I] );
						sSels.insert( lpItem );
						vSels.push_back( lpItem );
					}
					// And pass the original selections to the manager.
					m_hmManager.MoveUp( vSels );

					plbList->ResetContent();
					m_hmManager.FillListBoxWithHotkeys( plbList, sSels );
				}
			}
		}
		return true;
	}

	// Handles the Down button.
	bool COptionsPageHotkeys::DownButton() {
		CListBox * plbList = nullptr;
		plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_LIST ));
		if ( plbList ) {
			std::vector<INT> vSelections;
			if ( plbList->GetSelItems( vSelections ) ) {
				if ( vSelections.size() > 0 ) {
					// Store the selections for reconstruction the list box later.
					std::set<LPARAM> sSels;
					std::vector<LPARAM> vSels;
					for ( size_t I = 0; I < vSelections.size(); ++I ) {
						LPARAM lpItem = plbList->GetItemData( vSelections[I] );
						sSels.insert( lpItem );
						vSels.push_back( lpItem );
					}
					// And pass the original selections to the manager.
					m_hmManager.MoveDown( vSels );

					plbList->ResetContent();
					m_hmManager.FillListBoxWithHotkeys( plbList, sSels );
				}
			}
		}
		return true;
	}

	// Starts listening for input.
	bool COptionsPageHotkeys::BeginListening() {
		m_bKeyCode = m_bKeyModifier = 0;
		m_dwScanCode = 0;

		
		CButton * pbButton = static_cast<CButton *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_KEY_BUTTON ));
		if ( !pbButton ) { return false; }
		if ( m_lsListenState != MX_LS_LISTENING ) {
			::SetPropW( pbButton->Wnd(), m_szProp, reinterpret_cast<HANDLE>(this) );
			m_wpOrigProc = CHelpers::SetWndProc( pbButton->Wnd(), ButtonOverride );
		}
		m_lsListenState = MX_LS_LISTENING;
		pbButton->SetTextW( _DEC_WS_97CE63DA_Listening___.c_str() );
		pbButton->SetFocus();
		return true;
	}

	// Stops listening for input.
	bool COptionsPageHotkeys::StopListening( bool _bSuccess ) {
		if ( m_lsListenState != MX_LS_LISTENING ) { return false; }
		m_lsListenState = _bSuccess ? MX_LS_SUCCESS : MX_LS_FAILURE;
		if ( !_bSuccess ) {
			m_bKeyCode = m_bKeyModifier = 0;
			m_dwScanCode = 0;
		}
		
		if ( _bSuccess && m_bAddAfterSelectingButtons ) {
			AddButton();
		}
		else { ApplySettings(); }
		m_bAddAfterSelectingButtons = false;

		CButton * pbButton = static_cast<CButton *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_KEY_BUTTON ));
		if ( !pbButton ) { return false; }
		CHelpers::SetWndProc( pbButton->Wnd(), m_wpOrigProc );
		if ( !_bSuccess ) {
			pbButton->SetTextW( _DEC_WS_971A9E73_Press_to_set_hotkey_.c_str() );
		}

		return true;
	}

	// Fills a set of LPARAM's, one for each selected item in the list box.
	void COptionsPageHotkeys::GatherSelected( std::set<LPARAM> &_sResult ) {
		CListBox * plbList = nullptr;
		plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_LIST ));
		if ( plbList ) {
			std::vector<INT> vSelections;
			if ( plbList->GetSelItems( vSelections ) ) {
				for ( size_t I = 0; I < vSelections.size(); ++I ) {
					_sResult.insert( plbList->GetItemData( vSelections[I] ) );
				}
			}
		}
	}

	// Sets the current information based on the current selection.
	void COptionsPageHotkeys::FillInfoBySelection() {
		CListBox * plbList = nullptr;
		plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_LIST ));
		if ( plbList ) {
			if ( plbList->GetSelCount() ) {
				INT iSelIdx = plbList->GetCurSel();
				if ( iSelIdx != LB_ERR ) {
					LPARAM lpParm = plbList->GetCurSelItemData();
					m_hmManager.Lock();
					const CHotkeyManBase::MX_HOTKEY_INSTANCE * phiInst = m_hmManager.GetById( lpParm & MX_H_KEY_LPARAM_ID_MASK );
					if ( phiInst ) {
						CComboBox * pcbFuncCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_FUNCTION_COMBO ));
						if ( pcbFuncCombo ) {
							pcbFuncCombo->SetCurSelByItemData( phiInst->phhHandler->uiId );
						}

						CEdit * peParms[3] = {
							static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM1_EDIT )),
							static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM2_EDIT )),
							static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_HOTKEYS_OPTIONS_PARM3_EDIT )),
						};
						for ( size_t I = 0; I < phiInst->phhHandler->sParms; ++I ) {
							if ( peParms[I] ) {
								peParms[I]->SetTextW( std::to_wstring( phiInst->uiParms[I] ).c_str() );
							}
						}
						ApplySettings();
					}
					m_hmManager.Unlock();



					/*
					switch ( lpParm >> MX_H_KEY_LPARAM_METHOD_SHIFT ) {
						case CHotkeyManager::MX_M_POLLING : {
							break;
						}
						case CHotkeyManager::MX_M_HOOK : {
							break;
						}
						case CHotkeyManager::MX_M_HOTKEY : {
							break;
						}
					}
					*/
				}
			}
		}
	}

	// Edit window procedure.
	LRESULT CALLBACK COptionsPageHotkeys::ButtonOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		COptionsPageHotkeys * pophThis = reinterpret_cast<COptionsPageHotkeys *>(::GetPropW( _hWnd, m_szProp ));
		WNDPROC wpOrig = pophThis->m_wpOrigProc;
		switch ( _uMsg ) {
			case WM_GETDLGCODE : { return DLGC_WANTALLKEYS; }
			case WM_KILLFOCUS : {
				pophThis->StopListening( false );
				break;
			}
			case WM_KEYDOWN : {
				if ( static_cast<BYTE>(_wParam) == VK_LWIN || static_cast<BYTE>(_wParam) == VK_RWIN ) { return 0; }

				bool bIgnoreLeftRight = pophThis->IgnoringLeftRight();
				bool bExtended = (_lParam & (KF_EXTENDED << 16));
				if ( static_cast<BYTE>(_wParam) == VK_SHIFT ) {
					CSecureWString wTmp;
					pophThis->m_bKeyModifier = ::MapVirtualKeyW( (_lParam >> 16) & 0xFF, MAPVK_VSC_TO_VK_EX );
					CHotkeyManager::ModifierToString( pophThis->m_bKeyModifier, wTmp, bIgnoreLeftRight );
					wTmp += L"…";
					::SetWindowTextW( _hWnd, wTmp.c_str() );
					return 0;
				}
				if ( static_cast<BYTE>(_wParam) == VK_CONTROL ) {
					UINT uiTemp = ::MapVirtualKeyW( VK_CONTROL, MAPVK_VK_TO_VSC );
					CSecureWString wTmp = CHotkeyManager::ScanCodeToString( bIgnoreLeftRight ? (uiTemp << 16) : _lParam );
					pophThis->m_bKeyModifier = bExtended ? VK_RCONTROL : VK_LCONTROL;
					wTmp += L"…";
					::SetWindowTextW( _hWnd, wTmp.c_str() );
					return 0;
				}
				if ( static_cast<BYTE>(_wParam) == VK_MENU ) {
					UINT uiTemp = ::MapVirtualKeyW( VK_MENU, MAPVK_VK_TO_VSC );
					CSecureWString wTmp = CHotkeyManager::ScanCodeToString( bIgnoreLeftRight ? (uiTemp << 16) : _lParam );
					pophThis->m_bKeyModifier = bExtended ? VK_RMENU : VK_LMENU;
					wTmp += L"…";
					::SetWindowTextW( _hWnd, wTmp.c_str() );
					return 0;
				}
				CSecureWString wTmp;
				if ( pophThis->m_bKeyModifier ) {
					CHotkeyManager::ModifierToString( pophThis->m_bKeyModifier, wTmp, bIgnoreLeftRight );
					wTmp += L"+";
				}
				wTmp += CHotkeyManager::ScanCodeToString( _lParam );
				
				::SetWindowTextW( _hWnd, wTmp.c_str() );
				pophThis->m_bKeyCode = static_cast<BYTE>(_wParam);
				pophThis->m_dwScanCode = static_cast<DWORD>(_lParam);

				pophThis->StopListening( true );
				return 0;
			}
			case WM_SYSKEYDOWN : {
				if ( static_cast<BYTE>(_wParam) == VK_LWIN || static_cast<BYTE>(_wParam) == VK_RWIN || static_cast<BYTE>(_wParam) == VK_SHIFT || static_cast<BYTE>(_wParam) == VK_CONTROL ) { return 0; }

				bool bIgnoreLeftRight = pophThis->IgnoringLeftRight();
				bool bExtended = (_lParam & (KF_EXTENDED << 16));

				CSecureWString wTmp;
				if ( static_cast<BYTE>(_wParam) == VK_MENU ) {
					pophThis->m_bKeyModifier = bExtended ? VK_RMENU : VK_LMENU;
					CHotkeyManager::ModifierToString( pophThis->m_bKeyModifier, wTmp, bIgnoreLeftRight );
					wTmp += L"…";
					::SetWindowTextW( _hWnd, wTmp.c_str() );
					return 0;
				}
				
				if ( pophThis->m_bKeyModifier ) {
					CHotkeyManager::ModifierToString( pophThis->m_bKeyModifier, wTmp, bIgnoreLeftRight );
					wTmp += L"+";
				}
				wTmp += CHotkeyManager::ScanCodeToString( _lParam );
				
				::SetWindowTextW( _hWnd, wTmp.c_str() );
				pophThis->m_bKeyCode = static_cast<BYTE>(_wParam);
				pophThis->m_dwScanCode = static_cast<DWORD>(_lParam);

				pophThis->StopListening( true );
				return 0;
			}
		}
		return ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
	}

}	// namespace mx

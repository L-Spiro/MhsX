// Included first to avoid an error with std::byte.
#include "../MXMhsX.h"
#include <ShlObj_core.h>

#include "MXOptionsPagePrograms.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../MemHack/MXMemHack.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <Button/LSWButton.h>
#include <CheckButton/LSWCheckButton.h>
#include <commdlg.h>
#include <Edit/LSWEdit.h>
#include <EEExpEvalContainer.h>
#include <RadioButton/LSWRadioButton.h>

#include <Static/LSWStatic.h>

namespace mx {

	// Window property.
	WCHAR COptionsPagePrograms::m_szProp[2] = { 0 };

	COptionsPagePrograms::COptionsPagePrograms( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		COptionsPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<MX_OPTIONS *>(_ui64Data) ) {
		if ( m_poOptions ) {
			if ( m_poOptions->pmhMemHackObj ) {
				m_poOptions->pmhMemHackObj->LockPrograms();
				m_vPrograms = m_poOptions->pmhMemHackObj->Programs();
				m_poOptions->pmhMemHackObj->UnlockPrograms();
			}
		}

		if ( !m_szProp[0] ) {
			m_szProp[0] = L'H' + ((reinterpret_cast<UINT_PTR>(_pwParent) >> 2) & 0x0F);
		}
	}

	// == Functions.
	// Get the name of the page.
	std::wstring COptionsPagePrograms::GetName() const {
		return _DEC_WS_083F0713_Programs;
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COptionsPagePrograms::InitDialog() {
		if ( !m_poOptions ) { return LSW_H_CONTINUE; }

		
		CListBox * plbList = nullptr;
		CCheckButton * pcbCheck = nullptr;
		CEdit * peEdit = nullptr;
		CRadioButton * prbRadio = nullptr;
		CComboBox * pcbCombo = nullptr;

		peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_ARGUMENTS_EDIT ));
		if ( peEdit ) {
			peEdit->LimitText( 0x7FFFFFFFE );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_SHOW_CMD_COMBO ));
		if ( pcbCombo ) {
			int iTable[] = {
				SW_HIDE,
				SW_NORMAL,
				SW_SHOWMINIMIZED,
				SW_SHOWMAXIMIZED,
				SW_SHOWNOACTIVATE,
				SW_SHOW,
				SW_MINIMIZE,
				SW_SHOWMINNOACTIVE,
				SW_SHOWNA,
				SW_RESTORE,
				SW_SHOWDEFAULT,
				SW_FORCEMINIMIZE
			};
			std::string sName;
			for ( size_t I = 0; I < MX_ELEMENTS( iTable ); ++I ) {
				sName.clear();
				INT iIdx = pcbCombo->AddString( CUtilities::ShowWindowToString( iTable[I], sName ) );
				if ( iIdx != -1 ) {
					pcbCombo->SetItemData( iIdx, iTable[I] );
				}
			}
		}

		ApplySettings();
		
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED COptionsPagePrograms::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		if ( !m_poOptions ) { return LSW_H_CONTINUE; }
		//ApplySettings();
		switch ( _Id ) {
			case COptionsLayout::MX_OI_PROGRAMS_ADD : {
				if ( !AddButton() ) {
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_DELETE : {
				if ( !DeleteButton() ) {
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_UP : {
				if ( !UpButton() ) {
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_DOWN : {
				if ( !DownButton() ) {
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_OPTIONS_PROGRAM_BUTTON : {
				if ( BrowseProgram() ) {
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_OPTIONS_WORKINGDIR_BUTTON : {
				if ( BrowseWorkingDirectory() ) {
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_LIST : {
				switch ( _wCtrlCode ) {
					case LBN_SELCHANGE : {
						FillInfoBySelection();
						break;
					}
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_OPTIONS_HIDDEN_CHECK : {
				if ( HiddenCheck() ) {
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_OPTIONS_NAME_EDIT : {
				switch ( _wCtrlCode ) {
					case EN_CHANGE : {
						CListBox * plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
						if ( plbList ) {
							INT iSel = plbList->GetCurSel();
							LPARAM lpSel = plbList->GetCurSel() != LB_ERR ? plbList->GetCurSelItemData() : -1;
							if ( lpSel != -1 && size_t( lpSel ) < m_vPrograms.size() ) {
								CEdit * peEdit;

								peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_NAME_EDIT ));
								if ( peEdit ) {
									m_vPrograms[lpSel].wsMenuName = peEdit->GetTextW();
									ApplySettings();
								}
							}
						}
						break;
					}
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_OPTIONS_PROGRAM_EDIT : {
				switch ( _wCtrlCode ) {
					case EN_CHANGE : {
						CListBox * plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
						if ( plbList ) {
							INT iSel = plbList->GetCurSel();
							LPARAM lpSel = plbList->GetCurSel() != LB_ERR ? plbList->GetCurSelItemData() : -1;
							if ( lpSel != -1 && size_t( lpSel ) < m_vPrograms.size() ) {
								CEdit * peEdit;

								peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_PROGRAM_EDIT ));
								if ( peEdit ) {
									m_vPrograms[lpSel].wsPath = peEdit->GetTextW();
								}
							}
						}
						break;
					}
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_OPTIONS_ARGUMENTS_EDIT : {
				switch ( _wCtrlCode ) {
					case EN_CHANGE : {
						CListBox * plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
						if ( plbList ) {
							INT iSel = plbList->GetCurSel();
							LPARAM lpSel = plbList->GetCurSel() != LB_ERR ? plbList->GetCurSelItemData() : -1;
							if ( lpSel != -1 && size_t( lpSel ) < m_vPrograms.size() ) {
								CEdit * peEdit;

								peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_ARGUMENTS_EDIT ));
								if ( peEdit ) {
									m_vPrograms[lpSel].wsArgs = peEdit->GetTextW();
								}
							}
						}
						break;
					}
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_OPTIONS_WORKINGDIR_EDIT : {
				switch ( _wCtrlCode ) {
					case EN_CHANGE : {
						CListBox * plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
						if ( plbList ) {
							INT iSel = plbList->GetCurSel();
							LPARAM lpSel = plbList->GetCurSel() != LB_ERR ? plbList->GetCurSelItemData() : -1;
							if ( lpSel != -1 && size_t( lpSel ) < m_vPrograms.size() ) {
								CEdit * peEdit;

								peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_WORKINGDIR_EDIT ));
								if ( peEdit ) {
									m_vPrograms[lpSel].wsWorkingDir = peEdit->GetTextW();
								}
							}
						}
						break;
					}
				}
				break;
			}
			case COptionsLayout::MX_OI_PROGRAMS_OPTIONS_SHOW_CMD_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateShowType();
				}
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// Verifies the options, returning an error string in case of error.
	BOOL COptionsPagePrograms::Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
		/*ee::CExpEvalContainer::EE_RESULT eRes;
		CWidget * pwTemp = nullptr;*/
		CListBox * plbList = nullptr;
		plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
		for ( auto I = m_vPrograms.size(); I--; ) {
			if ( m_vPrograms[I].wsWorkingDir.size() ) {
				if ( !CFile::IsFolder( m_vPrograms[I].wsWorkingDir.c_str() ) ) {
					plbList->SetCurSelByItemData( static_cast<LPARAM>(I) );
					_pwWidget = FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_WORKINGDIR_EDIT );
					_wsError = _DEC_WS_D284E408_The_supplied_working_directory_is_invalid_;
					return FALSE;
				}
			}
			if ( !m_vPrograms[I].wsPath.size() ) {
				plbList->SetCurSelByItemData( static_cast<LPARAM>(I) );
				_pwWidget = FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_PROGRAM_EDIT );
				_wsError = _DEC_WS_50E7AAE8_The_supplied_executable_path_is_invalid_;
				return FALSE;
			}
			if ( !CFile::IsFile( m_vPrograms[I].wsPath.c_str() ) ) {
				bool bFound = false;
				std::vector<std::wstring> vPaths;
				CSystem::ExeSearchPaths( m_vPrograms[I].wsPath.c_str(), vPaths );
				for ( auto J = vPaths.size(); J--; ) {
					if ( CFile::IsFile( vPaths[J].c_str() ) ) {
						bFound = true;
						break;
					}
				}
				if ( !bFound ) {
					plbList->SetCurSelByItemData( static_cast<LPARAM>(I) );
					_pwWidget = FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_PROGRAM_EDIT );
					_wsError = _DEC_WS_50E7AAE8_The_supplied_executable_path_is_invalid_;
					return FALSE;
				}
			}
		}
		return TRUE;
	}

	// WM_KEYDOWN
	CWidget::LSW_HANDLED COptionsPagePrograms::KeyDown( UINT _uiKeyCode, UINT _lParam ) {
		return COptionsPage::KeyDown( _uiKeyCode, _lParam );
	}

	// Translate a child's tooltip text.
	std::wstring COptionsPagePrograms::TranslateTooltip( const std::string &_sText ) { 
		return CStringDecoder::DecodeToWString( _sText.c_str(), _sText.size() );
	}

	// Copies all the settings to the MX_OPTIONS structure.
	BOOL COptionsPagePrograms::Finalize() {
		if ( !m_poOptions ) { return TRUE; }
		if ( m_poOptions ) {
			if ( m_poOptions->pmhMemHackObj ) {
				m_poOptions->pmhMemHackObj->LockPrograms();
				m_poOptions->pmhMemHackObj->Programs() = m_vPrograms;
				m_poOptions->pmhMemHackObj->UnlockPrograms();
			}
		}

		return TRUE;
	}

	// Apply dialog settings.
	VOID COptionsPagePrograms::ApplySettings() {
		CListBox * plbList = nullptr;
		plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
		if ( plbList ) {
			INT iSel = plbList->GetCurSel();
			LPARAM lpSel = plbList->GetCurSel() != LB_ERR ? plbList->GetCurSelItemData() : -1;
			plbList->ResetContent();
			for ( size_t I = 0; I < m_vPrograms.size(); ++I ) {
				INT iRes = plbList->AddString( m_vPrograms[I].wsMenuName.c_str() );
				if ( iRes != LB_ERRSPACE ) {
					plbList->SetItemData( iRes, I );
					if ( I == lpSel ) {
						plbList->SetCurSel( I );
					}
				}
			}
		}
	}

	// Handles the Add button.
	bool COptionsPagePrograms::AddButton() {
		auto aIdx = m_vPrograms.size();
		try {
			m_vPrograms.push_back( MX_PROGRAM() );
			m_vPrograms[aIdx].wsMenuName = _DEC_WS_75FF111D_New_Program;
			ApplySettings();
			FillInfoBySelection();
			return true;
		}
		catch ( ... ) { return false; }
	}

	// Handles the Delete button.
	bool COptionsPagePrograms::DeleteButton() {
		std::vector<size_t> sResult;
		GatherSelected( sResult );
		for ( auto I = sResult.size(); I--; ) {
			m_vPrograms.erase( m_vPrograms.begin() + sResult[I] );
		}
		ApplySettings();
		FillInfoBySelection();
		return true;
	}

	// Handles the Up button.
	bool COptionsPagePrograms::UpButton() {
		std::vector<size_t> sResult;
		GatherSelected( sResult );
		CUtilities::MoveElementsUp( m_vPrograms, sResult );
		ApplySettings();
		if ( sResult.size() ) {
			CListBox * plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
			if ( plbList ) {
				plbList->SetCurSelByItemData( std::max<LPARAM>( sResult[0] - 1, 0 ) );
			}
		}
		return true;
	}

	// Handles the Down button.
	bool COptionsPagePrograms::DownButton() {
		std::vector<size_t> sResult;
		GatherSelected( sResult );
		CUtilities::MoveElementsDown( m_vPrograms, sResult );
		ApplySettings();
		if ( sResult.size() ) {
			CListBox * plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
			if ( plbList ) {
				plbList->SetCurSelByItemData( std::min<LPARAM>( sResult[0] + 1, m_vPrograms.size() - 1 ) );
			}
		}
		return true;
	}

	// Handles the Hidden check.
	bool COptionsPagePrograms::HiddenCheck() {
		int iSel = SelectedIdx();
		if ( iSel != -1 && size_t( iSel ) < m_vPrograms.size() ) {
			CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_HIDDEN_CHECK ));
			if ( pcbCheck ) {
				m_vPrograms[iSel].bVisible = !pcbCheck->IsChecked();
				return true;
			}
		}
		return false;
	}

	// Browse for a program.
	bool COptionsPagePrograms::BrowseProgram() {
		int iSel = SelectedIdx();
		if ( iSel != -1 && size_t( iSel ) < m_vPrograms.size() ) {
			OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
			std::wstring szFileName;
			szFileName.resize( 0xFFFF + 2 );

			CSecureWString wsFilter = _DEC_WS_412B1AEA_Executable_Files____exe__0__exe__0All_Files_______0____0_0;
			ofnOpenFile.hwndOwner = Wnd();
			ofnOpenFile.lpstrFilter = wsFilter.c_str();
			ofnOpenFile.lpstrFile = szFileName.data();
			ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
			ofnOpenFile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

			if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
				m_vPrograms[iSel].wsPath = ofnOpenFile.lpstrFile;
				CEdit * peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_PROGRAM_EDIT ));
				if ( peEdit ) {
					peEdit->SetTextW( m_vPrograms[iSel].wsPath.c_str() );
				}
				return true;
			}
		}
		return false;
	}

	// Browse for a working directory.
	bool COptionsPagePrograms::BrowseWorkingDirectory() {
		int iSel = SelectedIdx();
		if ( iSel != -1 && size_t( iSel ) < m_vPrograms.size() ) {
			std::wstring szFileName;
			szFileName.resize( 0xFFFF + 2 );
			CSecureWString wsSelectFolder = _DEC_WS_59A477CC_Select_a_Folder;

			BROWSEINFOW biInfo = {};
			biInfo.hwndOwner = Wnd();
			biInfo.pszDisplayName = szFileName.data();
			biInfo.lpszTitle = wsSelectFolder.c_str();
			LPITEMIDLIST lpilList = ::SHBrowseForFolderW( &biInfo );
			if ( lpilList != NULL ) {
				if ( ::SHGetPathFromIDListW( lpilList, szFileName.data() ) ) {
					m_vPrograms[iSel].wsWorkingDir = szFileName;
					CEdit * peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_WORKINGDIR_EDIT ));
					if ( peEdit ) {
						peEdit->SetTextW( m_vPrograms[iSel].wsWorkingDir.c_str() );
					}
					return true;
				}
			}
		
		}
		return false;
	}

	// Update the show type.
	bool COptionsPagePrograms::UpdateShowType() {
		int iSel = SelectedIdx();
		if ( iSel != -1 && size_t( iSel ) < m_vPrograms.size() ) {
			CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_SHOW_CMD_COMBO ));
			if ( pcbCombo ) {
				m_vPrograms[iSel].iShowCmd = static_cast<INT>(pcbCombo->GetCurSelItemData());
			}
		}
		return false;
	}

	// Gets the selected index or -1.
	int COptionsPagePrograms::SelectedIdx() const {
		const CListBox * plbList = static_cast<const CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
		if ( plbList ) {
			INT iSel = plbList->GetCurSel();
			LPARAM lpSel = plbList->GetCurSel() != LB_ERR ? plbList->GetCurSelItemData() : -1;
			if ( lpSel != -1 && size_t( lpSel ) < m_vPrograms.size() ) {
				return static_cast<int>(lpSel);
			}
		}
		return -1;
	}

	// Fills a set of LPARAM's, one for each selected item in the list box.
	void COptionsPagePrograms::GatherSelected( std::vector<size_t> &_sResult ) {
		CListBox * plbList = nullptr;
		plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
		if ( plbList ) {
			std::vector<INT> vSelections;
			INT iSel = plbList->GetCurSel();
			if ( iSel != LB_ERR ) {
				_sResult.push_back( size_t( plbList->GetItemData( iSel ) ) );
			}
			/*if ( plbList->GetSelItems( vSelections ) ) {
				for ( size_t I = 0; I < vSelections.size(); ++I ) {
					_sResult.push_back( plbList->GetItemData( vSelections[I] ) );
				}
			}*/
		}
	}

	// Sets the current information based on the current selection.
	void COptionsPagePrograms::FillInfoBySelection() {
		CListBox * plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
		if ( plbList ) {
			INT iSel = plbList->GetCurSel();
			LPARAM lpSel = plbList->GetCurSel() != LB_ERR ? plbList->GetCurSelItemData() : -1;
			if ( lpSel != -1 && size_t( lpSel ) < m_vPrograms.size() ) {
				CEdit * peEdit = nullptr;
				CCheckButton * pcbCheck = nullptr;
				CComboBox * pcbCombo = nullptr;

				peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_NAME_EDIT ));
				if ( peEdit ) {
					peEdit->SetTextW( m_vPrograms[lpSel].wsMenuName.c_str() );
				}

				peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_PROGRAM_EDIT ));
				if ( peEdit ) {
					peEdit->SetTextW( m_vPrograms[lpSel].wsPath.c_str() );
				}

				peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_ARGUMENTS_EDIT ));
				if ( peEdit ) {
					peEdit->SetTextW( m_vPrograms[lpSel].wsArgs.c_str() );
				}

				peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_WORKINGDIR_EDIT ));
				if ( peEdit ) {
					peEdit->SetTextW( m_vPrograms[lpSel].wsWorkingDir.c_str() );
				}

				pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_HIDDEN_CHECK ));
				if ( pcbCheck ) {
					pcbCheck->SetCheck( !m_vPrograms[lpSel].bVisible );
				}

				pcbCombo = static_cast<CComboBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_SHOW_CMD_COMBO ));
				if ( pcbCombo ) {
					pcbCombo->SetCurSelByItemData( m_vPrograms[lpSel].iShowCmd );
				}
			}
		}
	}

}	// namespace mx

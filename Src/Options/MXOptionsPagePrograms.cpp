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
		}
		return LSW_H_CONTINUE;
	}

	// Verifies the options, returning an error string in case of error.
	BOOL COptionsPagePrograms::Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
		ee::CExpEvalContainer::EE_RESULT eRes;
		CWidget * pwTemp = nullptr;
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
		std::vector<LPARAM> sResult;
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
		
		return true;
	}

	// Handles the Down button.
	bool COptionsPagePrograms::DownButton() {
		
		return true;
	}

	// Handles the Hidden check.
	bool COptionsPagePrograms::HiddenCheck() {
		CListBox * plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
		if ( plbList ) {
			INT iSel = plbList->GetCurSel();
			LPARAM lpSel = plbList->GetCurSel() != LB_ERR ? plbList->GetCurSelItemData() : -1;
			if ( lpSel != -1 && size_t( lpSel ) < m_vPrograms.size() ) {
				CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_HIDDEN_CHECK ));
				if ( pcbCheck ) {
					m_vPrograms[lpSel].bVisible = !pcbCheck->IsChecked();
					return true;
				}
			}
		}
		return false;
	}

	// Browse for a program.
	bool COptionsPagePrograms::BrowseProgram() {
		CListBox * plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
		if ( plbList ) {
			INT iSel = plbList->GetCurSel();
			LPARAM lpSel = plbList->GetCurSel() != LB_ERR ? plbList->GetCurSelItemData() : -1;
			if ( lpSel != -1 && size_t( lpSel ) < m_vPrograms.size() ) {
		
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
					m_vPrograms[lpSel].wsPath = ofnOpenFile.lpstrFile;
					CEdit * peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_PROGRAM_EDIT ));
					if ( peEdit ) {
						peEdit->SetTextW( m_vPrograms[lpSel].wsPath.c_str() );
					}
					return true;
				}
			}
		}
		return false;
	}

	// Browse for a working directory.
	bool COptionsPagePrograms::BrowseWorkingDirectory() {
		CListBox * plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
		if ( plbList ) {
			INT iSel = plbList->GetCurSel();
			LPARAM lpSel = plbList->GetCurSel() != LB_ERR ? plbList->GetCurSelItemData() : -1;
			if ( lpSel != -1 && size_t( lpSel ) < m_vPrograms.size() ) {
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
						m_vPrograms[lpSel].wsWorkingDir = szFileName;
						CEdit * peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_OPTIONS_WORKINGDIR_EDIT ));
						if ( peEdit ) {
							peEdit->SetTextW( m_vPrograms[lpSel].wsWorkingDir.c_str() );
						}
						return true;
					}
				}
		
			}
		}
		return false;
	}

	// Fills a set of LPARAM's, one for each selected item in the list box.
	void COptionsPagePrograms::GatherSelected( std::vector<LPARAM> &_sResult ) {
		CListBox * plbList = nullptr;
		plbList = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_PROGRAMS_LIST ));
		if ( plbList ) {
			std::vector<INT> vSelections;
			INT iSel = plbList->GetCurSel();
			if ( iSel != LB_ERR ) {
				_sResult.push_back( plbList->GetItemData( iSel ) );
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
			}
		}
	}

}	// namespace mx

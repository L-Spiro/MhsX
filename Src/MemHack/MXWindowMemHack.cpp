#include "MXWindowMemHack.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXLayoutManager.h"
#include "../MainWindow//MXMhsMainWindow.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <StatusBar/LSWStatusBar.h>

#include <psapi.h>


namespace mx {

	CWindowMemHack::CWindowMemHack() :
		m_pmmwWindow( nullptr ) {
		m_pmmwWindow = reinterpret_cast<CMhsMainWindow *>(mx::CMainWindowLayout::CreateMainWindow( this ));

		AddUserProgramsToMenu();

		m_pProcess.SetDetatchCallback( DetatchCallback, this, 0 );
	}
	CWindowMemHack::~CWindowMemHack() {
		m_pmmwWindow = nullptr;
	}

	// == Functions.
	// Opened a process.
	void CWindowMemHack::OpenedProcess() {
		CMemHack::OpenedProcess();
		if ( m_pmmwWindow ) {
			UpdateWindowTitle();

			CStatusBar * psbStatus = m_pmmwWindow->StatusBar();
			if ( psbStatus ) {
				CSecureWString sTemp = _DEC_WS_DAC52882_Opened_;
				CUtilities::ToHex( m_pProcess.ProcId(), sTemp, 4 );
				sTemp.append( L" (" );
				CUtilities::ToUnsigned( m_pProcess.ProcId(), sTemp );
				sTemp.append( L")." );
				/*sTemp.push_back( L' ' );
				std::string sFlagsTemp;
				CUtilities::OpenProcessFlagToString( m_pProcess.Flags(), sFlagsTemp, TRUE );
				sTemp += CUtilities::StringToWString( sFlagsTemp );*/
				psbStatus->SetTextW( 0, 0, sTemp.c_str() );
			}
		}
	}

	/**
	 * Updates the window title.
	 **/
	void CWindowMemHack::UpdateWindowTitle() {
		if ( m_pmmwWindow ) {
			CSecureWString swsString;
			if ( m_pProcess.ProcIsOpened() ) {
				if ( m_oOptions.dwOpenProc & MX_OP_TITLE_BAR_PROC_NAME ) {
					if ( swsString.size() ) {
						swsString += L'\u00A0';
					}
					swsString += CUtilities::NoExtension( CUtilities::GetFileName( m_pProcess.QueryProcessImageName() ) );

					if ( m_oOptions.dwOpenProc & MX_OP_TITLE_BAR_X86_FLAG ) {
						if ( m_pProcess.IsWow64Process() ) {
							swsString += L'*';
							swsString += L'\u00B3';	// 3
							swsString += L'\u00B2';	// 2
						}
					}
				}
				else if ( m_oOptions.dwOpenProc & MX_OP_TITLE_BAR_X86_FLAG ) {
					if ( swsString.size() ) {
						swsString += L'\u00A0';
					}
					if ( m_pProcess.IsWow64Process() ) {
						swsString += _DEC_WS_91D489A0__x86_; 
					}
				}
				if ( m_oOptions.dwOpenProc & MX_OP_TITLE_BAR_PROC_ID ) {
					if ( swsString.size() ) {
						swsString += L'\u00A0';
					}
					CUtilities::ToHex( m_pProcess.ProcId(), swsString, 4 );
					swsString.append( L"\u00A0(" );
					CUtilities::ToUnsigned( m_pProcess.ProcId(), swsString );
					swsString.append( L")" );
				}

				if ( m_oOptions.dwOpenProc & MX_OP_TITLE_BAR_RANDOM ) {
					const wchar_t wcTable[] = {
						L'a',
						L'b',
						L'e',
						L'i',
						L'd',
						L'o',
						L'u',
						L'C',
						L'2',
						L'0',
						L'-',
						L'_',
						L'\u00A0',
					};
					CUtilities::InsertRandomChars( swsString, swsString.size() / 3, wcTable, LSW_ELEMENTS( wcTable ) );
					CUtilities::ReplaceRandomChars( swsString, swsString.size() / 5, wcTable, LSW_ELEMENTS( wcTable ) );
				}
			}
			
			if ( !swsString.size() ) {
				swsString = _DEC_WS_BA5DABD6_L___Spiro__MHS__X;
			}
			m_pmmwWindow->SetTextW( swsString.c_str() );
		}
	}

	// Failed to open a process.
	void CWindowMemHack::FailedToOpenProcess( DWORD _dwId ) {
		CMemHack::FailedToOpenProcess( _dwId );
		if ( m_pmmwWindow ) {
			UpdateWindowTitle();

			CStatusBar * psbStatus = m_pmmwWindow->StatusBar();
			if ( psbStatus ) {
				CSecureWString sTemp = _DEC_WS_5D59A6A1_Failed_to_open_;
				CUtilities::ToHex( _dwId, sTemp, 4 );
				sTemp.append( L" (" );
				CUtilities::ToUnsigned( _dwId, sTemp );
				sTemp.append( L"). " );
				CBase::AppendError( L"", sTemp );
				psbStatus->SetTextW( 0, 0, sTemp.c_str() );
			}
		}
	}

	// Detach from the current process.
	void CWindowMemHack::Detach( BOOL _bCallProcessDetach ) {
		if ( m_pmmwWindow ) {
			UpdateWindowTitle();

			CStatusBar * psbStatus = m_pmmwWindow->StatusBar();
			if ( psbStatus && psbStatus->IsStatusBar() ) {
				try {
					CSecureWString sTemp = _DEC_WS_6EA39706_Detached_from_;
					CUtilities::ToHex( m_pProcess.ProcId(), sTemp, 4 );
					sTemp.append( L" (" );
					CUtilities::ToUnsigned( m_pProcess.ProcId(), sTemp );
					sTemp.append( L")." );
					/*sTemp.push_back( L' ' );
					std::string sFlagsTemp;
					CUtilities::OpenProcessFlagToString( m_pProcess.Flags(), sFlagsTemp, TRUE );
					sTemp += CUtilities::StringToWString( sFlagsTemp );*/
					psbStatus->SetTextW( 0, 0, sTemp.c_str() );
				}
				catch ( ...  ) {}
			}
		}
		CMemHack::Detach( _bCallProcessDetach );
	}

	// Sets the options.
	void CWindowMemHack::SetOptions( const MX_OPTIONS &_oOptions ) {
		Parent::SetOptions( _oOptions );
		if ( m_pmmwWindow ) {
			if ( m_pmmwWindow->ExpressionEvaluator() ) {
				m_pmmwWindow->ExpressionEvaluator()->SetUpdateSpeed( 1000 / max( Options().dwExpressionRefresh, static_cast<DWORD>(1) ) );
			}
			if ( m_pmmwWindow->FoundAddresses() ) {
				m_pmmwWindow->FoundAddresses()->SetUpdateSpeed( 1000 / max( Options().dwFoundAddressRefresh, static_cast<DWORD>(1) ) );
			}

			AddUserProgramsToMenu();

			UpdateWindowTitle();
		}
	}

	// Adds user programs to the menu.
	void CWindowMemHack::AddUserProgramsToMenu() {
		if ( !m_pmmwWindow ) { return; }
		HMENU hMenu = ::GetSubMenu( ::GetMenu( m_pmmwWindow->Wnd() ), 2 );
		std::vector<int> vIds = CHelpers::GetMenuItemIDs( hMenu );
		// Delete all menu items with a command greater than or equal to CMainWindowLayout::MX_MWMI_USER_PROGRAMS_BASE.
		for ( auto I = vIds.size(); I--; ) {
			if ( vIds[I] >= CMainWindowLayout::MX_MWMI_USER_PROGRAMS_BASE ) {
				::DeleteMenu( hMenu, vIds[I], MF_BYCOMMAND );
			}
		}

		CHelpers::SanitizeMenuSeparators( hMenu );

		if ( m_vPrograms.size() ) {
			bool bVisible = false;
			for ( auto I = m_vPrograms.size(); I--; ) {
				if ( m_vPrograms[I].bVisible ) {
					bVisible = true;
					break;
				}
			}
			if ( bVisible ) {
				int iInsertBefore = CMainWindowLayout::MX_MWMI_FLOATINGPOINTSTUDIO;
				vIds = CHelpers::GetMenuItemIDs( hMenu );
				for ( auto I = vIds.size(); I--; ) {
					if ( vIds[I] == iInsertBefore ) {
						iInsertBefore = static_cast<int>(I + 1);
						break;
					}
				}

				::InsertMenuW( hMenu, iInsertBefore++,
					MF_BYPOSITION | MF_SEPARATOR, 0, NULL );

				for ( size_t I = 0; I < m_vPrograms.size(); ++I ) {
					if ( m_vPrograms[I].bVisible ) {
						::InsertMenuW( hMenu, iInsertBefore,
							MF_BYPOSITION | MF_STRING, CMainWindowLayout::MX_MWMI_USER_PROGRAMS_BASE + I, m_vPrograms[I].wsMenuName.c_str() );
						++iInsertBefore;
					}
				}
				::InsertMenuW( hMenu, iInsertBefore++,
					MF_BYPOSITION | MF_SEPARATOR, 0, NULL );
			}

		}
		CHelpers::SanitizeMenuSeparators( hMenu );
		return;
		/*HMENU hMenu = ::GetSubMenu( ::GetMenu( m_pmmwWindow->Wnd() ), CMainWindowLayout::MX_MWMI_MENU_TOOLS );
		if ( hMenu ) {
			// Gather menu ID
		}*/
	}

	// The process detatch callback.
	void CWindowMemHack::DetatchCallback( void * _pvParm1,  uintptr_t /*_uiptrParm2*/ ) {
		//static_cast<CWindowMemHack *>(_pvParm1)->Detach( FALSE );
		CWindowMemHack * pwmhMemHack = static_cast<CWindowMemHack *>(_pvParm1);
		if ( pwmhMemHack->m_pmmwWindow ) {
			::PostMessageW( pwmhMemHack->m_pmmwWindow->Wnd(), CMhsMainWindow::MX_CUSTOM_MESSAGE::MX_CM_DETATCHED, 0, 0 );
		}
	}

}	// namespace mx
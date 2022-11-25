#include "MXWindowMemHack.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXLayoutManager.h"
#include "../MainWindow//MXMhsMainWindow.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <StatusBar/LSWStatusBar.h>


namespace mx {

	CWindowMemHack::CWindowMemHack() :
		m_pmmwWindow( nullptr ) {
		m_pmmwWindow = reinterpret_cast<CMhsMainWindow *>(mx::CMainWindowLayout::CreateMainWindow( this ));

		AddUserProgramsToMenu();
	}
	CWindowMemHack::~CWindowMemHack() {
		m_pmmwWindow = nullptr;
	}

	// == Functions.
	// Opened a process.
	void CWindowMemHack::OpenedProcess() {
		CMemHack::OpenedProcess();
		if ( m_pmmwWindow ) {
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

	// Failed to open a process.
	void CWindowMemHack::FailedToOpenProcess( DWORD _dwId ) {
		CMemHack::FailedToOpenProcess( _dwId );
		if ( m_pmmwWindow ) {
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
	void CWindowMemHack::Detach() {
		if ( m_pmmwWindow ) {
			CStatusBar * psbStatus = m_pmmwWindow->StatusBar();
			if ( psbStatus ) {
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
		}

		CMemHack::Detach();
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

}	// namespace mx
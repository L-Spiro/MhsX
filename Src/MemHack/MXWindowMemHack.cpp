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
				std::wstring sTemp = _DEC_WS_DAC52882_Opened_;
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
				std::wstring sTemp = _DEC_WS_5D59A6A1_Failed_to_open_;
				CUtilities::ToHex( _dwId, sTemp, 4 );
				sTemp.append( L" (" );
				CUtilities::ToUnsigned( _dwId, sTemp );
				sTemp.append( L"). " );
				CBase::AppendError( L"", sTemp );
				psbStatus->SetTextW( 0, 0, sTemp.c_str() );
			}
		}
	}

}	// namespace mx
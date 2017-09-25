#include "MXOpenProcessWindow.h"
#include "../Layouts/MXOpenProcessLayout.h"
#include "ListView/LSWListView.h"
#include "../System/MXSystem.h"

#include <Base/LSWBase.h>
#include <CommCtrl.h>

namespace mx {

	COpenProcessWindow::~COpenProcessWindow() {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COpenProcessWindow::InitDialog() {

		
		LV_COLUMNW lvColumn;
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 150;
		lvColumn.iImage = -1;
		CHAR * ptcHeaders[] = {
			"Process",
			"Process ID",
			"Windows",
		};
		CListView * plvLIst = static_cast<CListView *>(FindChild( COpenProcessLayout::MX_OPI_LISTVIEW ));
		for ( INT I = 0; I < MX_ELEMENTS( ptcHeaders ); I++ ) {
			
			INT iCol = plvLIst->AddColumn( ptcHeaders[I] );
			plvLIst->SetColumnWidth( iCol, 150 );
		}

		std::vector<MX_PROCESSES> vProcesses;
		CreateProcessList( vProcesses );


		return LSW_H_CONTINUE;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED COpenProcessWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED COpenProcessWindow::Command( WORD _Id, HWND _hControl ) {
		switch ( _Id ) {
			/*case COpenProcessLayout::MX_OP: {
				COpenProcessLayout::CreateOpenProcessDialog( this );
				break;
			}*/
		}
		return LSW_H_CONTINUE;
	}

	// Creates a vector of MX_PROCESSES structures.
	BOOL COpenProcessWindow::CreateProcessList( std::vector<MX_PROCESSES> &_pProcesses ) {
		// Create a stapshot.
		LSW_HANDLE hSnapObj = CSystem::CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0 );
		if ( hSnapObj.hHandle == INVALID_HANDLE_VALUE ) {
#ifdef _DEBUG
			lsw::CBase::PrintError( L"Unable to create process snapshot." );
#endif	// #ifdef _DEBUG
			return FALSE;
		}

		PROCESSENTRY32W pe32Pe = { sizeof( pe32Pe ) };
		if ( !CSystem::Process32FirstW( hSnapObj.hHandle, &pe32Pe ) ) {
#ifdef _DEBUG
			lsw::CBase::PrintError( L"CSystem::Process32FirstW()." );
#endif	// #ifdef _DEBUG
			return FALSE;
		}

		do {
			MX_PROCESSES pProc;
			pProc.peProcEntry = pe32Pe;
			pProc.dwId = pe32Pe.th32ProcessID;

			LSW_HANDLE hProc = CSystem::OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, pe32Pe.th32ProcessID );
			if ( hProc.hHandle ) {
				DWORD dwRealLen = CSystem::FullProcessPathLen( hProc.hHandle );
				pProc.sPath.resize( dwRealLen );
				CSystem::QueryFullProcessImageNameW( hProc.hHandle, 0, &pProc.sPath[0], &dwRealLen );

				dwRealLen += 10;
			}
			else {
				pProc.sPath = L"<?>";
			}

			_pProcesses.push_back( pProc );
		} while ( CSystem::Process32NextW( hSnapObj.hHandle, &pe32Pe ) );

		return TRUE;
	}

}	// namespace mx

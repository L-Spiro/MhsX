#include "MXOpenProcessWindow.h"
#include "../Layouts/MXOpenProcessLayout.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"

#include <Base/LSWBase.h>
#include <CommCtrl.h>

namespace mx {

	COpenProcessWindow::~COpenProcessWindow() {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COpenProcessWindow::InitDialog() {

		
		/*LV_COLUMNW lvColumn;
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 150;
		lvColumn.iImage = -1;*/
		CHAR * ptcHeaders[] = {
			"Process",
			"Process ID",
			"Path",
			"Windows",
		};
		CListView * plvLIst = static_cast<CListView *>(FindChild( COpenProcessLayout::MX_OPI_LISTVIEW ));
		for ( INT I = 0; I < MX_ELEMENTS( ptcHeaders ); I++ ) {
			
			INT iCol = plvLIst->AddColumn( ptcHeaders[I] );
			plvLIst->SetColumnWidth( iCol, 150 );
		}

		std::vector<MX_PROCESSES> vProcesses;
		CreateProcessList( vProcesses );

		FillListView( plvLIst, vProcesses );


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
				AddWindowTitles( hSnapObj.hHandle, pProc );
				

				dwRealLen += 10;
			}
			/*else {
				pProc.sPath = L"<?>";
			}*/

			_pProcesses.push_back( pProc );
		} while ( CSystem::Process32NextW( hSnapObj.hHandle, &pe32Pe ) );

		DWORD dwTotalProcesses = CSystem::EnumProcessesBufferSize();
		std::vector<DWORD> vIds;
		vIds.resize( dwTotalProcesses );
		CSystem::EnumProcesses( &vIds[0], vIds.size() * sizeof( DWORD ), &dwTotalProcesses );
		dwTotalProcesses /= sizeof( DWORD );

		return TRUE;
	}

	// Adds window titles to an MX_PROCESSES structure.
	VOID COpenProcessWindow::AddWindowTitles( HANDLE _hSnap, MX_PROCESSES &_pProc ) {
		THREADENTRY32 te32Te = { sizeof( THREADENTRY32 ) };
		if ( !CSystem::Thread32First( _hSnap, &te32Te ) ) { return; }
		
		do {
			if ( te32Te.th32OwnerProcessID == _pProc.dwId ) {
				CSystem::EnumThreadWindows( te32Te.th32ThreadID, EnumThreadWindows_GatherWindows, reinterpret_cast<LPARAM>(&_pProc) );
			}
		} while ( CSystem::Thread32Next( _hSnap, &te32Te ) );
	}

	// Fills a list view with the given MX_PROCESSES objects.
	VOID COpenProcessWindow::FillListView( lsw::CListView * _plvView, const std::vector<MX_PROCESSES> &_vProcs ) {
		for ( size_t I = 0; I < _vProcs.size(); ++I ) {
			INT iItem = _plvView->InsertItem( _vProcs[I].peProcEntry.szExeFile, _vProcs[I].dwId );

			std::string sTemp;
			CUtilities::ToHex( _vProcs[I].dwId, sTemp, 4 );
			sTemp += " (";
			CUtilities::ToUnsigned( _vProcs[I].dwId, sTemp );
			sTemp += ')';
			_plvView->SetItemText( iItem, 1, sTemp.c_str() );

			_plvView->SetItemText( iItem, 2, _vProcs[I].sPath.c_str() );

			_plvView->SetItemText( iItem, 3, _vProcs[I].sWindows.c_str() );
		}
	}

	// Enumerate thread windows.
	BOOL CALLBACK COpenProcessWindow::EnumThreadWindows_GatherWindows( HWND _hWnd, LPARAM _lParam ) {
		MX_PROCESSES * ppProc = reinterpret_cast<MX_PROCESSES *>(_lParam);
		std::wstring wTemp;
		CSystem::GetWindowTextW( _hWnd, wTemp );
		if ( wTemp.size() ) {
			if ( ppProc->sWindows.size() ) {
				ppProc->sWindows.push_back( L',' );
				ppProc->sWindows.push_back( L' ' );
			}
			ppProc->sWindows.append( wTemp );
			//ppProc->vWindows.push_back( wTemp );
		}
		::EnumChildWindows( _hWnd, EnumChildWindows_GatherWindows, _lParam );

		return TRUE;
	}

	// Enumerate child windows.
	BOOL CALLBACK COpenProcessWindow::EnumChildWindows_GatherWindows( HWND _hWnd, LPARAM _lParam ) {
		MX_PROCESSES * ppProc = reinterpret_cast<MX_PROCESSES *>(_lParam);
		std::wstring wTemp;
		CSystem::GetWindowTextW( _hWnd, wTemp );
		if ( wTemp.size() ) {
			if ( ppProc->sWindows.size() ) {
				ppProc->sWindows.push_back( L',' );
				ppProc->sWindows.push_back( L' ' );
			}
			ppProc->sWindows.append( wTemp );
			//ppProc->vWindows.push_back( wTemp );
		}

		return TRUE;
	}

}	// namespace mx

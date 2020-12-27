#include "MXOpenProcessWindow.h"
#include "../Layouts/MXOpenProcessLayout.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"

#include <Base/LSWBase.h>
#include <CommCtrl.h>

namespace mx {

	COpenProcessWindow::COpenProcessWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<MX_OPTIONS *>(_ui64Data) ) {
		m_dwMainOrAll = COpenProcessLayout::MX_OPI_RADIO_MAIN;
	}

	COpenProcessWindow::~COpenProcessWindow() {
	}

	// == Functions.
	// Gets the selected process ID and closes the window if valid.
	DWORD COpenProcessWindow::GetSelectedId() {
		CListView * plvView = static_cast<CListView *>(FindChild( COpenProcessLayout::MX_OPI_LISTVIEW ));
		LPARAM lpId = plvView->GetSelData();
		if ( lpId == -1 ) {
			CSystem::MessageBoxError( Wnd(), _T_CD04DF4D_No_process_has_been_selected_, _LEN_CD04DF4D, _T_F1AF2E7E_Select_a_Process, _LEN_F1AF2E7E );
			return DWINVALID;
		}
		::EndDialog( Wnd(), lpId );
		return static_cast<DWORD>(lpId);
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COpenProcessWindow::InitDialog() {
		const struct {
			const char * _pcText;
			size_t sLen;
			DWORD dwFlag;
		} aTitles[] = {
			{ _T_49A0210A_Process, _LEN_49A0210A, 0 },
			{ _T_C0D8DDA3_Process_ID, _LEN_C0D8DDA3, 0 },
			{ _T_AB662431_Path, _LEN_AB662431, MX_OP_SHOW_PATH },
			{ _T_2C5ABC07_Windows, _LEN_2C5ABC07, MX_OP_SHOW_WINDOWS },
			{ _T_3A226579_Parent, _LEN_3A226579, MX_OP_SHOW_PARENT },
		};
		DWORD dwFlags = m_poOptions ? m_poOptions->dwOpenProc : MX_OP_SHOW_ALL;
		CListView * plvLIst = static_cast<CListView *>(FindChild( COpenProcessLayout::MX_OPI_LISTVIEW ));
		for ( INT I = 0; I < MX_ELEMENTS( aTitles ); I++ ) {
			if ( !aTitles[I].dwFlag || (aTitles[I].dwFlag & dwFlags) ) {
				INT iCol = plvLIst->AddColumn( mx::CStringDecoder::DecodeToWString( aTitles[I]._pcText, aTitles[I].sLen ).c_str() );
				plvLIst->SetColumnWidth( iCol, 150 );
			}
		}

		MainList();		


		return LSW_H_CONTINUE;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED COpenProcessWindow::Close() {
		::EndDialog( Wnd(), -1 );
		return LSW_H_HANDLED;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED COpenProcessWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		if ( m_dwMainOrAll == _Id ) { return LSW_H_CONTINUE; }
		switch ( _Id ) {
			case COpenProcessLayout::MX_OPI_RADIO_MAIN : {
				MainList();
				break;
			}
			case COpenProcessLayout::MX_OPI_RADIO_ALL : {
				AllList();
				break;
			}
			case COpenProcessLayout::MX_OPI_BUTTON_REFRESH : {
				if ( m_dwMainOrAll == COpenProcessLayout::MX_OPI_RADIO_MAIN ) {
					MainList();
				}
				else {
					AllList();
				}
				break;
			}
			case COpenProcessLayout::MX_OPI_BUTTON_CANCEL : {
				return Close();
			}
			case COpenProcessLayout::MX_OPI_BUTTON_OK : {
				GetSelectedId();
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// Main process list.
	VOID COpenProcessWindow::MainList() {
		std::vector<MX_PROCESSES> vProcesses;
		CreateProcessList( vProcesses );
				
		CListView * plvLIst = static_cast<CListView *>(FindChild( COpenProcessLayout::MX_OPI_LISTVIEW ));
		FillListView( plvLIst, vProcesses );
		m_dwMainOrAll = COpenProcessLayout::MX_OPI_RADIO_MAIN;
	}

	// All process list.
	VOID COpenProcessWindow::AllList() {
		std::vector<MX_PROCESSES> vProcesses;
		CreateHiddenProcessList( vProcesses );
				
		CListView * plvLIst = static_cast<CListView *>(FindChild( COpenProcessLayout::MX_OPI_LISTVIEW ));
		FillListView( plvLIst, vProcesses );
		m_dwMainOrAll = COpenProcessLayout::MX_OPI_RADIO_ALL;
	}

	// Creates a vector of MX_PROCESSES structures.
	BOOL COpenProcessWindow::CreateProcessList( std::vector<MX_PROCESSES> &_pProcesses ) {
		DWORD dwFlags = m_poOptions ? m_poOptions->dwOpenProc : MX_OP_SHOW_ALL;
		// Create a stapshot.
		std::vector<PROCESSENTRY32W> vProcs;
		std::vector<THREADENTRY32> vThreads;
		LSW_HANDLE hSnapObj = CreateSnap( vProcs, vThreads );
		if ( hSnapObj.hHandle == INVALID_HANDLE_VALUE ) {
			return FALSE;
		}

		for ( size_t I = 0; I < vProcs.size(); ++I ) {
			MX_PROCESSES pProc;
			pProc.peProcEntry = vProcs[I];
			pProc.dwId = vProcs[I].th32ProcessID;
			pProc.bHasWow64 = FALSE;

			LSW_HANDLE hProc;
			if ( FillProcById( pProc.dwId, pProc, hProc ) ) {
				if ( dwFlags & MX_OP_SHOW_WINDOWS ) {
					AddWindowTitles( vThreads, pProc );
				}
			}
			_pProcesses.push_back( pProc );
		}

		/*
		DWORD dwTotalProcesses = CSystem::EnumProcessesBufferSize();
		std::vector<DWORD> vIds;
		vIds.resize( dwTotalProcesses );
		CSystem::EnumProcesses( &vIds[0], vIds.size() * sizeof( DWORD ), &dwTotalProcesses );
		dwTotalProcesses /= sizeof( DWORD );*/

		return TRUE;
	}

	// Creates a vector of MX_PROCESSES structures using a technique to find hidden process ID's.
	BOOL COpenProcessWindow::CreateHiddenProcessList( std::vector<MX_PROCESSES> &_pProcesses ) {
		DWORD dwFlags = m_poOptions ? m_poOptions->dwOpenProc : MX_OP_SHOW_ALL;
		std::vector<PROCESSENTRY32W> vProcs;
		std::vector<THREADENTRY32> vThreads;
		LSW_HANDLE hSnapObj = CreateSnap( vProcs, vThreads );
		// hSnapObj can be INVALID_HANDLE_VALUE.
#if 0
		std::set<DWORD> sIds;
		CreateProcListByThreadRefs( vThreads, sIds );
#endif	// #if 0
		
		const DWORD dwLast = std::max( static_cast<DWORD>(0xFFFF), HighestProcId( vProcs ) );
		for ( DWORD I = 0; I <= dwLast; I++ ) {
			{
				DWORD dwId = I;
				MX_PROCESSES pProc;
				pProc.peProcEntry = { 0 };
				pProc.peProcEntry.th32ParentProcessID = 0xFFFFFFFF;
				pProc.bHasWow64 = FALSE;
				if ( SnapHasProc( vProcs, dwId, pProc ) ) {
					LSW_HANDLE hProc;
					if ( FillProcById( dwId, pProc, hProc ) ) {
						AddWindowTitles( vThreads, pProc );
					}
					_pProcesses.push_back( pProc );
				}
				else {
					LSW_HANDLE hProc;
					if ( FillProcById( dwId, pProc, hProc ) ) {
						AddWindowTitles( vThreads, pProc );
						// Get the process name from the path.
						::wcscpy_s( pProc.peProcEntry.szExeFile, CSystem::GetFileName( pProc.sPath ).c_str() );
						_pProcesses.push_back( pProc );
					}
					else {
#if 0
#ifdef _DEBUG
						if ( ProcessListHasId( sIds, dwId ) ) {
							lsw::CBase::PrintError( L"ProcessListHasId." );
						}
						HANDLE hHnd = CSystem::OpenProcess( PROCESS_VM_READ, FALSE, dwId );
						if ( hHnd ) {
							lsw::CBase::PrintError( L"PROCESS_VM_READ." );
							::CloseHandle( hHnd );
						}
#endif	// #ifdef _DEBUG
#endif	// #if 0
					}
				}
			}
		}
		return TRUE;
	}

	// Adds window titles to an MX_PROCESSES structure.
	VOID COpenProcessWindow::AddWindowTitles( const std::vector<THREADENTRY32> &_vThreads, MX_PROCESSES &_pProc ) {
		if ( !ShowWindows() ) { return; }
		for ( size_t I = 0; I < _vThreads.size(); ++I ) {
			if ( _vThreads[I].th32OwnerProcessID == _pProc.dwId ) {
				CSystem::EnumThreadWindows( _vThreads[I].th32ThreadID,
					ShowChildWindows() ? EnumThreadWindows_GatherWindows : EnumThreadWindows_GatherWindowsNoChildren,
					reinterpret_cast<LPARAM>(&_pProc) );
			}
		}
	}

	// Fills a list view with the given MX_PROCESSES objects.
	VOID COpenProcessWindow::FillListView( lsw::CListView * _plvView, const std::vector<MX_PROCESSES> &_vProcs ) {
		_plvView->DeleteAll();
		_plvView->SetItemCount( static_cast<INT>(_vProcs.size()) );
		
		for ( size_t I = 0; I < _vProcs.size(); ++I ) {
			INT iIndex = 2;
			CSecureWString wsFile = _vProcs[I].peProcEntry.szExeFile;
			if ( _vProcs[I].bHasWow64 ) {
				wsFile += L" *32";
			}
			INT iItem = _plvView->InsertItem( wsFile.c_str(), _vProcs[I].dwId );

			std::string sTemp;
			CUtilities::ToHex( _vProcs[I].dwId, sTemp, 4 );
			sTemp += " (";
			CUtilities::ToUnsigned( _vProcs[I].dwId, sTemp );
			sTemp += ')';
			_plvView->SetItemText( iItem, 1, sTemp.c_str() );

			if ( ShowPaths() ) {
				_plvView->SetItemText( iItem, iIndex++, _vProcs[I].sPath.c_str() );
			}

			if ( ShowWindows() ) {
				_plvView->SetItemText( iItem, iIndex++, _vProcs[I].sWindows.c_str() );
			}

			if ( ShowParents() ) {
				const MX_PROCESSES * ppParent = FindProcess( _vProcs, _vProcs[I].peProcEntry.th32ParentProcessID );
				if ( ppParent ) {
					CSecureWString sParent = ppParent->peProcEntry.szExeFile;
					sParent += L' ';
					sParent += L'(';
					CUtilities::ToHex( ppParent->dwId, sParent, 4 );
					sParent += L')';

					_plvView->SetItemText( iItem, iIndex++, sParent.c_str() );
				}
			}
			
		}
	}

	// Fills an MX_PROCESSES structure based off a process ID.
	BOOL COpenProcessWindow::FillProcById( DWORD _dwId, MX_PROCESSES &_pProc, LSW_HANDLE &_hProcHandle ) {
		_pProc.dwId = _dwId;

		HANDLE hHnd = CSystem::OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, _dwId );
		if ( !hHnd ) { return FALSE; }
		if ( ShowPaths() ) {
			DWORD dwRealLen = CSystem::FullProcessPathLen( hHnd );
			_pProc.sPath.resize( dwRealLen );
			CSystem::QueryFullProcessImageNameW( hHnd, 0, &_pProc.sPath[0], &dwRealLen );
		}
		_pProc.bHasWow64 = FALSE;
		if ( MarkX86() ) {
			BOOL bSuc = CSystem::IsWow64Process( hHnd, &_pProc.bHasWow64 );
			if ( !bSuc ) {
				_pProc.bHasWow64 = FALSE;
			}
		}
		_hProcHandle = hHnd;
		//_pProc.dwOpenProcessTest = 0;
		/*_pProc.dwOpenProcessTest = CSystem::TestOpenProcess( _dwId );
		if ( 0x001f3fff != _pProc.dwOpenProcessTest ) {
			return FALSE;
		}*/
		return TRUE;
	}

	// Determines if a given process has a specified DLL loaded.
	BOOL COpenProcessWindow::ProcHasDll( HANDLE _hSnap, DWORD _dwId, const WCHAR * _pwcDll ) {
		MODULEENTRY32W meMod = { sizeof( meMod ) };
		if ( !CSystem::Module32FirstW( _hSnap, &meMod ) ) { return FALSE; }

		do {
			if ( meMod.th32ProcessID == _dwId ) {
				return TRUE;
			}
		} while ( CSystem::Module32NextW( _hSnap, &meMod ) );
		return FALSE;
	}

	// Determines if a given process ID is in a tool snapshot.
	BOOL COpenProcessWindow::SnapHasProc( std::vector<PROCESSENTRY32W> &_vProcs, DWORD _dwId, MX_PROCESSES &_pProc ) {
		for ( size_t I = 0; I < _vProcs.size(); ++I ) {
			if ( _vProcs[I].th32ProcessID == _dwId ) {
				_pProc.dwId = _dwId;
				_pProc.peProcEntry = _vProcs[I];
				return TRUE;
			}
		}
		return FALSE;
	}

	// Creates a snapshot and stores all PROCESSENTRY32W entries to a vector (for faster scanning).
	HANDLE COpenProcessWindow::CreateSnap( std::vector<PROCESSENTRY32W> &_vProcs, std::vector<THREADENTRY32> &_vThreads ) {
		HANDLE hSnapObj = CSystem::CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0 );
		if ( hSnapObj == INVALID_HANDLE_VALUE ) {
#ifdef _DEBUG
			lsw::CBase::PrintError( L"Unable to create process snapshot." );
#endif	// #ifdef _DEBUG
			return INVALID_HANDLE_VALUE;
		}

		PROCESSENTRY32W pe32Pe = { sizeof( pe32Pe ) };
		if ( !CSystem::Process32FirstW( hSnapObj, &pe32Pe ) ) {
#ifdef _DEBUG
			lsw::CBase::PrintError( L"CSystem::Process32FirstW()." );
#endif	// #ifdef _DEBUG
			::CloseHandle( hSnapObj );
			return INVALID_HANDLE_VALUE;
		}
		do {
			_vProcs.push_back( pe32Pe );
		} while ( CSystem::Process32NextW( hSnapObj, &pe32Pe ) );


		THREADENTRY32 te32Te = { sizeof( te32Te ) };
		if ( !CSystem::Thread32First( hSnapObj, &te32Te ) ) {
#ifdef _DEBUG
			lsw::CBase::PrintError( L"CSystem::Thread32First()." );
#endif	// #ifdef _DEBUG
		}
		else {
			do {
				_vThreads.push_back( te32Te );
			} while ( CSystem::Thread32Next( hSnapObj, &te32Te ) );
		}

		return hSnapObj;
	}

	// Creates a list of process ID's based off the system thread enumeration.
	VOID COpenProcessWindow::CreateProcListByThreadRefs( const std::vector<THREADENTRY32> &_vThreads, std::set<DWORD> &_sProcs ) {
		for ( size_t I = 0; I < _vThreads.size(); ++I ) {
			_sProcs.insert( _vThreads[I].th32OwnerProcessID );
		}
	}

	// Gets the highest process ID.
	DWORD COpenProcessWindow::HighestProcId( const std::vector<PROCESSENTRY32W> &_vProcs ) {
		DWORD dwRet = 0;
		for ( size_t I = 0; I < _vProcs.size(); ++I ) {
			dwRet = max( dwRet, _vProcs[I].th32ParentProcessID );
		}
		return dwRet;
	}

	// Determines if the given process ID is in the given set of process ID's.
	BOOL COpenProcessWindow::ProcessListHasId( const std::set<DWORD> &_sProcs, DWORD _dwId ) {
		return _sProcs.find( _dwId ) != _sProcs.end();
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

	// Enumerate thread windows (no children).
	BOOL CALLBACK COpenProcessWindow::EnumThreadWindows_GatherWindowsNoChildren( HWND _hWnd, LPARAM _lParam ) {
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

		::EnumChildWindows( _hWnd, EnumChildWindows_GatherWindows, _lParam );

		return TRUE;
	}

	// Finds an entry in an array of MX_PROCESSES objects with the given process ID.
	const COpenProcessWindow::MX_PROCESSES * COpenProcessWindow::FindProcess( const std::vector<MX_PROCESSES> &_pProcesses, DWORD _dwId ) {
		for ( size_t I = 0; I < _pProcesses.size(); ++I ) {
			if ( _pProcesses[I].dwId == _dwId ) { return &_pProcesses[I]; }
		}
		return nullptr;
	}

	// WM_NOTIFY->NM_DBLCLK for the owning window if the child either could not be resolved or returned LSW_HANDLED::LSW_H_CONTINUE.
	CWidget::LSW_HANDLED COpenProcessWindow::DblClk( const NMHDR * _phHdr, WORD _wControlId, CWidget * _pwWidget ) {
		if ( _wControlId == COpenProcessLayout::MX_OPI_LISTVIEW ) {
			GetSelectedId();
			return LSW_H_HANDLED;
		}
		return LSW_H_HANDLED;
	}

}	// namespace mx

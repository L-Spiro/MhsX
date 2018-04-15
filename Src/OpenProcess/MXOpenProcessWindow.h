#pragma once

#include "../MXMhsX.h"
#include "../Options/MXOptions.h"
#include <ListView/LSWListView.h>
#include <MainWindow/LSWMainWindow.h>
#include <set>
#include <TlHelp32.h>

using namespace lsw;

namespace mx {
	
	class COpenProcessWindow : public lsw::CMainWindow {
	public :
		COpenProcessWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, MX_OPTIONS * _poOptions, bool _bCreateWidget = true, HMENU _hMenu = NULL );
		~COpenProcessWindow();


		// == Types.
		// Structure for holding all the data for a list of processes.
		struct MX_PROCESSES {
			std::wstring					sPath;
			DWORD							dwId;
			//std::vector<std::wstring>		vWindows;
			std::wstring					sWindows;
			//std::wstring					sCommandLine;
			PROCESSENTRY32W					peProcEntry;
			BOOL							bHasWow64;
			//DWORD							dwOpenProcessTest;
		};


		// == Functions.
		// Gets the selected process ID and closes the window if valid.
		DWORD								GetSelectedId();

		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_CLOSE.
		virtual LSW_HANDLED					Close();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// Main process list.
		VOID								MainList();

		// All process list.
		VOID								AllList();

		// Creates a vector of MX_PROCESSES structures.
		BOOL								CreateProcessList( std::vector<MX_PROCESSES> &_pProcesses );

		// Creates a vector of MX_PROCESSES structures using a technique to find hidden process ID's.
		BOOL								CreateHiddenProcessList( std::vector<MX_PROCESSES> &_pProcesses );

		// Adds window titles to an MX_PROCESSES structure.
		VOID								AddWindowTitles( const std::vector<THREADENTRY32> &_vThreads, MX_PROCESSES &_pProc );

		// Fills a list view with the given MX_PROCESSES objects.
		VOID								FillListView( lsw::CListView * _plvView, const std::vector<MX_PROCESSES> &_vProcs );

		// Fills an MX_PROCESSES structure based off a process ID.
		BOOL								FillProcById( DWORD _dwId, MX_PROCESSES &_pProc, LSW_HANDLE &_hProcHandle );

		// Determines if a given process has a specified DLL loaded.
		BOOL								ProcHasDll( HANDLE _hSnap, DWORD _dwId, const WCHAR * _pwcDll );

		// Determines if a given process ID is in a tool snapshot.
		BOOL								SnapHasProc( std::vector<PROCESSENTRY32W> &_vProcs, DWORD _dwId, MX_PROCESSES &_pProc );

		// Creates a snapshot and stores all PROCESSENTRY32W (etc.) entries to a vector (for faster scanning).
		HANDLE								CreateSnap( std::vector<PROCESSENTRY32W> &_vProcs, std::vector<THREADENTRY32> &_vThreads );

		// Creates a list of process ID's based off the system thread enumeration.
		VOID								CreateProcListByThreadRefs( const std::vector<THREADENTRY32> &_vThreads, std::set<DWORD> &_sProcs );

		// Gets the highest process ID.
		DWORD								HighestProcId( const std::vector<PROCESSENTRY32W> &_vProcs );

		// Determines if the given process ID is in the given set of process ID's.
		BOOL								ProcessListHasId( const std::set<DWORD> &_sProcs, DWORD _dwId );

		// Show paths?
		BOOL								ShowPaths() const { return m_poOptions ? (m_poOptions->dwOpenProc & MX_OP_SHOW_PATH) : TRUE; }

		// Show windows?
		BOOL								ShowWindows() const { return m_poOptions ? (m_poOptions->dwOpenProc & MX_OP_SHOW_WINDOWS) : TRUE; }

		// Show child windows?
		BOOL								ShowChildWindows() const { return ShowWindows() && (m_poOptions ? (m_poOptions->dwOpenProc & MX_OP_SHOW_CHILDWINDOWS) : TRUE); }

		// Show parents?
		BOOL								ShowParents() const { return m_poOptions ? (m_poOptions->dwOpenProc & MX_OP_SHOW_PARENT) : TRUE; }

		// Mark x86?
		BOOL								MarkX86() const { return m_poOptions ? (m_poOptions->dwOpenProc & MX_OP_SHOW_X86) : TRUE; }

		// Enumerate thread windows.
		static BOOL CALLBACK				EnumThreadWindows_GatherWindows( HWND _hWnd, LPARAM _lParam );

		// Enumerate thread windows (no children).
		static BOOL CALLBACK				EnumThreadWindows_GatherWindowsNoChildren( HWND _hWnd, LPARAM _lParam );

		// Enumerate child windows.
		static BOOL CALLBACK				EnumChildWindows_GatherWindows( HWND _hWnd, LPARAM _lParam );

		


	protected :
		// == Members.
		// Main or All.
		DWORD								m_dwMainOrAll;

		// Options.
		MX_OPTIONS *						m_poOptions;

		// == Functions.
		// Finds an entry in an array of MX_PROCESSES objects with the given process ID.
		const MX_PROCESSES *				FindProcess( const std::vector<MX_PROCESSES> &_pProcesses, DWORD _dwId );

		// WM_NOTIFY->NM_DBLCLK for the owning window if the child either could not be resolved or returned LSW_HANDLED::LSW_H_CONTINUE.
		virtual LSW_HANDLED					DblClk( const NMHDR * _phHdr, WORD _wControlId, CWidget * _pwWidget );
	};

}	// namespace mx

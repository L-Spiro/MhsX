#pragma once

#include <ListView/LSWListView.h>
#include <MainWindow/LSWMainWindow.h>
#include <TlHelp32.h>

using namespace lsw;

namespace mx {
	
	class COpenProcessWindow : public lsw::CMainWindow {
	public :
		COpenProcessWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true ) :
			lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget ) {
		}
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
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_CLOSE.
		virtual LSW_HANDLED					Close();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _Id, HWND _hControl );

		// Creates a vector of MX_PROCESSES structures.
		BOOL								CreateProcessList( std::vector<MX_PROCESSES> &_pProcesses );

		// Adds window titles to an MX_PROCESSES structure.
		VOID								AddWindowTitles( HANDLE _hSnap, MX_PROCESSES &_pProc );

		// Fills a list view with the given MX_PROCESSES objects.
		VOID								FillListView( lsw::CListView * _plvView, const std::vector<MX_PROCESSES> &_vProcs );

		// Enumerate thread windows.
		static BOOL CALLBACK				EnumThreadWindows_GatherWindows( HWND _hWnd, LPARAM _lParam );

		// Enumerate child windows.
		static BOOL CALLBACK				EnumChildWindows_GatherWindows( HWND _hWnd, LPARAM _lParam );


	protected :
		// == Functions.
		// Finds an entry in an array of MX_PROCESSES objects with the given process ID.
		const MX_PROCESSES *				FindProcess( const std::vector<MX_PROCESSES> &_pProcesses, DWORD _dwId );
	};

}	// namespace mx

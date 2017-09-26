#pragma once

#include "../Files/MXFile.h"
#include "../PE/MXPeObject.h"
#include <LSWWin.h>
#include <string>
#include <TlHelp32.h>
#include <vector>

// == Macros.
#ifndef MX_COUNT_OF
#define MX_COUNT_OF(x)					((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
#define MX_ELEMENTS( x )				MX_COUNT_OF( x )
#endif	// #ifndef MX_COUNT_OF

// == Types.
typedef BOOL (WINAPI * LPFN_READPROCESSMEMORY)( HANDLE, LPCVOID, LPVOID, SIZE_T, SIZE_T * );
typedef BOOL (WINAPI * LPFN_WRITEPROCESSMEMORY)( HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T * );
typedef BOOL (WINAPI * LPFN_ENUMPROCESSES)( DWORD *, DWORD, DWORD * );
typedef HANDLE (WINAPI * LPFN_OPENPROCESS)( DWORD, BOOL, DWORD );
typedef HANDLE (WINAPI * LPFN_OPENTHREAD)( DWORD, BOOL, DWORD );
typedef HANDLE (WINAPI * LPFN_CREATETOOLHELP32SNAPSHOT)( DWORD, DWORD );
typedef BOOL (WINAPI * LPFN_PROCESS32FIRSTW)( HANDLE, LPPROCESSENTRY32W );
typedef BOOL (WINAPI * LPFN_PROCESS32NEXTW)( HANDLE, LPPROCESSENTRY32W );
typedef BOOL (WINAPI * LPFN_THREAD32FIRST)( HANDLE, LPTHREADENTRY32 );
typedef BOOL (WINAPI * LPFN_THREAD32NEXT)( HANDLE, LPTHREADENTRY32 );
typedef BOOL (WINAPI * LPFN_MODULE32FIRSTW)( HANDLE, LPMODULEENTRY32W );
typedef BOOL (WINAPI * LPFN_MODULE32NEXTW)( HANDLE, LPMODULEENTRY32W );
typedef BOOL (WINAPI * LPFN_QUERYFULLPROCESSIMAGENAMEW)( HANDLE, DWORD, LPWSTR, PDWORD );
typedef BOOL (WINAPI * LPFN_ENUMTHREADWINDOWS)( DWORD, WNDENUMPROC lpfn, LPARAM );

namespace mx {

	class CSystem {
	public :
		// == Functions.
		// Initialize the system.
		static VOID						InitSystem();

		// Is this WoW64?
		static BOOL						IsWow64Process() { return m_bIsWow; }

		// System information.
		static const SYSTEM_INFO &		GetSystemInfo() { return m_siSystemInfo; }

		// Gets a function address by DLL name and function name.
		static LPVOID					GetProcAddress( const WCHAR * _pwcDll, const CHAR * _pcFunc );

		// Gets the path to this .EXE in UTF-8.
		static std::string				GetSelfPath();

		// Gets the path to this .EXE in UTF-16.
		static std::wstring				GetSelfPathW();

		// Gets the path to a given loaded DLL given its UTF-8 name.
		static std::string				GetModulePath( const CHAR * _pcPath );

		// Gets the path to a given loaded DLL given its UTF-16 name.
		static std::wstring				GetModulePathW( const WCHAR * _pwcPath );

		// Gets the length of the string needed to hold the path to this executable, including the terminating NULL.
		static DWORD					GetSelfPathLength();

		// Gets the length of the path of a loaded DLL, including the terminating NULL.
		static DWORD					GetModulePathLength( const WCHAR * _pwcPath );

		// Gets the current directory in UTF-8.
		static std::string				GetCurDir();

		// Gets the current directory in UTF-16.
		static std::wstring				GetCurDirW();

		// Gets the system directory in UTF-8.
		static std::string				GetSystemDir();

		// Gets the system directory in UTF-16.
		static std::wstring				GetSystemDirW();

		// Gets the Windows directory in UTF-8.
		static std::string				GetWindowsDir();

		// Gets the Windows directory in UTF-16.
		static std::wstring				GetWindowsDirW();

		// Gets all the paths in the PATH environment variable.
		static size_t					GetPathEnv( std::vector<std::string> &_vReturn );

		// Gets all the paths in the PATH environment variable.
		static size_t					GetPathEnv( std::vector<std::wstring> &_vReturn );
		static size_t					GetPathEnvW( std::vector<std::wstring> &_vReturn ) { return GetPathEnv( _vReturn ); }

		// Gets all of the search paths for a DLL in the order in which they should be searched.
		static std::vector<std::string>	DllSearchPaths( const WCHAR * _pwcDll, std::vector<std::string> &_vResults, BOOL _bIncludeLoadedModulePath = TRUE );

		// Gets all of the search paths for a DLL in the order in which they should be searched.
		static std::vector<std::wstring>DllSearchPaths( const WCHAR * _pwcDll, std::vector<std::wstring> &_vResults, BOOL _bIncludeLoadedModulePath = TRUE );

		// Loads a DLL by name.  Uses the normal search paths DLL's use (local directory, working directory, system directory, windows directory, PATH directories).
		static BOOL						FindDll( const CHAR * _pcDll, CFile &_fReturn );

		// Loads a DLL by name.  Uses the normal search paths DLL's use (local directory, working directory, system directory, windows directory, PATH directories).
		static BOOL						FindDll( const WCHAR * _pwDll, CFile &_fReturn );

		// Gets the address of a process.  Slow because it loads the DLL from memory, parses it, then discards it.
		static LPCVOID					GetProcAddress( LPCSTR _lpcModule, LPCSTR  _lpcProcName );

		// Faster way to get the address of a function.
		static LPCVOID					GetProcAddress( LPCSTR _lpcModule, LPCSTR  _lpcProcName, const CPeObject &_poObj );

		// Faster way to get the address of a function, using encrypted strings.
		static LPCVOID					GetProcAddress( LPCSTR _lpcModule, size_t _sModuleLen, LPCSTR _lpcProcName, size_t _sProcLen, const CPeObject &_poObj );

		// ReadProcessMemory().
		static BOOL WINAPI 				ReadProcessMemory( HANDLE _hProcess, LPCVOID _lpBaseAddress, LPVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesRead );

		// WriteProcessMemory().
		static BOOL WINAPI				WriteProcessMemory( HANDLE _hProcess, LPVOID _lpBaseAddress, LPCVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten );

		// EnumProcesses().
		static BOOL WINAPI				EnumProcesses( DWORD * _pdwProcessIds, DWORD _dwCb, DWORD * _pdwBytesReturned );

		// Gets the total number of processes EnumProcesses() should want to return.  Might be inaccurate if processes are quickly and constantly
		//	being opened and closed etc., so this returns a buffer larger than necessary.
		static DWORD					EnumProcessesBufferSize();

		// OpenProcess().
		static HANDLE WINAPI			OpenProcess( DWORD _dwDesiredAccess, BOOL _bInheritHandle, DWORD _dwProcessId );

		// OpenThread().
		static HANDLE WINAPI			OpenThread( DWORD _dwDesiredAccess, BOOL _bInheritHandle, DWORD _dwThreadId );

		// CreateToolhelp32Snapshot().
		static HANDLE WINAPI			CreateToolhelp32Snapshot( DWORD _dwFlags, DWORD _th32ProcessID );

		// Process32FirstW().
		static BOOL WINAPI				Process32FirstW( HANDLE _hSnapshot, LPPROCESSENTRY32W _lppe );

		// Process32NextW().
		static BOOL WINAPI				Process32NextW( HANDLE _hSnapshot, LPPROCESSENTRY32W _lppe );

		// Thread32First().
		static BOOL WINAPI				Thread32First( HANDLE _hSnapshot, LPTHREADENTRY32 _lpte );

		// Thread32Next().
		static BOOL WINAPI				Thread32Next( HANDLE _hSnapshot, LPTHREADENTRY32 _lpte );

		// Module32FirstW().
		static BOOL WINAPI				Module32FirstW( HANDLE _hSnapshot, LPMODULEENTRY32W _lpme );

		// Module32NextW().
		static BOOL WINAPI				Module32NextW( HANDLE _hSnapshot, LPMODULEENTRY32W _lpme );

		// QueryFullProcessImageNameW().
		static BOOL WINAPI				QueryFullProcessImageNameW( HANDLE _hProcess, DWORD _dwFlags, LPWSTR _lpExeName, PDWORD _lpdwSize );

		// Determines how large a buffer must be to accept the full path of a process when calling QueryFullProcessImageNameW().
		static DWORD					FullProcessPathLen( HANDLE _hProcess );

		// EnumThreadWindows().
		static BOOL WINAPI				EnumThreadWindows( DWORD _dwThreadId, WNDENUMPROC _lpfn, LPARAM _lParam );

		// Gets window text.
		static BOOL						GetWindowTextW( HWND _hWnd, std::wstring &_sRes );


	protected :
		// == Members.
		// Is the WoW64?
		static BOOL						m_bIsWow;

		// System information.
		static SYSTEM_INFO				m_siSystemInfo;

		// ReadProcessMemory.
		static LPFN_READPROCESSMEMORY	m_pfReadProcessMemory;

		// WriteProcessMemory.
		static LPFN_WRITEPROCESSMEMORY	m_pfWriteProcessMemory;

		// EnumProcesses().
		static LPFN_ENUMPROCESSES		m_pfEnumProcesses;

		// OpenProcess().
		static LPFN_OPENPROCESS			m_pfOpenProcess;

		// OpenThread().
		static LPFN_OPENTHREAD			m_pfOpenThread;

		// CreateToolhelp32Snapshot().
		static LPFN_CREATETOOLHELP32SNAPSHOT
										m_pfCreateToolhelp32Snapshot;

		// Process32FirstW().
		static LPFN_PROCESS32FIRSTW		m_pfProcess32FirstW;

		// Process32NextW().
		static LPFN_PROCESS32NEXTW		m_pfProcess32NextW;

		// Thread32First().
		static LPFN_THREAD32FIRST		m_pfThread32First;

		// Thread32Next().
		static LPFN_THREAD32NEXT		m_pfThread32Next;

		// Module32FirstW().
		static LPFN_MODULE32FIRSTW		m_pfModule32FirstW;

		// Module32NextW().
		static LPFN_MODULE32NEXTW		m_pfModule32NextW;

		// QueryFullProcessImageNameW().
		static LPFN_QUERYFULLPROCESSIMAGENAMEW
										m_pfQueryFullProcessImageNameW;

		// EnumThreadWindows().
		static LPFN_ENUMTHREADWINDOWS	m_pfEnumThreadWindows;


		// == Functions.
		// Load kernel32.dll functions.
		static VOID						LoadKernel32();

		// Load User32.dll functions.
		static VOID						LoadUser32();
	};

}	// namespace mx

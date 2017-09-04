#pragma once

#include "../Files/MXFile.h"
#include "../LSWin/LSWWin.h"
#include "../PE/MXPeObject.h"
#include <string>
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

		// OpenProcess().
		static HANDLE WINAPI			OpenProcess( DWORD _dwDesiredAccess, BOOL _bInheritHandle, DWORD _dwProcessId );

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


		// == Functions.
		// Load kernel32.dll functions.
		static VOID						LoadKernel32();
	};

}	// namespace mx

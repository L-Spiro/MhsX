#include "MXSystem.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <Helpers/LSWHelpers.h>
#ifdef _DEBUG
#include <cassert>
#endif	// #ifdef _DEBUG


namespace mx {

	// == Members.
	// Is the WoW64?
	BOOL CSystem::m_bIsWow = FALSE;

	// System information.
	SYSTEM_INFO CSystem::m_siSystemInfo = { 0 };

	// ReadProcessMemory().
	LPFN_READPROCESSMEMORY CSystem::m_pfReadProcessMemory = nullptr;

	// WriteProcessMemory().
	LPFN_WRITEPROCESSMEMORY	CSystem::m_pfWriteProcessMemory = nullptr;

	// VirtualProtectEx().
	LPFN_VIRTUALPROTECTEX CSystem::m_pfVirtualProtectEx = nullptr;

	// VirtualAllocEx().
	LPFN_VIRTUALALLOCEX CSystem::m_pfVirtualAllocEx = nullptr;

	// VirtualFreeEx().
	LPFN_VIRTUALFREEEX CSystem::m_pfVirtualFreeEx = nullptr;

	// VirtualQueryEx().
	LPFN_VIRTUALQUERYEX CSystem::m_pfVirtualQueryEx = nullptr;

	// EnumProcesses().
	LPFN_ENUMPROCESSES CSystem::m_pfEnumProcesses = nullptr;

	// OpenProcess().
	LPFN_OPENPROCESS CSystem::m_pfOpenProcess = nullptr;

	// OpenThread().
	LPFN_OPENTHREAD CSystem::m_pfOpenThread = nullptr;

	// IsWow64Process().
	LPFN_ISWOW64PROCESS CSystem::m_pfIsWow64Process = nullptr;

	// Process32FirstW().
	LPFN_PROCESS32FIRSTW CSystem::m_pfProcess32FirstW = nullptr;

	// Process32NextW().
	LPFN_PROCESS32NEXTW CSystem::m_pfProcess32NextW = nullptr;

	// Thread32First().
	LPFN_THREAD32FIRST CSystem::m_pfThread32First = nullptr;

	// Thread32Next().
	LPFN_THREAD32NEXT CSystem::m_pfThread32Next = nullptr;

	// Module32FirstW().
	LPFN_MODULE32FIRSTW CSystem::m_pfModule32FirstW = nullptr;

	// Module32NextW().
	LPFN_MODULE32NEXTW CSystem::m_pfModule32NextW = nullptr;

	// CreateToolhelp32Snapshot().
	LPFN_CREATETOOLHELP32SNAPSHOT CSystem::m_pfCreateToolhelp32Snapshot = nullptr;

	// QueryFullProcessImageNameW().
	LPFN_QUERYFULLPROCESSIMAGENAMEW CSystem::m_pfQueryFullProcessImageNameW = nullptr;

	// EnumThreadWindows().
	LPFN_ENUMTHREADWINDOWS CSystem::m_pfEnumThreadWindows = nullptr;

	// OpenProcessToken().
	LPFN_OPENPROCESSTOKEN CSystem::m_pfOpenProcessToken = nullptr;

	// LookupPrivilegeValue().
	LPFN_LOOKUPPRIVILEGEVALUEW CSystem::m_pfLookupPrivilegeValueW = nullptr;

	// AdjustTokenPrivileges().
	LPFN_ADJUSTTOKENPRIVILEGES CSystem::m_pfAdjustTokenPrivileges = nullptr;

	// GetExitCodeProcess().
	LPFN_GETEXITCODEPROCESS CSystem::m_pfGetExitCodeProcess = nullptr;

	// GetExitCodeThread().
	LPFN_GETEXITCODETHREAD CSystem::m_pfGetExitCodeThread = nullptr;

	// IsDebuggerPresent().
	LPFN_ISDEBUGGERPRESENT CSystem::m_pfIsDebuggerPresent = nullptr;

	// CheckRemoteDebuggerPresent().
	LPFN_CHECKREMOTEDEBUGGERPRESENT CSystem::m_pfCheckRemoteDebuggerPresent = nullptr;

	// DebugActiveProcess().
	LPFN_DEBUGACTIVEPROCESS CSystem::m_pfDebugActiveProcess = nullptr;

	// DebugActiveProcessStop().
	LPFN_DEBUGACTIVEPROCESSSTOP CSystem::m_pfDebugActiveProcessStop = nullptr;

	// ContinueDebugEvent().
	LPFN_CONTINUEDEBUGEVENT CSystem::m_pfContinueDebugEvent = nullptr;

	// DebugSetProcessKillOnExit().
	LPFN_DEBUGSETPROCESSKILLONEXIT CSystem::m_pfDebugSetProcessKillOnExit = nullptr;

	// WaitForDebugEvent().
	LPFN_WAITFORDEBUGEVENT CSystem::m_pfWaitForDebugEvent = nullptr;

	// WaitForDebugEventEx().
	LPFN_WAITFORDEBUGEVENTEX CSystem::m_pfWaitForDebugEventEx = nullptr;

	// GetThreadContext().
	LPFN_GETTHREADCONTEXT CSystem::m_pfGetThreadContext = nullptr;

	// SetThreadContext().
	LPFN_SETTHREADCONTEXT CSystem::m_pfSetThreadContext = nullptr;

	// Wow64GetThreadContext().
	LPFN_WOW64GETTHREADCONTEXT CSystem::m_pfWow64GetThreadContext = nullptr;

	// Wow64SetThreadContext().
	LPFN_WOW64SETTHREADCONTEXT CSystem::m_pfWow64SetThreadContext = nullptr;

	// Wow64GetThreadSelectorEntry().
	LPFN_WOW64GETTHREADSELECTORENTRY CSystem::m_pfWow64GetThreadSelectorEntry = nullptr;

	// == Types.
	typedef BOOL (WINAPI * LPFN_ISWOW64PROCESS)( HANDLE, PBOOL );
	typedef VOID (WINAPI * LPFN_GETSYSTEMINFO)( LPSYSTEM_INFO );

	// Initialize the system.
	VOID CSystem::InitSystem() {
		// Is this WoW64?
		{
			CHAR szKernel32[_LEN_6AE69F02+1];
			CHAR szIsWow64Process[_LEN_2E50340B+1];
			LPFN_ISWOW64PROCESS pfIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(::GetProcAddress( ::GetModuleHandleA(
				_DEC_6AE69F02_kernel32_dll( szKernel32 ) ),
				_DEC_2E50340B_IsWow64Process( szIsWow64Process ) ));
			if ( pfIsWow64Process ) {
				pfIsWow64Process( ::GetCurrentProcess(), &m_bIsWow );
			}
			// Don't leave encrypted strings on the stack.
			::ZeroMemory( szKernel32, sizeof( szKernel32 ) );
			::ZeroMemory( szIsWow64Process, sizeof( szIsWow64Process ) );
			pfIsWow64Process = nullptr;
		}
		
		
		// System information.
		{
			LPFN_GETSYSTEMINFO pfGetSystemInfo = nullptr;
			CHAR szKernel32[_LEN_6AE69F02+1];
			CHAR szGetSystemInfo[_T_MAX_LEN];
			if ( IsWow64Process() ) {				
				pfGetSystemInfo = reinterpret_cast<LPFN_GETSYSTEMINFO>(::GetProcAddress( ::GetModuleHandleA(
					_DEC_6AE69F02_kernel32_dll( szKernel32 ) ),
					_DEC_EB64C435_GetNativeSystemInfo( szGetSystemInfo ) ));
			}
			else {
				pfGetSystemInfo = reinterpret_cast<LPFN_GETSYSTEMINFO>(::GetProcAddress( ::GetModuleHandleA(
					_DEC_6AE69F02_kernel32_dll( szKernel32 ) ),
					_DEC_763FADF6_GetSystemInfo( szGetSystemInfo ) ));
			}
			// Don't leave encrypted strings on the stack.
			::ZeroMemory( szKernel32, sizeof( szKernel32 ) );
			::ZeroMemory( szGetSystemInfo, sizeof( szGetSystemInfo ) );
			if ( pfGetSystemInfo ) {
				pfGetSystemInfo( &m_siSystemInfo );
			}
			pfGetSystemInfo = nullptr;
		}


		// Kernel32 imports.
		LoadKernel32();

		// User32 imports.
		LoadUser32();

		// Advapi32 imports.
		LoadAdvapi32();

		// Set the current working directory.
		/*
		::SetCurrentDirectoryW( GetModulePathW( nullptr ).c_str() );
		*/
	}

	// Gets a function address by DLL name and function name.
	LPVOID CSystem::GetProcAddress( const WCHAR * _pwcDll, const CHAR * _pcFunc ) {
		return nullptr;
	}

	// Gets the file name from a given path.
	std::string CSystem::GetFileName( const std::string &_sPath ) {
		const char * pcLast = std::strrchr( _sPath.c_str(), '\\' );
		if ( !pcLast ) {
			pcLast = std::strrchr( _sPath.c_str(), '/' );
			if ( !pcLast ) {
				return _sPath;
			}
		}
		return ++pcLast;
	}

	// Gets the file name from a given path.
	std::wstring CSystem::GetFileName( const std::wstring &_wsPath ) {
		const wchar_t * pwcLast = std::wcsrchr( _wsPath.c_str(), L'\\' );
		if ( !pwcLast ) {
			pwcLast = std::wcsrchr( _wsPath.c_str(), L'/' );
			if ( !pwcLast ) {
				return _wsPath;
			}
		}
		return ++pwcLast;
	}

	// Gets the path to this .EXE in UTF-8.
	std::string CSystem::GetSelfPath() {
		return CUtilities::WStringToString( GetSelfPathW() );
	}

	// Gets the path to this .EXE in UTF-16.
	std::wstring CSystem::GetSelfPathW() {
		return GetModulePathW( nullptr );
	}

	// Gets the path to a given loaded DLL given its UTF-8 name.
	std::string CSystem::GetModulePath( const CHAR * _pcPath ) {
		return CUtilities::WStringToString( GetModulePathW( CUtilities::StringToWString( _pcPath ).c_str() ) );
	}

	// Gets the path to a given loaded DLL given its UTF-16 name.
	std::wstring CSystem::GetModulePathW( const WCHAR * _pwcPath ) {
		HMODULE hMod = _pwcPath ? ::GetModuleHandleW( _pwcPath ) : NULL;

		DWORD dwLen = GetModulePathLength( _pwcPath );
		WCHAR * pwcBuffer = new( std::nothrow ) WCHAR[dwLen];
		if ( !pwcBuffer ) { return std::wstring(); }
		dwLen = ::GetModuleFileNameW( hMod, pwcBuffer, dwLen );
		while ( dwLen > 1 && pwcBuffer[dwLen-1] != L'\\' ) {
			pwcBuffer[(dwLen--)-1] = L'\0';
		}
		std::wstring wTemp = pwcBuffer;
		::ZeroMemory( pwcBuffer, dwLen );
		delete [] pwcBuffer;
		return wTemp;
	}

	// Gets the length of the string needed to hold the path to this executable, including the terminating NULL.
	DWORD CSystem::GetSelfPathLength() {
		return GetModulePathLength( nullptr );
	}

	// Gets the length of the path of a loaded DLL, including the terminating NULL.
	DWORD CSystem::GetModulePathLength( const WCHAR * _pwcPath ) {
		HMODULE hMod = _pwcPath ? ::GetModuleHandleW( _pwcPath ) : NULL;
		for ( DWORD I = 1024; TRUE; I += 1024 ) {
			WCHAR * pwcBuffer = new( std::nothrow ) WCHAR[I];
			DWORD dwLen = ::GetModuleFileNameW( hMod, pwcBuffer, I );
			::ZeroMemory( pwcBuffer, dwLen );
			delete [] pwcBuffer;
			if ( dwLen < I ) {
				return dwLen + 1;
			}
		}
	}

	// Gets the current directory in UTF-8.
	std::string CSystem::GetCurDir() {
		return CUtilities::WStringToString( GetCurDirW() );
	}

	// Gets the current directory in UTF-16.
	std::wstring CSystem::GetCurDirW() {
		DWORD dwLen = ::GetCurrentDirectoryW( 0, NULL ) + 1;
		WCHAR * pwcBuffer = new( std::nothrow ) WCHAR[dwLen];
		if ( !pwcBuffer ) { return std::wstring(); }
		dwLen = ::GetCurrentDirectoryW( dwLen, pwcBuffer );
		std::wstring wTemp = pwcBuffer;
		::ZeroMemory( pwcBuffer, dwLen );
		delete [] pwcBuffer;
		return CUtilities::FinishPath( wTemp );
	}

	// Gets the system directory in UTF-8.
	std::string CSystem::GetSystemDir() {
		return CUtilities::WStringToString( GetSystemDirW() );
	}

	// Gets the system directory in UTF-16.
	std::wstring CSystem::GetSystemDirW() {
		DWORD dwLen = ::GetSystemDirectoryW( NULL, 0 ) + 1;
		WCHAR * pwcBuffer = new( std::nothrow ) WCHAR[dwLen];
		if ( !pwcBuffer ) { return std::wstring(); }
		dwLen = ::GetSystemDirectoryW( pwcBuffer, dwLen );
		std::wstring wTemp = pwcBuffer;
		::ZeroMemory( pwcBuffer, dwLen );
		delete [] pwcBuffer;
		return CUtilities::FinishPath( wTemp );
	}

	// Gets the Windows directory in UTF-8.
	std::string CSystem::GetWindowsDir() {
		return CUtilities::WStringToString( GetWindowsDirW() );
	}

	// Gets the Windows directory in UTF-16.
	std::wstring CSystem::GetWindowsDirW() {
		DWORD dwLen = ::GetWindowsDirectoryW( NULL, 0 ) + 1;
		WCHAR * pwcBuffer = new( std::nothrow ) WCHAR[dwLen];
		if ( !pwcBuffer ) { return std::wstring(); }
		dwLen = ::GetWindowsDirectoryW( pwcBuffer, dwLen );
		std::wstring wTemp = pwcBuffer;
		::ZeroMemory( pwcBuffer, dwLen );
		delete [] pwcBuffer;
		return CUtilities::FinishPath( wTemp );
	}

	// Gets all the paths in the PATH environment variable.
	size_t CSystem::GetPathEnv( std::vector<std::string> &_vReturn ) {
		const DWORD dwSize = 32767;
		WCHAR * pwcBuffer = new( std::nothrow ) WCHAR[dwSize];

		WCHAR szPath[_LEN_3DC166BB+1];
		DWORD dwLen = GetEnvironmentVariableW( _DEC_3DC166BB_PATH( szPath ), pwcBuffer, dwSize );
		std::wstring wTemp;
		size_t sStart = _vReturn.size();
		for ( size_t I = 0; I < dwLen; ++I ) {
			if ( pwcBuffer[I] == L';' ) {
				if ( wTemp.size() ) {
					_vReturn.push_back( CUtilities::WStringToString( CUtilities::FinishPath( wTemp ) ) );
					wTemp.clear();
				}
			}
			else {
				wTemp.push_back( pwcBuffer[I] );
			}
		}
		::ZeroMemory( pwcBuffer, dwLen );
		delete [] pwcBuffer;

		return _vReturn.size() - sStart;
	}

	// Gets all the paths in the PATH environment variable.
	size_t CSystem::GetPathEnv( std::vector<std::wstring> &_vReturn ) {
		const DWORD dwSize = 32767;
		WCHAR * pwcBuffer = new( std::nothrow ) WCHAR[dwSize];

		WCHAR szPath[_LEN_3DC166BB+1];
		DWORD dwLen = GetEnvironmentVariableW( _DEC_3DC166BB_PATH( szPath ), pwcBuffer, dwSize );
		std::wstring wTemp;
		size_t sStart = _vReturn.size();
		for ( size_t I = 0; I < dwLen; ++I ) {
			if ( pwcBuffer[I] == L';' ) {
				if ( wTemp.size() ) {
					_vReturn.push_back( CUtilities::FinishPath( wTemp ) );
					wTemp.clear();
				}
			}
			else {
				wTemp.push_back( pwcBuffer[I] );
			}
		}
		::ZeroMemory( pwcBuffer, dwLen );
		delete [] pwcBuffer;

		return _vReturn.size() - sStart;
	}

	// Gets all of the search paths for a DLL in the order in which they should be searched.
	std::vector<std::string> CSystem::DllSearchPaths( const WCHAR * _pwcDll, std::vector<std::string> &_vResults, BOOL _bIncludeLoadedModulePath ) {
		std::string sDllUtf8 = CUtilities::WStringToString( _pwcDll );
		if ( _bIncludeLoadedModulePath ) {
			// If the module is already loaded by us, put it in front of the list.
			std::string sLoadedDir = GetModulePath( sDllUtf8.c_str() );
			if ( sLoadedDir.size() ) {
				_vResults.push_back( sLoadedDir );
			}
		}
		_vResults.push_back( GetSelfPath() );
		_vResults.push_back( GetCurDir() );
		_vResults.push_back( GetSystemDir() );
		_vResults.push_back( GetWindowsDir() );
		GetPathEnv( _vResults );
		for ( size_t I = 0; I < _vResults.size(); ++I ) {
			_vResults[I].append( sDllUtf8 );
		}
		return _vResults;
	}

	// Gets all of the search paths for a DLL in the order in which they should be searched.
	std::vector<std::wstring> CSystem::DllSearchPaths( const WCHAR * _pwcDll, std::vector<std::wstring> &_vResults, BOOL _bIncludeLoadedModulePath ) {
		std::wstring sDllUtf8 = _pwcDll;
		if ( _bIncludeLoadedModulePath ) {
			// If the module is already loaded by us, put it in front of the list.
			std::wstring sLoadedDir = GetModulePathW( sDllUtf8.c_str() );
			if ( sLoadedDir.size() ) {
				_vResults.push_back( sLoadedDir );
			}
		}
		_vResults.push_back( GetSelfPathW() );
		_vResults.push_back( GetCurDirW() );
		_vResults.push_back( GetSystemDirW() );
		_vResults.push_back( GetWindowsDirW() );
		GetPathEnv( _vResults );
		for ( size_t I = 0; I < _vResults.size(); ++I ) {
			_vResults[I].append( sDllUtf8 );
		}
		return _vResults;
	}

	// Loads a DLL by name.  Uses the normal search paths DLL's use (local directory, working directory, system directory, windows directory, PATH directories).
	BOOL CSystem::FindDll( const CHAR * _pcDll, CFile &_fReturn ) {
		return FindDll( CUtilities::StringToWString( _pcDll ).c_str(), _fReturn );
	}

	// Loads a DLL by name.  Uses the normal search paths DLL's use (local directory, working directory, system directory, windows directory, PATH directories).
	BOOL CSystem::FindDll( const WCHAR * _pwDll, CFile &_fReturn ) {
		std::vector<std::wstring> vTemp;
		DllSearchPaths( _pwDll, vTemp );

		for ( size_t I = 0; I < vTemp.size(); ++I ) {
			if ( _fReturn.OpenFile( vTemp[I].c_str(), TRUE ) ) { return TRUE; }
		}
		return FALSE;
	}

	// Gets the address of a process.  Slow because it loads the DLL from memory, parses it, then discards it.
	LPCVOID CSystem::GetProcAddress( LPCSTR _lpcModule, LPCSTR  _lpcProcName ) {
		CPeObject poObj;
		CFile fFile;
		if ( !FindDll( _lpcModule, fFile ) ) { return nullptr; }
		if ( !poObj.LoadImageFromMemory( fFile ) ) { return nullptr; }
		return GetProcAddress( _lpcModule, _lpcProcName, poObj );
	}

	// Faster way to get the address of a function.
	LPCVOID CSystem::GetProcAddress( LPCSTR _lpcModule, LPCSTR  _lpcProcName, const CPeObject &_poObj ) {
		HMODULE hMod = ::GetModuleHandleA( _lpcModule );
		if ( !hMod ) { return nullptr; }
		
		uint64_t ui64Temp = _poObj.GetExportAddress( _lpcProcName );
		if ( !ui64Temp ) { return nullptr; }
		ui64Temp += reinterpret_cast<uint64_t>(hMod);
		return reinterpret_cast<LPCVOID>(ui64Temp);
	}

	// Faster way to get the address of a function, using encrypted strings.
	LPCVOID CSystem::GetProcAddress( LPCSTR _lpcModule, size_t _sModuleLen, LPCSTR _lpcProcName, size_t _sProcLen, const CPeObject &_poObj ) {
		std::string sModule, sProc;

		CStringDecoder::Decode( _lpcModule, _sModuleLen, sModule );
		CStringDecoder::Decode( _lpcProcName, _sProcLen, sProc );

		LPCVOID pvRet = GetProcAddress( sModule.c_str(), sProc.c_str(), _poObj );
		// Security: Wipe the decoded values from memory.
		::ZeroMemory( const_cast<std::string::value_type *>(sModule.c_str()), sModule.size() );
		::ZeroMemory( const_cast<std::string::value_type *>(sProc.c_str()), sProc.size() );

		return pvRet;
	}

	// ReadProcessMemory().
	BOOL WINAPI CSystem::ReadProcessMemory( HANDLE _hProcess, LPCVOID _lpBaseAddress, LPVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesRead ) {
		return m_pfReadProcessMemory ? m_pfReadProcessMemory( _hProcess, _lpBaseAddress, _lpBuffer, _nSize, _lpNumberOfBytesRead ) : FALSE;
	}

	// WriteProcessMemory().
	BOOL WINAPI CSystem::WriteProcessMemory( HANDLE _hProcess, LPVOID _lpBaseAddress, LPCVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten ) {
		return m_pfWriteProcessMemory ? m_pfWriteProcessMemory( _hProcess, _lpBaseAddress, _lpBuffer, _nSize, _lpNumberOfBytesWritten ) : FALSE;
	}

	// VirtualProtectEx().
	BOOL WINAPI CSystem::VirtualProtectEx( HANDLE _hProcess, LPVOID _lpAddress, SIZE_T _dwSize, DWORD _flNewProtect, PDWORD _lpflOldProtect ) {
		return m_pfVirtualProtectEx ? m_pfVirtualProtectEx( _hProcess, _lpAddress, _dwSize, _flNewProtect, _lpflOldProtect ) : FALSE;
	}

	// VirtualAllocEx();
	LPVOID WINAPI CSystem::VirtualAllocEx( HANDLE _hProcess, LPVOID _lpAddress, SIZE_T _dwSize, DWORD _flAllocationType, DWORD _flProtect ) {
		return m_pfVirtualAllocEx ? m_pfVirtualAllocEx( _hProcess, _lpAddress, _dwSize, _flAllocationType, _flProtect ) : NULL;
	}

	// VirtualFreeEx().
	BOOL WINAPI CSystem::VirtualFreeEx( HANDLE _hProcess, LPVOID _lpAddress, SIZE_T _dwSize, DWORD _dwFreeType ) {
		return m_pfVirtualFreeEx ? m_pfVirtualFreeEx( _hProcess, _lpAddress, _dwSize, _dwFreeType ) : FALSE;
	}

	// VirtualQueryEx();
	SIZE_T WINAPI CSystem::VirtualQueryEx( HANDLE _hProcess, LPCVOID _lpAddress, PMEMORY_BASIC_INFORMATION _lpBuffer, SIZE_T _dwLength ) {
		return m_pfVirtualQueryEx ? m_pfVirtualQueryEx( _hProcess, _lpAddress, _lpBuffer, _dwLength ) : 0;
	}

	// EnumProcesses().
	BOOL WINAPI CSystem::EnumProcesses( DWORD * _pdwProcessIds, DWORD _dwCb, DWORD * _pdwBytesReturned ) {
		return m_pfEnumProcesses ? m_pfEnumProcesses( _pdwProcessIds, _dwCb, _pdwBytesReturned ) : FALSE;
	}

	// Gets the total number of processes EnumProcesses() should want to return.  Might be inaccurate if processes are quickly and constantly
	//	being opened and closed etc., so this returns a buffer larger than necessary.
	DWORD CSystem::EnumProcessesBufferSize() {
		std::vector<DWORD> vIds;
		DWORD dwSize = 0;
		while ( TRUE ) {
			dwSize += 1024;
			vIds.resize( dwSize );
			DWORD dwRet;
			if ( !CSystem::EnumProcesses( &vIds[0], vIds.size() * sizeof( DWORD ), &dwRet ) ) {
				return vIds.size() + 20;
			}
			if ( dwRet < vIds.size() * sizeof( DWORD ) ) { return dwRet / sizeof( DWORD ) + 20; }
		}
	}

	// OpenProcess().
	HANDLE WINAPI CSystem::OpenProcess( DWORD _dwDesiredAccess, BOOL _bInheritHandle, DWORD _dwProcessId ) {
		return m_pfOpenProcess ? m_pfOpenProcess( _dwDesiredAccess, _bInheritHandle, _dwProcessId ) : NULL;
	}

	// OpenThread().
	HANDLE WINAPI CSystem::OpenThread( DWORD _dwDesiredAccess, BOOL _bInheritHandle, DWORD _dwThreadId ) {
		return m_pfOpenThread ? m_pfOpenThread( _dwDesiredAccess, _bInheritHandle, _dwThreadId ) : NULL;
	}

	// IsWow64Process().
	BOOL WINAPI CSystem::IsWow64Process( HANDLE _hProcess, PBOOL _Wow64Process ) {
		return m_pfIsWow64Process ? m_pfIsWow64Process( _hProcess, _Wow64Process ) : FALSE;
	}

	// CreateToolhelp32Snapshot().
	HANDLE WINAPI CSystem::CreateToolhelp32Snapshot( DWORD _dwFlags, DWORD _th32ProcessID ) {
		return m_pfCreateToolhelp32Snapshot ? m_pfCreateToolhelp32Snapshot( _dwFlags, _th32ProcessID ) : NULL;
	}

	// Process32FirstW().
	BOOL WINAPI CSystem::Process32FirstW( HANDLE _hSnapshot, LPPROCESSENTRY32W _lppe ) {
		return m_pfProcess32FirstW ? m_pfProcess32FirstW( _hSnapshot, _lppe ) : FALSE;
	}

	// Process32NextW().
	BOOL WINAPI CSystem::Process32NextW( HANDLE _hSnapshot, LPPROCESSENTRY32W _lppe ) {
		return m_pfProcess32NextW ? m_pfProcess32NextW( _hSnapshot, _lppe ) : FALSE;
	}

	// Thread32First().
	BOOL WINAPI CSystem::Thread32First( HANDLE _hSnapshot, LPTHREADENTRY32 _lpte ) {
		return m_pfThread32First ? m_pfThread32First( _hSnapshot, _lpte ) : FALSE;
	}

	// Thread32Next().
	BOOL WINAPI CSystem::Thread32Next( HANDLE _hSnapshot, LPTHREADENTRY32 _lpte ) {
		return m_pfThread32Next ? m_pfThread32Next( _hSnapshot, _lpte ) : FALSE;
	}

	// Module32FirstW().
	BOOL WINAPI CSystem::Module32FirstW( HANDLE _hSnapshot, LPMODULEENTRY32W _lpme ) {
		return m_pfModule32FirstW ? m_pfModule32FirstW( _hSnapshot, _lpme ) : FALSE;
	}

	// Module32NextW().
	BOOL WINAPI CSystem::Module32NextW( HANDLE _hSnapshot, LPMODULEENTRY32W _lpme ) {
		return m_pfModule32NextW ? m_pfModule32NextW( _hSnapshot, _lpme ) : FALSE;
	}

	// QueryFullProcessImageNameW().
	BOOL WINAPI CSystem::QueryFullProcessImageNameW( HANDLE _hProcess, DWORD _dwFlags, LPWSTR _lpExeName, PDWORD _lpdwSize ) {
		return m_pfQueryFullProcessImageNameW ? m_pfQueryFullProcessImageNameW( _hProcess, _dwFlags, _lpExeName, _lpdwSize ) : FALSE;
	}

	// GetExitCodeProcess().
	BOOL WINAPI CSystem::GetExitCodeProcess( HANDLE _hProcess, LPDWORD _lpExitCode ) {
		return m_pfGetExitCodeProcess ? m_pfGetExitCodeProcess( _hProcess, _lpExitCode ) : FALSE;
	}

	// GetExitCodeThread().
	BOOL WINAPI CSystem::GetExitCodeThread( HANDLE _hThread, LPDWORD _lpExitCode ) {
		return m_pfGetExitCodeThread ? m_pfGetExitCodeThread( _hThread, _lpExitCode ) : FALSE;
	}

	// Determines how large a buffer must be to accept the full path of a process when calling QueryFullProcessImageNameW().
	DWORD CSystem::FullProcessPathLen( HANDLE _hProcess ) {
		std::vector<WCHAR> vBuffer;
		DWORD dwLen = MAX_PATH;
		DWORD dwTemp = dwLen;
		do {
			dwLen += MAX_PATH;
			vBuffer.resize( dwLen );
			dwTemp = dwLen;
			if ( QueryFullProcessImageNameW( _hProcess, 0, &vBuffer[0], &dwTemp ) ) {
				return dwTemp + 1;
			}
		} while ( dwTemp == dwLen && dwLen < 0xFFFF );
		return dwTemp + 1;
	}

	// EnumThreadWindows().
	BOOL WINAPI CSystem::EnumThreadWindows( DWORD _dwThreadId, WNDENUMPROC _lpfn, LPARAM _lParam ) {
		return m_pfEnumThreadWindows ? m_pfEnumThreadWindows( _dwThreadId, _lpfn, _lParam ) : FALSE;
	}

	// Gets window text.
	BOOL CSystem::GetWindowTextW( HWND _hWnd, std::wstring &_sRes ) {
		size_t sLen = ::GetWindowTextLengthW( _hWnd ) + 4;
		WCHAR * pwcBuffer = new WCHAR[sLen];
		if ( !pwcBuffer ) { return FALSE; }
		std::memset( pwcBuffer, 0, sLen );
		::GetWindowTextW( _hWnd, pwcBuffer, sLen );
		_sRes = pwcBuffer;
		delete [] pwcBuffer;
		return TRUE;
	}

	// OpenProcessToken().
	BOOL WINAPI CSystem::OpenProcessToken( HANDLE _ProcessHandle, DWORD _DesiredAccess, PHANDLE _TokenHandle ) {
		return m_pfOpenProcessToken ? m_pfOpenProcessToken( _ProcessHandle, _DesiredAccess, _TokenHandle ) : FALSE;
	}

	// LookupPrivilegeValueW().
	BOOL WINAPI CSystem::LookupPrivilegeValueW( LPCWSTR _lpSystemName, LPCWSTR _lpName, PLUID _lpLuid ) {
		return m_pfLookupPrivilegeValueW ? m_pfLookupPrivilegeValueW( _lpSystemName, _lpName, _lpLuid ) : FALSE;
	}

	// AdjustTokenPrivileges().
	BOOL WINAPI CSystem::AdjustTokenPrivileges( HANDLE _TokenHandle, BOOL _DisableAllPrivileges, PTOKEN_PRIVILEGES _NewState, DWORD _BufferLength, PTOKEN_PRIVILEGES _PreviousState, PDWORD _ReturnLength ) {
		return m_pfAdjustTokenPrivileges ? m_pfAdjustTokenPrivileges( _TokenHandle, _DisableAllPrivileges, _NewState, _BufferLength, _PreviousState, _ReturnLength ) : FALSE;
	}

	// Show an encrypted error message box.
	VOID CSystem::MessageBoxError( HWND _hWnd, const CHAR * _pcMsg, size_t _sMsgLen, const CHAR * _pcTitle, size_t _sTitleLen ) {
		std::string sMsg = mx::CStringDecoder::DecodeToString( _pcMsg, _sMsgLen );
		std::string sTitle = mx::CStringDecoder::DecodeToString( _pcTitle, _sTitleLen );
		lsw::CBase::MessageBoxError( _hWnd, sMsg.c_str(), sTitle.c_str() );
		std::memset( const_cast<char *>(sMsg.c_str()), 0, sMsg.size() );
		std::memset( const_cast<char *>(sTitle.c_str()), 0, sTitle.size() );
	}

	// Tests the flags that can be used to open a given process.
	DWORD CSystem::TestOpenProcess( DWORD _dwId ) {
		static const DWORD dwFlags[] = {
			PROCESS_TERMINATE,
			PROCESS_CREATE_THREAD,
			PROCESS_SET_SESSIONID,
			PROCESS_VM_OPERATION,
			PROCESS_VM_READ,
			PROCESS_VM_WRITE,
			PROCESS_DUP_HANDLE,
			PROCESS_CREATE_PROCESS,
			PROCESS_SET_QUOTA,
			PROCESS_SET_INFORMATION,
			PROCESS_QUERY_INFORMATION,
			PROCESS_SUSPEND_RESUME,
			PROCESS_QUERY_LIMITED_INFORMATION,
			PROCESS_SET_LIMITED_INFORMATION,
			STANDARD_RIGHTS_REQUIRED,
			SYNCHRONIZE,
		};
		DWORD dwRet = 0;
		for ( size_t I = 0; I < MX_ELEMENTS( dwFlags ); ++I ) {
			lsw::LSW_HANDLE hHandle = CSystem::OpenProcess( dwFlags[I], FALSE, _dwId );
			if ( hHandle.Valid() ) {
				dwRet |= dwFlags[I];
			}
		}
		return dwRet;
	}

	// IsDebuggerPresent().
	BOOL WINAPI CSystem::IsDebuggerPresent() {
		return m_pfIsDebuggerPresent ? m_pfIsDebuggerPresent() : FALSE;
	}

	// CheckRemoteDebuggerPresent().
	BOOL WINAPI CSystem::CheckRemoteDebuggerPresent( HANDLE _hProcess, PBOOL _pbDebuggerPresent ) {
		return m_pfCheckRemoteDebuggerPresent ? m_pfCheckRemoteDebuggerPresent( _hProcess, _pbDebuggerPresent ) : FALSE;
	}

	// DebugActiveProcess().
	BOOL WINAPI CSystem::DebugActiveProcess( DWORD _dwProcessId ) {
		return m_pfDebugActiveProcess ? m_pfDebugActiveProcess( _dwProcessId ) : FALSE;
	}

	// DebugActiveProcessStop().
	BOOL WINAPI CSystem::DebugActiveProcessStop( DWORD _dwProcessId ) {
		return m_pfDebugActiveProcessStop ? m_pfDebugActiveProcessStop( _dwProcessId ) : FALSE;
	}

	// ContinueDebugEvent().
	BOOL WINAPI CSystem::ContinueDebugEvent( DWORD _dwProcessId, DWORD _dwThreadId, DWORD _dwContinueStatus ) {
		return m_pfContinueDebugEvent ? m_pfContinueDebugEvent( _dwProcessId, _dwThreadId, _dwContinueStatus ) : FALSE;
	}

	// DebugSetProcessKillOnExit().
	BOOL WINAPI CSystem::DebugSetProcessKillOnExit( BOOL _KillOnExit ) {
		return m_pfDebugSetProcessKillOnExit ? m_pfDebugSetProcessKillOnExit( _KillOnExit ) : FALSE;
	}

	// WaitForDebugEvent().
	BOOL WINAPI CSystem::WaitForDebugEvent( LPDEBUG_EVENT _lpDebugEvent, DWORD _dwMilliseconds ) {
		if ( m_pfWaitForDebugEventEx ) {
			return m_pfWaitForDebugEventEx( _lpDebugEvent, _dwMilliseconds );
		}
		return m_pfWaitForDebugEvent ? m_pfWaitForDebugEvent( _lpDebugEvent, _dwMilliseconds ) : FALSE;
	}

	// GetThreadContext().
	BOOL WINAPI CSystem::GetThreadContext( HANDLE _hThread, LPCONTEXT _lpContext ) {
		return m_pfGetThreadContext ? m_pfGetThreadContext( _hThread, _lpContext ) : FALSE;
	}

	// SetThreadContext().
	BOOL WINAPI CSystem::SetThreadContext( HANDLE _hThread, const CONTEXT * _lpContext ) {
		return m_pfSetThreadContext ? m_pfSetThreadContext( _hThread, _lpContext ) : FALSE;
	}

	// Wow64GetThreadContext().
	BOOL WINAPI CSystem::Wow64GetThreadContext( HANDLE _hThread, PWOW64_CONTEXT _lpContext ) {
		return m_pfWow64GetThreadContext ? m_pfWow64GetThreadContext( _hThread, _lpContext ) : FALSE;
	}

	// Wow64SetThreadContext().
	BOOL WINAPI CSystem::Wow64SetThreadContext( HANDLE _hThread, const WOW64_CONTEXT * _lpContext ) {
		return m_pfWow64SetThreadContext ? m_pfWow64SetThreadContext( _hThread, _lpContext ) : FALSE;
	}

	// Wow64GetThreadSelectorEntry().
	BOOL WINAPI CSystem::Wow64GetThreadSelectorEntry( HANDLE _hThread, DWORD _dwSelector, PWOW64_LDT_ENTRY _lpSelectorEntry ) {
		return m_pfWow64GetThreadSelectorEntry ? m_pfWow64GetThreadSelectorEntry( _hThread, _dwSelector, _lpSelectorEntry ) : FALSE;
	}

	// Load kernel32.dll functions.
	VOID CSystem::LoadKernel32() {
		CHAR szKernel32[_LEN_6AE69F02+1];
		_DEC_6AE69F02_kernel32_dll( szKernel32 );
		CPeObject poObj;
		CFile fFile;
		if ( !FindDll( szKernel32, fFile ) ) {
			::ZeroMemory( szKernel32, MX_ELEMENTS( szKernel32 ) );
			return;
		}
		if ( !poObj.LoadImageFromMemory( fFile ) ) {
			::ZeroMemory( szKernel32, MX_ELEMENTS( szKernel32 ) );
			return;
		}


#ifdef _DEBUG
	LPVOID pfTemp;
	HMODULE hDll = ::GetModuleHandleA( szKernel32 );
#define MX_CHECK( NAME )																						\
	pfTemp = ::GetProcAddress( hDll, #NAME );																	\
	assert( pfTemp == m_pf ## NAME )
#else
#define MX_CHECK( NAME )
#endif // #ifdef _DEBUG

#define MX_PROCADDR( NAME, ENCNAME, ENCLEN )			{ LPVOID & pvRef = reinterpret_cast<LPVOID &>(m_pf ## NAME);													\
														pvRef = const_cast<LPVOID>(GetProcAddress( _T_6AE69F02_kernel32_dll, _LEN_6AE69F02, ENCNAME, ENCLEN, poObj ));	\
														MX_CHECK( NAME ); }

		MX_PROCADDR( ReadProcessMemory, _T_F7C7AE42_ReadProcessMemory, _LEN_F7C7AE42 );
		MX_PROCADDR( WriteProcessMemory, _T_4F58972E_WriteProcessMemory, _LEN_4F58972E );
		MX_PROCADDR( VirtualProtectEx, _T_5D180413_VirtualProtectEx, _LEN_5D180413 );
		MX_PROCADDR( VirtualAllocEx, _T_E62E824D_VirtualAllocEx, _LEN_E62E824D );
		MX_PROCADDR( VirtualFreeEx, _T_6B482023_VirtualFreeEx, _LEN_6B482023 );
		MX_PROCADDR( VirtualQueryEx, _T_1BFB8645_VirtualQueryEx, _LEN_1BFB8645 );

		m_pfEnumProcesses = reinterpret_cast<LPFN_ENUMPROCESSES>(GetProcAddress( _T_6AE69F02_kernel32_dll, _LEN_6AE69F02, _T_0509A21C_EnumProcesses, _LEN_0509A21C, poObj ));
		if ( !m_pfEnumProcesses ) {
			m_pfEnumProcesses = reinterpret_cast<LPFN_ENUMPROCESSES>(GetProcAddress( _T_6AE69F02_kernel32_dll, _LEN_6AE69F02, _T_0501393D_K32EnumProcesses, _LEN_0501393D, poObj ));
		}
		MX_PROCADDR( OpenProcess, _T_DF27514B_OpenProcess, _LEN_DF27514B );
		MX_PROCADDR( OpenThread, _T_B85486FF_OpenThread, _LEN_B85486FF );
		MX_PROCADDR( IsWow64Process, _T_2E50340B_IsWow64Process, _LEN_2E50340B );
		MX_PROCADDR( CreateToolhelp32Snapshot, _T_C1F3B876_CreateToolhelp32Snapshot, _LEN_C1F3B876 );
		MX_PROCADDR( Process32FirstW, _T_8197004C_Process32FirstW, _LEN_8197004C );
		MX_PROCADDR( Process32NextW, _T_BC6B67BF_Process32NextW, _LEN_BC6B67BF );
		MX_PROCADDR( Thread32First, _T_238B3114_Thread32First, _LEN_238B3114 );
		MX_PROCADDR( Thread32Next, _T_F5197707_Thread32Next, _LEN_F5197707 );
		MX_PROCADDR( Module32FirstW, _T_2735A2C6_Module32FirstW, _LEN_2735A2C6 );
		MX_PROCADDR( Module32NextW, _T_A29E8A1A_Module32NextW, _LEN_A29E8A1A );
		MX_PROCADDR( QueryFullProcessImageNameW, _T_FC3DC91C_QueryFullProcessImageNameW, _LEN_FC3DC91C );
		MX_PROCADDR( GetExitCodeProcess, _T_1CCA53FD_GetExitCodeProcess, _LEN_1CCA53FD );
		MX_PROCADDR( GetExitCodeThread, _T_59D89102_GetExitCodeThread, _LEN_59D89102 );
		MX_PROCADDR( IsDebuggerPresent, _T_8436F795_IsDebuggerPresent, _LEN_8436F795 );
		MX_PROCADDR( CheckRemoteDebuggerPresent, _T_FC1FDA83_CheckRemoteDebuggerPresent, _LEN_FC1FDA83 );
		MX_PROCADDR( DebugActiveProcess, _T_A0FF286A_DebugActiveProcess, _LEN_A0FF286A );
		MX_PROCADDR( DebugActiveProcessStop, _T_3E53E72D_DebugActiveProcessStop, _LEN_3E53E72D );
		MX_PROCADDR( ContinueDebugEvent, _T_D8E77E49_ContinueDebugEvent, _LEN_D8E77E49 );
		MX_PROCADDR( DebugSetProcessKillOnExit, _T_7757830E_DebugSetProcessKillOnExit, _LEN_7757830E );
		MX_PROCADDR( WaitForDebugEvent, _T_96AB83A1_WaitForDebugEvent, _LEN_96AB83A1 );
		MX_PROCADDR( WaitForDebugEventEx, _T_51A49928_WaitForDebugEventEx, _LEN_51A49928 );
		MX_PROCADDR( GetThreadContext, _T_649EB9C1_GetThreadContext, _LEN_649EB9C1 );
		MX_PROCADDR( SetThreadContext, _T_5688CBD8_SetThreadContext, _LEN_5688CBD8 );
		MX_PROCADDR( Wow64GetThreadContext, _T_49CB90FE_Wow64GetThreadContext, _LEN_49CB90FE );
		MX_PROCADDR( Wow64SetThreadContext, _T_7BDDE2E7_Wow64SetThreadContext, _LEN_7BDDE2E7 );
		MX_PROCADDR( Wow64GetThreadSelectorEntry, _T_742F716C_Wow64GetThreadSelectorEntry, _LEN_742F716C );

#ifdef _DEBUG
		pfTemp = ::GetProcAddress( hDll, "EnumProcesses" );
		if ( !pfTemp ) {
			pfTemp = ::GetProcAddress( hDll, "K32EnumProcesses" );
		}
		assert( pfTemp == m_pfEnumProcesses );
#endif	// #ifdef _DEBUG

		::ZeroMemory( szKernel32, MX_ELEMENTS( szKernel32 ) );
#undef MX_CHECK
#undef MX_PROCADDR
	}

	// Load User32.dll functions.
	VOID CSystem::LoadUser32() {
		CHAR szUser32[_LEN_02489AAB+1];
		_DEC_02489AAB_user32_dll( szUser32 );
		CPeObject poObj;
		CFile fFile;
		if ( !FindDll( szUser32, fFile ) ) {
			::ZeroMemory( szUser32, MX_ELEMENTS( szUser32 ) );
			return;
		}
		if ( !poObj.LoadImageFromMemory( fFile ) ) {
			::ZeroMemory( szUser32, MX_ELEMENTS( szUser32 ) );
			return;
		}

#ifdef _DEBUG
	LPVOID pfTemp;
	HMODULE hDll = ::GetModuleHandleA( szUser32 );
#define MX_CHECK( NAME )																						\
	pfTemp = ::GetProcAddress( hDll, #NAME );																	\
	assert( pfTemp == m_pf ## NAME )
#else
#define MX_CHECK( NAME )
#endif // #ifdef _DEBUG


#define MX_PROCADDR( NAME, ENCNAME, ENCLEN )			{ LPVOID & pvRef = reinterpret_cast<LPVOID &>(m_pf ## NAME);													\
														pvRef = const_cast<LPVOID>(GetProcAddress( _T_02489AAB_user32_dll, _LEN_02489AAB, ENCNAME, ENCLEN, poObj ));	\
														MX_CHECK( NAME ); }

		MX_PROCADDR( EnumThreadWindows, _T_AF5AA374_EnumThreadWindows, _LEN_AF5AA374 );

		::ZeroMemory( szUser32, MX_ELEMENTS( szUser32 ) );
#undef MX_CHECK
#undef MX_PROCADDR
	}

	// Load Advapi32.dll functions.
	VOID CSystem::LoadAdvapi32() {
		CHAR szAdvapi32[_LEN_F16ED7E0+1];
		_DEC_F16ED7E0_advapi32_dll( szAdvapi32 );
		CPeObject poObj;
		CFile fFile;
		if ( !FindDll( szAdvapi32, fFile ) ) {
			::ZeroMemory( szAdvapi32, MX_ELEMENTS( szAdvapi32 ) );
			return;
		}
		if ( !poObj.LoadImageFromMemory( fFile ) ) {
			::ZeroMemory( szAdvapi32, MX_ELEMENTS( szAdvapi32 ) );
			return;
		}

#ifdef _DEBUG
	LPVOID pfTemp;
	HMODULE hDll = ::GetModuleHandleA( szAdvapi32 );
#define MX_CHECK( NAME )																						\
	pfTemp = ::GetProcAddress( hDll, #NAME );																	\
	assert( pfTemp == m_pf ## NAME )
#else
#define MX_CHECK( NAME )
#endif // #ifdef _DEBUG


#define MX_PROCADDR( NAME, ENCNAME, ENCLEN )			{ LPVOID & pvRef = reinterpret_cast<LPVOID &>(m_pf ## NAME);													\
														pvRef = const_cast<LPVOID>(GetProcAddress( _T_F16ED7E0_advapi32_dll, _LEN_F16ED7E0, ENCNAME, ENCLEN, poObj ));	\
														MX_CHECK( NAME ); }

		MX_PROCADDR( OpenProcessToken, _T_F9C60615_OpenProcessToken, _LEN_F9C60615 );
		MX_PROCADDR( LookupPrivilegeValueW, _T_2E530A33_LookupPrivilegeValueW, _LEN_2E530A33 );
		MX_PROCADDR( AdjustTokenPrivileges, _T_0DE3E5CF_AdjustTokenPrivileges, _LEN_0DE3E5CF );

		::ZeroMemory( szAdvapi32, MX_ELEMENTS( szAdvapi32 ) );
#undef MX_CHECK
#undef MX_PROCADDR
	}

}	// namespace mx

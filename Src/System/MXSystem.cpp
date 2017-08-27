#include "MXSystem.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#ifdef _DEBUG
#include <cassert>
#endif	// #ifdef _DEBUG


namespace mx {

	// == Members.
	// Is the WoW64?
	BOOL CSystem::m_bIsWow = FALSE;

	// System information.
	SYSTEM_INFO CSystem::m_siSystemInfo = { 0 };

	// ReadProcessMemory.
	LPFN_READPROCESSMEMORY CSystem::m_pfReadProcessMemory = nullptr;

	// WriteProcessMemory.
	LPFN_WRITEPROCESSMEMORY	CSystem::m_pfWriteProcessMemory = nullptr;

	// EnumProcesses().
	LPFN_ENUMPROCESSES CSystem::m_pfEnumProcesses = nullptr;

	// OpenProcess().
	LPFN_OPENPROCESS CSystem::m_pfOpenProcess = nullptr;

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
	}

	// Gets a function address by DLL name and function name.
	LPVOID CSystem::GetProcAddress( const WCHAR * _pwcDll, const CHAR * _pcFunc ) {
		return nullptr;
	}

	// Gets the path to this .EXE in UTF-8.
	std::string CSystem::GetSelfPath() {
		return CUtilities::WStringToString( GetSelfPathW() );
	}

	// Gets the path to this .EXE in UTF-16.
	std::wstring CSystem::GetSelfPathW() {
		DWORD dwLen = GetSelfPathLength();
		WCHAR * pwcBuffer = new( std::nothrow ) WCHAR[dwLen];
		::GetModuleFileNameW( NULL, pwcBuffer, dwLen );
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
		for ( DWORD I = 1024; TRUE; I += 1024 ) {
			WCHAR * pwcBuffer = new( std::nothrow ) WCHAR[I];
			DWORD dwLen = ::GetModuleFileNameW( NULL, pwcBuffer, I );
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
	std::vector<std::string> CSystem::DllSearchPaths( const WCHAR * _pwcDll, std::vector<std::string> &_vResults ) {
		std::string sDllUtf8 = CUtilities::WStringToString( _pwcDll );
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
	std::vector<std::wstring> CSystem::DllSearchPaths( const WCHAR * _pwcDll, std::vector<std::wstring> &_vResults ) {
		std::wstring sDllUtf8 = _pwcDll;
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

	// EnumProcesses().
	BOOL WINAPI CSystem::EnumProcesses( DWORD * _pdwProcessIds, DWORD _dwCb, DWORD * _pdwBytesReturned ) {
		return m_pfEnumProcesses ? m_pfEnumProcesses( _pdwProcessIds, _dwCb, _pdwBytesReturned ) : FALSE;
	}

	// OpenProcess().
	HANDLE WINAPI CSystem::OpenProcess( DWORD _dwDesiredAccess, BOOL _bInheritHandle, DWORD _dwProcessId ) {
		return m_pfOpenProcess ? m_pfOpenProcess( _dwDesiredAccess, _bInheritHandle, _dwProcessId ) : NULL;
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


		m_pfReadProcessMemory = reinterpret_cast<LPFN_READPROCESSMEMORY>(GetProcAddress( _T_6AE69F02_kernel32_dll, _LEN_6AE69F02, _T_F7C7AE42_ReadProcessMemory, _LEN_F7C7AE42, poObj ));
		m_pfWriteProcessMemory = reinterpret_cast<LPFN_WRITEPROCESSMEMORY>(GetProcAddress( _T_6AE69F02_kernel32_dll, _LEN_6AE69F02, _T_4F58972E_WriteProcessMemory, _LEN_4F58972E, poObj ));
		m_pfEnumProcesses = reinterpret_cast<LPFN_ENUMPROCESSES>(GetProcAddress( _T_6AE69F02_kernel32_dll, _LEN_6AE69F02, _T_0509A21C_EnumProcesses, _LEN_0509A21C, poObj ));
		if ( !m_pfEnumProcesses ) {
			m_pfEnumProcesses = reinterpret_cast<LPFN_ENUMPROCESSES>(GetProcAddress( _T_6AE69F02_kernel32_dll, _LEN_6AE69F02, _T_0501393D_K32EnumProcesses, _LEN_0501393D, poObj ));
		}
		m_pfOpenProcess = reinterpret_cast<LPFN_OPENPROCESS>(GetProcAddress( _T_6AE69F02_kernel32_dll, _LEN_6AE69F02, _T_DF27514B_OpenProcess, _LEN_DF27514B, poObj ));

#ifdef _DEBUG
		LPVOID pfTemp = ::GetProcAddress( ::GetModuleHandleA( szKernel32 ), "ReadProcessMemory" );
		assert( pfTemp == m_pfReadProcessMemory );
		pfTemp = ::GetProcAddress( ::GetModuleHandleA( szKernel32 ), "WriteProcessMemory" );
		assert( pfTemp == m_pfWriteProcessMemory );
		pfTemp = ::GetProcAddress( ::GetModuleHandleA( szKernel32 ), "EnumProcesses" );
		if ( !pfTemp ) {
			pfTemp = ::GetProcAddress( ::GetModuleHandleA( szKernel32 ), "K32EnumProcesses" );
		}
		assert( pfTemp == m_pfEnumProcesses );
		pfTemp = ::GetProcAddress( ::GetModuleHandleA( szKernel32 ), "OpenProcess" );
		assert( pfTemp == m_pfOpenProcess );
#endif	// #ifdef _DEBUG

		::ZeroMemory( szKernel32, MX_ELEMENTS( szKernel32 ) );
	}

}	// namespace mx

#include "MXSystem.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"


namespace mx {

	// == Members.
	// Is the WoW64?
	BOOL CSystem::m_bIsWow = FALSE;

	// System information.
	SYSTEM_INFO CSystem::m_siSystemInfo = { 0 };

	// == Types.
	typedef BOOL (WINAPI * LPFN_ISWOW64PROCESS)( HANDLE, PBOOL );
	typedef VOID (WINAPI * LPFN_GETSYSTEMINFO)( LPSYSTEM_INFO );

	// Initialize the system.
	VOID CSystem::InitSystem() {
		// Is this WoW64?
		{
			CHAR szKernel32[_LEN_204C64E5+1];
			CHAR szIsWow64Process[_LEN_2E50340B+1];
			LPFN_ISWOW64PROCESS pfIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(::GetProcAddress( ::GetModuleHandleA(
				_DEC_204C64E5_kernel32(  szKernel32 ) ),
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
			CHAR szKernel32[_LEN_204C64E5+1];
			CHAR szGetSystemInfo[_T_MAX_LEN];
			if ( IsWow64Process() ) {				
				pfGetSystemInfo = reinterpret_cast<LPFN_GETSYSTEMINFO>(::GetProcAddress( ::GetModuleHandleA(
					_DEC_204C64E5_kernel32(  szKernel32 ) ),
					_DEC_EB64C435_GetNativeSystemInfo( szGetSystemInfo ) ));
			}
			else {
				pfGetSystemInfo = reinterpret_cast<LPFN_GETSYSTEMINFO>(::GetProcAddress( ::GetModuleHandleA(
					_DEC_204C64E5_kernel32(  szKernel32 ) ),
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

}	// namespace mx

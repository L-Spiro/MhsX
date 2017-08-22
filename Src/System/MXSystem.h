#pragma once

#include <string>
#include <Windows.h>
#include <vector>

// == Macros.
#ifndef MX_COUNT_OF
#define MX_COUNT_OF(x)					((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
#define MX_ELEMENTS( x )				MX_COUNT_OF( x )
#endif	// #ifndef MX_COUNT_OF

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

		// Gets the length of the string needed to hold the path to this executable, including the terminating NULL.
		static DWORD					GetSelfPathLength();

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

		// Gets all of the search paths for a DLL in the order in which they should be searched.
		static std::vector<std::string>	DllSearchPaths( const WCHAR * _pwcDll, std::vector<std::string> &_vResults );

		// Gets all of the search paths for a DLL in the order in which they should be searched.
		static std::vector<std::wstring>DllSearchPaths( const WCHAR * _pwcDll, std::vector<std::wstring> &_vResults );

		// Determines if a file exists.
		//GetFileAttributesW

	protected :
		// == Members.
		// Is the WoW64?
		static BOOL						m_bIsWow;

		// System information.
		static SYSTEM_INFO				m_siSystemInfo;
	};

}	// namespace mx

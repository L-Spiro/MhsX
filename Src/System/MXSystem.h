#pragma once

#include "../Files/MXFile.h"
#include "../MXMhsX.h"
#include "../PE/MXPeObject.h"
#include "../Strings/MXStringDecoder.h"

#include <string>
#include <TlHelp32.h>
#include <vector>
//#include <winternl.h>

// == Macros.

#define MX_CHECK_FLAGS_EQ( L, R )		(((L) & (R)) == (R))

#define MX_PTR2UINT( X )				static_cast<uint64_t>(reinterpret_cast<uintptr_t>(X))

#ifndef STATUS_PROCEDURE_NOT_FOUND
	#define STATUS_PROCEDURE_NOT_FOUND	((NTSTATUS)0xC000007AL)
#endif

// == Types.
typedef BOOL (WINAPI * LPFN_READPROCESSMEMORY)( HANDLE, LPCVOID, LPVOID, SIZE_T, SIZE_T * );
typedef BOOL (WINAPI * LPFN_WRITEPROCESSMEMORY)( HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T * );
typedef BOOL (WINAPI * LPFN_VIRTUALPROTECTEX)( HANDLE, LPVOID, SIZE_T, DWORD, PDWORD );
typedef LPVOID (WINAPI * LPFN_VIRTUALALLOCEX)( HANDLE, LPVOID, SIZE_T, DWORD, DWORD );
typedef BOOL (WINAPI * LPFN_VIRTUALFREEEX)( HANDLE, LPVOID, SIZE_T, DWORD );
typedef SIZE_T (WINAPI * LPFN_VIRTUALQUERYEX)( HANDLE, LPCVOID, PMEMORY_BASIC_INFORMATION, SIZE_T );
typedef BOOL (WINAPI * LPFN_ENUMPROCESSES)( DWORD *, DWORD, DWORD * );
typedef HANDLE (WINAPI * LPFN_OPENPROCESS)( DWORD, BOOL, DWORD );
typedef HANDLE (WINAPI * LPFN_OPENTHREAD)( DWORD, BOOL, DWORD );
typedef HANDLE (WINAPI * LPFN_CREATEREMOTETHREAD)( HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD );
typedef BOOL (WINAPI * LPFN_ISWOW64PROCESS)( HANDLE, PBOOL );
typedef HMODULE (WINAPI * LPFN_LOADLIBRARYW)( LPCWSTR );
typedef HMODULE (WINAPI * LPFN_LOADLIBRARYEXW)( LPCWSTR, HANDLE, DWORD );
typedef HANDLE (WINAPI * LPFN_CREATETOOLHELP32SNAPSHOT)( DWORD, DWORD );
typedef BOOL (WINAPI * LPFN_PROCESS32FIRSTW)( HANDLE, LPPROCESSENTRY32W );
typedef BOOL (WINAPI * LPFN_PROCESS32NEXTW)( HANDLE, LPPROCESSENTRY32W );
typedef BOOL (WINAPI * LPFN_THREAD32FIRST)( HANDLE, LPTHREADENTRY32 );
typedef BOOL (WINAPI * LPFN_THREAD32NEXT)( HANDLE, LPTHREADENTRY32 );
typedef BOOL (WINAPI * LPFN_MODULE32FIRSTW)( HANDLE, LPMODULEENTRY32W );
typedef BOOL (WINAPI * LPFN_MODULE32NEXTW)( HANDLE, LPMODULEENTRY32W );
typedef BOOL (WINAPI * LPFN_QUERYFULLPROCESSIMAGENAMEW)( HANDLE, DWORD, LPWSTR, PDWORD );
typedef BOOL (WINAPI * LPFN_ENUMTHREADWINDOWS)( DWORD, WNDENUMPROC lpfn, LPARAM );
typedef BOOL (WINAPI * LPFN_OPENPROCESSTOKEN)( HANDLE, DWORD, PHANDLE );
typedef BOOL (WINAPI * LPFN_LOOKUPPRIVILEGEVALUEW)( LPCWSTR, LPCWSTR, PLUID );
typedef BOOL (WINAPI * LPFN_ADJUSTTOKENPRIVILEGES)( HANDLE, BOOL, PTOKEN_PRIVILEGES, DWORD, PTOKEN_PRIVILEGES, PDWORD );
typedef BOOL (WINAPI * LPFN_GETEXITCODEPROCESS)( HANDLE, LPDWORD );
typedef BOOL (WINAPI * LPFN_GETEXITCODETHREAD)( HANDLE, LPDWORD );
typedef BOOL (WINAPI * LPFN_ISDEBUGGERPRESENT)();
typedef BOOL (WINAPI * LPFN_CHECKREMOTEDEBUGGERPRESENT)( HANDLE, PBOOL );
typedef BOOL (WINAPI * LPFN_DEBUGACTIVEPROCESS)( DWORD );
typedef BOOL (WINAPI * LPFN_DEBUGACTIVEPROCESSSTOP)( DWORD );
typedef BOOL (WINAPI * LPFN_CONTINUEDEBUGEVENT)( DWORD, DWORD, DWORD );
typedef BOOL (WINAPI * LPFN_DEBUGSETPROCESSKILLONEXIT)( BOOL );
typedef BOOL (WINAPI * LPFN_WAITFORDEBUGEVENT)( LPDEBUG_EVENT, DWORD );
typedef BOOL (WINAPI * LPFN_WAITFORDEBUGEVENTEX)( LPDEBUG_EVENT, DWORD );
typedef BOOL (WINAPI * LPFN_GETTHREADCONTEXT)( HANDLE, LPCONTEXT );
typedef BOOL (WINAPI * LPFN_SETTHREADCONTEXT)( HANDLE, const CONTEXT * );
typedef BOOL (WINAPI * LPFN_WOW64GETTHREADCONTEXT)( HANDLE, PWOW64_CONTEXT );
typedef BOOL (WINAPI * LPFN_WOW64SETTHREADCONTEXT)( HANDLE, const WOW64_CONTEXT * );
typedef BOOL (WINAPI * LPFN_WOW64GETTHREADSELECTORENTRY)( HANDLE, DWORD, PWOW64_LDT_ENTRY );
typedef UINT_PTR (WINAPI * LPFN_SETTIMER)( HWND, UINT_PTR, UINT, TIMERPROC );
typedef BOOL (WINAPI * LPFN_KILLTIMER)( HWND, UINT_PTR );
typedef LONG (NTAPI * LPFN_NTSUSPENDPROCESS)( HANDLE );
typedef LONG (NTAPI * LPFN_NTRESUMEPROCESS)( HANDLE );
typedef int (WINAPI * LPFN_COMPARESTRINGEX)( LPCWSTR, DWORD, LPCWCH, int, LPCWCH, int, LPNLSVERSIONINFO, LPVOID, LPARAM );
typedef HHOOK (WINAPI * LPFN_SETWINDOWSHOOKEX)( int, HOOKPROC, HINSTANCE, DWORD );
typedef LRESULT (WINAPI * LPFN_CALLNEXTHOOKEX)( HHOOK, int, WPARAM, LPARAM );
typedef BOOL (WINAPI * LPFN_UNHOOKWINDOWSHOOKEX)( HHOOK _hhk );
typedef BOOL (WINAPI * LPFN_REGISTERHOTKEY)( HWND, int, UINT, UINT );
typedef BOOL (WINAPI * LPFN_UNREGISTERHOTKEY)( HWND,  int );
typedef BOOL (WINAPI * LPFN_GETKEYBOARDSTATE)( PBYTE );
typedef SHORT (WINAPI * LPFN_GETASYNCKEYSTATE)( int );
typedef BOOL (WINAPI * LPFN_BEEP)( DWORD, DWORD );
typedef BOOL (WINAPI * LPFN_GETPROCESSTIMES)( HANDLE, LPFILETIME, LPFILETIME, LPFILETIME, LPFILETIME );

typedef struct _IO_STATUS_BLOCK {
	union { NTSTATUS Status; PVOID Pointer; };
	ULONG_PTR										Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef enum _FILE_INFORMATION_CLASS {
	FileDirectoryInformation						= 1,
	FileFullDirectoryInformation					= 2,
	FileBothDirectoryInformation					= 3,
	FileBasicInformation							= 4,
	FileStandardInformation							= 5,
	FileInternalInformation							= 6,
	FileEaInformation								= 7,
	FileAccessInformation							= 8,
} FILE_INFORMATION_CLASS;

typedef struct _FILE_ACCESS_INFORMATION {
	ULONG											AccessFlags;
} FILE_ACCESS_INFORMATION;

typedef NTSTATUS (NTAPI * LPFN_NTQUERYINFORMATIONFILE)( HANDLE, PIO_STATUS_BLOCK, PVOID, ULONG, FILE_INFORMATION_CLASS );

typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation = 0,
    ProcessDebugPort = 7,
    ProcessWow64Information = 26,
    ProcessImageFileName = 27,
    ProcessBreakOnTermination = 29
} PROCESSINFOCLASS;

typedef struct _PROCESS_BASIC_INFORMATION_LOCAL {
    PVOID  Reserved1;
    PVOID  PebBaseAddress;
    PVOID  Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID  Reserved3;
} PROCESS_BASIC_INFORMATION_LOCAL;

typedef NTSTATUS (NTAPI * LPFN_NTQUERYINFORMATIONPROCESS)( HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG );




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

		// Is the system 32-bit?
		static bool						Is32Bit();

		// Is the system 64-bit?
		static bool						Is64Bit();

		// Gets a function address by DLL name and function name.
		static LPVOID					GetProcAddress( const WCHAR * _pwcDll, const CHAR * _pcFunc );

		// Gets the file name from a given path.
		static CSecureString			GetFileName( const std::string &_sPath );

		// Gets the file name from a given path.
		static CSecureWString			GetFileName( const std::wstring &_wsPath );

		// Gets the path to this .EXE in UTF-8.
		static CSecureString			GetSelfPath();

		// Gets the path to this .EXE in UTF-16.
		static CSecureWString			GetSelfPathW();

		// Gets the Resources/ path.
		static CSecureWString			GetResourcesPathW();

		// Gets the path to a given loaded DLL given its UTF-8 name.
		static CSecureString			GetModulePath( const CHAR * _pcPath );

		// Gets the path to a given loaded DLL given its UTF-16 name.
		static CSecureWString			GetModulePathW( const WCHAR * _pwcPath );

		// Gets the length of the string needed to hold the path to this executable, including the terminating NULL.
		static DWORD					GetSelfPathLength();

		// Gets the length of the path of a loaded DLL, including the terminating NULL.
		static DWORD					GetModulePathLength( const WCHAR * _pwcPath );

		// Gets the current directory in UTF-8.
		static CSecureString			GetCurDir();

		// Gets the current directory in UTF-16.
		static CSecureWString			GetCurDirW();

		// Gets the system directory in UTF-8.
		static CSecureString			GetSystemDir();

		// Gets the system directory in UTF-16.
		static CSecureWString			GetSystemDirW();

		// Gets the Windows directory in UTF-8.
		static CSecureString			GetWindowsDir();

		// Gets the Windows directory in UTF-16.
		static CSecureWString			GetWindowsDirW();

		// Gets all the paths in the PATH environment variable.
		static size_t					GetPathEnv( std::vector<std::string> &_vReturn );

		// Gets all the paths in the PATH environment variable.
		static size_t					GetPathEnv( std::vector<std::wstring> &_vReturn );
		static size_t					GetPathEnvW( std::vector<std::wstring> &_vReturn ) { return GetPathEnv( _vReturn ); }

		// Gets all of the search paths for a DLL in the order in which they should be searched.
		static std::vector<std::string> &
										DllSearchPaths( const WCHAR * _pwcDll, std::vector<std::string> &_vResults, BOOL _bIncludeLoadedModulePath = TRUE );

		// Gets all of the search paths for a DLL in the order in which they should be searched.
		static std::vector<std::wstring> &
										DllSearchPaths( const WCHAR * _pwcDll, std::vector<std::wstring> &_vResults, BOOL _bIncludeLoadedModulePath = TRUE );

		// Gets all of the search paths for an EXE in the order in which they should be searched.
		static std::vector<std::string> &
										ExeSearchPaths( const WCHAR * _pwcExe, std::vector<std::string> &_vResults );

		// Gets all of the search paths for an EXE in the order in which they should be searched.
		static std::vector<std::wstring> &
										ExeSearchPaths( const WCHAR * _pwcExe, std::vector<std::wstring> &_vResults );

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

		// VirtualProtectEx().
		static BOOL WINAPI				VirtualProtectEx( HANDLE _hProcess, LPVOID _lpAddress, SIZE_T _dwSize, DWORD _flNewProtect, PDWORD _lpflOldProtect );

		// VirtualAllocEx();
		static LPVOID WINAPI			VirtualAllocEx( HANDLE _hProcess, LPVOID _lpAddress, SIZE_T _dwSize, DWORD _flAllocationType, DWORD _flProtect );

		// VirtualFreeEx().
		static BOOL WINAPI				VirtualFreeEx( HANDLE _hProcess, LPVOID _lpAddress, SIZE_T _dwSize, DWORD _dwFreeType );

		// VirtualQueryEx();
		static SIZE_T WINAPI			VirtualQueryEx( HANDLE _hProcess, LPCVOID _lpAddress, PMEMORY_BASIC_INFORMATION _lpBuffer, SIZE_T _dwLength );

		// EnumProcesses().
		static BOOL WINAPI				EnumProcesses( DWORD * _pdwProcessIds, DWORD _dwCb, DWORD * _pdwBytesReturned );

		// Gets the total number of processes EnumProcesses() should want to return.  Might be inaccurate if processes are quickly and constantly
		//	being opened and closed etc., so this returns a buffer larger than necessary.
		static DWORD					EnumProcessesBufferSize();

		// OpenProcess().
		static HANDLE WINAPI			OpenProcess( DWORD _dwDesiredAccess, BOOL _bInheritHandle, DWORD _dwProcessId );

		// OpenThread().
		static HANDLE WINAPI			OpenThread( DWORD _dwDesiredAccess, BOOL _bInheritHandle, DWORD _dwThreadId );

		// CreateRemoteThread().
		static HANDLE WINAPI			CreateRemoteThread( HANDLE _hProcess, LPSECURITY_ATTRIBUTES _lpThreadAttributes, SIZE_T _dwStackSize, LPTHREAD_START_ROUTINE _lpStartAddress,
			LPVOID _lpParameter, DWORD _dwCreationFlags, LPDWORD _lpThreadId );

		// IsWow64Process().
		static BOOL WINAPI				IsWow64Process( HANDLE _hProcess, PBOOL _Wow64Process );

		// LoadLibraryW().
		static HMODULE WINAPI			LoadLibraryW( LPCWSTR _lpLibFileName );

		// LoadLibraryExW().
		static HMODULE WINAPI			LoadLibraryExW( LPCWSTR _lpLibFileName, HANDLE _hFile, DWORD _dwFlags );

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

		// GetExitCodeProcess().
		static BOOL WINAPI				GetExitCodeProcess( HANDLE _hProcess, LPDWORD _lpExitCode );

		// GetExitCodeThread().
		static BOOL WINAPI				GetExitCodeThread( HANDLE _hThread, LPDWORD _lpExitCode );

		// Determines how large a buffer must be to accept the full path of a process when calling QueryFullProcessImageNameW().
		static DWORD					FullProcessPathLen( HANDLE _hProcess );

		// EnumThreadWindows().
		static BOOL WINAPI				EnumThreadWindows( DWORD _dwThreadId, WNDENUMPROC _lpfn, LPARAM _lParam );

		// Gets window text.
		static BOOL						GetWindowTextW( HWND _hWnd, std::wstring &_sRes );

		// OpenProcessToken().
		static BOOL WINAPI				OpenProcessToken( HANDLE _ProcessHandle, DWORD _DesiredAccess, PHANDLE _TokenHandle );

		// LookupPrivilegeValueW().
		static BOOL WINAPI				LookupPrivilegeValueW( LPCWSTR _lpSystemName, LPCWSTR _lpName, PLUID _lpLuid );

		// AdjustTokenPrivileges().
		static BOOL WINAPI				AdjustTokenPrivileges( HANDLE _TokenHandle, BOOL _DisableAllPrivileges, PTOKEN_PRIVILEGES _NewState, DWORD _BufferLength, PTOKEN_PRIVILEGES _PreviousState, PDWORD _ReturnLength );

		// Show an encrypted error message box.
		static VOID						MessageBoxError( HWND _hWnd, const CHAR * _pcMsg, size_t _sMsgLen, const CHAR * _pcTitle = _T_9C1C9375_Error, size_t _sTitleLen = _LEN_9C1C9375 );

		// Show an encrypted message box.
		static VOID						MessageBoxOk( HWND _hWnd, const CHAR * _pcMsg, size_t _sMsgLen, const CHAR * _pcTitle, size_t _sTitleLen );

		// Tests the flags that can be used to open a given process.
		static DWORD					TestOpenProcess( DWORD _dwId );

		// IsDebuggerPresent().
		static BOOL WINAPI				IsDebuggerPresent();

		// CheckRemoteDebuggerPresent().
		static BOOL WINAPI				CheckRemoteDebuggerPresent( HANDLE _hProcess, PBOOL _pbDebuggerPresent );

		// DebugActiveProcess().
		static BOOL WINAPI				DebugActiveProcess( DWORD _dwProcessId );

		// DebugActiveProcessStop().
		static BOOL WINAPI				DebugActiveProcessStop( DWORD _dwProcessId );

		// ContinueDebugEvent().
		static BOOL WINAPI				ContinueDebugEvent( DWORD _dwProcessId, DWORD _dwThreadId, DWORD _dwContinueStatus );

		// DebugSetProcessKillOnExit().
		static BOOL WINAPI				DebugSetProcessKillOnExit( BOOL _KillOnExit );

		// WaitForDebugEvent().
		static BOOL WINAPI				WaitForDebugEvent( LPDEBUG_EVENT _lpDebugEvent, DWORD _dwMilliseconds );

		// GetThreadContext().
		static BOOL WINAPI				GetThreadContext( HANDLE _hThread, LPCONTEXT _lpContext );

		// SetThreadContext().
		static BOOL WINAPI				SetThreadContext( HANDLE _hThread, const CONTEXT * _lpContext );

		// Wow64GetThreadContext().
		static BOOL WINAPI				Wow64GetThreadContext( HANDLE _hThread, PWOW64_CONTEXT _lpContext );

		// Wow64SetThreadContext().
		static BOOL WINAPI				Wow64SetThreadContext( HANDLE _hThread, const WOW64_CONTEXT * _lpContext );

		// Wow64GetThreadSelectorEntry().
		static BOOL WINAPI				Wow64GetThreadSelectorEntry( HANDLE _hThread, DWORD _dwSelector, PWOW64_LDT_ENTRY _lpSelectorEntry );

		// SetTimer().
		static UINT_PTR					SetTimer( HWND _hWnd, UINT_PTR _nIDEvent, UINT _uElapse, TIMERPROC _lpTimerFunc );

		// KillTimer().
		static BOOL						KillTimer( HWND _hWnd, UINT_PTR _uIDEvent );

		// NtSuspendProcess.
		static LONG NTAPI				NtSuspendProcess( HANDLE _hProcess );

		// NtResumeProcess.
		static LONG NTAPI				NtResumeProcess( HANDLE _hProcess );

		// CompareStringEx.
		static int WINAPI				CompareStringEx( LPCWSTR _lpLocaleName, DWORD _dwCmpFlags, LPCWCH _lpString1, int _cchCount1, LPCWCH _lpString2, int _cchCount2, LPNLSVERSIONINFO _lpVersionInformation,
			LPVOID _lpReserved, LPARAM _lParam );

		// SetWindowsHookExW.
		static HHOOK WINAPI				SetWindowsHookExW( int _idHook, HOOKPROC _lpfn, HINSTANCE _hmod, DWORD _dwThreadId );

		// CallNextHookEx.
		static LRESULT WINAPI			CallNextHookEx( HHOOK _hhk, int _nCode, WPARAM _wParam, LPARAM _lParam );

		// UnhookWindowsHookEx.
		static BOOL WINAPI				UnhookWindowsHookEx( HHOOK _hhk );


		// RegisterHotKey.
		static BOOL WINAPI				RegisterHotKey( HWND _hWnd, int _id, UINT _fsModifiers, UINT _vk );

		// UnregisterHotKey.
		static BOOL WINAPI				UnregisterHotKey( HWND _hWnd, int _id );

		
		// GetKeyboardState.
		static BOOL WINAPI				GetKeyboardState( PBYTE _lpKeyState );

		// GetAsyncKeyState.
		static SHORT WINAPI				GetAsyncKeyState( int _vKey );

		// GetProcessTimes.
		static BOOL WINAPI				GetProcessTimes( HANDLE _hProcess, LPFILETIME _lpCreationTime, LPFILETIME _lpExitTime, LPFILETIME _lpKernelTime, LPFILETIME _lpUserTime );

		// NtQueryInformationFile.
		static NTSTATUS NTAPI			NtQueryInformationFile( HANDLE _hFileHandle, PIO_STATUS_BLOCK _sbIoStatusBlock, PVOID _pvFileInformation, ULONG _ulLength, FILE_INFORMATION_CLASS _ficFileInformationClass );

		// NtQueryInformationFile.
		static NTSTATUS NTAPI			NtQueryInformationProcess( HANDLE _hProcessHandle, PROCESSINFOCLASS _picProcessInformationClass, PVOID _pvProcessInformation, ULONG _ulProcessInformationLength, PULONG _pulReturnLength );



		// Determines if the given address is out of the native range of this process.
		static bool						AddressIsInNativeMemoryRange( uint64_t _uiAddr ) {
			uint64_t ui64MissingBits = 0;
			ui64MissingBits ^= ~static_cast<uint64_t>(static_cast<uintptr_t>(-1));
			return (ui64MissingBits & _uiAddr) == 0;
		}


	protected :
		// == Members.
		// Is the WoW64?
		static BOOL						m_bIsWow;

		// System information.
		static SYSTEM_INFO				m_siSystemInfo;

		// ReadProcessMemory().
		static LPFN_READPROCESSMEMORY	m_pfReadProcessMemory;

		// WriteProcessMemory().
		static LPFN_WRITEPROCESSMEMORY	m_pfWriteProcessMemory;

		// VirtualProtectEx().
		static LPFN_VIRTUALPROTECTEX	m_pfVirtualProtectEx;
		
		// VirtualAllocEx().
		static LPFN_VIRTUALALLOCEX		m_pfVirtualAllocEx;

		// VirtualFreeEx().
		static LPFN_VIRTUALFREEEX		m_pfVirtualFreeEx;

		// VirtualQueryEx().
		static LPFN_VIRTUALQUERYEX		m_pfVirtualQueryEx;

		// EnumProcesses().
		static LPFN_ENUMPROCESSES		m_pfEnumProcesses;

		// OpenProcess().
		static LPFN_OPENPROCESS			m_pfOpenProcess;

		// OpenThread().
		static LPFN_OPENTHREAD			m_pfOpenThread;

		// CreateRemoteThread().
		static LPFN_CREATEREMOTETHREAD	m_pfCreateRemoteThread;

		// IsWow64Process().
		static LPFN_ISWOW64PROCESS		m_pfIsWow64Process;

		// LoadLibraryW().
		static LPFN_LOADLIBRARYW		m_pfLoadLibraryW;

		// LoadLibraryExW().
		static LPFN_LOADLIBRARYEXW		m_pfLoadLibraryExW;

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

		// OpenProcessToken().
		static LPFN_OPENPROCESSTOKEN	m_pfOpenProcessToken;

		// LookupPrivilegeValue().
		static LPFN_LOOKUPPRIVILEGEVALUEW
										m_pfLookupPrivilegeValueW;

		// AdjustTokenPrivileges().
		static LPFN_ADJUSTTOKENPRIVILEGES
										m_pfAdjustTokenPrivileges;

		// GetExitCodeProcess().
		static LPFN_GETEXITCODEPROCESS	m_pfGetExitCodeProcess;

		// GetExitCodeThread().
		static LPFN_GETEXITCODETHREAD	m_pfGetExitCodeThread;

		// IsDebuggerPresent().
		static LPFN_ISDEBUGGERPRESENT	m_pfIsDebuggerPresent;

		// CheckRemoteDebuggerPresent().
		static LPFN_CHECKREMOTEDEBUGGERPRESENT
										m_pfCheckRemoteDebuggerPresent;

		// DebugActiveProcess().
		static LPFN_DEBUGACTIVEPROCESS	m_pfDebugActiveProcess;

		// DebugActiveProcessStop().
		static LPFN_DEBUGACTIVEPROCESSSTOP
										m_pfDebugActiveProcessStop;

		// ContinueDebugEvent().
		static LPFN_CONTINUEDEBUGEVENT	m_pfContinueDebugEvent;

		// DebugSetProcessKillOnExit().
		static LPFN_DEBUGSETPROCESSKILLONEXIT
										m_pfDebugSetProcessKillOnExit;

		// WaitForDebugEvent().
		static LPFN_WAITFORDEBUGEVENT	m_pfWaitForDebugEvent;

		// WaitForDebugEventEx().
		static LPFN_WAITFORDEBUGEVENTEX	m_pfWaitForDebugEventEx;

		// GetThreadContext().
		static LPFN_GETTHREADCONTEXT	m_pfGetThreadContext;

		// SetThreadContext().
		static LPFN_SETTHREADCONTEXT	m_pfSetThreadContext;

		// Wow64GetThreadContext().
		static LPFN_WOW64GETTHREADCONTEXT
										m_pfWow64GetThreadContext;

		// Wow64SetThreadContext().
		static LPFN_WOW64SETTHREADCONTEXT
										m_pfWow64SetThreadContext;

		// Wow64GetThreadSelectorEntry().
		static LPFN_WOW64GETTHREADSELECTORENTRY
										m_pfWow64GetThreadSelectorEntry;

		// SetTimer().
		static LPFN_SETTIMER			m_pfSetTimer;

		// KillTimer().
		static LPFN_KILLTIMER			m_pfKillTimer;

		// NtSuspendProcess().
		static LPFN_NTSUSPENDPROCESS	m_pfNtSuspendProcess;

		// NtResumeProcess().
		static LPFN_NTRESUMEPROCESS		m_pfNtResumeProcess;

		// CompareStringEx().
		static LPFN_COMPARESTRINGEX		m_pfCompareStringEx;

		// SetWindowsHookExW().
		static LPFN_SETWINDOWSHOOKEX	m_pfSetWindowsHookExW;

		// CallNextHookEx().
		static LPFN_CALLNEXTHOOKEX		m_pfCallNextHookEx;

		// UnhookWindowsHookEx().
		static LPFN_UNHOOKWINDOWSHOOKEX	m_pfUnhookWindowsHookEx;

		// RegisterHotKey().
		static LPFN_REGISTERHOTKEY		m_pfRegisterHotKey;

		// UnregisterHotKey().
		static LPFN_UNREGISTERHOTKEY	m_pfUnregisterHotKey;

		// GetKeyboardState().
		static LPFN_GETKEYBOARDSTATE	m_pfGetKeyboardState;

		// GetAsyncKeyState().
		static LPFN_GETASYNCKEYSTATE	m_pfGetAsyncKeyState;

		// Beep().
		static LPFN_BEEP				m_pfBeep;

		// GetProcessTimes().
		static LPFN_GETPROCESSTIMES		m_pfGetProcessTimes;

		// NtQueryInformationFile().
		static LPFN_NTQUERYINFORMATIONFILE
										m_pfNtQueryInformationFile;

		// NtQueryInformationProcess().
		static LPFN_NTQUERYINFORMATIONPROCESS
										m_pfNtQueryInformationProcess;


		// == Functions.
		// Load kernel32.dll functions.
		static VOID						LoadKernel32();

		// Load User32.dll functions.
		static VOID						LoadUser32();

		// Load Advapi32.dll functions.
		static VOID						LoadAdvapi32();

		// Load ntdll.dll functions.
		static VOID						LoadNtdll();
	};

}	// namespace mx

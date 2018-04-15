#pragma once
#include "../MXMhsX.h"
#include <CriticalSection/LSWCriticalSection.h>
#include <Helpers/LSWHelpers.h>

using namespace lsw;

namespace mx {

	class CProcess {
	public :
		CProcess();
		~CProcess();


		// == Enumerations.
		// Mode to use for opening the process.
		enum MX_OPEN_PROC_MODE {
			MX_OPM_CONSERVATIVE,		// Use as few flags as necessary to work with the process.  Re-opens the process each time a new OpenProcess()
										//	flag needs to be added.  For example, PROCESS_VM_READ will only be used when ReadProcessMemory() is called
										//  for the first time.
			MX_OPM_NORMAL,				// Opens the process using
										//	PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE |
										//	PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME
			MX_OPM_FIXED,				// Opens the process using flags specified by the user.
		};


		// == Functions.
		// Open the given process with the given mode and flags.
		virtual bool					OpenProc( DWORD _dwId, MX_OPEN_PROC_MODE _opmMode, DWORD _dwFlags );

		// Detatch from the current process, if there is one.
		virtual void					Detatch();

		// Gets the process ID.
		DWORD							ProcId() const { LSW_ENT_CRIT( m_csCrit ); return m_dwId; }

		// Gets the mode.
		MX_OPEN_PROC_MODE				Mode() const { LSW_ENT_CRIT( m_csCrit ); return m_opmMode; }

		// Gets the flags used to open the process.
		DWORD							Flags() const { LSW_ENT_CRIT( m_csCrit ); return m_dwOpenProcFlags; }

		// Is a process open?
		bool							ProcIsOpened() const { LSW_ENT_CRIT( m_csCrit ); return m_hProcHandle.Valid() && m_dwId != static_cast<DWORD>(-1); }

		// Reads data from an area of memory in a specified process. The entire area to be read must be accessible or the operation fails.
		virtual bool					ReadProcessMemory( LPCVOID _lpBaseAddress, LPVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesRead );

		// Writes data to an area of memory in a specified process. The entire area to be written to must be accessible or the operation fails.
		virtual bool					WriteProcessMemory( LPVOID _lpBaseAddress, LPCVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten );

		// Changes the protection on a region of committed pages in the virtual address space of a specified process.
		virtual bool					VirtualProtectEx( LPVOID _lpAddress, SIZE_T _dwSize, DWORD _flNewProtect, PDWORD _lpflOldProtect );

		// Retrieves information about a range of pages within the virtual address space of a specified process.
		virtual bool					VirtualQueryEx( uint64_t _lpAddress, PMEMORY_BASIC_INFORMATION64 _lpBuffer );

		// Determines whether the specified process is running under WOW64.
		virtual bool					IsWow64Process();

		// Gets the base and size of a region given an address.
		virtual bool					GetChunk( uint64_t _lpAddress, uint64_t &_uiBaseAddress, uint64_t &_uiRegionSize );

		// 



	protected :
		// == Members.
		// Process ID.
		DWORD							m_dwId;

		// Handle to the process.  May be recreated many times, so do not make copies.
		LSW_HANDLE						m_hProcHandle;

		// OpenProcess() flags.
		DWORD							m_dwOpenProcFlags;

		// Mode for accessing the target process.
		MX_OPEN_PROC_MODE				m_opmMode;

		// Critical section.
		CCriticalSection				m_csCrit;


		// == Functions.
		// Internal open process.
		bool							OpenProcInternal( DWORD _dwId, DWORD _dwFlags );

		// Opens the target process conservatively.
		bool							OpenProcConservative( DWORD _dwId );

		// Reads data from an area of memory in a specified process. The entire area to be read must be accessible or the operation fails.
		bool							ReadProcessMemoryInternal( LPCVOID _lpBaseAddress, LPVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesRead );

		// Writes data to an area of memory in a specified process. The entire area to be written to must be accessible or the operation fails.
		bool							WriteProcessMemoryInternal( LPVOID _lpBaseAddress, LPCVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten );

		// Changes the protection on a region of committed pages in the virtual address space of a specified process.
		bool							VirtualProtectExInternal( LPVOID _lpAddress, SIZE_T _dwSize, DWORD _flNewProtect, PDWORD _lpflOldProtect );

		// Retrieves information about a range of pages within the virtual address space of a specified process.
		size_t							VirtualQueryExInternal( uint64_t _lpAddress, PMEMORY_BASIC_INFORMATION64 _lpBuffer );

	};

}	// namespace mx

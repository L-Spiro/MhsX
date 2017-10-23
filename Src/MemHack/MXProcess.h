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
		DWORD							ProcId() const { return m_dwId; }

		// Gets the mode.
		MX_OPEN_PROC_MODE				Mode() const { return m_opmMode; }


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

	};

}	// namespace mx

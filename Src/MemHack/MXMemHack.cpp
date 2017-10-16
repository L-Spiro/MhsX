#include "MXMemHack.h"
#include <Base/LSWBase.h>

namespace mx {

	CMemHack::CMemHack() :
		m_dwProcId( 0 ) {
	}
	CMemHack::~CMemHack() {
		Detach();
	}

	// == Functions.
	// Open a process by its ID.
	bool CMemHack::OpenProcess( DWORD _dwId ) {
		Detach();

		// PROCESS_CREATE_THREAD: CreateThread()
		// PROCESS_VM_READ: ReadProcessMemory()
		// PROCESS_VM_WRITE: WriteProcessMemory()
		// PROCESS_VM_OPERATION: VirtualProtectEx(), WriteProcessMemory()
		// PROCESS_QUERY_INFORMATION: OpenProcessToken()
		// PROCESS_SUSPEND_RESUME: NtSuspendProcess(), NtResumeProcess()

		DWORD dwAttempts[] = {
			PROCESS_CREATE_THREAD |											// The full range of what we might want to do.
				PROCESS_VM_READ | PROCESS_VM_WRITE |
				PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION |
				PROCESS_SUSPEND_RESUME,
			PROCESS_CREATE_THREAD |											// Maybe we can live without suspending the process.
				PROCESS_VM_READ | PROCESS_VM_WRITE |
				PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,
			/*PROCESS_CREATE_THREAD |											// Maybe we can live without suspending the process.
				PROCESS_VM_READ | PROCESS_VM_WRITE |
				PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,*/
		};

		return true;
	}

	// Detach from the current process.
	void CMemHack::Detach() {
		m_hProc.Reset();
	}

}	// namespace mx

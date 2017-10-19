#include "MXMemHack.h"
#include <Base/LSWBase.h>

namespace mx {

	CMemHack::CMemHack() :
		m_dwProcId( 0 ) {
		// Default options.
		m_oOptions.dwFoundAddressRefresh = 10;
		m_oOptions.dwMainRefresh = 10;
		m_oOptions.dwLockedRefresh = 1000;
		m_oOptions.dwExpressionRefresh = 100;
		m_oOptions.bDataTypesAsCodeNames = TRUE;
		m_oOptions.bDataTypeSizes = TRUE;
		m_oOptions.bDataTypeRanges = TRUE;
		// ====
		m_oOptions.bMemImage = TRUE;
		m_oOptions.bMemPrivate = TRUE;
		m_oOptions.bMemMapped = FALSE;
		m_oOptions.iThreadPriority = THREAD_PRIORITY_NORMAL;
		m_oOptions.dwBufferSize = 5 * 1024 * 1024;
		m_oOptions.bPreCache = TRUE;
		m_oOptions.bPostFlush = TRUE;
		m_oOptions.bPauseTarget = FALSE;
		m_oOptions.bUseEpsilon = TRUE;
		m_oOptions.bSmartEpsilon = TRUE;
		m_oOptions.dEpsilon = FLT_EPSILON;// (1.19209290E-07F) (Win32 uses 1.192092896e-07F);
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

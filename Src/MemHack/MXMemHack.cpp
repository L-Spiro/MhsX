#include "MXMemHack.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>

// TEMP.
#include "../Search/MXSearcher.h"

namespace mx {

	CMemHack::CMemHack() {
		// Default options.
		m_oOptions.dwFoundAddressRefresh = 10;
		m_oOptions.dwMainRefresh = 10;
		m_oOptions.dwLockedRefresh = 1000;
		m_oOptions.dwExpressionRefresh = 100;
		m_oOptions.bDataTypesAsCodeNames = TRUE;
		m_oOptions.bDataTypeSizes = TRUE;
		m_oOptions.bDataTypeRanges = TRUE;
		// ====
		m_oOptions.dwOpenProc = MX_OP_SHOW_ALL;
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
		m_oOptions.bAligned = TRUE;
		m_oOptions.bSameAsOriginal = TRUE;
	}
	CMemHack::~CMemHack() {
		Detach();
	}

	// == Functions.
	// Open a process by its ID.
	bool CMemHack::OpenProcess( DWORD _dwId ) {
		Detach();

		const CProcess::MX_OPEN_PROC_MODE opmMode = CProcess::MX_OPM_FIXED;
		switch ( opmMode ) {
			case CProcess::MX_OPM_FIXED : {
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

					PROCESS_QUERY_INFORMATION,										// The bare minimum.
					/*PROCESS_CREATE_THREAD |											// Maybe we can live without suspending the process.
						PROCESS_VM_READ | PROCESS_VM_WRITE |
						PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,*/
				};
				for ( size_t I = 0; I < MX_ELEMENTS( dwAttempts ); ++I ) {
					if ( m_pProcess.OpenProc( _dwId, opmMode, dwAttempts[I] ) ) {
						break;
					}
				}
				break;
			}
			default : {
				m_pProcess.OpenProc( _dwId, opmMode, 0 );
			}
		}

		if ( !m_pProcess.ProcIsOpened() ) {
			FailedToOpenProcess( _dwId );
			return false;
		}
		
		OpenedProcess();

		return true;
	}

	// Opened a process.
	void CMemHack::OpenedProcess() {
		CSearcher sSearcher( &m_pProcess );
		/*CAddressChunkList adcList;
		uint64_t ui64ChunkSize = 31ULL * 1024 * 1024 * 1024;
		sSearcher.GenerateChunks( adcList, ui64ChunkSize, 157 );*/
	}

	// Failed to open a process.
	void CMemHack::FailedToOpenProcess( DWORD _dwId ) {
	}

	// Detach from the current process.
	void CMemHack::Detach() {
		m_pProcess.Detatch();
		//m_hProc.Reset();
	}

	// Gets the options.
	const MX_OPTIONS & CMemHack::Options() const {
		// Gather system-wide options into our structure.
		m_oOptions.bDataTypesAsCodeNames = (CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES) ? TRUE : FALSE;
		m_oOptions.bDataTypeRanges = (CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_SHOWRANGES) ? TRUE : FALSE;
		m_oOptions.bDataTypeSizes = (CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_SHOWSIZES) ? TRUE : FALSE;
		return m_oOptions;
	}

	// Sets the options.
	void CMemHack::SetOptions( const MX_OPTIONS &_oOptions ) {
		m_oOptions = _oOptions;
		// Disperse options across the system.
		DWORD dwDataTypeOpts = 0;
		dwDataTypeOpts |= m_oOptions.bDataTypesAsCodeNames ? CUtilities::MX_DTO_CODENAMES : 0;
		dwDataTypeOpts |= m_oOptions.bDataTypeRanges ? CUtilities::MX_DTO_SHOWRANGES : 0;
		dwDataTypeOpts |= m_oOptions.bDataTypeSizes ? CUtilities::MX_DTO_SHOWSIZES : 0;
		CUtilities::Options.dwDataTypeOptions = dwDataTypeOpts;
	}

}	// namespace mx

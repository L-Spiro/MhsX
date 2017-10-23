#include "MXProcess.h"
#include "../System/MXSystem.h"

namespace mx {

	CProcess::CProcess() :
		m_dwId( static_cast<DWORD>(-1) ),
		m_dwOpenProcFlags( 0 ) {
	}
	CProcess::~CProcess() {
		Detatch();
	}

	// == Functions.
	// Open the given process with the given mode and flags.
	bool CProcess::OpenProc( DWORD _dwId, MX_OPEN_PROC_MODE _opmMode, DWORD _dwFlags ) {
		LSW_ENT_CRIT( m_csCrit );
		Detatch();

		m_opmMode = _opmMode;
		switch ( _opmMode ) {
			case MX_OPM_NORMAL : {
				if ( !OpenProcInternal( _dwId, PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE |
					PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME ) ) {
					if ( !OpenProcConservative( _dwId ) ) { return false; }
				}
				break;
			}
			case MX_OPM_CONSERVATIVE : {
				if ( !OpenProcInternal( _dwId, PROCESS_QUERY_INFORMATION ) ) {
					return false;
				}
				break;
			}
			case MX_OPM_FIXED : {
				if ( !OpenProcInternal( _dwId, _dwFlags ) ) {
					if ( !OpenProcConservative( _dwId ) ) { return false; }
				}
				break;
			}

		}

		return true;
	}

	// Detatch from the current process, if there is one.
	void CProcess::Detatch() {
		LSW_ENT_CRIT( m_csCrit );
		m_hProcHandle.Reset();
		m_dwId = static_cast<DWORD>(-1);
	}

	// Internal open process.
	bool CProcess::OpenProcInternal( DWORD _dwId, DWORD _dwFlags ) {
		LSW_ENT_CRIT( m_csCrit );
		if ( m_dwId == _dwId && m_dwOpenProcFlags == _dwFlags ) { return true; }
		// If opening with the new flags fails, keep old handle.
		HANDLE hTemp = CSystem::OpenProcess( _dwFlags, FALSE, _dwId );
		if ( LSW_HANDLE::Valid( hTemp ) ) {
			m_hProcHandle.Reset();
			m_hProcHandle.hHandle = hTemp;
			m_dwId = _dwId;
			m_dwOpenProcFlags = _dwFlags;
			return true;
		}
		return m_hProcHandle.Valid();
	}

	// Opens the target process conservatively.
	bool CProcess::OpenProcConservative( DWORD _dwId ) {
		if ( OpenProcInternal( _dwId, PROCESS_QUERY_INFORMATION ) ) {
			m_opmMode = MX_OPM_CONSERVATIVE;
			return true;
		}
		return false;
	}

}	// namespace mx

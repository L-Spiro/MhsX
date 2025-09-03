#include "MXProcess.h"
#include "../System/MXSystem.h"

#include <EEExpEval.h>

namespace mx {

	CProcess::CProcess() :
		m_dwId( DWINVALID ),
		m_dwOpenProcFlags( 0 ),
		m_pfDetatchCallback( nullptr ) {
		//m_opOpenProcThreadMonitor.ppProcess = this;
	}
	CProcess::~CProcess() {
		Detach();
	}

	// == Functions.
	// Open the given process with the given mode and flags.
	bool CProcess::OpenProc( DWORD _dwId, MX_OPEN_PROC_MODE _opmMode, DWORD _dwFlags ) {
		LSW_ENT_CRIT( m_csCrit );
		Detach();

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
				if ( !OpenProcConservative( _dwId ) ) {
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
			default : { return false; }
		}

		return true;
	}

	// Detach from the current process, if there is one.
	void CProcess::Detach() {
		LSW_ENT_CRIT( m_csCrit );
		m_tProcOpenThread.StopAndWait( 1, INFINITE );
		//Reset();
		m_hProcHandle.Reset();
		m_dwId = DWINVALID;
		if ( m_pfDetatchCallback ) {
			m_pfDetatchCallback( m_pvDetatchParm1, m_uiptrDetatchParm2 );
		}
	}

	// Reads data from an area of memory in a specified process. The entire area to be read must be accessible or the operation fails.
	bool CProcess::ReadProcessMemory( uint64_t _lpBaseAddress, LPVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesRead ) const {
		if ( _lpBaseAddress > MAXSIZE_T ) { return false; }
		LSW_ENT_CRIT( m_csCrit );
		if ( !ProcIsOpened() ) { return false; }
		if ( !ReadProcessMemoryInternal( reinterpret_cast<LPCVOID>(_lpBaseAddress), _lpBuffer, _nSize, _lpNumberOfBytesRead ) ) {
			if ( m_opmMode == CProcess::MX_OPM_CONSERVATIVE ) {
				// Add PROCESS_VM_READ and try again.
				if ( !MX_CHECK_FLAGS_EQ( m_dwOpenProcFlags, PROCESS_VM_READ ) ) {
					if ( const_cast<CProcess *>(this)->OpenProcInternal( m_dwId, m_dwOpenProcFlags | PROCESS_VM_READ ) ) {
						// Try again.
						return ReadProcessMemory( _lpBaseAddress, _lpBuffer, _nSize, _lpNumberOfBytesRead );
					}
				}
			}
			return false;
		}
		return true;
	}

	

	// Reads data from an area of memory in the current process. The entire area to be read must be accessible or the operation fails.
	// Preprocesses the data (applies byteswapping), which means an area larger than the requested size must be read.  _sBufferOffset returns the offset into _vBuffer where the requested data is actually stored.
	bool CProcess::ReadProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, std::vector<uint8_t> &_vBuffer, SIZE_T _nSize, size_t &_sBufferOffset, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesRead ) const {
		if MX_UNLIKELY( _ui64BaseAddress > MAXSIZE_T ) { return false; }
		
		try {
			uint32_t uiAlignment;
			switch ( _bsSwap ) {
				case CUtilities::MX_BS_NONE : {
					_vBuffer.resize( _nSize );
					_sBufferOffset = 0;
					return ReadProcessMemory( _ui64BaseAddress,
						_vBuffer.data(), _vBuffer.size(), _lpNumberOfBytesRead );
				}
				case CUtilities::MX_BS_2BYTE : {
					uiAlignment = 2;
					break;
				}
				case CUtilities::MX_BS_4BYTE : {
					uiAlignment = 4;
					break;
				}
				default : {
					uiAlignment = 8;
					break;
				}
			}
			uint64_t uiDataStart = _ui64BaseAddress, uiAdjustedLen = _nSize;
			CUtilities::SnapTo( uiAlignment, uiDataStart, uiAdjustedLen, _sBufferOffset );
			if ( _lpNumberOfBytesRead ) { (*_lpNumberOfBytesRead) = 0; }
			if MX_UNLIKELY( uiAdjustedLen > MAXSIZE_T ) { return false; }

			_vBuffer.resize( static_cast<size_t>(uiAdjustedLen) );
			if ( !ReadProcessMemory( uiDataStart,
				_vBuffer.data(), _vBuffer.size(), _lpNumberOfBytesRead ) ) {
				return false;
			}
			CUtilities::PreprocessByteSwap( _vBuffer.data(), uiAdjustedLen, _bsSwap );
			return true;
		}
		catch ( ... ) { return false; }
	}

	// Reads data from an area of memory in the current process. The entire area to be read must be accessible or the operation fails.
	// Preprocesses the data (applies byteswapping), which means an area larger than the requested size must be read.  _sBufferOffset returns the offset into _vBuffer where the requested data is actually stored.
	bool CProcess::ReadProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, LPVOID _lpvBuffer, SIZE_T _nSize, size_t &_sBufferOffset, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesRead ) const {
		if MX_UNLIKELY( _ui64BaseAddress > MAXSIZE_T ) { return false; }
		
		try {
			uint32_t uiAlignment;
			switch ( _bsSwap ) {
				case CUtilities::MX_BS_NONE : {
					_sBufferOffset = 0;
					return ReadProcessMemory( _ui64BaseAddress,
						_lpvBuffer, _nSize, _lpNumberOfBytesRead );
				}
				case CUtilities::MX_BS_2BYTE : {
					uiAlignment = 2;
					break;
				}
				case CUtilities::MX_BS_4BYTE : {
					uiAlignment = 4;
					break;
				}
				default : {
					uiAlignment = 8;
					break;
				}
			}
			uint64_t uiDataStart = _ui64BaseAddress, uiAdjustedLen = _nSize;
			CUtilities::SnapTo( uiAlignment, uiDataStart, uiAdjustedLen, _sBufferOffset );
			if ( _lpNumberOfBytesRead ) { (*_lpNumberOfBytesRead) = 0; }
			if MX_UNLIKELY( uiAdjustedLen > MAXSIZE_T ) { return false; }

			if ( !ReadProcessMemory( uiDataStart,
				_lpvBuffer, static_cast<SIZE_T>(uiAdjustedLen), _lpNumberOfBytesRead ) ) {
				return false;
			}
			CUtilities::PreprocessByteSwap( _lpvBuffer, uiAdjustedLen, _bsSwap );
			return true;
		}
		catch ( ... ) { return false; }
	}

	// Writes data to an area of memory in a specified process. The entire area to be written to must be accessible or the operation fails.
	bool CProcess::WriteProcessMemory( uint64_t _lpBaseAddress, LPCVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten ) {
		if ( _lpBaseAddress > MAXSIZE_T ) { return false; }
		LSW_ENT_CRIT( m_csCrit );
		if ( !ProcIsOpened() ) { return false; }
		if ( !WriteProcessMemoryInternal( reinterpret_cast<LPVOID>(_lpBaseAddress), _lpBuffer, _nSize, _lpNumberOfBytesWritten ) ) {
			if ( m_opmMode == CProcess::MX_OPM_CONSERVATIVE ) {
				// Add (PROCESS_VM_WRITE | PROCESS_VM_OPERATION) and try again.
				if ( !MX_CHECK_FLAGS_EQ( m_dwOpenProcFlags, PROCESS_VM_WRITE | PROCESS_VM_OPERATION ) ) {
					if ( OpenProcInternal( m_dwId, m_dwOpenProcFlags | (PROCESS_VM_WRITE | PROCESS_VM_OPERATION) ) ) {
						// Try again.
						return WriteProcessMemory( _lpBaseAddress, _lpBuffer, _nSize, _lpNumberOfBytesWritten );
					}
				}
			}
			return false;
		}
		return true;
	}

	// Writes data to an area of memory in the current process.  If the data is preprocessed and misaligned, it could result in up to 3 writes to memory.
	bool CProcess::WriteProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, LPCVOID _lpvBuffer, SIZE_T _nSize, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesWritten ) {
		if MX_UNLIKELY( _ui64BaseAddress > MAXSIZE_T ) { return false; }

		try {
			uint32_t uiAlignment;
			switch ( _bsSwap ) {
				case CUtilities::MX_BS_NONE : {
					return WriteProcessMemory( _ui64BaseAddress,
						_lpvBuffer, _nSize, _lpNumberOfBytesWritten );
				}
				case CUtilities::MX_BS_2BYTE : {
					uiAlignment = 2;
					break;
				}
				case CUtilities::MX_BS_4BYTE : {
					uiAlignment = 4;
					break;
				}
				default : {
					uiAlignment = 8;
					break;
				}
			}

			uint64_t uiDataStart = _ui64BaseAddress, uiAdjustedLen = _nSize;
			size_t sBufferOffset = 0;
			CUtilities::SnapTo( uiAlignment, uiDataStart, uiAdjustedLen, sBufferOffset );
			if ( _lpNumberOfBytesWritten ) { (*_lpNumberOfBytesWritten) = 0; }
			if MX_UNLIKELY( uiAdjustedLen > MAXSIZE_T ) { return false; }

			/*std::vector<uint8_t> vCopy( reinterpret_cast<const uint8_t *>(_lpvBuffer),
				reinterpret_cast<const uint8_t *>(_lpvBuffer) + _nSize );*/
			std::vector<uint8_t> vCopy( uiAdjustedLen );
			std::memcpy( vCopy.data() + sBufferOffset, _lpvBuffer, _nSize );
			CUtilities::PreprocessByteSwap( vCopy.data(), uiAdjustedLen, _bsSwap );
			if ( sBufferOffset == 0 && uiAdjustedLen == _nSize ) {
				// The byte-swapping did not cause disjointing of the data, so it can be copied in one go.
				return WriteProcessMemory( _ui64BaseAddress,
					vCopy.data(), _nSize, _lpNumberOfBytesWritten );
			}

			_ui64BaseAddress -= sBufferOffset;
			// Some disjointing will have occurred.  Create a disjoint map.
			std::vector<uint8_t> vDisjointMap( uiAdjustedLen );
			std::memset( vDisjointMap.data() + sBufferOffset, 0xFF, _nSize );
			CUtilities::PreprocessByteSwap( vDisjointMap.data(), uiAdjustedLen, _bsSwap );
			size_t sCnt = 0;
			size_t sLastIdx = 0;
			SIZE_T sTotalWritten = 0;
			for ( size_t I = 0; I < uiAdjustedLen; ++I ) {
				if ( !vDisjointMap[I] ) {
					// Write whatever we have accumulated up to this point.
					if ( sCnt ) {
						SIZE_T sTmp = 0;
						if ( !WriteProcessMemory( _ui64BaseAddress + sLastIdx,
							vCopy.data() + sLastIdx, sCnt, &sTmp ) ) {
							if ( _lpNumberOfBytesWritten ) { (*_lpNumberOfBytesWritten) = sTotalWritten + sTmp; }
							return false;
						}
						sTotalWritten += sTmp;
					}
					sCnt = 0;
				}
				else {
					if ( !sCnt++ ) { sLastIdx = I; }
				}
			}
			if ( sCnt ) {
				SIZE_T sTmp = 0;
				if ( !WriteProcessMemory( _ui64BaseAddress + sLastIdx,
					vCopy.data() + sLastIdx, sCnt, &sTmp ) ) {
					if ( _lpNumberOfBytesWritten ) { (*_lpNumberOfBytesWritten) = sTotalWritten + sTmp; }
					return false;
				}
				sTotalWritten += sTmp;
			}
			if ( _lpNumberOfBytesWritten ) { (*_lpNumberOfBytesWritten) = sTotalWritten; }
			return true;
		}
		catch ( ... ) { return false; }
	}

	// Changes the protection on a region of committed pages in the virtual address space of a specified process.
	bool CProcess::VirtualProtectEx( LPVOID _lpAddress, SIZE_T _dwSize, DWORD _flNewProtect, PDWORD _lpflOldProtect ) {
		LSW_ENT_CRIT( m_csCrit );
		if ( !ProcIsOpened() ) { return false; }
		if ( !VirtualProtectExInternal( _lpAddress, _dwSize, _flNewProtect, _lpflOldProtect ) ) {
			if ( m_opmMode == CProcess::MX_OPM_CONSERVATIVE ) {
				// Add (PROCESS_VM_OPERATION) and try again.
				if ( !MX_CHECK_FLAGS_EQ( m_dwOpenProcFlags, PROCESS_VM_OPERATION ) ) {
					if ( OpenProcInternal( m_dwId, m_dwOpenProcFlags | (PROCESS_VM_OPERATION) ) ) {
						// Try again.
						return VirtualProtectExInternal( _lpAddress, _dwSize, _flNewProtect, _lpflOldProtect );
					}
				}
			}
			return false;
		}
		return true;
	}

	// Retrieves information about a range of pages within the virtual address space of a specified process.
	bool CProcess::VirtualQueryEx( uint64_t _lpAddress, PMEMORY_BASIC_INFORMATION64 _lpBuffer ) {
		LSW_ENT_CRIT( m_csCrit );
		if ( !ProcIsOpened() ) { return false; }
		if ( !VirtualQueryExInternal( _lpAddress, _lpBuffer ) ) {
			if ( m_opmMode == CProcess::MX_OPM_CONSERVATIVE ) {
				// Add (PROCESS_QUERY_INFORMATION) and try again.
				if ( !MX_CHECK_FLAGS_EQ( m_dwOpenProcFlags, PROCESS_QUERY_INFORMATION ) ) {
					if ( OpenProcInternal( m_dwId, m_dwOpenProcFlags | (PROCESS_QUERY_INFORMATION) ) ) {
						// Try again.
						return VirtualQueryExInternal( _lpAddress, _lpBuffer ) != 0;
					}
				}
			}
			return false;
		}
		return true;
	}

	// Determines whether the specified process is running under WOW64.
	bool CProcess::IsWow64Process() const {
		LSW_ENT_CRIT( m_csCrit );
		if ( !ProcIsOpened() ) { return false; }
		BOOL bRet;
		if ( CSystem::IsWow64Process( m_hProcHandle.hHandle, &bRet ) ) { return bRet; }
		return false;
	}

	// Determines if the process uses a 32-bit addressing space.
	bool CProcess::Is32Bit() const {
		return IsWow64Process() || CSystem::Is32Bit();
	}

	// Gets the full path of the process.  Returns an empty string if there is no process opened.
	CSecureWString CProcess::QueryProcessImageName( DWORD _dwFlags ) const {
		LSW_ENT_CRIT( m_csCrit );
		if ( !ProcIsOpened() ) { return CSecureWString(); }
		CSecureWString swsTmp;
		swsTmp.resize( MAX_PATH );
		size_t stSize = swsTmp.size();
		while ( stSize == swsTmp.size() ) {
			DWORD dwSize = static_cast<DWORD>(swsTmp.size());
			if ( (!CSystem::QueryFullProcessImageNameW( ProcHandle(), _dwFlags, swsTmp.data(), &dwSize ) && dwSize == 0) || swsTmp.size() >= 1024 * 1024 ) {
				return CSecureWString();
			}
			if ( dwSize == static_cast<DWORD>(swsTmp.size()) ) {
				swsTmp.resize( swsTmp.size() + MAX_PATH );
				stSize = swsTmp.size();
			}
			else {
				swsTmp.resize( dwSize );
				break;
			}
		}

		return swsTmp;
	}

	// Gets the base and size of a region given an address.
	bool CProcess::GetChunk( uint64_t _lpAddress, uint64_t &_uiBaseAddress, uint64_t &_uiRegionSize ) {
		// Have to handle cases where the address is out of native range differently.
		if ( !CSystem::AddressIsInNativeMemoryRange( _lpAddress ) ) {
			// For now, abort.
			return false;
		}
		MEMORY_BASIC_INFORMATION64 mbiInfo;
		if ( !VirtualQueryEx( _lpAddress, &mbiInfo ) ) { return false; }
		_uiBaseAddress = mbiInfo.BaseAddress;
		_uiRegionSize = mbiInfo.RegionSize;
		return true;
	}

	// Resets all assocations with the current process.
	void CProcess::Reset() {
		m_tProcOpenThread.StopAndWait( 1, INFINITE );
		m_tProcOpenThread.ExitThread( 0 );
	}

	// Pauses the target process.
	LONG CProcess::SuspendProcess() const {
		return CSystem::NtSuspendProcess( m_hProcHandle.hHandle );
	}

	// Resume the target process.
	LONG CProcess::ResumeProcess() const {
		return CSystem::NtResumeProcess( m_hProcHandle.hHandle );
	}

	// Sets the detatch callback.
	void CProcess::SetDetatchCallback( PfDetatchCallback _pfFunc, void * _pvParm1,  uintptr_t _uiptrParm2 ) {
		m_pfDetatchCallback = _pfFunc;
		m_pvDetatchParm1 = _pvParm1;
		m_uiptrDetatchParm2 = _uiptrParm2;
	}

	// Internal open process.
	bool CProcess::OpenProcInternal( DWORD _dwId, DWORD _dwFlags ) {
		LSW_ENT_CRIT( m_csCrit );
		if ( m_dwId == _dwId && m_dwOpenProcFlags == _dwFlags ) { return true; }
		// If opening with the new flags fails, keep old handle.
		HANDLE hTemp = CSystem::OpenProcess( _dwFlags | SYNCHRONIZE, FALSE, _dwId );
		if ( LSW_HANDLE::Valid( hTemp ) ) {
			m_hProcHandle.Reset();
			m_hProcHandle.hHandle = hTemp;
			m_dwId = _dwId;
			m_dwOpenProcFlags = _dwFlags;

			DWORD dwVal = m_tProcOpenThread.StopAndWait( 1, INFINITE );

			m_opOpenProcThreadMonitor.hHandle = hTemp;
			::InterlockedExchange( &m_opOpenProcThreadMonitor.aAtom, 0 );
			m_tProcOpenThread.CreateThread( ThreadProc, &m_opOpenProcThreadMonitor, &m_opOpenProcThreadMonitor.aAtom );
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

	// Reads data from an area of memory in a specified process. The entire area to be read must be accessible or the operation fails.
	bool CProcess::ReadProcessMemoryInternal( LPCVOID _lpBaseAddress, LPVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesRead ) const {
		return CSystem::ReadProcessMemory( m_hProcHandle.hHandle, _lpBaseAddress, _lpBuffer, _nSize, _lpNumberOfBytesRead ) != 0;
	}

	// Writes data to an area of memory in a specified process. The entire area to be written to must be accessible or the operation fails.
	bool CProcess::WriteProcessMemoryInternal( LPVOID _lpBaseAddress, LPCVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten ) {
		return CSystem::WriteProcessMemory( m_hProcHandle.hHandle, _lpBaseAddress, _lpBuffer, _nSize, _lpNumberOfBytesWritten ) != 0;
	}

	// Changes the protection on a region of committed pages in the virtual address space of a specified process.
	bool CProcess::VirtualProtectExInternal( LPVOID _lpAddress, SIZE_T _dwSize, DWORD _flNewProtect, PDWORD _lpflOldProtect ) {
		return CSystem::VirtualProtectEx( m_hProcHandle.hHandle, _lpAddress, _dwSize, _flNewProtect, _lpflOldProtect );
	}

	// Retrieves information about a range of pages within the virtual address space of a specified process.
	size_t CProcess::VirtualQueryExInternal( uint64_t _lpAddress, PMEMORY_BASIC_INFORMATION64 _lpBuffer ) {
		if ( !_lpBuffer ) { return 0; }

		// Detect a range that can't be accessed with the Win32 API:
		if ( !CSystem::AddressIsInNativeMemoryRange( _lpAddress ) ) {
			return 0;
		}

		// Otherwise, the behavior is:
		// Only a 64-bit call handles MEMORY_BASIC_INFORMATION64 correctly, so use that as a starting point.
		SIZE_T sSize = CSystem::VirtualQueryEx( m_hProcHandle.hHandle, reinterpret_cast<LPCVOID>(_lpAddress), reinterpret_cast<PMEMORY_BASIC_INFORMATION>(_lpBuffer), sizeof( MEMORY_BASIC_INFORMATION64 ) );
		if ( sSize == sizeof( MEMORY_BASIC_INFORMATION64 ) ) {
			// Done.
			return sSize;
		}
		if ( sSize == sizeof( MEMORY_BASIC_INFORMATION32 ) ) {
			// In 32-bit Windows VirtualQueryEx() ignores dwLength and always fills out a 32-bit structure.
			(*_lpBuffer) = CHelpers::MemoryBasicInformation32To64( (*reinterpret_cast<PMEMORY_BASIC_INFORMATION32>(_lpBuffer)) );
			return sSize;
		}

		// In every other case, a full re-call is necessary.
		sSize = CSystem::VirtualQueryEx( m_hProcHandle.hHandle, reinterpret_cast<LPCVOID>(_lpAddress), reinterpret_cast<PMEMORY_BASIC_INFORMATION>(_lpBuffer), sizeof( MEMORY_BASIC_INFORMATION32 ) );
		if ( sSize == sizeof( MEMORY_BASIC_INFORMATION32 ) ) {
			// In 32-bit Windows VirtualQueryEx() ignores dwLength and always fills out a 32-bit structure.
			(*_lpBuffer) = CHelpers::MemoryBasicInformation32To64( (*reinterpret_cast<PMEMORY_BASIC_INFORMATION32>(_lpBuffer)) );
		}
		return sSize;
	}

	// Open-process thread.  Runs for as long as the target process is open.
	DWORD WINAPI CProcess::ThreadProc( LPVOID _lpParameter ) {
		MX_OPEN_PROC * popProc = reinterpret_cast<MX_OPEN_PROC *>(_lpParameter);
		lsw::LSW_THREAD_PRIORITY tpThreadPri( THREAD_PRIORITY_LOWEST );
		while ( ::WaitForSingleObject( popProc->hHandle, 0 ) == WAIT_TIMEOUT ) {
			if ( popProc->aAtom == 1 ) {
				return 0;
			}
			::Sleep( 1 );
		}
		//popProc->ppProcess->Detach();
		DWORD dwVal = ::WaitForSingleObject( popProc->hHandle, 0 );
		::InterlockedExchange( &popProc->aAtom, 1 );
		return 0;
	}

}	// namespace mx

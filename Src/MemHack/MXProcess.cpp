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
	bool CProcess::ReadProcessMemory( uint64_t _ui64BaseAddress, LPVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesRead ) const {
		if ( _ui64BaseAddress > UINTPTR_MAX ) { return false; }
		LSW_ENT_CRIT( m_csCrit );
		if ( !ProcIsOpened() ) { return false; }
		if ( !ReadProcessMemoryInternal( _ui64BaseAddress, _lpBuffer, _nSize, _lpNumberOfBytesRead ) ) {
			if ( m_opmMode == CProcess::MX_OPM_CONSERVATIVE ) {
				// Add PROCESS_VM_READ and try again.
				if ( !MX_CHECK_FLAGS_EQ( m_dwOpenProcFlags, PROCESS_VM_READ ) ) {
					if ( const_cast<CProcess *>(this)->OpenProcInternal( m_dwId, m_dwOpenProcFlags | PROCESS_VM_READ ) ) {
						// Try again.
						return ReadProcessMemory( _ui64BaseAddress, _lpBuffer, _nSize, _lpNumberOfBytesRead );
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
		if MX_UNLIKELY( _ui64BaseAddress > UINTPTR_MAX ) { return false; }
		
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
			if MX_UNLIKELY( uiAdjustedLen > UINTPTR_MAX ) { return false; }

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
		if MX_UNLIKELY( _ui64BaseAddress > UINTPTR_MAX ) { return false; }
		
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
			if MX_UNLIKELY( uiAdjustedLen > UINTPTR_MAX ) { return false; }

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
	bool CProcess::WriteProcessMemory( uint64_t _ui64BaseAddress, LPCVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten ) {
		if ( _ui64BaseAddress > UINTPTR_MAX ) { return false; }
		LSW_ENT_CRIT( m_csCrit );
		if ( !ProcIsOpened() ) { return false; }
		if ( !WriteProcessMemoryInternal( _ui64BaseAddress, _lpBuffer, _nSize, _lpNumberOfBytesWritten ) ) {
			if ( m_opmMode == CProcess::MX_OPM_CONSERVATIVE ) {
				// Add (PROCESS_VM_WRITE | PROCESS_VM_OPERATION) and try again.
				if ( !MX_CHECK_FLAGS_EQ( m_dwOpenProcFlags, PROCESS_VM_WRITE | PROCESS_VM_OPERATION ) ) {
					if ( OpenProcInternal( m_dwId, m_dwOpenProcFlags | (PROCESS_VM_WRITE | PROCESS_VM_OPERATION) ) ) {
						// Try again.
						return WriteProcessMemory( _ui64BaseAddress, _lpBuffer, _nSize, _lpNumberOfBytesWritten );
					}
				}
			}
			return false;
		}
		return true;
	}

	// Writes data to an area of memory in the current process.  If the data is preprocessed and misaligned, it could result in up to 3 writes to memory.
	bool CProcess::WriteProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, LPCVOID _lpvBuffer, SIZE_T _nSize, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesWritten ) {
		if MX_UNLIKELY( _ui64BaseAddress > UINTPTR_MAX ) { return false; }

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
			if MX_UNLIKELY( uiAdjustedLen > UINTPTR_MAX ) { return false; }

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
	bool CProcess::VirtualProtectEx( uint64_t _ui64Address, SIZE_T _dwSize, DWORD _flNewProtect, PDWORD _lpflOldProtect ) {
		LSW_ENT_CRIT( m_csCrit );
		if ( !ProcIsOpened() ) { return false; }
		if ( !VirtualProtectExInternal( _ui64Address, _dwSize, _flNewProtect, _lpflOldProtect ) ) {
			if ( m_opmMode == CProcess::MX_OPM_CONSERVATIVE ) {
				// Add (PROCESS_VM_OPERATION) and try again.
				if ( !MX_CHECK_FLAGS_EQ( m_dwOpenProcFlags, PROCESS_VM_OPERATION ) ) {
					if ( OpenProcInternal( m_dwId, m_dwOpenProcFlags | (PROCESS_VM_OPERATION) ) ) {
						// Try again.
						return VirtualProtectExInternal( _ui64Address, _dwSize, _flNewProtect, _lpflOldProtect );
					}
				}
			}
			return false;
		}
		return true;
	}

	// Sets a given range in the target process to readable.
	bool CProcess::SetReadable( uint64_t _ui64Address, SIZE_T _dwSize, PDWORD _lpflOldProtect ) {
		LSW_ENT_CRIT( m_csCrit );
		if MX_UNLIKELY( !ProcIsOpened() || !_ui64Address || !_dwSize ) { return false; }

		const DWORD dwPageSize = CSystem::GetSystemInfo().dwPageSize;

		// Align the request to page boundaries.
		const uint64_t ui64Base = (_ui64Address / dwPageSize) * dwPageSize;

		const uint64_t ui64EndUnaligned = _ui64Address + static_cast<uint64_t>(_dwSize);
		if ( ui64EndUnaligned < _ui64Address ) { return false; }	// Overflow.

		uint64_t ui64End = ui64EndUnaligned;
		const uint64_t ui64Rem = (ui64End % dwPageSize);
		if ( ui64Rem ) {
			const uint64_t ui64Add = dwPageSize - ui64Rem;
			if ( ui64End > (UINT64_MAX - ui64Add) ) { return false; }
			ui64End += ui64Add;
		}

		// Query the first region and ensure the entire range stays within it (single old-protect value).
		MEMORY_BASIC_INFORMATION64 mbiInfo{};
		if ( !VirtualQueryEx( _ui64Address, &mbiInfo ) ) { return false; }
		if ( mbiInfo.State != MEM_COMMIT ) { return false; }

		const uint64_t ui64RegionBase = static_cast<uint64_t>(mbiInfo.BaseAddress);
		const uint64_t ui64RegionEnd = ui64RegionBase + static_cast<uint64_t>(mbiInfo.RegionSize);
		if ( ui64RegionEnd < ui64RegionBase ) { return false; }	// Overflow.
		if ( ui64Base < ui64RegionBase || ui64End > ui64RegionEnd ) { return false; }

		DWORD dwProt = mbiInfo.Protect;
		if ( (dwProt & (PAGE_GUARD | PAGE_NOACCESS)) ) { return false; }

		// Keep cache-related modifiers, but drop GUARD if present (it will break reads/writes).
		const DWORD dwMods = (dwProt & (PAGE_NOCACHE | PAGE_WRITECOMBINE));
		const DWORD dwAccess = (dwProt & 0xFF);

		// If already writable, nothing to do (but still report old protect for symmetry if asked).
		if ( (dwAccess == PAGE_READWRITE) || (dwAccess == PAGE_EXECUTE_READWRITE) ||
			(dwAccess == PAGE_WRITECOPY) || (dwAccess == PAGE_EXECUTE_WRITECOPY) ) {
			if ( _lpflOldProtect ) { (*_lpflOldProtect) = dwProt; }
			return true;
		}

		// Choose a writable equivalent while preserving whether the page is executable.
		DWORD dwNewAccess = 0;
		switch ( dwAccess ) {
			case PAGE_READONLY :			{ dwNewAccess = PAGE_READWRITE; break; }
			case PAGE_EXECUTE :				{}				MX_FALLTHROUGH
			case PAGE_EXECUTE_READ :		{ dwNewAccess = PAGE_EXECUTE_READWRITE; break; }
			case PAGE_WRITECOPY :			{ dwNewAccess = PAGE_READWRITE; break; }
			case PAGE_EXECUTE_WRITECOPY :	{ dwNewAccess = PAGE_EXECUTE_READWRITE; break; }
			default :						{ return false; }
		}

		const DWORD dwNewProt = (dwNewAccess | dwMods);

		return VirtualProtectEx(
			ui64Base,
			static_cast<SIZE_T>( ui64End - ui64Base ),
			dwNewProt,
			_lpflOldProtect
		) != FALSE;
	}

	// Sets a given range in the target process to writeable.
	bool CProcess::SetWriteable( uint64_t _ui64Address, SIZE_T _stSize, PDWORD _lpflOldProtect ) {
		std::vector<MX_PROT_RANGE> vRanges;
		if ( !SetWriteableRanges( _ui64Address, _stSize, vRanges ) ) { return false; }
		if ( _lpflOldProtect ) {
			(*_lpflOldProtect) = vRanges.size() ? vRanges[0].dwOldProtect : 0;
		}
		return true;
	}

	// Sets the given address range to writeable, returning the previous protections to restore them back to their old states afterward.
	bool CProcess::SetWriteableRanges( uint64_t _ui64Address, SIZE_T _stSize, std::vector<MX_PROT_RANGE> &_vOldProtects ) {
		LSW_ENT_CRIT( m_csCrit );

		try {
			_vOldProtects.clear();

			if MX_UNLIKELY( !ProcIsOpened() || !_ui64Address || !_stSize ) { return false; }

			const uint64_t ui64EndUnaligned = _ui64Address + static_cast<uint64_t>(_stSize);
			if ( ui64EndUnaligned < _ui64Address ) { return false; }	// Overflow.

			const DWORD dwPageSize = CSystem::GetSystemInfo().dwPageSize;
			uint64_t ui64Cur = (_ui64Address / dwPageSize) * dwPageSize;
			const uint64_t ui64End = ((ui64EndUnaligned + (dwPageSize - 1)) / dwPageSize) * dwPageSize;

			// Preflight: walk regions and collect what we'd change.
			while ( ui64Cur < ui64End ) {
				MEMORY_BASIC_INFORMATION64 mbiInfo{};
				if ( !VirtualQueryEx( ui64Cur, &mbiInfo ) ) { return false; }

				const uint64_t ui64RegionBase = static_cast<uint64_t>(mbiInfo.BaseAddress);
				const uint64_t ui64RegionEnd = ui64RegionBase + static_cast<uint64_t>(mbiInfo.RegionSize);
				if ( ui64RegionEnd < ui64RegionBase ) { return false; }	// Overflow.

				if ( mbiInfo.State != MEM_COMMIT ) { return false; }

				const DWORD dwProt = mbiInfo.Protect;
				if ( (dwProt & (PAGE_GUARD | PAGE_NOACCESS)) ) { return false; }

				DWORD dwNewProt = ProtToWriteable( dwProt );
				if ( !dwNewProt ) { return false; }

				const uint64_t ui64ChunkBase = (ui64Cur > ui64RegionBase) ? ui64Cur : ui64RegionBase;
				const uint64_t ui64ChunkEnd = (ui64End < ui64RegionEnd) ? ui64End : ui64RegionEnd;
				if ( ui64ChunkEnd <= ui64ChunkBase ) { return false; }

				// Only record chunks that actually need changing.
				const DWORD dwAccess = (dwProt & 0xFF);
				const bool bAlreadyWriteable =
					(dwAccess == PAGE_READWRITE) || (dwAccess == PAGE_EXECUTE_READWRITE) ||
					(dwAccess == PAGE_WRITECOPY) || (dwAccess == PAGE_EXECUTE_WRITECOPY);

				if ( !bAlreadyWriteable ) {
					MX_PROT_RANGE prThis{};
					prThis.ui64Base = ui64ChunkBase;
					prThis.stSize = static_cast<SIZE_T>(ui64ChunkEnd - ui64ChunkBase);
					prThis.dwOldProtect = dwProt;	// Best-effort; Windows can return different old-protect per call.
					_vOldProtects.push_back( prThis );
				}

				ui64Cur = ui64RegionEnd;
			}

			// Apply changes.
			for ( size_t I = 0; I < _vOldProtects.size(); ++I ) {
				const MX_PROT_RANGE & prThis = _vOldProtects[I];

				MEMORY_BASIC_INFORMATION64 mbiInfo{};
				if ( !VirtualQueryEx( prThis.ui64Base, &mbiInfo ) ) { return false; }

				DWORD dwNewProt = ProtToWriteable( mbiInfo.Protect );
				if ( !dwNewProt ) { return false; }

				DWORD dwOldProt = 0;
				if ( VirtualProtectEx( prThis.ui64Base, prThis.stSize, dwNewProt, &dwOldProt ) == FALSE ) {
					// Roll back anything we already changed.
					for ( size_t J = 0; J < I; ++J ) {
						DWORD dwTmp = 0;
						VirtualProtectEx( _vOldProtects[J].ui64Base, _vOldProtects[J].stSize, _vOldProtects[J].dwOldProtect, &dwTmp );
					}
					return false;
				}

				// Store the exact old protection returned by VirtualProtectEx for accurate restoration.
				_vOldProtects[I].dwOldProtect = dwOldProt;
			}

			return true;
		}
		catch ( ... ) {
			// No page attributes will have been changed at the point where excpetions can be generated.  No need to roll back.
			return false;
		}
	}

	// Restores protections on multiple pages.
	bool CProcess::RestoreProtectRanges( const std::vector<MX_PROT_RANGE> & _vOldProtects ) {
		LSW_ENT_CRIT( m_csCrit );
		if MX_UNLIKELY( !ProcIsOpened() ) { return false; }

		bool bOk = true;
		for ( size_t I = 0; I < _vOldProtects.size(); ++I ) {
			DWORD dwTmp = 0;
			if ( VirtualProtectEx( _vOldProtects[I].ui64Base, _vOldProtects[I].stSize, _vOldProtects[I].dwOldProtect, &dwTmp ) == FALSE ) {
				bOk = false;
			}
		}
		return bOk;
	}

	// Retrieves information about a range of pages within the virtual address space of a specified process.
	bool CProcess::VirtualQueryEx( uint64_t _ui64Address, PMEMORY_BASIC_INFORMATION64 _lpBuffer ) const {
		LSW_ENT_CRIT( m_csCrit );
		if ( !ProcIsOpened() ) { return false; }
		if ( !VirtualQueryExInternal( _ui64Address, _lpBuffer ) ) {
			if ( m_opmMode == CProcess::MX_OPM_CONSERVATIVE ) {
				// Add (PROCESS_QUERY_INFORMATION) and try again.
				if ( !MX_CHECK_FLAGS_EQ( m_dwOpenProcFlags, PROCESS_QUERY_INFORMATION ) ) {
					if ( const_cast<CProcess *>(this)->OpenProcInternal( m_dwId, m_dwOpenProcFlags | (PROCESS_QUERY_INFORMATION) ) ) {
						// Try again.
						return VirtualQueryExInternal( _ui64Address, _lpBuffer ) != 0;
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
	bool CProcess::GetChunk( uint64_t _ui64Address, uint64_t &_uiBaseAddress, uint64_t &_uiRegionSize ) {
		// Have to handle cases where the address is out of native range differently.
		if ( !CSystem::AddressIsInNativeMemoryRange( _ui64Address ) ) {
			// For now, abort.
			return false;
		}
		MEMORY_BASIC_INFORMATION64 mbiInfo;
		if ( !VirtualQueryEx( _ui64Address, &mbiInfo ) ) { return false; }
		_uiBaseAddress = mbiInfo.BaseAddress;
		_uiRegionSize = mbiInfo.RegionSize;
		return true;
	}

	// Gets the chunks that overlap the given range of addresses.  Call within try/catch.
	std::vector<uint64_t> CProcess::GetChunks( uint64_t _ui64StartAddress, uint64_t _ui64EndAddress ) {
		std::vector<uint64_t> vReturn;

		uint64_t ui64Base = _ui64StartAddress, ui64Size;
		do {
			if ( !GetChunk( _ui64StartAddress, ui64Base, ui64Size ) ) { return vReturn; }
			vReturn.push_back( ui64Base );
			ui64Base += ui64Size;
		}
		while ( ui64Base < _ui64EndAddress );
		return vReturn;
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

	/**
	 * \brief Returns true if the page containing the address is committed and readable (not GUARD/NOACCESS).
	 *
	 * \param _ui64Addr Address to test.
	 * \return true if the page looks readable by protection flags; false otherwise.
	 *
	 * \note This is a best-effort check. Protections can change between the query and use.
	 */
	bool CProcess::IsReadableByQuery( uint64_t _ui64Addr ) const {
		LSW_ENT_CRIT( m_csCrit );
		if MX_UNLIKELY( !ProcIsOpened() || !_ui64Addr ) { return false; }

		uint64_t ui64Page = _ui64Addr / CSystem::GetSystemInfo().dwPageSize * CSystem::GetSystemInfo().dwPageSize;
		if ( ui64Page == m_ui64LastIsReadablePage ) { return m_bLastIsReadableResult; }

		m_ui64LastIsReadablePage = ui64Page;
		m_bLastIsReadableResult = false;

		MEMORY_BASIC_INFORMATION64 mbiInfo{};
		if ( !VirtualQueryEx( _ui64Addr, &mbiInfo ) ) { return false; }
		if ( mbiInfo.State != MEM_COMMIT ) { return false; }

		const DWORD dwProt = mbiInfo.Protect;
		if ( (dwProt & (PAGE_GUARD | PAGE_NOACCESS)) ) { return false; }

		m_bLastIsReadableResult = ProtIsReadable( dwProt );
		return m_bLastIsReadableResult;
	}

	/**
	 * \brief Probes a single remote address by reading 1 byte.
	 *
	 * \param _ui64Addr Remote virtual address to test.
	 * \return true if a 1-byte read succeeds; false otherwise.
	 *
	 * \note Use a 64-bit build for 64-bit targets to avoid address truncation.
	 */
	bool CProcess::IsAddressReadable( uint64_t _ui64Addr ) const {
		LSW_ENT_CRIT( m_csCrit );
		if MX_UNLIKELY( !ProcIsOpened() || !_ui64Addr ) { return false; }

		uint64_t ui64Page = _ui64Addr / CSystem::GetSystemInfo().dwPageSize * CSystem::GetSystemInfo().dwPageSize;
		if ( ui64Page == m_ui64LastIsReadablePage ) { return m_bLastIsReadableResult; }

		m_ui64LastIsReadablePage = ui64Page;

		BYTE bByte;
		SIZE_T stRead = 0;
		m_bLastIsReadableResult = (ReadProcessMemory( _ui64Addr, &bByte, sizeof( bByte ), &stRead ) != FALSE) && (stRead == sizeof( bByte ));
		return m_bLastIsReadableResult;
	}

	/**
	 * \brief Returns true if the page containing the address is committed and writable (not GUARD/NOACCESS).
	 *
	 * \param _ui64Addr Remote address to test (single pointer).
	 * \return Returns true if the containing page is MEM_COMMIT and writable by protection flags; false otherwise.
	 *
	 * \note When State != MEM_COMMIT, Protect/Type are undefined and must be ignored.
	 * \note PAGE_WRITECOPY (and EXECUTE_WRITECOPY) indicate copy-on-write; the first write creates a private copy in the target.
	 *       From a tool perspective, they still count as writable.
	 */
	bool CProcess::IsWritableByQuery( uint64_t _ui64Addr ) const {
		LSW_ENT_CRIT( m_csCrit );
		if MX_UNLIKELY( !ProcIsOpened() || !_ui64Addr ) { return false; }

		MEMORY_BASIC_INFORMATION64 mbiInfo{};
		if ( !VirtualQueryEx( _ui64Addr, &mbiInfo ) ) {
			return false;
		}

		if ( mbiInfo.State != MEM_COMMIT ) { return false; }
		if ( (mbiInfo.Protect & PAGE_GUARD) || (mbiInfo.Protect & PAGE_NOACCESS) ) { return false; }

		return ProtIsWritable( mbiInfo.Protect );
	}

	/**
	 * \brief Returns true if the page containing the address is committed and executable.
	 *
	 * \param _ui64Addr Remote virtual address to test (single pointer).
	 * \return true if the page is MEM_COMMIT and executable; false otherwise.
	 *
	 * \note When State != MEM_COMMIT, Protect/Type are undefined and must be ignored.
	 * \note Control Flow Guard (CFG) may still block indirect calls/jumps even on executable pages.
	 */
	bool CProcess::IsExecutableByQuery( uint64_t _ui64Addr ) const {
		LSW_ENT_CRIT( m_csCrit );
		if MX_UNLIKELY( !ProcIsOpened() || !_ui64Addr ) { return false; }

		uint64_t ui64Page = _ui64Addr / CSystem::GetSystemInfo().dwPageSize * CSystem::GetSystemInfo().dwPageSize;
		if ( ui64Page == m_ui64LastIsExecuteablePage ) { return m_bLastIsExecuteableResult; }

		m_ui64LastIsExecuteablePage = ui64Page;
		m_bLastIsExecuteableResult = false;

		MEMORY_BASIC_INFORMATION64 mbiInfo{};
		SIZE_T sSize = VirtualQueryEx( _ui64Addr, &mbiInfo );
		if ( !sSize ) { return false; }

		if ( mbiInfo.State != MEM_COMMIT ) { return false; }
		if ( mbiInfo.Protect & PAGE_GUARD ) { return false; }
		if ( mbiInfo.Protect & PAGE_NOACCESS ) { return false; }

		m_bLastIsExecuteableResult = ProtIsExecutable( mbiInfo.Protect );
		return m_bLastIsExecuteableResult;
	}

	/**
	 * \brief Reads the image base from the target's PEB (robust path).
	 *
	 * \param _hProc Handle to target process opened with PROCESS_QUERY_INFORMATION (or PROCESS_QUERY_LIMITED_INFORMATION) | PROCESS_VM_READ.
	 * \return Base address of the main module on success; nullptr on failure.
	 *
	 * \note Same-bitness path. For cross-bitness (32-bit tool Å® 64-bit target), use the WOW64
	 *       variants (NtWow64QueryInformationProcess64 + NtWow64ReadVirtualMemory64) or build x64.
	 */
	uint64_t CProcess::GetMainModuleBase_PEB() const {
		LSW_ENT_CRIT( m_csCrit );
		if MX_UNLIKELY( !ProcIsOpened() ) { return false; }

		PROCESS_BASIC_INFORMATION_LOCAL pbilInfo{};
		NTSTATUS stStatus = CSystem::NtQueryInformationProcess( m_hProcHandle.hHandle, ProcessBasicInformation, &pbilInfo, sizeof( pbilInfo ), nullptr );
		if ( stStatus < 0 || !pbilInfo.PebBaseAddress ) { return 0; }

		// The ImageBaseAddress is at PEB + offset 0x10 on both x86/x64, but we read it generically.
		// Define a tiny struct with just enough layout to reach ImageBaseAddress.
		struct PEB_MIN {
			BYTE        Reserved[0x10];
			PVOID       ImageBaseAddress;
		} pebMin;

		SIZE_T stRead = 0;
		if ( !ReadProcessMemory( static_cast<uint64_t>(reinterpret_cast<uintptr_t>(pbilInfo.PebBaseAddress)), &pebMin, sizeof( pebMin ), &stRead ) || stRead < sizeof( pebMin ) ) { return 0; }
		return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(pebMin.ImageBaseAddress));
	}

	/**
	 * \brief Gets the base address of the main module using ToolHelp (easy path).
	 *
	 * \param _dwPid Target process ID.
	 * \return Base address of the main module on success; nullptr on failure.
	 *
	 * \note Works when caller and target have compatible bitness (x64Å®x64 or x86Å®x86).
	 *       A 32-bit caller cannot enumerate 64-bit modules of a 64-bit target.
	 */
	uint64_t CProcess::GetMainModuleBase_Toolhelp( DWORD _dwPid ) {
		LSW_HANDLE hSnap = CSystem::CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _dwPid );
		if ( !hSnap.Valid() ) { return 0; }

		MODULEENTRY32W meEntry{};
		meEntry.dwSize = sizeof( meEntry );
		if ( CSystem::Module32FirstW( hSnap.hHandle, &meEntry ) ) {
			return reinterpret_cast<uint64_t>(meEntry.modBaseAddr);	// First module is the main module.
		}
		return 0;
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
	bool CProcess::ReadProcessMemoryInternal( uint64_t _ui64Address, LPVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesRead ) const {
		if ( _ui64Address > UINTPTR_MAX ) { return false; }
		return CSystem::ReadProcessMemory( m_hProcHandle.hHandle, reinterpret_cast<LPCVOID>(static_cast<uintptr_t>(_ui64Address)), _lpBuffer, _nSize, _lpNumberOfBytesRead ) != 0;
	}

	// Writes data to an area of memory in a specified process. The entire area to be written to must be accessible or the operation fails.
	bool CProcess::WriteProcessMemoryInternal( uint64_t _ui64Address, LPCVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten ) {
		if ( _ui64Address > UINTPTR_MAX ) { return false; }
		return CSystem::WriteProcessMemory( m_hProcHandle.hHandle, reinterpret_cast<LPVOID>(static_cast<uintptr_t>(_ui64Address)), _lpBuffer, _nSize, _lpNumberOfBytesWritten ) != 0;
	}

	// Changes the protection on a region of committed pages in the virtual address space of a specified process.
	bool CProcess::VirtualProtectExInternal( uint64_t _ui64Address, SIZE_T _dwSize, DWORD _flNewProtect, PDWORD _lpflOldProtect ) {
		if ( _ui64Address > UINTPTR_MAX ) { return false; }
		return CSystem::VirtualProtectEx( m_hProcHandle.hHandle, reinterpret_cast<LPVOID>(static_cast<uintptr_t>(_ui64Address)), _dwSize, _flNewProtect, _lpflOldProtect );
	}

	// Retrieves information about a range of pages within the virtual address space of a specified process.
	size_t CProcess::VirtualQueryExInternal( uint64_t _lpAddress, PMEMORY_BASIC_INFORMATION64 _lpBuffer ) const {
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

#include "MXHexEditorProcess.h"


namespace mx {

	CHexEditorProcess::CHexEditorProcess() {
	}
	CHexEditorProcess::~CHexEditorProcess() {
	}

	// == Functions.
	// Sets the target process.
	bool CHexEditorProcess::SetProcess( DWORD _dwId, CProcess::MX_OPEN_PROC_MODE _opmMode, DWORD _dwFlags ) {
		return m_pProcess.OpenProc( _dwId, _opmMode, _dwFlags );
	}

	// Resets the object back to scratch.
	void CHexEditorProcess::Reset() {
		m_pProcess.Reset();
	}

	// Read from the given data stream.
	bool CHexEditorProcess::Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const {
		const auto siInfo = mx::CSystem::GetSystemInfo();
		// Read in chunks of siInfo.dwPageSize.
		
		try {
			uint8_t * pui8Dst = nullptr;
			_bDst.resize( _sSize );
			pui8Dst = _bDst.data();


			while ( _sSize ) {
				uint64_t ui64Start = _ui64Addr / siInfo.dwPageSize * siInfo.dwPageSize;
				uint64_t ui64End = (_ui64Addr / siInfo.dwPageSize + 1) * siInfo.dwPageSize;
				_ui64Addr = std::max( _ui64Addr, ui64Start );
				size_t sSize = size_t( ui64End - _ui64Addr );
				sSize = std::min( sSize, _sSize );

				if ( !m_pProcess.ReadProcessMemory( _ui64Addr, pui8Dst, sSize, nullptr ) ) {
					std::memset( pui8Dst, 0xFF, sSize );
				}
				_ui64Addr += sSize;
				pui8Dst += sSize;
				_sSize -= sSize;
			}
			return true;
		}
		catch ( ... ) {
			// Failed to allocate.
			return false;
		}/*
		size_t sOff;
		return m_pProcess.ReadProcessMemory_PreProcessed( _ui64Addr, _bDst, _sSize, sOff, CUtilities::MX_BS_NONE, NULL );*/
	}

	// Overwrite bytes at a given address.
	bool CHexEditorProcess::Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) {
		const auto siInfo = mx::CSystem::GetSystemInfo();
		// Read in chunks of siInfo.dwPageSize.
		
		try {
			const uint8_t * pui8Src = reinterpret_cast<const uint8_t *>(_pvSrc);


			while ( _sSize ) {
				uint64_t ui64Start = _ui64Addr / siInfo.dwPageSize * siInfo.dwPageSize;
				uint64_t ui64End = (_ui64Addr / siInfo.dwPageSize + 1) * siInfo.dwPageSize;
				_ui64Addr = std::max( _ui64Addr, ui64Start );
				size_t sSize = size_t( ui64End - _ui64Addr );
				sSize = std::min( sSize, _sSize );

				if ( !m_pProcess.WriteProcessMemory( _ui64Addr, pui8Src, sSize, nullptr ) ) {
					// m_pProcess re-opens the process with new permissions, but maybe try changing the writability
					if ( m_pProcess.IsReadableByQuery( _ui64Addr ) /*|| m_pProcess.IsAddressReadable( _ui64Addr )*/ ) {
						std::vector<CProcess::MX_PROT_RANGE> vRestoreMe;
						
						CProcess::MX_PAUSE_RESUME prPauseMe( m_pProcess );
						if ( m_pProcess.SetWriteableRanges( _ui64Addr, sSize, vRestoreMe ) ) {
							m_pProcess.WriteProcessMemory( _ui64Addr, pui8Src, sSize, nullptr );	// Failure doesn’t matter.
							m_pProcess.RestoreProtectRanges( vRestoreMe );							// Restore old flags.
						}
					}
					
				}

				_ui64Addr += sSize;
				pui8Src += sSize;
				_sSize -= sSize;
			}
			return true;
		}
		catch ( ... ) {
			// Failed to allocate.
			return false;
		}
	}

	// Delets bytes at a given address.
	bool CHexEditorProcess::Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted ) {
		_ui64Deleted = 0;
		return false;
	}

	// Performs a Copy operation using the given selection, saved to the given clipboard index.
	bool CHexEditorProcess::Copy( const MX_SELECTION &_sSelection, uint32_t _ui32BytesPerRow, size_t _sClipIdx, const MX_COPYAS &_csParms, CSecureWString &_swsMsg ) {
		return false;
	}

	// Gets the size of the data source.
	uint64_t CHexEditorProcess::Size() const {
		return reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMaximumApplicationAddress);
	}

	// Default starting address.
	uint64_t CHexEditorProcess::DefaultAddress() const {
		return Process().GetMainModuleBase_PEB();
	}

	// Does the given range of addresses contain anything real-time?
	bool CHexEditorProcess::RangeContainsRealTime( uint64_t _ui64Start, uint64_t _ui64Total ) const {
		uint64_t ui64End = _ui64Start + _ui64Total;
		for ( uint64_t I = _ui64Start / mx::CSystem::GetSystemInfo().dwPageSize * mx::CSystem::GetSystemInfo().dwPageSize; I < ui64End; I += mx::CSystem::GetSystemInfo().dwPageSize ) {
			if ( Process().IsWritableByQuery( I ) ) { return true; }
		}
		return false;
	}

}	// namespace mx

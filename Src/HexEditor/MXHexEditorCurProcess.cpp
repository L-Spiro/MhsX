#include "MXHexEditorCurProcess.h"


namespace mx {

	CHexEditorCurProcess::CHexEditorCurProcess( CProcess &_pProcess ) :
		CHexEditorInterface(),
		m_pProcess( _pProcess ) {
	}
	CHexEditorCurProcess::~CHexEditorCurProcess() {
	}

	// == Functions.
	// Resets the object back to scratch.
	void CHexEditorCurProcess::Reset() {
		m_pProcess.Reset();
	}

	// Read from the given data stream.
	bool CHexEditorCurProcess::Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const {
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
	bool CHexEditorCurProcess::Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) {
		return false;
	}

	// Delets bytes at a given address.
	bool CHexEditorCurProcess::Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted ) {
		_ui64Deleted = 0;
		return false;
	}

	// Gets the size of the data source.
	uint64_t CHexEditorCurProcess::Size() const {
		return reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMaximumApplicationAddress);
	}

	// Default starting address.
	uint64_t CHexEditorCurProcess::DefaultAddress() const {
		return Process().GetMainModuleBase_PEB();
	}

	// Does the given range of addresses contain anything real-time?
	bool CHexEditorCurProcess::RangeContainsRealTime( uint64_t _ui64Start, uint64_t _ui64Total ) const {
		uint64_t ui64End = _ui64Start + _ui64Total;
		for ( uint64_t I = _ui64Start / mx::CSystem::GetSystemInfo().dwPageSize * mx::CSystem::GetSystemInfo().dwPageSize; I < ui64End; I += mx::CSystem::GetSystemInfo().dwPageSize ) {
			if ( Process().IsWritableByQuery( I ) ) { return true; }
		}
		return false;
	}

}	// namespace mx

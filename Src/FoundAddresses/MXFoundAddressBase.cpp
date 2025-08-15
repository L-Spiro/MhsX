#include "MXFoundAddressBase.h"
#include "../MemHack/MXMemHack.h"


namespace mx {

	// The number of instances of this class.
	std::atomic<size_t> CFoundAddressBase::s_aClassCount = 0;

	// The ID counter.
	std::atomic<size_t> CFoundAddressBase::s_aId = 1;

	CFoundAddressBase::CFoundAddressBase( CMemHack * _pmhMemHack ) :
		m_pmhMemHack( _pmhMemHack ) {
		try {
		}
		catch ( ... ) {
			throw;
		}
		++s_aClassCount;
		m_sId = s_aId++;
	}
	CFoundAddressBase::~CFoundAddressBase() {
		if ( --s_aClassCount == 0 ) {
			s_aId = 1;
		}
	}

	// == Functions.
	// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
	bool CFoundAddressBase::SaveSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary ) const {
		if ( _peJson == nullptr && nullptr == _psBinary ) { return false; }
		try {
			if ( _peJson ) {
			}
			else {
			}
		}
		catch ( ... ) { return false; }
		return true;
	}

	// Loads settings from either a JSON object or a byte buffer.
	bool CFoundAddressBase::LoadSettings( lson::CJson * _pjJson, CStream * _psBinary, uint32_t _ui32Version ) {
		if ( _ui32Version > MX_FOUND_ADDRESS_VERSION ) { return false; }

		return true;
	}

	// Writes to memory (optionally byte-swapping).
	bool CFoundAddressBase::WriteProcessMemory_PreProcessed( CMemHack * _pmhMemHack,
		uint64_t _ui64BaseAddress, const std::vector<std::vector<uint8_t>> &_vBuffer, bool _bContiguous, uint32_t _ui32Stride, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesWritten ) {
		if ( _lpNumberOfBytesWritten ) { (*_lpNumberOfBytesWritten) = 0; }
		if MX_UNLIKELY( !_vBuffer.size() ) {
			return true;
		}
		
		if MX_LIKELY( _bContiguous ) {
			return _pmhMemHack->WriteProcessMemory_PreProcessed( _ui64BaseAddress, _vBuffer[0], _bsSwap, _lpNumberOfBytesWritten );
		}

		SIZE_T sSize = 0;
		for ( size_t I = 0; I < _vBuffer.size(); ++I ) {
			if ( !_pmhMemHack->WriteProcessMemory_PreProcessed( _ui64BaseAddress, _vBuffer[I], _bsSwap, &sSize ) ) {
				if ( _lpNumberOfBytesWritten ) { (*_lpNumberOfBytesWritten) += sSize; }
				return false;
			}

			if ( _lpNumberOfBytesWritten ) { (*_lpNumberOfBytesWritten) += sSize; }
			sSize = 0;
			_ui64BaseAddress += _ui32Stride;
		}
		return true;
	}

}	// namespace mx

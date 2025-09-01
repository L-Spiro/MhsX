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
				MX_JSON_NUMBER( _DEC_S_2ABD43F2_Id, m_sId, _peJson );
				MX_JSON_NUMBER( _DEC_S_3A226579_Parent, m_sParent, _peJson );
				MX_JSON_NUMBER( _DEC_S_A79767ED_Color, (*reinterpret_cast<const uint32_t *>(&m_rgbqColor)), _peJson );
				
				{
					_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
					lson::CJson::CreateStringElement( _DEC_S_FE11D138_Name, ee::CExpEval::ToJsonString( ee::CExpEval::ToUtf8( m_wsName ) ), (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );

					_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
					lson::CJson::CreateArrayElement( _DEC_S_58E1D3EC_Children, (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );
					{
						lson::CJson::LSON_ELEMENT * peArray = _peJson->vObjectMembers[_peJson->vObjectMembers.size()-1].get();
						for ( size_t I = 0; I < m_vChildren.size(); ++I ) {
							MX_JSON_NUMBER( "", m_vChildren[I], peArray );
						}
					}
				}
			}
			else {
				if ( !_psBinary->WriteUi32( m_sId ) ) { return false; }
				if ( !_psBinary->WriteUi32( m_sParent ) ) { return false; }
				if ( !_psBinary->WriteUi32( (*reinterpret_cast<const uint32_t *>(&m_rgbqColor)) ) ) { return false; }

				if ( !_psBinary->WriteStringU16( m_wsName ) ) { return false; }

				if ( !_psBinary->WriteUi32( m_vChildren.size() ) ) { return false; }
				for ( size_t I = 0; I < m_vChildren.size(); ++I ) {
					if ( !_psBinary->WriteUi32( m_vChildren[I] ) ) { return false; }
				}
			}
		}
		catch ( ... ) { return false; }
		return true;
	}

	// Loads settings from either a JSON object or a byte buffer.
	bool CFoundAddressBase::LoadSettings( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, uint32_t _ui32Version, size_t &_sId ) {
		if ( _ui32Version > MX_FOUND_ADDRESS_VERSION ) { return false; }
		if ( nullptr == _pjJson && _psBinary == nullptr ) { return false; }
		try {
			if ( _pjJson ) {
				const lson::CJsonContainer::LSON_JSON_VALUE * pjvVal;
				MX_JSON_GET_NUMBER( _DEC_S_2ABD43F2_Id, _sId, size_t, pjvVal, _pjJson );
				MX_JSON_GET_NUMBER( _DEC_S_3A226579_Parent, m_sParent, size_t, pjvVal, _pjJson );
				MX_JSON_GET_NUMBER( _DEC_S_A79767ED_Color,  (*reinterpret_cast<uint32_t *>(&m_rgbqColor)), uint32_t, pjvVal, _pjJson );

				MX_JSON_GET_STRING( _DEC_S_FE11D138_Name, m_wsName, pjvVal, _pjJson );

				const lson::CJsonContainer::LSON_JSON_VALUE * pjvKeys = _pjcContainer->GetMemberByName( (*_pjJson), _DEC_S_58E1D3EC_Children );
				if ( !pjvKeys || pjvKeys->vtType != lson::CJsonContainer::LSON_VT_ARRAY ) { return false; }

				for ( size_t I = 0; I < pjvKeys->vArray.size(); ++I ) {
					const lson::CJsonContainer::LSON_JSON_VALUE & jvArrayVal = _pjcContainer->GetValue( pjvKeys->vArray[I] );
					if ( jvArrayVal.vtType == lson::CJsonContainer::LSON_VT_DECIMAL ) {
						m_vChildren.push_back( size_t( std::round( jvArrayVal.u.dDecimal ) ) );
					}
				}
			}
			else {
				uint32_t ui32Tmp;
				if ( !_psBinary->ReadUi32( ui32Tmp ) ) { return false; }
				_sId = size_t( ui32Tmp );
				if ( !_psBinary->ReadUi32( ui32Tmp ) ) { return false; }
				m_sParent = size_t( ui32Tmp );
				if ( _ui32Version >= 2 ) {
					if ( !_psBinary->ReadUi32( (*reinterpret_cast<uint32_t *>(&m_rgbqColor)) ) ) { return false; }
				}

				CSecureWString * pwsStr = reinterpret_cast<CSecureWString *>(&m_wsName);
				if ( !_psBinary->ReadStringU16( (*pwsStr) ) ) { return false; }

				size_t sTotal;
				if ( !_psBinary->ReadUi32( ui32Tmp ) ) { return false; }
				sTotal = size_t( ui32Tmp );
				for ( size_t I = 0; I < sTotal; ++I ) {
					if ( !_psBinary->ReadUi32( ui32Tmp ) ) { return false; }
					m_vChildren.push_back( size_t( ui32Tmp ) );
				}
			}
		}
		catch ( ... ) { return false; }
		return true;
	}

	// Remaps ID's after a file load.
	void CFoundAddressBase::RemapIds( const std::map<size_t, size_t> &_mMap ) {
		// Missing entries is not a case for failure.  Items that used to have parents can be used to a file without their parents (and children etc.)
		if ( m_sParent ) {
			auto iHasit = _mMap.find( m_sParent );
			if ( iHasit != _mMap.end() ) {
				m_sParent = iHasit->second;
			}
			else {
				m_sParent = 0;
			}
		}

		for ( auto I = m_vChildren.size(); I--; ) {
			auto iHasit = _mMap.find( m_vChildren[I] );
			if ( iHasit != _mMap.end() ) {
				m_vChildren[I] = iHasit->second;
			}
			else {
				m_vChildren.erase( m_vChildren.begin() + I );
			}
		}
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

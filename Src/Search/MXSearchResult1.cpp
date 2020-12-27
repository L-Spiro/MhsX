#include "MXSearchResult1.h"

namespace mx {

	CSearchResult1::CSearchResult1() :
		m_ui64Total( 0 ) {
	}
	CSearchResult1::~CSearchResult1() {
		Reset();
	}

	// == Functions.
	// Resets the address list to nothing, deallocates everything.
	void CSearchResult1::Reset() {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		for ( auto I = m_vAddressLists.size(); I--; ) {
			delete m_vAddressLists[I];
		}
		m_vAddressLists.clear();
		m_vAddressLists = std::vector<MX_ADDRESS_ARRAY *>();
		m_ui64Total = 0;
	}

	// Add a value.
	bool CSearchResult1::Add( uint64_t _ui64Address, uint8_t * _pui8Value, uint64_t _ui64Size ) {
	 	//lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		size_t sIdx = m_vAddressLists.size() - 1;
		//size_t sAddrSize = UINT32_MAX ? sizeof( uint64_t ) : sizeof( uint32_t );
		size_t sAddrSize;
		if ( _ui64Address <= (1ULL << (3ULL * 8ULL)) - 1ULL ) {
			sAddrSize = 3;
		}
		else if ( _ui64Address <= (1ULL << (4ULL * 8ULL)) - 1ULL ) {
			sAddrSize = 4;
		}
		else if ( _ui64Address <= (1ULL << (6ULL * 8ULL)) - 1ULL ) {
			sAddrSize = 6;
		}
		else { sAddrSize = 8; }
		if ( !m_vAddressLists.size() || m_vAddressLists[sIdx]->ui16SizeOfAddresses != sAddrSize || m_vAddressLists[sIdx]->ui16SizeOfValues != _ui64Size ) {
			if ( !AddAddressList( sAddrSize, _ui64Size ) ) {
				return false;
			}
			sIdx = m_vAddressLists.size() - 1;
		}

		if ( m_vAddressLists[sIdx]->Add( _ui64Address, _pui8Value ) ) {
			++m_ui64Total;
			return true;
		}
		return false;
	}

	// Gets a result by index.  Does not copy the result into a buffer, instead giving back a pointer directly to its internal storage,
	//	which means the caller must finish using the data before the search results are updated in any way.
	bool CSearchResult1::GetResultFast( size_t _stIdx, uint64_t &_ui64Address, const uint8_t * &_pui8Value ) const {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		if ( _stIdx >= m_ui64Total ) { return false; }
		_ui64Address = 0;
		for ( size_t I = 0; I < m_vAddressLists.size(); ++I ) {
			if ( static_cast<size_t>(m_vAddressLists[I]->ui32NumberOfEntries) > _stIdx ) {
				// Found it.
				size_t stOffset = (m_vAddressLists[I]->ui16SizeOfAddresses + m_vAddressLists[I]->ui16SizeOfValues) * _stIdx;
				std::memcpy( &_ui64Address, &m_vAddressLists[I]->pui8Values[stOffset], m_vAddressLists[I]->ui16SizeOfAddresses );
				stOffset += m_vAddressLists[I]->ui16SizeOfAddresses;
				_pui8Value = &m_vAddressLists[I]->pui8Values[stOffset];
				return true;
			}
			_stIdx -= static_cast<size_t>(m_vAddressLists[I]->ui32NumberOfEntries);
		}

		return false;
	}

	// Consolidates the results from another CSearchResult1 object.  Leaves _srResults in a valid state after stealing its pointers. 
	bool CSearchResult1::AppendResults( CSearchResult1 &_srResults ) {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		lsw::CCriticalSection::CEnterCrit ecCrit2( _srResults.m_csCrit );
		for ( size_t I = 0; I < _srResults.m_vAddressLists.size(); ++I ) {
			try {
				m_vAddressLists.push_back( _srResults.m_vAddressLists[I] );
				_srResults.m_vAddressLists[I] = nullptr;
			}
			catch ( const std::bad_alloc /*& _eE*/ ) { return false; }
		}
		m_ui64Total += _srResults.m_ui64Total;
		_srResults.m_ui64Total = 0;
		return true;
	}

	// Adds a new address list to m_vAddressLists.
	bool CSearchResult1::AddAddressList( size_t _sAddrSize, uint64_t _ui64Size ) {
		size_t sIdx = m_vAddressLists.size();
		MX_ADDRESS_ARRAY * parAddMe = new ( std::nothrow ) MX_ADDRESS_ARRAY();
		if ( !parAddMe ) { return false; }
		try {
			m_vAddressLists.push_back( parAddMe );
			m_vAddressLists[sIdx]->ui16SizeOfAddresses = static_cast<uint64_t>(_sAddrSize);
			m_vAddressLists[sIdx]->ui16SizeOfValues = _ui64Size;
			m_vAddressLists[sIdx]->ui32NumberOfEntries = 0;
		}
		catch ( const std::bad_alloc /*& _eE*/ ) {
			delete parAddMe;
			return false;
		}
		return true;
	}

}	// namespace mx

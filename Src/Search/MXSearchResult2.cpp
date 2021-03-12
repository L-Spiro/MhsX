#include "MXSearchResult2.h"

namespace mx {

	CSearchResult2::CSearchResult2() :
		m_ui64Total( 0 ) {
	}
	CSearchResult2::~CSearchResult2() {
		Reset();
	}

	// == Functions.
	// Resets the address list to nothing, deallocates everything.
	void CSearchResult2::Reset() {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		for ( auto I = m_vAddressLists.size(); I--; ) {
			delete m_vAddressLists[I];
		}
		m_vAddressLists.clear();
		m_vAddressLists = std::vector<MX_ADDRESS_ARRAY *>();
		m_ui64Total = 0;
	}

	// Add a value.
	bool CSearchResult2::Add( CInOutInterface * _pProc,
		uint64_t _ui64Address, uint16_t _ui16ItemSize, uint64_t _ui64TotalItems, uint16_t _ui16Align ) {
		// Lock externally.
	 	//lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		MX_ADDRESS_ARRAY * paaArray = new ( std::nothrow ) MX_ADDRESS_ARRAY();
		if ( !paaArray ) { return false; }
		try {
			m_vAddressLists.push_back( paaArray );
		}
		catch ( const std::bad_alloc & /*_eE*/ ) { delete paaArray; return false; }
		if ( !paaArray->Set( _pProc, _ui64Address, _ui16ItemSize, _ui64TotalItems, _ui16Align ) ) {
			delete paaArray;
			m_vAddressLists.pop_back();
			return false;
		}
		m_ui64Total += _ui64TotalItems;
		return true;
	}

	// Gets a result by index.  Does not copy the result into a buffer, instead giving back a pointer directly to its internal storage,
	//	which means the caller must finish using the data before the search results are updated in any way.
	bool CSearchResult2::GetResultFast( size_t _stIdx, uint64_t &_ui64Address, const uint8_t * &_pui8Value ) const {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		if ( _stIdx >= m_ui64Total ) { return false; }
		for ( size_t I = 0; I < m_vAddressLists.size(); ++I ) {
			if ( m_vAddressLists[I]->ui64NumberOfEntries > static_cast<uint64_t>(_stIdx) ) {
				// Found it.
				size_t stOffset = m_vAddressLists[I]->ui16Align * _stIdx;
				_ui64Address = m_vAddressLists[I]->ui64Address + stOffset;
				_pui8Value = &m_vAddressLists[I]->pui8Values[stOffset];
				return true;
			}
			_stIdx -= static_cast<size_t>(m_vAddressLists[I]->ui64NumberOfEntries);
		}

		return false;
	}

}	// namespace mx

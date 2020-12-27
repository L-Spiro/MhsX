#include "MXSearchResultRef.h"

namespace mx {

	CSearchResultRef::CSearchResultRef() :
		m_ui64Total( 0 ) {
	}
	CSearchResultRef::~CSearchResultRef() {
		Reset();
	}

	// == Functions.
	// Resets the address list to nothing, deallocates everything.
	void CSearchResultRef::Reset() {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		for ( auto I = m_vLanes.size(); I--; ) {
			delete m_vLanes[I];
		}
		m_vLanes.clear();
		m_vLanes = std::vector<MX_LANE *>();
		m_ui64Total = 0;
	}

	// Add a reference to a value.
	bool CSearchResultRef::Add( uint64_t _ui64Address, uint64_t _ui64Size, uint16_t _ui16Data ) {
		if ( !m_vLanes.size() ) {
			MX_LANE * plLane = new( std::nothrow ) MX_LANE();
			if ( !plLane ) { return false; }
			try { m_vLanes.push_back( plLane ); }
			catch ( const std::bad_alloc & ) { return false; }
			plLane->uiBaseAddr = _ui64Address;
			try { plLane->vAddresses.push_back( { static_cast<uint32_t>(_ui64Size), 0, _ui16Data } ); }
			catch ( const std::bad_alloc & ) { m_vLanes.pop_back(); return false; }
			++m_ui64Total;
			return true;
		}
		MX_LANE * plLane;
		size_t sIdx = m_vLanes.size() - 1;
		if ( _ui64Address <= (m_vLanes[sIdx]->uiBaseAddr + 0xFFFF) ) {
			// Address is close enough.
			plLane = m_vLanes[sIdx];
		}
		else {
			plLane = new( std::nothrow ) MX_LANE();
			if ( !plLane ) { return false; }
			try { m_vLanes.push_back( plLane ); }
			catch ( const std::bad_alloc & ) { return false; }
			plLane->uiBaseAddr = _ui64Address;
		}
		try { plLane->vAddresses.push_back( { static_cast<uint32_t>(_ui64Size), static_cast<uint16_t>(_ui64Address - plLane->uiBaseAddr), _ui16Data } ); }
		catch ( const std::bad_alloc & ) { return false; }
		++m_ui64Total;
		return true;
	}

	// Gets a result by index.  Does not copy the result into a buffer, instead giving back a pointer directly to its internal storage,
	//	which means the caller must finish using the data before the search results are updated in any way.
	bool CSearchResultRef::GetResultFast( size_t _stIdx, uint64_t &_ui64Address, const uint8_t * &_pui8Value ) const {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		if ( _stIdx >= m_ui64Total ) { return false; }
		for ( size_t I = 0; I < m_vLanes.size(); ++I ) {
			if ( m_vLanes[I]->vAddresses.size() > static_cast<uint64_t>(_stIdx) ) {
				// Found it.
				_ui64Address = m_vLanes[I]->vAddresses[_stIdx].uiAddr + m_vLanes[I]->uiBaseAddr;
				_pui8Value = reinterpret_cast<const uint8_t *>(&m_vLanes[I]->vAddresses[_stIdx]);
				return true;
			}
			_stIdx -= static_cast<size_t>(m_vLanes[I]->vAddresses.size());
		}

		return false;
	}

	// Consolidates the results from another CSearchResult1 object.  Leaves _srResults in a valid state after stealing its pointers. 
	bool CSearchResultRef::AppendResults( CSearchResultRef &_srrResults ) {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		lsw::CCriticalSection::CEnterCrit ecCrit2( _srrResults.m_csCrit );
		try {
			for ( size_t I = 0; I < _srrResults.m_vLanes.size(); ++I ) {
				m_vLanes.push_back( _srrResults.m_vLanes[I] );
				_srrResults.m_vLanes[I] = nullptr;
			}
		}
		catch ( const std::bad_alloc /*& _eE*/ ) { return false; }
		m_ui64Total += _srrResults.m_ui64Total;
		_srrResults.m_ui64Total = 0;
		return true;
	}

}	// namespace mx

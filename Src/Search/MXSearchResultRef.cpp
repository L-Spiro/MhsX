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
			catch ( const std::bad_alloc & ) { delete plLane; return false; }
			plLane->uiBaseAddr = _ui64Address;
			try { plLane->vAddresses.push_back( { static_cast<uint32_t>(_ui64Size), 0, _ui16Data } ); }
			catch ( const std::bad_alloc & ) { m_vLanes.pop_back(); delete plLane; return false; }
			++m_ui64Total;
			return true;
		}
		MX_LANE * plLane;
		size_t sIdx = m_vLanes.size() - 1;
		bool bDeleteMe = false;
		if ( _ui64Address <= (m_vLanes[sIdx]->uiBaseAddr + 0xFFFF) ) {
			// Address is close enough.
			plLane = m_vLanes[sIdx];
		}
		else {
			plLane = new( std::nothrow ) MX_LANE();
			bDeleteMe = true;
			if ( !plLane ) { return false; }
			try { m_vLanes.push_back( plLane ); }
			catch ( const std::bad_alloc & ) { delete plLane; return false; }
			plLane->uiBaseAddr = _ui64Address;
		}
		try { plLane->vAddresses.push_back( { static_cast<uint32_t>(_ui64Size), static_cast<uint16_t>(_ui64Address - plLane->uiBaseAddr), _ui16Data } ); }
		catch ( const std::bad_alloc & ) {
			if ( bDeleteMe ) { delete plLane; }
			return false;
		}
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

	// Gets a result under the assumption that the object is locked.
	bool CSearchResultRef::GetResultFast_Locked( size_t _stIdx, uint64_t &_ui64Address, const uint8_t * &_pui8Value,
		size_t &_stSearchIndex, uint64_t &_ui64SearchAccumulator ) const {
		if ( _stIdx >= m_ui64Total ) { return false; }
		_stIdx -= _ui64SearchAccumulator;
		for ( size_t I = _stSearchIndex; I < m_vLanes.size(); ++I ) {
			if ( m_vLanes[I]->vAddresses.size() > static_cast<uint64_t>(_stIdx) ) {
				// Found it.
				_ui64Address = m_vLanes[I]->vAddresses[_stIdx].uiAddr + m_vLanes[I]->uiBaseAddr;
				_pui8Value = reinterpret_cast<const uint8_t *>(&m_vLanes[I]->vAddresses[_stIdx]);
				_stSearchIndex = I;
				return true;
			}
			_stIdx -= static_cast<size_t>(m_vLanes[I]->vAddresses.size());
			_ui64SearchAccumulator += m_vLanes[I]->vAddresses.size();
		}

		return false;
	}

	// Gets the address of a result under the assumption that the object is locked.
	bool CSearchResultRef::GetResultFast_Locked( size_t _stIdx, uint64_t &_ui64Address,
		size_t &_stSearchIndex, uint64_t &_ui64SearchAccumulator ) const {
		if ( _stIdx >= m_ui64Total ) { return false; }
		_stIdx -= _ui64SearchAccumulator;
		for ( size_t I = _stSearchIndex; I < m_vLanes.size(); ++I ) {
			if ( m_vLanes[I]->vAddresses.size() > static_cast<uint64_t>(_stIdx) ) {
				// Found it.
				_ui64Address = m_vLanes[I]->vAddresses[_stIdx].uiAddr + m_vLanes[I]->uiBaseAddr;
				_stSearchIndex = I;
				return true;
			}
			_stIdx -= static_cast<size_t>(m_vLanes[I]->vAddresses.size());
			_ui64SearchAccumulator += m_vLanes[I]->vAddresses.size();
		}

		return false;
	}

	// Gets the first and last address.  The last address is the first address that is outside the range of the final item in the list, meaning the final item's address plus the size of that item.
	//	In other words, these addresses represent the entire range of addresses that fully encapsulates all values in the results.
	bool CSearchResultRef::GetEncapsulatingAddresses( uint64_t &_ui64Start, uint64_t &_ui64End ) const {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		if ( !m_ui64Total ) { return false; }
		_ui64Start = m_vLanes[0]->vAddresses[0].uiAddr + m_vLanes[0]->uiBaseAddr;
		size_t sIdx = m_vLanes.size() - 1;
		size_t sIdx2 = m_vLanes[sIdx]->vAddresses.size() - 1;
		_ui64End = m_vLanes[sIdx]->vAddresses[sIdx2].uiAddr + m_vLanes[sIdx]->uiBaseAddr;
		_ui64End += m_vLanes[sIdx]->vAddresses[sIdx2].uiSize;
		return true;
	}

	// Gets the index of the last result whose address is equal to or above the given address.
	uint64_t CSearchResultRef::GetIndexOfFirstItemWithAddressOrAbove( uint64_t _ui64Address ) const {
		//lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );

//#define MW_REF

		uint64_t ui64Count = 0;
		for ( size_t I = 0; I < m_vLanes.size(); ++I ) {
			uint64_t ui64Start = 0, ui64End = 0;
			GetAddress( I, 0, ui64Start );
			GetAddress( I, m_vLanes[I]->vAddresses.size() - 1, ui64End );
			if ( ui64Start >= _ui64Address ) {
				// Then it is this address in the list.
				return ui64Count;
			}
			if ( _ui64Address == ui64End ) {
				// Then it is the last address in this list.
				return ui64Count + m_vLanes[I]->vAddresses.size() - 1;
			}

			if ( _ui64Address >= ui64Start && _ui64Address <= ui64End ) {
#ifdef MW_REF
				// Reference.
				size_t stRef = 0;
				for ( stRef = 0; stRef < m_vLanes[I]->vAddresses.size(); ++stRef ) {
					uint64_t ui64Start2 = 0;
					GetAddress( I, stRef, ui64Start2 );
					if ( ui64Start2 >= _ui64Address ) {
						break;
					}
				}
#endif

				// It is in this list.  Do a binary search.
				size_t stJump = m_vLanes[I]->vAddresses.size() / 2;
				size_t stIdx = 1 + stJump;	// Because we checked index 0 in the (ui64Start >= _ui64Address) clause.
				while ( true ) {
					uint64_t ui64Tmp = 0;
					GetAddress( I, stIdx, ui64Tmp );
					if ( ui64Tmp == _ui64Address ) {
#ifdef MW_REF
						if ( stIdx != stRef ) {
							volatile int ghjg = 0;
						}
#endif
						return ui64Count + stIdx;
					}
					if ( ui64Tmp > _ui64Address ) {
						if ( stJump == 1 ) {
#ifdef MW_REF
							if ( stIdx != stRef ) {
								volatile int ghjg = 0;
							}
#endif
							return ui64Count + stIdx;
						}
						// We went too far.  Back it up.
						stIdx -= stJump;
						stJump /= 2;
						stIdx += stJump;
					}
					else if ( ui64Tmp < _ui64Address ) {
						if ( !stJump ) {
#ifdef MW_REF
							if ( stIdx + 1 != stRef ) {
								volatile int ghjg = 0;
							}
#endif
							return ui64Count + stIdx + 1;
						}
						// Target is either farther out or is this one.
						size_t stPrevIdx = stIdx;
						stIdx += stJump;
						// If we go beyond the end of the list, we can reduce jump sizes rapidly.
						while ( stIdx >= m_vLanes[I]->vAddresses.size() ) {
							stJump /= 2;
							stIdx = stPrevIdx + stJump;
							if ( !stJump ) {
								// Because of the _ui64Address < ui64End check above, we know that
								//	the address is in this range.
								// If stJump were to reach 0 and the loop were to continue, it would imply that stIdx is at the
								//	end of the list and that (ui64Tmp < _ui64Address), which
								//	contradicts (_ui64Address < ui64End) above.
								// So this loop necessarily must leave stIdx at no more than (m_vLanes[I]->vAddresses.size() - 2)
								//	and stJump at no less than 1.
								// Coming here is an error.
								break;
							}
						}
					}
				}
			}

			ui64Count += m_vLanes[I]->vAddresses.size();
		}
		return ui64Count;
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

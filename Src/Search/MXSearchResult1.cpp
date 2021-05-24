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
				GetAddressAndValue( I, _stIdx, _ui64Address, _pui8Value );
				return true;
			}
			_stIdx -= static_cast<size_t>(m_vAddressLists[I]->ui32NumberOfEntries);
		}

		return false;
	}

	// Gets a result under the assumption that the object is locked.
	bool CSearchResult1::GetResultFast_Locked( size_t _stIdx, uint64_t &_ui64Address, const uint8_t * &_pui8Value,
		size_t &_stSearchIndex, uint64_t &_ui64SearchAccumulator ) const {
		if ( _stIdx >= m_ui64Total ) { return false; }
		_stIdx -= _ui64SearchAccumulator;

		_ui64Address = 0;
		for ( size_t I = _stSearchIndex; I < m_vAddressLists.size(); ++I ) {
			if ( static_cast<size_t>(m_vAddressLists[I]->ui32NumberOfEntries) > _stIdx ) {
				// Found it.
				GetAddressAndValue( I, _stIdx, _ui64Address, _pui8Value );
				_stSearchIndex = I;
				return true;
			}
			_stIdx -= static_cast<size_t>(m_vAddressLists[I]->ui32NumberOfEntries);
			_ui64SearchAccumulator += m_vAddressLists[I]->ui32NumberOfEntries;
		}

		return false;
	}

	// Gets the address of a result under the assumption that the object is locked.
	bool CSearchResult1::GetResultFast_Locked( size_t _stIdx, uint64_t &_ui64Address,
		size_t &_stSearchIndex, uint64_t &_ui64SearchAccumulator ) const {
		if ( _stIdx >= m_ui64Total ) { return false; }
		_stIdx -= _ui64SearchAccumulator;

		_ui64Address = 0;
		for ( size_t I = _stSearchIndex; I < m_vAddressLists.size(); ++I ) {
			if ( static_cast<size_t>(m_vAddressLists[I]->ui32NumberOfEntries) > _stIdx ) {
				// Found it.
				GetAddress( I, _stIdx, _ui64Address );
				_stSearchIndex = I;
				return true;
			}
			_stIdx -= static_cast<size_t>(m_vAddressLists[I]->ui32NumberOfEntries);
			_ui64SearchAccumulator += m_vAddressLists[I]->ui32NumberOfEntries;
		}

		return false;
	}

	// Gets the first and last address.  The last address is the first address that is outside the range of the final item in the list, meaning the final item's address plus the size of that item.
	//	In other words, these addresses represent the entire range of addresses that fully encapsulates all values in the results.
	bool CSearchResult1::GetEncapsulatingAddresses( uint64_t &_ui64Start, uint64_t &_ui64End ) const {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		if ( !m_ui64Total ) { return false; }
		//if ( !GetResultFast( 0, _ui64Start, pui8Val ) ) { return false; }
		_ui64Start = 0;
		std::memcpy( &_ui64Start, &m_vAddressLists[0]->pui8Values[0], m_vAddressLists[0]->ui16SizeOfAddresses );
		size_t stIdx = m_vAddressLists.size() - 1;
		GetAddress( stIdx, m_vAddressLists[stIdx]->ui32NumberOfEntries - 1, _ui64End );
		//if ( !GetResultFast( m_ui64Total - 1, _ui64End, pui8Val ) ) { return false; }
		_ui64End += m_vAddressLists[m_vAddressLists.size()-1]->ui16SizeOfValues;
		return true;
	}

	// Gets the index of the last result whose address is equal to or above the given address.
	uint64_t CSearchResult1::GetIndexOfFirstItemWithAddressOrAbove( uint64_t _ui64Address ) const {
		//lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );

//#define MW_REF

		uint64_t ui64Count = 0;
		for ( size_t I = 0; I < m_vAddressLists.size(); ++I ) {
			uint64_t ui64Start = 0, ui64End = 0;
			GetAddress( I, 0, ui64Start );
			GetAddress( I, m_vAddressLists[I]->ui32NumberOfEntries - 1, ui64End );
			if ( ui64Start >= _ui64Address ) {
				// Then it is this address in the list.
				return ui64Count;
			}
			if ( _ui64Address == ui64End ) {
				// Then it is the last address in this list.
				return ui64Count + m_vAddressLists[I]->ui32NumberOfEntries - 1;
			}

			if ( _ui64Address >= ui64Start && _ui64Address <= ui64End ) {
#ifdef MW_REF
				// Reference.
				size_t stRef = 0;
				for ( stRef = 0; stRef < m_vAddressLists[I]->ui32NumberOfEntries; ++stRef ) {
					uint64_t ui64Start2 = 0;
					GetAddress( I, stRef, ui64Start2 );
					if ( ui64Start2 >= _ui64Address ) {
						break;
					}
				}
#endif

				// It is in this list.  Do a binary search.
				size_t stJump = m_vAddressLists[I]->ui32NumberOfEntries / 2;
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
						while ( stIdx >= m_vAddressLists[I]->ui32NumberOfEntries ) {
							stJump /= 2;
							stIdx = stPrevIdx + stJump;
							if ( !stJump ) {
								// Because of the _ui64Address < ui64End check above, we know that
								//	the address is in this range.
								// If stJump were to reach 0 and the loop were to continue, it would imply that stIdx is at the
								//	end of the list and that (ui64Tmp < _ui64Address), which
								//	contradicts (_ui64Address < ui64End) above.
								// So this loop necessarily must leave stIdx at no more than (m_vAddressLists[I]->ui32NumberOfEntries - 2)
								//	and stJump at no less than 1.
								// Coming here is an error.
								break;
							}
						}
					}
				}
			}

			ui64Count += m_vAddressLists[I]->ui32NumberOfEntries;
		}
		return ui64Count;
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

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

	// Finalize the list after a search.
	void CSearchResult2::Finalize() {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		m_vAddressLists.shrink_to_fit();
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

	// Gets a result under the assumption that the object is locked.
	bool CSearchResult2::GetResultFast_Locked( size_t _stIdx, uint64_t &_ui64Address, const uint8_t * &_pui8Value,
		size_t &_stSearchIndex, uint64_t &_ui64SearchAccumulator ) const {
		if ( _stIdx >= m_ui64Total ) { return false; }
		_stIdx -= _ui64SearchAccumulator;
		for ( size_t I = _stSearchIndex; I < m_vAddressLists.size(); ++I ) {
			if ( m_vAddressLists[I]->ui64NumberOfEntries > static_cast<uint64_t>(_stIdx) ) {
				// Found it.
				size_t stOffset = m_vAddressLists[I]->ui16Align * _stIdx;
				_ui64Address = m_vAddressLists[I]->ui64Address + stOffset;
				_pui8Value = &m_vAddressLists[I]->pui8Values[stOffset];
				_stSearchIndex = I;
				return true;
			}
			_stIdx -= static_cast<size_t>(m_vAddressLists[I]->ui64NumberOfEntries);
			_ui64SearchAccumulator += m_vAddressLists[I]->ui64NumberOfEntries;
		}

		return false;
	}

	// Gets the address of a result under the assumption that the object is locked.
	bool CSearchResult2::GetResultFast_Locked( size_t _stIdx, uint64_t &_ui64Address,
		size_t &_stSearchIndex, uint64_t &_ui64SearchAccumulator ) const {

		if ( _stIdx >= m_ui64Total ) { return false; }
		_stIdx -= _ui64SearchAccumulator;
		for ( size_t I = _stSearchIndex; I < m_vAddressLists.size(); ++I ) {
			if ( m_vAddressLists[I]->ui64NumberOfEntries > static_cast<uint64_t>(_stIdx) ) {
				// Found it.
				size_t stOffset = m_vAddressLists[I]->ui16Align * _stIdx;
				_ui64Address = m_vAddressLists[I]->ui64Address + stOffset;
				_stSearchIndex = I;
				return true;
			}
			_stIdx -= static_cast<size_t>(m_vAddressLists[I]->ui64NumberOfEntries);
			_ui64SearchAccumulator += m_vAddressLists[I]->ui64NumberOfEntries;
		}

		return false;
	}

	// Gets the first and last address.  The last address is the first address that is outside the range of the final item in the list, meaning the final item's address plus the size of that item.
	//	In other words, these addresses represent the entire range of addresses that fully encapsulates all values in the results.
	bool CSearchResult2::GetEncapsulatingAddresses( uint64_t &_ui64Start, uint64_t &_ui64End ) const {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );
		if ( !m_ui64Total ) { return false; }

		_ui64Start = m_vAddressLists[0]->ui64Address;
		const MX_ADDRESS_ARRAY * paaObj = m_vAddressLists[m_vAddressLists.size()-1];
		_ui64End = paaObj->ui64Address + paaObj->ui64NumberOfEntries * paaObj->ui16Align;
		return true;
	}

	// Gets the index of the last result whose address is equal to or above the given address.
	uint64_t CSearchResult2::GetIndexOfFirstItemWithAddressOrAbove( uint64_t _ui64Address ) const {
		//lsw::CCriticalSection::CEnterCrit ecCrit( m_csCrit );

//#define MW_REF

		uint64_t ui64Count = 0;
		for ( size_t I = 0; I < m_vAddressLists.size(); ++I ) {
			uint64_t ui64Start = 0, ui64End = 0;
			GetAddress( I, 0, ui64Start );
			GetAddress( I, m_vAddressLists[I]->ui64NumberOfEntries - 1, ui64End );
			if ( ui64Start >= _ui64Address ) {
				// Then it is this address in the list.
				return ui64Count;
			}
			if ( _ui64Address == ui64End ) {
				// Then it is the last address in this list.
				return ui64Count + m_vAddressLists[I]->ui64NumberOfEntries - 1;
			}
			
			if ( _ui64Address >= ui64Start && _ui64Address < ui64End ) {
#ifdef MW_REF
				// Reference.
				size_t stRef = 0;
				for ( stRef = 0; stRef < m_vAddressLists[I]->ui64NumberOfEntries; ++stRef ) {
					uint64_t ui64Start2 = 0;
					GetAddress( I, stRef, ui64Start2 );
					if ( ui64Start2 >= _ui64Address ) {
						break;
					}
				}
#endif

				// It is in this list.  Do a binary search.
				size_t stJump = m_vAddressLists[I]->ui64NumberOfEntries / 2;
				size_t stIdx = 1 + stJump;	// Because we checked index 0 in the (ui64Start >= _ui64Address) clause.
				
				while ( true ) {
					uint64_t ui64Tmp = 0;
					GetAddress( I, stIdx, ui64Tmp );
					if ( ui64Tmp == _ui64Address ) {
						return ui64Count + stIdx;
					}
					if ( ui64Tmp > _ui64Address ) {
						if ( stJump == 1 ) {
							// This is the first address above the given address.
							return ui64Count + stIdx;
						}
						// We went too far.  Back it up.
						stIdx -= stJump;
						stJump /= 2;
						stIdx += stJump;
					}
					else if ( ui64Tmp < _ui64Address ) {
						if ( !stJump ) {
							return ui64Count + stIdx + 1;
						}
						// Target is either farther out or is this one.
						size_t stPrevIdx = stIdx;
						stIdx += stJump;
						// If we go beyond the end of the list, we can reduce jump sizes rapidly.
						while ( stIdx >= m_vAddressLists[I]->ui64NumberOfEntries ) {
							stJump /= 2;
							stIdx = stPrevIdx + stJump;
							if ( !stJump ) {
								// Because of the _ui64Address < ui64End check above, we know that
								//	the address is in this range.
								// If stJump were to reach 0 and the loop were to continue, it would imply that stIdx is at the
								//	end of the list and that (ui64Tmp < _ui64Address), which
								//	contradicts (_ui64Address < ui64End) above.
								// So this loop necessarily must leave stIdx at no more than (m_vAddressLists[I]->ui64NumberOfEntries - 2)
								//	and stJump at no less than 1.
								// Coming here is an error.
								break;
							}
						}
					}
				}
			}

			ui64Count += m_vAddressLists[I]->ui64NumberOfEntries;
		}
		return ui64Count;
	}

}	// namespace mx

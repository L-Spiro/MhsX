#pragma once
#include "../MXMhsX.h"
#include "../Files/MXFile.h"
#include "../Utilities/MXUtilities.h"
#include "MXSearchResultBase.h"

#include <algorithm>
#include <CriticalSection/LSWCriticalSection.h>


// == Macros.
//#define MX_SR1_SMALLCOPY_ADDRESS( DEST, SRC, SIZE )												std::memcpy( DEST, SRC, SIZE )
#define MX_SR1_SMALLCOPY_ADDRESS( DEST, SRC, SIZE )												\
	switch ( SIZE ) {																			\
		case 3 : { memcpy_3( DEST, SRC ); break; }												\
		case 4 : { memcpy_4( DEST, SRC ); break; }												\
		case 6 : { memcpy_6( DEST, SRC ); break; }												\
		case 8 : { memcpy_8( DEST, SRC ); break; }												\
	}


namespace mx {

	class CSearchResult1 : public CSearchResultBase {
	public :
		CSearchResult1();
		virtual ~CSearchResult1();


		// == Functions.
		// Resets the address list to nothing, deallocates everything.
		void								Reset();

		// Add a value.
		virtual bool						Add( uint64_t _ui64Address, uint8_t * _pui8Value, uint64_t _ui64Size );

		// Gets the total number of results.
		virtual uint64_t					TotalResults() const { return m_ui64Total; }
		
		// Locks for reading a value quickly.  Unlock when done with the returned pointer.
		virtual void						Lock() const { m_csCrit.EnterCriticalSection(); }

		// Gets a result by index.  Does not copy the result into a buffer, instead giving back a pointer directly to its internal storage,
		//	which means the caller must finish using the data before the search results are updated in any way.
		virtual bool						GetResultFast( size_t _stIdx, uint64_t &_ui64Address, const uint8_t * &_pui8Value ) const;

		// Gets a result under the assumption that the object is locked.
		virtual bool						GetResultFast_Locked( size_t _stIdx, uint64_t &_ui64Address, const uint8_t * &_pui8Value,
			size_t &_stSearchIndex, uint64_t &_ui64SearchAccumulator ) const;

		// Gets the address of a result under the assumption that the object is locked.
		virtual bool						GetResultFast_Locked( size_t _stIdx, uint64_t &_ui64Address,
			size_t &_stSearchIndex, uint64_t &_ui64SearchAccumulator ) const;

		// Unlocks after done with the buffer returned by GetResultFast().
		virtual void						Unlock() const { m_csCrit.LeaveCriticalSection(); }

		// Gets the first and last address.  The last address is the first address that is outside the range of the final item in the list, meaning the final item's address plus the size of that item.
		//	In other words, these addresses represent the entire range of addresses that fully encapsulates all values in the results.
		virtual bool						GetEncapsulatingAddresses( uint64_t &_ui64Start, uint64_t &_ui64End ) const;

		// Gets the index of the last result whose address is equal to or above the given address.
		virtual uint64_t					GetIndexOfFirstItemWithAddressOrAbove( uint64_t _ui64Address ) const;

		// Consolidates the results from another CSearchResult1 object.  Leaves _srResults in a valid state after stealing its pointers. 
		bool								AppendResults( CSearchResult1 &_srResults );

		// Are copies if the data stored within the results?  If not, relative subsearches cannot be made since there is no "previous" value.
		virtual bool						StoresValues() const { return true; }



	protected :
		// == Types.
		// A single array of results (address + value).
		struct MX_ADDRESS_ARRAY {
			uint64_t						ui64AllocSize;			// Total bytes to which pui8Values points.
			uint64_t						ui64NumberOfEntries;	// Number of items.
			uint8_t *						pui8Values;
			uint16_t						ui16SizeOfAddresses;	// Number of bytes in the "address" part of each entry.
			uint16_t						ui16SizeOfValues;		// Number of bytes in each entry.  Location of any entry is pui8Values[(ui16SizeOfAddresses+ui16SizeOfValues)*IDX].

			MX_ADDRESS_ARRAY() :
				pui8Values( nullptr ),
				ui64AllocSize( 0 ),
				ui64NumberOfEntries( 0 ),
				ui16SizeOfAddresses( 0 ),
				ui16SizeOfValues( 0 ) {}
			MX_ADDRESS_ARRAY( MX_ADDRESS_ARRAY &&_aaOther ) {
				(*this) = std::move( _aaOther );
			}
			MX_ADDRESS_ARRAY( MX_ADDRESS_ARRAY &_aaOther ) {
				(*this) = _aaOther;
			}
			~MX_ADDRESS_ARRAY() {
				delete [] pui8Values;
				pui8Values = nullptr;
				ui64NumberOfEntries = 0;
				ui64AllocSize = 0;
			}

			MX_ADDRESS_ARRAY &				operator = ( MX_ADDRESS_ARRAY &&_aaOther ) {
				if ( this != &_aaOther ) {
					delete [] pui8Values;
#define MX_MOVE( NEM ) NEM = _aaOther.NEM; _aaOther.NEM = 0;
					MX_MOVE( pui8Values );
					MX_MOVE( ui64AllocSize );
					MX_MOVE( ui64NumberOfEntries );
					MX_MOVE( ui16SizeOfAddresses );
					MX_MOVE( ui16SizeOfValues );
#undef MX_MOVE
				}
				return (*this);
			}

			MX_ADDRESS_ARRAY &				operator = ( MX_ADDRESS_ARRAY &_aaOther ) {
				if ( this != &_aaOther ) {
					delete [] pui8Values;
					ui64AllocSize = 0;
					uint64_t ui64NewSize = (_aaOther.ui16SizeOfAddresses + _aaOther.ui16SizeOfValues) * _aaOther.ui64NumberOfEntries;
					pui8Values = new( std::nothrow ) uint8_t[ui64NewSize];
					if ( pui8Values ) {
						std::memcpy( pui8Values, _aaOther.pui8Values, ui64NewSize );
#define MX_MOVE( NEM ) NEM = _aaOther.NEM;
						ui64AllocSize = ui64NewSize;
						MX_MOVE( ui64NumberOfEntries );
						MX_MOVE( ui16SizeOfAddresses );
						MX_MOVE( ui16SizeOfValues );
#undef MX_MOVE
					}
					else {
						ui64NumberOfEntries = 0;
						ui64AllocSize = 0;
						ui16SizeOfAddresses = ui16SizeOfValues = 0;
					}
				}
				return (*this);
			}


			// == Function.
			// Adds an address and value.
			bool							Add( uint64_t _ui64Addr, uint8_t * _puiData ) {
				uint64_t ui64NewSize = (uint64_t( ui16SizeOfAddresses ) + ui16SizeOfValues) * (ui64NumberOfEntries + 1ULL);
				if ( ui64AllocSize < ui64NewSize ) {
					if ( !ReAlloc( std::max( 8ULL * 1024ULL * 1024ULL, ui64AllocSize * 2ULL ) ) ) {
						return false;
					}
				}
				size_t sIdx = (uint64_t( ui16SizeOfAddresses ) + ui16SizeOfValues) * ui64NumberOfEntries;
				//std::memcpy( &pui8Values[sIdx], &_ui64Addr, ui16SizeOfAddresses );
				MX_SR1_SMALLCOPY_ADDRESS( &pui8Values[sIdx], &_ui64Addr, ui16SizeOfAddresses );
				std::memcpy( &pui8Values[sIdx] + ui16SizeOfAddresses, _puiData, ui16SizeOfValues );
				++ui64NumberOfEntries;
				return true;
			}

			// Reallocate to a new size.
			bool							ReAlloc( uint64_t _ui64NewSize ) {
				uint8_t * pui8New = new ( std::nothrow ) uint8_t[_ui64NewSize];
				if ( !pui8New ) { return false; }
				std::memcpy( pui8New, pui8Values, ui64AllocSize );
				delete [] pui8Values;
				pui8Values = pui8New;
				ui64AllocSize = _ui64NewSize;
				return true;
			}
		};


		// == Members.
		// Array of address lists.
		std::vector<MX_ADDRESS_ARRAY *>		m_vAddressLists;

		// Total results.
		uint64_t							m_ui64Total;

		// Critical section.
		lsw::CCriticalSection				m_csCrit;


		// == Functions.
		// Adds a new address list to m_vAddressLists.
		bool								AddAddressList( size_t _sAddrSize, uint64_t _ui64Size );

		// Gets the address and value inside a given array.
		__inline void						GetAddressAndValue( size_t _sArrayIdx, size_t _stIdx,
			uint64_t &_ui64Address, const uint8_t * &_pui8Value ) const {
			size_t stOffset = (m_vAddressLists[_sArrayIdx]->ui16SizeOfAddresses + m_vAddressLists[_sArrayIdx]->ui16SizeOfValues) * _stIdx;
			//std::memcpy( &_ui64Address, &m_vAddressLists[_sArrayIdx]->pui8Values[stOffset], m_vAddressLists[_sArrayIdx]->ui16SizeOfAddresses );
			MX_SR1_SMALLCOPY_ADDRESS( &_ui64Address, &m_vAddressLists[_sArrayIdx]->pui8Values[stOffset], m_vAddressLists[_sArrayIdx]->ui16SizeOfAddresses );
			stOffset += m_vAddressLists[_sArrayIdx]->ui16SizeOfAddresses;
			_pui8Value = &m_vAddressLists[_sArrayIdx]->pui8Values[stOffset];
		}

		// Gets the address inside a given array.
		__inline void						GetAddress( size_t _sArrayIdx, size_t _stIdx,
			uint64_t &_ui64Address ) const {
			size_t stOffset = (m_vAddressLists[_sArrayIdx]->ui16SizeOfAddresses + m_vAddressLists[_sArrayIdx]->ui16SizeOfValues) * _stIdx;
			//std::memcpy( &_ui64Address, &m_vAddressLists[_sArrayIdx]->pui8Values[stOffset], m_vAddressLists[_sArrayIdx]->ui16SizeOfAddresses );
			MX_SR1_SMALLCOPY_ADDRESS( &_ui64Address, &m_vAddressLists[_sArrayIdx]->pui8Values[stOffset], m_vAddressLists[_sArrayIdx]->ui16SizeOfAddresses );
		}
	};

}	// namespace mx

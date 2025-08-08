#pragma once
#include "../MXMhsX.h"
#include "../Files/MXFile.h"
#include "../IO/MXInOutInterface.h"
#include "MXSearchResultBase.h"

#include <algorithm>
#include <CriticalSection/LSWCriticalSection.h>

namespace mx {

	class CSearchResult2 : public CSearchResultBase {
	public :
		CSearchResult2();
		virtual ~CSearchResult2();


		// == Functions.
		// Resets the address list to nothing, deallocates everything.
		void								Reset();

		// Add a value.
		virtual bool						Add(  CInOutInterface * _pProc,
			uint64_t _ui64Address, uint16_t _ui16ItemSize, uint64_t _ui64TotalItems, uint16_t _ui16Align );

		// Gets the total number of results.
		virtual uint64_t					TotalResults() const { return m_ui64Total; }

		// Finalize the list after a search.
		virtual void						Finalize();
		
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

		// Are copies if the data stored within the results?  If not, relative subsearches cannot be made since there is no "previous" value.
		virtual bool						StoresValues() const { return true; }


	protected :
		// == Types.
		// A single array of results (address + value).
		struct MX_ADDRESS_ARRAY {
			uint64_t						ui64AllocSize;			// Total bytes to which pui8Values points.
			uint64_t						ui64Address;			// Starting address of the chunk.
			uint64_t						ui64NumberOfEntries;	// Number of items.
			uint8_t *						pui8Values;
			uint16_t						ui16SizeOfValues;		// Number of bytes in each entry.  Location of any entry is pui8Values[(ui16SizeOfAddresses+ui16SizeOfValues)*IDX].
			uint16_t						ui16Align;				// Alignment (used to calculate offsets to each entry).

			MX_ADDRESS_ARRAY() :
				pui8Values( nullptr ),
				ui64AllocSize( 0 ),
				ui64Address( 0 ),
				ui64NumberOfEntries( 0 ),
				ui16SizeOfValues( 0 ),
				ui16Align( 0 ) {}
			MX_ADDRESS_ARRAY( MX_ADDRESS_ARRAY &&_aaOther ) noexcept {
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
				ui16SizeOfValues = 0;
				ui16Align = 0;
			}

			MX_ADDRESS_ARRAY &				operator = ( MX_ADDRESS_ARRAY &&_aaOther ) noexcept {
				if ( this != &_aaOther ) {
					delete [] pui8Values;
#define MX_MOVE( NEM ) NEM = _aaOther.NEM; _aaOther.NEM = 0;
					MX_MOVE( pui8Values );
					MX_MOVE( ui64AllocSize );
					MX_MOVE( ui64NumberOfEntries );
					MX_MOVE( ui16SizeOfValues );
					MX_MOVE( ui64Address );
					MX_MOVE( ui16Align );
#undef MX_MOVE
				}
				return (*this);
			}

			MX_ADDRESS_ARRAY &				operator = ( MX_ADDRESS_ARRAY &_aaOther ) {
				if ( this != &_aaOther ) {
					delete [] pui8Values;
					ui64AllocSize = 0;
					pui8Values = new( std::nothrow ) uint8_t[_aaOther.ui64AllocSize];
					if ( pui8Values ) {
						std::memcpy( pui8Values, _aaOther.pui8Values, _aaOther.ui64AllocSize );
#define MX_MOVE( NEM ) NEM = _aaOther.NEM;
						MX_MOVE( ui64AllocSize );
						MX_MOVE( ui64NumberOfEntries );
						MX_MOVE( ui16SizeOfValues );
						MX_MOVE( ui64Address );
						MX_MOVE( ui16Align );
#undef MX_MOVE
					}
				}
				return (*this);
			}


			// == Function.
			// Sets an address and value.
			bool							Set( CInOutInterface * _pProc,
				uint64_t _ui64Address, uint16_t _ui16ItemSize, uint64_t _ui64TotalItems, uint16_t _ui16Align ) {
				uint64_t ui64FinalSize = (_ui64TotalItems - 1) * _ui16Align + _ui16ItemSize;
				uint8_t * pui8New = new ( std::nothrow ) uint8_t[ui64FinalSize];
				if ( !pui8New ) { return false; }

				if ( !_pProc->Read( _ui64Address, pui8New, ui64FinalSize, NULL ) ) {
					delete [] pui8New;
					return false;
				}

				ui64AllocSize = ui64FinalSize;
				delete [] pui8Values;
				pui8Values = pui8New;
				ui64Address = _ui64Address;
				ui16SizeOfValues = _ui16ItemSize;
				ui64NumberOfEntries = _ui64TotalItems;
				ui16Align = _ui16Align;
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
		// Gets the address and value inside a given array.
		__inline void						GetAddressAndValue( size_t _sArrayIdx, size_t _stIdx,
			uint64_t &_ui64Address, const uint8_t * &_pui8Value ) const {
			size_t stOffset = m_vAddressLists[_sArrayIdx]->ui16Align * _stIdx;
			_ui64Address = m_vAddressLists[_sArrayIdx]->ui64Address + stOffset;
			_pui8Value = &m_vAddressLists[_sArrayIdx]->pui8Values[stOffset];
		}

		// Gets the address inside a given array.
		__inline void						GetAddress( size_t _sArrayIdx, size_t _stIdx,
			uint64_t &_ui64Address ) const {
			size_t stOffset = m_vAddressLists[_sArrayIdx]->ui16Align * _stIdx;
			_ui64Address = m_vAddressLists[_sArrayIdx]->ui64Address + stOffset;
		}
	};

}	// namespace mx

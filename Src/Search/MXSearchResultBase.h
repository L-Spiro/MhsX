#pragma once
#include "../MXMhsX.h"

namespace mx {

	class CSearchResultBase {
	public :
		CSearchResultBase();
		virtual ~CSearchResultBase();


		// == Functions.
		// Add a value.
		virtual bool						Add( uint64_t _ui64Address, uint8_t * _pui8Value, uint64_t _ui64Size ) { return false; }

		// Add a reference to a value.
		virtual bool						Add( uint64_t _ui64Address, uint64_t _ui64Size, uint16_t _ui16Data ) { return false; }

		// Finalize the list after a search.
		virtual void						Finalize() {}

		// Gets the total number of results.
		virtual uint64_t					TotalResults() const { return 0; }

		// Locks for reading a value quickly.  Unlock when done with the returned pointer.
		virtual void						Lock() const {}

		// Gets a result by index.  Does not copy the result into a buffer, instead giving back a pointer directly to its internal storage,
		//	which means the caller must finish using the data before the search results are updated in any way.
		virtual bool						GetResultFast( size_t _stIdx, uint64_t &_ui64Address, const uint8_t * &_pui8Value ) const { return false; }

		// Gets a result under the assumption that the object is locked.
		virtual bool						GetResultFast_Locked( size_t _stIdx, uint64_t &_ui64Address, const uint8_t * &_pui8Value,
			size_t &_stSearchIndex, uint64_t &_ui64SearchAccumulator ) const { return false; }

		// Gets the address of a result under the assumption that the object is locked.
		virtual bool						GetResultFast_Locked( size_t _stIdx, uint64_t &_ui64Address,
			size_t &_stSearchIndex, uint64_t &_ui64SearchAccumulator ) const { return false; }

		// Unlocks after done with the buffer returned by GetResultFast().
		virtual void						Unlock() const {}

		// Gets the first and last address.  The last address is the first address that is outside the range of the final item in the list, meaning the final item's address plus the size of that item.
		//	In other words, these addresses represent the entire range of addresses that fully encapsulates all values in the results.
		virtual bool						GetEncapsulatingAddresses( uint64_t &_ui64Start, uint64_t &_ui64End ) const { return false; }

		// Gets the index of the last result whose address is equal to or above the given address.
		virtual uint64_t					GetIndexOfFirstItemWithAddressOrAbove( uint64_t _ui64Address ) const { return static_cast<uint64_t>(-1); }

		// Are copies if the data stored within the results?  If not, relative subsearches cannot be made since there is no "previous" value.
		virtual bool						StoresValues() const { return false; }



	protected :

	};

}	// namespace mx

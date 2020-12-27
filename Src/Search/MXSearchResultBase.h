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

		// Unlocks after done with the buffer returned by GetResultFast().
		virtual void						Unlock() const {}


	protected :

	};

}	// namespace mx

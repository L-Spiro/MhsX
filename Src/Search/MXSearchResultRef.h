#pragma once
#include "../MXMhsX.h"
#include "MXSearchResultBase.h"

#include <vector>
#include <CriticalSection/LSWCriticalSection.h>

namespace mx {

	class CSearchResultRef : public CSearchResultBase {
	public :
		CSearchResultRef();
		virtual ~CSearchResultRef();


		// == Types.
		// == A stored reference to an address.  Includes the address, the size, and some data associated with it.
		struct MX_ADDRESS_REF {
			uint32_t						uiSize;						// Size of the data.
			uint16_t						uiAddr;						// Offset to the address.
			uint16_t						uiData;						// Any data accosiated with the value.
		};


		// == Functions.
		// Resets the address list to nothing, deallocates everything.
		void								Reset();

		// Add a reference to a value.
		virtual bool						Add( uint64_t _ui64Address, uint64_t _ui64Size, uint16_t _ui16Data );

		// Gets the total number of results.
		virtual uint64_t					TotalResults() const { return m_ui64Total; }
		
		// Locks for reading a value quickly.  Unlock when done with the returned pointer.
		virtual void						Lock() const { m_csCrit.EnterCriticalSection(); }

		// Unlocks after done with the buffer returned by GetResultFast().
		virtual void						Unlock() const { m_csCrit.LeaveCriticalSection(); }

		// Gets a result by index.  Does not copy the result into a buffer, instead giving back a pointer directly to its internal storage,
		//	which means the caller must finish using the data before the search results are updated in any way.
		virtual bool						GetResultFast( size_t _stIdx, uint64_t &_ui64Address, const uint8_t * &_pui8Value ) const;

		// Consolidates the results from another CSearchResult1 object.  Leaves _srResults in a valid state after stealing its pointers. 
		bool								AppendResults( CSearchResultRef &_srrResults );


	protected :
		// A base address plus array of references based off it.
		struct MX_LANE {
			uint64_t						uiBaseAddr;
			std::vector<MX_ADDRESS_REF>		vAddresses;
		};


		// == Members.
		// Total results.
		uint64_t							m_ui64Total;
		// Array of result lanes.
		std::vector<MX_LANE *>				m_vLanes;
		// Critical section.
		lsw::CCriticalSection				m_csCrit;
	};

}	// namespace mx

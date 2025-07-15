#pragma once

#include "../MXMhsX.h"
#include "MXFoundAddress.h"
#include "MXFoundAddressGroup.h"

#include <map>
#include <memory>


namespace mx {

	class CFoundAddressManager {
	public :
		CFoundAddressManager();
		~CFoundAddressManager();


		// == Functions.
		// Gets a pointer to a found-address object given its ID.
		inline CFoundAddressBase *											GetById( size_t _sId ) {
			auto aRet = m_mFoundAddresses.find( _sId );
			if ( aRet != m_mFoundAddresses.end() ) {
				return aRet->second.get();
			}
			return nullptr;
		}

		// Removes an item by ID.
		bool																Delete( size_t _sId );

		// Removes all items.
		void																DeleteAll();

		// Sets an item as a child of another.
		bool																SetParent( size_t _sId, size_t _sParent );

		// Adds a normal Found Address.
		CFoundAddress *														AddFoundAddress();

		// Adds a group.
		CFoundAddressGroup *												AddGroup();

	protected :
		// == Members.
		// The pool of found-address objects.
		std::map<size_t, std::unique_ptr<CFoundAddressBase>>				m_mFoundAddresses;
	};

}	// namespace mx

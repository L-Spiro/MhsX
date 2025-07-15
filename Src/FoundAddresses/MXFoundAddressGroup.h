#pragma once

#include "../MXMhsX.h"
#include "MXFoundAddressBase.h"


namespace mx {

	class CFoundAddressGroup : public CFoundAddressBase {
	public :
		CFoundAddressGroup() {
		}
		virtual ~CFoundAddressGroup() {
		}


		// == Functions.
		// Gets the item type.
		virtual MX_FOUND_ADDRESS_TYPES						Type() const { return MX_FAT_FOLDER; }
	};

}	//namespace mx

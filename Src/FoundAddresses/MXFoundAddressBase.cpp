#include "MXFoundAddressBase.h"


namespace mx {

	// The number of instances of this class.
	std::atomic<size_t> CFoundAddressBase::s_aClassCount = 0;

	// The ID counter.
	std::atomic<size_t> CFoundAddressBase::s_aId = 1;

	CFoundAddressBase::CFoundAddressBase() {
		try {
		}
		catch ( ... ) {
			throw;
		}
		++s_aClassCount;
		m_sId = s_aId++;
	}
	CFoundAddressBase::~CFoundAddressBase() {
		if ( --s_aClassCount == 0 ) {
			s_aId = 1;
		}
	}

}	// namespace mx

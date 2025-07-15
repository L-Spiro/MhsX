#include "MXFoundAddress.h"


namespace mx {

	CFoundAddress::CFoundAddress() {
	}
	CFoundAddress::~CFoundAddress() {
	}

	// == Functions.
	// Sets the address, type, and original data.
	bool CFoundAddress::InitWithAddressAndDataType( uint64_t _ui64Address, CUtilities::MX_DATA_TYPES _dtType, const uint8_t * _pui8Data ) {
		m_eAddressExp.Reset();
		m_sAddressTxt.clear();
		m_sAddressTxt = std::string();
		m_ui64Address = _ui64Address;

		m_vtValueType = CUtilities::MX_VT_DATA_TYPE;
		m_dtDataType = _dtType;
		try {
			m_vOriginalData.resize( CUtilities::DataTypeSize( _dtType ) );
			std::memcpy( m_vOriginalData.data(), _pui8Data, m_vOriginalData.size() );
		}
		catch ( ... ) {
			return false;
		}
		return true;
	}

}	// namespace mx

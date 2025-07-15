#pragma once

#include "../MXMhsX.h"
#include "../Utilities/MXUtilities.h"
#include "MXFoundAddressBase.h"

#include <EEExpEval.h>

#include <atomic>


namespace mx {

	class CFoundAddress : public CFoundAddressBase {
	public :
		CFoundAddress();
		virtual ~CFoundAddress();


		// == Functions.
		// Gets the item type.
		virtual MX_FOUND_ADDRESS_TYPES						Type() const { return MX_FAT_FOUND_ADDRESS; }

		// Sets the address, type, and original data.
		bool												InitWithAddressAndDataType( uint64_t _ui64Address, CUtilities::MX_DATA_TYPES _dtType, const uint8_t * _pui8Data );


	protected :
		// == Members.
		// The address expression (as text).
		std::string											m_sAddressTxt;
		// The address expression.
		lsw::CExpression									m_eAddressExp;
		// The address buffered (if not 0).
		uint64_t											m_ui64Address = 0;

		// The tyoe of data type.
		CUtilities::MX_VALUE_TYPE							m_vtValueType = CUtilities::MX_VT_DATA_TYPE;
		// The type of data.
		CUtilities::MX_DATA_TYPES							m_dtDataType = CUtilities::MX_DT_VOID;

		// The original data when added.
		std::vector<uint8_t>								m_vOriginalData;
	};

}	// namespace mx

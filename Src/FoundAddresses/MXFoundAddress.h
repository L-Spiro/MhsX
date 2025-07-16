#pragma once

#include "../MXMhsX.h"
#include "../Utilities/MXUtilities.h"
#include "MXFoundAddressBase.h"

#include <EEExpEval.h>

#include <atomic>


namespace mx {

	class CFoundAddress : public CFoundAddressBase {
	public :
		CFoundAddress( CMemHack * _pmhMemHack );
		virtual ~CFoundAddress();


		// == Functions.
		// Gets the item type.
		virtual MX_FOUND_ADDRESS_TYPES						Type() const { return MX_FAT_FOUND_ADDRESS; }

		// Sets the address, type, and original data.
		bool												InitWithAddressAndDataType( uint64_t _ui64Address, CUtilities::MX_DATA_TYPES _dtType, const uint8_t * _pui8Data );

		// Gets the Address text.
		virtual std::wstring								AddressText() const;

		// Gets the Value text.
		virtual std::wstring								ValueText() const { return ToText( m_vOriginalData ); }

		// Gets the Value When Locked text.
		virtual std::wstring								ValueWhenLockedText() const { return ToText( m_vLockedData ); }

		// Gets the Type text.
		virtual std::wstring								TypeText() const;

		// Gets the global type of the object.
		inline CUtilities::MX_VALUE_TYPE					ValueType() const { return m_vtValueType; }

		// Gets the data type.
		inline CUtilities::MX_DATA_TYPES					DataType() const { return m_dtDataType; }

		// Gets the pre-processing method (byteswapping).
		inline CUtilities::MX_BYTESWAP						PreProcessing() const { m_bsByteSwap; }

		// Sets the Value Type.
		bool												SetValueType( CUtilities::MX_VALUE_TYPE _vtType );

		// Sets the Data Type.
		bool												SetDataType( CUtilities::MX_DATA_TYPES _dtDataType );

		// Sets the Pre-Processing Type.
		inline bool											SetPreProcessing( CUtilities::MX_BYTESWAP _bsByteSwap ) { m_bsByteSwap = _bsByteSwap; }

		// Gets the size of the internal buffer needed to contain the data type.
		size_t												InternalBufferSize() const;


	protected :
		// == Members.
		// The address expression (as text).
		std::string											m_sAddressTxt;
		// The address expression.
		lsw::CExpression									m_eAddressExp;
		// The address buffered (if not 0).
		uint64_t											m_ui64Address = 0;

		// The type of data type.
		CUtilities::MX_VALUE_TYPE							m_vtValueType = CUtilities::MX_VT_DATA_TYPE;
		// The type of data.
		CUtilities::MX_DATA_TYPES							m_dtDataType = CUtilities::MX_DT_VOID;
		// Byteswapping.
		CUtilities::MX_BYTESWAP								m_bsByteSwap = CUtilities::MX_BS_NONE;

		// The original data when added.
		std::vector<uint8_t>								m_vOriginalData;
		// The data when locked.
		std::vector<uint8_t>								m_vLockedData;


		// == Functions.
		// Takes a blob of data and converts it to text.
		std::wstring										ToText( const std::vector<uint8_t> &_vBlob ) const;

		// Takes a blob of data and converts it to text.
		std::wstring										ToText( const uint8_t * _pui8Blob ) const;

		// Update internal buffers after the size of the item changes.
		void												UpdateBuffers();
	};

}	// namespace mx

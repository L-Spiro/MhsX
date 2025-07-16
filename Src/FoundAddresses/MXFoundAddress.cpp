#include "MXFoundAddress.h"

#include <Float16/EEFloat16.h>

#include <format>


namespace mx {

	CFoundAddress::CFoundAddress( CMemHack * _pmhMemHack ) :
		CFoundAddressBase( _pmhMemHack ) {
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
			m_vLockedData = m_vOriginalData;
		}
		catch ( ... ) {
			return false;
		}
		return true;
	}

	// Gets the Address text.
	std::wstring CFoundAddress::AddressText() const {
		if ( m_ui64Address ) {
			std::wstring wTmp;
			CUtilities::ToHex( m_ui64Address, wTmp, m_ui64Address > UINT_MAX ? 11 : 8, false );
			return wTmp;
		}
		return std::wstring();
	}

	// Takes a blob of data and converts it to text.
	std::wstring CFoundAddress::ToText( const std::vector<uint8_t> &_vBlob ) const {
		return ToText( _vBlob.data() );
	}

	// Takes a blob of data and converts it to text.
	std::wstring CFoundAddress::ToText( const uint8_t * _pui8Blob ) const {
		switch ( m_vtValueType ) {
			case CUtilities::MX_VT_DATA_TYPE : {
				switch ( m_dtDataType ) {
					case CUtilities::MX_DT_INT8 : {
						char szBuffer[32];
						int32_t i32Temp = (*reinterpret_cast<const int8_t *>(_pui8Blob));
						CUtilities::ByteToCString( (*_pui8Blob), szBuffer, FALSE, FALSE );
						//int iOffset = std::swprintf( _pwStr, _iMaxLen, L"%d (%S)", i32Temp, szBuffer );
						return std::format( L"{} ({})", i32Temp, CUtilities::XStringToWString( szBuffer, 0 ) );
					}
					default : {
						std::wstring wsTmp;
						CUtilities::ToDataTypeString( _pui8Blob, m_dtDataType, wsTmp );
						return wsTmp;
					}
				}
				break;
			}
			case CUtilities::MX_VT_STRING : {
				break;
			}
			case CUtilities::MX_VT_POINTER : {
				break;
			}
			case CUtilities::MX_VT_BLOB : {
				break;
			}
		}
		return std::wstring();
	}

	// Gets the Type text.
	std::wstring CFoundAddress::TypeText() const {
		switch ( m_vtValueType ) {
			case CUtilities::MX_VT_DATA_TYPE : {
				std::wstring wsTmp;
				CUtilities::PrintDataType( wsTmp, m_dtDataType, CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES );
				return wsTmp;
			}
			case CUtilities::MX_VT_STRING : {
				break;
			}
			case CUtilities::MX_VT_POINTER : {
				break;
			}
			case CUtilities::MX_VT_BLOB : {
				break;
			}
		}
		return std::wstring();
	}

	// Sets the Value Type.
	bool CFoundAddress::SetValueType( CUtilities::MX_VALUE_TYPE _vtType ) {
		m_vtValueType = _vtType;
		switch ( m_vtValueType ) {
			case CUtilities::MX_VT_DATA_TYPE : {}		MX_FALLTHROUGH
			case CUtilities::MX_VT_STRING : {}			MX_FALLTHROUGH
			case CUtilities::MX_VT_POINTER : {}			MX_FALLTHROUGH
			case CUtilities::MX_VT_BLOB : {	
				return true;
			}
		}
		return false;
	}

	// Sets the Data Type.
	bool CFoundAddress::SetDataType( CUtilities::MX_DATA_TYPES _dtDataType ) {
		if ( !CUtilities::IsDataType( _dtDataType ) ) { return false; }
		m_dtDataType = _dtDataType;
		UpdateBuffers();
		return true;
	}

	// Gets the size of the internal buffer needed to contain the data type.
	size_t CFoundAddress::InternalBufferSize() const {
		switch ( m_vtValueType ) {
			case CUtilities::MX_VT_DATA_TYPE : {
				return CUtilities::DataTypeSize( m_dtDataType );
			}
			case CUtilities::MX_VT_STRING : {
				break;
			}
			case CUtilities::MX_VT_POINTER : {
				return CUtilities::DataTypeSize( m_dtDataType );
			}
			case CUtilities::MX_VT_BLOB : {
				break;
			}
		}
		return 0;
	}

	// Update internal buffers after the size of the item changes.
	void CFoundAddress::UpdateBuffers() {
		auto sSize = InternalBufferSize();
		if ( sSize ) {
			try {
				m_vOriginalData.resize( sSize );
				m_vLockedData.resize( sSize );
			}
			catch ( ... ) {}
		}
	}

}	// namespace mx

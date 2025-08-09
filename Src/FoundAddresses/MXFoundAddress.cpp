#include "MXFoundAddress.h"
#include "../MemHack/MXMemHack.h"

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
		m_bBasicAddress = true;

		m_vtValueType = CUtilities::MX_VT_DATA_TYPE;
		m_dtDataType = _dtType;
		try {
			m_vOriginalData.resize( CUtilities::DataTypeSize( _dtType ) );
			std::memcpy( m_vOriginalData.data(), _pui8Data, m_vOriginalData.size() );
			m_vLockedData = m_vOriginalData;
			PrepareValueStructures();
			Dirty();
		}
		catch ( ... ) {
			return false;
		}
		return true;
	}

	// Gets the Address text.
	std::wstring CFoundAddress::AddressText() const {
		auto aAddr = FinalAddress();
		if ( m_bBasicAddress ) {
			std::wstring wTmp;
			CUtilities::ToHex( m_ui64Address, wTmp, m_ui64Address > UINT_MAX ? 12 : 8, false );
			return wTmp;
		}
		return std::wstring();
	}

	// Gets the Value text.
	std::wstring CFoundAddress::ValueText( bool * _pbRead ) const {
		if MX_UNLIKELY( _pbRead ) {
			(*_pbRead) = false;
		}
		if MX_LIKELY( m_bDirtyCurValue ) {
			m_bDirtyCurValue = !UndirtyCurValue();
		}
		if ( m_bDirtyCurValue ) {
			// Failed to read RAM.
			return std::wstring( L"N/A" );
		}
		try {
			auto ui32Elements = std::max<uint32_t>( m_ui32ArrayLen, 1 );
			if MX_LIKELY( m_bContiguous ) {
				if MX_UNLIKELY( !m_vCurData.vValue.size() ) {
					std::wstring( L"<C>" );
				}
			}
			else if MX_UNLIKELY( ui32Elements > m_vCurData.vValue.size() ) {
				std::wstring( L"<A>" );
			}
			std::wstring wsReturn;
			for ( uint32_t I = 0; I < ui32Elements; ++I ) {
				if ( wsReturn.size() ) {
					wsReturn += L' ';
				}
				if ( m_bContiguous ) {
					size_t sIdx = m_vCurData.vOffsets[0] + I * m_ui32DataTypeSize;
					if MX_LIKELY( sIdx + m_ui32DataTypeSize <= m_vCurData.vValue[0].size() ) {
						wsReturn += ToText( &m_vCurData.vValue[0][sIdx] );
					}
					else {
						wsReturn += L"<E>";
					}
				}
				else {
					size_t sIdx = m_vCurData.vOffsets[I];
					if MX_LIKELY( sIdx + m_ui32DataTypeSize <= m_vCurData.vValue[I].size() ) {
						wsReturn += ToText( &m_vCurData.vValue[I][sIdx] );
					}
					else {
						wsReturn += L"<E>";
					}
				}
			}
			if MX_UNLIKELY( _pbRead ) {
				(*_pbRead) = true;
			}
			return wsReturn;
		}
		catch ( ... ) { return std::wstring( L"---" ); }
		return std::wstring();
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
			case CUtilities::MX_VT_BLOB : {	
				return true;
			}
		}
		return false;
	}

	// Sets the Data Type.  Call within a try/catch block.
	bool CFoundAddress::SetDataType( CUtilities::MX_DATA_TYPES _dtDataType ) {
		if ( !CUtilities::IsDataType( _dtDataType ) ) { return false; }
		m_dtDataType = _dtDataType;
		PrepareValueStructures();
		UpdateBuffers();						// TODO: Merge into PrepareValueStructures().
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
			case CUtilities::MX_VT_BLOB : {
				break;
			}
		}
		return 0;
	}

	// Update internal buffers after the size of the item changes.
	void CFoundAddress::UpdateBuffers() {		// TODO: Merge into PrepareValueStructures().
		auto sSize = InternalBufferSize();
		if ( sSize ) {
			try {
				m_vOriginalData.resize( sSize );
				m_vLockedData.resize( sSize );
			}
			catch ( ... ) {}
		}
	}

	// Gets the actual final address of the item.
	uint64_t CFoundAddress::GetUpdatedFinalAddress() const {
		if ( m_bBasicAddress ) {
			return m_ui64Address;
		}
		if MX_UNLIKELY( nullptr == m_eAddressExp.GetContainer() ) {
			if ( !m_eAddressExp.SetExpression( m_sAddressTxt.c_str() ) ) { return 0; }
		}
		if MX_LIKELY( m_eAddressExp.GetContainer() ) {
			return uint64_t( m_eAddressExp.Evaluate() );
		}
		return 0;
	}

	// Undirty the current value.
	bool CFoundAddress::UndirtyCurValue() const {
		if MX_LIKELY( PrepareCurValueBuffer() ) {
			auto ui64Addr = m_ui64FinalTargetPreparedAddress;
			if MX_LIKELY( m_pmhMemHack ) {
				auto ui32Elements = std::max<uint32_t>( m_ui32ArrayLen, 1 );
				if MX_LIKELY( m_bContiguous ) {
					return m_pmhMemHack->ReadProcessMemory_PreProcessed( ui64Addr, m_vCurData.vValue[0], m_ui32DataTypeSize * ui32Elements, m_vCurData.vOffsets[0], m_bsByteSwap );
				}
				for ( uint32_t I = 0; I < ui32Elements; ++I ) {
					if ( !m_pmhMemHack->ReadProcessMemory_PreProcessed( ui64Addr, m_vCurData.vValue[I], m_ui32DataTypeSize * ui32Elements, m_vCurData.vOffsets[I], m_bsByteSwap ) ) { return false; }
					ui64Addr += m_ui16ActuaArrayStride;
				}
			}
		}
		return false;
	}

	// Prepares the MX_VALUE structures based on the value type, array length/stride.  Call within a try/catch block.
	void CFoundAddress::PrepareValueStructures() const {
		if ( CUtilities::IsDataType( m_dtDataType ) ) {
			m_ui32DataTypeSize = CUtilities::DataTypeSize( m_dtDataType );
			m_ui16ActuaArrayStride = std::max<uint16_t>( m_ui16ArrayStride, m_ui32DataTypeSize );
			m_bContiguous = m_ui16ActuaArrayStride == m_ui32DataTypeSize;

			if ( !PrepareCurValueBuffer() ) { throw; }
		}
	}

	// Updates the current-data buffer(s) without reading into them.
	bool CFoundAddress::PrepareCurValueBuffer() const {
		bool bReadAddressChain;
		auto ui64FinalAddr = FinalTargetAddress( &bReadAddressChain );
		if MX_UNLIKELY( bReadAddressChain && m_ui64FinalTargetPreparedAddress != ui64FinalAddr ) {
			try {
				auto ui32Elements = std::max<uint32_t>( m_ui32ArrayLen, 1 );
				if ( m_bContiguous ) {
					m_vCurData.vValue.resize( 1 );
					m_vCurData.vOffsets.resize( 1 );
					uint64_t ui64Addr = ui64FinalAddr;
					uint64_t ui64Len = m_ui32DataTypeSize * ui32Elements;
					CUtilities::SnapTo( CUtilities::ByteSwapSize( m_bsByteSwap ), ui64Addr, ui64Len, m_vCurData.vOffsets[0] );
					if MX_UNLIKELY( ui64Len > MAXSIZE_T ) { return false; }
					m_vCurData.vValue[0].resize( size_t( ui64Len ) );
				}
				else {
					m_vCurData.vValue.resize( ui32Elements );
					m_vCurData.vOffsets.resize( ui32Elements );
					uint64_t ui64Addr = ui64FinalAddr;
					for ( uint32_t I = 0; I < ui32Elements; ++I ) {
						uint64_t ui64ThisAddr = ui64Addr;
						uint64_t ui64Len = m_ui32DataTypeSize;
						CUtilities::SnapTo( CUtilities::ByteSwapSize( m_bsByteSwap ), ui64ThisAddr, ui64Len, m_vCurData.vOffsets[I] );
						if MX_UNLIKELY( ui64Len > MAXSIZE_T ) { return false; }
						m_vCurData.vValue[I].resize( size_t( ui64Len ) );

						ui64Addr += m_ui16ActuaArrayStride;
					}
				}
				m_ui64FinalTargetPreparedAddress = ui64FinalAddr;
			}
			catch ( ... ) { return false; }
		}
			
		return true;
	}

	// Gets the final target address via derefencing pointers.
	uint64_t CFoundAddress::FinalTargetAddress( bool * _bSuccess ) const {
#define MX_FAIL			if ( _bSuccess ) { (*_bSuccess) = false; return 0ULL; }
		if MX_UNLIKELY( m_pmhMemHack == nullptr ) { MX_FAIL; }
		auto ui64Addr = FinalAddress();

		uint8_t ui8Cach[16];
		size_t sOffset;
		size_t sSize = m_pmhMemHack->Process().Is32Bit() ? 4 : 8;
		for ( size_t I = 0; I < m_vPointers.size(); ++I ) {
			if ( !m_pmhMemHack->ReadProcessMemory_PreProcessed( ui64Addr, ui8Cach, sSize, sOffset, m_bsByteSwap ) ) {
				MX_FAIL;
			}
			ui64Addr = (*reinterpret_cast<uint64_t *>(&ui8Cach[sOffset]));

			ui64Addr += uint64_t( int64_t( m_vPointers[I].i32Offset ) );
		}
		return ui64Addr;
#undef MX_FAIL
	}

}	// namespace mx

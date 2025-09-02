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
			m_vLockedData.clear();
			m_vLockedData.push_back( m_vOriginalData );
			PrepareValueStructures();
			Dirty();
		}
		catch ( ... ) {
			return false;
		}
		return true;
	}

	// Sets the address, string type, and original data.
	bool CFoundAddress::InitWithAddressandStringType( uint64_t _ui64Address, UINT _uiCodePage, const uint8_t * _pui8Data, size_t _sLen ) {
		m_eAddressExp.Reset();
		m_sAddressTxt.clear();
		m_sAddressTxt = std::string();
		m_ui64Address = _ui64Address;
		m_bBasicAddress = true;

		try {
			std::string sTmp( reinterpret_cast<const char *>(_pui8Data),
				reinterpret_cast<const char *>(_pui8Data) + _sLen );
			SetAsString( sTmp, _uiCodePage );
		}
		catch ( ... ) { return false; }
		return true;
	}

	// Sets the Data Type.  Call within a try/catch block.
	void CFoundAddress::SetAsDataType( std::vector<std::vector<uint8_t>> &_vLockValue, CUtilities::MX_DATA_TYPES _dtDataType, uint32_t _ui32ArrayLen, uint32_t _ui32ArrayStride, std::vector<std::vector<uint8_t>> * _pvMaxLock ) {
		m_vtValueType = CUtilities::MX_VT_DATA_TYPE;
		m_dtDataType = _dtDataType;
		m_ui32ArrayLen = _ui32ArrayLen;
		m_ui16ArrayStride = _ui32ArrayStride;

		m_vLockedData = std::move( _vLockValue );
		if ( _pvMaxLock ) {
			m_vLockedDataMax = std::move( (*_pvMaxLock) );
		}

		PrepareValueStructures();
		m_bDirtyLockedLeft = true;
		Dirty();
	}

	// Sets the data type as a string.
	void CFoundAddress::SetAsString( const std::string &_sLockString, UINT _uiCodePage ) {
		m_vtValueType = CUtilities::MX_VT_STRING;
		m_uiCodePage = _uiCodePage;

		m_vLockedData.clear();
		m_vLockedData.push_back( std::vector<uint8_t>( reinterpret_cast<const uint8_t *>(_sLockString.data()),
			reinterpret_cast<const uint8_t *>(_sLockString.data()) + _sLockString.size() ) );
		m_bDirtyLockedLeft = true;
		Dirty();
	}

	// Gets the Address text.
	std::wstring CFoundAddress::AddressText() const {
		auto aAddr = FinalAddress();
		if ( m_bBasicAddress ) {
			std::wstring wTmp;
			//CUtilities::ToHex( aAddr, wTmp, aAddr > UINT_MAX ? 12 : 8, false );
			CUtilities::PrintAddress( aAddr, wTmp, false );
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
			if ( m_vtValueType == CUtilities::MX_VT_DATA_TYPE ) {
				auto ui32Elements = std::max<uint32_t>( m_ui32ArrayLen, 1 );
				if MX_LIKELY( m_bContiguous ) {
					if MX_UNLIKELY( !m_vCurData.vValue.size() ) {
						return std::wstring( L"<C>" );
					}
				}
				else if MX_UNLIKELY( ui32Elements > m_vCurData.vValue.size() ) {
					return std::wstring( L"<A>" );
				}
				CSecureWString wsReturn, swsError;
				CUtilities::ArrayBytesToWString( m_vCurData.vValue, &m_vCurData.vOffsets, wsReturn, m_dtDataType, ui32Elements, 0, m_bContiguous, swsError );
				if ( swsError.size() ) {
					return swsError;
				}
				if MX_UNLIKELY( _pbRead ) {
					(*_pbRead) = true;
				}
				return wsReturn;
			}
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
				return _DEC_WS_9912B79F_String;
			}
			case CUtilities::MX_VT_BLOB : {
				break;
			}
		}
		return std::wstring();
	}

	// Sets the locked status.
	void CFoundAddress::SetLocked( bool _bLocked ) {
		m_bLocked = _bLocked;
	}

	// Applies the lock.  A return of false indicates the process could not be written (or potentially read if needed).
	bool CFoundAddress::ApplyLock( bool _bEvenIfNotLocked ) {
		if ( !(m_bLocked || _bEvenIfNotLocked) ) { return true; }
		bool bSuccess = false;
		auto ui64Addr = FinalTargetAddress( &bSuccess );
		if MX_UNLIKELY( !bSuccess ) { return false; }


		if MX_LIKELY( m_vtValueType == CUtilities::MX_VT_DATA_TYPE ) {
			return WriteProcessMemory_PreProcessed( m_pmhMemHack, ui64Addr, m_vLockedData, m_bContiguous, m_ui16ActuaArrayStride, PreProcessing() );
		}
		if ( m_vtValueType == CUtilities::MX_VT_STRING ) {
			return m_pmhMemHack->WriteProcessMemory_PreProcessed( ui64Addr, m_vLockedData[0] );
		}
		return false;
	}

	// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
	bool CFoundAddress::SaveSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary ) const {
		if ( _peJson == nullptr && nullptr == _psBinary ) { return false; }
		if ( !CFoundAddressBase::SaveSettings( _peJson, _psBinary ) ) { return false; }
		try {
			if ( _peJson ) {
				_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
				lson::CJson::CreateStringElement( _DEC_S_6E13FA7E_AddressExpression, ee::CExpEval::ToJsonString( m_sAddressTxt ), (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );

				MX_JSON_NUMBER( _DEC_S_C2F3561D_Address, m_ui64Address, _peJson );
				MX_JSON_NUMBER( _DEC_S_F0717BC3_PublicId, m_ui32PublicId, _peJson );

				MX_JSON_NUMBER( _DEC_S_B083E9A2_ValueType, m_vtValueType, _peJson );
				MX_JSON_NUMBER( _DEC_S_DC7D620E_DataType, m_dtDataType, _peJson );
				MX_JSON_NUMBER( _DEC_S_E8055518_CodePage, m_uiCodePage, _peJson );
				MX_JSON_NUMBER( _DEC_S_12EECDA8_ByteSwap, m_bsByteSwap, _peJson );
				MX_JSON_NUMBER( _DEC_S_0BC42864_LockType, m_ltLockType, _peJson );

				MX_JSON_NUMBER( _DEC_S_C46D6620_ArrayLen, m_ui32ArrayLen, _peJson );
				MX_JSON_NUMBER( _DEC_S_09286372_ArrayStride, m_ui16ArrayStride, _peJson );

				_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
				lson::CJson::CreateArrayElement( _DEC_S_3A4782EF_Pointers, (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );
				{
					lson::CJson::LSON_ELEMENT * peArray = _peJson->vObjectMembers[_peJson->vObjectMembers.size()-1].get();
					for ( size_t I = 0; I < m_vPointers.size(); ++I ) {
						MX_JSON_NUMBER( "", m_vPointers[I].i32Offset, peArray );
					}
				}

				_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
				lson::CJson::CreateArrayElement( _DEC_S_C00CB9AD_ValueWhenLocked, (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );
				{
					lson::CJson::LSON_ELEMENT * peArray = _peJson->vObjectMembers[_peJson->vObjectMembers.size()-1].get();
					for ( size_t I = 0; I < m_vLockedData.size(); ++I ) {
						peArray->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
						lson::CJson::CreateStringElement( "", ee::CExpEval::ToHexString( m_vLockedData[I], false ), (*peArray->vObjectMembers[peArray->vObjectMembers.size()-1]) );
					}
				}

				_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
				lson::CJson::CreateArrayElement( _DEC_S_482AF340_ValueWhenLockedMax, (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );
				{
					lson::CJson::LSON_ELEMENT * peArray = _peJson->vObjectMembers[_peJson->vObjectMembers.size()-1].get();
					for ( size_t I = 0; I < m_vLockedDataMax.size(); ++I ) {
						peArray->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
						lson::CJson::CreateStringElement( "", ee::CExpEval::ToHexString( m_vLockedDataMax[I], false ), (*peArray->vObjectMembers[peArray->vObjectMembers.size()-1]) );
					}
				}

				_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
				lson::CJson::CreateStringElement( _DEC_S_389827E8_OriginalValue, ee::CExpEval::ToHexString( m_vOriginalData, false ), (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );

				MX_JSON_BOOL( _DEC_S_DA9281A7_BasicAddress, m_bBasicAddress, _peJson );
				MX_JSON_BOOL( _DEC_S_ED7ECDA7_Locked, m_bLocked, _peJson );
			}
			else {
				if ( !_psBinary->WriteString( m_sAddressTxt ) ) { return false; }

				if ( !_psBinary->Write( m_ui64Address ) ) { return false; }
				if ( !_psBinary->Write( m_ui32PublicId ) ) { return false; }

				if ( !_psBinary->Write( m_vtValueType ) ) { return false; }
				if ( !_psBinary->Write( m_dtDataType ) ) { return false; }
				if ( !_psBinary->Write( m_uiCodePage ) ) { return false; }
				if ( !_psBinary->Write( m_bsByteSwap ) ) { return false; }
				if ( !_psBinary->Write( m_ltLockType ) ) { return false; }

				if ( !_psBinary->Write( m_ui32ArrayLen ) ) { return false; }
				if ( !_psBinary->Write( m_ui16ArrayStride ) ) { return false; }

				if ( !_psBinary->WriteUi16( m_vPointers.size() ) ) { return false; }
				for ( size_t I = 0; I < m_vPointers.size(); ++I ) {
					if ( !_psBinary->Write( m_vPointers[I].i32Offset ) ) { return false; }
				}

				if ( !_psBinary->WriteUi32( m_vLockedData.size() ) ) { return false; }
				for ( size_t I = 0; I < m_vLockedData.size(); ++I ) {
					if ( !_psBinary->WriteSizeAndBytes( m_vLockedData[I] ) ) { return false; }
				}

				if ( !_psBinary->WriteUi32( m_vLockedDataMax.size() ) ) { return false; }
				for ( size_t I = 0; I < m_vLockedDataMax.size(); ++I ) {
					if ( !_psBinary->WriteSizeAndBytes( m_vLockedDataMax[I] ) ) { return false; }
				}

				if ( !_psBinary->WriteSizeAndBytes( m_vOriginalData ) ) { return false; }

				if ( !_psBinary->Write( m_bBasicAddress ) ) { return false; }
				if ( !_psBinary->Write( m_bLocked ) ) { return false; }
			}
		}
		catch ( ... ) { return false; }
		return true;
	}

	// Loads settings from either a JSON object or a byte buffer.
	bool CFoundAddress::LoadSettings( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, uint32_t _ui32Version, size_t &_sId ) {
		if ( _ui32Version > MX_FOUND_ADDRESS_VERSION ) { return false; }
		if ( !CFoundAddressBase::LoadSettings( _pjJson, _pjcContainer, _psBinary, _ui32Version, _sId ) ) { return false; }
		if ( nullptr == _pjJson && _psBinary == nullptr ) { return false; }
		try {
			if ( _pjJson ) {
				const lson::CJsonContainer::LSON_JSON_VALUE * pjvVal;
				CSecureWString swsTmp;
				MX_JSON_GET_STRING( _DEC_S_6E13FA7E_AddressExpression, swsTmp, pjvVal, _pjJson );
				m_sAddressTxt = ee::CExpEval::ToUtf8( swsTmp );

				MX_JSON_GET_NUMBER( _DEC_S_C2F3561D_Address, m_ui64Address, uint64_t, pjvVal, _pjJson );
				MX_JSON_GET_NUMBER( _DEC_S_F0717BC3_PublicId, m_ui32PublicId, uint32_t, pjvVal, _pjJson );

				MX_JSON_GET_NUMBER( _DEC_S_B083E9A2_ValueType, m_vtValueType, CUtilities::MX_VALUE_TYPE, pjvVal, _pjJson );
				MX_JSON_GET_NUMBER( _DEC_S_DC7D620E_DataType, m_dtDataType, CUtilities::MX_DATA_TYPES, pjvVal, _pjJson );
				MX_JSON_GET_NUMBER( _DEC_S_E8055518_CodePage, m_uiCodePage, UINT, pjvVal, _pjJson );
				MX_JSON_GET_NUMBER( _DEC_S_12EECDA8_ByteSwap, m_bsByteSwap, CUtilities::MX_BYTESWAP, pjvVal, _pjJson );
				MX_JSON_GET_NUMBER( _DEC_S_0BC42864_LockType, m_ltLockType, CUtilities::MX_LOCK_TYPES, pjvVal, _pjJson );

				MX_JSON_GET_NUMBER( _DEC_S_C46D6620_ArrayLen, m_ui32ArrayLen, uint32_t, pjvVal, _pjJson );
				MX_JSON_GET_NUMBER( _DEC_S_09286372_ArrayStride, m_ui16ArrayStride, uint16_t, pjvVal, _pjJson );

				{
					const lson::CJsonContainer::LSON_JSON_VALUE * pjvKeys = _pjcContainer->GetMemberByName( (*_pjJson), _DEC_S_3A4782EF_Pointers );
					if ( !pjvKeys || pjvKeys->vtType != lson::CJsonContainer::LSON_VT_ARRAY ) { return false; }

					for ( size_t I = 0; I < pjvKeys->vArray.size(); ++I ) {
						const lson::CJsonContainer::LSON_JSON_VALUE & jvArrayVal = _pjcContainer->GetValue( pjvKeys->vArray[I] );
						if ( jvArrayVal.vtType == lson::CJsonContainer::LSON_VT_DECIMAL ) {
							MX_POINTER pThis = { .i32Offset = int32_t( std::round( jvArrayVal.u.dDecimal ) ) };
							m_vPointers.push_back( pThis );
						}
					}
				}
				{
					const lson::CJsonContainer::LSON_JSON_VALUE * pjvKeys = _pjcContainer->GetMemberByName( (*_pjJson), _DEC_S_C00CB9AD_ValueWhenLocked );
					if ( !pjvKeys || pjvKeys->vtType != lson::CJsonContainer::LSON_VT_ARRAY ) { return false; }

					for ( size_t I = 0; I < pjvKeys->vArray.size(); ++I ) {
						const lson::CJsonContainer::LSON_JSON_VALUE & jvArrayVal = _pjcContainer->GetValue( pjvKeys->vArray[I] );
						if ( jvArrayVal.vtType == lson::CJsonContainer::LSON_VT_STRING ) {
							m_vLockedData.push_back( std::vector<uint8_t>() );
							bool bSuccess = false;
							m_vLockedData[m_vLockedData.size()-1] = ee::CExpEval::FromHexString<std::vector<uint8_t>>( _pjcContainer->GetString( jvArrayVal.u.stString ), &bSuccess );
							if ( !bSuccess ) { return false; }
						}
					}
				}
				{
					const lson::CJsonContainer::LSON_JSON_VALUE * pjvKeys = _pjcContainer->GetMemberByName( (*_pjJson), _DEC_S_482AF340_ValueWhenLockedMax );
					if ( !pjvKeys || pjvKeys->vtType != lson::CJsonContainer::LSON_VT_ARRAY ) { return false; }

					for ( size_t I = 0; I < pjvKeys->vArray.size(); ++I ) {
						const lson::CJsonContainer::LSON_JSON_VALUE & jvArrayVal = _pjcContainer->GetValue( pjvKeys->vArray[I] );
						if ( jvArrayVal.vtType == lson::CJsonContainer::LSON_VT_STRING ) {
							m_vLockedDataMax.push_back( std::vector<uint8_t>() );
							bool bSuccess = false;
							m_vLockedDataMax[m_vLockedDataMax.size()-1] = ee::CExpEval::FromHexString<std::vector<uint8_t>>( _pjcContainer->GetString( jvArrayVal.u.stString ), &bSuccess );
							if ( !bSuccess ) { return false; }
						}
					}
				}
				pjvVal = _pjcContainer->GetMemberByName( (*_pjJson), _DEC_S_389827E8_OriginalValue );
				if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_STRING ) {
					bool bSuccess = false;
					m_vOriginalData = ee::CExpEval::FromHexString<std::vector<uint8_t>>( _pjcContainer->GetString( pjvVal->u.stString ), &bSuccess );
					if ( !bSuccess ) { return false; }
				}

				MX_JSON_GET_BOOL( _DEC_S_DA9281A7_BasicAddress, m_bBasicAddress, pjvVal, _pjJson );
				MX_JSON_GET_BOOL( _DEC_S_ED7ECDA7_Locked, m_bLocked, pjvVal, _pjJson );
			}
			else {
				CSecureString * pwsStr = reinterpret_cast<CSecureString *>(&m_sAddressTxt);
				if ( !_psBinary->ReadString( (*pwsStr) ) ) { return false; }

				if ( !_psBinary->Read( m_ui64Address ) ) { return false; }
				if ( _ui32Version <= 2 ) {
					uint32_t ui32Discard;
					if ( !_psBinary->Read( ui32Discard ) ) { return false; }
				}
				if ( _ui32Version >= 4 ) {
					if ( !_psBinary->Read( m_ui32PublicId ) ) { return false; }
				}
				else { m_ui32PublicId = 0; }

				if ( !_psBinary->Read( m_vtValueType ) ) { return false; }
				if ( !_psBinary->Read( m_dtDataType ) ) { return false; }
				if ( !_psBinary->Read( m_uiCodePage ) ) { return false; }
				if ( !_psBinary->Read( m_bsByteSwap ) ) { return false; }
				if ( !_psBinary->Read( m_ltLockType ) ) { return false; }

				if ( !_psBinary->Read( m_ui32ArrayLen ) ) { return false; }
				if ( !_psBinary->Read( m_ui16ArrayStride ) ) { return false; }

				uint16_t ui16Pointers;
				if ( !_psBinary->ReadUi16( ui16Pointers ) ) { return false; }
				for ( size_t I = 0; I < ui16Pointers; ++I ) {
					MX_POINTER pThis;
					if ( !_psBinary->Read( pThis.i32Offset ) ) { return false; }
					m_vPointers.push_back( pThis );
				}

				uint32_t ui32Tmp;
				if ( !_psBinary->ReadUi32( ui32Tmp ) ) { return false; }
				for ( size_t I = 0; I < ui32Tmp; ++I ) {
					m_vLockedData.push_back( std::vector<uint8_t>() );
					if ( !_psBinary->ReadSizeAndBytes( m_vLockedData[m_vLockedData.size()-1] ) ) { return false; }
				}

				if ( !_psBinary->ReadUi32( ui32Tmp ) ) { return false; }
				for ( size_t I = 0; I < ui32Tmp; ++I ) {
					m_vLockedDataMax.push_back( std::vector<uint8_t>() );
					if ( !_psBinary->ReadSizeAndBytes( m_vLockedDataMax[m_vLockedDataMax.size()-1] ) ) { return false; }
				}

				if ( !_psBinary->ReadSizeAndBytes( m_vOriginalData ) ) { return false; }

				if ( !_psBinary->Read( m_bBasicAddress ) ) { return false; }
				if ( !_psBinary->Read( m_bLocked ) ) { return false; }
			}
		}
		catch ( ... ) { return false; }
		PrepareValueStructures();
		m_bDirtyLockedLeft = true;
		Dirty();
		return true;
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
		auto ui64Addr = FinalTargetAddress();
		if MX_LIKELY( m_pmhMemHack ) {
			auto ui32Elements = std::max<uint32_t>( m_ui32ArrayLen, 1 );
			if MX_LIKELY( m_bContiguous ) {
				if MX_UNLIKELY( m_vCurData.vValue.size() != 1 ) {
					m_vCurData.vValue.resize( 1 );
				}
				if MX_UNLIKELY( m_vCurData.vOffsets.size() != 1 ) {
					m_vCurData.vOffsets.resize( 1 );
				}
				return m_pmhMemHack->ReadProcessMemory_PreProcessed( ui64Addr, m_vCurData.vValue[0], m_ui32DataTypeSize * ui32Elements, m_vCurData.vOffsets[0], m_bsByteSwap );
			}

			if MX_UNLIKELY( m_vCurData.vValue.size() != ui32Elements ) {
				m_vCurData.vValue.resize( ui32Elements );
			}
			if MX_UNLIKELY( m_vCurData.vOffsets.size() != ui32Elements ) {
				m_vCurData.vOffsets.resize( ui32Elements );
			}
			for ( uint32_t I = 0; I < ui32Elements; ++I ) {
				if ( !m_pmhMemHack->ReadProcessMemory_PreProcessed( ui64Addr, m_vCurData.vValue[I], m_ui32DataTypeSize, m_vCurData.vOffsets[I], m_bsByteSwap ) ) { return false; }
				ui64Addr += m_ui16ActuaArrayStride;
			}
			return true;
		}
		return false;
	}

	// Prepares the MX_VALUE structures based on the value type, array length/stride.  Call within a try/catch block.
	void CFoundAddress::PrepareValueStructures() const {
		if ( CUtilities::IsDataType( m_dtDataType ) ) {
			m_ui32DataTypeSize = CUtilities::DataTypeSize( m_dtDataType );
			m_ui16ActuaArrayStride = std::max<uint16_t>( m_ui16ArrayStride, m_ui32DataTypeSize );
			m_bContiguous = m_ui16ActuaArrayStride == m_ui32DataTypeSize;

			//if ( !PrepareCurValueBuffer() ) { throw; }
		}
	}

	// Gets the final target address via derefencing pointers.
	uint64_t CFoundAddress::FinalTargetAddress( bool * _pbSuccess ) const {
#define MX_FAIL			if ( _pbSuccess ) { (*_pbSuccess) = false; return 0ULL; }
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
		if ( _pbSuccess ) { (*_pbSuccess) = true; }
		return ui64Addr;
#undef MX_FAIL
	}

	// Updates/gets the locked left text.
	const CSecureWString & CFoundAddress::LockedLeftText() const {
		if MX_UNLIKELY( m_bDirtyLockedLeft ) {
			CSecureString ssTmp;
			ssTmp.assign( reinterpret_cast<const char *>( m_vLockedData[0].data() ),
			reinterpret_cast<const char *>( m_vLockedData[0].data() ) + m_vLockedData[0].size() );
			m_swsLockedLeftText = CUtilities::MultiByteToWideChar( CodePage(), 0, ssTmp );
			m_bDirtyLockedLeft = false;
		}
		return m_swsLockedLeftText;
	}

}	// namespace mx

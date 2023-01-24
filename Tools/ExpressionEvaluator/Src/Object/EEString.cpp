#include "EEString.h"
#include "EEStringBaseApi.h"


namespace ee {

	// == Functions.
	// Converts to another object of the given type.
	CExpEvalContainer::EE_RESULT CString::ConvertTo( EE_NUM_CONSTANTS _ncType ) {
		CExpEvalContainer::EE_RESULT rRet;
		switch ( _ncType ) {
			case EE_NC_UNSIGNED : {
				bool bOverflowed = false;
				size_t sEaten = 0;
				rRet.u.ui64Val = ee::CExpEval::StoULL( m_sObj.c_str(), 10, &sEaten, UINT64_MAX, &bOverflowed );
				if ( !bOverflowed && sEaten ) {
					rRet.ncType = _ncType;
					return rRet;
				}
				break;
			}
			case EE_NC_SIGNED : {
				bool bOverflowed = false;
				size_t sEaten = 0;
				rRet.u.ui64Val = ee::CExpEval::StoULL( m_sObj.c_str(), 10, &sEaten, UINT64_MAX / 2ULL + 1ULL, &bOverflowed );
				if ( !bOverflowed && sEaten ) {
					rRet.ncType = _ncType;
					return rRet;
				}
				break;
			}
			case EE_NC_FLOATING : {
				bool bErrored = false;
				size_t sEaten = 0;
				rRet.u.dVal = ee::CExpEval::AtoF( m_sObj.c_str(), &sEaten, &bErrored );
				if ( !bErrored && sEaten ) {
					rRet.ncType = _ncType;
					return rRet;
				}
				break;
			}
		}
		rRet.ncType = EE_NC_INVALID;
		rRet.u.poObj = nullptr;
		return rRet;
	}

	// Initializes this object given another object.
	bool CString::InitializeFrom( const CExpEvalContainer::EE_RESULT &_rObj ) {
		switch ( _rObj.ncType ) {
			case EE_NC_UNSIGNED : {
				try {
					m_sObj = std::to_string( _rObj.u.ui64Val );
				}
				catch ( const std::bad_alloc & ) {
					return false;
				}
				return true;
			}
			case EE_NC_SIGNED : {
				try {
					m_sObj = std::to_string( _rObj.u.i64Val );
				}
				catch ( const std::bad_alloc & ) {
					return false;
				}
				return true;
			}
			case EE_NC_FLOATING : {
				try {
					m_sObj = std::to_string( _rObj.u.dVal );
				}
				catch ( const std::bad_alloc & ) {
					return false;
				}
				return true;
			}
			case EE_NC_OBJECT : {
				if ( _rObj.u.poObj ) {
					 return _rObj.u.poObj->ToString( m_sObj );
				}
			}
		}
		return false;
	}

	// Array access.
	CExpEvalContainer::EE_RESULT CString::ArrayAccess( int64_t _i64Idx ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_UNSIGNED };
		
		size_t sIdx;
		if ( _i64Idx < 0 ) {
			if ( !m_ui64Len ) { m_ui64Len = ee::CExpEval::CountUtfCodePoints( m_sObj ); }
			if ( m_ui64Len != static_cast<size_t>(m_ui64Len) ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }

			sIdx = ArrayIndexToLinearIndex( _i64Idx, static_cast<size_t>(m_ui64Len) );
		}
		else {
			sIdx = static_cast<size_t>(_i64Idx);
			if ( static_cast<int64_t>(sIdx) != _i64Idx ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }
		}
		uint32_t ui32Val;
		CStringBaseApi::UpdateArrayAccessOptimizer( m_sObj, m_vArrayOpt, sIdx, ui32Val );
		rRet.u.ui64Val = ui32Val;
		return rRet;
	}

	// Extended array access.
	CExpEvalContainer::EE_RESULT CString::ArrayAccessEx( int64_t _i64Idx0, int64_t _i64Idx1, uint32_t _ui32Mask ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };
		
		size_t sIdx0 = 0;
		if ( _ui32Mask & EE_AEF_START ) {
			if ( _i64Idx0 < 0 ) {
				if ( !m_ui64Len ) { m_ui64Len = ee::CExpEval::CountUtfCodePoints( m_sObj ); }
				if ( m_ui64Len != static_cast<size_t>(m_ui64Len) ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }

				sIdx0 = ArrayIndexToLinearIndex( _i64Idx0, static_cast<size_t>(m_ui64Len) );
			}
			else {
				sIdx0 = static_cast<size_t>(_i64Idx0);
				if ( static_cast<int64_t>(sIdx0) != _i64Idx0 ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }
			}
			sIdx0 = ee::CExpEval::GetUtf8CharPosByIdx( m_sObj, sIdx0 );
		}

		size_t sIdx1;
		if ( _ui32Mask & EE_AEF_END ) {
			if ( _i64Idx1 < 0 ) {
				if ( !m_ui64Len ) { m_ui64Len = ee::CExpEval::CountUtfCodePoints( m_sObj ); }
				if ( m_ui64Len != static_cast<size_t>(m_ui64Len) ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }

				sIdx1 = ArrayIndexToLinearIndex( _i64Idx1, static_cast<size_t>(m_ui64Len) );
			}
			else {
				sIdx1 = static_cast<size_t>(_i64Idx1);
				if ( static_cast<int64_t>(sIdx1) != _i64Idx1 ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }
			}
			sIdx1 = ee::CExpEval::GetUtf8CharPosByIdx( m_sObj, sIdx1 );
		}
		else {
			sIdx1 = m_sObj.size();
		}
		CString * psObj = reinterpret_cast<CString *>(m_peecContainer->AllocateObject<CString>());
		if ( psObj ) {
			try {
				for ( size_t I = sIdx0; I < sIdx1; ++I ) {
					psObj->m_sObj.push_back( m_sObj[I] );
				}
				psObj->Dirty();
				rRet = psObj->CreateResult();
			}
			catch ( ... ) {
				m_peecContainer->DeallocateObject( psObj );
			}
		}
		return rRet;
	}

	// Gets the array length.
	size_t CString::ArrayLength() {
		if ( !m_ui64Len ) { m_ui64Len = ee::CExpEval::CountUtfCodePoints( m_sObj ); }
		if ( m_ui64Len != size_t( m_ui64Len ) ) { return size_t( ~0 ); }
		return size_t( m_ui64Len );
	}

	// == Operators.
	// Operator ==.
	bool CString::Equals( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( _rRet.u.poObj && (_rRet.u.poObj->Type() & (EE_BIT_STRING)) ) {
				return std::strcmp( m_sObj.c_str(), static_cast<CString *>(_rRet.u.poObj)->m_sObj.c_str() ) == 0;
			}
			/*else if ( _rRet.u.poObj && (_rRet.u.poObj->Type() & (EE_BIT_STRING_REF)) ) {
				std::string sTmp;
				if ( !_rRet.u.poObj->ToString( sTmp ) ) { return false; }
				return std::strcmp( m_sObj.c_str(), static_cast<CStringRef *>(_rRet.u.poObj)->StringRef().c_str() ) == 0;
			}*/
		}
		return false;
	}

	// Operator +.
	CExpEvalContainer::EE_RESULT CString::Plus( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( _rRet.u.poObj && (_rRet.u.poObj->Type() & (EE_BIT_STRING)) ) {
				CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };
				
				CString * psObj = reinterpret_cast<CString *>(m_peecContainer->AllocateObject<CString>());
				if ( psObj ) {
					try {
						psObj->m_sObj = m_sObj + static_cast<CString *>(_rRet.u.poObj)->m_sObj;
						psObj->Dirty();
						rRet = psObj->CreateResult();
					}
					catch ( ... ) {
						m_peecContainer->DeallocateObject( psObj );
					}
				}

				return rRet;
			}
		}
		return { EE_NC_INVALID };
	}

	// Operator *.
	CExpEvalContainer::EE_RESULT CString::Multiply( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( (_rRet.ncType == EE_NC_SIGNED && _rRet.u.i64Val >= 0) || _rRet.ncType == EE_NC_UNSIGNED ) {
			CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };
			CString * psObj = reinterpret_cast<CString *>(m_peecContainer->AllocateObject<CString>());
			if ( psObj ) {
				try {
					psObj->m_sObj.reserve( size_t( m_sObj.size() * _rRet.u.ui64Val ) );
					for ( uint64_t I = 0; I < _rRet.u.ui64Val; ++I ) {
						psObj->m_sObj += m_sObj;
					}
					psObj->Dirty();
					rRet = psObj->CreateResult();
				}
				catch ( ... ) {
					m_peecContainer->DeallocateObject( psObj );
				}
			}			
			return rRet;
		}
		return { EE_NC_INVALID };
	}

	// Operator +=.
	bool CString::PlusEquals( CExpEvalContainer::EE_RESULT &_rRet ) {
		switch ( _rRet.ncType ) {
			case EE_NC_SIGNED : {}
			case EE_NC_UNSIGNED : {
				uint64_t ui64Val = _rRet.u.ui64Val;
				for ( auto I = 0; I < sizeof( _rRet.u.ui64Val ) / sizeof( char32_t ); ++I ) {
					uint32_t ui32Len;
					uint32_t ui32BackToUtf8 = ee::CExpEval::Utf32ToUtf8( static_cast<uint32_t>(ui64Val), ui32Len );
					if ( !ui32Len ) {
						m_sObj += std::to_string( ui64Val );
						break;
					}
					else {
						for ( uint32_t J = 0; J < ui32Len; ++J ) {
							m_sObj.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
							ui32BackToUtf8 >>= 8;
						}
					}
					ui64Val >>= 32ULL;
				}
				Dirty();
				_rRet = CreateResult();
				return true;
			}
			//case EE_NC_SIGNED : {
			//	//m_sObj += std::to_string( _rRet.u.i64Val );
			//	uint64_t ui64Val = _rRet.u.ui64Val;
			//	for ( auto I = 0; I < sizeof( _rRet.u.ui64Val ) / sizeof( char32_t ); ++I ) {
			//		uint32_t ui32Len;
			//		uint32_t ui32BackToUtf8 = Utf32ToUtf8( static_cast<uint32_t>(ui64Val), ui32Len );
			//		if ( !ui32Len ) {
			//			m_sObj += std::to_string( ui64Val );
			//			break;
			//		}
			//		else {
			//			for ( uint32_t J = 0; J < ui32Len; ++J ) {
			//				m_sObj.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
			//				ui32BackToUtf8 >>= 8;
			//			}
			//		}
			//		ui64Val >>= 32ULL;
			//	}
			//	Dirty();
			//	_rRet = CreateResult();
			//	return true;
			//}
			case EE_NC_FLOATING : {
				m_sObj += std::to_string( _rRet.u.dVal );
				Dirty();
				_rRet = CreateResult();
				return true;
			}
			case EE_NC_OBJECT : {
				if ( _rRet.u.poObj && (_rRet.u.poObj->Type() & (EE_BIT_STRING_REF | EE_BIT_STRING)) ) {
					std::string sTmp;
					if ( !_rRet.u.poObj->ToString( sTmp ) ) { return false; }
					m_sObj += sTmp;
					Dirty();
					_rRet = CreateResult();
					return true;
				}
				return false;
			}
		}
		return false;
	}

	// == Base API functions.
	// Gets the object as an ASCII string.
	CExpEvalContainer::EE_RESULT CString::Ascii() const {
		return CStringBaseApi::Ascii( m_sObj, m_peecContainer );
	}

	// Gets the binary form of the object as a string (0b****).
	CExpEvalContainer::EE_RESULT CString::Bin() const {
		return CStringBaseApi::Bin( m_sObj, m_peecContainer );	}

	// Gets the boolean form of the object as a string (0b****).
	CExpEvalContainer::EE_RESULT CString::Bool() const {
		return CStringBaseApi::Bool( m_sObj, m_peecContainer );
	}

	// Returns the character that represents the specified Unicode. 
	CExpEvalContainer::EE_RESULT CString::Chr() const {
		return CStringBaseApi::Chr( m_sObj, m_peecContainer );
	}

	// Gets the string interpreted to its best fit and then converted to float. 
	CExpEvalContainer::EE_RESULT CString::Float() const {
		return CStringBaseApi::Float( m_sObj, m_peecContainer );
	}

	// Gets the hexadecimal form of the object as a string (0x****).
	CExpEvalContainer::EE_RESULT CString::Hex() const {
		return CStringBaseApi::Hex( m_sObj, m_peecContainer );
	}

	// Gets the string interpreted to its best fit and then converted to int64_t. 
	CExpEvalContainer::EE_RESULT CString::Int() const {
		return CStringBaseApi::Int( m_sObj, m_peecContainer );
	}

	// Gets the octadecimal form of the object as a string (0o****).
	CExpEvalContainer::EE_RESULT CString::Oct() const {
		return CStringBaseApi::Oct( m_sObj, m_peecContainer );
	}

	// Returns the ordinal value of the object as a Unicode character (always EE_NC_UNSIGNED).
	CExpEvalContainer::EE_RESULT CString::Ord() const {
		return CStringBaseApi::Ord( m_sObj, m_peecContainer );
	}

	// Resets the object.
	void CString::Reset() {
		m_sObj.clear();
		Dirty();
	}

	// Resets only the optimization tables.
	void CString::Dirty() const {
		m_ui64Len = 0;
		m_vArrayOpt.clear();
	}

}	// namespace ee

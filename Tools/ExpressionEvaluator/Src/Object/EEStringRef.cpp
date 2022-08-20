#include "EEStringRef.h"
#include "EEString.h"
#include "EEStringBaseApi.h"

#include <iomanip>
#include <sstream>


namespace ee {

	// Converts to another object of the given type.
	CExpEvalContainer::EE_RESULT CStringRef::ConvertTo( EE_NUM_CONSTANTS _ncType ) {
		CExpEvalContainer::EE_RESULT rRet;
		switch ( _ncType ) {
			case EE_NC_UNSIGNED : {
				bool bOverflowed = false;
				size_t sEaten = 0;
				rRet.u.ui64Val = StoULL( m_peecContainer->m_vStrings[GetStrIdx()].c_str(), 10, &sEaten, UINT64_MAX, &bOverflowed );
				if ( !bOverflowed && sEaten ) {
					rRet.ncType = _ncType;
					return rRet;
				}
				break;
			}
			case EE_NC_SIGNED : {
				bool bOverflowed = false;
				size_t sEaten = 0;
				rRet.u.ui64Val = StoULL( m_peecContainer->m_vStrings[GetStrIdx()].c_str(), 10, &sEaten, UINT64_MAX / 2 + 1, &bOverflowed );
				if ( !bOverflowed && sEaten ) {
					rRet.ncType = _ncType;
					return rRet;
				}
				break;
			}
			case EE_NC_FLOATING : {
				bool bErrored = false;
				size_t sEaten = 0;
				rRet.u.dVal = AtoF( m_peecContainer->m_vStrings[GetStrIdx()].c_str(), &sEaten, &bErrored );
				if ( !bErrored && sEaten ) {
					rRet.ncType = _ncType;
					return rRet;
				}
				break;
			}
		}
		rRet.ncType = EE_NC_INVALID;
		return rRet;
	}

	// Array access.
	CExpEvalContainer::EE_RESULT CStringRef::ArrayAccess( int64_t _i64Idx ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_UNSIGNED };
		
		size_t sIdx;
		if ( _i64Idx < 0 ) {
			if ( !m_ui64Len ) { m_ui64Len = CountUtfCodePoints( m_peecContainer->m_vStrings[GetStrIdx()] ); }
			if ( m_ui64Len != static_cast<size_t>(m_ui64Len) ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }

			sIdx = ArrayIndexToLinearIndex( _i64Idx, static_cast<size_t>(m_ui64Len) );
		}
		else {
			sIdx = static_cast<size_t>(_i64Idx);
			if ( sIdx != _i64Idx ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }
		}
		uint32_t ui32Val;
		CStringBaseApi::UpdateArrayAccessOptimizer( m_peecContainer->m_vStrings[GetStrIdx()], m_vArrayOpt, sIdx, ui32Val );
		rRet.u.ui64Val = ui32Val;
		return rRet;
	}

	// == Operators.
	// Operator +=.
#if 0
	bool CStringRef::PlusEquals( CExpEvalContainer::EE_RESULT &_rRet ) {
		std::string sNewObj;
		if ( !ToString( sNewObj ) ) { return false; }

		switch ( _rRet.ncType ) {
			case EE_NC_UNSIGNED : {
				sNewObj += std::to_string( _rRet.u.ui64Val );
				CString * psCopy = m_peecContainer->AllocateObject<ee::CString>();
				if ( !psCopy ) { return false; }
				psCopy->SetString( sNewObj );
				_rRet.ncType = EE_NC_OBJECT;
				_rRet.u.poObj = psCopy;
				return true;
			}
			case EE_NC_SIGNED : {
				sNewObj += std::to_string( _rRet.u.i64Val );
				CString * psCopy = m_peecContainer->AllocateObject<ee::CString>();
				if ( !psCopy ) { return false; }
				psCopy->SetString( sNewObj );
				_rRet.ncType = EE_NC_OBJECT;
				_rRet.u.poObj = psCopy;
				return true;
			}
			case EE_NC_FLOATING : {
				sNewObj += std::to_string( _rRet.u.dVal );
				CString * psCopy = m_peecContainer->AllocateObject<ee::CString>();
				if ( !psCopy ) { return false; }
				psCopy->SetString( sNewObj );
				_rRet.ncType = EE_NC_OBJECT;
				_rRet.u.poObj = psCopy;
				return true;
			}
			case EE_NC_OBJECT : {
				if ( _rRet.u.poObj && (_rRet.u.poObj->Type() & (EE_BIT_STRING_REF | EE_BIT_STRING)) ) {
					std::string sTmp;
					if ( !_rRet.u.poObj->ToString( sTmp ) ) { return false; }
					sNewObj += sTmp;
					CString * psCopy = m_peecContainer->AllocateObject<ee::CString>();
					if ( !psCopy ) { return false; }
					psCopy->SetString( sNewObj );
					_rRet.ncType = EE_NC_OBJECT;
					_rRet.u.poObj = psCopy;
					return true;
				}
				return false;
			}
		}
		return false;
	}
#endif

	// Gets the object as an ASCII string.
	CExpEvalContainer::EE_RESULT CStringRef::Ascii() const {
		return CStringBaseApi::Ascii( m_peecContainer->m_vStrings[GetStrIdx()], m_peecContainer );
	}

	// Gets the binary form of the object as a string (0b****).
	CExpEvalContainer::EE_RESULT CStringRef::Bin() const {
		return CStringBaseApi::Bin( m_peecContainer->m_vStrings[GetStrIdx()], m_peecContainer );
	}

	// Gets the boolean form of the object as a string (0b****).
	CExpEvalContainer::EE_RESULT CStringRef::Bool() const {
		return CStringBaseApi::Bool( m_peecContainer->m_vStrings[GetStrIdx()], m_peecContainer );
	}

	// Returns the character that represents the specified Unicode. 
	CExpEvalContainer::EE_RESULT CStringRef::Chr() const {
		return CStringBaseApi::Chr( m_peecContainer->m_vStrings[GetStrIdx()], m_peecContainer );
	}

	// Gets the string interpreted to its best fit and then converted to float. 
	CExpEvalContainer::EE_RESULT CStringRef::Float() const {
		return CStringBaseApi::Float( m_peecContainer->m_vStrings[GetStrIdx()], m_peecContainer );
	}

	// Gets the hexadecimal form of the object as a string (0x****).
	CExpEvalContainer::EE_RESULT CStringRef::Hex() const {
		return CStringBaseApi::Hex( m_peecContainer->m_vStrings[GetStrIdx()], m_peecContainer );
	}

	// Gets the string interpreted to its best fit and then converted to int64_t. 
	CExpEvalContainer::EE_RESULT CStringRef::Int() const {
		return CStringBaseApi::Int( m_peecContainer->m_vStrings[GetStrIdx()], m_peecContainer );
	}

	// Gets the octadecimal form of the object as a string (0o****).
	CExpEvalContainer::EE_RESULT CStringRef::Oct() const {
		return CStringBaseApi::Oct( m_peecContainer->m_vStrings[GetStrIdx()], m_peecContainer );
	}

	// Returns the ordinal value of the object as a Unicode character (always EE_NC_UNSIGNED).
	CExpEvalContainer::EE_RESULT CStringRef::Ord() const {
		return CStringBaseApi::Ord( m_peecContainer->m_vStrings[GetStrIdx()], m_peecContainer );
	}

}	// namespace ee

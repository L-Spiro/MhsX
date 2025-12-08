#include "EEExpEvalContainer.h"
#include "Api/EEBaseApi.h"
#include "Array/EEDefaultArray.h"
#include "Array/EEDoubleArray.h"
#include "Array/EEFloatArray.h"
#include "Array/EEInt8Array.h"
#include "Array/EEInt16Array.h"
#include "Array/EEInt32Array.h"
#include "Array/EEInt64Array.h"
#include "Array/EEUInt8Array.h"
#include "Array/EEUInt16Array.h"
#include "Array/EEUInt32Array.h"
#include "Array/EEUInt64Array.h"
#include "Float16/EEFloat16.h"
#include "Object/EEString.h"
#include "Object/EEVector.h"
//#include "Object/EEStringRef.h"
#include "Unicode/EEUnicode.h"

#ifdef _DEBUG
#include <cassert>
#endif
#include <ctime>
#include <sstream>

#define EE_INVALID_IDX				static_cast<size_t>(-1)

namespace ee {

	CExpEvalContainer::~CExpEvalContainer() {
		/*for ( auto I = m_vStringObjectsU8.size(); I--; ) {
			delete m_vStringObjectsU8[I];
		}
		m_vStringObjectsU8.clear();*/

		for ( auto I = m_vArrayData.size(); I--; ) {
			delete m_vArrayData[I].m_pabBase;
			m_vArrayData[I].m_pabBase = nullptr;
		}
		m_vArrayData.clear();


		for ( auto I = m_vObjects.size(); I--; ) {
			delete m_vObjects[I];
		}
		m_vObjects.clear();
	}

	// == Function.
	/**
	 * Gets the result.  If the function returns false then the expression is invalid.  If it returns true but the return type is EE_NC_INVALID, the expression is valid but was unable to be resolved for some reason.
	 * 
	 * \param _rRes Holds the returned result after evaluating the already-parsed expression.
	 * \return Returns true if the expression has been parsed.
	 **/
	bool CExpEvalContainer::Resolve( EE_RESULT &_rRes ) {
		_rRes.ncType = EE_NC_INVALID;
		try {
			EE_ERROR_CODES ecError;
			if ( !ResolveNode_ExplicitStack( m_sTrans, _rRes, ecError ) ) {
				return false;
			}
			return true;
			//return ResolveNode( m_sTrans, _rRes );
		}
		catch ( const std::bad_alloc & ) { return false; }
	}

	/**
	 * Sets a numbered ($x) parameter.
	 * 
	 * \param _sIdx The index of the parameter to set.
	 * \param _rValue The value to assign to the parameter.
	 * \return Returns true if there was enough memory available to create the parameter index.
	 **/
	bool CExpEvalContainer::SetNumberedParm( size_t _sIdx, const EE_RESULT &_rValue ) {
		if ( _sIdx >= m_vNumberedParms.size() ) {
			try {
				m_vNumberedParms.resize( _sIdx + 1 );
			}
			catch ( const std::bad_alloc & ) { return false; }
		}
		m_vNumberedParms[_sIdx] = _rValue;
		return true;
	}

	/**
	 * Gets the type to use between 2 given types.
	 * 
	 * \param _ncLeft The left operand.
	 * \param _ncRight The right operand.
	 * \return Using C++ promotion rules, a common cast type for the given values is determined.
	 **/
	EE_NUM_CONSTANTS CExpEvalContainer::GetCastType( EE_NUM_CONSTANTS _ncLeft, EE_NUM_CONSTANTS _ncRight ) {
		return _ncLeft > _ncRight ? _ncLeft : _ncRight;
	}

	/**
	 * Converts a result to a given type.  Only basic types (no objects) can be converted)
	 * 
	 * \param _rRes The result to convert.
	 * \param _ncType The new type of the result.
	 * \return Returns a new result that represents the conversion from the _rRes type to _ncType, or an EE_NC_INVALID return if the types are incompatible completely.
	 **/
	CExpEvalContainer::EE_RESULT CExpEvalContainer::ConvertResult( const EE_RESULT &_rRes, EE_NUM_CONSTANTS _ncType ) {
		if ( (_rRes.ncType) == _ncType ) { return _rRes; }
		EE_RESULT rRes;
#ifdef _DEBUG
		assert( (_ncType) != EE_NC_OBJECT && (_rRes.ncType) != EE_NC_OBJECT );
#endif
		rRes.ncType = _ncType;
		if ( (_rRes.ncType) == ee::EE_NC_FLOATING ) {
			switch ( _ncType ) {
				case ee::EE_NC_SIGNED : {
					rRes.u.i64Val = static_cast<int64_t>(_rRes.u.dVal);
					break;
				}
				case ee::EE_NC_UNSIGNED : {
					rRes.u.ui64Val = static_cast<uint64_t>(_rRes.u.dVal);
					break;
				}
			}
		}
		else if ( (_rRes.ncType) == ee::EE_NC_SIGNED ) {
			switch ( _ncType ) {
				case ee::EE_NC_UNSIGNED : {
					rRes.u.ui64Val = static_cast<uint64_t>(_rRes.u.i64Val);
					break;
				}
				case ee::EE_NC_FLOATING : {
					rRes.u.dVal = static_cast<double>(_rRes.u.i64Val);
					break;
				}
			}
		}
		else if ( (_rRes.ncType) == ee::EE_NC_UNSIGNED ) {
			switch ( _ncType ) {
				case ee::EE_NC_FLOATING : {
					rRes.u.dVal = static_cast<double>(_rRes.u.ui64Val);
					break;
				}
				case ee::EE_NC_SIGNED : {
					rRes.u.i64Val = static_cast<int64_t>(_rRes.u.ui64Val);
					break;
				}
			}
		}
		return rRes;
	}

	/**
	 * Converts a result or object to a given type.  Same as ConvertResult() but it can convert objects as well.
	 * 
	 * \param _rRes The result to convert.
	 * \param _ncType The new type of the result.
	 * \return Returns a new result that represents the conversion from the _rRes type to _ncType, or an EE_NC_INVALID return if the types are incompatible completely.
	 **/
	CExpEvalContainer::EE_RESULT CExpEvalContainer::ConvertResultOrObject( const EE_RESULT &_rRes, EE_NUM_CONSTANTS _ncType ) {
		if ( (_rRes.ncType) == _ncType ) { return _rRes; }
		EE_RESULT rRes;
		rRes.ncType = _ncType;
		if ( (_rRes.ncType) == ee::EE_NC_FLOATING ) {
			switch ( _ncType ) {
				case ee::EE_NC_SIGNED : {
					rRes.u.i64Val = static_cast<int64_t>(_rRes.u.dVal);
					break;
				}
				case ee::EE_NC_UNSIGNED : {
					rRes.u.ui64Val = static_cast<uint64_t>(_rRes.u.dVal);
					break;
				}
				case EE_NC_OBJECT : {
					rRes.ncType = ee::EE_NC_INVALID;
					break;
				}
			}
		}
		else if ( (_rRes.ncType) == ee::EE_NC_SIGNED ) {
			switch ( _ncType ) {
				case ee::EE_NC_UNSIGNED : {
					rRes.u.ui64Val = static_cast<uint64_t>(_rRes.u.i64Val);
					break;
				}
				case ee::EE_NC_FLOATING : {
					rRes.u.dVal = static_cast<double>(_rRes.u.i64Val);
					break;
				}
				case EE_NC_OBJECT : {
					rRes.ncType = ee::EE_NC_INVALID;
					break;
				}
			}
		}
		else if ( (_rRes.ncType) == ee::EE_NC_UNSIGNED ) {
			switch ( _ncType ) {
				case ee::EE_NC_FLOATING : {
					rRes.u.dVal = static_cast<double>(_rRes.u.ui64Val);
					break;
				}
				case ee::EE_NC_SIGNED : {
					rRes.u.i64Val = static_cast<int64_t>(_rRes.u.ui64Val);
					break;
				}
				case EE_NC_OBJECT : {
					rRes.ncType = ee::EE_NC_INVALID;
					break;
				}
			}
		}
		else if ( (_rRes.ncType) == ee::EE_NC_OBJECT ) {
			if ( !_rRes.u.poObj ) { rRes.ncType = ee::EE_NC_INVALID; }
			else { return _rRes.u.poObj->ConvertTo( _ncType ); }
		}
		return rRes;
	}

	/**
	 * Performs ToString() on a given result or object.
	 *
	 * \param _rRes The result to convert to a string.
	 * \param _sReturn The resulting string form of the given result.
	 * \return Returns true of a to-string conversion was made.
	 */
	bool CExpEvalContainer::ToStringResultOrObject( const EE_RESULT &_rRes, std::string &_sReturn, uint32_t _ui32Depth, uint32_t _ui32Flags ) {
		switch ( _rRes.ncType ) {
			case ee::EE_NC_SIGNED : {
				char szFormat[32];
				std::snprintf( szFormat, std::size( szFormat ), "%I64d", _rRes.u.i64Val );
				_sReturn = szFormat;
				break;
			}
			case ee::EE_NC_UNSIGNED : {
				char szFormat[32];
				std::snprintf( szFormat, std::size( szFormat ), "%I64u", _rRes.u.ui64Val );
				_sReturn = szFormat;
				break;
			}
			case ee::EE_NC_FLOATING : {
				char szFormat[32];
				std::snprintf( szFormat, std::size( szFormat ), "%.17g", _rRes.u.dVal );
				_sReturn = szFormat;
				break;
			}
			case ee::EE_NC_OBJECT : {
				if ( !_rRes.u.poObj ) {
					_sReturn = "<null>";
				}
				else {
					return _rRes.u.poObj->ToString( _sReturn, _ui32Depth + 1, _ui32Flags );
				}
				break;
			}
			default : {
				_sReturn = "";
			}
		}
		return true;
	}

	/**
	 * Performs ToString() on a given result or object.
	 *
	 * \param _rRes The result to convert to a string.
	 * \param _sFormat The format string.
	 * \param _sReturn The resulting string form of the given result.
	 * \return Returns true of a to-string conversion was made.
	 */
	bool CExpEvalContainer::ToFormatStringResultOrObject( const EE_RESULT &_rRes, const std::string &_sFormat, std::string &_sReturn, uint32_t _ui32Flags ) {
		switch ( _rRes.ncType ) {
			case EE_NC_UNSIGNED : {
				_sReturn = std::vformat( _sFormat, std::make_format_args( _rRes.u.ui64Val ) );
				break;
			}
			case EE_NC_SIGNED : {
				_sReturn = std::vformat( _sFormat, std::make_format_args( _rRes.u.i64Val ) );
				break;
			}
			case EE_NC_FLOATING : {
				_sReturn = std::vformat( _sFormat, std::make_format_args( _rRes.u.dVal ) );
				break;
			}
			case EE_NC_OBJECT : {
				if ( !_rRes.u.poObj ) {
					_sReturn = "<null>";
				}
				else {
					_sReturn = _rRes.u.poObj->FormattedString( _sFormat, _ui32Flags );
				}
				break;
			}
			default : {
				_sReturn = "";
			}
		}
		return true;
	}

	/**
	 * Prforms an equality comparison on a pair of given results or objects.
	 *
	 * \param _rLeft The left operand.
	 * \param _rRight The right operand.
	 * \return If either result is an object, Obj->Equals() is called on te pair, otherwise a comparison between
	 */
	bool CExpEvalContainer::EqualResultOrObject( const EE_RESULT &_rLeft, const EE_RESULT &_rRight ) {
		EE_RESULT rRes;
		EE_ERROR_CODES ecCode = PerformOp( _rLeft, CExpEvalParser::token::EE_EQU_E, _rRight, rRes );
		if ( ecCode != EE_EC_SUCCESS || rRes.ncType == ee::EE_NC_INVALID ) { return false; }
		return !!rRes.u.ui64Val;
	}

	// Default ToString() function.
	std::wstring CExpEvalContainer::DefaultToString( EE_RESULT &_rResult, uint32_t _ui32Depth, uint64_t /*_ui64Options*/ ) {
		std::wstring wsString;
		switch ( _rResult.ncType ) {
			case ee::EE_NC_SIGNED : {
				wchar_t szFormat[32];
				std::swprintf( szFormat, std::size( szFormat ), L"%I64d", _rResult.u.i64Val );
				wsString = szFormat;
				break;
			}
			case ee::EE_NC_UNSIGNED : {
				wchar_t szFormat[32];
				std::swprintf( szFormat, std::size( szFormat ), L"%I64u", _rResult.u.ui64Val );
				wsString = szFormat;
				break;
			}
			case ee::EE_NC_FLOATING : {
				wchar_t szFormat[32];
				std::swprintf( szFormat, std::size( szFormat ), L"%.17g", _rResult.u.dVal );
				wsString = szFormat;
				break;
			}
			case ee::EE_NC_OBJECT : {
				if ( !_rResult.u.poObj ) {
					wsString = L"<null>";
				}
				else {
					std::string sTmp;
					if ( _rResult.u.poObj->ToString( sTmp, _ui32Depth + 1 ) ) {
						wsString = ee::CExpEval::StringToWString( sTmp );
					}
					else {
						wsString = L"<err>";
					}
				}
				break;
			}
			default : {
				wsString = L"";
			}
		}
		return wsString;
	}

	// Returns the index of a string if it exists or static_cast<size_t>(-1) otherwise.
	size_t CExpEvalContainer::HasString( const std::string &_sText ) const {
		for ( size_t I = 0; I < m_vStrings.size(); ++I ) {
			if ( m_vStrings[I] == _sText ) { return I; }
		}
		return static_cast<size_t>(-1);
	}

	// Returns the index of a custom variable if it exists or static_cast<size_t>(-1) otherwise.
	size_t CExpEvalContainer::HasCustomVar( const std::string &_sText ) const {
		if ( m_mCustomVariables.size() == 0 ) { return false; }	// Fastest check that is most-often the case.
		size_t sIdx = HasString( _sText );
		if ( sIdx == static_cast<size_t>(-1) ) { return false; }
		auto aFound = m_mCustomVariables.find( sIdx );
		if ( aFound == m_mCustomVariables.end() ) { return static_cast<size_t>(-1); }
		return sIdx;
	}

	// Determines whether or not the given text is a custom variable.
	bool CExpEvalContainer::IsCustomVar( const std::string &_sText ) const {
		if ( m_mCustomVariables.size() == 0 ) { return false; }	// Fastest check that is most-often the case.
		size_t sIdx = HasString( _sText );
		if ( sIdx == static_cast<size_t>(-1) ) { return false; }
		return m_mCustomVariables.find( sIdx ) != m_mCustomVariables.end();
	}

	// Returns the index of an array if it exists or static_cast<size_t>(-1) otherwise.
	size_t CExpEvalContainer::HasArray( const std::string &_sText ) const {
		if ( m_mArrays.size() == 0 ) { return false; }	// Fastest check that is most-often the case.
		size_t sIdx = HasString( _sText );
		if ( sIdx == static_cast<size_t>(-1) ) { return false; }
		auto aFound = m_mArrays.find( sIdx );
		if ( aFound == m_mArrays.end() ) { return static_cast<size_t>(-1); }
		return aFound->second;
	}

	// Determines whether or not a given text is an array.
	bool CExpEvalContainer::IsArray( const std::string &_sText ) const {
		if ( m_mArrays.size() == 0 ) { return false; }	// Fastest check that is most-often the case.
		size_t sIdx = HasString( _sText );
		if ( sIdx == static_cast<size_t>(-1) ) { return false; }
		return m_mArrays.find( sIdx ) != m_mArrays.end();
	}

	// Gets the string version of the given value.
	std::wstring CExpEvalContainer::ToString( EE_RESULT &_rResult, uint32_t _ui32Depth, uint64_t _ui64Options ) {
		if ( !m_pfToString ) { return DefaultToString( _rResult, _ui32Depth, _ui64Options ); }
		return m_pfToString( _rResult, _ui64Options );
	}

	// Prints a formatted string.
	int __cdecl CExpEvalContainer::PrintF( wchar_t * _pwcDst, size_t _sMaxLen,
		const wchar_t * _pwcFormat, int _iArgs, ... ) {
		va_list vaArgs;
		va_start( vaArgs, _iArgs );

		int iRet = PrintFVarArgs( _pwcDst, _sMaxLen, _pwcFormat, _iArgs, vaArgs );

		va_end( vaArgs );
		return iRet;
	}

	// Prints a formatted string.  All ... parameters must be of type "const EE_RESULT".
	int __cdecl CExpEvalContainer::PrintFVarArgs( wchar_t * _pwcDst, size_t _sMaxLen,
		const wchar_t * _pwcFormat, int _iArgs, va_list _vaArgPtr ) {
		size_t sDst = 0;
		EE_PRINTF_CONTEXT pcContext;
		EE_RESULT rTemp;
		rTemp.u.ui64Val = 0;
#define EE_PRINT_CHAR( VAL )		if ( sDst >= _sMaxLen ) { break; } _pwcDst[sDst++] = VAL;
		for ( size_t I = 0; _pwcFormat[I]; ++I ) {
			if ( _pwcFormat[I] == L'%' ) {
				if ( _pwcFormat[I+1] == L'%' ) {
					EE_PRINT_CHAR( L'%' );
					++I;
					continue;
				}
				EE_NUM_CONSTANTS ncType;
				if ( !FillContext( _pwcFormat, I, pcContext, ncType ) ) {
					EE_PRINT_CHAR( L'%' );
					++I;
					continue;
				}
				if ( _iArgs ) {
					rTemp = va_arg( _vaArgPtr, const EE_RESULT );
					--_iArgs;
				}
				if ( (ncType) == EE_NC_FLOATING ) {
					int iTemp = ::swprintf( &_pwcDst[sDst], _sMaxLen - sDst, pcContext.wcFormat, rTemp.u.dVal );
					if ( iTemp == -1 ) { return -1; }
					sDst += iTemp;
				}
				else {
					int iTemp = ::swprintf( &_pwcDst[sDst], _sMaxLen - sDst, pcContext.wcFormat, rTemp.u.ui64Val );
					if ( iTemp == -1 ) { return -1; }
					sDst += iTemp;
				}
			}
			else {
				EE_PRINT_CHAR( _pwcFormat[I] );
			}
		}
#undef EE_PRINT_CHAR

		if ( sDst < _sMaxLen ) { _pwcDst[sDst] = L'\0'; }
		return static_cast<int>(sDst);
	}

	// Prints a formatted string.  All ... parameters must be of type "const EE_RESULT".
	int __cdecl CExpEvalContainer::PrintF( wchar_t * _pwcDst, size_t _sMaxLen,
		const wchar_t * _pwcFormat, const std::vector<EE_RESULT> &_vParms ) {
		size_t sDst = 0, sIdx = 0;
		EE_PRINTF_CONTEXT pcContext;
		EE_RESULT rTemp;
		rTemp.u.ui64Val = 0;
#define EE_PRINT_CHAR( VAL )		if ( sDst >= _sMaxLen ) { break; } _pwcDst[sDst++] = VAL;
		for ( size_t I = 0; _pwcFormat[I]; ++I ) {
			if ( _pwcFormat[I] == L'%' ) {
				if ( _pwcFormat[I+1] == L'%' ) {
					EE_PRINT_CHAR( L'%' );
					++I;
					continue;
				}
				EE_NUM_CONSTANTS ncType;
				if ( !FillContext( _pwcFormat, I, pcContext, ncType ) ) {
					EE_PRINT_CHAR( L'%' );
					++I;
					continue;
				}
				if ( sIdx < _vParms.size() ) {
					rTemp = _vParms[sIdx++];
				}
				if ( (ncType) == EE_NC_FLOATING ) {
					int iTemp = ::swprintf( &_pwcDst[sDst], _sMaxLen - sDst, pcContext.wcFormat, rTemp.u.dVal );
					if ( iTemp == -1 ) { return -1; }
					sDst += iTemp;
				}
				else {
					int iTemp = ::swprintf( &_pwcDst[sDst], _sMaxLen - sDst, pcContext.wcFormat, rTemp.u.ui64Val );
					if ( iTemp == -1 ) { return -1; }
					sDst += iTemp;
				}
			}
			else {
				EE_PRINT_CHAR( _pwcFormat[I] );
			}
		}
#undef EE_PRINT_CHAR

		if ( sDst < _sMaxLen ) { _pwcDst[sDst] = L'\0'; }
		return static_cast<int>(sDst);
	}

	CExpEvalContainer::EE_ERROR_CODES CExpEvalContainer::PerformOp( EE_RESULT _rLeft, uint32_t _uiOp, EE_RESULT _rRight, EE_RESULT &_rResult ) {
		_rResult.ncType = GetCastType( _rLeft.ncType, _rRight.ncType );
		if ( _rResult.ncType != EE_NC_OBJECT ) {
			_rLeft = ConvertResultOrObject( _rLeft, _rResult.ncType );
			if ( _rLeft.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rRight = ConvertResultOrObject( _rRight, _rResult.ncType );
			if ( _rRight.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		}
		//else if ( _rLeft.ncType != EE_NC_OBJECT ) {
		//	// If casting to objects, the left side must be an object.
		//	_rResult.ncType = EE_NC_INVALID;
		//	return EE_EC_INVALIDCAST;
		//}
#define EE_OP( MEMBER, CASE, OP )													\
	case CASE : {																	\
		_rResult.u.MEMBER = _rLeft.u.MEMBER OP _rRight.u.MEMBER;					\
		break;																		\
	}
#define EE_OP_NO_ZERO( MEMBER, CASE, OP )											\
	case CASE : {																	\
		if ( _rRight.u.MEMBER ) {													\
			_rResult.u.MEMBER = _rLeft.u.MEMBER OP _rRight.u.MEMBER;				\
		}																			\
		else { _rResult.u.MEMBER = 0; }												\
		break;																		\
	}
#define EE_OP_BOOL( MEMBER, CASE, OP )												\
	case CASE : {																	\
		_rResult.ncType = EE_NC_UNSIGNED;											\
		_rResult.u.ui64Val = _rLeft.u.MEMBER OP _rRight.u.MEMBER;					\
		break;																		\
	}
#define EE_INT_CHECK( CASE, MEMBER )												\
	case CASE : {																	\
		switch ( _uiOp ) {															\
			EE_OP( MEMBER, '*', * )													\
			EE_OP_NO_ZERO( MEMBER, '/', / )											\
			EE_OP_NO_ZERO( MEMBER, '%', % )											\
			EE_OP( MEMBER, '+', + )													\
			EE_OP( MEMBER, '-', - )													\
			EE_OP( MEMBER, CExpEvalParser::token::EE_RIGHT_OP, >> )					\
			EE_OP( MEMBER, CExpEvalParser::token::EE_LEFT_OP, << )					\
			EE_OP( MEMBER, '&', & )													\
			EE_OP( MEMBER, '^', ^ )													\
			EE_OP( MEMBER, '|', | )													\
			EE_OP_BOOL( MEMBER, '<', < )											\
			EE_OP_BOOL( MEMBER, '>', > )											\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_REL_LE, <= )				\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_REL_GE, >= )				\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_EQU_E, == )				\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_EQU_NE, != )				\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_AND, && )					\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_OR, || )					\
			default : { return EE_EC_ERRORPROCESSINGOP; }							\
		}																			\
		break;																		\
	}

		switch ( _rResult.ncType ) {
			EE_INT_CHECK( EE_NC_SIGNED, i64Val )
			EE_INT_CHECK( EE_NC_UNSIGNED, ui64Val )
			case EE_NC_FLOATING : {
				switch ( _uiOp ) {
					EE_OP( dVal, '*', * )
					EE_OP( dVal, '/', / )
					EE_OP( dVal, '+', + )
					EE_OP( dVal, '-', - )
					EE_OP_BOOL( dVal, '<', < )
					EE_OP_BOOL( dVal, '>', > )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_REL_LE, <= )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_REL_GE, >= )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_EQU_E, == )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_EQU_NE, != )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_AND, && )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_OR, || )
					case CExpEvalParser::token::EE_RIGHT_OP : {
						_rResult.u.dVal = RShift( _rLeft.u.dVal, _rRight.u.dVal );
						break;
					}
					case CExpEvalParser::token::EE_LEFT_OP : {
						_rResult.u.dVal = LShift( _rLeft.u.dVal, _rRight.u.dVal );
						break;
					}
					case '%' : {
						_rResult.u.dVal = std::fmod( _rLeft.u.dVal, _rRight.u.dVal );
						break;
					}
					case '^' : {
						_rResult.u.dVal = std::pow( _rLeft.u.dVal, _rRight.u.dVal );
						break;
					}
					default : { return EE_EC_INVALIDTREE; }
				}
				break;
			}
			case EE_NC_OBJECT : {
				if ( _rLeft.ncType == EE_NC_OBJECT ) {
					// Left is an object, right may or may not be.
					switch ( _uiOp ) {
						case '+' : {
							_rResult = _rLeft.u.poObj->Plus( _rRight );
							break;
						}
						case '-' : {
							_rResult = _rLeft.u.poObj->Minus( _rRight );
							break;
						}
						case '*' : {
							_rResult = _rLeft.u.poObj->Multiply( _rRight );
							break;
						}
						case '/' : {
							_rResult = _rLeft.u.poObj->Divide( _rRight );
							break;
						}
						case CExpEvalParser::token::EE_EQU_E : {
							_rResult.ncType = EE_NC_UNSIGNED;
							_rResult.u.ui64Val = _rLeft.u.poObj->Equals( _rRight );
							break;
						}
						case CExpEvalParser::token::EE_EQU_NE : {
							_rResult.ncType = EE_NC_UNSIGNED;
							_rResult.u.ui64Val = !_rLeft.u.poObj->Equals( _rRight );
							break;
						}
						default : { return EE_EC_INVALIDTREE; }
					}
				}
				else if ( _rRight.ncType == EE_NC_OBJECT ) {
					// Left is NOT an object, right is.
					// If it is a vector, apply operator to each element.
					if ( _rRight.u.poObj && (_rRight.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
						ee::CVector * psObj = reinterpret_cast<ee::CVector *>(AllocateObject<ee::CVector>());
						if ( !psObj ) {
							_rResult.ncType = EE_NC_INVALID;
							return EE_EC_OUTOFMEMORY;
						}

						ee::CVector * pThis = static_cast<ee::CVector *>(_rRight.u.poObj);
						try {
							psObj->Resize( pThis->Size().u.ui64Val );
						}
						catch ( ... ) {
							DeallocateObject( psObj );
							_rResult.ncType = EE_NC_INVALID;
							return EE_EC_OUTOFMEMORY;
						}

						
						for ( auto I = pThis->GetBacking().size(); I--; ) {
							auto aRes = PerformOp( _rLeft, _uiOp, pThis->GetBacking()[I], psObj->GetBacking()[I] );
							if ( aRes != EE_EC_SUCCESS ) { return aRes; }
						}
						_rResult = psObj->CreateResult();
						return EE_EC_SUCCESS;
					}
					else {
						// Not a vector or is malformed.
						_rResult.ncType = EE_NC_INVALID;
						return EE_EC_INVALIDCAST;
					}
				}
				//_rLeft.u.poObj->Ord
				break;
			}
		}

#undef EE_INT_CHECK
#undef EE_OP_BOOL
#undef EE_OP_NO_ZERO
#undef EE_OP
		return EE_EC_SUCCESS;
	}

	// Performs an operation on 2 given results using the given operator.
	CExpEvalContainer::EE_ERROR_CODES CExpEvalContainer::PerformOp_S( EE_RESULT _rLeft, uint32_t _uiOp, EE_RESULT _rRight, EE_RESULT &_rResult ) {
		_rResult.ncType = GetCastType( _rLeft.ncType, _rRight.ncType );
		if ( _rResult.ncType != EE_NC_OBJECT ) {
			_rLeft = ConvertResult( _rLeft, _rResult.ncType );
			if ( _rLeft.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rRight = ConvertResult( _rRight, _rResult.ncType );
			if ( _rRight.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		}
		else if ( _rLeft.ncType != EE_NC_OBJECT ) {
			// If casting to objects, the left side must be an object.
			_rResult.ncType = EE_NC_INVALID;
			return EE_EC_INVALIDCAST;
		}
#define EE_OP( MEMBER, CASE, OP )													\
	case CASE : {																	\
		_rResult.u.MEMBER = _rLeft.u.MEMBER OP _rRight.u.MEMBER;					\
		break;																		\
	}
#define EE_OP_NO_ZERO( MEMBER, CASE, OP )											\
	case CASE : {																	\
		if ( _rRight.u.MEMBER ) {													\
			_rResult.u.MEMBER = _rLeft.u.MEMBER OP _rRight.u.MEMBER;				\
		}																			\
		else { _rResult.u.MEMBER = 0; }												\
		break;																		\
	}
#define EE_OP_BOOL( MEMBER, CASE, OP )												\
	case CASE : {																	\
		_rResult.ncType = EE_NC_UNSIGNED;											\
		_rResult.u.ui64Val = _rLeft.u.MEMBER OP _rRight.u.MEMBER;					\
		break;																		\
	}
#define EE_INT_CHECK( CASE, MEMBER )												\
	case CASE : {																	\
		switch ( _uiOp ) {															\
			EE_OP( MEMBER, '*', * )													\
			EE_OP_NO_ZERO( MEMBER, '/', / )											\
			EE_OP_NO_ZERO( MEMBER, '%', % )											\
			EE_OP( MEMBER, '+', + )													\
			EE_OP( MEMBER, '-', - )													\
			EE_OP( MEMBER, CExpEvalParser::token::EE_RIGHT_OP, >> )					\
			EE_OP( MEMBER, CExpEvalParser::token::EE_LEFT_OP, << )					\
			EE_OP( MEMBER, '&', & )													\
			EE_OP( MEMBER, '^', ^ )													\
			EE_OP( MEMBER, '|', | )													\
			EE_OP_BOOL( MEMBER, '<', < )											\
			EE_OP_BOOL( MEMBER, '>', > )											\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_REL_LE, <= )				\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_REL_GE, >= )				\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_EQU_E, == )				\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_EQU_NE, != )				\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_AND, && )					\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_OR, || )					\
			default : { return EE_EC_ERRORPROCESSINGOP; }							\
		}																			\
		break;																		\
	}

		switch ( _rResult.ncType ) {
			EE_INT_CHECK( EE_NC_SIGNED, i64Val )
			EE_INT_CHECK( EE_NC_UNSIGNED, ui64Val )
			case EE_NC_FLOATING : {
				switch ( _uiOp ) {
					EE_OP( dVal, '*', * )
					EE_OP( dVal, '/', / )
					EE_OP( dVal, '+', + )
					EE_OP( dVal, '-', - )
					EE_OP_BOOL( dVal, '<', < )
					EE_OP_BOOL( dVal, '>', > )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_REL_LE, <= )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_REL_GE, >= )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_EQU_E, == )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_EQU_NE, != )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_AND, && )
					EE_OP_BOOL( dVal, CExpEvalParser::token::EE_OR, || )
					case CExpEvalParser::token::EE_RIGHT_OP : {
						_rResult.u.dVal = RShift( _rLeft.u.dVal, _rRight.u.dVal );
						break;
					}
					case CExpEvalParser::token::EE_LEFT_OP : {
						_rResult.u.dVal = LShift( _rLeft.u.dVal, _rRight.u.dVal );
						break;
					}
					case '%' : {
						_rResult.u.dVal = std::fmod( _rLeft.u.dVal, _rRight.u.dVal );
						break;
					}
					case '^' : {
						_rResult.u.dVal = std::pow( _rLeft.u.dVal, _rRight.u.dVal );
						break;
					}
					default : { return EE_EC_INVALIDTREE; }
				}
				break;
			}
			case EE_NC_OBJECT : {
				switch ( _uiOp ) {
					case '+' : {
						_rResult = _rLeft.u.poObj->Plus( _rRight );
						break;
					}
					case '-' : {
						_rResult = _rLeft.u.poObj->Minus( _rRight );
						break;
					}
					case '*' : {
						_rResult = _rLeft.u.poObj->Multiply( _rRight );
						break;
					}
					case '/' : {
						_rResult = _rLeft.u.poObj->Divide( _rRight );
						break;
					}
					case CExpEvalParser::token::EE_EQU_E : {
						_rResult.ncType = EE_NC_UNSIGNED;
						_rResult.u.ui64Val = _rLeft.u.poObj->Equals( _rRight );
						break;
					}
					case CExpEvalParser::token::EE_EQU_NE : {
						_rResult.ncType = EE_NC_UNSIGNED;
						_rResult.u.ui64Val = !_rLeft.u.poObj->Equals( _rRight );
						break;
					}
					default : { return EE_EC_INVALIDTREE; }
				}
				break;
			}
		}

#undef EE_INT_CHECK
#undef EE_OP_BOOL
#undef EE_OP_NO_ZERO
#undef EE_OP
		return EE_EC_SUCCESS;
	}

	// Applies a 1-parameter intrinsic to a result.
	CExpEvalContainer::EE_ERROR_CODES CExpEvalContainer::PerformIntrinsic( EE_RESULT _rExp, uint32_t _uiIntrinsic, EE_RESULT &_rResult, uint32_t _ui32Depth ) {
		if ( !_ui32Depth && _rExp.ncType == EE_NC_OBJECT && _rExp.u.poObj && (_rExp.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
			// Apply everything to each element in the vector.
			ee::CVector * pThis = static_cast<ee::CVector *>(_rExp.u.poObj);
			for ( auto I = pThis->GetBacking().size(); I--; ) {
				auto aRes = PerformIntrinsic( pThis->GetBacking()[I], _uiIntrinsic, pThis->GetBacking()[I], _ui32Depth + 1 );
				if ( aRes != EE_EC_SUCCESS ) { return aRes; }
			}
			_rResult = pThis->CreateResult();
			return EE_EC_SUCCESS;
		}
		if ( _uiIntrinsic == CExpEvalParser::token::EE_BYTESWAPUSHORT ) {
			_rExp = ConvertResultOrObject( _rExp, EE_NC_UNSIGNED );
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rResult.u.ui64Val = ::_byteswap_ushort( static_cast<uint16_t>(_rExp.u.ui64Val) );
			_rResult.ncType = EE_NC_UNSIGNED;
			return EE_EC_SUCCESS;
		}
		if ( _uiIntrinsic == CExpEvalParser::token::EE_BYTESWAPULONG ) {
			_rExp = ConvertResultOrObject( _rExp, EE_NC_UNSIGNED );
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rResult.u.ui64Val = ::_byteswap_ulong( static_cast<uint32_t>(_rExp.u.ui64Val) );
			_rResult.ncType = EE_NC_UNSIGNED;
			return EE_EC_SUCCESS;
		}
		if ( _uiIntrinsic == CExpEvalParser::token::EE_BYTESWAPUINT64 ) {
			_rExp = ConvertResultOrObject( _rExp, EE_NC_UNSIGNED );
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rResult.u.ui64Val = ::_byteswap_uint64( _rExp.u.ui64Val );
			_rResult.ncType = EE_NC_UNSIGNED;
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_BITSWAP8 ) {
			_rExp = ConvertResultOrObject( _rExp, EE_NC_UNSIGNED );
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rResult.u.ui64Val = CExpEval::ReverseBits8( static_cast<uint8_t>(_rExp.u.ui64Val) );
			_rResult.ncType = EE_NC_UNSIGNED;
			return EE_EC_SUCCESS;
		}
		if ( _uiIntrinsic == CExpEvalParser::token::EE_BITSWAP16 ) {
			_rExp = ConvertResultOrObject( _rExp, EE_NC_UNSIGNED );
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rResult.u.ui64Val = CExpEval::ReverseBits16( static_cast<uint16_t>(_rExp.u.ui64Val) );
			_rResult.ncType = EE_NC_UNSIGNED;
			return EE_EC_SUCCESS;
		}
		if ( _uiIntrinsic == CExpEvalParser::token::EE_BITSWAP32 ) {
			_rExp = ConvertResultOrObject( _rExp, EE_NC_UNSIGNED );
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rResult.u.ui64Val = CExpEval::ReverseBits32( static_cast<uint32_t>(_rExp.u.ui64Val) );
			_rResult.ncType = EE_NC_UNSIGNED;
			return EE_EC_SUCCESS;
		}
		if ( _uiIntrinsic == CExpEvalParser::token::EE_BITSWAP64 ) {
			_rExp = ConvertResultOrObject( _rExp, EE_NC_UNSIGNED );
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rResult.u.ui64Val = CExpEval::ReverseBits64( _rResult.u.ui64Val );
			_rResult.ncType = EE_NC_UNSIGNED;
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_BITSWAPF16 ) {
			_rExp = ConvertResultOrObject( _rExp, EE_NC_FLOATING );
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rResult.u.dVal = CFloat16( CExpEval::ReverseBits16( CFloat16::DoubleToUi16( _rExp.u.dVal ) ) ).Value();
			_rResult.ncType = EE_NC_FLOATING;
			return EE_EC_SUCCESS;
		}
		if ( _uiIntrinsic == CExpEvalParser::token::EE_BITSWAPF32 ) {
			_rExp = ConvertResultOrObject( _rExp, EE_NC_FLOATING );
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			float fVal = static_cast<float>(_rExp.u.dVal);
			uint32_t * pui32Val = reinterpret_cast<uint32_t *>(&fVal);
			uint32_t ui32Tmp = CExpEval::ReverseBits32( (*pui32Val) );
			_rResult.u.dVal = (*reinterpret_cast<float *>(&ui32Tmp));
			_rResult.ncType = EE_NC_FLOATING;
			return EE_EC_SUCCESS;
		}
		if ( _uiIntrinsic == CExpEvalParser::token::EE_BITSWAPF64 ) {
			_rExp = ConvertResultOrObject( _rExp, EE_NC_FLOATING );
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rResult.u.ui64Val = CExpEval::ReverseBits64( _rExp.u.ui64Val );
			_rResult.ncType = EE_NC_FLOATING;
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_ASCII ) {
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			if ( _rExp.ncType == EE_NC_OBJECT ) {
				if ( !_rExp.u.poObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

				_rResult = _rExp.u.poObj->Ascii();
				return _rResult.ncType == EE_NC_INVALID ? EE_EC_OUTOFMEMORY : EE_EC_SUCCESS;
			}
			CString * psObj = AllocateObject<CString>();
			if ( !psObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			
			_rResult.u.poObj = psObj;
			_rResult.ncType = EE_NC_OBJECT;
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_BIN ) {
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			if ( _rExp.ncType == EE_NC_OBJECT ) {
				if ( !_rExp.u.poObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

				_rResult = _rExp.u.poObj->Bin();
				return _rResult.ncType == EE_NC_INVALID ? EE_EC_INVALIDAPIOPERATION : EE_EC_SUCCESS;
			}
			CString * psObj = AllocateObject<CString>();
			if ( !psObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			switch ( _rExp.ncType ) {
				case EE_NC_UNSIGNED : {
					psObj->SetString( ee::CExpEval::ToBinary( _rExp.u.ui64Val ) );
					_rResult = psObj->CreateResult();
					break;
				}
				case EE_NC_SIGNED : {
					psObj->SetString( ee::CExpEval::ToBinary( _rExp.u.i64Val ) );
					_rResult = psObj->CreateResult();
					break;
				}
				case EE_NC_FLOATING : {
					psObj->SetString( ee::CExpEval::ToBinary( _rExp.u.dVal ) );
					_rResult = psObj->CreateResult();
					break;
				}
				default : { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			}
			
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_BOOL ) {
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			if ( _rExp.ncType == EE_NC_OBJECT ) {
				if ( !_rExp.u.poObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

				_rResult = _rExp.u.poObj->Bool();
				return _rResult.ncType == EE_NC_INVALID ? EE_EC_INVALIDAPIOPERATION : EE_EC_SUCCESS;
			}
			CString * psObj = AllocateObject<CString>();
			if ( !psObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			switch ( _rExp.ncType ) {
				case EE_NC_UNSIGNED : {
					psObj->SetString( _rExp.u.ui64Val ? "True" : "False" );
					_rResult = psObj->CreateResult();
					break;
				}
				case EE_NC_SIGNED : {
					psObj->SetString( _rExp.u.i64Val ? "True" : "False" );
					_rResult = psObj->CreateResult();
					break;
				}
				case EE_NC_FLOATING : {
					psObj->SetString( static_cast<bool>(_rExp.u.ui64Val) ? "True" : "False" );
					_rResult = psObj->CreateResult();
					break;
				}
				default : { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			}
			
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_CHR ) {
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			if ( _rExp.ncType == EE_NC_OBJECT ) {
				if ( !_rExp.u.poObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

				_rResult = _rExp.u.poObj->Chr();
				return _rResult.ncType == EE_NC_INVALID ? EE_EC_INVALIDAPIOPERATION : EE_EC_SUCCESS;
			}
			CString * psObj = AllocateObject<CString>();
			if ( !psObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

			EE_RESULT rTmp = ConvertResultOrObject( _rExp, EE_NC_UNSIGNED );
			uint32_t ui32Len;
			uint32_t ui32Val = ee::CExpEval::Utf32ToUtf8( static_cast<uint32_t>(rTmp.u.ui64Val), ui32Len );
			std::string sTmp;
			for ( uint32_t I = 0; I < ui32Len; ++I ) {
				sTmp.push_back( static_cast<std::string::value_type>(ui32Val >> (I * sizeof( std::string::value_type ) * 8)) );
			}
			psObj->SetString( sTmp );
			_rResult = psObj->CreateResult();
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_FLOAT ) {
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			if ( _rExp.ncType == EE_NC_OBJECT ) {
				if ( !_rExp.u.poObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

				_rResult = _rExp.u.poObj->Float();
				return _rResult.ncType == EE_NC_INVALID ? EE_EC_INVALIDAPIOPERATION : EE_EC_SUCCESS;
			}
			_rResult = ConvertResultOrObject( _rExp, EE_NC_FLOATING );
			
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_HEX ) {
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			if ( _rExp.ncType == EE_NC_OBJECT ) {
				if ( !_rExp.u.poObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

				_rResult = _rExp.u.poObj->Hex();
				return EE_EC_SUCCESS;
			}
			CString * psObj = AllocateObject<CString>();
			if ( !psObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			switch ( _rExp.ncType ) {
				case EE_NC_UNSIGNED : {
					psObj->SetString( ee::CExpEval::ToHex( _rExp.u.ui64Val ) );
					_rResult = psObj->CreateResult();
					break;
				}
				case EE_NC_SIGNED : {
					psObj->SetString( ee::CExpEval::ToHex( _rExp.u.i64Val ) );
					_rResult = psObj->CreateResult();
					break;
				}
				case EE_NC_FLOATING : {
					psObj->SetString( ee::CExpEval::ToHex( _rExp.u.dVal ) );
					_rResult = psObj->CreateResult();
					break;
				}
				default : { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			}
			
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_INT ) {
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			if ( _rExp.ncType == EE_NC_OBJECT ) {
				if ( !_rExp.u.poObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

				_rResult = _rExp.u.poObj->Int();
				return _rResult.ncType == EE_NC_INVALID ? EE_EC_INVALIDAPIOPERATION : EE_EC_SUCCESS;
			}
			_rResult = ConvertResultOrObject( _rExp, EE_NC_SIGNED );
			
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_LEN ) {
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			if ( _rExp.ncType == EE_NC_OBJECT ) {
				if ( !_rExp.u.poObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

				_rResult = _rExp.u.poObj->Len();
				return _rResult.ncType == EE_NC_INVALID ? EE_EC_INVALIDAPIOPERATION : EE_EC_SUCCESS;
			}
			_rResult = ConvertResultOrObject( _rExp, EE_NC_SIGNED );
			_rResult.ncType = EE_NC_UNSIGNED;
			_rResult.u.ui64Val = 0;
			
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_OCT ) {
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			if ( _rExp.ncType == EE_NC_OBJECT ) {
				if ( !_rExp.u.poObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

				_rResult = _rExp.u.poObj->Oct();
				return EE_EC_SUCCESS;
			}
			CString * psObj = AllocateObject<CString>();
			if ( !psObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			switch ( _rExp.ncType ) {
				case EE_NC_UNSIGNED : {
					psObj->SetString( ee::CExpEval::ToOct( _rExp.u.ui64Val ) );
					_rResult = psObj->CreateResult();
					break;
				}
				case EE_NC_SIGNED : {
					psObj->SetString( ee::CExpEval::ToOct( _rExp.u.i64Val ) );
					_rResult = psObj->CreateResult();
					break;
				}
				case EE_NC_FLOATING : {
					psObj->SetString( ee::CExpEval::ToOct( _rExp.u.dVal ) );
					_rResult = psObj->CreateResult();
					break;
				}
				default : { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			}
			
			return EE_EC_SUCCESS;
		}

		if ( _uiIntrinsic == CExpEvalParser::token::EE_ORD ) {
			if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			if ( _rExp.ncType == EE_NC_OBJECT ) {
				if ( !_rExp.u.poObj ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

				_rResult = _rExp.u.poObj->Ord();
				return EE_EC_SUCCESS;
			}
			_rResult = ConvertResultOrObject( _rExp, EE_NC_SIGNED );
			
			return EE_EC_SUCCESS;
		}


		_rExp = ConvertResultOrObject( _rExp, EE_NC_FLOATING );
		if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		_rResult.ncType = EE_NC_FLOATING;
#define EE_OP( CASE, FUNC )												\
	case CExpEvalParser::token::CASE : {								\
		_rResult.u.dVal = FUNC( _rExp.u.dVal );							\
		return EE_EC_SUCCESS;											\
	}
		switch ( _uiIntrinsic ) {
			case CExpEvalParser::token::EE_DEG : {
				_rResult.ncType = EE_NC_FLOATING;
				_rExp = ConvertResultOrObject( _rExp, EE_NC_FLOATING );
				if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
				_rResult.u.dVal = (_rExp.u.dVal * 180.0) / EE_PI;
				return EE_EC_SUCCESS;
			}
			case CExpEvalParser::token::EE_RAD : {
				_rResult.ncType = EE_NC_FLOATING;
				_rExp = ConvertResultOrObject( _rExp, EE_NC_FLOATING );
				if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
				_rResult.u.dVal = (_rExp.u.dVal * EE_PI) / 180.0;
				return EE_EC_SUCCESS;
			}
			EE_OP( EE_COS, std::cos )
			EE_OP( EE_SIN, std::sin )
			EE_OP( EE_TAN, std::tan )
			EE_OP( EE_ACOS, std::acos )
			EE_OP( EE_ASIN, std::asin )
			EE_OP( EE_ATAN, std::atan )
			EE_OP( EE_COSH, std::cosh )
			EE_OP( EE_SINH, std::sinh )
			EE_OP( EE_TANH, std::tanh )
			EE_OP( EE_ACOSH, std::acosh )
			EE_OP( EE_ASINH, std::asinh )
			EE_OP( EE_ATANH, std::atanh )
			EE_OP( EE_EXP, std::exp )
			EE_OP( EE_LOG, std::log )
			EE_OP( EE_LOG10, std::log10 )
			EE_OP( EE_LOG2, std::log2 )
			EE_OP( EE_EXP2, std::exp2 )
			EE_OP( EE_EXPM1, std::expm1 )
			case CExpEvalParser::token::EE_ISINF : {
				_rResult.ncType = EE_NC_UNSIGNED;
				_rResult.u.ui64Val = std::isinf( _rExp.u.dVal );
				return EE_EC_SUCCESS;
			}
			case CExpEvalParser::token::EE_ISNAN : {
				_rResult.ncType = EE_NC_UNSIGNED;
				_rResult.u.ui64Val = std::isnan( _rExp.u.dVal );
				return EE_EC_SUCCESS;
			}
			case CExpEvalParser::token::EE_ILOGB : {
				_rResult.ncType = EE_NC_SIGNED;
				_rResult.u.i64Val = std::ilogb( _rExp.u.dVal );
				return EE_EC_SUCCESS;
			}
			EE_OP( EE_LOG1P, std::log1p )
			EE_OP( EE_LOGB, std::logb )
			EE_OP( EE_SQRT, std::sqrt )
			EE_OP( EE_CBRT, std::cbrt )
			EE_OP( EE_TGAMMA, std::tgamma )
			EE_OP( EE_LGAMMA, std::lgamma )
			EE_OP( EE_CEIL, std::ceil )
			EE_OP( EE_FLOOR, std::floor )
			EE_OP( EE_TRUNC, std::trunc )
			EE_OP( EE_ROUND, std::round )
			EE_OP( EE_NEARBYINT, std::nearbyint )
			EE_OP( EE_ABS, std::abs )

			EE_OP( EE_SINC, CExpEval::Sinc )
			
#undef EE_OP
		} 

		
		return EE_EC_UNRECOGNIZEDINTRINSIC1;
	}

	// Applies a 2-parameter intrinsic to a result.
	CExpEvalContainer::EE_ERROR_CODES CExpEvalContainer::PerformIntrinsic( EE_RESULT _rExp0, EE_RESULT _rExp1, uint32_t _uiIntrinsic, EE_RESULT &_rResult, bool _bIncludeNonConst ) {				
#define EE_OP( CASE, FUNC )																						\
	case CExpEvalParser::token::CASE : {																		\
		_rExp0 = ConvertResultOrObject( _rExp0, EE_NC_FLOATING );												\
		if ( _rExp0.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }	\
		_rExp1 = ConvertResultOrObject( _rExp1, EE_NC_FLOATING );												\
		if ( _rExp1.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }	\
		_rResult.ncType = EE_NC_FLOATING;																		\
		_rResult.u.dVal = ::FUNC( _rExp0.u.dVal, _rExp1.u.dVal );												\
		return EE_EC_SUCCESS;																					\
	}
#pragma warning( push )
#pragma warning( disable : 4244 )	// warning C4244: 'argument': conversion from 'double' to 'float', possible loss of data
		switch ( _uiIntrinsic ) {
			EE_OP( EE_ATAN2, atan2 )
			EE_OP( EE_POW, pow )
			EE_OP( EE_HYPOT, hypot )
			EE_OP( EE_MOD, fmod )
			EE_OP( EE_REMAINDER, remainder )
			EE_OP( EE_NEXTAFTER, nextafter )
			EE_OP( EE_NEXTAFTERF, nextafterf )
			EE_OP( EE_NEXTTOWARD, nexttoward )
			EE_OP( EE_NEXTTOWARDF, nexttowardf )
			EE_OP( EE_DIM, fdim )
			EE_OP( EE_MAX, ee::CExpEval::Max )
			EE_OP( EE_MIN, ee::CExpEval::Min )
			case CExpEvalParser::token::EE_RAND : {
				_rResult.ncType = GetCastType( _rExp0.ncType, _rExp1.ncType );
				_rExp0 = ConvertResultOrObject( _rExp0, _rResult.ncType );
				if ( _rExp0.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
				_rExp1 = ConvertResultOrObject( _rExp1, _rResult.ncType );
				if ( _rExp1.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
#define EE_QM( MEMBER ) std::min( _rExp0.u.MEMBER, _rExp1.u.MEMBER ), std::max( _rExp0.u.MEMBER, _rExp1.u.MEMBER )
				switch ( _rResult.ncType ) {
					case EE_NC_UNSIGNED : {
						if ( !_bIncludeNonConst ) { return EE_EC_NONCONSTNOTALLOWED; }
						std::uniform_int_distribution<uint64_t> uidDist( EE_QM( ui64Val ) );
						_rResult.u.ui64Val = uidDist( m_mRandGen );
						return EE_EC_SUCCESS;
					}
					case EE_NC_SIGNED : {
						if ( !_bIncludeNonConst ) { return EE_EC_NONCONSTNOTALLOWED; }
						std::uniform_int_distribution<int64_t> uidDist( EE_QM( i64Val ) );
						_rResult.u.i64Val = uidDist( m_mRandGen );
						return EE_EC_SUCCESS;
					}
					case EE_NC_FLOATING : {
						if ( !_bIncludeNonConst ) { return EE_EC_NONCONSTNOTALLOWED; }
						std::uniform_real_distribution<double> uidDist( EE_QM( dVal ) );
						_rResult.u.dVal = uidDist( m_mRandGen );
						return EE_EC_SUCCESS;
					}
#undef EE_QM
					default : {
						_rResult.ncType = EE_NC_INVALID;
						return EE_EC_INVALIDTREE;
					}
				}
				//return EE_EC_PROCESSINGERROR;
			}
			case CExpEvalParser::token::EE_LOG : {
				_rExp0 = ConvertResultOrObject( _rExp0, EE_NC_FLOATING );
				if ( _rExp0.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
				_rExp1 = ConvertResultOrObject( _rExp1, EE_NC_FLOATING );
				if ( _rExp1.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
				_rResult.ncType = EE_NC_FLOATING;
				_rResult.u.dVal = std::log( _rExp0.u.dVal ) / std::log( _rExp1.u.dVal );
				return EE_EC_SUCCESS;
			}
			default : {
				_rResult.ncType = EE_NC_INVALID;
				return EE_EC_UNRECOGNIZEDINTRINSIC2;
			}
		}
#pragma warning( pop )
#undef EE_OP
		//return EE_EC_UNRECOGNIZEDINTRINSIC2;
	}

	// Creates a float using the specific parameters.
	CExpEvalContainer::EE_ERROR_CODES CExpEvalContainer::PerformFloatX( EE_RESULT _rTempSignBits, EE_RESULT _rTempExpBits,
		EE_RESULT _rTempManBits, EE_RESULT _rTempImplied, EE_RESULT _rTempSignVal, EE_RESULT _rTempExpVal, EE_RESULT _rTempManVal,
		EE_RESULT &_rResult ) {

		_rTempSignBits = ConvertResultOrObject( _rTempSignBits, EE_NC_UNSIGNED );
		if ( _rTempSignBits.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		if ( _rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return EE_EC_TOOMANYSIGNBITS; }
		_rTempExpBits = ConvertResultOrObject( _rTempExpBits, EE_NC_UNSIGNED );
		if ( _rTempExpBits.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		if ( _rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return EE_EC_TOOMANYEXPBITS; }
		_rTempManBits = ConvertResultOrObject( _rTempManBits, EE_NC_UNSIGNED );
		if ( _rTempManBits.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		if ( _rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return EE_EC_TOOMANYMANTISSABITS; }
		_rTempImplied = ConvertResultOrObject( _rTempImplied, EE_NC_UNSIGNED );
		if ( _rTempImplied.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		_rTempSignVal = ConvertResultOrObject( _rTempSignVal, EE_NC_UNSIGNED );
		if ( _rTempSignVal.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		_rTempExpVal = ConvertResultOrObject( _rTempExpVal, EE_NC_UNSIGNED );
		if ( _rTempExpVal.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		_rTempManVal = ConvertResultOrObject( _rTempManVal, EE_NC_UNSIGNED );
		if ( _rTempManVal.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

		CFloatX fTemp;
		if ( _rTempSignBits.u.ui64Val ) {
			fTemp.bSign = _rTempSignVal.u.ui64Val ? true : false;
		}
		fTemp.uiExpBits = static_cast<uint16_t>(_rTempExpBits.u.ui64Val);
		fTemp.uiManBits = static_cast<uint16_t>(_rTempManBits.u.ui64Val);
		fTemp.uiExponent = ((1ULL << fTemp.uiExpBits) - 1ULL) & _rTempExpVal.u.ui64Val;
		fTemp.uiMantissa = ((1ULL << fTemp.uiManBits) - 1ULL) & _rTempManVal.u.ui64Val;
				
		_rResult.ncType = EE_NC_FLOATING;
		_rResult.u.dVal = fTemp.AsDouble();
		return EE_EC_SUCCESS;
	}

	// Creates a float using the specific parameters and a double input.
	CExpEvalContainer::EE_ERROR_CODES CExpEvalContainer::PerformFloatX( EE_RESULT _rTempSignBits, EE_RESULT _rTempExpBits,
		EE_RESULT _rTempManBits, EE_RESULT _rTempImplied, EE_RESULT _rTempDoubleVal,
		EE_RESULT &_rResult ) {
		_rTempSignBits = ConvertResultOrObject( _rTempSignBits, EE_NC_UNSIGNED );
		if ( _rTempSignBits.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		if ( _rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return EE_EC_TOOMANYSIGNBITS; }
		_rTempExpBits = ConvertResultOrObject( _rTempExpBits, EE_NC_UNSIGNED );
		if ( _rTempExpBits.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		if ( _rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return EE_EC_TOOMANYEXPBITS; }
		_rTempManBits = ConvertResultOrObject( _rTempManBits, EE_NC_UNSIGNED );
		if ( _rTempManBits.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		if ( _rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return EE_EC_TOOMANYMANTISSABITS; }
		_rTempImplied = ConvertResultOrObject( _rTempImplied, EE_NC_UNSIGNED );
		if ( _rTempImplied.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
		_rTempDoubleVal = ConvertResultOrObject( _rTempDoubleVal, EE_NC_FLOATING );
		if ( _rTempDoubleVal.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }

		CFloatX fTemp;
		fTemp.CreateFromDouble( _rTempDoubleVal.u.dVal, static_cast<uint16_t>(_rTempExpBits.u.ui64Val), static_cast<uint16_t>(_rTempManBits.u.ui64Val),
			_rTempImplied.u.ui64Val != 0, _rTempSignBits.u.ui64Val != 0 );
		_rResult.ncType = EE_NC_FLOATING;
		_rResult.u.dVal = fTemp.AsDouble();
		return EE_EC_SUCCESS;
	}

	// Deallocates an object.
	bool CExpEvalContainer::DeallocateObject( CObject * _poObj ) {
		// Reverse search because it is most likely the immediate deletion of a temporary object.
		for ( auto I = m_vObjects.size(); I--; ) {
			if ( m_vObjects[I] == _poObj ) {
				m_vObjects.erase( m_vObjects.begin() + I );
				delete _poObj;
				return true;
			}
		}
		return false;
	}

	// String formatting.
	std::string CExpEvalContainer::FormatString( const char * _pcFormat, size_t _stLen, const std::vector<EE_RESULT> &_vArgs ) {
		std::string sTmp;
		size_t stCharLen = 0;
		size_t sArgIdx = 0;
		try {
			while ( _stLen ) {
				uint32_t ui32ThisChar = ee::CExpEval::NextUtf8Char( _pcFormat, _stLen, &stCharLen );

				if ( ui32ThisChar == '{' ) {
					uint32_t ui32NextChar;
					if ( _stLen - stCharLen ) {
						// If there is a next char.
						size_t stNextCharLen = 0;
						ui32NextChar = ee::CExpEval::NextUtf8Char( _pcFormat + stCharLen, _stLen - stCharLen, &stNextCharLen );
						if ( ui32NextChar == '{' ) {
							stCharLen += stNextCharLen;
							// {{ gets reduced to {.
							sTmp.push_back( '{' );
							_pcFormat += stCharLen;
							_stLen -= stCharLen;
							continue;
						}

						_pcFormat += stCharLen;
						_stLen -= stCharLen;
						size_t stArg = sArgIdx;
						std::string sFormatter = EatStringFormatter( _pcFormat, _stLen, stArg, _vArgs );
						if ( stArg < _vArgs.size() ) {
							try {
								switch ( _vArgs[stArg].ncType ) {
									case EE_NC_UNSIGNED : {
										sTmp.append( std::vformat( sFormatter, std::make_format_args( _vArgs[stArg].u.ui64Val ) ) );
										sArgIdx = ++stArg;
										break;
									}
									case EE_NC_SIGNED : {
										sTmp.append( std::vformat( sFormatter, std::make_format_args( _vArgs[stArg].u.i64Val ) ) );
										sArgIdx = ++stArg;
										break;
									}
									case EE_NC_FLOATING : {
										sTmp.append( std::vformat( sFormatter, std::make_format_args( _vArgs[stArg].u.dVal ) ) );
										sArgIdx = ++stArg;
										break;
									}
									case EE_NC_OBJECT : {
										sTmp.append( _vArgs[stArg].u.poObj->FormattedString( sFormatter ) );
										sArgIdx = ++stArg;
										break;
									}
									default : {
										sTmp.append( std::vformat( sFormatter, std::make_format_args( "<null>" ) ) );
										sArgIdx = ++stArg;
									}
								}
							}
							catch ( ... ) {
								sTmp.append( sFormatter );
							}
						}
						else {
							sTmp.append( sFormatter );
						}
						continue;
					}
				}
				else if ( ui32ThisChar == '}' ) {
					// "}" and "}}" both resolve to "}".
					uint32_t ui32NextChar;
					if ( _stLen - stCharLen ) {
						// If there is a next char.
						size_t stNextCharLen = 0;
						ui32NextChar = ee::CExpEval::NextUtf8Char( _pcFormat + stCharLen, _stLen - stCharLen, &stNextCharLen );
						if ( ui32NextChar == '}' ) {
							stCharLen += stNextCharLen;
							// {{ gets reduced to {.
							sTmp.push_back( '}' );
							_pcFormat += stCharLen;
							_stLen -= stCharLen;
							continue;
						}
					}
				}
				while ( stCharLen ) {
					sTmp.push_back( (*_pcFormat++) );
					--_stLen;
					--stCharLen;
				}
			}
		}
		catch ( ... ) {
			sTmp.clear();
		}
		return sTmp;
	}

	// Decodes a string.
	size_t CExpEvalContainer::CreateString( const std::string &_sText ) {
		return AddString( _sText );
	}

	// Decodes an identifier.
	size_t CExpEvalContainer::CreateIdentifier( const char * _pcText ) {
		return AddString( _pcText );
	}

	// Creates a string expression.
	void CExpEvalContainer::CreateStringBasicExp( size_t _sStrIndex, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_STRING;
		_ndNode.u.sStringIndex = _sStrIndex;
		AddNode( _ndNode );
	}

	// Creates a custom-variable expression.
	void CExpEvalContainer::CreateCustomVar( size_t _sStrIndex, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_CUSTOM_VAR;
		_ndNode.u.sStringIndex = _sStrIndex;

		auto aTmp = m_mCustomVariables.find( _sStrIndex );
		if ( aTmp != m_mCustomVariables.end() ) {
			_ndNode.v.bIsConst = (*aTmp).second.bIsConst;
		}
		else { 
			_ndNode.v.bIsConst = false;
		}

		AddNode( _ndNode );
	}

	// Creates an array/access expression.
	void CExpEvalContainer::CreateArrayVar( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARRAY;
		_ndNode.u.sStringIndex = _sStrIndex;
		_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates an array/access expression.
	void CExpEvalContainer::CreateArrayString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_STRINGARRAY;
		_ndNode.u.sStringIndex = _sStrIndex;
		_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp;
		if ( IsConst( m_vNodes[_ndExp.sNodeIndex], rExp ) ) {
			int64_t i64Val = ConvertResultOrObject( rExp, EE_NC_SIGNED ).u.i64Val;
			uint64_t ui64Len = 0;
			size_t sIdx = size_t( i64Val );
			if ( i64Val < 0 ) {
				ui64Len = ee::CExpEval::CountUtfCodePoints( m_vStrings[_sStrIndex] );
				// The length must convert to size_t losslessly.
				if ( ui64Len != static_cast<uint64_t>(static_cast<size_t>(ui64Len)) ) {
					AddNode( _ndNode );
					return;
				}
				sIdx = CBaseApi::ArrayIndexToLinearIndex( i64Val, static_cast<size_t>(ui64Len) );
			}
			else {
				sIdx = static_cast<size_t>(i64Val);
				// Conversion must be lossless.
				if ( i64Val != static_cast<int64_t>(sIdx) ) {
					AddNode( _ndNode );
					return;
				}
			}
			
			_ndNode.u.ui64Val = ee::CExpEval::GetUtf8CodePointByIdx( m_vStrings[_sStrIndex], sIdx );
			_ndNode.nType = EE_N_NUMERICCONSTANT;
			_ndNode.v.ncConstType = EE_NC_UNSIGNED;

		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		
		AddNode( _ndNode );
	}

	// Creates an array/access expression.
	void CExpEvalContainer::CreateArrayString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_STRINGARRAY_EX;
		_ndNode.u.sStringIndex = _sStrIndex;
		_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp0, rExp1;
		if ( IsConst( m_vNodes[_ndExp0.sNodeIndex], rExp0 ) && IsConst( m_vNodes[_ndExp1.sNodeIndex], rExp1 ) ) {
			int64_t i64Val0 = ConvertResultOrObject( rExp0, EE_NC_SIGNED ).u.i64Val;
			int64_t i64Val1 = ConvertResultOrObject( rExp1, EE_NC_SIGNED ).u.i64Val;
			uint64_t ui64Len = 0;
			size_t sIdx0 = size_t( i64Val0 );
			if ( i64Val0 < 0 ) {
				ui64Len = ee::CExpEval::CountUtfCodePoints( m_vStrings[_sStrIndex] );
				// The length must convert to size_t losslessly.
				if ( ui64Len != static_cast<uint64_t>(static_cast<size_t>(ui64Len)) ) {
					AddNode( _ndNode );
					return;
				}
				sIdx0 = CBaseApi::ArrayIndexToLinearIndex( i64Val0, static_cast<size_t>(ui64Len) );
			}
			else {
				sIdx0 = static_cast<size_t>(i64Val0);
				// Conversion must be lossless.
				if ( i64Val0 != static_cast<int64_t>(sIdx0) ) {
					AddNode( _ndNode );
					return;
				}
			}

			size_t sIdx1 = size_t( i64Val1 );
			if ( i64Val1 < 0 ) {
				ui64Len = ee::CExpEval::CountUtfCodePoints( m_vStrings[_sStrIndex] );
				// The length must convert to size_t losslessly.
				if ( ui64Len != static_cast<uint64_t>(static_cast<size_t>(ui64Len)) ) {
					AddNode( _ndNode );
					return;
				}
				sIdx1 = CBaseApi::ArrayIndexToLinearIndex( i64Val1, static_cast<size_t>(ui64Len) );
			}
			else {
				sIdx1 = static_cast<size_t>(i64Val1);
				// Conversion must be lossless.
				if ( i64Val1 != static_cast<int64_t>(sIdx1) ) {
					AddNode( _ndNode );
					return;
				}
			}
			
			size_t sStartIdx = ee::CExpEval::GetUtf8CharPosByIdx( m_vStrings[_sStrIndex], sIdx0 );
			size_t sEndIdx = ee::CExpEval::GetUtf8CharPosByIdx( m_vStrings[_sStrIndex], sIdx1 );
			std::string sTmp;
			for ( size_t I = sStartIdx; I < sEndIdx; ++I ) {
				sTmp.push_back( m_vStrings[_sStrIndex][I] );
			}
			_ndNode.nType = EE_N_STRING;
			_ndNode.u.sStringIndex = AddString( sTmp );
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		
		AddNode( _ndNode );
	}

	// Creates an array/access expression.
	void CExpEvalContainer::CreateArrayToEndString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp0, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_STRINGARRAY_EX;
		_ndNode.u.sStringIndex = _sStrIndex;
		_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
		_ndNode.w.sNodeIndex = size_t( -1 );
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp0;
		if ( IsConst( m_vNodes[_ndExp0.sNodeIndex], rExp0 ) ) {
			int64_t i64Val0 = ConvertResultOrObject( rExp0, EE_NC_SIGNED ).u.i64Val;
			uint64_t ui64Len = 0;
			size_t sIdx0 = size_t( i64Val0 );
			if ( i64Val0 < 0 ) {
				ui64Len = ee::CExpEval::CountUtfCodePoints( m_vStrings[_sStrIndex] );
				// The length must convert to size_t losslessly.
				if ( ui64Len != static_cast<uint64_t>(static_cast<size_t>(ui64Len)) ) {
					AddNode( _ndNode );
					return;
				}
				sIdx0 = CBaseApi::ArrayIndexToLinearIndex( i64Val0, static_cast<size_t>(ui64Len) );
			}
			else {
				sIdx0 = static_cast<size_t>(i64Val0);
				// Conversion must be lossless.
				if ( i64Val0 != static_cast<int64_t>(sIdx0) ) {
					AddNode( _ndNode );
					return;
				}
			}

			
			size_t sStartIdx = ee::CExpEval::GetUtf8CharPosByIdx( m_vStrings[_sStrIndex], sIdx0 );
			std::string sTmp;
			for ( size_t I = sStartIdx; I < m_vStrings[_sStrIndex].size(); ++I ) {
				sTmp.push_back( m_vStrings[_sStrIndex][I] );
			}
			_ndNode.nType = EE_N_STRING;
			_ndNode.u.sStringIndex = AddString( sTmp );
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		
		AddNode( _ndNode );
	}

	// Creates an array/access expression.
	void CExpEvalContainer::CreateArrayFromStartString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp0, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_STRINGARRAY_EX;
		_ndNode.u.sStringIndex = _sStrIndex;
		_ndNode.v.sNodeIndex = size_t( -1 );
		_ndNode.w.sNodeIndex = _ndExp0.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp0;
		if ( IsConst( m_vNodes[_ndExp0.sNodeIndex], rExp0 ) ) {
			int64_t i64Val0 = ConvertResultOrObject( rExp0, EE_NC_SIGNED ).u.i64Val;
			uint64_t ui64Len = 0;
			size_t sIdx0 = size_t( i64Val0 );
			if ( i64Val0 < 0 ) {
				ui64Len = ee::CExpEval::CountUtfCodePoints( m_vStrings[_sStrIndex] );
				// The length must convert to size_t losslessly.
				if ( ui64Len != static_cast<uint64_t>(static_cast<size_t>(ui64Len)) ) {
					AddNode( _ndNode );
					return;
				}
				sIdx0 = CBaseApi::ArrayIndexToLinearIndex( i64Val0, static_cast<size_t>(ui64Len) );
			}
			else {
				sIdx0 = static_cast<size_t>(i64Val0);
				// Conversion must be lossless.
				if ( i64Val0 != static_cast<int64_t>(sIdx0) ) {
					AddNode( _ndNode );
					return;
				}
			}
			
			size_t sEndIdx = ee::CExpEval::GetUtf8CharPosByIdx( m_vStrings[_sStrIndex], sIdx0 );
			std::string sTmp;
			for ( size_t I = 0; I < sEndIdx; ++I ) {
				sTmp.push_back( m_vStrings[_sStrIndex][I] );
			}
			_ndNode.nType = EE_N_STRING;
			_ndNode.u.sStringIndex = AddString( sTmp );
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		
		AddNode( _ndNode );
	}

	// Creates an array/access expression.
	void CExpEvalContainer::CreateArrayAccess( const YYSTYPE::EE_NODE_DATA &_ndObj, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARRAY_ACCESS;
		_ndNode.u.sNodeIndex = _ndObj.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates an array/access expression.
	void CExpEvalContainer::CreateArrayAccessEx( const YYSTYPE::EE_NODE_DATA &_ndObj, size_t _sExp0, size_t _sExp1, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARRAY_ACCESS_EX;
		_ndNode.u.sNodeIndex = _ndObj.sNodeIndex;
		_ndNode.v.sNodeIndex = _sExp0;
		_ndNode.w.sNodeIndex = _sExp1;
		AddNode( _ndNode );
	}

	// Creates a postfix operator.
	void CExpEvalContainer::CreatePostfixOp( size_t _sStrIndex, int32_t _eType, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = _eType == CExpEvalParser::token::EE_PLUSPLUS ? EE_N_POSTFIX_PLUSPLUS : EE_N_POSTFIX_MINUSMINUS;
		_ndNode.u.sStringIndex = _sStrIndex;
		auto aTmp = m_mCustomVariables.find( _sStrIndex );
		if ( aTmp != m_mCustomVariables.end() ) {
			if ( (*aTmp).second.bIsConst ) { throw EE_EC_INVALID_WRITE_TO_CONST; }
		}
 		AddNode( _ndNode );
	}

	// Creates a prefix operator.
	void CExpEvalContainer::CreatePrefixOp( size_t _sStrIndex, int32_t _eType, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = _eType == CExpEvalParser::token::EE_PLUSPLUS ? EE_N_PREFIX_PLUSPLUS : EE_N_PREFIX_MINUSMINUS;
		_ndNode.u.sStringIndex = _sStrIndex;
		auto aTmp = m_mCustomVariables.find( _sStrIndex );
		if ( aTmp != m_mCustomVariables.end() ) {
			if ( (*aTmp).second.bIsConst ) { throw EE_EC_INVALID_WRITE_TO_CONST; }
		}
 		AddNode( _ndNode );
	}

	// Creates a hex constant (0x----).
	void CExpEvalContainer::CreateHex1( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = ee::CExpEval::StoULL( _pcText, 16 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a hex constant (----h).
	void CExpEvalContainer::CreateHex2( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = ee::CExpEval::StoULL( _pcText, 16 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a hex constant (----).
	void CExpEvalContainer::CreateHex3( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = ee::CExpEval::StoULL( _pcText, 16 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a binary constant (0b----).
	void CExpEvalContainer::CreateBin( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = ee::CExpEval::StoULL( _pcText + 2, 2 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a decimal constant.
	void CExpEvalContainer::CreateUInt( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		bool bOverflow;
		_ndNode.u.ui64Val = ee::CExpEval::StoULL( _pcText, 10, nullptr, ~0ULL, &bOverflow );
		if ( bOverflow ) {
			_ndNode.v.ncConstType = EE_NC_FLOATING;
			_ndNode.u.dVal = std::atof( _pcText );
		}
		else {
			_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		}
		AddNode( _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( uint64_t _uiVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = _uiVal;
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( int64_t _iVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.i64Val = _iVal;
		_ndNode.v.ncConstType = EE_NC_SIGNED;
		AddNode( _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( uint32_t _uiVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		CreateNumber( static_cast<uint64_t>(_uiVal), _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( int32_t _iVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		CreateNumber( static_cast<int64_t>(_iVal), _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( float _fVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		CreateDouble( _fVal, _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( double _dVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		CreateDouble( _dVal, _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( long _lVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		CreateNumber( static_cast<int64_t>(_lVal), _ndNode );
	}

	// Creates an oct constant.
	void CExpEvalContainer::CreateOct( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		auto aLen = std::strlen( _pcText );
		if ( aLen > 2 &&
			_pcText[0] == '0' && (_pcText[1] == 'o' || _pcText[1] == 'O') ) {
			_ndNode.u.ui64Val = ee::CExpEval::StoULL( _pcText + 2, 8 );
		}
		else {
			_ndNode.u.ui64Val = ee::CExpEval::StoULL( _pcText, 8 );
		}
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Create a double constant.
	void CExpEvalContainer::CreateDouble( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		CreateDouble( ee::CExpEval::AtoF( _pcText ), _ndNode );
	}

	// Create a double constant.
	void CExpEvalContainer::CreateDouble( double _dVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.dVal = _dVal;
		_ndNode.v.ncConstType = EE_NC_FLOATING;
		AddNode( _ndNode );
	}

	// Create a Unicode \N{} numeric constant.
	void CExpEvalContainer::CreateUnicodeNumericConstant( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		const char * pcStart = nullptr;
		size_t sLen = 0;
		for ( size_t I = 0; _pcText[I]; ++I ) {
			if ( _pcText[I] == '{' ) { pcStart = &_pcText[I+1]; }
			else if ( _pcText[I] == '}' ) {
				sLen = &_pcText[I] - pcStart;
				break;
			}
		}
		if ( pcStart && sLen ) {
			uint32_t ui32Code = CUnicode::GetCode( pcStart, sLen );
			if ( CUnicode::EE_INVALID != ui32Code ) {
				CreateNumber( ui32Code, _ndNode );
				return;
			}
		}
		CreateNumber( 0, _ndNode );
	}

	// Create a reinterpretation of bits to a float.
	void CExpEvalContainer::CreateAsFloat( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ASFLOAT;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp;
		if ( IsConst( m_vNodes[_ndExp.sNodeIndex], rExp ) ) {
			EE_RESULT rFinal;
			if ( PerformAsFloat( rExp, rFinal ) == EE_EC_SUCCESS ) {
				_ndNode.nType = EE_N_NUMERICCONSTANT;
				_ndNode.u.ui64Val = rFinal.u.ui64Val;
				_ndNode.v.ncConstType = rFinal.ncType;
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		AddNode( _ndNode );
	}

	// Create a reinterpretation of bits to a float of a common type.
	void CExpEvalContainer::CreateAsFloat( const YYSTYPE::EE_NODE_DATA &_ndExp, int32_t _eType, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = static_cast<ee::EE_NODES>(_eType);
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp;
		if ( IsConst( m_vNodes[_ndExp.sNodeIndex], rExp ) ) {
			EE_RESULT rFinal;
			switch ( _ndNode.nType ) {
				case ee::EE_N_ASFLOAT16 : {
					if ( PerformToFloat16( rExp, rFinal ) == EE_EC_SUCCESS ) {
						_ndNode.nType = EE_N_NUMERICCONSTANT;
						_ndNode.u.ui64Val = rFinal.u.ui64Val;
						_ndNode.v.ncConstType = rFinal.ncType;
					}
					break;
				}
				case ee::EE_N_ASFLOAT14 : {
					if ( PerformToFloat14( rExp, rFinal ) == EE_EC_SUCCESS ) {
						_ndNode.nType = EE_N_NUMERICCONSTANT;
						_ndNode.u.ui64Val = rFinal.u.ui64Val;
						_ndNode.v.ncConstType = rFinal.ncType;
					}
					break;
				}
				case ee::EE_N_ASFLOAT11 : {
					if ( PerformToFloat11( rExp, rFinal ) == EE_EC_SUCCESS ) {
						_ndNode.nType = EE_N_NUMERICCONSTANT;
						_ndNode.u.ui64Val = rFinal.u.ui64Val;
						_ndNode.v.ncConstType = rFinal.ncType;
					}
					break;
				}
				case ee::EE_N_ASFLOAT10 : {
					if ( PerformToFloat10( rExp, rFinal ) == EE_EC_SUCCESS ) {
						_ndNode.nType = EE_N_NUMERICCONSTANT;
						_ndNode.u.ui64Val = rFinal.u.ui64Val;
						_ndNode.v.ncConstType = rFinal.ncType;
					}
					break;
				}
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		AddNode( _ndNode );
	}

	// Create a reinterpretation of bits to a double.
	void CExpEvalContainer::CreateAsDouble( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ASDOUBLE;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp;
		if ( IsConst( m_vNodes[_ndExp.sNodeIndex], rExp ) ) {
			EE_RESULT rFinal;
			if ( PerformAsDouble( rExp, rFinal ) == EE_EC_SUCCESS ) {
				_ndNode.nType = EE_N_NUMERICCONSTANT;
				_ndNode.u.ui64Val = rFinal.u.ui64Val;
				_ndNode.v.ncConstType = rFinal.ncType;
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		AddNode( _ndNode );
	}

	// Create a reinterpretation of bits to a double with the given bit specifications.
	void CExpEvalContainer::CreateAsFloatX( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
		const YYSTYPE::EE_NODE_DATA &_ndExpBits,
		const YYSTYPE::EE_NODE_DATA &_ndManBits,
		const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
		const YYSTYPE::EE_NODE_DATA &_ndSignValue,	// 0 or 1.
		const YYSTYPE::EE_NODE_DATA &_ndExpValue,
		const YYSTYPE::EE_NODE_DATA &_ndManValue,
		YYSTYPE::EE_NODE_DATA &_ndNode ) {

		_ndNode.nType = EE_N_ASXFLOAT;
		_ndNode.u.sNodeIndex = _ndSignBits.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndExpBits.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndManBits.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndImplicitMan.sNodeIndex;
		_ndNode.y.sNodeIndex = _ndSignValue.sNodeIndex;
		_ndNode.z.sNodeIndex = _ndExpValue.sNodeIndex;
		_ndNode.a.sNodeIndex = _ndManValue.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp0, rExp1, rExp2, rExp3, rExp4, rExp5, rExp6;
		if ( IsConst( m_vNodes[_ndSignBits.sNodeIndex], rExp0 ) &&
			IsConst( m_vNodes[_ndExpBits.sNodeIndex], rExp1 ) &&
			IsConst( m_vNodes[_ndManBits.sNodeIndex], rExp2 ) &&
			IsConst( m_vNodes[_ndImplicitMan.sNodeIndex], rExp3 ) &&
			IsConst( m_vNodes[_ndSignValue.sNodeIndex], rExp4 ) &&
			IsConst( m_vNodes[_ndExpValue.sNodeIndex], rExp5 ) &&
			IsConst( m_vNodes[_ndManValue.sNodeIndex], rExp6 ) ) {
			EE_RESULT rFinal;
			if ( PerformFloatX( rExp0, rExp1, rExp2, rExp3, rExp4, rExp5, rExp6, rFinal ) == EE_EC_SUCCESS ) {
				_ndNode.nType = EE_N_NUMERICCONSTANT;
				_ndNode.u.ui64Val = rFinal.u.ui64Val;
				_ndNode.v.ncConstType = rFinal.ncType;
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		AddNode( _ndNode );
	}

	// Create a reinterpretation of bits to a double with the given bit specifications.
	void CExpEvalContainer::CreateAsFloatX( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
		const YYSTYPE::EE_NODE_DATA &_ndExpBits,
		const YYSTYPE::EE_NODE_DATA &_ndManBits,
		const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
		const YYSTYPE::EE_NODE_DATA &_ndExp,	// The double value to convert.
			
		YYSTYPE::EE_NODE_DATA &_ndNode ) {

		_ndNode.nType = EE_N_ASXFLOAT_FROM_DOUBLE;
		_ndNode.u.sNodeIndex = _ndSignBits.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndExpBits.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndManBits.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndImplicitMan.sNodeIndex;
		_ndNode.y.sNodeIndex = _ndExp.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp0, rExp1, rExp2, rExp3, rExp4;
		if ( IsConst( m_vNodes[_ndSignBits.sNodeIndex], rExp0 ) &&
			IsConst( m_vNodes[_ndExpBits.sNodeIndex], rExp1 ) &&
			IsConst( m_vNodes[_ndManBits.sNodeIndex], rExp2 ) &&
			IsConst( m_vNodes[_ndImplicitMan.sNodeIndex], rExp3 ) &&
			IsConst( m_vNodes[_ndExp.sNodeIndex], rExp4 ) ) {
			EE_RESULT rFinal;
			if ( PerformFloatX( rExp0, rExp1, rExp2, rExp3, rExp4, rFinal ) == EE_EC_SUCCESS ) {
				_ndNode.nType = EE_N_NUMERICCONSTANT;
				_ndNode.u.ui64Val = rFinal.u.ui64Val;
				_ndNode.v.ncConstType = rFinal.ncType;
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		AddNode( _ndNode );
	}

	// Gets a property of a floating-point type with the given bit configuration.
	void CExpEvalContainer::CreateAsFloatXProp( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
		const YYSTYPE::EE_NODE_DATA &_ndExpBits,
		const YYSTYPE::EE_NODE_DATA &_ndManBits,
		const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
		const EE_NODES _nProp,
			
		YYSTYPE::EE_NODE_DATA &_ndNode ) {

		_ndNode.nType = _nProp;
		_ndNode.u.sNodeIndex = _ndSignBits.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndExpBits.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndManBits.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndImplicitMan.sNodeIndex;
		AddNode( _ndNode );
	}

	// Gets a property of a floating-point type with the given bit configuration.
	void CExpEvalContainer::CreateAsFloatXProp( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
		const YYSTYPE::EE_NODE_DATA &_ndExpBits,
		const YYSTYPE::EE_NODE_DATA &_ndManBits,
		const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
		const YYSTYPE::EE_NODE_DATA &_ndExp,	// The double value to convert.
		const EE_NODES _nProp,
			
		YYSTYPE::EE_NODE_DATA &_ndNode ) {

		_ndNode.nType = _nProp;
		_ndNode.u.sNodeIndex = _ndSignBits.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndExpBits.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndManBits.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndImplicitMan.sNodeIndex;
		_ndNode.y.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a cast node.
	void CExpEvalContainer::CreateCast( const YYSTYPE::EE_NODE_DATA &_ndExp, EE_CAST_TYPES _ctCast, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_CAST;
		_ndNode.v.ctCast = _ctCast;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Create an address node.
	void CExpEvalContainer::CreateAddress( const YYSTYPE::EE_NODE_DATA &_ndExp, EE_CAST_TYPES _ctCast, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ADDRESS;
		_ndNode.v.ctCast = _ctCast;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		m_bHasAddressRead = true;
		AddNode( _ndNode );
	}

	// Creates member access.
	void CExpEvalContainer::CreateMemberAccess( const YYSTYPE::EE_NODE_DATA &_ndExp, size_t _sMember, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_MEMBERACCESS;
		_ndNode.v.sStringIndex = _sMember;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a user (??) node.
	void CExpEvalContainer::CreateUser( YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_USER_VAR;
		m_bHasUserVar = true;
		AddNode( _ndNode );
	}

	// Creates a number parameter ($<decimal>) node.
	void CExpEvalContainer::CreateNumberedParm( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMBERED_PARM;
		_ndNode.u.ui64Val = ee::CExpEval::StoULL( &_pcText[1] );
		m_sNumberedParmsAccessed.insert( static_cast<size_t>(_ndNode.u.ui64Val) );
		AddNode( _ndNode );
	}

	// Creates a dynamic numbered parameter node.
	void CExpEvalContainer::CreateDynamicParm( size_t _sStrIndex, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_DYNAMIC_NUMBERED_PARM;
		_ndNode.u.sStringIndex = _sStrIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp;
		rExp.ncType = EE_NC_INVALID;

		auto aFind = m_mCustomVariables.find( _sStrIndex );
		if ( aFind != m_mCustomVariables.end() ) {
			if ( aFind->second.bIsConst ) {
				rExp = ConvertResultOrObject( aFind->second.rRes, EE_NC_UNSIGNED );
				if ( rExp.ncType == EE_NC_INVALID ) { throw EE_EC_INVALIDCAST; }
				_ndNode.nType = EE_N_NUMBERED_PARM;
				_ndNode.u.ui64Val = rExp.u.ui64Val;
				m_sNumberedParmsAccessed.insert( static_cast<size_t>(_ndNode.u.ui64Val) );
			}
		}
		if ( (rExp.ncType) == EE_NC_INVALID ) {
			m_sNumberedParmsAccessed.insert( static_cast<size_t>(-1) );
		}
#else
		m_sNumberedParmsAccessed.insert( static_cast<size_t>(-1) );
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		AddNode( _ndNode );
	}

	// Creates a dynamic numbered parameter node.
	void CExpEvalContainer::CreateDynamicParmExp( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_DYNAMIC_NUMBERED_PARM_EXP;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;

#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp;
		rExp.ncType = EE_NC_INVALID;
		if ( IsConst( m_vNodes[_ndExp.sNodeIndex], rExp ) ) {
			rExp = ConvertResultOrObject( rExp, EE_NC_UNSIGNED );
			if ( rExp.ncType == EE_NC_INVALID ) { throw EE_EC_INVALIDCAST; }
			_ndNode.nType = EE_N_NUMBERED_PARM;
			_ndNode.u.ui64Val = rExp.u.ui64Val;
			m_sNumberedParmsAccessed.insert( static_cast<size_t>(_ndNode.u.ui64Val) );
		}
		if ( (rExp.ncType) == EE_NC_INVALID ) {
			m_sNumberedParmsAccessed.insert( static_cast<size_t>(-1) );
		}
#else
		m_sNumberedParmsAccessed.insert( static_cast<size_t>(-1) );
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME

		AddNode( _ndNode );
	}

	// Creates a node that gets the total numbered parameters.
	void CExpEvalContainer::CreateParmTotal( YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMBERED_PARM_TOTAL;
		AddNode( _ndNode );
	}

	// Create a unary node.
	void CExpEvalContainer::CreateUnary( const YYSTYPE::EE_NODE_DATA &_ndExp, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_UNARY;
		_ndNode.v.ui32Op = _uiOp;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp;
		if ( IsConst( m_vNodes[_ndExp.sNodeIndex], rExp ) ) {
			EE_RESULT rFinal;
			if ( PerformUnary( rExp, _uiOp, rFinal ) == EE_EC_SUCCESS ) {
				_ndNode.nType = EE_N_NUMERICCONSTANT;
				_ndNode.u.ui64Val = rFinal.u.ui64Val;
				_ndNode.v.ncConstType = rFinal.ncType;
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		AddNode( _ndNode );
	}

	// Create an operator node.
	void CExpEvalContainer::CreateOp( const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_OP;
		_ndNode.v.ui32Op = _uiOp;
		_ndNode.u.sNodeIndex = _ndLeft.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndRight.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rLeft, rRight;
		if ( IsConst( m_vNodes[_ndLeft.sNodeIndex], rLeft ) &&
			IsConst( m_vNodes[_ndRight.sNodeIndex], rRight ) ) {
			EE_RESULT rFinal;
			if ( PerformOp( rLeft, _uiOp, rRight, rFinal ) == EE_EC_SUCCESS ) {
				YYSTYPE::EE_NODE_DATA & ndTarget = m_vNodes[_ndRight.sNodeIndex];
				ndTarget.nType = EE_N_NUMERICCONSTANT;
				ndTarget.u.ui64Val = rFinal.u.ui64Val;
				ndTarget.v.ncConstType = rFinal.ncType;
				_ndNode.nType = ndTarget.nType;
				_ndNode.u.ui64Val = ndTarget.u.ui64Val;
				_ndNode.v.ncConstType = ndTarget.v.ncConstType;
				_ndNode.sNodeIndex = ndTarget.sNodeIndex;
				return;
				/*_ndNode.nType = EE_N_NUMERICCONSTANT;
				_ndNode.u.ui64Val = rFinal.u.ui64Val;
				_ndNode.v.ncConstType = rFinal.ncType;*/
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		AddNode( _ndNode );
	}

	// Create a conditional operator.
	void CExpEvalContainer::CreateConditional( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_CONDITIONAL;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndLeft.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndRight.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp;
		if ( IsConst( m_vNodes[_ndExp.sNodeIndex], rExp ) ) {
			switch ( rExp.ncType ) {
				case EE_NC_UNSIGNED : {
					_ndNode = rExp.u.ui64Val ? _ndLeft : _ndRight;
					break;
				}
				case EE_NC_SIGNED : {
					_ndNode = rExp.u.i64Val ? _ndLeft : _ndRight;
					break;
				}
				case EE_NC_FLOATING : {
					_ndNode = rExp.u.dVal ? _ndLeft : _ndRight;
					break;
				}
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		AddNode( _ndNode );
	}

	// Creates an assignment operator.
	void CExpEvalContainer::CreateAssignment( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndRight, uint32_t _uiOp, bool _bIsConst, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ASSIGNMENT;
		_ndNode.v.ui32Op = _uiOp;
		_ndNode.u.sNodeIndex = _ndRight.sNodeIndex;
		_ndNode.w.sNodeIndex = _sStrIndex;

		auto aTmp = m_mCustomVariables.find( _sStrIndex );
		if ( aTmp == m_mCustomVariables.end() ) {
			CExpEvalContainer::EE_CUSTOM_VAR cvVar = {
				0,
				CExpEvalContainer::DefaultResult(),
				_bIsConst
			};
			m_mCustomVariables[_sStrIndex] = cvVar;
			aTmp = m_mCustomVariables.find( _sStrIndex );
		}
		if ( aTmp != m_mCustomVariables.end() ) {
			(*aTmp).second.bIsConst = _bIsConst;
			if ( _bIsConst && _ndNode.v.ui32Op == '=' ) {	// Note: This is only called with '='.
				EE_RESULT rTmp;
				if ( IsConst( m_vNodes[_ndNode.u.sNodeIndex], rTmp ) ) {
					(*aTmp).second.rRes = rTmp;
					_ndNode.nType = EE_N_NUMERICCONSTANT;
					_ndNode.u.ui64Val = rTmp.u.ui64Val;
					_ndNode.v.ncConstType = rTmp.ncType;
					++(*aTmp).second.ui64UpdateCounter;
				}
				else {
					throw EE_EC_CONST_VAR_REQUIRES_CONST_EPRESSION;
				}
			}
		}

		/*
		CExpEvalContainer::EE_CUSTOM_VAR cvVar = {
			0,
			CExpEvalContainer::DefaultResult(),
		};
		//m_mCustomVariables.insert_or_assign( _ndNode.w.sNodeIndex, cvVar );
		auto aTmp = m_mCustomVariables.find( _ndNode.w.sNodeIndex );
		if ( aTmp != m_mCustomVariables.end() ) {
			if ( (*aTmp).second.bIsConst ) {
			}
		}
		else {
			m_mCustomVariables[_ndNode.w.sNodeIndex] = cvVar;
		}*/
		AddNode( _ndNode );
	}

	// Creates an assignment operator (to change a variable that has already been created.
	void CExpEvalContainer::CreateReAssignment( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndRight, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ASSIGNMENT;
		_ndNode.v.ui32Op = _uiOp;
		_ndNode.u.sNodeIndex = _ndRight.sNodeIndex;
		_ndNode.w.sNodeIndex = _sStrIndex;
		auto aTmp = m_mCustomVariables.find( _sStrIndex );
		if ( aTmp != m_mCustomVariables.end() ) {
			if ( (*aTmp).second.bIsConst ) { throw EE_EC_INVALID_WRITE_TO_CONST; }
		}
		AddNode( _ndNode );
	}

	// Creates a raw data array.
	void CExpEvalContainer::CreateRawArray( size_t _sStrIndex, uint32_t _ui32Backing, uint32_t _ui32BackingPersistence, const YYSTYPE::EE_NODE_DATA &_ndSize,
		size_t _sStartValueNodeIdx, size_t _sEndValueNodeIdx, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_CREATE_ARRAY;
		_ndNode.u.sStringIndex = _sStrIndex;
		_ndNode.v.ui32Op = _ui32Backing | (_ui32BackingPersistence << 16);
		_ndNode.w.sNodeIndex = _sStartValueNodeIdx;
		_ndNode.x.sNodeIndex = _sEndValueNodeIdx;
		_ndNode.z.sNodeIndex = _ndSize.sNodeIndex;

		// Add its entry to the global array list.
		EE_ARRAY_DATA adData = { 0 };
		_ndNode.y.sNodeIndex = m_vArrayData.size();
		m_vArrayData.push_back( adData );

		// Add it to the look-up registery (causes syntax to change so that this identifier is seen as EE_ARRAY instead of EE_IDENTIFIER).
		m_mArrays.insert_or_assign( _sStrIndex, _ndNode.y.sNodeIndex );
			
		AddNode( _ndNode );
	}

	// Creates an assignment operator to assign a value in an array.
	void CExpEvalContainer::CreateArrayReAssignment( size_t _sArrayIndex, const YYSTYPE::EE_NODE_DATA &_ndArray, const YYSTYPE::EE_NODE_DATA &_ndValue, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARRAY_ASSIGNMENT;
		_ndNode.v.ui32Op = _uiOp;
		_ndNode.u.sNodeIndex = _ndArray.sNodeIndex;
		_ndNode.w.sNodeIndex = _sArrayIndex;
		_ndNode.x.sNodeIndex = _ndValue.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates an assignment operator to assign a value in an array.
	void CExpEvalContainer::CreateArrayReAssignmentObj( size_t _sArrayIndex, const YYSTYPE::EE_NODE_DATA &_ndIdx, const YYSTYPE::EE_NODE_DATA &_ndValue, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARRAY_ASSIGNMENT_OBJ;
		_ndNode.v.ui32Op = _uiOp;
		_ndNode.u.sNodeIndex = _ndIdx.sNodeIndex;
		_ndNode.w.sNodeIndex = _sArrayIndex;
		_ndNode.x.sNodeIndex = _ndValue.sNodeIndex;

		AddNode( _ndNode );
	}

	// Creates an assignment operator to assign a value in an array.
	void CExpEvalContainer::CreateArrayReAssignment( const YYSTYPE::EE_NODE_DATA &_ndObj, const YYSTYPE::EE_NODE_DATA &_ndArray, const YYSTYPE::EE_NODE_DATA &_ndValue, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARRAY_ASSIGNMENT_OBJ;
		_ndNode.v.ui32Op = _uiOp;
		_ndNode.u.sNodeIndex = _ndArray.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndObj.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndValue.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates an assignment operator to write to an external address.
	void CExpEvalContainer::CreateAddressAssignment( EE_CAST_TYPES _ctCast, const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndValue, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ADDRESS_ASSIGNMENT;
		_ndNode.v.ctCast = _ctCast;
		_ndNode.u.ui32Intrinsic = _uiOp;
		_ndNode.w.sNodeIndex = _ndExp.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndValue.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates an array initializer.
	void CExpEvalContainer::CreateArrayInitializer( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARRAY_INITIALIZER;
		_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
		//_ndNode.u.sStringIndex = m_vVectorStack[m_vVectorStack.size()-1];
		AddNode( _ndNode );
	}

	// Creates an array initializer list.
	void CExpEvalContainer::CreateArrayInitializerList( size_t _stLeftIdx, size_t _stRightIdx, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARRAY_INITIALIZER_LIST;
		_ndNode.v.sNodeIndex = _stLeftIdx;
		_ndNode.u.sNodeIndex = _stRightIdx;
		AddNode( _ndNode );
	}

	// Creates a vector.
	void CExpEvalContainer::CreateVector( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARRAY_CREATE_VECTOR;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;

		_ndNode.v.sStringIndex = m_vObjects.size();
		ee::CObject * psObj = reinterpret_cast<ee::CVector *>(AllocateObject<ee::CVector>());
		if ( !psObj ) { throw EE_EC_OUTOFMEMORY; }

		AddNode( _ndNode );
	}

	// Creates a vector.
	void CExpEvalContainer::CreateVector( YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARRAY_CREATE_VECTOR;
		_ndNode.u.sNodeIndex = size_t( ~0 );

		_ndNode.v.sStringIndex = m_vObjects.size();
		ee::CObject * psObj = reinterpret_cast<ee::CVector *>(AllocateObject<ee::CVector>());
		if ( !psObj ) { throw EE_EC_OUTOFMEMORY; }

		AddNode( _ndNode );
	}

	// Create a 0-parm intrinsic.
	void CExpEvalContainer::CreateIntrinsic0( uint32_t _uiIntrinsic, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		switch ( _uiIntrinsic ) {
			case CExpEvalParser::token::EE_CLOCK : {
				_ndNode.nType = EE_N_INTRINSIC_0_SIGNED;
				_ndNode.uFuncPtr.pfClock = ::clock;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_SECONDS : {
				_ndNode.nType = EE_N_INTRINSIC_0_UNSIGNED_DIVISOR;
				_ndNode.uFuncPtr.pfIntrins0Unsigned = ee::CExpEval::TicksToMicroseconds;
				_ndNode.u.ui64Val = 1000000ULL;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_MILLISECONDS : {
				_ndNode.nType = EE_N_INTRINSIC_0_UNSIGNED_DIVISOR;
				_ndNode.uFuncPtr.pfIntrins0Unsigned = ee::CExpEval::TicksToMicroseconds;
				_ndNode.u.ui64Val = 1000ULL;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_MICROSECONDS : {
				_ndNode.nType = EE_N_INTRINSIC_0_UNSIGNED_DIVISOR;
				_ndNode.uFuncPtr.pfIntrins0Unsigned = ee::CExpEval::TicksToMicroseconds;
				_ndNode.u.ui64Val = 1ULL;
				AddNode( _ndNode );
				return;
			}
		}
		_ndNode.nType = EE_N_INTRINSIC_0;
		_ndNode.u.ui32Intrinsic = _uiIntrinsic;
		AddNode( _ndNode );
	}

	// Create a 1-parm intrinsic.
	void CExpEvalContainer::CreateIntrinsic1( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp;
		if ( IsConst( m_vNodes[_ndExp.sNodeIndex], rExp ) ) {
			EE_RESULT rFinal;
			if ( PerformIntrinsic( rExp, _uiIntrinsic, rFinal ) == EE_EC_SUCCESS ) {
				_ndNode.nType = EE_N_NUMERICCONSTANT;
				_ndNode.u.ui64Val = rFinal.u.ui64Val;
				_ndNode.v.ncConstType = rFinal.ncType;
				AddNode( _ndNode );
				return;
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		switch ( _uiIntrinsic ) {
			case CExpEvalParser::token::EE_COS : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::cos;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_SIN : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::sin;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_TAN : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::tan;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_ACOS : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::acos;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_ASIN : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::asin;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_ATAN : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::atan;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_COSH : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::cosh;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_SINH : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::sinh;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_TANH : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::tanh;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_ACOSH : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::acosh;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_ASINH : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::asinh;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_ATANH : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::atanh;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_EXP : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::exp;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_LOG : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::log;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_LOG10 : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::log10;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_LOG2 : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::log2;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_EXP2 : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::exp2;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_EXPM1 : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::expm1;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_LOG1P : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::log1p;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_LOGB : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::logb;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_SQRT : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::sqrt;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_CBRT : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::cbrt;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_TGAMMA : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::tgamma;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_LGAMMA : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::lgamma;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_CEIL : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::ceil;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_FLOOR : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::floor;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_TRUNC : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::trunc;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_ROUND : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::round;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_NEARBYINT : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::nearbyint;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_ABS : {
				_ndNode.nType = EE_N_INTRINSIC_1_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Float_Float = std::abs;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_BYTESWAPUSHORT : {
				_ndNode.nType = EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED16;
				_ndNode.uFuncPtr.pfIntrins1Unsigned_Unsigned16 = ::_byteswap_ushort;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_BYTESWAPULONG : {
				_ndNode.nType = EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED32;
				_ndNode.uFuncPtr.pfIntrins1Unsigned_Unsigned32 = ::_byteswap_ulong;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_BYTESWAPUINT64 : {
				_ndNode.nType = EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED64;
				_ndNode.uFuncPtr.pfIntrins1Unsigned_Unsigned64 = ::_byteswap_uint64;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_ISINF : {
				_ndNode.nType = EE_N_INTRINSIC_1_BOOL_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Bool_Float = std::isinf;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_ISNAN : {
				_ndNode.nType = EE_N_INTRINSIC_1_BOOL_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Bool_Float = std::isnan;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_ILOGB : {
				_ndNode.nType = EE_N_INTRINSIC_1_INT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins1Signed_Float = std::ilogb;
				_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
		}
		_ndNode.nType = EE_N_INTRINSIC_1;
		_ndNode.u.ui32Intrinsic = _uiIntrinsic;
		_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Create a 2-parm intrinsic.
	void CExpEvalContainer::CreateIntrinsic2( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, YYSTYPE::EE_NODE_DATA &_ndNode ) {
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp0, rExp1;
		if ( IsConst( m_vNodes[_ndExp0.sNodeIndex], rExp0 ) && 
			IsConst( m_vNodes[_ndExp1.sNodeIndex], rExp1 ) ) {
			EE_RESULT rFinal;
			if ( PerformIntrinsic( rExp0, rExp1, _uiIntrinsic, rFinal, false ) == EE_EC_SUCCESS ) {
				_ndNode.nType = EE_N_NUMERICCONSTANT;
				_ndNode.u.ui64Val = rFinal.u.ui64Val;
				_ndNode.v.ncConstType = rFinal.ncType;
				AddNode( _ndNode );
				return;
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		switch ( _uiIntrinsic ) {
			case CExpEvalParser::token::EE_ATAN2 : {
				_ndNode.nType = EE_N_INTRINSIC_2_FLOAT_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins2Float_Float_Float = ::atan2;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_POW : {
				_ndNode.nType = EE_N_INTRINSIC_2_FLOAT_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins2Float_Float_Float = ::pow;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_HYPOT : {
				_ndNode.nType = EE_N_INTRINSIC_2_FLOAT_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins2Float_Float_Float = ::hypot;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_MOD : {
				_ndNode.nType = EE_N_INTRINSIC_2_FLOAT_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins2Float_Float_Float = ::fmod;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_REMAINDER : {
				_ndNode.nType = EE_N_INTRINSIC_2_FLOAT_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins2Float_Float_Float = ::remainder;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_NEXTAFTER : {
				_ndNode.nType = EE_N_INTRINSIC_2_FLOAT_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins2Float_Float_Float = ::nextafter;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_NEXTAFTERF : {
				_ndNode.nType = EE_N_INTRINSIC_2_FLOAT_FLOAT32_FLOAT32;
				_ndNode.uFuncPtr.pfIntrins2Float32_Float32_Float32 = ::nextafterf;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_NEXTTOWARD : {
				_ndNode.nType = EE_N_INTRINSIC_2_FLOAT_FLOAT80_FLOAT80;
				_ndNode.uFuncPtr.pfIntrins2Float80_Float80_Float80 = ::nexttoward;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_NEXTTOWARDF : {
				_ndNode.nType = EE_N_INTRINSIC_2_FLOAT_FLOAT32_FLOAT80;
				_ndNode.uFuncPtr.pfIntrins2Float32_Float32_Float80 = ::nexttowardf;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_DIM : {
				_ndNode.nType = EE_N_INTRINSIC_2_FLOAT_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins2Float_Float_Float = ::fdim;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
		}
		_ndNode.nType = EE_N_INTRINSIC_2;
		_ndNode.u.ui32Intrinsic = _uiIntrinsic;
		_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;

		AddNode( _ndNode );
	}

	// Create a 3-parm intrinsic.
	void CExpEvalContainer::CreateIntrinsic3( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, const YYSTYPE::EE_NODE_DATA &_ndExp2, YYSTYPE::EE_NODE_DATA &_ndNode ) {
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp0, rExp1, rExp2;
		if ( IsConst( m_vNodes[_ndExp0.sNodeIndex], rExp0 ) &&
			IsConst( m_vNodes[_ndExp1.sNodeIndex], rExp1 ) &&
			IsConst( m_vNodes[_ndExp2.sNodeIndex], rExp2 ) ) {
			EE_RESULT rFinal;
			if ( PerformIntrinsic( rExp0, rExp1, rExp2, _uiIntrinsic, rFinal ) == EE_EC_SUCCESS ) {
				_ndNode.nType = EE_N_NUMERICCONSTANT;
				_ndNode.u.ui64Val = rFinal.u.ui64Val;
				_ndNode.v.ncConstType = rFinal.ncType;
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME

		switch ( _uiIntrinsic ) {
			case CExpEvalParser::token::EE_EPSILON: {
				_ndNode.nType = EE_N_INTRINSIC_3_BOOL_FLOAT_FLOAT_FLOAT;
				bool (* pfFunc)( double, double, double ) = ee::CExpEval::Epsilon;
				_ndNode.uFuncPtr.pfIntrins3Bool_Float_Float_Float = pfFunc;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				_ndNode.x.sNodeIndex = _ndExp2.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_EPSILONF: {
				_ndNode.nType = EE_N_INTRINSIC_3_BOOL_FLOAT32_FLOAT32_FLOAT32;
				bool (* pfFunc)( float, float, float ) = ee::CExpEval::Epsilon;
				_ndNode.uFuncPtr.pfIntrins3Bool_Float32_Float32_Float32 = pfFunc;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				_ndNode.x.sNodeIndex = _ndExp2.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_RELEPSILON: {
				_ndNode.nType = EE_N_INTRINSIC_3_BOOL_FLOAT_FLOAT_FLOAT;
				bool (* pfFunc)( double, double, double ) = ee::CExpEval::RelativeEpsilon;
				_ndNode.uFuncPtr.pfIntrins3Bool_Float_Float_Float = pfFunc;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				_ndNode.x.sNodeIndex = _ndExp2.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_RELEPSILONF: {
				_ndNode.nType = EE_N_INTRINSIC_3_BOOL_FLOAT32_FLOAT32_FLOAT32;
				bool (* pfFunc)( float, float, float ) = ee::CExpEval::RelativeEpsilon;
				_ndNode.uFuncPtr.pfIntrins3Bool_Float32_Float32_Float32 = pfFunc;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				_ndNode.x.sNodeIndex = _ndExp2.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
			case CExpEvalParser::token::EE_MADD : {
				_ndNode.nType = EE_N_INTRINSIC_3_FLOAT_FLOAT_FLOAT_FLOAT;
				_ndNode.uFuncPtr.pfIntrins3Float_Float_Float_Float = ::fma;
				_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
				_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
				_ndNode.x.sNodeIndex = _ndExp2.sNodeIndex;
				AddNode( _ndNode );
				return;
			}
		}
		_ndNode.nType = EE_N_INTRINSIC_3;
		_ndNode.u.ui32Intrinsic = _uiIntrinsic;
		_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndExp2.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a compound statement.
	void CExpEvalContainer::CreateCompoundStatement( const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_COMPOUND_STATEMENT;
		_ndNode.v.sNodeIndex = _ndLeft.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndRight.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a while loop.
	void CExpEvalContainer::CreateWhileLoop( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_WHILE_LOOP;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndStatements.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a for loop.
	void CExpEvalContainer::CreateForLoop( const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, const YYSTYPE::EE_NODE_DATA &_ndExp2, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_FOR_LOOP;
		_ndNode.u.sNodeIndex = _ndExp0.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndExp1.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndExp2.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndStatements.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a for loop.
	void CExpEvalContainer::CreateForLoop( const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_FOR_LOOP;
		_ndNode.u.sNodeIndex = _ndExp0.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndExp1.sNodeIndex;
		_ndNode.w.sNodeIndex = size_t( ~0 );
		_ndNode.x.sNodeIndex = _ndStatements.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a do-while loop.
	void CExpEvalContainer::CreateDoWhileLoop( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_DO_WHILE_LOOP;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndStatements.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a foreach declaration.
	void CExpEvalContainer::CreateForEachDecl( size_t _sStrIndex, size_t _sArrayIdx, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_FOREACHDECL0;
		_ndNode.u.sNodeIndex = _sStrIndex;
		_ndNode.v.sNodeIndex = _sArrayIdx;
		CExpEvalContainer::EE_CUSTOM_VAR cvVar = {
			0,
			CExpEvalContainer::DefaultResult(),
		};
		//m_mCustomVariables.insert_or_assign( _sStrIndex, cvVar );
		if ( m_mCustomVariables.find( _sStrIndex ) == m_mCustomVariables.end() ) {
			m_mCustomVariables[_sStrIndex] = cvVar;
		}
		AddNode( _ndNode );
	}

	// Creates a foreach declaration.
	void CExpEvalContainer::CreateForEachCustomDecl( size_t _sStrIndex, size_t _sArrayIdx, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_FOREACHDECL1;
		_ndNode.u.sNodeIndex = _sStrIndex;
		_ndNode.v.sNodeIndex = _sArrayIdx;
		CExpEvalContainer::EE_CUSTOM_VAR cvVar = {
			0,
			CExpEvalContainer::DefaultResult(),
		};
		//m_mCustomVariables.insert_or_assign( _sStrIndex, cvVar );
		if ( m_mCustomVariables.find( _sStrIndex ) == m_mCustomVariables.end() ) {
			m_mCustomVariables[_sStrIndex] = cvVar;
		}
		AddNode( _ndNode );
	}

	// Creates a foreach declaration.
	void CExpEvalContainer::CreateForEachStringDecl( size_t _sStrIndex, size_t _sArrayIdx, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_FOREACHDECL2;
		_ndNode.u.sNodeIndex = _sStrIndex;
		_ndNode.v.sNodeIndex = _sArrayIdx;
		CExpEvalContainer::EE_CUSTOM_VAR cvVar = {
			0,
			CExpEvalContainer::DefaultResult(),
		};
		//m_mCustomVariables.insert_or_assign( _sStrIndex, cvVar );
		if ( m_mCustomVariables.find( _sStrIndex ) == m_mCustomVariables.end() ) {
			m_mCustomVariables[_sStrIndex] = cvVar;
		}
		AddNode( _ndNode );
	}

	// Creates a foreach loop.
	void CExpEvalContainer::CreateForEachLoop( const YYSTYPE::EE_NODE_DATA &_ndDecl, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_FOREACH;
		_ndNode.u.sNodeIndex = _ndDecl.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndStatements.sNodeIndex;

		AddNode( _ndNode );
	}

	// Creates a foreach loop that runs over objects.
	void CExpEvalContainer::CreateForEachObjLoop( const YYSTYPE::EE_NODE_DATA &_ndDecl, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_FOREACHOBJ;
		_ndNode.u.sNodeIndex = _ndDecl.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndStatements.sNodeIndex;

		AddNode( _ndNode );
	}

	// Creates a foreach loop that runs over strings.
	void CExpEvalContainer::CreateForEachStrLoop( const YYSTYPE::EE_NODE_DATA &_ndDecl, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_FOREACHSTR;
		_ndNode.u.sNodeIndex = _ndDecl.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndStatements.sNodeIndex;

		AddNode( _ndNode );
	}

	// Creates a selection.
	void CExpEvalContainer::CreateSelectionStatement( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_IF;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndStatements.sNodeIndex;
		_ndNode.w.sNodeIndex = size_t( ~0 );
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp;
		if ( IsConst( m_vNodes[_ndExp.sNodeIndex], rExp ) ) {
			switch ( rExp.ncType ) {
				case EE_NC_UNSIGNED : {
					if ( rExp.u.ui64Val ) {
						_ndNode = _ndStatements;
					}
					else {
						_ndNode.nType = EE_N_NUMERICCONSTANT;
						_ndNode.u.ui64Val = 0;
						_ndNode.v.ncConstType = EE_NC_UNSIGNED;
					}
					break;
				}
				case EE_NC_SIGNED : {
					if ( rExp.u.i64Val ) {
						_ndNode = _ndStatements;
					}
					else {
						_ndNode.nType = EE_N_NUMERICCONSTANT;
						_ndNode.u.ui64Val = 0;
						_ndNode.v.ncConstType = EE_NC_UNSIGNED;
					}
					break;
				}
				case EE_NC_FLOATING : {
					if ( rExp.u.dVal ) {
						_ndNode = _ndStatements;
					}
					else {
						_ndNode.nType = EE_N_NUMERICCONSTANT;
						_ndNode.u.ui64Val = 0;
						_ndNode.v.ncConstType = EE_NC_UNSIGNED;
					}
					break;
				}
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		AddNode( _ndNode );
	}

	// Creates a selection.
	void CExpEvalContainer::CreateSelectionStatement( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements0, const YYSTYPE::EE_NODE_DATA &_ndStatements1, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_IF;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndStatements0.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndStatements1.sNodeIndex;
#ifdef EE_OPTIMIZE_FOR_RUNTIME
		EE_RESULT rExp;
		if ( IsConst( m_vNodes[_ndExp.sNodeIndex], rExp ) ) {
			switch ( rExp.ncType ) {
				case EE_NC_UNSIGNED : {
					_ndNode = rExp.u.ui64Val ? _ndStatements0 : _ndStatements1;
					break;
				}
				case EE_NC_SIGNED : {
					_ndNode = rExp.u.i64Val ? _ndStatements0 : _ndStatements1;
					break;
				}
				case EE_NC_FLOATING : {
					_ndNode = rExp.u.dVal ? _ndStatements0 : _ndStatements1;
					break;
				}
			}
		}
#endif	// #ifdef EE_OPTIMIZE_FOR_RUNTIME
		AddNode( _ndNode );
	}

	// Creates a continue.
	void CExpEvalContainer::CreateContinue( YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_CONTINUE;
		AddNode( _ndNode );
	}

	// Creates a break.
	void CExpEvalContainer::CreateBreak( YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_BREAK;
		AddNode( _ndNode );
	}

	// Creates an entry in an argument list.
	void CExpEvalContainer::CreateArgListEntry( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARG_LIST_ENTRY;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		_ndNode.v.sNodeIndex = size_t( ~0 );
		AddNode( _ndNode );
	}

	// Creates an entry in an argument list.
	void CExpEvalContainer::CreateArgListEntry( const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARG_LIST_ENTRY;
		_ndNode.u.sNodeIndex = _ndLeft.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndRight.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates an arg list.
	void CExpEvalContainer::CreateArgList( const YYSTYPE::EE_NODE_DATA &_ndList, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ARG_LIST;
		_ndNode.u.sNodeIndex = _ndList.sNodeIndex;
		AddNode( _ndNode );
	}

	// Starts an array.
	bool CExpEvalContainer::StartArray( /*YYSTYPE::EE_NODE_DATA &_ndNode*/ ) {
		size_t sIdx = m_vObjects.size();
		ee::CObject * psObj = reinterpret_cast<ee::CVector *>(AllocateObject<ee::CVector>());
		if ( !psObj ) { return false; }
		try {
			m_vVectorStack.push_back( sIdx );
		}
		catch ( ... ) {
			DeallocateObject( psObj );
			return false;
		}
		/*_ndNode.nType = EE_N_START_VECTOR;
		_ndNode.u.sNodeIndex = sIdx;
		AddNode( _ndNode );*/
		return true;
	}

	// Ends an array.
	void CExpEvalContainer::EndArray( /*YYSTYPE::EE_NODE_DATA &_ndNode*/ ) {
		//size_t sIdx = m_vVectorStack[m_vVectorStack.size()-1];
		m_vVectorStack.pop_back();
		/*_ndNode.nType = EE_N_END_VECTOR;
		_ndNode.u.sNodeIndex = sIdx;
		AddNode( _ndNode );*/
	}

	// Creates a format string in the format of: "Some string {}.".format( Args0, Arg1 ).
	void CExpEvalContainer::CreateFormat( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndArgs, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_FORMAT;
		_ndNode.u.sStringIndex = _sStrIndex;
		_ndNode.v.sNodeIndex = _ndArgs.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.add().
	void CExpEvalContainer::CreateVectorAdd( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndOperand, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_ADD;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndOperand.sNodeIndex;
		_ndNode.w.sNodeIndex = m_vObjects.size();
		AllocateObject<ee::CVector>();
		AddNode( _ndNode );
	}

	// Creates a vector.sub().
	void CExpEvalContainer::CreateVectorSub( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndOperand, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_SUB;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndOperand.sNodeIndex;
		_ndNode.w.sNodeIndex = m_vObjects.size();
		AllocateObject<ee::CVector>();
		AddNode( _ndNode );
	}

	// Creates a vector.mul().
	void CExpEvalContainer::CreateVectorMul( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndOperand, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_MUL;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndOperand.sNodeIndex;
		_ndNode.w.sNodeIndex = m_vObjects.size();
		AllocateObject<ee::CVector>();
		AddNode( _ndNode );
	}

	// Creates a vector.dot().
	void CExpEvalContainer::CreateVectorDot( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndOperand, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_DOT;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndOperand.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.append().
	void CExpEvalContainer::CreateVectorAppend( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_APPEND;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndVal.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.append().
	void CExpEvalContainer::CreateVectorAppend( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_APPEND_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		_ndNode.v.sNodeIndex = _ndVal.sNodeIndex;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.assign().
	void CExpEvalContainer::CreateVectorAssign( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndIdx, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_ASSIGN;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndVal.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndIdx.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.at().
	void CExpEvalContainer::CreateVectorAt( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndIdx, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_AT;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndIdx.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.at().
	void CExpEvalContainer::CreateVectorAt( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndIdx, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_AT_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		_ndNode.v.sNodeIndex = _ndIdx.sNodeIndex;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.capacity().
	void CExpEvalContainer::CreateVectorCapacity( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_CAPACITY;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.capacity().
	void CExpEvalContainer::CreateVectorCapacity( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_CAPACITY_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.clear().
	void CExpEvalContainer::CreateVectorClear( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_CLEAR;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.clear().
	void CExpEvalContainer::CreateVectorClear( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_CLEAR_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.cross().
	void CExpEvalContainer::CreateVectorCross( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndOperand, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_CROSS;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndOperand.sNodeIndex;
		_ndNode.w.sNodeIndex = m_vObjects.size();
		AllocateObject<ee::CVector>();
		AddNode( _ndNode );
	}

	// Creates a vector.empty().
	void CExpEvalContainer::CreateVectorEmpty( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_EMPTY;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.empty().
	void CExpEvalContainer::CreateVectorEmpty( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_EMPTY_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.erase().
	void CExpEvalContainer::CreateVectorErase( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndIdx, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_ERASE;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndIdx.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.erase().
	void CExpEvalContainer::CreateVectorErase( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndIdx, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_ERASE_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		_ndNode.v.sNodeIndex = _ndIdx.sNodeIndex;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.insert().
	void CExpEvalContainer::CreateVectorInsert( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndIdx, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_INSERT;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndVal.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndIdx.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.max_size().
	void CExpEvalContainer::CreateVectorMaxSize( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_MAX_SIZE;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.max_size().
	void CExpEvalContainer::CreateVectorMaxSize( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_MAX_SIZE_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.mag().
	void CExpEvalContainer::CreateVectorMag( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_MAG;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.mag().
	void CExpEvalContainer::CreateVectorMag( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_MAG_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.mag_sq().
	void CExpEvalContainer::CreateVectorMagSq( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_MAG_SQ;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.mag_sq().
	void CExpEvalContainer::CreateVectorMagSq( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_MAG_SQ_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.normalize().
	void CExpEvalContainer::CreateVectorNormalize( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_NORMALIZE;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.w.sNodeIndex = m_vObjects.size();
		AllocateObject<ee::CVector>();
		AddNode( _ndNode );
	}

	// Creates a vector.normalize().
	void CExpEvalContainer::CreateVectorNormalize( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_NORMALIZE_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		_ndNode.w.sNodeIndex = m_vObjects.size();
		AllocateObject<ee::CVector>();
		AddNode( _ndNode );
	}

	// Creates a vector.reserve().
	void CExpEvalContainer::CreateVectorReserve( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndSize, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_RESERVE;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndSize.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.reserve().
	void CExpEvalContainer::CreateVectorReserve( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndSize, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_RESERVE_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		_ndNode.v.sNodeIndex = _ndSize.sNodeIndex;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.resize().
	void CExpEvalContainer::CreateVectorResize( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndSize, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_RESIZE;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndSize.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.resize().
	void CExpEvalContainer::CreateVectorResize( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndSize, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_RESIZE_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		_ndNode.v.sNodeIndex = _ndSize.sNodeIndex;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.pop_back().
	void CExpEvalContainer::CreateVectorPopBack( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_POP_BACK;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.pop_back().
	void CExpEvalContainer::CreateVectorPopBack( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_POP_BACK_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.push_back().
	void CExpEvalContainer::CreateVectorPushBack( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_APPEND;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndVal.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.push_back().
	void CExpEvalContainer::CreateVectorPushBack( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_APPEND_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		_ndNode.v.sNodeIndex = _ndVal.sNodeIndex;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.shrink_to_fit().
	void CExpEvalContainer::CreateVectorShrinkToFit( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_SHRINK_TO_FIT;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.shrink_to_fit().
	void CExpEvalContainer::CreateVectorShrinkToFit( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_SHRINK_TO_FIT_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.size().
	void CExpEvalContainer::CreateVectorSize( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_SIZE;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.size().
	void CExpEvalContainer::CreateVectorSize( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_SIZE_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a vector.sum().
	void CExpEvalContainer::CreateVectorSum( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_SUM;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.swap().
	void CExpEvalContainer::CreateVectorSwap( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_SWAP;
		_ndNode.u.sNodeIndex = _ndVector.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndRight.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a vector.swap().
	void CExpEvalContainer::CreateVectorSwap( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_VECTOR_SWAP_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		_ndNode.v.sNodeIndex = _ndRight.sNodeIndex;
		auto aTmp = m_mCustomVariables.find( _sVarId );
		if ( aTmp == m_mCustomVariables.end() ) { throw EE_EC_INVALID_OBJECT; }
		AddNode( _ndNode );
	}

	// Creates a string tokenization.
	void CExpEvalContainer::CreateStringTokenize( const YYSTYPE::EE_NODE_DATA &_ndString, const YYSTYPE::EE_NODE_DATA &_ndTokenizer, const YYSTYPE::EE_NODE_DATA &_ndIncludeEmpty, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_STRING_TOKENIZE;
		_ndNode.u.sNodeIndex = _ndString.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndTokenizer.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndIncludeEmpty.sNodeIndex;
		_ndNode.x.sNodeIndex = m_vObjects.size();
		AllocateObject<ee::CVector>();
		AddNode( _ndNode );
	}

	// Creates a string tokenization.
	void CExpEvalContainer::CreateStringTokenize( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndTokenizer, const YYSTYPE::EE_NODE_DATA &_ndIncludeEmpty, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_STRING_TOKENIZE_IDENT;
		_ndNode.u.sNodeIndex = _sVarId;
		_ndNode.v.sNodeIndex = _ndTokenizer.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndIncludeEmpty.sNodeIndex;
		_ndNode.x.sNodeIndex = m_vObjects.size();
		AllocateObject<ee::CVector>();
		AddNode( _ndNode );
	}

	// Sets the translation-unit node.
	void CExpEvalContainer::SetTrans( YYSTYPE::EE_NODE_DATA &_ndNode ) {
		m_sTrans = _ndNode.sNodeIndex;
	}

	// Determines if a node is constant and, if so, returns the constant value associated with the node.
	bool CExpEvalContainer::IsConst( const YYSTYPE::EE_NODE_DATA &_ndExp, EE_RESULT &_rResult ) {
		switch ( _ndExp.nType ) {
			case EE_N_NUMERICCONSTANT : {
				_rResult.u.ui64Val = _ndExp.u.ui64Val;
				_rResult.ncType = _ndExp.v.ncConstType;
				return true;
			}
			case EE_N_CUSTOM_VAR : {
				if ( _ndExp.v.bIsConst ) {
					auto aFind = m_mCustomVariables.find( _ndExp.u.sStringIndex );
					if ( aFind == m_mCustomVariables.end() ) { return false; }
					_rResult = aFind->second.rRes;
					return true;
				}
				break;
			}
		}
		return false;
	}

	// Are we breaking from a loop?
	bool CExpEvalContainer::Breaking() const {
		if ( !m_vLoopStack.size() ) { return false; }
		return m_vLoopStack[m_vLoopStack.size()-1].bBreak;
	}

	// Break.  Returns false if not in a loop.
	bool CExpEvalContainer::Break() {
		if ( !m_vLoopStack.size() ) { return false; }
		m_vLoopStack[m_vLoopStack.size()-1].bBreak = true;
		return true;
	}

	// Are we continuing in a loop?
	bool CExpEvalContainer::Continuing() const {
		if ( !m_vLoopStack.size() ) { return false; }
		return m_vLoopStack[m_vLoopStack.size()-1].bContinue;
	}

	// Continue.  Returns false if not in a loop.
	bool CExpEvalContainer::Continue() {
		if ( !m_vLoopStack.size() ) { return false; }
		m_vLoopStack[m_vLoopStack.size()-1].bContinue = true;
		return true;
	}

	// Adds a node to its internal node tree.
	void CExpEvalContainer::AddNode( YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.sNodeIndex = m_vNodes.size();
		// Since the function returns void, try/catch here makes no sense.
		m_vNodes.push_back( _ndNode );
	}

	// Adds a string and returns its index into the stack.
	size_t CExpEvalContainer::AddString( const std::string &_sText ) {
		for ( size_t I = 0; I < m_vStrings.size(); ++I ) {
			if ( m_vStrings[I] == _sText ) { return I; }
		}
		m_vStrings.push_back( _sText );
		return m_vStrings.size() - 1;
	}

	// Resolves a node.
	bool CExpEvalContainer::ResolveNode( size_t _sNode, EE_RESULT &_rRes ) {
		if ( _sNode >= m_vNodes.size() ) { return false; }
		YYSTYPE::EE_NODE_DATA & _ndExp = m_vNodes[_sNode];

		switch ( _ndExp.nType ) {
			case EE_N_NUMERICCONSTANT : {
				_rRes.ncType = _ndExp.v.ncConstType;
				_rRes.u.ui64Val = _ndExp.u.ui64Val;
				/*switch ( _ndExp.v.ncConstType ) {
					case EE_NC_UNSIGNED : {
						_rRes.u.ui64Val = _ndExp.u.ui64Val;
						break;
					}
					case EE_NC_SIGNED : {
						_rRes.u.i64Val = _ndExp.u.i64Val;
						break;
					}
					case EE_NC_FLOATING : {
						_rRes.u.dVal = _ndExp.u.dVal;
						break;
					}
				}*/
				return true;
			}
			case EE_N_STRING : {
				if ( !m_pfshString ) { return false; }
				return m_pfshString( m_vStrings[_ndExp.u.sStringIndex], m_uiptrStringData, this, _rRes );
			}
			case EE_N_CUSTOM_VAR : {
				auto aFind = m_mCustomVariables.find( _ndExp.u.sStringIndex );
				if ( aFind == m_mCustomVariables.end() ) { return false; }
				_rRes = aFind->second.rRes;
				return _rRes.ncType != EE_NC_INVALID;
			}
			case EE_N_ARRAY : {
				if ( !m_vArrayData[_ndExp.u.sStringIndex].m_pabBase ) { return false; }
				EE_RESULT rNode;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rNode ) ) { return false; }
				rNode = ConvertResultOrObject( rNode, EE_NC_UNSIGNED );
				if ( rNode.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				return m_vArrayData[_ndExp.u.sStringIndex].m_pabBase->ReadValue( static_cast<size_t>(rNode.u.ui64Val), _rRes );
			}
			case EE_N_STRINGARRAY : {
				EE_RESULT rNode;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rNode ) ) { return false; }
				rNode = ConvertResultOrObject( rNode, EE_NC_UNSIGNED );
				if ( rNode.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = ee::CExpEval::GetUtf8CodePointByIdx( m_vStrings[_ndExp.u.sStringIndex], static_cast<size_t>(rNode.u.ui64Val) );
				return true;
			}
			case EE_N_ARRAY_ACCESS : {
				EE_RESULT rBase;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rBase ) ) { return false; }
				if ( rBase.ncType != EE_NC_OBJECT || !rBase.u.poObj ) { _rRes.ncType = EE_NC_INVALID; return false; }

				EE_RESULT rIdx;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rIdx ) ) { return false; }
				if ( rIdx.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				rIdx = ConvertResultOrObject( rIdx, EE_NC_SIGNED );
				if ( rIdx.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				_rRes = rBase.u.poObj->ArrayAccess( rIdx.u.i64Val );
				if ( _rRes.ncType == EE_NC_INVALID ) { return false; }
				return true;
			}
			case EE_N_ADDRESS : {
				if ( !m_pfahAddressHandler ) { return false; }
				EE_RESULT rLeft;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rLeft ) ) { return false; }
				EE_RESULT rAddr = ConvertResultOrObject( rLeft, EE_NC_UNSIGNED );
				if ( rAddr.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				return m_pfahAddressHandler( rAddr.u.ui64Val, _ndExp.v.ctCast, m_uiptrAddressData, this, _rRes );
			}
			case EE_N_MEMBERACCESS : {
				if ( !m_pfmahMemberAccess ) { return false; }
				EE_RESULT rLeft;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rLeft ) ) { return false; }
				return m_pfmahMemberAccess( rLeft, m_vStrings[_ndExp.v.sStringIndex], m_uiptrMemberAccess, this, _rRes );
			}
			case EE_N_USER_VAR : {
				if ( !m_pfUser ) { return false; }
				return m_pfUser( m_uiptrUserData, this, _rRes );
			}
			case EE_N_NUMBERED_PARM : {
				_rRes = GetNumberedParm( static_cast<size_t>(_ndExp.u.ui64Val) );
				return true;
			}
			case EE_N_DYNAMIC_NUMBERED_PARM : {
				auto aFind = m_mCustomVariables.find( _ndExp.u.sStringIndex );
				if ( aFind == m_mCustomVariables.end() ) { return false; }
				EE_RESULT rTemp = ConvertResultOrObject( aFind->second.rRes, EE_NC_UNSIGNED );
				if ( rTemp.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes = GetNumberedParm( static_cast<size_t>(rTemp.u.ui64Val) );
				return true;
			}
			case EE_N_DYNAMIC_NUMBERED_PARM_EXP : {
				EE_RESULT rTemp;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }
				rTemp = ConvertResultOrObject( rTemp, EE_NC_UNSIGNED );
				if ( rTemp.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes = GetNumberedParm( static_cast<size_t>(rTemp.u.ui64Val) );
				return true;
			}
			case EE_N_NUMBERED_PARM_TOTAL : {
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = m_vNumberedParms.size();
				return true;
			}
			case EE_N_UNARY : {
				EE_RESULT rTemp;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }
				return PerformUnary( rTemp, _ndExp.v.ui32Op, _rRes ) == EE_EC_SUCCESS;
			}
			case EE_N_CAST : {
				EE_RESULT rNode;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rNode ) ) { return false; }

				switch ( _ndExp.v.ctCast ) {
#define EE_CAST( TYPE, CASTTYPE, MEMBER )																\
	_rRes.ncType = TYPE;																				\
	_rRes.u.MEMBER = static_cast<CASTTYPE>(ConvertResultOrObject( rNode, _rRes.ncType ).u.MEMBER);		\
	if ( _rRes.ncType == EE_NC_INVALID ) { return false; }									\
	return true;
					case EE_CT_INT8 : { EE_CAST( EE_NC_SIGNED, int8_t, i64Val ) }
					case EE_CT_INT16 : { EE_CAST( EE_NC_SIGNED, int16_t, i64Val ) }
					case EE_CT_INT32 : { EE_CAST( EE_NC_SIGNED, int32_t, i64Val ) }
					case EE_CT_INT64 : { EE_CAST( EE_NC_SIGNED, int64_t, i64Val ) }
					case EE_CT_UINT8 : { EE_CAST( EE_NC_UNSIGNED, uint8_t, ui64Val ) }
					case EE_CT_UINT16 : { EE_CAST( EE_NC_UNSIGNED, uint16_t, ui64Val ) }
					case EE_CT_UINT32 : { EE_CAST( EE_NC_UNSIGNED, uint32_t, ui64Val ) }
					case EE_CT_UINT64 : { EE_CAST( EE_NC_UNSIGNED, uint64_t, ui64Val ) }
					case EE_CT_FLOAT : { EE_CAST( EE_NC_FLOATING, float, dVal ) }
					case EE_CT_DOUBLE : { EE_CAST( EE_NC_FLOATING, double, dVal ) }
#undef EE_CAST
					case EE_CT_FLOAT10 : {
						rNode = ConvertResultOrObject( rNode, EE_NC_FLOATING );
						if ( rNode.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
						CFloatX fTemp;
						fTemp.CreateFromDouble( rNode.u.dVal, 5, 6, true, false );
						_rRes.ncType = EE_NC_FLOATING;
						_rRes.u.dVal = fTemp.AsDouble();
						return true;
					}
					case EE_CT_FLOAT11 : {
						rNode = ConvertResultOrObject( rNode, EE_NC_FLOATING );
						if ( rNode.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
						CFloatX fTemp;
						fTemp.CreateFromDouble( rNode.u.dVal, 5, 7, true, false );
						_rRes.ncType = EE_NC_FLOATING;
						_rRes.u.dVal = fTemp.AsDouble();
						return true;
					}
					case EE_CT_FLOAT14 : {
						rNode = ConvertResultOrObject( rNode, EE_NC_FLOATING );
						if ( rNode.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
						CFloatX fTemp;
						fTemp.CreateFromDouble( rNode.u.dVal, 5, 10, true, false );
						_rRes.ncType = EE_NC_FLOATING;
						_rRes.u.dVal = fTemp.AsDouble();
						return true;
					}
					case EE_CT_FLOAT16 : {
						rNode = ConvertResultOrObject( rNode, EE_NC_FLOATING );
						if ( rNode.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
						CFloatX fTemp;
						fTemp.CreateFromDouble( rNode.u.dVal, 5, 11, true, true );
						_rRes.ncType = EE_NC_FLOATING;
						_rRes.u.dVal = fTemp.AsDouble();
						return true;
					}
				}
				return false;
			}
			case EE_N_OP : {
				EE_RESULT rLeft, rRight;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rLeft ) ) { return false; }
				// Short-circuit evaluation is not only expected by programmers, it will improve performance in loops.
				if ( _ndExp.v.ui32Op == CExpEvalParser::token::EE_AND ) {
					if ( ((rLeft.ncType) == EE_NC_UNSIGNED && !rLeft.u.ui64Val) ||
						((rLeft.ncType) == EE_NC_SIGNED && !rLeft.u.i64Val) ||
						((rLeft.ncType) == EE_NC_FLOATING && !rLeft.u.dVal) ) {
						_rRes.ncType = EE_NC_UNSIGNED;
						_rRes.u.ui64Val = false;
						return true;
					}
				}
				else if ( _ndExp.v.ui32Op == CExpEvalParser::token::EE_OR ) {
					if ( ((rLeft.ncType) == EE_NC_UNSIGNED && rLeft.u.ui64Val) ||
						((rLeft.ncType) == EE_NC_SIGNED && rLeft.u.i64Val) ||
						((rLeft.ncType) == EE_NC_FLOATING && rLeft.u.dVal) ) {
						_rRes.ncType = EE_NC_UNSIGNED;
						_rRes.u.ui64Val = true;
						return true;
					}
				}
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rRight ) ) { return false; }
				_rRes.ncType = GetCastType( rLeft.ncType, rRight.ncType );
				rLeft = ConvertResultOrObject( rLeft, _rRes.ncType );
				if ( rLeft.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				rRight = ConvertResultOrObject( rRight, _rRes.ncType );
				if ( rRight.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
#define EE_OP( MEMBER, CASE, OP )								\
	case CASE : {												\
		_rRes.u.MEMBER = rLeft.u.MEMBER OP rRight.u.MEMBER;		\
		return true;											\
	}
#define EE_OP_NO_ZERO( MEMBER, CASE, OP )						\
	case CASE : {												\
		if ( rRight.u.MEMBER ) {								\
			_rRes.u.MEMBER = rLeft.u.MEMBER OP rRight.u.MEMBER;	\
		}														\
		else { _rRes.u.MEMBER = 0; }							\
		return true;											\
	}
#define EE_OP_BOOL( MEMBER, CASE, OP )							\
	case CASE : {												\
		_rRes.ncType = EE_NC_UNSIGNED;							\
		_rRes.u.ui64Val = rLeft.u.MEMBER OP rRight.u.MEMBER;	\
		return true;											\
	}
#define EE_INT_CHECK( CASE, MEMBER )										\
	case CASE : {															\
		switch ( _ndExp.v.ui32Op ) {										\
			EE_OP( MEMBER, '*', * )											\
			EE_OP_NO_ZERO( MEMBER, '/', / )									\
			EE_OP_NO_ZERO( MEMBER, '%', % )									\
			EE_OP( MEMBER, '+', + )											\
			EE_OP( MEMBER, '-', - )											\
			EE_OP( MEMBER, CExpEvalParser::token::EE_RIGHT_OP, >> )			\
			EE_OP( MEMBER, CExpEvalParser::token::EE_LEFT_OP, << )			\
			EE_OP( MEMBER, '&', & )											\
			EE_OP( MEMBER, '^', ^ )											\
			EE_OP( MEMBER, '|', | )											\
			EE_OP_BOOL( MEMBER, '<', < )									\
			EE_OP_BOOL( MEMBER, '>', > )									\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_REL_LE, <= )		\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_REL_GE, >= )		\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_EQU_E, == )		\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_EQU_NE, != )		\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_AND, && )			\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_OR, || )			\
			default : { return false; }										\
		}																	\
	}

				switch ( _rRes.ncType ) {
					EE_INT_CHECK( EE_NC_SIGNED, i64Val )
					EE_INT_CHECK( EE_NC_UNSIGNED, ui64Val )
					case EE_NC_FLOATING : {
						switch ( _ndExp.v.ui32Op ) {
							EE_OP( dVal, '*', * )
							EE_OP( dVal, '/', / )
							EE_OP( dVal, '+', + )
							EE_OP( dVal, '-', - )
							EE_OP_BOOL( dVal, '<', < )
							EE_OP_BOOL( dVal, '>', > )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_REL_LE, <= )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_REL_GE, >= )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_EQU_E, == )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_EQU_NE, != )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_AND, && )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_OR, || )
							case CExpEvalParser::token::EE_RIGHT_OP : {
								_rRes.u.dVal = RShift( rLeft.u.dVal, rRight.u.dVal );
								return true;
							}
							case CExpEvalParser::token::EE_LEFT_OP : {
								_rRes.u.dVal = LShift( rLeft.u.dVal, rRight.u.dVal );
								return true;
							}
							case '%' : {
								_rRes.u.dVal = std::fmod( rLeft.u.dVal, rRight.u.dVal );
								return true;
							}
							case '^' : {
								_rRes.u.dVal = std::pow( rLeft.u.dVal, rRight.u.dVal );
								return true;
							}
							default : { return false; }
						}
					}
				}

#undef EE_INT_CHECK
#undef EE_OP_BOOL
#undef EE_OP_NO_ZERO
#undef EE_OP
				return false;
			}
			case EE_N_CONDITIONAL : {
				EE_RESULT rExp;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rExp ) ) { return false; }
				bool bTrue;
				if ( (rExp.ncType) == EE_NC_SIGNED ) {
					bTrue = rExp.u.i64Val != 0;
				}
				else if ( (rExp.ncType) == EE_NC_UNSIGNED ) {
					bTrue = rExp.u.ui64Val != 0;
				}
				else if ( (rExp.ncType) == EE_NC_FLOATING ) {
					bTrue = rExp.u.dVal != 0.0;
				}
				else { return false; }	// Can't happen.

				// Slight variation from C/C++.  In compiled code, the left and right nodes have to be the same type, so if one of them
				//	was double, the other would be cast to double (because the ?: expression must be of a single specific type).
				// Here, the return type is whatever the left or right node's return type is.  Lazy evaluation to save time.
				return bTrue ? ResolveNode( _ndExp.v.sNodeIndex, _rRes ) : ResolveNode( _ndExp.w.sNodeIndex, _rRes );
			}
			case EE_N_INTRINSIC_0 : {
				switch ( _ndExp.u.ui32Intrinsic ) {
					case CExpEvalParser::token::EE_CLOCK : {
						_rRes.ncType = EE_NC_SIGNED;
						_rRes.u.i64Val = ::clock();
						return true;
					}
					case CExpEvalParser::token::EE_SECONDS : {
						_rRes.ncType = EE_NC_UNSIGNED;
						_rRes.u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() ) / 1000000ULL;
						return true;
					}
					case CExpEvalParser::token::EE_MILLISECONDS : {
						_rRes.ncType = EE_NC_UNSIGNED;
						_rRes.u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() ) / 1000ULL;
						return true;
					}
					case CExpEvalParser::token::EE_MICROSECONDS : {
						_rRes.ncType = EE_NC_UNSIGNED;
						_rRes.u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() );
						return true;
					}
					case CExpEvalParser::token::EE_SECONDS_SINCE_START : {
						_rRes.ncType = EE_NC_UNSIGNED;
						_rRes.u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() - ee::CExpEval::StartTime() ) / 1000000ULL;
						return true;
					}
					case CExpEvalParser::token::EE_MILLISECONDS_SINCE_START : {
						_rRes.ncType = EE_NC_UNSIGNED;
						_rRes.u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() - ee::CExpEval::StartTime() ) / 1000ULL;
						return true;
					}
					case CExpEvalParser::token::EE_MICROSECONDS_SINCE_START : {
						_rRes.ncType = EE_NC_UNSIGNED;
						_rRes.u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() - ee::CExpEval::StartTime() );
						return true;
					}
				}
				_rRes.ncType = EE_NC_INVALID;
				return false;
			}
			case EE_N_INTRINSIC_0_SIGNED : {
				_rRes.ncType = EE_NC_SIGNED;
				_rRes.u.i64Val = _ndExp.uFuncPtr.pfClock();
				return true;
			}
			case EE_N_INTRINSIC_0_UNSIGNED_DIVISOR : {
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.i64Val = _ndExp.uFuncPtr.pfIntrins0Unsigned( ee::CExpEval::Time() ) / _ndExp.u.ui64Val;
				return true;
			}
			case EE_N_INTRINSIC_1 : {
				EE_RESULT rExp;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp ) ) { return false; }
				EE_ERROR_CODES ecError = PerformIntrinsic( rExp, _ndExp.u.ui32Intrinsic, _rRes );
				if ( ecError != EE_EC_SUCCESS ) { return false; }
				return true;
			}
			case EE_N_INTRINSIC_1_FLOAT_FLOAT : {
				EE_RESULT rExp;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp ) ) { return false; }
				
				EE_RESULT eTmp = ConvertResultOrObject( rExp, EE_NC_FLOATING );
				if ( eTmp.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = _ndExp.uFuncPtr.pfIntrins1Float_Float( eTmp.u.dVal );
				return true;
			}
			case EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED16 : {
				EE_RESULT rExp;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp ) ) { return false; }
				
				EE_RESULT eTmp = ConvertResultOrObject( rExp, EE_NC_UNSIGNED );
				if ( eTmp.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Unsigned_Unsigned16( static_cast<uint16_t>(eTmp.u.ui64Val) );
				return true;
			}
			case EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED32 : {
				EE_RESULT rExp;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp ) ) { return false; }
				
				EE_RESULT eTmp = ConvertResultOrObject( rExp, EE_NC_UNSIGNED );
				if ( eTmp.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Unsigned_Unsigned32( static_cast<uint32_t>(eTmp.u.ui64Val) );
				return true;
			}
			case EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED64 : {
				EE_RESULT rExp;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp ) ) { return false; }
				
				EE_RESULT eTmp = ConvertResultOrObject( rExp, EE_NC_UNSIGNED );
				if ( eTmp.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Unsigned_Unsigned64( eTmp.u.ui64Val );
				return true;
			}
			case EE_N_INTRINSIC_1_BOOL_FLOAT : {
				EE_RESULT rExp;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp ) ) { return false; }
				
				EE_RESULT eTmp = ConvertResultOrObject( rExp, EE_NC_FLOATING );
				if ( eTmp.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Bool_Float( eTmp.u.dVal );
				return true;
			}
			case EE_N_INTRINSIC_1_INT_FLOAT : {
				EE_RESULT rExp;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp ) ) { return false; }
				
				EE_RESULT eTmp = ConvertResultOrObject( rExp, EE_NC_FLOATING );
				if ( eTmp.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_SIGNED;
				_rRes.u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Signed_Float( eTmp.u.dVal );
				return true;
			}
			case EE_N_INTRINSIC_2 : {
				EE_RESULT rExp0, rExp1;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp0 ) ) { return false; }
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rExp1 ) ) { return false; }
				EE_ERROR_CODES ecError = PerformIntrinsic( rExp0, rExp1, _ndExp.u.ui32Intrinsic, _rRes, true );
				if ( ecError != EE_EC_SUCCESS ) { return false; }
				return true;
			}
			case EE_N_INTRINSIC_2_FLOAT_FLOAT_FLOAT : {
				EE_RESULT rExp0, rExp1;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp0 ) ) { return false; }
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rExp1 ) ) { return false; }
				
				EE_RESULT eTmp0 = ConvertResultOrObject( rExp0, EE_NC_FLOATING );
				if ( eTmp0.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				EE_RESULT eTmp1 = ConvertResultOrObject( rExp1, EE_NC_FLOATING );
				if ( eTmp1.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = _ndExp.uFuncPtr.pfIntrins2Float_Float_Float( eTmp0.u.dVal,
					eTmp1.u.dVal );
				return true;
			}
			case EE_N_INTRINSIC_2_FLOAT_FLOAT32_FLOAT32 : {
				EE_RESULT rExp0, rExp1;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp0 ) ) { return false; }
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rExp1 ) ) { return false; }
				
				EE_RESULT eTmp0 = ConvertResultOrObject( rExp0, EE_NC_FLOATING );
				if ( eTmp0.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				EE_RESULT eTmp1 = ConvertResultOrObject( rExp1, EE_NC_FLOATING );
				if ( eTmp1.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = _ndExp.uFuncPtr.pfIntrins2Float32_Float32_Float32( static_cast<float>(eTmp0.u.dVal),
					static_cast<float>(eTmp1.u.dVal) );
				return true;
			}
			case EE_N_INTRINSIC_2_FLOAT_FLOAT80_FLOAT80 : {
				EE_RESULT rExp0, rExp1;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp0 ) ) { return false; }
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rExp1 ) ) { return false; }

				EE_RESULT eTmp0 = ConvertResultOrObject( rExp0, EE_NC_FLOATING );
				if ( eTmp0.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				EE_RESULT eTmp1 = ConvertResultOrObject( rExp1, EE_NC_FLOATING );
				if ( eTmp1.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = double( _ndExp.uFuncPtr.pfIntrins2Float80_Float80_Float80( eTmp0.u.dVal,
					eTmp1.u.dVal ) );
				return true;
			}
			case EE_N_INTRINSIC_2_FLOAT_FLOAT32_FLOAT80 : {
				EE_RESULT rExp0, rExp1;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp0 ) ) { return false; }
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rExp1 ) ) { return false; }

				EE_RESULT eTmp0 = ConvertResultOrObject( rExp0, EE_NC_FLOATING );
				if ( eTmp0.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				EE_RESULT eTmp1 = ConvertResultOrObject( rExp1, EE_NC_FLOATING );
				if ( eTmp1.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = _ndExp.uFuncPtr.pfIntrins2Float32_Float32_Float80( static_cast<float>(eTmp0.u.dVal),
					eTmp1.u.dVal );
				return true;
			}
			case EE_N_INTRINSIC_3 : {
				EE_RESULT rExp0, rExp1, rExp2;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp0 ) ) { return false; }
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rExp1 ) ) { return false; }
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rExp2 ) ) { return false; }
				EE_ERROR_CODES ecError = PerformIntrinsic( rExp0, rExp1, rExp2, _ndExp.u.ui32Intrinsic, _rRes );
				if ( ecError != EE_EC_SUCCESS ) { return false; }
				return true;
			}
			case EE_N_INTRINSIC_3_FLOAT_FLOAT_FLOAT_FLOAT : {
				EE_RESULT rExp0, rExp1, rExp2;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rExp0 ) ) { return false; }
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rExp1 ) ) { return false; }
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rExp2 ) ) { return false; }

				EE_RESULT eTmp0 = ConvertResultOrObject( rExp0, EE_NC_FLOATING );
				if ( eTmp0.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				EE_RESULT eTmp1 = ConvertResultOrObject( rExp1, EE_NC_FLOATING );
				if ( eTmp1.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				EE_RESULT eTmp2 = ConvertResultOrObject( rExp2, EE_NC_FLOATING );
				if ( eTmp2.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = _ndExp.uFuncPtr.pfIntrins3Float_Float_Float_Float( rExp0.u.dVal,
					rExp1.u.dVal,
					rExp2.u.dVal );
				return true;
			}
			case EE_N_ASFLOAT : {
				EE_RESULT rTemp;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }
				EE_ERROR_CODES ecError = PerformAsFloat( rTemp, _rRes );
				if ( ecError != EE_EC_SUCCESS ) { return false; }
				return true;
			}
			case EE_N_ASDOUBLE : {
				EE_RESULT rTemp;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }
				EE_ERROR_CODES ecError = PerformAsDouble( rTemp, _rRes );
				if ( ecError != EE_EC_SUCCESS ) { return false; }
				return true;
			}
			case EE_N_ASXFLOAT : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }
				EE_RESULT rTempSignVal;
				if ( !ResolveNode( _ndExp.y.sNodeIndex, rTempSignVal ) ) { return false; }
				EE_RESULT rTempExpVal;
				if ( !ResolveNode( _ndExp.z.sNodeIndex, rTempExpVal ) ) { return false; }
				EE_RESULT rTempManVal;
				if ( !ResolveNode( _ndExp.a.sNodeIndex, rTempManVal ) ) { return false; }

				EE_ERROR_CODES ecError = PerformFloatX( rTempSignBits, rTempExpBits, rTempManBits, rTempImplied,
					rTempSignVal, rTempExpVal, rTempManVal, _rRes );
				if ( ecError != EE_EC_SUCCESS ) { return false; }
				return true;
			}
			case EE_N_ASXFLOAT_FROM_DOUBLE : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }
				EE_RESULT rTempDoubleVal;
				if ( !ResolveNode( _ndExp.y.sNodeIndex, rTempDoubleVal ) ) { return false; }

				EE_ERROR_CODES ecError = PerformFloatX( rTempSignBits, rTempExpBits, rTempManBits, rTempImplied,
					rTempDoubleVal, _rRes );
				if ( ecError != EE_EC_SUCCESS ) { return false; }
				return true;
			}
			case EE_N_ASFLOAT16 : {
				EE_RESULT rTemp;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }
				EE_ERROR_CODES ecError = PerformToFloat16( rTemp, _rRes );
				if ( ecError != EE_EC_SUCCESS ) { return false; }
				return true;
			}
			case EE_N_ASFLOAT14 : {
				EE_RESULT rTemp;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }
				EE_ERROR_CODES ecError = PerformToFloat14( rTemp, _rRes );
				if ( ecError != EE_EC_SUCCESS ) { return false; }
				return true;
			}
			case EE_N_ASFLOAT11 : {
				EE_RESULT rTemp;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }
				EE_ERROR_CODES ecError = PerformToFloat11( rTemp, _rRes );
				if ( ecError != EE_EC_SUCCESS ) { return false; }
				return true;
			}
			case EE_N_ASFLOAT10 : {
				EE_RESULT rTemp;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }
				EE_ERROR_CODES ecError = PerformToFloat10( rTemp, _rRes );
				if ( ecError != EE_EC_SUCCESS ) { return false; }
				return true;
			}
			case EE_N_ASXFLOAT_MAX : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
				if ( rTempImplied.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = CFloatX::GetMaxForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );
				return true;
			}
			case EE_N_ASXFLOAT_MIN : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
				if ( rTempImplied.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = CFloatX::GetNormalizedMinForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );
				return true;
			}
			case EE_N_ASXFLOAT_TRUE_MIN : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
				if ( rTempImplied.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = CFloatX::GetMinForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );
				return true;
			}
			case EE_N_ASXFLOAT_NAN : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
				if ( rTempImplied.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				CFloatX fTemp;
				fTemp.CreateNaN( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = fTemp.AsUint64();
				return true;
			}
			case EE_N_ASXFLOAT_INF : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
				if ( rTempImplied.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				CFloatX fTemp;
				fTemp.CreateInfP( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = fTemp.AsUint64();
				return true;
			}
			case EE_N_ASXFLOAT_SUBNORM_MAX : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
				if ( rTempImplied.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = CFloatX::GetDenormalizedMaxForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );
				return true;
			}
			case EE_N_ASXFLOAT_EPS : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
				if ( rTempImplied.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				CFloatX fOne, fNextUp;
				fOne.CreateFromDouble( 1.0, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );
				fNextUp.CreateFromParts( fOne.bSign, fOne.uiExponent, fOne.uiMantissa + 1, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );

				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = fNextUp.AsDouble() - fOne.AsDouble();
				return true;
			}
			case EE_N_ASXFLOAT_SIGNBIT : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }
				EE_RESULT rTempDoubleVal;
				if ( !ResolveNode( _ndExp.y.sNodeIndex, rTempDoubleVal ) ) { return false; }

				rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
				if ( rTempImplied.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				rTempDoubleVal = ConvertResultOrObject( rTempDoubleVal, EE_NC_FLOATING );
				if ( rTempDoubleVal.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				CFloatX fTemp;
				fTemp.CreateFromDouble( rTempDoubleVal.u.dVal, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = fTemp.SignBit();
				return true;
			}
			case EE_N_ASXFLOAT_EXPBITS : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }
				EE_RESULT rTempDoubleVal;
				if ( !ResolveNode( _ndExp.y.sNodeIndex, rTempDoubleVal ) ) { return false; }

				rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
				if ( rTempImplied.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				rTempDoubleVal = ConvertResultOrObject( rTempDoubleVal, EE_NC_FLOATING );
				if ( rTempDoubleVal.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				CFloatX fTemp;
				fTemp.CreateFromDouble( rTempDoubleVal.u.dVal, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = fTemp.ExpBits();
				return true;
			}
			case EE_N_ASXFLOAT_MANBITS : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( _ndExp.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( _ndExp.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( _ndExp.x.sNodeIndex, rTempImplied ) ) { return false; }
				EE_RESULT rTempDoubleVal;
				if ( !ResolveNode( _ndExp.y.sNodeIndex, rTempDoubleVal ) ) { return false; }

				rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
				if ( rTempImplied.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
				rTempDoubleVal = ConvertResultOrObject( rTempDoubleVal, EE_NC_FLOATING );
				if ( rTempDoubleVal.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				CFloatX fTemp;
				fTemp.CreateFromDouble( rTempDoubleVal.u.dVal, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = fTemp.ManBits();
				return true;
			}
			case EE_N_COMPOUND_STATEMENT : {
				if ( BreakingOrContinuing() ) { return true; }
				if ( !ResolveNode( _ndExp.v.sNodeIndex, _rRes ) ) { return false; }
				if ( BreakingOrContinuing() ) { return true; }
				if ( !ResolveNode( _ndExp.w.sNodeIndex, _rRes ) ) { return false; }
				return true;
			}
			case EE_N_WHILE_LOOP : {
				uint64_t ui64Safety = EE_MAX_ITERATION_COUNT;
				size_t stStackIdx = m_vLoopStack.size();
				{
					EE_LOOP_STACK_ADDER lsaLoopStack( m_vLoopStack, EE_LOOP_STACK() );
					while ( ui64Safety-- ) {
						// ==== Check the loop condition.
						EE_RESULT rTemp;
						if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }
						if ( (rTemp.ncType) == EE_NC_FLOATING ) {
							if ( !rTemp.u.dVal ) { break; }
						}
						else if ( (rTemp.ncType) == EE_NC_UNSIGNED ) {
							if ( !rTemp.u.ui64Val ) { break; }
						}
						else if ( (rTemp.ncType) == EE_NC_SIGNED ) {
							if ( !rTemp.u.i64Val ) { break; }
						}
						else { return false; }
						// ==============================

						// ==== Execute body.
						if ( !ResolveNode( _ndExp.v.sNodeIndex, _rRes ) ) { return false; }
						if ( m_vLoopStack[stStackIdx].bBreak ) { break; }
						m_vLoopStack[stStackIdx].bContinue = false;
						// ==================

						// Last.
						++m_vLoopStack[stStackIdx].sCurIdx;
					}
				}
				

				if ( ui64Safety == static_cast<uint64_t>(-1) ) {
					_rRes.ncType = EE_NC_UNSIGNED;
					_rRes.u.ui64Val = 0;
				}

				return true;
			}
			case EE_N_FOR_LOOP : {

				EE_RESULT rTempInit;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTempInit ) ) { return false; }

				uint64_t ui64Safety = EE_MAX_ITERATION_COUNT;
				size_t stStackIdx = m_vLoopStack.size();
				{
					EE_LOOP_STACK_ADDER lsaLoopStack( m_vLoopStack, EE_LOOP_STACK() );
					while ( ui64Safety-- ) {
						// ==== Check the loop condition.
						EE_RESULT rTemp;
						if ( !ResolveNode( _ndExp.v.sNodeIndex, rTemp ) ) { return false; }
						if ( (rTemp.ncType) == EE_NC_FLOATING ) {
							if ( !rTemp.u.dVal ) { break; }
						}
						else if ( (rTemp.ncType) == EE_NC_UNSIGNED ) {
							if ( !rTemp.u.ui64Val ) { break; }
						}
						else if ( (rTemp.ncType) == EE_NC_SIGNED ) {
							if ( !rTemp.u.i64Val ) { break; }
						}
						else { return false; }
						// ==============================

						// ==== Execute body.
						if ( !ResolveNode( _ndExp.x.sNodeIndex, _rRes ) ) { return false; }
						if ( m_vLoopStack[stStackIdx].bBreak ) { break; }
						m_vLoopStack[stStackIdx].bContinue = false;
						// ==================


						// ==== Increment/Post-Loop Expression
						if ( _ndExp.w.sNodeIndex != ~0 ) {
							if ( !ResolveNode( _ndExp.w.sNodeIndex, rTemp ) ) { return false; }
						}

						// Last.
						++m_vLoopStack[stStackIdx].sCurIdx;
					}
				}
				

				if ( ui64Safety == static_cast<uint64_t>(-1) ) {
					_rRes.ncType = EE_NC_UNSIGNED;
					_rRes.u.ui64Val = 0;
				}

				return true;
			}
			case EE_N_DO_WHILE_LOOP : {
				uint64_t ui64Safety = EE_MAX_ITERATION_COUNT;
				size_t stStackIdx = m_vLoopStack.size();
				{
					EE_LOOP_STACK_ADDER lsaLoopStack( m_vLoopStack, EE_LOOP_STACK() );
					while ( ui64Safety-- ) {
						// ==== Execute body.
						if ( !ResolveNode( _ndExp.v.sNodeIndex, _rRes ) ) { return false; }
						if ( m_vLoopStack[stStackIdx].bBreak ) { break; }
						m_vLoopStack[stStackIdx].bContinue = false;
						// ==================



						// ==== Check the loop condition.
						EE_RESULT rTemp;
						if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }
						if ( (rTemp.ncType) == EE_NC_FLOATING ) {
							if ( !rTemp.u.dVal ) { break; }
						}
						else if ( (rTemp.ncType) == EE_NC_UNSIGNED ) {
							if ( !rTemp.u.ui64Val ) { break; }
						}
						else if ( (rTemp.ncType) == EE_NC_SIGNED ) {
							if ( !rTemp.u.i64Val ) { break; }
						}
						else { return false; }
						// ==============================

						// Last.
						++m_vLoopStack[stStackIdx].sCurIdx;
					}
				}
				

				if ( ui64Safety == static_cast<uint64_t>(-1) ) {
					_rRes.ncType = EE_NC_UNSIGNED;
					_rRes.u.ui64Val = 0;
				}
				return true;
			}
			case EE_N_FOREACHDECL0 : {
				// Fall-through.  Loop is handled in EE_N_FOREACH.
				// This is just so that the parser parses the declaration before it parses the statements that follow.
				return true;
			}
			case EE_N_FOREACHDECL1 : {
				// Fall-through.  Loop is handled in EE_N_FOREACHOBJ.
				// This is just so that the parser parses the declaration before it parses the statements that follow.
				return true;
			}
			case EE_N_FOREACHDECL2 : {
				// Fall-through.  Loop is handled in EE_N_FOREACHSTR.
				// This is just so that the parser parses the declaration before it parses the statements that follow.
				return true;
			}
			case EE_N_FOREACH : {
				YYSTYPE::EE_NODE_DATA & ndDeclNode = m_vNodes[_ndExp.u.sNodeIndex];
				if ( ndDeclNode.v.sNodeIndex >= m_vArrayData.size() || !m_vArrayData[ndDeclNode.v.sNodeIndex].m_pabBase ) { return false; }

				auto aFind = m_mCustomVariables.find( ndDeclNode.u.sNodeIndex );
				if ( aFind == m_mCustomVariables.end() ) { return false; }


				size_t stStackIdx = m_vLoopStack.size();
				{
					EE_LOOP_STACK_ADDER lsaLoopStack( m_vLoopStack, EE_LOOP_STACK() );
					for ( size_t I = 0; I < m_vArrayData[ndDeclNode.v.sNodeIndex].m_pabBase->GetSize(); ++I ) {
						if ( !m_vArrayData[ndDeclNode.v.sNodeIndex].m_pabBase->ReadValue( I, aFind->second.rRes ) ) { return false; }

						// ==== Execute body.
						if ( !ResolveNode( _ndExp.v.sNodeIndex, _rRes ) ) { return false; }
						if ( m_vLoopStack[stStackIdx].bBreak ) { break; }
						m_vLoopStack[stStackIdx].bContinue = false;
						// ==================

						// Last.
						++m_vLoopStack[stStackIdx].sCurIdx;
					}
				}
				return true;
			}
			case EE_N_FOREACHOBJ : {
				YYSTYPE::EE_NODE_DATA & ndDeclNode = m_vNodes[_ndExp.u.sNodeIndex];
				// The custom_var part.
				auto aCusVar = m_mCustomVariables.find( ndDeclNode.v.sNodeIndex );
				if ( aCusVar == m_mCustomVariables.end() ) { return false; }
				if ( aCusVar->second.rRes.ncType != EE_NC_OBJECT || aCusVar->second.rRes.u.poObj == nullptr ) { return false; }

				// The identifier part.
				auto aFind = m_mCustomVariables.find( ndDeclNode.u.sNodeIndex );
				if ( aFind == m_mCustomVariables.end() ) { return false; }


				size_t stStackIdx = m_vLoopStack.size();
				{
					EE_LOOP_STACK_ADDER lsaLoopStack( m_vLoopStack, EE_LOOP_STACK() );
					uint64_t ui64Total = ConvertResultOrObject( aCusVar->second.rRes.u.poObj->Len(), EE_NC_UNSIGNED ).u.ui64Val;
					if ( ui64Total != static_cast<uint64_t>(static_cast<size_t>(ui64Total)) ) { return false; }
					for ( size_t I = 0; I < ui64Total; ++I ) {
						aFind->second.rRes = aCusVar->second.rRes.u.poObj->ArrayAccess( I );
						if ( aFind->second.rRes.ncType == EE_NC_INVALID ) { return false; }

						// ==== Execute body.
						if ( !ResolveNode( _ndExp.v.sNodeIndex, _rRes ) ) { return false; }
						if ( m_vLoopStack[stStackIdx].bBreak ) { break; }
						m_vLoopStack[stStackIdx].bContinue = false;
						// ==================

						// Last.
						++m_vLoopStack[stStackIdx].sCurIdx;
					}
				}
				return true;
			}
			case EE_N_FOREACHSTR : {
				YYSTYPE::EE_NODE_DATA & ndDeclNode = m_vNodes[_ndExp.u.sNodeIndex];
				// The string part.
				if ( ndDeclNode.v.sNodeIndex >= m_vStrings.size() ) { return false; }

				// The identifier part.
				auto aFind = m_mCustomVariables.find( ndDeclNode.u.sNodeIndex );
				if ( aFind == m_mCustomVariables.end() ) { return false; }


				size_t stStackIdx = m_vLoopStack.size();
				{
					EE_LOOP_STACK_ADDER lsaLoopStack( m_vLoopStack, EE_LOOP_STACK() );
					size_t sSize;
					aFind->second.rRes.ncType = EE_NC_UNSIGNED;
					for ( size_t I = 0; I < m_vStrings[ndDeclNode.v.sNodeIndex].size(); I += sSize ) {
						aFind->second.rRes.u.ui64Val = ee::CExpEval::NextUtf8Char( &m_vStrings[ndDeclNode.v.sNodeIndex][I], m_vStrings[ndDeclNode.v.sNodeIndex].size() - I, &sSize );
						if ( !sSize ) { break; }

						// ==== Execute body.
						if ( !ResolveNode( _ndExp.v.sNodeIndex, _rRes ) ) { return false; }
						if ( m_vLoopStack[stStackIdx].bBreak ) { break; }
						m_vLoopStack[stStackIdx].bContinue = false;
						// ==================

						// Last.
						++m_vLoopStack[stStackIdx].sCurIdx;
					}
				}
				return true;
			}
			case EE_N_IF : {
				EE_RESULT rTemp;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }

				// ==== Evaluate the condition.
				bool bResult = false;
				if ( (rTemp.ncType) == EE_NC_FLOATING ) {
					bResult = rTemp.u.dVal ? true : false;
				}
				else if ( (rTemp.ncType) == EE_NC_UNSIGNED ) {
					bResult = !!rTemp.u.ui64Val;
				}
				else if ( (rTemp.ncType) == EE_NC_SIGNED ) {
					bResult = !!rTemp.u.i64Val;
				}
				// ============================


				// == Execute the statement(s).
				if ( bResult ) {
					if ( !ResolveNode( _ndExp.v.sNodeIndex, _rRes ) ) { return false; }
				}
				else if ( _ndExp.w.sNodeIndex != ~0 ) {
					if ( !ResolveNode( _ndExp.w.sNodeIndex, _rRes ) ) { return false; }
				}
				else {
					_rRes.ncType = EE_NC_UNSIGNED;
					_rRes.u.ui64Val = 0;
				}
				// ==========================

				return true;
			}
			case EE_N_CONTINUE : {
				if ( !m_vLoopStack.size() ) {
					_rRes.ncType = EE_NC_INVALID;
					_rRes.u.ui64Val = 0;
					return false;
				}
				// Otherwise pass the result through.
				Continue();
				return true;
			}
			case EE_N_BREAK : {
				if ( !m_vLoopStack.size() ) {
					_rRes.ncType = EE_NC_INVALID;
					_rRes.u.ui64Val = 0;
					return false;
				}
				// Otherwise pass the result through.
				Break();
				return true;
			}
			case EE_N_ASSIGNMENT : {
				auto aFind = m_mCustomVariables.find( _ndExp.w.sNodeIndex );
				if ( aFind == m_mCustomVariables.end() ) { return false; }
				// If this were const and the assignment expression were const, then the variable would have been
				//	declared and assigned during compilation.  If this is const and it still tries to pass through
				//	EE_N_ASSIGNMENT then the assignment expression wasn't const.
				if ( (*aFind).second.bIsConst ) { return false; }

				if ( !ResolveNode( _ndExp.u.sNodeIndex, _rRes ) ) { return false; }
					
				

				switch ( _ndExp.v.ui32Op ) {
					case '=' : {
						aFind->second.rRes = _rRes;
						break;
					}

#define EE_CASE_PREP																						\
	EE_NUM_CONSTANTS ncTemp = GetCastType( aFind->second.rRes.ncType, _rRes.ncType );						\
	aFind->second.rRes = ConvertResultOrObject( aFind->second.rRes, ncTemp );								\
	if ( aFind->second.rRes.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }		\
	_rRes = ConvertResultOrObject( _rRes, ncTemp );															\
	if ( _rRes.ncType == EE_NC_INVALID ) return false 

#define EE_HANDLE_CASE( CASE, VAR, OP )																		\
	case CASE : {																							\
		aFind->second.rRes.u.VAR OP _rRes.u.VAR;															\
		_rRes = aFind->second.rRes;																			\
		break;																								\
	}

#define EE_HANDLE_CASE_BAD_ZERO( CASE, VAR, OP )															\
	case CASE : {																							\
		if ( !_rRes.u.VAR ) { return false; }																\
		aFind->second.rRes.u.VAR OP _rRes.u.VAR;															\
		_rRes = aFind->second.rRes;																			\
		break;																								\
	}

#define EE_CASE( CASE, OP )																					\
	case CASE : {																							\
		EE_CASE_PREP;																						\
		switch( _rRes.ncType ) {																			\
			EE_HANDLE_CASE( EE_NC_UNSIGNED, ui64Val, OP )													\
			EE_HANDLE_CASE( EE_NC_SIGNED, i64Val, OP )														\
			EE_HANDLE_CASE( EE_NC_FLOATING, dVal, OP )														\
			default : { return false; }																		\
		}																									\
		break;																								\
	}

#define EE_CASE_BAD_ZERO( CASE, OP )																		\
	case CASE : {																							\
		EE_CASE_PREP;																						\
		switch( _rRes.ncType ) {																			\
			EE_HANDLE_CASE_BAD_ZERO( EE_NC_UNSIGNED, ui64Val, OP )											\
			EE_HANDLE_CASE_BAD_ZERO( EE_NC_SIGNED, i64Val, OP )												\
			EE_HANDLE_CASE_BAD_ZERO( EE_NC_FLOATING, dVal, OP )												\
			default : { return false; }																		\
		}																									\
		break;																								\
	}

#define EE_INT_CASE( CASE, OP )																				\
	case CASE : {																							\
		EE_CASE_PREP;																						\
		switch( _rRes.ncType ) {																			\
			EE_HANDLE_CASE( EE_NC_UNSIGNED, ui64Val, OP )													\
			EE_HANDLE_CASE( EE_NC_SIGNED, i64Val, OP )														\
			default : { return false; }																		\
		}																									\
		break;																								\
	}

#define EE_INT_CASE_BAD_ZERO( CASE, OP )																	\
	case CASE : {																							\
		EE_CASE_PREP;																						\
		switch( _rRes.ncType ) {																			\
			EE_HANDLE_CASE_BAD_ZERO( EE_NC_UNSIGNED, ui64Val, OP )											\
			EE_HANDLE_CASE_BAD_ZERO( EE_NC_SIGNED, i64Val, OP )												\
			default : { return false; }																		\
		}																									\
		break;																								\
	}

					EE_CASE( CExpEvalParser::token::EE_ASS_PLUSEQUALS, += )
					EE_CASE( CExpEvalParser::token::EE_ASS_MINUSEQUALS, -= )
					EE_CASE( CExpEvalParser::token::EE_ASS_TIMESEQUALS, *= )
					EE_INT_CASE_BAD_ZERO( CExpEvalParser::token::EE_ASS_MODEQUALS, %= )
					EE_CASE_BAD_ZERO( CExpEvalParser::token::EE_ASS_DIVEQUALS, /= )
					case CExpEvalParser::token::EE_ASS_SHLEFTEQUALS : {
						EE_CASE_PREP;
						switch( _rRes.ncType ) {
							EE_HANDLE_CASE( EE_NC_UNSIGNED, ui64Val, <<= )
							EE_HANDLE_CASE( EE_NC_SIGNED, i64Val, <<= )
							case EE_NC_FLOATING : {
								aFind->second.rRes.u.dVal *= std::pow( 2.0, _rRes.u.dVal );
								_rRes = aFind->second.rRes;
								break;																								
							}
							default : { return false; }
						}
						break;
					}
					case CExpEvalParser::token::EE_ASS_SHRIGHTEQUALS : {
						EE_CASE_PREP;
						switch( _rRes.ncType ) {
							EE_HANDLE_CASE( EE_NC_UNSIGNED, ui64Val, >>= )
							EE_HANDLE_CASE( EE_NC_SIGNED, i64Val, >>= )
							case EE_NC_FLOATING : {
								aFind->second.rRes.u.dVal /= std::pow( 2.0, _rRes.u.dVal );
								_rRes = aFind->second.rRes;
								break;																								
							}
							default : { return false; }
						}
						break;
					}
					EE_INT_CASE( CExpEvalParser::token::EE_ASS_OREQUALS, |= )
					EE_INT_CASE( CExpEvalParser::token::EE_ASS_ANDEQUALS, &= )
					EE_INT_CASE( CExpEvalParser::token::EE_ASS_CARROTEQUALS, ^= )
				}
				

#undef EE_INT_CASE_BAD_ZERO
#undef EE_INT_CASE
#undef EE_CASE_BAD_ZERO
#undef EE_CASE
#undef EE_HANDLE_CASE_BAD_ZERO
#undef EE_HANDLE_CASE
#undef EE_CASE_PREP
				++aFind->second.ui64UpdateCounter;
				return true;
			}
			case EE_N_CREATE_ARRAY : {
				if ( (_rRes.ncType) == EE_NC_INVALID ) {
					// If this is the first statement in the entire script.
					_rRes.ncType = EE_NC_UNSIGNED;
					_rRes.u.ui64Val = 0;
				}

				if ( !m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase || (_ndExp.v.ui32Op >> 16) == CExpEvalParser::token::EE_TEMP ) {
					if ( !m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase ) {
						m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase = CreateArrayFromType( _ndExp.v.ui32Op & 0xFFFF );
						if ( !m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase ) { return false; }
					}

					// The array object exists.  Adjust its size and fill it.
					EE_RESULT rTemp;
					if ( !ResolveNode( _ndExp.z.sNodeIndex, rTemp ) ) { return false; }
					rTemp = ConvertResultOrObject( rTemp, EE_NC_UNSIGNED );
					if ( rTemp.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }
					m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase->SetSize( static_cast<size_t>(rTemp.u.ui64Val) );


					// Default start/end values.
					EE_RESULT rStart, rEnd;
					rStart.ncType = EE_NC_UNSIGNED;
					rStart.u.ui64Val = 0;
					if ( _ndExp.w.sNodeIndex != ~0 ) { if ( !ResolveNode( _ndExp.w.sNodeIndex, rStart ) ) { return false; } }
					if ( _ndExp.x.sNodeIndex != ~0 ) { if ( !ResolveNode( _ndExp.x.sNodeIndex, rEnd ) ) { return false; } }
					else {
						// No end value specified so default to matching the start value.
						rEnd = rStart;
					}
					m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase->Initialize( rStart, rEnd );
				}

				// Nothing to do here besides initialize the buffer if necessary.
				return true;
			}
			case EE_N_ARRAY_ASSIGNMENT : {
				EE_RESULT rTemp;
				if ( !ResolveNode( _ndExp.u.sNodeIndex, rTemp ) ) { return false; }
				rTemp = ConvertResultOrObject( rTemp, EE_NC_UNSIGNED );
				if ( rTemp.ncType == EE_NC_INVALID ) { _rRes.ncType = EE_NC_INVALID; return false; }

				switch ( _ndExp.v.ui32Op ) {
					case '=' : {
						return m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->WriteValue( static_cast<size_t>(rTemp.u.ui64Val), _rRes );
					}
					case CExpEvalParser::token::EE_ASS_PLUSEQUALS : {
						return m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->PlusEquals( static_cast<size_t>(rTemp.u.ui64Val), _rRes );
					}
					case CExpEvalParser::token::EE_ASS_MINUSEQUALS : {
						return m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->MinusEquals( static_cast<size_t>(rTemp.u.ui64Val), _rRes );
					}
					case CExpEvalParser::token::EE_ASS_TIMESEQUALS : {
						return m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->TimesEquals( static_cast<size_t>(rTemp.u.ui64Val), _rRes );
					}
					case CExpEvalParser::token::EE_ASS_MODEQUALS : {
						return m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->ModEquals( static_cast<size_t>(rTemp.u.ui64Val), _rRes );
					}
					case CExpEvalParser::token::EE_ASS_DIVEQUALS : {
						return m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->DivideEquals( static_cast<size_t>(rTemp.u.ui64Val), _rRes );
					}
					case CExpEvalParser::token::EE_ASS_CARROTEQUALS : {
						return m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->CarrotEquals( static_cast<size_t>(rTemp.u.ui64Val), _rRes );
					}
					case CExpEvalParser::token::EE_ASS_SHLEFTEQUALS : {
						return m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->ShiftLeftEquals( static_cast<size_t>(rTemp.u.ui64Val), _rRes );
					}
					case CExpEvalParser::token::EE_ASS_SHRIGHTEQUALS : {
						return m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->ShiftRightEquals( static_cast<size_t>(rTemp.u.ui64Val), _rRes );
					}
					case CExpEvalParser::token::EE_ASS_OREQUALS : {
						return m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->OrEquals( static_cast<size_t>(rTemp.u.ui64Val), _rRes );
					}
					case CExpEvalParser::token::EE_ASS_ANDEQUALS : {
						return m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->AndEquals( static_cast<size_t>(rTemp.u.ui64Val), _rRes );
					}
				}
				return false;
			}
		}
		return false;
	}

	// Resolves a node.
	bool CExpEvalContainer::ResolveNode_ExplicitStack( size_t _sNode, EE_RESULT &_rFinalResult, EE_ERROR_CODES &_ecError ) {
		if ( _sNode >= m_vNodes.size() ) { return false; }

		// The explicit stack.
		//std::vector<EE_STACK_OBJ> vStack;
		if ( m_vStack.size() ) {
			m_vStack.clear();
		}
		else {
			// Pre-allocating is absolutely 100% vital.  It WILL crash if this is not done.
			// Pointers to objects on the stack are used, so if re-allocation ever occurs the pointers will become
			//	invalid but may still be in use.  Pre-allocation solves this.  Additionally, the allocation size
			//	must be large enough to ensure that re-allocation never happens.  To ensure this, the stack is
			//	allocated to the number of nodes + 1.
			try {
				m_vStack.reserve( m_vNodes.size() + 1 );
			}
			catch ( const std::bad_alloc & ) {
				_ecError = EE_EC_OUTOFMEMORY;
				return false;
			}
		}

		// Remove run-time-allocated objects.
		while ( m_vsCompileTimeObjects != m_vObjects.size() ) {
			delete m_vObjects[m_vObjects.size()-1];
			m_vObjects.pop_back();
		}

		if ( !PushNode( m_vStack, m_vNodes.size() - 1, EE_INVALID_IDX, &_rFinalResult ) ) {
			_ecError = EE_EC_OUTOFMEMORY;
			return false;
		}

		_rFinalResult = DefaultResult();
		m_vLoopStack.clear();
		/*if ( m_vStrings.size() > m_vStringObjectsU8.size() ) {
			m_vStringObjectsU8.resize( m_vStrings.size() );
		}
		if ( m_vStringObjectsU8.size() < m_vStrings.size() ) {
			_ecError = EE_EC_OUTOFMEMORY;
			return false;
		}*/


#define EE_PUSH_EXP( IDX, RES )						if ( sParmIdx == EE_MAX_SUB_EXPRESSIONS ) { _ecError = EE_EC_RESULTSTOOSHORT; m_vLoopStack.clear(); return false; }	\
	if ( !PushNode( m_vStack, IDX, sThisIndex, RES ) ) { _ecError = EE_EC_OUTOFMEMORY; m_vLoopStack.clear(); return false; }

#define EE_PUSH( IDX )								EE_PUSH_EXP( IDX, &m_vStack[sThisIndex].sSubResults[sParmIdx++] )
#define EE_ERROR( IDX )								_ecError = IDX; m_vLoopStack.clear(); return false
#define EE_SEND_BACK								m_vStack[sThisIndex].bPassThrough = false; break
#define EE_HAND_OFF( IDX )							{																								\
		size_t sIdx = IDX;																															\
		size_t sParentIdx = m_vStack[sThisIndex].sParentIndex;																						\
		EE_RESULT * prTemp = m_vStack[sThisIndex].prResult;																							\
		m_vStack.pop_back();																														\
		if ( !PushNode( m_vStack, sIdx, sParentIdx, prTemp ) ) { _ecError = EE_EC_OUTOFMEMORY; m_vLoopStack.clear(); return false; }				\
	}

#define EE_DONE										m_vStack.pop_back(); continue
#define EE_BEGIN_LOOP								m_vStack[sThisIndex].sLoopStackIdx = m_vLoopStack.size(); m_vStack[sThisIndex].uiLoopCounter = 0; m_vLoopStack.push_back( EE_LOOP_STACK() )
#define EE_END_LOOP									m_vLoopStack.pop_back(); EE_DONE

		
		while ( m_vStack.size() ) {
			// Process the top of the stack.
			size_t sThisIndex = m_vStack.size() - 1;
			//EE_STACK_OBJ & soProcessMe = m_vStack[sThisIndex];

			//EE_STACK_OBJ & soDebugMe = m_vStack[sThisIndex];
#define soProcessMe									m_vStack[sThisIndex]
			bool bFirstPassOnThisNode = !soProcessMe.bPassThrough;
			uint64_t uiProcessCount = soProcessMe.uiProcessCount++;
			// Revisit this node after processing its children, but with the already-processed flag set to true.
			soProcessMe.bPassThrough = true;
			
			const YYSTYPE::EE_NODE_DATA & _ndExp = m_vNodes[soProcessMe.sIndex];
			size_t sParmIdx = 0;
			if ( bFirstPassOnThisNode ) {
				switch ( _ndExp.nType ) {
					case EE_N_NUMERICCONSTANT : {
						(*soProcessMe.prResult).ncType = _ndExp.v.ncConstType;
						(*soProcessMe.prResult).u.ui64Val = _ndExp.u.ui64Val;
						// Minor optimization.
						EE_DONE;
					}
					case EE_N_STRING : {
						if ( m_pfshString ) {
							 if ( !m_pfshString( m_vStrings[_ndExp.u.sStringIndex], m_uiptrStringData, this, (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_IDENTHANDLERFAILED ); }
						}
						else {
							// Return a string object reference.
							//if ( !CreateStringRef( _ndExp.u.sStringIndex ) ) { EE_ERROR( EE_EC_OUTOFMEMORY ); }
							CString * psString = AllocateObject<CString>();
							if ( !psString ) { EE_ERROR( EE_EC_OUTOFMEMORY ); }
							psString->SetString( m_vStrings[_ndExp.u.sStringIndex] );
							// _ndExp.u.sStringIndex now usable in either m_vStrings or m_vStringObjectsU8;
							(*soProcessMe.prResult).ncType = EE_NC_OBJECT;
							(*soProcessMe.prResult).u.poObj = psString;
						}
						// Minor optimization.
						EE_DONE;
					}
					case EE_N_CUSTOM_VAR : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sStringIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_VARNOTFOUND ); }
						(*soProcessMe.prResult) = aFind->second.rRes;
						if ( ((*soProcessMe.prResult).ncType) == EE_NC_INVALID ) { EE_ERROR( EE_EC_VARHASBADTYPE ); }
						// Minor optimization.
						EE_DONE;
					}
					case EE_N_ARRAY : {
						if ( !m_vArrayData[_ndExp.u.sStringIndex].m_pabBase ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
						EE_PUSH( _ndExp.v.sNodeIndex );
						continue;
					}
					case EE_N_STRINGARRAY : {
						EE_PUSH( _ndExp.v.sNodeIndex );
						continue;
					}
					case EE_N_STRINGARRAY_EX : {
						if ( _ndExp.v.sNodeIndex != size_t( -1 ) ) {
							EE_PUSH( _ndExp.v.sNodeIndex );
						}
						else {
							m_vStack[sThisIndex].sSubResults[sParmIdx++].ncType = EE_NC_INVALID;
						}
						if ( _ndExp.w.sNodeIndex != size_t( -1 ) ) {
							EE_PUSH( _ndExp.w.sNodeIndex );
						}
						else {
							m_vStack[sThisIndex].sSubResults[sParmIdx++].ncType = EE_NC_INVALID;
						}

						continue;
					}
					case EE_N_ARRAY_ACCESS : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						EE_PUSH( _ndExp.v.sNodeIndex );

						continue;
					}
					case EE_N_ARRAY_ACCESS_EX : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						if ( _ndExp.v.sNodeIndex != size_t( -1 ) ) {
							EE_PUSH( _ndExp.v.sNodeIndex );
						}
						else {
							m_vStack[sThisIndex].sSubResults[sParmIdx++].ncType = EE_NC_INVALID;
						}
						if ( _ndExp.w.sNodeIndex != size_t( -1 ) ) {
							EE_PUSH( _ndExp.w.sNodeIndex );
						}
						else {
							m_vStack[sThisIndex].sSubResults[sParmIdx++].ncType = EE_NC_INVALID;
						}

						continue;
					}
					case EE_N_ADDRESS : {
						if ( !m_pfahAddressHandler ) { EE_ERROR( EE_EC_NOADDRESSHANDLER ); }
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_MEMBERACCESS : {
						if ( !m_pfmahMemberAccess ) { EE_ERROR( EE_EC_NOMEMBERHANDLER ); }
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_USER_VAR : {
						if ( !m_pfUser ) { EE_ERROR( EE_EC_NOUSERHANDLER ); }
						if ( !m_pfUser( m_uiptrUserData, this, (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_USERHANDLERFAILED ); }
						// Minor optimization.
						EE_DONE;
					}
					case EE_N_NUMBERED_PARM : {
						(*soProcessMe.prResult) = GetNumberedParm( static_cast<size_t>(_ndExp.u.ui64Val) );
						EE_DONE;
					}
					case EE_N_DYNAMIC_NUMBERED_PARM : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sStringIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_VARNOTFOUND ); }
						if ( (aFind->second.rRes.ncType) == EE_NC_INVALID ) { EE_ERROR( EE_EC_VARHASBADTYPE ); }
						EE_RESULT rTemp = ConvertResultOrObject( aFind->second.rRes, EE_NC_UNSIGNED );
						if ( rTemp.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult) = GetNumberedParm( static_cast<size_t>(rTemp.u.ui64Val) );
						EE_DONE;
					}
					case EE_N_DYNAMIC_NUMBERED_PARM_EXP : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_NUMBERED_PARM_TOTAL : {
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.ui64Val = m_vNumberedParms.size();
						EE_DONE;
					}
					case EE_N_UNARY : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_CAST : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_OP : {
						if ( uiProcessCount == 0 ) {
							EE_PUSH( _ndExp.u.sNodeIndex );
							// Short-circuit evaluation means potentially sending this stack object back multiple times.
							if ( _ndExp.v.ui32Op == CExpEvalParser::token::EE_AND || _ndExp.v.ui32Op == CExpEvalParser::token::EE_OR ) {
								EE_SEND_BACK;	// Breaks.
							}
							// No short-circuiting, so just push the 2nd parameter and avoid wasting time coming back to this point.
							EE_PUSH( _ndExp.w.sNodeIndex );
						}
						// It will only come here if EE_SEND_BACK was called above.
						if ( uiProcessCount == 1 ) {
							sParmIdx = 1;
							EE_RESULT & rLeft = soProcessMe.sSubResults[0];
							// Short-circuit evaluation is not only expected by programmers, it will improve performance in loops.
							if ( _ndExp.v.ui32Op == CExpEvalParser::token::EE_AND ) {
								if ( ((rLeft.ncType) == EE_NC_UNSIGNED && !rLeft.u.ui64Val) ||
									((rLeft.ncType) == EE_NC_SIGNED && !rLeft.u.i64Val) ||
									((rLeft.ncType) == EE_NC_FLOATING && !rLeft.u.dVal) ) {
									(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
									(*soProcessMe.prResult).u.ui64Val = false;
									// Minor optimization.
									EE_DONE;
								}
							}
							else if ( _ndExp.v.ui32Op == CExpEvalParser::token::EE_OR ) {
								if ( ((rLeft.ncType) == EE_NC_UNSIGNED && rLeft.u.ui64Val) ||
									((rLeft.ncType) == EE_NC_SIGNED && rLeft.u.i64Val) ||
									((rLeft.ncType) == EE_NC_FLOATING && rLeft.u.dVal) ) {
									(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
									(*soProcessMe.prResult).u.ui64Val = true;
									// Minor optimization.
									EE_DONE;
								}
							}
							EE_PUSH( _ndExp.w.sNodeIndex );
						}
						continue;
					}
					case EE_N_CONDITIONAL : {
						if ( uiProcessCount == 0 ) {
							EE_PUSH( _ndExp.u.sNodeIndex );
							EE_SEND_BACK;	// Breaks.
						}
						EE_RESULT & rExp = soProcessMe.sSubResults[0];
						bool bTrue;
						if ( (rExp.ncType) == EE_NC_SIGNED ) {
							bTrue = rExp.u.i64Val != 0;
						}
						else if ( (rExp.ncType) == EE_NC_UNSIGNED ) {
							bTrue = rExp.u.ui64Val != 0;
						}
						else if ( (rExp.ncType) == EE_NC_FLOATING ) {
							bTrue = rExp.u.dVal != 0.0;
						}
						else { EE_ERROR( EE_EC_INVALIDTREE ); }	// Can't happen.

						// Slight variation from C/C++.  In compiled code, the left and right nodes have to be the same type, so if one of them
						//	was double, the other would be cast to double (because the ?: expression must be of a single specific type).
						// Here, the return type is whatever the left or right node's return type is.  Lazy evaluation to save time.
						EE_HAND_OFF( bTrue ? _ndExp.v.sNodeIndex : _ndExp.w.sNodeIndex );
						continue;
					}
					case EE_N_INTRINSIC_0 : {
						switch ( _ndExp.u.ui32Intrinsic ) {
							case CExpEvalParser::token::EE_CLOCK : {
								(*soProcessMe.prResult).ncType = EE_NC_SIGNED;
								(*soProcessMe.prResult).u.i64Val = ::clock();
								EE_DONE;
							}
							case CExpEvalParser::token::EE_SECONDS : {
								(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
								(*soProcessMe.prResult).u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() ) / 1000000ULL;
								EE_DONE;
							}
							case CExpEvalParser::token::EE_MILLISECONDS : {
								(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
								(*soProcessMe.prResult).u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() ) / 1000ULL;
								EE_DONE;
							}
							case CExpEvalParser::token::EE_MICROSECONDS : {
								(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
								(*soProcessMe.prResult).u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() );
								EE_DONE;
							}
							case CExpEvalParser::token::EE_SECONDS_SINCE_START : {
								(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
								(*soProcessMe.prResult).u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() - ee::CExpEval::StartTime() ) / 1000000ULL;
								EE_DONE;
							}
							case CExpEvalParser::token::EE_MILLISECONDS_SINCE_START : {
								(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
								(*soProcessMe.prResult).u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() - ee::CExpEval::StartTime() ) / 1000ULL;
								EE_DONE;
							}
							case CExpEvalParser::token::EE_MICROSECONDS_SINCE_START : {
								(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
								(*soProcessMe.prResult).u.ui64Val = ee::CExpEval::TicksToMicroseconds( ee::CExpEval::Time() - ee::CExpEval::StartTime() );
								EE_DONE;
							}
						}
						(*soProcessMe.prResult).ncType = EE_NC_INVALID;
						EE_ERROR( EE_EC_UNRECOGNIZEDINTRINSIC0 );
					}
					case EE_N_INTRINSIC_0_SIGNED : {
						(*soProcessMe.prResult).ncType = EE_NC_SIGNED;
						(*soProcessMe.prResult).u.i64Val = _ndExp.uFuncPtr.pfClock();
						EE_DONE;
					}
					case EE_N_INTRINSIC_0_UNSIGNED_DIVISOR : {
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.i64Val = _ndExp.uFuncPtr.pfIntrins0Unsigned( ee::CExpEval::Time() ) / _ndExp.u.ui64Val;
						EE_DONE;
					}
					case EE_N_INTRINSIC_1_FLOAT_FLOAT : {}
					case EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED16 : {}
					case EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED32 : {}
					case EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED64 : {}
					case EE_N_INTRINSIC_1_BOOL_FLOAT : {}
					case EE_N_INTRINSIC_1_INT_FLOAT : {}
					case EE_N_INTRINSIC_1 : {
						EE_PUSH( _ndExp.v.sNodeIndex );
						continue;
					}
					case EE_N_INTRINSIC_2_FLOAT_FLOAT_FLOAT : {}
					case EE_N_INTRINSIC_2_FLOAT_FLOAT32_FLOAT32 : {}
					case EE_N_INTRINSIC_2_FLOAT_FLOAT80_FLOAT80 : {}
					case EE_N_INTRINSIC_2_FLOAT_FLOAT32_FLOAT80 : {}
					case EE_N_INTRINSIC_2 : {
						EE_PUSH( _ndExp.v.sNodeIndex );
						EE_PUSH( _ndExp.w.sNodeIndex );
						continue;
					}
					case EE_N_INTRINSIC_3_BOOL_FLOAT32_FLOAT32_FLOAT32 : {}
					case EE_N_INTRINSIC_3_BOOL_FLOAT_FLOAT_FLOAT : {}
					case EE_N_INTRINSIC_3_FLOAT_FLOAT_FLOAT_FLOAT : {}
					case EE_N_INTRINSIC_3 : {
						EE_PUSH( _ndExp.v.sNodeIndex );
						EE_PUSH( _ndExp.w.sNodeIndex );
						EE_PUSH( _ndExp.x.sNodeIndex );
						continue;
					}
					case EE_N_ASFLOAT : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_ASDOUBLE : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_ASXFLOAT : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						EE_PUSH( _ndExp.v.sNodeIndex );
						EE_PUSH( _ndExp.w.sNodeIndex );
						EE_PUSH( _ndExp.x.sNodeIndex );
						EE_PUSH( _ndExp.y.sNodeIndex );
						EE_PUSH( _ndExp.z.sNodeIndex );
						EE_PUSH( _ndExp.a.sNodeIndex );
						continue;
					}
					case EE_N_ASXFLOAT_FROM_DOUBLE : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						EE_PUSH( _ndExp.v.sNodeIndex );
						EE_PUSH( _ndExp.w.sNodeIndex );
						EE_PUSH( _ndExp.x.sNodeIndex );
						EE_PUSH( _ndExp.y.sNodeIndex );
						continue;
					}
					case EE_N_ASFLOAT16 : {}
					case EE_N_ASFLOAT14 : {}
					case EE_N_ASFLOAT11 : {}
					case EE_N_ASFLOAT10 : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_ASXFLOAT_MAX : {}
					case EE_N_ASXFLOAT_MIN : {}
					case EE_N_ASXFLOAT_TRUE_MIN : {}
					case EE_N_ASXFLOAT_NAN : {}
					case EE_N_ASXFLOAT_INF : {}
					case EE_N_ASXFLOAT_SUBNORM_MAX : {}
					case EE_N_ASXFLOAT_EPS : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						EE_PUSH( _ndExp.v.sNodeIndex );
						EE_PUSH( _ndExp.w.sNodeIndex );
						EE_PUSH( _ndExp.x.sNodeIndex );
						continue;
					}
					case EE_N_ASXFLOAT_SIGNBIT : {}
					case EE_N_ASXFLOAT_EXPBITS : {}
					case EE_N_ASXFLOAT_MANBITS : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						EE_PUSH( _ndExp.v.sNodeIndex );
						EE_PUSH( _ndExp.w.sNodeIndex );
						EE_PUSH( _ndExp.x.sNodeIndex );
						EE_PUSH( _ndExp.y.sNodeIndex );
						continue;
					}
					case EE_N_COMPOUND_STATEMENT : {
						if ( BreakingOrContinuing() ) { EE_DONE; }
						if ( uiProcessCount == 0 ) {
							EE_PUSH_EXP( _ndExp.v.sNodeIndex, soProcessMe.prResult );
							EE_SEND_BACK;	// Breaks.
						}
						EE_HAND_OFF( _ndExp.w.sNodeIndex );
						continue;
					}
					case EE_N_WHILE_LOOP : {
						EE_BEGIN_LOOP;
						continue;
					}
					case EE_N_FOR_LOOP : {
						EE_BEGIN_LOOP;
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_DO_WHILE_LOOP : {
						EE_BEGIN_LOOP;
						continue;
					}
					case EE_N_FOREACHDECL0 : {
						// Fall-through.  Loop is handled in EE_N_FOREACH.
						// This is just so that the parser parses the declaration before it parses the statements that follow.
						EE_DONE;
					}
					case EE_N_FOREACHDECL1 : {
						// Fall-through.  Loop is handled in EE_N_FOREACHOBJ.
						// This is just so that the parser parses the declaration before it parses the statements that follow.
						EE_DONE;
					}
					case EE_N_FOREACHDECL2 : {
						// Fall-through.  Loop is handled in EE_N_FOREACHSTR.
						// This is just so that the parser parses the declaration before it parses the statements that follow.
						EE_DONE;
					}
					case EE_N_FOREACH : {
						YYSTYPE::EE_NODE_DATA & ndDeclNode = m_vNodes[_ndExp.u.sNodeIndex];
						if ( ndDeclNode.v.sNodeIndex >= m_vArrayData.size() || !m_vArrayData[ndDeclNode.v.sNodeIndex].m_pabBase ) { EE_ERROR( EE_EC_INVALIDLOOPDECL ); }

						auto aFind = m_mCustomVariables.find( ndDeclNode.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
						soProcessMe.prLoopCustomVarResult = &aFind->second.rRes;
						(*soProcessMe.prResult) = DefaultResult();
						EE_BEGIN_LOOP;
						continue;
					}
					case EE_N_FOREACHOBJ : {
						YYSTYPE::EE_NODE_DATA & ndDeclNode = m_vNodes[_ndExp.u.sNodeIndex];
						// The custom_var part.
						auto aCusVar = m_mCustomVariables.find( ndDeclNode.v.sNodeIndex );
						if ( aCusVar == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDLOOPDECL ); }
						if ( aCusVar->second.rRes.ncType != EE_NC_OBJECT || aCusVar->second.rRes.u.poObj == nullptr ) { EE_ERROR( EE_EC_INVALIDLOOPDECL ); }

						// The identifier part.
						auto aFind = m_mCustomVariables.find( ndDeclNode.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
						soProcessMe.prLoopCustomVarResult = &aFind->second.rRes;
						soProcessMe.prLoopObject = &aCusVar->second.rRes;
						(*soProcessMe.prResult) = DefaultResult();
						EE_BEGIN_LOOP;
						continue;
					}
					case EE_N_FOREACHSTR : {
						YYSTYPE::EE_NODE_DATA & ndDeclNode = m_vNodes[_ndExp.u.sNodeIndex];
						// The string part.
						if ( ndDeclNode.v.sNodeIndex >= m_vStrings.size() ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
						// The identifier part.
						auto aFind = m_mCustomVariables.find( ndDeclNode.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }

						soProcessMe.prLoopCustomVarResult = &aFind->second.rRes;
						soProcessMe.sForEachString = ndDeclNode.v.sNodeIndex;
						soProcessMe.sForEachStringPos = 0;
						(*soProcessMe.prResult) = DefaultResult();
						EE_BEGIN_LOOP;
						continue;
				
						/*YYSTYPE::EE_NODE_DATA & ndDeclNode = m_vNodes[_ndExp.u.sNodeIndex];
						// The string part.
						if ( ndDeclNode.v.sNodeIndex >= m_vStrings.size() ) { return false; }

						// The identifier part.
						auto aFind = m_mCustomVariables.find( ndDeclNode.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { return false; }


						size_t stStackIdx = m_vLoopStack.size();
						{
							EE_LOOP_STACK_ADDER lsaLoopStack( m_vLoopStack, EE_LOOP_STACK() );
							size_t sSize;
							aFind->second.rRes.ncType = EE_NC_UNSIGNED;
							for ( size_t I = 0; I < m_vStrings[ndDeclNode.v.sNodeIndex].size(); I += sSize ) {
								aFind->second.rRes.u.ui64Val = NextUtf8Char( &m_vStrings[ndDeclNode.v.sNodeIndex][I], m_vStrings[ndDeclNode.v.sNodeIndex].size() - I, &sSize );
								if ( !sSize ) { break; }

								// ==== Execute body.
								if ( !ResolveNode( _ndExp.v.sNodeIndex, _rRes ) ) { return false; }
								if ( m_vLoopStack[stStackIdx].bBreak ) { break; }
								m_vLoopStack[stStackIdx].bContinue = false;
								// ==================

								// Last.
								++m_vLoopStack[stStackIdx].sCurIdx;
							}
						}
						return true;*/
					}
					case EE_N_IF : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_CONTINUE : {
						if ( !m_vLoopStack.size() ) {
							(*soProcessMe.prResult).ncType = EE_NC_INVALID;
							(*soProcessMe.prResult).u.ui64Val = 0;
							EE_ERROR( EE_EC_PROCESSINGERROR );
						}
						// Otherwise pass the result through.
						Continue();
						EE_DONE;
					}
					case EE_N_BREAK : {
						if ( !m_vLoopStack.size() ) {
							(*soProcessMe.prResult).ncType = EE_NC_INVALID;
							(*soProcessMe.prResult).u.ui64Val = 0;
							EE_ERROR( EE_EC_PROCESSINGERROR );
						}
						// Otherwise pass the result through.
						Break();
						EE_DONE;
					}
					case EE_N_ASSIGNMENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.w.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						// If this were const and the assignment expression were const, then the variable would have been
						//	declared and assigned during compilation.  If this is const and it still tries to pass through
						//	EE_N_ASSIGNMENT then the assignment expression wasn't const.
						if ( (*aFind).second.bIsConst ) { EE_ERROR( EE_EC_CONST_VAR_REQUIRES_CONST_EPRESSION ); }
						EE_PUSH_EXP( _ndExp.u.sNodeIndex, soProcessMe.prResult );
						continue;
					}
					case EE_N_POSTFIX_MINUSMINUS : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sStringIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						if ( (*aFind).second.bIsConst ) { EE_ERROR( EE_EC_INVALID_WRITE_TO_CONST ); }
						(*soProcessMe.prResult) = aFind->second.rRes;
						switch ( aFind->second.rRes.ncType ) {
							case EE_NC_UNSIGNED : {
								--aFind->second.rRes.u.ui64Val;
								break;
							}
							case EE_NC_SIGNED : {
								--aFind->second.rRes.u.i64Val;
								break;
							}
							case EE_NC_FLOATING : {
								--aFind->second.rRes.u.dVal;
								break;
							}
						}
						++aFind->second.ui64UpdateCounter;
						EE_DONE;
					}
					case EE_N_POSTFIX_PLUSPLUS : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sStringIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						if ( (*aFind).second.bIsConst ) { EE_ERROR( EE_EC_INVALID_WRITE_TO_CONST ); }
						(*soProcessMe.prResult) = aFind->second.rRes;
						switch ( aFind->second.rRes.ncType ) {
							case EE_NC_UNSIGNED : {
								++aFind->second.rRes.u.ui64Val;
								break;
							}
							case EE_NC_SIGNED : {
								++aFind->second.rRes.u.i64Val;
								break;
							}
							case EE_NC_FLOATING : {
								++aFind->second.rRes.u.dVal;
								break;
							}
						}
						++aFind->second.ui64UpdateCounter;
						EE_DONE;
					}
					case EE_N_PREFIX_MINUSMINUS : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sStringIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }						
						if ( (*aFind).second.bIsConst ) { EE_ERROR( EE_EC_INVALID_WRITE_TO_CONST ); }
						switch ( aFind->second.rRes.ncType ) {
							case EE_NC_UNSIGNED : {
								--aFind->second.rRes.u.ui64Val;
								break;
							}
							case EE_NC_SIGNED : {
								--aFind->second.rRes.u.i64Val;
								break;
							}
							case EE_NC_FLOATING : {
								--aFind->second.rRes.u.dVal;
								break;
							}
						}
						(*soProcessMe.prResult) = aFind->second.rRes;
						++aFind->second.ui64UpdateCounter;
						EE_DONE;
					}
					case EE_N_PREFIX_PLUSPLUS : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sStringIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }						
						if ( (*aFind).second.bIsConst ) { EE_ERROR( EE_EC_INVALID_WRITE_TO_CONST ); }
						switch ( aFind->second.rRes.ncType ) {
							case EE_NC_UNSIGNED : {
								++aFind->second.rRes.u.ui64Val;
								break;
							}
							case EE_NC_SIGNED : {
								++aFind->second.rRes.u.i64Val;
								break;
							}
							case EE_NC_FLOATING : {
								++aFind->second.rRes.u.dVal;
								break;
							}
						}
						(*soProcessMe.prResult) = aFind->second.rRes;
						++aFind->second.ui64UpdateCounter;
						EE_DONE;
					}
					case EE_N_CREATE_ARRAY : {
						if ( ((*soProcessMe.prResult).ncType) == EE_NC_INVALID ) {
							// If this is the first statement in the entire script.
							(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
							(*soProcessMe.prResult).u.ui64Val = 0;
						}
						// Nothing to do here besides initialize the buffer if necessary.
						if ( !m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase || (_ndExp.v.ui32Op >> 16) == CExpEvalParser::token::EE_TEMP ) {
							if ( !m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase ) {
								m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase = CreateArrayFromType( _ndExp.v.ui32Op & 0xFFFF );
								if ( !m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase ) { EE_ERROR( EE_EC_OUTOFMEMORY ); }
							}
							EE_PUSH( _ndExp.z.sNodeIndex );
							continue;
						}
						else { EE_DONE; }
					}
					case EE_N_ARRAY_ASSIGNMENT : {
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_ARRAY_ASSIGNMENT_OBJ : {
						auto aFind = m_mCustomVariables.find( _ndExp.w.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						// If this were const and the assignment expression were const, then the variable would have been
						//	declared and assigned during compilation.  If this is const and it still tries to pass through
						//	EE_N_ASSIGNMENT then the assignment expression wasn't const.
						if ( (*aFind).second.bIsConst ) { EE_ERROR( EE_EC_CONST_VAR_REQUIRES_CONST_EPRESSION ); }

						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = IDX.
						EE_PUSH( _ndExp.x.sNodeIndex );		// soProcessMe.sSubResults[1] = VAL.
						continue;
					}
					case EE_N_ADDRESS_ASSIGNMENT : {
						if ( !m_pfahAddressWriteHandler ) { EE_ERROR( EE_EC_NOADDRESSHANDLER ); }
						EE_PUSH( _ndExp.w.sNodeIndex );
						EE_PUSH( _ndExp.x.sNodeIndex );
						continue;
					}
					case EE_N_ARRAY_INITIALIZER : {
						EE_PUSH( _ndExp.v.sNodeIndex );
						continue;
					}
					case EE_N_ARRAY_INITIALIZER_LIST : {
						if ( _ndExp.u.sNodeIndex != size_t( ~0 ) ) {
							EE_PUSH( _ndExp.u.sNodeIndex );
						}
						EE_PUSH( _ndExp.v.sNodeIndex );
						continue;
					}
					case EE_N_ARRAY_CREATE_VECTOR : {
						m_vVectorStack.push_back( _ndExp.v.sStringIndex );

						CObject * poVector = m_vObjects[_ndExp.v.sStringIndex];
						if ( !(poVector->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						static_cast<CVector *>(poVector)->Clear();
						if ( _ndExp.u.sNodeIndex != size_t( ~0 ) ) {
							EE_PUSH( _ndExp.u.sNodeIndex );
						}
						continue;
					}
					case EE_N_ARG_LIST_ENTRY : {
						if ( _ndExp.v.sNodeIndex != ~0 ) {
							EE_PUSH( _ndExp.v.sNodeIndex );
						}
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_ARG_LIST : {
						m_vFuncParms.push_back( std::vector<EE_RESULT>() );
						EE_PUSH( _ndExp.u.sNodeIndex );
						continue;
					}
					case EE_N_FORMAT : {
						EE_PUSH( _ndExp.v.sNodeIndex );
						continue;
					}
					case EE_N_VECTOR_ADD : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ1.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = OBJ2.
						continue;
					}
					case EE_N_VECTOR_SUB : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ1.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = OBJ2.
						continue;
					}
					case EE_N_VECTOR_MUL : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ1.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = OBJ2.
						continue;
					}
					case EE_N_VECTOR_DOT : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ1.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = OBJ2.
						continue;
					}
					case EE_N_VECTOR_CROSS : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ1.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = OBJ2.
						continue;
					}
					case EE_N_VECTOR_APPEND_IDENT : {
						//EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						++sParmIdx;							// Fake push so we can fill in soProcessMe.sSubResults[0] manually later.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = VAL.
						continue;
					}
					case EE_N_VECTOR_APPEND : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = VAL.
						continue;
					}
					case EE_N_VECTOR_ASSIGN : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = VAL.
						EE_PUSH( _ndExp.w.sNodeIndex );		// soProcessMe.sSubResults[2] = IDX.
						continue;
					}
					case EE_N_VECTOR_AT_IDENT : {
						//EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						++sParmIdx;							// Fake push so we can fill in soProcessMe.sSubResults[0] manually later.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = VAL.
						continue;
					}
					case EE_N_VECTOR_AT : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						EE_PUSH( _ndExp.w.sNodeIndex );		// soProcessMe.sSubResults[1] = IDX.
						continue;
					}
					case EE_N_VECTOR_CAPACITY : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						continue;
					}
					case EE_N_VECTOR_CLEAR : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						continue;
					}
					case EE_N_VECTOR_EMPTY : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						continue;
					}
					case EE_N_VECTOR_ERASE_IDENT : {
						//EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						++sParmIdx;							// Fake push so we can fill in soProcessMe.sSubResults[0] manually later.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = VAL.
						continue;
					}
					case EE_N_VECTOR_ERASE : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						EE_PUSH( _ndExp.w.sNodeIndex );		// soProcessMe.sSubResults[1] = IDX.
						continue;
					}
					case EE_N_VECTOR_INSERT : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = VAL.
						EE_PUSH( _ndExp.w.sNodeIndex );		// soProcessMe.sSubResults[2] = IDX.
						continue;
					}
					case EE_N_VECTOR_MAX_SIZE : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						continue;
					}
					case EE_N_VECTOR_MAG : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						continue;
					}
					case EE_N_VECTOR_MAG_SQ : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						continue;
					}
					case EE_N_VECTOR_NORMALIZE : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						continue;
					}
					case EE_N_VECTOR_RESERVE_IDENT : {
						//EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						++sParmIdx;							// Fake push so we can fill in soProcessMe.sSubResults[0] manually later.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = VAL.
						continue;
					}
					case EE_N_VECTOR_RESERVE : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						EE_PUSH( _ndExp.w.sNodeIndex );		// soProcessMe.sSubResults[1] = IDX.
						continue;
					}
					case EE_N_VECTOR_RESIZE_IDENT : {
						//EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						++sParmIdx;							// Fake push so we can fill in soProcessMe.sSubResults[0] manually later.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = VAL.
						continue;
					}
					case EE_N_VECTOR_RESIZE : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						EE_PUSH( _ndExp.w.sNodeIndex );		// soProcessMe.sSubResults[1] = IDX.
						continue;
					}
					case EE_N_VECTOR_POP_BACK : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						continue;
					}
					case EE_N_VECTOR_SHRINK_TO_FIT : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						continue;
					}
					case EE_N_VECTOR_SIZE : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						continue;
					}
					case EE_N_VECTOR_SUM : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						continue;
					}
					case EE_N_VECTOR_SWAP_IDENT : {
						//EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						++sParmIdx;							// Fake push so we can fill in soProcessMe.sSubResults[0] manually later.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = VAL.
						continue;
					}
					case EE_N_VECTOR_SWAP : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = OP.
						continue;
					}
					case EE_N_STRING_TOKENIZE_IDENT : {
						//EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						++sParmIdx;							// Fake push so we can fill in soProcessMe.sSubResults[0] manually later.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = TOKENIZER.
						EE_PUSH( _ndExp.w.sNodeIndex );		// soProcessMe.sSubResults[2] = INCLUDE_EMPTY.
						continue;
					}
					case EE_N_STRING_TOKENIZE : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = OBJ.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = TOKENIZER.
						EE_PUSH( _ndExp.w.sNodeIndex );		// soProcessMe.sSubResults[2] = INCLUDE_EMPTY.
						continue;
					}

					case EE_N_BARTHANN : {}					EE_FALLTHROUGH
					case EE_N_BARTLETT : {}					EE_FALLTHROUGH
					case EE_N_BLACKMAN : {}					EE_FALLTHROUGH
					case EE_N_BLACKMANHARRIS : {}			EE_FALLTHROUGH
					case EE_N_BLACKMANNUTTAL : {}			EE_FALLTHROUGH
					case EE_N_BOHMAN : {}					EE_FALLTHROUGH
					case EE_N_BOXCAR : {}					EE_FALLTHROUGH
					case EE_N_COSINE : {}					EE_FALLTHROUGH
					case EE_N_FLATTOP : {}					EE_FALLTHROUGH
					case EE_N_HANN : {}						EE_FALLTHROUGH
					case EE_N_HAMMING : {}					EE_FALLTHROUGH
					case EE_N_LANCZOS : {}					EE_FALLTHROUGH
					case EE_N_NUTTAL : {}					EE_FALLTHROUGH
					case EE_N_PARZEN : {}					EE_FALLTHROUGH
					case EE_N_TRIANG : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = N.
						continue;
					}
					case EE_N_CHEBWIN : {}					EE_FALLTHROUGH
					case EE_N_GAUSSIAN : {}					EE_FALLTHROUGH
					case EE_N_GENERAL_COSINE : {}			EE_FALLTHROUGH
					case EE_N_GENERAL_HAMMING : {}			EE_FALLTHROUGH
					case EE_N_KAISER : {}					EE_FALLTHROUGH
					case EE_N_KAISER_BESSEL : {}			EE_FALLTHROUGH
					case EE_N_TUKEY : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = N.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = BETA.
						continue;
					}
					case EE_N_EXPONENTIAL : {}				EE_FALLTHROUGH
					case EE_N_GENERAL_GAUSSIAN : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = N.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = P.
						EE_PUSH( _ndExp.x.sNodeIndex );		// soProcessMe.sSubResults[2] = SIGMA.
						continue;
					}

					case EE_N_TAYLOR : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = N.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = NBAR.
						EE_PUSH( _ndExp.x.sNodeIndex );		// soProcessMe.sSubResults[2] = SLL.
						EE_PUSH( _ndExp.y.sNodeIndex );		// soProcessMe.sSubResults[3] = NORMALIZE.
						continue;
					}

					case EE_N_TRAPEZOID_1 : {}				EE_FALLTHROUGH
					case EE_N_SIMPSON_1 : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = PARM0.
						continue;
					}
					case EE_N_TRAPEZOID_2 : {}				EE_FALLTHROUGH
					case EE_N_SIMPSON_2 : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = PARM0.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = PARM1.
						continue;
					}
					case EE_N_TRAPEZOIDSTRIDED_4 : {}		EE_FALLTHROUGH
					case EE_N_SIMPSONSTRIDED_4 : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = PARM0.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = PARM1.
						EE_PUSH( _ndExp.w.sNodeIndex );		// soProcessMe.sSubResults[2] = PARM2.
						EE_PUSH( _ndExp.x.sNodeIndex );		// soProcessMe.sSubResults[3] = PARM3.
						continue;
					}
					case EE_N_TRAPEZOIDSTRIDED_5 : {}		EE_FALLTHROUGH
					case EE_N_SIMPSONSTRIDED_5 : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = PARM0.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = PARM1.
						EE_PUSH( _ndExp.w.sNodeIndex );		// soProcessMe.sSubResults[2] = PARM2.
						EE_PUSH( _ndExp.x.sNodeIndex );		// soProcessMe.sSubResults[3] = PARM3.
						EE_PUSH( _ndExp.y.sNodeIndex );		// soProcessMe.sSubResults[4] = PARM4.
						continue;
					}

					case EE_N_LINSPACE_2 : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = PARM0.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = PARM1.
						continue;
					}
					case EE_N_LINSPACE_3 : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = PARM0.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = PARM1.
						EE_PUSH( _ndExp.x.sNodeIndex );		// soProcessMe.sSubResults[2] = PARM2.
						continue;
					}
					case EE_N_LINSPACE_4 : {
						EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = PARM0.
						EE_PUSH( _ndExp.v.sNodeIndex );		// soProcessMe.sSubResults[1] = PARM1.
						EE_PUSH( _ndExp.x.sNodeIndex );		// soProcessMe.sSubResults[2] = PARM2.
						EE_PUSH( _ndExp.y.sNodeIndex );		// soProcessMe.sSubResults[3] = PARM3.
						continue;
					}
				}
			}
			else {
				switch ( _ndExp.nType ) {
					case EE_N_ARRAY : {
						EE_RESULT rNode = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( !m_vArrayData[_ndExp.u.sStringIndex].m_pabBase->ReadValue( static_cast<size_t>(rNode.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_BADARRAYIDX ); }
						break;
					}
					case EE_N_STRINGARRAY : {
						EE_RESULT rNode = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.ui64Val = ee::CExpEval::GetUtf8CodePointByIdx( m_vStrings[_ndExp.u.sStringIndex], static_cast<size_t>(rNode.u.ui64Val) );
						EE_DONE;
					}
					case EE_N_STRINGARRAY_EX : {
						//if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT || !soProcessMe.sSubResults[0].u.poObj ) { EE_ERROR( EE_EC_ARRAY_FROM_NON_OBJECT ); }
						(*soProcessMe.prResult).ncType = EE_NC_INVALID;
						int64_t i64CodePoints = -1;

						uint32_t ui32Flags = 0;
						EE_RESULT rIdx;
						int64_t i64StartIdx = 0;
						size_t sStart = 0;
						if ( soProcessMe.sSubResults[1].ncType != EE_NC_INVALID ) {
							rIdx = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_SIGNED );
							if ( rIdx.ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_INVALIDCAST ); }
							i64StartIdx = soProcessMe.sSubResults[0].u.i64Val;

							if ( i64StartIdx < 0 ) {
								if ( i64CodePoints == -1 ) {
									i64CodePoints = ee::CExpEval::CountUtfCodePoints( m_vStrings[_ndExp.u.sStringIndex] );
									if ( int64_t( static_cast<size_t>(i64CodePoints) ) != i64CodePoints ) { EE_ERROR( EE_EC_BADARRAYIDX ); }
								}
								sStart = CBaseApi::ArrayIndexToLinearIndex( i64StartIdx, static_cast<size_t>(i64CodePoints) );
							}
							else {
								sStart = static_cast<size_t>(i64StartIdx);
								if ( static_cast<int64_t>(sStart) != i64StartIdx ) { EE_ERROR( EE_EC_BADARRAYIDX ); }
							}
							
							//if ( sStart == EE_INVALID_IDX ) { sStart = m_vStrings[_ndExp.u.sStringIndex].size(); }
						}
						ui32Flags |= CObject::EE_AEF_START;
						int64_t i64EndIdx = -1;
						size_t sEnd;
						if ( soProcessMe.sSubResults[2].ncType != EE_NC_INVALID ) {
							rIdx = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_SIGNED );
							if ( rIdx.ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_INVALIDCAST ); }
							i64EndIdx = soProcessMe.sSubResults[1].u.i64Val;
							if ( i64EndIdx < 0 ) {
								if ( i64CodePoints == -1 ) {
									i64CodePoints = ee::CExpEval::CountUtfCodePoints( m_vStrings[_ndExp.u.sStringIndex] );
									if ( int64_t( static_cast<size_t>(i64CodePoints) ) != i64CodePoints ) { EE_ERROR( EE_EC_BADARRAYIDX ); }
								}
								sEnd = CBaseApi::ArrayIndexToLinearIndex( i64EndIdx, static_cast<size_t>(i64CodePoints) );
								//if ( sEnd == EE_INVALID_IDX ) { sEnd = 0; }
							}
							else {
								sEnd = static_cast<size_t>(i64EndIdx);
								if ( static_cast<int64_t>(sEnd) != i64EndIdx ) { EE_ERROR( EE_EC_BADARRAYIDX ); }
							}
							

							sEnd = ee::CExpEval::GetUtf8CharPosByIdx( m_vStrings[_ndExp.u.sStringIndex], sEnd );
						}
						else {
							sEnd = m_vStrings[_ndExp.u.sStringIndex].size();
						}

						sStart = ee::CExpEval::GetUtf8CharPosByIdx( m_vStrings[_ndExp.u.sStringIndex], sStart );
						//(*soProcessMe.prResult) = soProcessMe.sSubResults[0].u.poObj->ArrayAccessEx( i64StartIdx, i64EndIdx, ui32Flags );
						CString * psObj = reinterpret_cast<CString *>(AllocateObject<CString>());
						if ( psObj ) {
							std::string sTmp;
							for ( size_t I = sStart; I < sEnd; ++I ) {
								sTmp.push_back( m_vStrings[_ndExp.u.sStringIndex][I] );
							}
							psObj->SetString( sTmp );
							(*soProcessMe.prResult) = psObj->CreateResult();
						}
						if ( (*soProcessMe.prResult).ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_DONE;
					}
					case EE_N_ARRAY_ACCESS : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT || !soProcessMe.sSubResults[0].u.poObj ) { EE_ERROR( EE_EC_ARRAY_FROM_NON_OBJECT ); }

						EE_RESULT rIdx;
						rIdx = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_SIGNED );
						if ( rIdx.ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_INVALIDCAST ); }

						(*soProcessMe.prResult) = soProcessMe.sSubResults[0].u.poObj->ArrayAccess( soProcessMe.sSubResults[1].u.i64Val );
						if ( (*soProcessMe.prResult).ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_DONE;
					}
					case EE_N_ARRAY_ACCESS_EX : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT || !soProcessMe.sSubResults[0].u.poObj ) { EE_ERROR( EE_EC_ARRAY_FROM_NON_OBJECT ); }

						uint32_t ui32Flags = 0;
						EE_RESULT rIdx;
						int64_t i64StartIdx = 0;
						if ( soProcessMe.sSubResults[1].ncType != EE_NC_INVALID ) {
							rIdx = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_SIGNED );
							if ( rIdx.ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_INVALIDCAST ); }
							i64StartIdx = soProcessMe.sSubResults[1].u.i64Val;
						}
						ui32Flags |= CObject::EE_AEF_START;
						int64_t i64EndIdx = -1;
						if ( soProcessMe.sSubResults[2].ncType != EE_NC_INVALID ) {
							rIdx = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_SIGNED );
							if ( rIdx.ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_INVALIDCAST ); }
							i64EndIdx = soProcessMe.sSubResults[2].u.i64Val;
							ui32Flags |= CObject::EE_AEF_END;
						}

						(*soProcessMe.prResult) = soProcessMe.sSubResults[0].u.poObj->ArrayAccessEx( i64StartIdx, i64EndIdx, ui32Flags );
						if ( (*soProcessMe.prResult).ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_DONE;
					}
					case EE_N_ADDRESS : {
						EE_RESULT rNode = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( !m_pfahAddressHandler( rNode.u.ui64Val, _ndExp.v.ctCast, m_uiptrAddressData, this, (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_ADDRESSHANDLERFAILED ); }
						break;
					}
					case EE_N_MEMBERACCESS : {
						if ( !m_pfmahMemberAccess( soProcessMe.sSubResults[0], m_vStrings[_ndExp.v.sStringIndex], m_uiptrMemberAccess, this, (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_MEMBERHANDELRFAILED ); }
						break;
					}
					case EE_N_DYNAMIC_NUMBERED_PARM_EXP : {
						EE_RESULT rTemp = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( rTemp.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult) = GetNumberedParm( static_cast<size_t>(rTemp.u.ui64Val) );
						EE_DONE;
					}
					case EE_N_UNARY : {
						EE_ERROR_CODES ecError = PerformUnary( soProcessMe.sSubResults[0], _ndExp.v.ui32Op, (*soProcessMe.prResult) );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_CAST : {
						EE_RESULT & rNode = soProcessMe.sSubResults[0];

						switch ( _ndExp.v.ctCast ) {
#define EE_CAST( TYPE, CASTTYPE, MEMBER )																									\
	(*soProcessMe.prResult).ncType = TYPE;																									\
	(*soProcessMe.prResult).u.MEMBER = static_cast<CASTTYPE>(ConvertResultOrObject( rNode, (*soProcessMe.prResult).ncType ).u.MEMBER);		\
	if ( (*soProcessMe.prResult).ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_INVALIDCAST ); }												\
	EE_DONE;
							case EE_CT_INT8 : { EE_CAST( EE_NC_SIGNED, int8_t, i64Val ) }
							case EE_CT_INT16 : { EE_CAST( EE_NC_SIGNED, int16_t, i64Val ) }
							case EE_CT_INT32 : { EE_CAST( EE_NC_SIGNED, int32_t, i64Val ) }
							case EE_CT_INT64 : { EE_CAST( EE_NC_SIGNED, int64_t, i64Val ) }
							case EE_CT_UINT8 : { EE_CAST( EE_NC_UNSIGNED, uint8_t, ui64Val ) }
							case EE_CT_UINT16 : { EE_CAST( EE_NC_UNSIGNED, uint16_t, ui64Val ) }
							case EE_CT_UINT32 : { EE_CAST( EE_NC_UNSIGNED, uint32_t, ui64Val ) }
							case EE_CT_UINT64 : { EE_CAST( EE_NC_UNSIGNED, uint64_t, ui64Val ) }
							case EE_CT_FLOAT : { EE_CAST( EE_NC_FLOATING, float, dVal ) }
							case EE_CT_DOUBLE : { EE_CAST( EE_NC_FLOATING, double, dVal ) }
#undef EE_CAST
							case EE_CT_FLOAT10 : {
								rNode = ConvertResultOrObject( rNode, EE_NC_FLOATING );
								if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
								CFloatX fTemp;
								fTemp.CreateFromDouble( rNode.u.dVal, 5, 6, true, false );
								(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
								(*soProcessMe.prResult).u.dVal = fTemp.AsDouble();
								EE_DONE;
							}
							case EE_CT_FLOAT11 : {
								rNode = ConvertResultOrObject( rNode, EE_NC_FLOATING );
								if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
								CFloatX fTemp;
								fTemp.CreateFromDouble( rNode.u.dVal, 5, 7, true, false );
								(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
								(*soProcessMe.prResult).u.dVal = fTemp.AsDouble();
								EE_DONE;
							}
							case EE_CT_FLOAT14 : {
								rNode = ConvertResultOrObject( rNode, EE_NC_FLOATING );
								if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
								CFloatX fTemp;
								fTemp.CreateFromDouble( rNode.u.dVal, 5, 10, true, false );
								(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
								(*soProcessMe.prResult).u.dVal = fTemp.AsDouble();
								EE_DONE;
							}
							case EE_CT_FLOAT16 : {
								rNode = ConvertResultOrObject( rNode, EE_NC_FLOATING );
								if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
								CFloatX fTemp;
								fTemp.CreateFromDouble( rNode.u.dVal, 5, 11, true, true );
								(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
								(*soProcessMe.prResult).u.dVal = fTemp.AsDouble();
								EE_DONE;
							}
						}
						EE_ERROR( EE_EC_INVALIDTREE );
					}
					case EE_N_OP : {
						EE_ERROR_CODES ecRet = PerformOp( soProcessMe.sSubResults[0], _ndExp.v.ui32Op, soProcessMe.sSubResults[1], (*soProcessMe.prResult) );
						if ( ecRet != EE_ERROR_CODES::EE_EC_SUCCESS ) { EE_ERROR( ecRet ); }
						EE_DONE;
					}
					case EE_N_INTRINSIC_1 : {
						EE_ERROR_CODES ecError = PerformIntrinsic( soProcessMe.sSubResults[0], _ndExp.u.ui32Intrinsic, (*soProcessMe.prResult) );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_INTRINSIC_1_FLOAT_FLOAT : {
						if ( soProcessMe.sSubResults[0].ncType == EE_NC_OBJECT && soProcessMe.sSubResults[0].u.poObj && (soProcessMe.sSubResults[0].u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(soProcessMe.sSubResults[0].u.poObj);
							for ( auto I = pvThis->GetBacking().size(); I--; ) {
								EE_RESULT rNode = ConvertResultOrObject( pvThis->GetBacking()[I], EE_NC_FLOATING );
								if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
								pvThis->GetBacking()[I].ncType = EE_NC_FLOATING;
								pvThis->GetBacking()[I].u.dVal = _ndExp.uFuncPtr.pfIntrins1Float_Float( rNode.u.dVal );
							}
							(*soProcessMe.prResult) = pvThis->CreateResult();
							EE_DONE;
						}
						EE_RESULT rNode = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_FLOATING );
						if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						(*soProcessMe.prResult).u.dVal = _ndExp.uFuncPtr.pfIntrins1Float_Float( rNode.u.dVal );
						EE_DONE;
					}
					case EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED16 : {
						if ( soProcessMe.sSubResults[0].ncType == EE_NC_OBJECT && soProcessMe.sSubResults[0].u.poObj && (soProcessMe.sSubResults[0].u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(soProcessMe.sSubResults[0].u.poObj);
							for ( auto I = pvThis->GetBacking().size(); I--; ) {
								EE_RESULT rNode = ConvertResultOrObject( pvThis->GetBacking()[I], EE_NC_UNSIGNED );
								if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
								pvThis->GetBacking()[I].ncType = EE_NC_UNSIGNED;
								pvThis->GetBacking()[I].u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Unsigned_Unsigned16( static_cast<uint16_t>(rNode.u.ui64Val) );
							}
							(*soProcessMe.prResult) = pvThis->CreateResult();
							EE_DONE;
						}
						EE_RESULT rNode = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Unsigned_Unsigned16( static_cast<uint16_t>(rNode.u.ui64Val) );
						EE_DONE;
					}
					case EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED32 : {
						if ( soProcessMe.sSubResults[0].ncType == EE_NC_OBJECT && soProcessMe.sSubResults[0].u.poObj && (soProcessMe.sSubResults[0].u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(soProcessMe.sSubResults[0].u.poObj);
							for ( auto I = pvThis->GetBacking().size(); I--; ) {
								EE_RESULT rNode = ConvertResultOrObject( pvThis->GetBacking()[I], EE_NC_UNSIGNED );
								if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
								pvThis->GetBacking()[I].ncType = EE_NC_UNSIGNED;
								pvThis->GetBacking()[I].u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Unsigned_Unsigned32( static_cast<uint32_t>(rNode.u.ui64Val) );
							}
							(*soProcessMe.prResult) = pvThis->CreateResult();
							EE_DONE;
						}
						EE_RESULT rNode = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Unsigned_Unsigned32( static_cast<uint32_t>(rNode.u.ui64Val) );
						EE_DONE;
					}
					case EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED64 : {
						if ( soProcessMe.sSubResults[0].ncType == EE_NC_OBJECT && soProcessMe.sSubResults[0].u.poObj && (soProcessMe.sSubResults[0].u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(soProcessMe.sSubResults[0].u.poObj);
							for ( auto I = pvThis->GetBacking().size(); I--; ) {
								EE_RESULT rNode = ConvertResultOrObject( pvThis->GetBacking()[I], EE_NC_UNSIGNED );
								if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
								pvThis->GetBacking()[I].ncType = EE_NC_UNSIGNED;
								pvThis->GetBacking()[I].u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Unsigned_Unsigned64( static_cast<uint64_t>(rNode.u.ui64Val) );
							}
							(*soProcessMe.prResult) = pvThis->CreateResult();
							EE_DONE;
						}
						EE_RESULT rNode = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Unsigned_Unsigned64( static_cast<uint64_t>(rNode.u.ui64Val) );
						EE_DONE;
					}
					case EE_N_INTRINSIC_1_BOOL_FLOAT : {
						if ( soProcessMe.sSubResults[0].ncType == EE_NC_OBJECT && soProcessMe.sSubResults[0].u.poObj && (soProcessMe.sSubResults[0].u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(soProcessMe.sSubResults[0].u.poObj);
							for ( auto I = pvThis->GetBacking().size(); I--; ) {
								EE_RESULT rNode = ConvertResultOrObject( pvThis->GetBacking()[I], EE_NC_FLOATING );
								if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
								pvThis->GetBacking()[I].ncType = EE_NC_UNSIGNED;
								pvThis->GetBacking()[I].u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Bool_Float( rNode.u.dVal );
							}
							(*soProcessMe.prResult) = pvThis->CreateResult();
							EE_DONE;
						}
						EE_RESULT rNode = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_FLOATING );
						if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Bool_Float( rNode.u.dVal );
						EE_DONE;
					}
					case EE_N_INTRINSIC_1_INT_FLOAT : {
						if ( soProcessMe.sSubResults[0].ncType == EE_NC_OBJECT && soProcessMe.sSubResults[0].u.poObj && (soProcessMe.sSubResults[0].u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(soProcessMe.sSubResults[0].u.poObj);
							for ( auto I = pvThis->GetBacking().size(); I--; ) {
								EE_RESULT rNode = ConvertResultOrObject( pvThis->GetBacking()[I], EE_NC_FLOATING );
								if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
								pvThis->GetBacking()[I].ncType = EE_NC_SIGNED;
								pvThis->GetBacking()[I].u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Signed_Float( rNode.u.dVal );
							}
							(*soProcessMe.prResult) = pvThis->CreateResult();
							EE_DONE;
						}
						EE_RESULT rNode = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_FLOATING );
						if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_SIGNED;
						(*soProcessMe.prResult).u.ui64Val = _ndExp.uFuncPtr.pfIntrins1Signed_Float( rNode.u.dVal );
						EE_DONE;
					}
					case EE_N_INTRINSIC_2 : {
						EE_ERROR_CODES ecError = PerformIntrinsic( soProcessMe.sSubResults[0], soProcessMe.sSubResults[1], _ndExp.u.ui32Intrinsic, (*soProcessMe.prResult), true );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_INTRINSIC_2_FLOAT_FLOAT_FLOAT : {
						EE_RESULT rNode0 = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_FLOATING );
						if ( rNode0.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_RESULT rNode1 = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( rNode1.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						(*soProcessMe.prResult).u.dVal = _ndExp.uFuncPtr.pfIntrins2Float_Float_Float( rNode0.u.dVal,
							rNode1.u.dVal );
						EE_DONE;
					}
					case EE_N_INTRINSIC_2_FLOAT_FLOAT32_FLOAT32 : {
						EE_RESULT rNode0 = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_FLOATING );
						if ( rNode0.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_RESULT rNode1 = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( rNode1.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						(*soProcessMe.prResult).u.dVal = _ndExp.uFuncPtr.pfIntrins2Float32_Float32_Float32( static_cast<float>(rNode0.u.dVal),
							static_cast<float>(rNode1.u.dVal) );
						EE_DONE;
					}
					case EE_N_INTRINSIC_2_FLOAT_FLOAT80_FLOAT80 : {
						EE_RESULT rNode0 = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_FLOATING );
						if ( rNode0.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_RESULT rNode1 = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( rNode1.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						(*soProcessMe.prResult).u.dVal = double( _ndExp.uFuncPtr.pfIntrins2Float80_Float80_Float80( rNode0.u.dVal,
							rNode1.u.dVal ) );
						EE_DONE;
					}
					case EE_N_INTRINSIC_2_FLOAT_FLOAT32_FLOAT80 : {
						EE_RESULT rNode0 = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_FLOATING );
						if ( rNode0.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_RESULT rNode1 = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( rNode1.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						(*soProcessMe.prResult).u.dVal = _ndExp.uFuncPtr.pfIntrins2Float32_Float32_Float80( static_cast<float>(rNode0.u.dVal),
							rNode1.u.dVal );
						EE_DONE;
					}
					case EE_N_INTRINSIC_3 : {
						EE_ERROR_CODES ecError = PerformIntrinsic( soProcessMe.sSubResults[0], soProcessMe.sSubResults[1], soProcessMe.sSubResults[2], _ndExp.u.ui32Intrinsic, (*soProcessMe.prResult) );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_INTRINSIC_3_FLOAT_FLOAT_FLOAT_FLOAT : {
						EE_RESULT rNode0 = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_FLOATING );
						if ( rNode0.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_RESULT rNode1 = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( rNode1.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_RESULT rNode2 = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_FLOATING );
						if ( rNode2.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						(*soProcessMe.prResult).u.dVal = _ndExp.uFuncPtr.pfIntrins3Float_Float_Float_Float( rNode0.u.dVal,
							rNode1.u.dVal,
							rNode2.u.dVal );
						EE_DONE;
					}
					case EE_N_INTRINSIC_3_BOOL_FLOAT32_FLOAT32_FLOAT32 : {
						EE_RESULT rNode0 = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_FLOATING );
						if ( rNode0.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_RESULT rNode1 = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( rNode1.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_RESULT rNode2 = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_FLOATING );
						if ( rNode2.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_SIGNED;
						(*soProcessMe.prResult).u.i64Val = _ndExp.uFuncPtr.pfIntrins3Bool_Float32_Float32_Float32( static_cast<float>(rNode0.u.dVal),
							static_cast<float>(rNode1.u.dVal),
							static_cast<float>(rNode2.u.dVal) );
						EE_DONE;
					}
					case EE_N_INTRINSIC_3_BOOL_FLOAT_FLOAT_FLOAT : {
						EE_RESULT rNode0 = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_FLOATING );
						if ( rNode0.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_RESULT rNode1 = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( rNode1.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						EE_RESULT rNode2 = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_FLOATING );
						if ( rNode2.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						(*soProcessMe.prResult).ncType = EE_NC_SIGNED;
						(*soProcessMe.prResult).u.i64Val = _ndExp.uFuncPtr.pfIntrins3Bool_Float_Float_Float( rNode0.u.dVal,
							rNode1.u.dVal,
							rNode2.u.dVal );
						EE_DONE;
					}
					case EE_N_ASFLOAT : {
						EE_ERROR_CODES ecError = PerformAsFloat( soProcessMe.sSubResults[0], (*soProcessMe.prResult) );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_ASDOUBLE : {
						EE_ERROR_CODES ecError = PerformAsDouble( soProcessMe.sSubResults[0], (*soProcessMe.prResult) );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_ASXFLOAT : {
						EE_ERROR_CODES ecError = PerformFloatX( soProcessMe.sSubResults[0], soProcessMe.sSubResults[1],
							soProcessMe.sSubResults[2], soProcessMe.sSubResults[3], soProcessMe.sSubResults[4], soProcessMe.sSubResults[5], soProcessMe.sSubResults[6],
							(*soProcessMe.prResult) );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_ASXFLOAT_FROM_DOUBLE : {
						EE_ERROR_CODES ecError = PerformFloatX( soProcessMe.sSubResults[0], soProcessMe.sSubResults[1],
							soProcessMe.sSubResults[2], soProcessMe.sSubResults[3], soProcessMe.sSubResults[4],
							(*soProcessMe.prResult) );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_ASFLOAT16 : {
						EE_ERROR_CODES ecError = PerformToFloat16( soProcessMe.sSubResults[0], (*soProcessMe.prResult) );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_ASFLOAT14 : {
						EE_ERROR_CODES ecError = PerformToFloat14( soProcessMe.sSubResults[0], (*soProcessMe.prResult) );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_ASFLOAT11 : {
						EE_ERROR_CODES ecError = PerformToFloat11( soProcessMe.sSubResults[0], (*soProcessMe.prResult) );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_ASFLOAT10 : {
						EE_ERROR_CODES ecError = PerformToFloat10( soProcessMe.sSubResults[0], (*soProcessMe.prResult) );
						if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }
						EE_DONE;
					}
					case EE_N_ASXFLOAT_MAX : {
						EE_RESULT & rTempSignBits = soProcessMe.sSubResults[0];
						EE_RESULT & rTempExpBits = soProcessMe.sSubResults[1];
						EE_RESULT & rTempManBits = soProcessMe.sSubResults[2];
						EE_RESULT & rTempImplied = soProcessMe.sSubResults[3];

						rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
						if ( rTempSignBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { EE_ERROR( EE_EC_TOOMANYSIGNBITS ); }
						rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
						if ( rTempExpBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { EE_ERROR( EE_EC_TOOMANYEXPBITS ); }
						rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
						if ( rTempManBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { EE_ERROR( EE_EC_TOOMANYMANTISSABITS ); }
						rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
						if ( rTempImplied.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						(*soProcessMe.prResult).u.dVal = CFloatX::GetMaxForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
							rTempImplied.u.ui64Val != 0 );
						break;
					}
					case EE_N_ASXFLOAT_MIN : {
						EE_RESULT & rTempSignBits = soProcessMe.sSubResults[0];
						EE_RESULT & rTempExpBits = soProcessMe.sSubResults[1];
						EE_RESULT & rTempManBits = soProcessMe.sSubResults[2];
						EE_RESULT & rTempImplied = soProcessMe.sSubResults[3];

						rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
						if ( rTempSignBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { EE_ERROR( EE_EC_TOOMANYSIGNBITS ); }
						rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
						if ( rTempExpBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { EE_ERROR( EE_EC_TOOMANYEXPBITS ); }
						rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
						if ( rTempManBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { EE_ERROR( EE_EC_TOOMANYMANTISSABITS ); }
						rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
						if ( rTempImplied.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						(*soProcessMe.prResult).u.dVal = CFloatX::GetNormalizedMinForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
							rTempImplied.u.ui64Val != 0 );
						break;
					}
					case EE_N_ASXFLOAT_TRUE_MIN : {
						EE_RESULT & rTempSignBits = soProcessMe.sSubResults[0];
						EE_RESULT & rTempExpBits = soProcessMe.sSubResults[1];
						EE_RESULT & rTempManBits = soProcessMe.sSubResults[2];
						EE_RESULT & rTempImplied = soProcessMe.sSubResults[3];

						rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
						if ( rTempSignBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { EE_ERROR( EE_EC_TOOMANYSIGNBITS ); }
						rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
						if ( rTempExpBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { EE_ERROR( EE_EC_TOOMANYEXPBITS ); }
						rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
						if ( rTempManBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { EE_ERROR( EE_EC_TOOMANYMANTISSABITS ); }
						rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
						if ( rTempImplied.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						(*soProcessMe.prResult).u.dVal = CFloatX::GetMinForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
							rTempImplied.u.ui64Val != 0 );
						break;
					}
					case EE_N_ASXFLOAT_NAN : {
						EE_RESULT & rTempSignBits = soProcessMe.sSubResults[0];
						EE_RESULT & rTempExpBits = soProcessMe.sSubResults[1];
						EE_RESULT & rTempManBits = soProcessMe.sSubResults[2];
						EE_RESULT & rTempImplied = soProcessMe.sSubResults[3];

						rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
						if ( rTempSignBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { EE_ERROR( EE_EC_TOOMANYSIGNBITS ); }
						rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
						if ( rTempExpBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { EE_ERROR( EE_EC_TOOMANYEXPBITS ); }
						rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
						if ( rTempManBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { EE_ERROR( EE_EC_TOOMANYMANTISSABITS ); }
						rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
						if ( rTempImplied.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						CFloatX fTemp;
						fTemp.CreateNaN( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
							rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.ui64Val = fTemp.AsUint64();
						break;
					}
					case EE_N_ASXFLOAT_INF : {
						EE_RESULT & rTempSignBits = soProcessMe.sSubResults[0];
						EE_RESULT & rTempExpBits = soProcessMe.sSubResults[1];
						EE_RESULT & rTempManBits = soProcessMe.sSubResults[2];
						EE_RESULT & rTempImplied = soProcessMe.sSubResults[3];

						rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
						if ( rTempSignBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { EE_ERROR( EE_EC_TOOMANYSIGNBITS ); }
						rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
						if ( rTempExpBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { EE_ERROR( EE_EC_TOOMANYEXPBITS ); }
						rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
						if ( rTempManBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { EE_ERROR( EE_EC_TOOMANYMANTISSABITS ); }
						rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
						if ( rTempImplied.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						CFloatX fTemp;
						fTemp.CreateInfP( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
							rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.ui64Val = fTemp.AsUint64();
						break;
					}
					case EE_N_ASXFLOAT_SUBNORM_MAX : {
						EE_RESULT & rTempSignBits = soProcessMe.sSubResults[0];
						EE_RESULT & rTempExpBits = soProcessMe.sSubResults[1];
						EE_RESULT & rTempManBits = soProcessMe.sSubResults[2];
						EE_RESULT & rTempImplied = soProcessMe.sSubResults[3];

						rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
						if ( rTempSignBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { EE_ERROR( EE_EC_TOOMANYSIGNBITS ); }
						rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
						if ( rTempExpBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { EE_ERROR( EE_EC_TOOMANYEXPBITS ); }
						rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
						if ( rTempManBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { EE_ERROR( EE_EC_TOOMANYMANTISSABITS ); }
						rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
						if ( rTempImplied.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						(*soProcessMe.prResult).u.dVal = CFloatX::GetDenormalizedMaxForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
							rTempImplied.u.ui64Val != 0 );
						break;
					}
					case EE_N_ASXFLOAT_EPS : {
						EE_RESULT & rTempSignBits = soProcessMe.sSubResults[0];
						EE_RESULT & rTempExpBits = soProcessMe.sSubResults[1];
						EE_RESULT & rTempManBits = soProcessMe.sSubResults[2];
						EE_RESULT & rTempImplied = soProcessMe.sSubResults[3];

						rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
						if ( rTempSignBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { EE_ERROR( EE_EC_TOOMANYSIGNBITS ); }
						rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
						if ( rTempExpBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { EE_ERROR( EE_EC_TOOMANYEXPBITS ); }
						rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
						if ( rTempManBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { EE_ERROR( EE_EC_TOOMANYMANTISSABITS ); }
						rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
						if ( rTempImplied.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						CFloatX fOne, fNextUp;
						fOne.CreateFromDouble( 1.0, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
							rTempImplied.u.ui64Val != 0 );
						fNextUp.CreateFromParts( fOne.bSign, fOne.uiExponent, fOne.uiMantissa + 1, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
							rTempImplied.u.ui64Val != 0 );

						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						(*soProcessMe.prResult).u.dVal = fNextUp.AsDouble() - fOne.AsDouble();
						break;
					}
					case EE_N_ASXFLOAT_SIGNBIT : {
						EE_RESULT & rTempSignBits = soProcessMe.sSubResults[0];
						EE_RESULT & rTempExpBits = soProcessMe.sSubResults[1];
						EE_RESULT & rTempManBits = soProcessMe.sSubResults[2];
						EE_RESULT & rTempImplied = soProcessMe.sSubResults[3];
						EE_RESULT & rTempDoubleVal = soProcessMe.sSubResults[4];

						rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
						if ( rTempSignBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { EE_ERROR( EE_EC_TOOMANYSIGNBITS ); }
						rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
						if ( rTempExpBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { EE_ERROR( EE_EC_TOOMANYEXPBITS ); }
						rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
						if ( rTempManBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { EE_ERROR( EE_EC_TOOMANYMANTISSABITS ); }
						rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
						if ( rTempImplied.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						rTempDoubleVal = ConvertResultOrObject( rTempDoubleVal, EE_NC_FLOATING );
						if ( rTempDoubleVal.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }


						CFloatX fTemp;
						fTemp.CreateFromDouble( rTempDoubleVal.u.dVal, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
							rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.ui64Val = fTemp.SignBit();
						break;
					}
					case EE_N_ASXFLOAT_EXPBITS : {
						EE_RESULT & rTempSignBits = soProcessMe.sSubResults[0];
						EE_RESULT & rTempExpBits = soProcessMe.sSubResults[1];
						EE_RESULT & rTempManBits = soProcessMe.sSubResults[2];
						EE_RESULT & rTempImplied = soProcessMe.sSubResults[3];
						EE_RESULT & rTempDoubleVal = soProcessMe.sSubResults[4];

						rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
						if ( rTempSignBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { EE_ERROR( EE_EC_TOOMANYSIGNBITS ); }
						rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
						if ( rTempExpBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { EE_ERROR( EE_EC_TOOMANYEXPBITS ); }
						rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
						if ( rTempManBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { EE_ERROR( EE_EC_TOOMANYMANTISSABITS ); }
						rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
						if ( rTempImplied.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						rTempDoubleVal = ConvertResultOrObject( rTempDoubleVal, EE_NC_FLOATING );
						if ( rTempDoubleVal.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }


						CFloatX fTemp;
						fTemp.CreateFromDouble( rTempDoubleVal.u.dVal, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
							rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.ui64Val = fTemp.ExpBits();
						break;
					}
					case EE_N_ASXFLOAT_MANBITS : {
						EE_RESULT & rTempSignBits = soProcessMe.sSubResults[0];
						EE_RESULT & rTempExpBits = soProcessMe.sSubResults[1];
						EE_RESULT & rTempManBits = soProcessMe.sSubResults[2];
						EE_RESULT & rTempImplied = soProcessMe.sSubResults[3];
						EE_RESULT & rTempDoubleVal = soProcessMe.sSubResults[4];

						rTempSignBits = ConvertResultOrObject( rTempSignBits, EE_NC_UNSIGNED );
						if ( rTempSignBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { EE_ERROR( EE_EC_TOOMANYSIGNBITS ); }
						rTempExpBits = ConvertResultOrObject( rTempExpBits, EE_NC_UNSIGNED );
						if ( rTempExpBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { EE_ERROR( EE_EC_TOOMANYEXPBITS ); }
						rTempManBits = ConvertResultOrObject( rTempManBits, EE_NC_UNSIGNED );
						if ( rTempManBits.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { EE_ERROR( EE_EC_TOOMANYMANTISSABITS ); }
						rTempImplied = ConvertResultOrObject( rTempImplied, EE_NC_UNSIGNED );
						if ( rTempImplied.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						rTempDoubleVal = ConvertResultOrObject( rTempDoubleVal, EE_NC_FLOATING );
						if ( rTempDoubleVal.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }


						CFloatX fTemp;
						fTemp.CreateFromDouble( rTempDoubleVal.u.dVal, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
							rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
						(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
						(*soProcessMe.prResult).u.ui64Val = fTemp.ManBits();
						break;
					}
					case EE_N_WHILE_LOOP : {
						if ( soProcessMe.uiLoopCounter < EE_MAX_ITERATION_COUNT ) {
							uint32_t uiStage = (uiProcessCount - 1) % 3;
							if ( uiStage == 0 ) {
								// Evaluate the condition.
								EE_PUSH( _ndExp.u.sNodeIndex );
								continue;
							}
							if ( uiStage == 1 ) {
								// Check the condition and optionally evaluate the statement.
								// ==== Check the loop condition.
								EE_RESULT & rTemp = soProcessMe.sSubResults[0];
								if ( (rTemp.ncType) == EE_NC_FLOATING ) {
									if ( !rTemp.u.dVal ) { EE_END_LOOP; }
								}
								else if ( (rTemp.ncType) == EE_NC_UNSIGNED ) {
									if ( !rTemp.u.ui64Val ) { EE_END_LOOP; }
								}
								else if ( (rTemp.ncType) == EE_NC_SIGNED ) {
									if ( !rTemp.u.i64Val ) { EE_END_LOOP; }
								}
								else { EE_ERROR( EE_EC_INVALILOOPCONDITIONEXP ); }
								// ==============================

								// ==== Execute body.
								EE_PUSH_EXP( _ndExp.v.sNodeIndex, soProcessMe.prResult );
								continue;
							}
							if ( uiStage == 2 ) {
								// After a successful iteration, adjust some state information.
								if ( m_vLoopStack[soProcessMe.sLoopStackIdx].bBreak ) { EE_END_LOOP; }
								m_vLoopStack[soProcessMe.sLoopStackIdx].bContinue = false;
								// ==================

								// Last.
								++m_vLoopStack[soProcessMe.sLoopStackIdx].sCurIdx;
								++soProcessMe.uiLoopCounter;
								continue;
							}
						}
						else {
							(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
							(*soProcessMe.prResult).u.ui64Val = 0;
						}

						EE_END_LOOP;
					}
					case EE_N_FOR_LOOP : {
						if ( soProcessMe.uiLoopCounter < EE_MAX_ITERATION_COUNT ) {
							uint32_t uiStage = (uiProcessCount - 1) % 4;
							if ( uiStage == 0 ) {
								// Evaluate the condition.
								EE_PUSH( _ndExp.v.sNodeIndex );
								continue;
							}
							if ( uiStage == 1 ) {
								// Check the condition and optionally evaluate the statement.
								// ==== Check the loop condition.
								EE_RESULT & rTemp = soProcessMe.sSubResults[0];
								if ( (rTemp.ncType) == EE_NC_FLOATING ) {
									if ( !rTemp.u.dVal ) { EE_END_LOOP; }
								}
								else if ( (rTemp.ncType) == EE_NC_UNSIGNED ) {
									if ( !rTemp.u.ui64Val ) { EE_END_LOOP; }
								}
								else if ( (rTemp.ncType) == EE_NC_SIGNED ) {
									if ( !rTemp.u.i64Val ) { EE_END_LOOP; }
								}
								else { EE_ERROR( EE_EC_INVALILOOPCONDITIONEXP ); }
								// ==============================

								// ==== Execute body.
								EE_PUSH_EXP( _ndExp.x.sNodeIndex, soProcessMe.prResult );
								continue;
							}
							if ( uiStage == 2 ) {
								// ==== Increment/Post-Loop Expression
								// Only gets executed if we did not break out.
								if ( m_vLoopStack[soProcessMe.sLoopStackIdx].bBreak ) { EE_END_LOOP; }
								if ( _ndExp.w.sNodeIndex != ~0 ) {
									EE_PUSH( _ndExp.w.sNodeIndex );
								}
								continue;
							}
							if ( uiStage == 3 ) {
								// After a successful iteration, adjust some state information.
								m_vLoopStack[soProcessMe.sLoopStackIdx].bContinue = false;
								// ==================

								// Last.
								++m_vLoopStack[soProcessMe.sLoopStackIdx].sCurIdx;
								++soProcessMe.uiLoopCounter;
								continue;
							}
						}
						else {
							(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
							(*soProcessMe.prResult).u.ui64Val = 0;
						}

						EE_END_LOOP;
					}
					case EE_N_DO_WHILE_LOOP : {
						if ( soProcessMe.uiLoopCounter < EE_MAX_ITERATION_COUNT ) {
							uint32_t uiStage = (uiProcessCount - 1) % 3;
							if ( uiStage == 0 ) {
								// ==== Execute body.
								EE_PUSH_EXP( _ndExp.v.sNodeIndex, soProcessMe.prResult );
								continue;
							}
							if ( uiStage == 1 ) {
								if ( m_vLoopStack[soProcessMe.sLoopStackIdx].bBreak ) { EE_END_LOOP; }
								m_vLoopStack[soProcessMe.sLoopStackIdx].bContinue = false;
								EE_PUSH( _ndExp.u.sNodeIndex );
								continue;
							}
							if ( uiStage == 2 ) {
								// ==== Check the loop condition.
								EE_RESULT & rTemp = soProcessMe.sSubResults[0];
								if ( (rTemp.ncType) == EE_NC_FLOATING ) {
									if ( !rTemp.u.dVal ) { EE_END_LOOP; }
								}
								else if ( (rTemp.ncType) == EE_NC_UNSIGNED ) {
									if ( !rTemp.u.ui64Val ) { EE_END_LOOP; }
								}
								else if ( (rTemp.ncType) == EE_NC_SIGNED ) {
									if ( !rTemp.u.i64Val ) { EE_END_LOOP; }
								}
								else { EE_ERROR( EE_EC_INVALILOOPCONDITIONEXP ); }
								// ==============================

								continue;
							}
						}
						else {
							(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
							(*soProcessMe.prResult).u.ui64Val = 0;
						}

						EE_END_LOOP;
					}
					case EE_N_FOREACH : {
						YYSTYPE::EE_NODE_DATA & ndDeclNode = m_vNodes[_ndExp.u.sNodeIndex];
						if ( soProcessMe.uiLoopCounter < m_vArrayData[ndDeclNode.v.sNodeIndex].m_pabBase->GetSize() ) {
							uint32_t uiStage = (uiProcessCount - 1) % 2;
							if ( uiStage == 0 ) {
								if ( !m_vArrayData[ndDeclNode.v.sNodeIndex].m_pabBase->ReadValue( static_cast<size_t>(soProcessMe.uiLoopCounter), (*soProcessMe.prLoopCustomVarResult) ) ) { EE_ERROR( EE_EC_ARRAYACCESSERROR ); }
								// ==== Execute body.
								EE_PUSH_EXP( _ndExp.v.sNodeIndex, soProcessMe.prResult );
								

								continue;
							}
							if ( uiStage == 1 ) {
								if ( m_vLoopStack[soProcessMe.sLoopStackIdx].bBreak ) { EE_END_LOOP; }
								m_vLoopStack[soProcessMe.sLoopStackIdx].bContinue = false;

								// Last.
								++m_vLoopStack[soProcessMe.sLoopStackIdx].sCurIdx;
								++soProcessMe.uiLoopCounter;
								continue;
							}
						}
						

						EE_END_LOOP;
					}
					case EE_N_FOREACHOBJ : {
						//YYSTYPE::EE_NODE_DATA & ndDeclNode = m_vNodes[_ndExp.u.sNodeIndex];
						if ( soProcessMe.uiLoopCounter < soProcessMe.prLoopObject->u.poObj->Len().u.ui64Val ) {
							uint32_t uiStage = (uiProcessCount - 1) % 2;
							if ( uiStage == 0 ) {
								//if ( !m_vArrayData[ndDeclNode.v.sNodeIndex].m_pabBase->ReadValue( static_cast<size_t>(soProcessMe.uiLoopCounter), (*soProcessMe.prLoopCustomVarResult) ) ) { EE_ERROR( EE_EC_ARRAYACCESSERROR ); }
								(*soProcessMe.prLoopCustomVarResult) = soProcessMe.prLoopObject->u.poObj->ArrayAccess( soProcessMe.uiLoopCounter );
								if ( (*soProcessMe.prLoopCustomVarResult).ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_ARRAYACCESSERROR ); }
								// ==== Execute body.
								EE_PUSH_EXP( _ndExp.v.sNodeIndex, soProcessMe.prResult );
								

								continue;
							}
							if ( uiStage == 1 ) {
								if ( m_vLoopStack[soProcessMe.sLoopStackIdx].bBreak ) { EE_END_LOOP; }
								m_vLoopStack[soProcessMe.sLoopStackIdx].bContinue = false;

								// Last.
								++m_vLoopStack[soProcessMe.sLoopStackIdx].sCurIdx;
								++soProcessMe.uiLoopCounter;
								continue;
							}
						}
						

						EE_END_LOOP;
					}
					case EE_N_FOREACHSTR : {
						YYSTYPE::EE_NODE_DATA & ndDeclNode = m_vNodes[_ndExp.u.sNodeIndex];
						if ( soProcessMe.sForEachStringPos < m_vStrings[soProcessMe.sForEachString].size() ) {
							uint32_t uiStage = (uiProcessCount - 1) % 2;
							if ( uiStage == 0 ) {
								size_t sSize;
								(*soProcessMe.prLoopCustomVarResult).ncType = EE_NC_UNSIGNED;
								size_t I = soProcessMe.sForEachStringPos;
								(*soProcessMe.prLoopCustomVarResult).u.ui64Val = ee::CExpEval::NextUtf8Char( &m_vStrings[ndDeclNode.v.sNodeIndex][I],
									m_vStrings[ndDeclNode.v.sNodeIndex].size() - I, &sSize );
								if ( !sSize ) {
									EE_ERROR( EE_EC_ARRAYACCESSERROR );
								}
								soProcessMe.sForEachStringPos += sSize;
								// ==== Execute body.
								EE_PUSH_EXP( _ndExp.v.sNodeIndex, soProcessMe.prResult );
								

								continue;
							}
							if ( uiStage == 1 ) {
								if ( m_vLoopStack[soProcessMe.sLoopStackIdx].bBreak ) { EE_END_LOOP; }
								m_vLoopStack[soProcessMe.sLoopStackIdx].bContinue = false;

								// Last.
								++m_vLoopStack[soProcessMe.sLoopStackIdx].sCurIdx;
								++soProcessMe.uiLoopCounter;
								continue;
							}
						}
						EE_END_LOOP;
					}
					case EE_N_IF : {
						EE_RESULT & rTemp = soProcessMe.sSubResults[0];

						// ==== Evaluate the condition.
						bool bResult = false;
						if ( (rTemp.ncType) == EE_NC_FLOATING ) {
							bResult = rTemp.u.dVal ? true : false;
						}
						else if ( (rTemp.ncType) == EE_NC_UNSIGNED ) {
							bResult = !!rTemp.u.ui64Val;
						}
						else if ( (rTemp.ncType) == EE_NC_SIGNED ) {
							bResult = !!rTemp.u.i64Val;
						}
						// ============================


						// == Execute the statement(s).
						if ( bResult ) {
							EE_HAND_OFF( _ndExp.v.sNodeIndex );
							continue;
						}
						else if ( _ndExp.w.sNodeIndex != ~0 ) {
							EE_HAND_OFF( _ndExp.w.sNodeIndex );
							continue;
						}
						else {
							(*soProcessMe.prResult).ncType = EE_NC_UNSIGNED;
							(*soProcessMe.prResult).u.ui64Val = 0;
						}
						// ==========================

						break;
					}
					case EE_N_ASSIGNMENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.w.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						// Will have been checked in the first switch case.
						//if ( (*aFind).second.bIsConst ) { EE_ERROR( EE_EC_CONST_VAR_REQUIRES_CONST_EPRESSION ); }
				

						switch ( _ndExp.v.ui32Op ) {
							case '=' : {
								aFind->second.rRes = (*soProcessMe.prResult);
								break;
							}

#define EE_CASE_PREP( OBJ_FUNC )																											\
	if ( aFind->second.rRes.ncType == EE_NC_OBJECT && aFind->second.rRes.u.poObj != nullptr ) {												\
		if ( !aFind->second.rRes.u.poObj->OBJ_FUNC( (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_INVALIDOPERATOR ); }					 	\
		break;																																\
	}																																		\
	EE_NUM_CONSTANTS ncTemp = GetCastType( aFind->second.rRes.ncType, (*soProcessMe.prResult).ncType );										\
	aFind->second.rRes = ConvertResultOrObject( aFind->second.rRes, ncTemp );																\
	if ( aFind->second.rRes.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }	\
	(*soProcessMe.prResult) = ConvertResultOrObject( (*soProcessMe.prResult), ncTemp );														\
	if ( (*soProcessMe.prResult).ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_INVALIDCAST ); }

#define EE_HANDLE_CASE( CASE, VAR, OP )																		\
	case CASE : {																							\
		aFind->second.rRes.u.VAR OP (*soProcessMe.prResult).u.VAR;											\
		(*soProcessMe.prResult) = aFind->second.rRes;														\
		break;																								\
	}

#define EE_HANDLE_CASE_OBJ( CASE, VAR, OP )																	\
	case EE_NC_OBJECT : {																					\
		aFind->second.rRes.u.VAR OP (*soProcessMe.prResult).u.VAR;											\
		(*soProcessMe.prResult) = aFind->second.rRes;														\
		break;																								\
	}

#define EE_HANDLE_CASE_BAD_ZERO( CASE, VAR, OP )															\
	case CASE : {																							\
		if ( !(*soProcessMe.prResult).u.VAR ) { EE_ERROR( EE_EC_OPMUSTNOTBEZERO ); }						\
		aFind->second.rRes.u.VAR OP (*soProcessMe.prResult).u.VAR;											\
		(*soProcessMe.prResult) = aFind->second.rRes;														\
		break;																								\
	}

#define EE_CASE( CASE, OP, OBJ_FUNC )																		\
	case CASE : {																							\
		EE_CASE_PREP( OBJ_FUNC );																			\
		switch( (*soProcessMe.prResult).ncType ) {															\
			EE_HANDLE_CASE( EE_NC_UNSIGNED, ui64Val, OP )													\
			EE_HANDLE_CASE( EE_NC_SIGNED, i64Val, OP )														\
			EE_HANDLE_CASE( EE_NC_FLOATING, dVal, OP )														\
			default : { EE_ERROR( EE_EC_PROCESSINGERROR ); }												\
		}																									\
		break;																								\
	}

#define EE_CASE_BAD_ZERO( CASE, OP, OBJ_FUNC )																\
	case CASE : {																							\
		EE_CASE_PREP( OBJ_FUNC );																			\
		switch( (*soProcessMe.prResult).ncType ) {															\
			EE_HANDLE_CASE_BAD_ZERO( EE_NC_UNSIGNED, ui64Val, OP )											\
			EE_HANDLE_CASE_BAD_ZERO( EE_NC_SIGNED, i64Val, OP )												\
			EE_HANDLE_CASE_BAD_ZERO( EE_NC_FLOATING, dVal, OP )												\
			default : { EE_ERROR( EE_EC_PROCESSINGERROR ); }												\
		}																									\
		break;																								\
	}

#define EE_INT_CASE( CASE, OP, OBJ_FUNC )																	\
	case CASE : {																							\
		EE_CASE_PREP( OBJ_FUNC );																			\
		switch( (*soProcessMe.prResult).ncType ) {															\
			EE_HANDLE_CASE( EE_NC_UNSIGNED, ui64Val, OP )													\
			EE_HANDLE_CASE( EE_NC_SIGNED, i64Val, OP )														\
			default : { EE_ERROR( EE_EC_PROCESSINGERROR ); }												\
		}																									\
		break;																								\
	}

#define EE_INT_CASE_BAD_ZERO( CASE, OP, OBJ_FUNC )															\
	case CASE : {																							\
		EE_CASE_PREP( OBJ_FUNC );																			\
		switch( (*soProcessMe.prResult).ncType ) {															\
			EE_HANDLE_CASE_BAD_ZERO( EE_NC_UNSIGNED, ui64Val, OP )											\
			EE_HANDLE_CASE_BAD_ZERO( EE_NC_SIGNED, i64Val, OP )												\
			default : { EE_ERROR( EE_EC_PROCESSINGERROR ); }												\
		}																									\
		break;																								\
	}

							EE_CASE( CExpEvalParser::token::EE_ASS_PLUSEQUALS, +=, PlusEquals )
							EE_CASE( CExpEvalParser::token::EE_ASS_MINUSEQUALS, -=, MinusEquals )
							EE_CASE( CExpEvalParser::token::EE_ASS_TIMESEQUALS, *=, TimesEquals )
							EE_INT_CASE_BAD_ZERO( CExpEvalParser::token::EE_ASS_MODEQUALS, %=, ModEquals )
							EE_CASE_BAD_ZERO( CExpEvalParser::token::EE_ASS_DIVEQUALS, /=, DivideEquals )
							case CExpEvalParser::token::EE_ASS_SHLEFTEQUALS : {
								EE_CASE_PREP( ShiftLeftEquals );
								switch( (*soProcessMe.prResult).ncType ) {
									EE_HANDLE_CASE( EE_NC_UNSIGNED, ui64Val, <<= )
									EE_HANDLE_CASE( EE_NC_SIGNED, i64Val, <<= )
									case EE_NC_FLOATING : {
										aFind->second.rRes.u.dVal *= std::pow( 2.0, (*soProcessMe.prResult).u.dVal );
										(*soProcessMe.prResult) = aFind->second.rRes;
										break;																								
									}
									default : { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								}
								break;
							}
							case CExpEvalParser::token::EE_ASS_SHRIGHTEQUALS : {
								EE_CASE_PREP( ShiftRightEquals );
								switch( (*soProcessMe.prResult).ncType ) {
									EE_HANDLE_CASE( EE_NC_UNSIGNED, ui64Val, >>= )
									EE_HANDLE_CASE( EE_NC_SIGNED, i64Val, >>= )
									case EE_NC_FLOATING : {
										aFind->second.rRes.u.dVal /= std::pow( 2.0, (*soProcessMe.prResult).u.dVal );
										(*soProcessMe.prResult) = aFind->second.rRes;
										break;																								
									}
									default : { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								}
								break;
							}
							EE_INT_CASE( CExpEvalParser::token::EE_ASS_OREQUALS, |=, OrEquals )
							EE_INT_CASE( CExpEvalParser::token::EE_ASS_ANDEQUALS, &=, AndEquals )
							EE_INT_CASE( CExpEvalParser::token::EE_ASS_CARROTEQUALS, ^=, CarrotEquals )
						}
				

#undef EE_INT_CASE_BAD_ZERO
#undef EE_INT_CASE
#undef EE_CASE_BAD_ZERO
#undef EE_CASE
#undef EE_HANDLE_CASE_BAD_ZERO
#undef EE_HANDLE_CASE
#undef EE_CASE_PREP
						++aFind->second.ui64UpdateCounter;
						break;
					}
					case EE_N_CREATE_ARRAY : {
						if ( soProcessMe.uiProcessCount == 1 ) {
							// The array object exists.  Adjust its size and fill it.
							EE_RESULT rTemp = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
							if ( rTemp.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
							m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase->SetSize( static_cast<size_t>(rTemp.u.ui64Val) );

							if ( _ndExp.w.sNodeIndex != ~0 ) {
								EE_PUSH_EXP( _ndExp.w.sNodeIndex, &soProcessMe.sSubResults[0] );
							}
							else {
								soProcessMe.sSubResults[0].ncType = EE_NC_UNSIGNED;
								soProcessMe.sSubResults[0].u.ui64Val = 0;
							}
							if ( _ndExp.x.sNodeIndex != ~0 ) {
								EE_PUSH_EXP( _ndExp.x.sNodeIndex, &soProcessMe.sSubResults[1] );
							}
							else {
								soProcessMe.sSubResults[1].ncType = EE_NC_UNSIGNED;
								soProcessMe.sSubResults[1].u.ui64Val = 0;
							}
							break;
						}
						m_vArrayData[_ndExp.y.sNodeIndex].m_pabBase->Initialize( soProcessMe.sSubResults[0], soProcessMe.sSubResults[1] );
						break;
					}
					case EE_N_ARRAY_ASSIGNMENT : {
						EE_RESULT rTemp = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( rTemp.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						switch ( _ndExp.v.ui32Op ) {
							case '=' : {
								if ( !m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->WriteValue( static_cast<size_t>(rTemp.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_PLUSEQUALS : {
								if ( !m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->PlusEquals( static_cast<size_t>(rTemp.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_MINUSEQUALS : {
								if ( !m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->MinusEquals( static_cast<size_t>(rTemp.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_TIMESEQUALS : {
								if ( !m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->TimesEquals( static_cast<size_t>(rTemp.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_MODEQUALS : {
								if ( !m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->ModEquals( static_cast<size_t>(rTemp.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_DIVEQUALS : {
								if ( !m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->DivideEquals( static_cast<size_t>(rTemp.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_CARROTEQUALS : {
								if ( !m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->CarrotEquals( static_cast<size_t>(rTemp.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_SHLEFTEQUALS : {
								if ( !m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->ShiftLeftEquals( static_cast<size_t>(rTemp.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_SHRIGHTEQUALS : {
								if ( !m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->ShiftRightEquals( static_cast<size_t>(rTemp.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_OREQUALS : {
								if ( !m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->OrEquals( static_cast<size_t>(rTemp.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_ANDEQUALS : {
								if ( !m_vArrayData[_ndExp.w.sNodeIndex].m_pabBase->AndEquals( static_cast<size_t>(rTemp.u.ui64Val), (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_PROCESSINGERROR ); }
								break;
							}
						}
						break;
					}
					case EE_N_ARRAY_ASSIGNMENT_OBJ : {
						//EE_PUSH( _ndExp.u.sNodeIndex );		// soProcessMe.sSubResults[0] = IDX.
						//EE_PUSH( _ndExp.x.sNodeIndex );		// soProcessMe.sSubResults[1] = VAL.
						// If the object is not an array type then fail.
						auto aFind = m_mCustomVariables.find( _ndExp.w.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						if ( aFind->second.rRes.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( aFind->second.rRes.u.poObj == nullptr ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !(aFind->second.rRes.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }

						EE_RESULT rIdx = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( rIdx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						ee::CVector * pvThis = static_cast<ee::CVector *>(aFind->second.rRes.u.poObj);
						auto sIdx = CObject::ArrayIndexToLinearIndex( rIdx.u.ui64Val, pvThis->GetBacking().size() );
						if ( sIdx == EE_INVALID_IDX ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_BADARRAYIDX ); }
						switch ( _ndExp.v.ui32Op ) {
							case '=' : {
								pvThis->GetBacking()[sIdx] = soProcessMe.sSubResults[1];
								break;
							}
							case CExpEvalParser::token::EE_ASS_PLUSEQUALS : {
								auto aCode = PerformOp( pvThis->GetBacking()[sIdx], '+', soProcessMe.sSubResults[1], pvThis->GetBacking()[sIdx] );
								if ( aCode != EE_EC_SUCCESS ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( aCode ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_MINUSEQUALS : {
								auto aCode = PerformOp( pvThis->GetBacking()[sIdx], '-', soProcessMe.sSubResults[1], pvThis->GetBacking()[sIdx] );
								if ( aCode != EE_EC_SUCCESS ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( aCode ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_TIMESEQUALS : {
								auto aCode = PerformOp( pvThis->GetBacking()[sIdx], '*', soProcessMe.sSubResults[1], pvThis->GetBacking()[sIdx] );
								if ( aCode != EE_EC_SUCCESS ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( aCode ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_MODEQUALS : {
								auto aCode = PerformOp( pvThis->GetBacking()[sIdx], '%', soProcessMe.sSubResults[1], pvThis->GetBacking()[sIdx] );
								if ( aCode != EE_EC_SUCCESS ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( aCode ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_DIVEQUALS : {
								auto aCode = PerformOp( pvThis->GetBacking()[sIdx], '/', soProcessMe.sSubResults[1], pvThis->GetBacking()[sIdx] );
								if ( aCode != EE_EC_SUCCESS ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( aCode ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_CARROTEQUALS : {
								auto aCode = PerformOp( pvThis->GetBacking()[sIdx], '^', soProcessMe.sSubResults[1], pvThis->GetBacking()[sIdx] );
								if ( aCode != EE_EC_SUCCESS ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( aCode ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_SHLEFTEQUALS : {
								auto aCode = PerformOp( pvThis->GetBacking()[sIdx], CExpEvalParser::token::EE_LEFT_OP, soProcessMe.sSubResults[1], pvThis->GetBacking()[sIdx] );
								if ( aCode != EE_EC_SUCCESS ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( aCode ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_SHRIGHTEQUALS : {
								auto aCode = PerformOp( pvThis->GetBacking()[sIdx], CExpEvalParser::token::EE_RIGHT_OP, soProcessMe.sSubResults[1], pvThis->GetBacking()[sIdx] );
								if ( aCode != EE_EC_SUCCESS ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( aCode ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_OREQUALS : {
								auto aCode = PerformOp( pvThis->GetBacking()[sIdx], '|', soProcessMe.sSubResults[1], pvThis->GetBacking()[sIdx] );
								if ( aCode != EE_EC_SUCCESS ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( aCode ); }
								break;
							}
							case CExpEvalParser::token::EE_ASS_ANDEQUALS : {
								auto aCode = PerformOp( pvThis->GetBacking()[sIdx], '&', soProcessMe.sSubResults[1], pvThis->GetBacking()[sIdx] );
								if ( aCode != EE_EC_SUCCESS ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( aCode ); }
								break;
							}
							default : { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_ERRORPROCESSINGOP ); }
						}
						(*soProcessMe.prResult) = pvThis->CreateResult();
						break;
					}
					case EE_N_ADDRESS_ASSIGNMENT : {
						EE_RESULT rNode = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( rNode.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						(*soProcessMe.prResult) = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
						if ( (*soProcessMe.prResult).ncType == EE_NC_INVALID ) { EE_ERROR( EE_EC_INVALIDCAST ); }

						switch ( _ndExp.u.ui32Intrinsic ) {
							case '=' : {
								if ( !m_pfahAddressWriteHandler( rNode.u.ui64Val, _ndExp.v.ctCast, m_uiptrAddressWriteData, this, (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_ADDRESSHANDLERFAILED ); }
								break;
							}
#define MX_MOD_ASSAIGN( CASE, OP )																																					\
	case CExpEvalParser::token::CASE : {																																			\
		if ( !m_pfahAddressHandler ) { EE_ERROR( EE_EC_NOADDRESSHANDLER ); }																										\
		EE_RESULT rCurVal;																																							\
		if ( !m_pfahAddressHandler( rNode.u.ui64Val, _ndExp.v.ctCast, m_uiptrAddressData, this, rCurVal ) ) { EE_ERROR( EE_EC_ADDRESSHANDLERFAILED ); }								\
		EE_ERROR_CODES ecError = PerformOp( rCurVal, OP, (*soProcessMe.prResult), (*soProcessMe.prResult) );																		\
		if ( ecError != EE_EC_SUCCESS ) { EE_ERROR( ecError ); }																													\
		if ( !m_pfahAddressWriteHandler( rNode.u.ui64Val, _ndExp.v.ctCast, m_uiptrAddressWriteData, this, (*soProcessMe.prResult) ) ) { EE_ERROR( EE_EC_ADDRESSHANDLERFAILED ); }	\
		break;																																										\
	}
							MX_MOD_ASSAIGN( EE_ASS_PLUSEQUALS, '+' )
							MX_MOD_ASSAIGN( EE_ASS_MINUSEQUALS, '-' )
							MX_MOD_ASSAIGN( EE_ASS_TIMESEQUALS, '*' )
							MX_MOD_ASSAIGN( EE_ASS_MODEQUALS, '%' )
							MX_MOD_ASSAIGN( EE_ASS_DIVEQUALS, '/' )
							MX_MOD_ASSAIGN( EE_ASS_CARROTEQUALS, '^' )
							MX_MOD_ASSAIGN( EE_ASS_SHLEFTEQUALS, CExpEvalParser::token::EE_LEFT_OP )
							MX_MOD_ASSAIGN( EE_ASS_SHRIGHTEQUALS, CExpEvalParser::token::EE_RIGHT_OP )
							MX_MOD_ASSAIGN( EE_ASS_OREQUALS, '|' )
							MX_MOD_ASSAIGN( EE_ASS_ANDEQUALS, '&' )
							default : {
								EE_ERROR( EE_EC_ERRORPROCESSINGOP );
							}
#undef MX_MOD_ASSAIGN
						}

						break;
					}
					case EE_N_ARRAY_INITIALIZER : {
						size_t sIdx = m_vVectorStack[m_vVectorStack.size()-1];
						if ( sIdx >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						CObject * poVector = m_vObjects[sIdx];
						if ( (poVector->Type() & CObject::EE_BIT_VECTOR) == 0 ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						ee::CVector * pvThis = static_cast<ee::CVector *>(poVector);
						(*soProcessMe.prResult) = pvThis->PushBack( soProcessMe.sSubResults[0] );
						
						break;
					}
					case EE_N_ARRAY_CREATE_VECTOR : {
						//EE_PUSH( _ndExp.u.sNodeIndex );
						CObject * poVector = m_vObjects[_ndExp.v.sStringIndex];
						(*soProcessMe.prResult) = poVector->CreateResult();
						m_vVectorStack.pop_back();
						break;
					}
					case EE_N_ARG_LIST_ENTRY : {
						if ( m_vFuncParms.size() ) {
							m_vFuncParms[m_vFuncParms.size()-1].push_back( soProcessMe.sSubResults[0] );
						}
						break;
					}
					case EE_N_FORMAT : {
						if ( m_vFuncParms.size() ) {
							std::string sTmp = FormatString( m_vStrings[_ndExp.u.sStringIndex].c_str(), m_vStrings[_ndExp.u.sStringIndex].size(),
								m_vFuncParms[m_vFuncParms.size()-1] );
							CString * psObj = reinterpret_cast<CString *>(AllocateObject<CString>());
							if ( psObj ) {
								psObj->SetString( sTmp );
								(*soProcessMe.prResult) = psObj->CreateResult();
							}
							// This counts as a function call so pop the argument stack.
							m_vFuncParms.pop_back();
						}
						break;
					}
					case EE_N_VECTOR_ADD : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( soProcessMe.sSubResults[1].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						CObject * poObj2 = soProcessMe.sSubResults[1].u.poObj;
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( (poObj->Type() & CObject::EE_BIT_VECTOR) && (poObj2->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							ee::CVector * pvThat = static_cast<ee::CVector *>(poObj2);
							(*soProcessMe.prResult) = pvThis->Add( pvThat, static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]) );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_SUB : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( soProcessMe.sSubResults[1].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						CObject * poObj2 = soProcessMe.sSubResults[1].u.poObj;
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( (poObj->Type() & CObject::EE_BIT_VECTOR) && (poObj2->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							ee::CVector * pvThat = static_cast<ee::CVector *>(poObj2);
							(*soProcessMe.prResult) = pvThis->Sub( pvThat, static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]) );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_MUL : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( (poObj->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							if ( soProcessMe.sSubResults[1].ncType != EE_NC_OBJECT ) {
								(*soProcessMe.prResult) = pvThis->Mul( soProcessMe.sSubResults[1], static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]) );
							}
							else {
								CObject * poObj2 = soProcessMe.sSubResults[1].u.poObj;
								if ( !(poObj2->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
								ee::CVector * pvThat = static_cast<ee::CVector *>(poObj2);
								(*soProcessMe.prResult) = pvThis->Mul( pvThat, static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]) );
							}
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_DOT : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( soProcessMe.sSubResults[1].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						CObject * poObj2 = soProcessMe.sSubResults[1].u.poObj;
						if ( (poObj->Type() & CObject::EE_BIT_VECTOR) && (poObj2->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							ee::CVector * pvThat = static_cast<ee::CVector *>(poObj2);
							(*soProcessMe.prResult) = pvThis->Dot( pvThat );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_CROSS : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( soProcessMe.sSubResults[1].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						CObject * poObj2 = soProcessMe.sSubResults[1].u.poObj;
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( (poObj->Type() & CObject::EE_BIT_VECTOR) && (poObj2->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							ee::CVector * pvThat = static_cast<ee::CVector *>(poObj2);
							(*soProcessMe.prResult) = pvThis->Cross( pvThat, static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]) );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_APPEND_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_APPEND : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Append( soProcessMe.sSubResults[1] );
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							/*EE_RESULT rIdx = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
							if ( rIdx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }*/
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->Append( soProcessMe.sSubResults[1] );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_ASSIGN : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							EE_RESULT rIdx = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_UNSIGNED );
							if ( rIdx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Assign( size_t( rIdx.u.ui64Val ), soProcessMe.sSubResults[1] );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_AT_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_AT : {
						EE_RESULT rIdx = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
						if ( rIdx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->At( size_t( rIdx.u.ui64Val ) );
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->At( size_t( rIdx.u.ui64Val ) );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_CAPACITY_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_CAPACITY : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Capacity();
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->Capacity();
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_CLEAR_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_CLEAR : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Clear();
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->Clear();
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_EMPTY_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_EMPTY : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Empty();
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->Empty();
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_ERASE_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_ERASE : {
						EE_RESULT rIdx = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
						if ( rIdx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Erase( size_t( rIdx.u.ui64Val ) );
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->Erase( size_t( rIdx.u.ui64Val ) );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_INSERT : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;

						EE_RESULT rIdx = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_UNSIGNED );
						if ( rIdx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Insert( size_t( rIdx.u.ui64Val ), soProcessMe.sSubResults[1] );
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->Insert( size_t( rIdx.u.ui64Val ), soProcessMe.sSubResults[1] );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_MAX_SIZE_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_MAX_SIZE : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->MaxSize();
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->MaxSize();
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_MAG_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_MAG : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Mag();
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_MAG_SQ_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_MAG_SQ : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->MagSq();
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_NORMALIZE_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_NORMALIZE : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( (poObj->Type() & CObject::EE_BIT_VECTOR) ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Normalize( static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]) );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
						continue;
					}
					case EE_N_VECTOR_RESERVE_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_RESERVE : {
						EE_RESULT rIdx = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
						if ( rIdx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Reserve( size_t( rIdx.u.ui64Val ) );
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->Reserve( size_t( rIdx.u.ui64Val ) );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_RESIZE_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_RESIZE : {
						EE_RESULT rIdx = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
						if ( rIdx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Resize( size_t( rIdx.u.ui64Val ) );
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->Resize( size_t( rIdx.u.ui64Val ) );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_POP_BACK_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_POP_BACK : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->PopBack();
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->PopBack();
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_SHRINK_TO_FIT : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->ShrinkToFit();
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->ShrinkToFit();
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_SIZE_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_SIZE : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Size();
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->Size();
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_SUM : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Sum();
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_VECTOR_SWAP_IDENT : {
						auto aFind = m_mCustomVariables.find( _ndExp.u.sNodeIndex );
						if ( aFind == m_mCustomVariables.end() ) { EE_ERROR( EE_EC_INVALIDTREE ); }
						soProcessMe.sSubResults[0] = (*aFind).second.rRes;
						EE_FALLTHROUGH
					}
					case EE_N_VECTOR_SWAP : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_VECTOR ) {
							ee::CVector * pvThis = static_cast<ee::CVector *>(poObj);
							(*soProcessMe.prResult) = pvThis->Swap( soProcessMe.sSubResults[1] );
						}
						else if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							ee::CString * pvThis = static_cast<ee::CString *>(poObj);
							(*soProcessMe.prResult) = pvThis->Swap( soProcessMe.sSubResults[1] );
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
					case EE_N_STRING_TOKENIZE_IDENT : {
						if ( _ndExp.x.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.x.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						EE_RESULT rTokenizer = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
						if ( rTokenizer.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						EE_RESULT rEmpty = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_UNSIGNED );
						if ( rEmpty.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						bool bErrored;
						std::vector<std::string> vTokenized = ee::CExpEval::TokenizeUtf( m_vStrings[_ndExp.u.sStringIndex],
							uint32_t( rTokenizer.u.ui64Val ),
							rEmpty.u.ui64Val != 0,
							&bErrored );
						if ( bErrored ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						ee::CVector * pvVector = static_cast<ee::CVector *>(m_vObjects[_ndExp.x.sNodeIndex]);
						pvVector->Clear();

						for ( std::vector<std::string>::size_type I = 0; I < vTokenized.size(); ++I ) {
							ee::CString * pvTmp = AllocateObject<ee::CString>();
							if ( !pvTmp ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
							pvTmp->SetString( vTokenized[I] );
							auto aTmp = pvTmp->CreateResult();
							pvVector->PushBack( aTmp );
						}

						(*soProcessMe.prResult) = pvVector->CreateResult();
						break;
					}
					case EE_N_STRING_TOKENIZE : {
						if ( soProcessMe.sSubResults[0].ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( _ndExp.x.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.x.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						CObject * poObj = soProcessMe.sSubResults[0].u.poObj;
						if ( poObj->Type() & CObject::EE_BIT_STRING ) {
							EE_RESULT rTokenizer = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
							if ( rTokenizer.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

							EE_RESULT rEmpty = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_UNSIGNED );
							if ( rEmpty.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

							bool bErrored;
							ee::CString * psString = static_cast<ee::CString *>(poObj);
							std::vector<std::string> vTokenized = ee::CExpEval::TokenizeUtf( psString->GetBacking(),
								uint32_t( rTokenizer.u.ui64Val ),
								rEmpty.u.ui64Val != 0,
								&bErrored );
							if ( bErrored ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

							ee::CVector * pvVector = static_cast<ee::CVector *>(m_vObjects[_ndExp.x.sNodeIndex]);
							pvVector->Clear();

							for ( std::vector<std::string>::size_type I = 0; I < vTokenized.size(); ++I ) {
								ee::CString * pvTmp = AllocateObject<ee::CString>();
								if ( !pvTmp ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
								pvTmp->SetString( vTokenized[I] );
								auto aTmp = pvTmp->CreateResult();
								pvVector->PushBack( aTmp );
							}

							(*soProcessMe.prResult) = pvVector->CreateResult();
						}
						else { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						break;
					}
#define EE_WINDOW_FUNC( CASE, FUNC )																																						\
	case CASE : {																																											\
		auto aRet = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );																									\
		if ( aRet.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }															\
		if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }												\
		if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }					\
		std::vector<double> dTmp;																																							\
		if ( !CExpEval::FUNC<double>( size_t( aRet.u.ui64Val ), dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }								\
																																															\
		ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);																									\
		if ( !pvThis->FromPrimitiveArray<double, EE_NC_FLOATING>( dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }								\
		(*soProcessMe.prResult) = pvThis->CreateResult();																																	\
		break;																																												\
	}
					EE_WINDOW_FUNC( EE_N_BARTHANN, BarthannWindow )
					EE_WINDOW_FUNC( EE_N_BARTLETT, BartlettWindow )
					EE_WINDOW_FUNC( EE_N_BLACKMAN, BlackmanWindow )
					EE_WINDOW_FUNC( EE_N_BLACKMANHARRIS, BlackmanHarrisWindow )
					EE_WINDOW_FUNC( EE_N_BLACKMANNUTTAL, BlackmanNuttalWindow )
					EE_WINDOW_FUNC( EE_N_BOHMAN, BohmanWindow )
					EE_WINDOW_FUNC( EE_N_BOXCAR, BoxcarWindow )
					EE_WINDOW_FUNC( EE_N_COSINE, CosineWindow )
					EE_WINDOW_FUNC( EE_N_FLATTOP, FlatTopWindow )
					EE_WINDOW_FUNC( EE_N_HANN, HannWindow )
					EE_WINDOW_FUNC( EE_N_HAMMING, HammingWindow )
					EE_WINDOW_FUNC( EE_N_LANCZOS, LanczosWindow )
					EE_WINDOW_FUNC( EE_N_NUTTAL, NuttallWindow )
					EE_WINDOW_FUNC( EE_N_PARZEN, ParzenWindow )
					EE_WINDOW_FUNC( EE_N_TRIANG, TriangWindow )
#undef EE_WINDOW_FUNC

					case EE_N_CHEBWIN : {
						auto aRet = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( aRet.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rAt = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( rAt.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						std::vector<double> dTmp;
						if ( !CExpEval::ChebwinWindow<double>( size_t( aRet.u.ui64Val ), rAt.u.dVal, dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);
						if ( !pvThis->FromPrimitiveArray<double, EE_NC_FLOATING>( dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult) = pvThis->CreateResult();
						break;
					}
					case EE_N_EXPONENTIAL : {
						auto aRet = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( aRet.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rTau = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( rTau.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rCenter = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_FLOATING );
						if ( rCenter.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						std::vector<double> dTmp;
						if ( !CExpEval::ExponentialWindow<double>( size_t( aRet.u.ui64Val ), rTau.u.dVal, rCenter.u.dVal, dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);
						if ( !pvThis->FromPrimitiveArray<double, EE_NC_FLOATING>( dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult) = pvThis->CreateResult();
						break;
					}
					case EE_N_GAUSSIAN : {
						auto aRet = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( aRet.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto aSigma = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( aSigma.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						std::vector<double> dTmp;
						if ( !CExpEval::GaussianWindow<double>( size_t( aRet.u.ui64Val ), aSigma.u.dVal, dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);
						if ( !pvThis->FromPrimitiveArray<double, EE_NC_FLOATING>( dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult) = pvThis->CreateResult();
						break;
					}
					case EE_N_GENERAL_COSINE : {
						auto aRet = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( aRet.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto aA = soProcessMe.sSubResults[1];
						if ( aA.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !aA.u.poObj || !(aA.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						auto pvVec = static_cast<CVector *>(aA.u.poObj);
						std::vector<double> vA;
						if ( !pvVec->ToPrimitiveArray<double, EE_NC_FLOATING>( vA ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						std::vector<double> dTmp;
						if ( !CExpEval::GeneralCosineWindow<double>( size_t( aRet.u.ui64Val ), vA, dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);
						if ( !pvThis->FromPrimitiveArray<double, EE_NC_FLOATING>( dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult) = pvThis->CreateResult();
						break;
					}
					case EE_N_GENERAL_GAUSSIAN : {
						auto aRet = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( aRet.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto aP = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( aP.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto aSigma = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_FLOATING );
						if ( aSigma.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						std::vector<double> dTmp;
						if ( !CExpEval::GeneralGaussianWindow<double>( size_t( aRet.u.ui64Val ), aP.u.dVal, aSigma.u.dVal, dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);
						if ( !pvThis->FromPrimitiveArray<double, EE_NC_FLOATING>( dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult) = pvThis->CreateResult();
						break;
					}
					case EE_N_GENERAL_HAMMING : {
						auto aRet = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( aRet.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto aAlpha = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( aAlpha.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						std::vector<double> dTmp;
						if ( !CExpEval::GeneralHammingWindow<double>( size_t( aRet.u.ui64Val ), aAlpha.u.dVal, dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);
						if ( !pvThis->FromPrimitiveArray<double, EE_NC_FLOATING>( dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult) = pvThis->CreateResult();
						break;
					}
					case EE_N_KAISER : {
						auto aRet = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( aRet.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto aBeta = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( aBeta.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						std::vector<double> dTmp;
						if ( !CExpEval::KaiserWindow<double>( size_t( aRet.u.ui64Val ), aBeta.u.dVal, dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);
						if ( !pvThis->FromPrimitiveArray<double, EE_NC_FLOATING>( dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult) = pvThis->CreateResult();
						break;
					}
					case EE_N_KAISER_BESSEL : {
						auto aRet = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( aRet.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto aBeta = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( aBeta.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						std::vector<double> dTmp;
						if ( !CExpEval::KaiserBesselDerivedWindow<double>( size_t( aRet.u.ui64Val ), aBeta.u.dVal, dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);
						if ( !pvThis->FromPrimitiveArray<double, EE_NC_FLOATING>( dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult) = pvThis->CreateResult();
						break;
					}
					case EE_N_TAYLOR : {
						auto aRet = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( aRet.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rNBar = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_SIGNED );
						if ( rNBar.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rSll = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_FLOATING );
						if ( rSll.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rNorm = ConvertResultOrObject( soProcessMe.sSubResults[3], EE_NC_SIGNED );
						if ( rNorm.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						std::vector<double> dTmp;
						if ( !CExpEval::TaylorWindow<double>( size_t( aRet.u.ui64Val ), static_cast<int>(rNBar.u.i64Val), rSll.u.dVal, rNorm.u.i64Val != 0, dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);
						if ( !pvThis->FromPrimitiveArray<double, EE_NC_FLOATING>( dTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult) = pvThis->CreateResult();
						break;
					}
					case EE_N_TUKEY : {
						auto aRet = ConvertResultOrObject( soProcessMe.sSubResults[0], EE_NC_UNSIGNED );
						if ( aRet.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rAlpha = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_FLOATING );
						if ( rAlpha.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						std::vector<double> vTmp;
						if ( !CExpEval::TukeyWindow<double>( size_t( aRet.u.ui64Val ), rAlpha.u.dVal, vTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);
						if ( !pvThis->FromPrimitiveArray<double, EE_NC_FLOATING>( vTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult) = pvThis->CreateResult();
						break;
					}

					case EE_N_TRAPEZOID_1 : {
						// _vY must be a vector.
						auto rY = soProcessMe.sSubResults[0];
						if ( rY.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !rY.u.poObj || !(rY.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }

						std::vector<double> vY;
						ee::CVector * pvY = static_cast<ee::CVector *>(rY.u.poObj);
						if ( !pvY->ToPrimitiveArray<double, EE_NC_FLOATING>( vY ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						
						try {
							(*soProcessMe.prResult).u.dVal = CExpEval::Trapezoid1D<double>( vY, 1.0 );
						}
						catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						break;
					}
					case EE_N_TRAPEZOID_2 : {
						// _vY must be a vector.
						auto rY = soProcessMe.sSubResults[0];
						if ( rY.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !rY.u.poObj || !(rY.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }

						std::vector<double> vY;
						ee::CVector * pvY = static_cast<ee::CVector *>(rY.u.poObj);
						if ( !pvY->ToPrimitiveArray<double, EE_NC_FLOATING>( vY ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						
						// The 2nd parameter can be either a vector or a primitive.
						auto rParm2 = soProcessMe.sSubResults[1];
						if ( rParm2.ncType == EE_NC_OBJECT ) {
							if ( !rParm2.u.poObj || !(rParm2.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }

							ee::CVector * pvX = static_cast<ee::CVector *>(rParm2.u.poObj);
							std::vector<double> vX;
							if ( !pvX->ToPrimitiveArray<double, EE_NC_FLOATING>( vX ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
							try {
								(*soProcessMe.prResult).u.dVal = CExpEval::Trapezoid1D<double>( vY, vX );
							}
							catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						}
						else {
							// Must be a basic primitive.
							auto rDx = ConvertResultOrObject( rParm2, EE_NC_FLOATING );
							if ( rDx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

							try {
								(*soProcessMe.prResult).u.dVal = CExpEval::Trapezoid1D<double>( vY, rDx.u.dVal );
							}
							catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						}
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						break;
					}
					case EE_N_TRAPEZOIDSTRIDED_4 : {
						// _vY must be a vector.
						auto rY = soProcessMe.sSubResults[0];
						if ( rY.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !rY.u.poObj || !(rY.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }

						auto rStart = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
						if ( rStart.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rStride = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_UNSIGNED );
						if ( rStride.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						auto rX = soProcessMe.sSubResults[3];
						if ( rX.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !rX.u.poObj || !(rX.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						std::vector<double> vX;
						ee::CVector * pvX = static_cast<ee::CVector *>(rX.u.poObj);
						if ( !pvX->ToPrimitiveArray<double, EE_NC_FLOATING>( vX ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }

						std::vector<double> vY;
						ee::CVector * pvY = static_cast<ee::CVector *>(rY.u.poObj);
						if ( !pvY->ToPrimitiveArray<double, EE_NC_FLOATING>( vY ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						
						try {
							(*soProcessMe.prResult).u.dVal = CExpEval::TrapezoidStrided<double>( vY, size_t( rStart.u.ui64Val ), size_t( rStride.u.ui64Val ), vX );
						}
						catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						break;
					}
					case EE_N_TRAPEZOIDSTRIDED_5 : {
						// _vY must be a vector.
						auto rY = soProcessMe.sSubResults[0];
						if ( rY.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !rY.u.poObj || !(rY.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }

						auto rStart = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
						if ( rStart.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rCount = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_UNSIGNED );
						if ( rCount.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rStride = ConvertResultOrObject( soProcessMe.sSubResults[3], EE_NC_UNSIGNED );
						if ( rStride.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rDx = ConvertResultOrObject( soProcessMe.sSubResults[4], EE_NC_FLOATING );
						if ( rDx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						std::vector<double> vY;
						ee::CVector * pvY = static_cast<ee::CVector *>(rY.u.poObj);
						if ( !pvY->ToPrimitiveArray<double, EE_NC_FLOATING>( vY ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						
						try {
							(*soProcessMe.prResult).u.dVal = CExpEval::TrapezoidStrided<double>( vY, size_t( rStart.u.ui64Val ), size_t( rCount.u.ui64Val ), size_t( rStride.u.ui64Val ), rDx.u.dVal );
						}
						catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						break;
					}

					case EE_N_SIMPSON_1 : {
						// _vY must be a vector.
						auto rY = soProcessMe.sSubResults[0];
						if ( rY.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !rY.u.poObj || !(rY.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }

						std::vector<double> vY;
						ee::CVector * pvY = static_cast<ee::CVector *>(rY.u.poObj);
						if ( !pvY->ToPrimitiveArray<double, EE_NC_FLOATING>( vY ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						
						try {
							(*soProcessMe.prResult).u.dVal = CExpEval::Simpson1D<double>( vY, 1.0 );
						}
						catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						break;
					}
					case EE_N_SIMPSON_2 : {
						// _vY must be a vector.
						auto rY = soProcessMe.sSubResults[0];
						if ( rY.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !rY.u.poObj || !(rY.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }

						std::vector<double> vY;
						ee::CVector * pvY = static_cast<ee::CVector *>(rY.u.poObj);
						if ( !pvY->ToPrimitiveArray<double, EE_NC_FLOATING>( vY ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						
						// The 2nd parameter can be either a vector or a primitive.
						auto rParm2 = soProcessMe.sSubResults[1];
						if ( rParm2.ncType == EE_NC_OBJECT ) {
							if ( !rParm2.u.poObj || !(rParm2.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }

							ee::CVector * pvX = static_cast<ee::CVector *>(rParm2.u.poObj);
							std::vector<double> vX;
							if ( !pvX->ToPrimitiveArray<double, EE_NC_FLOATING>( vX ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
							try {
								(*soProcessMe.prResult).u.dVal = CExpEval::Simpson1D<double>( vY, vX );
							}
							catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						}
						else {
							// Must be a basic primitive.
							auto rDx = ConvertResultOrObject( rParm2, EE_NC_FLOATING );
							if ( rDx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

							try {
								(*soProcessMe.prResult).u.dVal = CExpEval::Simpson1D<double>( vY, rDx.u.dVal );
							}
							catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						}
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						break;
					}
					case EE_N_SIMPSONSTRIDED_4 : {
						// _vY must be a vector.
						auto rY = soProcessMe.sSubResults[0];
						if ( rY.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !rY.u.poObj || !(rY.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }

						auto rStart = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
						if ( rStart.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rStride = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_UNSIGNED );
						if ( rStride.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						auto rX = soProcessMe.sSubResults[3];
						if ( rX.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !rX.u.poObj || !(rX.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						std::vector<double> vX;
						ee::CVector * pvX = static_cast<ee::CVector *>(rX.u.poObj);
						if ( !pvX->ToPrimitiveArray<double, EE_NC_FLOATING>( vX ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }

						std::vector<double> vY;
						ee::CVector * pvY = static_cast<ee::CVector *>(rY.u.poObj);
						if ( !pvY->ToPrimitiveArray<double, EE_NC_FLOATING>( vY ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						
						try {
							(*soProcessMe.prResult).u.dVal = CExpEval::SimpsonStrided<double>( vY, size_t( rStart.u.ui64Val ), size_t( rStride.u.ui64Val ), vX );
						}
						catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						break;
					}
					case EE_N_SIMPSONSTRIDED_5 : {
						// _vY must be a vector.
						auto rY = soProcessMe.sSubResults[0];
						if ( rY.ncType != EE_NC_OBJECT ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }
						if ( !rY.u.poObj || !(rY.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALID_OBJECT ); }

						auto rStart = ConvertResultOrObject( soProcessMe.sSubResults[1], EE_NC_UNSIGNED );
						if ( rStart.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rCount = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_UNSIGNED );
						if ( rCount.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rStride = ConvertResultOrObject( soProcessMe.sSubResults[3], EE_NC_UNSIGNED );
						if ( rStride.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }
						auto rDx = ConvertResultOrObject( soProcessMe.sSubResults[4], EE_NC_FLOATING );
						if ( rDx.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						std::vector<double> vY;
						ee::CVector * pvY = static_cast<ee::CVector *>(rY.u.poObj);
						if ( !pvY->ToPrimitiveArray<double, EE_NC_FLOATING>( vY ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						
						try {
							(*soProcessMe.prResult).u.dVal = CExpEval::SimpsonStrided<double>( vY, size_t( rStart.u.ui64Val ), size_t( rCount.u.ui64Val ), size_t( rStride.u.ui64Val ), rDx.u.dVal );
						}
						catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
						(*soProcessMe.prResult).ncType = EE_NC_FLOATING;
						break;
					}

					case EE_N_LINSPACE_2 : {
						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						try {
							switch ( _ndExp.z.sNodeIndex ) {
#define EE_LINSPACE( CASE, TYPE, MEMBER, NC_TYPE )																														\
	case CASE : {																																						\
		auto rStart = ConvertResultOrObject( soProcessMe.sSubResults[0], NC_TYPE );																						\
		if ( rStart.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }										\
		auto rEnd = ConvertResultOrObject( soProcessMe.sSubResults[1], NC_TYPE );																						\
		if ( rEnd.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }											\
		std::vector<TYPE> vTmp = ee::CExpEval::Linspace<TYPE>( TYPE( rStart.u.MEMBER ), TYPE( rEnd.u.MEMBER ) );														\
		ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);																				\
		if ( !pvThis->FromPrimitiveArray<TYPE, NC_TYPE>( vTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }					\
		(*soProcessMe.prResult) = pvThis->CreateResult();																												\
		break;																																							\
	}
								EE_LINSPACE( CExpEvalParser::token::EE_INT8, int8_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_INT16, int16_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_INT32, int32_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_INT64, int64_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT8, uint8_t, ui64Val, EE_NC_UNSIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT16, uint16_t, ui64Val, EE_NC_UNSIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT32, uint32_t, ui64Val, EE_NC_UNSIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT64, uint64_t, ui64Val, EE_NC_UNSIGNED )

								EE_LINSPACE( CExpEvalParser::token::EE_FLOAT, float, dVal, EE_NC_FLOATING )
								case CExpEvalParser::token::EE_DEFAULT : {}				EE_FALLTHROUGH
								EE_LINSPACE( CExpEvalParser::token::EE_DOUBLE, double, dVal, EE_NC_FLOATING )

							}
						}
						catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
#undef EE_LINSPACE
						break;
					}
					case EE_N_LINSPACE_3 : {
						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						auto sNum = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_UNSIGNED );
						if ( sNum.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						try {
							switch ( _ndExp.z.sNodeIndex ) {
#define EE_LINSPACE( CASE, TYPE, MEMBER, NC_TYPE )																														\
	case CASE : {																																						\
		auto rStart = ConvertResultOrObject( soProcessMe.sSubResults[0], NC_TYPE );																						\
		if ( rStart.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }										\
		auto rEnd = ConvertResultOrObject( soProcessMe.sSubResults[1], NC_TYPE );																						\
		if ( rEnd.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }											\
		std::vector<TYPE> vTmp = ee::CExpEval::Linspace<TYPE>( TYPE( rStart.u.MEMBER ), TYPE( rEnd.u.MEMBER ), size_t( sNum.u.ui64Val ) );								\
		ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);																				\
		if ( !pvThis->FromPrimitiveArray<TYPE, NC_TYPE>( vTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }					\
		(*soProcessMe.prResult) = pvThis->CreateResult();																												\
		break;																																							\
	}
								EE_LINSPACE( CExpEvalParser::token::EE_INT8, int8_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_INT16, int16_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_INT32, int32_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_INT64, int64_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT8, uint8_t, ui64Val, EE_NC_UNSIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT16, uint16_t, ui64Val, EE_NC_UNSIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT32, uint32_t, ui64Val, EE_NC_UNSIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT64, uint64_t, ui64Val, EE_NC_UNSIGNED )

								EE_LINSPACE( CExpEvalParser::token::EE_FLOAT, float, dVal, EE_NC_FLOATING )
								case CExpEvalParser::token::EE_DEFAULT : {}				EE_FALLTHROUGH
								EE_LINSPACE( CExpEvalParser::token::EE_DOUBLE, double, dVal, EE_NC_FLOATING )

							}
						}
						catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
#undef EE_LINSPACE
						break;
					}
					case EE_N_LINSPACE_4 : {
						if ( _ndExp.w.sNodeIndex >= m_vObjects.size() ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }
						if ( !(m_vObjects[_ndExp.w.sNodeIndex]->Type() & CObject::EE_BIT_VECTOR) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_PROCESSINGERROR ); }

						auto sNum = ConvertResultOrObject( soProcessMe.sSubResults[2], EE_NC_UNSIGNED );
						if ( sNum.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						auto sEndPt = ConvertResultOrObject( soProcessMe.sSubResults[3], EE_NC_UNSIGNED );
						if ( sEndPt.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }

						try {
							switch ( _ndExp.z.sNodeIndex ) {
#define EE_LINSPACE( CASE, TYPE, MEMBER, NC_TYPE )																														\
	case CASE : {																																						\
		auto rStart = ConvertResultOrObject( soProcessMe.sSubResults[0], NC_TYPE );																						\
		if ( rStart.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }										\
		auto rEnd = ConvertResultOrObject( soProcessMe.sSubResults[1], NC_TYPE );																						\
		if ( rEnd.ncType == EE_NC_INVALID ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_INVALIDCAST ); }											\
		std::vector<TYPE> vTmp = ee::CExpEval::Linspace<TYPE>( TYPE( rStart.u.MEMBER ), TYPE( rEnd.u.MEMBER ), size_t( sNum.u.ui64Val ), sEndPt.u.ui64Val != 0 );		\
		ee::CVector * pvThis = static_cast<ee::CVector *>(m_vObjects[_ndExp.w.sNodeIndex]);																				\
		if ( !pvThis->FromPrimitiveArray<TYPE, NC_TYPE>( vTmp ) ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }					\
		(*soProcessMe.prResult) = pvThis->CreateResult();																												\
		break;																																							\
	}
								EE_LINSPACE( CExpEvalParser::token::EE_INT8, int8_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_INT16, int16_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_INT32, int32_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_INT64, int64_t, i64Val, EE_NC_SIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT8, uint8_t, ui64Val, EE_NC_UNSIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT16, uint16_t, ui64Val, EE_NC_UNSIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT32, uint32_t, ui64Val, EE_NC_UNSIGNED )
								EE_LINSPACE( CExpEvalParser::token::EE_UINT64, uint64_t, ui64Val, EE_NC_UNSIGNED )

								EE_LINSPACE( CExpEvalParser::token::EE_FLOAT, float, dVal, EE_NC_FLOATING )
								case CExpEvalParser::token::EE_DEFAULT : {}				EE_FALLTHROUGH
								EE_LINSPACE( CExpEvalParser::token::EE_DOUBLE, double, dVal, EE_NC_FLOATING )

							}
						}
						catch ( ... ) { (*soProcessMe.prResult).ncType = EE_NC_INVALID; EE_ERROR( EE_EC_OUTOFMEMORY ); }
#undef EE_LINSPACE
						break;
					}
				}



				m_vStack.pop_back();
			}
#undef soProcessMe
		}

		m_vLoopStack.clear();

#undef EE_END_LOOP
#undef EE_BEGIN_LOOP
#undef EE_DONE
#undef EE_HAND_OFF
#undef EE_SEND_BACK
#undef EE_ERROR
#undef EE_PUSH
#undef EE_PUSH_EXP
		return true;
	}

	// Takes an array type and creates an array for the given type.
	CArrayBase * CExpEvalContainer::CreateArrayFromType( uint32_t _ui32Type ) {
		switch ( _ui32Type ) {
			case CExpEvalParser::token::EE_INT8 : {
				return new ( std::nothrow ) CInt8Array( this );
			}
			case CExpEvalParser::token::EE_INT16 : {
				return new ( std::nothrow ) CInt16Array( this );
			}
			case CExpEvalParser::token::EE_INT32 : {
				return new ( std::nothrow ) CInt32Array( this );
			}
			case CExpEvalParser::token::EE_INT64 : {
				return new ( std::nothrow ) CInt64Array( this );
			}
			case CExpEvalParser::token::EE_UINT8 : {
				return new ( std::nothrow ) CUInt8Array( this );
			}
			case CExpEvalParser::token::EE_UINT16 : {
				return new ( std::nothrow ) CUInt16Array( this );
			}
			case CExpEvalParser::token::EE_UINT32 : {
				return new ( std::nothrow ) CUInt32Array( this );
			}
			case CExpEvalParser::token::EE_UINT64 : {
				return new ( std::nothrow ) CUInt64Array( this );
			}
			case CExpEvalParser::token::EE_FLOAT : {
				return new ( std::nothrow ) CFloatArray( this );
			}
			case CExpEvalParser::token::EE_DOUBLE : {
				return new ( std::nothrow ) CDoubleArray( this );
			}
			case CExpEvalParser::token::EE_DEFAULT : {
				return new ( std::nothrow ) CDefaultArray( this );
			}
		}
		return nullptr;
	}

	// Creates a string reference given a string index.
	bool CExpEvalContainer::CreateStringRef( size_t /*_stStrIdx*/ ) {
		return false;
		/*if ( m_vStringObjectsU8.size() <= _stStrIdx ) {
			m_vStringObjectsU8.resize( _stStrIdx + 1 );
		}
		if ( m_vStringObjectsU8.size() <= _stStrIdx ) { return false; }

		if ( !m_vStringObjectsU8[_stStrIdx] ) {
			m_vStringObjectsU8[_stStrIdx] = new( std::nothrow ) CStringRef( this, _stStrIdx );
			if ( !m_vStringObjectsU8[_stStrIdx] ) { return false; }
		}

		return true;*/
	}
	
	// Shifting for floats.
	double CExpEvalContainer::LShift( double _dVal, double _dShift ) {
		return _dVal * std::pow( 2.0, _dShift );
	}

	// Shifting for floats.
	double CExpEvalContainer::RShift( double _dVal, double _dShift ) {
		return _dVal / std::pow( 2.0, _dShift );
	}

	// Fills out a context structure for PrintF().
	bool CExpEvalContainer::FillContext( const wchar_t * _pwcFormat, size_t &_sPos, EE_PRINTF_CONTEXT &_pcContext, EE_NUM_CONSTANTS &_ncType ) {
		size_t sDstIdx = 0;
		_pcContext.wcFormat[sDstIdx++] = L'%';
		// First gather all valid flags.
		size_t sThisPos = _sPos + 1;
#define EE_APPEND_CHAR( VAL )		if ( sDstIdx == std::size( _pcContext.wcFormat ) - 1 ) { return false; } _pcContext.wcFormat[sDstIdx++] = VAL;
		while ( IsPrintFFlag( _pwcFormat[sThisPos] ) ) {
			 EE_APPEND_CHAR( _pwcFormat[sThisPos++] );
		}
		// Now all valid width specifers.
		while ( IsPrintFWidth( _pwcFormat[sThisPos] ) ) {
			 EE_APPEND_CHAR( _pwcFormat[sThisPos++] );
		}

		if ( _pwcFormat[sThisPos] == L'.' ) {
			EE_APPEND_CHAR( _pwcFormat[sThisPos++] );
			while ( IsPrintFPrecision( _pwcFormat[sThisPos] ) ) {
				 EE_APPEND_CHAR( _pwcFormat[sThisPos++] );
			}
		}

		// Skip size specifiers.
		while ( IsPrintFArgSize( _pwcFormat[sThisPos] ) ) {
			++sThisPos;
		}

		// Swap with our size depending on the type.
		if ( IsPrintFIntType( _pwcFormat[sThisPos] ) ) {
			EE_APPEND_CHAR( L'I' );
			EE_APPEND_CHAR( L'6' );
			EE_APPEND_CHAR( L'4' );
			EE_APPEND_CHAR( _pwcFormat[sThisPos++] );
			_ncType = EE_NC_UNSIGNED;
		}
		else if ( IsPrintFFloatType( _pwcFormat[sThisPos] ) ) {
			EE_APPEND_CHAR( _pwcFormat[sThisPos++] );
			_ncType = EE_NC_FLOATING;
		}
		else {
			// We don't handle strings.
			return false;
		}
		EE_APPEND_CHAR( L'\0' );

		_sPos = sThisPos - 1;
		return true;
#undef	EE_APPEND_CHAR
	}

	// Eats the {..} part out of a string.  Assumes that _pcFormat points to the next character after the opening {.
	// Also assumes that from { to } is all standard ASCII, since no special characters are allowed inside valid formatters.
	std::string CExpEvalContainer::EatStringFormatter( const char * &_pcFormat, size_t &_stLen, size_t &_stArgIdx, const std::vector<EE_RESULT> &/*_vArgs*/ ) {
		std::string sRet;
		sRet.push_back( '{' );
		size_t stNumberEat = 0;
		uint64_t ui64Idx = ee::CExpEval::StoULL( _pcFormat, 10, &stNumberEat );
		if ( stNumberEat ) {
			_stArgIdx = static_cast<size_t>(ui64Idx);
			_pcFormat += stNumberEat;
			_stLen -= stNumberEat;
		}
		while ( _stLen ) {
			if ( (*_pcFormat) == '{' ) { break; }
			sRet.push_back( (*_pcFormat) );
			++_pcFormat;
			--_stLen;
			if ( sRet[sRet.size()-1] == '}' ) { break; }
		}

		return sRet;
	}

}	// namespace ee;

#undef EE_INVALID_IDX

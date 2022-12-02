#pragma once

#include "EEExpEval.h"
#include "EEExpEvalSyntaxNodes.h"
#include "EEFloatX.h"
#include "Gen/EEExpEvalParser.h"
#include <cstdarg>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#define EE_MAX_SUB_EXPRESSIONS				7
#define EE_OPTIMIZE_FOR_RUNTIME

namespace ee {

	class CArrayBase;
	class CExpEvalLexer;
	class CObject;
	class CStringRef;

	class CExpEvalContainer {
		friend class						CExpEvalParser;
		friend class						CStringRef;			// Hides away the details behind the optimization wherein string literals inside the Expression are read-only references to reduce copying etc.
	public :
		CExpEvalContainer( CExpEvalLexer * _plLexer ) :
			m_bTreatAllAsHex( false ),
			m_peelLexer( _plLexer ),
			m_pfahAddressHandler( nullptr ),
			m_uiptrAddressData( 0 ),
			m_pfahAddressWriteHandler( nullptr ),
			m_uiptrAddressWriteData( 0 ),
			m_pfUser( nullptr ),
			m_uiptrUserData( 0 ),
			m_pfshString( nullptr ),
			m_uiptrStringData( 0 ),
			m_pfmahMemberAccess( nullptr ),
			m_uiptrMemberAccess( 0 ),
			m_pfToString( nullptr ),
			m_bHasUserVar( false ),
			m_bHasAddressRead( false ) {
			m_mRandGen = std::mt19937_64( m_rdRandDevice() );
		}
		~CExpEvalContainer();


		// == Enumerations.
		// Error codes.
		enum EE_ERROR_CODES {
			EE_EC_SUCCESS,					// No problem.

			EE_EC_INVALIDTREE,				// The syntax tree is invalid.
			EE_EC_OUTOFMEMORY,				// Out of memory.
			EE_EC_PROCESSINGERROR,			// General error.
			
			EE_EC_RESULTSTOOSHORT,			// The sub-expression results array is too short.
			EE_EC_NOIDENTIFIERHANDLER,		// No call to SetStringHandler() has been made with a non-nullptr value.
			EE_EC_IDENTHANDLERFAILED,		// Identifier handler returned false.
			
			EE_EC_VARNOTFOUND,				// Custom variable not found by name.
			EE_EC_VARHASBADTYPE,			// Custom variable has an invalid type.
			EE_EC_BADARRAYIDX,				// Array index out-of-range.
			EE_EC_NOADDRESSHANDLER,			// No address handler.
			EE_EC_ADDRESSHANDLERFAILED,		// Address handler failed.
			
			EE_EC_NOMEMBERHANDLER,			// No call to SetMemberAccessHandler() has been made with a non-nullptr value.
			EE_EC_MEMBERHANDELRFAILED,		// Member handler returned false.
			EE_EC_NOUSERHANDLER,			// No call to SetUserHandler() has been made with a non-nullptr value.
			EE_EC_USERHANDLERFAILED,		// User handler returned false.
			
			EE_EC_FLOATWITHTILDE,			// The ~ cannot be used with floating-point values.
			EE_EC_UNRECOGNIZEDUNARYOPERATOR,// Unrecognized unary operator.
			EE_EC_UNRECOGNIZEDINTRINSIC0,	// Unrecognized no-parameter intrinsic.
			EE_EC_UNRECOGNIZEDINTRINSIC1,	// Unrecognized 1-parameter intrinsic.
			EE_EC_UNRECOGNIZEDINTRINSIC2,	// Unrecognized 2-parameter intrinsic.
			EE_EC_UNRECOGNIZEDINTRINSIC3,	// Unrecognized 3-parameter intrinsic.

			EE_EC_TOOMANYSIGNBITS,			// Too many sign bits in a custom float.
			EE_EC_TOOMANYEXPBITS,			// Too many exponent bits in a custom float.
			EE_EC_TOOMANYMANTISSABITS,		// Too many mantissa bits in a custom float.

			EE_EC_INVALILOOPCONDITIONEXP,	// Invalid expression inside a loop condition.
			EE_EC_INVALIDLOOPDECL,			// Invalid declaration inside a loop.

			EE_EC_ARRAYACCESSERROR,			// Error accessing the contents of an array for a for-each loop.

			EE_EC_ERRORPROCESSINGOP,		// An invalid operator was submitted.

			EE_EC_OPMUSTNOTBEZERO,			// 0 not allow on the right-hand side of an op (X / 0, X % 0, etc.)

			EE_EC_INVALIDOPERATOR,			// Invalid operation, such as "~0.3".

			EE_EC_INVALIDAPIOPERATION,		// Invalid operation that is part of the base API implementation and is expected to be working.

			EE_EC_INVALID_WRITE_TO_CONST,							// Invalid attempt to write to a custom variable declared with "const".

			EE_EC_CONST_VAR_REQUIRES_CONST_EPRESSION,				// A const variable was attempted to be created but was not assigned a const expression.

			EE_EC_NONCONSTNOTALLOWED,		// A call to resolve a non-constant expression could have succeeded but failed due to passed-in flags.

			EE_EC_INVALIDCAST,				// An object failed to cast into another object or primitive, or could not be created from one.
			EE_EC_ARRAY_FROM_NON_OBJECT,	// An array access was attempted on a type that is not an object.

			EE_EC_UNIMPLEMENTED,			// Error for development to indicate that a feature is not implemented yet.
		};

		
		// == Types.
		// The result structure.
		struct EE_RESULT {
			EE_NUM_CONSTANTS				ncType;
			union {
				int64_t						i64Val;
				uint64_t					ui64Val;
				double						dVal;
				CObject *					poObj;
			}								u;
		};

		// Address ([<address>]) handler.
		typedef bool (__stdcall *			PfAddressHandler)( uint64_t _ui64Address, EE_CAST_TYPES _tType, uintptr_t _uiptrData, CExpEvalContainer * _peecContainer, EE_RESULT &_rResult );

		// User-variable function handler.
		typedef bool (__stdcall *			PfUserVarHandler)( uintptr_t _uiptrData, CExpEvalContainer * _peecContainer, EE_RESULT &_rResult );

		// String handler.
		typedef bool (__stdcall *			PfStringHandler)( const std::string &_sString, uintptr_t _uiptrData, CExpEvalContainer * _peecContainer, EE_RESULT &_rResult );

		// Member-access handler.
		typedef bool (__stdcall *			PfMemberAccessHandler)( const EE_RESULT &_rLeft, const std::string &_sMember, uintptr_t _uiptrData, CExpEvalContainer * _peecContainer, EE_RESULT &_rResult );

		// ToString() function.
		typedef std::wstring (__stdcall *	PfToStringHandler)( EE_RESULT &_rResult, uint64_t _ui64Options );


		// == Functions.
		// Get the result.
		bool								Resolve( EE_RESULT &_rRes );

		// Do we treat everything as hex?
		bool								TreatAllAsHex() const { return m_bTreatAllAsHex; }

		// Set whether to treat all as hex or not.
		void								SetTreatAsHex( bool _bVal ) { m_bTreatAllAsHex = _bVal; }

		// Set the address handler.
		void								SetAddressHandler( PfAddressHandler _pfHandler, uintptr_t _uiptrData ) { m_pfahAddressHandler = _pfHandler; m_uiptrAddressData = _uiptrData; }

		// Set the address write handler.
		void								SetAddressWriteHandler( PfAddressHandler _pfHandler, uintptr_t _uiptrData ) { m_pfahAddressWriteHandler = _pfHandler; m_uiptrAddressWriteData = _uiptrData; }

		// Set the user data handler.
		void								SetUserHandler( PfUserVarHandler _pfHandler, uintptr_t _uiptrData ) { m_pfUser = _pfHandler; m_uiptrUserData = _uiptrData; }

		// Set the string handler.
		void								SetStringHandler( PfStringHandler _pfHandler, uintptr_t _uiptrData ) { m_pfshString = _pfHandler; m_uiptrStringData = _uiptrData; }

		// Set the identifier handler.
		void								SetMemberAccessHandler( PfMemberAccessHandler _pfHandler, uintptr_t _uiptrData ) { m_pfmahMemberAccess = _pfHandler; m_uiptrMemberAccess = _uiptrData; }

		// Sets the ToString() handler.
		void								SetToStringHandler( PfToStringHandler _pfHandler ) { m_pfToString = _pfHandler; }

		// Sets the lexer to NULL.  Call after the expression has been successfully parsed.
		void								ExpWasParsed() { m_peelLexer = nullptr; }

		// Gets the user data passed in SetUserHandler().
		uintptr_t							UserData() const { return m_uiptrUserData; }

		// Sets the user data to be used by the handler passed in SetUserHandler().
		void								SetUserData( uintptr_t _uiptrData ) { m_uiptrUserData = _uiptrData; }

		// Does the compilsed script have a call to "??"?
		bool								HasAccessToUserData() const { return m_bHasUserVar; }

		// Does the compiled script have a dereference?
		bool								HasAddressRead() const { return m_bHasAddressRead; }

		// Sets a numbered parameter.
		bool								SetNumberedParm( size_t _sIdx, const EE_RESULT &_rValue );

		// Gets a numbered parameter.
		EE_RESULT							GetNumberedParm( size_t _sIdx ) {
			return _sIdx >= m_vNumberedParms.size() ?
				DefaultResult() :
				m_vNumberedParms[_sIdx];
		}

		// Gets the total numbered parameters.
		__inline size_t						TotalNumberedParms() const { return m_vNumberedParms.size(); }

		// Determines if a numbered parameter is accessed by the script.
		__inline bool						IsNumberedParmAccessed( size_t _sIdx ) const { return m_sNumberedParmsAccessed.find( _sIdx ) != m_sNumberedParmsAccessed.end(); }

		// Determines if any numbered parameter is accessed by the script.
		__inline bool						IsNumberedParmAccessed() const { return m_sNumberedParmsAccessed.size() != 0; }

		// Determines if dynamic access to the numbered parameters is used within the script.
		__inline bool						IsDynamicNumberedParmAccessed() const { return IsNumberedParmAccessed( static_cast<size_t>(-1) ); }

		// Gets the type to use between 2 given types.
		static EE_NUM_CONSTANTS				GetCastType( EE_NUM_CONSTANTS _ncLeft, EE_NUM_CONSTANTS _ncRight );

		// Converts a result to a given type.
		static EE_RESULT					ConvertResult( const EE_RESULT &_rRes, EE_NUM_CONSTANTS _ncType );

		// Converts a result or object to a given type.
		EE_RESULT							ConvertResultOrObject( const EE_RESULT &_rRes, EE_NUM_CONSTANTS _ncType );

		/**
		 * Performs ToString() on a given result or object.
		 *
		 * \param _rRes The result to convert to a string.
		 * \param _sReturn The resulting string form of the given result.
		 * \return Returns true of a to-string conversion was made.
		 */
		bool								ToStringResultOrObject( const EE_RESULT &_rRes, std::string &_sReturn );

		/**
		 * Performs ToString() on a given result or object.
		 *
		 * \param _rRes The result to convert to a string.
		 * \param _sFormat The format string.
		 * \param _sReturn The resulting string form of the given result.
		 * \return Returns true of a to-string conversion was made.
		 */
		bool								ToFormatStringResultOrObject( const EE_RESULT &_rRes, const std::string &_sFormat, std::string &_sReturn );

		/**
		 * Prforms an equality comparison on a pair of given results or objects.
		 *
		 * \param _rLeft The left operand.
		 * \param _rRight The right operand.
		 * \return If either result is an object, Obj->Equals() is called on te pair, otherwise a comparison between
		 */
		bool								EqualResultOrObject( const EE_RESULT &_rLeft, const EE_RESULT &_rRight );

		// Gets the default result value.
		static EE_RESULT					DefaultResult() { EE_RESULT rRes; rRes.ncType = EE_NC_UNSIGNED; rRes.u.ui64Val = 0UL; return rRes; }

		// Default ToString() function.
		static std::wstring					DefaultToString( EE_RESULT &_rResult, uint64_t _ui64Options );

		// Returns the index of a string if it exists or static_cast<size_t>(-1) otherwise.
		size_t								HasString( const std::string &_sText ) const;

		// Returns the index of a custom variable if it exists or static_cast<size_t>(-1) otherwise.
		size_t								HasCustomVar( const std::string &_sText ) const;

		// Determines whether or not the given text is a custom variable.
		bool								IsCustomVar( const std::string &_sText ) const;

		// Returns the index of an array if it exists or static_cast<size_t>(-1) otherwise.
		size_t								HasArray( const std::string &_sText ) const;

		// Determines whether or not a given text is an array.
		bool								IsArray( const std::string &_sText ) const;

		// Gets the string version of the given value.
		std::wstring						ToString( EE_RESULT &_rResult, uint64_t _ui64Options );

		// Prints a formatted string.  All ... parameters must be of type "const EE_RESULT".
		static int __cdecl					PrintF( wchar_t * _pwcDst, size_t _sMaxLen,
			const wchar_t * _pwcFormat, int _iArgs, ... );

		// Prints a formatted string.  All ... parameters must be of type "const EE_RESULT".
		static int __cdecl					PrintFVarArgs( wchar_t * _pwcDst, size_t _sMaxLen,
			const wchar_t * _pwcFormat, int _iArgs, va_list _vaArgPtr );

		// Prints a formatted string.
		static int __cdecl					PrintF( wchar_t * _pwcDst, size_t _sMaxLen,
			const wchar_t * _pwcFormat, const std::vector<EE_RESULT> &_vParms );

		// Performs an operation on 2 given results using the given operator.
		EE_ERROR_CODES						PerformOp( EE_RESULT _rLeft, uint32_t _uiOp, EE_RESULT _rRight, EE_RESULT &_rResult );

		// Applies a unary operation to a result.
		inline EE_ERROR_CODES				PerformUnary( EE_RESULT _rExp, uint32_t _uiOp, EE_RESULT &_rResult ) {
			switch ( _uiOp ) {
				case '+' : {
					_rResult = _rExp;
					return EE_EC_SUCCESS;
				}
				case '-' : {
					switch ( _rExp.ncType ) {
						case EE_NC_SIGNED : {
							_rResult.ncType = _rExp.ncType;
							_rResult.u.i64Val = -_rExp.u.i64Val;
							return EE_EC_SUCCESS;
						}
						case EE_NC_UNSIGNED : {
							_rExp = ConvertResultOrObject( _rExp, EE_NC_SIGNED );
							if ( _rExp.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
							_rResult.ncType = _rExp.ncType;
							_rResult.u.i64Val = -_rExp.u.i64Val;
							return EE_EC_SUCCESS;
						}
						case EE_NC_FLOATING : {
							_rResult.ncType = _rExp.ncType;
							_rResult.u.dVal = -_rExp.u.dVal;
							return EE_EC_SUCCESS;
						}
					}
					return EE_EC_UNRECOGNIZEDUNARYOPERATOR;
				}
				case '~' : {
					switch ( _rExp.ncType ) {
						case EE_NC_SIGNED : {
							_rResult.ncType = _rExp.ncType;
							_rResult.u.i64Val = ~_rExp.u.i64Val;
							return EE_EC_SUCCESS;
						}
						case EE_NC_UNSIGNED : {
							_rResult.ncType = _rExp.ncType;
							_rResult.u.ui64Val = ~_rExp.u.ui64Val;
							return EE_EC_SUCCESS;
						}
						case EE_NC_FLOATING : {
							return EE_EC_INVALIDOPERATOR;
						}
					}
					return EE_EC_UNRECOGNIZEDUNARYOPERATOR;
				}
				case '!' : {
					switch ( _rExp.ncType ) {
						case EE_NC_SIGNED : {
							_rResult.ncType = _rExp.ncType;
							_rResult.u.i64Val = !_rExp.u.i64Val;
							return EE_EC_SUCCESS;
						}
						case EE_NC_UNSIGNED : {
							_rResult.ncType = _rExp.ncType;
							_rResult.u.ui64Val = !_rExp.u.ui64Val;
							return EE_EC_SUCCESS;
						}
						case EE_NC_FLOATING : {
							_rResult.ncType = EE_NC_UNSIGNED;
							_rResult.u.ui64Val = !_rExp.u.dVal;
							return EE_EC_SUCCESS;
						}
					}
					return EE_EC_UNRECOGNIZEDUNARYOPERATOR;
				}
			}
			return EE_EC_UNRECOGNIZEDUNARYOPERATOR;
		}

		// Applies a 1-parameter intrinsic to a result.
		EE_ERROR_CODES						PerformIntrinsic( EE_RESULT _rExp, uint32_t _uiIntrinsic, EE_RESULT &_rResult );

		// Applies a 2-parameter intrinsic to a result.
		EE_ERROR_CODES						PerformIntrinsic( EE_RESULT _rExp0, EE_RESULT _rExp1, uint32_t _uiIntrinsic, EE_RESULT &_rResult, bool _bIncludeNonConst );

		// Applies a 3-parameter intrinsic to a result.
		inline EE_ERROR_CODES				PerformIntrinsic( EE_RESULT _rExp0, EE_RESULT _rExp1, EE_RESULT _rExp2, uint32_t _uiIntrinsic, EE_RESULT &_rResult ) {
			_rExp0 = ConvertResultOrObject( _rExp0, EE_NC_FLOATING );
			if ( _rExp0.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rExp1 = ConvertResultOrObject( _rExp1, EE_NC_FLOATING );
			if ( _rExp1.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			_rExp2 = ConvertResultOrObject( _rExp2, EE_NC_FLOATING );
			if ( _rExp2.ncType == EE_NC_INVALID ) { _rResult.ncType = EE_NC_INVALID; return EE_EC_INVALIDCAST; }
			
	
			switch ( _uiIntrinsic ) {
				case CExpEvalParser::token::EE_EPSILON : {
					_rResult.ncType = EE_NC_SIGNED;
					_rResult.u.i64Val = ee::CExpEval::Epsilon( _rExp0.u.dVal, _rExp1.u.dVal, _rExp2.u.dVal );
					return EE_EC_SUCCESS;
				}
				case CExpEvalParser::token::EE_EPSILONF : {
					_rResult.ncType = EE_NC_SIGNED;
					_rResult.u.i64Val = ee::CExpEval::Epsilon( static_cast<float>(_rExp0.u.dVal), static_cast<float>(_rExp1.u.dVal), static_cast<float>(_rExp2.u.dVal) );
					return EE_EC_SUCCESS;
				}
				case CExpEvalParser::token::EE_RELEPSILON : {
					_rResult.ncType = EE_NC_SIGNED;
					_rResult.u.i64Val = ee::CExpEval::RelativeEpsilon( _rExp0.u.dVal, _rExp1.u.dVal, _rExp2.u.dVal );
					return EE_EC_SUCCESS;
				}
				case CExpEvalParser::token::EE_RELEPSILONF : {
					_rResult.ncType = EE_NC_SIGNED;
					_rResult.u.i64Val = ee::CExpEval::RelativeEpsilon( static_cast<float>(_rExp0.u.dVal), static_cast<float>(_rExp1.u.dVal), static_cast<float>(_rExp2.u.dVal) );
					return EE_EC_SUCCESS;
				}
				case CExpEvalParser::token::EE_MADD : {
					_rResult.ncType = EE_NC_FLOATING;
					_rResult.u.dVal = std::fma( _rExp0.u.dVal, _rExp1.u.dVal, _rExp2.u.dVal );
					return EE_EC_SUCCESS;
				}
				default : {
					_rResult.ncType = EE_NC_INVALID;
					return EE_EC_UNRECOGNIZEDINTRINSIC3;
				}
			}
		}

		// Applies a (*reinterpret_cast<float *>(&X)) reinterpretation of a 32-bit integer bits as a 32-bit float.
		static inline EE_ERROR_CODES		PerformAsFloat( EE_RESULT _rExp, EE_RESULT &_rResult ) {
			// This is for reinterpreting a uint32_t as a float.  The expression in ndNode.u.sNodeIndex
			//	can either be an integral type (cast up to 64 bits) or a float (cast up to a double).
			float fVal = 0.0f;
			switch ( _rExp.ncType ) {
				case EE_NC_UNSIGNED : {
					fVal = (*reinterpret_cast<float *>(&_rExp.u.ui64Val));
					break;
				}
				case EE_NC_SIGNED : {
					fVal = (*reinterpret_cast<float *>(&_rExp.u.i64Val));
					break;
				}
				case EE_NC_FLOATING : {
					// as_float() is not really intended to reinterpret 64 "double" bits as "float" bits.
					//	More likely, the user wants to see the float value cast from a double.
					fVal = static_cast<float>(_rExp.u.dVal);
					break;
				}
				default : { return EE_EC_INVALIDTREE; }
			}
			_rResult.ncType = EE_NC_FLOATING;
			_rResult.u.dVal = fVal;
			return EE_EC_SUCCESS;
		}

		// Applies a (*reinterpret_cast<double *>(&X)) reinterpretation of a 64-bit integer bits as a 64-bit float.
		static inline EE_ERROR_CODES		PerformAsDouble( EE_RESULT _rExp, EE_RESULT &_rResult ) {
			// This is for reinterpreting a uint64_t as a float.  The expression in ndNode.u.sNodeIndex
			//	can either be an integral type (cast up to 64 bits) or a float (cast up to a double).
			double dVal = 0.0;
			switch ( _rExp.ncType ) {
				case EE_NC_UNSIGNED : {
					dVal = (*reinterpret_cast<double *>(&_rExp.u.ui64Val));
					break;
				}
				case EE_NC_SIGNED : {
					dVal = (*reinterpret_cast<double *>(&_rExp.u.i64Val));
					break;
				}
				case EE_NC_FLOATING : {
					// Nothing to do.
					dVal = _rExp.u.dVal;
					break;
				}
				default : { return EE_EC_INVALIDTREE; }
			}
			_rResult.ncType = EE_NC_FLOATING;
			_rResult.u.dVal = dVal;
			return EE_EC_SUCCESS;
		}

		// Creates a float using the specific parameters.
		EE_ERROR_CODES						PerformFloatX( EE_RESULT _rTempSignBits, EE_RESULT _rTempExpBits,
			EE_RESULT _rTempManBits, EE_RESULT _rTempImplied, EE_RESULT _rTempSignVal, EE_RESULT _rTempExpVal, EE_RESULT _rTempManVal,
			EE_RESULT &_rResult );

		// Creates a float using the specific parameters and a double input.
		EE_ERROR_CODES						PerformFloatX( EE_RESULT _rTempSignBits, EE_RESULT _rTempExpBits,
			EE_RESULT _rTempManBits, EE_RESULT _rTempImplied, EE_RESULT _rTempDoubleVal,
			EE_RESULT &_rResult );
		
		// Converts the given value to an X-bit float.
		inline EE_ERROR_CODES				PerformToFloatX( EE_RESULT _rTempDoubleVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign,
			EE_RESULT &_rResult ) {
			CFloatX fTemp;
			
			switch ( _rTempDoubleVal.ncType ) {
				case EE_NC_FLOATING : {
					fTemp.CreateFromDouble( _rTempDoubleVal.u.dVal, _uiExpBits,  _uiManBits, _bImplicitMantissaBit, _bHasSign );
					break;
				}
				case EE_NC_SIGNED : {
					fTemp.CreateFromBits( _rTempDoubleVal.u.i64Val, _uiExpBits,  _uiManBits, _bImplicitMantissaBit, _bHasSign );
					break;
				}
				case EE_NC_UNSIGNED : {
					fTemp.CreateFromBits( _rTempDoubleVal.u.ui64Val, _uiExpBits,  _uiManBits, _bImplicitMantissaBit, _bHasSign );
					break;
				}
				default : { return EE_EC_PROCESSINGERROR; }
			}

			_rResult.ncType = EE_NC_FLOATING;
			_rResult.u.dVal = fTemp.AsDouble();
			
			return EE_EC_SUCCESS;
		}

		// Converts the given value to a 16-bit float.
		inline EE_ERROR_CODES				PerformToFloat16( EE_RESULT _rTempDoubleVal, EE_RESULT &_rResult ) {
			return PerformToFloatX( _rTempDoubleVal, 5, 11, true, true, _rResult );
		}

		// Converts the given value to a 14-bit float.
		inline EE_ERROR_CODES				PerformToFloat14( EE_RESULT _rTempDoubleVal, EE_RESULT &_rResult ) {
			return PerformToFloatX( _rTempDoubleVal, 5, 10, true, false, _rResult );
		}

		// Converts the given value to a 11-bit float.
		inline EE_ERROR_CODES				PerformToFloat11( EE_RESULT _rTempDoubleVal, EE_RESULT &_rResult ) {
			return PerformToFloatX( _rTempDoubleVal, 5, 7, true, false, _rResult );
		}

		// Converts the given value to a 10-bit float.
		inline EE_ERROR_CODES				PerformToFloat10( EE_RESULT _rTempDoubleVal, EE_RESULT &_rResult ) {
			return PerformToFloatX( _rTempDoubleVal, 5, 6, true, false, _rResult );
		}

		// Allocates a new object of the given type.
		template <typename _tType>
		_tType *							AllocateObject( uint32_t _ui32Flags = 0 ) {
			try {
				auto aIdx = m_vObjects.size();
				_tType * ptObj = new( std::nothrow ) _tType( this );
				if ( !ptObj ) { return nullptr; }
				m_vObjects.push_back( ptObj );
				return ptObj;
			}
			catch ( const std::bad_alloc /*& _eE*/ ) { return nullptr; }
		}

		// Deallocates an object.
		bool								DeallocateObject( CObject * _poObj );

		// String formatting.
		std::string							FormatString( const char * _pcFormat, size_t _stLen, const std::vector<EE_RESULT> &_vArgs );
		
	protected :
		// == Types.
		// The PrintF() '%' context information.
		struct EE_PRINTF_CONTEXT {
			wchar_t							wcFormat[5+20+1+20];
		};

		// == Functions.
		// Decodes a string.
		size_t								CreateString( const std::string &_sText );

		// Decodes an identifier.
		size_t								CreateIdentifier( const char * _pcText );

		// Creates a string expression.
		void								CreateStringBasicExp( size_t _sStrIndex, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a custom-variable expression.
		void								CreateCustomVar( size_t _sStrIndex, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an array/access expression.
		void								CreateArrayVar( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an array/access expression.
		void								CreateArrayString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an array/access expression.
		void								CreateArrayString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an array/access expression.
		void								CreateArrayToEndString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp0, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an array/access expression.
		void								CreateArrayFromStartString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp0, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an array/access expression.
		void								CreateArrayAccess( const YYSTYPE::EE_NODE_DATA &_ndObj, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an array/access expression.
		void								CreateArrayAccessEx( const YYSTYPE::EE_NODE_DATA &_ndObj, size_t _sExp0, size_t _sExp1, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a postfix operator.
		void								CreatePostfixOp( size_t _sStrIndex, int32_t _eType, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a prefix operator.
		void								CreatePrefixOp( size_t _sStrIndex, int32_t _eType, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a hex constant (0x----).
		void								CreateHex1( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a hex constant (----h).
		void								CreateHex2( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a hex constant (----).
		void								CreateHex3( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a binary constant (0b----).
		void								CreateBin( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a decimal constant.
		void								CreateUInt( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a numeric constant.
		void								CreateNumber( uint64_t _uiVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a numeric constant.
		void								CreateNumber( int64_t _iVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a numeric constant.
		void								CreateNumber( uint32_t _uiVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a numeric constant.
		void								CreateNumber( int32_t _iVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a numeric constant.
		void								CreateNumber( float _fVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a numeric constant.
		void								CreateNumber( double _dVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a numeric constant.
		void								CreateNumber( long _lVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an oct constant.
		void								CreateOct( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a double constant.
		void								CreateDouble( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a double constant.
		void								CreateDouble( double _dVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a Unicode \N{} numeric constant.
		void								CreateUnicodeNumericConstant( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a reinterpretation of bits to a float.
		void								CreateAsFloat( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a reinterpretation of bits to a float of a common type.
		void								CreateAsFloat( const YYSTYPE::EE_NODE_DATA &_ndExp, int32_t _eType, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a reinterpretation of bits to a double.
		void								CreateAsDouble( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a reinterpretation of bits to a double with the given bit specifications.
		void								CreateAsFloatX( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
			const YYSTYPE::EE_NODE_DATA &_ndExpBits,
			const YYSTYPE::EE_NODE_DATA &_ndManBits,
			const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
			const YYSTYPE::EE_NODE_DATA &_ndSignValue,	// 0 or 1.
			const YYSTYPE::EE_NODE_DATA &_ndExpValue,
			const YYSTYPE::EE_NODE_DATA &_ndManValue,
			
			YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a reinterpretation of bits to a double with the given bit specifications.
		void								CreateAsFloatX( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
			const YYSTYPE::EE_NODE_DATA &_ndExpBits,
			const YYSTYPE::EE_NODE_DATA &_ndManBits,
			const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
			const YYSTYPE::EE_NODE_DATA &_ndExp,	// The double value to convert.
			
			YYSTYPE::EE_NODE_DATA &_ndNode );

		// Gets a property of a floating-point type with the given bit configuration.
		void								CreateAsFloatXProp( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
			const YYSTYPE::EE_NODE_DATA &_ndExpBits,
			const YYSTYPE::EE_NODE_DATA &_ndManBits,
			const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
			const EE_NODES _nProp,
			
			YYSTYPE::EE_NODE_DATA &_ndNode );

		// Gets a property of a floating-point type with the given bit configuration.
		void								CreateAsFloatXProp( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
			const YYSTYPE::EE_NODE_DATA &_ndExpBits,
			const YYSTYPE::EE_NODE_DATA &_ndManBits,
			const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
			const YYSTYPE::EE_NODE_DATA &_ndExp,	// The double value to convert.
			const EE_NODES _nProp,
			
			YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a cast node.
		void								CreateCast( const YYSTYPE::EE_NODE_DATA &_ndExp, EE_CAST_TYPES _ctCast, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create an address node.
		void								CreateAddress( const YYSTYPE::EE_NODE_DATA &_ndExp, EE_CAST_TYPES _ctCast, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates member access.
		void								CreateMemberAccess( const YYSTYPE::EE_NODE_DATA &_ndExp, size_t _sMember, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a user (??) node.
		void								CreateUser( YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a number parameter ($<decimal>) node.
		void								CreateNumberedParm( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a dynamic numbered parameter node.
		void								CreateDynamicParm( size_t _sStrIndex, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a dynamic numbered parameter node.
		void								CreateDynamicParmExp( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a node that gets the total numbered parameters.
		void								CreateParmTotal( YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a unary node.
		void								CreateUnary( const YYSTYPE::EE_NODE_DATA &_ndExp, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create an operator node.
		void								CreateOp( const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a conditional operator.
		void								CreateConditional( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an assignment operator.
		void								CreateAssignment( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndRight, uint32_t _uiOp, bool _bIsConst, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an assignment operator (to change a variable that has already been created.
		void								CreateReAssignment( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndRight, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an array.
		void								CreateArray( size_t _sStrIndex, uint32_t _ui32Backing, uint32_t _ui32BackingPersistence, const YYSTYPE::EE_NODE_DATA &_ndSize,
			size_t _sStartValueNodeIdx, size_t _sEndValueNodeIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an assignment operator to assign a value in an array.
		void								CreateArrayReAssignment( size_t _sArrayIndex, const YYSTYPE::EE_NODE_DATA &_ndArray, const YYSTYPE::EE_NODE_DATA &_ndValue, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an assignment operator to write to an external address.
		void								CreateAddressAssignment( EE_CAST_TYPES _ctCast, const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndValue, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a 0-parm intrinsic.
		void								CreateIntrinsic0( uint32_t _uiIntrinsic, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a 1-parm intrinsic.
		void								CreateIntrinsic1( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a 2-parm intrinsic.
		void								CreateIntrinsic2( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a 3-parm intrinsic.
		void								CreateIntrinsic3( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, const YYSTYPE::EE_NODE_DATA &_ndExp2, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a compound statement.
		void								CreateCompoundStatement( const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a while loop.
		void								CreateWhileLoop( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a for loop.
		void								CreateForLoop( const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, const YYSTYPE::EE_NODE_DATA &_ndExp2, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a for loop.
		void								CreateForLoop( const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a do-while loop.
		void								CreateDoWhileLoop( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a foreach declaration.
		void								CreateForEachDecl( size_t _sStrIndex, size_t _sArrayIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a foreach declaration.
		void								CreateForEachCustomDecl( size_t _sStrIndex, size_t _sArrayIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a foreach declaration.
		void								CreateForEachStringDecl( size_t _sStrIndex, size_t _sArrayIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a foreach loop.
		void								CreateForEachLoop( const YYSTYPE::EE_NODE_DATA &_ndDecl, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a foreach loop that runs over objects.
		void								CreateForEachObjLoop( const YYSTYPE::EE_NODE_DATA &_ndDecl, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a foreach loop that runs over strings.
		void								CreateForEachStrLoop( const YYSTYPE::EE_NODE_DATA &_ndDecl, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a selection.
		void								CreateSelectionStatement( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a selection.
		void								CreateSelectionStatement( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements0, const YYSTYPE::EE_NODE_DATA &_ndStatements1, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a continue.
		void								CreateContinue( YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a break.
		void								CreateBreak( YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an entry in an argument list.
		void								CreateArgListEntry( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an entry in an argument list.
		void								CreateArgListEntry( const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an arg list.
		void								CreateArgList( const YYSTYPE::EE_NODE_DATA &_ndList, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a format string in the format of: "Some string {}.".format( Args0, Arg1 ).
		void								CreateFormat( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndArgs, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Sets the translation-unit node.
		void								SetTrans( YYSTYPE::EE_NODE_DATA &_ndNode );

		// Determines if a node is constant and, if so, returns the constant value associated with the node.
		bool								IsConst( const YYSTYPE::EE_NODE_DATA &_ndExp, EE_RESULT &_rResult );

		// Are we breaking from a loop?
		bool								Breaking() const;

		// Break.  Returns false if not in a loop.
		bool								Break();

		// Are we continuing in a loop?
		bool								Continuing() const;

		// Continue.  Returns false if not in a loop.
		bool								Continue();

		// Are we breaking or continuing?
		bool								BreakingOrContinuing() const {
			if ( !m_vLoopStack.size() ) { return false; }
			auto aIdx = m_vLoopStack.size() - 1;
			return m_vLoopStack[aIdx].bBreak || m_vLoopStack[aIdx].bContinue;
		}


	protected :
		// == Types.
		// A custom variable.
		struct EE_CUSTOM_VAR {
			uint64_t						ui64UpdateCounter;
			EE_RESULT						rRes;
			bool							bIsConst;
		};

		// A for-loop stack.
		struct EE_LOOP_STACK {
			size_t							sCurIdx;
			bool							bBreak;
			bool							bContinue;

			EE_LOOP_STACK() :
				sCurIdx( 0 ),
				bBreak( false ),
				bContinue( false ) {
			}
		};

		// A structure for safely adding and removing loop scopes to the stack.
		struct EE_LOOP_STACK_ADDER {
			EE_LOOP_STACK_ADDER( std::vector<EE_LOOP_STACK> & _vStack, const EE_LOOP_STACK &_lsLoopStackItem ) :
				vStack( _vStack ) {
				vStack.push_back( _lsLoopStackItem );
			}
			~EE_LOOP_STACK_ADDER() {
				vStack.pop_back();
			}

			std::vector<EE_LOOP_STACK> &	vStack;
		};

		// An array and its metadata.
		struct EE_ARRAY_DATA {
			CArrayBase *					m_pabBase;
		};

		// For use with the explicit stack during parsing.
		struct EE_STACK_OBJ {
			// Counts the number of times the node has been processed.
			uint64_t						uiProcessCount;

			// Loop counter.
			uint64_t						uiLoopCounter;

			// Array of results for already-parsed sub-expressions.
			EE_RESULT						sSubResults[EE_MAX_SUB_EXPRESSIONS];

			// The result of the expression.
			EE_RESULT *						prResult;

			// For-each loop custom variable result.
			EE_RESULT *						prLoopCustomVarResult;

			// For-each loop object.
			EE_RESULT *						prLoopObject;

			// Index of the stack item.
			size_t							sIndex;

			// Index of the parent stack item.
			size_t							sParentIndex;

			// Loop stack index.
			size_t							sLoopStackIdx;

			// For-each string.
			size_t							sForEachString;

			// For-each string current position in the string.
			size_t							sForEachStringPos;			

			// Pointer 

			// To break processing into 2 parts.
			bool							bPassThrough;
		};

		// Encapsulate an object.
		/*struct EE_OBJECT {
			// The actual object pointer.
			CObject *						poObj;

			// The reference count,
			int64_t							i64RefCnt;


			EE_OBJECT() :
				poObj( nullptr ),
				i64RefCnt( 0 ) {
			}
			~EE_OBJECT() {
				//delete poObj;
			}
		};*/


		// == Members.
		// The lexer.
		CExpEvalLexer *						m_peelLexer;

		// The stack of nodes.
		std::vector<YYSTYPE::EE_NODE_DATA>	m_vNodes;

		// The stack of UTF-8 strings.
		std::vector<std::string>			m_vStrings;

		// The array of objects.
		std::vector<CObject *>				m_vObjects;

		// 1-for-1 array of string references to strings held inside m_vStrings;
		//std::vector<CStringRef *>			m_vStringObjectsU8;

		// 1-for-1 array of string references to strings held inside m_vStrings;
		//std::vector<CStringRef *>			m_vStringObjectsU16;

		// The map of custom variables. std::map<name_as_index, EE_CUSTOM_VAR>.
		std::map<size_t, EE_CUSTOM_VAR>		m_mCustomVariables;

		// The map of arrays.  std::map<name_as_index, index_into_array_data>.
		std::map<size_t, size_t>			m_mArrays;

		// The actual array data.
		std::vector<EE_ARRAY_DATA>			m_vArrayData;

		// The stack of loop scopes.
		std::vector<EE_LOOP_STACK>			m_vLoopStack;

		// The explicit resolve stack.
		std::vector<EE_STACK_OBJ>			m_vStack;

		// Numbered parameters.
		std::vector<EE_RESULT>				m_vNumberedParms;

		// Function parameters.
		std::vector<std::vector<EE_RESULT>>	m_vFuncParms;

		// Referenced numbered parameters.
		std::set<size_t>					m_sNumberedParmsAccessed;

		// Treate everything as hex?
		bool								m_bTreatAllAsHex;

		// Address handler.
		PfAddressHandler					m_pfahAddressHandler;

		// Address data.
		uintptr_t							m_uiptrAddressData;

		// Address write handler.
		PfAddressHandler					m_pfahAddressWriteHandler;

		// Address write data.
		uintptr_t							m_uiptrAddressWriteData;

		// User function.
		PfUserVarHandler					m_pfUser;
		
		// Data to pass to the user function.
		uintptr_t							m_uiptrUserData;

		// String handler.
		PfStringHandler						m_pfshString;

		// Data to pass to the string handler.
		uintptr_t							m_uiptrStringData;

		// Member-access handler.
		PfMemberAccessHandler				m_pfmahMemberAccess;

		// Data to pass to the member-access handler.
		uintptr_t							m_uiptrMemberAccess;

		// ToString() function.
		PfToStringHandler					m_pfToString;

		// Translation unit.
		size_t								m_sTrans;

		// The random device.
		std::random_device					m_rdRandDevice;

		// The generator.
		std::mt19937_64						m_mRandGen;

		// Was ?? used?
		bool								m_bHasUserVar;

		// Was the dereferece operator used?
		bool								m_bHasAddressRead;

		// Buffer for printing a float.
		//static wchar_t						m_szFloatPrintBuffer[2000+309+3];


		// == Functions.
		// Adds a node to its internal node tree.
		void								AddNode( YYSTYPE::EE_NODE_DATA &_ndNode );

		// Adds a string and returns its index into the stack.
		size_t								AddString( const std::string &_sText );

		// Resolves a node.
		bool								ResolveNode( size_t _sNode, EE_RESULT &_rRes );

		// Resolves a node.
		bool								ResolveNode_ExplicitStack( size_t _sNode, EE_RESULT &_rRes, EE_ERROR_CODES &_ecError );

		// Takes an array type and creates an array for the given type.
		CArrayBase *						CreateArrayFromType( uint32_t _ui32Type );

		// Creates a string reference given a string index.
		bool								CreateStringRef( size_t _stStrIdx );

		// Pushes a node onto the explicit stack.
		static bool							PushNode( std::vector<EE_STACK_OBJ> &_vStack, size_t _sNodeIdx, size_t _sParentIdx, EE_RESULT * _prResult ) {
			EE_STACK_OBJ soDoMeNext;
			soDoMeNext.bPassThrough = false;
			soDoMeNext.sIndex = _sNodeIdx;
			soDoMeNext.sParentIndex = _sParentIdx;
			soDoMeNext.prResult = _prResult;
			soDoMeNext.uiProcessCount = 0;

			/*uint32_t ui32ParentData = 0;
			if ( soDoMeNext.sParentIndex != EE_INVALID_IDX ) {
				ui32ParentData = _vStack[soDoMeNext.sParentIndex].u.ui32Data;
			}*/

			_vStack.push_back( soDoMeNext );
			return true;
		}

		// Shifting for floats.
		static double						LShift( double _dVal, double _dShift );

		// Shifting for floats.
		static double						RShift( double _dVal, double _dShift );

		// Converts a node from identifier to custom variable.
		static bool							ConvertIdentifierToCustomVar( YYSTYPE::EE_NODE_DATA &_ndNode ) {
			if ( _ndNode.nType != EE_N_STRING ) { return false; }
			_ndNode.nType = EE_N_CUSTOM_VAR;
			return true;
		}

		// Fills out a context structure for PrintF().
		static bool							FillContext( const wchar_t * _pwcFormat, size_t &_sPos, EE_PRINTF_CONTEXT &_pcContext, EE_NUM_CONSTANTS &_ncType );

		// Is the given character a valid PrintF() flag?
		static bool							IsPrintFFlag( wchar_t _wcFlag ) {
			return _wcFlag == L'+' ||
				_wcFlag == L'-' || _wcFlag == L'#' ||
				_wcFlag == L'0' || _wcFlag == L' ';
		}

		// Is the given character a valid PrintF() width specifier?
		static bool							IsPrintFWidth( wchar_t _wcFlag ) {
			return _wcFlag >= L'0' && _wcFlag <= L'9';
		}

		// Is the given character a valid PrintF() precision specifier?
		static bool							IsPrintFPrecision( wchar_t _wcFlag ) {
			return _wcFlag >= L'0' && _wcFlag <= L'9';
		}

		// Is the given character a valid PrintF() size specifier?
		static bool							IsPrintFArgSize( wchar_t _wcFlag ) {
			return _wcFlag == L'h' ||
				_wcFlag == L'j' || _wcFlag == L'l' ||
				_wcFlag == L'L' || _wcFlag == L't' ||
				_wcFlag == L'w' || _wcFlag == L'z' ||
				_wcFlag == L'I' || _wcFlag == L'3' ||
				_wcFlag == L'2' || _wcFlag == L'6' ||
				_wcFlag == L'4';
		}

		// Is the given character a valid PrintF() int-type?
		static bool							IsPrintFIntType( wchar_t _wcFlag ) {
			return _wcFlag == L'd' || _wcFlag == L'i' ||
				_wcFlag == L'o' || _wcFlag == L'u' ||
				_wcFlag == L'x' || _wcFlag == L'X' ||
				_wcFlag == L'c' || _wcFlag == L'C';
		}

		// Is the given character a valid PrintF() float-type?
		static bool							IsPrintFFloatType( wchar_t _wcFlag ) {
			return _wcFlag == L'a' || _wcFlag == L'A' ||
				_wcFlag == L'e' || _wcFlag == L'E' ||
				_wcFlag == L'f' || _wcFlag == L'F' ||
				_wcFlag == L'g' || _wcFlag == L'G';
		}

		// Eats the {..} part out of a string.  Assumes that _pcFormat points to the next character after the opening {.
		// Also assumes that from { to } is all standard ASCII, since no special characters are allowed inside valid formatters.
		static std::string					EatStringFormatter( const char * &_pcFormat, size_t &_stLen, size_t &_stArgIdx, const std::vector<EE_RESULT> &_vArgs );

	};

}	// namespace ee

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

	class CExpEvalContainer {
		friend class						CExpEvalParser;
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
			m_sTrans( 0 ),
			m_pfToString( nullptr ),
			m_bHasUserVar( false ),
			m_bHasAddressRead( false ) {
			m_mRandGen = std::mt19937_64( m_rdRandDevice() );
		}
		~CExpEvalContainer();


		// == Enumerations.
		/**
		 * \brief Error codes returned by expression-evaluation operations.
		 *
		 * These values describe failure or success states produced by parsing, evaluation,
		 * conversions, intrinsic execution, and API-level operations.
		 */
		enum EE_ERROR_CODES {
			EE_EC_SUCCESS,											/**< No problem. */

			EE_EC_INVALIDTREE,										/**< The syntax tree is invalid. */
			EE_EC_OUTOFMEMORY,										/**< Out of memory. */
			EE_EC_PROCESSINGERROR,									/**< General error. */
			
			EE_EC_RESULTSTOOSHORT,									/**< The sub-expression results array is too short. */
			EE_EC_NOIDENTIFIERHANDLER,								/**< No call to SetStringHandler() has been made with a non-nullptr value. */
			EE_EC_IDENTHANDLERFAILED,								/**< Identifier handler returned false. */
			
			EE_EC_VARNOTFOUND,										/**< Custom variable not found by name. */
			EE_EC_VARHASBADTYPE,									/**< Custom variable has an invalid type. */
			EE_EC_BADARRAYIDX,										/**< Array index out-of-range. */
			EE_EC_NOADDRESSHANDLER,									/**< No address handler. */
			EE_EC_ADDRESSHANDLERFAILED,								/**< Address handler failed. */
			
			EE_EC_NOMEMBERHANDLER,									/**< No call to SetMemberAccessHandler() has been made with a non-nullptr value. */
			EE_EC_MEMBERHANDELRFAILED,								/**< Member handler returned false. */
			EE_EC_NOUSERHANDLER,									/**< No call to SetUserHandler() has been made with a non-nullptr value. */
			EE_EC_USERHANDLERFAILED,								/**< User handler returned false. */
			
			EE_EC_FLOATWITHTILDE,									/**< The ~ cannot be used with floating-point values. */
			EE_EC_UNRECOGNIZEDUNARYOPERATOR,						/**< Unrecognized unary operator. */
			EE_EC_UNRECOGNIZEDINTRINSIC0,							/**< Unrecognized no-parameter intrinsic. */
			EE_EC_UNRECOGNIZEDINTRINSIC1,							/**< Unrecognized 1-parameter intrinsic. */
			EE_EC_UNRECOGNIZEDINTRINSIC2,							/**< Unrecognized 2-parameter intrinsic. */
			EE_EC_UNRECOGNIZEDINTRINSIC3,							/**< Unrecognized 3-parameter intrinsic. */

			EE_EC_TOOMANYSIGNBITS,									/**< Too many sign bits in a custom float. */
			EE_EC_TOOMANYEXPBITS,									/**< Too many exponent bits in a custom float. */
			EE_EC_TOOMANYMANTISSABITS,								/**< Too many mantissa bits in a custom float. */

			EE_EC_INVALILOOPCONDITIONEXP,							/**< Invalid expression inside a loop condition. */
			EE_EC_INVALIDLOOPDECL,									/**< Invalid declaration inside a loop. */

			EE_EC_ARRAYACCESSERROR,									/**< Error accessing the contents of an array for a for-each loop. */

			EE_EC_ERRORPROCESSINGOP,								/**< An invalid operator was submitted. */

			EE_EC_OPMUSTNOTBEZERO,									/**< 0 not allow on the right-hand side of an op (X / 0, X % 0, etc.) */

			EE_EC_INVALIDOPERATOR,									/**< Invalid operation, such as "~0.3". */

			EE_EC_INVALIDAPIOPERATION,								/**< Invalid operation that is part of the base API implementation and is expected to be working. */

			EE_EC_INVALID_WRITE_TO_CONST,							/**< Invalid attempt to write to a custom variable declared with "const". */

			EE_EC_CONST_VAR_REQUIRES_CONST_EPRESSION,				/**< A const variable was attempted to be created but was not assigned a const expression. */

			EE_EC_NONCONSTNOTALLOWED,								/**< A call to resolve a non-constant expression could have succeeded but failed due to passed-in flags. */

			EE_EC_INVALIDCAST,										/**< An object failed to cast into another object or primitive, or could not be created from one. */
			EE_EC_ARRAY_FROM_NON_OBJECT,							/**< An array access was attempted on a type that is not an object. */

			EE_EC_INVALID_OBJECT,									/**< Operation performed on an object of an invalid type. */

			EE_EC_UNIMPLEMENTED,									/**< Error for development to indicate that a feature is not implemented yet. */
		};

		
		// == Types.
		/** The result structure. */
		struct EE_RESULT {
			EE_NUM_CONSTANTS				ncType;
			union {
				int64_t						i64Val;
				uint64_t					ui64Val;
				double						dVal;
				CObject *					poObj;
			}								u;
		};

		/** Address ([<address>]) handler. */
		typedef bool (EE_CALLBACK *			PfAddressHandler)( uint64_t _ui64Address, EE_CAST_TYPES _tType, uintptr_t _uiptrData, CExpEvalContainer * _peecContainer, EE_RESULT &_rResult );

		/** User-variable function handler. */
		typedef bool (EE_CALLBACK *			PfUserVarHandler)( uintptr_t _uiptrData, CExpEvalContainer * _peecContainer, EE_RESULT &_rResult );

		/** String handler. */
		typedef bool (EE_CALLBACK *			PfStringHandler)( const std::string &_sString, uintptr_t _uiptrData, CExpEvalContainer * _peecContainer, EE_RESULT &_rResult );

		/** Member-access handler. */
		typedef bool (EE_CALLBACK *			PfMemberAccessHandler)( const EE_RESULT &_rLeft, const std::string &_sMember, uintptr_t _uiptrData, CExpEvalContainer * _peecContainer, EE_RESULT &_rResult );

		/** ToString() function. */
		typedef std::wstring (EE_CALLBACK *	PfToStringHandler)( EE_RESULT &_rResult, uint64_t _ui64Options );


		// == Functions.
		/**
		 * Gets the result.  If the function returns false then the expression is invalid.  If it returns true but the return type is EE_NC_INVALID, the expression is valid but was unable to be resolved for some reason.
		 * 
		 * \param _rRes Holds the returned result after evaluating the already-parsed expression.
		 * \return Returns true if the expression has been parsed.
		 **/
		bool								Resolve( EE_RESULT &_rRes );

		/**
		 * Callsd when parsing is finished.
		 **/
		void								Parsed() {
			m_vsCompileTimeObjects = m_vObjects.size();
		}

		/**
		 * Do we treat everything as hex?
		 * 
		 * \return Returns true if decimal and octal numbers are treated as hexadecimal by default.
		 **/
		bool								TreatAllAsHex() const { return m_bTreatAllAsHex; }

		/**
		 * Set whether to treat all as hex or not.
		 * 
		 * \param _bVal If true, decimal and octal numbers are treated as hexadecimal by default.
		 **/
		void								SetTreatAsHex( bool _bVal ) { m_bTreatAllAsHex = _bVal; }

		/**
		 * Sets the address handler.
		 * 
		 * \param _pfHandler The handler call-back function.
		 * \param _uiptrData Data to be passed to the call-back function.  It is always pointer-width.
		 **/
		void								SetAddressHandler( PfAddressHandler _pfHandler, uintptr_t _uiptrData ) { m_pfahAddressHandler = _pfHandler; m_uiptrAddressData = _uiptrData; }

		/**
		 * Sets the address write handler.
		 * 
		 * \param _pfHandler The handler call-back function.
		 * \param _uiptrData Data to be passed to the call-back function.  It is always pointer-width.
		 **/
		void								SetAddressWriteHandler( PfAddressHandler _pfHandler, uintptr_t _uiptrData ) { m_pfahAddressWriteHandler = _pfHandler; m_uiptrAddressWriteData = _uiptrData; }

		/**
		 * Sets the user data handler.
		 * 
		 * \param _pfHandler The handler call-back function.
		 * \param _uiptrData Data to be passed to the call-back function.  It is always pointer-width.
		 **/
		void								SetUserHandler( PfUserVarHandler _pfHandler, uintptr_t _uiptrData ) { m_pfUser = _pfHandler; m_uiptrUserData = _uiptrData; }

		/**
		 * Sets the string handler.
		 * 
		 * \param _pfHandler The handler call-back function.
		 * \param _uiptrData Data to be passed to the call-back function.  It is always pointer-width.
		 **/
		void								SetStringHandler( PfStringHandler _pfHandler, uintptr_t _uiptrData ) { m_pfshString = _pfHandler; m_uiptrStringData = _uiptrData; }

		/**
		 * Sets the identifier handler.
		 * 
		 * \param _pfHandler The handler call-back function.
		 * \param _uiptrData Data to be passed to the call-back function.  It is always pointer-width.
		 **/
		void								SetMemberAccessHandler( PfMemberAccessHandler _pfHandler, uintptr_t _uiptrData ) { m_pfmahMemberAccess = _pfHandler; m_uiptrMemberAccess = _uiptrData; }

		/**
		 * Sets the ToString() handler.
		 * 
		 * \param _pfHandler The handler call-back function.
		 **/
		void								SetToStringHandler( PfToStringHandler _pfHandler ) { m_pfToString = _pfHandler; }

		/**
		 * Sets the lexer to NULL.  Call after the expression has been successfully parsed.
		 **/
		void								ExpWasParsed() { m_peelLexer = nullptr; }

		/**
		 * Gets the user data passed in SetUserHandler()/SetUserData().
		 * 
		 * \return Returns the user data passed in SetUserHandler()/SetUserData().
		 **/
		uintptr_t							UserData() const { return m_uiptrUserData; }

		/**
		 * Sets the user data to be used by the handler passed in SetUserHandler()/SetUserData().
		 * 
		 * \param _uiptrData Data to be passed to the call-back function.  It is always pointer-width.
		 **/
		void								SetUserData( uintptr_t _uiptrData ) { m_uiptrUserData = _uiptrData; }

		/**
		 * Does the compilsed script have a call to "??"?
		 * 
		 * \return Returns true if the script uses ??.
		 **/
		bool								HasAccessToUserData() const { return m_bHasUserVar; }

		/**
		 * Does the compiled script have a dereference?
		 * 
		 * \return Returns true if the script uses a dereference, such as ui8[<address>].
		 **/
		bool								HasAddressRead() const { return m_bHasAddressRead; }

		/**
		 * Sets a numbered ($x) parameter.
		 * 
		 * \param _sIdx The index of the parameter to set.
		 * \param _rValue The value to assign to the parameter.
		 * \return Returns true if there was enough memory available to create the parameter index.
		 **/
		bool								SetNumberedParm( size_t _sIdx, const EE_RESULT &_rValue );

		/**
		 * Gets a numbered parameter.
		 * 
		 * \param _sIdx The index of the parameter to get.
		 * \return Returns EE_NC_UNSIGNED with a value of 0 if the index is invalid, otherwise it returns the parameter at the given index.
		 **/
		EE_RESULT							GetNumberedParm( size_t _sIdx ) {
			return _sIdx >= m_vNumberedParms.size() ?
				DefaultResult() :
				m_vNumberedParms[_sIdx];
		}

		/**
		 * Gets the total numbered parameters ($$).
		 * 
		 * \return Returns the total numbered parameters ($$).
		 **/
		__inline size_t						TotalNumberedParms() const { return m_vNumberedParms.size(); }

		/**
		 * Determines if a numbered parameter ($x) is accessed by the script.
		 * 
		 * \param _sIdx The parameter index to check for access.
		 * \return Returns true if the given parameter index is accessed by the script.
		 **/
		__inline bool						IsNumberedParmAccessed( size_t _sIdx ) const { return m_sNumberedParmsAccessed.find( _sIdx ) != m_sNumberedParmsAccessed.end(); }

		/**
		 * Determines if any numbered parameter ($x) is accessed by the script.
		 * 
		 * \return Returns true if any numbered parameter is accessed by the script.
		 **/
		__inline bool						IsNumberedParmAccessed() const { return m_sNumberedParmsAccessed.size() != 0; }

		/**
		 * Determines if dynamic access to the numbered parameters is used within the script.
		 * 
		 * \return Returns true if a numbered parameter is accessed via a dynamix run-time index.
		 **/
		__inline bool						IsDynamicNumberedParmAccessed() const { return IsNumberedParmAccessed( static_cast<size_t>(-1) ); }

		/**
		 * Gets the type to use between 2 given types.
		 * 
		 * \param _ncLeft The left operand.
		 * \param _ncRight The right operand.
		 * \return Using C++ promotion rules, a common cast type for the given values is determined.
		 **/
		static EE_NUM_CONSTANTS				GetCastType( EE_NUM_CONSTANTS _ncLeft, EE_NUM_CONSTANTS _ncRight );

		/**
		 * Converts a result to a given type.  Only basic types (no objects) can be converted)
		 * 
		 * \param _rRes The result to convert.
		 * \param _ncType The new type of the result.
		 * \return Returns a new result that represents the conversion from the _rRes type to _ncType, or an EE_NC_INVALID return if the types are incompatible completely.
		 **/
		static EE_RESULT					ConvertResult( const EE_RESULT &_rRes, EE_NUM_CONSTANTS _ncType );

		/**
		 * Converts a result or object to a given type.  Same as ConvertResult() but it can convert objects as well.
		 * 
		 * \param _rRes The result to convert.
		 * \param _ncType The new type of the result.
		 * \return Returns a new result that represents the conversion from the _rRes type to _ncType, or an EE_NC_INVALID return if the types are incompatible completely.
		 **/
		EE_RESULT							ConvertResultOrObject( const EE_RESULT &_rRes, EE_NUM_CONSTANTS _ncType );

		/**
		 * Performs ToString() on a given result or object.
		 *
		 * \param _rRes The result to convert to a string.
		 * \param _sReturn The resulting string form of the given result.
		 * \return Returns true of a to-string conversion was made.
		 */
		bool								ToStringResultOrObject( const EE_RESULT &_rRes, std::string &_sReturn, uint32_t _ui32Depth, uint32_t _ui32Flags = 0 );

		/**
		 * Performs ToString() on a given result or object.
		 *
		 * \param _rRes The result to convert to a string.
		 * \param _sFormat The format string.
		 * \param _sReturn The resulting string form of the given result.
		 * \return Returns true of a to-string conversion was made.
		 */
		bool								ToFormatStringResultOrObject( const EE_RESULT &_rRes, const std::string &_sFormat, std::string &_sReturn, uint32_t _ui32Flags = 0 );

		/**
		 * Prforms an equality comparison on a pair of given results or objects.
		 *
		 * \param _rLeft The left operand.
		 * \param _rRight The right operand.
		 * \return If either result is an object, Obj->Equals() is called on te pair, otherwise a comparison between
		 */
		bool								EqualResultOrObject( const EE_RESULT &_rLeft, const EE_RESULT &_rRight );

		/**
		 * \brief Gets the default result value.
		 *
		 * The default result is an unsigned integer with a value of 0.
		 *
		 * \return Returns a default-initialized \c EE_RESULT.
		 */
		static EE_RESULT					DefaultResult() { EE_RESULT rRes; rRes.ncType = EE_NC_UNSIGNED; rRes.u.ui64Val = 0UL; return rRes; }

		/**
		 * \brief Default \c ToString() implementation.
		 *
		 * Converts a result into a display string using the default formatting rules.
		 *
		 * \param _rResult The result to format.
		 * \param _ui32Depth Current formatting depth (used to limit recursion).
		 * \param _ui64Options Formatting options.
		 * \return Returns a formatted string representation of \c _rResult.
		 */
		static std::wstring					DefaultToString( EE_RESULT &_rResult, uint32_t _ui32Depth, uint64_t _ui64Options );

		/**
		 * \brief Returns the index of a string if it exists.
		 *
		 * \param _sText The string to search for.
		 * \return Returns the index of \c _sText if found; otherwise returns \c static_cast<size_t>(-1).
		 */
		size_t								HasString( const std::string &_sText ) const;

		/**
		 * \brief Returns the index of a custom variable if it exists.
		 *
		 * \param _sText The custom variable name to search for.
		 * \return Returns the index of \c _sText if found; otherwise returns \c static_cast<size_t>(-1).
		 */
		size_t								HasCustomVar( const std::string &_sText ) const;

		/**
		 * \brief Determines whether the given text is a custom variable name.
		 *
		 * \param _sText The text to test.
		 * \return Returns true if \c _sText identifies a custom variable; otherwise false.
		 */
		bool								IsCustomVar( const std::string &_sText ) const;

		/**
		 * \brief Returns the index of an array if it exists.
		 *
		 * \param _sText The array name to search for.
		 * \return Returns the index of \c _sText if found; otherwise returns \c static_cast<size_t>(-1).
		 */
		size_t								HasArray( const std::string &_sText ) const;

		/**
		 * \brief Determines whether the given text refers to an array.
		 *
		 * \param _sText The text to test.
		 * \return Returns true if \c _sText identifies an array; otherwise false.
		 */
		bool								IsArray( const std::string &_sText ) const;

		/**
		 * \brief Gets the string version of the given value.
		 *
		 * \param _rResult The result to stringify.
		 * \param _ui32Depth Current formatting depth (used to limit recursion).
		 * \param _ui64Options Formatting options.
		 * \return Returns a string representation of \c _rResult.
		 */
		std::wstring						ToString( EE_RESULT &_rResult, uint32_t _ui32Depth, uint64_t _ui64Options );

		/**
		 * \brief Prints a formatted string to a wide-character destination buffer.
		 *
		 * All variadic arguments must be of type \c const \c EE_RESULT.
		 *
		 * \param _pwcDst Destination buffer.
		 * \param _sMaxLen Maximum number of wide characters writable to \c _pwcDst.
		 * \param _pwcFormat Format string.
		 * \param _iArgs Number of variadic arguments.
		 * \param ... Variadic argument list. Each argument must be a \c const \c EE_RESULT.
		 * \return Returns the number of characters written (or that would have been written), matching the underlying formatter's behavior.
		 */
		static int __cdecl					PrintF( wchar_t * _pwcDst, size_t _sMaxLen,
			const wchar_t * _pwcFormat, int _iArgs, ... );

		/**
		 * \brief Prints a formatted string to a wide-character destination buffer using a \c va_list.
		 *
		 * All arguments described by \c _vaArgPtr must be of type \c const \c EE_RESULT.
		 *
		 * \param _pwcDst Destination buffer.
		 * \param _sMaxLen Maximum number of wide characters writable to \c _pwcDst.
		 * \param _pwcFormat Format string.
		 * \param _iArgs Number of arguments in \c _vaArgPtr.
		 * \param _vaArgPtr Variadic argument list. Each argument must be a \c const \c EE_RESULT.
		 * \return Returns the number of characters written (or that would have been written), matching the underlying formatter's behavior.
		 */
		static int __cdecl					PrintFVarArgs( wchar_t * _pwcDst, size_t _sMaxLen,
			const wchar_t * _pwcFormat, int _iArgs, va_list _vaArgPtr );

		/**
		 * \brief Prints a formatted string to a wide-character destination buffer.
		 *
		 * \param _pwcDst Destination buffer.
		 * \param _sMaxLen Maximum number of wide characters writable to \c _pwcDst.
		 * \param _pwcFormat Format string.
		 * \param _vParms Parameters used for formatting.
		 * \return Returns the number of characters written (or that would have been written), matching the underlying formatter's behavior.
		 */
		static int __cdecl					PrintF( wchar_t * _pwcDst, size_t _sMaxLen,
			const wchar_t * _pwcFormat, const std::vector<EE_RESULT> &_vParms );

		/**
		 * \brief Performs an operation on two results using the given operator.
		 *
		 * \param _rLeft The left operand.
		 * \param _uiOp The operator (implementation-defined token/value).
		 * \param _rRight The right operand.
		 * \param _rResult Receives the operation result.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
		EE_ERROR_CODES						PerformOp( EE_RESULT _rLeft, uint32_t _uiOp, EE_RESULT _rRight, EE_RESULT &_rResult );

		/**
		 * \brief Performs an operation on two results using the given operator (static form).
		 *
		 * \param _rLeft The left operand.
		 * \param _uiOp The operator (implementation-defined token/value).
		 * \param _rRight The right operand.
		 * \param _rResult Receives the operation result.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
		static EE_ERROR_CODES				PerformOp_S( EE_RESULT _rLeft, uint32_t _uiOp, EE_RESULT _rRight, EE_RESULT &_rResult );

		/**
		 * \brief Applies a unary operation to a result.
		 *
		 * Supported unary operators are implementation-defined; this implementation handles
		 * \c '+', \c '-', \c '~', and \c '!'.
		 *
		 * \param _rExp The operand.
		 * \param _uiOp The unary operator.
		 * \param _rResult Receives the result of the operation.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
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
						default : {}
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
						default : {}
					}
					return EE_EC_UNRECOGNIZEDUNARYOPERATOR;
				}
				case '!' : {
					switch ( _rExp.ncType ) {
						case EE_NC_SIGNED : {
							_rResult.ncType = _rExp.ncType;
							_rResult.u.i64Val = bool( !_rExp.u.i64Val );
							return EE_EC_SUCCESS;
						}
						case EE_NC_UNSIGNED : {
							_rResult.ncType = _rExp.ncType;
							_rResult.u.ui64Val = bool( !_rExp.u.ui64Val );
							return EE_EC_SUCCESS;
						}
						case EE_NC_FLOATING : {
							_rResult.ncType = EE_NC_UNSIGNED;
							_rResult.u.ui64Val = !_rExp.u.dVal;
							return EE_EC_SUCCESS;
						}
						default : {}
					}
					return EE_EC_UNRECOGNIZEDUNARYOPERATOR;
				}
				default : {}
			}
			return EE_EC_UNRECOGNIZEDUNARYOPERATOR;
		}

		/**
		 * \brief Applies a 1-parameter intrinsic to a result.
		 *
		 * \param _rExp The intrinsic input.
		 * \param _uiIntrinsic The intrinsic identifier.
		 * \param _rResult Receives the intrinsic result.
		 * \param _ui32Depth Current evaluation depth (used to limit recursion).
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
		EE_ERROR_CODES						PerformIntrinsic( EE_RESULT _rExp, uint32_t _uiIntrinsic, EE_RESULT &_rResult, uint32_t _ui32Depth = 0 );

		/**
		 * \brief Applies a 2-parameter intrinsic to two results.
		 *
		 * \param _rExp0 The first intrinsic input.
		 * \param _rExp1 The second intrinsic input.
		 * \param _uiIntrinsic The intrinsic identifier.
		 * \param _rResult Receives the intrinsic result.
		 * \param _bIncludeNonConst Indicates whether non-constant operations are permitted for this call.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
		EE_ERROR_CODES						PerformIntrinsic( EE_RESULT _rExp0, EE_RESULT _rExp1, uint32_t _uiIntrinsic, EE_RESULT &_rResult, bool _bIncludeNonConst );

		/**
		 * \brief Applies a 3-parameter intrinsic to three results.
		 *
		 * \param _rExp0 The first intrinsic input.
		 * \param _rExp1 The second intrinsic input.
		 * \param _rExp2 The third intrinsic input.
		 * \param _uiIntrinsic The intrinsic identifier.
		 * \param _rResult Receives the intrinsic result.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
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

		/**
		 * \brief Reinterprets a 32-bit integer bit pattern as a 32-bit float.
		 *
		 * If \c _rExp is integral, the low 32 bits are reinterpreted as \c float.
		 * If \c _rExp is floating-point, the value is cast to \c float (no reinterpretation of double bits).
		 *
		 * \param _rExp The source value.
		 * \param _rResult Receives the floating-point result.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
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

		/**
		 * \brief Reinterprets a 64-bit integer bit pattern as a 64-bit float.
		 *
		 * If \c _rExp is integral, the 64-bit pattern is reinterpreted as \c double.
		 * If \c _rExp is floating-point, the value is returned unchanged.
		 *
		 * \param _rExp The source value.
		 * \param _rResult Receives the floating-point result.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
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

		/**
		 * \brief Creates a floating-point value from explicit component parameters.
		 *
		 * \param _rTempSignBits Number of sign bits.
		 * \param _rTempExpBits Number of exponent bits.
		 * \param _rTempManBits Number of mantissa bits.
		 * \param _rTempImplied Non-zero if the mantissa has an implied leading bit.
		 * \param _rTempSignVal Sign value.
		 * \param _rTempExpVal Exponent value.
		 * \param _rTempManVal Mantissa value.
		 * \param _rResult Receives the resulting floating-point value.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
		EE_ERROR_CODES						PerformFloatX( EE_RESULT _rTempSignBits, EE_RESULT _rTempExpBits,
			EE_RESULT _rTempManBits, EE_RESULT _rTempImplied, EE_RESULT _rTempSignVal, EE_RESULT _rTempExpVal, EE_RESULT _rTempManVal,
			EE_RESULT &_rResult );

		/**
		 * \brief Creates a floating-point value from a double input using explicit format parameters.
		 *
		 * \param _rTempSignBits Number of sign bits.
		 * \param _rTempExpBits Number of exponent bits.
		 * \param _rTempManBits Number of mantissa bits.
		 * \param _rTempImplied Non-zero if the mantissa has an implied leading bit.
		 * \param _rTempDoubleVal Input value.
		 * \param _rResult Receives the resulting floating-point value.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
		EE_ERROR_CODES						PerformFloatX( EE_RESULT _rTempSignBits, EE_RESULT _rTempExpBits,
			EE_RESULT _rTempManBits, EE_RESULT _rTempImplied, EE_RESULT _rTempDoubleVal,
			EE_RESULT &_rResult );
		
		/**
		 * \brief Converts the given value to an X-bit floating-point format.
		 *
		 * The result is returned as \c EE_NC_FLOATING with the converted value stored in \c u.dVal.
		 *
		 * \param _rTempDoubleVal The source value (floating, signed, or unsigned).
		 * \param _uiExpBits Exponent bit count.
		 * \param _uiManBits Mantissa bit count.
		 * \param _bImplicitMantissaBit True if the mantissa has an implied leading bit.
		 * \param _bHasSign True if the format contains a sign bit.
		 * \param _rResult Receives the converted value.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
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

		/**
		 * \brief Converts the given value to a 16-bit float representation.
		 *
		 * \param _rTempDoubleVal The source value.
		 * \param _rResult Receives the converted value.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
		inline EE_ERROR_CODES				PerformToFloat16( EE_RESULT _rTempDoubleVal, EE_RESULT &_rResult ) {
			return PerformToFloatX( _rTempDoubleVal, 5, 11, true, true, _rResult );
		}

		/**
		 * \brief Converts the given value to a 14-bit float representation.
		 *
		 * \param _rTempDoubleVal The source value.
		 * \param _rResult Receives the converted value.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
		inline EE_ERROR_CODES				PerformToFloat14( EE_RESULT _rTempDoubleVal, EE_RESULT &_rResult ) {
			return PerformToFloatX( _rTempDoubleVal, 5, 10, true, false, _rResult );
		}

		/**
		 * \brief Converts the given value to an 11-bit float representation.
		 *
		 * \param _rTempDoubleVal The source value.
		 * \param _rResult Receives the converted value.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
		inline EE_ERROR_CODES				PerformToFloat11( EE_RESULT _rTempDoubleVal, EE_RESULT &_rResult ) {
			return PerformToFloatX( _rTempDoubleVal, 5, 7, true, false, _rResult );
		}

		/**
		 * \brief Converts the given value to a 10-bit float representation.
		 *
		 * \param _rTempDoubleVal The source value.
		 * \param _rResult Receives the converted value.
		 * \return Returns an \c EE_ERROR_CODES value describing success or failure.
		 */
		inline EE_ERROR_CODES				PerformToFloat10( EE_RESULT _rTempDoubleVal, EE_RESULT &_rResult ) {
			return PerformToFloatX( _rTempDoubleVal, 5, 6, true, false, _rResult );
		}

		/**
		 * \brief Allocates a new object of the given type and registers it for tracking.
		 *
		 * \tparam _tType The object type to allocate.
		 * \param _ui32Flags Optional allocation flags (currently unused).
		 * \return Returns a pointer to the allocated object on success; otherwise returns \c nullptr.
		 */
		template <typename _tType>
		_tType *							AllocateObject( uint32_t _ui32Flags = 0 ) {
			static_cast<void>(_ui32Flags);
			try {
				//auto aIdx = m_vObjects.size();
				_tType * ptObj = new( std::nothrow ) _tType( this );
				if ( !ptObj ) { return nullptr; }
				m_vObjects.push_back( ptObj );
				return ptObj;
			}
			catch ( const std::bad_alloc /*& _eE*/ ) { return nullptr; }
		}

		/**
		 * \brief Deallocates an object previously allocated by this container.
		 *
		 * \param _poObj The object to deallocate.
		 * \return Returns true if the object was found and deallocated; otherwise false.
		 */
		bool								DeallocateObject( CObject * _poObj );

		/**
		 * \brief Formats a string using a format buffer and a list of evaluated arguments.
		 *
		 * \param _pcFormat Pointer to the format string.
		 * \param _stLen Length of \c _pcFormat in bytes (0 means null-terminated, if supported by the implementation).
		 * \param _vArgs Arguments used for formatting.
		 * \return Returns the formatted string.
		 */
		std::string							FormatString( const char * _pcFormat, size_t _stLen, const std::vector<EE_RESULT> &_vArgs );
		
	protected :
		// == Types.
		// The PrintF() '%' context information.
		struct EE_PRINTF_CONTEXT {
			wchar_t							wcFormat[5+20+1+20];
		};

		// == Functions.
		/**
		 * \brief Decodes and stores a string literal.
		 *
		 * The text is decoded (ex: escape sequences) and stored in the internal string table.
		 *
		 * \param _sText The source string text to decode and store.
		 * \return Returns the index of the stored string in the internal string table.
		 */
		size_t								CreateString( const std::string &_sText );

		/**
		 * \brief Decodes and stores an identifier.
		 *
		 * The identifier text is decoded/stored in the internal string table and can be referenced by index.
		 *
		 * \param _pcText The identifier text.
		 * \return Returns the index of the stored identifier string in the internal string table.
		 */
		size_t								CreateIdentifier( const char * _pcText );

		/**
		 * \brief Creates a string expression node.
		 *
		 * Populates \c _ndNode to represent a string constant referenced by \c _sStrIndex.
		 *
		 * \param _sStrIndex Index into the internal string table.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateStringBasicExp( size_t _sStrIndex, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a custom-variable expression node.
		 *
		 * Populates \c _ndNode to represent a custom variable referenced by \c _sStrIndex.
		 *
		 * \param _sStrIndex Index into the internal string table for the variable name.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateCustomVar( size_t _sStrIndex, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an array-variable access expression node.
		 *
		 * Builds a node that represents an array access where the array name is referenced
		 * by \c _sStrIndex and the index expression is provided in \c _ndExp.
		 *
		 * \param _sStrIndex Index into the internal string table for the array name.
		 * \param _ndExp Index expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayVar( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a string array/slice access expression node with one index expression.
		 *
		 * Builds a node that represents string access where the string is referenced by \c _sStrIndex
		 * and the index expression is provided in \c _ndExp.
		 *
		 * \param _sStrIndex Index into the internal string table for the string reference.
		 * \param _ndExp Index expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a string slice access expression node with two index expressions.
		 *
		 * Builds a node that represents string slicing with optional start/end indices.
		 *
		 * \param _sStrIndex Index into the internal string table for the string reference.
		 * \param _ndExp0 Start/index-0 expression node data.
		 * \param _ndExp1 End/index-1 expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a string slice expression from a given index to the end.
		 *
		 * \param _sStrIndex Index into the internal string table for the string reference.
		 * \param _ndExp0 Start expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayToEndString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp0, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a string slice expression from the beginning to a given index.
		 *
		 * \param _sStrIndex Index into the internal string table for the string reference.
		 * \param _ndExp0 End expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayFromStartString( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndExp0, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a generic array access expression node.
		 *
		 * Builds a node that represents array access on an evaluated object expression \c _ndObj
		 * using index expression \c _ndExp.
		 *
		 * \param _ndObj The object expression node data.
		 * \param _ndExp The index expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayAccess( const YYSTYPE::EE_NODE_DATA &_ndObj, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a generic extended array access expression node.
		 *
		 * Builds a node that represents extended array access/slicing on an evaluated object expression \c _ndObj
		 * using two stored sub-expression indices.
		 *
		 * \param _ndObj The object expression node data.
		 * \param _sExp0 Index into the sub-expression results array for the first index expression.
		 * \param _sExp1 Index into the sub-expression results array for the second index expression.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayAccessEx( const YYSTYPE::EE_NODE_DATA &_ndObj, size_t _sExp0, size_t _sExp1, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a postfix operator expression node.
		 *
		 * \param _sStrIndex Index into the internal string table for the operator text (or token spelling).
		 * \param _eType The postfix operator type/token.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreatePostfixOp( size_t _sStrIndex, int32_t _eType, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a prefix operator expression node.
		 *
		 * \param _sStrIndex Index into the internal string table for the operator text (or token spelling).
		 * \param _eType The prefix operator type/token.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreatePrefixOp( size_t _sStrIndex, int32_t _eType, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a hexadecimal constant from a \c 0x-prefixed string.
		 *
		 * \param _pcText Text containing the hexadecimal constant.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateHex1( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a hexadecimal constant from an \c h-suffixed string.
		 *
		 * \param _pcText Text containing the hexadecimal constant.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateHex2( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a hexadecimal constant from an unsuffixed hexadecimal string.
		 *
		 * \param _pcText Text containing the hexadecimal constant.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateHex3( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a binary constant from a \c 0b-prefixed string.
		 *
		 * \param _pcText Text containing the binary constant.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateBin( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an unsigned decimal constant from text.
		 *
		 * \param _pcText Text containing the decimal constant.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateUInt( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an unsigned 64-bit numeric constant.
		 *
		 * \param _uiVal The value to store.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateNumber( uint64_t _uiVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a signed 64-bit numeric constant.
		 *
		 * \param _iVal The value to store.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateNumber( int64_t _iVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an unsigned 32-bit numeric constant.
		 *
		 * \param _uiVal The value to store.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateNumber( uint32_t _uiVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a signed 32-bit numeric constant.
		 *
		 * \param _iVal The value to store.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateNumber( int32_t _iVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a 32-bit floating-point numeric constant.
		 *
		 * \param _fVal The value to store.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateNumber( float _fVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a 64-bit floating-point numeric constant.
		 *
		 * \param _dVal The value to store.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateNumber( double _dVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a numeric constant from a \c long value.
		 *
		 * \param _lVal The value to store.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateNumber( long _lVal, YYSTYPE::EE_NODE_DATA &_ndNode );
		
#ifdef __APPLE__
		/**
		 * \brief Creates a numeric constant from a \c long \c double value.
		 *
		 * \param _dVal The value to store.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateNumber( long double _dVal, YYSTYPE::EE_NODE_DATA &_ndNode );
		
		/**
		 * \brief Creates a numeric constant from a \c ::clock_t value.
		 *
		 * \param _cVal The value to store.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateNumber( ::clock_t _cVal, YYSTYPE::EE_NODE_DATA &_ndNode );
#endif	// #ifdef __APPLE__

		/**
		 * \brief Creates an octal constant from text.
		 *
		 * \param _pcText Text containing the octal constant.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateOct( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a double constant from text.
		 *
		 * \param _pcText Text containing the floating-point constant.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateDouble( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a double constant.
		 *
		 * \param _dVal The value to store.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateDouble( double _dVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a Unicode \c \\N{} numeric constant node.
		 *
		 * Decodes a \c \\N{...} escape/name sequence from \c _pcText and produces a numeric constant
		 * node representing the resolved Unicode code point (or an invalid node on failure).
		 *
		 * \param _pcText Source text containing the \c \\N{...} construct.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateUnicodeNumericConstant( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an as-float node (reinterpret bits as \c float).
		 *
		 * Produces a node that reinterprets the input expression's bits as a 32-bit float.
		 *
		 * \param _ndExp Expression node to reinterpret.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateAsFloat( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an as-float node (reinterpret bits as \c float) for a common source type.
		 *
		 * Produces a node that reinterprets the input expression's bits as a 32-bit float, with
		 * \c _eType describing the source/common type classification used during parsing.
		 *
		 * \param _ndExp Expression node to reinterpret.
		 * \param _eType The source/common type token used to guide node creation.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateAsFloat( const YYSTYPE::EE_NODE_DATA &_ndExp, int32_t _eType, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an as-double node (reinterpret bits as \c double).
		 *
		 * Produces a node that reinterprets the input expression's bits as a 64-bit float.
		 *
		 * \param _ndExp Expression node to reinterpret.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateAsDouble( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a FloatX node from explicit bit-field specifications.
		 *
		 * Produces a node that constructs a floating-point value from explicit sign/exponent/mantissa
		 * bit sizes and provided component values.
		 *
		 * \param _ndSignBits Expression providing the number of sign bits.
		 * \param _ndExpBits Expression providing the number of exponent bits.
		 * \param _ndManBits Expression providing the number of mantissa bits.
		 * \param _ndImplicitMan Expression indicating whether the mantissa has an implicit leading bit.
		 * \param _ndSignValue Expression providing the sign value (0 or 1).
		 * \param _ndExpValue Expression providing the exponent value.
		 * \param _ndManValue Expression providing the mantissa value.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateAsFloatX( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
			const YYSTYPE::EE_NODE_DATA &_ndExpBits,
			const YYSTYPE::EE_NODE_DATA &_ndManBits,
			const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
			const YYSTYPE::EE_NODE_DATA &_ndSignValue,	// 0 or 1.
			const YYSTYPE::EE_NODE_DATA &_ndExpValue,
			const YYSTYPE::EE_NODE_DATA &_ndManValue,
			
			YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a FloatX node by converting from a double expression.
		 *
		 * Produces a node that converts the provided double expression into a floating-point type
		 * described by the given bit-field specifications.
		 *
		 * \param _ndSignBits Expression providing the number of sign bits.
		 * \param _ndExpBits Expression providing the number of exponent bits.
		 * \param _ndManBits Expression providing the number of mantissa bits.
		 * \param _ndImplicitMan Expression indicating whether the mantissa has an implicit leading bit.
		 * \param _ndExp Expression providing the double value to convert.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateAsFloatX( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
			const YYSTYPE::EE_NODE_DATA &_ndExpBits,
			const YYSTYPE::EE_NODE_DATA &_ndManBits,
			const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
			const YYSTYPE::EE_NODE_DATA &_ndExp,	// The double value to convert.
			
			YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a node that queries a FloatX property from a bit configuration.
		 *
		 * Builds a node that returns a property (such as min/max/epsilon/etc. depending on \c _nProp)
		 * of a floating-point format described by the provided bit-field specifications.
		 *
		 * \param _ndSignBits Expression providing the number of sign bits.
		 * \param _ndExpBits Expression providing the number of exponent bits.
		 * \param _ndManBits Expression providing the number of mantissa bits.
		 * \param _ndImplicitMan Expression indicating whether the mantissa has an implicit leading bit.
		 * \param _nProp The property to query.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateAsFloatXProp( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
			const YYSTYPE::EE_NODE_DATA &_ndExpBits,
			const YYSTYPE::EE_NODE_DATA &_ndManBits,
			const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
			const EE_NODES _nProp,
			
			YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a node that queries a FloatX property for a configured format and input value.
		 *
		 * Builds a node that returns a property (selected by \c _nProp) of the configured floating-point
		 * format, optionally using \c _ndExp as an input value for the property computation.
		 *
		 * \param _ndSignBits Expression providing the number of sign bits.
		 * \param _ndExpBits Expression providing the number of exponent bits.
		 * \param _ndManBits Expression providing the number of mantissa bits.
		 * \param _ndImplicitMan Expression indicating whether the mantissa has an implicit leading bit.
		 * \param _ndExp Expression providing the double value used by the queried property.
		 * \param _nProp The property to query.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateAsFloatXProp( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
			const YYSTYPE::EE_NODE_DATA &_ndExpBits,
			const YYSTYPE::EE_NODE_DATA &_ndManBits,
			const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
			const YYSTYPE::EE_NODE_DATA &_ndExp,	// The double value to convert.
			const EE_NODES _nProp,
			
			YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a cast expression node.
		 *
		 * Produces a node that converts the value of \c _ndExp according to the cast type \c _ctCast.
		 *
		 * \param _ndExp Expression node to cast.
		 * \param _ctCast The cast type to apply.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateCast( const YYSTYPE::EE_NODE_DATA &_ndExp, EE_CAST_TYPES _ctCast, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an address-of / address node.
		 *
		 * Produces a node that resolves an address from \c _ndExp, applying \c _ctCast to determine
		 * the address interpretation/width.
		 *
		 * \param _ndExp Expression node from which to produce an address.
		 * \param _ctCast The cast type describing the address interpretation.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateAddress( const YYSTYPE::EE_NODE_DATA &_ndExp, EE_CAST_TYPES _ctCast, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a member-access expression node.
		 *
		 * Builds a node that accesses member \c _sMember from the object expression \c _ndExp.
		 *
		 * \param _ndExp Object expression node data.
		 * \param _sMember Index/name token for the member to access.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateMemberAccess( const YYSTYPE::EE_NODE_DATA &_ndExp, size_t _sMember, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a user (??) node.
		 *
		 * \param _ndNode Receives the resulting node data.
		 */

		void								CreateUser( YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a numbered-parameter node (e.g. \c $<decimal>).
		 *
		 * Parses \c _pcText and produces a node that references the corresponding numbered parameter.
		 *
		 * \param _pcText Source text containing the numbered parameter.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateNumberedParm( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a dynamic numbered-parameter node from a string-table index.
		 *
		 * \param _sStrIndex Index into the internal string table used to resolve the parameter reference.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateDynamicParm( size_t _sStrIndex, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a dynamic numbered-parameter node from an expression.
		 *
		 * \param _ndExp Expression node providing the parameter index.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateDynamicParmExp( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a node that returns the total number of numbered parameters.
		 *
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateParmTotal( YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a unary operator node.
		 *
		 * \param _ndExp Operand expression node data.
		 * \param _uiOp Unary operator token/character.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateUnary( const YYSTYPE::EE_NODE_DATA &_ndExp, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a binary operator node.
		 *
		 * \param _ndLeft Left operand expression node data.
		 * \param _ndRight Right operand expression node data.
		 * \param _uiOp Operator token/character.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateOp( const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a conditional (\c ?: ) operator node.
		 *
		 * \param _ndExp Condition expression node data.
		 * \param _ndLeft True-expression node data.
		 * \param _ndRight False-expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateConditional( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an assignment expression node.
		 *
		 * Builds a node that assigns the evaluated right-hand side into a variable referenced by \c _sStrIndex,
		 * using the operator \c _uiOp, optionally declaring the variable as \c const.
		 *
		 * \param _sStrIndex Index into the internal string table for the variable name.
		 * \param _ndRight Right-hand side expression node data.
		 * \param _uiOp Assignment operator token/character (e.g. \c '=' , \c '+=' , etc.).
		 * \param _bIsConst Indicates whether the assignment declares a const variable.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateAssignment( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndRight, uint32_t _uiOp, bool _bIsConst, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a reassignment expression node.
		 *
		 * Builds a node that modifies an already-created variable referenced by \c _sStrIndex by applying
		 * the operator \c _uiOp with the evaluated right-hand side.
		 *
		 * \param _sStrIndex Index into the internal string table for the variable name.
		 * \param _ndRight Right-hand side expression node data.
		 * \param _uiOp Assignment operator token/character (e.g. \c '=' , \c '+=' , etc.).
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateReAssignment( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndRight, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a raw data array node.
		 *
		 * Builds a node representing a raw array with a backing type and persistence setting, with size provided
		 * by \c _ndSize. Optional initializer values are provided by the node index range.
		 *
		 * \param _sStrIndex Index into the internal string table for the array name (or backing identifier).
		 * \param _ui32Backing Backing type identifier.
		 * \param _ui32BackingPersistence Backing persistence identifier.
		 * \param _ndSize Expression node data that evaluates to the array size.
		 * \param _sStartValueNodeIdx Index of the first initializer-value node (inclusive).
		 * \param _sEndValueNodeIdx Index of the last initializer-value node (inclusive).
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateRawArray( size_t _sStrIndex, uint32_t _ui32Backing, uint32_t _ui32BackingPersistence, const YYSTYPE::EE_NODE_DATA &_ndSize,
			size_t _sStartValueNodeIdx, size_t _sEndValueNodeIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an assignment node that writes to an array element by name/index.
		 *
		 * \param _sArrayIndex Index into the internal string table for the array identifier.
		 * \param _ndArray Array/index expression node data describing the element.
		 * \param _ndValue Value expression node data to assign.
		 * \param _uiOp Assignment operator token/character.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayReAssignment( size_t _sArrayIndex, const YYSTYPE::EE_NODE_DATA &_ndArray, const YYSTYPE::EE_NODE_DATA &_ndValue, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an assignment node that writes to an array element on an object.
		 *
		 * \param _sArrayIndex Index into the internal string table for the array identifier.
		 * \param _ndIdx Index expression node data selecting the element.
		 * \param _ndValue Value expression node data to assign.
		 * \param _uiOp Assignment operator token/character.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayReAssignmentObj( size_t _sArrayIndex, const YYSTYPE::EE_NODE_DATA &_ndIdx, const YYSTYPE::EE_NODE_DATA &_ndValue, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an assignment node that writes to an array element via object/array expressions.
		 *
		 * \param _ndObj Object expression node data.
		 * \param _ndArray Array/index expression node data describing the element.
		 * \param _ndValue Value expression node data to assign.
		 * \param _uiOp Assignment operator token/character.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayReAssignment( const YYSTYPE::EE_NODE_DATA &_ndObj, const YYSTYPE::EE_NODE_DATA &_ndArray, const YYSTYPE::EE_NODE_DATA &_ndValue, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an assignment node that writes to an external address.
		 *
		 * \param _ctCast Address cast/type used to interpret the destination.
		 * \param _ndExp Expression node data that evaluates to the destination address.
		 * \param _ndValue Value expression node data to assign.
		 * \param _uiOp Assignment operator token/character.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateAddressAssignment( EE_CAST_TYPES _ctCast, const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndValue, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an array initializer node.
		 *
		 * \param _ndExp Expression node data providing the initializer expression.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayInitializer( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an array initializer-list node.
		 *
		 * Builds a node that references a contiguous range of initializer entries.
		 *
		 * \param _stLeftIdx Index of the first initializer entry (inclusive).
		 * \param _stRightIdx Index of the last initializer entry (inclusive).
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArrayInitializerList( size_t _stLeftIdx, size_t _stRightIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a vector from an initializer expression.
		 *
		 * \param _ndExp Expression node data providing the vector initializer.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVector( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an empty vector node.
		 *
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVector( YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a 0-parameter intrinsic call node.
		 *
		 * \param _uiIntrinsic Intrinsic token/identifier.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateIntrinsic0( uint32_t _uiIntrinsic, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a 1-parameter intrinsic call node.
		 *
		 * \param _uiIntrinsic Intrinsic token/identifier.
		 * \param _ndExp Parameter expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateIntrinsic1( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a 2-parameter intrinsic call node.
		 *
		 * \param _uiIntrinsic Intrinsic token/identifier.
		 * \param _ndExp0 First parameter expression node data.
		 * \param _ndExp1 Second parameter expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateIntrinsic2( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a 3-parameter intrinsic call node.
		 *
		 * \param _uiIntrinsic Intrinsic token/identifier.
		 * \param _ndExp0 First parameter expression node data.
		 * \param _ndExp1 Second parameter expression node data.
		 * \param _ndExp2 Third parameter expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateIntrinsic3( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, const YYSTYPE::EE_NODE_DATA &_ndExp2, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a compound statement node.
		 *
		 * Joins two statement nodes into a single compound statement node.
		 *
		 * \param _ndLeft Left statement node data.
		 * \param _ndRight Right statement node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateCompoundStatement( const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a while-loop node.
		 *
		 * \param _ndExp Condition expression node data.
		 * \param _ndStatements Loop body statements node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateWhileLoop( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a for-loop node with initializer/condition/iteration expressions.
		 *
		 * \param _ndExp0 Initializer expression node data.
		 * \param _ndExp1 Condition expression node data.
		 * \param _ndExp2 Iteration expression node data.
		 * \param _ndStatements Loop body statements node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateForLoop( const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, const YYSTYPE::EE_NODE_DATA &_ndExp2, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a for-loop node with initializer and condition expressions.
		 *
		 * \param _ndExp0 Initializer expression node data.
		 * \param _ndExp1 Condition expression node data.
		 * \param _ndStatements Loop body statements node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateForLoop( const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a do-while loop node.
		 *
		 * \param _ndExp Condition expression node data.
		 * \param _ndStatements Loop body statements node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateDoWhileLoop( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a foreach declaration node.
		 *
		 * \param _sStrIndex Index into the internal string table for the loop variable name.
		 * \param _sArrayIdx Index into the internal string table for the array identifier (or array reference).
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateForEachDecl( size_t _sStrIndex, size_t _sArrayIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a foreach declaration node for custom variables.
		 *
		 * \param _sStrIndex Index into the internal string table for the loop variable name.
		 * \param _sArrayIdx Index into the internal string table for the array/custom-variable identifier.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateForEachCustomDecl( size_t _sStrIndex, size_t _sArrayIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a foreach declaration node for strings.
		 *
		 * \param _sStrIndex Index into the internal string table for the loop variable name.
		 * \param _sArrayIdx Index into the internal string table for the string identifier.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateForEachStringDecl( size_t _sStrIndex, size_t _sArrayIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a foreach loop node.
		 *
		 * \param _ndDecl Foreach declaration node data.
		 * \param _ndStatements Loop body statements node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateForEachLoop( const YYSTYPE::EE_NODE_DATA &_ndDecl, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a foreach loop node that iterates over objects.
		 *
		 * \param _ndDecl Foreach declaration node data.
		 * \param _ndStatements Loop body statements node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateForEachObjLoop( const YYSTYPE::EE_NODE_DATA &_ndDecl, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a foreach loop node that iterates over strings.
		 *
		 * \param _ndDecl Foreach declaration node data.
		 * \param _ndStatements Loop body statements node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateForEachStrLoop( const YYSTYPE::EE_NODE_DATA &_ndDecl, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a selection statement node.
		 *
		 * \param _ndExp Condition expression node data.
		 * \param _ndStatements Selection body statements node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateSelectionStatement( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an if/else selection statement node.
		 *
		 * \param _ndExp Condition expression node data.
		 * \param _ndStatements0 True-branch statements node data.
		 * \param _ndStatements1 False-branch statements node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateSelectionStatement( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndStatements0, const YYSTYPE::EE_NODE_DATA &_ndStatements1, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a continue statement node.
		 *
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateContinue( YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a break statement node.
		 *
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateBreak( YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a single argument-list entry node.
		 *
		 * \param _ndExp Argument expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArgListEntry( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an argument-list entry by joining two entries.
		 *
		 * \param _ndLeft Left list/entry node data.
		 * \param _ndRight Right list/entry node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArgListEntry( const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates an argument-list node.
		 *
		 * \param _ndList Argument-list entry/list node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateArgList( const YYSTYPE::EE_NODE_DATA &_ndList, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Begins parsing/constructing an array literal.
		 *
		 * Initializes internal state used to accumulate array elements while the parser processes
		 * an array construct. The resulting array node is produced later when the array is closed.
		 *
		 * \return Returns true if the array state was started successfully; false otherwise.
		 */
		bool								StartArray( /*YYSTYPE::EE_NODE_DATA &_ndNode*/ );

		/**
		 * \brief Finishes parsing/constructing an array literal.
		 *
		 * Finalizes internal state associated with the most recently started array construct and
		 * seals any pending element accumulation.
		 */
		void								EndArray( /*YYSTYPE::EE_NODE_DATA &_ndNode*/ );

		/**
		 * \brief Creates a format-string node using \c .format() semantics.
		 *
		 * Builds a node representing a call of the form:
		 * \code
		 * "Some string {}.".format( Arg0, Arg1 )
		 * \endcode
		 *
		 * \param _sStrIndex Index into the internal string table for the format string literal.
		 * \param _ndArgs Argument-list node data supplying the format parameters.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateFormat( size_t _sStrIndex, const YYSTYPE::EE_NODE_DATA &_ndArgs, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.add() node.
		 *
		 * Builds a node representing element-wise addition of \c _ndVector and \c _ndOperand.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndOperand Operand expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorAdd( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndOperand, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.sub() node.
		 *
		 * Builds a node representing element-wise subtraction of \c _ndOperand from \c _ndVector.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndOperand Operand expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorSub( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndOperand, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.mul() node.
		 *
		 * Builds a node representing element-wise multiplication of \c _ndVector and \c _ndOperand.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndOperand Operand expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorMul( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndOperand, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.dot() node.
		 *
		 * Builds a node representing the dot product of \c _ndVector and \c _ndOperand.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndOperand Operand expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorDot( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndOperand, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.append() node.
		 *
		 * Builds a node representing an append operation on the vector expression.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndVal Value expression node data to append.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorAppend( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.append() node by variable id.
		 *
		 * Builds a node representing an append operation on a vector variable referenced by \c _sVarId.
		 *
		 * \param _sVarId Variable identifier for the destination vector.
		 * \param _ndVal Value expression node data to append.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorAppend( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.assign() node.
		 *
		 * Builds a node representing assignment of \c _ndVal to the element at index \c _ndIdx in \c _ndVector.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndIdx Index expression node data.
		 * \param _ndVal Value expression node data to assign.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorAssign( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndIdx, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.at() node.
		 *
		 * Builds a node representing retrieval of the element at index \c _ndIdx from \c _ndVector.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndIdx Index expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorAt( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.at() node by variable id.
		 *
		 * Builds a node representing retrieval of the element at index \c _ndIdx from the vector variable
		 * referenced by \c _sVarId.
		 *
		 * \param _sVarId Variable identifier for the source vector.
		 * \param _ndIdx Index expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorAt( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.capacity() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorCapacity( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.capacity() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorCapacity( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.clear() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorClear( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.clear() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorClear( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.cross() node.
		 *
		 * Builds a node representing the cross product of \c _ndVector and \c _ndOperand.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndOperand Operand vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorCross( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndOperand, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.empty() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorEmpty( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.empty() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorEmpty( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.erase() node.
		 *
		 * Builds a node representing erasure of the element at index \c _ndIdx from \c _ndVector.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndIdx Index expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorErase( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.erase() node by variable id.
		 *
		 * Builds a node representing erasure of the element at index \c _ndIdx from the vector variable
		 * referenced by \c _sVarId.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndIdx Index expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorErase( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndIdx, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.insert() node.
		 *
		 * Builds a node representing insertion of \c _ndVal at index \c _ndIdx in \c _ndVector.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndIdx Index expression node data.
		 * \param _ndVal Value expression node data to insert.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorInsert( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndIdx, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.max_size() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorMaxSize( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.max_size() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorMaxSize( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.mag() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorMag( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.mag() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorMag( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.mag_sq() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorMagSq( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.mag_sq() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorMagSq( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.normalize() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorNormalize( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.normalize() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorNormalize( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.reserve() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndSize Size expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorReserve( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndSize, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.reserve() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndSize Size expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorReserve( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndSize, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.resize() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndSize Size expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorResize( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndSize, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.resize() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndSize Size expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorResize( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndSize, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.pop_back() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorPopBack( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.pop_back() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorPopBack( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.push_back() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndVal Value expression node data to push.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorPushBack( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.push_back() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndVal Value expression node data to push.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorPushBack( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndVal, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.shrink_to_fit() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorShrinkToFit( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.shrink_to_fit() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorShrinkToFit( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.size() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorSize( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.size() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorSize( size_t _sVarId, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.sum() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorSum( const YYSTYPE::EE_NODE_DATA &_ndVector, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.swap() node.
		 *
		 * \param _ndVector Vector expression node data.
		 * \param _ndRight Right-hand operand expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorSwap( const YYSTYPE::EE_NODE_DATA &_ndVector, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a \c vector.swap() node by variable id.
		 *
		 * \param _sVarId Variable identifier for the vector.
		 * \param _ndRight Right-hand operand expression node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateVectorSwap( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a string tokenization node.
		 *
		 * Builds a node representing tokenization of \c _ndString using \c _ndTokenizer, with behavior
		 * controlled by \c _ndIncludeEmpty.
		 *
		 * \param _ndString String expression node data.
		 * \param _ndTokenizer Tokenizer expression node data.
		 * \param _ndIncludeEmpty Expression node data indicating whether empty tokens are included.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateStringTokenize( const YYSTYPE::EE_NODE_DATA &_ndString, const YYSTYPE::EE_NODE_DATA &_ndTokenizer, const YYSTYPE::EE_NODE_DATA &_ndIncludeEmpty, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a string tokenization node by variable id.
		 *
		 * Builds a node representing tokenization of the string variable referenced by \c _sVarId using
		 * \c _ndTokenizer, with behavior controlled by \c _ndIncludeEmpty.
		 *
		 * \param _sVarId Variable identifier for the source string.
		 * \param _ndTokenizer Tokenizer expression node data.
		 * \param _ndIncludeEmpty Expression node data indicating whether empty tokens are included.
		 * \param _ndNode Receives the resulting node data.
		 */
		void								CreateStringTokenize( size_t _sVarId, const YYSTYPE::EE_NODE_DATA &_ndTokenizer, const YYSTYPE::EE_NODE_DATA &_ndIncludeEmpty, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Creates a 1-parameter node of a given primitive type.
		 *
		 * Sets \c _ndNode to \c _nType and assigns its first operand index from \c _ndParm1, then adds the node.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndNode Receives the resulting node data.
		 * \param _nType The node type to create.
		 */
		void								CreateBasicNode( const YYSTYPE::EE_NODE_DATA &_ndParm1, YYSTYPE::EE_NODE_DATA &_ndNode, ee::EE_NODES _nType ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 2-parameter node of a given primitive type.
		 *
		 * Sets \c _ndNode to \c _nType and assigns its first two operand indices from the parameter nodes,
		 * then adds the node.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndParm2 Second parameter node data.
		 * \param _ndNode Receives the resulting node data.
		 * \param _nType The node type to create.
		 */
		void								CreateBasicNode( const YYSTYPE::EE_NODE_DATA &_ndParm1, const YYSTYPE::EE_NODE_DATA &_ndParm2, YYSTYPE::EE_NODE_DATA &_ndNode, ee::EE_NODES _nType ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.v.sNodeIndex = _ndParm2.sNodeIndex;
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 3-parameter node of a given primitive type.
		 *
		 * Sets \c _ndNode to \c _nType and assigns its first three operand indices from the parameter nodes,
		 * then adds the node.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndParm2 Second parameter node data.
		 * \param _ndParm3 Third parameter node data.
		 * \param _ndNode Receives the resulting node data.
		 * \param _nType The node type to create.
		 */
		void								CreateBasicNode( const YYSTYPE::EE_NODE_DATA &_ndParm1, const YYSTYPE::EE_NODE_DATA &_ndParm2, const YYSTYPE::EE_NODE_DATA &_ndParm3, YYSTYPE::EE_NODE_DATA &_ndNode, ee::EE_NODES _nType ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.v.sNodeIndex = _ndParm2.sNodeIndex;
			_ndNode.w.sNodeIndex = _ndParm3.sNodeIndex;
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 4-parameter node of a given primitive type.
		 *
		 * Sets \c _ndNode to \c _nType and assigns its first four operand indices from the parameter nodes,
		 * then adds the node.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndParm2 Second parameter node data.
		 * \param _ndParm3 Third parameter node data.
		 * \param _ndParm4 Fourth parameter node data.
		 * \param _ndNode Receives the resulting node data.
		 * \param _nType The node type to create.
		 */
		void								CreateBasicNode( const YYSTYPE::EE_NODE_DATA &_ndParm1, const YYSTYPE::EE_NODE_DATA &_ndParm2, const YYSTYPE::EE_NODE_DATA &_ndParm3, const YYSTYPE::EE_NODE_DATA &_ndParm4, YYSTYPE::EE_NODE_DATA &_ndNode, ee::EE_NODES _nType ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.v.sNodeIndex = _ndParm2.sNodeIndex;
			_ndNode.w.sNodeIndex = _ndParm3.sNodeIndex;
			_ndNode.x.sNodeIndex = _ndParm4.sNodeIndex;
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 5-parameter node of a given primitive type.
		 *
		 * Sets \c _ndNode to \c _nType and assigns its first five operand indices from the parameter nodes,
		 * then adds the node.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndParm2 Second parameter node data.
		 * \param _ndParm3 Third parameter node data.
		 * \param _ndParm4 Fourth parameter node data.
		 * \param _ndParm5 Fifth parameter node data.
		 * \param _ndNode Receives the resulting node data.
		 * \param _nType The node type to create.
		 */
		void								CreateBasicNode( const YYSTYPE::EE_NODE_DATA &_ndParm1, const YYSTYPE::EE_NODE_DATA &_ndParm2, const YYSTYPE::EE_NODE_DATA &_ndParm3, const YYSTYPE::EE_NODE_DATA &_ndParm4, const YYSTYPE::EE_NODE_DATA &_ndParm5, YYSTYPE::EE_NODE_DATA &_ndNode, ee::EE_NODES _nType ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.v.sNodeIndex = _ndParm2.sNodeIndex;
			_ndNode.w.sNodeIndex = _ndParm3.sNodeIndex;
			_ndNode.x.sNodeIndex = _ndParm4.sNodeIndex;
			_ndNode.y.sNodeIndex = _ndParm5.sNodeIndex;
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 1-parameter node of a given object type.
		 *
		 * Creates an object of type \c _tType, stores it in the object list, and emits a node of type \c _nType
		 * with one operand.
		 *
		 * \tparam _nType Node type to emit.
		 * \tparam _tType Object type to allocate.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		template <ee::EE_NODES _nType, typename _tType>
		void								CreateBasicObjectNode( const YYSTYPE::EE_NODE_DATA &_ndParm1, YYSTYPE::EE_NODE_DATA &_ndNode ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.w.sNodeIndex = m_vObjects.size();
			AllocateObject<_tType>();
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 2-parameter node of a given object type.
		 *
		 * Creates an object of type \c _tType, stores it in the object list, and emits a node of type \c _nType
		 * with two operands.
		 *
		 * \tparam _nType Node type to emit.
		 * \tparam _tType Object type to allocate.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndParm2 Second parameter node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		template <ee::EE_NODES _nType, typename _tType>
		void								CreateBasicObjectNode( const YYSTYPE::EE_NODE_DATA &_ndParm1, const YYSTYPE::EE_NODE_DATA &_ndParm2, YYSTYPE::EE_NODE_DATA &_ndNode ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.v.sNodeIndex = _ndParm2.sNodeIndex;
			_ndNode.w.sNodeIndex = m_vObjects.size();
			AllocateObject<_tType>();
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 3-parameter node of a given object type.
		 *
		 * Creates an object of type \c _tType, stores it in the object list, and emits a node of type \c _nType
		 * with three operands.
		 *
		 * \tparam _nType Node type to emit.
		 * \tparam _tType Object type to allocate.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndParm2 Second parameter node data.
		 * \param _ndParm3 Third parameter node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		template <ee::EE_NODES _nType, typename _tType>
		void								CreateBasicObjectNode( const YYSTYPE::EE_NODE_DATA &_ndParm1, const YYSTYPE::EE_NODE_DATA &_ndParm2, const YYSTYPE::EE_NODE_DATA &_ndParm3, YYSTYPE::EE_NODE_DATA &_ndNode ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.v.sNodeIndex = _ndParm2.sNodeIndex;
			_ndNode.x.sNodeIndex = _ndParm3.sNodeIndex;
			_ndNode.w.sNodeIndex = m_vObjects.size();
			AllocateObject<_tType>();
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 4-parameter node of a given object type.
		 *
		 * Creates an object of type \c _tType, stores it in the object list, and emits a node of type \c _nType
		 * with four operands.
		 *
		 * \tparam _nType Node type to emit.
		 * \tparam _tType Object type to allocate.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndParm2 Second parameter node data.
		 * \param _ndParm3 Third parameter node data.
		 * \param _ndParm4 Fourth parameter node data.
		 * \param _ndNode Receives the resulting node data.
		 */
		template <ee::EE_NODES _nType, typename _tType>
		void								CreateBasicObjectNode( const YYSTYPE::EE_NODE_DATA &_ndParm1, const YYSTYPE::EE_NODE_DATA &_ndParm2, const YYSTYPE::EE_NODE_DATA &_ndParm3, const YYSTYPE::EE_NODE_DATA &_ndParm4, YYSTYPE::EE_NODE_DATA &_ndNode ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.v.sNodeIndex = _ndParm2.sNodeIndex;
			_ndNode.x.sNodeIndex = _ndParm3.sNodeIndex;
			_ndNode.y.sNodeIndex = _ndParm4.sNodeIndex;
			_ndNode.w.sNodeIndex = m_vObjects.size();
			AllocateObject<_tType>();
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 1-parameter node of a given object type with one extra 32-bit parameter.
		 *
		 * Creates an object of type \c _tType, stores it in the object list, writes \c _ui32Ex into the node's
		 * extra field, and emits a node of type \c _nType with one operand.
		 *
		 * \tparam _nType Node type to emit.
		 * \tparam _tType Object type to allocate.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ui32Ex Extra 32-bit parameter to store in the node.
		 * \param _ndNode Receives the resulting node data.
		 */
		template <ee::EE_NODES _nType, typename _tType>
		void								CreateBasicObjectNode_Ex1( const YYSTYPE::EE_NODE_DATA &_ndParm1, uint32_t _ui32Ex, YYSTYPE::EE_NODE_DATA &_ndNode ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.z.sNodeIndex = size_t( _ui32Ex );
			_ndNode.w.sNodeIndex = m_vObjects.size();
			AllocateObject<_tType>();
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 2-parameter node of a given object type with one extra 32-bit parameter.
		 *
		 * Creates an object of type \c _tType, stores it in the object list, writes \c _ui32Ex into the node's
		 * extra field, and emits a node of type \c _nType with two operands.
		 *
		 * \tparam _nType Node type to emit.
		 * \tparam _tType Object type to allocate.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndParm2 Second parameter node data.
		 * \param _ui32Ex Extra 32-bit parameter to store in the node.
		 * \param _ndNode Receives the resulting node data.
		 */
		template <ee::EE_NODES _nType, typename _tType>
		void								CreateBasicObjectNode_Ex1( const YYSTYPE::EE_NODE_DATA &_ndParm1, const YYSTYPE::EE_NODE_DATA &_ndParm2, uint32_t _ui32Ex, YYSTYPE::EE_NODE_DATA &_ndNode ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.v.sNodeIndex = _ndParm2.sNodeIndex;
			_ndNode.z.sNodeIndex = size_t( _ui32Ex );
			_ndNode.w.sNodeIndex = m_vObjects.size();
			AllocateObject<_tType>();
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 3-parameter node of a given object type with one extra 32-bit parameter.
		 *
		 * Creates an object of type \c _tType, stores it in the object list, writes \c _ui32Ex into the node's
		 * extra field, and emits a node of type \c _nType with three operands.
		 *
		 * \tparam _nType Node type to emit.
		 * \tparam _tType Object type to allocate.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndParm2 Second parameter node data.
		 * \param _ndParm3 Third parameter node data.
		 * \param _ui32Ex Extra 32-bit parameter to store in the node.
		 * \param _ndNode Receives the resulting node data.
		 */
		template <ee::EE_NODES _nType, typename _tType>
		void								CreateBasicObjectNode_Ex1( const YYSTYPE::EE_NODE_DATA &_ndParm1, const YYSTYPE::EE_NODE_DATA &_ndParm2, const YYSTYPE::EE_NODE_DATA &_ndParm3, uint32_t _ui32Ex, YYSTYPE::EE_NODE_DATA &_ndNode ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.v.sNodeIndex = _ndParm2.sNodeIndex;
			_ndNode.x.sNodeIndex = _ndParm3.sNodeIndex;
			_ndNode.z.sNodeIndex = size_t( _ui32Ex );
			_ndNode.w.sNodeIndex = m_vObjects.size();
			AllocateObject<_tType>();
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 4-parameter node of a given object type with one extra 32-bit parameter.
		 *
		 * Creates an object of type \c _tType, stores it in the object list, writes \c _ui32Ex into the node's
		 * extra field, and emits a node of type \c _nType with four operands.
		 *
		 * \tparam _nType Node type to emit.
		 * \tparam _tType Object type to allocate.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndParm2 Second parameter node data.
		 * \param _ndParm3 Third parameter node data.
		 * \param _ndParm4 Fourth parameter node data.
		 * \param _ui32Ex Extra 32-bit parameter to store in the node.
		 * \param _ndNode Receives the resulting node data.
		 */
		template <ee::EE_NODES _nType, typename _tType>
		void								CreateBasicObjectNode_Ex1( const YYSTYPE::EE_NODE_DATA &_ndParm1, const YYSTYPE::EE_NODE_DATA &_ndParm2, const YYSTYPE::EE_NODE_DATA &_ndParm3, const YYSTYPE::EE_NODE_DATA &_ndParm4, uint32_t _ui32Ex, YYSTYPE::EE_NODE_DATA &_ndNode ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.v.sNodeIndex = _ndParm2.sNodeIndex;
			_ndNode.x.sNodeIndex = _ndParm3.sNodeIndex;
			_ndNode.y.sNodeIndex = _ndParm4.sNodeIndex;
			_ndNode.z.sNodeIndex = size_t( _ui32Ex );
			_ndNode.w.sNodeIndex = m_vObjects.size();
			AllocateObject<_tType>();
			AddNode( _ndNode );
		}

		/**
		 * \brief Creates a 5-parameter node of a given object type with one extra 32-bit parameter.
		 *
		 * Creates an object of type \c _tType, stores it in the object list, writes \c _ui32Ex into the node's
		 * extra field, and emits a node of type \c _nType with five operands.
		 *
		 * \tparam _nType Node type to emit.
		 * \tparam _tType Object type to allocate.
		 *
		 * \param _ndParm1 First parameter node data.
		 * \param _ndParm2 Second parameter node data.
		 * \param _ndParm3 Third parameter node data.
		 * \param _ndParm4 Fourth parameter node data.
		 * \param _ndParm5 Fifth parameter node data.
		 * \param _ui32Ex Extra 32-bit parameter to store in the node.
		 * \param _ndNode Receives the resulting node data.
		 */
		template <ee::EE_NODES _nType, typename _tType>
		void								CreateBasicObjectNode_Ex1( const YYSTYPE::EE_NODE_DATA &_ndParm1, const YYSTYPE::EE_NODE_DATA &_ndParm2, const YYSTYPE::EE_NODE_DATA &_ndParm3, const YYSTYPE::EE_NODE_DATA &_ndParm4, const YYSTYPE::EE_NODE_DATA &_ndParm5, uint32_t _ui32Ex, YYSTYPE::EE_NODE_DATA &_ndNode ) {
			_ndNode.nType = _nType;
			_ndNode.u.sNodeIndex = _ndParm1.sNodeIndex;
			_ndNode.v.sNodeIndex = _ndParm2.sNodeIndex;
			_ndNode.x.sNodeIndex = _ndParm3.sNodeIndex;
			_ndNode.y.sNodeIndex = _ndParm4.sNodeIndex;
			_ndNode.a.sNodeIndex = _ndParm5.sNodeIndex;
			_ndNode.z.sNodeIndex = size_t( _ui32Ex );
			_ndNode.w.sNodeIndex = m_vObjects.size();
			AllocateObject<_tType>();
			AddNode( _ndNode );
		}

		/**
		 * \brief Sets the translation-unit node.
		 *
		 * Records \c _ndNode as the root/translation-unit node for the current parse.
		 *
		 * \param _ndNode Translation-unit node data.
		 */
		void								SetTrans( YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Determines whether an expression node is constant.
		 *
		 * If \c _ndExp is constant, returns true and writes the associated constant value to \c _rResult.
		 *
		 * \param _ndExp Expression node data to test.
		 * \param _rResult Receives the constant value when the expression is constant.
		 * \return Returns true if the expression is constant; false otherwise.
		 */
		bool								IsConst( const YYSTYPE::EE_NODE_DATA &_ndExp, EE_RESULT &_rResult );

		/**
		 * \brief Determines whether the parser is currently breaking out of a loop.
		 *
		 * \return Returns true if the current loop scope has a pending break; false otherwise.
		 */
		bool								Breaking() const;

		/**
		 * \brief Requests a break from the current loop.
		 *
		 * Sets the break flag on the current loop scope.
		 *
		 * \return Returns true if a loop scope exists and the break was recorded; false if not in a loop.
		 */
		bool								Break();

		/**
		 * \brief Determines whether the parser is currently continuing a loop.
		 *
		 * \return Returns true if the current loop scope has a pending continue; false otherwise.
		 */
		bool								Continuing() const;

		/**
		 * \brief Requests a continue for the current loop.
		 *
		 * Sets the continue flag on the current loop scope.
		 *
		 * \return Returns true if a loop scope exists and the continue was recorded; false if not in a loop.
		 */
		bool								Continue();

		/**
		 * \brief Determines whether the current loop scope is breaking or continuing.
		 *
		 * \return Returns true if a loop scope exists and either break or continue is pending; false otherwise.
		 */
		bool								BreakingOrContinuing() const {
			if ( !m_vLoopStack.size() ) { return false; }
			auto aIdx = m_vLoopStack.size() - 1;
			return m_vLoopStack[aIdx].bBreak || m_vLoopStack[aIdx].bContinue;
		}


	protected :
		// == Types.
		/**
		 * \brief A user-defined variable entry.
		 *
		 * Stores the current value for a custom variable along with metadata used to determine whether the value
		 * is constant and whether it is up-to-date.
		 */
		struct EE_CUSTOM_VAR {
			uint64_t						ui64UpdateCounter;
			EE_RESULT						rRes;
			bool							bIsConst;
		};

		/**
		 * \brief Tracks state for a loop scope.
		 *
		 * Used to support loop execution (for/while/do-while/foreach) and to record break/continue requests.
		 */
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

		/**
		 * \brief RAII helper that pushes and pops a loop scope.
		 *
		 * Ensures the loop-scope stack is restored even when exiting early (returns) or on exceptions.
		 */
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

		/**
		 * \brief Array metadata entry.
		 *
		 * Holds the backing array interface along with any additional metadata (as needed).
		 */
		struct EE_ARRAY_DATA {
			CArrayBase *					m_pabBase;
		};

		/**
		 * \brief Explicit-stack frame used during evaluation.
		 *
		 * Represents one in-flight node during non-recursive (explicit stack) evaluation, including intermediate
		 * sub-results, loop state, and pointers to where outputs should be written.
		 */
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


		// == Members.
		CExpEvalLexer *						m_peelLexer;											/**< The lexer. */
		std::vector<YYSTYPE::EE_NODE_DATA>	m_vNodes;												/**< The stack of nodes. */
		std::vector<std::string>			m_vStrings;												/**< The stack of UTF-8 strings. */
		std::vector<CObject *>				m_vObjects;												/**< The array of objects. */
		size_t								m_vsCompileTimeObjects = 0;								/**< The number of objects allocated during compilation. */
		std::map<size_t, EE_CUSTOM_VAR>		m_mCustomVariables;										/**< The map of custom variables. std::map<name_as_index, EE_CUSTOM_VAR>. */
		std::map<size_t, size_t>			m_mArrays;												/**< The map of arrays.  std::map<name_as_index, index_into_array_data>. */
		std::vector<EE_ARRAY_DATA>			m_vArrayData;											/**< The actual array data. */
		std::vector<EE_LOOP_STACK>			m_vLoopStack;											/**< The stack of loop scopes. */
		std::vector<EE_STACK_OBJ>			m_vStack;												/**< The explicit resolve stack. */
		std::vector<EE_RESULT>				m_vNumberedParms;										/**< Numbered parameters. */
		std::vector<std::vector<EE_RESULT>>	m_vFuncParms;											/**< Function parameters. */
		std::set<size_t>					m_sNumberedParmsAccessed;								/**< Referenced numbered parameters. */
		std::vector<size_t>					m_vVectorStack;											/**< Stack of arrays being constructed. */
		bool								m_bTreatAllAsHex;										/**< Treate everything as hex? */
		PfAddressHandler					m_pfahAddressHandler;									/**< Address handler. */
		uintptr_t							m_uiptrAddressData;										/**< Address data. */
		PfAddressHandler					m_pfahAddressWriteHandler;								/**< Address write handler. */
		uintptr_t							m_uiptrAddressWriteData;								/**< Address write data. */
		PfUserVarHandler					m_pfUser;												/**< User function. */
		uintptr_t							m_uiptrUserData;										/**< Data to pass to the user function. */
		PfStringHandler						m_pfshString;											/**< String handler. */
		uintptr_t							m_uiptrStringData;										/**< Data to pass to the string handler. */
		PfMemberAccessHandler				m_pfmahMemberAccess;									/**< Member-access handler. */
		uintptr_t							m_uiptrMemberAccess;									/**< Data to pass to the member-access handler. */
		PfToStringHandler					m_pfToString;											/**< ToString() function. */
		size_t								m_sTrans;												/**< Translation unit. */
		std::random_device					m_rdRandDevice;											/**< The random device. */
		std::mt19937_64						m_mRandGen;												/**< The generator. */
		bool								m_bHasUserVar;											/**< Was ?? used? */
		bool								m_bHasAddressRead;										/**< Was the dereferece operator used? */


		// == Functions.
		/**
		 * \brief Adds a node to the internal node tree.
		 *
		 * Stores \c _ndNode into the evaluator's node list/tree, assigning any required indices and ownership
		 * information for later evaluation.
		 *
		 * \param _ndNode Node data to add.
		 */
		void								AddNode( YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * \brief Adds a string to the internal string table.
		 *
		 * \param _sText String to add.
		 * \return Returns the index of the added string within the internal string table.
		 */
		size_t								AddString( const std::string &_sText );

		/**
		 * \brief Resolves (evaluates) a node using an explicit evaluation stack.
		 *
		 * Performs non-recursive evaluation of the expression rooted at \c _sNode, returning detailed error
		 * information through \c _ecError.
		 *
		 * \param _sNode Node index to resolve.
		 * \param _rFinalResult Receives the resolved value.
		 * \param _ecError Receives a specific error code when evaluation fails.
		 * \return Returns true if evaluation succeeded; false otherwise.
		 */
		bool								ResolveNode_ExplicitStack( size_t _sNode, EE_RESULT &_rFinalResult, EE_ERROR_CODES &_ecError );

		/**
		 * \brief Creates an array instance based on a type identifier.
		 *
		 * \param _ui32Type Type identifier describing the array element/backing type.
		 * \return Returns a pointer to the created array instance, or nullptr on failure.
		 */
		CArrayBase *						CreateArrayFromType( uint32_t _ui32Type );

		/**
		 * \brief Creates a string reference object from a string-table index.
		 *
		 * \param _stStrIdx Index into the internal string table.
		 * \return Returns true if the string reference object was created successfully; false otherwise.
		 */
		bool								CreateStringRef( size_t _stStrIdx );

		/**
		 * \brief Pushes a node onto the explicit evaluation stack.
		 *
		 * Initializes a new stack frame for node \c _sNodeIdx, linking it to its parent frame and indicating
		 * where the node's result should be written.
		 *
		 * \param _vStack Explicit evaluation stack.
		 * \param _sNodeIdx Index of the node to push.
		 * \param _sParentIdx Index of the parent stack frame, or \c EE_INVALID_IDX if none.
		 * \param _prResult Pointer to where the node result should be written.
		 * \return Returns true if the frame was pushed; false otherwise.
		 */
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

		/**
		 * \brief Left-shift operation for floating-point values.
		 *
		 * Interprets \c _dShift as a shift amount and applies a left shift-like operation to \c _dVal using
		 * floating-point semantics as defined by the evaluator.
		 *
		 * \param _dVal Input value.
		 * \param _dShift Shift amount.
		 * \return Returns the shifted value.
		 */
		static double						LShift( double _dVal, double _dShift );

		/**
		 * \brief Right-shift operation for floating-point values.
		 *
		 * Interprets \c _dShift as a shift amount and applies a right shift-like operation to \c _dVal using
		 * floating-point semantics as defined by the evaluator.
		 *
		 * \param _dVal Input value.
		 * \param _dShift Shift amount.
		 * \return Returns the shifted value.
		 */
		static double						RShift( double _dVal, double _dShift );

		/**
		 * \brief Converts an identifier node into a custom-variable node.
		 *
		 * \param _ndNode Node to convert.
		 * \return Returns true if the node was converted; false if the node is not an identifier node.
		 */
		static bool							ConvertIdentifierToCustomVar( YYSTYPE::EE_NODE_DATA &_ndNode ) {
			if ( _ndNode.nType != EE_N_STRING ) { return false; }
			_ndNode.nType = EE_N_CUSTOM_VAR;
			return true;
		}

		/**
		 * \brief Parses a PrintF() formatter and fills the context structure.
		 *
		 * Consumes the next formatter specifier from \c _pwcFormat, updating \c _sPos and writing the parsed
		 * information into \c _pcContext along with the inferred numeric type in \c _ncType.
		 *
		 * \param _pwcFormat Format string.
		 * \param _sPos Current parsing position within \c _pwcFormat; updated on success.
		 * \param _pcContext Receives parsed formatter context.
		 * \param _ncType Receives the expected argument type category.
		 * \return Returns true if a formatter was parsed successfully; false otherwise.
		 */
		static bool							FillContext( const wchar_t * _pwcFormat, size_t &_sPos, EE_PRINTF_CONTEXT &_pcContext, EE_NUM_CONSTANTS &_ncType );

		/**
		 * \brief Determines if a character is a valid PrintF() flag.
		 *
		 * \param _wcFlag Character to test.
		 * \return Returns true if \c _wcFlag is a supported PrintF() flag; false otherwise.
		 */
		static bool							IsPrintFFlag( wchar_t _wcFlag ) {
			return _wcFlag == L'+' ||
				_wcFlag == L'-' || _wcFlag == L'#' ||
				_wcFlag == L'0' || _wcFlag == L' ';
		}

		/**
		 * \brief Determines if a character is a valid PrintF() width specifier digit.
		 *
		 * \param _wcFlag Character to test.
		 * \return Returns true if \c _wcFlag is a valid width digit; false otherwise.
		 */
		static bool							IsPrintFWidth( wchar_t _wcFlag ) {
			return _wcFlag >= L'0' && _wcFlag <= L'9';
		}

		/**
		 * \brief Determines if a character is a valid PrintF() precision specifier digit.
		 *
		 * \param _wcFlag Character to test.
		 * \return Returns true if \c _wcFlag is a valid precision digit; false otherwise.
		 */
		static bool							IsPrintFPrecision( wchar_t _wcFlag ) {
			return _wcFlag >= L'0' && _wcFlag <= L'9';
		}

		/**
		 * \brief Determines if a character is a valid PrintF() argument-size specifier.
		 *
		 * \param _wcFlag Character to test.
		 * \return Returns true if \c _wcFlag is a supported size specifier; false otherwise.
		 */
		static bool							IsPrintFArgSize( wchar_t _wcFlag ) {
			return _wcFlag == L'h' ||
				_wcFlag == L'j' || _wcFlag == L'l' ||
				_wcFlag == L'L' || _wcFlag == L't' ||
				_wcFlag == L'w' || _wcFlag == L'z' ||
				_wcFlag == L'I' || _wcFlag == L'3' ||
				_wcFlag == L'2' || _wcFlag == L'6' ||
				_wcFlag == L'4';
		}

		/**
		 * \brief Determines if a character is a valid PrintF() integer conversion specifier.
		 *
		 * \param _wcFlag Character to test.
		 * \return Returns true if \c _wcFlag is a supported integer conversion specifier; false otherwise.
		 */
		static bool							IsPrintFIntType( wchar_t _wcFlag ) {
			return _wcFlag == L'd' || _wcFlag == L'i' ||
				_wcFlag == L'o' || _wcFlag == L'u' ||
				_wcFlag == L'x' || _wcFlag == L'X' ||
				_wcFlag == L'c' || _wcFlag == L'C';
		}

		/**
		 * \brief Determines if a character is a valid PrintF() floating-point conversion specifier.
		 *
		 * \param _wcFlag Character to test.
		 * \return Returns true if \c _wcFlag is a supported floating-point conversion specifier; false otherwise.
		 */
		static bool							IsPrintFFloatType( wchar_t _wcFlag ) {
			return _wcFlag == L'a' || _wcFlag == L'A' ||
				_wcFlag == L'e' || _wcFlag == L'E' ||
				_wcFlag == L'f' || _wcFlag == L'F' ||
				_wcFlag == L'g' || _wcFlag == L'G';
		}

		/**
		 * \brief Consumes a \c {..} formatter from a UTF-8 format string.
		 *
		 * Parses and removes the formatter portion beginning immediately after an opening \c '{' and ending at the
		 * matching \c '}'. The formatter is interpreted as ASCII-only.
		 *
		 * \param _pcFormat Pointer to the current format-string position; must point to the character immediately
		 * after the opening \c '{'. Updated to point past the closing \c '}'.
		 * \param _stLen Remaining length of the format string; updated as characters are consumed.
		 * \param _stArgIdx Receives/updates the argument index selected by the formatter.
		 * \param _vArgs Argument list used by the formatter (for indexed/typed selection as supported).
		 * \return Returns the decoded/expanded formatter text to splice into the final formatted output.
		 */
		static std::string					EatStringFormatter( const char * &_pcFormat, size_t &_stLen, size_t &_stArgIdx, const std::vector<EE_RESULT> &_vArgs );

	};

}	// namespace ee

#pragma once

#include "../EEExpEval.h"
#include "../Api/EEBaseApi.h"
#include "../EEExpEvalContainer.h"

#include <vector>

#if defined( _MSC_VER )
#define EE_PURE_DECL								{ return { EE_NC_INVALID }; }

#pragma warning( push )

// warning C4100: '_i64Idx': unreferenced formal parameter
#pragma warning( disable : 4100 )
#else
#define EE_PURE_DECL								;
#endif	// #if defined( _MSC_VER )

namespace ee {

	class CObject : public CBaseApi {
	public :
		CObject( CExpEvalContainer * _peecContainer ) :
			m_peecContainer( _peecContainer ) {
		}
		virtual										~CObject() {}


		// == Enumerations.
		/**
		 * \brief Bit flags describing object types that are implemented directly by the expression-evaluation runtime.
		 *
		 * These flags identify concrete built-in object implementations (for example, a dynamic UTF-8 string type
		 * or a vector container) that can be created and manipulated by the evaluator.
		 *
		 * \note This is a bitmask enumeration.  Multiple flags may be combined.
		 */
		enum EE_BUILT_IN_TYPES : uint32_t {
			EE_BIT_STRING							= (1 << 1),						/**< Built-in string object type (CString). */
			EE_BIT_VECTOR							= (1 << 2),						/**< Built-in vector object type (CVector). */
		};

		/**
		 * \brief Bit flags controlling extended array-access behavior.
		 *
		 * Extended array access allows implementations to interpret one or two indices depending on which
		 * flags are set in the mask passed to ArrayAccessEx().
		 *
		 * \note This is a bitmask enumeration.  Multiple flags may be combined.
		 */
		enum EE_ARRAY_EX_FLAGS : uint32_t {
			EE_AEF_START							= (1 << 0),						/**< Indicates that _i64Idx0 is valid and should be used. */
			EE_AEF_END								= (1 << 1),						/**< Indicates that _i64Idx1 is valid and should be used. */
		};

		/**
		 * \brief Flags controlling string conversion and formatting behaviors.
		 *
		 * These flags influence how ToString() and FormattedString() escape or present data.
		 *
		 * \note This is a bitmask enumeration.  Multiple flags may be combined.
		 */
		enum EE_TOSTRING_FLAGS : uint32_t {
			/** No special formatting is applied. */
			EE_TF_NONE								= 0,

			/**
			 * Treats the output as a C-style string literal, escaping quotes and other characters as needed.
			 * Example: Hey "Bob" -> "Hey \"Bob\""
			 */
			EE_TF_C_STRING							= (1 << 0),						// Hey "Bob" -> "Hey \"Bob\""
		};


		// == Functions.
		/**
		 * \brief Gets the runtime type identifier of this object.
		 *
		 * Derived classes should return a stable type value used by the evaluator to identify the object
		 * implementation.
		 *
		 * \return Returns the object type identifier.
		 */
		virtual uint32_t							Type() const { return 0; };

#if defined( _MSC_VER )
		/**
		 * \brief Creates a string representation of the object.
		 *
		 * The produced string is typically UTF-8 unless the derived type documents otherwise.  Implementations
		 * may use _ui32Depth to limit recursion when rendering nested objects.
		 *
		 * \param _sString Receives the textual representation.
		 * \param _ui32Depth Current formatting depth (used to prevent runaway recursion).
		 * \param _ui32Flags Formatting flags (EE_TOSTRING_FLAGS).
		 * \return Returns true if the conversion succeeded; false otherwise.
		 */
		virtual bool								ToString( std::string &_sString, uint32_t /*_ui32Depth*/, uint32_t _ui32Flags = EE_TF_NONE ) = 0 {
			static_cast<void>(_ui32Flags);
			_sString = "<null>";
			return false;
		}
		
		/**
		 * \brief Creates a formatted string representation of the object.
		 *
		 * The meaning of _sFormat is defined by the derived type.  Implementations may honor _ui32Flags to
		 * escape or quote the output.
		 *
		 * \param _sFormat A format string interpreted by the derived type.
		 * \param _ui32Flags Formatting flags (EE_TOSTRING_FLAGS).
		 * \return Returns the formatted representation.
		 */
		virtual std::string							FormattedString( const std::string &/*_sFormat*/, uint32_t _ui32Flags = EE_TF_NONE ) = 0 {
			static_cast<void>(_ui32Flags);
			return std::string( "<null>" );
		}
		
		/**
		 * \brief Converts this object into another evaluator value type.
		 *
		 * Derived classes should return an EE_RESULT whose EE_NUM_CONSTANTS tag matches the requested type,
		 * or EE_NC_INVALID on failure.
		 *
		 * \param _ncType The destination numeric/value type.
		 * \return Returns the converted value, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		ConvertTo( EE_NUM_CONSTANTS /*_ncType*/ ) = 0 {
			CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };
			return rRet;
		}

		/**
		 * \brief Initializes this object from an evaluator result.
		 *
		 * Derived classes should interpret _rObj according to their conversion rules.
		 *
		 * \param _rObj Source evaluator result.
		 * \return Returns true if initialization succeeded; false otherwise.
		 */
		virtual bool								InitializeFrom( const CExpEvalContainer::EE_RESULT &/*_rObj*/ ) = 0 {
			return false;
		}
#else
		/**
		 * \brief Creates a string representation of the object.
		 *
		 * The produced string is typically UTF-8 unless the derived type documents otherwise.  Implementations
		 * may use _ui32Depth to limit recursion when rendering nested objects.
		 *
		 * \param _sString Receives the textual representation.
		 * \param _ui32Depth Current formatting depth (used to prevent runaway recursion).
		 * \param _ui32Flags Formatting flags (EE_TOSTRING_FLAGS).
		 * \return Returns true if the conversion succeeded; false otherwise.
		 */
		virtual bool								ToString( std::string &_sString, uint32_t _ui32Depth, uint32_t _ui32Flags = EE_TF_NONE ) = 0;
		
		/**
		 * \brief Creates a formatted string representation of the object.
		 *
		 * The meaning of _sFormat is defined by the derived type.  Implementations may honor _ui32Flags to
		 * escape or quote the output.
		 *
		 * \param _sFormat A format string interpreted by the derived type.
		 * \param _ui32Flags Formatting flags (EE_TOSTRING_FLAGS).
		 * \return Returns the formatted representation.
		 */
		virtual std::string							FormattedString( const std::string &_sFormat, uint32_t _ui32Flags = EE_TF_NONE ) = 0;
		
		/**
		 * \brief Converts this object into another evaluator value type.
		 *
		 * Derived classes should return an EE_RESULT whose EE_NUM_CONSTANTS tag matches the requested type,
		 * or EE_NC_INVALID on failure.
		 *
		 * \param _ncType The destination numeric/value type.
		 * \return Returns the converted value, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		ConvertTo( EE_NUM_CONSTANTS _ncType ) = 0;

		/**
		 * \brief Initializes this object from an evaluator result.
		 *
		 * Derived classes should interpret _rObj according to their conversion rules.
		 *
		 * \param _rObj Source evaluator result.
		 * \return Returns true if initialization succeeded; false otherwise.
		 */
		virtual bool								InitializeFrom( const CExpEvalContainer::EE_RESULT &_rObj ) = 0;
#endif	// #if defined( _MSC_VER )

		/**
		 * \brief Creates an evaluator result that references this object.
		 *
		 * \return Returns an EE_RESULT tagged as EE_NC_OBJECT, referencing this instance.
		 */
		CExpEvalContainer::EE_RESULT				CreateResult() {
			CExpEvalContainer::EE_RESULT rMe = { EE_NC_OBJECT };
			rMe.u.poObj = this;
			return rMe;
		}

		/**
		 * \brief Creates an evaluator result that references this object.
		 *
		 * \return Returns an EE_RESULT tagged as EE_NC_OBJECT, referencing this instance.
		 */
		CExpEvalContainer::EE_RESULT				CreateResult() const {
			CExpEvalContainer::EE_RESULT rMe = { EE_NC_OBJECT };
			rMe.u.poObj = const_cast<CObject *>(this);
			return rMe;
		}

		/**
		 * \brief Performs array-style indexing on this object.
		 *
		 * Derived classes should return the element at the given index or EE_NC_INVALID if indexing is not
		 * supported or the index is out of range.
		 *
		 * \param _i64Idx The index to access.
		 * \return Returns the accessed element as an evaluator result, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		ArrayAccess( int64_t _i64Idx ) = 0 EE_PURE_DECL

		/**
		 * \brief Performs extended array-style indexing on this object.
		 *
		 * This form allows two indices to be provided.  The _ui32Mask indicates which indices are valid
		 * (see EE_ARRAY_EX_FLAGS).  Implementations may interpret the pair as a slice/range, 2D index,
		 * or other container-specific addressing scheme.
		 *
		 * \param _i64Idx0 The first index value (valid if EE_AEF_START is set in _ui32Mask).
		 * \param _i64Idx1 The second index value (valid if EE_AEF_END is set in _ui32Mask).
		 * \param _ui32Mask A mask of EE_ARRAY_EX_FLAGS describing which indices are valid.
		 * \return Returns the accessed element/range as an evaluator result, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		ArrayAccessEx( int64_t _i64Idx0, int64_t _i64Idx1, uint32_t _ui32Mask ) = 0 EE_PURE_DECL

		/**
		 * \brief Gets the logical length of the object when treated as an array.
		 *
		 * \return Returns the number of accessible elements.
		 */
		virtual size_t								ArrayLength() { return 1; }

		/**
		 * \brief Compares this object with another evaluator value for equality.
		 *
		 * Implementations should interpret _rRet as the right-hand operand and return true if the comparison
		 * can be performed, setting an appropriate result internally as required by the evaluator.
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation is supported and the comparison was performed; false otherwise.
		 */
		virtual bool								Equals( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return false; }

		/**
		 * \brief Adds this object and another evaluator value.
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns the result of the operation, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Plus( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return { EE_NC_INVALID }; }

		/**
		 * \brief Subtracts another evaluator value from this object.
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns the result of the operation, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Minus( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return { EE_NC_INVALID }; }

		/**
		 * \brief Multiplies this object by another evaluator value.
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns the result of the operation, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Multiply( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return { EE_NC_INVALID }; }

		/**
		 * \brief Divides this object by another evaluator value.
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns the result of the operation, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Divide( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return { EE_NC_INVALID }; }

		/**
		 * \brief Performs an in-place addition (operator+=).
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								PlusEquals( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return false; }

		/**
		 * \brief Performs an in-place subtraction (operator-=).
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								MinusEquals( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return false; }

		/**
		 * \brief Performs an in-place multiplication (operator*=).
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								TimesEquals( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return false; }

		/**
		 * \brief Performs an in-place division (operator/=).
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								DivideEquals( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return false; }

		/**
		 * \brief Performs an in-place modulo (operator%=).
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								ModEquals( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return false; }

		/**
		 * \brief Performs an in-place left shift (operator<<=).
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								ShiftLeftEquals( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return false; }

		/**
		 * \brief Performs an in-place right shift (operator>>=).
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								ShiftRightEquals( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return false; }

		/**
		 * \brief Performs an in-place XOR (operator^=).
		 *
		 * \note Named "CarrotEquals" to mirror the caret (^) operator name.
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								CarrotEquals( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return false; }

		/**
		 * \brief Performs an in-place bitwise OR (operator|=).
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								OrEquals( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return false; }

		/**
		 * \brief Performs an in-place bitwise AND (operator&=).
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								AndEquals( CExpEvalContainer::EE_RESULT &_rRet ) { static_cast<void>(_rRet); return false; }


	protected :
		// == Members.
		CExpEvalContainer *							m_peecContainer;
	};

}	// namespace ee

#if defined( _MSC_VER )
#pragma warning( pop )
#endif	// #if defined( _MSC_VER )

#undef EE_PURE_DECL

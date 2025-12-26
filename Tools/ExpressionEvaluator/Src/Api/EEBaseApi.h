#pragma once

#include "../EEExpEval.h"
#include "../EEExpEvalContainer.h"

#if defined( _MSC_VER )
#define EE_PURE_DECL											{ return { EE_NC_INVALID }; }
#else
#define EE_PURE_DECL											;
#endif	// #if defined( _MSC_VER )

namespace ee {

	/**
	 * Objects that allow the base API to operate on them must provide implementations of these functions.
	 */
	class CBaseApi {
	public :
		// == Functions.
		/**
		 * \brief Converts the object from UTF-8 to ASCII.
		 *
		 * \return Returns the converted value as an EE_RESULT.
		 *
		 * \note Behavior is implementation-defined for code points that are not representable in ASCII.
		 */
		virtual CExpEvalContainer::EE_RESULT					Ascii() const = 0 EE_PURE_DECL

		/**
		 * \brief Gets the binary form of the object as a string.
		 *
		 * \return Returns an EE_RESULT containing a string formatted as 0b****.
		 */
		virtual CExpEvalContainer::EE_RESULT					Bin() const = 0 EE_PURE_DECL

		/**
		 * \brief Gets the boolean form of the object as a string.
		 *
		 * \return Returns an EE_RESULT containing a string formatted as 0b0 or 0b1.
		 */
		virtual CExpEvalContainer::EE_RESULT					Bool() const = 0 EE_PURE_DECL

		/**
		 * \brief Returns the character represented by the object as a Unicode code point.
		 *
		 * \return Returns an EE_RESULT containing the resulting character.
		 *
		 * \note This is typically the inverse of Ord() for values that represent a valid Unicode scalar value.
		 */
		virtual CExpEvalContainer::EE_RESULT					Chr() const = 0 EE_PURE_DECL

		/**
		 * \brief Interprets the object as its best-fit numeric representation and converts it to a float.
		 *
		 * \return Returns an EE_RESULT containing the converted floating-point value.
		 */
		virtual CExpEvalContainer::EE_RESULT					Float() const = 0 EE_PURE_DECL

		/**
		 * \brief Gets the hexadecimal form of the object as a string.
		 *
		 * \return Returns an EE_RESULT containing a string formatted as 0x****.
		 */
		virtual CExpEvalContainer::EE_RESULT					Hex() const = 0 EE_PURE_DECL

		/**
		 * \brief Interprets the object as its best-fit numeric representation and converts it to an int64_t.
		 *
		 * \return Returns an EE_RESULT containing the converted integer value.
		 */
		virtual CExpEvalContainer::EE_RESULT					Int() const = 0 EE_PURE_DECL

		/**
		 * \brief Gets the number of elements in the object.
		 *
		 * \return Returns an EE_RESULT containing the element count.
		 *
		 * \note For arrays, maps, sets, lists, etc., this is the number of items.
		 * \note For Unicode strings, this is the number of UTF code points.
		 */
		virtual CExpEvalContainer::EE_RESULT					Len() const = 0 EE_PURE_DECL

		/**
		 * \brief Gets the octal form of the object as a string.
		 *
		 * \return Returns an EE_RESULT containing a string formatted as 0o****.
		 */
		virtual CExpEvalContainer::EE_RESULT					Oct() const = 0 EE_PURE_DECL

		/**
		 * \brief Returns the ordinal (numeric) value of the object as a Unicode code point.
		 *
		 * \return Returns an EE_RESULT containing the ordinal value (always EE_NC_UNSIGNED).
		 *
		 * \note This is typically the inverse of Chr() for values that represent a valid Unicode scalar value.
		 */
		virtual CExpEvalContainer::EE_RESULT					Ord() const = 0 EE_PURE_DECL

		/**
		 * \brief Converts a signed array index into a zero-based linear index.
		 *
		 * \param _i64SignedIdx The signed index to translate. Non-negative values index from the beginning; negative values index from the end.
		 * \param _stActualLen The length of the array.
		 * \return Returns the translated zero-based index on success; EE_INVALID_IDX on failure.
		 *
		 * \note Negative indices behave like Python indexing:
		 * - -1 refers to the last element.
		 * - -2 refers to the second-to-last element, etc.
		 * \note If \p _i64SignedIdx is out of range for \p _stActualLen, EE_INVALID_IDX is returned.
		 * \note This function also rejects negative values that cannot be safely represented as size_t.
		 */
		static size_t											ArrayIndexToLinearIndex( int64_t _i64SignedIdx, size_t _stActualLen ) {
			if ( _i64SignedIdx < 0 ) {
				_i64SignedIdx = -_i64SignedIdx - 1;
				if ( _i64SignedIdx != static_cast<int64_t>(static_cast<size_t>(_i64SignedIdx)) || static_cast<size_t>(_i64SignedIdx) >= _stActualLen ) { return EE_INVALID_IDX; }
				return static_cast<size_t>(_stActualLen-1-_i64SignedIdx);
			}
			if ( _i64SignedIdx != static_cast<int64_t>(static_cast<size_t>(_i64SignedIdx)) || static_cast<size_t>(_i64SignedIdx) >= _stActualLen ) { return EE_INVALID_IDX; }
			return static_cast<size_t>(_i64SignedIdx);
		}
		
	};

}	// namespace ee

#undef EE_PURE_DECL

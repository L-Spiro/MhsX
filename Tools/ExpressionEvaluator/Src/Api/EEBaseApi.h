#pragma once

#include "../EEExpEval.h"
#include "../EEExpEvalContainer.h"

namespace ee {

	/**
	 * Objects that allow the base API to operate on them must provide implementations of these functions.
	 */
	class CBaseApi {
	public :
		// == Functions.
		// Converts from UTF-8 to ASCII.
		virtual CExpEvalContainer::EE_RESULT					Ascii() const = 0 { return { EE_NC_INVALID }; }

		// Gets the binary form of the object as a string (0b****).
		virtual CExpEvalContainer::EE_RESULT					Bin() const = 0 { return { EE_NC_INVALID }; }

		// Gets the boolean form of the object as a string (0b****).
		virtual CExpEvalContainer::EE_RESULT					Bool() const = 0 { return { EE_NC_INVALID }; }

		// Returns the character that represents the specified Unicode. 
		virtual CExpEvalContainer::EE_RESULT					Chr() const = 0 { return { EE_NC_INVALID }; }

		// Gets the string interpreted to its best fit and then converted to float. 
		virtual CExpEvalContainer::EE_RESULT					Float() const = 0 { return { EE_NC_INVALID }; }

		// Gets the hexadecimal form of the object as a string (0x****).
		virtual CExpEvalContainer::EE_RESULT					Hex() const = 0 { return { EE_NC_INVALID }; }

		// Gets the string interpreted to its best fit and then converted to int64_t. 
		virtual CExpEvalContainer::EE_RESULT					Int() const = 0 { return { EE_NC_INVALID }; }

		// Gets the number of elements in the object.  For arrays, maps, sets, lists, etc., this is the number of items.  For Unicode strings, this is the number of UTF code points.
		virtual CExpEvalContainer::EE_RESULT					Len() const = 0 { return { EE_NC_INVALID }; }

		// Gets the octadecimal form of the object as a string (0o****).
		virtual CExpEvalContainer::EE_RESULT					Oct() const = 0 { return { EE_NC_INVALID }; }

		// Returns the ordinal value of the object as a Unicode character (always EE_NC_UNSIGNED).
		virtual CExpEvalContainer::EE_RESULT					Ord() const = 0 { return { EE_NC_INVALID }; }

		// Translates from a signed index to an actual index given an array's length.
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

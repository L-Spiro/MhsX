#pragma once

#include "../EEExpEval.h"
#include "../EEExpEvalContainer.h"


namespace ee {

	// Implementations of common string functions.
	class CStringBaseApi {
	public :
		// == Functions.
		// Creates an EE_RESULT using a best-fit interpretation of the string as a number.
		static CExpEvalContainer::EE_RESULT			BestFitResult( const std::string &_sIn );

		// Gets the object as an ASCII string.
		static CExpEvalContainer::EE_RESULT			Ascii( const std::string &_sIn, CExpEvalContainer * _peecCont );

		// Gets the binary form of the object as a string (0b****).
		static CExpEvalContainer::EE_RESULT			Bin( const std::string &_sIn, CExpEvalContainer * _peecCont );

		// Gets the boolean form of the object as a string (0b****).
		static CExpEvalContainer::EE_RESULT			Bool( const std::string &_sIn, CExpEvalContainer * _peecCont );

		// Returns the character that represents the specified Unicode. 
		static CExpEvalContainer::EE_RESULT			Chr( const std::string &_sIn, CExpEvalContainer * _peecCont );

		// Gets the string interpreted to its best fit and then converted to float. 
		static CExpEvalContainer::EE_RESULT			Float( const std::string &_sIn, CExpEvalContainer * _peecCont );

		// Gets the string interpreted to its best fit and then converted to int64_t. 
		static CExpEvalContainer::EE_RESULT			Int( const std::string &_sIn, CExpEvalContainer * _peecCont );

		// Gets the hexadecimal form of the object as a string (0x****).
		static CExpEvalContainer::EE_RESULT			Hex( const std::string &_sIn, CExpEvalContainer * _peecCont );

		// Gets the octadecimal form of the object as a string (0o****).
		static CExpEvalContainer::EE_RESULT			Oct( const std::string &_sIn, CExpEvalContainer * _peecCont );

		// Returns the ordinal value of the object as a Unicode character (always EE_NC_UNSIGNED).
		static CExpEvalContainer::EE_RESULT			Ord( const std::string &_sIn, CExpEvalContainer * _peecCont );

		// Updates an optimization table for improving array-access speeds.
		static void									UpdateArrayAccessOptimizer( const std::string &_sIn,
			std::vector<size_t> &_vArray,
			size_t _stIdx, uint32_t &_ui32Result );
	};

}	// namespace ee
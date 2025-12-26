#pragma once

#include "../EEExpEval.h"
#include "../EEExpEvalContainer.h"


namespace ee {

	// Implementations of common string functions.
	class CStringBaseApi {
	public :
		// == Functions.
		/**
		 * \brief Creates an EE_RESULT using a best-fit interpretation of the string as a number.
		 *
		 * The input string is analyzed to determine the most appropriate numeric interpretation (for example,
		 * integer vs. floating-point, and/or base detection).  If no reasonable numeric interpretation can be
		 * made, the returned result is EE_NC_INVALID.
		 *
		 * \param _sIn The input string to interpret.
		 * \return Returns an EE_RESULT containing the interpreted numeric value, or EE_NC_INVALID on failure.
		 */
		static CExpEvalContainer::EE_RESULT			BestFitResult( const std::string &_sIn );

		/**
		 * \brief Gets the object as an ASCII string.
		 *
		 * Produces a string object containing the ASCII representation of the input.  Non-ASCII characters may
		 * be replaced, removed, or otherwise normalized according to the evaluator's rules.
		 *
		 * \param _sIn The input string.
		 * \param _peecCont Evaluator/container used to create any required objects/results.
		 * \return Returns an EE_RESULT representing the ASCII string result, or EE_NC_INVALID on failure.
		 */
		static CExpEvalContainer::EE_RESULT			Ascii( const std::string &_sIn, CExpEvalContainer * _peecCont );

		/**
		 * \brief Gets the binary form of the object as a string (0b****).
		 *
		 * The input is first interpreted as a number (best-fit), then formatted as a binary literal string with
		 * a "0b" prefix.
		 *
		 * \param _sIn The input string.
		 * \param _peecCont Evaluator/container used to create any required objects/results.
		 * \return Returns an EE_RESULT representing the binary string result, or EE_NC_INVALID on failure.
		 */
		static CExpEvalContainer::EE_RESULT			Bin( const std::string &_sIn, CExpEvalContainer * _peecCont );

		/**
		 * \brief Gets the boolean form of the object as a string (0b****).
		 *
		 * The input is interpreted (best-fit) and reduced to a boolean value, which is then formatted as a
		 * binary literal string (typically "0b0" or "0b1").
		 *
		 * \param _sIn The input string.
		 * \param _peecCont Evaluator/container used to create any required objects/results.
		 * \return Returns an EE_RESULT representing the boolean string result, or EE_NC_INVALID on failure.
		 */
		static CExpEvalContainer::EE_RESULT			Bool( const std::string &_sIn, CExpEvalContainer * _peecCont );

		/**
		 * \brief Returns the character that represents the specified Unicode.
		 *
		 * The input is interpreted as a Unicode code point value and converted into a string containing the
		 * corresponding UTF-8 character.
		 *
		 * \param _sIn The input string.
		 * \param _peecCont Evaluator/container used to create any required objects/results.
		 * \return Returns an EE_RESULT representing the character string result, or EE_NC_INVALID on failure.
		 */
		static CExpEvalContainer::EE_RESULT			Chr( const std::string &_sIn, CExpEvalContainer * _peecCont );

		/**
		 * \brief Gets the string interpreted to its best fit and then converted to float.
		 *
		 * The input is parsed using a best-fit numeric interpretation and returned as a floating-point result.
		 *
		 * \param _sIn The input string.
		 * \param _peecCont Evaluator/container used to create any required objects/results.
		 * \return Returns an EE_RESULT containing the floating-point value, or EE_NC_INVALID on failure.
		 */
		static CExpEvalContainer::EE_RESULT			Float( const std::string &_sIn, CExpEvalContainer * _peecCont );

		/**
		 * \brief Gets the string interpreted to its best fit and then converted to int64_t.
		 *
		 * The input is parsed using a best-fit numeric interpretation and returned as a signed 64-bit integer
		 * result.
		 *
		 * \param _sIn The input string.
		 * \param _peecCont Evaluator/container used to create any required objects/results.
		 * \return Returns an EE_RESULT containing the integer value, or EE_NC_INVALID on failure.
		 */
		static CExpEvalContainer::EE_RESULT			Int( const std::string &_sIn, CExpEvalContainer * _peecCont );

		/**
		 * \brief Gets the hexadecimal form of the object as a string (0x****).
		 *
		 * The input is first interpreted as a number (best-fit), then formatted as a hexadecimal literal string
		 * with a "0x" prefix.
		 *
		 * \param _sIn The input string.
		 * \param _peecCont Evaluator/container used to create any required objects/results.
		 * \return Returns an EE_RESULT representing the hexadecimal string result, or EE_NC_INVALID on failure.
		 */
		static CExpEvalContainer::EE_RESULT			Hex( const std::string &_sIn, CExpEvalContainer * _peecCont );

		/**
		 * \brief Gets the octadecimal form of the object as a string (0o****).
		 *
		 * The input is first interpreted as a number (best-fit), then formatted as an octal literal string with
		 * a "0o" prefix.
		 *
		 * \param _sIn The input string.
		 * \param _peecCont Evaluator/container used to create any required objects/results.
		 * \return Returns an EE_RESULT representing the octal string result, or EE_NC_INVALID on failure.
		 */
		static CExpEvalContainer::EE_RESULT			Oct( const std::string &_sIn, CExpEvalContainer * _peecCont );

		/**
		 * \brief Returns the ordinal value of the object as a Unicode character (always EE_NC_UNSIGNED).
		 *
		 * The input is interpreted as a Unicode character/code point and returned as its unsigned ordinal value.
		 *
		 * \param _sIn The input string.
		 * \param _peecCont Evaluator/container used to create any required objects/results.
		 * \return Returns an EE_RESULT of type EE_NC_UNSIGNED containing the ordinal value, or EE_NC_INVALID on failure.
		 */
		static CExpEvalContainer::EE_RESULT			Ord( const std::string &_sIn, CExpEvalContainer * _peecCont );

		/**
		 * \brief Updates an optimization table for improving array-access speeds.
		 *
		 * This function updates _vArray with cached byte offsets (or other indexing metadata) that accelerates
		 * code-point-based indexing into the UTF-8 string.  The implementation may update only a portion of the
		 * table beginning at _stIdx and reports status/details through _ui32Result.
		 *
		 * \param _sIn The input UTF-8 string.
		 * \param _vArray The optimization table to update.
		 * \param _stIdx The starting element/code-point index at which to begin updating.
		 * \param _ui32Result Receives implementation-defined status/result information.
		 */
		static void									UpdateArrayAccessOptimizer( const std::string &_sIn,
			std::vector<size_t> &_vArray,
			size_t _stIdx, uint32_t &_ui32Result );
	};

}	// namespace ee
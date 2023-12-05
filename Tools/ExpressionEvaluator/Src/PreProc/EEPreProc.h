/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: Parses a single expression related to #if and #elif.
 */


#ifndef __EE_PREPROC_H__
#define __EE_PREPROC_H__

#include "../EEExpEval.h"
#include "EEPreProcSyntaxNodes.h"		// Must be included before LSGPreProcParser.h.
#include "../PreProcGen/EEPreProcParser.h"
#include "EEPreProcContainer.h"

#include <map>
#include <set>

namespace ee {

	/**
	 * Class CPreProc
	 * \brief Parses a single expression related to #if and #elif.
	 *
	 * Description: Parses a single expression related to #if and #elif.
	 */
	class CPreProc {
	public :
		// == Various constructors.
		CPreProc();

		
		// == Enumerations.
		/**
		 * Error codes.
		 */
		enum EE_ERRORS {
			EE_E_FAILURE,							/**< A nor-error parsing error occurred. */
			EE_E_SUCCESS,							/**< Parsing succeeded. */
			EE_E_ERROR,								/**< Parsing failed. */
			EE_E_OUT_OF_MEMORY,						/**< Memory error. */
			EE_E_PROCESSING_ERROR,					/**< General processing error. */
			EE_E_SYNTAX_IF,							/**< Syntax error on an #if directive. */
			EE_E_SYNTAX_ELIF,						/**< Syntax error on an #elif directive. */
			EE_E_SYNTAX_ELSE,						/**< Syntax error on an #else directive. */
			EE_E_SYNTAX_DEFINE,						/**< Syntax error on a #define directive. */
			EE_E_SYNTAX_UNDEF,						/**< Syntax error on an #undef directive. */
			EE_E_MACRO_EXPANSION,					/**< Error expanding a macro. */
			EE_E_UNMATCHED_ENDIF,					/**< An unmatched #endif. */
		};


		// == Types.
		/**
		 * A define.
		 */
		typedef struct EE_PREPROC_DEFINE {
			/**
			 * The name of the define.
			 */
			std::string								sName;

			/**
			 * Optional parameters.
			 */
			std::vector<std::string>				sParms;


			// == Operators.
			/**
			 * Copy operator.
			 *
			 * \param _pdOther The define to copy.
			 * \return Returns this object after the copy.
			 */
			EE_PREPROC_DEFINE &						operator = ( const EE_PREPROC_DEFINE &_pdOther );

			/**
			 * Compare this object against other.  Returns true if the name of this object comes before
			 *	the name of the given object.
			 *
			 * \param _pdOther The object against which to compare this object.
			 * \return Returns true if the name of this object comes before the name of the given object.
			 */
			bool									operator < ( const EE_PREPROC_DEFINE &_pdOther ) const;

			/**
			 * Check for equality between this object and another.
			 *
			 * \param _pdOther The object against which to check for equality.
			 * \return Returns true if the names of both objects are the same.
			 */
			bool									operator == ( const EE_PREPROC_DEFINE &_pdOther ) const;
		} * LPEE_PREPROC_DEFINE, * const LPCEE_PREPROC_DEFINE;

		/**
		 * The dictionary.
		 */
		typedef std::map<EE_PREPROC_DEFINE, std::string>
													EE_MACROS;


		// == Functions.
		/**
		 * Gets the container.
		 *
		 * \return Returns the container.
		 */
		CPreProcContainer &							GetContainer();

		/**
		 * Parse an expression, returning an error code.
		 *
		 * \param _sExp The expression to parse.
		 * \return Returns an error code depending on the result of the expression.  EE_E_ERROR is returned
		 *	if the expression is bad, EE_E_SUCCESS indicates that the expression is valid.
		 */
		EE_ERRORS									Parse( const std::string &_sExp );

		/**
		 * Evaluate the parsed expression with the given macro dictionary.
		 *
		 * \param _mMacros The macros.
		 * \param _bRet Holds the result of the expression.
		 * \return Returns true if the expression was successfully evaluated.
		 */
		bool										GetResult( const EE_MACROS &_mMacros, bool &_bRet ) const;

		/**
		 * Fully preprocesses an in-memory file.
		 *
		 * \param _sFile The file to preprocess.
		 * \param _sResult The preprocessed result.
		 * \param _mMacros Predefined macros.
		 * \return Returns an error code indicating success or a type of failure.
		 */
		static EE_ERRORS							PreProcessFile( const std::string &_sFile,
			std::string &_sResult,
			EE_MACROS &_mMacros );


	protected :
		// == Types.
		/**
		 * Clear states for internal use.
		 */
		enum EE_CLEAR_STATE {
			EE_CS_NONE,
			EE_CS_CLEAR,
			EE_CS_SEEKING_ENDIF,
		};

		/**
		 * # and ## directives.
		 */
		enum EE_TOKEN {
			EE_T_NONE,
			EE_T_CONCAT,
			EE_T_STRINGIZE
		};


		// == Members.
		/** The container for holding the parsed data. */
		CPreProcContainer							m_ppcContainer;


		// == Functions.
		/**
		 * Process a single node and return the result of that node.
		 *
		 * \param _stIndex The index of the node to process.
		 * \param _i64Return Holds the return value of the node.
		 * \param _mMacros The macros.
		 * \return Returns true if the node was evaluated.
		 */
		bool										EvalNode( size_t _stIndex, int64_t &_i64Return, const EE_MACROS &_mMacros ) const;

		/**
		 * Handles a #define.
		 *
		 * \param _sLine The text after "#define".  Parsed to determin the name, parameters, and definition of a macro.
		 * \param _mMacros Set of macros to which to add the new macro.
		 * \return Returns true if the text is valid and the macro was added.
		 */
		static EE_ERRORS							ParseDefine( const std::string &_sLine, CPreProc::EE_MACROS &_mMacros );

		/**
		 * Replace all macros in a given string.
		 *
		 * \param _sString The string in which to expand macros.
		 * \param _mMacros The macro dictionary.
		 * \return Returns true if the expansion succeeds.  Otherwise fills _sError with the error string and returns false.
		 */
		static bool									ExpandMacros( std::string &_sString, const CPreProc::EE_MACROS &_mMacros );

		/**
		 * Replace all macros in a given string.
		 *
		 * \param _sString The string in which to expand macros.
		 * \param _mMacros The macro dictionary.
		 * \param _sUsedValues Values that have already been expanded.
		 * \return Returns true if the expansion succeeds.  Otherwise fills _sError with the error string and returns false.
		 */
		static bool									ExpandMacros( std::string &_sString, const CPreProc::EE_MACROS &_mMacros, const std::set<std::string> &_sUsedValues );

		/**
		 * Gets the parameters for a function-style macro.  If no parameters are given, returns false.
		 *
		 * \param _psString The string from which to extract function-style macro parameters.
		 * \param _stPos The position at which to begin the extraction.
		 * \param _stNewPos The position after successful extraction.
		 * \param _vRet The returned parameters, as strings.
		 * \return Returns true if the parameters were extracted.  False indicates no parameters present or an invalid parameter string.
		 */
		static bool									GetMacroParms( const std::string &_sString, size_t _stPos, size_t &_stNewPos, std::vector<std::string> &_vRet );

		/**
		 * Gets the replacement string for a given macro and optional parameters.  Returns false if there is an error expanding the macro.
		 *
		 * \param _iMacro Iterator of the macro to expand.
		 * \param _vParms Optional macro parameters.  If this does not match the number of parameters the macro receives, false is returned.
		 * \param _mMacros The macro dictionary.
		 * \param _sUsedValues Values that have already been expanded.  Allows recursion to be avoided.
		 * \param _sRet The returned fully expanded string.
		 * \return Returns true if there were no errors during macro expansion.
		 */
		static bool									GetMacroReplacementString( CPreProc::EE_MACROS::const_iterator &_iMacro, std::vector<std::string> &_vParms,
			const CPreProc::EE_MACROS &_mMacros, const std::set<std::string> &_sUsedValues,
			std::string &_sRet );

		/**
		 * Eats a macro-expansion token from the end of a line and 
		 *
		 * \param PARM DESC
		 * \param PARM DESC
		 * \return RETURN
		 */
	};

}	// namespace ee

#endif	// __EE_PREPROC_H__


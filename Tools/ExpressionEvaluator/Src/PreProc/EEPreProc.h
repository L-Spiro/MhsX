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
			const EE_MACROS &_mMacros );


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
	};

}	// namespace ee

#endif	// __EE_PREPROC_H__


#pragma once

#include "EEExpEval.h"
#include "EEExpEvalContainer.h"

#ifdef yyFlexLexer
#undef yyFlexLexer
#endif
#define yyFlexLexer			CExpEvalLexBase
#include "Gen/EEExpEvalLexBase.h"
#include "Gen/EEExpEvalParser.h"
#include "Unicode/EEUnicode.h"

namespace ee {

	/**
	 * Class CExpEvalLexer
	 * \brief Defines the lexer base class by #define'ing yyFlexLexer before including EEExpEvalLexBase.h.
	 *
	 * Description: Defines the lexer base class by #define'ing yyFlexLexer before including EEExpEvalLexBase.h.
	 */
	class CExpEvalLexer : public CExpEvalLexBase {
	public :
		// == Various constructors.
		CExpEvalLexer( std::istream * _pisStream, std::ostream * _posStream = nullptr ) :
			Parent( _pisStream, _posStream ),
			m_pslpContainer( nullptr ) {
		}
		virtual ~CExpEvalLexer() {
		}


		// == Functions.
		/**
		 * \brief Sets the parser container used by the lexer.
		 *
		 * The lexer queries the container for the current compilation/evaluation state, such as which identifiers
		 * are custom variables or arrays, and uses that information to adjust emitted token types.
		 *
		 * \param _slpParserContainer Container that provides identifier classification and other parser state.
		 */
		void							SetContainer( const CExpEvalContainer &_slpParserContainer ) {
			m_pslpContainer = &_slpParserContainer;
		}

		/**
		 * \brief Lexes the next token from the input stream.
		 *
		 * Delegates to the base lexer to obtain the next token, then optionally reclassifies identifiers based on
		 * the current container state. If the token is an identifier and a container has been set, the identifier
		 * may be converted to a custom-variable token or an array token.
		 *
		 * \return Returns the next token code.
		 */
		virtual int						yylex() {
			int iRet = CExpEvalLexBase::yylex();
			const char * pcText = YYText();
			if ( iRet == yy::CExpEvalParser::token::EE_IDENTIFIER && m_pslpContainer != nullptr ) {
				if ( m_pslpContainer->IsCustomVar( pcText ) ) {
					iRet = yy::CExpEvalParser::token::EE_CUSTOM_VAR;
				}
				if ( m_pslpContainer->IsArray( pcText ) ) {
					iRet = yy::CExpEvalParser::token::EE_ARRAY;
				}
			}
			
			return iRet;
		}


	protected :
		// == Members.
		const CExpEvalContainer *		m_pslpContainer;										/**< The container that handles collection of all of the script data. */

	private :
		typedef CExpEvalLexBase			Parent;
	};

}	// namespace ee {


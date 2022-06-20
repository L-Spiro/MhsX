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
		// Sets the container.  This is used to access everything the lexer needs about the current state of the
		//	compiled data.
		void							SetContainer( const CExpEvalContainer &_slpParserContainer ) {
			m_pslpContainer = &_slpParserContainer;
		}

		// The lexer.
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
		// The container that handles collection of all of the script data.
		const CExpEvalContainer *		m_pslpContainer;

	private :
		typedef CExpEvalLexBase			Parent;
	};

}	// namespace ee {


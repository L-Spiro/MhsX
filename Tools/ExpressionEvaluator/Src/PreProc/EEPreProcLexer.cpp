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
 * Description: Defines the lexer base class for the shader preprocessor by #define'ing yyFlexLexer before
*	including LSGPreProcLexBase.h.
 */

#include "EEPreProcLexer.h"


namespace ee {

	// == Various constructors.
	CPreProcLexer::CPreProcLexer( std::istream * _pisStream, std::ostream * _posStream ) :
		Parent( _pisStream, _posStream ) {
	}

	// == Functions.
	/**
	 * Perform the work of scanning input for tokens.
	 *
	 * \return Returns the next token.
	 */
	uint32_t CPreProcLexer::NewLex() {
		return Parent::yylex();
	}

}	// namespace ee

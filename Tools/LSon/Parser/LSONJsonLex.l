%option 				noyywrap 8bit nodefault


%{
/*
 * VICIOUSLY, BRUTALLY, MERCILESSLY borrowed from https://lloydrochester.com/post/flex-bison/json-parse/
 */

#include "../LSONJsonSyntaxNodes.h"
#include "../LSONJsonContainer.h"
#include "../LSONJsonLexer.h"
#include "LSONJsonParser.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YY_NEVER_INTERACTIVE		1

%}

EXP 					([Ee][-+]?[0-9]+)

%%

"{"						{ return yy::CJsonParser::token::LSON_LCURLY; }
"}"						{ return yy::CJsonParser::token::LSON_RCURLY; }
"["						{ return yy::CJsonParser::token::LSON_LBRAC; }
"]"						{ return yy::CJsonParser::token::LSON_RBRAC; }
","						{ return yy::CJsonParser::token::LSON_COMMA; }
":"						{ return yy::CJsonParser::token::LSON_COLON; }
"true"					{ return yy::CJsonParser::token::LSON_VTRUE; }
"false"					{ return yy::CJsonParser::token::LSON_VFALSE; }
"null"					{ return yy::CJsonParser::token::LSON_VNULL; }
[ \t\r\n]+				{ }
\"(\\.|[^\\"])*\"		{ return yy::CJsonParser::token::LSON_STRING; }
-?[0-9]+\.?[0-9]*{EXP}?	{ return yy::CJsonParser::token::LSON_DECIMAL; }
.						{ }

%%

/**
 * Scans directly from a contiguous block of memory.
 * The buffer MUST end with two NUL ('\0') bytes.
 *
 * \param _pcBase The contiguous block of memory to scan.
 * \param _stSize The total size of the memory block, including the two NUL bytes.
 * \return Returns the new buffer state, or nullptr if the buffer is invalid.
 */
struct yy_buffer_state * CJsonLexer::ScanMemoryBuffer( char * _pcBase, size_t _stSize ) {
	if ( _stSize < 2 || _pcBase[_stSize-2] != '\0' || _pcBase[_stSize-1] != '\0' ) {
		return nullptr;
	}

	struct yy_buffer_state * pbState = new yy_buffer_state;
	if ( !pbState ) {
		return nullptr;
	}

	pbState->yy_buf_size = static_cast<int>(_stSize - 2);
	pbState->yy_buf_pos = pbState->yy_ch_buf = _pcBase;
	
	pbState->yy_is_our_buffer = 0; 
	pbState->yy_input_file = nullptr;
	pbState->yy_n_chars = pbState->yy_buf_size;
	pbState->yy_is_interactive = 0;
	pbState->yy_at_bol = 1;
	
	pbState->yy_fill_buffer = 0;   
	pbState->yy_buffer_status = 0; // YY_BUFFER_NEW

	yy_switch_to_buffer( pbState );
	return pbState;
}
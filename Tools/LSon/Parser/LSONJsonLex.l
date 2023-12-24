%option 				yylineno noyywrap 8bit nodefault


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
[0-9]+\.?[0-9]*{EXP}?	{ return yy::CJsonParser::token::LSON_DECIMAL; }
.						{ }

%%

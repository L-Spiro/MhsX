%{
/*
 * VICIOUSLY, BRUTALLY, MERCILESSLY borrowed from https://lloydrochester.com/post/flex-bison/json-parse/
 */

#include "../LSONJson.h"
#include "../LSONJsonContainer.h"
#include "../LSONJsonLexer.h"
#include <string>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YYINCLUDED_STDLIB_H

//extern char yytext[];
// Announce to Flex the prototype we want for lexing function.
extern int yylex( /*YYSTYPE*/void * _pvNodeUnion, lson::CJsonLexer * _pxlLexer );

#include "../LSONJsonSyntaxNodes.h"

%}

%pure-parser

%parse-param												{ class CJsonLexer * m_peelLexer }
%parse-param												{ class CJsonContainer * m_peecContainer }
%lex-param													{ CJsonLexer * m_peelLexer }


%token LSON_LCURLY LSON_RCURLY LSON_LBRAC LSON_RBRAC LSON_COMMA LSON_COLON
%token LSON_VTRUE LSON_VFALSE LSON_VNULL
%token LSON_STRING LSON_DECIMAL

%start json

%%

json
	:
    | value
    ;

value
	: object
	| LSON_STRING
	| LSON_DECIMAL
	| array
	| LSON_VTRUE
	| LSON_VFALSE
	| LSON_VNULL
	;

object
	: LSON_LCURLY LSON_RCURLY
	| LSON_LCURLY members LSON_RCURLY
	;

members
	: member
	| members LSON_COMMA member
	;

member
	: LSON_STRING LSON_COLON value
    ;

array
	: LSON_LBRAC LSON_RBRAC
	| LSON_LBRAC values LSON_RBRAC
	;

values
	: value
	| values LSON_COMMA value
	;

%%

int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, lson::CJsonLexer * _pxlLexer ) {
	return _pxlLexer->yylex();
}

void yy::parser::error( const yy::location &/*_lLoc*/, const std::string &/*_strM*/ ) {

}
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

%parse-param												{ class CJsonLexer * m_pjlLexer }
%parse-param												{ class CJsonContainer * m_pjcContainer }
%lex-param													{ CJsonLexer * m_pjlLexer }


%token LSON_LCURLY LSON_RCURLY LSON_LBRAC LSON_RBRAC LSON_COMMA LSON_COLON
%token LSON_VTRUE LSON_VFALSE LSON_VNULL
%token LSON_STRING LSON_DECIMAL

%type <sStringIndex>										string
%type <dDecimal>											decimal
%type <nNode>												json
%type <nNode>												value
%type <nNode>												object
%type <nNode>												members
%type <nNode>												member
%type <nNode>												array
%type <nNode>												values

%start json

%%

json
	:
    | value
    ;

value
	: object
	| string												{ m_pjcContainer->AddStringValue( $$, $1 ); }
	| decimal												{ m_pjcContainer->AddDecimalValue( $$, $1 ); }
	| array
	| LSON_VTRUE											{ m_pjcContainer->AddTrueValue( $$ ); }
	| LSON_VFALSE											{ m_pjcContainer->AddFalseValue( $$ ); }
	| LSON_VNULL											{ m_pjcContainer->AddNullValue( $$ ); }
	;

object
	: LSON_LCURLY LSON_RCURLY
	| LSON_LCURLY members LSON_RCURLY
	;

members
	: member												{ $$ = $1; }
	| members LSON_COMMA member								{ m_pjcContainer->AddMemberList( $$, $1, $3 ); }
	;

member
	: string LSON_COLON value								{ m_pjcContainer->AddMember( $$, $1, $3 ); }
    ;

array
	: LSON_LBRAC LSON_RBRAC
	| LSON_LBRAC values LSON_RBRAC
	;

values
	: value
	| values LSON_COMMA value
	;
	
string
	: LSON_STRING											{ $$ = m_pjcContainer->AddValue( m_pjlLexer->YYText() ); }
	
decimal
	: LSON_DECIMAL											{ $$ = ::atof( m_pjlLexer->YYText() ); }

%%

int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, lson::CJsonLexer * _pxlLexer ) {
	return _pxlLexer->yylex();
}

void yy::parser::error( const yy::location &/*_lLoc*/, const std::string &/*_strM*/ ) {

}
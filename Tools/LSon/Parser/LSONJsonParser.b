%{
/*
 * VICIOUSLY, BRUTALLY, MERCILESSLY borrowed from https://lloydrochester.com/post/flex-bison/json-parse/
 */

#include "../LSONJson.h"
#include "../LSONJsonContainer.h"
#include "../LSONJsonLexer.h"
#include <string>
#include <string_view>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YYINCLUDED_STDLIB_H

// Announce to Flex the prototype we want for lexing function.
extern int yylex( void * _pvNodeUnion, lson::CJsonLexer * _pxlLexer );

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
%type <stValue>												value
%type <stValue>												object
%type <stValue>												array
%type <mMember>												member

%start json

%%

json
	:
	| value													{ m_pjcContainer->SetRoot( $1 ); }
	;

value
	: object												{ $$ = $1; }
	| string												{ $$ = m_pjcContainer->AddStringValue( $1 ); }
	| decimal												{ $$ = m_pjcContainer->AddDecimalValue( $1 ); }
	| array													{ $$ = $1; }
	| LSON_VTRUE											{ $$ = m_pjcContainer->AddTrueValue(); }
	| LSON_VFALSE											{ $$ = m_pjcContainer->AddFalseValue(); }
	| LSON_VNULL											{ $$ = m_pjcContainer->AddNullValue(); }
	;

object
	: LSON_LCURLY LSON_RCURLY								{ $$ = m_pjcContainer->AddObject(); }
	| LSON_LCURLY members LSON_RCURLY						{ $$ = m_pjcContainer->EndObject(); }
	;

members
	: member												{ m_pjcContainer->BeginMembers( $1 ); }
	| members LSON_COMMA member								{ m_pjcContainer->AddMember( $3 ); }
	;

member
	: string LSON_COLON value								{ $$.stName = $1; $$.stValue = $3; }
	;

array
	: LSON_LBRAC LSON_RBRAC									{ $$ = m_pjcContainer->AddArray(); }
	| LSON_LBRAC values LSON_RBRAC							{ $$ = m_pjcContainer->EndArray(); }
	;

values
	: value													{ m_pjcContainer->BeginValues( $1 ); }
	| values LSON_COMMA value								{ m_pjcContainer->AddArrayValue( $3 ); }
	;
	
string
	: LSON_STRING											{ $$ = m_pjcContainer->AddQuoteString( std::string_view( m_pjlLexer->YYText(), m_pjlLexer->YYLeng() ) ); }
	
decimal
	: LSON_DECIMAL											{ $$ = CJsonContainer::ParseDecimal( m_pjlLexer->YYText(), m_pjlLexer->YYLeng() ); }

%%

int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, lson::CJsonLexer * _pxlLexer ) {
	return _pxlLexer->yylex();
}

void yy::parser::error( const yy::location &/*_lLoc*/, const std::string &/*_strM*/ ) {

}

%{


#include "../EEExpEval.h"
#include "../EEExpEvalContainer.h"
#include "../EEExpEvalLexer.h"
#include <cstdio>

#pragma warning( push )

// warning C4065: switch statement contains 'default' but no 'case' labels
#pragma warning( disable : 4065 )
// warning C4127: conditional exp is constant
#pragma warning( disable : 4127 )
// warning C4512: 'slice<T,S>' : assignment operator could not be generated
#pragma warning( disable : 4512 )


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YYINCLUDED_STDLIB_H


// Announce to Flex the prototype we want for lexing function.
extern int yylex( /*YYSTYPE*/void * _pvNodeUnion, ee::CExpEvalLexer * _peelLexer );


#include "../EEExpEvalSyntaxNodes.h"


%}


%pure-parser

%parse-param												{ class CExpEvalLexer * m_peelLexer }
%parse-param												{ class CExpEvalContainer * m_peecContainer }
%lex-param													{ CExpEvalLexer * m_peelLexer }

%token EE_CHAR_CONSTANT EE_FLOAT_CONSTANT EE_STRING_CONSTANT EE_HEX_CONSTANT1 EE_HEX_CONSTANT2 EE_HEX_CONSTANT3
%token EE_DEC_CONSTANT EE_OCT_CONSTANT EE_PUREDEC_CONSTANT
%token EE_IDENTIFIER
%token EE_MEMBERACCESS
%token EE_EQU_E EE_EQU_NE EE_LEFT_OP EE_RIGHT_OP EE_REL_GE EE_REL_LE
%token EE_OB_BYTE EE_OB_WORD EE_OB_QWORD EE_OB_FLOAT EE_OB_DOUBLE EE_OB_DWORD
%token EE_COS EE_SIN EE_TAN EE_ACOS EE_ASIN EE_ATAN EE_ATAN2
%token EE_COSH EE_SINH EE_TANH EE_ACOSH EE_ASINH EE_ATANH
%token EE_EXP EE_LOG EE_LOG10 EE_LOG2 EE_EXP2 EE_EXPM1 EE_ILOGB EE_LOG1P EE_LOGB
%token EE_POW EE_SQRT EE_CBRT EE_HYPOT EE_TGAMMA EE_LGAMMA
%token EE_CEIL EE_FLOOR EE_MOD EE_TRUNC EE_ROUND EE_NEARBYINT EE_REMAINDER EE_REMQUO
%token EE_NEXTAFTER EE_NEXTTOWARD
%token EE_DIM EE_MAX EE_MIN
%token EE_ABS EE_MADD

%type <sStringIndex>										string												
%type <ndData>												basic_expr
%type <ndData>												postfix_exp
%type <ndData>												unary_exp
%type <ui32Unary>											unary_operator
%type <ndData>												multiplicative_exp
%type <ndData>												additive_exp
%type <ndData>												shift_exp
%type <ndData>												relational_exp
%type <ndData>												equality_exp
%type <ndData>												logical_and_exp
%type <ndData>												logical_xor_exp
%type <ndData>												logical_or_exp
%type <ndData>												and_exp
%type <ndData>												or_exp
%type <ndData>												conditional_exp
%type <ndData>												intrinsic
%type <ndData>												exp
%type <ndData>												translation_unit

%start translation_unit


%left	EE_OR
%left	EE_AND
%left	'|'
%left	'^'
%left	'&'
%left	EE_EQU_NE EE_EQU_E
%left	EE_REL_GE EE_REL_LE '>' '<'
%left	EE_RIGHT_OP EE_LEFT_OP
%left   '+' '-'
%left   '*' '/' '%'
%right	EE_UMINUS EE_UPLUS EE_UNARY_1SCOMPL EE_LOG_NOT
%left	EE_MEMBERACCESS


%%

string
	: EE_STRING_CONSTANT									{ $$ = m_peecContainer->CreateString( m_peelLexer->YYText() ); }
	| EE_IDENTIFIER											{ $$ = m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ); }
	;
	
basic_expr
	: string												{ m_peecContainer->CreateStringBasicExp( $1, $$ ); }
	| EE_HEX_CONSTANT1										{ m_peecContainer->CreateHex1( m_peelLexer->YYText(), $$ ); }
	| EE_HEX_CONSTANT2										{ m_peecContainer->CreateHex2( m_peelLexer->YYText(), $$ ); }
	| EE_HEX_CONSTANT3										{ m_peecContainer->CreateHex3( m_peelLexer->YYText(), $$ ); }
	| EE_DEC_CONSTANT										{
																if ( m_peecContainer->TreatAllAsHex() ) {
																	m_peecContainer->CreateHex3( m_peelLexer->YYText(), $$ );
																}
																else {
																	m_peecContainer->CreateUInt( m_peelLexer->YYText(), $$ );
																}
															}
	| EE_PUREDEC_CONSTANT									{ m_peecContainer->CreateUInt( m_peelLexer->YYText() + 1, $$ ); }
	| EE_OCT_CONSTANT										{ m_peecContainer->CreateOct( m_peelLexer->YYText(), $$ ); }
	| EE_CHAR_CONSTANT										{ m_peecContainer->CreateChar( m_peelLexer->YYText(), $$ ); }
	| EE_FLOAT_CONSTANT										{
																if ( m_peecContainer->TreatAllAsHex() && ValidHex( m_peelLexer->YYText() ) ) {
																	m_peecContainer->CreateHex3( m_peelLexer->YYText(), $$ );
																}
																else {
																	m_peecContainer->CreateDouble( m_peelLexer->YYText(), $$ );
																}
															}
	| '(' exp ')'											{ $$ = $2; }
	| '[' exp ']'											{ m_peecContainer->CreateAddress( $2, EE_CT_UINT32, $$ ); }
	| EE_OB_DWORD exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_UINT32, $$ ); }
	| EE_OB_BYTE exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_UINT8, $$ ); }
	| EE_OB_WORD exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_UINT16, $$ ); }
	| EE_OB_QWORD exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_UINT64, $$ ); }
	| EE_OB_FLOAT exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_FLOAT, $$ ); }
	| EE_OB_DOUBLE exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_DOUBLE, $$ ); }
	;

postfix_exp
	: basic_expr											{ $$ = $1; }
	| postfix_exp EE_MEMBERACCESS EE_IDENTIFIER				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	;

unary_exp
	: postfix_exp											{ $$ = $1; }
	| unary_operator unary_exp								{ m_peecContainer->CreateUnary( $2, $1, $$ ); }
	;

unary_operator
	: '+'	%prec  EE_UPLUS									{ $$ = '+'; }
	| '-'	%prec  EE_UMINUS								{ $$ = '-'; }
	| '~'	%prec  EE_UNARY_1SCOMPL							{ $$ = '~'; }
	| '!'	%prec  EE_LOG_NOT								{ $$ = '!'; }
	;

multiplicative_exp
	: unary_exp												{ $$ = $1; }
	| multiplicative_exp '*' unary_exp						{ m_peecContainer->CreateOp( $1, $3, '*', $$ ); }
	| multiplicative_exp '/' unary_exp						{ m_peecContainer->CreateOp( $1, $3, '/', $$ ); }
	| multiplicative_exp '%' unary_exp						{ m_peecContainer->CreateOp( $1, $3, '%', $$ ); }
	;

additive_exp
	: multiplicative_exp									{ $$ = $1; }
	| additive_exp '+' multiplicative_exp					{ m_peecContainer->CreateOp( $1, $3, '+', $$ ); }
	| additive_exp '-' multiplicative_exp					{ m_peecContainer->CreateOp( $1, $3, '-', $$ ); }
	;

shift_exp
	: additive_exp											{ $$ = $1; }
	| shift_exp EE_RIGHT_OP additive_exp					{ m_peecContainer->CreateOp( $1, $3, token::EE_RIGHT_OP, $$ ); }
	| shift_exp EE_LEFT_OP additive_exp						{ m_peecContainer->CreateOp( $1, $3, token::EE_LEFT_OP, $$ ); }
	;

relational_exp
	: shift_exp												{ $$ = $1; }
	| relational_exp '<' shift_exp							{ m_peecContainer->CreateOp( $1, $3, '<', $$ ); }
	| relational_exp '>' shift_exp							{ m_peecContainer->CreateOp( $1, $3, '>', $$ ); }
	| relational_exp EE_REL_LE shift_exp					{ m_peecContainer->CreateOp( $1, $3, token::EE_REL_LE, $$ ); }
	| relational_exp EE_REL_GE shift_exp					{ m_peecContainer->CreateOp( $1, $3, token::EE_REL_GE, $$ ); }
	;

equality_exp
	: relational_exp										{ $$ = $1; }
	| equality_exp EE_EQU_E relational_exp					{ m_peecContainer->CreateOp( $1, $3, token::EE_EQU_E, $$ ); }
	| equality_exp EE_EQU_NE relational_exp					{ m_peecContainer->CreateOp( $1, $3, token::EE_EQU_NE, $$ ); }
	;
	
logical_and_exp
	: equality_exp											{ $$ = $1; }
	| logical_and_exp '&' equality_exp						{ m_peecContainer->CreateOp( $1, $3, '&', $$ ); }
	;

logical_xor_exp
	: logical_and_exp										{ $$ = $1; }
	| logical_xor_exp '^' logical_and_exp					{ m_peecContainer->CreateOp( $1, $3, '^', $$ ); }
	;

logical_or_exp
	: logical_xor_exp										{ $$ = $1; }
	| logical_or_exp '|' logical_xor_exp					{ m_peecContainer->CreateOp( $1, $3, '|', $$ ); }
	;
	
and_exp
	: logical_or_exp										{ $$ = $1; }
	| and_exp EE_AND logical_or_exp							{ m_peecContainer->CreateOp( $1, $3, token::EE_AND, $$ ); }
	;

or_exp
	: and_exp 												{ $$ = $1; }
	| or_exp EE_OR and_exp									{ m_peecContainer->CreateOp( $1, $3, token::EE_OR, $$ ); }
	;

conditional_exp
	: or_exp												{ $$ = $1; }
	| or_exp '?' exp ':' conditional_exp					{ m_peecContainer->CreateConditional( $1, $3, $5, $$ ); }
	;

intrinsic
	: conditional_exp										{ $$ = $1; }
	| EE_COS '(' intrinsic ')'								{ m_peecContainer->CreateIntrinsic1( token::EE_COS, $3, $$ ); }
	| EE_SIN '(' intrinsic ')'								{ m_peecContainer->CreateIntrinsic1( token::EE_SIN, $3, $$ ); }
	| EE_TAN '(' intrinsic ')'								{ m_peecContainer->CreateIntrinsic1( token::EE_TAN, $3, $$ ); }
	| EE_ACOS '(' intrinsic ')'								{ m_peecContainer->CreateIntrinsic1( token::EE_ACOS, $3, $$ ); }
	| EE_ASIN '(' intrinsic ')'								{ m_peecContainer->CreateIntrinsic1( token::EE_ASIN, $3, $$ ); }
	| EE_ATAN '(' intrinsic ')'								{ m_peecContainer->CreateIntrinsic1( token::EE_ATAN, $3, $$ ); }
	| EE_ATAN2 '(' exp ',' intrinsic ')'					{ m_peecContainer->CreateIntrinsic2( token::EE_ATAN2, $3, $5, $$ ); }
	| EE_COSH '(' intrinsic ')'								{ m_peecContainer->CreateIntrinsic1( token::EE_COSH, $3, $$ ); }
	;

exp
	: intrinsic												{ $$ = $1; }
	;


translation_unit
	: exp													{ m_peecContainer->SetTrans( $1 ); }
	;


%%

int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, ee::CExpEvalLexer * _peelLexer ) {
	return _peelLexer->yylex();
}

void yy::parser::error( const yy::location &/*_lLoc*/, const std::string &/*_strM*/ ) {

}
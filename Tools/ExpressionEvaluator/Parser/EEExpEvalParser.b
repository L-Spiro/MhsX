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
%token EE_MEMBERACCESS EE_USER_VAR
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
%token EE_A EE_ALLADI EE_ALPHA EE_B EE_B2 EE_B4 EE_BETA EE_BH EE_C2 EE_CAHEN EE_CATALAN EE_CONWAY EE_DELTA EE_E EE_ERDOS EE_EULER EE_F EE_GR EE_GWK EE_HALFPI EE_HSMC EE_ICE EE_K
%token EE_LAMBDA EE_LAPLACE EE_LEVY EE_M1 EE_MU EE_NIVEN EE_OMEGA EE_P2 EE_PI EE_PLASTIC EE_PORTER EE_PSI EE_RAMAN EE_RAMAMU EE_SIERP EE_THETA EE_VISW EE_Z3 EE_ZETA 

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
	| EE_PI													{ m_peecContainer->CreateDouble( 3.1415926535897932384626433832795, $$ ); }
	| EE_HALFPI												{ m_peecContainer->CreateDouble( 1.5707963267948966192313216916398, $$ ); }
	| EE_E													{ m_peecContainer->CreateDouble( 2.7182818284590452353602874713527, $$ ); }
	| EE_ZETA												{ m_peecContainer->CreateDouble( 1.202056903159594285399738161511449990764986292, $$ ); }
	| EE_GR													{ m_peecContainer->CreateDouble( 1.6180339887498948482045868343656381177203091798057628621, $$ ); }
	| EE_EULER												{ m_peecContainer->CreateDouble( 0.5772156649015328606065120900824024310421, $$ ); }
	| EE_K													{ m_peecContainer->CreateDouble( 2.685452001065306445309714835481795693820382293994462953051152345557218, $$ ); }
	| EE_A													{ m_peecContainer->CreateDouble( 1.2824271291006226368753425688697917277676889273250011920637400217404, $$ ); }
	| EE_M1													{ m_peecContainer->CreateDouble( 0.26149721284764278375542683860869585, $$ ); }
	| EE_B													{ m_peecContainer->CreateDouble( 0.28016949902386913303, $$ ); }
	| EE_GWK												{ m_peecContainer->CreateDouble( 0.30366300289873265859744812190155623, $$ ); }
	| EE_HSMC												{ m_peecContainer->CreateDouble( 0.35323637185499598454351655043268201, $$ ); }
	| EE_OMEGA												{ m_peecContainer->CreateDouble( 0.56714329040978387299996866221035554, $$ ); }
	| EE_MU													{ m_peecContainer->CreateDouble( 0.62432998854355087099293638310083724, $$ ); }
	| EE_CAHEN												{ m_peecContainer->CreateDouble( 0.6434105462, $$ ); }
	| EE_C2													{ m_peecContainer->CreateDouble( 0.66016181584686957392781211001455577, $$ ); }
	| EE_LAPLACE											{ m_peecContainer->CreateDouble( 0.66274341934918158097474209710925290, $$ ); }
	| EE_BETA												{ m_peecContainer->CreateDouble( 0.70258, $$ ); }
	| EE_RAMAN												{ m_peecContainer->CreateDouble( 0.76422365358922066299069873125009232, $$ ); }
	| EE_ALLADI												{ m_peecContainer->CreateDouble( 0.8093940205, $$ ); }
	| EE_B4													{ m_peecContainer->CreateDouble( 0.87058838, $$ ); }
	| EE_CATALAN											{ m_peecContainer->CreateDouble( 0.91596559417721901505460351493238411, $$ ); }
	| EE_LAMBDA												{ m_peecContainer->CreateDouble( 1.0986858055, $$ ); }
	| EE_VISW												{ m_peecContainer->CreateDouble( 1.13198824, $$ ); }
	| EE_Z3													{ m_peecContainer->CreateDouble( 1.20205690315959428539973816151144999, $$ ); }
	| EE_CONWAY												{ m_peecContainer->CreateDouble( 1.30357726903429639125709911215255189, $$ ); }
	| EE_THETA												{ m_peecContainer->CreateDouble( 1.30637788386308069046861449260260571, $$ ); }
	| EE_PLASTIC											{ m_peecContainer->CreateDouble( 1.32471795724474602596090885447809734, $$ ); }
	| EE_RAMAMU												{ m_peecContainer->CreateDouble( 1.45136923488338105028396848589202744, $$ ); }
	| EE_BH													{ m_peecContainer->CreateDouble( 1.45607494858268967139959535111654356, $$ ); }
	| EE_PORTER												{ m_peecContainer->CreateDouble( 1.4670780794, $$ ); }
	| EE_ICE												{ m_peecContainer->CreateDouble( 1.5396007178, $$ ); }
	| EE_ERDOS												{ m_peecContainer->CreateDouble( 1.60669515241529176378330152319092458, $$ ); }
	| EE_NIVEN												{ m_peecContainer->CreateDouble( 1.70521114010536776428855145343450816, $$ ); }
	| EE_B2													{ m_peecContainer->CreateDouble( 1.902160583104, $$ ); }
	| EE_P2													{ m_peecContainer->CreateDouble( 2.29558714939263807403429804918949039, $$ ); }
	| EE_ALPHA												{ m_peecContainer->CreateDouble( 2.50290787509589282228390287321821578, $$ ); }
	| EE_SIERP												{ m_peecContainer->CreateDouble( 2.58498175957925321706589358738317116, $$ ); }
	| EE_F													{ m_peecContainer->CreateDouble( 2.80777024202851936522150118655777293, $$ ); }
	| EE_LEVY												{ m_peecContainer->CreateDouble( 3.27582291872181115978768188245384386, $$ ); }
	| EE_PSI												{ m_peecContainer->CreateDouble( 3.35988566624317755317201130291892717, $$ ); }
	| EE_DELTA												{ m_peecContainer->CreateDouble( 4.66920160910299067185320382046620161, $$ ); }
	| '(' exp ')'											{ $$ = $2; }
	| '[' exp ']'											{ m_peecContainer->CreateAddress( $2, EE_CT_UINT32, $$ ); }
	| EE_OB_DWORD exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_UINT32, $$ ); }
	| EE_OB_BYTE exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_UINT8, $$ ); }
	| EE_OB_WORD exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_UINT16, $$ ); }
	| EE_OB_QWORD exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_UINT64, $$ ); }
	| EE_OB_FLOAT exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_FLOAT, $$ ); }
	| EE_OB_DOUBLE exp ']'									{ m_peecContainer->CreateAddress( $2, EE_CT_DOUBLE, $$ ); }
	| EE_USER_VAR											{ m_peecContainer->CreateUser( $$ ); }
	| intrinsic												{ $$ = $1; }
	;

postfix_exp
	: basic_expr											{ $$ = $1; }
	| postfix_exp EE_MEMBERACCESS EE_IDENTIFIER				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_STRING_CONSTANT		{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateString( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_HEX_CONSTANT3			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_E						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_PI						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_HALFPI					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_ZETA					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_GR						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_EULER					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_K						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_A						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_M1						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_B						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_GWK					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_HSMC					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_OMEGA					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_MU						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_CAHEN					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_C2						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LAPLACE				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_BETA					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_RAMAN					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_ALLADI					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_B4						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_CATALAN				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LAMBDA					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_VISW					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_Z3						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_CONWAY					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_THETA					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_PLASTIC				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_RAMAMU					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_BH						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_PORTER					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_ICE					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_ERDOS					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_NIVEN					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_B2						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_P2						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_ALPHA					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_SIERP					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_F						{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LEVY					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_PSI					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DELTA					{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
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
	: EE_COS '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_COS, $3, $$ ); }
	| EE_SIN '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_SIN, $3, $$ ); }
	| EE_TAN '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_TAN, $3, $$ ); }
	| EE_ACOS '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ACOS, $3, $$ ); }
	| EE_ASIN '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ASIN, $3, $$ ); }
	| EE_ATAN '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ATAN, $3, $$ ); }
	| EE_ATAN2 '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_ATAN2, $3, $5, $$ ); }
	| EE_COSH '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_COSH, $3, $$ ); }
	| EE_SINH '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_SINH, $3, $$ ); }
	| EE_TANH '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_TANH, $3, $$ ); }
	| EE_ACOSH '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ACOSH, $3, $$ ); }
	| EE_ASINH '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ASINH, $3, $$ ); }
	| EE_ATANH '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ATANH, $3, $$ ); }
	| EE_EXP '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_EXP, $3, $$ ); }
	| EE_LOG '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_LOG, $3, $$ ); }
	| EE_LOG10 '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_LOG10, $3, $$ ); }
	| EE_LOG2 '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_LOG2, $3, $$ ); }
	| EE_EXP2 '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_EXP2, $3, $$ ); }
	| EE_EXPM1 '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_EXPM1, $3, $$ ); }
	| EE_ILOGB '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ILOGB, $3, $$ ); }
	| EE_LOG1P '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_LOG1P, $3, $$ ); }
	| EE_LOGB '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_LOGB, $3, $$ ); }
	| EE_POW '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_POW, $3, $5, $$ ); }
	| EE_SQRT '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_SQRT, $3, $$ ); }
	| EE_CBRT '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_CBRT, $3, $$ ); }
	| EE_HYPOT '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_HYPOT, $3, $5, $$ ); }
	| EE_TGAMMA '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_TGAMMA, $3, $$ ); }
	| EE_LGAMMA '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_LGAMMA, $3, $$ ); }
	| EE_CEIL '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_CEIL, $3, $$ ); }
	| EE_FLOOR '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_FLOOR, $3, $$ ); }
	| EE_MOD '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_MOD, $3, $5, $$ ); }
	| EE_TRUNC '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_TRUNC, $3, $$ ); }
	| EE_ROUND '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ROUND, $3, $$ ); }
	| EE_NEARBYINT '(' exp ')'								{ m_peecContainer->CreateIntrinsic1( token::EE_NEARBYINT, $3, $$ ); }
	| EE_REMAINDER '(' exp ',' exp ')'						{ m_peecContainer->CreateIntrinsic2( token::EE_REMAINDER, $3, $5, $$ ); }
	| EE_NEXTAFTER '(' exp ',' exp ')'						{ m_peecContainer->CreateIntrinsic2( token::EE_NEXTAFTER, $3, $5, $$ ); }
	| EE_NEXTTOWARD '(' exp ',' exp ')'						{ m_peecContainer->CreateIntrinsic2( token::EE_NEXTTOWARD, $3, $5, $$ ); }
	| EE_DIM '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_DIM, $3, $5, $$ ); }
	| EE_MAX '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_MAX, $3, $5, $$ ); }
	| EE_MIN '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_MIN, $3, $5, $$ ); }
	| EE_ABS '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ABS, $3, $$ ); }
	| EE_MADD '(' exp ',' exp ',' exp ')'					{ m_peecContainer->CreateIntrinsic3( token::EE_MADD, $3, $5, $7, $$ ); }
	;

exp
	: conditional_exp										{ $$ = $1; }
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
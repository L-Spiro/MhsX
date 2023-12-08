%{


#include "../EEExpEval.h"
#include "../EEExpEvalContainer.h"
#include "../EEExpEvalLexer.h"
#include <cstdio>
#include <ctime>

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

#ifndef LDBL_DECIMAL_DIG
#define LDBL_DECIMAL_DIG DBL_DECIMAL_DIG
#endif	// #ifndef LDBL_DECIMAL_DIG


// Announce to Flex the prototype we want for lexing function.
extern int yylex( /*YYSTYPE*/void * _pvNodeUnion, ee::CExpEvalLexer * _peelLexer );


#include "../EEExpEvalSyntaxNodes.h"


%}


%pure-parser

%parse-param												{ class CExpEvalLexer * m_peelLexer }
%parse-param												{ class CExpEvalContainer * m_peecContainer }
%lex-param													{ CExpEvalLexer * m_peelLexer }

%token EE_DO EE_ELSE EE_FOR EE_FOREACH EE_IF EE_IN EE_WHILE
%token EE_BREAK EE_CONTINUE
%token EE_NEW EE_COPY
%token EE_ANY EE_DEFAULT EE_DOUBLE EE_FLOAT EE_FLOAT10 EE_FLOAT11 EE_FLOAT14 EE_FLOAT16 EE_INT8 EE_INT16 EE_INT32 EE_INT64 EE_OBJECT EE_PERSISTENT EE_TEMP EE_UINT8 EE_UINT16 EE_UINT32 EE_UINT64

%token EE_CONST
%token EE_CHAR_CONSTANT EE_FLOAT_CONSTANT EE_HEX_CONSTANT1 EE_HEX_CONSTANT2 EE_HEX_CONSTANT3 EE_STRING_CONSTANT EE_UNICODE_CONSTANT
%token EE_BIN_CONSTANT EE_DEC_CONSTANT EE_OCT_CONSTANT EE_PUREDEC_CONSTANT
%token EE_IDENTIFIER
%token EE_ARRAY EE_CUSTOM_VAR EE_MEMBERACCESS EE_PARM EE_PARM_TOTAL EE_USER_VAR
%token EE_EQU_E EE_EQU_NE EE_LEFT_OP EE_RIGHT_OP EE_REL_GE EE_REL_LE
%token EE_PLUSPLUS EE_MINUSMINUS
%token EE_ASS_PLUSEQUALS EE_ASS_MINUSEQUALS EE_ASS_TIMESEQUALS EE_ASS_MODEQUALS EE_ASS_DIVEQUALS EE_ASS_CARROTEQUALS EE_ASS_SHLEFTEQUALS EE_ASS_SHRIGHTEQUALS EE_ASS_OREQUALS EE_ASS_ANDEQUALS
%token EE_OB_BYTE EE_OB_SBYTE EE_OB_WORD EE_OB_SWORD EE_OB_QWORD EE_OB_SQWORD EE_OB_FLOAT EE_OB_DOUBLE EE_OB_DWORD EE_OB_SDWORD EE_OB_FLOAT16
%token EE_COS EE_SIN EE_TAN EE_ACOS EE_ASIN EE_ATAN EE_ATAN2
%token EE_COSH EE_SINH EE_TANH EE_ACOSH EE_ASINH EE_ATANH
%token EE_EPSILON EE_RELEPSILON EE_EPSILONF EE_RELEPSILONF
%token EE_EXP EE_LOG EE_LOG10 EE_LOG2 EE_EXP2 EE_EXPM1 EE_ILOGB EE_LOG1P EE_LOGB
%token EE_POW EE_SQRT EE_CBRT EE_HYPOT EE_TGAMMA EE_LGAMMA
%token EE_CEIL EE_FLOOR EE_MOD EE_TRUNC EE_ROUND EE_NEARBYINT EE_REMAINDER EE_REMQUO
%token EE_NEXTAFTER EE_NEXTAFTERF EE_NEXTTOWARD EE_NEXTTOWARDF
%token EE_DIM EE_MAX EE_MIN
%token EE_DEG EE_RAD
%token EE_ABS EE_MADD EE_RAND
%token EE_ISNAN EE_ISINF
%token EE_BYTESWAPUSHORT EE_BYTESWAPULONG EE_BYTESWAPUINT64
%token EE_FORMAT
%token EE_A EE_ALLADI EE_ALPHA EE_B EE_B2 EE_B4 EE_BETA EE_BH EE_C2 EE_CAHEN EE_CATALAN EE_CONWAY EE_DELTA EE_E EE_ERDOS EE_EULER EE_F EE_GR EE_GWK EE_HALFPI EE_HSMC EE_ICE EE_K
%token EE_LAMBDA EE_LAPLACE EE_LEVY EE_M1 EE_MU EE_NIVEN EE_OMEGA EE_P2 EE_PI EE_PLASTIC EE_PORTER EE_PSI EE_RAMAN EE_RAMAMU EE_SIERP EE_THETA EE_VISW EE_Z3 EE_ZETA 

%token EE_CHAR_BIT EE_MB_LEN_MAX EE_CHAR_MIN EE_CHAR_MAX EE_SCHAR_MIN EE_SHRT_MIN EE_INT_MIN EE_LONG_MIN EE_LLONG_MIN EE_SCHAR_MAX EE_SHRT_MAX EE_INT_MAX EE_LONG_MAX EE_LLONG_MAX EE_UCHAR_MAX EE_USHRT_MAX EE_UINT_MAX EE_ULONG_MAX EE_ULLONG_MAX EE_FLT_RADIX EE_DECIMAL_DIG EE_FLT_DECIMAL_DIG EE_DBL_DECIMAL_DIG EE_LDBL_DECIMAL_DIG EE_FLT_MIN EE_DBL_MIN EE_LDBL_MIN EE_FLT_TRUE_MIN EE_DBL_TRUE_MIN EE_LDBL_TRUE_MIN EE_FLT_MAX EE_DBL_MAX EE_LDBL_MAX EE_FLT_EPSILON EE_DBL_EPSILON EE_LDBL_EPSILON EE_FLT_DIG EE_DBL_DIG EE_LDBL_DIG EE_FLT_MANT_DIG EE_DBL_MANT_DIG EE_LDBL_MANT_DIG EE_FLT_MIN_EXP EE_DBL_MIN_EXP EE_LDBL_MIN_EXP EE_FLT_MIN_10_EXP EE_DBL_MIN_10_EXP EE_LDBL_MIN_10_EXP EE_FLT_MAX_EXP EE_DBL_MAX_EXP EE_LDBL_MAX_EXP EE_FLT_MAX_10_EXP EE_DBL_MAX_10_EXP EE_LDBL_MAX_10_EXP EE_INT8_MIN EE_INT16_MIN EE_INT32_MIN EE_INT64_MIN EE_INT8_MAX EE_INT16_MAX EE_INT32_MAX EE_INT64_MAX EE_UINT8_MAX EE_UINT16_MAX EE_UINT32_MAX EE_UINT64_MAX  
%token EE_AS_FLOAT EE_AS_DOUBLE EE_AS_FLOAT24 EE_AS_FLOAT16 EE_AS_FLOAT14 EE_AS_FLOAT11 EE_AS_FLOAT10
%token EE_AS_FLOAT_MAX EE_AS_FLOAT_MIN EE_AS_FLOAT_TRUE_MIN EE_AS_FLOAT_NAN EE_AS_FLOAT_INF EE_AS_FLOAT_SUBNORM_MAX EE_AS_FLOAT_EPS
%token EE_TRUE EE_FALSE
%token EE_AS_FLOAT_SIGNBIT EE_AS_FLOAT_EXPBITS EE_AS_FLOAT_MANBITS
%token EE_TO_FLOAT32 EE_TO_FLOAT64 EE_TO_INT8 EE_TO_INT16 EE_TO_INT32 EE_TO_INT64 EE_TO_UINT8 EE_TO_UINT16 EE_TO_UINT32 EE_TO_UINT64
%token EE_CLOCK EE_CLOCKS_PER_SEC EE_SECONDS EE_MILLISECONDS EE_MICROSECONDS EE_SECONDS_SINCE_START EE_MILLISECONDS_SINCE_START EE_MICROSECONDS_SINCE_START

%token EE_ASCII EE_BIN EE_BOOL EE_CHR EE_HEX EE_INT EE_LEN EE_OCT EE_ORD

%type <sStringIndex>										identifier
%type <sStringIndex>										string
%type <sStringIndex>										custom_var
%type <sStringIndex>										array_var
%type <ndData>												basic_expr
%type <ndData>												postfix_exp
%type <ndData>												unary_exp
%type <ndData>												cast_exp
%type <ui32Unary>											unary_operator
%type <ui32Backing>											backing_type
%type <ui32Backing>											backing_persistence
%type <ui32Backing>											cast_type
%type <ui32Backing>											address_type
%type <ndData.v.ctCast>										assignment_op
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
%type <ndData>												assignment_exp
%type <ndData>												argument_exp_entry
%type <ndData>												argument_exp_list
%type <ndData>												intrinsic
%type <ndData>												exp
%type <ndData>												exp_statement
%type <ndData>												statement
%type <ndData>												compound_statement
%type <ndData>												statement_list
%type <ndData>												foreach_decl0
%type <ndData>												foreach_decl1
%type <ndData>												foreach_decl2
%type <ndData>												iteration_exp
%type <ndData>												selection_exp
%type <ndData>												jump_exp
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

custom_var
	: EE_CUSTOM_VAR											{ $$ = m_peecContainer->HasCustomVar( m_peelLexer->YYText() ); }
	;
	
array_var
	: EE_ARRAY												{ $$ = m_peecContainer->HasArray( m_peelLexer->YYText() ); }
	;
	
identifier
	: EE_IDENTIFIER											{ $$ = m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ); }
	;
	
string
	: EE_STRING_CONSTANT									{ $$ = m_peecContainer->CreateString( m_peelLexer->get_tmp_txt() ); }
	;
	
basic_expr
	: string												{ m_peecContainer->CreateStringBasicExp( $1, $$ ); }
	| custom_var											{ m_peecContainer->CreateCustomVar( $1, $$ ); }
	| EE_USER_VAR											{ m_peecContainer->CreateUser( $$ ); }
	| EE_PARM												{ m_peecContainer->CreateNumberedParm( m_peelLexer->YYText(), $$ ); }
	| '$' '[' custom_var ']'								{ m_peecContainer->CreateDynamicParm( $3, $$ ); }
	| array_var '[' exp ']'									{ m_peecContainer->CreateArrayVar( $1, $3, $$ ); }
	| string '[' exp ']'									{ m_peecContainer->CreateArrayString( $1, $3, $$ ); }
	| string '[' exp ':' exp ']'							{ m_peecContainer->CreateArrayString( $1, $3, $5, $$ ); }
	| string '[' exp ':' ']'								{ m_peecContainer->CreateArrayToEndString( $1, $3, $$ ); }
	| string '[' ':' exp ']'								{ m_peecContainer->CreateArrayFromStartString( $1, $4, $$ ); }
	| '$' '[' exp ']'										{ m_peecContainer->CreateDynamicParmExp( $3, $$ ); }
	| EE_PARM_TOTAL											{ m_peecContainer->CreateParmTotal( $$ ); }
	//| basic_expr EE_PLUSPLUS								{ m_peecContainer->CreatePostfixOp( $1, CExpEvalParser::token::EE_PLUSPLUS, $$ ); }
	//| basic_expr EE_MINUSMINUS								{ m_peecContainer->CreatePostfixOp( $1, CExpEvalParser::token::EE_MINUSMINUS, $$ ); }
	
	| EE_UNICODE_CONSTANT									{ m_peecContainer->CreateUnicodeNumericConstant( m_peelLexer->YYText(), $$ ); }
	| EE_HEX_CONSTANT1										{ m_peecContainer->CreateHex1( m_peelLexer->YYText(), $$ ); }
	| EE_HEX_CONSTANT2										{ m_peecContainer->CreateHex2( m_peelLexer->YYText(), $$ ); }
	| EE_HEX_CONSTANT3										{ m_peecContainer->CreateHex3( m_peelLexer->YYText(), $$ ); }
	| EE_BIN_CONSTANT										{ m_peecContainer->CreateBin( m_peelLexer->YYText(), $$ ); }
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
	| EE_CHAR_CONSTANT										{ m_peecContainer->CreateNumber( m_peelLexer->get_tmp_txt()[0], $$ ); }
	| EE_FLOAT_CONSTANT										{
																if ( m_peecContainer->TreatAllAsHex() && ee::CExpEval::ValidHex( m_peelLexer->YYText() ) ) {
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
	| EE_CHAR_BIT											{ m_peecContainer->CreateNumber( CHAR_BIT, $$ ); }
	| EE_MB_LEN_MAX											{ m_peecContainer->CreateNumber( MB_LEN_MAX, $$ ); }
	| EE_CHAR_MIN											{ m_peecContainer->CreateNumber( CHAR_MIN, $$ ); }
	| EE_CHAR_MAX											{ m_peecContainer->CreateNumber( CHAR_MAX, $$ ); }
	| EE_SCHAR_MIN											{ m_peecContainer->CreateNumber( SCHAR_MIN, $$ ); }
	| EE_SHRT_MIN											{ m_peecContainer->CreateNumber( SHRT_MIN, $$ ); }
	| EE_INT_MIN											{ m_peecContainer->CreateNumber( INT_MIN, $$ ); }
	| EE_LONG_MIN											{ m_peecContainer->CreateNumber( LONG_MIN, $$ ); }
	| EE_LLONG_MIN											{ m_peecContainer->CreateNumber( LLONG_MIN, $$ ); }
	| EE_SCHAR_MAX											{ m_peecContainer->CreateNumber( SCHAR_MAX, $$ ); }
	| EE_SHRT_MAX											{ m_peecContainer->CreateNumber( SHRT_MAX, $$ ); }
	| EE_INT_MAX											{ m_peecContainer->CreateNumber( INT_MAX, $$ ); }
	| EE_LONG_MAX											{ m_peecContainer->CreateNumber( LONG_MAX, $$ ); }
	| EE_LLONG_MAX											{ m_peecContainer->CreateNumber( LLONG_MAX, $$ ); }
	| EE_UCHAR_MAX											{ m_peecContainer->CreateNumber( UCHAR_MAX, $$ ); }
	| EE_USHRT_MAX											{ m_peecContainer->CreateNumber( USHRT_MAX, $$ ); }
	| EE_UINT_MAX											{ m_peecContainer->CreateNumber( UINT_MAX, $$ ); }
	| EE_ULONG_MAX											{ m_peecContainer->CreateNumber( static_cast<uint64_t>(ULONG_MAX), $$ ); }
	| EE_ULLONG_MAX											{ m_peecContainer->CreateNumber( ULLONG_MAX, $$ ); }
	| EE_FLT_RADIX											{ m_peecContainer->CreateNumber( FLT_RADIX, $$ ); }
	| EE_DECIMAL_DIG										{ m_peecContainer->CreateNumber( DECIMAL_DIG, $$ ); }
	| EE_FLT_DECIMAL_DIG									{ m_peecContainer->CreateNumber( FLT_DECIMAL_DIG, $$ ); }
	| EE_DBL_DECIMAL_DIG									{ m_peecContainer->CreateNumber( DBL_DECIMAL_DIG, $$ ); }
	| EE_LDBL_DECIMAL_DIG									{ m_peecContainer->CreateNumber( LDBL_DECIMAL_DIG, $$ ); }
	| EE_FLT_MIN											{ m_peecContainer->CreateNumber( FLT_MIN, $$ ); }
	| EE_DBL_MIN											{ m_peecContainer->CreateNumber( DBL_MIN, $$ ); }
	| EE_LDBL_MIN											{ m_peecContainer->CreateNumber( LDBL_MIN, $$ ); }
	| EE_FLT_TRUE_MIN										{ m_peecContainer->CreateNumber( FLT_TRUE_MIN, $$ ); }
	| EE_DBL_TRUE_MIN										{ m_peecContainer->CreateNumber( DBL_TRUE_MIN, $$ ); }
	| EE_LDBL_TRUE_MIN										{ m_peecContainer->CreateNumber( LDBL_TRUE_MIN, $$ ); }
	| EE_FLT_MAX											{ m_peecContainer->CreateNumber( FLT_MAX, $$ ); }
	| EE_DBL_MAX											{ m_peecContainer->CreateNumber( DBL_MAX, $$ ); }
	| EE_LDBL_MAX											{ m_peecContainer->CreateNumber( LDBL_MAX, $$ ); }
	| EE_FLT_EPSILON										{ m_peecContainer->CreateNumber( FLT_EPSILON, $$ ); }
	| EE_DBL_EPSILON										{ m_peecContainer->CreateNumber( DBL_EPSILON, $$ ); }
	| EE_LDBL_EPSILON										{ m_peecContainer->CreateNumber( LDBL_EPSILON, $$ ); }
	| EE_FLT_DIG											{ m_peecContainer->CreateNumber( FLT_DIG, $$ ); }
	| EE_DBL_DIG											{ m_peecContainer->CreateNumber( DBL_DIG, $$ ); }
	| EE_LDBL_DIG											{ m_peecContainer->CreateNumber( LDBL_DIG, $$ ); }
	| EE_FLT_MANT_DIG										{ m_peecContainer->CreateNumber( FLT_MANT_DIG, $$ ); }
	| EE_DBL_MANT_DIG										{ m_peecContainer->CreateNumber( DBL_MANT_DIG, $$ ); }
	| EE_LDBL_MANT_DIG										{ m_peecContainer->CreateNumber( LDBL_MANT_DIG, $$ ); }
	| EE_FLT_MIN_EXP										{ m_peecContainer->CreateNumber( FLT_MIN_EXP, $$ ); }
	| EE_DBL_MIN_EXP										{ m_peecContainer->CreateNumber( DBL_MIN_EXP, $$ ); }
	| EE_LDBL_MIN_EXP										{ m_peecContainer->CreateNumber( LDBL_MIN_EXP, $$ ); }
	| EE_FLT_MIN_10_EXP										{ m_peecContainer->CreateNumber( FLT_MIN_10_EXP, $$ ); }
	| EE_DBL_MIN_10_EXP										{ m_peecContainer->CreateNumber( DBL_MIN_10_EXP, $$ ); }
	| EE_LDBL_MIN_10_EXP									{ m_peecContainer->CreateNumber( LDBL_MIN_10_EXP, $$ ); }
	| EE_FLT_MAX_EXP										{ m_peecContainer->CreateNumber( FLT_MAX_EXP, $$ ); }
	| EE_DBL_MAX_EXP										{ m_peecContainer->CreateNumber( DBL_MAX_EXP, $$ ); }
	| EE_LDBL_MAX_EXP										{ m_peecContainer->CreateNumber( LDBL_MAX_EXP, $$ ); }
	| EE_FLT_MAX_10_EXP										{ m_peecContainer->CreateNumber( FLT_MAX_10_EXP, $$ ); }
	| EE_DBL_MAX_10_EXP										{ m_peecContainer->CreateNumber( DBL_MAX_10_EXP, $$ ); }
	| EE_LDBL_MAX_10_EXP									{ m_peecContainer->CreateNumber( LDBL_MAX_10_EXP, $$ ); }
	| EE_INT8_MIN											{ m_peecContainer->CreateNumber( INT8_MIN, $$ ); }
	| EE_INT16_MIN											{ m_peecContainer->CreateNumber( INT16_MIN, $$ ); }
	| EE_INT32_MIN											{ m_peecContainer->CreateNumber( INT32_MIN, $$ ); }
	| EE_INT64_MIN											{ m_peecContainer->CreateNumber( INT64_MIN, $$ ); }
	| EE_INT8_MAX											{ m_peecContainer->CreateNumber( INT8_MAX, $$ ); }
	| EE_INT16_MAX											{ m_peecContainer->CreateNumber( INT16_MAX, $$ ); }
	| EE_INT32_MAX											{ m_peecContainer->CreateNumber( INT32_MAX, $$ ); }
	| EE_INT64_MAX											{ m_peecContainer->CreateNumber( INT64_MAX, $$ ); }
	| EE_UINT8_MAX											{ m_peecContainer->CreateNumber( UINT8_MAX, $$ ); }
	| EE_UINT16_MAX											{ m_peecContainer->CreateNumber( UINT16_MAX, $$ ); }
	| EE_UINT32_MAX											{ m_peecContainer->CreateNumber( UINT32_MAX, $$ ); }
	| EE_UINT64_MAX											{ m_peecContainer->CreateNumber( UINT64_MAX, $$ ); }
	| EE_CLOCKS_PER_SEC										{ m_peecContainer->CreateNumber( CLOCKS_PER_SEC, $$ ); }
	| EE_TRUE												{ m_peecContainer->CreateNumber( 1, $$ ); }
	| EE_FALSE												{ m_peecContainer->CreateNumber( 0, $$ ); }
	| '(' exp ')'											{ $$ = $2; }
	//| '[' exp ']'											{ m_peecContainer->CreateAddress( $2, EE_CT_UINT32, $$ ); }
	| address_type exp ']'									{ m_peecContainer->CreateAddress( $2, static_cast<ee::EE_CAST_TYPES>($1), $$ ); }
	| intrinsic												{ $$ = $1; }
	;

postfix_exp
	: basic_expr											{ $$ = $1; }
	| custom_var EE_PLUSPLUS								{ m_peecContainer->CreatePostfixOp( $1, CExpEvalParser::token::EE_PLUSPLUS, $$ ); }
	| custom_var EE_MINUSMINUS								{ m_peecContainer->CreatePostfixOp( $1, CExpEvalParser::token::EE_MINUSMINUS, $$ ); }
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
	| postfix_exp EE_MEMBERACCESS EE_CHAR_BIT				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_MB_LEN_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_CHAR_MIN				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_CHAR_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_SCHAR_MIN				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_SHRT_MIN				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_INT_MIN				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LONG_MIN				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LLONG_MIN				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_SCHAR_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_SHRT_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_INT_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LONG_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LLONG_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_UCHAR_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_USHRT_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_UINT_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_ULONG_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_ULLONG_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_RADIX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DECIMAL_DIG			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_DECIMAL_DIG		{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DBL_DECIMAL_DIG		{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LDBL_DECIMAL_DIG		{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_MIN				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DBL_MIN				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LDBL_MIN				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_TRUE_MIN			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DBL_TRUE_MIN			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LDBL_TRUE_MIN			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DBL_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LDBL_MAX				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_EPSILON			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DBL_EPSILON			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LDBL_EPSILON			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_DIG				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DBL_DIG				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LDBL_DIG				{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_MANT_DIG			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DBL_MANT_DIG			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LDBL_MANT_DIG			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_MIN_EXP			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DBL_MIN_EXP			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LDBL_MIN_EXP			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_MIN_10_EXP			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DBL_MIN_10_EXP			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LDBL_MIN_10_EXP		{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_MAX_EXP			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DBL_MAX_EXP			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LDBL_MAX_EXP			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_FLT_MAX_10_EXP			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_DBL_MAX_10_EXP			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_LDBL_MAX_10_EXP		{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp EE_MEMBERACCESS EE_CLOCKS_PER_SEC			{ m_peecContainer->CreateMemberAccess( $1, m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), $$ ); }
	| postfix_exp '[' exp ']'								{ m_peecContainer->CreateArrayAccess( $1, $3, $$ ); }
	| postfix_exp '[' exp ':' exp ']'						{ m_peecContainer->CreateArrayAccessEx( $1, $3.sNodeIndex, $5.sNodeIndex, $$ ); }
	| postfix_exp '[' exp ':' ']'							{ m_peecContainer->CreateArrayAccessEx( $1, $3.sNodeIndex, size_t( -1 ), $$ ); }
	| postfix_exp '[' ':' exp ']'							{ m_peecContainer->CreateArrayAccessEx( $1, size_t( -1 ), $4.sNodeIndex, $$ ); }
	;

unary_exp
	: postfix_exp											{ $$ = $1; }
	| EE_PLUSPLUS custom_var								{ m_peecContainer->CreatePrefixOp( $2, CExpEvalParser::token::EE_PLUSPLUS, $$ ); }
	| EE_MINUSMINUS custom_var								{ m_peecContainer->CreatePrefixOp( $2, CExpEvalParser::token::EE_MINUSMINUS, $$ ); }
	| unary_operator unary_exp								{ m_peecContainer->CreateUnary( $2, $1, $$ ); }
	;
	
cast_exp
	: unary_exp												{ $$ = $1; }
	| '(' cast_type ')' cast_exp							{ m_peecContainer->CreateCast( $4, static_cast<ee::EE_CAST_TYPES>($2), $$ ); }
	;

unary_operator
	: '+'	%prec  EE_UPLUS									{ $$ = '+'; }
	| '-'	%prec  EE_UMINUS								{ $$ = '-'; }
	| '~'	%prec  EE_UNARY_1SCOMPL							{ $$ = '~'; }
	| '!'	%prec  EE_LOG_NOT								{ $$ = '!'; }
	;

multiplicative_exp
	: cast_exp												{ $$ = $1; }
	| multiplicative_exp '*' cast_exp						{ m_peecContainer->CreateOp( $1, $3, '*', $$ ); }
	| multiplicative_exp '/' cast_exp						{ m_peecContainer->CreateOp( $1, $3, '/', $$ ); }
	| multiplicative_exp '%' cast_exp						{ m_peecContainer->CreateOp( $1, $3, '%', $$ ); }
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
	| conditional_exp '?' exp ':' or_exp					{ m_peecContainer->CreateConditional( $1, $3, $5, $$ ); }
	;
	
assignment_op
	: '='													{ $$ = static_cast<ee::EE_CAST_TYPES>('='); }
	| EE_ASS_PLUSEQUALS										{ $$ = static_cast<ee::EE_CAST_TYPES>(CExpEvalParser::token::EE_ASS_PLUSEQUALS); }
	| EE_ASS_MINUSEQUALS									{ $$ = static_cast<ee::EE_CAST_TYPES>(CExpEvalParser::token::EE_ASS_MINUSEQUALS); }
	| EE_ASS_TIMESEQUALS									{ $$ = static_cast<ee::EE_CAST_TYPES>(CExpEvalParser::token::EE_ASS_TIMESEQUALS); }
	| EE_ASS_MODEQUALS										{ $$ = static_cast<ee::EE_CAST_TYPES>(CExpEvalParser::token::EE_ASS_MODEQUALS); }
	| EE_ASS_DIVEQUALS										{ $$ = static_cast<ee::EE_CAST_TYPES>(CExpEvalParser::token::EE_ASS_DIVEQUALS); }
	| EE_ASS_CARROTEQUALS									{ $$ = static_cast<ee::EE_CAST_TYPES>(CExpEvalParser::token::EE_ASS_CARROTEQUALS); }
	| EE_ASS_SHLEFTEQUALS									{ $$ = static_cast<ee::EE_CAST_TYPES>(CExpEvalParser::token::EE_ASS_SHLEFTEQUALS); }
	| EE_ASS_SHRIGHTEQUALS									{ $$ = static_cast<ee::EE_CAST_TYPES>(CExpEvalParser::token::EE_ASS_SHRIGHTEQUALS); }
	| EE_ASS_OREQUALS										{ $$ = static_cast<ee::EE_CAST_TYPES>(CExpEvalParser::token::EE_ASS_OREQUALS); }
	| EE_ASS_ANDEQUALS										{ $$ = static_cast<ee::EE_CAST_TYPES>(CExpEvalParser::token::EE_ASS_ANDEQUALS); }
	;
	
assignment_exp
	: conditional_exp										{ $$ = $1; }
	| custom_var assignment_op assignment_exp				{ m_peecContainer->CreateReAssignment( $1, $3, $2, $$ ); }
	| identifier '=' EE_NEW backing_type '(' exp ')'
															{ m_peecContainer->CreateArray( $1, size_t( $4 ), static_cast<size_t>(token::EE_TEMP), $6, size_t( ~0 ), size_t( ~0 ), $$ ); }
	| identifier '=' EE_NEW backing_type '(' exp ',' backing_persistence ')'
															{ m_peecContainer->CreateArray( $1, size_t( $4 ), $8, $6, size_t( ~0 ), size_t( ~0 ), $$ ); }
	| identifier '=' EE_NEW backing_type '(' exp ',' backing_persistence ',' exp ')'
															{ m_peecContainer->CreateArray( $1, $4, $8, $6, $10.sNodeIndex, $10.sNodeIndex, $$ ); }
	| identifier '=' EE_NEW backing_type '(' exp ',' backing_persistence ',' exp ',' exp ')'
															{ m_peecContainer->CreateArray( $1, $4, $8, $6, $10.sNodeIndex, $12.sNodeIndex, $$ ); }
	| identifier '=' EE_NEW '(' exp ')'
															{ m_peecContainer->CreateArray( $1, static_cast<size_t>(CExpEvalParser::token::EE_DEFAULT), static_cast<size_t>(token::EE_TEMP), $5, size_t( ~0 ), size_t( ~0 ), $$ ); }
	| identifier '=' EE_NEW '(' exp ',' backing_persistence ')'
															{ m_peecContainer->CreateArray( $1, static_cast<size_t>(CExpEvalParser::token::EE_DEFAULT), $7, $5, size_t( ~0 ), size_t( ~0 ), $$ ); }
	| identifier '=' EE_NEW '(' exp ',' backing_persistence ',' exp ')'
															{ m_peecContainer->CreateArray( $1, static_cast<size_t>(CExpEvalParser::token::EE_DEFAULT), $7, $5, $9.sNodeIndex, $9.sNodeIndex, $$ ); }
	| identifier '=' EE_NEW '(' exp ',' backing_persistence ',' exp ',' exp ')'
															{ m_peecContainer->CreateArray( $1, static_cast<size_t>(CExpEvalParser::token::EE_DEFAULT), $7, $5, $9.sNodeIndex, $11.sNodeIndex, $$ ); }
	| identifier '=' assignment_exp							{ m_peecContainer->CreateAssignment( $1, $3, '=', false, $$ ); }
	| EE_CONST identifier '=' assignment_exp				{ m_peecContainer->CreateAssignment( $2, $4, '=', true, $$ ); }
	| array_var '[' exp ']' assignment_op assignment_exp	{ m_peecContainer->CreateArrayReAssignment( $1, $3, $6, $5, $$ ); }
	| address_type exp ']' assignment_op assignment_exp		{ m_peecContainer->CreateAddressAssignment( static_cast<ee::EE_CAST_TYPES>($1), $2, $5, $4, $$ ); }
	;
	
address_type
	: EE_OB_DWORD											{ $$ = ee::EE_CT_UINT32; }
	| EE_OB_BYTE											{ $$ = ee::EE_CT_UINT8; }
	| EE_OB_WORD											{ $$ = ee::EE_CT_UINT16; }
	| EE_OB_QWORD											{ $$ = ee::EE_CT_UINT64; }
	| EE_OB_SDWORD											{ $$ = ee::EE_CT_INT32; }
	| EE_OB_SBYTE											{ $$ = ee::EE_CT_INT8; }
	| EE_OB_SWORD											{ $$ = ee::EE_CT_INT16; }
	| EE_OB_SQWORD											{ $$ = ee::EE_CT_INT64; }
	| EE_OB_FLOAT											{ $$ = ee::EE_CT_FLOAT; }
	| EE_OB_FLOAT16 										{ $$ = ee::EE_CT_FLOAT16; }
	| EE_OB_DOUBLE											{ $$ = ee::EE_CT_DOUBLE; }
	;

backing_type
	: EE_INT8												{ $$ = CExpEvalParser::token::EE_INT8; }
	| EE_INT16												{ $$ = CExpEvalParser::token::EE_INT16; }
	| EE_INT32												{ $$ = CExpEvalParser::token::EE_INT32; }
	| EE_INT64												{ $$ = CExpEvalParser::token::EE_INT64; }
	| EE_UINT8												{ $$ = CExpEvalParser::token::EE_UINT8; }
	| EE_UINT16												{ $$ = CExpEvalParser::token::EE_UINT16; }
	| EE_UINT32												{ $$ = CExpEvalParser::token::EE_UINT32; }
	| EE_UINT64												{ $$ = CExpEvalParser::token::EE_UINT64; }
	| EE_FLOAT												{ $$ = CExpEvalParser::token::EE_FLOAT; }
	| EE_DOUBLE												{ $$ = CExpEvalParser::token::EE_DOUBLE; }
	| EE_DEFAULT											{ $$ = CExpEvalParser::token::EE_DEFAULT; }
	;
	
cast_type
	: EE_INT8												{ $$ = ee::EE_CT_INT8; }
	| EE_INT16												{ $$ = ee::EE_CT_INT16; }
	| EE_INT32												{ $$ = ee::EE_CT_INT32; }
	| EE_INT64												{ $$ = ee::EE_CT_INT64; }
	| EE_UINT8												{ $$ = ee::EE_CT_UINT8; }
	| EE_UINT16												{ $$ = ee::EE_CT_UINT16; }
	| EE_UINT32												{ $$ = ee::EE_CT_UINT32; }
	| EE_UINT64												{ $$ = ee::EE_CT_UINT64; }
	| EE_FLOAT												{ $$ = ee::EE_CT_FLOAT; }
	| EE_DOUBLE												{ $$ = ee::EE_CT_DOUBLE; }
	| EE_FLOAT10											{ $$ = ee::EE_CT_FLOAT10; }
	| EE_FLOAT11											{ $$ = ee::EE_CT_FLOAT11; }
	| EE_FLOAT14											{ $$ = ee::EE_CT_FLOAT14; }
	| EE_FLOAT16											{ $$ = ee::EE_CT_FLOAT16; }
	;
	
backing_persistence
	: EE_PERSISTENT											{ $$ = token::EE_PERSISTENT; }
	| EE_TEMP												{ $$ = token::EE_TEMP; }
	;
	
argument_exp_entry
	: conditional_exp										{ m_peecContainer->CreateArgListEntry( $1, $$ ); }
	| argument_exp_entry ',' conditional_exp				{ m_peecContainer->CreateArgListEntry( $1, $3, $$ ); }
	;
	
argument_exp_list
	: argument_exp_entry									{ m_peecContainer->CreateArgList( $1, $$ ); }
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
	| EE_EPSILON '(' exp ',' exp ',' exp ')'				{ m_peecContainer->CreateIntrinsic3( token::EE_EPSILON, $3, $5, $7, $$ ); }
	| EE_RELEPSILON '(' exp ',' exp ',' exp ')'				{ m_peecContainer->CreateIntrinsic3( token::EE_RELEPSILON, $3, $5, $7, $$ ); }
	| EE_EPSILONF '(' exp ',' exp ',' exp ')'				{ m_peecContainer->CreateIntrinsic3( token::EE_EPSILONF, $3, $5, $7, $$ ); }
	| EE_RELEPSILONF '(' exp ',' exp ',' exp ')'			{ m_peecContainer->CreateIntrinsic3( token::EE_RELEPSILONF, $3, $5, $7, $$ ); }
	| EE_EXP '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_EXP, $3, $$ ); }
	| EE_LOG '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_LOG, $3, $$ ); }
	| EE_LOG '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_LOG, $3, $5, $$ ); }
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
	| EE_NEXTAFTERF '(' exp ',' exp ')'						{ m_peecContainer->CreateIntrinsic2( token::EE_NEXTAFTERF, $3, $5, $$ ); }
	| EE_NEXTTOWARD '(' exp ',' exp ')'						{ m_peecContainer->CreateIntrinsic2( token::EE_NEXTTOWARD, $3, $5, $$ ); }
	| EE_NEXTTOWARDF '(' exp ',' exp ')'					{ m_peecContainer->CreateIntrinsic2( token::EE_NEXTTOWARDF, $3, $5, $$ ); }
	| EE_DIM '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_DIM, $3, $5, $$ ); }
	| EE_MAX '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_MAX, $3, $5, $$ ); }
	| EE_MIN '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_MIN, $3, $5, $$ ); }
	| EE_DEG '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_DEG, $3, $$ ); }
	| EE_RAD '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_RAD, $3, $$ ); }
	| EE_ABS '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ABS, $3, $$ ); }
	| EE_MADD '(' exp ',' exp ',' exp ')'					{ m_peecContainer->CreateIntrinsic3( token::EE_MADD, $3, $5, $7, $$ ); }
	| EE_RAND '(' exp ',' exp ')'							{ m_peecContainer->CreateIntrinsic2( token::EE_RAND, $3, $5, $$ ); }
	| EE_ISNAN '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ISNAN, $3, $$ ); }
	| EE_ISINF '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ISINF, $3, $$ ); }
	| EE_BYTESWAPUSHORT '(' exp ')'							{ m_peecContainer->CreateIntrinsic1( token::EE_BYTESWAPUSHORT, $3, $$ ); }
	| EE_BYTESWAPULONG '(' exp ')'							{ m_peecContainer->CreateIntrinsic1( token::EE_BYTESWAPULONG, $3, $$ ); }
	| EE_BYTESWAPUINT64 '(' exp ')'							{ m_peecContainer->CreateIntrinsic1( token::EE_BYTESWAPUINT64, $3, $$ ); }
	| EE_AS_FLOAT '(' exp ')'								{ m_peecContainer->CreateAsFloat( $3, $$ ); }
	| EE_AS_DOUBLE '(' exp ')'								{ m_peecContainer->CreateAsDouble( $3, $$ ); }
	| EE_AS_FLOAT '(' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ')'
															{ m_peecContainer->CreateAsFloatX( $3, $5, $7, $9, $11, $13, $15, $$ ); }
	| EE_AS_FLOAT '(' exp ',' exp ',' exp ',' exp ',' exp ')'
															{ m_peecContainer->CreateAsFloatX( $3, $5, $7, $9, $11, $$ ); }
	| EE_AS_FLOAT16 '(' exp ')'								{ m_peecContainer->CreateAsFloat( $3, ee::EE_N_ASFLOAT16, $$ ); }
	| EE_AS_FLOAT14 '(' exp ')'								{ m_peecContainer->CreateAsFloat( $3, ee::EE_N_ASFLOAT14, $$ ); }
	| EE_AS_FLOAT11 '(' exp ')'								{ m_peecContainer->CreateAsFloat( $3, ee::EE_N_ASFLOAT11, $$ ); }
	| EE_AS_FLOAT10 '(' exp ')'								{ m_peecContainer->CreateAsFloat( $3, ee::EE_N_ASFLOAT10, $$ ); }
	| EE_AS_FLOAT_MAX '(' exp ',' exp ',' exp ',' exp ')'	{ m_peecContainer->CreateAsFloatXProp( $3, $5, $7, $9, ee::EE_N_ASXFLOAT_MAX, $$ ); }
	| EE_AS_FLOAT_MIN '(' exp ',' exp ',' exp ',' exp ')'	{ m_peecContainer->CreateAsFloatXProp( $3, $5, $7, $9, ee::EE_N_ASXFLOAT_MIN, $$ ); }
	| EE_AS_FLOAT_TRUE_MIN '(' exp ',' exp ',' exp ',' exp ')'
															{ m_peecContainer->CreateAsFloatXProp( $3, $5, $7, $9, ee::EE_N_ASXFLOAT_TRUE_MIN, $$ ); }
	| EE_AS_FLOAT_NAN '(' exp ',' exp ',' exp ',' exp ')'	{ m_peecContainer->CreateAsFloatXProp( $3, $5, $7, $9, ee::EE_N_ASXFLOAT_NAN, $$ ); }
	| EE_AS_FLOAT_INF '(' exp ',' exp ',' exp ',' exp ')'	{ m_peecContainer->CreateAsFloatXProp( $3, $5, $7, $9, ee::EE_N_ASXFLOAT_INF, $$ ); }
	| EE_AS_FLOAT_SUBNORM_MAX '(' exp ',' exp ',' exp ',' exp ')'
															{ m_peecContainer->CreateAsFloatXProp( $3, $5, $7, $9, ee::EE_N_ASXFLOAT_SUBNORM_MAX, $$ ); }
	| EE_AS_FLOAT_EPS '(' exp ',' exp ',' exp ',' exp ')'	{ m_peecContainer->CreateAsFloatXProp( $3, $5, $7, $9, ee::EE_N_ASXFLOAT_EPS, $$ ); }
	| EE_AS_FLOAT_SIGNBIT '(' exp ',' exp ',' exp ',' exp ',' exp ')'
															{ m_peecContainer->CreateAsFloatXProp( $3, $5, $7, $9, $11, ee::EE_N_ASXFLOAT_SIGNBIT, $$ ); }
	| EE_AS_FLOAT_EXPBITS '(' exp ',' exp ',' exp ',' exp ',' exp ')'
															{ m_peecContainer->CreateAsFloatXProp( $3, $5, $7, $9, $11, ee::EE_N_ASXFLOAT_EXPBITS, $$ ); }
	| EE_AS_FLOAT_MANBITS '(' exp ',' exp ',' exp ',' exp ',' exp ')'
															{ m_peecContainer->CreateAsFloatXProp( $3, $5, $7, $9, $11, ee::EE_N_ASXFLOAT_MANBITS, $$ ); }
	| EE_TO_FLOAT32 '(' exp ')'								{ m_peecContainer->CreateCast( $3, ee::EE_CT_FLOAT, $$ ); }
	| EE_TO_FLOAT64 '(' exp ')'								{ m_peecContainer->CreateCast( $3, ee::EE_CT_DOUBLE, $$ ); }
	| EE_TO_INT8 '(' exp ')'								{ m_peecContainer->CreateCast( $3, ee::EE_CT_INT8, $$ ); }
	| EE_TO_INT16 '(' exp ')'								{ m_peecContainer->CreateCast( $3, ee::EE_CT_INT16, $$ ); }
	| EE_TO_INT32 '(' exp ')'								{ m_peecContainer->CreateCast( $3, ee::EE_CT_INT32, $$ ); }
	| EE_TO_INT64 '(' exp ')'								{ m_peecContainer->CreateCast( $3, ee::EE_CT_INT64, $$ ); }
	| EE_TO_UINT8 '(' exp ')'								{ m_peecContainer->CreateCast( $3, ee::EE_CT_UINT8, $$ ); }
	| EE_TO_UINT16 '(' exp ')'								{ m_peecContainer->CreateCast( $3, ee::EE_CT_UINT16, $$ ); }
	| EE_TO_UINT32 '(' exp ')'								{ m_peecContainer->CreateCast( $3, ee::EE_CT_UINT32, $$ ); }
	| EE_TO_UINT64 '(' exp ')'								{ m_peecContainer->CreateCast( $3, ee::EE_CT_UINT64, $$ ); }
	| EE_CLOCK '(' ')'										{ m_peecContainer->CreateIntrinsic0( ee::CExpEvalParser::token::EE_CLOCK, $$ ); }
	| EE_SECONDS '(' ')'									{ m_peecContainer->CreateIntrinsic0( ee::CExpEvalParser::token::EE_SECONDS, $$ ); }
	| EE_MILLISECONDS '(' ')'								{ m_peecContainer->CreateIntrinsic0( ee::CExpEvalParser::token::EE_MILLISECONDS, $$ ); }
	| EE_MICROSECONDS '(' ')'								{ m_peecContainer->CreateIntrinsic0( ee::CExpEvalParser::token::EE_MICROSECONDS, $$ ); }
	| EE_SECONDS_SINCE_START '(' ')'						{ m_peecContainer->CreateIntrinsic0( ee::CExpEvalParser::token::EE_SECONDS_SINCE_START, $$ ); }
	| EE_MILLISECONDS_SINCE_START '(' ')'					{ m_peecContainer->CreateIntrinsic0( ee::CExpEvalParser::token::EE_MILLISECONDS_SINCE_START, $$ ); }
	| EE_MICROSECONDS_SINCE_START '(' ')'					{ m_peecContainer->CreateIntrinsic0( ee::CExpEvalParser::token::EE_MICROSECONDS_SINCE_START, $$ ); }
	| EE_ASCII '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ASCII, $3, $$ ); }
	| EE_BIN '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_BIN, $3, $$ ); }
	| EE_BOOL '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_BOOL, $3, $$ ); }
	| EE_CHR '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_CHR, $3, $$ ); }
	| EE_FLOAT '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_FLOAT, $3, $$ ); }
	| EE_HEX '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_HEX, $3, $$ ); }
	| EE_INT '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_INT, $3, $$ ); }
	| EE_LEN '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_LEN, $3, $$ ); }
	| EE_OCT '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_OCT, $3, $$ ); }
	| EE_ORD '(' exp ')'									{ m_peecContainer->CreateIntrinsic1( token::EE_ORD, $3, $$ ); }
	| string '.' EE_FORMAT '(' argument_exp_list ')'		{ m_peecContainer->CreateFormat( $1, $5, $$ ); }
	;

exp
	: assignment_exp										{ $$ = $1; }
	//| iteration_exp											{ $$ = $1; }
	//| selection_exp											{ $$ = $1; }
	//| jump_exp												{ $$ = $1; }
	;
	
exp_statement
	: exp ';'												{ $$ = $1; }
	| iteration_exp											{ $$ = $1; }
	| selection_exp											{ $$ = $1; }
	| jump_exp ';'											{ $$ = $1; }
	;

statement
	: exp_statement											{ $$ = $1; }
	;
	
compound_statement
	: statement												{ $$ = $1; }
	| compound_statement statement							{ m_peecContainer->CreateCompoundStatement( $1, $2, $$ ); }
	;
	
statement_list
	: exp													{ $$ = $1; }
	| compound_statement									{ $$ = $1; }
	| compound_statement exp								{ m_peecContainer->CreateCompoundStatement( $1, $2, $$ ); }
	;
	
foreach_decl0
	: EE_FOREACH '(' identifier EE_IN array_var ')'			{ m_peecContainer->CreateForEachDecl( $3, $5, $$ ); }
	;

foreach_decl1
	: EE_FOREACH '(' identifier EE_IN custom_var ')'		{ m_peecContainer->CreateForEachCustomDecl( $3, $5, $$ ); }
	;
	
foreach_decl2
	: EE_FOREACH '(' identifier EE_IN string ')'			{ m_peecContainer->CreateForEachStringDecl( $3, $5, $$ ); }
	;
	
iteration_exp
	: EE_WHILE '(' exp ')' '{' statement_list '}' 			{ m_peecContainer->CreateWhileLoop( $3, $6, $$ ); }
	| EE_FOR '(' exp_statement exp_statement ')' '{' statement_list '}'
															{ m_peecContainer->CreateForLoop( $3, $4, $7, $$ ); }
	| EE_FOR '(' exp_statement exp_statement exp ')' '{' statement_list '}'
															{ m_peecContainer->CreateForLoop( $3, $4, $5, $8, $$ ); }
	| EE_DO '{' statement_list '}' EE_WHILE '(' exp ')'		{ m_peecContainer->CreateDoWhileLoop( $7, $3, $$ ); }
	| foreach_decl0 '{' statement_list '}'					{ m_peecContainer->CreateForEachLoop( $1, $3, $$ ); }
	| foreach_decl1 '{' statement_list '}'					{ m_peecContainer->CreateForEachObjLoop( $1, $3, $$ ); }
	| foreach_decl2 '{' statement_list '}'					{ m_peecContainer->CreateForEachStrLoop( $1, $3, $$ ); }
	;
	
selection_exp
	: EE_IF '(' exp ')' '{' statement_list '}'				{ m_peecContainer->CreateSelectionStatement( $3, $6, $$ ); }
	| EE_IF '(' exp ')' '{' statement_list '}' EE_ELSE '{' statement_list '}'
															{ m_peecContainer->CreateSelectionStatement( $3, $6, $10, $$ ); }
	;
	
jump_exp
	: EE_CONTINUE											{ m_peecContainer->CreateContinue( $$ ); }
	| EE_BREAK												{ m_peecContainer->CreateBreak( $$ ); }
	;

translation_unit
	: statement_list										{ m_peecContainer->SetTrans( $1 ); }
	;


%%

int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, ee::CExpEvalLexer * _peelLexer ) {
	return _peelLexer->yylex();
}

void yy::parser::error( const yy::location &/*_lLoc*/, const std::string &/*_strM*/ ) {

}
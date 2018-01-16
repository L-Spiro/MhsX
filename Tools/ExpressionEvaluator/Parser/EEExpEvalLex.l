%option								yylineno

D									[0-9]
O									[0-7]
L									[a-zA-Z_]
H									[a-fA-F0-9]
E									[Ee][+-]?{D}+
FS									(f|F|l|L)
IS									(u|U|l|L)*
WS									[ \t\v\r\n\f]

%{

#include "../EEExpEvalSyntaxNodes.h"
#include "EEExpEvalParser.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include "ComplexAddr.tab.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YY_NEVER_INTERACTIVE		1


%}

%%

(?i:Pi)								{ return yy::CExpEvalParser::token::EE_PI; }
(?i:HalfPi)							{ return yy::CExpEvalParser::token::EE_HALFPI; }
(?i:E)								{ return yy::CExpEvalParser::token::EE_E; }
(?i:Zeta)							{ return yy::CExpEvalParser::token::EE_ZETA; }
(?i:GR)								{ return yy::CExpEvalParser::token::EE_GR; }
(?i:GoldenRatio)					{ return yy::CExpEvalParser::token::EE_GR; }
(?i:GoldenR)						{ return yy::CExpEvalParser::token::EE_GR; }
(?i:GoldenRat)						{ return yy::CExpEvalParser::token::EE_GR; }
(?i:GRatio)							{ return yy::CExpEvalParser::token::EE_GR; }
(?i:Euler)							{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:EulerMascheroni)				{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:Mascheroni)						{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:EM)								{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:K)								{ return yy::CExpEvalParser::token::EE_K; }
(?i:Khinchin)						{ return yy::CExpEvalParser::token::EE_K; }
(?i:A)								{ return yy::CExpEvalParser::token::EE_A; }
(?i:Glaisher)						{ return yy::CExpEvalParser::token::EE_A; }
(?i:GlaisherKinkelin)				{ return yy::CExpEvalParser::token::EE_A; }
(?i:Kinkelin)						{ return yy::CExpEvalParser::token::EE_A; }
(?i:M1)								{ return yy::CExpEvalParser::token::EE_M1; }
(?i:Meissel)						{ return yy::CExpEvalParser::token::EE_M1; }
(?i:MeisselMertens)					{ return yy::CExpEvalParser::token::EE_M1; }
(?i:Mertens)						{ return yy::CExpEvalParser::token::EE_M1; }
(?i:B)								{ return yy::CExpEvalParser::token::EE_B; }
(?i:Bernstein)						{ return yy::CExpEvalParser::token::EE_B; }
(?i:Lamda)							{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:Lambda)							{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:GWK)							{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:Gauss)							{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:Kuzmin)							{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:Wirsing)						{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:GaussKuzminWirsing)				{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:Sigma)							{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:HSMC)							{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:HSM)							{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:Hafner)							{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:Sarnak)							{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:McCurley)						{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:HafnerSarnakMcCurley)			{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:Omega)							{ return yy::CExpEvalParser::token::EE_OMEGA; }
(?i:Mu)								{ return yy::CExpEvalParser::token::EE_MU; }
(?i:Golomb)							{ return yy::CExpEvalParser::token::EE_MU; }
(?i:Dickman)						{ return yy::CExpEvalParser::token::EE_MU; }
(?i:GolombDickman)					{ return yy::CExpEvalParser::token::EE_MU; }
(?i:GD)								{ return yy::CExpEvalParser::token::EE_MU; }
(?i:Cahen)							{ return yy::CExpEvalParser::token::EE_CAHEN; }
(?i:C2)								{ return yy::CExpEvalParser::token::EE_C2; }
(?i:TwinPrime)						{ return yy::CExpEvalParser::token::EE_C2; }
(?i:TP)								{ return yy::CExpEvalParser::token::EE_C2; }
(?i:Laplace)						{ return yy::CExpEvalParser::token::EE_LAPLACE; }
(?i:Beta)							{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:Embree)							{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:Trefethen)						{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:EmbreeTrefethen)				{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:Landau)							{ return yy::CExpEvalParser::token::EE_RAMAN; }
(?i:LandauRamanujan)				{ return yy::CExpEvalParser::token::EE_RAMAN; }
(?i:Alladi)							{ return yy::CExpEvalParser::token::EE_ALLADI; }
(?i:Grinstead)						{ return yy::CExpEvalParser::token::EE_ALLADI; }
(?i:AlladiGrinstead)				{ return yy::CExpEvalParser::token::EE_ALLADI; }
(?i:B4)								{ return yy::CExpEvalParser::token::EE_B4; }
(?i:Brun)							{ return yy::CExpEvalParser::token::EE_B4; }
(?i:BrunPrimeQuadruplets)			{ return yy::CExpEvalParser::token::EE_B4; }
(?i:BrunPQ)							{ return yy::CExpEvalParser::token::EE_B4; }
(?i:BrunPrimeQuad)					{ return yy::CExpEvalParser::token::EE_B4; }
(?i:BrunPrimeQ)						{ return yy::CExpEvalParser::token::EE_B4; }
(?i:Catalan)						{ return yy::CExpEvalParser::token::EE_CATALAN; }
(?i:Lengyel)						{ return yy::CExpEvalParser::token::EE_LAMBDA; }
(?i:Leng)							{ return yy::CExpEvalParser::token::EE_LAMBDA; }
(?i:Viswanath)						{ return yy::CExpEvalParser::token::EE_VISW; }
(?i:Viswan)							{ return yy::CExpEvalParser::token::EE_VISW; }
(?i:Vis)							{ return yy::CExpEvalParser::token::EE_VISW; }
(?i:Zeta3)							{ return yy::CExpEvalParser::token::EE_Z3; }
(?i:Apery)							{ return yy::CExpEvalParser::token::EE_Z3; }
(?i:Conway)							{ return yy::CExpEvalParser::token::EE_CONWAY; }
(?i:Theta)							{ return yy::CExpEvalParser::token::EE_THETA; }
(?i:Mill)							{ return yy::CExpEvalParser::token::EE_THETA; }
(?i:Rho)							{ return yy::CExpEvalParser::token::EE_PLASTIC; }
(?i:Plastic)						{ return yy::CExpEvalParser::token::EE_PLASTIC; }
(?i:Ramanujan)						{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:Raman)							{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:RamaMu)							{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:Soldner)						{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:RamanujanSoldner)				{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:RS)								{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:RamaSoldner)					{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:RamaSol)						{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:Blackhouse)						{ return yy::CExpEvalParser::token::EE_BH; }
(?i:BH)								{ return yy::CExpEvalParser::token::EE_BH; }
(?i:Porter)							{ return yy::CExpEvalParser::token::EE_PORTER; }
(?i:Lieb)							{ return yy::CExpEvalParser::token::EE_ICE; }
(?i:SquareIce)						{ return yy::CExpEvalParser::token::EE_ICE; }
(?i:SqIce)							{ return yy::CExpEvalParser::token::EE_ICE; }
(?i:EB)								{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:Erdos)							{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:Borwein)						{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:ErdosBorwein)					{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:Niven)							{ return yy::CExpEvalParser::token::EE_NIVEN; }
(?i:Niv)							{ return yy::CExpEvalParser::token::EE_NIVEN; }
(?i:B2)								{ return yy::CExpEvalParser::token::EE_B2; }
(?i:Brun2)							{ return yy::CExpEvalParser::token::EE_B2; }
(?i:BrunTwinPrime)					{ return yy::CExpEvalParser::token::EE_B2; }
(?i:BrunTP)							{ return yy::CExpEvalParser::token::EE_B2; }
(?i:P2)								{ return yy::CExpEvalParser::token::EE_P2; }
(?i:Parabolic)						{ return yy::CExpEvalParser::token::EE_P2; }
(?i:Alpha)							{ return yy::CExpEvalParser::token::EE_ALPHA; }
(?i:Feigenbaum)						{ return yy::CExpEvalParser::token::EE_ALPHA; }
(?i:FeigenbaumA)					{ return yy::CExpEvalParser::token::EE_ALPHA; }
(?i:Sierp)							{ return yy::CExpEvalParser::token::EE_SIERP; }
(?i:Sierpinski)						{ return yy::CExpEvalParser::token::EE_SIERP; }
(?i:F)								{ return yy::CExpEvalParser::token::EE_F; }
(?i:Fransen)						{ return yy::CExpEvalParser::token::EE_F; }
(?i:Robinson)						{ return yy::CExpEvalParser::token::EE_F; }
(?i:FransenRobinson)				{ return yy::CExpEvalParser::token::EE_F; }
(?i:Levy)							{ return yy::CExpEvalParser::token::EE_LEVY; }
(?i:Psi)							{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:ReciprocalFibonacci)			{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:RecipFibonacci)					{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:RFibonacci)						{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:RF)								{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:Delta)							{ return yy::CExpEvalParser::token::EE_DELTA; }
(?i:FeigenbaumD)					{ return yy::CExpEvalParser::token::EE_DELTA; }

"cos"								{ return yy::CExpEvalParser::token::EE_COS; }
"cosf"								{ return yy::CExpEvalParser::token::EE_COS; }
"sin"								{ return yy::CExpEvalParser::token::EE_SIN; }
"sinf"								{ return yy::CExpEvalParser::token::EE_SIN; }
"tan"								{ return yy::CExpEvalParser::token::EE_TAN; }
"tanf"								{ return yy::CExpEvalParser::token::EE_TAN; }
"acos"								{ return yy::CExpEvalParser::token::EE_ACOS; }
"acosf"								{ return yy::CExpEvalParser::token::EE_ACOS; }
"asin"								{ return yy::CExpEvalParser::token::EE_ASIN; }
"asinf"								{ return yy::CExpEvalParser::token::EE_ASIN; }
"atan"								{ return yy::CExpEvalParser::token::EE_ATAN; }
"atanf"								{ return yy::CExpEvalParser::token::EE_ATAN; }
"atan2"								{ return yy::CExpEvalParser::token::EE_ATAN2; }
"atan2f"							{ return yy::CExpEvalParser::token::EE_ATAN2; }
"cosh"								{ return yy::CExpEvalParser::token::EE_COSH; }
"coshf"								{ return yy::CExpEvalParser::token::EE_COSH; }
"sinh"								{ return yy::CExpEvalParser::token::EE_SINH; }
"sinhf"								{ return yy::CExpEvalParser::token::EE_SINH; }
"tanh"								{ return yy::CExpEvalParser::token::EE_TANH; }
"tanhf"								{ return yy::CExpEvalParser::token::EE_TANH; }
"acosh"								{ return yy::CExpEvalParser::token::EE_ACOSH; }
"acoshf"							{ return yy::CExpEvalParser::token::EE_ACOSH; }
"asinh"								{ return yy::CExpEvalParser::token::EE_ASINH; }
"asinhf"							{ return yy::CExpEvalParser::token::EE_ASINH; }
"atanh"								{ return yy::CExpEvalParser::token::EE_ATANH; }
"atanhf"							{ return yy::CExpEvalParser::token::EE_ATANH; }
"exp"								{ return yy::CExpEvalParser::token::EE_EXP; }
"expf"								{ return yy::CExpEvalParser::token::EE_EXP; }
"log"								{ return yy::CExpEvalParser::token::EE_LOG; }
"logf"								{ return yy::CExpEvalParser::token::EE_LOG; }
"log10"								{ return yy::CExpEvalParser::token::EE_LOG10; }
"log10f"							{ return yy::CExpEvalParser::token::EE_LOG10; }
"log2"								{ return yy::CExpEvalParser::token::EE_LOG2; }
"log2f"								{ return yy::CExpEvalParser::token::EE_LOG2; }
"exp2"								{ return yy::CExpEvalParser::token::EE_EXP2; }
"exp2f"								{ return yy::CExpEvalParser::token::EE_EXP2; }
"expm1"								{ return yy::CExpEvalParser::token::EE_EXPM1; }
"ilogb"								{ return yy::CExpEvalParser::token::EE_ILOGB; }
"log1p"								{ return yy::CExpEvalParser::token::EE_LOG1P; }
"logb"								{ return yy::CExpEvalParser::token::EE_LOGB; }
"pow"								{ return yy::CExpEvalParser::token::EE_POW; }
"powf"								{ return yy::CExpEvalParser::token::EE_POW; }
"sqrt"								{ return yy::CExpEvalParser::token::EE_SQRT; }
"sqrtf"								{ return yy::CExpEvalParser::token::EE_SQRT; }
"cbrt"								{ return yy::CExpEvalParser::token::EE_CBRT; }
"cbrtf"								{ return yy::CExpEvalParser::token::EE_CBRT; }
"hypot"								{ return yy::CExpEvalParser::token::EE_HYPOT; }
"hypotf"							{ return yy::CExpEvalParser::token::EE_HYPOT; }
"tgamma"							{ return yy::CExpEvalParser::token::EE_TGAMMA; }
"lgamma"							{ return yy::CExpEvalParser::token::EE_LGAMMA; }
"ceil"								{ return yy::CExpEvalParser::token::EE_CEIL; }
"ceilf"								{ return yy::CExpEvalParser::token::EE_CEIL; }
"floor"								{ return yy::CExpEvalParser::token::EE_FLOOR; }
"floorf"							{ return yy::CExpEvalParser::token::EE_FLOOR; }
"mod"								{ return yy::CExpEvalParser::token::EE_MOD; }
"fmod"								{ return yy::CExpEvalParser::token::EE_MOD; }
"fmodf"								{ return yy::CExpEvalParser::token::EE_MOD; }
"trunc"								{ return yy::CExpEvalParser::token::EE_TRUNC; }
"truncf"							{ return yy::CExpEvalParser::token::EE_TRUNC; }
"round"								{ return yy::CExpEvalParser::token::EE_ROUND; }
"roundf"							{ return yy::CExpEvalParser::token::EE_ROUND; }
"nearbyint"							{ return yy::CExpEvalParser::token::EE_NEARBYINT; }
"remainder"							{ return yy::CExpEvalParser::token::EE_REMAINDER; }
"remquo"							{ return yy::CExpEvalParser::token::EE_REMQUO; }
"nextafter"							{ return yy::CExpEvalParser::token::EE_NEXTAFTER; }
"nextafterf"						{ return yy::CExpEvalParser::token::EE_NEXTAFTERF; }
"nexttoward"						{ return yy::CExpEvalParser::token::EE_NEXTTOWARD; }
"nexttowardf"						{ return yy::CExpEvalParser::token::EE_NEXTTOWARDF; }
"dim"								{ return yy::CExpEvalParser::token::EE_DIM; }
"fdim"								{ return yy::CExpEvalParser::token::EE_DIM; }
"fdimf"								{ return yy::CExpEvalParser::token::EE_DIM; }
"max"								{ return yy::CExpEvalParser::token::EE_MAX; }
"min"								{ return yy::CExpEvalParser::token::EE_MIN; }
"abs"								{ return yy::CExpEvalParser::token::EE_ABS; }
"fabs"								{ return yy::CExpEvalParser::token::EE_ABS; }
"madd"								{ return yy::CExpEvalParser::token::EE_MADD; }
"fma"								{ return yy::CExpEvalParser::token::EE_MADD; }

"CHAR_BIT"							{ return yy::CExpEvalParser::token::EE_CHAR_BIT; }
"MB_LEN_MAX"						{ return yy::CExpEvalParser::token::EE_MB_LEN_MAX; }
"CHAR_MIN"							{ return yy::CExpEvalParser::token::EE_CHAR_MIN; }
"CHAR_MAX"							{ return yy::CExpEvalParser::token::EE_CHAR_MAX; }
"SCHAR_MIN"							{ return yy::CExpEvalParser::token::EE_SCHAR_MIN; }
"SHRT_MIN"							{ return yy::CExpEvalParser::token::EE_SHRT_MIN; }
"INT_MIN"							{ return yy::CExpEvalParser::token::EE_INT_MIN; }
"LONG_MIN"							{ return yy::CExpEvalParser::token::EE_LONG_MIN; }
"LLONG_MIN"							{ return yy::CExpEvalParser::token::EE_LLONG_MIN; }
"SCHAR_MAX"							{ return yy::CExpEvalParser::token::EE_SCHAR_MAX; }
"SHRT_MAX"							{ return yy::CExpEvalParser::token::EE_SHRT_MAX; }
"INT_MAX"							{ return yy::CExpEvalParser::token::EE_INT_MAX; }
"LONG_MAX"							{ return yy::CExpEvalParser::token::EE_LONG_MAX; }
"LLONG_MAX"							{ return yy::CExpEvalParser::token::EE_LLONG_MAX; }
"UCHAR_MAX"							{ return yy::CExpEvalParser::token::EE_UCHAR_MAX; }
"USHRT_MAX"							{ return yy::CExpEvalParser::token::EE_USHRT_MAX; }
"UINT_MAX"							{ return yy::CExpEvalParser::token::EE_UINT_MAX; }
"ULONG_MAX"							{ return yy::CExpEvalParser::token::EE_ULONG_MAX; }
"ULLONG_MAX"						{ return yy::CExpEvalParser::token::EE_ULLONG_MAX; }
"FLT_RADIX"							{ return yy::CExpEvalParser::token::EE_FLT_RADIX; }
"DECIMAL_DIG"						{ return yy::CExpEvalParser::token::EE_DECIMAL_DIG; }
"FLT_DECIMAL_DIG"					{ return yy::CExpEvalParser::token::EE_FLT_DECIMAL_DIG; }
"DBL_DECIMAL_DIG"					{ return yy::CExpEvalParser::token::EE_DBL_DECIMAL_DIG; }
"LDBL_DECIMAL_DIG"					{ return yy::CExpEvalParser::token::EE_LDBL_DECIMAL_DIG; }
"FLT_MIN"							{ return yy::CExpEvalParser::token::EE_FLT_MIN; }
"DBL_MIN"							{ return yy::CExpEvalParser::token::EE_DBL_MIN; }
"LDBL_MIN"							{ return yy::CExpEvalParser::token::EE_LDBL_MIN; }
"FLT_TRUE_MIN"						{ return yy::CExpEvalParser::token::EE_FLT_TRUE_MIN; }
"DBL_TRUE_MIN"						{ return yy::CExpEvalParser::token::EE_DBL_TRUE_MIN; }
"LDBL_TRUE_MIN"						{ return yy::CExpEvalParser::token::EE_LDBL_TRUE_MIN; }
"FLT_MAX"							{ return yy::CExpEvalParser::token::EE_FLT_MAX; }
"DBL_MAX"							{ return yy::CExpEvalParser::token::EE_DBL_MAX; }
"LDBL_MAX"							{ return yy::CExpEvalParser::token::EE_LDBL_MAX; }
"FLT_EPSILON"						{ return yy::CExpEvalParser::token::EE_FLT_EPSILON; }
"DBL_EPSILON"						{ return yy::CExpEvalParser::token::EE_DBL_EPSILON; }
"LDBL_EPSILON"						{ return yy::CExpEvalParser::token::EE_LDBL_EPSILON; }
"FLT_DIG"							{ return yy::CExpEvalParser::token::EE_FLT_DIG; }
"DBL_DIG"							{ return yy::CExpEvalParser::token::EE_DBL_DIG; }
"LDBL_DIG"							{ return yy::CExpEvalParser::token::EE_LDBL_DIG; }
"FLT_MANT_DIG"						{ return yy::CExpEvalParser::token::EE_FLT_MANT_DIG; }
"DBL_MANT_DIG"						{ return yy::CExpEvalParser::token::EE_DBL_MANT_DIG; }
"LDBL_MANT_DIG"						{ return yy::CExpEvalParser::token::EE_LDBL_MANT_DIG; }
"FLT_MIN_EXP"						{ return yy::CExpEvalParser::token::EE_FLT_MIN_EXP; }
"DBL_MIN_EXP"						{ return yy::CExpEvalParser::token::EE_DBL_MIN_EXP; }
"LDBL_MIN_EXP"						{ return yy::CExpEvalParser::token::EE_LDBL_MIN_EXP; }
"FLT_MIN_10_EXP"					{ return yy::CExpEvalParser::token::EE_FLT_MIN_10_EXP; }
"DBL_MIN_10_EXP"					{ return yy::CExpEvalParser::token::EE_DBL_MIN_10_EXP; }
"LDBL_MIN_10_EXP"					{ return yy::CExpEvalParser::token::EE_LDBL_MIN_10_EXP; }
"FLT_MAX_EXP"						{ return yy::CExpEvalParser::token::EE_FLT_MAX_EXP; }
"DBL_MAX_EXP"						{ return yy::CExpEvalParser::token::EE_DBL_MAX_EXP; }
"LDBL_MAX_EXP"						{ return yy::CExpEvalParser::token::EE_LDBL_MAX_EXP; }
"FLT_MAX_10_EXP"					{ return yy::CExpEvalParser::token::EE_FLT_MAX_10_EXP; }
"DBL_MAX_10_EXP"					{ return yy::CExpEvalParser::token::EE_DBL_MAX_10_EXP; }
"LDBL_MAX_10_EXP"					{ return yy::CExpEvalParser::token::EE_LDBL_MAX_10_EXP; }

"as_float"							{ return yy::CExpEvalParser::token::EE_AS_FLOAT; }
"as_double"							{ return yy::CExpEvalParser::token::EE_AS_DOUBLE; }

"true"								{ return yy::CExpEvalParser::token::EE_TRUE; }
"false"								{ return yy::CExpEvalParser::token::EE_FALSE; }

L?'(\\.|[^\\'])+'					{ return yy::CExpEvalParser::token::EE_CHAR_CONSTANT; }
\"(\\.|[^\\"])*\"					{ return yy::CExpEvalParser::token::EE_STRING_CONSTANT; }

{D}+{E}{FS}?						{ return yy::CExpEvalParser::token::EE_FLOAT_CONSTANT; }
{D}*"."{D}+({E})?{FS}?				{ return yy::CExpEvalParser::token::EE_FLOAT_CONSTANT; }
{D}+"."{D}*({E})?{FS}?				{ return yy::CExpEvalParser::token::EE_FLOAT_CONSTANT; }

[bB]("["|"<:")						{ return yy::CExpEvalParser::token::EE_OB_BYTE; }
[wW]("["|"<:")						{ return yy::CExpEvalParser::token::EE_OB_WORD; }
[qQ]("["|"<:")						{ return yy::CExpEvalParser::token::EE_OB_QWORD; }
[fF]("["|"<:")						{ return yy::CExpEvalParser::token::EE_OB_FLOAT; }
[dD]("["|"<:")						{ return yy::CExpEvalParser::token::EE_OB_DOUBLE; }
("u8["|"u8<:")						{ return yy::CExpEvalParser::token::EE_OB_BYTE; }
("u16["|"u16<:")					{ return yy::CExpEvalParser::token::EE_OB_WORD; }
("u32["|"u32<:")					{ return yy::CExpEvalParser::token::EE_OB_DWORD; }
("u64["|"u64<:")					{ return yy::CExpEvalParser::token::EE_OB_QWORD; }
("i8["|"i8<:")						{ return yy::CExpEvalParser::token::EE_OB_SBYTE; }
("i16["|"i16<:")					{ return yy::CExpEvalParser::token::EE_OB_SWORD; }
("i32["|"i32<:")					{ return yy::CExpEvalParser::token::EE_OB_SDWORD; }
("i64["|"i64<:")					{ return yy::CExpEvalParser::token::EE_OB_SQWORD; }

("["|"<:")							{ return ('['); }
("]"|":>")							{ return (']'); }
"("									{ return ('('); }
")"									{ return (')'); }

"<<"								{ return yy::CExpEvalParser::token::EE_LEFT_OP; }
">>"								{ return yy::CExpEvalParser::token::EE_RIGHT_OP; }
","									{ return (','); }
"!"									{ return ('!'); }
"~"									{ return ('~'); }
"-"									{ return ('-'); }
"+"									{ return ('+'); }
"*"									{ return ('*'); }
"/"									{ return ('/'); }
"%"									{ return ('%'); }
"<"									{ return '<'; }
">"									{ return '>'; }
"<="								{ return yy::CExpEvalParser::token::EE_REL_LE; }
">="								{ return yy::CExpEvalParser::token::EE_REL_GE; }
"&"									{ return ('&'); }
"|"									{ return ('|'); }
"^"									{ return ('^'); }
"?"									{ return ('?'); }
":"									{ return (':'); }
"??"								{ return yy::CExpEvalParser::token::EE_USER_VAR; }
"&&"								{ return yy::CExpEvalParser::token::EE_AND; }
"||"								{ return yy::CExpEvalParser::token::EE_OR; }
"=="								{ return yy::CExpEvalParser::token::EE_EQU_E; }
"!="								{ return yy::CExpEvalParser::token::EE_EQU_NE; }
"::"								{ return yy::CExpEvalParser::token::EE_MEMBERACCESS; }

"#"{D}+{IS}?						{ return yy::CExpEvalParser::token::EE_PUREDEC_CONSTANT; }
0[xX]{H}+{IS}?						{ return yy::CExpEvalParser::token::EE_HEX_CONSTANT1; }
{H}+{IS}?[hH]						{ return yy::CExpEvalParser::token::EE_HEX_CONSTANT2; }
0{O}+{IS}?							{ return yy::CExpEvalParser::token::EE_OCT_CONSTANT; }
{D}+{IS}?							{ return yy::CExpEvalParser::token::EE_DEC_CONSTANT; }
{H}+{IS}?							{ return yy::CExpEvalParser::token::EE_HEX_CONSTANT3; }

{L}({L}|{D})*						{ return yy::CExpEvalParser::token::EE_IDENTIFIER; }

.									{ }
{WS}								{ }


%%


int yyFlexLexer::yywrap() {
	return 1;
}
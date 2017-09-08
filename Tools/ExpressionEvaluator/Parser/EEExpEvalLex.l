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

"cos"								{ return yy::CExpEvalParser::token::EE_COS; }
"sin"								{ return yy::CExpEvalParser::token::EE_SIN; }
"tan"								{ return yy::CExpEvalParser::token::EE_TAN; }
"acos"								{ return yy::CExpEvalParser::token::EE_ACOS; }
"asin"								{ return yy::CExpEvalParser::token::EE_ASIN; }
"atan"								{ return yy::CExpEvalParser::token::EE_ATAN; }
"atan2"								{ return yy::CExpEvalParser::token::EE_ATAN2; }
"cosh"								{ return yy::CExpEvalParser::token::EE_COSH; }
"sinh"								{ return yy::CExpEvalParser::token::EE_SINH; }
"tanh"								{ return yy::CExpEvalParser::token::EE_TANH; }
"acosh"								{ return yy::CExpEvalParser::token::EE_ACOSH; }
"asinh"								{ return yy::CExpEvalParser::token::EE_ASINH; }
"atanh"								{ return yy::CExpEvalParser::token::EE_ATANH; }
"exp"								{ return yy::CExpEvalParser::token::EE_EXP; }
"log"								{ return yy::CExpEvalParser::token::EE_LOG; }
"log10"								{ return yy::CExpEvalParser::token::EE_LOG10; }
"log2"								{ return yy::CExpEvalParser::token::EE_LOG2; }
"exp2"								{ return yy::CExpEvalParser::token::EE_EXP2; }
"expm1"								{ return yy::CExpEvalParser::token::EE_EXPM1; }
"ilogb"								{ return yy::CExpEvalParser::token::EE_ILOGB; }
"log1p"								{ return yy::CExpEvalParser::token::EE_LOG1P; }
"logb"								{ return yy::CExpEvalParser::token::EE_LOGB; }
"pow"								{ return yy::CExpEvalParser::token::EE_POW; }
"sqrt"								{ return yy::CExpEvalParser::token::EE_SQRT; }
"cbrt"								{ return yy::CExpEvalParser::token::EE_CBRT; }
"hypot"								{ return yy::CExpEvalParser::token::EE_HYPOT; }
"tgamma"							{ return yy::CExpEvalParser::token::EE_TGAMMA; }
"lgamma"							{ return yy::CExpEvalParser::token::EE_LGAMMA; }
"ceil"								{ return yy::CExpEvalParser::token::EE_CEIL; }
"floor"								{ return yy::CExpEvalParser::token::EE_FLOOR; }
"mod"								{ return yy::CExpEvalParser::token::EE_MOD; }
"trunc"								{ return yy::CExpEvalParser::token::EE_TRUNC; }
"round"								{ return yy::CExpEvalParser::token::EE_ROUND; }
"nearbyint"							{ return yy::CExpEvalParser::token::EE_NEARBYINT; }
"remainder"							{ return yy::CExpEvalParser::token::EE_REMAINDER; }
"remquo"							{ return yy::CExpEvalParser::token::EE_REMQUO; }
"nextafter"							{ return yy::CExpEvalParser::token::EE_NEXTAFTER; }
"nexttoward"						{ return yy::CExpEvalParser::token::EE_NEXTTOWARD; }
"dim"								{ return yy::CExpEvalParser::token::EE_DIM; }
"max"								{ return yy::CExpEvalParser::token::EE_MAX; }
"min"								{ return yy::CExpEvalParser::token::EE_MIN; }
"abs"								{ return yy::CExpEvalParser::token::EE_ABS; }
"madd"								{ return yy::CExpEvalParser::token::EE_MADD; }

("["|"<:")							{ return ('['); }
("]"|":>")							{ return (']'); }
"("									{ return ('('); }
")"									{ return (')'); }

"<<"								{ return yy::CExpEvalParser::token::EE_LEFT_OP; }
">>"								{ return yy::CExpEvalParser::token::EE_RIGHT_OP; }
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
"??"								{ /*return yy::CExpEvalParser::token::EE_USER_VAR;*/ }
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
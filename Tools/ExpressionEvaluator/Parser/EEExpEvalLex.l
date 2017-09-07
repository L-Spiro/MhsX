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

[bB]("["|"<:")						{ return EE_OB_BYTE; }
[wW]("["|"<:")						{ return EE_OB_WORD; }
[qQ]("["|"<:")						{ return EE_OB_QWORD; }
[fF]("["|"<:")						{ return EE_OB_FLOAT; }
[dD]("["|"<:")						{ return EE_OB_DOUBLE; }
("u8["|"u8<:")						{ return EE_OB_BYTE; }
("u16["|"u16<:")					{ return EE_OB_WORD; }
("u32["|"u32<:")					{ return EE_OB_DWORD; }
("u64["|"u64<:")					{ return EE_OB_QWORD; }

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
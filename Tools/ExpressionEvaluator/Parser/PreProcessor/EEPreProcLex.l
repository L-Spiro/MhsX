D															[0-9]
O															[0-7]
L															[a-zA-Z_]
H															[a-fA-F0-9]
E															[Ee][+-]?{D}+
FS															(f|F|l|L)
IS															(u|U|l|L)*
WS															[ \t\v\r\n\f]

%x															comment0

%{
#include "../PreProc/EEPreProcSyntaxNodes.h"				// Must be included before EEPreProcParser.h.
#include "EEPreProcParser.h"
using namespace ee;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YY_NEVER_INTERACTIVE								1

%}


%%


"//".*														{}
"/*"														BEGIN( comment0 );
<comment0>[^*\n]*											{ }
<comment0>"*"+[^*/\n]*										{ }
<comment0>\n												{ }
<comment0>"*"+"/"											BEGIN( INITIAL );

"defined"													{ return CPreProcParser::token::EE_PPT_DEFINED; }

L?'(\\.|[^\\'])+'											{ return CPreProcParser::token::EE_PPT_CHAR_CONSTANT; }

{D}+{E}{FS}?												{ return CPreProcParser::token::EE_PPT_FLOAT_CONSTANT; }
{D}*"."{D}+({E})?{FS}?										{ return CPreProcParser::token::EE_PPT_FLOAT_CONSTANT; }
{D}+"."{D}*({E})?{FS}?										{ return CPreProcParser::token::EE_PPT_FLOAT_CONSTANT; }

0{O}+{IS}?													{ return CPreProcParser::token::EE_PPT_OCT_CONSTANT; }
{D}+{IS}?													{ return CPreProcParser::token::EE_PPT_DEC_CONSTANT; }
0[xX]{H}+{IS}?												{ return CPreProcParser::token::EE_PPT_HEX_CONSTANT; }

{L}({L}|{D})*												{ return CPreProcParser::token::EE_PPT_IDENTIFIER; }

"("															{ return '('; }
")"															{ return ')'; }

"-"															{ return '-'; }
"+"															{ return '+'; }
"*"															{ return '*'; }
"/"															{ return '/'; }
"%"															{ return '%'; }

"~"															{ return '~'; }
"&"															{ return '&'; }
"^"															{ return '^'; }
"|"															{ return '|'; }
"<<"														{ return CPreProcParser::token::EE_PPT_SHIFT_LEFT; }
">>"														{ return CPreProcParser::token::EE_PPT_SHIFT_RIGHT; }

"&&"														{ return CPreProcParser::token::EE_PPT_AND; }
"||"														{ return CPreProcParser::token::EE_PPT_OR; }
":"															{ return ':'; }
"?"															{ return '?'; }

">"															{ return '>'; }
"<"															{ return '<'; }
">="														{ return CPreProcParser::token::EE_PPT_GE; }
"<="														{ return CPreProcParser::token::EE_PPT_LE; }
"=="														{ return CPreProcParser::token::EE_PPT_EQUALS; }
"!="														{ return CPreProcParser::token::EE_PPT_NOT_EQUALS; }

"!"															{ return '!'; }

.															{ }
{WS}														{ }


%%


int yyFlexLexer::yywrap() {
	return 1;
}

%option								yylineno noyywrap

D									[0-9]
O									[0-7]
B									[0-1]
L									[a-zA-Z_]
H   								[a-fA-F0-9]
HP  								(0[xX])
E									([Ee][+-]?{D}+)
P									([Pp][+-]?{D}+)
FS									(f|F|l|L)
IS  								(((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
ES  								(\\(['"\?\\abfnrtv ]|[0-7]{1,3}|x[a-fA-F0-9]{1,2}|u[a-fA-F0-9]{4}|U[a-fA-F0-9]{8}|N\{[_a-zA-Z0-9, \-]+\}))
ESX  								(\\(u[a-fA-F0-9]{4}|U[a-fA-F0-9]{8}))
WS									[ \t\v\r\n\f]
WS_P								({WS}*\+{WS}*)
WSC									({WS}*|{WS_P})
LS0									"\"\"\""
LS1									"'''"
CS0									\"
CS1									\'
CS0_S								"u8"?{CS0}
U16_S								[uL]{CS0}
U32_S								"U"{CS0}
RS0_S								[rR]{CS0}
RS1_S								[rR]{CS1}

%x									comment0 cstring0 cstring1 longstring0 longstring1 rawstring0 rawstring1 utf16string0 utf32string0

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
"//".*								{}
"/*"								BEGIN( comment0 );
<comment0>[^*\n]*					{ }
<comment0>"*"+[^*/\n]*				{ }
<comment0>\n						{ }
<comment0>"*"+"/"					BEGIN( INITIAL );

<utf16string0>\\x[a-fA-F0-9]{3,4}								{
																	size_t sConsumed = 0;
																	uint32_t uiVal = ee::CExpEval::EscapeX( &yytext[1], yyleng - 1, sConsumed, 4 );
																	if ( !sConsumed ) { yy_tmp_txt.append( yytext, yyleng ); }
																	else {
																		uint32_t ui32Len;
																		uint32_t ui32BackToUtf8 = ee::CExpEval::Utf32ToUtf8( uiVal, ui32Len );
																		if ( ui32BackToUtf8 == EE_UTF_INVALID ) { yy_tmp_txt.append( yytext, yyleng ); }
																		else {
																			for ( uint32_t J = 0; J < ui32Len; ++J ) {
																				yy_tmp_txt.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
																				ui32BackToUtf8 >>= 8;
																			}
																		}
																	}
																}
																
<utf16string0>\\[0-7]{4,6}										{
																	size_t sConsumed = 0;
																	uint32_t uiVal = ee::CExpEval::EscapeOctal( &yytext[1], yyleng - 1, sConsumed, 6 );
																	if ( !sConsumed ) { yy_tmp_txt.append( yytext, yyleng ); }
																	else {
																		uint32_t ui32Len;
																		uint32_t ui32BackToUtf8 = ee::CExpEval::Utf32ToUtf8( uiVal, ui32Len );
																		if ( ui32BackToUtf8 == EE_UTF_INVALID ) { yy_tmp_txt.append( yytext, yyleng ); }
																		else {
																			for ( uint32_t J = 0; J < ui32Len; ++J ) {
																				yy_tmp_txt.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
																				ui32BackToUtf8 >>= 8;
																			}
																		}
																	}
																}
																
<utf32string0>\\x[a-fA-F0-9]{3,8}								{
																	size_t sConsumed = 0;
																	uint32_t uiVal = ee::CExpEval::EscapeX( &yytext[1], yyleng - 1, sConsumed, 8 );
																	if ( !sConsumed ) { yy_tmp_txt.append( yytext, yyleng ); }
																	else {
																		uint32_t ui32Len;
																		uint32_t ui32BackToUtf8 = ee::CExpEval::Utf32ToUtf8( uiVal, ui32Len );
																		if ( ui32BackToUtf8 == EE_UTF_INVALID ) { yy_tmp_txt.append( yytext, yyleng ); }
																		else {
																			for ( uint32_t J = 0; J < ui32Len; ++J ) {
																				yy_tmp_txt.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
																				ui32BackToUtf8 >>= 8;
																			}
																		}
																	}
																}
																
<utf32string0>\\[0-7]{4,11}										{
																	size_t sConsumed = 0;
																	uint32_t uiVal = ee::CExpEval::EscapeOctal( &yytext[1], yyleng - 1, sConsumed, 11 );
																	if ( !sConsumed ) { yy_tmp_txt.append( yytext, yyleng ); }
																	else {
																		uint32_t ui32Len;
																		uint32_t ui32BackToUtf8 = ee::CExpEval::Utf32ToUtf8( uiVal, ui32Len );
																		if ( ui32BackToUtf8 == EE_UTF_INVALID ) { yy_tmp_txt.append( yytext, yyleng ); }
																		else {
																			for ( uint32_t J = 0; J < ui32Len; ++J ) {
																				yy_tmp_txt.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
																				ui32BackToUtf8 >>= 8;
																			}
																		}
																	}
																}

<longstring0,longstring1,cstring0,cstring1,utf16string0,utf32string0>{ES}					{
																	size_t sConsumed = 0;
																	bool bEscapeFound = false;
																	uint32_t uiVal = static_cast<uint32_t>(ee::CExpEval::ResolveEscape( yytext, yyleng, sConsumed, false, &bEscapeFound ));
																	if ( !bEscapeFound ) { yy_tmp_txt.append( yytext, yyleng ); }
																	else {
																		uint32_t ui32Len;
																		uint32_t ui32BackToUtf8 = ee::CExpEval::Utf32ToUtf8( uiVal, ui32Len );
																		for ( uint32_t J = 0; J < ui32Len; ++J ) {
																			yy_tmp_txt.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
																			ui32BackToUtf8 >>= 8;
																		}
																	}
																}

{LS0}								{ BEGIN( longstring0 ); yy_tmp_txt.clear(); }
<longstring0>{LS0}					{
										BEGIN( INITIAL );
										return yy::CExpEvalParser::token::EE_STRING_CONSTANT;
									}
<longstring0>({LS0}{WSC}{LS0})		{	/* Nothing to do. */ }
<longstring0>({LS0}{WSC}{LS1})		{ BEGIN( longstring1 ); }
<longstring0>({LS0}{WSC}{CS0_S})	{ BEGIN( cstring0 ); }
<longstring0>({LS0}{WSC}{CS1})		{ BEGIN( cstring1 ); }
<longstring0>({LS0}{WSC}{RS0_S})	{ BEGIN( rawstring0 ); }
<longstring0>({LS0}{WSC}{RS1_S})	{ BEGIN( rawstring1 ); }
<longstring0>({LS0}{WSC}{U16_S})	{ BEGIN( utf16string0 ); }
<longstring0>({LS0}{WSC}{U32_S})	{ BEGIN( utf32string0 ); }
									
{LS1}								{ BEGIN( longstring1 ); yy_tmp_txt.clear(); }
<longstring1>{LS1}					{
										BEGIN( INITIAL );
										return yy::CExpEvalParser::token::EE_STRING_CONSTANT;
									}
<longstring1>({LS1}{WSC}{LS1})		{	/* Nothing to do. */ }
<longstring1>({LS1}{WSC}{LS0})		{ BEGIN( longstring0 ); }
<longstring1>({LS1}{WSC}{CS0_S})	{ BEGIN( cstring0 ); }
<longstring1>({LS1}{WSC}{CS1})		{ BEGIN( cstring1 ); }
<longstring1>({LS1}{WSC}{RS0_S})	{ BEGIN( rawstring0 ); }
<longstring1>({LS1}{WSC}{RS1_S})	{ BEGIN( rawstring1 ); }
<longstring1>({LS1}{WSC}{U16_S})	{ BEGIN( utf16string0 ); }
<longstring1>({LS1}{WSC}{U32_S})	{ BEGIN( utf32string0 ); }


{CS0_S}								{ BEGIN( cstring0 ); yy_tmp_txt.clear(); }
<cstring0>{CS0}						{
										BEGIN( INITIAL );
										return yy::CExpEvalParser::token::EE_STRING_CONSTANT;
									}
									
{CS1}								{ BEGIN( cstring1 ); yy_tmp_txt.clear(); }
<cstring1>{CS1}						{
										BEGIN( INITIAL );
										if ( yy_tmp_txt.size() == 1 ) { return yy::CExpEvalParser::token::EE_CHAR_CONSTANT; }
										return yy::CExpEvalParser::token::EE_STRING_CONSTANT;
									}

{RS0_S}								{ BEGIN( rawstring0 ); yy_tmp_txt.clear(); }
<rawstring0>{CS0}					{
										BEGIN( INITIAL );
										return yy::CExpEvalParser::token::EE_STRING_CONSTANT;
									}
									
{RS1_S}								{ BEGIN( rawstring1 ); yy_tmp_txt.clear(); }
<rawstring1>{CS1}					{
										BEGIN( INITIAL );
										if ( yy_tmp_txt.size() == 1 ) { return yy::CExpEvalParser::token::EE_CHAR_CONSTANT; }
										return yy::CExpEvalParser::token::EE_STRING_CONSTANT;
									}
									
{U16_S}								{ BEGIN( utf16string0 ); yy_tmp_txt.clear(); }
<utf16string0>{CS0}					{
										BEGIN( INITIAL );
										return yy::CExpEvalParser::token::EE_STRING_CONSTANT;
									}
									
{U32_S}								{ BEGIN( utf32string0 ); yy_tmp_txt.clear(); }
<utf32string0>{CS0}					{
										BEGIN( INITIAL );
										return yy::CExpEvalParser::token::EE_STRING_CONSTANT;
									}

<rawstring0,cstring0,utf16string0,utf32string0>({CS0}{WSC}{LS1})		{ BEGIN( longstring1 ); }
<rawstring0,cstring0,utf16string0,utf32string0>({CS0}{WSC}{LS0})		{ BEGIN( longstring0 ); }
<rawstring0,cstring0,utf16string0,utf32string0>({CS0}{WSC}{RS0_S})		{ BEGIN( rawstring0 ); }
<rawstring0,cstring0,utf16string0,utf32string0>({CS0}{WSC}{RS1_S})		{ BEGIN( rawstring1 ); }
<rawstring0,cstring0,utf16string0,utf32string0>({CS0}{WSC}{CS0_S})		{ BEGIN( cstring0 ); }
<rawstring0,cstring0,utf16string0,utf32string0>({CS0}{WSC}{CS1})		{ BEGIN( cstring1 ); }
<rawstring0,cstring0,utf16string0,utf32string0>({CS0}{WSC}{U16_S})		{ BEGIN( utf16string0 ); }
<rawstring0,cstring0,utf16string0,utf32string0>({CS0}{WSC}{U32_S})		{ BEGIN( utf32string0 ); }

<rawstring1,cstring1>({CS1}{WSC}{LS1})					{ BEGIN( longstring1 ); }
<rawstring1,cstring1>({CS1}{WSC}{LS0})					{ BEGIN( longstring0 ); }
<rawstring1,cstring1>({CS1}{WSC}{RS0_S})				{ BEGIN( rawstring0 ); }
<rawstring1,cstring1>({CS1}{WSC}{RS1_S})				{ BEGIN( rawstring1 ); }
<rawstring1,cstring1>({CS1}{WSC}{CS0_S})				{ BEGIN( cstring0 ); }
<rawstring1,cstring1>({CS1}{WS}*{CS1})					{ BEGIN( cstring1 ); }
<rawstring1,cstring1>({CS1}{WSC}{U16_S})				{ BEGIN( utf16string0 ); }
<rawstring1,cstring1>({CS1}{WSC}{U32_S})				{ BEGIN( utf32string0 ); }

<longstring0,longstring1,cstring0,cstring1,rawstring0,rawstring1,utf16string0,utf32string0>.		{ yy_tmp_txt.append( yytext, yyleng ); }


"const"								{ return yy::CExpEvalParser::token::EE_CONST; }

"do"								{ return yy::CExpEvalParser::token::EE_DO; }
"for"								{ return yy::CExpEvalParser::token::EE_FOR; }
"foreach"							{ return yy::CExpEvalParser::token::EE_FOREACH; }
"if"								{ return yy::CExpEvalParser::token::EE_IF; }
"else"								{ return yy::CExpEvalParser::token::EE_ELSE; }
"while"								{ return yy::CExpEvalParser::token::EE_WHILE; }
"in"								{ return yy::CExpEvalParser::token::EE_IN; }

"break"								{ return yy::CExpEvalParser::token::EE_BREAK; }
"continue"							{ return yy::CExpEvalParser::token::EE_CONTINUE; }

"new"								{ return yy::CExpEvalParser::token::EE_NEW; }
"copy"								{ return yy::CExpEvalParser::token::EE_COPY; }

"true"								{ return yy::CExpEvalParser::token::EE_TRUE; }
"false"								{ return yy::CExpEvalParser::token::EE_FALSE; }

"int8"								{ return yy::CExpEvalParser::token::EE_INT8; }
"int16"								{ return yy::CExpEvalParser::token::EE_INT16; }
"int32"								{ return yy::CExpEvalParser::token::EE_INT32; }
"int64"								{ return yy::CExpEvalParser::token::EE_INT64; }
"uint8"								{ return yy::CExpEvalParser::token::EE_UINT8; }
"uint16"							{ return yy::CExpEvalParser::token::EE_UINT16; }
"uint32"							{ return yy::CExpEvalParser::token::EE_UINT32; }
"uint64"							{ return yy::CExpEvalParser::token::EE_UINT64; }
"float"								{ return yy::CExpEvalParser::token::EE_FLOAT; }
"float10"							{ return yy::CExpEvalParser::token::EE_FLOAT10; }
"float11"							{ return yy::CExpEvalParser::token::EE_FLOAT11; }
"float14"							{ return yy::CExpEvalParser::token::EE_FLOAT14; }
"float16"							{ return yy::CExpEvalParser::token::EE_FLOAT16; }
"float32"							{ return yy::CExpEvalParser::token::EE_FLOAT; }
"double"							{ return yy::CExpEvalParser::token::EE_DOUBLE; }
"float64"							{ return yy::CExpEvalParser::token::EE_DOUBLE; }
"object"							{ return yy::CExpEvalParser::token::EE_OBJECT; }
"persistent"						{ return yy::CExpEvalParser::token::EE_PERSISTENT; }
"temp"								{ return yy::CExpEvalParser::token::EE_TEMP; }
"temporary"							{ return yy::CExpEvalParser::token::EE_TEMP; }
"default"							{ return yy::CExpEvalParser::token::EE_DEFAULT; }
"any"								{ return yy::CExpEvalParser::token::EE_ANY; }

"#"{D}+{IS}?						{ return yy::CExpEvalParser::token::EE_PUREDEC_CONSTANT; }
0[bB]{B}+{IS}?						{ return yy::CExpEvalParser::token::EE_BIN_CONSTANT; }
0[oO]{O}+{IS}?						{ return yy::CExpEvalParser::token::EE_OCT_CONSTANT; }
0[xX]{H}+{IS}?						{ return yy::CExpEvalParser::token::EE_HEX_CONSTANT1; }
{H}+{IS}?[hH]						{ return yy::CExpEvalParser::token::EE_HEX_CONSTANT2; }
0{O}+{IS}?							{ return yy::CExpEvalParser::token::EE_OCT_CONSTANT; }
{D}+{IS}?							{ return yy::CExpEvalParser::token::EE_DEC_CONSTANT; }
{H}+{IS}?							{ return yy::CExpEvalParser::token::EE_HEX_CONSTANT3; }

(?i:_Pi)							{ return yy::CExpEvalParser::token::EE_PI; }
(?i:_HalfPi)						{ return yy::CExpEvalParser::token::EE_HALFPI; }
(?i:_E)								{ return yy::CExpEvalParser::token::EE_E; }
(?i:_Zeta)							{ return yy::CExpEvalParser::token::EE_ZETA; }
(?i:_GR)							{ return yy::CExpEvalParser::token::EE_GR; }
(?i:_GoldenRatio)					{ return yy::CExpEvalParser::token::EE_GR; }
(?i:_GoldenR)						{ return yy::CExpEvalParser::token::EE_GR; }
(?i:_GoldenRat)						{ return yy::CExpEvalParser::token::EE_GR; }
(?i:_GRatio)						{ return yy::CExpEvalParser::token::EE_GR; }
(?i:_Euler)							{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:_EulerMascheroni)				{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:_Mascheroni)					{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:_EM)							{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:_K)								{ return yy::CExpEvalParser::token::EE_K; }
(?i:_Khinchin)						{ return yy::CExpEvalParser::token::EE_K; }
(?i:_A)								{ return yy::CExpEvalParser::token::EE_A; }
(?i:_Glaisher)						{ return yy::CExpEvalParser::token::EE_A; }
(?i:_GlaisherKinkelin)				{ return yy::CExpEvalParser::token::EE_A; }
(?i:_Kinkelin)						{ return yy::CExpEvalParser::token::EE_A; }
(?i:_M1)							{ return yy::CExpEvalParser::token::EE_M1; }
(?i:_Meissel)						{ return yy::CExpEvalParser::token::EE_M1; }
(?i:_MeisselMertens)				{ return yy::CExpEvalParser::token::EE_M1; }
(?i:_Mertens)						{ return yy::CExpEvalParser::token::EE_M1; }
(?i:_B)								{ return yy::CExpEvalParser::token::EE_B; }
(?i:_Bernstein)						{ return yy::CExpEvalParser::token::EE_B; }
(?i:_Lamda)							{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_Lambda)						{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_GWK)							{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_Gauss)							{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_Kuzmin)						{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_Wirsing)						{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_GaussKuzminWirsing)			{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_Sigma)							{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_HSMC)							{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_HSM)							{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_Hafner)						{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_Sarnak)						{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_McCurley)						{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_HafnerSarnakMcCurley)			{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_Omega)							{ return yy::CExpEvalParser::token::EE_OMEGA; }
(?i:_Mu)							{ return yy::CExpEvalParser::token::EE_MU; }
(?i:_Golomb)						{ return yy::CExpEvalParser::token::EE_MU; }
(?i:_Dickman)						{ return yy::CExpEvalParser::token::EE_MU; }
(?i:_GolombDickman)					{ return yy::CExpEvalParser::token::EE_MU; }
(?i:_GD)							{ return yy::CExpEvalParser::token::EE_MU; }
(?i:_Cahen)							{ return yy::CExpEvalParser::token::EE_CAHEN; }
(?i:_C2)							{ return yy::CExpEvalParser::token::EE_C2; }
(?i:_TwinPrime)						{ return yy::CExpEvalParser::token::EE_C2; }
(?i:_TP)							{ return yy::CExpEvalParser::token::EE_C2; }
(?i:_Laplace)						{ return yy::CExpEvalParser::token::EE_LAPLACE; }
(?i:_Beta)							{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:_Embree)						{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:_Trefethen)						{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:_EmbreeTrefethen)				{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:_Landau)						{ return yy::CExpEvalParser::token::EE_RAMAN; }
(?i:_LandauRamanujan)				{ return yy::CExpEvalParser::token::EE_RAMAN; }
(?i:_Alladi)						{ return yy::CExpEvalParser::token::EE_ALLADI; }
(?i:_Grinstead)						{ return yy::CExpEvalParser::token::EE_ALLADI; }
(?i:_AlladiGrinstead)				{ return yy::CExpEvalParser::token::EE_ALLADI; }
(?i:_B4)							{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_Brun)							{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_BrunPrimeQuadruplets)			{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_BrunPQ)						{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_BrunPrimeQuad)					{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_BrunPrimeQ)					{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_Catalan)						{ return yy::CExpEvalParser::token::EE_CATALAN; }
(?i:_Lengyel)						{ return yy::CExpEvalParser::token::EE_LAMBDA; }
(?i:_Leng)							{ return yy::CExpEvalParser::token::EE_LAMBDA; }
(?i:_Viswanath)						{ return yy::CExpEvalParser::token::EE_VISW; }
(?i:_Viswan)						{ return yy::CExpEvalParser::token::EE_VISW; }
(?i:_Vis)							{ return yy::CExpEvalParser::token::EE_VISW; }
(?i:_Zeta3)							{ return yy::CExpEvalParser::token::EE_Z3; }
(?i:_Apery)							{ return yy::CExpEvalParser::token::EE_Z3; }
(?i:_Conway)						{ return yy::CExpEvalParser::token::EE_CONWAY; }
(?i:_Theta)							{ return yy::CExpEvalParser::token::EE_THETA; }
(?i:_Mill)							{ return yy::CExpEvalParser::token::EE_THETA; }
(?i:_Rho)							{ return yy::CExpEvalParser::token::EE_PLASTIC; }
(?i:_Plastic)						{ return yy::CExpEvalParser::token::EE_PLASTIC; }
(?i:_Ramanujan)						{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_Raman)							{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_RamaMu)						{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_Soldner)						{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_RamanujanSoldner)				{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_RS)							{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_RamaSoldner)					{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_RamaSol)						{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_Blackhouse)					{ return yy::CExpEvalParser::token::EE_BH; }
(?i:_BH)							{ return yy::CExpEvalParser::token::EE_BH; }
(?i:_Porter)						{ return yy::CExpEvalParser::token::EE_PORTER; }
(?i:_Lieb)							{ return yy::CExpEvalParser::token::EE_ICE; }
(?i:_SquareIce)						{ return yy::CExpEvalParser::token::EE_ICE; }
(?i:_SqIce)							{ return yy::CExpEvalParser::token::EE_ICE; }
(?i:_EB)							{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:_Erdos)							{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:_Borwein)						{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:_ErdosBorwein)					{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:_Niven)							{ return yy::CExpEvalParser::token::EE_NIVEN; }
(?i:_Niv)							{ return yy::CExpEvalParser::token::EE_NIVEN; }
(?i:_B2)							{ return yy::CExpEvalParser::token::EE_B2; }
(?i:_Brun2)							{ return yy::CExpEvalParser::token::EE_B2; }
(?i:_BrunTwinPrime)					{ return yy::CExpEvalParser::token::EE_B2; }
(?i:_BrunTP)						{ return yy::CExpEvalParser::token::EE_B2; }
(?i:_P2)							{ return yy::CExpEvalParser::token::EE_P2; }
(?i:_Parabolic)						{ return yy::CExpEvalParser::token::EE_P2; }
(?i:_Alpha)							{ return yy::CExpEvalParser::token::EE_ALPHA; }
(?i:_Feigenbaum)					{ return yy::CExpEvalParser::token::EE_ALPHA; }
(?i:_FeigenbaumA)					{ return yy::CExpEvalParser::token::EE_ALPHA; }
(?i:_Sierp)							{ return yy::CExpEvalParser::token::EE_SIERP; }
(?i:_Sierpinski)					{ return yy::CExpEvalParser::token::EE_SIERP; }
(?i:_F)								{ return yy::CExpEvalParser::token::EE_F; }
(?i:_Fransen)						{ return yy::CExpEvalParser::token::EE_F; }
(?i:_Robinson)						{ return yy::CExpEvalParser::token::EE_F; }
(?i:_FransenRobinson)				{ return yy::CExpEvalParser::token::EE_F; }
(?i:_Levy)							{ return yy::CExpEvalParser::token::EE_LEVY; }
(?i:_Psi)							{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:_ReciprocalFibonacci)			{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:_RecipFibonacci)				{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:_RFibonacci)					{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:_RF)							{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:_Delta)							{ return yy::CExpEvalParser::token::EE_DELTA; }
(?i:_FeigenbaumD)					{ return yy::CExpEvalParser::token::EE_DELTA; }

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
"epsilon"							{ return yy::CExpEvalParser::token::EE_EPSILON; }
"repsilon"							{ return yy::CExpEvalParser::token::EE_RELEPSILON; }
"relepsilon"						{ return yy::CExpEvalParser::token::EE_RELEPSILON; }
"epsilonf"							{ return yy::CExpEvalParser::token::EE_EPSILONF; }
"repsilonf"							{ return yy::CExpEvalParser::token::EE_RELEPSILONF; }
"relepsilonf"						{ return yy::CExpEvalParser::token::EE_RELEPSILONF; }
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
"deg"								{ return yy::CExpEvalParser::token::EE_DEG; }
"rad"								{ return yy::CExpEvalParser::token::EE_RAD; }
"abs"								{ return yy::CExpEvalParser::token::EE_ABS; }
"fabs"								{ return yy::CExpEvalParser::token::EE_ABS; }
"madd"								{ return yy::CExpEvalParser::token::EE_MADD; }
"fma"								{ return yy::CExpEvalParser::token::EE_MADD; }
"rand"								{ return yy::CExpEvalParser::token::EE_RAND; }
"isnan"								{ return yy::CExpEvalParser::token::EE_ISNAN; }
"isinf"								{ return yy::CExpEvalParser::token::EE_ISINF; }
"_byteswap_ushort"					{ return yy::CExpEvalParser::token::EE_BYTESWAPUSHORT; }
"_byteswap_uint16"					{ return yy::CExpEvalParser::token::EE_BYTESWAPUSHORT; }
"_byteswap_ulong"					{ return yy::CExpEvalParser::token::EE_BYTESWAPULONG; }
"_byteswap_uint32"					{ return yy::CExpEvalParser::token::EE_BYTESWAPULONG; }
"_byteswap_uint64"					{ return yy::CExpEvalParser::token::EE_BYTESWAPUINT64; }

"format"							{ return yy::CExpEvalParser::token::EE_FORMAT; }


"ascii"								{ return yy::CExpEvalParser::token::EE_ASCII; }
"bin"								{ return yy::CExpEvalParser::token::EE_BIN; }
"bool"								{ return yy::CExpEvalParser::token::EE_BOOL; }
"chr"								{ return yy::CExpEvalParser::token::EE_CHR; }
"hex"								{ return yy::CExpEvalParser::token::EE_HEX; }
"int"								{ return yy::CExpEvalParser::token::EE_INT; }
"len"								{ return yy::CExpEvalParser::token::EE_LEN; }
"oct"								{ return yy::CExpEvalParser::token::EE_OCT; }
"ord"								{ return yy::CExpEvalParser::token::EE_ORD; }


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
"INT8_MIN"							{ return yy::CExpEvalParser::token::EE_INT8_MIN; }
"INT16_MIN"							{ return yy::CExpEvalParser::token::EE_INT16_MIN; }
"INT32_MIN"							{ return yy::CExpEvalParser::token::EE_INT32_MIN; }
"INT64_MIN"							{ return yy::CExpEvalParser::token::EE_INT64_MIN; }
"INT8_MAX"							{ return yy::CExpEvalParser::token::EE_INT8_MAX; }
"INT16_MAX"							{ return yy::CExpEvalParser::token::EE_INT16_MAX; }
"INT32_MAX"							{ return yy::CExpEvalParser::token::EE_INT32_MAX; }
"INT64_MAX"							{ return yy::CExpEvalParser::token::EE_INT64_MAX; }
"UINT8_MAX"							{ return yy::CExpEvalParser::token::EE_UINT8_MAX; }
"UINT16_MAX"						{ return yy::CExpEvalParser::token::EE_UINT16_MAX; }
"UINT32_MAX"						{ return yy::CExpEvalParser::token::EE_UINT32_MAX; }
"UINT64_MAX"						{ return yy::CExpEvalParser::token::EE_UINT64_MAX; }
"CLOCKS_PER_SEC"					{ return yy::CExpEvalParser::token::EE_CLOCKS_PER_SEC; }

"as_float64"						{ return yy::CExpEvalParser::token::EE_AS_DOUBLE; }
"as_double"							{ return yy::CExpEvalParser::token::EE_AS_DOUBLE; }
"as_float32"						{ return yy::CExpEvalParser::token::EE_AS_FLOAT; }
"as_float"							{ return yy::CExpEvalParser::token::EE_AS_FLOAT; }
"as_float16"						{ return yy::CExpEvalParser::token::EE_AS_FLOAT16; }
"as_float14"						{ return yy::CExpEvalParser::token::EE_AS_FLOAT14; }
"as_float11"						{ return yy::CExpEvalParser::token::EE_AS_FLOAT11; }
"as_float10"						{ return yy::CExpEvalParser::token::EE_AS_FLOAT10; }
"as_float_max"						{ return yy::CExpEvalParser::token::EE_AS_FLOAT_MAX; }
"as_float_min"						{ return yy::CExpEvalParser::token::EE_AS_FLOAT_MIN; }
"as_float_true_min"					{ return yy::CExpEvalParser::token::EE_AS_FLOAT_TRUE_MIN; }
"as_float_nan"						{ return yy::CExpEvalParser::token::EE_AS_FLOAT_NAN; }
"as_float_inf"						{ return yy::CExpEvalParser::token::EE_AS_FLOAT_INF; }
"as_float_subnorm_max"				{ return yy::CExpEvalParser::token::EE_AS_FLOAT_SUBNORM_MAX; }
"as_float_eps"						{ return yy::CExpEvalParser::token::EE_AS_FLOAT_EPS; }
"as_float_signbit"					{ return yy::CExpEvalParser::token::EE_AS_FLOAT_SIGNBIT; }
"as_float_expbits"					{ return yy::CExpEvalParser::token::EE_AS_FLOAT_EXPBITS; }
"as_float_manbits"					{ return yy::CExpEvalParser::token::EE_AS_FLOAT_MANBITS; }
"to_float"							{ return yy::CExpEvalParser::token::EE_TO_FLOAT32; }
"to_float32"						{ return yy::CExpEvalParser::token::EE_TO_FLOAT32; }
"to_double"							{ return yy::CExpEvalParser::token::EE_TO_FLOAT64; }
"to_float64"						{ return yy::CExpEvalParser::token::EE_TO_FLOAT64; }
"to_int8"							{ return yy::CExpEvalParser::token::EE_TO_INT8; }
"to_int16"							{ return yy::CExpEvalParser::token::EE_TO_INT16; }
"to_int32"							{ return yy::CExpEvalParser::token::EE_TO_INT32; }
"to_int64"							{ return yy::CExpEvalParser::token::EE_TO_INT64; }
"to_uint8"							{ return yy::CExpEvalParser::token::EE_TO_UINT8; }
"to_uint16"							{ return yy::CExpEvalParser::token::EE_TO_UINT16; }
"to_uint32"							{ return yy::CExpEvalParser::token::EE_TO_UINT32; }
"to_uint64"							{ return yy::CExpEvalParser::token::EE_TO_UINT64; }
"tofloat"							{ return yy::CExpEvalParser::token::EE_TO_FLOAT32; }
"tofloat32"							{ return yy::CExpEvalParser::token::EE_TO_FLOAT32; }
"todouble"							{ return yy::CExpEvalParser::token::EE_TO_FLOAT64; }
"tofloat64"							{ return yy::CExpEvalParser::token::EE_TO_FLOAT64; }
"toint8"							{ return yy::CExpEvalParser::token::EE_TO_INT8; }
"toint16"							{ return yy::CExpEvalParser::token::EE_TO_INT16; }
"toint32"							{ return yy::CExpEvalParser::token::EE_TO_INT32; }
"toint64"							{ return yy::CExpEvalParser::token::EE_TO_INT64; }
"touint8"							{ return yy::CExpEvalParser::token::EE_TO_UINT8; }
"touint16"							{ return yy::CExpEvalParser::token::EE_TO_UINT16; }
"touint32"							{ return yy::CExpEvalParser::token::EE_TO_UINT32; }
"touint64"							{ return yy::CExpEvalParser::token::EE_TO_UINT64; }
"tofloat16"							{ return yy::CExpEvalParser::token::EE_AS_FLOAT16; }
"tofloat14"							{ return yy::CExpEvalParser::token::EE_AS_FLOAT14; }
"tofloat11"							{ return yy::CExpEvalParser::token::EE_AS_FLOAT11; }
"tofloat10"							{ return yy::CExpEvalParser::token::EE_AS_FLOAT10; }

"clock"								{ return yy::CExpEvalParser::token::EE_CLOCK; }
"seconds"							{ return yy::CExpEvalParser::token::EE_SECONDS; }
"secs"								{ return yy::CExpEvalParser::token::EE_SECONDS; }
"milliseconds"						{ return yy::CExpEvalParser::token::EE_MILLISECONDS; }
"microseconds"						{ return yy::CExpEvalParser::token::EE_MICROSECONDS; }
"seconds_since_start"				{ return yy::CExpEvalParser::token::EE_SECONDS_SINCE_START; }
"secs_since_start"					{ return yy::CExpEvalParser::token::EE_SECONDS_SINCE_START; }
"milliseconds_since_start"			{ return yy::CExpEvalParser::token::EE_MILLISECONDS_SINCE_START; }
"mills_since_start"					{ return yy::CExpEvalParser::token::EE_MILLISECONDS_SINCE_START; }
"mils_since_start"					{ return yy::CExpEvalParser::token::EE_MILLISECONDS_SINCE_START; }
"ms_since_start"					{ return yy::CExpEvalParser::token::EE_MILLISECONDS_SINCE_START; }
"microseconds_since_start"			{ return yy::CExpEvalParser::token::EE_MICROSECONDS_SINCE_START; }
"micros_since_start"				{ return yy::CExpEvalParser::token::EE_MICROSECONDS_SINCE_START; }

{D}+{E}{FS}?						{ return yy::CExpEvalParser::token::EE_FLOAT_CONSTANT; }
{D}*"."{D}+({E})?{FS}?				{ return yy::CExpEvalParser::token::EE_FLOAT_CONSTANT; }
{D}+"."{D}*({E})?{FS}?				{ return yy::CExpEvalParser::token::EE_FLOAT_CONSTANT; }
{HP}{H}+{P}{FS}?					{ return yy::CExpEvalParser::token::EE_FLOAT_CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}?				{ return yy::CExpEvalParser::token::EE_FLOAT_CONSTANT; }
{HP}{H}+"."{P}{FS}?					{ return yy::CExpEvalParser::token::EE_FLOAT_CONSTANT; }

[bB]("["|"<:")						{ return yy::CExpEvalParser::token::EE_OB_BYTE; }
[wW]("["|"<:")						{ return yy::CExpEvalParser::token::EE_OB_WORD; }
[qQ]("["|"<:")						{ return yy::CExpEvalParser::token::EE_OB_QWORD; }
[fF]("["|"<:")						{ return yy::CExpEvalParser::token::EE_OB_FLOAT; }
("f16["|"f16<:")					{ return yy::CExpEvalParser::token::EE_OB_FLOAT16; }
("f32["|"f32<:")					{ return yy::CExpEvalParser::token::EE_OB_FLOAT; }
("f64["|"f64<:")					{ return yy::CExpEvalParser::token::EE_OB_DOUBLE; }
[dD]("["|"<:")						{ return yy::CExpEvalParser::token::EE_OB_DOUBLE; }
("u8["|"u8<:")						{ return yy::CExpEvalParser::token::EE_OB_BYTE; }
("ui8["|"u8<:")						{ return yy::CExpEvalParser::token::EE_OB_BYTE; }
("u16["|"u16<:")					{ return yy::CExpEvalParser::token::EE_OB_WORD; }
("ui16["|"u16<:")					{ return yy::CExpEvalParser::token::EE_OB_WORD; }
("u32["|"u32<:")					{ return yy::CExpEvalParser::token::EE_OB_DWORD; }
("ui32["|"u32<:")					{ return yy::CExpEvalParser::token::EE_OB_DWORD; }
("u64["|"u64<:")					{ return yy::CExpEvalParser::token::EE_OB_QWORD; }
("ui64["|"u64<:")					{ return yy::CExpEvalParser::token::EE_OB_QWORD; }
("i8["|"i8<:")						{ return yy::CExpEvalParser::token::EE_OB_SBYTE; }
("i16["|"i16<:")					{ return yy::CExpEvalParser::token::EE_OB_SWORD; }
("i32["|"i32<:")					{ return yy::CExpEvalParser::token::EE_OB_SDWORD; }
("i64["|"i64<:")					{ return yy::CExpEvalParser::token::EE_OB_SQWORD; }


("["|"<:")							{ return '['; }
("]"|":>")							{ return ']'; }
"("									{ return '('; }
")"									{ return ')'; }
"{"									{ return '{'; }
"}"									{ return '}'; }

"<<"								{ return yy::CExpEvalParser::token::EE_LEFT_OP; }
">>"								{ return yy::CExpEvalParser::token::EE_RIGHT_OP; }
"++"								{ return yy::CExpEvalParser::token::EE_PLUSPLUS; }
"--"								{ return yy::CExpEvalParser::token::EE_MINUSMINUS; }
"."									{ return '.'; }
","									{ return ','; }
"!"									{ return '!'; }
"~"									{ return '~'; }
"-"									{ return '-'; }
"+"									{ return '+'; }
"*"									{ return '*'; }
"/"									{ return '/'; }
"%"									{ return '%'; }
"<"									{ return '<'; }
">"									{ return '>'; }
"<="								{ return yy::CExpEvalParser::token::EE_REL_LE; }
">="								{ return yy::CExpEvalParser::token::EE_REL_GE; }
"&"									{ return '&'; }
"|"									{ return '|'; }
"^"									{ return '^'; }
"?"									{ return '?'; }
":"									{ return ':'; }
";"									{ return ';'; }
"="									{ return '='; }
"+="								{ return yy::CExpEvalParser::token::EE_ASS_PLUSEQUALS; }
"-="								{ return yy::CExpEvalParser::token::EE_ASS_MINUSEQUALS; }
"*="								{ return yy::CExpEvalParser::token::EE_ASS_TIMESEQUALS; }
"%="								{ return yy::CExpEvalParser::token::EE_ASS_MODEQUALS; }
"/="								{ return yy::CExpEvalParser::token::EE_ASS_DIVEQUALS; }
"^="								{ return yy::CExpEvalParser::token::EE_ASS_CARROTEQUALS; }
"<<="								{ return yy::CExpEvalParser::token::EE_ASS_SHLEFTEQUALS; }
">>="								{ return yy::CExpEvalParser::token::EE_ASS_SHRIGHTEQUALS; }
"|="								{ return yy::CExpEvalParser::token::EE_ASS_OREQUALS; }
"&="								{ return yy::CExpEvalParser::token::EE_ASS_ANDEQUALS; }
"??"								{ return yy::CExpEvalParser::token::EE_USER_VAR; }
"&&"								{ return yy::CExpEvalParser::token::EE_AND; }
"||"								{ return yy::CExpEvalParser::token::EE_OR; }
"=="								{ return yy::CExpEvalParser::token::EE_EQU_E; }
"!="								{ return yy::CExpEvalParser::token::EE_EQU_NE; }
"::"								{ return yy::CExpEvalParser::token::EE_MEMBERACCESS; }
"$"									{ return '$'; }


{L}({L}|{D})*						{ return yy::CExpEvalParser::token::EE_IDENTIFIER; }
"$"{D}+								{ return yy::CExpEvalParser::token::EE_PARM; }
"$$"								{ return yy::CExpEvalParser::token::EE_PARM_TOTAL; }

.									{ }
{WS}								{ }


%%


/*int yyFlexLexer::yywrap() {
	return 1;
}*/
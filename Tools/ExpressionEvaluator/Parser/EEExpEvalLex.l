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
"int8_t"							{ return yy::CExpEvalParser::token::EE_INT8; }
"int16_t"							{ return yy::CExpEvalParser::token::EE_INT16; }
"int32_t"							{ return yy::CExpEvalParser::token::EE_INT32; }
"int64_t"							{ return yy::CExpEvalParser::token::EE_INT64; }
"uint8_t"							{ return yy::CExpEvalParser::token::EE_UINT8; }
"uint16_t"							{ return yy::CExpEvalParser::token::EE_UINT16; }
"uint32_t"							{ return yy::CExpEvalParser::token::EE_UINT32; }
"uint64_t"							{ return yy::CExpEvalParser::token::EE_UINT64; }
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
"char"								{ return yy::CExpEvalParser::token::EE_INT8; }
"unsigned"{WS}+"char"				{ return yy::CExpEvalParser::token::EE_UINT8; }
"short"								{ return yy::CExpEvalParser::token::EE_INT16; }
"unsigned"{WS}+"short"				{ return yy::CExpEvalParser::token::EE_UINT16; }
"unsigned"{WS}+"int"				{ return sizeof( unsigned int ) == sizeof( uint32_t ) ? yy::CExpEvalParser::token::EE_UINT32 : yy::CExpEvalParser::token::EE_UINT64; }
"long"								{ return sizeof( long ) == sizeof( int32_t ) ? yy::CExpEvalParser::token::EE_INT32 : yy::CExpEvalParser::token::EE_INT64; }
"unsigned"{WS}+"long"				{ return sizeof( unsigned long ) == sizeof( uint32_t ) ? yy::CExpEvalParser::token::EE_UINT32 : yy::CExpEvalParser::token::EE_UINT64; }

"add"								{ return yy::CExpEvalParser::token::EE_ADD; }

(?i:_Pi_)							{ return yy::CExpEvalParser::token::EE_PI_; }
(?i:_HalfPi_)						{ return yy::CExpEvalParser::token::EE_HALFPI; }
(?i:_E_)							{ return yy::CExpEvalParser::token::EE_E; }
(?i:_Zeta_)							{ return yy::CExpEvalParser::token::EE_ZETA; }
(?i:_GR_)							{ return yy::CExpEvalParser::token::EE_GR; }
(?i:_GoldenRatio_)					{ return yy::CExpEvalParser::token::EE_GR; }
(?i:_GoldenR_)						{ return yy::CExpEvalParser::token::EE_GR; }
(?i:_GoldenRat_)					{ return yy::CExpEvalParser::token::EE_GR; }
(?i:_GRatio_)						{ return yy::CExpEvalParser::token::EE_GR; }
(?i:_Euler_)						{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:_EulerMascheroni_)				{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:_Mascheroni_)					{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:_EM_)							{ return yy::CExpEvalParser::token::EE_EULER; }
(?i:_K_)							{ return yy::CExpEvalParser::token::EE_K; }
(?i:_Khinchin_)						{ return yy::CExpEvalParser::token::EE_K; }
(?i:_A_)							{ return yy::CExpEvalParser::token::EE_A; }
(?i:_Glaisher_)						{ return yy::CExpEvalParser::token::EE_A; }
(?i:_GlaisherKinkelin_)				{ return yy::CExpEvalParser::token::EE_A; }
(?i:_Kinkelin_)						{ return yy::CExpEvalParser::token::EE_A; }
(?i:_M1_)							{ return yy::CExpEvalParser::token::EE_M1; }
(?i:_Meissel_)						{ return yy::CExpEvalParser::token::EE_M1; }
(?i:_MeisselMertens_)				{ return yy::CExpEvalParser::token::EE_M1; }
(?i:_Mertens_)						{ return yy::CExpEvalParser::token::EE_M1; }
(?i:_B_)							{ return yy::CExpEvalParser::token::EE_B; }
(?i:_Bernstein_)					{ return yy::CExpEvalParser::token::EE_B; }
(?i:_Lamda_)						{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_Lambda_)						{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_GWK_)							{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_Gauss_)						{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_Kuzmin_)						{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_Wirsing_)						{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_GaussKuzminWirsing_)			{ return yy::CExpEvalParser::token::EE_GWK; }
(?i:_Sigma_)						{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_HSMC_)							{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_HSM_)							{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_Hafner_)						{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_Sarnak_)						{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_McCurley_)						{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_HafnerSarnakMcCurley_)			{ return yy::CExpEvalParser::token::EE_HSMC; }
(?i:_Omega_)						{ return yy::CExpEvalParser::token::EE_OMEGA; }
(?i:_Mu_)							{ return yy::CExpEvalParser::token::EE_MU; }
(?i:_Golomb_)						{ return yy::CExpEvalParser::token::EE_MU; }
(?i:_Dickman_)						{ return yy::CExpEvalParser::token::EE_MU; }
(?i:_GolombDickman_)				{ return yy::CExpEvalParser::token::EE_MU; }
(?i:_GD_)							{ return yy::CExpEvalParser::token::EE_MU; }
(?i:_Cahen_)						{ return yy::CExpEvalParser::token::EE_CAHEN; }
(?i:_C2_)							{ return yy::CExpEvalParser::token::EE_C2; }
(?i:_TwinPrime_)					{ return yy::CExpEvalParser::token::EE_C2; }
(?i:_TP_)							{ return yy::CExpEvalParser::token::EE_C2; }
(?i:_Laplace_)						{ return yy::CExpEvalParser::token::EE_LAPLACE; }
(?i:_Beta_)							{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:_Embree_)						{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:_Trefethen_)					{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:_EmbreeTrefethen_)				{ return yy::CExpEvalParser::token::EE_BETA; }
(?i:_Landau_)						{ return yy::CExpEvalParser::token::EE_RAMAN; }
(?i:_LandauRamanujan_)				{ return yy::CExpEvalParser::token::EE_RAMAN; }
(?i:_Alladi_)						{ return yy::CExpEvalParser::token::EE_ALLADI; }
(?i:_Grinstead_)					{ return yy::CExpEvalParser::token::EE_ALLADI; }
(?i:_AlladiGrinstead_)				{ return yy::CExpEvalParser::token::EE_ALLADI; }
(?i:_B4_)							{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_Brun_)							{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_BrunPrimeQuadruplets_)			{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_BrunPQ_)						{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_BrunPrimeQuad_)				{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_BrunPrimeQ_)					{ return yy::CExpEvalParser::token::EE_B4; }
(?i:_Catalan_)						{ return yy::CExpEvalParser::token::EE_CATALAN; }
(?i:_Lengyel_)						{ return yy::CExpEvalParser::token::EE_LAMBDA; }
(?i:_Leng_)							{ return yy::CExpEvalParser::token::EE_LAMBDA; }
(?i:_Viswanath_)					{ return yy::CExpEvalParser::token::EE_VISW; }
(?i:_Viswan_)						{ return yy::CExpEvalParser::token::EE_VISW; }
(?i:_Vis_)							{ return yy::CExpEvalParser::token::EE_VISW; }
(?i:_Zeta3_)						{ return yy::CExpEvalParser::token::EE_Z3; }
(?i:_Apery_)						{ return yy::CExpEvalParser::token::EE_Z3; }
(?i:_Conway_)						{ return yy::CExpEvalParser::token::EE_CONWAY; }
(?i:_Theta_)						{ return yy::CExpEvalParser::token::EE_THETA; }
(?i:_Mill_)							{ return yy::CExpEvalParser::token::EE_THETA; }
(?i:_Rho_)							{ return yy::CExpEvalParser::token::EE_PLASTIC; }
(?i:_Plastic_)						{ return yy::CExpEvalParser::token::EE_PLASTIC; }
(?i:_Ramanujan_)					{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_Raman_)						{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_RamaMu_)						{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_Soldner_)						{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_RamanujanSoldner_)				{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_RS_)							{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_RamaSoldner_)					{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_RamaSol_)						{ return yy::CExpEvalParser::token::EE_RAMAMU; }
(?i:_Blackhouse_)					{ return yy::CExpEvalParser::token::EE_BH; }
(?i:_BH_)							{ return yy::CExpEvalParser::token::EE_BH; }
(?i:_Porter_)						{ return yy::CExpEvalParser::token::EE_PORTER; }
(?i:_Lieb_)							{ return yy::CExpEvalParser::token::EE_ICE; }
(?i:_SquareIce_)					{ return yy::CExpEvalParser::token::EE_ICE; }
(?i:_SqIce_)						{ return yy::CExpEvalParser::token::EE_ICE; }
(?i:_EB_)							{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:_Erdos_)						{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:_Borwein_)						{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:_ErdosBorwein_)					{ return yy::CExpEvalParser::token::EE_ERDOS; }
(?i:_Niven_)						{ return yy::CExpEvalParser::token::EE_NIVEN; }
(?i:_Niv_)							{ return yy::CExpEvalParser::token::EE_NIVEN; }
(?i:_B2_)							{ return yy::CExpEvalParser::token::EE_B2; }
(?i:_Brun2_)						{ return yy::CExpEvalParser::token::EE_B2; }
(?i:_BrunTwinPrime_)				{ return yy::CExpEvalParser::token::EE_B2; }
(?i:_BrunTP_)						{ return yy::CExpEvalParser::token::EE_B2; }
(?i:_P2_)							{ return yy::CExpEvalParser::token::EE_P2; }
(?i:_Parabolic_)					{ return yy::CExpEvalParser::token::EE_P2; }
(?i:_Alpha_)						{ return yy::CExpEvalParser::token::EE_ALPHA; }
(?i:_Feigenbaum_)					{ return yy::CExpEvalParser::token::EE_ALPHA; }
(?i:_FeigenbaumA_)					{ return yy::CExpEvalParser::token::EE_ALPHA; }
(?i:_Sierp_)						{ return yy::CExpEvalParser::token::EE_SIERP; }
(?i:_Sierpinski_)					{ return yy::CExpEvalParser::token::EE_SIERP; }
(?i:_F_)							{ return yy::CExpEvalParser::token::EE_F; }
(?i:_Fransen_)						{ return yy::CExpEvalParser::token::EE_F; }
(?i:_Robinson_)						{ return yy::CExpEvalParser::token::EE_F; }
(?i:_FransenRobinson_)				{ return yy::CExpEvalParser::token::EE_F; }
(?i:_Levy_)							{ return yy::CExpEvalParser::token::EE_LEVY; }
(?i:_Psi_)							{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:_ReciprocalFibonacci_)			{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:_RecipFibonacci_)				{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:_RFibonacci_)					{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:_RF_)							{ return yy::CExpEvalParser::token::EE_PSI; }
(?i:_Delta_)						{ return yy::CExpEvalParser::token::EE_DELTA; }
(?i:_FeigenbaumD_)					{ return yy::CExpEvalParser::token::EE_DELTA; }

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
"_byteswap16"						{ return yy::CExpEvalParser::token::EE_BYTESWAPUSHORT; }
"_byteswap_16"						{ return yy::CExpEvalParser::token::EE_BYTESWAPUSHORT; }
"_byteswap_uint16"					{ return yy::CExpEvalParser::token::EE_BYTESWAPUSHORT; }
"_byteswap_32"						{ return yy::CExpEvalParser::token::EE_BYTESWAPULONG; }
"_byteswap32"						{ return yy::CExpEvalParser::token::EE_BYTESWAPULONG; }
"_byteswap_uint32"					{ return yy::CExpEvalParser::token::EE_BYTESWAPULONG; }
"_byteswap_64"						{ return yy::CExpEvalParser::token::EE_BYTESWAPUINT64; }
"_byteswap64"						{ return yy::CExpEvalParser::token::EE_BYTESWAPUINT64; }
"_byteswap_uint64"					{ return yy::CExpEvalParser::token::EE_BYTESWAPUINT64; }
"_byteswap_ushort"					{ return yy::CExpEvalParser::token::EE_BYTESWAPUSHORT; }
"_byteswap_ulong"					{ return yy::CExpEvalParser::token::EE_BYTESWAPULONG; }
"__rbit8"							{ return yy::CExpEvalParser::token::EE_BITSWAP8; }
"__rbit_8"							{ return yy::CExpEvalParser::token::EE_BITSWAP8; }
"__rbit_uint8"						{ return yy::CExpEvalParser::token::EE_BITSWAP8; }
"__rbit16"							{ return yy::CExpEvalParser::token::EE_BITSWAP16; }
"__rbit_16"							{ return yy::CExpEvalParser::token::EE_BITSWAP16; }
"__rbit_uint16"						{ return yy::CExpEvalParser::token::EE_BITSWAP16; }
"__rbit32"							{ return yy::CExpEvalParser::token::EE_BITSWAP32; }
"__rbit_32"							{ return yy::CExpEvalParser::token::EE_BITSWAP32; }
"__rbit_uint32"						{ return yy::CExpEvalParser::token::EE_BITSWAP32; }
"__rbit64"							{ return yy::CExpEvalParser::token::EE_BITSWAP64; }
"__rbit_64"							{ return yy::CExpEvalParser::token::EE_BITSWAP64; }
"__rbit_uint64"						{ return yy::CExpEvalParser::token::EE_BITSWAP64; }
"__rbit_ushort"						{ return yy::CExpEvalParser::token::EE_BITSWAP16; }
"__rbit_ulong"						{ return yy::CExpEvalParser::token::EE_BITSWAP32; }
"__rbit_f16"						{ return yy::CExpEvalParser::token::EE_BITSWAPF16; }
"__rbit_f32"						{ return yy::CExpEvalParser::token::EE_BITSWAPF32; }
"__rbit_f64"						{ return yy::CExpEvalParser::token::EE_BITSWAPF64; }
"__rbitf16"							{ return yy::CExpEvalParser::token::EE_BITSWAPF16; }
"__rbitf32"							{ return yy::CExpEvalParser::token::EE_BITSWAPF32; }
"__rbitf64"							{ return yy::CExpEvalParser::token::EE_BITSWAPF64; }
"__rbit"							{ return yy::CExpEvalParser::token::EE_BITSWAP32; }

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

"append"							{ return yy::CExpEvalParser::token::EE_APPEND; }
"assign"							{ return yy::CExpEvalParser::token::EE_ASSIGN; }
"at"								{ return yy::CExpEvalParser::token::EE_AT; }
"capacity"							{ return yy::CExpEvalParser::token::EE_CAPACITY; }
"clear"								{ return yy::CExpEvalParser::token::EE_CLEAR; }
"cross"								{ return yy::CExpEvalParser::token::EE_CROSS; }
"dot"								{ return yy::CExpEvalParser::token::EE_DOT; }
"empty"								{ return yy::CExpEvalParser::token::EE_EMPTY; }
"erase"								{ return yy::CExpEvalParser::token::EE_ERASE; }
"insert"							{ return yy::CExpEvalParser::token::EE_INSERT; }
"length"							{ return yy::CExpEvalParser::token::EE_SIZE; }
"max_size"							{ return yy::CExpEvalParser::token::EE_MAX_SIZE; }
"mag"								{ return yy::CExpEvalParser::token::EE_MAG; }
"mag_sq"							{ return yy::CExpEvalParser::token::EE_MAGSQ; }
"mul"								{ return yy::CExpEvalParser::token::EE_MUL; }
"normalize"							{ return yy::CExpEvalParser::token::EE_NORMALIZE; }
"reserve"							{ return yy::CExpEvalParser::token::EE_RESERVE; }
"resize"							{ return yy::CExpEvalParser::token::EE_RESIZE; }
"pop_back"							{ return yy::CExpEvalParser::token::EE_POP_BACK; }
"push_back"							{ return yy::CExpEvalParser::token::EE_PUSH_BACK; }
"shrink_to_fit"						{ return yy::CExpEvalParser::token::EE_SHRINK_TO_FIT; }
"size"								{ return yy::CExpEvalParser::token::EE_SIZE; }
"sub"								{ return yy::CExpEvalParser::token::EE_SUB; }
"sum"								{ return yy::CExpEvalParser::token::EE_SUM; }
"swap"								{ return yy::CExpEvalParser::token::EE_SWAP; }
"tokenize"							{ return yy::CExpEvalParser::token::EE_TOKENIZE; }

"barthann"							{ return yy::CExpEvalParser::token::EE_BARTHANN; }
"bartlett"							{ return yy::CExpEvalParser::token::EE_BARTLETT; }
"blackman"							{ return yy::CExpEvalParser::token::EE_BLACKMAN; }
"blackmanharris"					{ return yy::CExpEvalParser::token::EE_BLACKMANHARRIS; }
"blackmannuttall"					{ return yy::CExpEvalParser::token::EE_BLACKMANNUTTAL; }
"bohman"							{ return yy::CExpEvalParser::token::EE_BOHMAN; }
"boxcar"							{ return yy::CExpEvalParser::token::EE_BOXCAR; }
"chebwin"							{ return yy::CExpEvalParser::token::EE_CHEBWIN; }
"cosine"							{ return yy::CExpEvalParser::token::EE_COSINE; }
"exponential"						{ return yy::CExpEvalParser::token::EE_EXPONENTIAL; }
"flattop"							{ return yy::CExpEvalParser::token::EE_FLATTOP; }
"gaussian"							{ return yy::CExpEvalParser::token::EE_GAUSSIAN; }
"general_cosine"					{ return yy::CExpEvalParser::token::EE_GENERAL_COSINE; }
"general_gaussian"					{ return yy::CExpEvalParser::token::EE_GENERAL_GUASSIAN; }
"general_hamming"					{ return yy::CExpEvalParser::token::EE_GENERAL_HAMMING; }
"hamming"							{ return yy::CExpEvalParser::token::EE_HAMMING; }
"hann"								{ return yy::CExpEvalParser::token::EE_HANN; }
"hanning"							{ return yy::CExpEvalParser::token::EE_HANN; }
"kaiser"							{ return yy::CExpEvalParser::token::EE_KAISER; }
"kaiser_bessel_derived"				{ return yy::CExpEvalParser::token::EE_KAISER_BESSEL; }
"lanczos"							{ return yy::CExpEvalParser::token::EE_LANCZOS; }
"nuttall"							{ return yy::CExpEvalParser::token::EE_NUTTAL; }
"parzen"							{ return yy::CExpEvalParser::token::EE_PARZEN; }
"sinc"								{ return yy::CExpEvalParser::token::EE_SINC; }
"taylor"							{ return yy::CExpEvalParser::token::EE_TAYLOR; }
"triang"							{ return yy::CExpEvalParser::token::EE_TRIANG; }
"tukey"								{ return yy::CExpEvalParser::token::EE_TUKEY; }

"trapezoid"							{ return yy::CExpEvalParser::token::EE_TRAPEZOID; }
"cumulative_trapezoid"				{ return yy::CExpEvalParser::token::EE_CUM_TRAPEZOID; }
"simpson"							{ return yy::CExpEvalParser::token::EE_SIMPSON; }
"cumulative_simpson"				{ return yy::CExpEvalParser::token::EE_CUM_SIMPSON; }
"romb"								{ return yy::CExpEvalParser::token::EE_ROMB; }

"arange"							{ return yy::CExpEvalParser::token::EE_ARANGE; }
"full"								{ return yy::CExpEvalParser::token::EE_FULL; }
"full_like"							{ return yy::CExpEvalParser::token::EE_FULL_LIKE; }
"geomspace"							{ return yy::CExpEvalParser::token::EE_GEOMSPACE; }
"linspace"							{ return yy::CExpEvalParser::token::EE_LINSPACE; }
"logspace"							{ return yy::CExpEvalParser::token::EE_LOGSPACE; }
"ones"								{ return yy::CExpEvalParser::token::EE_ONES; }
"ones_like"							{ return yy::CExpEvalParser::token::EE_ONES_LIKE; }
"zeros"								{ return yy::CExpEvalParser::token::EE_ZEROS; }
"zeros_like"						{ return yy::CExpEvalParser::token::EE_ZEROS_LIKE; }


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

"#"{D}+{IS}?						{ return yy::CExpEvalParser::token::EE_PUREDEC_CONSTANT; }
0[bB]{B}+{IS}?						{ return yy::CExpEvalParser::token::EE_BIN_CONSTANT; }
0[oO]{O}+{IS}?						{ return yy::CExpEvalParser::token::EE_OCT_CONSTANT; }
0[xX]{H}+{IS}?						{ return yy::CExpEvalParser::token::EE_HEX_CONSTANT1; }
{H}+{IS}?[hH]						{ return yy::CExpEvalParser::token::EE_HEX_CONSTANT2; }
0{O}+{IS}?							{ return yy::CExpEvalParser::token::EE_OCT_CONSTANT; }
{D}+{IS}?							{ return yy::CExpEvalParser::token::EE_DEC_CONSTANT; }
{H}+{IS}?							{ return yy::CExpEvalParser::token::EE_HEX_CONSTANT3; }

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
("{"|"<%")							{ return '{'; }
("}"|"%>")							{ return '}'; }

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
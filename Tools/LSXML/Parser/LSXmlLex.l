%option 				yylineno noyywrap 8bit nodefault


%{
/*
 * WARNING: This is not an XML 1.0 parser, but an experiment with an XML-like
 * language. See http://www.w3.org/XML/9707/XML-in-C
 *
 * Author: Bert Bos <bert@w3.org>
 * Created: 9 July 1997
 *
 * Copyright © 1997-2000 World Wide Web Consortium
 * See http://www.w3.org/Consortium/Legal/copyright-software-19980720.html
 */

#include "../LSXmlSyntaxNodes.h"
#include "../LSXmlContainer.h"
#include "../LSXmlLexer.h"
#include "LSXmlParser.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YY_NEVER_INTERACTIVE		1

static int keep;					/* To store start condition */

%}


nl									(\r\n|\r|\n)
ws									[ \t\r\n]+
open								{nl}?"<"
close								">"{nl}?
namestart							[A-Za-z\200-\377_]
namechar							[A-Za-z\200-\377_0-9.:-]
esc									"&#"[0-9]+";"|"&#x"[0-9a-fA-F]+";"
name								{namestart}{namechar}*
data								([^<\n&]|\n[^<&]|\n{esc}|{esc})+
comment								{open}"!--"([^-]|"-"[^-])*"--"{close}
string								\"([^"&]|{esc})*\"|\'([^'&]|{esc})*\'
version								{open}"?XML-VERSION 1.0?"{close}
encoding							{open}"?XML-ENCODING"{ws}{name}{ws}?"?"{close}
attdef								{open}"?XML-ATT"

/*
 * The CONTENT mode is used for the content of elements, i.e.,
 * between the ">" and "<" of element tags.
 * The INITIAL mode is used outside the top level element
 * and inside markup.
 */

%s CONTENT


%%

<INITIAL>{ws}						{ /* skip */ }
<INITIAL>{version}					{ return yy::CXmlParser::token::LSX_VERSION; }
<INITIAL>{encoding}					{ return yy::CXmlParser::token::LSX_ENCODING; }
<INITIAL>"/"						{ return yy::CXmlParser::token::LSX_SLASH; }
<INITIAL>"="						{ return yy::CXmlParser::token::LSX_EQ; }
<INITIAL>{close}					{ BEGIN(CONTENT); return yy::CXmlParser::token::LSX_CLOSE; }
<INITIAL>{name}						{ return yy::CXmlParser::token::LSX_NAME; }
<INITIAL>{string}					{ return yy::CXmlParser::token::LSX_VALUE; }
<INITIAL>"?"{close}					{ BEGIN(keep); return yy::CXmlParser::token::LSX_ENDDEF; }

{attdef}							{ keep = YY_START; BEGIN(INITIAL); return yy::CXmlParser::token::LSX_ATTDEF; }
{open}{ws}?{name}					{ BEGIN(INITIAL); return yy::CXmlParser::token::LSX_START; }
{open}{ws}?"/"						{ BEGIN(INITIAL); return yy::CXmlParser::token::LSX_END; }
{comment}							{ return yy::CXmlParser::token::LSX_COMMENT; }

<CONTENT>{data}						{ return yy::CXmlParser::token::LSX_DATA; }

.									{ fprintf(stderr, "!ERROR(%c)\n", *yytext); }
{nl}								{ /* skip, must be an extra one at EOF */; }

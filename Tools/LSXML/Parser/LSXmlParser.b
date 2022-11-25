%{
/*
 * WARNING: This is not an XML 1.0 parser, but an experiment with an XML-like
 * language. See http://www.w3.org/XML/9707/XML-in-C
 *
 * Author: Bert Bos <bert@w3.org>
 * Created: 9 July 1997
 *
 * Copyright © 1997-2004 World Wide Web Consortium
 * See http://www.w3.org/Consortium/Legal/copyright-software-19980720.html
 */

#include "../LSXml.h"
#include "../LSXmlContainer.h"
#include "../LSXmlLexer.h"
#include <string>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YYINCLUDED_STDLIB_H

//extern char yytext[];
// Announce to Flex the prototype we want for lexing function.
extern int yylex( /*YYSTYPE*/void * _pvNodeUnion, lsx::CXmlLexer * _pxlLexer );

#include "../LSXmlSyntaxNodes.h"

%}

%pure-parser

%parse-param												{ class CXmlLexer * m_peelLexer }
%parse-param												{ class CXmlContainer * m_peecContainer }
%lex-param													{ CXmlLexer * m_peelLexer }

%union {char *s;}

%token VERSION ATTDEF ENDDEF EQ SLASH CLOSE END
%token <s> ENCODING NAME VALUE DATA COMMENT START
%type <s> name_opt

%%

document
 : prolog element misc_seq_opt
 ;
prolog
 : version_opt encoding_opt
   misc_seq_opt
 ;
version_opt
 : VERSION													{ }
 | /*empty*/
 ;
encoding_opt
 : ENCODING													{ }
 | /*empty*/
 ;
misc_seq_opt
 : misc_seq_opt misc
 | /*empty*/
 ;
misc
 : COMMENT													{ }
 | attribute_decl
 ;
attribute_decl
 : ATTDEF NAME												{ }
   attribute_seq_opt ENDDEF									{ }
 ;	
element
 : START													{ }
   attribute_seq_opt
   empty_or_content
 ;
empty_or_content
 : SLASH CLOSE												{ }
 | CLOSE													{ }
   content END name_opt CLOSE								{ }
 ;
content
 : content DATA												{ }
 | content misc
 | content element
 | /*empty*/
 ;
name_opt
 : NAME														{ }
 | /*empty*/												{ }
 ;
attribute_seq_opt
 : attribute_seq_opt attribute
 | /*empty*/
 ;
attribute
 : NAME														{ }
 | NAME EQ VALUE											{ }
 ;


%%

int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, lsx::CXmlLexer * _pxlLexer ) {
	return _pxlLexer->yylex();
}

void yy::parser::error( const yy::location &/*_lLoc*/, const std::string &/*_strM*/ ) {

}
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


%token LSX_VERSION LSX_ATTDEF LSX_ENDDEF LSX_EQ LSX_SLASH LSX_CLOSE LSX_END
%token LSX_ENCODING LSX_NAME LSX_VALUE LSX_DATA LSX_COMMENT LSX_START

%type <sStringIndex>										data
%type <sStringIndex>										name
%type <sStringIndex>										name_opt
%type <sStringIndex>										start
%type <sStringIndex>										value
%type <nNode>												attribute
%type <nNode>												attribute_seq_opt
%type <nNode>												attribute_decl
%type <nNode>												content
%type <nNode>												document
%type <nNode>												element
%type <nNode>												empty_or_content
%type <nNode>												encoding_opt
%type <nNode>												misc
%type <nNode>												misc_seq_opt
%type <nNode>												prolog
%type <nNode>												version_opt

%start document

%%

name
 : LSX_NAME													{ $$ = m_peecContainer->AddString( m_peelLexer->YYText() ); }
 ;
 
value
 : LSX_VALUE												{ $$ = m_peecContainer->AddValue( m_peelLexer->YYText() ); }
 ;
 
data
 : LSX_DATA													{ $$ = m_peecContainer->AddString( m_peelLexer->YYText() ); }
 ;
 
start
 : LSX_START												{ $$ = m_peecContainer->AddAttributeStart( m_peelLexer->YYText() ); }
 ;

document
 : prolog element misc_seq_opt								{ m_peecContainer->AddDocument( $$, $1, $2, $3 ); }
 ;
prolog
 : version_opt encoding_opt
   misc_seq_opt												{ m_peecContainer->AddProlog( $$, $1, $2, $3 ); }
 ;
version_opt
 : LSX_VERSION												{ m_peecContainer->AddVersion( $$, m_peelLexer->YYText() ); }
 |															{ m_peecContainer->AddEmpty( $$ ); }
 ;
encoding_opt
 : LSX_ENCODING												{ m_peecContainer->AddEncoding( $$, m_peelLexer->YYText() ); }
 | 															{ m_peecContainer->AddEmpty( $$ ); }
 ;
misc_seq_opt
 : misc_seq_opt misc										{ m_peecContainer->AddMiscSeq( $$, $1, $2 ); }
 | 															{ m_peecContainer->AddEmpty( $$ ); }
 ;
misc
 : LSX_COMMENT												{ m_peecContainer->AddEmpty( $$ ); }
 | attribute_decl											{ $$ = $1; }
 ;
attribute_decl
 : LSX_ATTDEF name
   attribute_seq_opt LSX_ENDDEF								{ m_peecContainer->AddAttributeDecl( $$, $2, $3 ); }
 ;	
element
 : start
   attribute_seq_opt
   empty_or_content											{ m_peecContainer->AddElement( $$, $1, $2, $3 ); }
 ;
empty_or_content
 : LSX_SLASH LSX_CLOSE										{ m_peecContainer->AddEmpty( $$ ); }
 | LSX_CLOSE
   content LSX_END name_opt LSX_CLOSE						{ m_peecContainer->AddContent( $$, $2, $4 ); }
 ;
content
 : content data												{ m_peecContainer->AddContentData( $$, $1, $2 ); }
 | content misc												{ m_peecContainer->AddContentMisc( $$, $1, $2 ); }
 | content element											{ m_peecContainer->AddContentElement( $$, $1, $2 ); }
 | 															{ m_peecContainer->AddEmpty( $$ ); }
 ;
name_opt
 : name														{ $$ = $1; }
 | /*empty*/												{ $$ = size_t( -1 ); }
 ;
attribute_seq_opt
 : attribute_seq_opt attribute								{ m_peecContainer->AddAttributeList( $$, $1, $2 ); }
 | 															{ m_peecContainer->AddEmpty( $$ ); }
 ;
attribute
 : name														{ m_peecContainer->AddAttribute( $$, $1 ); }
 | name LSX_EQ value										{ m_peecContainer->AddAttribute( $$, $1, $3 ); }
 ;


%%

int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, lsx::CXmlLexer * _pxlLexer ) {
	return _pxlLexer->yylex();
}

void yy::parser::error( const yy::location &/*_lLoc*/, const std::string &/*_strM*/ ) {

}
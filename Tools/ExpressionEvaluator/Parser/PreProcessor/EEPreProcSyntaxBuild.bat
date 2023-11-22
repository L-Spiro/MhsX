bison -ld --skeleton=lalr1.cc -o "EEPreProcParser.cpp" "EEPreProcParser.b"

copy EEPreProcParser.cpp "../../Src/PreProcGen/EEPreProcParser.cpp"
copy EEPreProcParser.hpp "../../Src/PreProcGen/EEPreProcParser.h"
copy location.hh "../../Src/PreProcGen/EEPreProcParserLocation.h"
copy position.hh "../../Src/PreProcGen/EEPreProcParserPosition.h"
copy stack.hh "../../Src/PreProcGen/EEPreProcParserStack.h"

del location.hh
del position.hh
del stack.hh

LSUtility.exe "../../Src/PreProcGen/EEPreProcParser.cpp" -r \r "" -r \n \r\n
LSUtility.exe "../../Src/PreProcGen/EEPreProcParser.cpp" -r EEPreProcParser.hpp EEPreProcParser.h -r parser CPreProcParser
LSUtility.exe "../../Src/PreProcGen/EEPreProcParser.cpp" -r m_ppplLexer_yyarg _ppplLexer -r m_pslpLexer_yyarg _pslpLexer -r "\"syntax error\"" "\"Syntax error.\"" -r "\""syntax error," "\""Syntax error,"
LSUtility.exe "../../Src/PreProcGen/EEPreProcParser.h" -r "#include <string>" "" -r "#include <iostream>" "" -r parser CPreProcParser
LSUtility.exe "../../Src/PreProcGen/EEPreProcParser.h" -r m_ppplLexer_yyarg _ppplLexer -r m_pslpLexer_yyarg _pslpLexer
LSUtility.exe "../../Src/PreProcGen/EEPreProcParser.h" -r location.hh EEPreProcParserLocation.h -r position.hh EEPreProcParserPosition.h -r stack.hh EEPreProcParserStack.h
LSUtility.exe "../../Src/PreProcGen/EEPreProcParser.h" -r iff if -r PARSER_HEADER_H __EE_PREPROCPARSER_H__
LSUtility.exe "../../Src/PreProcGen/EEPreProcParserLocation.h" -r \r "" -r \n \r\n -r position.hh EEPreProcParserPosition.h -r BISON_LOCATION_HH __EE_EXPEVALPARSERLOCATION_H__ -r YY_YY_LOCATION_HH_INCLUDED __EE_EXPEVALPARSERLOCATION_H__
LSUtility.exe "../../Src/PreProcGen/EEPreProcParserPosition.h" -r \r "" -r \n \r\n -r std::max max -r BISON_POSITION_HH __EE_EXPEVALPARSERPOSITION_H__ -r YY_YY_POSITION_HH_INCLUDED __EE_EXPEVALPARSERPOSITION_H__ -r "\r\nnamespace yy" "using namespace std;\r\n\r\nnamespace yy"
LSUtility.exe "../../Src/PreProcGen/EEPreProcParserStack.h" -r \r "" -r \n \r\n -r "unsigned int*    height" "size_t height" -r BISON_STACK_HH __EE_EXPEVALPARSERSTACK_H__ -r YY_YY_STACK_HH_INCLUDED __EE_EXPEVALPARSERSTACK_H__
set s1="deque<T>>"
:: set s1=deque^<T^>^>
set s2="deque<T> >"
:: set s2=deque^<T^> ^>
LSUtility.exe "../../Src/PreProcGen/EEPreProcParserStack.h" -r %s1% %s2%

set s3="stack<T>>"
:: set s3=stack^<T^>^>
set s4="stack<T> >"
:: set s4=stack^<T^> ^>
LSUtility.exe "../../Src/PreProcGen/EEPreProcParserStack.h" -r %s3% %s4%


:: Below are just to make the code look pretty.  If this causes an error in a future version of Bison, remove it.
LSUtility.exe "../../Src/PreProcGen/EEPreProcParserLocation.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../../Src/PreProcGen/EEPreProcParserPosition.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../../Src/PreProcGen/EEPreProcParserStack.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
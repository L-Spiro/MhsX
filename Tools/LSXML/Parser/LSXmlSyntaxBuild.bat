bison -ld --skeleton=lalr1.cc -o "LSXmlParser.cpp" "LSXmlParser.b"

copy LSXmlParser.cpp "../Src/Gen/LSXmlParser.cpp"
copy LSXmlParser.hpp "../Src/Gen/LSXmlParser.h"
copy location.hh "../Src/Gen/LSXmlParserLocation.h"
copy position.hh "../Src/Gen/LSXmlParserPosition.h"
copy stack.hh "../Src/Gen/LSXmlParserStack.h"

del location.hh
del position.hh
del stack.hh

LSUtility.exe "../Src/Gen/LSXmlParser.cpp" -r \r "" -r \n \r\n
LSUtility.exe "../Src/Gen/LSXmlParser.cpp" -r LSXmlParser.hpp LSXmlParser.h -r parser CXmlParser
LSUtility.exe "../Src/Gen/LSXmlParser.cpp" -r m_pslLexer_yyarg _pslLexer -r m_pslpLexer_yyarg _pslpLexer -r "\"syntax error\"" "\"Syntax error.\"" -r "\""syntax error," "\""Syntax error,"
LSUtility.exe "../Src/Gen/LSXmlParser.h" -r "#include <string>" "" -r "#include <iostream>" "" -r parser CXmlParser
LSUtility.exe "../Src/Gen/LSXmlParser.h" -r m_pslLexer_yyarg _pslLexer -r m_pslpLexer_yyarg _pslpLexer
LSUtility.exe "../Src/Gen/LSXmlParser.h" -r location.hh LSXmlParserLocation.h -r position.hh LSXmlParserPosition.h -r stack.hh LSXmlParserStack.h
LSUtility.exe "../Src/Gen/LSXmlParser.h" -r iff if -r PARSER_HEADER_H __LSX_XML_PARSER_H__
LSUtility.exe "../Src/Gen/LSXmlParserLocation.h" -r \r "" -r \n \r\n -r position.hh LSXmlParserPosition.h -r BISON_LOCATION_HH __EE_EXPEVALPARSERLOCATION_H__
LSUtility.exe "../Src/Gen/LSXmlParserPosition.h" -r \r "" -r \n \r\n -r std::max max -r BISON_POSITION_HH __EE_EXPEVALPARSERPOSITION_H__ -r "\r\nnamespace yy" "using namespace std;\r\n\r\nnamespace yy"
LSUtility.exe "../Src/Gen/LSXmlParserStack.h" -r \r "" -r \n \r\n -r "unsigned int*    height" "size_t height" -r BISON_STACK_HH __EE_EXPEVALPARSERSTACK_H__

:: set s1=deque^<T^>^>
:: set s2=deque^<T^> ^>
:: LSUtility.exe "../Src/Gen/LSXmlParserStack.h" -r %s1% %s2%
LSUtility.exe "../Src/Gen/LSXmlParserStack.h" -r "deque<T>>" "deque<T> >"


:: set s3=stack^<T^>^>
:: set s4=stack^<T^> ^>
:: LSUtility.exe "../Src/Gen/LSXmlParserStack.h" -r %s3% %s4%
LSUtility.exe "../Src/Gen/LSXmlParserStack.h" -r "stack<T>>" "stack<T> >"

:: Below are just to make the code look pretty.  If this causes an error in a future version of Bison, remove it.
LSUtility.exe "../Src/Gen/LSXmlParserLocation.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../Src/Gen/LSXmlParserPosition.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../Src/Gen/LSXmlParserStack.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"

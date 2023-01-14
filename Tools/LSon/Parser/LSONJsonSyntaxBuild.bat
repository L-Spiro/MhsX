bison -ld --skeleton=lalr1.cc -o "LSONJsonParser.cpp" "LSONJsonParser.b"

copy LSONJsonParser.cpp "../Src/Gen/LSONJsonParser.cpp"
copy LSONJsonParser.hpp "../Src/Gen/LSONJsonParser.h"
copy location.hh "../Src/Gen/LSONJsonParserLocation.h"
copy position.hh "../Src/Gen/LSONJsonParserPosition.h"
copy stack.hh "../Src/Gen/LSONJsonParserStack.h"

del location.hh
del position.hh
del stack.hh

LSUtility.exe "../Src/Gen/LSONJsonParser.cpp" -r \r "" -r \n \r\n
LSUtility.exe "../Src/Gen/LSONJsonParser.cpp" -r LSONJsonParser.hpp LSONJsonParser.h -r parser CJsonParser
LSUtility.exe "../Src/Gen/LSONJsonParser.cpp" -r m_pslLexer_yyarg _pslLexer -r m_pslpLexer_yyarg _pslpLexer -r "\"syntax error\"" "\"Syntax error.\"" -r "\""syntax error," "\""Syntax error,"
LSUtility.exe "../Src/Gen/LSONJsonParser.h" -r "#include <string>" "" -r "#include <iostream>" "" -r parser CJsonParser
LSUtility.exe "../Src/Gen/LSONJsonParser.h" -r m_pslLexer_yyarg _pslLexer -r m_pslpLexer_yyarg _pslpLexer
LSUtility.exe "../Src/Gen/LSONJsonParser.h" -r location.hh LSONJsonParserLocation.h -r position.hh LSONJsonParserPosition.h -r stack.hh LSONJsonParserStack.h
LSUtility.exe "../Src/Gen/LSONJsonParser.h" -r iff if -r PARSER_HEADER_H __LSON_JSON_PARSER_H__
LSUtility.exe "../Src/Gen/LSONJsonParserLocation.h" -r \r "" -r \n \r\n -r position.hh LSONJsonParserPosition.h -r BISON_LOCATION_HH __LSON_LSONMLPARSERLOCATION_H__
LSUtility.exe "../Src/Gen/LSONJsonParserPosition.h" -r \r "" -r \n \r\n -r std::max max -r BISON_POSITION_HH __LSON_LSONMLPARSERPOSITION_H__ -r "\r\nnamespace yy" "using namespace std;\r\n\r\nnamespace yy"
LSUtility.exe "../Src/Gen/LSONJsonParserStack.h" -r \r "" -r \n \r\n -r "unsigned int*    height" "size_t height" -r BISON_STACK_HH __LSON_LSONMLPARSERSTACK_H__
LSUtility.exe "../Src/Gen/LSONJsonParserStack.h" -r "#include <deque>" "#include <deque>\r\n#ifdef yy\r\n#undef yy\r\n#endif  // yy\r\n#define yy                  lson"

:: set s1=deque^<T^>^>
:: set s2=deque^<T^> ^>
:: LSUtility.exe "../Src/Gen/LSONJsonParserStack.h" -r %s1% %s2%
LSUtility.exe "../Src/Gen/LSONJsonParserStack.h" -r "deque<T>>" "deque<T> >"


:: set s3=stack^<T^>^>
:: set s4=stack^<T^> ^>
:: LSUtility.exe "../Src/Gen/LSONJsonParserStack.h" -r %s3% %s4%
LSUtility.exe "../Src/Gen/LSONJsonParserStack.h" -r "stack<T>>" "stack<T> >"

:: Below are just to make the code look pretty.  If this causes an error in a future version of Bison, remove it.
LSUtility.exe "../Src/Gen/LSONJsonParserLocation.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../Src/Gen/LSONJsonParserPosition.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../Src/Gen/LSONJsonParserStack.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"

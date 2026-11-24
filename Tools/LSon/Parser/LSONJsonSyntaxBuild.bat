bison -ld --skeleton=lalr1.cc -o "LSONJsonParser.cpp" "LSONJsonParser.b"

copy LSONJsonParser.cpp "../Src/Gen/LSONJsonParser.cpp"
copy LSONJsonParser.hpp "../Src/Gen/LSONJsonParser.h"
copy location.hh "../Src/Gen/LSONJsonParserLocation.h"
copy position.hh "../Src/Gen/LSONJsonParserPosition.h"
:: The stack of the skeleton (stack.hh) uses a std::deque; LSONJsonParserFastStack.h replaces it with a std::vector.
copy LSONJsonParserFastStack.h "../Src/Gen/LSONJsonParserStack.h"

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

:: Below are just to make the code look pretty.  If this causes an error in a future version of Bison, remove it.
LSUtility.exe "../Src/Gen/LSONJsonParserLocation.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../Src/Gen/LSONJsonParserPosition.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"

bison -ld --skeleton=lalr1.cc -o "EEExpEvalParser.cpp" "EEExpEvalParser.b"

copy EEExpEvalParser.cpp "../Src/Gen/EEExpEvalParser.cpp"
copy EEExpEvalParser.hpp "../Src/Gen/EEExpEvalParser.h"
copy location.hh "../Src/Gen/EEExpEvalParserLocation.h"
copy position.hh "../Src/Gen/EEExpEvalParserPosition.h"
copy stack.hh "../Src/Gen/EEExpEvalParserStack.h"

del location.hh
del position.hh
del stack.hh

LSUtility.exe "../Src/Gen/EEExpEvalParser.cpp" -r \r "" -r \n \r\n
LSUtility.exe "../Src/Gen/EEExpEvalParser.cpp" -r EEExpEvalParser.hpp EEExpEvalParser.h -r parser CExpEvalParser
LSUtility.exe "../Src/Gen/EEExpEvalParser.cpp" -r m_pslLexer_yyarg _pslLexer -r m_pslpLexer_yyarg _pslpLexer -r "\"syntax error\"" "\"Syntax error.\"" -r "\""syntax error," "\""Syntax error,"
LSUtility.exe "../Src/Gen/EEExpEvalParser.h" -r "#include <string>" "" -r "#include <iostream>" "" -r parser CExpEvalParser
LSUtility.exe "../Src/Gen/EEExpEvalParser.h" -r m_pslLexer_yyarg _pslLexer -r m_pslpLexer_yyarg _pslpLexer
LSUtility.exe "../Src/Gen/EEExpEvalParser.h" -r location.hh EEExpEvalParserLocation.h -r position.hh EEExpEvalParserPosition.h -r stack.hh EEExpEvalParserStack.h
LSUtility.exe "../Src/Gen/EEExpEvalParser.h" -r iff if -r PARSER_HEADER_H __EE_EXPEVAL_PARSER_H__
LSUtility.exe "../Src/Gen/EEExpEvalParserLocation.h" -r \r "" -r \n \r\n -r position.hh EEExpEvalParserPosition.h -r BISON_LOCATION_HH __EE_EXPEVALPARSERLOCATION_H__
LSUtility.exe "../Src/Gen/EEExpEvalParserPosition.h" -r \r "" -r \n \r\n -r std::max max -r BISON_POSITION_HH __EE_EXPEVALPARSERPOSITION_H__ -r "\r\nnamespace yy" "using namespace std;\r\n\r\nnamespace yy"
LSUtility.exe "../Src/Gen/EEExpEvalParserStack.h" -r \r "" -r \n \r\n -r "unsigned int*    height" "size_t height" -r BISON_STACK_HH __EE_EXPEVALPARSERSTACK_H__
LSUtility.exe "../Src/Gen/EEExpEvalParserStack.h" -r "#include <deque>" "#include <deque>\r\n#ifdef yy\r\n#undef yy\r\n#endif  // yy\r\n#define yy                  ee"

:: set s1=deque^<T^>^>
:: set s2=deque^<T^> ^>
:: LSUtility.exe "../Src/Gen/EEExpEvalParserStack.h" -r %s1% %s2%
LSUtility.exe "../Src/Gen/EEExpEvalParserStack.h" -r "deque<T>>" "deque<T> >"


:: set s3=stack^<T^>^>
:: set s4=stack^<T^> ^>
:: LSUtility.exe "../Src/Gen/EEExpEvalParserStack.h" -r %s3% %s4%
LSUtility.exe "../Src/Gen/EEExpEvalParserStack.h" -r "stack<T>>" "stack<T> >"

:: Below are just to make the code look pretty.  If this causes an error in a future version of Bison, remove it.
LSUtility.exe "../Src/Gen/EEExpEvalParserLocation.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../Src/Gen/EEExpEvalParserPosition.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"
LSUtility.exe "../Src/Gen/EEExpEvalParserStack.h" -r "(" "( " -r ")" " )" -r " (" "(" -r "(  )" "()" -r : " :" -r " : :" "::" -r "std::string" "std::string " -r "# " "#" -r ")  :" ") :" -r "operator+(" "operator + (" -r "operator-(" "operator - (" -r "operator-=(" "operator -= (" -r "operator+=(" "operator += (" -r "operator==(" "operator == (" -r "operator!=(" "operator != (" -r "operator<<(" "operator << (" -r if( "if (" -r for( "for (" -r while( "while (" -r "operator [](" "operator [] (" -r "& " " &" -r "& &" "&&" -r "> >" ">>" -r "operator+" "operator +" -r "operator-" "operator -" -r "operator=" "operator =" -r "operator!" "operator !" -r "' :'" "':'"

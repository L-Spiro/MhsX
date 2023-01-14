flex.exe -+ -L -Cf -8 --nounistd -PLSONJsonLex "LSONJsonLex.l"

del LSONJsonLexBase.cpp
rename lex.LSONJsonLex.cc LSONJsonLexBase.cpp
copy LSONJsonLexBase.cpp "../Src/Gen"
copy FlexLexer.h "../Src/Gen/LSONJsonLexBase.h"

LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r \r "" -r \n \r\n
LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r LSONJsonLexFlexLexer CJsonLexBase -r "#line * \"lex.LSONJsonLex.cc\"" "#include \"../LSONJson.h\"\r\n\r\nusing namespace lson;\r\n// warning C4127: conditional expression is constant\r\n#pragma warning( disable : 4127 )\r\n#ifndef __STDC_VERSION__\r\n#define __STDC_VERSION__ 201112L\r\n#endif // #ifndef __STDC_VERSION__\r\n\r\n"
LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r "<FlexLexer.h>" \"LSONJsonLexBase.h\"
LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r "register" ""
LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r "LexerOutput( yytext, yyleng )" "LexerOutput( yytext, static_cast<int>(yyleng) )" -r "int yyleng" "uintptr_t yyleng" -r "extern uintptr_t yyleng;" "" -r "int yyl" "uintptr_t yyl"
LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r "int num_to_read" "uintptr_t num_to_read" -r "int new_size" "uintptr_t new_size" -r "int num_to_alloc" "uintptr_t num_to_alloc"
LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r "int max_size" "uintptr_t max_size"
LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r "read( buf, max_size )" "read( buf, static_cast<std::streamsize>(max_size) )"
LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r "int yy_n_chars" "uintptr_t yy_n_chars"
LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r "int number_to_move" "uintptr_t number_to_move"
::LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r "#line 3 \"lex.LSONJsonLex.cc\"" ""
LSUtility.exe "../Src/Gen/LSONJsonLexBase.cpp" -r "int offset" "uintptr_t offset" -r "return yyin->gcount()" "return static_cast<int>(yyin->gcount())"

LSUtility.exe "../Src/Gen/LSONJsonLexBase.h" -r "int yyleng" "uintptr_t yyleng" -r "int YYLeng" "uintptr_t YYLeng" -r "int yylineno;" "int yylineno;FLEX_STD string curFile;" -r "int yy_top_state();" "int yy_top_state();void set_file( const char* buf );" -r "int yylex();" "int yylex();void get_line( FLEX_STD string &_sFileName, int &_iLine );const FLEX_STD string & get_file() const;const std::string & get_tmp_txt() const { return yy_tmp_txt; };const std::u16string & get_tmp_txt16() const { return yy_tmp_txt16; };const std::u32string & get_tmp_txt32() const { return yy_tmp_txt32; }"
LSUtility.exe "../Src/Gen/LSONJsonLexBase.h" -r "int yy_n_chars" "uintptr_t yy_n_chars"
LSUtility.exe "../Src/Gen/LSONJsonLexBase.h" -r "int yy_prev_more_offset" "int yy_prev_more_offset;std::string yy_tmp_txt;std::u16string yy_tmp_txt16;std::u32string yy_tmp_txt32"
LSUtility.exe "../Src/Gen/LSONJsonLexBase.h" -r "int max_size" "uintptr_t max_size" -r __FLEX_LEXER_H __LSON_JSONLEXBASE_H__ -r "#ifndef __LSON_JSONLEXBASE_H__" "#pragma once\r\n#ifndef __LSON_JSONLEXBASE_H__"

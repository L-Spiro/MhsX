flex.exe -+ -L -Cf --nounistd -PEEPreProcLex "EEPreProcLex.l"
del EEPreProcLexBase.cpp
rename lex.EEPreProcLex.cc EEPreProcLexBase.cpp
copy EEPreProcLexBase.cpp "../../Src/PreProcGen"
copy FlexLexer.h "../../Src/PreProcGen/EEPreProcLexBase.h"

LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.cpp" -r \r "" -r \n \r\n
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.cpp" -r EEPreProcLexFlexLexer CPreProcLexBase -r "#line * \"lex.EEPreProcLex.cc\"" "#include \"../EEExpEval.h\"\r\n\r\nusing namespace ee;\r\n// warning C4127: conditional expression is constant\r\n#pragma warning( disable : 4127 )"
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.cpp" -r "<FlexLexer.h>" \"EEPreProcLexBase.h\"
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.cpp" -r "using namespace ee;" "using namespace ee;\r\n#define register"
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.cpp" -r "LexerOutput( yytext, yyleng )" "LexerOutput( yytext, static_cast<int>(yyleng) )" -r "int yyleng" "uintptr_t yyleng" -r "extern uintptr_t yyleng;" "" -r "int yyl" "uintptr_t yyl"
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.cpp" -r "int num_to_read" "uintptr_t num_to_read" -r "int new_size" "uintptr_t new_size" -r "int num_to_alloc" "uintptr_t num_to_alloc"
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.cpp" -r "int max_size" "uintptr_t max_size"
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.cpp" -r "read( buf, max_size )" "read( buf, static_cast<std::streamsize>(max_size) )"
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.cpp" -r "int yy_n_chars" "uintptr_t yy_n_chars"
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.cpp" -r "int number_to_move" "uintptr_t number_to_move"
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.cpp" -r "int offset" "uintptr_t offset" -r "return yyin->gcount()" "return static_cast<int>(yyin->gcount())"

LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.h" -r "int yyleng" "uintptr_t yyleng" -r "int YYLeng" "uintptr_t YYLeng" -r "int yylineno;" "int yylineno;FLEX_STD string curFile;" -r "int yy_top_state();" "int yy_top_state();void set_file( const char* buf );" -r "int yylex();" "int yylex();void get_line( FLEX_STD string &_sFileName, int &_iLine );const FLEX_STD string & get_file() const;"
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.h" -r "int yy_n_chars" "uintptr_t yy_n_chars"
LSUtility.exe "../../Src/PreProcGen/EEPreProcLexBase.h" -r "int max_size" "uintptr_t max_size" -r __FLEX_LEXER_H __EE_PREPROCLEXBASE_H__
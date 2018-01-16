
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C++ LALR(1) parser skeleton written by Akim Demaille.  */

#ifndef PARSER_HEADER_H
# define PARSER_HEADER_H



#include <string>
#include <iostream>
#include "stack.hh"



namespace yy {

  class position;
  class location;


} // yy


#include "location.hh"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
do {							\
  if (N)						\
    {							\
      (Current).begin = (Rhs)[1].begin;			\
      (Current).end   = (Rhs)[N].end;			\
    }							\
  else							\
    {							\
      (Current).begin = (Current).end = (Rhs)[0].end;	\
    }							\
} while (false)
#endif



namespace yy {


  /// A Bison parser.
  class parser
  {
  public:
    /// Symbol semantic values.
#ifndef YYSTYPE
    typedef YYSTYPE semantic_type;
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;
    /// Tokens.
    struct token
    {
      /* Tokens.  */
   enum yytokentype {
     EE_CHAR_CONSTANT = 258,
     EE_FLOAT_CONSTANT = 259,
     EE_STRING_CONSTANT = 260,
     EE_HEX_CONSTANT1 = 261,
     EE_HEX_CONSTANT2 = 262,
     EE_HEX_CONSTANT3 = 263,
     EE_BIN_CONSTANT = 264,
     EE_DEC_CONSTANT = 265,
     EE_OCT_CONSTANT = 266,
     EE_PUREDEC_CONSTANT = 267,
     EE_IDENTIFIER = 268,
     EE_MEMBERACCESS = 269,
     EE_USER_VAR = 270,
     EE_EQU_E = 271,
     EE_EQU_NE = 272,
     EE_LEFT_OP = 273,
     EE_RIGHT_OP = 274,
     EE_REL_GE = 275,
     EE_REL_LE = 276,
     EE_OB_BYTE = 277,
     EE_OB_SBYTE = 278,
     EE_OB_WORD = 279,
     EE_OB_SWORD = 280,
     EE_OB_QWORD = 281,
     EE_OB_SQWORD = 282,
     EE_OB_FLOAT = 283,
     EE_OB_DOUBLE = 284,
     EE_OB_DWORD = 285,
     EE_OB_SDWORD = 286,
     EE_COS = 287,
     EE_SIN = 288,
     EE_TAN = 289,
     EE_ACOS = 290,
     EE_ASIN = 291,
     EE_ATAN = 292,
     EE_ATAN2 = 293,
     EE_COSH = 294,
     EE_SINH = 295,
     EE_TANH = 296,
     EE_ACOSH = 297,
     EE_ASINH = 298,
     EE_ATANH = 299,
     EE_EXP = 300,
     EE_LOG = 301,
     EE_LOG10 = 302,
     EE_LOG2 = 303,
     EE_EXP2 = 304,
     EE_EXPM1 = 305,
     EE_ILOGB = 306,
     EE_LOG1P = 307,
     EE_LOGB = 308,
     EE_POW = 309,
     EE_SQRT = 310,
     EE_CBRT = 311,
     EE_HYPOT = 312,
     EE_TGAMMA = 313,
     EE_LGAMMA = 314,
     EE_CEIL = 315,
     EE_FLOOR = 316,
     EE_MOD = 317,
     EE_TRUNC = 318,
     EE_ROUND = 319,
     EE_NEARBYINT = 320,
     EE_REMAINDER = 321,
     EE_REMQUO = 322,
     EE_NEXTAFTER = 323,
     EE_NEXTAFTERF = 324,
     EE_NEXTTOWARD = 325,
     EE_NEXTTOWARDF = 326,
     EE_DIM = 327,
     EE_MAX = 328,
     EE_MIN = 329,
     EE_ABS = 330,
     EE_MADD = 331,
     EE_A = 332,
     EE_ALLADI = 333,
     EE_ALPHA = 334,
     EE_B = 335,
     EE_B2 = 336,
     EE_B4 = 337,
     EE_BETA = 338,
     EE_BH = 339,
     EE_C2 = 340,
     EE_CAHEN = 341,
     EE_CATALAN = 342,
     EE_CONWAY = 343,
     EE_DELTA = 344,
     EE_E = 345,
     EE_ERDOS = 346,
     EE_EULER = 347,
     EE_F = 348,
     EE_GR = 349,
     EE_GWK = 350,
     EE_HALFPI = 351,
     EE_HSMC = 352,
     EE_ICE = 353,
     EE_K = 354,
     EE_LAMBDA = 355,
     EE_LAPLACE = 356,
     EE_LEVY = 357,
     EE_M1 = 358,
     EE_MU = 359,
     EE_NIVEN = 360,
     EE_OMEGA = 361,
     EE_P2 = 362,
     EE_PI = 363,
     EE_PLASTIC = 364,
     EE_PORTER = 365,
     EE_PSI = 366,
     EE_RAMAN = 367,
     EE_RAMAMU = 368,
     EE_SIERP = 369,
     EE_THETA = 370,
     EE_VISW = 371,
     EE_Z3 = 372,
     EE_ZETA = 373,
     EE_CHAR_BIT = 374,
     EE_MB_LEN_MAX = 375,
     EE_CHAR_MIN = 376,
     EE_CHAR_MAX = 377,
     EE_SCHAR_MIN = 378,
     EE_SHRT_MIN = 379,
     EE_INT_MIN = 380,
     EE_LONG_MIN = 381,
     EE_LLONG_MIN = 382,
     EE_SCHAR_MAX = 383,
     EE_SHRT_MAX = 384,
     EE_INT_MAX = 385,
     EE_LONG_MAX = 386,
     EE_LLONG_MAX = 387,
     EE_UCHAR_MAX = 388,
     EE_USHRT_MAX = 389,
     EE_UINT_MAX = 390,
     EE_ULONG_MAX = 391,
     EE_ULLONG_MAX = 392,
     EE_FLT_RADIX = 393,
     EE_DECIMAL_DIG = 394,
     EE_FLT_DECIMAL_DIG = 395,
     EE_DBL_DECIMAL_DIG = 396,
     EE_LDBL_DECIMAL_DIG = 397,
     EE_FLT_MIN = 398,
     EE_DBL_MIN = 399,
     EE_LDBL_MIN = 400,
     EE_FLT_TRUE_MIN = 401,
     EE_DBL_TRUE_MIN = 402,
     EE_LDBL_TRUE_MIN = 403,
     EE_FLT_MAX = 404,
     EE_DBL_MAX = 405,
     EE_LDBL_MAX = 406,
     EE_FLT_EPSILON = 407,
     EE_DBL_EPSILON = 408,
     EE_LDBL_EPSILON = 409,
     EE_FLT_DIG = 410,
     EE_DBL_DIG = 411,
     EE_LDBL_DIG = 412,
     EE_FLT_MANT_DIG = 413,
     EE_DBL_MANT_DIG = 414,
     EE_LDBL_MANT_DIG = 415,
     EE_FLT_MIN_EXP = 416,
     EE_DBL_MIN_EXP = 417,
     EE_LDBL_MIN_EXP = 418,
     EE_FLT_MIN_10_EXP = 419,
     EE_DBL_MIN_10_EXP = 420,
     EE_LDBL_MIN_10_EXP = 421,
     EE_FLT_MAX_EXP = 422,
     EE_DBL_MAX_EXP = 423,
     EE_LDBL_MAX_EXP = 424,
     EE_FLT_MAX_10_EXP = 425,
     EE_DBL_MAX_10_EXP = 426,
     EE_LDBL_MAX_10_EXP = 427,
     EE_AS_FLOAT = 428,
     EE_AS_DOUBLE = 429,
     EE_AS_FLOAT16 = 430,
     EE_AS_FLOAT14 = 431,
     EE_AS_FLOAT11 = 432,
     EE_AS_FLOAT10 = 433,
     EE_TRUE = 434,
     EE_FALSE = 435,
     EE_OR = 436,
     EE_AND = 437,
     EE_LOG_NOT = 438,
     EE_UNARY_1SCOMPL = 439,
     EE_UPLUS = 440,
     EE_UMINUS = 441
   };

    };
    /// Token type.
    typedef token::yytokentype token_type;

    /// Build a parser object.
    parser (class CExpEvalLexer * m_peelLexer_yyarg, class CExpEvalContainer * m_peecContainer_yyarg);
    virtual ~parser ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

  private:
    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Generate an error message.
    /// \param state   the state where the error occurred.
    /// \param tok     the lookahead token.
    virtual std::string yysyntax_error_ (int yystate);

#if YYDEBUG
    /// \brief Report a symbol value on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_value_print_ (int yytype,
					 const semantic_type* yyvaluep,
					 const location_type* yylocationp);
    /// \brief Report a symbol on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_print_ (int yytype,
				   const semantic_type* yyvaluep,
				   const location_type* yylocationp);
#endif


    /// State numbers.
    typedef int state_type;
    /// State stack type.
    typedef stack<state_type>    state_stack_type;
    /// Semantic value stack type.
    typedef stack<semantic_type> semantic_stack_type;
    /// location stack type.
    typedef stack<location_type> location_stack_type;

    /// The state stack.
    state_stack_type yystate_stack_;
    /// The semantic value stack.
    semantic_stack_type yysemantic_stack_;
    /// The location stack.
    location_stack_type yylocation_stack_;

    /// Internal symbol numbers.
    typedef unsigned char token_number_type;
    /* Tables.  */
    /// For a state, the index in \a yytable_ of its portion.
    static const short int yypact_[];
    static const short int yypact_ninf_;

    /// For a state, default rule to reduce.
    /// Unless\a  yytable_ specifies something else to do.
    /// Zero means the default is an error.
    static const unsigned short int yydefact_[];

    static const short int yypgoto_[];
    static const short int yydefgoto_[];

    /// What to do in a state.
    /// \a yytable_[yypact_[s]]: what to do in state \a s.
    /// - if positive, shift that token.
    /// - if negative, reduce the rule which number is the opposite.
    /// - if zero, do what YYDEFACT says.
    static const unsigned short int yytable_[];
    static const signed char yytable_ninf_;

    static const short int yycheck_[];

    /// For a state, its accessing symbol.
    static const unsigned char yystos_[];

    /// For a rule, its LHS.
    static const unsigned char yyr1_[];
    /// For a rule, its RHS length.
    static const unsigned char yyr2_[];

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#endif

#if YYERROR_VERBOSE
    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    virtual std::string yytnamerr_ (const char *n);
#endif

#if YYDEBUG
    /// A type to store symbol numbers and -1.
    typedef short int rhs_number_type;
    /// A `-1'-separated list of the rules' RHS.
    static const rhs_number_type yyrhs_[];
    /// For each rule, the index of the first RHS symbol in \a yyrhs_.
    static const unsigned short int yyprhs_[];
    /// For each rule, its source line number.
    static const unsigned short int yyrline_[];
    /// For each scanner token number, its symbol number.
    static const unsigned short int yytoken_number_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    /* Debugging.  */
    int yydebug_;
    std::ostream* yycdebug_;
#endif

    /// Convert a scanner token number \a t to a symbol number.
    token_number_type yytranslate_ (int t);

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg        Why this token is reclaimed.
    /// \param yytype       The symbol type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    inline void yydestruct_ (const char* yymsg,
			     int yytype,
			     semantic_type* yyvaluep,
			     location_type* yylocationp);

    /// Pop \a n symbols the three stacks.
    inline void yypop_ (unsigned int n = 1);

    /* Constants.  */
    static const int yyeof_;
    /* LAST_ -- Last index in TABLE_.  */
    static const int yylast_;
    static const int yynnts_;
    static const int yyempty_;
    static const int yyfinal_;
    static const int yyterror_;
    static const int yyerrcode_;
    static const int yyntokens_;
    static const unsigned int yyuser_token_number_max_;
    static const token_number_type yyundef_token_;

    /* User arguments.  */
    class CExpEvalLexer * m_peelLexer;
    class CExpEvalContainer * m_peecContainer;
  };


} // yy




#endif /* ! defined PARSER_HEADER_H */

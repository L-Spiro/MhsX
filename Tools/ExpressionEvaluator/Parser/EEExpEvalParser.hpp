
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
     EE_DO = 258,
     EE_ELSE = 259,
     EE_FOR = 260,
     EE_FOREACH = 261,
     EE_IF = 262,
     EE_IN = 263,
     EE_WHILE = 264,
     EE_BREAK = 265,
     EE_CONTINUE = 266,
     EE_NEW = 267,
     EE_COPY = 268,
     EE_ANY = 269,
     EE_DEFAULT = 270,
     EE_DOUBLE = 271,
     EE_FLOAT = 272,
     EE_FLOAT10 = 273,
     EE_FLOAT11 = 274,
     EE_FLOAT14 = 275,
     EE_FLOAT16 = 276,
     EE_INT8 = 277,
     EE_INT16 = 278,
     EE_INT32 = 279,
     EE_INT64 = 280,
     EE_OBJECT = 281,
     EE_PERSISTENT = 282,
     EE_TEMP = 283,
     EE_UINT8 = 284,
     EE_UINT16 = 285,
     EE_UINT32 = 286,
     EE_UINT64 = 287,
     EE_CONST = 288,
     EE_CHAR_CONSTANT = 289,
     EE_FLOAT_CONSTANT = 290,
     EE_HEX_CONSTANT1 = 291,
     EE_HEX_CONSTANT2 = 292,
     EE_HEX_CONSTANT3 = 293,
     EE_STRING_CONSTANT = 294,
     EE_UNICODE_CONSTANT = 295,
     EE_BIN_CONSTANT = 296,
     EE_DEC_CONSTANT = 297,
     EE_OCT_CONSTANT = 298,
     EE_PUREDEC_CONSTANT = 299,
     EE_IDENTIFIER = 300,
     EE_ARRAY = 301,
     EE_CUSTOM_VAR = 302,
     EE_MEMBERACCESS = 303,
     EE_USER_VAR = 304,
     EE_EQU_E = 305,
     EE_EQU_NE = 306,
     EE_LEFT_OP = 307,
     EE_RIGHT_OP = 308,
     EE_REL_GE = 309,
     EE_REL_LE = 310,
     EE_PLUSPLUS = 311,
     EE_MINUSMINUS = 312,
     EE_ASS_PLUSEQUALS = 313,
     EE_ASS_MINUSEQUALS = 314,
     EE_ASS_TIMESEQUALS = 315,
     EE_ASS_MODEQUALS = 316,
     EE_ASS_DIVEQUALS = 317,
     EE_ASS_CARROTEQUALS = 318,
     EE_ASS_SHLEFTEQUALS = 319,
     EE_ASS_SHRIGHTEQUALS = 320,
     EE_ASS_OREQUALS = 321,
     EE_ASS_ANDEQUALS = 322,
     EE_OB_BYTE = 323,
     EE_OB_SBYTE = 324,
     EE_OB_WORD = 325,
     EE_OB_SWORD = 326,
     EE_OB_QWORD = 327,
     EE_OB_SQWORD = 328,
     EE_OB_FLOAT = 329,
     EE_OB_DOUBLE = 330,
     EE_OB_DWORD = 331,
     EE_OB_SDWORD = 332,
     EE_OB_FLOAT16 = 333,
     EE_COS = 334,
     EE_SIN = 335,
     EE_TAN = 336,
     EE_ACOS = 337,
     EE_ASIN = 338,
     EE_ATAN = 339,
     EE_ATAN2 = 340,
     EE_COSH = 341,
     EE_SINH = 342,
     EE_TANH = 343,
     EE_ACOSH = 344,
     EE_ASINH = 345,
     EE_ATANH = 346,
     EE_EPSILON = 347,
     EE_RELEPSILON = 348,
     EE_EPSILONF = 349,
     EE_RELEPSILONF = 350,
     EE_EXP = 351,
     EE_LOG = 352,
     EE_LOG10 = 353,
     EE_LOG2 = 354,
     EE_EXP2 = 355,
     EE_EXPM1 = 356,
     EE_ILOGB = 357,
     EE_LOG1P = 358,
     EE_LOGB = 359,
     EE_POW = 360,
     EE_SQRT = 361,
     EE_CBRT = 362,
     EE_HYPOT = 363,
     EE_TGAMMA = 364,
     EE_LGAMMA = 365,
     EE_CEIL = 366,
     EE_FLOOR = 367,
     EE_MOD = 368,
     EE_TRUNC = 369,
     EE_ROUND = 370,
     EE_NEARBYINT = 371,
     EE_REMAINDER = 372,
     EE_REMQUO = 373,
     EE_NEXTAFTER = 374,
     EE_NEXTAFTERF = 375,
     EE_NEXTTOWARD = 376,
     EE_NEXTTOWARDF = 377,
     EE_DIM = 378,
     EE_MAX = 379,
     EE_MIN = 380,
     EE_DEG = 381,
     EE_RAD = 382,
     EE_ABS = 383,
     EE_MADD = 384,
     EE_RAND = 385,
     EE_ISNAN = 386,
     EE_ISINF = 387,
     EE_BYTESWAPUSHORT = 388,
     EE_BYTESWAPULONG = 389,
     EE_BYTESWAPUINT64 = 390,
     EE_A = 391,
     EE_ALLADI = 392,
     EE_ALPHA = 393,
     EE_B = 394,
     EE_B2 = 395,
     EE_B4 = 396,
     EE_BETA = 397,
     EE_BH = 398,
     EE_C2 = 399,
     EE_CAHEN = 400,
     EE_CATALAN = 401,
     EE_CONWAY = 402,
     EE_DELTA = 403,
     EE_E = 404,
     EE_ERDOS = 405,
     EE_EULER = 406,
     EE_F = 407,
     EE_GR = 408,
     EE_GWK = 409,
     EE_HALFPI = 410,
     EE_HSMC = 411,
     EE_ICE = 412,
     EE_K = 413,
     EE_LAMBDA = 414,
     EE_LAPLACE = 415,
     EE_LEVY = 416,
     EE_M1 = 417,
     EE_MU = 418,
     EE_NIVEN = 419,
     EE_OMEGA = 420,
     EE_P2 = 421,
     EE_PI = 422,
     EE_PLASTIC = 423,
     EE_PORTER = 424,
     EE_PSI = 425,
     EE_RAMAN = 426,
     EE_RAMAMU = 427,
     EE_SIERP = 428,
     EE_THETA = 429,
     EE_VISW = 430,
     EE_Z3 = 431,
     EE_ZETA = 432,
     EE_CHAR_BIT = 433,
     EE_MB_LEN_MAX = 434,
     EE_CHAR_MIN = 435,
     EE_CHAR_MAX = 436,
     EE_SCHAR_MIN = 437,
     EE_SHRT_MIN = 438,
     EE_INT_MIN = 439,
     EE_LONG_MIN = 440,
     EE_LLONG_MIN = 441,
     EE_SCHAR_MAX = 442,
     EE_SHRT_MAX = 443,
     EE_INT_MAX = 444,
     EE_LONG_MAX = 445,
     EE_LLONG_MAX = 446,
     EE_UCHAR_MAX = 447,
     EE_USHRT_MAX = 448,
     EE_UINT_MAX = 449,
     EE_ULONG_MAX = 450,
     EE_ULLONG_MAX = 451,
     EE_FLT_RADIX = 452,
     EE_DECIMAL_DIG = 453,
     EE_FLT_DECIMAL_DIG = 454,
     EE_DBL_DECIMAL_DIG = 455,
     EE_LDBL_DECIMAL_DIG = 456,
     EE_FLT_MIN = 457,
     EE_DBL_MIN = 458,
     EE_LDBL_MIN = 459,
     EE_FLT_TRUE_MIN = 460,
     EE_DBL_TRUE_MIN = 461,
     EE_LDBL_TRUE_MIN = 462,
     EE_FLT_MAX = 463,
     EE_DBL_MAX = 464,
     EE_LDBL_MAX = 465,
     EE_FLT_EPSILON = 466,
     EE_DBL_EPSILON = 467,
     EE_LDBL_EPSILON = 468,
     EE_FLT_DIG = 469,
     EE_DBL_DIG = 470,
     EE_LDBL_DIG = 471,
     EE_FLT_MANT_DIG = 472,
     EE_DBL_MANT_DIG = 473,
     EE_LDBL_MANT_DIG = 474,
     EE_FLT_MIN_EXP = 475,
     EE_DBL_MIN_EXP = 476,
     EE_LDBL_MIN_EXP = 477,
     EE_FLT_MIN_10_EXP = 478,
     EE_DBL_MIN_10_EXP = 479,
     EE_LDBL_MIN_10_EXP = 480,
     EE_FLT_MAX_EXP = 481,
     EE_DBL_MAX_EXP = 482,
     EE_LDBL_MAX_EXP = 483,
     EE_FLT_MAX_10_EXP = 484,
     EE_DBL_MAX_10_EXP = 485,
     EE_LDBL_MAX_10_EXP = 486,
     EE_AS_FLOAT = 487,
     EE_AS_DOUBLE = 488,
     EE_AS_FLOAT24 = 489,
     EE_AS_FLOAT16 = 490,
     EE_AS_FLOAT14 = 491,
     EE_AS_FLOAT11 = 492,
     EE_AS_FLOAT10 = 493,
     EE_AS_FLOAT_MAX = 494,
     EE_AS_FLOAT_MIN = 495,
     EE_AS_FLOAT_TRUE_MIN = 496,
     EE_AS_FLOAT_NAN = 497,
     EE_AS_FLOAT_INF = 498,
     EE_AS_FLOAT_SUBNORM_MAX = 499,
     EE_AS_FLOAT_EPS = 500,
     EE_TRUE = 501,
     EE_FALSE = 502,
     EE_AS_FLOAT_SIGNBIT = 503,
     EE_AS_FLOAT_EXPBITS = 504,
     EE_AS_FLOAT_MANBITS = 505,
     EE_TO_FLOAT32 = 506,
     EE_TO_FLOAT64 = 507,
     EE_TO_INT8 = 508,
     EE_TO_INT16 = 509,
     EE_TO_INT32 = 510,
     EE_TO_INT64 = 511,
     EE_TO_UINT8 = 512,
     EE_TO_UINT16 = 513,
     EE_TO_UINT32 = 514,
     EE_TO_UINT64 = 515,
     EE_CLOCK = 516,
     EE_CLOCKS_PER_SEC = 517,
     EE_SECONDS = 518,
     EE_MILLISECONDS = 519,
     EE_MICROSECONDS = 520,
     EE_SECONDS_SINCE_START = 521,
     EE_MILLISECONDS_SINCE_START = 522,
     EE_MICROSECONDS_SINCE_START = 523,
     EE_OR = 524,
     EE_AND = 525,
     EE_LOG_NOT = 526,
     EE_UNARY_1SCOMPL = 527,
     EE_UPLUS = 528,
     EE_UMINUS = 529
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
    typedef unsigned short int token_number_type;
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
    static const unsigned short int yystos_[];

    /// For a rule, its LHS.
    static const unsigned short int yyr1_[];
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


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
     EE_PARM = 304,
     EE_PARM_TOTAL = 305,
     EE_USER_VAR = 306,
     EE_EQU_E = 307,
     EE_EQU_NE = 308,
     EE_LEFT_OP = 309,
     EE_RIGHT_OP = 310,
     EE_REL_GE = 311,
     EE_REL_LE = 312,
     EE_PLUSPLUS = 313,
     EE_MINUSMINUS = 314,
     EE_ASS_PLUSEQUALS = 315,
     EE_ASS_MINUSEQUALS = 316,
     EE_ASS_TIMESEQUALS = 317,
     EE_ASS_MODEQUALS = 318,
     EE_ASS_DIVEQUALS = 319,
     EE_ASS_CARROTEQUALS = 320,
     EE_ASS_SHLEFTEQUALS = 321,
     EE_ASS_SHRIGHTEQUALS = 322,
     EE_ASS_OREQUALS = 323,
     EE_ASS_ANDEQUALS = 324,
     EE_OB_BYTE = 325,
     EE_OB_SBYTE = 326,
     EE_OB_WORD = 327,
     EE_OB_SWORD = 328,
     EE_OB_QWORD = 329,
     EE_OB_SQWORD = 330,
     EE_OB_FLOAT = 331,
     EE_OB_DOUBLE = 332,
     EE_OB_DWORD = 333,
     EE_OB_SDWORD = 334,
     EE_OB_FLOAT16 = 335,
     EE_COS = 336,
     EE_SIN = 337,
     EE_TAN = 338,
     EE_ACOS = 339,
     EE_ASIN = 340,
     EE_ATAN = 341,
     EE_ATAN2 = 342,
     EE_COSH = 343,
     EE_SINH = 344,
     EE_TANH = 345,
     EE_ACOSH = 346,
     EE_ASINH = 347,
     EE_ATANH = 348,
     EE_EPSILON = 349,
     EE_RELEPSILON = 350,
     EE_EPSILONF = 351,
     EE_RELEPSILONF = 352,
     EE_EXP = 353,
     EE_LOG = 354,
     EE_LOG10 = 355,
     EE_LOG2 = 356,
     EE_EXP2 = 357,
     EE_EXPM1 = 358,
     EE_ILOGB = 359,
     EE_LOG1P = 360,
     EE_LOGB = 361,
     EE_POW = 362,
     EE_SQRT = 363,
     EE_CBRT = 364,
     EE_HYPOT = 365,
     EE_TGAMMA = 366,
     EE_LGAMMA = 367,
     EE_CEIL = 368,
     EE_FLOOR = 369,
     EE_MOD = 370,
     EE_TRUNC = 371,
     EE_ROUND = 372,
     EE_NEARBYINT = 373,
     EE_REMAINDER = 374,
     EE_REMQUO = 375,
     EE_NEXTAFTER = 376,
     EE_NEXTAFTERF = 377,
     EE_NEXTTOWARD = 378,
     EE_NEXTTOWARDF = 379,
     EE_DIM = 380,
     EE_MAX = 381,
     EE_MIN = 382,
     EE_DEG = 383,
     EE_RAD = 384,
     EE_ABS = 385,
     EE_MADD = 386,
     EE_RAND = 387,
     EE_ISNAN = 388,
     EE_ISINF = 389,
     EE_BYTESWAPUSHORT = 390,
     EE_BYTESWAPULONG = 391,
     EE_BYTESWAPUINT64 = 392,
     EE_A = 393,
     EE_ALLADI = 394,
     EE_ALPHA = 395,
     EE_B = 396,
     EE_B2 = 397,
     EE_B4 = 398,
     EE_BETA = 399,
     EE_BH = 400,
     EE_C2 = 401,
     EE_CAHEN = 402,
     EE_CATALAN = 403,
     EE_CONWAY = 404,
     EE_DELTA = 405,
     EE_E = 406,
     EE_ERDOS = 407,
     EE_EULER = 408,
     EE_F = 409,
     EE_GR = 410,
     EE_GWK = 411,
     EE_HALFPI = 412,
     EE_HSMC = 413,
     EE_ICE = 414,
     EE_K = 415,
     EE_LAMBDA = 416,
     EE_LAPLACE = 417,
     EE_LEVY = 418,
     EE_M1 = 419,
     EE_MU = 420,
     EE_NIVEN = 421,
     EE_OMEGA = 422,
     EE_P2 = 423,
     EE_PI = 424,
     EE_PLASTIC = 425,
     EE_PORTER = 426,
     EE_PSI = 427,
     EE_RAMAN = 428,
     EE_RAMAMU = 429,
     EE_SIERP = 430,
     EE_THETA = 431,
     EE_VISW = 432,
     EE_Z3 = 433,
     EE_ZETA = 434,
     EE_CHAR_BIT = 435,
     EE_MB_LEN_MAX = 436,
     EE_CHAR_MIN = 437,
     EE_CHAR_MAX = 438,
     EE_SCHAR_MIN = 439,
     EE_SHRT_MIN = 440,
     EE_INT_MIN = 441,
     EE_LONG_MIN = 442,
     EE_LLONG_MIN = 443,
     EE_SCHAR_MAX = 444,
     EE_SHRT_MAX = 445,
     EE_INT_MAX = 446,
     EE_LONG_MAX = 447,
     EE_LLONG_MAX = 448,
     EE_UCHAR_MAX = 449,
     EE_USHRT_MAX = 450,
     EE_UINT_MAX = 451,
     EE_ULONG_MAX = 452,
     EE_ULLONG_MAX = 453,
     EE_FLT_RADIX = 454,
     EE_DECIMAL_DIG = 455,
     EE_FLT_DECIMAL_DIG = 456,
     EE_DBL_DECIMAL_DIG = 457,
     EE_LDBL_DECIMAL_DIG = 458,
     EE_FLT_MIN = 459,
     EE_DBL_MIN = 460,
     EE_LDBL_MIN = 461,
     EE_FLT_TRUE_MIN = 462,
     EE_DBL_TRUE_MIN = 463,
     EE_LDBL_TRUE_MIN = 464,
     EE_FLT_MAX = 465,
     EE_DBL_MAX = 466,
     EE_LDBL_MAX = 467,
     EE_FLT_EPSILON = 468,
     EE_DBL_EPSILON = 469,
     EE_LDBL_EPSILON = 470,
     EE_FLT_DIG = 471,
     EE_DBL_DIG = 472,
     EE_LDBL_DIG = 473,
     EE_FLT_MANT_DIG = 474,
     EE_DBL_MANT_DIG = 475,
     EE_LDBL_MANT_DIG = 476,
     EE_FLT_MIN_EXP = 477,
     EE_DBL_MIN_EXP = 478,
     EE_LDBL_MIN_EXP = 479,
     EE_FLT_MIN_10_EXP = 480,
     EE_DBL_MIN_10_EXP = 481,
     EE_LDBL_MIN_10_EXP = 482,
     EE_FLT_MAX_EXP = 483,
     EE_DBL_MAX_EXP = 484,
     EE_LDBL_MAX_EXP = 485,
     EE_FLT_MAX_10_EXP = 486,
     EE_DBL_MAX_10_EXP = 487,
     EE_LDBL_MAX_10_EXP = 488,
     EE_INT8_MIN = 489,
     EE_INT16_MIN = 490,
     EE_INT32_MIN = 491,
     EE_INT64_MIN = 492,
     EE_INT8_MAX = 493,
     EE_INT16_MAX = 494,
     EE_INT32_MAX = 495,
     EE_INT64_MAX = 496,
     EE_UINT8_MAX = 497,
     EE_UINT16_MAX = 498,
     EE_UINT32_MAX = 499,
     EE_UINT64_MAX = 500,
     EE_AS_FLOAT = 501,
     EE_AS_DOUBLE = 502,
     EE_AS_FLOAT24 = 503,
     EE_AS_FLOAT16 = 504,
     EE_AS_FLOAT14 = 505,
     EE_AS_FLOAT11 = 506,
     EE_AS_FLOAT10 = 507,
     EE_AS_FLOAT_MAX = 508,
     EE_AS_FLOAT_MIN = 509,
     EE_AS_FLOAT_TRUE_MIN = 510,
     EE_AS_FLOAT_NAN = 511,
     EE_AS_FLOAT_INF = 512,
     EE_AS_FLOAT_SUBNORM_MAX = 513,
     EE_AS_FLOAT_EPS = 514,
     EE_TRUE = 515,
     EE_FALSE = 516,
     EE_AS_FLOAT_SIGNBIT = 517,
     EE_AS_FLOAT_EXPBITS = 518,
     EE_AS_FLOAT_MANBITS = 519,
     EE_TO_FLOAT32 = 520,
     EE_TO_FLOAT64 = 521,
     EE_TO_INT8 = 522,
     EE_TO_INT16 = 523,
     EE_TO_INT32 = 524,
     EE_TO_INT64 = 525,
     EE_TO_UINT8 = 526,
     EE_TO_UINT16 = 527,
     EE_TO_UINT32 = 528,
     EE_TO_UINT64 = 529,
     EE_CLOCK = 530,
     EE_CLOCKS_PER_SEC = 531,
     EE_SECONDS = 532,
     EE_MILLISECONDS = 533,
     EE_MICROSECONDS = 534,
     EE_SECONDS_SINCE_START = 535,
     EE_MILLISECONDS_SINCE_START = 536,
     EE_MICROSECONDS_SINCE_START = 537,
     EE_ASCII = 538,
     EE_BIN = 539,
     EE_BOOL = 540,
     EE_CHR = 541,
     EE_HEX = 542,
     EE_INT = 543,
     EE_LEN = 544,
     EE_OCT = 545,
     EE_ORD = 546,
     EE_OR = 547,
     EE_AND = 548,
     EE_LOG_NOT = 549,
     EE_UNARY_1SCOMPL = 550,
     EE_UPLUS = 551,
     EE_UMINUS = 552
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

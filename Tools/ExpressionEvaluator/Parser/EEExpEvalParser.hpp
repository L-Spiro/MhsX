
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
     EE_STATIC_CAST = 269,
     EE_ANY = 270,
     EE_DEFAULT = 271,
     EE_DOUBLE = 272,
     EE_FLOAT = 273,
     EE_FLOAT10 = 274,
     EE_FLOAT11 = 275,
     EE_FLOAT14 = 276,
     EE_FLOAT16 = 277,
     EE_INT8 = 278,
     EE_INT16 = 279,
     EE_INT32 = 280,
     EE_INT64 = 281,
     EE_OBJECT = 282,
     EE_PERSISTENT = 283,
     EE_TEMP = 284,
     EE_UINT8 = 285,
     EE_UINT16 = 286,
     EE_UINT32 = 287,
     EE_UINT64 = 288,
     EE_CONST = 289,
     EE_CHAR_CONSTANT = 290,
     EE_FLOAT_CONSTANT = 291,
     EE_HEX_CONSTANT1 = 292,
     EE_HEX_CONSTANT2 = 293,
     EE_HEX_CONSTANT3 = 294,
     EE_STRING_CONSTANT = 295,
     EE_UNICODE_CONSTANT = 296,
     EE_BIN_CONSTANT = 297,
     EE_DEC_CONSTANT = 298,
     EE_OCT_CONSTANT = 299,
     EE_PUREDEC_CONSTANT = 300,
     EE_IDENTIFIER = 301,
     EE_ARRAY = 302,
     EE_CUSTOM_VAR = 303,
     EE_MEMBERACCESS = 304,
     EE_PARM = 305,
     EE_PARM_TOTAL = 306,
     EE_USER_VAR = 307,
     EE_EQU_E = 308,
     EE_EQU_NE = 309,
     EE_LEFT_OP = 310,
     EE_RIGHT_OP = 311,
     EE_REL_GE = 312,
     EE_REL_LE = 313,
     EE_PLUSPLUS = 314,
     EE_MINUSMINUS = 315,
     EE_ASS_PLUSEQUALS = 316,
     EE_ASS_MINUSEQUALS = 317,
     EE_ASS_TIMESEQUALS = 318,
     EE_ASS_MODEQUALS = 319,
     EE_ASS_DIVEQUALS = 320,
     EE_ASS_CARROTEQUALS = 321,
     EE_ASS_SHLEFTEQUALS = 322,
     EE_ASS_SHRIGHTEQUALS = 323,
     EE_ASS_OREQUALS = 324,
     EE_ASS_ANDEQUALS = 325,
     EE_OB_BYTE = 326,
     EE_OB_SBYTE = 327,
     EE_OB_WORD = 328,
     EE_OB_SWORD = 329,
     EE_OB_QWORD = 330,
     EE_OB_SQWORD = 331,
     EE_OB_FLOAT = 332,
     EE_OB_DOUBLE = 333,
     EE_OB_DWORD = 334,
     EE_OB_SDWORD = 335,
     EE_OB_FLOAT16 = 336,
     EE_COS = 337,
     EE_SIN = 338,
     EE_TAN = 339,
     EE_ACOS = 340,
     EE_ASIN = 341,
     EE_ATAN = 342,
     EE_ATAN2 = 343,
     EE_COSH = 344,
     EE_SINH = 345,
     EE_TANH = 346,
     EE_ACOSH = 347,
     EE_ASINH = 348,
     EE_ATANH = 349,
     EE_EPSILON = 350,
     EE_RELEPSILON = 351,
     EE_EPSILONF = 352,
     EE_RELEPSILONF = 353,
     EE_EXP = 354,
     EE_LOG = 355,
     EE_LOG10 = 356,
     EE_LOG2 = 357,
     EE_EXP2 = 358,
     EE_EXPM1 = 359,
     EE_ILOGB = 360,
     EE_LOG1P = 361,
     EE_LOGB = 362,
     EE_POW = 363,
     EE_SQRT = 364,
     EE_CBRT = 365,
     EE_HYPOT = 366,
     EE_TGAMMA = 367,
     EE_LGAMMA = 368,
     EE_CEIL = 369,
     EE_FLOOR = 370,
     EE_MOD = 371,
     EE_TRUNC = 372,
     EE_ROUND = 373,
     EE_NEARBYINT = 374,
     EE_REMAINDER = 375,
     EE_REMQUO = 376,
     EE_NEXTAFTER = 377,
     EE_NEXTAFTERF = 378,
     EE_NEXTTOWARD = 379,
     EE_NEXTTOWARDF = 380,
     EE_DIM = 381,
     EE_MAX = 382,
     EE_MIN = 383,
     EE_DEG = 384,
     EE_RAD = 385,
     EE_ABS = 386,
     EE_MADD = 387,
     EE_RAND = 388,
     EE_ISNAN = 389,
     EE_ISINF = 390,
     EE_SINC = 391,
     EE_BYTESWAPUSHORT = 392,
     EE_BYTESWAPULONG = 393,
     EE_BYTESWAPUINT64 = 394,
     EE_BITSWAP8 = 395,
     EE_BITSWAP16 = 396,
     EE_BITSWAP32 = 397,
     EE_BITSWAP64 = 398,
     EE_BITSWAPF16 = 399,
     EE_BITSWAPF32 = 400,
     EE_BITSWAPF64 = 401,
     EE_FORMAT = 402,
     EE_A = 403,
     EE_ALLADI = 404,
     EE_ALPHA = 405,
     EE_B = 406,
     EE_B2 = 407,
     EE_B4 = 408,
     EE_BETA = 409,
     EE_BH = 410,
     EE_C2 = 411,
     EE_CAHEN = 412,
     EE_CATALAN = 413,
     EE_CONWAY = 414,
     EE_DELTA = 415,
     EE_E = 416,
     EE_ERDOS = 417,
     EE_EULER = 418,
     EE_F = 419,
     EE_GR = 420,
     EE_GWK = 421,
     EE_HALFPI = 422,
     EE_HSMC = 423,
     EE_ICE = 424,
     EE_K = 425,
     EE_LAMBDA = 426,
     EE_LAPLACE = 427,
     EE_LEVY = 428,
     EE_M1 = 429,
     EE_MU = 430,
     EE_NIVEN = 431,
     EE_OMEGA = 432,
     EE_P2 = 433,
     EE_PI_ = 434,
     EE_PLASTIC = 435,
     EE_PORTER = 436,
     EE_PSI = 437,
     EE_RAMAN = 438,
     EE_RAMAMU = 439,
     EE_SIERP = 440,
     EE_THETA = 441,
     EE_VISW = 442,
     EE_Z3 = 443,
     EE_ZETA = 444,
     EE_M_E = 445,
     EE_M_LOG2E = 446,
     EE_M_LOG10E = 447,
     EE_M_LN2 = 448,
     EE_M_LN10 = 449,
     EE_M_PI = 450,
     EE_M_PI_2 = 451,
     EE_M_PI_4 = 452,
     EE_M_1_PI = 453,
     EE_M_2_PI = 454,
     EE_M_2_SQRTPI = 455,
     EE_M_SQRT2 = 456,
     EE_M_SQRT1_2 = 457,
     EE_CHAR_BIT = 458,
     EE_MB_LEN_MAX = 459,
     EE_CHAR_MIN = 460,
     EE_CHAR_MAX = 461,
     EE_SCHAR_MIN = 462,
     EE_SHRT_MIN = 463,
     EE_INT_MIN = 464,
     EE_LONG_MIN = 465,
     EE_LLONG_MIN = 466,
     EE_SCHAR_MAX = 467,
     EE_SHRT_MAX = 468,
     EE_INT_MAX = 469,
     EE_LONG_MAX = 470,
     EE_LLONG_MAX = 471,
     EE_UCHAR_MAX = 472,
     EE_USHRT_MAX = 473,
     EE_UINT_MAX = 474,
     EE_ULONG_MAX = 475,
     EE_ULLONG_MAX = 476,
     EE_FLT_RADIX = 477,
     EE_DECIMAL_DIG = 478,
     EE_FLT_DECIMAL_DIG = 479,
     EE_DBL_DECIMAL_DIG = 480,
     EE_LDBL_DECIMAL_DIG = 481,
     EE_FLT_MIN = 482,
     EE_DBL_MIN = 483,
     EE_LDBL_MIN = 484,
     EE_FLT_TRUE_MIN = 485,
     EE_DBL_TRUE_MIN = 486,
     EE_LDBL_TRUE_MIN = 487,
     EE_FLT_MAX = 488,
     EE_DBL_MAX = 489,
     EE_LDBL_MAX = 490,
     EE_FLT_EPSILON = 491,
     EE_DBL_EPSILON = 492,
     EE_LDBL_EPSILON = 493,
     EE_FLT_DIG = 494,
     EE_DBL_DIG = 495,
     EE_LDBL_DIG = 496,
     EE_FLT_MANT_DIG = 497,
     EE_DBL_MANT_DIG = 498,
     EE_LDBL_MANT_DIG = 499,
     EE_FLT_MIN_EXP = 500,
     EE_DBL_MIN_EXP = 501,
     EE_LDBL_MIN_EXP = 502,
     EE_FLT_MIN_10_EXP = 503,
     EE_DBL_MIN_10_EXP = 504,
     EE_LDBL_MIN_10_EXP = 505,
     EE_FLT_MAX_EXP = 506,
     EE_DBL_MAX_EXP = 507,
     EE_LDBL_MAX_EXP = 508,
     EE_FLT_MAX_10_EXP = 509,
     EE_DBL_MAX_10_EXP = 510,
     EE_LDBL_MAX_10_EXP = 511,
     EE_INT8_MIN = 512,
     EE_INT16_MIN = 513,
     EE_INT32_MIN = 514,
     EE_INT64_MIN = 515,
     EE_INT8_MAX = 516,
     EE_INT16_MAX = 517,
     EE_INT32_MAX = 518,
     EE_INT64_MAX = 519,
     EE_UINT8_MAX = 520,
     EE_UINT16_MAX = 521,
     EE_UINT32_MAX = 522,
     EE_UINT64_MAX = 523,
     EE_INT_FAST8_MIN = 524,
     EE_INT_FAST16_MIN = 525,
     EE_INT_FAST32_MIN = 526,
     EE_INT_FAST64_MIN = 527,
     EE_INT_LEAST8_MIN = 528,
     EE_INT_LEAST16_MIN = 529,
     EE_INT_LEAST32_MIN = 530,
     EE_INT_LEAST64_MIN = 531,
     EE_INT_FAST8_MAX = 532,
     EE_INT_FAST16_MAX = 533,
     EE_INT_FAST32_MAX = 534,
     EE_INT_FAST64_MAX = 535,
     EE_INT_LEAST8_MAX = 536,
     EE_INT_LEAST16_MAX = 537,
     EE_INT_LEAST32_MAX = 538,
     EE_INT_LEAST64_MAX = 539,
     EE_UINT_FAST8_MAX = 540,
     EE_UINT_FAST16_MAX = 541,
     EE_UINT_FAST32_MAX = 542,
     EE_UINT_FAST64_MAX = 543,
     EE_AS_FLOAT = 544,
     EE_AS_DOUBLE = 545,
     EE_AS_FLOAT24 = 546,
     EE_AS_FLOAT16 = 547,
     EE_AS_FLOAT14 = 548,
     EE_AS_FLOAT11 = 549,
     EE_AS_FLOAT10 = 550,
     EE_AS_FLOAT_MAX = 551,
     EE_AS_FLOAT_MIN = 552,
     EE_AS_FLOAT_TRUE_MIN = 553,
     EE_AS_FLOAT_NAN = 554,
     EE_AS_FLOAT_INF = 555,
     EE_AS_FLOAT_SUBNORM_MAX = 556,
     EE_AS_FLOAT_EPS = 557,
     EE_TRUE = 558,
     EE_FALSE = 559,
     EE_AS_FLOAT_SIGNBIT = 560,
     EE_AS_FLOAT_EXPBITS = 561,
     EE_AS_FLOAT_MANBITS = 562,
     EE_TO_FLOAT32 = 563,
     EE_TO_FLOAT64 = 564,
     EE_TO_INT8 = 565,
     EE_TO_INT16 = 566,
     EE_TO_INT32 = 567,
     EE_TO_INT64 = 568,
     EE_TO_UINT8 = 569,
     EE_TO_UINT16 = 570,
     EE_TO_UINT32 = 571,
     EE_TO_UINT64 = 572,
     EE_CLOCK = 573,
     EE_CLOCKS_PER_SEC = 574,
     EE_SECONDS = 575,
     EE_MILLISECONDS = 576,
     EE_MICROSECONDS = 577,
     EE_SECONDS_SINCE_START = 578,
     EE_MILLISECONDS_SINCE_START = 579,
     EE_MICROSECONDS_SINCE_START = 580,
     EE_ASCII = 581,
     EE_BIN = 582,
     EE_BOOL = 583,
     EE_CHR = 584,
     EE_HEX = 585,
     EE_INT = 586,
     EE_LEN = 587,
     EE_OCT = 588,
     EE_ORD = 589,
     EE_ADD = 590,
     EE_APPEND = 591,
     EE_ASSIGN = 592,
     EE_AT = 593,
     EE_CAPACITY = 594,
     EE_CLEAR = 595,
     EE_CROSS = 596,
     EE_DOT = 597,
     EE_EMPTY = 598,
     EE_ERASE = 599,
     EE_INSERT = 600,
     EE_MAX_SIZE = 601,
     EE_MAG = 602,
     EE_MAGSQ = 603,
     EE_MUL = 604,
     EE_NORMALIZE = 605,
     EE_RESERVE = 606,
     EE_RESIZE = 607,
     EE_POP_BACK = 608,
     EE_PUSH_BACK = 609,
     EE_SHRINK_TO_FIT = 610,
     EE_SIZE = 611,
     EE_SUB = 612,
     EE_SUM = 613,
     EE_SWAP = 614,
     EE_TOKENIZE = 615,
     EE_BARTHANN = 616,
     EE_BARTLETT = 617,
     EE_BLACKMAN = 618,
     EE_BLACKMANHARRIS = 619,
     EE_BLACKMANNUTTAL = 620,
     EE_BOHMAN = 621,
     EE_BOXCAR = 622,
     EE_CHEBWIN = 623,
     EE_COSINE = 624,
     EE_EXPONENTIAL = 625,
     EE_FLATTOP = 626,
     EE_GAUSSIAN = 627,
     EE_GENERAL_COSINE = 628,
     EE_GENERAL_GUASSIAN = 629,
     EE_GENERAL_HAMMING = 630,
     EE_HAMMING = 631,
     EE_HANN = 632,
     EE_KAISER = 633,
     EE_KAISER_BESSEL = 634,
     EE_LANCZOS = 635,
     EE_NUTTAL = 636,
     EE_PARZEN = 637,
     EE_TAYLOR = 638,
     EE_TRIANG = 639,
     EE_TUKEY = 640,
     EE_CUM_SIMPSON = 641,
     EE_CUM_TRAPEZOID = 642,
     EE_ROMB = 643,
     EE_SIMPSON = 644,
     EE_TRAPEZOID = 645,
     EE_ARANGE = 646,
     EE_FULL = 647,
     EE_FULL_LIKE = 648,
     EE_GEOMSPACE = 649,
     EE_LINSPACE = 650,
     EE_LOGSPACE = 651,
     EE_ONES = 652,
     EE_ONES_LIKE = 653,
     EE_ZEROS = 654,
     EE_ZEROS_LIKE = 655,
     EE_OR = 656,
     EE_AND = 657,
     EE_LOG_NOT = 658,
     EE_UNARY_1SCOMPL = 659,
     EE_UPLUS = 660,
     EE_UMINUS = 661
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
    static const short int yytable_[];
    static const short int yytable_ninf_;

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

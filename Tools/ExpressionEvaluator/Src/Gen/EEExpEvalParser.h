
/* A Bison CExpEvalParser, made by GNU Bison 2.4.1.  */

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
   part or all of the Bison CExpEvalParser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   CExpEvalParser generator using the skeleton or a modified version thereof
   as a CExpEvalParser skeleton.  Alternatively, if you modify or redistribute
   the CExpEvalParser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C++ LALR(1) CExpEvalParser skeleton written by Akim Demaille.  */

#ifndef __EE_EXPEVAL_PARSER_H__
# define __EE_EXPEVAL_PARSER_H__





#include "EEExpEvalParserStack.h"



namespace yy {

  class position;
  class location;


} // yy


#include "EEExpEvalParserLocation.h"

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


  /// A Bison CExpEvalParser.
  class CExpEvalParser
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
     EE_SINC = 390,
     EE_BYTESWAPUSHORT = 391,
     EE_BYTESWAPULONG = 392,
     EE_BYTESWAPUINT64 = 393,
     EE_BITSWAP8 = 394,
     EE_BITSWAP16 = 395,
     EE_BITSWAP32 = 396,
     EE_BITSWAP64 = 397,
     EE_BITSWAPF16 = 398,
     EE_BITSWAPF32 = 399,
     EE_BITSWAPF64 = 400,
     EE_FORMAT = 401,
     EE_A = 402,
     EE_ALLADI = 403,
     EE_ALPHA = 404,
     EE_B = 405,
     EE_B2 = 406,
     EE_B4 = 407,
     EE_BETA = 408,
     EE_BH = 409,
     EE_C2 = 410,
     EE_CAHEN = 411,
     EE_CATALAN = 412,
     EE_CONWAY = 413,
     EE_DELTA = 414,
     EE_E = 415,
     EE_ERDOS = 416,
     EE_EULER = 417,
     EE_F = 418,
     EE_GR = 419,
     EE_GWK = 420,
     EE_HALFPI = 421,
     EE_HSMC = 422,
     EE_ICE = 423,
     EE_K = 424,
     EE_LAMBDA = 425,
     EE_LAPLACE = 426,
     EE_LEVY = 427,
     EE_M1 = 428,
     EE_MU = 429,
     EE_NIVEN = 430,
     EE_OMEGA = 431,
     EE_P2 = 432,
     EE_PI_ = 433,
     EE_PLASTIC = 434,
     EE_PORTER = 435,
     EE_PSI = 436,
     EE_RAMAN = 437,
     EE_RAMAMU = 438,
     EE_SIERP = 439,
     EE_THETA = 440,
     EE_VISW = 441,
     EE_Z3 = 442,
     EE_ZETA = 443,
     EE_CHAR_BIT = 444,
     EE_MB_LEN_MAX = 445,
     EE_CHAR_MIN = 446,
     EE_CHAR_MAX = 447,
     EE_SCHAR_MIN = 448,
     EE_SHRT_MIN = 449,
     EE_INT_MIN = 450,
     EE_LONG_MIN = 451,
     EE_LLONG_MIN = 452,
     EE_SCHAR_MAX = 453,
     EE_SHRT_MAX = 454,
     EE_INT_MAX = 455,
     EE_LONG_MAX = 456,
     EE_LLONG_MAX = 457,
     EE_UCHAR_MAX = 458,
     EE_USHRT_MAX = 459,
     EE_UINT_MAX = 460,
     EE_ULONG_MAX = 461,
     EE_ULLONG_MAX = 462,
     EE_FLT_RADIX = 463,
     EE_DECIMAL_DIG = 464,
     EE_FLT_DECIMAL_DIG = 465,
     EE_DBL_DECIMAL_DIG = 466,
     EE_LDBL_DECIMAL_DIG = 467,
     EE_FLT_MIN = 468,
     EE_DBL_MIN = 469,
     EE_LDBL_MIN = 470,
     EE_FLT_TRUE_MIN = 471,
     EE_DBL_TRUE_MIN = 472,
     EE_LDBL_TRUE_MIN = 473,
     EE_FLT_MAX = 474,
     EE_DBL_MAX = 475,
     EE_LDBL_MAX = 476,
     EE_FLT_EPSILON = 477,
     EE_DBL_EPSILON = 478,
     EE_LDBL_EPSILON = 479,
     EE_FLT_DIG = 480,
     EE_DBL_DIG = 481,
     EE_LDBL_DIG = 482,
     EE_FLT_MANT_DIG = 483,
     EE_DBL_MANT_DIG = 484,
     EE_LDBL_MANT_DIG = 485,
     EE_FLT_MIN_EXP = 486,
     EE_DBL_MIN_EXP = 487,
     EE_LDBL_MIN_EXP = 488,
     EE_FLT_MIN_10_EXP = 489,
     EE_DBL_MIN_10_EXP = 490,
     EE_LDBL_MIN_10_EXP = 491,
     EE_FLT_MAX_EXP = 492,
     EE_DBL_MAX_EXP = 493,
     EE_LDBL_MAX_EXP = 494,
     EE_FLT_MAX_10_EXP = 495,
     EE_DBL_MAX_10_EXP = 496,
     EE_LDBL_MAX_10_EXP = 497,
     EE_INT8_MIN = 498,
     EE_INT16_MIN = 499,
     EE_INT32_MIN = 500,
     EE_INT64_MIN = 501,
     EE_INT8_MAX = 502,
     EE_INT16_MAX = 503,
     EE_INT32_MAX = 504,
     EE_INT64_MAX = 505,
     EE_UINT8_MAX = 506,
     EE_UINT16_MAX = 507,
     EE_UINT32_MAX = 508,
     EE_UINT64_MAX = 509,
     EE_AS_FLOAT = 510,
     EE_AS_DOUBLE = 511,
     EE_AS_FLOAT24 = 512,
     EE_AS_FLOAT16 = 513,
     EE_AS_FLOAT14 = 514,
     EE_AS_FLOAT11 = 515,
     EE_AS_FLOAT10 = 516,
     EE_AS_FLOAT_MAX = 517,
     EE_AS_FLOAT_MIN = 518,
     EE_AS_FLOAT_TRUE_MIN = 519,
     EE_AS_FLOAT_NAN = 520,
     EE_AS_FLOAT_INF = 521,
     EE_AS_FLOAT_SUBNORM_MAX = 522,
     EE_AS_FLOAT_EPS = 523,
     EE_TRUE = 524,
     EE_FALSE = 525,
     EE_AS_FLOAT_SIGNBIT = 526,
     EE_AS_FLOAT_EXPBITS = 527,
     EE_AS_FLOAT_MANBITS = 528,
     EE_TO_FLOAT32 = 529,
     EE_TO_FLOAT64 = 530,
     EE_TO_INT8 = 531,
     EE_TO_INT16 = 532,
     EE_TO_INT32 = 533,
     EE_TO_INT64 = 534,
     EE_TO_UINT8 = 535,
     EE_TO_UINT16 = 536,
     EE_TO_UINT32 = 537,
     EE_TO_UINT64 = 538,
     EE_CLOCK = 539,
     EE_CLOCKS_PER_SEC = 540,
     EE_SECONDS = 541,
     EE_MILLISECONDS = 542,
     EE_MICROSECONDS = 543,
     EE_SECONDS_SINCE_START = 544,
     EE_MILLISECONDS_SINCE_START = 545,
     EE_MICROSECONDS_SINCE_START = 546,
     EE_ASCII = 547,
     EE_BIN = 548,
     EE_BOOL = 549,
     EE_CHR = 550,
     EE_HEX = 551,
     EE_INT = 552,
     EE_LEN = 553,
     EE_OCT = 554,
     EE_ORD = 555,
     EE_ADD = 556,
     EE_APPEND = 557,
     EE_ASSIGN = 558,
     EE_AT = 559,
     EE_CAPACITY = 560,
     EE_CLEAR = 561,
     EE_CROSS = 562,
     EE_DOT = 563,
     EE_EMPTY = 564,
     EE_ERASE = 565,
     EE_INSERT = 566,
     EE_MAX_SIZE = 567,
     EE_MAG = 568,
     EE_MAGSQ = 569,
     EE_MUL = 570,
     EE_NORMALIZE = 571,
     EE_RESERVE = 572,
     EE_RESIZE = 573,
     EE_POP_BACK = 574,
     EE_PUSH_BACK = 575,
     EE_SHRINK_TO_FIT = 576,
     EE_SIZE = 577,
     EE_SUB = 578,
     EE_SUM = 579,
     EE_SWAP = 580,
     EE_TOKENIZE = 581,
     EE_BARTLETT = 582,
     EE_BLACKMAN = 583,
     EE_HAMMING = 584,
     EE_HANN = 585,
     EE_KAISER = 586,
     EE_OR = 587,
     EE_AND = 588,
     EE_LOG_NOT = 589,
     EE_UNARY_1SCOMPL = 590,
     EE_UPLUS = 591,
     EE_UMINUS = 592
   };

    };
    /// Token type.
    typedef token::yytokentype token_type;

    /// Build a CExpEvalParser object.
    CExpEvalParser (class CExpEvalLexer * m_peelLexer_yyarg, class CExpEvalContainer * m_peecContainer_yyarg);
    virtual ~CExpEvalParser ();

    /// Parse.
    /// \returns  0 if parsing succeeded.
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




#endif /* ! defined __EE_EXPEVAL_PARSER_H__ */

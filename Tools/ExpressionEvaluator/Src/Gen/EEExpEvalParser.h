
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
     EE_CHAR_CONSTANT = 258,
     EE_FLOAT_CONSTANT = 259,
     EE_STRING_CONSTANT = 260,
     EE_HEX_CONSTANT1 = 261,
     EE_HEX_CONSTANT2 = 262,
     EE_HEX_CONSTANT3 = 263,
     EE_DEC_CONSTANT = 264,
     EE_OCT_CONSTANT = 265,
     EE_PUREDEC_CONSTANT = 266,
     EE_IDENTIFIER = 267,
     EE_MEMBERACCESS = 268,
     EE_USER_VAR = 269,
     EE_EQU_E = 270,
     EE_EQU_NE = 271,
     EE_LEFT_OP = 272,
     EE_RIGHT_OP = 273,
     EE_REL_GE = 274,
     EE_REL_LE = 275,
     EE_OB_BYTE = 276,
     EE_OB_SBYTE = 277,
     EE_OB_WORD = 278,
     EE_OB_SWORD = 279,
     EE_OB_QWORD = 280,
     EE_OB_SQWORD = 281,
     EE_OB_FLOAT = 282,
     EE_OB_DOUBLE = 283,
     EE_OB_DWORD = 284,
     EE_OB_SDWORD = 285,
     EE_COS = 286,
     EE_SIN = 287,
     EE_TAN = 288,
     EE_ACOS = 289,
     EE_ASIN = 290,
     EE_ATAN = 291,
     EE_ATAN2 = 292,
     EE_COSH = 293,
     EE_SINH = 294,
     EE_TANH = 295,
     EE_ACOSH = 296,
     EE_ASINH = 297,
     EE_ATANH = 298,
     EE_EXP = 299,
     EE_LOG = 300,
     EE_LOG10 = 301,
     EE_LOG2 = 302,
     EE_EXP2 = 303,
     EE_EXPM1 = 304,
     EE_ILOGB = 305,
     EE_LOG1P = 306,
     EE_LOGB = 307,
     EE_POW = 308,
     EE_SQRT = 309,
     EE_CBRT = 310,
     EE_HYPOT = 311,
     EE_TGAMMA = 312,
     EE_LGAMMA = 313,
     EE_CEIL = 314,
     EE_FLOOR = 315,
     EE_MOD = 316,
     EE_TRUNC = 317,
     EE_ROUND = 318,
     EE_NEARBYINT = 319,
     EE_REMAINDER = 320,
     EE_REMQUO = 321,
     EE_NEXTAFTER = 322,
     EE_NEXTAFTERF = 323,
     EE_NEXTTOWARD = 324,
     EE_NEXTTOWARDF = 325,
     EE_DIM = 326,
     EE_MAX = 327,
     EE_MIN = 328,
     EE_ABS = 329,
     EE_MADD = 330,
     EE_A = 331,
     EE_ALLADI = 332,
     EE_ALPHA = 333,
     EE_B = 334,
     EE_B2 = 335,
     EE_B4 = 336,
     EE_BETA = 337,
     EE_BH = 338,
     EE_C2 = 339,
     EE_CAHEN = 340,
     EE_CATALAN = 341,
     EE_CONWAY = 342,
     EE_DELTA = 343,
     EE_E = 344,
     EE_ERDOS = 345,
     EE_EULER = 346,
     EE_F = 347,
     EE_GR = 348,
     EE_GWK = 349,
     EE_HALFPI = 350,
     EE_HSMC = 351,
     EE_ICE = 352,
     EE_K = 353,
     EE_LAMBDA = 354,
     EE_LAPLACE = 355,
     EE_LEVY = 356,
     EE_M1 = 357,
     EE_MU = 358,
     EE_NIVEN = 359,
     EE_OMEGA = 360,
     EE_P2 = 361,
     EE_PI = 362,
     EE_PLASTIC = 363,
     EE_PORTER = 364,
     EE_PSI = 365,
     EE_RAMAN = 366,
     EE_RAMAMU = 367,
     EE_SIERP = 368,
     EE_THETA = 369,
     EE_VISW = 370,
     EE_Z3 = 371,
     EE_ZETA = 372,
     EE_CHAR_BIT = 373,
     EE_MB_LEN_MAX = 374,
     EE_CHAR_MIN = 375,
     EE_CHAR_MAX = 376,
     EE_SCHAR_MIN = 377,
     EE_SHRT_MIN = 378,
     EE_INT_MIN = 379,
     EE_LONG_MIN = 380,
     EE_LLONG_MIN = 381,
     EE_SCHAR_MAX = 382,
     EE_SHRT_MAX = 383,
     EE_INT_MAX = 384,
     EE_LONG_MAX = 385,
     EE_LLONG_MAX = 386,
     EE_UCHAR_MAX = 387,
     EE_USHRT_MAX = 388,
     EE_UINT_MAX = 389,
     EE_ULONG_MAX = 390,
     EE_ULLONG_MAX = 391,
     EE_FLT_RADIX = 392,
     EE_DECIMAL_DIG = 393,
     EE_FLT_DECIMAL_DIG = 394,
     EE_DBL_DECIMAL_DIG = 395,
     EE_LDBL_DECIMAL_DIG = 396,
     EE_FLT_MIN = 397,
     EE_DBL_MIN = 398,
     EE_LDBL_MIN = 399,
     EE_FLT_TRUE_MIN = 400,
     EE_DBL_TRUE_MIN = 401,
     EE_LDBL_TRUE_MIN = 402,
     EE_FLT_MAX = 403,
     EE_DBL_MAX = 404,
     EE_LDBL_MAX = 405,
     EE_FLT_EPSILON = 406,
     EE_DBL_EPSILON = 407,
     EE_LDBL_EPSILON = 408,
     EE_FLT_DIG = 409,
     EE_DBL_DIG = 410,
     EE_LDBL_DIG = 411,
     EE_FLT_MANT_DIG = 412,
     EE_DBL_MANT_DIG = 413,
     EE_LDBL_MANT_DIG = 414,
     EE_FLT_MIN_EXP = 415,
     EE_DBL_MIN_EXP = 416,
     EE_LDBL_MIN_EXP = 417,
     EE_FLT_MIN_10_EXP = 418,
     EE_DBL_MIN_10_EXP = 419,
     EE_LDBL_MIN_10_EXP = 420,
     EE_FLT_MAX_EXP = 421,
     EE_DBL_MAX_EXP = 422,
     EE_LDBL_MAX_EXP = 423,
     EE_FLT_MAX_10_EXP = 424,
     EE_DBL_MAX_10_EXP = 425,
     EE_LDBL_MAX_10_EXP = 426,
     EE_AS_FLOAT = 427,
     EE_AS_DOUBLE = 428,
     EE_TRUE = 429,
     EE_FALSE = 430,
     EE_OR = 431,
     EE_AND = 432,
     EE_LOG_NOT = 433,
     EE_UNARY_1SCOMPL = 434,
     EE_UPLUS = 435,
     EE_UMINUS = 436
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




#endif /* ! defined __EE_EXPEVAL_PARSER_H__ */

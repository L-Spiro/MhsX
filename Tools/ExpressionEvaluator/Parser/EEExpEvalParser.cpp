
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
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


/* First part of user declarations.  */




#include "../EEExpEval.h"
#include "../EEExpEvalContainer.h"
#include "../EEExpEvalLexer.h"
#include <cstdio>

#pragma warning( push )

// warning C4065: switch statement contains 'default' but no 'case' labels
#pragma warning( disable : 4065 )
// warning C4127: conditional exp is constant
#pragma warning( disable : 4127 )
// warning C4512: 'slice<T,S>' : assignment operator could not be generated
#pragma warning( disable : 4512 )


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YYINCLUDED_STDLIB_H

#ifndef LDBL_DECIMAL_DIG
#define LDBL_DECIMAL_DIG DBL_DECIMAL_DIG
#endif	// #ifndef LDBL_DECIMAL_DIG


// Announce to Flex the prototype we want for lexing function.
extern int yylex( /*YYSTYPE*/void * _pvNodeUnion, ee::CExpEvalLexer * _peelLexer );


#include "../EEExpEvalSyntaxNodes.h"






#include "EEExpEvalParser.hpp"

/* User implementation prologue.  */



#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)



namespace yy {

#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

#endif

  /// Build a parser object.
  parser::parser (class CExpEvalLexer * m_peelLexer_yyarg, class CExpEvalContainer * m_peecContainer_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      m_peelLexer (m_peelLexer_yyarg),
      m_peecContainer (m_peecContainer_yyarg)
  {
  }

  parser::~parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  int
  parser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    semantic_type yylval;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, m_peelLexer);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:

    { (yyval.sStringIndex) = m_peecContainer->CreateString( m_peelLexer->YYText() ); }
    break;

  case 3:

    { (yyval.sStringIndex) = m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ); }
    break;

  case 4:

    { m_peecContainer->CreateStringBasicExp( (yysemantic_stack_[(1) - (1)].sStringIndex), (yyval.ndData) ); }
    break;

  case 5:

    { m_peecContainer->CreateHex1( m_peelLexer->YYText(), (yyval.ndData) ); }
    break;

  case 6:

    { m_peecContainer->CreateHex2( m_peelLexer->YYText(), (yyval.ndData) ); }
    break;

  case 7:

    { m_peecContainer->CreateHex3( m_peelLexer->YYText(), (yyval.ndData) ); }
    break;

  case 8:

    { m_peecContainer->CreateBin( m_peelLexer->YYText(), (yyval.ndData) ); }
    break;

  case 9:

    {
																if ( m_peecContainer->TreatAllAsHex() ) {
																	m_peecContainer->CreateHex3( m_peelLexer->YYText(), (yyval.ndData) );
																}
																else {
																	m_peecContainer->CreateUInt( m_peelLexer->YYText(), (yyval.ndData) );
																}
															}
    break;

  case 10:

    { m_peecContainer->CreateUInt( m_peelLexer->YYText() + 1, (yyval.ndData) ); }
    break;

  case 11:

    { m_peecContainer->CreateOct( m_peelLexer->YYText(), (yyval.ndData) ); }
    break;

  case 12:

    { m_peecContainer->CreateChar( m_peelLexer->YYText(), (yyval.ndData) ); }
    break;

  case 13:

    {
																if ( m_peecContainer->TreatAllAsHex() && ValidHex( m_peelLexer->YYText() ) ) {
																	m_peecContainer->CreateHex3( m_peelLexer->YYText(), (yyval.ndData) );
																}
																else {
																	m_peecContainer->CreateDouble( m_peelLexer->YYText(), (yyval.ndData) );
																}
															}
    break;

  case 14:

    { m_peecContainer->CreateDouble( 3.1415926535897932384626433832795, (yyval.ndData) ); }
    break;

  case 15:

    { m_peecContainer->CreateDouble( 1.5707963267948966192313216916398, (yyval.ndData) ); }
    break;

  case 16:

    { m_peecContainer->CreateDouble( 2.7182818284590452353602874713527, (yyval.ndData) ); }
    break;

  case 17:

    { m_peecContainer->CreateDouble( 1.202056903159594285399738161511449990764986292, (yyval.ndData) ); }
    break;

  case 18:

    { m_peecContainer->CreateDouble( 1.6180339887498948482045868343656381177203091798057628621, (yyval.ndData) ); }
    break;

  case 19:

    { m_peecContainer->CreateDouble( 0.5772156649015328606065120900824024310421, (yyval.ndData) ); }
    break;

  case 20:

    { m_peecContainer->CreateDouble( 2.685452001065306445309714835481795693820382293994462953051152345557218, (yyval.ndData) ); }
    break;

  case 21:

    { m_peecContainer->CreateDouble( 1.2824271291006226368753425688697917277676889273250011920637400217404, (yyval.ndData) ); }
    break;

  case 22:

    { m_peecContainer->CreateDouble( 0.26149721284764278375542683860869585, (yyval.ndData) ); }
    break;

  case 23:

    { m_peecContainer->CreateDouble( 0.28016949902386913303, (yyval.ndData) ); }
    break;

  case 24:

    { m_peecContainer->CreateDouble( 0.30366300289873265859744812190155623, (yyval.ndData) ); }
    break;

  case 25:

    { m_peecContainer->CreateDouble( 0.35323637185499598454351655043268201, (yyval.ndData) ); }
    break;

  case 26:

    { m_peecContainer->CreateDouble( 0.56714329040978387299996866221035554, (yyval.ndData) ); }
    break;

  case 27:

    { m_peecContainer->CreateDouble( 0.62432998854355087099293638310083724, (yyval.ndData) ); }
    break;

  case 28:

    { m_peecContainer->CreateDouble( 0.6434105462, (yyval.ndData) ); }
    break;

  case 29:

    { m_peecContainer->CreateDouble( 0.66016181584686957392781211001455577, (yyval.ndData) ); }
    break;

  case 30:

    { m_peecContainer->CreateDouble( 0.66274341934918158097474209710925290, (yyval.ndData) ); }
    break;

  case 31:

    { m_peecContainer->CreateDouble( 0.70258, (yyval.ndData) ); }
    break;

  case 32:

    { m_peecContainer->CreateDouble( 0.76422365358922066299069873125009232, (yyval.ndData) ); }
    break;

  case 33:

    { m_peecContainer->CreateDouble( 0.8093940205, (yyval.ndData) ); }
    break;

  case 34:

    { m_peecContainer->CreateDouble( 0.87058838, (yyval.ndData) ); }
    break;

  case 35:

    { m_peecContainer->CreateDouble( 0.91596559417721901505460351493238411, (yyval.ndData) ); }
    break;

  case 36:

    { m_peecContainer->CreateDouble( 1.0986858055, (yyval.ndData) ); }
    break;

  case 37:

    { m_peecContainer->CreateDouble( 1.13198824, (yyval.ndData) ); }
    break;

  case 38:

    { m_peecContainer->CreateDouble( 1.20205690315959428539973816151144999, (yyval.ndData) ); }
    break;

  case 39:

    { m_peecContainer->CreateDouble( 1.30357726903429639125709911215255189, (yyval.ndData) ); }
    break;

  case 40:

    { m_peecContainer->CreateDouble( 1.30637788386308069046861449260260571, (yyval.ndData) ); }
    break;

  case 41:

    { m_peecContainer->CreateDouble( 1.32471795724474602596090885447809734, (yyval.ndData) ); }
    break;

  case 42:

    { m_peecContainer->CreateDouble( 1.45136923488338105028396848589202744, (yyval.ndData) ); }
    break;

  case 43:

    { m_peecContainer->CreateDouble( 1.45607494858268967139959535111654356, (yyval.ndData) ); }
    break;

  case 44:

    { m_peecContainer->CreateDouble( 1.4670780794, (yyval.ndData) ); }
    break;

  case 45:

    { m_peecContainer->CreateDouble( 1.5396007178, (yyval.ndData) ); }
    break;

  case 46:

    { m_peecContainer->CreateDouble( 1.60669515241529176378330152319092458, (yyval.ndData) ); }
    break;

  case 47:

    { m_peecContainer->CreateDouble( 1.70521114010536776428855145343450816, (yyval.ndData) ); }
    break;

  case 48:

    { m_peecContainer->CreateDouble( 1.902160583104, (yyval.ndData) ); }
    break;

  case 49:

    { m_peecContainer->CreateDouble( 2.29558714939263807403429804918949039, (yyval.ndData) ); }
    break;

  case 50:

    { m_peecContainer->CreateDouble( 2.50290787509589282228390287321821578, (yyval.ndData) ); }
    break;

  case 51:

    { m_peecContainer->CreateDouble( 2.58498175957925321706589358738317116, (yyval.ndData) ); }
    break;

  case 52:

    { m_peecContainer->CreateDouble( 2.80777024202851936522150118655777293, (yyval.ndData) ); }
    break;

  case 53:

    { m_peecContainer->CreateDouble( 3.27582291872181115978768188245384386, (yyval.ndData) ); }
    break;

  case 54:

    { m_peecContainer->CreateDouble( 3.35988566624317755317201130291892717, (yyval.ndData) ); }
    break;

  case 55:

    { m_peecContainer->CreateDouble( 4.66920160910299067185320382046620161, (yyval.ndData) ); }
    break;

  case 56:

    { m_peecContainer->CreateNumber( CHAR_BIT, (yyval.ndData) ); }
    break;

  case 57:

    { m_peecContainer->CreateNumber( MB_LEN_MAX, (yyval.ndData) ); }
    break;

  case 58:

    { m_peecContainer->CreateNumber( CHAR_MIN, (yyval.ndData) ); }
    break;

  case 59:

    { m_peecContainer->CreateNumber( CHAR_MAX, (yyval.ndData) ); }
    break;

  case 60:

    { m_peecContainer->CreateNumber( SCHAR_MIN, (yyval.ndData) ); }
    break;

  case 61:

    { m_peecContainer->CreateNumber( SHRT_MIN, (yyval.ndData) ); }
    break;

  case 62:

    { m_peecContainer->CreateNumber( INT_MIN, (yyval.ndData) ); }
    break;

  case 63:

    { m_peecContainer->CreateNumber( LONG_MIN, (yyval.ndData) ); }
    break;

  case 64:

    { m_peecContainer->CreateNumber( LLONG_MIN, (yyval.ndData) ); }
    break;

  case 65:

    { m_peecContainer->CreateNumber( SCHAR_MAX, (yyval.ndData) ); }
    break;

  case 66:

    { m_peecContainer->CreateNumber( SHRT_MAX, (yyval.ndData) ); }
    break;

  case 67:

    { m_peecContainer->CreateNumber( INT_MAX, (yyval.ndData) ); }
    break;

  case 68:

    { m_peecContainer->CreateNumber( LONG_MAX, (yyval.ndData) ); }
    break;

  case 69:

    { m_peecContainer->CreateNumber( LLONG_MAX, (yyval.ndData) ); }
    break;

  case 70:

    { m_peecContainer->CreateNumber( UCHAR_MAX, (yyval.ndData) ); }
    break;

  case 71:

    { m_peecContainer->CreateNumber( USHRT_MAX, (yyval.ndData) ); }
    break;

  case 72:

    { m_peecContainer->CreateNumber( UINT_MAX, (yyval.ndData) ); }
    break;

  case 73:

    { m_peecContainer->CreateNumber( static_cast<uint64_t>(ULONG_MAX), (yyval.ndData) ); }
    break;

  case 74:

    { m_peecContainer->CreateNumber( ULLONG_MAX, (yyval.ndData) ); }
    break;

  case 75:

    { m_peecContainer->CreateNumber( FLT_RADIX, (yyval.ndData) ); }
    break;

  case 76:

    { m_peecContainer->CreateNumber( DECIMAL_DIG, (yyval.ndData) ); }
    break;

  case 77:

    { m_peecContainer->CreateNumber( FLT_DECIMAL_DIG, (yyval.ndData) ); }
    break;

  case 78:

    { m_peecContainer->CreateNumber( DBL_DECIMAL_DIG, (yyval.ndData) ); }
    break;

  case 79:

    { m_peecContainer->CreateNumber( LDBL_DECIMAL_DIG, (yyval.ndData) ); }
    break;

  case 80:

    { m_peecContainer->CreateNumber( FLT_MIN, (yyval.ndData) ); }
    break;

  case 81:

    { m_peecContainer->CreateNumber( DBL_MIN, (yyval.ndData) ); }
    break;

  case 82:

    { m_peecContainer->CreateNumber( LDBL_MIN, (yyval.ndData) ); }
    break;

  case 83:

    { m_peecContainer->CreateNumber( FLT_TRUE_MIN, (yyval.ndData) ); }
    break;

  case 84:

    { m_peecContainer->CreateNumber( DBL_TRUE_MIN, (yyval.ndData) ); }
    break;

  case 85:

    { m_peecContainer->CreateNumber( LDBL_TRUE_MIN, (yyval.ndData) ); }
    break;

  case 86:

    { m_peecContainer->CreateNumber( FLT_MAX, (yyval.ndData) ); }
    break;

  case 87:

    { m_peecContainer->CreateNumber( DBL_MAX, (yyval.ndData) ); }
    break;

  case 88:

    { m_peecContainer->CreateNumber( LDBL_MAX, (yyval.ndData) ); }
    break;

  case 89:

    { m_peecContainer->CreateNumber( FLT_EPSILON, (yyval.ndData) ); }
    break;

  case 90:

    { m_peecContainer->CreateNumber( DBL_EPSILON, (yyval.ndData) ); }
    break;

  case 91:

    { m_peecContainer->CreateNumber( LDBL_EPSILON, (yyval.ndData) ); }
    break;

  case 92:

    { m_peecContainer->CreateNumber( FLT_DIG, (yyval.ndData) ); }
    break;

  case 93:

    { m_peecContainer->CreateNumber( DBL_DIG, (yyval.ndData) ); }
    break;

  case 94:

    { m_peecContainer->CreateNumber( LDBL_DIG, (yyval.ndData) ); }
    break;

  case 95:

    { m_peecContainer->CreateNumber( FLT_MANT_DIG, (yyval.ndData) ); }
    break;

  case 96:

    { m_peecContainer->CreateNumber( DBL_MANT_DIG, (yyval.ndData) ); }
    break;

  case 97:

    { m_peecContainer->CreateNumber( LDBL_MANT_DIG, (yyval.ndData) ); }
    break;

  case 98:

    { m_peecContainer->CreateNumber( FLT_MIN_EXP, (yyval.ndData) ); }
    break;

  case 99:

    { m_peecContainer->CreateNumber( DBL_MIN_EXP, (yyval.ndData) ); }
    break;

  case 100:

    { m_peecContainer->CreateNumber( LDBL_MIN_EXP, (yyval.ndData) ); }
    break;

  case 101:

    { m_peecContainer->CreateNumber( FLT_MIN_10_EXP, (yyval.ndData) ); }
    break;

  case 102:

    { m_peecContainer->CreateNumber( DBL_MIN_10_EXP, (yyval.ndData) ); }
    break;

  case 103:

    { m_peecContainer->CreateNumber( LDBL_MIN_10_EXP, (yyval.ndData) ); }
    break;

  case 104:

    { m_peecContainer->CreateNumber( FLT_MAX_EXP, (yyval.ndData) ); }
    break;

  case 105:

    { m_peecContainer->CreateNumber( DBL_MAX_EXP, (yyval.ndData) ); }
    break;

  case 106:

    { m_peecContainer->CreateNumber( LDBL_MAX_EXP, (yyval.ndData) ); }
    break;

  case 107:

    { m_peecContainer->CreateNumber( FLT_MAX_10_EXP, (yyval.ndData) ); }
    break;

  case 108:

    { m_peecContainer->CreateNumber( DBL_MAX_10_EXP, (yyval.ndData) ); }
    break;

  case 109:

    { m_peecContainer->CreateNumber( LDBL_MAX_10_EXP, (yyval.ndData) ); }
    break;

  case 110:

    { m_peecContainer->CreateNumber( 1, (yyval.ndData) ); }
    break;

  case 111:

    { m_peecContainer->CreateNumber( 0, (yyval.ndData) ); }
    break;

  case 112:

    { (yyval.ndData) = (yysemantic_stack_[(3) - (2)].ndData); }
    break;

  case 113:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT32, (yyval.ndData) ); }
    break;

  case 114:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT32, (yyval.ndData) ); }
    break;

  case 115:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT8, (yyval.ndData) ); }
    break;

  case 116:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT16, (yyval.ndData) ); }
    break;

  case 117:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT64, (yyval.ndData) ); }
    break;

  case 118:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_INT32, (yyval.ndData) ); }
    break;

  case 119:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_INT8, (yyval.ndData) ); }
    break;

  case 120:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_INT16, (yyval.ndData) ); }
    break;

  case 121:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_INT64, (yyval.ndData) ); }
    break;

  case 122:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_FLOAT, (yyval.ndData) ); }
    break;

  case 123:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_DOUBLE, (yyval.ndData) ); }
    break;

  case 124:

    { m_peecContainer->CreateUser( (yyval.ndData) ); }
    break;

  case 125:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 126:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 127:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 128:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateString( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 129:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 130:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 131:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 132:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 133:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 134:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 135:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 136:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 137:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 138:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 139:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 140:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 141:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 142:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 143:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 144:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 145:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 146:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 147:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 148:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 149:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 150:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 151:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 152:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 153:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 154:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 155:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 156:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 157:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 158:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 159:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 160:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 161:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 162:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 163:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 164:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 165:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 166:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 167:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 168:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 169:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 170:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 171:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 172:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 173:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 174:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 175:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 176:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 177:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 178:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 179:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 180:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 181:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 182:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 183:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 184:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 185:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 186:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 187:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 188:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 189:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 190:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 191:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 192:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 193:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 194:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 195:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 196:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 197:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 198:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 199:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 200:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 201:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 202:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 203:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 204:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 205:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 206:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 207:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 208:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 209:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 210:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 211:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 212:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 213:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 214:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 215:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 216:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 217:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 218:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 219:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 220:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 221:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 222:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 223:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 224:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 225:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 226:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 227:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 228:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 229:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 230:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 231:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 232:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 233:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 234:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 235:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 236:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 237:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 238:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 239:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 240:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 241:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 242:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 243:

    { m_peecContainer->CreateUnary( (yysemantic_stack_[(2) - (2)].ndData), (yysemantic_stack_[(2) - (1)].ui32Unary), (yyval.ndData) ); }
    break;

  case 244:

    { (yyval.ui32Unary) = '+'; }
    break;

  case 245:

    { (yyval.ui32Unary) = '-'; }
    break;

  case 246:

    { (yyval.ui32Unary) = '~'; }
    break;

  case 247:

    { (yyval.ui32Unary) = '!'; }
    break;

  case 248:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 249:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '*', (yyval.ndData) ); }
    break;

  case 250:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '/', (yyval.ndData) ); }
    break;

  case 251:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '%', (yyval.ndData) ); }
    break;

  case 252:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 253:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '+', (yyval.ndData) ); }
    break;

  case 254:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '-', (yyval.ndData) ); }
    break;

  case 255:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 256:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_RIGHT_OP, (yyval.ndData) ); }
    break;

  case 257:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_LEFT_OP, (yyval.ndData) ); }
    break;

  case 258:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 259:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '<', (yyval.ndData) ); }
    break;

  case 260:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '>', (yyval.ndData) ); }
    break;

  case 261:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_REL_LE, (yyval.ndData) ); }
    break;

  case 262:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_REL_GE, (yyval.ndData) ); }
    break;

  case 263:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 264:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_EQU_E, (yyval.ndData) ); }
    break;

  case 265:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_EQU_NE, (yyval.ndData) ); }
    break;

  case 266:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 267:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '&', (yyval.ndData) ); }
    break;

  case 268:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 269:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '^', (yyval.ndData) ); }
    break;

  case 270:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 271:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '|', (yyval.ndData) ); }
    break;

  case 272:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 273:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_AND, (yyval.ndData) ); }
    break;

  case 274:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 275:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_OR, (yyval.ndData) ); }
    break;

  case 276:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 277:

    { m_peecContainer->CreateConditional( (yysemantic_stack_[(5) - (1)].ndData), (yysemantic_stack_[(5) - (3)].ndData), (yysemantic_stack_[(5) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 278:

    { m_peecContainer->CreateIntrinsic1( token::EE_COS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 279:

    { m_peecContainer->CreateIntrinsic1( token::EE_SIN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 280:

    { m_peecContainer->CreateIntrinsic1( token::EE_TAN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 281:

    { m_peecContainer->CreateIntrinsic1( token::EE_ACOS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 282:

    { m_peecContainer->CreateIntrinsic1( token::EE_ASIN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 283:

    { m_peecContainer->CreateIntrinsic1( token::EE_ATAN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 284:

    { m_peecContainer->CreateIntrinsic2( token::EE_ATAN2, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 285:

    { m_peecContainer->CreateIntrinsic1( token::EE_COSH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 286:

    { m_peecContainer->CreateIntrinsic1( token::EE_SINH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 287:

    { m_peecContainer->CreateIntrinsic1( token::EE_TANH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 288:

    { m_peecContainer->CreateIntrinsic1( token::EE_ACOSH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 289:

    { m_peecContainer->CreateIntrinsic1( token::EE_ASINH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 290:

    { m_peecContainer->CreateIntrinsic1( token::EE_ATANH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 291:

    { m_peecContainer->CreateIntrinsic1( token::EE_EXP, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 292:

    { m_peecContainer->CreateIntrinsic1( token::EE_LOG, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 293:

    { m_peecContainer->CreateIntrinsic1( token::EE_LOG10, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 294:

    { m_peecContainer->CreateIntrinsic1( token::EE_LOG2, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 295:

    { m_peecContainer->CreateIntrinsic1( token::EE_EXP2, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 296:

    { m_peecContainer->CreateIntrinsic1( token::EE_EXPM1, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 297:

    { m_peecContainer->CreateIntrinsic1( token::EE_ILOGB, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 298:

    { m_peecContainer->CreateIntrinsic1( token::EE_LOG1P, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 299:

    { m_peecContainer->CreateIntrinsic1( token::EE_LOGB, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 300:

    { m_peecContainer->CreateIntrinsic2( token::EE_POW, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 301:

    { m_peecContainer->CreateIntrinsic1( token::EE_SQRT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 302:

    { m_peecContainer->CreateIntrinsic1( token::EE_CBRT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 303:

    { m_peecContainer->CreateIntrinsic2( token::EE_HYPOT, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 304:

    { m_peecContainer->CreateIntrinsic1( token::EE_TGAMMA, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 305:

    { m_peecContainer->CreateIntrinsic1( token::EE_LGAMMA, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 306:

    { m_peecContainer->CreateIntrinsic1( token::EE_CEIL, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 307:

    { m_peecContainer->CreateIntrinsic1( token::EE_FLOOR, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 308:

    { m_peecContainer->CreateIntrinsic2( token::EE_MOD, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 309:

    { m_peecContainer->CreateIntrinsic1( token::EE_TRUNC, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 310:

    { m_peecContainer->CreateIntrinsic1( token::EE_ROUND, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 311:

    { m_peecContainer->CreateIntrinsic1( token::EE_NEARBYINT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 312:

    { m_peecContainer->CreateIntrinsic2( token::EE_REMAINDER, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 313:

    { m_peecContainer->CreateIntrinsic2( token::EE_NEXTAFTER, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 314:

    { m_peecContainer->CreateIntrinsic2( token::EE_NEXTAFTERF, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 315:

    { m_peecContainer->CreateIntrinsic2( token::EE_NEXTTOWARD, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 316:

    { m_peecContainer->CreateIntrinsic2( token::EE_NEXTTOWARDF, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 317:

    { m_peecContainer->CreateIntrinsic2( token::EE_DIM, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 318:

    { m_peecContainer->CreateIntrinsic2( token::EE_MAX, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 319:

    { m_peecContainer->CreateIntrinsic2( token::EE_MIN, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 320:

    { m_peecContainer->CreateIntrinsic1( token::EE_ABS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 321:

    { m_peecContainer->CreateIntrinsic3( token::EE_MADD, (yysemantic_stack_[(8) - (3)].ndData), (yysemantic_stack_[(8) - (5)].ndData), (yysemantic_stack_[(8) - (7)].ndData), (yyval.ndData) ); }
    break;

  case 322:

    { m_peecContainer->CreateAsFloat( (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 323:

    { m_peecContainer->CreateAsDouble( (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 324:

    { m_peecContainer->CreateAsFloatX( (yysemantic_stack_[(16) - (3)].ndData), (yysemantic_stack_[(16) - (5)].ndData), (yysemantic_stack_[(16) - (7)].ndData), (yysemantic_stack_[(16) - (9)].ndData), (yysemantic_stack_[(16) - (11)].ndData), (yysemantic_stack_[(16) - (13)].ndData), (yysemantic_stack_[(16) - (15)].ndData), (yyval.ndData) ); }
    break;

  case 325:

    { m_peecContainer->CreateAsFloatX( (yysemantic_stack_[(12) - (3)].ndData), (yysemantic_stack_[(12) - (5)].ndData), (yysemantic_stack_[(12) - (7)].ndData), (yysemantic_stack_[(12) - (9)].ndData), (yysemantic_stack_[(12) - (11)].ndData), (yyval.ndData) ); }
    break;

  case 326:

    { m_peecContainer->CreateAsFloat( (yysemantic_stack_[(4) - (3)].ndData), ee::EE_N_ASFLOAT16, (yyval.ndData) ); }
    break;

  case 327:

    { m_peecContainer->CreateAsFloat( (yysemantic_stack_[(4) - (3)].ndData), ee::EE_N_ASFLOAT14, (yyval.ndData) ); }
    break;

  case 328:

    { m_peecContainer->CreateAsFloat( (yysemantic_stack_[(4) - (3)].ndData), ee::EE_N_ASFLOAT11, (yyval.ndData) ); }
    break;

  case 329:

    { m_peecContainer->CreateAsFloat( (yysemantic_stack_[(4) - (3)].ndData), ee::EE_N_ASFLOAT10, (yyval.ndData) ); }
    break;

  case 330:

    { m_peecContainer->CreateAsFloatXProp( (yysemantic_stack_[(10) - (3)].ndData), (yysemantic_stack_[(10) - (5)].ndData), (yysemantic_stack_[(10) - (7)].ndData), (yysemantic_stack_[(10) - (9)].ndData), ee::EE_N_ASXFLOAT_MAX, (yyval.ndData) ); }
    break;

  case 331:

    { m_peecContainer->CreateAsFloatXProp( (yysemantic_stack_[(10) - (3)].ndData), (yysemantic_stack_[(10) - (5)].ndData), (yysemantic_stack_[(10) - (7)].ndData), (yysemantic_stack_[(10) - (9)].ndData), ee::EE_N_ASXFLOAT_MIN, (yyval.ndData) ); }
    break;

  case 332:

    { m_peecContainer->CreateAsFloatXProp( (yysemantic_stack_[(10) - (3)].ndData), (yysemantic_stack_[(10) - (5)].ndData), (yysemantic_stack_[(10) - (7)].ndData), (yysemantic_stack_[(10) - (9)].ndData), ee::EE_N_ASXFLOAT_TRUE_MIN, (yyval.ndData) ); }
    break;

  case 333:

    { m_peecContainer->CreateAsFloatXProp( (yysemantic_stack_[(10) - (3)].ndData), (yysemantic_stack_[(10) - (5)].ndData), (yysemantic_stack_[(10) - (7)].ndData), (yysemantic_stack_[(10) - (9)].ndData), ee::EE_N_ASXFLOAT_NAN, (yyval.ndData) ); }
    break;

  case 334:

    { m_peecContainer->CreateAsFloatXProp( (yysemantic_stack_[(10) - (3)].ndData), (yysemantic_stack_[(10) - (5)].ndData), (yysemantic_stack_[(10) - (7)].ndData), (yysemantic_stack_[(10) - (9)].ndData), ee::EE_N_ASXFLOAT_INF, (yyval.ndData) ); }
    break;

  case 335:

    { m_peecContainer->CreateAsFloatXProp( (yysemantic_stack_[(10) - (3)].ndData), (yysemantic_stack_[(10) - (5)].ndData), (yysemantic_stack_[(10) - (7)].ndData), (yysemantic_stack_[(10) - (9)].ndData), ee::EE_N_ASXFLOAT_SUBNORM_MAX, (yyval.ndData) ); }
    break;

  case 336:

    { m_peecContainer->CreateAsFloatXProp( (yysemantic_stack_[(10) - (3)].ndData), (yysemantic_stack_[(10) - (5)].ndData), (yysemantic_stack_[(10) - (7)].ndData), (yysemantic_stack_[(10) - (9)].ndData), ee::EE_N_ASXFLOAT_EPS, (yyval.ndData) ); }
    break;

  case 337:

    { m_peecContainer->CreateAsFloatXProp( (yysemantic_stack_[(12) - (3)].ndData), (yysemantic_stack_[(12) - (5)].ndData), (yysemantic_stack_[(12) - (7)].ndData), (yysemantic_stack_[(12) - (9)].ndData), (yysemantic_stack_[(12) - (11)].ndData), ee::EE_N_ASXFLOAT_SIGNBIT, (yyval.ndData) ); }
    break;

  case 338:

    { m_peecContainer->CreateAsFloatXProp( (yysemantic_stack_[(12) - (3)].ndData), (yysemantic_stack_[(12) - (5)].ndData), (yysemantic_stack_[(12) - (7)].ndData), (yysemantic_stack_[(12) - (9)].ndData), (yysemantic_stack_[(12) - (11)].ndData), ee::EE_N_ASXFLOAT_EXPBITS, (yyval.ndData) ); }
    break;

  case 339:

    { m_peecContainer->CreateAsFloatXProp( (yysemantic_stack_[(12) - (3)].ndData), (yysemantic_stack_[(12) - (5)].ndData), (yysemantic_stack_[(12) - (7)].ndData), (yysemantic_stack_[(12) - (9)].ndData), (yysemantic_stack_[(12) - (11)].ndData), ee::EE_N_ASXFLOAT_MANBITS, (yyval.ndData) ); }
    break;

  case 340:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 341:

    { m_peecContainer->SetTrans( (yysemantic_stack_[(1) - (1)].ndData) ); }
    break;



	default:
          break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (int yystate)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const short int parser::yypact_ninf_ = -423;
  const short int
  parser::yypact_[] =
  {
       264,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,  -187,  -173,  -167,  -166,  -165,  -164,  -163,
    -162,  -161,  -160,  -159,  -158,  -157,  -156,  -155,  -154,  -153,
    -152,  -151,  -150,  -149,  -148,  -147,  -146,  -145,  -144,  -143,
    -142,  -141,  -140,  -139,  -138,  -137,  -136,  -135,  -134,  -133,
    -132,  -131,  -130,  -129,  -128,  -127,  -126,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -125,  -124,  -123,  -122,  -121,  -120,  -119,
    -118,  -117,  -116,  -115,  -114,  -113,  -423,  -423,  -112,  -111,
    -110,  -423,  -423,   264,   264,  -423,  -423,  -423,  -423,    84,
    -423,   264,  -177,  -172,    10,   -10,    14,   -96,   -94,   -89,
     -87,  -178,  -423,  -423,  -423,   106,  -103,  -102,  -101,  -100,
     -99,   -98,   -97,   -95,   -93,   -92,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   -88,   -91,   599,  -423,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
     -86,   -85,   -84,   -83,   -82,   -81,   -80,   -79,   -78,   -77,
     -76,   -75,   -74,   -72,   -71,   -70,   -69,   -68,   -67,   -66,
     -65,   -64,   -63,   -62,   -61,   -60,   -59,   -58,   -57,   -55,
     -56,   -54,   -52,   -51,   -53,   -50,   -49,   -48,   -47,   -46,
     -43,   -42,   -45,   -41,  -194,   -44,   -33,   -32,   -31,   -30,
     -36,   -35,   -34,   -27,   -26,   -25,   -24,   -23,   -22,   -21,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -177,  -177,  -172,  -172,    10,    10,    10,    10,   -10,   -10,
      14,   -96,   -94,   -89,   -87,   -29,  -423,  -423,  -423,  -423,
    -423,  -423,   264,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,   264,  -423,
    -423,   264,  -423,  -423,  -423,  -423,   264,  -423,  -423,  -423,
     264,   264,   264,   264,   264,   264,   264,   264,  -423,   264,
    -423,   264,  -423,  -423,  -423,  -423,  -423,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   -12,   -11,
      -9,    -8,    -7,    -6,    55,    56,    57,    58,    70,    72,
     -20,   -17,    66,    67,    68,    69,   116,   240,   241,   242,
     243,   244,  -423,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
    -423,  -423,  -423,  -423,  -423,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   252,   246,   249,
     250,   251,   253,   254,   255,   257,   259,   262,   263,  -423,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   265,   271,   273,   274,   275,   276,   277,   278,   272,
     279,   280,   264,  -423,  -423,  -423,  -423,  -423,  -423,  -423,
     264,   264,   264,  -193,   281,   282,   283,  -423,   264,  -423,
    -423,  -423,   285,   264,   284,  -423
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned short int
  parser::yydefact_[] =
  {
         0,    12,    13,     2,     5,     6,     7,     8,     9,    11,
      10,     3,   124,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    21,    33,    50,
      23,    48,    34,    31,    43,    29,    28,    35,    39,    55,
      16,    46,    19,    52,    18,    24,    15,    25,    45,    20,
      36,    30,    53,    22,    27,    47,    26,    49,    14,    41,
      44,    54,    32,    42,    51,    40,    37,    38,    17,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   110,   111,     0,     0,
       0,   244,   245,     0,     0,   246,   247,     4,   126,   242,
     248,     0,   252,   255,   258,   263,   266,   268,   270,   272,
     274,   276,   340,   125,   341,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   243,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
     115,   119,   116,   120,   117,   121,   122,   123,   114,   118,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     112,   113,   128,   129,   127,   137,   149,   166,   139,   164,
     150,   147,   159,   145,   144,   151,   155,   171,   130,   162,
     135,   168,   134,   140,   132,   141,   161,   136,   152,   146,
     169,   138,   143,   163,   142,   165,   131,   157,   160,   170,
     148,   158,   167,   156,   153,   154,   133,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   227,   226,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   249,   250,   251,
     253,   254,   257,   256,   262,   261,   260,   259,   264,   265,
     267,   269,   271,   273,   275,     0,   278,   279,   280,   281,
     282,   283,     0,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,   295,   296,   297,   298,   299,     0,   301,
     302,     0,   304,   305,   306,   307,     0,   309,   310,   311,
       0,     0,     0,     0,     0,     0,     0,     0,   320,     0,
     322,     0,   323,   326,   327,   328,   329,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   277,   284,   300,   303,   308,   312,   313,   314,
     315,   316,   317,   318,   319,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   321,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   330,   331,   332,   333,   334,   335,   336,
       0,     0,     0,     0,     0,     0,     0,   325,     0,   337,
     338,   339,     0,     0,     0,   324
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  parser::yypgoto_[] =
  {
      -423,  -423,  -423,  -423,  -179,  -423,  -251,  -249,  -271,  -253,
    -109,  -180,  -174,  -168,  -181,  -423,  -422,  -423,   -13,  -423
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  parser::yydefgoto_[] =
  {
        -1,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char parser::yytable_ninf_ = -1;
  const unsigned short int
  parser::yytable_[] =
  {
       206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     287,   288,   279,   297,   550,   667,   494,   495,   496,   497,
     216,   551,   668,   280,   281,   282,   283,   284,   285,   286,
     291,   292,   490,   491,   217,   298,   492,   493,   498,   499,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,   278,   293,
     294,   487,   488,   489,   295,   296,   299,   300,   301,   302,
     303,   304,   305,   306,   501,   307,   504,   308,   309,   371,
     370,   502,   506,   507,   508,   509,   510,   511,   503,   513,
     514,   515,   516,   517,   518,   512,   519,   520,   521,   522,
     523,   524,   525,   526,   527,   592,   529,   530,     0,   532,
     533,   534,   528,   535,   537,   531,   538,   539,     0,   536,
       0,     0,   540,   548,   552,   541,   542,   543,   544,   545,
     276,   277,   546,   547,   549,   553,   554,   555,   556,   557,
     558,   559,     0,     0,   500,   567,   289,   290,   560,   561,
     562,   563,   564,   565,   566,   605,   593,   594,   606,   595,
     596,   597,   598,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   339,   340,   341,   342,   343,   344,   345,   346,
     347,   348,   349,   350,   351,   352,   353,   354,   355,   356,
     357,   358,   359,   360,   361,   362,   363,   364,   365,   366,
     367,   368,   369,   599,   600,   601,   602,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,   603,    12,
     604,   607,   608,   609,   610,   505,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,   611,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   612,   613,   614,   615,   616,
     629,   630,   181,   182,   631,   632,   633,     0,   634,   635,
     636,   183,   637,   184,   638,   185,   186,   639,   640,   653,
     652,   654,   655,   656,   657,   658,   659,   660,     0,   669,
     670,   671,   675,     0,   661,   662,     0,     0,     0,   568,
     673,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   569,     0,     0,   570,     0,
       0,     0,     0,   571,     0,     0,     0,   572,   573,   574,
     575,   576,   577,   578,   579,     0,   580,     0,   581,     0,
       0,     0,     0,     0,   582,   583,   584,   585,   586,   587,
     588,   589,   590,   591,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   617,   618,   619,   620,   621,   622,   623,   624,
     625,   626,   627,   628,   372,     0,     0,   373,     0,     0,
       0,     0,   374,     0,     0,     0,     0,   641,   642,   643,
     644,   645,   646,   647,   648,   649,   650,   651,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   663,
       0,     0,     0,     0,     0,     0,     0,   664,   665,   666,
       0,     0,     0,     0,     0,   672,     0,     0,     0,     0,
     674,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   375,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
     389,   390,   391,   392,   393,   394,   395,   396,   397,   398,
     399,   400,   401,   402,   403,   404,   405,   406,   407,   408,
     409,   410,   411,   412,   413,   414,   415,   416,   417,   418,
     419,   420,   421,   422,   423,   424,   425,   426,   427,   428,
     429,   430,   431,   432,   433,   434,   435,   436,   437,   438,
     439,   440,   441,   442,   443,   444,   445,   446,   447,   448,
     449,   450,   451,   452,   453,   454,   455,   456,   457,   458,
     459,   460,   461,   462,   463,   464,   465,   466,   467,   468,
     469,   470,   471,   472,   473,   474,   475,   476,   477,   478,
     479,   480,   481,   482,   483,     0,     0,   484,   485,   486
  };

  /* YYCHECK.  */
  const short int
  parser::yycheck_[] =
  {
        13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      20,    21,   191,   191,   208,   208,   287,   288,   289,   290,
     207,   215,   215,   200,   201,   202,   198,   199,    18,    19,
      16,    17,   283,   284,   207,   213,   285,   286,   291,   292,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,    14,   195,
     194,   280,   281,   282,   193,   192,     0,   210,   210,   210,
     210,   210,   210,   210,   294,   210,   297,   210,   210,   210,
     208,   295,   208,   208,   208,   208,   208,   208,   296,   208,
     208,   208,   208,   208,   208,   215,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   567,   208,   208,    -1,   208,
     208,   208,   215,   208,   208,   215,   208,   208,    -1,   215,
      -1,    -1,   215,   208,   208,   215,   215,   215,   215,   215,
     183,   184,   215,   215,   215,   208,   208,   208,   208,   215,
     215,   215,    -1,    -1,   293,   214,   196,   197,   215,   215,
     215,   215,   215,   215,   215,   215,   208,   208,   215,   208,
     208,   208,   208,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   208,   208,   208,   208,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,   208,    15,
     208,   215,   215,   215,   215,   298,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,   215,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   215,   215,   215,   215,   215,
     208,   215,   198,   199,   215,   215,   215,    -1,   215,   215,
     215,   207,   215,   209,   215,   211,   212,   215,   215,   208,
     215,   208,   208,   208,   208,   208,   208,   215,    -1,   208,
     208,   208,   208,    -1,   215,   215,    -1,    -1,    -1,   512,
     215,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   528,    -1,    -1,   531,    -1,
      -1,    -1,    -1,   536,    -1,    -1,    -1,   540,   541,   542,
     543,   544,   545,   546,   547,    -1,   549,    -1,   551,    -1,
      -1,    -1,    -1,    -1,   557,   558,   559,   560,   561,   562,
     563,   564,   565,   566,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   605,   606,   607,   608,   609,   610,   611,   612,
     613,   614,   615,   616,     5,    -1,    -1,     8,    -1,    -1,
      -1,    -1,    13,    -1,    -1,    -1,    -1,   630,   631,   632,
     633,   634,   635,   636,   637,   638,   639,   640,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   652,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   660,   661,   662,
      -1,    -1,    -1,    -1,    -1,   668,    -1,    -1,    -1,    -1,
     673,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,    -1,    -1,   188,   189,   190
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  parser::yystos_[] =
  {
         0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    15,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   198,   199,   207,   209,   211,   212,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   234,   234,    14,   220,
     200,   201,   202,   198,   199,    18,    19,    20,    21,   196,
     197,    16,    17,   195,   194,   193,   192,   191,   213,     0,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     208,   210,     5,     8,    13,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   188,   189,   190,   220,   220,   220,
     222,   222,   223,   223,   224,   224,   224,   224,   225,   225,
     226,   227,   228,   229,   230,   234,   208,   208,   208,   208,
     208,   208,   215,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   215,   208,
     208,   215,   208,   208,   208,   208,   215,   208,   208,   208,
     215,   215,   215,   215,   215,   215,   215,   215,   208,   215,
     208,   215,   208,   208,   208,   208,   208,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   214,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   232,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   208,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   215,   208,   208,   208,   208,   208,   208,   208,
     215,   215,   215,   234,   234,   234,   234,   208,   215,   208,
     208,   208,   234,   215,   234,   208
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   124,    94,    38,    62,    60,    43,    45,
      42,    47,    37,   448,   449,   450,   451,    40,    41,    91,
      93,   126,    33,    63,    58,    44
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  parser::yyr1_[] =
  {
         0,   216,   217,   217,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   220,   220,   221,   221,   221,   221,   222,   222,
     222,   222,   223,   223,   223,   224,   224,   224,   225,   225,
     225,   225,   225,   226,   226,   226,   227,   227,   228,   228,
     229,   229,   230,   230,   231,   231,   232,   232,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     234,   235
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  parser::yyr2_[] =
  {
         0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     2,     1,     1,     1,     1,     1,     3,
       3,     3,     1,     3,     3,     1,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     5,     4,     4,
       4,     4,     4,     4,     6,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       6,     4,     4,     6,     4,     4,     4,     4,     6,     4,
       4,     4,     6,     6,     6,     6,     6,     6,     6,     6,
       4,     8,     4,     4,    16,    12,     4,     4,     4,     4,
      10,    10,    10,    10,    10,    10,    10,    12,    12,    12,
       1,     1
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const parser::yytname_[] =
  {
    "$end", "error", "$undefined", "EE_CHAR_CONSTANT", "EE_FLOAT_CONSTANT",
  "EE_STRING_CONSTANT", "EE_HEX_CONSTANT1", "EE_HEX_CONSTANT2",
  "EE_HEX_CONSTANT3", "EE_BIN_CONSTANT", "EE_DEC_CONSTANT",
  "EE_OCT_CONSTANT", "EE_PUREDEC_CONSTANT", "EE_IDENTIFIER",
  "EE_MEMBERACCESS", "EE_USER_VAR", "EE_EQU_E", "EE_EQU_NE", "EE_LEFT_OP",
  "EE_RIGHT_OP", "EE_REL_GE", "EE_REL_LE", "EE_OB_BYTE", "EE_OB_SBYTE",
  "EE_OB_WORD", "EE_OB_SWORD", "EE_OB_QWORD", "EE_OB_SQWORD",
  "EE_OB_FLOAT", "EE_OB_DOUBLE", "EE_OB_DWORD", "EE_OB_SDWORD", "EE_COS",
  "EE_SIN", "EE_TAN", "EE_ACOS", "EE_ASIN", "EE_ATAN", "EE_ATAN2",
  "EE_COSH", "EE_SINH", "EE_TANH", "EE_ACOSH", "EE_ASINH", "EE_ATANH",
  "EE_EXP", "EE_LOG", "EE_LOG10", "EE_LOG2", "EE_EXP2", "EE_EXPM1",
  "EE_ILOGB", "EE_LOG1P", "EE_LOGB", "EE_POW", "EE_SQRT", "EE_CBRT",
  "EE_HYPOT", "EE_TGAMMA", "EE_LGAMMA", "EE_CEIL", "EE_FLOOR", "EE_MOD",
  "EE_TRUNC", "EE_ROUND", "EE_NEARBYINT", "EE_REMAINDER", "EE_REMQUO",
  "EE_NEXTAFTER", "EE_NEXTAFTERF", "EE_NEXTTOWARD", "EE_NEXTTOWARDF",
  "EE_DIM", "EE_MAX", "EE_MIN", "EE_ABS", "EE_MADD", "EE_A", "EE_ALLADI",
  "EE_ALPHA", "EE_B", "EE_B2", "EE_B4", "EE_BETA", "EE_BH", "EE_C2",
  "EE_CAHEN", "EE_CATALAN", "EE_CONWAY", "EE_DELTA", "EE_E", "EE_ERDOS",
  "EE_EULER", "EE_F", "EE_GR", "EE_GWK", "EE_HALFPI", "EE_HSMC", "EE_ICE",
  "EE_K", "EE_LAMBDA", "EE_LAPLACE", "EE_LEVY", "EE_M1", "EE_MU",
  "EE_NIVEN", "EE_OMEGA", "EE_P2", "EE_PI", "EE_PLASTIC", "EE_PORTER",
  "EE_PSI", "EE_RAMAN", "EE_RAMAMU", "EE_SIERP", "EE_THETA", "EE_VISW",
  "EE_Z3", "EE_ZETA", "EE_CHAR_BIT", "EE_MB_LEN_MAX", "EE_CHAR_MIN",
  "EE_CHAR_MAX", "EE_SCHAR_MIN", "EE_SHRT_MIN", "EE_INT_MIN",
  "EE_LONG_MIN", "EE_LLONG_MIN", "EE_SCHAR_MAX", "EE_SHRT_MAX",
  "EE_INT_MAX", "EE_LONG_MAX", "EE_LLONG_MAX", "EE_UCHAR_MAX",
  "EE_USHRT_MAX", "EE_UINT_MAX", "EE_ULONG_MAX", "EE_ULLONG_MAX",
  "EE_FLT_RADIX", "EE_DECIMAL_DIG", "EE_FLT_DECIMAL_DIG",
  "EE_DBL_DECIMAL_DIG", "EE_LDBL_DECIMAL_DIG", "EE_FLT_MIN", "EE_DBL_MIN",
  "EE_LDBL_MIN", "EE_FLT_TRUE_MIN", "EE_DBL_TRUE_MIN", "EE_LDBL_TRUE_MIN",
  "EE_FLT_MAX", "EE_DBL_MAX", "EE_LDBL_MAX", "EE_FLT_EPSILON",
  "EE_DBL_EPSILON", "EE_LDBL_EPSILON", "EE_FLT_DIG", "EE_DBL_DIG",
  "EE_LDBL_DIG", "EE_FLT_MANT_DIG", "EE_DBL_MANT_DIG", "EE_LDBL_MANT_DIG",
  "EE_FLT_MIN_EXP", "EE_DBL_MIN_EXP", "EE_LDBL_MIN_EXP",
  "EE_FLT_MIN_10_EXP", "EE_DBL_MIN_10_EXP", "EE_LDBL_MIN_10_EXP",
  "EE_FLT_MAX_EXP", "EE_DBL_MAX_EXP", "EE_LDBL_MAX_EXP",
  "EE_FLT_MAX_10_EXP", "EE_DBL_MAX_10_EXP", "EE_LDBL_MAX_10_EXP",
  "EE_AS_FLOAT", "EE_AS_DOUBLE", "EE_AS_FLOAT16", "EE_AS_FLOAT14",
  "EE_AS_FLOAT11", "EE_AS_FLOAT10", "EE_AS_FLOAT_MAX", "EE_AS_FLOAT_MIN",
  "EE_AS_FLOAT_TRUE_MIN", "EE_AS_FLOAT_NAN", "EE_AS_FLOAT_INF",
  "EE_AS_FLOAT_SUBNORM_MAX", "EE_AS_FLOAT_EPS", "EE_TRUE", "EE_FALSE",
  "EE_AS_FLOAT_SIGNBIT", "EE_AS_FLOAT_EXPBITS", "EE_AS_FLOAT_MANBITS",
  "EE_OR", "EE_AND", "'|'", "'^'", "'&'", "'>'", "'<'", "'+'", "'-'",
  "'*'", "'/'", "'%'", "EE_LOG_NOT", "EE_UNARY_1SCOMPL", "EE_UPLUS",
  "EE_UMINUS", "'('", "')'", "'['", "']'", "'~'", "'!'", "'?'", "':'",
  "','", "$accept", "string", "basic_expr", "postfix_exp", "unary_exp",
  "unary_operator", "multiplicative_exp", "additive_exp", "shift_exp",
  "relational_exp", "equality_exp", "logical_and_exp", "logical_xor_exp",
  "logical_or_exp", "and_exp", "or_exp", "conditional_exp", "intrinsic",
  "exp", "translation_unit", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const parser::rhs_number_type
  parser::yyrhs_[] =
  {
       235,     0,    -1,     5,    -1,    13,    -1,   217,    -1,     6,
      -1,     7,    -1,     8,    -1,     9,    -1,    10,    -1,    12,
      -1,    11,    -1,     3,    -1,     4,    -1,   108,    -1,    96,
      -1,    90,    -1,   118,    -1,    94,    -1,    92,    -1,    99,
      -1,    77,    -1,   103,    -1,    80,    -1,    95,    -1,    97,
      -1,   106,    -1,   104,    -1,    86,    -1,    85,    -1,   101,
      -1,    83,    -1,   112,    -1,    78,    -1,    82,    -1,    87,
      -1,   100,    -1,   116,    -1,   117,    -1,    88,    -1,   115,
      -1,   109,    -1,   113,    -1,    84,    -1,   110,    -1,    98,
      -1,    91,    -1,   105,    -1,    81,    -1,   107,    -1,    79,
      -1,   114,    -1,    93,    -1,   102,    -1,   111,    -1,    89,
      -1,   119,    -1,   120,    -1,   121,    -1,   122,    -1,   123,
      -1,   124,    -1,   125,    -1,   126,    -1,   127,    -1,   128,
      -1,   129,    -1,   130,    -1,   131,    -1,   132,    -1,   133,
      -1,   134,    -1,   135,    -1,   136,    -1,   137,    -1,   138,
      -1,   139,    -1,   140,    -1,   141,    -1,   142,    -1,   143,
      -1,   144,    -1,   145,    -1,   146,    -1,   147,    -1,   148,
      -1,   149,    -1,   150,    -1,   151,    -1,   152,    -1,   153,
      -1,   154,    -1,   155,    -1,   156,    -1,   157,    -1,   158,
      -1,   159,    -1,   160,    -1,   161,    -1,   162,    -1,   163,
      -1,   164,    -1,   165,    -1,   166,    -1,   167,    -1,   168,
      -1,   169,    -1,   170,    -1,   171,    -1,   172,    -1,   186,
      -1,   187,    -1,   207,   234,   208,    -1,   209,   234,   210,
      -1,    30,   234,   210,    -1,    22,   234,   210,    -1,    24,
     234,   210,    -1,    26,   234,   210,    -1,    31,   234,   210,
      -1,    23,   234,   210,    -1,    25,   234,   210,    -1,    27,
     234,   210,    -1,    28,   234,   210,    -1,    29,   234,   210,
      -1,    15,    -1,   233,    -1,   218,    -1,   219,    14,    13,
      -1,   219,    14,     5,    -1,   219,    14,     8,    -1,   219,
      14,    90,    -1,   219,    14,   108,    -1,   219,    14,    96,
      -1,   219,    14,   118,    -1,   219,    14,    94,    -1,   219,
      14,    92,    -1,   219,    14,    99,    -1,   219,    14,    77,
      -1,   219,    14,   103,    -1,   219,    14,    80,    -1,   219,
      14,    95,    -1,   219,    14,    97,    -1,   219,    14,   106,
      -1,   219,    14,   104,    -1,   219,    14,    86,    -1,   219,
      14,    85,    -1,   219,    14,   101,    -1,   219,    14,    83,
      -1,   219,    14,   112,    -1,   219,    14,    78,    -1,   219,
      14,    82,    -1,   219,    14,    87,    -1,   219,    14,   100,
      -1,   219,    14,   116,    -1,   219,    14,   117,    -1,   219,
      14,    88,    -1,   219,    14,   115,    -1,   219,    14,   109,
      -1,   219,    14,   113,    -1,   219,    14,    84,    -1,   219,
      14,   110,    -1,   219,    14,    98,    -1,   219,    14,    91,
      -1,   219,    14,   105,    -1,   219,    14,    81,    -1,   219,
      14,   107,    -1,   219,    14,    79,    -1,   219,    14,   114,
      -1,   219,    14,    93,    -1,   219,    14,   102,    -1,   219,
      14,   111,    -1,   219,    14,    89,    -1,   219,    14,   119,
      -1,   219,    14,   120,    -1,   219,    14,   121,    -1,   219,
      14,   122,    -1,   219,    14,   123,    -1,   219,    14,   124,
      -1,   219,    14,   125,    -1,   219,    14,   126,    -1,   219,
      14,   127,    -1,   219,    14,   128,    -1,   219,    14,   129,
      -1,   219,    14,   130,    -1,   219,    14,   131,    -1,   219,
      14,   132,    -1,   219,    14,   133,    -1,   219,    14,   134,
      -1,   219,    14,   135,    -1,   219,    14,   136,    -1,   219,
      14,   137,    -1,   219,    14,   138,    -1,   219,    14,   139,
      -1,   219,    14,   140,    -1,   219,    14,   141,    -1,   219,
      14,   142,    -1,   219,    14,   143,    -1,   219,    14,   144,
      -1,   219,    14,   145,    -1,   219,    14,   146,    -1,   219,
      14,   147,    -1,   219,    14,   148,    -1,   219,    14,   149,
      -1,   219,    14,   150,    -1,   219,    14,   151,    -1,   219,
      14,   152,    -1,   219,    14,   153,    -1,   219,    14,   154,
      -1,   219,    14,   155,    -1,   219,    14,   156,    -1,   219,
      14,   157,    -1,   219,    14,   158,    -1,   219,    14,   159,
      -1,   219,    14,   160,    -1,   219,    14,   161,    -1,   219,
      14,   162,    -1,   219,    14,   163,    -1,   219,    14,   164,
      -1,   219,    14,   165,    -1,   219,    14,   166,    -1,   219,
      14,   167,    -1,   219,    14,   168,    -1,   219,    14,   169,
      -1,   219,    14,   170,    -1,   219,    14,   171,    -1,   219,
      14,   172,    -1,   219,    14,   174,    -1,   219,    14,   173,
      -1,   219,    14,   175,    -1,   219,    14,   176,    -1,   219,
      14,   177,    -1,   219,    14,   178,    -1,   219,    14,   179,
      -1,   219,    14,   180,    -1,   219,    14,   181,    -1,   219,
      14,   182,    -1,   219,    14,   183,    -1,   219,    14,   184,
      -1,   219,    14,   185,    -1,   219,    14,   188,    -1,   219,
      14,   189,    -1,   219,    14,   190,    -1,   219,    -1,   221,
     220,    -1,   198,    -1,   199,    -1,   211,    -1,   212,    -1,
     220,    -1,   222,   200,   220,    -1,   222,   201,   220,    -1,
     222,   202,   220,    -1,   222,    -1,   223,   198,   222,    -1,
     223,   199,   222,    -1,   223,    -1,   224,    19,   223,    -1,
     224,    18,   223,    -1,   224,    -1,   225,   197,   224,    -1,
     225,   196,   224,    -1,   225,    21,   224,    -1,   225,    20,
     224,    -1,   225,    -1,   226,    16,   225,    -1,   226,    17,
     225,    -1,   226,    -1,   227,   195,   226,    -1,   227,    -1,
     228,   194,   227,    -1,   228,    -1,   229,   193,   228,    -1,
     229,    -1,   230,   192,   229,    -1,   230,    -1,   231,   191,
     230,    -1,   231,    -1,   231,   213,   234,   214,   232,    -1,
      32,   207,   234,   208,    -1,    33,   207,   234,   208,    -1,
      34,   207,   234,   208,    -1,    35,   207,   234,   208,    -1,
      36,   207,   234,   208,    -1,    37,   207,   234,   208,    -1,
      38,   207,   234,   215,   234,   208,    -1,    39,   207,   234,
     208,    -1,    40,   207,   234,   208,    -1,    41,   207,   234,
     208,    -1,    42,   207,   234,   208,    -1,    43,   207,   234,
     208,    -1,    44,   207,   234,   208,    -1,    45,   207,   234,
     208,    -1,    46,   207,   234,   208,    -1,    47,   207,   234,
     208,    -1,    48,   207,   234,   208,    -1,    49,   207,   234,
     208,    -1,    50,   207,   234,   208,    -1,    51,   207,   234,
     208,    -1,    52,   207,   234,   208,    -1,    53,   207,   234,
     208,    -1,    54,   207,   234,   215,   234,   208,    -1,    55,
     207,   234,   208,    -1,    56,   207,   234,   208,    -1,    57,
     207,   234,   215,   234,   208,    -1,    58,   207,   234,   208,
      -1,    59,   207,   234,   208,    -1,    60,   207,   234,   208,
      -1,    61,   207,   234,   208,    -1,    62,   207,   234,   215,
     234,   208,    -1,    63,   207,   234,   208,    -1,    64,   207,
     234,   208,    -1,    65,   207,   234,   208,    -1,    66,   207,
     234,   215,   234,   208,    -1,    68,   207,   234,   215,   234,
     208,    -1,    69,   207,   234,   215,   234,   208,    -1,    70,
     207,   234,   215,   234,   208,    -1,    71,   207,   234,   215,
     234,   208,    -1,    72,   207,   234,   215,   234,   208,    -1,
      73,   207,   234,   215,   234,   208,    -1,    74,   207,   234,
     215,   234,   208,    -1,    75,   207,   234,   208,    -1,    76,
     207,   234,   215,   234,   215,   234,   208,    -1,   173,   207,
     234,   208,    -1,   174,   207,   234,   208,    -1,   173,   207,
     234,   215,   234,   215,   234,   215,   234,   215,   234,   215,
     234,   215,   234,   208,    -1,   173,   207,   234,   215,   234,
     215,   234,   215,   234,   215,   234,   208,    -1,   175,   207,
     234,   208,    -1,   176,   207,   234,   208,    -1,   177,   207,
     234,   208,    -1,   178,   207,   234,   208,    -1,   179,   207,
     234,   215,   234,   215,   234,   215,   234,   208,    -1,   180,
     207,   234,   215,   234,   215,   234,   215,   234,   208,    -1,
     181,   207,   234,   215,   234,   215,   234,   215,   234,   208,
      -1,   182,   207,   234,   215,   234,   215,   234,   215,   234,
     208,    -1,   183,   207,   234,   215,   234,   215,   234,   215,
     234,   208,    -1,   184,   207,   234,   215,   234,   215,   234,
     215,   234,   208,    -1,   185,   207,   234,   215,   234,   215,
     234,   215,   234,   208,    -1,   188,   207,   234,   215,   234,
     215,   234,   215,   234,   215,   234,   208,    -1,   189,   207,
     234,   215,   234,   215,   234,   215,   234,   215,   234,   208,
      -1,   190,   207,   234,   215,   234,   215,   234,   215,   234,
     215,   234,   208,    -1,   232,    -1,   234,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  parser::yyprhs_[] =
  {
         0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    37,
      39,    41,    43,    45,    47,    49,    51,    53,    55,    57,
      59,    61,    63,    65,    67,    69,    71,    73,    75,    77,
      79,    81,    83,    85,    87,    89,    91,    93,    95,    97,
      99,   101,   103,   105,   107,   109,   111,   113,   115,   117,
     119,   121,   123,   125,   127,   129,   131,   133,   135,   137,
     139,   141,   143,   145,   147,   149,   151,   153,   155,   157,
     159,   161,   163,   165,   167,   169,   171,   173,   175,   177,
     179,   181,   183,   185,   187,   189,   191,   193,   195,   197,
     199,   201,   203,   205,   207,   209,   211,   213,   215,   217,
     219,   221,   223,   227,   231,   235,   239,   243,   247,   251,
     255,   259,   263,   267,   271,   273,   275,   277,   281,   285,
     289,   293,   297,   301,   305,   309,   313,   317,   321,   325,
     329,   333,   337,   341,   345,   349,   353,   357,   361,   365,
     369,   373,   377,   381,   385,   389,   393,   397,   401,   405,
     409,   413,   417,   421,   425,   429,   433,   437,   441,   445,
     449,   453,   457,   461,   465,   469,   473,   477,   481,   485,
     489,   493,   497,   501,   505,   509,   513,   517,   521,   525,
     529,   533,   537,   541,   545,   549,   553,   557,   561,   565,
     569,   573,   577,   581,   585,   589,   593,   597,   601,   605,
     609,   613,   617,   621,   625,   629,   633,   637,   641,   645,
     649,   653,   657,   661,   665,   669,   673,   677,   681,   685,
     689,   693,   697,   701,   705,   709,   713,   717,   721,   725,
     729,   733,   737,   739,   742,   744,   746,   748,   750,   752,
     756,   760,   764,   766,   770,   774,   776,   780,   784,   786,
     790,   794,   798,   802,   804,   808,   812,   814,   818,   820,
     824,   826,   830,   832,   836,   838,   842,   844,   850,   855,
     860,   865,   870,   875,   880,   887,   892,   897,   902,   907,
     912,   917,   922,   927,   932,   937,   942,   947,   952,   957,
     962,   969,   974,   979,   986,   991,   996,  1001,  1006,  1013,
    1018,  1023,  1028,  1035,  1042,  1049,  1056,  1063,  1070,  1077,
    1084,  1089,  1098,  1103,  1108,  1125,  1138,  1143,  1148,  1153,
    1158,  1169,  1180,  1191,  1202,  1213,  1224,  1235,  1248,  1261,
    1274,  1276
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  parser::yyrline_[] =
  {
         0,   108,   108,   109,   113,   114,   115,   116,   117,   118,
     126,   127,   128,   129,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   361,   362,   363,   364,   365,
     366,   367,   371,   372,   376,   377,   378,   379,   383,   384,
     385,   386,   390,   391,   392,   396,   397,   398,   402,   403,
     404,   405,   406,   410,   411,   412,   416,   417,   421,   422,
     426,   427,   431,   432,   436,   437,   441,   442,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   459,   460,   461,   462,   463,   464,   465,   466,   467,
     468,   469,   470,   471,   472,   473,   474,   475,   476,   477,
     478,   479,   480,   481,   482,   483,   484,   485,   486,   487,
     488,   489,   490,   491,   492,   494,   496,   497,   498,   499,
     500,   501,   502,   504,   505,   506,   508,   509,   511,   513,
     518,   523
  };

  // Print the state stack on the debug stream.
  void
  parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  parser::token_number_type
  parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   212,     2,     2,     2,   202,   195,     2,
     207,   208,   200,   198,   215,   199,     2,   201,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   214,     2,
     197,     2,   196,   213,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   209,     2,   210,   194,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   193,     2,   211,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   203,   204,
     205,   206
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int parser::yyeof_ = 0;
  const int parser::yylast_ = 789;
  const int parser::yynnts_ = 20;
  const int parser::yyempty_ = -2;
  const int parser::yyfinal_ = 299;
  const int parser::yyterror_ = 1;
  const int parser::yyerrcode_ = 256;
  const int parser::yyntokens_ = 216;

  const unsigned int parser::yyuser_token_number_max_ = 451;
  const parser::token_number_type parser::yyundef_token_ = 2;



} // yy





int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, ee::CExpEvalLexer * _peelLexer ) {
	return _peelLexer->yylex();
}

void yy::parser::error( const yy::location &/*_lLoc*/, const std::string &/*_strM*/ ) {

}

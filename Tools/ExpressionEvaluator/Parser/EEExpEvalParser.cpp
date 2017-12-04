
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

    {
																if ( m_peecContainer->TreatAllAsHex() ) {
																	m_peecContainer->CreateHex3( m_peelLexer->YYText(), (yyval.ndData) );
																}
																else {
																	m_peecContainer->CreateUInt( m_peelLexer->YYText(), (yyval.ndData) );
																}
															}
    break;

  case 9:

    { m_peecContainer->CreateUInt( m_peelLexer->YYText() + 1, (yyval.ndData) ); }
    break;

  case 10:

    { m_peecContainer->CreateOct( m_peelLexer->YYText(), (yyval.ndData) ); }
    break;

  case 11:

    { m_peecContainer->CreateChar( m_peelLexer->YYText(), (yyval.ndData) ); }
    break;

  case 12:

    {
																if ( m_peecContainer->TreatAllAsHex() && ValidHex( m_peelLexer->YYText() ) ) {
																	m_peecContainer->CreateHex3( m_peelLexer->YYText(), (yyval.ndData) );
																}
																else {
																	m_peecContainer->CreateDouble( m_peelLexer->YYText(), (yyval.ndData) );
																}
															}
    break;

  case 13:

    { m_peecContainer->CreateDouble( 3.1415926535897932384626433832795, (yyval.ndData) ); }
    break;

  case 14:

    { m_peecContainer->CreateDouble( 1.5707963267948966192313216916398, (yyval.ndData) ); }
    break;

  case 15:

    { m_peecContainer->CreateDouble( 2.7182818284590452353602874713527, (yyval.ndData) ); }
    break;

  case 16:

    { m_peecContainer->CreateDouble( 1.202056903159594285399738161511449990764986292, (yyval.ndData) ); }
    break;

  case 17:

    { m_peecContainer->CreateDouble( 1.6180339887498948482045868343656381177203091798057628621, (yyval.ndData) ); }
    break;

  case 18:

    { m_peecContainer->CreateDouble( 0.5772156649015328606065120900824024310421, (yyval.ndData) ); }
    break;

  case 19:

    { m_peecContainer->CreateDouble( 2.685452001065306445309714835481795693820382293994462953051152345557218, (yyval.ndData) ); }
    break;

  case 20:

    { m_peecContainer->CreateDouble( 1.2824271291006226368753425688697917277676889273250011920637400217404, (yyval.ndData) ); }
    break;

  case 21:

    { m_peecContainer->CreateDouble( 0.26149721284764278375542683860869585, (yyval.ndData) ); }
    break;

  case 22:

    { m_peecContainer->CreateDouble( 0.28016949902386913303, (yyval.ndData) ); }
    break;

  case 23:

    { m_peecContainer->CreateDouble( 0.30366300289873265859744812190155623, (yyval.ndData) ); }
    break;

  case 24:

    { m_peecContainer->CreateDouble( 0.35323637185499598454351655043268201, (yyval.ndData) ); }
    break;

  case 25:

    { m_peecContainer->CreateDouble( 0.56714329040978387299996866221035554, (yyval.ndData) ); }
    break;

  case 26:

    { m_peecContainer->CreateDouble( 0.62432998854355087099293638310083724, (yyval.ndData) ); }
    break;

  case 27:

    { m_peecContainer->CreateDouble( 0.6434105462, (yyval.ndData) ); }
    break;

  case 28:

    { m_peecContainer->CreateDouble( 0.66016181584686957392781211001455577, (yyval.ndData) ); }
    break;

  case 29:

    { m_peecContainer->CreateDouble( 0.66274341934918158097474209710925290, (yyval.ndData) ); }
    break;

  case 30:

    { m_peecContainer->CreateDouble( 0.70258, (yyval.ndData) ); }
    break;

  case 31:

    { m_peecContainer->CreateDouble( 0.76422365358922066299069873125009232, (yyval.ndData) ); }
    break;

  case 32:

    { m_peecContainer->CreateDouble( 0.8093940205, (yyval.ndData) ); }
    break;

  case 33:

    { m_peecContainer->CreateDouble( 0.87058838, (yyval.ndData) ); }
    break;

  case 34:

    { m_peecContainer->CreateDouble( 0.91596559417721901505460351493238411, (yyval.ndData) ); }
    break;

  case 35:

    { m_peecContainer->CreateDouble( 1.0986858055, (yyval.ndData) ); }
    break;

  case 36:

    { m_peecContainer->CreateDouble( 1.13198824, (yyval.ndData) ); }
    break;

  case 37:

    { m_peecContainer->CreateDouble( 1.20205690315959428539973816151144999, (yyval.ndData) ); }
    break;

  case 38:

    { m_peecContainer->CreateDouble( 1.30357726903429639125709911215255189, (yyval.ndData) ); }
    break;

  case 39:

    { m_peecContainer->CreateDouble( 1.30637788386308069046861449260260571, (yyval.ndData) ); }
    break;

  case 40:

    { m_peecContainer->CreateDouble( 1.32471795724474602596090885447809734, (yyval.ndData) ); }
    break;

  case 41:

    { m_peecContainer->CreateDouble( 1.45136923488338105028396848589202744, (yyval.ndData) ); }
    break;

  case 42:

    { m_peecContainer->CreateDouble( 1.45607494858268967139959535111654356, (yyval.ndData) ); }
    break;

  case 43:

    { m_peecContainer->CreateDouble( 1.4670780794, (yyval.ndData) ); }
    break;

  case 44:

    { m_peecContainer->CreateDouble( 1.5396007178, (yyval.ndData) ); }
    break;

  case 45:

    { m_peecContainer->CreateDouble( 1.60669515241529176378330152319092458, (yyval.ndData) ); }
    break;

  case 46:

    { m_peecContainer->CreateDouble( 1.70521114010536776428855145343450816, (yyval.ndData) ); }
    break;

  case 47:

    { m_peecContainer->CreateDouble( 1.902160583104, (yyval.ndData) ); }
    break;

  case 48:

    { m_peecContainer->CreateDouble( 2.29558714939263807403429804918949039, (yyval.ndData) ); }
    break;

  case 49:

    { m_peecContainer->CreateDouble( 2.50290787509589282228390287321821578, (yyval.ndData) ); }
    break;

  case 50:

    { m_peecContainer->CreateDouble( 2.58498175957925321706589358738317116, (yyval.ndData) ); }
    break;

  case 51:

    { m_peecContainer->CreateDouble( 2.80777024202851936522150118655777293, (yyval.ndData) ); }
    break;

  case 52:

    { m_peecContainer->CreateDouble( 3.27582291872181115978768188245384386, (yyval.ndData) ); }
    break;

  case 53:

    { m_peecContainer->CreateDouble( 3.35988566624317755317201130291892717, (yyval.ndData) ); }
    break;

  case 54:

    { m_peecContainer->CreateDouble( 4.66920160910299067185320382046620161, (yyval.ndData) ); }
    break;

  case 55:

    { (yyval.ndData) = (yysemantic_stack_[(3) - (2)].ndData); }
    break;

  case 56:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT32, (yyval.ndData) ); }
    break;

  case 57:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT32, (yyval.ndData) ); }
    break;

  case 58:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT8, (yyval.ndData) ); }
    break;

  case 59:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT16, (yyval.ndData) ); }
    break;

  case 60:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT64, (yyval.ndData) ); }
    break;

  case 61:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_FLOAT, (yyval.ndData) ); }
    break;

  case 62:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_DOUBLE, (yyval.ndData) ); }
    break;

  case 63:

    { m_peecContainer->CreateUser( (yyval.ndData) ); }
    break;

  case 64:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 65:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 66:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 67:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateString( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 68:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 69:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 70:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 71:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 72:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 73:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 74:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 75:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 76:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 77:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 78:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 79:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 80:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 81:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 82:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 83:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 84:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 85:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 86:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 87:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 88:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 89:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 90:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 91:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 92:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 93:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 94:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 95:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 96:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 97:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 98:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 99:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 100:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 101:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 102:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 103:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 104:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 105:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 106:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 107:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 108:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 109:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 110:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 111:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 112:

    { m_peecContainer->CreateUnary( (yysemantic_stack_[(2) - (2)].ndData), (yysemantic_stack_[(2) - (1)].ui32Unary), (yyval.ndData) ); }
    break;

  case 113:

    { (yyval.ui32Unary) = '+'; }
    break;

  case 114:

    { (yyval.ui32Unary) = '-'; }
    break;

  case 115:

    { (yyval.ui32Unary) = '~'; }
    break;

  case 116:

    { (yyval.ui32Unary) = '!'; }
    break;

  case 117:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 118:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '*', (yyval.ndData) ); }
    break;

  case 119:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '/', (yyval.ndData) ); }
    break;

  case 120:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '%', (yyval.ndData) ); }
    break;

  case 121:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 122:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '+', (yyval.ndData) ); }
    break;

  case 123:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '-', (yyval.ndData) ); }
    break;

  case 124:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 125:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_RIGHT_OP, (yyval.ndData) ); }
    break;

  case 126:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_LEFT_OP, (yyval.ndData) ); }
    break;

  case 127:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 128:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '<', (yyval.ndData) ); }
    break;

  case 129:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '>', (yyval.ndData) ); }
    break;

  case 130:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_REL_LE, (yyval.ndData) ); }
    break;

  case 131:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_REL_GE, (yyval.ndData) ); }
    break;

  case 132:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 133:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_EQU_E, (yyval.ndData) ); }
    break;

  case 134:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_EQU_NE, (yyval.ndData) ); }
    break;

  case 135:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 136:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '&', (yyval.ndData) ); }
    break;

  case 137:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 138:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '^', (yyval.ndData) ); }
    break;

  case 139:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 140:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '|', (yyval.ndData) ); }
    break;

  case 141:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 142:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_AND, (yyval.ndData) ); }
    break;

  case 143:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 144:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_OR, (yyval.ndData) ); }
    break;

  case 145:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 146:

    { m_peecContainer->CreateConditional( (yysemantic_stack_[(5) - (1)].ndData), (yysemantic_stack_[(5) - (3)].ndData), (yysemantic_stack_[(5) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 147:

    { m_peecContainer->CreateIntrinsic1( token::EE_COS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 148:

    { m_peecContainer->CreateIntrinsic1( token::EE_SIN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 149:

    { m_peecContainer->CreateIntrinsic1( token::EE_TAN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 150:

    { m_peecContainer->CreateIntrinsic1( token::EE_ACOS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 151:

    { m_peecContainer->CreateIntrinsic1( token::EE_ASIN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 152:

    { m_peecContainer->CreateIntrinsic1( token::EE_ATAN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 153:

    { m_peecContainer->CreateIntrinsic2( token::EE_ATAN2, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 154:

    { m_peecContainer->CreateIntrinsic1( token::EE_COSH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 155:

    { m_peecContainer->CreateIntrinsic1( token::EE_SINH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 156:

    { m_peecContainer->CreateIntrinsic1( token::EE_TANH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 157:

    { m_peecContainer->CreateIntrinsic1( token::EE_ACOSH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 158:

    { m_peecContainer->CreateIntrinsic1( token::EE_ASINH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 159:

    { m_peecContainer->CreateIntrinsic1( token::EE_ATANH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 160:

    { m_peecContainer->CreateIntrinsic1( token::EE_EXP, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 161:

    { m_peecContainer->CreateIntrinsic1( token::EE_LOG, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 162:

    { m_peecContainer->CreateIntrinsic1( token::EE_LOG10, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 163:

    { m_peecContainer->CreateIntrinsic1( token::EE_LOG2, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 164:

    { m_peecContainer->CreateIntrinsic1( token::EE_EXP2, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 165:

    { m_peecContainer->CreateIntrinsic1( token::EE_EXPM1, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 166:

    { m_peecContainer->CreateIntrinsic1( token::EE_ILOGB, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 167:

    { m_peecContainer->CreateIntrinsic1( token::EE_LOG1P, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 168:

    { m_peecContainer->CreateIntrinsic1( token::EE_LOGB, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 169:

    { m_peecContainer->CreateIntrinsic2( token::EE_POW, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 170:

    { m_peecContainer->CreateIntrinsic1( token::EE_SQRT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 171:

    { m_peecContainer->CreateIntrinsic1( token::EE_CBRT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 172:

    { m_peecContainer->CreateIntrinsic2( token::EE_HYPOT, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 173:

    { m_peecContainer->CreateIntrinsic1( token::EE_TGAMMA, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 174:

    { m_peecContainer->CreateIntrinsic1( token::EE_LGAMMA, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 175:

    { m_peecContainer->CreateIntrinsic1( token::EE_CEIL, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 176:

    { m_peecContainer->CreateIntrinsic1( token::EE_FLOOR, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 177:

    { m_peecContainer->CreateIntrinsic2( token::EE_MOD, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 178:

    { m_peecContainer->CreateIntrinsic1( token::EE_TRUNC, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 179:

    { m_peecContainer->CreateIntrinsic1( token::EE_ROUND, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 180:

    { m_peecContainer->CreateIntrinsic1( token::EE_NEARBYINT, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 181:

    { m_peecContainer->CreateIntrinsic2( token::EE_REMAINDER, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 182:

    { m_peecContainer->CreateIntrinsic2( token::EE_NEXTAFTER, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 183:

    { m_peecContainer->CreateIntrinsic2( token::EE_NEXTAFTERF, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 184:

    { m_peecContainer->CreateIntrinsic2( token::EE_NEXTTOWARD, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 185:

    { m_peecContainer->CreateIntrinsic2( token::EE_NEXTTOWARDF, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 186:

    { m_peecContainer->CreateIntrinsic2( token::EE_DIM, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 187:

    { m_peecContainer->CreateIntrinsic2( token::EE_MAX, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 188:

    { m_peecContainer->CreateIntrinsic2( token::EE_MIN, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 189:

    { m_peecContainer->CreateIntrinsic1( token::EE_ABS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 190:

    { m_peecContainer->CreateIntrinsic3( token::EE_MADD, (yysemantic_stack_[(8) - (3)].ndData), (yysemantic_stack_[(8) - (5)].ndData), (yysemantic_stack_[(8) - (7)].ndData), (yyval.ndData) ); }
    break;

  case 191:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 192:

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
  const short int parser::yypact_ninf_ = -262;
  const short int
  parser::yypact_[] =
  {
       169,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,   169,   169,   169,   169,   169,   169,  -100,   -98,
     -97,   -96,   -95,   -94,   -93,   -92,   -91,   -90,   -89,   -88,
     -87,   -86,   -85,   -84,   -83,   -82,   -81,   -80,   -79,   -78,
     -77,   -76,   -75,   -74,   -73,   -72,   -71,   -70,   -69,   -68,
     -67,   -66,   -65,   -64,   -63,   -62,   -61,   -60,   -59,   -58,
     -57,   -56,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,   169,   169,  -262,  -262,
    -262,  -262,    12,  -262,   169,  -109,  -104,     2,   -13,     6,
     -43,   -41,   -36,   -34,  -105,  -262,  -262,  -262,    82,   -50,
     -49,   -48,   -47,   -46,   -45,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   -42,
     -40,   307,  -262,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,  -262,  -262,  -262,  -262,  -262,  -262,  -262,   -39,
     -35,   -33,   -32,   -31,   -30,   -29,   -28,   -27,   -26,   -23,
     -21,   -20,   -19,   -18,   -17,   -16,   -15,   -14,   -11,   -10,
      -9,    29,    37,    38,    32,    40,    51,    53,    54,    48,
      56,    57,   100,   145,   146,   147,   148,   149,   150,   151,
     154,   162,   156,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -109,  -109,  -104,  -104,     2,     2,     2,
       2,   -13,   -13,     6,   -43,   -41,   -36,   -34,   158,  -262,
    -262,  -262,  -262,  -262,  -262,   169,  -262,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
    -262,   169,  -262,  -262,   169,  -262,  -262,  -262,  -262,   169,
    -262,  -262,  -262,   169,   169,   169,   169,   169,   169,   169,
     169,  -262,   169,   169,   165,   166,   167,   171,   174,   175,
     176,   177,   178,   179,   180,   183,   182,  -262,  -262,  -262,
    -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,  -262,
     169,   185,  -262
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  parser::yydefact_[] =
  {
         0,    11,    12,     2,     5,     6,     7,     8,    10,     9,
       3,    63,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    20,    32,    49,    22,    47,    33,    30,    42,
      28,    27,    34,    38,    54,    15,    45,    18,    51,    17,
      23,    14,    24,    44,    19,    35,    29,    52,    21,    26,
      46,    25,    48,    13,    40,    43,    53,    31,    41,    50,
      39,    36,    37,    16,   113,   114,     0,     0,   115,   116,
       4,    65,   111,   117,     0,   121,   124,   127,   132,   135,
     137,   139,   141,   143,   145,   191,    64,   192,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   112,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,    58,    59,    60,    61,    62,    57,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    55,    56,    67,    68,    66,    76,    88,
     105,    78,   103,    89,    86,    98,    84,    83,    90,    94,
     110,    69,   101,    74,   107,    73,    79,    71,    80,   100,
      75,    91,    85,   108,    77,    82,   102,    81,   104,    70,
      96,    99,   109,    87,    97,   106,    95,    92,    93,    72,
     118,   119,   120,   122,   123,   126,   125,   131,   130,   129,
     128,   133,   134,   136,   138,   140,   142,   144,     0,   147,
     148,   149,   150,   151,   152,     0,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,     0,   170,   171,     0,   173,   174,   175,   176,     0,
     178,   179,   180,     0,     0,     0,     0,     0,     0,     0,
       0,   189,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   146,   153,   169,
     172,   177,   181,   182,   183,   184,   185,   186,   187,   188,
       0,     0,   190
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  parser::yypgoto_[] =
  {
      -262,  -262,  -262,  -262,  -106,  -262,  -163,  -162,  -180,  -166,
     -99,  -107,   102,  -108,   117,  -262,  -261,  -262,   -12,  -262
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  parser::yydefgoto_[] =
  {
        -1,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char parser::yytable_ninf_ = -1;
  const unsigned short int
  parser::yytable_[] =
  {
       129,   130,   131,   132,   133,   134,   190,   191,   182,   200,
     307,   308,   309,   310,   183,   184,   185,   186,   187,   188,
     189,   194,   195,   303,   304,   181,   305,   306,   311,   312,
     135,   201,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   196,   197,   300,   301,   302,
     198,   199,   202,   203,   204,   205,   206,   207,   208,   253,
     314,   316,   319,   254,   179,   180,   320,   313,   321,   322,
     323,   324,   377,   326,   327,   328,   192,   193,   329,   325,
     330,   331,   332,   333,   334,   335,   336,   337,     0,     0,
     338,   339,   340,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   341,   342,   343,
     344,   345,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   346,    11,   347,   348,   349,   350,   351,   318,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,   352,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   353,   354,   355,   356,   357,   358,   359,
     104,   105,   360,   361,   362,   363,   378,   379,   380,   106,
     315,   107,   381,   108,   109,   382,   383,   384,   385,   386,
     387,   388,   255,   364,   389,   256,   392,   317,     0,   257,
     390,     0,     0,     0,     0,     0,     0,     0,     0,   365,
       0,     0,   366,     0,     0,     0,     0,   367,     0,     0,
       0,   368,   369,   370,   371,   372,   373,   374,   375,     0,
     376,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   391,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   296,   297,   298,
     299
  };

  /* YYCHECK.  */
  const short int
  parser::yycheck_[] =
  {
        12,    13,    14,    15,    16,    17,    19,    20,   114,   114,
     190,   191,   192,   193,   123,   124,   125,   121,   122,    17,
      18,    15,    16,   186,   187,    13,   188,   189,   194,   195,
     130,   136,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   118,   117,   183,   184,   185,
     116,   115,     0,   133,   133,   133,   133,   133,   133,   131,
     197,   199,   131,   133,   106,   107,   131,   196,   131,   131,
     131,   131,   363,   131,   131,   131,   119,   120,   131,   138,
     131,   131,   131,   131,   131,   131,   131,   131,    -1,    -1,
     131,   131,   131,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   138,   131,   131,
     138,   131,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,   131,    14,   131,   131,   138,   131,   131,   201,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,   131,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   138,   138,   138,   138,   138,   138,   138,
     121,   122,   138,   131,   138,   137,   131,   131,   131,   130,
     198,   132,   131,   134,   135,   131,   131,   131,   131,   131,
     131,   131,     5,   325,   131,     8,   131,   200,    -1,    12,
     138,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   341,
      -1,    -1,   344,    -1,    -1,    -1,    -1,   349,    -1,    -1,
      -1,   353,   354,   355,   356,   357,   358,   359,   360,    -1,
     362,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   390,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  parser::yystos_[] =
  {
         0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    14,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   121,   122,   130,   132,   134,   135,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   157,
     157,   157,   157,   157,   157,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   157,
     157,    13,   143,   123,   124,   125,   121,   122,    17,    18,
      19,    20,   119,   120,    15,    16,   118,   117,   116,   115,
     114,   136,     0,   133,   133,   133,   133,   133,   133,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   131,   133,     5,     8,    12,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     143,   143,   143,   145,   145,   146,   146,   147,   147,   147,
     147,   148,   148,   149,   150,   151,   152,   153,   157,   131,
     131,   131,   131,   131,   131,   138,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   138,   131,   131,   138,   131,   131,   131,   131,   138,
     131,   131,   131,   138,   138,   138,   138,   138,   138,   138,
     138,   131,   138,   137,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   155,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     138,   157,   131
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
     365,   366,   367,   368,   369,   370,   124,    94,    38,    62,
      60,    43,    45,    42,    47,    37,   371,   372,   373,   374,
      40,    41,    91,    93,   126,    33,    63,    58,    44
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  parser::yyr1_[] =
  {
         0,   139,   140,   140,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     142,   143,   143,   144,   144,   144,   144,   145,   145,   145,
     145,   146,   146,   146,   147,   147,   147,   148,   148,   148,
     148,   148,   149,   149,   149,   150,   150,   151,   151,   152,
     152,   153,   153,   154,   154,   155,   155,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   157,   158
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
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     2,     1,     1,     1,     1,     1,     3,     3,
       3,     1,     3,     3,     1,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     5,     4,     4,     4,
       4,     4,     4,     6,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     6,
       4,     4,     6,     4,     4,     4,     4,     6,     4,     4,
       4,     6,     6,     6,     6,     6,     6,     6,     6,     4,
       8,     1,     1
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const parser::yytname_[] =
  {
    "$end", "error", "$undefined", "EE_CHAR_CONSTANT", "EE_FLOAT_CONSTANT",
  "EE_STRING_CONSTANT", "EE_HEX_CONSTANT1", "EE_HEX_CONSTANT2",
  "EE_HEX_CONSTANT3", "EE_DEC_CONSTANT", "EE_OCT_CONSTANT",
  "EE_PUREDEC_CONSTANT", "EE_IDENTIFIER", "EE_MEMBERACCESS", "EE_USER_VAR",
  "EE_EQU_E", "EE_EQU_NE", "EE_LEFT_OP", "EE_RIGHT_OP", "EE_REL_GE",
  "EE_REL_LE", "EE_OB_BYTE", "EE_OB_WORD", "EE_OB_QWORD", "EE_OB_FLOAT",
  "EE_OB_DOUBLE", "EE_OB_DWORD", "EE_COS", "EE_SIN", "EE_TAN", "EE_ACOS",
  "EE_ASIN", "EE_ATAN", "EE_ATAN2", "EE_COSH", "EE_SINH", "EE_TANH",
  "EE_ACOSH", "EE_ASINH", "EE_ATANH", "EE_EXP", "EE_LOG", "EE_LOG10",
  "EE_LOG2", "EE_EXP2", "EE_EXPM1", "EE_ILOGB", "EE_LOG1P", "EE_LOGB",
  "EE_POW", "EE_SQRT", "EE_CBRT", "EE_HYPOT", "EE_TGAMMA", "EE_LGAMMA",
  "EE_CEIL", "EE_FLOOR", "EE_MOD", "EE_TRUNC", "EE_ROUND", "EE_NEARBYINT",
  "EE_REMAINDER", "EE_REMQUO", "EE_NEXTAFTER", "EE_NEXTAFTERF",
  "EE_NEXTTOWARD", "EE_NEXTTOWARDF", "EE_DIM", "EE_MAX", "EE_MIN",
  "EE_ABS", "EE_MADD", "EE_A", "EE_ALLADI", "EE_ALPHA", "EE_B", "EE_B2",
  "EE_B4", "EE_BETA", "EE_BH", "EE_C2", "EE_CAHEN", "EE_CATALAN",
  "EE_CONWAY", "EE_DELTA", "EE_E", "EE_ERDOS", "EE_EULER", "EE_F", "EE_GR",
  "EE_GWK", "EE_HALFPI", "EE_HSMC", "EE_ICE", "EE_K", "EE_LAMBDA",
  "EE_LAPLACE", "EE_LEVY", "EE_M1", "EE_MU", "EE_NIVEN", "EE_OMEGA",
  "EE_P2", "EE_PI", "EE_PLASTIC", "EE_PORTER", "EE_PSI", "EE_RAMAN",
  "EE_RAMAMU", "EE_SIERP", "EE_THETA", "EE_VISW", "EE_Z3", "EE_ZETA",
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
       158,     0,    -1,     5,    -1,    12,    -1,   140,    -1,     6,
      -1,     7,    -1,     8,    -1,     9,    -1,    11,    -1,    10,
      -1,     3,    -1,     4,    -1,   103,    -1,    91,    -1,    85,
      -1,   113,    -1,    89,    -1,    87,    -1,    94,    -1,    72,
      -1,    98,    -1,    75,    -1,    90,    -1,    92,    -1,   101,
      -1,    99,    -1,    81,    -1,    80,    -1,    96,    -1,    78,
      -1,   107,    -1,    73,    -1,    77,    -1,    82,    -1,    95,
      -1,   111,    -1,   112,    -1,    83,    -1,   110,    -1,   104,
      -1,   108,    -1,    79,    -1,   105,    -1,    93,    -1,    86,
      -1,   100,    -1,    76,    -1,   102,    -1,    74,    -1,   109,
      -1,    88,    -1,    97,    -1,   106,    -1,    84,    -1,   130,
     157,   131,    -1,   132,   157,   133,    -1,    26,   157,   133,
      -1,    21,   157,   133,    -1,    22,   157,   133,    -1,    23,
     157,   133,    -1,    24,   157,   133,    -1,    25,   157,   133,
      -1,    14,    -1,   156,    -1,   141,    -1,   142,    13,    12,
      -1,   142,    13,     5,    -1,   142,    13,     8,    -1,   142,
      13,    85,    -1,   142,    13,   103,    -1,   142,    13,    91,
      -1,   142,    13,   113,    -1,   142,    13,    89,    -1,   142,
      13,    87,    -1,   142,    13,    94,    -1,   142,    13,    72,
      -1,   142,    13,    98,    -1,   142,    13,    75,    -1,   142,
      13,    90,    -1,   142,    13,    92,    -1,   142,    13,   101,
      -1,   142,    13,    99,    -1,   142,    13,    81,    -1,   142,
      13,    80,    -1,   142,    13,    96,    -1,   142,    13,    78,
      -1,   142,    13,   107,    -1,   142,    13,    73,    -1,   142,
      13,    77,    -1,   142,    13,    82,    -1,   142,    13,    95,
      -1,   142,    13,   111,    -1,   142,    13,   112,    -1,   142,
      13,    83,    -1,   142,    13,   110,    -1,   142,    13,   104,
      -1,   142,    13,   108,    -1,   142,    13,    79,    -1,   142,
      13,   105,    -1,   142,    13,    93,    -1,   142,    13,    86,
      -1,   142,    13,   100,    -1,   142,    13,    76,    -1,   142,
      13,   102,    -1,   142,    13,    74,    -1,   142,    13,   109,
      -1,   142,    13,    88,    -1,   142,    13,    97,    -1,   142,
      13,   106,    -1,   142,    13,    84,    -1,   142,    -1,   144,
     143,    -1,   121,    -1,   122,    -1,   134,    -1,   135,    -1,
     143,    -1,   145,   123,   143,    -1,   145,   124,   143,    -1,
     145,   125,   143,    -1,   145,    -1,   146,   121,   145,    -1,
     146,   122,   145,    -1,   146,    -1,   147,    18,   146,    -1,
     147,    17,   146,    -1,   147,    -1,   148,   120,   147,    -1,
     148,   119,   147,    -1,   148,    20,   147,    -1,   148,    19,
     147,    -1,   148,    -1,   149,    15,   148,    -1,   149,    16,
     148,    -1,   149,    -1,   150,   118,   149,    -1,   150,    -1,
     151,   117,   150,    -1,   151,    -1,   152,   116,   151,    -1,
     152,    -1,   153,   115,   152,    -1,   153,    -1,   154,   114,
     153,    -1,   154,    -1,   154,   136,   157,   137,   155,    -1,
      27,   130,   157,   131,    -1,    28,   130,   157,   131,    -1,
      29,   130,   157,   131,    -1,    30,   130,   157,   131,    -1,
      31,   130,   157,   131,    -1,    32,   130,   157,   131,    -1,
      33,   130,   157,   138,   157,   131,    -1,    34,   130,   157,
     131,    -1,    35,   130,   157,   131,    -1,    36,   130,   157,
     131,    -1,    37,   130,   157,   131,    -1,    38,   130,   157,
     131,    -1,    39,   130,   157,   131,    -1,    40,   130,   157,
     131,    -1,    41,   130,   157,   131,    -1,    42,   130,   157,
     131,    -1,    43,   130,   157,   131,    -1,    44,   130,   157,
     131,    -1,    45,   130,   157,   131,    -1,    46,   130,   157,
     131,    -1,    47,   130,   157,   131,    -1,    48,   130,   157,
     131,    -1,    49,   130,   157,   138,   157,   131,    -1,    50,
     130,   157,   131,    -1,    51,   130,   157,   131,    -1,    52,
     130,   157,   138,   157,   131,    -1,    53,   130,   157,   131,
      -1,    54,   130,   157,   131,    -1,    55,   130,   157,   131,
      -1,    56,   130,   157,   131,    -1,    57,   130,   157,   138,
     157,   131,    -1,    58,   130,   157,   131,    -1,    59,   130,
     157,   131,    -1,    60,   130,   157,   131,    -1,    61,   130,
     157,   138,   157,   131,    -1,    63,   130,   157,   138,   157,
     131,    -1,    64,   130,   157,   138,   157,   131,    -1,    65,
     130,   157,   138,   157,   131,    -1,    66,   130,   157,   138,
     157,   131,    -1,    67,   130,   157,   138,   157,   131,    -1,
      68,   130,   157,   138,   157,   131,    -1,    69,   130,   157,
     138,   157,   131,    -1,    70,   130,   157,   131,    -1,    71,
     130,   157,   138,   157,   138,   157,   131,    -1,   155,    -1,
     157,    -1
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
      99,   101,   103,   105,   107,   109,   113,   117,   121,   125,
     129,   133,   137,   141,   143,   145,   147,   151,   155,   159,
     163,   167,   171,   175,   179,   183,   187,   191,   195,   199,
     203,   207,   211,   215,   219,   223,   227,   231,   235,   239,
     243,   247,   251,   255,   259,   263,   267,   271,   275,   279,
     283,   287,   291,   295,   299,   303,   307,   311,   315,   319,
     323,   327,   329,   332,   334,   336,   338,   340,   342,   346,
     350,   354,   356,   360,   364,   366,   370,   374,   376,   380,
     384,   388,   392,   394,   398,   402,   404,   408,   410,   414,
     416,   420,   422,   426,   428,   432,   434,   440,   445,   450,
     455,   460,   465,   470,   477,   482,   487,   492,   497,   502,
     507,   512,   517,   522,   527,   532,   537,   542,   547,   552,
     559,   564,   569,   576,   581,   586,   591,   596,   603,   608,
     613,   618,   625,   632,   639,   646,   653,   660,   667,   674,
     679,   688,   690
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  parser::yyrline_[] =
  {
         0,    98,    98,    99,   103,   104,   105,   106,   107,   115,
     116,   117,   118,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   230,   231,   235,   236,   237,   238,   242,   243,   244,
     245,   249,   250,   251,   255,   256,   257,   261,   262,   263,
     264,   265,   269,   270,   271,   275,   276,   280,   281,   285,
     286,   290,   291,   295,   296,   300,   301,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   341,   342,   343,   344,   345,   346,   347,
     348,   352,   357
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
       2,     2,     2,   135,     2,     2,     2,   125,   118,     2,
     130,   131,   123,   121,   138,   122,     2,   124,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   137,     2,
     120,     2,   119,   136,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   132,     2,   133,   117,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   116,     2,   134,     2,     2,     2,
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
     115,   126,   127,   128,   129
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int parser::yyeof_ = 0;
  const int parser::yylast_ = 420;
  const int parser::yynnts_ = 20;
  const int parser::yyempty_ = -2;
  const int parser::yyfinal_ = 202;
  const int parser::yyterror_ = 1;
  const int parser::yyerrcode_ = 256;
  const int parser::yyntokens_ = 139;

  const unsigned int parser::yyuser_token_number_max_ = 374;
  const parser::token_number_type parser::yyundef_token_ = 2;



} // yy





int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, ee::CExpEvalLexer * _peelLexer ) {
	return _peelLexer->yylex();
}

void yy::parser::error( const yy::location &/*_lLoc*/, const std::string &/*_strM*/ ) {

}

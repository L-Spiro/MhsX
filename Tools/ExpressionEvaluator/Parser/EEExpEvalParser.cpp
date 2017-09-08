
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

    { (yyval.ndData) = (yysemantic_stack_[(3) - (2)].ndData); }
    break;

  case 14:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT32, (yyval.ndData) ); }
    break;

  case 15:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT32, (yyval.ndData) ); }
    break;

  case 16:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT8, (yyval.ndData) ); }
    break;

  case 17:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT16, (yyval.ndData) ); }
    break;

  case 18:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_UINT64, (yyval.ndData) ); }
    break;

  case 19:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_FLOAT, (yyval.ndData) ); }
    break;

  case 20:

    { m_peecContainer->CreateAddress( (yysemantic_stack_[(3) - (2)].ndData), EE_CT_DOUBLE, (yyval.ndData) ); }
    break;

  case 21:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 22:

    { m_peecContainer->CreateMemberAccess( (yysemantic_stack_[(3) - (1)].ndData), m_peecContainer->CreateIdentifier( m_peelLexer->YYText() ), (yyval.ndData) ); }
    break;

  case 23:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 24:

    { m_peecContainer->CreateUnary( (yysemantic_stack_[(2) - (2)].ndData), (yysemantic_stack_[(2) - (1)].ui32Unary), (yyval.ndData) ); }
    break;

  case 25:

    { (yyval.ui32Unary) = '+'; }
    break;

  case 26:

    { (yyval.ui32Unary) = '-'; }
    break;

  case 27:

    { (yyval.ui32Unary) = '~'; }
    break;

  case 28:

    { (yyval.ui32Unary) = '!'; }
    break;

  case 29:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 30:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '*', (yyval.ndData) ); }
    break;

  case 31:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '/', (yyval.ndData) ); }
    break;

  case 32:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '%', (yyval.ndData) ); }
    break;

  case 33:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 34:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '+', (yyval.ndData) ); }
    break;

  case 35:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '-', (yyval.ndData) ); }
    break;

  case 36:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 37:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_RIGHT_OP, (yyval.ndData) ); }
    break;

  case 38:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_LEFT_OP, (yyval.ndData) ); }
    break;

  case 39:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 40:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '<', (yyval.ndData) ); }
    break;

  case 41:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '>', (yyval.ndData) ); }
    break;

  case 42:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_REL_LE, (yyval.ndData) ); }
    break;

  case 43:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_REL_GE, (yyval.ndData) ); }
    break;

  case 44:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 45:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_EQU_E, (yyval.ndData) ); }
    break;

  case 46:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_EQU_NE, (yyval.ndData) ); }
    break;

  case 47:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 48:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '&', (yyval.ndData) ); }
    break;

  case 49:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 50:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '^', (yyval.ndData) ); }
    break;

  case 51:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 52:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '|', (yyval.ndData) ); }
    break;

  case 53:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 54:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_AND, (yyval.ndData) ); }
    break;

  case 55:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 56:

    { m_peecContainer->CreateOp( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_OR, (yyval.ndData) ); }
    break;

  case 57:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 58:

    { m_peecContainer->CreateConditional( (yysemantic_stack_[(5) - (1)].ndData), (yysemantic_stack_[(5) - (3)].ndData), (yysemantic_stack_[(5) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 59:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 60:

    { m_peecContainer->CreateIntrinsic1( token::EE_COS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 61:

    { m_peecContainer->CreateIntrinsic1( token::EE_SIN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 62:

    { m_peecContainer->CreateIntrinsic1( token::EE_TAN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 63:

    { m_peecContainer->CreateIntrinsic1( token::EE_ACOS, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 64:

    { m_peecContainer->CreateIntrinsic1( token::EE_ASIN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 65:

    { m_peecContainer->CreateIntrinsic1( token::EE_ATAN, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 66:

    { m_peecContainer->CreateIntrinsic2( token::EE_ATAN2, (yysemantic_stack_[(6) - (3)].ndData), (yysemantic_stack_[(6) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 67:

    { m_peecContainer->CreateIntrinsic1( token::EE_COSH, (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 68:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 69:

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
  const signed char parser::yypact_ninf_ = -76;
  const signed char
  parser::yypact_[] =
  {
        -3,   -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,
     -76,    -3,    -3,    -3,    -3,    -3,    -3,   -75,   -31,   -29,
     -20,    -4,    -2,     0,     6,   -76,   -76,    -3,    -3,   -76,
     -76,   -76,   -76,    82,   -76,    28,    -8,    -1,    61,    -7,
      65,    27,    29,    31,    33,   -22,   -76,   -76,   -76,   106,
      21,    24,    26,    32,    34,    35,    -3,    -3,    -3,    -3,
      -3,    -3,    -3,    -3,    25,    36,    95,   -76,    28,    28,
      28,    28,    28,    28,    28,    28,    28,    28,    28,    28,
      28,    28,    28,    28,    28,    28,    -3,   -76,   -76,   -76,
     -76,   -76,   -76,   -76,    39,    40,    41,    42,    43,    44,
      15,    45,   -76,   -76,   -76,   -76,   -76,   -76,    -8,    -8,
      -1,    -1,    61,    61,    61,    61,    -7,    -7,    65,    27,
      29,    31,    33,    18,   -76,   -76,   -76,   -76,   -76,   -76,
      -3,   -76,    28,    46,   -76,   -76
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  parser::yydefact_[] =
  {
         0,    11,    12,     2,     5,     6,     7,     8,    10,     9,
       3,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    25,    26,     0,     0,    27,
      28,     4,    21,    23,    29,     0,    33,    36,    39,    44,
      47,    49,    51,    53,    55,    57,    59,    68,    69,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    24,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     1,    16,    17,
      18,    19,    20,    15,     0,     0,     0,     0,     0,     0,
       0,     0,    13,    14,    22,    30,    31,    32,    34,    35,
      38,    37,    43,    42,    41,    40,    45,    46,    48,    50,
      52,    54,    56,     0,    60,    61,    62,    63,    64,    65,
       0,    67,     0,     0,    58,    66
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  parser::yypgoto_[] =
  {
       -76,   -76,   -76,   -76,    20,   -76,    22,    23,   -62,    19,
      38,    52,    53,    37,    50,   -76,     5,     3,    30,   -76
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  parser::yydefgoto_[] =
  {
        -1,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char parser::yytable_ninf_ = -1;
  const unsigned char
  parser::yytable_[] =
  {
         1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      56,    75,    76,   112,   113,   114,   115,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    50,    51,    52,    53,    54,    55,    85,    11,    12,
      13,    14,    15,    16,    57,    67,    58,    64,    65,    94,
      95,    96,    97,    98,    99,    59,   101,    77,    78,    86,
      68,    69,    70,    25,    26,    71,    72,    73,    74,    79,
      80,    60,    27,    61,    28,    62,    29,    30,   105,   106,
     107,    63,   100,   108,   109,    66,   110,   111,   116,   117,
      81,    82,    83,    84,    25,    26,    87,   104,   130,    88,
     132,   102,    89,    27,    90,    28,   123,    29,    30,   118,
      91,   121,    92,    93,   103,   124,   125,   126,   127,   128,
     129,   131,   135,   133,   119,   122,   120,   134
  };

  /* YYCHECK.  */
  const unsigned char
  parser::yycheck_[] =
  {
         3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      85,    18,    19,    75,    76,    77,    78,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    11,    12,    13,    14,    15,    16,    69,    20,    21,
      22,    23,    24,    25,    85,    35,    85,    27,    28,    56,
      57,    58,    59,    60,    61,    85,    63,    74,    75,    91,
      78,    79,    80,    76,    77,    76,    77,    16,    17,    14,
      15,    85,    85,    85,    87,    85,    89,    90,    68,    69,
      70,    85,    62,    71,    72,    13,    73,    74,    79,    80,
      73,    72,    71,    70,    76,    77,     0,    12,    93,    88,
      92,    86,    88,    85,    88,    87,    86,    89,    90,    81,
      88,    84,    88,    88,    88,    86,    86,    86,    86,    86,
      86,    86,    86,   130,    82,    85,    83,   132
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  parser::yystos_[] =
  {
         0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    76,    77,    85,    87,    89,
      90,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     112,   112,   112,   112,   112,   112,    85,    85,    85,    85,
      85,    85,    85,    85,   112,   112,    13,    98,    78,    79,
      80,    76,    77,    16,    17,    18,    19,    74,    75,    14,
      15,    73,    72,    71,    70,    69,    91,     0,    88,    88,
      88,    88,    88,    88,   111,   111,   111,   111,   111,   111,
     112,   111,    86,    88,    12,    98,    98,    98,   100,   100,
     101,   101,   102,   102,   102,   102,   103,   103,   104,   105,
     106,   107,   108,   112,    86,    86,    86,    86,    86,    86,
      93,    86,    92,   111,   110,    86
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
     325,   124,    94,    38,    62,    60,    43,    45,    42,    47,
      37,   326,   327,   328,   329,    40,    41,    91,    93,   126,
      33,    63,    58,    44
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  parser::yyr1_[] =
  {
         0,    94,    95,    95,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    97,    97,    98,    98,    99,    99,    99,    99,   100,
     100,   100,   100,   101,   101,   101,   102,   102,   102,   103,
     103,   103,   103,   103,   104,   104,   104,   105,   105,   106,
     106,   107,   107,   108,   108,   109,   109,   110,   110,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   112,   113
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  parser::yyr2_[] =
  {
         0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     3,     1,     2,     1,     1,     1,     1,     1,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     1,
       3,     3,     3,     3,     1,     3,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     5,     1,
       4,     4,     4,     4,     4,     4,     6,     4,     1,     1
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
  "EE_PUREDEC_CONSTANT", "EE_IDENTIFIER", "EE_MEMBERACCESS", "EE_EQU_E",
  "EE_EQU_NE", "EE_LEFT_OP", "EE_RIGHT_OP", "EE_REL_GE", "EE_REL_LE",
  "EE_OB_BYTE", "EE_OB_WORD", "EE_OB_QWORD", "EE_OB_FLOAT", "EE_OB_DOUBLE",
  "EE_OB_DWORD", "EE_COS", "EE_SIN", "EE_TAN", "EE_ACOS", "EE_ASIN",
  "EE_ATAN", "EE_ATAN2", "EE_COSH", "EE_SINH", "EE_TANH", "EE_ACOSH",
  "EE_ASINH", "EE_ATANH", "EE_EXP", "EE_LOG", "EE_LOG10", "EE_LOG2",
  "EE_EXP2", "EE_EXPM1", "EE_ILOGB", "EE_LOG1P", "EE_LOGB", "EE_POW",
  "EE_SQRT", "EE_CBRT", "EE_HYPOT", "EE_TGAMMA", "EE_LGAMMA", "EE_CEIL",
  "EE_FLOOR", "EE_MOD", "EE_TRUNC", "EE_ROUND", "EE_NEARBYINT",
  "EE_REMAINDER", "EE_REMQUO", "EE_NEXTAFTER", "EE_NEXTTOWARD", "EE_DIM",
  "EE_MAX", "EE_MIN", "EE_ABS", "EE_MADD", "EE_OR", "EE_AND", "'|'", "'^'",
  "'&'", "'>'", "'<'", "'+'", "'-'", "'*'", "'/'", "'%'", "EE_LOG_NOT",
  "EE_UNARY_1SCOMPL", "EE_UPLUS", "EE_UMINUS", "'('", "')'", "'['", "']'",
  "'~'", "'!'", "'?'", "':'", "','", "$accept", "string", "basic_expr",
  "postfix_exp", "unary_exp", "unary_operator", "multiplicative_exp",
  "additive_exp", "shift_exp", "relational_exp", "equality_exp",
  "logical_and_exp", "logical_xor_exp", "logical_or_exp", "and_exp",
  "or_exp", "conditional_exp", "intrinsic", "exp", "translation_unit", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const parser::rhs_number_type
  parser::yyrhs_[] =
  {
       113,     0,    -1,     5,    -1,    12,    -1,    95,    -1,     6,
      -1,     7,    -1,     8,    -1,     9,    -1,    11,    -1,    10,
      -1,     3,    -1,     4,    -1,    85,   112,    86,    -1,    87,
     112,    88,    -1,    25,   112,    88,    -1,    20,   112,    88,
      -1,    21,   112,    88,    -1,    22,   112,    88,    -1,    23,
     112,    88,    -1,    24,   112,    88,    -1,    96,    -1,    97,
      13,    12,    -1,    97,    -1,    99,    98,    -1,    76,    -1,
      77,    -1,    89,    -1,    90,    -1,    98,    -1,   100,    78,
      98,    -1,   100,    79,    98,    -1,   100,    80,    98,    -1,
     100,    -1,   101,    76,   100,    -1,   101,    77,   100,    -1,
     101,    -1,   102,    17,   101,    -1,   102,    16,   101,    -1,
     102,    -1,   103,    75,   102,    -1,   103,    74,   102,    -1,
     103,    19,   102,    -1,   103,    18,   102,    -1,   103,    -1,
     104,    14,   103,    -1,   104,    15,   103,    -1,   104,    -1,
     105,    73,   104,    -1,   105,    -1,   106,    72,   105,    -1,
     106,    -1,   107,    71,   106,    -1,   107,    -1,   108,    70,
     107,    -1,   108,    -1,   109,    69,   108,    -1,   109,    -1,
     109,    91,   112,    92,   110,    -1,   110,    -1,    26,    85,
     111,    86,    -1,    27,    85,   111,    86,    -1,    28,    85,
     111,    86,    -1,    29,    85,   111,    86,    -1,    30,    85,
     111,    86,    -1,    31,    85,   111,    86,    -1,    32,    85,
     112,    93,   111,    86,    -1,    33,    85,   111,    86,    -1,
     111,    -1,   112,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  parser::yyprhs_[] =
  {
         0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    29,    33,    37,    41,    45,    49,
      53,    57,    59,    63,    65,    68,    70,    72,    74,    76,
      78,    82,    86,    90,    92,    96,   100,   102,   106,   110,
     112,   116,   120,   124,   128,   130,   134,   138,   140,   144,
     146,   150,   152,   156,   158,   162,   164,   168,   170,   176,
     178,   183,   188,   193,   198,   203,   208,   215,   220,   222
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  parser::yyrline_[] =
  {
         0,    96,    96,    97,   101,   102,   103,   104,   105,   113,
     114,   115,   116,   124,   125,   126,   127,   128,   129,   130,
     131,   135,   136,   140,   141,   145,   146,   147,   148,   152,
     153,   154,   155,   159,   160,   161,   165,   166,   167,   171,
     172,   173,   174,   175,   179,   180,   181,   185,   186,   190,
     191,   195,   196,   200,   201,   205,   206,   210,   211,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   227,   232
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
       2,     2,     2,    90,     2,     2,     2,    80,    73,     2,
      85,    86,    78,    76,    93,    77,     2,    79,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    92,     2,
      75,     2,    74,    91,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    87,     2,    88,    72,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    71,     2,    89,     2,     2,     2,
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
      65,    66,    67,    68,    69,    70,    81,    82,    83,    84
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int parser::yyeof_ = 0;
  const int parser::yylast_ = 137;
  const int parser::yynnts_ = 20;
  const int parser::yyempty_ = -2;
  const int parser::yyfinal_ = 87;
  const int parser::yyterror_ = 1;
  const int parser::yyerrcode_ = 256;
  const int parser::yyntokens_ = 94;

  const unsigned int parser::yyuser_token_number_max_ = 329;
  const parser::token_number_type parser::yyundef_token_ = 2;



} // yy





int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, ee::CExpEvalLexer * _peelLexer ) {
	return _peelLexer->yylex();
}

void yy::parser::error( const yy::location &/*_lLoc*/, const std::string &/*_strM*/ ) {

}

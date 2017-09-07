
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




#include "../MXMhsX.h"
#include "MXExpEvalContainer.h"
#include "MXExpEvalLexer.h"
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
extern int yylex( /*YYSTYPE*/void * _pvNodeUnion, mx::CExpEvalLexer * _pslLexer );


#include "MXExpEvalSyntaxNodes.h"






#include "MXExpEvalParser.hpp"

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
  parser::parser (class CExpEvalLexer * m_pslLexer_yyarg, class CExpEvalContainer * m_pslpContainer_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      m_pslLexer (m_pslLexer_yyarg),
      m_pslpContainer (m_pslpContainer_yyarg)
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
	yychar = yylex (&yylval, m_pslLexer);
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

    { pcContainer->MakeIdentifierNode( &(yyval), plLexer->YYText() ); }
    break;

  case 3:

    { (yyval).iType = uint64_t; (yyval).u.ui64Val = MH_CodeString::HexStringToUINT64( &plLexer->YYText()[2] ); }
    break;

  case 4:

    {
																if ( pcContainer->UseLiteralIdentifiers() && !(plLexer->YYText()[0] >= '0' && plLexer->YYText()[0] <= '9') ) {
																	pcContainer->MakeIdentifierNode( &(yyval), plLexer->YYText() );
																}
																else {
																	(yyval).iType = uint64_t;
																	(yyval).u.ui64Val = MH_CodeString::HexStringToUINT64( plLexer->YYText() );
																}
															}
    break;

  case 5:

    {
																if ( pcContainer->UseLiteralIdentifiers() && !(plLexer->YYText()[0] >= '0' && plLexer->YYText()[0] <= '9') ) {
																	pcContainer->MakeIdentifierNode( &(yyval), plLexer->YYText() );
																}
																else {
																	(yyval).iType = uint64_t;
																	(yyval).u.ui64Val = MH_CodeString::HexStringToUINT64( plLexer->YYText() );
																}
															}
    break;

  case 6:

    { (yyval).iType = uint64_t; (yyval).u.ui64Val = pcContainer->TreatAllAsHex() ? MH_CodeString::HexStringToUINT64( plLexer->YYText() ) : _atoi64( plLexer->YYText() ); }
    break;

  case 7:

    { (yyval).iType = uint64_t; (yyval).u.ui64Val = _atoi64( &plLexer->YYText()[1] ); }
    break;

  case 8:

    { (yyval).iType = uint64_t; (yyval).u.ui64Val = pcContainer->TreatAllAsHex() ? MH_CodeString::HexStringToUINT64( plLexer->YYText() ) : MH_CodeString::OctStringToUINT64( plLexer->YYText() ); }
    break;

  case 9:

    { pcContainer->MakeCharNode( &(yyval), plLexer->YYText() ); }
    break;

  case 10:

    { pcContainer->MakeStringNode( &(yyval), plLexer->YYText() ); }
    break;

  case 11:

    { if ( pcContainer->TreatAllAsHex() && MH_CodeString::ValidHex( plLexer->YYText() ) ) { (yyval).iType = uint64_t; (yyval).u.ui64Val = MH_CodeString::HexStringToUINT64( plLexer->YYText() ); }
																else { (yyval).iType = MT_DOUBLE; (yyval).u.dVal = atof( plLexer->YYText() ); }
															}
    break;

  case 12:

    { CopyMemory( &(yyval), &(yysemantic_stack_[(3) - (2)]), sizeof( (yyval) ) ); }
    break;

  case 13:

    { pcContainer->MakeAddressNode( &(yyval), MT_ULONG, &(yysemantic_stack_[(3) - (2)]) ); }
    break;

  case 14:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 15:

    { pcContainer->MakePostFixArrayNode( &(yyval), &(yysemantic_stack_[(4) - (1)]), &(yysemantic_stack_[(4) - (3)]) ); }
    break;

  case 16:

    { pcContainer->MakePostFixMemberAccessNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), plLexer->YYText() ); }
    break;

  case 17:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 18:

    { pcContainer->MakeUnaryNode( &(yyval), (yysemantic_stack_[(2) - (1)]), &(yysemantic_stack_[(2) - (2)]) ); }
    break;

  case 19:

    { (yyval) = '+'; }
    break;

  case 20:

    { (yyval) = '-'; }
    break;

  case 21:

    { (yyval) = '~'; }
    break;

  case 22:

    { (yyval) = '!'; }
    break;

  case 23:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 24:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), '*' ); }
    break;

  case 25:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), '/' ); }
    break;

  case 26:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), '%' ); }
    break;

  case 27:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 28:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), '+' ); }
    break;

  case 29:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), '-' ); }
    break;

  case 30:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 31:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), MX_RIGHT_OP ); }
    break;

  case 32:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), MX_LEFT_OP ); }
    break;

  case 33:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 34:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), '<' ); }
    break;

  case 35:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), '>' ); }
    break;

  case 36:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), MX_REL_LE ); }
    break;

  case 37:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), MX_REL_GE ); }
    break;

  case 38:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 39:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), MX_EQU_E ); }
    break;

  case 40:

    { pcContainer->MakeOpNode( &(yyval), &(yysemantic_stack_[(3) - (1)]), &(yysemantic_stack_[(3) - (3)]), MX_EQU_NE ); }
    break;

  case 41:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 42:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)]), (yysemantic_stack_[(3) - (3)]), '&', (yyval) ); }
    break;

  case 43:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 44:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)]), (yysemantic_stack_[(3) - (3)]), '^', (yyval) ); }
    break;

  case 45:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 46:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)]), (yysemantic_stack_[(3) - (3)]), '|', (yyval) ); }
    break;

  case 47:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 48:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)]), (yysemantic_stack_[(3) - (3)]), token::MX_AND, (yyval) ); }
    break;

  case 49:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 50:

    { m_pslpContainer->CreateMath( (yysemantic_stack_[(3) - (1)]), (yysemantic_stack_[(3) - (3)]), token::MX_OR, (yyval) ); }
    break;

  case 51:

    { (yyval) = (yysemantic_stack_[(1) - (1)]); }
    break;

  case 52:

    { pcContainer->MakeConditionalExp( &(yyval), &(yysemantic_stack_[(5) - (1)]), &(yysemantic_stack_[(5) - (3)]), &(yysemantic_stack_[(5) - (5)]) ); }
    break;

  case 54:

    { pcContainer->SetTrans( &(yysemantic_stack_[(1) - (1)]) ); }
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
  const signed char parser::yypact_ninf_ = -33;
  const signed char
  parser::yypact_[] =
  {
        -1,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,   -33,
     -33,   -33,   -33,    -1,    -1,   -33,   -33,   -33,     0,   -33,
      -1,   -11,     1,    14,    27,    18,    -3,    13,     3,    28,
      -8,   -33,   -33,    23,    19,    20,    45,    -1,   -33,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   -33,   -33,
     -33,   -33,    21,   -33,   -33,   -33,   -11,   -11,     1,     1,
      14,    14,    14,    14,    27,    27,    18,    -3,    13,     3,
      28,    15,   -33,    -1,   -33
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  parser::yydefact_[] =
  {
         0,     9,    11,    10,     3,     4,     5,     6,     8,     7,
       2,    19,    20,     0,     0,    21,    22,    14,    17,    23,
       0,    27,    30,    33,    38,    41,    43,    45,    47,    49,
      51,    53,    54,     0,     0,     0,     0,     0,    18,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     1,    12,
      13,    16,     0,    24,    25,    26,    28,    29,    32,    31,
      37,    36,    35,    34,    39,    40,    42,    44,    46,    48,
      50,     0,    15,     0,    52
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  parser::yypgoto_[] =
  {
       -33,   -33,   -33,     2,   -33,     5,     6,   -32,     4,     9,
      10,     8,    11,    12,   -33,   -19,   -13,   -33
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  parser::yydefgoto_[] =
  {
        -1,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char parser::yytable_ninf_ = -1;
  const unsigned char
  parser::yytable_[] =
  {
        34,    35,     1,     2,     3,     4,     5,     6,     7,     8,
       9,    10,    56,    36,    70,    71,    72,    73,    39,    40,
      41,    52,    38,    58,    62,    54,    11,    12,    42,    43,
      44,    45,    50,    51,    57,    13,    53,    14,    37,    15,
      16,    63,    64,    65,    81,    46,    47,    66,    67,    55,
      68,    69,    48,    49,    74,    75,    59,    61,    83,    60,
      82,    76,    78,    77,    84,     0,    79,     0,    80
  };

  /* YYCHECK.  */
  const signed char
  parser::yycheck_[] =
  {
        13,    14,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    20,    13,    46,    47,    48,    49,    29,    30,
      31,    24,    20,     0,    37,    22,    27,    28,    27,    28,
      16,    17,    14,    15,    42,    36,    23,    38,    38,    40,
      41,    39,    40,    41,    57,    18,    19,    42,    43,    21,
      44,    45,    25,    26,    50,    51,    37,    12,    43,    39,
      39,    52,    54,    53,    83,    -1,    55,    -1,    56
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  parser::yystos_[] =
  {
         0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    27,    28,    36,    38,    40,    41,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    60,    60,    13,    38,    47,    29,
      30,    31,    27,    28,    16,    17,    18,    19,    25,    26,
      14,    15,    24,    23,    22,    21,    20,    42,     0,    37,
      39,    12,    60,    47,    47,    47,    49,    49,    50,    50,
      51,    51,    51,    51,    52,    52,    53,    54,    55,    56,
      57,    60,    39,    43,    59
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   124,    94,    38,    62,    60,    43,    45,    42,
      47,    37,   277,   278,   279,   280,    40,    41,    91,    93,
     126,    33,    63,    58
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  parser::yyr1_[] =
  {
         0,    44,    45,    45,    45,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    46,    46,    46,    47,    47,    48,
      48,    48,    48,    49,    49,    49,    49,    50,    50,    50,
      51,    51,    51,    52,    52,    52,    52,    52,    53,    53,
      53,    54,    54,    55,    55,    56,    56,    57,    57,    58,
      58,    59,    59,    60,    61
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  parser::yyr2_[] =
  {
         0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     1,     4,     3,     1,     2,     1,
       1,     1,     1,     1,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     3,     1,     3,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     5,     1,     1
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const parser::yytname_[] =
  {
    "$end", "error", "$undefined", "MX_CHAR_CONSTANT", "MX_FLOAT_CONSTANT",
  "MX_STRING_CONSTANT", "MX_HEX_CONSTANT1", "MX_HEX_CONSTANT2",
  "MX_HEX_CONSTANT3", "MX_DEC_CONSTANT", "MX_OCT_CONSTANT",
  "MX_PUREDEC_CONSTANT", "MX_IDENTIFIER", "MX_MEMBERACCESS", "MX_EQU_E",
  "MX_EQU_NE", "MX_LEFT_OP", "MX_RIGHT_OP", "MX_REL_GE", "MX_REL_LE",
  "MX_OR", "MX_AND", "'|'", "'^'", "'&'", "'>'", "'<'", "'+'", "'-'",
  "'*'", "'/'", "'%'", "MX_LOG_NOT", "MX_UNARY_NOT", "MX_UPLUS",
  "MX_UMINUS", "'('", "')'", "'['", "']'", "'~'", "'!'", "'?'", "':'",
  "$accept", "basic_expr", "postfix_exp", "unary_exp", "unary_operator",
  "multiplicative_exp", "additive_exp", "shift_exp", "relational_exp",
  "equality_exp", "logical_and_exp", "logical_xor_exp", "logical_or_exp",
  "and_exp", "or_exp", "conditional_exp", "exp", "translation_unit", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const parser::rhs_number_type
  parser::yyrhs_[] =
  {
        61,     0,    -1,    12,    -1,     6,    -1,     7,    -1,     8,
      -1,     9,    -1,    11,    -1,    10,    -1,     3,    -1,     5,
      -1,     4,    -1,    36,    60,    37,    -1,    38,    60,    39,
      -1,    45,    -1,    46,    38,    60,    39,    -1,    46,    13,
      12,    -1,    46,    -1,    48,    47,    -1,    27,    -1,    28,
      -1,    40,    -1,    41,    -1,    47,    -1,    49,    29,    47,
      -1,    49,    30,    47,    -1,    49,    31,    47,    -1,    49,
      -1,    50,    27,    49,    -1,    50,    28,    49,    -1,    50,
      -1,    51,    17,    50,    -1,    51,    16,    50,    -1,    51,
      -1,    52,    26,    51,    -1,    52,    25,    51,    -1,    52,
      19,    51,    -1,    52,    18,    51,    -1,    52,    -1,    53,
      14,    52,    -1,    53,    15,    52,    -1,    53,    -1,    54,
      24,    53,    -1,    54,    -1,    55,    23,    54,    -1,    55,
      -1,    56,    22,    55,    -1,    56,    -1,    57,    21,    56,
      -1,    57,    -1,    58,    20,    57,    -1,    58,    -1,    58,
      42,    60,    43,    59,    -1,    59,    -1,    60,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  parser::yyprhs_[] =
  {
         0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    27,    31,    33,    38,    42,    44,    47,
      49,    51,    53,    55,    57,    61,    65,    69,    71,    75,
      79,    81,    85,    89,    91,    95,    99,   103,   107,   109,
     113,   117,   119,   123,   125,   129,   131,   135,   137,   141,
     143,   147,   149,   155,   157
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  parser::yyrline_[] =
  {
         0,    67,    72,    73,    74,    83,    92,    93,    94,    95,
      96,    97,   100,   101,   105,   106,   107,   111,   112,   116,
     117,   118,   119,   124,   125,   126,   127,   131,   132,   133,
     137,   138,   139,   143,   144,   145,   146,   147,   151,   152,
     153,   157,   158,   162,   163,   167,   168,   172,   173,   177,
     178,   182,   183,   188,   193
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
       2,     2,     2,    41,     2,     2,     2,    31,    24,     2,
      36,    37,    29,    27,     2,    28,     2,    30,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    43,     2,
      26,     2,    25,    42,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    38,     2,    39,    23,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    22,     2,    40,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    32,    33,    34,
      35
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int parser::yyeof_ = 0;
  const int parser::yylast_ = 68;
  const int parser::yynnts_ = 18;
  const int parser::yyempty_ = -2;
  const int parser::yyfinal_ = 58;
  const int parser::yyterror_ = 1;
  const int parser::yyerrcode_ = 256;
  const int parser::yyntokens_ = 44;

  const unsigned int parser::yyuser_token_number_max_ = 280;
  const parser::token_number_type parser::yyundef_token_ = 2;



} // yy





int yylex( /*YYSTYPE*/void * /*_pvNodeUnion*/, CShaderLexer * _pslLexer ) {
	return _pslLexer->NewLex();
}

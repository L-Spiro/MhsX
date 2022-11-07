
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
#include "../PreProc/EEPreProcContainer.h"
#include "../PreProc/EEPreProcLexer.h"
#include <cstdio>

#pragma warning( push )

// warning C4065: switch statement contains 'default' but no 'case' labels
#pragma warning( disable : 4065 )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

// warning C4512: 'ee::slice<T,S>' : assignment operator could not be generated
#pragma warning( disable : 4512 )



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define YYINCLUDED_STDLIB_H


// Announce to Flex the prototype we want for lexing function.
extern int yylex( /*YYSTYPE*/void * _pvNodeUnion, ee::CPreProcLexer * _ppplLexer );


#include "../PreProc/EEPreProcSyntaxNodes.h"






#include "EEPreProcParser.hpp"

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
  parser::parser (class CPreProcLexer * m_ppplLexer_yyarg, class CPreProcContainer * m_pppcContainer_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      m_ppplLexer (m_ppplLexer_yyarg),
      m_pppcContainer (m_pppcContainer_yyarg)
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
	yychar = yylex (&yylval, m_ppplLexer);
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

    { m_pppcContainer->CreateIdentifier( m_ppplLexer->YYText(), (yyval.ndData) ); }
    break;

  case 3:

    { m_pppcContainer->CreateDecimal( m_ppplLexer->YYText(), (yyval.ndData) ); }
    break;

  case 4:

    { m_pppcContainer->CreateHexadecimal( m_ppplLexer->YYText(), (yyval.ndData) ); }
    break;

  case 5:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 6:

    { m_pppcContainer->CreateDefined( (yysemantic_stack_[(4) - (3)].ndData), (yyval.ndData) ); }
    break;

  case 7:

    { m_pppcContainer->CreateDefined( (yysemantic_stack_[(2) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 8:

    { (yyval.ndData) = (yysemantic_stack_[(3) - (2)].ndData); }
    break;

  case 9:

    { (yyval.ui64Op) = '!'; }
    break;

  case 10:

    { (yyval.ui64Op) = '~'; }
    break;

  case 11:

    { (yyval.ui64Op) = '-'; }
    break;

  case 12:

    { (yyval.ui64Op) = '+'; }
    break;

  case 13:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 14:

    { m_pppcContainer->CreateUnaryOp( static_cast<uint32_t>((yysemantic_stack_[(2) - (1)].ui64Op)), (yysemantic_stack_[(2) - (2)].ndData), (yyval.ndData) ); }
    break;

  case 15:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 16:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '*', (yyval.ndData) ); }
    break;

  case 17:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '/', (yyval.ndData) ); }
    break;

  case 18:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '%', (yyval.ndData) ); }
    break;

  case 19:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 20:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '+', (yyval.ndData) ); }
    break;

  case 21:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '-', (yyval.ndData) ); }
    break;

  case 22:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 23:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_PPT_SHIFT_LEFT, (yyval.ndData) ); }
    break;

  case 24:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_PPT_SHIFT_RIGHT, (yyval.ndData) ); }
    break;

  case 25:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 26:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '>', (yyval.ndData) ); }
    break;

  case 27:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '<', (yyval.ndData) ); }
    break;

  case 28:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_PPT_GE, (yyval.ndData) ); }
    break;

  case 29:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_PPT_LE, (yyval.ndData) ); }
    break;

  case 30:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 31:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_PPT_EQUALS, (yyval.ndData) ); }
    break;

  case 32:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_PPT_NOT_EQUALS, (yyval.ndData) ); }
    break;

  case 33:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 34:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '&', (yyval.ndData) ); }
    break;

  case 35:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 36:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '^', (yyval.ndData) ); }
    break;

  case 37:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 38:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), '|', (yyval.ndData) ); }
    break;

  case 39:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 40:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_PPT_AND, (yyval.ndData) ); }
    break;

  case 41:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 42:

    { m_pppcContainer->CreateMath( (yysemantic_stack_[(3) - (1)].ndData), (yysemantic_stack_[(3) - (3)].ndData), token::EE_PPT_OR, (yyval.ndData) ); }
    break;

  case 43:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 44:

    { m_pppcContainer->CreateTertiary( (yysemantic_stack_[(5) - (1)].ndData), (yysemantic_stack_[(5) - (3)].ndData), (yysemantic_stack_[(5) - (5)].ndData), (yyval.ndData) ); }
    break;

  case 45:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 46:

    { (yyval.ndData) = (yysemantic_stack_[(1) - (1)].ndData); }
    break;

  case 47:

    { m_pppcContainer->CreateTranslationUnit( (yysemantic_stack_[(2) - (1)].ndData), (yysemantic_stack_[(2) - (2)].ndData), (yyval.ndData) ); }
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
  const signed char parser::yypact_ninf_ = -19;
  const signed char
  parser::yypact_[] =
  {
        11,    -6,   -19,   -19,   -19,   -19,   -19,    11,   -19,   -19,
     -19,   -19,    11,   -19,    27,   -12,    30,    -3,    31,   -18,
     -14,    -9,     9,   -11,   -19,   -19,     1,    15,   -19,    -1,
     -19,    11,    11,    11,    11,    11,    11,    11,    11,    11,
      11,    11,    11,    11,    11,    11,    11,    11,    11,    11,
     -19,   -19,    28,   -19,   -19,   -19,   -19,    27,    27,   -12,
     -12,    30,    30,    30,    30,    -3,    -3,    31,   -18,   -14,
      -9,     9,   -10,   -19,    11,   -19
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  parser::yydefact_[] =
  {
         0,     0,     3,     4,     2,    12,    11,     0,     9,    10,
       5,    13,     0,    15,    19,    22,    25,    30,    33,    35,
      37,    39,    41,    43,    45,    46,     0,     0,     7,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,    47,     0,     8,    16,    17,    18,    20,    21,    23,
      24,    28,    29,    26,    27,    31,    32,    34,    36,    38,
      40,    42,     0,     6,     0,    44
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  parser::yypgoto_[] =
  {
       -19,     4,   -19,   -19,    18,    25,    26,    14,    22,     0,
      -8,    20,   -13,    19,   -19,    -5,    -7,   -19
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  parser::yydefgoto_[] =
  {
        -1,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char parser::yytable_ninf_ = -1;
  const unsigned char
  parser::yytable_[] =
  {
        29,    50,    48,     4,     1,    28,    44,     2,     3,    45,
       4,    38,    39,    46,     1,    34,    35,     2,     3,    51,
       4,    47,    40,    41,     4,    49,    27,    74,     5,     6,
      30,    52,    53,     7,    70,     8,     9,    68,     5,     6,
      36,    37,    72,     7,    67,     8,     9,    42,    43,    54,
      55,    56,    61,    62,    63,    64,    31,    32,    33,    57,
      58,    73,    59,    60,    65,    66,    69,    71,     0,    75
  };

  /* YYCHECK.  */
  const signed char
  parser::yycheck_[] =
  {
         7,     0,    13,     9,     3,     1,    24,     6,     7,    23,
       9,    14,    15,    22,     3,    27,    28,     6,     7,    26,
       9,    12,    25,    26,     9,    36,    32,    37,    27,    28,
      12,    27,    33,    32,    47,    34,    35,    45,    27,    28,
      10,    11,    49,    32,    44,    34,    35,    16,    17,    31,
      32,    33,    38,    39,    40,    41,    29,    30,    31,    34,
      35,    33,    36,    37,    42,    43,    46,    48,    -1,    74
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  parser::yystos_[] =
  {
         0,     3,     6,     7,     9,    27,    28,    32,    34,    35,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    32,    39,    54,
      42,    29,    30,    31,    27,    28,    10,    11,    14,    15,
      25,    26,    16,    17,    24,    23,    22,    12,    13,    36,
       0,    54,    39,    33,    42,    42,    42,    43,    43,    44,
      44,    45,    45,    45,    45,    46,    46,    47,    48,    49,
      50,    51,    54,    33,    37,    53
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
      47,    37,    40,    41,    33,   126,    63,    58
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  parser::yyr1_[] =
  {
         0,    38,    39,    40,    40,    40,    40,    40,    40,    41,
      41,    41,    41,    42,    42,    43,    43,    43,    43,    44,
      44,    44,    45,    45,    45,    46,    46,    46,    46,    46,
      47,    47,    47,    48,    48,    49,    49,    50,    50,    51,
      51,    52,    52,    53,    53,    54,    55,    55
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  parser::yyr2_[] =
  {
         0,     2,     1,     1,     1,     1,     4,     2,     3,     1,
       1,     1,     1,     1,     2,     1,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     1,     3,     3,     3,     3,
       1,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     5,     1,     1,     2
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const parser::yytname_[] =
  {
    "$end", "error", "$undefined", "EE_PPT_DEFINED", "EE_PPT_CHAR_CONSTANT",
  "EE_PPT_FLOAT_CONSTANT", "EE_PPT_DEC_CONSTANT", "EE_PPT_HEX_CONSTANT",
  "EE_PPT_OCT_CONSTANT", "EE_PPT_IDENTIFIER", "EE_PPT_SHIFT_LEFT",
  "EE_PPT_SHIFT_RIGHT", "EE_PPT_AND", "EE_PPT_OR", "EE_PPT_GE",
  "EE_PPT_LE", "EE_PPT_EQUALS", "EE_PPT_NOT_EQUALS", "EE_PPT_NOT",
  "EE_PPT_1SCOMPLIMENT", "EE_PPT_UMINUS", "EE_PPT_UPLUS", "'|'", "'^'",
  "'&'", "'>'", "'<'", "'+'", "'-'", "'*'", "'/'", "'%'", "'('", "')'",
  "'!'", "'~'", "'?'", "':'", "$accept", "identifier", "basic_exp",
  "unary_op", "unary_exp", "multiplicative_exp", "additive_exp",
  "shift_exp", "relational_exp", "equality_exp", "logical_and_exp",
  "logical_xor_exp", "logical_or_exp", "and_exp", "or_exp",
  "conditional_exp", "exp", "translation_unit", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const parser::rhs_number_type
  parser::yyrhs_[] =
  {
        55,     0,    -1,     9,    -1,     6,    -1,     7,    -1,    39,
      -1,     3,    32,    39,    33,    -1,     3,    39,    -1,    32,
      54,    33,    -1,    34,    -1,    35,    -1,    28,    -1,    27,
      -1,    40,    -1,    41,    42,    -1,    42,    -1,    43,    29,
      42,    -1,    43,    30,    42,    -1,    43,    31,    42,    -1,
      43,    -1,    44,    27,    43,    -1,    44,    28,    43,    -1,
      44,    -1,    45,    10,    44,    -1,    45,    11,    44,    -1,
      45,    -1,    46,    25,    45,    -1,    46,    26,    45,    -1,
      46,    14,    45,    -1,    46,    15,    45,    -1,    46,    -1,
      47,    16,    46,    -1,    47,    17,    46,    -1,    47,    -1,
      48,    24,    47,    -1,    48,    -1,    49,    23,    48,    -1,
      49,    -1,    50,    22,    49,    -1,    50,    -1,    51,    12,
      50,    -1,    51,    -1,    52,    13,    51,    -1,    52,    -1,
      52,    36,    54,    37,    53,    -1,    53,    -1,    54,    -1,
      55,    54,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  parser::yyprhs_[] =
  {
         0,     0,     3,     5,     7,     9,    11,    16,    19,    23,
      25,    27,    29,    31,    33,    36,    38,    42,    46,    50,
      52,    56,    60,    62,    66,    70,    72,    76,    80,    84,
      88,    90,    94,    98,   100,   104,   106,   110,   112,   116,
     118,   122,   124,   128,   130,   136,   138,   140
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  parser::yyrline_[] =
  {
         0,    84,    84,    88,    89,    90,    91,    92,    93,    97,
      98,    99,   100,   104,   105,   109,   110,   111,   112,   116,
     117,   118,   122,   123,   124,   128,   129,   130,   131,   132,
     136,   137,   138,   142,   143,   147,   148,   152,   153,   157,
     158,   162,   163,   167,   168,   171,   175,   176
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
       2,     2,     2,    34,     2,     2,     2,    31,    24,     2,
      32,    33,    29,    27,     2,    28,     2,    30,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,     2,
      26,     2,    25,    36,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    23,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    22,     2,    35,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int parser::yyeof_ = 0;
  const int parser::yylast_ = 69;
  const int parser::yynnts_ = 18;
  const int parser::yyempty_ = -2;
  const int parser::yyfinal_ = 50;
  const int parser::yyterror_ = 1;
  const int parser::yyerrcode_ = 256;
  const int parser::yyntokens_ = 38;

  const unsigned int parser::yyuser_token_number_max_ = 276;
  const parser::token_number_type parser::yyundef_token_ = 2;



} // yy






int yylex( /*YYSTYPE*/void * _pvNodeUnion, ee::CPreProcLexer * _ppplLexer ) {
	static_cast<void *>(_pvNodeUnion);
	return _ppplLexer->NewLex();
}


void yy::parser::error( const yy::location &_lLoc, const std::string &_strM ) {
	static_cast<const yy::location &>(_lLoc);
	static_cast<const std::string &>(_strM);
	/*std::string sError;
	std::string sFile;
	int iLine;
	m_ppplLexer->get_line( sFile, iLine );
	if ( sFile.size() ) {
		// In a file.
		sError = "ERROR: " + sFile + " line: " + CString::CreateFromUI64( iLine ).CStr() + ": " + _strM;
	}
	else {
		sError = "ERROR: line: " + std::string( CString::CreateFromUI64( iLine ).CStr() ) + ": " + _strM;
	}
	CStd::DebugPrintA( sError.c_str() );*/
}

#pragma warning( pop )

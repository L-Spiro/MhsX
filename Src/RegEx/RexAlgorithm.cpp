#include "RexAlgorithm.h"
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include <iostream>
#include <bitset>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <utility>
#include <fstream>
using namespace std;

typedef bitset<256>			REXA_CharSet;
typedef int					REXA_StateId;

template<class TSet>
struct REXA_LessSet{
    bool operator()(const TSet& rTSet1, const TSet& rTSet2)const
    {
        if( rTSet1==rTSet2 ){
            return false;
        }
        for(int i=0;i<(int)rTSet1.size();i++){
            if( rTSet1[i]!=rTSet2[i] )
                return rTSet1[i]<rTSet2[i];
        }
        return false;
    }
};
typedef REXA_LessSet<REXA_CharSet>  REXA_LessCharSet;
typedef set<REXA_CharSet,REXA_LessCharSet> REXA_CharSetSet;


// State and Translationtable
struct REXA_NDFAState;

struct REXA_StateSet : public  set<const struct REXA_NDFAState*>
{
	bool	IsAccepting		()const;
};

typedef vector<REXA_StateSet>		TransitionVector;



struct REXA_NDFATransition{

	REXA_NDFATransition(const REXA_CharSet& m_rTransChars,REXA_NDFAState* pStateNext)
		:m_transChars(m_rTransChars),m_nextState(pStateNext)
	{
	}
    REXA_NDFATransition(const char* pcszChars=0,REXA_NDFAState* pNextState=0)
    {
		if(!pcszChars || strlen(pcszChars)==0 ){
			m_bIsMute=true;
			return;
		}
        const unsigned char* pucszChars= (const unsigned char*)pcszChars;
        m_nextState= pNextState;
        while( *pucszChars ){
            m_transChars[*pucszChars++]= true;
        }
    }
    REXA_CharSet     m_transChars;
    REXA_NDFAState*  m_nextState;
	bool		m_bIsMute;
};
struct REXA_NDFAState{
    REXA_NDFAState(int id=0,bool isAccepting=false)
        :m_id(id),
        m_nAnswer(REXA_DFAState::eNoAnswer),
		m_isAccepting(isAccepting)
    {
    }

    static void			CollectStates   (const REXA_NDFAState* pState,
											set<const REXA_NDFAState*>& rsetStates);
    static void         CollectStates   (REXA_NDFAState* pState,
                                            set<REXA_NDFAState*>& rsetStates);

    static REXA_NDFAState*	CopyStates	(REXA_NDFAState* pState,
									map<REXA_NDFAState*,REXA_NDFAState*>& rMapStates);
	REXA_NDFAState*			Clone		();
    void				DeleteAll	();

    int                         m_id;
	int							m_nAnswer;
    bool                        m_isAccepting;
    vector<REXA_NDFATransition>      m_transitions;

};
struct REXA_TranslatingTable{
	typedef	map< REXA_StateSet,TransitionVector >	TranslationMap;
	typedef	map<const REXA_NDFAState*,int>			MapStateOccurrenceCount;

    REXA_TranslatingTable    (const REXA_NDFAState* pStartState);

	void					ComputeOccCounts();
	int						GetAnswerCode	(const REXA_StateSet& rSet)const;

	vector<REXA_CharSet>		m_vecCharsets;
    const REXA_NDFAState*	m_pStartState;
	TranslationMap		m_mapTransitions;

	MapStateOccurrenceCount		m_mapStateOccurrenceCount;
    
private:
    vector<REXA_CharSet> 
            CharsetToVector			(const REXA_CharSetSet& rCharSetSet);

    set<REXA_CharSet,REXA_LessCharSet> 
            ComputeDisjointCharSets     (REXA_CharSetSet& rCharSets);

    void    GenerateTranslationTable	( set<const REXA_NDFAState*>& rSetState);

    void    CollectCharSets     (   const REXA_NDFAState* pCurState,
                                    set<int>& rVisitedStates,REXA_CharSetSet& rCharSets);
};

class REXA_NDFAToTable{
public:
    void     NDFA_To_Table      (REXA_NDFAState* pStartState,
                                REXA_TranslatingTable& rTable);

static vector<REXA_CharSet> 
            CharsetToVector     (const REXA_CharSetSet& rCharSetSet);

    set<REXA_CharSet,REXA_LessCharSet> 
            ComputeDisjointCharSets     (REXA_CharSetSet& rCharSets);

static void    GenerateTranslationTable    ( map<int,REXA_NDFAState*>& rMapIdState,
                                                  REXA_TranslatingTable& rTable);


static void CollectCharSets     (   REXA_NDFAState* pCurState,
                                    set<int>& rVisitedStates,
									REXA_CharSetSet& rCharSets);
};

class REXA_NDFAToDfa{
public:
	typedef			map<REXA_NDFAState*,int> MapStateOccurrenceCount;

	REXA_NDFAToDfa		(const REXA_TranslatingTable& rTable,char cEos);
    REXA_DFAState*		TableToDfaStates();
private:
	REXA_DFAState*		CreateDfaStates(
						const REXA_StateSet&				rStateSet,
						map<REXA_StateSet,REXA_DFAState*>&	rMapStateSetDFAState);

	const REXA_TranslatingTable&		m_rTable;
	unsigned char				        m_cEos;
};

class REXA_Scanner{
    friend class REXA_ParserImpl;
    friend class REXA_Exception;
public:
//constructor,desctructor,typedefs
	REXA_Scanner(const char* pcszToParse="")   {SetSource(pcszToParse);}
private:
    void SetSource(const char* pcszSrc){
                                        m_pcszToParse= pcszSrc;m_pTokBeg= pcszSrc;
                                        m_pTokEnd= pcszSrc;m_eLastSymbol= eEof;
                                        }
//operations
	REXA_ESymbols	operator()		();
    
    int             GetLastSymbol   (){return m_eLastSymbol;}
static string       GetSymbolString (REXA_ESymbols eRegexpSymbol);
    void            AdvanceAndMatch (REXA_ESymbols eSymbol);
	void			Match			(REXA_ESymbols eSymbol);
	REXA_BegEnd		GetToken		(){return make_pair(m_pTokBeg,m_pTokEnd);}
	const char*		GetSource		(){return m_pcszToParse;}
	REXA_CharSet			GetCharSet		(){return m_charSet;}
    char            GetChar         (const char*& rpFrom,bool& rbIsEscape);
    void            AddRange        (   unsigned char cFirst,
                                        unsigned char cLast, 
                                        REXA_CharSet& rCharSet);
    const char*     SkipWhiteSpace  ();
	const char*		m_pcszToParse;
	const char*		m_pTokBeg;
    const char*     m_pTokEnd;
	REXA_CharSet			m_charSet;
    string          m_strLiteral;
	REXA_ESymbols   m_eLastSymbol;
};

class REXA_ParserImpl{
public:
	REXA_ParserImpl				(char cEos=0)
        :m_cEos(cEos),m_eAtomSet(eCharSet|eIdent|eLParen),
        m_nStateIdMax(0)
	{
    }
    ~REXA_ParserImpl();
    const REXA_NDFAState* ParseRegExp           (string strRegExp);
    bool       ParseRegExpDefinition (string strRegExpDefiniton,int nAnswer);
 
    char                    m_cEos;
    map<string,REXA_NDFAState*>  m_mapNamedStates;
private:
// grammar related functions
	void       NamedRegExp		(int nAnswer);
	REXA_NDFAState* RegExp			();
	REXA_NDFAState* Term				();
	REXA_NDFAState* Factor			();
	REXA_NDFAState* Atom				();
    REXA_NDFAState* Name             (REXA_BegEnd nameToFind);
    REXA_NDFAState* ParseCharSet     (REXA_CharSet&	rCharSet);	
//NDFA-Automtaton constructors
    REXA_NDFAState* Concat           (REXA_NDFAState* pState0,REXA_NDFAState* pState1);
    REXA_NDFAState* Union            (REXA_NDFAState* pState0,REXA_NDFAState* pState1);
    REXA_NDFAState* MakeStar         (REXA_NDFAState* pState);
    REXA_NDFAState* MakePlus         (REXA_NDFAState* pState);
    REXA_NDFAState* MakeOptional     (REXA_NDFAState* pState);
    REXA_NDFAState* MakeIgnoreCase   (REXA_NDFAState* pState);
//helper functions for grammar related functions
    void			InsertNamedState(	REXA_BegEnd name,
										REXA_NDFAState* pState,int nAnwer);
 
	vector<pair<REXA_NDFAState*,REXA_NDFATransition> >
					CollectAcceptingTransitions(REXA_NDFAState* pState);
    vector<REXA_NDFAState*> CollectAcceptingStates(REXA_NDFAState* pState);
	void			CollectStates(
							REXA_NDFAState* pState,
							set<REXA_NDFAState*>& rsetStates);

    REXA_NDFAState*               CloneNDFA(REXA_NDFAState* pToClone);
    REXA_NDFAState*               GetNewNDFAState(int id=0,bool isAccepting=false);

    int                     NextStateId          ();

	REXA_Scanner   	        m_scanner;
    const int               m_eAtomSet;
    int                     m_nStateIdMax;

    vector<REXA_NDFAState*>      m_vecAllStates;
};


static struct SymbolString{
    REXA_ESymbols   eSym;
    const char*     strSym;
}g_symbolStrings[]={
    {   eEof,       "eos"				},
    {   eIllegal,   "ill"				},
    {   eStar,      "*"                 },
    {   ePlus,      "+"                 },
    {   eAssign,    "="                 },
    {   eOpt,       "?"                 },
    {   eAlternative,"|"                },
    {   eCharSet,   "[<ch>]"			},
    {   eLParen,    "("                 },
    {   eRParen,    ")"                 }
};

//-------------------------------------------------------------------------------
REXA_Exception::REXA_Exception(
        const char* pPos,
        const char* pRegExp,
        string strMsg)
//-------------------------------------------------------------------------------
{
    m_strErrMsg=   strMsg;
    m_nErrPos= (int)(pPos-pRegExp);
}
//-------------------------------------------------------------------------------
REXA_Exception::REXA_Exception(
		REXA_ESymbols eExpectedSymbol,
		REXA_BegEnd    token,
		const char*   pRegExp,
		string		  strMsg)
//-------------------------------------------------------------------------------
{
    
    m_strErrMsg=  REXA_Scanner::GetSymbolString(eExpectedSymbol)    
		+  " exp\r\n  ";
    m_nErrPos= (int)(token.first - pRegExp);
}
//-------------------------------------------------------------------------------
string       REXA_Scanner::GetSymbolString     (REXA_ESymbols eRegexpSymbol)
//-------------------------------------------------------------------------------
{
    for(int i=0;i<sizeof(g_symbolStrings)/sizeof(SymbolString);i++){
        if( g_symbolStrings[i].eSym==eRegexpSymbol ){
            return g_symbolStrings[i].strSym;
        }
    }
    return "";
}

//-------------------------------------------------------------------------------
const char*    REXA_Scanner::SkipWhiteSpace    ()
//-------------------------------------------------------------------------------
{
    const char* pCur= m_pTokEnd;
    while(isspace(*pCur))
        pCur++;
    return pCur;
}

//------------------------------------------------------------------------------
void			REXA_Scanner::Match		    (REXA_ESymbols eSymbol)
//------------------------------------------------------------------------------
{
	if( eSymbol!=m_eLastSymbol ){
        throw REXA_Exception(eSymbol,GetToken(),m_pcszToParse);
	}
}
//------------------------------------------------------------------------------
void            REXA_Scanner::AdvanceAndMatch (REXA_ESymbols eSymbol)
//------------------------------------------------------------------------------
{
    if( eSymbol!=operator()() ){
        throw REXA_Exception(eSymbol,GetToken(),m_pcszToParse);
    }
}
//------------------------------------------------------------------------------
char          REXA_Scanner::GetChar(const char*& rpFrom,bool& rbIsEscape)
//------------------------------------------------------------------------------
{
    rbIsEscape= false;
    if( *rpFrom=='\0' ) return '\0';
    char c;
    if( *rpFrom=='\\' ){
        rbIsEscape= true;
        rpFrom++;
        switch(*rpFrom){
        case '0':  return '\0';
        case 'n':  rpFrom++; return '\n';
        case 't':  rpFrom++; return '\t';
        case 'v':  rpFrom++; return '\v';
        case 'a':  rpFrom++; return '\a';
        case 'b':  rpFrom++; return '\b';
        case 'r':  rpFrom++; return '\r';
        case 'x':  c= *rpFrom++; 
            if( isxdigit(rpFrom[0]) && isxdigit(rpFrom[1]) ){
                unsigned hexChar=0;
                for(int i=0;i<2;i++){
                    hexChar*=16;
                    hexChar+= isdigit(rpFrom[i])?(rpFrom[i]-'0'):(toupper(rpFrom[i])-'A'+16);
                }
                c= (char)hexChar;
                rpFrom+= 2;
            }
            return c;
        default:    c= *rpFrom++; return c;
        }
    }
    return *rpFrom++;
}
//------------------------------------------------------------------------------
void    REXA_Scanner::AddRange(
                           unsigned char cFirst, 
                           unsigned char cLast, 
                           REXA_CharSet& rCharSet)
//------------------------------------------------------------------------------
{
    if( cFirst>cLast ){
        swap(cFirst,cLast);
    }
    for(unsigned char c= cFirst; c<cLast;c++){
        rCharSet[c]= true;
    }
    rCharSet[cLast]= true;
}
//------------------------------------------------------------------------------
REXA_ESymbols REXA_Scanner::operator()	()
//------------------------------------------------------------------------------
{
	m_pTokBeg= m_pTokEnd= SkipWhiteSpace();
    switch(*m_pTokEnd){
    case '\0':  return m_eLastSymbol=eEof;
    case '+':   m_pTokEnd++; return m_eLastSymbol=ePlus;
    case '?':   m_pTokEnd++; return m_eLastSymbol=eOpt;
    case '*':   m_pTokEnd++; return m_eLastSymbol=eStar;
    case '(':   m_pTokEnd++; return m_eLastSymbol=eLParen;
    case ')':   m_pTokEnd++; return m_eLastSymbol=eRParen;
    case '=':   m_pTokEnd++; return m_eLastSymbol=eAssign;
	case '|':   m_pTokEnd++; return m_eLastSymbol=eAlternative;
    case '[': {
                m_pTokEnd++;
                REXA_CharSet cset;
                char c;
                bool bIsEscape;
				bool bIsFirstTime= true;
                bool bPrevWasRangeSeparator= false;
				bool bUseComplement= false;
                char cPrev;
				cPrev=GetChar(m_pTokEnd,bIsEscape);
				if( cPrev=='^' && !bIsEscape ){
					bUseComplement= true;
                    cPrev=GetChar(m_pTokEnd,bIsEscape);
				}
				while(	cPrev	&&	!(cPrev==']' && !bIsEscape) ){
					c=GetChar(m_pTokEnd,bIsEscape);
					if( !bIsEscape && c=='-'  ){
						c=GetChar(m_pTokEnd,bIsEscape);
                        if(!c){
                            break;
                        }
						AddRange(
							(unsigned char)cPrev,(unsigned char)c,cset);
						cPrev= GetChar(m_pTokEnd,bIsEscape);
					}else{
                        cset[(unsigned char)cPrev]= true;
                        cPrev= c;
					}
				}
                if( !cPrev  || !c ){
                    throw REXA_Exception(
                                m_pTokBeg,m_pcszToParse,"no ] ");
                }
                m_charSet= cset;
				if(bUseComplement){
					m_charSet.flip();
				}
                return m_eLastSymbol=eCharSet;
              }
    case '.':{
                m_pTokEnd++;
                m_charSet= REXA_CharSet();
                for(int i=0;i<256;i++){
                    m_charSet[i]= true;
                }
                return m_eLastSymbol=eCharSet;
             }
    case '$':{
                if(  isalpha(m_pTokEnd[1])||m_pTokEnd[1]=='_' ){
                    m_pTokEnd++;
                    m_strLiteral="";
                    while(  isalpha(*m_pTokEnd)||isdigit(*m_pTokEnd)||m_pTokEnd[1]=='_' ){
                        m_strLiteral+= *m_pTokEnd++;
                    }
                    return m_eLastSymbol=eIdent;
                 }
             }
    default:
        {
            if( *m_pTokEnd=='\\' && m_pTokEnd[1]=='i' ){
                m_pTokEnd++; 
                m_pTokEnd++;
                return m_eLastSymbol=eIgnoreCase;
            }else{
                bool bIsEscape;
                char c=GetChar(m_pTokEnd,bIsEscape);
                m_charSet= REXA_CharSet();
                m_charSet[(unsigned char)c]= true;
                return m_eLastSymbol=eCharSet;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// grammar related functions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
REXA_ParserImpl::~REXA_ParserImpl()
//-----------------------------------------------------------------------------
{
    for(int i=0;i<(int)m_vecAllStates.size();++i){
        delete m_vecAllStates[i];
    }
}

//-----------------------------------------------------------------------------
void		REXA_ParserImpl::InsertNamedState(REXA_BegEnd name,
										   REXA_NDFAState* pState,int nAnswer)
//-----------------------------------------------------------------------------
{
	vector<REXA_NDFAState*>  vecAccepting= CollectAcceptingStates(pState);
	for(int i=0;i<(int)vecAccepting.size();i++){
		vecAccepting[i]->m_nAnswer= nAnswer;
	}
    pair<map<string,REXA_NDFAState*>::iterator,bool> itBool=
        m_mapNamedStates.insert(make_pair(string(name.first,name.second),pState));
    if( !itBool.second ){
        itBool.first->second= pState;
    }
}
//-----------------------------------------------------------------------------
void REXA_ParserImpl::NamedRegExp		(int nAnswer)
//-----------------------------------------------------------------------------
//$1 NamedRegExp= Ident "=" RegExp.
{
	m_scanner.Match(eIdent);
	REXA_BegEnd name= m_scanner.GetToken();

	m_scanner.AdvanceAndMatch(eAssign);
    m_scanner();
	REXA_NDFAState* pState=RegExp();
    if( m_scanner.GetLastSymbol()!=eEof){
         throw REXA_Exception(
                        m_scanner.m_pTokBeg,
                        m_scanner.m_pcszToParse,
                        "unx symb");
    }
	InsertNamedState(name,pState,nAnswer);
}
//-----------------------------------------------------------------------------
const REXA_NDFAState*  REXA_ParserImpl::ParseRegExp          (string strRegExp)
//-----------------------------------------------------------------------------
{
    m_scanner.SetSource(strRegExp.c_str());
    m_scanner();
    const REXA_NDFAState* cRegExp= RegExp();
    if( m_scanner.GetLastSymbol()!=eEof){
         throw REXA_Exception(
                        m_scanner.m_pTokBeg,
                        m_scanner.m_pcszToParse,
                        "unx symb");
    }
    return cRegExp;
}
//-----------------------------------------------------------------------------
bool       REXA_ParserImpl::ParseRegExpDefinition(string strRegExpDefiniton,
											   int nAnswer)
//-----------------------------------------------------------------------------
{
    m_scanner.SetSource(strRegExpDefiniton.c_str());
    m_scanner();
    NamedRegExp(nAnswer);
    return true;
}
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::RegExp			()
//-----------------------------------------------------------------------------
//$2 RegExp= Term {"|" Term}.
{
    REXA_NDFAState* pState= Term();
    while( m_scanner.GetLastSymbol()==eAlternative ){
        m_scanner();
        pState= Union(pState,Term());
    }
    return pState;
}
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::Term			()
//-----------------------------------------------------------------------------
//$3 Term= Factor {Factor}.
{
    REXA_NDFAState* pState= Factor();
    while( m_scanner.GetLastSymbol()&m_eAtomSet ){
        pState= Concat(pState,Factor());
    }
    return pState;
}
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::Factor			()
//-----------------------------------------------------------------------------
//$4 Factor= Atom ["*"|"+"|"?"|"\i"].
{
    REXA_NDFAState* pState= Atom();
    switch(m_scanner.GetLastSymbol()){
    case eStar:
        {
            pState= MakeStar(pState);
            m_scanner();
            return pState;
        }
    case ePlus:
        {
            pState= MakePlus(pState);
            m_scanner();
            return pState;
        }
    case eOpt:
        {
            pState= MakeOptional(pState);
            m_scanner();
            return pState;
        }
    case eIgnoreCase:
        {
            pState= MakeIgnoreCase(pState);
            m_scanner();
            return pState;
        }
    default:
        return pState;
    }
}
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::Atom				()
//-----------------------------------------------------------------------------
//$5 Atom= REXA_CharSet |  Ident | "(" RegExp ").
{
    switch(m_scanner.GetLastSymbol()){
    case eCharSet:
        {
			REXA_NDFAState* pState= ParseCharSet(m_scanner.GetCharSet());
            m_scanner();
            return pState;
        }

    case eIdent:
        {
            REXA_NDFAState* pState= Name(m_scanner.GetToken());
			m_scanner();
            return pState;
        }
    case eLParen:
        {
			m_scanner();
            REXA_NDFAState* pState= RegExp();
            m_scanner.Match(eRParen);
            m_scanner();
            return pState;
        }
    default: throw REXA_Exception(
                        m_scanner.m_pTokBeg,
                        m_scanner.m_pcszToParse,
                        "[,',( or iden exp");
        
    }
    return 0;
}
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::Name             (REXA_BegEnd nameToFind)
//-----------------------------------------------------------------------------
{

	map<string,REXA_NDFAState*>::iterator it=
		m_mapNamedStates.find(string(nameToFind.first,nameToFind.second));
	if( it!= m_mapNamedStates.end() ){
		return CloneNDFA(it->second);;
	}
    throw REXA_Exception(   m_scanner.m_pTokBeg,
                            m_scanner.m_pcszToParse,
                            string(nameToFind.first,nameToFind.second-nameToFind.first)
                            + " not in name dict");
    return 0;
}
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::ParseCharSet   (REXA_CharSet& rCharSet)
//-----------------------------------------------------------------------------
{
    rCharSet[(unsigned char)m_cEos]=false;
    REXA_NDFAState* pStartState= GetNewNDFAState(NextStateId(),false);
	REXA_NDFAState* pFinalState= GetNewNDFAState(NextStateId(),true);
	pStartState->m_transitions.push_back(REXA_NDFATransition(rCharSet,pFinalState));
    return pStartState;
}

///////////////////////////////////////////////////////////////////////////////
//NDFA-Automtaton constructors
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::Concat         (REXA_NDFAState* pState0,REXA_NDFAState* pState1)
//-----------------------------------------------------------------------------
{ //Terms: F0 resp. F1: StateGraph reachable from 'pState0' resp. 'pState1'
	//1. for each transition in F0 leading from state q to an accepting state 
	//   add a transition leading from q to 'pState1'
	vector<pair<REXA_NDFAState*,REXA_NDFATransition> > vecTrans= 
								CollectAcceptingTransitions(pState0);
    vector<REXA_NDFAState*>  vecAccepting= CollectAcceptingStates(pState0);
	for(int i=0;i<(int)vecTrans.size();i++){
		vecTrans[i].first->m_transitions.push_back(
			REXA_NDFATransition(vecTrans[i].second.m_transChars,pState1));
	}
	//2. if 'pState0' is accepting 
	//   add all transitions from 'pState1'  to 'pState0'
	if( pState0->m_isAccepting ){
		pState0->m_transitions.insert(	pState0->m_transitions.end(),
										pState1->m_transitions.begin(),
										pState1->m_transitions.end());
	}
	//3. accepting states of F1 are accepting states of the catenation
	//   accepting states of F0 are no langer accepting
	for(int j=0;j<(int)vecAccepting.size();j++){
		vecAccepting[j]->m_isAccepting= false;
	}
    return pState0;
}
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::Union          (REXA_NDFAState* pState0,REXA_NDFAState* pState1)
//-----------------------------------------------------------------------------
{ //1. 'pState0' gets also the transitions of 'pState1'
	pState0->m_transitions.insert(	pState0->m_transitions.end(),
									pState1->m_transitions.begin(),
									pState1->m_transitions.end());
 //2. if one of the start states is accepting the new start state is also accepting
	if( pState1->m_isAccepting ){
		pState0->m_isAccepting= true;
	} 
    return pState0;
}
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::MakeStar       (REXA_NDFAState* pState)
//-----------------------------------------------------------------------------
{ // //1. for each transition  leading from state q to an accepting state 
	//   add a transition leading from q to 'pState
    vector<pair<REXA_NDFAState*,REXA_NDFATransition> > vecTrans= 
								CollectAcceptingTransitions(pState);
	for(int i=0;i<(int)vecTrans.size();i++){
		vecTrans[i].first->m_transitions.push_back(
			REXA_NDFATransition(vecTrans[i].second.m_transChars,pState));
	}
	pState->m_isAccepting=true;
    return pState;
}
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::MakePlus       (REXA_NDFAState* pState)
//-----------------------------------------------------------------------------
{ //catenate original state graph and its dupplication; then apply MakeStar
    return Concat(pState,MakeStar(CloneNDFA(pState)));
}
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::MakeOptional   (REXA_NDFAState* pState)
//-----------------------------------------------------------------------------
{ //start state gets optional
    pState->m_isAccepting= true;
	return pState;
}
//-----------------------------------------------------------------------------
REXA_NDFAState* REXA_ParserImpl::MakeIgnoreCase (REXA_NDFAState* pState)
//-----------------------------------------------------------------------------
{
    set<REXA_NDFAState*>     setStates;
    REXA_NDFAState::CollectStates(pState,setStates);
    set<REXA_NDFAState*>::iterator it;
    for(it= setStates.begin();it!=setStates.end();++it){
        for(int i=0;i<(int)(*it)->m_transitions.size();i++){
            for(int j=0;j<(int)(*it)->m_transitions[i].m_transChars.size();j++){
                if(     (*it)->m_transitions[i].m_transChars[j]
                    &&  isupper(j) ){
                    (*it)->m_transitions[i].
                        m_transChars[(unsigned char)(tolower(j))]= true;
                }
            }
        }
    }
    return pState;
}

//NDFA-Automaton helper  functions
//-----------------------------------------------------------------------------
vector<REXA_NDFAState*>  REXA_ParserImpl::CollectAcceptingStates(REXA_NDFAState* pState)
//-----------------------------------------------------------------------------
{
    set<REXA_NDFAState*>     setStates;
    vector<REXA_NDFAState*>  vecAcceptingStates;
    REXA_NDFAState::CollectStates(pState,setStates);
    set<REXA_NDFAState*>::iterator it;
    for(it= setStates.begin();it!=setStates.end();++it){
        if( (*it)->m_isAccepting ){
            vecAcceptingStates.push_back(*it);
        }
    }
    return vecAcceptingStates;
}

//-----------------------------------------------------------------------------
vector<pair<REXA_NDFAState*,REXA_NDFATransition> >	
			REXA_ParserImpl::CollectAcceptingTransitions(REXA_NDFAState*	pState)
//-----------------------------------------------------------------------------
{
	vector<pair<REXA_NDFAState*,REXA_NDFATransition> > vecStatTrans;

	set<REXA_NDFAState*> setStates;
    REXA_NDFAState::CollectStates(pState,setStates);
	set<REXA_NDFAState*>::iterator	it;
	for(it= setStates.begin();it!=setStates.end();++it){
        for(int i=0;i<(int)(*it)->m_transitions.size();i++){
            if( (*it)->m_transitions[i].m_nextState->m_isAccepting ){
                vecStatTrans.push_back(make_pair(*it,(*it)->m_transitions[i]));
            }
        }
    }
	return vecStatTrans;
}

//-------------------------------------------------------------------------------
int  REXA_ParserImpl::NextStateId                  ()
//-------------------------------------------------------------------------------
{
    return m_nStateIdMax++;
}
//-------------------------------------------------------------------------------
REXA_NDFAState*   REXA_ParserImpl::CloneNDFA(REXA_NDFAState* pToClone)
//-------------------------------------------------------------------------------
{
    set<REXA_NDFAState*> setStates;
    REXA_NDFAState* pState= pToClone->Clone();
    REXA_NDFAState::CollectStates(pState,setStates);
    set<REXA_NDFAState*>::const_iterator it;
    for(it= setStates.begin();it!=setStates.end();++it){
        (*it)->m_id= NextStateId();
        m_vecAllStates.push_back(*it);
    }
    return pState;
}
//-------------------------------------------------------------------------------
REXA_NDFAState*   REXA_ParserImpl::GetNewNDFAState(int id,bool isAccepting)
//-------------------------------------------------------------------------------
{
    REXA_NDFAState* pState= new REXA_NDFAState(id,isAccepting);
    m_vecAllStates.push_back(pState);
    return pState;
}


///////////////////////////////////////////////////////////////////////////
//  NDFA    State
///////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
void    REXA_NDFAState::DeleteAll()
//-----------------------------------------------------------------------------
{
    set<const REXA_NDFAState*> setStates;
    CollectStates(this,setStates);
    set<const REXA_NDFAState*>::iterator it;
    for(it= setStates.begin();it!=setStates.end();++it){
        delete *it;
    }
}
//-------------------------------------------------------------------------------
void         REXA_NDFAState::CollectStates(const REXA_NDFAState* pState,set<const REXA_NDFAState*>& rsetStates)
//-------------------------------------------------------------------------------
{
    pair<set<const REXA_NDFAState*>::iterator,bool> itBool=    rsetStates.insert(pState);
    if( itBool.second ){
        for(int i=0;i<(int)(*itBool.first)->m_transitions.size();i++){
            CollectStates((*itBool.first)->m_transitions[i].m_nextState,rsetStates);
        }
    }
}
//-------------------------------------------------------------------------------
void         REXA_NDFAState::CollectStates   (REXA_NDFAState* pState,
                                            set<REXA_NDFAState*>& rsetStates)
//-------------------------------------------------------------------------------
{
    pair<set<REXA_NDFAState*>::iterator,bool> itBool=    rsetStates.insert(pState);
    if( itBool.second ){
        for(int i=0;i<(int)(*itBool.first)->m_transitions.size();i++){
            CollectStates((*itBool.first)->m_transitions[i].m_nextState,rsetStates);
        }
    }
}
//-------------------------------------------------------------------------------
REXA_NDFAState*  REXA_NDFAState::CopyStates      
                        (REXA_NDFAState* pState,map<REXA_NDFAState*,REXA_NDFAState*>& rMapStates)
//-------------------------------------------------------------------------------
{
	pair<map<REXA_NDFAState*,REXA_NDFAState*>::iterator,bool> itDone= 
			rMapStates.insert(make_pair(pState,(REXA_NDFAState*)0));
	if( itDone.second ){
		REXA_NDFAState* pNewState= new REXA_NDFAState(*pState);
        itDone.first->second= pNewState;
		for(int i=0;i<(int)pNewState->m_transitions.size();i++){
			pNewState->m_transitions[i].m_nextState=
				CopyStates(pNewState->m_transitions[i].m_nextState,rMapStates);
		}
	}
    return itDone.first->second;
}


//-----------------------------------------------------------------------------
REXA_NDFAState*		REXA_NDFAState::Clone()
//-----------------------------------------------------------------------------
{
	map<REXA_NDFAState*,REXA_NDFAState*> mapStates;
	return CopyStates(this,mapStates);
}
///////////////////////////////////////////////////////////////////////////
//  REXA_StateSet
///////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
bool	REXA_StateSet::IsAccepting	()const
//-----------------------------------------------------------------------------
{
	REXA_StateSet::const_iterator it;
	for(it= begin();it!=end();++it){
		if( (*it)->m_isAccepting ){
			return true;
		}
	}
	return false;
}


//-----------------------------------------------------------------------------
void PrintCharSet(ostream& os,const REXA_CharSet& rCharSet)
//-----------------------------------------------------------------------------
{
	os	<<	"\"";
	for(int i=0;i<(int)rCharSet.size();i++){
		if( rCharSet[i] ){
			os	<<	(char)i;
		}
	}
	os	<<	"\"";
}
//-----------------------------------------------------------------------------
int				REXA_TranslatingTable::GetAnswerCode	(const REXA_StateSet& rSet)const
//-----------------------------------------------------------------------------
{	
	REXA_StateSet::const_iterator it;
	int nAnswerCode=	REXA_DFAState::eNoAnswer;
	int nMinCount=		INT_MAX;
	for(it= rSet.begin();it!=rSet.end();++it){
		MapStateOccurrenceCount::const_iterator itMap=
			m_mapStateOccurrenceCount.find((*it));
		if( itMap!=m_mapStateOccurrenceCount.end() ){
			if( itMap->second<nMinCount ){
				nMinCount=		itMap->second;
				nAnswerCode=	itMap->first->m_nAnswer;
			}
		}
	}
	return nAnswerCode;
}
//-----------------------------------------------------------------------------
void			REXA_TranslatingTable::ComputeOccCounts	()
//-----------------------------------------------------------------------------
{	//computes the occurrences of all states with an existing answer code
	TranslationMap::const_iterator it;
	for(	it= m_mapTransitions.begin();
			it!=m_mapTransitions.end();
			++it){
				REXA_StateSet::const_iterator it1;
				for( it1= it->first.begin();it1!=it->first.end();++it1){
					if(		(*it1)->m_isAccepting 
						&&	(*it1)->m_nAnswer!=REXA_DFAState::eNoAnswer){
						pair<MapStateOccurrenceCount::iterator,bool> pairRes=
							m_mapStateOccurrenceCount.insert(make_pair(*it1,1));
						if( !pairRes.second ){
							pairRes.first->second++;
						}
					}
				}
			}
}

//-----------------------------------------------------------------------------
set<REXA_CharSet,REXA_LessCharSet> 
            REXA_TranslatingTable::ComputeDisjointCharSets(REXA_CharSetSet& rCharSets)
//-----------------------------------------------------------------------------
{
    vector< REXA_CharSetSet > charSets(256);
    vector< REXA_CharSet >    unionSets(256);
    REXA_CharSetSet           resultSet;
    REXA_CharSetSet::const_iterator it;
    for(it= rCharSets.begin();it!=rCharSets.end();++it){
        charSets[it->count()].insert(*it);
    }

//cumulative union
    for(int k= 1;k<256;k++){
        for(int j=0;j<k;j++){
            for(it= charSets[j].begin();it!= charSets[j].end();++it){
                unionSets[k]|= *it;
            }
        }
    }

//reduce charset into disjoint charsets
    for(int l=0;l<256;l++){
        for(it= charSets[l].begin();it!= charSets[l].end();++it){
            REXA_CharSet reducedSet= *it;
            REXA_CharSet unionSet= ~unionSets[l];
            reducedSet&= unionSet;
            resultSet.insert(reducedSet);
        }
    }

    return resultSet;
}
//-----------------------------------------------------------------------------
void REXA_TranslatingTable::CollectCharSets(const REXA_NDFAState* pCurState,
                     set<int>& rVisitedStates,
                     REXA_CharSetSet& rCharSets)
//-----------------------------------------------------------------------------
{
    if( rVisitedStates.find(pCurState->m_id) == rVisitedStates.end() ){
        rVisitedStates.insert(pCurState->m_id);
        for(int i=0;i<(int)pCurState->m_transitions.size();i++){
            if( pCurState->m_transitions[i].m_transChars.count()!=0 ){
                rCharSets.insert(pCurState->m_transitions[i].m_transChars);
                CollectCharSets(
					pCurState->m_transitions[i].m_nextState,
					rVisitedStates,rCharSets);
            }
        }
    }
}



//# Generates a translation table from the NDFA-states present in 'vecpStates'
//example: NDFA having the States S,A,E.  S('a') -> A. S('a') -> E. A('b') -> A. A('b') -> E.
//TranslationTable: 
//  REXA_StateSet    'a'     'b'
//--------------------------
//    {S}      {A,E}    
//   {A,E}             {A,E}
// corresponding DFA: {S}('a') -> {A,E}. {A,E}('b') -> {A,E}
//-----------------------------------------------------------------------------
void REXA_TranslatingTable::GenerateTranslationTable(set<const REXA_NDFAState*>& rSetState)
//-----------------------------------------------------------------------------
{
    set< REXA_StateSet > setPendingStateSets;
	REXA_StateSet startStateSet;

	startStateSet.insert(m_pStartState);
    setPendingStateSets.insert(startStateSet);

    while( setPendingStateSets.size()>0 ){
        REXA_StateSet curStateSet= *setPendingStateSets.begin();
        setPendingStateSets.erase(curStateSet);
        if( m_mapTransitions.find(curStateSet)==m_mapTransitions.end() ){
            TransitionVector transVector(m_vecCharsets.size());
			for(int i=0;i<(int)m_vecCharsets.size();i++){
				REXA_StateSet::const_iterator it;
				for(it= curStateSet.begin();it!=curStateSet.end();++it){
					for(int j=0;j<(int)(*it)->m_transitions.size();j++){	
						const  REXA_NDFATransition& rTransition= (*it)->m_transitions[j];
						REXA_CharSet charSet= rTransition.m_transChars;
						charSet&= m_vecCharsets[i];
						if( charSet.any() ){
							transVector[i].insert(rTransition.m_nextState);
						}
					}
				}
			}
			m_mapTransitions.insert(make_pair(curStateSet,transVector));
			for(int l=0;l<(int)transVector.size();l++){
				setPendingStateSets.insert(transVector[l]);
			}
        }
    }
}

static REXA_DFAState* GetIllegalState()
{
    static REXA_DFAState illegalState(REXA_DFAState::eNoAnswer,true);
    return &illegalState;
}
static REXA_DFAState* GetEosState()
{
    bool bFirstTime= true;
    static REXA_DFAState eosState(REXA_DFAState::eEos,true);
    if( bFirstTime ){
        bFirstTime= false;
        for(int i=0;i<sizeof(eosState.aNext)/sizeof(eosState.aNext[0]);i++){
            eosState.aNext[i]= &eosState;
        }
    }
    return &eosState;
}

REXA_DFAState* REXA_DFAState::ms_pIllegalState= ::GetIllegalState();
REXA_DFAState* REXA_DFAState::ms_pEosState= ::GetEosState();

//-------------------------------------------------------------------------------
REXA_DFAState::REXA_DFAState(int nAnwserId,bool bIsFinal)
		:m_nAnwserId(nAnwserId),
         m_bIsFinal(bIsFinal),
         m_bIsAccepting(false)	
//-------------------------------------------------------------------------------
{
    for(int i=0;i<256;i++)	aNext[i]=0;
}
//-------------------------------------------------------------------------------
void    REXA_DFAState::CollectStates(REXA_DFAState* pState,set<REXA_DFAState*>& rsetStates)
//-------------------------------------------------------------------------------
{
    if( !pState ) return;

    pair<set<REXA_DFAState*>::iterator,bool> itBool=    rsetStates.insert(pState);
    if( itBool.second ){
        for(int i=0;i<256;i++){
            CollectStates((*itBool.first)->aNext[i],rsetStates);
        }
    }
}

//-----------------------------------------------------------------------------
void    REXA_DFAState::DeleteAll()
//-----------------------------------------------------------------------------
{
    set<REXA_DFAState*> setStates;
    CollectStates(this,setStates);
    set<REXA_DFAState*>::iterator it;
    for(it= setStates.begin();it!=setStates.end();++it){
        if( *it!=ms_pIllegalState && *it!=ms_pEosState ){
            delete *it;
        }
    }
}



//-----------------------------------------------------------------------------
vector<REXA_CharSet> REXA_NDFAToTable::CharsetToVector(const REXA_CharSetSet& rCharSetSet)
//-----------------------------------------------------------------------------
{
	vector<REXA_CharSet> vecCharSet;
	REXA_CharSetSet::const_iterator it;
	for(it= rCharSetSet.begin();it!=rCharSetSet.end();++it){
		vecCharSet.push_back(*it);
	}
	return vecCharSet;
}
//-----------------------------------------------------------------------------
REXA_TranslatingTable::REXA_TranslatingTable    (const REXA_NDFAState* pStartState)
//-----------------------------------------------------------------------------
{
    set<int>                setVisitedStates;
    REXA_CharSetSet              charSets; 
	set<const REXA_NDFAState*>   setStates;
	
	REXA_NDFAState::CollectStates(pStartState,setStates);

	CollectCharSets(pStartState,setVisitedStates,charSets);
	m_vecCharsets=	REXA_NDFAToTable::CharsetToVector(
								ComputeDisjointCharSets(charSets));
    m_pStartState=	pStartState;
	GenerateTranslationTable(setStates);
	ComputeOccCounts();
}


//-----------------------------------------------------------------------------
REXA_NDFAToDfa::REXA_NDFAToDfa	(const REXA_TranslatingTable& rTable,char cEos)
						:m_rTable(rTable),m_cEos((unsigned char)cEos)
//-----------------------------------------------------------------------------
{
	
}
//-----------------------------------------------------------------------------
REXA_DFAState* REXA_NDFAToDfa::CreateDfaStates(
					const REXA_StateSet&				    rStateSet,
					map<REXA_StateSet,REXA_DFAState*>&	rMapStateSetDFAState)
//-----------------------------------------------------------------------------
{  
    pair<map<REXA_StateSet,REXA_DFAState*>::iterator,bool> pairItBool=
			rMapStateSetDFAState.insert(make_pair(rStateSet,(REXA_DFAState*)0));
	if( pairItBool.second ){
		REXA_DFAState* pDFAState= new REXA_DFAState();
		pairItBool.first->second= pDFAState;
		REXA_TranslatingTable::TranslationMap::const_iterator it=
			m_rTable.m_mapTransitions.find(rStateSet);
		if( it!= m_rTable.m_mapTransitions.end() ){
			const TransitionVector& rvecTrans= it->second;
			for(int i=0;i<(int)rvecTrans.size();i++){
				if( rvecTrans[i].size()>0 ){
					REXA_DFAState* pTransState= 
						CreateDfaStates(rvecTrans[i],rMapStateSetDFAState);
					for(int j=0;j<256;j++){
                        if( m_rTable.m_vecCharsets[i][j] ){
                            pDFAState->aNext[j]= pTransState;
                        }
					}
				}
			}
			if( rStateSet.IsAccepting() ){
				int nAnswerCode= m_rTable.GetAnswerCode(rStateSet);
                pDFAState->m_bIsAccepting= true;
                pDFAState->m_nAnwserId= nAnswerCode;
				REXA_DFAState* pAcceptState= 
					new REXA_DFAState(nAnswerCode,true);
                pAcceptState->m_bIsAccepting= true;
				for(int j=0;j<256;j++){
					if( !pDFAState->aNext[j] ){
						pDFAState->aNext[j]= pAcceptState;
					}
				}
			}else{
				for(int j=0;j<256;j++){
					if( !pDFAState->aNext[j] ){
                        pDFAState->aNext[j]= REXA_DFAState::ms_pIllegalState;
					}
				}
                pDFAState->aNext[m_cEos]= REXA_DFAState::ms_pEosState;
			}
		}
        return pDFAState;
    }
	return  pairItBool.first->second;
}
//-----------------------------------------------------------------------------
REXA_DFAState* REXA_NDFAToDfa::TableToDfaStates()
//-----------------------------------------------------------------------------
{ 
	REXA_StateSet startStateSet;
	startStateSet.insert(m_rTable.m_pStartState);

	map<REXA_StateSet,REXA_DFAState*> mapStateSetDFAState;

    REXA_TranslatingTable::TranslationMap::const_iterator it;
	it= m_rTable.m_mapTransitions.find(startStateSet);
	if( it!=m_rTable.m_mapTransitions.end() ){
		return  CreateDfaStates(
					it->first,
					mapStateSetDFAState);
		
    }
    return 0;
}

//-----------------------------------------------------------------------------
REXA_Parser::REXA_Parser     (char cEos)
//-----------------------------------------------------------------------------
{
    m_pParserImp= new REXA_ParserImpl(cEos);

}
//-----------------------------------------------------------------------------
REXA_Parser::~REXA_Parser    ()
//-----------------------------------------------------------------------------
{
    delete m_pParserImp;
}
//-----------------------------------------------------------------------------
REXA_DFAState*      REXA_Parser::ParseRegExp     (string strRegExp)
//-----------------------------------------------------------------------------
{
    const REXA_NDFAState*    pNdfa=  m_pParserImp->ParseRegExp(strRegExp);
	REXA_TranslatingTable    table(pNdfa);
	REXA_NDFAToDfa           ndfaToDfa(table,m_pParserImp->m_cEos);
    return                   ndfaToDfa.TableToDfaStates();
}
//-----------------------------------------------------------------------------
void                REXA_Parser::AddRegDefinition (   string strDefName,
                                                string strDefExpression,
                                                int nAnswer)
//-----------------------------------------------------------------------------
{
    string str= strDefName+ "=" + strDefExpression;
    m_pParserImp->ParseRegExpDefinition(str,nAnswer);
}
//-----------------------------------------------------------------------------
void                REXA_Parser::RemoveRegDefinition (   string strDefName)
//-----------------------------------------------------------------------------
{
    m_pParserImp->m_mapNamedStates.erase(strDefName);
}
//-----------------------------------------------------------------------------
void                REXA_Parser::GetRegDefinitions   (vector<string>& vecDefinitions)
//-----------------------------------------------------------------------------
{
    map<string,REXA_NDFAState*>::iterator it;
    for(it= m_pParserImp->m_mapNamedStates.begin();
        it!=m_pParserImp->m_mapNamedStates.end();
        ++it){
        vecDefinitions.push_back(it->first);
    }
}



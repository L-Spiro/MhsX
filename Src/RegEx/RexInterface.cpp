#include "RexInterface.h"


REXI_Base::REXI_Base(char cEos)
:m_cEos(cEos),m_pszSource(""),m_pszTokenBeg(""),m_pszTokenEnd("")
,m_pStartState(REXA_DFAState::ms_pEosState)
{
}
REXI_Base::~REXI_Base()
{
    FreeMemory();
}
REXI_DefErr   REXI_Base::AddRegDefinition    (string strName,string strRegExp,
                                            int nIdAnswer)
{
    REXI_DefErr defErr;
    defErr.eErrCode=REXI_DefErr::eNoErr;
    defErr.strErrMsg="";
    defErr.nErrOffset=0;
    try{
        m_regexpParser.AddRegDefinition(strName,strRegExp,nIdAnswer);
    }catch(REXA_Exception e){
        defErr.strErrMsg= e.m_strErrMsg;
        if( e.m_nErrPos>(int)strName.size()+1){
            defErr.eErrCode= REXI_DefErr::eErrInRegExp;
            defErr.nErrOffset= (int)(e.m_nErrPos-(strName.size()+1));
        }else{
            defErr.eErrCode=REXI_DefErr::eErrInName;
            defErr.nErrOffset= e.m_nErrPos;
        }
    }
    return defErr;
}
void                REXI_Base::GetRegDefinitions   (vector<string>& vecDefinitions)
{
    m_regexpParser.GetRegDefinitions(vecDefinitions);
}
void                REXI_Base::RemoveRegDefinition (string strName)
{
    m_regexpParser.RemoveRegDefinition(strName);
}


REXI_DefErr   REXI_Base::SetRegexp    (string strRegExp)
{
    REXI_DefErr defErr;
    defErr.eErrCode=REXI_DefErr::eNoErr;
    defErr.strErrMsg="";
    defErr.nErrOffset=0;
    try{
        REXA_DFAState* pStartState= m_regexpParser.ParseRegExp(strRegExp);
        FreeMemory();
        m_pStartState= pStartState;
    }catch(REXA_Exception e){
        defErr.eErrCode=REXI_DefErr::eErrInRegExp;
        defErr.strErrMsg= e.m_strErrMsg;
        defErr.nErrOffset= e.m_nErrPos;
    }
    return defErr;
}	


REXI_Search::REXI_Search(char cEos)
:REXI_Base(cEos),m_nIdAnswer(0)
{
}
REXI_DefErr  REXI_Search::AddRegDef(string strName,string strRegExp)
{
     return REXI_Base::AddRegDefinition(strName,strRegExp,m_nIdAnswer++);
}
bool REXI_Search::MatchHereImpl()
{
    m_pszTokenBeg= m_pszTokenEnd;

    const REXA_DFAState* pState= m_pStartState;
    const char* pAccept=0;
	while( !(pState= pState->
                        aNext[*(unsigned char*)m_pszTokenEnd++])->m_bIsFinal){
        if(pState->m_bIsAccepting){
            pAccept= m_pszTokenEnd;
        }
	}
	m_pszTokenEnd--;
    if(!pState->m_bIsAccepting  ){
        if( pAccept ){
            m_pszTokenEnd= pAccept;
            return true;
        }
        return false;
    }
    return true;
}
bool REXI_Search::MatchHere(const char*& rpcszSrc, int& nMatchLen,bool& bEos)
{
    SetSource(rpcszSrc);
    bool bFound= MatchHereImpl();
    nMatchLen= (int)(m_pszTokenEnd-m_pszTokenBeg);
    rpcszSrc= m_pszTokenEnd;
    bEos= (*m_pszTokenEnd==m_cEos);
    rpcszSrc= m_pszTokenEnd;
    return bFound;
}

bool REXI_Search::Find(const char*& rpcszSrc, int& nMatchLen,bool& bEos)
{
    bool bFound;
    SetSource(rpcszSrc);
    for(;;){
        while(m_pStartState->aNext[*(unsigned char*)m_pszTokenEnd]==
                                                REXA_DFAState::ms_pIllegalState ){
            m_pszTokenEnd++;
        }
        if( MatchHereImpl() ){
            bFound= true;
            break;
        }
        bEos= (*m_pszTokenEnd==m_cEos);
        if( bEos ){
            bFound= false;
            break;
        }
        m_pszTokenEnd= m_pszTokenBeg+1;
    }
    bEos= (*m_pszTokenEnd==m_cEos);
    nMatchLen= (int)(m_pszTokenEnd-m_pszTokenBeg);
    rpcszSrc= m_pszTokenEnd;
	return bFound;
}
#ifndef REX_INTERFACE_INCLUDED_
#define REX_INTERFACE_INCLUDED_

#include "RexAlgorithm.h"

struct REXI_DefErr{
        enum{eNoErr,eErrInName,eErrInRegExp} eErrCode;
        string  strErrMsg;
        int     nErrOffset;
};

class REXI_Base{
public:

	REXI_Base(char cEos='\0');
    virtual ~REXI_Base()=0;

    inline void			SetSource			(const char* pszSource);
    inline const char*  GetSource    ()const;

    
    REXI_DefErr          AddRegDefinition    (string strName,string strRegExp,
                                            int nIdAnswer);
    void            GetRegDefinitions   (vector<string>& vecDefinitions);
    void            RemoveRegDefinition (string strName);

	REXI_DefErr			SetRegexp			(string strRegExp);


protected:

    inline void FreeMemory    ();          

//source,source position and end of string character
	char				m_cEos;
	const char*			m_pszSource;
    const char*         m_pszTokenBeg;
    const char*         m_pszTokenEnd;

//algorithmic objects
	REXA_Parser		    m_regexpParser;

	REXA_DFAState*      m_pStartState;

};

class REXI_Search : public REXI_Base
{ 
public:
    REXI_Search(char cEos='\0');

    REXI_DefErr
            AddRegDef   (string strName,string strRegExp);
    inline  REXI_DefErr  
            SetRegexp	(string strRegExp);
    bool    MatchHere   (const char*& rpcszSrc, int& nMatchLen,bool& bEos);
    bool    Find        (const char*& rpcszSrc, int& nMatchLen,bool& bEos);
private:
    bool    MatchHereImpl();
    int     m_nIdAnswer;
};

inline const char*     REXI_Base::GetSource()const  {return m_pszSource;}
inline void			   REXI_Base::SetSource(const char* pszSource)
{
    m_pszSource= pszSource;m_pszTokenBeg= m_pszTokenEnd=pszSource;
}
inline REXI_DefErr	        REXI_Search::SetRegexp	(string strRegExp)
{
    return REXI_Base::SetRegexp(strRegExp);
}
inline void             REXI_Base::FreeMemory()     {m_pStartState->DeleteAll();}
#endif

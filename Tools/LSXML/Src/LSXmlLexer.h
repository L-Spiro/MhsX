#pragma once

#include "LSXml.h"
#include "LSXmlContainer.h"

#ifdef yyFlexLexer
#undef yyFlexLexer
#endif
#define yyFlexLexer			CXmlLexBase

#include "Gen/LSXmlLexBase.h"
#include "Gen/LSXmlParser.h"


namespace lsx {

	class CXmlLexer : public CXmlLexBase {
	public :
		// == Various constructors.
		CXmlLexer( std::istream * _pisStream, std::ostream * _posStream = nullptr ) :
			Parent( _pisStream, _posStream ),
			m_pxlContainer( nullptr ) {
		}
		virtual ~CXmlLexer() {
		}


	protected :
		// == Members.
		// The container that handles collection of all of the script data.
		const CXmlContainer *							m_pxlContainer;


	private :
		typedef CXmlLexBase								Parent;
	};

}	// namespace lsx

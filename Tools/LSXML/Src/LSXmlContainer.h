#pragma once

#include "LSXml.h"
#include "LSXmlSyntaxNodes.h"
#include "Gen/LSXmlParser.h"


namespace lsx {

	class CXmlLexer;

	class CXmlContainer {
	public :
		CXmlContainer( CXmlLexer * _pxlLexer );



	protected :
		// == Members.
		/** The lexer object. */
		CXmlLexer *										m_pxlLexer;
	};

}	// namespace lsx

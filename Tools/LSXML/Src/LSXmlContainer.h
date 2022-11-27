#pragma once

#include "LSXml.h"
#include "LSXmlSyntaxNodes.h"
#include "Gen/LSXmlParser.h"


namespace lsx {

	class CXmlLexer;

	/**
	 * Class CXmlContainer
	 * \brief Processes the results of the parsing and stores the result.
	 *
	 * Description: Processes the results of the parsing and stores the result.
	 */
	class CXmlContainer {
	public :
		CXmlContainer( CXmlLexer * _pxlLexer );



	protected :
		// == Members.
		/** The lexer object. */
		CXmlLexer *										m_pxlLexer;
	};

}	// namespace lsx

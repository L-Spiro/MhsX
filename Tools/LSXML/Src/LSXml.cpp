#include "LSXml.h"
#include "LSXmlContainer.h"
#include "LSXmlLexer.h"
#include "Gen/LSXmlParser.h"	// Must come after container/lexer.
#include <sstream>


namespace lsx {

	CXml::CXml() :
		m_pxcContainer( nullptr ) {
	}
	CXml::~CXml() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the object back to its defult useable state.
	 */
	void CXml::Reset() {
		delete m_pxcContainer;
		m_pxcContainer = nullptr;
	}

	/**
	 * Sets the loaded XML data.
	 *
	 * \param _pcXml The NULL-terminated XML data.
	 * \return Returns true if the XML data was successfully parsed.
	 */
	bool CXml::SetXml( const char * _pcXml ) {
		Reset();
		if ( !_pcXml ) { return false; }

		std::istringstream isStream( _pcXml );

		// Streams created.  Create the parsers.
		std::unique_ptr<CXmlLexer> ppplLexer = std::make_unique<CXmlLexer>( &isStream, nullptr );
		if ( !ppplLexer ) {
			return false;
		}

		m_pxcContainer = new( std::nothrow ) CXmlContainer( ppplLexer.get() );

		std::unique_ptr<CXmlParser> ppppParser = std::make_unique<CXmlParser>( ppplLexer.get(), m_pxcContainer );
		if ( !ppppParser ) {
			Reset();
			return false;
		}


		if ( ppppParser->parse() == 0 ) {
			//m_pxcContainer->PrintNode( m_pxcContainer->Root(), 0 );
			m_pxcContainer->BuidTree();
			m_pxcContainer->PrintTree();
			// Parsed.
			return true;
		}
		Reset();
		return false;
	}

}	// namespace lsx

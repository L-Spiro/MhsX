#include "LSONJson.h"
#include "LSONJsonContainer.h"
#include "LSONJsonLexer.h"
#include "Gen/LSONJsonParser.h"	// Must come after container/lexer.
#include <sstream>


namespace lson {

	CJson::CJson() :
		m_pxcContainer( nullptr ) {
	}
	CJson::~CJson() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the object back to its defult useable state.
	 */
	void CJson::Reset() {
		delete m_pxcContainer;
		m_pxcContainer = nullptr;
	}

	/**
	 * Sets the loaded XML data.
	 *
	 * \param _pcXml The NULL-terminated XML data.
	 * \return Returns true if the XML data was successfully parsed.
	 */
	bool CJson::SetXml( const char * _pcXml ) {
		Reset();
		if ( !_pcXml ) { return false; }

		std::istringstream isStream( _pcXml );

		// Streams created.  Create the parsers.
		std::unique_ptr<CJsonLexer> ppplLexer = std::make_unique<CJsonLexer>( &isStream, nullptr );
		if ( !ppplLexer ) {
			return false;
		}

		m_pxcContainer = new( std::nothrow ) CJsonContainer( ppplLexer.get() );

		std::unique_ptr<CJsonParser> ppppParser = std::make_unique<CJsonParser>( ppplLexer.get(), m_pxcContainer );
		if ( !ppppParser ) {
			Reset();
			return false;
		}


		if ( ppppParser->parse() == 0 ) {
			//m_pxcContainer->PrintNode( m_pxcContainer->Root(), 0 );
			m_pxcContainer->BuidTree();
			//m_pxcContainer->PrintTree();
			// Parsed.
			return true;
		}
		Reset();
		return false;
	}

}	// namespace lson

#include "LSONJson.h"
#include "LSONJsonContainer.h"
#include "LSONJsonLexer.h"
#include "Gen/LSONJsonParser.h"	// Must come after container/lexer.
#include <sstream>


namespace lson {

	CJson::CJson() :
		m_pjcContainer( nullptr ) {
	}
	CJson::~CJson() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the object back to its defult useable state.
	 */
	void CJson::Reset() {
		delete m_pjcContainer;
		m_pjcContainer = nullptr;
	}

	/**
	 * Sets the loaded JSON data.
	 *
	 * \param _pcJson The NULL-terminated JSON data.
	 * \return Returns true if the JSON data was successfully parsed.
	 */
	bool CJson::SetJson( const char * _pcJson ) {
		Reset();
		if ( !_pcJson ) { return false; }

		std::istringstream isStream( _pcJson );

		// Streams created.  Create the parsers.
		std::unique_ptr<CJsonLexer> pjlLexer = std::make_unique<CJsonLexer>( &isStream, nullptr );
		if ( !pjlLexer ) {
			return false;
		}

		m_pjcContainer = new( std::nothrow ) CJsonContainer( pjlLexer.get() );

		std::unique_ptr<CJsonParser> ppppParser = std::make_unique<CJsonParser>( pjlLexer.get(), m_pjcContainer );
		if ( !ppppParser ) {
			Reset();
			return false;
		}


		if ( ppppParser->parse() == 0 ) {
			//m_pjcContainer->PrintNode( m_pjcContainer->Root(), 0 );
			m_pjcContainer->BuildTree();
			//m_pjcContainer->PrintTree();
			// Parsed.
			return true;
		}
		Reset();
		return false;
	}

}	// namespace lson

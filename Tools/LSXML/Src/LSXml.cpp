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
	bool CXml::SetXml( char * _pcXml ) {
		Reset();
		if ( !_pcXml ) { return false; }

		std::istringstream isStream( _pcXml );

		// Streams created.  Create the parsers.
		std::unique_ptr<CXmlLexer> pxlLexer = std::make_unique<CXmlLexer>( &isStream, nullptr );
		if ( !pxlLexer ) {
			return false;
		}
		struct yy_buffer_state * pbState = pxlLexer->ScanMemoryBuffer( const_cast<char *>(_pcXml), std::strlen( _pcXml ) + 2 );
		if ( !pbState ) {
			pxlLexer->yy_delete_buffer( pbState );
			return false;
		}

		m_pxcContainer = new( std::nothrow ) CXmlContainer( pxlLexer.get() );

		std::unique_ptr<CXmlParser> ppppParser = std::make_unique<CXmlParser>( pxlLexer.get(), m_pxcContainer );
		if ( !ppppParser ) {
			pxlLexer->yy_delete_buffer( pbState );
			Reset();
			return false;
		}


		if ( ppppParser->parse() == 0 ) {
			pxlLexer->yy_delete_buffer( pbState );
			m_pxcContainer->BuildTree();
			//m_pxcContainer->PrintTree();
			// Parsed.
			return true;
		}
		pxlLexer->yy_delete_buffer( pbState );
		Reset();
		return false;
	}

}	// namespace lsx

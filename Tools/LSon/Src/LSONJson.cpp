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

	/**
	 * Creates an object JSON element.
	 * 
	 * \param _sName The name of the element.
	 * \param _eElement The return structure.
	 **/
	void CJson::CreateObjectElement( const std::string &_sName, LSON_ELEMENT &_eElement ) {
		_eElement.ntType = LSON_OBJECT;
		_eElement.sName = _sName;
		_eElement.vObjectMembers.clear();
	}

	/**
	 * Creates an array JSON element.
	 * 
	 * \param _sName The name of the element.
	 * \param _eElement The return structure.
	 **/
	void CJson::CreateArrayElement( const std::string &_sName, LSON_ELEMENT &_eElement ) {
		_eElement.ntType = LSON_ARRAY;
		_eElement.sName = _sName;
		_eElement.vArray.clear();
	}

	/**
	 * Creates a string JSON element.
	 * 
	 * \param _sName The name of the element.
	 * \param _sString The string.
	 * \param _eElement The return structure.
	 **/
	void CJson::CreateStringElement( const std::string &_sName, const std::string &_sString, LSON_ELEMENT &_eElement ) {
		_eElement.ntType = LSON_STRING;
		_eElement.sName = _sName;
		_eElement.sString = _sString;
	}

	/**
	 * Creates a number JSON element.
	 * 
	 * \param _sName The name of the element.
	 * \param _dNumber The number.
	 * \param _eElement The return structure.
	 **/
	void CJson::CreateNumberElement( const std::string &_sName, double _dNumber, LSON_ELEMENT &_eElement ) {
		_eElement.ntType = LSON_STRING;
		_eElement.sName = _sName;
		_eElement.dNumber = _dNumber;
	}

	/**
	 * Creates a true JSON element.
	 * 
	 * \param _sName The name of the element.
	 * \param _eElement The return structure.
	 **/
	void CJson::CreateTrueElement( const std::string &_sName, LSON_ELEMENT &_eElement ) {
		_eElement.ntType = LSON_TRUE;
		_eElement.sName = _sName;
	}

	/**
	 * Creates a false JSON element.
	 * 
	 * \param _sName The name of the element.
	 * \param _eElement The return structure.
	 **/
	void CJson::CreateFalseElement( const std::string &_sName, LSON_ELEMENT &_eElement ) {
		_eElement.ntType = LSON_FALSE;
		_eElement.sName = _sName;
	}

	/**
	 * Creates a null JSON element.
	 * 
	 * \param _sName The name of the element.
	 * \param _eElement The return structure.
	 **/
	void CJson::CreateNullElement( const std::string &_sName, LSON_ELEMENT &_eElement ) {
		_eElement.ntType = LSON_NULL;
		_eElement.sName = _sName;
	}

}	// namespace lson

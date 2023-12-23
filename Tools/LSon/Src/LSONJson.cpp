#include "LSONJson.h"
#include "LSONJsonContainer.h"
#include "LSONJsonLexer.h"
#include "Gen/LSONJsonParser.h"	// Must come after container/lexer.
#include <format>
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
		_eElement.vObjectMembers.clear();
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
		_eElement.ntType = LSON_NUMBER;
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

	/**
	 * Writes an element JSON element to a byte buffer.
	 * 
	 * \param _eElement The element to write.
	 * \param _vBuffer The buffer to which to append the element.
	 * \param _iIndent The indent level.
	 * \param _bValueOnly Set to true inside arrays.
	 * \return Returns true if the element was appended to the end of the given data buffer.
	 **/
	bool CJson::WriteElement( const LSON_ELEMENT &_eElement, std::vector<uint8_t> &_vBuffer, int32_t _iIndent, bool _bValueOnly ) {
		switch ( _eElement.ntType ) {
			case LSON_OBJECT : {
				return WriteObjectElement( _eElement, _vBuffer, _iIndent, _bValueOnly );
			}
			case LSON_ARRAY : {
				return WriteArrayElement( _eElement, _vBuffer, _iIndent, _bValueOnly );
			}
			case LSON_STRING : {
				return WriteStringElement( _eElement, _vBuffer, _iIndent, _bValueOnly );
			}
			case LSON_NUMBER : {
				return WriteNumberElement( _eElement, _vBuffer, _iIndent, _bValueOnly );
			}
			case LSON_TRUE : {
				return WriteTrueElement( _eElement, _vBuffer, _iIndent, _bValueOnly );
			}
			case LSON_FALSE : {
				return WriteFalseElement( _eElement, _vBuffer, _iIndent, _bValueOnly );
			}
			case LSON_NULL : {
				return WriteNullElement( _eElement, _vBuffer, _iIndent, _bValueOnly );
			}
			default : { return false; }
		}
	}

	/**
	 * Writes an object JSON element to a byte buffer.
	 * 
	 * \param _eElement The element to write.
	 * \param _vBuffer The buffer to which to append the element.
	 * \param _iIndent The indent level.
	 * \param _bValueOnly Set to true inside arrays.
	 * \return Returns true if the element was appended to the end of the given data buffer.
	 **/
	bool CJson::WriteObjectElement( const LSON_ELEMENT &_eElement, std::vector<uint8_t> &_vBuffer, int32_t _iIndent, bool _bValueOnly ) {
		try {
			if ( !WriteIndent( _vBuffer, _iIndent ) ) { return false; }
			if ( !_bValueOnly ) {
				if ( _eElement.sName.size() ) {
					if ( !WriteString( _eElement.sName, _vBuffer ) ) { return false; }
					_vBuffer.push_back( 0x20 );
					_vBuffer.push_back( ':' );
					_vBuffer.push_back( 0x20 );
				}
			}

			_vBuffer.push_back( '{' );
			_vBuffer.push_back( '\r' );
			_vBuffer.push_back( '\n' );

			for ( size_t I = 0; I < _eElement.vObjectMembers.size(); ++I ) {
				if ( I != 0 ) {
					_vBuffer.push_back( ',' );
					_vBuffer.push_back( '\r' );
					_vBuffer.push_back( '\n' );
				}
				if ( !WriteElement( (*_eElement.vObjectMembers[I]), _vBuffer, _iIndent + 1 ) ) { return false; }
			}
			/*if ( _eElement.vObjectMembers.size() == 0 ) {
				if ( !WriteIndent( _vBuffer, _iIndent ) ) { return false; }
			}*/
			_vBuffer.push_back( '\r' );
			_vBuffer.push_back( '\n' );
			if ( !WriteIndent( _vBuffer, _iIndent ) ) { return false; }
			_vBuffer.push_back( '}' );

			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Writes an array JSON element to a byte buffer.
	 * 
	 * \param _eElement The element to write.
	 * \param _vBuffer The buffer to which to append the element.
	 * \param _iIndent The indent level.
	 * \param _bValueOnly Set to true inside arrays.
	 * \return Returns true if the element was appended to the end of the given data buffer.
	 **/
	bool CJson::WriteArrayElement( const LSON_ELEMENT &_eElement, std::vector<uint8_t> &_vBuffer, int32_t _iIndent, bool _bValueOnly ) {
		try {
			if ( !WriteIndent( _vBuffer, _iIndent ) ) { return false; }
			if ( !_bValueOnly ) {
				if ( !WriteString( _eElement.sName, _vBuffer ) ) { return false; }
				_vBuffer.push_back( 0x20 );
				_vBuffer.push_back( ':' );
				_vBuffer.push_back( 0x20 );
			}

			_vBuffer.push_back( '[' );
			_vBuffer.push_back( '\r' );
			_vBuffer.push_back( '\n' );

			for ( size_t I = 0; I < _eElement.vObjectMembers.size(); ++I ) {
				if ( I != 0 ) {
					_vBuffer.push_back( ',' );
					_vBuffer.push_back( '\r' );
					_vBuffer.push_back( '\n' );
				}
				if ( !WriteElement( (*_eElement.vObjectMembers[I]), _vBuffer, _iIndent + 1, true ) ) { return false; }
			}
			/*if ( _eElement.vObjectMembers.size() == 0 ) {
				if ( !WriteIndent( _vBuffer, _iIndent ) ) { return false; }
			}*/
			_vBuffer.push_back( '\r' );
			_vBuffer.push_back( '\n' );
			if ( !WriteIndent( _vBuffer, _iIndent ) ) { return false; }
			_vBuffer.push_back( ']' );

			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Writes a string JSON element to a byte buffer.
	 * 
	 * \param _eElement The element to write.
	 * \param _vBuffer The buffer to which to append the element.
	 * \param _iIndent The indent level.
	 * \param _bValueOnly Set to true inside arrays.
	 * \return Returns true if the element was appended to the end of the given data buffer.
	 **/
	bool CJson::WriteStringElement( const LSON_ELEMENT &_eElement, std::vector<uint8_t> &_vBuffer, int32_t _iIndent, bool _bValueOnly ) {
		try {
			if ( !WriteIndent( _vBuffer, _iIndent ) ) { return false; }
			if ( !_bValueOnly ) {
				if ( !WriteString( _eElement.sName, _vBuffer ) ) { return false; }
				_vBuffer.push_back( 0x20 );
				_vBuffer.push_back( ':' );
				_vBuffer.push_back( 0x20 );
			}

			if ( !WriteString( _eElement.sString, _vBuffer ) ) { return false; }

			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Writes a number JSON element to a byte buffer.
	 * 
	 * \param _eElement The element to write.
	 * \param _vBuffer The buffer to which to append the element.
	 * \param _iIndent The indent level.
	 * \param _bValueOnly Set to true inside arrays.
	 * \return Returns true if the element was appended to the end of the given data buffer.
	 **/
	bool CJson::WriteNumberElement( const LSON_ELEMENT &_eElement, std::vector<uint8_t> &_vBuffer, int32_t _iIndent, bool _bValueOnly ) {
		try {
			if ( !WriteIndent( _vBuffer, _iIndent ) ) { return false; }
			if ( !_bValueOnly ) {
				if ( !WriteString( _eElement.sName, _vBuffer ) ) { return false; }
				_vBuffer.push_back( 0x20 );
				_vBuffer.push_back( ':' );
				_vBuffer.push_back( 0x20 );
			}

			std::string sTmp = std::format( "{:.2000}", _eElement.dNumber );
			for ( size_t I = 0; I < sTmp.size(); ++I ) {
				_vBuffer.push_back( sTmp[I] );
			}

			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Writes a true JSON element to a byte buffer.
	 * 
	 * \param _eElement The element to write.
	 * \param _vBuffer The buffer to which to append the element.
	 * \param _iIndent The indent level.
	 * \param _bValueOnly Set to true inside arrays.
	 * \return Returns true if the element was appended to the end of the given data buffer.
	 **/
	bool CJson::WriteTrueElement( const LSON_ELEMENT &_eElement, std::vector<uint8_t> &_vBuffer, int32_t _iIndent, bool _bValueOnly ) {
		try {
			if ( !WriteIndent( _vBuffer, _iIndent ) ) { return false; }
			if ( !_bValueOnly ) {
				if ( !WriteString( _eElement.sName, _vBuffer ) ) { return false; }
				_vBuffer.push_back( 0x20 );
				_vBuffer.push_back( ':' );
				_vBuffer.push_back( 0x20 );
			}

			_vBuffer.push_back( 't' );
			_vBuffer.push_back( 'r' );
			_vBuffer.push_back( 'u' );
			_vBuffer.push_back( 'e' );

			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Writes a false JSON element to a byte buffer.
	 * 
	 * \param _eElement The element to write.
	 * \param _vBuffer The buffer to which to append the element.
	 * \param _iIndent The indent level.
	 * \param _bValueOnly Set to true inside arrays.
	 * \return Returns true if the element was appended to the end of the given data buffer.
	 **/
	bool CJson::WriteFalseElement( const LSON_ELEMENT &_eElement, std::vector<uint8_t> &_vBuffer, int32_t _iIndent, bool _bValueOnly ) {
		try {
			if ( !WriteIndent( _vBuffer, _iIndent ) ) { return false; }
			if ( !_bValueOnly ) {
				if ( !WriteString( _eElement.sName, _vBuffer ) ) { return false; }
				_vBuffer.push_back( 0x20 );
				_vBuffer.push_back( ':' );
				_vBuffer.push_back( 0x20 );
			}

			_vBuffer.push_back( 'f' );
			_vBuffer.push_back( 'a' );
			_vBuffer.push_back( 'l' );
			_vBuffer.push_back( 's' );
			_vBuffer.push_back( 'e' );

			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Writes a null JSON element to a byte buffer.
	 * 
	 * \param _eElement The element to write.
	 * \param _vBuffer The buffer to which to append the element.
	 * \param _iIndent The indent level.
	 * \param _bValueOnly Set to true inside arrays.
	 * \return Returns true if the element was appended to the end of the given data buffer.
	 **/
	bool CJson::WriteNullElement( const LSON_ELEMENT &_eElement, std::vector<uint8_t> &_vBuffer, int32_t _iIndent, bool _bValueOnly ) {
		try {
			if ( !WriteIndent( _vBuffer, _iIndent ) ) { return false; }
			if ( !_bValueOnly ) {
				if ( !WriteString( _eElement.sName, _vBuffer ) ) { return false; }
				_vBuffer.push_back( 0x20 );
				_vBuffer.push_back( ':' );
				_vBuffer.push_back( 0x20 );
			}

			_vBuffer.push_back( 'n' );
			_vBuffer.push_back( 'u' );
			_vBuffer.push_back( 'l' );
			_vBuffer.push_back( 'l' );

			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Writes an indentation level to a buffer.
	 * 
	 * \param _vBuffer The buffer to which to append the indentation.
	 * \param _iIndent The indent level.
	 * \return Returns true if the indentation was appended to the end of the given data buffer.
	 **/
	bool CJson::WriteIndent( std::vector<uint8_t> &_vBuffer, int32_t _iIndent ) {
		try {
			for ( int32_t I = _iIndent; --I >= 0; ) {
				_vBuffer.push_back( 0x20 );
				_vBuffer.push_back( 0x20 );
				_vBuffer.push_back( 0x20 );
				_vBuffer.push_back( 0x20 );
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Writes a string to the buffer, enclosed in quotations.
	 * 
	 * \param _sString The string to write.
	 * \param _vBuffer The buffer to which to append the string.
	 * \return Returns true if the string was appended to the end of the given data buffer.
	 **/
	bool CJson::WriteString( const std::string &_sString, std::vector<uint8_t> &_vBuffer ) {
		try {
			_vBuffer.push_back( '"' );
			for ( size_t I = 0; I < _sString.size(); ++I ) {
				_vBuffer.push_back( _sString[I] );
			}
			_vBuffer.push_back( '"' );
			return true;
		}
		catch ( ... ) { return false; }
	}

}	// namespace lson

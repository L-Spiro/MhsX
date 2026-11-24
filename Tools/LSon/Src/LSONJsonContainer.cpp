#include "LSONJsonContainer.h"
#include "LSONJsonLexer.h"

#include <charconv>
#include <cstdlib>
#include <version>


namespace lson {

	CJsonContainer::CJsonContainer( CJsonLexer * _pxlLexer ) :
		m_pxlLexer( _pxlLexer ),
		m_stMemberListDepth( 0 ),
		m_stValueListDepth( 0 ),
		m_stRoot( size_t( -1 ) ) {
	}

	// == Functions.
	/**
	 * Reserves room for the results of parsing a JSON file of a given size.
	 *
	 * \param _sBytes The size of the JSON text, in bytes.
	 */
	void CJsonContainer::Reserve( size_t _sBytes ) {
		try {
			m_vValues.reserve( _sBytes / 32 + 16 );
			m_vStrings.reserve( _sBytes / 64 + 16 );
			m_mStringIndex.reserve( _sBytes / 64 + 16 );
		}
		catch ( ... ) {}
	}

	/**
	 * Adds a string and returns its index into the stack.
	 *
	 * \param _svText The string to add or whose existing index is to be found.
	 * \return Returns the index of the added string.
	 */
	size_t CJsonContainer::AddString( std::string_view _svText ) {
		// One lookup either finds the string or adds it.
		auto aIt = m_mStringIndex.try_emplace( _svText, m_vStrings.size() );
		if ( aIt.second ) {
			m_vStrings.push_back( _svText );
		}
		return aIt.first->second;
	}

	/**
	 * Removes quotes from the front and end before adding the string.
	 *
	 * \param _svText The string to add or whose existing index is to be found.
	 * \return Returns the index of the added string after stripping the enclosing quotes from it.
	 */
	size_t CJsonContainer::AddQuoteString( std::string_view _svText ) {
		return AddString( _svText.substr( 1, _svText.size() - 2 ) );
	}

	/**
	 * Returns the index of a string or -1 if it does not exist.
	 *
	 * \param _svText The string to find.
	 * \return Returns the index of the string if it exists or size_t( -1 ).
	 */
	size_t CJsonContainer::FindString( std::string_view _svText ) const {
		auto aIdx = m_mStringIndex.find( _svText );
		if ( aIdx != m_mStringIndex.end() ) { return aIdx->second; }
		return size_t( -1 );
	}

	/**
	 * Creates a string value.
	 *
	 * \param _stString The index of the string.
	 * \return Returns the index of the value created.
	 */
	size_t CJsonContainer::AddStringValue( size_t _stString ) {
		size_t stIdx = AddValue( LSON_VT_STRING );
		m_vValues[stIdx].u.stString = _stString;
#ifdef _DEBUG
		m_vValues[stIdx].svString = GetString( _stString );
#endif	// #ifdef _DEBUG
		return stIdx;
	}

	/**
	 * Creates a decimal value.
	 *
	 * \param _dValue The value.
	 * \return Returns the index of the value created.
	 */
	size_t CJsonContainer::AddDecimalValue( double _dValue ) {
		size_t stIdx = AddValue( LSON_VT_DECIMAL );
		m_vValues[stIdx].u.dDecimal = _dValue;
		return stIdx;
	}

	/**
	 * Creates a TRUE value.
	 *
	 * \return Returns the index of the value created.
	 */
	size_t CJsonContainer::AddTrueValue() {
		return AddValue( LSON_VT_TRUE );
	}

	/**
	 * Creates a FALSE value.
	 *
	 * \return Returns the index of the value created.
	 */
	size_t CJsonContainer::AddFalseValue() {
		return AddValue( LSON_VT_FALSE );
	}

	/**
	 * Creates a NULL value.
	 *
	 * \return Returns the index of the value created.
	 */
	size_t CJsonContainer::AddNullValue() {
		return AddValue( LSON_VT_NULL );
	}

	/**
	 * Creates an object value with no members.
	 *
	 * \return Returns the index of the value created.
	 */
	size_t CJsonContainer::AddObject() {
		return AddValue( LSON_VT_OBJECT );
	}

	/**
	 * Starts the member list of an object with its first member.  Member lists nest (an object inside a member's value is finished
	 *	before the next member of its parent is added), so they are kept on a stack.
	 *
	 * \param _mMember The first member.
	 */
	void CJsonContainer::BeginMembers( const YYSTYPE::LSON_MEMBER &_mMember ) {
		if ( m_stMemberListDepth == m_vMemberLists.size() ) {
			m_vMemberLists.emplace_back();
		}
		// The list keeps its capacity from previous objects at this depth.
		m_vMemberLists[m_stMemberListDepth++].clear();
		AddMember( _mMember );
	}

	/**
	 * Adds a member to the current member list.
	 *
	 * \param _mMember The member to add.
	 */
	void CJsonContainer::AddMember( const YYSTYPE::LSON_MEMBER &_mMember ) {
		LSON_JSON_MEMBER jmMember;
		jmMember.stName = _mMember.stName;
		jmMember.stValue = _mMember.stValue;
#ifdef _DEBUG
		jmMember.svName = GetString( jmMember.stName );
#endif	// #ifdef _DEBUG
		m_vMemberLists[m_stMemberListDepth-1].push_back( jmMember );
	}

	/**
	 * Creates an object value from the current member list and ends the list.
	 *
	 * \return Returns the index of the value created.
	 */
	size_t CJsonContainer::EndObject() {
		size_t stIdx = AddValue( LSON_VT_OBJECT );
		const std::vector<LSON_JSON_MEMBER> & vList = m_vMemberLists[--m_stMemberListDepth];
		// Copied (not moved) so that the list keeps its capacity and the object gets an exact fit.
		m_vValues[stIdx].oObject.vMembers.assign( vList.begin(), vList.end() );
		return stIdx;
	}

	/**
	 * Creates an array value with no values.
	 *
	 * \return Returns the index of the value created.
	 */
	size_t CJsonContainer::AddArray() {
		return AddValue( LSON_VT_ARRAY );
	}

	/**
	 * Starts the value list of an array with its first value.  Value lists nest, so they are kept on a stack.
	 *
	 * \param _stValue The index of the first value.
	 */
	void CJsonContainer::BeginValues( size_t _stValue ) {
		if ( m_stValueListDepth == m_vValueLists.size() ) {
			m_vValueLists.emplace_back();
		}
		// The list keeps its capacity from previous arrays at this depth.
		m_vValueLists[m_stValueListDepth++].clear();
		AddArrayValue( _stValue );
	}

	/**
	 * Adds a value to the current value list.
	 *
	 * \param _stValue The index of the value to add.
	 */
	void CJsonContainer::AddArrayValue( size_t _stValue ) {
		m_vValueLists[m_stValueListDepth-1].push_back( _stValue );
	}

	/**
	 * Creates an array value from the current value list and ends the list.
	 *
	 * \return Returns the index of the value created.
	 */
	size_t CJsonContainer::EndArray() {
		size_t stIdx = AddValue( LSON_VT_ARRAY );
		const std::vector<size_t> & vList = m_vValueLists[--m_stValueListDepth];
		// Copied (not moved) so that the list keeps its capacity and the array gets an exact fit.
		m_vValues[stIdx].vArray.assign( vList.begin(), vList.end() );
		return stIdx;
	}

	/**
	 * Finishes the results of the parsing.  The values are built during parsing, so this only verifies that there is a root.
	 *
	 * \return Returns true if the JSON has a root value.
	 */
	bool CJsonContainer::BuildTree() {
		// The lists are only needed while parsing.
		m_vMemberLists = std::vector<std::vector<LSON_JSON_MEMBER>>();
		m_vValueLists = std::vector<std::vector<size_t>>();
		m_stMemberListDepth = m_stValueListDepth = 0;
		return m_stRoot < m_vValues.size();
	}

	/**
	 * Finds a member by name and returns a pointer to the given value or nullptr. _jvValue must be an object.
	 *
	 * \param _jvValue The object owning the member to find.
	 * \param _svName The name of the member to locate.
	 * \return Returns a pointer to the member value with the given name or nullptr if it can't be found.
	 */
	const CJsonContainer::LSON_JSON_VALUE * CJsonContainer::GetMemberByName( const LSON_JSON_VALUE &_jvValue, std::string_view _svName ) const {
		if ( _jvValue.vtType == LSON_VT_OBJECT ) {
			// Strings are unique, so the names can be compared by index.
			const size_t stName = FindString( _svName );
			if ( stName == size_t( -1 ) ) { return nullptr; }
			for ( size_t I = 0; I < _jvValue.oObject.vMembers.size(); ++I ) {
				if ( _jvValue.oObject.vMembers[I].stName == stName ) {
					return &GetValue( _jvValue.oObject.vMembers[I].stValue );
				}
			}
		}
		return nullptr;
	}

	/**
	 * Converts the text of a number to a double without regard to the locale (where std::from_chars() supports floating-point values).
	 *
	 * \param _pcText The text of the number.
	 * \param _sLen The length of the text.
	 * \return Returns the converted number, or 0.0 if the text is not a number.
	 */
	double CJsonContainer::ParseDecimal( const char * _pcText, size_t _sLen ) {
#if defined( __cpp_lib_to_chars )
		double dRet = 0.0;
		auto aRes = std::from_chars( _pcText, _pcText + _sLen, dRet );
		static_cast<void>(aRes);
		return dRet;
#else
		// No floating-point std::from_chars() (older standard libraries).  The lexer's text is NUL-terminated.
		static_cast<void>(_sLen);
		return std::strtod( _pcText, nullptr );
#endif	// #if defined( __cpp_lib_to_chars )
	}

}	// namespace lson

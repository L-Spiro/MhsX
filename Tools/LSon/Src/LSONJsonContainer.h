#pragma once

#include "LSONJsonSyntaxNodes.h"
#include "Gen/LSONJsonParser.h"

#include <string_view>
#include <unordered_map>
#include <vector>


namespace lson {

	class CJsonLexer;

	/**
	 * Class CJsonContainer
	 * \brief Processes the results of the parsing and stores the result.
	 *
	 * Description: Processes the results of the parsing and stores the result.  The values are built directly by the parser's actions as
	 *	it reduces (children before their parents), so there is no intermediate syntax tree.  Strings are not copied:  they point into the
	 *	buffer that was parsed, which must outlive the container.
	 */
	class CJsonContainer {
	public :
		CJsonContainer( CJsonLexer * _pxlLexer );


		// == Enumerations
		/** Object types. */
		enum LSON_VALUE_TYPE {
			LSON_VT_OBJECT,																		/**< An object. */
			LSON_VT_STRING,																		/**< A string. */
			LSON_VT_DECIMAL,																	/**< A number. */
			LSON_VT_ARRAY,																		/**< An array of values. */
			LSON_VT_TRUE,																		/**< A TRUE value. */
			LSON_VT_FALSE,																		/**< A FALSE value. */
			LSON_VT_NULL,																		/**< A NULL value. */
		};


		// == Types.
		/** A member. */
		struct LSON_JSON_MEMBER {
			size_t										stName;									/**< Name of the member. */
			size_t										stValue;								/**< Index of the member value. */
#ifdef _DEBUG
			std::string_view							svName;									/**< The name of the member (for debugging). */
#endif	// #ifdef _DEBUG
		};

		/** An object. */
		struct LSON_OBJECT {
			std::vector<LSON_JSON_MEMBER>				vMembers;								/**< The object's members. */
		};

		/** A JSON value. */
		struct LSON_JSON_VALUE {
			LSON_VALUE_TYPE								vtType;									/**< The type of the value. */
			union {
				size_t									stString;								/**< The string index if vtType is LSON_VT_STRING. */
				double									dDecimal;								/**< The number if vtType is LSON_VT_DECIMAL. */
			}											u;
			std::vector<size_t>							vArray;									/**< The array, if vtType is LSON_VT_ARRAY. */
			LSON_OBJECT									oObject;								/**< The object, if vtType is LSON_VT_OBJECT. */
#ifdef _DEBUG
			std::string_view							svString;								/**< The string, if vtType is LSON_VT_STRING (for debugging). */
#endif	// #ifdef _DEBUG
		};




		// == Functions.
		/**
		 * Reserves room for the results of parsing a JSON file of a given size.
		 *
		 * \param _sBytes The size of the JSON text, in bytes.
		 */
		void											Reserve( size_t _sBytes );

		/**
		 * Adds a string and returns its index into the stack.
		 *
		 * \param _svText The string to add or whose existing index is to be found.
		 * \return Returns the index of the added string.
		 */
		size_t											AddString( std::string_view _svText );

		/**
		 * Removes quotes from the front and end before adding the string.
		 *
		 * \param _svText The string to add or whose existing index is to be found.
		 * \return Returns the index of the added string after stripping the enclosing quotes from it.
		 */
		size_t											AddQuoteString( std::string_view _svText );

		/**
		 * Returns the index of a string or -1 if it does not exist.
		 *
		 * \param _svText The string to find.
		 * \return Returns the index of the string if it exists or size_t( -1 ).
		 */
		size_t											FindString( std::string_view _svText ) const;

		/**
		 * Gets a string by index.
		 *
		 * \param _stIdx The index of the string to get.
		 * \return Returns the string at the given index.
		 */
		std::string_view								GetString( size_t _stIdx ) const { return m_vStrings[_stIdx]; }

		/**
		 * Creates a string value.
		 *
		 * \param _stString The index of the string.
		 * \return Returns the index of the value created.
		 */
		size_t											AddStringValue( size_t _stString );

		/**
		 * Creates a decimal value.
		 *
		 * \param _dValue The value.
		 * \return Returns the index of the value created.
		 */
		size_t											AddDecimalValue( double _dValue );

		/**
		 * Creates a TRUE value.
		 *
		 * \return Returns the index of the value created.
		 */
		size_t											AddTrueValue();

		/**
		 * Creates a FALSE value.
		 *
		 * \return Returns the index of the value created.
		 */
		size_t											AddFalseValue();

		/**
		 * Creates a NULL value.
		 *
		 * \return Returns the index of the value created.
		 */
		size_t											AddNullValue();

		/**
		 * Creates an object value with no members.
		 *
		 * \return Returns the index of the value created.
		 */
		size_t											AddObject();

		/**
		 * Starts the member list of an object with its first member.  Member lists nest (an object inside a member's value is finished
		 *	before the next member of its parent is added), so they are kept on a stack.
		 *
		 * \param _mMember The first member.
		 */
		void											BeginMembers( const YYSTYPE::LSON_MEMBER &_mMember );

		/**
		 * Adds a member to the current member list.
		 *
		 * \param _mMember The member to add.
		 */
		void											AddMember( const YYSTYPE::LSON_MEMBER &_mMember );

		/**
		 * Creates an object value from the current member list and ends the list.
		 *
		 * \return Returns the index of the value created.
		 */
		size_t											EndObject();

		/**
		 * Creates an array value with no values.
		 *
		 * \return Returns the index of the value created.
		 */
		size_t											AddArray();

		/**
		 * Starts the value list of an array with its first value.  Value lists nest, so they are kept on a stack.
		 *
		 * \param _stValue The index of the first value.
		 */
		void											BeginValues( size_t _stValue );

		/**
		 * Adds a value to the current value list.
		 *
		 * \param _stValue The index of the value to add.
		 */
		void											AddArrayValue( size_t _stValue );

		/**
		 * Creates an array value from the current value list and ends the list.
		 *
		 * \return Returns the index of the value created.
		 */
		size_t											EndArray();

		/**
		 * Sets the root value.
		 *
		 * \param _stValue The index of the root value.
		 */
		void											SetRoot( size_t _stValue ) { m_stRoot = _stValue; }

		/**
		 * Finishes the results of the parsing.  The values are built during parsing, so this only verifies that there is a root.
		 *
		 * \return Returns true if the JSON has a root value.
		 */
		bool											BuildTree();

		/**
		 * Gets the index of the root value.
		 *
		 * \return Returns the index of the root value (see GetValue()).
		 */
		inline size_t									GetRoot() const { return m_stRoot; }

		/**
		 * Gets a value by index.
		 *
		 * \param _stIdx The index of the value to get.
		 * \return Returns the value at the given index.
		 */
		inline const LSON_JSON_VALUE &					GetValue( size_t _stIdx ) const { return m_vValues[_stIdx]; }

		/**
		 * Finds a member by name and returns a pointer to the given value or nullptr. _jvValue must be an object.
		 *
		 * \param _jvValue The object owning the member to find.
		 * \param _svName The name of the member to locate.
		 * \return Returns a pointer to the member value with the given name or nullptr if it can't be found.
		 */
		const LSON_JSON_VALUE *							GetMemberByName( const LSON_JSON_VALUE &_jvValue, std::string_view _svName ) const;

		/**
		 * Converts the text of a number to a double without regard to the locale (where std::from_chars() supports floating-point values).
		 *
		 * \param _pcText The text of the number.
		 * \param _sLen The length of the text.
		 * \return Returns the converted number, or 0.0 if the text is not a number.
		 */
		static double									ParseDecimal( const char * _pcText, size_t _sLen );


	protected :
		// == Members.
		CJsonLexer *									m_pxlLexer;								/**< The lexer. */
		std::vector<std::string_view>					m_vStrings;								/**< The strings (unique), pointing into the parsed text. */
		std::unordered_map<std::string_view, size_t>	m_mStringIndex;							/**< Maps strings to their indices in m_vStrings. */
		std::vector<LSON_JSON_VALUE>					m_vValues;								/**< The values. */
		std::vector<std::vector<LSON_JSON_MEMBER>>		m_vMemberLists;							/**< The member lists being built (a stack, reused between objects). */
		std::vector<std::vector<size_t>>				m_vValueLists;							/**< The value lists being built (a stack, reused between arrays). */
		size_t											m_stMemberListDepth;					/**< The number of member lists being built. */
		size_t											m_stValueListDepth;						/**< The number of value lists being built. */
		size_t											m_stRoot;								/**< The index of the root value. */


		// == Functions.
		/**
		 * Adds a value of a given type and returns its index.
		 *
		 * \param _vtType The type of the value.
		 * \return Returns the index of the added value inside m_vValues.
		 */
		inline size_t									AddValue( LSON_VALUE_TYPE _vtType ) {
			size_t stIdx = m_vValues.size();
			LSON_JSON_VALUE & jvValue = m_vValues.emplace_back();
			jvValue.vtType = _vtType;
			jvValue.u.dDecimal = 0.0;
			return stIdx;
		}
	};

}	// namespace lson

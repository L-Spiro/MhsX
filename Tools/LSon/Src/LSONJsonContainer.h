#pragma once

#include "LSONJsonSyntaxNodes.h"
#include "Gen/LSONJsonParser.h"

#include <vector>


namespace lson {

	class CJsonLexer;

	/**
	 * Class CJsonContainer
	 * \brief Processes the results of the parsing and stores the result.
	 *
	 * Description: Processes the results of the parsing and stores the result.
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
			std::string									sName;
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
			std::string									sString;
#endif	// #ifdef _DEBUG
		};

		

		
		// == Functions.
		/**
		 * Adds a string and returns its index into the stack.
		 *
		 * \param _sText The string to add or whose existing index is to be found.
		 * \return Returns the index of the added string.
		 */
		size_t											AddString( const std::string &_sText );

		/**
		 * Removes quotes from the front and end before adding the string.
		 *
		 * \param _sText The string to add or whose existing index is to be found.
		 * \return Returns the index of the added string after stripping the enclosing quotes from it.
		 */
		size_t											AddQuoteString( const std::string &_sText );

		/**
		 * Returns the index of a string or -1 if it does not exist.
		 *
		 * \param _sText The string to find.
		 * \return Returns the index of the string if it exists or size_t( -1 ).
		 */
		size_t											FindString( const std::string &_sText ) const;

		/**
		 * Gets a reference to a string by index.
		 *
		 * \param _stIdx String index.
		 * \return Returns a constant reference to the string given its index.
		 */
		const std::string								GetString( size_t _stIdx ) const { return m_vStrings[_stIdx]; }

		/**
		 * Creates an object value node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nValue The value.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddObjectValue( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nValue );

		/**
		 * Creates a string value node.
		 *
		 * \param _nNode The resulting node.
		 * \param _stValue The string.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddStringValue( YYSTYPE::LSON_NODE &_nNode, size_t _stValue );

		/**
		 * Creates a decimal value node.
		 *
		 * \param _nNode The resulting node.
		 * \param _dValue The value.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddDecimalValue( YYSTYPE::LSON_NODE &_nNode, double _dValue );

		/**
		 * Creates an array value node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nValue The value.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddArrayValue( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nValue );

		/**
		 * Creates a TRUE value node.
		 *
		 * \param _nNode The resulting node.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddTrueValue( YYSTYPE::LSON_NODE &_nNode );

		/**
		 * Creates a FALSE value node.
		 *
		 * \param _nNode The resulting node.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddFalseValue( YYSTYPE::LSON_NODE &_nNode );

		/**
		 * Creates a NULL value node.
		 *
		 * \param _nNode The resulting node.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddNullValue( YYSTYPE::LSON_NODE &_nNode );

		/**
		 * Creates a member node.
		 *
		 * \param _nNode The resulting node.
		 * \param _stName The name of the member.
		 * \param _nValue The value.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddMember( YYSTYPE::LSON_NODE &_nNode, size_t _stName, const YYSTYPE::LSON_NODE &_nValue );

		/**
		 * Creates a member-list node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left node.
		 * \param _nRight The right node.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddMemberList( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight );

		/**
		 * Creates an empty member-list node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left node.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddMemberList( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft );

		/**
		 * Creates an object node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nMembers The members node.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddObject( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nMembers );

		/**
		 * Creates an object node with no members.
		 *
		 * \param _nNode The resulting node.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddObject( YYSTYPE::LSON_NODE &_nNode );

		/**
		 * Creates a value-list node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left node.
		 * \param _nRight The right node.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddValueList( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight );

		/**
		 * Creates an empty value-list node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left node.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddValueList( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft );

		/**
		 * Creates an array node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nValues The values node.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddArray( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nValues );

		/**
		 * Creates an empty array node.
		 *
		 * \param _nNode The resulting node.
		 * \return Returns the index of the value node created.
		 */
		size_t											AddArray( YYSTYPE::LSON_NODE &_nNode );

		/**
		 * Creates the JSON node.
		 *
		 * \param _nNode The resulting node.
		 */
		void											AddJson( YYSTYPE::LSON_NODE &_nNode );

		/**
		 * Builds the tree that can be used to access data in the JSON.
		 */
		bool											BuildTree();

		/**
		 * Gets the root value index.
		 *
		 * \return Returns the root value's index.
		 */
		inline constexpr size_t							GetRoot() const { return 0; }

		/**
		 * Gets a value by index.
		 *
		 * \param _stIdx Index of te value to get.
		 * \return Returns a constant reference to a value loaded from the JSON file by index.
		 */
		inline const LSON_JSON_VALUE &					GetValue( size_t _stIdx ) const { return m_vValues[_stIdx]; }

		/**
		 * Finds a member by name and returns a pointer to the given value or nullptr.  _jvValue must be an object.
		 *
		 * \param _jvValue The object owning the member to find.
		 * \param _sName The name of the member to locate.
		 * \return Returns a pointer to the member value with the given name or nullptr if it can't be found.
		 */
		const LSON_JSON_VALUE *							GetMemberByName( const LSON_JSON_VALUE &_jvValue, const std::string &_sName );


	protected :
		// == Members.
		/** The lexer object. */
		CJsonLexer *									m_pxlLexer;
		/** The stack of nodes. */
		std::vector<YYSTYPE::LSON_NODE>					m_vNodes;
		/** The stack of UTF-8 strings. */
		std::vector<std::string>						m_vStrings;
		/** Tree values, referenced by index. */
		std::vector<LSON_JSON_VALUE>					m_vValues;
		/** The resulting tree. */
		//CTree<size_t>									m_tRoot;
		/** The base node. */
		size_t											m_stRoot;


		// == Functions.
		/**
		 * Adds a node and returns its index into the array of nodes.
		 *
		 * \param _nNode The node to add.
		 * \return Returns the index of the added node inside m_vNodes.
		 */
		size_t											AddNode( YYSTYPE::LSON_NODE &_nNode );

		/**
		 * Adds a value to be indexed by the tree.
		 *
		 * \param _jvValue The value structure to added to the internal database of values.
		 * \return Returns the index of the stored value.
		 */
		inline size_t									AddValue( LSON_JSON_VALUE &_jvValue ) {
			size_t stIdx = m_vValues.size();
			m_vValues.push_back( std::move( _jvValue ) );
			return stIdx;
		}

		/**
		 * Converts a pointer to a tree pointer (nullptr = &m_tRoot, everything else is pass-through).
		 *
		 * \param _ptPtr The pointer to sanitize.
		 * \return Returns the input pointer or &m_tRoot if the input is nullptr.
		 */
		/*CTree<LSON_JSON_ELEMENT> *						GetTreePointer( CTree<LSON_JSON_ELEMENT> * _ptPtr ) {
			return _ptPtr ? _ptPtr : &m_tRoot;
		}*/

		/**
		 * Gets the current tree pointer from a stack of tree pointers.
		 *
		 * \param _vStack The input stack of pointers.
		 * \return Returns &m_tRoot if the stack is empty, otherwise the top of the stack is returned.
		 */
		LSON_JSON_VALUE *								CurStackPointer( const std::vector<size_t> &_vStack ) {
			return (m_vValues.size() == 0 || _vStack.size() == 0) ? nullptr : &m_vValues[_vStack[_vStack.size()-1]];
		}

		/**
		 * Prints the tree recursively.
		 * 
		 * \param _ptNode The node to print.
		 * \param _i32Depth The node depth.
		 */
		//void											PrintTree( const CTree<LSON_JSON_ELEMENT> * _ptNode, int32_t _i32Depth );
		
	};

}	// namespace lson

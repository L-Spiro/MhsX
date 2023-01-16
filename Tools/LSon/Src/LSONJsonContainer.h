#pragma once

//#include "LSONJson.h"
#include "LSONJsonSyntaxNodes.h"
#include "Gen/LSONJsonParser.h"
#include "Tree/LSONTree.h"

#include <map>
#include <set>


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


		// == Types.
		/** The public attibute structure. */
		struct LSON_JSON_ATTRIBUTE {
			size_t										stNameString = size_t( -1 );			/**< Use GetString() to get the name of the attribute. */
			size_t										stValueString = size_t( -1 );			/**< Use GetString() to get the name of the attribute value (unles sit is size_t( -1 )). */
		};

		/** The public node structure. */
		struct LSON_JSON_ELEMENT {
			size_t										stNameString = size_t( -1 );			/**< Use GetString() to get the name of the element. */
			std::vector<LSON_JSON_ATTRIBUTE>			vAttributes;							/**< The attributes associated with this element. */
			std::string									sData;									/**< Combined data values minus any data values that are nothing but whitespace. */
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
		 * \return Returns the index of the value node created.
		 */
		size_t											AddMemberList( YYSTYPE::LSON_NODE &_nNode );

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

#if 0
		/**
		 * Creates an attribute start string.
		 *
		 * \param _sText The name of the attribute.
		 * \return Returns the index of the string created.
		 */
		size_t											AddAttributeStart( const std::string &_sText );

		/**
		 * Creates an attribute node.
		 *
		 * \param _nNode The resulting node.
		 * \param _stName The name of the attribute.
		 * \param _stValue The optional value of the attribute.
		 * \return Returns the index of the attribute node created.
		 */
		size_t											AddAttribute( YYSTYPE::LSON_NODE &_nNode, size_t _stName, size_t _stValue = size_t( -1 ) );

		/**
		 * Creates an attribute list.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left attribute node.
		 * \param _nRight The right attribute node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddAttributeList( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight );

		/**
		 * Creates a blank attribute list.
		 *
		 * \param _nNode The resulting node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddAttributeList( YYSTYPE::LSON_NODE &_nNode );

		/**
		 * Creates an attribute declaration.
		 *
		 * \param _nNode The resulting node.
		 * \param _stName The attribute name.
		 * \param _nAttributes Optional attributes associated with it.
		 * \return Returns the index of the node created.
		 */
		size_t											AddAttributeDecl( YYSTYPE::LSON_NODE &_nNode, size_t _stName, const YYSTYPE::LSON_NODE &_nAttributes );

		/**
		 * Creates a data content node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left content node.
		 * \param _stData The data.
		 * \return Returns the index of the node created.
		 */
		size_t											AddContentData( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, size_t _stData );

		/**
		 * Creates a miscellaneous content node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left content node.
		 * \param _nRight The right content node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddContentMisc( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight );

		/**
		 * Creates an element content node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left content node.
		 * \param _nRight The right content node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddContentElement( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight );

		/**
		 * Creates a content node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nContent The content node.
		 * \param _stName The name.
		 * \return Returns the index of the node created.
		 */
		size_t											AddContent( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nContent, size_t _stName );

		/**
		 * Creates an element node.
		 *
		 * \param _nNode The resulting node.
		 * \param _stName The name of the element.
		 * \param _nAttributes The attribute node.
		 * \param _nContent The content node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddElement( YYSTYPE::LSON_NODE &_nNode, size_t _stName, const YYSTYPE::LSON_NODE &_nAttributes, const YYSTYPE::LSON_NODE &_nContent );

		/**
		 * Creates an element node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left node.
		 * \param _nRight The right node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddMiscSeq( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight );

		/**
		 * Creates an encoding node.
		 *
		 * \param _nNode The resulting node.
		 * \param _pcEncoding The encoding string.
		 * \return Returns the index of the node created.
		 */
		size_t											AddEncoding( YYSTYPE::LSON_NODE &_nNode, const char * _pcEncoding );

		/**
		 * Creates a version node.
		 *
		 * \param _nNode The resulting node.
		 * \param _pcVersion The version string.
		 * \return Returns the index of the node created.
		 */
		size_t											AddVersion( YYSTYPE::LSON_NODE &_nNode, const char * _pcVersion );

		/**
		 * Creates a prolog node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nVersion The optional version node.
		 * \param _nEncoding The optional encoding node.
		 * \param _nMisc The optional misc. sequence node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddProlog( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nVersion, const YYSTYPE::LSON_NODE &_nEncoding, const YYSTYPE::LSON_NODE &_nMisc );

		/**
		 * Creates a document node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nProlog The prolog node.
		 * \param _nElement The element node.
		 * \param _nMisc The optional misc. sequence node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddDocument( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nProlog, const YYSTYPE::LSON_NODE &_nElement, const YYSTYPE::LSON_NODE &_nMisc );

		/**
		 * Creates an empty node.
		 *
		 * \param _nNode The resulting node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddEmpty( YYSTYPE::LSON_NODE &_nNode );

		/**
		 * Prints a node recursively.
		 *
		 * \param _stIdx The node index to print.
		 * \param _i32Depth The node depth.
		 */
		void											PrintNode( size_t _stIdx, int32_t _i32Depth ) const;

		/**
		 * Gets the root node.
		 *
		 * \return Returns the index of the root node.
		 */
		size_t											Root() const { return m_stRoot; }

		/**
		 * Builds the JSON tree non-recursively.
		 *
		 * \return Returns true if there were no memory errors during tree creation.
		 */
		bool											BuidTree();

		/**
		 * Prints the tree recursively.
		 */
		void											PrintTree();

		/**
		 * Gets the next item.
		 *
		 * \param _ptThis The items whose next item is to be obtained.
		 * \return Returns the next item.
		 */
		CTree<LSON_JSON_ELEMENT> *						Next( CTree<LSON_JSON_ELEMENT> * _ptThis );

		/**
		 * Gets the previous item.
		 *
		 * \param _ptThis The items whose previous item is to be obtained.
		 * \return Returns the previous item.
		 */
		CTree<LSON_JSON_ELEMENT> *						Prev( CTree<LSON_JSON_ELEMENT> * _ptThis );

		/**
		 * Gets the first child element by name.
		 *
		 * \param _ptParent The node whose children are to be scanned for elements matching the given name.
		 * \param _sName The name of the child element to find.
		 * \return Returns a pointer to the child element of te given name or nullptr if there is none.
		 */
		const CTree<LSON_JSON_ELEMENT> *				GetChildElement( const CTree<LSON_JSON_ELEMENT> * _ptParent, const std::string &_sName ) const;

		/**
		 * Gets the first child element by name.
		 *
		 * \param _ptParent The node whose children are to be scanned for elements matching the given name.
		 * \param _sName The name of the child element to find.
		 * \return Returns a pointer to the child element of te given name or nullptr if there is none.
		 */
		CTree<LSON_JSON_ELEMENT> *						GetChildElement( CTree<LSON_JSON_ELEMENT> * _ptParent, const std::string &_sName );

		/**
		 * Gathers the indices of children nodes (non-recursively) whose element names match the given name.
		 *
		 * \param _ptParent The node whose children are to be scanned for elements matching the given name.
		 * \param _sName The name of the children elements to gather.
		 * \return Returns an array of indices indicating the children of _ptParent whose element names match _sName.
		 */
		std::vector<size_t>								GatherChildElementIndices( const CTree<LSON_JSON_ELEMENT> * _ptParent, const std::string &_sName ) const;

		/**
		 * Gets the value of an attribute on a given element.
		 *
		 * \param _ptElement The element whose attributes are to be searched.
		 * \param _sName The name of the attribute whose value is to be found.
		 * \param _sRet Holds the returned value of the attribute.
		 * \return Returns true if the given attribute was found on the given item.
		 */
		bool											GetAttributeValue( const CTree<LSON_JSON_ELEMENT> * _ptElement, const std::string &_sName, std::string &_sRet ) const;

		/**
		 * Gets the data of an child element given its name.
		 *
		 * \param _ptParent The element whose child elements are to be searched.
		 * \param _sName The name of the child element whose value is to be found.
		 * \param _sRet Holds the returned data value of the element.
		 * \return Returns true if the given child element was found on the given item.
		 */
		bool											GetChildElementData( const CTree<LSON_JSON_ELEMENT> * _ptParent, const std::string &_sName, std::string &_sRet ) const;

		/**
		 * Gets the data associated with a given element.
		 *
		 * \param _ptElement The element whose data is to be returned.
		 * \param PARM Holds the return value of the element’s data if true is returned.
		 * \return Returns true 
		 */

		/**
		 * Gathers every attribute and their values into a multi-map.
		 *
		 * \param _mmDst The destination multi-map.
		 * \return Returns true if there were no memory errors.
		 */
		bool											GatherAttributes( std::multimap<std::string, std::string> &_mmDst );

		/**
		 * Gathers every attribute and their values into a map.
		 *
		 * \param _mDst The destination map.
		 * \return Returns true if there were no memory errors.
		 */
		bool											GatherAttributes( std::map<std::string, std::set<std::string>> &_mDst );

		/**
		 * Gathers every element and their data values into a map.
		 *
		 * \param _mDst The destination map.
		 * \return Returns true if there were no memory errors.
		 */
		bool											GatherElements( std::map<std::string, std::set<std::string>> &_mDst );

#endif
	protected :
		// == Members.
		/** The lexer object. */
		CJsonLexer *									m_pxlLexer;
		/** The stack of nodes. */
		std::vector<YYSTYPE::LSON_NODE>					m_vNodes;
		/** The stack of UTF-8 strings. */
		std::vector<std::string>						m_vStrings;
		/** The resulting tree. */
		CTree<LSON_JSON_ELEMENT>						m_tRoot;
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
		 * Converts a pointer to a tree pointer (nullptr = &m_tRoot, everything else is pass-through).
		 *
		 * \param _ptPtr The pointer to sanitize.
		 * \return Returns the input pointer or &m_tRoot if the input is nullptr.
		 */
		CTree<LSON_JSON_ELEMENT> *						GetTreePointer( CTree<LSON_JSON_ELEMENT> * _ptPtr ) {
			return _ptPtr ? _ptPtr : &m_tRoot;
		}

		/**
		 * Gets the current tree pointer from a stack of tree pointers.
		 *
		 * \param _vStack The input stack of pointers.
		 * \return Returns &m_tRoot if the stack is empty, otherwise the top of the stack is returned.
		 */
		CTree<LSON_JSON_ELEMENT> *						CurStackPointer( const std::vector<CTree<LSON_JSON_ELEMENT> *> &_vStack ) {
			return GetTreePointer( (_vStack.size() == 0) ? nullptr : _vStack[_vStack.size()-1] );
		}

		/**
		 * Determines if a string is entirely whitespace.
		 *
		 * \param _sString The string to check.
		 * \return Returns true if the given string is entirely whitespace ([ \t\r\n]+).
		 */
		bool											IsWhitespace( const std::string & _sString );

		/**
		 * Prints the tree recursively.
		 * 
		 * \param _ptNode The node to print.
		 * \param _i32Depth The node depth.
		 */
		void											PrintTree( const CTree<LSON_JSON_ELEMENT> * _ptNode, int32_t _i32Depth );
		
	};

}	// namespace lson

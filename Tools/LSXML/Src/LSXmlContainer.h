#pragma once

//#include "LSXml.h"
#include "LSXmlSyntaxNodes.h"
#include "Gen/LSXmlParser.h"
#include "Tree/LSXTree.h"

#include <map>
#include <set>


namespace lsx {

	class CXmlLexer;

	/**
	 * Class CXmlContainer
	 * \brief Processes the results of the parsing and stores the result.
	 *
	 * Description: Processes the results of the parsing and stores the result.
	 */
	class CXmlContainer {
	public :
		CXmlContainer( CXmlLexer * _pxlLexer );


		// == Types.
		/** The public attibute structure. */
		struct LSX_XML_ATTRIBUTE {
			size_t										stNameString = size_t( -1 );			/**< Use GetString() to get the name of the attribute. */
			size_t										stValueString = size_t( -1 );			/**< Use GetString() to get the name of the attribute value (unles sit is size_t( -1 )). */
		};

		/** The public node structure. */
		struct LSX_XML_ELEMENT {
			size_t										stNameString = size_t( -1 );			/**< Use GetString() to get the name of the element. */
			std::vector<LSX_XML_ATTRIBUTE>				vAttributes;							/**< The attributes associated with this element. */
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
		 * A value is a string with quotes around it.  After the quotes are removed, this behaves as AddString().
		 *
		 * \param _sText The string to add or whose existing index is to be found.
		 * \return Returns the index of the added string after stripping the enclosing quotes from it.
		 */
		size_t											AddValue( const std::string &_sText );

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
		size_t											AddAttribute( YYSTYPE::LSX_NODE &_nNode, size_t _stName, size_t _stValue = size_t( -1 ) );

		/**
		 * Creates an attribute list.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left attribute node.
		 * \param _nRight The right attribute node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddAttributeList( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nLeft, const YYSTYPE::LSX_NODE &_nRight );

		/**
		 * Creates a blank attribute list.
		 *
		 * \param _nNode The resulting node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddAttributeList( YYSTYPE::LSX_NODE &_nNode );

		/**
		 * Creates an attribute declaration.
		 *
		 * \param _nNode The resulting node.
		 * \param _stName The attribute name.
		 * \param _nAttributes Optional attributes associated with it.
		 * \return Returns the index of the node created.
		 */
		size_t											AddAttributeDecl( YYSTYPE::LSX_NODE &_nNode, size_t _stName, const YYSTYPE::LSX_NODE &_nAttributes );

		/**
		 * Creates a data content node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left content node.
		 * \param _stData The data.
		 * \return Returns the index of the node created.
		 */
		size_t											AddContentData( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nLeft, size_t _stData );

		/**
		 * Creates a miscellaneous content node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left content node.
		 * \param _nRight The right content node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddContentMisc( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nLeft, const YYSTYPE::LSX_NODE &_nRight );

		/**
		 * Creates an element content node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left content node.
		 * \param _nRight The right content node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddContentElement( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nLeft, const YYSTYPE::LSX_NODE &_nRight );

		/**
		 * Creates a content node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nContent The content node.
		 * \param _stName The name.
		 * \return Returns the index of the node created.
		 */
		size_t											AddContent( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nContent, size_t _stName );

		/**
		 * Creates an element node.
		 *
		 * \param _nNode The resulting node.
		 * \param _stName The name of the element.
		 * \param _nAttributes The attribute node.
		 * \param _nContent The content node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddElement( YYSTYPE::LSX_NODE &_nNode, size_t _stName, const YYSTYPE::LSX_NODE &_nAttributes, const YYSTYPE::LSX_NODE &_nContent );

		/**
		 * Creates an element node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nLeft The left node.
		 * \param _nRight The right node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddMiscSeq( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nLeft, const YYSTYPE::LSX_NODE &_nRight );

		/**
		 * Creates an encoding node.
		 *
		 * \param _nNode The resulting node.
		 * \param _pcEncoding The encoding string.
		 * \return Returns the index of the node created.
		 */
		size_t											AddEncoding( YYSTYPE::LSX_NODE &_nNode, const char * _pcEncoding );

		/**
		 * Creates a version node.
		 *
		 * \param _nNode The resulting node.
		 * \param _pcVersion The version string.
		 * \return Returns the index of the node created.
		 */
		size_t											AddVersion( YYSTYPE::LSX_NODE &_nNode, const char * _pcVersion );

		/**
		 * Creates a prolog node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nVersion The optional version node.
		 * \param _nEncoding The optional encoding node.
		 * \param _nMisc The optional misc. sequence node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddProlog( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nVersion, const YYSTYPE::LSX_NODE &_nEncoding, const YYSTYPE::LSX_NODE &_nMisc );

		/**
		 * Creates a document node.
		 *
		 * \param _nNode The resulting node.
		 * \param _nProlog The prolog node.
		 * \param _nElement The element node.
		 * \param _nMisc The optional misc. sequence node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddDocument( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nProlog, const YYSTYPE::LSX_NODE &_nElement, const YYSTYPE::LSX_NODE &_nMisc );

		/**
		 * Creates an empty node.
		 *
		 * \param _nNode The resulting node.
		 * \return Returns the index of the node created.
		 */
		size_t											AddEmpty( YYSTYPE::LSX_NODE &_nNode );

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
		 * Builds the XML tree non-recursively.
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
		CTree<LSX_XML_ELEMENT> *						Next( CTree<LSX_XML_ELEMENT> * _ptThis );

		/**
		 * Gets the previous item.
		 *
		 * \param _ptThis The items whose previous item is to be obtained.
		 * \return Returns the previous item.
		 */
		CTree<LSX_XML_ELEMENT> *						Prev( CTree<LSX_XML_ELEMENT> * _ptThis );

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


	protected :
		// == Members.
		/** The lexer object. */
		CXmlLexer *										m_pxlLexer;
		/** The stack of nodes. */
		std::vector<YYSTYPE::LSX_NODE>					m_vNodes;
		/** The stack of UTF-8 strings. */
		std::vector<std::string>						m_vStrings;
		/** The resulting tree. */
		CTree<LSX_XML_ELEMENT>							m_tRoot;
		/** The base node. */
		size_t											m_stRoot;


		// == Functions.
		/**
		 * Adds a node and returns its index into the array of nodes.
		 *
		 * \param _nNode The node to add.
		 * \return Returns the index of the added node inside m_vNodes.
		 */
		size_t											AddNode( YYSTYPE::LSX_NODE &_nNode );

		/**
		 * Converts a pointer to a tree pointer (nullptr = &m_tRoot, everything else is pass-through).
		 *
		 * \param _ptPtr The pointer to sanitize.
		 * \return Returns the input pointer or &m_tRoot if the input is nullptr.
		 */
		CTree<LSX_XML_ELEMENT> *						GetTreePointer( CTree<LSX_XML_ELEMENT> * _ptPtr ) {
			return _ptPtr ? _ptPtr : &m_tRoot;
		}

		/**
		 * Gets the current tree pointer from a stack of tree pointers.
		 *
		 * \param _vStack The input stack of pointers.
		 * \return Returns &m_tRoot if the stack is empty, otherwise the top of the stack is returned.
		 */
		CTree<LSX_XML_ELEMENT> *						CurStackPointer( const std::vector<CTree<LSX_XML_ELEMENT> *> &_vStack ) {
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
		void											PrintTree( const CTree<LSX_XML_ELEMENT> * _ptNode, int32_t _i32Depth );
		
	};

}	// namespace lsx

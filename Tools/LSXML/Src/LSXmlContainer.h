#pragma once

#include "LSXml.h"
#include "LSXmlSyntaxNodes.h"
#include "Gen/LSXmlParser.h"


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


	protected :
		// == Members.
		/** The lexer object. */
		CXmlLexer *										m_pxlLexer;
		/** The stack of nodes. */
		std::vector<YYSTYPE::LSX_NODE>					m_vNodes;
		/** The stack of UTF-8 strings. */
		std::vector<std::string>						m_vStrings;
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

		
	};

}	// namespace lsx

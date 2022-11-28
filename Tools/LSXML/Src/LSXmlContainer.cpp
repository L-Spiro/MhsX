// TMP.
#include <Windows.h>

#include "LSXmlContainer.h"
#include "LSXmlLexer.h"


#include <format>


namespace lsx {

	CXmlContainer::CXmlContainer( CXmlLexer * _pxlLexer ) :
		m_pxlLexer( _pxlLexer ),
		m_stRoot( size_t( -1 ) ) {
	}

	// == Functions.
	/**
	 * Adds a string and returns its index into the stack.
	 *
	 * \param _sText The string to add or whose existing index is to be found.
	 * \return Returns the index of the added string.
	 */
	size_t CXmlContainer::AddString( const std::string &_sText ) {
		size_t stRet = FindString( _sText );
		if ( stRet == size_t( -1 ) ) {
			m_vStrings.push_back( _sText );
			return m_vStrings.size() - 1;
		}
		return stRet;
	}

	/**
	 * A value is a string with quotes around it.  After the quotes are removed, this behaves as AddString().
	 *
	 * \param _sText The string to add or whose existing index is to be found.
	 * \return Returns the index of the added string after stripping the enclosing quotes from it.
	 */
	size_t CXmlContainer::AddValue( const std::string &_sText ) {
		return AddString( _sText.substr( 1, _sText.size() - 2 ) );
	}

	/**
	 * Returns the index of a string or -1 if it does not exist.
	 *
	 * \param _sText The string to find.
	 * \return Returns the index of the string if it exists or size_t( -1 ).
	 */
	size_t CXmlContainer::FindString( const std::string &_sText ) const {
		for ( auto I = m_vStrings.size(); I--; ) {
			if ( m_vStrings[I] == _sText ) { return I; }
		}
		return size_t( -1 );
	}

	/**
	 * Creates an attribute start string.
	 *
	 * \param _sText The name of the attribute.
	 * \return Returns the index of the string created.
	 */
	size_t CXmlContainer::AddAttributeStart( const std::string &_sText ) {
		for ( auto I = _sText.size(); I--; ) {
			// Parser matches:
			// {open}{ws}?{name}
			// Easiest approach is to scan backward for {ws}:
			// ws									[ \t\r\n]+
			// or '<':
			// open								{nl}?"<"
			if ( (_sText[I] == ' ' ||
				_sText[I] == '\t' ||
				_sText[I] == '\r' ||
				_sText[I] == '\n') ||
				_sText[I] == '<' ) {
				return AddString( _sText.substr( I + 1, std::string::npos ) );
			}
		}

		return AddString( _sText );
	}

	/**
	 * Creates an attribute node.
	 *
	 * \param _nNode The resulting node.
	 * \param _stName The name of the attribute.
	 * \param _stValue The optional value of the attribute.
	 * \return Returns the index of the attribute node created.
	 */
	size_t CXmlContainer::AddAttribute( YYSTYPE::LSX_NODE &_nNode, size_t _stName, size_t _stValue ) {
		_nNode.nType = LSX_N_ATTRIBUTE;
		_nNode.u.aAttributeObj.sAttribute = _stName;
		_nNode.u.aAttributeObj.sValue = _stValue;

		return AddNode( _nNode );
	}

	/**
	 * Creates an attribute list.
	 *
	 * \param _nNode The resulting node.
	 * \param _nLeft The left attribute node.
	 * \param _nRight The right attribute node.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddAttributeList( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nLeft, const YYSTYPE::LSX_NODE &_nRight ) {
		_nNode.nType = LSX_N_ATTRIBUTE_LIST;
		_nNode.u.aAttributeListObj.sLeft = _nLeft.stNodeIdx;
		_nNode.u.aAttributeListObj.sRight = _nRight.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates a blank attribute list.
	 *
	 * \param _nNode The resulting node.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddAttributeList( YYSTYPE::LSX_NODE &_nNode ) {
		_nNode.nType = LSX_N_ATTRIBUTE_LIST;
		_nNode.u.aAttributeListObj.sLeft = size_t( -1 );
		_nNode.u.aAttributeListObj.sRight = size_t( -1 );

		return AddNode( _nNode );
	}

	/**
	 * Creates an attribute declaration.
	 *
	 * \param _nNode The resulting node.
	 * \param _stName The attribute name.
	 * \param _nAttributes Optional attributes associated with it.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddAttributeDecl( YYSTYPE::LSX_NODE &_nNode, size_t _stName, const YYSTYPE::LSX_NODE &_nAttributes ) {
		_nNode.nType = LSX_N_ATTRIBUTE_DECL;
		_nNode.u.aAttributeDeclObj.sName = _stName;
		_nNode.u.aAttributeDeclObj.sList = _nAttributes.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates a data content node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nLeft The left content node.
	 * \param _stData The data.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddContentData( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nLeft, size_t _stData ) {
		_nNode.nType = LSX_N_CONTENT_DATA;
		_nNode.u.cContentObj.sLeft = _nLeft.stNodeIdx;
		_nNode.u.cContentObj.sRight = _stData;

		return AddNode( _nNode );
	}

	/**
	 * Creates a miscellaneous content node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nLeft The left content node.
	 * \param _nRight The right content node.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddContentMisc( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nLeft, const YYSTYPE::LSX_NODE &_nRight ) {
		_nNode.nType = LSX_N_CONTENT_MISC;
		_nNode.u.cContentObj.sLeft = _nLeft.stNodeIdx;
		_nNode.u.cContentObj.sRight = _nRight.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates an element content node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nLeft The left content node.
	 * \param _nRight The right content node.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddContentElement( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nLeft, const YYSTYPE::LSX_NODE &_nRight ) {
		_nNode.nType = LSX_N_CONTENT_ELEMENT;
		_nNode.u.cContentObj.sLeft = _nLeft.stNodeIdx;
		_nNode.u.cContentObj.sRight = _nRight.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates a content node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nContent The content node.
	 * \param _stName The name.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddContent( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nContent, size_t _stName ) {
		_nNode.nType = LSX_N_CONTENT;
		_nNode.u.cContentObj.sLeft = _nContent.stNodeIdx;
		_nNode.u.cContentObj.sRight = _stName;

		return AddNode( _nNode );
	}

	/**
	 * Creates an element node.
	 *
	 * \param _nNode The resulting node.
	 * \param _stName The name of the element.
	 * \param _nAttributes The attribute node.
	 * \param _nContent The content node.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddElement( YYSTYPE::LSX_NODE &_nNode, size_t _stName, const YYSTYPE::LSX_NODE &_nAttributes, const YYSTYPE::LSX_NODE &_nContent ) {
		_nNode.nType = LSX_N_ELEMENT;
		_nNode.u.eElementObj.sName = _stName;
		_nNode.u.eElementObj.sAttribute = _nAttributes.stNodeIdx;
		_nNode.u.eElementObj.sContent = _nContent.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates an element node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nLeft The left node.
	 * \param _nRight The right node.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddMiscSeq( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nLeft, const YYSTYPE::LSX_NODE &_nRight ) {
		_nNode.nType = LSX_N_MISC_SEQ;
		_nNode.u.msMiscSeqObj.sLeft = _nLeft.stNodeIdx;
		_nNode.u.msMiscSeqObj.sRight = _nRight.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates an encoding node.
	 *
	 * \param _nNode The resulting node.
	 * \param _pcEncoding The encoding string.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddEncoding( YYSTYPE::LSX_NODE &_nNode, const char * _pcEncoding ) {
		_nNode.nType = LSX_N_ENCODING;
		_nNode.u.eEncodingObj.sEncodingStr = AddString( _pcEncoding );

		return AddNode( _nNode );
	}

	/**
	 * Creates a version node.
	 *
	 * \param _nNode The resulting node.
	 * \param _pcVersion The version string.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddVersion( YYSTYPE::LSX_NODE &_nNode, const char * _pcVersion ) {
		_nNode.nType = LSX_N_VERSION;
		_nNode.u.vVersionObj.sVersionStr = AddString( _pcVersion );

		return AddNode( _nNode );
	}

	/**
	 * Creates a prolog node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nVersion The optional version node.
	 * \param _nEncoding The optional encoding node.
	 * \param _nMisc The optional misc. sequence node.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddProlog( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nVersion, const YYSTYPE::LSX_NODE &_nEncoding, const YYSTYPE::LSX_NODE &_nMisc ) {
		_nNode.nType = LSX_N_PROLOG;
		_nNode.u.pPrologObj.sVersion = _nVersion.stNodeIdx;
		_nNode.u.pPrologObj.sEncoding = _nEncoding.stNodeIdx;
		_nNode.u.pPrologObj.sMiscSeq = _nMisc.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates a document node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nProlog The prolog node.
	 * \param _nElement The element node.
	 * \param _nMisc The optional misc. sequence node.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddDocument( YYSTYPE::LSX_NODE &_nNode, const YYSTYPE::LSX_NODE &_nProlog, const YYSTYPE::LSX_NODE &_nElement, const YYSTYPE::LSX_NODE &_nMisc ) {
		_nNode.nType = LSX_N_DOCUMENT;
		_nNode.u.dDocumentObj.sProlog = _nProlog.stNodeIdx;
		_nNode.u.dDocumentObj.sElement = _nElement.stNodeIdx;
		_nNode.u.dDocumentObj.sMiscSeq = _nMisc.stNodeIdx;

		return (m_stRoot = AddNode( _nNode ));
	}

	/**
	 * Creates an empty node.
	 *
	 * \param _nNode The resulting node.
	 * \return Returns the index of the node created.
	 */
	size_t CXmlContainer::AddEmpty( YYSTYPE::LSX_NODE &_nNode ) {
		_nNode.nType = LSX_N_EMPTY;
		_nNode.stNodeIdx = size_t( -1 );
		_nNode.u.cContentObj.sLeft = size_t( -1 );
		_nNode.u.cContentObj.sRight = size_t( -1 );

		return size_t( -1 );;
	}

	/**
	 * Prints a node recursively.
	 *
	 * \param _stIdx The node index to print.
	 * \param _i32Depth The node depth.
	 */
	void CXmlContainer::PrintNode( size_t _stIdx, int32_t _i32Depth ) const {
		if ( _stIdx == size_t( -1 ) ) { return; }
		const YYSTYPE::LSX_NODE &nThisNode = m_vNodes[_stIdx];
		std::string sPrintMe;
		switch ( nThisNode.nType ) {
			case LSX_N_DOCUMENT : {
				if ( nThisNode.u.dDocumentObj.sProlog != size_t( -1 ) ) {
					sPrintMe = std::format( "{0: >{1}}DOCUMENT PROLOG:\r\n", "", _i32Depth * 5 );
					::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.dDocumentObj.sProlog, _i32Depth + 1 );
				}
				if ( nThisNode.u.dDocumentObj.sElement != size_t( -1 ) ) {
					sPrintMe = std::format( "{0: >{1}}DOCUMENT ELEMENT:\r\n", "", _i32Depth * 5 );
					::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.dDocumentObj.sElement, _i32Depth + 1 );
				}
				if ( nThisNode.u.dDocumentObj.sMiscSeq != size_t( -1 ) ) {
					sPrintMe = std::format( "{0: >{1}}DOCUMENT MISC:\r\n", "", _i32Depth * 5 );
					::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.dDocumentObj.sMiscSeq, _i32Depth + 1 );
				}
				break;
			}
			case LSX_N_PROLOG : {
				if ( nThisNode.u.pPrologObj.sVersion != size_t( -1 ) ) {
					sPrintMe = std::format( "{0: >{1}}PROLOG VERSION:\r\n", "", _i32Depth * 5 );
					::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.pPrologObj.sVersion, _i32Depth + 1 );
				}
				if ( nThisNode.u.pPrologObj.sEncoding != size_t( -1 ) ) {
					sPrintMe = std::format( "{0: >{1}}PROLOG ENCODING:\r\n", "", _i32Depth * 5 );
					::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.pPrologObj.sEncoding, _i32Depth + 1 );
				}
				if ( nThisNode.u.pPrologObj.sMiscSeq != size_t( -1 ) ) {
					sPrintMe = std::format( "{0: >{1}}PROLOG MISC:\r\n", "", _i32Depth * 5 );
					::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.pPrologObj.sMiscSeq, _i32Depth + 1 );
				}
				break;
			}
			case LSX_N_VERSION : {
				sPrintMe = std::format( "{0: >{1}}VERSION: {2}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.vVersionObj.sVersionStr] );
				::OutputDebugStringA( sPrintMe.c_str() );
				break;
			}
			case LSX_N_ENCODING : {
				sPrintMe = std::format( "{0: >{1}}ENCODING: {2}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.eEncodingObj.sEncodingStr] );
				::OutputDebugStringA( sPrintMe.c_str() );
				break;
			}
			case LSX_N_MISC_SEQ : {
				if ( nThisNode.u.msMiscSeqObj.sLeft != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}MISC SEQ LEFT:\r\n", "", _i32Depth * 5 );
					//::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.msMiscSeqObj.sLeft, _i32Depth + 0 );
				}
				if ( nThisNode.u.msMiscSeqObj.sRight != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}MISC SEQ RIGHT:\r\n", "", _i32Depth * 5 );
					//::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.msMiscSeqObj.sRight, _i32Depth + 0 );
				}
				break;
			}
			case LSX_N_ELEMENT : {
				sPrintMe = std::format( "{0: >{1}}ELEMENT ATTRIBUTE: {2}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.eElementObj.sName] );
				::OutputDebugStringA( sPrintMe.c_str() );
				PrintNode( nThisNode.u.eElementObj.sAttribute, _i32Depth + 1 );
				if ( nThisNode.u.eElementObj.sContent != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}ELEMENT CONTENT:\r\n", "", _i32Depth * 5 );
					//::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.eElementObj.sContent, _i32Depth + 1 );
				}
				break;
			}
			case LSX_N_CONTENT : {
				if ( nThisNode.u.cContentObj.sLeft != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}CONTENT LEFT:\r\n", "", _i32Depth * 5 );
					//::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.cContentObj.sLeft, _i32Depth + 0 );
				}
				if ( nThisNode.u.cContentObj.sRight != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}CONTENT RIGHT: {2}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.cContentObj.sRight] );
					//::OutputDebugStringA( sPrintMe.c_str() );
				}
				break;
			}
			case LSX_N_CONTENT_ELEMENT : {
				if ( nThisNode.u.cContentObj.sLeft != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}CONTENT ELEMENT LEFT:\r\n", "", _i32Depth * 5 );
					//::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.cContentObj.sLeft, _i32Depth + 0 );
				}
				if ( nThisNode.u.cContentObj.sRight != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}CONTENT ELEMENT RIGHT:\r\n", "", _i32Depth * 5 );
					//::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.cContentObj.sRight, _i32Depth + 0 );
				}
				break;
			}
			case LSX_N_CONTENT_MISC : {
				if ( nThisNode.u.cContentObj.sLeft != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}CONTENT MISC LEFT:\r\n", "", _i32Depth * 5 );
					//::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.cContentObj.sLeft, _i32Depth + 0 );
				}
				if ( nThisNode.u.cContentObj.sRight != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}CONTENT MISC RIGHT:\r\n", "", _i32Depth * 5 );
					//::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.cContentObj.sRight, _i32Depth + 0 );
				}
				break;
			}
			case LSX_N_CONTENT_DATA : {
				if ( nThisNode.u.cContentObj.sLeft != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}CONTENT DATA LEFT:\r\n", "", _i32Depth * 5 );
					//::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.cContentObj.sLeft, _i32Depth + 0 );
				}
				if ( nThisNode.u.cContentObj.sRight != size_t( -1 ) ) {
					sPrintMe = std::format( "{0: >{1}}CONTENT DATA RIGHT: {2}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.cContentObj.sRight] );
					::OutputDebugStringA( sPrintMe.c_str() );
				}
				break;
			}
			case LSX_N_ATTRIBUTE_DECL : {
				sPrintMe = std::format( "{0: >{1}}ATTR DECL: {2}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.aAttributeDeclObj.sName] );
				::OutputDebugStringA( sPrintMe.c_str() );
				PrintNode( nThisNode.u.aAttributeDeclObj.sList, _i32Depth + 1 );
				break;
			}
			case LSX_N_ATTRIBUTE_LIST : {
				if ( nThisNode.u.aAttributeListObj.sLeft != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}ATTR LIST LEFT:\r\n", "", _i32Depth * 5 );
					//::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.aAttributeListObj.sLeft, _i32Depth + 0 );
				}
				if ( nThisNode.u.aAttributeListObj.sRight != size_t( -1 ) ) {
					//sPrintMe = std::format( "{0: >{1}}ATTR LIST RIGHT:\r\n", "", _i32Depth * 5 );
					//::OutputDebugStringA( sPrintMe.c_str() );
					PrintNode( nThisNode.u.aAttributeListObj.sRight, _i32Depth + 0 );
				}
				break;
			}
			case LSX_N_ATTRIBUTE : {
				if ( nThisNode.u.aAttributeObj.sValue != size_t( -1 ) ) {
					sPrintMe = std::format( "{0: >{1}}ATTR: {2} = {3}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.aAttributeObj.sAttribute], m_vStrings[nThisNode.u.aAttributeObj.sValue] );
					::OutputDebugStringA( sPrintMe.c_str() );
				}
				else {
					sPrintMe = std::format( "{0: >{1}}ATTR: {2}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.aAttributeObj.sAttribute] );
					::OutputDebugStringA( sPrintMe.c_str() );
				}
				break;
			}
		}
	}

	/**
	 * Adds a node and returns its index into the array of nodes.
	 *
	 * \param _nNode The node to add.
	 * \return Returns the index of the added node inside m_vNodes.
	 */
	size_t CXmlContainer::AddNode( YYSTYPE::LSX_NODE &_nNode ) {
		_nNode.stNodeIdx = m_vNodes.size();
		m_vNodes.push_back( _nNode );
		return _nNode.stNodeIdx;
	}

}	// namespace lsx

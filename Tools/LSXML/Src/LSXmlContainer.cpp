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
					sPrintMe = std::format( "{0: >{1}}CONTENT RIGHT: {2}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.cContentObj.sRight] );
					::OutputDebugStringA( sPrintMe.c_str() );
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
	 * Builds the XML tree non-recursively.
	 *
	 * \return Returns true if there were no memory errors during tree creation.
	 */
	bool CXmlContainer::BuildTree() {
		if ( m_stRoot == size_t( -1 ) ) { return false; }
		while ( m_tRoot.Size() ) {
			m_tRoot.RemoveChild( 0 );
		}
		struct LSX_STACK_OBJ {
			size_t					stNodeIdx;
			bool					bPassThrough = false;

			LSX_STACK_OBJ( size_t _stIdx ) :
				stNodeIdx( _stIdx ) {
			}
		};
		std::vector<LSX_STACK_OBJ> vStack;
		std::vector<CTree<LSX_XML_ELEMENT> *> vCurElement;
		try {
#define LXM_PUSH( IDX )			vStack.push_back( LSX_STACK_OBJ( m_vNodes[stNodeIdx].u.IDX ) )
			vStack.reserve( m_vNodes.size() + 1 );
			vStack.push_back( LSX_STACK_OBJ( m_stRoot ) );
			while ( vStack.size() ) {
				size_t stThis = vStack.size() - 1;
				bool bFirstPass = !vStack[stThis].bPassThrough;
				vStack[stThis].bPassThrough = true;
				size_t stNodeIdx = vStack[stThis].stNodeIdx;
				if ( stNodeIdx == size_t( -1 ) ) {
					vStack.pop_back();
					continue;
				}
				if ( bFirstPass ) {
					// Push in revese order (top gets processed sooner).
					switch ( m_vNodes[stNodeIdx].nType ) {
						case LSX_N_DOCUMENT : {
							LXM_PUSH( dDocumentObj.sMiscSeq );
							LXM_PUSH( dDocumentObj.sElement );
							LXM_PUSH( dDocumentObj.sProlog );
							break;
						}
						case LSX_N_PROLOG : {
							LXM_PUSH( pPrologObj.sMiscSeq );
							LXM_PUSH( pPrologObj.sEncoding );
							LXM_PUSH( pPrologObj.sVersion );
							break;
						}
						case LSX_N_VERSION : {
							// Store the version somewhere.
							break;
						}
						case LSX_N_ENCODING : {
							// Store the encoding somewhere.
							break;
						}
						case LSX_N_MISC_SEQ : {
							LXM_PUSH( msMiscSeqObj.sRight );
							LXM_PUSH( msMiscSeqObj.sLeft );
							break;
						}
						case LSX_N_ELEMENT : {
							CTree<LSX_XML_ELEMENT> * ptParent = CurStackPointer( vCurElement );
							LSX_XML_ELEMENT eElement;
							eElement.stNameString = m_vNodes[stNodeIdx].u.eElementObj.sName;
							ptParent->InsertChild( eElement, ptParent->Size() );
							vCurElement.push_back( ptParent->GetChild( ptParent->Size() - 1 ) );

							LXM_PUSH( eElementObj.sContent );
							LXM_PUSH( eElementObj.sAttribute );
							break;
						}
						case LSX_N_CONTENT : {
							LXM_PUSH( cContentObj.sLeft );
							break;
						}
						case LSX_N_CONTENT_ELEMENT : {
							LXM_PUSH( cContentObj.sRight );
							LXM_PUSH( cContentObj.sLeft );
							break;
						}
						case LSX_N_CONTENT_MISC : {
							LXM_PUSH( cContentObj.sRight );
							LXM_PUSH( cContentObj.sLeft );
							break;
						}
						case LSX_N_CONTENT_DATA : {
							if ( m_vNodes[stNodeIdx].u.cContentObj.sRight != size_t( -1 ) ) {
								if ( !IsWhitespace( m_vStrings[m_vNodes[stNodeIdx].u.cContentObj.sRight] ) ) {
									CTree<LSX_XML_ELEMENT> * ptParent = CurStackPointer( vCurElement );
									ptParent->Value().sData.append( m_vStrings[m_vNodes[stNodeIdx].u.cContentObj.sRight] );
								}
							}
							LXM_PUSH( cContentObj.sLeft );
							break;
						}
						case LSX_N_ATTRIBUTE_DECL : {
							/*CTree<LSX_XML_ELEMENT> * ptParent = CurStackPointer( vCurElement );
							ptParent->Value().*/
							// TODO: Haven’t encountered this case yet.
							LXM_PUSH( aAttributeDeclObj.sList );
							break;
						}
						case LSX_N_ATTRIBUTE_LIST : {
							LXM_PUSH( aAttributeListObj.sRight );
							LXM_PUSH( aAttributeListObj.sLeft );
							break;
						}
						case LSX_N_ATTRIBUTE : {
							CTree<LSX_XML_ELEMENT> * ptParent = CurStackPointer( vCurElement );
							LSX_XML_ATTRIBUTE xaAttr = { m_vNodes[stNodeIdx].u.aAttributeObj.sAttribute, m_vNodes[stNodeIdx].u.aAttributeObj.sValue };
							ptParent->Value().vAttributes.push_back( xaAttr );
							break;
						}
					}
				}
				else {
					switch ( m_vNodes[stNodeIdx].nType ) {
						case LSX_N_ELEMENT : {
							vCurElement.pop_back();
							break;
						}
					}
					vStack.pop_back();
				}
			}
#undef LXM_PUSH
			return true;
		}
		catch ( ... ) {

		}
		return false;
	}

	/**
	 * Prints the tree recursively.
	 */
	void CXmlContainer::PrintTree() {
		PrintTree( Next( nullptr ), 0 );
	}

	/**
	 * Gets the next item.
	 *
	 * \param _ptThis The items whose next item is to be obtained.
	 * \return Returns the next item.
	 */
	CTree<CXmlContainer::LSX_XML_ELEMENT> * CXmlContainer::Next( CTree<LSX_XML_ELEMENT> * _ptThis ) {
		if ( !_ptThis ) {
			if ( !m_tRoot.Size() ) { return nullptr; }		// We skip the very rootest node.
			return const_cast<CTree<LSX_XML_ELEMENT> *>(m_tRoot.GetChild( 0 ));
		}
		return CTree<LSX_XML_ELEMENT>::Next( _ptThis );
	}

	/**
	 * Gets the previous item.
	 *
	 * \param _ptThis The items whose previous item is to be obtained.
	 * \return Returns the previous item.
	 */
	CTree<CXmlContainer::LSX_XML_ELEMENT> * CXmlContainer::Prev( CTree<LSX_XML_ELEMENT> * _ptThis ) {
		if ( !_ptThis ) {
			if ( !m_tRoot.Size() ) { return nullptr; }
			_ptThis = &m_tRoot;
			while ( _ptThis ) {
				if ( _ptThis->Size() ) {
					_ptThis = _ptThis->GetChild( _ptThis->Size() - 1 );
				}
				else { break; }
			}
			return _ptThis;
		}
		_ptThis = CTree<LSX_XML_ELEMENT>::Prev( _ptThis );
		if ( _ptThis == &m_tRoot ) { _ptThis = nullptr; }	// We skip the very rootest node.
		return _ptThis;
	}

	/**
	 * Gets the first child element by name.
	 *
	 * \param _ptParent The node whose children are to be scanned for elements matching the given name.
	 * \param _sName The name of the child element to find.
	 * \return Returns a pointer to the child element of te given name or nullptr if there is none.
	 */
	const CTree<CXmlContainer::LSX_XML_ELEMENT> * CXmlContainer::GetChildElement( const CTree<LSX_XML_ELEMENT> * _ptParent, const std::string &_sName ) const {
		if ( _ptParent ) {
			for ( size_t I = 0; I < _ptParent->Size(); ++I ) {
				if ( GetString( _ptParent->GetChild( I )->Value().stNameString ) == _sName ) {
					return _ptParent->GetChild( I );
				}
			}
		}
		return nullptr;
	}

	/**
	 * Gets the first child element by name.
	 *
	 * \param _ptParent The node whose children are to be scanned for elements matching the given name.
	 * \param _sName The name of the child element to find.
	 * \return Returns a pointer to the child element of te given name or nullptr if there is none.
	 */
	CTree<CXmlContainer::LSX_XML_ELEMENT> * CXmlContainer::GetChildElement( CTree<LSX_XML_ELEMENT> * _ptParent, const std::string &_sName ) {
		if ( _ptParent ) {
			for ( size_t I = 0; I < _ptParent->Size(); ++I ) {
				if ( GetString( _ptParent->GetChild( I )->Value().stNameString ) == _sName ) {
					return _ptParent->GetChild( I );
				}
			}
		}
		return nullptr;
	}

	/**
	 * Gathers the indices of children nodes (non-recursively) whose element names match the given name.
	 *
	 * \param _ptParent The node whose children are to be scanned for elements matching the given name.
	 * \param _sName The name of the children elements to gather.
	 * \return Returns an array of indices indicating the children of _ptParent whose element names match _sName.
	 */
	std::vector<size_t> CXmlContainer::GatherChildElementIndices( const CTree<LSX_XML_ELEMENT> * _ptParent, const std::string &_sName ) const {
		std::vector<size_t> vRet;
		if ( !_ptParent ) { return vRet; }
		for ( size_t I = 0; I < _ptParent->Size(); ++I ) {
			if ( GetString( _ptParent->GetChild( I )->Value().stNameString ) == _sName ) {
				vRet.push_back( I );
			}
		}
		return vRet;
	}

	/**
	 * Gets the value of an attribute on a given element.
	 *
	 * \param _ptElement The element whose attributes are to be searched.
	 * \param _sName The name of the attribute whose value is to be found.
	 * \param _sRet Holds the returned value of the attribute.
	 * \return Returns true if the given attribute was found on the given item.
	 */
	bool CXmlContainer::GetAttributeValue( const CTree<LSX_XML_ELEMENT> * _ptElement, const std::string &_sName, std::string &_sRet ) const {
		_sRet.clear();
		if ( !_ptElement ) { return false; }
		for ( auto I = _ptElement->Value().vAttributes.size(); I--; ) {
			if ( GetString( _ptElement->Value().vAttributes[I].stNameString ) == _sName ) {
				if ( _ptElement->Value().vAttributes[I].stValueString != size_t( -1 ) ) {
					_sRet = GetString( _ptElement->Value().vAttributes[I].stValueString );
				}
				return true;
			}
		}
		return false;
	}

	/**
	 * Gets the data of an child element given its name.
	 *
	 * \param _ptParent The element whose child elements are to be searched.
	 * \param _sName The name of the child element whose value is to be found.
	 * \param _sRet Holds the returned data value of the element.
	 * \return Returns true if the given child element was found on the given item.
	 */
	bool CXmlContainer::GetChildElementData( const CTree<LSX_XML_ELEMENT> * _ptParent, const std::string &_sName, std::string &_sRet ) const {
		_sRet.clear();
		if ( !_ptParent ) { return false; }
		for ( size_t I = 0; I < _ptParent->Size(); ++I ) {
			if ( GetString( _ptParent->GetChild( I )->Value().stNameString ) == _sName ) {
				_sRet = _ptParent->GetChild( I )->Value().sData;
				return true;
			}
		}
		return false;
	}

	/**
	 * Gathers every attribute and their values into a multi-map.
	 *
	 * \param _mmDst The destination multi-map.
	 * \return Returns true if there were no memory errors.
	 */
	bool CXmlContainer::GatherAttributes( std::multimap<std::string, std::string> &_mmDst ) {
		try {
			CTree<CXmlContainer::LSX_XML_ELEMENT> * ptItem = Next( nullptr );
			while ( ptItem ) {
				for ( size_t I = ptItem->Value().vAttributes.size(); I--; ) {
					/*auto aThis = _mmDst.find( GetString( ptItem->Value().vAttributes[I].stNameString ) );
					if ( aThis == _mmDst.end() ) {
						aThis = _mmDst.insert( std::pair( GetString( ptItem->Value().vAttributes[I].stNameString ) );
					}*/
					if ( ptItem->Value().vAttributes[I].stValueString == size_t( -1 ) ) {
						_mmDst.insert( std::pair( GetString( ptItem->Value().vAttributes[I].stNameString ), std::string() ) );
					}
					else {
						_mmDst.insert( std::pair( GetString( ptItem->Value().vAttributes[I].stNameString ), GetString( ptItem->Value().vAttributes[I].stValueString ) ) );
					}
				}

				ptItem = Next( ptItem );
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Gathers every attribute and their values into a map.
	 *
	 * \param _mDst The destination map.
	 * \return Returns true if there were no memory errors.
	 */
	bool CXmlContainer::GatherAttributes( std::map<std::string, std::set<std::string>> &_mDst ) {
		try {
			CTree<CXmlContainer::LSX_XML_ELEMENT> * ptItem = Next( nullptr );
			while ( ptItem ) {
				for ( size_t I = ptItem->Value().vAttributes.size(); I--; ) {
					auto aThis = _mDst.find( GetString( ptItem->Value().vAttributes[I].stNameString ) );
					if ( aThis == _mDst.end() ) {
						std::string sTmp = GetString( ptItem->Value().vAttributes[I].stNameString );
						_mDst.insert( std::pair( sTmp, std::set<std::string>() ) );
						aThis = _mDst.find( GetString( ptItem->Value().vAttributes[I].stNameString ) );
					}
					if ( ptItem->Value().vAttributes[I].stValueString != size_t( -1 ) ) {
						std::string sTmp = GetString( ptItem->Value().vAttributes[I].stValueString );
						aThis->second.insert( sTmp );
					}
				}

				ptItem = Next( ptItem );
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Gathers every element and their data values into a map.
	 *
	 * \param _mDst The destination map.
	 * \return Returns true if there were no memory errors.
	 */
	bool CXmlContainer::GatherElements( std::map<std::string, std::set<std::string>> &_mDst ) {
		try {
			CTree<CXmlContainer::LSX_XML_ELEMENT> * ptItem = Next( nullptr );
			while ( ptItem ) {
				auto aThis = _mDst.find( GetString( ptItem->Value().stNameString ) );
				if ( aThis == _mDst.end() ) {
					std::string sTmp = GetString( ptItem->Value().stNameString );
					_mDst.insert( std::pair( sTmp, std::set<std::string>() ) );
					aThis = _mDst.find( GetString( ptItem->Value().stNameString ) );
				}
				if ( ptItem->Value().sData.size() ) {
					aThis->second.insert( ptItem->Value().sData );
				}

				ptItem = Next( ptItem );
			}
			return true;
		}
		catch ( ... ) { return false; }
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

	/**
	 * Determines if a string is entirely whitespace.
	 *
	 * \param _sString The string to check.
	 * \return Returns true if the given string is entirely whitespace ([ \t\r\n]+).
	 */
	bool CXmlContainer::IsWhitespace( const std::string & _sString ) {
		for ( auto I = _sString.size(); I--; ) {
			if ( _sString[I] != ' ' &&
				_sString[I] != '\t' &&
				_sString[I] != '\r' &&
				_sString[I] != '\n' ) { return false; }
		}
		return true;
	}

	/**
	 * Prints the tree recursively.
	 * 
	 * \param _ptNode The node to print.
	 * \param _i32Depth The node depth.
	 */
	void CXmlContainer::PrintTree( const CTree<LSX_XML_ELEMENT> * _ptNode, int32_t _i32Depth ) {
		std::string sPrintMe;
		if ( _ptNode->Value().stNameString != size_t( -1 ) ) {
			sPrintMe = std::format( "{0: >{1}}Node: {2}\r\n", "", _i32Depth * 5, GetString( _ptNode->Value().stNameString ) );
			::OutputDebugStringA( sPrintMe.c_str() );
		}
		for ( size_t I = 0; I < _ptNode->Value().vAttributes.size(); ++I ) {
			if ( _ptNode->Value().vAttributes[I].stValueString == size_t( -1 ) ) {
				sPrintMe = std::format( "{0: >{1}}{2}\r\n", "", (_i32Depth + 1) * 5, GetString( _ptNode->Value().vAttributes[I].stNameString ) );
			}
			else {
				sPrintMe = std::format( "{0: >{1}}{2} = {3}\r\n", "", (_i32Depth + 1) * 5, GetString( _ptNode->Value().vAttributes[I].stNameString ), GetString( _ptNode->Value().vAttributes[I].stValueString ) );
			}
			::OutputDebugStringA( sPrintMe.c_str() );
		}
		if ( _ptNode->Value().sData.size() ) {
			sPrintMe = std::format( "{0: >{1}}{2}\r\n", "", (_i32Depth + 1) * 5, _ptNode->Value().sData );
			::OutputDebugStringA( sPrintMe.c_str() );
		}

		for ( size_t I = 0; I < _ptNode->Size(); ++I ) {
			PrintTree( _ptNode->GetChild( I ), _i32Depth + 1 );
		}
	}

}	// namespace lsx

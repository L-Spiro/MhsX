// TMP.
#include <Windows.h>

#include "LSONJsonContainer.h"
#include "LSONJsonLexer.h"


#include <format>


namespace lson {

	CJsonContainer::CJsonContainer( CJsonLexer * _pxlLexer ) :
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
	size_t CJsonContainer::AddString( const std::string &_sText ) {
		size_t stRet = FindString( _sText );
		if ( stRet == size_t( -1 ) ) {
			m_vStrings.push_back( _sText );
			return m_vStrings.size() - 1;
		}
		return stRet;
	}

	/**
	 * Removes quotes from the front and end before adding the string.
	 *
	 * \param _sText The string to add or whose existing index is to be found.
	 * \return Returns the index of the added string after stripping the enclosing quotes from it.
	 */
	size_t CJsonContainer::AddQuoteString( const std::string &_sText ) {
		return AddString( _sText.substr( 1, _sText.size() - 2 ) );
	}

	/**
	 * Returns the index of a string or -1 if it does not exist.
	 *
	 * \param _sText The string to find.
	 * \return Returns the index of the string if it exists or size_t( -1 ).
	 */
	size_t CJsonContainer::FindString( const std::string &_sText ) const {
		for ( auto I = m_vStrings.size(); I--; ) {
			if ( m_vStrings[I] == _sText ) { return I; }
		}
		return size_t( -1 );
	}

	/**
	 * Creates an object value node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nValue The value.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddObjectValue( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nValue ) {
		_nNode.nType = LSON_N_VALUE;
		_nNode.u.vValue.vType = LSON_V_OBJECT;
		_nNode.u.vValue.v.stValue = _nValue.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates a string value node.
	 *
	 * \param _nNode The resulting node.
	 * \param _stValue The string.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddStringValue( YYSTYPE::LSON_NODE &_nNode, size_t _stValue ) {
		_nNode.nType = LSON_N_VALUE;
		_nNode.u.vValue.vType = LSON_V_STRING;
		_nNode.u.vValue.v.stValue = _stValue;

		return AddNode( _nNode );
	}

	/**
	 * Creates a decimal value node.
	 *
	 * \param _nNode The resulting node.
	 * \param _dValue The value.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddDecimalValue( YYSTYPE::LSON_NODE &_nNode, double _dValue ) {
		_nNode.nType = LSON_N_VALUE;
		_nNode.u.vValue.vType = LSON_V_DECIMAL;
		_nNode.u.vValue.v.dDecimal = _dValue;

		return AddNode( _nNode );
	}

	/**
	 * Creates an array value node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nValue The value.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddArrayValue( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nValue ) {
		_nNode.nType = LSON_N_VALUE;
		_nNode.u.vValue.vType = LSON_V_ARRAY;
		_nNode.u.vValue.v.stValue = _nValue.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates a TRUE value node.
	 *
	 * \param _nNode The resulting node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddTrueValue( YYSTYPE::LSON_NODE &_nNode ) {
		_nNode.nType = LSON_N_VALUE;
		_nNode.u.vValue.vType = LSON_V_TRUE;
		_nNode.u.vValue.v.dDecimal = 0.0;

		return AddNode( _nNode );
	}

	/**
	 * Creates a FALSE value node.
	 *
	 * \param _nNode The resulting node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddFalseValue( YYSTYPE::LSON_NODE &_nNode ) {
		_nNode.nType = LSON_N_VALUE;
		_nNode.u.vValue.vType = LSON_V_FALSE;
		_nNode.u.vValue.v.dDecimal = 0.0;

		return AddNode( _nNode );
	}

	/**
	 * Creates a NULL value node.
	 *
	 * \param _nNode The resulting node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddNullValue( YYSTYPE::LSON_NODE &_nNode ) {
		_nNode.nType = LSON_N_VALUE;
		_nNode.u.vValue.vType = LSON_V_NULL;
		_nNode.u.vValue.v.dDecimal = 0.0;

		return AddNode( _nNode );
	}

	/**
	 * Creates a member node.
	 *
	 * \param _nNode The resulting node.
	 * \param _stName The name of the member.
	 * \param _nValue The value.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddMember( YYSTYPE::LSON_NODE &_nNode, size_t _stName, const YYSTYPE::LSON_NODE &_nValue ) {
		_nNode.nType = LSON_N_MEMBER;
		_nNode.u.mMember.stName = _stName;
		_nNode.u.mMember.stValue = _nValue.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates a member-list node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nLeft The left node.
	 * \param _nRight The right node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddMemberList( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight ) {
		_nNode.nType = LSON_N_MEMBER_LIST;
		_nNode.u.mlMembers.stLeft = _nLeft.stNodeIdx;
		_nNode.u.mlMembers.stRight = _nRight.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates an empty member-list node.
	 *
	 * \param _nNode The resulting node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddMemberList( YYSTYPE::LSON_NODE &_nNode ) {
		_nNode.nType = LSON_N_MEMBER_LIST;
		_nNode.u.mlMembers.stLeft = size_t( -1 );
		_nNode.u.mlMembers.stRight = size_t( -1 );

		return AddNode( _nNode );
	}

	/**
	 * Creates an object node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nMembers The members node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddObject( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nMembers ) {
		_nNode.nType = LSON_N_OBJECT;
		_nNode.u.oObject.stMembers = _nMembers.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates an object node with no members.
	 *
	 * \param _nNode The resulting node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddObject( YYSTYPE::LSON_NODE &_nNode ) {
		_nNode.nType = LSON_N_OBJECT;
		_nNode.u.oObject.stMembers = size_t( -1 );

		return AddNode( _nNode );
	}

	/**
	 * Creates a value-list node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nLeft The left node.
	 * \param _nRight The right node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddValueList( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight ) {
		_nNode.nType = LSON_N_VALUE_LIST;
		_nNode.u.vlValues.stLeft = _nLeft.stNodeIdx;
		_nNode.u.vlValues.stRight = _nRight.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates an empty value-list node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nLeft The left node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddValueList( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft ) {
		_nNode.nType = LSON_N_VALUE_LIST;
		_nNode.u.vlValues.stLeft = _nLeft.stNodeIdx;
		_nNode.u.vlValues.stRight = size_t( -1 );

		return AddNode( _nNode );
	}

	/**
	 * Creates an array node.
	 *
	 * \param _nNode The resulting node.
	 * \param _nValues The values node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddArray( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nValues ) {
		_nNode.nType = LSON_N_ARRAY;
		_nNode.u.aArray.stValues = _nValues.stNodeIdx;

		return AddNode( _nNode );
	}

	/**
	 * Creates an empty array node.
	 *
	 * \param _nNode The resulting node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddArray( YYSTYPE::LSON_NODE &_nNode ) {
		_nNode.nType = LSON_N_ARRAY;
		_nNode.u.aArray.stValues = size_t( -1 );

		return AddNode( _nNode );
	}

	/**
	 * Creates the JSON node.
	 *
	 * \param _nNode The resulting node.
	 */
	void CJsonContainer::AddJson( YYSTYPE::LSON_NODE &_nNode ) {
		m_stRoot = _nNode.stNodeIdx;
	}

	/**
	 * Builds the tree that can be used to access data in the JSON.
	 * 
	 * \return Returns true if the tree was successfully built.
	 */
	bool CJsonContainer::BuildTree() {
		if ( m_stRoot == size_t( -1 ) ) { return false; }
		/*while ( m_tRoot.Size() ) {
			m_tRoot.RemoveChild( 0 );
		}*/
		struct LSON_STACK_OBJ {
			size_t *				pstReturn = nullptr;
			size_t					stNodeIdx;
			size_t					stParmResult[2];
			size_t					stResults = 0;
			//size_t					

			bool					bPassThrough = false;
			LSON_STACK_OBJ( size_t _stIdx ) :
				stNodeIdx( _stIdx ) {
			}
		};
		std::vector<LSON_STACK_OBJ> vStack;
		std::vector<size_t> vCurElement;
		try {
#define LSON_PUSH( IDX )			vStack.push_back( LSON_STACK_OBJ( m_vNodes[stNodeIdx].u.IDX ) )
#define LSON_PUSH_RESULT( IDX )		LSON_PUSH( IDX ); vStack[vStack.size()-1].pstReturn = &vStack[stThis].stParmResult[vStack[stThis].stResults++]

			vStack.reserve( m_vNodes.size() + 1 );
			vStack.push_back( LSON_STACK_OBJ( m_stRoot ) );
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
						case LSON_N_MEMBER : {
							LSON_PUSH_RESULT( mMember.stValue );
							break;
						}
						case LSON_N_MEMBER_LIST : {
							if ( m_vNodes[stNodeIdx].u.mlMembers.stRight != size_t( -1 ) ) {
								LSON_PUSH_RESULT( mlMembers.stRight );
							}
							LSON_PUSH_RESULT( mlMembers.stLeft );
							break;
						}
						case LSON_N_VALUE : {
							LSON_JSON_VALUE jvVal;
							switch ( m_vNodes[stNodeIdx].u.vValue.vType ) {
								case LSON_V_OBJECT : {
									jvVal.vtType = LSON_VT_OBJECT;
									LSON_PUSH( vValue.v.stValue );
									break;
								}
								case LSON_V_STRING : {
									jvVal.vtType = LSON_VT_STRING;
									jvVal.u.stString = m_vNodes[stNodeIdx].u.vValue.v.stValue;
#ifdef _DEBUG
									jvVal.sString = GetString( jvVal.u.stString );
#endif	// #ifdef _DEBUG
									break;
								}
								case LSON_V_DECIMAL : {
									jvVal.vtType = LSON_VT_DECIMAL;
									jvVal.u.dDecimal = m_vNodes[stNodeIdx].u.vValue.v.dDecimal;
									break;
								}
								case LSON_V_ARRAY : {
									jvVal.vtType = LSON_VT_ARRAY;
									LSON_PUSH( vValue.v.stValue );
									break;
								}
								case LSON_V_TRUE : {
									jvVal.vtType = LSON_VT_TRUE;
									break;
								}
								case LSON_V_FALSE : {
									jvVal.vtType = LSON_VT_FALSE;
									break;
								}
								case LSON_V_NULL : {
									jvVal.vtType = LSON_VT_NULL;
									break;
								}
							}
							vCurElement.push_back( AddValue( jvVal ) );
							LSON_JSON_VALUE * pjvCurVal = CurStackPointer( vCurElement );
							if ( pjvCurVal ) {
							}
							break;
						}
						case LSON_N_VALUE_LIST : {
							if ( m_vNodes[stNodeIdx].u.vlValues.stRight != size_t( -1 ) ) {
								LSON_PUSH_RESULT( vlValues.stRight );
							}
							LSON_PUSH_RESULT( vlValues.stLeft );
							break;
						}
						case LSON_N_OBJECT : {
							LSON_PUSH( oObject.stMembers );
							break;
						}
						case LSON_N_ARRAY : {
							if ( m_vNodes[stNodeIdx].u.aArray.stValues != size_t( -1 ) ) {
								LSON_PUSH( aArray.stValues );
							}
							break;
						}
					}
				}
				else {
					switch ( m_vNodes[stNodeIdx].nType ) {
						case LSON_N_MEMBER : {
							if ( vStack[stThis].pstReturn ) {
								(*vStack[stThis].pstReturn) = vCurElement[vCurElement.size()-1];
							}
							LSON_JSON_VALUE * pjvCurVal = CurStackPointer( vCurElement );
							if ( pjvCurVal ) {
								LSON_JSON_MEMBER jmMember;
								jmMember.stName = m_vNodes[stNodeIdx].u.mMember.stName;
								jmMember.stValue = vStack[stThis].stParmResult[0];
#ifdef _DEBUG
								jmMember.sName = GetString( jmMember.stName );
#endif	// #ifdef _DEBUG
								pjvCurVal->oObject.vMembers.push_back( jmMember );
							}
							break;
						}
						case LSON_N_MEMBER_LIST : {
							if ( vStack[stThis].pstReturn ) {
								(*vStack[stThis].pstReturn) = vCurElement[vCurElement.size()-1];
							}
							break;
						}
						case LSON_N_VALUE : {
							size_t stThisIdx = vCurElement[vCurElement.size()-1];
							if ( vStack[stThis].pstReturn ) {
								(*vStack[stThis].pstReturn) = stThisIdx;
							}
							LSON_JSON_VALUE * pjvCurVal2 = CurStackPointer( vCurElement );
							vCurElement.pop_back();

							LSON_JSON_VALUE * pjvCurVal = CurStackPointer( vCurElement );
							if ( pjvCurVal ) {
								if ( pjvCurVal->vtType == LSON_V_ARRAY ) {
									pjvCurVal->vArray.push_back( stThisIdx );
								}
							}

							break;
						}
						case LSON_N_VALUE_LIST : {
							if ( vStack[stThis].pstReturn ) {
								(*vStack[stThis].pstReturn) = vCurElement[vCurElement.size()-1];
							}
							break;
						}
						case LSON_N_OBJECT : {
							if ( vStack[stThis].pstReturn ) {
								(*vStack[stThis].pstReturn) = vCurElement[vCurElement.size()-1];
							}
							break;
						}
					}
					vStack.pop_back();
				}
			}

#undef LSON_PUSH_RESULT
#undef LSON_PUSH
			return true;
		}
		catch ( ... ) {

		}
		return false;
	}

#if 0
	/**
	 * Creates an attribute start string.
	 *
	 * \param _sText The name of the attribute.
	 * \return Returns the index of the string created.
	 */
	size_t CJsonContainer::AddAttributeStart( const std::string &_sText ) {
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
	size_t CJsonContainer::AddAttribute( YYSTYPE::LSON_NODE &_nNode, size_t _stName, size_t _stValue ) {
		_nNode.nType = LSON_N_ATTRIBUTE;
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
	size_t CJsonContainer::AddAttributeList( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight ) {
		_nNode.nType = LSON_N_ATTRIBUTE_LIST;
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
	size_t CJsonContainer::AddAttributeList( YYSTYPE::LSON_NODE &_nNode ) {
		_nNode.nType = LSON_N_ATTRIBUTE_LIST;
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
	size_t CJsonContainer::AddAttributeDecl( YYSTYPE::LSON_NODE &_nNode, size_t _stName, const YYSTYPE::LSON_NODE &_nAttributes ) {
		_nNode.nType = LSON_N_ATTRIBUTE_DECL;
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
	size_t CJsonContainer::AddContentData( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, size_t _stData ) {
		_nNode.nType = LSON_N_CONTENT_DATA;
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
	size_t CJsonContainer::AddContentMisc( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight ) {
		_nNode.nType = LSON_N_CONTENT_MISC;
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
	size_t CJsonContainer::AddContentElement( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight ) {
		_nNode.nType = LSON_N_CONTENT_ELEMENT;
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
	size_t CJsonContainer::AddContent( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nContent, size_t _stName ) {
		_nNode.nType = LSON_N_CONTENT;
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
	size_t CJsonContainer::AddElement( YYSTYPE::LSON_NODE &_nNode, size_t _stName, const YYSTYPE::LSON_NODE &_nAttributes, const YYSTYPE::LSON_NODE &_nContent ) {
		_nNode.nType = LSON_N_ELEMENT;
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
	size_t CJsonContainer::AddMiscSeq( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft, const YYSTYPE::LSON_NODE &_nRight ) {
		_nNode.nType = LSON_N_MISC_SEQ;
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
	size_t CJsonContainer::AddEncoding( YYSTYPE::LSON_NODE &_nNode, const char * _pcEncoding ) {
		_nNode.nType = LSON_N_ENCODING;
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
	size_t CJsonContainer::AddVersion( YYSTYPE::LSON_NODE &_nNode, const char * _pcVersion ) {
		_nNode.nType = LSON_N_VERSION;
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
	size_t CJsonContainer::AddProlog( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nVersion, const YYSTYPE::LSON_NODE &_nEncoding, const YYSTYPE::LSON_NODE &_nMisc ) {
		_nNode.nType = LSON_N_PROLOG;
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
	size_t CJsonContainer::AddDocument( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nProlog, const YYSTYPE::LSON_NODE &_nElement, const YYSTYPE::LSON_NODE &_nMisc ) {
		_nNode.nType = LSON_N_DOCUMENT;
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
	size_t CJsonContainer::AddEmpty( YYSTYPE::LSON_NODE &_nNode ) {
		_nNode.nType = LSON_N_EMPTY;
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
	void CJsonContainer::PrintNode( size_t _stIdx, int32_t _i32Depth ) const {
		if ( _stIdx == size_t( -1 ) ) { return; }
		const YYSTYPE::LSON_NODE &nThisNode = m_vNodes[_stIdx];
		std::string sPrintMe;
		switch ( nThisNode.nType ) {
			case LSON_N_DOCUMENT : {
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
			case LSON_N_PROLOG : {
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
			case LSON_N_VERSION : {
				sPrintMe = std::format( "{0: >{1}}VERSION: {2}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.vVersionObj.sVersionStr] );
				::OutputDebugStringA( sPrintMe.c_str() );
				break;
			}
			case LSON_N_ENCODING : {
				sPrintMe = std::format( "{0: >{1}}ENCODING: {2}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.eEncodingObj.sEncodingStr] );
				::OutputDebugStringA( sPrintMe.c_str() );
				break;
			}
			case LSON_N_MISC_SEQ : {
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
			case LSON_N_ELEMENT : {
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
			case LSON_N_CONTENT : {
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
			case LSON_N_CONTENT_ELEMENT : {
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
			case LSON_N_CONTENT_MISC : {
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
			case LSON_N_CONTENT_DATA : {
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
			case LSON_N_ATTRIBUTE_DECL : {
				sPrintMe = std::format( "{0: >{1}}ATTR DECL: {2}\r\n", "", _i32Depth * 5, m_vStrings[nThisNode.u.aAttributeDeclObj.sName] );
				::OutputDebugStringA( sPrintMe.c_str() );
				PrintNode( nThisNode.u.aAttributeDeclObj.sList, _i32Depth + 1 );
				break;
			}
			case LSON_N_ATTRIBUTE_LIST : {
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
			case LSON_N_ATTRIBUTE : {
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
	 * Builds the JSON tree non-recursively.
	 *
	 * \return Returns true if there were no memory errors during tree creation.
	 */
	bool CJsonContainer::BuildTree() {
		if ( m_stRoot == size_t( -1 ) ) { return false; }
		while ( m_tRoot.Size() ) {
			m_tRoot.RemoveChild( 0 );
		}
		struct LSON_STACK_OBJ {
			size_t					stNodeIdx;
			bool					bPassThrough = false;

			LSON_STACK_OBJ( size_t _stIdx ) :
				stNodeIdx( _stIdx ) {
			}
		};
		std::vector<LSON_STACK_OBJ> vStack;
		std::vector<CTree<LSON_JSON_ELEMENT> *> vCurElement;
		try {
#define LSON_PUSH( IDX )			vStack.push_back( LSON_STACK_OBJ( m_vNodes[stNodeIdx].u.IDX ) )
			vStack.reserve( m_vNodes.size() + 1 );
			vStack.push_back( LSON_STACK_OBJ( m_stRoot ) );
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
						case LSON_N_DOCUMENT : {
							LSON_PUSH( dDocumentObj.sMiscSeq );
							LSON_PUSH( dDocumentObj.sElement );
							LSON_PUSH( dDocumentObj.sProlog );
							break;
						}
						case LSON_N_PROLOG : {
							LSON_PUSH( pPrologObj.sMiscSeq );
							LSON_PUSH( pPrologObj.sEncoding );
							LSON_PUSH( pPrologObj.sVersion );
							break;
						}
						case LSON_N_VERSION : {
							// Store the version somewhere.
							break;
						}
						case LSON_N_ENCODING : {
							// Store the encoding somewhere.
							break;
						}
						case LSON_N_MISC_SEQ : {
							LSON_PUSH( msMiscSeqObj.sRight );
							LSON_PUSH( msMiscSeqObj.sLeft );
							break;
						}
						case LSON_N_ELEMENT : {
							CTree<LSON_JSON_ELEMENT> * ptParent = CurStackPointer( vCurElement );
							LSON_JSON_ELEMENT eElement;
							eElement.stNameString = m_vNodes[stNodeIdx].u.eElementObj.sName;
							ptParent->InsertChild( eElement, ptParent->Size() );
							vCurElement.push_back( ptParent->GetChild( ptParent->Size() - 1 ) );

							LSON_PUSH( eElementObj.sContent );
							LSON_PUSH( eElementObj.sAttribute );
							break;
						}
						case LSON_N_CONTENT : {
							LSON_PUSH( cContentObj.sLeft );
							break;
						}
						case LSON_N_CONTENT_ELEMENT : {
							LSON_PUSH( cContentObj.sRight );
							LSON_PUSH( cContentObj.sLeft );
							break;
						}
						case LSON_N_CONTENT_MISC : {
							LSON_PUSH( cContentObj.sRight );
							LSON_PUSH( cContentObj.sLeft );
							break;
						}
						case LSON_N_CONTENT_DATA : {
							if ( m_vNodes[stNodeIdx].u.cContentObj.sRight != size_t( -1 ) ) {
								if ( !IsWhitespace( m_vStrings[m_vNodes[stNodeIdx].u.cContentObj.sRight] ) ) {
									CTree<LSON_JSON_ELEMENT> * ptParent = CurStackPointer( vCurElement );
									ptParent->Value().sData.append( m_vStrings[m_vNodes[stNodeIdx].u.cContentObj.sRight] );
								}
							}
							LSON_PUSH( cContentObj.sLeft );
							break;
						}
						case LSON_N_ATTRIBUTE_DECL : {
							/*CTree<LSON_JSON_ELEMENT> * ptParent = CurStackPointer( vCurElement );
							ptParent->Value().*/
							// TODO: Haven’t encountered this case yet.
							LSON_PUSH( aAttributeDeclObj.sList );
							break;
						}
						case LSON_N_ATTRIBUTE_LIST : {
							LSON_PUSH( aAttributeListObj.sRight );
							LSON_PUSH( aAttributeListObj.sLeft );
							break;
						}
						case LSON_N_ATTRIBUTE : {
							CTree<LSON_JSON_ELEMENT> * ptParent = CurStackPointer( vCurElement );
							LSON_JSON_ATTRIBUTE xaAttr = { m_vNodes[stNodeIdx].u.aAttributeObj.sAttribute, m_vNodes[stNodeIdx].u.aAttributeObj.sValue };
							ptParent->Value().vAttributes.push_back( xaAttr );
							break;
						}
					}
				}
				else {
					switch ( m_vNodes[stNodeIdx].nType ) {
						case LSON_N_ELEMENT : {
							vCurElement.pop_back();
							break;
						}
					}
					vStack.pop_back();
				}
			}
#undef LSON_PUSH
			return true;
		}
		catch ( ... ) {

		}
		return false;
	}

	/**
	 * Prints the tree recursively.
	 */
	void CJsonContainer::PrintTree() {
		PrintTree( Next( nullptr ), 0 );
	}

	/**
	 * Gets the next item.
	 *
	 * \param _ptThis The items whose next item is to be obtained.
	 * \return Returns the next item.
	 */
	CTree<CJsonContainer::LSON_JSON_ELEMENT> * CJsonContainer::Next( CTree<LSON_JSON_ELEMENT> * _ptThis ) {
		if ( !_ptThis ) {
			if ( !m_tRoot.Size() ) { return nullptr; }		// We skip the very rootest node.
			return const_cast<CTree<LSON_JSON_ELEMENT> *>(m_tRoot.GetChild( 0 ));
		}
		return CTree<LSON_JSON_ELEMENT>::Next( _ptThis );
	}

	/**
	 * Gets the previous item.
	 *
	 * \param _ptThis The items whose previous item is to be obtained.
	 * \return Returns the previous item.
	 */
	CTree<CJsonContainer::LSON_JSON_ELEMENT> * CJsonContainer::Prev( CTree<LSON_JSON_ELEMENT> * _ptThis ) {
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
		_ptThis = CTree<LSON_JSON_ELEMENT>::Prev( _ptThis );
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
	const CTree<CJsonContainer::LSON_JSON_ELEMENT> * CJsonContainer::GetChildElement( const CTree<LSON_JSON_ELEMENT> * _ptParent, const std::string &_sName ) const {
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
	CTree<CJsonContainer::LSON_JSON_ELEMENT> * CJsonContainer::GetChildElement( CTree<LSON_JSON_ELEMENT> * _ptParent, const std::string &_sName ) {
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
	std::vector<size_t> CJsonContainer::GatherChildElementIndices( const CTree<LSON_JSON_ELEMENT> * _ptParent, const std::string &_sName ) const {
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
	bool CJsonContainer::GetAttributeValue( const CTree<LSON_JSON_ELEMENT> * _ptElement, const std::string &_sName, std::string &_sRet ) const {
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
	bool CJsonContainer::GetChildElementData( const CTree<LSON_JSON_ELEMENT> * _ptParent, const std::string &_sName, std::string &_sRet ) const {
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
	bool CJsonContainer::GatherAttributes( std::multimap<std::string, std::string> &_mmDst ) {
		try {
			CTree<CJsonContainer::LSON_JSON_ELEMENT> * ptItem = Next( nullptr );
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
	bool CJsonContainer::GatherAttributes( std::map<std::string, std::set<std::string>> &_mDst ) {
		try {
			CTree<CJsonContainer::LSON_JSON_ELEMENT> * ptItem = Next( nullptr );
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
	bool CJsonContainer::GatherElements( std::map<std::string, std::set<std::string>> &_mDst ) {
		try {
			CTree<CJsonContainer::LSON_JSON_ELEMENT> * ptItem = Next( nullptr );
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
#endif	// 0

	/**
	 * Adds a node and returns its index into the array of nodes.
	 *
	 * \param _nNode The node to add.
	 * \return Returns the index of the added node inside m_vNodes.
	 */
	size_t CJsonContainer::AddNode( YYSTYPE::LSON_NODE &_nNode ) {
		_nNode.stNodeIdx = m_vNodes.size();
		m_vNodes.push_back( _nNode );
		return _nNode.stNodeIdx;
	}

	/**
	 * Prints the tree recursively.
	 * 
	 * \param _ptNode The node to print.
	 * \param _i32Depth The node depth.
	 */
	/*void CJsonContainer::PrintTree( const CTree<LSON_JSON_ELEMENT> * _ptNode, int32_t _i32Depth ) {
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
	}*/

}	// namespace lson

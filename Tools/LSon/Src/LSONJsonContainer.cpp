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
	 * \param _nLeft The left node.
	 * \return Returns the index of the value node created.
	 */
	size_t CJsonContainer::AddMemberList( YYSTYPE::LSON_NODE &_nNode, const YYSTYPE::LSON_NODE &_nLeft ) {
		_nNode.nType = LSON_N_MEMBER_LIST;
		_nNode.u.mlMembers.stLeft = _nLeft.stNodeIdx;
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

#ifdef _DEBUG
//#pragma optimize( "gt", on ) // Optimize for maximum speed
#endif
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
							jvVal.u.dDecimal = 0.0;
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

	/**
	 * Finds a member by name and returns a pointer to the given value or nullptr.  _jvValue must be an object.
	 *
	 * \param _jvValue The object owning the member to find.
	 * \param _sName The name of the member to locate.
	 * \return Returns a pointer to the member value with the given name or nullptr if it can't be found.
	 */
	const CJsonContainer::LSON_JSON_VALUE * CJsonContainer::GetMemberByName( const LSON_JSON_VALUE &_jvValue, const std::string &_sName ) {
		if ( _jvValue.vtType == LSON_VT_OBJECT ) {
			for ( size_t I = 0; I < _jvValue.oObject.vMembers.size(); ++I ) {
				if ( GetString( _jvValue.oObject.vMembers[I].stName ) == _sName ) {
					return &GetValue( _jvValue.oObject.vMembers[I].stValue );
				}
			}
		}
		return nullptr;
	}

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

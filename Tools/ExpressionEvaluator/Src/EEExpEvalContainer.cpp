#include "EEExpEvalContainer.h"
#include "Gen/EEExpEvalParser.h"

namespace ee {

	// == Function.
	// Gets the type to use between 2 given types.
	EE_NUM_CONSTANTS CExpEvalContainer::GetCastType( EE_NUM_CONSTANTS _ncLeft, EE_NUM_CONSTANTS _ncRight ) {
		return _ncLeft > _ncRight ? _ncLeft : _ncRight;
	}

	// Converts a result to a given type.
	CExpEvalContainer::EE_RESULT CExpEvalContainer::ConvertResult( const EE_RESULT &_rRes, EE_NUM_CONSTANTS _ncType ) {
		if ( _rRes.ncType == _ncType ) { return _rRes; }
		EE_RESULT rRes;
		rRes.ncType = _ncType;
		if ( _rRes.ncType == EE_NC_FLOATING ) {
			switch ( _ncType ) {
				case EE_NC_SIGNED : {
					rRes.u.i64Val = static_cast<int64_t>(_rRes.u.dVal);
					break;
				}
				case EE_NC_UNSIGNED : {
					rRes.u.ui64Val = static_cast<uint64_t>(_rRes.u.dVal);
					break;
				}
			}
		}
		else if ( _rRes.ncType == EE_NC_SIGNED ) {
			switch ( _ncType ) {
				case EE_NC_UNSIGNED : {
					rRes.u.ui64Val = static_cast<uint64_t>(_rRes.u.i64Val);
					break;
				}
				case EE_NC_FLOATING : {
					rRes.u.dVal = static_cast<double>(_rRes.u.i64Val);
					break;
				}
			}
		}
		else if ( _rRes.ncType == EE_NC_UNSIGNED ) {
			switch ( _ncType ) {
				case EE_NC_FLOATING : {
					rRes.u.dVal = static_cast<double>(_rRes.u.ui64Val);
					break;
				}
				case EE_NC_SIGNED : {
					rRes.u.i64Val = static_cast<int64_t>(_rRes.u.ui64Val);
					break;
				}
			}
		}
		return rRes;
	}

	// Decodes a string.
	size_t CExpEvalContainer::CreateString( const char * _pcText ) {
		std::string sTemp = DecodeEscapes( RemoveQuotes( _pcText, std::strlen( _pcText ) ) );
		return AddString( sTemp );
	}

	// Decodes an identifier.
	size_t CExpEvalContainer::CreateIdentifier( const char * _pcText ) {
		return AddString( _pcText );
	}

	// Creates a string expression.
	void CExpEvalContainer::CreateStringBasicExp( size_t _sStrIndex, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_IDENTIFIER;
		_ndNode.u.sStringIndex = _sStrIndex;
		AddNode( _ndNode );
	}

	// Creates a hex constant (0x----).
	void CExpEvalContainer::CreateHex1( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = StoULL( _pcText, 16 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a hex constant (----h).
	void CExpEvalContainer::CreateHex2( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = StoULL( _pcText, 16 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a hex constant (----).
	void CExpEvalContainer::CreateHex3( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = StoULL( _pcText, 16 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a decimal constant.
	void CExpEvalContainer::CreateUInt( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = StoULL( _pcText, 10 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates an oct constant.
	void CExpEvalContainer::CreateOct( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = StoULL( _pcText, 8 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Create a character constant.
	void CExpEvalContainer::CreateChar( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		std::string sTemp = _pcText;
		if ( sTemp[0] == 'L' ) {
			sTemp.erase( sTemp.begin() );
		}
		sTemp = RemoveQuotes( sTemp.c_str(), sTemp.size() );
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		size_t sLen;
		_ndNode.u.ui64Val = DecodeEscape( sTemp.c_str(), sTemp.size(), sLen );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Create a double constant.
	void CExpEvalContainer::CreateDouble( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		CreateDouble( std::atof( _pcText ), _ndNode );
	}

	// Create a double constant.
	void CExpEvalContainer::CreateDouble( double _dVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.dVal = _dVal;
		_ndNode.v.ncConstType = EE_NC_FLOATING;
		AddNode( _ndNode );
	}

	// Create an address node.
	void CExpEvalContainer::CreateAddress( const YYSTYPE::EE_NODE_DATA &_ndExp, EE_CAST_TYPES _ctCast, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ADDRESS;
		_ndNode.v.ctCast = _ctCast;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates member access.
	void CExpEvalContainer::CreateMemberAccess( const YYSTYPE::EE_NODE_DATA &_ndExp, size_t _sMember, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_MEMBERACCESS;
		_ndNode.v.sStringIndex = _sMember;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Creates a user (??) node.
	void CExpEvalContainer::CreateUser( YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_USER_VAR;
		AddNode( _ndNode );
	}

	// Create a unary node.
	void CExpEvalContainer::CreateUnary( const YYSTYPE::EE_NODE_DATA &_ndExp, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_UNARY;
		_ndNode.v.ui32Op = _uiOp;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Create an operator node.
	void CExpEvalContainer::CreateOp( const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_OP;
		_ndNode.v.ui32Op = _uiOp;
		_ndNode.u.sNodeIndex = _ndLeft.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndRight.sNodeIndex;
		AddNode( _ndNode );
	}

	// Create a conditional operator.
	void CExpEvalContainer::CreateConditional( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_CONDITIONAL;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndLeft.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndRight.sNodeIndex;
		AddNode( _ndNode );
	}

	// Create a 1-parm intrinsic.
	void CExpEvalContainer::CreateIntrinsic1( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_INTRINSIC_1;
		_ndNode.u.ui32Intrinsic = _uiIntrinsic;
		_ndNode.v.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Create a 2-parm intrinsic.
	void CExpEvalContainer::CreateIntrinsic2( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_INTRINSIC_2;
		_ndNode.u.ui32Intrinsic = _uiIntrinsic;
		_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
		AddNode( _ndNode );
	}

	// Create a 3-parm intrinsic.
	void CExpEvalContainer::CreateIntrinsic3( uint32_t _uiIntrinsic, const YYSTYPE::EE_NODE_DATA &_ndExp0, const YYSTYPE::EE_NODE_DATA &_ndExp1, const YYSTYPE::EE_NODE_DATA &_ndExp2, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_INTRINSIC_3;
		_ndNode.u.ui32Intrinsic = _uiIntrinsic;
		_ndNode.v.sNodeIndex = _ndExp0.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndExp1.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndExp2.sNodeIndex;
		AddNode( _ndNode );
	}

	// Sets the translation-unit node.
	void CExpEvalContainer::SetTrans( YYSTYPE::EE_NODE_DATA &_ndNode ) {
		m_sTrans = _ndNode.sNodeIndex;
	}

	// Adds a node to its internal node tree.
	void CExpEvalContainer::AddNode( YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.sNodeIndex = m_vNodes.size();
		m_vNodes.push_back( _ndNode );
	}

	// Adds a string and returns its index into the stack.
	size_t CExpEvalContainer::AddString( const std::string &_sText ) {
		for ( size_t I = 0; I < m_vStrings.size(); ++I ) {
			if ( m_vStrings[I] == _sText ) { return I; }
		}

		m_vStrings.push_back( _sText );
		return m_vStrings.size() - 1;
	}

	// Resolves a node.
	bool CExpEvalContainer::ResolveNode( size_t _sNode, EE_RESULT &_rRes ) {
		if ( _sNode >= m_vNodes.size() ) { return false; }
		YYSTYPE::EE_NODE_DATA & ndNode = m_vNodes[_sNode];

		switch ( ndNode.nType ) {
			case EE_N_NUMERICCONSTANT : {
				_rRes.ncType = ndNode.v.ncConstType;
				switch ( ndNode.v.ncConstType ) {
					case EE_NC_UNSIGNED : {
						_rRes.u.ui64Val = ndNode.u.ui64Val;
						break;
					}
					case EE_NC_SIGNED : {
						_rRes.u.i64Val = ndNode.u.i64Val;
						break;
					}
					case EE_NC_FLOATING : {
						_rRes.u.dVal = ndNode.u.dVal;
						break;
					}
				}
				return true;
			}
			case EE_N_IDENTIFIER : {
				if ( !m_pfshString ) { return false; }
				return m_pfshString( m_vStrings[ndNode.u.sStringIndex], m_uiptrStringData, this, _rRes );
			}
			case EE_N_ADDRESS : {
				return false; // TODO.
			}
			case EE_N_MEMBERACCESS : {
				if ( !m_pfmahMemberAccess ) { return false; }
				EE_RESULT rLeft;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rLeft ) ) { return false; }
				return m_pfmahMemberAccess( rLeft, m_vStrings[ndNode.v.sStringIndex], m_uiptrMemberAccess, this, _rRes );
			}
			case EE_N_USER_VAR : {
				if ( !m_pfUser ) { return false; }
				return m_pfUser( m_uiptrUserData, this, _rRes );
			}
			case EE_N_UNARY : {
				EE_RESULT rTemp;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTemp ) ) { return false; }
				switch ( ndNode.v.ui32Op ) {
					case '+' : {
						_rRes = rTemp;
						return true;
					}
					case '-' : {
						switch ( rTemp.ncType ) {
							case EE_NC_SIGNED : {
								_rRes.ncType = rTemp.ncType;
								_rRes.u.i64Val = -rTemp.u.i64Val;
								return true;
							}
							case EE_NC_UNSIGNED : {
								rTemp = ConvertResult( rTemp, EE_NC_SIGNED );
								_rRes.ncType = rTemp.ncType;
								_rRes.u.i64Val = -rTemp.u.i64Val;
								return true;
							}
							case EE_NC_FLOATING : {
								_rRes.ncType = rTemp.ncType;
								_rRes.u.dVal = -rTemp.u.dVal;
								return true;
							}
							default : { return false; }
						}
						return false;
					}
					case '~' : {
						switch ( rTemp.ncType ) {
							case EE_NC_SIGNED : {
								_rRes.ncType = rTemp.ncType;
								_rRes.u.i64Val = ~rTemp.u.i64Val;
								return true;
							}
							case EE_NC_UNSIGNED : {
								_rRes.ncType = rTemp.ncType;
								_rRes.u.ui64Val = ~rTemp.u.ui64Val;
								return true;
							}
							case EE_NC_FLOATING : {
								return false;
							}
							default : { return false; }
						}
						return false;
					}
					case '!' : {
						switch ( rTemp.ncType ) {
							case EE_NC_SIGNED : {
								_rRes.ncType = rTemp.ncType;
								_rRes.u.i64Val = !rTemp.u.i64Val;
								return true;
							}
							case EE_NC_UNSIGNED : {
								_rRes.ncType = rTemp.ncType;
								_rRes.u.ui64Val = !rTemp.u.ui64Val;
								return true;
							}
							case EE_NC_FLOATING : {
								_rRes.ncType = EE_NC_UNSIGNED;
								_rRes.u.ui64Val = !rTemp.u.dVal;
								return false;
							}
							default : { return false; }
						}
						return false;
					}
					default : { return false; }
				}
				return false;
			}
			case EE_N_OP : {
				EE_RESULT rLeft, rRight;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rLeft ) ) { return false; }
				if ( !ResolveNode( ndNode.w.sNodeIndex, rRight ) ) { return false; }
				_rRes.ncType = GetCastType( rLeft.ncType, rRight.ncType );
				rLeft = ConvertResult( rLeft, _rRes.ncType );
				rRight = ConvertResult( rRight, _rRes.ncType );
#define EE_OP( MEMBER, CASE, OP )								\
	case CASE : {												\
		_rRes.u.MEMBER = rLeft.u.MEMBER OP rRight.u.MEMBER;		\
		return true;											\
	}
#define EE_OP_BOOL( MEMBER, CASE, OP )							\
	case CASE : {												\
		_rRes.ncType = EE_NC_UNSIGNED;							\
		_rRes.u.ui64Val = rLeft.u.MEMBER OP rRight.u.MEMBER;	\
		return true;											\
	}
#define EE_INT_CHECK( CASE, MEMBER )										\
	case CASE : {															\
		switch ( ndNode.v.ui32Op ) {										\
			EE_OP( MEMBER, '*', * )											\
			EE_OP( MEMBER, '/', / )											\
			EE_OP( MEMBER, '%', % )											\
			EE_OP( MEMBER, '+', + )											\
			EE_OP( MEMBER, '-', - )											\
			EE_OP( MEMBER, CExpEvalParser::token::EE_RIGHT_OP, >> )			\
			EE_OP( MEMBER, CExpEvalParser::token::EE_LEFT_OP, << )			\
			EE_OP( MEMBER, '&', & )											\
			EE_OP( MEMBER, '^', ^ )											\
			EE_OP( MEMBER, '|', | )											\
			EE_OP_BOOL( MEMBER, '<', < )									\
			EE_OP_BOOL( MEMBER, '>', > )									\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_REL_LE, <= )		\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_REL_GE, >= )		\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_EQU_E, == )		\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_EQU_NE, != )		\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_AND, && )			\
			EE_OP_BOOL( MEMBER, CExpEvalParser::token::EE_OR, || )			\
			default : { return false; }										\
		}																	\
		return false;														\
	}

				switch ( _rRes.ncType ) {
					EE_INT_CHECK( EE_NC_SIGNED, i64Val )
					EE_INT_CHECK( EE_NC_UNSIGNED, ui64Val )
					case EE_NC_FLOATING : {
						switch ( ndNode.v.ui32Op ) {
							EE_OP( dVal, '*', * )
							EE_OP( dVal, '/', / )
							EE_OP( dVal, '+', + )
							EE_OP( dVal, '-', - )
							EE_OP_BOOL( dVal, '<', < )
							EE_OP_BOOL( dVal, '>', > )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_REL_LE, <= )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_REL_GE, >= )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_EQU_E, == )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_EQU_NE, != )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_AND, && )
							EE_OP_BOOL( dVal, CExpEvalParser::token::EE_OR, || )
							case '%' : {
								_rRes.u.dVal = std::fmod( rLeft.u.dVal, rRight.u.dVal );
								return true;
							}
							case '^' : {
								_rRes.u.dVal = std::pow( rLeft.u.dVal, rRight.u.dVal );
								return true;
							}
							default : { return false; }
						}
						return false;
					}
				}

#undef EE_INT_CHECK
#undef EE_OP_BOOL
#undef EE_OP
				return false;
			}
			case EE_N_CONDITIONAL : {
				EE_RESULT rExp;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rExp ) ) { return false; }
				bool bTrue;
				if ( rExp.ncType == EE_NC_SIGNED ) {
					bTrue = rExp.u.i64Val != 0;
				}
				else if ( rExp.ncType == EE_NC_UNSIGNED ) {
					bTrue = rExp.u.ui64Val != 0;
				}
				else if ( rExp.ncType == EE_NC_FLOATING ) {
					bTrue = rExp.u.dVal != 0.0;
				}
				else { return false; }	// Can't happen.

				// Slight variation from C/C++.  In compiled code, the left and right nodes have to be the same type, so if one of them
				//	was double, the other would be cast to double (because the ?: expression must be of a single specific type).
				// Here, the return type is whatever the left or right node's return type is.  Lazy evaluation to save time.
				return bTrue ? ResolveNode( ndNode.v.sNodeIndex, _rRes ) : ResolveNode( ndNode.w.sNodeIndex, _rRes );
			}
			case EE_N_INTRINSIC_1 : {
				EE_RESULT rExp;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rExp ) ) { return false; }
				rExp = ConvertResult( rExp, EE_NC_FLOATING );
				_rRes.ncType = EE_NC_FLOATING;
#define EE_OP( CASE, FUNC )												\
	case CExpEvalParser::token::CASE : {								\
		_rRes.u.dVal = ::FUNC( rExp.u.dVal );							\
		return true;													\
	}
				switch ( ndNode.u.ui32Intrinsic ) {
					EE_OP( EE_COS, cos )
					EE_OP( EE_SIN, sin )
					EE_OP( EE_TAN, tan )
					EE_OP( EE_ACOS, acos )
					EE_OP( EE_ASIN, asin )
					EE_OP( EE_ATAN, atan )
					EE_OP( EE_COSH, cosh )
					EE_OP( EE_SINH, sinh )
					EE_OP( EE_TANH, tanh )
					EE_OP( EE_ACOSH, acosh )
					EE_OP( EE_ASINH, asinh )
					EE_OP( EE_ATANH, atanh )
					EE_OP( EE_EXP, exp )
					EE_OP( EE_LOG, log )
					EE_OP( EE_LOG10, log10 )
					EE_OP( EE_LOG2, log2 )
					EE_OP( EE_EXP2, exp2 )
					EE_OP( EE_EXPM1, expm1 )
					case CExpEvalParser::token::EE_ILOGB : {
						_rRes.ncType = EE_NC_SIGNED;
						_rRes.u.i64Val = ::ilogb( rExp.u.dVal );
						return true;
					}
					EE_OP( EE_LOG1P, log1p )
					EE_OP( EE_LOGB, logb )
					EE_OP( EE_SQRT, sqrt )
					EE_OP( EE_CBRT, cbrt )
					EE_OP( EE_TGAMMA, tgamma )
					EE_OP( EE_LGAMMA, lgamma )
					EE_OP( EE_CEIL, ceil )
					EE_OP( EE_FLOOR, floor )
					EE_OP( EE_TRUNC, trunc )
					EE_OP( EE_ROUND, round )
					EE_OP( EE_NEARBYINT, nearbyint )
					EE_OP( EE_ABS, abs )
#undef EE_OP
				} 
				return false;
			}
			case EE_N_INTRINSIC_2 : {
				EE_RESULT rExp0, rExp1;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rExp0 ) ) { return false; }
				rExp0 = ConvertResult( rExp0, EE_NC_FLOATING );
				if ( !ResolveNode( ndNode.w.sNodeIndex, rExp1 ) ) { return false; }
				rExp1 = ConvertResult( rExp1, EE_NC_FLOATING );
				_rRes.ncType = EE_NC_FLOATING;
#define EE_OP( CASE, FUNC )												\
	case CExpEvalParser::token::CASE : {								\
		_rRes.u.dVal = ::FUNC( rExp0.u.dVal, rExp1.u.dVal );			\
		return true;													\
	}
				switch ( ndNode.u.ui32Intrinsic ) {
					EE_OP( EE_ATAN2, atan2 )
					EE_OP( EE_POW, pow )
					EE_OP( EE_HYPOT, hypot )
					EE_OP( EE_MOD, fmod )
					EE_OP( EE_REMAINDER, remainder )
					EE_OP( EE_NEXTAFTER, nextafter )
					EE_OP( EE_NEXTTOWARD, nexttoward )
					EE_OP( EE_DIM, fdim )
					EE_OP( EE_MAX, max )
					EE_OP( EE_MIN, min )
				}
#undef EE_OP
			}
			case EE_N_INTRINSIC_3 : {
				EE_RESULT rExp0, rExp1, rExp2;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rExp0 ) ) { return false; }
				rExp0 = ConvertResult( rExp0, EE_NC_FLOATING );
				if ( !ResolveNode( ndNode.w.sNodeIndex, rExp1 ) ) { return false; }
				rExp1 = ConvertResult( rExp1, EE_NC_FLOATING );
				if ( !ResolveNode( ndNode.x.sNodeIndex, rExp2 ) ) { return false; }
				rExp2 = ConvertResult( rExp2, EE_NC_FLOATING );
				_rRes.ncType = EE_NC_FLOATING;
	
				switch ( ndNode.u.ui32Intrinsic ) {
					case CExpEvalParser::token::EE_MADD : {
						_rRes.u.dVal = ::fma( rExp0.u.dVal, rExp1.u.dVal, rExp2.u.dVal );
						return true;
					}
				}
			}
		}
		return false;
	}

	// String to integer, from any base.  Since std::stoull() raises exceptions etc.
	uint64_t CExpEvalContainer::StoULL( const char * _pcText, int _iBase ) {
		// Negate?
		bool bNegate = false;
		if ( _pcText[0] == '-' ) {
			bNegate = true;
			++_pcText;
		}

		// Skip whitespace.
		while ( (*_pcText) == ' ' || (*_pcText) == '\t' || (*_pcText) == '\r' || (*_pcText) == '\n' ) {
			++_pcText;
		}

		// Skip any opening "0", "0x", etc.
		if ( _iBase == 0 || _iBase == 8 || _iBase == 16 ) {
			if ( (*_pcText) == '0' ) {
				++_pcText;
				if ( _iBase == 0 ) {
					_iBase = 8;
				}
			}
			if ( (*_pcText) == 'x' || (*_pcText) == 'X' ) {
				++_pcText;
				if ( _iBase == 0 ) {
					_iBase = 16;
				}
			}
		}
		if ( _iBase == 0 ) {
			_iBase = 10;
		}
		uint64_t uiRes = 0;
		for ( size_t I = 0; _pcText[I]; ++I ) {
			uint8_t uiNext = static_cast<uint8_t>(_pcText[I]);
			if ( uiNext >= '0' && uiNext <= '9' ) {
				uiNext -= '0';
			}
			else {
				if ( uiNext >= 'A' && uiNext <= 'Z' ) {
					uiNext -= 'A';
				}
				else {
					uiNext -= 'a';
				}
				uiNext += 10;
			}
			if ( uiNext >= _iBase ) {
				break;
			}
			// Check for overflow.
			uint64_t uiTemp = uiRes;
			uiRes *= _iBase;
			uiRes += uiNext;
			if ( (uiRes - uiNext) / _iBase != uiTemp) {
				return std::numeric_limits<uint64_t>::max();
			}
			/*if ( uiRes < uiTemp ) {
				return std::numeric_limits<uint64_t>::max();
			}*/
		}
		uint64_t uiCheck = std::strtoull( _pcText, nullptr, _iBase );
		return bNegate ? static_cast<uint64_t>(-static_cast<int64_t>(uiRes)) : uiRes;
	}

}	// namespace ee;

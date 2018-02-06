#include "EEExpEvalContainer.h"
#include "EEFloatX.h"
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

	// Creates a binary constant (0b----).
	void CExpEvalContainer::CreateBin( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = StoULL( _pcText + 2, 2 );
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

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( uint64_t _uiVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = _uiVal;
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( int64_t _iVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.i64Val = _iVal;
		_ndNode.v.ncConstType = EE_NC_SIGNED;
		AddNode( _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( uint32_t _uiVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		CreateNumber( static_cast<uint64_t>(_uiVal), _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( int32_t _iVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		CreateNumber( static_cast<int64_t>(_iVal), _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( float _fVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		CreateDouble( _fVal, _ndNode );
	}

	// Creates a numeric constant.
	void CExpEvalContainer::CreateNumber( double _dVal, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		CreateDouble( _dVal, _ndNode );
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

	// Create a reinterpretation of bits to a float.
	void CExpEvalContainer::CreateAsFloat( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ASFLOAT;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Create a reinterpretation of bits to a float of a common type.
	void CExpEvalContainer::CreateAsFloat( const YYSTYPE::EE_NODE_DATA &_ndExp, int32_t _eType, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = static_cast<ee::EE_NODES>(_eType);
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Create a reinterpretation of bits to a double.
	void CExpEvalContainer::CreateAsDouble( const YYSTYPE::EE_NODE_DATA &_ndExp, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_ASDOUBLE;
		_ndNode.u.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Create a reinterpretation of bits to a double with the given bit specifications.
	void CExpEvalContainer::CreateAsFloatX( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
		const YYSTYPE::EE_NODE_DATA &_ndExpBits,
		const YYSTYPE::EE_NODE_DATA &_ndManBits,
		const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
		const YYSTYPE::EE_NODE_DATA &_ndSignValue,	// 0 or 1.
		const YYSTYPE::EE_NODE_DATA &_ndExpValue,
		const YYSTYPE::EE_NODE_DATA &_ndManValue,
		YYSTYPE::EE_NODE_DATA &_ndNode ) {

		_ndNode.nType = EE_N_ASXFLOAT;
		_ndNode.u.sNodeIndex = _ndSignBits.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndExpBits.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndManBits.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndImplicitMan.sNodeIndex;
		_ndNode.y.sNodeIndex = _ndSignValue.sNodeIndex;
		_ndNode.z.sNodeIndex = _ndExpValue.sNodeIndex;
		_ndNode.a.sNodeIndex = _ndManValue.sNodeIndex;
		AddNode( _ndNode );
	}

	// Create a reinterpretation of bits to a double with the given bit specifications.
	void CExpEvalContainer::CreateAsFloatX( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
		const YYSTYPE::EE_NODE_DATA &_ndExpBits,
		const YYSTYPE::EE_NODE_DATA &_ndManBits,
		const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
		const YYSTYPE::EE_NODE_DATA &_ndExp,	// The double value to convert.
			
		YYSTYPE::EE_NODE_DATA &_ndNode ) {

		_ndNode.nType = EE_N_ASXFLOAT_FROM_DOUBLE;
		_ndNode.u.sNodeIndex = _ndSignBits.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndExpBits.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndManBits.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndImplicitMan.sNodeIndex;
		_ndNode.y.sNodeIndex = _ndExp.sNodeIndex;
		AddNode( _ndNode );
	}

	// Gets a property of a floating-point type with the given bit configuration.
	void CExpEvalContainer::CreateAsFloatXProp( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
		const YYSTYPE::EE_NODE_DATA &_ndExpBits,
		const YYSTYPE::EE_NODE_DATA &_ndManBits,
		const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
		const EE_NODES _nProp,
			
		YYSTYPE::EE_NODE_DATA &_ndNode ) {

		_ndNode.nType = _nProp;
		_ndNode.u.sNodeIndex = _ndSignBits.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndExpBits.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndManBits.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndImplicitMan.sNodeIndex;
		AddNode( _ndNode );
	}

	// Gets a property of a floating-point type with the given bit configuration.
	void CExpEvalContainer::CreateAsFloatXProp( const YYSTYPE::EE_NODE_DATA &_ndSignBits,
		const YYSTYPE::EE_NODE_DATA &_ndExpBits,
		const YYSTYPE::EE_NODE_DATA &_ndManBits,
		const YYSTYPE::EE_NODE_DATA &_ndImplicitMan,
		const YYSTYPE::EE_NODE_DATA &_ndExp,	// The double value to convert.
		const EE_NODES _nProp,
			
		YYSTYPE::EE_NODE_DATA &_ndNode ) {

		_ndNode.nType = _nProp;
		_ndNode.u.sNodeIndex = _ndSignBits.sNodeIndex;
		_ndNode.v.sNodeIndex = _ndExpBits.sNodeIndex;
		_ndNode.w.sNodeIndex = _ndManBits.sNodeIndex;
		_ndNode.x.sNodeIndex = _ndImplicitMan.sNodeIndex;
		_ndNode.y.sNodeIndex = _ndExp.sNodeIndex;
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
							case CExpEvalParser::token::EE_RIGHT_OP : {
								_rRes.u.dVal = RShift( rLeft.u.dVal, rRight.u.dVal );
								return true;
							}
							case CExpEvalParser::token::EE_LEFT_OP : {
								_rRes.u.dVal = LShift( rLeft.u.dVal, rRight.u.dVal );
								return true;
							}
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
#pragma warning( push )
#pragma warning( disable : 4244 )	// warning C4244: 'argument': conversion from 'double' to 'float', possible loss of data
				switch ( ndNode.u.ui32Intrinsic ) {
					EE_OP( EE_ATAN2, atan2 )
					EE_OP( EE_POW, pow )
					EE_OP( EE_HYPOT, hypot )
					EE_OP( EE_MOD, fmod )
					EE_OP( EE_REMAINDER, remainder )
					EE_OP( EE_NEXTAFTER, nextafter )
					EE_OP( EE_NEXTAFTERF, nextafterf )
					EE_OP( EE_NEXTTOWARD, nexttoward )
					EE_OP( EE_NEXTTOWARDF, nexttowardf )
					EE_OP( EE_DIM, fdim )
					EE_OP( EE_MAX, max )
					EE_OP( EE_MIN, min )
				}
#pragma warning( pop )
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
			case EE_N_ASFLOAT : {
				EE_RESULT rTemp;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTemp ) ) { return false; }
				// This is for reinterpreting a uint32_t as a float.  The expression in ndNode.u.sNodeIndex
				//	can either be an integral type (cast up to 64 bits) or a float (cast up to a double).
				float fVal = 0.0f;
				switch ( rTemp.ncType ) {
					case EE_NC_UNSIGNED : {
						fVal = (*reinterpret_cast<float *>(&rTemp.u.ui64Val));
						break;
					}
					case EE_NC_SIGNED : {
						fVal = (*reinterpret_cast<float *>(&rTemp.u.i64Val));
						break;
					}
					case EE_NC_FLOATING : {
						// as_float() is not really intended to reinterpret 64 "double" bit as "float" bits.
						//	More likely, the user wants to see the float value cast from a double.
						fVal = static_cast<float>(rTemp.u.dVal);
						break;
					}
					default : { return false; }
				}
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = fVal;
				return true;
			}
			case EE_N_ASDOUBLE : {
				EE_RESULT rTemp;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTemp ) ) { return false; }
				// This is for reinterpreting a uint64_t as a float.  The expression in ndNode.u.sNodeIndex
				//	can either be an integral type (cast up to 64 bits) or a float (cast up to a double).
				double dVal = 0.0;
				switch ( rTemp.ncType ) {
					case EE_NC_UNSIGNED : {
						dVal = (*reinterpret_cast<double *>(&rTemp.u.ui64Val));
						break;
					}
					case EE_NC_SIGNED : {
						dVal = (*reinterpret_cast<double *>(&rTemp.u.i64Val));
						break;
					}
					case EE_NC_FLOATING : {
						// Nothing to do.
						dVal = rTemp.u.dVal;
						break;
					}
					default : { return false; }
				}
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = dVal;
				return true;
			}
			case EE_N_ASXFLOAT : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }
				EE_RESULT rTempSignVal;
				if ( !ResolveNode( ndNode.y.sNodeIndex, rTempSignVal ) ) { return false; }
				EE_RESULT rTempExpVal;
				if ( !ResolveNode( ndNode.z.sNodeIndex, rTempExpVal ) ) { return false; }
				EE_RESULT rTempManVal;
				if ( !ResolveNode( ndNode.a.sNodeIndex, rTempManVal ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );
				rTempSignVal = ConvertResult( rTempSignVal, EE_NC_UNSIGNED );
				rTempExpVal = ConvertResult( rTempExpVal, EE_NC_UNSIGNED );
				rTempManVal = ConvertResult( rTempManVal, EE_NC_UNSIGNED );
				CFloatX fTemp;
				if ( rTempSignBits.u.ui64Val ) {
					fTemp.bSign = rTempSignVal.u.ui64Val ? true : false;
				}
				fTemp.uiExpBits = static_cast<uint16_t>(rTempExpBits.u.ui64Val);
				fTemp.uiManBits = static_cast<uint16_t>(rTempManBits.u.ui64Val);
				fTemp.uiExponent = ((1ULL << fTemp.uiExpBits) - 1ULL) & rTempExpVal.u.ui64Val;
				fTemp.uiMantissa = ((1ULL << fTemp.uiManBits) - 1ULL) & rTempManVal.u.ui64Val;
				
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = fTemp.AsDouble();
				return true;
			}
			case EE_N_ASXFLOAT_FROM_DOUBLE : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }
				EE_RESULT rTempDoubleVal;
				if ( !ResolveNode( ndNode.y.sNodeIndex, rTempDoubleVal ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );
				rTempDoubleVal = ConvertResult( rTempDoubleVal, EE_NC_FLOATING );

				CFloatX fTemp;
				fTemp.CreateFromDouble( rTempDoubleVal.u.dVal, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = fTemp.AsDouble();
				return true;
			}
			case EE_N_ASFLOAT16 : {
				EE_RESULT rTemp;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTemp ) ) { return false; }
				rTemp = ConvertResult( rTemp, EE_NC_FLOATING );
				CFloatX fTemp;
				fTemp.CreateFromDouble( rTemp.u.dVal, 5, 11, true, true );
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = fTemp.AsDouble();
				return true;
			}
			case EE_N_ASFLOAT14 : {
				EE_RESULT rTemp;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTemp ) ) { return false; }
				rTemp = ConvertResult( rTemp, EE_NC_FLOATING );
				CFloatX fTemp;
				fTemp.CreateFromDouble( rTemp.u.dVal, 5, 10, true, false );
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = fTemp.AsDouble();
				return true;
			}
			case EE_N_ASFLOAT11 : {
				EE_RESULT rTemp;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTemp ) ) { return false; }
				rTemp = ConvertResult( rTemp, EE_NC_FLOATING );
				CFloatX fTemp;
				fTemp.CreateFromDouble( rTemp.u.dVal, 5, 7, true, false );
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = fTemp.AsDouble();
				return true;
			}
			case EE_N_ASFLOAT10 : {
				EE_RESULT rTemp;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTemp ) ) { return false; }
				rTemp = ConvertResult( rTemp, EE_NC_FLOATING );
				CFloatX fTemp;
				fTemp.CreateFromDouble( rTemp.u.dVal, 5, 6, true, false );
				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = fTemp.AsDouble();
				return true;
			}
			case EE_N_ASXFLOAT_MAX : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );

				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = CFloatX::GetMaxForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );
				return true;
			}
			case EE_N_ASXFLOAT_MIN : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );

				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = CFloatX::GetNormalizedMinForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );
				return true;
			}
			case EE_N_ASXFLOAT_TRUE_MIN : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );

				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = CFloatX::GetMinForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );
				return true;
			}
			case EE_N_ASXFLOAT_NAN : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );

				CFloatX fTemp;
				fTemp.CreateNaN( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = fTemp.AsUint64();
				return true;
			}
			case EE_N_ASXFLOAT_INF : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );

				CFloatX fTemp;
				fTemp.CreateInfP( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = fTemp.AsUint64();
				return true;
			}
			case EE_N_ASXFLOAT_SUBNORM_MAX : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );

				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = CFloatX::GetDenormalizedMaxForBits( static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );
				return true;
			}
			case EE_N_ASXFLOAT_EPS : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );

				CFloatX fOne, fNextUp;
				fOne.CreateFromDouble( 1.0, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );
				fNextUp.CreateFromParts( fOne.bSign, fOne.uiExponent, fOne.uiMantissa + 1, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0 );

				_rRes.ncType = EE_NC_FLOATING;
				_rRes.u.dVal = fNextUp.AsDouble() - fOne.AsDouble();
				return true;
			}
			case EE_N_ASXFLOAT_SIGNBIT : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }
				EE_RESULT rTempDoubleVal;
				if ( !ResolveNode( ndNode.y.sNodeIndex, rTempDoubleVal ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );
				rTempDoubleVal = ConvertResult( rTempDoubleVal, EE_NC_FLOATING );

				CFloatX fTemp;
				fTemp.CreateFromDouble( rTempDoubleVal.u.dVal, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = fTemp.SignBit();
				return true;
			}
			case EE_N_ASXFLOAT_EXPBITS : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }
				EE_RESULT rTempDoubleVal;
				if ( !ResolveNode( ndNode.y.sNodeIndex, rTempDoubleVal ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );
				rTempDoubleVal = ConvertResult( rTempDoubleVal, EE_NC_FLOATING );

				CFloatX fTemp;
				fTemp.CreateFromDouble( rTempDoubleVal.u.dVal, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = fTemp.ExpBits();
				return true;
			}
			case EE_N_ASXFLOAT_MANBITS : {
				EE_RESULT rTempSignBits;
				if ( !ResolveNode( ndNode.u.sNodeIndex, rTempSignBits ) ) { return false; }
				EE_RESULT rTempExpBits;
				if ( !ResolveNode( ndNode.v.sNodeIndex, rTempExpBits ) ) { return false; }
				EE_RESULT rTempManBits;
				if ( !ResolveNode( ndNode.w.sNodeIndex, rTempManBits ) ) { return false; }
				EE_RESULT rTempImplied;
				if ( !ResolveNode( ndNode.x.sNodeIndex, rTempImplied ) ) { return false; }
				EE_RESULT rTempDoubleVal;
				if ( !ResolveNode( ndNode.y.sNodeIndex, rTempDoubleVal ) ) { return false; }

				rTempSignBits = ConvertResult( rTempSignBits, EE_NC_UNSIGNED );
				if ( rTempSignBits.u.ui64Val > CFloatX::MaxSignBits() ) { return false; }
				rTempExpBits = ConvertResult( rTempExpBits, EE_NC_UNSIGNED );
				if ( rTempExpBits.u.ui64Val > CFloatX::MaxExpBits() ) { return false; }
				rTempManBits = ConvertResult( rTempManBits, EE_NC_UNSIGNED );
				if ( rTempManBits.u.ui64Val > DBL_MANT_DIG ) { return false; }
				rTempImplied = ConvertResult( rTempImplied, EE_NC_UNSIGNED );
				rTempDoubleVal = ConvertResult( rTempDoubleVal, EE_NC_FLOATING );

				CFloatX fTemp;
				fTemp.CreateFromDouble( rTempDoubleVal.u.dVal, static_cast<uint16_t>(rTempExpBits.u.ui64Val), static_cast<uint16_t>(rTempManBits.u.ui64Val),
					rTempImplied.u.ui64Val != 0, rTempSignBits.u.ui64Val != 0 );
				_rRes.ncType = EE_NC_UNSIGNED;
				_rRes.u.ui64Val = fTemp.ManBits();
				return true;
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
		}
		return bNegate ? static_cast<uint64_t>(-static_cast<int64_t>(uiRes)) : uiRes;
	}

	// Shifting for floats.
	double CExpEvalContainer::LShift( double _dVal, double _dShift ) {
		return _dVal * ::pow( 2.0, _dShift );
	}

	// Shifting for floats.
	double CExpEvalContainer::RShift( double _dVal, double _dShift ) {
		return _dVal / ::pow( 2.0, _dShift );
	}

}	// namespace ee;

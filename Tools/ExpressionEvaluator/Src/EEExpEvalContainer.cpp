#include "EEExpEvalContainer.h"

namespace ee {

	// == Function.
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
		_ndNode.u.ui64Val = std::stoul( _pcText, nullptr, 16 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a hex constant (----h).
	void CExpEvalContainer::CreateHex2( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		std::string sTemp = _pcText;
		sTemp.pop_back();
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = std::stoul( sTemp, nullptr, 16 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a hex constant (----).
	void CExpEvalContainer::CreateHex3( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = std::stoul( _pcText, nullptr, 16 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates a decimal constant.
	void CExpEvalContainer::CreateUInt( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = std::stoul( _pcText, nullptr, 10 );
		_ndNode.v.ncConstType = EE_NC_UNSIGNED;
		AddNode( _ndNode );
	}

	// Creates an oct constant.
	void CExpEvalContainer::CreateOct( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.ui64Val = std::stoul( _pcText, nullptr, 8 );
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
		_ndNode.nType = EE_N_NUMERICCONSTANT;
		_ndNode.u.dVal = std::atof( _pcText );
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

}	// namespace ee;

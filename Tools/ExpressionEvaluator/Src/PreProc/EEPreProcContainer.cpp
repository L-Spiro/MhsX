/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: Stores all of the data parsed during the parsing of an expression for use with the preprocessor.
 */

#include "EEPreProcContainer.h"


namespace ee {

	// == Various constructors.
	CPreProcContainer::CPreProcContainer() {
	}

	// == Functions.
	/**
	 * Creates a node pointing to an identifier (by index into the identifier stack).
	 *
	 * \param _pcText The string name of the identifier.
	 * \param _ndNode Node to create based off the string constant.
	 * \return Returns true if the node was added.
	 */
	bool CPreProcContainer::CreateIdentifier( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = EE_PPN_IDENTIFIER;

		_ndNode.nuNodeData.ui32IdentifierIndex = AddIdentifier( _pcText );
		if ( _ndNode.nuNodeData.ui32IdentifierIndex == UINT32_MAX ) { return false; }

		return AddNode( _ndNode );
	}

	/**
	 * Accepts a string representation of a decimal constant and stores the result in the
	 *	given node.
	 *
	 * \param _pcText String constant containing the numeric.
	 * \param _ndNode Node to create based off the string constant.
	 * \return Returns true if the node was added.
	 */
	bool CPreProcContainer::CreateDecimal( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = EE_PPN_NUMERICCONSTANT;

		std::string sString = _pcText;
		if ( sString.size() == 0 ) {
			// Error.
			_ndNode.nuNodeData.ui64Const = 0ULL;
			_ndNode.nueNodeDataEx.ncConstantType = EE_PPNC_INVALID;
			return false;
		}

		switch ( ee::CExpEval::ClassifyString( sString ) ) {
			case EE_SNC_SIGNED : {
				_ndNode.nueNodeDataEx.ncConstantType = EE_PPNC_SIGNED;
				break;
			}
			case EE_SNC_UNSIGNED : {
				_ndNode.nueNodeDataEx.ncConstantType = EE_PPNC_UNSIGNED;
				break;
			}
			case EE_SNC_INVALID : {
				_ndNode.nuNodeData.ui64Const = 0ULL;
				_ndNode.nueNodeDataEx.ncConstantType = EE_PPNC_INVALID;
				return false;
			}
		}
		_ndNode.nuNodeData.i64Const = ee::CExpEval::StoULL( _pcText );

		return AddNode( _ndNode );
	}

	/**
	 * Accepts a string representation of a hexadecimal constant and stores the result in the
	 *	given node.
	 *
	 * \param _pcText String constant containing the numeric.
	 * \param _ndNode Node to create based off the string constant.
	 * \return Returns true if the node was added.
	 */
	bool CPreProcContainer::CreateHexadecimal( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = EE_PPN_NUMERICCONSTANT;

		std::string sString = _pcText;
		if ( sString.size() == 0 ) {
			// Error.
			_ndNode.nuNodeData.ui64Const = 0ULL;
			_ndNode.nueNodeDataEx.ncConstantType = EE_PPNC_INVALID;
			return false;
		}

		switch ( ee::CExpEval::ClassifyString( sString ) ) {
			case EE_SNC_SIGNED : {
				_ndNode.nueNodeDataEx.ncConstantType = EE_PPNC_SIGNED;
				break;
			}
			case EE_SNC_UNSIGNED : {
				_ndNode.nueNodeDataEx.ncConstantType = EE_PPNC_UNSIGNED;
				break;
			}
			case EE_SNC_INVALID : {
				_ndNode.nuNodeData.ui64Const = 0ULL;
				_ndNode.nueNodeDataEx.ncConstantType = EE_PPNC_INVALID;
				return false;
			}
		}
		_ndNode.nuNodeData.i64Const = ee::CExpEval::StoULL( _pcText, 16 );

		return AddNode( _ndNode );
	}

	/**
	 * Accepts a representation of a floating-point constant and stores the result in the
	 *	given node.
	 *
	 * \param _pcText String constant containing the numeric.
	 * \param _ndNode Node to create based off the string constant.
	 * \return Returns true if the node was added.
	 */
	bool CPreProcContainer::CreateFloatingPoint( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = EE_PPN_NUMERICCONSTANT;

		std::string sString = _pcText;
		if ( sString.size() == 0 ) {
			// Error.
			_ndNode.nuNodeData.ui64Const = 0ULL;
			_ndNode.nueNodeDataEx.ncConstantType = EE_PPNC_INVALID;
			return false;
		}

		_ndNode.nuNodeData.dConst = ee::CExpEval::AtoF( _pcText );
		_ndNode.nueNodeDataEx.ncConstantType = EE_PPNC_FLOATING;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a unary !, ~, -, or + node.
	 *
	 * \param _ui32Op The operator.
	 * \param _ndBase The base expression modified by the operator.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	bool CPreProcContainer::CreateUnaryOp( uint32_t _ui32Op, const YYSTYPE::EE_NODE_DATA &_ndBase,
		YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = EE_PPN_UNARY;

		_ndNode.nuNodeData.ui32UnaryOp = _ui32Op;
		_ndNode.nueNodeDataEx.stNodeIndexEx = _ndBase.stNodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a node referencing two other nodes with an operator between them.  From X * Y to
	 *	X && Y.
	 *
	 * \param _ndLeft The left operand.
	 * \param _ndRight The right operand.
	 * \param _ui32Op The operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	bool CPreProcContainer::CreateMath( const YYSTYPE::EE_NODE_DATA &_ndLeft,
		const YYSTYPE::EE_NODE_DATA &_ndRight,
		uint32_t _ui32Op, YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = EE_PPN_MATH;
		_ndNode.nuNodeData.stNodeIndex = _ndLeft.stNodeIndex;
		_ndNode.nueNodeDataEx.stNodeIndexEx = _ndRight.stNodeIndex;
		_ndNode.nuoOp.ui32Op = _ui32Op;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a tertiary ?: expression.
	 *
	 * \param _ndCond The conditional operand.
	 * \param _ndLeft The left operand.
	 * \param _ndRight The right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	bool CPreProcContainer::CreateTertiary( const YYSTYPE::EE_NODE_DATA &_ndCond,
		const YYSTYPE::EE_NODE_DATA &_ndLeft,
		const YYSTYPE::EE_NODE_DATA &_ndRight,
		YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = EE_PPN_TERTIARY;
		_ndNode.nuNodeData.stNodeIndex = _ndCond.stNodeIndex;
		_ndNode.nueNodeDataEx.stNodeIndexEx = _ndLeft.stNodeIndex;
		_ndNode.nuoOp.stNodeIndex3 = _ndRight.stNodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a defined() expression.
	 *
	 * \param _ndIdent The identifier.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	bool CPreProcContainer::CreateDefined( const YYSTYPE::EE_NODE_DATA &_ndIdent,
		YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = EE_PPN_DEFINED;
		_ndNode.nuNodeData.stNodeIndex = _ndIdent.stNodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a list of external declarations.
	 *
	 * \param param _ndLeft Left operand.
	 * \param _ndRight Right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	bool CPreProcContainer::CreateTranslationUnit( const YYSTYPE::EE_NODE_DATA &_ndLeft,
		const YYSTYPE::EE_NODE_DATA &_ndRight,
		YYSTYPE::EE_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = EE_PPN_TU;
		_ndNode.nuNodeData.stNodeIndex = _ndLeft.stNodeIndex;
		_ndNode.nueNodeDataEx.stNodeIndexEx = _ndRight.stNodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Gets a node for read-only.
	 *
	 * \param _ui32Index Index of the node to get.
	 * \return Returns a constant reference to the requested node.
	 */
	const EE_PREPROC_SYNTAX_NODES::EE_NODE_DATA & CPreProcContainer::GetNode( size_t _stIndex ) const {
		return m_vNodes[_stIndex];
	}

	/**
	 * Gets the total number of nodes.
	 *
	 * \return Returns the total number of nodes in the syntax tree.
	 */
	size_t CPreProcContainer::TotalNodes() const {
		return m_vNodes.size();
	}

	/**
	 * Gets a string from the string pool.
	 *
	 * \param _ui32Index Index of the string to get.
	 * \return Returns the string at the given index.
	 */
	const std::string & CPreProcContainer::GetString( uint32_t _ui32Index ) const {
		return m_vStringPool[_ui32Index];
	}

	/**
	 * Adds a node to the stack.  Set the node index too.
	 *
	 * \param _ndNode The node to add to the stack.
	 * \return Returns true if the node was added.
	 */
	bool CPreProcContainer::AddNode( EE_PREPROC_SYNTAX_NODES::EE_NODE_DATA &_ndNode ) {
		_ndNode.stNodeIndex = m_vNodes.size();
		try {
			m_vNodes.push_back( _ndNode );
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Adds an identifier by name.  Returns the index of the created identifier in the identifier stack.
	 *	The type of the created identifier is undefined.
	 *
	 * \param _pcText The string name of the identifier.
	 * \return Returns the index of the created identifier.
	 */
	uint32_t CPreProcContainer::AddIdentifier( const char * _pcText ) {
		// Find the name in the existing stack of strings if it is there.
		uint32_t ui32Ret = uint32_t( HasString( std::string( _pcText ) ) );
		if ( ui32Ret == static_cast<uint32_t>(static_cast<size_t>(-1)) ) {
			// Not there so add it.
			try {
				ui32Ret = uint32_t( m_vStringPool.size() );
				m_vStringPool.push_back( _pcText );
			}
			catch ( ... ) {
				return UINT32_MAX;
			}
		}
		return ui32Ret;
	}

	// Returns the index of a string if it exists or static_cast<size_t>(-1) otherwise.
	size_t CPreProcContainer::HasString( const std::string &_sText ) const {
		for ( size_t I = 0; I < m_vStringPool.size(); ++I ) {
			if ( m_vStringPool[I] == _sText ) { return I; }
		}
		return static_cast<size_t>(-1);
	}

}	// namespace ee

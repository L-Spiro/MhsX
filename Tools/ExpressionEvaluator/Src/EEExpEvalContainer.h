#pragma once

#include "EEExpEval.h"
#include "EEExpEvalSyntaxNodes.h"
#include <string>
#include <vector>

namespace ee {

	class CExpEvalLexer;

	class CExpEvalContainer {
	public :
		CExpEvalContainer( CExpEvalLexer * _plLexer ) :
			m_bTreateAllAsHex( false ),
			m_peelLexer( _plLexer ) {
		}


		// == Functions.
		// Do we treat everything as hex?
		bool								TreatAllAsHex() const { return m_bTreateAllAsHex; }

		// Set whether to treat all as hex or not.
		void								SetTreatAsHex( bool _bVal ) { m_bTreateAllAsHex = _bVal; }

		// Decodes a string.
		size_t								CreateString( const char * _pcText );

		// Decodes an identifier.
		size_t								CreateIdentifier( const char * _pcText );

		// Creates a string expression.
		void								CreateStringBasicExp( size_t _sStrIndex, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a hex constant (0x----).
		void								CreateHex1( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a hex constant (----h).
		void								CreateHex2( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a hex constant (----).
		void								CreateHex3( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates a decimal constant.
		void								CreateUInt( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates an oct constant.
		void								CreateOct( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a character constant.
		void								CreateChar( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a double constant.
		void								CreateDouble( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create an address node.
		void								CreateAddress( const YYSTYPE::EE_NODE_DATA &_ndExp, EE_CAST_TYPES _ctCast, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Creates member access.
		void								CreateMemberAccess( const YYSTYPE::EE_NODE_DATA &_ndExp, size_t _sMember, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a unary node.
		void								CreateUnary( const YYSTYPE::EE_NODE_DATA &_ndExp, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create an operator node.
		void								CreateOp( const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, uint32_t _uiOp, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Create a conditional operator.
		void								CreateConditional( const YYSTYPE::EE_NODE_DATA &_ndExp, const YYSTYPE::EE_NODE_DATA &_ndLeft, const YYSTYPE::EE_NODE_DATA &_ndRight, YYSTYPE::EE_NODE_DATA &_ndNode );

		// Sets the translation-unit node.
		void								SetTrans( YYSTYPE::EE_NODE_DATA &_ndNode );


	protected :
		// == Members.
		// The lexer.
		CExpEvalLexer *						m_peelLexer;

		// The stack of nodes.
		std::vector<YYSTYPE::EE_NODE_DATA>	m_vNodes;

		// The stack of strings.
		std::vector<std::string>			m_vStrings;

		// Treate everything as hex?
		bool								m_bTreateAllAsHex;

		// Translation unit.
		size_t								m_sTrans;

		// == Functions.
		// Adds a node to its internal node tree.
		void								AddNode( YYSTYPE::EE_NODE_DATA &_ndNode );

		// Adds a string and returns its index into the stack.
		size_t								AddString( const std::string &_sText );
	};

}	// namespace ee

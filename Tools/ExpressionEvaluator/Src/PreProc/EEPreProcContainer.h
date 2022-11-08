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


#ifndef __EE_PREPROCCONTAINER_H__
#define __EE_PREPROCCONTAINER_H__

#include "../EEExpEval.h"
#include "EEPreProcSyntaxNodes.h"

namespace ee {

	/**
	 * Class CPreProcContainer
	 * \brief Stores all of the data parsed during the parsing of an expression for use with the preprocessor.
	 *
	 * Description: Stores all of the data parsed during the parsing of an expression for use with the preprocessor.
	 */
	class CPreProcContainer {
	public :
		// == Various constructors.
		CPreProcContainer();


		// == Functions.
		/**
		 * Creates a node pointing to an identifier (by index into the identifier stack).
		 *
		 * \param _pcText The string name of the identifier.
		 * \param _ndNode Node to create based off the string constant.
		 * \return Returns true if the node was added.
		 */
		bool										CreateIdentifier( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * Accepts a string representation of a decimal constant and stores the result in the
		 *	given node.
		 *
		 * \param _pcText String constant containing the numeric.
		 * \param _ndNode Node to create based off the string constant.
		 * \return Returns true if the node was added.
		 */
		bool										CreateDecimal( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * Accepts a string representation of a hexadecimal constant and stores the result in the
		 *	given node.
		 *
		 * \param _pcText String constant containing the numeric.
		 * \param _ndNode Node to create based off the string constant.
		 * \return Returns true if the node was added.
		 */
		bool 										CreateHexadecimal( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * Accepts a representation of a floating-point constant and stores the result in the
		 *	given node.
		 *
		 * \param _pcText String constant containing the numeric.
		 * \param _ndNode Node to create based off the string constant.
		 * \return Returns true if the node was added.
		 */
		bool 										CreateFloatingPoint( const char * _pcText, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * Creates a unary !, ~, -, or + node.
		 *
		 * \param _ui32Op The operator.
		 * \param _ndBase The base expression modified by the operator.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		bool 										CreateUnaryOp( uint32_t _ui32Op, const YYSTYPE::EE_NODE_DATA &_ndBase,
			YYSTYPE::EE_NODE_DATA &_ndNode );

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
		bool 										CreateMath( const YYSTYPE::EE_NODE_DATA &_ndLeft,
			const YYSTYPE::EE_NODE_DATA &_ndRight,
			uint32_t _ui32Op, YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * Creates a tertiary ?: expression.
		 *
		 * \param _ndCond The conditional operand.
		 * \param _ndLeft The left operand.
		 * \param _ndRight The right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		bool 										CreateTertiary( const YYSTYPE::EE_NODE_DATA &_ndCond,
			const YYSTYPE::EE_NODE_DATA &_ndLeft,
			const YYSTYPE::EE_NODE_DATA &_ndRight,
			YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * Creates a defined() expression.
		 *
		 * \param _ndIdent The identifier.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		bool 										CreateDefined( const YYSTYPE::EE_NODE_DATA &_ndIdent,
			YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * Creates a list of external declarations.
		 *
		 * \param param _ndLeft Left operand.
		 * \param _ndRight Right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		bool 										CreateTranslationUnit( const YYSTYPE::EE_NODE_DATA &_ndLeft,
			const YYSTYPE::EE_NODE_DATA &_ndRight,
			YYSTYPE::EE_NODE_DATA &_ndNode );

		/**
		 * Gets a node for read-only.
		 *
		 * \param _stIndex Index of the node to get.
		 * \return Returns a constant reference to the requested node.
		 */
		const EE_PREPROC_SYNTAX_NODES::EE_NODE_DATA &
													GetNode( size_t _stIndex ) const;

		/**
		 * Gets the total number of nodes.
		 *
		 * \return Returns the total number of nodes in the syntax tree.
		 */
		size_t	 									TotalNodes() const;

		/**
		 * Gets a string from the string pool.
		 *
		 * \param _ui32Index Index of the string to get.
		 * \return Returns the string at the given index.
		 */
		const std::string &							GetString( uint32_t _ui32Index ) const;


	protected :
		// == Members.
		/**
		 * A pool of strings.  Strings are binary and not necessarily terminated with a NULL.
		 */
		std::vector<std::string>					m_vStringPool;

		/**
		 * The stack of nodes.
		 */
		std::vector<EE_PREPROC_SYNTAX_NODES::EE_NODE_DATA>
													m_vNodes;


		// == Functions.
		/**
		 * Adds a node to the stack.  Set the node index too.
		 *
		 * \param _ndNode The node to add to the stack.
		 * \return Returns true if the node was added.
		 */
		bool										AddNode( EE_PREPROC_SYNTAX_NODES::EE_NODE_DATA &_ndNode );

		/**
		 * Adds an identifier by name.  Returns the index of the created identifier in the identifier stack.
		 *	The type of the created identifier is undefined.
		 *
		 * \param _pcText The string name of the identifier.
		 * \return Returns the index of the created identifier.
		 */
		uint32_t									AddIdentifier( const char * _pcText );

		// Returns the index of a string if it exists or static_cast<size_t>(-1) otherwise.
		size_t										HasString( const std::string &_sText ) const;
	};

}	// namespace ee

#endif	// __EE_PREPROCCONTAINER_H__


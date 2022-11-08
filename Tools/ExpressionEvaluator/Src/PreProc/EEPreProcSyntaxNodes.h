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
 * Description: Nodes for the syntax tree of the preprocessor parser.
 */


#ifndef __EE_PREPROCSYNTAXNODES_H__
#define __EE_PREPROCSYNTAXNODES_H__

#include "../EEExpEval.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#ifndef yy
#define yy																ee
#endif	// #ifndef yy
#ifdef YYSTYPE
#undef YYSTYPE
#endif	// #ifdef YYSTYPE
#define YYSTYPE															EE_PREPROC_SYNTAX_NODES

namespace ee {

	/**
	 * Enumerate node types.
	 */
	enum EE_PREPROC_NODE {
		EE_PPN_IDENTIFIER,
		EE_PPN_NUMERICCONSTANT,
		EE_PPN_UNARY,
		EE_PPN_MATH,
		EE_PPN_TERTIARY,
		EE_PPN_DEFINED,
		EE_PPN_TU
	};

	/**
	 * Numeric constant types.
	 */
	enum EE_PREPROC_NUM_CONSTANTS {
		EE_PPNC_SIGNED,
		EE_PPNC_UNSIGNED,
		EE_PPNC_FLOATING,
		EE_PPNC_INVALID,
	};

	/**
	 * Nodes in the parser.
	 */
	union YYSTYPE {
		uint64_t														ui64Op;				// Operators (mainly unary).

		struct EE_NODE_DATA {
			EE_PREPROC_NODE												ppnNodeType;		// One of the EE_PPN_* constants.
			size_t														stNodeIndex;		// Index of the node within the stack of nodes.
			union EE_NODE_UNION {
				uint32_t												ui32IdentifierIndex;// Index of an identifier in the master list of identifiers.
				uint64_t												ui64Const;			// Value of unsigned constant numeric.
				int64_t													i64Const;			// Value of signed constant numeric.
				double													dConst;				// Value of floating-point numeric.
				uint32_t												ui32UnaryOp;		// Unary operator.
				size_t													stNodeIndex;		// Index of a node.  Multi-purpose.
			}															nuNodeData;			// Node data.
			union EE_NODE_UNION_EX {
				EE_PREPROC_NUM_CONSTANTS								ncConstantType;		// Whether the given numeric constant is signed, unsigned, or floating.
				size_t													stNodeIndexEx;	// Index of a node.  Multi-purpose.
			}															nueNodeDataEx;
			union EE_NODE_UNION_OP {
				uint32_t												ui32Op;				// Operator.
				size_t													stNodeIndex3;		// Tertiary expression and initializer index in single declarations.
			}															nuoOp;
		}																ndData;
	};


}	// namespace ee

#endif	// __EE_PREPROCSYNTAXNODES_H__

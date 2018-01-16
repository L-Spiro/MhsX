#pragma once

#include "EEExpEval.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#ifndef yy
#define yy																ee
#endif	// #ifndef yy
#ifdef YYSTYPE
#undef YYSTYPE
#endif	// #ifdef YYSTYPE
#define YYSTYPE															EE_SYNTAX_NODES


namespace ee {

	// == Node types.
	enum EE_NODES {
		EE_N_NUMERICCONSTANT,
		EE_N_IDENTIFIER,
		EE_N_ADDRESS,
		EE_N_MEMBERACCESS,
		EE_N_UNARY,
		EE_N_OP,
		EE_N_CONDITIONAL,
		EE_N_INTRINSIC_1,
		EE_N_INTRINSIC_2,
		EE_N_INTRINSIC_3,
		EE_N_USER_VAR,
		EE_N_ASFLOAT,
		EE_N_ASDOUBLE,
		EE_N_ASXFLOAT,
		EE_N_ASXFLOAT_FROM_DOUBLE,
		EE_N_ASFLOAT16,
		EE_N_ASFLOAT14,
		EE_N_ASFLOAT11,
		EE_N_ASFLOAT10,
	};

	// Constant types.
	enum EE_NUM_CONSTANTS {
		EE_NC_UNSIGNED													= 0,
		EE_NC_SIGNED													= 1,
		EE_NC_FLOATING													= 2,
		EE_NC_INVALID													= 3,
	};

	// Cast types.
	enum EE_CAST_TYPES {
		EE_CT_INT8,
		EE_CT_UINT8,
		EE_CT_INT16,
		EE_CT_UINT16,
		EE_CT_INT32,
		EE_CT_UINT32,
		EE_CT_INT64,
		EE_CT_UINT64,
		EE_CT_FLOAT,
		EE_CT_DOUBLE,
	};

	union YYSTYPE {
		size_t															sStringIndex;						// String index.
		uint32_t														ui32Unary;							// Unary operator.
		struct EE_NODE_DATA {
			EE_NODES													nType;								// Type of the node.
			size_t														sNodeIndex;							// Index of the node in the stack of nodes.
			union {
				uint64_t												ui64Val;							// 64-bit unsigned integer value.
				int64_t													i64Val;								// 64-bit signed integer value.
				double													dVal;								// Double value.
				size_t													sStringIndex;						// Index of the string/identifier.
				size_t													sNodeIndex;							// When the index of another node is needed.
				uint32_t												ui32Intrinsic;						// Intrinsic.
			}															u;
			union {
				EE_NUM_CONSTANTS										ncConstType;						// Type of numeric constant.
				EE_CAST_TYPES											ctCast;								// Casting to a type.
				size_t													sStringIndex;						// Index of the string.
				size_t													sNodeIndex;							// Index of a node.
				uint32_t												ui32Op;								// Operator.
			}															v;
			union {
				size_t													sNodeIndex;							// Node index.
			}															w;
			union {
				size_t													sNodeIndex;							// Node index.
			}															x;
			union {
				size_t													sNodeIndex;							// Node index.
			}															y;
			union {
				size_t													sNodeIndex;							// Node index.
			}															z;
			union {
				size_t													sNodeIndex;							// Node index.
			}															a;
			
		}																ndData;
	};
}	// namespace ee

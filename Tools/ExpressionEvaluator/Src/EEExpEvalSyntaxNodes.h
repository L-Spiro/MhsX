#pragma once

#include "EEExpEval.h"
#include <ctime>


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
		EE_N_STRING,
		EE_N_CUSTOM_VAR,
		EE_N_ARRAY,
		EE_N_STRINGARRAY,
		EE_N_STRINGARRAY_EX,
		EE_N_ARRAY_ACCESS,
		EE_N_ARRAY_ACCESS_EX,
		EE_N_POSTFIX_PLUSPLUS,
		EE_N_POSTFIX_MINUSMINUS,
		EE_N_PREFIX_PLUSPLUS,
		EE_N_PREFIX_MINUSMINUS,
		//EE_N_CAST,
		EE_N_ADDRESS,
		EE_N_MEMBERACCESS,
		EE_N_USER_VAR,
		EE_N_NUMBERED_PARM,
		EE_N_DYNAMIC_NUMBERED_PARM,
		EE_N_DYNAMIC_NUMBERED_PARM_EXP,
		EE_N_NUMBERED_PARM_TOTAL,
		EE_N_UNARY,
		EE_N_CAST,
		EE_N_OP,
		EE_N_CONDITIONAL,
		EE_N_INTRINSIC_0,
		EE_N_INTRINSIC_0_SIGNED,
		EE_N_INTRINSIC_0_UNSIGNED_DIVISOR,
		EE_N_INTRINSIC_1,
		EE_N_INTRINSIC_1_FLOAT_FLOAT,
		EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED16,
		EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED32,
		EE_N_INTRINSIC_1_UNSIGNED_UNSIGNED64,
		EE_N_INTRINSIC_1_BOOL_FLOAT,
		EE_N_INTRINSIC_1_INT_FLOAT,
		EE_N_INTRINSIC_2,
		EE_N_INTRINSIC_2_FLOAT_FLOAT_FLOAT,
		EE_N_INTRINSIC_2_FLOAT_FLOAT32_FLOAT32,
		EE_N_INTRINSIC_2_FLOAT_FLOAT80_FLOAT80,
		EE_N_INTRINSIC_2_FLOAT_FLOAT32_FLOAT80,
		EE_N_INTRINSIC_3,
		EE_N_INTRINSIC_3_FLOAT_FLOAT_FLOAT_FLOAT,
		EE_N_INTRINSIC_3_BOOL_FLOAT32_FLOAT32_FLOAT32,
		EE_N_INTRINSIC_3_BOOL_FLOAT_FLOAT_FLOAT,
		EE_N_ASFLOAT,
		EE_N_ASDOUBLE,
		EE_N_ASXFLOAT,
		EE_N_ASXFLOAT_FROM_DOUBLE,
		EE_N_ASFLOAT16,
		EE_N_ASFLOAT14,
		EE_N_ASFLOAT11,
		EE_N_ASFLOAT10,
		EE_N_ASXFLOAT_MAX,
		EE_N_ASXFLOAT_MIN,
		EE_N_ASXFLOAT_TRUE_MIN,
		EE_N_ASXFLOAT_NAN,
		EE_N_ASXFLOAT_INF,
		EE_N_ASXFLOAT_SUBNORM_MAX,
		EE_N_ASXFLOAT_EPS,
		EE_N_ASXFLOAT_SIGNBIT,
		EE_N_ASXFLOAT_EXPBITS,
		EE_N_ASXFLOAT_MANBITS,
		EE_N_COMPOUND_STATEMENT,
		EE_N_WHILE_LOOP,
		EE_N_FOR_LOOP,
		EE_N_DO_WHILE_LOOP,
		EE_N_FOREACHDECL0,
		EE_N_FOREACHDECL1,
		EE_N_FOREACHDECL2,
		EE_N_FOREACH,
		EE_N_FOREACHOBJ,
		EE_N_FOREACHSTR,
		EE_N_IF,
		EE_N_CONTINUE,
		EE_N_BREAK,
		EE_N_ASSIGNMENT,
		EE_N_CREATE_ARRAY,
		EE_N_ARRAY_ASSIGNMENT,
		EE_N_ADDRESS_ASSIGNMENT,
		EE_N_ARRAY_INITIALIZER,
		EE_N_ARRAY_INITIALIZER_LIST,
		EE_N_ARG_LIST_ENTRY,
		EE_N_ARG_LIST,
		EE_N_FORMAT,
	};

	// Constant types.  Ordered by cast priority.
	enum EE_NUM_CONSTANTS {
		EE_NC_UNSIGNED													= 0x00,
		EE_NC_SIGNED													= 0x01,
		EE_NC_FLOATING													= 0x02,
		EE_NC_OBJECT													= 0x03,
		EE_NC_INVALID													= 0x80,
		//EE_NC_MASK														= 0xFF,
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
		EE_CT_FLOAT10,
		EE_CT_FLOAT11,
		EE_CT_FLOAT14,
		EE_CT_FLOAT16,
		EE_CT_DOUBLE,
	};

	// Function pointers.
	typedef ::clock_t (__cdecl * Intrins0Clock)();
	typedef uint64_t (* Intrins0Unsigned)( uint64_t );
	typedef double (__cdecl * Intrins1_Float_Float)( double );
	typedef uint16_t (__cdecl * Intrins1_Unsigned16_Unsigned16)( uint16_t );
	typedef unsigned long (__cdecl * Intrins1_Unsigned32_Unsigned32)( unsigned long );							// Because of ::_byteswap_ulong().
	typedef uint64_t (__cdecl * Intrins1_Unsigned64_Unsigned64)( uint64_t );
	typedef bool (__cdecl * Intrins1_Bool_Float)( double );
	typedef int (__cdecl * Intrins1_Signed_Float)( double );
	typedef double (__cdecl * Intrins2_Float_Float_Float)( double, double );
	typedef float (__cdecl * Intrins2_Float_Float32_Float32)( float, float );
	typedef long double (__cdecl * Intrins2_Float_Float80_Float80)( long double, long double );
	typedef float (__cdecl * Intrins2_Float_Float32_Float80)( float, long double );
	typedef double (__cdecl * Intrins3_Float_Float_Float_Float)( double, double, double );
	typedef bool (__cdecl * Intrins3_Bool_Float32_Float32_Float32)( float, float, float );
	typedef bool (__cdecl * Intrins3_Bool_Float_Float_Float)( double, double, double );

	union YYSTYPE {
		size_t															sStringIndex;							// String index.
		uint32_t														ui32Unary;								// Unary operator.
		uint32_t														ui32Backing;							// Backing type (for creating arrays).
		struct EE_NODE_DATA {
			EE_NODES													nType;									// Type of the node.
			size_t														sNodeIndex;								// Index of the node in the stack of nodes.
			union {
				uint64_t												ui64Val;								// 64-bit unsigned integer value.
				int64_t													i64Val;									// 64-bit signed integer value.
				double													dVal;									// Double value.
				size_t													sStringIndex;							// Index of the string/identifier.
				size_t													sNodeIndex;								// When the index of another node is needed.
				uint32_t												ui32Intrinsic;							// Intrinsic.
			}															u;
			union {
				EE_NUM_CONSTANTS										ncConstType;							// Type of numeric constant.
				EE_CAST_TYPES											ctCast;									// Casting to a type.
				size_t													sStringIndex;							// Index of the string.
				size_t													sNodeIndex;								// Index of a node.
				uint32_t												ui32Op;									// Operator.
				bool													bIsConst;								// Is the user variable const?
			}															v;
			union {
				size_t													sNodeIndex;								// Node index.
			}															w;
			union {
				size_t													sNodeIndex;								// Node index.
			}															x;
			union {
				size_t													sNodeIndex;								// Node index.
			}															y;
			union {
				size_t													sNodeIndex;								// Node index.
			}															z;
			union {
				size_t													sNodeIndex;								// Node index.
			}															a;
			union {
				Intrins0Clock											pfClock;								// ::clock().
				Intrins0Unsigned										pfIntrins0Unsigned;						// Any no-parameter instrinsic that returns a uint64_t.
				Intrins1_Float_Float									pfIntrins1Float_Float;					// Any single-parameter function that takes a double and gives back a double.
				Intrins1_Unsigned16_Unsigned16							pfIntrins1Unsigned_Unsigned16;			// Any single-paremeter function that takes a uint16_t and returns a uint16_t.
				Intrins1_Unsigned32_Unsigned32							pfIntrins1Unsigned_Unsigned32;			// Any single-paremeter function that takes a uint32_t and returns a uint32_t.
				Intrins1_Unsigned64_Unsigned64							pfIntrins1Unsigned_Unsigned64;			// Any single-paremeter function that takes a uint64_t and returns a uint64_t.
				Intrins1_Bool_Float										pfIntrins1Bool_Float;					// Any single-paremeter function that takes a double and returns a bool.
				Intrins1_Signed_Float									pfIntrins1Signed_Float;					// Any single-paremeter function that takes a double and returns an int.
				Intrins2_Float_Float_Float								pfIntrins2Float_Float_Float;			// Any 2-parameter function that takes all doubles and returns a double.
				Intrins2_Float_Float32_Float32							pfIntrins2Float32_Float32_Float32;		// Any 2-parameter function that takes all floats and returns a float.
				Intrins2_Float_Float80_Float80							pfIntrins2Float80_Float80_Float80;		// Any 2-parameter function that takes all long doubles and returns a long double.
				Intrins2_Float_Float32_Float80							pfIntrins2Float32_Float32_Float80;		// Any 2-parameter function that takes a float and long double and returns a float.
				Intrins3_Float_Float_Float_Float						pfIntrins3Float_Float_Float_Float;		// Any 3-parameter function that takes 3 doubles and returns a double.
				Intrins3_Bool_Float32_Float32_Float32					pfIntrins3Bool_Float32_Float32_Float32;	// Any 3-parameter function that takes 3 doubles and returns a bool.
				Intrins3_Bool_Float_Float_Float							pfIntrins3Bool_Float_Float_Float;		// Any 3-parameter function that takes 3 doubles and returns a bool.
			}															uFuncPtr;
			
		}																ndData;
	};
}	// namespace ee

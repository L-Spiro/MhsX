#pragma once


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#ifndef yy
#define yy																lson
#endif	// #ifndef yy
#ifdef YYSTYPE
#undef YYSTYPE
#endif	// #ifdef YYSTYPE
#define YYSTYPE															LSON_SYNTAX_NODES


namespace lson {

	// == Node types.
	enum LSON_NODES {
		LSON_N_MEMBER,
		LSON_N_MEMBER_LIST,
		LSON_N_VALUE,
	};

	enum LSON_VALUES {
		LSON_V_OBJECT,
		LSON_V_STRING,
		LSON_V_DECIMAL,
		LSON_V_ARRAY,
		LSON_V_TRUE,
		LSON_V_FALSE,
		LSON_V_NULL,
	};


	union YYSTYPE {
		size_t															sStringIndex;							/**< String index. */
		double															dDecimal;								/**< Decimal value. */
		struct LSON_NODE {
			LSON_NODES													nType;									/**< The node type. */
			size_t														stNodeIdx;								/**< The node index. */
			union {
				struct LSON_VALUE {
					LSON_VALUES											vType;									/**< The type of value. */
					union {
						size_t											stValue;								/**< The object node index if vType is LSON_V_OBJECT, string index if vType is LSON_V_STRING, array node index if vType is LSON_V_ARRAY. */
						double											dDecimal;								/**< The value, if vType is LSON_V_DECIMAL. */
					}													v;
				}														vValue;
				struct LSON_MEMBER {
					size_t												stName;									/**< The name of the member. */
					size_t												stValue;								/**< The value of the member. */
				}														mMember;
				struct LSON_MEMBER_LIST {
					size_t												stLeft;									/**< The left node. */
					size_t												stRight;								/**< The right node. */
				}														mlMembers;
			}															u;
		}																nNode;
	};

}	// namespace lson

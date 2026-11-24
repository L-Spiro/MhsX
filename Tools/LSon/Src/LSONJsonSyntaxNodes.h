#pragma once

#include <cstddef>


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

	/**
	 * Union YYSTYPE
	 * \brief The semantic value of a symbol of the grammar.
	 *
	 * Description: The semantic value of a symbol of the grammar.  The parser builds the final values (CJsonContainer::LSON_JSON_VALUE)
	 *	as it reduces, so only indices travel on the parser's stack.  Keep this small:  the parser copies it with every shift and reduction.
	 */
	union YYSTYPE {
		size_t															sStringIndex;							/**< String index. */
		double															dDecimal;								/**< Decimal value. */
		size_t															stValue;								/**< The index of a value (CJsonContainer::GetValue()). */
		/** A member of an object. */
		struct LSON_MEMBER {
			size_t														stName;									/**< The string index of the name of the member. */
			size_t														stValue;								/**< The index of the value of the member. */
		}																mMember;
	};

}	// namespace lson

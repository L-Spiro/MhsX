#pragma once

/**
 * Copyright L. Spiro 2026
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Replaces the stack of the Bison 2.4 lalr1.cc skeleton (stack.hh), which keeps its items in a std::deque and pushes them onto
 *	the front, with one that keeps them in a std::vector and pushes them onto the back.  The parser pushes and pops its state, value,
 *	and location for every token and every reduction, so this is the hottest container of the parser.  LSONJsonSyntaxBuild.bat copies this
 *	file over the generated one (as ../Src/Gen/LSONJsonParserStack.h).  The interface is the same as the skeleton's:  index 0 is the top.
 */

#ifndef __LSON_LSONMLPARSERSTACK_H__
#define __LSON_LSONMLPARSERSTACK_H__

#include <vector>
#ifdef yy
#undef yy
#endif  // yy
#define yy                  lson


namespace yy {

	/**
	 * Class stack
	 * \brief A stack whose index 0 is its top.
	 *
	 * Description: A stack whose index 0 is its top, kept in a vector whose back is the top.
	 */
	template <class T, class S = std::vector<T>>
	class stack {
	public :
		// == Types.
		/** Iterates from the bottom to the top. */
		typedef typename S::iterator					iterator;
		/** Iterates from the bottom to the top. */
		typedef typename S::const_iterator				const_iterator;


		stack() {
			seq_.reserve( 256 );
		}
		stack( unsigned int _uiN ) :
			seq_( _uiN ) {
			seq_.reserve( 256 );
		}


		// == Operators.
		/**
		 * Gets an item counting down from the top.
		 *
		 * \param _uiI The index of the item (0 is the top).
		 * \return Returns the item.
		 */
		inline T &										operator [] ( unsigned int _uiI ) { return seq_[seq_.size()-1-_uiI]; }

		/**
		 * Gets an item counting down from the top.
		 *
		 * \param _uiI The index of the item (0 is the top).
		 * \return Returns the item.
		 */
		inline const T &								operator [] ( unsigned int _uiI ) const { return seq_[seq_.size()-1-_uiI]; }


		// == Functions.
		/**
		 * Pushes an item onto the top.
		 *
		 * \param _tVal The item to push.
		 */
		inline void										push( const T &_tVal ) { seq_.push_back( _tVal ); }

		/**
		 * Pops items off of the top.
		 *
		 * \param _uiN The number of items to pop.
		 */
		inline void										pop( unsigned int _uiN = 1 ) { seq_.resize( seq_.size() - _uiN ); }

		/**
		 * Gets the number of items.
		 *
		 * \return Returns the number of items.
		 */
		inline size_t									height() const { return seq_.size(); }

		/**
		 * Gets an iterator to the bottom.
		 *
		 * \return Returns an iterator to the bottom.
		 */
		inline const_iterator							begin() const { return seq_.begin(); }

		/**
		 * Gets an iterator past the top.
		 *
		 * \return Returns an iterator past the top.
		 */
		inline const_iterator							end() const { return seq_.end(); }


	private :
		// == Members.
		S												seq_;											/**< The items (the back is the top). */
	};

	/**
	 * Class slice
	 * \brief Presents a slice of the top of a stack.
	 *
	 * Description: Presents a slice of the top of a stack.
	 */
	template <class T, class S = stack<T>>
	class slice {
	public :
		slice( const S &_sStack, unsigned int _uiRange ) :
			stack_( _sStack ),
			range_( _uiRange ) {
		}


		// == Operators.
		/**
		 * Gets an item of the slice.
		 *
		 * \param _uiI The index of the item.
		 * \return Returns the item.
		 */
		inline const T &								operator [] ( unsigned int _uiI ) const { return stack_[range_-_uiI]; }


	private :
		// == Members.
		const S &										stack_;											/**< The stack. */
		unsigned int									range_;											/**< The range of the slice. */
	};

}	// namespace yy


#endif	// #ifndef __LSON_LSONMLPARSERSTACK_H__

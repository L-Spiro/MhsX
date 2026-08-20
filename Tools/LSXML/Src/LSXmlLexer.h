#pragma once

#include "LSXml.h"
#include "LSXmlContainer.h"

#ifdef yyFlexLexer
#undef yyFlexLexer
#endif
#define yyFlexLexer			CXmlLexBase

#include "Gen/LSXmlLexBase.h"
#include "Gen/LSXmlParser.h"


namespace lsx {

	class CXmlLexer : public CXmlLexBase {
	public :
		// == Various constructors.
		CXmlLexer( std::istream * _pisStream, std::ostream * _posStream = nullptr ) :
			Parent( _pisStream, _posStream ),
			m_pxlContainer( nullptr ) {
		}
		virtual ~CXmlLexer() {
		}


		// == Functions.
		/**
		 * Scans directly from a contiguous block of memory.
		 * The buffer MUST end with two NUL ('\0') bytes.
		 *
		 * \param _pcBase The contiguous block of memory to scan.
		 * \param _stSize The total size of the memory block, including the two NUL bytes.
		 * \return Returns the new buffer state, or nullptr if the buffer is invalid.
		 */
		struct yy_buffer_state *						ScanMemoryBuffer( char * _pcBase, size_t _stSize );

	protected :
		// == Members.
		// The container that handles collection of all of the script data.
		const CXmlContainer *							m_pxlContainer;


	private :
		typedef CXmlLexBase								Parent;
	};

}	// namespace lsx

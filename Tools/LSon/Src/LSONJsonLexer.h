#pragma once

#include "LSONJson.h"
#include "LSONJsonContainer.h"

#ifdef yyFlexLexer
#undef yyFlexLexer
#endif
#define yyFlexLexer			CJsonLexBase

#include "Gen/LSONJsonLexBase.h"
#include "Gen/LSONJsonParser.h"


namespace lson {

	class CJsonLexer : public CJsonLexBase {
	public :
		// == Various constructors.
		CJsonLexer( std::istream * _pisStream, std::ostream * _posStream = nullptr ) :
			Parent( _pisStream, _posStream ),
			m_pxlContainer( nullptr ) {
		}
		virtual ~CJsonLexer() {
		}


	protected :
		// == Members.
		// The container that handles collection of all of the script data.
		const CJsonContainer *								m_pxlContainer;


	private :
		typedef CJsonLexBase								Parent;
	};

}	// namespace lson

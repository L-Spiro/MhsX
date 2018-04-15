#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXProcess.h"
#include "MXAddressChunkList.h"
#include <cassert>


namespace mx {

	class CSearcher {
	public :
		CSearcher( CProcess * _ppProcess ) :
			m_ppProcess( _ppProcess ) {
			assert( m_ppProcess != nullptr );
		}
		~CSearcher() {
			m_ppProcess = nullptr;
		}


		// == Types.
		struct MX_SEARCH_CHUNK_OPTIONS {
			bool							bStaticOnly;
			DWORD							dwTypes;
		};


		// == Functions.
		// List a list of chunks.
		bool								GenerateChunks( CAddressChunkList &_aclList, uint64_t _ui64MaxLen, uint64_t _ui64Start = 0, uint64_t _ui64End = static_cast<uint64_t>(-16) );


	protected :
		// == Members.
		// The process.
		CProcess *							m_ppProcess;


		// == Functions.
	};

}	// namespace mx

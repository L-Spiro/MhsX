#pragma once
#include "../MXMhsX.h"
#include <vector>


namespace mx {

	class CAddressChunkList {
	public :
		CAddressChunkList() {
		}
		~CAddressChunkList() {
		}


		// == Types.
		// A chunk.
		struct MX_CHUNK {
			uint64_t				tStart;
			uint64_t				sLen;
		};


		// == Functions.
		// Gets the chunks.
		const std::vector<MX_CHUNK> &
									Chunks() const { return m_vChunks; }

		// Adds a chunk.
		bool						Add( uint64_t _ui64Addr, uint64_t _ui64Len ) {
			size_t sLen = m_vChunks.size() + 1;
			m_vChunks.push_back( MX_CHUNK { _ui64Addr, _ui64Len } );
			return m_vChunks.size() == sLen;
		}


	protected :
		// == Members.
		// List of chunks.
		std::vector<MX_CHUNK>		m_vChunks;

	};

}	// namespace mx

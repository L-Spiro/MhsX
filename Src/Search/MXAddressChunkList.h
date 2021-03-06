#pragma once
#include "../MXMhsX.h"
#include <algorithm>
#include <vector>


namespace mx {

	class CAddressChunkList {
	public :
		CAddressChunkList() :
			m_ui64Largest( 0 ) {
		}
		~CAddressChunkList() {
		}


		// == Types.
		// A chunk.
		struct MX_CHUNK {
			uint64_t				ui64Start;
			uint64_t				ui64Len;
		};


		// == Functions.
		// Gets the chunks.
		const std::vector<MX_CHUNK> &
									Chunks() const { return m_vChunks; }

		// Adds a chunk.
		bool						Add( uint64_t _ui64Addr, uint64_t _ui64Len ) {
			size_t sLen = m_vChunks.size() + 1;
			try {
				m_vChunks.push_back( MX_CHUNK { _ui64Addr, _ui64Len } );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {
				return false;
			}
			m_ui64Largest = std::max( m_ui64Largest, _ui64Len );
			return m_vChunks.size() == sLen;
		}

		// Largest chunk added.
		uint64_t					LargestChunk() const { return m_ui64Largest; }

		// Largest chunk between the given start and end points.
		uint64_t					LargestChunk( size_t _sStartIdx, size_t _sEndIdx ) const {
			uint64_t uiSize = 0;
			for ( size_t I = _sStartIdx; I < _sEndIdx && I < m_vChunks.size(); ++I ) {
				uiSize = std::max( uiSize, m_vChunks[I].ui64Len );
			}
			return uiSize;
		}

		// Gets the chunk containing the given address.
		bool						FindChunk( uint64_t _ui64Addr, MX_CHUNK &_cChunk ) const {
			for ( size_t I = 0; I < m_vChunks.size(); ++I ) {
				if ( m_vChunks[I].ui64Start <= _ui64Addr && (m_vChunks[I].ui64Start + m_vChunks[I].ui64Len) > _ui64Addr ) {
					_cChunk = m_vChunks[I];
					return true;
				}
			}
			return false;
		}

		// Gets the chunk containing the given address.  Begins the search on the given index.
		bool						FindChunk( uint64_t _ui64Addr, MX_CHUNK &_cChunk, size_t &_stSearchStart ) const {
			if ( _stSearchStart >= m_vChunks.size() ) { return false; }
			for ( size_t I = _stSearchStart; I < m_vChunks.size(); ++I ) {
				if ( m_vChunks[I].ui64Start <= _ui64Addr && (m_vChunks[I].ui64Start + m_vChunks[I].ui64Len) > _ui64Addr ) {
					_cChunk = m_vChunks[I];
					_stSearchStart = I;
					return true;
				}
			}
			for ( size_t I = 0; I < _stSearchStart; ++I ) {
				if ( m_vChunks[I].ui64Start <= _ui64Addr && (m_vChunks[I].ui64Start + m_vChunks[I].ui64Len) > _ui64Addr ) {
					_cChunk = m_vChunks[I];
					_stSearchStart = I;
					return true;
				}
			}
			return false;
		}


	protected :
		// == Members.
		// List of chunks.
		std::vector<MX_CHUNK>		m_vChunks;

		// Largest chunk that has been added.
		uint64_t					m_ui64Largest;

	};

}	// namespace mx

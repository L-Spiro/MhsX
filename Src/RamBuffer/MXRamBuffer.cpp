#include "MXRamBuffer.h"


namespace mx {

	// Reads from target process using buffered values.  Updates the buffer if any part of the read value is out of range of what is buffered.
	bool CRamBuffer::Read( uint64_t _ui64Address, void * _pvDest, size_t _stSize ) {
		uint64_t ui64Buffered;
		if ( !Buffer( _ui64Address, ui64Buffered ) ) { return false; }

		uint8_t * pui8Dst = static_cast<uint8_t *>(_pvDest);
		_ui64Address -= m_ui64Low;
		// Buffer as many times as it takes to get the whole data.
		while ( _stSize > ui64Buffered ) {
			
			std::memcpy( pui8Dst, &m_vBuffer[_ui64Address], ui64Buffered );
			_stSize -= ui64Buffered;
			_ui64Address += ui64Buffered;
			pui8Dst += ui64Buffered;

			// Still more to read?
			if ( _stSize > ui64Buffered ) {
				if ( !Buffer( _ui64Address, ui64Buffered ) ) { return false; }
			}
		}

		// The mount left to read is less than what is buffered.
		std::memcpy( pui8Dst, &m_vBuffer[_ui64Address], _stSize );
		return true;
	}

	// Buffers the chunk containing the given address.  Returns the amount of data from the address to the end of the buffer.
	bool CRamBuffer::Buffer( uint64_t _ui64Address, uint64_t &_ui64AmountAfterAddr ) {
		
		if ( _ui64Address >= m_ui64Low && _ui64Address < m_ui64Hi ) {
			_ui64AmountAfterAddr = m_ui64Hi - _ui64Address;
			return true;
		}
		if ( !m_ppProcess || !m_paclChunks ) { return false; }

		CAddressChunkList::MX_CHUNK cChunk;
		if ( !m_paclChunks->FindChunk( _ui64Address, cChunk, m_stSearchIdx ) ) {
			return false;
		}

		// Set the chunk to the range we need from it.
		// Round down to the nearest 16 bytes.
		uint64_t ui64NewEnd = cChunk.ui64Start + cChunk.ui64Len;
		uint64_t ui64NewStart = _ui64Address / 16 * 16;
		
		cChunk.ui64Start = ui64NewStart;
		cChunk.ui64Len = ui64NewEnd - ui64NewStart;

		// Clamp the end of the buffer by how much we are allowed to read.
		cChunk.ui64Len = std::min( cChunk.ui64Len, m_ui64Max );

		// If rounding down caused the desired address to fall out of range, extend the range, even if it means barely going over m_ui64Max.
		// We must at least include 1 requested byte.
		cChunk.ui64Len = std::max( _ui64Address - ui64NewStart + 1, cChunk.ui64Len );


		if ( !Alloc( cChunk.ui64Len ) ) {
			return false;
		}
		SIZE_T stRead = 0;
		if ( !m_ppProcess->ReadProcessMemory( cChunk.ui64Start, &m_vBuffer[0], static_cast<SIZE_T>(cChunk.ui64Len), &stRead ) ) {
			return false;
		}
		if ( m_pfPreProcFunc ) {
			m_pfPreProcFunc( &m_vBuffer[0], stRead, m_ui64PreProcParm );
		}
		_ui64AmountAfterAddr = stRead + cChunk.ui64Start - _ui64Address;
		m_ui64Low = cChunk.ui64Start;
		m_ui64Hi = m_ui64Low + stRead;

		return true;
	}

	// Attempts to allocate a buffer of the given size.  If allocation fails, the size is reduced until it succeeds.
	bool CRamBuffer::Alloc( uint64_t &_ui64SizeOfAloc ) {
		while ( m_vBuffer.size() < _ui64SizeOfAloc ) {
			try {
				m_vBuffer.resize( _ui64SizeOfAloc );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			_ui64SizeOfAloc = std::max( _ui64SizeOfAloc, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !_ui64SizeOfAloc ) {
				if ( !m_vBuffer.size() ) {
					return false;
				}
				_ui64SizeOfAloc = m_vBuffer.size();	// We know it will be smaller than the starting amount because this while loop
													//	can only be entered on that condition.
				return true;
			}
		}
		return true;
	}

}	// namespace mx

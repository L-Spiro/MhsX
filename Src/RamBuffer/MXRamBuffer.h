#pragma once
#include "../MemHack/MXProcess.h"
#include "../Search/MXAddressChunkList.h"
#include <vector>

namespace mx {

	class CRamBuffer {
	public :
		CRamBuffer( CProcess * _ppProcess, const CAddressChunkList * _paclChunks, uint64_t _ui64Max ) :
			m_ui64Low( 0 ),
			m_ui64Hi( 0 ),
			m_ppProcess( _ppProcess ),
			m_paclChunks( _paclChunks ),
			m_ui64Max( _ui64Max ),
			m_stSearchIdx( 0 ) {
		}
		~CRamBuffer() {
		}


		// == Types.
		// Byte preprocessing.
		typedef void						(* pfPreprocessFunc)( void * _pvData, size_t _stLen, uint64_t _ui64Parm );


		// == Functions.
		// Reads from target process using buffered values.  Updates the buffer if any part of the read value is out of range of what is buffered.
		bool								Read( uint64_t _ui64Address, void * _pvDest, size_t _stSize );

		// Sets the preprocessing function pointer.
		void								SetPreproc( pfPreprocessFunc _pfFunc, uint64_t _ui64Parm ) {
			m_pfPreProcFunc = _pfFunc;
			m_ui64PreProcParm = _ui64Parm;
		}


	protected:
		// == Members.
		// The range buffered (low).
		uint64_t							m_ui64Low;
		// The range buffered (high).
		uint64_t							m_ui64Hi;
		// The max size to buffer.
		uint64_t							m_ui64Max;
		// Preprocessing parameter.
		uint64_t							m_ui64PreProcParm;
		// The process from which to read.
		CProcess *							m_ppProcess;
		// The chunk list.
		const CAddressChunkList *			m_paclChunks;
		// The preprocessing function.
		pfPreprocessFunc					m_pfPreProcFunc;
		// The buffer.
		std::vector<uint8_t>				m_vBuffer;
		// The chunk-scanning index;
		size_t								m_stSearchIdx;


		// == Functions.
		// Buffers the chunk containing the given address.  Returns the amount of data from the address to the end of the buffer.
		bool								Buffer( uint64_t _ui64Address, uint64_t &_ui64AmountAfterAddr );

		// Attempts to allocate a buffer of the given size.  If allocation fails, the size is reduced until it succeeds.
		bool								Alloc( uint64_t &_ui64SizeOfAloc );
	};

}	// namespace mx

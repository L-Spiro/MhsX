#pragma once
#include "../MXMhsX.h"

namespace mx {

	class CInOutInterface {
	public :
		// == Functions.
		// Read from the given data stream.
		virtual bool				Read( uint64_t _ui64Addr, void * _pvDest, size_t _sSize, size_t * _psBytesRead = nullptr ) const = 0;

		// Write to the given data stream.
		virtual bool				Write( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize, size_t * _psBytesWritten = nullptr ) = 0;
	};

}	// namespace mx

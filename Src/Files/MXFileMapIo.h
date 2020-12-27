#pragma once
#include "../MXMhsX.h"
#include "MXFileMap.h"
#include "../IO/MXInOutInterface.h"

namespace mx {

	class CFileMapIo : public CInOutInterface, CFileMap {
	public :
		CFileMapIo() {}
		~CFileMapIo() {}


		// == Functions.
		// Read from the given data stream.
		virtual bool				Read( uint64_t _ui64Addr, void * _pvDest, size_t _sSize, size_t * _psBytesRead = nullptr ) const {
			DWORD dwRead = CFileMap::Read( _pvDest, _ui64Addr, _sSize );
			if ( _psBytesRead ) { (*_psBytesRead) = dwRead; }
			return dwRead == _sSize;
		}

		// Write to the given data stream.
		virtual bool				Write( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize, size_t * _psBytesWritten = nullptr ) {
			DWORD dwWritten = CFileMap::Write( _pvSrc, _ui64Addr, _sSize );
			if ( _psBytesWritten ) { (*_psBytesWritten) = dwWritten; }
			return dwWritten == _sSize;
		}

	};

}

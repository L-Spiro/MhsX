#pragma once
#include "../MXMhsX.h"
#include "MXFile.h"
#include "../IO/MXInOutInterface.h"
#include "../Utilities/MXUtilities.h"

namespace mx {

	class CFileIo : public CInOutInterface, CFile {
	public :
		CFileIo() {}
		~CFileIo() {}


		// == Functions.
		// Read from the given data stream.
		virtual bool				Read( uint64_t _ui64Addr, void * _pvDest, size_t _sSize, size_t * _psBytesRead = nullptr ) const {
			UINT64 ui64MoveTo = MovePointerTo( _ui64Addr );
			if ( ui64MoveTo != _ui64Addr ) {
				if ( _psBytesRead ) { (*_psBytesRead) = 0; }
				return false;
			}
			DWORD dwBytesRead = 0;
			if ( CFile::Read( _pvDest, _sSize, &dwBytesRead ) ) {
				if ( _psBytesRead ) { (*_psBytesRead) = dwBytesRead; }
				if ( dwBytesRead != _sSize ) {
					return false;
				}
				return true;
			}
			if ( _psBytesRead ) { (*_psBytesRead) = 0; }
			return false;
		}

		// Write to the given data stream.
		virtual bool				Write( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize, size_t * _psBytesWritten = nullptr ) {
			UINT64 ui64MoveTo = MovePointerTo( _ui64Addr );
			if ( ui64MoveTo != _ui64Addr ) {
				if ( _psBytesWritten ) { (*_psBytesWritten) = 0; }
				return false;
			}

			DWORD dwBytesWritten = 0;
			if ( CFile::Write( _pvSrc, _sSize, &dwBytesWritten ) ) {
				if ( _psBytesWritten ) { (*_psBytesWritten) = dwBytesWritten; }
				if ( dwBytesWritten != _sSize ) {
					return false;
				}
				return true;
			}
			if ( _psBytesWritten ) { (*_psBytesWritten) = 0; }
			return false;
		}

	};

}

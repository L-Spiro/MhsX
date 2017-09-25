#include "MXFile.h"


namespace mx {

	CFile::CFile() :
		m_hFile( INVALID_HANDLE_VALUE ) {
	}
	CFile::~CFile() {
		Close();
	}

	// == Functions.
	// Opens an existing file for read or write.
	BOOL CFile::OpenFile( LPCSTR _lpcFile, BOOL _bOpenForRead ) {
		Close();
		m_hFile = ::CreateFileA( _lpcFile,
			_bOpenForRead ? GENERIC_READ : GENERIC_WRITE,
			_bOpenForRead ? FILE_SHARE_READ : FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

		if ( m_hFile == INVALID_HANDLE_VALUE ) {
			return FALSE;
		}
		return TRUE;
	}

	// Opens an existing file for read or write.
	BOOL CFile::OpenFile( LPCWSTR _lpwFile, BOOL _bOpenForRead ) {
		Close();
		m_hFile = ::CreateFileW( _lpwFile,
			_bOpenForRead ? GENERIC_READ : GENERIC_WRITE,
			_bOpenForRead ? FILE_SHARE_READ : FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

		if ( m_hFile == INVALID_HANDLE_VALUE ) {
			return FALSE;
		}
		return TRUE;
	}

	// Creates a file and opens for writing.
	BOOL CFile::CreateNewFile( LPCSTR _lpcFile, BOOL _bFailIfExists ) {
		Close();
		m_hFile = ::CreateFileA( _lpcFile,
              FILE_APPEND_DATA,
              FILE_SHARE_READ,
              NULL,
              _bFailIfExists ? CREATE_NEW : CREATE_ALWAYS,
              FILE_ATTRIBUTE_NORMAL,
              NULL );
		if ( m_hFile == INVALID_HANDLE_VALUE ) {
			return FALSE;
		}
		return TRUE;
	}

	// Creates a file and opens for writing.
	BOOL CFile::CreateNewFile( LPCWSTR _lpwFile, BOOL _bFailIfExists ) {
		Close();
		m_hFile = ::CreateFileW( _lpwFile,
              FILE_APPEND_DATA,
              FILE_SHARE_READ,
              NULL,
              _bFailIfExists ? CREATE_NEW : CREATE_ALWAYS,
              FILE_ATTRIBUTE_NORMAL,
              NULL );
		if ( m_hFile == INVALID_HANDLE_VALUE ) {
			return FALSE;
		}
		return TRUE;
	}

	// Closes the opened file.
	VOID CFile::Close() {
		if ( m_hFile != INVALID_HANDLE_VALUE ) {
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}

	// Reads from the opened file.
	DWORD CFile::Read( LPVOID _lpvBuffer, DWORD _dwNumberOfBytesToRead ) const {
		if ( m_hFile == INVALID_HANDLE_VALUE ) { return 0; }
		DWORD dwRead = 0;
		if ( ::ReadFile( m_hFile, _lpvBuffer, _dwNumberOfBytesToRead, &dwRead, NULL ) ) { return dwRead; }
		return 0;
	}

	// Writes to the opened file.
	DWORD CFile::Write( LPCVOID _lpvBuffer, DWORD _dwNumberOfBytesToWrite ) {
		if ( m_hFile == INVALID_HANDLE_VALUE ) { return 0; }
		DWORD dwWritten = 0;
		if ( ::WriteFile( m_hFile, _lpvBuffer, _dwNumberOfBytesToWrite, &dwWritten, NULL ) ) { return dwWritten; }
		return 0;
	}

	// Gets the size of the file.
	UINT64 CFile::Size() const {
		return Size( m_hFile );
	}

	// Moves the file pointer from the current position and returns the new position.
	UINT64 CFile::MovePointerBy( INT64 _i64Offset ) {
		if ( m_hFile == INVALID_HANDLE_VALUE ) { return 0; }
		LARGE_INTEGER liMoveBy;
		liMoveBy.QuadPart = _i64Offset;
		LARGE_INTEGER liNewPos;
		if ( ::SetFilePointerEx( m_hFile, liMoveBy, &liNewPos, FILE_CURRENT ) ) { return liNewPos.QuadPart; }
		return 0;
	}

	// Moves the file pointer to the given file position.
	UINT64 CFile::MovePointerTo( UINT64 _ui64Pos, BOOL _bFromEnd ) {
		if ( m_hFile == INVALID_HANDLE_VALUE ) { return 0; }
		LARGE_INTEGER liMoveBy;
		liMoveBy.QuadPart = _ui64Pos;
		LARGE_INTEGER liNewPos;
		if ( ::SetFilePointerEx( m_hFile, liMoveBy, &liNewPos, _bFromEnd ? FILE_END : FILE_BEGIN ) ) { return liNewPos.QuadPart; }
		return 0;
	}

	// == Statics.
	// Gets the size of a file given its path.
	UINT64 CFile::Size( LPCSTR _lpcFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExA( _lpcFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.nFileSizeHigh) << 32) | wfadData.nFileSizeLow;
		}
		return 0;
	}

	// Gets the size of a file given its path.
	UINT64 CFile::Size( LPCWSTR _lpwFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExW( _lpwFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.nFileSizeHigh) << 32) | wfadData.nFileSizeLow;
		}
		return 0;
	}

	// Gets the size of the file.
	UINT64 CFile::Size( HANDLE _hFile ) {
		if ( _hFile == INVALID_HANDLE_VALUE ) { return 0; }
		LARGE_INTEGER liInt;
		if ( ::GetFileSizeEx( _hFile, &liInt ) ) { return liInt.QuadPart; }
		return 0;
	}

	// Gets the creation time of a file.
	UINT64 CFile::CreationTime( LPCSTR _lpcFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExA( _lpcFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.ftCreationTime.dwHighDateTime) << 32) | wfadData.ftCreationTime.dwLowDateTime;
		}
		return 0;
	}

	// Gets the creation time of a file.
	UINT64 CFile::CreationTime( LPCWSTR _lpwFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExW( _lpwFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.ftCreationTime.dwHighDateTime) << 32) | wfadData.ftCreationTime.dwLowDateTime;
		}
		return 0;
	}

	// Gets the last access time of a file.
	UINT64 CFile::LastAccessTime( LPCSTR _lpcFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExA( _lpcFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.ftLastAccessTime.dwHighDateTime) << 32) | wfadData.ftLastAccessTime.dwLowDateTime;
		}
		return 0;
	}

	// Gets the last access time of a file.
	UINT64 CFile::LastAccessTime( LPCWSTR _lpwFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExW( _lpwFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.ftLastAccessTime.dwHighDateTime) << 32) | wfadData.ftLastAccessTime.dwLowDateTime;
		}
		return 0;
	}

	// Gets the last write time of a file.
	UINT64 CFile::LastWriteTime( LPCSTR _lpcFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExA( _lpcFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.ftLastWriteTime.dwHighDateTime) << 32) | wfadData.ftLastWriteTime.dwLowDateTime;
		}
		return 0;
	}

	// Gets the last write time of a file.
	UINT64 CFile::LastWriteTime( LPCWSTR _lpwFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExW( _lpwFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.ftLastWriteTime.dwHighDateTime) << 32) | wfadData.ftLastWriteTime.dwLowDateTime;
		}
		return 0;
	}

	// Gets the file attributes (flags, creation time, write time, access time, and size).
	CFile::MX_FILE_ATTR CFile::Attributes( LPCSTR _lpcFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExA( _lpcFile, GetFileExInfoStandard, &wfadData ) ) {
			return Win32AttrToMxAttr( wfadData );
		}
		MX_FILE_ATTR faAttr = { 0 };
		return faAttr;
	}

	// Gets the file attributes (flags, creation time, write time, access time, and size).
	CFile::MX_FILE_ATTR CFile::Attributes( LPCWSTR _lpwFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExW( _lpwFile, GetFileExInfoStandard, &wfadData ) ) {
			return Win32AttrToMxAttr( wfadData );
		}
		MX_FILE_ATTR faAttr = { 0 };
		return faAttr;
	}

	// Loads a file to memory.
	std::vector<uint8_t> & CFile::LoadToMemory( LPCSTR _lpcFile, std::vector<uint8_t> &_vRet ) {
		_vRet.clear();
		UINT64 ui64Size = Size( _lpcFile );
		if ( !ui64Size ) { return _vRet; }
		size_t sSize = static_cast<size_t>(ui64Size);
		if ( ui64Size != sSize ) { return _vRet; }	// Ensure that size_t is large enough to load the file.
		_vRet.resize( sSize );

		CFile fFile;
		if ( !fFile.OpenFile( _lpcFile, TRUE ) ) { return _vRet; }
		size_t sBufferSize = 0;
		for ( size_t I = 0; I < sSize; I += sBufferSize ) {
			sBufferSize = sSize - I;
			if ( sBufferSize > UINT32_MAX ) {	// DWORD-sized buffers.
				sBufferSize = UINT32_MAX;
			}
			if ( fFile.Read( &_vRet[I], sBufferSize ) != sBufferSize ) { return _vRet; }
		}
		return _vRet;
	}

	// Loads a file to memory.
	std::vector<uint8_t> & CFile::LoadToMemory( LPCWSTR _lpwFile, std::vector<uint8_t> &_vRet ) {
		_vRet.clear();
		UINT64 ui64Size = Size( _lpwFile );
		if ( !ui64Size ) { return _vRet; }
		size_t sSize = static_cast<size_t>(ui64Size);
		if ( ui64Size != sSize ) { return _vRet; }	// Ensure that size_t is large enough to load the file.
		_vRet.resize( sSize );

		CFile fFile;
		if ( !fFile.OpenFile( _lpwFile, TRUE ) ) { return _vRet; }
		size_t sBufferSize = 0;
		for ( size_t I = 0; I < sSize; I += sBufferSize ) {
			sBufferSize = sSize - I;
			if ( sBufferSize > UINT32_MAX ) {	// DWORD-sized buffers.
				sBufferSize = UINT32_MAX;
			}
			if ( fFile.Read( &_vRet[I], sBufferSize ) != sBufferSize ) { return _vRet; }
		}
		return _vRet;
	}

	// Converts a WIN32_FILE_ATTRIBUTE_DATA structure to an MX_FILE_ATTR structure.
	CFile::MX_FILE_ATTR CFile::Win32AttrToMxAttr( const WIN32_FILE_ATTRIBUTE_DATA &_wfadData ) {
		MX_FILE_ATTR faAttr = {
			_wfadData.dwFileAttributes,																						// dwAttr
			(static_cast<UINT64>(_wfadData.ftCreationTime.dwHighDateTime) << 32) | _wfadData.ftCreationTime.dwLowDateTime,	// ui64CreationTime
			(static_cast<UINT64>(_wfadData.ftLastAccessTime.dwHighDateTime) << 32) | _wfadData.ftLastAccessTime.dwLowDateTime,// ui64AccessTime
			(static_cast<UINT64>(_wfadData.ftLastWriteTime.dwHighDateTime) << 32) | _wfadData.ftLastWriteTime.dwLowDateTime,	// ui64WriteTime
			(static_cast<UINT64>(_wfadData.nFileSizeHigh) << 32) | _wfadData.nFileSizeLow,									// ui64Size
		};
		return faAttr;
	}

}	// namespace mx

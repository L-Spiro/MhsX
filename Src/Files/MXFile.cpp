#include "MXFile.h"
#include <string>


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
	DWORD CFile::Read( LPVOID _lpvBuffer, DWORD _dwNumberOfBytesToRead, DWORD * _pdwBytesRead ) const {
		if ( m_hFile == INVALID_HANDLE_VALUE ) { return 0; }
		DWORD dwRead = 0;
		if ( ::ReadFile( m_hFile, _lpvBuffer, _dwNumberOfBytesToRead, &dwRead, NULL ) ) {
			if ( _pdwBytesRead ) { (*_pdwBytesRead) = dwRead; }
			return dwRead;
		}
		if ( _pdwBytesRead ) { (*_pdwBytesRead) = 0; }
		return 0;
	}

	// Writes to the opened file.
	DWORD CFile::Write( LPCVOID _lpvBuffer, DWORD _dwNumberOfBytesToWrite, DWORD * _pdwBytesWritten ) {
		if ( m_hFile == INVALID_HANDLE_VALUE ) { return 0; }
		DWORD dwWritten = 0;
		if ( ::WriteFile( m_hFile, _lpvBuffer, _dwNumberOfBytesToWrite, &dwWritten, NULL ) ) {
			if ( _pdwBytesWritten ) { (*_pdwBytesWritten) = dwWritten; }
			return dwWritten;
		}
		if ( _pdwBytesWritten ) { (*_pdwBytesWritten) = 0; }
		return 0;
	}

	// Gets the size of the file.
	UINT64 CFile::Size() const {
		return Size( m_hFile );
	}

	// Moves the file pointer from the current position and returns the new position.
	UINT64 CFile::MovePointerBy( INT64 _i64Offset ) const {
		if ( m_hFile == INVALID_HANDLE_VALUE ) { return 0; }
		LARGE_INTEGER liMoveBy;
		liMoveBy.QuadPart = _i64Offset;
		LARGE_INTEGER liNewPos;
		if ( ::SetFilePointerEx( m_hFile, liMoveBy, &liNewPos, FILE_CURRENT ) ) { return liNewPos.QuadPart; }
		return 0;
	}

	// Moves the file pointer to the given file position.
	UINT64 CFile::MovePointerTo( UINT64 _ui64Pos, BOOL _bFromEnd ) const {
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
			return (static_cast<UINT64>(wfadData.nFileSizeHigh) << 32ULL) | wfadData.nFileSizeLow;
		}
		return 0;
	}

	// Gets the size of a file given its path.
	UINT64 CFile::Size( LPCWSTR _lpwFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExW( _lpwFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.nFileSizeHigh) << 32ULL) | wfadData.nFileSizeLow;
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
			return (static_cast<UINT64>(wfadData.ftCreationTime.dwHighDateTime) << 32ULL) | wfadData.ftCreationTime.dwLowDateTime;
		}
		return 0;
	}

	// Gets the creation time of a file.
	UINT64 CFile::CreationTime( LPCWSTR _lpwFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExW( _lpwFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.ftCreationTime.dwHighDateTime) << 32ULL) | wfadData.ftCreationTime.dwLowDateTime;
		}
		return 0;
	}

	// Gets the last access time of a file.
	UINT64 CFile::LastAccessTime( LPCSTR _lpcFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExA( _lpcFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.ftLastAccessTime.dwHighDateTime) << 32ULL) | wfadData.ftLastAccessTime.dwLowDateTime;
		}
		return 0;
	}

	// Gets the last access time of a file.
	UINT64 CFile::LastAccessTime( LPCWSTR _lpwFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExW( _lpwFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.ftLastAccessTime.dwHighDateTime) << 32ULL) | wfadData.ftLastAccessTime.dwLowDateTime;
		}
		return 0;
	}

	// Gets the last write time of a file.
	UINT64 CFile::LastWriteTime( LPCSTR _lpcFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExA( _lpcFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.ftLastWriteTime.dwHighDateTime) << 32ULL) | wfadData.ftLastWriteTime.dwLowDateTime;
		}
		return 0;
	}

	// Gets the last write time of a file.
	UINT64 CFile::LastWriteTime( LPCWSTR _lpwFile ) {
		WIN32_FILE_ATTRIBUTE_DATA wfadData;
		if ( ::GetFileAttributesExW( _lpwFile, GetFileExInfoStandard, &wfadData ) ) {
			return (static_cast<UINT64>(wfadData.ftLastWriteTime.dwHighDateTime) << 32ULL) | wfadData.ftLastWriteTime.dwLowDateTime;
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
		uint64_t ui64BufferSize = 0;
		for ( size_t I = 0; I < sSize; I += ui64BufferSize ) {
			ui64BufferSize = sSize - I;
			if ( ui64BufferSize > UINT32_MAX ) {	// DWORD-sized buffers.
				ui64BufferSize = UINT32_MAX;
			}
			if ( fFile.Read( &_vRet[I], static_cast<DWORD>(ui64BufferSize) ) != ui64BufferSize ) { return _vRet; }
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
		uint64_t ui64BufferSize = 0;
		for ( size_t I = 0; I < sSize; I += ui64BufferSize ) {
			ui64BufferSize = sSize - I;
			if ( ui64BufferSize > UINT32_MAX ) {	// DWORD-sized buffers.
				ui64BufferSize = UINT32_MAX;
			}
			if ( fFile.Read( &_vRet[I], ui64BufferSize ) != ui64BufferSize ) { return _vRet; }
		}
		return _vRet;
	}

	// Determines if the given path represents an existing file (not folder).
	bool CFile::IsFile( LPCSTR _lpcFile ) {
		DWORD dwAttr = ::GetFileAttributesA( _lpcFile );
		return (dwAttr != INVALID_FILE_ATTRIBUTES) &&
			!(dwAttr & FILE_ATTRIBUTE_DIRECTORY);
	}

	// Determines if the given path represents an existing file (not folder).
	bool CFile::IsFile( LPCWSTR _lpwFile ) {
		std::wstring wsTmp;
		wsTmp = L"\\\\?\\";
		wsTmp += _lpwFile;
		DWORD dwAttr = ::GetFileAttributesW( wsTmp.c_str() );
		return (dwAttr != INVALID_FILE_ATTRIBUTES) &&
			!(dwAttr & FILE_ATTRIBUTE_DIRECTORY);
	}

	// Determines if the given path represents an existing folder (not file).
	bool CFile::IsFolder( LPCSTR _lpcFolder ) {
		DWORD dwAttr = ::GetFileAttributesA( _lpcFolder );
		return (dwAttr != INVALID_FILE_ATTRIBUTES) &&
			(dwAttr & FILE_ATTRIBUTE_DIRECTORY);
	}

	// Determines if the given path represents an existing folder (not file).
	bool CFile::IsFolder( LPCWSTR _lpwFolder ) {
		std::wstring wsTmp;
		wsTmp = L"\\\\?\\";
		wsTmp += _lpwFolder;
		DWORD dwAttr = ::GetFileAttributesW( wsTmp.c_str() );
		return (dwAttr != INVALID_FILE_ATTRIBUTES) &&
			(dwAttr & FILE_ATTRIBUTE_DIRECTORY);
	}

	/**
	 * Finds files/folders in a given directory.
	 * 
	 * \param _pcFolderPath The path to the directory to search.
	 * \param _pcSearchString A wildcard search string to find only certain files/folders.
	 * \param _bIncludeFolders If true, folders are included in the return.
	 * \param _vResult The return array.  Found files and folders are appended to the array.
	 * \return Returns _vResult.
	 **/
	std::vector<std::wstring> & CFile::FindFiles( const wchar_t * _pcFolderPath, const wchar_t * _pcSearchString, bool _bIncludeFolders, std::vector<std::wstring> &_vResult ) {
		std::filesystem::path sPath = std::filesystem::path( _pcFolderPath ).make_preferred();

		std::filesystem::path sSearch;
		if ( _pcSearchString ) { sSearch = std::filesystem::path( _pcSearchString ).make_preferred(); }
		else { sSearch = L"*"; }


		std::filesystem::path sSearchPath = (sPath / sSearch).make_preferred();
		// Add the "\\?\" prefix to the path to support long paths
		std::wstring wsSearchPath = sSearchPath.native();
		if ( wsSearchPath.compare( 0, 4, L"\\\\?\\" ) != 0 ) {
			wsSearchPath = L"\\\\?\\" + wsSearchPath;
		}

		WIN32_FIND_DATAW wfdData;
		HANDLE hDir = ::FindFirstFileW( wsSearchPath.c_str(), &wfdData );
		if ( INVALID_HANDLE_VALUE == hDir ) { return _vResult; }
		
		do {
			if ( wfdData.cFileName[0] == L'.' ) { continue; }
			bool bIsFolder = ((wfdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
			if ( !_bIncludeFolders && bIsFolder ) { continue; }
			try {
				// Construct the full path to the file
				std::filesystem::path pFilePath = sPath / wfdData.cFileName;
				pFilePath = pFilePath.make_preferred();

				// Add the "\\?\" prefix if it's not already present
				std::wstring wsFilePath = pFilePath.native();
				if ( wsFilePath.compare( 0, 4, L"\\\\?\\" ) != 0 ) {
					wsFilePath = L"\\\\?\\" + wsFilePath;
				}

				_vResult.push_back( wsFilePath );
			}
			catch ( ... ) {
				::FindClose( hDir );
				return _vResult;
			}
		} while ( ::FindNextFileW( hDir, &wfdData ) );

		::FindClose( hDir );
		return _vResult;
	}

	/**
	 * Delets all files of a given type in a given folder.
	 * 
	 * \param _pcFolderPath The directory in which to delete files.
	 * \param _pcType The file type to delete.
	 * \param _bIncludeFolders Whether to include subfolders or not.
	 * \return Returns the number of files deleted.
	 **/
	size_t CFile::DeleteFiles( const wchar_t * _pcFolderPath, const wchar_t * _pcType, bool _bIncludeFolders ) {
		std::vector<std::wstring> vList;
		FindFiles( _pcFolderPath, (std::wstring( L"*." ) + _pcType).c_str(), _bIncludeFolders, vList );
		size_t sCnt = 0;
		for ( auto I = vList.size(); I--; ) {
			if ( IsFile( vList[I].c_str() ) ) {
				sCnt += ::DeleteFileW( vList[I].c_str() );
			}
		}
		return sCnt;
	}

	// Converts a WIN32_FILE_ATTRIBUTE_DATA structure to an MX_FILE_ATTR structure.
	CFile::MX_FILE_ATTR CFile::Win32AttrToMxAttr( const WIN32_FILE_ATTRIBUTE_DATA &_wfadData ) {
		MX_FILE_ATTR faAttr = {
			_wfadData.dwFileAttributes,																								// dwAttr
			(static_cast<UINT64>(_wfadData.ftCreationTime.dwHighDateTime) << 32ULL) | _wfadData.ftCreationTime.dwLowDateTime,		// ui64CreationTime
			(static_cast<UINT64>(_wfadData.ftLastAccessTime.dwHighDateTime) << 32ULL) | _wfadData.ftLastAccessTime.dwLowDateTime,	// ui64AccessTime
			(static_cast<UINT64>(_wfadData.ftLastWriteTime.dwHighDateTime) << 32ULL) | _wfadData.ftLastWriteTime.dwLowDateTime,		// ui64WriteTime
			(static_cast<UINT64>(_wfadData.nFileSizeHigh) << 32ULL) | _wfadData.nFileSizeLow,										// ui64Size
		};
		return faAttr;
	}

}	// namespace mx

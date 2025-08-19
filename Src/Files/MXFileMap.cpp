#include "MXFileMap.h"
#include "../System/MXSystem.h"

#include <algorithm>

// == Macros.
#define MX_MAP_BUF_SIZE			static_cast<UINT64>(mx::CSystem::GetSystemInfo().dwAllocationGranularity * 64)

namespace mx {

	CFileMap::CFileMap() {
	}
	CFileMap::~CFileMap() {
		Close();
	}

	// == Functions.
	// Creates a file mapping.  Always opens for read, may also open for write.
	BOOL CFileMap::CreateMap( LPCSTR _lpcFile, BOOL _bOpenForWrite ) {
		Close();
		m_hFile = ::CreateFileA( _lpcFile,
			_bOpenForWrite ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

		if ( m_hFile == INVALID_HANDLE_VALUE ) {
			return FALSE;
		}
		m_bWritable = _bOpenForWrite;
		return CreateFileMap();
	}

	// Creates a file mapping.  Always opens for read, may also open for write.
	BOOL CFileMap::CreateMap( LPCWSTR _lpwFile, BOOL _bOpenForWrite ) {
		Close();
		m_hFile = ::CreateFileW( _lpwFile,
			_bOpenForWrite ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

		if ( m_hFile == INVALID_HANDLE_VALUE ) {
			return FALSE;
		}
		m_bWritable = _bOpenForWrite;
		return CreateFileMap();
	}

	// Closes the opened file map.
	VOID CFileMap::Close() {
		if ( m_pbMapBuffer ) {
			::UnmapViewOfFile( m_pbMapBuffer );
			m_pbMapBuffer = nullptr;
		}
		if ( m_hMap != INVALID_HANDLE_VALUE ) {
			::CloseHandle( m_hMap );
			m_hMap = INVALID_HANDLE_VALUE;
		}
		if ( m_hFile != INVALID_HANDLE_VALUE ) {
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		}
		m_bIsEmpty = TRUE;
		m_bWritable = TRUE;
		m_ui64Size = 0;
		m_ui64MapStart = MAXUINT64;
		m_dwMapSize = 0;
	}

	// Gets the size of the file.
	UINT64 CFileMap::Size() const {
		if ( !m_ui64Size ) {
			m_ui64Size = CFile::Size( m_hFile );
		}
		return m_ui64Size;
	}

	// Reads from the opened file.
	DWORD CFileMap::Read( LPVOID _lpvBuffer, UINT64 _ui64From, DWORD _dwNumberOfBytesToRead ) const {
		_dwNumberOfBytesToRead = static_cast<DWORD>(std::min( static_cast<UINT64>(_dwNumberOfBytesToRead), Size() - _ui64From ));
		// Read in 8-megabyte chunks.
		PBYTE pbDst = static_cast<PBYTE>(_lpvBuffer);
		DWORD dwWritten = 0;
		while ( _dwNumberOfBytesToRead ) {
			DWORD dwReadAmount = std::min( m_dwChunkSize, _dwNumberOfBytesToRead );
			if ( !MapRegion( _ui64From, dwReadAmount ) ) {
				return dwWritten;
			}
			std::memcpy( pbDst, &m_pbMapBuffer[_ui64From-m_ui64MapStart], dwReadAmount );


			_dwNumberOfBytesToRead -= dwReadAmount;
			_ui64From += dwReadAmount;
			pbDst += dwReadAmount;
		}
		return dwWritten;
	}

	// Writes to the opened file.
	DWORD CFileMap::Write( LPCVOID _lpvBuffer, UINT64 _ui64From, DWORD _dwNumberOfBytesToWrite ) {
		_dwNumberOfBytesToWrite = static_cast<DWORD>(std::min( static_cast<UINT64>(_dwNumberOfBytesToWrite), Size() - _ui64From ));
		// Write in 8-megabyte chunks.
		const BYTE * pbSrc = static_cast<const BYTE *>(_lpvBuffer);
		DWORD dwWritten = 0;
		while ( _dwNumberOfBytesToWrite ) {
			DWORD dwWriteAmount = std::min( m_dwChunkSize, _dwNumberOfBytesToWrite );
			if ( !MapRegion( _ui64From, dwWriteAmount ) ) {
				return dwWritten;
			}
			std::memcpy( &m_pbMapBuffer[_ui64From-m_ui64MapStart], pbSrc, dwWriteAmount );


			_dwNumberOfBytesToWrite -= dwWriteAmount;
			_ui64From += dwWriteAmount;
			pbSrc += dwWriteAmount;
		}
		return dwWritten;
	}

	// Map a region of the file.
	BOOL CFileMap::MapRegion( UINT64 _ui64Offset, DWORD _dwSize ) const {
		if ( m_hMap == INVALID_HANDLE_VALUE ) { return FALSE; }
		UINT64 ui64Adjusted = AdjustBase( _ui64Offset );
		DWORD dwNewSize = static_cast<DWORD>((_ui64Offset - ui64Adjusted) + _dwSize);
		dwNewSize = static_cast<DWORD>(std::min( static_cast<UINT64>(dwNewSize), Size() - ui64Adjusted ));
		if ( m_pbMapBuffer && ui64Adjusted == m_ui64MapStart && dwNewSize == m_dwMapSize ) { return TRUE; }

		if ( m_pbMapBuffer ) {
			// Unmap existing buffer.
			::UnmapViewOfFile( m_pbMapBuffer );
			m_pbMapBuffer = nullptr;
			m_ui64MapStart = MAXUINT64;
			m_dwMapSize = 0;
		}

		m_pbMapBuffer = static_cast<PBYTE>(::MapViewOfFile( m_hMap,
			(m_bWritable ? FILE_MAP_WRITE : 0) | FILE_MAP_READ,
			static_cast<DWORD>(ui64Adjusted >> 32),
			static_cast<DWORD>(ui64Adjusted),
			dwNewSize ));
		if ( !m_pbMapBuffer ) { return FALSE; }
		m_ui64MapStart = ui64Adjusted;
		m_dwMapSize = dwNewSize;
		return TRUE;
	}

	// Creates the file map.
	BOOL CFileMap::CreateFileMap() {
		if ( m_hFile == INVALID_HANDLE_VALUE ) { return FALSE; }
		// Can't open 0-sized files.  Emulate the successful mapping of such a file.
		m_bIsEmpty = Size() == 0;
		if ( m_bIsEmpty ) { return TRUE; }
		m_hMap = ::CreateFileMappingW( m_hFile,
			NULL,
			m_bWritable ? PAGE_READWRITE : PAGE_READONLY,
			0,
			0,
			NULL );
		if ( m_hMap == NULL ) {
			// For some reason ::CreateFileMapping() returns NULL rather than INVALID_HANDLE_VALUE.
			m_hMap = INVALID_HANDLE_VALUE;
			Close();
			return FALSE;
		}
		m_ui64MapStart = MAXUINT64;
		m_dwMapSize = 0;
		return TRUE;
	}

	// Adjusts the input to the nearest mapping offset.
	UINT64 CFileMap::AdjustBase( UINT64 _ui64Offset ) const {
		if ( _ui64Offset < (MX_MAP_BUF_SIZE >> 1) ) { return 0; }
		if ( Size() <= MX_MAP_BUF_SIZE ) { return 0; }
		return ((_ui64Offset + (MX_MAP_BUF_SIZE >> 2)) & (~((MX_MAP_BUF_SIZE >> 1) - 1))) - (MX_MAP_BUF_SIZE >> 1);
	}

}	// namespace mx

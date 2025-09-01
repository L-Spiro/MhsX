#include "MXFileMap.h"
#include "../System/MXSystem.h"

#include <algorithm>

// == Macros.
#define MX_MAP_BUF_SIZE			static_cast<UINT64>(mx::CSystem::GetSystemInfo().dwAllocationGranularity * 64)

namespace mx {

	// ID generator.
	std::atomic<uint64_t> CFileMap::s_aId = 0;

	CFileMap::CFileMap() {
	}
	CFileMap::~CFileMap() {
		Close();
	}

	// == Functions.
	/**
	 * Creates (or opens) a file and its mapping.
	 * \brief If _ui64CreationSize != 0, the file is (re)created and resized to that many bytes.
	 *
	 * \param _pFile Path to the file.
	 * \param _bOpenForWrite TRUE to open for write when not creating; ignored when creating (write is forced).
	 * \param _ui64CreationSize If non-zero, (re)create the file and set its size to this value.
	 * \return Returns TRUE on success.
	 */
	BOOL CFileMap::CreateMap( const std::filesystem::path &_pFile, BOOL _bOpenForWrite, uint64_t _ui64CreationSize ) {
		Close();
		const bool bCreate = (_ui64CreationSize != 0ULL);
		const DWORD dwDesiredAccess = bCreate ? (GENERIC_READ | GENERIC_WRITE)
			: (_bOpenForWrite ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ);
		const DWORD dwCreationDisposition = bCreate ? CREATE_ALWAYS : OPEN_EXISTING;

		m_hFile = ::CreateFileW(
			_pFile.generic_wstring().c_str(),
			dwDesiredAccess,
			0,
			NULL,
			dwCreationDisposition,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if ( m_hFile == INVALID_HANDLE_VALUE ) { return FALSE; }

		// If creating, resize to _ui64CreationSize bytes.
		if ( bCreate ) {
			LARGE_INTEGER liSize;
			liSize.QuadPart = static_cast<LONGLONG>(_ui64CreationSize);

			if ( !::SetFilePointerEx( m_hFile, liSize, nullptr, FILE_BEGIN ) ) {
				Close();
				return FALSE;
			}
			if ( !::SetEndOfFile( m_hFile ) ) {
				Close();
				return FALSE;
			}
			liSize.QuadPart = 0;
			::SetFilePointerEx( m_hFile, liSize, nullptr, FILE_BEGIN );
		}


		m_bWritable = _bOpenForWrite;
		return CreateFileMap();
	}

	// Closes the opened file map.
	VOID CFileMap::Close() {
		UnMapRegion();
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
		DWORD dwRead = 0;
		while ( _dwNumberOfBytesToRead ) {
			DWORD dwReadAmount = std::min( m_dwChunkSize, _dwNumberOfBytesToRead );
			if ( !MapRegion( _ui64From, dwReadAmount ) ) {
				return dwRead;
			}
			std::memcpy( pbDst, &m_pbMapBuffer[_ui64From-m_ui64MapStart], dwReadAmount );


			_dwNumberOfBytesToRead -= dwReadAmount;
			_ui64From += dwReadAmount;
			pbDst += dwReadAmount;
			dwRead += dwReadAmount;
		}
		return dwRead;
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
			dwWritten += dwWriteAmount;
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

		const_cast<CFileMap *>(this)->UnMapRegion();

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

	// Closes the current region map.
	void CFileMap::UnMapRegion() {
		if ( m_pbMapBuffer ) {
			// Unmap existing buffer.
			::UnmapViewOfFile( m_pbMapBuffer );
			m_pbMapBuffer = nullptr;
			m_ui64MapStart = MAXUINT64;
			m_dwMapSize = 0;
		}
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

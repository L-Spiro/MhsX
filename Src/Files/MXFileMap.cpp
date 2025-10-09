/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A standard file-mapping wrapper.
 */
 
#include "MXFileMap.h"
#include "../System/MXSystem.h"

#include <algorithm>

// == Macros.
#if defined( _WIN32 )
	// == Windows ============================================
	// Buffer size is 64Å~ the allocation granularity.
	#define MX_MAP_BUF_SIZE				static_cast<uint64_t>(mx::CSystem::GetSystemInfo().dwAllocationGranularity * 64)
#else
	// == POSIX: macOS / iOS / Linux =========================
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <errno.h>

	// Provide INVALID_HANDLE_VALUE compatibility if not present.
	#ifndef INVALID_HANDLE_VALUE
		#define INVALID_HANDLE_VALUE reinterpret_cast<HANDLE>(-1)
	#endif

	namespace mx::detail {
		static inline uint64_t			MapBufSize() {
			long lPg = ::sysconf( _SC_PAGESIZE );
			if ( lPg <= 0 ) { lPg = 4096; }
			static const uint64_t s_ui64 = static_cast<uint64_t>(lPg) * 64ULL;
			return s_ui64;
		}
	}
	#define MX_MAP_BUF_SIZE				mx::detail::MapBufSize()
#endif

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
	 * \param _bOpenForWrite true to open for write when not creating; ignored when creating (write is forced).
	 * \param _ui64CreationSize If non-zero, (re)create the file and set its size to this value.
	 * \return Returns true on success.
	 */
	bool CFileMap::CreateMap( const std::filesystem::path &_pFile, bool _bOpenForWrite, uint64_t _ui64CreationSize ) {
		Close();
		const bool bCreate = (_ui64CreationSize != 0ULL);

#if defined( _WIN32 )
		const DWORD dwDesiredAccess = bCreate ? (GENERIC_READ | GENERIC_WRITE)
			: (_bOpenForWrite ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ);
		const DWORD dwCreationDisposition = bCreate ? CREATE_ALWAYS : OPEN_EXISTING;
		const DWORD dwShare = (_bOpenForWrite || bCreate) ? FILE_SHARE_READ : (FILE_SHARE_READ | FILE_SHARE_WRITE);

		m_hFile = ::CreateFileW(
			_pFile.c_str(),
			dwDesiredAccess,
			dwShare,
			NULL,
			dwCreationDisposition,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if ( m_hFile == INVALID_HANDLE_VALUE ) { return false; }

		// If creating, resize to _ui64CreationSize bytes.
		if ( bCreate ) {
			LARGE_INTEGER liSize;
			liSize.QuadPart = static_cast<LONGLONG>(_ui64CreationSize);

			if ( !::SetFilePointerEx( m_hFile, liSize, nullptr, FILE_BEGIN ) ) {
				Close();
				return false;
			}
			if ( !::SetEndOfFile( m_hFile ) ) {
				Close();
				return false;
			}
			liSize.QuadPart = 0;
			::SetFilePointerEx( m_hFile, liSize, nullptr, FILE_BEGIN );
			m_bWritable = true;
		}
		else {
			m_bWritable = _bOpenForWrite;
		}
		return CreateFileMap();
#else
		int flags = 0;
		int mode = 0644;

		if ( bCreate ) {
			flags = O_CREAT | O_TRUNC | O_RDWR;
		}
		else {
			flags = _bOpenForWrite ? O_RDWR : O_RDONLY;
		}

		int fd = ::open( _pFile.c_str(), flags, mode );
		if ( fd < 0 ) { return false; }

		m_hFile = fd;

		if ( bCreate ) {
			// Set size to _ui64CreationSize.
			if ( ::ftruncate( fd, static_cast<off_t>(_ui64CreationSize) ) != 0 ) {
				Close();
				return false;
			}
			m_bWritable = true;
		}
		else {
			m_bWritable = _bOpenForWrite;
		}

		return CreateFileMap();
#endif
	}

	/**
	 * Closes the file and mapping.
	 * \brief Releases any open handles and unmaps any active view.
	 */
	void CFileMap::Close() {
		UnMapRegion();

#if defined( _WIN32 )
		if ( m_hMap != INVALID_HANDLE_VALUE ) {
			::CloseHandle( m_hMap );
			m_hMap = INVALID_HANDLE_VALUE;
		}
		if ( m_hFile != INVALID_HANDLE_VALUE ) {
			::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
		}
#else
		// On POSIX we alias m_hMap to the same fd as m_hFile; just close the fd once.
		if ( m_hFile != INVALID_HANDLE_VALUE ) {
			int fd = m_hFile;
			::close( fd );
			m_hFile = INVALID_HANDLE_VALUE;
			m_hMap = INVALID_HANDLE_VALUE;
		}
#endif
		m_bIsEmpty = true;
		m_bWritable = false;
		m_ui64Size = 0;
	}

	/**
	 * Gets the file size in bytes.
	 * \brief Returns the cached size if available; may query the OS when needed.
	 * 
	 * \return Returns the size of the underlying file.
	 */
	uint64_t CFileMap::Size() const {
		if MX_UNLIKELY( !m_ui64Size ) {
#if defined( _WIN32 )
			m_ui64Size = CFile::Size( m_hFile );
#else
			if ( m_hFile != INVALID_HANDLE_VALUE ) {
				int fd = m_hFile;
				struct stat st;
				if ( ::fstat( fd, &st ) == 0 ) {
					m_ui64Size = static_cast<uint64_t>(st.st_size);
				}
			}
#endif
		}
		return m_ui64Size;
	}

	/**
	 * Reads from the opened file.
	 * \brief Copies data starting at a given file offset into a caller-provided buffer, using a temporary or provided region map.
	 * 
	 * \param _lpvBuffer Destination buffer to receive the bytes.
	 * \param _ui64From File-relative byte offset to begin reading.
	 * \param _sNumberOfBytesToRead Number of bytes to read into the destination buffer.
	 * \param _prmMap Optional pointer to a CRegionMap to reuse; if nullptr, a transient map is used internally.
	 * \return Returns the number of bytes actually read; 0 on failure.
	 */
	size_t CFileMap::Read( LPVOID _lpvBuffer, uint64_t _ui64From, size_t _sNumberOfBytesToRead, CRegionMap * _prmMap ) const {
		if MX_UNLIKELY( _ui64From >= Size() ) { return 0; }
		CRegionMap * pmrUseMe = _prmMap ? _prmMap : &m_rmMap;
		_sNumberOfBytesToRead = static_cast<size_t>(std::min( static_cast<uint64_t>(_sNumberOfBytesToRead), Size() - _ui64From ));
		// Read in 8-megabyte chunks.
		uint8_t * pbDst = static_cast<uint8_t *>(_lpvBuffer);
		size_t sRead = 0;
		while ( _sNumberOfBytesToRead ) {
			uint32_t dwReadAmount = std::min<size_t>( m_ui32ChunkSize, _sNumberOfBytesToRead );
			if MX_UNLIKELY( !MapRegion( _ui64From, dwReadAmount, pmrUseMe ) ) {
				return sRead;
			}
			std::memcpy( pbDst, &pmrUseMe->Data()[_ui64From-pmrUseMe->Start()], dwReadAmount );


			_sNumberOfBytesToRead -= dwReadAmount;
			_ui64From += dwReadAmount;
			pbDst += dwReadAmount;
			sRead += dwReadAmount;
		}
		return sRead;
	}

	/**
	 * Writes to the opened file.
	 * \brief Copies data from a caller-provided buffer into the file at a given offset, using a temporary or provided region map.
	 * 
	 * \param _lpvBuffer Source buffer containing the bytes to write.
	 * \param _ui64From File-relative byte offset to begin writing.
	 * \param _dwNumberOfBytesToWrite Number of bytes to write from the source buffer.
	 * \param _prmMap Optional pointer to a CRegionMap to reuse; if nullptr, a transient map is used internally.
	 * \return Returns the number of bytes actually written; 0 on failure.
	 */
	uint32_t CFileMap::Write( LPCVOID _lpvBuffer, uint64_t _ui64From, uint32_t _dwNumberOfBytesToWrite, CRegionMap * _prmMap ) {
		if MX_UNLIKELY( !m_bWritable || _ui64From >= Size() ) { return 0; }
		CRegionMap * pmrUseMe = _prmMap ? _prmMap : &m_rmMap;
		_dwNumberOfBytesToWrite = static_cast<uint32_t>(std::min( static_cast<uint64_t>(_dwNumberOfBytesToWrite), Size() - _ui64From ));
		// Write in 8-megabyte chunks.
		const BYTE * pbSrc = static_cast<const BYTE *>(_lpvBuffer);
		uint32_t dwWritten = 0;
		while ( _dwNumberOfBytesToWrite ) {
			uint32_t dwWriteAmount = std::min( m_ui32ChunkSize, _dwNumberOfBytesToWrite );
			if MX_UNLIKELY( !MapRegion( _ui64From, dwWriteAmount, pmrUseMe ) ) {
				return dwWritten;
			}
			std::memcpy( &pmrUseMe->Data()[_ui64From-pmrUseMe->Start()], pbSrc, dwWriteAmount );


			_dwNumberOfBytesToWrite -= dwWriteAmount;
			_ui64From += dwWriteAmount;
			pbSrc += dwWriteAmount;
			dwWritten += dwWriteAmount;
		}
		return dwWritten;
	}

	/**
	 * Maps a region of the file into memory.
	 * \brief Ensures a view covering the requested range is available, using _prmMap if provided.
	 * 
	 * \param _ui64Offset File-relative byte offset where the view should begin.
	 * \param _sSize Minimum number of bytes that must be accessible from the view.
	 * \param _prmMap Optional pointer to a CRegionMap to receive/reuse the mapping; if nullptr, uses an internal map.
	 * \return Returns true on success; false on failure.
	 */
	bool CFileMap::MapRegion( uint64_t _ui64Offset, size_t _sSize, CRegionMap * _prmMap ) const {
#if defined( _WIN32 )
		if ( m_hMap == INVALID_HANDLE_VALUE ) { return false; }
#else
		if ( m_hFile == INVALID_HANDLE_VALUE ) { return false; }
		// On POSIX we alias m_hMap to the same fd as m_hFile.
		if ( m_hMap == INVALID_HANDLE_VALUE ) { m_hMap = m_hFile; }
#endif

		CRegionMap * pmrUseMe = _prmMap ? _prmMap : &m_rmMap;

		uint64_t ui64Adjusted = AdjustBase( _ui64Offset );
		size_t sNewSize = static_cast<uint32_t>((_ui64Offset - ui64Adjusted) + _sSize);
		sNewSize = std::min<size_t>( static_cast<uint64_t>(sNewSize), Size() - ui64Adjusted );
		if ( pmrUseMe->Data() &&
			ui64Adjusted >= pmrUseMe->Start() && (sNewSize + ui64Adjusted) == (pmrUseMe->Size() + pmrUseMe->Start()) ) { return true; }

		return pmrUseMe->MapRegion( m_hMap, ui64Adjusted, sNewSize, m_bWritable );
	}

	/**
	 * Unmaps any currently mapped region.
	 * \brief Convenience wrapper that clears the active region map associated with this file map.
	 */
	void CFileMap::UnMapRegion() {
		m_rmMap.UnMapRegion();
	}

	/**
	 * Is the opened file read-only?
	 * 
	 * \return Returns true if the file is read-only.
	 **/
	bool CFileMap::ReadOnly() const {
		return !m_bWritable;
#if 0
		if ( m_hFile == nullptr || m_hFile == INVALID_HANDLE_VALUE ) { return true; }

		// Query granted access on this handle.
		IO_STATUS_BLOCK sIos = {};
		FILE_ACCESS_INFORMATION sFai = {};
		NTSTATUS nsStatus = CSystem::NtQueryInformationFile(
			m_hFile,
			&sIos,
			&sFai,
			sizeof( sFai ),
			FileAccessInformation
		);

		// STATUS_SUCCESS == 0.
		if ( nsStatus == 0 ) {
			static constexpr ACCESS_MASK amWriteLike =
				FILE_WRITE_DATA | FILE_APPEND_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | DELETE;
			return ((sFai.AccessFlags & amWriteLike) == 0);
		}

		// Fallback #1: try creating a RW mapping on the existing handle (no file modification).
		if ( HANDLE hMap = ::CreateFileMappingW( m_hFile, nullptr, PAGE_READWRITE, 0, 0, nullptr ) ) {
			::CloseHandle( hMap );
			return false;	// RW mapping succeeded => not read-only.
		}
		if ( ::GetLastError() == ERROR_ACCESS_DENIED ) { return true; }

		// Fallback #2: probe with a separate write-open by path (covers ACL/environment if the above failed).
		if ( !m_pFilePath.empty() ) {
			const HANDLE hProbe = ::CreateFileW(
				m_pFilePath.c_str(),
				FILE_WRITE_DATA,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				nullptr,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				nullptr
			);
			if ( hProbe != INVALID_HANDLE_VALUE ) {
				::CloseHandle( hProbe );
				return false;
			}
			if ( ::GetLastError() == ERROR_ACCESS_DENIED ) { return true; }
		}

		// Could not positively prove read-only; assume writable to avoid disabling features unnecessarily.
		return false;
#endif	// #if 0
	}

	/**
	 * Creates the file-mapping object for the currently opened file.
	 * \brief Initializes m_hMap from m_hFile using CreateFileMapping(); does not map a view.
	 * 
	 * \return Returns true on success; false on failure (m_hMap remains INVALID_HANDLE_VALUE).
	 */
	bool CFileMap::CreateFileMap() {
		m_rmMap.UnMapRegion();
		if ( m_hFile == INVALID_HANDLE_VALUE ) { return false; }

#if defined( _WIN32 )
		// Can't open 0-sized files.  Emulate the successful mapping of such a file.
		m_bIsEmpty = Size() == 0;
		if ( m_bIsEmpty ) { return true; }
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
			return false;
		}
#else
		// Can't open 0-sized files.  Emulate the successful mapping of such a file.
		m_bIsEmpty = Size() == 0;
		if ( m_bIsEmpty ) {
			m_hMap = INVALID_HANDLE_VALUE;	// No mapping handle needed.
			return true;
		}

		// On POSIX there is no separate mapping handle; pass the fd via m_hMap.
		m_hMap = m_hFile;
#endif
		return true;
	}

	/**
	 * Adjusts a requested file offset down to a valid mapping base.
	 * \brief Aligns _ui64Offset to the system allocation granularity for MapViewOfFile().
	 * 
	 * \param _ui64Offset Requested file-relative byte offset for mapping.
	 * \return Returns the aligned base offset suitable for MapViewOfFile().
	 */
	uint64_t CFileMap::AdjustBase( uint64_t _ui64Offset ) const {
		const uint64_t ui64Size = MX_MAP_BUF_SIZE;
		if ( _ui64Offset < (ui64Size >> 1) ) { return 0; }
		if ( Size() <= ui64Size ) { return 0; }
		return ((_ui64Offset + (ui64Size >> 2)) & (~((ui64Size >> 1) - 1))) - (ui64Size >> 1);
	}

}	// namespace mx

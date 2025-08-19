#pragma once

#include <LSWWin.h>
#include "MXFile.h"

namespace mx {

	class CFileMap {
	public :
		CFileMap();
		~CFileMap();


		// == Functions.
		// Creates a file mapping.  Always opens for read, may also open for write.
		BOOL					CreateMap( LPCSTR _lpcFile, BOOL _bOpenForWrite );

		// Creates a file mapping.  Always opens for read, may also open for write.
		BOOL					CreateMap( LPCWSTR _lpwFile, BOOL _bOpenForWrite );

		// Closes the opened file map.
		VOID					Close();

		// Gets the size of the file.
		UINT64					Size() const;

		// Reads from the opened file.
		DWORD					Read( LPVOID _lpvBuffer, UINT64 _ui64From, DWORD _dwNumberOfBytesToRead ) const;

		// Writes to the opened file.
		DWORD					Write( LPCVOID _lpvBuffer, UINT64 _ui64From, DWORD _dwNumberOfBytesToWrite );

		// Sets the chunk size.
		inline VOID				SetChunkSize( DWORD _dwSize ) { m_dwChunkSize = _dwSize; }

		// Map a region of the file.
		BOOL					MapRegion( UINT64 _ui64Offset, DWORD _dwSize ) const;


	protected :
		// == Members.
		// The actual file handle.
		HANDLE					m_hFile = INVALID_HANDLE_VALUE;

		// File-mapping handle.
		HANDLE					m_hMap = INVALID_HANDLE_VALUE;

		// Mapped bytes.
		mutable PBYTE			m_pbMapBuffer = nullptr;

		// Mapping chunk size.
		DWORD					m_dwChunkSize = static_cast<DWORD>(8 * 1024 * 1024);

		// Is the file 0-sized?
		BOOL					m_bIsEmpty = TRUE;

		// Read-only or read-write?
		BOOL					m_bWritable = TRUE;

		// Size of the file.
		mutable UINT64			m_ui64Size = 0;

		// Map start.
		mutable UINT64			m_ui64MapStart = MAXUINT64;

		// Mapped size.
		mutable DWORD			m_dwMapSize = 0;


		// == Functions.
		// Creates the file map.
		BOOL					CreateFileMap();

		// Adjusts the input to the nearest mapping offset.
		UINT64					AdjustBase( UINT64 _ui64Offset ) const;
	};

}	// namespace mx

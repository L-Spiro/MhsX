#include "MXLargeCoWFileWindow.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "MXFile.h"

namespace mx {

	// == Functions.
	// Open a file and create a map.
	bool CLargeCoWFileWindow::MX_FILE_MAP::OpenFile( const std::filesystem::path &_pFile ) {
		Close();
		hFile = ::CreateFileW( _pFile.generic_wstring().c_str(),
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

		if ( hFile == INVALID_HANDLE_VALUE ) { return FALSE; }
		pPath = _pFile;
		return CreateFileMap();
	}

	// Closes the current file.
	void CLargeCoWFileWindow::MX_FILE_MAP::Close() {
		Upmap();
		if ( hMap != INVALID_HANDLE_VALUE ) {
			::CloseHandle( hMap );
			hMap = INVALID_HANDLE_VALUE;
		}
		if ( hFile != INVALID_HANDLE_VALUE ) {
			::CloseHandle( hFile );
			hFile = INVALID_HANDLE_VALUE;
		}
		bIsEmpty = true;
		ui64Size = 0;
	}

	// Gets the size of the file.
	uint64_t CLargeCoWFileWindow::MX_FILE_MAP::Size() const {
		if ( !ui64Size ) {
			ui64Size = CFile::Size( hFile );
		}
		return ui64Size;
	}

	// Creates the file map.
	bool CLargeCoWFileWindow::MX_FILE_MAP::CreateFileMap() {
		if ( hFile == INVALID_HANDLE_VALUE ) { return false; }
		// Can't open 0-sized files.  Emulate the successful mapping of such a file.
		bIsEmpty = Size() == 0;
		if ( bIsEmpty ) { return true; }
		hMap = ::CreateFileMappingW( hFile,
			NULL,
			PAGE_READONLY,
			0,
			0,
			NULL );
		if ( hMap == NULL ) {
			// For some reason ::CreateFileMapping() returns NULL rather than INVALID_HANDLE_VALUE.
			hMap = INVALID_HANDLE_VALUE;
			Close();
			return false;
		}
		ui64MapStart = UINT64_MAX;
		ui32MapSize = 0;
		return true;
	}

	// Map a region of the file.
	bool CLargeCoWFileWindow::MX_FILE_MAP::MapRegion( uint64_t _ui64Offset, DWORD _dwSize ) {
		if ( bIsEmpty && hFile != INVALID_HANDLE_VALUE ) { return true; }	// File is opened but size is 0.
		if ( hMap == INVALID_HANDLE_VALUE ) { return FALSE; }				// File not mapped.

		_dwSize = static_cast<DWORD>(std::min<uint64_t>( _dwSize, Size() - _ui64Offset ));
		if ( pui8MapBuffer && _ui64Offset == ui64MapStart && _dwSize == ui32MapSize ) { return TRUE; }

		Upmap();

		pui8MapBuffer = static_cast<PBYTE>(::MapViewOfFile( hMap,
			FILE_MAP_READ,
			static_cast<DWORD>(_ui64Offset >> 32),
			static_cast<DWORD>(_ui64Offset),
			_dwSize ));
		if ( !pui8MapBuffer ) { return FALSE; }
		ui64MapStart = _ui64Offset;
		ui32MapSize = _dwSize;
		return TRUE;
	}

	// Unmap.
	void CLargeCoWFileWindow::MX_FILE_MAP::Upmap() {
		if ( pui8MapBuffer ) {
			// Unmap existing buffer.
			::UnmapViewOfFile( pui8MapBuffer );
			pui8MapBuffer = nullptr;
			ui64MapStart = UINT64_MAX;
			ui32MapSize = 0;
		}
	}

	CLargeCoWFileWindow::CLargeCoWFileWindow() {
	}
	CLargeCoWFileWindow::~CLargeCoWFileWindow() {
		CFile::EraseFilesInDirectory( m_pEditDiractory );
		Reset();
	}

	// == Functions.
	// Opens a file for editing.  Maps the first sector to memory.
	bool CLargeCoWFileWindow::OpenFile( const std::filesystem::path &_pFile ) {
		Reset();

		if ( !m_fmMainMap.OpenFile( _pFile ) ) {
			Reset();
			return false;
		}
		try {
			m_pEditDiractory = std::filesystem::temp_directory_path() / _DEC_S_9DAF7683_Dues_Hex_Machina.c_str();
			m_pEditDiractory /= _pFile.filename();
			CFile::EraseFilesInDirectory( m_pEditDiractory );
		}
		catch ( ... ) {
			Reset();
			return false;
		}
		return true;
	}

	// Resets the object back to scratch.
	void CLargeCoWFileWindow::Reset() {
		m_fmMainMap.Close();
		m_vEditedMaps = std::vector<MX_FILE_MAP>();
		m_pEditDiractory = std::filesystem::path();
	}

}	// namespace mx

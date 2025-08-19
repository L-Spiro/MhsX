#include "MXLargeCoWFileWindow.h"
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

		if ( hFile == INVALID_HANDLE_VALUE ) {
			return FALSE;
		}
		return CreateFileMap();
	}

	// Closes the current file.
	void CLargeCoWFileWindow::MX_FILE_MAP::Close() {
		if ( pbMapBuffer ) {
			::UnmapViewOfFile( pbMapBuffer );
			pbMapBuffer = nullptr;
		}
		if ( hMap != INVALID_HANDLE_VALUE ) {
			::CloseHandle( hMap );
			hMap = INVALID_HANDLE_VALUE;
		}
		if ( hFile != INVALID_HANDLE_VALUE ) {
			::CloseHandle( hFile );
			hFile = INVALID_HANDLE_VALUE;
		}
		bIsEmpty = TRUE;
		ui64Size = 0;
		ui64MapStart = UINT64_MAX;
		ui32MapSize = 0;
	}

	// Gets the size of the file.
	uint64_t CLargeCoWFileWindow::MX_FILE_MAP::Size() const {
		if ( !ui64Size ) {
			ui64Size = CFile::Size( hFile );
		}
		return ui64Size;
	}

	// Creates the file map.
	BOOL CLargeCoWFileWindow::MX_FILE_MAP::CreateFileMap() {
		if ( hFile == INVALID_HANDLE_VALUE ) { return FALSE; }
		// Can't open 0-sized files.  Emulate the successful mapping of such a file.
		bIsEmpty = Size() == 0;
		if ( bIsEmpty ) { return TRUE; }
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
			return FALSE;
		}
		ui64MapStart = UINT64_MAX;
		ui32MapSize = 0;
		return TRUE;
	}

	CLargeCoWFileWindow::CLargeCoWFileWindow() {
	}
	CLargeCoWFileWindow::~CLargeCoWFileWindow() {
		Reset();
	}

	// == Functions.
	// Opens a file for editing.  Maps the first sector to memory.
	bool CLargeCoWFileWindow::OpenFile( const std::filesystem::path &_pFile ) {
		Reset();
		
		return true;
	}

	// Resets the object back to scratch.
	void CLargeCoWFileWindow::Reset() {
		//m_fmFileMap.Close();
	}

}	// namespace mx

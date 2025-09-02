#include "MXLargeCoWFileWindow.h"
#include "../CRC/MXCrc.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"
#include "MXFile.h"

namespace mx {

	// == Functions.
	// Open a file and create a map.
	//bool CLargeCoWFileWindow::MX_FILE_MAP::OpenFile( const std::filesystem::path &_pFile ) {
	//	Close();
	//	hFile = ::CreateFileW( _pFile.generic_wstring().c_str(),
	//		GENERIC_READ,
	//		0,
	//		NULL,
	//		OPEN_EXISTING,
	//		FILE_ATTRIBUTE_NORMAL,
	//		NULL );

	//	if ( hFile == INVALID_HANDLE_VALUE ) { return FALSE; }
	//	pPath = _pFile;
	//	return CreateFileMap();
	//}

	//// Closes the current file.
	//void CLargeCoWFileWindow::MX_FILE_MAP::Close() {
	//	Upmap();
	//	if ( hMap != INVALID_HANDLE_VALUE ) {
	//		::CloseHandle( hMap );
	//		hMap = INVALID_HANDLE_VALUE;
	//	}
	//	if ( hFile != INVALID_HANDLE_VALUE ) {
	//		::CloseHandle( hFile );
	//		hFile = INVALID_HANDLE_VALUE;
	//	}
	//	bIsEmpty = true;
	//	ui64Size = 0;
	//}

	//// Gets the size of the file.
	//uint64_t CLargeCoWFileWindow::MX_FILE_MAP::Size() const {
	//	if ( !ui64Size ) {
	//		ui64Size = CFile::Size( hFile );
	//	}
	//	return ui64Size;
	//}

	//// Creates the file map.
	//bool CLargeCoWFileWindow::MX_FILE_MAP::CreateFileMap() {
	//	if ( hFile == INVALID_HANDLE_VALUE ) { return false; }
	//	// Can't open 0-sized files.  Emulate the successful mapping of such a file.
	//	bIsEmpty = Size() == 0;
	//	if ( bIsEmpty ) { return true; }
	//	hMap = ::CreateFileMappingW( hFile,
	//		NULL,
	//		PAGE_READONLY,
	//		0,
	//		0,
	//		NULL );
	//	if ( hMap == NULL ) {
	//		// For some reason ::CreateFileMapping() returns NULL rather than INVALID_HANDLE_VALUE.
	//		hMap = INVALID_HANDLE_VALUE;
	//		Close();
	//		return false;
	//	}
	//	ui64MapStart = UINT64_MAX;
	//	ui32MapSize = 0;
	//	return true;
	//}

	//// Map a region of the file.
	//bool CLargeCoWFileWindow::MX_FILE_MAP::MapRegion( uint64_t _ui64Offset, DWORD _dwSize ) {
	//	if ( bIsEmpty && hFile != INVALID_HANDLE_VALUE ) { return true; }	// File is opened but size is 0.
	//	if ( hMap == INVALID_HANDLE_VALUE ) { return FALSE; }				// File not mapped.

	//	_dwSize = static_cast<DWORD>(std::min<uint64_t>( _dwSize, Size() - _ui64Offset ));
	//	if ( pui8MapBuffer && _ui64Offset == ui64MapStart && _dwSize == ui32MapSize ) { return TRUE; }

	//	Upmap();

	//	pui8MapBuffer = static_cast<PBYTE>(::MapViewOfFile( hMap,
	//		FILE_MAP_READ,
	//		static_cast<DWORD>(_ui64Offset >> 32),
	//		static_cast<DWORD>(_ui64Offset),
	//		_dwSize ));
	//	if ( !pui8MapBuffer ) { return FALSE; }
	//	ui64MapStart = _ui64Offset;
	//	ui32MapSize = _dwSize;
	//	return TRUE;
	//}

	//// Unmap.
	//void CLargeCoWFileWindow::MX_FILE_MAP::Upmap() {
	//	if ( pui8MapBuffer ) {
	//		// Unmap existing buffer.
	//		::UnmapViewOfFile( pui8MapBuffer );
	//		pui8MapBuffer = nullptr;
	//		ui64MapStart = UINT64_MAX;
	//		ui32MapSize = 0;
	//	}
	//}

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

		if ( !m_fmMainMap.CreateMap( _pFile, FALSE ) ) {
			Reset();
			return false;
		}
		try {
			uint64_t ui64Size = m_fmMainMap.Size();
			CCrc::Init();
			uint32_t ui32Crc = UINT32_MAX;
			
			std::vector<uint8_t> vBuffer;
			for ( uint64_t I = 0; I < ui64Size; ) {
				uint64_t ui64Chunk = std::min<uint64_t>( ui64Size - I, 8ULL * 1024ULL * 1024ULL );	// Walk in chunks of 8 megabytes.
				vBuffer.resize( size_t( ui64Chunk ) );
				DWORD dwRead = m_fmMainMap.Read( vBuffer.data(), I, DWORD( ui64Chunk ) );
				if MX_UNLIKELY( !dwRead ) { break; }
				for ( uint64_t J = 0; J < dwRead; ++J ) {
					CCrc::GetCrc( CCrc::StdTable(), vBuffer[J], ui32Crc );
				}
				I += dwRead;
			}
			ui32Crc = ~ui32Crc;

			m_pEditDiractory = std::filesystem::temp_directory_path() / _DEC_S_9DAF7683_Dues_Hex_Machina.c_str();
			m_pEditDiractory /= _pFile.filename();
			m_pEditDiractory += std::to_string( ui32Crc );
			CFile::EraseFilesInDirectory( m_pEditDiractory );

			// Make a single section that spans the whole file.
			MX_LOGICAL_SECTION lsSection = { .stType = MX_ST_ORIGINAL_FILE,
				.ui64Start = 0, .ui64Size = ui64Size, .ui64Offset = 0 };
			m_vLogicalMap.push_back( lsSection );
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
		m_vEditedMaps = std::vector<std::unique_ptr<CFileMap>>();
		m_pEditDiractory = std::filesystem::path();
		m_vLogicalMap = std::vector<MX_LOGICAL_SECTION>();
	}

	// Updates the list of active segments.  Call within a try/catch block.
	void CLargeCoWFileWindow::UpdateActiveSegments( uint64_t _ui64Id, size_t _sMax ) {
		// Erase any null pointers.
		m_vEditedMaps.erase(
			std::remove_if(
				m_vEditedMaps.begin(),
				m_vEditedMaps.end(),
				[]( const std::unique_ptr<CFileMap> &_upMap ) { return !_upMap; }
			),
			m_vEditedMaps.end()
		);
		// Remove the insertion ID and invalid ID's from the list.
		for ( auto I = m_vActiveSegments.size(); I--; ) {
			const uint64_t ui64IdAtI = m_vActiveSegments[I];

			if ( m_vActiveSegments[I] == _ui64Id ) {
				m_vActiveSegments.erase( m_vActiveSegments.begin() + I );
				continue;
			}

			auto aFound = std::find_if(
				m_vEditedMaps.begin(),
				m_vEditedMaps.end(),
				[ui64IdAtI]( const std::unique_ptr<CFileMap> &_upMap ) { return _upMap->Id() == ui64IdAtI; }
			);
			if ( aFound == m_vEditedMaps.end() ) {
				m_vActiveSegments.erase( m_vActiveSegments.begin() + I );
				continue;
			}
		}
		// Insert at the top.
		m_vActiveSegments.insert( m_vActiveSegments.begin(), _ui64Id );

		// Resize.
		if ( _sMax > 0 && m_vActiveSegments.size() > _sMax ) {
			m_vActiveSegments.resize( _sMax );
		}

		// Close an mappings not in the hot cache.
		for ( auto I = m_vEditedMaps.size(); I--; ) {
			const uint64_t ui64Id = m_vEditedMaps[I]->Id();
			auto aFound = std::find_if(
				m_vActiveSegments.begin(),
				m_vActiveSegments.end(),
				[ui64Id]( const size_t _sId ) { return _sId == ui64Id; }
			);
			if ( aFound == m_vActiveSegments.end() ) {
				// This ID was not in the m_vActiveSegments list.  Remove its mapping.
				m_vEditedMaps[I]->UnMapRegion();
			}
		}
	}

	// Simplifies the logical view.
	void CLargeCoWFileWindow::SimplifyLogicalView() {
		m_vLogicalMap.erase(
			std::remove_if(
				m_vLogicalMap.begin(),
				m_vLogicalMap.end(),
				[]( const auto &_lsThis ) { return _lsThis.ui64Size == 0ULL; }
			),
			m_vLogicalMap.end()
		);


		for ( size_t I = 1; I < m_vLogicalMap.size(); ) {
			// If they both point to the same medium.
			auto & lsPrev = m_vLogicalMap[I-1];
			auto & lsCur = m_vLogicalMap[I];

			bool bSameBacking = false;
			if ( lsPrev.stType == lsCur.stType ) {
				if ( lsPrev.stType == MX_ST_ORIGINAL_FILE ) {
					bSameBacking = true;	// Only one original file.
				}
				else if ( lsPrev.stType == MX_ST_SEGMENT ) {
					bSameBacking = (lsPrev.u.saSegmentAddress.sFile == lsCur.u.saSegmentAddress.sFile);
				}
			}


			if ( bSameBacking ) {
				// If they point to contiguous data inside the medium.
				if ( lsPrev.ui64Offset + lsPrev.ui64Size == lsCur.ui64Offset ) {
					// The current section can be merged into the previous section.
					uint64_t ui64Size = lsCur.ui64Size;
					m_vLogicalMap.erase( m_vLogicalMap.begin() + I );
					// lsPrev and lsCur are invalidated.
					m_vLogicalMap[I-1].ui64Size += ui64Size;
					continue;
				}
			}
			++I;
		}
	}

}	// namespace mx

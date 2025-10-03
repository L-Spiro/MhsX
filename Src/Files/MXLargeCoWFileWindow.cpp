#include "MXLargeCoWFileWindow.h"
#include "../CRC/MXCrc.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"
#include "MXFile.h"

#include <algorithm>
#include <cstdint>

namespace mx {

	CLargeCoWFileWindow::CLargeCoWFileWindow() {
	}
	CLargeCoWFileWindow::~CLargeCoWFileWindow() {
		CFile::EraseFilesInDirectory( m_pEditDiractory );
		CFile::DeleteDirectorySafely( m_pEditDiractory );
		Reset();
		m_pEditDiractory = std::filesystem::path();
	}

	// == Functions.
	// Opens a file for editing.  Maps the first sector to memory.
	bool CLargeCoWFileWindow::OpenFile( const std::filesystem::path &_pFile ) {
		Reset();

		if ( !m_fmMainMap.CreateMap( _pFile, TRUE ) ) {
			if ( !m_fmMainMap.CreateMap( _pFile, FALSE ) ) {
				Reset();
				return false;
			}
		}
		try {
			uint64_t ui64Size = m_fmMainMap.Size();
			CCrc::Init();
			m_pFilePath = _pFile;
			//m_ui32OriginalCrc = UINT32_MAX;
			//
			//std::vector<uint8_t> vBuffer;
			//for ( uint64_t I = 0; I < ui64Size; ) {
			//	uint64_t ui64Chunk = std::min<uint64_t>( ui64Size - I, 8ULL * 1024ULL * 1024ULL );	// Walk in chunks of 8 megabytes.
			//	vBuffer.resize( size_t( ui64Chunk ) );
			//	DWORD dwRead = m_fmMainMap.Read( vBuffer.data(), I, DWORD( ui64Chunk ) );
			//	if MX_UNLIKELY( !dwRead ) { break; }
			//	for ( uint64_t J = 0; J < dwRead; ++J ) {
			//		CCrc::GetCrc( CCrc::StdTable(), vBuffer[J], m_ui32OriginalCrc );
			//	}
			//	I += dwRead;
			//}
			//m_ui32OriginalCrc = ~m_ui32OriginalCrc;
			if ( !ReadOnly() ) {
				CreateWriteDirectories();
			}

			// Make a single section that spans the whole file.
			MX_LOGICAL_SECTION lsSection = { .stType = MX_ST_ORIGINAL_FILE,
				.ui64Start = 0, .ui64Size = ui64Size, .ui64Offset = 0 };
			m_vLogicalMap.push_back( lsSection );
			UpdateSize();
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
		m_vLogicalMap = std::vector<MX_LOGICAL_SECTION>();
		m_ui32OriginalCrc = 0;
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

	// Updates the file size as per the logical view.
	void CLargeCoWFileWindow::UpdateSize() {
		uint64_t ui64Tmp = 0;

		for ( size_t I = 0; I < m_vLogicalMap.size(); ++I ) {
			ui64Tmp += m_vLogicalMap[I].ui64Size;
		}

		m_ui64Size = ui64Tmp;
	}

	/**
	 * Performs a logical DELETE on a vector of MX_LOGICAL_SECTION items.
	 * \brief Removes the logical range [_ui64DelStart, _ui64DelStart + _ui64DelSize) and compacts everything after by shifting left.
	 *
	 * Sections that overlap the deleted range are trimmed or split as needed. Sections entirely after the
	 * deleted range have their ui64Start reduced by _ui64DelSize. Adjacent compatible sections are coalesced.
	 *
	 * \param _vSections Vector of sections sorted by ui64Start (ties are not expected but tolerated).
	 * \param _ui64DelStart Logical start of the delete range.
	 * \param _ui64DelSize Logical size of the delete range. A value of 0 is a no-op.
	 * \return Returns true if any modification was made to _vSections.
	 **/
	bool CLargeCoWFileWindow::DeleteRange( std::vector<MX_LOGICAL_SECTION> &_vSections, uint64_t _ui64DelStart, uint64_t _ui64DelSize ) {
		if ( _ui64DelSize == 0u ) { return true; }

		// Compute delete end with overflow protection.
		const uint64_t ui64DelEnd = (_ui64DelStart > (std::numeric_limits<uint64_t>::max)() - _ui64DelSize)
			? (std::numeric_limits<uint64_t>::max)()
			: (_ui64DelStart + _ui64DelSize);

		std::vector<MX_LOGICAL_SECTION> vOut;
		vOut.reserve( _vSections.size() );

		bool bChanged = false;

		for ( const MX_LOGICAL_SECTION & lsIn : _vSections ) {
			const uint64_t ui64SecStart = lsIn.ui64Start;
			const uint64_t ui64SecEnd = (ui64SecStart > (std::numeric_limits<uint64_t>::max)() - lsIn.ui64Size)
				? (std::numeric_limits<uint64_t>::max)()
				: (ui64SecStart + lsIn.ui64Size);

			if ( ui64SecEnd <= _ui64DelStart ) {
				// Fully before the delete range.
				vOut.emplace_back( lsIn );
			}
			else if ( ui64SecStart >= ui64DelEnd ) {
				// Fully after the delete range: shift left by the deleted size.
				MX_LOGICAL_SECTION lsShifted = lsIn;
				lsShifted.ui64Start -= _ui64DelSize;
				vOut.emplace_back( lsShifted );
				if ( lsShifted.ui64Start != lsIn.ui64Start ) { bChanged = true; }
			}
			else {
				// Overlaps the delete range; may produce up to 2 pieces: left keep and right keep (split).
				// Left keep: [ui64SecStart, _ui64DelStart)
				if ( ui64SecStart < _ui64DelStart ) {
					MX_LOGICAL_SECTION lsLeft = lsIn;
					const uint64_t ui64NewSize = _ui64DelStart - ui64SecStart;
					lsLeft.ui64Size = ui64NewSize;
					// ui64Start, ui64Offset, and u remain unchanged.
					if ( ui64NewSize != lsIn.ui64Size ) { bChanged = true; }
					vOut.emplace_back( lsLeft );
				}

				// Right keep: [ui64DelEnd, ui64SecEnd) collapses to start at _ui64DelStart after deletion.
				if ( ui64SecEnd > ui64DelEnd ) {
					MX_LOGICAL_SECTION lsRight = lsIn;
					const uint64_t ui64CutInto = ui64DelEnd - ui64SecStart;          // Bytes removed from the head of this section.
					const uint64_t ui64Remain = ui64SecEnd - ui64DelEnd;             // Bytes remaining on the right side.

					lsRight.ui64Start = _ui64DelStart;                               // Collapses onto the delete start.
					lsRight.ui64Size = ui64Remain;
					lsRight.ui64Offset = lsIn.ui64Offset + ui64CutInto;              // Advance backing-file offset by cut amount.

					// Note: If detailed per-source addressing also lives in u.*, only ui64Offset is advanced here.
					// Adjust u.* as appropriate in your codebase if it mirrors ui64Offset internally.

					vOut.emplace_back( lsRight );
					bChanged = true;
				}
				else {
					// If nothing remains on the right and no left kept, the section is fully removed.
					bChanged = true;
				}
			}
		}

		if ( !bChanged ) {
			return false;
		}

		// Sort (defensive; input is expected sorted, but overlap handling might relax ordering when split).
		std::sort( vOut.begin(), vOut.end() );

		// Coalesce adjacent compatible sections:
		// Two sections are coalescible if they are consecutive logically and come from the same source
		// with continuous backing offset progression.
		/*std::vector<MX_LOGICAL_SECTION> vCoalesced;
		vCoalesced.reserve( vOut.size() );

		for ( const MX_LOGICAL_SECTION & lsCur : vOut ) {
			if ( vCoalesced.empty() ) {
				vCoalesced.emplace_back( lsCur );
				continue;
			}

			MX_LOGICAL_SECTION & lsBack = vCoalesced.back();
			const bool bSameType = (lsBack.stType == lsCur.stType);
			const bool bContiguousLogical = (lsBack.ui64Start + lsBack.ui64Size == lsCur.ui64Start);
			const bool bContiguousBacking = (lsBack.ui64Offset + lsBack.ui64Size == lsCur.ui64Offset);

			if ( bSameType && bContiguousLogical && bContiguousBacking ) {
				lsBack.ui64Size += lsCur.ui64Size;
			}
			else {
				vCoalesced.emplace_back( lsCur );
			}
		}

		_vSections.swap( vCoalesced );*/
		_vSections.swap( vOut );
		SimplifyLogicalView();
		return true;
	}

	/**
	 * Creates the directories needed to make it not read-only.
	 * 
	 * \return Returns true if the file was successfully made read/write.
	 **/
	bool CLargeCoWFileWindow::CreateWriteDirectories() {
		m_pEditDiractory = std::filesystem::temp_directory_path() / _DEC_S_9DAF7683_Dues_Hex_Machina.c_str();
		::OutputDebugStringW( (m_pEditDiractory.generic_wstring() + L"\r\n").c_str() );
		m_pEditDiractory /= m_pFilePath.filename();
		//m_pEditDiractory += std::to_string( m_ui32OriginalCrc );
		std::error_code ecError;
		while ( std::filesystem::exists( m_pEditDiractory, ecError ) ) {
			m_pEditDiractory += L'-';
		}
		CFile::EraseFilesInDirectory( m_pEditDiractory );
		if ( !CFile::MakeDirectorySafely( m_pEditDiractory ) ) { return false; }
		return true;
	}

}	// namespace mx

#pragma once

#include <LSWWin.h>
#include "../Files/MXFileMap.h"

#include <filesystem>
#include <mutex>

namespace mx {

	class CLargeCoWFileWindow {
	public :
		CLargeCoWFileWindow();
		~CLargeCoWFileWindow();


		// == Functions.
		// Opens a file for editing.  Maps the first sector to memory.
		bool													OpenFile( const std::filesystem::path &_pFile );

		// Resets the object back to scratch.
		void													Reset();

		// Sets the segment file size.
		void													SetSegmentFileSize( size_t _sSize ) { m_sSegmentFileSize = _sSize; }


	protected :
		// == Enumerations.
		enum MX_SECTION_TYPE {
			MX_ST_ORIGINAL_FILE,								// A section from the original file.
			MX_ST_SEGMENT,										// A section from a segment file.
		};


		// == Types.
		// A modification segment addreess.
		struct MX_SEGMENT_ADDRESS {
			size_t												sFile;											// Index into m_vEditedMaps.
			uint64_t											ui64Offset;										// Offset into the file.
		};

		// A snippet from the original file.
		struct MX_ORIGINAL_ADDRESS {
			uint64_t											ui64Offset;										// Offset into the file.
		};

		// A logical section of the file.
		struct MX_LOGICAL_SECTION {
			MX_SECTION_TYPE										stType;											// A segment from the original file or from a segment file.
			uint64_t											ui64Start;										// The logical starting point of the section.
			uint64_t											ui64Size;										// The logical size of the section.
			union {
				MX_ORIGINAL_ADDRESS								oaOriginalAddress;								// The address inside the original file.
				MX_SEGMENT_ADDRESS								saSegmentAddress;								// The address inside a segment file.
			}													u;
		};


		// == Members.
		CFileMap												m_fmMainMap;									// The map for the main file being edited.
		std::vector<std::unique_ptr<CFileMap>>					m_vEditedMaps;									// Mappings of edited copies.
		std::vector<uint64_t>									m_vActiveSegments;								// List of edited segments currently loaded into RAM.  Sorted by most-recent access.
		std::filesystem::path									m_pEditDiractory;								// Folder where edited copies go.
		std::vector<MX_LOGICAL_SECTION>							m_vLogicalMap;									// The logical file map.
		size_t													m_sSegmentFileSize = 24 * 1024 * 1024;			// Segment file size.  Defaults to 24 megabytes.


		// == Functions.
		// Updates the list of active segments.  Call within a try/catch block.
		void													UpdateActiveSegments( uint64_t _ui64Id, size_t _sMax );
	};

}	// namespace mx

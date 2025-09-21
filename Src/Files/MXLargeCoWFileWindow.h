#pragma once

#include <LSWWin.h>
#include "../Files/MXFileMap.h"

#include <filesystem>
#include <mutex>
#include <vector>

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

		// The file size.
		inline uint64_t											Size() const { return m_ui64Size; }


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
		};

		// A snippet from the original file.
		struct MX_ORIGINAL_ADDRESS {
			
		};

		// A logical section of the file.
		struct MX_LOGICAL_SECTION {
			MX_SECTION_TYPE										stType;											// A segment from the original file or from a segment file.
			uint64_t											ui64Start;										// The logical starting point of the section.
			uint64_t											ui64Size;										// The logical size of the section.
			uint64_t											ui64Offset;										// Offset into the file.
			union {
				MX_ORIGINAL_ADDRESS								oaOriginalAddress;								// The address inside the original file.
				MX_SEGMENT_ADDRESS								saSegmentAddress;								// The address inside a segment file.
			}													u;


			// == Operators.
			inline bool											operator == ( const MX_LOGICAL_SECTION &_lsOther ) const {
				return ui64Start == _lsOther.ui64Start;
			}

			inline bool											operator < ( const MX_LOGICAL_SECTION &_lsOther ) const {
				return ui64Start < _lsOther.ui64Start;
			}

			inline bool											operator > ( const MX_LOGICAL_SECTION &_lsOther ) const {
				return ui64Start > _lsOther.ui64Start;
			}
		};


		// == Members.
		uint64_t												m_ui64Size = 0;									// File size, derived from the logical view.
		CFileMap												m_fmMainMap;									// The map for the main file being edited.
		std::vector<std::unique_ptr<CFileMap>>					m_vEditedMaps;									// Mappings of edited copies.
		std::vector<uint64_t>									m_vActiveSegments;								// List of edited segments currently loaded into RAM.  Sorted by most-recent access.
		std::filesystem::path									m_pEditDiractory;								// Folder where edited copies go.
		std::vector<MX_LOGICAL_SECTION>							m_vLogicalMap;									// The logical file map.
		size_t													m_sSegmentFileSize = 24 * 1024 * 1024;			// Segment file size.  Defaults to 24 megabytes.
		size_t													m_sFileId = 0;									// The file names for segments.
		uint32_t												m_ui32OriginalCrc = 0;							// The CRC at the time of loading.


		// == Functions.
		// Updates the list of active segments.  Call within a try/catch block.
		void													UpdateActiveSegments( uint64_t _ui64Id, size_t _sMax );

		// Simplifies the logical view.
		void													SimplifyLogicalView();

		// Updates the file size as per the logical view.
		void													UpdateSize();

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
		bool													DeleteRange( std::vector<MX_LOGICAL_SECTION> &_vSections, uint64_t _ui64DelStart, uint64_t _ui64DelSize );
	};

}	// namespace mx

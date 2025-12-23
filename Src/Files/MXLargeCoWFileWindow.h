#pragma once

#include <LSWWin.h>
#include "../Files/MXFileMap.h"
#include "../HexEditor/MXHexEditorInterface.h"

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

		// Is the given data stream read-only?
		inline bool												ReadOnly() const { return m_fmMainMap.ReadOnly(); }

		// Read from the file.
		bool													Read( uint64_t _ui64Addr, CHexEditorInterface::CBuffer &_bDst, size_t _sSize ) const;

		// Delete from the file.
		bool													Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted );

		// Begins an Undo sequence.  Call before making multiple delete operations in a row.
		bool													BeginUndo() {
			MX_UNDO_ITEM uiItem = { .uoOp = CHexEditorInterface::MX_UO_GROUP_START };
			return PushUndo( uiItem );
		}

		// Ends an Undo sequence.  Call when done making modifications.
		bool													EndUndo() {
			MX_UNDO_ITEM uiItem = { .uoOp = CHexEditorInterface::MX_UO_GROUP_END };
			return PushUndo( uiItem );
		}

		// Determines if there are items to undo.
		virtual bool											HasUndo() const { return m_sUndoIdx != size_t( -1 ); }

		// Determines if there are items to redo.
		virtual bool											HasRedo() const { return m_sUndoIdx != size_t( m_vUndoStack.size() - 1 ); }

		// Performs an Undo operation.
		virtual bool											Undo( CSecureWString &_swsStatus );

		// Performs an Redo operation.
		virtual bool											Redo( CSecureWString &_swsStatus );




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

		// A single Undo event.
		struct MX_UNDO_ITEM {
			CHexEditorInterface::MX_UNDO_OP						uoOp = CHexEditorInterface::MX_UO_GROUP_START;

			// For CHexEditorInterface::MX_UO_DELETE.
			std::vector<MX_LOGICAL_SECTION>						vDelete_Undo;
			std::vector<MX_LOGICAL_SECTION>						vDelete_Redo;
		};


		// == Members.
		uint64_t												m_ui64Size = 0;									// File size, derived from the logical view.
		CFileMap												m_fmMainMap;									// The map for the main file being edited.
		std::filesystem::path									m_pFilePath;									// the path to the file.
		mutable std::vector<std::unique_ptr<CFileMap>>			m_vEditedMaps;									// Mappings of edited copies.
		mutable std::vector<uint64_t>							m_vActiveSegments;								// List of edited segments currently loaded into RAM.  Sorted by most-recent access.
		std::filesystem::path									m_pEditDiractory;								// Folder where edited copies go.
		std::vector<MX_LOGICAL_SECTION>							m_vLogicalMap;									// The logical file map.
		size_t													m_sSegmentFileSize = 24 * 1024 * 1024;			// Segment file size.  Defaults to 24 megabytes.
		size_t													m_sFileId = 0;									// The file names for segments.
		size_t													m_sTotalActiveSegments = 5;						// The total number of segments to keep active. */
		std::vector<MX_UNDO_ITEM>								m_vUndoStack;									// The Undo stack.
		size_t													m_sUndoIdx = size_t( -1 );						// The Undo stack pointer.
		//uint32_t												m_ui32OriginalCrc = 0;							// The CRC at the time of loading.


		// == Functions.
		// Updates the list of active segments.  Call within a try/catch block.
		void													UpdateActiveSegments( uint64_t _ui64Id, size_t _sMax ) const;

		// Simplifies the logical view.
		void													SimplifyLogicalView();

		// Updates the file size as per the logical view.
		void													UpdateSize();

		// Performs a logical DELETE on a vector of MX_LOGICAL_SECTION items.
		bool													DeleteRange( std::vector<MX_LOGICAL_SECTION> &_vSections, uint64_t _ui64DelStart, uint64_t _ui64DelSize, uint64_t &_ui64Deleted );

		// Creates the directories needed to make it not read-only.
		bool													CreateWriteDirectories();

		// Pushes the given item to the Undo stack.
		inline bool												PushUndo( MX_UNDO_ITEM &_uiItem ) {
			try {
				++m_sUndoIdx;
				m_vUndoStack.resize( m_sUndoIdx + 1 );
				m_vUndoStack[m_sUndoIdx] = std::move( _uiItem );

				return true;
			}
			catch ( ... ) { --m_sUndoIdx; return false; }
		}

		// Performs a single Undo operation.
		CHexEditorInterface::MX_UNDO_OP							SingleUndo();

		// Performs a single Redo operation.
		CHexEditorInterface::MX_UNDO_OP							SingleRedo();
	};

}	// namespace mx

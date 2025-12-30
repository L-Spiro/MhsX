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
		/**
		 * \brief Opens a file for copy-on-write editing and maps the initial window.
		 *
		 * Loads the target file, initializes internal state, and memory-maps the first sector/window
		 * to enable fast reads. Subsequent edits are tracked through the logical map and any edit
		 * segment files as needed.
		 *
		 * \param _pFile Path to the file to open.
		 * \return Returns true if the file was opened and the initial mapping was created; false otherwise.
		 */
		bool													OpenFile( const std::filesystem::path &_pFile );

		/**
		 * \brief Resets the object to its initial, unopened state.
		 *
		 * Clears all logical-view state, unmaps any mapped regions, closes any opened files, and
		 * discards any pending edit tracking. After calling this, the object is equivalent to a
		 * freshly constructed instance.
		 */
		void													Reset();

		/**
		 * \brief Sets the target size, in bytes, for segment files used to store edits.
		 *
		 * Segment files are temporary edit storage. Implementations may create, extend, or recreate
		 * segment files around this size as edits are performed.
		 *
		 * \param _sSize Desired segment file size in bytes.
		 */
		void													SetSegmentFileSize( size_t _sSize ) { m_sSegmentFileSize = _sSize; }

		/**
		 * \brief Gets the current logical size of the edited view.
		 *
		 * This size reflects the user-visible logical file after applying the current logical map,
		 * including any insertions and deletions.
		 *
		 * \return Returns the logical size in bytes.
		 */
		inline uint64_t											Size() const { return m_ui64Size; }

		/**
		 * \brief Indicates whether the underlying file mapping is read-only.
		 *
		 * \return Returns true if edits are not permitted; false otherwise.
		 */
		inline bool												ReadOnly() const { return m_fmMainMap.ReadOnly(); }

		/**
		 * \brief Reads bytes from the logical view into the destination buffer.
		 *
		 * Reads across logical sections as needed. The bytes returned reflect the current edited
		 * view (i.e., after applying the logical map).
		 *
		 * \param _ui64Addr Logical address to read from.
		 * \param _bDst Destination buffer to receive the data. The implementation may resize or append
		 *              depending on the buffer semantics.
		 * \param _sSize Number of bytes to read.
		 * \return Returns true if the requested bytes were read; false otherwise.
		 */
		bool													Read( uint64_t _ui64Addr, CHexEditorInterface::CBuffer &_bDst, size_t _sSize ) const;

		/**
		 * \brief Deletes bytes from the logical view.
		 *
		 * Removes up to the requested number of bytes starting at the given logical address.
		 * The logical map is updated to reflect the deletion.
		 *
		 * \param _ui64Addr Logical address at which deletion begins.
		 * \param _ui64Size Number of bytes requested to delete.
		 * \param _ui64Deleted Receives the number of bytes actually deleted.
		 * \return Returns true if the deletion succeeded; false otherwise.
		 */
		bool													Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted );

		/**
		 * \brief Inserts bytes at the given logical address.
		 *
		 * \param _ui64Addr Logical address at which to insert.
		 * \param _bSrc Bytes to insert.
		 * \param _ui64Inserted Returns the number of bytes inserted.
		 * \return Returns true if the insert succeeded.
		 */
		bool													Insert( uint64_t _ui64Addr, const CHexEditorInterface::CBuffer &_bSrc, uint64_t &_ui64Inserted );

		/**
		 * \brief Begins an Undo grouping sequence.
		 *
		 * Call this before performing multiple modifications that should be undone as a single
		 * user-visible action (for example, a batch of delete operations). Each subsequent Undo
		 * record is added normally, but the group start/end markers allow Undo/Redo to treat them
		 * as one logical unit.
		 *
		 * \return Returns true if the group-start marker was pushed onto the Undo stack; false otherwise.
		 */
		bool													BeginUndo() {
			MX_UNDO_ITEM uiItem = { .uoOp = CHexEditorInterface::MX_UO_GROUP_START };

			if ( PushUndo( uiItem ) ) {
				try {
					m_vItemsInGroup.push_back( 0ULL );
				}
				catch ( ... ) { return false; }
				++m_i32UndoGroupCnt;
				return true;
			}
			return false;
		}

		/**
		 * \brief Ends an Undo grouping sequence.
		 *
		 * Call this after completing a batch of modifications that began with BeginUndo().
		 * This pushes a group-end marker so that Undo/Redo can traverse the grouped operations
		 * correctly.
		 *
		 * \return Returns true if the group-end marker was pushed onto the Undo stack; false otherwise.
		 */
		bool													EndUndo() {
			MX_UNDO_ITEM uiItem = { .uoOp = CHexEditorInterface::MX_UO_GROUP_END };
			if ( PushUndo( uiItem ) ) {
				try {
					m_vItemsInGroup.pop_back();
				}
				catch ( ... ) { return false; }
				--m_i32UndoGroupCnt;
				return true;
			}
			return false;
		}

		/**
		 * \brief Indicates whether there is at least one operation available to undo.
		 *
		 * \return Returns true if an Undo operation is available; false otherwise.
		 */
		virtual bool											HasUndo() const { return m_sUndoIdx != size_t( -1 ); }

		/**
		 * \brief Indicates whether there is at least one operation available to redo.
		 *
		 * \return Returns true if a Redo operation is available; false otherwise.
		 */
		virtual bool											HasRedo() const { return m_sUndoIdx != size_t( m_vUndoStack.size() - 1 ); }

		/**
		 * \brief Performs an Undo operation.
		 *
		 * Undoes the most recent operation (or most recent group if group markers are present),
		 * updating internal state and producing a user-facing status string.
		 *
		 * \param _swsStatus Receives a short status message describing the action performed.
		 * \return Returns true if an Undo operation was performed; false otherwise.
		 */
		virtual bool											Undo( CSecureWString &_swsStatus );

		/**
		 * \brief Performs a Redo operation.
		 *
		 * Redoes the most recently undone operation (or group if group markers are present),
		 * updating internal state and producing a user-facing status string.
		 *
		 * \param _swsStatus Receives a short status message describing the action performed.
		 * \return Returns true if a Redo operation was performed; false otherwise.
		 */
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
		uint64_t												m_ui64Size = 0;									/**< File size, derived from the logical view. */
		CFileMap												m_fmMainMap;									/**< The map for the main file being edited. */
		std::filesystem::path									m_pFilePath;									/**< The path to the file. */
		mutable std::vector<std::unique_ptr<CFileMap>>			m_vEditedMaps;									/**< Mappings of edited copies. */
		mutable std::vector<uint64_t>							m_vActiveSegments;								/**< List of edited segments currently loaded into RAM.  Sorted by most-recent access. */
		std::filesystem::path									m_pEditDiractory;								/**< Folder where edited copies go. */
		std::vector<MX_LOGICAL_SECTION>							m_vLogicalMap;									/**< The logical file map. */
		size_t													m_sSegmentFileSize = 24 * 1024 * 1024;			/**< Segment file size.  Defaults to 24 megabytes. */
		size_t													m_sFileId = 0;									/**< The file names for segments. */
		size_t													m_sTotalActiveSegments = 5;						/**< The total number of segments to keep active. */
		int32_t													m_i32UndoGroupCnt = 0;							/**< How deep inside an Undo group count we are. */

		std::vector<MX_UNDO_ITEM>								m_vUndoStack;									/**< The Undo stack. */
		size_t													m_sUndoIdx = size_t( -1 );						/**< The Undo stack pointer. */
		uint64_t												m_ui64UndoFileId = 0;							/**< The Undo file-backup total. */
		uint64_t												m_ui64UndoStackBase = 0;						/**< The actual size of the Undo stack. */
		std::vector<uint64_t>									m_vItemsInGroup;								/**< For each begin/end Undo group, this is the number of items in it. */

		std::filesystem::path                                   m_pCurSegmentFile;								/**< Current segment file path. */
		std::vector<std::filesystem::path>						m_vSegmentFiles;								/**< A history of segment files to be cleaned up on shut-down. */
		uint64_t												m_ui64CurSegmentUsed = 0;						/**< Next free byte in current segment file. */
		size_t													m_sCurSegmentMapIdx = size_t( -1 );				/**< Index into m_vEditedMaps for current segment file. */


		// == Functions.
		/**
		 * \brief Updates the list of active segment ids.
		 *
		 * Scans the logical view and updates the segment activity list for the given id range.
		 * Call this from within a try/catch block because it may allocate and can throw.
		 *
		 * \param _ui64Id Base segment id to update.
		 * \param _sMax Maximum number of segment ids to consider.
		 */
		void													UpdateActiveSegments( uint64_t _ui64Id, size_t _sMax ) const;

		/**
		 * \brief Simplifies the current logical view.
		 *
		 * Merges adjacent compatible sections, removes empty sections, and normalizes the logical
		 * map to a compact form. This should be called after any operation that mutates the logical
		 * map (insert/delete/split/merge).
		 */
		void													SimplifyLogicalView();

		/**
		 * \brief Updates the cached logical file size to match the logical view.
		 *
		 * Recomputes the current logical size based on the logical map, reflecting the user-visible
		 * file size after all applied modifications.
		 */
		void													UpdateSize();

		/**
		 * \brief Performs a logical delete across a section vector.
		 *
		 * Updates the provided section list by removing up to the requested range. The deletion may
		 * split sections and/or remove sections entirely. The actual deleted byte count is returned.
		 *
		 * \param _vSections Section vector to modify.
		 * \param _ui64DelStart Logical start address of the deletion.
		 * \param _ui64DelSize Number of bytes requested to delete.
		 * \param _ui64Deleted Receives the number of bytes actually deleted.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		bool													DeleteRange( std::vector<MX_LOGICAL_SECTION> &_vSections, uint64_t _ui64DelStart, uint64_t _ui64DelSize, uint64_t &_ui64Deleted );

		/**
		 * \brief Creates edit directory if needed.
		 *
		 * \return Returns true if the edit directory exists or was created.
		 */
		bool													CreateWriteDirectories();

		/**
		 * \brief Builds a full path to a segment file within m_pEditDiractory.  Call within try/catch block.
		 *
		 * \param _sId Segment file numeric id.
		 * \return Full path to the segment file.
		 */
		std::filesystem::path									SegmentFilePath( size_t _sId ) const;

		/**
		 * \brief Ensures the current segment file exists and is sized appropriately for a new allocation.
		 *
		 * Rules:
		 *  - If it does not exist, create it.
		 *  - If it exists but is too small, extend it.
		 *  - If it exists but is too large, create a new file.
		 *  - If it exists but does not have enough remaining space, create a new file.
		 *
		 * \param _ui64NeedBytes Minimum contiguous bytes needed.
		 * \return Returns true if the current segment file is ready.
		 */
		bool													EnsureCurrentSegmentFile( uint64_t _ui64NeedBytes );

		/**
		 * \brief Ensures a mapping exists for the current segment file and returns it.
		 *
		 * \return Returns pointer to the current segment mapping on success; nullptr on failure.
		 */
		CFileMap *												EnsureCurrentSegmentMap();

		/**
		 * \brief Pushes an Undo item onto the Undo stack.
		 *
		 * Advances the Undo index, truncates any redo history, and stores the given item. This is
		 * exception-safe; if storage fails, the Undo index is restored.
		 *
		 * \param _uiItem Undo item to push (moved into the stack).
		 * \return Returns true if the item was pushed; false otherwise.
		 */
		inline bool												PushUndo( MX_UNDO_ITEM &_uiItem ) {
			try {
				++m_sUndoIdx;
				m_vUndoStack.resize( m_sUndoIdx + 1 );
				m_vUndoStack[m_sUndoIdx] = std::move( _uiItem );
				return true;
			}
			catch ( ... ) { --m_sUndoIdx; return false; }
		}

		/**
		 * \brief Performs a single Undo step.
		 *
		 * Executes exactly one undo record from the Undo stack (not an entire group). Group-level
		 * Undo behavior should be implemented by calling this repeatedly while traversing group
		 * markers.
		 *
		 * \return Returns the undo operation type that was processed.
		 */
		CHexEditorInterface::MX_UNDO_OP							SingleUndo();

		/**
		 * \brief Performs a single Redo step.
		 *
		 * Executes exactly one redo record from the Undo stack (not an entire group). Group-level
		 * Redo behavior should be implemented by calling this repeatedly while traversing group
		 * markers.
		 *
		 * \return Returns the undo operation type that was processed.
		 */
		CHexEditorInterface::MX_UNDO_OP							SingleRedo();
	};

}	// namespace mx

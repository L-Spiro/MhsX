#include "MXLargeCoWFileWindow.h"
#include "../CRC/MXCrc.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"
#include "MXFile.h"

#include <algorithm>
#include <cassert>
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

	/**
	 * \brief Resets the object to its initial, unopened state.
	 *
	 * Clears all logical-view state, unmaps any mapped regions, closes any opened files, and
	 * discards any pending edit tracking. After calling this, the object is equivalent to a
	 * freshly constructed instance.
	 */
	void CLargeCoWFileWindow::Reset() {
		try {
			m_ui64Size = 0;
			m_fmMainMap.Close();
			m_pFilePath = std::filesystem::path();
			m_vEditedMaps = std::vector<std::unique_ptr<CFileMap>>();
			m_vActiveSegments = std::vector<uint64_t>();
			m_vLogicalMap = std::vector<MX_LOGICAL_SECTION>();
			m_sFileId = 0;
			m_i32UndoGroupCnt = 0;

			m_vUndoStack = std::vector<MX_UNDO_ITEM>();
			m_sUndoIdx = size_t( -1 );
			m_ui64UndoFileId = 0;
			m_ui64UndoStackBase = 0;
			m_vItemsInGroup = std::vector<uint64_t>();

			m_pCurSegmentFile = std::filesystem::path();
			m_ui64CurSegmentUsed = 0;
			m_sCurSegmentMapIdx = size_t( -1 );
			for ( auto I = m_vSegmentFiles.size(); I--; ) {
				try {
					std::error_code ecError;
					std::filesystem::remove( m_vSegmentFiles[I], ecError );
				}
				catch ( ... ) {}
			}
			m_vSegmentFiles = std::vector<std::filesystem::path>();
		}
		catch ( ... ) {}
	}

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
	bool CLargeCoWFileWindow::Read( uint64_t _ui64Addr, CHexEditorInterface::CBuffer &_bDst, size_t _sSize ) const {
		_sSize = std::min<size_t>( _sSize, Size() - _ui64Addr );

		return Read( _ui64Addr, _bDst, _sSize, m_vLogicalMap );
	}

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
	bool CLargeCoWFileWindow::Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted ) {
		return DeleteRange( m_vLogicalMap, _ui64Addr, _ui64Size, _ui64Deleted );
	}

	/**
	 * \brief Inserts bytes at the given logical address.
	 *
	 * \param _ui64Addr Logical address at which to insert.
	 * \param _bSrc Bytes to insert.
	 * \param _ui64Inserted Returns the number of bytes inserted.
	 * \return Returns true if the insert succeeded.
	 */
	bool CLargeCoWFileWindow::Insert( uint64_t _ui64Addr, const CHexEditorInterface::CBuffer &_bSrc, uint64_t &_ui64Inserted ) {
		_ui64Inserted = 0;
		if ( ReadOnly() ) { return false; }
		if ( _bSrc.empty() ) { return true; }
		if ( !BeginInsertData( _bSrc.size() ) ) { return false; }
		if ( !AppendInsertData( _bSrc.data(), _bSrc.size() ) ) {
			CancelInsertData();
			return false;
		}
		return EndInsertData( _ui64Addr, _ui64Inserted );
	}

	/**
	 * Performs a Copy operation using the given selection, saved to the given clipboard index (max index of TotalClipboards).
	 * 
	 * \param _sSelection The current selection.
	 * \param _ui32BytesPerRow The number of bytes per row.
	 * \param _sClipIdx The cliboard index (between 0 and TotalClipboards).
	 * \param _swsMsg Upon return, holds the message to print in the Status Bar.
	 * \return Returns true if the Copy operation was successful.
	 **/
	bool CLargeCoWFileWindow::Copy( const MX_SELECTION &_sSelection, uint32_t _ui32BytesPerRow, size_t _sClipIdx, const MX_COPYAS &_csParms, CSecureWString &_swsMsg ) {
		if ( _sClipIdx >= TotalClipboards ) {
			_swsMsg = _DEC_WS_1468A1DF_Internal_error_;	// Can only be my mistake.
			return false;
		}
		try {
			m_cbClipBoards[_sClipIdx].vLogicalMap = m_vLogicalMap;
			m_cbClipBoards[_sClipIdx].sSelection = _sSelection;
			m_cbClipBoards[_sClipIdx].ui32BytesPerRow = _ui32BytesPerRow;
			m_cbClipBoards[_sClipIdx].caCopyAs = _csParms;
			m_cbClipBoards[_sClipIdx].ui64Size = m_ui64Size;
			return true;
		}
		catch ( ... ) {
			ReleaseClipboard( _sClipIdx );
			_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * \brief Renders the snapshot held by a clipboard slot into one Windows clipboard format.
	 *
	 * The bytes are read through the logical map saved by Copy(), so later edits do not affect the result.  Segment files are never
	 *	overwritten, so the snapshot stays valid for as long as this object exists.
	 *
	 * \param _sClipIdx The clipboard index (less than TotalClipboards).
	 * \param _uiFormat The clipboard format to produce.
	 * \param _csSink Receives the rendered bytes.
	 * \param _swsMsg Receives an error message on failure.
	 * \return Returns false if the slot is empty or rendering fails.
	 */
	bool CLargeCoWFileWindow::RenderClipboard( size_t _sClipIdx, UINT _uiFormat, CCopySink &_csSink, CSecureWString &_swsMsg ) const {
		if ( _sClipIdx >= TotalClipboards || !m_cbClipBoards[_sClipIdx].sSelection.HasSelection() ) {
			_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		try {
			return CCopyFormatter::Render( ClipboardSource( _sClipIdx ), _uiFormat, _csSink, _swsMsg );
		}
		catch ( ... ) {
			_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * \brief Discards the snapshot held by a clipboard slot.
	 *
	 * \param _sClipIdx The clipboard index (less than TotalClipboards).
	 */
	void CLargeCoWFileWindow::ReleaseClipboard( size_t _sClipIdx ) {
		if ( _sClipIdx >= TotalClipboards ) { return; }
		m_cbClipBoards[_sClipIdx] = MX_CLIPBOARD();
	}

	/**
	 * \brief Gets the number of bytes in the snapshot held by a clipboard slot.
	 *
	 * \param _sClipIdx The clipboard index (less than TotalClipboards).
	 * \return Returns the number of copied bytes, or 0 if the slot is empty.
	 */
	uint64_t CLargeCoWFileWindow::ClipboardBytes( size_t _sClipIdx ) const {
		if ( _sClipIdx >= TotalClipboards ) { return 0; }
		try {
			return CCopyFormatter::TotalBytes( ClipboardSource( _sClipIdx ) );
		}
		catch ( ... ) { return 0; }
	}

	/**
	 * \brief Gets the Copy parameters of the snapshot held by a clipboard slot.
	 *
	 * \param _sClipIdx The clipboard index (less than TotalClipboards).
	 * \param _caCopyAs Receives the Copy parameters.
	 * \return Returns false if the slot is empty.
	 */
	bool CLargeCoWFileWindow::ClipboardCopyAs( size_t _sClipIdx, MX_COPYAS &_caCopyAs ) const {
		if ( _sClipIdx >= TotalClipboards || !m_cbClipBoards[_sClipIdx].sSelection.HasSelection() ) { return false; }
		try {
			_caCopyAs = m_cbClipBoards[_sClipIdx].caCopyAs;
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * \brief Streams the raw selected bytes of the snapshot held by a clipboard slot, in order, to a callback.
	 *
	 * \param _sClipIdx The clipboard index (less than TotalClipboards).
	 * \param _fChunk Receives each block of bytes; returning false stops the stream.
	 * \return Returns false if the slot is empty, a read failed, or _fChunk returned false.
	 */
	bool CLargeCoWFileWindow::ReadClipboard( size_t _sClipIdx, const std::function<bool ( const uint8_t *, size_t )> &_fChunk ) const {
		if ( _sClipIdx >= TotalClipboards || !m_cbClipBoards[_sClipIdx].sSelection.HasSelection() ) { return false; }
		try {
			const CCopyFormatter::MX_COPY_SOURCE csSource = ClipboardSource( _sClipIdx );
			constexpr size_t sChunk = 8 * 1024 * 1024;
			CHexEditorInterface::CBuffer bBuffer;
			const uint64_t ui64Ranges = CCopyFormatter::TotalRanges( csSource );
			for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
				const MX_SEL_RANGE srRange = CCopyFormatter::RangeAt( csSource, I );
				for ( uint64_t ui64Done = 0; ui64Done < srRange.ui64Size; ) {
					const size_t sThis = size_t( std::min<uint64_t>( srRange.ui64Size - ui64Done, sChunk ) );
					if ( !csSource.fRead( srRange.ui64Start + ui64Done, bBuffer, sThis ) || bBuffer.size() < sThis ) { return false; }
					if ( !_fChunk( bBuffer.data(), sThis ) ) { return false; }
					ui64Done += sThis;
				}
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * \brief Inserts the raw bytes of this object's own clipboard snapshot at the given address.
	 *
	 * The snapshot's sections of the original file and segment files are spliced into the logical map, so pasting any amount of
	 *	data copied from this file is instantaneous and uses no additional storage.  A Column selection with very many lines is
	 *	copied into a segment instead, to keep the logical map small.
	 *
	 * \param _ui64Addr The address at which to insert (clamped to the end of the file).
	 * \param _sClipIdx The clipboard index (less than TotalClipboards).
	 * \param _ui64Inserted Receives the number of bytes inserted.
	 * \return Returns true if the insert succeeded.
	 */
	bool CLargeCoWFileWindow::InsertClipboard( uint64_t _ui64Addr, size_t _sClipIdx, uint64_t &_ui64Inserted ) {
		_ui64Inserted = 0;
		if ( ReadOnly() || _sClipIdx >= TotalClipboards || !m_cbClipBoards[_sClipIdx].sSelection.HasSelection() ) { return false; }
		try {
			const CCopyFormatter::MX_COPY_SOURCE csSource = ClipboardSource( _sClipIdx );
			const uint64_t ui64Ranges = CCopyFormatter::TotalRanges( csSource );
			const uint64_t ui64Total = CCopyFormatter::TotalBytes( csSource );
			if ( !ui64Total ) { return true; }

			constexpr uint64_t ui64MaxSplicedRanges = 4096;
			if ( ui64Ranges <= ui64MaxSplicedRanges ) {
				std::vector<MX_LOGICAL_SECTION> vSections;
				uint64_t ui64Pos = 0;
				for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
					const MX_SEL_RANGE srRange = CCopyFormatter::RangeAt( csSource, I );
					if ( !ExtractSections( m_cbClipBoards[_sClipIdx].vLogicalMap, srRange.ui64Start, srRange.ui64Size, ui64Pos, vSections ) ) { return false; }
					ui64Pos += srRange.ui64Size;
				}
				return InsertSections( _ui64Addr, vSections, ui64Total, _ui64Inserted );
			}

			// Many short lines: copy them into one contiguous segment.
			if ( !BeginInsertData( ui64Total ) ) { return false; }
			if ( !ReadClipboard( _sClipIdx, [this]( const uint8_t * _pui8Data, size_t _sSize ) { return AppendInsertData( _pui8Data, _sSize ); } ) ) {
				CancelInsertData();
				return false;
			}
			return EndInsertData( _ui64Addr, _ui64Inserted );
		}
		catch ( ... ) {
			CancelInsertData();
			return false;
		}
	}

	/**
	 * \brief Begins a streamed insertion of a known number of bytes.  Follow with AppendInsertData() and EndInsertData().
	 *
	 * The bytes are written straight into a segment file, so data of any size can be inserted without holding it in RAM.
	 *
	 * \param _ui64Total The exact number of bytes that will be appended.
	 * \return Returns false if the file is read-only, an insertion is already pending, or segment storage could not be prepared.
	 */
	bool CLargeCoWFileWindow::BeginInsertData( uint64_t _ui64Total ) {
		if ( ReadOnly() || m_bInsertPending ) { return false; }
		if ( _ui64Total ) {
			if ( !EnsureCurrentSegmentFile( _ui64Total ) ) { return false; }
			if ( !EnsureCurrentSegmentMap() ) { return false; }
		}
		m_sInsertMapIdx = m_sCurSegmentMapIdx;
		m_ui64InsertStart = m_ui64CurSegmentUsed;
		m_ui64InsertTotal = _ui64Total;
		m_ui64InsertWritten = 0;
		m_bInsertPending = true;
		return true;
	}

	/**
	 * \brief Appends bytes to the pending streamed insertion.
	 *
	 * \param _pvData The bytes to append.
	 * \param _sSize The number of bytes to which _pvData points.
	 * \return Returns false if no insertion is pending, more than the announced total would be written, or the write failed.
	 */
	bool CLargeCoWFileWindow::AppendInsertData( const void * _pvData, size_t _sSize ) {
		if ( !m_bInsertPending || _sSize > m_ui64InsertTotal - m_ui64InsertWritten ) { return false; }
		if ( !_sSize ) { return true; }
		if ( m_sInsertMapIdx >= m_vEditedMaps.size() || !m_vEditedMaps[m_sInsertMapIdx] ) { return false; }
		CFileMap * pfmSeg = m_vEditedMaps[m_sInsertMapIdx].get();

		const uint8_t * pui8Src = static_cast<const uint8_t *>(_pvData);
		while ( _sSize ) {
			const uint32_t ui32This = uint32_t( std::min<size_t>( _sSize, UINT32_MAX ) );
			if ( pfmSeg->Write( pui8Src, m_ui64InsertStart + m_ui64InsertWritten, ui32This ) != ui32This ) { return false; }
			pui8Src += ui32This;
			_sSize -= ui32This;
			m_ui64InsertWritten += ui32This;
		}
		return true;
	}

	/**
	 * \brief Completes the pending streamed insertion, inserting the appended bytes at the given address.
	 *
	 * \param _ui64Addr The address at which to insert (clamped to the end of the file).
	 * \param _ui64Inserted Receives the number of bytes inserted.
	 * \return Returns false (and cancels the insertion) if fewer bytes than announced were appended or the insert failed.
	 */
	bool CLargeCoWFileWindow::EndInsertData( uint64_t _ui64Addr, uint64_t &_ui64Inserted ) {
		_ui64Inserted = 0;
		if ( !m_bInsertPending ) { return false; }
		if ( m_ui64InsertWritten != m_ui64InsertTotal ) {
			CancelInsertData();
			return false;
		}
		m_bInsertPending = false;
		if ( !m_ui64InsertTotal ) { return true; }

		// The bytes now belong to the segment whether or not the insert below succeeds.
		m_ui64CurSegmentUsed = m_ui64InsertStart + m_ui64InsertTotal;
		try {
			MX_LOGICAL_SECTION lsNew{};
			lsNew.stType = MX_ST_SEGMENT;
			lsNew.ui64Start = 0;
			lsNew.ui64Size = m_ui64InsertTotal;
			lsNew.ui64Offset = m_ui64InsertStart;
			lsNew.u.saSegmentAddress.sFile = m_sInsertMapIdx;
			std::vector<MX_LOGICAL_SECTION> vSections( 1, lsNew );
			return InsertSections( _ui64Addr, vSections, m_ui64InsertTotal, _ui64Inserted );
		}
		catch ( ... ) { return false; }
	}

	/**
	 * \brief Abandons the pending streamed insertion, if any.  The file is not changed.
	 */
	void CLargeCoWFileWindow::CancelInsertData() {
		// Nothing was committed to m_ui64CurSegmentUsed, so the written bytes are simply reused by the next insertion.
		m_bInsertPending = false;
		m_ui64InsertTotal = m_ui64InsertWritten = 0;
	}

	/**
	 * \brief Performs an Undo operation.
	 *
	 * Undoes the most recent operation (or most recent group if group markers are present),
	 * updating internal state and producing a user-facing status string.
	 *
	 * \param _swsStatus Receives a short status message describing the action performed.
	 * \return Returns true if an Undo operation was performed; false otherwise.
	 */
	bool CLargeCoWFileWindow::Undo( CSecureWString &_swsStatus ) {
		if ( m_sUndoIdx == size_t( -1 ) ) { return true; }
		if ( m_sUndoIdx >= m_vUndoStack.size() ) {
			_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}

		int32_t i32Scope = 0;
		CHexEditorInterface::MX_UNDO_OP uoUndoType = CHexEditorInterface::MX_UO_INVALID;
		do {
			auto uoOp = SingleUndo();
			if ( uoOp == CHexEditorInterface::MX_UO_INVALID ) {
				_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
				return false;
			}
			else if ( uoOp == CHexEditorInterface::MX_UO_GROUP_START ) {
				--i32Scope;
			}
			else if ( uoOp == CHexEditorInterface::MX_UO_GROUP_END ) {
				++i32Scope;
			}
			else {
				uoUndoType = uoOp;
			}
		}
		while ( i32Scope );

		_swsStatus = _DEC_WS_08D60FD3_Undo_Operation__;
		switch ( uoUndoType ) {
			case CHexEditorInterface::MX_UO_DELETE : {
				_swsStatus += _DEC_WS_63A6700E____Delete___;
				break;
			}
			case CHexEditorInterface::MX_UO_INSERT : {
				_swsStatus += _DEC_WS_39F0A272____Insert___;
				break;
			}
			default : {
				_swsStatus += _DEC_WS_DFA2AFF1_None;
			}
		}

		return true;
	}

	/**
	 * \brief Performs a Redo operation.
	 *
	 * Redoes the most recently undone operation (or group if group markers are present),
	 * updating internal state and producing a user-facing status string.
	 *
	 * \param _swsStatus Receives a short status message describing the action performed.
	 * \return Returns true if a Redo operation was performed; false otherwise.
	 */
	bool CLargeCoWFileWindow::Redo( CSecureWString &_swsStatus ) {
		size_t sNewIdx = m_sUndoIdx + 1;
		if ( sNewIdx == size_t( m_vUndoStack.size() ) ) { return true; }
		if ( sNewIdx >= m_vUndoStack.size() ) {
			_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}

		int32_t i32Scope = 0;
		CHexEditorInterface::MX_UNDO_OP uoUndoType = CHexEditorInterface::MX_UO_INVALID;
		do {
			auto uoOp = SingleRedo();
			if ( uoOp == CHexEditorInterface::MX_UO_INVALID ) {
				_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
				return false;
			}
			else if ( uoOp == CHexEditorInterface::MX_UO_GROUP_START ) {
				++i32Scope;
			}
			else if ( uoOp == CHexEditorInterface::MX_UO_GROUP_END ) {
				--i32Scope;
			}
			else {
				uoUndoType = uoOp;
			}
		}
		while ( i32Scope );

		_swsStatus = _DEC_WS_2BDB5158_Redo_Operation__;
		switch ( uoUndoType ) {
			case CHexEditorInterface::MX_UO_DELETE : {
				_swsStatus += _DEC_WS_63A6700E____Delete___;
				break;
			}
			case CHexEditorInterface::MX_UO_INSERT : {
				_swsStatus += _DEC_WS_39F0A272____Insert___;
				break;
			}
			default : {
				_swsStatus += _DEC_WS_DFA2AFF1_None;
			}
		}

		return true;
	}

	/**
	 * \brief Updates the list of active segment ids.
	 *
	 * Scans the logical view and updates the segment activity list for the given id range.
	 * Call this from within a try/catch block because it may allocate and can throw.
	 *
	 * \param _ui64Id Base segment id to update.
	 * \param _sMax Maximum number of segment ids to consider.
	 */
	void CLargeCoWFileWindow::UpdateActiveSegments( uint64_t _ui64Id, size_t _sMax ) const {
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

		// Close any mappings not in the hot cache.
		for ( auto I = m_vEditedMaps.size(); I--; ) {
			const uint64_t ui64Id = m_vEditedMaps[I]->Id();
			auto aFound = std::find_if(
				m_vActiveSegments.begin(),
				m_vActiveSegments.end(),
				[ui64Id]( const uint64_t _ui64Id ) { return _ui64Id == ui64Id; }
			);
			if ( aFound == m_vActiveSegments.end() ) {
				// This ID was not in the m_vActiveSegments list.  Remove its mapping.
				m_vEditedMaps[I]->UnMapRegion();
			}
		}
	}

	/**
	 * \brief Simplifies the current logical view.
	 *
	 * Merges adjacent compatible sections, removes empty sections, and normalizes the logical
	 * map to a compact form. This should be called after any operation that mutates the logical
	 * map (insert/delete/split/merge).
	 */
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

	/**
	 * \brief Updates the cached logical file size to match the logical view.
	 *
	 * Recomputes the current logical size based on the logical map, reflecting the user-visible
	 * file size after all applied modifications.
	 */
	void CLargeCoWFileWindow::UpdateSize() {
		m_ui64Size = CalcSize( m_vLogicalMap );
	}

	/**
	 * \brief Updates the cached logical file size to match the logical view.
	 *
	 * Recomputes the current logical size based on the logical map, reflecting the user-visible
	 * file size after all applied modifications.
	 * 
	 * \param _vLogicalMap Tnhe logical map on which to calculate the size.
	 */
	uint64_t CLargeCoWFileWindow::CalcSize( const std::vector<MX_LOGICAL_SECTION> &_vLogicalMap ) const {
		uint64_t ui64Tmp = 0;

		for ( size_t I = 0; I < _vLogicalMap.size(); ++I ) {
			ui64Tmp += _vLogicalMap[I].ui64Size;
		}
		return ui64Tmp;
	}

	/**
	 * \brief Reads bytes from the given logical view into the destination buffer.
	 *
	 * Reads across logical sections as needed. The bytes returned reflect the current edited
	 * view (i.e., after applying the logical map).
	 *
	 * \param _ui64Addr Logical address to read from.
	 * \param _bDst Destination buffer to receive the data. The implementation may resize or append
	 *              depending on the buffer semantics.
	 * \param _sSize Number of bytes to read.
	 * \param _vLogicalMap The logical map to use to read the data.
	 * \return Returns true if the requested bytes were read; false otherwise.
	 */
	bool CLargeCoWFileWindow::Read( uint64_t _ui64Addr, CHexEditorInterface::CBuffer &_bDst, size_t _sSize, const std::vector<MX_LOGICAL_SECTION> &_vLogicalMap ) const {
		//_sSize = std::min<size_t>( _sSize, CalcSize( _vLogicalMap ) - _ui64Addr );

		try {
			if MX_UNLIKELY( _sSize == 0 ) {
				_bDst.clear();
				return true;
			}
			_bDst.resize( _sSize );
		}
		catch ( ... ) { return false; }

		size_t sSizeCopy = _sSize;
		uint8_t * pbDst = _bDst.data();
		for ( size_t I = 0; I < _vLogicalMap.size(); ++I ) {
			uint64_t ui64LogicalEnd = _vLogicalMap[I].ui64Start + _vLogicalMap[I].ui64Size;
			if ( _ui64Addr >= _vLogicalMap[I].ui64Start && _ui64Addr < ui64LogicalEnd ) {
				uint64_t ui64MaxSize = std::min<uint64_t>( ui64LogicalEnd - _ui64Addr, sSizeCopy );

				switch ( _vLogicalMap[I].stType ) {
					case MX_ST_ORIGINAL_FILE : {
						if ( ui64MaxSize != static_cast<uint64_t>(m_fmMainMap.Read( pbDst, _vLogicalMap[I].ui64Offset + (_ui64Addr - _vLogicalMap[I].ui64Start), ui64MaxSize )) ) { return false; }
						sSizeCopy -= ui64MaxSize;
						if ( !sSizeCopy ) { return true; }
						_ui64Addr += ui64MaxSize;
						pbDst += ui64MaxSize;
						break;
					}
					case MX_ST_SEGMENT : {
						if ( _vLogicalMap[I].u.saSegmentAddress.sFile >= m_vEditedMaps.size() ) { return false; }
						if ( !m_vEditedMaps[_vLogicalMap[I].u.saSegmentAddress.sFile].get() ) { return false; }
						try {
							auto pfmMap = m_vEditedMaps[_vLogicalMap[I].u.saSegmentAddress.sFile].get();
							UpdateActiveSegments( pfmMap->Id(), m_sTotalActiveSegments );

							if ( ui64MaxSize != static_cast<uint64_t>(pfmMap->Read( pbDst, _vLogicalMap[I].ui64Offset + (_ui64Addr - _vLogicalMap[I].ui64Start), ui64MaxSize )) ) { return false; }
							sSizeCopy -= ui64MaxSize;
							if ( !sSizeCopy ) { return true; }
							_ui64Addr += ui64MaxSize;
							pbDst += ui64MaxSize;
						}
						catch ( ... ) { return false; }


						break;
					}
					default: { return false; }
				}
			}
		}
		// If there is anything left, something went wrong internally.
		//assert( false );
		return false;
	}

	/**
	 * \brief Describes the snapshot held by a clipboard slot for CCopyFormatter.
	 *
	 * \param _sClipIdx The clipboard index (less than TotalClipboards).
	 * \return Returns the copy source, whose read function refers to this object and the slot's logical map.
	 */
	CCopyFormatter::MX_COPY_SOURCE CLargeCoWFileWindow::ClipboardSource( size_t _sClipIdx ) const {
		const MX_CLIPBOARD &cbClip = m_cbClipBoards[_sClipIdx];
		CCopyFormatter::MX_COPY_SOURCE csSource;
		csSource.fRead = [this, &cbClip]( uint64_t _ui64Addr, std::vector<uint8_t> &_vDst, size_t _sSize ) {
			return Read( _ui64Addr, _vDst, _sSize, cbClip.vLogicalMap );
		};
		csSource.sSelection = cbClip.sSelection;
		csSource.ui32BytesPerRow = cbClip.ui32BytesPerRow;
		csSource.caCopyAs = cbClip.caCopyAs;
		csSource.ui64Size = cbClip.ui64Size;
		return csSource;
	}

	/**
	 * \brief Inserts sections into the logical map at the given address, shifting everything after it, and records the Undo item.
	 *
	 * \param _ui64Addr The address at which to insert (clamped to the end of the file).
	 * \param _vSections The sections to insert, contiguous, with ui64Start relative to the first (0-based).
	 * \param _ui64Total The total size of _vSections.
	 * \param _ui64Inserted Receives the number of bytes inserted.
	 * \return Returns true if the insert succeeded.
	 */
	bool CLargeCoWFileWindow::InsertSections( uint64_t _ui64Addr, const std::vector<MX_LOGICAL_SECTION> &_vSections, uint64_t _ui64Total, uint64_t &_ui64Inserted ) {
		_ui64Inserted = 0;
		if ( ReadOnly() ) { return false; }
		if ( !_ui64Total ) { return true; }
		if ( _ui64Addr > Size() ) { _ui64Addr = Size(); }
		try {
			std::vector<MX_LOGICAL_SECTION> vBefore = m_vLogicalMap;
			std::vector<MX_LOGICAL_SECTION> vNew;
			vNew.reserve( m_vLogicalMap.size() + _vSections.size() + 1 );

			// Sections that end at or before the insertion point are unchanged.
			size_t I = 0;
			for ( ; I < m_vLogicalMap.size() && m_vLogicalMap[I].ui64Start + m_vLogicalMap[I].ui64Size <= _ui64Addr; ++I ) {
				vNew.push_back( m_vLogicalMap[I] );
			}
			// A section that straddles the insertion point is split.
			bool bHasRight = false;
			MX_LOGICAL_SECTION lsRight{};
			if ( I < m_vLogicalMap.size() && m_vLogicalMap[I].ui64Start < _ui64Addr ) {
				MX_LOGICAL_SECTION lsLeft = m_vLogicalMap[I];
				lsLeft.ui64Size = _ui64Addr - lsLeft.ui64Start;
				vNew.push_back( lsLeft );

				lsRight = m_vLogicalMap[I];
				lsRight.ui64Start = _ui64Addr;
				lsRight.ui64Offset += lsLeft.ui64Size;
				lsRight.ui64Size -= lsLeft.ui64Size;
				bHasRight = true;
				++I;
			}
			// The new sections.
			for ( const auto &lsThis : _vSections ) {
				MX_LOGICAL_SECTION lsCopy = lsThis;
				lsCopy.ui64Start += _ui64Addr;
				vNew.push_back( lsCopy );
			}
			// Everything after the insertion point moves up.
			if ( bHasRight ) {
				lsRight.ui64Start += _ui64Total;
				vNew.push_back( lsRight );
			}
			for ( ; I < m_vLogicalMap.size(); ++I ) {
				MX_LOGICAL_SECTION lsCopy = m_vLogicalMap[I];
				lsCopy.ui64Start += _ui64Total;
				vNew.push_back( lsCopy );
			}

			m_vLogicalMap.swap( vNew );
			SimplifyLogicalView();
			UpdateSize();

			// Consecutive inserts inside one Undo group become a single Undo item: keep the first "before" and update the "after".
			uint64_t ui64TotalInGroup = m_vItemsInGroup.size() ? m_vItemsInGroup[m_vItemsInGroup.size()-1] : 0;
			if ( ui64TotalInGroup && m_vUndoStack.size() > m_sUndoIdx && m_vUndoStack[m_sUndoIdx].uoOp == CHexEditorInterface::MX_UO_INSERT ) {
				m_vUndoStack[m_sUndoIdx].vDelete_Redo = m_vLogicalMap;
			}
			else {
				MX_UNDO_ITEM uUndoMe = { .uoOp = CHexEditorInterface::MX_UO_INSERT, .vDelete_Undo = std::move( vBefore ), .vDelete_Redo = m_vLogicalMap };
				if ( !PushUndo( uUndoMe ) ) { return false; }
				if ( m_vItemsInGroup.size() ) {
					m_vItemsInGroup[m_vItemsInGroup.size()-1]++;
				}
			}
			_ui64Inserted = _ui64Total;
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * \brief Appends the sections of a logical map that cover a range, clipped to the range.  Call within a try/catch block.
	 *
	 * \param _vMap The logical map to read.
	 * \param _ui64Start The start of the range.
	 * \param _ui64Size The size of the range.
	 * \param _ui64DstStart The ui64Start to give the first appended section; later sections follow contiguously.
	 * \param _vDst Receives the sections.
	 * \return Returns false if the map does not cover the whole range.
	 */
	bool CLargeCoWFileWindow::ExtractSections( const std::vector<MX_LOGICAL_SECTION> &_vMap, uint64_t _ui64Start, uint64_t _ui64Size,
		uint64_t _ui64DstStart, std::vector<MX_LOGICAL_SECTION> &_vDst ) {
		if ( !_ui64Size ) { return true; }
		const uint64_t ui64End = _ui64Start + _ui64Size;
		// The first section that ends after _ui64Start.  Sections are sorted and contiguous.
		auto aIt = std::upper_bound( _vMap.begin(), _vMap.end(), _ui64Start,
			[]( uint64_t _ui64Val, const MX_LOGICAL_SECTION &_lsSec ) { return _ui64Val < _lsSec.ui64Start; } );
		if ( aIt != _vMap.begin() ) { --aIt; }
		uint64_t ui64Covered = 0;
		for ( ; aIt != _vMap.end() && (*aIt).ui64Start < ui64End; ++aIt ) {
			const uint64_t ui64SecEnd = (*aIt).ui64Start + (*aIt).ui64Size;
			const uint64_t ui64From = std::max( _ui64Start, (*aIt).ui64Start );
			const uint64_t ui64To = std::min( ui64End, ui64SecEnd );
			if ( ui64From >= ui64To ) { continue; }
			MX_LOGICAL_SECTION lsPiece = (*aIt);
			lsPiece.ui64Offset += ui64From - (*aIt).ui64Start;
			lsPiece.ui64Start = _ui64DstStart + (ui64From - _ui64Start);
			lsPiece.ui64Size = ui64To - ui64From;
			_vDst.push_back( lsPiece );
			ui64Covered += lsPiece.ui64Size;
		}
		return ui64Covered == _ui64Size;
	}

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
	bool CLargeCoWFileWindow::DeleteRange( std::vector<MX_LOGICAL_SECTION> &_vSections, uint64_t _ui64DelStart, uint64_t _ui64DelSize, uint64_t &_ui64Deleted ) {
		_ui64Deleted = 0;
		if ( _ui64DelSize == 0U ) { return true; }

		// Compute delete end with overflow protection.
		const uint64_t ui64DelEnd = (_ui64DelStart > (std::numeric_limits<uint64_t>::max)() - _ui64DelSize) ?
			(std::numeric_limits<uint64_t>::max)() :
			(_ui64DelStart + _ui64DelSize);

		std::vector<MX_LOGICAL_SECTION> vOut;
		vOut.reserve( _vSections.size() );

		bool bChanged = false;

		for ( const MX_LOGICAL_SECTION & lsIn : _vSections ) {
			const uint64_t ui64SecStart = lsIn.ui64Start;
			const uint64_t ui64SecEnd = (ui64SecStart > (std::numeric_limits<uint64_t>::max)() - lsIn.ui64Size) ?
				(std::numeric_limits<uint64_t>::max)() :
				(ui64SecStart + lsIn.ui64Size);

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

		

		try {
			// Sort (defensive; input is expected sorted, but overlap handling might relax ordering when split).
			std::sort( vOut.begin(), vOut.end() );

			uint64_t ui64TotalInGroup = m_vItemsInGroup.size() ? m_vItemsInGroup[m_vItemsInGroup.size()-1] : 0;
			bool bAddNew = true;
			if ( ui64TotalInGroup && m_vUndoStack.size() > m_sUndoIdx && m_vUndoStack[m_sUndoIdx].uoOp == CHexEditorInterface::MX_UO_DELETE ) {
				m_vUndoStack[m_sUndoIdx].vDelete_Redo = vOut;
				bAddNew = false;
			}
			if ( bAddNew ) {
				MX_UNDO_ITEM uUndoMe = { .uoOp = CHexEditorInterface::MX_UO_DELETE, .vDelete_Undo = _vSections, .vDelete_Redo = vOut };
				if ( !PushUndo( uUndoMe ) ) { return false; }
				if ( m_vItemsInGroup.size() ) {
					m_vItemsInGroup[m_vItemsInGroup.size()-1]++;
				}
			}
		}
		catch ( ... ) { return false; }
		_vSections.swap( vOut );
		SimplifyLogicalView();
		UpdateSize();
		_ui64Deleted = _ui64DelSize;
		return true;
	}

	/**
	 * \brief Creates edit directory if needed.
	 *
	 * \return Returns true if the edit directory exists or was created.
	 */
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

	/**
	 * \brief Builds a full path to a segment file within m_pEditDiractory.  Call within try/catch block.
	 *
	 * \param _sId Segment file numeric id.
	 * \return Full path to the segment file.
	 */
	std::filesystem::path CLargeCoWFileWindow::SegmentFilePath( size_t _sId ) const {
		// Example: "<edit-dir>/seg_000012.mxseg"
		wchar_t wszTmp[64];
		::swprintf_s( wszTmp, L"seg_%06zu.mxseg", _sId );
		return m_pEditDiractory / std::filesystem::path( wszTmp );
	}

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
	bool CLargeCoWFileWindow::EnsureCurrentSegmentFile( uint64_t _ui64NeedBytes ) {
		if ( !CreateWriteDirectories() ) { return false; }

		try {
			// If we don't have a current segment file, pick one.
			if ( m_pCurSegmentFile.empty() ) {
				m_pCurSegmentFile = SegmentFilePath( m_sFileId++ );
				m_ui64CurSegmentUsed = 0;
				m_sCurSegmentMapIdx = size_t( -1 );
				m_vSegmentFiles.push_back( m_pCurSegmentFile );
			}

			auto MakeNew = [&]() -> bool {
				m_pCurSegmentFile = SegmentFilePath( m_sFileId++ );
				m_ui64CurSegmentUsed = 0;
				m_sCurSegmentMapIdx = size_t( -1 );
				m_vSegmentFiles.push_back( m_pCurSegmentFile );
				return true;
			};

			std::error_code ec;
			const bool bExists = std::filesystem::exists( m_pCurSegmentFile, ec );

			const uint64_t ui64TargetSize = std::max( static_cast<uint64_t>( m_sSegmentFileSize ), _ui64NeedBytes );

			if ( !bExists ) {
				// Create new and size it.
				HANDLE hFile = ::CreateFileW(
					m_pCurSegmentFile.c_str(),
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ,
					nullptr,
					CREATE_NEW,
					FILE_ATTRIBUTE_NORMAL,
					nullptr
				);
				if ( hFile == INVALID_HANDLE_VALUE ) { return false; }

				LARGE_INTEGER liSize;
				liSize.QuadPart = static_cast<LONGLONG>( ui64TargetSize );
				if ( ::SetFilePointerEx( hFile, liSize, nullptr, FILE_BEGIN ) == FALSE || ::SetEndOfFile( hFile ) == FALSE ) {
					::CloseHandle( hFile );
					return false;
				}
				::CloseHandle( hFile );
				return true;
			}

			// Exists: check size.
			const uint64_t ui64CurSize = static_cast<uint64_t>( std::filesystem::file_size( m_pCurSegmentFile, ec ) );
			if ( ec ) { return false; }

			// If file is too large, create a new file.
			if ( ui64CurSize > static_cast<uint64_t>(m_sSegmentFileSize) ) {
				MakeNew();
				return EnsureCurrentSegmentFile( _ui64NeedBytes );
			}

			// If too small, extend it, but only while nothing has been written to or mapped from it (an open mapping would conflict with
			//	reopening the file, and data already in it could leave too little contiguous room); otherwise start a new file.
			if ( ui64CurSize < ui64TargetSize && (m_ui64CurSegmentUsed != 0 || m_sCurSegmentMapIdx != size_t( -1 )) ) {
				MakeNew();
				return EnsureCurrentSegmentFile( _ui64NeedBytes );
			}
			if ( ui64CurSize < ui64TargetSize ) {
				HANDLE hFile = ::CreateFileW(
					m_pCurSegmentFile.c_str(),
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ,
					nullptr,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					nullptr
				);
				if ( hFile == INVALID_HANDLE_VALUE ) { return false; }

				LARGE_INTEGER liSize;
				liSize.QuadPart = static_cast<LONGLONG>( ui64TargetSize );
				if ( ::SetFilePointerEx( hFile, liSize, nullptr, FILE_BEGIN ) == FALSE || ::SetEndOfFile( hFile ) == FALSE ) {
					::CloseHandle( hFile );
					return false;
				}
				::CloseHandle( hFile );

				// Force remap next time.
				m_sCurSegmentMapIdx = size_t( -1 );
				return true;
			}

			// Size okay, but do we have contiguous room left?
			// If not, create a new file.
			if ( m_ui64CurSegmentUsed > ui64CurSize || ui64CurSize - m_ui64CurSegmentUsed < _ui64NeedBytes ) {
				MakeNew();
				return EnsureCurrentSegmentFile( _ui64NeedBytes );
			}

			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * \brief Ensures a mapping exists for the current segment file and returns it.
	 *
	 * \return Returns pointer to the current segment mapping on success; nullptr on failure.
	 */
	CFileMap * CLargeCoWFileWindow::EnsureCurrentSegmentMap() {
		try {
			if ( m_pCurSegmentFile.empty() ) { return nullptr; }

			// If we already have an index, ensure it's still valid.
			if ( m_sCurSegmentMapIdx != size_t( -1 ) && m_sCurSegmentMapIdx < m_vEditedMaps.size() && m_vEditedMaps[m_sCurSegmentMapIdx] ) {
				return m_vEditedMaps[m_sCurSegmentMapIdx].get();
			}

			// Create new map object.
			std::unique_ptr<CFileMap> upMap = std::make_unique<CFileMap>();
			if ( !upMap->CreateMap( m_pCurSegmentFile, true ) ) { return nullptr; }
			//if ( !upMap->MapWholeFile() ) { return nullptr; }

			m_sCurSegmentMapIdx = m_vEditedMaps.size();
			m_vEditedMaps.push_back( std::move( upMap ) );

			return m_vEditedMaps[m_sCurSegmentMapIdx].get();
		}
		catch ( ... ) { return nullptr; }
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
	CHexEditorInterface::MX_UNDO_OP CLargeCoWFileWindow::SingleUndo() {
		if ( m_sUndoIdx == size_t( -1 ) ) { return CHexEditorInterface::MX_UO_INVALID; }
		if ( m_sUndoIdx >= m_vUndoStack.size() ) { return CHexEditorInterface::MX_UO_INVALID; }

		CHexEditorInterface::MX_UNDO_OP uoRet = m_vUndoStack[m_sUndoIdx].uoOp;
		switch ( uoRet ) {
			case CHexEditorInterface::MX_UO_INSERT : {}		MX_FALLTHROUGH
			case CHexEditorInterface::MX_UO_DELETE : {
				m_vLogicalMap = m_vUndoStack[m_sUndoIdx].vDelete_Undo;
				UpdateSize();
				break;
			}
		}
		--m_sUndoIdx;

		return uoRet;
	}

	/**
	 * \brief Performs a single Redo step.
	 *
	 * Executes exactly one redo record from the Undo stack (not an entire group). Group-level
	 * Redo behavior should be implemented by calling this repeatedly while traversing group
	 * markers.
	 *
	 * \return Returns the undo operation type that was processed.
	 */
	CHexEditorInterface::MX_UNDO_OP CLargeCoWFileWindow::SingleRedo() {
		size_t sNewIdx = m_sUndoIdx + 1;
		if ( sNewIdx == size_t( -1 ) ) { return CHexEditorInterface::MX_UO_INVALID; }
		if ( sNewIdx >= m_vUndoStack.size() ) { return CHexEditorInterface::MX_UO_INVALID; }

		CHexEditorInterface::MX_UNDO_OP uoRet = m_vUndoStack[sNewIdx].uoOp;
		switch ( uoRet ) {
			case CHexEditorInterface::MX_UO_INSERT : {}		MX_FALLTHROUGH
			case CHexEditorInterface::MX_UO_DELETE : {
				m_vLogicalMap = m_vUndoStack[sNewIdx].vDelete_Redo;
				UpdateSize();
				break;
			}
		}
		++m_sUndoIdx;

		return uoRet;
	}

}	// namespace mx

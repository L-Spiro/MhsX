#include "MXHexEditorProcess.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"

#include <filesystem>


namespace mx {

	// == CProcessUndo.
	/**
	 * Begins an Undo group.  At the outermost level, a new (possibly empty) group is added and anything that could be redone
	 *	is discarded.
	 *
	 * \return Returns true if the group was begun.
	 **/
	bool CProcessUndo::BeginUndo() {
		if ( m_i32Depth == 0 ) {
			try {
				m_vGroups.resize( m_sApplied );
				m_vGroups.emplace_back();
			}
			catch ( ... ) { return false; }
			++m_sApplied;
		}
		++m_i32Depth;
		return true;
	}

	/**
	 * Ends an Undo group begun with BeginUndo().
	 *
	 * \return Returns true if a group was open.
	 **/
	bool CProcessUndo::EndUndo() {
		if ( m_i32Depth == 0 ) { return false; }
		--m_i32Depth;
		return true;
	}

	/**
	 * Records an overwrite.  Outside of a group, the overwrite is a group of its own.
	 *
	 * \param _ui64Addr The address that was written.
	 * \param _bOld The bytes that were there before.  Moved from.
	 * \param _pvNew The bytes that were written.
	 * \param _sSize The number of bytes at _pvNew (and in _bOld).
	 * \return Returns true if the item was recorded.
	 **/
	bool CProcessUndo::AddOverwrite( uint64_t _ui64Addr, CHexEditorInterface::CBuffer &&_bOld, const void * _pvNew, size_t _sSize ) {
		if ( !_sSize ) { return true; }
		const bool bOwnGroup = m_i32Depth == 0;
		if ( bOwnGroup && !BeginUndo() ) { return false; }
		bool bRet = false;
		try {
			MX_OVERWRITE oItem;
			oItem.ui64Addr = _ui64Addr;
			oItem.bOld = std::move( _bOld );
			oItem.bOld.resize( _sSize, 0xFF );
			const uint8_t * pui8New = static_cast<const uint8_t *>(_pvNew);
			oItem.bNew.assign( pui8New, pui8New + _sSize );
			m_vGroups[m_sApplied-1].push_back( std::move( oItem ) );
			bRet = true;
		}
		catch ( ... ) {}
		if ( bOwnGroup ) { EndUndo(); }
		return bRet;
	}

	/**
	 * Undoes the last group by writing back the bytes that were there before it.
	 *
	 * \param _pfWrite Writes the bytes.
	 * \param _swsStatus Receives the text for the status bar.
	 * \return Returns false if a group is still open.  Returns true if there was nothing to undo.
	 **/
	bool CProcessUndo::Undo( const PfWrite &_pfWrite, CSecureWString &_swsStatus ) {
		if ( GroupOpen() ) {
			_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		if ( !HasUndo() ) { return true; }
		const MX_GROUP &gGroup = m_vGroups[m_sApplied-1];
		// Last write first, so that overlapping writes end with the oldest bytes.
		for ( size_t I = gGroup.size(); I--; ) {
			_pfWrite( gGroup[I].ui64Addr, gGroup[I].bOld.data(), gGroup[I].bOld.size() );
		}
		--m_sApplied;
		_swsStatus = _DEC_WS_08D60FD3_Undo_Operation__;
		if ( gGroup.size() ) { _swsStatus += _DEC_WS_2EDB73F7____Overwrite___; }
		else { _swsStatus += _DEC_WS_DFA2AFF1_None; }
		return true;
	}

	/**
	 * Redoes the last undone group by writing its bytes again.
	 *
	 * \param _pfWrite Writes the bytes.
	 * \param _swsStatus Receives the text for the status bar.
	 * \return Returns false if a group is still open.  Returns true if there was nothing to redo.
	 **/
	bool CProcessUndo::Redo( const PfWrite &_pfWrite, CSecureWString &_swsStatus ) {
		if ( GroupOpen() ) {
			_swsStatus = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		if ( !HasRedo() ) { return true; }
		const MX_GROUP &gGroup = m_vGroups[m_sApplied];
		for ( size_t I = 0; I < gGroup.size(); ++I ) {
			_pfWrite( gGroup[I].ui64Addr, gGroup[I].bNew.data(), gGroup[I].bNew.size() );
		}
		++m_sApplied;
		_swsStatus = _DEC_WS_2BDB5158_Redo_Operation__;
		if ( gGroup.size() ) { _swsStatus += _DEC_WS_2EDB73F7____Overwrite___; }
		else { _swsStatus += _DEC_WS_DFA2AFF1_None; }
		return true;
	}

	/**
	 * Discards every item (if no group is open).
	 **/
	void CProcessUndo::Reset() {
		if ( GroupOpen() ) { return; }
		m_vGroups = std::vector<MX_GROUP>();
		m_sApplied = 0;
	}


	// == CHexEditorProcess.
	CHexEditorProcess::CHexEditorProcess() {
	}
	CHexEditorProcess::~CHexEditorProcess() {
	}

	// == Functions.
	// Sets the target process.
	bool CHexEditorProcess::SetProcess( DWORD _dwId, CProcess::MX_OPEN_PROC_MODE _opmMode, DWORD _dwFlags ) {
		return m_pProcess.OpenProc( _dwId, _opmMode, _dwFlags );
	}

	// Gets the text displayed in the tab.
	CSecureWString CHexEditorProcess::TabString() const {
		CSecureWString swsTmp = _DEC_WS_E9985D3A_Process__;
		std::filesystem::path pPath = m_pProcess.QueryProcessImageName( 0 );
		if ( !pPath.has_filename() ) {
			swsTmp += std::to_wstring( m_pProcess.ProcId() );
		}
		else {
			swsTmp += pPath.filename();
			swsTmp += L" (";
			swsTmp += std::to_wstring( m_pProcess.ProcId() );
			swsTmp += L")";
		}
		return swsTmp;
	}

	// Gets the text displayed in the status bar.
	CSecureWString CHexEditorProcess::StatusString() const {
		return TabString();
	}

	// Resets the object back to scratch.
	void CHexEditorProcess::Reset() {
		m_pProcess.Reset();
		m_puUndo.Reset();
	}

	// Read from the given data stream.
	bool CHexEditorProcess::Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const {
		const auto siInfo = mx::CSystem::GetSystemInfo();
		// Read in chunks of siInfo.dwPageSize.
		
		try {
			uint8_t * pui8Dst = nullptr;
			_bDst.resize( _sSize );
			pui8Dst = _bDst.data();


			while ( _sSize ) {
				uint64_t ui64Start = _ui64Addr / siInfo.dwPageSize * siInfo.dwPageSize;
				uint64_t ui64End = (_ui64Addr / siInfo.dwPageSize + 1) * siInfo.dwPageSize;
				_ui64Addr = std::max( _ui64Addr, ui64Start );
				size_t sSize = size_t( ui64End - _ui64Addr );
				sSize = std::min( sSize, _sSize );

				if ( !m_pProcess.ReadProcessMemory( _ui64Addr, pui8Dst, sSize, nullptr ) ) {
					std::memset( pui8Dst, 0xFF, sSize );
				}
				_ui64Addr += sSize;
				pui8Dst += sSize;
				_sSize -= sSize;
			}
			return true;
		}
		catch ( ... ) {
			// Failed to allocate.
			return false;
		}/*
		size_t sOff;
		return m_pProcess.ReadProcessMemory_PreProcessed( _ui64Addr, _bDst, _sSize, sOff, CUtilities::MX_BS_NONE, NULL );*/
	}

	// Overwrite bytes at a given address.
	bool CHexEditorProcess::Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) {
		// The bytes being replaced are kept for Undo (unreadable bytes read as 0xFF, and writing them back does nothing).
		CBuffer bOld;
		const bool bHaveOld = Read( _ui64Addr, bOld, _sSize );
		if ( !WriteToProcess( m_pProcess, _ui64Addr, _pvSrc, _sSize ) ) { return false; }
		if ( bHaveOld ) { m_puUndo.AddOverwrite( _ui64Addr, std::move( bOld ), _pvSrc, _sSize ); }
		return true;
	}

	/**
	 * Writes bytes to a process, changing the protection of the pages temporarily if they cannot otherwise be written.
	 *
	 * \param _pProcess The process.
	 * \param _ui64Addr The address to write.
	 * \param _pvSrc The bytes to write.
	 * \param _sSize The number of bytes to write.
	 * \return Returns false on a memory failure.  Pages that cannot be written are skipped.
	 **/
	bool CHexEditorProcess::WriteToProcess( CProcess &_pProcess, uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) {
		const auto siInfo = mx::CSystem::GetSystemInfo();
		// Read in chunks of siInfo.dwPageSize.
		
		try {
			const uint8_t * pui8Src = reinterpret_cast<const uint8_t *>(_pvSrc);


			while ( _sSize ) {
				uint64_t ui64Start = _ui64Addr / siInfo.dwPageSize * siInfo.dwPageSize;
				uint64_t ui64End = (_ui64Addr / siInfo.dwPageSize + 1) * siInfo.dwPageSize;
				_ui64Addr = std::max( _ui64Addr, ui64Start );
				size_t sSize = size_t( ui64End - _ui64Addr );
				sSize = std::min( sSize, _sSize );

				if ( !_pProcess.WriteProcessMemory( _ui64Addr, pui8Src, sSize, nullptr ) ) {
					// _pProcess re-opens the process with new permissions, but maybe try changing the writability
					if ( _pProcess.IsReadableByQuery( _ui64Addr ) /*|| _pProcess.IsAddressReadable( _ui64Addr )*/ ) {
						std::vector<CProcess::MX_PROT_RANGE> vRestoreMe;
						
						CProcess::MX_PAUSE_RESUME prPauseMe( _pProcess );
						if ( _pProcess.SetWriteableRanges( _ui64Addr, sSize, vRestoreMe ) ) {
							_pProcess.WriteProcessMemory( _ui64Addr, pui8Src, sSize, nullptr );	// Failure doesn’t matter.
							_pProcess.RestoreProtectRanges( vRestoreMe );							// Restore old flags.
						}
					}
					
				}

				_ui64Addr += sSize;
				pui8Src += sSize;
				_sSize -= sSize;
			}
			return true;
		}
		catch ( ... ) {
			// Failed to allocate.
			return false;
		}
	}

	// Delets bytes at a given address.
	bool CHexEditorProcess::Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted ) {
		_ui64Deleted = 0;
		return false;
	}

	// Performs a Copy operation using the given selection, saved to the given clipboard index.
	bool CHexEditorProcess::Copy( const MX_SELECTION &_sSelection, uint32_t _ui32BytesPerRow, size_t _sClipIdx, const MX_COPYAS &_csParms, CSecureWString &_swsMsg ) {
		if ( _sClipIdx >= CCopyPasteManager::TotalClipboards ) {
			_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		try {
			// Process memory can change at any moment, so the data is taken now rather than when it is pasted.
			CCopyFormatter::MX_COPY_SOURCE csSource;
			csSource.fRead = [this]( uint64_t _ui64Addr, std::vector<uint8_t> &_vDst, size_t _sSize ) {
				return Read( _ui64Addr, _vDst, _sSize );
			};
			csSource.sSelection = _sSelection;
			csSource.ui32BytesPerRow = _ui32BytesPerRow;
			csSource.caCopyAs = _csParms;
			csSource.ui64Size = Size();
			if ( csSource.caCopyAs.wsName.empty() ) {
				std::filesystem::path pPath = m_pProcess.QueryProcessImageName( 0 );
				csSource.caCopyAs.wsName = pPath.filename().wstring();
			}
			return CCopyPasteManager::CopyLiveSource( weak_from_this().lock(), csSource, _sClipIdx, m_ccClipBoards[_sClipIdx], _swsMsg );
		}
		catch ( ... ) {
			_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * \brief Renders the bytes captured for a clipboard slot by a previous Copy() into one Windows clipboard format.
	 *
	 * \param _sClipIdx The clipboard slot.
	 * \param _uiFormat The clipboard format to produce.
	 * \param _csSink Receives the rendered bytes.
	 * \param _swsMsg Receives an error message on failure.
	 * \return Returns false if nothing is captured for the slot (slot 0 is never captured) or rendering fails.
	 */
	bool CHexEditorProcess::RenderClipboard( size_t _sClipIdx, UINT _uiFormat, CCopySink &_csSink, CSecureWString &_swsMsg ) const {
		if ( _sClipIdx >= CCopyPasteManager::TotalClipboards ) {
			_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		return m_ccClipBoards[_sClipIdx].Render( _uiFormat, _csSink, _swsMsg );
	}

	/**
	 * \brief Discards the bytes captured for a clipboard slot.
	 *
	 * \param _sClipIdx The clipboard slot.
	 */
	void CHexEditorProcess::ReleaseClipboard( size_t _sClipIdx ) {
		if ( _sClipIdx >= CCopyPasteManager::TotalClipboards ) { return; }
		m_ccClipBoards[_sClipIdx].Reset();
	}

	/**
	 * Performs an Undo operation.
	 *
	 * \param _swsStatus Receives the text for the status bar.
	 * \return Returns true if the operation succeeded (or there was nothing to undo).
	 **/
	bool CHexEditorProcess::Undo( CSecureWString &_swsStatus ) {
		return m_puUndo.Undo( [this]( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) { return WriteToProcess( m_pProcess, _ui64Addr, _pvSrc, _sSize ); }, _swsStatus );
	}

	/**
	 * Performs a Redo operation.
	 *
	 * \param _swsStatus Receives the text for the status bar.
	 * \return Returns true if the operation succeeded (or there was nothing to redo).
	 **/
	bool CHexEditorProcess::Redo( CSecureWString &_swsStatus ) {
		return m_puUndo.Redo( [this]( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) { return WriteToProcess( m_pProcess, _ui64Addr, _pvSrc, _sSize ); }, _swsStatus );
	}

	// Gets the size of the data source.
	uint64_t CHexEditorProcess::Size() const {
		return reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMaximumApplicationAddress);
	}

	// Default starting address.
	uint64_t CHexEditorProcess::DefaultAddress() const {
		return Process().GetMainModuleBase_PEB();
	}

	// Does the given range of addresses contain anything real-time?
	bool CHexEditorProcess::RangeContainsRealTime( uint64_t _ui64Start, uint64_t _ui64Total ) const {
		uint64_t ui64End = _ui64Start + _ui64Total;
		for ( uint64_t I = _ui64Start / mx::CSystem::GetSystemInfo().dwPageSize * mx::CSystem::GetSystemInfo().dwPageSize; I < ui64End; I += mx::CSystem::GetSystemInfo().dwPageSize ) {
			if ( Process().IsWritableByQuery( I ) ) { return true; }
		}
		return false;
	}

}	// namespace mx

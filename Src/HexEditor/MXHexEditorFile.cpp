#include "MXHexEditorFile.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "MXCopyPasteManager.h"


namespace mx {

	CHexEditorFile::CHexEditorFile() {
	}
	CHexEditorFile::~CHexEditorFile() {
		Reset();
	}

	// == Functions.
	// Sets the target file.
	bool CHexEditorFile::SetFile( const std::filesystem::path &_pPath ) {
		return m_lcwfwFile.OpenFile( _pPath );
	}

	// Resets the object back to scratch.
	void CHexEditorFile::Reset() {
		m_lcwfwFile.Reset();
	}

	// Gets the text displayed in the tab.
	CSecureWString CHexEditorFile::TabString() const {
		auto pPath = m_lcwfwFile.Path();
		CSecureWString swsTmp;
		if ( !pPath.has_filename() ) {
			swsTmp = pPath.generic_wstring();
		}
		else {
			swsTmp = pPath.filename().generic_wstring();
		}

		if ( ReadOnly() ) { swsTmp += L" \U0001F512"; }	// Lock.
		return swsTmp;
	}

	// Gets the text displayed in the status bar.
	CSecureWString CHexEditorFile::StatusString() const {
		return m_lcwfwFile.Path().generic_wstring();
	}

	// Read from the given data stream.
	bool CHexEditorFile::Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const {
		return m_lcwfwFile.Read( _ui64Addr, _bDst, _sSize );
	}

	// Does the given data stream allow insertion and deletion?  Processes return true.
	bool CHexEditorFile::OverwriteOnly() const {
		return false;
	}

	// Insert bytes at a given address.
	bool CHexEditorFile::Insert( uint64_t _ui64Addr, const CBuffer &_Src, uint64_t &_ui64Inserted ) {
		return m_lcwfwFile.Insert( _ui64Addr, _Src, _ui64Inserted );
	}

	// Overwrite bytes at a given address.
	bool CHexEditorFile::Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) {
		return false;
	}

	// Delets bytes at a given address.
	bool CHexEditorFile::Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted ) {
		return m_lcwfwFile.Delete( _ui64Addr, _ui64Size, _ui64Deleted );
	}

	// Performs a Copy operation using the given selection, saved to the given clipboard index.
	bool CHexEditorFile::Copy( const MX_SELECTION &_sSelection, uint32_t _ui32BytesPerRow, size_t _sClipIdx, const MX_COPYAS &_csParms, CSecureWString &_swsMsg ) {
		auto spThis = weak_from_this().lock();
		if ( !spThis || _sClipIdx >= CCopyPasteManager::TotalClipboards ) {
			_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		try {
			MX_COPYAS caParms = _csParms;
			if ( caParms.wsName.empty() ) {
				caParms.wsName = m_lcwfwFile.Path().filename().wstring();
			}
			// A file can only change through this object, and the logical map is copy-on-write, so a snapshot of the map is a complete
			//	copy of the data no matter how large the selection is.  Rendering is deferred until something is pasted.
			if ( !m_lcwfwFile.Copy( _sSelection, _ui32BytesPerRow, _sClipIdx, caParms, _swsMsg ) ) { return false; }

			if ( _sClipIdx == CCopyPasteManager::WindowsClipboard ) {
				return CCopyPasteManager::SetWindowsClipboardDeferred( spThis, caParms, m_lcwfwFile.ClipboardBytes( _sClipIdx ), _swsMsg );
			}
			if ( !CCopyPasteManager::SetSlot( _sClipIdx, spThis ) ) {
				m_lcwfwFile.ReleaseClipboard( _sClipIdx );
				_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
				return false;
			}
			return true;
		}
		catch ( ... ) {
			_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * \brief Renders the snapshot held for a clipboard slot by a previous Copy() into one Windows clipboard format.
	 *
	 * \param _sClipIdx The clipboard slot.
	 * \param _uiFormat The clipboard format to produce.
	 * \param _csSink Receives the rendered bytes.
	 * \param _swsMsg Receives an error message on failure.
	 * \return Returns false if the slot holds nothing from this file or rendering fails.
	 */
	bool CHexEditorFile::RenderClipboard( size_t _sClipIdx, UINT _uiFormat, CCopySink &_csSink, CSecureWString &_swsMsg ) const {
		return m_lcwfwFile.RenderClipboard( _sClipIdx, _uiFormat, _csSink, _swsMsg );
	}

	/**
	 * \brief Discards the snapshot held for a clipboard slot.
	 *
	 * \param _sClipIdx The clipboard slot.
	 */
	void CHexEditorFile::ReleaseClipboard( size_t _sClipIdx ) {
		m_lcwfwFile.ReleaseClipboard( _sClipIdx );
	}

	/**
	 * \brief Gets the Copy parameters of the data held for a clipboard slot.
	 *
	 * \param _sClipIdx The clipboard slot.
	 * \param _caCopyAs Receives the Copy parameters.
	 * \return Returns false if nothing is held for the slot.
	 */
	bool CHexEditorFile::ClipboardCopyAs( size_t _sClipIdx, MX_COPYAS &_caCopyAs ) const {
		return m_lcwfwFile.ClipboardCopyAs( _sClipIdx, _caCopyAs );
	}

	/**
	 * \brief Gets the number of raw bytes held for a clipboard slot.
	 *
	 * \param _sClipIdx The clipboard slot.
	 * \return Returns the number of bytes, or 0 if nothing is held for the slot.
	 */
	uint64_t CHexEditorFile::ClipboardBytes( size_t _sClipIdx ) const {
		return m_lcwfwFile.ClipboardBytes( _sClipIdx );
	}

	/**
	 * \brief Streams the raw bytes held for a clipboard slot, in order, to a callback.
	 *
	 * \param _sClipIdx The clipboard slot.
	 * \param _fChunk Receives each block of bytes; returning false stops the stream.
	 * \return Returns false if nothing is held for the slot, a read failed, or _fChunk returned false.
	 */
	bool CHexEditorFile::ReadClipboard( size_t _sClipIdx, const std::function<bool ( const uint8_t *, size_t )> &_fChunk ) const {
		return m_lcwfwFile.ReadClipboard( _sClipIdx, _fChunk );
	}

	/**
	 * \brief Pastes data at an address.
	 *
	 * Data copied from this file is spliced in without copying it; anything else is streamed into segment storage.  In Overwrite
	 *	mode the pasted bytes replace the bytes at the address (extending the file if they run past the end).
	 *
	 * \param _ui64Addr The address at which to paste.
	 * \param _pdData The data to paste (see CCopyPasteManager::GetPasteData()).
	 * \param _bOverwrite If true, Overwrite mode; otherwise Insert mode.
	 * \param _ui64Pasted Receives the number of bytes pasted.
	 * \param _swsMsg Receives an error message on failure.
	 * \return Returns true if the data was pasted.
	 */
	bool CHexEditorFile::Paste( uint64_t _ui64Addr, const MX_PASTE_DATA &_pdData, bool _bOverwrite, uint64_t &_ui64Pasted, CSecureWString &_swsMsg ) {
		_ui64Pasted = 0;
		if ( ReadOnly() ) {
			_swsMsg = _DEC_WS_D4FFE2E0_File_is_Read_Only;
			return false;
		}
		if ( !_pdData.ui64Size ) { return true; }
		try {
			if ( _ui64Addr > Size() ) { _ui64Addr = Size(); }

			// Overwrite mode replaces as many existing bytes as are pasted.
			if ( _bOverwrite ) {
				const uint64_t ui64Replace = std::min( _pdData.ui64Size, Size() - _ui64Addr );
				uint64_t ui64Deleted = 0;
				if ( ui64Replace && !m_lcwfwFile.Delete( _ui64Addr, ui64Replace, ui64Deleted ) ) {
					_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
					return false;
				}
			}

			uint64_t ui64Inserted = 0;
			bool bOk = false;
			if ( _pdData.spSource.get() == static_cast<const CHexEditorInterface *>(this) ) {
				// Copied from this file: splice the snapshot in.
				bOk = m_lcwfwFile.InsertClipboard( _ui64Addr, _pdData.sSourceSlot, ui64Inserted );
			}
			else if ( _pdData.spSource ) {
				// Copied from another source: stream it straight into segment storage.
				bOk = m_lcwfwFile.BeginInsertData( _pdData.ui64Size );
				if ( bOk ) {
					bOk = _pdData.spSource->ReadClipboard( _pdData.sSourceSlot, [this]( const uint8_t * _pui8Data, size_t _sSize ) {
						return m_lcwfwFile.AppendInsertData( _pui8Data, _sSize );
					} );
					if ( bOk ) { bOk = m_lcwfwFile.EndInsertData( _ui64Addr, ui64Inserted ); }
					else { m_lcwfwFile.CancelInsertData(); }
				}
			}
			else {
				bOk = m_lcwfwFile.Insert( _ui64Addr, _pdData.vBytes, ui64Inserted );
			}
			if ( !bOk ) {
				_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
				return false;
			}
			_ui64Pasted = ui64Inserted;
			return true;
		}
		catch ( ... ) {
			_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	// Gets the size of the data source.
	uint64_t CHexEditorFile::Size() const {
		return m_lcwfwFile.Size();
	}

	// Begins an Undo sequence.  Call before making multiple delete operations in a row.
	bool CHexEditorFile::BeginUndo() {
		return m_lcwfwFile.BeginUndo();
	}

	// Ends an Undo sequence.  Call when done making modifications.
	bool CHexEditorFile::EndUndo() {
		return m_lcwfwFile.EndUndo();
	}

	// Determines if there are items to undo.
	bool CHexEditorFile::HasUndo() const {
		return m_lcwfwFile.HasUndo();
	}

	// Determines if there are items to redo.
	bool CHexEditorFile::HasRedo() const {
		return m_lcwfwFile.HasRedo();
	}

	// Performs an Undo operation.
	bool CHexEditorFile::Undo( CSecureWString &_swsStatus ) {
		return m_lcwfwFile.Undo( _swsStatus );
	}

	// Performs an Redo operation.
	bool CHexEditorFile::Redo( CSecureWString &_swsStatus ) {
		return m_lcwfwFile.Redo( _swsStatus );
	}

}	// namespace mx

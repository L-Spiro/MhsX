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

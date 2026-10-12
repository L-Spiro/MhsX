#include "MXHexEditorCurProcess.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"

#include <filesystem>


namespace mx {

	CHexEditorCurProcess::CHexEditorCurProcess( CProcess &_pProcess ) :
		CHexEditorInterface(),
		m_pProcess( _pProcess ) {
	}
	CHexEditorCurProcess::~CHexEditorCurProcess() {
	}

	// == Functions.
	// Resets the object back to scratch.
	void CHexEditorCurProcess::Reset() {
		m_pProcess.Reset();
	}

	// Gets the text displayed in the tab.
	CSecureWString CHexEditorCurProcess::TabString() const {
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
	CSecureWString CHexEditorCurProcess::StatusString() const {
		return TabString();
	}

	// Read from the given data stream.
	bool CHexEditorCurProcess::Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const {
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
	bool CHexEditorCurProcess::Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) {
		return false;
	}

	// Delets bytes at a given address.
	bool CHexEditorCurProcess::Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted ) {
		_ui64Deleted = 0;
		return false;
	}

	// Performs a Copy operation using the given selection, saved to the given clipboard index.
	bool CHexEditorCurProcess::Copy( const MX_SELECTION &_sSelection, uint32_t _ui32BytesPerRow, size_t _sClipIdx, const MX_COPYAS &_csParms, CSecureWString &_swsMsg ) {
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
	bool CHexEditorCurProcess::RenderClipboard( size_t _sClipIdx, UINT _uiFormat, CCopySink &_csSink, CSecureWString &_swsMsg ) const {
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
	void CHexEditorCurProcess::ReleaseClipboard( size_t _sClipIdx ) {
		if ( _sClipIdx >= CCopyPasteManager::TotalClipboards ) { return; }
		m_ccClipBoards[_sClipIdx].Reset();
	}

	/**
	 * \brief Gets the Copy parameters of the data held for a clipboard slot.
	 *
	 * \param _sClipIdx The clipboard slot.
	 * \param _caCopyAs Receives the Copy parameters.
	 * \return Returns false if nothing is held for the slot.
	 */
	bool CHexEditorCurProcess::ClipboardCopyAs( size_t _sClipIdx, MX_COPYAS &_caCopyAs ) const {
		if ( _sClipIdx >= CCopyPasteManager::TotalClipboards || m_ccClipBoards[_sClipIdx].Empty() ) { return false; }
		try {
			_caCopyAs = m_ccClipBoards[_sClipIdx].CopyAs();
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * \brief Gets the number of raw bytes held for a clipboard slot.
	 *
	 * \param _sClipIdx The clipboard slot.
	 * \return Returns the number of bytes, or 0 if nothing is held for the slot.
	 */
	uint64_t CHexEditorCurProcess::ClipboardBytes( size_t _sClipIdx ) const {
		if ( _sClipIdx >= CCopyPasteManager::TotalClipboards ) { return 0; }
		return m_ccClipBoards[_sClipIdx].Data().size();
	}

	/**
	 * \brief Streams the raw bytes held for a clipboard slot, in order, to a callback.
	 *
	 * \param _sClipIdx The clipboard slot.
	 * \param _fChunk Receives each block of bytes; returning false stops the stream.
	 * \return Returns false if nothing is held for the slot, a read failed, or _fChunk returned false.
	 */
	bool CHexEditorCurProcess::ReadClipboard( size_t _sClipIdx, const std::function<bool ( const uint8_t *, size_t )> &_fChunk ) const {
		if ( _sClipIdx >= CCopyPasteManager::TotalClipboards || m_ccClipBoards[_sClipIdx].Empty() ) { return false; }
		const auto &vData = m_ccClipBoards[_sClipIdx].Data();
		return _fChunk( vData.data(), vData.size() );
	}

	// Gets the size of the data source.
	uint64_t CHexEditorCurProcess::Size() const {
		return reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMaximumApplicationAddress);
	}

	// Default starting address.
	uint64_t CHexEditorCurProcess::DefaultAddress() const {
		return m_ui64StartAddress == UINT64_MAX ? Process().GetMainModuleBase_PEB() : m_ui64StartAddress;
	}

	// Does the given range of addresses contain anything real-time?
	bool CHexEditorCurProcess::RangeContainsRealTime( uint64_t _ui64Start, uint64_t _ui64Total ) const {
		uint64_t ui64End = _ui64Start + _ui64Total;
		for ( uint64_t I = _ui64Start / mx::CSystem::GetSystemInfo().dwPageSize * mx::CSystem::GetSystemInfo().dwPageSize; I < ui64End; I += mx::CSystem::GetSystemInfo().dwPageSize ) {
			if ( Process().IsWritableByQuery( I ) ) { return true; }
		}
		return false;
	}

}	// namespace mx

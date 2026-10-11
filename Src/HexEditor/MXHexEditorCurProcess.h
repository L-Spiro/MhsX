#pragma once

#include "../MXMhsX.h"
#include "../MemHack/MXProcess.h"
#include "MXCopyPasteManager.h"
#include "MXHexEditorInterface.h"


namespace mx {

	class CHexEditorCurProcess : public CHexEditorInterface {
	public :
		CHexEditorCurProcess( CProcess &_pProcess );
		virtual ~CHexEditorCurProcess();


		// == Functions.
		// Gets the process object directly.
		const CProcess &					Process() const { return m_pProcess; }

		// Resets the object back to scratch.
		void								Reset();

		// Gets the text displayed in the tab.
		virtual CSecureWString				TabString() const;

		// Gets the text displayed in the status bar.
		virtual CSecureWString				StatusString() const;

		// Read from the given data stream.
		virtual bool						Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const;

		// Does the given data stream allow insertion and deletion?  Processes return true.
		virtual bool						OverwriteOnly() const { return true; }

		// Is the given data stream read-only?
		virtual bool						ReadOnly() const { return !m_pProcess.Writeable(); }

		// Insert bytes at a given address.
		virtual bool						Insert( uint64_t /*_ui64Addr*/, const CBuffer &/*_Src*/, uint64_t &/*_ui64Inserted*/ ) { return false; }

		// Overwrite bytes at a given address.
		virtual bool						Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize );

		// Delets bytes at a given address.
		virtual bool						Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted );

		// Performs a Copy operation using the given selection, saved to the given clipboard index.
		virtual bool						Copy( const MX_SELECTION &_sSelection, uint32_t _ui32BytesPerRow, size_t _sClipIdx, const MX_COPYAS &_csParms, CSecureWString &_swsMsg );

		/**
		 * \brief Renders the bytes captured for a clipboard slot by a previous Copy() into one Windows clipboard format.
		 *
		 * \param _sClipIdx The clipboard slot.
		 * \param _uiFormat The clipboard format to produce.
		 * \param _csSink Receives the rendered bytes.
		 * \param _swsMsg Receives an error message on failure.
		 * \return Returns false if nothing is captured for the slot (slot 0 is never captured) or rendering fails.
		 */
		virtual bool						RenderClipboard( size_t _sClipIdx, UINT _uiFormat, CCopySink &_csSink, CSecureWString &_swsMsg ) const override;

		/**
		 * \brief Discards the bytes captured for a clipboard slot.
		 *
		 * \param _sClipIdx The clipboard slot.
		 */
		virtual void						ReleaseClipboard( size_t _sClipIdx ) override;

		// Gets the size of the data source.
		virtual uint64_t					Size() const;

		// Gets the interface type.
		virtual MX_HEX_EDITOR_TYPES			Type() const { return MX_HET_CUR_PROCESS; }

		// Default starting address.
		virtual uint64_t					DefaultAddress() const override;

		// Sets the default starting address.
		virtual void						SetDefaultAddress( uint64_t _ui64Addr ) override { m_ui64StartAddress = _ui64Addr; }

		// Does the given range of addresses contain anything real-time?
		virtual bool						RangeContainsRealTime( uint64_t _ui64Start, uint64_t _ui64Total ) const override;

		// Begins an Undo sequence.  Call before making multiple delete operations in a row.
		virtual bool						BeginUndo() { return false; }

		// Ends an Undo sequence.  Call when done making modifications.
		virtual bool						EndUndo() { return false; }

		// Determines if there are items to undo.
		virtual bool						HasUndo() const { return false; }

		// Determines if there are items to redo.
		virtual bool						HasRedo() const { return false; }

		// Performs an Undo operation.
		virtual bool						Undo( CSecureWString &/*_swsStatus*/ ) { return false; }

		// Performs an Redo operation.
		virtual bool						Redo( CSecureWString &/*_swsStatus*/ ) { return false; }


	protected :
		// == Members.
		// The target process.
		CProcess &							m_pProcess;
		// The starting address.
		uint64_t							m_ui64StartAddress = UINT64_MAX;
		/** The bytes captured by Copy() for each internal clipboard slot (slot 0 goes straight to the Windows clipboard). */
		CCopyCapture						m_ccClipBoards[CCopyPasteManager::TotalClipboards];
		
	};

}	// namespace mx

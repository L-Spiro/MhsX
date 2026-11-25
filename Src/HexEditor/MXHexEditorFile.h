#pragma once

#include "../MXMhsX.h"
#include "../Files/MXLargeCoWFileWindow.h"
#include "MXHexEditorInterface.h"

#include <filesystem>


namespace mx {

	class CHexEditorFile : public CHexEditorInterface {
	public :
		CHexEditorFile();
		virtual ~CHexEditorFile();


		// == Functions.
		// Sets the target file.
		bool								SetFile( const std::filesystem::path &_pPath );

		/**
		 * Starts a new, untitled file that exists only in memory and edit storage until it is saved.
		 *
		 * \param _wsName The name shown for the file until it is saved (for example, "Untitled1.txt").
		 * \param _bInitial The initial contents (for example, a byte-order mark).  They are not part of the Undo history.
		 * \return Returns true if the file was created.
		 **/
		bool								CreateNew( const std::wstring &_wsName, const CBuffer &_bInitial );

		/**
		 * Determines whether this is a new file that has never been saved.
		 *
		 * \return Returns true if the file has no path yet.
		 **/
		inline bool							IsUntitled() const { return m_lcwfwFile.IsUntitled(); }

		/**
		 * Determines whether the data differs from what was last opened or saved.
		 *
		 * \return Returns true if there are unsaved changes.
		 **/
		inline bool							IsModified() const { return m_lcwfwFile.IsModified(); }

		/**
		 * Gets the path of the file.
		 *
		 * \return Returns the path, which is empty for an untitled file.
		 **/
		inline const std::filesystem::path &	Path() const { return m_lcwfwFile.Path(); }

		/**
		 * Gets the name of the file: its file name, or the name given to CreateNew() if it is untitled.
		 *
		 * \return Returns the name.
		 **/
		CSecureWString						DisplayName() const;

		/**
		 * Saves the file, which takes the given path (see CLargeCoWFileWindow::SaveTo()).
		 *
		 * \param _pPath The file to write.
		 * \param _dwError Receives the Windows error code on failure.
		 * \return Returns true if the file was saved.
		 **/
		bool								SaveTo( const std::filesystem::path &_pPath, DWORD &_dwError ) { return m_lcwfwFile.SaveTo( _pPath, _dwError ); }

		/**
		 * Renames the file on disk.
		 *
		 * \param _pPath The new path.  It must not exist.
		 * \param _dwError Receives the Windows error code on failure.
		 * \return Returns true if the file was renamed.
		 **/
		bool								Rename( const std::filesystem::path &_pPath, DWORD &_dwError ) { return m_lcwfwFile.Rename( _pPath, _dwError ); }

		/**
		 * Writes a range of the file to an open file at its current position.
		 *
		 * \param _hFile The file, opened for writing.
		 * \param _ui64Start The address of the first byte to write.
		 * \param _ui64Size The number of bytes to write.
		 * \param _dwError Receives the Windows error code on failure.
		 * \return Returns true if every byte was written.
		 **/
		bool								WriteRange( HANDLE _hFile, uint64_t _ui64Start, uint64_t _ui64Size, DWORD &_dwError ) const { return m_lcwfwFile.WriteRange( _hFile, _ui64Start, _ui64Size, _dwError ); }

		// Resets the object back to scratch.
		void								Reset();

		// Gets the text displayed in the tab.
		virtual CSecureWString				TabString() const;

		// Gets the text displayed in the status bar.
		virtual CSecureWString				StatusString() const;

		// Read from the given data stream.
		virtual bool						Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const;

		// Does the given data stream allow insertion and deletion?  Processes return true.
		virtual bool						OverwriteOnly() const;

		// Is the file itself read-only (regardless of Read Only mode)?
		virtual bool						SourceReadOnly() const { return m_lcwfwFile.ReadOnly(); }

		/**
		 * Determines whether saving the file over itself keeps its modification time (Edit -> Keep File Time).
		 *
		 * \return Returns true if the file's times are restored after it is saved.
		 **/
		inline bool							KeepFileTime() const { return m_bKeepFileTime; }

		/**
		 * Sets whether saving the file over itself keeps its modification time (Edit -> Keep File Time).
		 *
		 * \param _bKeep If true, the file's times are restored after it is saved.
		 **/
		inline void							SetKeepFileTime( bool _bKeep ) { m_bKeepFileTime = _bKeep; }

		// Insert bytes at a given address.
		virtual bool						Insert( uint64_t _ui64Addr, const CBuffer &_Src, uint64_t &_ui64Inserted );

		// Overwrite bytes at a given address.
		virtual bool						Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize );

		// Delets bytes at a given address.
		virtual bool						Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted );

		// Performs a Copy operation using the given selection, saved to the given clipboard index.
		virtual bool						Copy( const MX_SELECTION &_sSelection, uint32_t _ui32BytesPerRow, size_t _sClipIdx, const MX_COPYAS &_csParms, CSecureWString &_swsMsg );

		/**
		 * \brief Renders the snapshot held for a clipboard slot by a previous Copy() into one Windows clipboard format.
		 *
		 * \param _sClipIdx The clipboard slot.
		 * \param _uiFormat The clipboard format to produce.
		 * \param _csSink Receives the rendered bytes.
		 * \param _swsMsg Receives an error message on failure.
		 * \return Returns false if the slot holds nothing from this file or rendering fails.
		 */
		virtual bool						RenderClipboard( size_t _sClipIdx, UINT _uiFormat, CCopySink &_csSink, CSecureWString &_swsMsg ) const override;

		/**
		 * \brief Discards the snapshot held for a clipboard slot.
		 *
		 * \param _sClipIdx The clipboard slot.
		 */
		virtual void						ReleaseClipboard( size_t _sClipIdx ) override;

		/**
		 * \brief Gets the Copy parameters of the data held for a clipboard slot.
		 *
		 * \param _sClipIdx The clipboard slot.
		 * \param _caCopyAs Receives the Copy parameters.
		 * \return Returns false if nothing is held for the slot.
		 */
		virtual bool						ClipboardCopyAs( size_t _sClipIdx, MX_COPYAS &_caCopyAs ) const override;

		/**
		 * \brief Gets the number of raw bytes held for a clipboard slot.
		 *
		 * \param _sClipIdx The clipboard slot.
		 * \return Returns the number of bytes, or 0 if nothing is held for the slot.
		 */
		virtual uint64_t					ClipboardBytes( size_t _sClipIdx ) const override;

		/**
		 * \brief Streams the raw bytes held for a clipboard slot, in order, to a callback.
		 *
		 * \param _sClipIdx The clipboard slot.
		 * \param _fChunk Receives each block of bytes; returning false stops the stream.
		 * \return Returns false if nothing is held for the slot, a read failed, or _fChunk returned false.
		 */
		virtual bool						ReadClipboard( size_t _sClipIdx, const std::function<bool ( const uint8_t *, size_t )> &_fChunk ) const override;

		/**
		 * \brief Determines whether Paste() is supported (and the source is writable).
		 *
		 * \return Returns true if the file is not read-only.
		 */
		virtual bool						CanPaste() const override { return !ReadOnly(); }

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
		virtual bool						Paste( uint64_t _ui64Addr, const MX_PASTE_DATA &_pdData, bool _bOverwrite, uint64_t &_ui64Pasted, CSecureWString &_swsMsg ) override;

		// Gets the size of the data source.
		virtual uint64_t					Size() const;

		// Gets the interface type.
		virtual MX_HEX_EDITOR_TYPES			Type() const { return MX_HET_FILE; }

		// Begins an Undo sequence.  Call before making multiple delete operations in a row.
		virtual bool						BeginUndo();

		// Ends an Undo sequence.  Call when done making modifications.
		virtual bool						EndUndo();

		// Determines if there are items to undo.
		virtual bool						HasUndo() const;

		// Determines if there are items to redo.
		virtual bool						HasRedo() const;

		// Performs an Undo operation.
		virtual bool						Undo( CSecureWString &_swsStatus );

		// Performs an Redo operation.
		virtual bool						Redo( CSecureWString &_swsStatus );


	protected :
		// == Members.
		// The copy-on-write file.
		CLargeCoWFileWindow					m_lcwfwFile;
		/** The name of an untitled file. */
		std::wstring						m_wsNewName;
		/** If true, saving the file over itself keeps its creation and modification times. */
		bool								m_bKeepFileTime = false;
	};

}	// namespace mx

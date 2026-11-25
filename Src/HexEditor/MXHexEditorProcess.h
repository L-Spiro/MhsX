#pragma once

#include "../MXMhsX.h"
#include "../MemHack/MXProcess.h"
#include "MXCopyPasteManager.h"
#include "MXHexEditorInterface.h"


namespace mx {

	/**
	 * An Undo/Redo stack for data sources that can only be overwritten (process memory).  Each item keeps the bytes from before and
	 *	after an overwrite: Undo writes the old bytes back and Redo writes the new bytes again.  Items made between BeginUndo() and
	 *	the matching EndUndo() are undone and redone together, as are the groups of a file's Undo stack, so that the hex control's
	 *	selection Undo items stay in step with it.
	 **/
	class CProcessUndo {
	public :
		/** Writes bytes to the data source without recording an Undo item. */
		typedef std::function<bool ( uint64_t, const void *, size_t )>
											PfWrite;


		// == Functions.
		/**
		 * Begins an Undo group.  At the outermost level, a new (possibly empty) group is added and anything that could be redone
		 *	is discarded.
		 *
		 * \return Returns true if the group was begun.
		 **/
		bool								BeginUndo();

		/**
		 * Ends an Undo group begun with BeginUndo().
		 *
		 * \return Returns true if a group was open.
		 **/
		bool								EndUndo();

		/**
		 * Records an overwrite.  Outside of a group, the overwrite is a group of its own.
		 *
		 * \param _ui64Addr The address that was written.
		 * \param _bOld The bytes that were there before.  Moved from.
		 * \param _pvNew The bytes that were written.
		 * \param _sSize The number of bytes at _pvNew (and in _bOld).
		 * \return Returns true if the item was recorded.
		 **/
		bool								AddOverwrite( uint64_t _ui64Addr, CHexEditorInterface::CBuffer &&_bOld, const void * _pvNew, size_t _sSize );

		/**
		 * Determines if there are items to undo.
		 *
		 * \return Returns true if Undo() would undo something.
		 **/
		bool								HasUndo() const { return m_sApplied != 0; }

		/**
		 * Determines if there are items to redo.
		 *
		 * \return Returns true if Redo() would redo something.
		 **/
		bool								HasRedo() const { return m_sApplied < m_vGroups.size(); }

		/**
		 * Undoes the last group by writing back the bytes that were there before it.
		 *
		 * \param _pfWrite Writes the bytes.
		 * \param _swsStatus Receives the text for the status bar.
		 * \return Returns false if a group is still open.  Returns true if there was nothing to undo.
		 **/
		bool								Undo( const PfWrite &_pfWrite, CSecureWString &_swsStatus );

		/**
		 * Redoes the last undone group by writing its bytes again.
		 *
		 * \param _pfWrite Writes the bytes.
		 * \param _swsStatus Receives the text for the status bar.
		 * \return Returns false if a group is still open.  Returns true if there was nothing to redo.
		 **/
		bool								Redo( const PfWrite &_pfWrite, CSecureWString &_swsStatus );

		/**
		 * Discards every item (if no group is open).
		 **/
		void								Reset();

		/**
		 * Determines whether a group is open.
		 *
		 * \return Returns true between BeginUndo() and the matching EndUndo().
		 **/
		bool								GroupOpen() const { return m_i32Depth != 0; }


	protected :
		// == Types.
		/** One overwrite. */
		struct MX_OVERWRITE {
			/** The address that was written. */
			uint64_t						ui64Addr = 0;
			/** The bytes before the write. */
			CHexEditorInterface::CBuffer	bOld;
			/** The bytes that were written. */
			CHexEditorInterface::CBuffer	bNew;
		};

		/** The overwrites undone/redone together. */
		typedef std::vector<MX_OVERWRITE>	MX_GROUP;


		// == Members.
		/** The groups, oldest first. */
		std::vector<MX_GROUP>				m_vGroups;
		/** The number of groups (from the start of m_vGroups) currently applied.  The rest can be redone. */
		size_t								m_sApplied = 0;
		/** How deeply BeginUndo() calls are nested. */
		int32_t								m_i32Depth = 0;
	};

	class CHexEditorProcess : public CHexEditorInterface {
	public :
		CHexEditorProcess();
		virtual ~CHexEditorProcess();


		// == Functions.
		// Sets the target process.
		bool								SetProcess( DWORD _dwId, CProcess::MX_OPEN_PROC_MODE _opmMode, DWORD _dwFlags );

		// Gets the process object directly.
		const CProcess &					Process() const { return m_pProcess; }

		// Gets the text displayed in the tab.
		virtual CSecureWString				TabString() const;

		// Gets the text displayed in the status bar.
		virtual CSecureWString				StatusString() const;

		// Resets the object back to scratch.
		void								Reset();

		/**
		 * Writes bytes to a process, changing the protection of the pages temporarily if they cannot otherwise be written.
		 *
		 * \param _pProcess The process.
		 * \param _ui64Addr The address to write.
		 * \param _pvSrc The bytes to write.
		 * \param _sSize The number of bytes to write.
		 * \return Returns false on a memory failure.  Pages that cannot be written are skipped.
		 **/
		static bool							WriteToProcess( CProcess &_pProcess, uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize );

		// Read from the given data stream.
		virtual bool						Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const;

		// Does the given data stream allow insertion and deletion?  Processes return true.
		virtual bool						OverwriteOnly() const { return true; }

		// Is the process itself read-only (regardless of Read Only mode)?
		virtual bool						SourceReadOnly() const { return !m_pProcess.Writeable(); }

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
		virtual MX_HEX_EDITOR_TYPES			Type() const { return MX_HET_PROCESS; }

		// Default starting address.
		virtual uint64_t					DefaultAddress() const override;

		// Does the given range of addresses contain anything real-time?
		virtual bool						RangeContainsRealTime( uint64_t _ui64Start, uint64_t _ui64Total ) const override;

		// Begins an Undo sequence.  Call before making multiple overwrite operations in a row.
		virtual bool						BeginUndo() { return m_puUndo.BeginUndo(); }

		// Ends an Undo sequence.  Call when done making modifications.
		virtual bool						EndUndo() { return m_puUndo.EndUndo(); }

		// Determines if there are items to undo.
		virtual bool						HasUndo() const { return m_puUndo.HasUndo(); };

		// Determines if there are items to redo.
		virtual bool						HasRedo() const { return m_puUndo.HasRedo(); };

		// Performs an Undo operation.
		virtual bool						Undo( CSecureWString &_swsStatus );

		// Performs an Redo operation.
		virtual bool						Redo( CSecureWString &_swsStatus );


	protected :
		// == Members.
		// The target process.
		CProcess							m_pProcess;
		/** The bytes captured by Copy() for each internal clipboard slot (slot 0 goes straight to the Windows clipboard). */
		CCopyCapture						m_ccClipBoards[CCopyPasteManager::TotalClipboards];
		/** The Undo/Redo stack of the edits made to the process's memory. */
		CProcessUndo						m_puUndo;
		
	};

}	// namespace mx

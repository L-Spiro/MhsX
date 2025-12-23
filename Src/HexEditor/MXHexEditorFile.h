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

		// Resets the object back to scratch.
		void								Reset();

		// Read from the given data stream.
		virtual bool						Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const;

		// Does the given data stream allow insertion and deletion?  Processes return true.
		virtual bool						OverwriteOnly() const;

		// Is the given data stream read-only?
		virtual bool						ReadOnly() const { return m_lcwfwFile.ReadOnly(); }

		// Insert bytes at a given address.
		virtual bool						Insert( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize );

		// Overwrite bytes at a given address.
		virtual bool						Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize );

		// Delets bytes at a given address.
		virtual bool						Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted );

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
	};

}	// namespace mx

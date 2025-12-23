#pragma once

#include "../MXMhsX.h"

namespace mx {

	class CHexEditorInterface {
	public :
		virtual ~CHexEditorInterface() {}
		// == Enumerations.
		// Interface types.
		enum MX_HEX_EDITOR_TYPES {
			MX_HET_NONE,
			MX_HET_FILE,
			MX_HET_PROCESS,
			MX_HET_CUR_PROCESS
		};

		// Undo operations.
		enum MX_UNDO_OP {
			MX_UO_INVALID,
			MX_UO_GROUP_START,
			MX_UO_GROUP_END,
			MX_UO_DELETE,
		};


		// == Types.
		// Read/write buffer.
		typedef std::vector<uint8_t>		CBuffer;

		/** An Undo edit block. */
		struct MX_UNDO {
			MX_UNDO( CHexEditorInterface * _pheiInterface ) :
				pheiInterface( _pheiInterface ),
				bUndoResult( _pheiInterface && _pheiInterface->BeginUndo() ) {
			}
			~MX_UNDO() {
				if ( Ready() ) {
					pheiInterface->EndUndo();
				}
				pheiInterface = nullptr;
			}


			// == Functions.
			// Returns true if the call to BeginUndo() was successful.
			inline bool						Ready() const { return pheiInterface && bUndoResult; }


			// == Members.
			CHexEditorInterface *			pheiInterface = nullptr;
			bool							bUndoResult = false;
		};


		// == Functions.
		// Read from the given data stream.
		virtual bool						Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const = 0;

		// Does the given data stream allow insertion and deletion?  Processes return true.
		virtual bool						OverwriteOnly() const = 0;

		// Is the given data stream read-only?
		virtual bool						ReadOnly() const { return false; }

		// Insert bytes at a given address.
		virtual bool						Insert( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) = 0;

		// Overwrite bytes at a given address.
		virtual bool						Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) = 0;

		// Delets bytes at a given address.
		virtual bool						Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted ) = 0;

		// Gets the size of the data source.
		virtual uint64_t					Size() const = 0;

		// Gets the interface type.
		virtual MX_HEX_EDITOR_TYPES			Type() const = 0 { return MX_HET_NONE; }

		// Default starting address.
		virtual uint64_t					DefaultAddress() const { return 0; }

		// Does the given range of addresses contain anything real-time?
		virtual bool						RangeContainsRealTime( uint64_t _ui64Start, uint64_t _ui64Total ) const { return false; }

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
	};

}	// namespace mx

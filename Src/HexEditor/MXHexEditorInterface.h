#pragma once

#include "../MXMhsX.h"
#include "MXCharSets.h"
#include "MXCopyAs.h"
#include "MXCopyFormatter.h"
#include "MXDataFmt.h"
#include "MXSelection.h"

#include <memory>

namespace mx {

	/**
	 * Class CHexEditorInterface
	 * \brief The data source behind a hex-editor tab.
	 *
	 * Instances are always owned by std::shared_ptr (the tab and, after a Copy, CCopyPasteManager), so an interface can register
	 *	itself with CCopyPasteManager during Copy() via weak_from_this().
	 */
	class CHexEditorInterface : public std::enable_shared_from_this<CHexEditorInterface> {
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
			MX_UO_INSERT,
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
		// Gets the text displayed in the tab.
		virtual CSecureWString				TabString() const = 0;

		// Gets the text displayed in the status bar.
		virtual CSecureWString				StatusString() const = 0;

		// Read from the given data stream.
		virtual bool						Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const = 0;

		// Does the given data stream allow insertion and deletion?  Processes return true.
		virtual bool						OverwriteOnly() const = 0;

		// Is the given data stream read-only?
		virtual bool						ReadOnly() const { return false; }

		// Insert bytes at a given address.
		virtual bool						Insert( uint64_t _ui64Addr, const CBuffer &_Src, uint64_t &_ui64Inserted ) = 0;

		// Overwrite bytes at a given address.
		virtual bool						Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) = 0;

		// Delets bytes at a given address.
		virtual bool						Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted ) = 0;

		// Performs a Copy operation using the given selection, saved to the given clipboard index.
		virtual bool						Copy( const MX_SELECTION &_sSelection, uint32_t _ui32BytesPerRow, size_t _sClipIdx, const MX_COPYAS &_csParms, CSecureWString &_swsMsg ) = 0;

		/**
		 * \brief Renders the data held for a clipboard slot by a previous Copy() into one Windows clipboard format.
		 *
		 * Called by CCopyPasteManager for deferred rendering (WM_RENDERFORMAT/WM_RENDERALLFORMATS) and available for Paste.  Sources
		 *	that place their data on the Windows clipboard immediately need not hold anything for slot 0.
		 *
		 * \param _sClipIdx The clipboard slot.
		 * \param _uiFormat The clipboard format to produce (one of CCopyFormatter::Formats() for the copy type).
		 * \param _csSink Receives the rendered bytes.
		 * \param _swsMsg Receives an error message on failure.
		 * \return Returns false if the slot holds nothing from this interface or rendering fails.
		 */
		virtual bool						RenderClipboard( size_t /*_sClipIdx*/, UINT /*_uiFormat*/, CCopySink &/*_csSink*/, CSecureWString &/*_swsMsg*/ ) const { return false; }

		/**
		 * \brief Discards the data held for a clipboard slot.  Called by CCopyPasteManager when the slot stops referring to this interface.
		 *
		 * \param _sClipIdx The clipboard slot.
		 */
		virtual void						ReleaseClipboard( size_t /*_sClipIdx*/ ) {}

		// Gets the size of the data source.
		virtual uint64_t					Size() const = 0;

		// Gets the interface type.
		virtual MX_HEX_EDITOR_TYPES			Type() const = 0 { return MX_HET_NONE; }

		// Default starting address.
		virtual uint64_t					DefaultAddress() const { return 0; }

		// Sets the default starting address.
		virtual void						SetDefaultAddress( uint64_t /*_ui64Addr*/ ) {}

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

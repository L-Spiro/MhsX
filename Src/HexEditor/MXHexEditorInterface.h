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


		// == Types.
		// Read/write buffer.
		typedef std::vector<uint8_t>		CBuffer;


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
		virtual bool						Delete( uint64_t _ui64Addr, uint64_t _ui64Size ) = 0;

		// Gets the size of the data source.
		virtual uint64_t					Size() const = 0;

		// Gets the interface type.
		virtual MX_HEX_EDITOR_TYPES			Type() const = 0 { return MX_HET_NONE; }
	};

}	// namespace mx

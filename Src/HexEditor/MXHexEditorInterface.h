#pragma once

#include "../MXMhsX.h"

namespace mx {

	class CHexEditorInterface {
	public :
		virtual ~CHexEditorInterface() {}


		// == Functions.
		// Read from the given data stream.
		virtual bool				Read( uint64_t _ui64Addr, void * _pvDest, size_t _sSize ) const = 0;

		// Does the given data stream allow insertion and deletion?  Processes return true.
		virtual bool				OverwriteOnly() const = 0;

		// Is the given data stream read-only?
		virtual bool				ReadOnly() const { return false; }

		// Insert bytes at a given address.
		virtual bool				Insert( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) = 0;

		// Overwrite bytes at a given address.
		virtual bool				Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) = 0;

		// Delets bytes at a given address.
		virtual bool				Delete( uint64_t _ui64Addr, uint64_t _ui64Size ) = 0;

		// Gets the size of the data source.
		virtual uint64_t			Size() const = 0;
	};

}	// namespace mx

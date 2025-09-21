#pragma once

#include "../MXMhsX.h"
#include "../Files\MXLargeCoWFileWindow.h"
#include "MXHexEditorInterface.h"

#include <filesystem>


namespace mx {

	class CHexEditorFile : public CHexEditorInterface {
	public :
		CHexEditorFile();
		virtual ~CHexEditorFile();


		// == Functions.
		// Sets the target file.
		bool						SetFile( const std::filesystem::path &_pPath );

		// Resets the object back to scratch.
		void						Reset();

		// Read from the given data stream.
		virtual bool				Read( uint64_t _ui64Addr, void * _pvDest, size_t _sSize ) const;

		// Does the given data stream allow insertion and deleteion?  Processes return true.
		virtual bool				OverwriteOnly() const;

		// Insert bytes at a given address.
		virtual bool				Insert( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize );

		// Overwrite bytes at a given address.
		virtual bool				Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize );

		// Delets bytes at a given address.
		virtual bool				Delete( uint64_t _ui64Addr, uint64_t _ui64Size );

		// Gets the size of the data source.
		virtual uint64_t			Size() const;


	protected :
		// == Members.
		// The copy-on-write file.
		CLargeCoWFileWindow			m_lcwfwFile;
	};

}	// namespace mx

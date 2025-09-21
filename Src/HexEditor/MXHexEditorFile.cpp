#include "MXHexEditorFile.h"


namespace mx {

	CHexEditorFile::CHexEditorFile() {
	}
	CHexEditorFile::~CHexEditorFile() {
		Reset();
	}

	// == Functions.
	// Sets the target file.
	bool CHexEditorFile::SetFile( const std::filesystem::path &_pPath ) {
		return m_lcwfwFile.OpenFile( _pPath );
	}

	// Resets the object back to scratch.
	void CHexEditorFile::Reset() {
		m_lcwfwFile.Reset();
	}

	// Read from the given data stream.
	bool CHexEditorFile::Read( uint64_t _ui64Addr, void * _pvDest, size_t _sSize ) const {
		return false;
	}

	// Does the given data stream allow insertion and deleteion?  Processes return true.
	bool CHexEditorFile::OverwriteOnly() const {
		return false;
	}

	// Insert bytes at a given address.
	bool CHexEditorFile::Insert( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) {
		return false;
	}

	// Overwrite bytes at a given address.
	bool CHexEditorFile::Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize ) {
		return false;
	}

	// Delets bytes at a given address.
	bool CHexEditorFile::Delete( uint64_t _ui64Addr, uint64_t _ui64Size ) {
		return false;
	}

	// Gets the size of the data source.
	uint64_t CHexEditorFile::Size() const {
		return m_lcwfwFile.Size();
	}

}	// namespace mx

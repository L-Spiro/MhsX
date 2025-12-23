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
	bool CHexEditorFile::Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const {
		return m_lcwfwFile.Read( _ui64Addr, _bDst, _sSize );
	}

	// Does the given data stream allow insertion and deletion?  Processes return true.
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
	bool CHexEditorFile::Delete( uint64_t _ui64Addr, uint64_t _ui64Size, uint64_t &_ui64Deleted ) {
		return m_lcwfwFile.Delete( _ui64Addr, _ui64Size, _ui64Deleted );
	}

	// Gets the size of the data source.
	uint64_t CHexEditorFile::Size() const {
		return m_lcwfwFile.Size();
	}

	// Begins an Undo sequence.  Call before making multiple delete operations in a row.
	bool CHexEditorFile::BeginUndo() {
		return m_lcwfwFile.BeginUndo();
	}

	// Ends an Undo sequence.  Call when done making modifications.
	bool CHexEditorFile::EndUndo() {
		return m_lcwfwFile.EndUndo();
	}

	// Determines if there are items to undo.
	bool CHexEditorFile::HasUndo() const {
		return m_lcwfwFile.HasUndo();
	}

	// Determines if there are items to redo.
	bool CHexEditorFile::HasRedo() const {
		return m_lcwfwFile.HasRedo();
	}

	// Performs an Undo operation.
	bool CHexEditorFile::Undo( CSecureWString &_swsStatus ) {
		return m_lcwfwFile.Undo( _swsStatus );
	}

	// Performs an Redo operation.
	bool CHexEditorFile::Redo( CSecureWString &_swsStatus ) {
		return m_lcwfwFile.Redo( _swsStatus );
	}

}	// namespace mx

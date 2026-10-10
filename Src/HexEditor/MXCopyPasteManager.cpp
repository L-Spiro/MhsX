#include "MXCopyPasteManager.h"
#include "../Files/MXLargeCoWFileWindow.h"


namespace mx {

	static_assert( size_t( CCopyPasteManager::TotalClipboards ) == size_t( CLargeCoWFileWindow::TotalClipboards ),
		"CCopyPasteManager and CLargeCoWFileWindow must agree on the number of clipboards." );
	static_assert( size_t( CCopyPasteManager::WindowsClipboard ) < size_t( CCopyPasteManager::TotalClipboards ),
		"The Windows clipboard slot must be a valid slot." );


	// == Members.
	/** The data source held by each slot. */
	std::shared_ptr<CHexEditorInterface> CCopyPasteManager::m_spSlots[CCopyPasteManager::TotalClipboards];

	/** Guards m_spSlots and m_bShutDown. */
	std::mutex CCopyPasteManager::m_mMutex;

	/** Set by ShutDown(); once set, SetSlot() fails. */
	bool CCopyPasteManager::m_bShutDown = false;


	// == Functions.
	/**
	 * \brief Registers the data source for a Copy operation into the given slot.
	 *
	 * The slot takes a reference to _spInterface and holds it until the slot is overwritten, cleared, or ShutDown() is called.  Any
	 *	interface previously held by the slot is released (and destroyed if this was its last reference).
	 *
	 * \param _sIdx The index of the slot to fill.  Must be less than TotalClipboards.
	 * \param _spInterface The interface from which the data was copied.  Must not be empty.
	 * \return Returns false if _sIdx is out of range, _spInterface is empty, or ShutDown() has already been called.
	 */
	bool CCopyPasteManager::SetSlot( size_t _sIdx, const std::shared_ptr<CHexEditorInterface> &_spInterface ) {
		if ( _sIdx >= TotalClipboards || !_spInterface ) { return false; }

		// Declared before the lock so that it is destroyed after the lock is released.
		std::shared_ptr<CHexEditorInterface> spReleaseMe;
		std::lock_guard<std::mutex> lgLock( m_mMutex );
		if ( m_bShutDown ) { return false; }
		spReleaseMe = std::move( m_spSlots[_sIdx] );
		m_spSlots[_sIdx] = _spInterface;
		return true;
	}

	/**
	 * \brief Gets the data source held by the given slot for a Paste operation.
	 *
	 * The slot is not modified.  The returned pointer is a new reference, so the interface remains alive for as long as the caller
	 *	holds it, even if the slot is overwritten or cleared in the meantime (for example, during a long streaming Paste).
	 *
	 * \param _sIdx The index of the slot to read.
	 * \return Returns the interface held by the slot, or an empty pointer if _sIdx is out of range or the slot is empty.
	 */
	std::shared_ptr<CHexEditorInterface> CCopyPasteManager::GetSlot( size_t _sIdx ) {
		if ( _sIdx >= TotalClipboards ) { return std::shared_ptr<CHexEditorInterface>(); }
		std::lock_guard<std::mutex> lgLock( m_mMutex );
		return m_spSlots[_sIdx];
	}

	/**
	 * \brief Determines whether the given slot currently holds a data source.
	 *
	 * \param _sIdx The index of the slot to check.
	 * \return Returns true if _sIdx is in range and the slot holds an interface.
	 */
	bool CCopyPasteManager::SlotIsSet( size_t _sIdx ) {
		if ( _sIdx >= TotalClipboards ) { return false; }
		std::lock_guard<std::mutex> lgLock( m_mMutex );
		return static_cast<bool>(m_spSlots[_sIdx]);
	}

	/**
	 * \brief Releases the data source held by the given slot.
	 *
	 * Intended for cases in which a slot's contents become invalid without a new Copy operation, such as another application taking
	 *	ownership of the Windows clipboard.
	 *
	 * \param _sIdx The index of the slot to clear.
	 * \return Returns true if the slot held an interface that was released.
	 */
	bool CCopyPasteManager::ClearSlot( size_t _sIdx ) {
		if ( _sIdx >= TotalClipboards ) { return false; }

		// Declared before the lock so that it is destroyed after the lock is released.
		std::shared_ptr<CHexEditorInterface> spReleaseMe;
		std::lock_guard<std::mutex> lgLock( m_mMutex );
		spReleaseMe = std::move( m_spSlots[_sIdx] );
		return static_cast<bool>(spReleaseMe);
	}

	/**
	 * \brief Releases all slots and prevents any further slots from being set.
	 *
	 * Call once as the application shuts down, before the Hex Editor and the libraries its interfaces depend upon are torn down, so
	 *	that interfaces kept alive only by this class are destroyed in an orderly manner rather than during static destruction.
	 *	Calling it more than once is harmless.
	 */
	void CCopyPasteManager::ShutDown() {
		// Declared before the lock so that the released interfaces are destroyed after the lock is released.
		std::shared_ptr<CHexEditorInterface> spReleaseMe[TotalClipboards];
		std::lock_guard<std::mutex> lgLock( m_mMutex );
		m_bShutDown = true;
		for ( size_t I = 0; I < TotalClipboards; ++I ) {
			spReleaseMe[I] = std::move( m_spSlots[I] );
		}
	}

	/**
	 * \brief Determines whether ShutDown() has been called.
	 *
	 * \return Returns true if ShutDown() has been called.
	 */
	bool CCopyPasteManager::IsShutDown() {
		std::lock_guard<std::mutex> lgLock( m_mMutex );
		return m_bShutDown;
	}

}	// namespace mx

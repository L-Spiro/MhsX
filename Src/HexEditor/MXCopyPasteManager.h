#pragma once

#include "../MXMhsX.h"
#include "MXHexEditorInterface.h"

#include <memory>
#include <mutex>


namespace mx {

	/**
	 * Class CCopyPasteManager
	 * \brief Owns references to the data sources behind each of the Copy/Paste slots.
	 *
	 * A Copy operation registers the CHexEditorInterface it copied from into one of TotalClipboards slots.  The slot keeps that
	 *	interface alive (even after its Hex Editor tab or the whole Hex Editor window has been closed) so that the copied data can be
	 *	produced later, on demand, when a Paste operation occurs.  Slot 0 (WindowsClipboard) is the slot backed by the Windows clipboard.
	 *
	 * Pasting does not remove anything from a slot, so a single Copy can be pasted any number of times.  A slot is released only when a
	 *	later Copy operation overwrites it, when ClearSlot() is called, or when ShutDown() is called as the application closes.
	 *
	 * All functions are thread-safe.  Interfaces released by this class are always destroyed outside of its lock, so an interface's
	 *	destructor may safely take time or call back into this class.
	 */
	class CCopyPasteManager {
	public :
		CCopyPasteManager() = delete;


		// == Constants.
		/**
		 * The number of Copy/Paste slots, including the Windows clipboard (index 0).  Must match CLargeCoWFileWindow::TotalClipboards.
		 */
		enum : size_t { TotalClipboards = 10 };

		/**
		 * The index of the slot that is backed by the Windows clipboard.
		 */
		enum : size_t { WindowsClipboard = 0 };


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
		static bool												SetSlot( size_t _sIdx, const std::shared_ptr<CHexEditorInterface> &_spInterface );

		/**
		 * \brief Gets the data source held by the given slot for a Paste operation.
		 *
		 * The slot is not modified.  The returned pointer is a new reference, so the interface remains alive for as long as the caller
		 *	holds it, even if the slot is overwritten or cleared in the meantime (for example, during a long streaming Paste).
		 *
		 * \param _sIdx The index of the slot to read.
		 * \return Returns the interface held by the slot, or an empty pointer if _sIdx is out of range or the slot is empty.
		 */
		static std::shared_ptr<CHexEditorInterface>				GetSlot( size_t _sIdx );

		/**
		 * \brief Determines whether the given slot currently holds a data source.
		 *
		 * \param _sIdx The index of the slot to check.
		 * \return Returns true if _sIdx is in range and the slot holds an interface.
		 */
		static bool												SlotIsSet( size_t _sIdx );

		/**
		 * \brief Releases the data source held by the given slot.
		 *
		 * Intended for cases in which a slot's contents become invalid without a new Copy operation, such as another application taking
		 *	ownership of the Windows clipboard.
		 *
		 * \param _sIdx The index of the slot to clear.
		 * \return Returns true if the slot held an interface that was released.
		 */
		static bool												ClearSlot( size_t _sIdx );

		/**
		 * \brief Releases all slots and prevents any further slots from being set.
		 *
		 * Call once as the application shuts down, before the Hex Editor and the libraries its interfaces depend upon are torn down, so
		 *	that interfaces kept alive only by this class are destroyed in an orderly manner rather than during static destruction.
		 *	Calling it more than once is harmless.
		 */
		static void												ShutDown();

		/**
		 * \brief Determines whether ShutDown() has been called.
		 *
		 * \return Returns true if ShutDown() has been called.
		 */
		static bool												IsShutDown();


	protected :
		// == Members.
		/** The data source held by each slot. */
		static std::shared_ptr<CHexEditorInterface>				m_spSlots[TotalClipboards];
		/** Guards m_spSlots and m_bShutDown. */
		static std::mutex										m_mMutex;
		/** Set by ShutDown(); once set, SetSlot() fails. */
		static bool												m_bShutDown;
	};

}	// namespace mx

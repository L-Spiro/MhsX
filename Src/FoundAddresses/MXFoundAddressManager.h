#pragma once

#include "../MXMhsX.h"
#include "../Utilities/MXStream.h"
#include "MXFoundAddress.h"
#include "MXFoundAddressGroup.h"

#include <LSONJson.h>

#include <map>
#include <memory>
#include <mutex>


namespace mx {

	class CFoundAddressManager {
	public :
		CFoundAddressManager();
		~CFoundAddressManager();


		// == Types.
		// A locking object.
		struct MX_LOCK {
			MX_LOCK( CFoundAddressManager * _pfamMan );
			~MX_LOCK();


			// == Operators.
			// Arrow operator to access manager members.
			CFoundAddressManager *											operator -> () const {
				return pfamManager;
			}
			// Dereference operator to get a reference to the manager.
			CFoundAddressManager &											operator * () const {
				return (*pfamManager);
			}


		private :
			// == Members.
			CFoundAddressManager *											pfamManager;
		};
		friend struct														MX_LOCK;


		// == Functions.
		// Gets a pointer to a found-address object given its ID.
		inline CFoundAddressBase *											GetById( size_t _sId ) {
			std::lock_guard<std::recursive_mutex> lg( m_mtxLock );
			if ( !m_i32LockCount ) { return nullptr; }
			auto aRet = m_mFoundAddresses.find( _sId );
			if ( aRet != m_mFoundAddresses.end() ) {
				return aRet->second.get();
			}
			return nullptr;
		}

		// Gets the total number of items.
		inline size_t														Total() const { return m_mFoundAddresses.size(); }

		// Removes an item by ID.
		bool																Delete( size_t _sId );

		// Removes all items.
		void																DeleteAll();

		// Sets an item as a child of another.
		bool																SetParent( size_t _sId, size_t _sParent );

		// Adds a normal Found Address.
		CFoundAddress *														AddFoundAddress( CMemHack * _pmhMemHack );

		// Adds a group.
		CFoundAddressGroup *												AddGroup( CMemHack * _pmhMemHack );

		// Unlocks all entries.
		void																UnlockAll();

		// Dirties all entries.
		void																DirtyAll();

		// Notifies all that a process has been opened.
		void																ProcessOpened();

		// Applies all timer-based locks.
		void																ApplyTimerLocks( bool _bIncludethreadLocks = false );

		// Saves all program settings.
		bool																SaveSettings( const std::wstring &_wsPath, bool _bAsJson, const std::vector<LPARAM> &_vOrder ) const;

		// Loads all program settings.
		bool																LoadSettings( const std::wstring &_wsPath, bool _bAsJson, CMemHack * _pmhMemHack, std::vector<CFoundAddressBase *> &_vLoaded );
		

	protected :
		// == Members.
		// The pool of found-address objects.
		std::map<size_t, std::unique_ptr<CFoundAddressBase>>				m_mFoundAddresses;
		// The lock critical section.
		mutable std::recursive_mutex										m_mtxLock;
		// Are we alive?
		std::atomic<bool>													m_aAlive = true;
		// Lock count.
		int32_t																m_i32LockCount = 0;


		// == Functions.
		// Locks the object for access to its data.
		CFoundAddressManager *												Lock() {
			m_mtxLock.lock();
			++m_i32LockCount;
			return this;
		}

		// Unlocks the object when done with it.
		void																UnLock() {
			--m_i32LockCount;
			m_mtxLock.unlock();
		}
	};

}	// namespace mx

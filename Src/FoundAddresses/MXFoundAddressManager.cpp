#include "MXFoundAddressManager.h"


namespace mx {

	CFoundAddressManager::MX_LOCK::MX_LOCK( CFoundAddressManager * _pfamMan ) :
		pfamManager( _pfamMan ) {
		if ( pfamManager ) {
			pfamManager->Lock();
		}
	}
	CFoundAddressManager::MX_LOCK::~MX_LOCK() {
		if ( pfamManager ) {
			pfamManager->UnLock();
			pfamManager = nullptr;
		}
	}

	CFoundAddressManager::CFoundAddressManager() {
	}
	CFoundAddressManager::~CFoundAddressManager() {
		DeleteAll();
		std::lock_guard<std::recursive_mutex> lg( m_mtxLock );
		m_aAlive = false;
	}

	// == Functions.
	// Removes an item by ID.
	bool CFoundAddressManager::Delete( size_t _sId ) {
		MX_LOCK lLock( this );
		auto pfabThis = GetById( _sId );
		if ( !pfabThis ) { return false; }

		if ( pfabThis->Parent() ) {
			auto pfabParent = GetById( pfabThis->Parent() );
			if ( pfabParent ) {
				pfabParent->RemoveChild( pfabThis );
			}
		}
		for ( auto I = pfabThis->Children().size(); I--; ) {
			pfabThis->RemoveChild( GetById( pfabThis->Children()[I] ) );
		}


		auto iTmp = m_mFoundAddresses.find( _sId );
		if ( iTmp != m_mFoundAddresses.end() ) {
			iTmp->second.reset();
			m_mFoundAddresses.erase( iTmp );
		}
		return true;
	}

	// Removes all items.
	void CFoundAddressManager::DeleteAll() {
		std::lock_guard<std::recursive_mutex> lg( m_mtxLock );
		for ( auto & I : m_mFoundAddresses ) {
			I.second.reset();
		}
		m_mFoundAddresses.clear();
	}

	// Sets an item as a child of another.
	bool CFoundAddressManager::SetParent( size_t _sId, size_t _sParent ) {
		MX_LOCK lLock( this );
		auto pfabThis = GetById( _sId );
		if ( !pfabThis ) { return false; }
		if ( pfabThis->Parent() == _sParent ) { return true; }

		auto pfabCurParent = GetById( pfabThis->Parent() );
		if ( pfabCurParent ) {
			pfabCurParent->RemoveChild( pfabThis );
		}
		if ( !_sParent ) {
			pfabThis->UnParent();
			return true;
		}

		auto pfabParent = GetById( _sParent );
		if ( !pfabParent ) { return false; }

		return pfabParent->AddChild( pfabThis );
	}

	// Adds a normal Found Address.
	CFoundAddress * CFoundAddressManager::AddFoundAddress( CMemHack * _pmhMemHack ) {
		std::lock_guard<std::recursive_mutex> lg( m_mtxLock );
		if ( !m_i32LockCount || !m_aAlive ) { return nullptr; }

		auto pfaAddress = std::make_unique<CFoundAddress>( _pmhMemHack );
		CFoundAddress * pAddress = pfaAddress.get();
		size_t sKey = pAddress->Id();
		m_mFoundAddresses.emplace( sKey, std::move( pfaAddress ) );
		return pAddress;
	}

	// Adds a group.
	CFoundAddressGroup * CFoundAddressManager::AddGroup( CMemHack * _pmhMemHack ) {
		std::lock_guard<std::recursive_mutex> lg( m_mtxLock );
		if ( !m_i32LockCount || !m_aAlive ) { return nullptr; }

		auto pfaAddress = std::make_unique<CFoundAddressGroup>( _pmhMemHack );
		CFoundAddressGroup * pAddress = pfaAddress.get();
		size_t sKey = pAddress->Id();
		m_mFoundAddresses.emplace( sKey, std::move( pfaAddress ) );
		return pAddress;
	}

	// Dirties all entries.
	void CFoundAddressManager::DirtyAll() {
		std::lock_guard<std::recursive_mutex> lg( m_mtxLock );
		for ( auto & I : m_mFoundAddresses ) {
			if MX_LIKELY( I.second ) {
				I.second->Dirty();
			}
		}
	}

}	// namespace mx

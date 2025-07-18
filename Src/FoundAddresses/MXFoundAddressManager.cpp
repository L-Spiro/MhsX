#include "MXFoundAddressManager.h"


namespace mx {

	CFoundAddressManager::CFoundAddressManager() {
	}
	CFoundAddressManager::~CFoundAddressManager() {
		DeleteAll();
	}

	// == Functions.
	// Removes an item by ID.
	bool CFoundAddressManager::Delete( size_t _sId ) {
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
		for ( auto & I : m_mFoundAddresses ) {
			I.second.reset();
		}
		m_mFoundAddresses.clear();
	}

	// Sets an item as a child of another.
	bool CFoundAddressManager::SetParent( size_t _sId, size_t _sParent ) {
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
		auto pfaAddress = std::make_unique<CFoundAddress>( _pmhMemHack );
		CFoundAddress * pAddress = pfaAddress.get();
		size_t sKey = pAddress->Id();
		m_mFoundAddresses.emplace( sKey, std::move( pfaAddress ) );
		return pAddress;
	}

	// Adds a group.
	CFoundAddressGroup * CFoundAddressManager::AddGroup( CMemHack * _pmhMemHack ) {
		auto pfaAddress = std::make_unique<CFoundAddressGroup>( _pmhMemHack );
		CFoundAddressGroup * pAddress = pfaAddress.get();
		size_t sKey = pAddress->Id();
		m_mFoundAddresses.emplace( sKey, std::move( pfaAddress ) );
		return pAddress;
	}

	// Dirties all entries.
	void CFoundAddressManager::DirtyAll() {
		for ( auto & I : m_mFoundAddresses ) {
			if MX_LIKELY( I.second ) {
				I.second->Dirty();
			}
		}
	}

}	// namespace mx

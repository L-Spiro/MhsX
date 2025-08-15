#include "MXFoundAddressManager.h"
#include "MXFoundAddress.h"


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
		m_mFoundAddresses = std::map<size_t, std::unique_ptr<CFoundAddressBase>>();
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
	 // Unlocks all entries.
	void CFoundAddressManager::UnlockAll() {
		std::lock_guard<std::recursive_mutex> lg( m_mtxLock );
		for ( auto & I : m_mFoundAddresses ) {
			if MX_LIKELY( I.second && I.second->Type() == MX_FAT_FOUND_ADDRESS ) {
				reinterpret_cast<CFoundAddress *>(I.second.get())->SetLocked( false );
			}
		}
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

	// Applies all timer-based locks.
	void CFoundAddressManager::ApplyTimerLocks( bool /*_bIncludethreadLocks*/ ) {
		std::lock_guard<std::recursive_mutex> lg( m_mtxLock );
		for ( auto & I : m_mFoundAddresses ) {
			if MX_LIKELY( I.second && I.second->Type() == MX_FAT_FOUND_ADDRESS ) {
				reinterpret_cast<CFoundAddress *>(I.second.get())->ApplyLock();
			}
		}
	}

	// Saves all program settings.
	bool CFoundAddressManager::SaveSettings( const std::wstring &_wsPath, bool _bAsJson, const std::vector<LPARAM> &_vOrder ) const {
		std::vector<uint8_t> vBuffer;
		std::lock_guard<std::recursive_mutex> lg( m_mtxLock );

		if ( _bAsJson ) {
			lson::CJson::LSON_ELEMENT eRoot;
			lson::CJson::CreateObjectElement( "", eRoot );
			MX_JSON_NUMBER( _DEC_S_70A1EA5F_Version, MX_FOUND_ADDRESS_VERSION, &eRoot );

			eRoot.vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
			lson::CJson::CreateArrayElement( _DEC_S_20DFC649_Items, (*eRoot.vObjectMembers[eRoot.vObjectMembers.size()-1]) );
			{
				lson::CJson::LSON_ELEMENT * peArray = eRoot.vObjectMembers[eRoot.vObjectMembers.size()-1].get();
				for ( size_t I = 0; I < _vOrder.size(); ++I ) {
					auto pfabThis = const_cast<CFoundAddressManager *>(this)->GetById( _vOrder[I] );
					if ( pfabThis ) {
						peArray->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
						lson::CJson::CreateObjectElement( _DEC_S_BF298A20_Item, (*peArray->vObjectMembers[peArray->vObjectMembers.size()-1]) );
						auto peParent = peArray->vObjectMembers[peArray->vObjectMembers.size()-1].get();

						MX_JSON_NUMBER( _DEC_S_2CECF817_Type, pfabThis->Type(), peParent );
						if ( !pfabThis->SaveSettings( peParent, nullptr ) ) { return false; }
					}
				}
			}

			if ( !lson::CJson::WriteElement( eRoot, vBuffer, 0 ) ) { return false; }
		}
		else {
			
			CStream sStream( vBuffer );
			constexpr uint32_t ui32Version = MX_FOUND_ADDRESS_VERSION;
			if ( !sStream.Write( ui32Version ) ) { return false; }

			if ( !sStream.Write( _vOrder.size() ) ) { return false; }
			for ( size_t I = 0; I < _vOrder.size(); ++I ) {
				auto pfabThis = const_cast<CFoundAddressManager *>(this)->GetById( _vOrder[I] );
				if ( pfabThis ) { return false; }
				if ( !sStream.Write( pfabThis->Type() ) ) { return false; }
				if ( !pfabThis->SaveSettings( nullptr, &sStream ) ) { return false; }
			}
		}
		
		CFile fFile;
		if ( !fFile.CreateNewFile( _wsPath.c_str(), FALSE ) ) { return false; }
		if ( !fFile.Write( vBuffer.data(), static_cast<DWORD>(vBuffer.size()) ) ) { return false; }
		
		return true;
	}

	// Loads all program settings.
	bool CFoundAddressManager::LoadSettings( const std::wstring &_wsPath, bool _bAsJson ) {
		std::vector<uint8_t> vBuffer;
		CFile fFile;
		fFile.LoadToMemory( _wsPath.c_str(), vBuffer );
		if ( vBuffer.size() == 0 ) { return false; }
		if ( _bAsJson ) {
			lson::CJson jSon;
			vBuffer.push_back( 0 );

			if ( !jSon.SetJson( reinterpret_cast<const char *>(vBuffer.data()) ) ) { return false; }
			/*if ( !m_pmhMemHack->LoadSettings( &jSon, nullptr, oOptions ) ) { return false; }
			if ( !m_pmhMemHack->HotkeyManager().LoadSettings( &jSon, nullptr ) ) { return false; }*/
		}
		else {
			CStream sStream( vBuffer );
			/*if ( !m_pmhMemHack->LoadSettings( nullptr, &sStream, oOptions ) ) { return false; }
			if ( !m_pmhMemHack->HotkeyManager().LoadSettings( nullptr, &sStream ) ) { return false; }*/
		}

		return true;
	}

}	// namespace mx

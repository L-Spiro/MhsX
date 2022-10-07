#include "MXHotkeyRegisterHotKeyMethod.h"
#include "../System/MXSystem.h"


namespace mx {

	CHotkeyRegisterHotKeyMethod::CHotkeyRegisterHotKeyMethod() :
		m_hWnd( NULL ) {
	}
	CHotkeyRegisterHotKeyMethod::~CHotkeyRegisterHotKeyMethod() {
		Reset();
	}

	// == Functions.
	// Handle WM_HOTKEY messages.
	size_t CHotkeyRegisterHotKeyMethod::HandleWM_HOTKEY( INT _iId, INT _iCode, INT _iMod ) {
		MX_HOTKEY_LOCK hlLock( this );
		size_t sCnt = 0;
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csHandlerCrit );
		for ( size_t I = m_vHotkeys.size(); I--; ) {
			if ( m_vHotkeys[I].iIdentifier == _iId ) {
				ExecuteByindex( I );
				++sCnt;
			}
		}
		return sCnt;
	}

	// Adds a key and returns the ID or 0.
	LONG CHotkeyRegisterHotKeyMethod::AddKey( INT _iCode, INT _iMod, UINT _uiScanCode, uint32_t _ui32Function,
		uint64_t _ui64Parm0, uint64_t _ui64Parm1, uint64_t _ui64Parm2,
		std::string &_sError ) {
		if ( HasKey( _iCode, _iMod ) ) {
			_sError = _DEC_S_EF7346DB_The____Hotkey____method_does_not_allow_duplicate_hotkeys_.c_str();
			return 0;
		}
		return Parent::AddKey( _iCode, _iMod, _uiScanCode, _ui32Function, _ui64Parm0, _ui64Parm1, _ui64Parm2, _sError );
	}

	// Begins polling or registers the hotkeys.
	bool CHotkeyRegisterHotKeyMethod::Start() {
		MX_HOTKEY_LOCK hlLock( this );
		if ( IncStartStopCount() == 1 ) {
			return RegisterAll();
		}
		return true;
	}

	// Stops polling or unregisters the hotkeys.
	bool CHotkeyRegisterHotKeyMethod::Stop() {
		MX_HOTKEY_LOCK hlLock( this );
		if ( DecStartStopCount() == 0 ) {
			UnregisterAll();
		}
		return true;
	}

	// Removes all hotkeys.
	void CHotkeyRegisterHotKeyMethod::Clear() {
		MX_HOTKEY_LOCK hlLock( this );
		UnregisterAll();
		Parent::Clear();
	}

	// Copies the object.
	bool CHotkeyRegisterHotKeyMethod::Copy( const CHotkeyManBase &_hmbOther ) {
		MX_HOTKEY_LOCK hlLock( this );
		UnregisterAll();

		return Parent::Copy( _hmbOther );
	}

	// Returns a boolean indicating whether the given key and modifier exist.
	bool CHotkeyRegisterHotKeyMethod::HasKey( INT _iCode, INT _iMod ) const {
		MX_HOTKEY_LOCK hlLock( this );
		_iMod = VirtualKeyToModifier( _iMod );
		for ( size_t I = m_vHotkeys.size(); I--; ) {
			if ( m_vHotkeys[I].iCode == _iCode && VirtualKeyToModifier( m_vHotkeys[I].iMod ) == _iMod ) { return true; }
		}
		return false;
	}

	// Registers all of the hotkeys.
	bool CHotkeyRegisterHotKeyMethod::RegisterAll() {
		MX_HOTKEY_LOCK hlLock( this );
		if ( m_hWnd == NULL ) { return false; }
		for ( size_t I = 0; I < m_vHotkeys.size(); ++I ) {
			CSystem::RegisterHotKey( m_hWnd, m_vHotkeys[I].iIdentifier, VirtualKeyToModifier( m_vHotkeys[I].iMod ), m_vHotkeys[I].iCode );
		}

		return true;
	}

	// Unregisters all of the hotkeys.
	void CHotkeyRegisterHotKeyMethod::UnregisterAll() {
		MX_HOTKEY_LOCK hlLock( this );
		if ( m_hWnd != NULL ) {
			for ( size_t I = 0; I < m_vHotkeys.size(); ++I ) {
				CSystem::UnregisterHotKey( m_hWnd, m_vHotkeys[I].iIdentifier );
			}
		}
		m_lStartStopCount = 0;
	}

	// Converts a virtual key to a ::RegisterHotKey() modifier.
	UINT CHotkeyRegisterHotKeyMethod::VirtualKeyToModifier( UINT _uiKey ) {
		switch ( _uiKey ) {
			case VK_MENU : {}
			case VK_LMENU : {}
			case VK_RMENU : { return MOD_ALT; }
			case VK_CONTROL : {}
			case VK_LCONTROL : {}
			case VK_RCONTROL : { return MOD_CONTROL; }
			case VK_SHIFT : {}
			case VK_LSHIFT : {}
			case VK_RSHIFT : { return MOD_SHIFT; }
			case VK_LWIN : {}
			case VK_RWIN : { return MOD_WIN; }
		}
		return 0;
	}

}	// namespace mx

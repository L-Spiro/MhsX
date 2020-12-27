#include "MXHotkeyWhKeyboardMethod.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../System/MXSystem.h"
#include <Base/LSWBase.h>


namespace mx {

	// The point to the last instance of this class to create a hook procedure.
	CHotkeyWhKeyboardMethod * CHotkeyWhKeyboardMethod::m_phwkmHotkeyMan = nullptr;

	CHotkeyWhKeyboardMethod::CHotkeyWhKeyboardMethod() :
		m_hWnd( NULL ),
		m_hHook( NULL ) {
	}
	CHotkeyWhKeyboardMethod::~CHotkeyWhKeyboardMethod() {
		while ( m_hHook ) { Stop(); }
	}

	// == Functions.
	// Begins polling or registers the hotkeys.
	bool CHotkeyWhKeyboardMethod::Start() {
		MX_HOTKEY_LOCK hlLock( this );
		while ( m_hHook ) { Stop(); }
		if ( IncStartStopCount() == 1 ) {
			if ( !TotalHotkeys() ) { return true; }
			m_hHook = CSystem::SetWindowsHookExW( WH_KEYBOARD, CHotkeyWhKeyboardMethod::KeyboardProc, NULL, ::GetCurrentThreadId() );
			if ( m_hHook != NULL ) {
				m_phwkmHotkeyMan = this;
			}
		}
		return true;
	}

	// Stops polling or unregisters the hotkeys.
	bool CHotkeyWhKeyboardMethod::Stop() {
		MX_HOTKEY_LOCK hlLock( this );
		if ( DecStartStopCount() == 0 ) {
			if ( m_hHook ) {
				CSystem::UnhookWindowsHookEx( m_hHook );
				m_hHook = NULL;
				m_phwkmHotkeyMan = nullptr;
			}
		}
		return true;
	}

	// Copies the object.
	bool CHotkeyWhKeyboardMethod::Copy( const CHotkeyManBase &_hmbOther ) {
		MX_HOTKEY_LOCK hlLock( this );
		while ( m_hHook ) { Stop(); }
		return Parent::Copy( _hmbOther );
	}

	// The hook procedure.
	LRESULT CALLBACK CHotkeyWhKeyboardMethod::KeyboardProc( int _iCode, WPARAM _wParam, LPARAM _lParam ) {
		MX_HOTKEY_LOCK hlLock( m_phwkmHotkeyMan );
		if ( _iCode < 0 ) { return ::CallNextHookEx( m_phwkmHotkeyMan->m_hHook, _iCode, _wParam, _lParam ); }
		WORD wProps = HIWORD( _lParam );
		/*WORD wRepCnt = LOWORD( _lParam );
		char szBuffer[64];
		sprintf( szBuffer, "%.8X\r\n", static_cast<uint32_t>(_lParam) );*/
		
		if ( !(wProps & KF_UP) /*&& (wProps & KF_REPEAT)*/ ) {
			lsw::CCriticalSection::CEnterCrit ecCrit( m_csHandlerCrit );
			for ( size_t I = 0; I < m_phwkmHotkeyMan->m_vHotkeys.size(); ++I ) {
				if ( m_phwkmHotkeyMan->m_vHotkeys[I].iCode == _wParam ) {
					//::OutputDebugStringA( szBuffer );
					bool bSuccess = m_phwkmHotkeyMan->m_vHotkeys[I].iMod == 0;
					if ( !bSuccess ) {
						SHORT sKey = CSystem::GetAsyncKeyState( m_phwkmHotkeyMan->m_vHotkeys[I].iMod );
						bSuccess = sKey & 0x8000;
					}
					if ( bSuccess ) {
						if ( !m_phwkmHotkeyMan->m_hWnd ) {
							m_phwkmHotkeyMan->ExecuteByindex( I );
						}
						else {
							::SendNotifyMessageW( m_phwkmHotkeyMan->m_hWnd, CMhsMainWindow::MX_CM_HOOKKEY, m_phwkmHotkeyMan->Get( I ).iIdentifier, 0 );
						}
					}
				}
			}
		}
		return ::CallNextHookEx( m_phwkmHotkeyMan->m_hHook, _iCode, _wParam, _lParam );
	}

}	// namespace mx

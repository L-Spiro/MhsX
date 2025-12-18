#include "MXHotkeyPollingMethod.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../System/MXSystem.h"


namespace mx {

	CHotkeyPollingMethod::CHotkeyPollingMethod() :
		m_lCancelThread( FALSE ),
		m_hWnd( NULL ) {
	}
	CHotkeyPollingMethod::~CHotkeyPollingMethod() {
		StopPollingThreadAndWait();
	}

	// == Functions.
	// Begins polling or registers the hotkeys.
	bool CHotkeyPollingMethod::Start() {
		return StartPollingThread();
	}

	// Stops polling or unregisters the hotkeys.
	bool CHotkeyPollingMethod::Stop() {
		return StopPollingThreadAndWait();
	}

	// Starts the polling thread.
	bool CHotkeyPollingMethod::StartPollingThread() {
		MX_HOTKEY_LOCK hlLock( this );
		StopPollingThreadAndWait();
		if ( !TotalHotkeys() ) { return true; }
		m_lCancelThread = TRUE;
		if ( !m_tThread.CreateThread( ThreadProc, this, &m_lCancelThread ) ) {
			m_lCancelThread = FALSE;
			return false;
		}
		return true;
	}

	// Stops the polling thread and waits.
	bool CHotkeyPollingMethod::StopPollingThreadAndWait() {
		if ( m_lCancelThread == TRUE ) {
			m_tThread.StopAndWait( FALSE );
		}
		return true;
	}

	// Performs a single poll.
	bool CHotkeyPollingMethod::Poll() {
		MX_HOTKEY_LOCK hlLock( this );

		if ( !m_vHotkeys.size() ) { return true; }
		SHORT sKeyState[256];
		std::memset( sKeyState, -1, sizeof( sKeyState ) );
#define MX_KEY( IDX )	((sKeyState[IDX] == -1) ? (sKeyState[IDX] = CSystem::GetAsyncKeyState( IDX )) : sKeyState[IDX])

		lsw::CCriticalSection::CEnterCrit ecCrit( m_csHandlerCrit );
		for ( size_t I = 0; I < m_vHotkeys.size(); ++I ) {
			if ( m_vHotkeys[I].iCode < std::size( sKeyState ) ) {
				SHORT sKey = MX_KEY( m_vHotkeys[I].iCode );
				bool bJustHit = (sKey & 0x8001) == 0x8001;
				if ( bJustHit ) {
					bool bSuccess = false;
					if ( m_vHotkeys[I].iMod ) {
						if ( m_vHotkeys[I].iMod < std::size( sKeyState ) ) {
							sKey = MX_KEY( m_vHotkeys[I].iMod );
							bSuccess = sKey & 0x8000;
						}
					}
					else {
						bSuccess = true;
					}
					if ( bSuccess ) {
						if ( m_hWnd == NULL ) {
							ExecuteByindex( I );
						}
						else {
							::SendNotifyMessageW( m_hWnd, CMhsMainWindow::MX_CM_HOTKEY, m_vHotkeys[I].iIdentifier, 0 );
						}
					}
				}
			}
#undef MX_KEY
		}
		return true;
	}

	// The thread procedure.
	DWORD WINAPI CHotkeyPollingMethod::ThreadProc( LPVOID _lpParameter ) {
		CHotkeyPollingMethod * phpmHotkey = reinterpret_cast<CHotkeyPollingMethod *>(_lpParameter);
		lsw::LSW_THREAD_PRIORITY tpTreadPriority( THREAD_PRIORITY_BELOW_NORMAL );
		while ( phpmHotkey->m_lCancelThread == TRUE ) {
			phpmHotkey->Poll();
			::Sleep( 1 );
		}
		return 0;
	}

	// Copies the object.
	bool CHotkeyPollingMethod::Copy( const CHotkeyManBase &_hmbOther ) {
		MX_HOTKEY_LOCK hlLock( this );
		StopPollingThreadAndWait();

		return Parent::Copy( _hmbOther );
	}

}	// namespace mx

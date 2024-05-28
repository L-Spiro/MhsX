#include "LSWInputListenerBase.h"


namespace lsw {

	// == Members.
	/** Window property. */
	WCHAR CInputListenerBase::m_szProp[2] = { 0 };

	CInputListenerBase::CInputListenerBase() :
		m_lsListenState( LSW_LS_WAITING ),
		m_wpOrigProc( nullptr ) {
		if ( !m_szProp[0] ) {
			m_szProp[0] = L'H' + ((reinterpret_cast<UINT_PTR>(this) >> 2) & 0x0F);
		}

		m_kResult.bKeyCode = m_kResult.bKeyModifier = 0;
		m_kResult.dwScanCode = 0;
	}

	// == Functions.
	/**
	 * Starts listening for keyboard input.
	 *
	 * \param _pwControl The control that will be listening for keyboard input.
	 * \return Returns true if beginning the keyboard isten was successful.
	 */
	bool CInputListenerBase::BeginListening_Keyboard( CWidget * _pwControl ) {
		m_kResult.bKeyCode = m_kResult.bKeyModifier = 0;
		m_kResult.dwScanCode = 0;

		if ( !_pwControl ) { return false; }
		if ( m_lsListenState != LSW_LS_LISTENING ) {
			::SetPropW( _pwControl->Wnd(), m_szProp, reinterpret_cast<HANDLE>(this) );
			m_wpOrigProc = CHelpers::SetWndProc( _pwControl->Wnd(), ControlOverride );
		}
		m_lsListenState = LSW_LS_LISTENING;
		_pwControl->SetTextW( ListeningText() );
		_pwControl->SetFocus();
		return true;
	}

	/**
	 * Called after focus on the control is lost or a key has been registered.
	 *
	 * \param _pwControl The control used for listening.
	 * \param _bSuccess Set to true if the call is due to a key being registered.
	 * \return Returns true if the listening state is LSW_LS_LISTENING and the old control procedure was restored.
	 */
	bool CInputListenerBase::StopListening_Keyboard( CWidget * _pwControl, bool _bSuccess ) {
		if ( m_lsListenState != LSW_LS_LISTENING ) { return false; }
		m_lsListenState = _bSuccess ? LSW_LS_SUCCESS : LSW_LS_FAILURE;
		if ( !_bSuccess ) {
			m_kResult.bKeyCode = m_kResult.bKeyModifier = 0;
			m_kResult.dwScanCode = 0;
		}

		if ( !_pwControl ) { return false; }
		CHelpers::SetWndProc( _pwControl->Wnd(), m_wpOrigProc );
		if ( !_bSuccess ) {
			_pwControl->SetTextW( DefaultText() );
		}

		return true;
	}

	/**
	 * Window procedure.
	 *
	 * \param _hWnd The window parameter.
	 * \param _uMsg The message parameter.
	 * \param _wParam The WPARAM.
	 * \param _lParam The LPARAM.
	 * \return Returns the result of the message-handler.
	 */
	LRESULT CALLBACK CInputListenerBase::ControlOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CWidget * pwThis = LSW_WIN2CLASS( _hWnd );
		CInputListenerBase * pophThis = reinterpret_cast<CInputListenerBase *>(::GetPropW( _hWnd, m_szProp ));
		WNDPROC wpOrig = pophThis->m_wpOrigProc;
		switch ( _uMsg ) {
			case WM_GETDLGCODE : { return DLGC_WANTALLKEYS; }
			case WM_KILLFOCUS : {
				pophThis->StopListening_Keyboard( pwThis, false );
				break;
			}
			case WM_KEYDOWN : {
				if ( static_cast<BYTE>(_wParam) == VK_LWIN || static_cast<BYTE>(_wParam) == VK_RWIN ) { return 0; }

				bool bIgnoreLeftRight = pophThis->IgnoringLeftRight();
				bool bExtended = (_lParam & (KF_EXTENDED << 16));
				if ( static_cast<BYTE>(_wParam) == VK_SHIFT ) {
					std::wstring wTmp;
					if ( !pophThis->AllowKeyCombos() ) {
					}
					else {
						pophThis->m_kResult.bKeyModifier = BYTE( ::MapVirtualKeyW( (_lParam >> 16) & 0xFF, MAPVK_VSC_TO_VK_EX ) );
						CHelpers::ModifierToString( pophThis->m_kResult.bKeyModifier, wTmp, bIgnoreLeftRight );
						wTmp += L"\u2026";	// "...".
						::SetWindowTextW( _hWnd, wTmp.c_str() );
						return 0;
					}
				}
				if ( static_cast<BYTE>(_wParam) == VK_CONTROL ) {
					if ( !pophThis->AllowKeyCombos() ) {
					}
					else {
						UINT uiTemp = ::MapVirtualKeyW( VK_CONTROL, MAPVK_VK_TO_VSC );
						std::wstring wTmp = CHelpers::ScanCodeToString( bIgnoreLeftRight ? (uiTemp << 16) : UINT( _lParam ) );
						pophThis->m_kResult.bKeyModifier = bExtended ? VK_RCONTROL : VK_LCONTROL;
						wTmp += L"\u2026";	// "...".
						::SetWindowTextW( _hWnd, wTmp.c_str() );
						return 0;
					}
				}
				if ( static_cast<BYTE>(_wParam) == VK_MENU && pophThis->AllowKeyCombos() ) {
					if ( !pophThis->AllowKeyCombos() ) {
					}
					else {
						UINT uiTemp = ::MapVirtualKeyW( VK_MENU, MAPVK_VK_TO_VSC );
						std::wstring wTmp = CHelpers::ScanCodeToString( bIgnoreLeftRight ? (uiTemp << 16) : UINT( _lParam ) );
						pophThis->m_kResult.bKeyModifier = bExtended ? VK_RMENU : VK_LMENU;
						wTmp += L"\u2026";	// "...".
						::SetWindowTextW( _hWnd, wTmp.c_str() );
						return 0;
					}
				}
				std::wstring wTmp;
				if ( pophThis->m_kResult.bKeyModifier ) {
					CHelpers::ModifierToString( pophThis->m_kResult.bKeyModifier, wTmp, bIgnoreLeftRight );
					wTmp += L"+";
				}
				wTmp += CHelpers::ScanCodeToString( UINT( _lParam ) );
				
				::SetWindowTextW( _hWnd, wTmp.c_str() );
				pophThis->m_kResult.bKeyCode = static_cast<BYTE>(_wParam);
				pophThis->m_kResult.dwScanCode = static_cast<DWORD>(_lParam);

				pophThis->StopListening_Keyboard( pwThis, true );
				return 0;
			}
			case WM_SYSKEYDOWN : {
				if ( static_cast<BYTE>(_wParam) == VK_LWIN || static_cast<BYTE>(_wParam) == VK_RWIN || static_cast<BYTE>(_wParam) == VK_SHIFT || static_cast<BYTE>(_wParam) == VK_CONTROL ) { return 0; }

				bool bIgnoreLeftRight = pophThis->IgnoringLeftRight();
				bool bExtended = (_lParam & (KF_EXTENDED << 16));

				std::wstring wTmp;
				if ( static_cast<BYTE>(_wParam) == VK_MENU ) {
					if ( !pophThis->AllowKeyCombos() ) {
					}
					else {
						pophThis->m_kResult.bKeyModifier = bExtended ? VK_RMENU : VK_LMENU;
						CHelpers::ModifierToString( pophThis->m_kResult.bKeyModifier, wTmp, bIgnoreLeftRight );
						wTmp += L"\u2026";	// "...".
						::SetWindowTextW( _hWnd, wTmp.c_str() );
						return 0;
					}
				}
				
				if ( pophThis->m_kResult.bKeyModifier ) {
					CHelpers::ModifierToString( pophThis->m_kResult.bKeyModifier, wTmp, bIgnoreLeftRight );
					wTmp += L"+";
				}
				wTmp += CHelpers::ScanCodeToString( UINT( _lParam ) );
				
				::SetWindowTextW( _hWnd, wTmp.c_str() );
				pophThis->m_kResult.bKeyCode = static_cast<BYTE>(_wParam);
				pophThis->m_kResult.dwScanCode = static_cast<DWORD>(_lParam);

				pophThis->StopListening_Keyboard( pwThis, true );
				return 0;
			}
		}
		return ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
	}

}	// namespace lsw

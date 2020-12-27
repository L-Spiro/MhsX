#include "MXHotkeyManager.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"
#include <map>


namespace mx {

	// The unique ID of each hotkey.
	LONG CHotkeyManager::m_lHotkeyId = 0x1000;

	CHotkeyManager::CHotkeyManager() {
	}
	CHotkeyManager::~CHotkeyManager() {
		Reset();
	}


	// == Functions.
	// Adds a hotkey.
	bool CHotkeyManager::AddHotkey( INT _iId, INT _iCode, INT _iMod, UINT _uiScanCode, uint32_t _ui32Method, uint32_t _ui32Function,
		uint64_t _ui64Parm0, uint64_t _ui64Parm1, uint64_t _ui64Parm2, std::string &_sError ) {
		MX_HOTKEY_LOCK hlLock( this );

		switch ( _ui32Method ) {
			case CHotkeyManager::MX_M_POLLING : {
				LONG lRes = m_hpmPollingHotkeys.AddKey( _iCode, _iMod, _uiScanCode, _ui32Function,
					_ui64Parm0, _ui64Parm1, _ui64Parm2, _sError );
				return lRes != 0;
			}
			case CHotkeyManager::MX_M_HOOK : {
				LONG lRes = m_hwkmWhKeyboardMethod.AddKey( _iCode, _iMod, _uiScanCode, _ui32Function,
					_ui64Parm0, _ui64Parm1, _ui64Parm2, _sError );
				return lRes != 0;
			}
			case CHotkeyManager::MX_M_HOTKEY : {
				LONG lRes = m_hrhkmRegisteredHotkeys.AddKey( _iCode, _iMod, _uiScanCode, _ui32Function,
					_ui64Parm0, _ui64Parm1, _ui64Parm2, _sError );
				return lRes != 0;
			}
		}

		return false;
	}

	// Deletes a hotkey based off its LPARAM (a combination of its ID and method).
	bool CHotkeyManager::DeleteByLParam( LPARAM _lpDeleteMe ) {
		MX_HOTKEY_LOCK hlLock( this );
		INT iMethod = static_cast<UINT_PTR>(_lpDeleteMe) >> MX_H_KEY_LPARAM_METHOD_SHIFT;
		UINT uiId = _lpDeleteMe & MX_H_KEY_LPARAM_ID_MASK;
		switch ( iMethod ) {
			case CHotkeyManager::MX_M_POLLING : {
				return m_hpmPollingHotkeys.DeleteById( uiId );
			}
			case CHotkeyManager::MX_M_HOOK : {
				return m_hwkmWhKeyboardMethod.DeleteById( uiId );
			}
			case CHotkeyManager::MX_M_HOTKEY : {
				return m_hrhkmRegisteredHotkeys.DeleteById( uiId );
			}
		}
		return false;
	}

	// Moves all hotkeys up from an array of hotkeys identified by their LPARAM values.
	bool CHotkeyManager::MoveUp( const std::vector<LPARAM> &_vMoveUs ) {
		const INT iMethods[] = {
			CHotkeyManager::MX_M_POLLING,
			CHotkeyManager::MX_M_HOOK,
			CHotkeyManager::MX_M_HOTKEY
		};
		CHotkeyManBase * phmbHotkeys[] = {
			&m_hpmPollingHotkeys,
			&m_hwkmWhKeyboardMethod,
			&m_hrhkmRegisteredHotkeys
		};
		for ( size_t J = 0; J < MX_ELEMENTS( iMethods ); ++J ) {
			if ( !phmbHotkeys[J] ) { continue; }
			// Separate them out by hotkey type.
			std::set<INT> sSingleListMovers;
			for ( size_t I = 0; I < _vMoveUs.size(); ++I ) {
				INT iMethod = static_cast<UINT_PTR>(_vMoveUs[I]) >> MX_H_KEY_LPARAM_METHOD_SHIFT;
				if ( iMethod == iMethods[J] ) {
					INT iId = _vMoveUs[I] & MX_H_KEY_LPARAM_ID_MASK;
					sSingleListMovers.insert( iId );
				}
			}
			phmbHotkeys[J]->MoveUp( sSingleListMovers );
		}
		return true;
	}

	// Moves all hotkeys down from an array of hotkeys identified by their LPARAM values.
	bool CHotkeyManager::MoveDown( const std::vector<LPARAM> &_vMoveUs ) {
		const INT iMethods[] = {
			CHotkeyManager::MX_M_POLLING,
			CHotkeyManager::MX_M_HOOK,
			CHotkeyManager::MX_M_HOTKEY
		};
		CHotkeyManBase * phmbHotkeys[] = {
			&m_hpmPollingHotkeys,
			&m_hwkmWhKeyboardMethod,
			&m_hrhkmRegisteredHotkeys
		};
		for ( size_t J = 0; J < MX_ELEMENTS( iMethods ); ++J ) {
			if ( !phmbHotkeys[J] ) { continue; }
			// Separate them out by hotkey type.
			std::set<INT> sSingleListMovers;
			for ( size_t I = 0; I < _vMoveUs.size(); ++I ) {
				INT iMethod = static_cast<UINT_PTR>(_vMoveUs[I]) >> MX_H_KEY_LPARAM_METHOD_SHIFT;
				if ( iMethod == iMethods[J] ) {
					INT iId = _vMoveUs[I] & MX_H_KEY_LPARAM_ID_MASK;
					sSingleListMovers.insert( iId );
				}
			}
			phmbHotkeys[J]->MoveDown( sSingleListMovers );
		}
		return true;
	}

	// Handle WM_HOTKEY messages.
	bool CHotkeyManager::HandleWM_HOTKEY( INT _iId, INT _iCode, INT _iMod ) {
		MX_HOTKEY_LOCK hlLock( this );
		return m_hrhkmRegisteredHotkeys.HandleWM_HOTKEY( _iId, _iCode, _iMod );
	}

	// Handle a poll hotkey.
	bool CHotkeyManager::HandlePollHotkey( INT _iId ) {
		MX_HOTKEY_LOCK hlLock( this );
		for ( auto I = m_hpmPollingHotkeys.TotalHotkeys(); I--; ) {
			if ( m_hpmPollingHotkeys.Get( I ).iIdentifier == _iId ) {
				m_hpmPollingHotkeys.ExecuteByindex( I );
				return true;
			}
		}
		return false;
	}

	// Handle a WH_KEYBOARD hooked hotkey.
	bool CHotkeyManager::HandleHookHotkey( INT _iId ) {
		MX_HOTKEY_LOCK hlLock( this );
		for ( auto I = m_hwkmWhKeyboardMethod.TotalHotkeys(); I--; ) {
			if ( m_hwkmWhKeyboardMethod.Get( I ).iIdentifier == _iId ) {
				m_hwkmWhKeyboardMethod.ExecuteByindex( I );
				return true;
			}
		}
		return false;
	}

	// Determines if the given key combination already exists.
	bool CHotkeyManager::HaveHotkeyAlready( INT _iCode, INT _iMod ) const {
		MX_HOTKEY_LOCK hlLock( this );
		return m_hpmPollingHotkeys.HasKey( _iCode, _iMod ) || m_hwkmWhKeyboardMethod.HasKey( _iCode, _iMod ) || m_hrhkmRegisteredHotkeys.HasKey( _iCode, _iMod );
	}

	// Gets a hotkey based off its ID.
	const CHotkeyManBase::MX_HOTKEY_INSTANCE * CHotkeyManager::GetById( INT _iId ) const {
		const CHotkeyManBase::MX_HOTKEY_INSTANCE * phiInt = nullptr;
		if ( phiInt = m_hpmPollingHotkeys.GetById( _iId ) ) { return phiInt; }
		if ( phiInt = m_hwkmWhKeyboardMethod.GetById( _iId ) ) { return phiInt; }
		if ( phiInt = m_hrhkmRegisteredHotkeys.GetById( _iId ) ) { return phiInt; }
		return phiInt;
	}

	// Creates a copy of the hotkey manager.
	bool CHotkeyManager::Copy( const CHotkeyManager &_hmSrc ) {
		MX_HOTKEY_LOCK hlLock( this );
		Reset();
		if ( !m_hpmPollingHotkeys.Copy( _hmSrc.m_hpmPollingHotkeys ) ) { return false; }
		if ( !m_hwkmWhKeyboardMethod.Copy( _hmSrc.m_hwkmWhKeyboardMethod ) ) { return false; }
		if ( !m_hrhkmRegisteredHotkeys.Copy( _hmSrc.m_hrhkmRegisteredHotkeys ) ) { return false; }

		return true;
	}

	// Starts all hotkeys.
	void CHotkeyManager::Start() {
		MX_HOTKEY_LOCK hlLock( this );
		m_hpmPollingHotkeys.Start();
		m_hwkmWhKeyboardMethod.Start();
		m_hrhkmRegisteredHotkeys.Start();
	}

	// Stops all hotkeys.
	void CHotkeyManager::Stop() {
		MX_HOTKEY_LOCK hlLock( this );
		m_hpmPollingHotkeys.Stop();
		m_hwkmWhKeyboardMethod.Stop();
		m_hrhkmRegisteredHotkeys.Stop();
	}

	// Clears the hotkey manager and fully resets its state to default.
	void CHotkeyManager::Reset() {
		MX_HOTKEY_LOCK hlLock( this );
		m_hpmPollingHotkeys.Reset();
		m_hwkmWhKeyboardMethod.Reset();
		m_hrhkmRegisteredHotkeys.Reset();
	}

	// Gets a text description of a hotkey along with a set of flags to describe key properties.
	bool CHotkeyManager::GetKeyDescByIndex( size_t _sIdx, std::string &_sDesc, LPARAM &_lpData ) {
		if ( _sIdx < m_hpmPollingHotkeys.TotalHotkeys() ) {
			_lpData = m_hpmPollingHotkeys.Get( _sIdx ).iIdentifier | (MX_M_POLLING << MX_H_KEY_LPARAM_METHOD_SHIFT);
			_sDesc = _DEC_S_236424D8_POLL_;
			return DescribeHotkey( m_hpmPollingHotkeys.Get( _sIdx ), _sDesc, false );
		}
		_sIdx -= m_hpmPollingHotkeys.TotalHotkeys();


		if ( _sIdx < m_hwkmWhKeyboardMethod.TotalHotkeys() ) {
			_lpData = m_hwkmWhKeyboardMethod.Get( _sIdx ).iIdentifier | (MX_M_HOOK << MX_H_KEY_LPARAM_METHOD_SHIFT);
			_sDesc = _DEC_S_3EF3D005_HOOK_;
			return DescribeHotkey( m_hwkmWhKeyboardMethod.Get( _sIdx ), _sDesc, false );
		}
		_sIdx -= m_hwkmWhKeyboardMethod.TotalHotkeys();


		if ( _sIdx < m_hrhkmRegisteredHotkeys.TotalHotkeys() ) {
			_lpData = m_hrhkmRegisteredHotkeys.Get( _sIdx ).iIdentifier | (MX_M_HOTKEY << MX_H_KEY_LPARAM_METHOD_SHIFT);
			_sDesc = _DEC_S_C4F2B257_HKEY_;
			return DescribeHotkey( m_hrhkmRegisteredHotkeys.Get( _sIdx ), _sDesc, true );
		}
		_sIdx -= m_hrhkmRegisteredHotkeys.TotalHotkeys();

		return false;
	}

	// Fills a list box with the hotkey instances.
	bool CHotkeyManager::FillListBoxWithHotkeys( lsw::CListBox * _plbBox, const std::set<LPARAM> &_sSelections ) {
		size_t sTotal = Total();
		for ( size_t I = 0; I < sTotal; ++I ) {
			CSecureString ssTmp;
			LPARAM lpParm;
			if ( !GetKeyDescByIndex( I, ssTmp, lpParm ) ) { return false; }
			INT iRes = _plbBox->AddString( CUtilities::StringToWString( ssTmp ).c_str() );
			if ( iRes == LB_ERRSPACE ) { return false; }
			_plbBox->SetItemData( iRes, lpParm );
			_plbBox->SetSel( _sSelections.count( lpParm ) != 0, iRes );
		}
		return true;
	}

	// Gets the information about a key.
	UINT CHotkeyManager::VirtualKeyInfo( UINT _uCode ) {
		UINT uiResult = 0;
		UINT uiKey = ::MapVirtualKeyW( _uCode, MAPVK_VK_TO_VSC_EX );
		UINT uiChar = ::MapVirtualKeyW( _uCode, MAPVK_VK_TO_CHAR );

		WCHAR szBufText[64];
		WCHAR szBufText2[64];
		if ( !::GetKeyNameTextW( (uiKey << 16), szBufText, MX_ELEMENTS( szBufText ) ) ) { return false; }
		if ( ::GetKeyNameTextW( ((uiKey | KF_EXTENDED) << 16), szBufText2, MX_ELEMENTS( szBufText2 ) ) ) {
			if ( std::wcscmp( szBufText2, szBufText ) != 0 ) {
				uiResult |= MX_K_HAS_EXTENDED;
			}
		}
		if ( uiChar ) {
			uiResult |= MX_K_PRINTABLE;
		}
		if ( uiKey & 0xE000 == 0xE000 ) {
			uiResult |= MX_K_HAS_LEFT_RIGHT;
		}
		return uiResult;
	}

	// Maps a given virtual-key code to a into a virtual-key code as text.
	CSecureWString CHotkeyManager::VirtualKeyToString( UINT _uCode, bool bExtended ) {
		CSecureWString wsRet;
		//WCHAR szBufText[64];
		UINT uiKey = ::MapVirtualKeyW( _uCode, MAPVK_VK_TO_VSC_EX );
		return ScanCodeToString( ((uiKey | (bExtended ? KF_EXTENDED : 0)) << 16) );
		/*if ( ::GetKeyNameTextW( ((uiKey | (bExtended ? KF_EXTENDED : 0)) << 16), szBufText, MX_ELEMENTS( szBufText ) ) ) {
			wsRet += szBufText;
		}
		return wsRet;*/
	}

	// Maps a given virtual-key code to a into a virtual-key code as text.
	CSecureWString CHotkeyManager::VirtualKeyToString( UINT _uCode ) {
		CSecureWString wsFinalName;
		// Is it a key code?
		UINT uiKey = ::MapVirtualKeyW( _uCode, MAPVK_VK_TO_VSC_EX );
		UINT uiKeyBack = ::MapVirtualKeyW( uiKey, MAPVK_VSC_TO_VK_EX );
		if ( _uCode != uiKeyBack && uiKeyBack ) {
			CSecureWString wsName = ScanCodeToString( uiKey << 16 );
			//CSecureWString wsAltName = ScanCodeToString( (uiKey | KF_EXTENDED) << 16 );
			wsFinalName += wsName;
		}
		else {
		//if ( _uCode == uiKeyBack ) {
			CSecureWString wsName = ScanCodeToString( uiKey << 16 );
			CSecureWString wsAltName = ScanCodeToString( (uiKey | KF_EXTENDED) << 16 );
			if ( wsName.size() ) {					
				wsFinalName += wsName;

				bool bAltNameMatches = wsAltName == wsName;
				if ( wsAltName.size() && !bAltNameMatches ) {
					wsName = wsAltName;

					if ( wsFinalName.size() ) {
						wsFinalName += L", ";
					}
					wsFinalName += wsName;
				}
			}
		}
		return wsFinalName;
	}

	// Maps a given scan code to a into a virtual-key code as text.
	CSecureWString CHotkeyManager::ScanCodeToString( UINT _uiKey ) {
		CSecureWString wsRet;
		WCHAR szBufText[64];
		if ( ::GetKeyNameTextW( _uiKey, szBufText, MX_ELEMENTS( szBufText ) ) ) {
			wsRet += CUtilities::EscapeUnprintable( szBufText, true, false );
		}
		return wsRet;
	}

	// Fills a combo box with all named scan codes.
	void CHotkeyManager::FillComboWithCodes( lsw::CComboBox * _pcbCombo ) {
		if ( !_pcbCombo ) { return; }
		/*UINT uiKey2 = ::MapVirtualKeyW( VK_PRIOR, MAPVK_VK_TO_VSC_EX );
		UINT uiKeyBack2 = ::MapVirtualKeyW( uiKey2, MAPVK_VSC_TO_VK_EX );
		CSecureWString wsName2 = ScanCodeToString( uiKey2 << 16 );
		CSecureWString wsAltName2 = ScanCodeToString( (uiKey2 | KF_EXTENDED) << 16 );*/
		static const UINT uiSkip[] = {
			VK_SHIFT, VK_CONTROL, VK_MENU,
			VK_LSHIFT,
			VK_RSHIFT,
			VK_LCONTROL,
			VK_RCONTROL,
			VK_LMENU,
			VK_RMENU,
			VK_LWIN,
			VK_RWIN
		};

		for ( UINT I = 0; I <= 0xFF; ++I ) {
			bool bSkip = false;
			for ( auto J = MX_ELEMENTS( uiSkip ); J--; ) {
				if ( uiSkip[J] == I ) { bSkip = true; break; }
			}
			if ( bSkip ) { continue; }
			CSecureWString wsFinalName = VirtualKeyToString( I );
			// Is it a key code?
			/*UINT uiKey = ::MapVirtualKeyW( I, MAPVK_VK_TO_VSC_EX );
			UINT uiKeyBack = ::MapVirtualKeyW( uiKey, MAPVK_VSC_TO_VK_EX );
			if ( I != uiKeyBack && uiKeyBack ) {
				CSecureWString wsName = ScanCodeToString( uiKey << 16 );
				//CSecureWString wsAltName = ScanCodeToString( (uiKey | KF_EXTENDED) << 16 );
				wsFinalName += wsName;
			}
			else {
			//if ( I == uiKeyBack ) {
				CSecureWString wsName = ScanCodeToString( uiKey << 16 );
				CSecureWString wsAltName = ScanCodeToString( (uiKey | KF_EXTENDED) << 16 );
				if ( wsName.size() ) {					
					wsFinalName += wsName;

					bool bAltNameMatches = wsAltName == wsName;
					if ( wsAltName.size() && !bAltNameMatches ) {
						wsName = wsAltName;

						if ( wsFinalName.size() ) {
							wsFinalName += L", ";
						}
						wsFinalName += wsName;
					}
				}
			}*/
			if ( wsFinalName.size() ) {
				wsFinalName += L": ";
				CUtilities::ToHex( I, wsFinalName, 2 );
				
				INT iIndex = _pcbCombo->AddString( wsFinalName.c_str() );
				_pcbCombo->SetItemData( iIndex, I );
			}
			/*}
			else {
				CSecureWString wsName = ScanCodeToString( uiKey << 16 );
				INT iIndex = _pcbCombo->AddString( wsName.c_str() );
				_pcbCombo->SetItemData( iIndex, I );
			}*/
		}
#if 0
		std::map<CSecureWString, UINT> mMap;
		for ( UINT I = VK_BACK; I <= VK_OEM_CLEAR; ++I ) {
			CSecureWString wsName = VirtualKeyToString( I, false );
			if ( wsName.size() ) {
				UINT uiKey = ::MapVirtualKeyW( I, MAPVK_VK_TO_VSC );
				UINT uiKeyBack = ::MapVirtualKeyW( uiKey, MAPVK_VSC_TO_VK );
				if ( I == uiKeyBack ) {
					wsName += L":";
					CUtilities::ToHex( I, wsName, 2 );
					/*wsName += L":";
					CUtilities::ToHex( uiKey, wsName, 2 );
					wsName += L":";
					CUtilities::ToHex( uiKeyBack, wsName, 4 );*/

					size_t sCount = mMap.count( wsName );
					if ( !sCount ) {
						mMap[wsName] = uiKey;
					}
					else if ( mMap[wsName] != uiKey ) {
						CSecureWString wTmp = wsName;
						size_t J = 1;
						while ( mMap.count( wTmp ) ) {
							if ( mMap[wTmp] == uiKey ) { break; }
							wTmp = wsName;
							wTmp += L"[";
							CUtilities::ToUnsigned( J++, wTmp );
							wTmp += L"]";
						}
						mMap[wTmp] = uiKey;
					}
				}
			}
		}

		// Same for extended codes.
		for ( UINT I = VK_BACK; I <= VK_OEM_CLEAR; ++I ) {
			CSecureWString wsName = VirtualKeyToString( I, true );
			if ( wsName.size() ) {
				UINT uiKey = ::MapVirtualKeyW( I, MAPVK_VK_TO_VSC_EX );
				UINT uiKeyBack = ::MapVirtualKeyW( uiKey, MAPVK_VSC_TO_VK_EX );
				if ( I == uiKeyBack ) {
					wsName += L":";
					CUtilities::ToHex( I, wsName, 2 );
					/*wsName += L":";
					CUtilities::ToHex( uiKey, wsName, 2 );
					wsName += L":";
					CUtilities::ToHex( uiKeyBack, wsName, 4 );*/

					size_t sCount = mMap.count( wsName );
					if ( !sCount ) {
						mMap[wsName] = uiKey;
					}
					else if ( mMap[wsName] != uiKey ) {
						CSecureWString wTmp = wsName;
						size_t J = 1;
						while ( mMap.count( wTmp ) ) {
							if ( mMap[wTmp] == uiKey ) { break; }
							wTmp = wsName;
							wTmp += L"[";
							CUtilities::ToUnsigned( J++, wTmp );
							wTmp += L"]";
						}
						mMap[wTmp] = uiKey;
					}
				}
			}
		}

		for ( auto I = mMap.begin(); I != mMap.end(); ++I ) {
			INT iIndex = _pcbCombo->AddString( (*I).first.c_str() );
			_pcbCombo->SetItemData( iIndex, (*I).second );
		}
#endif
	}

	// Fills a combo with modifiers.
	void CHotkeyManager::FillComboWithMods( lsw::CComboBox * _pcbCombo ) {
		if ( !_pcbCombo ) { return; }
		INT iIndex = _pcbCombo->AddString( _DEC_WS_DFA2AFF1_None.c_str() );
		_pcbCombo->SetItemData( iIndex, 0 );

		static const struct {
			UINT			uiCode;
			UINT			uiExt;
		} sData[] = {
			{ VK_SHIFT },
			{ VK_CONTROL },
			{ VK_MENU },
			/*{ VK_LWIN, KF_EXTENDED },
			{ VK_RWIN, KF_EXTENDED },*/
			/*{ VK_LSHIFT },
			{ VK_RSHIFT, KF_EXTENDED },
			{ VK_LCONTROL },
			{ VK_RCONTROL, KF_EXTENDED },
			{ VK_LMENU },
			{ VK_RMENU, KF_EXTENDED },*/
		};
		for ( auto J = 0; J < MX_ELEMENTS( sData ); ++J ) {
			CSecureWString wsFinalName;
			UINT uiKey = ::MapVirtualKeyW( sData[J].uiCode, MAPVK_VK_TO_VSC_EX );

			CSecureWString wsAltName = ScanCodeToString( (uiKey | sData[J].uiExt) << 16 );

			wsFinalName += wsAltName;


			
			if ( wsFinalName.size() ) {				
				iIndex = _pcbCombo->AddString( wsFinalName.c_str() );
				_pcbCombo->SetItemData( iIndex, sData[J].uiCode );
			}
		}
	}

	// Fills a combo with methods.
	void CHotkeyManager::FillComboWithMethods( lsw::CComboBox * _pcbCombo ) {
		if ( !_pcbCombo ) { return; }
		static const struct {
			const char *	pcName;
			size_t			sLen;
			uint32_t		ui32Method;
		} sMethods[] = {
			{ _T_LEN_05871BCC_Polling, MX_M_POLLING },
			//{ _T_LEN_046AEC6B_Hook, MX_M_HOOK },			// Commenting this out is the only reason the feature is not available.  Until DLL injection works it will be of limited use.
			{ _T_LEN_D8278C4E_Hotkey, MX_M_HOTKEY },
		};
		for ( auto J = 0; J < MX_ELEMENTS( sMethods ); ++J ) {
			INT iIndex = _pcbCombo->AddString( mx::CStringDecoder::DecodeToWString( sMethods[J].pcName, sMethods[J].sLen ).c_str() );
			_pcbCombo->SetItemData( iIndex, sMethods[J].ui32Method );
		}
	}

	// Fills a combo with functions.
	void CHotkeyManager::FillComboWithFunctions( lsw::CComboBox * _pcbCombo ) {
		if ( !_pcbCombo ) { return; }
		lsw::CCriticalSection::CEnterCrit ecCrit( CHotkeyManBase::m_csHandlerCrit );
		for ( auto J = 0; J < CHotkeyManBase::m_vHandlers.size(); ++J ) {
			INT iIndex = _pcbCombo->AddString( mx::CStringDecoder::DecodeToWString( CHotkeyManBase::m_vHandlers[J].hdName.pcDesc, CHotkeyManBase::m_vHandlers[J].hdName.sLen ).c_str() );
			_pcbCombo->SetItemData( iIndex, CHotkeyManBase::m_vHandlers[J].uiId );
		}
	}

	// Creates a text description of a hotkey instance.
	bool CHotkeyManager::DescribeHotkey( const CHotkeyManBase::MX_HOTKEY_INSTANCE &_hiInst, std::string &_sRes, bool _bIgnoreLeftRight ) {
		try {
			size_t sParms = _hiInst.phhHandler->sParms;
			CSecureWString wswTmp;
			if ( _hiInst.iMod ) {
				if ( !ModifierToString( _hiInst.iMod, wswTmp, _bIgnoreLeftRight ) ) { return false; }
				wswTmp += L"+";
			}
			wswTmp += ScanCodeToString( _hiInst.uiScanCode );
			_sRes += CUtilities::WStringToString( wswTmp );
			_sRes += ":";
			_sRes += mx::CStringDecoder::DecodeToString( _hiInst.phhHandler->hdName.pcDesc, _hiInst.phhHandler->hdName.sLen );
			_sRes += "(";
			for ( size_t I = 0; I < sParms; ++I ) {
				if ( I ) {
					_sRes += ",";
				}
				const char * pcFormat = "%I64u";
				CSecureString ssTmp;
				if ( _hiInst.phhHandler->hdFormattings[I].pcDesc ) {
					if ( _hiInst.phhHandler->hdFormattings[I].sLen ) {
						ssTmp = mx::CStringDecoder::DecodeToString( _hiInst.phhHandler->hdFormattings[I].pcDesc, _hiInst.phhHandler->hdFormattings[I].sLen );
					}
					else {
						ssTmp = _hiInst.phhHandler->hdFormattings[I].pcDesc;
					}
					pcFormat = ssTmp.c_str();
				}
				char szBuffer[64];
				::sprintf_s( szBuffer, MX_ELEMENTS( szBuffer ), pcFormat, _hiInst.uiParms[I], _hiInst.uiParms[I], _hiInst.uiParms[I] );
				_sRes += szBuffer;
			}

			_sRes += ")";
		}
		catch ( const std::bad_alloc & ) { return false; }

		return true;
	}

	// Converts a key code and a modifier to text.
	bool CHotkeyManager::KeyCodeToString( INT _iCode, INT _iMod, CSecureWString &_swsResult, bool _bIgnoreLeftRight ) {
		if ( _iMod ) {
			if ( !ModifierToString( _iMod, _swsResult, _bIgnoreLeftRight ) ) { return false; }
			_swsResult += L"+";
		}
		_swsResult += VirtualKeyToString( _iCode );
		return true;
	}

	// Converts a modifier to text.
	bool CHotkeyManager::ModifierToString( INT _iMod, CSecureWString &_swsResult, bool _bIgnoreLeftRight ) {
		if ( _bIgnoreLeftRight ) {
			if ( _iMod == VK_LSHIFT || _iMod == VK_RSHIFT ) {
				_iMod = VK_SHIFT;
			}
		}
		UINT uiKey = ::MapVirtualKeyW( _iMod, MAPVK_VK_TO_VSC_EX );
		UINT uiExt = (_iMod == VK_LWIN || _iMod == VK_RWIN) ? KF_EXTENDED : 0;
		if ( !_bIgnoreLeftRight ) {
			uiExt |= (_iMod == VK_RCONTROL) ? KF_EXTENDED : 0;
			uiExt |= (_iMod == VK_RMENU) ? KF_EXTENDED : 0;
		}
		_swsResult += ScanCodeToString( (uiKey | uiExt) << 16 );
		return true;
	}

}	// namespace mx

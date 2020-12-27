#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXSecureWString.h"
#include "MXHotkeyPollingMethod.h"
#include "MXHotkeyRegisterHotKeyMethod.h"
#include "MXHotkeyWhKeyboardMethod.h"
#include <ComboBox/LSWComboBox.h>
#include <ListBox/LSWListBox.h>
#include <set>


#define MX_H_KEY_LPARAM_METHOD_SHIFT				28
#define MX_H_KEY_LPARAM_ID_MASK						0xFFFFFFF


namespace mx {

	class CHotkeyManager {
	public :
		CHotkeyManager();
		~CHotkeyManager();


		// == Enumerations.
		// Key properties.
		enum MX_KEY : uint32_t {
			MX_K_HAS_EXTENDED						= (1 << 0),
			MX_K_HAS_LEFT_RIGHT						= (1 << 1),
			MX_K_PRINTABLE							= (1 << 2),
		};

		// Methods for running/managing hotkeys.
		enum MX_METHOD : uint32_t {
			MX_M_POLLING,
			MX_M_HOOK,
			MX_M_HOTKEY,
		};


		// == Types.
		// Safely locks the hotkey manager.
		struct MX_HOTKEY_LOCK {
			MX_HOTKEY_LOCK( const CHotkeyManager * _phmbHotkeyMan ) :
				phkmHotkeyMan( _phmbHotkeyMan ) {
				if ( phkmHotkeyMan ) { phkmHotkeyMan->Lock(); }
			}
			~MX_HOTKEY_LOCK() {
				if ( phkmHotkeyMan ) { phkmHotkeyMan->Unlock(); }
			}


		protected :
			const CHotkeyManager *					phkmHotkeyMan;
		};


		// == Functions.
		// Adds a hotkey.
		bool										AddHotkey( INT _iId, INT _iCode, INT _iMod, UINT _uiScanCode, uint32_t _ui32Method, uint32_t _ui32Function,
			uint64_t _ui64Parm0, uint64_t _ui64Parm1, uint64_t _ui64Parm2, std::string &_sError );

		// Deletes a hotkey based off its LPARAM (a combination of its ID and method).
		bool										DeleteByLParam( LPARAM _lpDeleteMe );

		// Moves all hotkeys up from an array of hotkeys identified by their LPARAM values.
		bool										MoveUp( const std::vector<LPARAM> &_vMoveUs );

		// Moves all hotkeys down from an array of hotkeys identified by their LPARAM values.
		bool										MoveDown( const std::vector<LPARAM> &_vMoveUs );

		// Handle WM_HOTKEY messages.
		bool										HandleWM_HOTKEY( INT _iId, INT _iCode, INT _iMod );

		// Handle a poll hotkey.
		bool										HandlePollHotkey( INT _iId );

		// Handle a WH_KEYBOARD hooked hotkey.
		bool										HandleHookHotkey( INT _iId );

		// Determines if the given key combination already exists.
		bool										HaveHotkeyAlready( INT _iCode, INT _iMod ) const;

		// Gets a hotkey based off its ID.
		const CHotkeyManBase::MX_HOTKEY_INSTANCE *	GetById( INT _iId ) const;

		// Creates a copy of the hotkey manager.
		bool										Copy( const CHotkeyManager &_hmSrc );

		// Starts all hotkeys.
		void										Start();

		// Stops all hotkeys.
		void										Stop();

		// Clears the hotkey manager and fully resets its state to default.
		void										Reset();

		// Gets the "polling hotkey" manager.
		CHotkeyManBase *							GetPollingHotkeyManager() { return &m_hpmPollingHotkeys; }

		// Gets the "polling hotkey" manager.
		const CHotkeyManBase *						GetPollingHotkeyManager() const { return &m_hpmPollingHotkeys; }

		// Gets the "WH_KEYBOARD hotkey" manager.
		CHotkeyManBase *							GetWhKeyboardHotkeyManager() { return &m_hwkmWhKeyboardMethod; }

		// Gets the "WH_KEYBOARD hotkey" manager.
		const CHotkeyManBase *						GetWhKeyboardHotkeyManager() const { return &m_hwkmWhKeyboardMethod; }

		// Gets the "registered hotkey" manager.
		CHotkeyManBase *							GetRegisteredHotkeyManager() { return &m_hrhkmRegisteredHotkeys; }

		// Gets the "registered hotkey" manager.
		const CHotkeyManBase *						GetRegisteredHotkeyManager() const { return &m_hrhkmRegisteredHotkeys; }

		// Returns the total number of hotkeys in the manager.
		size_t										Total() const { return m_hrhkmRegisteredHotkeys.TotalHotkeys() + m_hwkmWhKeyboardMethod.TotalHotkeys() + m_hpmPollingHotkeys.TotalHotkeys(); }

		// Locks for updating.
		virtual void								Lock() const { m_csCrit.EnterCriticalSection(); }

		// Unlocks.
		virtual void								Unlock() const { m_csCrit.LeaveCriticalSection(); }

		// Gets a text description of a hotkey along with a set of flags to describe key properties.
		virtual bool								GetKeyDescByIndex( size_t _sIdx, std::string &_sDesc, LPARAM &_lpData );

		// Fills a list box with the hotkey instances.
		virtual bool								FillListBoxWithHotkeys( lsw::CListBox * _plbBox, const std::set<LPARAM> &_sSelections );

		// Gets the information about a key.
		static UINT									VirtualKeyInfo( UINT _uCode );

		// Maps a given virtual-key code to a into a virtual-key code as text.
		static CSecureWString						VirtualKeyToString( UINT _uCode, bool bExtended );

		// Maps a given virtual-key code to a into a virtual-key code as text.
		static CSecureWString						VirtualKeyToString( UINT _uCode );

		// Maps a given scan code to a into a virtual-key code as text.
		static CSecureWString						ScanCodeToString( UINT _uiKey );

		// Fills a combo box with all named scan codes.
		static void									FillComboWithCodes( lsw::CComboBox * _pcbCombo );

		// Fills a combo with modifiers.
		static void									FillComboWithMods( lsw::CComboBox * _pcbCombo );

		// Fills a combo with methods.
		static void									FillComboWithMethods( lsw::CComboBox * _pcbCombo );

		// Fills a combo with functions.
		static void									FillComboWithFunctions( lsw::CComboBox * _pcbCombo );

		// Creates a text description of a hotkey instance.
		static bool									DescribeHotkey( const CHotkeyManBase::MX_HOTKEY_INSTANCE &_hiInst, std::string &_sRes, bool _bIgnoreLeftRight );

		// Converts a key code and a modifier to text.
		static bool									KeyCodeToString( INT _iCode, INT _iMod, CSecureWString &_swsResult, bool _bIgnoreLeftRight );

		// Converts a modifier to text.
		static bool									ModifierToString( INT _iMod, CSecureWString &_swsResult, bool _bIgnoreLeftRight );

		// Prints information about a given WM_HOTKEY

	protected :
		// == Members.
		// The polling manager.
		CHotkeyPollingMethod						m_hpmPollingHotkeys;
		// The WH_KEYBOARD manager.
		CHotkeyWhKeyboardMethod						m_hwkmWhKeyboardMethod;
		// The registered hotkey manager.
		CHotkeyRegisterHotKeyMethod					m_hrhkmRegisteredHotkeys;
		// The critical section for this manager.
		lsw::CCriticalSection						m_csCrit;

		// The unique ID of each hotkey.
		static LONG									m_lHotkeyId;


		// == Functions.
		
	};

}	// namespace mx

#pragma once

#include "../MXMhsX.h"
#include "MXHotkeyManBase.h"



namespace mx {

	class CHotkeyRegisterHotKeyMethod : public CHotkeyManBase {
	public :
		CHotkeyRegisterHotKeyMethod();
		virtual ~CHotkeyRegisterHotKeyMethod();


		// == Functions.
		// Sets the window with which to register the hotkeys.
		virtual void					SetWindow( HWND _hWnd ) { m_hWnd = _hWnd; }

		// Handle WM_HOTKEY messages.
		size_t							HandleWM_HOTKEY( INT _iId, INT _iCode, INT _iMod );

		// Loads hotkey data from a file loaded to memory.
		virtual bool					LoadFromMemory( const std::vector<uint8_t> &_vFile, uint32_t _uiVersion ) { return false; }

		// Saves hotkey data to an in-memory file image.
		virtual bool					SaveToMemory( std::vector<uint8_t> &_vFile ) { return false; }

		// Adds a key and returns the ID or 0.
		virtual LONG					AddKey( INT _iCode, INT _iMod, UINT _uiScanCode, uint32_t _ui32Function,
											uint64_t _ui64Parm0, uint64_t _ui64Parm1, uint64_t _ui64Parm2,
											std::string &_sError );

		// Begins polling or registers the hotkeys.
		virtual bool					Start();

		// Stops polling or unregisters the hotkeys.
		virtual bool					Stop();

		// Removes all hotkeys.
		virtual void					Clear();

		// Copies the object.
		virtual bool					Copy( const CHotkeyManBase &_hmbOther );

		// Returns a boolean indicating whether the given key and modifier exist.
		virtual bool					HasKey( INT _iCode, INT _iMod ) const;


	protected :
		// == Members.
		// The window where the hotkeys are registered.
		HWND							m_hWnd;


		// == Functions.
		// Registers all of the hotkeys.
		bool							RegisterAll();

		// Unregisters all of the hotkeys.
		void							UnregisterAll();

		// Converts a virtual key to a ::RegisterHotKey() modifier.
		static UINT						VirtualKeyToModifier( UINT _uiKey );

	private :
		typedef CHotkeyManBase			Parent;

	};

}	// namespace mx

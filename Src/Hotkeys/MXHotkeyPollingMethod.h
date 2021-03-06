#pragma once

#include "../MXMhsX.h"
#include "MXHotkeyManBase.h"
#include <Threads/LSWThread.h>



namespace mx {

	class CHotkeyPollingMethod : public CHotkeyManBase {
	public :
		CHotkeyPollingMethod();
		virtual ~CHotkeyPollingMethod();


		// == Functions.
		// Sets the window to notify of hotkey presses.
		virtual void					SetWindow( HWND _hWnd ) { m_hWnd = _hWnd; }

		// Loads hotkey data from a file loaded to memory.
		virtual bool					LoadFromMemory( const std::vector<uint8_t> &_vFile, uint32_t _uiVersion ) { return false; }

		// Saves hotkey data to an in-memory file image.
		virtual bool					SaveToMemory( std::vector<uint8_t> &_vFile ) { return false; }

		// Begins polling or registers the hotkeys.
		virtual bool					Start();

		// Stops polling or unregisters the hotkeys.
		virtual bool					Stop();

		// Copies the object.
		virtual bool					Copy( const CHotkeyManBase &_hmbOther );


	protected :
		// == Members.
		// The polling thread.
		lsw::CThread					m_tThread;
		// The cancel atom.
		volatile LONG					m_lCancelThread;
		// The window to notify of hotkey presses.
		HWND							m_hWnd;


		// == Functions.
		// Starts the polling thread.
		bool							StartPollingThread();

		// Stops the polling thread and waits.
		bool							StopPollingThreadAndWait();

		// Performs a single poll.
		bool							Poll();

		// The thread procedure.
		static DWORD WINAPI				ThreadProc( LPVOID _lpParameter );

	private :
		typedef CHotkeyManBase			Parent;
	};

}	// namespace mx

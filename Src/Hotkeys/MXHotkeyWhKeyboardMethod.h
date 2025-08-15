#pragma once

#include "../MXMhsX.h"
#include "MXHotkeyManBase.h"
#include <Threads/LSWThread.h>



namespace mx {

	class CHotkeyWhKeyboardMethod : public CHotkeyManBase {
	public :
		CHotkeyWhKeyboardMethod();
		virtual ~CHotkeyWhKeyboardMethod();


		// == Functions.
		// Sets the window to notify of hotkey presses.
		virtual void					SetWindow( HWND _hWnd ) { m_hWnd = _hWnd; }

		// Loads hotkey data from a file loaded to memory.
		virtual bool					LoadFromMemory( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, uint32_t _uiVersion ) { return CHotkeyManBase::LoadFromMemory( _pjJson, _pjcContainer, _psBinary, _uiVersion ); }

		// Saves hotkey data to an in-memory file image.
		virtual bool					SaveToMemory( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary ) const { return CHotkeyManBase::SaveToMemory( _peJson, _psBinary ); }

		// Begins polling or registers the hotkeys.
		virtual bool					Start();

		// Stops polling or unregisters the hotkeys.
		virtual bool					Stop();

		// Copies the object.
		virtual bool					Copy( const CHotkeyManBase &_hmbOther );


	protected :
		// == Members.
		// The window to notify of hotkey presses.
		HWND							m_hWnd;
		// The next hook procedure.
		HHOOK							m_hHook;
		// The point to the last instance of this class to create a hook procedure.
		static CHotkeyWhKeyboardMethod *m_phwkmHotkeyMan;


		// == Functions.
		// The hook procedure.
		static LRESULT CALLBACK			KeyboardProc( int _iCode, WPARAM _wParam, LPARAM _lParam );

	private :
		typedef CHotkeyManBase			Parent;
	};

}	// namespace mx

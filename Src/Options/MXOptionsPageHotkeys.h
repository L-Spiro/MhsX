#pragma once
#include "../MXMhsX.h"
#include "../Hotkeys/MXHotkeyManager.h"
#include "../Options/MXOptions.h"
#include "MXOptionsPage.h"
#include <Images/LSWBitmap.h>
#include <LSWWin.h>
#include <set>

using namespace lsw;

namespace mx {

	class COptionsPageHotkeys : public COptionsPage {
	public :
		COptionsPageHotkeys( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Get the name of the page.
		virtual std::wstring					GetName() const;

		// WM_INITDIALOG.
		virtual LSW_HANDLED						InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED						Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// Verifies the options, returning an error string in case of error.
		virtual BOOL							Verify( std::wstring &_wsError, CWidget * &_pwWidget );

		// Copies all the settings to the MX_OPTIONS structure.
		virtual BOOL							Finalize();

		// WM_KEYDOWN
		virtual LSW_HANDLED						KeyDown( UINT _uiKeyCode, UINT _uiFlags );

		// Are we ignoring left and right modifier keys?
		bool									IgnoringLeftRight() const;

		// Translate a child's tooltip text.
		virtual std::wstring					TranslateTooltip( const std::string &_sText );


	protected :
		// == Enumerations.
		enum MX_LISTENING_STATE : uint32_t {
			MX_LS_WAITING,
			MX_LS_LISTENING,
			MX_LS_SUCCESS,
			MX_LS_FAILURE,
		};


		// == Members.
		// Options.
		MX_OPTIONS *							m_poOptions;
		// Our copy of the hotkey manager.  Changes get discarded if cancelling.
		CHotkeyManager							m_hmManager;
		// Button original procedure.
		WNDPROC									m_wpOrigProc;
		// Key-press state.
		MX_LISTENING_STATE						m_lsListenState;
		// If true, the hotkey is automatically added after the buttons are determined.
		bool									m_bAddAfterSelectingButtons;
		// The hotkey virtual key code.
		BYTE									m_bKeyCode;
		// The hotkey modifier code.
		BYTE									m_bKeyModifier;
		// The hotkey scancode.
		DWORD									m_dwScanCode;
		// The warning bitmap.
		lsw::CBitmap							m_bBitmap;
		// Window property.
		static WCHAR							m_szProp[2];


		// == Functions.
		// Apply dialog settings.
		VOID									ApplySettings();

		// Handles the Add button.
		bool									AddButton();
		
		// Handles the Delete button.
		bool									DeleteButton();

		// Handles the Up button.
		bool									UpButton();

		// Handles the Down button.
		bool									DownButton();

		// Starts listening for input.
		bool									BeginListening();

		// Stops listening for input.
		bool									StopListening( bool _bSuccess );

		// Fills a set of LPARAM's, one for each selected item in the list box.
		void									GatherSelected( std::set<LPARAM> &_sResult );

		// Sets the current information based on the current selection.
		void									FillInfoBySelection();

		// Edit window procedure.
		static LRESULT CALLBACK					ButtonOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );
	};

}	// namespace mx

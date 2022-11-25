#pragma once
#include "../MXMhsX.h"
#include "../Hotkeys/MXHotkeyManager.h"
#include "../Options/MXOptions.h"
#include "../Programs/MXPrograms.h"
#include "MXOptionsPage.h"
#include <Images/LSWBitmap.h>
#include <LSWWin.h>
#include <set>

using namespace lsw;

namespace mx {

	class COptionsPagePrograms : public COptionsPage {
	public :
		COptionsPagePrograms( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


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
		// Programs.
		std::vector<MX_PROGRAM>					m_vPrograms;
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

		// Handles the Hidden check.
		bool									HiddenCheck();

		// Browse for a program.
		bool									BrowseProgram();

		// Browse for a working directory.
		bool									BrowseWorkingDirectory();

		// Fills a set of LPARAM's, one for each selected item in the list box.
		void									GatherSelected( std::vector<LPARAM> &_sResult );

		// Sets the current information based on the current selection.
		void									FillInfoBySelection();
	};

}	// namespace mx

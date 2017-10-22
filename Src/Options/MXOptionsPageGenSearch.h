#pragma once
#include "../MXMhsX.h"
#include "../Options/MXOptions.h"
#include "MXOptionsPage.h"
#include <LSWWin.h>

using namespace lsw;

namespace mx {

	class COptionsPageGenSearch : public COptionsPage {
	public :
		COptionsPageGenSearch( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, MX_OPTIONS * _poOptions, bool _bCreateWidget = true, HMENU _hMenu = NULL );


		// == Functions.
		// Get the name of the page.
		virtual std::wstring					GetName() const;

		// WM_INITDIALOG.
		virtual LSW_HANDLED						InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED						Command( WORD _Id, HWND _hControl );

		// Verifies the options, returning an error string in case of error.
		virtual BOOL							Verify( std::wstring &_wsError, CWidget * &_pwWidget );

		// Copies all the settings to the MX_OPTIONS structure.
		virtual BOOL							Finalize();


	protected :
		// == Members.
		// Options.
		MX_OPTIONS *							m_poOptions;


		// == Functions.
		// Apply dialog settings.
		VOID									ApplySettings();									
	};

}	// namespace mx

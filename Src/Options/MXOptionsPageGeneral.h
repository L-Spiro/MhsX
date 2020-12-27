#pragma once
#include "../MXMhsX.h"
#include "../Options/MXOptions.h"
#include "MXOptionsPage.h"
#include <LSWWin.h>

using namespace lsw;

namespace mx {

	class COptionsPageGeneral : public COptionsPage {
	public :
		COptionsPageGeneral( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


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

		// Updates the text example of data-type settings.
		virtual void							UpdateExample();


	protected :
		// == Members.
		// Options.
		MX_OPTIONS *							m_poOptions;
	};

}	// namespace mx

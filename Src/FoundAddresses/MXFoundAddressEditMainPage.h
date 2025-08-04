#pragma once
#include "../MXMhsX.h"
#include "../Strings/MXSecureWString.h"
#include "MXFoundAddressEditPageBase.h"

#include <LSWWin.h>

using namespace lsw;

namespace mx {

	class CFoundAddressEditMainPage : public CFoundAddressEditPageBase {
	public :
		CFoundAddressEditMainPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


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

		// Updates the contents of page based on settings etc.
		virtual void							Update();


	protected :
		// == Members.


		// == Functions.
		// Gathers all descriptions and returns the Description string.
		CSecureWString							GatherDescriptions() const;

		// Gathers all current value strings.
		CSecureWString							GatherCurValue() const;
		
	};

}	// namespace mx

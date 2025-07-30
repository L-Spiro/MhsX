#pragma once
#include "../MXMhsX.h"
#include <LSWWin.h>
#include <Widget/LSWWidget.h>

#include <string>

using namespace lsw;

namespace mx {

	class CFoundAddressEditPageBase : public lsw::CWidget {
	public :
		CFoundAddressEditPageBase( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			lsw::CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		}


		// == Functions.
		// Get the name of the page.
		virtual std::wstring					GetName() const { return std::wstring(); }

		// Verifies the options, returning an error string in case of error.
		virtual BOOL							Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
			_pwWidget = nullptr;
			return FALSE;
		}

		// Copies all the settings to the MX_OPTIONS structure.
		virtual BOOL							Finalize() { return TRUE; }

		// WM_INITDIALOG.
		//virtual LSW_HANDLED					InitDialog();
	};

}	// namespace mx

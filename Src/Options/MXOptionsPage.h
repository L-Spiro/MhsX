#pragma once
#include "../MXMhsX.h"
#include "../Options/MXOptions.h"
#include <LSWWin.h>
#include <Widget/LSWWidget.h>

#include <string>

using namespace lsw;

namespace mx {

	class COptionsPage : public lsw::CWidget {
	public :
		COptionsPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Get the name of the page.
		virtual std::wstring					GetName() const { return std::wstring(); }

		// Verifies the options, returning an error string in case of error.
		virtual BOOL							Verify( std::wstring &_wsError, CWidget * &_pwWidget );

		// Copies all the settings to the MX_OPTIONS structure.
		virtual BOOL							Finalize();

		// WM_INITDIALOG.
		//virtual LSW_HANDLED					InitDialog();
	};

}	// namespace mx

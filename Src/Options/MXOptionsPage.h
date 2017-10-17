#pragma once
#include <LSWWin.h>
#include <Widget/LSWWidget.h>

#include <string>

using namespace lsw;

namespace mx {

	class COptionsPage : public lsw::CWidget {
	public :
		COptionsPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );


		// == Functions.
		// Get the name of the page.
		virtual std::wstring					GetName() const { return std::wstring(); }

		// WM_INITDIALOG.
		//virtual LSW_HANDLED					InitDialog();
	};

}	// namespace mx

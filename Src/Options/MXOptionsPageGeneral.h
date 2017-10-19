#pragma once
#include "../MXMhsX.h"
#include "../Options/MXOptions.h"
#include "MXOptionsPage.h"
#include <LSWWin.h>

using namespace lsw;

namespace mx {

	class COptionsPageGeneral : public COptionsPage {
	public :
		COptionsPageGeneral( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, MX_OPTIONS * _poOptions, bool _bCreateWidget = true, HMENU _hMenu = NULL );


		// == Functions.
		// Get the name of the page.
		virtual std::wstring					GetName() const;

		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();


	protected :
		// == Members.
		// Options.
		MX_OPTIONS *							m_poOptions;
	};

}	// namespace mx

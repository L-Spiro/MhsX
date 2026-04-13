#pragma once

#include "../MXMhsX.h"
#include "../Layouts/MXNumberInputLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	class CNumberInputDialog : public lsw::CMainWindow {
	public :
		CNumberInputDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CNumberInputDialog();


	protected :
		// A pointer to the options.
		CNumberInputLayout::MX_NUMBER_DIALOG_PARMS *				m_pndpParms;
	};

}	// namespace mx

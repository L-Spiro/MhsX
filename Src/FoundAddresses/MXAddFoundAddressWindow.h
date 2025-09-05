#pragma once

#include "../MXMhsX.h"
#include "../Layouts/MXAddFoundAddressWindowLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <MainWindow/LSWMainWindow.h>
#include <Tab/LSWTab.h>


using namespace lsw;

namespace mx {

	class CMemHack;

	class CAddFoundAddressWindow : public lsw::CMainWindow {
	public :
		CAddFoundAddressWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED									InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED									Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		// WM_CLOSE.
		virtual LSW_HANDLED									Close();

		// Updates the dialog after changing some parameters.
		void												Update();


	protected :
		// == Members.
		// Options.
		CMemHack *											m_pmhMemHack;


	private :
		typedef CAddFoundAddressWindowLayout				Layout;
		typedef lsw::CMainWindow							CParent;
	};

}	// namespace mx

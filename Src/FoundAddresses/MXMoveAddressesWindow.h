#pragma once

#include "../MXMhsX.h"
#include "../Layouts/MXMoveAddressesWindowLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <MainWindow/LSWMainWindow.h>
#include <Tab/LSWTab.h>


using namespace lsw;

namespace mx {

	class CWindowMemHack;

	class CMoveAddressWindow : public lsw::CMainWindow {
	public :
		CMoveAddressWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED									InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED									Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		// WM_CLOSE.
		virtual LSW_HANDLED									Close();


	protected :
		// == Members.
		// Options.
		CWindowMemHack *									m_pmhMemHack;


	private :
		typedef CMoveAddressesWindowLayout					Layout;
		typedef lsw::CMainWindow							CParent;
	};

}	// namespace mx

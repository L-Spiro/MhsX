#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXStringDecoder.h"
#include "MXFoundAddressEditMainPage.h"

#include <MainWindow/LSWMainWindow.h>
#include <Tab/LSWTab.h>


using namespace lsw;

namespace mx {

	class CMemHack;

	class CFoundAddressEditWindow : public lsw::CMainWindow {
	public :
		CFoundAddressEditWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Enumerations.
		// Pages.
		enum MX_PAGES : int32_t {
			MX_P_MAIN,
			MX_P_ADDRESS,
			MX_P_LOCK,
			MX_P_HOTKEYS,
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED									InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED									Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		// WM_CLOSE.
		virtual LSW_HANDLED									Close();


	protected :
		// == Members.
		// Settings.
		CFoundAddressEditPageBase::MX_PARMS					m_pParms;
		// Options pages.
		std::vector<CFoundAddressEditPageBase *>			m_vPages;


	private :
		typedef lsw::CMainWindow							CParent;
	};

}	// namespace mx

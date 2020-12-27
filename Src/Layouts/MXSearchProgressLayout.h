#pragma once

#include "../MXMhsX.h"
#include "../MemHack/MXMemHack.h"
#include "../Search/MXSearcher.h"
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CSearchProgressLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_SEARCH_PROG_WINDOW_IDS : WORD {
			MX_SPW_NONE,
			MX_SPW_MAINWINDOW					= 400,
			MX_SPW_PROGRESSBAR,
			MX_SPW_CANCEL,
		};

		
		// == Types.
		struct MX_SEARCH_DLG_PARMS {
			class CMhsMainWindow *				pmmwWindow;
			const CSearcher::MX_SEARCH_PARMS *	pspSearch;
		};


		// == Functions.
		// Creates the Open Process dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static BOOL								CreateSearchProgressDialog( CWidget * _pwParent, const MX_SEARCH_DLG_PARMS * _psdpParms );


	protected :
		// == Members.
		// The layout for the Search Progress dialog.
		static LSW_WIDGET_LAYOUT				m_wlSearchProgressDialog[];

	};


}	// namespace mx

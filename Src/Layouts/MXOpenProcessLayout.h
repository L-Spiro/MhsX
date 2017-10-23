#pragma once
#include "../MXMhsX.h"
#include "../Options/MXOptions.h"
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {
	
	class COpenProcessLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_OPEN_PROCESS_IDS {
			MX_OPI_NONE,
			MX_OPI_DIALOG,
			MX_OPI_GROUP,
			MX_OPI_GROUPSHOW,
			MX_OPI_LISTVIEW,
			MX_OPI_BUTTON_OK,
			MX_OPI_BUTTON_CANCEL,
			MX_OPI_RADIO_MAIN,
			MX_OPI_RADIO_ALL,
			MX_OPI_CHECK_RESTR,
			MX_OPI_BUTTON_REFRESH,
		};


		// == Functions.
		// Creates the Open Process dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static DWORD							CreateOpenProcessDialog( CWidget * _pwParent, MX_OPTIONS * _poOptions );


	protected :
		// == Members.
		// The layout for the Open Process dialog.
		static LSW_WIDGET_LAYOUT				m_wlOpenProcessDialog[];

	};

}	// namespace mx

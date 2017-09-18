#pragma once

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
			MX_OPI_LISTVIEW,
		};


		// == Functions.
		// Creates the Open Process dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static BOOL								CreateOpenProcessDialog( CWidget * _pwParent );


	protected :
		// == Members.
		// The layout for the Open Process dialog.
		static LSW_WIDGET_LAYOUT				m_wlOpenProcessDialog[];

	};

}	// namespace mx

#pragma once

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CExpressionSubsearchLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_CONTROL_IDS {
			MX_C_NONE,
			MX_C_MAINWINDOW						= 5300,
		};


		// == Functions.
		// Creates the Template window.
		static CWidget *						CreateTemplateWindow( CWidget * _pwParent );


	protected :
		// == Members.
		// The layout for the window.
		static LSW_WIDGET_LAYOUT				m_wlMainWindow[];
	};

}	// namespace mx

#pragma once

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {
	
	//
	class CMainWindowLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_MAIN_WINDOW_IDS {
			MX_MWI_NONE,
			MX_MWI_MAINWINDOW,
			MX_MWI_STOREDADDRESSES,
			
		};


		// == Functions.
		// Creates the main window.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc., and registers the main window class.
		static BOOL								CreateMainWindow();

		// Gets the main window widget.
		static CWidget *						MainWindow() { return m_pwMainWindow; }


	protected :
		// == Members.
		// The layout for the main window.
		static LSW_WIDGET_LAYOUT				m_wlMainWindow[];

		// The class for the main window.
		static ATOM								m_aMainClass;

		// The main window.
		static CWidget *						m_pwMainWindow;

	};

}	// namespace mx

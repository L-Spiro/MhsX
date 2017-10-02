#pragma once

#include <Layout/LSWMenuLayout.h>
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
			MX_MWI_REBAR0,
			MX_MWI_TOOLBAR0,
			
		};

		// Menu items
		enum MX_MAINWINDOW_MENU_IDS {
			MX_MWMI_MENU_BAR					= 300,
			MX_MWMI_MENU_FILE,
			MX_MWMI_MENU_SEARCH,
			MX_MWMI_MENU_TOOLS,
			MX_MWMI_MENU_WINDOW,
			MX_MWMI_MENU_HELP,

			MX_MWMI_FILE						= 100,
			MX_MWMI_SEARCH,
			MX_MWMI_TOOLS,
			MX_MWMI_WINDOW,
			MX_MWMI_HELP,

			MX_MWMI_OPENPROCESS,
			MX_MWMI_OPENRECENT,
			MX_MWMI_OPENFORDEBUG,
			MX_MWMI_OPENSAVEFILE,
			MX_MWMI_SAVE,
			MX_MWMI_SAVEAS,

			MX_MWMI_DATATYPE,
			MX_MWMI_POINTER,
			MX_MWMI_STRING,
			MX_MWMI_GROUP,
			MX_MWMI_EXPRESSION,
			MX_MWMI_SUB,
			MX_MWMI_INSERT,
			MX_MWMI_SEARCHOPTIONS,

			MX_MWMI_DATATYPESEARCH,
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

		// Menu bar items for the main window.
		static LSW_MENU_ITEM					m_miMenuBar[];

		// File menu.
		static LSW_MENU_ITEM					m_miFileMenu[];

		// Search menu.
		static LSW_MENU_ITEM					m_miSearchMenu[];

		// Menus.
		static LSW_MENU_LAYOUT					m_miMenus[];

		// The class for the main window.
		static ATOM								m_aMainClass;

		// The main window.
		static CWidget *						m_pwMainWindow;

	};

}	// namespace mx

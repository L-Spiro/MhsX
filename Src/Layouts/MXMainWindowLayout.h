#pragma once

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CMemHack;
	
	// Layout for the main MHS window.
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
			MX_MWI_STATUSBAR,
			MX_MWI_SPLITTER,

			MX_MWI_TEST0,
			MX_MWI_TEST1,
			MX_MWI_TEST2,
			MX_MWI_TEST3,
			
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
			MX_MWMI_ADDENTRY,
			MX_MWMI_OPENSAVEFILE,
			MX_MWMI_SAVE,
			MX_MWMI_SAVEAS,
			MX_MWMI_SAVE_SELECTED_AS,
			MX_MWMI_DELETE,
			MX_MWMI_DELETEALL,
			MX_MWMI_LOCK,
			MX_MWMI_UNLOCK,
			MX_MWMI_UNLOCK_ALL,
			MX_MWMI_LOCK_ONCE,
			MX_MWMI_EDIT,
			MX_MWMI_MOVE_UP,
			MX_MWMI_MOVE_DOWN,
			MX_MWMI_SET_MOVE_ADDRESS_SOURCE,
			MX_MWMI_MOVE_ADDRESS,
			MX_MWMI_COPY_ADDRESS,
			MX_MWMI_EXIT,

			MX_MWMI_DATATYPE,
			MX_MWMI_POINTER,
			MX_MWMI_STRING,
			MX_MWMI_GROUP,
			MX_MWMI_EXPRESSION,
			MX_MWMI_SUB,
			//MX_MWMI_INSERT,  MX_MWMI_ADDENTRY
			MX_MWMI_SEARCHOPTIONS,

			MX_MWMI_DATATYPESEARCH,

			MX_MWMI_OPTIONS,
			MX_MWMI_PEWORKS,
			MX_MWMI_STRINGTHEORY,
			MX_MWMI_FLOATINGPOINTSTUDIO,
			MX_MWMI_DEUS_HEX_MACHINA,

			MX_MWMI_SHOWFOUNDADDR,
			MX_MWMI_SHOW_EXPEVAL,
			MX_MWMI_SHOW_CONVERTER,
			MX_MWMI_SHOW_ALL,


			MX_MWMI_USER_PROGRAMS_BASE			= 1000,
			MX_MWMI_SHOW_RECENT_BASE			= 2000,
		};


		// == Functions.
		// Creates the main window.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc., and registers the main window class.
		static CWidget *						CreateMainWindow( CMemHack * _pmhMemHack );

		// Creates the main menu and adds it to the window.
		static BOOL								CreateMenu( CWidget * _pwMainWindow );

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

		// Options menu.
		static LSW_MENU_ITEM					m_miOptionsMenu[];

		// Windows menu.
		static LSW_MENU_ITEM					m_miWindowsMenu[];

		// Menus.
		static LSW_MENU_LAYOUT					m_miMenus[];

		// The class for the main window.
		static ATOM								m_aMainClass;

		// The main window.
		static CWidget *						m_pwMainWindow;

	};

}	// namespace mx

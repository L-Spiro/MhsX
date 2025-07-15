#pragma once

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CPeWorksLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_EXP_EVAL_WINDOW_IDS {
			MX_PEW_NONE,
			MX_PEW_MAINWINDOW					= 3200,
			MX_PEW_STATUSBAR,
			MX_PEW_TOOLBAR0,
			MX_PEW_REBAR0,
			MX_PEW_TABS,

			MX_PEW_TAB_TREELIST_START			= 3400,
		};

		// Menu items.
		enum MX_MAINWINDOW_MENU_IDS {
			MX_PEW_MENU_BAR						= 780,
			MX_PEW_MENU_FILE,

			MX_PEW_SEARCH,
			MX_PEW_TOOLS,
			MX_PEW_WINDOW,
			MX_PEW_HELP,

			MX_PEW_FILE							= 800,
			MX_PEW_OPENFILE,
			MX_PEW_OPENPE,
			MX_PEW_OPENRECENT,

			MX_PEW_COPY_FIELD,
			MX_PEW_COPY_VALUE,
			MX_PEW_COPY_OFFSET,
			MX_PEW_COPY_BYTES,
			MX_PEW_COPY_DESC,
			MX_PEW_COPY_ALL,
			MX_PEW_COPY_FIELD_VALUE,
			MX_PEW_COPY_EXPAND_SELECTED,
			MX_PEW_COPY_EXPAND_ALL,
			MX_PEW_COPY_COLLAPSE_SELECTED,
			MX_PEW_COPY_COLLAPSE_ALL
			
		};


		// == Functions.
		// Creates the PE Works window.
		static CWidget *						CreatePeWorksWindow( CWidget * _pwParent );


	protected :
		// == Members.
		// The layout for the PE Works window.
		static LSW_WIDGET_LAYOUT				m_wlMainWindow[];

		// Menu bar items for the main window.
		static LSW_MENU_ITEM					m_miMenuBar[];

		// File menu.
		static LSW_MENU_ITEM					m_miFileMenu[];

		// Menus.
		static LSW_MENU_LAYOUT					m_miMenus[];
	};

}	// namespace mx

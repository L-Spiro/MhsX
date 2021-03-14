#pragma once

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {
	
	// Layout for the Found Addresses dockable window.
	class CFoundAddressLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_FOUND_ADDRESSES_WINDOW_IDS {
			MX_FAW_NONE,
			MX_FAW_MAINWINDOW					= 2000,
			MX_FAW_REBAR0,
			MX_FAW_TOOLBAR0,
			MX_FAW_PADDING_EDIT_LEFT,
			MX_FAW_PADDINGLEFT,
			MX_FAW_PADDING_EDIT_RIGHT,
			MX_FAW_PADDINGRIGHT,
			MX_FAW_LIST,
		};

		// Buttons.
		enum MX_BUTTON_COMMANDS {
			MX_BC_ADD							= 1010,
			MX_BC_SEARCH,
			MX_BC_SUBSEARCH,
			MX_BC_PREFIX_BYTES,
			MX_BC_POSTFIX_BYTES,
			MX_BC_COPY_ADDRESS,
			MX_BC_COPY_VALUE,
			MX_BC_COPY_CUR_VALUE,
			MX_BC_COPY_ALL,
		};


		// == Functions.
		// Creates the Found Addresses dockable window.
		static CWidget *						CreateFoundAddressesWindow( CWidget * _pwParent, class CMhsMainWindow * _pmmwMhsWindow );



	protected :
		// == Members.
		// The layout for the Found Addresses dockable window.
		static LSW_WIDGET_LAYOUT				m_wlMainWindow[];
	};

}	// namespace mx

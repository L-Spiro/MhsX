#pragma once

#include "../LSWWin.h"

namespace lsw {

	enum LSW_LAYOUTTYPES : DWORD {
		LSW_LT_WIDGET,
		LSW_LT_BUTTON,							// WC_BUTTONW
		LSW_LT_CHECK,							// WC_BUTTONW
		LSW_LT_CHILDWINDOW,						// lsw::CBase::ChildWindowAtom()
		LSW_LT_COMBOBOX,						// WC_COMBOBOXW
		LSW_LT_COMBOBOXEX,						// WC_COMBOBOXEXW
		LSW_LT_DOCKWINDOW,						// lsw::CBase::DockableAtom()
		LSW_LT_EDIT,							// WC_EDITW
		LSW_LT_GROUPBOX,						// WC_BUTTONW
		LSW_LT_LABEL,							// WC_STATICW
		LSW_LT_LISTBOX,							// WC_LISTBOXW
		LSW_LT_LISTVIEW,						// WC_LISTVIEWW
		LSW_LT_MULTISPLITTER,					// lsw::CBase::MultiSplitterAtom()
		LSW_LT_PROGRESSBAR,						// PROGRESS_CLASS
		LSW_LT_RADIO,							// WC_BUTTONW
		LSW_LT_REBAR,							// REBARCLASSNAMEW
		LSW_LT_SPLITTER,						// lsw::CBase::SplitterAtom()
		LSW_LT_STATUSBAR,						// STATUSCLASSNAMEW
		LSW_LT_TAB,								// WC_TABCONTROLW
		LSW_LT_TOOLBAR,							// TOOLBARCLASSNAMEW
		LSW_LT_TOOLTIP,							// TOOLTIPS_CLASS
		LSW_LT_TRACKBAR,						// TRACKBAR_CLASS
		LSW_LT_TREELIST,						// lsw::CBase::TreeListAtom()
		LSW_LT_TREELISTVIEW,					// lsw::CBase::TreeListViewAtom()
		LSW_LT_TREEVIEW,						// WC_TREEVIEWW
		LSW_LT_UPDOWN,							// UPDOWN_CLASSW
		LSW_LT_MAINWINDOW,

		LSW_LT_CUSTOM = 100,
	};

}	// namespace lsw

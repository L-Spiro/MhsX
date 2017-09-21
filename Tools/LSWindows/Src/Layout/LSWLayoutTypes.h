#pragma once

#include "../LSWWin.h"

namespace lsw {

	enum LSW_LAYOUTTYPES : DWORD {
		LSW_LT_WIDGET,
		LSW_LT_BUTTON,
		LSW_LT_RADIO,
		LSW_LT_CHECK,
		LSW_LT_LISTVIEW,
		LSW_LT_TREEVIEW,
		LSW_LT_GROUPBOX,
		LSW_LT_LABEL,
		LSW_LT_MAINWINDOW,

		LSW_LT_CUSTOM = 100,
	};

}	// namespace lsw

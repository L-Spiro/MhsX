#pragma once

#include "../LSWWin.h"

namespace lsw {

	struct LSW_MENU_ITEM {
		BOOL						bIsSeperator;				// If true, no other flags are used.
		DWORD						dwId;						// Unique ID for the menu item, must not be 0.
		BOOL						bCheckable;
		BOOL						bChecked;
		BOOL						bEnabled;
		// Icon.
		LPCWSTR						lpwcText;
		// Shortcut.

	};

	struct LSW_MENU_LAYOUT {
		DWORD						dwId;						// Unique ID for the menu item, must not be 0.
		DWORD						dwParentMenuId;
		DWORD						dwParentMenuItemId;
		// Shortcut.
		SIZE_T						stTotalMenuItems;
		LSW_MENU_ITEM *				pmiItems;
	};

}	// namespace lsw

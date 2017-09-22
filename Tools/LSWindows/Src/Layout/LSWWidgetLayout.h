#pragma once

#include "../LSWWin.h"
#include "LSWLayoutTypes.h"

namespace lsw {

	struct LSW_WIDGET_LAYOUT {
		WORD						ltType;
		WORD						wId;						// Unique ID for the control, must not be 0.
		LPCWSTR						lpwcClass;					// Only needed for custom types.  List views, buttons, etc. already know their classes.
		BOOL						bEnabled;
		INT							iLeft;
		INT							iTop;
		DWORD						dwWidth;
		DWORD						dwHeight;
		DWORD						dwStyle;
		DWORD						dwStyleEx;
		LPCWCH						pwcText;
		DWORD						dwParentId;					// 0 for no parent.

		// Expressions for dynamic sizing.
		LPCSTR						pcLeftSizeExp;
		LPCSTR						pcRightSizeExp;
		LPCSTR						pcTopSizeExp;
		LPCSTR						pcBottomSizeExp;
		LPCSTR						pcWidthSizeExp;
		LPCSTR						pcHeightSizeExp;
	};

}	// namespace lsw

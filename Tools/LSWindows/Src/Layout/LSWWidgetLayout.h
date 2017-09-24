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
		SIZE_T						sTextLen;
		DWORD						dwParentId;					// 0 for no parent.

		// Expressions for dynamic sizing.
		LPCSTR						pcLeftSizeExp;
		SIZE_T						sLeftSizeExpLen;
		LPCSTR						pcRightSizeExp;
		SIZE_T						sRightSizeExpLen;
		LPCSTR						pcTopSizeExp;
		SIZE_T						sTopSizeExpLen;
		LPCSTR						pcBottomSizeExp;
		SIZE_T						sBottomSizeExpLen;
		LPCSTR						pcWidthSizeExp;
		SIZE_T						sWidthSizeExpLen;
		LPCSTR						pcHeightSizeExp;
		SIZE_T						sHeightSizeExpLen;
	};

}	// namespace lsw

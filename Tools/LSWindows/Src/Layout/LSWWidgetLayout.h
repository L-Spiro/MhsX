#pragma once

#include "../LSWWin.h"
#include "LSWLayoutTypes.h"

namespace lsw {

	struct LSW_WIDGET_LAYOUT {
		LSW_LAYOUTTYPES				ltType;
		BOOL						bEnabled;
		INT							iLeft;
		INT							iRight;
		DWORD						dwWidth;
		DWORD						dwHeight;
		LPCWCH						pwcText;
		LSW_WIDGET_LAYOUT *			pwlParent;
	};

}	// namespace lsw

#pragma once

#include "../LSWWin.h"

namespace lsw {

	struct LSW_RECT : public RECT {
		LONG					Width() const { return right - left; }
		LONG					Height() const { return bottom - top; }
		VOID					SetWidth( LONG _lW ) { right = left + _lW; }
		VOID					SetHeight( LONG _lH ) { bottom = top + _lH; }
		POINT					UpperLeft() const { return { left, top }; }
		POINT					BottomRight() const { return { right, bottom }; }
	};

}	// namespace lsw

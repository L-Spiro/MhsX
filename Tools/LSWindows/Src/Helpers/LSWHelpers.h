#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"

namespace lsw {

	struct LSW_RECT : public RECT {
		LSW_RECT() {}
		LSW_RECT( const RECT &_rRect ) { 
			left = _rRect.left;
			right = _rRect.right;
			top = _rRect.top;
			bottom = _rRect.bottom;
		}
		LONG								Width() const { return right - left; }
		LONG								Height() const { return bottom - top; }
		VOID								SetWidth( LONG _lW ) { right = left + _lW; }
		VOID								SetHeight( LONG _lH ) { bottom = top + _lH; }
		POINT								UpperLeft() const { return { left, top }; }
		POINT								BottomRight() const { return { right, bottom }; }
	};

	class CHelpers {
	public :
		// Aligns a WORD pointer to a 4-byte address.
		static LPWORD						DwordAlign( LPWORD _lpIn ) {
			ULONG_PTR ulVal;

			ulVal = reinterpret_cast<ULONG_PTR>(_lpIn);
			ulVal++;
			ulVal >>= 1;
			ulVal <<= 1;
			return reinterpret_cast<LPWORD>(ulVal);
		}

		// Finds a layout by its ID.
		static LSW_WIDGET_LAYOUT *			FindLayout( LSW_WIDGET_LAYOUT * _pwlLayouts, size_t _sTotal, DWORD _dwId ) {
			for ( size_t I = 0; I < _sTotal; ++I ) {
				if ( _pwlLayouts[I].dwId == _dwId ) { return &_pwlLayouts[I]; }
			}
			return nullptr;
		}
	};


}	// namespace lsw

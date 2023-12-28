#include "LSWTrackBar.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CTrackBar::CTrackBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	// == Functions.
	// Sets the trackbar range.
	void CTrackBar::SetRange( BOOL _bRedraw, WORD _wLow, WORD _wHigh ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_SETRANGE, static_cast<WPARAM>(_bRedraw), MAKELPARAM( _wLow, _wHigh ) );
		}
	}

	// Sets the minimum range value.
	void CTrackBar::SetRangeMin( BOOL _bRedraw, DWORD _dwMin ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_SETRANGEMIN, static_cast<WPARAM>(_bRedraw), static_cast<LPARAM>(_dwMin) );
		}
	}

	// Sets the maximum range value.
	void CTrackBar::SetRangeMax( BOOL _bRedraw, DWORD _dwMax ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_SETRANGEMAX, static_cast<WPARAM>(_bRedraw), static_cast<LPARAM>(_dwMax) );
		}
	}

	// Gets the position of the trackbar.
	uint32_t CTrackBar::GetPos() const {
		if ( Wnd() ) {
			return static_cast<uint32_t>(::SendMessageW( Wnd(), TBM_GETPOS, 0, 0 ));
		}
		return 0;
	}

	// Sets the position of the trackbar knob.
	void CTrackBar::SetPos( BOOL _bRedraw, LPARAM _lpPos ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_SETPOS, static_cast<WPARAM>(_bRedraw), _lpPos );
		}
	}

	// Sets the tic frequency.
	void CTrackBar::SetTicFreq( DWORD _dwFreq ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_SETTICFREQ, static_cast<WPARAM>(_dwFreq), 0 );
		}
	}

	// Sets a tick mark in a trackbar at the specified logical position.
	BOOL CTrackBar::SetTic( DWORD _dwPos ) {
		if ( Wnd() ) {
			return static_cast<BOOL>(::SendMessageW( Wnd(), TBM_SETTIC, 0, static_cast<LPARAM>(_dwPos) ));
		}
		return FALSE;
	}

	// Retrieves the logical position of a tick mark in a trackbar. The logical position can be any of the integer values in the trackbar's range of minimum to maximum slider positions.
	DWORD CTrackBar::GetTic( WPARAM _wpIndex ) const {
		if ( Wnd() ) {
			return static_cast<DWORD>(::SendMessageW( Wnd(), TBM_GETTIC, _wpIndex, 0 ));
		}
		return 0;
	}

	// Retrieves the address of an array that contains the positions of the tick marks for a trackbar.
	// The number of elements in the array is two less than the tick count returned by GetNumTics(). Note that the values in the array may include duplicate positions and may not be in sequential order. The returned pointer is valid until you change the trackbar's tick marks.
	PDWORD CTrackBar::GetPTics() const {
		if ( Wnd() ) {
			return reinterpret_cast<PDWORD>(::SendMessageW( Wnd(), TBM_GETPTICS, 0, 0 ));
		}
		return nullptr;
	}

	// Retrieves the number of tick marks in a trackbar.
	// If no tick flag is set, it returns 2 for the beginning and ending ticks. If TBS_NOTICKS is set, it returns zero. Otherwise, it takes the difference between the range minimum and maximum, divides by the tick frequency, and adds 2.
	WPARAM CTrackBar::GetNumTics() const {
		if ( Wnd() ) {
			return static_cast<WPARAM>(::SendMessageW( Wnd(), TBM_GETNUMTICS, 0, 0 ));
		}
		return 0;
	}

	// Retrieves the current physical position of a tick mark in a trackbar.
	// Returns the distance, in client coordinates, from the left or top of the trackbar's client area to the specified tick mark. The return value is the x-coordinate of the tick mark for a horizontal trackbar or the y-coordinate for a vertical trackbar. If wParam is not a valid index, the return value is -1.
	// Because the first and last tick marks are not available through this message, valid indexes are offset from their tick position on the trackbar. If the difference between TBM_GETRANGEMIN and TBM_GETRANGEMAX is less than two, then there is no valid index and this message will fail.
	INT CTrackBar::GetTicPos( WPARAM _wpIndex ) const {
		if ( Wnd() ) {
			return static_cast<INT>(::SendMessageW( Wnd(), TBM_GETTICPOS, _wpIndex, 0 ));
		}
		return -1;
	}

	// Removes the current tick marks from a trackbar. This message does not remove the first and last tick marks, which are created automatically by the trackbar.
	void CTrackBar::ClearTics( BOOL _bRedraw ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_CLEARTICS, static_cast<WPARAM>(_bRedraw), 0 );
		}
	}

	// Retrieves the number of logical positions the trackbar's slider moves in response to keyboard input from the arrow keys, such as the or keys. The logical positions are the integer increments in the trackbar's range of minimum to maximum slider positions.
	DWORD CTrackBar::GetLineSize() const {
		if ( Wnd() ) {
			return static_cast<DWORD>(::SendMessageW( Wnd(), TBM_GETLINESIZE, 0, 0 ));
		}
		return 0;
	}

	// Sets the number of logical positions the trackbar's slider moves in response to keyboard input from the arrow keys, such as the or keys. The logical positions are the integer increments in the trackbar's range of minimum to maximum slider positions.
	// Returns a 32-bit value that specifies the previous line size.
	DWORD CTrackBar::SetLineSize( DWORD _dwSize ) {
		if ( Wnd() ) {
			return static_cast<DWORD>(::SendMessageW( Wnd(), TBM_SETLINESIZE, 0, static_cast<LPARAM>(_dwSize) ));
		}
		return 0;
	}

	// Retrieves the number of logical positions the trackbar's slider moves in response to keyboard input, such as the or keys, or mouse input, such as clicks in the trackbar's channel. The logical positions are the integer increments in the trackbar's range of minimum to maximum slider positions.
	DWORD CTrackBar::GetPageSize() const {
		if ( Wnd() ) {
			return static_cast<DWORD>(::SendMessageW( Wnd(), TBM_GETPAGESIZE, 0, 0 ));
		}
		return 0;
	}

	// Sets the number of logical positions the trackbar's slider moves in response to keyboard input, such as the or keys, or mouse input, such as clicks in the trackbar's channel. The logical positions are the integer increments in the trackbar's range of minimum to maximum slider positions.
	// Returns a 32-bit value that specifies the previous page size.
	DWORD CTrackBar::SetPageSize( DWORD _dwSize ) {
		if ( Wnd() ) {
			return static_cast<DWORD>(::SendMessageW( Wnd(), TBM_SETPAGESIZE, 0, static_cast<LPARAM>(_dwSize) ));
		}
		return 0;
	}

	// Retrieves the size and position of the bounding rectangle for the slider in a trackbar.
	void CTrackBar::GetThumbRect( RECT & _rRect ) const {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_GETTHUMBRECT, 0, reinterpret_cast<LPARAM>(&_rRect) );
		}
		else {
			reinterpret_cast<LSW_RECT *>(&_rRect)->Zero();
		}
	}

	// Retrieves the length, in pixels, of the slider in a trackbar.
	DWORD CTrackBar::GetThumbLength() const {
		if ( Wnd() ) {
			return static_cast<DWORD>(::SendMessageW( Wnd(), TBM_GETTHUMBLENGTH, 0, 0 ));
		}
		return 0;
	}

	// Sets the length, in pixels, of the slider in a trackbar. This message is ignored if the trackbar does not have the TBS_FIXEDLENGTH style.
	void CTrackBar::SetThumbLength( DWORD _dwLen ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_SETTHUMBLENGTH, static_cast<WPARAM>(_dwLen), 0 );
		}
	}

	// Retrieves the size and position of the bounding rectangle for a trackbar's channel. (The channel is the area over which the slider moves. It contains the highlight when a range is selected.)
	void CTrackBar::GetChannelRect( RECT & _rRect ) const {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_GETCHANNELRECT, 0, reinterpret_cast<LPARAM>(&_rRect) );
		}
		else {
			reinterpret_cast<LSW_RECT *>(&_rRect)->Zero();
		}
	}

	// Sets the starting and ending positions for the available selection range in a trackbar.
	void CTrackBar::SetSel( BOOL _bRedraw, WORD _wLow, WORD _wHigh ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_SETSEL, static_cast<WPARAM>(_bRedraw), MAKELPARAM( _wLow, _wHigh ) );
		}
	}

	// Sets the starting logical position of the current selection range in a trackbar. This message is ignored if the trackbar does not have the TBS_ENABLESELRANGE style.
	void CTrackBar::SetSelStart( BOOL _bRedraw, DWORD _dwStart ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_SETSELSTART, static_cast<WPARAM>(_bRedraw), static_cast<LPARAM>(_dwStart) );
		}
	}

	// Sets the ending logical position of the current selection range in a trackbar. This message is ignored if the trackbar does not have the TBS_ENABLESELRANGE style.
	void CTrackBar::SetSelEnd( BOOL _bRedraw, DWORD _dwEnd ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_SETSELEND, static_cast<WPARAM>(_bRedraw), static_cast<LPARAM>(_dwEnd) );
		}
	}

	// Retrieves the starting position of the current selection range in a trackbar.
	DWORD CTrackBar::GetSelStart() const {
		if ( Wnd() ) {
			return static_cast<DWORD>(::SendMessageW( Wnd(), TBM_GETSELSTART, 0, 0 ));
		}
		return 0;
	}

	// Retrieves the ending position of the current selection range in a trackbar.
	DWORD CTrackBar::GetSelEnd() const {
		if ( Wnd() ) {
			return static_cast<DWORD>(::SendMessageW( Wnd(), TBM_GETSELEND, 0, 0 ));
		}
		return 0;
	}

	// Clears the current selection range in a trackbar.
	void CTrackBar::ClearSel( BOOL _bRedraw ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), TBM_CLEARSEL, static_cast<WPARAM>(_bRedraw), 0 );
		}
	}

}	// namespace lsw

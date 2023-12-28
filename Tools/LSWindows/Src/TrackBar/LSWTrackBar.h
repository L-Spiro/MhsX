#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CTrackBar : public CWidget {
	public :
		CTrackBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_TRACKBAR; }

		// Sets the trackbar range.
		virtual void						SetRange( BOOL _bRedraw, WORD _wLow, WORD _wHigh );

		// Sets the minimum range value.
		virtual void						SetRangeMin( BOOL _bRedraw, DWORD _dwMin );

		// Sets the maximum range value.
		virtual void						SetRangeMax( BOOL _bRedraw, DWORD _dwMax );

		// Gets the position of the trackbar.
		virtual uint32_t					GetPos() const;

		// Sets the position of the trackbar knob.
		virtual void						SetPos( BOOL _bRedraw, LPARAM _lpPos );

		// Sets the tic frequency.
		virtual void						SetTicFreq( DWORD _dwFreq );

		// Sets a tick mark in a trackbar at the specified logical position.
		virtual BOOL						SetTic( DWORD _dwPos );

		// Retrieves the logical position of a tick mark in a trackbar. The logical position can be any of the integer values in the trackbar's range of minimum to maximum slider positions.
		virtual DWORD						GetTic( WPARAM _wpIndex ) const;

		// Retrieves the address of an array that contains the positions of the tick marks for a trackbar.
		// The number of elements in the array is two less than the tick count returned by GetNumTics(). Note that the values in the array may include duplicate positions and may not be in sequential order. The returned pointer is valid until you change the trackbar's tick marks.
		virtual PDWORD						GetPTics() const;

		// Retrieves the number of tick marks in a trackbar.
		// If no tick flag is set, it returns 2 for the beginning and ending ticks. If TBS_NOTICKS is set, it returns zero. Otherwise, it takes the difference between the range minimum and maximum, divides by the tick frequency, and adds 2.
		virtual WPARAM						GetNumTics() const;

		// Retrieves the current physical position of a tick mark in a trackbar.
		// Returns the distance, in client coordinates, from the left or top of the trackbar's client area to the specified tick mark. The return value is the x-coordinate of the tick mark for a horizontal trackbar or the y-coordinate for a vertical trackbar. If wParam is not a valid index, the return value is -1.
		// Because the first and last tick marks are not available through this message, valid indexes are offset from their tick position on the trackbar. If the difference between TBM_GETRANGEMIN and TBM_GETRANGEMAX is less than two, then there is no valid index and this message will fail.
		virtual INT							GetTicPos( WPARAM _wpIndex ) const;

		// Removes the current tick marks from a trackbar. This message does not remove the first and last tick marks, which are created automatically by the trackbar.
		virtual void						ClearTics( BOOL _bRedraw );

		// Retrieves the number of logical positions the trackbar's slider moves in response to keyboard input from the arrow keys, such as the or keys. The logical positions are the integer increments in the trackbar's range of minimum to maximum slider positions.
		virtual DWORD						GetLineSize() const;

		// Sets the number of logical positions the trackbar's slider moves in response to keyboard input from the arrow keys, such as the or keys. The logical positions are the integer increments in the trackbar's range of minimum to maximum slider positions.
		// Returns a 32-bit value that specifies the previous line size.
		virtual DWORD						SetLineSize( DWORD _dwSize );

		// Retrieves the number of logical positions the trackbar's slider moves in response to keyboard input, such as the or keys, or mouse input, such as clicks in the trackbar's channel. The logical positions are the integer increments in the trackbar's range of minimum to maximum slider positions.
		virtual DWORD						GetPageSize() const;

		// Sets the number of logical positions the trackbar's slider moves in response to keyboard input, such as the or keys, or mouse input, such as clicks in the trackbar's channel. The logical positions are the integer increments in the trackbar's range of minimum to maximum slider positions.
		// Returns a 32-bit value that specifies the previous page size.
		virtual DWORD						SetPageSize( DWORD _dwSize );

		// Retrieves the size and position of the bounding rectangle for the slider in a trackbar.
		virtual void						GetThumbRect( RECT & _rRect ) const;

		// Retrieves the length, in pixels, of the slider in a trackbar.
		virtual DWORD						GetThumbLength() const;

		// Sets the length, in pixels, of the slider in a trackbar. This message is ignored if the trackbar does not have the TBS_FIXEDLENGTH style.
		virtual void						SetThumbLength( DWORD _dwLen );

		// Retrieves the size and position of the bounding rectangle for a trackbar's channel. (The channel is the area over which the slider moves. It contains the highlight when a range is selected.)
		virtual void						GetChannelRect( RECT & _rRect ) const;

		// Sets the starting and ending positions for the available selection range in a trackbar.
		virtual void						SetSel( BOOL _bRedraw, WORD _wLow, WORD _wHigh );

		// Sets the starting logical position of the current selection range in a trackbar. This message is ignored if the trackbar does not have the TBS_ENABLESELRANGE style.
		virtual void						SetSelStart( BOOL _bRedraw, DWORD _dwStart );

		// Sets the ending logical position of the current selection range in a trackbar. This message is ignored if the trackbar does not have the TBS_ENABLESELRANGE style.
		virtual void						SetSelEnd( BOOL _bRedraw, DWORD _dwEnd );

		// Retrieves the starting position of the current selection range in a trackbar.
		virtual DWORD						GetSelStart() const;

		// Retrieves the ending position of the current selection range in a trackbar.
		virtual DWORD						GetSelEnd() const;

		// Clears the current selection range in a trackbar.
		virtual void						ClearSel( BOOL _bRedraw );

	protected :
		// == Functions.
		// Setting the HWND after the control has been created.
		//virtual void						InitControl( HWND _hWnd );

	private :
		typedef CWidget						Parent;
	};
		
}	// namespace lsw

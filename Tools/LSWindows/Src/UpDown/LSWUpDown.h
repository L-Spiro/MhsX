#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CUpDown : public CWidget {
	public :
		CUpDown( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );



	protected :
		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

		// Gets the radix base (10 or 16).
		virtual uint32_t					GetBase() const;

		// Gets the position.
		virtual int32_t						GetPos() const;

		// Gets the buddy control.
		virtual CWidget *					GetBuddy() const;

		// Gets the range.
		virtual void						GetRange( int32_t &_i32Lower, int32_t &_i32Upper ) const;

		// Determines if the control is using Unicode or not.
		virtual BOOL						GetUnicodeFormat() const;

		// Sets the base radic (10 or 16) and returns the previous radix.
		virtual uint32_t					SetBase( uint32_t _ui32Base );

		// Sets the buddy control and returns the previous buddy control.
		virtual CWidget *					SetBuddy( CWidget * _pwBuddy );

		// Sets the position and returns the previous position.
		virtual int32_t						SetPos( int32_t _i32NewPos );

		// Sets the range.
		virtual void						SetRange( int32_t _i32Lower, int32_t _i32Upper );

		// Sets to use Unicode (TRUE) or ASCII (FALSE) and returns the previous Unicode setting.
		virtual BOOL						SetUnicodeFormat( BOOL _bUseUnicode );

		// Sort routine.
		//static int CALLBACK					CompareFunc( LPARAM _lParam1, LPARAM _lParam2, LPARAM _lParamSort );

	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw

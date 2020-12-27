#pragma once

#include "../LSWWin.h"
#include "../ImageList/LSWImageList.h"
#include "../Widget/LSWWidget.h"

#include <vector>

namespace lsw {

	class CToolBar : public CWidget {
	public :
		CToolBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Sets the image list.
		HIMAGELIST							SetImageList( INT _iIndex, CImageList &_ilList );

		// Checks a button.
		BOOL								CheckButton( WORD _wId, BOOL _bChecked );

		// Auto-sizes.
		VOID								AutoSize();

		// Determines if a button is checked.
		BOOL								IsChecked( WORD _wId ) const;

		// Gets the button count.
		DWORD								GetButtonCount() const;

		// Deletes a button.
		BOOL								DeleteButton( DWORD _dwIndex );

		// Enables or disables a button.
		BOOL								EnableButton( WORD _wId, BOOL _bEnabled );

		// Gets a button's data.
		BOOL								GetButton( DWORD _dwIndex, TBBUTTON &_tButton );

		// Returns a DWORD value that contains the width and height values in the low word and high word, respectively.
		DWORD								GetButtonSize() const;

		// Gets the button text length in wchar_t units, including the NULL character.
		DWORD								GetButtonTextLenW( WORD _wId ) const;

		// Gets the button text length in char units, including the NULL character.
		DWORD								GetButtonTextLenA( WORD _wId ) const;

		// Gets the button text.  _pwcBuffer should be at least GetButtonTextLenW() characters in length.
		DWORD								GetButtonTextW( WORD _wId, WCHAR * _pwcBuffer ) const;

		// Gets the button text.  _pcBuffer should be at least GetButtonTextLenA() characters in length.
		DWORD								GetButtonTextA( WORD _wId, CHAR * _pcBuffer ) const;

		// Gets a button's rectangle.
		LSW_RECT							GetButtonRect( DWORD _dwIndex ) const;

		// Gets a rectangle large enough to fit any of the buttons inside.
		LSW_RECT							GetMinBoundingRect() const;

		// Adds buttons.
		BOOL								AddButtons( const TBBUTTON * _pbButtons, size_t _sTotal );

		// TBN_QUERYINSERT.
		virtual LSW_HANDLED					TbnQueryInsert( const LPNMTOOLBARW _lptbToolBar );

		// TBN_QUERYDELETE.
		virtual LSW_HANDLED					TbnQueryDelete( const LPNMTOOLBARW _lptbToolBar );

		// TBN_RESET.
		virtual LSW_HANDLED					TbnReset();

		// TBN_GETBUTTONINFO.
		virtual LSW_HANDLED					TbnGetButtonInfo( LPNMTOOLBARW _lptbToolBar );

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_TOOLBAR; }

		// Returns true if this is a CToolBar class.
		virtual bool						IsToolBar() const { return true; }


	protected :
		// == Members.
		// Copy of the buttons for TBN_GETBUTTONINFO.
		// TODO: Button text should be copied into a std::wstring or else pointers here can
		//	point to invalid addresses.
		std::vector<TBBUTTON>				m_vButtons;


		// == Functions.
		// Gets the index of a button given its command ID.
		size_t								IndexOfButton( WORD _wId ) const;


	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw

#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CListBox : public CWidget {
	public :
		CListBox( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Adds a string to the list box.  The return value is the zero-based index of the string in the list box. If an error occurs,
		//	the return value is LB_ERR. If there is insufficient space to store the new string, the return value is LB_ERRSPACE.
		INT									AddString( LPCSTR _lpStr );

		// Adds a string to the list box.  The return value is the zero-based index of the string in the list box. If an error occurs,
		//	the return value is LB_ERR. If there is insufficient space to store the new string, the return value is LB_ERRSPACE.
		INT									AddString( LPCWSTR _lpStr );

		// Gets the number of items in the list box.
		INT									GetCount() const;

		// Gets the total number of selected items in a multiple-selection list box.
		INT									GetSelCount() const;
		
		// Gets the index of the currently selected item in a single-selection list box.
		INT									GetCurSel() const;

		// Sets the currently selected item in a single-selection list box
		INT									SetCurSel( INT _iIndex );

		// Fills a buffer with an array of integers that specify the item numbers of selected items in a multiple-selection list box.
		bool								GetSelItems( std::vector<INT> &_vResult ) const;

		// Selects an item in a multiple-selection list box and, if necessary, scrolls the item into view.
		bool								SetSel( BOOL _bSelected, INT _iIndex );

		// Gets the application-defined value associated with the specified list item in a list box.
		LRESULT								GetItemData( INT _iIndex ) const;

		// Sets the application-defined value associated with the specified list item in a list box.
		INT									SetItemData( INT _iIndex, LPARAM _pData );

		// Sets the height of list items or the selection field in a combo box.  If the index or height is invalid, the return value is CB_ERR.
		INT									SetItemHeight( INT _iIndex, INT _cyItem );

		// Sets the selection based on item data.
		INT									SetCurSelByItemData( LPARAM _pData );

		// Gets the currently selected items data.
		LPARAM								GetCurSelItemData() const;

		// Removes all items from the list box.
		void								ResetContent();

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_LISTBOX; }

		// Returns true if this is a CListBox class.
		virtual bool						IsListBox() const { return true; }


	protected :
		// == Functions.

	private :
		typedef CWidget						Parent;
	};
		
}	// namespace lsw

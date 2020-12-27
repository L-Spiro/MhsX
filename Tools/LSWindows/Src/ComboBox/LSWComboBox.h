#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CComboBox : public CWidget {
	public :
		CComboBox( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Prepares the combo box to receive a large number of items on init.
		INT									InitStorage( WPARAM _wpTotalItems, LPARAM _lpStringBytes );

		// Adds a string to a list in a combo box. If the combo box does not have the CBS_SORT style, the
		//	string is added to the end of the list. Otherwise, the string is inserted into the list and the list is sorted.
		INT									AddString( LPCSTR _psString );

		// Adds a string to a list in a combo box. If the combo box does not have the CBS_SORT style, the
		//	string is added to the end of the list. Otherwise, the string is inserted into the list and the list is sorted.
		INT									AddString( LPCWSTR _pwsString );

		// Deletes the item at the specified location in a list in a combo box.
		INT									DeleteString( INT  _iIndex );

		// Gets the number of items in the list box of a combo box.
		INT									GetCount() const;

		// Gets the length of an item's text.
		INT									GetLBTextLen( INT _iIndex ) const;

		// Gets a string from a list in a combo box.
		INT									GetLBText( INT _iIndex, std::wstring &_wString ) const;

		// Copies the text of the specified window's title bar (if it has one) into a buffer. If the specified window is a control, the text of the control is copied.
		virtual INT							GetTextA( LPSTR _lpString, INT _nMaxCount ) const;

		// Copies the text of the specified window's title bar (if it has one) into a buffer. If the specified window is a control, the text of the control is copied.
		virtual INT							GetTextW( LPWSTR _lpString, INT _nMaxCount ) const;

		// Gets the window text.
		virtual std::string					GetTextA() const;

		// Gets the window text.
		virtual std::wstring				GetTextW() const;

		// Retrieves the length, in characters, of the specified window's title bar text (if the window has a title bar).
		//	If the specified window is a control, the function retrieves the length of the text within the control
		virtual INT							GetTextLengthA() const;

		// Retrieves the length, in characters, of the specified window's title bar text (if the window has a title bar).
		//	If the specified window is a control, the function retrieves the length of the text within the control
		virtual INT							GetTextLengthW() const;

		// Gets the index of the currently selected item in a combo box.
		INT									GetCurSel() const;

		// Gets the application-defined value associated with the specified list item in a combo box.
		LRESULT								GetItemData( INT _iIndex ) const;

		// Retrieves the height of list items in a combo box.
		INT									GetItemHeight() const;

		// Gets the minimum number of visible items in the drop-down list of a combo box.
		INT									GetMinVisible() const;

		// Adds a string to a list in a combo box at the specified location.
		INT									InsertString( INT _iIndex, LPCSTR _lpStr );

		// Adds a string to a list in a combo box at the specified location.
		INT									InsertString( INT _iIndex, LPCWSTR _lpStr );

		// Removes all items from the list box and edit control of a combo box.
		VOID								ResetContent();

		// Sets the application-defined value associated with the specified list item in a combo box.
		INT									SetItemData( INT _iIndex, LPARAM _pData );

		// Sets the height of list items or the selection field in a combo box.  If the index or height is invalid, the return value is CB_ERR.
		INT									SetItemHeight( INT _iIndex, INT _cyItem );

		// Sets the minimum number of visible items in the drop-down list of a combo box.
		BOOL								SetMinVisible( INT _iMinVisible );

		// Shows or hides the list in a combo box.
		BOOL								ShowDropdown( BOOL _fShow );

		// Gets the rectangle for a string in the combo bod.
		LSW_RECT							GetItemRect( INT _iIndex ) const;

		// Gets the minimum width for the list box.
		LONG								GetListMinWidth() const;

		// Auto-sets the minimum width of the list box after adding all the strings.
		LONG								AutoSetMinListWidth();

		// Sets the selection based on index.
		INT									SetCurSel( INT _iIndex );

		// Sets the selection based on item data.
		INT									SetCurSelByItemData( LPARAM _pData );

		// Gets the currently selected items data.
		LPARAM								GetCurSelItemData() const;

		// Selects a range of text.  Implemented by CEdit and CComboBox.
		virtual VOID						SetSel( INT _iStart, INT _iEnd ) const;

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_COMBOBOX; }

		// Returns true if this is a CComboBox class.
		virtual bool						IsComboBox() const { return true; }

		// Replaces the selected text in an edit control or a rich edit control with the specified text.
		virtual void						ReplaceSel( bool _bCanUndo, const wchar_t * _pwcText );


	private :
	};

}	// namespace lsw

#pragma once

#include "../LSWWin.h"
#include "../ImageList/LSWImageList.h"
#include "../Images/LSWBitmap.h"
#include "../Layout/LSWMenuLayout.h"
#include "../Widget/LSWWidget.h"

#include <map>
#include <string>
#include <vector>

namespace lsw {

	/**
	 * A widget class representing a toolbar.
	 **/
	class CToolBar : public CWidget {
	public :
		CToolBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * Sets the image list.
		 * 
		 * \param _iIndex Index of the image list.
		 * \param _ilList The image list to set.
		 * \return Returns the handle to the previous image list.
		 **/
		HIMAGELIST							SetImageList( INT _iIndex, CImageList &_ilList );

		/**
		 * Checks a button.
		 * 
		 * \param _wId Command ID of the button.
		 * \param _bChecked TRUE to check the button, FALSE to uncheck it.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								CheckButton( WORD _wId, BOOL _bChecked );

		/**
		 * Auto-sizes.
		 **/
		VOID								AutoSize();

		/**
		 * Determines if a button is checked.
		 * 
		 * \param _wId Command ID of the button.
		 * \return Returns TRUE if the button is checked, or FALSE otherwise.
		 **/
		BOOL								IsChecked( WORD _wId ) const;

		/**
		 * Gets the button count.
		 * 
		 * \return Returns the number of buttons in the toolbar.
		 **/
		DWORD								GetButtonCount() const;

		/**
		 * Deletes a button.
		 * 
		 * \param _dwIndex Zero-based index of the button to delete.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								DeleteButton( DWORD _dwIndex );

		/**
		 * Enables or disables a button.
		 * 
		 * \param _wId Command ID of the button.
		 * \param _bEnabled TRUE to enable the button, FALSE to disable it.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								EnableButton( WORD _wId, BOOL _bEnabled );

		/**
		 * Gets a button's data.
		 * 
		 * \param _dwIndex Zero-based index of the button.
		 * \param _tButton A reference to a TBBUTTON structure to receive the button information.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								GetButton( DWORD _dwIndex, TBBUTTON &_tButton );

		/**
		 * Retrieves the bounding rectangle for a specified toolbar button.  This method does not retrieve the bounding rectangle for buttons whose state is set to the TBSTATE_HIDDEN value
		 * 
		 * \param _iCommandId Command identifier of the button.
		 * \param _rRect Reference to an LSW_RECT structure that will receive the bounding rectangle information.
		 * \return Returns nonzero if successful, or zero otherwise.
		 **/
		BOOL								GetRect( int _iCommandId, LSW_RECT &_rRect ) const;

		/**
		 * Returns a DWORD value that contains the width and height values in the low word and high word, respectively.
		 * 
		 * \return Returns the dimensions of the buttons.
		 **/
		DWORD								GetButtonSize() const;

		/**
		 * Gets the button text length in wchar_t units, including the NULL character.
		 * 
		 * \param _wId Command ID of the button.
		 * \return Returns the length of the button text in wide characters.
		 **/
		DWORD								GetButtonTextLenW( WORD _wId ) const;

		/**
		 * Gets the button text length in char units, including the NULL character.
		 * 
		 * \param _wId Command ID of the button.
		 * \return Returns the length of the button text in standard characters.
		 **/
		DWORD								GetButtonTextLenA( WORD _wId ) const;

		/**
		 * Gets the button text.  _pwcBuffer should be at least GetButtonTextLenW() characters in length.
		 * 
		 * \param _wId Command ID of the button.
		 * \param _pwcBuffer A pointer to a wide character buffer that receives the text.
		 * \return Returns the length of the text copied to the buffer.
		 **/
		DWORD								GetButtonTextW( WORD _wId, WCHAR * _pwcBuffer ) const;

		/**
		 * Gets the button text.  _pcBuffer should be at least GetButtonTextLenA() characters in length.
		 * 
		 * \param _wId Command ID of the button.
		 * \param _pcBuffer A pointer to a character buffer that receives the text.
		 * \return Returns the length of the text copied to the buffer.
		 **/
		DWORD								GetButtonTextA( WORD _wId, CHAR * _pcBuffer ) const;

		/**
		 * Gets a button's rectangle.
		 * 
		 * \param _dwIndex Zero-based index of the button.
		 * \return Returns an LSW_RECT containing the button's bounding rectangle.
		 **/
		LSW_RECT							GetButtonRect( DWORD _dwIndex ) const;

		/**
		 * Gets a rectangle large enough to fit any of the buttons inside.
		 * 
		 * \return Returns an LSW_RECT representing the minimum bounding rectangle.
		 **/
		LSW_RECT							GetMinBoundingRect() const;

		/**
		 * Adds buttons.
		 * 
		 * \param _pbButtons A pointer to an array of TBBUTTON structures.
		 * \param _sTotal The number of buttons in the array to add.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								AddButtons( const TBBUTTON * _pbButtons, size_t _sTotal );

		/**
		 * Gives a button a drop-down menu.  The button should have the BTNS_DROPDOWN style (a separate arrow beside the button; clicking
		 *	the button itself sends its command as usual) or BTNS_WHOLEDROPDOWN (the whole button shows the menu).  When the arrow is
		 *	clicked the menu is shown below the button.  The owner receives WM_INITMENUPOPUP (to enable and check the items) and the
		 *	WM_COMMAND of the chosen item, just as for its menu bar.
		 *
		 * \param _wId Command ID of the button.
		 * \param _pmlLayouts The menu layouts (see CLayoutManager::CreatePopupMenu()).  The first is the menu shown; the rest are its
		 *	submenus.  They must remain valid for as long as the toolbar exists.
		 * \param _sTotal The number of layouts at _pmlLayouts.
		 * \param _pwOwner The window that owns the menu.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								SetDropDownMenu( WORD _wId, const LSW_MENU_LAYOUT * _pmlLayouts, size_t _sTotal, CWidget * _pwOwner );

		/**
		 * Shows the drop-down menu below a button, if one was given with SetDropDownMenu().
		 *
		 * \param _wId Command ID of the button.
		 * \return Returns TRUE if a menu was shown.
		 **/
		BOOL								ShowDropDownMenu( WORD _wId );

		/**
		 * Sets the text of a button's tooltip (the toolbar must have the TBSTYLE_TOOLTIPS style).  Tooltips can instead be supplied
		 *	when they are needed by overriding TbnGetInfoTip() in the window that receives the toolbar's notifications, which avoids
		 *	keeping the text in memory.
		 *
		 * \param _wId Command ID of the button.
		 * \param _pwcText The text, or nullptr to remove the button's tooltip.
		 * \return Returns TRUE if successful, or FALSE otherwise.
		 **/
		BOOL								SetButtonToolTip( WORD _wId, LPCWSTR _pwcText );

		/**
		 * TBN_GETINFOTIPW.  Supplies a button's tooltip text if one was given with SetButtonToolTip().
		 *
		 * \param _pgitTip The notification data.  iItem is the command ID of the button.
		 * \return Returns LSW_H_HANDLED if text was supplied.
		 **/
		virtual LSW_HANDLED					TbnGetInfoTip( LPNMTBGETINFOTIPW _pgitTip );

		/**
		 * TBN_DROPDOWN.  Shows the button's drop-down menu, if it has one (see SetDropDownMenu()).
		 *
		 * \param _lptbToolBar A pointer to an NMTOOLBARW structure.
		 * \return Returns LSW_H_HANDLED if a menu was shown.
		 **/
		virtual LSW_HANDLED					TbnDropDown( LPNMTOOLBARW _lptbToolBar );

		/**
		 * Creates an edit box with an embedded icon inside it and placeholder hint text.
		 * 
		 * \param _iCommandId The command ID of the separator placeholder.
		 * \param _iEditId The control ID to assign to the newly created edit box.
		 * \param _iType The image type to embed in the edit control.
		 * \param _bImage The image to embed in the edit control.
		 * \param _pwszCueText The hint text to display when the edit control is empty.
		 * \param _pwEditOut Pointer to an HWND that receives the edit control handle.
		 * \return bool Returns true if the control was successfully created.
		 */
		virtual bool						CreateIconEditBox( int _iCommandId, int _iEditId, INT _iType, CBitmap &_bImage, LPCWSTR _pwszCueText, CWidget * &_pwEditOut );

		/**
		 * TBN_QUERYINSERT.
		 * 
		 * \param _lptbToolBar A pointer to an NMTOOLBARW structure.
		 * \return Returns an LSW_HANDLED indicating how the message was handled.
		 **/
		virtual LSW_HANDLED					TbnQueryInsert( const LPNMTOOLBARW _lptbToolBar );

		/**
		 * TBN_QUERYDELETE.
		 * 
		 * \param _lptbToolBar A pointer to an NMTOOLBARW structure.
		 * \return Returns an LSW_HANDLED indicating how the message was handled.
		 **/
		virtual LSW_HANDLED					TbnQueryDelete( const LPNMTOOLBARW _lptbToolBar );

		/**
		 * TBN_RESET.
		 * 
		 * \return Returns an LSW_HANDLED indicating how the message was handled.
		 **/
		virtual LSW_HANDLED					TbnReset();

		/**
		 * TBN_GETBUTTONINFO.
		 * 
		 * \param _lptbToolBar A pointer to an NMTOOLBARW structure.
		 * \return Returns an LSW_HANDLED indicating how the message was handled.
		 **/
		virtual LSW_HANDLED					TbnGetButtonInfo( LPNMTOOLBARW _lptbToolBar );

		/**
		 * Determines the type of control this is.
		 * 
		 * \return Returns the specific widget type identifier.
		 **/
		virtual uint32_t					WidgetType() const { return LSW_LT_TOOLBAR; }

		/**
		 * Returns true if this is a CToolBar class.
		 * 
		 * \return Returns true to indicate this is a toolbar widget.
		 **/
		virtual bool						IsToolBar() const { return true; }


	protected :
		// == Types.
		/**
		 * A drop-down menu given to a button.
		 **/
		struct LSW_DROP_DOWN {
			WORD							wId;													/**< Command ID of the button. */
			const LSW_MENU_LAYOUT *			pmlLayouts;												/**< The menu layouts. */
			size_t							sTotal;													/**< The number of layouts at pmlLayouts. */
			CWidget *						pwOwner;												/**< The window that owns the menu. */
		};


		// == Members.
		/**
		 * The buttons' drop-down menus.
		 **/
		std::vector<LSW_DROP_DOWN>			m_vDropDowns;

		/**
		 * The buttons' tooltips given by SetButtonToolTip(), by command ID.
		 **/
		std::map<WORD, std::wstring>		m_mToolTips;

		/**
		 * Copy of the buttons for TBN_GETBUTTONINFO.
		 * TODO: Button text should be copied into a std::wstring or else pointers here can
		 * point to invalid addresses.
		 **/
		std::vector<TBBUTTON>				m_vButtons;


		// == Functions.
		/**
		 * Gets the index of a button given its command ID.
		 * 
		 * \param _wId Command ID of the button.
		 * \return Returns the zero-based index of the requested button.
		 **/
		size_t								IndexOfButton( WORD _wId ) const;

		/**
		 * \brief Subclasses the edit control to handle background painting for its child static controls.
		 * \param _hWnd Handle to the edit control window.
		 * \param _uMsg The message being processed.
		 * \param _wParam Additional message information.
		 * \param _lParam Additional message information.
		 * \param _uIdSubclass The subclass ID.
		 * \param _dwRefData Reference data provided when subclassing.
		 * \return LRESULT Returns the result of the message processing.
		 */
		static LRESULT CALLBACK				EditSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uIdSubclass, DWORD_PTR _dwRefData );


	private :
		/**
		 * A typedef for the parent class structure.
		 **/
		typedef CWidget						Parent;
	};

}	// namespace lsw

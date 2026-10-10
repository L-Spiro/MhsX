#pragma once

#include "../LSWWin.h"
#include "../ImageList/LSWImageList.h"
#include "../Images/LSWBitmap.h"
#include "../Widget/LSWWidget.h"

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
		// == Members.
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


	private :
		/**
		 * A typedef for the parent class structure.
		 **/
		typedef CWidget						Parent;
	};

}	// namespace lsw

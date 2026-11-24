#include "LSWToolBar.h"
#include "../Layout/LSWLayoutManager.h"
#include "../Static/LSWStatic.h"

namespace lsw {

	CToolBar::CToolBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}


	// == Functions.
	/**
	 * Sets the image list.
	 * 
	 * \param _iIndex Index of the image list.
	 * \param _ilList The image list to set.
	 * \return Returns the handle to the previous image list.
	 **/
	HIMAGELIST CToolBar::SetImageList( INT _iIndex, CImageList &_ilList ) {
		if ( !Wnd() ) { return NULL; }
		return reinterpret_cast<HIMAGELIST>(::SendMessageW( Wnd(), TB_SETIMAGELIST, static_cast<WPARAM>(_iIndex), reinterpret_cast<LPARAM>(_ilList.Handle()) ));
	}

	/**
	 * Checks a button.
	 * 
	 * \param _wId Command ID of the button.
	 * \param _bChecked TRUE to check the button, FALSE to uncheck it.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CToolBar::CheckButton( WORD _wId, BOOL _bChecked ) {
		if ( !Wnd() ) { return FALSE; }
		BOOL bRet = static_cast<BOOL>(::SendMessageW( Wnd(), TB_CHECKBUTTON, static_cast<WPARAM>(_wId), MAKELPARAM( _bChecked, 0 ) ));
		if ( bRet ) {
			size_t sIndex = IndexOfButton( _wId );
			if ( sIndex != static_cast<size_t>(-1) ) {
				if ( _bChecked ) {
					// Add TBSTATE_CHECKED.
					m_vButtons[sIndex].fsState |= TBSTATE_CHECKED;
				}
				else {
					// Remove TBSTATE_CHECKED.
					m_vButtons[sIndex].fsState &= ~TBSTATE_CHECKED;
				}
			}
		}
		return bRet;
	}

	/**
	 * Auto-sizes.
	 **/
	VOID CToolBar::AutoSize() {
		if ( !Wnd() ) { return; }
		::SendMessageW( Wnd(), TB_AUTOSIZE, 0L, 0L );
	}

	/**
	 * Determines if a button is checked.
	 * 
	 * \param _wId Command ID of the button.
	 * \return Returns TRUE if the button is checked, or FALSE otherwise.
	 **/
	BOOL CToolBar::IsChecked( WORD _wId ) const {
		TBBUTTONINFOW tbiButton = { 0 };
		tbiButton.cbSize = sizeof( tbiButton );
		tbiButton.dwMask = TBIF_STATE;
		if ( -1 != static_cast<BOOL>(::SendMessageW( Wnd(), TB_GETBUTTONINFOW, static_cast<WPARAM>(_wId), reinterpret_cast<LPARAM>(&tbiButton) )) ) {
			return (tbiButton.fsState & TBSTATE_CHECKED) ? TRUE : FALSE;
		}
		return FALSE;
	}

	/**
	 * Gets the button count.
	 * 
	 * \return Returns the number of buttons in the toolbar.
	 **/
	DWORD CToolBar::GetButtonCount() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<DWORD>(::SendMessageW( Wnd(), TB_BUTTONCOUNT, 0L, 0L ));
	}

	/**
	 * Deletes a button.
	 * 
	 * \param _dwIndex Zero-based index of the button to delete.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CToolBar::DeleteButton( DWORD _dwIndex ) {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageW( Wnd(), TB_DELETEBUTTON, static_cast<WPARAM>(_dwIndex), 0L ));
	}

	/**
	 * Enables or disables a button.
	 * 
	 * \param _wId Command ID of the button.
	 * \param _bEnabled TRUE to enable the button, FALSE to disable it.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CToolBar::EnableButton( WORD _wId, BOOL _bEnabled ) {
		if ( !Wnd() ) { return FALSE; }
		BOOL bRet = static_cast<BOOL>(::SendMessageW( Wnd(), TB_ENABLEBUTTON, static_cast<WPARAM>(_wId), MAKELPARAM( _bEnabled, 0 ) ));
		if ( bRet ) {
			size_t sIndex = IndexOfButton( _wId );
			if ( sIndex != static_cast<size_t>(-1) ) {
				if ( _bEnabled ) {
					// Add TBSTATE_ENABLED.
					m_vButtons[sIndex].fsState |= TBSTATE_ENABLED;
				}
				else {
					// Remove TBSTATE_ENABLED.
					m_vButtons[sIndex].fsState &= ~TBSTATE_ENABLED;
				}
			}
		}
		return bRet;
	}

	/**
	 * Gets a button's data.
	 * 
	 * \param _dwIndex Zero-based index of the button.
	 * \param _tButton A reference to a TBBUTTON structure to receive the button information.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CToolBar::GetButton( DWORD _dwIndex, TBBUTTON &_tButton ) {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageW( Wnd(), TB_GETBUTTON, static_cast<WPARAM>(_dwIndex), reinterpret_cast<LPARAM>(&_tButton) ));
	}

	/**
	 * Retrieves the bounding rectangle for a specified toolbar button.  This method does not retrieve the bounding rectangle for buttons whose state is set to the TBSTATE_HIDDEN value
	 * 
	 * \param _iCommandId Command identifier of the button.
	 * \param _rRect Reference to an LSW_RECT structure that will receive the bounding rectangle information.
	 * \return Returns nonzero if successful, or zero otherwise.
	 **/
	BOOL CToolBar::GetRect( int _iCommandId, LSW_RECT &_rRect ) const {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageW( Wnd(), TB_GETRECT, static_cast<WPARAM>(_iCommandId), reinterpret_cast<LPARAM>(&_rRect) ));
	}

	/**
	 * Returns a DWORD value that contains the width and height values in the low word and high word, respectively.
	 * 
	 * \return Returns the dimensions of the buttons.
	 **/
	DWORD CToolBar::GetButtonSize() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<DWORD>(::SendMessageW( Wnd(), TB_GETBUTTONSIZE, 0L, 0L ));
	}

	/**
	 * Gets the button text length in wchar_t units, including the NULL character.
	 * 
	 * \param _wId Command ID of the button.
	 * \return Returns the length of the button text in wide characters.
	 **/
	DWORD CToolBar::GetButtonTextLenW( WORD _wId ) const {
		if ( !Wnd() ) { return 1; }
		INT iRet = static_cast<INT>(::SendMessageW( Wnd(), TB_GETBUTTONTEXTW, static_cast<WPARAM>(_wId), NULL ));
		return static_cast<DWORD>(max( iRet + 1, 1 ));
	}

	/**
	 * Gets the button text length in char units, including the NULL character.
	 * 
	 * \param _wId Command ID of the button.
	 * \return Returns the length of the button text in standard characters.
	 **/
	DWORD CToolBar::GetButtonTextLenA( WORD _wId ) const {
		if ( !Wnd() ) { return 1; }
		INT iRet = static_cast<INT>(::SendMessageA( Wnd(), TB_GETBUTTONTEXTA, static_cast<WPARAM>(_wId), NULL ));
		return static_cast<DWORD>(max( iRet + 1, 1 ));
	}

	/**
	 * Gets the button text.  _pwcBuffer should be at least GetButtonTextLenW() characters in length.
	 * 
	 * \param _wId Command ID of the button.
	 * \param _pwcBuffer A pointer to a wide character buffer that receives the text.
	 * \return Returns the length of the text copied to the buffer.
	 **/
	DWORD CToolBar::GetButtonTextW( WORD _wId, WCHAR * _pwcBuffer ) const {
		if ( !Wnd() ) { return static_cast<DWORD>(-1); }
		return static_cast<DWORD>(::SendMessageW( Wnd(), TB_GETBUTTONTEXTW, static_cast<WPARAM>(_wId), reinterpret_cast<LPARAM>(_pwcBuffer) ));
	}

	/**
	 * Gets the button text.  _pcBuffer should be at least GetButtonTextLenA() characters in length.
	 * 
	 * \param _wId Command ID of the button.
	 * \param _pcBuffer A pointer to a character buffer that receives the text.
	 * \return Returns the length of the text copied to the buffer.
	 **/
	DWORD CToolBar::GetButtonTextA( WORD _wId, CHAR * _pcBuffer ) const {
		if ( !Wnd() ) { return static_cast<DWORD>(-1); }
		return static_cast<DWORD>(::SendMessageA( Wnd(), TB_GETBUTTONTEXTA, static_cast<WPARAM>(_wId), reinterpret_cast<LPARAM>(_pcBuffer) ));
	}

	/**
	 * Gets a button's rectangle.
	 * 
	 * \param _dwIndex Zero-based index of the button.
	 * \return Returns an LSW_RECT containing the button's bounding rectangle.
	 **/
	LSW_RECT CToolBar::GetButtonRect( DWORD _dwIndex ) const {
		LSW_RECT rRet;
		rRet.left = rRet.top = rRet.right = rRet.bottom = 0;
		if ( !Wnd() ) { return rRet; }
		::SendMessageW( Wnd(), TB_GETITEMRECT, static_cast<WPARAM>(_dwIndex), reinterpret_cast<LPARAM>(&rRet) );
		return rRet;
	}

	/**
	 * Gets a rectangle large enough to fit any of the buttons inside.
	 * 
	 * \return Returns an LSW_RECT representing the minimum bounding rectangle.
	 **/
	LSW_RECT CToolBar::GetMinBoundingRect() const {
		LONG lW = 0, lH = 0;
		LSW_RECT rRet;
		DWORD dwTotal = GetButtonCount();
		for ( DWORD I = 0; I < dwTotal; ++I ) {
			rRet = GetButtonRect( I );
			lW = max( lW, rRet.Width() );
			lH = max( lH, rRet.Height() );
		}
		rRet.left = rRet.top = 0;
		rRet.SetWidth( lW );
		rRet.SetHeight( lH );
		return rRet;
	}

	/**
	 * Adds buttons.
	 * 
	 * \param _pbButtons A pointer to an array of TBBUTTON structures.
	 * \param _sTotal The number of buttons in the array to add.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CToolBar::AddButtons( const TBBUTTON * _pbButtons, size_t _sTotal ) {
		if ( !Wnd() ) { return FALSE; }
		::SendMessageW( Wnd(), TB_BUTTONSTRUCTSIZE, sizeof( (*_pbButtons) ), 0L );
		for ( size_t I = 0; I < _sTotal; ++I ) {
			if ( _pbButtons[I].fsStyle & BTNS_DROPDOWN ) {
				const DWORD dwExStyle = static_cast<DWORD>(::SendMessageW( Wnd(), TB_GETEXTENDEDSTYLE, 0L, 0L ));
				::SendMessageW( Wnd(), TB_SETEXTENDEDSTYLE, 0L, static_cast<LPARAM>(dwExStyle | TBSTYLE_EX_DRAWDDARROWS) );
				break;
			}
		}
		BOOL bRet = static_cast<BOOL>(::SendMessageW( Wnd(), TB_ADDBUTTONSW, static_cast<WPARAM>(_sTotal), reinterpret_cast<LPARAM>(_pbButtons) ));
		if ( bRet ) {
			for ( size_t I = 0; I < _sTotal; ++I ) {
				m_vButtons.push_back( (*_pbButtons++) );
			}
			return TRUE;
		}
		return FALSE;
	}

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
	BOOL CToolBar::SetDropDownMenu( WORD _wId, const LSW_MENU_LAYOUT * _pmlLayouts, size_t _sTotal, CWidget * _pwOwner ) {
		if ( !_pmlLayouts || !_sTotal || !_pwOwner ) { return FALSE; }
		try {
			for ( auto &ddThis : m_vDropDowns ) {
				if ( ddThis.wId == _wId ) {
					ddThis.pmlLayouts = _pmlLayouts;
					ddThis.sTotal = _sTotal;
					ddThis.pwOwner = _pwOwner;
					return TRUE;
				}
			}
			m_vDropDowns.push_back( { _wId, _pmlLayouts, _sTotal, _pwOwner } );
			return TRUE;
		}
		catch ( ... ) { return FALSE; }
	}

	/**
	 * Shows the drop-down menu below a button, if one was given with SetDropDownMenu().
	 *
	 * \param _wId Command ID of the button.
	 * \return Returns TRUE if a menu was shown.
	 **/
	BOOL CToolBar::ShowDropDownMenu( WORD _wId ) {
		if ( !Wnd() ) { return FALSE; }
		for ( const auto &ddThis : m_vDropDowns ) {
			if ( ddThis.wId != _wId ) { continue; }
			if ( !ddThis.pwOwner || !ddThis.pwOwner->Wnd() ) { return FALSE; }

			HMENU hMenu = CBase::LayoutManager()->CreateDisplayPopupMenu( ddThis.pmlLayouts, ddThis.sTotal );
			if ( !hMenu ) { return FALSE; }

			// Below the button, or above it if there is no room below; never covering it.
			LSW_RECT rButton;
			if ( !GetRect( _wId, rButton ) ) {
				::DestroyMenu( hMenu );
				return FALSE;
			}
			::MapWindowPoints( Wnd(), NULL, reinterpret_cast<POINT *>(&rButton), 2 );
			TPMPARAMS tpParms = { sizeof( TPMPARAMS ) };
			tpParms.rcExclude = rButton;

			// Shown as pressed while the menu is open.
			::SendMessageW( Wnd(), TB_PRESSBUTTON, static_cast<WPARAM>(_wId), MAKELPARAM( TRUE, 0 ) );
			::TrackPopupMenuEx( hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_VERTICAL | TPM_RIGHTBUTTON,
				rButton.left, rButton.bottom, ddThis.pwOwner->Wnd(), &tpParms );
			::SendMessageW( Wnd(), TB_PRESSBUTTON, static_cast<WPARAM>(_wId), MAKELPARAM( FALSE, 0 ) );
			// The chosen command was posted to the owner, so the menu can go.
			::DestroyMenu( hMenu );
			return TRUE;
		}
		return FALSE;
	}

	/**
	 * Sets the text of a button's tooltip (the toolbar must have the TBSTYLE_TOOLTIPS style).  Tooltips can instead be supplied
	 *	when they are needed by overriding TbnGetInfoTip() in the window that receives the toolbar's notifications, which avoids
	 *	keeping the text in memory.
	 *
	 * \param _wId Command ID of the button.
	 * \param _pwcText The text, or nullptr to remove the button's tooltip.
	 * \return Returns TRUE if successful, or FALSE otherwise.
	 **/
	BOOL CToolBar::SetButtonToolTip( WORD _wId, LPCWSTR _pwcText ) {
		try {
			if ( !_pwcText ) {
				m_mToolTips.erase( _wId );
				return TRUE;
			}
			m_mToolTips[_wId] = _pwcText;
			return TRUE;
		}
		catch ( ... ) { return FALSE; }
	}

	/**
	 * TBN_GETINFOTIPW.  Supplies a button's tooltip text if one was given with SetButtonToolTip().
	 *
	 * \param _pgitTip The notification data.  iItem is the command ID of the button.
	 * \return Returns LSW_H_HANDLED if text was supplied.
	 **/
	CWidget::LSW_HANDLED CToolBar::TbnGetInfoTip( LPNMTBGETINFOTIPW _pgitTip ) {
		if ( !_pgitTip || !_pgitTip->pszText || _pgitTip->cchTextMax <= 0 ) { return LSW_H_CONTINUE; }
		auto aFound = m_mToolTips.find( static_cast<WORD>(_pgitTip->iItem) );
		if ( aFound == m_mToolTips.end() ) { return LSW_H_CONTINUE; }
		::wcsncpy_s( _pgitTip->pszText, static_cast<size_t>(_pgitTip->cchTextMax), aFound->second.c_str(), _TRUNCATE );
		return LSW_H_HANDLED;
	}

	/**
	 * TBN_DROPDOWN.  Shows the button's drop-down menu, if it has one (see SetDropDownMenu()).
	 *
	 * \param _lptbToolBar A pointer to an NMTOOLBARW structure.
	 * \return Returns LSW_H_HANDLED if a menu was shown.
	 **/
	CWidget::LSW_HANDLED CToolBar::TbnDropDown( LPNMTOOLBARW _lptbToolBar ) {
		if ( _lptbToolBar && ShowDropDownMenu( static_cast<WORD>(_lptbToolBar->iItem) ) ) { return LSW_H_HANDLED; }
		return LSW_H_CONTINUE;
	}

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
	bool CToolBar::CreateIconEditBox( int _iCommandId, int _iEditId, INT _iType, CBitmap &_bImage, LPCWSTR _pwszCueText, CWidget * &_pwEditOut ) {
		bool bSuccess = false;
		LSW_RECT rcItem;
		_pwEditOut = nullptr;

		if ( GetRect( _iCommandId, rcItem ) ) {
			// Create the edit control over the placeholder separator.
			LSW_WIDGET_LAYOUT wlLayout = {
				LSW_LT_EDIT,									// ltType
				static_cast<WORD>(_iEditId),					// wId
				L"EDIT",										// lpwcClass
				TRUE,											// bEnabled
				TRUE,											// bActive
				rcItem.left,									// iLeft
				rcItem.top,										// iTop
				DWORD( rcItem.Width() ),						// dwWidth
				DWORD( rcItem.Height() ),						// dwHeight
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_LEFT | WS_CLIPCHILDREN,			// dwStyle
				WS_EX_CLIENTEDGE,								// dwStyleEx
				nullptr,										// pwcText
				0,												// sTextLen
				static_cast<DWORD>(Id()),						// dwParentId
			};
			auto pwEdit = lsw::CBase::LayoutManager()->CreateWidget( wlLayout, this, true, NULL, 0 );

			if ( pwEdit != nullptr ) {
				::SetWindowSubclass( pwEdit->Wnd(), EditSubclassProc, 0, 0 );
				::SendMessageW( pwEdit->Wnd(), WM_SETFONT, static_cast<WPARAM>(::SendMessageW( Wnd(), WM_GETFONT, 0, 0 )), MAKELPARAM( TRUE, 0 ) );

				if ( _pwszCueText != NULL ) {
					::SendMessageW( pwEdit->Wnd(), EM_SETCUEBANNER, FALSE, reinterpret_cast<LPARAM>(_pwszCueText) );
				}

				int iIconWidth = ::GetSystemMetrics( SM_CXSMICON );
				int iIconHeight = ::GetSystemMetrics( SM_CYSMICON );
            
				LSW_RECT rcEditRect;
				::GetClientRect( pwEdit->Wnd(), &rcEditRect );
				int iIconY = ((rcEditRect.Height()) - iIconHeight) / 2;

				LSW_WIDGET_LAYOUT wlStaticLayout = {
					LSW_LT_LABEL,									// ltType
					static_cast<WORD>(0),							// wId
					L"STATIC",										// lpwcClass
					TRUE,											// bEnabled
					TRUE,											// bActive
					2,												// iLeft
					iIconY,											// iTop
					DWORD( iIconWidth ),							// dwWidth
					DWORD( iIconHeight ),							// dwHeight
					WS_CHILD | WS_VISIBLE | SS_BITMAP,				// dwStyle
					0,												// dwStyleEx
					nullptr,										// pwcText
					0,												// sTextLen
					static_cast<DWORD>(pwEdit->Id()),				// dwParentId
				};
				auto psStatic = reinterpret_cast<CStatic *>(lsw::CBase::LayoutManager()->CreateWidget( wlStaticLayout, pwEdit, true, NULL, 0 ));
				if ( psStatic != NULL ) {
					psStatic->SetImage( _iType, _bImage );

					int iLeftMargin = iIconWidth + 6;
					::SendMessageW( pwEdit->Wnd(), EM_SETMARGINS, EC_LEFTMARGIN, MAKELPARAM( iLeftMargin, 0 ) );
				}

				_pwEditOut = pwEdit;
				bSuccess = true;
			}
		}
		return bSuccess;
	}

	/**
	 * TBN_QUERYINSERT.
	 * 
	 * \param _lptbToolBar A pointer to an NMTOOLBARW structure.
	 * \return Returns an LSW_HANDLED indicating how the message was handled.
	 **/
	CWidget::LSW_HANDLED CToolBar::TbnQueryInsert( const LPNMTOOLBARW /*_lptbToolBar*/ ) {
		return LSW_H_HANDLED;
	}

	/**
	 * TBN_QUERYDELETE.
	 * 
	 * \param _lptbToolBar A pointer to an NMTOOLBARW structure.
	 * \return Returns an LSW_HANDLED indicating how the message was handled.
	 **/
	CWidget::LSW_HANDLED CToolBar::TbnQueryDelete( const LPNMTOOLBARW /*_lptbToolBar*/ ) {
		return LSW_H_HANDLED;
	}

	/**
	 * TBN_RESET.
	 * 
	 * \return Returns an LSW_HANDLED indicating how the message was handled.
	 **/
	CWidget::LSW_HANDLED CToolBar::TbnReset() {
		if ( !m_vButtons.size() ) { return LSW_H_CONTINUE; }
		std::vector<TBBUTTON> vCopy = m_vButtons;

		for ( DWORD I = GetButtonCount(); I--; ) {
			DeleteButton( 0 );
		}
		m_vButtons.clear();
		AddButtons( &vCopy[0], vCopy.size() );
		return LSW_H_CONTINUE;	// To keep the dialog open.
		//return LSW_H_HANDLED;	// To close the dialog.
	}

	/**
	 * TBN_GETBUTTONINFO.
	 * 
	 * \param _lptbToolBar A pointer to an NMTOOLBARW structure.
	 * \return Returns an LSW_HANDLED indicating how the message was handled.
	 **/
	CWidget::LSW_HANDLED CToolBar::TbnGetButtonInfo( LPNMTOOLBARW _lptbToolBar ) {
		if ( static_cast<size_t>(_lptbToolBar->iItem) >= m_vButtons.size() ) { return LSW_H_CONTINUE; }
		_lptbToolBar->tbButton = m_vButtons[_lptbToolBar->iItem];
		return LSW_H_HANDLED;
	}

	/**
	 * Gets the index of a button given its command ID.
	 * 
	 * \param _wId Command ID of the button.
	 * \return Returns the zero-based index of the requested button.
	 **/
	size_t CToolBar::IndexOfButton( WORD _wId ) const {
		for ( size_t I = m_vButtons.size(); I--; ) {
			if ( m_vButtons[I].idCommand == _wId ) { return I; }
		}
		return static_cast<size_t>(-1);
	}

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
	LRESULT CALLBACK CToolBar::EditSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uIdSubclass, DWORD_PTR /*_dwRefData*/ ) {
		if ( _uMsg == WM_CTLCOLORSTATIC ) {
			HDC hdcStatic = reinterpret_cast<HDC>(_wParam);
			::SetBkColor( hdcStatic, ::GetSysColor( COLOR_WINDOW ) );
			return reinterpret_cast<LRESULT>(::GetSysColorBrush( COLOR_WINDOW ));
		}
		else if ( _uMsg == WM_NCDESTROY ) {
			::RemoveWindowSubclass( _hWnd, EditSubclassProc, _uIdSubclass );
		}
		return ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam );
	}

}	// namespace lsw

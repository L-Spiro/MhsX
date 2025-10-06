#pragma once

#include "../LSWWin.h"
#include "../Expressions/LSWExpression.h"
#include "../Helpers/LSWHelpers.h"
#include "../Layout/LSWWidgetLayout.h"

#include <vector>


#define LSW_WIN2CLASS( H )					reinterpret_cast<CWidget *>(::GetWindowLongPtrW( H, GWLP_USERDATA ))
#define LSW_TOTAL_INTERNAL_CUSTOM_COMMANDS	0x100
#define LSW_USER							(WM_USER + LSW_TOTAL_INTERNAL_CUSTOM_COMMANDS)

namespace lsw {

	class CWidget {
		friend class						CDockable;
		friend class						CLayoutManager;
		friend class						CTreeListView;
	public :
		CWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		virtual ~CWidget();


		// == Enumerations.
		enum LSW_HANDLED {
			LSW_H_HANDLED,					// The message was handled and the default procedure should not be called.
			LSW_H_CONTINUE,					// Continue and let the default procedure handle it.
		};

		enum LSW_NOTIFICATIONS {
			// Tab control notifications.
			LSW_TAB_NM_BASE					= (0U - 3048U),
			LSW_TAB_NM_CLOSE				= (LSW_TAB_NM_BASE - 0),
			LSW_TAB_NM_CHECK,
		};

		enum LSN_CUSTOM_COMMANDS {
			// We have up to LSW_TOTAL_INTERNAL_CUSTOM_COMMANDS commands.
			LSW_BN_COLOR_CHANGED			= WM_USER,
		};


		// == Types.
#pragma pack( push, 1 )
		// A structure used to send a WM_NOTIFY to the parent as a request for docking information.
		struct LSW_NMTABCLOSE {
			NMHDR							hdr;				// Required header.
			INT								iTab;				// Tab being closed.
			CWidget *						pwWidget;			// Source widget.
		};
#pragma pack( pop )


		// == Functions.
		/**
		 * Default top-level window procedure.
		 * \brief Routes messages to the appropriate handler and provides default processing.
		 *
		 * Subclasses may forward unhandled messages here. This procedure must be static and is typically
		 * associated with the registered window class for CWidget-based windows.
		 *
		 * \param _hWnd Window receiving the message.
		 * \param _uMsg Message identifier (WM_*).
		 * \param _wParam WPARAM message data.
		 * \param _lParam LPARAM message data.
		 * \return Returns an LRESULT as defined by the message semantics.
		 */
		static LRESULT CALLBACK				WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

		/**
		 * Default dialog box procedure.
		 * \brief Handles dialog-specific messages and dispatches notifications.
		 *
		 * Use this for dialog templates associated with CWidget-based dialogs. Unhandled messages
		 * are passed to ::DefDlgProc().
		 *
		 * \param _hWnd Dialog window handle.
		 * \param _uMsg Message identifier (WM_*).
		 * \param _wParam WPARAM message data.
		 * \param _lParam LPARAM message data.
		 * \return Returns TRUE if the message is processed; otherwise FALSE (or a control-specific INT_PTR).
		 */
		static INT_PTR CALLBACK				DialogProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );


		/**
		 * The Window handle.
		 * 
		 * \return Returns the native window handle.
		 **/
		HWND								Wnd() const { return m_hWnd; }

		/**
		 * The parent widget.
		 * 
		 * \return Returns a pointer to the parent or nullptr.
		 **/
		CWidget *							Parent() { return m_pwParent; }

		/**
		 * The parent widget.
		 * 
		 * \return Returns a pointer to the parent or nullptr.
		 **/
		const CWidget *						Parent() const { return m_pwParent; }

		/**
		 * The ancestor widget.
		 * 
		 * \return Returns a pointer to the most parent object or nullptr.
		 **/
		CWidget *							Ancestor();

		/**
		 * The ancestor widget.
		 * 
		 * \return Returns a pointer to the most parent object or nullptr.
		 **/
		const CWidget *						Ancestor() const;

		/**
		 * Custom ID.
		 * 
		 * \return Returns the control ID.
		 **/
		WORD								Id() const { return m_wId; }

		/**
		 * Enabled or disabled.
		 * 
		 * \return Returns TRUE or FALSE.
		 **/
		BOOL								Enabled() const { return m_bEnabled; }

		/**
		 * Enables or disables the window.
		 * \brief Calls ::EnableWindow() and updates the internal enabled flag.
		 *
		 * \param _bEnable Non-zero to enable; zero to disable.
		 * \return Returns the previous enabled state as reported by ::EnableWindow().
		 */
		BOOL								SetEnabled( BOOL _bEnable ) { m_bEnabled = (_bEnable != 0); return ::EnableWindow( Wnd(), m_bEnabled ); }

		/**
		 * Sets whether textual numeric input is treated as hexadecimal.
		 * \brief Toggles the parsing mode between hex and non-hex.
		 *
		 * \param _bVal Non-zero to treat as hex; zero otherwise.
		 * \return Returns the previous flag value.
		 */
		BOOL								SetTreatAsHex( BOOL _bVal ) { BOOL bRet = m_bTreatAsHex; m_bTreatAsHex = _bVal; return bRet; }

		/**
		 * Does it treat text as hex by default?
		 * 
		 * \return Returns TRUE if the Expression treats standard decimal values as hexadecimal.
		 **/
		BOOL								TreatAsHex() const { return m_bTreatAsHex; }

		/**
		 * Sets the address read handler.
		 * \brief Installs a callback used to resolve addresses during expression evaluation.
		 *
		 * \param _pfahHandler Function pointer for address handling.
		 * \param _uiptrData Opaque user data passed to the handler.
		 */
		VOID								SetAddressHandler( ee::CExpEvalContainer::PfAddressHandler _pfahHandler, uintptr_t _uiptrData ) {
			m_pfahAddressHandler = _pfahHandler;
			m_uiptrAddressHandlerData = _uiptrData;
		}

		/**
		 * Sets the address write handler.
		 * \brief Installs a callback used to write to resolved addresses during evaluation.
		 *
		 * \param _pfahHandler Function pointer for address write handling.
		 * \param _uiptrData Opaque user data passed to the handler.
		 */
		VOID								SetAddressWriteHandler( ee::CExpEvalContainer::PfAddressHandler _pfahHandler, uintptr_t _uiptrData ) {
			m_pfahAddressWriteHandler = _pfahHandler;
			m_uiptrAddressWriteHandlerData = _uiptrData;
		}

		/**
		 * Gets the address read handler.
		 * \brief Returns the installed address resolution callback.
		 *
		 * \return Returns the current read handler, or nullptr if none.
		 */
		ee::CExpEvalContainer::PfAddressHandler
											GetAddressHandler() const { return m_pfahAddressHandler; }

		/**
		 * Gets the opaque data for the address read handler.
		 * \brief Returns the user data pointer associated with the read handler.
		 *
		 * \return Returns the user data value.
		 */
		uintptr_t							GetAddressHandlerData() const { return m_uiptrAddressHandlerData; }

		/**
		 * Queries visibility.
		 * \brief Returns TRUE if the window is visible.
		 *
		 * \return Returns non-zero if visible; otherwise zero.
		 */
		BOOL								Visible() const { return ::IsWindowVisible( Wnd() ); }

		/**
		 * Shows or hides the window.
		 * \brief Calls ::ShowWindow() with SW_SHOW or SW_HIDE.
		 *
		 * \param _bVis Non-zero to show; zero to hide.
		 * \return Returns non-zero if the window was previously visible; otherwise zero.
		 */
		BOOL								SetVisible( BOOL _bVis ) { return ::ShowWindow( Wnd(), _bVis ? SW_SHOW : SW_HIDE ); }

		/**
		 * Gets the current window style.
		 * \brief Retrieves GWL_STYLE via ::GetWindowLongW().
		 *
		 * \return Returns the current style bits.
		 */
		DWORD								GetStyle() const { return static_cast<DWORD>(::GetWindowLongW( Wnd(), GWL_STYLE )); }

		/**
		 * Gets the current extended window style.
		 * \brief Retrieves GWL_EXSTYLE via ::GetWindowLongW().
		 *
		 * \return Returns the current extended style bits.
		 */
		DWORD								GetStyleEx() const { return static_cast<DWORD>(::GetWindowLongW( Wnd(), GWL_EXSTYLE )); }

		/**
		 * Sets the window style.
		 * \brief Writes GWL_STYLE via ::SetWindowLongW().
		 *
		 * \param _dwStyle The new style bits.
		 * \return Returns the previous style.
		 */
		DWORD								SetStyle( DWORD _dwStyle ) { return static_cast<DWORD>(::SetWindowLongW( Wnd(), GWL_STYLE, static_cast<LONG>(_dwStyle) )); }

		/**
		 * Sets the extended window style.
		 * \brief Writes GWL_EXSTYLE via ::SetWindowLongW().
		 *
		 * \param _dwStyle The new extended style bits.
		 * \return Returns the previous extended style.
		 */
		DWORD								SetStyleEx( DWORD _dwStyle ) { return static_cast<DWORD>(::SetWindowLongW( Wnd(), GWL_EXSTYLE, static_cast<LONG>(_dwStyle) )); }

		/**
		 * Adds style flags.
		 * \brief OR's the given flags into the current style.
		 *
		 * \param _dwFlags Style bits to add.
		 * \return Returns the previous style.
		 */
		DWORD								AddStyle( DWORD _dwFlags ) { return SetStyle( GetStyle() | _dwFlags ); }

		/**
		 * Adds extended style flags.
		 * \brief OR's the given flags into the current extended style.
		 *
		 * \param _dwFlags Extended style bits to add.
		 * \return Returns the previous extended style.
		 */
		DWORD								AddStyleEx( DWORD _dwFlags ) { return SetStyleEx( GetStyleEx() | _dwFlags ); }

		/**
		 * Removes style flags.
		 * \brief Clears the given bits from the current style.
		 *
		 * \param _dwFlags Style bits to remove.
		 * \return Returns the previous style.
		 */
		DWORD								RemoveStyle( DWORD _dwFlags ) { return SetStyle( GetStyle() & ~_dwFlags ); }

		/**
		 * Removes extended style flags.
		 * \brief Clears the given bits from the current extended style.
		 *
		 * \param _dwFlags Extended style bits to remove.
		 * \return Returns the previous extended style.
		 */
		DWORD								RemoveStyleEx( DWORD _dwFlags ) { return SetStyleEx( GetStyleEx() & ~_dwFlags ); }

		/**
		 * Determines if the window has a menu.
		 * 
		 * \return Returns true if the window has a menu.
		 **/
		inline bool							HasMenu() { return Wnd() && (::GetMenu( Wnd() ) != nullptr); }

		/**
		 * Gets the window text (ANSI).
		 * \brief Copies the caption/control text into the provided buffer.
		 *
		 * \param _lpString Destination buffer.
		 * \param _nMaxCount Maximum number of characters to copy.
		 * \return Returns the number of characters copied, not including the null terminator.
		 */
		virtual INT							GetTextA( LPSTR _lpString, INT _nMaxCount ) const { return ::GetWindowTextA( Wnd(), _lpString, _nMaxCount ); }

		/**
		 * Gets the window text (Unicode).
		 * \brief Copies the caption/control text into the provided buffer.
		 *
		 * \param _lpString Destination buffer.
		 * \param _nMaxCount Maximum number of wide characters to copy.
		 * \return Returns the number of characters copied, not including the null terminator.
		 */
		virtual INT							GetTextW( LPWSTR _lpString, INT _nMaxCount ) const { return ::GetWindowTextW( Wnd(), _lpString, _nMaxCount ); }

		/**
		 * Gets the window text as std::string (ANSI).
		 * \brief Returns the control text encoded as the current ANSI code page.
		 *
		 * \return Returns the text string.
		 */
		virtual std::string					GetTextA() const;

		/**
		 * Gets the window text as std::wstring (Unicode).
		 * \brief Returns the control text as UTF-16.
		 *
		 * \return Returns the wide text string.
		 */
		virtual std::wstring				GetTextW() const;

		/**
		 * Gets the window text encoded as UTF-8.
		 * \brief Converts the control text to a UTF-8 std::string.
		 *
		 * \return Returns the UTF-8 text string.
		 */
		virtual std::string					GetTextUTF8() const;

		/**
		 * Gets the window text length (ANSI).
		 * \brief Retrieves the character count for the caption/control text.
		 *
		 * \return Returns the length in characters, excluding the null terminator.
		 */
		virtual INT							GetTextLengthA() const { return ::GetWindowTextLengthA( Wnd() ); }

		/**
		 * Gets the window text length (Unicode).
		 * \brief Retrieves the character count for the caption/control text.
		 *
		 * \return Returns the length in wide characters, excluding the null terminator.
		 */
		virtual INT							GetTextLengthW() const { return ::GetWindowTextLengthW( Wnd() ); }

		/**
		 * Sets the window text (ANSI).
		 * \brief Updates the caption/control text from an ANSI string.
		 *
		 * \param _lpString Null-terminated ANSI string.
		 * \return Returns non-zero on success; otherwise zero.
		 */
		virtual BOOL						SetTextA( LPCSTR _lpString )  { return ::SetWindowTextA( Wnd(), _lpString ); }

		/**
		 * Sets the window text (Unicode).
		 * \brief Updates the caption/control text from a UTF-16 string.
		 *
		 * \param _lpwString Null-terminated UTF-16 string.
		 * \return Returns non-zero on success; otherwise zero.
		 */
		virtual BOOL						SetTextW( LPCWSTR _lpwString ) { return ::SetWindowTextW( Wnd(), _lpwString ); }

		/**
		 * Evaluates the control text as an expression.
		 * \brief Parses the text and computes a value using the expression evaluator.
		 *
		 * The evaluator may consult the address handler to resolve symbols or memory references.
		 *
		 * \param _eResult Receives the evaluated result and type.
		 * \param _ui32Flags Evaluation flags controlling parsing behavior.
		 * \param _pbExpIsValid Optional pointer to receive TRUE if expression was valid.
		 * \param _psObjStringResult Optional pointer to receive object-string output.
		 * \return Returns TRUE on success; otherwise FALSE.
		 */
		BOOL								GetTextAsExpression( ee::CExpEvalContainer::EE_RESULT &_eResult, uint32_t _ui32Flags, BOOL * _pbExpIsValid = nullptr,
			std::string * _psObjStringResult = nullptr ) const;

		/**
		 * Evaluates the control text as a signed 64-bit expression.
		 * \brief Convenience wrapper that requests an int64_t result.
		 *
		 * \param _eResult Receives the evaluated result.
		 * \return Returns TRUE on success; otherwise FALSE.
		 */
		BOOL								GetTextAsInt64Expression( ee::CExpEvalContainer::EE_RESULT &_eResult ) const;

		/**
		 * Evaluates the control text as an unsigned 64-bit expression.
		 * \brief Convenience wrapper that requests a uint64_t result.
		 *
		 * \param _eResult Receives the evaluated result.
		 * \return Returns TRUE on success; otherwise FALSE.
		 */
		BOOL								GetTextAsUInt64Expression( ee::CExpEvalContainer::EE_RESULT &_eResult ) const;

		/**
		 * Evaluates the control text as a double-precision expression.
		 * \brief Convenience wrapper that requests a double result.
		 *
		 * \param _eResult Receives the evaluated result.
		 * \return Returns TRUE on success; otherwise FALSE.
		 */
		BOOL								GetTextAsDoubleExpression( ee::CExpEvalContainer::EE_RESULT &_eResult ) const;

		/**
		 * Sets the text limit for the control.
		 * 
		 * \param _iLen The maximum number of TCHARs the user can enter, not including the terminating null character. If this parameter is zero, the text length is limited to 0x7FFFFFFE characters.
		 **/
		virtual void						LimitText( int _iLen = 0 ) { static_cast<void>(_iLen); }

		/**
		 * Gives the keyboard focus to this window.
		 * \brief Calls ::SetFocus() and updates internal focus tracking.
		 *
		 * \return Returns the previously focused widget, or nullptr.
		 */
		CWidget *							SetFocus() const;

		/**
		 * Queries the internal focus flag.
		 * \brief Returns true if this widget currently believes it has focus.
		 *
		 * \return Returns true if focused; otherwise false.
		 */
		bool								GetFocus() const;

		/**
		 * Selects a range of text.
		 * \brief Base implementation is a no-op; text controls override.
		 *
		 * \param _iStart Inclusive start character index.
		 * \param _iEnd Exclusive end character index.
		 */
		virtual VOID						SetSel( INT /*_iStart*/, INT /*_iEnd*/ ) const {}

		/**
		 * Queries the checked state (tri-state capable).
		 * \brief Base implementation returns FALSE; buttons override.
		 *
		 * \return Returns non-zero if checked; otherwise zero.
		 */
		virtual BOOL						IsChecked() const { return FALSE; }

		/**
		 * Sets the check state.
		 * \brief Base implementation is a no-op; check/radio buttons override.
		 *
		 * \param _uCheck One of BST_UNCHECKED, BST_CHECKED, or BST_INDETERMINATE.
		 */
		virtual VOID						CheckButton( UINT /*_uCheck*/ ) {}

		/**
		 * Gets the check state.
		 * \brief Returns BST_* for tri-state-aware controls.
		 *
		 * \return Returns the check state constant.
		 **/
		virtual INT							CheckState() const { return BST_INDETERMINATE; }

		/**
		 * Checks the control.
		 * \brief Sets the state to BST_CHECKED.
		 */
		virtual VOID						Check() { CheckButton( BST_CHECKED ); }

		/**
		 * Unchecks the control.
		 * \brief Sets the state to BST_UNCHECKED.
		 */
		virtual VOID						UnCheck() { CheckButton( BST_UNCHECKED ); }

		/**
		 * Sets the check state from a boolean.
		 * \brief Maps TRUE to BST_CHECKED and FALSE to BST_UNCHECKED.
		 *
		 * \param _bChecked New checked state.
		 */
		virtual VOID						SetCheck( BOOL _bChecked ) { CheckButton( _bChecked ? BST_CHECKED : BST_UNCHECKED ); }

		/**
		 * Selects by item-data.
		 * \brief Base implementation returns CB_ERR; combo-like controls override.
		 *
		 * \param _pData Item-data value to match.
		 * \return Returns the index on success; otherwise CB_ERR.
		 */
		virtual INT							SetCurSelByItemData( LPARAM /*_pData*/ ) { return CB_ERR; }

		/**
		 * Gets the selected item's data.
		 * \brief Base returns -1; combo-like controls override.
		 *
		 * \return Returns the item-data for the current selection.
		 */
		virtual LPARAM						GetCurSelItemData() const { return LPARAM( -1 ); }

		/**
		 * Gets the selected index.
		 * \brief Base returns -1; list/combo controls override.
		 *
		 * \return Returns the zero-based index, or -1 if none.
		 */
		virtual INT							GetCurSel() const { return -1; }

		/**
		 * Notification for a child tab's toggle.
		 * \brief Called when a tab control child changes a check state.
		 *
		 * \param _pwTab Pointer to the tab control.
		 * \param _iTab Zero-based tab index that toggled.
		 */
		virtual void						TabToggled( CWidget * /*_pwTab*/, int /*_iTab*/ ) {}

		/**
		 * Sets the logical parent widget.
		 * \brief Updates the containment hierarchy and returns the previous parent.
		 *
		 * \param _pwParent The new parent widget.
		 * \return Returns the previous parent widget pointer.
		 */
		virtual CWidget *					SetParent( CWidget * _pwParent );

		/**
		 * Gets the window rectangle in screen coordinates.
		 * \brief Returns the current window rectangle via ::GetWindowRect().
		 *
		 * \param _pwChild Optional child pointer for specialized layouts (unused by base).
		 * \return Returns the rectangle in screen coordinates.
		 */
		virtual LSW_RECT					WindowRect( const CWidget * /*_pwChild*/ = nullptr ) const { LSW_RECT rTemp; ::GetWindowRect( Wnd(), &rTemp ); return rTemp; }

		/**
		 * Gets the client rectangle in client coordinates.
		 * \brief Returns the current client rectangle via ::GetClientRect().
		 *
		 * \param _pwChild Optional child pointer for specialized layouts (unused by base).
		 * \return Returns the client rectangle.
		 */
		virtual LSW_RECT					ClientRect( const CWidget * /*_pwChild*/ = nullptr ) const { LSW_RECT rTemp; ::GetClientRect( Wnd(), &rTemp ); return rTemp; }

		/**
		 * Gets the starting window rectangle.
		 * \brief Returns the initial rectangle captured at creation/layout time.
		 *
		 * \return Returns a reference to the starting rectangle.
		 */
		virtual const LSW_RECT &			StartRect() const { return m_rStartingRect; }

		/**
		 * Gets the starting client rectangle relative to the parent's starting client area.
		 * \brief Useful for proportional or anchored layout computations.
		 *
		 * \return Returns a reference to the starting client rectangle.
		 */
		virtual const LSW_RECT &			StartClientRect() const { return m_rStartingClientRect; }

		/**
		 * Gets the virtual client rectangle.
		 * \brief Allows parents to adjust available space for toolbars, status bars, or splitters.
		 *
		 * \param pwChild Optional child for which the virtual rect is requested.
		 * \return Returns the virtual client rectangle.
		 */
		virtual const LSW_RECT				VirtualClientRect( const CWidget * pwChild ) const { return ClientRect( pwChild ); }

		/**
		 * Updates cached rectangles from current window metrics.
		 * \brief Captures current WindowRect() and ClientRect() as the new base sizes.
		 */
		VOID								UpdateRects();

		/**
		 * Forces a size/layout update.
		 * \brief Sends a synthetic WM_SIZE to recompute layout and repaint.
		 */
		VOID								ForceSizeUpdate() {
			LSW_RECT rRect;
			// Send a fake WM_SIZE message to cause the window to recalculate and redraw its layout.
			::GetClientRect( Wnd(), &rRect );
			::SendMessageW( Wnd(), WM_SIZE, SIZE_RESTORED, MAKELPARAM( rRect.Width(), rRect.Height() ) );
		}

		/**
		 * Tests whether this widget owns a given child widget.
		 * \brief Walks the child list to locate the specified widget.
		 *
		 * \param _pwChild The child widget to find.
		 * \return Returns true if the child is contained; otherwise false.
		 */
		bool								HasChild( const CWidget * _pwChild ) const;

		/**
		 * Finds a child by control ID.
		 * \brief Returns a direct child whose ID matches _wId.
		 *
		 * \param _wId The child control ID.
		 * \return Returns a pointer to the child, or nullptr if not found.
		 */
		CWidget *							FindChild( WORD _wId );

		/**
		 * Finds a child by control ID (const).
		 * \brief Const overload returning a const pointer.
		 *
		 * \param _wId The child control ID.
		 * \return Returns a const pointer to the child, or nullptr if not found.
		 */
		const CWidget *						FindChild( WORD _wId ) const;

		/**
		 * Finds a parent by control ID.
		 * \brief Walks up the parent chain until an ID match is found.
		 *
		 * \param _wId The parent control ID.
		 * \return Returns a pointer to the parent, or nullptr if not found.
		 */
		CWidget *							FindParent( WORD _wId );

		/**
		 * Finds a parent by control ID (const).
		 * \brief Const overload returning a const pointer.
		 *
		 * \param _wId The parent control ID.
		 * \return Returns a const pointer to the parent, or nullptr if not found.
		 */
		const CWidget *						FindParent( WORD _wId ) const;

		/**
		 * Gets the current font.
		 * \brief Queries the control's HFONT via WM_GETFONT.
		 *
		 * \return Returns the HFONT, or nullptr if none.
		 */
		HFONT								GetFont() const { return reinterpret_cast<HFONT>(::SendMessageW( Wnd(), WM_GETFONT, 0L, 0L )); }

		/**
		 * Gets the widget type identifier.
		 * \brief Returns one of LSW_LT_* constants.
		 *
		 * \return Returns LSW_LT_WIDGET in the base class.
		 */
		virtual uint32_t					WidgetType() const { return LSW_LT_WIDGET; }

		/** \brief Returns true if this is a CButton. */
		virtual bool						IsButton() const { return false; }

		/** \brief Returns true if this is a CColorButton. */
		virtual bool						IsColorButton() const { return false; }

		/** \brief Returns true if this is a CCheckButton. */
		virtual bool						IsCheckButton() const { return false; }

		/** \brief Returns true if this is a CComboBox. */
		virtual bool						IsComboBox() const { return false; }

		/** \brief Returns true if this is a CComboBoxEx. */
		virtual bool						IsComboBoxEx() const { return false; }

		/** \brief Returns true if this is a CDockable. */
		virtual bool						IsDockable() const { return false; }

		/** \brief Returns true if this is a CEdit. */
		virtual bool						IsEdit() const { return false; }

		/** \brief Returns true if this is a CGroupBox. */
		virtual bool						IsGroupBox() const { return false; }

		/** \brief Returns true if this is a CListBox. */
		virtual bool						IsListBox() const { return false; }

		/** \brief Returns true if this is a CListView. */
		virtual bool						IsListView() const { return false; }

		/** \brief Returns true if this is a CMainWindow. */
		virtual bool						IsMainWindow() const { return false; }

		/** \brief Returns true if this is a CMultiSplitter. */
		virtual bool						IsMultiSplitter() const { return false; }

		/** \brief Returns true if this is a CTreeList. */
		virtual bool						IsTreeList() const { return false; }

		/** \brief Returns true if this is a CTreeListView. */
		virtual bool						IsTreeListView() const { return false; }

		/** \brief Returns true if this is a CProgressBar. */
		virtual bool						IsProgressBar() const { return false; }

		/** \brief Returns true if this is a CRadioButton. */
		virtual bool						IsRadioButton() const { return false; }

		/** \brief Returns true if this is a CRebar. */
		virtual bool						IsRebar() const { return false; }

		/** \brief Returns true if this is a CSplitter. */
		virtual bool						IsSplitter() const { return false; }

		/** \brief Returns true if this is a CStatic. */
		virtual bool						IsStatic() const { return false; }

		/** \brief Returns true if this is a CStatusBar. */
		virtual bool						IsStatusBar() const { return false; }

		/** \brief Returns true if this is a CTab. */
		virtual bool						IsTab() const { return false; }

		/** \brief Returns true if this is a CToolBar. */
		virtual bool						IsToolBar() const { return false; }

		/**
		 * Sets the logical parent pointer only.
		 * \brief Updates the stored parent widget without changing Win32 parentage.
		 *
		 * This does not call ::SetParent(); it only updates the internal pointer used by the layout
		 * and widget hierarchy.
		 *
		 * \param _pwParent The new parent widget pointer.
		 */
		void								SetWidgetParent( CWidget * _pwParent );

		/**
		 * Sets the small and big icons.
		 * 
		 * \param _hSmall The small icon handle or NULL.
		 * \param _hBig The big icon handle or NULL.
		 * \return Returns true if all icons were set.
		 **/
		bool								SetIcons( HICON _hSmall, HICON _hBig );

		/**
		 * Sets the small icon, returning the previous icon that was set.
		 * 
		 * \param _hSmall The small icon handle or NULL.
		 * \return Returns the previous icon.
		 **/
		HICON								SetSmallIcon( HICON _hSmall );

		/**
		 * Sets the big icon, returning the previous icon that was set.
		 * 
		 * \param _hBig The big icon handle or NULL.
		 * \return Returns the previous icon.
		 **/
		HICON								SetBigIcon( HICON _hBig );

		/**
		 * Sets user custom data.
		 * 
		 * \param _ui64Data The custom user value to associate with this object.
		 **/
		void								SetUserData( uint64_t _ui64Data ) { m_ui64UserData = _ui64Data; }

		/**
		 * Gets the user custom data.
		 * 
		 * \return Returns the user custom data.
		 **/
		uint64_t							GetUserData() const { return m_ui64UserData; }

		// Translate a child's tooltip text.
		virtual std::wstring				TranslateTooltip( const std::string &_sText );

		// Sets a given font on all children of a window.
		static BOOL CALLBACK				EnumChildWindows_SetFont( HWND _hWnd, LPARAM _lParam );

		// Converts a point from pixels to dialog units.
		static POINT						PixelsToDialogUnits( HWND _hWnd, LONG _lX, LONG _lY );


	protected :
		// == Members.
		/** User data. */
		uint64_t							m_ui64UserData;
		/** The window handle. */
		HWND								m_hWnd;
		/** The tooltip control. */
		HWND								m_hTooltip;
		/** The address handler. */
		ee::CExpEvalContainer::PfAddressHandler
											m_pfahAddressHandler;
		/** The data to be sent to the address handler. */
		uintptr_t							m_uiptrAddressHandlerData;
		/** The address write handler. */
		ee::CExpEvalContainer::PfAddressHandler
											m_pfahAddressWriteHandler;
		/** The data to be sent to the address write handler. */
		uintptr_t							m_uiptrAddressWriteHandlerData;
		/** The tooltip text. */
		std::string							m_sTooltipText;
		/** Children. */
		std::vector<CWidget *>				m_vChildren;
		/** Dock windows as children of this window. */
		std::vector<CDockable *>			m_vDockables;
		/** Parent. */
		CWidget *							m_pwParent;
		/** Original rectangle. */
		LSW_RECT							m_rStartingRect;
		/** This object's starting window rect in relationship with the parent's starting client rect. */
		LSW_RECT							m_rStartingClientRect;
		/** Extended styles. */
		DWORD								m_dwExtendedStyles;
		/** Tooltip styles. */
		DWORD								m_dwTooltipStyle;
		/** Tooltip extended styles. */
		DWORD								m_dwTooltipStyleEx;
		/** Last hit returned by NcHitTest(). */
		INT									m_iLastHit;
		/** Enabled. */
		BOOL								m_bEnabled;
		// The window rectangle.
		//LSW_RECT							m_rRect;
		// The client rectangle.
		//LSW_RECT							m_rClientRect;
		/** Default state.  Depends on the type of control. */
		BOOL								m_bActive;
		/** Treat text as hex when possible? */
		BOOL								m_bTreatAsHex;		
		/** Width expression. */
		CExpression							m_eWidth;
		/** Height expression. */
		CExpression							m_eHeight;
		/** Left expression. */
		CExpression							m_eLeft;
		/** Right expression. */
		CExpression							m_eRight;
		/** Top expression. */
		CExpression							m_eTop;
		/** Bottom expression. */
		CExpression							m_eBottom;
		/** Show as active or not. */
		BOOL								m_bShowAsActive;
		/** If in the destructor, the WM_NCDESTROY handler should not call delete. */
		BOOL								m_bInDestructor;
		/** Custom ID. */
		WORD								m_wId;
		/** The X DPI. */
		WORD								m_wDpiX = USER_DEFAULT_SCREEN_DPI;
		/** The Y DPI. */
		WORD								m_wDpiY = USER_DEFAULT_SCREEN_DPI;
		/** Tracks whether the control has focus or not. */
		bool								m_bHasFocus;

		/** The tooltip buffer. */
		static WCHAR						m_wcToolTipBuffer[1024*2];


		// == Message Handlers.
		/**
		 * Handles WM_NCCREATE.
		 * \brief Performs non-client-area initialization before window creation completes.
		 *
		 * Called after the window is allocated but before the client area is created. Use to
		 * parse CREATESTRUCTW and set window data that affects non-client metrics.
		 *
		 * \param _csCreateParms Reference to the CREATESTRUCTW passed by the system.
		 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
		 */
		virtual LSW_HANDLED					NcCreate( const CREATESTRUCTW &/*_csCreateParms*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_CREATE.
		 * \brief Performs client-area initialization when the window is created.
		 *
		 * Use to create child controls, allocate resources, and finish setup that requires
		 * a valid HWND and client area.
		 *
		 * \param _csCreateParms Reference to the CREATESTRUCTW passed by the system.
		 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
		 */
		virtual LSW_HANDLED					Create( const CREATESTRUCTW &/*_csCreateParms*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_INITDIALOG.
		 * \brief Initializes a dialog after creation and before it is shown.
		 *
		 * Use to set control defaults, attach data bindings, and position the dialog.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
		 */
		virtual LSW_HANDLED					InitDialog() { return LSW_H_CONTINUE; };

		/**
		 * Handles WM_DESTROY.
		 * \brief Cleans up client-area resources prior to window destruction.
		 *
		 * Release GDI objects, timers, and other resources allocated during Create() or runtime.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
		 */
		virtual LSW_HANDLED					Destroy() { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_NCDESTROY.
		 * \brief Final cleanup after the non-client area is destroyed.
		 *
		 * This is the last message a window receives. Use to clear pointers stored in GWLP_USERDATA
		 * and to finalize per-window allocations.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
		 */
		virtual LSW_HANDLED					NcDestroy() { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_CLOSE.
		 * \brief Requests window closure and gives a chance to veto or prompt the user.
		 *
		 * Use to confirm unsaved changes or to cancel the close. Post WM_DESTROY if approved.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
		 */
		virtual LSW_HANDLED					Close() { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_CANCELMODE.
		 * \brief Notifies the window that a modal loop is being canceled.
		 *
		 * Use to cancel tracking operations (captures, drags, menus) and restore state.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
		 */
		virtual LSW_HANDLED					CancelMode() { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_SIZE.
		 * \brief Responds to client-area size changes.
		 *
		 * Called after the window's client size changes. Override to update layouts,
		 * reposition children, or cache new sizes.
		 *
		 * \param _wParam The requested sizing type (e.g., SIZE_RESTORED, SIZE_MINIMIZED).
		 * \param _lWidth The new client width, in pixels.
		 * \param _lHeight The new client height, in pixels.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Size( WPARAM /*_wParam*/, LONG /*_lWidth*/, LONG /*_lHeight*/ );

		/**
		 * Handles WM_SIZING.
		 * \brief Intercepts interactive resizing and optionally constrains the drag rectangle.
		 *
		 * Modify *_prRect to enforce size limits or aspect ratios while the user resizes the window.
		 *
		 * \param _iEdge The edge (or corner) being dragged (WMSZ_*).
		 * \param _prRect Pointer to the current drag rectangle in screen coordinates.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Sizing( INT /*_iEdge*/, LSW_RECT * /*_prRect*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_SIZE when minimized.
		 * \brief Notified specifically for SIZE_MINIMIZED.
		 *
		 * \param _lWidth The new client width (usually 0 for minimized).
		 * \param _lHeight The new client height (usually 0 for minimized).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Minimized( LONG /*_lWidth*/, LONG /*_lHeight*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_MOVE.
		 * \brief Notified when the window is moved.
		 *
		 * \param _lX New x-position of the window (screen coordinates).
		 * \param _lY New y-position of the window (screen coordinates).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Move( LONG /*_lX*/, LONG /*_lY*/ );

		/**
		 * Handles WM_WINDOWPOSCHANGED.
		 * \brief Notified after size, position, or Z-order changes.
		 *
		 * \param _pwpPos Pointer to WINDOWPOS describing the change.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					WindowPosChanged( const WINDOWPOS * /*_pwpPos*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_GETMINMAXINFO.
		 * \brief Provides minimum/maximum tracking sizes.
		 *
		 * Override to fill *(_pmmiInfo) with size constraints.
		 *
		 * \param _pmmiInfo Pointer to a MINMAXINFO structure to populate.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					GetMinMaxInfo( MINMAXINFO * /*_pmmiInfo*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_DPICHANGED.
		 * \brief Responds to per-monitor DPI changes by resizing and repositioning the window.
		 *
		 * Use _pRect as the suggested new window rectangle scaled for the new DPI.
		 *
		 * \param _wX The new horizontal DPI.
		 * \param _wY The new vertical DPI.
		 * \param _pRect Suggested new window rectangle (screen coordinates).
		 * \return Returns an LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED					DpiChanged( WORD _wX, WORD _wY, LPRECT _pRect );

		/**
		 * Handles WM_SETFONT.
		 * \brief Receives notification that the control's font is changing.
		 *
		 * Override to cache the font or to trigger layout recalculation.
		 *
		 * \param _hFont The new font handle.
		 */
		virtual void						SetFont( HFONT /*_hFont*/ ) {}

		/**
		 * Handles WM_GETFONT.
		 * \brief Supplies the font to be used by the control.
		 *
		 * Return reinterpret_cast<HFONT>(-1) to let the default routine handle it; otherwise
		 * return NULL or a valid HFONT as appropriate.
		 *
		 * \return Returns a font handle or reinterpret_cast<HFONT>(-1) for default handling.
		 */
		virtual HFONT						GetFont() { return reinterpret_cast<HFONT>(-1); }

		/**
		 * Handles WM_COMMAND dispatch.
		 * \brief Routes command notifications from menus, accelerators, or child controls.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
		 * \param _wId The identifier for the command or control.
		 * \param _pwSrc The source control for control notifications; otherwise nullptr.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Command( WORD /*_wCtrlCode*/, WORD /*_wId*/, CWidget * /*_pwSrc*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_COMMAND from a menu.
		 * \brief Invoked for menu command selections.
		 *
		 * \param _Id The menu command identifier.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					MenuCommand( WORD /*_Id*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_COMMAND from an accelerator.
		 * \brief Invoked for accelerator keystrokes.
		 *
		 * \param _Id The accelerator command identifier.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					AcceleratorCommand( WORD /*_Id*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_NOTIFY -> NM_DBLCLK for this item.
		 * \brief Item double-click notification for child; bubbles to parent on continue.
		 *
		 * \param _phHdr The item-activation header for the double-click.
		 * \return Returns a LSW_HANDLED code. LSW_H_CONTINUE bubbles to the owning window.
		 */
		virtual LSW_HANDLED					DblClk( const LPNMITEMACTIVATE _phHdr ) {
			if ( m_pwParent ) { return m_pwParent->DblClk( _phHdr ); }
			return LSW_H_CONTINUE;
		}

		/**
		 * Handles NM_DBLCLK at the owning window.
		 * \brief Fallback when the child could not be resolved or continued.
		 *
		 * \param _phHdr The item-activation header for the double-click.
		 * \param _wControlId The child control ID.
		 * \param _pwWidget The child widget pointer.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					DblClk( const LPNMITEMACTIVATE /*_phHdr*/, WORD /*_wControlId*/, CWidget * /*_pwWidget*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_SETFOCUS.
		 * \brief Notified when the window receives keyboard focus.
		 *
		 * \param _hPrevFocus The window that previously had focus.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					SetFocus( HWND /*_hPrevFocus*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_KILLFOCUS.
		 * \brief Notified when the window loses keyboard focus.
		 *
		 * \param _hNewFocus The window receiving focus (may be NULL).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					KillFocus( HWND /*_hNewFocus*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_ERASEBKGND.
		 * \brief Allows custom background erasing.
		 *
		 * \param _hDc Device context provided for erasing.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					EraseBkgnd( HDC /*_hDc*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_PAINT.
		 * \brief Performs painting for the client area.
		 *
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Paint() { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_NCPAINT.
		 * \brief Performs painting for the non-client area.
		 *
		 * \param _hRgn Update region for non-client painting.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					NcPaint( HRGN /*_hRgn*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_CTLCOLOREDIT.
		 * \brief Customizes colors for edit controls.
		 *
		 * \param _hDc Device context for the control.
		 * \param _pwControl The edit control widget.
		 * \param _hBrush Output brush used to paint the background.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					CtlColorEdit( HDC /*_hDc*/, CWidget * /*_pwControl*/, HBRUSH &/*_hBrush*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_CTLCOLORLISTBOX.
		 * \brief Customizes colors for list box controls.
		 *
		 * \param _hDc Device context for the control.
		 * \param _pwControl The list box widget.
		 * \param _hBrush Output brush used to paint the background.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					CtlColorListBox( HDC /*_hDc*/, CWidget * /*_pwControl*/, HBRUSH &/*_hBrush*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_CTLCOLORSTATIC.
		 * \brief Customizes colors for static controls.
		 *
		 * \param _hDc Device context for the static control.
		 * \param _pwControl The static control widget.
		 * \param _hBrush Output brush used to paint the background.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					CtlColorStatic( HDC /*_hDc*/, CWidget * /*_pwControl*/, HBRUSH &/*_hBrush*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_CTLCOLORDLG.
		 * \brief Customizes colors for dialog boxes.
		 *
		 * \param _hDc Device context for the dialog box.
		 * \param _pwControl The dialog widget.
		 * \param _hBrush Output brush used to paint the background.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					CtlColorDlg( HDC /*_hDc*/, CWidget * /*_pwControl*/, HBRUSH &/*_hBrush*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_SETCURSOR.
		 * \brief Sets the cursor for the window or its child controls.
		 *
		 * \param _pwControl The control under the cursor, or nullptr.
		 * \param _wHitTest Hit-test code (HT*).
		 * \param _wIdent Mouse message identifier (e.g., WM_MOUSEMOVE).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					SetCursor( CWidget * /*_pwControl*/, WORD /*_wHitTest*/, WORD /*_wIdent*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_ACTIVATE.
		 * \brief Notified when the window is activated or deactivated.
		 *
		 * \param _bMinimized TRUE if minimized when activated.
		 * \param _wActivationMode WA_ACTIVE/WA_CLICKACTIVE/WA_INACTIVE.
		 * \param _pwWidget The widget being activated or deactivated.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Activate( BOOL /*_bMinimized*/, WORD /*_wActivationMode*/, CWidget * /*_pwWidget*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_NCACTIVATE.
		 * \brief Updates the title-bar active state.
		 *
		 * \param _bTitleBarActive TRUE if the non-client area should show active state.
		 * \param _lParam Reserved.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					NcActivate( BOOL _bTitleBarActive, LPARAM /*_lParam*/ ) { m_bShowAsActive = _bTitleBarActive; return LSW_H_CONTINUE; }

		/**
		 * Handles WM_ENABLE.
		 * \brief Notified when the enabled state changes.
		 *
		 * \param _bEnabled TRUE if enabled; otherwise FALSE.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Enable( BOOL /*_bEnabled*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_CAPTURECHANGED.
		 * \brief Notified when mouse capture changes to another window.
		 *
		 * \param _pwNewCaptureOwner The new capture owner widget, or nullptr.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					CaptureChanged( CWidget * /*_pwNewCaptureOwner*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_HSCROLL.
		 * \brief Horizontal scroll-bar notifications.
		 *
		 * \param _uScrollPos The position, if provided by the scroll code.
		 * \param _uScrollType The scroll request (SB_*).
		 * \param _pwWidget The scrolling control.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					HScroll( USHORT /*_uScrollPos*/, USHORT /*_uScrollType*/, CWidget * /*_pwWidget*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_VSCROLL.
		 * \brief Vertical scroll-bar notifications.
		 *
		 * \param _uScrollPos The position, if provided by the scroll code.
		 * \param _uScrollType The scroll request (SB_*).
		 * \param _pwWidget The scrolling control.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					VScroll( USHORT /*_uScrollPos*/, USHORT /*_uScrollType*/, CWidget * /*_pwWidget*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_INPUT.
		 * \brief Processes raw input from human interface devices.
		 *
		 * \param _iCode Input code (RIM_INPUT or RIM_INPUTSINK).
		 * \param _hRawInput Handle used with ::GetRawInputData() to retrieve raw data.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Input( INT /*_iCode*/, HRAWINPUT /*_hRawInput*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_INPUT_DEVICE_CHANGE.
		 * \brief Notified when raw-input devices are added or removed.
		 *
		 * \param _iNotifCode GIDC_ARRIVAL or GIDC_REMOVAL.
		 * \param _hDevice Handle to the raw input device.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					InputDeviceChanged( INT /*_iNotifCode*/, HANDLE /*_hDevice*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_GETDLGCODE.
		 * \brief Specifies which input keys the control wants to process.
		 *
		 * Override to return a mask of DLGC_* flags. The base returns 0xFFFF to opt in broadly,
		 * including Tab processing.
		 *
		 * \param _wKey The virtual key that prompted the query.
		 * \return Returns a DLGC_* mask.
		 **/
		virtual WORD						GetDlgCode( WORD /*_wKey*/ ) { return 0xFFFF;/*DLGC_WANTTAB*/; }

		/**
		 * Handles WM_KEYDOWN.
		 * \brief Notified when a non-system key is pressed.
		 *
		 * \param _uiKeyCode Virtual-key code.
		 * \param _uiFlags Repeat/scan/extended/context/previous/transition flags.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					KeyDown( UINT /*_uiKeyCode*/, UINT /*_uiFlags*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_KEYUP.
		 * \brief Notified when a non-system key is released.
		 *
		 * \param _uiKeyCode Virtual-key code.
		 * \param _uiFlags Repeat/scan/extended/context/previous/transition flags.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					KeyUp( UINT /*_uiKeyCode*/, UINT /*_uiFlags*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_SYSKEYDOWN.
		 * \brief Notified when a system key (Alt-combo) is pressed.
		 *
		 * \param _uiKeyCode Virtual-key code.
		 * \param _uiFlags Repeat/scan/extended/context/previous/transition flags.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					SysKeyDown( UINT /*_uiKeyCode*/, UINT /*_uiFlags*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_SYSKEYUP.
		 * \brief Notified when a system key (Alt-combo) is released.
		 *
		 * \param _uiKeyCode Virtual-key code.
		 * \param _uiFlags Repeat/scan/extended/context/previous/transition flags.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					SysKeyUp( UINT /*_uiKeyCode*/, UINT /*_uiFlags*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_LBUTTONDBLCLK. */
		virtual LSW_HANDLED					LButtonDblClk( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_LBUTTONDOWN. */
		virtual LSW_HANDLED					LButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_LBUTTONUP. */
		virtual LSW_HANDLED					LButtonUp( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_MBUTTONDBLCLK. */
		virtual LSW_HANDLED					MButtonDblClk( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_MBUTTONDOWN. */
		virtual LSW_HANDLED					MButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_MBUTTONUP. */
		virtual LSW_HANDLED					MButtonUp( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_MOUSEACTIVATE.
		 * \brief Determines activation behavior for mouse input.
		 *
		 * \param _pwTopLevelParent The top-level parent widget.
		 * \param _iHitTest Hit-test code.
		 * \param _iReturnCode In/out return code (MA_*).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					MouseActivate( CWidget * /*_pwTopLevelParent*/, INT /*_iHitTest*/, INT &/*_iReturnCode*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_MOUSEHOVER. */
		virtual LSW_HANDLED					MouseHover( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_MOUSEHWHEEL. */
		virtual LSW_HANDLED					MouseHWheel( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_MOUSELEAVE. */
		virtual LSW_HANDLED					MouseLeave()  { return LSW_H_CONTINUE; }

		/** \brief Handles WM_MOUSEMOVE. */
		virtual LSW_HANDLED					MouseMove( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_MOUSEWHEEL. */
		virtual LSW_HANDLED					MouseWheel( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_NCHITTEST.
		 * \brief Determines the position of the cursor hot spot in the non-client area.
		 *
		 * \param _pCursorPos Screen coordinates of the cursor.
		 * \param _iReturnHitTest Receives the HT* result.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED					NcHitTest( const POINTS &/*_pCursorPos*/, INT &/*_iReturnHitTest*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCLBUTTONDBLCLK. */
		virtual LSW_HANDLED					NcButtonDblClk( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCLBUTTONDOWN. */
		virtual LSW_HANDLED					NcLButtonDown( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCLBUTTONUP. */
		virtual LSW_HANDLED					NcLButtonUp( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCMBUTTONDBLCLK. */
		virtual LSW_HANDLED					NcMButtonDblClk( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCMBUTTONDOWN. */
		virtual LSW_HANDLED					NcMButtonDown( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCMBUTTONUP. */
		virtual LSW_HANDLED					NcMButtonUp( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCMOUSEHOVER. */
		virtual LSW_HANDLED					NcMouseHover( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCMOUSELEAVE. */
		virtual LSW_HANDLED					NcMouseLeave() { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCMOUSEMOVE. */
		virtual LSW_HANDLED					NcMouseMove( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCRBUTTONDBLCLK. */
		virtual LSW_HANDLED					NcRButtonDblClk( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCRBUTTONDOWN. */
		virtual LSW_HANDLED					NcRButtonDown( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCRBUTTONUP. */
		virtual LSW_HANDLED					NcRButtonUp( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCXBUTTONDBLCLK. */
		virtual LSW_HANDLED					NcXButtonDblClk( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCXBUTTONDOWN. */
		virtual LSW_HANDLED					NcXButtonDown( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_NCXBUTTONUP. */
		virtual LSW_HANDLED					NcXButtonUp( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_RBUTTONDBLCLK. */
		virtual LSW_HANDLED					RButtonDblClk( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_RBUTTONDOWN. */
		virtual LSW_HANDLED					RButtonDown( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_RBUTTONUP. */
		virtual LSW_HANDLED					RButtonUp( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_XBUTTONDBLCLK. */
		virtual LSW_HANDLED					XButtonDblClk( DWORD /*_dwVirtKeys*/, DWORD /*_dwButton*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_XBUTTONDOWN. */
		virtual LSW_HANDLED					XButtonDown( DWORD /*_dwVirtKeys*/, DWORD /*_dwButton*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/** \brief Handles WM_XBUTTONUP. */
		virtual LSW_HANDLED					XButtonUp( DWORD /*_dwVirtKeys*/, DWORD /*_dwButton*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles TBN_QUERYINSERT.
		 * \brief Determines whether a toolbar button can be inserted.
		 *
		 * \param _lptbToolBar Notification data for the toolbar.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					TbnQueryInsert( const LPNMTOOLBARW /*_lptbToolBar*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles TBN_QUERYDELETE.
		 * \brief Determines whether a toolbar button can be deleted.
		 *
		 * \param _lptbToolBar Notification data for the toolbar.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					TbnQueryDelete( const LPNMTOOLBARW /*_lptbToolBar*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles TBN_RESET.
		 * \brief Notified when the toolbar is resetting to default settings.
		 *
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					TbnReset() { return LSW_H_CONTINUE; }

		/**
		 * Handles TBN_GETBUTTONINFO.
		 * \brief Supplies button information during toolbar customization.
		 *
		 * \param _lptbToolBar In/out structure describing the requested button.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					TbnGetButtonInfo( LPNMTOOLBARW /*_lptbToolBar*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_DEVICECHANGE.
		 * \brief Notified of device arrival/removal and related events.
		 *
		 * Return LSW_H_HANDLED to veto queries (equivalent to BROADCAST_QUERY_DENY).
		 *
		 * \param _wDbtEvent The DBT_* event code.
		 * \param _lParam Event-specific data pointer (varies by event).
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED					DeviceChange( WORD /*_wDbtEvent*/, LPARAM /*_lParam*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_SYSCOMMAND.
		 * \brief Processes system commands such as SC_CLOSE or SC_MAXIMIZE.
		 *
		 * \param _wCommand The SC_* command.
		 * \param _iMouseX Mouse x-position (screen), if invoked by mouse; otherwise unused.
		 * \param _iMouseY Mouse y-position (screen), if invoked by mouse; 1 for accelerator; 0 for mnemonic.
		 * \return Returns a LSW_HANDLED code. LSW_H_HANDLED prevents default processing.
		 **/
		virtual LSW_HANDLED					SysCommand( WORD /*_wCommand*/, int /*_iMouseX*/, int /*_iMouseY*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Notification: LVN_GETDISPINFO.
		 * \brief Supplies on-demand text or image data for a virtual list-view.
		 *
		 * \param _plvdiInfo Pointer to NMLVDISPINFOW to fill.
		 * \return Returns TRUE if handled; otherwise FALSE.
		 **/
		virtual BOOL						GetDispInfoNotify( NMLVDISPINFOW * /*_plvdiInfo*/ ) { return FALSE; }

		/**
		 * Notification: CBEN_GETDISPINFO.
		 * \brief Supplies on-demand item data for a ComboBoxEx control.
		 *
		 * \param _plvdiInfo Pointer to NMCOMBOBOXEXW to fill.
		 * \return Returns TRUE if handled; otherwise FALSE.
		 **/
		virtual BOOL						GetDispInfoNotify( NMCOMBOBOXEXW * /*_plvdiInfo*/ ) { return FALSE; }

		/**
		 * Handles WM_NOTIFY -> LVN_ITEMCHANGED.
		 * \brief Notified when a list-view item's state or data changes.
		 *
		 * \param _lplvParm Pointer to NMLISTVIEW notification data.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Notify_ItemChanged( LPNMLISTVIEW /*_lplvParm*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_NOTIFY -> LVN_ODSTATECHANGED.
		 * \brief Notified when owner-data list-view item state changes.
		 *
		 * \param _lposcParm Pointer to NMLVODSTATECHANGE notification data.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Notify_OdStateChange( LPNMLVODSTATECHANGE /*_lposcParm*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Custom-draw: CDDS_PREPAINT.
		 * \brief Allows pre-paint handling for the list-view control.
		 *
		 * \param _lpcdParm Pointer to NMLVCUSTOMDRAW.
		 * \return Returns CDRF_* flags.
		 */
		virtual DWORD						Notify_CustomDraw_PrePaint( LPNMLVCUSTOMDRAW /*_lpcdParm*/ ) { return CDRF_DODEFAULT; }

		/**
		 * Custom-draw: CDDS_ITEMPREPAINT.
		 * \brief Allows per-item pre-paint customization.
		 *
		 * \param _lpcdParm Pointer to NMLVCUSTOMDRAW.
		 * \return Returns CDRF_* flags.
		 */
		virtual DWORD						Notify_CustomDraw_ItemPrePaint( LPNMLVCUSTOMDRAW /*_lpcdParm*/ ) { return CDRF_DODEFAULT; }

		/**
		 * Custom-draw: CDDS_ITEMPREPAINT | CDDS_SUBITEM.
		 * \brief Allows per-subitem pre-paint customization.
		 *
		 * \param _lpcdParm Pointer to NMLVCUSTOMDRAW.
		 * \return Returns CDRF_* flags.
		 */
		virtual DWORD						Notify_CustomDraw_ItemPrePaintSubItem( LPNMLVCUSTOMDRAW /*_lpcdParm*/ ) { return CDRF_DODEFAULT; }

		/**
		 * Custom-draw: CDDS_ITEMPOSTPAINT.
		 * \brief Allows per-item post-paint customization.
		 *
		 * \param _lpcdParm Pointer to NMLVCUSTOMDRAW.
		 * \return Returns CDRF_* flags.
		 */
		virtual DWORD						Notify_CustomDraw_ItemPostPaint( LPNMLVCUSTOMDRAW /*_lpcdParm*/ ) { return CDRF_DODEFAULT; }

		/**
		 * Handles WM_TIMER.
		 * \brief Notified when a timer elapses.
		 *
		 * \param _uiptrId Timer identifier.
		 * \param _tpProc Optional callback associated with the timer.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Timer( UINT_PTR /*_uiptrId*/, TIMERPROC /*_tpProc*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_HOTKEY.
		 * \brief Notified when a registered hotkey is pressed.
		 *
		 * \param _iIdentifier Hotkey ID.
		 * \param _iVirtualKey Virtual-key code.
		 * \param _iMod Modifier flags (MOD_*).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Hotkey( INT /*_iIdentifier*/, INT /*_iVirtualKey*/, INT /*_iMod*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_CONTEXTMENU.
		 * \brief Notified when a context menu is requested.
		 *
		 * \param _pwControl The control under the cursor, if any.
		 * \param _iX Screen x-position.
		 * \param _iY Screen y-position.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					ContextMenu( CWidget * /*_pwControl*/, INT /*_iX*/, INT /*_iY*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_INITMENU.
		 * \brief Sent when a menu is about to become active. It occurs when the user clicks an item on the menu bar or presses a menu key. This allows the application to modify the menu before it is displayed.
		 *
		 * \param _hMenu Handle to the popup or submenu.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					InitMenu( HMENU /*_hMenu*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles WM_INITMENUPOPUP.
		 * \brief Initializes a popup or submenu before it is displayed.
		 *
		 * \param _hMenu Handle to the popup or submenu.
		 * \param _wPos Zero-based position of the menu item that opened the popup.
		 * \param _bIsWindowMenu TRUE if the menu is the window menu.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					InitMenuPopup( HMENU /*_hMenu*/, WORD /*_wPos*/, BOOL /*_bIsWindowMenu*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Custom user-defined message handler.
		 * \brief Handles WM_USER and other private messages.
		 *
		 * \param _uMsg Message identifier.
		 * \param _wParam WPARAM data.
		 * \param _lParam LPARAM data.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					CustomPrivateMsg( UINT /*_uMsg*/, WPARAM /*_wParam*/, LPARAM /*_lParam*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Provides text for a CTreeListView item on demand.
		 * \brief Supports dynamic, real-time item text generation.
		 *
		 * Return a pointer to a persistent std::wstring containing the text. You may store the text
		 * in _wsOptionalBuffer and return &_wsOptionalBuffer for convenience. Return nullptr to use
		 * the item's existing text (SetItemText()).
		 *
		 * \param _pwSrc The requesting tree-list view widget.
		 * \param _iItem The zero-based item index.
		 * \param _iSubItem The zero-based column index.
		 * \param _lpParam The lParam associated with the item.
		 * \param _wsOptionalBuffer Optional buffer for returning persistent text.
		 * \return Returns a pointer to the text to display, or nullptr to use the stored item text.
		 **/
		virtual std::wstring *				TreeListView_ItemText( CWidget * _pwSrc, int _iItem, int _iSubItem, LPARAM _lpParam, std::wstring &_wsOptionalBuffer ) {
			if ( m_pwParent ) { return m_pwParent->TreeListView_ItemText( _pwSrc, _iItem, _iSubItem, _lpParam, _wsOptionalBuffer ); }
			return nullptr;
		}


		// == Functions.
		/**
		 * Removes a child widget from this widget.
		 * \brief Detaches the child from the internal child list.
		 *
		 * \param _pwChild The child to remove.
		 */
		virtual void						RemoveChild( const CWidget * _pwChild );

		/**
		 * Adds a child widget to this widget.
		 * \brief Attaches the child to the internal child list.
		 *
		 * \param _pwChild The child to add.
		 */
		void								AddChild( CWidget * _pwChild );

		/**
		 * Notified when a child is removed.
		 * \brief Called on this widget and on the widget that lost a child for convenience.
		 *
		 * \param _pwChild The child that was removed.
		 */
		virtual void						ChildWasRemoved( const CWidget * _pwChild );

		/**
		 * Notified when a child tab closes one of its headers.
		 * \brief Bubbles the notification to the parent by default.
		 *
		 * \param _pwChild The child tab control.
		 * \param _iTab The index of the closed tab.
		 */
		virtual void						ChildTabClosed( CWidget * _pwChild, INT _iTab ) {
			if ( Parent() ) {
				Parent()->ChildTabClosed( _pwChild, _iTab );
			}
		}

		/**
		 * Evaluates expressions to compute a new size for the control.
		 * \brief Applies expression-driven layout to determine bounds.
		 */
		virtual void						EvalNewSize();

		/**
		 * Attaches an HWND to this widget after creation.
		 * \brief Finalizes control initialization once the window handle exists.
		 *
		 * \param _hWnd The created window handle.
		 */
		virtual void						InitControl( HWND _hWnd );

		/**
		 * Adds a dockable window to this widget's dockable list.
		 * \brief Registers a CDockable for activation/visibility management.
		 *
		 * \param _pdDock The dockable window pointer.
		 */
		void								AddDockable( CDockable * _pdDock );

		/**
		 * Removes a dockable window from this widget's dockable list.
		 * \brief Unregisters a CDockable from management.
		 *
		 * \param _pdDock The dockable window pointer.
		 */
		void								RemDockable( CDockable * _pdDock );

		/**
		 * Retrieves dockable windows.
		 * \brief Fills a vector with dockable widgets, optionally including this widget.
		 *
		 * \param _vReturn Output vector receiving dockables.
		 * \param _bIncludeParent TRUE to include this widget in the list.
		 */
		void								GetDockables( std::vector<CWidget *> &_vReturn, BOOL _bIncludeParent );

		/**
		 * Handles WM_NCACTIVATE for dockable windows.
		 * \brief Call on the owner window to propagate activation to tool windows.
		 *
		 * \param _pwWnd Pointer to the window that received WM_NCACTIVATE.
		 * \param _wParam WPARAM from WM_NCACTIVATE.
		 * \param _lParam LPARAM from WM_NCACTIVATE.
		 * \param _bCallDefault TRUE to call the default procedure.
		 * \return Returns the LRESULT from handling or default processing.
		 */
		LRESULT CALLBACK					DockNcActivate( CWidget * _pwWnd, WPARAM _wParam, LPARAM _lParam, BOOL _bCallDefault );

		/**
		 * Handles WM_ENABLE for dockable windows.
		 * \brief Call on the owner window to propagate enable/disable state.
		 *
		 * \param _pwWnd Pointer to the window that received WM_ENABLE.
		 * \param _wParam WPARAM from WM_ENABLE.
		 * \param _lParam LPARAM from WM_ENABLE.
		 * \param _bCallDefault TRUE to call the default procedure.
		 * \return Returns the LRESULT from handling or default processing.
		 */
		LRESULT CALLBACK					DockEnable( CWidget * _pwWnd, WPARAM _wParam, LPARAM _lParam, BOOL _bCallDefault );

		/**
		 * EnumChildWindows callback that attaches HWNDs to CWidget instances.
		 * \brief Associates child windows with their owning widgets.
		 *
		 * \param _hWnd Child window handle.
		 * \param _lParam Caller-supplied parameter.
		 * \return Returns TRUE to continue enumeration; otherwise FALSE.
		 */
		static BOOL CALLBACK				EnumChildWindows_AttachWindowToWidget( HWND _hWnd, LPARAM _lParam );

		/**
		 * EnumChildWindows callback that applies enable/disable state.
		 * \brief Sets enabled state on enumerated children.
		 *
		 * \param _hWnd Child window handle.
		 * \param _lParam Caller-supplied parameter.
		 * \return Returns TRUE to continue enumeration; otherwise FALSE.
		 */
		static BOOL CALLBACK				EnumChildWindows_SetEnabled( HWND _hWnd, LPARAM _lParam );

		/**
		 * EnumChildWindows callback that stores starting rectangles.
		 * \brief Captures initial window/client rectangles for layout.
		 *
		 * \param _hWnd Child window handle.
		 * \param _lParam Caller-supplied parameter.
		 * \return Returns TRUE to continue enumeration; otherwise FALSE.
		 */
		static BOOL CALLBACK				EnumChildWindows_SetStartingRect( HWND _hWnd, LPARAM _lParam );

		/**
		 * EnumChildWindows callback that resizes controls on parent resize.
		 * \brief Performs per-child layout when the window size changes.
		 *
		 * \param _hWnd Child window handle.
		 * \param _lParam Caller-supplied parameter (typically new client rect).
		 * \return Returns TRUE to continue enumeration; otherwise FALSE.
		 */
		static BOOL CALLBACK				EnumChildWindows_ResizeControls( HWND _hWnd, LPARAM _lParam );

		/**
		 * Expression evaluator user-variable handler for "??".
		 * \brief Resolves "??" to the current widget pointer in expressions.
		 *
		 * \param _uiptrData Opaque user data provided by the caller.
		 * \param _peecContainer Evaluator container.
		 * \param _rResult Receives the resolved result.
		 * \return Returns true on success; otherwise false.
		 */
		static bool __stdcall				WidgetUserVarHandler( uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );

		/**
		 * Expression evaluator member-access handler.
		 * \brief Resolves member access on the left-hand result for widget fields.
		 *
		 * \param _rLeft The left-hand evaluated result.
		 * \param _sMember The member name requested.
		 * \param _uiptrData Opaque user data provided by the caller.
		 * \param _peecContainer Evaluator container.
		 * \param _rResult Receives the resolved result.
		 * \return Returns true on success; otherwise false.
		 */
		static bool __stdcall				WidgetMemberAccessHandler( const ee::CExpEvalContainer::EE_RESULT &_rLeft, const std::string &_sMember, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );

		/**
		 * Performs control setup for a list of widgets.
		 * \brief Creates and initializes child controls under a parent.
		 *
		 * \param _pwParent The parent widget.
		 * \param _vWidgetList The list of widgets to set up.
		 */
		static VOID							ControlSetup( CWidget * _pwParent, const std::vector<CWidget *> &_vWidgetList );

		/**
		 * Shared window/dialog procedure.
		 * \brief Default message handler for both windows and dialogs.
		 *
		 * \param _hWnd Target window handle.
		 * \param _uMsg Message identifier.
		 * \param _wParam WPARAM data.
		 * \param _lParam LPARAM data.
		 * \param _bIsDlg TRUE if a dialog; otherwise FALSE.
		 * \param _lrWndResult Out parameter receiving the LRESULT for window procs.
		 * \param _ipDiagResult Out parameter receiving the INT_PTR for dialog procs.
		 */
		static VOID CALLBACK				WndDlgProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, BOOL _bIsDlg, LRESULT &_lrWndResult, INT_PTR &_ipDiagResult );


	};

}	// namespace lsw

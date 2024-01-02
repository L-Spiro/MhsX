#pragma once

#include "../LSWWin.h"
#include "../Expressions/LSWExpression.h"
#include "../Helpers/LSWHelpers.h"
#include "../Layout/LSWWidgetLayout.h"

#include <vector>


#define LSW_WIN2CLASS( H )					reinterpret_cast<CWidget *>(::GetWindowLongPtrW( H, GWLP_USERDATA ))

namespace lsw {

	class CWidget {
		friend class						CDockable;
		friend class						CLayoutManager;
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
		// The default message handler.
		static LRESULT CALLBACK				WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

		// The default dialog message handler.
		static INT_PTR CALLBACK				DialogProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );


		// The Window handle.
		HWND								Wnd() const { return m_hWnd; }

		// The parent widget.
		CWidget *							Parent() { return m_pwParent; }

		// The parent widget.
		const CWidget *						Parent() const { return m_pwParent; }

		// The ancestor widget.
		CWidget *							Ancestor();

		// The ancestor widget.
		const CWidget *						Ancestor() const;

		// Custom ID.
		WORD								Id() const { return m_wId; }

		// Enabled or disabled.
		BOOL								Enabled() const { return m_bEnabled; }

		// Enable or disable.
		BOOL								SetEnabled( BOOL _bEnable ) { m_bEnabled = (_bEnable != 0); return ::EnableWindow( Wnd(), m_bEnabled ); }

		// Set treats all as hex or not.
		BOOL								SetTreatAsHex( BOOL _bVal ) { BOOL bRet = m_bTreatAsHex; m_bTreatAsHex = _bVal; return bRet; }

		// Does it treat text as hex by default?
		BOOL								TreatAsHex() const { return m_bTreatAsHex; }

		// Sets the address handler.
		VOID								SetAddressHandler( ee::CExpEvalContainer::PfAddressHandler _pfahHandler, uintptr_t _uiptrData ) {
			m_pfahAddressHandler = _pfahHandler;
			m_uiptrAddressHandlerData = _uiptrData;
		}

		// Sets the address write handler.
		VOID								SetAddressWriteHandler( ee::CExpEvalContainer::PfAddressHandler _pfahHandler, uintptr_t _uiptrData ) {
			m_pfahAddressWriteHandler = _pfahHandler;
			m_uiptrAddressWriteHandlerData = _uiptrData;
		}

		// Gets the address handler.
		ee::CExpEvalContainer::PfAddressHandler
											GetAddressHandler() const { return m_pfahAddressHandler; }

		// Gets the data for the address handler.
		uintptr_t							GetAddressHandlerData() const { return m_uiptrAddressHandlerData; }

		// Is visible.
		BOOL								Visible() const { return ::IsWindowVisible( Wnd() ); }

		// Set visible or not.
		BOOL								SetVisible( BOOL _bVis ) { return ::ShowWindow( Wnd(), _bVis ? SW_SHOW : SW_HIDE ); }

		// Get the window style.
		DWORD								GetStyle() const { return static_cast<DWORD>(::GetWindowLongW( Wnd(), GWL_STYLE )); }

		// Get the window extended style.
		DWORD								GetStyleEx() const { return static_cast<DWORD>(::GetWindowLongW( Wnd(), GWL_EXSTYLE )); }

		// Set the window style.  Returns the previous style.
		DWORD								SetStyle( DWORD _dwStyle ) { return static_cast<DWORD>(::SetWindowLongW( Wnd(), GWL_STYLE, static_cast<LONG>(_dwStyle) )); }

		// Set the window extended style.  Returns the previous extended style.
		DWORD								SetStyleEx( DWORD _dwStyle ) { return static_cast<DWORD>(::SetWindowLongW( Wnd(), GWL_EXSTYLE, static_cast<LONG>(_dwStyle) )); }

		// Adds a flag to the current style of the window.
		DWORD								AddStyle( DWORD _dwFlags ) { return SetStyle( GetStyle() | _dwFlags ); }

		// Adds a flag to the current extended style of the window.
		DWORD								AddStyleEx( DWORD _dwFlags ) { return SetStyleEx( GetStyleEx() | _dwFlags ); }

		// Removes a flag from the current style of the window.
		DWORD								RemoveStyle( DWORD _dwFlags ) { return SetStyle( GetStyle() & ~_dwFlags ); }

		// Removes a flag from the current extended style of the window.
		DWORD								RemoveStyleEx( DWORD _dwFlags ) { return SetStyleEx( GetStyleEx() & ~_dwFlags ); }

		// Copies the text of the specified window's title bar (if it has one) into a buffer. If the specified window is a control, the text of the control is copied.
		virtual INT							GetTextA( LPSTR _lpString, INT _nMaxCount ) const { return ::GetWindowTextA( Wnd(), _lpString, _nMaxCount ); }

		// Copies the text of the specified window's title bar (if it has one) into a buffer. If the specified window is a control, the text of the control is copied.
		virtual INT							GetTextW( LPWSTR _lpString, INT _nMaxCount ) const { return ::GetWindowTextW( Wnd(), _lpString, _nMaxCount ); }

		// Gets the window text.
		virtual std::string					GetTextA() const;

		// Gets the window text.
		virtual std::wstring				GetTextW() const;

		// Gets the window text in UTF-8.
		virtual std::string					GetTextUTF8() const;

		// Retrieves the length, in characters, of the specified window's title bar text (if the window has a title bar).
		//	If the specified window is a control, the function retrieves the length of the text within the control
		virtual INT							GetTextLengthA() const { return ::GetWindowTextLengthA( Wnd() ); }

		// Retrieves the length, in characters, of the specified window's title bar text (if the window has a title bar).
		//	If the specified window is a control, the function retrieves the length of the text within the control
		virtual INT							GetTextLengthW() const { return ::GetWindowTextLengthW( Wnd() ); }

		// Changes the text of the specified window's title bar (if it has one). If the specified window is a control, the text of the control is changed.
		virtual BOOL						SetTextA( LPCSTR _lpString )  { return ::SetWindowTextA( Wnd(), _lpString ); }

		// Changes the text of the specified window's title bar (if it has one). If the specified window is a control, the text of the control is changed.
		virtual BOOL						SetTextW( LPCWSTR _lpwString ) { return ::SetWindowTextW( Wnd(), _lpwString ); }

		// Get the value of the text as an expression.
		BOOL								GetTextAsExpression( ee::CExpEvalContainer::EE_RESULT &_eResult, BOOL * _pbExpIsValid = nullptr,
			std::string * _psObjStringResult = nullptr ) const;

		// Get the value of the text as an int64_t expression.
		BOOL								GetTextAsInt64Expression( ee::CExpEvalContainer::EE_RESULT &_eResult ) const;

		// Get the value of the text as a uint64_t expression.
		BOOL								GetTextAsUInt64Expression( ee::CExpEvalContainer::EE_RESULT &_eResult ) const;

		// Get the value of the text as a double expression.
		BOOL								GetTextAsDoubleExpression( ee::CExpEvalContainer::EE_RESULT &_eResult ) const;

		// If the function succeeds, the return value is the pointer to the window that previously had the keyboard focus.
		CWidget *							SetFocus() const;

		// Gets the value of the boolean that tracks the window's focus.
		bool								GetFocus() const;

		// Selects a range of text.  Implemented by CEdit and CComboBox.
		virtual VOID						SetSel( INT /*_iStart*/, INT /*_iEnd*/ ) const {}

		// Are we checked?  Implemented by CCheckButton and CRadioButton.
		virtual BOOL						IsChecked() const { return FALSE; }

		// Changes the check state of a button control.  Implemented by CCheckButton and CRadioButton.
		virtual VOID						CheckButton( UINT /*_uCheck*/ ) {}

		// Sets the button state to BST_CHECKED.
		virtual VOID						Check() { CheckButton( BST_CHECKED ); }

		// Sets the button state to BST_UNCHECKED.
		virtual VOID						UnCheck() { CheckButton( BST_UNCHECKED ); }

		// Sets the button to BST_CHECKED or BST_UNCHECKED.
		virtual VOID						SetCheck( BOOL _bChecked ) { CheckButton( _bChecked ? BST_CHECKED : BST_UNCHECKED ); }

		// Sets the parent window.
		virtual CWidget *					SetParent( CWidget * _pwParent );

		// Window rectangle.
		//virtual const LSW_RECT &			WindowRect() const { return m_rRect; }
		virtual LSW_RECT					WindowRect( const CWidget * /*_pwChild*/ = nullptr ) const { LSW_RECT rTemp; ::GetWindowRect( Wnd(), &rTemp ); return rTemp; }

		// Client rectangle.
		//virtual const LSW_RECT &			ClientRect() const { return m_rClientRect; }
		virtual LSW_RECT					ClientRect( const CWidget * /*_pwChild*/ = nullptr ) const { LSW_RECT rTemp; ::GetClientRect( Wnd(), &rTemp ); return rTemp; }

		// Starting window rectangle.
		virtual const LSW_RECT &			StartRect() const { return m_rStartingRect; }

		// This control's starting window rectangle relative to its parent's starting client rectangle.
		virtual const LSW_RECT &			StartClientRect() const { return m_rStartingClientRect; }

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		// pwChild can be nullptr.  If not nullptr, it is assumed to be a child of the widget, and this widget might create a specific rectangle for
		//	the pwChild, such as for splitter controls.
		virtual const LSW_RECT				VirtualClientRect( const CWidget * pwChild ) const { return ClientRect( pwChild ); }

		// Updates all rectangles with the current window rectangles.  If a control changes size and you wish to set the new size as its "base" size,
		//	call this.
		VOID								UpdateRects();

		// Forces the resizing of this control and all child controls.
		VOID								ForceSizeUpdate() {
			LSW_RECT rRect;
			// Send a fake WM_SIZE message to cause the window to recalculate and redraw its layout.
			::GetClientRect( Wnd(), &rRect );
			::SendMessageW( Wnd(), WM_SIZE, SIZE_RESTORED, MAKELPARAM( rRect.Width(), rRect.Height() ) );
		}

		// Do we have a given child widget?
		bool								HasChild( const CWidget * _pwChild ) const;

		// Gets a pointer to a child with the given ID.
		CWidget *							FindChild( WORD _wId );

		// Gets a pointer to a child with the given ID.
		const CWidget *						FindChild( WORD _wId ) const;

		// Gets a pointer to a parent with the given ID.
		CWidget *							FindParent( WORD _wId );

		// Gets a pointer to a parent with the given ID.
		const CWidget *						FindParent( WORD _wId ) const;

		// Gets the font.
		HFONT								GetFont() const { return reinterpret_cast<HFONT>(::SendMessageW( Wnd(), WM_GETFONT, 0L, 0L )); }

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_WIDGET; }

		// Returns true if this is a CButton class.
		virtual bool						IsButton() const { return false; }

		// Returns true if this is a CCheckButton class.
		virtual bool						IsCheckButton() const { return false; }

		// Returns true if this is a CComboBox class.
		virtual bool						IsComboBox() const { return false; }

		// Returns true if this is a CComboBoxEx class.
		virtual bool						IsComboBoxEx() const { return false; }

		// Returns true if this is a CDockable class.
		virtual bool						IsDockable() const { return false; }

		// Returns true if this is a CEdit class.
		virtual bool						IsEdit() const { return false; }

		// Returns true if this is a CGroupBox class.
		virtual bool						IsGroupBox() const { return false; }

		// Returns true if this is a CListBox class.
		virtual bool						IsListBox() const { return false; }

		// Returns true if this is a CListView class.
		virtual bool						IsListView() const { return false; }

		// Returns true if this is a CMainWindow class.
		virtual bool						IsMainWindow() const { return false; }

		// Returns true if this is a CMultiSplitter class.
		virtual bool						IsMultiSplitter() const { return false; }

		// Returns true if this is a CTreeList class.
		virtual bool						IsTreeList() const { return false; }

		// Returns true if this is a CTreeListView class.
		virtual bool						IsTreeListView() const { return false; }

		// Returns true if this is a CProgressBar class.
		virtual bool						IsProgressBar() const { return false; }

		// Returns true if this is a CRadioButton class.
		virtual bool						IsRadioButton() const { return false; }

		// Returns true if this is a CRebar class.
		virtual bool						IsRebar() const { return false; }

		// Returns true if this is a CSplitter class.
		virtual bool						IsSplitter() const { return false; }

		// Returns true if this is a CStatic class.
		virtual bool						IsStatic() const { return false; }

		// Returns true if this is a CStatusBar class.
		virtual bool						IsStatusBar() const { return false; }

		// Returns true if this is a CTab class.
		virtual bool						IsTab() const { return false; }

		// Returns true if this is a CToolBar class.
		virtual bool						IsToolBar() const { return false; }

		// Set the parent.
		void								SetWidgetParent( CWidget * _pwParent );

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
		/** Tracks whether the control has focus or not. */
		bool								m_bHasFocus;				


		// == Message Handlers.
		// WM_NCCREATE.
		virtual LSW_HANDLED					NcCreate( const CREATESTRUCTW &/*_csCreateParms*/ ) { return LSW_H_CONTINUE; }

		// WM_CREATE.
		virtual LSW_HANDLED					Create( const CREATESTRUCTW &/*_csCreateParms*/ ) { return LSW_H_CONTINUE; }

		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog() { return LSW_H_CONTINUE; };

		// WM_DESTROY.
		virtual LSW_HANDLED					Destroy() { return LSW_H_CONTINUE; }

		// WM_NCDESTROY.
		virtual LSW_HANDLED					NcDestroy() { return LSW_H_CONTINUE; }

		// WM_CLOSE.
		virtual LSW_HANDLED					Close() { return LSW_H_CONTINUE; }

		// WM_CANCELMODE.
		virtual LSW_HANDLED					CancelMode() { return LSW_H_CONTINUE; }

		/**
		 * The WM_SIZE handler.
		 *
		 * \param _wParam The type of resizing requested.
		 * \param _lWidth The new width of the client area.
		 * \param _lHeight The new height of the client area.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED					Size( WPARAM /*_wParam*/, LONG /*_lWidth*/, LONG /*_lHeight*/ );

		/**
		 * The WM_SIZING handler.
		 *
		 * \param _iEdge The edge of the window that is being sized.
		 * \param _prRect A pointer to a RECT structure with the screen coordinates of the drag rectangle. To change the size or position of the drag rectangle, an application must change the members of this structure.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED					Sizing( INT /*_iEdge*/, LSW_RECT * /*_prRect*/ ) { return LSW_H_CONTINUE; }

		// WM_SIZE, SIZE_MINIMIZED.
		virtual LSW_HANDLED					Minimized( LONG /*_lWidth*/, LONG /*_lHeight*/ ) { return LSW_H_CONTINUE; }

		// WM_MOVE.
		virtual LSW_HANDLED					Move( LONG /*_lX*/, LONG /*_lY*/ );

		// WM_WINDOWPOSCHANGED.
		virtual LSW_HANDLED					WindowPosChanged( const WINDOWPOS * /*_pwpPos*/ ) { return LSW_H_CONTINUE; }

		// WM_GETMINMAXINFO.
		virtual LSW_HANDLED					GetMinMaxInfo( MINMAXINFO * /*_pmmiInfo*/ ) { return LSW_H_CONTINUE; }

		/**
		 * Handles the WM_COMMAND message.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator, otherwise it is a Control-defined notification code.
		 * \param _wId The ID of the control if _wCtrlCode is not 0 or 1.
		 * \param _pwSrc The source control if _wCtrlCode is not 0 or 1.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Command( WORD /*_wCtrlCode*/, WORD /*_wId*/, CWidget * /*_pwSrc*/ ) { return LSW_H_CONTINUE; }

		// WM_COMMAND from menu.
		virtual LSW_HANDLED					MenuCommand( WORD /*_Id*/ ) { return LSW_H_CONTINUE; }

		// WM_COMMAND from accelerator.
		virtual LSW_HANDLED					AcceleratorCommand( WORD /*_Id*/ ) { return LSW_H_CONTINUE; }

		// WM_NOTIFY->NM_DBLCLK on this item (if LSW_HANDLED::LSW_H_CONTINUE, message is passed to owning window).
		virtual LSW_HANDLED					DblClk( const NMHDR * /*_phHdr*/ ) { return LSW_H_CONTINUE; }

		// WM_NOTIFY->NM_DBLCLK for the owning window if the child either could not be resolved or returned LSW_HANDLED::LSW_H_CONTINUE.
		virtual LSW_HANDLED					DblClk( const NMHDR * /*_phHdr*/, WORD /*_wControlId*/, CWidget * /*_pwWidget*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_SETFOCUS handler.
		 *
		 * \param _hPrevFocus A handle to the window that previously had focus.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED					SetFocus( HWND /*_hPrevFocus*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_KILLFOCUS handler.
		 *
		 * \param _hNewFocus A handle to the window that receives the keyboard focus. This parameter can be NULL.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED					KillFocus( HWND /*_hNewFocus*/ ) { return LSW_H_CONTINUE; }

		// WM_ERASEBKGND.
		virtual LSW_HANDLED					EraseBkgnd( HDC /*_hDc*/ ) { return LSW_H_CONTINUE; }

		// WM_PAINT.
		virtual LSW_HANDLED					Paint() { return LSW_H_CONTINUE; }

		// WM_NCPAINT.
		virtual LSW_HANDLED					NcPaint( HRGN /*_hRgn*/ ) { return LSW_H_CONTINUE; }

		// WM_CTLCOLOREDIT.
		virtual LSW_HANDLED					CtlColorEdit( HDC /*_hDc*/, CWidget * /*_pwControl*/, HBRUSH &/*_hBrush*/ ) { return LSW_H_CONTINUE; }

		// WM_CTLCOLORLISTBOX
		virtual LSW_HANDLED					CtlColorListBox( HDC /*_hDc*/, CWidget * /*_pwControl*/, HBRUSH &/*_hBrush*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_CTLCOLORSTATIC handler.
		 *
		 * \param _hDc Handle to the device context for the static control window
		 * \param _pwControl Handle to the static control.
		 * \param _hBrush If an application processes this message, the return value is a handle to a brush that the system uses to paint the background of the static control.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED					CtlColorStatic( HDC /*_hDc*/, CWidget * /*_pwControl*/, HBRUSH &/*_hBrush*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_CTLCOLORDLG handler.
		 *
		 * \param _hDc Handle to the device context for the dialog box
		 * \param _pwControl Handle to the dialog box.
		 * \param _hBrush If an application processes this message, the return value is a handle to a brush that the system uses to paint the background of the dialog box.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED					CtlColorDlg( HDC /*_hDc*/, CWidget * /*_pwControl*/, HBRUSH &/*_hBrush*/ ) { return LSW_H_CONTINUE; }

		// WM_SETCURSOR.
		virtual LSW_HANDLED					SetCursor( CWidget * /*_pwControl*/, WORD /*_wHitTest*/, WORD /*_wIdent*/ ) { return LSW_H_CONTINUE; }

		// WM_ACTIVATE.
		virtual LSW_HANDLED					Activate( BOOL /*_bMinimized*/, WORD /*_wActivationMode*/, CWidget * /*_pwWidget*/ ) { return LSW_H_CONTINUE; }

		// WM_NCACTIVATE.
		virtual LSW_HANDLED					NcActivate( BOOL _bTitleBarActive, LPARAM /*_lParam*/ ) { m_bShowAsActive = _bTitleBarActive; return LSW_H_CONTINUE; }

		// WM_ENABLE.
		virtual LSW_HANDLED					Enable( BOOL /*_bEnabled*/ ) { return LSW_H_CONTINUE; }

		// WM_CAPTURECHANGED.
		virtual LSW_HANDLED					CaptureChanged( CWidget * /*_pwNewCaptureOwner*/ ) { return LSW_H_CONTINUE; }

		// WM_HSCROLL
		virtual LSW_HANDLED					HScroll( USHORT /*_uScrollPos*/, USHORT /*_uScrollType*/, HWND /*_hSender*/ ) { return LSW_H_CONTINUE; }

		// WM_VSCROLL
		virtual LSW_HANDLED					VScroll( USHORT /*_uScrollPos*/, USHORT /*_uScrollType*/, HWND /*_hSender*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_INPUT handler.
		 *
		 * \param _iCode The input code. Use GET_RAWINPUT_CODE_WPARAM macro to get the value. Can be one of the following values: RIM_INPUT, RIM_INPUTSINK.
		 * \param _hRawInput A HRAWINPUT handle to the RAWINPUT structure that contains the raw input from the device. To get the raw data, use this handle in the call to GetRawInputData.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Input( INT /*_iCode*/, HRAWINPUT /*_hRawInput*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_INPUT_DEVICE_CHANGE handler.
		 *
		 * \param _iNotifCode This parameter can be one of the following values: GIDC_ARRIVAL, GIDC_REMOVAL.
		 * \param _hDevice The HANDLE to the raw input device.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					InputDeviceChanged( INT /*_iNotifCode*/, HANDLE /*_hDevice*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_KEYDOWN handler.
		 *
		 * \param _uiKeyCode The virtual-key code of the nonsystem key.
		 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					KeyDown( UINT /*_uiKeyCode*/, UINT /*_uiFlags*/ ) { return LSW_H_CONTINUE; }

		// WM_KEYUP
		virtual LSW_HANDLED					KeyUp( UINT /*_uiKeyCode*/, UINT /*_uiFlags*/ ) { return LSW_H_CONTINUE; }

		// WM_SYSKEYDOWN
		virtual LSW_HANDLED					SysKeyDown( UINT /*_uiKeyCode*/, UINT /*_uiFlags*/ ) { return LSW_H_CONTINUE; }

		// WM_SYSKEYUP
		virtual LSW_HANDLED					SysKeyUp( UINT /*_uiKeyCode*/, UINT /*_uiFlags*/ ) { return LSW_H_CONTINUE; }

		// WM_LBUTTONDBLCLK.
		virtual LSW_HANDLED					LButtonDblClk( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_LBUTTONDOWN.
		virtual LSW_HANDLED					LButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_LBUTTONUP.
		virtual LSW_HANDLED					LButtonUp( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_MBUTTONDBLCLK.
		virtual LSW_HANDLED					MButtonDblClk( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_MBUTTONDOWN.
		virtual LSW_HANDLED					MButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_MBUTTONUP.
		virtual LSW_HANDLED					MButtonUp( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_MOUSEACTIVATE.
		virtual LSW_HANDLED					MouseActivate( CWidget * /*_pwTopLevelParent*/, INT /*_iHitTest*/, INT &/*_iReturnCode*/ ) { return LSW_H_CONTINUE; }

		// WM_MOUSEHOVER.
		virtual LSW_HANDLED					MouseHover( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_MOUSEHWHEEL.
		virtual LSW_HANDLED					MouseHWheel( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_MOUSELEAVE.
		virtual LSW_HANDLED					MouseLeave()  { return LSW_H_CONTINUE; }
		
		// WM_MOUSEMOVE.
		virtual LSW_HANDLED					MouseMove( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_MOUSEWHEEL.
		virtual LSW_HANDLED					MouseWheel( DWORD /*_dwVirtKeys*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCHITTEST.
		virtual LSW_HANDLED					NcHitTest( const POINTS &/*_pCursorPos*/, INT &/*_iReturnHitTest*/ ) { return LSW_H_CONTINUE; }

		// WM_NCLBUTTONDBLCLK.
		virtual LSW_HANDLED					NcButtonDblClk( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCLBUTTONDOWN.
		virtual LSW_HANDLED					NcLButtonDown( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCLBUTTONUP.
		virtual LSW_HANDLED					NcLButtonUp( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCMBUTTONDBLCLK.
		virtual LSW_HANDLED					NcMButtonDblClk( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCMBUTTONDOWN.
		virtual LSW_HANDLED					NcMButtonDown( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCMBUTTONUP.
		virtual LSW_HANDLED					NcMButtonUp( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCMOUSEHOVER.
		virtual LSW_HANDLED					NcMouseHover( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCMOUSELEAVE.
		virtual LSW_HANDLED					NcMouseLeave() { return LSW_H_CONTINUE; }

		// WM_NCMOUSEMOVE.
		virtual LSW_HANDLED					NcMouseMove( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCRBUTTONDBLCLK.
		virtual LSW_HANDLED					NcRButtonDblClk( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCRBUTTONDOWN.
		virtual LSW_HANDLED					NcRButtonDown( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCRBUTTONUP.
		virtual LSW_HANDLED					NcRButtonUp( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCXBUTTONDBLCLK.
		virtual LSW_HANDLED					NcXButtonDblClk( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCXBUTTONDOWN.
		virtual LSW_HANDLED					NcXButtonDown( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_NCXBUTTONUP.
		virtual LSW_HANDLED					NcXButtonUp( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_RBUTTONDBLCLK.
		virtual LSW_HANDLED					RButtonDblClk( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_RBUTTONDOWN.
		virtual LSW_HANDLED					RButtonDown( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_RBUTTONUP.
		virtual LSW_HANDLED					RButtonUp( INT /*_iHitTest*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_XBUTTONDBLCLK.
		virtual LSW_HANDLED					XButtonDblClk( DWORD /*_dwVirtKeys*/, DWORD /*_dwButton*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_XBUTTONDOWN.
		virtual LSW_HANDLED					XButtonDown( DWORD /*_dwVirtKeys*/, DWORD /*_dwButton*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// WM_XBUTTONUP.
		virtual LSW_HANDLED					XButtonUp( DWORD /*_dwVirtKeys*/, DWORD /*_dwButton*/, const POINTS &/*_pCursorPos*/ ) { return LSW_H_CONTINUE; }

		// TBN_QUERYINSERT.
		virtual LSW_HANDLED					TbnQueryInsert( const LPNMTOOLBARW /*_lptbToolBar*/ ) { return LSW_H_CONTINUE; }

		// TBN_QUERYDELETE.
		virtual LSW_HANDLED					TbnQueryDelete( const LPNMTOOLBARW /*_lptbToolBar*/ ) { return LSW_H_CONTINUE; }

		// TBN_RESET.
		virtual LSW_HANDLED					TbnReset() { return LSW_H_CONTINUE; }

		// TBN_GETBUTTONINFO.
		virtual LSW_HANDLED					TbnGetButtonInfo( LPNMTOOLBARW /*_lptbToolBar*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_DEVICECHANGE handler.
		 * 
		 * \param _wDbtEvent The event that has occurred.  One of the DBT_* values from the Dbt.h header file.
		 * \param _lParam A pointer to a structure that contains event-specific data. Its format depends on the value of the wParam parameter. For more information, refer to the documentation for each event.
		 * \return Returns an LSW_HANDLED code.  Returning LSW_H_HANDLED is the same as returning BROADCAST_QUERY_DENY.
		 **/
		virtual LSW_HANDLED					DeviceChange( WORD /*_wDbtEvent*/, LPARAM /*_lParam*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_SYSCOMMAND handler.
		 * 
		 * \param _wCommand The type of system command requested.  One of the SC_* values.
		 * \param _iMouseX Specifies the horizontal position of the cursor, in screen coordinates, if a window menu command is chosen with the mouse. Otherwise, this parameter is not used.
		 * \param _iMouseY Specifies the vertical position of the cursor, in screen coordinates, if a window menu command is chosen with the mouse. This parameter is 1 if the command is chosen using a system accelerator, or zero if using a mnemonic.
		 * \return Returns an LSW_HANDLED code.  Return LSW_H_HANDLED to prevent the command from being sent to the default procedure.
		 **/
		virtual LSW_HANDLED					SysCommand( WORD /*_wCommand*/, int /*_iMouseX*/, int /*_iMouseY*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_NOTIFY -> LVN_ITEMCHANGED handler.
		 *
		 * \param _lplvParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Notify_ItemChanged( LPNMLISTVIEW /*_lplvParm*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_NOTIFY -> LVN_ODSTATECHANGED handler.
		 *
		 * \param _lposcParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Notify_OdStateChange( LPNMLVODSTATECHANGE /*_lposcParm*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_NOTIFY -> NM_CUSTOMDRAW -> CDDS_PREPAINT handler.
		 *
		 * \param _lpcdParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual DWORD						Notify_CustomDraw_PrePaint( LPNMLVCUSTOMDRAW /*_lpcdParm*/ ) { return CDRF_DODEFAULT; }

		/**
		 * The WM_NOTIFY -> NM_CUSTOMDRAW -> CDDS_ITEMPREPAINT handler.
		 *
		 * \param _lpcdParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual DWORD						Notify_CustomDraw_ItemPrePaint( LPNMLVCUSTOMDRAW /*_lpcdParm*/ ) { return CDRF_DODEFAULT; }

		/**
		 * The WM_NOTIFY -> NM_CUSTOMDRAW -> (CDDS_ITEMPREPAINT | CDDS_SUBITEM) handler.
		 *
		 * \param _lpcdParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual DWORD						Notify_CustomDraw_ItemPrePaintSubItem( LPNMLVCUSTOMDRAW /*_lpcdParm*/ ) { return CDRF_DODEFAULT; }

		// WM_TIMER.
		virtual LSW_HANDLED					Timer( UINT_PTR /*_uiptrId*/, TIMERPROC /*_tpProc*/ ) { return LSW_H_CONTINUE; }

		// WM_HOTKEY.
		virtual LSW_HANDLED					Hotkey( INT /*_iIdentifier*/, INT /*_iVirtualKey*/, INT /*_iMod*/ ) { return LSW_H_CONTINUE; }

		// WM_CONTEXTMENU.
		virtual LSW_HANDLED					ContextMenu( CWidget * /*_pwControl*/, INT /*_iX*/, INT /*_iY*/ ) { return LSW_H_CONTINUE; }

		/**
		 * The WM_INITMENUPOPUP handler.
		 *
		 * \param _hMenu A handle to the drop-down menu or submenu.
		 * \param _wPos The zero-based relative position of the menu item that opens the drop-down menu or submenu.
		 * \param _bIsWindowMenu Indicates whether the drop-down menu is the window menu. If the menu is the window menu, this parameter is TRUE; otherwise, it is FALSE.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					InitMenuPopup( HMENU /*_hMenu*/, WORD /*_wPos*/, BOOL /*_bIsWindowMenu*/ ) { return LSW_H_CONTINUE; }

		// WM_USER/custom messages.
		virtual LSW_HANDLED					CustomPrivateMsg( UINT /*_uMsg*/, WPARAM /*_wParam*/, LPARAM /*_lParam*/ ) { return LSW_H_CONTINUE; }


		// == Functions.
		// Remove a child.
		virtual void						RemoveChild( const CWidget * _pwChild );

		// Add a child.
		void								AddChild( CWidget * _pwChild );

		// Informs that a child was removed from a child control (IE this control's child had a child control removed from it).
		// Is also called on the control from which a child was removed for convenience.
		virtual void						ChildWasRemoved( const CWidget * _pwChild );

		// Informs tha a child tab closed one of it headers.
		virtual void						ChildTabClosed( CWidget * /*_pwChild*/, INT /*_iTab*/ ) {}

		// Evaluates expressions to determine a new rectangle for the control.
		virtual void						EvalNewSize();

		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

		// Adds a dockable window to the list of dockable windows.
		void								AddDockable( CDockable * _pdDock );

		// Removes a dockable window from the list of dockable windows.
		void								RemDockable( CDockable * _pdDock );

		// Gets the array of dockables, optionally including this object.
		void								GetDockables( std::vector<CWidget *> &_vReturn, BOOL _bIncludeParent );

		// Handle WM_NCACTIVATE for dockables.  Should be called on the owner window.
		//	_pwWnd = Pointer to window that received WM_NCACTIVATE (can be the owner or one of its tool windows).
		//	_wParam = WPARAM of the WM_NCACTIVATE message.
		//	_lParam = LPARAM of the WM_NCACTIVATE message.
		LRESULT CALLBACK					DockNcActivate( CWidget * _pwWnd, WPARAM _wParam, LPARAM _lParam, BOOL _bCallDefault );

		// Handle WM_ENABLE for Should be called on the owner window.
		//	_pwWnd = Pointer to window that received WM_ENABLE (can be the owner or one of its tool windows).
		//	_wParam = WPARAM of the WM_ENABLE message.
		//	_lParam = LPARAM of the WM_ENABLE message.
		LRESULT CALLBACK					DockEnable( CWidget * _pwWnd, WPARAM _wParam, LPARAM _lParam, BOOL _bCallDefault );

		// Attaches a control/window to its CWidget.
		static BOOL CALLBACK				EnumChildWindows_AttachWindowToWidget( HWND _hWnd, LPARAM _lParam );

		// Applies enabled/disabled settings.
		static BOOL CALLBACK				EnumChildWindows_SetEnabled( HWND _hWnd, LPARAM _lParam );

		// Sets all the starting rectangles for all widgets.
		static BOOL CALLBACK				EnumChildWindows_SetStartingRect( HWND _hWnd, LPARAM _lParam );

		// Resizes all controls when the window resizes.
		static BOOL CALLBACK				EnumChildWindows_ResizeControls( HWND _hWnd, LPARAM _lParam );

		// Evaluates "??" inside expressions.  ?? = this pointer.
		static bool __stdcall				WidgetUserVarHandler( uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );

		// Evaluates member access in expressions.
		static bool __stdcall				WidgetMemberAccessHandler( const ee::CExpEvalContainer::EE_RESULT &_rLeft, const std::string &_sMember, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );

		// Handles control setup.
		static VOID							ControlSetup( CWidget * _pwParent, const std::vector<CWidget *> &_vWidgetList );

		// The default message handler.
		static VOID CALLBACK				WndDlgProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, BOOL _bIsDlg, LRESULT &_lrWndResult, INT_PTR &_ipDiagResult );

	};

}	// namespace lsw

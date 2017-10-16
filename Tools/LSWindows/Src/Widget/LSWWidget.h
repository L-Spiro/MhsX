#pragma once

#include "../LSWWin.h"
#include "../Expressions/LSWExpression.h"
#include "../Helpers/LSWHelpers.h"
#include "../Layout/LSWWidgetLayout.h"

#include <vector>

namespace lsw {

	class CWidget {
		friend class						CLayoutManager;
	public :
		CWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );
		~CWidget();


		// == Enumerations.
		enum LSW_HANDLED {
			LSW_H_HANDLED,					// The message was handled and the default procedure should not be called.
			LSW_H_CONTINUE,					// Continue and let the default procedure handle it.
		};


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

		// Custom ID.
		WORD								Id() const { return m_wId; }

		// Enabled or disabled.
		BOOL								Enabled() const { return m_bEnabled; }

		// Enable or disable.
		BOOL								Enable( BOOL _bEnable ) { m_bEnabled = (_bEnable != 0); return ::EnableWindow( Wnd(), m_bEnabled ); }

		// Copies the text of the specified window's title bar (if it has one) into a buffer. If the specified window is a control, the text of the control is copied.
		INT									GetTextA( LPSTR _lpString, INT _nMaxCount ) const { return ::GetWindowTextA( Wnd(), _lpString, _nMaxCount ); }

		// Copies the text of the specified window's title bar (if it has one) into a buffer. If the specified window is a control, the text of the control is copied.
		INT									GetTextW( LPWSTR _lpString, INT _nMaxCount ) const { return ::GetWindowTextW( Wnd(), _lpString, _nMaxCount ); }

		// Retrieves the length, in characters, of the specified window's title bar text (if the window has a title bar).
		//	If the specified window is a control, the function retrieves the length of the text within the control
		INT									GetTextLengthA() const { return ::GetWindowTextLengthA( Wnd() ); }

		// Retrieves the length, in characters, of the specified window's title bar text (if the window has a title bar).
		//	If the specified window is a control, the function retrieves the length of the text within the control
		INT									GetTextLengthW() const { return ::GetWindowTextLengthW( Wnd() ); }

		// Changes the text of the specified window's title bar (if it has one). If the specified window is a control, the text of the control is changed.
		BOOL								SetTextA( LPCSTR _lpString ) { return ::SetWindowTextA( Wnd(), _lpString ); }

		// Changes the text of the specified window's title bar (if it has one). If the specified window is a control, the text of the control is changed.
		BOOL								SetTextW( LPCWSTR _lpwString ) { return ::SetWindowTextW( Wnd(), _lpwString ); }

		// Window rectangle.
		const LSW_RECT &					WindowRect() const { return m_rRect; }

		// Client rectangle.
		const LSW_RECT &					ClientRect() const { return m_rClientRect; }

		// Starting window rectangle.
		const LSW_RECT &					StartRect() const { return m_rStartingRect; }

		// Updates all rectangles with the current window rectangles.  If a control changes size and you wish to set the new size as its "base" size,
		//	call this.
		VOID								UpdateRects();

		// Do we have a given child widget?
		bool								HasChild( const CWidget * _pwChild ) const;

		// Gets a pointer to a child with the given ID.
		CWidget *							FindChild( WORD _wId );

		// Sets a given font on all children of a window.
		static BOOL CALLBACK				EnumChildWindows_SetFont( HWND _hWnd, LPARAM _lParam );

		// Converts a point from pixels to dialog units.
		static POINT						PixelsToDialogUnits( HWND _hWnd, LONG _lX, LONG _lY );


	protected :
		// == Members.
		// The window handle.
		HWND								m_hWnd;

		// Custom ID.
		WORD								m_wId;

		// Enabled.
		BOOL								m_bEnabled;

		// The window rectangle.
		LSW_RECT							m_rRect;

		// The client rectangle.
		LSW_RECT							m_rClientRect;

		// Original rectangle.
		LSW_RECT							m_rStartingRect;

		// Extended styles.
		DWORD								m_dwExtendedStyles;

		// Default state.  Depends on the type of control.
		BOOL								m_bActive;

		// Children.
		std::vector<CWidget *>				m_vChildren;

		// Parent.
		CWidget *							m_pwParent;

		// Width expression.
		CExpression							m_eWidth;

		// Height expression.
		CExpression							m_eHeight;

		// Left expression.
		CExpression							m_eLeft;

		// Right expression.
		CExpression							m_eRight;

		// Top expression.
		CExpression							m_eTop;

		// Bottom expression.
		CExpression							m_eBottom;


		// == Message Handlers.
		// WM_NCCREATE.
		virtual LSW_HANDLED					NcCreate( const CREATESTRUCTW &_csCreateParms );

		// WM_CREATE.
		virtual LSW_HANDLED					Create( const CREATESTRUCTW &_csCreateParms );

		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_DESTROY.
		virtual LSW_HANDLED					Destroy();

		// WM_NCDESTROY.
		virtual LSW_HANDLED					NcDestroy();

		// WM_CLOSE.
		virtual LSW_HANDLED					Close();

		// WM_SIZE.
		virtual LSW_HANDLED					Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );
		// WM_SIZE, SIZE_MINIMIZED.
		virtual LSW_HANDLED					Minimized( LONG _lWidth, LONG _lHeight );

		// WM_MOVE.
		virtual LSW_HANDLED					Move( LONG _lX, LONG _lY );

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _Id, HWND _hControl );

		// WM_COMMAND from menu.
		virtual LSW_HANDLED					MenuCommand( WORD _Id );

		// WM_COMMAND from accelerator.
		virtual LSW_HANDLED					AcceleratorCommand( WORD _Id );

		// WM_NOTIFY->NM_DBLCLK on this item (if LSW_HANDLED::LSW_H_CONTINUE, message is passed to owning window).
		virtual LSW_HANDLED					DblClk( const NMHDR * _phHdr );

		// WM_NOTIFY->NM_DBLCLK for the owning window if the child either could not be resolved or returned LSW_HANDLED::LSW_H_CONTINUE.
		virtual LSW_HANDLED					DblClk( const NMHDR * _phHdr, WORD _wControlId, CWidget * _pwWidget );

		// WM_ERASEBKGND.
		virtual LSW_HANDLED					EraseBkgnd( HDC _hDc );


		// == Functions.
		// Remove a child.
		void								RemoveChild( const CWidget * _pwChild );

		// Add a chld.
		void								AddChild( CWidget * _pwChild );

		// Set the parent.
		void								SetParent( CWidget * _pwParent );

		// Evaluates expressions to determine a new rectangle for the control.
		virtual void						EvalNewSize();

		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

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

	};

}	// namespace lsw

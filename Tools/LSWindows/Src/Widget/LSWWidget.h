#pragma once

#include "../LSWWin.h"
#include "../Expressions/LSWExpression.h"
#include "../Helpers/LSWHelpers.h"
#include "../Layout/LSWWidgetLayout.h"

#include <vector>

namespace lsw {

	class CWidget {
	public :
		CWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true );
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

		// Window rectangle.
		const LSW_RECT &					WindowRect() const { return m_rRect; }

		// Client rectangle.
		const LSW_RECT &					ClientRect() const { return m_rClientRect; }

		// Starting window rectangle.
		const LSW_RECT &					StartRect() const { return m_rStartingRect; }

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


		// == Functions.
		// Remove a child.
		void								RemoveChild( const CWidget * _pwChild );

		// Add a chld.
		void								AddChild( CWidget * _pwChild );

		// Set the parent.
		void								SetParent( CWidget * _pwParent );

		// Evaluates expressions to determine a new rectangle for the control.
		void								EvalNewSize();

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

	};

}	// namespace lsw

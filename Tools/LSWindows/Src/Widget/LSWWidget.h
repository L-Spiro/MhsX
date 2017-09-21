#pragma once

#include "../LSWWin.h"
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
		DWORD								Id() const { return m_dwId; }

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

		// Sets a given font on all children of a window.
		static BOOL CALLBACK				EnumChildWindows_SetFont( HWND _hWnd, LPARAM _lParam );

		// Converts a point from pixels to dialog units.
		static POINT						PixelsToDialogUnits( HWND _hWnd, LONG _lX, LONG _lY );


	protected :
		// == Members.
		// The window handle.
		HWND								m_hWnd;

		// Custom ID.
		DWORD								m_dwId;

		// Enabled.
		BOOL								m_bEnabled;

		// The window rectangle.
		LSW_RECT							m_rRect;

		// The client rectangle.
		LSW_RECT							m_rClientRect;

		// Original rectangle.
		LSW_RECT							m_rStartingRect;

		// Children.
		std::vector<CWidget *>				m_vChildren;

		// Parent.
		CWidget *							m_pwParent;


		// == Message Handlers.
		// WM_NCCREATE.
		virtual LSW_HANDLED					NcCreate( const CREATESTRUCTW &_csCreateParms );

		// WM_CREATE.
		virtual LSW_HANDLED					Create( const CREATESTRUCTW &_csCreateParms );

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


		// == Functions.
		// Remove a child.
		void								RemoveChild( const CWidget * _pwChild );

		// Add a chld.
		void								AddChild( CWidget * _pwChild );

		// Set the parent.
		void								SetParent( CWidget * _pwParent );

		// Attaches a control/window to its CWidget.
		static BOOL CALLBACK				EnumChildWindows_AttachWindowToWidget( HWND _hWnd, LPARAM _lParam );

		// Applies enabled/disabled settings.
		static BOOL CALLBACK				EnumChildWindows_SetEnabled( HWND _hWnd, LPARAM _lParam );

	};

}	// namespace lsw

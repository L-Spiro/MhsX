#pragma once

#include "../LSWWin.h"
#include "../Helpers/LSWHelpers.h"

namespace lsw {

	class CWidget {
	public :
		CWidget();
		~CWidget();


		// == Enumerations.
		enum LSW_HANDLED {
			LSW_H_HANDLED,					// The message was handled and the default procedure should not be called.
			LSW_H_CONTINUE,					// Continue and let the default procedure handle it.
		};


	protected :
		// == Members.
		// The window handle.
		HWND								m_hWnd;

		// The window rectangle.
		LSW_RECT							m_rRect;

		// The client rectangle.
		LSW_RECT							m_rClientRect;


		// == Functions.
		// The default message handler.
		static LRESULT CALLBACK				WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );


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
	};

}	// namespace lsw

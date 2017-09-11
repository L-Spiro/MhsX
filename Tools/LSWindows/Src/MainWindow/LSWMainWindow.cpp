#include "LSWMainWindow.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CMainWindow::CMainWindow( LPCWSTR _pcClass, LPCWSTR _pcTitle, WNDPROC _lpfnWndProc ) {
		/*CWndClassEx wceEx( _lpfnWndProc, _pcClass );
		wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1) );*/
		ATOM aAtom = CBase::GetRegisteredClassAtom( _pcClass );

		m_hWnd = ::CreateWindowW( reinterpret_cast<LPCWSTR>(aAtom),
			_pcTitle,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			64, 64,
			772, 605,
			NULL,
			NULL,
			CBase::GetThisHandle(),
			static_cast<CWidget *>(this) );
		/*
		int idStatus = 90;
		HWND hwndStatus = ::CreateWindowExW(
        0,                       // no extended styles
        STATUSCLASSNAMEW,         // name of status bar class
        (PCTSTR) NULL,           // no text when first created
        SBARS_SIZEGRIP |         // includes a sizing grip
        WS_CHILD | WS_VISIBLE,   // creates a visible child window
        0, 0, 0, 0,              // ignores size and position
        m_hWnd,              // handle to parent window
        NULL,       // child window identifier
        CBase::GetThisHandle(),
        NULL );                   // no window creation data
		*/
	}

	// == Functions.
	// WM_NCDESTROY.
	CWidget::LSW_HANDLED CMainWindow::NcDestroy() {
		::PostQuitMessage( 0 );
		return LSW_H_CONTINUE;
	}
	
}	// namespace lsw

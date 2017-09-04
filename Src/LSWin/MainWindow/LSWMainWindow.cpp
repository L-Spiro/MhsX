#include "LSWMainWindow.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CMainWindow::CMainWindow( LPCWSTR _pcClass, LPCWSTR _pcTitle, WNDPROC _lpfnWndProc ) {
		CWndClassEx wceEx( _lpfnWndProc, _pcClass );
		wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1) );
		ATOM aAtom = CBase::RegisterClassExW( wceEx );

		m_hWnd = ::CreateWindowW( reinterpret_cast<LPCWSTR>(aAtom),
			_pcTitle,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			64, 64,
			772, 605,
			NULL,
			NULL,
			::GetModuleHandleW( NULL ),
			static_cast<CWidget *>(this) );

	}

	// == Functions.
	
}	// namespace lsw

#include "LSWMainWindow.h"
#include "../Base/LSWBase.h"
#include <dbt.h>

namespace lsw {

	CMainWindow::CMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

#if 0
	CMainWindow::CMainWindow( LPCWSTR _pcClass, LPCWSTR _pcTitle /*, WNDPROC _lpfnWndProc*/ ) {
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
		//::EnableWindow( m_hWnd, FALSE );
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
#endif

	// == Functions.
	/**
	 * Registers this window for notifications sensitive to changes in plug-and-play USB devices.
	 * 
	 * \return Returns true if the registry for notification of the changing of PnP devices.
	 **/
	bool CMainWindow::RegisterDeviceInterfaceToHwnd() {
#ifdef LSW_USB_INPUTS
		DEV_BROADCAST_DEVICEINTERFACE_W dbNotifFilter = { .dbcc_size = sizeof( DEV_BROADCAST_DEVICEINTERFACE_W ) };
		dbNotifFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		dbNotifFilter.dbcc_classguid = LSW_HDEVNOTIFY::s_gUsbPnPDevices;
		return m_hDevNotify.RegisterDeviceNot( Wnd(), &dbNotifFilter, DEVICE_NOTIFY_WINDOW_HANDLE ) == TRUE;
#else
		return FALSE;
#endif	// #ifdef LSW_USB_INPUTS
	}
	
}	// namespace lsw

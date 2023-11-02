#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CMainWindow : public CWidget {
	public :
		CMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * Determines the type of control this is.
		 * 
		 * \return Returns LSW_LT_MAINWINDOW.
		 **/
		virtual uint32_t						WidgetType() const { return LSW_LT_MAINWINDOW; }

		/**
		 * Returns true if this is a CMainWindow class.
		 * 
		 * \return Returns true.
		 **/
		virtual bool							IsMainWindow() const { return true; }

		/**
		 * Registers this window for notifications sensitive to changes in plug-and-play USB devices.
		 * 
		 * \return Returns true if the registry for notification of the changing of PnP devices.
		 **/
		virtual bool							RegisterDeviceInterfaceToHwnd();


	protected :
		// == Members.
		/** Ability to set up WM_DEVICECHANGE notifications. */
		LSW_HDEVNOTIFY							m_hDevNotify;


		// == Functions.

	private :
		//typedef CWidget						Parent;
	};
		
}	// namespace lsw

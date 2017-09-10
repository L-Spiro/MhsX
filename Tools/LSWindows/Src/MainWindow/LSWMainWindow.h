#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CMainWindow : public CWidget {
	public :
		CMainWindow( LPCWSTR _pcClass, LPCWSTR _pcTitle, WNDPROC _lpfnWndProc = WindowProc );


	protected :
		// == Functions.
	};
		
}	// namespace lsw

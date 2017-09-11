#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CMainWindow : public CWidget {
	public :
		CMainWindow( LPCWSTR _pcClass, LPCWSTR _pcTitle, WNDPROC _lpfnWndProc = WindowProc );


	protected :
		// == Functions.
		// WM_NCDESTROY.
		virtual LSW_HANDLED					NcDestroy();
	};
		
}	// namespace lsw

#pragma once
#include "MXOptionsPage.h"
#include <ListBox/LSWListBox.h>
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	class COptionsWindow : public lsw::CMainWindow {
	public :
		COptionsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _Id, HWND _hControl );

		// WM_CLOSE.
		virtual LSW_HANDLED					Close();


	protected :
		// == Members.
		// Options pages.
		std::vector<COptionsPage *>			m_vPages;


		// == Functions.
		// Sets the page by index.
		void								SetPage( DWORD _dwIndex );

		// Gets the list box pointer.
		CListBox *							ListBox();
	};

}	// namespace mx

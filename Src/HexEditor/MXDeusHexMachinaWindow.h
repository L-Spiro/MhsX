#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXStringDecoder.h"

#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>
#include <StatusBar/LSWStatusBar.h>
#include <Tab/LSWTab.h>
#include <TreeListView/LSWTreeListView.h>

using namespace lsw;

namespace mx {

	class CDeusHexMachinaWindow : public lsw::CMainWindow {
	public :
		CDeusHexMachinaWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CDeusHexMachinaWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_OPENFILE,

			MX_I_TOTAL,
		};

		// Menus.
		enum MX_MENUS {
			MX_M_CONTEXT_MENU						= 200,
		};


		// == Functions.
		// Prepares to create the window.  Creates the atom if necessary.
		static void									PrepareWindow();


	protected :
		// == Members.
		// The main window class.
		static ATOM									m_aAtom;
	};


}	// namespace mx

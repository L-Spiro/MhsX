#pragma once

#include "../MXMhsX.h"
#include "../Layouts/MXDeuHexMachinaWindowLayout.h"
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

		// Gets the base tab control.
		CTab *										GetTab() const;

		// WM_INITDIALOG.
		virtual LSW_HANDLED							InitDialog();

		// WM_CLOSE.
		virtual LSW_HANDLED							Close();

		// WM_ERASEBKGND.
		virtual LSW_HANDLED							EraseBkgnd( HDC _hDc );

		// Translate a child's tooltip text.
		virtual std::wstring						TranslateTooltip( const std::string &_sText ) { 
			return CStringDecoder::DecodeToWString( _sText.c_str(), _sText.size() );
		}

		// Gets the status bar.
		CStatusBar *								StatusBar();

		// Gets the status bar.
		const CStatusBar *							StatusBar() const;

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const LSW_RECT						VirtualClientRect( const CWidget * pwChild ) const;


	protected :
		// == Members.
		// Image list.
		CImageList									m_iImages;
		// Images.
		CBitmap										m_bBitmaps[MX_I_TOTAL];
		// Image mapping.
		INT											m_iImageMap[MX_I_TOTAL];

		// The main window class.
		static ATOM									m_aAtom;


	private :
		typedef CDeusHexMachinaLayout				Layout;
	};


}	// namespace mx

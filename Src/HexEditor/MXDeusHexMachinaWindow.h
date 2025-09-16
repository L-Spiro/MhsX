#pragma once

#include "../MXMhsX.h"
#include "../Layouts/MXDeuHexMachinaWindowLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>
#include <MultiSplitter/LSWMultiSplitter.h>
#include <StatusBar/LSWStatusBar.h>
#include <Tab/LSWTab.h>
#include <TreeListView/LSWTreeListView.h>

#include <filesystem>

using namespace lsw;

namespace mx {
	
	class CWindowMemHack;

	class CDeusHexMachinaWindow : public lsw::CMainWindow {
	public :
		CDeusHexMachinaWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CDeusHexMachinaWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_NEW,
			MX_I_OPENFILE,
			MX_I_SAVE,
			MX_I_SAVEAS,
			MX_I_SAVEALL,
			MX_I_CLOSE,

			MX_I_CUT,
			MX_I_COPY,
			MX_I_PASTE,
			MX_I_UNDO,
			MX_I_REDO,

			MX_I_FIND,
			MX_I_GOTO,

			MX_I_EXP,
			MX_I_COMPARE,
			MX_I_HISTOGRAM,

			MX_I_TOTAL,
		};

		// Menus.
		enum MX_MENUS {
			MX_M_CONTEXT_MENU						= 200,
		};


		// == Functions.
		// Gets the base tab control.
		CTab *										GetTab() const;

		// WM_INITDIALOG.
		virtual LSW_HANDLED							InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED							Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		// WM_COMMAND from menu.
		virtual LSW_HANDLED							MenuCommand( WORD _wId );

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

		// Gets the multi-splitter.
		CMultiSplitter *							MultiSplitter() {
			return static_cast<CMultiSplitter *>(FindChild( Layout::MX_W_MAIN_SPLITTER ));
		}

		// Gets the main tab.
		CTab *										Tab() {
			return static_cast<CTab *>(FindChild( Layout::MX_W_TABS ));
		}
		
		// Prepares to create the window.  Creates the atom if necessary.
		static void									PrepareWindow();


	protected :
		// == Members.
		// Image list.
		CImageList									m_iImages;
		// Images.
		CBitmap										m_bBitmaps[MX_I_TOTAL];
		// Image mapping.
		INT											m_iImageMap[MX_I_TOTAL];

		// The MHS object where we can access the current process and our options.
		CWindowMemHack *							m_pmhMemHack = nullptr;

		// The main window class.
		static ATOM									m_aAtom;


		// == Functions.
		// Performs a Save As operation.
		void										SaveAs();

		// Performs a Save operation.
		void										Save();

		// Performs an Open operation.
		void										Open();

		// Performs an Open operation.
		void										Open( const std::filesystem::path &_pPath );


	private :
		typedef CDeusHexMachinaLayout				Layout;
	};


}	// namespace mx

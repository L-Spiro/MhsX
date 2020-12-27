#pragma once
#include "../Strings/MXStringDecoder.h"
#include "MXOptionsPage.h"
#include "MXOptions.h"
#include <ListBox/LSWListBox.h>
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	class COptionsWindow : public lsw::CMainWindow {
	public :
		COptionsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Enumerations.
		// Pages.
		enum MX_PAGES : int32_t {
			MX_P_GENERAL,
			MX_P_OPEN_PROCESS,
			MX_P_GENERAL_SEARCH,
			MX_P_SEARCH_EX,
			MX_P_HOTKEYS,
		};


		// == Types.
		// To pass both an options pointer and an index.
		struct MX_PARMS {
			MX_OPTIONS *					poOptions;
			int32_t							i32Page;
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// WM_CLOSE.
		virtual LSW_HANDLED					Close();

		// Translate a child's tooltip text.
		virtual std::wstring				TranslateTooltip( const std::string &_sText ) { 
			return CStringDecoder::DecodeToWString( _sText.c_str(), _sText.size() );
		}

		// Gets the last visible page.
		static int32_t						LastVisiblePage() { return m_i32LastVis; }


	protected :
		// == Members.
		// Options pages.
		std::vector<COptionsPage *>			m_vPages;

		// Options.
		MX_OPTIONS *						m_poOptions;

		// Starting index.
		int32_t								m_i32StartIdx;

		// Last-visible page.
		static int32_t						m_i32LastVis;


		// == Functions.
		// Sets the page by index.
		void								SetPage( DWORD _dwIndex );

		// Gets the list box pointer.
		CListBox *							ListBox();


	private :
		typedef lsw::CMainWindow			Parent;
	};

}	// namespace mx

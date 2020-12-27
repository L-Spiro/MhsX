#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXMemHack.h"
#include "../Options/MXOptions.h"
#include "../Search/MXSearcher.h"
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>
#include <string>

using namespace lsw;

namespace mx {

	class CSearchProgressWindow : public lsw::CMainWindow {
	public :
		CSearchProgressWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


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


	protected :
		// == Types.
		struct MX_SEARCH_THREAD {
			CSearchProgressWindow *			pspwProgWindow;		// This object.
			CWidget *						pwProgressHandle;
			class CMhsMainWindow *			pmmwMainWindow;
			const CSearcher::MX_SEARCH_PARMS *
											pspSearch;
		};


		// == Members.
		// The main window.
		class CMhsMainWindow *				m_pmmwMainWindow;

		// The search parameters.
		const CSearcher::MX_SEARCH_PARMS *	m_pspSearch;

		// The search thread.
		CThread								m_tThread;

		// Search-thread parameters.
		MX_SEARCH_THREAD					m_stParms;

		// The abort trigger.
		bool								m_bAbort;


		// == Functions.
		// The search thread.
		static DWORD WINAPI					SearchThreadProc( LPVOID _lpParameter );
	};

}	// namespace mx

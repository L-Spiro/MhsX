#include "MXSearchProgressWindow.h"
#include "../Layouts/MXSearchProgressLayout.h"
#include "../MainWindow/MXMhsMainWindow.h"

namespace mx {

	CSearchProgressWindow::CSearchProgressWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmmwMainWindow( nullptr ),
		m_pspSearch( nullptr ),
		m_bAbort( false ) {
		if ( _ui64Data ) {
			m_pmmwMainWindow = reinterpret_cast<CSearchProgressLayout::MX_SEARCH_DLG_PARMS *>(_ui64Data)->pmmwWindow;
			m_pspSearch = reinterpret_cast<CSearchProgressLayout::MX_SEARCH_DLG_PARMS *>(_ui64Data)->pspSearch;
		}
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CSearchProgressWindow::InitDialog() {
		CWidget::LSW_HANDLED hRet = CMainWindow::InitDialog();

		if ( m_pspSearch && m_pmmwMainWindow ) {
			const_cast<CSearcher::MX_SEARCH_PARMS *>(m_pspSearch)->pbAbort = &m_bAbort;

			m_stParms.pspwProgWindow = this;
			m_stParms.pwProgressHandle = FindChild( CSearchProgressLayout::MX_SPW_PROGRESSBAR );
			m_stParms.pmmwMainWindow = m_pmmwMainWindow;
			m_stParms.pspSearch = m_pspSearch;
			
			m_tThread.CreateThread( SearchThreadProc, &m_stParms );
		}

		return hRet;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CSearchProgressWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CSearchProgressLayout::MX_SPW_CANCEL : {
				m_bAbort = true;
				break;
			}
		}
		return LSW_H_HANDLED;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CSearchProgressWindow::Close() {
		// Only allow SearchThreadProc() to close this window.
		m_bAbort = true;
		return LSW_H_HANDLED;
	}

	// The search thread.
	DWORD WINAPI CSearchProgressWindow::SearchThreadProc( LPVOID _lpParameter ) {
		MX_SEARCH_THREAD * pstParms = reinterpret_cast<MX_SEARCH_THREAD *>(_lpParameter);
		pstParms->pmmwMainWindow->Search( const_cast<mx::CSearcher::MX_SEARCH_PARMS *>(pstParms->pspSearch), pstParms->pspwProgWindow, false, pstParms->pwProgressHandle );
		::EndDialog( pstParms->pspwProgWindow->Wnd(), 0 );
		return 0;
	}

}	// namespace mx


#include "MXTemplateWindow.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>

namespace mx {

	// == Members.
	// The main window class.
	ATOM CTemplateWindow::m_aAtom = 0;

	CTemplateWindow::CTemplateWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		
	}
	CTemplateWindow::~CTemplateWindow() {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CTemplateWindow::InitDialog() {
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CTemplateWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			/*case CTemplateLayout::MX_PEW_OPENPE : {
				break;
			}*/
		}
		return LSW_H_CONTINUE;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CTemplateWindow::Close() {
		
		//::EndDialog( Wnd(), 0 );
		::DestroyWindow( Wnd() );
		//CUtilities::PrintTotalGuiObjects( GR_GDIOBJECTS );
		return LSW_H_HANDLED;
	}

	// WM_ERASEBKGND.
	CWidget::LSW_HANDLED CTemplateWindow::EraseBkgnd( HDC _hDc ) {
		return LSW_H_HANDLED;
	}

	// Prepares to create the window.  Creates the atom if necessary.
	void CTemplateWindow::PrepareTemplate() {
		if ( !m_aAtom ) {
			lsw::CWndClassEx wceEx;
			wceEx.SetInstance( lsw::CBase::GetThisHandle() );
			WCHAR szStr[9];
			mx::CUtilities::RandomString( szStr, std::size( szStr ) );
			wceEx.SetClassName( szStr );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(CTLCOLOR_DLG + 1) );
			wceEx.SetWindPro( CWidget::WindowProc );
			m_aAtom = lsw::CBase::RegisterClassExW( wceEx.Obj() );
		}
	}

}	// namespace mx

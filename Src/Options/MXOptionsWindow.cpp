#include "MXOptionsWindow.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <ListBox/LSWListBox.h>

namespace mx {

	COptionsWindow::COptionsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, MX_OPTIONS * _poOptions, bool _bCreateWidget, HMENU _hMenu ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu ),
		m_poOptions( _poOptions ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COptionsWindow::InitDialog() {
		CWidget * pwGeneral = COptionsLayout::CreateGeneralPage( this );
		CWidget * pwGenSearch = COptionsLayout::CreateGeneralSearchPage( this );

		// Order of pushing them here determines the order in the list.
		m_vPages.push_back( static_cast<COptionsPage *>(pwGeneral) );
		m_vPages.push_back( static_cast<COptionsPage *>(pwGenSearch) );

		CListBox * plbBox = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_LIST ));
		if ( plbBox ) {
			for ( size_t I = 0; I < m_vPages.size(); ++I ) {
				plbBox->AddString( m_vPages[I]->GetName().c_str() );
			}
		}
		plbBox->SetCurSel( 0 );
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED COptionsWindow::Command( WORD _Id, HWND _hControl ) {
		switch ( _Id ) {
			case COptionsLayout::MX_OI_LIST : {
				//COptionsLayout::CreateOptionsDialog( this );
				CListBox * plbBox = ListBox();
				SetPage( plbBox->GetCurSel() );
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED COptionsWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	// Sets the page by index.
	void COptionsWindow::SetPage( DWORD _dwIndex ) {
		for ( size_t I = 0; I < m_vPages.size(); ++I ) {
			m_vPages[I]->SetVisible( I == _dwIndex );
		}
	}

	// Gets the list box pointer.
	CListBox * COptionsWindow::ListBox() {
		return static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_LIST ));
	}

}	// namespace mx
#include "MXOptionsWindow.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <ListBox/LSWListBox.h>

namespace mx {

	// == Members.
	// Last-visible page.
	int32_t COptionsWindow::m_i32LastVis = COptionsWindow::MX_P_GENERAL;

	// == Functions.
	COptionsWindow::COptionsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		m_poOptions = reinterpret_cast<MX_PARMS *>(_ui64Data)->poOptions;
		m_i32StartIdx = reinterpret_cast<MX_PARMS *>(_ui64Data)->i32Page;
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COptionsWindow::InitDialog() {
		Parent::InitDialog();
		CWidget * pwGeneral = COptionsLayout::CreateGeneralPage( this, m_poOptions );
		CWidget * pwOpenProc = COptionsLayout::CreateOpenProcPage( this, m_poOptions );
		CWidget * pwGenSearch = COptionsLayout::CreateGeneralSearchPage( this, m_poOptions );
		CWidget * pwSearchEx = COptionsLayout::CreateSearchExPage( this, m_poOptions );
		CWidget * pwHotkeys = COptionsLayout::CreateHotkeysPage( this, m_poOptions );


		// Order of pushing them here determines the order in the window/list.
		m_vPages.push_back( static_cast<COptionsPage *>(pwGeneral) );
		m_vPages.push_back( static_cast<COptionsPage *>(pwOpenProc) );
		m_vPages.push_back( static_cast<COptionsPage *>(pwGenSearch) );
		m_vPages.push_back( static_cast<COptionsPage *>(pwSearchEx) );
		m_vPages.push_back( static_cast<COptionsPage *>(pwHotkeys) );

		CListBox * plbBox = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_LIST ));
		if ( plbBox ) {
			for ( size_t I = 0; I < m_vPages.size(); ++I ) {
				if ( m_vPages[I] ) {
					plbBox->AddString( m_vPages[I]->GetName().c_str() );
				}
			}
			plbBox->SetCurSel( m_i32StartIdx );
			SetPage( m_i32StartIdx );
		}


		CWidget * pwDots = FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_DOTS );
		if ( pwDots ) {
			std::string sTemp;
			for ( size_t I = 0; I < 100; ++I ) {
				sTemp.push_back( I % 2 ? '-' : ' ' );
			}
			pwDots->SetTextA( sTemp.c_str() );
		}
		
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED COptionsWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case COptionsLayout::MX_OI_LIST : {
				switch ( _wCtrlCode ) {
					case LBN_SELCHANGE : {
						CListBox * plbBox = ListBox();
						SetPage( plbBox->GetCurSel() );
						break;
					}
				}
				break;
			}
			case COptionsLayout::MX_OI_OK : {
				for ( size_t I = 0; I < m_vPages.size(); ++I ) {
					CSecureWString wsTemp;
					CWidget * pwTemp = nullptr;
					if ( !m_vPages[I]->Verify( wsTemp, pwTemp ) ) {
						CListBox * plbBox = ListBox();
						plbBox->SetCurSel( I );
						SetPage( I );
						pwTemp->SetFocus();
						pwTemp->SetSel( 0, -1 );
						lsw::CBase::MessageBoxError( Wnd(), wsTemp.c_str() );
						return LSW_H_CONTINUE;
					}
				}
				for ( size_t I = 0; I < m_vPages.size(); ++I ) {
					if ( m_vPages[I]->Visible() ) {
						m_i32LastVis = static_cast<int32_t>(I);
					}
					m_vPages[I]->Finalize();
				}
				::EndDialog( Wnd(), 1 );
				return LSW_H_HANDLED;
			}
			case COptionsLayout::MX_OI_CANCEL : {
				return Close();
			}
		}
		return Parent::Command( _wCtrlCode, _Id, _pwSrc );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED COptionsWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	// Sets the page by index.
	void COptionsWindow::SetPage( DWORD _dwIndex ) {
		for ( size_t I = 0; I < m_vPages.size(); ++I ) {
			if ( m_vPages[I] ) {
				m_vPages[I]->SetVisible( I == _dwIndex );
			}
		}
	}

	// Gets the list box pointer.
	CListBox * COptionsWindow::ListBox() {
		return static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_LIST ));
	}

}	// namespace mx

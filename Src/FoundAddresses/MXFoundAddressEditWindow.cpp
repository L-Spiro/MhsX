#include "MXFoundAddressEditWindow.h"
#include "../Layouts/MXFoundAddressEditLayout.h"
#include "../MemHack/MXMemHack.h"

#include <Button/LSWColorButton.h>
#include <Tab/LSWTab.h>


namespace mx {

	CFoundAddressEditWindow::CFoundAddressEditWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pParms( (*reinterpret_cast<CFoundAddressEditPageBase::MX_PARMS *>(_ui64Data)) ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CFoundAddressEditWindow::InitDialog() {
		try {
			auto ptTab = reinterpret_cast<lsw::CTab *>(FindChild( CFoundAddressEditLayout::MX_FAEI_TAB ));
			if ( ptTab ) {
				ptTab->SetShowCloseBoxes( false );

				auto pfaempMain = reinterpret_cast<CFoundAddressEditMainPage *>(CFoundAddressEditLayout::CreateGeneralPage( this, m_pParms.pmhMemHack, m_pParms.vSelection ));
				if ( pfaempMain ) {
					m_vPages.push_back( pfaempMain );
				}


				lsw::CHelpers::FillTabAndFitWindow( reinterpret_cast<CTabPageBase **>(m_vPages.data()), m_vPages.size(), ptTab, this, FALSE );
			}
		}
		catch ( ... ) {}

		for ( auto I = m_vPages.size(); I--; ) {
			m_vPages[I]->Update();
		}

		return CParent::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CFoundAddressEditWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		switch ( _wId ) {
			case CFoundAddressEditLayout::MX_FAEI_OK : {
				for ( size_t I = 0; I < m_vPages.size(); ++I ) {
					CSecureWString wsTemp;
					CWidget * pwTemp = nullptr;
					if ( !m_vPages[I]->Verify( wsTemp, pwTemp ) ) {
						auto ptTab = reinterpret_cast<lsw::CTab *>(FindChild( CFoundAddressEditLayout::MX_FAEI_TAB ));
						if ( ptTab ) {
							ptTab->SetCurSel( I );
						}
						pwTemp->SetFocus();
						pwTemp->SetSel( 0, -1 );
						lsw::CBase::MessageBoxError( Wnd(), wsTemp.c_str() );
						return LSW_H_CONTINUE;
					}
				}
				for ( size_t I = 0; I < m_vPages.size(); ++I ) {
					/*if ( m_vPages[I]->Visible() ) {
						m_i32LastVis = static_cast<int32_t>(I);
					}*/
					m_vPages[I]->Finalize();
				}
				::EndDialog( Wnd(), 1 );
				return LSW_H_HANDLED;
			}
			case CFoundAddressEditLayout::MX_FAEI_CANCEL : {
				return Close();
			}
		}
		
		return CParent::Command( _wCtrlCode, _wId, _pwSrc );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CFoundAddressEditWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

}	// namespace mx

#include "MXMoveAddressesWindow.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../MemHack/MXWindowMemHack.h"

#include <EEExpEval.h>
#include <Tab/LSWTab.h>


namespace mx {

	CMoveAddressWindow::CMoveAddressWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmhMemHack( reinterpret_cast<CWindowMemHack *>(_ui64Data) ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CMoveAddressWindow::InitDialog() {
		try {
			auto pwTmp = FindChild( Layout::MX_MAI_ADDR_START_COMBO );
			if ( pwTmp ) {
				CUtilities::FillComboWithStrings( pwTmp, m_pmhMemHack->Options().vMoveAddrSourceHistory, -1, -1 );
				if ( m_pmhMemHack->Window()->MoveFromAddr() ) {
					CSecureWString swsTmp;
					pwTmp->SetTextW( CUtilities::PrintAddress( m_pmhMemHack->Window()->MoveFromAddr(), swsTmp ) );
				}
			}

			pwTmp = FindChild( Layout::MX_MAI_MOVE_BY_COMBO );
			if ( pwTmp ) {
				CUtilities::FillComboWithStrings( pwTmp, m_pmhMemHack->Options().vMoveAddrByHistory, -1, -1 );
				if ( m_pmhMemHack->Options().vMoveAddrByHistory.size() ) {
					pwTmp->SetTextW( m_pmhMemHack->Options().vMoveAddrByHistory[0].c_str() );
				}
			}

			pwTmp = FindChild( Layout::MX_MAI_ADDR_END_COMBO );
			if ( pwTmp ) {
				CUtilities::FillComboWithStrings( pwTmp, m_pmhMemHack->Options().vMoveAddrTargetHistory, -1, -1 );
				if ( m_pmhMemHack->Options().vMoveAddrTargetHistory.size() ) {
					pwTmp->SetTextW( m_pmhMemHack->Options().vMoveAddrTargetHistory[0].c_str() );
				}
				pwTmp->SetFocus();
				pwTmp->SetSel( 0, -1 );
			}

			

			FromAddressUpdate();
		}
		catch ( ... ) {}

		return CParent::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CMoveAddressWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		switch ( _wId ) {
			case Layout::MX_MAI_OK : {
				{
					bool bSuccess;
					int64_t i64Offset = AdjustAmount( bSuccess );
					if ( !bSuccess ) { break; }
					if ( m_pmhMemHack->Window() ) {
						auto pwTree = m_pmhMemHack->Window()->MainTreeView();
						if ( pwTree ) {
							auto famMan = m_pmhMemHack->FoundAddressManager();
							std::vector<LPARAM> vSelected;
							if ( pwTree->GatherSelectedLParam( vSelected, true ) >= 1 ) {
								pwTree->BeginLargeUpdate();
								for ( auto I = vSelected.size(); I--; ) {
									auto pfabThis = famMan->GetById( size_t( vSelected[I] ) );
									if ( pfabThis && pfabThis->Type() == MX_FAT_FOUND_ADDRESS ) {
										reinterpret_cast<CFoundAddress *>(pfabThis)->AdjustAddress( i64Offset );
									}
								}
								pwTree->FinishUpdate();
							}
						}
					}
				}
				{
					MX_OPTIONS oOption = m_pmhMemHack->Options();
					auto pwTmp = FindChild( Layout::MX_MAI_ADDR_START_COMBO );
					if ( pwTmp ) {
						CUtilities::AddOrMove<CSecureWString>( oOption.vMoveAddrSourceHistory, pwTmp->GetTextW() );
					}
					pwTmp = FindChild( Layout::MX_MAI_ADDR_END_COMBO );
					if ( pwTmp ) {
						CUtilities::AddOrMove<CSecureWString>( oOption.vMoveAddrTargetHistory, pwTmp->GetTextW() );
					}
					pwTmp = FindChild( Layout::MX_MAI_MOVE_BY_COMBO );
					if ( pwTmp ) {
						CUtilities::AddOrMove<CSecureWString>( oOption.vMoveAddrByHistory, pwTmp->GetTextW() );
					}
					m_pmhMemHack->SetOptions( oOption );
				}
				::EndDialog( Wnd(), 1 );
				return LSW_H_HANDLED;
			}
			case Layout::MX_MAI_CANCEL : {
				return Close();
			}

			case Layout::MX_MAI_ADDR_START_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					FromAddressUpdate();
					Update();
					return LSW_H_HANDLED;
				}
				break;
			}
			case Layout::MX_MAI_ADDR_END_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					ToAddressUpdate();
					Update();
					return LSW_H_HANDLED;
				}
				break;
			}
			case Layout::MX_MAI_MOVE_BY_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					ByUpdate();
					Update();
					return LSW_H_HANDLED;
				}
				break;
			}
		}
		Update();
		return CParent::Command( _wCtrlCode, _wId, _pwSrc );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CMoveAddressWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	// Updates the dialog based on inputs from the From Address.
	void CMoveAddressWindow::FromAddressUpdate() {
		ee::CExpEvalContainer::EE_RESULT rPoint, rTo;
		auto pwTmp = FindChild( Layout::MX_MAI_ADDR_START_COMBO );
		if ( !pwTmp ) { return; }
		if ( !pwTmp->GetTextAsUInt64Expression( rPoint ) ) { return; }


		CWidget * pwTo = FindChild( Layout::MX_MAI_ADDR_END_COMBO );
		if ( pwTo && pwTo->GetTextAsUInt64Expression( rTo ) ) {
			pwTmp = FindChild( Layout::MX_MAI_MOVE_BY_COMBO );
			if ( pwTmp ) {
				int64_t i64MoveBy = int64_t( rTo.u.ui64Val - rPoint.u.ui64Val );
				pwTmp->SetTextW( std::to_wstring( i64MoveBy ).c_str() );
			}
		}
	}

	// Updates the dialog based on inputs from the To Address.
	void CMoveAddressWindow::ToAddressUpdate() {
		FromAddressUpdate();
	}

	// Updates the dialog based on inputs from the By Amount.
	void CMoveAddressWindow::ByUpdate() {
		ee::CExpEvalContainer::EE_RESULT rPoint, rBy;
		auto pwTmp = FindChild( Layout::MX_MAI_ADDR_START_COMBO );
		if ( !pwTmp ) { return; }
		if ( !pwTmp->GetTextAsUInt64Expression( rPoint ) ) { return; }


		CWidget * pwTo = FindChild( Layout::MX_MAI_MOVE_BY_COMBO );
		if ( pwTo && pwTo->GetTextAsUInt64Expression( rBy ) ) {
			pwTmp = FindChild( Layout::MX_MAI_ADDR_END_COMBO );
			if ( pwTmp ) {
				CSecureWString swsTmp;
				pwTmp->SetTextW( CUtilities::PrintAddress( rBy.u.ui64Val + rPoint.u.ui64Val, swsTmp ) );
			}
		}
	}

	// Updates the UI state.
	void CMoveAddressWindow::Update() {
		auto pwTmp = FindChild( Layout::MX_MAI_OK );
		if ( pwTmp ) {
			bool bSuccess;
			AdjustAmount( bSuccess );
			pwTmp->SetEnabled( bSuccess );
		}
	}

	// Gets the adjustment amount.
	int64_t CMoveAddressWindow::AdjustAmount( bool &_bSuccess ) {
		_bSuccess = false;
		ee::CExpEvalContainer::EE_RESULT rPoint;
		auto pwTmp = FindChild( Layout::MX_MAI_MOVE_BY_COMBO );
		if ( !pwTmp ) { return 0; }
		if ( !pwTmp->GetTextAsInt64Expression( rPoint ) ) { return 0; }

		_bSuccess = true;
		return rPoint.u.i64Val;
	}

}	// namespace mx

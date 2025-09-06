#include "MXAddFoundAddressWindow.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../MemHack/MXWindowMemHack.h"

#include <EEExpEval.h>
#include <Tab/LSWTab.h>


namespace mx {

	CAddFoundAddressWindow::CAddFoundAddressWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmhMemHack( reinterpret_cast<CMemHack *>(_ui64Data) ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CAddFoundAddressWindow::InitDialog() {
		try {
			auto pwTmp = FindChild( Layout::MX_ADI_TYPE_COMBO );
			if ( pwTmp ) {
				LPARAM lpParm = m_pmhMemHack->Options().lpAddValLastType == LPARAM( -1 ) ? m_pmhMemHack->Searcher().LastSearchParms().dtLVal.dtType : m_pmhMemHack->Options().lpAddValLastType;
				if ( m_pmhMemHack->Searcher().LastSearchParms().stType == CUtilities::MX_ST_STRING_SEARCH ) {
					lpParm = CUtilities::MX_DT_STRING;
				}
				CUtilities::FillComboWithEditTypes( pwTmp, lpParm );
			}


			pwTmp = FindChild( Layout::MX_ADI_DESC_COMBO );
			if ( pwTmp ) {
				CUtilities::FillComboWithStrings( pwTmp, m_pmhMemHack->Options().vAddValDescriptionHistory, 0 );
			}
			pwTmp = FindChild( Layout::MX_ADI_ADDRESS_COMBO );
			if ( pwTmp ) {
				CUtilities::FillComboWithStrings( pwTmp, m_pmhMemHack->Options().vAddValAddrHistory, 0 );
			}
			pwTmp = FindChild( Layout::MX_ADI_CODEPAGE_COMBO );
			if ( pwTmp ) {
				CUtilities::FillComboBoxWithCodePages( pwTmp, m_pmhMemHack->Options().lpAddValLastCodePage );
			}

			Update();
		}
		catch ( ... ) {}

		return CParent::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CAddFoundAddressWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		switch ( _wId ) {
			case Layout::MX_ADI_OK : {
				{
					CSecureWString swsStr;
					auto pwTmp = FindChild( Layout::MX_ADI_DESC_COMBO );
					if ( pwTmp ) {
						swsStr = pwTmp->GetTextW();
					}
					uint64_t ui64Address;
					pwTmp = FindChild( Layout::MX_ADI_ADDRESS_COMBO );
					if ( pwTmp ) {
						ee::CExpEvalContainer::EE_RESULT rRes;
						if ( !pwTmp->GetTextAsUInt64Expression( rRes ) ) {
							lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_55A026BA_Invalid_Address.c_str() );
							return LSW_H_HANDLED;
						}
						ui64Address = rRes.u.ui64Val;
					}

					CUtilities::MX_DATA_TYPES dtType = CUtilities::MX_DT_UINT32;
					pwTmp = FindChild( Layout::MX_ADI_TYPE_COMBO );
					if ( pwTmp ) {
						dtType = static_cast<CUtilities::MX_DATA_TYPES>(pwTmp->GetCurSelItemData());
					}
					try {
						auto famMan = m_pmhMemHack->FoundAddressManager();
						auto faAddress = famMan->AddFoundAddress( m_pmhMemHack );
						if ( faAddress ) {
							if ( dtType == CUtilities::MX_DT_STRING ) {
								UINT uiCodePage = CCodePages::GetSystemDefaultAnsiCodePage();
								pwTmp = FindChild( Layout::MX_ADI_CODEPAGE_COMBO );
								if ( pwTmp ) {
									uiCodePage = static_cast<UINT>(pwTmp->GetCurSelItemData());
									if MX_UNLIKELY( uiCodePage == static_cast<UINT>(-1) ) {
										uiCodePage = CCodePages::GetSystemDefaultAnsiCodePage();
									}
								}
							}
							else {
								size_t sOffset = 0;
								std::vector<uint8_t> vData( CUtilities::DataTypeSize( dtType ) );
								m_pmhMemHack->ReadProcessMemory_PreProcessed( ui64Address, vData, CUtilities::DataTypeSize( dtType ), sOffset );
								if ( !faAddress->InitWithAddressAndDataType( ui64Address, dtType, vData.data() ) ) {
									famMan->Delete( faAddress->Id() );
									lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_55A026BA_Invalid_Address.c_str() );
									return LSW_H_HANDLED;
								}
								faAddress->SetPreProcessing( m_pmhMemHack->Searcher().LastSearchParms().bsByteSwapping );
								
								auto ptlvTree = static_cast<CWindowMemHack *>(m_pmhMemHack)->Window()->MainTreeView();
								if ( ptlvTree ) {
									CUtilities::AddFoundAddressToTreeListView( ptlvTree, faAddress, &swsStr );
								}
							}
						}
						else {
							lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_F39F91A5_Out_of_memory_.c_str() );
							return LSW_H_HANDLED;
						}
					}
					catch ( ... ) {

					}
				}
				{
					MX_OPTIONS oOption = m_pmhMemHack->Options();
					auto pwTmp = FindChild( Layout::MX_ADI_DESC_COMBO );
					if ( pwTmp ) {
						CUtilities::AddOrMove<CSecureWString>( oOption.vAddValDescriptionHistory, pwTmp->GetTextW() );
					}
					pwTmp = FindChild( Layout::MX_ADI_ADDRESS_COMBO );
					if ( pwTmp ) {
						CUtilities::AddOrMove<CSecureWString>( oOption.vAddValAddrHistory, pwTmp->GetTextW() );
					}
					pwTmp = FindChild( Layout::MX_ADI_TYPE_COMBO );
					if ( pwTmp ) {
						oOption.lpAddValLastType = pwTmp->GetCurSelItemData();
					}
					pwTmp = FindChild( Layout::MX_ADI_CODEPAGE_COMBO );
					if ( pwTmp ) {
						oOption.lpAddValLastCodePage = pwTmp->GetCurSelItemData();
					}
					m_pmhMemHack->SetOptions( oOption );
				}
				::EndDialog( Wnd(), 1 );
				return LSW_H_HANDLED;
			}
			case Layout::MX_ADI_CANCEL : {
				return Close();
			}
			return LSW_H_CONTINUE;
		}
		Update();
		return CParent::Command( _wCtrlCode, _wId, _pwSrc );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CAddFoundAddressWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	// Updates the dialog after changing some parameters.
	void CAddFoundAddressWindow::Update() {
		CUtilities::MX_DATA_TYPES dtType = CUtilities::MX_DT_UINT32;
		auto pwTmp = FindChild( Layout::MX_ADI_TYPE_COMBO );
		if ( pwTmp ) {
			dtType = static_cast<CUtilities::MX_DATA_TYPES>(pwTmp->GetCurSelItemData());
		}


		pwTmp = FindChild( Layout::MX_ADI_CODEPAGE_LABEL );
		if ( pwTmp ) {
			pwTmp->SetEnabled( dtType == CUtilities::MX_DT_STRING );
		}
		pwTmp = FindChild( Layout::MX_ADI_CODEPAGE_COMBO );
		if ( pwTmp ) {
			pwTmp->SetEnabled( dtType == CUtilities::MX_DT_STRING );
		}
	}

}	// namespace mx

#include "MXFoundAddressEditMainPage.h"
#include "../FoundAddresses/MXFoundAddressManager.h"
#include "../Layouts/MXFoundAddressEditLayout.h"
#include "../MemHack/MXMemHack.h"
#include "../Strings/MXStringDecoder.h"


namespace mx {

	CFoundAddressEditMainPage::CFoundAddressEditMainPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CFoundAddressEditPageBase( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data )/*,
		m_poOptions( reinterpret_cast<MX_OPTIONS *>(_ui64Data) )*/ {
	}

	// == Functions.
	// Get the name of the page.
	std::wstring CFoundAddressEditMainPage::GetName() const {
		return _DEC_WS_01940FD6_General;
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CFoundAddressEditMainPage::InitDialog() {
		auto pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_GENERAL_DESC_COMBO );
		if ( pwTmp ) {
			auto wsText = GatherDescriptions();
			pwTmp->SetTextW( wsText.c_str() );
		}
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_VALUE_CUR_VAL_COMBO );
		if ( pwTmp ) {
			auto wsText = GatherCurValue();
			pwTmp->SetTextW( wsText.c_str() );
		}
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CFoundAddressEditMainPage::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		return LSW_H_CONTINUE;
	}

	// Verifies the options, returning an error string in case of error.
	BOOL CFoundAddressEditMainPage::Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
		return FALSE;
	}

	// Copies all the settings to the MX_OPTIONS structure.
	BOOL CFoundAddressEditMainPage::Finalize() {
		return FALSE;
	}

	// Updates the contents of page based on settings etc.
	void CFoundAddressEditMainPage::Update() {
		auto pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_GENERAL_ARRAY_CHECK );
		if ( pwTmp ) {
			pwTmp->SetEnabled( m_pParms.vSelection.size() >= 2 );
		}
	}

	// Gathers all descriptions and returns the Description string.
	CSecureWString CFoundAddressEditMainPage::GatherDescriptions() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CSecureWString wswRet;
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		auto faTmp = famManager->GetById( m_pParms.vSelection[0] );
		if ( !faTmp ) { return CSecureWString(); }
		wswRet = faTmp->DescriptionText();

		for ( size_t I = 1; I < m_pParms.vSelection.size(); ++I ) {
			faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp ) {
				if ( faTmp->DescriptionText() != wswRet ) { return CSecureWString(); }
			}
		}

		return wswRet;
	}

	// Gathers all current value strings.
	CSecureWString CFoundAddressEditMainPage::GatherCurValue() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CSecureWString wswRet;
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		auto faTmp = famManager->GetById( m_pParms.vSelection[0] );
		if ( !faTmp ) { return CSecureWString(); }
		wswRet = faTmp->ValueText();

		for ( size_t I = 1; I < m_pParms.vSelection.size(); ++I ) {
			faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp ) {
				if ( faTmp->ValueText() != wswRet ) { return CSecureWString(); }
			}
		}

		return wswRet;
	}

}	// namespace mx

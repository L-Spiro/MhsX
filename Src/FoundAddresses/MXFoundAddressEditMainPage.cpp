#include "MXFoundAddressEditMainPage.h"
#include "../FoundAddresses/MXFoundAddressBase.h"
#include "../FoundAddresses/MXFoundAddressManager.h"
#include "../Layouts/MXFoundAddressEditLayout.h"
#include "../MemHack/MXMemHack.h"
#include "../Strings/MXStringDecoder.h"

#include <CheckButton/LSWCheckButton.h>

#include <EEExpEvalContainer.h>


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
		auto pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboWithEditTypes( pwTmp, GatherDataTypes() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_QL_LOCK_TYPE_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboBoxWithLocktypes( pwTmp, GatherLockTypes() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_STRING_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboBoxWithStringTypes( pwTmp, CUtilities::MX_LT_EXACT );
		}
		pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_DATA_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboBoxWithStdDataTypes( pwTmp, GatherDataTypes() );
		}
		/*pwTmp = FindChild( Layout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboBoxWithStdDataTypes( pwTmp, GatherDataTypes() );
		}*/


		pwTmp = FindChild( Layout::MX_FAEI_GENERAL_DESC_COMBO );
		if ( pwTmp ) {
			auto wsText = GatherDescriptions();
			pwTmp->SetTextW( wsText.c_str() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
		if ( pwTmp ) {
			auto wsText = GatherCurValue();
			pwTmp->SetTextW( wsText.c_str() );
		}

		pwTmp = FindChild( Layout::MX_FAEI_VALUE_ARRAY_LEN_COMBO );
		if ( pwTmp ) {
			auto wsText = GatherArrayLens();
			pwTmp->SetTextW( wsText.c_str() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO );
		if ( pwTmp ) {
			auto wsText = GatherArrayStrides();
			pwTmp->SetTextW( wsText.c_str() );
		}

		pwTmp = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
		if ( pwTmp ) {
			auto wsText = GatherLockedValue();
			pwTmp->SetTextW( wsText.c_str() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_INFO_ADDRESS_EDIT );
		if ( pwTmp ) {
			auto wsText = GatherAddress();
			pwTmp->SetTextW( wsText.c_str() );
		}
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CFoundAddressEditMainPage::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {

		Update();
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
		// Enabled.
		int iType = -1;
		auto pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_COMBO );
		if ( pwTmp ) {
			iType = pwTmp->GetCurSelItemData();
		}
		int iValWhenLocked = -1;
		pwTmp = FindChild( Layout::MX_FAEI_QL_LOCK_TYPE_COMBO );
		if ( pwTmp ) {
			iValWhenLocked = pwTmp->GetCurSelItemData();
		}

		pwTmp = FindChild( Layout::MX_FAEI_VALUE_ARRAY_LEN_COMBO );
		ee::CExpEvalContainer::EE_RESULT rRes;
		if ( pwTmp ) {
			rRes.u.i64Val = -1;
			pwTmp->GetTextAsInt64Expression( rRes );
		}

		bool bIsMulti = m_pParms.vSelection.size() >= 2;

		
		{
			struct {
				Layout::MX_FOUND_ADDRESS_EDIT_IDS						wId;
				bool													bCondition0;
			} cControls[] = {
				{ Layout::MX_FAEI_GENERAL_ARRAY_CHECK,					bIsMulti },
				{ Layout::MX_FAEI_VALUE_TYPE_DATA_COMBO,				iType == LPARAM( CUtilities::MX_DT_VOID ) || iType == -1 },
				{ Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_LABEL,		iValWhenLocked == LPARAM( CUtilities::MX_LT_RANGE ) },
				{ Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO,		iValWhenLocked == LPARAM( CUtilities::MX_LT_RANGE ) },

				{ Layout::MX_FAEI_VALUE_ARRAY_LEN_LABEL,				iType != LPARAM( CUtilities::MX_DT_STRING ) },
				{ Layout::MX_FAEI_VALUE_ARRAY_LEN_COMBO,				iType != LPARAM( CUtilities::MX_DT_STRING ) },

				{ Layout::MX_FAEI_VALUE_ARRAY_STRIDE_LABEL,				rRes.u.i64Val >= 2 },
				{ Layout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO,				rRes.u.i64Val >= 2 },
			};
			for ( auto I = MX_ELEMENTS( cControls ); I--; ) {
				auto pwThis = FindChild( cControls[I].wId );
				if ( pwThis ) {
					pwThis->SetEnabled( cControls[I].bCondition0 );
				}
			}
		}

		{
			struct {
				Layout::MX_FOUND_ADDRESS_EDIT_IDS						wId;
				bool													bCondition0;
			} cControls[] = {
				{ Layout::MX_FAEI_VALUE_TYPE_STRING_COMBO,				iType == LPARAM( CUtilities::MX_DT_STRING ) },
				{ Layout::MX_FAEI_VALUE_TYPE_DATA_COMBO,				iType != LPARAM( CUtilities::MX_DT_STRING ) },
				{ Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_LABEL,		iValWhenLocked != LPARAM( CUtilities::MX_LT_RANGE ) },
				{ Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_MIN_LABEL,		iValWhenLocked == LPARAM( CUtilities::MX_LT_RANGE ) },
			};
			for ( auto I = MX_ELEMENTS( cControls ); I--; ) {
				auto pwThis = FindChild( cControls[I].wId );
				if ( pwThis ) {
					pwThis->SetVisible( cControls[I].bCondition0 );
				}
			}
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
		bool bRead;
		wswRet = faTmp->ValueText( &bRead );
		if ( !bRead ) { return CSecureWString(); }
		for ( size_t I = 1; I < m_pParms.vSelection.size(); ++I ) {
			faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp ) {
				if ( faTmp->ValueText( &bRead ) != wswRet ) { return CSecureWString(); }
				if ( !bRead ) { return CSecureWString(); }
			}
		}

		return wswRet;
	}

	// Gathers all locked-value strings.
	CSecureWString CFoundAddressEditMainPage::GatherLockedValue() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CSecureWString wswRet;
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		auto faTmp = famManager->GetById( m_pParms.vSelection[0] );
		if ( !faTmp ) { return CSecureWString(); }
		wswRet = faTmp->ValueWhenLockedText();

		for ( size_t I = 1; I < m_pParms.vSelection.size(); ++I ) {
			faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp ) {
				if ( faTmp->ValueWhenLockedText() != wswRet ) { return CSecureWString(); }
			}
		}

		return wswRet;
	}

	// Gathers all address strings.
	CSecureWString CFoundAddressEditMainPage::GatherAddress() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CSecureWString wswRet;
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		auto faTmp = famManager->GetById( m_pParms.vSelection[0] );
		if ( !faTmp ) { return CSecureWString(); }
		wswRet = faTmp->AddressText();

		for ( size_t I = 1; I < m_pParms.vSelection.size(); ++I ) {
			faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp ) {
				if ( faTmp->AddressText() != wswRet ) { return CSecureWString(); }
			}
		}

		return wswRet;
	}

	// Gathers all data types.
	CUtilities::MX_DATA_TYPES CFoundAddressEditMainPage::GatherDataTypes() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CUtilities::MX_DATA_TYPES( -1 ); }
		CUtilities::MX_DATA_TYPES dtRet = CUtilities::MX_DATA_TYPES( -1 );
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		bool bFound = false;

		for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
			auto faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp && faTmp->Type() == MX_FAT_FOUND_ADDRESS ) {
				if ( !bFound ) {
					dtRet = reinterpret_cast<CFoundAddress *>(faTmp)->DataType();
					bFound = true;
					continue;
				}
				if ( reinterpret_cast<CFoundAddress *>(faTmp)->DataType() != dtRet ) { return CUtilities::MX_DATA_TYPES( -1 ); }
			}
		}

		return dtRet;
	}

	// Gathers all lock types.
	CUtilities::MX_LOCK_TYPES CFoundAddressEditMainPage::GatherLockTypes() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CUtilities::MX_LOCK_TYPES( -1 ); }
		CUtilities::MX_LOCK_TYPES dtRet = CUtilities::MX_LOCK_TYPES( -1 );
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		bool bFound = false;

		for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
			auto faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp && faTmp->Type() == MX_FAT_FOUND_ADDRESS ) {
				if ( !bFound ) {
					dtRet = CUtilities::MX_LOCK_TYPES( LockType( faTmp ) );
					bFound = true;
					continue;
				}
				if ( CUtilities::MX_LOCK_TYPES( LockType( faTmp ) ) != dtRet ) { return CUtilities::MX_LOCK_TYPES( -1 ); }
			}
		}

		return dtRet;
	}

	// Gathers all array lengths.
	CSecureWString CFoundAddressEditMainPage::GatherArrayLens() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CUtilities::MX_LOCK_TYPES dtRet = CUtilities::MX_LOCK_TYPES( -1 );
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		bool bFound = false;

		uint32_t ui32Len = 0;
		for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
			auto faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp && faTmp->Type() == MX_FAT_FOUND_ADDRESS ) {
				if ( !bFound ) {
					ui32Len = reinterpret_cast<CFoundAddress *>(faTmp)->ArrayLen();
					bFound = true;
					continue;
				}
				if ( reinterpret_cast<CFoundAddress *>(faTmp)->ArrayLen() != ui32Len ) { return CSecureWString(); }
			}
		}

		return CSecureWString( std::to_wstring( ui32Len ) );
	}

	// Gathers all array strides.
	CSecureWString CFoundAddressEditMainPage::GatherArrayStrides() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CUtilities::MX_LOCK_TYPES dtRet = CUtilities::MX_LOCK_TYPES( -1 );
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		bool bFound = false;

		uint16_t ui16Stride = 0;
		for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
			auto faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp && faTmp->Type() == MX_FAT_FOUND_ADDRESS ) {
				if ( !bFound ) {
					ui16Stride = reinterpret_cast<CFoundAddress *>(faTmp)->ArrayStride();
					bFound = true;
					continue;
				}
				if ( reinterpret_cast<CFoundAddress *>(faTmp)->ArrayStride() != ui16Stride ) { return CSecureWString(); }
			}
		}

		return CSecureWString( std::to_wstring( ui16Stride ) );
	}

	// Gets the lock type for a given found address.
	LPARAM CFoundAddressEditMainPage::LockType( CFoundAddressBase * _pfabFoundAddress ) {
		if ( _pfabFoundAddress->Type() != MX_FAT_FOUND_ADDRESS ) { return LPARAM( -1 ); }
		return LPARAM( reinterpret_cast<CFoundAddress *>(_pfabFoundAddress)->LockType() );
	}

}	// namespace mx

#include "MXOptionsPageGenSearch.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <CheckButton/LSWCheckButton.h>
#include <Edit/LSWEdit.h>
#include <RadioButton/LSWRadioButton.h>

namespace mx {

	COptionsPageGenSearch::COptionsPageGenSearch( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		COptionsPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<MX_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
	// Get the name of the page.
	std::wstring COptionsPageGenSearch::GetName() const {
		return _DEC_WS_F530FBCF_General_Search;
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COptionsPageGenSearch::InitDialog() {
		if ( !m_poOptions ) { return LSW_H_CONTINUE; }

		CCheckButton * pcbCheck = nullptr;
		CEdit * peEdit = nullptr;
		CRadioButton * prbRadio = nullptr;


#define MX_CHECK( ID, MEMBER )																					\
	pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::ID ));									\
	if ( pcbCheck ) {																							\
		pcbCheck->SetCheck( m_poOptions->MEMBER );																\
	}
		/*COptionsLayout::MX_OI_GENERAL_SEARCH_MEM_IMAGE;
		m_poOptions->bMemImage;*/
		MX_CHECK( MX_OI_GENERAL_SEARCH_MEM_IMAGE, bMemImage );
		MX_CHECK( MX_OI_GENERAL_SEARCH_MEM_PRIVATE, bMemPrivate );
		MX_CHECK( MX_OI_GENERAL_SEARCH_MEM_MAPPED, bMemMapped );

		MX_CHECK( MX_OI_GENERAL_SEARCH_POSTFLUSH, bPostFlush );
		MX_CHECK( MX_OI_GENERAL_SEARCH_PRECACHE, bPreCache );
		MX_CHECK( MX_OI_GENERAL_SEARCH_PAUSE, bPauseTarget );

		MX_CHECK( MX_OI_GENERAL_SEARCH_USE_EPSILON, bUseEpsilon );
		MX_CHECK( MX_OI_GENERAL_SEARCH_SMART_EPS, bSmartEpsilon );
#undef MX_CHECK

#define MX_CHECK( ID, MEMBER )																					\
	prbRadio = static_cast<CRadioButton *>(FindChild( COptionsLayout::ID ));									\
	if ( prbRadio ) {																							\
		prbRadio->SetCheck( m_poOptions->MEMBER );																\
	}
		MX_CHECK( MX_OI_GENERAL_SEARCH_TP_NORMAL, iThreadPriority == THREAD_PRIORITY_NORMAL );
		MX_CHECK( MX_OI_GENERAL_SEARCH_TP_HIGH, iThreadPriority == THREAD_PRIORITY_HIGHEST );
		MX_CHECK( MX_OI_GENERAL_SEARCH_TP_CRIT, iThreadPriority == THREAD_PRIORITY_TIME_CRITICAL );
#undef MX_CHECK

		peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_BUFFER_SIZE_EDIT ));
		if ( peEdit ) {
			peEdit->SetTextA( CUtilities::ToUnsigned( m_poOptions->ui64BufferSize / 1024 / 1024 ) );
		}

		peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_EPSILON_EDIT ));
		if ( peEdit ) {
			std::string sTemp;
			peEdit->SetTextA( CUtilities::ToDouble( m_poOptions->dEpsilon, sTemp ) );
		}

		ApplySettings();
		
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED COptionsPageGenSearch::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		if ( !m_poOptions ) { return LSW_H_CONTINUE; }
		ApplySettings();
		return LSW_H_CONTINUE;
	}

	// Verifies the options, returning an error string in case of error.
	BOOL COptionsPageGenSearch::Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
		ee::CExpEvalContainer::EE_RESULT eRes;
		CWidget * pwTemp = nullptr;
		pwTemp = FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_BUFFER_SIZE_EDIT );
		if ( pwTemp ) {
			if ( !pwTemp->GetTextAsUInt64Expression( eRes ) ) {
				_pwWidget = pwTemp;
				_wsError = _DEC_WS_C6AC1231_Invalid_search_buffer_size_;
				return FALSE;
			}
		}

		pwTemp = FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_EPSILON_EDIT );
		if ( pwTemp ) {
			if ( !pwTemp->GetTextAsDoubleExpression( eRes ) ) {
				_pwWidget = pwTemp;
				_wsError = _DEC_WS_552321B5_Invalid_epsilon_;
				return FALSE;
			}
		}
		return TRUE;
	}

	// Copies all the settings to the MX_OPTIONS structure.
	BOOL COptionsPageGenSearch::Finalize() {
		if ( !m_poOptions ) { return TRUE; }
		CCheckButton * pcbCheck = nullptr;
		CRadioButton * prbRadio = nullptr;
		CEdit * peEdit = nullptr;

		// ==== SEARCH BOOLEANS ==== //
#define MX_GETCHECK( ID, MEMBER )																				\
	pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::ID ));									\
	if ( pcbCheck ) {																							\
		m_poOptions->MEMBER = pcbCheck->IsChecked();															\
	}
		/*COptionsLayout::MX_OI_GENERAL_SEARCH_MEM_IMAGE;
		m_poOptions->bMemImage;*/
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_MEM_IMAGE, bMemImage );
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_MEM_PRIVATE, bMemPrivate );
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_MEM_MAPPED, bMemMapped );

		MX_GETCHECK( MX_OI_GENERAL_SEARCH_POSTFLUSH, bPostFlush );
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_PRECACHE, bPreCache );
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_PAUSE, bPauseTarget );

		MX_GETCHECK( MX_OI_GENERAL_SEARCH_USE_EPSILON, bUseEpsilon );
		MX_GETCHECK( MX_OI_GENERAL_SEARCH_SMART_EPS, bSmartEpsilon );
#undef MX_GETCHECK

		// ==== THREAD PRIORITY ==== //
		static const struct {
			COptionsLayout::MX_OPTIONS_IDS	iId;
			INT								iPri;
		} aTemp [] = {
			{ COptionsLayout::MX_OI_GENERAL_SEARCH_TP_NORMAL, THREAD_PRIORITY_NORMAL },
			{ COptionsLayout::MX_OI_GENERAL_SEARCH_TP_HIGH, THREAD_PRIORITY_HIGHEST },
			{ COptionsLayout::MX_OI_GENERAL_SEARCH_TP_CRIT, THREAD_PRIORITY_TIME_CRITICAL },
		};
		for ( size_t I = 0; I < std::size( aTemp ); ++I ) {
			prbRadio = static_cast<CRadioButton *>(FindChild( aTemp[I].iId ));
			if ( prbRadio ) {
				if ( prbRadio->IsChecked() ) {
					m_poOptions->iThreadPriority = aTemp[I].iPri;
					break;
				}
			}
		}

		// ==== EDIT BOXES ==== //
		ee::CExpEvalContainer::EE_RESULT eRes;
		peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_BUFFER_SIZE_EDIT ));
		if ( peEdit ) {
			if ( peEdit->GetTextAsUInt64Expression( eRes ) ) {
				m_poOptions->ui64BufferSize = eRes.u.ui64Val * 1024ULL * 1024ULL;
			}
		}

		peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_EPSILON_EDIT ));
		if ( peEdit ) {
			if ( peEdit->GetTextAsDoubleExpression( eRes ) ) {
				m_poOptions->dEpsilon = eRes.u.dVal;
			}
		}

		return TRUE;
	}

	// Apply dialog settings.
	VOID COptionsPageGenSearch::ApplySettings() {
		CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_USE_EPSILON ));
		if ( pcbCheck ) {
			CWidget * pwWidget = FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_EPSILON_EDIT );
			if ( pwWidget ) {
				pwWidget->SetEnabled( pcbCheck->IsChecked() );
			}
			pwWidget = FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_SMART_EPS );
			if ( pwWidget ) {
				pwWidget->SetEnabled( pcbCheck->IsChecked() );
			}
		}
	}

}	// namespace mx

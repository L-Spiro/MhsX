#include "MXOptionsPageGenSearch.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <CheckButton/LSWCheckButton.h>
#include <Edit/LSWEdit.h>
#include <RadioButton/LSWRadioButton.h>

namespace mx {

	COptionsPageGenSearch::COptionsPageGenSearch( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, MX_OPTIONS * _poOptions, bool _bCreateWidget, HMENU _hMenu ) :
		COptionsPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu ),
		m_poOptions( _poOptions ) {
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
			peEdit->SetTextA( CUtilities::ToUnsigned( m_poOptions->dwBufferSize / 1024 / 1024 ) );
		}

		peEdit = static_cast<CEdit *>(FindChild( COptionsLayout::MX_OI_GENERAL_SEARCH_EPSILON_EDIT ));
		if ( peEdit ) {
			std::string sTemp;
			peEdit->SetTextA( CUtilities::ToDouble( m_poOptions->dEpsilon, sTemp ) );
		}

		
		
		return LSW_H_CONTINUE;
	}

}	// namespace mx

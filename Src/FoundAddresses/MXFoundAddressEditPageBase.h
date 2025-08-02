#pragma once
#include "../MXMhsX.h"
#include <LSWWin.h>

#include <Tab/LSWTabPageBase.h>
#include <Widget/LSWWidget.h>

#include <string>

using namespace lsw;

namespace mx {

	class CMemHack;

	class CFoundAddressEditPageBase : public lsw::CTabPageBase {
	public :
		CFoundAddressEditPageBase( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 ) :
			lsw::CTabPageBase( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
			m_pParms( (*reinterpret_cast<MX_PARMS *>(_ui64Data)) ) {
		}


		// == Types.
		// To pass both an options pointer and an index.
		struct MX_PARMS {
			CMemHack *							pmhMemHack;
			std::vector<LPARAM> &				vSelection;
		};


		// == Functions.
		// Verifies the options, returning an error string in case of error.
		virtual BOOL							Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
			_pwWidget = nullptr;
			return FALSE;
		}

		// Copies all the settings to the MX_OPTIONS structure.
		virtual BOOL							Finalize() { return TRUE; }


	protected :
		// == Members.
		// Parameters.
		MX_PARMS								m_pParms;
	};

}	// namespace mx

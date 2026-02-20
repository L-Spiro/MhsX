#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXMemHack.h"
#include "../Options/MXOptions.h"
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CNumberInputLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_NUMBER_INPUT_IDS {
			MX_NI_NONE,
			MX_NI_DIALOG					= 4600,
			MX_NI_SEARCH_GROUP,
			MX_NI_DATA_TYPE_LABEL,
			MX_NI_DATA_TYPE_COMBO,
			MX_NI_EVALUATION_TYPE_LABEL,
			MX_NI_EVALUATION_TYPE_COMBO,
			MX_NI_LVAL_LABEL,
			MX_NI_RVAL_LABEL,
			MX_NI_LVAL_COMBO,
			MX_NI_RVAL_COMBO,
			MX_NI_EXP_LABEL,
			MX_NI_EXP_COMBO,

			MX_NI_CANCEL,
			MX_NI_OK,
		};


		// == Functions.
		// Creates the New Data-Type Search dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static DWORD							CreateNewDataTypeSearchDialog( CWidget * _pwParent, CMemHack * _pmhMemHack );


	protected :
		// == Members.
		// The layout for the New Data-Type Search dialog.
		static LSW_WIDGET_LAYOUT				m_wlNewDataTypeSearchDialog[];
	};

}	// namespace mx

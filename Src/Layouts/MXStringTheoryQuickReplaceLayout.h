#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXMemHack.h"
#include "../Options/MXOptions.h"
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CStringTheoryQuickReplaceLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_QUICK_REPLACE_IDS {
			MX_QRI_NONE,
			MX_QRI_DIALOG					= 4400,
			MX_QRI_SEARCH_GROUP,
			/*MX_QRI_DATA_TYPE_LABEL,
			MX_QRI_DATA_TYPE_COMBO,
			MX_QRI_EVALUATION_TYPE_LABEL,
			MX_QRI_EVALUATION_TYPE_COMBO,
			MX_QRI_LVAL_LABEL,
			MX_QRI_RVAL_LABEL,
			MX_QRI_LVAL_COMBO,
			MX_QRI_RVAL_COMBO,
			MX_QRI_EXP_LABEL,
			MX_QRI_EXP_COMBO,

			MX_QRI_OPTIONS_GROUP,
			MX_QRI_SAO_CHECK,
			MX_QRI_ALIGNED,
			MX_QRI_INVERT,

			MX_QRI_SEARCH_RANGE_GROUP,
			MX_QRI_FROM_LABEL,
			MX_QRI_TO_LABEL,
			MX_QRI_FROM_COMBO,
			MX_QRI_TO_COMBO,
			MX_QRI_SEARCH_BUTTON,
			MX_QRI_GENERAL_SEARCH_OPTIONS_BUTTON,*/

			MX_QRI_DESC_LABEL,

			MX_QRI_CANCEL,
			MX_QRI_OK,
		};


		// == Functions.
		// Creates the Quick Replace dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static DWORD							CreateQuickReplaceDialog( CWidget * _pwParent, CMemHack * _pmhMemHack );


	protected :
		// == Members.
		// The layout for the Quick Replace dialog.
		static LSW_WIDGET_LAYOUT				m_wlQuickReplaceDialog[];
	};

}	// namespace mx

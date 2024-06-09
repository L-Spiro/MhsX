#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXMemHack.h"
#include "../Options/MXOptions.h"
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CNewPointerSearchLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_NEW_POINTER_SEARCH_IDS {
			MX_NPSI_NONE,
			MX_NPSI_DIALOG					= 3900,
			MX_NPSI_SEARCH_GROUP,
			MX_NPSI_DATA_TYPE_LABEL,
			MX_NPSI_DATA_TYPE_COMBO,
			MX_NPSI_EVALUATION_TYPE_LABEL,
			MX_NPSI_EVALUATION_TYPE_COMBO,
			MX_NPSI_LVAL_LABEL,
			MX_NPSI_RVAL_LABEL,
			MX_NPSI_LVAL_COMBO,
			MX_NPSI_RVAL_COMBO,

			MX_NPSI_OPTIONS_GROUP,
			MX_NPSI_SAO_CHECK,
			MX_NPSI_ALIGNED,
			MX_NPSI_INVERT,

			MX_NPSI_SEARCH_RANGE_GROUP,
			MX_NPSI_FROM_LABEL,
			MX_NPSI_TO_LABEL,
			MX_NPSI_FROM_COMBO,
			MX_NPSI_TO_COMBO,
			MX_NPSI_SEARCH_BUTTON,
			MX_NPSI_GENERAL_SEARCH_OPTIONS_BUTTON,

			MX_NPSI_DESC_LABEL,

			MX_NPSI_CANCEL,
			MX_NPSI_OK,
		};


		// == Functions.
		// Creates the New Pointer Search dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static DWORD							CreateNewPointerSearchDialog( CWidget * _pwParent, CMemHack * _pmhMemHack );


	protected :
		// == Members.
		// The layout for the New Data-Type Search dialog.
		static LSW_WIDGET_LAYOUT				m_wlNewPointerSearchDialog[];
	};

}	// namespace mx

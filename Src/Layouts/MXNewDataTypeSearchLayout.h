#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXMemHack.h"
#include "../Options/MXOptions.h"
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CNewDataTypeSearchLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_NEW_DATA_SEARCH_IDS {
			MX_NDSI_NONE,
			MX_NDSI_DIALOG					= 3700,
			MX_NDSI_SEARCH_GROUP,
			MX_NDSI_DATA_TYPE_LABEL,
			MX_NDSI_DATA_TYPE_COMBO,
			MX_NDSI_EVALUATION_TYPE_LABEL,
			MX_NDSI_EVALUATION_TYPE_COMBO,
			MX_NDSI_LVAL_LABEL,
			MX_NDSI_RVAL_LABEL,
			MX_NDSI_LVAL_COMBO,
			MX_NDSI_RVAL_COMBO,
			MX_NDSI_EXP_LABEL,
			MX_NDSI_EXP_COMBO,

			MX_NDSI_OPTIONS_GROUP,
			MX_NDSI_SAO_CHECK,
			MX_NDSI_ALIGNED,
			MX_NDSI_INVERT,

			MX_NDSI_SEARCH_RANGE_GROUP,
			MX_NDSI_FROM_LABEL,
			MX_NDSI_TO_LABEL,
			MX_NDSI_FROM_COMBO,
			MX_NDSI_TO_COMBO,
			MX_NDSI_SEARCH_BUTTON,
			MX_NDSI_GENERAL_SEARCH_OPTIONS_BUTTON,

			MX_NDSI_DESC_LABEL,

			MX_NDSI_CANCEL,
			MX_NDSI_OK,
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

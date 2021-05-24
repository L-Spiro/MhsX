#pragma once

#include "../MemHack/MXMemHack.h"
#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CStandardSubsearchLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_CONTROL_IDS {
			MX_C_NONE,
			MX_C_MAINWINDOW						= 4900,
			MX_C_OK,
			MX_C_CANCEL,

			MX_C_INFO_GROUP,
			MX_C_INFO_ORIG_SEARCH_TYPE_LABEL,
			MX_C_INFO_ORIG_SEARCH_TYPE_VAL_LABEL,
			MX_C_INFO_SEARCH_TYPE_LABEL,
			MX_C_INFO_SEARCH_TYPE_VAL_LABEL,
			MX_C_INFO_RESULTS_LABEL,
			MX_C_INFO_RESULTS_VAL_LABEL,

			MX_C_SEARCH_GROUP,
			MX_C_SEARCH_TYPE_LABEL,
			MX_C_SEARCH_TYPE_COMBO,
			MX_C_SEARCH_LEFT_DESC_LABEL,
			MX_C_SEARCH_RIGHT_DESC_LABEL,
			MX_C_SEARCH_LEFT_VALUE_COMBO,
			MX_C_SEARCH_RIGHT_VALUE_COMBO,
			MX_C_SEARCH_EXP_LABEL,
			MX_C_SEARCH_EXP_COMBO,

			MX_C_OPTIONS_GROUP,
			MX_C_OPTIONS_INVERT_CHECK,
			MX_C_OPTIONS_OPTIONS_BUTTON,

			MX_C_DESC_LABEL,
		};


		// == Functions.
		// Creates the Standard Subsearch dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static DWORD							CreateStandardSubsearchDialog( CWidget * _pwParent, CMemHack * _pmhMemHack );


	protected :
		// == Members.
		// The layout for the window.
		static LSW_WIDGET_LAYOUT				m_wlMainWindow[];


		// == Functions.
		// Creates the description of the last search
	};

}	// namespace mx

#pragma once

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {
	
	class COptionsLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_OPTIONS_IDS {
			MX_OI_NONE,
			MX_OI_DIALOG,
			MX_OI_LIST,

			MX_OI_GENERAL,
			MX_OI_GENERAL_REFRESH_RATE,
			MX_OI_GENERAL_REFRESH_FOUND_ADDRESSES,
			MX_OI_GENERAL_REFRESH_FOUND_ADDRESSES_CB,
			MX_OI_GENERAL_TPS_0,

			MX_OI_GENERAL_REFRESH_MAIN_LIST,
			MX_OI_GENERAL_REFRESH_MAIN_LIST_CB,
			MX_OI_GENERAL_TPS_1,

			MX_OI_GENERAL_UPDDATE_LOCKED_VALUE,
			MX_OI_GENERAL_UPDDATE_LOCKED_VALUE_CB,
			MX_OI_GENERAL_TPS_2,

			MX_OI_GENERAL_UPDDATE_EXP_EVAL,
			MX_OI_GENERAL_UPDDATE_EXP_EVAL_CB,
			MX_OI_GENERAL_TPS_3,

			MX_OI_GENERAL_VIEW_SETTINGS,
			MX_OI_GENERAL_VIEW_CODE_NAMES,
			MX_OI_GENERAL_VIEW_SIZES,
			MX_OI_GENERAL_VIEW_RANGES,
		};


		// == Functions.
		// Creates the Options dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static BOOL								CreateOptionsDialog( CWidget * _pwParent );

		// Creates the general options page.
		static CWidget *						CreateGeneralPage( CWidget * _pwParent );


	protected :
		// == Members.
		// The layout for the Options dialog.
		static LSW_WIDGET_LAYOUT				m_wlOptionsDialog[];


		// General section.
		static LSW_WIDGET_LAYOUT				m_wlOptionsGeneral[];


		// == Functions.
		// Default window-creation.
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal );

	};

}	// namespace mx

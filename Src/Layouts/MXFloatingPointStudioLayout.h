#pragma once

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

#define MX_FPS_CHECKBOX_TRIM					2

namespace mx {

	class CFloatingPointStudioLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_CONTROL_IDS {
			MX_CI_NONE,
			MX_CI_MAINWINDOW					= 3600,
			MX_CI_CHECK_START					= MX_CI_MAINWINDOW + 1,
			MX_CI_LABEL_START					= MX_CI_CHECK_START + 65,
			MX_CI_KEY_LABEL_START				= MX_CI_LABEL_START + 65,
			MX_CI_VALUE_LABEL					= MX_CI_KEY_LABEL_START + 65,
			MX_CI_VALUE_EDIT,
			MX_CI_VALUE_HEX_LABEL,
			MX_CI_VALUE_HEX_EDIT,
			MX_CI_VALUE_BIN_LABEL,
			MX_CI_VALUE_BIN_EDIT,
			MX_CI_PARM_SIGNBIT_CHECK,
			MX_CI_PARM_IMPBIT_CHECK,
			MX_CI_PARM_EXP_LABEL,
			MX_CI_PARM_EXP_EDIT,
			MX_CI_PARM_MAN_LABEL,
			MX_CI_PARM_MAN_EDIT,

			MX_CI_OUT_BASE10_LABEL,
			MX_CI_OUT_BASE10_EDIT,
			MX_CI_OUT_BASE16_LABEL,
			MX_CI_OUT_BASE16_EDIT,
			MX_CI_OUT_BASE02_LABEL,
			MX_CI_OUT_BASE02_EDIT,

			MX_CI_OUT_PINF_LABEL,
			MX_CI_OUT_PINF_EDIT,
			MX_CI_OUT_NINF_LABEL,
			MX_CI_OUT_NINF_EDIT,
			MX_CI_OUT_NAN_LABEL,
			MX_CI_OUT_NAN_EDIT,
			MX_CI_OUT_MAX_LABEL,
			MX_CI_OUT_MAX_EDIT,
			MX_CI_OUT_MIN_LABEL,
			MX_CI_OUT_MIN_EDIT,
		};


		// == Functions.
		// Creates the window.
		static CWidget *						CreateFloatingPointStudioWindow( CWidget * _pwParent );


	protected :
		// == Members.
		// The layout for the window.
		static LSW_WIDGET_LAYOUT				m_wlMainWindow[];
	};

}	// namespace mx

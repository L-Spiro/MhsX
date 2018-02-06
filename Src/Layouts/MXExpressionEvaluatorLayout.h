#pragma once

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {
	
	// Layout for the Expression Evaluator dockable window.
	class CExpressionEvaluatorLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_EXP_EVAL_WINDOW_IDS {
			MX_EE_NONE,
			MX_EE_MAINWINDOW					= 5000,
			MX_EE_REBAR0,
			MX_EE_TOOLBAR0,
			MX_EE_LIST,
			MX_EE_EXP_TEXT,
			MX_EE_EXP_COMBO,
			MX_EE_EXP_OUTPUTTEXT,
			MX_EE_EXP_OUTPUTCOMBO,
			MX_EE_EXP_RESULT,
		};

		// Buttons.
		enum MX_BUTTON_COMMANDS {
			MX_BC_ADD							= 6013,
			MX_BC_REMOVE,
			MX_BC_SCINOT,
			MX_BC_EXTOUT,
		};


		// == Functions.
		// Creates the Expression Evaluator dockable window.
		static CWidget *						CreateExpEvalWindow( CWidget * _pwParent );



	protected :
		// == Members.
		// The layout for the Expression Evaluator dockable window.
		static LSW_WIDGET_LAYOUT				m_wlMainWindow[];

	};

}	// namespace mx

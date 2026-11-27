#pragma once

#include "../MXMhsX.h"
#include "../HexEditor/MXInspector.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	/**
	 * The layouts of the Inspector (a dockable window of the Hex Editor) and its Customize dialog.
	 **/
	class CInspectorLayout {
	public :
		// == Enumerations.
		/** Control IDs of the Inspector. */
		enum MX_INSPECTOR_IDS {
			MX_II_NONE,
			MX_II_MAINWINDOW												= 3100,
			MX_II_LIST,
		};

		/** Control IDs of the Customize dialog. */
		enum MX_INSPECTOR_CUSTOMIZE_IDS {
			MX_ICI_NONE,
			MX_ICI_DIALOG													= 6600,
			MX_ICI_TYPES_LABEL,
			MX_ICI_TYPES_LIST,
			MX_ICI_MOVE_UP,
			MX_ICI_MOVE_DOWN,
			MX_ICI_RESET,
			MX_ICI_DATE_LABEL,
			MX_ICI_DATE_COMBO,
			MX_ICI_TIME_LABEL,
			MX_ICI_TIME_COMBO,
			MX_ICI_OK,
			MX_ICI_CANCEL,
		};


		// == Types.
		/** Parameters of the Customize dialog. */
		struct MX_CUSTOMIZE_PARMS {
			CInspector::MX_SETTINGS											sSettings;				/**< In: the current settings.  Out: the settings chosen. */
		};


		// == Functions.
		/**
		 * Creates the Inspector.
		 *
		 * \param _pwParent The parent (the dock target it is created in).
		 * \return Returns the Inspector, or nullptr on failure.
		 **/
		static CWidget *													CreateInspectorWindow( CWidget * _pwParent );

		/**
		 * Shows the Customize dialog of the Inspector.
		 *
		 * \param _pwParent The parent window.
		 * \param _cpParms The current settings on input and the chosen settings on output.
		 * \return Returns true if OK was pressed.
		 **/
		static bool															CreateCustomizeDialog( CWidget * _pwParent, MX_CUSTOMIZE_PARMS &_cpParms );


	protected :
		// == Members.
		/** The layout of the Inspector. */
		static LSW_WIDGET_LAYOUT											m_wlInspector[];
		/** The layout of the Customize dialog. */
		static LSW_WIDGET_LAYOUT											m_wlCustomize[];
	};

}	// namespace mx

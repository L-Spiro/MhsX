#pragma once

#include "../MXMhsX.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	/**
	 * The layout of the Find Results window (a dockable window of the Hex Editor).
	 **/
	class CFindResultsLayout {
	public :
		// == Enumerations.
		/** Control IDs. */
		enum MX_FIND_RESULTS_IDS {
			MX_FRI_NONE,
			MX_FRI_MAINWINDOW												= 6900,
			MX_FRI_MAP,
			MX_FRI_LIST,
		};


		// == Functions.
		/**
		 * Creates the Find Results window.
		 *
		 * \param _pwParent The parent (the dock target it is created in).
		 * \return Returns the window, or nullptr on failure.
		 **/
		static CWidget *													CreateFindResultsWindow( CWidget * _pwParent );


	protected :
		// == Members.
		/** The layout of the Find Results window. */
		static LSW_WIDGET_LAYOUT											m_wlFindResults[];
	};

}	// namespace mx

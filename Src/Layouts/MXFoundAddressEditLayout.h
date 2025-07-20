#pragma once

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {
	
	class CMemHack;

	class CFoundAddressEditLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_FOUND_ADDRESS_EDIT_IDS : WORD {
			MX_FAEI_NONE,
			MX_FAEI_DIALOG					= 5200,
			MX_FAEI_TAB,

			MX_FAEI_GENERAL_PAGE,
			MX_FAEI_GENERAL_GROUP,
			MX_FAEI_GENERAL_DESC_LABEL,
			MX_FAEI_GENERAL_DESC_COMBO,
			MX_FAEI_GENERAL_ARRAY_CHECK,
			MX_FAEI_GENERAL_COLOR_BUTTON,
		};


		// == Functions.
		// Creates the Edit dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static BOOL								CreateEditDialog( CWidget * _pwParent, CMemHack * _poEdit, int32_t _i32Page );

		// Creates the general edit page.
		static CWidget *						CreateGeneralPage( CWidget * _pwParent, CMemHack * _poEdit );

		//// Creates the open-process edit page.
		//static CWidget *						CreateOpenProcPage( CWidget * _pwParent, CMemHack * _poEdit );

		//// Creates the general search edit page.
		//static CWidget *						CreateGeneralSearchPage( CWidget * _pwParent, CMemHack * _poEdit );

		//// Creates the extended search edit page.
		//static CWidget *						CreateSearchExPage( CWidget * _pwParent, CMemHack * _poEdit );

		//// Creates the hotkeys edit page.
		//static CWidget *						CreateHotkeysPage( CWidget * _pwParent, CMemHack * _poEdit );

		//// Creates the programs edit page.
		//static CWidget *						CreateProgramsPage( CWidget * _pwParent, CMemHack * _poEdit );



	protected :
		// == Members.
		// The layout for the edit dialog.
		static LSW_WIDGET_LAYOUT				m_wlMainDialog[];

		// General section.
		static LSW_WIDGET_LAYOUT				m_wlOptionsGeneral[];


		// == Functions.
		// Default window-creation.
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, CMemHack * _poEdit );
	};

}	// namespace mx

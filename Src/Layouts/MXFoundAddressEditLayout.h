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
			MX_FAEI_OK,
			MX_FAEI_CANCEL,

			MX_FAEI_GENERAL_PAGE,
			MX_FAEI_GENERAL_GROUP,
			MX_FAEI_GENERAL_DESC_LABEL,
			MX_FAEI_GENERAL_DESC_COMBO,
			MX_FAEI_GENERAL_ARRAY_CHECK,
			MX_FAEI_GENERAL_COLOR_BUTTON,

			MX_FAEI_VALUE_GROUP,
			MX_FAEI_VALUE_CUR_VAL_LABEL,
			MX_FAEI_VALUE_CUR_VAL_COMBO,
			MX_FAEI_VALUE_CUR_VAL_DESC_LABEL,
			MX_FAEI_VALUE_TYPE_LABEL,
			MX_FAEI_VALUE_TYPE_COMBO,
			MX_FAEI_VALUE_TYPE_STRING_COMBO,
			MX_FAEI_VALUE_TYPE_DATA_COMBO,
			MX_FAEI_VALUE_TYPE_SHOW_AS_HEX_CHECK,
			MX_FAEI_VALUE_TYPE_SCIENTIFIC_CHECK,
			MX_FAEI_VALUE_TYPE_ESCAPE_CHECK,
			MX_FAEI_VALUE_ARRAY_LEN_LABEL,
			MX_FAEI_VALUE_ARRAY_LEN_COMBO,
			MX_FAEI_VALUE_ARRAY_STRIDE_LABEL,
			MX_FAEI_VALUE_ARRAY_STRIDE_COMBO,

			MX_FAEI_QL_GROUP,
			MX_FAEI_QL_LOCK_TYPE_LABEL,
			MX_FAEI_QL_LOCK_TYPE_COMBO,
			MX_FAEI_QL_STRING_LOCK_TYPE_COMBO,
			MX_FAEI_QL_LOCKED_CHECK,
			MX_FAEI_QL_VALUE_WHEN_LOCKED_GROUP,
			MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_LABEL,
			MX_FAEI_QL_VALUE_WHEN_LOCKED_MIN_LABEL,
			MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO,
			MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_LABEL,
			MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO,

			MX_FAEI_INFO_GROUP,
			MX_FAEI_INFO_ADDRESS_LABEL,
			MX_FAEI_INFO_ADDRESS_EDIT,
			MX_FAEI_INFO_MODULE_LABEL,
			MX_FAEI_INFO_MODULE_EDIT,
			MX_FAEI_INFO_DESC_LABEL,
		};


		// == Functions.
		// Creates the Edit dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static BOOL								CreateEditDialog( CWidget * _pwParent, CMemHack * _poMemHack, std::vector<LPARAM> &_vSelected );

		// Creates the general edit page.
		static CWidget *						CreateGeneralPage( CWidget * _pwParent, CMemHack * _poMemHack, std::vector<LPARAM> &_vSelected );

		//// Creates the open-process edit page.
		//static CWidget *						CreateOpenProcPage( CWidget * _pwParent, CMemHack * _poMemHack, std::vector<LPARAM> &_vSelected );

		//// Creates the general search edit page.
		//static CWidget *						CreateGeneralSearchPage( CWidget * _pwParent, CMemHack * _poMemHack, std::vector<LPARAM> &_vSelected );

		//// Creates the extended search edit page.
		//static CWidget *						CreateSearchExPage( CWidget * _pwParent, CMemHack * _poMemHack, std::vector<LPARAM> &_vSelected );

		//// Creates the hotkeys edit page.
		//static CWidget *						CreateHotkeysPage( CWidget * _pwParent, CMemHack * _poMemHack, std::vector<LPARAM> &_vSelected );

		//// Creates the programs edit page.
		//static CWidget *						CreateProgramsPage( CWidget * _pwParent, CMemHack * _poMemHack, std::vector<LPARAM> &_vSelected );



	protected :
		// == Members.
		// The layout for the edit dialog.
		static LSW_WIDGET_LAYOUT				m_wlMainDialog[];

		// General section.
		static LSW_WIDGET_LAYOUT				m_wlOptionsGeneral[];


		// == Functions.
		// Default window-creation.
		static CWidget *						CreatePage( CWidget * _pwParent, const LSW_WIDGET_LAYOUT * _pwlLayout, size_t _sTotal, CMemHack * _poMemHack, std::vector<LPARAM> &_vSelected );
	};

}	// namespace mx

#pragma once
#include "../MXMhsX.h"
#include "../Options/MXOptions.h"
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CMemHack;
	
	class CAddFoundAddressWindowLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_ADD_ENTRY_IDS : WORD {
			MX_ADI_NONE,
			MX_ADI_DIALOG					= 4500,
			MX_ADI_OK,
			MX_ADI_CANCEL,

			MX_ADI_GROUP,
			MX_ADI_DESC_LABEL,
			MX_ADI_DESC_COMBO,
			MX_ADI_ADDRESS_LABEL,
			MX_ADI_ADDRESS_COMBO,
			MX_ADI_TYPE_LABEL,
			MX_ADI_TYPE_COMBO,

			MX_ADI_INFO_GROUP,
			MX_ADI_INFO_VALUE_LABEL,
			MX_ADI_INFO_VALUE_EDIT
		};


		// == Functions.
		// Creates the Add Found Address dialog.
		static BOOL								CreateAddEntryDialog( CWidget * _pwParent, CMemHack * _pmhMemHack );


	protected :
		// == Members.
		// The layout for the dialog.
		static LSW_WIDGET_LAYOUT				m_wlDialog[];

	};

}	// namespace mx

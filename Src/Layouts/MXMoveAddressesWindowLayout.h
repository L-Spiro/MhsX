#pragma once
#include "../MXMhsX.h"
#include "../Options/MXOptions.h"
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CWindowMemHack;
	
	class CMoveAddressesWindowLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_MOVE_ADDRESS_IDS : WORD {
			MX_MAI_NONE,
			MX_MAI_DIALOG					= 4500,
			MX_MAI_OK,
			MX_MAI_CANCEL,

			MX_MAI_GROUP,
			MX_MAI_ADDR_START_LABEL,
			MX_MAI_ADDR_START_COMBO,
			MX_MAI_ADDR_END_LABEL,
			MX_MAI_ADDR_END_COMBO,
			MX_MAI_TYPE_LABEL,
			MX_MAI_TYPE_COMBO,

			MX_MAI_INFO_GROUP,
			MX_MAI_INFO_VALUE_LABEL,
			MX_MAI_INFO_VALUE_EDIT
		};


		// == Functions.
		// Creates the Move Addresses dialog.
		static BOOL								CreateMoveAddressDialog( CWidget * _pwParent, CWindowMemHack * _pmhMemHack );


	protected :
		// == Members.
		// The layout for the dialog.
		static LSW_WIDGET_LAYOUT				m_wlDialog[];

	};

}	// namespace mx

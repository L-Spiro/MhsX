#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXMemHack.h"
#include "../Options/MXOptions.h"
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CNewExpressionSearchLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_NEW_EXPRESSION_SEARCH_IDS {
			MX_NESI_NONE,
			MX_NESI_DIALOG					= 3800,

			MX_NESI_SETTINGS_GROUP,
			MX_NESI_SETTINGS_QQ_LABEL,
			MX_NESI_SETTINGS_QQ_COMBO,
			MX_NESI_SETTINGS_DESC_LABEL,

			MX_NESI_SETTINGS_QQ_TYPE_LABEL,
			MX_NESI_SETTINGS_QQ_TYPE_COMBO,

			MX_NESI_SIZE_GROUP,
			MX_NESI_SIZE_DYN_SIZE_RADIO,
			MX_NESI_SIZE_FIXED_SIZE_RADIO,			
			MX_NESI_SIZE_FIXED_SIZE_COMBO,
			MX_NESI_SIZE_FIXED_SIZE_LABEL,

			MX_NESI_SIZE_SIZE_LABEL,

			MX_NESI_EXPRESSION_GROUP,
			MX_NESI_EXPRESSION_EDIT,
			MX_NESI_EXPRESSION_DUMMY_LABEL,
			MX_NESI_EXPRESSION_DUMMY_COMBO,
			MX_NESI_EXPRESSION_DUMMY_HELP_LABEL,
			MX_NESI_EXPRESSION_RESULT_LABEL,
			MX_NESI_EXPRESSION_RESULT_EDIT,
			MX_NESI_EXPRESSION_RESULT_HELP_LABEL,

			MX_NESI_OPTIONS_GROUP,
			MX_NESI_OPTIONS_ALIGN_LABEL,
			MX_NESI_OPTIONS_ALIGN_COMBO,
			MX_NESI_OPTIONS_ALIGN_BYTES_LABEL,
			MX_NESI_OPTIONS_GENERAL_OPTIONS_BUTTON,

			MX_NESI_SEARCH_RANGE_GROUP,
			MX_NESI_FROM_LABEL,
			MX_NESI_TO_LABEL,
			MX_NESI_FROM_COMBO,
			MX_NESI_TO_COMBO,

			

			MX_NESI_CANCEL,
			MX_NESI_OK,
		};


		// == Functions.
		// Creates the New Eexpression Search dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static DWORD							CreateNewExpressionSearchDialog( CWidget * _pwParent, CMemHack * _pmhMemHack );


	protected :
		// == Members.
		// The layout for the New Expression Search dialog.
		static LSW_WIDGET_LAYOUT				m_wlNewExpressionSearchDialog[];
	};

}	// namespace mx

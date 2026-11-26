#pragma once
#include "../MXMhsX.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	/**
	 * The layout of the Select Range dialog (Select -> Select Range...), which selects a range of bytes in the Hex Editor given its
	 *	start address and its size or end address.
	 **/
	class CSelectRangeLayout {
	public :
		// == Enumerations.
		/** Control IDs. */
		enum MX_SELECT_RANGE_IDS {
			MX_SRI_NONE,
			MX_SRI_DIALOG						= 6100,
			MX_SRI_START_LABEL,
			MX_SRI_START_EDIT,
			MX_SRI_SIZE_RADIO,
			MX_SRI_END_RADIO,
			MX_SRI_VALUE_EDIT,

			MX_SRI_OK,
			MX_SRI_CANCEL,
		};


		// == Types.
		/** Parameters passed to and returned by the dialog. */
		struct MX_SELECT_RANGE_PARMS {
			uint64_t							ui64Start = 0;						/**< In: the default start address.  Out: the start address entered. */
			uint64_t							ui64Size = 0;						/**< In: the default size.  Out: the number of bytes to select. */
			uint64_t							ui64DataSize = 0;					/**< The size of the data (the start address may not be greater than this). */
			uint64_t							ui64Bias = 0;						/**< The custom starting address: addresses are entered as shown, with it added. */
			bool								bHex = true;						/**< If true, numbers are shown and entered in hexadecimal by default. */
			bool								bEnd = false;						/**< In/out: true if the second value is the (inclusive) end address rather than the size. */
		};


		// == Functions.
		/**
		 * Shows the Select Range dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		 *
		 * \param _pwParent The parent window.
		 * \param _srpParms The default values on input and the values entered on output.
		 * \return Returns TRUE if the user pressed OK.
		 **/
		static DWORD							CreateSelectRangeDialog( CWidget * _pwParent, MX_SELECT_RANGE_PARMS &_srpParms );


	protected :
		// == Members.
		/** The layout of the Select Range dialog. */
		static LSW_WIDGET_LAYOUT				m_wlSelectRangeDialog[];
	};

}	// namespace mx

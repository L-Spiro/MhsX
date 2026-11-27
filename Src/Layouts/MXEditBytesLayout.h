#pragma once
#include "../MXMhsX.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	/**
	 * The layouts of the Insert Bytes, Overwrite Bytes, and Set File Size dialogs (Edit -> Insert/Overwrite).  All three are run by
	 *	CEditBytesDialog and share control IDs; Set File Size has no start address or size/end choice.
	 **/
	class CEditBytesLayout {
	public :
		// == Enumerations.
		/** Control IDs. */
		enum MX_EDIT_BYTES_IDS {
			MX_EBI_NONE,
			MX_EBI_DIALOG						= 6700,
			MX_EBI_START_LABEL,
			MX_EBI_START_EDIT,
			MX_EBI_SIZE_RADIO,
			MX_EBI_END_RADIO,
			MX_EBI_VALUE_EDIT,
			MX_EBI_INFO_LABEL,
			MX_EBI_NUMBERS_LABEL,
			MX_EBI_HEX_RADIO,
			MX_EBI_DEC_RADIO,
			MX_EBI_BYTE_LABEL,
			MX_EBI_BV_HEX_LABEL,
			MX_EBI_BV_HEX_EDIT,
			MX_EBI_BV_DEC_LABEL,
			MX_EBI_BV_DEC_EDIT,
			MX_EBI_BV_CHAR_LABEL,
			MX_EBI_BV_CHAR_EDIT,

			MX_EBI_OK,
			MX_EBI_CANCEL,
		};

		/** What the dialog does. */
		enum MX_EDIT_BYTES_MODE {
			MX_EBM_INSERT,														/**< Insert Bytes: inserts a run of one byte value. */
			MX_EBM_OVERWRITE,													/**< Overwrite Bytes: writes one byte value over a range. */
			MX_EBM_SET_SIZE,													/**< Set File Size: removes bytes from the end or adds them. */
		};


		// == Types.
		/** Parameters passed to and returned by the dialog. */
		struct MX_EDIT_BYTES_PARMS {
			MX_EDIT_BYTES_MODE					ebmMode = MX_EBM_INSERT;			/**< What the dialog does. */
			uint64_t							ui64Start = 0;						/**< In: the default start address.  Out: the start address entered (without the starting address). */
			uint64_t							ui64Size = 0;						/**< In: the default size (the current size for Set File Size).  Out: the size entered (the new size for Set File Size). */
			uint64_t							ui64DataSize = 0;					/**< The size of the data. */
			uint64_t							ui64Bias = 0;						/**< The custom starting address: addresses are entered as shown, with it added. */
			bool								bHex = true;						/**< In/out: if true, numbers are entered in hexadecimal. */
			bool								bEnd = false;						/**< In/out: true if the second value is the (inclusive) end address rather than the size. */
			bool								bCanGrow = true;					/**< If true, bytes may be written past the end of the data (adding them). */
			uint8_t								ui8Value = 0;						/**< In/out: the byte value. */
		};


		// == Functions.
		/**
		 * Shows the Insert Bytes, Overwrite Bytes, or Set File Size dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it
		 *	can decode strings etc.
		 *
		 * \param _pwParent The parent window.
		 * \param _ebpParms The default values on input and the values entered on output.
		 * \return Returns TRUE if the user pressed OK.
		 **/
		static DWORD							CreateEditBytesDialog( CWidget * _pwParent, MX_EDIT_BYTES_PARMS &_ebpParms );


	protected :
		// == Members.
		/** The layout of the Insert Bytes and Overwrite Bytes dialogs. */
		static LSW_WIDGET_LAYOUT				m_wlEditBytesDialog[];
		/** The layout of the Set File Size dialog. */
		static LSW_WIDGET_LAYOUT				m_wlSetSizeDialog[];
	};

}	// namespace mx

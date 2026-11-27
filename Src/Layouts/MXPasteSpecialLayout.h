#pragma once
#include "../MXMhsX.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

#include <vector>

using namespace lsw;

namespace mx {

	/**
	 * The layout of the Paste Special dialog (Edit -> Paste Special...), which lists the formats on the Windows clipboard and pastes
	 *	the raw bytes of the one chosen.
	 **/
	class CPasteSpecialLayout {
	public :
		// == Enumerations.
		/** Control IDs. */
		enum MX_PASTE_SPECIAL_IDS {
			MX_PSI_NONE,
			MX_PSI_DIALOG						= 6800,
			MX_PSI_FORMATS_LABEL,
			MX_PSI_FORMATS_LIST,

			MX_PSI_PASTE,
			MX_PSI_CANCEL,
		};


		// == Types.
		/** Parameters returned by the dialog. */
		struct MX_PASTE_SPECIAL_PARMS {
			std::vector<uint8_t>				vBytes;								/**< Out: the bytes of the chosen format. */
			UINT								uiFormat = 0;						/**< In: the format to select by default (the last one used).  Out: the chosen format. */
		};


		// == Functions.
		/**
		 * Shows the Paste Special dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		 *
		 * \param _pwParent The parent window.
		 * \param _pspParms Receives the bytes of the chosen format.
		 * \return Returns TRUE if the user pressed Paste and bytes were read.
		 **/
		static DWORD							CreatePasteSpecialDialog( CWidget * _pwParent, MX_PASTE_SPECIAL_PARMS &_pspParms );


	protected :
		// == Members.
		/** The layout of the Paste Special dialog. */
		static LSW_WIDGET_LAYOUT				m_wlPasteSpecialDialog[];
	};

}	// namespace mx

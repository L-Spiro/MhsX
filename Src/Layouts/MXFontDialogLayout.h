#pragma once
#include "../MXMhsX.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	/**
	 * The layout of the Change Font dialog (View -> Font -> Change Font...), which chooses the font of the Hex Editor.  The dialog
	 *	can be resized: the lists grow with it, the Font and Font Style columns share the extra width, and the Size column keeps its
	 *	width.
	 **/
	class CFontDialogLayout {
	public :
		// == Enumerations.
		/** Control IDs. */
		enum MX_FONT_DIALOG_IDS {
			MX_FDI_NONE,
			MX_FDI_DIALOG						= 6200,
			MX_FDI_FONT_LABEL,
			MX_FDI_FONT_EDIT,
			MX_FDI_FONT_LIST,
			MX_FDI_STYLE_LABEL,
			MX_FDI_STYLE_EDIT,
			MX_FDI_STYLE_LIST,
			MX_FDI_SIZE_LABEL,
			MX_FDI_SIZE_EDIT,
			MX_FDI_SIZE_LIST,
			MX_FDI_EFFECTS_GROUP,
			MX_FDI_STRIKEOUT,
			MX_FDI_UNDERLINE,
			MX_FDI_WS_LABEL,
			MX_FDI_WS_COMBO,
			MX_FDI_SAMPLE_GROUP,
			MX_FDI_SAMPLE,

			MX_FDI_OK,
			MX_FDI_CANCEL,
		};


		// == Types.
		/** Parameters passed to and returned by the dialog. */
		struct MX_FONT_DIALOG_PARMS {
			LOGFONTW							lfFont {};							/**< In: the current font.  Out: the chosen font (lfHeight for ui32Dpi). */
			int32_t								i32PointSize = 10;					/**< In: the current size.  Out: the chosen size, in points. */
			uint32_t							ui32Dpi = 96;						/**< The DPI used to convert between points and pixels. */
		};


		// == Functions.
		/**
		 * Shows the Change Font dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		 *
		 * \param _pwParent The parent window.
		 * \param _fdpParms The current font on input and the chosen font on output.
		 * \return Returns TRUE if the user pressed OK.
		 **/
		static DWORD							CreateFontDialog( CWidget * _pwParent, MX_FONT_DIALOG_PARMS &_fdpParms );


	protected :
		// == Members.
		/** The layout of the Change Font dialog. */
		static LSW_WIDGET_LAYOUT				m_wlFontDialog[];
	};

}	// namespace mx

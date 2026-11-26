#pragma once
#include "../MXMhsX.h"
#include "../HexEditor/MXMiniMap.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CHexEditorControl;

	/**
	 * The layout of the Mini Map Options dialog (View -> Mini Map -> Mini Map Options..., or right-clicking the Mini Map).
	 **/
	class CMiniMapOptionsLayout {
	public :
		// == Enumerations.
		/** Control IDs. */
		enum MX_MINI_MAP_OPTIONS_IDS {
			MX_MMOI_NONE,
			MX_MMOI_DIALOG						= 6500,
			MX_MMOI_POSITION_LABEL,
			MX_MMOI_POSITION_COMBO,
			MX_MMOI_HIDE,
			MX_MMOI_RESET,
			MX_MMOI_COLORIZE_CHECK,
			MX_MMOI_COLORIZE_COMBO,
			MX_MMOI_HIGHLIGHT_STRUCTS,
			MX_MMOI_BPR_LABEL,
			MX_MMOI_BPR_EDIT,
			MX_MMOI_AUTO_SNAP_CHECK,
			MX_MMOI_AUTO_SNAP_EDIT,
			MX_MMOI_BRIGHTNESS_LABEL,
			MX_MMOI_BRIGHTNESS_TRACK,
			MX_MMOI_BRIGHTNESS_VALUE,
			MX_MMOI_CONTRAST_LABEL,
			MX_MMOI_CONTRAST_TRACK,
			MX_MMOI_CONTRAST_VALUE,
			MX_MMOI_ZOOM_LABEL,
			MX_MMOI_ZOOM_TRACK,
			MX_MMOI_ZOOM_VALUE,

			MX_MMOI_OK,
			MX_MMOI_CANCEL,
		};


		// == Types.
		/** Parameters passed to and returned by the dialog. */
		struct MX_MINI_MAP_OPTIONS_PARMS {
			CMiniMap::MX_MINI_MAP				mmOptions;							/**< In: the current options.  Out: the options chosen. */
			CHexEditorControl *					phecControl = nullptr;				/**< The hex editor that shows the changes while the dialog is open (may be nullptr). */
			bool								bHide = false;						/**< Out: true if Hide was pressed (the Mini Map should be hidden). */
		};


		// == Functions.
		/**
		 * Shows the Mini Map Options dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		 *
		 * \param _pwParent The parent window.
		 * \param _mmopParms The current options on input and the chosen options on output.
		 * \return Returns TRUE if the user pressed OK or Hide.
		 **/
		static DWORD							CreateMiniMapOptionsDialog( CWidget * _pwParent, MX_MINI_MAP_OPTIONS_PARMS &_mmopParms );


	protected :
		// == Members.
		/** The layout of the Mini Map Options dialog. */
		static LSW_WIDGET_LAYOUT				m_wlMiniMapOptionsDialog[];
	};

}	// namespace mx

#pragma once
#include "../MXMhsX.h"
#include "../HexEditor/MXHexFind.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

#include <functional>
#include <vector>

using namespace lsw;

namespace mx {

	/**
	 * The layout of the Hex Editor's Find dialog (Find -> Find...).
	 **/
	class CFindLayout {
	public :
		// == Enumerations.
		/** Control IDs. */
		enum MX_FIND_IDS {
			MX_FI_NONE,
			MX_FI_DIALOG						= 6400,
			MX_FI_FIND_LABEL,
			MX_FI_FIND_COMBO,
			MX_FI_TYPE_LABEL,
			MX_FI_TYPE_COMBO,
			MX_FI_TOLERANCE_LABEL,
			MX_FI_TOLERANCE_EDIT,
			MX_FI_STATUS,
			MX_FI_OPTIONS_GROUP,
			MX_FI_FIND_ALL_OCCURRENCES,
			MX_FI_MATCH_CASE,
			MX_FI_WHOLE_WORD,
			MX_FI_REGEX,
			MX_FI_WILDCARDS,
			MX_FI_MULTIPLE_RANGES,
			MX_FI_DIRECTION_LABEL,
			MX_FI_UP,
			MX_FI_DOWN,
			MX_FI_RANGE_LABEL,
			MX_FI_LOCK_TO_SELECTION,
			MX_FI_ADVANCED_GROUP,
			MX_FI_INCREMENTAL,
			MX_FI_WRAP,
			MX_FI_TYPE_SPECIFIERS,
			MX_FI_HIDE_AFTER,
			MX_FI_QUICK_FIND,
			MX_FI_CUSTOM_COLOR,
			MX_FI_FORE_CHECK,
			MX_FI_FORE_COLOR,
			MX_FI_BACK_LABEL,
			MX_FI_BACK_COLOR,
			MX_FI_LIMIT,
			MX_FI_LIMIT_EDIT,
			MX_FI_MAX_WILDCARD_LABEL,
			MX_FI_MAX_WILDCARD_EDIT,

			MX_FI_FIND,
			MX_FI_FIND_NEXT,
			MX_FI_FIND_PREV,
			MX_FI_FIND_ALL,
			MX_FI_CLOSE,
		};

		/** What the dialog asks the Hex Editor to do. */
		enum MX_FIND_ACTION {
			MX_FA_NEXT,												/**< Find the next match. */
			MX_FA_PREV,												/**< Find the previous match. */
			MX_FA_ALL,												/**< Find all matches. */
			MX_FA_INCREMENTAL,										/**< Find from where the search started (the text changed). */
		};


		// == Types.
		/** Parameters passed to the dialog. */
		struct MX_FIND_DIALOG_PARMS {
			CHexFind::MX_FIND_PARMS *			pfpParms = nullptr;				/**< In/out: the search settings. */
			std::vector<CSecureWString> *		pvHistory = nullptr;			/**< In/out: what was searched for before (most recent first). */
			bool								bHasSelection = false;			/**< Whether there is a selection (for Lock to Selection). */
			uint64_t							ui64SelStart = 0;				/**< The start of the selection. */
			uint64_t							ui64SelSize = 0;				/**< The size of the selection. */
			/** Does a search with the settings in (*pfpParms) and fills the status text.  Returns true if a match was selected (Find
			 *	Next/Previous) or if the search was done (Find All, even if nothing was found). */
			std::function<bool ( MX_FIND_ACTION, CSecureWString & )>
												fAction;
		};


		// == Functions.
		/**
		 * Shows the Find dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		 *
		 * \param _pwParent The parent window.
		 * \param _fdpParms The settings and the function that searches.
		 * \return Returns the dialog result.
		 **/
		static DWORD							CreateFindDialog( CWidget * _pwParent, MX_FIND_DIALOG_PARMS &_fdpParms );


	protected :
		// == Members.
		/** The layout of the Find dialog. */
		static LSW_WIDGET_LAYOUT				m_wlFindDialog[];
	};

}	// namespace mx

#pragma once

#include "../MXMhsX.h"
#include "../CodePages/MXCodePages.h"

namespace mx {

	class CCharSets {
	public :
		// == Types.
		/** A character set. */
		struct MX_CHAR_SET {
			const char *												pcName = nullptr;								/** Name of the character set. Appears in the menu. */
			size_t														sNameLen = 0;									/** Length of the name. */
			const char *												pcStatusBar = nullptr;							/** Status-bar indicator. */
			size_t														sStatusLen = 0;									/** Status-bar indicator length. */
			uint32_t													ui32Code = CCodePages::MX_Windows_1252;			/** Code page. */
			uint32_t													ui32Id = 0;										/** ID. */
			bool														bHideOver127 = true;							/** Hide values over 127. */
			bool														bTopMenu = false;								/** Show in the top-level menu. */
		};


		// == Functions.
		// Gets the total number of presets.
		static size_t													Total();


		// == Members.
		static const MX_CHAR_SET										m_csSets[];										/** The predefined character sets. */
		
	};

}	// namespace mx

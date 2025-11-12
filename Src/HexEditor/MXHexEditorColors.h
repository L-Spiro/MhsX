#pragma once

#include "../MXMhsX.h"
#include "../CodePages/MXCodePages.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"

namespace mx {

	struct MX_HEX_EDITOR_COLORS {
		// == Members.
#define MX_COLOR( NAME, TEXTNAME, FG, BG )																											\
	COLORREF									cr ## NAME;																							\
	static constexpr COLORREF					NAME ## Default_Fg() { return FG; }																	\
	static constexpr COLORREF					NAME ## Default_Bg() { return BG; }
#include "MXHexEditorColorImpl.h"

	
		// == Functions.
		// Sets the default colors for the dark theme (foreground).
		void											DefaultDarkTheme_Fg() {
#undef MX_COLOR

#define MX_COLOR( NAME, TEXTNAME, FG, BG )																											\
	cr ## NAME = NAME ## Default_Fg();
#include "MXHexEditorColorImpl.h"
		}

		// Sets the default colors for the dark theme (background).
		void											DefaultDarkTheme_Bg() {
#undef MX_COLOR

#define MX_COLOR( NAME, TEXTNAME, FG, BG )																											\
	cr ## NAME = NAME ## Default_Bg();
#include "MXHexEditorColorImpl.h"
		}
	};

}	// namespace mx

#undef MX_COLOR

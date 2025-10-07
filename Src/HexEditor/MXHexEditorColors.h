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

	//	COLORREF									crText = MX_RGBA( 0x92, 0x92, 0x92, 0xFF );				// Text color.
	//	COLORREF									crAddSepLine = MX_RGBA( 0x42, 0x42, 0x42, 0x00 );		// Address separator line.
	//	COLORREF									crRulerText = MX_RGBA( 0x57, 0x57, 0x57, 0xFF );		// Ruler text color.
	//	COLORREF									crRulerLine = MX_RGBA( 0x57, 0x57, 0x57, 0x00 );		// Ruler line.
	//	COLORREF									crRulerMarker = MX_RGBA( 0x81, 0x81, 0x81, 0xFF );		// Ruler marker color.
	//	COLORREF									crEditorBk = MX_RGBA( 0x23, 0x22, 0x20, 0xFF );			// Background color for the whole editor.
	};

}	// namespace mx

#undef MX_COLOR

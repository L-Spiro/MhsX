#pragma once

#include "../MXMhsX.h"

#include <vector>

namespace mx {

	class CMiniMap {
	public :
		// == Enumerations.
		/** Colors. */
		enum MX_COLORS {
			MX_C_NONE,
			MX_C_ASCII,
			MX_C_RAINBOW,
			MX_C_HEAT,
			MX_C_OCEAN,
			MX_C_GREY,
			MX_C_CUSTOM
		};

		
		// == Types.
		/** The mini-map data. */
		struct MX_MINI_MAP {
			MX_COLORS								cColors = MX_C_NONE;				/**< Color palette. */
			int32_t									i32Brightness = 0;					/**< Brightness (-100..100). */
			uint32_t								ui32Contrast = 50;					/**< Contrast (0..100). */
			uint32_t								ui32Zoom = 2;						/**< Pixel size. */
			uint32_t								ui32SnapLines = 16;					/**< Snap granularity (UI edit, default 16). */
			COLORREF								crCusLow = RGB( 0, 0, 0 );			/**< The custom-color low end. */
			COLORREF								crCusHi = RGB( 0xFF, 0xFF, 0xFF );	/**< The custom-color low end. */
			bool									bAutoSnap = false;					/**< Auto Snap. */
			bool									bRightSize = true;					/**< Set to the right side of the screen (drag bar is on the left).  If false, the drag bar is on the right. */
			bool									bHighlightStructs = true;			/**< Highlight structures from templates in the Mini Map. */
		};

		/** The mini-map state. */
		struct MX_MINI_MAP_STATE {
			std::vector<COLORREF>					vBuffer;							/**< The draw buffer. */
			uint64_t								ui64Address = 0;					/**< The start address. */
			uint64_t								ui64End = 0;						/**< The end address. */
		};


		// == Functions.
		/**
		 * Draws the mini-map.
		 * 
		 * \param _hDc The device context to which to draw the mini-map.
		 * \param _miSettings The mini-map parameters.
		 **/
		//void										DrawMiniMap( HDC _hDc, const MX_MINIMAP_INPUTS &_miSettings, uint32_t /*_ui32Flags*/ );

		

		static const COLORREF						m_crRainbow[16*16];					/**< Rainbow color scheme. */
		static const COLORREF						m_crAscii[16*16];					/**< ASCII color scheme. */
	};

}	//namespace mx

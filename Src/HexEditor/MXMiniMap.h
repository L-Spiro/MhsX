#pragma once

#include "../MXMhsX.h"

#include <vector>

namespace mx {

	// == Types.
	/** Mini-map immutable inputs per frame. */
	struct MX_MINIMAP_INPUTS {
		uint64_t								ui64TotalBytes;					/**< Total file size in bytes. */
		uint32_t								ui32BytesPerRow;				/**< CurStyle()->uiBytesPerRow. */
		uint64_t								ui64FirstVisibleLine;			/**< m_sdScrollView[m_eaEditAs].ui64FirstVisibleLine. */
		int32_t									i32PageLines;					/**< m_sdScrollView[m_eaEditAs].i32PageLines (>=1). */
		//uint32_t								ui32ScreenSize;					/**< Screen height. */
		// Widget geometry (device/screen pixels).
		int32_t									i32X;
		int32_t									i32Y;
		int32_t									i32W;
		int32_t									i32H;
		// Options.
		bool									bAutoSnap;						/**< Auto Snap. */
		uint32_t								ui32SnapLines;					/**< Snap granularity (UI edit, default 16). */
	};

	/** The mini-map data. */
	struct MX_MINI_MAP {
		// == Members.
		std::vector<COLORREF>					vBuffer;						/**< The draw buffer. */
	};

	class CMiniMap {
	public :
		// == Enumerations.
		/** Render flags. */

		


		// == Functions.
		/**
		 * Draws the mini-map.
		 * 
		 * \param _hDc The device context to which to draw the mini-map.
		 * \param _miSettings The mini-map parameters.
		 **/
		//void										DrawMiniMap( HDC _hDc, const MX_MINIMAP_INPUTS &_miSettings, uint32_t /*_ui32Flags*/ );

		

		static COLORREF								m_crRainbow[16*16];			/**< Rainbow color scheme. */
		static COLORREF								m_crAscii[16*16];			/**< ASCII color scheme. */
	};

}	//namespace mx

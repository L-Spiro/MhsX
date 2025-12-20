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
			MX_C_CUBEHELIX,

			MX_C_VIRIDIS,
			MX_C_PLASMA,
			MX_C_MAGMA,
			MX_C_INFERNO,
			MX_C_CIVIDIS,
			MX_C_TURBO,
			MX_C_COOLWARM,
			MX_C_BLUEWHITERED,
			MX_C_TWILIGHT,
			MX_C_TWILIGHTSHIFTED,
			MX_C_CUSTOM = 100,
		};

		
		// == Types.
		/** The mini-map data. */
		struct MX_MINI_MAP {
			MX_COLORS								cColors = MX_C_NONE;				/**< Color palette. */
			int32_t									i32Brightness = 0;					/**< Brightness (-100..100). */
			uint32_t								ui32Contrast = 50;					/**< Contrast (0..100). */
			uint32_t								ui32Zoom = 4;						/**< Pixel size. */
			uint32_t								ui32BytesPerRow = 16;				/**< Mini-map bytes per line. */
			uint32_t								ui32SnapLines = 16;					/**< Snap granularity (UI edit, default 16). */
			COLORREF								crCusLow = RGB( 0, 0, 0 );			/**< The custom-color low end. */
			COLORREF								crCusHi = RGB( 0xFF, 0xFF, 0xFF );	/**< The custom-color low end. */
			bool									bAutoSnap = true;					/**< Auto Snap. */
			bool									bRightSize = true;					/**< Set to the right side of the screen (drag bar is on the left).  If false, the drag bar is on the right. */
			bool									bHighlightStructs = true;			/**< Highlight structures from templates in the Mini Map. */

			uint8_t									ui8Lo = 0;							/**< The low value when the color mode is MX_C_NONE. */
			uint8_t									ui8Hi = 0;							/**< The high value when the color mode is MX_C_NONE. */
		};

		/** The mini-map state. */
		struct MX_MINI_MAP_STATE {
			std::vector<COLORREF>					vBuffer;							/**< The draw buffer. */
			uint64_t								ui64Address = 0;					/**< The start address. */
			uint64_t								ui64End = 0;						/**< The end address. */
			bool									bHighlight = false;					/**< If true, draw the Mini-Map with a highlight (indicates the mouse is over the Mini-Map). */
		};


		// == Functions.
		/**
		 * Adjusts an 8-bit value by the given brightness amount.
		 *
		 * \param _i32Brightness Brightness in [-100..100].
		 * \param _uValue Input value in [0..255].
		 * \return Returns the brightness-adjusted value in [0..255].
		 */
		static inline uint8_t						BrightnessValue( int32_t _i32Brightness, uint8_t _uValue ) {
			if ( _i32Brightness < -100 ) { _i32Brightness = -100; }
			else if ( _i32Brightness > 100 ) { _i32Brightness = 100; }

			const int32_t i32X = _uValue;

			if ( _i32Brightness == 0 ) { return _uValue; }

			if ( _i32Brightness < 0 ) {
				// y = x * (100 + B) / 100.
				const int32_t i32Scale = 100 + _i32Brightness;								// 0..99
				const int32_t i32Y = (i32X * i32Scale + 50) / 100;							// Rounded.
				return static_cast<uint8_t>( i32Y );
			}
			else {
				// y = x + (255 - x) * B / 100.
				const int32_t i32Y = i32X + (((255 - i32X) * _i32Brightness + 50) / 100);	// Rounded.
				return static_cast<uint8_t>( i32Y );
			}
		}

		/**
		 * Computes the low/high grayscale endpoints implied by the Mini-Map Contrast setting,
		 * using 0x49 as the midpoint.
		 *
		 * \param _ui32Contrast Contrast in [0..100].
		 * \param _ui8Lo Holds the returned low endpoint.
		 * \param _ui8Hi Holds the returned high endpoint.
		 */
		static inline void							ContrastEndpoints( uint32_t _ui32Contrast, uint8_t &_ui8Lo, uint8_t &_ui8Hi ) {
			int32_t i32C = static_cast<int32_t>(std::min<uint32_t>( _ui32Contrast, 100 ));

			const int32_t i32M = 0x49;
			const int32_t i32LoDec = (i32M * i32C + 50) / 100;
			const int32_t i32HiInc = ((0xFF - i32M) * i32C + 50) / 100;

			int32_t i32Lo = i32M - i32LoDec;
			int32_t i32Hi = i32M + i32HiInc;

			if ( i32Lo < 0 ) { i32Lo = 0; }
			if ( i32Hi > 0xFF ) { i32Hi = 0xFF; }

			_ui8Lo = static_cast<uint8_t>(i32Lo);
			_ui8Hi = static_cast<uint8_t>(i32Hi);
		}

		/**
		 * Interpolates between the contrast endpoints using an input byte.
		 *
		 * \param _uValue The input byte in [0..255].
		 * \param _ui8Lo The low endpoint (contrast low).
		 * \param _ui8Hi The high endpoint (contrast high).
		 * \return Returns the interpolated value in [0..255].
		 */
		static inline uint8_t						ApplyContrast( uint8_t _uValue, uint8_t _ui8Lo, uint8_t _ui8Hi ) {
			const int32_t i32Lo = _ui8Lo;
			const int32_t i32Hi = _ui8Hi;
			const int32_t i32Range = i32Hi - i32Lo;

			// out = lo + (range * value) / 255, rounded.
			const int32_t i32Out = i32Lo + ((i32Range * static_cast<int32_t>(_uValue) + 127) / 255);

			if ( i32Out < 0 ) { return 0; }
			if ( i32Out > 255 ) { return 255; }
			return static_cast<uint8_t>(i32Out);
		}

		/**
		 * \brief Looks up a color in a 16-step palette using 8-bit interpolation.
		 * 
		 * \param _pcrPalette The 16-entry palette.
		 * \param _ui8T The interpolation value in [0..255].
		 * \return Returns the interpolated COLORREF.
		 */
		static inline COLORREF						Palette16( const COLORREF * _pcrPalette, uint8_t _ui8T ) {
			const uint32_t ui32Pos = static_cast<uint32_t>(_ui8T) * 15U;
			const uint32_t ui32Idx = ui32Pos / 255U;							// 0..15.
			const uint32_t ui32Frac = ui32Pos - (ui32Idx * 255U);				// 0..254.

			const uint32_t ui32Idx1 = (ui32Idx < 15U) ? (ui32Idx + 1U) : 15U;

			const COLORREF cr0 = _pcrPalette[ui32Idx];
			const COLORREF cr1 = _pcrPalette[ui32Idx1];

			const int32_t i32R0 = GetRValue( cr0 );
			const int32_t i32G0 = GetGValue( cr0 );
			const int32_t i32B0 = GetBValue( cr0 );

			const int32_t i32R1 = GetRValue( cr1 );
			const int32_t i32G1 = GetGValue( cr1 );
			const int32_t i32B1 = GetBValue( cr1 );

			const int32_t i32R = i32R0 + (((i32R1 - i32R0) * static_cast<int32_t>(ui32Frac) + 127) / 255);
			const int32_t i32G = i32G0 + (((i32G1 - i32G0) * static_cast<int32_t>(ui32Frac) + 127) / 255);
			const int32_t i32B = i32B0 + (((i32B1 - i32B0) * static_cast<int32_t>(ui32Frac) + 127) / 255);

			return RGB( i32R, i32G, i32B );
		}


		

		static const COLORREF						m_crRainbow[16*16];					/**< Rainbow color scheme. */
		static const COLORREF						m_crAscii[16*16];					/**< ASCII color scheme. */
		static const COLORREF						m_crCubehelix[16*16];				/**< Cubehelix color scheme. */
		static const COLORREF						m_crViridis16[16];					/**< Viridis color scheme. */
		static const COLORREF						m_crPlasma16[16];					/**< Plasma color scheme. */
		static const COLORREF						m_crMagma16[16];					/**< Magma color scheme. */
		static const COLORREF						m_crInferno16[16];					/**< Inferno color scheme. */
		static const COLORREF						m_crCividis16[16];					/**< Cividis color scheme. */
		static const COLORREF						m_crTurbo16[16];					/**< Turbo color scheme. */
		static const COLORREF						m_crCoolWarm16[16];					/**< CoolWarm color scheme. */
		static const COLORREF						m_crBlueWhiteRed16[16];				/**< BlueWhiteRed color scheme. */
		static const COLORREF						m_crTwilight16[16];					/**< Twilight color scheme. */
		static const COLORREF						m_crTwilightShifted16[16];			/**< Twilight Shifted color scheme. */
	};

}	//namespace mx

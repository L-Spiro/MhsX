#pragma once

#include <LSWWin.h>
#include <EEFloatX.h>

// ********************************************************* //
// Logic largely ripped from https://github.com/x448/float16 //
// ********************************************************* //

namespace mx {

	class CFloat16 {
	public :
		CFloat16() :
			m_uiVal( 0 ) {
		}
		CFloat16( double _dVal ) {
			/*ee::CFloatX fFloat;
			fFloat.CreateFromDouble( _dVal, 5, 11, true, true );
			uint16_t uiVal = static_cast<uint16_t>(fFloat.AsUint64());*/
			m_uiVal = DoubleToUi16( _dVal );
			/*if ( m_uiVal != uiVal ) {
				int gjhg = 0;
			}*/
		}
		CFloat16( uint16_t _uiBits ) :
			m_uiVal( _uiBits ) {
		}
		CFloat16( const uint8_t * _puiBits ) :
			m_uiVal( (*reinterpret_cast<const uint16_t *>(_puiBits))) {
		}


		// == Operators.
		// Cast to int8_t.
		operator					int8_t() const {
			return int8_t( Value() );
		}
		// Cast to int16_t.
		operator					int16_t() const {
			return int16_t( Value() );
		}
		// Cast to int32_t.
		operator					int32_t() const {
			return int32_t( Value() );
		}
		// Cast to int64_t.
		operator					int64_t() const {
			return int64_t( Value() );
		}
		// Cast to uint8_t.
		operator					uint8_t() const {
			return uint8_t( Value() );
		}
		// Cast to uint16_t.
		operator					uint16_t() const {
			return uint16_t( Value() );
		}
		// Cast to uint32_t.
		operator					uint32_t() const {
			return uint32_t( Value() );
		}
		// Cast to uint64_t.
		operator					uint64_t() const {
			return uint64_t( Value() );
		}
		// Cast to float.
		operator					float() const {
			return float( Value() );
		}
		// Cast to double.
		operator					double() const {
			return double( Value() );
		}


		// == Functions.
		// Gets the value as a double.
		double						Value() const {
			union {
				float				fFloat;
				uint32_t			uiInt;
			} uTmp;

			uint32_t uiSign = (m_uiVal & 0x8000) << 16;
			uint32_t uiExp = (m_uiVal & 0x7C00) >> 10;
			uint32_t uiCoef = (m_uiVal & 0x03FF) << 13;
			do {
				if ( uiExp == 0x1F ) {
					uTmp.uiInt = uiCoef == 0 ?
						(uiSign | 0x7F800000 | uiCoef) :
						(uiSign | 0x7FC00000 | uiCoef);
					break;
				}
				if ( uiExp == 0 ) {
					if ( !uiCoef ) {
						uTmp.uiInt = uiSign;
						break;
					}
					++uiExp;
					while ( (uiCoef & 0x7F800000) == 0 ) {
						uiCoef <<= 1;
						--uiExp;
					}
					uiCoef &= 0x007FFFFF;
				}

				uTmp.uiInt = uiSign | ((uiExp + (0x7F - 0xF)) << 23) | uiCoef;
			} while ( false );
			return uTmp.fFloat;
			/*ee::CFloatX fFloat;
			fFloat.CreateFromBits( m_uiVal, 5, 11, true, true );
			return fFloat.AsDouble();*/
		}

		// Gets the 16-bit int value.
		uint16_t					RawValue() const { return m_uiVal; }

		// Gets the max value.
		static double				Max() {
			static double dMax = ee::CFloatX::GetMaxForBits( 5, 11, true );
			return dMax;
		}

		// Converts from a double to a 16-bit integer containing the bits for the 16-bit float.
		static uint16_t				DoubleToUi16( double _dVal ) {
			union {
				float				fFloat;
				uint32_t			uiInt;
			} uTmp;
			uTmp.fFloat = static_cast<float>(_dVal);
			uint32_t uiSign = uTmp.uiInt & 0x80000000;
			uint32_t uiExp = uTmp.uiInt & 0x7F800000;
			uint32_t uiCoef = uTmp.uiInt & 0x007FFFFF;
			if ( uiExp == 0x7F800000 ) {
				// NaN or Inf.
				uint32_t uiNanBit = 0;
				if ( !uiCoef ) { uiNanBit = 0x0200; }
				return static_cast<uint16_t>( (uiSign >> 16) | 0x7C00 | uiNanBit | (uiCoef >> 13) );
			}
			uint32_t uiHalfSign = uiSign >> 16;
			int32_t iUnbiasedExp = (uiExp >> 23) - 127;
			int32_t iHalfExp = iUnbiasedExp + 15;

			if ( iHalfExp >= 0x1F ) { return static_cast<uint16_t>(uiHalfSign | 0x7C00); }
			if ( iHalfExp <= 0 ) {
				if ( 14 - iHalfExp > 24 ) { return static_cast<uint16_t>(uiHalfSign); }
				uint32_t uiC = uiCoef | 0x00800000;
				uint32_t uiHalfCoef = uiC >> static_cast<uint32_t>(14 - iHalfExp);
				uint32_t uiRoundBit = 1 << static_cast<uint32_t>(13 - iHalfExp);
				if ( (uiC & uiRoundBit) != 0 && (uiC & (3 * uiRoundBit - 1)) != 0 ) {
					uiHalfCoef++;
				}
				return static_cast<uint16_t>(uiHalfSign | uiHalfCoef);
			}
			uint32_t uiHalfEp = iHalfExp << 10;
			uint32_t uiHalfCoef = uiCoef >> 13;
			uint32_t uiRoundBit = 0x00001000;
			if ( (uiCoef & uiRoundBit) != 0 && (uiCoef & (3 * uiRoundBit - 1)) != 0 ) {
				return static_cast<uint16_t>((uiHalfSign | uiHalfEp | uiHalfCoef) + 1);
			}
			return static_cast<uint16_t>(uiHalfSign | uiHalfEp | uiHalfCoef);
		}


	protected :
		// == Members.
		uint16_t					m_uiVal;
	};

	

}	// namespace mx

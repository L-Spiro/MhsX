#include "EEFloatX.h"
#include <cmath>
#include <limits>
#ifdef _DEBUG
#include <cassert>
#endif	// #ifdef _DEBUG


namespace ee {

	// == Functions.
	// Creates a +InF value.
	CFloatX & CFloatX::CreateInfP( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		uiManBits = _uiManBits;
		uiExpBits = _uiExpBits;
		uiExponent = AllExpBitsSet( _uiExpBits );
		uiMantissa = 0;
		bImplicitManBit = _bImplicitMantissaBit;
		bHasSign = _bHasSign;
		bSign = false;
		return (*this);
	}

	// Creates a -InF value.
	CFloatX & CFloatX::CreateInfN( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		uiManBits = _uiManBits;
		uiExpBits = _uiExpBits;
		uiExponent = AllExpBitsSet( _uiExpBits );
		uiMantissa = 0;
		bImplicitManBit = _bImplicitMantissaBit;
		bHasSign = _bHasSign;
		bSign = true;
		return (*this);
	}

	// Creates a NaN value.
	CFloatX & CFloatX::CreateNaN( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		uiManBits = _uiManBits;
		uiExpBits = _uiExpBits;
		uiExponent = AllExpBitsSet( _uiExpBits );
		uiMantissa = 1ULL << ((_uiManBits - (_bImplicitMantissaBit ? 1 : 0)) - 1ULL);
		bImplicitManBit = _bImplicitMantissaBit;
		bHasSign = _bHasSign;
		bSign = false;
		return (*this);
	}

	// Creates a max value.
	CFloatX & CFloatX::CreateMax( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		uiManBits = _uiManBits;
		uiExpBits = _uiExpBits;
		bHasSign = _bHasSign;
		bImplicitManBit = _bImplicitMantissaBit;
		uint64_t uiRealManBits = bImplicitManBit ? uiManBits - 1 : uiManBits;
		uiMantissa = (1ULL << uiRealManBits) - 1ULL;
		uiExponent = (1ULL << uiExpBits) - 2ULL;
		bSign = false;
		return (*this);
	}

	// Creates the smallest normalized value.
	CFloatX & CFloatX::CreateMinNormalized( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		uiManBits = _uiManBits;
		uiExpBits = _uiExpBits;
		bHasSign = _bHasSign;
		bImplicitManBit = _bImplicitMantissaBit;
		uiMantissa = 0ULL;
		uiExponent = uiExpBits ? 1 : 0;
		bSign = false;
		return (*this);
	}

	// Creates the smallest non-0 value.
	CFloatX & CFloatX::CreateMin( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		uiManBits = _uiManBits;
		uiExpBits = _uiExpBits;
		bHasSign = _bHasSign;
		bImplicitManBit = _bImplicitMantissaBit;
		uint64_t uiRealManBits = bImplicitManBit ? uiManBits - 1 : uiManBits;
		uiMantissa = uiRealManBits ? 1 : 0;
		uiExponent = !uiRealManBits && uiExpBits ? 1 : 0;
		bSign = false;
		return (*this);
	}

	// Creates epsilon, the smallest value such that 1.0+X does not equal 1.
	CFloatX & CFloatX::CreateEpsilon( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		if ( _uiExpBits <= 1 ) {
			return CreateFromDouble( 0.0, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		}
		CreateFromDouble( 1.0, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		uint64_t uiVal = AsUint64() + 1;
		CreateFromBits( uiVal, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		return CreateFromDouble( AsDouble() - 1.0, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
	}

	// Creates the next-after value going up from the given number.
	CFloatX & CFloatX::CreateNextAfterUp( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		CreateFromDouble( _dVal, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		uint64_t uiVal = AsUint64SansSign();
		if ( SignBit() && uiVal ) {	// Excludes -0.
			// Signed -0 is treated as 0 below, this area means it is specifically a number below 0.
			--uiVal;
			if ( !uiVal ) {
				// It reached 0.
				return CreateFromBits( uiVal, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
			}
			// Add the sign back.
			return CreateFromBits( uiVal | (1ULL << (_uiExpBits + RealMantissaBits( _uiManBits, _bImplicitMantissaBit ))), _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		}
		else {
			// Go away from 0.
			++uiVal;
			uint64_t uiMask = (1ULL << (_uiExpBits + RealMantissaBits( _uiManBits, _bImplicitMantissaBit ))) - 1ULL;
			if ( (uiVal & uiMask) != uiVal ) {
				// Overflow.
				return (*this);
			}
			return CreateFromBits( uiVal, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		}
	}

	// Creates the next-after value going down from the given number.
	CFloatX & CFloatX::CreateNextAfterDown( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		CreateFromDouble( _dVal, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		uint64_t uiVal = AsUint64SansSign();
		if ( SignBit() && uiVal ) {	// Excludes -0.
			// Signed -0 is treated as 0 below, this area means it is specifically a number below 0.
			// Go away from 0.
			++uiVal;
			uint64_t uiMask = (1ULL << (_uiExpBits + RealMantissaBits( _uiManBits, _bImplicitMantissaBit ))) - 1ULL;
			if ( (uiVal & uiMask) != uiVal ) {
				// Overflow.
				return (*this);
			}
			// Add the sign back.
			return CreateFromBits( uiVal | (1ULL << (_uiExpBits + RealMantissaBits( _uiManBits, _bImplicitMantissaBit ))), _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		}
		else {
			// If already 0, cross the threshold and add a sign.
			if ( !uiVal ) {
				++uiVal;
				return CreateFromBits( uiVal | (1ULL << (_uiExpBits + RealMantissaBits( _uiManBits, _bImplicitMantissaBit ))), _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
			}
			// Go toward 0.
			--uiVal;
			if ( !uiVal ) {
				// It reached 0.
				return CreateFromBits( uiVal, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
			}
			return CreateFromBits( uiVal, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		}
	}

	// Is this a NaN?
	bool CFloatX::IsNaN() const {
		return (uiMantissa != 0) &&
			(uiExponent == AllExpBitsSet( uiExpBits ));
	}

	// Is this +InF?
	bool CFloatX::IsInfP() const {
		return (bSign == 0) &&
			(uiMantissa == 0) &&
			(uiExponent == AllExpBitsSet( uiExpBits ));
	}

	// Is this -InF?
	bool CFloatX::IsInfN() const {
		return (bSign == 1) &&
			(uiMantissa == 0) &&
			(uiExponent == AllExpBitsSet( uiExpBits ));
	}

	// Create from a double.
	CFloatX & CFloatX::CreateFromDouble( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		uiManBits = _uiManBits;
		uiExpBits = _uiExpBits;
		bHasSign = _bHasSign;
		bImplicitManBit = _bImplicitMantissaBit;
		
		union EE_DOUBLE_INT {
			double		dVal;
			int64_t		iVal;
			uint64_t	uiVal;
		};
		uint64_t uiFinal;
		{

			// If this is just a double, skip the cast.
			if ( _uiExpBits == EE_FLOATX_DBL_EXP_BITS && _uiManBits == EE_FLOATX_DBL_MAN_BITS && _bImplicitMantissaBit && _bHasSign ) {
				EE_DOUBLE_INT diVal;
				diVal.dVal = _dVal;
				CreateFromBits( diVal.uiVal, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
				return (*this);
			}

			// Clamp to 0 if no sign bit.
			if ( !_bHasSign && _dVal <= 0.0 ) {
				uiExponent = 0;
				uiMantissa = 0;
				bSign = false;
				return (*this);
			}
			// Adaptation from:
			//	https://stackoverflow.com/questions/1659440/32-bit-to-16-bit-floating-point-conversion
			// Shared under The Unlicense (choosealicense.com/licenses/unlicense).  Public domain.
		
			const uint64_t uiRealMantissa = RealMantissaBits( _uiManBits, _bImplicitMantissaBit );
			const int64_t iShiftSign = 64 - TotalBits( _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
			//const int64_t iShift = RealMantissaBits( EE_FLOATX_DBL_MAN_BITS, true ) - uiRealMantissa;
			const int64_t iShift = (EE_FLOATX_DBL_MAN_BITS - 1) - uiRealMantissa;

			CFloatX fTemp;
			const uint64_t uiInfN = 0x7FF0000000000000ULL;											// 64-bit infinity.
			fTemp.CreateMax( _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
			double dTempVal = fTemp.AsDouble();
			const uint64_t uiMaxN = (*reinterpret_cast<const uint64_t *>(&dTempVal));				// Max X-float normal as 64-bit float.
			fTemp.CreateMinNormalized( _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
			dTempVal = fTemp.AsDouble();
			const uint64_t uiMinN = (*reinterpret_cast<const uint64_t *>(&dTempVal));				// Min X-float normal as 64-bit float.
			//const uint64_t uiSignN = (1ULL << 63);													// 64-bit sign bit.

			double dTemp = (*reinterpret_cast<const double *>(&uiMinN));
			dTempVal = (1ULL << (EE_FLOATX_DBL_MAN_BITS - 1)) / dTemp;
			if ( std::isinf( dTempVal ) ) {
				dTempVal = std::nexttoward( dTempVal, 0.0 );
			}
			const uint64_t uiMulN = (*reinterpret_cast<const uint64_t *>(&dTempVal));				// (1 << EE_FLOATX_DBL_MAN_BITS) / uiMinN
			dTempVal = dTemp / (1ULL << ((EE_FLOATX_DBL_MAN_BITS - 1) - iShift));
			//const uint64_t uiMulC = (*reinterpret_cast<const uint64_t *>(&dTempVal));				// uiMinN / (1 << (EE_FLOATX_DBL_MAN_BITS - iShift))

			const int64_t uiInfC = uiInfN >> iShift;
			const int64_t uiNanN = (uiInfC + 1) << iShift;											// Min X-float NaN as a 64-bit float.
			const int64_t uiMaxC = uiMaxN >> iShift;
			const int64_t uiMinC = uiMinN >> iShift;
			//const int64_t uiSignC = uiSignN >> iShiftSign;											// X-float sign bit.

			/*fTemp.CreateMinNormalized( 11, EE_FLOATX_DBL_MAN_BITS, true, true );
			dTempVal = fTemp.AsDouble();*/
			dTempVal = DBL_MIN;
			const uint64_t uiNorC = (*reinterpret_cast<const uint64_t *>(&dTempVal)) >> iShift;		// Min 64-bit float normal down shifted.
			const uint64_t uiSubC = uiNorC - 1;														// Max 64-bit float subnormal down shifted.
        

			const uint64_t uiMaxD = uiInfC - uiMaxC - 1;
			const uint64_t uiMinD = uiMinC - uiSubC - 1;


			EE_DOUBLE_INT diV, diS;
			diV.dVal = _dVal;
			uint64_t uiSign = SignBit( _dVal );
			diV.iVal ^= uiSign;
			uiSign >>= iShiftSign;
			diS.iVal = uiMulN;
			diS.iVal = static_cast<int64_t>(diS.dVal * diV.dVal);									// Correct subnormals.
			diV.iVal ^= (diS.iVal ^ diV.iVal) & -(static_cast<int64_t>(uiMinN) > diV.iVal);
			diV.iVal ^= (uiInfN ^ diV.iVal) & -((static_cast<int64_t>(uiInfN) > diV.iVal) & (diV.iVal > static_cast<int64_t>(uiMaxN)));
			diV.iVal ^= (uiNanN ^ diV.iVal) & -((static_cast<int64_t>(uiNanN) > diV.iVal) & (diV.iVal > static_cast<int64_t>(uiInfN)));
			diV.uiVal >>= iShift;
			diV.iVal ^= ((diV.iVal - uiMaxD) ^ diV.iVal) & -(diV.iVal > static_cast<int64_t>(uiMaxC));
			diV.iVal ^= ((diV.iVal - uiMinD) ^ diV.iVal) & -(diV.iVal > static_cast<int64_t>(uiSubC));
			uiFinal = diV.uiVal | uiSign;
		}
		
#if 0
		{
			const uint64_t uiRealMantissa = RealMantissaBits( _uiManBits, _bImplicitMantissaBit );
			const int64_t iShiftSign = 64 - TotalBits( _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
			const int64_t iShiftMan = RealMantissaBits( EE_FLOATX_DBL_MAN_BITS, true ) - uiRealMantissa;
			const int64_t iShiftExp = 52 + (11 - _uiExpBits);

			EE_DOUBLE_INT diTmp;
			diTmp.dVal = _dVal;
			uint64_t uiSign = diTmp.uiVal & 0x8000000000000000ULL;
			uint64_t uiExp = diTmp.uiVal & 0x7FF0000000000000ULL;
			uint64_t uiCoef = diTmp.uiVal & 0x000FFFFFFFFFFFFFULL;
			if ( uiExp == 0x7FF0000000000000ULL ) {
				// NaN or Inf.
				if ( uiCoef ) {
					// NaN.
					uiFinal = ((uiSign >> iShiftSign) | (uiExp >> iShiftExp) | (uiCoef >> iShiftMan)) | (1ULL << (_uiManBits - 1ULL));
				}
				else {
					// Inf.
					uiFinal = (uiSign >> iShiftSign) | (uiExp >> iShiftExp) | (uiCoef >> iShiftMan);
				}
			}
			else {
				const int64_t iMaxBias = (1ULL << _uiExpBits) - 1ULL;
				uint64_t uiShiftedSign = uiSign >> iShiftSign;
				int64_t iUnbiasedExp = (uiExp >> 52ULL) - 1023ULL;
				int64_t iNewBias = (1ULL << (_uiExpBits - 1ULL)) - 1ULL;
				int64_t iNewExp = iUnbiasedExp + iNewBias;
				if ( iNewExp >= iMaxBias ) {
					// Infinity.
					uiFinal = uiShiftedSign | (iMaxBias << uiRealMantissa);
				}
				else if ( iNewExp <= 0 ) {
					int64_t iBitDiff = 53 - uiRealMantissa;
					if ( iBitDiff - iNewExp > 53 ) {
						// Underflow to 0.
						uiFinal = uiShiftedSign;
					}
					else {
						uint64_t uiC = uiCoef | 0x0010000000000000ULL;
						uint64_t uiNewCoef = uiC >> static_cast<uint64_t>(iBitDiff - iHalfExp);
						uint64_t uiRoundBit = 1ULL << static_cast<uint64_t>((iBitDiff - 1) - iHalfExp);
						if ( (uiC & uiRoundBit) != 0 && (uiC & (3 * uiRoundBit - 1)) != 0 ) {
							uiNewCoef++;
						}
						uiFinal = uiShiftedSign | uiNewCoef;
					}
				}
			}
		}
#endif

#if 0
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
#endif
		

		/*uiExponent = (uiFinal >> uiRealMantissa) & ((1ULL << uiExpBits) - 1ULL);
		uiMantissa = uiFinal & ((1ULL << uiRealMantissa) - 1ULL);
		bSign = uiSign ? true : false;*/
		CreateFromBits( uiFinal, _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		
		/*
#ifdef _DEBUG
		double dVal = AsDouble();
		if ( uiExpBits == 8 && uiManBits == FLT_MANT_DIG && bImplicitManBit ) {
			double dTemp = AsDouble();
			float fCastVal = static_cast<float>(_dVal);
			double fVal = fCastVal;
			uint32_t ui32Val = (*reinterpret_cast<uint32_t *>(&fCastVal));
			assert( fVal == dTemp );
		}
#endif	// #ifdef _DEBUG
		*/

            //return diV.uiVal | uiSign;*/


#if 0
		if ( ::isnan( _dVal ) ) {
			return CreateNaN( _uiExpBits, _uiManBits, _bImplicitMantissaBit );
		}
		if ( ::isinf( _dVal ) ) {
			return (_dVal > 0.0) ? CreateInfP( _uiExpBits, _uiManBits, _bImplicitMantissaBit ) :
				CreateInfN( _uiExpBits, _uiManBits, _bImplicitMantissaBit );
		}
		// Input is not NaN or InF.  But is it too large for the given bits?
		double dMax = GetMaxForBits( _uiExpBits, _uiManBits, _bImplicitMantissaBit );
		if ( _dVal > dMax ) { return CreateInfP( _uiExpBits, _uiManBits, _bImplicitMantissaBit ); }
		else if ( _dVal < -dMax ) { return CreateInfN( _uiExpBits, _uiManBits, _bImplicitMantissaBit ); }
		
		uiManBits = _uiManBits;
		uiExpBits = _uiExpBits;
		bImplicitManBit = _bImplicitMantissaBit;

		uint64_t uiAsInt = (*reinterpret_cast<uint64_t *>(&_dVal));
		bSign = static_cast<int8_t>((uiAsInt >> 63ULL) & 0x1ULL);
		uint64_t uiSExp = (uiAsInt >> (EE_FLOATX_DBL_MAN_BITS - 1ULL)) & ((1ULL << 11ULL) - 1ULL);
		uint64_t uiSMan = uiAsInt & ((1ULL << (EE_FLOATX_DBL_MAN_BITS - 1ULL)) - 1ULL);


		double dExp = uiSExp ? (uiSExp - 1023.0) :
			(uiSExp - 1022.0);
		double dNewExp = dExp + ExpBias();
		double dMaxExp = MaxExpBias( uiExpBits );

		uint64_t uiNewExp = static_cast<uint64_t>(dExp + ExpBias());
		
#endif



#if 0
		double dExp = uiSExp ? (uiSExp - 1023.0) :
			(uiSExp - 1022.0);
		double dBias = -DenormalExpBias();
		uint64_t uiNewExp;
		uint64_t uiShift = uiManBits;
		if ( !uiSExp || dExp < dBias ) {
			// Denormalized number.
			//dBias = DenormalExpBias();
			//dExp = 0.0;
			uiNewExp = 0;//(dExp + DenormalExpBias());
			--uiShift;
		}
		else {
			//dBias = ExpBias();
			uiNewExp = static_cast<uint64_t>(dExp + ExpBias());
		}
		uiExponent = uiNewExp & ((1ULL << uiExpBits) - 1ULL);
		uiMantissa = (uiSMan >> (EE_FLOATX_DBL_MAN_BITS - uiShift));
		if ( uiSExp ) {
			// Rounding.
			uint64_t uiFirstMissingBit = (uiSMan >> (EE_FLOATX_DBL_MAN_BITS - uiShift - 1ULL)) & 1;
			uiMantissa += uiFirstMissingBit;
		}

		// From normal to denormal?
		if ( uiSExp && !uiExponent ) {
			//uiMantissa |= (1ULL << (uiShift -1ULL));
		}
#ifdef _DEBUG
		if ( uiExpBits == 8 && uiManBits == FLT_MANT_DIG && bImplicitManBit ) {
			double dTemp = AsDouble();
			float fCastVal = static_cast<float>(_dVal);
			double fVal = fCastVal;
			uint32_t ui32Val = (*reinterpret_cast<uint32_t *>(&fCastVal));
			assert( fVal == dTemp );
		}
#endif	// #ifdef _DEBUG
		
		// 0.0000000000000000000000000000000000000000000014012984643248171
		//FLT_TRUE_MIN;
		// 1.401298464e-45F
		//FLT_MAX
#endif	// #if 0
		return (*this);
	}

	// Create from bits in a uint64_t value.
	CFloatX & CFloatX::CreateFromBits( uint64_t _uiVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		uiManBits = _uiManBits;
		uiExpBits = _uiExpBits;
		bHasSign = _bHasSign;
		bImplicitManBit = _bImplicitMantissaBit;

		uint64_t uiRealMantissa = RealMantissaBits( _uiManBits, _bImplicitMantissaBit );
		
		uiExponent = (_uiVal >> uiRealMantissa) & ((1ULL << uiExpBits) - 1ULL);
		uiMantissa = _uiVal & ((1ULL << uiRealMantissa) - 1ULL);
		bSign = (_bHasSign && (_uiVal & (1ULL << (TotalBits( _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign ) - 1ULL)))) ? true : false;
		return (*this);
	}

	// Create from separate values.
	CFloatX & CFloatX::CreateFromParts( uint64_t _uiSign, uint64_t _uiExp, uint64_t _uiMan,
		uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
		uiManBits = _uiManBits;
		uiExpBits = _uiExpBits;
		bHasSign = _bHasSign;
		bImplicitManBit = _bImplicitMantissaBit;

		uint64_t uiRealMantissa = RealMantissaBits( _uiManBits, _bImplicitMantissaBit );

		uiExponent = _uiExp & ((1ULL << uiExpBits) - 1ULL);
		uiMantissa = _uiMan & ((1ULL << uiRealMantissa) - 1ULL);
		bSign = (_bHasSign && _uiSign) ? true : false;
		return (*this);
	}

	// Cast to double.
	double CFloatX::AsDouble() const {
		/*if ( uiExpBits == EE_FLOATX_DBL_EXP_BITS && uiManBits == EE_FLOATX_DBL_MAN_BITS + 1 && bImplicitManBit && bHasSign ) {
			union EE_DOUBLE_INT {
				double		dVal;
				int64_t		iVal;
				uint64_t	uiVal;
			};
			EE_DOUBLE_INT diVal;
			diVal.uiVal = AsUint64();
			return diVal.dVal;
		}*/

		if ( IsNaN() ) { return std::numeric_limits<double>::quiet_NaN(); }
		if ( IsInfP() ) { return std::numeric_limits<double>::infinity(); }
		if ( IsInfN() ) { return -std::numeric_limits<double>::infinity(); }
		uint64_t uiManBitsTemp = RealMantissaBits( uiManBits, bImplicitManBit );
		double dManPow = std::pow( 2.0, uiManBitsTemp );
		double dSig = uiMantissa / dManPow;

		if ( IsDenormalized() ) {
			return Sign() * std::pow( 2.0, -DenormalExpBias() ) * dSig;
		}
		return Sign() * std::pow( 2.0, uiExponent - ExpBias() ) * (1.0 + dSig);
	}

	// Put all the bits together into a uint64_t value.
	uint64_t CFloatX::AsUint64() const {
		uint64_t uiVal = ((bHasSign && bSign) ? 1ULL : 0ULL);
		uiVal <<= uiExpBits;
		uiVal |= uiExponent & ((1ULL << uiExpBits) - 1ULL);
		uint64_t uiRealManBits = RealMantissaBits( uiManBits, bImplicitManBit );
		uiVal <<= uiRealManBits;
		uiVal |= uiMantissa & ((1ULL << uiRealManBits) - 1ULL);
		return uiVal;
	}

	// Put all the bits together into a uint64_t value except for the sign value.
	uint64_t CFloatX::AsUint64SansSign() const {
		uint64_t uiVal = 0ULL;
		uiVal |= uiExponent & ((1ULL << uiExpBits) - 1ULL);
		uint64_t uiRealManBits = RealMantissaBits( uiManBits, bImplicitManBit );
		uiVal <<= uiRealManBits;
		uiVal |= uiMantissa & ((1ULL << uiRealManBits) - 1ULL);
		return uiVal;
	}

	// Sets the sign bit, if applicable.
	CFloatX & CFloatX::SetSign( bool _bEnabled ) {
		if ( bHasSign ) {
			bSign = _bEnabled != false ? true : false;
		}
		return (*this);
	}

	// Sets or unsets a bit in the exponent.
	CFloatX & CFloatX::SetExpBit( bool _bEnabled, uint16_t _uiBit ) {
		if ( _uiBit < uiExpBits ) {
			if ( _bEnabled ) {
				uiExponent |= (1ULL << _uiBit);
			}
			else {
				uiExponent ^= ~(1ULL << _uiBit);
			}
		}
		return (*this);
	}

	// Sets or unsets a bit in the mantissa.
	CFloatX & CFloatX::SetManBit( bool _bEnabled, uint16_t _uiBit ) {
		if ( _uiBit < uiManBits ) {
			if ( _bEnabled ) {
				uiMantissa |= (1ULL << _uiBit);
			}
			else {
				uiMantissa ^= ~(1ULL << _uiBit);
			}
		}
		return (*this);
	}

	// Gets the maximum possible value for a float type with the given bits.
	double CFloatX::GetMaxForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit ) {
		uint64_t uiMaxExp = AllExpBitsSet( _uiExpBits ) - 1;
		double dExpBias = ExpBias( _uiExpBits );
		double dMaxExp = std::pow( 2.0, uiMaxExp - dExpBias );
		double dMan = _bImplicitMantissaBit ? 1.0 : 0.0;
		if ( _bImplicitMantissaBit ) { --_uiManBits; }
		double dManPow = std::pow( 2.0, _uiManBits );
		double dAdd = (dManPow - 1.0) / dManPow;
		dMan += dAdd;
		/*double dMultiplier = 0.5;
		for ( uint16_t I = _uiManBits; I--; dMultiplier /= 2.0 ) {
			dMan += dMultiplier;
		}*/
		return dMaxExp * dMan;
	}

	// Gets the smallest possible non-0 value for a float type with the given bits.
	double CFloatX::GetMinForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit ) {
		double dExpBias = ExpBias( _uiExpBits ) - 1.0;	// Denormalized exponent.
		double dMaxExp = std::pow( 2.0, -dExpBias );
		if ( _bImplicitMantissaBit ) { --_uiManBits; }
		double dManPow = std::pow( 2.0, _uiManBits );
		// Mantissa is just the value 1.
		double dMan = 1.0 / dManPow;
		// No implicit mantissa "1".
		return dMaxExp * dMan;
	}

	// Gets the smallest normalized non-0 value for a float type with the given bits.
	double CFloatX::GetNormalizedMinForBits( uint16_t _uiExpBits, uint16_t /*_uiManBits*/, bool /*_bImplicitMantissaBit*/ ) {
		double dExpBias = ExpBias( _uiExpBits );		// Normalized exponent.
		double dMaxExp = std::pow( 2.0, 1.0 - dExpBias );
		return dMaxExp;
	}

	// Gets the maximum denormalized value for a float type with the given bits.
	double CFloatX::GetDenormalizedMaxForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit ) {
		double dExpBias = ExpBias( _uiExpBits ) - 1.0;	// Denormalized exponent.
		double dMaxExp = std::pow( 2.0, -dExpBias );
		if ( _bImplicitMantissaBit ) { --_uiManBits; }
		double dManPow = std::pow( 2.0, _uiManBits );
		double dMan = (dManPow - 1.0) / dManPow;
		/*double dMultiplier = 0.5;
		for ( uint16_t I = _uiManBits; I--; dMultiplier /= 2.0 ) {
			dMan += dMultiplier;
		}*/
		return dMaxExp * dMan;
	}

}	// namespace ee
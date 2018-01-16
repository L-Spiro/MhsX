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
		uiExponent = (1 << _uiExpBits) - 1;
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
		uiMantissa = 1;
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

		// Clamp to 0 if no sign bit.
		if ( !_bHasSign && _dVal <= 0.0 ) {
			uiExponent = 0;
			uiMantissa = 0;
			bSign = true;
			return (*this);
		}
		// Adaptation from:
		//	https://stackoverflow.com/questions/1659440/32-bit-to-16-bit-floating-point-conversion
		// Shared under The Unlicense (choosealicense.com/licenses/unlicense).  Public domain.
		union EE_DOUBLE_INT {
			double		dVal;
			int64_t		iVal;
			uint64_t	uiVal;
		};
		const uint64_t uiRealMantissa = RealMantissaBits( _uiManBits, _bImplicitMantissaBit );
		const int64_t iShiftSign = 64 - TotalBits( _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		const int64_t iShift = RealMantissaBits( DBL_MANT_DIG, true ) - uiRealMantissa;

		CFloatX fTemp;
		const uint64_t uiInfN = 0x7FF0000000000000ULL;											// 64-bit infinity.
		fTemp.CreateMax( _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		double dTempVal = fTemp.AsDouble();
        const uint64_t uiMaxN = (*reinterpret_cast<const uint64_t *>(&dTempVal));				// Max X-float normal as 64-bit float.
		fTemp.CreateMinNormalized( _uiExpBits, _uiManBits, _bImplicitMantissaBit, _bHasSign );
		dTempVal = fTemp.AsDouble();
        const uint64_t uiMinN = (*reinterpret_cast<const uint64_t *>(&dTempVal));				// Min X-float normal as 64-bit float.
        const uint64_t uiSignN = (1ULL << 63);													// 64-bit sign bit.

		double dTemp = (*reinterpret_cast<const double *>(&uiMinN));
		dTempVal = (1ULL << DBL_MANT_DIG) / dTemp;
		const uint64_t uiMulN = (*reinterpret_cast<const uint64_t *>(&dTempVal));				// (1 << DBL_MANT_DIG) / uiMinN
		dTempVal = dTemp / (1ULL << (DBL_MANT_DIG - iShift));
        const uint64_t uiMulC = (*reinterpret_cast<const uint64_t *>(&dTempVal));				// uiMinN / (1 << (DBL_MANT_DIG - iShift))

		const int64_t uiInfC = uiInfN >> iShift;
        const int64_t uiNanN = (uiInfC + 1) << iShift;											// Min X-float NaN as a 64-bit float.
        const int64_t uiMaxC = uiMaxN >> iShift;
        const int64_t uiMinC = uiMinN >> iShift;
        const int64_t uiSignC = uiSignN >> iShiftSign;											// X-float sign bit.

		fTemp.CreateMinNormalized( 11, DBL_MANT_DIG, true, true );
		dTempVal = fTemp.AsDouble();
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
        diV.iVal ^= (diS.iVal ^ diV.iVal) & -(uiMinN > diV.iVal);
        diV.iVal ^= (uiInfN ^ diV.iVal) & -((uiInfN > diV.iVal) & (diV.iVal > uiMaxN));
        diV.iVal ^= (uiNanN ^ diV.iVal) & -((uiNanN > diV.iVal) & (diV.iVal > uiInfN));
        diV.uiVal >>= iShift;
		diV.iVal ^= ((diV.iVal - uiMaxD) ^ diV.iVal) & -(diV.iVal > uiMaxC);
        diV.iVal ^= ((diV.iVal - uiMinD) ^ diV.iVal) & -(diV.iVal > uiSubC);
		uint64_t uiFinal = diV.uiVal | uiSign;
		

		

		uiExponent = (uiFinal >> uiRealMantissa) & ((1ULL << uiExpBits) - 1ULL);
		uiMantissa = uiFinal & ((1ULL << uiRealMantissa) - 1ULL);
		bSign = uiSign ? true : false;
		

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
		uint64_t uiSExp = (uiAsInt >> (DBL_MANT_DIG - 1ULL)) & ((1ULL << 11ULL) - 1ULL);
		uint64_t uiSMan = uiAsInt & ((1ULL << (DBL_MANT_DIG - 1ULL)) - 1ULL);


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
		uiMantissa = (uiSMan >> (DBL_MANT_DIG - uiShift));
		if ( uiSExp ) {
			// Rounding.
			uint64_t uiFirstMissingBit = (uiSMan >> (DBL_MANT_DIG - uiShift - 1ULL)) & 1;
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

	// Cast to double.
	double CFloatX::AsDouble() const {
		if ( IsNaN() ) { return std::numeric_limits<double>::quiet_NaN(); }
		if ( IsInfP() ) { return std::numeric_limits<double>::infinity(); }
		if ( IsInfN() ) { return -std::numeric_limits<double>::infinity(); }
		uint64_t uiManBitsTemp = uiManBits - 1;
		double dManPow = ::pow( 2.0, uiManBitsTemp );
		double dSig = uiMantissa / dManPow;

		if ( IsDenormalized() ) {
			return Sign() * ::pow( 2.0, -DenormalExpBias() ) * dSig;
		}
		return Sign() * ::pow( 2.0, uiExponent - ExpBias() ) * (1.0 + dSig);
	}

	// Put all the bits together into a uint64_t value.
	uint64_t CFloatX::AsUint64() const {
		uint64_t uiVal = ((bHasSign && bSign) ? 1 : 0);
		uiVal <<= uiExpBits;
		uiVal |= uiExponent & ((1ULL << uiExpBits) - 1ULL);
		uint64_t uiRealManBits = bImplicitManBit ? uiManBits - 1 : uiManBits;
		uiVal <<= uiRealManBits;
		uiVal |= uiMantissa & ((1ULL << uiRealManBits) - 1ULL);
		return uiVal;
	}

	// Gets the maximum possible value for a float type with the given bits.
	double CFloatX::GetMaxForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit ) {
		uint64_t uiMaxExp = AllExpBitsSet( _uiExpBits ) - 1;
		double dExpBias = ExpBias( _uiExpBits );
		double dMaxExp = ::pow( 2.0, uiMaxExp - dExpBias );
		double dMan = _bImplicitMantissaBit ? 1.0 : 0.0;
		if ( _bImplicitMantissaBit ) { --_uiManBits; }
		double dManPow = ::pow( 2.0, _uiManBits );
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
		double dMaxExp = ::pow( 2.0, -dExpBias );
		if ( _bImplicitMantissaBit ) { --_uiManBits; }
		double dManPow = ::pow( 2.0, _uiManBits );
		// Mantissa is just the value 1.
		double dMan = 1.0 / dManPow;
		// No implicit mantissa "1".
		return dMaxExp * dMan;
	}

}	// namespace ee
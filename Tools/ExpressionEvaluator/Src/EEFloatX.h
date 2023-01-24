#pragma once

#include <cfloat>
#include <cmath>
#include <cstdint>

#define EE_FLOATX_DBL_EXP_BITS			11
#define EE_FLOATX_DBL_MAN_BITS			DBL_MANT_DIG
#define EE_FLOATX_FLT_EXP_BITS			8
#define EE_FLOATX_FLT_MAN_BITS			FLT_MANT_DIG
#define EE_FLOAT16_EPSILON				0.0009765625f

namespace ee {

	// A floating-point value with any number of bits, separated into a sign bit, X exponent bits, and Y mantissa bits.
	class CFloatX {
	public :
		CFloatX() :
			uiExponent( 0 ),
			uiMantissa( 0 ),
			uiManBits( 0 ),
			uiExpBits( 0 ),
			bSign( false ),
			bImplicitManBit( true ),
			bHasSign( false ) {
		}
		uint64_t						uiExponent;
		uint64_t						uiMantissa;
		uint16_t						uiExpBits;
		uint16_t						uiManBits;
		bool							bSign;
		bool							bImplicitManBit;
		bool							bHasSign;


		// == Functions.
		// Creates a +InF value.
		CFloatX &						CreateInfP( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		// Creates a -InF value.
		CFloatX &						CreateInfN( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		// Creates a NaN value.
		CFloatX &						CreateNaN( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		// Creates a max value.
		CFloatX &						CreateMax( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		// Creates the smallest normalized value.
		CFloatX &						CreateMinNormalized( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		// Creates the smallest non-0 value.
		CFloatX &						CreateMin( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		// Creates epsilon, the smallest value such that 1.0+X does not equal 1.
		CFloatX &						CreateEpsilon( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		// Creates the next-after value going up from the given number.
		CFloatX &						CreateNextAfterUp( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		// Creates the next-after value going down from the given number.
		CFloatX &						CreateNextAfterDown( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		// Is this a NaN?
		bool							IsNaN() const;

		// Is this +InF?
		bool							IsInfP() const;

		// Is this -InF?
		bool							IsInfN() const;

		// Create from a double.
		CFloatX &						CreateFromDouble( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit = true, bool _bHasSign = true );

		// Create from bits in a uint64_t value.
		CFloatX &						CreateFromBits( uint64_t _uiVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit = true, bool _bHasSign = true );

		// Create from separate values.
		CFloatX &						CreateFromParts( uint64_t _uiSign, uint64_t _uiExp, uint64_t _uiMan,
			uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit = true, bool _bHasSign = true );

		// Cast to double.
		double							AsDouble() const;

		// Put all the bits together into a uint64_t value.
		uint64_t						AsUint64() const;

		// Gets the exponent with all bits set.
		uint64_t						AllExpBitsSet() const { return AllExpBitsSet( uiExpBits ); }

		// Put all the bits together into a uint64_t value except for the sign value.
		uint64_t						AsUint64SansSign() const;

		// Gets the exponent bias.
		double							ExpBias() const { return ExpBias( uiExpBits ); }

		// Gets the denormalized exponent bias.
		double							DenormalExpBias() const { return ExpBias() - 1; }

		// Determines if the type is denormalized or not.
		bool							IsDenormalized() const { return uiExponent == 0; }

		// Gets a sign multiplier.
		double							Sign() const { return ((bHasSign && bSign) ? -1.0 : 1.0); }

		// Gets the sign bit shifted.
		uint64_t						SignBit() const { return ((bHasSign && bSign) ? 1 : 0); }

		// Gets the exponent bits shifted.
		uint64_t						ExpBits() const { return uiExponent & ((1ULL << uiExpBits) - 1ULL); }

		// Gets the mantissa bits shifted.
		uint64_t						ManBits() const { return uiMantissa & ((1ULL << RealMantissaBits( uiManBits, bImplicitManBit )) - 1ULL); }

		// Sets the sign bit, if applicable.
		CFloatX &						SetSign( bool _bEnabled );

		// Sets or unsets a bit in the exponent.
		CFloatX &						SetExpBit( bool _bEnabled, uint16_t _uiBit );

		// Sets or unsets a bit in the mantissa.
		CFloatX &						SetManBit( bool _bEnabled, uint16_t _uiBit );

		// Gets the maximum possible value for a float type with the given bits.
		static double					GetMaxForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		// Gets the smallest possible non-0 value for a float type with the given bits.
		static double					GetMinForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		// Gets the smallest normalized non-0 value for a float type with the given bits.
		static double					GetNormalizedMinForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		// Gets the maximum denormalized value for a float type with the given bits.
		static double					GetDenormalizedMaxForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		// Gets an exponent with all bits set.
		static uint64_t					AllExpBitsSet( uint16_t _uiExpBits ) { return (1 << _uiExpBits) - 1; }

		// Gets the exponent bias.
		static const double				ExpBias( uint16_t _uiExpBits ) { return (1 << (_uiExpBits - 1)) - 1; }

		// Gets the maximum normalized exponent bias for a given number of bits.
		static const double				MaxExpBias( uint16_t _uiExpBits ) { return (1 << (_uiExpBits - 1)) - 1; }

		// Gets the minimum normalized exponent bias for a given number of bits.
		static const double				MinExpBias( uint16_t _uiExpBits ) { return (1 << (_uiExpBits - 1)) - 1; }

		// Gets the maximum number of bits in a sign.
		static const uint32_t			MaxSignBits() { return 1; }

		// Gets the maximum number of bits in an exponent.
		static const uint32_t			MaxExpBits() { return 11; }

		// Gets the maximum number of bits in a mantissa.
		static const uint32_t			MaxManBits() { return EE_FLOATX_DBL_MAN_BITS; }

		// Gets the real number of mantissa bits based on whether or not there is an implicit bit.
		static const uint64_t			RealMantissaBits( uint16_t _uiManBits, bool _bImplicitMantissaBit ) { return _bImplicitMantissaBit ? _uiManBits - 1 : _uiManBits; }

		// Gets the sign bit from a double (not shifted).
		static uint64_t					SignBit( double _dVal ) { return (*reinterpret_cast<uint64_t *>(&_dVal)) & (1ULL << 63ULL); }

		// Gets the exponent from a double (not shifted).
		static uint64_t					ExpBits( double _dVal ) { return (*reinterpret_cast<uint64_t *>(&_dVal)) & (((1ULL << 11ULL) - 1ULL) << (EE_FLOATX_DBL_MAN_BITS - 1ULL)); }

		// Gets the mantissa from a double (not shifted).
		static uint64_t					ManBits( double _dVal ) { return (*reinterpret_cast<uint64_t *>(&_dVal)) & (((1ULL << (EE_FLOATX_DBL_MAN_BITS - 2ULL)) - 1ULL)); }

		// Gets the number of bits in a given representation.
		static uint64_t					TotalBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
			return (_uiExpBits + _uiManBits) + (_bHasSign ? 1 : 0) - (_bImplicitMantissaBit ? 1 : 0);
		}

		// Gets the precision given the number of mantissa bits for numbers greater than 1.
		static double					Precision( uint16_t _uiManBits ) {
			return _uiManBits > 1 ? std::log10( std::pow( 2.0, _uiManBits ) ) :
				(std::log10( _uiManBits ) + 1.0);
		}
	};

}	// namespace ee
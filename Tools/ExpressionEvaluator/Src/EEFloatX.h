#pragma once

#include <cfloat>
#define _USE_MATH_DEFINES
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
		/**
		 * \brief Creates a +infinity value for the specified floating-point bit layout.
		 *
		 * Initializes this instance to represent positive infinity using the provided exponent/mantissa bit sizes
		 * and encoding options (implicit mantissa bit and presence of a sign bit).
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateInfP( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * \brief Creates a -infinity value for the specified floating-point bit layout.
		 *
		 * Initializes this instance to represent negative infinity using the provided exponent/mantissa bit sizes
		 * and encoding options (implicit mantissa bit and presence of a sign bit).
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateInfN( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * \brief Creates a NaN value for the specified floating-point bit layout.
		 *
		 * Initializes this instance to represent a quiet NaN using the provided exponent/mantissa bit sizes
		 * and encoding options. The exact NaN payload/bit pattern is implementation-defined within this class.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateNaN( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * \brief Creates the maximum finite value for the specified floating-point bit layout.
		 *
		 * Initializes this instance to the largest representable non-infinite value for the provided format.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateMax( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * \brief Creates the smallest positive normalized value for the specified floating-point bit layout.
		 *
		 * Initializes this instance to the minimum representable positive normalized value (i.e. the smallest
		 * value with a non-zero exponent and a zero mantissa, using the format's rules).
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateMinNormalized( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * \brief Creates the smallest positive non-zero value for the specified floating-point bit layout.
		 *
		 * Initializes this instance to the minimum representable positive value, including denormal/subnormal
		 * values if the format supports them.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateMin( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * \brief Creates epsilon for the specified floating-point bit layout.
		 *
		 * Epsilon is the smallest value X such that (1.0 + X) != 1.0 when represented in the specified format.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateEpsilon( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * \brief Creates the next representable value above the given number for the specified bit layout.
		 *
		 * Interprets the input as a real value, converts it to the specified format, and then advances to the
		 * next representable value greater than that representation.
		 *
		 * \param _dVal Input value.
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateNextAfterUp( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * \brief Creates the next representable value below the given number for the specified bit layout.
		 *
		 * Interprets the input as a real value, converts it to the specified format, and then steps to the
		 * next representable value less than that representation.
		 *
		 * \param _dVal Input value.
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateNextAfterDown( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * \brief Determines whether this value is a NaN.
		 *
		 * \return Returns true if the stored bit pattern represents a NaN for this instance's configuration.
		 */
		bool							IsNaN() const;

		/**
		 * \brief Determines whether this value is positive infinity.
		 *
		 * \return Returns true if the stored bit pattern represents +infinity for this instance's configuration.
		 */
		bool							IsInfP() const;

		/**
		 * \brief Determines whether this value is negative infinity.
		 *
		 * \return Returns true if the stored bit pattern represents -infinity for this instance's configuration.
		 */
		bool							IsInfN() const;

		/**
		 * \brief Creates this value from a double using the specified floating-point bit layout.
		 *
		 * Converts the given double-precision value into the custom format described by the parameters and
		 * stores the resulting sign/exponent/mantissa fields in this instance.
		 *
		 * \param _dVal Input value.
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateFromDouble( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit = true, bool _bHasSign = true );

		/**
		 * \brief Creates this value from a raw integer bit pattern using the specified floating-point bit layout.
		 *
		 * Interprets the low bits of the provided integer as sign/exponent/mantissa fields according to the
		 * specified format and stores them in this instance.
		 *
		 * \param _uiVal Raw bit pattern.
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateFromBits( uint64_t _uiVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit = true, bool _bHasSign = true );

		/**
		 * \brief Creates this value from explicit sign/exponent/mantissa parts using the specified bit layout.
		 *
		 * \param _uiSign Sign bit value (0 or 1). Ignored if \p _bHasSign is false.
		 * \param _uiExp Exponent field value (unshifted).
		 * \param _uiMan Mantissa field value (unshifted; does not include the implicit bit).
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \param _bHasSign True if the format contains a sign bit; false for unsigned formats.
		 * \return Returns *this.
		 */
		CFloatX &						CreateFromParts( uint64_t _uiSign, uint64_t _uiExp, uint64_t _uiMan,
			uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit = true, bool _bHasSign = true );

		/**
		 * \brief Converts this value to a double.
		 *
		 * Decodes the stored fields using this instance's configuration and returns the nearest representable
		 * double-precision value (including infinities and NaNs where applicable).
		 *
		 * \return Returns the decoded value as a double.
		 */
		double							AsDouble() const;

		/**
		 * \brief Packs the stored fields into a raw integer bit pattern.
		 *
		 * \return Returns a uint64_t containing the packed representation for this instance's configuration.
		 */
		uint64_t						AsUint64() const;

		/**
		 * \brief Gets an exponent field value where all exponent bits are set for this instance's exponent width.
		 *
		 * \return Returns the all-ones exponent value for the current configuration.
		 */
		uint64_t						AllExpBitsSet() const { return AllExpBitsSet( uiExpBits ); }

		/**
		 * \brief Packs the stored fields into a raw integer bit pattern excluding the sign bit.
		 *
		 * \return Returns the packed representation with the sign bit cleared/omitted.
		 */
		uint64_t						AsUint64SansSign() const;

		/**
		 * \brief Gets the exponent bias for this instance's exponent width.
		 *
		 * \return Returns the exponent bias.
		 */
		double							ExpBias() const { return ExpBias( uiExpBits ); }

		/**
		 * \brief Gets the exponent bias used for denormalized numbers.
		 *
		 * \return Returns the denormal exponent bias.
		 */
		double							DenormalExpBias() const { return ExpBias() - 1; }

		/**
		 * \brief Determines whether this value is denormalized (subnormal) in its current configuration.
		 *
		 * \return Returns true if the exponent field is zero; false otherwise.
		 */
		bool							IsDenormalized() const { return uiExponent == 0; }

		/**
		 * \brief Gets a sign multiplier for this value.
		 *
		 * \return Returns -1.0 when the sign bit is present and set; otherwise returns +1.0.
		 */
		double							Sign() const { return ((bHasSign && bSign) ? -1.0 : 1.0); }

		/**
		 * \brief Gets the sign bit as a 0/1 value.
		 *
		 * \return Returns 1 if the sign bit is present and set; otherwise returns 0.
		 */
		uint64_t						SignBit() const { return ((bHasSign && bSign) ? 1 : 0); }

		/**
		 * \brief Gets the exponent field value masked to the configured exponent width.
		 *
		 * \return Returns the exponent bits (unshifted).
		 */
		uint64_t						ExpBits() const { return uiExponent & ((1ULL << uiExpBits) - 1ULL); }

		/**
		 * \brief Gets the mantissa field value masked to the configured mantissa width.
		 *
		 * The returned value excludes any implicit leading mantissa bit.
		 *
		 * \return Returns the mantissa bits (unshifted).
		 */
		uint64_t						ManBits() const { return uiMantissa & ((1ULL << RealMantissaBits( uiManBits, bImplicitManBit )) - 1ULL); }

		/**
		 * \brief Enables or disables the sign bit when the format supports a sign.
		 *
		 * \param _bEnabled True to set the sign bit; false to clear it.
		 * \return Returns *this.
		 */
		CFloatX &						SetSign( bool _bEnabled );

		/**
		 * \brief Sets or clears a bit in the exponent field.
		 *
		 * \param _bEnabled True to set the bit; false to clear it.
		 * \param _uiBit Bit index within the exponent field (0 is least significant).
		 * \return Returns *this.
		 */
		CFloatX &						SetExpBit( bool _bEnabled, uint16_t _uiBit );

		/**
		 * \brief Sets or clears a bit in the mantissa field.
		 *
		 * \param _bEnabled True to set the bit; false to clear it.
		 * \param _uiBit Bit index within the mantissa field (0 is least significant).
		 * \return Returns *this.
		 */
		CFloatX &						SetManBit( bool _bEnabled, uint16_t _uiBit );

		/**
		 * \brief Computes the maximum finite value representable by the given bit layout.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \return Returns the maximum finite value as a double.
		 */
		static double					GetMaxForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		/**
		 * \brief Computes the smallest positive non-zero value representable by the given bit layout.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \return Returns the minimum positive non-zero value as a double.
		 */
		static double					GetMinForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		/**
		 * \brief Computes the smallest positive normalized non-zero value representable by the given bit layout.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \return Returns the minimum positive normalized value as a double.
		 */
		static double					GetNormalizedMinForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		/**
		 * \brief Computes the maximum denormal/subnormal value representable by the given bit layout.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if the format uses an implicit leading mantissa bit; false otherwise.
		 * \return Returns the maximum denormalized value as a double.
		 */
		static double					GetDenormalizedMaxForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		/**
		 * \brief Returns an all-ones exponent field for the given exponent width.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \return Returns (2^_uiExpBits - 1).
		 */
		static uint64_t					AllExpBitsSet( uint16_t _uiExpBits ) { return (1 << _uiExpBits) - 1; }

		/**
		 * \brief Computes the exponent bias for the given exponent width.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \return Returns the exponent bias.
		 */
		static const double				ExpBias( uint16_t _uiExpBits ) { return (1 << (_uiExpBits - 1)) - 1; }

		/**
		 * \brief Returns the maximum normalized exponent bias for a given exponent width.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \return Returns the maximum normalized exponent bias.
		 */
		static const double				MaxExpBias( uint16_t _uiExpBits ) { return (1 << (_uiExpBits - 1)) - 1; }

		/**
		 * \brief Returns the minimum normalized exponent bias for a given exponent width.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \return Returns the minimum normalized exponent bias.
		 */
		static const double				MinExpBias( uint16_t _uiExpBits ) { return (1 << (_uiExpBits - 1)) - 1; }

		/**
		 * \brief Gets the maximum number of sign bits supported by this implementation.
		 *
		 * \return Returns the maximum sign-bit count.
		 */
		static const uint32_t			MaxSignBits() { return 1; }

		/**
		 * \brief Gets the maximum number of exponent bits supported by this implementation.
		 *
		 * \return Returns the maximum exponent-bit count.
		 */
		static const uint32_t			MaxExpBits() { return 11; }

		/**
		 * \brief Gets the maximum number of mantissa bits supported by this implementation.
		 *
		 * \return Returns the maximum mantissa-bit count.
		 */
		static const uint32_t			MaxManBits() { return EE_FLOATX_DBL_MAN_BITS; }

		/**
		 * \brief Computes the number of explicit mantissa bits stored for a format.
		 *
		 * When the format uses an implicit leading mantissa bit, the stored mantissa width is one less than
		 * the nominal mantissa width.
		 *
		 * \param _uiManBits Nominal mantissa bit count.
		 * \param _bImplicitMantissaBit True if an implicit leading bit is used.
		 * \return Returns the number of explicitly stored mantissa bits.
		 */
		static const uint64_t			RealMantissaBits( uint16_t _uiManBits, bool _bImplicitMantissaBit ) { return _bImplicitMantissaBit ? _uiManBits - 1 : _uiManBits; }

		/**
		 * \brief Extracts the sign bit from a double without shifting it.
		 *
		 * \param _dVal Input value.
		 * \return Returns the sign bit mask (bit 63) from the IEEE-754 double representation.
		 */
		static uint64_t					SignBit( double _dVal ) { return (*reinterpret_cast<uint64_t *>(&_dVal)) & (1ULL << 63ULL); }

		/**
		 * \brief Extracts the exponent field from a double without shifting it.
		 *
		 * \param _dVal Input value.
		 * \return Returns the exponent bits mask from the IEEE-754 double representation.
		 */
		static uint64_t					ExpBits( double _dVal ) { return (*reinterpret_cast<uint64_t *>(&_dVal)) & (((1ULL << 11ULL) - 1ULL) << (EE_FLOATX_DBL_MAN_BITS - 1ULL)); }

		/**
		 * \brief Extracts the mantissa field from a double without shifting it.
		 *
		 * \param _dVal Input value.
		 * \return Returns the mantissa bits mask from the IEEE-754 double representation.
		 */
		static uint64_t					ManBits( double _dVal ) { return (*reinterpret_cast<uint64_t *>(&_dVal)) & (((1ULL << (EE_FLOATX_DBL_MAN_BITS - 2ULL)) - 1ULL)); }

		/**
		 * \brief Computes the total number of stored bits for a given representation.
		 *
		 * \param _uiExpBits Number of exponent bits.
		 * \param _uiManBits Number of mantissa bits (including the implicit bit if enabled).
		 * \param _bImplicitMantissaBit True if an implicit leading mantissa bit is used.
		 * \param _bHasSign True if the representation includes a sign bit.
		 * \return Returns the total number of bits used by the representation.
		 */
		static uint64_t					TotalBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
			return (_uiExpBits + _uiManBits) + (_bHasSign ? 1 : 0) - (_bImplicitMantissaBit ? 1 : 0);
		}

		/**
		 * \brief Estimates base-10 precision for values greater than 1 given the mantissa width.
		 *
		 * \param _uiManBits Mantissa bit count.
		 * \return Returns an estimate of decimal digits of precision.
		 */
		static double					Precision( uint16_t _uiManBits ) {
			return _uiManBits > 1 ? std::log10( std::pow( 2.0, _uiManBits ) ) :
				(std::log10( _uiManBits ) + 1.0);
		}
	};

}	// namespace ee
#pragma once

#include "../EEExpEval.h"
#include <climits>
#include <crtdbg.h>
#include <cstdint>
#include <cstring>
#include <intrin.h>

#define EE_BIGINT_TABLE_GEN

#ifdef EE_BIGINT_TABLE_GEN
#include <vector>
#endif	// EE_BIGINT_TABLE_GEN

#if 0
#define EE_BI_THROW		throw()
#else 
#define EE_BI_THROW
#endif	// EE_BI_THROW

// Basically a rip of corecrt_internal_big_integer.h from Microsoft, modified for my needs (minimum of 80-bit floats to strings).

#ifndef _DEBUG
#pragma optimize( "gt", on ) // Optimize for maximum speed
#endif

// A lightweight, sufficiently functional high-precision integer type for use in
// the binary floating point <=> decimal string conversions.  We define only the
// operations ( and in some cases parts of operations ) that are actually used.
//
// We require sufficient precision to represent the reciprocal of the smallest
// representable _uiValue ( the smallest denormal, 2^-16445 ).  During parsing, we may
// also consider up to 768 decimal digits.  For this, we require an additional
// log2( 10^768 ) bits of precision.  Finally, we require 32 bits of space for
// pre-division numerator shifting.
//
// PERFORMANCE NOTE:  We intentionally do not initialize the m_ui32Data array when a
// CBigInt object is constructed.  Profiling showed that zero initialization
// caused a substantial performance hit.  Initialization of the m_ui32Data array is
// not necessary:  all operations on the CBigInt type are carefully written
// to only access elements at indices [0, m_ui32Used], and all operations correctly
// update m_ui32Used as the utilized size increases.


namespace ee {

	class CBigInt {
	public :
		__forceinline CBigInt() EE_BI_THROW :
			m_ui32Used( 0 ) {
#ifdef _DEBUG
			std::memset( m_ui32Data, 0xCC, sizeof( m_ui32Data ) );
#endif
		}
		__forceinline CBigInt( CBigInt const &_biOther ) EE_BI_THROW :
			m_ui32Used( _biOther.m_ui32Used ) {
			::memcpy_s( m_ui32Data, sizeof( m_ui32Data ), _biOther.m_ui32Data, _biOther.m_ui32Used * sizeof( uint32_t ) );
		}
		__forceinline CBigInt( uint64_t _uiSrc ) EE_BI_THROW {
			(*this) = MakeBigInt( _uiSrc );
		}
		__forceinline CBigInt( const uint32_t * _uiSrc, uint32_t _uiLen ) EE_BI_THROW :
			m_ui32Used( _uiLen ) {
#ifdef _DEBUG
			std::memset( m_ui32Data, 0xCC, sizeof( m_ui32Data ) );
#endif
			::memcpy_s( m_ui32Data, sizeof( m_ui32Data ), _uiSrc, _uiLen * sizeof( uint32_t ) );
		}


		// == Types.
		struct EE_UNPACK_INDEX {
			uint16_t ui64Offset;						// The offset of this power's initial byte in the array.
			uint8_t ui8Zeroes;							// The number of omitted leading zero elements.
			uint8_t ui8Size;							// The number of elements present for this power.
		};

		__forceinline CBigInt &							operator = ( CBigInt const &_biOther ) EE_BI_THROW {
			m_ui32Used = _biOther.m_ui32Used;
			::memcpy_s( m_ui32Data, sizeof( m_ui32Data ), _biOther.m_ui32Data, _biOther.m_ui32Used * sizeof( uint32_t ) );
			return (*this);
		}

		__forceinline CBigInt &							operator = ( uint64_t _uiOther ) EE_BI_THROW {
			(*this) = MakeBigInt( _uiOther );
			return (*this);
		}

		__forceinline bool								LessThan( const CBigInt &_biOther ) const {
			if ( (*this).m_ui32Used > _biOther.m_ui32Used ) { return false; }
			if ( (*this).m_ui32Used < _biOther.m_ui32Used ) { return true; }

			uint32_t I = (*this).m_ui32Used - 1;
			for ( ; I != static_cast<uint32_t>(-1) && (*this).m_ui32Data[I] == _biOther.m_ui32Data[I]; --I ) {
				// No-op
			}

			if ( I == static_cast<uint32_t>(-1) ) { return false; }
			// Original code:
			/*
			if (lhs._data[i] <= rhs._data[i])
				return true;

			return false;
			*/
			// #1: Can be simplified to just a return statement.
			// #2: <= operator here is an inaccuracy (not a bug).  My version fixes this.

			return (*this).m_ui32Data[I] < _biOther.m_ui32Data[I];
		}

		__forceinline bool								GreaterThan( const CBigInt &_biOther ) const {
			if ( (*this).m_ui32Used < _biOther.m_ui32Used ) { return false; }
			if ( (*this).m_ui32Used > _biOther.m_ui32Used ) { return true; }
			uint32_t I = (*this).m_ui32Used - 1;
			for ( ; I != static_cast<uint32_t>(-1) && (*this).m_ui32Data[I] == _biOther.m_ui32Data[I]; --I ) {
				// No-op
			}

			if ( I == static_cast<uint32_t>(-1) ) { return false; }

			return (*this).m_ui32Data[I] > _biOther.m_ui32Data[I];
		}

		enum : uint32_t {
			EE_MAX_BITS  =
				16445 +									// 16445 bits required to represent 2^16445
				2552 +									// ceil( log2( 10^768 ) )
				32,										// shift space
            
			EE_ELEM_BITS  = sizeof( uint32_t ) * CHAR_BIT,

			EE_ELEM_COUNT = (EE_MAX_BITS + EE_ELEM_BITS - 1) / EE_ELEM_BITS
		};

		uint32_t										m_ui32Used;                // The number of elements currently in use
		uint32_t										m_ui32Data[EE_ELEM_COUNT]; // The number, stored in little endian form



		std::string										ToString( int _iBase = 10 ) {
			_iBase = _iBase <= 0 ? 10 : _iBase;
			std::string sRes;

			// Get the actual number of digits.  There is always at least one.
			size_t ui64Len = 0;
			CBigInt ui64ValueCopy = (*this);
			CBigInt biMul = _iBase;

			while ( true ) {
				if ( this->LessThan( biMul ) ) { break; }
				Multiply( biMul, _iBase );
				++ui64Len;
			}

			sRes.resize( ui64Len + 1 );
			biMul = 1;
			//biMul = _iBase;
			CBigInt biQuo;
			for ( size_t I = 0; I < sRes.size(); ++I ) {
				ui64ValueCopy = (*this);
				uint64_t uiQuo = Divide( ui64ValueCopy, biMul )/* % _iBase*/;
				//uint64_t uiMod = ui64ValueCopy.m_ui32Data[0] % _iBase;
				//uint64_t uiQuo = 1;
				//ui64ValueCopy = (*this);
				//DivideFull( ui64ValueCopy, biMul, biQuo );
				char cTemp = static_cast<char>(uiQuo >= 10 ? ((uiQuo - 10) + 'A') : (uiQuo + '0'));
				sRes[sRes.size()-1-I] = cTemp;
				//ui64ValueCopy = biQuo;
				
				Multiply( biMul, _iBase );
			}

			return sRes;
		}


		static __forceinline ee::CBigInt __cdecl		MakeBigInt( uint64_t const _uiValue ) EE_BI_THROW {
			ee::CBigInt biX{};
			biX.m_ui32Data[0] = _uiValue & 0xFFFFFFFF;
			biX.m_ui32Data[1] = _uiValue >> 32;
			biX.m_ui32Used    = biX.m_ui32Data[1] == 0 ? 1 : 2;
			return biX;
		}

		static __forceinline ee::CBigInt __cdecl		MakeBigIntPo2( uint32_t const power ) EE_BI_THROW {
			uint32_t const uiOne = 1;

			ee::CBigInt biX{};

			uint32_t const uiElemIndex = power / ee::CBigInt::EE_ELEM_BITS;
			uint32_t const uiBitIndex = power % ee::CBigInt::EE_ELEM_BITS;

			std::memset( biX.m_ui32Data, 0, uiElemIndex * sizeof( uint32_t ) );
			biX.m_ui32Data[uiElemIndex] = (uiOne << uiBitIndex);
			biX.m_ui32Used = uiElemIndex + 1;

			return biX;
		}

		static __forceinline bool __cdecl				IsZero( ee::CBigInt const &_uiValue ) EE_BI_THROW {
			return _uiValue.m_ui32Used == 0;
		}

		static __forceinline uint32_t __cdecl			BitScanRev( uint32_t const _uiValue ) EE_BI_THROW {
			unsigned long ulIndex = 0;
			if ( _BitScanReverse( &ulIndex, _uiValue ) ) { return ulIndex + 1; }
			return 0;
		}

		static __forceinline uint32_t __cdecl			BitScanRev( uint64_t const _uiValue ) EE_BI_THROW {
			if ( _uiValue > UINT32_MAX ) {
				return BitScanRev( reinterpret_cast<uint32_t const*>( &_uiValue )[1] ) + 32;
			}
			else {
				return BitScanRev( reinterpret_cast<uint32_t const*>( &_uiValue )[0] );
			}
		}

		static __forceinline uint32_t __cdecl			BitScanRev( ee::CBigInt const &_biX ) EE_BI_THROW {
			if ( _biX.m_ui32Used == 0 ) { return 0; }

			return (_biX.m_ui32Used - 1) * ee::CBigInt::EE_ELEM_BITS + BitScanRev( _biX.m_ui32Data[_biX.m_ui32Used-1] );
		}

		// Shifts the high precision integer _biX by _uiN bits to the left.  Returns true if
		// the left shift was successful; false if it overflowed.  When overflow occurs,
		// the high precision integer is reset to zero.
		static __forceinline bool __cdecl				ShiftLeft( ee::CBigInt & _biX, uint32_t const _uiN ) EE_BI_THROW {
			uint32_t const ui32UnitShift = _uiN / ee::CBigInt::EE_ELEM_BITS;
			uint32_t const ui32BitShift  = _uiN % ee::CBigInt::EE_ELEM_BITS;

			uint64_t const ui64One = 1;

			uint32_t const ui32MsbBits = ui32BitShift;
			uint32_t const ui32LsbBits = ee::CBigInt::EE_ELEM_BITS - ui32MsbBits;

			uint32_t const ui32LsbMask = static_cast<uint32_t>((ui64One << ui32LsbBits) - ui64One);
			uint32_t const ui32MsbMask = ~ui32LsbMask;

			bool const bBitShiftsIntoNextUnit = ui32BitShift > (ee::CBigInt::EE_ELEM_BITS - BitScanRev( _biX.m_ui32Data[_biX.m_ui32Used-1] ));

			bool const bUnitShiftWillOverflow = _biX.m_ui32Used + ui32UnitShift > ee::CBigInt::EE_ELEM_COUNT;
			bool const bBitShiftWillOverflow =
				_biX.m_ui32Used + ui32UnitShift == ee::CBigInt::EE_ELEM_COUNT &&
				bBitShiftsIntoNextUnit;

			if ( bUnitShiftWillOverflow || bBitShiftWillOverflow ) {
				_biX = ee::CBigInt{};
				return false;
			}

			uint32_t const ui32MaxDstIndex = EE_MIN_( _biX.m_ui32Used + ui32UnitShift, ee::CBigInt::EE_ELEM_COUNT - 1 );

			for ( uint32_t ui32DstIdx = ui32MaxDstIndex;
				 ui32DstIdx != static_cast<uint32_t>(-1) && ui32DstIdx >= ui32UnitShift;
				 --ui32DstIdx ) {
				uint32_t const ui32UpperSrcIdx = ui32DstIdx - ui32UnitShift;
				uint32_t const ui32LowerSrcIdx = ui32DstIdx - ui32UnitShift - 1;

				uint32_t const ui32UpperSrc = ui32UpperSrcIdx < _biX.m_ui32Used ? _biX.m_ui32Data[ui32UpperSrcIdx] : 0;
				uint32_t const ui32LowerSrc = ui32LowerSrcIdx < _biX.m_ui32Used ? _biX.m_ui32Data[ui32LowerSrcIdx] : 0;

				uint32_t const ui32ShiftedUpperSrc = (ui32UpperSrc & ui32LsbMask) << ui32MsbBits;
				uint32_t const ui32ShiftedLowerSrc = (ui32LowerSrc & ui32MsbMask) >> ui32LsbBits;

				uint32_t const ui32CombinedShiftedSrc = ui32ShiftedUpperSrc | ui32ShiftedLowerSrc;

				_biX.m_ui32Data[ui32DstIdx] = ui32CombinedShiftedSrc;
			}

			for ( uint32_t ui32DstIdx = 0; ui32DstIdx != ui32UnitShift; ++ui32DstIdx ) {
				_biX.m_ui32Data[ui32DstIdx] = 0;
			}

			_biX.m_ui32Used = bBitShiftsIntoNextUnit
				? ui32MaxDstIndex + 1
				: ui32MaxDstIndex;

			return true;
		}

		static __forceinline bool __cdecl				ShiftRight( ee::CBigInt & _biX, uint32_t const _uiN ) EE_BI_THROW {
			uint32_t const ui32UnitShift = _uiN / ee::CBigInt::EE_ELEM_BITS;
			uint32_t const ui32BitShift  = _uiN % ee::CBigInt::EE_ELEM_BITS;

			uint64_t const ui64One = 1;

			uint32_t const ui32MsbBits = ui32BitShift;
			uint32_t const ui32LsbBits = ee::CBigInt::EE_ELEM_BITS - ui32MsbBits;

			uint32_t const ui32MsbMask = static_cast<uint32_t>((ui64One << (32 - ui32LsbBits)) - ui64One);
			uint32_t const ui32LsbMask = ~ui32MsbMask;
			
			bool const bBitShiftsIntoNextUnit = ui32BitShift >= (BitScanRev( _biX.m_ui32Data[_biX.m_ui32Used-1] ));

			bool const bUnitShiftWillOverflow = ui32UnitShift >= _biX.m_ui32Used;
			/*bool const bBitShiftWillOverflow =
				_biX.m_ui32Used + ui32UnitShift == ee::CBigInt::EE_ELEM_COUNT &&
				bBitShiftsIntoNextUnit;*/

			if ( bUnitShiftWillOverflow /*|| bBitShiftWillOverflow*/ ) {
				_biX = ee::CBigInt{};
				return true;
			}

			uint32_t const ui32MaxDstIndex = _biX.m_ui32Used - ui32UnitShift;

			for ( uint32_t ui32DstIdx = 1;
				 ui32DstIdx <= ui32MaxDstIndex;
				 ++ui32DstIdx ) {
				uint32_t const ui32UpperSrcIdx = ui32DstIdx + ui32UnitShift;
				uint32_t const ui32LowerSrcIdx = ui32DstIdx + ui32UnitShift - 1;

				uint32_t const ui32UpperSrc = ui32UpperSrcIdx < _biX.m_ui32Used ? _biX.m_ui32Data[ui32UpperSrcIdx] : 0;
				uint32_t const ui32LowerSrc = ui32LowerSrcIdx < _biX.m_ui32Used ? _biX.m_ui32Data[ui32LowerSrcIdx] : 0;

				uint32_t const ui32ShiftedUpperSrc = (ui32UpperSrc & ui32MsbMask) << ui32LsbBits;
				uint32_t const ui32ShiftedLowerSrc = (ui32LowerSrc & ui32LsbMask) >> ui32MsbBits;

				uint32_t const ui32CombinedShiftedSrc = ui32ShiftedUpperSrc | ui32ShiftedLowerSrc;

				_biX.m_ui32Data[ui32DstIdx-1] = ui32CombinedShiftedSrc;
			}

			_biX.m_ui32Used = bBitShiftsIntoNextUnit
				? ui32MaxDstIndex - 1
				: ui32MaxDstIndex;

			return true;
		}

		// Adds a 32-bit _uiValue to the high-precision integer _biX.  Returns true if the
		// addition was successful; false if it overflowed.  When overflow occurs, the
		// high precision integer is reset to zero.
		static __forceinline bool __cdecl				Add( ee::CBigInt& _biX, uint32_t _uiValue ) EE_BI_THROW {
			if ( _uiValue == 0 ) {
				return true;
			}

			uint32_t uiCarry = _uiValue;
			for ( uint32_t i = 0; i != _biX.m_ui32Used; ++i ) {
				uint64_t const ui64Res = static_cast<uint64_t>(_biX.m_ui32Data[i]) + uiCarry;
				_biX.m_ui32Data[i] = static_cast<uint32_t>(ui64Res);
				uiCarry = static_cast<uint32_t>(ui64Res >> 32);
			}

			if ( uiCarry != 0 ) {
				if ( _biX.m_ui32Used < ee::CBigInt::EE_ELEM_COUNT ) {
					_biX.m_ui32Data[_biX.m_ui32Used] = uiCarry;
					++_biX.m_ui32Used;
				}
				else {
					_biX = ee::CBigInt{};
					return false;
				}
			}

			return true;
		}

		/*
		// Too easy to create unexpected long loops.
		static __forceinline bool __cdecl Add64( ee::CBigInt& _biX, uint64_t _uiValue ) {
			while ( _uiValue ) {
				uint32_t uiAddMe = static_cast<uint32_t>(_uiValue >= 0xFFFFFFFF ? 0xFFFFFFFF : _uiValue);
				if ( !Add( _biX, uiAddMe ) ) { return false; }
				_uiValue -= uiAddMe;
			}
			return true;
		}*/
		static __forceinline uint32_t __cdecl			AddCarry( 
			uint32_t &_uiU1,
			uint32_t const u2,
			uint32_t const _uiCarry ) EE_BI_THROW {
			uint64_t const ui64Uu = static_cast<uint64_t>(_uiU1) + u2 + _uiCarry;
			_uiU1 = static_cast<uint32_t>(ui64Uu);
			return static_cast<uint32_t>(ui64Uu >> 32);
		}

		static __forceinline uint32_t __cdecl			AddMulCarry( 
			uint32_t &_uiUAdd,
			uint32_t const _uiUMul1,
			uint32_t const _uiUMul2,
			uint32_t const _uiUCarry ) EE_BI_THROW {
			uint64_t const ui64UuRes = static_cast<uint64_t>(_uiUMul1) * _uiUMul2 + _uiUAdd + _uiUCarry;
			_uiUAdd = static_cast<uint32_t>(ui64UuRes);
			return  reinterpret_cast<uint32_t const *>(&ui64UuRes)[1];
		}

		static __forceinline uint32_t __cdecl			MultiplyCore( 
			uint32_t * const _uiMultiplicand,
			uint32_t const _uiMultiplicandCount,
			uint32_t const _uiMultiplier ) EE_BI_THROW {
			uint32_t uiCarry = 0;
			for ( uint32_t I = 0; I != _uiMultiplicandCount; ++I ) {
				uint64_t const ui64Res = static_cast<uint64_t>( _uiMultiplicand[I] ) * _uiMultiplier + uiCarry;
				_uiMultiplicand[I] = static_cast<uint32_t>(ui64Res);
				uiCarry = static_cast<uint32_t>(ui64Res >> 32);
			}

			return uiCarry;
		}


		// Multiplies the high precision multiplicand by a 32-bit multiplier.  Returns
		// true if the multiplication was successful; false if it overflowed.  When
		// overflow occurs, the multiplicand is reset to zero.
		static __forceinline bool __cdecl				Multiply( ee::CBigInt &_biMultiplicand, uint32_t const _uiMultiplier ) EE_BI_THROW {
			if ( _uiMultiplier == 0 ) {
				_biMultiplicand = ee::CBigInt{};
				return true;
			}

			if ( _uiMultiplier == 1 ) { return true; }
			if ( _biMultiplicand.m_ui32Used == 0 ) { return true; }

			uint32_t const uiCarry = MultiplyCore( _biMultiplicand.m_ui32Data, _biMultiplicand.m_ui32Used, _uiMultiplier );
			if ( uiCarry != 0 ) {
				if ( _biMultiplicand.m_ui32Used < ee::CBigInt::EE_ELEM_COUNT ) {
					_biMultiplicand.m_ui32Data[_biMultiplicand.m_ui32Used] = uiCarry;
					++_biMultiplicand.m_ui32Used;
				}
				else {
					_biMultiplicand = ee::CBigInt{};
					return false;
				}
			}

			return true;
		}

		// This high precision integer division implementation was translated from the
		// implementation of System.Numerics.BigIntegerBuilder.Mul in the .NET Framework
		// sources.  It multiplies the multiplicand by the multiplier and returns true
		// if the multiplication was successful; false if it overflowed.  When overflow
		// occurs, the multiplicand is reset to zero.
		static __forceinline bool __cdecl				Multiply( ee::CBigInt & _biMultiplicand, ee::CBigInt const &_biMultiplier ) EE_BI_THROW {
			if ( _biMultiplier.m_ui32Used <= 1 ) {
				return Multiply( _biMultiplicand, _biMultiplier.m_ui32Data[0] );
			}

			if ( _biMultiplicand.m_ui32Used <= 1 ) {
				uint32_t const uiSmallMultiplier = _biMultiplicand.m_ui32Data[0];
				_biMultiplicand = _biMultiplier;
				return Multiply( _biMultiplicand, uiSmallMultiplier );
			}

			// We prefer more iterations on the inner loop and fewer on the outer:
			bool const bMultiplierIsShorter = _biMultiplier.m_ui32Used < _biMultiplicand.m_ui32Used;
			uint32_t const * const puiRgu1 = bMultiplierIsShorter ? _biMultiplier.m_ui32Data : _biMultiplicand.m_ui32Data;
			uint32_t const * const puiRgu2 = bMultiplierIsShorter ? _biMultiplicand.m_ui32Data : _biMultiplier.m_ui32Data;

			uint32_t const cu1 = bMultiplierIsShorter ? _biMultiplier.m_ui32Used : _biMultiplicand.m_ui32Used;
			uint32_t const cu2 = bMultiplierIsShorter ? _biMultiplicand.m_ui32Used : _biMultiplier.m_ui32Used;

			ee::CBigInt biRes{};
			for ( uint32_t uiIu1 = 0; uiIu1 != cu1; ++uiIu1 ) {
				uint32_t const uiUCur = puiRgu1[uiIu1];
				if ( uiUCur == 0 ) {
					if ( uiIu1 == biRes.m_ui32Used ) {
						biRes.m_ui32Data[uiIu1] = 0;
						biRes.m_ui32Used = uiIu1 + 1;
					}

					continue;
				}

				uint32_t uiUCarry = 0;
				uint32_t uiIuCarry = uiIu1;
				for ( uint32_t uiIu2 = 0; uiIu2 != cu2 && uiIuCarry != ee::CBigInt::EE_ELEM_COUNT; ++uiIu2, ++uiIuCarry ) {
					if ( uiIuCarry == biRes.m_ui32Used ) {
						biRes.m_ui32Data[uiIuCarry] = 0;
						biRes.m_ui32Used = uiIuCarry + 1;
					}

					uiUCarry = AddMulCarry( biRes.m_ui32Data[uiIuCarry], uiUCur, puiRgu2[uiIu2], uiUCarry );
				}

				while ( uiUCarry != 0 && uiIuCarry != ee::CBigInt::EE_ELEM_COUNT ) {
					if ( uiIuCarry == biRes.m_ui32Used ) {
						biRes.m_ui32Data[uiIuCarry] = 0;
						biRes.m_ui32Used = uiIuCarry + 1;
					}

					uiUCarry = AddCarry( biRes.m_ui32Data[uiIuCarry++], 0, uiUCarry );
				}

				if ( uiIuCarry == ee::CBigInt::EE_ELEM_COUNT ) {
					_biMultiplicand = ee::CBigInt{};
					return false;
				}
			}

			// Store the biRes in the multiplicand and compute the actual number of
			// elements used:
			_biMultiplicand = biRes;
			return true;
		}

		// Multiplies the high precision integer biX by 10^power.  Returns true if the
		// multiplication was successful; false if it overflowed.  When overflow occurs,
		// the high precision integer is reset to zero.
		static __forceinline bool __cdecl				MulByPo10( ee::CBigInt& biX, uint32_t const _uiPower ) EE_BI_THROW {
			// To improve performance, we use a table of precomputed powers of ten, from
			// 10^10 through 10^380, in increments of ten.  In its unpacked form, as an
			// array of ee::CBigInt objects, this table consists mostly of zero elements.
			// Thus, we store the table in a packed form, trimming leading and trailing
			// zero elements.  We provide an uiIndex that is used to unpack powers from the
			// table, using the function that appears after this function in this file.
			//
			// The minimum _uiValue representable with double precision is 5E-324.  With
			// this table we can thus compute most multiplications with a single Multiply.
			static uint32_t const ui32LargePowerData[] = {
				0x540be400, 0x00000002, 0x63100000, 0x6bc75e2d, 0x00000005, 0x40000000, 0x4674edea, 0x9f2c9cd0, 
				0x0000000c, 0xb9f56100, 0x5ca4bfab, 0x6329f1c3, 0x0000001d, 0xb5640000, 0xc40534fd, 0x926687d2, 
				0x6c3b15f9, 0x00000044, 0x10000000, 0x946590d9, 0xd762422c, 0x9a224501, 0x4f272617, 0x0000009f, 
				0x07950240, 0x245689c1, 0xc5faa71c, 0x73c86d67, 0xebad6ddc, 0x00000172, 0xcec10000, 0x63a22764, 
				0xefa418ca, 0xcdd17b25, 0x6bdfef70, 0x9dea3e1f, 0x0000035f, 0xe4000000, 0xcdc3fe6e, 0x66bc0c6a, 
				0x2e391f32, 0x5a450203, 0x71d2f825, 0xc3c24a56, 0x000007da, 0xa82e8f10, 0xaab24308, 0x8e211a7c, 
				0xf38ace40, 0x84c4ce0b, 0x7ceb0b27, 0xad2594c3, 0x00001249, 0xdd1a4000, 0xcc9f54da, 0xdc5961bf, 
				0xc75cabab, 0xf505440c, 0xd1bc1667, 0xfbb7af52, 0x608f8d29, 0x00002a94, 0x21000000, 0x17bb8a0c, 
				0x56af8ea4, 0x06479fa9, 0x5d4bb236, 0x80dc5fe0, 0xf0feaa0a, 0xa88ed940, 0x6b1a80d0, 0x00006323, 
				0x324c3864, 0x8357c796, 0xe44a42d5, 0xd9a92261, 0xbd3c103d, 0x91e5f372, 0xc0591574, 0xec1da60d, 
				0x102ad96c, 0x0000e6d3, 0x1e851000, 0x6e4f615b, 0x187b2a69, 0x0450e21c, 0x2fdd342b, 0x635027ee, 
				0xa6c97199, 0x8e4ae916, 0x17082e28, 0x1a496e6f, 0x0002196e, 0x32400000, 0x04ad4026, 0xf91e7250, 
				0x2994d1d5, 0x665bcdbb, 0xa23b2e96, 0x65fa7ddb, 0x77de53ac, 0xb020a29b, 0xc6bff953, 0x4b9425ab, 
				0x0004e34d, 0xfbc32d81, 0x5222d0f4, 0xb70f2850, 0x5713f2f3, 0xdc421413, 0xd6395d7d, 0xf8591999, 
				0x0092381c, 0x86b314d6, 0x7aa577b9, 0x12b7fe61, 0x000b616a, 0x1d11e400, 0x56c3678d, 0x3a941f20, 
				0x9b09368b, 0xbd706908, 0x207665be, 0x9b26c4eb, 0x1567e89d, 0x9d15096e, 0x7132f22b, 0xbe485113, 
				0x45e5a2ce, 0x001a7f52, 0xbb100000, 0x02f79478, 0x8c1b74c0, 0xb0f05d00, 0xa9dbc675, 0xe2d9b914, 
				0x650f72df, 0x77284b4c, 0x6df6e016, 0x514391c2, 0x2795c9cf, 0xd6e2ab55, 0x9ca8e627, 0x003db1a6, 
				0x40000000, 0xf4ecd04a, 0x7f2388f0, 0x580a6dc5, 0x43bf046f, 0xf82d5dc3, 0xee110848, 0xfaa0591c, 
				0xcdf4f028, 0x192ea53f, 0xbcd671a0, 0x7d694487, 0x10f96e01, 0x791a569d, 0x008fa475, 0xb9b2e100, 
				0x8288753c, 0xcd3f1693, 0x89b43a6b, 0x089e87de, 0x684d4546, 0xfddba60c, 0xdf249391, 0x3068ec13, 
				0x99b44427, 0xb68141ee, 0x5802cac3, 0xd96851f1, 0x7d7625a2, 0x014e718d, 0xfb640000, 0xf25a83e6, 
				0x9457ad0f, 0x0080b511, 0x2029b566, 0xd7c5d2cf, 0xa53f6d7d, 0xcdb74d1c, 0xda9d70de, 0xb716413d, 
				0x71d0ca4e, 0xd7e41398, 0x4f403a90, 0xf9ab3fe2, 0x264d776f, 0x030aafe6, 0x10000000, 0x09ab5531, 
				0xa60c58d2, 0x566126cb, 0x6a1c8387, 0x7587f4c1, 0x2c44e876, 0x41a047cf, 0xc908059e, 0xa0ba063e, 
				0xe7cfc8e8, 0xe1fac055, 0xef0144b2, 0x24207eb0, 0xd1722573, 0xe4b8f981, 0x071505ae, 0x7a3b6240, 
				0xcea45d4f, 0x4fe24133, 0x210f6d6d, 0xe55633f2, 0x25c11356, 0x28ebd797, 0xd396eb84, 0x1e493b77, 
				0x471f2dae, 0x96ad3820, 0x8afaced1, 0x4edecddb, 0x5568c086, 0xb2695da1, 0x24123c89, 0x107d4571, 
				0x1c410000, 0x6e174a27, 0xec62ae57, 0xef2289aa, 0xb6a2fbdd, 0x17e1efe4, 0x3366bdf2, 0x37b48880, 
				0xbfb82c3e, 0x19acde91, 0xd4f46408, 0x35ff6a4e, 0x67566a0e, 0x40dbb914, 0x782a3bca, 0x6b329b68, 
				0xf5afc5d9, 0x266469bc, 0xe4000000, 0xfb805ff4, 0xed55d1af, 0x9b4a20a8, 0xab9757f8, 0x01aefe0a, 
				0x4a2ca67b, 0x1ebf9569, 0xc7c41c29, 0xd8d5d2aa, 0xd136c776, 0x93da550c, 0x9ac79d90, 0x254bcba8, 
				0x0df07618, 0xf7a88809, 0x3a1f1074, 0xe54811fc, 0x59638ead, 0x97cbe710, 0x26d769e8, 0xb4e4723e, 
				0x5b90aa86, 0x9c333922, 0x4b7a0775, 0x2d47e991, 0x9a6ef977, 0x160b40e7, 0x0c92f8c4, 0xf25ff010, 
				0x25c36c11, 0xc9f98b42, 0x730b919d, 0x05ff7caf, 0xb0432d85, 0x2d2b7569, 0xa657842c, 0xd01fef10, 
				0xc77a4000, 0xe8b862e5, 0x10d8886a, 0xc8cd98e5, 0x108955c5, 0xd059b655, 0x58fbbed4, 0x03b88231, 
				0x034c4519, 0x194dc939, 0x1fc500ac, 0x794cc0e2, 0x3bc980a1, 0xe9b12dd1, 0x5e6d22f8, 0x7b38899a, 
				0xce7919d8, 0x78c67672, 0x79e5b99f, 0xe494034e, 0x00000001, 0xa1000000, 0x6c5cd4e9, 0x9be47d6f, 
				0xf93bd9e7, 0x77626fa1, 0xc68b3451, 0xde2b59e8, 0xcf3cde58, 0x2246ff58, 0xa8577c15, 0x26e77559, 
				0x17776753, 0xebe6b763, 0xe3fd0a5f, 0x33e83969, 0xa805a035, 0xf631b987, 0x211f0f43, 0xd85a43db, 
				0xab1bf596, 0x683f19a2, 0x00000004, 0xbe7dfe64, 0x4bc9042f, 0xe1f5edb0, 0x8fa14eda, 0xe409db73, 
				0x674fee9c, 0xa9159f0d, 0xf6b5b5d6, 0x7338960e, 0xeb49c291, 0x5f2b97cc, 0x0f383f95, 0x2091b3f6, 
				0xd1783714, 0xc1d142df, 0x153e22de, 0x8aafdf57, 0x77f5e55f, 0xa3e7ca8b, 0x032f525b, 0x42e74f3d, 
				0x0000000a, 0xf4dd1000, 0x5d450952, 0xaeb442e1, 0xa3b3342e, 0x3fcda36f, 0xb4287a6e, 0x4bc177f7, 
				0x67d2c8d0, 0xaea8f8e0, 0xadc93b67, 0x6cc856b3, 0x959d9d0b, 0x5b48c100, 0x4abe8a3d, 0x52d936f4, 
				0x71dbe84d, 0xf91c21c5, 0x4a458109, 0xd7aad86a, 0x08e14c7c, 0x759ba59c, 0xe43c8800, 0x00000017, 
				0x92400000, 0x04f110d4, 0x186472be, 0x8736c10c, 0x1478abfb, 0xfc51af29, 0x25eb9739, 0x4c2b3015, 
				0xa1030e0b, 0x28fe3c3b, 0x7788fcba, 0xb89e4358, 0x733de4a4, 0x7c46f2c2, 0x8f746298, 0xdb19210f, 
				0x2ea3b6ae, 0xaa5014b2, 0xea39ab8d, 0x97963442, 0x01dfdfa9, 0xd2f3d3fe, 0xa0790280, 0x00000037, 
				0x509c9b01, 0xc7dcadf1, 0x383dad2c, 0x73c64d37, 0xea6d67d0, 0x519ba806, 0xc403f2f8, 0xa052e1a2, 
				0xd710233a, 0x448573a9, 0xcf12d9ba, 0x70871803, 0x52dc3a9b, 0xe5b252e8, 0x0717fb4e, 0xbe4da62f, 
				0x0aabd7e1, 0x8c62ed4f, 0xceb9ec7b, 0xd4664021, 0xa1158300, 0xcce375e6, 0x842f29f2, 0x00000081, 
				0x7717e400, 0xd3f5fb64, 0xa0763d71, 0x7d142fe9, 0x33f44c66, 0xf3b8f12e, 0x130f0d8e, 0x734c9469, 
				0x60260fa8, 0x3c011340, 0xcc71880a, 0x37a52d21, 0x8adac9ef, 0x42bb31b4, 0xd6f94c41, 0xc88b056c, 
				0xe20501b8, 0x5297ed7c, 0x62c361c4, 0x87dad8aa, 0xb833eade, 0x94f06861, 0x13cc9abd, 0x8dc1d56a, 
				0x0000012d, 0x13100000, 0xc67a36e8, 0xf416299e, 0xf3493f0a, 0x77a5a6cf, 0xa4be23a3, 0xcca25b82, 
				0x3510722f, 0xbe9d447f, 0xa8c213b8, 0xc94c324e, 0xbc9e33ad, 0x76acfeba, 0x2e4c2132, 0x3e13cd32, 
				0x70fe91b4, 0xbb5cd936, 0x42149785, 0x46cc1afd, 0xe638ddf8, 0x690787d2, 0x1a02d117, 0x3eb5f1fe, 
				0xc3b9abae, 0x1c08ee6f, 0x000002be, 0x40000000, 0x8140c2aa, 0x2cf877d9, 0x71e1d73d, 0xd5e72f98, 
				0x72516309, 0xafa819dd, 0xd62a5a46, 0x2a02dcce, 0xce46ddfe, 0x2713248d, 0xb723d2ad, 0xc404bb19, 
				0xb706cc2b, 0x47b1ebca, 0x9d094bdc, 0xc5dc02ca, 0x31e6518e, 0x8ec35680, 0x342f58a8, 0x8b041e42, 
				0xfebfe514, 0x05fffc13, 0x6763790f, 0x66d536fd, 0xb9e15076, 0x00000662, 0x67b06100, 0xd2010a1a, 
				0xd005e1c0, 0xdb12733b, 0xa39f2e3f, 0x61b29de2, 0x2a63dce2, 0x942604bc, 0x6170d59b, 0xc2e32596, 
				0x140b75b9, 0x1f1d2c21, 0xb8136a60, 0x89d23ba2, 0x60f17d73, 0xc6cad7df, 0x0669df2b, 0x24b88737, 
				0x669306ed, 0x19496eeb, 0x938ddb6f, 0x5e748275, 0xc56e9a36, 0x3690b731, 0xc82842c5, 0x24ae798e, 
				0x00000ede, 0x41640000, 0xd5889ac1, 0xd9432c99, 0xa280e71a, 0x6bf63d2e, 0x8249793d, 0x79e7a943, 
				0x22fde64a, 0xe0d6709a, 0x05cacfef, 0xbd8da4d7, 0xe364006c, 0xa54edcb3, 0xa1a8086e, 0x748f459e, 
				0xfc8e54c8, 0xcc74c657, 0x42b8c3d4, 0x57d9636e, 0x35b55bcc, 0x6c13fee9, 0x1ac45161, 0xb595badb, 
				0xa1f14e9d, 0xdcf9e750, 0x07637f71, 0xde2f9f2b, 0x0000229d, 0x10000000, 0x3c5ebd89, 0xe3773756, 
				0x3dcba338, 0x81d29e4f, 0xa4f79e2c, 0xc3f9c774, 0x6a1ce797, 0xac5fe438, 0x07f38b9c, 0xd588ecfa, 
				0x3e5ac1ac, 0x85afccce, 0x9d1f3f70, 0xe82d6dd3, 0x177d180c, 0x5e69946f, 0x648e2ce1, 0x95a13948, 
				0x340fe011, 0xb4173c58, 0x2748f694, 0x7c2657bd, 0x758bda2e, 0x3b8090a0, 0x2ddbb613, 0x6dcf4890, 
				0x24e4047e, 0x00005099, 
			};

			static ee::CBigInt::EE_UNPACK_INDEX const uiLargePwrIndices[] = {
				{    0,  0,  2 }, {    2,  0,  3 }, {    5,  0,  4 }, {    9,  1,  4 }, 
				{   13,  1,  5 }, {   18,  1,  6 }, {   24,  2,  6 }, {   30,  2,  7 }, 
				{   37,  2,  8 }, {   45,  3,  8 }, {   53,  3,  9 }, {   62,  3, 10 }, 
				{   72,  4, 10 }, {   82,  4, 11 }, {   93,  4, 12 }, {  105,  5, 12 }, 
				{  117,  5, 13 }, {  130,  5, 14 }, {  144,  5, 15 }, {  159,  6, 15 }, 
				{  174,  6, 16 }, {  190,  6, 17 }, {  207,  7, 17 }, {  224,  7, 18 }, 
				{  242,  7, 19 }, {  261,  8, 19 }, {  280,  8, 21 }, {  301,  8, 22 }, 
				{  323,  9, 22 }, {  345,  9, 23 }, {  368,  9, 24 }, {  392, 10, 24 }, 
				{  416, 10, 25 }, {  441, 10, 26 }, {  467, 10, 27 }, {  494, 11, 27 }, 
				{  521, 11, 28 }, {  549, 11, 29 },
			};

			uint32_t uiLargePower = _uiPower / 10;
			while ( uiLargePower != 0 ) {
				uint32_t const uiCurPower = uiLargePower > EE_COUNT_OF( uiLargePwrIndices )
					? EE_COUNT_OF( uiLargePwrIndices )
					: uiLargePower;

				ee::CBigInt::EE_UNPACK_INDEX const & uiIndex = uiLargePwrIndices[uiCurPower-1];
				ee::CBigInt biMultiplier{};
				biMultiplier.m_ui32Used = uiIndex.ui8Size + uiIndex.ui8Zeroes;

				uint32_t const * const puiSrc = ui32LargePowerData + uiIndex.ui64Offset;
        
				std::memset( biMultiplier.m_ui32Data, 0, uiIndex.ui8Zeroes * sizeof( uint32_t ) );
				std::memcpy( biMultiplier.m_ui32Data + uiIndex.ui8Zeroes, puiSrc, uiIndex.ui8Size * sizeof( uint32_t ) );

				if ( !Multiply( biX, biMultiplier ) ) {
					biX = ee::CBigInt{};
					return false;
				}

				uiLargePower -= uiCurPower;
			}

			static uint32_t const small_powers_of_ten[9] = {
				  10,
				 100,
				1000,
				1000 *   10,
				1000 *  100,
				1000 * 1000,
				1000 * 1000 *   10,
				1000 * 1000 *  100,
				1000 * 1000 * 1000
			};

			uint32_t const uiSmallPower = _uiPower % 10;
			if ( uiSmallPower != 0 ) {
				if ( !Multiply( biX, small_powers_of_ten[uiSmallPower - 1] ) ) {
					return false;
				}
			}

			return true;
		}

#ifdef EE_BIGINT_TABLE_GEN
		// The following non-compiled functions are the generators for the big powers of
		// ten table found in MulByPo10().  This code is provided for
		// future use if the table needs to be amended.  Do not remove this code.

		static uint32_t									CountLeadingZeroes( ee::CBigInt const &biX ) {
			for ( uint32_t i = 0; i != biX.m_ui32Used; ++i ) {
				if ( biX.m_ui32Data[i] != 0 ) { return i; }
			}

			return 0;
		}

		static void										GenTable() {
			std::vector<uint32_t> vElements;
			std::vector<ee::CBigInt::EE_UNPACK_INDEX> vIndices;

			for ( uint32_t i = 10; i != 390; i += 10 ) {
				ee::CBigInt biX = MakeBigInt( 1 );
				for ( uint32_t j = 0; j != i; ++j ) {
					Multiply( biX, 10 );
				}

				ee::CBigInt::EE_UNPACK_INDEX index{};
				index.ui64Offset = static_cast<uint16_t>( vElements.size() );
				index.ui8Zeroes = uint8_t( CountLeadingZeroes( biX ) );
				index.ui8Size = uint8_t( biX.m_ui32Used - index.ui8Zeroes );

				for ( uint32_t j = index.ui8Zeroes; j != biX.m_ui32Used; ++j ) {
					vElements.push_back( biX.m_ui32Data[j] );
				}
				vIndices.push_back( index );
			}

			_RPT0( _CRT_WARN, "static uint32_t const ui32LargePowerData[] = {\n" );
			for ( uint32_t i = 0; i != vElements.size(); ++i ) {
				_RPT2( _CRT_WARN, "%s0x%08X, ", i % 8 == 0 ? "\n    " : "", vElements[i] );
			}
			_RPT0( _CRT_WARN, "\n};\n" );
    
			_RPT0( _CRT_WARN, "static ee::CBigInt::EE_UNPACK_INDEX const uiLargePwrIndices[] = {\n" );
			for ( uint32_t i = 0; i != vIndices.size(); ++i ) {
				_RPT4( _CRT_WARN, "%s{ %4u, %2u, %2u }, ",
					i % 4 == 0 ? "\n    " : "",
					vIndices[i].ui64Offset,
					vIndices[i].ui8Zeroes,
					vIndices[i].ui8Size );
			}
			_RPT0( _CRT_WARN, "};\n" );
		}

#endif	// EE_BIGINT_TABLE_GEN


		// Computes the number of zeroes higher than the most significant set bit in '_ui32U'
		static __forceinline uint32_t __cdecl			CountSequentialHighZeroes( uint32_t const _ui32U ) EE_BI_THROW {
			unsigned long ulRes;
			return _BitScanReverse( &ulRes, _ui32U ) ? 31 - ulRes : 32;
		}

		// PERFORMANCE NOTE:  On x86, for multiplication of a 64-bit unsigned integer by
		// a 32-bit unsigned integer, the compiler will generate a call to _allmul.  For
		// division-heavy conversions, the inline assembly version presented here gives a
		// 10% overall performance improvement (not 10% faster division--10% faster total).
		// This function [1] uses only two 32-bit multiplies instead of the three required
		// for general 64-bit biX 64-bit multiplication, and [2] is inlineable, allowing the
		// compile to elide the extreme overhead of calling the _allmul function.
#ifdef _M_IX86
		static __forceinline uint64_t __cdecl			multiply_64_32( 
			uint64_t const _ui64Multiplicand,
			uint32_t const _ui32Multiplier ) EE_BI_THROW {
			__asm {
				mov eax, dword ptr [_ui64Multiplicand+4]
				mul _ui32Multiplier

				mov ecx, eax

				mov eax, dword ptr [_ui64Multiplicand]
				mul _ui32Multiplier

				add edx, ecx
			}
		}
#else
		static __forceinline uint64_t __cdecl			multiply_64_32( 
			uint64_t const _ui64Multiplicand,
			uint32_t const _ui32Multiplier ) EE_BI_THROW {
			return _ui64Multiplicand * _ui32Multiplier;
		}
#endif

		// This high precision integer division implementation was translated from the
		// implementation of System.Numerics.BigIntegerBuilder.ModDivCore in the .NET
		// Framework sources.  It computes both quotient and remainder.
		//	The remainder is stored in the numerator argument and the least significant 32 bits of the
		// quotient are returned from the function.
		static inline uint64_t __cdecl					Divide( 
			ee::CBigInt &_biNumerator,
			ee::CBigInt const &_biDenominator ) EE_BI_THROW {
			// If the numerator is zero, then both the quotient and remainder are zero:
			if ( _biNumerator.m_ui32Used == 0 ) { return 0; }

			// If the denominator is zero, then uh oh. We can't divide by zero:
			if ( _biDenominator.m_ui32Used == 0 ) {
				//_ASSERTE( ( "Division by zero", false ) );
				return 0;
			}

			uint32_t uiMaxNumerElemIdx = _biNumerator.m_ui32Used - 1;
			uint32_t uiMaxDenomElemIdx = _biDenominator.m_ui32Used - 1;

			// The numerator and denominator are both nonzero.  If the denominator is
			// only one element wide, we can take the fast route.
			if ( uiMaxDenomElemIdx == 0 ) {
				uint32_t const uiSmallDenom = _biDenominator.m_ui32Data[0];

				if ( uiSmallDenom == 1 ) {
					uint32_t const uiQuotient = _biNumerator.m_ui32Data[0];
					_biNumerator = ee::CBigInt{};
					return uiQuotient;
				}

				if ( uiMaxNumerElemIdx == 0 ) {
					uint32_t const uiSmallNumer = _biNumerator.m_ui32Data[0];

					_biNumerator = ee::CBigInt{};
					_biNumerator.m_ui32Data[0] = uiSmallNumer % uiSmallDenom;
					_biNumerator.m_ui32Used = _biNumerator.m_ui32Data[0] > 0 ? 1 : 0;
					return uiSmallNumer / uiSmallDenom;
				}

				// We count down in the next loop, so the last assignment to quotient
				// will be the correct one.
				uint64_t uiQuotient = 0;

				uint64_t ui64Uu = 0;
				for ( uint32_t Iv = uiMaxNumerElemIdx; Iv != static_cast<uint32_t>(-1); --Iv ) {
					ui64Uu = (ui64Uu << 32) | _biNumerator.m_ui32Data[Iv];
					uiQuotient = (uiQuotient << 32) + static_cast<uint32_t>(ui64Uu / uiSmallDenom);
					ui64Uu %= uiSmallDenom;
				}

				_biNumerator = ee::CBigInt{};
				_biNumerator.m_ui32Data[1] = static_cast<uint32_t>(ui64Uu >> 32);
				_biNumerator.m_ui32Data[0] = static_cast<uint32_t>(ui64Uu);
				_biNumerator.m_ui32Used = (_biNumerator.m_ui32Data[1] > 0) ? 2 : 1;
				return uiQuotient;
			}

			if ( uiMaxDenomElemIdx > uiMaxNumerElemIdx ) { return 0; }

			uint32_t uiCuDen  = uiMaxDenomElemIdx + 1;
			int32_t  iCuDiff = uiMaxNumerElemIdx - uiMaxDenomElemIdx;

			// Determine whether the result will have iCuDiff or iCuDiff + 1 digits:
			int32_t iCuQuo = iCuDiff;
			for ( int32_t Iu = uiMaxNumerElemIdx; ; --Iu ) {
				if ( Iu < iCuDiff ) {
					++iCuQuo;
					break;
				}

				if ( _biDenominator.m_ui32Data[Iu-iCuDiff] != _biNumerator.m_ui32Data[Iu] ) {
					if ( _biDenominator.m_ui32Data[Iu-iCuDiff] < _biNumerator.m_ui32Data[Iu] ) {
						++iCuQuo;
					}

					break;
				}
			}

			if ( iCuQuo == 0 ) { return 0; }

			// Get the uint to use for the trial divisions.  We normalize so the
			// high bit is set.
			uint32_t uiUDen = _biDenominator.m_ui32Data[uiCuDen-1];
			uint32_t uiUDenNext = _biDenominator.m_ui32Data[uiCuDen-2];

			uint32_t uiCBitShiftLeft  = CountSequentialHighZeroes( uiUDen );
			uint32_t uiCBitShiftRight = 32 - uiCBitShiftLeft;
			if ( uiCBitShiftLeft > 0 ) {
				uiUDen = (uiUDen << uiCBitShiftLeft) | (uiUDenNext >> uiCBitShiftRight);
				uiUDenNext <<= uiCBitShiftLeft;

				if ( uiCuDen > 2 ) {
					uiUDenNext |= _biDenominator.m_ui32Data[uiCuDen-3] >> uiCBitShiftRight;
				}
			}

			uint64_t ui64Quotient{};
			for ( int32_t Iu = iCuQuo; --Iu >= 0; ) {
				// Get the high (normalized) bits of the numerator.
				uint32_t uiUNumHi = ( Iu + uiCuDen <= uiMaxNumerElemIdx )
					? _biNumerator.m_ui32Data[Iu + uiCuDen]
					: 0;

				uint64_t ui64UuNum = _biNumerator.m_ui32Data[Iu+uiCuDen-1];
				reinterpret_cast<uint32_t*>(&ui64UuNum)[1] = uiUNumHi;

				uint32_t uiUNumNext = _biNumerator.m_ui32Data[Iu+uiCuDen-2];
				if ( uiCBitShiftLeft > 0 ) {
					ui64UuNum = (ui64UuNum << uiCBitShiftLeft) | (uiUNumNext >> uiCBitShiftRight);
					uiUNumNext <<= uiCBitShiftLeft;

					if ( Iu + uiCuDen >= 3 ) {
						uiUNumNext |= _biNumerator.m_ui32Data[Iu+uiCuDen-3] >> uiCBitShiftRight;
					}
				}

				// Divide to get the quotient digit:
				uint64_t ui64UuQuo = ui64UuNum / uiUDen;
				uint64_t ui64UuRem = static_cast<uint32_t>( ui64UuNum % uiUDen );

				if ( ui64UuQuo > UINT32_MAX ) {
					ui64UuRem += uiUDen * (ui64UuQuo - UINT32_MAX);
					ui64UuQuo  = UINT32_MAX;
				}

				while ( ui64UuRem <= UINT32_MAX && ui64UuQuo * uiUDenNext > ((ui64UuRem << 32) | uiUNumNext) ) {
					--ui64UuQuo;
					ui64UuRem += uiUDen;
				}

				// Multiply and subtract.  Note that ui64UuQuo may be one too large.  If
				// we have a borrow at the end, we'll add the denominator back on and 
				// decrement ui64UuQuo.
				if ( ui64UuQuo > 0 ) {
					uint64_t ui64UuBorrow = 0;

					for ( uint32_t Iu2 = 0; Iu2 < uiCuDen; ++Iu2 ) {
						ui64UuBorrow += multiply_64_32( ui64UuQuo, _biDenominator.m_ui32Data[Iu2] );

						uint32_t const uiUSub = static_cast<uint32_t>(ui64UuBorrow);
						ui64UuBorrow >>= 32;
						if ( _biNumerator.m_ui32Data[Iu+Iu2] < uiUSub ) {
							++ui64UuBorrow;
						}

						_biNumerator.m_ui32Data[Iu+Iu2] -= uiUSub;
					}

					if ( uiUNumHi < ui64UuBorrow ) {
						// Add, tracking carry:
						uint32_t uiUCarry = 0;
						for ( uint32_t Iu2 = 0; Iu2 < uiCuDen; ++Iu2 ) {
							uint64_t const ui64Sum =
								static_cast<uint64_t>(_biNumerator.m_ui32Data[Iu+Iu2]) +
								static_cast<uint64_t>(_biDenominator.m_ui32Data[Iu2])    +
								uiUCarry;

							_biNumerator.m_ui32Data[Iu+Iu2] = static_cast<uint32_t>(ui64Sum);
							uiUCarry = ui64Sum >> 32;
						}

						--ui64UuQuo;
					}

					uiMaxNumerElemIdx = Iu + uiCuDen - 1;
				}

				ui64Quotient = (ui64Quotient << 32) + static_cast<uint32_t>(ui64UuQuo);
			}

			// Trim the remainder:
			for ( uint32_t I = uiMaxNumerElemIdx + 1; I < _biNumerator.m_ui32Used; ++I ) {
				_biNumerator.m_ui32Data[I] = 0;
			}

			_biNumerator.m_ui32Used = uiMaxNumerElemIdx + 1;
			while ( _biNumerator.m_ui32Used != 0 && _biNumerator.m_ui32Data[_biNumerator.m_ui32Used-1] == 0 ) {
				--_biNumerator.m_ui32Used;
			}

			return ui64Quotient;
		}


		static inline void __cdecl						DivideFull( 
			ee::CBigInt &_biNumerator,
			ee::CBigInt const &_biDenominator,
			ee::CBigInt &_biQuotient ) EE_BI_THROW {
			// If the numerator is zero, then both the quotient and remainder are zero:
			if ( _biNumerator.m_ui32Used == 0 ) {
				_biQuotient = 0;
				return;
			}

			// If the denominator is zero, then uh oh. We can't divide by zero:
			if ( _biDenominator.m_ui32Used == 0 ) {
				//_ASSERTE( ( "Division by zero", false ) );
				_biQuotient = 0;
				return;
			}

			uint32_t uiMaxNumerElemIdx = _biNumerator.m_ui32Used - 1;
			uint32_t uiMaxDenomElemIdx = _biDenominator.m_ui32Used - 1;

			// The numerator and denominator are both nonzero.  If the denominator is
			// only one element wide, we can take the fast route.
#if 0
			if ( uiMaxDenomElemIdx == 0 ) {
				uint32_t const uiSmallDenom = _biDenominator.m_ui32Data[0];

				if ( uiSmallDenom == 1 ) {
					_biQuotient = _biNumerator;
					_biNumerator = ee::CBigInt{};
					return;
				}

				if ( uiMaxNumerElemIdx == 0 ) {
					uint32_t const uiSmallNumer = _biNumerator.m_ui32Data[0];

					_biNumerator = ee::CBigInt{};
					_biNumerator.m_ui32Data[0] = uiSmallNumer % uiSmallDenom;
					_biNumerator.m_ui32Used = _biNumerator.m_ui32Data[0] > 0 ? 1 : 0;
					_biQuotient = uiSmallNumer / uiSmallDenom;
					return;
				}

				// We count down in the next loop, so the last assignment to quotient
				// will be the correct one.
				//uint64_t uiQuotient = 0;
				_biQuotient = 0;

				uint64_t ui64Uu = 0;
				//ee::CBigInt biUu = 0;
				for ( uint32_t Iv = uiMaxNumerElemIdx; Iv != static_cast<uint32_t>(-1); --Iv ) {
					ui64Uu = (ui64Uu << 32) | _biNumerator.m_ui32Data[Iv];
					//uiQuotient = (uiQuotient << 32) + static_cast<uint32_t>(ui64Uu / uiSmallDenom);
					ShiftLeft( _biQuotient, 32 );
					Add( _biQuotient, static_cast<uint32_t>(ui64Uu / uiSmallDenom) );
					ui64Uu %= uiSmallDenom;
				}

				_biNumerator = ee::CBigInt{};
				_biNumerator.m_ui32Data[1] = static_cast<uint32_t>(ui64Uu >> 32);
				_biNumerator.m_ui32Data[0] = static_cast<uint32_t>(ui64Uu);
				_biNumerator.m_ui32Used = (_biNumerator.m_ui32Data[1] > 0) ? 2 : 1;
				//_biQuotient = uiQuotient;
				return;
			}
#endif	// #if 0

			if ( uiMaxDenomElemIdx > uiMaxNumerElemIdx ) {
				_biQuotient = 0;
				return;
			}

			uint32_t uiCuDen  = uiMaxDenomElemIdx + 1;
			int32_t  iCuDiff = uiMaxNumerElemIdx - uiMaxDenomElemIdx;

			// Determine whether the result will have iCuDiff or iCuDiff + 1 digits:
			int32_t iCuQuo = iCuDiff;
			for ( int32_t Iu = uiMaxNumerElemIdx; ; --Iu ) {
				if ( Iu < iCuDiff ) {
					++iCuQuo;
					break;
				}

				if ( _biDenominator.m_ui32Data[Iu-iCuDiff] != _biNumerator.m_ui32Data[Iu] ) {
					if ( _biDenominator.m_ui32Data[Iu-iCuDiff] < _biNumerator.m_ui32Data[Iu] ) {
						++iCuQuo;
					}

					break;
				}
			}

			if ( iCuQuo == 0 ) {
				_biQuotient = 0;
				return;
			}

			// Get the uint to use for the trial divisions.  We normalize so the
			// high bit is set.
			uint32_t uiUDen = _biDenominator.m_ui32Data[uiCuDen-1];
			uint32_t uiUDenNext = _biDenominator.m_ui32Data[uiCuDen-2];

			uint32_t uiCBitShiftLeft  = CountSequentialHighZeroes( uiUDen );
			uint32_t uiCBitShiftRight = 32 - uiCBitShiftLeft;
			if ( uiCBitShiftLeft > 0 ) {
				uiUDen = (uiUDen << uiCBitShiftLeft) | (uiUDenNext >> uiCBitShiftRight);
				uiUDenNext <<= uiCBitShiftLeft;

				if ( uiCuDen > 2 ) {
					uiUDenNext |= _biDenominator.m_ui32Data[uiCuDen-3] >> uiCBitShiftRight;
				}
			}

			//uint64_t ui64Quotient{};
			_biQuotient = 0;
			for ( int32_t Iu = iCuQuo; --Iu >= 0; ) {
				// Get the high (normalized) bits of the numerator.
				uint32_t uiUNumHi = ( Iu + uiCuDen <= uiMaxNumerElemIdx )
					? _biNumerator.m_ui32Data[Iu + uiCuDen]
					: 0;

				uint64_t ui64UuNum = _biNumerator.m_ui32Data[Iu+uiCuDen-1];
				reinterpret_cast<uint32_t*>(&ui64UuNum)[1] = uiUNumHi;

				uint32_t uiUNumNext = _biNumerator.m_ui32Data[Iu+uiCuDen-2];
				if ( uiCBitShiftLeft > 0 ) {
					ui64UuNum = (ui64UuNum << uiCBitShiftLeft) | (uiUNumNext >> uiCBitShiftRight);
					uiUNumNext <<= uiCBitShiftLeft;

					if ( Iu + uiCuDen >= 3 ) {
						uiUNumNext |= _biNumerator.m_ui32Data[Iu+uiCuDen-3] >> uiCBitShiftRight;
					}
				}

				// Divide to get the quotient digit:
				uint64_t ui64UuQuo = ui64UuNum / uiUDen;
				uint64_t ui64UuRem = static_cast<uint32_t>( ui64UuNum % uiUDen );

				if ( ui64UuQuo > UINT32_MAX ) {
					ui64UuRem += uiUDen * (ui64UuQuo - UINT32_MAX);
					ui64UuQuo  = UINT32_MAX;
				}

				while ( ui64UuRem <= UINT32_MAX && ui64UuQuo * uiUDenNext > ((ui64UuRem << 32) | uiUNumNext) ) {
					--ui64UuQuo;
					ui64UuRem += uiUDen;
				}

				// Multiply and subtract.  Note that ui64UuQuo may be one too large.  If
				// we have a borrow at the end, we'll add the denominator back on and 
				// decrement ui64UuQuo.
				if ( ui64UuQuo > 0 ) {
					uint64_t ui64UuBorrow = 0;

					for ( uint32_t Iu2 = 0; Iu2 < uiCuDen; ++Iu2 ) {
						ui64UuBorrow += multiply_64_32( ui64UuQuo, _biDenominator.m_ui32Data[Iu2] );

						uint32_t const uiUSub = static_cast<uint32_t>(ui64UuBorrow);
						ui64UuBorrow >>= 32;
						if ( _biNumerator.m_ui32Data[Iu+Iu2] < uiUSub ) {
							++ui64UuBorrow;
						}

						_biNumerator.m_ui32Data[Iu+Iu2] -= uiUSub;
					}

					if ( uiUNumHi < ui64UuBorrow ) {
						// Add, tracking carry:
						uint32_t uiUCarry = 0;
						for ( uint32_t Iu2 = 0; Iu2 < uiCuDen; ++Iu2 ) {
							uint64_t const ui64Sum =
								static_cast<uint64_t>(_biNumerator.m_ui32Data[Iu+Iu2]) +
								static_cast<uint64_t>(_biDenominator.m_ui32Data[Iu2])    +
								uiUCarry;

							_biNumerator.m_ui32Data[Iu+Iu2] = static_cast<uint32_t>(ui64Sum);
							uiUCarry = ui64Sum >> 32;
						}

						--ui64UuQuo;
					}

					uiMaxNumerElemIdx = Iu + uiCuDen - 1;
				}

				//ui64Quotient = (ui64Quotient << 32) + static_cast<uint32_t>(ui64UuQuo);
				ShiftLeft( _biQuotient, 32 );
				Add( _biQuotient, static_cast<uint32_t>(ui64UuQuo) );
			}

			// Trim the remainder:
			for ( uint32_t I = uiMaxNumerElemIdx + 1; I < _biNumerator.m_ui32Used; ++I ) {
				_biNumerator.m_ui32Data[I] = 0;
			}

			_biNumerator.m_ui32Used = uiMaxNumerElemIdx + 1;
			while ( _biNumerator.m_ui32Used != 0 && _biNumerator.m_ui32Data[_biNumerator.m_ui32Used-1] == 0 ) {
				--_biNumerator.m_ui32Used;
			}

			return;
		}


	protected :
#if 0
		static inline void __cdecl						DivideM( const ee::CBigInt &_liLeft,
			const ee::CBigInt &_liRight,
			ee::CBigInt &_liQuotient,
			ee::CBigInt &_liRemainder ) {

			ee::CBigInt liTemp0;
			ee::CBigInt liTemp1;
			//CLargeInteger<_uMaxDigits+1> liTemp1;
			//pliResult = &liTemp0;
			//pliRemainder = &liTemp1;

			uint32_t ui32Mask = 0x80000000UL;
			uint32_t ui32Value = _liRight.m_ui32Data[_liRight.m_ui32Used-1];
			uint32_t ui32Shift = 0, ui32ResPos = 0;
			while ( ui32Mask && (ui32Value & ui32Mask) == 0 ) {
				++ui32Shift;
				ui32Mask >>= 1;
			}

			for ( uint32_t I = _liLeft.m_ui32Used; I--; ) {
				liTemp1.m_ui32Data[I] = _liLeft.m_ui32Data[I];
			}
			liTemp1.m_ui32Used = _liLeft.m_ui32Used + 1;

			ShiftLeft( liTemp1, ui32Shift );
			ee::CBigInt liNewRight = _liRight;
			//ee::CBigInt liNewRight = _liRight << ui32Shift;
			ShiftLeft( liNewRight, ui32Shift );

			uint32_t ui32J = liTemp1.m_ui32Used - liNewRight.m_ui32Used;
			uint32_t ui32Pos = liTemp1.m_ui32Used - 1;

			uint32_t ui32FirstDivisorByte = liNewRight.m_ui32Data[liNewRight.m_ui32Used-1];
			uint32_t ui32SecondDivisorByte = liNewRight.m_ui32Data[liNewRight.m_ui32Used-2];

			uint32_t ui32DivisorLen = liNewRight.m_ui32Used + 1;
			uint32_t * pui32DividendPart = new( std::nothrow ) uint32_t[ui32DivisorLen];

			for ( ; ui32J > 0; --ui32J ) {
				uint64_t ui64Dividend = (static_cast<uint64_t>(liTemp1.m_ui32Data[ui32Pos]) << 32ULL) + liTemp1.m_ui32Data[ui32Pos-1];

				uint64_t ui64Q = ui64Dividend / ui32FirstDivisorByte;
				uint64_t ui64R = ui64Dividend % ui32FirstDivisorByte;

				bool bDone = false;
				while ( !bDone ) {
					bDone = true;

					if ( ui64Q == 0x100000000ULL ||
						(ui64Q * ui32SecondDivisorByte) > ((ui64R << 32ULL) + liTemp1.m_ui32Data[ui32Pos-2]) ) {
						--ui64Q;
						ui64R += ui32FirstDivisorByte;
						if ( ui64R < 0x100000000ULL ) {
							bDone = false;
						}
					}
				}

				for ( uint32_t I = 0; I < ui32DivisorLen; ++I ) {
					pui32DividendPart[I] = liTemp1.m_ui32Data[ui32Pos-I];
				}

				// Again on the heap instead of on the stack.
				ee::CBigInt liTemp2;
				//ee::CBigInt * pliK;
				//if ( _uMaxDigits < 128 ) {
					liTemp2 = ee::CBigInt( pui32DividendPart, ui32DivisorLen );
					//pliK = &liTemp2;
				/*}
				else {
					pliK = new( std::nothrow ) ee::CBigInt( pui32DividendPart, ui32DivisorLen );
				}*/
			
				ee::CBigInt liS = liNewRight;// * ee::CBigInt( static_cast<int64_t>(ui64Q) ) );
				Multiply( liS, ee::CBigInt( static_cast<int64_t>(ui64Q) ) );

				while ( liS > liTemp2 ) {
					--ui64Q;
					liS = liS - liNewRight;
					//Subtr
				}

				ee::CBigInt liY = liTemp2 - liS;
				for ( uint32_t I = 0; I < ui32DivisorLen; ++I ) {
					liTemp1.m_ui32Data[ui32Pos-I] = liY.m_ui32Data[liNewRight.m_ui32Used-I];
				}

				/*if ( _uMaxDigits < 128 ) {
				}
				else {
					delete pliK;
				}*/

				pliResult->m_ui32Data[ui32ResPos++] = static_cast<uint32_t>(ui64Q);
				--ui32Pos;
			}

			_liQuotient.m_ui32Used = ui32ResPos;

			uint32_t Y = 0;
			for ( int32_t X = static_cast<int32_t>(_liQuotient.m_ui32Used) - 1; X >= 0; --X, ++Y ) {
				_liQuotient.m_ui32Data[Y] = pliResult->m_ui32Data[X];
			}

			for ( ; Y < _uMaxDigits; ++Y ) {
				_liQuotient.m_ui32Data[Y] = 0;
			}

			while ( _liQuotient.m_ui32Used > 1 && _liQuotient.m_ui32Data[_liQuotient.m_ui32Used-1] == 0 ) {
				--_liQuotient.m_ui32Used;
			}

			_liQuotient.m_ui32Used = EE_MIN( _liQuotient.m_ui32Used, 1 );

			_liRemainder.m_ui32Used = ShiftRight( liTemp1.m_ui32Data, liTemp1.m_ui32Used, ui32Shift );
			//std::memcpy_s( _liRemainder.m_ui32Data, liTemp1.m_ui32Data, sizeof( _liRemainder.m_ui32Data[0] ) * _liRemainder.m_ui32Used );
			::memcpy_s( _liRemainder.m_ui32Data, sizeof( _liRemainder.m_ui32Data ), _biOther.m_ui32Data, _biOther.m_ui32Used * sizeof( uint32_t ) );
			for ( Y = _liRemainder.m_ui32Used; Y < _uMaxDigits; ++Y ) {
				_liRemainder.m_ui32Data[Y] = 0;
			}

			delete pui32DividendPart;
		}
#endif	// #if 0
	};

}	// namespace ee

__forceinline bool __cdecl								operator == ( ee::CBigInt const &_biLhs, ee::CBigInt const &_biRhs ) EE_BI_THROW {
	if ( _biLhs.m_ui32Used != _biRhs.m_ui32Used ) { return false; }

    for ( uint32_t i = 0; i != _biLhs.m_ui32Used; ++i ) {
		if ( _biLhs.m_ui32Data[i] != _biRhs.m_ui32Data[i] ) { return false; }
    }

    return true;
}

__forceinline bool __cdecl								operator != ( ee::CBigInt const &_biLhs, ee::CBigInt const &_biRhs ) EE_BI_THROW {
    return !(_biRhs == _biLhs);
}

__forceinline bool __cdecl								operator < ( ee::CBigInt const &_biLhs, ee::CBigInt const &_biRhs ) EE_BI_THROW {
	return _biLhs.LessThan( _biRhs );
}

__forceinline bool __cdecl								operator > ( ee::CBigInt const &_biLhs, ee::CBigInt const &_biRhs ) EE_BI_THROW {
	return _biLhs.GreaterThan( _biRhs );
}

__forceinline bool __cdecl								operator >= ( ee::CBigInt const &_biLhs, ee::CBigInt const &_biRhs ) EE_BI_THROW {
    return !(_biLhs < _biRhs);
}

__forceinline bool __cdecl								operator <= ( ee::CBigInt const &_biLhs, ee::CBigInt const &_biRhs ) EE_BI_THROW {
    return !(_biLhs > _biRhs);
}

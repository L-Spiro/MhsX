#pragma once

#include "SinCos/EESinCos.h"

#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cinttypes>
#include <cwctype>
#include <fenv.h>
#include <iomanip>
#include <numbers>
#include <numeric>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>					// For QueryPerformanceCounter() and QueryPerformanceFrequency().
#endif

#ifndef EE_MIN_
#define EE_MIN_( A, B )					(((A) < (B)) ? (A) : (B))
#endif	// EE_MIN_

#ifndef EE_MAX_
#define EE_MAX_( A, B )					(((A) > (B)) ? (A) : (B))
#endif	// EE_MAX_

#ifndef EE_UTF_INVALID
#define EE_UTF_INVALID					~static_cast<uint32_t>(0)
#endif	// EE_UTF_INVALID

#ifndef EE_INVALID_IDX
#define EE_INVALID_IDX					static_cast<size_t>(-1)
#endif	// EE_INVALID_IDX

#ifndef EE_MAX_ITERATION_COUNT
#ifndef NDEBUG
#define EE_MAX_ITERATION_COUNT			90000
#else
#define EE_MAX_ITERATION_COUNT			0x80000000
#endif	// NDEBUG
#endif	// EE_MAX_ITERATION_COUNT

#ifndef EE_PI
#define EE_PI							3.141592653589793115997963468544185161590576171875	// 3.14159265358979323846264338327950288419716939937510 rounded to the nearest representable double.
#endif // #ifndef EE_PI

#if defined( _MSC_VER )
#define EE_FALLTHROUGH					[[fallthrough]];
#else
#define EE_FALLTHROUGH					
#endif	// #if defined( _MSC_VER )


namespace ee {

	// == Enumerations.
	/** String numeric classifications. */
	enum EE_STRING_NUMBER_CLASS {
		EE_SNC_UNSIGNED,
		EE_SNC_SIGNED,
		EE_SNC_FLOAT,
		EE_SNC_INVALID
	};

	/**
	 * Kind of sort to apply to a text column (no Win32 APIs used).
	 * 
	 * Enumerants select the comparison policy for strings. See CmpTextNoWin32().
	 **/
	enum EE_TEXTPOLICY {
		EE_TP_ORDINAL_CI = 0,			/**< \brief Case-insensitive ordinal (basic Unicode fold). */
		EE_TP_NATURAL_CI				/**< \brief Natural order: numeric runs compare by value, case-insensitive. */
	};

	class CExpEval {
	public :
		// == Types.
		/** Sinc-filter window function. */
		template <typename _tType = double>
		using							PfWindowFunc = bool (*)( size_t _sN, std::vector<_tType> &_vRet );

		/** Temporarily setting the floating-point rounding mode. */
		struct							EE_FEROUNDMODE {
			EE_FEROUNDMODE( int _iNewMode ) :
				iPrevMode( ::fegetround() ) {
				::fesetround( _iNewMode );
			}
			~EE_FEROUNDMODE() {
				::fesetround( iPrevMode );
			}

			int													iPrevMode;					/**< The previous rounding mode. */
			// FE_TONEAREST FE_DOWNWARD FE_UPWARD FE_TOWARDZERO
		};

		// == Functions.
		/**
		 * Gets the time of initialization.
		 * 
		 * \return Returns the system time when InitializeExpressionEvaluatorLibrary() was called.
		 **/
		static uint64_t					StartTime();

		/**
		 * Called at start-up to initialize the Expression Evaluator Library.
		 **/
		static inline void				InitializeExpressionEvaluatorLibrary() {
			StartTime();
		}

		// Min.
		template <typename _tT>
		static inline _tT				Min( const _tT &_tA, const _tT &_tB ) { return _tA < _tB ? _tA : _tB; }

		// Max.
		template <typename _tT>
		static inline _tT				Max( const _tT &_tA, const _tT &_tB ) { return _tA > _tB ? _tA : _tB; }

		/**
		 * Gets the largest absolute value in the given vector.
		 * 
		 * \param _tA The vector to find the max() of.
		 * \return Returns the maximum absolute value in the given vector.
		 **/
		template <typename _tType = std::vector<double>>
		static inline typename _tType::value_type
										MaxVec( const _tType &_tA ) {
			typename _tType::value_type dRet = _tType::value_type( 0 );
			for ( auto I = _tA.size(); I--; ) {
				typename _tType::value_type dTmp = _tA[I];
				dTmp = dTmp < _tType::value_type( 0 ) ? -dTmp : dTmp;
				dRet = Max( dRet, dTmp );
			}
			return dRet;
		}

		/**
		 * Is the given character valid binary character?
		 * 
		 * \param _cValue The value to test.
		 * \return Returns true if _cValue is '0' or '1'. 
		 **/
		static inline bool				ValidBin( char _cValue ) {
			return (_cValue >= '0' && _cValue <= '1');
		}

		/**
		 * Is the given character valid hex character?
		 * 
		 * \param _cValue The value to test.
		 * \return Returns true if _cValue is a valid hexadecimal character (0-9, a-f, A-F). 

		 **/
		static inline bool				ValidHex( char _cValue ) {
			return ((_cValue >= '0' && _cValue <= '9') ||
				(_cValue >= 'a' && _cValue <= 'f') ||
				(_cValue >= 'A' && _cValue <= 'F'));
		}

		/**
		 * Is the given string valid hex?
		 * 
		 * \param _pcValue The string to test.
		 * \return Returns true if all characters in the string are valid hexadecimal characters.
		 **/
		static inline bool				ValidHex( const char * _pcValue ) {
			while ( (*_pcValue++) ) {
				if ( ((*_pcValue) == ' ' || (*_pcValue) == '\t') ||
					ValidHex( (*_pcValue) ) ) { continue; }
				return false;
			}
			return true;
		}

		/**
		 * Is the given character valid octal character?
		 * 
		 * \param _cValue The value to test.
		 * \return Returns true if _cValue is a valid octal character (0-7).
		 **/
		static inline bool				ValidOctal( char _cValue ) {
			return (_cValue >= '0' && _cValue <= '7');
		}

		/**
		 * Converts a hex character to an integer.
		 * 
		 * \param _cVal The character to convert.
		 * \return Returns the converted character between 0 and 15 inclusive.
		 **/
		static inline uint32_t			HexToUint32( char _cVal ) {
			if ( _cVal >= '0' && _cVal <= '9' ) { return _cVal - '0'; }
			if ( _cVal >= 'A' && _cVal <= 'F' ) { return _cVal - 'A' + 10; }
			if ( _cVal >= 'a' && _cVal <= 'f' ) { return _cVal - 'a' + 10; }
			return 0;
		}

		/**
		 * Converts an ASCII or fullwidth decimal digit to its numeric value.
		 * 
		 * Accepts U+0030..U+0039 ('0'..'9') and U+FF10..U+FF19 ('０'..'９'). Non-digits are rejected.
		 * 
		 * \param _wcVal Input wide character.
		 * \return Returns the digit value in the range 0..9 on success; otherwise -1.
		 **/
		static inline int32_t			DigitValWide( wchar_t _wcVal ) {
			if ( _wcVal >= L'0' && _wcVal <= L'9' ) { return static_cast<int32_t>(_wcVal - L'0'); }
			if ( _wcVal >= L'\uFF10' && _wcVal <= L'\uFF19' ) { return static_cast<int32_t>(_wcVal - L'\uFF10'); }
			return -1;
		}

		/**
		 * Converts an octal character to an integer.
		 * 
		 * \param _cVal The character to convert.
		 * \return Returns the converted character between 0 and 7 inclusive.
		 **/
		static inline uint32_t			OctalToUint32( char _cVal ) {
			if ( _cVal >= '0' && _cVal <= '7' ) { return _cVal - '0'; }
			return 0;
		}

		/**
		 * Checks for whitespace without throwing exceptions.
		 * 
		 * \param _cVal The character to test.
		 * \return Returns true if _cVal is ' ', '\r', '\n', '\t', '\f', or '\v'.
		 **/
		static inline bool				IsWhiteSpace( char _cVal ) {
			return _cVal == ' ' ||
				_cVal == '\r' ||
				_cVal == '\n' ||
				_cVal == '\t' ||
				_cVal == '\f' ||
				_cVal == '\v';
		}

		/**
		 * Checks for digits (0-9) without throwing exceptions.
		 * 
		 * \param _cVal The character to test.
		 * \return Returns true if _cVal is between 0 and 9 inclusive.
		 **/
		static inline bool				IsDigit( char _cVal ) {
			return _cVal >= '0' && _cVal <= '9';
		}

		/**
		 * Checks for digits (0-9) without throwing exceptions.
		 * 
		 * \param _wcVal The character to test.
		 * \return Returns true if _cVal is between 0 and 9 inclusive.
		 **/
		static inline bool				IsDigit( wchar_t _wcVal ) {
			return (_wcVal >= L'0' && _wcVal <= L'9') ||
				(_wcVal >= L'\uFF10' && _wcVal <= L'\uFF19');
		}

		/**
		 * Checks for alpha characters (a-z, A-Z) without throwing exceptions.
		 * 
		 * \param _cVal The character to test.
		 * \return Returns true if _cVal is an alpha character (a-z, A-Z).
		 **/
		static inline bool				IsAlpha( char _cVal ) {
			return (_cVal >= 'A' && _cVal <= 'Z') ||
				(_cVal >= 'a' && _cVal <= 'z');
		}

		/**
		 * Determines if a character is an identifier character.  Sets _bIsFirst to false internally.
		 * 
		 * \param _cVal		The character to test.
		 * \param _bIsFirst	Indicates that _cVal is the first character in the string being tested.
		 * \return			If _bIsFirst, true is returned if _cVal is an alpha character or '_', otherwise true is returned if _cVal is an alpha character, a digit, or '_'.
		 **/
		static inline bool				IsIdentifier( char _cVal, bool &_bIsFirst ) {
			if ( _bIsFirst ) {
				_bIsFirst = false;
				return IsAlpha( _cVal ) || _cVal == '_';
			}
			return IsAlpha( _cVal ) || IsDigit( _cVal ) || _cVal == '_';
		}

		/**
		 * Determines if the given string is an identifier.
		 * 
		 * \param _sIdent		The string to test.
		 * \tparam _tStringType	The type of string (std::string, std::wstring, std:u8string, etc.)
		 * \return				Returns true if the whole string is a valid identifier.
		 **/
		template <typename _tStringType = std::string>
		static bool						IsIdentifier( const _tStringType &_sIdent ) {
			if ( !_sIdent.size() ) { return false; }
			bool bStart = true;
			for ( size_t I = 0; I < _sIdent.size(); ++I ) {
				if ( !IsIdentifier( static_cast<char>(_sIdent[I]), bStart ) ) { return false; }
			}
			return true;
		}

		/**
		 * Folds a wide character for portable, case-insensitive comparison.
		 * 
		 * Converts the following before comparison: fullwidth space to ASCII space, fullwidth ASCII
		 * (U+FF01..U+FF5E) to ASCII (U+0021..U+007E), Hiragana (U+3041..U+3096) to Katakana
		 * (U+30A1..U+30F6), then applies towlower(). This is a lightweight normalization to reduce
		 * width, kana-type, and case differences without using Win32 collation.
		 * 
		 * \param _wcVal Input wide character.
		 * \return Returns the folded wide character suitable for ordinal comparisons.
		 **/
		static inline wchar_t			FoldCharBasicJa( wchar_t _wcVal ) {
			// Fullwidth space -> space.
			if ( _wcVal == 0x3000 ) { _wcVal = L' '; }

			// Fullwidth ASCII (FF01..FF5E) -> ASCII (0021..007E).
			if ( _wcVal >= 0xFF01 && _wcVal <= 0xFF5E ) {
				_wcVal = static_cast<wchar_t>(_wcVal - 0xFEE0);
			}

			// Hiragana (3041..3096) -> Katakana (30A1..30F6).
			if ( _wcVal >= 0x3041 && _wcVal <= 0x3096 ) {
				_wcVal = static_cast<wchar_t>(_wcVal + 0x0060);
			}

			// Case-fold (basic).
			_wcVal = static_cast<wchar_t>(::towlower( _wcVal ));
			return _wcVal;
		}

		/**
		 * Case-insensitive ordinal comparison using a lightweight Unicode fold.
		 * 
		 * Converts fullwidth ASCII to ASCII, normalizes Hiragana to Katakana, and applies towlower().
		 * 
		 * \param _wsA Left string.
		 * \param _wsB Right string.
		 * \return Returns -1 if _wsA < _wsB, 1 if _wsA > _wsB, or 0 if equal.
		 **/
		static inline int32_t			CmpOrdinalCi( const std::wstring &_wsA, const std::wstring &_wsB ) {
			size_t sA = 0, sB = 0;
			const size_t sNa = _wsA.size(), sNb = _wsB.size();
			while ( sA < sNa && sB < sNb ) {
				wchar_t wcA = FoldCharBasicJa( _wsA[sA] );
				wchar_t wcB = FoldCharBasicJa( _wsB[sB] );
				if ( wcA < wcB ) { return -1; }
				if ( wcA > wcB ) { return 1; }
				++sA; ++sB;
			}
			if ( sA < sNa ) { return 1; }
			if ( sB < sNb ) { return -1; }
			return 0;
		}

		/**
		 * Natural-order comparison (case-insensitive).
		 * 
		 * Numeric runs (ASCII and fullwidth digits) are compared by numeric value with a tie-break on
		 * leading zeros. Non-numeric segments compare via CmpOrdinalCi().
		 * 
		 * \param _wsA Left string.
		 * \param _wsB Right string.
		 * \return Returns -1 if _wsA < _wsB, 1 if _wsA > _wsB, or 0 if equal.
		 **/
		static inline int32_t			CmpNaturalCi( const std::wstring &_wsA, const std::wstring &_wsB ) {
			size_t sA = 0, sB = 0;
			const size_t sNa = _wsA.size(), sNb = _wsB.size();

			while ( sA < sNa && sB < sNb ) {
				wchar_t wcA = FoldCharBasicJa( _wsA[sA] );
				wchar_t wcB = FoldCharBasicJa( _wsB[sB] );

				const bool bDa = IsDigit( wcA );
				const bool bDb = IsDigit( wcB );

				if ( bDa && bDb ) {
					// Scan digit runs.
					size_t sSa = sA, sSb = sB;
					while ( sA < sNa && IsDigit( FoldCharBasicJa( _wsA[sA] ) ) ) { ++sA; }
					while ( sB < sNb && IsDigit( FoldCharBasicJa( _wsB[sB] ) ) ) { ++sB; }

					// Strip leading zeros.
					size_t sZa = sSa;
					while ( sZa < sA && DigitValWide( FoldCharBasicJa( _wsA[sZa] ) ) == 0 ) { ++sZa; }
					size_t sZb = sSb;
					while ( sZb < sB && DigitValWide( FoldCharBasicJa( _wsB[sZb] ) ) == 0 ) { ++sZb; }

					const size_t sLenA = sA - sZa;
					const size_t sLenB = sB - sZb;

					if ( sLenA != sLenB ) {
						return (sLenA < sLenB) ? -1 : 1;
					}

					// Same effective length: compare digit-by-digit.
					for ( size_t I = 0; I < sLenA; ++I ) {
						const int32_t i32A = DigitValWide( FoldCharBasicJa( _wsA[sZa + I] ) );
						const int32_t i32B = DigitValWide( FoldCharBasicJa( _wsB[sZb + I] ) );
						if ( i32A < i32B ) { return -1; }
						if ( i32A > i32B ) { return 1; }
					}

					// Numeric value equal: fewer leading zeros wins (e.g., "2" < "02").
					const size_t sLzA = sZa - sSa;
					const size_t sLzB = sZb - sSb;
					if ( sLzA != sLzB ) { return (sLzA < sLzB) ? -1 : 1; }

					// Equal numeric tokens; continue.
					continue;
				}

				// Non-numeric path: ordinal folded compare of single code units.
				if ( wcA < wcB ) { return -1; }
				if ( wcA > wcB ) { return 1; }
				++sA; ++sB;
			}

			if ( sA < sNa ) { return 1; }
			if ( sB < sNb ) { return -1; }
			return 0;
		}

		/**
		 * Unified text comparison entry point.
		 * 
		 * Selects either ordinal case-insensitive or natural-order comparison based on _tpPolicy.
		 * 
		 * \param _wsA Left string.
		 * \param _wsB Right string.
		 * \param _tpPolicy Text comparison policy (see LSN_TEXTPOLICY).
		 * \return Returns -1 if _a < _b, 1 if _a > _b, or 0 if equal.
		 **/
		static inline uint32_t			CmpText( const std::wstring &_wsA, const std::wstring &_wsB, EE_TEXTPOLICY _tpPolicy ) {
			if ( _tpPolicy == EE_TP_NATURAL_CI ) { return CmpNaturalCi( _wsA, _wsB ); }
			else { return CmpOrdinalCi( _wsA, _wsB ); }
		}

		/**
		 * Decodes a single escape sequence from a character buffer.
		 *
		 * \param _pcValue	Pointer to the input character sequence (may include escape markers).
		 * \param _sLen		Length of the input buffer available.
		 * \param _sSize	Output: number of characters consumed from the input buffer.
		 * \return			The decoded wide character, or 0 if decoding fails.
		 */
		static inline wchar_t			DecodeEscape( const char * _pcValue, size_t _sLen, size_t &_sSize ) {
			struct {
				// The actual character to find.
				wchar_t				cEscape;
				// The character to write to the string.
				wchar_t				cValue;
			} sEscapes[] = {
				{ 'a', '\a' },
				{ 'b', '\b' },
				{ 'f', '\f' },
				{ 'n', '\n' },
				{ 'r', '\r' },
				{ 't', '\t' },
				{ 'v', '\v' },
				{ '\\', '\\' },
				{ '\'', '\'' },
				{ '"', '\"' },
				{ '?', '\?' },
				{ ' ', ' ' },
			};

			if ( _sLen == 0 ) {
				_sSize = 0;
				return static_cast<wchar_t>(0);
			}
			// If the length is 1 or the next character is not a back slash, the next character is the return.
			_sSize = 1;
			if ( _sLen == 1 || _pcValue[0] != static_cast<wchar_t>('\\') ) {
				return _pcValue[0];
			}
			// Possible sequence here.
			switch ( _pcValue[1] ) {
				case 'x' : {
					// Hex sequence.  Decode.
					if ( _sLen == 2 ) {
						return _pcValue[0];
					}
					wchar_t cReturn = 0;
					wchar_t cThis;
					// Decode the hex characters (maximum of 4).
					for ( size_t I = 2; I < _sLen && I < 6; I++ ) {
						if ( _pcValue[I] >= static_cast<wchar_t>('0') && _pcValue[I] <= static_cast<wchar_t>('9') ) {
							cThis = _pcValue[I] - static_cast<wchar_t>('0');
						}
						else if ( _pcValue[I] >= static_cast<wchar_t>('A') && _pcValue[I] <= static_cast<wchar_t>('F') ) {
							cThis = _pcValue[I] - static_cast<wchar_t>('A') + 0xA;
						}
						else if ( _pcValue[I] >= static_cast<wchar_t>('a') && _pcValue[I] <= static_cast<wchar_t>('f') ) {
							cThis = _pcValue[I] - static_cast<wchar_t>('a') + 0xA;
						}
						else {
							// No characters were eaten?
							if ( I == 2 ) { return _pcValue[0]; }

							// Eat the x.
							++_sSize;
							return cReturn;
						}
						cReturn <<= 4;
						cReturn |= cThis;
						++_sSize;
					}
					// Eat the x.
					++_sSize;
					return cReturn;
				}
				default : {
					if ( _pcValue[1] >= '0' && _pcValue[1] <= '7' ) {
						// Decode octal sequences.
						wchar_t cReturn = 0;
						wchar_t cThis;
						for ( size_t I = 1; I < _sLen && I < 4; I++ ) {
							if ( _pcValue[I] >= '0' && _pcValue[I] <= '7' ) {
								cThis = _pcValue[I] - '0';
							}
							else {
								// No characters were eaten?
								if ( I == 1 ) { return _pcValue[0]; }
								return cReturn;
							}
							cReturn <<= 3;
							cReturn |= cThis;
							++_sSize;
						}
						return cReturn;
					}
					else {
						for ( size_t J = 0; sEscapes[J].cEscape != ' '; J++ ) {
							if ( _pcValue[1] == sEscapes[J].cEscape ) {
								++_sSize;
								return sEscapes[J].cValue;
							}
						}
					}
				}
			}
			return _pcValue[0];
		}

		/**
		 * Decodes all escape sequences in a C-string into a raw std::string.
		 *
		 * \param _pcValue	Pointer to the null-terminated C-string containing escape sequences.
		 * \param _sLen		Length of the input buffer (excluding null-terminator).
		 * \return			A std::string with all escapes decoded.
		 */
		static inline std::string		DecodeEscapes( const char * _pcValue, size_t _sLen ) {
			std::string sRet;
			size_t ui32Size;
			for ( size_t I = 0; I < _sLen; I += ui32Size ) {
				sRet.push_back( static_cast<char>(DecodeEscape( &_pcValue[I], _sLen - I, ui32Size )) );
			}
			return sRet;
		}

		/**
		 * Decodes all escape sequences in a std::string into raw values.
		 *
		 * \param _sInput	The input std::string containing escape sequences.
		 * \return			A new std::string with escapes decoded.
		 */
		static inline std::string		DecodeEscapes( const std::string &_sInput ) {
			return DecodeEscapes( _sInput.c_str(), _sInput.size() );
		}

		/**
		 * Removes leading and trailing quote characters from a C-string.
		 *
		 * \param _pcValue	Pointer to the input C-string (including quotes).
		 * \param _sLen		Length of the input buffer (including quotes).
		 * \return			A std::string with the surrounding quotes removed,
		 *					or an empty string if length <= 2.
		 */
		static inline std::string		RemoveQuotes( const char * _pcValue, size_t _sLen ) {
			if ( _sLen <= 2 ) { return std::string(); }
			std::string sRet = ++_pcValue;
			sRet.pop_back();
			return sRet;
		}

		/**
		 * Removes all occurrences of a specific character from a string.
		 *
		 * \tparam _tStringType		Type of the string (e.g., std::string or std::wstring).
		 * \param _sInput			The string to modify (in-place).
		 * \param _ui32RemoveMe		The character code to remove.
		 * \return					Reference to the modified input string.
		 */
		template <typename _tStringType = std::string>
		static inline _tStringType &	RemoveChar( _tStringType &_sInput, uint32_t _ui32RemoveMe ) {
			for ( auto I = _sInput.size(); I--; ) {
				if ( _sInput[I] == static_cast<_tStringType::value_type>(_ui32RemoveMe) ) {
					_sInput.erase( I, 1 );
				}
			}
			return _sInput;
		}

		/**
		 * Trims whitespace from both ends of a given string.
		 *
		 * \tparam _tType	Type of the string (e.g., std::string).
		 * \param _sString	The input string to trim.
		 * \return			A new string with leading and trailing whitespace removed.
		 */
		template <typename _tType = std::string>
		static _tType					TrimWhitespace( const _tType &_sString ) {
			_tType sCopy = _sString;
			while ( sCopy.size() && IsWhiteSpace( sCopy[sCopy.size()-1] ) ) {
				sCopy.pop_back();
			}
			// Count the opening whitespace.
			size_t stOpening = 0;
			while ( sCopy.size() && IsWhiteSpace( sCopy[stOpening] ) ) {
				++stOpening;
			}
			if ( stOpening ) {
				sCopy.erase( 0, stOpening );
			}
			return sCopy;
		}

		/**
		 * Counts the number of valid hexadecimal characters from a starting position in a C-string.
		 *
		 * \param _pcValue		Pointer to the input buffer.
		 * \param _sLen			Total length of the buffer.
		 * \param _sCntFromPos	Index from which to start counting.
		 * \return				Number of consecutive hex digits found.
		 */
		static inline size_t			CountHexChars( const char * _pcValue, size_t _sLen, size_t _sCntFromPos ) {
			size_t sRet = 0;
			for ( ; _sCntFromPos < _sLen; ++_sCntFromPos ) {
				if ( ValidHex( _pcValue[_sCntFromPos] ) ) { ++sRet; }
				else { break; }
			}
			return sRet;
		}

		/**
		 * Counts the number of valid octal characters from a starting position in a C-string.
		 *
		 * \param _pcValue		Pointer to the input buffer.
		 * \param _sLen			Total length of the buffer.
		 * \param _sCntFromPos	Index from which to start counting.
		 * \return				Number of consecutive octal digits found.
		 */
		static inline size_t			CountOctalChars( const char * _pcValue, size_t _sLen, size_t _sCntFromPos ) {
			size_t sRet = 0;
			for ( ; _sCntFromPos < _sLen; ++_sCntFromPos ) {
				if ( ValidOctal( _pcValue[_sCntFromPos] ) ) { ++sRet; }
				else { break; }
			}
			return sRet;
		}

		/**
		 * Counts the number of alphanumeric or underscore characters from a starting position.
		 *
		 * \param _pcValue		Pointer to the input buffer.
		 * \param _sLen			Total length of the buffer.
		 * \param _sCntFromPos	Index from which to start counting.
		 * \return				Number of consecutive alphanumeric or underscore characters.
		 */
		static inline size_t			CountAlphanumeric( const char * _pcValue, size_t _sLen, size_t _sCntFromPos ) {
			size_t sRet = 0;
			for ( ; _sCntFromPos < _sLen; ++_sCntFromPos ) {
				if ( (_pcValue[_sCntFromPos] >= 'a' && _pcValue[_sCntFromPos] <= 'z') ||
					(_pcValue[_sCntFromPos] >= 'A' && _pcValue[_sCntFromPos] <= 'Z') ||
					(_pcValue[_sCntFromPos] >= '0' && _pcValue[_sCntFromPos] <= '9') ||
					_pcValue[_sCntFromPos] == '_' ) { ++sRet; }
				else { break; }
			}
			return sRet;
		}

		/**
		 * Parses a hexadecimal escape sequence ("xNNNN") into a uint32_t value.
		 *
		 * \param _pcValue				Pointer to the input sequence (starting at 'x').
		 * \param _sLen					Length of the buffer.
		 * \param _sCharsConsumed		Output: number of characters consumed (including 'x').
		 * \param _sMaxAllowedHexChars	Maximum hex digits to parse (default 4).
		 * \return						Parsed numeric value, or 0 if invalid.
		 */
		static inline uint32_t			EscapeX( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed, size_t _sMaxAllowedHexChars = 4 ) {
			_sCharsConsumed = 0;
			if ( _sLen >= 2 && _pcValue[0] == 'x' ) { // 'x' and 1 or more hex characters.
				size_t sHexChars = CountHexChars( _pcValue, _sLen, 1 );
				if ( sHexChars >= 1 ) {
					sHexChars = sHexChars < _sMaxAllowedHexChars ? sHexChars : _sMaxAllowedHexChars;
					size_t sPos = 1;
					uint32_t ui32Ret = 0;
					for ( size_t I = 0; I < sHexChars; ++I ) {
						char cTemp = _pcValue[sPos++];
						ui32Ret <<= 4;
						ui32Ret |= HexToUint32( cTemp );
					}
					_sCharsConsumed = sHexChars + 1;
					return ui32Ret;
				}
			}
			return 0;
		}

		/**
		 * Parses an octal escape sequence into a uint32_t value.
		 *
		 * \param _pcValue					Pointer to the input buffer.
		 * \param _sLen						Length of the buffer.
		 * \param _sCharsConsumed			Output: number of octal digits consumed.
		 * \param _sMaxAllowedOctalChars	Maximum octal digits to parse (default 4).
		 * \return							Parsed octal value, or 0 if invalid.
		 */
		static inline uint32_t			EscapeOctal( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed, size_t _sMaxAllowedOctalChars = 4 ) {
			_sCharsConsumed = 0;
			if ( _sLen >= 1 ) {
				size_t sOctalChars = CountOctalChars( _pcValue, _sLen, 0 );
				if ( sOctalChars >= 1 && sOctalChars <= _sMaxAllowedOctalChars ) {
					sOctalChars = sOctalChars < _sMaxAllowedOctalChars ? sOctalChars : _sMaxAllowedOctalChars;
					size_t sPos = 0;
					uint32_t ui32Ret = 0;
					for ( size_t I = 0; I < sOctalChars; ++I ) {
						char cTemp = _pcValue[sPos++];
						ui32Ret <<= 3;
						ui32Ret |= OctalToUint32( cTemp );
					}
					_sCharsConsumed = sOctalChars;
					return ui32Ret;
				}
			}

			return 0;
		}

		/**
		 * Parses a 4-digit Unicode escape ("uNNNN") into a uint32_t value (no surrogate pairs).
		 *
		 * \param _pcValue			Pointer starting at 'u'.
		 * \param _sLen				Buffer length.
		 * \param _sCharsConsumed	Output: 5 characters consumed (\uNNNN).
		 * \return					Parsed Unicode codepoint, or 0 if invalid.
		 */
		static inline uint32_t			EscapeUnicodeWide4( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed ) {
			_sCharsConsumed = 0;
			if ( _sLen >= (4 + 1) && _pcValue[0] == 'u' ) {
				const size_t sHexChars = CountHexChars( _pcValue, _sLen, 1 );
				if ( sHexChars >= 4 ) {
					size_t sPos = 1;
					uint32_t ui32Ret = 0;
					for ( size_t I = 0; I < 4; ++I ) {
						char cTemp = _pcValue[sPos++];
						ui32Ret <<= 4;
						ui32Ret |= HexToUint32( cTemp );
					}
					_sCharsConsumed = 4 + 1;
					return ui32Ret;
				}
			}
			return 0;
		}

		/**
		 * Parses a Unicode escape sequence with surrogate pair support ("uNNNN"\uNNNN).
		 *
		 * \param _pcValue			Pointer starting at first 'u'.
		 * \param _sLen				Buffer length.
		 * \param _sCharsConsumed	Output: total characters consumed across both escapes.
		 * \return					Parsed Unicode codepoint, or first half if no valid pair.
		 */
		static inline uint32_t			EscapeUnicodeWide4WithSurrogatePairs( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed ) {
			uint32_t uiLeft = EscapeUnicodeWide4( _pcValue, _sLen, _sCharsConsumed );
			if ( !_sCharsConsumed ) { return uiLeft; }

			if ( (uiLeft & 0xFC00) == 0xD800 ) {					// In order to be a surrogate pair.
				_pcValue += _sCharsConsumed;
				_sLen -= _sCharsConsumed;
				if ( _sLen >= (1 + 1 + 4) ) {						// '\' + 'u' + NNNN.
					if ( _pcValue[0] == '\\' ) {
						++_pcValue;
						--_sLen;
						size_t sConsumedRight;
						uint32_t uiRight = EscapeUnicodeWide4( _pcValue, _sLen, sConsumedRight );

						if ( (uiRight & 0xFC00) == 0xDC00 ) {		// 2nd half of surrogate pair must be valid.
							uint32_t ui32H = uiLeft & 0x03FF;
							uint32_t ui32L = uiRight & 0x03FF;
							uiLeft = 0x10000 + (ui32H << 10) + ui32L;
							_sCharsConsumed = _sCharsConsumed + 1 + sConsumedRight;
						}
					}
				}
			}
			return uiLeft;
		}

		// Convert a \U******** Unicode character to a uint32_t.
		static inline uint32_t			EscapeUnicode8( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed ) {
			_sCharsConsumed = 0;
			if ( _sLen >= (8 + 1) && _pcValue[0] == 'U' ) {
				const size_t sHexChars = CountHexChars( _pcValue, _sLen, 1 );
				if ( sHexChars == 8 ) {
					size_t sPos = 1;
					uint32_t ui32Ret = 0;
					for ( size_t I = 0; I < sHexChars; ++I ) {
						char cTemp = _pcValue[sPos++];
						ui32Ret <<= 4;
						ui32Ret |= HexToUint32( cTemp );
					}
					_sCharsConsumed = sHexChars + 1;
					return ui32Ret;
				}
			}
			return 0;
		}

		// Converts a \N{*} named Unicode character to a uint32_t.
		static uint32_t					EscapeNamedUnicode( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed );

		// Converts an &#nnnn; or an &#xhhhh; HTML character to a uint64_t.
		static uint64_t					EscapeHtml( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed );

		// Escapes double quotes in a string (" -> \").
		template <typename _tType = std::string>
		static _tType					EscapeQuotes( const _tType &_sInput, bool _bEscapeSlashes ) {
			_tType sRet;
			for ( size_t I = 0; I < _sInput.size(); ++I ) {
				if ( _sInput[I] == _tType::value_type( '\"' ) ) {
					sRet.push_back( _tType::value_type( '\\' ) );
				}
				else if ( _bEscapeSlashes && _sInput[I] == _tType::value_type( '\\' ) ) {
					sRet.push_back( _tType::value_type( '\\' ) );
				}
				sRet.push_back( _sInput[I] );
			}
			return sRet;
		}

		/**
		 * Gets the next UTF-32 character from a stream or error (EE_UTF_INVALID).
		 * 
		 * \param _puiString	The string to parse.
		 * \param _sLen			The length of the string to which _puiString points.
		 * \param _psSize		Optional pointer to a size_t that will contain the number of characters eaten from _puiString during the parsing.
		 * \return				Returns the next character as a UTF-32 code.
		 **/
		static inline uint32_t			NextUtf32Char( const uint32_t * _puiString, size_t _sLen, size_t * _psSize = nullptr ) {
			if ( _sLen == 0 ) {
				if ( _psSize ) { (*_psSize) = 0; }
				return 0;
			}
			if ( _psSize ) { (*_psSize) = 1; }
			uint32_t ui32Ret = (*_puiString);
			if ( ui32Ret & 0xFFE00000 ) { return EE_UTF_INVALID; }
			return ui32Ret;
		}

		/**
		 * Gets the next UTF-16 character from a stream or error (EE_UTF_INVALID).
		 * 
		 * \param _pwcString	The string to parse.
		 * \param _sLen			The length of the string to which _pwcString points.
		 * \param _psSize		Optional pointer to a size_t that will contain the number of characters eaten from _pwcString during the parsing.
		 * \return				Returns the next character as a UTF-32 code.
		 **/
		static uint32_t					NextUtf16Char( const wchar_t * _pwcString, size_t _sLen, size_t * _psSize = nullptr );

		/**
		 * Gets the next UTF-8 character from a stream or error (EE_UTF_INVALID).
		 * 
		 * \param _pcString		The string to parse.
		 * \param _sLen			The length of the string to which _pcString points.
		 * \param _psSize		Optional pointer to a size_t that will contain the number of characters eaten from _pcString during the parsing.
		 * \return				Returns the next character as a UTF-32 code.
		 **/
		static uint32_t					NextUtf8Char( const char * _pcString, size_t _sLen, size_t * _psSize = nullptr );

		/**
		 * Gets the size of the given UTF-8 character.
		 * 
		 * \param _pcString		Pointer to the UTF-8 characters to decode.
		 * \param _sLen			The number of characters to which _pcString points.
		 * \return				Returns the size of the UTF-8 character to which _pcString points.
		 **/
		static size_t					Utf8CharSize( const char * _pcString, size_t _sLen );

		/**
		 * Converts a UTF-32 character to a UTF-16 character.
		 * 
		 * \param _ui32Utf32	The UTF-32 character to convert to UTF-16.
		 * \param _ui32Len		Holds the returned number of 16-bit characters held in the return value.
		 * \return				Returns up to 2 UTF-16 characters.
		 **/
		static uint32_t					Utf32ToUtf16( uint32_t _ui32Utf32, uint32_t &_ui32Len );

		/**
		 * Converts a UTF-32 character to a UTF-8 character.
		 * 
		 * \param _ui32Utf32	The UTF-32 character to convert to UTF-8.
		 * \param _ui32Len		Holds the returned number of 16-bit characters held in the return value.
		 * \return				Returns up to 4 UTF-8 characters.
		 **/
		static uint32_t					Utf32ToUtf8( uint32_t _ui32Utf32, uint32_t &_ui32Len );

		/**
		 * Converts a wstring to a UTF-8 string.  Converts the full length of the string, so embedded 0 characters don't stop the conversion.
		 * 
		 * \param _wsIn			The UTF-16 string to convert.
		 * \return				Returns the converted UTF-8 string, including any embedded 0's.
		 **/
		static std::string				WStringToString( const std::wstring &_wsIn ) { return WStringToString( _wsIn.c_str(), _wsIn.size() ); }

		/**
		 * Converts a wstring to a UTF-8 string.  Converts the full length of the string, so embedded 0 characters don't stop the conversion.
		 * 
		 * \param _pwcIn		The UTF-16 string to convert.
		 * \param _sLen			The number of characters to which _pwcIn points.
		 * \return				Returns the converted UTF-8 string, including any embedded 0's.
		 **/
		static std::string				WStringToString( const wchar_t * _pwcIn, size_t _sLen );

		/**
		 * Converts a UTF-8 string to wstring (UTF-16).  Converts the full length of the string, so embedded 0 characters don't stop the conversion.
		 * 
		 * \param _sIn			The UTF-8 string to convert.
		 * \return				Returns the converted UTF-16 string, including any embedded 0's.
		 **/
		static std::wstring				StringToWString( const std::string &_sIn ) { return StringToWString( _sIn.c_str(), _sIn.size() ); }

		/**
		 * Converts a UTF-8 string to wstring (UTF-16).  Converts the full length of the string, so embedded 0 characters don't stop the conversion.
		 * 
		 * \param _pcIn			The UTF-8 string to convert.
		 * \param _sLen			The number of characters to which _pcIn points.
		 * \return				Returns the converted UTF-16 string, including any embedded 0's.
		 **/
		static std::wstring				StringToWString( const char * _pcIn, size_t _sLen );

		/**
		 * Converts a UTF-32 string to a UTF-16 string.
		 * 
		 * \param _puiUtf32String	Pointer to the UTF-32 character array.
		 * \param _sLen				Number of code points to convert.
		 * \return					Returns the converted UTF-16 string.
		 **/
		static std::wstring				Utf32StringToWString( const uint32_t * _puiUtf32String, size_t _sLen );

		/**
		 * Gets the Nth Unicode code point in the given UTF-8 string.
		 * 
		 * \param _sIn			The UTF-8 string.
		 * \param _sIx			The zero-based index of the code point.
		 * \return				The Unicode code point at the specified index.
		 **/
		static uint32_t					GetUtf8CodePointByIdx( const std::string &_sIn, size_t _sIx );

		/**
		 * Gets the byte position of the Nth UTF-8 character in the string.
		 * 
		 * \param _sIn			The UTF-8 string.
		 * \param _sIx			The zero-based index of the UTF-8 character.
		 * \return				The byte position of the character.
		 **/
		static size_t					GetUtf8CharPosByIdx( const std::string &_sIn, size_t _sIx );

		/**
		 * Converts a UTF-16 wstring to UTF-8. If invalid characters are encountered, raw bytes are copied instead of using EE_UTF_INVALID.
		 * 
		 * \param _wsString		The UTF-16 wide string to convert.
		 * \return				The resulting UTF-8 string.
		 **/
		static std::string				ToUtf8( const std::wstring &_wsString );

		/**
		 * Converts a UTF-16 u16string to UTF-8. If invalid characters are encountered, raw bytes are copied instead of using EE_UTF_INVALID.
		 * 
		 * \param _u16String	The UTF-16 string to convert.
		 * \return				The resulting UTF-8 string.
		 **/
		static std::string				ToUtf8( const std::u16string &_u16String );

		/**
		 * Converts a UTF-32 u32string to UTF-8.
		 * 
		 * \param _u32String	The UTF-32 string to convert.
		 * \return				The resulting UTF-8 string.
		 **/
		static std::string				ToUtf8( const std::u32string &_u32String );

		/**
		 * Converts a UTF-8 string to UTF-16. If invalid characters are encountered, the original bytes are copied instead of substituting EE_UTF_INVALID.
		 * 
		 * \tparam _tOutType	The output UTF-16 string type (e.g. std::wstring).
		 * \param _sIn			The UTF-8 string to convert.
		 * \return				The resulting UTF-16 string.
		 **/
		template <typename _tOutType = std::wstring>
		static inline _tOutType			ToUtf16( const std::string &_sIn ) {
			_tOutType otTmp;
			const char * pcSrc = _sIn.c_str();
			size_t sSize = _sIn.size();

			for ( size_t I = 0; I < sSize; ) {
				size_t sThisSize = 0;
				uint32_t ui32This = NextUtf8Char( &pcSrc[I], _sIn.size() - I, &sThisSize );
				if ( ui32This == EE_UTF_INVALID ) {
					for ( size_t J = 0; J < sThisSize; ++J ) {
						otTmp.push_back( static_cast<uint8_t>(pcSrc[I+J]) );
					}
					I += sThisSize;
					continue;
				}
				I += sThisSize;
				uint32_t ui32Len;
				uint32_t ui32Converted = Utf32ToUtf16( ui32This, ui32Len );
				for ( uint32_t J = 0; J < ui32Len; ++J ) {
					otTmp.push_back( static_cast<_tOutType::value_type>(ui32Converted & 0xFFFFU) );
					ui32Converted >>= 16;
				}
			}
			return otTmp;
		}

		/**
		 * Converts a UTF-32 string to UTF-16. If invalid characters are encountered, the original bytes are copied instead of substituting EE_UTF_INVALID.
		 * 
		 * \tparam _tOutType	The output UTF-16 string type (e.g. std::wstring).
		 * \param _u32String	The UTF-32 string to convert.
		 * \return				The resulting UTF-16 string.
		 **/
		template <typename _tOutType = std::wstring>
		static inline _tOutType			ToUtf16( const std::u32string &_u32String ) {
			_tOutType swsTemp;
			size_t sSize = _u32String.size();

			for ( size_t I = 0; I < sSize; ) {
				size_t sThisSize = 0;
				uint32_t ui32This = NextUtf32Char( reinterpret_cast<const uint32_t *>(&_u32String.data()[I]), _u32String.size() - I, &sThisSize );
				if ( ui32This == EE_UTF_INVALID ) {
					for ( size_t J = 0; J < sThisSize * sizeof( std::u32string::value_type ); ++J ) {
						swsTemp.push_back( reinterpret_cast<const _tOutType::value_type *>(&_u32String.data()[I])[J] );
					}
					I += sThisSize;
					continue;
				}
				I += sThisSize;
				uint32_t ui32Len;
				uint32_t ui32Converted = Utf32ToUtf16( ui32This, ui32Len );
				for ( uint32_t J = 0; J < ui32Len; ++J ) {
					swsTemp.push_back( static_cast<_tOutType::value_type>(ui32Converted & 0xFFFFU) );
					ui32Converted >>= 16;
				}
			}
			return swsTemp;
		}

		/**
		 * Converts a UTF-8 string to UTF-32. If invalid characters are encountered, the original bytes are copied instead of substituting EE_UTF_INVALID.
		 * 
		 * \param _sIn			The UTF-8 string to convert.
		 * \return				The resulting UTF-32 string.
		 **/
		static std::u32string			ToUtf32( const std::string &_sIn );

		/**
		 * Converts a UTF-8 string to ASCII by stripping or replacing non-ASCII characters.
		 * 
		 * \param _sIn			The UTF-8 string to convert.
		 * \return				The resulting ASCII string.
		 **/
		static std::string				ToAscii( const std::string &_sIn );

		/**
		 * Converts a UTF-8 string to a null-terminated C string.
		 * 
		 * \param _sIn			The UTF-8 string to convert.
		 * \return				The resulting C string (still std::string but usable as .c_str()).
		 **/
		static std::string				ToCString( const std::string &_sIn );

		/**
		 * Converts a UTF-8 string to a JSON string.
		 * 
		 * \param _sIn			The UTF-8 string to convert.
		 * \return				The resulting JSON string (still std::string but usable as .c_str()).
		 **/
		static std::string				ToJsonString( const std::string &_sIn );

		/**
		 * Represents a value in binary notation.
		 * 
		 * \param _ui64Val	The value to print.
		 * \return			Returns the printed value.
		 **/
		static std::string				ToBinary( uint64_t _ui64Val );

		/**
		 * Represents a value in binary notation.
		 * 
		 * \param _i64Val	The value to print.
		 * \return			Returns the printed value.
		 **/
		static std::string				ToBinary( int64_t _i64Val );

		/**
		 * Represents a value in binary notation.
		 * 
		 * \param _d4Val	The value to print.
		 * \return			Returns the printed value.
		 **/
		static std::string				ToBinary( double _d4Val );

		/**
		 * Represents a value in binary notation.
		 * 
		 * \param _ui64Val		The value to print.
		 * \param _i32Digits	The number of digits to print, or a number less than 1 to print only the required number of digits.
		 * \return				Returns the printed value.
		 **/
		static std::string				ToBinary( uint64_t _ui64Val, int32_t _i32Digits );

		/**
		 * Takes an input array of bytes, converts each value to a 2-digit hexadecimal value, and outputs the converted string with optional spaces between each value.
		 * 
		 * \tparam _tType The vector/string type.  The contained type must be 1 byte in size.
		 * \param _tIn The input array of bytes.
		 * \param _bSpaces If true, spaces will be added between each output hex value.
		 * \param _pbSuccess An optional pointer to receive the success/failure status.  Failure always means there was a memory failure.
		 * \return Returns the array of bytes as a string of hexadecimal bytes.
		 **/
		template <typename _tType>
		static inline std::string		ToHexString( const _tType &_tIn, bool _bSpaces = true, bool * _pbSuccess = nullptr ) {
			static_assert( sizeof( typename _tType::value_type ) == 1U, "_tType::value_type must be 1 byte." );
			using _tVal = typename _tType::value_type;

			std::string sStr;
			if ( _pbSuccess ) { (*_pbSuccess) = false; }
			try {
				std::stringstream ssStream;
				ssStream.setf( std::ios::hex, std::ios::basefield );
				ssStream.setf( std::ios::uppercase );
				ssStream.setf( std::ios::fmtflags( 0 ), std::ios::showbase ); // Ensure no 0x.
				ssStream.fill( '0' );

				for ( size_t I = 0; I < _tIn.size(); ++I ) {
					if ( _bSpaces && I ) { ssStream << ' '; }
					const unsigned int uiVal = static_cast<unsigned int>(static_cast<uint8_t>(_tIn[I]));
					ssStream << std::setw( 2 ) << uiVal;
				}

				sStr = ssStream.str();
			}
			catch ( ... ) { return sStr; }
			if ( _pbSuccess ) { (*_pbSuccess) = true; }
			return sStr;
		}

		/**
		 * Takes a hex string (created by ToHexString()) and creates a container of a 1-byte type.
		 * 
		 * \param _sString The input hex string.
		 * \param _pbSuccess An optional pointer to receive the success/failure status.  Failure always means there was a memory failure.
		 * \return Returns the raw bytes encoded into the string.
		 **/
		template <typename _tOutType, typename _tStrType>
		static inline _tOutType			FromHexString( const _tStrType &_sString, bool * _pbSuccess = nullptr ) {
			static_assert( sizeof( typename _tOutType::value_type ) == 1U, "_tOutType::value_type must be 1 byte." );
			using _tInVal = typename _tStrType::value_type;

			_tOutType otOutput;
			if ( _pbSuccess ) { (*_pbSuccess) = false; }
			try {
				uint8_t ui8Byte = 0;
				bool bCnt = false;
				for ( size_t I = 0; I < _sString.size(); ++I ) {
					if constexpr ( sizeof( _tInVal ) != 1U ) {
						if ( _sString[I] > UINT8_MAX ) { continue; }
					}
					char cThis = char( _sString[I] );
					if ( ValidHex( cThis ) ) {
						ui8Byte <<= 4;
						ui8Byte = ui8Byte & 0xF0;
						ui8Byte |= HexToUint32( cThis );
						bCnt = !bCnt;
						if ( !bCnt ) {
							otOutput.push_back( _tOutType::value_type( ui8Byte ) );
						}
					}
				}
			}
			catch ( ... ) { return otOutput; }
			if ( _pbSuccess ) { (*_pbSuccess) = true; }
			return otOutput;
		}

		/**
		 * Represents a value in hexadecimal notation.
		 * 
		 * \param _ui64Val	The value to print.
		 * \return			Returns the printed value.
		 **/
		static std::string				ToHex( uint64_t _ui64Val );

		/**
		 * Represents a value in hexadecimal notation.
		 * 
		 * \param _i64Val	The value to print.
		 * \return			Returns the printed value.
		 **/
		static std::string				ToHex( int64_t _i64Val );

		/**
		 * Represents a value in hexadecimal notation.
		 * 
		 * \param _d4Val	The value to print.
		 * \return			Returns the printed value.
		 **/
		static std::string				ToHex( double _d4Val );

		/**
		 * Represents a value in octal notation.
		 * 
		 * \param _ui64Val		The unsigned 64-bit value to convert.
		 * \return				The value as an octal-formatted string.
		 **/
		static std::string				ToOct( uint64_t _ui64Val );

		/**
		 * Represents a value in octal notation.
		 * 
		 * \param _i64Val		The signed 64-bit value to convert.
		 * \return				The value as an octal-formatted string.
		 **/
		static std::string				ToOct( int64_t _i64Val );

		/**
		 * Represents a double-precision floating-point value in octal notation.
		 * 
		 * \param _d4Val		The double value to convert.
		 * \return				The value as an octal-formatted string.
		 **/
		static std::string				ToOct( double _d4Val );

		/**
		 * Classifies a string as one of the EE_NUM_CONSTANTS types. Determines whether the string represents a valid floating-point,
		 * signed integer, unsigned integer, or is invalid.
		 * 
		 * \param _sIn				The input string to classify.
		 * \param _pui8SpecialBase	Optional pointer to receive the detected numeric base (e.g., 8, 10, 16).
		 * \return					Returns the classification as an EE_STRING_NUMBER_CLASS enum value.
		 **/
		static EE_STRING_NUMBER_CLASS	ClassifyString( const std::string &_sIn, uint8_t * _pui8SpecialBase = nullptr );

		/**
		 * Counts the number of set (non-zero) bytes in a 64-bit unsigned value.
		 * 
		 * \param _ui64Value		The value to analyze.
		 * \return					The number of non-zero bytes.
		 **/
		static inline uint32_t			CountSetBytes( uint64_t _ui64Value ) {
			uint32_t uiRet = 0;
			while ( _ui64Value ) {
				++uiRet;
				_ui64Value >>= 8;
			}
			return uiRet;
		}

		/**
		 * Counts the number of bits set to 1 in a 64-bit unsigned value.
		 * 
		 * \param _ui64Value		The value to analyze.
		 * \return					The total count of bits set to 1.
		 **/
		static inline uint32_t			CountSetBits( uint64_t _ui64Value ) {
			uint32_t ui32Cnt = 0;
			while ( _ui64Value ) {
				_ui64Value &= (_ui64Value - 1);
				ui32Cnt++;
			}
			return ui32Cnt;
		}

		/**
		 * Finds the index of the highest set bit in a 64-bit unsigned value.
		 * 
		 * \param _ui64Value		The value to examine.
		 * \return					The zero-based index of the most significant bit set to 1. Returns 0 if no bits are set.
		 **/
		static inline uint32_t			HighestSetBit( uint64_t _ui64Value ) {
			unsigned long ulPos;
#if defined( _AMD64_ ) && (defined(_M_AMD64) && !defined(RC_INVOKED) && !defined(MIDL_PASS))
			if ( _BitScanReverse64( &ulPos, _ui64Value ) ) {
				return static_cast<uint32_t>(ulPos);
			}

#else
			unsigned long ulPosHi;
			if ( _BitScanReverse( &ulPosHi, static_cast<unsigned long>(_ui64Value >> 32ULL) ) ) {
				return ulPosHi + 32UL;
			}
			if ( _BitScanReverse( &ulPos, static_cast<unsigned long>(_ui64Value) ) ) {
				return ulPos;
			}
#endif	// #if defined( _AMD64_ ) && (defined(_M_AMD64) && !defined(RC_INVOKED) && !defined(MIDL_PASS))
			return 0;
		}

		/**
		 * Reverses the bits in an 8-bit value.
		 * 
		 * \param _ui8Val	The value to bit-reverse.
		 * \return			Returns the bit-reversed result.
		 **/
		static inline uint8_t			ReverseBits8( uint8_t _ui8Val ) {
			_ui8Val = (_ui8Val & 0xF0) >> 4 | (_ui8Val & 0x0F) << 4;
			_ui8Val = (_ui8Val & 0xCC) >> 2 | (_ui8Val & 0x33) << 2;
			_ui8Val = (_ui8Val & 0xAA) >> 1 | (_ui8Val & 0x55) << 1;
			return _ui8Val;
		}

		/**
		 * Reverses the bits in an 16-bit value.
		 * 
		 * \param _ui16Val	The value to bit-reverse.
		 * \return			Returns the bit-reversed result.
		 **/
		static inline uint16_t			ReverseBits16( uint16_t _ui16Val ) {
			_ui16Val = ((_ui16Val >> 1) & 0x5555) | ((_ui16Val & 0x5555) << 1);
			_ui16Val = ((_ui16Val >> 2) & 0x3333) | ((_ui16Val & 0x3333) << 2);
			_ui16Val = ((_ui16Val >> 4) & 0x0F0F) | ((_ui16Val & 0x0F0F) << 4);
			_ui16Val = ((_ui16Val >> 8) & 0x00FF) | ((_ui16Val & 0x00FF) << 8);
			return _ui16Val;
		}

		/**
		 * Reverses the bits in an 32-bit value.
		 * 
		 * \param _ui32Val	The value to bit-reverse.
		 * \return			Returns the bit-reversed result.
		 **/
		static inline uint32_t			ReverseBits32( uint32_t _ui32Val ) {
			_ui32Val = ((_ui32Val >> 1) & 0x55555555U) | ((_ui32Val & 0x55555555U) << 1);
			_ui32Val = ((_ui32Val >> 2) & 0x33333333U) | ((_ui32Val & 0x33333333U) << 2);
			_ui32Val = ((_ui32Val >> 4) & 0x0F0F0F0FU) | ((_ui32Val & 0x0F0F0F0FU) << 4);
			_ui32Val = ((_ui32Val >> 8) & 0x00FF00FFU) | ((_ui32Val & 0x00FF00FFU) << 8);
			_ui32Val = ((_ui32Val >> 16) & 0xFFFFU) | ((_ui32Val & 0xFFFFU) << 16);
			return _ui32Val;
		}

		/**
		 * Reverses the bits in an 64-bit value.
		 * 
		 * \param _ui64Val	The value to bit-reverse.
		 * \return			Returns the bit-reversed result.
		 **/
		static inline uint64_t			ReverseBits64( uint64_t _ui64Val ) {
			_ui64Val = ((_ui64Val >> 1) & 0x5555555555555555ULL) | ((_ui64Val & 0x5555555555555555ULL) << 1);
			_ui64Val = ((_ui64Val >> 2) & 0x3333333333333333ULL) | ((_ui64Val & 0x3333333333333333ULL) << 2);
			_ui64Val = ((_ui64Val >> 4) & 0x0F0F0F0F0F0F0F0FULL) | ((_ui64Val & 0x0F0F0F0F0F0F0F0FULL) << 4);
			_ui64Val = ((_ui64Val >> 8) & 0x00FF00FF00FF00FFULL) | ((_ui64Val & 0x00FF00FF00FF00FFULL) << 8);
			_ui64Val = ((_ui64Val >> 16) & 0x0000FFFF0000FFFFULL) | ((_ui64Val & 0x0000FFFF0000FFFFULL) << 16);
			_ui64Val = ((_ui64Val >> 32) & 0x00000000FFFFFFFFULL) | ((_ui64Val & 0x00000000FFFFFFFFULL) << 32);
			return _ui64Val;
		}
	
		/**
		 * Resolves escape sequences in a string and outputs the result as a UTF-32 character array.
		 * 
		 * Supported escape sequences:
		 *  \'         single quote							byte 0x27 in ASCII encoding  
		 *  \"         double quote                         byte 0x22 in ASCII encoding  
		 *  \?         question mark                        byte 0x3f in ASCII encoding  
		 *  \\         backslash                            byte 0x5c in ASCII encoding  
		 *  \a         audible bell                         byte 0x07 in ASCII encoding  
		 *  \b         backspace                            byte 0x08 in ASCII encoding  
		 *  \f         form feed - new page                 byte 0x0c in ASCII encoding  
		 *  \n         line feed - new line                 byte 0x0a in ASCII encoding  
		 *  \r         carriage return                      byte 0x0d in ASCII encoding  
		 *  \t         horizontal tab                       byte 0x09 in ASCII encoding  
		 *  \v         vertical tab                         byte 0x0b in ASCII encoding  
		 *  \nnn       arbitrary octal value                byte nnn  
		 *  \xnn       arbitrary hexadecimal value          byte nn  
		 *  \unnnn     universal character name             code point U+nnnn  
		 *  \Unnnnnnnn universal character name             code point U+nnnnnnnn  
		 *  \N{name}   Unicode character with the given name
		 * 
		 * \param _sInput      The input string containing escape sequences.
		 * \param _vOutput     The output vector that receives UTF-32 code points after escape resolution.
		 **/
		static void						ResolveAllEscapes( const std::string &_sInput, std::vector<uint32_t> &_vOutput );

		/**
		 * Resolves escape sequences in a string and outputs the result as either UTF-8 or byte-preserved encoding.
		 * 
		 * Supported escape sequences:
		 *  \'         single quote                         byte 0x27 in ASCII encoding  
		 *  \"         double quote                         byte 0x22 in ASCII encoding  
		 *  \?         question mark                        byte 0x3f in ASCII encoding  
		 *  \\         backslash                            byte 0x5c in ASCII encoding  
		 *  \a         audible bell                         byte 0x07 in ASCII encoding  
		 *  \b         backspace                            byte 0x08 in ASCII encoding  
		 *  \f         form feed - new page                 byte 0x0c in ASCII encoding  
		 *  \n         line feed - new line                 byte 0x0a in ASCII encoding  
		 *  \r         carriage return                      byte 0x0d in ASCII encoding  
		 *  \t         horizontal tab                       byte 0x09 in ASCII encoding  
		 *  \v         vertical tab                         byte 0x0b in ASCII encoding  
		 *  \nnn       arbitrary octal value                byte nnn  
		 *  \xnn       arbitrary hexadecimal value          byte nn  
		 *  \unnnn     universal character name             code point U+nnnn  
		 *  \Unnnnnnnn universal character name             code point U+nnnnnnnn  
		 *  \N{name}   Unicode character with the given name
		 * 
		 * \param _sInput      The input string containing escape sequences.
		 * \param _sOutput     The output string with resolved escapes, encoded as UTF-8 if _bIsUtf8 is true.
		 * \param _bIsUtf8     If true, encodes characters as UTF-8; otherwise encodes as raw bytes.
		 **/
		static void						ResolveAllEscapes( const std::string &_sInput, std::string &_sOutput, bool _bIsUtf8 );

		/**
		 * Resolves a single escape sequence from the input (e.g., \n, \x1B, or &#xNNNN;).
		 * If the input is not an escape sequence, returns the first character as-is.
		 * 
		 * \param _pcInput			Pointer to the character buffer to parse.
		 * \param _sLen				Length of the input buffer.
		 * \param _sCharLen			Reference to receive the number of characters consumed.
		 * \param _bIncludeHtml		If true, also resolves HTML/XML escape sequences.
		 * \param _pbEscapeFound	Optional pointer to receive whether an escape was found.
		 * \return					Returns the resolved character/code point.
		 **/
		static uint64_t					ResolveEscape( const char * _pcInput, size_t _sLen, size_t &_sCharLen, bool _bIncludeHtml, bool * _pbEscapeFound = nullptr );

		/**
		 * Resolves all HTML/XML escape sequences in the input string (e.g., &amp;, &#x20;).
		 * 
		 * \param _sInput			The input string to scan for escape sequences.
		 * \param _sOutput			The output string with resolved characters.
		 * \param _bIsUtf8			If true, handles UTF-8 encoded content; otherwise assumes narrow encoding.
		 **/
		static void						ResolveAllHtmlXmlEscapes( const std::string &_sInput, std::string &_sOutput, bool _bIsUtf8 );

		/**
		 * Counts the number of Unicode code points in a UTF-8 string.
		 * 
		 * \param _sInput			The UTF-8 input string.
		 * \return					Returns the number of UTF-8 code points.
		 **/
		static uint64_t					CountUtfCodePoints( const std::string &_sInput );

		/**
		 * String to integer, from any base.  Since std::stoull() raises exceptions etc.
		 * 
		 * \param _pcText		The text to parse.
		 * \param _iBase		The base of the number to parse out of the text.
		 * \param _psEaten		Optional pointer to a size_t that will be set to the number of character eaten during parsing.
		 * \param _uiMax		The maximum value after which overflow is considered to have happened.
		 * \param _pbOverflow	An optional pointer to a boolean used to indicate whether overflow has occurred or not.
		 * \return				Returns the value parsed from the string.
		 **/
		static uint64_t					StoULL( const char * _pcText, int _iBase = 10, size_t * _psEaten = nullptr, uint64_t _uiMax = 0xFFFFFFFFFFFFFFFFULL, bool * _pbOverflow = nullptr );

		/**
		 * String to double.  Unlike std::atof(), this returns the number of characters eaten, and casts to float when the f postfix is seen.
		 * 
		 * \param _pcText		The text to parse.
		 * \param _psEaten		The number of characters consumed while parsing the double value.
		 * \param _pbError		Optional pointer to a booleon which will be set to true if there are parsing errors.
		 * \return				Returns the parsed double.
		 **/
		static double					AtoF( const char * _pcText, size_t * _psEaten = nullptr, bool * _pbError = nullptr );

		// Basic epsilon comparison.
		static inline bool __cdecl		Epsilon( double _dLeft, double _dRight, double _dEpsilon ) {
			return std::abs( _dLeft - _dRight ) <= _dEpsilon;
		}

		// Basic epsilon comparison.
		static inline bool __cdecl		Epsilon( float _fLeft, float _fRight, float _fEpsilon ) {
			return std::abs( _fLeft - _fRight ) <= _fEpsilon;
		}

		// More accurate epsilon comparison.
		static inline bool __cdecl		RelativeEpsilon( double _dLeft, double _dRight, double _dEpsilon ) {
			if ( _dLeft == _dRight ) { return true; }	// Handle infinities.

			double dA = std::abs( _dLeft );
			double dB = std::abs( _dRight );
			double dDiff = std::abs( _dLeft - _dRight );

			if ( _dLeft == 0.0 || _dRight == 0.0 || (dA + dB < DBL_MIN) ) {
				// Values very close or equal to 0.
				return dDiff < (_dEpsilon * DBL_MIN);
			}

			// Relative error.
			return dDiff / Min( (dA + dB), DBL_MAX ) < _dEpsilon;
		}

		// More accurate epsilon comparison.
		static inline bool __cdecl		RelativeEpsilon( float _fLeft, float _fRight, float _fEpsilon ) {
			if ( _fLeft == _fRight ) { return true; }	// Handle infinities.

			float fA = std::abs( _fLeft );
			float fB = std::abs( _fRight );
			float fDiff = std::abs( _fLeft - _fRight );

			if ( _fLeft == 0.0f || _fRight == 0.0f || (fA + fB < FLT_MIN) ) {
				// Values very close or equal to 0.
				return fDiff < (_fEpsilon * FLT_MIN);
			}

			// Relative error.
			return fDiff / Min( (fA + fB), FLT_MAX ) < _fEpsilon;
		}

		// Gets the current time in system units.
		static inline uint64_t			Time() {
#ifdef _WIN32
			LARGE_INTEGER liInt;
			::QueryPerformanceCounter( &liInt );
			return liInt.QuadPart;
#else
#error "No implementation for Time()."
#endif	// #ifdef _WIN32
		}

		// Gets the timer frequency.
		static uint64_t					TimerFrequency();

		// Converts ticks to microseconds.
		static inline uint64_t			TicksToMicroseconds( uint64_t _ui64Ticks ) {
			return (_ui64Ticks * 1000000ULL) / TimerFrequency();
		}

		// Parse a string into an array of strings given a delimiter.
		template <typename _tType = std::string>
		static std::vector<_tType>		Tokenize( const _tType &_sInput, uint32_t _ui32Token, bool _bIncludeEmptyTokens = true, bool * pbErrored = nullptr ) {
			std::vector<_tType> vRet;
			try {
				_tType tCurLine;
				for ( size_t I = 0; I < _sInput.size(); ++I ) {
					if ( _sInput[I] == _tType::value_type( _ui32Token ) ) {
						if ( tCurLine.size() || _bIncludeEmptyTokens ) {
							vRet.push_back( tCurLine );
						}
						tCurLine.clear();
					}
					else {
						tCurLine.push_back( _sInput[I] );
					}
				}
				if ( tCurLine.size() || _bIncludeEmptyTokens ) {
					vRet.push_back( tCurLine );
				}
				if ( pbErrored ) { (*pbErrored) = false; }
			}
			catch ( ... ) {
				if ( pbErrored ) { (*pbErrored) = true; }
			}
			return vRet;
		}

		// Parse a string into an array of strings given a UTF-32 delimiter.
		template <typename _tType = std::string>
		static std::vector<_tType>		TokenizeUtf( const _tType &_sInput, uint32_t _ui32Token, bool _bIncludeEmptyTokens = true, bool * pbErrored = nullptr ) {
			std::vector<_tType> vRet;
			try {
				_tType tCurLine;
				size_t sSize = 1;
				for ( size_t I = 0; I < _sInput.size(); I += sSize ) {
					uint32_t ui32Char;
					if constexpr ( sizeof( _tType::value_type ) == sizeof( char8_t ) ) {
						ui32Char = NextUtf8Char( reinterpret_cast<const char *>(&_sInput[I]), _sInput.size() - I, &sSize );
					}
					else if constexpr ( sizeof( _tType::value_type ) == sizeof( char16_t ) ) {
						ui32Char = NextUtf16Char( &_sInput[I], _sInput.size() - I, &sSize );
					}
					else {
						sSize = 1;
						ui32Char = _sInput[I];
					}

					if ( ui32Char == _ui32Token ) {
						if ( tCurLine.size() || _bIncludeEmptyTokens ) {
							vRet.push_back( tCurLine );
						}
						tCurLine.clear();
					}
					else {
						tCurLine.append( &_sInput[I], sSize );
					}
				}
				if ( tCurLine.size() || _bIncludeEmptyTokens ) {
					vRet.push_back( tCurLine );
				}
				if ( pbErrored ) { (*pbErrored) = false; }
			}
			catch ( ... ) {
				if ( pbErrored ) { (*pbErrored) = true; }
			}
			return vRet;
		}

		// Recombines an array of strings back into a single string.
		template <typename _tType = std::string>
		static _tType					Reconstitute( const std::vector<_tType> &_vArray, uint32_t _ui32Token, bool * pbErrored = nullptr ) {
			_tType sRet;
			try {
				for ( size_t I = 0; I < _vArray.size(); ++I ) {
					sRet.append( _vArray[I] );
					sRet.push_back( _tType::value_type( _ui32Token ) );
				}
				if ( pbErrored ) { (*pbErrored) = false; }
			}
			catch ( ... ) {
				if ( pbErrored ) { (*pbErrored) = true; }
			}
			return sRet;
		}

		// Merges lines that end with \ with the next line below.
		template <typename _tType = std::string>
		static std::vector<_tType> &	MergeBackslashedLines( std::vector<_tType> &_vArray ) {
			for ( size_t I = 0; I < _vArray.size(); ++I ) {
				if ( _vArray[I].size() && _vArray[I][_vArray[I].size()-1] == '\\' ) {
					// If the whole line is just \ by itself, clear the line and move on.
					_tType sCopy = TrimWhitespace<_tType>( _vArray[I] );
					if ( sCopy.size() == 1 && sCopy[0] == '\\' ) {
						_vArray[I].clear();
					}
					else {
						// Begin combining lines.
						size_t sIdx = I;
						_vArray[sIdx].pop_back();	// Remove the \.
						for ( ++I; I < _vArray.size(); ++I ) {
							_vArray[sIdx].append( _vArray[I] );
							_vArray[I].clear();
							if ( _vArray[sIdx][_vArray[sIdx].size()-1] != '\\' ) {
								break;
							}
							else {
								_vArray[sIdx].pop_back();	// Remove the \.
							}
						}
					}
				}
			}
			return _vArray;
		}

		// Detrmines the length of a C/C++/Python string inside the given text starting at the given position.  A non-0 return indicates the substring at the given
		//	text position is the start of a code-format string.
		template <typename _tType = std::string>
		static size_t					CodeStringLength( const _tType &_sInput, size_t _sPos ) {
#define EE_PREVIEW( OFF )			(((_sPos + (OFF)) < _sInput.size()) ? _sInput[_sPos+(OFF)] : _tType::value_type( '\0' ))
			size_t sStart = _sPos;
			if ( _sInput[_sPos] == _tType::value_type( '\'' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\'' ) && EE_PREVIEW( 2 ) == _tType::value_type( '\'' ) ) {
				_sPos += 2;	// Eat the 2nd and 3rd '.
				// Eat string quotes.
				while ( ++_sPos < _sInput.size() ) {
					if ( _sInput[_sPos] == _tType::value_type( '\\' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\\' ) ) {
						// \\ sequence.  Skip.
						++_sPos;
					}
					else if ( _sInput[_sPos] == _tType::value_type( '\\' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\'' ) ) {
						// \' sequence.  Skip.
						++_sPos;
					}
					else if ( _sInput[_sPos] == _tType::value_type( '\'' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\'' ) && EE_PREVIEW( 2 ) == _tType::value_type( '\'' ) ) {
						return _sPos - sStart + 3;
					}
				}
			}
			else if ( _sInput[_sPos] == _tType::value_type( '\"' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\"' ) && EE_PREVIEW( 2 ) == _tType::value_type( '\"' ) ) {
				_sPos += 2;	// Eat the 2nd and 3rd ".
				// Eat string quotes.
				while ( ++_sPos < _sInput.size() ) {
					if ( _sInput[_sPos] == _tType::value_type( '\\' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\\' ) ) {
						// \\ sequence.  Skip.
						++_sPos;
					}
					else if ( _sInput[_sPos] == _tType::value_type( '\\' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\"' ) ) {
						// \" sequence.  Skip.
						++_sPos;
					}
					else if ( _sInput[_sPos] == _tType::value_type( '\"' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\"' ) && EE_PREVIEW( 2 ) == _tType::value_type( '\"' ) ) {
						return _sPos - sStart + 3;
					}
				}
			}
			else if ( (_sInput[_sPos] == _tType::value_type( 'r' ) || _sInput[_sPos] == _tType::value_type( 'R' )) &&
				EE_PREVIEW( 1 ) == _tType::value_type( '\"' ) ) {
				++_sPos;	// Eat the R.
				// Eat raw string quotes.
				while ( ++_sPos < _sInput.size() ) {
					if ( _sInput[_sPos] == _tType::value_type( '\"' ) ) {
						return _sPos - sStart + 1;
					}
				}
			}
			else if ( _sInput[_sPos] == _tType::value_type( '\"' ) ) {
				// Eat string quotes.
				while ( ++_sPos < _sInput.size() ) {
					if ( _sInput[_sPos] == _tType::value_type( '\\' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\\' ) ) {
						// \\ sequence.  Skip.
						++_sPos;
					}
					else if ( _sInput[_sPos] == _tType::value_type( '\\' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\"' ) ) {
						// \" sequence.  Skip.
						++_sPos;
					}
					else if ( _sInput[_sPos] == _tType::value_type( '\"' ) ) {
						return _sPos - sStart + 1;
					}
				}
			}
			else if ( _sInput[_sPos] == _tType::value_type( '\'' ) ) {
				// Eat character quotes.
				while ( ++_sPos < _sInput.size() ) {
					if ( _sInput[_sPos] == _tType::value_type( '\\' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\\' ) ) {
						// \\ sequence.  Skip.
						++_sPos;
					}
					else if ( _sInput[_sPos] == _tType::value_type( '\\' ) && EE_PREVIEW( 1 ) == _tType::value_type( '\'' ) ) {
						// \" sequence.  Skip.
						++_sPos;
					}
					else if ( _sInput[_sPos] == _tType::value_type( '\'' ) ) {
						return _sPos - sStart + 1;
					}
				}
			}
#undef EE_PREVIEW
			return 0;
		}

		// Removes C-style comments from a string.
		template <typename _tType = std::string>
		static _tType &					RemoveCComments( _tType &_sInput ) {
			size_t sIdx = 0;
#define EE_PREVIEW( OFF )			(((sIdx + (OFF)) < _sInput.size()) ? _sInput[sIdx+(OFF)] : _tType::value_type( '\0' ))

			while ( sIdx < _sInput.size() ) {
				size_t stStrLen = CodeStringLength( _sInput, sIdx );
				if ( stStrLen ) {
					sIdx += stStrLen;
					continue;
				}

				if ( _sInput[sIdx] == _tType::value_type( '/' ) && EE_PREVIEW( 1 ) == _tType::value_type( '/' ) ) {
					size_t stStart = sIdx;
					sIdx += 2;
					uint32_t ui32LastChar = '\0';
					size_t stNewLines = 0;
					while ( sIdx < _sInput.size() ) {
						if ( _sInput[sIdx] == _tType::value_type( '\n' ) ) {
							if ( ui32LastChar == '\\' ) {
								++stNewLines;
							}
							else {
								// Add back the new lines we skipped.
								for ( auto J = stNewLines; J--; ) {
									_sInput[stStart++] = _tType::value_type( '\n' );
								}
								_sInput.erase( stStart, sIdx - stStart );
								sIdx = stStart;
								break;
							}
						}
						ui32LastChar = _sInput[sIdx++];
					}
					if ( sIdx == _sInput.size() ) {
						_sInput.erase( stStart );
					}
					continue;
				}

				++sIdx;
			}

#undef EE_PREVIEW
			return _sInput;
		}

		// Removes C++-style comments from a string.
		template <typename _tType = std::string>
		static _tType &					RemoveCPlusPlusComments( _tType &_sInput ) {
			size_t sIdx = 0;
#define EE_PREVIEW( OFF )			(((sIdx + (OFF)) < _sInput.size()) ? _sInput[sIdx+(OFF)] : _tType::value_type( '\0' ))
			_tType sNewLines;
			while ( sIdx < _sInput.size() ) {
				size_t stStrLen = CodeStringLength( _sInput, sIdx );
				if ( stStrLen ) {
					sIdx += stStrLen;
					continue;
				}

				if ( _sInput[sIdx] == _tType::value_type( '/' ) && EE_PREVIEW( 1 ) == _tType::value_type( '*' ) ) {
					size_t I = sIdx++;
					while ( ++sIdx < _sInput.size() ) {
						if ( _sInput[sIdx] == _tType::value_type( '*' ) && EE_PREVIEW( 1 ) == _tType::value_type( '/' ) ) { sIdx += 2; break; }
						if ( _sInput[sIdx] == _tType::value_type( '\n' ) ) { sNewLines.push_back( '\n' ); }
					}
					_sInput.erase( I, sIdx - I );
					_sInput.insert( I, sNewLines );
					break;
				}

				++sIdx;
			}

#undef EE_PREVIEW
			return _sInput;
		}

		// Removes C/C++ comments from a string.
		template <typename _tType = std::string>
		static _tType &					RemoveComments( _tType &_sInput ) {
			size_t sIdx = 0;
#define EE_PREVIEW( OFF )			(((sIdx + (OFF)) < _sInput.size()) ? _sInput[sIdx+(OFF)] : _tType::value_type( '\0' ))
			_tType sNewLines;
			while ( sIdx < _sInput.size() ) {
				size_t stStrLen = CodeStringLength( _sInput, sIdx );
				if ( stStrLen ) {
					sIdx += stStrLen;
					continue;
				}

				if ( _sInput[sIdx] == _tType::value_type( '/' ) && EE_PREVIEW( 1 ) == _tType::value_type( '/' ) ) {
					size_t stStart = sIdx;
					sIdx += 2;
					uint32_t ui32LastChar = '\0';
					size_t stNewLines = 0;
					while ( sIdx < _sInput.size() ) {
						if ( _sInput[sIdx] == _tType::value_type( '\n' ) ) {
							if ( ui32LastChar == '\\' ) {
								++stNewLines;
							}
							else {
								// Add back the new lines we skipped.
								for ( auto J = stNewLines; J--; ) {
									_sInput[stStart++] = _tType::value_type( '\n' );
								}
								_sInput.erase( stStart, sIdx - stStart );
								sIdx = stStart;
								break;
							}
						}
						ui32LastChar = _sInput[sIdx++];
					}
					if ( sIdx == _sInput.size() ) {
						_sInput.erase( stStart );
					}
					continue;
				}

				if ( _sInput[sIdx] == _tType::value_type( '/' ) && EE_PREVIEW( 1 ) == _tType::value_type( '*' ) ) {
					size_t I = sIdx++;
					while ( ++sIdx < _sInput.size() ) {
						if ( _sInput[sIdx] == _tType::value_type( '*' ) && EE_PREVIEW( 1 ) == _tType::value_type( '/' ) ) { sIdx += 2; break; }
						if ( _sInput[sIdx] == _tType::value_type( '\n' ) ) { sNewLines.push_back( '\n' ); }
					}
					_sInput.erase( I, sIdx - I );
					_sInput.insert( I, sNewLines );
					break;
				}

				++sIdx;
			}

#undef EE_PREVIEW
			return _sInput;
		}

		// Pulls any preprocessing directives out of a single line.
		static bool						PreprocessingDirectives( const std::string &_sInput, std::string &_sDirective, std::string &_sParms );

		/**
		 * Computes the least common multiple (LCM) of two integers.
		 *
		 * \param _tA First integer value.
		 * \param _tB Second integer value.
		 * \return Returns the LCM (always non-negative). If either input is 0, returns 0.
		 */
		template <typename _tInt>
		static inline _tInt				Lcm( _tInt _tA, _tInt _tB ) {
			static_assert( std::is_integral_v<_tInt>, "" );

			if ( _tA == 0 || _tB == 0 ) { return 0; }

			_tInt tGcd = std::gcd( _tA, _tB );

			// Divide before multiply to reduce overflow risk.
			// Note: This can still overflow for large values; see LcmChecked() below if you need detection.
			_tInt tDiv = _tA / tGcd;
			_tInt tLcm = tDiv * _tB;

			// LCM is defined as non-negative.
			if ( tLcm < 0 ) { tLcm = -tLcm; }

			return tLcm;
		}

		/**
		 * Computes the LCM of two integers, returning std::nullopt on overflow.
		 *
		 * \param _tA First integer value.
		 * \param _tB Second integer value.
		 * \return Returns the LCM if it fits in the type; otherwise std::nullopt. If either input is 0, returns 0.
		 */
		template <typename _tInt>
		static inline std::optional<_tInt>
										LcmChecked( _tInt _tA, _tInt _tB ) {
			static_assert( std::is_integral_v<_tInt>, "" );

			if ( _tA == 0 || _tB == 0 ) { return _tInt( 0 ); }

			_tInt tGcd = std::gcd( _tA, _tB );
			_tInt tDiv = _tA / tGcd;

			// Overflow check for: tDiv * _tB
			// Use unsigned magnitude checks to handle negatives safely.
			using _tUnsigned = std::make_unsigned_t<_tInt>;
			auto AbsU = []( _tInt _tX ) -> _tUnsigned {
				// Convert to unsigned magnitude without UB on INT_MIN.
				_tUnsigned u = static_cast<_tUnsigned>( _tX );
				if ( _tX < 0 ) { u = _tUnsigned( 0 ) - u; }
				return u;
			};

			_tUnsigned uA = AbsU( tDiv );
			_tUnsigned uB = AbsU( _tB );

			if ( uA != 0 && uB > _tUnsigned( std::numeric_limits<_tInt>::max() ) / uA ) {
				return std::nullopt;
			}

			_tInt tLcm = tDiv * _tB;
			if ( tLcm < 0 ) { tLcm = -tLcm; }

			return tLcm;
		}

		/**
		 * Internal helper to guard trivial window sizes.
		 * 
		 * \param _sN   Number of samples.
		 * \param _vRet Returned vector.
		 * \return      Returns true if handled or memory was available; false on allocation failure.
		 **/
		template <typename _tType>
		static bool						HandleTrivialWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			try {
				if ( _sN == 0 ) {
					_vRet.clear();
					return true;
				}
				_vRet.resize( _sN );
				if ( _sN == 1 ) {
					_vRet[0] = _tType( 1 );
				}
				return true;
			}
			catch ( ... ) {
				return false;
			}
		}

		/**
		 * A 0th-order Bessel function needed for the Kaiser window.
		 * 
		 * \param _dX	The Bessel term.
		 * \return		Returns the Bessel function for the given term.
		 **/
		static inline double			Bessel0( double _dX ) {
			constexpr double dEspiRatio = 1.0e-16;
			double dSum = 1.0, dPow = 1.0, dDs = 1.0, dXh = _dX * 0.5;
			uint32_t ui32K = 0;

			while ( dDs > dSum * dEspiRatio ) {
				++ui32K;
				dPow *= (dXh / ui32K);
				dDs = dPow * dPow;
				dSum += dDs;
			}

			return dSum;
		}

		/**
		 * Barthann (modified Bartlett-Hann) window.
		 * 
		 * \brief
		 * Returns a modified Bartlett-Hann window (Barthann).
		 * A common definition is:
		 *   x = n / (N-1)
		 *   w[n] = 0.62
		 *          - 0.48 * |x - 0.5|
		 *          - 0.38 * cos( 2*pi*x ).
		 * 
		 * \param _sN   Number of samples.
		 * \param _vRet Returned vector.
		 * \return      Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						BarthannWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN == 1 ) { return true; }

			double dDen = double( _sN - 1 );
			for ( auto I = _sN; I--; ) {
				double dX = double( I ) / dDen;
				double dV = 0.62
							- 0.48 * std::fabs( dX - 0.5 )
							- 0.38 * std::cos( 2.0 * EE_PI * dX );
				_vRet[I] = _tType( dV );
			}
			return true;
		}

		/**
		 * Creates a Bartlett window.
		 * 
		 * \param _sN	Number of samples.  Generally an odd number.
		 * \param _vRet	The returned vector.
		 * \return		Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						BartlettWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			size_t stMax = _vRet.size() - 1;
			double d2InvMax = 2.0 / stMax;
			double dHalfMax = stMax / 2.0;
			for ( auto I = _sN; I--; ) {
				_vRet[I] = _tType( d2InvMax * (dHalfMax - std::abs( I - dHalfMax )) );
			}

			return true;
		}

		/**
		 * Creates a Blackman window.
		 * 
		 * \param _sN	Number of samples.  Generally an odd number.
		 * \param _vRet	The returned vector.
		 * \return		Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						BlackmanWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			// These exact values place zeros at the third and fourth sidelobes, but result in a discontinuity at the edges and a 6 dB/oct fall-off.
			//constexpr double dA0 = 7938.0 / 18608.0;	// 0.42659071367153911236158592146239243447780609130859375
			//constexpr double dA1 = 9240.0 / 18608.0;	// 0.4965606190885640813803547644056379795074462890625
			//constexpr double dA2 = 1430.0 / 18608.0;	// 0.07684866723989682013584712194642634131014347076416015625
			
			// The truncated coefficients do not null the sidelobes as well, but have an improved 18 dB/oct fall-off.
			constexpr double dA = 0.16;					// 0.1600000000000000033306690738754696212708950042724609375
			constexpr double dA0 = (1.0 - dA) / 2.0;	// 0.419999999999999984456877655247808434069156646728515625
			constexpr double dA1 = 1.0 / 2.0;			// 0.5
			constexpr double dA2 = dA / 2.0;			// 0.08000000000000000166533453693773481063544750213623046875
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _vRet.size() - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			double dTauInvMax2 = 2.0 * dTauInvMax;
			for ( auto I = _sN; I--; ) {
				_vRet[I] = _tType( dA0 - dA1 * std::cos( dTauInvMax * I ) + dA2 * std::cos( dTauInvMax2 * I ) );
			}

			return true;
		}

		/**
		 * Creates a Blackman-Harris window.
		 * 
		 * \param _sN	Number of samples.  Generally an odd number.
		 * \param _vRet	The returned vector.
		 * \return		Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						BlackmanHarrisWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			constexpr double dA0 = 0.35875;				// 0.35875000000000001332267629550187848508358001708984375
			constexpr double dA1 = 0.48829;				// 0.48829000000000000181188397618825547397136688232421875
			constexpr double dA2 = 0.14128;				// 0.1412799999999999889244151063394383527338504791259765625
			constexpr double dA3 = 0.01168;				// 0.0116799999999999994104715739240418770350515842437744140625

			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _vRet.size() - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			double dTauInvMax2 = 2.0 * dTauInvMax;
			double dTauInvMax3 = 3.0 * dTauInvMax;
			for ( auto I = _sN; I--; ) {
				_vRet[I] = _tType( dA0 - dA1 * std::cos( dTauInvMax * I ) + dA2 * std::cos( dTauInvMax2 * I ) - dA3 * std::cos( dTauInvMax3 * I ) );
			}

			return true;
		}

		/**
		 * Creates a Blackman-Nuttal window.
		 * 
		 * \param _sN	Number of samples.  Generally an odd number.
		 * \param _vRet	The returned vector.
		 * \return		Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						BlackmanNuttalWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			constexpr double dA0 = 0.3635819;			// 0.3635819000000000134065203383215703070163726806640625
			constexpr double dA1 = 0.4891775;			// 0.489177499999999987334575735076214186847209930419921875
			constexpr double dA2 = 0.1365995;			// 0.1365995000000000125783827797931735403835773468017578125
			constexpr double dA3 = 0.0106411;			// 0.01064110000000000055830895462349872104823589324951171875

			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _vRet.size() - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			double dTauInvMax2 = 2.0 * dTauInvMax;
			double dTauInvMax3 = 3.0 * dTauInvMax;
			for ( auto I = _sN; I--; ) {
				_vRet[I] = _tType( dA0 - dA1 * std::cos( dTauInvMax * I ) + dA2 * std::cos( dTauInvMax2 * I ) - dA3 * std::cos( dTauInvMax3 * I ) );
			}

			return true;
		}

		/**
		 * Bohman window.
		 * 
		 * \brief
		 * Returns a Bohman window:
		 * Let M = (N - 1) / 2.
		 *   r = |k / M - 1|
		 *   w[k] = (1 - r) cos( pi*r ) + (1/pi) sin( pi*r ).
		 * 
		 * \param _sN   Number of samples.
		 * \param _vRet Returned vector.
		 * \return      Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						BohmanWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN == 1 ) { return true; }

			try {
				double dM = (double( _sN ) - 1.0) * 0.5;
				double dInvPi = 1.0 / EE_PI;
				for ( auto I = _sN; I--; ) {
					double dR = std::fabs( double(I) / dM - 1.0 );
					double dCr, dSr;
					::SinCos( EE_PI * dR, &dSr, &dCr );
					double dV = (1.0 - dR) * dCr + dInvPi * dSr;
					_vRet[I] = _tType( dV );
				}
				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Boxcar (rectangular) window.
		 * 
		 * \brief
		 * Returns a simple boxcar window of ones.
		 * 
		 * \param _sN   Number of samples.
		 * \param _vRet Returned vector.
		 * \return      Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						BoxcarWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			for ( auto I = _sN; I--; ) {
				_vRet[I] = _tType( 1 );
			}
			return true;
		}

		/**
		 * Creates a Dolph-Chebyshev (Chebwin) window.
		 * 
		 * \brief
		 * Implements the same Chebyshev window as scipy.signal.windows.chebwin()
		 * / MATLAB's chebwin(), using the Lyons procedure:
		 *   - M samples total.
		 *   - Sidelobe attenuation of _dAt dB.
		 *   - Window is normalized so that max( w[n] ) == 1.
		 * 
		 * \param _sM	Number of samples in the window (M).
		 * \param _dAt	Sidelobe attenuation in dB (e.g., 100.0).
		 * \param _vRet	The returned window values.
		 * \return		Returns true if there was enough memory to resize the vector.
		 **/
		template<typename _tType = double>
		static bool						ChebwinWindow( size_t _sM, double _dAt, std::vector<_tType> &_vRet ) {
			try {
				if ( !HandleTrivialWindow( _sM, _vRet ) ) { return false; }
				if ( _sM == 1 ) { return true; }

				// Non-positive attenuation degenerates toward a boxcar.
				if ( _dAt <= 0.0 ) {
					for ( size_t I = 0; I < _sM; ++I ) {
						_vRet[I] = _tType( 1 );
					}
					return true;
				}

				const size_t sM = _sM;
				const size_t sN = sM - 1;		// Lyons: N = M - 1.

				// Step 2: γ controls sidelobe level, sidelobes at -20*γ dB.
				const double dGamma = _dAt / 20.0;
				const double dTenGamma = std::pow( 10.0, dGamma );

				// Step 3: α = cosh( acosh(10^γ) / N ).
				const double dAlpha = std::cosh( std::acosh( dTenGamma ) / double( sN ) );

				// Step 4 & 5: Build W(m) sequence in the frequency domain.
				std::vector<double> vW;
				vW.resize( sN );

				for ( size_t M = 0; M < sN; ++M ) {
					const double dCos = std::cos( EE_PI * double( M ) / double( sN ) );
					const double dA = std::fabs( dAlpha * dCos );

					double dVal;
					if ( dA > 1.0 ) {
						// W(M) = (-1)^M * cosh( N * acosh( |A(M)| ) ).
						dVal = std::cosh( double( sN ) * std::acosh( dA ) );
					}
					else {
						// W(M) = (-1)^M * cos( N * acos( |A(M)| ) ).
						dVal = std::cos( double( sN ) * std::acos( dA ) );
					}

					if ( M & 1 ) { dVal = -dVal; }
					vW[M] = dVal;
				}

				// Lyons 2011 update: If M is even and we used |A(m)|, flip the last M/2 - 1 W(m)'s.
				if ( (sM & 1) == 0 ) {
					const size_t sHalf = sM / 2;
					if ( sHalf > 1 ) {
						const size_t sCount = sHalf - 1;
						const size_t sStart = sN - sCount;
						for ( size_t m = sStart; m < sN; ++m ) {
							vW[m] = -vW[m];
						}
					}
				}

				// Step 6: N-point inverse DFT of W(m), but we can ignore the 1/N scale factor
				// because we'll normalize later. Only the real part is needed, so we use cos().
				std::vector<double> vTmp;
				vTmp.resize( sN );

				for ( size_t N = 0; N < sN; ++N ) {
					double dSum = 0.0;
					for ( size_t K = 0; K < sN; ++K ) {
						const double dAngle = 2.0 * EE_PI * double( N * K ) / double( sN );
						dSum += vW[K] * std::cos( dAngle );
					}
					vTmp[N] = dSum;
				}

				// Step 7: Replace w(0) with w(0)/2.
				vTmp[0] *= 0.5;

				// Step 8: Append w(0) to create M-sample symmetric sequence.
				std::vector<double> vCheb;
				vCheb.resize( sM );
				for ( size_t N = 0; N < sN; ++N ) {
					vCheb[N] = vTmp[N];
				}
				vCheb[sM-1] = vTmp[0];

				// Step 9: Normalize so that max( |w(k)| ) == 1.
				double dMax = 0.0;
				for ( size_t N = 0; N < sM; ++N ) {
					const double dAbs = std::fabs( vCheb[N] );
					if ( dAbs > dMax ) {
						dMax = dAbs;
					}
				}
				if ( dMax == 0.0 ) {
					dMax = 1.0;
				}

				for ( size_t N = 0; N < sM; ++N ) {
					_vRet[N] = _tType( vCheb[N] / dMax );
				}

				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Cosine window (a.k.a. sine window).
		 * 
		 * \brief
		 * Returns a window with a simple cosine shape:
		 *   w[n] = sin( pi * n / (N-1) ),  0 <= n < N, N > 1.
		 * 
		 * \param _sN   Number of samples.
		 * \param _vRet Returned vector.
		 * \return      Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						CosineWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN == 1 ) { return true; }

			double dDen = double( _sN - 1 );
			for ( auto I = _sN; I--; ) {
				double dV = std::sin( EE_PI * double(I) / dDen );
				_vRet[I] = _tType( dV );
			}
			return true;
		}

		/**
		 * Computes Discrete Prolate Spheroidal Sequences (DPSS), similar to scipy.signal.windows.dpss().
		 * 
		 * \brief
		 * Generates the first Kmax DPSS tapers (orders [0..(_sKmax-1)]) for a given sequence length and
		 * time-halfbandwidth product NW. The tapers are the eigenvectors of a symmetric tridiagonal matrix
		 * (Percival & Walden form), found here via power iteration with deflation.
		 * 
		 * \param _sM          Length of each window (M). 0 => No output. 1 => All windows are [1].
		 * \param _dNW         Time-halfbandwidth product NW (standardized half bandwidth).
		 * \param _sKmax       Number of windows to generate (orders 0.._sKmax-1). 0 => No output.
		 * \param _vvRet       Returned windows. On success, resized to [_sKmax][_sM]. Each window has L2 norm 1.
		 * \param _pvRatios    Optional output for associated eigenvalues (concentration-like ratios). Can be nullptr.
		 * \return             Returns true if all allocations succeeded.
		 **/
		template<typename _tType = double>
		static bool						DpssWindows( size_t _sM, double _dNW, size_t _sKmax, std::vector<std::vector<_tType>> &_vvRet, std::vector<double> *_pvRatios = nullptr ) {
			try {
				_vvRet.clear();
				if ( _pvRatios ) {
					_pvRatios->clear();
				}

				if ( _sM == 0 || _sKmax == 0 ) { return true; }

				_vvRet.resize( _sKmax );
				for ( size_t K = 0; K < _sKmax; ++K ) { _vvRet[K].resize( _sM ); }

				if ( _sM == 1 ) {
					// Trivial 1-sample windows: all [1].
					for ( size_t K = 0; K < _sKmax; ++K ) {
						_vvRet[K][0] = _tType( 1 );
						if ( _pvRatios ) { _pvRatios->push_back( 1.0 ); }
					}
					return true;
				}

				// ===============================
				// Build the symmetric tridiagonal matrix.
				// ===============================
				std::vector<double> vDiag( _sM );
				std::vector<double> vOff( _sM > 1 ? _sM - 1 : 0 );

				// W = NW / M; the cosine term uses cos( 2*pi*W ) == cos( 2*pi*NW/M ).
				double dCos = std::cos( 2.0 * EE_PI * _dNW / double( _sM ) );

				for ( size_t I = 0; I < _sM; ++I ) {
					double dTmp = 0.5 * (double( _sM ) - 2.0 * double( I ) - 1.0);
					vDiag[I] = dTmp * dTmp * dCos;
				}
				for ( size_t I = 1; I < _sM; ++I ) {
					// Off-diagonal entries: 0.5 * n * (M - n), n = 1..M-1.
					vOff[I-1] = 0.5 * double( I ) * (double( _sM ) - double( I ));
				}

				// ===============================
				// Helper: y = A * x for symmetric tridiagonal A.
				// ===============================
				auto MatVec = [&vDiag, &vOff]( const std::vector<double> &_vIn, std::vector<double> &_vOut ) {
					size_t sN = vDiag.size();
					_vOut.resize( sN );
					if ( sN == 0 ) { return; }
					if ( sN == 1 ) {
						_vOut[0] = vDiag[0] * _vIn[0];
						return;
					}

					_vOut[0] = vDiag[0] * _vIn[0] + vOff[0] * _vIn[1];
					for ( size_t I = 1; I + 1 < sN; ++I ) {
						_vOut[I] = vOff[I-1] * _vIn[I-1] + vDiag[I] * _vIn[I] + vOff[I] * _vIn[I+1];
					}
					_vOut[sN-1] = vOff[sN-2] * _vIn[sN-2] + vDiag[sN-1] * _vIn[sN-1];
				};

				// ===============================
				// Power iteration with simple deflation to get Kmax largest eigenpairs.
				// ===============================
				const uint32_t ui32MaxIter = 2000;
				const double dTol = 1.0e-12;

				std::vector<double> vCur( _sM );
				std::vector<double> vNext( _sM );
				std::vector<double> vAx( _sM );
				std::vector<std::vector<double>> vPrevEig;
				vPrevEig.reserve( _sKmax );
				if ( _pvRatios ) {
					_pvRatios->reserve( _sKmax );
				}

				for ( size_t K = 0; K < _sKmax; ++K ) {
					// ---- Initial guess.
					// Start with a simple constant vector, then gently modulate for higher orders.
					for ( size_t I = 0; I < _sM; ++I ) {
						vCur[I] = 1.0;
					}
					if ( K > 0 ) {
						for ( size_t I = 0; I < _sM; ++I ) {
							double dPhase = EE_PI * double( K * (2 * I + 1) ) / (2.0 * double( _sM ));
							vCur[I] *= std::cos( dPhase );
						}
					}

					// Orthogonalize initial guess against previously found eigenvectors.
					for ( size_t J = 0; J < vPrevEig.size(); ++J ) {
						double dDot = 0.0;
						for ( size_t I = 0; I < _sM; ++I ) {
							dDot += vCur[I] * vPrevEig[J][I];
						}
						for ( size_t I = 0; I < _sM; ++I ) {
							vCur[I] -= dDot * vPrevEig[J][I];
						}
					}

					// Normalize initial vector.
					double dNorm = 0.0;
					for ( size_t I = 0; I < _sM; ++I ) {
						dNorm += vCur[I] * vCur[I];
					}
					dNorm = std::sqrt( dNorm );
					if ( dNorm == 0.0 ) {
						vCur[0] = 1.0;
						dNorm = 1.0;
					}
					for ( size_t I = 0; I < _sM; ++I ) { vCur[I] /= dNorm; }

					// ---- Power iteration loop.
					for ( uint32_t ui32Iter = 0; ui32Iter < ui32MaxIter; ++ui32Iter ) {
						// w = A * v.
						MatVec( vCur, vNext );

						// Deflate: remove components along earlier eigenvectors.
						for ( size_t J = 0; J < vPrevEig.size(); ++J ) {
							double dDot = 0.0;
							for ( size_t I = 0; I < _sM; ++I ) {
								dDot += vNext[I] * vPrevEig[J][I];
							}
							for ( size_t I = 0; I < _sM; ++I ) {
								vNext[I] -= dDot * vPrevEig[J][I];
							}
						}

						// Normalize w.
						dNorm = 0.0;
						for ( size_t I = 0; I < _sM; ++I ) { dNorm += vNext[I] * vNext[I]; }
						dNorm = std::sqrt( dNorm );
						if ( dNorm == 0.0 ) { break; }
						for ( size_t I = 0; I < _sM; ++I ) { vNext[I] /= dNorm; }

						// Check convergence: ||w - v||_2.
						double dDiff = 0.0;
						for ( size_t I = 0; I < _sM; ++I ) {
							double dDelta = vNext[I] - vCur[I];
							dDiff += dDelta * dDelta;
						}
						dDiff = std::sqrt( dDiff );
						vCur.swap( vNext );
						if ( dDiff < dTol ) { break; }
					}

					// Enforce deterministic sign: make the center sample non-negative.
					size_t sCenter = _sM / 2;
					if ( vCur[sCenter] < 0.0 ) {
						for ( size_t I = 0; I < _sM; ++I ) {
							vCur[I] = -vCur[I];
						}
					}

					// Compute associated eigenvalue (Rayleigh quotient).
					MatVec( vCur, vAx );
					double dLambda = 0.0;
					for ( size_t I = 0; I < _sM; ++I ) {
						dLambda += vCur[I] * vAx[I];
					}
					if ( _pvRatios ) {
						_pvRatios->push_back( dLambda );
					}

					// Store eigenvector for deflation.
					vPrevEig.emplace_back( _sM );
					for ( size_t I = 0; I < _sM; ++I ) {
						vPrevEig.back()[I] = vCur[I];
					}

					// Copy to output (cast to requested type).
					for ( size_t I = 0; I < _sM; ++I ) {
						_vvRet[K][I] = _tType( vCur[I] );
					}
				}

				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Exponential window.
		 * 
		 * \brief
		 * Returns an exponential window:
		 *   w[n] = exp( -|n - center| / tau ).
		 * 
		 * \param _sN       Number of samples.
		 * \param _dTau     The decay parameter tau (> 0).
		 * \param _dCenter  The center position. If < 0, the center is (N-1)/2.
		 * \param _vRet     Returned vector.
		 * \return          Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						ExponentialWindow( size_t _sN, double _dTau, double _dCenter, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN == 1 ) { return true; }

			if ( _dTau <= 0.0 ) { _dTau = 1.0; }
			if ( _dCenter < 0.0 || _dCenter > double( _sN - 1 ) ) {
				_dCenter = (double( _sN ) - 1.0) * 0.5;
			}

			try {
				for ( auto I = _sN; I--; ) {
					double dN = double(I);
					double dV = std::exp( -std::fabs( dN - _dCenter ) / _dTau );
					_vRet[I] = _tType( dV );
				}
				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Creates a Flat-Top window.
		 * 
		 * \param _sN	Number of samples.  Generally an odd number.
		 * \param _vRet	The returned vector.
		 * \return		Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						FlatTopWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			constexpr double dA0 = 0.21557895;			// 0.215578949999999991415933209282229654490947723388671875
			constexpr double dA1 = 0.41663158;			// 0.4166315800000000013625367500935681164264678955078125
			constexpr double dA2 = 0.277263158;			// 0.2772631580000000095509449238306842744350433349609375
			constexpr double dA3 = 0.083578947;			// 0.08357894700000000065553962258491083048284053802490234375
			constexpr double dA4 = 0.006947368;			// 0.006947367999999999772786640761523813125677406787872314453125

			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			try {
				const double dTau = 2.0 * std::numbers::pi;
				size_t stMax = _vRet.size() - 1;
				double dInvMax = 1.0 / stMax;
				double dTauInvMax = dTau * dInvMax;
				double dTauInvMax2 = 2.0 * dTauInvMax;
				double dTauInvMax3 = 3.0 * dTauInvMax;
				double dTauInvMax4 = 4.0 * dTauInvMax;
				for ( auto I = _sN; I--; ) {
					_vRet[I] = _tType( dA0 - dA1 * std::cos( dTauInvMax * I ) + dA2 * std::cos( dTauInvMax2 * I ) - dA3 * std::cos( dTauInvMax3 * I ) + dA4 * std::cos( dTauInvMax4 * I ) );
				}

				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Gaussian window.
		 * 
		 * \brief
		 * Returns a Gaussian window:
		 *   n = 0..N-1; m = (N-1)/2
		 *   w[n] = exp( -0.5 * ((n - m) / sigma)^2 ).
		 * 
		 * \param _sN       Number of samples.
		 * \param _dSigma   Standard deviation of the Gaussian in samples.
		 * \param _vRet     Returned vector.
		 * \return          Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						GaussianWindow( size_t _sN, double _dSigma, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN == 1 ) { return true; }

			if ( _dSigma <= 0.0 ) { _dSigma = 1.0; }

			double dM = (double( _sN ) - 1.0) * 0.5;
			for ( auto I = _sN; I--; ) {
				double dN = double( I ) - dM;
				double dV = std::exp( -0.5 * (dN / _dSigma) * (dN / _dSigma) );
				_vRet[I] = _tType( dV );
			}
			return true;
		}

		/**
		 * Generalized cosine window.
		 * 
		 * \brief
		 * Returns a window defined by a sum of cosine terms.
		 * 
		 * \param _sN   Number of samples.
		 * \param _vA   Coefficients a_k for the cosine terms (a[0] + a[1] cos(...) + ...).
		 * \param _vRet The returned vector.
		 * \return      Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool                     GeneralCosineWindow( size_t _sN, const std::vector<_tType> &_vA, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			if ( _vA.empty() ) {
				for ( auto I = _sN; I--; ) {
					_vRet[I] = _tType( 1 );
				}
				return true;
			}

			double dStep = (2.0 * EE_PI) / double( _sN - 1 );
			for ( auto I = _sN; I--; ) {
				double dX = -EE_PI + dStep * double( I );
				double dSum = 0.0;
				for ( size_t K = 0; K < _vA.size(); ++K ) {
					dSum += double( _vA[K] ) * std::cos( double( K ) * dX );
				}
				_vRet[I] = _tType( dSum );
			}
			return true;
		}

		/**
		 * Generalized Gaussian window.
		 * 
		 * \brief
		 * Returns a window with a generalized Gaussian shape:
		 *   w[n] = exp( -0.5 * ((n - m) / sigma)^(2p) ).
		 * 
		 * \param _sN       Number of samples.
		 * \param _dP       Shape parameter p.
		 * \param _dSigma   Sigma parameter.
		 * \param _vRet     Returned vector.
		 * \return          Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						GeneralGaussianWindow( size_t _sN, double _dP, double _dSigma, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN == 1 ) { return true; }

			if ( _dP <= 0.0 ) { _dP = 1.0; }
			if ( _dSigma <= 0.0 ) { _dSigma = 1.0; }

			double dM = (double( _sN ) - 1.0) * 0.5;
			double dPow = 2.0 * _dP;
			for ( auto I = _sN; I--; ) {
				double dN = (double( I ) - dM) / _dSigma;
				double dAbs = std::fabs( dN );
				double dTerm = std::pow( dAbs, dPow );
				double dV = std::exp( -0.5 * dTerm );
				_vRet[I] = _tType( dV );
			}
			return true;
		}

		/**
		 * Generalized Hamming window.
		 * 
		 * \brief
		 * Returns a generalized Hamming window:
		 *   w[n] = alpha - (1 - alpha) cos( 2*pi*n / (N-1) ).
		 * 
		 * \param _sN       Number of samples.
		 * \param _dAlpha   Alpha parameter.
		 * \param _vRet     Returned vector.
		 * \return          Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						GeneralHammingWindow( size_t _sN, double _dAlpha, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN == 1 ) { return true; }

			double dDen = double( _sN - 1 );
			for ( auto I = _sN; I--; ) {
				double dCos = std::cos( 2.0 * EE_PI * double( I ) / dDen );
				double dV = _dAlpha - (1.0 - _dAlpha) * dCos;
				_vRet[I] = _tType( dV );
			}
			return true;
		}

		/**
		 * Creates a Hann window.
		 * 
		 * \param _sN	Number of samples.  Generally an odd number.
		 * \param _vRet	The returned vector.
		 * \return		Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						HannWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _vRet.size() - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			for ( auto I = _sN; I--; ) {
				_vRet[I] = _tType( 0.5 - 0.5 * std::cos( dTauInvMax * I ) );
			}

			return true;
		}

		/**
		 * Creates a Hamming window.
		 * 
		 * \param _sN	Number of samples.  Generally an odd number.
		 * \param _vRet	The returned vector.
		 * \return		Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						HammingWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _vRet.size() - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			for ( auto I = _sN; I--; ) {
				_vRet[I] = _tType( 0.53836 - 0.46164 * std::cos( dTauInvMax * I ) );
			}

			return true;
		}

		/**
		 * Creates a Kaiser window.
		 * 
		 * \param _sN		Number of samples.  Generally an odd number.
		 * \param _tBeta	The beta parameter.
		 * \param _vRet		The returned vector.
		 * \return			Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						KaiserWindow( size_t _sN, _tType _tBeta, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			double dDenominator = Bessel0( double( _tBeta ) );

			for ( auto I = _sN; I--; ) {
				double dRatio = 0.0;
				if ( _sN > 1 ) {
					dRatio = (2.0 * I) / (_sN - 1) - 1.0;
				}
				double dArg = _tBeta * std::sqrt( 1.0 - dRatio * dRatio );
				_vRet[I] = _tType( Bessel0( dArg ) / dDenominator );
			}

			return true;
		}

		/**
		 * Kaiser-Bessel derived (KBD) window.
		 * 
		 * \brief
		 * Returns a symmetric Kaiser-Bessel derived window of length N with shape 0 -> 1 -> 0,
		 * matching scipy.signal.windows.kaiser_bessel_derived (symmetry always symmetric here).
		 * 
		 * The algorithm:
		 *   Let N = _sN and H = ceil(N/2).
		 *   1) Build a Kaiser window k of length H.
		 *   2) Compute cumulative sum c[n] = sum_{i=0..n} k[i].
		 *   3) Normalize: h[n] = sqrt( c[n] / c[H-1] ),  n = 0..H-1.  (0 -> 1).
		 *   4) Mirror h[] to get full length N:
		 *       If N is odd (N = 2H-1):
		 *           w[0..H-1]   = h[0..H-1]
		 *           w[H..N-1]   = h[H-2..0]
		 *       If N is even (N = 2H):
		 *           w[0..H-1]   = h[0..H-1]
		 *           w[H..N-1]   = h[H-1..0]
		 * 
		 * \param _sN       Number of samples.
		 * \param _dBeta    Beta parameter for the underlying Kaiser window.
		 * \param _vRet     Returned vector (size _sN), symmetric 0 -> 1 -> 0.
		 * \return          Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						KaiserBesselDerivedWindow( size_t _sN, double _dBeta, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			try {
				const bool bOdd   = ((_sN & 1) != 0);
				const size_t sHalf = (_sN + 1) / 2;        // H = ceil(N/2).

				// ---- Build Kaiser window of length sHalf.
				std::vector<double> vKaiser( sHalf );
				const double dI0Beta = Bessel0( EE_PI * _dBeta );
				const size_t sLenMinus1 = (sHalf > 1 ? (sHalf - 1) : 1);

				for ( size_t I = 0; I < sHalf; ++I ) {
					double dX   = (2.0 * double( I ) / double( sLenMinus1 )) - 1.0;
					double dRad = std::sqrt( std::max( 0.0, 1.0 - dX * dX ) );
					double dVal = Bessel0( EE_PI * _dBeta * dRad ) / dI0Beta;
					vKaiser[I] = dVal;
				}

				// ---- Cumulative sum of Kaiser.
				std::vector<double> vCum( sHalf );
				double dSum = 0.0;
				for ( size_t I = 0; I < sHalf; ++I ) {
					dSum += vKaiser[I];
					vCum[I] = dSum;
				}
				double dNorm = vCum[sHalf - 1];
				if ( dNorm == 0.0 ) { dNorm = 1.0; }

				// ---- Half KBD window: 0 -> 1.
				std::vector<double> vHalf( sHalf );
				for ( size_t I = 0; I < sHalf; ++I ) {
					vHalf[I] = std::sqrt( vCum[I] / dNorm );
				}

				// ---- Mirror to full symmetric window: 0 -> 1 -> 0.
				if ( bOdd ) {
					// N = 2*H - 1
					// Left + center:
					for ( size_t I = 0; I < sHalf; ++I ) {
						_vRet[I] = _tType( vHalf[I] );
					}
					// Right (mirror excluding center).
					for ( size_t I = 0; I < sHalf - 1; ++I ) {
						_vRet[_sN-1-I] = _tType( vHalf[I] );
					}
				}
				else {
					// N = 2*H
					// Left:
					for ( size_t I = 0; I < sHalf; ++I ) {
						_vRet[I] = _tType( vHalf[I] );
					}
					// Right (mirror including last half sample).
					for ( size_t I = 0; I < sHalf; ++I ) {
						_vRet[_sN-1-I] = _tType( vHalf[I] );
					}
				}

				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Creates a Lanczos window.
		 * 
		 * \param _sN		Number of samples.  Generally an odd number.
		 * \param _vRet		The returned vector.
		 * \return			Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						LanczosWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			const double dNMinus1  = _sN ? (_sN - 1.0) : 0.0;
			const double dHalf = dNMinus1 / 2.0;
			for ( auto I = _sN; I--; ) {
				_vRet[I] = _tType( Sinc( ((std::fabs( I - dHalf ) * 2.0) / dNMinus1) ) );	// Refactored to produce fully symmetrical values.
				//_vRet[I] = _tType( Sinc( ((I * 2.0) / dNMinus1) - 1.0 ) );
			}

			return true;
		}

		/**
		 * Creates a Nuttall window.
		 * 
		 * \param _sN	Number of samples.  Generally an odd number.
		 * \param _vRet	The returned vector.
		 * \return		Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						NuttallWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			constexpr double dA0 = 0.355768;			// 0.35576799999999997314859001562581397593021392822265625
			constexpr double dA1 = 0.487396;			// 0.487395999999999995910826555700623430311679840087890625
			constexpr double dA2 = 0.144232;			// 0.1442319999999999990958343687452725134789943695068359375
			constexpr double dA3 = 0.012604;			// 0.0126040000000000006197264923457623808644711971282958984375

			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN <= 1 ) { return true; }

			const double dTau = 2.0 * std::numbers::pi;
			size_t stMax = _vRet.size() - 1;
			double dInvMax = 1.0 / stMax;
			double dTauInvMax = dTau * dInvMax;
			double dTauInvMax2 = 2.0 * dTauInvMax;
			double dTauInvMax3 = 3.0 * dTauInvMax;
			for ( auto I = _sN; I--; ) {
				_vRet[I] = _tType( dA0 - dA1 * std::cos( dTauInvMax * I ) + dA2 * std::cos( dTauInvMax2 * I ) - dA3 * std::cos( dTauInvMax3 * I ) );
			}

			return true;
		}

		/**
		 * Parzen window.
		 * 
		 * \brief
		 * Returns a Parzen (de la Vallée Poussin) window:
		 * Let m = (N-1)/2 and r = |n - m| / m.
		 *   if 0 <= r <= 0.5:
		 *       w = 1 - 6 r^2 + 6 r^3
		 *   else if 0.5 < r <= 1:
		 *       t = 1 - r
		 *       w = 2 t^3
		 *   else:
		 *       w = 0
		 * 
		 * \param _sN   Number of samples.
		 * \param _vRet Returned vector.
		 * \return      Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool                     ParzenWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN == 1 ) { return true; }

			double dM = (double( _sN ) - 1.0) * 0.5;
			for ( auto I = _sN; I--; ) {
				double dR = std::fabs( double( I ) - dM ) / dM;
				double dV = 0.0;
				if ( dR <= 0.5 ) {
					double dR2 = dR * dR;
					double dR3 = dR2 * dR;
					dV = 1.0 - (6.0 * dR2) + (6.0 * dR3);
				}
				else if ( dR <= 1.0 ) {
					double dT = 1.0 - dR;
					double dT2 = dT * dT;
					double dT3 = dT2 * dT;
					dV = 2.0 * dT3;
				}
				_vRet[I] = _tType( dV );
			}
			return true;
		}

		/**
		 * Generates a Taylor window.
		 *
		 * \param _sN		Number of samples.
		 * \param _iNBar	Number of nearly constant level sidelobes adjacent to the mainlobe (nbar).
		 * \param _dSll	Desired sidelobe level (positive, in dB) relative to the mainlobe.
		 * \param _bNorm	If true, normalize so the maximum sample is 1.0.
		 * \param _vRet		The returned vector.
		 * \return			Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						TaylorWindow( size_t _sN, int _iNBar, double _dSll, bool _bNorm, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN == 1 ) { return true; }
			try {
				if ( _dSll < 0.0 ) { _dSll = -_dSll; }

				constexpr double dTwoPi = EE_PI * 2.0;
				const double dNBar = static_cast<double>(_iNBar);

				// A = acosh(10^(SLL/20)) / pi  (SLL positive).
				const double dA = std::acosh( std::pow( 10.0, _dSll / 20.0 ) ) / EE_PI;

				// sigma^2 term (often written as S^2 or similar).
				const double dSp2 = (dNBar * dNBar) / (dA * dA + (dNBar - 0.5) * (dNBar - 0.5));

				// Precompute F_m for m = 1..nbar-1.
				std::vector<double> vFm;
				vFm.resize( static_cast<size_t>(_iNBar > 1 ? (_iNBar - 1) : 0) );

				for ( int M = 1; M < _iNBar; ++M ) {
					const double dM = static_cast<double>( M );

					double dNum = 1.0;
					double dDen = 1.0;

					for ( int N = 1; N < _iNBar; ++N ) {
						const double dN = static_cast<double>( N );

						// Numerator product term.
						const double dNumTerm =
							1.0 - (dM * dM) / (dSp2 * (dA * dA + (dN - 0.5) * (dN - 0.5)));
						dNum *= dNumTerm;

						// Denominator product term (exclude N == M).
						if ( N != M ) {
							const double dDenTerm = 1.0 - (dM * dM) / (dN * dN);
							dDen *= dDenTerm;
						}
					}

					// (-1)^(M+1) without calling std::pow().
					const double dSign = (M & 1) ? 1.0 : -1.0;

					vFm[static_cast<size_t>(M-1)] = (dSign * dNum) / (2.0 * dDen);
				}

				// Generate window samples.
				const double dHalfN = 0.5 * static_cast<double>( _sN );

				for ( size_t I = 0; I < _sN; ++I ) {
					const double dK = static_cast<double>( I );
					// Xi in [-0.5, 0.5) as per common Taylor window implementations.
					const double dXi = (dK - dHalfN + 0.5) / static_cast<double>( _sN );

					double dSum = 0.0;
					for ( int M = 1; M < _iNBar; ++M ) {
						dSum += vFm[static_cast<size_t>( M - 1 )] *
							std::cos( dTwoPi * static_cast<double>( M ) * dXi );
					}

					const double dVal = 1.0 + 2.0 * dSum;
					_vRet[I] = static_cast<_tType>(dVal);
				}

				// Optional normalization so that max == 1.0.
				if ( _bNorm ) {
					double dMax = 0.0;
					for ( size_t I = 0; I < _sN; ++I ) {
						const double dAbs = std::abs( static_cast<double>(_vRet[I]) );
						if ( dAbs > dMax ) { dMax = dAbs; }
					}
					if ( dMax > 0.0 ) {
						const double dInvMax = 1.0 / dMax;
						for ( size_t I = 0; I < _sN; ++I ) {
							_vRet[I] = static_cast<_tType>(static_cast<double>(_vRet[I]) * dInvMax);
						}
					}
				}

				return true;
			}
			catch ( ... ) {
				return false;
			}
		}

		/**
		 * Triangular window (SciPy's triang).
		 * 
		 * \brief
		 * Returns a symmetric triangular window with peak 1 and endpoints > 0:
		 *   center = (N-1)/2
		 *   denom  = N/2
		 *   w[n] = 1 - |(n - center) / denom|.
		 * 
		 * \param _sN   Number of samples.
		 * \param _vRet Returned vector.
		 * \return      Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						TriangWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN == 1 ) { return true; }

			double dCenter = (double( _sN ) - 1.0) * 0.5;
			double dDen = double( _sN ) * 0.5;
			for ( auto I = _sN; I--; ) {
				double dV = 1.0 - std::fabs( (double( I ) - dCenter) / dDen );
				_vRet[I] = _tType( dV );
			}
			return true;
		}

		/**
		 * Tukey window (tapered cosine).
		 * 
		 * \brief
		 * Returns a Tukey window with parameter alpha:
		 *   alpha <= 0  -> rectangular window
		 *   alpha >= 1  -> Hann window
		 *   else:
		 *       taper length L = alpha * (N-1)
		 *       first half of taper:  0 <= n < L/2
		 *           w[n] = 0.5 * (1 - cos( 2*pi*n / L ))
		 *       flat region: L/2 <= n <= (N-1) - L/2
		 *           w[n] = 1
		 *       trailing taper: symmetric.
		 * 
		 * \param _sN       Number of samples.
		 * \param _dAlpha   Alpha parameter in [0,1].
		 * \param _vRet     Returned vector.
		 * \return          Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						TukeyWindow( size_t _sN, double _dAlpha, std::vector<_tType> &_vRet ) {
			if ( !HandleTrivialWindow( _sN, _vRet ) ) { return false; }
			if ( _sN == 1 ) { return true; }

			if ( _dAlpha <= 0.0 ) {
				return BoxcarWindow<_tType>( _sN, _vRet );
			}
			if ( _dAlpha >= 1.0 ) {
				// Hann window.
				double dDen = double( _sN - 1 );
				for ( size_t I = 0; I < _sN; ++I ) {
					double dV = 0.5 * (1.0 - std::cos( 2.0 * EE_PI * double( I ) / dDen ));
					_vRet[I] = _tType( dV );
				}
				return true;
			}

			double dN1 = double( _sN - 1 );
			double dL = _dAlpha * dN1;
			double dHalfL = dL * 0.5;
			double dFlatStart = dHalfL;
			double dFlatEnd = dN1 - dHalfL;

			for ( size_t I = 0; I < _sN; ++I ) {
				double dN = double( I );
				double dV = 0.0;
				if ( dN < dFlatStart ) {
					// Rising cosine.
					dV = 0.5 * (1.0 - std::cos( 2.0 * EE_PI * dN / dL ));
				}
				else if ( dN <= dFlatEnd ) {
					// Flat region.
					dV = 1.0;
				}
				else {
					// Falling cosine (symmetric).
					double dNr = dN1 - dN;
					dV = 0.5 * (1.0 - std::cos( 2.0 * EE_PI * dNr / dL ));
				}
				_vRet[I] = _tType( dV );
			}
			return true;
		}

		/**
		 * Standard sinc() function.
		 * 
		 * \param _dX	The operand.
		 * \return		Returns sin(x*PI) / x*PI.
		 **/
		static inline double			Sinc( double _dX ) {
			_dX = std::fabs( _dX * std::numbers::pi );
			if ( _dX < 0.01 ) {
				double dXsq = _dX * _dX;
				return 1.0 + dXsq * ((-1.0 / 6.0) + dXsq * (1.0 / 120.0));
			}

			return std::sin( _dX ) / _dX;
		}

		/**
		 * \brief Computes the trapezoidal integral over 1D data with uniform spacing.
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _tDx The uniform spacing between samples.
		 * \return Returns the approximate integral.
		 **/
		template <typename _tType = double>
		static inline _tType			Trapezoid1D( const std::vector<_tType> &_vY, _tType _tDx = _tType( 1 ) ) {
			if ( _vY.size() < 2 ) { return _tType( 0 ); }

			_tType tIntegrate = _tType( 0 );

			// Interior points.
			for ( size_t I = 1; I + 1 < _vY.size(); ++I ) {
				tIntegrate += _vY[I];
			}

			// Endpoints with half weight.
			tIntegrate += _tType( 0.5 ) * (_vY.front() + _vY.back());

			return tIntegrate * _tDx;
		}

		/**
		 * \brief Computes the trapezoidal integral over 1D data with non-uniform spacing.  Call within a try/catch block.
		 * 
		 * This matches scipy.integrate.trapezoid( y, x=x ) for 1D input.
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _vX The sample locations x[i], same size as _vY.
		 * \return Returns the approximate integral.
		 **/
		template <typename _tType = double>
		static inline _tType			Trapezoid1D( const std::vector<_tType> &_vY, const std::vector<_tType> &_vX ) {
			if ( _vX.size() != _vY.size() ) {
				throw std::invalid_argument( "" );
			}
			if ( _vY.size() < 2 ) { return _tType( 0 ); }

			_tType tIntegrate = _tType( 0 );

			for ( size_t I = 0; I + 1 < _vY.size(); ++I ) {
				const _tType tDx  = _vX[I+1] - _vX[I];
				const _tType tAvg = _tType( 0.5 ) * (_vY[I] + _vY[I+1]);
				tIntegrate += tDx * tAvg;
			}

			return tIntegrate;
		}

		/**
		 * \brief Trapezoidal integration along a strided axis with uniform spacing.  Call within a try/catch block.
		 * 
		 * This is analogous to scipy.integrate.trapezoid( y, dx=dx, axis=axis ) on a
		 * view of the array along 'axis', flattened to a 1D strided line.
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The full data buffer.
		 * \param _sStart The starting index into _vY for this integration line.
		 * \param _sCount The number of samples along this line.
		 * \param _sStride The stride (in elements) between successive samples.
		 * \param _tDx The uniform spacing between samples.
		 * \return Returns the approximate integral along that line.
		 **/
		template <typename _tType = double>
		static inline _tType			TrapezoidStrided( const std::vector<_tType> &_vY,
			size_t _sStart,
			size_t _sCount,
			size_t _sStride,
			_tType _tDx = _tType( 1 ) ) {
			if ( _sCount < 2 ) { return _tType( 0 ); }

			const size_t sLastIndex = _sStart + (_sCount - 1) * _sStride;
			if ( sLastIndex >= _vY.size() ) { throw std::out_of_range( "" ); }

			_tType tIntegrate = _tType( 0 );

			// First and last samples (half weight).
			const _tType tFirst = _vY[_sStart];
			const _tType tLast  = _vY[sLastIndex];

			// Interior samples.
			for ( size_t I = 1; I + 1 < _sCount; ++I ) {
				const size_t sIdx = _sStart + I * _sStride;
				tIntegrate += _vY[sIdx];
			}

			tIntegrate += _tType( 0.5 ) * (tFirst + tLast);

			return tIntegrate * _tDx;
		}

		/**
		 * \brief Trapezoidal integration along a strided axis with non-uniform spacing.  Call within a try/catch block.
		 * 
		 * This is analogous to scipy.integrate.trapezoid( y, x=x, axis=axis ) where x is
		 * 1D and y is multi-dimensional. The integration is performed along a single
		 * strided line of y, with x giving the coordinates along that axis.
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The full data buffer.
		 * \param _sStart The starting index into _vY for this integration line.
		 * \param _sStride The stride (in elements) between successive samples.
		 * \param _vX The 1D coordinates x[0..N-1] along this axis.
		 * \return Returns the approximate integral along that line.
		 **/
		template <typename _tType = double>
		static inline _tType			TrapezoidStrided( const std::vector<_tType> &_vY,
			size_t _sStart,
			size_t _sStride,
			const std::vector<_tType> &_vX ) {
			const size_t sCount = _vX.size();

			if ( sCount < 2 ) { return _tType( 0 ); }

			const size_t sLastIndex = _sStart + (sCount - 1) * _sStride;
			if ( sLastIndex >= _vY.size() ) { throw std::out_of_range( "" ); }

			_tType tIntegrate = _tType( 0 );

			for ( size_t I = 0; I + 1 < sCount; ++I ) {
				const size_t sIdx0 = _sStart + I * _sStride;
				const size_t sIdx1 = _sStart + (I + 1) * _sStride;

				const _tType tY0  = _vY[sIdx0];
				const _tType tY1  = _vY[sIdx1];
				const _tType tDx  = _vX[I+1] - _vX[I];
				const _tType tAvg = _tType( 0.5 ) * (tY0 + tY1);

				tIntegrate += tDx * tAvg;
			}

			return tIntegrate;
		}

		/**
		 * \brief Computes the cumulative trapezoidal integral over 1D data with uniform spacing.
		 * 
		 * This matches scipy.integrate.cumulative_trapezoid( y, dx=dx, initial=None )
		 * in behavior for 1D input (no initial term).
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _tDx The uniform spacing between samples.
		 * \return Returns a vector of size _vY.size() - 1 containing the cumulative integral.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										CumulativeTrapezoid1D( const std::vector<_tType> &_vY, _tType _tDx = _tType( 1 ) ) {
			if ( _vY.size() < 2 ) { return std::vector<_tType>(); }

			std::vector<_tType> vOut;
			vOut.resize( _vY.size() - 1 );

			_tType tIntegrate = _tType( 0 );

			for ( size_t I = 1; I < _vY.size(); ++I ) {
				const _tType tArea = _tDx * _tType( 0.5 ) * (_vY[I-1] + _vY[I]);
				tIntegrate += tArea;
				vOut[I-1] = tIntegrate;
			}

			return vOut;
		}

		/**
		 * \brief Computes the cumulative trapezoidal integral over 1D data with uniform spacing and an initial value.
		 * 
		 * This matches scipy.integrate.cumulative_trapezoid( y, dx=dx, initial=initial )
		 * in behavior for 1D input (output has the same length as _vY).
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _tDx The uniform spacing between samples.
		 * \param _tInitial The initial integral value (prepended to the cumulative result).
		 * \return Returns a vector of size _vY.size() containing the cumulative integral.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										CumulativeTrapezoid1D( const std::vector<_tType> &_vY, _tType _tDx, _tType _tInitial ) {
			if ( _vY.empty() ) { return std::vector<_tType>(); }

			std::vector<_tType> vOut;
			vOut.resize( _vY.size() );

			vOut[0] = _tInitial;
			_tType tIntegrate = _tInitial;

			for ( size_t I = 1; I < _vY.size(); ++I ) {
				const _tType tArea = _tDx * _tType( 0.5 ) * (_vY[I-1] + _vY[I]);
				tIntegrate += tArea;
				vOut[I] = tIntegrate;
			}

			return vOut;
		}

		/**
		 * \brief Computes the cumulative trapezoidal integral over 1D data with non-uniform spacing.  Call within a try/catch block.
		 * 
		 * This matches scipy.integrate.cumulative_trapezoid( y, x=x, initial=None )
		 * in behavior for 1D input (no initial term).
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _vX The sample locations x[i], same size as _vY.
		 * \return Returns a vector of size _vY.size() - 1 containing the cumulative integral.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										CumulativeTrapezoid1D( const std::vector<_tType> &_vY, const std::vector<_tType> &_vX ) {
			if ( _vX.size() != _vY.size() ) { throw std::invalid_argument( "" ); }
			if ( _vY.size() < 2 ) { return std::vector<_tType>(); }

			std::vector<_tType> vOut;
			vOut.resize( _vY.size() - 1 );

			_tType tIntegrate = _tType( 0 );

			for ( size_t I = 1; I < _vY.size(); ++I ) {
				const _tType tDx   = _vX[I] - _vX[I-1];
				const _tType tArea = tDx * _tType( 0.5 ) * (_vY[I-1] + _vY[I]);
				tIntegrate += tArea;
				vOut[I-1] = tIntegrate;
			}

			return vOut;
		}

		/**
		 * \brief Computes the cumulative trapezoidal integral over 1D data with non-uniform spacing and an initial value.  Call within a try/catch block.
		 * 
		 * This matches scipy.integrate.cumulative_trapezoid( y, x=x, initial=initial )
		 * in behavior for 1D input (output has the same length as _vY).
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _vX The sample locations x[i], same size as _vY.
		 * \param _tInitial The initial integral value (prepended to the cumulative result).
		 * \return Returns a vector of size _vY.size() containing the cumulative integral.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										CumulativeTrapezoid1D( const std::vector<_tType> &_vY,
			const std::vector<_tType> &_vX,
			_tType _tInitial ) {
			if ( _vX.size() != _vY.size() ) { throw std::invalid_argument( "" ); }
			if ( _vY.empty() ) { return std::vector<_tType>(); }

			std::vector<_tType> vOut;
			vOut.resize( _vY.size() );

			vOut[0] = _tInitial;
			_tType tIntegrate = _tInitial;

			for ( size_t I = 1; I < _vY.size(); ++I ) {
				const _tType tDx   = _vX[I] - _vX[I-1];
				const _tType tArea = tDx * _tType( 0.5 ) * (_vY[I-1] + _vY[I]);
				tIntegrate += tArea;
				vOut[I] = tIntegrate;
			}

			return vOut;
		}

		/**
		 * \brief Computes the composite Simpson integral over a subrange of uniformly spaced samples.
		 * 
		 * Applies Simpson's 1/3 rule on the range [_sStart, _sEnd) of _vY, assuming uniform spacing _tDx.
		 * The number of samples in the subrange must be odd and at least 3 for Simpson's rule to apply.
		 * Ranges with fewer than 3 samples contribute 0 to the integral.
		 * 
		 * \tparam _tType The value type stored in the vector (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _sStart The starting index (inclusive).
		 * \param _sEnd The ending index (exclusive).
		 * \param _tDx The uniform spacing between samples.
		 * \return Returns the Simpson integral over the given subrange.
		 **/
		template <typename _tType = double>
		static inline _tType			SimpsonUniformRange( const std::vector<_tType> &_vY,
			size_t _sStart,
			size_t _sEnd,
			_tType _tDx ) {
			const size_t sCount = _sEnd - _sStart;
			if ( sCount < size_t( 3 ) ) { return _tType( 0 ); }

			_tType tSum = _vY[_sStart] + _vY[_sEnd-1];

			// Odd-indexed terms (4 * y_i).
			for ( size_t I = _sStart + 1; I + 1 < _sEnd; I += 2 ) {
				tSum += _tType( 4 ) * _vY[I];
			}

			// Even-indexed interior terms (2 * y_i).
			for ( size_t I = _sStart + 2; I + 1 < _sEnd; I += 2 ) {
				tSum += _tType( 2 ) * _vY[I];
			}

			return _tDx * tSum / _tType( 3 );
		}

		/**
		 * \brief Computes the composite Simpson integral over a strided subrange of uniformly spaced samples.
		 * 
		 * Applies Simpson's 1/3 rule on a strided sequence of samples in _vY. The logical indices
		 * along the integration line are [0.._sCount-1], mapped to _vY[_sStart + I * _sStride].
		 * The number of samples must be odd and at least 3 for Simpson's rule to apply.
		 * 
		 * \tparam _tType The value type stored in the vector (defaults to double).
		 * \param _vY The sample values buffer.
		 * \param _sStart The starting index into _vY for the first sample in the line.
		 * \param _sStride The stride (in elements) between successive samples.
		 * \param _sCount The number of samples along the line.
		 * \param _tDx The uniform spacing between samples.
		 * \return Returns the Simpson integral over the given strided line.
		 **/
		template <typename _tType = double>
		static inline _tType			SimpsonUniformRangeStrided( const std::vector<_tType> &_vY,
			size_t _sStart,
			size_t _sStride,
			size_t _sCount,
			_tType _tDx ) {
			if ( _sCount < size_t( 3 ) ) { return _tType( 0 ); }

			const size_t sLastIndex = _sStart + (_sCount - 1) * _sStride;
			if ( sLastIndex >= _vY.size() ) { throw std::out_of_range( "" ); }

			_tType tSum = _vY[_sStart] + _vY[sLastIndex];

			// Odd-indexed terms (4 * y_i).
			for ( size_t I = 1; I + 1 < _sCount; I += 2 ) {
				const size_t sIdx = _sStart + I * _sStride;
				tSum += _tType( 4 ) * _vY[sIdx];
			}

			// Even-indexed interior terms (2 * y_i).
			for ( size_t I = 2; I + 1 < _sCount; I += 2 ) {
				const size_t sIdx = _sStart + I * _sStride;
				tSum += _tType( 2 ) * _vY[sIdx];
			}

			return _tDx * tSum / _tType( 3 );
		}

		/**
		 * \brief Computes the Simpson integral over 1D data with uniform spacing.
		 * 
		 * This matches scipy.integrate.simpson( y, dx=dx ) for 1D input, using the default
		 * even='avg' behavior when there are an even number of samples.
		 * 
		 * \tparam _tType The value type stored in the vector (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _tDx The uniform spacing between samples.
		 * \return Returns the approximate integral.
		 **/
		template <typename _tType = double>
		static inline _tType			Simpson1D( const std::vector<_tType> &_vY, _tType _tDx = _tType( 1 ) ) {
			const size_t sCount = _vY.size();
			if ( sCount < 2 ) { return _tType( 0 ); }

			// For exactly two samples, Simpson's rule degenerates to the trapezoidal rule.
			if ( sCount == 2 ) {
				return _tDx * _tType( 0.5 ) * ( _vY[0] + _vY[1] );
			}

			// Odd number of samples -> even number of intervals: standard composite Simpson.
			if ( (sCount % 2) == 1 ) {
				return SimpsonUniformRange( _vY, 0, sCount, _tDx );
			}

			// Even number of samples: emulate SciPy's even='avg':
			// 1) Simpson on first N-1 points + trapezoid on last interval.
			// 2) Simpson on last N-1 points  + trapezoid on first interval.
			// Then average the two results.
			const size_t sLast = sCount - 1;

			const _tType tS1 = SimpsonUniformRange( _vY, 0, sLast, _tDx );
			const _tType tTrapLast = _tDx * _tType( 0.5 ) *
				(_vY[sLast - 1] + _vY[sLast]);
			const _tType tR1 = tS1 + tTrapLast;

			const _tType tS2 = SimpsonUniformRange( _vY, 1, sCount, _tDx );
			const _tType tTrapFirst = _tDx * _tType( 0.5 ) *
				(_vY[0] + _vY[1]);
			const _tType tR2 = tS2 + tTrapFirst;

			return _tType( 0.5 ) * ( tR1 + tR2 );
		}

		/**
		 * \brief Computes the Simpson integral over 1D data with non-uniform spacing.  Call within a try/catch block.
		 * 
		 * This matches scipy.integrate.simpson( y, x=x ) for 1D input, using the
		 * standard non-uniform composite Simpson's rule formulation (including the
		 * special handling for the final interval when the number of intervals is odd).
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _vX The sample locations x[i], same size as _vY.
		 * \return Returns the approximate integral.
		 **/
		template <typename _tType = double>
		static inline _tType			Simpson1D( const std::vector<_tType> &_vY, const std::vector<_tType> &_vX ) {
			if ( _vX.size() != _vY.size() ) { throw std::invalid_argument( "" ); }
			if ( _vY.size() < 2 ) { return _tType( 0 ); }

			const size_t sCount = _vX.size();
			const size_t sN = sCount - 1;     // Number of intervals.

			std::vector<_tType> vH;
			vH.resize( sN );

			for ( size_t I = 0; I < sN; ++I ) {
				vH[I] = _vX[I+1] - _vX[I];
			}

			_tType tResult = _tType( 0 );

			// Main Simpson contributions over pairs of adjacent intervals.
			for ( size_t I = 1; I < sN; I += 2 ) {
				const _tType tH0 = vH[I - 1];
				const _tType tH1 = vH[I];
				const _tType tHph = tH1 + tH0;
				const _tType tHdh = tH1 / tH0;
				const _tType tHmh = tH1 * tH0;

				tResult += (tHph / _tType( 6 )) *
					((_tType( 2 ) - tHdh) * _vY[I-1]
						+ ( ( tHph * tHph ) / tHmh ) * _vY[I]
						+ (_tType( 2 ) - _tType( 1 ) / tHdh) * _vY[I+1]);
			}

			// If there is an odd number of intervals, adjust the last one as in the standard formula.
			if ( (sN % 2) == 1 ) {
				const _tType tH0 = vH[sN-2];
				const _tType tH1 = vH[sN-1];

				tResult += _vY[sN] * (_tType( 2 ) * tH1 * tH1 + _tType( 3 ) * tH0 * tH1) /
					(_tType( 6 ) * ( tH0 + tH1 ));
				tResult += _vY[sN-1] * (tH1 * tH1 + _tType( 3 ) * tH1 * tH0) /
					(_tType( 6 ) * tH0);
				tResult -= _vY[sN-2] * (tH1 * tH1 * tH1) /
					(_tType( 6 ) * tH0 * (tH0 + tH1));
			}

			return tResult;
		}

		/**
		 * \brief Computes the Simpson integral over a strided 1D line with uniform spacing.  Call within a try/catch block.
		 * 
		 * This is analogous to scipy.integrate.simpson( y, dx=dx, axis=axis ) on a
		 * view of the array along 'axis', flattened to a 1D strided line.
		 * 
		 * \tparam _tType The value type stored in the vector (defaults to double).
		 * \param _vY The full data buffer.
		 * \param _sStart The starting index into _vY for this integration line.
		 * \param _sCount The number of samples along this line.
		 * \param _sStride The stride (in elements) between successive samples.
		 * \param _tDx The uniform spacing between samples.
		 * \return Returns the approximate integral along that line.
		 **/
		template <typename _tType = double>
		static inline _tType			SimpsonStrided( const std::vector<_tType> &_vY,
			size_t _sStart,
			size_t _sCount,
			size_t _sStride,
			_tType _tDx = _tType( 1 ) ) {
			if ( _sCount < 2 ) { return _tType( 0 ); }

			const size_t sLastIndex = _sStart + (_sCount - 1) * _sStride;
			if ( sLastIndex >= _vY.size() ) { throw std::out_of_range( "" ); }

			// For exactly two samples, Simpson's rule degenerates to the trapezoidal rule.
			if ( _sCount == 2 ) {
				const _tType tY0 = _vY[_sStart];
				const _tType tY1 = _vY[_sStart + _sStride];
				return _tDx * _tType( 0.5 ) * (tY0 + tY1);
			}

			// Odd number of samples -> even number of intervals: standard composite Simpson.
			if ( (_sCount % 2) == 1 ) {
				return SimpsonUniformRangeStrided( _vY, _sStart, _sStride, _sCount, _tDx );
			}

			// Even number of samples: emulate SciPy's even='avg':
			// 1) Simpson on first N-1 points + trapezoid on last interval.
			// 2) Simpson on last N-1 points  + trapezoid on first interval.
			// Then average the two results.
			const size_t sLast = _sCount - 1;

			const _tType tS1 = SimpsonUniformRangeStrided( _vY, _sStart, _sStride, sLast, _tDx );
			{
				const size_t sIdxA = _sStart + (sLast - 1) * _sStride;
				const size_t sIdxB = _sStart + sLast * _sStride;
				const _tType tTrapLast = _tDx * _tType( 0.5 ) * (_vY[sIdxA] + _vY[sIdxB]);
				const _tType tR1 = tS1 + tTrapLast;

				const _tType tS2 = SimpsonUniformRangeStrided( _vY,
					_sStart + _sStride,
					_sStride,
					sLast,
					_tDx );
				const _tType tTrapFirst = _tDx * _tType( 0.5 )
					* (_vY[_sStart] + _vY[_sStart+_sStride]);
				const _tType tR2 = tS2 + tTrapFirst;

				return _tType( 0.5 ) * (tR1 + tR2);
			}
		}

		/**
		 * \brief Computes the Simpson integral over a strided 1D line with non-uniform spacing.  Call within a try/catch block.
		 * 
		 * This is analogous to scipy.integrate.simpson( y, x=x, axis=axis ) where x is
		 * 1D and y is multi-dimensional. The integration is performed along a single
		 * strided line of y, with x giving the coordinates along that axis.
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The full data buffer.
		 * \param _sStart The starting index into _vY for this integration line.
		 * \param _sStride The stride (in elements) between successive samples.
		 * \param _vX The 1D coordinates x[0..N-1] along this axis.
		 * \return Returns the approximate integral along that line.
		 **/
		template <typename _tType = double>
		static inline _tType			SimpsonStrided( const std::vector<_tType> &_vY,
			size_t _sStart,
			size_t _sStride,
			const std::vector<_tType> &_vX ) {
			const size_t sCount = _vX.size();
			if ( sCount < 2 ) { return _tType( 0 ); }

			const size_t sLastIndex = _sStart + (sCount - 1) * _sStride;
			if ( sLastIndex >= _vY.size() ) { throw std::out_of_range( "" ); }

			const size_t sN = sCount - 1;     // Number of intervals.

			std::vector<_tType> vH;
			vH.resize( sN );

			for ( size_t I = 0; I < sN; ++I ) {
				vH[I] = _vX[I+1] - _vX[I];
			}

			_tType tResult = _tType( 0 );

			// Main Simpson contributions over pairs of adjacent intervals.
			for ( size_t I = 1; I < sN; I += 2 ) {
				const _tType tH0 = vH[I-1];
				const _tType tH1 = vH[I];
				const _tType tHph = tH1 + tH0;
				const _tType tHdh = tH1 / tH0;
				const _tType tHmh = tH1 * tH0;

				const size_t sIdx0 = _sStart + (I - 1) * _sStride;
				const size_t sIdx1 = _sStart + I * _sStride;
				const size_t sIdx2 = _sStart + (I + 1) * _sStride;

				tResult += (tHph / _tType( 6 )) *
					(( _tType( 2 ) - tHdh) * _vY[sIdx0] +
					((tHph * tHph) / tHmh) * _vY[sIdx1] +
					(_tType( 2 ) - _tType( 1 ) / tHdh) * _vY[sIdx2]);
			}

			// If there is an odd number of intervals, adjust the last one as in the standard formula.
			if ( (sN % 2) == 1 ) {
				const _tType tH0 = vH[sN-2];
				const _tType tH1 = vH[sN-1];

				const size_t sIdxN   = _sStart + sN * _sStride;
				const size_t sIdxNm1 = _sStart + (sN - 1) * _sStride;
				const size_t sIdxNm2 = _sStart + (sN - 2) * _sStride;

				tResult += _vY[sIdxN] * (_tType( 2 ) * tH1 * tH1 + _tType( 3 ) * tH0 * tH1) /
					(_tType( 6 ) * (tH0 + tH1));
				tResult += _vY[sIdxNm1] * (tH1 * tH1 + _tType( 3 ) * tH1 * tH0) /
					(_tType( 6 ) * tH0);
				tResult -= _vY[sIdxNm2] * (tH1 * tH1 * tH1) /
					(_tType( 6 ) * tH0 * (tH0 + tH1));
			}

			return tResult;
		}

		/**
		 * \brief Computes Simpson sub-interval integrals for equally spaced samples.  Call within a try/catch block.
		 * 
		 * Helper for cumulative Simpson integration with equal interval widths.
		 * Given N samples in _vY and N-1 spacings in _vDx, returns N-2 sub-interval
		 * integrals corresponding to the "h1" intervals (Cartwright equal-interval
		 * formula).
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _vDx The spacing between consecutive x-values, size _vY.size() - 1.
		 * \return Returns a vector of size _vY.size() - 2 with sub-interval integrals.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										CumulativeSimpsonEqualIntervalsSub( const std::vector<_tType> &_vY,
			const std::vector<_tType> &_vDx ) {
			if ( _vY.size() < size_t( 3 ) ) { return std::vector<_tType>(); }

			if ( _vDx.size() + 1 != _vY.size() ) { throw std::invalid_argument( "" ); }

			std::vector<_tType> vSub;
			vSub.resize( _vY.size() - 2 );

			for ( size_t I = 0; I + 2 < _vY.size(); ++I ) {
				const _tType tD  = _vDx[I];
				const _tType tF1 = _vY[I];
				const _tType tF2 = _vY[I + 1];
				const _tType tF3 = _vY[I + 2];

				const _tType tInner = (_tType( 5 ) * tF1 / _tType( 4 )) +
					(_tType( 2 ) * tF2) -
					(tF3 / _tType( 4 ));

				vSub[I] = tD * tInner / _tType( 3 );
			}

			return vSub;
		}

		/**
		 * \brief Computes Simpson sub-interval integrals for unequally spaced samples.  Call within a try/catch block.
		 * 
		 * Helper for cumulative Simpson integration with unequal interval widths.
		 * Given N samples in _vY and N-1 spacings in _vDx, returns N-2 sub-interval
		 * integrals corresponding to the "h1" intervals (Cartwright unequal-interval
		 * formula).
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _vDx The spacing between consecutive x-values, size _vY.size() - 1.
		 * \return Returns a vector of size _vY.size() - 2 with sub-interval integrals.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										CumulativeSimpsonUnequalIntervalsSub( const std::vector<_tType> &_vY,
			const std::vector<_tType> &_vDx ) {
			if ( _vY.size() < size_t( 3 ) ) { return std::vector<_tType>(); }

			if ( _vDx.size() + 1 != _vY.size() ) { throw std::invalid_argument( "" ); }

			std::vector<_tType> vSub;
			vSub.resize( _vY.size() - 2 );

			for ( size_t I = 0; I + 2 < _vY.size(); ++I ) {
				const _tType tX21 = _vDx[I];
				const _tType tX32 = _vDx[I + 1];

				const _tType tF1 = _vY[I];
				const _tType tF2 = _vY[I + 1];
				const _tType tF3 = _vY[I + 2];

				const _tType tX31               = tX21 + tX32;
				const _tType tX21_x31           = tX21 / tX31;
				const _tType tX21_x32           = tX21 / tX32;
				const _tType tX21x21_x31x32     = tX21_x31 * tX21_x32;
				const _tType tCoeff1            = _tType( 3 ) - tX21_x31;
				const _tType tCoeff2            = _tType( 3 ) + tX21x21_x31x32 + tX21_x31;
				const _tType tCoeff3            = -tX21x21_x31x32;

				vSub[I] = (tX21 / _tType( 6 )) * (tCoeff1 * tF1 + tCoeff2 * tF2 + tCoeff3 * tF3);
			}

			return vSub;
		}

		/**
		 * \brief Combines "h1" and "h2" Simpson sub-interval integrals and returns per-interval contributions.  Call within a try/catch block.
		 * 
		 * This mirrors SciPy's _cumulatively_sum_simpson_integrals behavior in 1D:
		 * sub-interval integrals from the left-biased (h1) and right-biased (h2)
		 * formulas are interleaved, and the last interval uses the final h2 value.
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vSubH1 The sub-interval integrals for h1 intervals (size N-2).
		 * \param _vSubH2 The sub-interval integrals for h2 intervals (size N-2).
		 * \return Returns a vector of size N-1 with per-interval contributions.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										CumulativeSimpsonCombineSubIntervals( const std::vector<_tType> &_vSubH1,
			const std::vector<_tType> &_vSubH2 ) {
			if ( _vSubH1.size() != _vSubH2.size() ) { throw std::invalid_argument( "" );
			}
			if ( _vSubH1.empty() ) { return std::vector<_tType>(); }

			const size_t sSubCount   = _vSubH1.size();       // N-2
			const size_t sIntervalCt = sSubCount + 1;  // N-1

			std::vector<_tType> vSub;
			vSub.resize( sIntervalCt );

			// sub_integrals[..., :-1:2] = sub_integrals_h1[..., ::2]
			for ( size_t J = 0; J + 1 < sIntervalCt; J += 2 ) {
				vSub[J] = _vSubH1[J];
			}

			// sub_integrals[..., 1::2] = sub_integrals_h2[..., ::2]
			for ( size_t J = 1, R = 0;
				  J < sIntervalCt && R < sSubCount;
				  J += 2, R += 2 ) {
				vSub[J] = _vSubH2[R];
			}

			// sub_integrals[..., -1] = sub_integrals_h2[..., -1]
			vSub[sIntervalCt - 1] = _vSubH2[sSubCount - 1];

			return vSub;
		}

		/**
		 * \brief Cumulatively integrates 1D data using the composite Simpson's 1/3 rule with uniform spacing.
		 * 
		 * For fewer than 3 points, falls back to the cumulative trapezoidal rule.
		 * This matches scipy.integrate.cumulative_simpson( y, dx=dx, x=None, initial=None ) for 1D input.
		 * 
		 * \tparam _tType The value type stored in the vector (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _tDx The uniform spacing between samples.
		 * \return Returns a vector of size _vY.size() - 1 containing the cumulative integral.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										CumulativeSimpson1D( const std::vector<_tType> &_vY, _tType _tDx = _tType( 1 ) ) {
			if ( _vY.size() < 2 ) { return std::vector<_tType>(); }

			// For 2 points, no Simpson; use cumulative trapezoid.
			if ( _vY.size() < size_t( 3 ) ) {
				std::vector<_tType> vOut;
				vOut.resize( _vY.size() - 1 );

				_tType tIntegrate = _tType( 0 );
				for ( size_t I = 1; I < _vY.size(); ++I ) {
					const _tType tArea = _tDx * _tType( 0.5 ) * (_vY[I-1] + _vY[I]);
					tIntegrate += tArea;
					vOut[I-1] = tIntegrate;
				}
				return vOut;
			}

			// Build dx array (equal intervals).
			std::vector<_tType> vDx;
			vDx.resize( _vY.size() - 1 );
			for ( size_t I = 0; I < vDx.size(); ++I ) {
				vDx[I] = _tDx;
			}

			// h1 contributions.
			std::vector<_tType> vSubH1 = CumulativeSimpsonEqualIntervalsSub( _vY, vDx );

			// h2 contributions: reverse inputs, compute, then reverse results.
			std::vector<_tType> vYRev( _vY.rbegin(), _vY.rend() );
			std::vector<_tType> vDxRev( vDx.rbegin(), vDx.rend() );
			std::vector<_tType> vSubH2Rev = CumulativeSimpsonEqualIntervalsSub( vYRev, vDxRev );
			std::vector<_tType> vSubH2( vSubH2Rev.rbegin(), vSubH2Rev.rend() );

			// Combine h1 and h2 into per-interval contributions.
			std::vector<_tType> vSub = CumulativeSimpsonCombineSubIntervals( vSubH1, vSubH2 );

			// Cumulative sum of interval contributions.
			std::vector<_tType> vOut;
			vOut.resize( vSub.size() );

			_tType tIntegrate = _tType( 0 );
			for ( size_t I = 0; I < vSub.size(); ++I ) {
				tIntegrate += vSub[I];
				vOut[I] = tIntegrate;
			}

			return vOut;
		}

		/**
		 * \brief Cumulatively integrates 1D data using the composite Simpson's 1/3 rule with uniform spacing and an initial value.
		 * 
		 * For fewer than 3 points, falls back to the cumulative trapezoidal rule.
		 * The result matches scipy.integrate.cumulative_simpson( y, dx=dx, x=None, initial=initial )
		 * for 1D input.
		 * 
		 * \tparam _tType The value type stored in the vector (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _tDx The uniform spacing between samples.
		 * \param _tInitial The initial integral value (inserted at the beginning and added to the cumulative result).
		 * \return Returns a vector of size _vY.size() containing the cumulative integral.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										CumulativeSimpson1D( const std::vector<_tType> &_vY,
			_tType _tDx,
			_tType _tInitial ) {
			if ( _vY.empty() ) { return std::vector<_tType>(); }

			// Base cumulative Simpson (no initial).
			std::vector<_tType> vBase = CumulativeSimpson1D( _vY, _tDx );

			std::vector<_tType> vOut;
			vOut.resize( _vY.size() );

			vOut[0] = _tInitial;
			for ( size_t I = 0; I < vBase.size(); ++I ) {
				vOut[I+1] = _tInitial + vBase[I];
			}

			return vOut;
		}

		/**
		 * \brief Cumulatively integrates 1D data using the composite Simpson's 1/3 rule with non-uniform spacing.  Call within a try/catch block.
		 * 
		 * For fewer than 3 points, falls back to the cumulative trapezoidal rule with non-uniform spacing.
		 * This matches scipy.integrate.cumulative_simpson( y, x=x, dx=1.0, initial=None ) for 1D input.
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _vX The sample locations x[i], same size as _vY and strictly increasing.
		 * \return Returns a vector of size _vY.size() - 1 containing the cumulative integral.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										CumulativeSimpson1D( const std::vector<_tType> &_vY, const std::vector<_tType> &_vX ) {
			if ( _vX.size() != _vY.size() ) { throw std::invalid_argument( "" ); }

			if ( _vY.size() < 2 ) { return std::vector<_tType>(); }

			// Build dx and check strictly increasing x.
			std::vector<_tType> vDx;
			vDx.resize( _vX.size() - 1 );
			for ( size_t I = 1; I < _vX.size(); ++I ) {
				const _tType tDx = _vX[I] - _vX[I - 1];
				if ( tDx <= _tType( 0 ) ) {
					throw std::invalid_argument( "" );
				}
				vDx[I - 1] = tDx;
			}

			// For 2 points, no Simpson; use cumulative trapezoid.
			if ( _vY.size() < size_t( 3 ) ) {
				std::vector<_tType> vOut;
				vOut.resize( _vY.size() - 1 );

				_tType tIntegrate = _tType( 0 );
				for ( size_t I = 1; I < _vY.size(); ++I ) {
					const _tType tArea = vDx[I-1] * _tType( 0.5 ) * (_vY[I-1] + _vY[I]);
					tIntegrate += tArea;
					vOut[I - 1] = tIntegrate;
				}
				return vOut;
			}

			// h1 contributions (unequal intervals).
			std::vector<_tType> vSubH1 = CumulativeSimpsonUnequalIntervalsSub( _vY, vDx );

			// h2 contributions: reverse inputs, compute, then reverse results.
			std::vector<_tType> vYRev( _vY.rbegin(), _vY.rend() );
			std::vector<_tType> vDxRev( vDx.rbegin(), vDx.rend() );
			std::vector<_tType> vSubH2Rev = CumulativeSimpsonUnequalIntervalsSub( vYRev, vDxRev );
			std::vector<_tType> vSubH2( vSubH2Rev.rbegin(), vSubH2Rev.rend() );

			// Combine h1 and h2 into per-interval contributions.
			std::vector<_tType> vSub = CumulativeSimpsonCombineSubIntervals( vSubH1, vSubH2 );

			// Cumulative sum of interval contributions.
			std::vector<_tType> vOut;
			vOut.resize( vSub.size() );

			_tType tIntegrate = _tType( 0 );
			for ( size_t I = 0; I < vSub.size(); ++I ) {
				tIntegrate += vSub[I];
				vOut[I] = tIntegrate;
			}

			return vOut;
		}

		/**
		 * \brief Cumulatively integrates 1D data using the composite Simpson's 1/3 rule with non-uniform spacing and an initial value.  Call within a try/catch block.
		 * 
		 * For fewer than 3 points, falls back to the cumulative trapezoidal rule with non-uniform spacing.
		 * This matches scipy.integrate.cumulative_simpson( y, x=x, initial=initial ) for 1D input.
		 * 
		 * \tparam _tType The value type stored in the vectors (defaults to double).
		 * \param _vY The sample values y[i].
		 * \param _vX The sample locations x[i], same size as _vY and strictly increasing.
		 * \param _tInitial The initial integral value (inserted at the beginning and added to the cumulative result).
		 * \return Returns a vector of size _vY.size() containing the cumulative integral.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										CumulativeSimpson1D( const std::vector<_tType> &_vY,
			const std::vector<_tType> &_vX,
			_tType _tInitial ) {
			if ( _vY.empty() ) { return std::vector<_tType>(); }

			std::vector<_tType> vBase = CumulativeSimpson1D( _vY, _vX );

			std::vector<_tType> vOut;
			vOut.resize( _vY.size() );

			vOut[0] = _tInitial;
			for ( size_t I = 0; I < vBase.size(); ++I ) {
				vOut[I+1] = _tInitial + vBase[I];
			}

			return vOut;
		}

		/**
		 * \brief Romberg integration over a 1D array of equally spaced samples. Must be called within a try/catch block.
		 * 
		 * The number of samples must be 2^k + 1 for some integer k >= 1. The spacing between samples is _tDx.
		 * This mirrors scipy.integrate.romb( y, dx=dx ) for 1D input.
		 * 
		 * \tparam _tType The floating-point type used for accumulation and return (defaults to double).
		 * \param _vY The vector of function samples y[0..N-1].
		 * \param _tDx The spacing between consecutive samples.
		 * \return Returns the Romberg approximation to the integral over the sampled interval.
		 * \throw std::invalid_argument Thrown if the input size is invalid.
		 **/
		template <typename _tType = double>
		static inline _tType			Romb1D( const std::vector<_tType> &_vY, _tType _tDx = _tType( 1 ) ) {
			const size_t stNsamps = _vY.size();
			if ( stNsamps < 2 ) { throw std::invalid_argument( "" ); }

			const size_t stNinterv = stNsamps - 1;
			if ( (stNinterv & (stNinterv - 1)) != 0 ) { throw std::invalid_argument( "" ); }

			// Number of Romberg levels k such that stNinterv == 2^k.
			size_t stK = 0;
			{
				size_t stTmp = stNinterv;
				while ( stTmp > 1 ) {
					stTmp >>= 1;
					++stK;
				}
			}

			// Romberg table rows (we only keep previous and current).
			std::vector<_tType> vRowPrev;
			std::vector<_tType> vRowCurr;
			vRowPrev.reserve( stK + 1 );
			vRowCurr.reserve( stK + 1 );

			const size_t stLastIndex = stNsamps - 1;

			// Build T_l (trapezoid approximations) for l = 0..k from the pre-sampled data.
			// For each level l:
			//  * Number of intervals  = 2^l
			//  * Index step in samples = 2^(k-l)
			//  * Step width h_l = step * dx
			for ( size_t stL = 0; stL <= stK; ++stL ) {
				const size_t stStep      = 1 << (stK - stL);   // Index step between samples.
				const size_t stIntervals = 1 << stL;           // Number of intervals.
				const _tType tH          = _tDx * static_cast<_tType>( stStep );

				// Trapezoid T_l over this coarser grid.
				_tType tSum = _vY[0] + _vY[stLastIndex];
				for ( size_t stJ = 1; stJ < stIntervals; ++stJ ) {
					const size_t stIdx = stJ * stStep;
					tSum += _vY[stIdx];
				}
				const _tType tT = tH * tSum * _tType( 0.5 );

				vRowCurr.resize( stL + 1 );
				vRowCurr[0] = tT;

				// Richardson extrapolation for this row.
				for ( size_t stM = 1; stM <= stL; ++stM ) {
					const _tType tFourPow = static_cast<_tType>(1 << (2 * stM));
					const _tType tDenom   = tFourPow - _tType( 1 );
					const _tType tPrev    = vRowCurr[stM-1];

					vRowCurr[stM] = tPrev + (tPrev - vRowPrev[stM-1]) / tDenom; 
				}

				vRowPrev = vRowCurr;
			}

			return vRowPrev[stK];
		}

		/**
		 * \brief Romberg integration over a strided 1D line of equally spaced samples. Must be called within a try/catch block.
		 * 
		 * This is the "axis" analogue of Romb1D. It integrates along the logical sequence
		 * 
		 *     y[k] = _vY[_sStart + k * _sStride],  k = 0..(_sCount-1),
		 * 
		 * where the length _sCount must be 2^k + 1 for some integer k >= 1. The spacing
		 * between samples along this line is _tDx. This mirrors scipy.integrate.romb( y, dx=dx, axis=axis )
		 * for one axis slice at a time.
		 * 
		 * \tparam _tType The floating-point type used for accumulation and return (defaults to double).
		 * \param _vY The full data buffer.
		 * \param _sStart The starting index in _vY for this integration line.
		 * \param _sCount The number of samples along this line (must be 2^k + 1).
		 * \param _sStride The stride (in elements) between successive samples along the line.
		 * \param _tDx The spacing between consecutive samples along the line.
		 * \return Returns the Romberg approximation to the integral over the sampled line.
		 * \throw std::invalid_argument Thrown if _sCount is invalid.
		 * \throw std::out_of_range Thrown if the line exceeds _vY.size().
		 **/
		template <typename _tType = double>
		_tType							RombStrided( const std::vector<_tType> &_vY,
			size_t _sStart,
			size_t _sCount,
			size_t _sStride,
			_tType _tDx = _tType( 1 ) ) {
			if ( _sCount < 2 ) { throw std::invalid_argument( "" ); }

			const size_t stLastIndex = _sStart + (_sCount - 1) * _sStride;
			if ( stLastIndex >= _vY.size() ) { throw std::out_of_range( "" ); }

			const size_t stNinterv = _sCount - 1;
			if ( (stNinterv & (stNinterv - 1)) != 0 ) { throw std::invalid_argument( "" ); }

			// Number of Romberg levels k such that stNinterv == 2^k.
			size_t stK = 0;
			{
				size_t stTmp = stNinterv;
				while ( stTmp > 1 ) {
					stTmp >>= 1;
					++stK;
				}
			}

			std::vector<_tType> vRowPrev;
			std::vector<_tType> vRowCurr;
			vRowPrev.reserve( stK + 1 );
			vRowCurr.reserve( stK + 1 );

			// Build T_l (trapezoid approximations) for l = 0..k along the strided line.
			for ( size_t stL = 0; stL <= stK; ++stL ) {
				const size_t stStep			= 1 << (stK - stL);   // Step in "logical sample index" units.
				const size_t stIntervals	= 1 << stL;           // Number of intervals.
				const _tType tH				= _tDx * static_cast<_tType>(stStep);

				const size_t stIdx0			= _sStart;
				const size_t stIdxLast		= _sStart + (_sCount - 1) * _sStride;

				_tType tSum = _vY[stIdx0] + _vY[stIdxLast];

				for ( size_t stJ = 1; stJ < stIntervals; ++stJ ) {
					const size_t stLogicalIdx = stJ * stStep;
					const size_t stIdx        = _sStart + stLogicalIdx * _sStride;
					tSum += _vY[stIdx];
				}

				const _tType tT = tH * tSum * _tType( 0.5 );

				vRowCurr.resize( stL + 1 );
				vRowCurr[0] = tT;

				for ( size_t stM = 1; stM <= stL; ++stM ) {
					const _tType tFourPow = static_cast<_tType>(1 << (2 * stM));
					const _tType tDenom   = tFourPow - _tType( 1 );
					const _tType tPrev    = vRowCurr[stM-1];

					vRowCurr[stM] = tPrev +
						(tPrev - vRowPrev[stM-1]) / tDenom;
				}

				vRowPrev = vRowCurr;
			}

			return vRowPrev[stK];
		}

		/**
		 * \brief Creates a 1D vector of linearly spaced values. Must be called within a try/catch block.
		 * 
		 * This function emulates numpy.linspace for 1D data:
		 * 
		 *   - If _stNum == 0, returns an empty vector.
		 *   - If _stNum == 1, returns {_tStart} regardless of _bEndpoint.
		 *   - If _bEndpoint is true, the last element equals _tStop.
		 *   - If _bEndpoint is false, the sequence goes up to but does not include _tStop.
		 * 
		 * If _ptStep is not nullptr, the step size used to generate the sequence is
		 * written to *_ptStep (similar to retstep=True in NumPy).
		 * 
		 * \tparam _tType
		 *      The value type used for the sequence (defaults to double).
		 * \param _tStart
		 *      The starting value of the sequence.
		 * \param _tStop
		 *      The final value of the sequence (inclusive if _bEndpoint is true).
		 * \param _stNum
		 *      The number of samples to generate.
		 * \param _bEndpoint
		 *      If true, _tStop is included as the last sample. If false, _tStop is excluded.
		 * \param _ptStep
		 *      Optional pointer that, if non-null, receives the step size used.
		 * \return
		 *      Returns a vector containing the generated samples.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										Linspace( _tType _tStart,
			_tType _tStop,
			size_t _stNum = 50,
			bool _bEndpoint = true,
			_tType * _ptStep = nullptr ) {
			std::vector<_tType> vOut;

			if ( _stNum == 0 ) {
				if ( _ptStep ) {
					(*_ptStep) = _tType( 0 );
				}
				return vOut;
			}

			vOut.resize( _stNum );

			if ( _stNum == 1 ) {
				vOut[0] = _tStart;
				if ( _ptStep ) {
					(*_ptStep) = _tType( 0 );
				}
				return vOut;
			}

			const double dDiv = _bEndpoint ?
				static_cast<double>(_stNum - 1.0) :
				static_cast<double>(_stNum);

			const double dStep = static_cast<double>(_tStop - _tStart) / dDiv;

			if ( _ptStep ) {
				(*_ptStep) = _tType( dStep );
			}

			for ( size_t I = 0; I < _stNum; ++I ) {
				vOut[I] = static_cast<_tType>(_tStart + dStep * I);
			}

			// For _bEndpoint == true, the formula already gives vOut.back() == _tStop
			// algebraically, but a direct assignment avoids any accumulated rounding.
			if ( _bEndpoint ) {
				vOut[_stNum-1] = static_cast<_tType>(_tStop);
			}

			return vOut;
		}

		/**
		 * \brief Creates a 1D vector with values in the half-open interval [0, _tStop). Must be called within a try/catch block.
		 * 
		 * This emulates numpy.arange( stop ) for 1D data.
		 * 
		 * \tparam _tType
		 *      The value type used for the sequence (defaults to double).
		 * \param _tStop
		 *      The end of the half-open interval [0, _tStop).
		 * \return
		 *      Returns a vector containing the generated samples.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										Arange( _tType _tStop ) {
			return Arange<_tType>( _tType( 0 ), _tStop, _tType( 1 ) );
		}

		/**
		 * \brief Creates a 1D vector with values in the half-open interval [_tStart, _tStop) with a given step. Must be called within a try/catch block.
		 * 
		 * This emulates numpy.arange( start, stop, step, dtype=_tOut ) for 1D data.
		 * 
		 * For floating-point arguments, the length is computed as:
		 *     ceil((stop - start) / step)
		 * 
		 * The actual step used to populate the array matches NumPy's documented behavior:
		 *     step_actual = dtype(start + step) - dtype(start)
		 * 
		 * \tparam _tOut
		 *      The output element type (NumPy's dtype). Defaults to double.
		 * \tparam _tStart
		 *      The type of the start parameter.
		 * \tparam _tStop
		 *      The type of the stop parameter.
		 * \tparam _tStep
		 *      The type of the step parameter.
		 * \param _tStartVal
		 *      The start of the half-open interval.
		 * \param _tStopVal
		 *      The end of the half-open interval.
		 * \param _tStepVal
		 *      The step between consecutive samples (must be non-zero).
		 * \return
		 *      Returns a vector containing the generated samples.
		 * \throw std::invalid_argument
		 *      Thrown if _tStepVal is zero.
		 **/
		template <typename _tOut = double, typename _tStart, typename _tStop, typename _tStep>
		static inline std::vector<_tOut>Arange( _tStart _tStartVal, _tStop _tStopVal, _tStep _tStepVal ) {
			const long double ldStart = static_cast<long double>(_tStartVal);
			const long double ldStop  = static_cast<long double>(_tStopVal);
			const long double ldStep  = static_cast<long double>(_tStepVal);

			if ( ldStep == static_cast<long double>(0) ) { throw std::invalid_argument( "" ); }

			size_t stCount = 0;

			// Only produce values if we can move from start toward stop using the sign of step.
			if ( (ldStep > static_cast<long double>(0) && ldStart < ldStop) ||
				 (ldStep < static_cast<long double>(0) && ldStart > ldStop) ) {
				const long double ldSpan = ldStop - ldStart;
				const long double ldRawCount = ldSpan / ldStep;

				long double ldCeilCount = std::ceill( ldRawCount );
				if ( ldCeilCount < static_cast<long double>(0) ) {
					ldCeilCount = static_cast<long double>(0);
				}

				stCount = static_cast<size_t>(ldCeilCount);
			}
			else {
				stCount = 0;
			}

			std::vector<_tOut> vOut;
			vOut.resize( stCount );

			if ( stCount == 0 ) { return vOut; }

			// NumPy uses dtype(start + step) - dtype(start) as the internal step,
			// which reproduces the documented "instabilities" for integer dtypes.
			const _tOut tStartCast = static_cast<_tOut>( ldStart );
			vOut[0] = tStartCast;

			if ( stCount > 1 ) {
				const _tOut tNextCast = static_cast<_tOut>(ldStart + ldStep);
				const _tOut tStepActual = static_cast<_tOut>(tNextCast - tStartCast);

				for ( size_t I = 1; I < stCount; ++I ) {
					vOut[I] = static_cast<_tOut>(vOut[I-1] + tStepActual);
				}
			}

			return vOut;
		}

		/**
		 * \brief Creates a 1D vector filled with zeros. Must be called within a try/catch block.
		 * 
		 * This emulates numpy.zeros( shape ) for 1D data.
		 * 
		 * \tparam _tType
		 *      The value type stored in the vector (defaults to double).
		 * \param _stCount
		 *      The number of elements.
		 * \return
		 *      Returns a vector of length _stCount, filled with zero.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										Zeros( size_t _stCount ) {
			return std::vector<_tType>( _stCount, _tType( 0 ) );
		}

		/**
		 * \brief Creates a 1D vector filled with zeros, matching the length of another vector. Must be called within a try/catch block.
		 * 
		 * This emulates numpy.zeros_like( a ) for 1D data.
		 * 
		 * \tparam _tType
		 *      The value type stored in the vectors.
		 * \param _vRef
		 *      The reference vector whose length is used.
		 * \return
		 *      Returns a vector of length _vRef.size(), filled with zero.
		 **/
		template <typename _tType>
		static inline std::vector<_tType>
										ZerosLike( const std::vector<_tType> &_vRef ) {
			return std::vector<_tType>( _vRef.size(), _tType( 0 ) );
		}

		/**
		 * \brief Creates a 1D vector filled with ones. Must be called within a try/catch block.
		 * 
		 * This emulates numpy.ones( shape ) for 1D data.
		 * 
		 * \tparam _tType
		 *      The value type stored in the vector (defaults to double).
		 * \param _stCount
		 *      The number of elements.
		 * \return
		 *      Returns a vector of length _stCount, filled with one.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										Ones( size_t _stCount ) {
			return std::vector<_tType>( _stCount, _tType( 1 ) );
		}

		/**
		 * \brief Creates a 1D vector filled with ones, matching the length of another vector. Must be called within a try/catch block.
		 * 
		 * This emulates numpy.ones_like( a ) for 1D data.
		 * 
		 * \tparam _tType
		 *      The value type stored in the vectors.
		 * \param _vRef
		 *      The reference vector whose length is used.
		 * \return
		 *      Returns a vector of length _vRef.size(), filled with one.
		 **/
		template <typename _tType>
		static inline std::vector<_tType>
										OnesLike( const std::vector<_tType> &_vRef ) {
			return std::vector<_tType>( _vRef.size(), _tType( 1 ) );
		}

		/**
		 * \brief Creates a 1D vector filled with a constant value. Must be called within a try/catch block.
		 * 
		 * This emulates numpy.full( shape, fill_value ) for 1D data.
		 * 
		 * \tparam _tType
		 *      The value type stored in the vector (defaults to double).
		 * \param _stCount
		 *      The number of elements.
		 * \param _tValue
		 *      The value with which to fill the vector.
		 * \return
		 *      Returns a vector of length _stCount, filled with _tValue.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										Full( size_t _stCount, _tType _tValue ) {
			return std::vector<_tType>( _stCount, _tValue );
		}

		/**
		 * \brief Creates a 1D vector filled with a constant value, matching the length of another vector. Must be called within a try/catch block.
		 * 
		 * This emulates numpy.full_like( a, fill_value ) for 1D data.
		 * 
		 * \tparam _tType
		 *      The value type stored in the vectors.
		 * \param _vRef
		 *      The reference vector whose length is used.
		 * \param _tValue
		 *      The value with which to fill the vector.
		 * \return
		 *      Returns a vector of length _vRef.size(), filled with _tValue.
		 **/
		template <typename _tType>
		static inline std::vector<_tType>
										FullLike( const std::vector<_tType> &_vRef, _tType _tValue ) {
			return std::vector<_tType>( _vRef.size(), _tValue );
		}

		/**
		 * \brief Creates a 1D vector of numbers spaced evenly on a log scale. Must be called within a try/catch block.
		 * 
		 * This emulates numpy.logspace( start, stop, num=50, endpoint=True, base=10.0, dtype=_tType, axis=0 )
		 * for real dtypes in 1D.
		 * 
		 * Floating-point _tType:
		 *     Behaves like NumPy's logspace for real inputs.
		 * Integer _tType:
		 *     Behaves like np.around(np.logspace(...)).astype(int), which avoids
		 *     truncation artifacts when values are very close to integers.
		 * 
		 * \tparam _tType
		 *      The result dtype (NumPy's dtype analogue). Defaults to double.
		 * \tparam _tStart
		 *      Type of the starting exponent.
		 * \tparam _tStop
		 *      Type of the final exponent.
		 * \param _tStartExp
		 *      The starting exponent.
		 * \param _tStopExp
		 *      The final exponent.
		 * \param _stNum
		 *      The number of samples to generate (default 50).
		 * \param _bEndpoint
		 *      If true, the last sample is approximately base**_tStopExp; otherwise it is excluded.
		 * \param _dBase
		 *      The base of the log scale (default 10.0).
		 * \return
		 *      Returns a vector of log-spaced samples.
		 * \throw std::overflow_error
		 *      Thrown for integer _tType if a rounded value is out of range for _tType.
		 **/
		template <typename _tType = double, typename _tStart, typename _tStop>
		static inline std::vector<_tType>
										Logspace( _tStart _tStartExp,
			_tStop _tStopExp,
			size_t _stNum = 50,
			bool _bEndpoint = true,
			double _dBase = 10.0 ) {
			std::vector<_tType> vOut;

			if ( _stNum == 0 ) { return vOut; }

			vOut.resize( _stNum );

			if ( _stNum == 1 ) {
				// NumPy returns base**start when num == 1
				const double dStartExp = static_cast<double>(_tStartExp);
				const double dVal      = std::pow( _dBase, dStartExp );

				if constexpr ( std::is_integral_v<_tType> ) {
					long long ll = std::llround( static_cast<long double>(dVal) );

					if ( ll < static_cast<long long>(std::numeric_limits<_tType>::min()) ||
						 ll > static_cast<long long>(std::numeric_limits<_tType>::max()) ) { throw std::overflow_error( "" ); }

					vOut[0] = static_cast<_tType>(ll);
				}
				else { vOut[0] = static_cast<_tType>(dVal); }

				return vOut;
			}

			// Internal exponents in double (NumPy uses float64 logic here).
			const double dStartExp = static_cast<double>(_tStartExp);
			const double dStopExp  = static_cast<double>(_tStopExp);

			const double dCount   = static_cast<double>(_stNum);
			const double dDivisor = _bEndpoint
				? (dCount - 1.0)
				: dCount;

			const double dStepExp = (dStopExp - dStartExp) / dDivisor;

			for ( size_t I = 0; I < _stNum; ++I ) {
				const double dExp = dStartExp + dStepExp * static_cast<double>(I);
				const double dVal = std::pow( _dBase, dExp );

				if constexpr ( std::is_integral_v<_tType> ) {
					long long ll = std::llround( static_cast<long double>(dVal) );

					if ( ll < static_cast<long long>(std::numeric_limits<_tType>::min()) ||
						 ll > static_cast<long long>(std::numeric_limits<_tType>::max()) ) { throw std::overflow_error( "" ); }

					vOut[I] = static_cast<_tType>(ll);
				}
				else { vOut[I] = static_cast<_tType>(dVal); }
			}

			// Force exact endpoint when requested.
			if ( _bEndpoint ) {
				const double dEndVal = std::pow( _dBase, dStopExp );

				if constexpr ( std::is_integral_v<_tType> ) {
					long long ll = std::llround( static_cast<long double>(dEndVal) );

					if ( ll < static_cast<long long>(std::numeric_limits<_tType>::min()) ||
						 ll > static_cast<long long>(std::numeric_limits<_tType>::max()) ) { throw std::overflow_error( "" ); }

					vOut[_stNum-1] = static_cast<_tType>(ll);
				}
				else { vOut[_stNum-1] = static_cast<_tType>(dEndVal); }
			}

			return vOut;
		}

		/**
		 * \brief Creates a 1D vector of numbers spaced evenly on a multiplicative (geometric) scale. Must be called within a try/catch block.
		 * 
		 * This emulates numpy.geomspace( start, stop, num=50, endpoint=True, dtype=_tType, axis=0 )
		 * for real dtypes in 1D.
		 * 
		 * Floating-point _tType:
		 *     Behaves like NumPy's geomspace for real inputs.
		 * Integer _tType:
		 *     Behaves like np.around(np.geomspace(...)).astype(int), which avoids
		 *     truncation artifacts when values are very close to integers.
		 * 
		 * If start and stop have opposite signs, the real-valued version cannot reproduce
		 * NumPy's complex-valued spiral and will throw.
		 * 
		 * \tparam _tType
		 *      The result type (NumPy's dtype analogue). Defaults to double.
		 * \param _tStart
		 *      The starting value.
		 * \param _tStop
		 *      The final value.
		 * \param _stNum
		 *      The number of samples to generate (default 50).
		 * \param _bEndpoint
		 *      If true, the last sample is (approximately) _tStop; otherwise it is excluded.
		 * \return
		 *      Returns a vector of geometrically spaced samples.
		 * \throw std::invalid_argument
		 *      Thrown if start and stop have opposite signs.
		 * \throw std::overflow_error
		 *      Thrown for integer _tType if a rounded value is out of range for _tType.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										Geomspace( _tType _tStart,
			_tType _tStop,
			size_t _stNum = size_t( 50 ),
			bool _bEndpoint = true ) {
			std::vector<_tType> vOut;

			if ( _stNum == 0 ) { return vOut; }

			vOut.resize( _stNum );

			if ( _stNum == 1 ) {
				vOut[0] = static_cast<_tType>( _tStart );
				return vOut;
			}

			// Internal work in double to match NumPy's float64 behaviour.
			const double dStart = static_cast<double>(_tStart);
			const double dStop  = static_cast<double>(_tStop);

			const bool bNegStart = (dStart < 0.0);
			const bool bNegStop  = (dStop < 0.0);

			int iSign = 1;
			double dMagStart = dStart;
			double dMagStop  = dStop;

			if ( bNegStart && bNegStop ) {
				dMagStart = -dStart;
				dMagStop  = -dStop;
				iSign = -1;
			}
			else if ( bNegStart != bNegStop ) { throw std::invalid_argument( "" ); }

			// Ratio of magnitudes.
			const double dRatio = dMagStop / dMagStart;

			// Exponent goes from 0 to 1 (endpoint) or 0 to 1 - 1/num (no endpoint).
			const double dExpStart = 0.0;
			const double dExpStop  = _bEndpoint ?
				1.0 :
				1.0 - 1.0 / static_cast<double>(_stNum);
			const double dStepExp  = (dExpStop - dExpStart) /
									 static_cast<double>(_stNum - 1);

			for ( size_t I = 0; I < _stNum; ++I ) {
				const double dExp = dExpStart + dStepExp * static_cast<double>(I);
				const double dValMag = dMagStart * std::pow( dRatio, dExp );
				const double dSignedVal = static_cast<double>(iSign) * dValMag;

				if constexpr ( std::is_integral_v<_tType> ) {
					long double ld = static_cast<long double>(dSignedVal);
					long long ll = std::llround( ld );

					if ( ll < static_cast<long long>(std::numeric_limits<_tType>::min()) ||
						 ll > static_cast<long long>(std::numeric_limits<_tType>::max()) ) { throw std::overflow_error( "" ); }

					vOut[I] = static_cast<_tType>(ll);
				}
				else { vOut[I] = static_cast<_tType>(dSignedVal); }
			}

			// Force exact endpoint equality when requested.
			if ( _bEndpoint ) { vOut[_stNum-1] = static_cast<_tType>(_tStop); }

			return vOut;
		}

		/**
		 * \brief Converts a single value to another type using NumPy-style astype semantics.
		 * 
		 * This emulates numpy.asarray(x).astype(_tOut)[...] for a single value:
		 * 
		 *   - Floating-point to integer: Truncates toward zero (via static_cast).
		 *   - Integer to floating-point: Converts exactly where representable.
		 *   - Any numeric type to bool: Zero becomes false, non-zero becomes true.
		 * 
		 * No range checking is performed; overflow and underflow follow normal C++ conversion
		 * rules for static_cast.
		 * 
		 * \tparam _tOut
		 *      The output type (NumPy's dtype analogue).
		 * \tparam _tIn
		 *      The input type.
		 * \param _tValue
		 *      The value to convert.
		 * \return
		 *      Returns the converted value.
		 **/
		template <typename _tOut, typename _tIn>
		static inline _tOut				Astype( _tIn _tValue ) {
			return static_cast<_tOut>(_tValue);
		}

		/**
		 * \brief Converts a vector to another type using NumPy-style astype semantics. Must be called within a try/catch block.
		 * 
		 * This emulates numpy.asarray(v).astype(_tOut) for 1D arrays:
		 * 
		 *   - A new vector is always created (like copy=True in NumPy).
		 *   - Floating-point to integer: Truncates toward zero (via static_cast).
		 *   - Integer to floating-point: Converts exactly where representable.
		 *   - Any numeric type to bool: Zero becomes false, non-zero becomes true.
		 * 
		 * No range checking is performed; overflow and underflow follow normal C++ conversion
		 * rules for static_cast (similar in spirit to NumPy's casting='unsafe').
		 * 
		 * \tparam _tOut
		 *      The output element type (NumPy's dtype analogue).
		 * \tparam _tIn
		 *      The input element type.
		 * \param _vIn
		 *      The input vector.
		 * \return
		 *      Returns a new vector whose elements are converted to _tOut.
		 **/
		template <typename _tOut, typename _tIn>
		static inline std::vector<_tOut>
										Astype( const std::vector<_tIn> &_vIn ) {
			std::vector<_tOut> vOut;
			vOut.resize( _vIn.size() );

			for ( size_t I = 0; I < _vIn.size(); ++I ) {
				vOut[I] = static_cast<_tOut>(_vIn[I]);
			}

			return vOut;
		}

		/**
		 * \brief Computes the n-th discrete difference along a 1D array. Must be called within a try/catch block.
		 * 
		 * This emulates numpy.diff( a, n=_stN ) for 1D input:
		 * 
		 *   - If _stN == 0, a copy of _vX is returned.
		 *   - Each application of the difference reduces the length by 1.
		 *   - If _stN > 0 and _vX.size() <= _stN, an empty vector is returned.
		 * 
		 * \tparam _tType
		 *      The value type stored in the vector (defaults to double).
		 * \param _vX
		 *      The input samples.
		 * \param _stN
		 *      The order of the difference (default 1).
		 * \return
		 *      Returns the n-th order difference of _vX.
		 **/
		template <typename _tType = double>
		static inline std::vector<_tType>
										Diff1D( const std::vector<_tType> &_vX, size_t _stN = 1 ) {
			if ( _stN == 0 ) { return _vX; }

			if ( _vX.size() <= _stN ) { return std::vector<_tType>(); }

			std::vector<_tType> vCur = _vX;

			for ( size_t N = 0; N < _stN; ++N ) {
				if ( vCur.size() < 2 ) { return std::vector<_tType>(); }

				std::vector<_tType> vNext;
				vNext.resize( vCur.size() - 1 );

				for ( size_t I = 0; I + 1 < vCur.size(); ++I ) {
					vNext[I] = vCur[I+1] - vCur[I];
				}

				vCur.swap( vNext );
			}

			return vCur;
		}

		/**
		 * \brief   Computes the normalized magnitude response |H(f)| of an M-tap rectangular-windowed sinc filter at frequency _dFreqHz.
		 * 
		 * \param   _dSampleHz  Sampling rate Fs (Hz).
		 * \param   _dFreqHz    Frequency at which to evaluate (Hz).
		 * \param   _sM         Number of taps (must be odd).
		 * \return  |H(f)| normalized so H(0)=1.
		 */
		static double					SincFilterResponse( double _dSampleHz, double _dFreqHz, size_t _sM ) {
			double dOmega = 2.0 * EE_PI * (_dFreqHz / _dSampleHz);
			double dNum   = std::sin( _sM * dOmega * 0.5 );
			double dDen   = _sM * std::sin( dOmega * 0.5 );
			if ( std::fabs( dDen ) < 1e-16 ) { return 1.0; }
			return std::fabs( dNum / dDen );
		}

		/**
		 * \brief   Estimates the roll-off in dB/octave for an M-tap sinc around cutoff _dFc by comparing H(_dFc) to H(2·_dFc).
		 * 
		 * \param   _sM                 Tap-count (odd).
		 * \param   _dSampleHz          Sampling rate Fs (Hz).
		 * \param   _dFc                Cut-off frequency (Hz).
		 * \return  20·log10( H(_dFc) / H(2·_dFc) ), i.e. dB drop per octave.
		 */
		static double					CalcSlopeDbPerOctave( size_t _sM, double _dSampleHz, double _dFc ) {
			double dHfc  = SincFilterResponse( _dSampleHz, _dFc,  _sM );
			double dH2fc = SincFilterResponse( _dSampleHz, 2.0 * _dFc, _sM );
			if ( dHfc <= 0.0 || dH2fc <= 0.0 ) { return 0.0; }
			return 20.0 * std::log10( dHfc / dH2fc );
		}

		/**
		 * \brief   Finds an odd tap-count M for a windowed-sinc low-pass whose roll-off near cutoff is closest to the desired dB/octave.
		 * 
		 * \param   _dSampleHz              Sampling rate Fs (Hz).
		 * \param   _dFc                    Cut-off frequency (Hz).
		 * \param   _dDesiredSlopeDbPerOct  Target slope in dB per octave.
		 * \return  Best-matching odd tap-count (3…1023).
		 */
		static inline size_t			GetSincFilterM( double _dSampleHz, double _dFc, double _dDesiredSlopeDbPerOct ) {
			constexpr size_t csMaxTaps = 1 * 1024 * 1024 + 1;		// Must be odd.
			size_t sBestM   = 3;
			double dBestErr = 1e9;

			for ( size_t sM = 3; sM <= csMaxTaps; sM += 2 ) {
				double dSlope = CalcSlopeDbPerOctave( sM, _dSampleHz, _dFc );
				double dErr   = std::fabs( dSlope - _dDesiredSlopeDbPerOct );
				if ( dErr < dBestErr ) {
					dBestErr = dErr;
					sBestM   = sM;
					if ( dErr < 0.0001 ) { break; } // Close enough.
				}
			}

			return sBestM;
		}

		/**
		 * \brief Calculates the exponent M for a SINC^M filter to achieve a desired fall‐off slope in dB per octave.
		 * 
		 * \param _dSlopeDbPerOctave Desired slope in decibels per octave.
		 * \return The exponent M to use in the SINC^M filter.
		 */
		static inline double			GetSincFilterM( double _dSlopeDbPerOctave ) {
			//                             6.0205999132796239042747778944899
			constexpr double cdSlopePerM = 6.02059991327962418239394537522457540035247802734375;	// = 6.0205999132796239042747778944899 dB/octave per unit M (rounded to nearest actual double value).
			double dM = _dSlopeDbPerOctave / cdSlopePerM;
			return dM;
		}

		/**
		 * Calculates the ideal filter width (M) for a windowed-sinc filter,
		 * given a desired transition width in Hz.
		 *
		 * \param  _dHz						The sample rate in Hz.
		 * \param  _dTransitionHz			The desired width of the transition band in Hz.
		 * \param  _TransitionMultiplier	The transition multiplier.
		 * \return							Returns the calculated (odd) filter length M.
		 */
		static inline size_t			CalcIdealSincM( double _dHz, double _dTransitionHz, size_t _TransitionMultiplier = 6.0 ) {
			/**
			 * Window			Main-Lobe Width (Null-to-Null)	Transition Multiplier for -6 dB	Multiplier for -40 dB	Multiplier for -60 dB	Typical Attenuation at Sidelobe
			 * Rectangular		4.0 × dHz/dTrans				4.0								—						—						~-13 dB
			 * Hanning			8.0 × dHz/dTrans				4.0								8.0						—						~-31 dB
			 * Hamming			8.0 × dHz/dTrans				4.0								8.0						—						~-41 dB
			 * Blackman			12.0 × dHz/dTrans				6.0								12.0					—						~-58 dB
			 * Kaiser (β=8.6)	~12.0 × dHz/dTrans				6.0								12.0					24.0					up to -90 dB (for large β)
			 **/
			if ( _dTransitionHz <= 0.0 || _dHz <= 0.0 ) { return 1; }
			size_t sM = size_t( std::ceil( _TransitionMultiplier * _dHz / _dTransitionHz ) );
			if ( !( sM & 1 ) ) { ++sM; }  // Make odd for symmetry.
			return sM;
		}

		/**
		 * Creates a windowed sinc low-pass filter.  Call within a try/catch block.
		 * 
		 * \tparam _tType			The type of the filter coefficients.
		 * \param _dHz				The input Hz.
		 * \param _dFc				The cut-off frequency.
		 * \param _sM				The size of the coefficient table to create.  On output, it contains the midpoint value for the created coefficient buffer.
		 * \param _pfWindowFunc		The window function.
		 * \return					Returns the created sinc-filter coefficients.
		 * \throws					std::runtime_error on allocation or window error.
		 **/
		template <typename _tType = std::vector<double>>
		static inline _tType			SincFilterLpf( double _dHz, double _dFc, size_t &_sM, PfWindowFunc<typename _tType::value_type> _pfWindowFunc = &BlackmanWindow<_tType::value_type> ) {
			_tType vFilter;
			if ( _dFc > _dHz / 2.0 || _dHz <= 0.0 ) {
				vFilter.push_back( _tType::value_type( 1.0 ) );
				_sM = 0;
				return vFilter;
			}
			if ( !(_sM & 1) ) { ++_sM; }
			_dFc /= _dHz;
			
			vFilter.resize( _sM );				// Can throw.
			if ( !_pfWindowFunc( _sM, vFilter ) ) { throw std::runtime_error( "SincFilter(): Failed to apply windowing function." ); }

			size_t sL = _sM / 2;
			double dFc2 = 2.0 * _dFc;
			int64_t i64SignedL = int64_t( sL );
			for ( auto I = _sM; I--; ) {
				int64_t N = int64_t( I ) - i64SignedL;
				vFilter[I] = _tType::value_type( vFilter[I] * dFc2 * Sinc( dFc2 * N ) );
			}

			// Normalize.
			double dSum = 0.0;
			for ( auto & I : vFilter ) { dSum += double( I ); }
			for ( auto & I : vFilter ) { I = _tType::value_type( I / dSum ); }
			_sM = sL;
			return vFilter;
		}

		/**
		 * Creates a windowed sinc high-pass filter.  Call within a try/catch block.
		 *
		 * \tparam _tType			The type of the filter coefficients.
		 * \param  _dHz				The sample rate in Hz.
		 * \param  _dFc				The cutoff frequency in Hz.
		 * \param  _sM				On input, the filter width (may be adjusted to odd); on output, set to half-width.
		 * \param  _pfWindowFunc	The window function pointer (defaults to Hamming).
		 * \return					Returns the created filter coefficients.
		 * \throws					std::runtime_error on allocation or window error.
		 */
		template <typename _tType = std::vector<double>>
		static inline _tType			SincFilterHpf( double _dHz, double _dFc, size_t &_sM, PfWindowFunc<typename _tType::value_type> _pfWindowFunc = &BlackmanWindow<_tType::value_type> ) {
			_tType vFilter;
			if ( _dFc > _dHz / 2.0 || _dHz <= 0.0 ) {
				vFilter.push_back( _tType::value_type( 1.0 ) );
				_sM = 0;
				return vFilter;
			}
			if ( !(_sM & 1) ) { ++_sM; }
			_dFc /= _dHz;

			vFilter.resize( _sM );
			if ( !_pfWindowFunc( _sM, vFilter ) ) { throw std::runtime_error( "SincFilterHpf(): Failed to apply windowing function." ); }

			size_t sL = _sM / 2;
			double dFc2 = 2.0 * _dFc;
			int64_t i64SignedL = int64_t( sL );
			for ( auto I = _sM; I--; ) {
				int64_t N = int64_t( I ) - i64SignedL;
				if ( N == 0 ) {
					// Center tap for HPF
					vFilter[I] = _tType::value_type( vFilter[I] * (1.0 - dFc2) );
				}
				else {
					vFilter[I] = _tType::value_type( vFilter[I] * (-dFc2 * Sinc( dFc2 * N )) );
				}
			}

			// Normalize.
			double dSum = 0.0;
			for ( auto & I : vFilter ) { dSum += double( I ); }
			for ( auto & I : vFilter ) { I = _tType::value_type( I / dSum ); }
			_sM = sL;
			return vFilter;
		}

		/**
		 * \brief   Creates a windowed sinc band-pass filter.  Call within a try/catch block.
		 *
		 * \tparam  _tType			The type of the filter coefficients.
		 * \param   _dHz			The sample rate in Hz.
		 * \param   _dF1			The low cutoff frequency in Hz.
		 * \param   _dF2			The high cutoff frequency in Hz.
		 * \param   _sM				On input, the filter width (may be adjusted to odd); on output, set to half-width (midpoint index).
		 * \param   _pfWindowFunc	The window function pointer (defaults to Hamming).
		 * \return					Returns the created band-pass sinc-filter coefficients.
		 * \throws					std::runtime_error on allocation or window error.
		 **/
		template <typename _tType = std::vector<double>>
		static inline _tType			SincFilterBpf( double _dHz, double _dF1, double _dF2, size_t &_sM, PfWindowFunc<typename _tType::value_type> _pfWindowFunc = &BlackmanWindow<_tType::value_type> ) {
			_tType vFilter;
			if ( _dHz <= 0.0 ||
				 _dF1 >= _dF2 ||
				 _dF2 > _dHz * 0.5 ||
				 _dF1 < 0.0 ) {
				vFilter.push_back( _tType::value_type( 1.0 ) );
				_sM = 0;
				return vFilter;
			}

			if ( !(_sM & 1) ) { ++_sM; }

			double dF1n = _dF1 / _dHz;
			double dF2n = _dF2 / _dHz;

			vFilter.resize( _sM );
			if ( !_pfWindowFunc( _sM, vFilter ) ) { throw std::runtime_error( "SincFilterBpf(): Failed to apply windowing function." ); }

			size_t   sL    = _sM / 2;
			double   dF12  = 2.0 * dF1n;
			double   dF22  = 2.0 * dF2n;
			int64_t  i64L  = int64_t( sL );

			// Build band-pass by subtracting two low-pass responses.
			for ( auto I = _sM; I--; ) {
				int64_t N = int64_t( I ) - i64L;
				if ( N == 0 ) {
					// Center tap: difference of DC gains.
					vFilter[I] = _tType::value_type( vFilter[I] * (dF22 - dF12) );
				}
				else {
					// Off-center taps: difference of sinc kernels.
					double dLo = dF12 * Sinc( dF12 * N );
					double dHi = dF22 * Sinc( dF22 * N );
					vFilter[I] = _tType::value_type( vFilter[I] * (dHi - dLo) );
				}
			}

			// Normalize.
			double dSum = 0.0;
			for ( auto & I : vFilter ) { dSum += double( I ); }
			for ( auto & I : vFilter ) { I = _tType::value_type( I / dSum ); }

			_sM = sL;
			return vFilter;
		}

		/**
		 * \brief  Calculates a “gated” AC-RMS: subtracts DC and only counts samples above a threshold.
		 *
		 * \param  _vSamples    The input samples.
		 * \param  _dThreshold  The absolute amplitude gate; samples with |x–mean| < _dThreshold are ignored.
		 * \return              The computed gated RMS; zero if no samples exceed threshold.
		 * \throws std::runtime_error if _vSamples is empty.
		 */
		template <typename _tType = std::vector<double>>
		static inline double			CalcRmsGated( const _tType &_vSamples, double _dThreshold ) {
			if ( _vSamples.empty() ) { return 0.0; }

			// Remove DC.
			double dMean = 0.0;
			for ( auto I = _vSamples.size(); I--; ) {
				dMean += double( _vSamples[I] );
			}
			/*for ( double dSample : _vSamples ) {
				dMean += dSample;
			}*/
			dMean /= static_cast<double>(_vSamples.size());

			// Accumulate only above threshold.
			double dSumSq    = 0.0;
			size_t sCount    = 0;
			for ( auto I = _vSamples.size(); I--; ) {
				double dCentered = double( _vSamples[I] ) - dMean;
				if ( std::abs( dCentered ) >= _dThreshold ) {
					dSumSq += dCentered * dCentered;
					++sCount;
				}
			}
			if ( sCount == 0 ) { return 0.0; }

			double dMeanSq = dSumSq / static_cast<double>(sCount);
			return std::sqrt( dMeanSq );
		}

	};

}	// namespace ee


// ===============================
// Count Leading Zeros
// ===============================
#if defined( _MSC_VER )
		#pragma intrinsic( _BitScanReverse )
	#ifdef _WIN64
		#pragma intrinsic( _BitScanReverse64 )
	#endif  // #ifdef _WIN64
#endif  // #if defined( _MSC_VER )

inline unsigned int						CountLeadingZeros( uint16_t _ui16X ) {
#if defined( _MSC_VER )
	unsigned long ulIndex;
	auto ucIsNonZero = ::_BitScanReverse( &ulIndex, _ui16X );
	return ucIsNonZero ? (15 - static_cast<int>(ulIndex - 16)) : 16;
#else
	return _ui32X != 0 ? (__builtin_clz( static_cast<uint32_t>(_ui16X) ) - 16) : 16;
#endif  // #if defined( _MSC_VER )
}

inline unsigned int						CountLeadingZeros( uint32_t _ui32X ) {
#if defined( _MSC_VER )
	unsigned long ulIndex;
	auto ucIsNonZero = ::_BitScanReverse( &ulIndex, _ui32X );
	return ucIsNonZero ? (31 - static_cast<int>(ulIndex)) : 32;
#else
	return _ui32X != 0 ? __builtin_clz( _ui32X ) : 32;
#endif  // #if defined( _MSC_VER )
}

inline unsigned int						CountLeadingZeros( uint64_t _ui64X ) {
#if defined( _MSC_VER )
	#if defined( _WIN64 )
		// Benchmark against (1000000*50) values.
		// _BitScanReverse64(): 0.06879443333333333 seconds
		// Manual version: 0.188431 seconds.
		#if 1
			unsigned long ulIndex;
			auto ucIsNonZero = ::_BitScanReverse64( &ulIndex, _ui64X ) != 0;
			//return ((63 - static_cast<int>(ulIndex)) * ucIsNonZero) + (64 * !ucIsNonZero);
			return ucIsNonZero ? (63 - static_cast<int>(ulIndex)) : 64;
		#else
			unsigned long uiN = 0;
			if ( _ui64X == 0 ) { return 64; }
			if ( (_ui64X & 0xFFFFFFFF00000000ULL) == 0 ) { uiN += 32; _ui64X <<= 32; }
			if ( (_ui64X & 0xFFFF000000000000ULL) == 0 ) { uiN += 16; _ui64X <<= 16; }
			if ( (_ui64X & 0xFF00000000000000ULL) == 0 ) { uiN += 8;  _ui64X <<= 8; }
			if ( (_ui64X & 0xF000000000000000ULL) == 0 ) { uiN += 4;  _ui64X <<= 4; }
			if ( (_ui64X & 0xC000000000000000ULL) == 0 ) { uiN += 2;  _ui64X <<= 2; }
			if ( (_ui64X & 0x8000000000000000ULL) == 0 ) { uiN += 1; }

			return uiN;
		#endif  // #if 0
	#else
		if ( _ui64X == 0 ) { return 64; }
		unsigned int uiN = 0;

		#if 0
			// Benchmark against (1000000*50) values (x86 on x64 hardware). 
			// 0.0002651
			// 0.0002426333333333333
			if ( (_ui64X & 0xFFFFFFFF00000000ULL) == 0 ) { uiN += 32; _ui64X <<= 32; }
			if ( (_ui64X & 0xFFFF000000000000ULL) == 0 ) { uiN += 16; _ui64X <<= 16; }
			if ( (_ui64X & 0xFF00000000000000ULL) == 0 ) { uiN += 8;  _ui64X <<= 8; }
			if ( (_ui64X & 0xF000000000000000ULL) == 0 ) { uiN += 4;  _ui64X <<= 4; }
			if ( (_ui64X & 0xC000000000000000ULL) == 0 ) { uiN += 2;  _ui64X <<= 2; }
			if ( (_ui64X & 0x8000000000000000ULL) == 0 ) { uiN += 1; }
			return uiN;
		#else
			// Benchmark against (1000000*50) values (x86 on x64 hardware). 
			// 0.0002392 seconds.
			if ( _ui64X & 0xFFFFFFFF00000000ULL )        { uiN += 32; _ui64X >>= 32; }
			if ( _ui64X & 0xFFFF0000 )                   { uiN += 16; _ui64X >>= 16; }
			if ( _ui64X & 0xFFFFFF00 )                   { uiN += 8;  _ui64X >>= 8; }
			if ( _ui64X & 0xFFFFFFF0 )                   { uiN += 4;  _ui64X >>= 4; }
			if ( _ui64X & 0xFFFFFFFC )                   { uiN += 2;  _ui64X >>= 2; }
			if ( _ui64X & 0xFFFFFFFE )                   { uiN += 1; }
			return 63 - uiN;
		#endif  // #if 0
	#endif  // #if defined( _WIN64 )
#elif defined( __GNUC__ )
	return _ui64X != 0 ? __builtin_clzll( _ui64X ) : 64;
#endif  // #if defined( __GNUC__ )
}


// ===============================
// 128-Bit Div/Mul
// ===============================
#if defined( _MSC_VER )
	#ifdef _WIN64
		#include <immintrin.h>
		#pragma intrinsic( _udiv128 )
		#pragma intrinsic( _div128 )
		#pragma intrinsic( _umul128 )
		#pragma intrinsic( _mul128 )

		#pragma intrinsic( _addcarry_u64 )
		#pragma intrinsic( _subborrow_u64 )

		/**
		 * \brief Adds two unsigned 128-bit values (hi:lo + hi:lo) and returns the low 64 bits.
		 *
		 * \param _ui64AHigh High 64 bits of the first addend.
		 * \param _ui64ALow Low 64 bits of the first addend.
		 * \param _ui64BHigh High 64 bits of the second addend.
		 * \param _ui64BLow Low 64 bits of the second addend.
		 * \param _pui64HighSum Receives the high 64 bits of the sum.
		 * \return Returns the low 64 bits of the sum.
		 **/
		inline uint64_t												_uadd128( uint64_t _ui64AHigh, uint64_t _ui64ALow, uint64_t _ui64BHigh, uint64_t _ui64BLow, uint64_t * _pui64HighSum ) {
			uint64_t ui64Lo = 0;
			uint64_t ui64Hi = 0;

			const unsigned char ucCarry = _addcarry_u64( 0, _ui64ALow, _ui64BLow, &ui64Lo );
			_addcarry_u64( ucCarry, _ui64AHigh, _ui64BHigh, &ui64Hi );

			if ( _pui64HighSum ) { (*_pui64HighSum) = ui64Hi; }
			return ui64Lo;
		}

		/**
		 * \brief Adds two signed 128-bit values (hi:lo + hi:lo) and returns the low 64 bits.
		 *
		 * \param _i64AHigh High 64 bits of the first addend.
		 * \param _i64ALow Low 64 bits of the first addend.
		 * \param _i64BHigh High 64 bits of the second addend.
		 * \param _i64BLow Low 64 bits of the second addend.
		 * \param _pi64HighSum Receives the high 64 bits of the sum.
		 * \return Returns the low 64 bits of the sum.
		 **/
		inline int64_t												_add128( int64_t _i64AHigh, int64_t _i64ALow, int64_t _i64BHigh, int64_t _i64BLow, int64_t * _pi64HighSum ) {
			uint64_t ui64Hi = 0;
			const uint64_t ui64Lo = _uadd128(
				static_cast<uint64_t>(_i64AHigh),
				static_cast<uint64_t>(_i64ALow),
				static_cast<uint64_t>(_i64BHigh),
				static_cast<uint64_t>(_i64BLow),
				&ui64Hi
			);

			if ( _pi64HighSum ) { (*_pi64HighSum) = static_cast<int64_t>(ui64Hi); }
			return static_cast<int64_t>(ui64Lo);
		}

		/**
		 * \brief Subtracts two unsigned 128-bit values (hi:lo - hi:lo) and returns the low 64 bits.
		 *
		 * \param _ui64AHigh High 64 bits of the minuend.
		 * \param _ui64ALow Low 64 bits of the minuend.
		 * \param _ui64BHigh High 64 bits of the subtrahend.
		 * \param _ui64BLow Low 64 bits of the subtrahend.
		 * \param _pui64HighDiff Receives the high 64 bits of the difference.
		 * \return Returns the low 64 bits of the difference.
		 **/
		inline uint64_t												_usub128( uint64_t _ui64AHigh, uint64_t _ui64ALow, uint64_t _ui64BHigh, uint64_t _ui64BLow, uint64_t * _pui64HighDiff ) {
			uint64_t ui64Lo = 0;
			uint64_t ui64Hi = 0;

			const unsigned char ucBorrow = _subborrow_u64( 0, _ui64ALow, _ui64BLow, &ui64Lo );
			_subborrow_u64( ucBorrow, _ui64AHigh, _ui64BHigh, &ui64Hi );

			if ( _pui64HighDiff ) { (*_pui64HighDiff) = ui64Hi; }
			return ui64Lo;
		}

		/**
		 * \brief Subtracts two signed 128-bit values (hi:lo - hi:lo) and returns the low 64 bits.
		 *
		 * \param _i64AHigh High 64 bits of the minuend.
		 * \param _i64ALow Low 64 bits of the minuend.
		 * \param _i64BHigh High 64 bits of the subtrahend.
		 * \param _i64BLow Low 64 bits of the subtrahend.
		 * \param _pi64HighDiff Receives the high 64 bits of the difference.
		 * \return Returns the low 64 bits of the difference.
		 **/
		inline int64_t												_sub128( int64_t _i64AHigh, int64_t _i64ALow, int64_t _i64BHigh, int64_t _i64BLow, int64_t * _pi64HighDiff ) {
			uint64_t ui64Hi = 0;
			const uint64_t ui64Lo = _usub128(
				static_cast<uint64_t>(_i64AHigh),
				static_cast<uint64_t>(_i64ALow),
				static_cast<uint64_t>(_i64BHigh),
				static_cast<uint64_t>(_i64BLow),
				&ui64Hi
			);

			if ( _pi64HighDiff ) { (*_pi64HighDiff) = static_cast<int64_t>(ui64Hi); }
			return static_cast<int64_t>(ui64Lo);
		}
	#else
		#include <bit>
		#include <cassert>
		#include <intrin.h>

		#pragma intrinsic( _addcarry_u32 )
		#pragma intrinsic( _subborrow_u32 )

		/**
		 * \brief Adds two unsigned 128-bit values (hi:lo + hi:lo) and returns the low 64 bits.
		 *
		 * \param _ui64AHigh High 64 bits of the first addend.
		 * \param _ui64ALow Low 64 bits of the first addend.
		 * \param _ui64BHigh High 64 bits of the second addend.
		 * \param _ui64BLow Low 64 bits of the second addend.
		 * \param _pui64HighSum Receives the high 64 bits of the sum.
		 * \return Returns the low 64 bits of the sum.
		 **/
		inline uint64_t					_uadd128( uint64_t _ui64AHigh, uint64_t _ui64ALow, uint64_t _ui64BHigh, uint64_t _ui64BLow, uint64_t * _pui64HighSum ) {
			uint32_t ui32ALo0 = static_cast<uint32_t>(_ui64ALow);
			uint32_t ui32ALo1 = static_cast<uint32_t>(_ui64ALow >> 32);
			uint32_t ui32AHi0 = static_cast<uint32_t>(_ui64AHigh);
			uint32_t ui32AHi1 = static_cast<uint32_t>(_ui64AHigh >> 32);

			uint32_t ui32BLo0 = static_cast<uint32_t>(_ui64BLow);
			uint32_t ui32BLo1 = static_cast<uint32_t>(_ui64BLow >> 32);
			uint32_t ui32BHi0 = static_cast<uint32_t>(_ui64BHigh);
			uint32_t ui32BHi1 = static_cast<uint32_t>(_ui64BHigh >> 32);

			uint32_t ui32Lo0 = 0, ui32Lo1 = 0, ui32Hi0 = 0, ui32Hi1 = 0;

			unsigned char ucC = 0;
			ucC = _addcarry_u32( ucC, ui32ALo0, ui32BLo0, &ui32Lo0 );
			ucC = _addcarry_u32( ucC, ui32ALo1, ui32BLo1, &ui32Lo1 );
			ucC = _addcarry_u32( ucC, ui32AHi0, ui32BHi0, &ui32Hi0 );
			_addcarry_u32( ucC, ui32AHi1, ui32BHi1, &ui32Hi1 );

			const uint64_t ui64Lo = (static_cast<uint64_t>(ui32Lo1) << 32) | ui32Lo0;
			const uint64_t ui64Hi = (static_cast<uint64_t>(ui32Hi1) << 32) | ui32Hi0;

			if ( _pui64HighSum ) { (*_pui64HighSum) = ui64Hi; }
			return ui64Lo;
		}

		/**
		 * \brief Adds two signed 128-bit values (hi:lo + hi:lo) and returns the low 64 bits.
		 *
		 * \param _i64AHigh High 64 bits of the first addend.
		 * \param _i64ALow Low 64 bits of the first addend.
		 * \param _i64BHigh High 64 bits of the second addend.
		 * \param _i64BLow Low 64 bits of the second addend.
		 * \param _pi64HighSum Receives the high 64 bits of the sum.
		 * \return Returns the low 64 bits of the sum.
		 **/
		inline int64_t					_add128( int64_t _i64AHigh, int64_t _i64ALow, int64_t _i64BHigh, int64_t _i64BLow, int64_t * _pi64HighSum ) {
			uint64_t ui64Hi = 0;
			const uint64_t ui64Lo = _uadd128(
				static_cast<uint64_t>(_i64AHigh),
				static_cast<uint64_t>(_i64ALow),
				static_cast<uint64_t>(_i64BHigh),
				static_cast<uint64_t>(_i64BLow),
				&ui64Hi
			);

			if ( _pi64HighSum ) { (*_pi64HighSum) = static_cast<int64_t>(ui64Hi); }
			return static_cast<int64_t>(ui64Lo);
		}

		/**
		 * \brief Subtracts two unsigned 128-bit values (hi:lo - hi:lo) and returns the low 64 bits.
		 *
		 * \param _ui64AHigh High 64 bits of the minuend.
		 * \param _ui64ALow Low 64 bits of the minuend.
		 * \param _ui64BHigh High 64 bits of the subtrahend.
		 * \param _ui64BLow Low 64 bits of the subtrahend.
		 * \param _pui64HighDiff Receives the high 64 bits of the difference.
		 * \return Returns the low 64 bits of the difference.
		 **/
		inline uint64_t					_usub128( uint64_t _ui64AHigh, uint64_t _ui64ALow, uint64_t _ui64BHigh, uint64_t _ui64BLow, uint64_t * _pui64HighDiff ) {
			uint32_t ui32ALo0 = static_cast<uint32_t>(_ui64ALow);
			uint32_t ui32ALo1 = static_cast<uint32_t>(_ui64ALow >> 32);
			uint32_t ui32AHi0 = static_cast<uint32_t>(_ui64AHigh);
			uint32_t ui32AHi1 = static_cast<uint32_t>(_ui64AHigh >> 32);

			uint32_t ui32BLo0 = static_cast<uint32_t>(_ui64BLow);
			uint32_t ui32BLo1 = static_cast<uint32_t>(_ui64BLow >> 32);
			uint32_t ui32BHi0 = static_cast<uint32_t>(_ui64BHigh);
			uint32_t ui32BHi1 = static_cast<uint32_t>(_ui64BHigh >> 32);

			uint32_t ui32Lo0 = 0, ui32Lo1 = 0, ui32Hi0 = 0, ui32Hi1 = 0;

			unsigned char ucB = 0;
			ucB = _subborrow_u32( ucB, ui32ALo0, ui32BLo0, &ui32Lo0 );
			ucB = _subborrow_u32( ucB, ui32ALo1, ui32BLo1, &ui32Lo1 );
			ucB = _subborrow_u32( ucB, ui32AHi0, ui32BHi0, &ui32Hi0 );
			_subborrow_u32( ucB, ui32AHi1, ui32BHi1, &ui32Hi1 );

			const uint64_t ui64Lo = (static_cast<uint64_t>(ui32Lo1) << 32) | ui32Lo0;
			const uint64_t ui64Hi = (static_cast<uint64_t>(ui32Hi1) << 32) | ui32Hi0;

			if ( _pui64HighDiff ) { (*_pui64HighDiff) = ui64Hi; }
			return ui64Lo;
		}

		/**
		 * \brief Subtracts two signed 128-bit values (hi:lo - hi:lo) and returns the low 64 bits.
		 *
		 * \param _i64AHigh High 64 bits of the minuend.
		 * \param _i64ALow Low 64 bits of the minuend.
		 * \param _i64BHigh High 64 bits of the subtrahend.
		 * \param _i64BLow Low 64 bits of the subtrahend.
		 * \param _pi64HighDiff Receives the high 64 bits of the difference.
		 * \return Returns the low 64 bits of the difference.
		 **/
		inline int64_t					_sub128( int64_t _i64AHigh, int64_t _i64ALow, int64_t _i64BHigh, int64_t _i64BLow, int64_t * _pi64HighDiff ) {
			uint64_t ui64Hi = 0;
			const uint64_t ui64Lo = _usub128(
				static_cast<uint64_t>(_i64AHigh),
				static_cast<uint64_t>(_i64ALow),
				static_cast<uint64_t>(_i64BHigh),
				static_cast<uint64_t>(_i64BLow),
				&ui64Hi
			);

			if ( _pi64HighDiff ) { (*_pi64HighDiff) = static_cast<int64_t>(ui64Hi); }
			return static_cast<int64_t>(ui64Lo);
		}

		/**
		 * \brief Performs unsigned 128-bit-by-64-bit division returning a 64-bit unsigned quotient and (optionally) a 64-bit unsigned remainder.
		 *
		 * Given a unsigned 128-bit dividend split into high and low 64-bit parts, and a unsigned 64-bit divisor,
		 * this routine computes the C/C++-semantics result where the quotient is truncated toward zero and the remainder
		 * has the same sign as the dividend with |remainder| < |divisor|.
		 * Internally it reduces to an unsigned divide via _udiv128() on absolute magnitudes and then fixes signs,
		 * detecting overflow of the 64-bit unsigned quotient.
		 *
		 * \param _ui64High			The unsigned high 64 bits of the 128-bit dividend (sign bit is taken from this).
		 * \param _ui64Low			The unsigned low 64 bits of the 128-bit dividend.
		 * \param _ui64Divisor      The unsigned 64-bit divisor.
		 * \param _pui64Remainder   Optional pointer that receives the unsigned remainder (same sign as the dividend).
		 * \return Returns the unsigned 64-bit quotient.
		 * \throws std::overflow_error On division by zero or when the true quotient does not fit in a unsigned 64-bit integer.
		 */
		inline uint64_t					_udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
			if ( _ui64Divisor == 0 ) [[unlikely]] { throw std::overflow_error( "Division by zero is not allowed." ); }
			if ( _ui64High >= _ui64Divisor ) [[unlikely]] { throw std::overflow_error( "The division would overflow the 64-bit quotient." ); }
			if ( _ui64High == 0 ) {
				if ( _pui64Remainder ) { (*_pui64Remainder) = _ui64Low % _ui64Divisor; }
				return _ui64Low / _ui64Divisor;
			}

			// == Knuth-style division in base b = 2^32.
			constexpr uint64_t ui64BitsPerWord = sizeof( uint64_t ) * 8;
			constexpr uint64_t ui64Base = 1ULL << (ui64BitsPerWord / 2);	// b = 2^32

			uint64_t ui64V = _ui64Divisor;
			uint64_t ui64Un64;												// High "digit" after normalization step.
			uint64_t ui64Un10;												// Low 64 after normalization step.

			// Normalize: shift left so that the top bit of ui64V is set.
			unsigned int uiShift = static_cast<unsigned int>(std::countl_zero( ui64V ));
			if ( uiShift > 0 ) {
				ui64V = ui64V << uiShift;
				ui64Un64 = (_ui64High << uiShift) | (_ui64Low >> (ui64BitsPerWord - uiShift));
				ui64Un10 = _ui64Low << uiShift;
			}
			else {
				// Avoid (x >> 64) UB for uiShift==0.
				ui64Un64 = _ui64High;
				ui64Un10 = _ui64Low;
			}

			// Split divisor into two 32-bit digits.
			const uint64_t ui64Vn1 = ui64V >> (ui64BitsPerWord / 2);
			const uint64_t ui64Vn0 = ui64V & 0xFFFFFFFFULL;

			// Split the low (normalized) 64 into two 32-bit digits.
			uint64_t ui64Un1 = ui64Un10 >> (ui64BitsPerWord / 2);
			uint64_t ui64Un0 = ui64Un10 & 0xFFFFFFFFULL;

			// First quotient digit q1.
			uint64_t ui64Q1 = ui64Un64 / ui64Vn1;
			uint64_t ui64Rhat = ui64Un64 - ui64Q1 * ui64Vn1;

			// Correct q1 (at most 2 decrements).
			while ( ui64Q1 >= ui64Base || ui64Q1 * ui64Vn0 > ui64Base * ui64Rhat + ui64Un1 ) {
				ui64Q1 -= 1;
				ui64Rhat += ui64Vn1;
				if ( ui64Rhat >= ui64Base ) { break; }
			}

			// Combine and subtract q1 * v.
			uint64_t ui64Un21 = ui64Un64 * ui64Base + ui64Un1 - ui64Q1 * ui64V;

			// Second quotient digit q0.
			uint64_t ui64Q0 = ui64Un21 / ui64Vn1;
			ui64Rhat = ui64Un21 - ui64Q0 * ui64Vn1;

			// Correct q0 (at most 2 decrements).
			while ( ui64Q0 >= ui64Base || ui64Q0 * ui64Vn0 > ui64Base * ui64Rhat + ui64Un0 ) {
				ui64Q0 -= 1;
				ui64Rhat += ui64Vn1;
				if ( ui64Rhat >= ui64Base ) { break; }
			}

			// Remainder (denormalize back).
			if ( _pui64Remainder ) {
				(*_pui64Remainder) = ((ui64Un21 * ui64Base + ui64Un0) - ui64Q0 * ui64V) >> uiShift;
			}

			// Quotient.
			return ui64Q1 * ui64Base + ui64Q0;

			/*uint64_t ui64Q = 0;
			uint64_t ui64R = _ui64High;

			for ( int I = 63; I >= 0; --I ) {
				ui64R = (ui64R << 1) | ((_ui64Low >> I) & 1);

				if ( ui64R >= _ui64Divisor ) {
					ui64R -= _ui64Divisor;
					ui64Q |= 1ULL << I;
				}
			}

			if ( _pui64Remainder ) { (*_pui64Remainder) = ui64R; }
			return ui64Q;*/
		}

		/**
		 * \brief Performs signed 128-bit-by-64-bit division returning a 64-bit signed quotient and (optionally) a 64-bit signed remainder.
		 *
		 * Given a signed 128-bit dividend split into high and low 64-bit parts, and a signed 64-bit divisor,
		 * this routine computes the C/C++-semantics result where the quotient is truncated toward zero and the remainder
		 * has the same sign as the dividend with |remainder| < |divisor|.
		 * Internally it reduces to an unsigned divide via _udiv128() on absolute magnitudes and then fixes signs,
		 * detecting overflow of the 64-bit signed quotient.
		 *
		 * \param _i64HighDividend The signed high 64 bits of the 128-bit dividend (sign bit is taken from this).
		 * \param _i64LowDividend  The signed low 64 bits of the 128-bit dividend.
		 * \param _i64Divisor      The signed 64-bit divisor.
		 * \param _pi64Remainder   Optional pointer that receives the signed remainder (same sign as the dividend).
		 * \return Returns the signed 64-bit quotient.
		 * \throws std::overflow_error On division by zero or when the true quotient does not fit in a signed 64-bit integer.
		 */
		inline int64_t					_div128( int64_t _i64HighDividend, int64_t _i64LowDividend, int64_t _i64Divisor, int64_t * _pi64Remainder ) {
			// Validate divisor.
			if ( _i64Divisor == 0 ) [[unlikely]] { throw std::overflow_error( "Division by zero is not allowed." ); }

			// Determine signs and compute absolute magnitudes without invoking UB on INT64_MIN.
			const bool bDividendNeg		= (_i64HighDividend < 0);
			const bool bDivisorNeg		= (_i64Divisor < 0);
			const bool bQuotNeg			= (bDividendNeg != bDivisorNeg);		// XOR: quotient sign.
			const bool bRemNeg			= bDividendNeg;							// Remainder sign matches dividend (C++ semantics).

			// Represent the 128-bit dividend as unsigned parts; bit patterns preserved on cast.
			uint64_t ui64Hi = static_cast<uint64_t>(_i64HighDividend);
			uint64_t ui64Lo = static_cast<uint64_t>(_i64LowDividend);

			// Two's-complement negate (128-bit) if the dividend is negative: (hi,lo) = - (hi,lo).
			auto Negate128 = []( uint64_t &_ui64Hi, uint64_t &_ui64Lo ) {
				uint64_t ui64NewLo = ~_ui64Lo + 1ULL;							// Add 1 to low after invert.
				_ui64Hi = ~_ui64Hi + (ui64NewLo == 0ULL ? 1ULL : 0ULL);			// Propagate carry into high if low wrapped.
				_ui64Lo = ui64NewLo;
			};
			if ( bDividendNeg ) {
				Negate128( ui64Hi, ui64Lo );
			}

			// Absolute value of the divisor as unsigned using two's-complement trick (works for INT64_MIN).
			uint64_t ui64Div = bDivisorNeg ? static_cast<uint64_t>(0ULL - static_cast<uint64_t>(_i64Divisor)) :
				static_cast<uint64_t>(_i64Divisor);

			// Use existing unsigned 128/64 divide to get magnitude of quotient and remainder.
			uint64_t ui64Rem = 0ULL;
			const uint64_t ui64UQuot = _udiv128( ui64Hi, ui64Lo, ui64Div, &ui64Rem );

			// Check for signed 64-bit overflow of the quotient's final value.
			// If the result is non-negative, it must be <= INT64_MAX.
			// If the result is negative, magnitude may be up to 2^63 (i.e., 0x8000'0000'0000'0000).
			if ( !bQuotNeg ) {
				if ( ui64UQuot > static_cast<uint64_t>(INT64_MAX) ) [[unlikely]] {
					throw std::overflow_error( "The division would overflow the 64-bit signed quotient." );
				}
			}
			else {
				if ( ui64UQuot > (1ULL << 63) ) [[unlikely]] {
					throw std::overflow_error( "The division would overflow the 64-bit signed quotient." );
				}
			}

			// Form the signed quotient without invoking UB on INT64_MIN.
			// For negative: two's-complement via (0 - magnitude) in unsigned domain then cast.
			const int64_t i64Quot = bQuotNeg ? static_cast<int64_t>(static_cast<uint64_t>(0ULL - ui64UQuot)) :
				static_cast<int64_t>(ui64UQuot);

			// Signed remainder (same sign as dividend).
			if ( _pi64Remainder ) {
				(*_pi64Remainder) = bRemNeg ? static_cast<int64_t>(static_cast<uint64_t>(0ULL - ui64Rem)) :
					static_cast<int64_t>(ui64Rem);
			}

			return i64Quot;
		}

		/**
		 * \brief Computes u64 * u64 -> u128, returning the low 64 bits and writing the high 64 bits.
		 *
		 * \param _ui64Multiplier The first operand.
		 * \param _ui64Multiplicand The second operand.
		 * \param _pui64ProductHi Receives the high 64 bits of the product.
		 * \return Returns the low 64 bits of the product.
		 **/
		inline uint64_t					_umul128( uint64_t _ui64Multiplier, uint64_t _ui64Multiplicand, uint64_t * _pui64ProductHi ) {
			assert( _pui64ProductHi );

			// _ui64Multiplier   = ab = a * 2^32 + b
			// _ui64Multiplicand = cd = c * 2^32 + d
			// ab * cd = a * c * 2^64 + (a * d + b * c) * 2^32 + b * d
			uint64_t ui64A = _ui64Multiplier >> 32;
			uint64_t ui64B = static_cast<uint32_t>(_ui64Multiplier);
			uint64_t ui64C = _ui64Multiplicand >> 32;
			uint64_t ui64D = static_cast<uint32_t>(_ui64Multiplicand);

			uint64_t ui64Ad = __emulu( static_cast<unsigned int>(ui64A), static_cast<unsigned int>(ui64D) );
			uint64_t ui64Bd = __emulu( static_cast<unsigned int>(ui64B), static_cast<unsigned int>(ui64D) );

			uint64_t ui64Abdc = ui64Ad + __emulu( static_cast<unsigned int>(ui64B), static_cast<unsigned int>(ui64C) );
			uint64_t ui64AbdcCarry = (ui64Abdc < ui64Ad);

			// _ui64Multiplier * _ui64Multiplicand = _pui64ProductHi * 2^64 + ui64ProductLo
			uint64_t ui64ProductLo = ui64Bd + (ui64Abdc << 32);
			uint64_t ui64ProductLoCarry = (ui64ProductLo < ui64Bd);
			(*_pui64ProductHi) = __emulu( static_cast<unsigned int>(ui64A), static_cast<unsigned int>(ui64C) ) + (ui64Abdc >> 32) + (ui64AbdcCarry << 32) + ui64ProductLoCarry;

			return ui64ProductLo;
		}

		/**
		 * \brief Computes s64 * s64 -> s128, returning the low 64 bits and writing the high 64 bits.
		 *
		 * \param _i64Multiplier The first operand.
		 * \param _i64Multiplicand The second operand.
		 * \param _pi64HighProduct Receives the high 64 bits of the product.
		 * \return Returns the low 64 bits of the product.
		 **/
		inline int64_t					_mul128( int64_t _i64Multiplier, int64_t _i64Multiplicand, int64_t * _pi64HighProduct ) {
			assert( _pi64HighProduct );

			// Do unsigned multiply on magnitudes, then apply sign to the 128-bit result.
			uint64_t ui64A = static_cast<uint64_t>(_i64Multiplier);
			uint64_t ui64B = static_cast<uint64_t>(_i64Multiplicand);

			const bool bNegA = (_i64Multiplier < 0);
			const bool bNegB = (_i64Multiplicand < 0);
			const bool bNeg = (bNegA != bNegB);

			if ( bNegA ) { ui64A = (~ui64A) + 1ULL; }
			if ( bNegB ) { ui64B = (~ui64B) + 1ULL; }

			uint64_t ui64Hi = 0;
			uint64_t ui64Lo = _umul128( ui64A, ui64B, &ui64Hi );

			if ( bNeg ) {
				// Two's-complement negate 128-bit (hi:lo).
				ui64Lo = (~ui64Lo) + 1ULL;
				ui64Hi = (~ui64Hi) + (ui64Lo == 0 ? 1ULL : 0ULL);
			}

			(*_pi64HighProduct) = static_cast<int64_t>(ui64Hi);
			return static_cast<int64_t>(ui64Lo);
		}
	#endif  // #ifdef _WIN64
#elif defined( __x86_64__ ) || defined( _M_X64 )
	#include <immintrin.h>

	/**
	 * \brief Adds two unsigned 128-bit values (hi:lo + hi:lo) and returns the low 64 bits.
	 *
	 * \param _ui64AHigh High 64 bits of the first addend.
	 * \param _ui64ALow Low 64 bits of the first addend.
	 * \param _ui64BHigh High 64 bits of the second addend.
	 * \param _ui64BLow Low 64 bits of the second addend.
	 * \param _pui64HighSum Receives the high 64 bits of the sum.
	 * \return Returns the low 64 bits of the sum.
	 **/
	inline uint64_t						_uadd128( uint64_t _ui64AHigh, uint64_t _ui64ALow, uint64_t _ui64BHigh, uint64_t _ui64BLow, uint64_t * _pui64HighSum ) {
		uint64_t ui64Lo = _ui64ALow;
		uint64_t ui64Hi = _ui64AHigh;

		asm volatile(
			"addq %2, %0\n\t"
			"adcq %3, %1"
			: "+r"(ui64Lo), "+r"(ui64Hi)
			: "r"(_ui64BLow), "r"(_ui64BHigh)
			: "cc"
		);

		if ( _pui64HighSum ) { (*_pui64HighSum) = ui64Hi; }
		return ui64Lo;
	}

	/**
	 * \brief Adds two signed 128-bit values (hi:lo + hi:lo) and returns the low 64 bits.
	 *
	 * \param _i64AHigh High 64 bits of the first addend.
	 * \param _i64ALow Low 64 bits of the first addend.
	 * \param _i64BHigh High 64 bits of the second addend.
	 * \param _i64BLow Low 64 bits of the second addend.
	 * \param _pi64HighSum Receives the high 64 bits of the sum.
	 * \return Returns the low 64 bits of the sum.
	 **/
	inline int64_t						_add128( int64_t _i64AHigh, int64_t _i64ALow, int64_t _i64BHigh, int64_t _i64BLow, int64_t * _pi64HighSum ) {
		uint64_t ui64Hi = 0;
		const uint64_t ui64Lo = _uadd128(
			static_cast<uint64_t>(_i64AHigh),
			static_cast<uint64_t>(_i64ALow),
			static_cast<uint64_t>(_i64BHigh),
			static_cast<uint64_t>(_i64BLow),
			&ui64Hi
		);

		if ( _pi64HighSum ) { (*_pi64HighSum) = static_cast<int64_t>(ui64Hi); }
		return static_cast<int64_t>(ui64Lo);
	}

	/**
	 * \brief Subtracts two unsigned 128-bit values (hi:lo - hi:lo) and returns the low 64 bits.
	 *
	 * \param _ui64AHigh High 64 bits of the minuend.
	 * \param _ui64ALow Low 64 bits of the minuend.
	 * \param _ui64BHigh High 64 bits of the subtrahend.
	 * \param _ui64BLow Low 64 bits of the subtrahend.
	 * \param _pui64HighDiff Receives the high 64 bits of the difference.
	 * \return Returns the low 64 bits of the difference.
	 **/
	inline uint64_t						_usub128( uint64_t _ui64AHigh, uint64_t _ui64ALow, uint64_t _ui64BHigh, uint64_t _ui64BLow, uint64_t * _pui64HighDiff ) {
		uint64_t ui64Lo = _ui64ALow;
		uint64_t ui64Hi = _ui64AHigh;

		asm volatile(
			"subq %2, %0\n\t"
			"sbbq %3, %1"
			: "+r"(ui64Lo), "+r"(ui64Hi)
			: "r"(_ui64BLow), "r"(_ui64BHigh)
			: "cc"
		);

		if ( _pui64HighDiff ) { (*_pui64HighDiff) = ui64Hi; }
		return ui64Lo;
	}

	/**
	 * \brief Subtracts two signed 128-bit values (hi:lo - hi:lo) and returns the low 64 bits.
	 *
	 * \param _i64AHigh High 64 bits of the minuend.
	 * \param _i64ALow Low 64 bits of the minuend.
	 * \param _i64BHigh High 64 bits of the subtrahend.
	 * \param _i64BLow Low 64 bits of the subtrahend.
	 * \param _pi64HighDiff Receives the high 64 bits of the difference.
	 * \return Returns the low 64 bits of the difference.
	 **/
	inline int64_t						_sub128( int64_t _i64AHigh, int64_t _i64ALow, int64_t _i64BHigh, int64_t _i64BLow, int64_t * _pi64HighDiff ) {
		uint64_t ui64Hi = 0;
		const uint64_t ui64Lo = _usub128(
			static_cast<uint64_t>(_i64AHigh),
			static_cast<uint64_t>(_i64ALow),
			static_cast<uint64_t>(_i64BHigh),
			static_cast<uint64_t>(_i64BLow),
			&ui64Hi
		);

		if ( _pi64HighDiff ) { (*_pi64HighDiff) = static_cast<int64_t>(ui64Hi); }
		return static_cast<int64_t>(ui64Lo);
	}

	/**
	 * \brief Performs unsigned 128-bit-by-64-bit division returning a 64-bit unsigned quotient and (optionally) a 64-bit unsigned remainder.
	 *
	 * Given a unsigned 128-bit dividend split into high and low 64-bit parts, and a unsigned 64-bit divisor,
	 * this routine computes the C/C++-semantics result where the quotient is truncated toward zero and the remainder
	 * has the same sign as the dividend with |remainder| < |divisor|.
	 * Internally it reduces to an unsigned divide via _udiv128() on absolute magnitudes and then fixes signs,
	 * detecting overflow of the 64-bit unsigned quotient.  Implementation using x86_64 assembly for GCC and Clang.
	 *
	 * \param _ui64High			The unsigned high 64 bits of the 128-bit dividend (sign bit is taken from this).
	 * \param _ui64Low			The unsigned low 64 bits of the 128-bit dividend.
	 * \param _ui64Divisor      The unsigned 64-bit divisor.
	 * \param _pui64Remainder   Optional pointer that receives the unsigned remainder (same sign as the dividend).
	 * \return Returns the unsigned 64-bit quotient.
	 * \throws std::overflow_error On division by zero or when the true quotient does not fit in a unsigned 64-bit integer.
	 */ 
	inline uint64_t                    _udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
		uint64_t ui64Quot, ui64Rem;

		asm volatile(
			"divq %4"
			: "=a"(ui64Quot), "=d"(ui64Rem)
			: "a"(_ui64Low), "d"(_ui64High), "r"(_ui64Divisor)
			: "cc"
		);

		if ( _pui64Remainder ) {
			(*_pui64Remainder) = ui64Rem;
		}
		return ui64Quot;
	}

	/**
	 * \brief Performs signed 128-bit-by-64-bit division returning a 64-bit signed quotient and (optionally) a 64-bit signed remainder.
	 *
	 * Given a signed 128-bit dividend split into high and low 64-bit parts, and a signed 64-bit divisor,
	 * this routine computes the C/C++-semantics result where the quotient is truncated toward zero and the remainder
	 * has the same sign as the dividend with |remainder| < |divisor|.
	 * Internally it reduces to an unsigned divide via _udiv128() on absolute magnitudes and then fixes signs,
	 * detecting overflow of the 64-bit signed quotient.  Signed 128/64 -> 64 division (RDX:RAX / r/m64), quotient in RAX, remainder in RDX.
	 *
	 * \param _i64HighDividend The signed high 64 bits of the 128-bit dividend (sign bit is taken from this).
	 * \param _i64LowDividend  The signed low 64 bits of the 128-bit dividend.
	 * \param _i64Divisor      The signed 64-bit divisor.
	 * \param _pi64Remainder   Optional pointer that receives the signed remainder (same sign as the dividend).
	 * \return Returns the signed 64-bit quotient.
	 * \throws std::overflow_error On division by zero or when the true quotient does not fit in a signed 64-bit integer.
	 */
	inline int64_t						_div128( int64_t _i64High, int64_t _i64Low, int64_t _i64Divisor, int64_t * _pi64Remainder ) {
		int64_t i64Quot = 0;
		int64_t i64Rem = 0;

		asm volatile(
			"idivq %4"
			: "=a"(i64Quot), "=d"(i64Rem)
			: "a"(_i64Low), "d"(_i64High), "rm"(_i64Divisor)
			: "cc"
		);

		if ( _pi64Remainder ) {
			(*_pi64Remainder) = i64Rem;
		}
		return i64Quot;
	}

	/**
	 * \brief Computes u64 * u64 -> u128, returning the low 64 bits and writing the high 64 bits.  Unsigned 64 * 64 -> 128: RDX:RAX = RAX * r/m64.
	 *
	 * \param _ui64Multiplier The first operand.
	 * \param _ui64Multiplicand The second operand.
	 * \param _pui64ProductHi Receives the high 64 bits of the product.
	 * \return Returns the low 64 bits of the product.
	 **/
	inline uint64_t						_umul128( uint64_t _ui64Multiplier, uint64_t _ui64Multiplicand, uint64_t * _pui64HighProduct ) {
		uint64_t ui64A = _ui64Multiplier;	// RAX input -> low output.
		uint64_t ui64D = 0;					// RDX output.

		asm volatile(
			"mulq %2"
			: "+a"(ui64A), "=d"(ui64D)
			: "rm"(_ui64Multiplicand)
			: "cc"
		);

		if ( _pui64HighProduct ) {
			(*_pui64HighProduct) = ui64D;
		}
		return ui64A;
	}

	/**
	 * \brief Computes s64 * s64 -> s128, returning the low 64 bits and writing the high 64 bits.  Signed 64 * 64 -> 128: RDX:RAX = RAX * r/m64 (signed).
	 *
	 * \param _i64Multiplier The first operand.
	 * \param _i64Multiplicand The second operand.
	 * \param _pi64HighProduct Receives the high 64 bits of the product.
	 * \return Returns the low 64 bits of the product.
	 **/
	inline int64_t						_mul128( int64_t _i64Multiplier, int64_t _i64Multiplicand, int64_t * _pi64HighProduct ) {
		int64_t i64A = _i64Multiplier;		// RAX input -> low output.
		int64_t i64D = 0;					// RDX output.

		asm volatile(
			"imulq %2"
			: "+a"(i64A), "=d"(i64D)
			: "rm"(_i64Multiplicand)
			: "cc"
		);

		if ( _pi64HighProduct ) {
			(*_pi64HighProduct) = i64D;
		}
		return i64A;
	}

#elif defined( __SIZEOF_INT128__ )
	/**
	 * \brief Adds two unsigned 128-bit values (hi:lo + hi:lo) and returns the low 64 bits.
	 *
	 * \param _ui64AHigh High 64 bits of the first addend.
	 * \param _ui64ALow Low 64 bits of the first addend.
	 * \param _ui64BHigh High 64 bits of the second addend.
	 * \param _ui64BLow Low 64 bits of the second addend.
	 * \param _pui64HighSum Receives the high 64 bits of the sum.
	 * \return Returns the low 64 bits of the sum.
	 **/
	inline uint64_t						_uadd128( uint64_t _ui64AHigh, uint64_t _ui64ALow, uint64_t _ui64BHigh, uint64_t _ui64BLow, uint64_t * _pui64HighSum ) {
		const __uint128_t ui128A = (static_cast<__uint128_t>(_ui64AHigh) << 64) | static_cast<__uint128_t>(_ui64ALow);
		const __uint128_t ui128B = (static_cast<__uint128_t>(_ui64BHigh) << 64) | static_cast<__uint128_t>(_ui64BLow);
		const __uint128_t ui128S = ui128A + ui128B;

		if ( _pui64HighSum ) { (*_pui64HighSum) = static_cast<uint64_t>(ui128S >> 64); }
		return static_cast<uint64_t>(ui128S);
	}

	/**
	 * \brief Adds two signed 128-bit values (hi:lo + hi:lo) and returns the low 64 bits.
	 *
	 * \param _i64AHigh High 64 bits of the first addend.
	 * \param _i64ALow Low 64 bits of the first addend.
	 * \param _i64BHigh High 64 bits of the second addend.
	 * \param _i64BLow Low 64 bits of the second addend.
	 * \param _pi64HighSum Receives the high 64 bits of the sum.
	 * \return Returns the low 64 bits of the sum.
	 **/
	inline int64_t						_add128( int64_t _i64AHigh, int64_t _i64ALow, int64_t _i64BHigh, int64_t _i64BLow, int64_t * _pi64HighSum ) {
		const __int128 i128A =
			(static_cast<__int128>(_i64AHigh) << 64) |
			static_cast<__int128>(static_cast<unsigned __int128>(static_cast<uint64_t>(_i64ALow)));
		const __int128 i128B =
			(static_cast<__int128>(_i64BHigh) << 64) |
			static_cast<__int128>(static_cast<unsigned __int128>(static_cast<uint64_t>(_i64BLow)));
		const __int128 i128S = i128A + i128B;

		if ( _pi64HighSum ) { (*_pi64HighSum) = static_cast<int64_t>(i128S >> 64); }
		return static_cast<int64_t>(i128S);
	}

	/**
	 * \brief Subtracts two unsigned 128-bit values (hi:lo - hi:lo) and returns the low 64 bits.
	 *
	 * \param _ui64AHigh High 64 bits of the minuend.
	 * \param _ui64ALow Low 64 bits of the minuend.
	 * \param _ui64BHigh High 64 bits of the subtrahend.
	 * \param _ui64BLow Low 64 bits of the subtrahend.
	 * \param _pui64HighDiff Receives the high 64 bits of the difference.
	 * \return Returns the low 64 bits of the difference.
	 **/
	inline uint64_t						_usub128( uint64_t _ui64AHigh, uint64_t _ui64ALow, uint64_t _ui64BHigh, uint64_t _ui64BLow, uint64_t * _pui64HighDiff ) {
		const __uint128_t ui128A = (static_cast<__uint128_t>(_ui64AHigh) << 64) | static_cast<__uint128_t>(_ui64ALow);
		const __uint128_t ui128B = (static_cast<__uint128_t>(_ui64BHigh) << 64) | static_cast<__uint128_t>(_ui64BLow);
		const __uint128_t ui128D = ui128A - ui128B;

		if ( _pui64HighDiff ) { (*_pui64HighDiff) = static_cast<uint64_t>(ui128D >> 64); }
		return static_cast<uint64_t>(ui128D);
	}

	/**
	 * \brief Subtracts two signed 128-bit values (hi:lo - hi:lo) and returns the low 64 bits.
	 *
	 * \param _i64AHigh High 64 bits of the minuend.
	 * \param _i64ALow Low 64 bits of the minuend.
	 * \param _i64BHigh High 64 bits of the subtrahend.
	 * \param _i64BLow Low 64 bits of the subtrahend.
	 * \param _pi64HighDiff Receives the high 64 bits of the difference.
	 * \return Returns the low 64 bits of the difference.
	 **/
	inline int64_t						_sub128( int64_t _i64AHigh, int64_t _i64ALow, int64_t _i64BHigh, int64_t _i64BLow, int64_t * _pi64HighDiff ) {
		const __int128 i128A =
			(static_cast<__int128>(_i64AHigh) << 64) |
			static_cast<__int128>(static_cast<unsigned __int128>(static_cast<uint64_t>(_i64ALow)));
		const __int128 i128B =
			(static_cast<__int128>(_i64BHigh) << 64) |
			static_cast<__int128>(static_cast<unsigned __int128>(static_cast<uint64_t>(_i64BLow)));
		const __int128 i128D = i128A - i128B;

		if ( _pi64HighDiff ) { (*_pi64HighDiff) = static_cast<int64_t>(i128D >> 64); }
		return static_cast<int64_t>(i128D);
	}

	/**
	 * \brief Divides an unsigned 128-bit dividend by a 64-bit unsigned divisor.
	 *
	 * The dividend is provided as a high/low pair (high:low). The quotient is returned as a 64-bit value,
	 * and the remainder is optionally written to *_pui64Remainder.
	 *
	 * This matches the typical _udiv128 contract: the quotient must fit in 64 bits, which requires
	 * _ui64High < _ui64Divisor.
	 *
	 * \param _ui64High High 64 bits of the unsigned 128-bit dividend.
	 * \param _ui64Low Low 64 bits of the unsigned 128-bit dividend.
	 * \param _ui64Divisor The 64-bit unsigned divisor.
	 * \param _pui64Remainder Optional pointer that receives the 64-bit unsigned remainder.
	 * \return Returns the 64-bit unsigned quotient.
	 *
	 * \throws std::overflow_error Thrown if _ui64Divisor is 0.
	 * \throws std::overflow_error Thrown if the quotient would overflow 64 bits (i.e. _ui64High >= _ui64Divisor).
	 **/
	inline uint64_t                     _udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
		if ( _ui64Divisor == 0 ) {
			throw std::overflow_error( "_udiv128: Division by zero is not allowed." );
		}

		if ( _ui64High >= _ui64Divisor ) {
			throw std::overflow_error( "_udiv128: The division would overflow the 64-bit quotient." );
		}

		if ( _ui64High == 0 ) {
			if ( _pui64Remainder ) { (*_pui64Remainder) = _ui64Low % _ui64Divisor; }
			return _ui64Low / _ui64Divisor;
		}
	
		// Combine the high and low parts into a single __uint128_t value.
		__uint128_t ui128Dividend = static_cast<__uint128_t>(_ui64High) << 64 | _ui64Low;
	
		if ( _pui64Remainder ) {
			(*_pui64Remainder) = static_cast<uint64_t>(ui128Dividend % _ui64Divisor);
		}
		return static_cast<uint64_t>(ui128Dividend / _ui64Divisor);
	}

	/**
	 * \brief Divides a signed 128-bit dividend by a 64-bit signed divisor.
	 *
	 * The dividend is provided as a high/low pair (high:low), where _i64High is the sign-extended high 64 bits.
	 * The quotient is returned as a 64-bit signed value, and the remainder is optionally written to *_pi64Remainder.
	 *
	 * Division follows C/C++ semantics: truncation toward zero, and the remainder has the same sign as the dividend.
	 * This matches the typical _div128 contract: the quotient must fit in int64_t, otherwise overflow is reported.
	 *
	 * \param _i64High High 64 bits of the signed 128-bit dividend (sign-extended).
	 * \param _i64Low Low 64 bits of the signed 128-bit dividend.
	 * \param _i64Divisor The 64-bit signed divisor.
	 * \param _pi64Remainder Optional pointer that receives the 64-bit signed remainder.
	 * \return Returns the 64-bit signed quotient.
	 *
	 * \throws std::overflow_error Thrown if _i64Divisor is 0.
	 * \throws std::overflow_error Thrown if the quotient would overflow int64_t.
	 **/
	inline int64_t						_div128( int64_t _i64High, int64_t _i64Low, int64_t _i64Divisor, int64_t * _pi64Remainder ) {
		if ( _i64Divisor == 0 ) {
			throw std::overflow_error( "_div128: Division by zero is not allowed." );
		}

		// Combine the high and low parts into a single __int128 value.
		// Note: The low part is treated as an unsigned 64-bit chunk when OR'ing into the 128-bit value.
		const __int128 i128Dividend =
			(static_cast<__int128>(_i64High) << 64) |
			static_cast<__int128>(static_cast<unsigned __int128>(static_cast<uint64_t>(_i64Low)));

		// Compute quotient and remainder using truncation toward zero (C/C++ semantics).
		const __int128 i128Quot = i128Dividend / static_cast<__int128>(_i64Divisor);
		const __int128 i128Rem = i128Dividend % static_cast<__int128>(_i64Divisor);

		// The MSVC-style _div128 contract returns an int64 quotient; overflow must be reported.
		if ( i128Quot < static_cast<__int128>(INT64_MIN) || i128Quot > static_cast<__int128>(INT64_MAX) ) {
			throw std::overflow_error( "_div128: The division would overflow the 64-bit quotient." );
		}

		if ( _pi64Remainder ) {
			// Remainder is always in [-|divisor|+1, |divisor|-1], so it fits in int64_t.
			(*_pi64Remainder) = static_cast<int64_t>(i128Rem);
		}

		return static_cast<int64_t>(i128Quot);
	}

	/**
	 * \brief Computes u64 * u64 -> u128, returning the low 64 bits and writing the high 64 bits.
	 *
	 * \param _ui64Multiplier The first operand.
	 * \param _ui64Multiplicand The second operand.
	 * \param _pui64ProductHi Receives the high 64 bits of the product.
	 * \return Returns the low 64 bits of the product.
	 **/
	inline uint64_t						_umul128( uint64_t _ui64Multiplier, uint64_t _ui64Multiplicand, uint64_t * _pui64ProductHi ) {
		assert( _pui64ProductHi );

		__uint128_t ui128Tmp = static_cast<__uint128_t>(_ui64Multiplier) * static_cast<__uint128_t>(_ui64Multiplicand);
		(*_pui64ProductHi) = static_cast<uint64_t>(ui128Tmp >> 64);
		return static_cast<uint64_t>(ui128Tmp);
	}

	/**
	 * \brief Computes s64 * s64 -> s128, returning the low 64 bits and writing the high 64 bits.
	 *
	 * \param _i64Multiplier The first operand.
	 * \param _i64Multiplicand The second operand.
	 * \param _pi64HighProduct Receives the high 64 bits of the product.
	 * \return Returns the low 64 bits of the product.
	 **/
	inline int64_t						_mul128( int64_t _i64Multiplier, int64_t _i64Multiplicand, int64_t * _pi64HighProduct ) {
		assert( _pi64HighProduct );

		const __int128 i128Prod = static_cast<__int128>(_i64Multiplier) * static_cast<__int128>(_i64Multiplicand);
		(*_pi64HighProduct) = static_cast<int64_t>(i128Prod >> 64);
		return static_cast<int64_t>(i128Prod);
	}
#endif  // #if defined( _MSC_VER )


/**
 * \brief Multiplies two 64-bit unsigned values to a 128-bit intermediate, then divides by a 64-bit unsigned divisor.
 *
 * This is effectively: (a * b) / d with full 128-bit intermediate precision.
 *
 * Precondition (same as udiv128-style contract): d != 0 and high(a*b) < d (quotient fits in 64 bits).
 *
 * \param _ui64A The first multiplicand.
 * \param _ui64B The second multiplicand.
 * \param _ui64Divisor The divisor.
 * \param _pui64Remainder Receives the remainder.
 * \return Returns the 64-bit quotient.
 **/
static inline uint64_t					_umuldiv128( uint64_t _ui64A, uint64_t _ui64B, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
	assert( _pui64Remainder );
	assert( _ui64Divisor != 0 );

	uint64_t ui64Hi = 0;
	const uint64_t ui64Lo = _umul128( _ui64A, _ui64B, &ui64Hi );

	// Same contract as _udiv128(): quotient must fit in 64 bits.
	assert( ui64Hi < _ui64Divisor );

	return _udiv128( ui64Hi, ui64Lo, _ui64Divisor, _pui64Remainder );
}

/**
 * \brief Checked version of _umuldiv128() that returns false if the quotient would not fit in 64 bits.
 *
 * \param _ui64A The first multiplicand.
 * \param _ui64B The second multiplicand.
 * \param _ui64Divisor The divisor.
 * \param _pui64Quotient Receives the quotient.
 * \param _pui64Remainder Receives the remainder.
 * \return Returns true if the quotient fits in 64 bits; otherwise false (outputs not written).
 **/
static inline bool						_umuldiv128_checked( uint64_t _ui64A, uint64_t _ui64B, uint64_t _ui64Divisor, uint64_t * _pui64Quotient, uint64_t * _pui64Remainder ) {
	assert( _pui64Quotient );
	assert( _pui64Remainder );
	assert( _ui64Divisor != 0 );

	uint64_t ui64Hi = 0;
	const uint64_t ui64Lo = _umul128( _ui64A, _ui64B, &ui64Hi );

	if ( ui64Hi >= _ui64Divisor ) { return false; }

	(*_pui64Quotient) = _udiv128( ui64Hi, ui64Lo, _ui64Divisor, _pui64Remainder );
	return true;
}

/**
 * \brief Multiplies two 64-bit signed values to a 128-bit intermediate, then divides by a 64-bit signed divisor.
 *
 * This is effectively: (a * b) / d with full 128-bit intermediate precision (truncates toward 0).
 *
 * Precondition: d != 0 and quotient fits in 64 bits (same style/contract as _div128()).
 *
 * \param _i64A The first multiplicand.
 * \param _i64B The second multiplicand.
 * \param _i64Divisor The divisor.
 * \param _pi64Remainder Receives the remainder (same sign as dividend, per C/C++).
 * \return Returns the 64-bit quotient.
 **/
static inline int64_t					_muldiv128( int64_t _i64A, int64_t _i64B, int64_t _i64Divisor, int64_t * _pi64Remainder ) {
	assert( _pi64Remainder );
	assert( _i64Divisor != 0 );

	int64_t i64Hi = 0;
	const int64_t i64Lo = _mul128( _i64A, _i64B, &i64Hi );

	return _div128( i64Hi, i64Lo, _i64Divisor, _pi64Remainder );
}

/**
 * \brief Computes round((A * B) / Div) using a 128-bit intermediate.
 *
 * This is (A * B + Div/2) / Div, using full 128-bit precision for the product.
 * Throws on division by zero or if the quotient would overflow 64 bits (same policy as EE_muldiv128()).
 *
 * \param _ui64A The first value.
 * \param _ui64B The second value.
 * \param _ui64Divisor The divisor.
 * \param _pui64Remainder Receives the remainder after rounding.
 * \return Returns round((A * B) / Divisor).
 **/
static inline uint64_t					_umuldiv128_rounded( uint64_t _ui64A, uint64_t _ui64B, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
	assert( _pui64Remainder );
	assert( _ui64Divisor != 0 );

	uint64_t ui64Hi = 0;
	uint64_t ui64Lo = _umul128( _ui64A, _ui64B, &ui64Hi );

	// Add Div/2 for rounding, propagating carry into the high word.
	{
		const uint64_t ui64Add = _ui64Divisor >> 1;
		const uint64_t ui64Old = ui64Lo;
		ui64Lo += ui64Add;
		if ( ui64Lo < ui64Old ) { ++ui64Hi; }	// Lower-half overflow.
	}

	// Same contract as _udiv128(): quotient must fit in 64 bits.
	assert( ui64Hi < _ui64Divisor );

	return _udiv128( ui64Hi, ui64Lo, _ui64Divisor, _pui64Remainder );
}

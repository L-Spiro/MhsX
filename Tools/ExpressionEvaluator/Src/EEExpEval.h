#pragma once

#include "SinCos/EESinCos.h"
#include <cmath>
#include <cinttypes>
#include <numbers>
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

#ifndef EE_COUNT_OF
#define EE_COUNT_OF( x )				((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
#endif	// EE_COUNT_OF

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
#define EE_MAX_ITERATION_COUNT			0x10000000
#endif	// NDEBUG
#endif	// EE_MAX_ITERATION_COUNT

#ifndef EE_PI
#define EE_PI							3.1415926535897932384626433832795
#endif // #ifndef EE_PI

#if defined( _MSC_VER )
#define EE_FALLTHROUGH					[[fallthrough]];
#else
#define EE_FALLTHROUGH					
#endif	// #if defined( _MSC_VER )


namespace ee {

	// == Enumerations.
	// String numeric classifications.
	enum EE_STRING_NUMBER_CLASS {
		EE_SNC_UNSIGNED,
		EE_SNC_SIGNED,
		EE_SNC_FLOAT,
		EE_SNC_INVALID
	};
	class CExpEval {
	public :
		// == Types.
		/** Sinc-filter window function. */
		template <typename _tType = double>
		using							PfWindowFunc = bool (*)( size_t _sN, std::vector<_tType> &_vRet );

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
			std::wstring swsTemp;
			const char * pcSrc = _sIn.c_str();
			size_t sSize = _sIn.size();

			for ( size_t I = 0; I < sSize; ) {
				size_t sThisSize = 0;
				uint32_t ui32This = NextUtf8Char( &pcSrc[I], _sIn.size() - I, &sThisSize );
				if ( ui32This == EE_UTF_INVALID ) {
					for ( size_t J = 0; J < sThisSize; ++J ) {
						swsTemp.push_back( static_cast<uint8_t>(pcSrc[I+J]) );
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
		 * Creates a Hann window.
		 * 
		 * \param _sN	Number of samples.  Generally an odd number.
		 * \param _vRet	The returned vector.
		 * \return		Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						HannWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			try {
				_vRet.resize( _sN );

				const double dTau = 2.0 * std::numbers::pi;
				size_t stMax = _vRet.size() - 1;
				double dInvMax = 1.0 / stMax;
				double dTauInvMax = dTau * dInvMax;
				for ( auto I = _vRet.size(); I--; ) {
					_vRet[I] = _tType( 0.5 - 0.5 * std::cos( dTauInvMax * I ) );
				}

				return true;
			}
			catch ( ... ) { return false; }
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
			try {
				_vRet.resize( _sN );

				const double dTau = 2.0 * std::numbers::pi;
				size_t stMax = _vRet.size() - 1;
				double dInvMax = 1.0 / stMax;
				double dTauInvMax = dTau * dInvMax;
				for ( auto I = _vRet.size(); I--; ) {
					_vRet[I] = _tType( 0.53836 - 0.46164 * std::cos( dTauInvMax * I ) );
				}

				return true;
			}
			catch ( ... ) { return false; }
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
			try {
				_vRet.resize( _sN );

				const double dTau = 2.0 * std::numbers::pi;
				size_t stMax = _vRet.size() - 1;
				double dInvMax = 1.0 / stMax;
				double dTauInvMax = dTau * dInvMax;
				double dTauInvMax2 = 2.0 * dTauInvMax;
				for ( auto I = _vRet.size(); I--; ) {
					_vRet[I] = _tType( dA0 - dA1 * std::cos( dTauInvMax * I ) + dA2 * std::cos( dTauInvMax2 * I ) );
				}

				return true;
			}
			catch ( ... ) { return false; }
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
			try {
				_vRet.resize( _sN );

				size_t stMax = _vRet.size() - 1;
				double d2InvMax = 2.0 / stMax;
				double dHalfMax = stMax / 2.0;
				for ( auto I = _vRet.size(); I--; ) {
					_vRet[I] = _tType( d2InvMax * (dHalfMax - std::abs( I - dHalfMax )) );
				}

				return true;
			}
			catch ( ... ) { return false; }
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
		 * Creates a Kaiser window.
		 * 
		 * \param _sN		Number of samples.  Generally an odd number.
		 * \param _tBeta	The beta parameter.
		 * \param _vRet		The returned vector.
		 * \return			Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						KaiserWindow( size_t _sN, _tType _tBeta, std::vector<_tType> &_vRet ) {
			try {
				_vRet.resize( _sN );

				double dDenominator = Bessel0( double( _tBeta ) );

				for ( size_t I = 0; I < _sN; ++I ) {
					double dRatio = 0.0;
					if ( _sN > 1 ) {
						dRatio = (2.0 * I) / (_sN - 1) - 1.0;
					}
					double dArg = _tBeta * std::sqrt( 1.0 - dRatio * dRatio );
					_vRet[I] = _tType( Bessel0( dArg ) / dDenominator );
				}


				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Creates a Nuttal window.
		 * 
		 * \param _sN	Number of samples.  Generally an odd number.
		 * \param _vRet	The returned vector.
		 * \return		Returns true if there was enough memory to resize the vector.
		 **/
		template <typename _tType = double>
		static bool						NuttalWindow( size_t _sN, std::vector<_tType> &_vRet ) {
			constexpr double dA0 = 0.355768;			// 0.35576799999999997314859001562581397593021392822265625
			constexpr double dA1 = 0.487396;			// 0.487395999999999995910826555700623430311679840087890625
			constexpr double dA2 = 0.144232;			// 0.1442319999999999990958343687452725134789943695068359375
			constexpr double dA3 = 0.012604;			// 0.0126040000000000006197264923457623808644711971282958984375
			try {
				_vRet.resize( _sN );

				const double dTau = 2.0 * std::numbers::pi;
				size_t stMax = _vRet.size() - 1;
				double dInvMax = 1.0 / stMax;
				double dTauInvMax = dTau * dInvMax;
				double dTauInvMax2 = 2.0 * dTauInvMax;
				double dTauInvMax3 = 3.0 * dTauInvMax;
				for ( auto I = _vRet.size(); I--; ) {
					_vRet[I] = _tType( dA0 - dA1 * std::cos( dTauInvMax * I ) + dA2 * std::cos( dTauInvMax2 * I ) - dA3 * std::cos( dTauInvMax3 * I ) );
				}

				return true;
			}
			catch ( ... ) { return false; }
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
			try {
				_vRet.resize( _sN );

				const double dTau = 2.0 * std::numbers::pi;
				size_t stMax = _vRet.size() - 1;
				double dInvMax = 1.0 / stMax;
				double dTauInvMax = dTau * dInvMax;
				double dTauInvMax2 = 2.0 * dTauInvMax;
				double dTauInvMax3 = 3.0 * dTauInvMax;
				for ( auto I = _vRet.size(); I--; ) {
					_vRet[I] = _tType( dA0 - dA1 * std::cos( dTauInvMax * I ) + dA2 * std::cos( dTauInvMax2 * I ) - dA3 * std::cos( dTauInvMax3 * I ) );
				}

				return true;
			}
			catch ( ... ) { return false; }
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
			try {
				_vRet.resize( _sN );

				const double dTau = 2.0 * std::numbers::pi;
				size_t stMax = _vRet.size() - 1;
				double dInvMax = 1.0 / stMax;
				double dTauInvMax = dTau * dInvMax;
				double dTauInvMax2 = 2.0 * dTauInvMax;
				double dTauInvMax3 = 3.0 * dTauInvMax;
				for ( auto I = _vRet.size(); I--; ) {
					_vRet[I] = _tType( dA0 - dA1 * std::cos( dTauInvMax * I ) + dA2 * std::cos( dTauInvMax2 * I ) - dA3 * std::cos( dTauInvMax3 * I ) );
				}

				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Creates a FLat-Top window.
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
			try {
				_vRet.resize( _sN );

				const double dTau = 2.0 * std::numbers::pi;
				size_t stMax = _vRet.size() - 1;
				double dInvMax = 1.0 / stMax;
				double dTauInvMax = dTau * dInvMax;
				double dTauInvMax2 = 2.0 * dTauInvMax;
				double dTauInvMax3 = 3.0 * dTauInvMax;
				double dTauInvMax4 = 4.0 * dTauInvMax;
				for ( auto I = _vRet.size(); I--; ) {
					_vRet[I] = _tType( dA0 - dA1 * std::cos( dTauInvMax * I ) + dA2 * std::cos( dTauInvMax2 * I ) - dA3 * std::cos( dTauInvMax3 * I ) + dA4 * std::cos( dTauInvMax4 * I ) );
				}

				return true;
			}
			catch ( ... ) { return false; }
		}

		/**
		 * Standard sinc() function.
		 * 
		 * \param _dX	The operand.
		 * \return		Returns sin(x*PI) / x*PI.
		 **/
		static inline double			Sinc( double _dX ) {
			_dX *= std::numbers::pi;
			if ( _dX < 0.01 && _dX > -0.01 ) {
				return 1.0 + _dX * _dX * (-1.0 / 6.0 + _dX * _dX * 1.0 / 120.0);
			}

			return std::sin( _dX ) / _dX;
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

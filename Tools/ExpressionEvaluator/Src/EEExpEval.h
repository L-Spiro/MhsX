#pragma once

#include <cinttypes>
#include <string>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>				// For QueryPerformanceCounter() and QueryPerformanceFrequency().
#endif

#ifndef EE_MIN_
#define EE_MIN_( A, B )				(((A) < (B)) ? (A) : (B))
#endif	// EE_MIN_

#ifndef EE_MAX_
#define EE_MAX_( A, B )				(((A) > (B)) ? (A) : (B))
#endif	// EE_MAX_

#ifndef EE_COUNT_OF
#define EE_COUNT_OF( x )			((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
#endif	// EE_COUNT_OF

#ifndef EE_MAX_ITERATION_COUNT
#ifndef NDEBUG
#define EE_MAX_ITERATION_COUNT		90000
#else
#define EE_MAX_ITERATION_COUNT		9000000
#endif	// NDEBUG
#endif	// EE_MAX_ITERATION_COUNT


namespace ee {

	// Gets the time of initialization.
	uint64_t						StartTime();

	static inline void				InitializeExpressionEvaluatorLibrary() {
		StartTime();
	}

	// Is the given character valid hex character?
	static inline bool				ValidHex( char _cValue ) {
		return ((_cValue >= '0' && _cValue <= '9') ||
			(_cValue >= 'a' && _cValue <= 'f') ||
			(_cValue >= 'A' && _cValue <= 'F'));
	}

	// Is the given string valid hex?
	static inline bool				ValidHex( const char * _pcValue ) {
		while ( (*_pcValue++) ) {
			if ( ((*_pcValue) == ' ' || (*_pcValue) == '\t') ||
				ValidHex( (*_pcValue) ) ) { continue; }
			return false;
		}
		return true;
	}

	// Is the given character valid octal character?
	static inline bool				ValidOctal( char _cValue ) {
		return (_cValue >= '0' && _cValue <= '7');
	}

	// Converts a hex character to an integer.
	static inline uint32_t			HexToUint32( char _cVal ) {
		if ( _cVal >= '0' && _cVal <= '9' ) { return _cVal - '0'; }
		if ( _cVal >= 'A' && _cVal <= 'F' ) { return _cVal - 'A' + 10; }
		if ( _cVal >= 'a' && _cVal <= 'f' ) { return _cVal - 'a' + 10; }
		return 0;
	}

	// Converts an octal character to an integer.
	static inline uint32_t			OctalToUint32( char _cVal ) {
		if ( _cVal >= '0' && _cVal <= '7' ) { return _cVal - '0'; }
		return 0;
	}

	// Decodes a single escape sequence.
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

	// Converts a string from code form into raw values (decodes escape sequences).
	static inline std::string		DecodeEscapes( const char * _pcValue, size_t _sLen ) {
		std::string sRet;
		size_t ui32Size;
		for ( size_t I = 0; I < _sLen; I += ui32Size ) {
			sRet.push_back( static_cast<char>(DecodeEscape( &_pcValue[I], _sLen - I, ui32Size )) );
		}
		return sRet;
	}

	// Converts a string from code form into raw values (decodes escape sequences).
	static inline std::string		DecodeEscapes( const std::string &_sInput ) {
		return DecodeEscapes( _sInput.c_str(), _sInput.size() );
	}

	// Removes quotes from the start and end of a string.
	static inline std::string		RemoveQuotes( const char * _pcValue, size_t _sLen ) {
		if ( _sLen <= 2 ) { return std::string(); }
		std::string sRet = ++_pcValue;
		sRet.pop_back();
		return sRet;
	}

	// Counts the number of sequential hex characters in a string starting at the given character.
	static inline size_t			CountHexChars( const char * _pcValue, size_t _sLen, size_t _sCntFromPos ) {
		size_t sRet = 0;
		for ( ; _sCntFromPos < _sLen; ++_sCntFromPos ) {
			if ( ValidHex( _pcValue[_sCntFromPos] ) ) { ++sRet; }
			else { break; }
		}
		return sRet;
	}

	// Counts the number of sequential octal characters in a string starting at the given character.
	static inline size_t			CountOctalChars( const char * _pcValue, size_t _sLen, size_t _sCntFromPos ) {
		size_t sRet = 0;
		for ( ; _sCntFromPos < _sLen; ++_sCntFromPos ) {
			if ( ValidOctal( _pcValue[_sCntFromPos] ) ) { ++sRet; }
			else { break; }
		}
		return sRet;
	}

	// Counts the number of sequential capital and lower-case alphanumeric characters in a string starting at the given character.
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

	// Convert a \x**** character to a uint32_t.
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

	// Convert a \*** octal character to a uint32_t.
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

	// Convert a \u**** Unicode character to a uint32_t.  Does not handle surrogate pairs.
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

	// Converts 1 or 2 \u**** Unicode character(s) to a uint32_t.
	static inline uint32_t			EscapeUnicodeWide4WithSurrogatePairs( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed ) {
		uint32_t uiLeft = EscapeUnicodeWide4( _pcValue, _sLen, _sCharsConsumed );
		if ( !_sCharsConsumed ) { return uiLeft; }

		uint32_t ui32Top = uiLeft & 0xFC00UL;
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
	uint32_t						EspaceNamedUnicode( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed );

	// Converts an &#nnnn; or an &#xhhhh; HTML character to a uint32_t.
	uint32_t						EscapeHtml( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed );

	// String to integer, from any base.  Since std::stoull() raises exceptions etc.
	uint64_t						StoULL( const char * _pcText, int _iBase = 10, size_t * _psEaten = nullptr, uint64_t _uiMax = 0xFFFFFFFFFFFFFFFFULL );

	// String to double.  Unlike std::atof(), this returns the number of characters eaten.
	double							AtoF( const char * _pcText, size_t * _psEaten = nullptr );

	// Min.
	template <typename _tT>
	static inline _tT				Min( const _tT &_tA, const _tT &_tB ) { return _tA < _tB ? _tA : _tB; }

	// Max.
	template <typename _tT>
	static inline _tT				Max( const _tT &_tA, const _tT &_tB ) { return _tA > _tB ? _tA : _tB; }

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
	uint64_t						TimerFrequency();

	// Converts ticks to microseconds.
	static inline uint64_t			TicksToMicroseconds( uint64_t _ui64Ticks ) {
		return (_ui64Ticks * 1000000ULL) / TimerFrequency();
	}

}	// namespace ee

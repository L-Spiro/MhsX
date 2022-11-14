#pragma once

#include <cinttypes>
#include <string>
#include <vector>
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

#ifndef EE_UTF_INVALID
#define EE_UTF_INVALID				~static_cast<uint32_t>(0)
#endif	// EE_UTF_INVALID

#ifndef EE_INVALID_IDX
#define EE_INVALID_IDX				static_cast<size_t>(-1)
#endif	// EE_INVALID_IDX

#ifndef EE_MAX_ITERATION_COUNT
#ifndef NDEBUG
#define EE_MAX_ITERATION_COUNT		90000
#else
#define EE_MAX_ITERATION_COUNT		9000000
#endif	// NDEBUG
#endif	// EE_MAX_ITERATION_COUNT


namespace ee {

	// == Enumerations.
	// String numeric classifications.
	enum EE_STRING_NUMBER_CLASS {
		EE_SNC_UNSIGNED,
		EE_SNC_SIGNED,
		EE_SNC_FLOAT,
		EE_SNC_INVALID
	};

	// == Functions.
	// Gets the time of initialization.
	uint64_t						StartTime();

	// Called at start-up to initialize the Expression Evaluator Library.
	static inline void				InitializeExpressionEvaluatorLibrary() {
		StartTime();
	}

	// Min.
	template <typename _tT>
	static inline _tT				Min( const _tT &_tA, const _tT &_tB ) { return _tA < _tB ? _tA : _tB; }

	// Max.
	template <typename _tT>
	static inline _tT				Max( const _tT &_tA, const _tT &_tB ) { return _tA > _tB ? _tA : _tB; }

	// Is the given character valid hex character?
	static inline bool				ValidBin( char _cValue ) {
		return (_cValue >= '0' && _cValue <= '1');
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

	// Checks for whitespace without throwing exceptions.
	static inline bool				IsWhiteSpace( char _cVal ) {
		return _cVal == ' ' ||
			_cVal == '\r' ||
			_cVal == '\n' ||
			_cVal == '\t' ||
			_cVal == '\f' ||
			_cVal == '\v';
	}

	// Checks for digits (0-9) without throwing exceptions.
	static inline bool				IsDigit( char _cVal ) {
		return _cVal >= '0' && _cVal <= '9';
	}

	// Checks for alpha characters (a-z, A-Z) without throwing exceptions.
	static inline bool				IsAlpha( char _cVal ) {
		return (_cVal >= 'A' && _cVal <= 'Z') ||
			(_cVal >= 'a' && _cVal <= 'z');
	}

	// Determines if a character is an identifier character.  Sets _bIsFirst to false internally.
	static inline bool				IsIdentifier( char _cVal, bool &_bIsFirst ) {
		if ( _bIsFirst ) {
			_bIsFirst = false;
			return IsAlpha( _cVal ) || _cVal == '_';
		}
		return IsAlpha( _cVal ) || IsDigit( _cVal ) || _cVal == '_';
	}

	// Determines if the given string is an identifier.
	template <typename _tStringType = std::string>
	static bool						IsIdentifier( const _tStringType &_sIdent ) {
		if ( !_sIdent.size() ) { return false; }
		bool bStart = true;
		for ( size_t I = 0; I < _sIdent.size(); ++I ) {
			if ( !IsIdentifier( static_cast<char>(_sIdent[I]), bStart ) ) { return false; }
		}
		return true;
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

	// Remove a character from a string.
	template <typename _tStringType = std::string>
	static inline _tStringType &	RemoveChar( _tStringType &_sInput, uint32_t _ui32RemoveMe ) {
		for ( auto I = _sInput.size(); I--; ) {
			if ( _sInput[I] == static_cast<_tStringType::value_type>(_ui32RemoveMe) ) {
				_sInput.erase( I, 1 );
			}
		}
		return _sInput;
	}

	// Trims whitespace from the start and end of a given string.
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
	uint32_t						EscapeNamedUnicode( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed );

	// Converts an &#nnnn; or an &#xhhhh; HTML character to a uint64_t.
	uint64_t						EscapeHtml( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed );

	// Escapes double quotes in a string (" -> \").
	template <typename _tType = std::string>
	static _tType					EscapeQuotes( const _tType &_sInput ) {
		_tType sRet;
		for ( size_t I = 0; I < _sInput.size(); ++I ) {
			if ( _sInput[I] == _tType::value_type( '\"' ) ) {
				sRet.push_back( _tType::value_type( '\\' ) );
			}
			sRet.push_back( _sInput[I] );
		}
		return sRet;
	}

	// Gets the next UTF-32 character from a stream or error (EE_UTF_INVALID)
	inline uint32_t					NextUtf32Char( const uint32_t * _puiString, size_t _sLen, size_t * _psSize = nullptr ) {
		if ( _sLen == 0 ) {
			if ( _psSize ) { (*_psSize) = 0; }
			return 0;
		}
		if ( _psSize ) { (*_psSize) = 1; }
		uint32_t ui32Ret = (*_puiString);
		if ( ui32Ret & 0xFFE00000 ) { return EE_UTF_INVALID; }
		return ui32Ret;
	}

	// Gets the next UTF-16 character from a stream or error (EE_UTF_INVALID)
	uint32_t						NextUtf16Char( const wchar_t * _pwcString, size_t _sLen, size_t * _psSize = nullptr );

	// Gets the next UTF-8 character from a stream or error (EE_UTF_INVALID)
	uint32_t						NextUtf8Char( const char * _pcString, size_t _sLen, size_t * _psSize = nullptr );

	// Gets the size of the given UTF-8 character.
	size_t							Utf8CharSize( const char * _pcString, size_t _sLen );

	// Converts a UTF-32 character to a UTF-16 character.
	uint32_t						Utf32ToUtf16( uint32_t _ui32Utf32, uint32_t &_ui32Len );

	// Converts a UTF-32 character to a UTF-8 character.
	uint32_t						Utf32ToUtf8( uint32_t _ui32Utf32, uint32_t &_ui32Len );

	// Converts a wstring to a UTF-8 string.
	std::string						WStringToString( const std::wstring &_wsIn );

	// Converts a UTF-8 string to wstring (UTF-16).
	std::wstring					StringToWString( const std::string &_sIn );

	// Converts a UTF-8 string to wstring (UTF-16).
	std::wstring					StringToWString( const char * _pcIn, size_t _sLen );

	// Converts a UTF-32 string to a UTF-16 string.
	std::wstring					Utf32StringToWString( const uint32_t * _puiUtf32String, size_t _sLen );

	// Gets the Nth Unicode code point in the given string.
	uint32_t						GetUtf8CodePointByIdx( const std::string &_sIn, size_t _sIx );

	// Gets the Nth Unicode code point in the given string.
	size_t							GetUtf8CharPosByIdx( const std::string &_sIn, size_t _sIx );

	// Converts a wstring to a UTF-8 string.  The main difference between this and WStringToString() is that this will copy the raw characters over on error
	//	compared to WStringToString(), which will output an error character (EE_UTF_INVALID).
	std::string						ToUtf8( const std::wstring &_wsString );

	// Converts a u16string to a UTF-8 string.  The main difference between this and WStringToString() is that this will copy the raw characters over on error
	//	compared to WStringToString(), which will output an error character (EE_UTF_INVALID).
	std::string						ToUtf8( const std::u16string &_u16String );

	// Converts a u32string to a UTF-8 string.
	std::string						ToUtf8( const std::u32string &_u32String );

	// Converts from UTF-8 to UTF-16, copying the original characters instead of EE_UTF_INVALID as StringToWString() would.
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

	// Converts from UTF-32 to UTF-16, copying the original characters instead of EE_UTF_INVALID.
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

	// Converts from UTF-8 to UTF-32, copying the original characters instead of EE_UTF_INVALID.
	std::u32string					ToUtf32( const std::string &_sIn );

	// Converts from UTF-8 to ASCII.
	std::string						ToAscii( const std::string &_sIn );

	// Represents a value in binary notation.
	std::string						ToBinary( uint64_t _ui64Val );

	// Represents a value in binary notation.
	std::string						ToBinary( int64_t _i64Val );

	// Represents a value in binary notation.
	std::string						ToBinary( double _d4Val );

	// Represents a value in hexadecimal notation.
	std::string						ToHex( uint64_t _ui64Val );

	// Represents a value in hexadecimal notation.
	std::string						ToHex( int64_t _i64Val );

	// Represents a value in hexadecimal notation.
	std::string						ToHex( double _d4Val );

	// Represents a value in octadecimal notation.
	std::string						ToOct( uint64_t _ui64Val );

	// Represents a value in octadecimal notation.
	std::string						ToOct( int64_t _i64Val );

	// Represents a value in octadecimal notation.
	std::string						ToOct( double _d4Val );

	// Classifies a string as one of the EE_NUM_CONSTANTS types, IE whether the string is a valid float-point or a signed or unsigned integer.  Or EE_NC_INVALID.
	EE_STRING_NUMBER_CLASS			ClassifyString( const std::string &_sIn, uint8_t * _pui8SpecialBase = nullptr );

	// Counts the number of bytes (not bits) set in the given 64-bit value.
	static inline uint32_t			CountSetBytes( uint64_t _ui64Value ) {
		uint32_t uiRet = 0;
		while ( _ui64Value ) {
			++uiRet;
			_ui64Value >>= 8;
		}
		return uiRet;
	}

	// Finds the idex of the highest-set bit in the given value.
	static inline uint32_t			CountSetBits( uint64_t _ui64Value ) {
		uint32_t ui32Cnt = 0;
		while ( _ui64Value ) {
			_ui64Value &= (_ui64Value - 1);
			ui32Cnt++;
		}
		return ui32Cnt;
	}

	// Finds the highest-set bit in a given value.
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
	
	// Resolves escape sequences.  Returns the full string as a 32-bit character array.
	// \'	single quote	byte 0x27 in ASCII encoding
	// \"	double quote	byte 0x22 in ASCII encoding
	// \?	question mark	byte 0x3f in ASCII encoding
	// \\	backslash	byte 0x5c in ASCII encoding
	// \a	audible bell	byte 0x07 in ASCII encoding
	// \b	backspace	byte 0x08 in ASCII encoding
	// \f	form feed - new page	byte 0x0c in ASCII encoding
	// \n	line feed - new line	byte 0x0a in ASCII encoding
	// \r	carriage return	byte 0x0d in ASCII encoding
	// \t	horizontal tab	byte 0x09 in ASCII encoding
	// \v	vertical tab	byte 0x0b in ASCII encoding
	// \nnn	arbitrary octal value	byte nnn
	// \xnn	arbitrary hexadecimal value	byte nn
	// \unnnn universal character name (arbitrary Unicode value); may result in several characters	code point U+nnnn
	// \Unnnnnnnn universal character name (arbitrary Unicode value); may result in several characters	code point U+nnnnnnnn
	// \N{name} Character named NAME in the Unicode database
	void							ResolveAllEscapes( const std::string &_sInput, std::vector<uint32_t> &_vOutput );

	// Resolves escape sequences.
	// \'	single quote	byte 0x27 in ASCII encoding
	// \"	double quote	byte 0x22 in ASCII encoding
	// \?	question mark	byte 0x3f in ASCII encoding
	// \\	backslash	byte 0x5c in ASCII encoding
	// \a	audible bell	byte 0x07 in ASCII encoding
	// \b	backspace	byte 0x08 in ASCII encoding
	// \f	form feed - new page	byte 0x0c in ASCII encoding
	// \n	line feed - new line	byte 0x0a in ASCII encoding
	// \r	carriage return	byte 0x0d in ASCII encoding
	// \t	horizontal tab	byte 0x09 in ASCII encoding
	// \v	vertical tab	byte 0x0b in ASCII encoding
	// \nnn	arbitrary octal value	byte nnn
	// \xnn	arbitrary hexadecimal value	byte nn
	// \unnnn universal character name (arbitrary Unicode value); may result in several characters	code point U+nnnn
	// \Unnnnnnnn universal character name (arbitrary Unicode value); may result in several characters	code point U+nnnnnnnn
	// \N{name} Character named NAME in the Unicode database
	void							ResolveAllEscapes( const std::string &_sInput, std::string &_sOutput, bool _bIsUtf8 );

	// Resolves a single escape character, or returns the first input character if not an escape character.
	uint64_t						ResolveEscape( const char * _pcInput, size_t _sLen, size_t &_sCharLen, bool _bIncludeHtml, bool * _pbEscapeFound = nullptr );

	// Resolves HTML/XML characters.
	// &#nnnn;
	// &#xhhhh;
	void							ResolveAllHtmlXmlEscapes( const std::string &_sInput, std::string &_sOutput, bool _bIsUtf8 );

	// Counts the number of UTF-8 code points in the given string.
	uint64_t						CountUtfCodePoints( const std::string &_sInput );

	// String to integer, from any base.  Since std::stoull() raises exceptions etc.
	uint64_t						StoULL( const char * _pcText, int _iBase = 10, size_t * _psEaten = nullptr, uint64_t _uiMax = 0xFFFFFFFFFFFFFFFFULL, bool * _pbOverflow = nullptr );

	// String to double.  Unlike std::atof(), this returns the number of characters eaten.
	double							AtoF( const char * _pcText, size_t * _psEaten = nullptr, bool * _pbError = nullptr );

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
	uint64_t						TimerFrequency();

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
	static bool						PreprocessingDirectives( const std::string &_sInput, std::string &_sDirective, std::string &_sParms ) {
		_sDirective.clear();
		_sParms.clear();
		std::string sTmp = TrimWhitespace( _sInput );
		if ( !sTmp.size() ) { return false; }
		if ( sTmp[0] == '#' ) {
			// Move to the directive.
			size_t stIdx = 1;
			while ( stIdx < sTmp.size() && IsWhiteSpace( sTmp[stIdx] ) ) { ++stIdx; }

			// Copy the directive.
			while ( stIdx < sTmp.size() && !IsWhiteSpace( sTmp[stIdx] ) ) {
				if ( !IsAlpha( sTmp[stIdx] ) ) { return false; }
				_sDirective.push_back( sTmp[stIdx++] );
			}

			// Skip to the parameters.
			while ( stIdx < sTmp.size() && IsWhiteSpace( sTmp[stIdx] ) ) { ++stIdx; }

			// Copy any parameters.
			_sParms = sTmp.substr( stIdx );
			return true;
		}
		return false;
	}

}	// namespace ee

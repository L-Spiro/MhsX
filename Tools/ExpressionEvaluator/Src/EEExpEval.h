#pragma once

#include <cinttypes>
#include <string>

namespace ee {

	// Decodes a single escape sequence.
	static wchar_t DecodeEscape( const char * _pcValue, size_t _ui32Len, size_t &_ui32Size ) {
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

		if ( _ui32Len == 0 ) {
			_ui32Size = 0;
			return static_cast<wchar_t>(0);
		}
		// If the length is 1 or the next character is not a back slash, the next character is the return.
		_ui32Size = 1;
		if ( _ui32Len == 1 || _pcValue[0] != static_cast<wchar_t>('\\') ) {
			return _pcValue[0];
		}
		// Possible sequence here.
		switch ( _pcValue[1] ) {
			case 'x' : {
				// Hex sequence.  Decode.
				if ( _ui32Len == 2 ) {
					return _pcValue[0];
				}
				wchar_t cReturn = 0;
				wchar_t cThis;
				// Decode the hex characters (maximum of 4).
				for ( size_t I = 2; I < _ui32Len && I < 6; I++ ) {
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
						++_ui32Size;
						return cReturn;
					}
					cReturn <<= 4;
					cReturn |= cThis;
					++_ui32Size;
				}
				// Eat the x.
				++_ui32Size;
				return cReturn;
			}
			default : {
				if ( _pcValue[1] >= '0' && _pcValue[1] <= '7' ) {
					// Decode octal sequences.
					wchar_t cReturn = 0;
					wchar_t cThis;
					for ( size_t I = 1; I < _ui32Len && I < 4; I++ ) {
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
						++_ui32Size;
					}
					return cReturn;
				}
				else {
					for ( size_t J = 0; sEscapes[J].cEscape != ' '; J++ ) {
						if ( _pcValue[1] == sEscapes[J].cEscape ) {
							++_ui32Size;
							return sEscapes[J].cValue;
						}
					}
				}
			}
		}
		return _pcValue[0];
	}

	// Converts a string from code form into raw values (decodes escape sequences).
	static std::string DecodeEscapes( const char * _pcValue, size_t _ui32Len ) {
		std::string sRet;
		size_t ui32Size;
		for ( size_t I = 0; I < _ui32Len; I += ui32Size ) {
			sRet.push_back( static_cast<char>(DecodeEscape( &_pcValue[I], _ui32Len - I, ui32Size )) );
		}
		return sRet;
	}

	// Converts a string from code form into raw values (decodes escape sequences).
	static std::string DecodeEscapes( const std::string &_sInput ) {
		return DecodeEscapes( _sInput.c_str(), _sInput.size() );
	}

	// Removes quotes from the start and end of a string.
	static std::string RemoveQuotes( const char * _pcValue, size_t _ui32Len ) {
		if ( _ui32Len <= 2 ) { return std::string(); }
		std::string sRet = ++_pcValue;
		sRet.pop_back();
		return sRet;
	}

	// Is the given string valid hex?
	static bool ValidHex( const char * _pcValue ) {
		while ( (*_pcValue ) ) {
			if ( ((*_pcValue) == ' ' || (*_pcValue) == '\t') ||
				((*_pcValue) >= '0' && (*_pcValue) <= '9') ||
				((*_pcValue) >= 'a' && (*_pcValue) <= 'f') ||
				((*_pcValue) >= 'A' && (*_pcValue) <= 'F') ) { continue; }
			return false;
		}
		return true;
	}

}	// namespace ee

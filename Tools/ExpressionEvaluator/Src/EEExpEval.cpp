#include "EEExpEval.h"
#include "Unicode/EEUnicode.h"

namespace ee {

	// Gets the time of initialization.
	uint64_t StartTime() {
		static uint64_t uiStart = 0;
		if ( !uiStart ) {
#ifdef _WIN32
			::QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER *>(&uiStart) );
#else
#error "No implementation for StartTime()."
#endif	// #ifdef _WIN32
		}
		return uiStart;
	}

	// Converts a \N{*} named Unicode character to a uint32_t.
	uint32_t EspaceNamedUnicode( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed ) {
		const char * pcOrig = _pcValue;
		_sCharsConsumed = 0;
		const char * pcStart = nullptr;
		size_t sLen = 0;
		for ( size_t I = 0; I < _sLen; ++I ) {
			if ( _pcValue[I] == '{' ) { pcStart = &_pcValue[I+1]; }
			else if ( _pcValue[I] == '}' ) {
				sLen = &_pcValue[I] - pcStart;
				break;
			}
		}
		if ( pcStart && sLen ) {
			uint32_t ui32Code = CUnicode::GetCode( pcStart, sLen );
			if ( CUnicode::EE_INVALID != ui32Code ) {
				// We ate however many characters from the start plus the characters in the actual name plus the final }.
				_sCharsConsumed = sLen + (pcStart - pcOrig) + 1;
				return ui32Code;
			}
		}
		return 0;
	}

	// Converts an &#nnnn; or an &#xhhhh; HTML character to a uint32_t.
	uint32_t EscapeHtml( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed ) {
		// The & is assumed to have been eaten already.
		const char * pcOrig = _pcValue;
		_sCharsConsumed = 0;
		if ( _sLen && (*_pcValue) == '#' ) {
			++_pcValue;
			--_sLen;
			if ( _sLen && (*_pcValue) == 'x' ) {
				++_pcValue;
				--_sLen;
				// Go hex.
				// Eat until ;.
				uint32_t ui32Ret = 0;
				bool bFoundOne = false;
				for ( size_t I = 0; I < _sLen; ++I ) {
					char cTemp = _pcValue[I];
					if ( !((cTemp >= 'a' && cTemp <= 'f') ||
						(cTemp >= 'A' && cTemp <= 'F') ||
						(cTemp >= '0' && cTemp <= '9') ) ) {
						if ( !bFoundOne ) { return 0; }
						// This is the only point of success.
						_sCharsConsumed = _pcValue - pcOrig + I;
						if ( cTemp == ';' ) { ++_sCharsConsumed; }
						return ui32Ret;
					}
					ui32Ret <<= 4;
					ui32Ret |= HexToUint32( cTemp );
					bFoundOne = true;
				}
			}
			uint32_t ui32Ret = 0;
			bool bFoundOne = false;
			for ( size_t I = 0; I < _sLen; ++I ) {
				char cTemp = _pcValue[I];
				if ( !(cTemp >= '0' && cTemp <= '9') ) {
					if ( !bFoundOne ) { return 0; }
					// This is the only point of success.
					_sCharsConsumed = _pcValue - pcOrig + I;
					if ( cTemp == ';' ) { ++_sCharsConsumed; }
					return ui32Ret;
				}
				ui32Ret *= 10;
				ui32Ret += cTemp - '0';
				bFoundOne = true;
			}
		}
		return 0;
	}

	// String to integer, from any base.  Since std::stoull() raises exceptions etc.
	uint64_t StoULL( const char * _pcText, int _iBase, size_t * _psEaten, uint64_t _uiMax, bool * _pbOverflow ) {
		if ( _pbOverflow ) { (*_pbOverflow) = false; }
		const char * _pcOrig = _pcText;
		// Negate?
		bool bNegate = false;
		if ( _pcText[0] == '-' ) {
			bNegate = true;
			++_pcText;
		}

		// Skip whitespace.
		while ( ::isspace( (*_pcText) ) ) {
			++_pcText;
		}

		// Skip any opening "0", "0x", etc.
		if ( _iBase == 0 || _iBase == 2 || _iBase == 8 || _iBase == 16 ) {
			
			if ( (*_pcText) == '0' ) {
				++_pcText;
				
				if ( (*_pcText) == 'b' || (*_pcText) == 'B' ) {
					if ( _iBase == 0 || _iBase == 2 ) {
						++_pcText;
						_iBase = 2;
					}
				}
				else if ( (*_pcText) == 'x' || (*_pcText) == 'X' ) {
					++_pcText;
					if ( _iBase == 0 ) {
						_iBase = 16;
					}
				}
				else if ( _iBase == 0 ) {
					_iBase = 8;
				}
			}
			
		}
		if ( _iBase == 0 ) {
			_iBase = 10;
		}
		uint64_t uiRes = 0;
		for ( ; (*_pcText); ++_pcText ) {
			uint8_t uiNext = static_cast<uint8_t>((*_pcText));
			if ( uiNext >= '0' && uiNext <= '9' ) {
				uiNext -= '0';
			}
			else {
				if ( uiNext >= 'A' && uiNext <= 'Z' ) {
					uiNext -= 'A';
				}
				else {
					uiNext -= 'a';
				}
				uiNext += 10;
			}
			if ( uiNext >= _iBase ) {
				break;
			}
			

			
			uint64_t uiTemp = uiRes;
			uint64_t uiPreview = uiRes * _iBase + uiNext;
			// Bounds checking.
			if ( uiPreview > _uiMax ) { break; }

			// Confirm the eating of the character.
			//if ( _psEaten ) { (*_psEaten ) = &_pcText[I] - _pcOrig + 1; }

			// Check for overflow.
			if ( uiPreview < uiTemp ) {
			//if ( (uiPreview - uiNext) / _iBase != uiTemp) {
				if ( _psEaten ) { (*_psEaten ) = _pcText - _pcOrig + 1; }
				if ( _pbOverflow ) { (*_pbOverflow) = true; }
				return std::numeric_limits<uint64_t>::max();
			}
			uiRes = uiPreview;
		}
		if ( _psEaten ) { (*_psEaten ) = _pcText - _pcOrig; }
		return bNegate ? static_cast<uint64_t>(-static_cast<int64_t>(uiRes)) : uiRes;
	}

	// String to double.  Unlike std::atof(), this returns the number of characters eaten.
	double AtoF( const char * _pcText, size_t * _psEaten ) {
		const char * _pcOrig = _pcText;
		// The number is in the following format:
		// [whitespace] [sign] [digits] [.digits] [ {e | E }[sign]digits]
		// Skip whitespace.
		while ( ::isspace( (*_pcText) ) ) { ++_pcText; }

		const char * _pcNumberBegins = _pcText;
		// Optional sign.
		if ( (*_pcText) == '-' || (*_pcText) == '+' ) {
			++_pcText;
		}
		// Digits must be next.
		if ( !::isdigit( (*_pcText) ) ) {
			if ( _psEaten ) { (*_psEaten) = _pcText - _pcOrig; }
			return 0.0;
		}
		while ( ::isdigit( (*_pcText) ) ) { ++_pcText; }

		// A decimal point and trailing digits are optional.
		if ( (*_pcText) == '.' ) {
			++_pcText;
			while ( ::isdigit( (*_pcText) ) ) { ++_pcText; }
		}
		// Exponent.
		if ( ::toupper( (*_pcText) ) == 'E' ) {
			++_pcText;
			// Optional sign.
			if ( (*_pcText) == '-' || (*_pcText) == '+' ) {
				++_pcText;
			}
			// Digits must be next.
			if ( !::isdigit( (*_pcText) ) ) {
				if ( _psEaten ) { (*_psEaten) = _pcText - _pcOrig; }
				return 0.0;
			}
			while ( ::isdigit( (*_pcText) ) ) { ++_pcText; }
		}

		if ( _psEaten ) { (*_psEaten) = _pcText - _pcOrig; }
		return ::atof( _pcNumberBegins );
	}

	// Gets the timer frequency.
	uint64_t TimerFrequency() {
		static uint64_t uiFreq = 0;
		if ( !uiFreq ) {
#ifdef _WIN32
			::QueryPerformanceFrequency( reinterpret_cast<LARGE_INTEGER *>(&uiFreq) );
#else
#error "No implementation for TimerFrequency()."
#endif	// #ifdef _WIN32
		}
		return uiFreq;
	}

}	// namespace ee

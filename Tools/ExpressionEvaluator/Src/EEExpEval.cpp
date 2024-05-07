#include "EEExpEval.h"
#include "Html/EEHtml.h"
#include "Unicode/EEUnicode.h"

#include <iomanip>
#include <sstream>

namespace ee {

// == Members.
#ifdef __GNUC__
	::mach_timebase_info_data_t CExpEval::m_mtidInfoData = { 0 };
#endif	// #ifdef __GNUC__

	// == Functions.
	// Gets the time of initialization.
	uint64_t CExpEval::StartTime() {
		static uint64_t uiStart = 0;
		if ( !uiStart ) {
#ifdef _WIN32
			::QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER *>(&uiStart) );
#else
			if ( !m_mtidInfoData.denom ) {
				if ( KERN_SUCCESS != ::mach_timebase_info( &m_mtidInfoData ) ) {
					return 0ULL;
				}
			}
			uiStart = ::mach_absolute_time() * m_mtidInfoData.numer;
#endif	// #ifdef _WIN32
		}
		return uiStart;
	}

	// Converts a \N{*} named Unicode character to a uint32_t.
	uint32_t CExpEval::EscapeNamedUnicode( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed ) {
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

	// Converts an &#nnnn; or an &#xhhhh; HTML character to a uint64_t.
	uint64_t CExpEval::EscapeHtml( const char * _pcValue, size_t _sLen, size_t &_sCharsConsumed ) {
		// The & is assumed to have been eaten already.
		const char * pcOrig = _pcValue;
		_sCharsConsumed = 0;
		if ( _sLen ) {
			if ( (*_pcValue) == '#' ) {
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

			size_t sCount = CountAlphanumeric( _pcValue, _sLen, 0 );
			//size_t sCodeLen;
			uint64_t uiCode = CHtml::GetCode( _pcValue, sCount );
			if ( uiCode != EE_UTF_INVALID ) {
				_sCharsConsumed = sCount + 1;
				_pcValue += _sCharsConsumed;
				_sLen -= _sCharsConsumed;
				if ( _sLen && (*_pcValue) == ';' ) {
					// The ';' is nestled against the name as it should be.  Eat it.  /* Not meant to be read offensively. */
					++_sCharsConsumed;
				}
				return uiCode;
			}
		}
		return 0;
	}

	// Gets the next UTF-16 character from a stream or error (EE_UTF_INVALID)
	uint32_t CExpEval::NextUtf16Char( const wchar_t * _pwcString, size_t _sLen, size_t * _psSize ) {
		if ( _sLen == 0 ) { return 0; }

		// Get the low bits (which may be all there are).
		uint32_t ui32Ret = (*reinterpret_cast<const uint16_t *>(_pwcString));

		uint32_t ui32Top = ui32Ret & 0xFC00U;
		// Check to see if this is a surrogate pair.
		if ( ui32Top == 0xD800U ) {
			if ( _sLen < 2 ) {
				// Not enough space to decode correctly.
				if ( _psSize ) {
					(*_psSize) = 1;
				}
				return EE_UTF_INVALID;
			}

			// Need to add the next character to it.
			// Remove the 0xD800.
			ui32Ret &= ~0xD800U;
			ui32Ret <<= 10;

			// Get the second set of bits.
			uint32_t ui32Next = (*reinterpret_cast<const uint16_t *>(++_pwcString));
			if ( (ui32Next & 0xFC00U) != 0xDC00U ) {
				// Invalid second character.
				// Standard defines this as an error.
				if ( _psSize ) {
					(*_psSize) = 1;
				}
				return EE_UTF_INVALID;
			}
			if ( _psSize ) {
				(*_psSize) = 2;
			}

			ui32Next &= ~0xDC00U;

			// Add the second set of bits.
			ui32Ret |= ui32Next;

			return ui32Ret + 0x10000U;
		}

		if ( _psSize ) {
			(*_psSize) = 1;
		}
		return ui32Ret;
	}

	// Gets the next UTF-8 character from a stream or error (EE_UTF_INVALID)
	uint32_t CExpEval::NextUtf8Char( const char * _pcString, size_t _sLen, size_t * _psSize ) {
		if ( _sLen == 0 ) { if ( _psSize ) { (*_psSize) = 0; } return 0; }

		// Get the low bits (which may be all there are).
		uint32_t ui32Ret = (*reinterpret_cast<const uint8_t *>(_pcString));

		// The first byte is a special case.
		if ( (ui32Ret & 0x80U) == 0 ) {
			// We are done.
			if ( _psSize ) { (*_psSize) = 1; }
			return ui32Ret;
		}

		// We are in a multi-byte sequence.  Get bits from the top, starting
		//	from the second bit.
		uint32_t I = 0x20;
		uint32_t ui32Len = 2;
		uint32_t ui32Mask = 0xC0U;
		while ( ui32Ret & I ) {
			// Add this bit to the mask to be removed later.
			ui32Mask |= I;
			I >>= 1;
			++ui32Len;
			if ( I == 0 ) {
				// Invalid sequence.
				if ( _psSize ) {
					(*_psSize) = 1;
				}
				return EE_UTF_INVALID;
			}
		}

		// Bounds checking.
		if ( ui32Len > _sLen ) {
			if ( _psSize ) { (*_psSize) = _sLen; }
			return EE_UTF_INVALID;
		}

		// We know the size now, so set it.
		// Even if we return an invalid character we want to return the correct number of
		//	bytes to skip.
		if ( _psSize ) { (*_psSize) = ui32Len; }

		// If the length is greater than 4, it is invalid.
		if ( ui32Len > 4 ) {
			// Invalid sequence.
			return EE_UTF_INVALID;
		}

		// Mask out the leading bits.
		ui32Ret &= ~ui32Mask;

		// For every trailing bit, add it to the final value.
		for ( I = ui32Len - 1; I--; ) {
			uint32_t ui32This = (*reinterpret_cast<const uint8_t *>(++_pcString));
			// Validate the byte.
			if ( (ui32This & 0xC0U) != 0x80U ) {
				// Invalid.
				return EE_UTF_INVALID;
			}

			ui32Ret <<= 6;
			ui32Ret |= (ui32This & 0x3F);
		}

		// Finally done.
		return ui32Ret;
	}

	// Gets the size of the given UTF-8 character.
	size_t CExpEval::Utf8CharSize( const char * _pcString, size_t _sLen ) {
		if ( _sLen == 0 ) { return 0; }

		// Get the low bits (which may be all there are).
		uint32_t ui32Ret = (*reinterpret_cast<const uint8_t *>(_pcString));

		// The first byte is a special case.
		if ( (ui32Ret & 0x80U) == 0 ) {
			// We are done.
			return 1;
		}

		// We are in a multi-byte sequence.  Get bits from the top, starting
		//	from the second bit.
		uint32_t I = 0x20;
		size_t sLen = 2;
		while ( ui32Ret & I ) {
			// Add this bit to the mask to be removed later.
			I >>= 1;
			++sLen;
			if ( I == 0 ) { return 1; }
		}

		// Bounds checking.
		if ( sLen > _sLen ) {
			return _sLen;
		}
		return sLen;
	}

	// Converts a UTF-32 character to a UTF-16 character.
	uint32_t CExpEval::Utf32ToUtf16( uint32_t _ui32Utf32, uint32_t &_ui32Len ) {
		if ( _ui32Utf32 > 0x10FFFF ) {
			_ui32Len = 1;
			return EE_UTF_INVALID;
		}
		if ( 0x10000 <= _ui32Utf32 ) {
			_ui32Len = 2;

			// Break into surrogate pairs.
			_ui32Utf32 -= 0x10000UL;
			uint32_t ui32Hi = (_ui32Utf32 >> 10) & 0x3FF;
			uint32_t ui32Low = _ui32Utf32 & 0x3FF;

			return (0xD800 | ui32Hi) |
				((0xDC00 | ui32Low) << 16);
		}
		_ui32Len = 1;
		return _ui32Utf32;
	}

	// Converts a UTF-32 character to a UTF-8 character.
	uint32_t CExpEval::Utf32ToUtf8( uint32_t _ui32Utf32, uint32_t &_ui32Len ) {
		// Handle the single-character case separately since it is a special case.
		if ( _ui32Utf32 < 0x80U ) {
			_ui32Len = 1;
			return _ui32Utf32;
		}

		// Upper bounds checking.
		if ( _ui32Utf32 > 0x10FFFFU ) {
			// Invalid character.  How do we handle it?
			// Return a default character.
			_ui32Len = 1;
			return EE_UTF_INVALID;
		}

		// Every other case uses bit markers.
		// Start from the lowest encoding and check upwards.
		uint32_t ui32High = 0x00000800U;
		uint32_t ui32Mask = 0xC0;
		_ui32Len = 2;
		while ( _ui32Utf32 >= ui32High ) {
			ui32High <<= 5;
			ui32Mask = (ui32Mask >> 1) | 0x80U;
			++_ui32Len;
		}

		// Encode the first byte.
		uint32_t ui32BottomMask = ~((ui32Mask >> 1) | 0xFFFFFF80U);
		uint32_t ui32Ret = ui32Mask | ((_ui32Utf32 >> ((_ui32Len - 1) * 6)) & ui32BottomMask);
		// Now fill in the rest of the bits.
		uint32_t ui32Shift = 8;
		for ( uint32_t I = _ui32Len - 1; I--; ) {
			// Shift down, mask off 6 bits, and add the 10xxxxxx flag.
			uint32_t ui32This = ((_ui32Utf32 >> (I * 6)) & 0x3F) | 0x80;

			ui32Ret |= ui32This << ui32Shift;
			ui32Shift += 8;
		}

		return ui32Ret;
	}

	// Converts a wstring to a UTF-8 string.
	std::string CExpEval::WStringToString( const std::wstring &_wsIn ) {
#if 0
		// Problematic; terminates at the first \0 character, making it impossible to use for strings meant for OPENFILENAMEW etc.
		//return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes( _wsIn );
		// Instead, convert up to the \0 and add the \0 manually and then keep converting.
		std::string ssTemp;
		std::string::size_type sStart = 0;
		for ( std::string::size_type I = 0; I < _wsIn.size(); ++I ) {
			if ( _wsIn[I] == '\0' ) {
				ssTemp.append( std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes( _wsIn.substr( sStart, I - sStart ).data() ) );
				ssTemp.push_back( '\0' );
				sStart = I + 1;
			}
		}

		if ( sStart < _wsIn.size() ) {
			ssTemp.append( std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes( _wsIn.substr( sStart ).data() ) );
		}
		return ssTemp;
#else
		std::string ssTemp;
		const wchar_t * pwcSrc = _wsIn.c_str();
		size_t I = 0;
		size_t sSize;
		do {
			uint32_t ui32This = NextUtf16Char( &pwcSrc[I], _wsIn.size() - I, &sSize );
			uint32_t ui32Len;
			uint32_t ui32Converted = Utf32ToUtf8( ui32This, ui32Len );
			for ( uint32_t J = 0; J < ui32Len; ++J ) {
				ssTemp.push_back( static_cast<char>(ui32Converted & 0xFFU) );
				ui32Converted >>= 8;
			}
			I += sSize;
		} while ( I < _wsIn.size() );
		return ssTemp;
#endif
	}

	// Converts a UTF-8 string to wstring (UTF-16).
	std::wstring CExpEval::StringToWString( const std::string &_sIn ) {
#if 0
		// Problematic; terminates at the first \0 character, making it impossible to use for strings meant for OPENFILENAMEW etc.
		//return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes( _sIn.data() );
		
		// Instead, convert up to the \0 and add the \0 manually and then keep converting.
		std::wstring wTemp;
		std::string::size_type sStart = 0;
		for ( std::string::size_type I = 0; I < _sIn.size(); ++I ) {
			if ( _sIn[I] == '\0' ) {
				wTemp.append( std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes( _sIn.substr( sStart, I - sStart ).data() ) );
				wTemp.push_back( L'\0' );
				sStart = I + 1;
			}
		}

		if ( sStart < _sIn.size() ) {
			wTemp.append( std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes( _sIn.substr( sStart ).data() ) );
		}
		return wTemp;
#else
		return StringToWString( static_cast<const char *>(_sIn.c_str()), _sIn.size() );
#endif
	}

	// Converts a UTF-8 string to wstring (UTF-16).
	std::wstring CExpEval::StringToWString( const char * _pcIn, size_t _sLen ) {
		std::wstring swsTemp;
		size_t I = 0;
		size_t sSize;
		do {
			uint32_t ui32This = NextUtf8Char( &_pcIn[I], _sLen - I, &sSize );
			uint32_t ui32Len;
			uint32_t ui32Converted = Utf32ToUtf16( ui32This, ui32Len );
			for ( uint32_t J = 0; J < ui32Len; ++J ) {
				swsTemp.push_back( static_cast<wchar_t>(ui32Converted & 0xFFFFU) );
				ui32Converted >>= 16;
			}
			I += sSize;
		} while ( I < _sLen );
		return swsTemp;
	}

	// Converts a UTF-32 string to a UTF-16 string.
	std::wstring CExpEval::Utf32StringToWString( const uint32_t * _puiUtf32String, size_t _sLen ) {
		std::wstring swsTemp;
		size_t I = 0;
		size_t sSize;
		do {
			uint32_t ui32This = NextUtf32Char( &_puiUtf32String[I], _sLen - I, &sSize );
			uint32_t ui32Len;
			uint32_t ui32Converted = Utf32ToUtf16( ui32This, ui32Len );
			for ( uint32_t J = 0; J < ui32Len; ++J ) {
				swsTemp.push_back( static_cast<wchar_t>(ui32Converted & 0xFFFFU) );
				ui32Converted >>= 16;
			}
			I += sSize;
		} while ( I < _sLen );
		return swsTemp;
	}

	// Gets the Nth Unicode code point in the given string.
	uint32_t CExpEval::GetUtf8CodePointByIdx( const std::string &_sIn, size_t _sIx ) {
		uint32_t ui32Char = 0;
		++_sIx;
		size_t sThisSize = 0;
		for ( size_t I = 0; I < _sIn.size() && _sIx; --_sIx, I += sThisSize ) {
			ui32Char = NextUtf8Char( &_sIn[I], _sIn.size() - I, &sThisSize );
		}
		if ( _sIx != 0 ) {
			return EE_UTF_INVALID;
		}
		return ui32Char;
	}

	// Gets the Nth Unicode code point in the given string.
	size_t CExpEval::GetUtf8CharPosByIdx( const std::string &_sIn, size_t _sIx ) {
		size_t I = 0;
		size_t sThisSize = 0;
		//uint32_t ui32Char = 0;
		for ( I = 0; I < _sIn.size() && _sIx; --_sIx, I += sThisSize ) {
			sThisSize = Utf8CharSize( &_sIn[I], _sIn.size() - I );
			if ( !sThisSize ) { return I; }
		}
		return I;
	}

	// Converts a wstring to a UTF-8 string.  The main difference between this and WStringToString() is that this will copy the raw characters over on error
	//	compared to WStringToString(), which will output an error character (EE_UTF_INVALID).
	std::string CExpEval::ToUtf8( const std::wstring &_wsString ) {
		std::string sRet;
		const wchar_t * pwcInput = _wsString.c_str();
		size_t sSize = _wsString.size();

		for ( size_t I = 0; I < sSize; ) {
			size_t sThisSize = 0;
			uint32_t ui32Char = NextUtf16Char( &pwcInput[I], sSize - I, &sThisSize );
			if ( ui32Char == EE_UTF_INVALID ) {
				ui32Char = pwcInput[I];
			}
			I += sThisSize;
			uint32_t ui32Len = 0;
			uint32_t ui32BackToUtf8 = Utf32ToUtf8( ui32Char, ui32Len );
			for ( uint32_t J = 0; J < ui32Len; ++J ) {
				sRet.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
				ui32BackToUtf8 >>= 8;
			}
		}
		return sRet;
	}

	// Converts a u16string to a UTF-8 string.  The main difference between this and WStringToString() is that this will copy the raw characters over on error
	//	compared to WStringToString(), which will output an error character (EE_UTF_INVALID).
	std::string CExpEval::ToUtf8( const std::u16string &_u16String ) {
		std::string sRet;
		const char16_t * pc16Input = _u16String.c_str();
		size_t sSize = _u16String.size();

		for ( size_t I = 0; I < sSize; ) {
			size_t sThisSize = 0;
			uint32_t ui32Char = NextUtf16Char( reinterpret_cast<const wchar_t *>(&pc16Input[I]), sSize - I, &sThisSize );
			if ( ui32Char == EE_UTF_INVALID ) {
				ui32Char = pc16Input[I];
			}
			I += sThisSize;
			uint32_t ui32Len = 0;
			uint32_t ui32BackToUtf8 = Utf32ToUtf8( ui32Char, ui32Len );
			for ( uint32_t J = 0; J < ui32Len; ++J ) {
				sRet.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
				ui32BackToUtf8 >>= 8;
			}
		}
		return sRet;
	}

	// Converts a u32string to a UTF-8 string.
	std::string CExpEval::ToUtf8( const std::u32string &_u32String ) {
		std::string sRet;
		const char32_t * pc32Input = _u32String.c_str();
		size_t sSize = _u32String.size();

		for ( size_t I = 0; I < sSize; ) {
			size_t sThisSize = 0;
			uint32_t ui32Char = NextUtf32Char( reinterpret_cast<const uint32_t *>(&pc32Input[I]), sSize - I, &sThisSize );
			//if ( ui32Char == EE_UTF_INVALID ) { ui32Char = pc32Input[I]; }
			if ( ui32Char == EE_UTF_INVALID ) {
				for ( size_t J = 0; J < sThisSize * sizeof( std::u32string::value_type ); ++J ) {
					sRet.push_back( reinterpret_cast<const std::string::value_type *>(&_u32String.data()[I])[J] );
				}
				I += sThisSize;
				continue;
			}
			I += sThisSize;
			uint32_t ui32Len = 0;
			uint32_t ui32BackToUtf8 = Utf32ToUtf8( ui32Char, ui32Len );
			for ( uint32_t J = 0; J < ui32Len; ++J ) {
				sRet.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
				ui32BackToUtf8 >>= 8;
			}
		}
		return sRet;
	}

	// Converts from UTF-8 to UTF-32, copying the original characters instead of EE_UTF_INVALID.
	std::u32string CExpEval::ToUtf32( const std::string &_sIn ) {
		std::u32string u32Return;
		const char * pcSrc = _sIn.c_str();
		size_t sSize = _sIn.size();

		for ( size_t I = 0; I < sSize; ) {
			size_t sThisSize = 0;
			uint32_t ui32This = NextUtf8Char( &pcSrc[I], _sIn.size() - I, &sThisSize );
			if ( ui32This == EE_UTF_INVALID ) {
				for ( size_t J = 0; J < sThisSize; ++J ) {
					u32Return.push_back( static_cast<uint8_t>(pcSrc[I+J]) );
				}
				I += sThisSize;
				continue;
			}
			I += sThisSize;
			u32Return.push_back( ui32This );
		}
		return u32Return;
	}

	// Converts from UTF-8 to ASCII.
	std::string CExpEval::ToAscii( const std::string &_sIn ) {
		size_t sSize = _sIn.size();
		std::string sRet;
		
		for ( size_t I = 0; I < sSize; ) {
			size_t sThisSize = 0;
			uint32_t ui32This = NextUtf8Char( &_sIn[I], sSize - I, &sThisSize );
			if ( ui32This != EE_UTF_INVALID ) {
				uint32_t ui32Len = 0;
				uint32_t ui32BackToUtf8 = Utf32ToUtf8( ui32This, ui32Len );
				if ( ui32Len == 1 ) {
					struct {
						// The actual character to find.
						char				cEscape;
						// The character to write to return.
						char				cValue;
					} sEscapes[] = {
						{ 'n', '\n' },
						{ 'r', '\r' },
						{ 't', '\t' },
					};
					bool bFound = false;
					for ( auto J = EE_COUNT_OF( sEscapes ); J--; ) {
						if ( ui32BackToUtf8 == static_cast<uint8_t>(sEscapes[J].cValue) ) {
							sRet.push_back( '\\' );
							sRet.push_back( sEscapes[J].cEscape );
							bFound = true;
							break;
						}
					}
					if ( bFound ) {
						I += sThisSize;
						continue;
					}
					// Check if printable.
					if ( (static_cast<int8_t>(ui32This) >= -1 || ui32This < 256) && ::isprint( ui32This ) ) {
						sRet.push_back( static_cast<std::string::value_type>(ui32This) );
						I += sThisSize;
						continue;
					}
					// Not printable.
					std::stringstream ssStream;
					ssStream << "\\x" <<
						std::setfill( '0' ) << std::setw( 2 ) <<
						std::hex << ui32This;
					sRet.append( ssStream.str() );
					
					I += sThisSize;
					continue;
				}
				// ui32Len is greater than 1, so at the very least a \u is needed.
				ui32BackToUtf8 = Utf32ToUtf16( ui32This, ui32Len );
				if ( ui32Len == 1 ) {
					// If only the bottom bits are set, \x can be used instead.
					if ( ui32BackToUtf8 & ~0xFF ) {
						// Have to use the longer \u form.
						std::stringstream ssStream;
						ssStream << "\\u" <<
							std::setfill( '0' ) << std::setw( 4 ) <<
							std::hex << ui32This;
						sRet.append( ssStream.str() );
					}
					else {
						std::stringstream ssStream;
						ssStream << "\\x" <<
							std::setfill( '0' ) << std::setw( 2 ) <<
							std::hex << ui32This;
						sRet.append( ssStream.str() );
					}

					
				}
				else {
					// There was a validity check above for (ui32This != EE_UTF_INVALID) so we know that
					//	ui32This did decode into a valid value.  \U should work with no further checking.
					std::stringstream ssStream;
					ssStream << "\\U" <<
						std::setfill( '0' ) << std::setw( 8 ) <<
						std::hex << ui32This;
					sRet.append( ssStream.str() );
				}
				I += sThisSize;
				continue;
			}
			// It was invalid.
			for ( size_t J = 0; J < sThisSize; ++J ) {
				std::stringstream ssStream;
				ssStream << "\\x" <<
					std::setfill( '0' ) << std::setw( 2 ) <<
					std::hex << _sIn[I++];
				sRet.append( ssStream.str() );
			}
		}
		return sRet;
	}

	// Represents a value in binary notation.
	std::string CExpEval::ToBinary( uint64_t _ui64Val ) {
		std::string sTmp;
		sTmp.push_back( '0' );
		sTmp.push_back( 'b' );
		uint32_t ui32Bits = HighestSetBit( _ui64Val ) + 1U;
 		for ( uint64_t I = 0ULL; I < ui32Bits; ++I ) {
			sTmp.push_back( (_ui64Val & (1LL << (ui32Bits - I - 1))) ? '1' : '0' );
		}
		return sTmp;
	}

	// Represents a value in binary notation.
	std::string CExpEval::ToBinary( int64_t _i64Val ) {
		std::string sTmp;
		bool bNeg = _i64Val < 0;
		if ( bNeg ) {
			sTmp.push_back( '-' );
			_i64Val = -_i64Val;
		}
		sTmp.push_back( '0' );
		sTmp.push_back( 'b' );
		uint32_t ui32Bits = HighestSetBit( _i64Val ) + 1U;
		for ( uint64_t I = 0ULL; I < ui32Bits; ++I ) {
			sTmp.push_back( (_i64Val & (1LL << (ui32Bits - I - 1))) ? '1' : '0' );
		}
		return sTmp;
	}

	// Represents a value in binary notation.
	std::string CExpEval::ToBinary( double _d4Val ) {
		std::string sTmp;
		int64_t i64Val = (*reinterpret_cast<int64_t *>(&_d4Val));
		bool bNeg = i64Val < 0;
		if ( bNeg ) {
			sTmp.push_back( '-' );
			i64Val = -i64Val;
		}
		sTmp.push_back( '0' );
		sTmp.push_back( 'b' );
		uint32_t ui32Bits = HighestSetBit( i64Val ) + 1U;
		for ( uint64_t I = 0ULL; I < ui32Bits; ++I ) {
			sTmp.push_back( (i64Val & (1LL << (ui32Bits - I - 1))) ? '1' : '0' );
		}
		return sTmp;
	}

	// Represents a value in hexadecimal notation.
	std::string CExpEval::ToHex( uint64_t _ui64Val ) {
		std::string sTmp;
		std::stringstream ssStream;
		ssStream << "0x" <<
			std::hex <<
			std::uppercase <<
			_ui64Val;
		sTmp = ssStream.str();
		return sTmp;
	}

	// Represents a value in hexadecimal notation.
	std::string CExpEval::ToHex( int64_t _i64Val ) {
		std::string sTmp;
		std::stringstream ssStream;
		_i64Val < 0 ? 
			ssStream << "-0x" <<
				std::hex <<
				std::uppercase <<
				-_i64Val :
			ssStream << "0x" <<
				std::hex <<
				std::uppercase <<
				_i64Val;
		sTmp = ssStream.str();
		return sTmp;
	}

	// Represents a value in hexadecimal notation.
	std::string CExpEval::ToHex( double _d4Val ) {
		std::string sTmp;
		std::stringstream ssStream;
		ssStream << std::hexfloat <<
			std::uppercase <<
			_d4Val;
		sTmp = ssStream.str();
		return sTmp;
	}

	// Represents a value in octadecimal notation.
	std::string CExpEval::ToOct( uint64_t _ui64Val ) {
		std::string sTmp;
		std::stringstream ssStream;
		ssStream << "0o" <<
			std::oct <<
			std::uppercase <<
			_ui64Val;
		sTmp = ssStream.str();
		return sTmp;
	}

	// Represents a value in octadecimal notation.
	std::string CExpEval::ToOct( int64_t _i64Val ) {
		std::string sTmp;
		std::stringstream ssStream;
		_i64Val < 0 ? 
			ssStream << "-0o" <<
				std::oct <<
				std::uppercase <<
				-_i64Val :
			ssStream << "0o" <<
				std::oct <<
				std::uppercase <<
				_i64Val;
		sTmp = ssStream.str();
		return sTmp;
	}

	// Represents a value in octadecimal notation.
	std::string CExpEval::ToOct( double _d4Val ) {
		int64_t i64Tmp = (*reinterpret_cast<int64_t *>(&_d4Val));
		std::string sTmp;
		std::stringstream ssStream;
		i64Tmp < 0 ? 
			ssStream << "-0o" <<
				std::oct <<
				std::uppercase <<
				-i64Tmp :
			ssStream << "0o" <<
				std::oct <<
				std::uppercase <<
				i64Tmp;
		sTmp = ssStream.str();
		return sTmp;
	}

	// Classifies a string as one of the EE_NUM_CONSTANTS types, IE whether the string is a valid float-point or a signed or unsigned integer.  Or EE_NC_INVALID.
	EE_STRING_NUMBER_CLASS CExpEval::ClassifyString( const std::string &_sIn, uint8_t * _pui8SpecialBase ) {
		if ( _pui8SpecialBase ) { (*_pui8SpecialBase) = 0; }
		size_t stIdx = 0;
		while ( IsWhiteSpace( _sIn[stIdx] ) ) { ++stIdx; }
		bool bHasNegSign = _sIn[stIdx] == '-';
		bool bHasPosSign = _sIn[stIdx] == '+';
		if ( bHasNegSign && bHasPosSign ) { return EE_SNC_INVALID; }
		if ( bHasNegSign || bHasPosSign ) {
			// Eat it.
			++stIdx;
		}


		// 0x***/0b***
		if ( (_sIn.size() - stIdx) >= 3 ) {
			if ( _sIn[stIdx] == '0' ) {
				if ( (_sIn[stIdx+1] == 'x' || _sIn[stIdx+1] == 'X') ) {
					size_t I = stIdx + 2;
					for ( ; I < _sIn.size() && ValidHex( _sIn[I] ); ++I ) {}

					if ( stIdx < _sIn.size() ) {
						if ( (_sIn[stIdx] == 'l' || _sIn[stIdx] == 'L') ) { return EE_SNC_SIGNED; }
						else if ( (_sIn[stIdx] == 'u' || _sIn[stIdx] == 'U') ) { return EE_SNC_UNSIGNED; }
					}

					if ( I == _sIn.size() ) {
						if ( _pui8SpecialBase ) { (*_pui8SpecialBase) = 16; }
						return bHasNegSign ? EE_SNC_SIGNED : EE_SNC_UNSIGNED;
					}
					if ( _sIn[I] == '.' ) { return EE_SNC_FLOAT; }
					if ( (_sIn[I] == 'p' || _sIn[I] == 'P') && _sIn.size() > I + 1 ) {
						++I;
						if ( (_sIn[I] == '+' || _sIn[I] == '-') && _sIn.size() > I + 1 ) { ++I; }
						bool bFound = false;
						while ( I < _sIn.size() && IsDigit( _sIn[I++] ) ) { bFound = true; }
						if ( bFound ) { return EE_SNC_FLOAT; }
					}
					// Hex integer literal.
					if ( _pui8SpecialBase ) { (*_pui8SpecialBase) = 16; }
					return bHasNegSign ? EE_SNC_SIGNED : EE_SNC_UNSIGNED;
				}
				if ( (_sIn[stIdx+1] == 'b' || _sIn[stIdx+1] == 'B') &&
					ValidBin( _sIn[stIdx+++2] ) ) {
					stIdx += 2;
					while ( stIdx < _sIn.size() && ValidBin( _sIn[stIdx] ) ) { ++stIdx; }
					// Binary integer literal.
					if ( _pui8SpecialBase ) { (*_pui8SpecialBase) = 2; }
					if ( stIdx < _sIn.size() ) {
						if ( (_sIn[stIdx] == 'l' || _sIn[stIdx] == 'L') ) { return EE_SNC_SIGNED; }
						else if ( (_sIn[stIdx] == 'u' || _sIn[stIdx] == 'U') ) { return EE_SNC_UNSIGNED; }
					}
					return bHasNegSign ? EE_SNC_SIGNED : EE_SNC_UNSIGNED;
				}
			}
		}

		// ***h
		bool bFound = false;
		size_t I = stIdx;
		if ( _pui8SpecialBase ) {
			for ( ; I < _sIn.size(); ++I ) {
				if ( !ValidHex( _sIn[I] ) ) { break; }
				bFound = true;
			}
			if ( bFound ) {
				if ( _sIn[I] == 'h' || _sIn[I] == 'H' ) {
					(*_pui8SpecialBase) = 16;
					return bHasNegSign ? EE_SNC_SIGNED : EE_SNC_UNSIGNED;
				}
			}


			// Now we can easily distinguish between a decimal format and a floating-point format.
			bFound = false;
			I = stIdx;
		}
		for ( ; I < _sIn.size(); ++I ) {
			if ( !IsDigit( _sIn[I] ) ) { break; }
			bFound = true;
		}
		if ( bFound ) {
			if ( _sIn[I] == '.' ) { return EE_SNC_FLOAT; }
			if ( (_sIn[I] == 'e' || _sIn[I] == 'E') &&
				_sIn.size() > I + 1 ) {
				++I;
				if ( (_sIn[I] == '+' || _sIn[I] == '-') &&
					_sIn.size() > I + 1 ) { ++I; }
				if ( IsDigit( _sIn[I] ) ) { return EE_SNC_FLOAT; }
			}
			else if ( (_sIn[I] == 'l' || _sIn[I] == 'L') ) { return EE_SNC_SIGNED; }
			else if ( (_sIn[I] == 'u' || _sIn[I] == 'U') ) { return EE_SNC_UNSIGNED; }
			return bHasNegSign ? EE_SNC_SIGNED : EE_SNC_UNSIGNED;
		}


		return EE_SNC_INVALID;
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
	void CExpEval::ResolveAllEscapes( const std::string &_sInput, std::vector<uint32_t> &_vOutput ) {
		size_t sLen = 1;
		for ( size_t I = 0; I < _sInput.size() && sLen; ) {
			uint64_t uiVal = ResolveEscape( &_sInput.c_str()[I], _sInput.size() - I, sLen, false );
			do {
				_vOutput.push_back( static_cast<std::vector<uint32_t>::value_type>(uiVal) );
				uiVal >>= 32ULL;
			} while ( uiVal );
			I += sLen;
		}
	}

	// Resolves escape sequences.  Input and output are both assumed to be UTF-8.
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
	void CExpEval::ResolveAllEscapes( const std::string &_sInput, std::string &_sOutput, bool _bIsUtf8 ) {
		size_t sLen = 1;
		for ( size_t I = 0; I < _sInput.size() && sLen; I += sLen ) {
			uint64_t uiNext = _bIsUtf8 ? NextUtf8Char( &_sInput[I], _sInput.size() - I, &sLen ) : (static_cast<void>(sLen = 1), static_cast<uint8_t>(_sInput[I]));
			if ( sLen == 1 ) {
				// It was just a regular character.  Could be an escape.
				uiNext = ResolveEscape( &_sInput[I], _sInput.size() - I, sLen, false );
			}
			do {
				uint32_t ui32Len;
				uint32_t ui32BackToUtf8 = _bIsUtf8 ? Utf32ToUtf8( static_cast<uint32_t>(uiNext), ui32Len ) : (static_cast<void>(ui32Len = CountSetBytes( uiNext )), static_cast<uint32_t>(uiNext));
				if ( ui32BackToUtf8 == EE_UTF_INVALID ) {
					ui32Len = 1;
					ui32BackToUtf8 = static_cast<uint32_t>(uiNext);
				}
				for ( uint32_t J = 0; J < ui32Len; ++J ) {
					_sOutput.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
					ui32BackToUtf8 >>= 8;
				}
				uiNext >>= 32ULL;
			} while ( uiNext );
		}
	}

	// Resolves a single escape character, or returns the first input character if not an escape character.
	uint64_t CExpEval::ResolveEscape( const char * _pcInput, size_t _sLen, size_t &_sCharLen, bool _bIncludeHtml, bool * _pbEscapeFound ) {
		if ( _pbEscapeFound ) { (*_pbEscapeFound) = false; }
		if ( !_sLen ) { _sCharLen = 0; return 0; }
		if ( _sLen == 1 ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
		// There are at least 2 characters so an escape is possible.
		if ( _sLen >= 2 && (*_pcInput) == '&' && _bIncludeHtml ) {
			size_t sLen;
			uint64_t uiTmp = EscapeHtml( _pcInput + 1, _sLen - 1, sLen );
			if ( sLen ) {
				_sCharLen = sLen + 1;
				if ( _pbEscapeFound ) { (*_pbEscapeFound) = true; }
				return uiTmp;
			}
		}
		if ( _bIncludeHtml || (*_pcInput) != '\\' ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }	// Not an escape.
		struct {
			// The actual character to find.
			char				cEscape;
			// The character to write to return.
			char				cValue;
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
			{ '\0', '\0' },
		};
		char cNext = _pcInput[1];
		_sCharLen = 1;
		switch ( cNext ) {
			case 'x' : {
				uint32_t ui32Temp = EscapeX( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; if ( _pbEscapeFound ) { (*_pbEscapeFound) = true; } }
				return ui32Temp;
			}
			case 'u' : {
				// Takes \uNNNN and \uNNNN\uNNNN.
				uint32_t ui32Temp = EscapeUnicodeWide4WithSurrogatePairs( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; if ( _pbEscapeFound ) { (*_pbEscapeFound) = true; } }
				return ui32Temp;
			}
			case 'U' : {
				uint32_t ui32Temp = EscapeUnicode8( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; if ( _pbEscapeFound ) { (*_pbEscapeFound) = true; } }
				return ui32Temp;
			}
			case 'N' : {
				uint32_t ui32Temp = EscapeNamedUnicode( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; if ( _pbEscapeFound ) { (*_pbEscapeFound) = true; } }
				return ui32Temp;
			}
			default : {
				if ( _pcInput[1] >= '0' && _pcInput[1] <= '7' ) {
					uint32_t ui32Temp = EscapeOctal( &_pcInput[1], _sLen, _sCharLen );
					++_sCharLen;	// Eat the \.
					if ( _pbEscapeFound ) { (*_pbEscapeFound) = true; }
					return ui32Temp;
				}
				else {
					for ( size_t J = 0; sEscapes[J].cEscape != '\0'; J++ ) {
						if ( _pcInput[1] == sEscapes[J].cEscape ) {
							++_sCharLen;
							if ( _pbEscapeFound ) { (*_pbEscapeFound) = true; }
							return static_cast<uint8_t>(sEscapes[J].cValue);
						}
					}
					// Invalid escape.
					return static_cast<uint8_t>(*_pcInput);
				}
			}
		}
	}

	// Resolves HTML/XML characters.
	// &#nnnn;
	// &#xhhhh;
	void CExpEval::ResolveAllHtmlXmlEscapes( const std::string &_sInput, std::string &_sOutput, bool _bIsUtf8 ) {
		size_t sLen = 1;
		for ( size_t I = 0; I < _sInput.size() && sLen; I += sLen ) {
			uint64_t uiNext = _bIsUtf8 ? NextUtf8Char( &_sInput[I], _sInput.size() - I, &sLen ) : (static_cast<void>(sLen = 1), static_cast<uint8_t>(_sInput[I]));
			if ( sLen == 1 && uiNext == '&' ) {
				// It was just a regular character.  Could be an escape.
				uiNext = ResolveEscape( &_sInput[I], _sInput.size() - I, sLen, true );
			}
			do {
				uint32_t ui32Len;
				uint32_t ui32BackToUtf8 = _bIsUtf8 ? Utf32ToUtf8( static_cast<uint32_t>(uiNext), ui32Len ) : (static_cast<void>(ui32Len = CountSetBytes( static_cast<uint32_t>(uiNext) )), static_cast<uint32_t>(uiNext));
				if ( ui32BackToUtf8 == EE_UTF_INVALID ) {
					ui32Len = 1;
					ui32BackToUtf8 = static_cast<uint32_t>(uiNext);
				}
				for ( uint32_t J = 0; J < ui32Len; ++J ) {
					_sOutput.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
					ui32BackToUtf8 >>= 8;
				}
				uiNext >>= 32ULL;
			} while ( uiNext );
		}
	}

	// Counts the number of UTF-8 code points in the given string.
	uint64_t CExpEval::CountUtfCodePoints( const std::string &_sInput ) {
		uint64_t ui64Tally = 0;
		size_t sPos = 0;
		while ( sPos < _sInput.size() ) {
			size_t sThisSize = Utf8CharSize( &_sInput[sPos], _sInput.size() - sPos );
			if ( !sThisSize ) { break; }
			++ui64Tally;
			sPos += sThisSize;
		}

		return ui64Tally;
	}

	// String to integer, from any base.  Since std::stoull() raises exceptions etc.
	uint64_t CExpEval::StoULL( const char * _pcText, int _iBase, size_t * _psEaten, uint64_t _uiMax, bool * _pbOverflow ) {
		if ( _pbOverflow ) { (*_pbOverflow) = false; }
		const char * _pcOrig = _pcText;
		// Negate?
		bool bNegate = false;
		if ( _pcText[0] == '-' ) {
			bNegate = true;
			++_pcText;
		}

		// Skip whitespace.
		while ( IsWhiteSpace( (*_pcText) ) ) {
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
				else if ( (*_pcText) == 'o' || (*_pcText) == 'O' ) {
					++_pcText;
					if ( _iBase == 0 ) {
						_iBase = 8;
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
			uint64_t uiPreview = uiRes * static_cast<uint64_t>(_iBase) + uiNext;
			// Bounds checking.
			if ( uiPreview > _uiMax ) { break; }

			// Confirm the eating of the character.
			//if ( _psEaten ) { (*_psEaten ) = &_pcText[I] - _pcOrig + 1; }

			// Check for overflow.
			//if ( uiPreview < uiTemp ) {
			if ( (uiPreview - uiNext) / _iBase != uiTemp) {
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
	double CExpEval::AtoF( const char * _pcText, size_t * _psEaten, bool * _pbError ) {
		const char * _pcOrig = _pcText;
		if ( _pbError ) { (*_pbError) = true; }
		// The number is in the following format:
		// [whitespace] [sign] [digits] [.digits] [ {e | E }[sign]digits]
		// Skip whitespace.
		while ( IsWhiteSpace( (*_pcText) ) ) { ++_pcText; }
		//bool bLastWasQuote = false;
		const char * _pcNumberBegins = _pcText;
		// Optional sign.
		if ( (*_pcText) == '-' || (*_pcText) == '+' ) {
			++_pcText;
		}
		if ( _pcText[0] == '0' &&
			(_pcText[1] == 'x' || _pcText[1] == 'x') &&
			(ValidHex( _pcText[2] ) || _pcText[2] == '.') ) {
			_pcText += 2;
			// Digits are optional.
			bool bHasDigits = false;
			while ( ValidHex( (*_pcText) ) ) { ++_pcText; bHasDigits = true; }

			// A decimal point is optional.
			if ( (*_pcText) == '.' ) {
				++_pcText;
				while ( ValidHex( (*_pcText) ) ) { ++_pcText; bHasDigits = true; }
			}

			if ( !bHasDigits ) {
				if ( _psEaten ) { (*_psEaten) = _pcText - _pcOrig; }
				return 0.0;
			}

			// Exponent.
			if ( (*_pcText) != 'P' && (*_pcText) != 'p' ) {
				if ( _psEaten ) { (*_psEaten) = _pcText - _pcOrig; }
				return 0.0;
			}
			++_pcText;

			// Optional sign.
			if ( (*_pcText) == '-' || (*_pcText) == '+' ) {
				++_pcText;
			}
			// Digits must be next.
			if ( !IsDigit( (*_pcText) ) ) {
				if ( _psEaten ) { (*_psEaten) = _pcText - _pcOrig; }
				return 0.0;
			}
			while ( IsDigit( (*_pcText) ) ) { ++_pcText; }
		}
		else {
			// Digits must be next.
			if ( !IsDigit( (*_pcText) ) ) {
				if ( _psEaten ) { (*_psEaten) = _pcText - _pcOrig; }
				return 0.0;
			}
			while ( IsDigit( (*_pcText) ) ) { ++_pcText; }

			// A decimal point and trailing digits are optional.
			if ( (*_pcText) == '.' ) {
				++_pcText;
				while ( IsDigit( (*_pcText) ) ) { ++_pcText; }
			}
			// Exponent.
			if ( (*_pcText) == 'E' || (*_pcText) == 'e' ) {
				++_pcText;
				// Optional sign.
				if ( (*_pcText) == '-' || (*_pcText) == '+' ) {
					++_pcText;
				}
				// Digits must be next.
				if ( !IsDigit( (*_pcText) ) ) {
					if ( _psEaten ) { (*_psEaten) = _pcText - _pcOrig; }
					return 0.0;
				}
				while ( IsDigit( (*_pcText) ) ) { ++_pcText; }
			}
		}

		// Optional suffix.
		bool bFloat = false;
		if ( (*_pcText) == 'f' || (*_pcText) == 'F' ) {
			++_pcText;
			bFloat = true;
		}
		else if ( (*_pcText) == 'l' || (*_pcText) == 'L' ) {
			++_pcText;
		}

		if ( _psEaten ) { (*_psEaten) = _pcText - _pcOrig; }
		if ( _pbError ) { (*_pbError) = false; }
		return bFloat ? float( ::atof( _pcNumberBegins ) ) : ::atof( _pcNumberBegins );
	}

	// Gets the timer frequency.
	uint64_t CExpEval::TimerFrequency() {
		static uint64_t uiFreq = 0;
		if ( !uiFreq ) {
#ifdef _WIN32
			::QueryPerformanceFrequency( reinterpret_cast<LARGE_INTEGER *>(&uiFreq) );
#else
			if ( !m_mtidInfoData.denom ) {
				if ( KERN_SUCCESS != ::mach_timebase_info( &m_mtidInfoData ) ) {
					return 0ULL;
				}
			}
			uiFreq = (m_mtidInfoData.denom * 1000000000ULL);
#endif	// #ifdef _WIN32
		}
		return uiFreq;
	}

	// Pulls any preprocessing directives out of a single line.
	bool CExpEval::PreprocessingDirectives( const std::string &_sInput, std::string &_sDirective, std::string &_sParms ) {
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

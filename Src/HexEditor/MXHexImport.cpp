#include "MXHexImport.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"

#include <algorithm>
#include <cstring>
#include <map>
#include <string>


namespace mx {

	// == Functions.
	/**
	 * Imports text.
	 *
	 * \param _pui8Text The text (ASCII or UTF-8).
	 * \param _sSize The number of bytes at _pui8Text.
	 * \param _itType The kind of text, or MX_IT_AUTO to decide from the contents.
	 * \param _ioOptions The import settings.
	 * \param _irResult Receives the bytes.
	 * \param _swsError Receives a description of the problem on failure.
	 * \return Returns true if the text was imported.
	 **/
	bool CHexImport::Import( const uint8_t * _pui8Text, size_t _sSize, MX_IMPORT_TYPE _itType, const MX_IMPORT_OPTIONS &_ioOptions,
		MX_IMPORT_RESULT &_irResult, CSecureWString &_swsError ) {
		try {
			// A UTF-8 byte-order mark is not part of the data.
			if ( _sSize >= 3 && _pui8Text[0] == 0xEF && _pui8Text[1] == 0xBB && _pui8Text[2] == 0xBF ) {
				_pui8Text += 3;
				_sSize -= 3;
			}
			if ( _itType == MX_IT_AUTO ) { _itType = Detect( _pui8Text, _sSize ); }
			_irResult = MX_IMPORT_RESULT();
			_irResult.itType = _itType;

			bool bRet = false;
			size_t sBadLine = 0;
			switch ( _itType ) {
				case MX_IT_INTEL_HEX : {}							MX_FALLTHROUGH
				case MX_IT_MOTOROLA : {
					return ImportRecords( _pui8Text, _sSize, _itType == MX_IT_INTEL_HEX, _ioOptions, _irResult, _swsError );
				}
				case MX_IT_DECIMAL_TEXT : {
					bRet = ImportNumbers( _pui8Text, _sSize, 10, _irResult.vData, sBadLine );
					break;
				}
				case MX_IT_BINARY_TEXT : {
					bRet = ImportNumbers( _pui8Text, _sSize, 2, _irResult.vData, sBadLine );
					break;
				}
				case MX_IT_SOURCE_CODE : {
					bRet = ImportSourceCode( _pui8Text, _sSize, _irResult.vData, sBadLine );
					break;
				}
				case MX_IT_BASE64 : {
					bRet = ImportBase64( _pui8Text, _sSize, _irResult.vData, sBadLine );
					break;
				}
				case MX_IT_UUENCODING : {
					bRet = ImportUuEncoding( _pui8Text, _sSize, _irResult.vData, sBadLine );
					break;
				}
				default : {
					bRet = ImportHexText( _pui8Text, _sSize, _irResult.vData );
				}
			}
			if ( !bRet ) {
				LineError( _DEC_WS_2EF0FF63_The_file_is_not_valid___The_first_problem_is_on_line_, sBadLine, _swsError );
				return false;
			}
			if ( _irResult.vData.empty() ) {
				_swsError = _DEC_WS_7F0157C2_The_file_contains_nothing_that_can_be_imported_;
				return false;
			}
			return true;
		}
		catch ( ... ) {
			_swsError = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * Decides what kind of text something is.
	 *
	 * \param _pui8Text The text.
	 * \param _sSize The number of bytes at _pui8Text.
	 * \return Returns the kind of text (MX_IT_HEX_TEXT if nothing more specific fits).
	 **/
	CHexImport::MX_IMPORT_TYPE CHexImport::Detect( const uint8_t * _pui8Text, size_t _sSize ) {
		// The first character that is not white space.
		size_t sFirst = 0;
		while ( sFirst < _sSize && (_pui8Text[sFirst] == ' ' || _pui8Text[sFirst] == '\t' || _pui8Text[sFirst] == '\r' || _pui8Text[sFirst] == '\n') ) { ++sFirst; }
		if ( sFirst == _sSize ) { return MX_IT_HEX_TEXT; }

		if ( _pui8Text[sFirst] == ':' && sFirst + 1 < _sSize && HexVal( _pui8Text[sFirst+1] ) >= 0 ) { return MX_IT_INTEL_HEX; }
		if ( (_pui8Text[sFirst] == 'S' || _pui8Text[sFirst] == 's') && sFirst + 1 < _sSize && _pui8Text[sFirst+1] >= '0' && _pui8Text[sFirst+1] <= '9' ) { return MX_IT_MOTOROLA; }

		try {
			// "begin " at the start of any line in the first part of the text.
			const std::string sBegin = _DEC_S_84CBAE3B_begin_;
			const size_t sLook = std::min<size_t>( _sSize, 4096 );
			for ( size_t I = sFirst; I + sBegin.size() <= sLook; ++I ) {
				if ( (I == 0 || _pui8Text[I-1] == '\n') && std::equal( sBegin.begin(), sBegin.end(), _pui8Text + I ) ) { return MX_IT_UUENCODING; }
			}
		}
		catch ( ... ) {}

		bool bBase64Only = true, bNonHexBase64 = false;
		for ( size_t I = sFirst; I < _sSize; ++I ) {
			const uint8_t ui8This = _pui8Text[I];
			if ( ui8This == '{' || ui8This == '[' ) { return MX_IT_SOURCE_CODE; }
			if ( ui8This == '\r' || ui8This == '\n' ) { continue; }
			const bool bAlnum = (ui8This >= '0' && ui8This <= '9') || (ui8This >= 'A' && ui8This <= 'Z') || (ui8This >= 'a' && ui8This <= 'z');
			if ( bAlnum || ui8This == '+' || ui8This == '/' || ui8This == '=' ) {
				if ( HexVal( ui8This ) < 0 ) { bNonHexBase64 = true; }
			}
			else {
				bBase64Only = false;
			}
		}
		// Text that could only be Base64 (letters past F, '+', '/', or '=' and no spaces or punctuation).
		if ( bBase64Only && bNonHexBase64 ) { return MX_IT_BASE64; }
		return MX_IT_HEX_TEXT;
	}

	/**
	 * Splits text into lines (without their line endings).
	 *
	 * \param _pui8Text The text.
	 * \param _sSize The number of bytes at _pui8Text.
	 * \param _vLines Receives the start and length of each line.
	 **/
	void CHexImport::SplitLines( const uint8_t * _pui8Text, size_t _sSize, std::vector<std::pair<size_t, size_t>> &_vLines ) {
		_vLines.clear();
		size_t sStart = 0;
		for ( size_t I = 0; I <= _sSize; ++I ) {
			if ( I == _sSize || _pui8Text[I] == '\n' ) {
				size_t sEnd = I;
				if ( sEnd > sStart && _pui8Text[sEnd-1] == '\r' ) { --sEnd; }
				_vLines.push_back( { sStart, sEnd - sStart } );
				sStart = I + 1;
			}
		}
	}

	/**
	 * Imports pairs of hexadecimal digits.
	 *
	 * \param _pui8Text The text.
	 * \param _sSize The number of bytes at _pui8Text.
	 * \param _vData Receives the bytes.
	 * \return Returns false on a memory failure.
	 **/
	bool CHexImport::ImportHexText( const uint8_t * _pui8Text, size_t _sSize, std::vector<uint8_t> &_vData ) {
		try {
			_vData.clear();
			_vData.reserve( _sSize / 2 );
			int32_t i32High = -1;
			for ( size_t I = 0; I < _sSize; ++I ) {
				// "0x" prefixes are skipped.
				if ( i32High < 0 && _pui8Text[I] == '0' && I + 2 < _sSize && (_pui8Text[I+1] == 'x' || _pui8Text[I+1] == 'X') && HexVal( _pui8Text[I+2] ) >= 0 ) {
					++I;
					continue;
				}
				const int32_t i32Val = HexVal( _pui8Text[I] );
				if ( i32Val < 0 ) {
					// A lone digit before a separator is a whole byte.
					if ( i32High >= 0 ) {
						_vData.push_back( static_cast<uint8_t>(i32High) );
						i32High = -1;
					}
					continue;
				}
				if ( i32High < 0 ) { i32High = i32Val; }
				else {
					_vData.push_back( static_cast<uint8_t>((i32High << 4) | i32Val) );
					i32High = -1;
				}
			}
			if ( i32High >= 0 ) { _vData.push_back( static_cast<uint8_t>(i32High) ); }
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Imports decimal or binary numbers.
	 *
	 * \param _pui8Text The text.
	 * \param _sSize The number of bytes at _pui8Text.
	 * \param _ui32Base 10 or 2.
	 * \param _vData Receives the bytes.
	 * \param _sBadLine Receives the line (from 1) of the first invalid number on failure, or 0 for a memory failure.
	 * \return Returns false on failure.
	 **/
	bool CHexImport::ImportNumbers( const uint8_t * _pui8Text, size_t _sSize, uint32_t _ui32Base, std::vector<uint8_t> &_vData, size_t &_sBadLine ) {
		_sBadLine = 0;
		try {
			_vData.clear();
			size_t sLine = 1;
			uint32_t ui32Val = 0;
			uint32_t ui32Digits = 0;
			const uint8_t ui8Last = static_cast<uint8_t>('0' + _ui32Base - 1);
			for ( size_t I = 0; I <= _sSize; ++I ) {
				const uint8_t ui8This = I < _sSize ? _pui8Text[I] : ' ';
				if ( ui8This >= '0' && ui8This <= '9' ) {
					// A digit that does not belong to the base (a 2 in binary text) is an error.
					if ( ui8This > ui8Last ) {
						_sBadLine = sLine;
						return false;
					}
					ui32Val = ui32Val * _ui32Base + uint32_t( ui8This - '0' );
					++ui32Digits;
					if ( ui32Val > 255 || (_ui32Base == 2 && ui32Digits > 8) ) {
						_sBadLine = sLine;
						return false;
					}
					continue;
				}
				if ( ui32Digits ) {
					_vData.push_back( static_cast<uint8_t>(ui32Val) );
					ui32Val = 0;
					ui32Digits = 0;
				}
				if ( ui8This == '\n' ) { ++sLine; }
				else if ( (ui8This >= 'A' && ui8This <= 'Z') || (ui8This >= 'a' && ui8This <= 'z') ) {
					// Letters mean this is not decimal/binary text.
					_sBadLine = sLine;
					return false;
				}
			}
			return true;
		}
		catch ( ... ) {
			_sBadLine = 0;
			return false;
		}
	}

	/**
	 * Imports the values of a C, C++, Java, or Python array.
	 *
	 * \param _pui8Text The text.
	 * \param _sSize The number of bytes at _pui8Text.
	 * \param _vData Receives the bytes.
	 * \param _sBadLine Receives the line (from 1) of the first invalid value on failure, or 0 for a memory failure.
	 * \return Returns false on failure.
	 **/
	bool CHexImport::ImportSourceCode( const uint8_t * _pui8Text, size_t _sSize, std::vector<uint8_t> &_vData, size_t &_sBadLine ) {
		_sBadLine = 0;
		try {
			_vData.clear();
			// The values are between the first '{' (C, C++, Java) or, failing that, the first '[' (Python) and the matching close.
			size_t sPos = 0;
			uint8_t ui8Close = 0;
			for ( size_t I = 0; I < _sSize; ++I ) {
				if ( _pui8Text[I] == '{' ) { sPos = I + 1; ui8Close = '}'; break; }
			}
			if ( !ui8Close ) {
				for ( size_t I = 0; I < _sSize; ++I ) {
					if ( _pui8Text[I] == '[' ) { sPos = I + 1; ui8Close = ']'; break; }
				}
			}
			size_t sLine = 1;
			for ( size_t I = 0; I < sPos; ++I ) {
				if ( _pui8Text[I] == '\n' ) { ++sLine; }
			}

			int32_t i32Depth = 0;
			while ( sPos < _sSize ) {
				const uint8_t ui8This = _pui8Text[sPos];
				if ( ui8This == '\n' ) { ++sLine; ++sPos; continue; }
				// Comments.
				if ( ui8This == '/' && sPos + 1 < _sSize && _pui8Text[sPos+1] == '/' ) {
					while ( sPos < _sSize && _pui8Text[sPos] != '\n' ) { ++sPos; }
					continue;
				}
				if ( ui8This == '#' ) {
					while ( sPos < _sSize && _pui8Text[sPos] != '\n' ) { ++sPos; }
					continue;
				}
				if ( ui8This == '/' && sPos + 1 < _sSize && _pui8Text[sPos+1] == '*' ) {
					sPos += 2;
					while ( sPos + 1 < _sSize && !(_pui8Text[sPos] == '*' && _pui8Text[sPos+1] == '/') ) {
						if ( _pui8Text[sPos] == '\n' ) { ++sLine; }
						++sPos;
					}
					sPos += 2;
					continue;
				}
				// Nested brackets (Python's join([...]) has parentheses; C arrays of arrays have braces).
				if ( ui8This == '{' || ui8This == '[' || ui8This == '(' ) {
					// "(byte)" and similar casts are skipped whole.
					if ( ui8This == '(' ) {
						size_t sEnd = sPos + 1;
						while ( sEnd < _sSize && ((_pui8Text[sEnd] >= 'a' && _pui8Text[sEnd] <= 'z') || (_pui8Text[sEnd] >= 'A' && _pui8Text[sEnd] <= 'Z') || _pui8Text[sEnd] == '_' || _pui8Text[sEnd] == ' ') ) { ++sEnd; }
						if ( sEnd < _sSize && _pui8Text[sEnd] == ')' && sEnd > sPos + 1 ) {
							sPos = sEnd + 1;
							continue;
						}
					}
					++i32Depth;
					++sPos;
					continue;
				}
				if ( ui8This == '}' || ui8This == ']' || ui8This == ')' ) {
					if ( i32Depth == 0 && ui8This == ui8Close ) { break; }
					if ( i32Depth ) { --i32Depth; }
					++sPos;
					continue;
				}
				// Byte strings (Python's b'\x00\x01').
				if ( ui8This == '\'' || ui8This == '"' ) {
					const uint8_t ui8Quote = ui8This;
					++sPos;
					while ( sPos < _sSize && _pui8Text[sPos] != ui8Quote ) {
						uint8_t ui8Char = _pui8Text[sPos++];
						if ( ui8Char == '\n' ) {
							_sBadLine = sLine;
							return false;
						}
						if ( ui8Char == '\\' && sPos < _sSize ) {
							const uint8_t ui8Esc = _pui8Text[sPos++];
							switch ( ui8Esc ) {
								case 'x' : {}				MX_FALLTHROUGH
								case 'X' : {
									if ( sPos + 1 >= _sSize || HexVal( _pui8Text[sPos] ) < 0 || HexVal( _pui8Text[sPos+1] ) < 0 ) {
										_sBadLine = sLine;
										return false;
									}
									ui8Char = static_cast<uint8_t>((HexVal( _pui8Text[sPos] ) << 4) | HexVal( _pui8Text[sPos+1] ));
									sPos += 2;
									break;
								}
								case 'n' : { ui8Char = '\n'; break; }
								case 'r' : { ui8Char = '\r'; break; }
								case 't' : { ui8Char = '\t'; break; }
								case '0' : { ui8Char = 0; break; }
								default : { ui8Char = ui8Esc; }
							}
						}
						_vData.push_back( ui8Char );
					}
					++sPos;
					continue;
				}
				// Numbers: 0x.., 0b.., or decimal, optionally negative (Java bytes are signed).
				const bool bNeg = ui8This == '-' && sPos + 1 < _sSize && _pui8Text[sPos+1] >= '0' && _pui8Text[sPos+1] <= '9';
				if ( bNeg || (ui8This >= '0' && ui8This <= '9') ) {
					if ( bNeg ) { ++sPos; }
					uint32_t ui32Base = 10;
					if ( _pui8Text[sPos] == '0' && sPos + 1 < _sSize ) {
						if ( _pui8Text[sPos+1] == 'x' || _pui8Text[sPos+1] == 'X' ) { ui32Base = 16; sPos += 2; }
						else if ( _pui8Text[sPos+1] == 'b' || _pui8Text[sPos+1] == 'B' ) { ui32Base = 2; sPos += 2; }
					}
					uint64_t ui64Val = 0;
					size_t sDigits = 0;
					while ( sPos < _sSize ) {
						int32_t i32Digit = HexVal( _pui8Text[sPos] );
						if ( i32Digit < 0 || uint32_t( i32Digit ) >= ui32Base ) { break; }
						ui64Val = ui64Val * ui32Base + uint32_t( i32Digit );
						if ( ui64Val > 0xFFFF ) {
							_sBadLine = sLine;
							return false;
						}
						++sDigits;
						++sPos;
					}
					// Suffixes such as 'u' or 'U'.
					while ( sPos < _sSize && (_pui8Text[sPos] == 'u' || _pui8Text[sPos] == 'U' || _pui8Text[sPos] == 'l' || _pui8Text[sPos] == 'L') ) { ++sPos; }
					if ( !sDigits || (!bNeg && ui64Val > 255) || (bNeg && ui64Val > 128) ) {
						_sBadLine = sLine;
						return false;
					}
					_vData.push_back( static_cast<uint8_t>(bNeg ? (0x100 - ui64Val) & 0xFF : ui64Val) );
					continue;
				}
				// Identifiers (b in b'', byte, etc.) are skipped.
				if ( (ui8This >= 'a' && ui8This <= 'z') || (ui8This >= 'A' && ui8This <= 'Z') || ui8This == '_' ) {
					while ( sPos < _sSize && ((_pui8Text[sPos] >= 'a' && _pui8Text[sPos] <= 'z') || (_pui8Text[sPos] >= 'A' && _pui8Text[sPos] <= 'Z') ||
						(_pui8Text[sPos] >= '0' && _pui8Text[sPos] <= '9') || _pui8Text[sPos] == '_') ) { ++sPos; }
					continue;
				}
				// Separators.
				++sPos;
			}
			return true;
		}
		catch ( ... ) {
			_sBadLine = 0;
			return false;
		}
	}

	/**
	 * Imports Intel HEX or Motorola records.
	 *
	 * \param _pui8Text The text.
	 * \param _sSize The number of bytes at _pui8Text.
	 * \param _bIntel True for Intel HEX, false for Motorola.
	 * \param _ioOptions The import settings.
	 * \param _irResult Receives the bytes and the starting address.
	 * \param _swsError Receives a description of the problem on failure.
	 * \return Returns false on failure.
	 **/
	bool CHexImport::ImportRecords( const uint8_t * _pui8Text, size_t _sSize, bool _bIntel, const MX_IMPORT_OPTIONS &_ioOptions,
		MX_IMPORT_RESULT &_irResult, CSecureWString &_swsError ) {
		try {
			std::vector<std::pair<size_t, size_t>> vLines;
			SplitLines( _pui8Text, _sSize, vLines );
			std::vector<MX_CHUNK> vChunks;
			std::vector<uint8_t> vRecord;
			uint64_t ui64Base = 0;
			for ( size_t L = 0; L < vLines.size(); ++L ) {
				const uint8_t * pui8Line = _pui8Text + vLines[L].first;
				size_t sLen = vLines[L].second;
				// Leading and trailing white space and blank lines are ignored.
				while ( sLen && (pui8Line[0] == ' ' || pui8Line[0] == '\t') ) { ++pui8Line; --sLen; }
				while ( sLen && (pui8Line[sLen-1] == ' ' || pui8Line[sLen-1] == '\t') ) { --sLen; }
				if ( !sLen ) { continue; }

				const size_t sStart = _bIntel ? 1 : 2;
				if ( (_bIntel && pui8Line[0] != ':') || (!_bIntel && ((pui8Line[0] != 'S' && pui8Line[0] != 's') || sLen < 2)) || ((sLen - sStart) & 1) ) {
					LineError( _DEC_WS_2EF0FF63_The_file_is_not_valid___The_first_problem_is_on_line_, L + 1, _swsError );
					return false;
				}
				vRecord.clear();
				for ( size_t I = sStart; I < sLen; I += 2 ) {
					const int32_t i32Hi = HexVal( pui8Line[I] ), i32Lo = HexVal( pui8Line[I+1] );
					if ( i32Hi < 0 || i32Lo < 0 ) {
						LineError( _DEC_WS_2EF0FF63_The_file_is_not_valid___The_first_problem_is_on_line_, L + 1, _swsError );
						return false;
					}
					vRecord.push_back( static_cast<uint8_t>((i32Hi << 4) | i32Lo) );
				}

				uint8_t ui8Sum = 0;
				for ( auto ui8This : vRecord ) { ui8Sum = static_cast<uint8_t>(ui8Sum + ui8This); }
				if ( _bIntel ) {
					// ":LLAAAATT<data>CC"; all bytes add up to 0.
					if ( vRecord.size() < 5 || vRecord.size() != size_t( vRecord[0] ) + 5 ) {
						LineError( _DEC_WS_2EF0FF63_The_file_is_not_valid___The_first_problem_is_on_line_, L + 1, _swsError );
						return false;
					}
					if ( ui8Sum != 0 ) {
						LineError( _DEC_WS_A27C6232_The_checksum_is_wrong_on_line_, L + 1, _swsError );
						return false;
					}
					const size_t sCount = vRecord[0];
					const uint64_t ui64Off = (uint64_t( vRecord[1] ) << 8) | vRecord[2];
					const uint8_t ui8Type = vRecord[3];
					const uint8_t * pui8Data = vRecord.data() + 4;
					if ( ui8Type == 0x01 ) { break; }
					switch ( ui8Type ) {
						case 0x00 : {
							MX_CHUNK cChunk;
							cChunk.ui64Addr = (ui64Base + ui64Off) << (_ioOptions.bWordAddresses ? 1 : 0);
							cChunk.vBytes.assign( pui8Data, pui8Data + sCount );
							vChunks.push_back( std::move( cChunk ) );
							break;
						}
						case 0x02 : {
							// Extended segment address (the segment times 16).
							if ( sCount != 2 ) {
								LineError( _DEC_WS_2EF0FF63_The_file_is_not_valid___The_first_problem_is_on_line_, L + 1, _swsError );
								return false;
							}
							ui64Base = ((uint64_t( pui8Data[0] ) << 8) | pui8Data[1]) << 4;
							break;
						}
						case 0x04 : {
							// Extended linear address (the upper 16 bits).
							if ( sCount != 2 ) {
								LineError( _DEC_WS_2EF0FF63_The_file_is_not_valid___The_first_problem_is_on_line_, L + 1, _swsError );
								return false;
							}
							ui64Base = ((uint64_t( pui8Data[0] ) << 8) | pui8Data[1]) << 16;
							break;
						}
						case 0x03 : {}								MX_FALLTHROUGH
						case 0x05 : { break; }						// Start addresses (for running the program) are not data.
						default : {
							LineError( _DEC_WS_2EF0FF63_The_file_is_not_valid___The_first_problem_is_on_line_, L + 1, _swsError );
							return false;
						}
					}
				}
				else {
					// "S<type><count><address><data><checksum>"; the count, address, data, and checksum add up to 0xFF.
					const uint8_t ui8Type = static_cast<uint8_t>(pui8Line[1] - '0');
					static const size_t sAddrBytes[10] = { 2, 2, 3, 4, 0, 2, 3, 4, 3, 2 };
					if ( ui8Type > 9 || ui8Type == 4 || vRecord.size() < 1 + sAddrBytes[ui8Type] + 1 || vRecord.size() != size_t( vRecord[0] ) + 1 ) {
						LineError( _DEC_WS_2EF0FF63_The_file_is_not_valid___The_first_problem_is_on_line_, L + 1, _swsError );
						return false;
					}
					if ( ui8Sum != 0xFF ) {
						LineError( _DEC_WS_A27C6232_The_checksum_is_wrong_on_line_, L + 1, _swsError );
						return false;
					}
					if ( ui8Type >= 7 ) { break; }
					if ( ui8Type >= 1 && ui8Type <= 3 ) {
						uint64_t ui64Addr = 0;
						for ( size_t I = 0; I < sAddrBytes[ui8Type]; ++I ) {
							ui64Addr = (ui64Addr << 8) | vRecord[1+I];
						}
						const uint8_t * pui8Data = vRecord.data() + 1 + sAddrBytes[ui8Type];
						const size_t sCount = vRecord.size() - 1 - sAddrBytes[ui8Type] - 1;
						MX_CHUNK cChunk;
						cChunk.ui64Addr = ui64Addr << (_ioOptions.bWordAddresses ? 1 : 0);
						cChunk.vBytes.assign( pui8Data, pui8Data + sCount );
						vChunks.push_back( std::move( cChunk ) );
					}
					// S0 (header) and S5/S6 (record counts) are not data.
				}
			}

			// Put the runs of bytes together, filling the gaps.
			uint64_t ui64Min = UINT64_MAX, ui64Max = 0;
			for ( const auto &cChunk : vChunks ) {
				if ( cChunk.vBytes.empty() ) { continue; }
				ui64Min = std::min( ui64Min, cChunk.ui64Addr );
				ui64Max = std::max( ui64Max, cChunk.ui64Addr + cChunk.vBytes.size() );
			}
			if ( ui64Min == UINT64_MAX ) {
				_swsError = _DEC_WS_7F0157C2_The_file_contains_nothing_that_can_be_imported_;
				return false;
			}
			const uint64_t ui64Start = _ioOptions.bAutoStartAddress ? ui64Min : 0;
			if ( ui64Max - ui64Start > 1024ULL * 1024ULL * 1024ULL ) {
				_swsError = _DEC_WS_76ED7F76_The_imported_data_would_be_larger_than_1_GB_;
				return false;
			}
			_irResult.vData.assign( size_t( ui64Max - ui64Start ), _ioOptions.ui8FillByte );
			for ( const auto &cChunk : vChunks ) {
				if ( cChunk.vBytes.size() ) {
					std::memcpy( _irResult.vData.data() + (cChunk.ui64Addr - ui64Start), cChunk.vBytes.data(), cChunk.vBytes.size() );
				}
			}
			_irResult.ui64StartAddress = ui64Start;
			return true;
		}
		catch ( ... ) {
			_swsError = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * Imports Base64.
	 *
	 * \param _pui8Text The text.
	 * \param _sSize The number of bytes at _pui8Text.
	 * \param _vData Receives the bytes.
	 * \param _sBadLine Receives the line (from 1) of the first invalid character on failure, or 0 for a memory failure.
	 * \return Returns false on failure.
	 **/
	bool CHexImport::ImportBase64( const uint8_t * _pui8Text, size_t _sSize, std::vector<uint8_t> &_vData, size_t &_sBadLine ) {
		_sBadLine = 0;
		try {
			_vData.clear();
			_vData.reserve( _sSize * 3 / 4 );
			uint32_t ui32Bits = 0;
			uint32_t ui32Count = 0;
			size_t sLine = 1;
			bool bPadding = false;
			for ( size_t I = 0; I < _sSize; ++I ) {
				const uint8_t ui8This = _pui8Text[I];
				int32_t i32Val = -1;
				if ( ui8This >= 'A' && ui8This <= 'Z' ) { i32Val = ui8This - 'A'; }
				else if ( ui8This >= 'a' && ui8This <= 'z' ) { i32Val = ui8This - 'a' + 26; }
				else if ( ui8This >= '0' && ui8This <= '9' ) { i32Val = ui8This - '0' + 52; }
				else if ( ui8This == '+' || ui8This == '-' ) { i32Val = 62; }
				else if ( ui8This == '/' || ui8This == '_' ) { i32Val = 63; }
				else if ( ui8This == '=' ) { bPadding = true; continue; }
				else if ( ui8This == '\n' ) { ++sLine; continue; }
				else if ( ui8This == '\r' || ui8This == ' ' || ui8This == '\t' ) { continue; }
				if ( i32Val < 0 || bPadding ) {
					_sBadLine = sLine;
					return false;
				}
				ui32Bits = (ui32Bits << 6) | uint32_t( i32Val );
				if ( ++ui32Count == 4 ) {
					_vData.push_back( static_cast<uint8_t>(ui32Bits >> 16) );
					_vData.push_back( static_cast<uint8_t>(ui32Bits >> 8) );
					_vData.push_back( static_cast<uint8_t>(ui32Bits) );
					ui32Bits = 0;
					ui32Count = 0;
				}
			}
			// A partial group gives 1 or 2 more bytes.
			if ( ui32Count == 1 ) {
				_sBadLine = sLine;
				return false;
			}
			if ( ui32Count == 2 ) {
				_vData.push_back( static_cast<uint8_t>(ui32Bits >> 4) );
			}
			else if ( ui32Count == 3 ) {
				_vData.push_back( static_cast<uint8_t>(ui32Bits >> 10) );
				_vData.push_back( static_cast<uint8_t>(ui32Bits >> 2) );
			}
			return true;
		}
		catch ( ... ) {
			_sBadLine = 0;
			return false;
		}
	}

	/**
	 * Imports a uuencoded file.
	 *
	 * \param _pui8Text The text.
	 * \param _sSize The number of bytes at _pui8Text.
	 * \param _vData Receives the bytes.
	 * \param _sBadLine Receives the line (from 1) of the first invalid line on failure, or 0 for a memory failure.
	 * \return Returns false on failure.
	 **/
	bool CHexImport::ImportUuEncoding( const uint8_t * _pui8Text, size_t _sSize, std::vector<uint8_t> &_vData, size_t &_sBadLine ) {
		_sBadLine = 0;
		try {
			_vData.clear();
			std::vector<std::pair<size_t, size_t>> vLines;
			SplitLines( _pui8Text, _sSize, vLines );
			const std::string sBegin = _DEC_S_84CBAE3B_begin_;
			const std::string sEnd = _DEC_S_00FC33B1_end;

			// The data follows the "begin <mode> <name>" line.
			size_t L = 0;
			for ( ; L < vLines.size(); ++L ) {
				if ( vLines[L].second >= sBegin.size() && std::equal( sBegin.begin(), sBegin.end(), _pui8Text + vLines[L].first ) ) { break; }
			}
			if ( L == vLines.size() ) {
				_sBadLine = 1;
				return false;
			}
			auto UuVal = []( uint8_t _ui8Char ) -> uint32_t { return uint32_t( _ui8Char - ' ' ) & 0x3F; };
			for ( ++L; L < vLines.size(); ++L ) {
				const uint8_t * pui8Line = _pui8Text + vLines[L].first;
				const size_t sLen = vLines[L].second;
				if ( sLen == sEnd.size() && std::equal( sEnd.begin(), sEnd.end(), pui8Line ) ) { break; }
				if ( !sLen ) { continue; }
				const uint32_t ui32Bytes = UuVal( pui8Line[0] );
				if ( !ui32Bytes ) { continue; }
				// Each 4 characters give 3 bytes.  Trailing spaces are sometimes trimmed, so missing characters count as 0.
				if ( (sLen - 1) * 3 / 4 + 3 < ui32Bytes ) {
					_sBadLine = L + 1;
					return false;
				}
				uint32_t ui32Done = 0;
				for ( size_t I = 1; ui32Done < ui32Bytes; I += 4 ) {
					uint32_t ui32Bits = 0;
					for ( size_t J = 0; J < 4; ++J ) {
						ui32Bits = (ui32Bits << 6) | (I + J < sLen ? UuVal( pui8Line[I+J] ) : 0);
					}
					for ( int32_t K = 2; K >= 0 && ui32Done < ui32Bytes; --K, ++ui32Done ) {
						_vData.push_back( static_cast<uint8_t>(ui32Bits >> (K * 8)) );
					}
				}
			}
			return true;
		}
		catch ( ... ) {
			_sBadLine = 0;
			return false;
		}
	}

	/**
	 * Makes the "invalid at line N" or "checksum is wrong at line N" error.
	 *
	 * \param _swsPrefix The start of the message.
	 * \param _sLine The line (from 1), or 0 for a memory failure.
	 * \param _swsError Receives the message.
	 **/
	void CHexImport::LineError( const CSecureWString &_swsPrefix, size_t _sLine, CSecureWString &_swsError ) {
		try {
			if ( !_sLine ) {
				_swsError = _DEC_WS_F39F91A5_Out_of_memory_;
				return;
			}
			_swsError = _swsPrefix;
			_swsError += std::to_wstring( _sLine );
			_swsError.push_back( L'.' );
		}
		catch ( ... ) {}
	}

}	// namespace mx

#include "MXInspector.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"

#include <EEExpEval.h>
#include <Expressions/LSWExpression.h>
#include <Float16/EEFloat16.h>

#include <cmath>
#include <cstring>


namespace mx {

	// == Functions.
	/**
	 * Gets the default settings.
	 *
	 * \param _sSettings Receives the default settings.
	 **/
	void CInspector::DefaultSettings( MX_SETTINGS &_sSettings ) {
		_sSettings = MX_SETTINGS();
		for ( uint32_t I = 0; I < MX_IT_TOTAL; ++I ) {
			_sSettings.vRows.push_back( MX_ROW_SETTING{ .itType = static_cast<MX_INSPECTOR_TYPE>(I), .bShown = true } );
		}
		_sSettings.wsDateFormat = _DEC_WS_B27B5A4D_MM_dd_yyyy;
		_sSettings.wsTimeFormat = _DEC_WS_0153EF33_HH_mm_ss;
	}

	/**
	 * Makes sure that every type appears exactly once in a set of settings (types that are missing are added at the end, hidden,
	 *	and invalid or repeated types are removed).
	 *
	 * \param _sSettings The settings to fix.
	 **/
	void CInspector::FixSettings( MX_SETTINGS &_sSettings ) {
		bool bSeen[MX_IT_TOTAL] = {};
		std::vector<MX_ROW_SETTING> vRows;
		vRows.reserve( MX_IT_TOTAL );
		for ( const auto & rsRow : _sSettings.vRows ) {
			if ( rsRow.itType >= MX_IT_TOTAL || bSeen[rsRow.itType] ) { continue; }
			bSeen[rsRow.itType] = true;
			vRows.push_back( rsRow );
		}
		for ( uint32_t I = 0; I < MX_IT_TOTAL; ++I ) {
			if ( !bSeen[I] ) { vRows.push_back( MX_ROW_SETTING{ .itType = static_cast<MX_INSPECTOR_TYPE>(I), .bShown = false } ); }
		}
		_sSettings.vRows = std::move( vRows );
	}

	/**
	 * Gets the name of a type, as shown in the Type column.
	 *
	 * \param _itType The type.
	 * \return Returns the name of the type.
	 **/
	CSecureWString CInspector::TypeName( MX_INSPECTOR_TYPE _itType ) {
		switch ( _itType ) {
			case MX_IT_BINARY : { return _DEC_WS_C14CEC33_Binary; }
			case MX_IT_INT8 : { return _DEC_WS_CDD690D1_Signed_Byte; }
			case MX_IT_UINT8 : { return _DEC_WS_FA7FD31C_Unsigned_Byte; }
			case MX_IT_INT16 : { return _DEC_WS_D4A628B5_Signed_Short; }
			case MX_IT_UINT16 : { return _DEC_WS_31443FFB_Unsigned_Short; }
			case MX_IT_INT32 : { return _DEC_WS_072A59C1_Signed_Int; }
			case MX_IT_UINT32 : { return _DEC_WS_7652642A_Unsigned_Int; }
			case MX_IT_INT64 : { return _DEC_WS_29277F4A_Signed_Int64; }
			case MX_IT_UINT64 : { return _DEC_WS_CCC56804_Unsigned_Int64; }
			case MX_IT_FLOAT : { return _DEC_WS_08647191_Float; }
			case MX_IT_DOUBLE : { return _DEC_WS_DD4BF7D9_Double; }
			case MX_IT_FLOAT16 : { return _DEC_WS_8337F554_Half_Float; }
			case MX_IT_VECTOR4 : { return _DEC_WS_06A07CB1_Vector4__Float_; }
			case MX_IT_STRING : { return _DEC_WS_9912B79F_String; }
			case MX_IT_STRING_UTF16 : { return _DEC_WS_A4CEA253_Unicode_String; }
			case MX_IT_STRING_UTF8 : { return _DEC_WS_463530AE_UTF_8_String; }
			case MX_IT_DOSDATE : { return _DEC_WS_11DA8664_DOSDATE; }
			case MX_IT_DOSTIME : { return _DEC_WS_D4D0295B_DOSTIME; }
			case MX_IT_FILETIME : { return _DEC_WS_CB8F936A_FILETIME; }
			case MX_IT_OLETIME : { return _DEC_WS_BFE36D2D_OLETIME; }
			case MX_IT_TIME_T : { return _DEC_WS_584829E3_time_t; }
			case MX_IT_TIME64_T : { return _DEC_WS_2AC7B303_time64_t; }
			case MX_IT_GUID : { return _DEC_WS_1DFA2206_GUID; }
			default : { return CSecureWString(); }
		}
	}

	/**
	 * Gets the size of a type in bytes.
	 *
	 * \param _itType The type.
	 * \return Returns the size of the type, or 0 for strings (their sizes depend on the data).
	 **/
	size_t CInspector::TypeSize( MX_INSPECTOR_TYPE _itType ) {
		switch ( _itType ) {
			case MX_IT_BINARY : {}								MX_FALLTHROUGH
			case MX_IT_INT8 : {}								MX_FALLTHROUGH
			case MX_IT_UINT8 : { return 1; }
			case MX_IT_INT16 : {}								MX_FALLTHROUGH
			case MX_IT_UINT16 : {}								MX_FALLTHROUGH
			case MX_IT_FLOAT16 : {}								MX_FALLTHROUGH
			case MX_IT_DOSDATE : {}								MX_FALLTHROUGH
			case MX_IT_DOSTIME : { return 2; }
			case MX_IT_INT32 : {}								MX_FALLTHROUGH
			case MX_IT_UINT32 : {}								MX_FALLTHROUGH
			case MX_IT_FLOAT : {}								MX_FALLTHROUGH
			case MX_IT_TIME_T : { return 4; }
			case MX_IT_INT64 : {}								MX_FALLTHROUGH
			case MX_IT_UINT64 : {}								MX_FALLTHROUGH
			case MX_IT_DOUBLE : {}								MX_FALLTHROUGH
			case MX_IT_FILETIME : {}							MX_FALLTHROUGH
			case MX_IT_OLETIME : {}								MX_FALLTHROUGH
			case MX_IT_TIME64_T : { return 8; }
			case MX_IT_VECTOR4 : {}								MX_FALLTHROUGH
			case MX_IT_GUID : { return 16; }
			default : { return 0; }
		}
	}

	/**
	 * Converts the data into the text of a type.
	 *
	 * \param _itType The type.
	 * \param _dData The data.
	 * \param _sSettings The settings (number, date, and time formats).
	 * \param _wsText Receives the text.
	 * \param _sBytes Receives the number of bytes the value uses (for strings, including the terminator if there is one).
	 * \return Returns false if there is not enough data or the data is not a valid value of the type (the value is shown empty).
	 **/
	bool CInspector::Format( MX_INSPECTOR_TYPE _itType, const MX_DATA &_dData, const MX_SETTINGS &_sSettings,
		std::wstring &_wsText, size_t &_sBytes ) {
		_wsText.clear();
		_sBytes = 0;
		if ( !_dData.pui8Data ) { return false; }
		try {
			const size_t sSize = TypeSize( _itType );
			if ( sSize ) {
				if ( _dData.sSize < sSize ) { return false; }
				_sBytes = sSize;
			}
			const uint8_t * pui8Data = _dData.pui8Data;
			const bool bBe = _dData.bBigEndian;
			switch ( _itType ) {
				case MX_IT_BINARY : {
					for ( uint32_t I = 8; I--; ) {
						_wsText.push_back( (pui8Data[0] & (1U << I)) ? L'1' : L'0' );
					}
					return true;
				}
				case MX_IT_INT8 : {}							MX_FALLTHROUGH
				case MX_IT_UINT8 : {}							MX_FALLTHROUGH
				case MX_IT_INT16 : {}							MX_FALLTHROUGH
				case MX_IT_UINT16 : {}							MX_FALLTHROUGH
				case MX_IT_INT32 : {}							MX_FALLTHROUGH
				case MX_IT_UINT32 : {}							MX_FALLTHROUGH
				case MX_IT_INT64 : {}							MX_FALLTHROUGH
				case MX_IT_UINT64 : {
					const uint64_t ui64Raw = ReadUnsigned( pui8Data, sSize, bBe );
					const bool bSigned = _itType == MX_IT_INT8 || _itType == MX_IT_INT16 || _itType == MX_IT_INT32 || _itType == MX_IT_INT64;
					if ( _sSettings.nfFormat == MX_NF_DECIMAL ) {
						if ( bSigned ) {
							// Sign-extend.
							const uint32_t ui32Shift = static_cast<uint32_t>(64 - sSize * 8);
							const int64_t i64Val = static_cast<int64_t>(ui64Raw << ui32Shift) >> ui32Shift;
							CUtilities::ToSigned( i64Val, _wsText );
						}
						else {
							CUtilities::ToUnsigned( ui64Raw, _wsText );
						}
					}
					else {
						// Other formats show the bits of the value (two's complement for negative values).
						FormatUnsigned( ui64Raw, _sSettings.nfFormat, _sSettings.hsHexStyle, _wsText,
							_sSettings.nfFormat == MX_NF_HEX ? static_cast<uint32_t>(sSize * 2) :
							_sSettings.nfFormat == MX_NF_BINARY ? static_cast<uint32_t>(sSize * 8) : 1 );
					}
					return true;
				}
				case MX_IT_FLOAT : {
					const uint32_t ui32Raw = static_cast<uint32_t>(ReadUnsigned( pui8Data, 4, bBe ));
					float fVal;
					std::memcpy( &fVal, &ui32Raw, sizeof( fVal ) );
					CUtilities::ToDouble( fVal, _wsText, -7 );
					return true;
				}
				case MX_IT_DOUBLE : {
					const uint64_t ui64Raw = ReadUnsigned( pui8Data, 8, bBe );
					double dVal;
					std::memcpy( &dVal, &ui64Raw, sizeof( dVal ) );
					CUtilities::ToDouble( dVal, _wsText, -15 );
					return true;
				}
				case MX_IT_FLOAT16 : {
					const uint16_t ui16Raw = static_cast<uint16_t>(ReadUnsigned( pui8Data, 2, bBe ));
					CUtilities::ToDouble( ee::CFloat16( ui16Raw ).Value(), _wsText, -7 );
					return true;
				}
				case MX_IT_VECTOR4 : {
					for ( size_t I = 0; I < 4; ++I ) {
						if ( I ) {
							_wsText.push_back( L',' );
							_wsText.push_back( L' ' );
						}
						const uint32_t ui32Raw = static_cast<uint32_t>(ReadUnsigned( pui8Data + I * 4, 4, bBe ));
						float fVal;
						std::memcpy( &fVal, &ui32Raw, sizeof( fVal ) );
						CUtilities::ToDouble( fVal, _wsText, -7 );
					}
					return true;
				}
				case MX_IT_STRING : {
					size_t I = 0;
					for ( ; I < _dData.sSize && I < MX_MAX_STRING_CHARS; ++I ) {
						if ( !pui8Data[I] ) {
							++I;
							break;
						}
						// Bytes are shown as Latin-1 (Windows-1252 without the extra characters).
						AppendShownChar( pui8Data[I], _wsText );
					}
					_sBytes = I;
					return true;
				}
				case MX_IT_STRING_UTF16 : {
					size_t I = 0;
					size_t sChars = 0;
					for ( ; I + 1 < _dData.sSize && sChars < MX_MAX_STRING_CHARS; I += 2, ++sChars ) {
						const uint32_t ui32Unit = static_cast<uint32_t>(ReadUnsigned( pui8Data + I, 2, bBe ));
						if ( !ui32Unit ) {
							I += 2;
							break;
						}
						if ( ui32Unit >= 0xD800 && ui32Unit <= 0xDBFF && I + 3 < _dData.sSize ) {
							const uint32_t ui32Low = static_cast<uint32_t>(ReadUnsigned( pui8Data + I + 2, 2, bBe ));
							if ( ui32Low >= 0xDC00 && ui32Low <= 0xDFFF ) {
								AppendShownChar( 0x10000 + ((ui32Unit - 0xD800) << 10) + (ui32Low - 0xDC00), _wsText );
								I += 2;
								continue;
							}
						}
						// Unpaired surrogates are shown as replacement characters.
						AppendShownChar( (ui32Unit >= 0xD800 && ui32Unit <= 0xDFFF) ? 0xFFFD : ui32Unit, _wsText );
					}
					_sBytes = I;
					return true;
				}
				case MX_IT_STRING_UTF8 : {
					size_t I = 0;
					size_t sChars = 0;
					while ( I < _dData.sSize && sChars < MX_MAX_STRING_CHARS ) {
						const uint8_t ui8Lead = pui8Data[I];
						if ( !ui8Lead ) {
							++I;
							break;
						}
						uint32_t ui32Char = 0xFFFD;
						size_t sLen = 1;
						size_t sNeed = 0;
						uint32_t ui32Min = 0;
						if ( ui8Lead < 0x80 ) { ui32Char = ui8Lead; }
						else if ( (ui8Lead & 0xE0) == 0xC0 ) { sNeed = 1; ui32Min = 0x80; ui32Char = ui8Lead & 0x1F; }
						else if ( (ui8Lead & 0xF0) == 0xE0 ) { sNeed = 2; ui32Min = 0x800; ui32Char = ui8Lead & 0x0F; }
						else if ( (ui8Lead & 0xF8) == 0xF0 ) { sNeed = 3; ui32Min = 0x10000; ui32Char = ui8Lead & 0x07; }
						else { ui32Char = 0xFFFD; }
						if ( sNeed ) {
							bool bValid = I + sNeed < _dData.sSize;
							for ( size_t J = 1; bValid && J <= sNeed; ++J ) {
								const uint8_t ui8Cont = pui8Data[I+J];
								if ( (ui8Cont & 0xC0) != 0x80 ) { bValid = false; }
								else { ui32Char = (ui32Char << 6) | (ui8Cont & 0x3F); }
							}
							if ( bValid && (ui32Char < ui32Min || ui32Char > 0x10FFFF || (ui32Char >= 0xD800 && ui32Char <= 0xDFFF)) ) { bValid = false; }
							if ( bValid ) { sLen = sNeed + 1; }
							else { ui32Char = 0xFFFD; }
						}
						AppendShownChar( ui32Char, _wsText );
						I += sLen;
						++sChars;
					}
					_sBytes = I;
					return true;
				}
				case MX_IT_DOSDATE : {
					const uint32_t ui32Raw = static_cast<uint32_t>(ReadUnsigned( pui8Data, 2, bBe ));
					SYSTEMTIME stTime = {};
					stTime.wDay = static_cast<WORD>(ui32Raw & 0x1F);
					stTime.wMonth = static_cast<WORD>((ui32Raw >> 5) & 0x0F);
					stTime.wYear = static_cast<WORD>((ui32Raw >> 9) + 1980);
					FILETIME ftCheck;
					if ( !::SystemTimeToFileTime( &stTime, &ftCheck ) ) { return false; }
					return FormatDateTime( stTime, true, false, _sSettings, _wsText );
				}
				case MX_IT_DOSTIME : {
					const uint32_t ui32Raw = static_cast<uint32_t>(ReadUnsigned( pui8Data, 2, bBe ));
					SYSTEMTIME stTime = {};
					stTime.wYear = 2000;
					stTime.wMonth = 1;
					stTime.wDay = 1;
					stTime.wSecond = static_cast<WORD>((ui32Raw & 0x1F) * 2);
					stTime.wMinute = static_cast<WORD>((ui32Raw >> 5) & 0x3F);
					stTime.wHour = static_cast<WORD>(ui32Raw >> 11);
					if ( stTime.wHour > 23 || stTime.wMinute > 59 || stTime.wSecond > 59 ) { return false; }
					return FormatDateTime( stTime, false, true, _sSettings, _wsText );
				}
				case MX_IT_FILETIME : {
					SYSTEMTIME stTime;
					if ( !FileTimeToLocal( ReadUnsigned( pui8Data, 8, bBe ), stTime ) ) { return false; }
					return FormatDateTime( stTime, true, true, _sSettings, _wsText );
				}
				case MX_IT_OLETIME : {
					const uint64_t ui64Raw = ReadUnsigned( pui8Data, 8, bBe );
					double dVal;
					std::memcpy( &dVal, &ui64Raw, sizeof( dVal ) );
					// Years 100 to 9999.
					if ( !std::isfinite( dVal ) || dVal < -657434.0 || dVal >= 2958466.0 ) { return false; }
					// The whole part is the day (negative before December 30, 1899) and the fraction is always the time of day.
					const double dWhole = std::trunc( dVal );
					const int64_t i64Ms = static_cast<int64_t>(dWhole) * 86400000LL + std::llround( std::fabs( dVal - dWhole ) * 86400000.0 );
					const int64_t i64Ms1601 = i64Ms + MX_OLE_EPOCH_MS;
					if ( i64Ms1601 < 0 ) { return false; }
					const uint64_t ui64FileTime = static_cast<uint64_t>(i64Ms1601) * 10000ULL;
					FILETIME ftTime = { static_cast<DWORD>(ui64FileTime), static_cast<DWORD>(ui64FileTime >> 32) };
					SYSTEMTIME stTime;
					// OLE dates have no time zone.
					if ( !::FileTimeToSystemTime( &ftTime, &stTime ) ) { return false; }
					return FormatDateTime( stTime, true, true, _sSettings, _wsText );
				}
				case MX_IT_TIME_T : {}							MX_FALLTHROUGH
				case MX_IT_TIME64_T : {
					const uint64_t ui64Raw = ReadUnsigned( pui8Data, sSize, bBe );
					const int64_t i64Seconds = (_itType == MX_IT_TIME_T) ? static_cast<int64_t>(static_cast<int32_t>(ui64Raw)) : static_cast<int64_t>(ui64Raw);
					if ( i64Seconds < -MX_UNIX_EPOCH_SECONDS || i64Seconds > MX_MAX_FILETIME_SECONDS - MX_UNIX_EPOCH_SECONDS ) { return false; }
					SYSTEMTIME stTime;
					if ( !FileTimeToLocal( static_cast<uint64_t>(i64Seconds + MX_UNIX_EPOCH_SECONDS) * 10000000ULL, stTime ) ) { return false; }
					return FormatDateTime( stTime, true, true, _sSettings, _wsText );
				}
				case MX_IT_GUID : {
					_wsText.push_back( L'{' );
					FormatUnsigned( ReadUnsigned( pui8Data, 4, bBe ), MX_NF_HEX, MX_HS_NONE, _wsText, 8 );
					_wsText.push_back( L'-' );
					FormatUnsigned( ReadUnsigned( pui8Data + 4, 2, bBe ), MX_NF_HEX, MX_HS_NONE, _wsText, 4 );
					_wsText.push_back( L'-' );
					FormatUnsigned( ReadUnsigned( pui8Data + 6, 2, bBe ), MX_NF_HEX, MX_HS_NONE, _wsText, 4 );
					_wsText.push_back( L'-' );
					for ( size_t I = 8; I < 16; ++I ) {
						if ( I == 10 ) { _wsText.push_back( L'-' ); }
						FormatUnsigned( pui8Data[I], MX_NF_HEX, MX_HS_NONE, _wsText, 2 );
					}
					_wsText.push_back( L'}' );
					return true;
				}
				default : { return false; }
			}
		}
		catch ( ... ) {
			_wsText.clear();
			return false;
		}
	}

	/**
	 * Gets the value of an integer type as an unsigned number (used to offer "Go to Address").
	 *
	 * \param _itType The type.
	 * \param _dData The data.
	 * \param _ui64Value Receives the value.
	 * \return Returns true if the type is an integer type (including Binary), there is enough data, and the value is not negative.
	 **/
	bool CInspector::UnsignedValue( MX_INSPECTOR_TYPE _itType, const MX_DATA &_dData, uint64_t &_ui64Value ) {
		if ( !IsInteger( _itType ) && _itType != MX_IT_BINARY ) { return false; }
		const size_t sSize = TypeSize( _itType );
		if ( !_dData.pui8Data || _dData.sSize < sSize ) { return false; }
		_ui64Value = ReadUnsigned( _dData.pui8Data, sSize, _dData.bBigEndian );
		const bool bSigned = _itType == MX_IT_INT8 || _itType == MX_IT_INT16 || _itType == MX_IT_INT32 || _itType == MX_IT_INT64;
		if ( bSigned && (_ui64Value & (1ULL << (sSize * 8 - 1))) ) { return false; }
		return true;
	}

	/**
	 * Converts text typed for a type into bytes.
	 *
	 * \param _itType The type.
	 * \param _wsText The text typed.
	 * \param _bBigEndian Are values written big-endian?
	 * \param _sSettings The settings (number, date, and time formats).
	 * \param _vBytes Receives the bytes.  For strings this includes the terminator.
	 * \param _swsError Receives the reason on failure.
	 * \return Returns true if the text was converted.
	 **/
	bool CInspector::Parse( MX_INSPECTOR_TYPE _itType, const std::wstring &_wsText, bool _bBigEndian,
		const MX_SETTINGS &_sSettings, std::vector<uint8_t> &_vBytes, CSecureWString &_swsError ) {
		_vBytes.clear();
		try {
			// Surrounding spaces are ignored except in strings.
			std::wstring wsTrimmed = _wsText;
			if ( !IsString( _itType ) ) {
				while ( wsTrimmed.size() && ::iswspace( wsTrimmed.back() ) ) { wsTrimmed.pop_back(); }
				size_t sStart = 0;
				while ( sStart < wsTrimmed.size() && ::iswspace( wsTrimmed[sStart] ) ) { ++sStart; }
				wsTrimmed.erase( 0, sStart );
				if ( wsTrimmed.empty() ) {
					_swsError = _DEC_WS_DB7538F7_Enter_a_value_;
					return false;
				}
			}

			switch ( _itType ) {
				case MX_IT_BINARY : {}							MX_FALLTHROUGH
				case MX_IT_INT8 : {}							MX_FALLTHROUGH
				case MX_IT_UINT8 : {}							MX_FALLTHROUGH
				case MX_IT_INT16 : {}							MX_FALLTHROUGH
				case MX_IT_UINT16 : {}							MX_FALLTHROUGH
				case MX_IT_INT32 : {}							MX_FALLTHROUGH
				case MX_IT_UINT32 : {}							MX_FALLTHROUGH
				case MX_IT_INT64 : {}							MX_FALLTHROUGH
				case MX_IT_UINT64 : {
					const size_t sSize = TypeSize( _itType );
					uint64_t ui64Bits = 0;
					bool bHaveBits = false;

					// Binary digits (the Binary row always shows them; "0b" may start them).
					{
						size_t sStart = 0;
						if ( wsTrimmed.size() > 2 && wsTrimmed[0] == L'0' && (wsTrimmed[1] == L'b' || wsTrimmed[1] == L'B') ) { sStart = 2; }
						bool bAllBits = (sStart != 0 || _itType == MX_IT_BINARY) && wsTrimmed.size() > sStart;
						for ( size_t I = sStart; bAllBits && I < wsTrimmed.size(); ++I ) {
							if ( wsTrimmed[I] != L'0' && wsTrimmed[I] != L'1' ) { bAllBits = false; }
						}
						if ( bAllBits ) {
							if ( wsTrimmed.size() - sStart > sSize * 8 ) {
								_swsError = _DEC_WS_4C21E26F_The_value_is_out_of_range_;
								return false;
							}
							for ( size_t I = sStart; I < wsTrimmed.size(); ++I ) {
								ui64Bits = (ui64Bits << 1) | ((wsTrimmed[I] == L'1') ? 1 : 0);
							}
							bHaveBits = true;
						}
					}

					// Hexadecimal with an "h" suffix.
					if ( !bHaveBits && wsTrimmed.size() > 1 && (wsTrimmed.back() == L'h' || wsTrimmed.back() == L'H') ) {
						bool bAllHex = wsTrimmed.size() - 1 <= 16;
						uint64_t ui64Val = 0;
						for ( size_t I = 0; bAllHex && I + 1 < wsTrimmed.size(); ++I ) {
							const wchar_t wcThis = wsTrimmed[I];
							uint32_t ui32Digit;
							if ( wcThis >= L'0' && wcThis <= L'9' ) { ui32Digit = wcThis - L'0'; }
							else if ( wcThis >= L'a' && wcThis <= L'f' ) { ui32Digit = wcThis - L'a' + 10; }
							else if ( wcThis >= L'A' && wcThis <= L'F' ) { ui32Digit = wcThis - L'A' + 10; }
							else { bAllHex = false; break; }
							ui64Val = (ui64Val << 4) | ui32Digit;
						}
						if ( bAllHex ) {
							if ( sSize < 8 && (ui64Val >> (sSize * 8)) ) {
								_swsError = _DEC_WS_4C21E26F_The_value_is_out_of_range_;
								return false;
							}
							ui64Bits = ui64Val;
							bHaveBits = true;
						}
					}

					if ( !bHaveBits ) {
						ee::CExpEvalContainer::EE_RESULT rRes;
						if ( !Evaluate( wsTrimmed, _sSettings.nfFormat == MX_NF_HEX && _itType != MX_IT_BINARY, rRes ) ) {
							_swsError = _DEC_WS_AED44EA4_Invalid_value_;
							return false;
						}
						// The value can be given as signed or unsigned.
						const double dMinSigned = -std::ldexp( 1.0, static_cast<int>(sSize * 8 - 1) );
						const double dMaxUnsigned = std::ldexp( 1.0, static_cast<int>(sSize * 8) ) - 1.0;
						switch ( rRes.ncType ) {
							case ee::EE_NC_SIGNED : {
								if ( sSize < 8 && (static_cast<double>(rRes.u.i64Val) < dMinSigned || static_cast<double>(rRes.u.i64Val) > dMaxUnsigned) ) {
									_swsError = _DEC_WS_4C21E26F_The_value_is_out_of_range_;
									return false;
								}
								ui64Bits = static_cast<uint64_t>(rRes.u.i64Val);
								break;
							}
							case ee::EE_NC_UNSIGNED : {
								if ( sSize < 8 && (rRes.u.ui64Val >> (sSize * 8)) ) {
									_swsError = _DEC_WS_4C21E26F_The_value_is_out_of_range_;
									return false;
								}
								ui64Bits = rRes.u.ui64Val;
								break;
							}
							case ee::EE_NC_FLOATING : {
								const double dVal = rRes.u.dVal;
								if ( !std::isfinite( dVal ) || dVal != std::trunc( dVal ) ) {
									_swsError = _DEC_WS_FC3F0A05_Enter_a_whole_number_;
									return false;
								}
								if ( dVal < dMinSigned || dVal >= std::ldexp( 1.0, static_cast<int>(sSize * 8) ) ) {
									_swsError = _DEC_WS_4C21E26F_The_value_is_out_of_range_;
									return false;
								}
								ui64Bits = (dVal < 0.0) ? static_cast<uint64_t>(static_cast<int64_t>(dVal)) :
									(dVal >= 9223372036854775808.0) ? static_cast<uint64_t>(dVal - 9223372036854775808.0) + 0x8000000000000000ULL :
									static_cast<uint64_t>(dVal);
								break;
							}
							default : {
								_swsError = _DEC_WS_AED44EA4_Invalid_value_;
								return false;
							}
						}
					}
					WriteUnsigned( ui64Bits, sSize, _bBigEndian, _vBytes );
					return true;
				}
				case MX_IT_FLOAT : {}							MX_FALLTHROUGH
				case MX_IT_DOUBLE : {}							MX_FALLTHROUGH
				case MX_IT_FLOAT16 : {}							MX_FALLTHROUGH
				case MX_IT_VECTOR4 : {
					// A Vector4 is 4 numbers separated by commas or semicolons (or by spaces if there are no commas or semicolons).
					std::vector<std::wstring> vParts;
					if ( _itType == MX_IT_VECTOR4 ) {
						const bool bCommas = wsTrimmed.find( L',' ) != std::wstring::npos || wsTrimmed.find( L';' ) != std::wstring::npos;
						std::wstring wsCur;
						for ( wchar_t wcThis : wsTrimmed ) {
							const bool bSep = bCommas ? (wcThis == L',' || wcThis == L';') : (::iswspace( wcThis ) != 0);
							if ( bSep ) {
								if ( !bCommas && wsCur.empty() ) { continue; }
								vParts.push_back( wsCur );
								wsCur.clear();
							}
							else { wsCur.push_back( wcThis ); }
						}
						vParts.push_back( wsCur );
						if ( vParts.size() != 4 ) {
							_swsError = _DEC_WS_4AAE8501_Enter_4_numbers_separated_by_commas_;
							return false;
						}
					}
					else {
						vParts.push_back( wsTrimmed );
					}
					for ( const auto & wsPart : vParts ) {
						ee::CExpEvalContainer::EE_RESULT rRes;
						if ( !Evaluate( wsPart, false, rRes ) ) {
							_swsError = _DEC_WS_AED44EA4_Invalid_value_;
							return false;
						}
						const double dVal = ee::CExpEvalContainer::ConvertResult( rRes, ee::EE_NC_FLOATING ).u.dVal;
						switch ( _itType ) {
							case MX_IT_DOUBLE : {
								uint64_t ui64Raw;
								std::memcpy( &ui64Raw, &dVal, sizeof( ui64Raw ) );
								WriteUnsigned( ui64Raw, 8, _bBigEndian, _vBytes );
								break;
							}
							case MX_IT_FLOAT16 : {
								WriteUnsigned( ee::CFloat16( dVal ).RawValue(), 2, _bBigEndian, _vBytes );
								break;
							}
							default : {
								const float fVal = static_cast<float>(dVal);
								uint32_t ui32Raw;
								std::memcpy( &ui32Raw, &fVal, sizeof( ui32Raw ) );
								WriteUnsigned( ui32Raw, 4, _bBigEndian, _vBytes );
							}
						}
					}
					return true;
				}
				case MX_IT_STRING : {
					for ( wchar_t wcThis : _wsText ) {
						if ( !wcThis || static_cast<uint32_t>(wcThis) > 0xFF ) {
							_swsError = _DEC_WS_60DF53B9_The_text_has_characters_that_cannot_be_stored_in_an_ASCII_string_;
							_vBytes.clear();
							return false;
						}
						_vBytes.push_back( static_cast<uint8_t>(wcThis) );
					}
					_vBytes.push_back( 0 );
					return true;
				}
				case MX_IT_STRING_UTF16 : {
					for ( wchar_t wcThis : _wsText ) {
						WriteUnsigned( static_cast<uint16_t>(wcThis), 2, _bBigEndian, _vBytes );
					}
					WriteUnsigned( 0, 2, _bBigEndian, _vBytes );
					return true;
				}
				case MX_IT_STRING_UTF8 : {
					const std::string sUtf8 = ee::CExpEval::ToUtf8( _wsText );
					_vBytes.assign( sUtf8.begin(), sUtf8.end() );
					_vBytes.push_back( 0 );
					return true;
				}
				case MX_IT_DOSDATE : {
					SYSTEMTIME stTime = {};
					if ( !ParseDateTime( wsTrimmed, true, false, _sSettings, stTime ) || stTime.wYear < 1980 || stTime.wYear > 1980 + 127 ) {
						_swsError = _DEC_WS_EE9D3790_Invalid_date_or_time_;
						return false;
					}
					WriteUnsigned( ((stTime.wYear - 1980) << 9) | (stTime.wMonth << 5) | stTime.wDay, 2, _bBigEndian, _vBytes );
					return true;
				}
				case MX_IT_DOSTIME : {
					SYSTEMTIME stTime = {};
					stTime.wYear = 2000;
					stTime.wMonth = 1;
					stTime.wDay = 1;
					if ( !ParseDateTime( wsTrimmed, false, true, _sSettings, stTime ) ) {
						_swsError = _DEC_WS_EE9D3790_Invalid_date_or_time_;
						return false;
					}
					WriteUnsigned( (stTime.wHour << 11) | (stTime.wMinute << 5) | (stTime.wSecond / 2), 2, _bBigEndian, _vBytes );
					return true;
				}
				case MX_IT_FILETIME : {}						MX_FALLTHROUGH
				case MX_IT_TIME_T : {}							MX_FALLTHROUGH
				case MX_IT_TIME64_T : {
					SYSTEMTIME stTime = {};
					uint64_t ui64FileTime;
					if ( !ParseDateTime( wsTrimmed, true, true, _sSettings, stTime ) || !LocalToFileTime( stTime, ui64FileTime ) ) {
						_swsError = _DEC_WS_EE9D3790_Invalid_date_or_time_;
						return false;
					}
					if ( _itType == MX_IT_FILETIME ) {
						WriteUnsigned( ui64FileTime, 8, _bBigEndian, _vBytes );
						return true;
					}
					const int64_t i64Seconds = static_cast<int64_t>(ui64FileTime / 10000000ULL) - MX_UNIX_EPOCH_SECONDS;
					if ( _itType == MX_IT_TIME_T && (i64Seconds < INT32_MIN || i64Seconds > INT32_MAX) ) {
						_swsError = _DEC_WS_4C21E26F_The_value_is_out_of_range_;
						return false;
					}
					WriteUnsigned( static_cast<uint64_t>(i64Seconds), TypeSize( _itType ), _bBigEndian, _vBytes );
					return true;
				}
				case MX_IT_OLETIME : {
					SYSTEMTIME stTime = {};
					FILETIME ftTime;
					if ( !ParseDateTime( wsTrimmed, true, true, _sSettings, stTime ) || !::SystemTimeToFileTime( &stTime, &ftTime ) ) {
						_swsError = _DEC_WS_EE9D3790_Invalid_date_or_time_;
						return false;
					}
					const uint64_t ui64FileTime = (static_cast<uint64_t>(ftTime.dwHighDateTime) << 32) | ftTime.dwLowDateTime;
					const int64_t i64Ms = static_cast<int64_t>(ui64FileTime / 10000ULL) - MX_OLE_EPOCH_MS;
					// Before December 30, 1899 the whole part counts back and the fraction still counts forward (-1.25 is 6 AM, December 29, 1899).
					int64_t i64Days = i64Ms / 86400000LL;
					int64_t i64DayMs = i64Ms % 86400000LL;
					if ( i64DayMs < 0 ) {
						--i64Days;
						i64DayMs += 86400000LL;
					}
					const double dFrac = static_cast<double>(i64DayMs) / 86400000.0;
					const double dVal = (i64Days < 0) ? static_cast<double>(i64Days) - dFrac : static_cast<double>(i64Days) + dFrac;
					uint64_t ui64Raw;
					std::memcpy( &ui64Raw, &dVal, sizeof( ui64Raw ) );
					WriteUnsigned( ui64Raw, 8, _bBigEndian, _vBytes );
					return true;
				}
				case MX_IT_GUID : {
					uint8_t ui8Digits[32];
					size_t sDigits = 0;
					for ( wchar_t wcThis : wsTrimmed ) {
						uint32_t ui32Digit;
						if ( wcThis >= L'0' && wcThis <= L'9' ) { ui32Digit = wcThis - L'0'; }
						else if ( wcThis >= L'a' && wcThis <= L'f' ) { ui32Digit = wcThis - L'a' + 10; }
						else if ( wcThis >= L'A' && wcThis <= L'F' ) { ui32Digit = wcThis - L'A' + 10; }
						else if ( wcThis == L'{' || wcThis == L'}' || wcThis == L'-' || ::iswspace( wcThis ) ) { continue; }
						else { sDigits = 0; break; }
						if ( sDigits == 32 ) { sDigits = 33; break; }
						ui8Digits[sDigits++] = static_cast<uint8_t>(ui32Digit);
					}
					if ( sDigits != 32 ) {
						_swsError = _DEC_WS_B092065A_Enter_32_hexadecimal_digits_;
						return false;
					}
					auto Digits = [&]( size_t _sFirst, size_t _sCount ) {
						uint64_t ui64Val = 0;
						for ( size_t I = 0; I < _sCount; ++I ) { ui64Val = (ui64Val << 4) | ui8Digits[_sFirst+I]; }
						return ui64Val;
					};
					WriteUnsigned( Digits( 0, 8 ), 4, _bBigEndian, _vBytes );
					WriteUnsigned( Digits( 8, 4 ), 2, _bBigEndian, _vBytes );
					WriteUnsigned( Digits( 12, 4 ), 2, _bBigEndian, _vBytes );
					for ( size_t I = 16; I < 32; I += 2 ) {
						_vBytes.push_back( static_cast<uint8_t>(Digits( I, 2 )) );
					}
					return true;
				}
				default : {
					_swsError = _DEC_WS_AED44EA4_Invalid_value_;
					return false;
				}
			}
		}
		catch ( ... ) {
			_vBytes.clear();
			_swsError = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * Formats an unsigned number in a given number format.
	 *
	 * \param _ui64Value The value.
	 * \param _nfFormat The number format.
	 * \param _hsHexStyle How hexadecimal numbers are written.
	 * \param _wsText Receives the text (appended).
	 * \param _ui32Digits The fewest digits to write (hexadecimal and binary are padded with 0's).
	 **/
	void CInspector::FormatUnsigned( uint64_t _ui64Value, MX_NUMBER_FORMAT _nfFormat, MX_HEX_STYLE _hsHexStyle, std::wstring &_wsText, uint32_t _ui32Digits ) {
		switch ( _nfFormat ) {
			case MX_NF_HEX : {
				if ( _hsHexStyle == MX_HS_PREFIX_0X ) {
					_wsText.push_back( L'0' );
					_wsText.push_back( L'x' );
				}
				uint32_t ui32Digits = 1;
				while ( ui32Digits < 16 && (_ui64Value >> (ui32Digits * 4)) ) { ++ui32Digits; }
				ui32Digits = std::max( ui32Digits, std::min( _ui32Digits, 16U ) );
				for ( uint32_t I = ui32Digits; I--; ) {
					const uint32_t ui32Digit = static_cast<uint32_t>((_ui64Value >> (I * 4)) & 0xF);
					_wsText.push_back( static_cast<wchar_t>(ui32Digit < 10 ? (L'0' + ui32Digit) : (L'A' + ui32Digit - 10)) );
				}
				if ( _hsHexStyle == MX_HS_SUFFIX_H ) { _wsText.push_back( L'h' ); }
				break;
			}
			case MX_NF_OCTAL : {
				CUtilities::ToOct( _ui64Value, _wsText, 1 );
				break;
			}
			case MX_NF_BINARY : {
				CUtilities::ToBinary( _ui64Value, _wsText, std::max( _ui32Digits, 1U ) );
				break;
			}
			default : {
				CUtilities::ToUnsigned( _ui64Value, _wsText );
			}
		}
	}

	/**
	 * Reads a value of up to 8 bytes in the given byte order.
	 *
	 * \param _pui8Data The bytes.
	 * \param _sSize The number of bytes to read (1 to 8).
	 * \param _bBigEndian Are the bytes big-endian?
	 * \return Returns the value.
	 **/
	uint64_t CInspector::ReadUnsigned( const uint8_t * _pui8Data, size_t _sSize, bool _bBigEndian ) {
		uint64_t ui64Val = 0;
		for ( size_t I = 0; I < _sSize && I < 8; ++I ) {
			const uint64_t ui64Byte = _pui8Data[_bBigEndian ? I : (_sSize - 1 - I)];
			ui64Val = (ui64Val << 8) | ui64Byte;
		}
		return ui64Val;
	}

	/**
	 * Appends a value of up to 8 bytes in the given byte order.
	 *
	 * \param _ui64Value The value.
	 * \param _sSize The number of bytes to write (1 to 8).
	 * \param _bBigEndian Are the bytes big-endian?
	 * \param _vBytes Receives the bytes (appended).
	 **/
	void CInspector::WriteUnsigned( uint64_t _ui64Value, size_t _sSize, bool _bBigEndian, std::vector<uint8_t> &_vBytes ) {
		for ( size_t I = 0; I < _sSize && I < 8; ++I ) {
			const size_t sShift = _bBigEndian ? (_sSize - 1 - I) : I;
			_vBytes.push_back( static_cast<uint8_t>(_ui64Value >> (sShift * 8)) );
		}
	}

	/**
	 * Formats a date and/or time.
	 *
	 * \param _stTime The date and time.
	 * \param _bDate Include the date?
	 * \param _bTime Include the time?
	 * \param _sSettings The settings (date and time formats).
	 * \param _wsText Receives the text.
	 * \return Returns false if the date or time could not be formatted.
	 **/
	bool CInspector::FormatDateTime( const SYSTEMTIME &_stTime, bool _bDate, bool _bTime, const MX_SETTINGS &_sSettings,
		std::wstring &_wsText ) {
		wchar_t wszBuffer[128];
		if ( _bDate ) {
			const int iLen = ::GetDateFormatEx( LOCALE_NAME_USER_DEFAULT, _sSettings.wsDateFormat.empty() ? DATE_SHORTDATE : 0, &_stTime,
				_sSettings.wsDateFormat.empty() ? nullptr : _sSettings.wsDateFormat.c_str(), wszBuffer, static_cast<int>(std::size( wszBuffer )), nullptr );
			if ( iLen <= 0 ) { return false; }
			_wsText.append( wszBuffer );
		}
		if ( _bTime ) {
			const int iLen = ::GetTimeFormatEx( LOCALE_NAME_USER_DEFAULT, 0, &_stTime,
				_sSettings.wsTimeFormat.empty() ? nullptr : _sSettings.wsTimeFormat.c_str(), wszBuffer, static_cast<int>(std::size( wszBuffer )) );
			if ( iLen <= 0 ) { return false; }
			if ( _bDate ) { _wsText.push_back( L' ' ); }
			_wsText.append( wszBuffer );
		}
		return true;
	}

	/**
	 * Reads a date and/or time typed in the date and time formats of the settings.  Numbers are read in the order of the fields of
	 *	the formats; month names (full or abbreviated) are accepted where the format shows them, and AM/PM where the format has it.
	 *
	 * \param _wsText The text.
	 * \param _bDate Is a date expected?
	 * \param _bTime Is a time expected?
	 * \param _sSettings The settings (date and time formats).
	 * \param _stTime Receives the date and time.  Fields that are not expected are left as they are.
	 * \return Returns true if the text was read and is a valid date and time.
	 **/
	bool CInspector::ParseDateTime( const std::wstring &_wsText, bool _bDate, bool _bTime, const MX_SETTINGS &_sSettings,
		SYSTEMTIME &_stTime ) {
		// The formats (the user's formats if none are set).
		auto LocaleText = []( LCTYPE _lcType ) {
			wchar_t wszBuffer[128];
			const int iLen = ::GetLocaleInfoEx( LOCALE_NAME_USER_DEFAULT, _lcType, wszBuffer, static_cast<int>(std::size( wszBuffer )) );
			return (iLen > 0) ? std::wstring( wszBuffer ) : std::wstring();
		};
		std::wstring wsPicture;
		if ( _bDate ) { wsPicture += _sSettings.wsDateFormat.empty() ? LocaleText( LOCALE_SSHORTDATE ) : _sSettings.wsDateFormat; }
		if ( _bTime ) {
			if ( _bDate ) { wsPicture.push_back( L' ' ); }
			wsPicture += _sSettings.wsTimeFormat.empty() ? LocaleText( LOCALE_STIMEFORMAT ) : _sSettings.wsTimeFormat;
		}

		// The fields of the formats.
		struct MX_FIELD {
			wchar_t															wcField;
			size_t															sCount;
		};
		std::vector<MX_FIELD> vFields;
		for ( size_t I = 0; I < wsPicture.size(); ) {
			const wchar_t wcThis = wsPicture[I];
			if ( wcThis == L'\'' ) {
				// Quoted text.
				const size_t sEnd = wsPicture.find( L'\'', I + 1 );
				I = (sEnd == std::wstring::npos) ? wsPicture.size() : sEnd + 1;
				continue;
			}
			if ( wcThis == L'y' || wcThis == L'M' || wcThis == L'd' || wcThis == L'H' || wcThis == L'h' || wcThis == L'm' ||
				wcThis == L's' || wcThis == L't' || wcThis == L'g' ) {
				size_t sCount = 0;
				while ( I < wsPicture.size() && wsPicture[I] == wcThis ) {
					++sCount;
					++I;
				}
				vFields.push_back( MX_FIELD{ wcThis, sCount } );
				continue;
			}
			++I;
		}

		// The numbers and words typed.
		struct MX_TOKEN {
			bool															bNumber;
			uint32_t														ui32Value;
			std::wstring													wsWord;
		};
		std::vector<MX_TOKEN> vTokens;
		for ( size_t I = 0; I < _wsText.size(); ) {
			const wchar_t wcThis = _wsText[I];
			if ( wcThis >= L'0' && wcThis <= L'9' ) {
				uint32_t ui32Val = 0;
				size_t sDigits = 0;
				while ( I < _wsText.size() && _wsText[I] >= L'0' && _wsText[I] <= L'9' ) {
					if ( ++sDigits > 9 ) { return false; }
					ui32Val = ui32Val * 10 + (_wsText[I] - L'0');
					++I;
				}
				vTokens.push_back( MX_TOKEN{ true, ui32Val, std::wstring() } );
				continue;
			}
			if ( ::iswalpha( wcThis ) ) {
				std::wstring wsWord;
				while ( I < _wsText.size() && ::iswalpha( _wsText[I] ) ) { wsWord.push_back( _wsText[I++] ); }
				vTokens.push_back( MX_TOKEN{ false, 0, wsWord } );
				continue;
			}
			++I;
		}

		auto SameText = []( const std::wstring &_wsLeft, const std::wstring &_wsRight ) {
			return !_wsLeft.empty() && ::CompareStringEx( LOCALE_NAME_USER_DEFAULT, NORM_IGNORECASE, _wsLeft.c_str(), static_cast<int>(_wsLeft.size()),
				_wsRight.c_str(), static_cast<int>(_wsRight.size()), nullptr, nullptr, 0 ) == CSTR_EQUAL;
		};

		size_t sToken = 0;
		uint32_t ui32Year = _stTime.wYear, ui32Month = _stTime.wMonth, ui32Day = _stTime.wDay;
		uint32_t ui32Hour = 0, ui32Minute = 0, ui32Second = 0;
		bool bTwelveHour = false, bPm = false, bHaveAmPm = false;
		for ( const auto & fField : vFields ) {
			const bool bNextIsNumber = sToken < vTokens.size() && vTokens[sToken].bNumber;
			const bool bNextIsWord = sToken < vTokens.size() && !vTokens[sToken].bNumber;
			switch ( fField.wcField ) {
				case L'y' : {
					if ( !bNextIsNumber ) { return false; }
					ui32Year = vTokens[sToken++].ui32Value;
					// 2-digit years: 1950 to 2049.
					if ( fField.sCount <= 2 && ui32Year < 100 ) { ui32Year += (ui32Year < 50) ? 2000 : 1900; }
					break;
				}
				case L'M' : {
					if ( bNextIsNumber ) {
						ui32Month = vTokens[sToken++].ui32Value;
						break;
					}
					if ( !bNextIsWord ) { return false; }
					uint32_t ui32Found = 0;
					for ( uint32_t I = 0; I < 12 && !ui32Found; ++I ) {
						if ( SameText( vTokens[sToken].wsWord, LocaleText( LOCALE_SMONTHNAME1 + I ) ) ||
							SameText( vTokens[sToken].wsWord, LocaleText( LOCALE_SABBREVMONTHNAME1 + I ) ) ) {
							ui32Found = I + 1;
						}
					}
					if ( !ui32Found ) { return false; }
					ui32Month = ui32Found;
					++sToken;
					break;
				}
				case L'd' : {
					if ( fField.sCount >= 3 ) {
						// The name of the day is not needed.
						if ( bNextIsWord ) { ++sToken; }
						break;
					}
					if ( !bNextIsNumber ) { return false; }
					ui32Day = vTokens[sToken++].ui32Value;
					break;
				}
				case L'h' : {}									MX_FALLTHROUGH
				case L'H' : {
					if ( !bNextIsNumber ) { return false; }
					ui32Hour = vTokens[sToken++].ui32Value;
					bTwelveHour = fField.wcField == L'h';
					break;
				}
				case L'm' : {
					if ( !bNextIsNumber ) { return false; }
					ui32Minute = vTokens[sToken++].ui32Value;
					break;
				}
				case L's' : {
					if ( !bNextIsNumber ) { return false; }
					ui32Second = vTokens[sToken++].ui32Value;
					break;
				}
				case L't' : {
					if ( bNextIsWord ) {
						const std::wstring wsPm = LocaleText( LOCALE_S2359 );
						const std::wstring wsAm = LocaleText( LOCALE_S1159 );
						const std::wstring & wsWord = vTokens[sToken].wsWord;
						if ( SameText( wsWord, wsPm ) || (wsPm.size() && SameText( wsWord, wsPm.substr( 0, 1 ) )) ) {
							bPm = true;
							bHaveAmPm = true;
						}
						else if ( SameText( wsWord, wsAm ) || (wsAm.size() && SameText( wsWord, wsAm.substr( 0, 1 ) )) ) {
							bPm = false;
							bHaveAmPm = true;
						}
						else { return false; }
						++sToken;
					}
					break;
				}
				case L'g' : {
					if ( bNextIsWord ) { ++sToken; }
					break;
				}
			}
		}
		// Everything typed must have been used.
		if ( sToken != vTokens.size() ) { return false; }

		if ( bTwelveHour && bHaveAmPm ) {
			if ( ui32Hour < 1 || ui32Hour > 12 ) { return false; }
			ui32Hour %= 12;
			if ( bPm ) { ui32Hour += 12; }
		}
		if ( ui32Hour > 23 || ui32Minute > 59 || ui32Second > 59 ) { return false; }

		SYSTEMTIME stTime = _stTime;
		if ( _bDate ) {
			if ( ui32Year > 30827 || ui32Month < 1 || ui32Month > 12 || ui32Day < 1 || ui32Day > 31 ) { return false; }
			stTime.wYear = static_cast<WORD>(ui32Year);
			stTime.wMonth = static_cast<WORD>(ui32Month);
			stTime.wDay = static_cast<WORD>(ui32Day);
		}
		if ( _bTime ) {
			stTime.wHour = static_cast<WORD>(ui32Hour);
			stTime.wMinute = static_cast<WORD>(ui32Minute);
			stTime.wSecond = static_cast<WORD>(ui32Second);
			stTime.wMilliseconds = 0;
		}
		stTime.wDayOfWeek = 0;
		// Checks that the day exists in the month.
		FILETIME ftCheck;
		if ( !::SystemTimeToFileTime( &stTime, &ftCheck ) ) { return false; }
		_stTime = stTime;
		return true;
	}

	/**
	 * Converts a FILETIME value in UTC to local time.
	 *
	 * \param _ui64FileTime The FILETIME value.
	 * \param _stLocal Receives the local time.
	 * \return Returns false if the value is out of range.
	 **/
	bool CInspector::FileTimeToLocal( uint64_t _ui64FileTime, SYSTEMTIME &_stLocal ) {
		if ( _ui64FileTime > 0x7FFFFFFFFFFFFFFFULL ) { return false; }
		FILETIME ftTime = { static_cast<DWORD>(_ui64FileTime), static_cast<DWORD>(_ui64FileTime >> 32) };
		SYSTEMTIME stUtc;
		if ( !::FileTimeToSystemTime( &ftTime, &stUtc ) ) { return false; }
		if ( !::SystemTimeToTzSpecificLocalTime( nullptr, &stUtc, &_stLocal ) ) { return false; }
		return true;
	}

	/**
	 * Converts a local time to a FILETIME value in UTC.
	 *
	 * \param _stLocal The local time.
	 * \param _ui64FileTime Receives the FILETIME value.
	 * \return Returns false if the time is invalid.
	 **/
	bool CInspector::LocalToFileTime( const SYSTEMTIME &_stLocal, uint64_t &_ui64FileTime ) {
		SYSTEMTIME stUtc;
		if ( !::TzSpecificLocalTimeToSystemTime( nullptr, &_stLocal, &stUtc ) ) { return false; }
		FILETIME ftTime;
		if ( !::SystemTimeToFileTime( &stUtc, &ftTime ) ) { return false; }
		_ui64FileTime = (static_cast<uint64_t>(ftTime.dwHighDateTime) << 32) | ftTime.dwLowDateTime;
		return true;
	}

	/**
	 * Evaluates text as an expression.
	 *
	 * \param _wsText The text.
	 * \param _bHex Are numbers without a prefix or suffix hexadecimal?
	 * \param _rResult Receives the result.
	 * \return Returns true if the text is a valid expression with a numeric result.
	 **/
	bool CInspector::Evaluate( const std::wstring &_wsText, bool _bHex, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		_rResult.ncType = ee::EE_NC_INVALID;
		try {
			const std::string sUtf8 = ee::CExpEval::ToUtf8( _wsText );
			if ( sUtf8.empty() ) { return false; }
			lsw::CExpression eExp;
			eExp.SetTreatsAsHex( _bHex );
			if ( !eExp.SetExpression( sUtf8.c_str() ) || !eExp.GetContainer() ) { return false; }
			if ( !eExp.GetContainer()->Resolve( _rResult ) ) { return false; }
			if ( _rResult.ncType == ee::EE_NC_OBJECT ) {
				// Objects (strings, arrays, etc.) belong to the container.
				_rResult.u.poObj = nullptr;
				_rResult.ncType = ee::EE_NC_INVALID;
				return false;
			}
			return _rResult.ncType == ee::EE_NC_SIGNED || _rResult.ncType == ee::EE_NC_UNSIGNED || _rResult.ncType == ee::EE_NC_FLOATING;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Converts a character to the form shown in the Value column (control characters are shown as dots).
	 *
	 * \param _ui32Char The character (UTF-32).
	 * \param _wsText Receives the character (appended).
	 **/
	void CInspector::AppendShownChar( uint32_t _ui32Char, std::wstring &_wsText ) {
		if ( _ui32Char < 0x20 || _ui32Char == 0x7F || (_ui32Char >= 0x80 && _ui32Char < 0xA0) ) {
			_wsText.push_back( L'.' );
			return;
		}
		if ( _ui32Char > 0xFFFF ) {
			_ui32Char -= 0x10000;
			_wsText.push_back( static_cast<wchar_t>(0xD800 + (_ui32Char >> 10)) );
			_wsText.push_back( static_cast<wchar_t>(0xDC00 + (_ui32Char & 0x3FF)) );
			return;
		}
		_wsText.push_back( static_cast<wchar_t>(_ui32Char) );
	}

}	// namespace mx

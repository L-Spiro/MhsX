#include "MXUtilities.h"
#include "../CodePages/MXCodePages.h"
#include "../Float16/MXFloat16.h"
#include "../Html/MXHtml.h"
#include "../MemHack/MXProcess.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include <EEExpEval.h>

#include <algorithm>
#include <cmath>
#include <codecvt>
#include <ctime>
#include <cuchar>
#include <locale>
#include <random>

namespace mx {

	// Options.
	CUtilities::MX_UTIL_OPTIONS CUtilities::Options = {
		FALSE,							// bUse0xForHex
		FALSE,							// bShortenEnumNames
		MX_DTO_DEFAULT,					// dwDataTypeOptions
	};

	// Data-type information.
	CUtilities::MX_DATA_TYPE_INFO CUtilities::DataTypeInfo[] = {
		//dtType			pcCodeName,					pcBasicName							pcRange																	dwSize					dwAlign					bIsFloat	bIsSigned
		{ MX_DT_INT8,		_T_LEN_1C554140_int8_t,		_T_LEN_2CCCF8A1_Char,				_T_LEN_636B7D96__128_to_127,											sizeof( int8_t ),		__alignof( int8_t ),	FALSE,		TRUE },
		{ MX_DT_UINT8,		_T_LEN_8EABC011_uint8_t,	_T_LEN_3DF23550_Byte,				_T_LEN_D7DC8296_0_to_255,												sizeof( uint8_t ),		__alignof( uint8_t ),	FALSE,		FALSE },
		{ MX_DT_INT16,		_T_LEN_BFD40C35_int16_t,	_T_LEN_4EE9BFA6_Short,				_T_LEN_3A2FE9B5__32_768_to_32_767,										sizeof( int16_t ),		__alignof( int16_t ),	FALSE,		TRUE },
		{ MX_DT_UINT16,		_T_LEN_A32A93D6_uint16_t,	_T_LEN_31443FFB_Unsigned_Short,		_T_LEN_B6F7A5DA_0_to_65_535,											sizeof( uint16_t ),		__alignof( uint16_t ),	FALSE,		FALSE },
		{ MX_DT_INT32,		_T_LEN_12D46C62_int32_t,	_T_LEN_2C1C9C51_Int,				_T_LEN_3929F59C__2_147_483_648_to_2_147_483_647,						sizeof( int32_t ),		__alignof( int32_t ),	FALSE,		TRUE },
		{ MX_DT_UINT32,		_T_LEN_0E2AF381_uint32_t,	_T_LEN_7652642A_Unsigned_Int,		_T_LEN_6022BCC9_0_to_4_294_967_295,										sizeof( uint32_t ),		__alignof( uint32_t ),	FALSE,		FALSE },
		{ MX_DT_INT64,		_T_LEN_2187E0E2_int64_t,	_T_LEN_DB26D37C_64_bit_Int,			_T_LEN_D27BE76B__9_223_372_036_854_775_808_to_9_223_372_036_854_775_807,sizeof( int64_t ),		__alignof( int64_t ),	FALSE,		TRUE },
		{ MX_DT_UINT64,		_T_LEN_3D797F01_uint64_t,	_T_LEN_8FFBC537_Unsigned_64_bit_Int,_T_LEN_5539660F_0_to_18_446_744_073_709_551_615,						sizeof( uint64_t ),		__alignof( uint64_t ),	FALSE,		FALSE },
		{ MX_DT_FLOAT,		_T_LEN_C9A55E95_float,		_T_LEN_08647191_Float,				_T_LEN_B518763F___3_402823466e_38,										sizeof( float ),		__alignof( float ),		TRUE,		TRUE },
		{ MX_DT_FLOAT16,	_T_LEN_C8DB4BDD_float16,	_T_LEN_5B68EE1F_Half,				_T_LEN_56F7FDCA___65_504,												sizeof( uint16_t ),		__alignof( uint16_t ),	TRUE,		TRUE },
		{ MX_DT_DOUBLE,		_T_LEN_DAE7F2EF_double,		_T_LEN_DD4BF7D9_Double,				_T_LEN_AC0850FC___1_7976931348623158e_308,								sizeof( double ),		__alignof( double ),	TRUE,		TRUE },
		{ MX_DT_VOID,		_T_LEN_D27BD9EE_void,		_T_LEN_724976D0_Void,				_T_LEN_5F837256__undefined_,											DWINVALID,				DWINVALID,				FALSE,		FALSE },
	};

	// Internal buffer for temporary strings.
	CHAR CUtilities::m_szTemp[128] = { 0 };

	// Buffer for printing a float.
	wchar_t CUtilities::m_szFloatPrintBuffer[2000+309+3] = { 0 };

	// The critical section.
	lsw::CCriticalSection CUtilities::m_csCrit;

	CUtilities::MX_PROCESS_PAUSE_HELPER::MX_PROCESS_PAUSE_HELPER( class CProcess * _ppProc ) :
		ppProc( _ppProc ) {
		lResult = _ppProc->SuspendProcess();
	}
	CUtilities::MX_PROCESS_PAUSE_HELPER::~MX_PROCESS_PAUSE_HELPER() {
		lResult = ppProc->ResumeProcess();
	}

	// Convert a single byte to hex.  _pcString must be at least 3 characters long.
	VOID CUtilities::ByteToHex( BYTE _bIn, CHAR * _pcString, BOOL _bLower ) {
		BYTE bTemp = (_bIn >> 4);
#define MX_HEX_TO_CHAR( VAL )		((VAL) < 10 ? ('0' + (VAL)) : ((_bLower ? 'a' : 'A') + ((VAL) - 10)))
		_pcString[0] = MX_HEX_TO_CHAR( bTemp );
		bTemp = (_bIn & 0xF);
		_pcString[1] = MX_HEX_TO_CHAR( bTemp );
		_pcString[2] = '\0';
	}

	// Convert a single byte to hex.
	VOID CUtilities::ByteToHex( BYTE _bIn, std::string &_sString, BOOL _bLower ) {
		BYTE bTemp = (_bIn >> 4);
		
		_sString.push_back( MX_HEX_TO_CHAR( bTemp ) );
		bTemp = (_bIn & 0xF);
		_sString.push_back( MX_HEX_TO_CHAR( bTemp ) );
#undef MX_HEX_TO_CHAR
	}
	
	// Convert a byte array to a hex string.  _pcString must be twice as long as the input, plus 1 character for the NULL.
	VOID CUtilities::BytesToHex( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		for ( size_t I = 0; I < _sLen; ++I, ++bIn, _pcString += 2 ) {
			ByteToHex( (*bIn), _pcString, _bLower );
		}
	}

	// Convert a byte array to a hex string.
	VOID CUtilities::BytesToHex( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		for ( size_t I = 0; I < _sLen; ++I, ++bIn ) {
			ByteToHex( (*bIn), _sString, _bLower );
		}
	}

	// Convert a byte array to a hex string.  _pcString must be 3 times as long as the input.
	VOID CUtilities::BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		for ( size_t I = 0; I < _sLen; ++I, ++bIn ) {
			ByteToHex( (*bIn), _pcString, _bLower );
			_pcString += 2;
			(*_pcString++) = ' ';
		}
		(*_pcString) = '\0';
	}

	// Convert a byte array to a hex string.
	VOID CUtilities::BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		size_t sEnd = _sLen - 1;
		for ( size_t I = 0; I < _sLen; ++I, ++bIn ) {
			ByteToHex( (*bIn), _sString, _bLower );
			if ( I != sEnd ) {
				_sString.push_back( ' ' );
			}
		}
	}

	// Convert a byte array to a hex string.
	VOID CUtilities::BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, std::string &_sString, const uint8_t * _puiMeta, size_t _sMetaLen, size_t _sMetaCharLen, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		size_t sEnd = _sLen - 1;
		for ( size_t I = 0; I < _sLen; ++I, ++bIn ) {
			size_t sMetaIdx = I / _sMetaCharLen;
			if ( sMetaIdx < _sMetaLen && _puiMeta[sMetaIdx] != 0 ) {
				_sString.push_back( _puiMeta[sMetaIdx] );
				I += _sMetaCharLen - 1;
				bIn += _sMetaCharLen - 1;
			}
			else {
				ByteToHex( (*bIn), _sString, _bLower );
			}
			if ( I != sEnd ) {
				_sString.push_back( ' ' );
			}
		}
	}

	// Convert a byte array to a hex string.
	VOID CUtilities::BytesToHexWithSpaces_MultiFormat( const VOID * _pvIn, size_t _sLen, std::string &_sString, const uint8_t * _puiMeta, size_t _sMetaLen, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		for ( size_t I = 0; I < _sMetaLen; ++I ) {
			switch ( _puiMeta[I] & CUtilities::MX_WF_MASK ) {
				case CUtilities::MX_WF_ANY_CHAR : {
					if ( _sString.size() ) { _sString.push_back( ' ' ); }
					_sString.push_back( '?' );
					break;
				}
				case CUtilities::MX_WF_ANY : {
					if ( _sString.size() ) { _sString.push_back( ' ' ); }
					_sString.push_back( '*' );
					break;
				}
				default : {
					size_t sLen = DataTypeSize( static_cast<CUtilities::MX_DATA_TYPES>(_puiMeta[I] & CUtilities::MX_DT_MASK) );
					for ( size_t J = 0; J < sLen; ++J ) {
						if ( _sString.size() ) { _sString.push_back( ' ' ); }
						ByteToHex( (*bIn++), _sString, _bLower );
					}
				}
			}
		}
	}

	// Is the given character printable?  If not, it should be printed as a space or question mark.
	BOOL CUtilities::ByteIsPrintable( BYTE _bIn, BOOL _bPrintExtended ) {
		if ( _bIn < 32 || _bIn == 127 ) { return FALSE; }
		return _bIn < 128 || _bPrintExtended;
	}

	// Convert a single byte to a printable character.  If _bPrintExtended is TRUE, characters above 127 are kept.
	CHAR CUtilities::ByteToPrintable( BYTE _bIn, BOOL _bPrintExtended, BOOL _bUseQuestionMarks ) {
		CHAR cDefault = _bUseQuestionMarks ? '?' : ' ';
		return ByteIsPrintable( _bIn, _bPrintExtended ) ? _bIn : cDefault;
	}

	// Converts a byte to a C-string value.  _pcString must be at least 5 characters long.  Returns the number of characters written.
	DWORD CUtilities::ByteToCString( BYTE _bIn, CHAR * _pcString, BOOL _bLower, BOOL _bExtended ) {
#define MX_PRINTESC( VAL )	_pcString[0] = '\\'; _pcString[1] = VAL; _pcString[2] = '\0';
#define MX_ESC_CASE( ESC, VAL )				\
	case ESC : {							\
		MX_PRINTESC( VAL );					\
		return 2;							\
	}
		switch ( _bIn ) {
			MX_ESC_CASE( '\\', '\\' )
			MX_ESC_CASE( '\n', 'n' )
			MX_ESC_CASE( '\t', 't' )
			MX_ESC_CASE( '\v', 'v' )
			MX_ESC_CASE( '\b', 'b' )
			MX_ESC_CASE( '\r', 'r' )
			MX_ESC_CASE( '\f', 'f' )
			MX_ESC_CASE( '\a', 'a' )
			MX_ESC_CASE( '\'', '\'' )
			MX_ESC_CASE( '\"', '\"' )
			MX_ESC_CASE( '\0', '0' )
			default : {
				if ( !ByteIsPrintable( _bIn, _bExtended ) ) {
					(*_pcString++) = '\\';
					(*_pcString++) = 'x';
					ByteToHex( _bIn, _pcString, _bLower );
					return 4;
				}
				_pcString[0] = _bIn;
				_pcString[1] = '\0';
				return 1;
			}
		}
#undef MX_ESC_CASE
#undef MX_PRINTESC
	}

	// Converts a byte to a C-string value.  Returns the number of characters written.
	DWORD CUtilities::ByteToCString( BYTE _bIn, std::string &_sString, BOOL _bLower, BOOL _bExtended ) {
#define MX_PRINTESC( VAL )	_sString.push_back( '\\' ); _sString.push_back( VAL );
#define MX_ESC_CASE( ESC, VAL )				\
	case ESC : {							\
		MX_PRINTESC( VAL );					\
		return 2;							\
	}
		switch ( _bIn ) {
			MX_ESC_CASE( '\\', '\\' )
			MX_ESC_CASE( '\n', 'n' )
			MX_ESC_CASE( '\t', 't' )
			MX_ESC_CASE( '\v', 'v' )
			MX_ESC_CASE( '\b', 'b' )
			MX_ESC_CASE( '\r', 'r' )
			MX_ESC_CASE( '\f', 'f' )
			MX_ESC_CASE( '\a', 'a' )
			MX_ESC_CASE( '\'', '\'' )
			MX_ESC_CASE( '\"', '\"' )
			MX_ESC_CASE( '\0', '0' )
			default : {
				if ( !ByteIsPrintable( _bIn, _bExtended ) ) {
					_sString.append( "\\x" );
					ByteToHex( _bIn, _sString, _bLower );
					return 4;
				}
				_sString.push_back( _bIn );
				return 1;
			}
		}
#undef MX_ESC_CASE
#undef MX_PRINTESC
	}

	// Convert a byte array to a C string.  Returns the number of characters written.
	DWORD CUtilities::BytesToCString( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower, BOOL _bExtended ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		DWORD dwTotal = 0;
		for ( size_t I = 0; I < _sLen; ++I, ++bIn ) {
			DWORD dwLen = ByteToCString( (*bIn), _pcString, _bLower, _bExtended );
			_pcString += dwLen;
			dwTotal += dwLen;
		}
		return dwTotal;
	}

	// Convert a byte array to a C string.  Returns the number of characters written.
	DWORD CUtilities::BytesToCString( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower, BOOL _bExtended ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		DWORD dwTotal = 0;
		for ( size_t I = 0; I < _sLen; ++I, ++bIn ) {
			DWORD dwLen = ByteToCString( (*bIn), _sString, _bLower, _bExtended );
			dwTotal += dwLen;
		}
		return dwTotal;
	}

	// Converts a MX_MACHINE_TYPES enum to its string value.  _pcRet should be at least 32 characters long.
	const CHAR * CUtilities::MachineTypeToString( uint32_t _uiType, CHAR * _pcRet ) {
		struct {
			uint32_t ui32Type;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ MX_IMAGE_FILE_MACHINE_UNKNOWN, _T_3BE1EBA6_IMAGE_FILE_MACHINE_UNKNOWN, _LEN_3BE1EBA6 },
			{ MX_IMAGE_FILE_MACHINE_AM33, _T_69459837_IMAGE_FILE_MACHINE_AM33, _LEN_69459837 },
			{ MX_IMAGE_FILE_MACHINE_AMD64, _T_67AB713F_IMAGE_FILE_MACHINE_AMD64, _LEN_67AB713F },
			{ MX_IMAGE_FILE_MACHINE_ARM, _T_36D7EB23_IMAGE_FILE_MACHINE_ARM, _LEN_36D7EB23 },
			{ MX_IMAGE_FILE_MACHINE_ARMNT, _T_CA549FDF_IMAGE_FILE_MACHINE_ARMNT, _LEN_CA549FDF },
			{ MX_IMAGE_FILE_MACHINE_ARM64, _T_60000D79_IMAGE_FILE_MACHINE_ARM64, _LEN_60000D79 },
			{ MX_IMAGE_FILE_MACHINE_EBC, _T_9CA47CA9_IMAGE_FILE_MACHINE_EBC, _LEN_9CA47CA9 },
			{ MX_IMAGE_FILE_MACHINE_I386, _T_6100D8C6_IMAGE_FILE_MACHINE_I386, _LEN_6100D8C6 },
			{ MX_IMAGE_FILE_MACHINE_IA64, _T_46F8285A_IMAGE_FILE_MACHINE_IA64, _LEN_46F8285A },
			{ MX_IMAGE_FILE_MACHINE_M32R, _T_5E52025A_IMAGE_FILE_MACHINE_M32R, _LEN_5E52025A },
			{ MX_IMAGE_FILE_MACHINE_MIPS16, _T_C5EBA7A5_IMAGE_FILE_MACHINE_MIPS16, _LEN_C5EBA7A5 },
			{ MX_IMAGE_FILE_MACHINE_MIPSFPU, _T_0D6344C2_IMAGE_FILE_MACHINE_MIPSFPU, _LEN_0D6344C2 },
			{ MX_IMAGE_FILE_MACHINE_MIPSFPU16, _T_C043BDF2_IMAGE_FILE_MACHINE_MIPSFPU16, _LEN_C043BDF2 },
			{ MX_IMAGE_FILE_MACHINE_POWERPC, _T_F59A307F_IMAGE_FILE_MACHINE_POWERPC, _LEN_F59A307F },
			{ MX_IMAGE_FILE_MACHINE_POWERPCFP, _T_02EF0B87_IMAGE_FILE_MACHINE_POWERPCFP, _LEN_02EF0B87 },
			{ MX_IMAGE_FILE_MACHINE_R4000, _T_5413D068_IMAGE_FILE_MACHINE_R4000, _LEN_5413D068 },
			{ MX_IMAGE_FILE_MACHINE_SH3, _T_2EE53ADD_IMAGE_FILE_MACHINE_SH3, _LEN_2EE53ADD },
			{ MX_IMAGE_FILE_MACHINE_SH3DSP, _T_02B7CDAE_IMAGE_FILE_MACHINE_SH3DSP, _LEN_02B7CDAE },
			{ MX_IMAGE_FILE_MACHINE_SH4, _T_B081AF7E_IMAGE_FILE_MACHINE_SH4, _LEN_B081AF7E },
			{ MX_IMAGE_FILE_MACHINE_SH5, _T_C7869FE8_IMAGE_FILE_MACHINE_SH5, _LEN_C7869FE8 },
			{ MX_IMAGE_FILE_MACHINE_THUMB, _T_903C248C_IMAGE_FILE_MACHINE_THUMB, _LEN_903C248C },
			{ MX_IMAGE_FILE_MACHINE_WCEMIPSV2, _T_CF5B6B4D_IMAGE_FILE_MACHINE_WCEMIPSV2, _LEN_CF5B6B4D },
			{ MX_IMAGE_FILE_MACHINE_R3000, _T_C9C4E8D1_IMAGE_FILE_MACHINE_R3000, _LEN_C9C4E8D1 },
			{ MX_IMAGE_FILE_MACHINE_R10000, _T_7F06AAEB_IMAGE_FILE_MACHINE_R10000, _LEN_7F06AAEB },
			{ MX_IMAGE_FILE_MACHINE_ALPHA, _T_55EBCC3D_IMAGE_FILE_MACHINE_ALPHA, _LEN_55EBCC3D },
			{ MX_IMAGE_FILE_MACHINE_SH3E, _T_2CF811C1_IMAGE_FILE_MACHINE_SH3E, _LEN_2CF811C1 },
			{ MX_IMAGE_FILE_MACHINE_ALPHA64, _T_114BE4F1_IMAGE_FILE_MACHINE_ALPHA64, _LEN_114BE4F1 },
			{ MX_IMAGE_FILE_MACHINE_AXP64, _T_1B43DD3E_IMAGE_FILE_MACHINE_AXP64, _LEN_1B43DD3E },
			{ MX_IMAGE_FILE_MACHINE_TRICORE, _T_8B8D8482_IMAGE_FILE_MACHINE_TRICORE, _LEN_8B8D8482 },
			{ MX_IMAGE_FILE_MACHINE_CEF, _T_A7026253_IMAGE_FILE_MACHINE_CEF, _LEN_A7026253 },
			{ MX_IMAGE_FILE_MACHINE_CEE, _T_3E0B33E9_IMAGE_FILE_MACHINE_CEE, _LEN_3E0B33E9 },
		};

		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ); ++I ) {
			if ( aTable[I].ui32Type == _uiType ) {
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, _pcRet );
				return _pcRet;
			}
		}
		_DEC_629B9E5B_Unknown( _pcRet );
		return _pcRet;
	}

	const CHAR * CUtilities::PeCharacteristicsToString( uint32_t _uiVal, std::string &_sString ) {
		struct {
			uint32_t ui32Type;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ MX_IMAGE_FILE_RELOCS_STRIPPED, _T_A8CF4CB1_IMAGE_FILE_RELOCS_STRIPPED, _LEN_A8CF4CB1 },
			{ MX_IMAGE_FILE_EXECUTABLE_IMAGE, _T_8CE929D7_IMAGE_FILE_EXECUTABLE_IMAGE, _LEN_8CE929D7 },
			{ MX_IMAGE_FILE_LINE_NUMS_STRIPPED, _T_4ACFE92C_IMAGE_FILE_LINE_NUMS_STRIPPED, _LEN_4ACFE92C },
			{ MX_IMAGE_FILE_LOCAL_SYMS_STRIPPED, _T_36012C69_IMAGE_FILE_LOCAL_SYMS_STRIPPED, _LEN_36012C69 },
			{ MX_IMAGE_FILE_AGGRESIVE_WS_TRIM, _T_C5765866_IMAGE_FILE_AGGRESIVE_WS_TRIM, _LEN_C5765866 },
			{ MX_IMAGE_FILE_LARGE_ADDRESS_AWARE, _T_CA4B53ED_IMAGE_FILE_LARGE_ADDRESS_AWARE, _LEN_CA4B53ED },
			{ 0x40, _T_400C4BC7_40h, _LEN_400C4BC7 },
			{ MX_IMAGE_FILE_BYTES_REVERSED_LO, _T_9D6947D9_IMAGE_FILE_BYTES_REVERSED_LO, _LEN_9D6947D9 },
			{ MX_IMAGE_FILE_32BIT_MACHINE, _T_38AEDDBB_IMAGE_FILE_32BIT_MACHINE, _LEN_38AEDDBB },
			{ MX_IMAGE_FILE_DEBUG_STRIPPED, _T_4DFC5FF2_IMAGE_FILE_DEBUG_STRIPPED, _LEN_4DFC5FF2 },
			{ MX_IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, _T_550884A0_IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, _LEN_550884A0 },
			{ MX_IMAGE_FILE_NET_RUN_FROM_SWAP, _T_6A707443_IMAGE_FILE_NET_RUN_FROM_SWAP, _LEN_6A707443 },
			{ MX_IMAGE_FILE_SYSTEM, _T_337193FA_IMAGE_FILE_SYSTEM, _LEN_337193FA },
			{ MX_IMAGE_FILE_DLL, _T_548BE4A7_IMAGE_FILE_DLL, _LEN_548BE4A7 },
			{ MX_IMAGE_FILE_UP_SYSTEM_ONLY, _T_C072A113_IMAGE_FILE_UP_SYSTEM_ONLY, _LEN_C072A113 },
			{ MX_IMAGE_FILE_BYTES_REVERSED_HI, _T_106627E8_IMAGE_FILE_BYTES_REVERSED_HI, _LEN_106627E8 },
		};
		BOOL bAdded = FALSE;
		for ( size_t J = 0; J < sizeof( aTable ) / sizeof( aTable[0] ); ++J ) {
			if ( _uiVal & aTable[J].ui32Type ) {
				if ( bAdded ) {
					_sString.append( " | " );
				}
				CHAR szBuffer[_T_MAX_LEN];
				CStringDecoder::Decode( aTable[J].pcName, aTable[J].ui32StrLen, szBuffer );
				_sString.append( szBuffer );
				::ZeroMemory( szBuffer, _T_MAX_LEN );
				bAdded = TRUE;
			}
		}
		if ( !bAdded ) {
			_sString.append( "No Flags Set" );
		}
		return _sString.c_str();
	}

	// Converts a MX_PE_MAGIC_FLAGS enum to its string value.  _pcRet should be at least 32 characters long.
	const CHAR * CUtilities::PeMagicTypeToString( uint32_t _uiType, CHAR * _pcRet ) {
		struct {
			uint32_t ui32Type;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ MX_IMAGE_NT_OPTIONAL_HDR32_MAGIC, _T_ACF38497_IMAGE_NT_OPTIONAL_HDR32_MAGIC, _LEN_ACF38497 },
			{ MX_IMAGE_NT_OPTIONAL_HDR64_MAGIC, _T_87C3BFC9_IMAGE_NT_OPTIONAL_HDR64_MAGIC, _LEN_87C3BFC9 },
			{ MX_IMAGE_ROM_OPTIONAL_HDR_MAGIC, _T_7FD30EBC_IMAGE_ROM_OPTIONAL_HDR_MAGIC, _LEN_7FD30EBC },
		};

		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ); ++I ) {
			if ( aTable[I].ui32Type == _uiType ) {
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, _pcRet );
				return _pcRet;
			}
		}
		_DEC_629B9E5B_Unknown( _pcRet );
		return _pcRet;
	}

	// Converts a MX_PE_WINDOWS_SUBSYSTEM enum to its string value.  _pcRet should be at least 32 characters long.
	const CHAR * CUtilities::PeSubSystemTypeToString( uint32_t _uiType, CHAR * _pcRet ) {
		struct {
			uint32_t ui32Type;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ MX_IMAGE_SUBSYSTEM_UNKNOWN, _T_31308582_IMAGE_SUBSYSTEM_UNKNOWN, _LEN_31308582 },
			{ MX_IMAGE_SUBSYSTEM_NATIVE, _T_DFACF153_IMAGE_SUBSYSTEM_NATIVE, _LEN_DFACF153 },
			{ MX_IMAGE_SUBSYSTEM_WINDOWS_GUI, _T_D8923296_IMAGE_SUBSYSTEM_WINDOWS_GUI, _LEN_D8923296 },
			{ MX_IMAGE_SUBSYSTEM_WINDOWS_CUI, _T_DF9B9A4A_IMAGE_SUBSYSTEM_WINDOWS_CUI, _LEN_DF9B9A4A },
			{ MX_IMAGE_SUBSYSTEM_OS2_CUI, _T_F4F9EB46_IMAGE_SUBSYSTEM_OS2_CUI, _LEN_F4F9EB46 },
			{ MX_IMAGE_SUBSYSTEM_POSIX_CUI, _T_CDD7A962_IMAGE_SUBSYSTEM_POSIX_CUI, _LEN_CDD7A962 },
			{ MX_IMAGE_SUBSYSTEM_NATIVE_WINDOWS, _T_D43166A9_IMAGE_SUBSYSTEM_NATIVE_WINDOWS, _LEN_D43166A9 },
			{ MX_IMAGE_SUBSYSTEM_WINDOWS_CE_GUI, _T_C6B4AD26_IMAGE_SUBSYSTEM_WINDOWS_CE_GUI, _LEN_C6B4AD26 },
			{ MX_IMAGE_SUBSYSTEM_EFI_APPLICATION, _T_FC04CDE9_IMAGE_SUBSYSTEM_EFI_APPLICATION, _LEN_FC04CDE9 },
			{ MX_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER, _T_7702C513_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER, _LEN_7702C513 },
			{ MX_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER, _T_4147CE4F_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER, _LEN_4147CE4F },
			{ MX_IMAGE_SUBSYSTEM_EFI_ROM, _T_2734017B_IMAGE_SUBSYSTEM_EFI_ROM, _LEN_2734017B },
			{ MX_IMAGE_SUBSYSTEM_XBOX, _T_C3B643FF_IMAGE_SUBSYSTEM_XBOX, _LEN_C3B643FF },
			{ MX_IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION, _T_EAA62787_IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION, _LEN_EAA62787 },
		};

		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ); ++I ) {
			if ( aTable[I].ui32Type == _uiType ) {
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, _pcRet );
				return _pcRet;
			}
		}
		_DEC_629B9E5B_Unknown( _pcRet );
		return _pcRet;
	}

	// Creates a string with all of the MX_PE_DLL_CHARACTERISTICS bit flags set in a given value.
	const CHAR * CUtilities::PeDllCharacteristicsToString( uint32_t _uiVal, std::string &_sString ) {
		struct {
			uint32_t ui32Type;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ 0x01, _T_C4EDFC27_1h, _LEN_C4EDFC27 },
			{ 0x02, _T_EFC0AFE4_2h, _LEN_EFC0AFE4 },
			{ 0x04, _T_B99A0862_4h, _LEN_B99A0862 },
			{ 0x08, _T_152F476E_8h, _LEN_152F476E },
			{ 0x10, _T_46C7892C_10h, _LEN_46C7892C },
			{ MX_IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA, _T_31084C02_IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA, _LEN_31084C02 },
			{ MX_IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE, _T_472FF2F7_IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE, _LEN_472FF2F7 },
			{ MX_IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY, _T_F7B79C9B_IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY, _LEN_F7B79C9B },
			{ MX_IMAGE_DLLCHARACTERISTICS_NX_COMPAT, _T_6877CA9D_IMAGE_DLLCHARACTERISTICS_NX_COMPAT, _LEN_6877CA9D },
			{ MX_IMAGE_DLLCHARACTERISTICS_NO_ISOLATION, _T_B64C0CEB_IMAGE_DLLCHARACTERISTICS_NO_ISOLATION, _LEN_B64C0CEB },
			{ MX_IMAGE_DLLCHARACTERISTICS_NO_SEH, _T_ED7D814E_IMAGE_DLLCHARACTERISTICS_NO_SEH, _LEN_ED7D814E },
			{ MX_IMAGE_DLLCHARACTERISTICS_NO_BIND, _T_85DF150A_IMAGE_DLLCHARACTERISTICS_NO_BIND, _LEN_85DF150A },
			{ MX_IMAGE_DLLCHARACTERISTICS_APPCONTAINER, _T_B6C7F0A0_IMAGE_DLLCHARACTERISTICS_APPCONTAINER, _LEN_B6C7F0A0 },
			{ MX_IMAGE_DLLCHARACTERISTICS_WDM_DRIVER, _T_39C8E2AD_IMAGE_DLLCHARACTERISTICS_WDM_DRIVER, _LEN_39C8E2AD },
			{ MX_IMAGE_DLLCHARACTERISTICS_GUARD_CF, _T_88FF96BC_IMAGE_DLLCHARACTERISTICS_GUARD_CF, _LEN_88FF96BC },
			{ MX_IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE, _T_5C0EC779_IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE, _LEN_5C0EC779 },
		};
		BOOL bAdded = FALSE;
		for ( size_t J = 0; J < sizeof( aTable ) / sizeof( aTable[0] ); ++J ) {
			if ( _uiVal & aTable[J].ui32Type ) {
				if ( bAdded ) {
					_sString.append( " | " );
				}
				CHAR szBuffer[_T_MAX_LEN];
				CStringDecoder::Decode( aTable[J].pcName, aTable[J].ui32StrLen, szBuffer );
				_sString.append( szBuffer );
				bAdded = TRUE;
			}
		}
		if ( !bAdded ) {
			_sString.append( "No Flags Set" );
		}
		return _sString.c_str();
	}

	// Creates a string with all of the MX_PE_SECTION_FLAGS bit flags set in a given value.
	const CHAR * CUtilities::PeSectionFlagsToString( uint32_t _uiVal, std::string &_sString ) {
		struct {
			uint32_t ui32Type;
			uint32_t ui32Mask;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ MX_IMAGE_SCN_TYPE_DSECT, MX_IMAGE_SCN_TYPE_DSECT, _T_12BD7DCA_IMAGE_SCN_TYPE_DSECT, _LEN_12BD7DCA },
			{ MX_IMAGE_SCN_TYPE_NOLOAD, MX_IMAGE_SCN_TYPE_NOLOAD, _T_9F3CF5C8_IMAGE_SCN_TYPE_NOLOAD, _LEN_9F3CF5C8 },
			{ MX_IMAGE_SCN_TYPE_GROUP, MX_IMAGE_SCN_TYPE_GROUP, _T_FBC39467_IMAGE_SCN_TYPE_GROUP, _LEN_FBC39467 },
			{ MX_IMAGE_SCN_TYPE_NO_PAD, MX_IMAGE_SCN_TYPE_NO_PAD, _T_CAEAE9F4_IMAGE_SCN_TYPE_NO_PAD, _LEN_CAEAE9F4 },
			{ MX_IMAGE_SCN_TYPE_COPY, MX_IMAGE_SCN_TYPE_COPY, _T_47C297E8_IMAGE_SCN_TYPE_COPY, _LEN_47C297E8 },
			{ MX_IMAGE_SCN_CNT_CODE, MX_IMAGE_SCN_CNT_CODE, _T_87089CB5_IMAGE_SCN_CNT_CODE, _LEN_87089CB5 },
			{ MX_IMAGE_SCN_CNT_INITIALIZED_DATA, MX_IMAGE_SCN_CNT_INITIALIZED_DATA, _T_CC93B5FD_IMAGE_SCN_CNT_INITIALIZED_DATA, _LEN_CC93B5FD },
			{ MX_IMAGE_SCN_CNT_UNINITIALIZED_DATA, MX_IMAGE_SCN_CNT_UNINITIALIZED_DATA, _T_F7613740_IMAGE_SCN_CNT_UNINITIALIZED_DATA, _LEN_F7613740 },
			{ MX_IMAGE_SCN_LNK_OTHER, MX_IMAGE_SCN_LNK_OTHER, _T_E93677C7_IMAGE_SCN_LNK_OTHER, _LEN_E93677C7 },
			{ MX_IMAGE_SCN_LNK_INFO, MX_IMAGE_SCN_LNK_INFO, _T_10343F98_IMAGE_SCN_LNK_INFO, _LEN_10343F98 },
			{ MX_IMAGE_SCN_TYPE_OVER, MX_IMAGE_SCN_TYPE_OVER, _T_BE848F33_IMAGE_SCN_TYPE_OVER, _LEN_BE848F33 },
			{ MX_IMAGE_SCN_LNK_REMOVE, MX_IMAGE_SCN_LNK_REMOVE, _T_6DB02461_IMAGE_SCN_LNK_REMOVE, _LEN_6DB02461 },
			{ MX_IMAGE_SCN_LNK_COMDAT, MX_IMAGE_SCN_LNK_COMDAT, _T_8CECA67B_IMAGE_SCN_LNK_COMDAT, _LEN_8CECA67B },
			{ 0x00002000, 0x00002000, _T_55ACDE53_2000h, _LEN_55ACDE53 },
			{ MX_IMAGE_SCN_MEM_PROTECTED, MX_IMAGE_SCN_MEM_PROTECTED, _T_FB4B6464_IMAGE_SCN_MEM_PROTECTED, _LEN_FB4B6464 },
			{ MX_IMAGE_SCN_GPREL, MX_IMAGE_SCN_GPREL, _T_EF5A5D03_IMAGE_SCN_GPREL, _LEN_EF5A5D03 },
			{ MX_IMAGE_SCN_MEM_SYSHEAP, MX_IMAGE_SCN_MEM_SYSHEAP, _T_23AD020D_IMAGE_SCN_MEM_SYSHEAP, _LEN_23AD020D },
			{ MX_IMAGE_SCN_MEM_PURGEABLE, MX_IMAGE_SCN_MEM_PURGEABLE, _T_66A27CD0_IMAGE_SCN_MEM_PURGEABLE, _LEN_66A27CD0 },
			{ MX_IMAGE_SCN_MEM_LOCKED, MX_IMAGE_SCN_MEM_LOCKED, _T_441A0DA4_IMAGE_SCN_MEM_LOCKED, _LEN_441A0DA4 },
			{ MX_IMAGE_SCN_MEM_PRELOAD, MX_IMAGE_SCN_MEM_PRELOAD, _T_0F0D4C8D_IMAGE_SCN_MEM_PRELOAD, _LEN_0F0D4C8D },
			{ MX_IMAGE_SCN_ALIGN_1BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_8233C5F3_IMAGE_SCN_ALIGN_1BYTES, _LEN_8233C5F3 },
			{ MX_IMAGE_SCN_ALIGN_2BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_04A7B75D_IMAGE_SCN_ALIGN_2BYTES, _LEN_04A7B75D },
			{ MX_IMAGE_SCN_ALIGN_4BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_D2FE5440_IMAGE_SCN_ALIGN_4BYTES, _LEN_D2FE5440 },
			{ MX_IMAGE_SCN_ALIGN_8BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_A53C943B_IMAGE_SCN_ALIGN_8BYTES, _LEN_A53C943B },
			{ MX_IMAGE_SCN_ALIGN_16BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_70C332A9_IMAGE_SCN_ALIGN_16BYTES, _LEN_70C332A9 },
			{ MX_IMAGE_SCN_ALIGN_32BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_7CCD6196_IMAGE_SCN_ALIGN_32BYTES, _LEN_7CCD6196 },
			{ MX_IMAGE_SCN_ALIGN_64BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_F8ACAD2C_IMAGE_SCN_ALIGN_64BYTES, _LEN_F8ACAD2C },
			{ MX_IMAGE_SCN_ALIGN_128BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_35CA3FD7_IMAGE_SCN_ALIGN_128BYTES, _LEN_35CA3FD7 },
			{ MX_IMAGE_SCN_ALIGN_256BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_44E867CA_IMAGE_SCN_ALIGN_256BYTES, _LEN_44E867CA },
			{ MX_IMAGE_SCN_ALIGN_512BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_21F308D6_IMAGE_SCN_ALIGN_512BYTES, _LEN_21F308D6 },
			{ MX_IMAGE_SCN_ALIGN_1024BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_E7EAD6BA_IMAGE_SCN_ALIGN_1024BYTES, _LEN_E7EAD6BA },
			{ MX_IMAGE_SCN_ALIGN_2048BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_CA751F3E_IMAGE_SCN_ALIGN_2048BYTES, _LEN_CA751F3E },
			{ MX_IMAGE_SCN_ALIGN_4096BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_E2B26604_IMAGE_SCN_ALIGN_4096BYTES, _LEN_E2B26604 },
			{ MX_IMAGE_SCN_ALIGN_8192BYTES, MX_IMAGE_SCN_ALIGN_MASK, _T_53BDD798_IMAGE_SCN_ALIGN_8192BYTES, _LEN_53BDD798 },
			{ MX_IMAGE_SCN_LNK_NRELOC_OVFL, MX_IMAGE_SCN_LNK_NRELOC_OVFL, _T_E0B78E5E_IMAGE_SCN_LNK_NRELOC_OVFL, _LEN_E0B78E5E },
			{ MX_IMAGE_SCN_MEM_DISCARDABLE, MX_IMAGE_SCN_MEM_DISCARDABLE, _T_7ACC9A0B_IMAGE_SCN_MEM_DISCARDABLE, _LEN_7ACC9A0B },
			{ MX_IMAGE_SCN_MEM_NOT_CACHED, MX_IMAGE_SCN_MEM_NOT_CACHED, _T_DE3D9492_IMAGE_SCN_MEM_NOT_CACHED, _LEN_DE3D9492 },
			{ MX_IMAGE_SCN_MEM_NOT_PAGED, MX_IMAGE_SCN_MEM_NOT_PAGED, _T_6B802CDA_IMAGE_SCN_MEM_NOT_PAGED, _LEN_6B802CDA },
			{ MX_IMAGE_SCN_MEM_SHARED, MX_IMAGE_SCN_MEM_SHARED, _T_BD44318E_IMAGE_SCN_MEM_SHARED, _LEN_BD44318E },
			{ MX_IMAGE_SCN_MEM_EXECUTE, MX_IMAGE_SCN_MEM_EXECUTE, _T_92103CF0_IMAGE_SCN_MEM_EXECUTE, _LEN_92103CF0 },
			{ MX_IMAGE_SCN_MEM_READ, MX_IMAGE_SCN_MEM_READ, _T_5B1EF8D1_IMAGE_SCN_MEM_READ, _LEN_5B1EF8D1 },
			{ MX_IMAGE_SCN_MEM_WRITE, MX_IMAGE_SCN_MEM_WRITE, _T_64C40597_IMAGE_SCN_MEM_WRITE, _LEN_64C40597 },
		};

		BOOL bAdded = FALSE;
		for ( size_t J = 0; J < sizeof( aTable ) / sizeof( aTable[0] ); ++J ) {
			if ( (_uiVal & aTable[J].ui32Mask) == aTable[J].ui32Type ) {
				if ( bAdded ) {
					_sString.append( " | " );
				}
				CHAR szBuffer[_T_MAX_LEN];
				CStringDecoder::Decode( aTable[J].pcName, aTable[J].ui32StrLen, szBuffer );
				_sString.append( szBuffer );
				bAdded = TRUE;
			}
		}
		if ( !bAdded ) {
			_sString.append( "No Flags Set" );
		}
		return _sString.c_str();
	}

	// Gets a Windows version string based on the given major and minor versions.
	const CHAR * CUtilities::WindowsVersion( uint32_t _uiMajor, uint32_t _uiMinor, std::string &_sString ) {
		struct {
			uint32_t ui32Major;
			uint32_t ui32Minor;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ 1, 4, _T_AA1E20F5_Windows_1_0, _LEN_AA1E20F5 },
			{ 2, 3, _T_A8589EAC_Windows_2_0, _LEN_A8589EAC },
			{ 2, 11, _T_4A0F909F_Windows_2_1x, _LEN_4A0F909F },
			{ 3, 0, _T_A99AF49B_Windows_3_0, _LEN_A99AF49B },
			{ 3, 10, _T_463E3378_Windows_NT_3_1, _LEN_463E3378 },
			{ 3, 11, _T_F2B3F7FA_Windows_3_1x, _LEN_F2B3F7FA },
			{ 3, 50, _T_4153F761_Windows_NT_3_5, _LEN_4153F761 },
			{ 3, 51, _T_B928ED8E_Windows_NT_3_51, _LEN_B928ED8E },
			{ 4, 0, _T_73EFE378_Windows_95_OEM__Windows_NT_4_00, _LEN_73EFE378 },
			{ 4, 3, _T_A51C769B_Windows_95_OEM_Service_Release_2_1_2_5_C, _LEN_A51C769B },
			{ 4, 10, _T_FD665CF7_Windows_98__Windows_98_Second_Edition__SE_, _LEN_FD665CF7 },
			{ 4, 90, _T_539F5134_Windows_Millenium, _LEN_539F5134 },
			{ 5, 0, _T_D926616C_Windows_NT_5_00__Beta_2___Windows_2000, _LEN_D926616C },
			{ 5, 1, _T_188456AD_Windows_XP, _LEN_188456AD },
			{ 5, 2, _T_4F24E6A8_Windows__NET_Server__Windows_Server_2003__Windows_Home_Server, _LEN_4F24E6A8 },
			{ 6, 0, _T_C24AB33B_Windows_Longhorn__Windows_Vista__Windows_Server_2008, _LEN_C24AB33B },
			{ 6, 1, _T_CE5C7984_Windows_7__Windows_Server_2008_R2__Windows_Home_Server_2011, _LEN_CE5C7984 },
			{ 6, 2, _T_902D8006_Windows_Server_2012__Windows_8__Windows_Phone_8, _LEN_902D8006 },
			{ 6, 3, _T_70E13043_Windows_Server_2012_R2__Windows_8_1, _LEN_70E13043 },
			{ 10, 0, _T_2AD621EE_Windows_10__Windows_Server_2016, _LEN_2AD621EE },
		};

		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ); ++I ) {
			if ( aTable[I].ui32Major == _uiMajor && aTable[I].ui32Minor == _uiMinor ) {
				CHAR szBuffer[_T_MAX_LEN];
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, szBuffer );
				_sString.append( szBuffer );
				::ZeroMemory( szBuffer, _T_MAX_LEN );
				return _sString.c_str();
			}
		}
		_sString.append( _DEC_S_629B9E5B_Unknown );
		return _sString.c_str();
	}

	// Creates an IMAGE_REL_BASED_* string.
	const CHAR * CUtilities::PeRelocBaseToString( uint32_t _uiVal, std::string &_sString ) {
		struct {
			uint32_t ui32Type;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ MX_IMAGE_REL_BASED_ABSOLUTE, _T_60ECEA1A_ABSOLUTE, _LEN_60ECEA1A }, // 0
			{ MX_IMAGE_REL_BASED_HIGH, _T_C79009E1_HIGH, _LEN_C79009E1 }, // 1
			{ MX_IMAGE_REL_BASED_LOW, _T_042F3A2B_LOW, _LEN_042F3A2B }, // 2
			{ MX_IMAGE_REL_BASED_HIGHLOW, _T_8DC2A876_HIGHLOW, _LEN_8DC2A876 }, // 3
			{ MX_IMAGE_REL_BASED_HIGHADJ, _T_05E88E37_HIGHADJ, _LEN_05E88E37 }, // 4
			{ MX_IMAGE_REL_BASED_MACHINE_SPECIFIC_5, _T_2940DFA0_MACHINE_SPECIFIC_5, _LEN_2940DFA0 }, // 5
			{ MX_IMAGE_REL_BASED_RESERVED, _T_8B91778E_RESERVED, _LEN_8B91778E }, // 6
			{ MX_IMAGE_REL_BASED_MACHINE_SPECIFIC_7, _T_C74EBE8C_MACHINE_SPECIFIC_7, _LEN_C74EBE8C }, // 7
			{ MX_IMAGE_REL_BASED_MACHINE_SPECIFIC_8, _T_57F1A31D_MACHINE_SPECIFIC_8, _LEN_57F1A31D }, // 8
			{ MX_IMAGE_REL_BASED_MACHINE_SPECIFIC_9, _T_20F6938B_MACHINE_SPECIFIC_9, _LEN_20F6938B }, // 9
			{ MX_IMAGE_REL_BASED_DIR64, _T_4ADD813D_DIR64, _LEN_4ADD813D }, // 10
		};
		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ); ++I ) {
			if ( aTable[I].ui32Type == _uiVal ) {
				CHAR szBuffer[_T_MAX_LEN];
				if ( !Options.bShortenEnumNames ) {
					CStringDecoder::Decode( _T_13E57F2A_IMAGE_REL_BASED_, _LEN_13E57F2A, szBuffer );
					_sString += szBuffer;
				}
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, szBuffer );
				_sString += szBuffer;
				return _sString.c_str();
			}
		}
		_sString += _DEC_S_629B9E5B_Unknown;
		return _sString.c_str();
	}

	// Creates a PROCESS_ string.
	const CHAR * CUtilities::OpenProcessFlagToString( uint32_t _uiVal, std::string &_sString, BOOL _bShort ) {
		_bShort = (_bShort == -1) ? Options.bShortenEnumNames : _bShort;
		uint32_t  uiCopy = _uiVal;
		struct {
			uint32_t ui32Type;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ PROCESS_TERMINATE, _T_2857F6FF_TERMINATE, _LEN_2857F6FF }, // 0
			{ PROCESS_CREATE_THREAD, _T_E250C94C_CREATE_THREAD, _LEN_E250C94C }, // 1
			{ PROCESS_SET_SESSIONID, _T_9D9335AB_SET_SESSIONID, _LEN_9D9335AB }, // 2
			{ PROCESS_VM_OPERATION, _T_461A1C82_VM_OPERATION, _LEN_461A1C82 }, // 3
			{ PROCESS_VM_READ, _T_5B6A3194_VM_READ, _LEN_5B6A3194 }, // 4
			{ PROCESS_VM_WRITE, _T_6272C441_VM_WRITE, _LEN_6272C441 }, // 5
			{ PROCESS_DUP_HANDLE, _T_1C1FE7F1_DUP_HANDLE, _LEN_1C1FE7F1 }, // 6
			{ PROCESS_CREATE_PROCESS, _T_B67B97FA_CREATE_PROCESS, _LEN_B67B97FA }, // 7
			{ PROCESS_SET_QUOTA, _T_BB83E852_SET_QUOTA, _LEN_BB83E852 }, // 8
			{ PROCESS_SET_INFORMATION, _T_C50CB72D_SET_INFORMATION, _LEN_C50CB72D }, // 9
			{ PROCESS_QUERY_INFORMATION, _T_864ADAA0_QUERY_INFORMATION, _LEN_864ADAA0 }, // 10
			{ PROCESS_SUSPEND_RESUME, _T_9A5DB186_SUSPEND_RESUME, _LEN_9A5DB186 }, // 11
			{ PROCESS_QUERY_LIMITED_INFORMATION, _T_0F01B77A_QUERY_LIMITED_INFORMATION, _LEN_0F01B77A }, // 12
			{ PROCESS_SET_LIMITED_INFORMATION, _T_EB0CC1BC_SET_LIMITED_INFORMATION, _LEN_EB0CC1BC }, // 13
		};
		bool bAdded = false;
		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ) && _uiVal; ++I ) {
			if ( (aTable[I].ui32Type & _uiVal) == aTable[I].ui32Type ) {
				if ( bAdded ) {
					_sString.push_back( '|' );
				}
				CHAR szBuffer[_T_MAX_LEN];
				if ( !_bShort ) {
					CStringDecoder::Decode( _T_CD613405_PROCESS_, _LEN_CD613405, szBuffer );
					_sString += szBuffer;
				}
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, szBuffer );
				_sString += szBuffer;
				bAdded = true;
				_uiVal &= ~aTable[I].ui32Type;
			}
		}
		if ( _uiVal ) {
			if ( bAdded ) {
				_sString.push_back( '|' );
			}
			ToHex( _uiVal, _sString, 8 );
		}
		if ( bAdded ) {
			_sString.push_back( ' ' );
		}
		ToHex( uiCopy, _sString, 8 );
		//_sString += _DEC_S_629B9E5B_Unknown;
		return _sString.c_str();
	}

	// Creates a string that best represents the given size.
	const CHAR * CUtilities::SizeString( uint64_t _uiSize, std::string &_sString ) {
		CHAR szBuffer[_T_MAX_LEN];
		struct {
			uint64_t uiSize;
			const CHAR * pcName;
			uint32_t ui32StrLen;
			const CHAR * pcFormat;
		}
		static const aTable[] = {
			{ 1ULL << 60ULL, _T_4F3411E5_exbibyte, _LEN_4F3411E5, "%.18f " },
			{ 1ULL << 50ULL, _T_7D66D3A4_pebibyte, _LEN_7D66D3A4, "%.15f " },
			{ 1ULL << 40ULL, _T_F92CDD5E_tebibyte, _LEN_F92CDD5E, "%.12f " },
			{ 1ULL << 30ULL, _T_CCB881E2_gibibyte, _LEN_CCB881E2, "%.9f " },
			{ 1ULL << 20ULL, _T_9AD8F75E_mebibyte, _LEN_9AD8F75E, "%.6f " },
			{ 1ULL << 10ULL, _T_9B1794AD_kibibyte, _LEN_9B1794AD, "%.3f " },
		};
		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ); ++I ) {
			if ( _uiSize >= aTable[I].uiSize ) {
				CHAR szNumber[64];
				if ( _uiSize % aTable[I].uiSize == 0 ) {
					::sprintf( szNumber, "%I64u ", _uiSize / aTable[I].uiSize );
				}
				else {
					::sprintf( szNumber, aTable[I].pcFormat, static_cast<DOUBLE>(_uiSize) / static_cast<DOUBLE>(aTable[I].uiSize) );
				}
				_sString.append( szNumber );
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, szBuffer );
				_sString.append( szBuffer );
				if ( _uiSize != aTable[I].uiSize ) {
					_sString.push_back( 's' );
				}
				::ZeroMemory( szBuffer, _T_MAX_LEN );
				return _sString.c_str();
			}
		}
		CHAR szNumber[64];
		::sprintf( szNumber, "%I64u ", _uiSize );
		_sString.append( szNumber );
		_DEC_9DC09A6E_byte( szBuffer );
		_sString.append( szBuffer );
		if ( _uiSize != 1 ) {
			_sString.push_back( 's' );
		}
		::ZeroMemory( szBuffer, _T_MAX_LEN );
		return _sString.c_str();
	}

	// Creates a date string.  _pcRet should be at least 64 characters long.
	const CHAR * CUtilities::CreateDateString( uint64_t _uiTime, CHAR * _pcRet, size_t _sLen ) {
		if ( _uiTime ) {
			time_t tRawTime = _uiTime;
			tm * ptTimeInfo;
			ptTimeInfo = std::localtime( &tRawTime );
			CHAR szTemp[128];
			std::strftime( szTemp, sizeof( szTemp ),"%d-%m-%Y %I:%M:%S %Z", ptTimeInfo);
			::sprintf_s( _pcRet, _sLen, "%s (%I64u)", szTemp, _uiTime );
		}
		else {
			::sprintf_s( _pcRet, _sLen, "%I64u", _uiTime );
		}
		return _pcRet;
	}

	// Creates a date string.
	const CHAR * CUtilities::CreateDateString( uint64_t _uiTime, std::string &_sString ) {
		if ( _uiTime ) {
			time_t tRawTime = _uiTime;
			tm * ptTimeInfo;
			ptTimeInfo = std::localtime( &tRawTime );
			CHAR szTemp[128];
			CHAR szTemp2[128];
			std::strftime( szTemp, sizeof( szTemp ),"%d-%m-%Y %I:%M:%S %Z", ptTimeInfo);
			::sprintf_s( szTemp2, MX_ELEMENTS( szTemp2 ), "%s (%I64u)", szTemp, _uiTime );
			_sString = szTemp2;
		}
		else {
			CHAR szTemp2[128];
			::sprintf_s( szTemp2, MX_ELEMENTS( szTemp2 ), "%I64u", _uiTime );
			_sString = szTemp2;
		}
		return _sString.c_str();
	}

	// Creates a hexadecimal string.  Returns the internal buffer, which means the result must be copied as it will be overwritten when the next function that uses the internal buffer is called.
	const CHAR * CUtilities::ToHex( uint64_t _uiValue, uint32_t _uiNumDigits ) {
		return ToHex( _uiValue, m_szTemp, MX_ELEMENTS( m_szTemp ), _uiNumDigits );
	}
	
	// Creates a hexadecimal string.
	const CHAR * CUtilities::ToHex( uint64_t _uiValue, CHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits ) {
		_uiNumDigits = std::max( _uiNumDigits, 1U );
		CHAR szFormat[32];
		if ( Options.bUse0xForHex ) {
			::sprintf_s( szFormat, MX_ELEMENTS( szFormat ), "0x%%.%uI64X", _uiNumDigits );
		}
		else {
			::sprintf_s( szFormat, MX_ELEMENTS( szFormat ), "%%.%uI64Xh", _uiNumDigits );
		}
		::sprintf_s( _pcRet, _sLen, szFormat, _uiValue );
		return _pcRet;
	}

	// Creates a hexadecimal string.
	const CHAR * CUtilities::ToHex( uint64_t _uiValue, std::string &_sString, uint32_t _uiNumDigits ) {
		CHAR szTemp[32];
		ToHex( _uiValue, szTemp, MX_ELEMENTS( szTemp ), _uiNumDigits );

		_sString += szTemp;
		return _sString.c_str();
	}

	// Creates a hexadecimal string.
	const WCHAR * CUtilities::ToHex( uint64_t _uiValue, WCHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits ) {
		_uiNumDigits = std::max( _uiNumDigits, 1U );
		WCHAR szFormat[32];
		if ( Options.bUse0xForHex ) {
			::swprintf_s( szFormat, MX_ELEMENTS( szFormat ), L"0x%%.%uI64X", _uiNumDigits );
		}
		else {
			::swprintf_s( szFormat, MX_ELEMENTS( szFormat ), L"%%.%uI64Xh", _uiNumDigits );
		}
		::swprintf_s( _pcRet, _sLen, szFormat, _uiValue );
		return _pcRet;
	}

	// Creates a hexadecimal string.
	const WCHAR * CUtilities::ToHex( uint64_t _uiValue, std::wstring &_sString, uint32_t _uiNumDigits ) {
		WCHAR szTemp[32];
		ToHex( _uiValue, szTemp, MX_ELEMENTS( szTemp ), _uiNumDigits );

		_sString += szTemp;
		return _sString.c_str();
	}

	// Creates a binary (0bxxxx) string from an integer value.
	const CHAR * CUtilities::ToBinary( uint64_t _uiValue, std::string &_sString, uint32_t _uiNumDigits ) {
		_uiNumDigits = EE_MAX_( _uiNumDigits, 1 );
		// Determine max number of bits set in the number.
		uint64_t uiCopy = _uiValue;
		uint32_t uiTotalDigits = 0;
		while ( uiCopy ) {
			++uiTotalDigits;
			uiCopy >>= 1;
		}

		_uiNumDigits = EE_MAX_( _uiNumDigits, uiTotalDigits );
		_sString.push_back( '0' );
		_sString.push_back( 'b' );
		for ( uint64_t I = _uiNumDigits; I--; ) {
			_sString.push_back( (_uiValue & (1ULL << I)) ? '1' : '0' );
		}
		return _sString.c_str();
	}

	// Creates a binary (0bxxxx) string from an integer value.
	const WCHAR * CUtilities::ToBinary( uint64_t _uiValue, std::wstring &_sString, uint32_t _uiNumDigits ) {
		_uiNumDigits = EE_MAX_( _uiNumDigits, 1 );
		// Determine max number of bits set in the number.
		uint64_t uiCopy = _uiValue;
		uint32_t uiTotalDigits = 0;
		while ( uiCopy ) {
			++uiTotalDigits;
			uiCopy >>= 1;
		}

		_uiNumDigits = EE_MAX_( _uiNumDigits, uiTotalDigits );
		_sString.push_back( L'0' );
		_sString.push_back( L'b' );
		for ( uint64_t I = _uiNumDigits; I--; ) {
			_sString.push_back( (_uiValue & (1ULL << I)) ? L'1' : L'0' );
		}
		return _sString.c_str();
	}

	// Creates an unsigned integer string.  Returns the internal buffer, which means the result must be copied as it will be overwritten when the next function that uses the internal buffer is called.
	const CHAR * CUtilities::ToUnsigned( uint64_t _uiValue, uint32_t _uiNumDigits ) {
		return ToUnsigned( _uiValue, m_szTemp, MX_ELEMENTS( m_szTemp ), _uiNumDigits );
	}

	// Creates an unsigned integer string.
	const CHAR * CUtilities::ToUnsigned( uint64_t _uiValue, CHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits ) {
		_uiNumDigits = std::max( _uiNumDigits, 1U );
		CHAR szFormat[32];
		::sprintf_s( szFormat, MX_ELEMENTS( szFormat ), "%%.%uI64u", _uiNumDigits );
		::sprintf_s( _pcRet, _sLen, szFormat, _uiValue );
		return _pcRet;
	}

	// Creates an unsigned integer string.
	const CHAR * CUtilities::ToUnsigned( uint64_t _uiValue, std::string &_sString, uint32_t _uiNumDigits ) {
		CHAR szTemp[32];
		ToUnsigned( _uiValue, szTemp, MX_ELEMENTS( szTemp ), _uiNumDigits );

		_sString += szTemp;
		return _sString.c_str();
	}

	// Creates an unsigned integer string.
	const WCHAR * CUtilities::ToUnsigned( uint64_t _uiValue, WCHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits ) {
		_uiNumDigits = std::max( _uiNumDigits, 1U );
		WCHAR szFormat[32];
		::swprintf_s( szFormat, MX_ELEMENTS( szFormat ), L"%%.%uI64u", _uiNumDigits );
		::swprintf_s( _pcRet, _sLen, szFormat, _uiValue );
		return _pcRet;
	}

	// Creates an unsigned integer string.
	const WCHAR * CUtilities::ToUnsigned( uint64_t _uiValue, std::wstring &_sString, uint32_t _uiNumDigits ) {
		WCHAR szTemp[32];
		ToUnsigned( _uiValue, szTemp, MX_ELEMENTS( szTemp ), _uiNumDigits );

		_sString += szTemp;
		return _sString.c_str();
	}

	// Creates a signed integer string.
	const CHAR * CUtilities::ToSigned( int64_t _iValue, CHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits ) {
		_uiNumDigits = std::max( _uiNumDigits, 1U );
		CHAR szFormat[32];
		::sprintf_s( szFormat, MX_ELEMENTS( szFormat ), "%%.%uI64d", _uiNumDigits );
		::sprintf_s( _pcRet, _sLen, szFormat, _iValue );
		return _pcRet;
	}

	// Creates a signed integer string.
	const CHAR * CUtilities::ToSigned( int64_t _iValue, std::string &_sString, uint32_t _uiNumDigits ) {
		CHAR szTemp[32];
		ToSigned( _iValue, szTemp, MX_ELEMENTS( szTemp ), _uiNumDigits );

		_sString += szTemp;
		return _sString.c_str();
	}

	// Creates an signed integer string.
	const WCHAR * CUtilities::ToSigned( int64_t _iValue, WCHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits ) {
		_uiNumDigits = std::max( _uiNumDigits, 1U );
		WCHAR szFormat[32];
		::swprintf_s( szFormat, MX_ELEMENTS( szFormat ), L"%%.%uI64d", _uiNumDigits );
		::swprintf_s( _pcRet, _sLen, szFormat, _iValue );
		return _pcRet;
	}

	// Creates an signed integer string.
	const WCHAR * CUtilities::ToSigned( int64_t _iValue, std::wstring &_sString, uint32_t _uiNumDigits ) {
		WCHAR szTemp[32];
		ToSigned( _iValue, szTemp, MX_ELEMENTS( szTemp ), _uiNumDigits );

		_sString += szTemp;
		return _sString.c_str();
	}

	// Creates a double string.
	const CHAR * CUtilities::ToDouble( double _dValue, std::string &_sString, int32_t _iSigDigits ) {
		const size_t sLen = sizeof( m_szFloatPrintBuffer ) / sizeof( CHAR );
		LSW_ENT_CRIT( m_csCrit );
		/*CHAR * pcBuffer = new( std::nothrow ) CHAR[sLen];
		if ( !pcBuffer ) { return nullptr; }*/
		CHAR * pcBuffer = reinterpret_cast<CHAR *>(m_szFloatPrintBuffer);
		int iLen = 0;
		if ( _iSigDigits == 0 ) {
			iLen = ::sprintf_s( pcBuffer, sLen, "%.2000f", _dValue );
		}
		else if ( _iSigDigits < 0 ) {
			CHAR szFormat[32];
			::sprintf_s( szFormat, MX_ELEMENTS( szFormat ), "%%.%ug", -_iSigDigits );
			iLen = ::sprintf_s( pcBuffer, sLen, szFormat, _dValue );
		}
		else {
			CHAR szFormat[32];
			::sprintf_s( szFormat, MX_ELEMENTS( szFormat ), "%%.%ue", _iSigDigits );
			iLen = ::sprintf_s( pcBuffer, sLen, szFormat, _dValue );
		}
		_sString.append( pcBuffer, iLen );
		/*delete [] pcBuffer;
		pcBuffer = nullptr;*/
		if ( _iSigDigits == 0 ) {
#define MX_LAST ((_sString.size() > 0) ? _sString[_sString.size()-1] : '\0')
			while ( MX_LAST == '0' ) {
				_sString.pop_back();
			}
			if ( MX_LAST == '.' ) {
				double dCheckMe = static_cast<double>(static_cast<uint64_t>(-1));
				if ( std::abs( _dValue ) > static_cast<double>(static_cast<uint64_t>(-1)) ) {
					_sString.push_back( '0' );
				}
				else {
					while ( MX_LAST == '.' ) {
						_sString.pop_back();
					}
				}
			}
		}
		return _sString.c_str();
#undef MX_LAST
	}

	// Creates a double string.
	const WCHAR * CUtilities::ToDouble( double _dValue, std::wstring &_sString, int32_t _iSigDigits ) {
		const size_t sLen = sizeof( m_szFloatPrintBuffer ) / sizeof( WCHAR );
		LSW_ENT_CRIT( m_csCrit );
		/*WCHAR * pcBuffer = new( std::nothrow ) WCHAR[sLen];
		if ( !pcBuffer ) { return nullptr; }*/
		WCHAR * pcBuffer = m_szFloatPrintBuffer;
		int iLen = 0;
		if ( _iSigDigits == 0 ) {
			iLen = ::swprintf_s( pcBuffer, sLen, L"%.2000f", _dValue );
		}
		else if ( _iSigDigits < 0 ) {
			WCHAR szFormat[32];
			::swprintf_s( szFormat, MX_ELEMENTS( szFormat ), L"%%.%ug", -_iSigDigits );
			iLen = ::swprintf_s( pcBuffer, sLen, szFormat, _dValue );
		}
		else {
			WCHAR szFormat[32];
			::swprintf_s( szFormat, MX_ELEMENTS( szFormat ), L"%%.%ue", _iSigDigits );
			iLen = ::swprintf_s( pcBuffer, sLen, szFormat, _dValue );
		}
		_sString.append( pcBuffer, iLen );
		/*delete [] pcBuffer;
		pcBuffer = nullptr;*/
		if ( _iSigDigits == 0 ) {
#define MX_LAST ((_sString.size() > 0) ? _sString[_sString.size()-1] : L'\0')
			while ( MX_LAST == L'0' ) {
				_sString.pop_back();
			}
			if ( MX_LAST == L'.' ) {
				double dCheckMe = static_cast<double>(static_cast<uint64_t>(-1));
				if ( std::abs( _dValue ) > static_cast<double>(static_cast<uint64_t>(-1)) ) {
					_sString.push_back( L'0' );
				}
				else {
					while ( MX_LAST == L'.' ) {
						_sString.pop_back();
					}
				}
			}
		}
		return _sString.c_str();
#undef MX_LAST
	}

	// Gets the next UTF-16 character from a stream or error (MX_UTF_INVALID)
	uint32_t CUtilities::NextUtf16Char( const wchar_t * _pwcString, size_t _sLen, size_t * _psSize ) {
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
				return MX_UTF_INVALID;
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
				return MX_UTF_INVALID;
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

	// Gets the next UTF-8 character from a stream or error (MX_UTF_INVALID)
	uint32_t CUtilities::NextUtf8Char( const char * _pcString, size_t _sLen, size_t * _psSize ) {
		/*mbstate_t sState;
		std::mbsinit( &sState );
		char16_t szTemp[2] = { 0 };
		size_t sLen = std::mbrtoc16( szTemp, _pcString, _sLen, &sState );*/

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
				return MX_UTF_INVALID;
			}
		}

		// Bounds checking.
		if ( ui32Len > _sLen ) {
			if ( _psSize ) { (*_psSize) = _sLen; }
			return MX_UTF_INVALID;
		}

		// We know the size now, so set it.
		// Even if we return an invalid character we want to return the correct number of
		//	bytes to skip.
		if ( _psSize ) { (*_psSize) = ui32Len; }

		// If the length is greater than 4, it is invalid.
		if ( ui32Len > 4 ) {
			// Invalid sequence.
			return MX_UTF_INVALID;
		}

		// Mask out the leading bits.
		ui32Ret &= ~ui32Mask;

		// For every trailing bit, add it to the final value.
		for ( I = ui32Len - 1; I--; ) {
			uint32_t ui32This = (*reinterpret_cast<const uint8_t *>(++_pcString));
			// Validate the byte.
			if ( (ui32This & 0xC0U) != 0x80U ) {
				// Invalid.
				return MX_UTF_INVALID;
			}

			ui32Ret <<= 6;
			ui32Ret |= (ui32This & 0x3F);
		}

		// Finally done.
		return ui32Ret;
	}

	// Converts a UTF-32 character to a UTF-16 character.
	uint32_t CUtilities::Utf32ToUtf16( uint32_t _ui32Utf32, uint32_t &_ui32Len ) {
		if ( _ui32Utf32 > 0x10FFFF ) {
			_ui32Len = 1;
			return MX_UTF_INVALID;
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
	uint32_t CUtilities::Utf32ToUtf8( uint32_t _ui32Utf32, uint32_t &_ui32Len ) {
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
			return MX_UTF_INVALID;
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

	// Counts the number of bytes (not bits) set in the given 64-bit value.
	uint32_t CUtilities::CountSetBytes( uint64_t _ui64Value ) {
		uint32_t uiRet = 0;
		while ( _ui64Value ) {
			++uiRet;
			_ui64Value >>= 8;
		}
		return uiRet;
	}

	// Creates a string with the given data interpreted as a given type.
	const WCHAR * CUtilities::ToDataTypeString( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtType, std::wstring &_sString,
		bool _bMustPrintNumber ) {
		ee::CExpEvalContainer::EE_RESULT eCopy;
		//ee::CExpEvalContainer::ConvertResult
		switch ( _dtType ) {
			case MX_DT_FLOAT16 : {
				eCopy = DetailedConvertResult( _rRes, _dtType );
				if ( _bMustPrintNumber && std::isinf( CFloat16( eCopy.u.dVal ).Value() ) ) {
					_sString += CFloat16( eCopy.u.dVal ).Value() > 0.0 ? _DEC_WS_90D8D134_as_float16_0x7C00_ :
						_DEC_WS_3C64313D__as_float16_0x7C00_;
				}
				else if ( _bMustPrintNumber && std::isnan( CFloat16( eCopy.u.dVal ).Value() ) ) {
					_sString += _DEC_WS_B5B38EE8_as_float16_0x7E00_;
				}
				else {
					CUtilities::ToDouble( CFloat16( eCopy.u.dVal ).Value(), _sString );
				}
				break;
			}
			case MX_DT_FLOAT : {
				eCopy = DetailedConvertResult( _rRes, _dtType );
				if ( _bMustPrintNumber && std::isinf( static_cast<float>(eCopy.u.dVal) ) ) {
					_sString += static_cast<float>(eCopy.u.dVal) > 0.0 ? _DEC_WS_15349BDB_as_float32_0x7F800000_ :
						_DEC_WS_56CE0A86__as_float32_0x7F800000_;
				}
				else if ( _bMustPrintNumber && std::isnan( static_cast<float>(eCopy.u.dVal) ) ) {
					_sString += _DEC_WS_FB5B1F0A_as_float32_0x7FC00000_;
				}
				else {
					CUtilities::ToDouble( static_cast<float>(eCopy.u.dVal), _sString );
				}
				break;
			}
			case MX_DT_DOUBLE : {
				eCopy = DetailedConvertResult( _rRes, _dtType );
				if ( _bMustPrintNumber && std::isinf( static_cast<double>(eCopy.u.dVal) ) ) {
					_sString += static_cast<double>(eCopy.u.dVal) > 0.0 ? _DEC_WS_73C8F4F3_as_float64_0x7FF0000000000000_ :
						_DEC_WS_5F0E778A__as_float64_0x7FF0000000000000_;
				}
				else if ( _bMustPrintNumber && std::isnan( static_cast<double>(eCopy.u.dVal) ) ) {
					_sString += _DEC_WS_4091EAC5_as_float64_0x7FF8000000000000_;
				}
				else {
					CUtilities::ToDouble( static_cast<double>(eCopy.u.dVal), _sString );
				}
				break;
			}
			
#define MX_TO_S( TYPE, CAST )																\
	case TYPE : {																			\
		eCopy = DetailedConvertResult( _rRes, _dtType );									\
		CUtilities::ToSigned( static_cast<CAST>(eCopy.u.i64Val), _sString );				\
		break;																				\
	}
			MX_TO_S( MX_DT_INT8, int8_t )
			MX_TO_S( MX_DT_INT16, int16_t )
			MX_TO_S( MX_DT_INT32, int32_t )
			MX_TO_S( MX_DT_INT64, int64_t )
#undef MX_TO_S

#define MX_TO_US( TYPE, CAST )																\
	case TYPE : {																			\
		eCopy = DetailedConvertResult( _rRes, _dtType );									\
		CUtilities::ToUnsigned( static_cast<CAST>(eCopy.u.ui64Val), _sString );				\
		break;																				\
	}
			MX_TO_US( MX_DT_UINT8, uint8_t )
			MX_TO_US( MX_DT_UINT16, uint16_t )
			MX_TO_US( MX_DT_UINT32, uint32_t )
			MX_TO_US( MX_DT_UINT64, uint64_t )
#undef MX_TO_US
		}

		return _sString.c_str();
	}

	// Returns -1 if the given result cast to the given type is -inf, 1 if it is +inf, otherwise 0.
	int32_t CUtilities::DataTypeIsInf( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtType ) {
		ee::CExpEvalContainer::EE_RESULT eCopy;
		switch ( _dtType ) {
			case MX_DT_FLOAT16 : {
				eCopy = ee::CExpEvalContainer::ConvertResult( _rRes, ee::EE_NC_FLOATING );
				if ( std::isinf( CFloat16( eCopy.u.dVal ).Value() ) ) {
					return eCopy.u.dVal > 0.0 ? 1 : -1;
				}
				return 0;
			}
			case MX_DT_FLOAT : {
				eCopy = ee::CExpEvalContainer::ConvertResult( _rRes, ee::EE_NC_FLOATING );
				if ( std::isinf( static_cast<float>(eCopy.u.dVal) ) ) {
					return eCopy.u.dVal > 0.0 ? 1 : -1;
				}
				return 0;
			}
			case MX_DT_DOUBLE : {
				eCopy = ee::CExpEvalContainer::ConvertResult( _rRes, ee::EE_NC_FLOATING );
				if ( std::isinf( static_cast<double>(eCopy.u.dVal) ) ) {
					return eCopy.u.dVal > 0.0 ? 1 : -1;
				}
				return 0;
			}
			default : { return 0; }
		}
	}

	// Returns true if the given result cast to the given type is nan.
	bool CUtilities::DataTypeIsNan( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtType ) {
		ee::CExpEvalContainer::EE_RESULT eCopy;
		switch ( _dtType ) {
			case MX_DT_FLOAT16 : {
				eCopy = ee::CExpEvalContainer::ConvertResult( _rRes, ee::EE_NC_FLOATING );
				return std::isnan( CFloat16( eCopy.u.dVal ).Value() );
			}
			case MX_DT_FLOAT : {
				eCopy = ee::CExpEvalContainer::ConvertResult( _rRes, ee::EE_NC_FLOATING );
				return std::isnan( static_cast<float>(eCopy.u.dVal) );
			}
			case MX_DT_DOUBLE : {
				eCopy = ee::CExpEvalContainer::ConvertResult( _rRes, ee::EE_NC_FLOATING );
				return std::isnan( static_cast<double>(eCopy.u.dVal) );
			}
			default : { return false; }
		}
	}

	// Gets the size of a data type.
	DWORD CUtilities::DataTypeSize( CUtilities::MX_DATA_TYPES _dtType ) {
		for ( size_t I = 0; I < MX_ELEMENTS( DataTypeInfo ); ++I ) {
			if ( DataTypeInfo[I].dtType == _dtType ) { return DataTypeInfo[I].dwSize; }
		}
		return 0;
	}

	// Gets the alignment of a data type.
	DWORD CUtilities::DataTypeAlign( CUtilities::MX_DATA_TYPES _dtType ) {
		for ( size_t I = 0; I < MX_ELEMENTS( DataTypeInfo ); ++I ) {
			if ( DataTypeInfo[I].dtType == _dtType ) { return DataTypeInfo[I].dwAlign; }
		}
		return 0;
	}

	// Is the data type a float type?
	BOOL CUtilities::DataTypeIsFloat( CUtilities::MX_DATA_TYPES _dtType ) {
		for ( size_t I = 0; I < MX_ELEMENTS( DataTypeInfo ); ++I ) {
			if ( DataTypeInfo[I].dtType == _dtType ) { return DataTypeInfo[I].bIsFloat; }
		}
		return FALSE;
	}

	// Is the data type signed?
	BOOL CUtilities::DataTypeIsSigned( CUtilities::MX_DATA_TYPES _dtType ) {
		for ( size_t I = 0; I < MX_ELEMENTS( DataTypeInfo ); ++I ) {
			if ( DataTypeInfo[I].dtType == _dtType ) { return DataTypeInfo[I].bIsSigned; }
		}
		return FALSE;
	}

	// Is the data type the max value for the given type?
	BOOL CUtilities::DataTypeIsMax( CUtilities::MX_DATA_TYPE _dtType ) {
		if ( DataTypeIsFloat( _dtType.dtType ) ) {
			switch ( _dtType.dtType ) {
				case CUtilities::MX_DT_FLOAT16 : {
					return _dtType.u.Float64 == CFloat16::Max();
				}
				case CUtilities::MX_DT_FLOAT : {
					return _dtType.u.Float32 == FLT_MAX;
				}
				case CUtilities::MX_DT_DOUBLE : {
					return _dtType.u.Float64 == DBL_MAX;
				}
				default : { return false; }
			}
		}
		uint64_t ui64Mask = ~1ULL >> (64ULL - DataTypeSize( _dtType.dtType ) * 8ULL);
		if ( DataTypeIsSigned( _dtType.dtType ) ) {
			int64_t i64Max = ~1ULL >> (65ULL - DataTypeSize( _dtType.dtType ) * 8ULL);
			return (_dtType.u.Int64 & ui64Mask) == i64Max;
		}
		uint64_t ui64Max = ~1ULL >> (64ULL - DataTypeSize( _dtType.dtType ) * 8ULL);
		return (_dtType.u.UInt64 & ui64Mask) == ui64Max;
	}

	// Is the data type the max value for the given type?
	BOOL CUtilities::DataTypeIsMin( CUtilities::MX_DATA_TYPE _dtType ) {
		if ( DataTypeIsFloat( _dtType.dtType ) ) {
			switch ( _dtType.dtType ) {
				case CUtilities::MX_DT_FLOAT : {
					return _dtType.u.Float32 == -FLT_MAX;
				}
				case CUtilities::MX_DT_DOUBLE : {
					return _dtType.u.Float64 == -DBL_MAX;
				}
				default : { return false; }
			}
		}
		uint64_t ui64Mask = ~1ULL >> (64ULL - DataTypeSize( _dtType.dtType ) * 8ULL);
		if ( DataTypeIsSigned( _dtType.dtType ) ) {
			 int64_t i64Min = 1ULL << ((DataTypeSize( _dtType.dtType ) * 8ULL) - 1);
			 return (_dtType.u.Int64 & ui64Mask) == i64Min;
		}
		return (_dtType.u.UInt64 & ui64Mask) == 0;
	}

	// Gets the range of a data type as a string.
	const CHAR * CUtilities::DataTypeRange( CUtilities::MX_DATA_TYPES _dtType, std::string &_sString ) {
		for ( size_t I = 0; I < MX_ELEMENTS( DataTypeInfo ); ++I ) {
			if ( DataTypeInfo[I].dtType == _dtType ) {
				_sString.append( mx::CStringDecoder::DecodeToString( DataTypeInfo[I].pcRange, DataTypeInfo[I].sRangeLen ) );
				break;
			}
		}
		return _sString.c_str();
	}

	// Gets the range of a data type as a string.
	const WCHAR * CUtilities::DataTypeRange( CUtilities::MX_DATA_TYPES _dtType, std::wstring &_sString ) {
		for ( size_t I = 0; I < MX_ELEMENTS( DataTypeInfo ); ++I ) {
			if ( DataTypeInfo[I].dtType == _dtType ) {
				_sString.append( mx::CStringDecoder::DecodeToWString( DataTypeInfo[I].pcRange, DataTypeInfo[I].sRangeLen ) );
				break;
			}
		}
		return _sString.c_str();
	}

	// Prints the size of the given data type as a string.
	const CHAR * CUtilities::DataTypeSize( CUtilities::MX_DATA_TYPES _dtType, std::string &_sString ) {
		DWORD dwSize = DataTypeSize( _dtType );
		if ( !dwSize ) {
			_sString.append( _DEC_S_5F837256__undefined_ );
		}
		else {
			ToUnsigned( dwSize, _sString );
			_sString.append( dwSize == 1 ? _DEC_S_BB679D6B__byte : _DEC_S_C1D51046__bytes );
		}
		return _sString.c_str();
	}

	// Prints the size of the given data type as a string.
	const WCHAR * CUtilities::DataTypeSize( CUtilities::MX_DATA_TYPES _dtType, std::wstring &_sString ) {
		DWORD dwSize = DataTypeSize( _dtType );
		if ( !dwSize ) {
			_sString.append( _DEC_WS_5F837256__undefined_ );
		}
		else {
			ToUnsigned( dwSize, _sString );
			_sString.append( dwSize == 1 ? _DEC_WS_BB679D6B__byte : _DEC_WS_C1D51046__bytes );
		}
		return _sString.c_str();
	}

	// Prints a data type given the options.
	const CHAR * CUtilities::PrintDataType( std::string &_sString, CUtilities::MX_DATA_TYPES _dtType, DWORD _dwOptions ) {
		if ( _dwOptions == DWINVALID ) {
			_dwOptions = Options.dwDataTypeOptions;
		}

		for ( size_t I = 0; I < MX_ELEMENTS( DataTypeInfo ); ++I ) {
			if ( DataTypeInfo[I].dtType == _dtType ) {
				_sString.append( (_dwOptions & MX_DTO_CODENAMES) ?
					mx::CStringDecoder::DecodeToString( DataTypeInfo[I].pcCodeName, DataTypeInfo[I].sCodeNameLen ) :
					mx::CStringDecoder::DecodeToString( DataTypeInfo[I].pcBasicName, DataTypeInfo[I].sBasicNameLen ) );

				if ( _dwOptions & (MX_DTO_SHOWRANGES | MX_DTO_SHOWSIZES) ) {
					_sString.append( " (" );

					if ( _dwOptions & MX_DTO_SHOWSIZES ) {
						DataTypeSize( _dtType, _sString );

						if ( _dwOptions & MX_DTO_SHOWRANGES ) {
							_sString.append( ", " );
						}
					}

					if ( _dwOptions & MX_DTO_SHOWRANGES ) {
						_sString.append( mx::CStringDecoder::DecodeToString( DataTypeInfo[I].pcRange, DataTypeInfo[I].sRangeLen ) );
					}

					_sString.push_back( ')' );
				}
				break;
			}
		}
		return _sString.c_str();
	}

	// Prints a data type given the options.
	const WCHAR * CUtilities::PrintDataType( std::wstring &_sString, CUtilities::MX_DATA_TYPES _dtType, DWORD _dwOptions ) {
		if ( _dwOptions == DWINVALID ) {
			_dwOptions = Options.dwDataTypeOptions;
		}

		for ( size_t I = 0; I < MX_ELEMENTS( DataTypeInfo ); ++I ) {
			if ( DataTypeInfo[I].dtType == _dtType ) {
				_sString.append( (_dwOptions & MX_DTO_CODENAMES) ?
					mx::CStringDecoder::DecodeToWString( DataTypeInfo[I].pcCodeName, DataTypeInfo[I].sCodeNameLen ) :
					mx::CStringDecoder::DecodeToWString( DataTypeInfo[I].pcBasicName, DataTypeInfo[I].sBasicNameLen ) );

				if ( _dwOptions & (MX_DTO_SHOWRANGES | MX_DTO_SHOWSIZES) ) {
					_sString.append( L" (" );

					if ( _dwOptions & MX_DTO_SHOWSIZES ) {
						DataTypeSize( _dtType, _sString );

						if ( _dwOptions & MX_DTO_SHOWRANGES ) {
							_sString.append( L", " );
						}
					}

					if ( _dwOptions & MX_DTO_SHOWRANGES ) {
						_sString.append( mx::CStringDecoder::DecodeToWString( DataTypeInfo[I].pcRange, DataTypeInfo[I].sRangeLen ) );
					}

					_sString.push_back( L')' );
				}
				break;
			}
		}
		return _sString.c_str();
	}

	// Converts a given value to a string given the type.
	CSecureString & CUtilities::DataTypeToString( const void * _pvData, CUtilities::MX_DATA_TYPES _dtType, CSecureString &_sRet, uint32_t _uiNumDigits, int32_t _iSigDigits ) {
		const MX_DATA_TYPE * _pdtData = static_cast<const MX_DATA_TYPE *>(_pvData);
		switch ( _dtType ) {
#define MX_CASE_PAIR( INT )											\
	case MX_DT_INT ## INT : {										\
		ToSigned( _pdtData->u.Int ## INT, _sRet, _uiNumDigits );	\
		break;														\
	}																\
	case MX_DT_UINT ## INT : {										\
		ToUnsigned( _pdtData->u.UInt ## INT, _sRet, _uiNumDigits );	\
		break;														\
	}

			MX_CASE_PAIR( 8 )
			MX_CASE_PAIR( 16 )
			MX_CASE_PAIR( 32 )
			MX_CASE_PAIR( 64 )
			case MX_DT_FLOAT16 : {
				ToDouble( CFloat16( _pdtData->u.UInt16 ).Value(), _sRet, _iSigDigits );
				break;
			}
			case MX_DT_FLOAT : {
				ToDouble( _pdtData->u.Float, _sRet, _iSigDigits );
				break;
			}
			case MX_DT_DOUBLE : {
				ToDouble( _pdtData->u.Double, _sRet, _iSigDigits );
				break;
			}
			case MX_DT_VOID : {
				char szBuffer[32];
				int iLen = ::sprintf_s( szBuffer, MX_ELEMENTS( szBuffer ), "%.11I64X", _pdtData->u.Pointer64 );
				_sRet.append( szBuffer, iLen );
				break;
			}
#undef MX_CASE_PAIR
		}

		return _sRet;
	}

	// Converts a given value to a string given the type.
	CSecureWString & CUtilities::DataTypeToString( const void * _pvData, CUtilities::MX_DATA_TYPES _dtType, CSecureWString &_swsRet, uint32_t _uiNumDigits, int32_t _iSigDigits ) {
		const MX_DATA_TYPE * _pdtData = static_cast<const MX_DATA_TYPE *>(_pvData);
		switch ( _dtType ) {
#define MX_CASE_PAIR( INT )												\
	case MX_DT_INT ## INT : {											\
		ToSigned( _pdtData->u.Int ## INT, _swsRet, _uiNumDigits );		\
		break;															\
	}																	\
	case MX_DT_UINT ## INT : {											\
		ToUnsigned( _pdtData->u.UInt ## INT, _swsRet, _uiNumDigits );	\
		break;															\
	}

			MX_CASE_PAIR( 8 )
			MX_CASE_PAIR( 16 )
			MX_CASE_PAIR( 32 )
			MX_CASE_PAIR( 64 )
			case MX_DT_FLOAT16 : {
				ToDouble( CFloat16( _pdtData->u.UInt16 ).Value(), _swsRet, _iSigDigits );
				break;
			}
			case MX_DT_FLOAT : {
				ToDouble( _pdtData->u.Float, _swsRet, _iSigDigits );
				break;
			}
			case MX_DT_DOUBLE : {
				ToDouble( _pdtData->u.Double, _swsRet, _iSigDigits );
				break;
			}
			case MX_DT_VOID : {
				wchar_t szBuffer[32];
				int iLen = ::swprintf_s( szBuffer, MX_ELEMENTS( szBuffer ), L"%.11I64X", _pdtData->u.Pointer64 );
				_swsRet.append( szBuffer, iLen );
				break;
			}
#undef MX_CASE_PAIR
		}

		return _swsRet;
	}

	// Performs a more detailed conversion of a result to one of our data types.
	ee::CExpEvalContainer::EE_RESULT CUtilities::DetailedConvertResult( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtType ) {
		ee::CExpEvalContainer::EE_RESULT rRet;
		if ( DataTypeIsFloat( _dtType ) ) {
			rRet.ncType = ee::EE_NC_FLOATING;
		}
		else if ( DataTypeIsSigned( _dtType ) ) {
			rRet.ncType = ee::EE_NC_SIGNED;
		}
		else {
			rRet.ncType = ee::EE_NC_UNSIGNED;
		}
		
		switch ( _rRes.ncType ) {
#define MX_CASE( TYPE, CASTTYPE, MEMBERTYPE, MEMBER, BACKTYPE, BACKMEMBER )									\
	case TYPE : {																							\
		rRet.u.BACKMEMBER = static_cast<BACKTYPE>(CASTTYPE( static_cast<MEMBERTYPE>(_rRes.u.MEMBER) ));		\
		return rRet;																						\
	}
			case ee::EE_NC_FLOATING : {

#define MX_BIG_CASE( MEMBERTYPE, MEMBER )																	\
	switch ( _dtType ) {																					\
		MX_CASE( MX_DT_INT8, int8_t, MEMBERTYPE, MEMBER, int64_t, i64Val )									\
		MX_CASE( MX_DT_INT16, int16_t, MEMBERTYPE, MEMBER, int64_t, i64Val )								\
		MX_CASE( MX_DT_INT32, int32_t, MEMBERTYPE, MEMBER, int64_t, i64Val )								\
		MX_CASE( MX_DT_INT64, int64_t, MEMBERTYPE, MEMBER, int64_t, i64Val )								\
		MX_CASE( MX_DT_UINT8, uint8_t, MEMBERTYPE, MEMBER, uint64_t, ui64Val )								\
		MX_CASE( MX_DT_UINT16, uint16_t, MEMBERTYPE, MEMBER, uint64_t, ui64Val )							\
		MX_CASE( MX_DT_UINT32, uint32_t, MEMBERTYPE, MEMBER, uint64_t, ui64Val )							\
		MX_CASE( MX_DT_UINT64, uint64_t, MEMBERTYPE, MEMBER, uint64_t, ui64Val )							\
		MX_CASE( MX_DT_FLOAT16, CFloat16, double, MEMBER, double, dVal )									\
		MX_CASE( MX_DT_FLOAT, float, MEMBERTYPE, MEMBER, double, dVal )										\
		MX_CASE( MX_DT_DOUBLE, double, MEMBERTYPE, MEMBER, double, dVal )									\
	}

				MX_BIG_CASE( double, dVal )
				break;
			}
			case ee::EE_NC_SIGNED : {
				MX_BIG_CASE( int64_t, i64Val )
				break;
			}
			case ee::EE_NC_UNSIGNED : {
				MX_BIG_CASE( uint64_t, ui64Val )
				break;
			}

#undef MX_BIG_CASE
#undef MX_CASE
		}

		return ee::CExpEvalContainer::ConvertResult( _rRes, rRet.ncType );
	}

	// Converts a MX_REGEX_ENCODING value to an actual code page.
	UINT CUtilities::RegexCodePageToCodePage( MX_REGEX_ENCODING _reEncoding ) {
		const struct {
			MX_REGEX_ENCODING		reValue;
			UINT					uiRet;
		} sTable[] = {
			{ MX_RE_ASCII,			CCodePages::MX_us_ascii },
			{ MX_RE_UTF8,			CCodePages::MX_utf_8 },
			{ MX_RE_UTF16_BE,		CCodePages::MX_utf_16BE },
			{ MX_RE_UTF16_LE,		CCodePages::MX_utf_16 },
			{ MX_RE_UTF32_BE,		CCodePages::MX_utf_32BE },
			{ MX_RE_UTF32_LE,		CCodePages::MX_utf_32 },
			{ MX_RE_ISO_8859_1,		CCodePages::MX_iso_8859_1 },
			{ MX_RE_ISO_8859_2,		CCodePages::MX_iso_8859_2 },
			{ MX_RE_ISO_8859_3,		CCodePages::MX_iso_8859_3 },
			{ MX_RE_ISO_8859_4,		CCodePages::MX_iso_8859_4 },
			{ MX_RE_ISO_8859_5,		CCodePages::MX_iso_8859_5 },
			{ MX_RE_ISO_8859_6,		CCodePages::MX_iso_8859_6 },
			{ MX_RE_ISO_8859_7,		CCodePages::MX_iso_8859_7 },
			{ MX_RE_ISO_8859_8,		CCodePages::MX_iso_8859_8 },
			{ MX_RE_ISO_8859_9,		CCodePages::MX_iso_8859_9 },
			{ MX_RE_ISO_8859_10,	28600 },
			{ MX_RE_ISO_8859_11,	874 },
			{ MX_RE_ISO_8859_13,	CCodePages::MX_iso_8859_13 },
			{ MX_RE_ISO_8859_14,	28604 },
			{ MX_RE_ISO_8859_15,	CCodePages::MX_iso_8859_15 },
			{ MX_RE_ISO_8859_16,	28606 },
			{ MX_RE_EUC_JP,			CCodePages::MX_euc_jp },
			{ MX_RE_EUC_TW,			51950 },	// EUC Traditional Chinese
			{ MX_RE_EUC_KR,			CCodePages::MX_euc_kr },
			{ MX_RE_EUC_CN,			CCodePages::MX_EUC_CN },
			{ MX_RE_S_JIS,			CCodePages::MX_shift_jis },
			{ MX_RE_KOI_8,			CCodePages::MX_koi8_u },
			{ MX_RE_KOI8_R,			CCodePages::MX_koi8_r },
			{ MX_RE_WINDOWS_1251,	CCodePages::MX_windows_1251 },
			{ MX_RE_BIG_5,			CCodePages::MX_big5 },
			{ MX_RE_GB_18030,		CCodePages::MX_GB18030 },
		};
		for ( size_t I = MX_ELEMENTS( sTable ); I--; ) {
			if ( sTable[I].reValue == _reEncoding ) {
				return sTable[I].uiRet;
			}
		}
		return CP_UTF8;
	}

	// MX_SEARCH_TYPES value to a string.
	const WCHAR * CUtilities::SearchTypeToString( CUtilities::MX_SEARCH_TYPES _stType, std::wstring &_sString ) {
		static const struct {
			const char *				pcName;
			size_t						sLen;
			uint32_t					ui32Id;
		} aData[] = {
			{ _T_LEN_2CB9A1E0_Data_Type_Search,	MX_ST_DATATYPE_SEARCH },
			{ _T_LEN_A26A67DC_Pointer_Search,	MX_ST_POINTER_SEARCH },
			{ _T_LEN_9468D604_String_Search,	MX_ST_STRING_SEARCH },
			{ _T_LEN_408D6284_Expression_Search,MX_ST_EXP_SEARCH },
			{ _T_LEN_62CB7D7B_Group_Search,		MX_ST_GROUP_SEARCH },
		};

		for ( size_t I = MX_ELEMENTS( aData ); I--; ) {
			if ( aData[I].ui32Id == _stType ) {
				_sString += mx::CStringDecoder::DecodeToWString( aData[I].pcName, aData[I].sLen );
				return _sString.c_str();
			}
		}

		_sString += _DEC_WS_629B9E5B_Unknown;
		return _sString.c_str();
	}

	// MX_EVAL_TYPES value to a string.
	const WCHAR * CUtilities::EvaluationTypeToString( CUtilities::MX_EVAL_TYPES _etType, std::wstring &_sString ) {
		static const struct {
			const char *				pcName;
			size_t						sLen;
			uint32_t					ui32Id;
		} aData[] = {
			{ _T_LEN_396582B1_Exact_Value,			MX_ET_EXACT },
			{ _T_LEN_474E42C3_Greater_Than,			MX_ET_GREATER_THAN },
			{ _T_LEN_C8A1ADFC_Lower_Than,			MX_ET_LESS_THAN },
			{ _T_LEN_5246754D_Range,				MX_ET_RANGE },
			{ _T_LEN_629B9E5B_Unknown,				MX_ET_UNKNOWN },
			{ _T_LEN_8558CA08_Quick_Expression,		MX_ET_QUICK_EXP },
		};

		for ( size_t I = MX_ELEMENTS( aData ); I--; ) {
			if ( aData[I].ui32Id == _etType ) {
				_sString += mx::CStringDecoder::DecodeToWString( aData[I].pcName, aData[I].sLen );
				return _sString.c_str();
			}
		}

		_sString += _DEC_WS_629B9E5B_Unknown;
		return _sString.c_str();
	}

	// Converts an ee::CExpEvalContainer::EE_RESULT to a CUtilities::MX_DATA_TYPE with the given data type.
	CUtilities::MX_DATA_TYPE CUtilities::ExpEvalResultToDataType( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtDataType ) {
		CUtilities::MX_DATA_TYPE dtRet;
		dtRet.dtType = _dtDataType;
		switch ( _dtDataType ) {
#define MX_CASE( CASE, C_TYPE, CONTAINER_TYPE, DATA_TYPE_MEMBER, RESULT_MEMBER )										\
	case CASE : {																										\
		ee::CExpEvalContainer::EE_RESULT eCopy = ee::CExpEvalContainer::ConvertResult( _rRes, ee::CONTAINER_TYPE );		\
		dtRet.u.DATA_TYPE_MEMBER = static_cast<C_TYPE>(eCopy.u.RESULT_MEMBER);											\
		return dtRet;																									\
	}
			MX_CASE( MX_DT_INT8, int8_t, EE_NC_SIGNED, Int64, i64Val )
			MX_CASE( MX_DT_INT16, int16_t, EE_NC_SIGNED, Int64, i64Val )
			MX_CASE( MX_DT_INT32, int32_t, EE_NC_SIGNED, Int64, i64Val )
			MX_CASE( MX_DT_INT64, int64_t, EE_NC_SIGNED, Int64, i64Val )

			MX_CASE( MX_DT_UINT8, uint8_t, EE_NC_UNSIGNED, UInt64, ui64Val )
			MX_CASE( MX_DT_UINT16, uint16_t, EE_NC_UNSIGNED, UInt64, ui64Val )
			MX_CASE( MX_DT_UINT32, uint32_t, EE_NC_UNSIGNED, UInt64, ui64Val )
			MX_CASE( MX_DT_UINT64, uint64_t, EE_NC_UNSIGNED, UInt64, ui64Val )

			MX_CASE( MX_DT_FLOAT, float, EE_NC_FLOATING, Float32, dVal )
			MX_CASE( MX_DT_DOUBLE, double, EE_NC_FLOATING, Float64, dVal )
			case MX_DT_FLOAT16 : {
				ee::CExpEvalContainer::EE_RESULT eCopy = ee::CExpEvalContainer::ConvertResult( _rRes, ee::EE_NC_FLOATING );
				dtRet.u.Float64 = CFloat16( eCopy.u.dVal ).Value();
				return dtRet;
			}

#undef MX_CASE
		}
		dtRet.dtType = MX_DT_VOID;
		return dtRet;
	}

	// Clears the internal temporary buffer (as a security measure).
	VOID CUtilities::ClearInternalBuffer() {
		std::memset( m_szTemp, 0, sizeof( m_szTemp ) );
	}

	// Converts a wstring to a UTF-8 string.
	CSecureString CUtilities::WStringToString( const CSecureWString &_wsIn ) {
#if 0
		// Problematic; terminates at the first \0 character, making it impossible to use for strings meant for OPENFILENAMEW etc.
		//return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes( _wsIn );
		// Instead, convert up to the \0 and add the \0 manually and then keep converting.
		CSecureString ssTemp;
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
		CSecureString ssTemp;
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
	CSecureWString CUtilities::StringToWString( const std::string &_sIn ) {
#if 0
		// Problematic; terminates at the first \0 character, making it impossible to use for strings meant for OPENFILENAMEW etc.
		//return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes( _sIn.data() );
		
		// Instead, convert up to the \0 and add the \0 manually and then keep converting.
		CSecureWString wTemp;
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
	CSecureWString CUtilities::StringToWString( const char * _pcIn, size_t _sLen ) {
		CSecureWString swsTemp;
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
	CSecureWString CUtilities::Utf32StringToWString( const uint32_t * _puiUtf32String, size_t _sLen ) {
		CSecureWString swsTemp;
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

	// Converts a wstring to a UTF8 string.
	CSecureString CUtilities::ToUtf8( const std::wstring &_wsString ) {
		CSecureString sRet;
		const wchar_t * pwcInput = _wsString.c_str();
		size_t sSize = _wsString.size();

		for ( size_t I = 0; I < sSize; ) {
			size_t sThisSize = 0;
			uint32_t ui32Char = NextUtf16Char( &pwcInput[I], sSize - I, &sThisSize );
			if ( ui32Char == MX_UTF_INVALID ) {
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

	// Converts from UTF-8 to UTF-16, copying the original characters instead of MX_UTF_INVALID as StringToWString() would.
	CSecureWString CUtilities::ToUtf16( const std::string &_sIn ) {
		CSecureWString swsTemp;
		const char * pcSrc = _sIn.c_str();
		size_t sSize = _sIn.size();

		for ( size_t I = 0; I < sSize; ) {
			size_t sThisSize = 0;
			uint32_t ui32This = NextUtf8Char( &pcSrc[I], _sIn.size() - I, &sThisSize );
			if ( ui32This == MX_UTF_INVALID ) {
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
				swsTemp.push_back( static_cast<wchar_t>(ui32Converted & 0xFFFFU) );
				ui32Converted >>= 16;
			}
		}
		return swsTemp;
	}

	// Converts from UTF-8 to UTF-32, copying the original characters instead of MX_UTF_INVALID.
	std::vector<uint32_t> CUtilities::ToUtf32( const std::string &_sIn ) {
		std::vector<uint32_t> vReturn;
		const char * pcSrc = _sIn.c_str();
		size_t sSize = _sIn.size();

		for ( size_t I = 0; I < sSize; ) {
			size_t sThisSize = 0;
			uint32_t ui32This = NextUtf8Char( &pcSrc[I], _sIn.size() - I, &sThisSize );
			if ( ui32This == MX_UTF_INVALID ) {
				for ( size_t J = 0; J < sThisSize; ++J ) {
					vReturn.push_back( static_cast<uint8_t>(pcSrc[I+J]) );
				}
				I += sThisSize;
				continue;
			}
			I += sThisSize;
			vReturn.push_back( ui32This );
		}
		return vReturn;
	}

	// Converts from UTF-8 to any single-byte code page.
	CSecureString CUtilities::ToCodePage( const std::string &_sIn, UINT _uiCodePage, bool * _pbError ) {
		if ( _pbError ) { (*_pbError) = false; }
		CSecureWString swsUtf16 = StringToWString( _sIn );
		int iLen = ::WideCharToMultiByte( _uiCodePage, 0, swsUtf16.c_str(), swsUtf16.size(),
			NULL, 0,
			NULL, NULL );
		if ( iLen <= 0 ) {
			if ( _pbError ) { (*_pbError) = true; }
			return CSecureString( _sIn );
		}
		std::vector<uint8_t> vResult;
		if ( !Resize( vResult, iLen ) ) {
			if ( _pbError ) { (*_pbError) = true; }
			return CSecureString( _sIn );
		}
		::WideCharToMultiByte( _uiCodePage, 0, swsUtf16.c_str(), swsUtf16.size(),
			reinterpret_cast<LPSTR>(vResult.data()), vResult.size(),
			NULL, NULL );

		CSecureString ssRet( reinterpret_cast<const CSecureString::value_type *>(vResult.data()), vResult.size() );
		return ssRet;
	}

	// Converts from any single-byte code page to UTF-8.
	CSecureString CUtilities::FromCodePage( const std::string &_sIn, UINT _uiCodePage, bool * _pbError ) {
		if ( _pbError ) { (*_pbError) = false; }

		int iLen = ::MultiByteToWideChar( _uiCodePage, 0, _sIn.c_str(), _sIn.size(),
			NULL, 0 );
		if ( iLen <= 0 ) {
			if ( _pbError ) { (*_pbError) = true; }
			return CSecureString( _sIn );
		}
		std::vector<WCHAR> vResult;
		try { vResult.resize( iLen ); }
		catch ( const std::bad_alloc /*& _eE*/ ) {
			if ( _pbError ) { (*_pbError) = true; }
			return CSecureString( _sIn );
		}
		::MultiByteToWideChar( _uiCodePage, 0, _sIn.c_str(), _sIn.size(),
			reinterpret_cast<LPWSTR>(vResult.data()), vResult.size() );
		CSecureWString swsTmp( reinterpret_cast<const CSecureWString::value_type *>(vResult.data()), vResult.size() );
		return WStringToString( swsTmp );
	}

	// Maps a UTF-16 (wide character) string to a new character string. The new character string is not necessarily from a multibyte character set.
	CSecureString CUtilities::WideCharToMultiByte( UINT _uiCodePage,
		DWORD _dwFlags,
		const std::wstring &_wsString,
		LPCCH _lpDefaultChar,
		LPBOOL _lpUsedDefaultChar,
		DWORD * _pdwLastError ) {
		if ( _pdwLastError ) { (*_pdwLastError) = ERROR_SUCCESS; }
		
		switch ( _uiCodePage ) {
			case CCodePages::MX_utf_16 : {
				// From UTF-16 to UTF-16.  Just copy over.
				CSecureString ssRet;
				for ( size_t I = 0; I < _wsString.size(); ++I ) {
					ssRet.push_back( static_cast<uint8_t>(_wsString[I] >> 0) );
					ssRet.push_back( static_cast<uint8_t>(_wsString[I] >> 8) );
				}
				return ssRet;
			}
			case CCodePages::MX_utf_16BE : {
				// From UTF-16 to UTF-16 BE.  Just perform a byte-swap.
				CSecureString ssRet;
				for ( size_t I = 0; I < _wsString.size(); ++I ) {
					ssRet.push_back( static_cast<uint8_t>(_wsString[I] >> 8) );
					ssRet.push_back( static_cast<uint8_t>(_wsString[I] >> 0) );
				}
				return ssRet;
			}
			case CCodePages::MX_utf_32 : {
				// From UTF-16 to UTF-32.  Convert up and copy.
				CSecureString ssRet;
				size_t sLen = 1;
				for ( size_t I = 0; I < _wsString.size(); I += sLen ) {
					uint32_t ui32This = NextUtf16Char( &_wsString[I], _wsString.size() - I, &sLen );
					if ( ui32This == MX_UTF_INVALID ) {
						if ( _lpDefaultChar ) {
							ui32This = (*_lpDefaultChar);
							if ( _lpUsedDefaultChar ) { (*_lpUsedDefaultChar ) = TRUE; }
						}
						else if ( _dwFlags & WC_ERR_INVALID_CHARS ) {
							if ( _pdwLastError ) { (*_pdwLastError) = ERROR_NO_UNICODE_TRANSLATION; }
							return CSecureString();
						}
					}
					if ( ui32This != MX_UTF_INVALID ) {
						ssRet.push_back( static_cast<uint8_t>(ui32This >> 0) );
						ssRet.push_back( static_cast<uint8_t>(ui32This >> 8) );
						ssRet.push_back( static_cast<uint8_t>(ui32This >> 16) );
						ssRet.push_back( static_cast<uint8_t>(ui32This >> 24) );
					}
				}
				return ssRet;
			}
			case CCodePages::MX_utf_32BE : {
				// From UTF-16 to UTF-32.  Convert up and copy byteswapped.
				CSecureString ssRet;
				size_t sLen = 1;
				for ( size_t I = 0; I < _wsString.size(); I += sLen ) {
					uint32_t ui32This = NextUtf16Char( &_wsString[I], _wsString.size() - I, &sLen );
					if ( ui32This == MX_UTF_INVALID ) {
						if ( _lpDefaultChar ) {
							ui32This = (*_lpDefaultChar);
							if ( _lpUsedDefaultChar ) { (*_lpUsedDefaultChar ) = TRUE; }
						}
						else if ( _dwFlags & WC_ERR_INVALID_CHARS ) {
							if ( _pdwLastError ) { (*_pdwLastError) = ERROR_NO_UNICODE_TRANSLATION; }
							return CSecureString();
						}
					}
					if ( ui32This != MX_UTF_INVALID ) {
						ssRet.push_back( static_cast<uint8_t>(ui32This >> 24) );
						ssRet.push_back( static_cast<uint8_t>(ui32This >> 16) );
						ssRet.push_back( static_cast<uint8_t>(ui32This >> 8) );
						ssRet.push_back( static_cast<uint8_t>(ui32This >> 0) );
					}
				}
				return ssRet;
			}
		}

		int iLen = ::WideCharToMultiByte( _uiCodePage, _dwFlags, _wsString.c_str(), _wsString.size(),
			NULL, 0,
			_lpDefaultChar, _lpUsedDefaultChar );
		if ( iLen <= 0 ) {
			if ( _pdwLastError ) { (*_pdwLastError) = ::GetLastError(); }
			return CSecureString();
		}
		std::vector<uint8_t> vResult;
		if ( !Resize( vResult, iLen ) ) {
			if ( _pdwLastError ) { (*_pdwLastError) = ERROR_NOT_ENOUGH_MEMORY; }
			return CSecureString();
		}

		::WideCharToMultiByte( _uiCodePage, _dwFlags, _wsString.c_str(), _wsString.size(),
			reinterpret_cast<LPSTR>(vResult.data()), vResult.size(),
			_lpDefaultChar, _lpUsedDefaultChar );
		return CSecureString( reinterpret_cast<const CSecureString::value_type *>(vResult.data()), vResult.size() );
	}

	// Maps a character string to a UTF-16 (wide character) string. The character string is not necessarily from a multibyte character set.
	CSecureWString CUtilities::MultiByteToWideChar( UINT _uiCodePage,
		DWORD _dwFlags,
		const std::string &_sString,
		DWORD * _pdwLastError ) {
		if ( _pdwLastError ) { (*_pdwLastError) = ERROR_SUCCESS; }

		switch ( _uiCodePage ) {
			case CCodePages::MX_utf_16 : {
				// From UTF-16 to UTF-16.  Just copy over.
				const WCHAR * _pcSrc = reinterpret_cast<const WCHAR *>(_sString.c_str());
				CSecureWString swsRet;
				for ( size_t I = _sString.size() / sizeof( WCHAR ); I--; ) {
					swsRet.push_back( (*_pcSrc++) );
				}
				return swsRet;
			}
			case CCodePages::MX_utf_16BE : {
				// From bytswapped UTF-16 BE to UTF-16.  Un-byteswap and then copy.
				const WCHAR * _pcSrc = reinterpret_cast<const WCHAR *>(_sString.c_str());
				CSecureWString swsRet;
				for ( size_t I = _sString.size() / sizeof( WCHAR ); I--; ) {
					swsRet.push_back( ::_byteswap_ushort( (*_pcSrc++) ) );
				}
				return swsRet;
			}
			case CCodePages::MX_utf_32 : {
				// 32-bit raw characters to UTF-16.  Convert.
				const uint32_t * _puiSrc = reinterpret_cast<const uint32_t *>(_sString.c_str());
				CSecureWString swsRet;
				for ( size_t I = _sString.size() / sizeof( uint32_t ); I--; ) {
					uint32_t uiLen;
					uint32_t ui32This = Utf32ToUtf16( (*_puiSrc++), uiLen );

					if ( ui32This == MX_UTF_INVALID ) {
						if ( _dwFlags & MB_ERR_INVALID_CHARS ) {
							if ( _pdwLastError ) { (*_pdwLastError) = ERROR_NO_UNICODE_TRANSLATION; }
							return CSecureWString();
						}
						ui32This = Utf32ToUtf16( 0xFFFD, uiLen );
					}
					if ( ui32This != MX_UTF_INVALID ) {
						for ( uint32_t J = 0; J < uiLen; ++J ) {
							swsRet.push_back( static_cast<uint16_t>(ui32This) );
							ui32This >>= 16;
						}	
					}
				}
				return swsRet;
			}
			case CCodePages::MX_utf_32BE : {
				// Byteswapped 32-bit raw characters to UTF-16.  Byteswap and convert.
				const uint32_t * _puiSrc = reinterpret_cast<const uint32_t *>(_sString.c_str());
				CSecureWString swsRet;
				for ( size_t I = _sString.size() / sizeof( uint32_t ); I--; ) {
					uint32_t uiLen;
					uint32_t ui32This = Utf32ToUtf16( ::_byteswap_ulong( (*_puiSrc++) ), uiLen );

					if ( ui32This == MX_UTF_INVALID ) {
						if ( _dwFlags & MB_ERR_INVALID_CHARS ) {
							if ( _pdwLastError ) { (*_pdwLastError) = ERROR_NO_UNICODE_TRANSLATION; }
							return CSecureWString();
						}
						ui32This = Utf32ToUtf16( 0xFFFD, uiLen );
					}
					if ( ui32This != MX_UTF_INVALID ) {
						for ( uint32_t J = 0; J < uiLen; ++J ) {
							swsRet.push_back( static_cast<uint16_t>(ui32This) );
							ui32This >>= 16;
						}	
					}
				}
				return swsRet;
			}
		}

		int iLen = ::MultiByteToWideChar( _uiCodePage, _dwFlags, _sString.c_str(), _sString.size(),
			NULL, 0 );
		if ( iLen <= 0 ) {
			if ( _pdwLastError ) { (*_pdwLastError) = ::GetLastError(); }
			return CSecureWString();
		}

		std::vector<WCHAR> vResult;
		if ( !Resize( vResult, iLen ) ) {
			if ( _pdwLastError ) { (*_pdwLastError) = ERROR_NOT_ENOUGH_MEMORY; }
			return CSecureWString();
		}

		::MultiByteToWideChar( _uiCodePage, _dwFlags, _sString.c_str(), _sString.size(),
			reinterpret_cast<LPWSTR>(vResult.data()), vResult.size() );
		return CSecureWString( reinterpret_cast<const CSecureWString::value_type *>(vResult.data()), vResult.size() );
	}

	// Determines if _dwFlags must be 0 for a call to MultiByteToWideChar().
	bool CUtilities::MultiByteWideChar_FlagsMustBe0( UINT _uiCodePage ) {
		static const UINT uiList[] = {
			CCodePages::MX_iso_2022_jp,
			CCodePages::MX_csISO2022JP,
			CCodePages::MX_iso_2022_jp_,
			CCodePages::MX_iso_2022_kr,
			CCodePages::MX_x_cp50227,
			50229,
			CCodePages::MX_utf_7,
			CCodePages::MX_utf_8,
			42,
			CCodePages::MX_GB18030,
			CCodePages::MX_hz_gb_2312
		};
		for ( size_t I = 0; I < MX_ELEMENTS( uiList ); ++I ) {
			if ( uiList[I] == _uiCodePage ) { return true; }
		}
		return _uiCodePage >= 57002 && _uiCodePage <= 57011;
	}

	// Normalizes characters of a text string according to Unicode 4.0 TR#15.
	CSecureWString CUtilities::NormalizeString( NORM_FORM _nfNormForm,
		const std::wstring &_wsString,
		DWORD * _pdwLastError ) {
		if ( _pdwLastError ) { (*_pdwLastError) = ERROR_SUCCESS; }

		int iLen = ::NormalizeString( _nfNormForm, _wsString.c_str(), _wsString.size(),
			NULL, 0 );
		if ( iLen <= 0 ) {
			if ( _pdwLastError ) { (*_pdwLastError) = ::GetLastError(); }
			return CSecureWString();
		}

		std::vector<WCHAR> vResult;
		iLen = -iLen;
		while ( iLen < 0 ) {
			iLen = -iLen;
			if ( !Resize( vResult, iLen ) ) {
				if ( _pdwLastError ) { (*_pdwLastError) = ERROR_NOT_ENOUGH_MEMORY; }
				return CSecureWString();
			}
			iLen = ::NormalizeString( _nfNormForm, _wsString.c_str(), _wsString.size(),
				(vResult.data()), vResult.size() );
			if ( iLen < 0 ) {
				DWORD dwError = ::GetLastError();
				if ( dwError != ERROR_INSUFFICIENT_BUFFER ) {
					if ( _pdwLastError ) { (*_pdwLastError) = dwError; }
					return CSecureWString();
				}
			}
		}
		return CSecureWString( reinterpret_cast<const CSecureWString::value_type *>(vResult.data()), iLen );
	}

	// Adds escapes to only NULL characters.
	CSecureWString CUtilities::EscapeNulOnly( const CSecureWString &_swsInput, bool _bIncludeBackSlash ) {
		CSecureWString swsOut;
		for ( size_t I = 0; I < _swsInput.size(); ++I ) {
			if ( _swsInput[I] == L'\\' && _bIncludeBackSlash ) {
				swsOut.push_back( L'\\' );
				swsOut.push_back( L'\\' );
			}
			else if ( !_swsInput[I] ) {
				swsOut.push_back( L'\\' );
				swsOut.push_back( L'u' );
				swsOut.push_back( L'0' );
				swsOut.push_back( L'0' );
				swsOut.push_back( L'0' );
				swsOut.push_back( L'0' );
			}
			else {
				swsOut.push_back( _swsInput[I] );
			}
		}
		return swsOut;
	}

	// Adds escapes to all unprintable characters.
	CSecureWString CUtilities::EscapeUnprintable( const CSecureWString &_swsInput, bool _bIncludeBackSlash, bool _bKeepNewline ) {
		CSecureWString swsOut;
		size_t sLen;
		std::vector<WORD> vTypeData, vType3Data;
		if ( !Resize( vTypeData, _swsInput.size() ) ) { return CSecureWString( _swsInput ); }
		::GetStringTypeW( CT_CTYPE1,
			_swsInput.c_str(),
			_swsInput.size(),
			vTypeData.data() );

		if ( !Resize( vType3Data, _swsInput.size() ) ) { return CSecureWString( _swsInput ); }
		::GetStringTypeW( CT_CTYPE3,
			_swsInput.c_str(),
			_swsInput.size(),
			vType3Data.data() );
		static struct {
			wchar_t wcSrc;
			wchar_t wcDst;
		} const sTable[] = {
			{ L'\a', L'a' },
			{ L'\b', L'b' },
			{ L'\f', L'f' },
			{ L'\n', L'n' },
			{ L'\r', L'r' },
			{ L'\t', L't' },
			{ L'\v', L'v' },
		};
		for ( size_t I = 0; I < _swsInput.size();  ) {
			uint32_t ui32This = NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			if ( sLen == 1 ) {
				bool bFound = false;
				for ( size_t J = 0; J < MX_ELEMENTS( sTable ) && !bFound; ++J ) {
					if ( _swsInput[I] == sTable[J].wcSrc ) {
						swsOut.push_back( L'\\' );
						swsOut.push_back( sTable[J].wcDst );
						bFound = true;
						if ( _bKeepNewline && _swsInput[I] == L'\n' ) {
							swsOut.push_back( L'\r' );
							swsOut.push_back( L'\n' );
						}
						break;
					}
				}
				if ( _bIncludeBackSlash && _swsInput[I] == '\\' ) {
					swsOut.push_back( L'\\' );
					swsOut.push_back( L'\\' );
					bFound = true;
				}
				if ( bFound ) { ++I; continue; }
				if ( (vTypeData[I] & C1_CNTRL) ||
					!::iswprint( _swsInput[I] ) ||
					!(vTypeData[I] & C1_DEFINED) ||
					(vType3Data[I] & (C3_HIGHSURROGATE | C3_LOWSURROGATE)) ) {
					char szBuffer[16];
					::sprintf_s( szBuffer, "\\u%.4X", static_cast<uint16_t>(_swsInput[I]) );
					char * pcTmp = szBuffer;
					while ( (*pcTmp) ) {
						swsOut.push_back( (*pcTmp++) );
					}
					
					++I;
					continue;
				}
			}
			for ( size_t J = 0; J < sLen; ++J ) {
				swsOut.push_back( static_cast<uint16_t>(_swsInput[I++]) );
			}
		}
		return swsOut;
	}

	// Adds escapes to all unprintable characters.
	std::vector<uint32_t> CUtilities::EscapeUnprintable( const std::vector<uint32_t> &_swsInput, bool _bIncludeBackSlash, bool _bKeepNewline ) {
		CSecureWString swsConv = CUtilities::Utf32StringToWString( _swsInput.data(), _swsInput.size() );
		size_t sLen;
		
		static struct {
			wchar_t wcSrc;
			wchar_t wcDst;
		} const sTable[] = {
			{ L'\a', L'a' },
			{ L'\b', L'b' },
			{ L'\f', L'f' },
			{ L'\n', L'n' },
			{ L'\r', L'r' },
			{ L'\t', L't' },
			{ L'\v', L'v' },
		};
		std::vector<uint32_t> vOutput;
		for ( size_t I = 0; I < _swsInput.size();  ) {
			uint32_t ui32This = NextUtf32Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			if ( sLen == 1 && MX_UTF_INVALID != ui32This ) {
				bool bFound = false;
				for ( size_t J = 0; J < MX_ELEMENTS( sTable ) && !bFound; ++J ) {
					if ( _swsInput[I] == sTable[J].wcSrc ) {
						vOutput.push_back( L'\\' );
						vOutput.push_back( sTable[J].wcDst );
						bFound = true;
						if ( _bKeepNewline && _swsInput[I] == L'\n' ) {
							vOutput.push_back( L'\r' );
							vOutput.push_back( L'\n' );
						}
						break;
					}
					if ( _bIncludeBackSlash && _swsInput[I] == '\\' ) {
						vOutput.push_back( L'\\' );
						vOutput.push_back( L'\\' );
						bFound = true;
					}
				}
				if ( bFound ) { ++I; continue; }

				uint32_t ui32ThisLen;
				uint32_t ui32Conv = Utf32ToUtf16( ui32This, ui32ThisLen );
				WORD wTypeData[2], wType3Data[2];
				::GetStringTypeW( CT_CTYPE1,
					reinterpret_cast<LPCWCH>(&ui32Conv),
					ui32ThisLen,
					wTypeData );
				::GetStringTypeW( CT_CTYPE3,
					reinterpret_cast<LPCWCH>(&ui32Conv),
					ui32ThisLen,
					wType3Data );

				if ( (wTypeData[0] & C1_CNTRL) ||
					!::iswprint( static_cast<wint_t>(ui32Conv) ) ||
					!(wTypeData[0] & C1_DEFINED) ) {
					char szBuffer[16];
					::sprintf_s( szBuffer, "\\U%.8X", static_cast<uint32_t>(_swsInput[I]) );
					char * pcTmp = szBuffer;
					while ( (*pcTmp) ) {
						vOutput.push_back( (*pcTmp++) );
					}
					
					++I;
					continue;
				}
			}
			for ( size_t J = 0; J < sLen; ++J ) {
				vOutput.push_back( static_cast<uint32_t>(_swsInput[I++]) );
			}
		}
		return vOutput;
	}

	// Escapes all characters non-UTF.
	CSecureString CUtilities::EscapeAllNonUtf( const CSecureString &_ssInput, bool _bKeepNewline ) {
		CSecureString ssTmp;
		for ( size_t I = 0; I < _ssInput.size(); ++I ) {
			ssTmp.push_back( '\\' );
			ssTmp.push_back( 'x' );
			char szBuffer[8];
			::sprintf_s( szBuffer, "%.2X", static_cast<uint8_t>(_ssInput[I]) );
			ssTmp.push_back( szBuffer[0] );
			ssTmp.push_back( szBuffer[1] );
			if ( _bKeepNewline && _ssInput[I] == '\n' ) {
				ssTmp.push_back( '\r' );
				ssTmp.push_back( '\n' );
			}
		}
		return ssTmp;
	}

	// Escapes all characters.
	CSecureString CUtilities::EscapeAllW( const CSecureString &_ssInput, bool _bKeepNewline ) {
		CSecureString ssTmp;
		size_t sLen = 1;
		for ( size_t I = 0; I < _ssInput.size() && sLen; I += sLen ) {

			uint32_t ui32This = NextUtf8Char( &_ssInput[I], _ssInput.size() - I, &sLen );
			if ( sLen == 1 ) {
				// A single character can be used for all formats.
				ssTmp.push_back( '\\' );
				ssTmp.push_back( 'x' );
				char szBuffer[8];
				::sprintf_s( szBuffer, "%.2X", static_cast<uint8_t>(_ssInput[I]) );
				ssTmp.push_back( szBuffer[0] );
				ssTmp.push_back( szBuffer[1] );
				if ( _bKeepNewline && _ssInput[I] == '\n' ) {
					ssTmp.push_back( '\r' );
					ssTmp.push_back( '\n' );
				}
				continue;
			}
			uint32_t ui32ThisLen = 0;
			uint32_t ui32ThisUtf16 = Utf32ToUtf16( ui32This, ui32ThisLen );
			if ( ui32ThisLen == 1 ) {
				// To preserve the UTF data, a \uNNNN must be used.
				ssTmp.push_back( '\\' );
				ssTmp.push_back( 'u' );
				char szBuffer[8];
				::sprintf_s( szBuffer, "%.4X", static_cast<uint16_t>(ui32ThisUtf16) );
				ssTmp.push_back( szBuffer[0] );
				ssTmp.push_back( szBuffer[1] );
				ssTmp.push_back( szBuffer[2] );
				ssTmp.push_back( szBuffer[3] );
				continue;
			}
			// Have to use full 32-bit \U to preserve UTF data.
			ssTmp.push_back( '\\' );
			ssTmp.push_back( 'U' );
			char szBuffer[16];
			::sprintf_s( szBuffer, "%.8X", ui32This );
			ssTmp.push_back( szBuffer[0] );
			ssTmp.push_back( szBuffer[1] );
			ssTmp.push_back( szBuffer[2] );
			ssTmp.push_back( szBuffer[3] );
			ssTmp.push_back( szBuffer[4] );
			ssTmp.push_back( szBuffer[5] );
			ssTmp.push_back( szBuffer[6] );
			ssTmp.push_back( szBuffer[7] );
		}
		return ssTmp;
	}

	// Escapes all characters.
	CSecureWString CUtilities::EscapeAllW( const CSecureWString &_swsInput, bool _bKeepNewline ) {
		CSecureWString swsTmp;
		for ( size_t I = 0; I < _swsInput.size(); ++I ) {
			swsTmp.push_back( '\\' );
			swsTmp.push_back( 'u' );
			char szBuffer[8];
			::sprintf_s( szBuffer, "%.4X", static_cast<uint16_t>(_swsInput[I]) );
			swsTmp.push_back( szBuffer[0] );
			swsTmp.push_back( szBuffer[1] );
			swsTmp.push_back( szBuffer[2] );
			swsTmp.push_back( szBuffer[3] );
			if ( _bKeepNewline && _swsInput[I] == L'\n' ) {
				swsTmp.push_back( L'\r' );
				swsTmp.push_back( L'\n' );
			}
		}
		return swsTmp;
	}

	// Escapes all characters.
	std::vector<uint32_t> CUtilities::EscapeAllW( const std::vector<uint32_t> &_swsInput, bool _bKeepNewline ) {
		std::vector<uint32_t> swsTmp;
		for ( size_t I = 0; I < _swsInput.size(); ++I ) {
			swsTmp.push_back( '\\' );
			swsTmp.push_back( 'U' );
			char szBuffer[16];
			::sprintf_s( szBuffer, "%.8X", static_cast<uint32_t>(_swsInput[I]) );
			swsTmp.push_back( szBuffer[0] );
			swsTmp.push_back( szBuffer[1] );
			swsTmp.push_back( szBuffer[2] );
			swsTmp.push_back( szBuffer[3] );
			swsTmp.push_back( szBuffer[4] );
			swsTmp.push_back( szBuffer[5] );
			swsTmp.push_back( szBuffer[6] );
			swsTmp.push_back( szBuffer[7] );
			if ( _bKeepNewline && _swsInput[I] == L'\n' ) {
				swsTmp.push_back( L'\r' );
				swsTmp.push_back( L'\n' );
			}
		}
		return swsTmp;
	}

	// Escapes standard characters.
	CSecureString CUtilities::EscapeStandard( const CSecureString &_ssInput, bool _bKeepNewline ) {
		size_t sLen;
		
		static struct {
			wchar_t wcSrc;
			wchar_t wcDst;
		} const sTable[] = {
			{ L'\'', L'\'' },
			{ L'"', L'"' },
			{ L'\\', L'\\' },
			{ L'\a', L'a' },
			{ L'\b', L'b' },
			{ L'\f', L'f' },
			{ L'\n', L'n' },
			{ L'\r', L'r' },
			{ L'\t', L't' },
			{ L'\v', L'v' },
		};
		CSecureString vOutput;
		for ( size_t I = 0; I < _ssInput.size();  ) {
			uint32_t ui32This = NextUtf8Char( &_ssInput[I], _ssInput.size() - I, &sLen );
			if ( sLen == 1 && MX_UTF_INVALID != ui32This ) {
				bool bFound = false;
				for ( size_t J = 0; J < MX_ELEMENTS( sTable ) && !bFound; ++J ) {
					if ( _ssInput[I] == sTable[J].wcSrc ) {
						vOutput.push_back( L'\\' );
						vOutput.push_back( sTable[J].wcDst );
						bFound = true;
						if ( _bKeepNewline && _ssInput[I] == L'\n' ) {
							vOutput.push_back( L'\r' );
							vOutput.push_back( L'\n' );
						}
						continue;
					}
				}
				if ( bFound ) { ++I; continue; }
				if ( ui32This == 0 ) {
					vOutput.push_back( L'\\' );
					vOutput.push_back( L'0' );
					 ++I;
					continue;
				}
			}
			uint32_t ui32ThisLen;
			uint32_t ui32Conv = Utf32ToUtf16( ui32This, ui32ThisLen );
			WORD wTypeData[2]/*, wType3Data[2]*/;
			::GetStringTypeW( CT_CTYPE1,
				reinterpret_cast<LPCWCH>(&ui32Conv),
				ui32ThisLen,
				wTypeData );
			/*::GetStringTypeW( CT_CTYPE3,
				reinterpret_cast<LPCWCH>(&ui32Conv),
				ui32ThisLen,
				wType3Data );*/

			if ( (wTypeData[0] & C1_CNTRL) ||
				/*!::iswprint( static_cast<wint_t>(ui32Conv) ) ||*/
				!(wTypeData[0] & C1_DEFINED) ) {
				char szBuffer[16];
				if ( sLen == 1 ) {
					::sprintf_s( szBuffer, "\\x%.2X", static_cast<uint8_t>(_ssInput[I]) );
				}
				else if ( ui32ThisLen == 1 ) {
					::sprintf_s( szBuffer, "\\u%.4X", static_cast<uint16_t>(ui32Conv) );
				}
				else {
					::sprintf_s( szBuffer, "\\U%.8X", ui32This );
				}
				char * pcTmp = szBuffer;
				while ( (*pcTmp) ) {
					vOutput.push_back( (*pcTmp++) );
				}
					
				++I;
				continue;
			}
			for ( size_t J = 0; J < sLen; ++J ) {
				vOutput.push_back( static_cast<uint8_t>(_ssInput[I++]) );
			}
		}
		return vOutput;
	}

	// Escapes standard characters.
	CSecureWString CUtilities::EscapeStandard( const CSecureWString &_swsInput, bool _bKeepNewline ) {
		CSecureWString swsOut;
		size_t sLen;
		/*std::vector<WORD> vTypeData, vType3Data;
		if ( !Resize( vTypeData, _swsInput.size() ) ) { return CSecureWString( _swsInput ); }
		::GetStringTypeW( CT_CTYPE1,
			_swsInput.c_str(),
			_swsInput.size(),
			vTypeData.data() );
		if ( !Resize( vType3Data, _swsInput.size() ) ) { return CSecureWString( _swsInput ); }
		::GetStringTypeW( CT_CTYPE3,
			_swsInput.c_str(),
			_swsInput.size(),
			vType3Data.data() );*/
		static struct {
			wchar_t wcSrc;
			wchar_t wcDst;
		} const sTable[] = {
			{ L'\'', L'\'' },
			{ L'"', L'"' },
			{ L'\\', L'\\' },
			{ L'\a', L'a' },
			{ L'\b', L'b' },
			{ L'\f', L'f' },
			{ L'\n', L'n' },
			{ L'\r', L'r' },
			{ L'\t', L't' },
			{ L'\v', L'v' },
		};
		for ( size_t I = 0; I < _swsInput.size();  ) {
			uint32_t ui32This = NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			if ( sLen == 1 ) {
				bool bFound = false;
				for ( size_t J = 0; J < MX_ELEMENTS( sTable ) && !bFound; ++J ) {
					if ( _swsInput[I] == sTable[J].wcSrc ) {
						swsOut.push_back( L'\\' );
						swsOut.push_back( sTable[J].wcDst );
						bFound = true;
						if ( _bKeepNewline && _swsInput[I] == L'\n' ) {
							swsOut.push_back( L'\r' );
							swsOut.push_back( L'\n' );
						}
						continue;
					}
				}
				if ( bFound ) { ++I; continue; }
				if ( ui32This == 0 ) {
					swsOut.push_back( L'\\' );
					swsOut.push_back( L'0' );
					 ++I;
					continue;
				}
			}
			uint32_t ui32ThisLen;
			uint32_t ui32Conv = Utf32ToUtf16( ui32This, ui32ThisLen );
			WORD wTypeData[2]/*, wType3Data[2]*/;
			::GetStringTypeW( CT_CTYPE1,
				reinterpret_cast<LPCWCH>(&ui32Conv),
				ui32ThisLen,
				wTypeData );
			/*::GetStringTypeW( CT_CTYPE3,
				reinterpret_cast<LPCWCH>(&ui32Conv),
				ui32ThisLen,
				wType3Data );*/

			if ( (wTypeData[0] & C1_CNTRL) ||
				/*!::iswprint( _swsInput[I] ) ||*/
				!(wTypeData[0] & C1_DEFINED) /*||
				(wType3Data[0] & (C3_HIGHSURROGATE | C3_LOWSURROGATE))*/ ) {
				char szBuffer[16];
				if ( ui32This <= 0x7F /*&& (wType3Data[0] & (C3_HIGHSURROGATE | C3_LOWSURROGATE)) == 0*/ ) {
					::sprintf_s( szBuffer, "\\x%.2X", static_cast<uint8_t>(_swsInput[I]) );
				}
				else if ( ui32ThisLen == 1 ) {
					::sprintf_s( szBuffer, "\\u%.4X", static_cast<uint16_t>(_swsInput[I]) );
				}
				else {
					::sprintf_s( szBuffer, "\\U%.8X", ui32This );
				}
				char * pcTmp = szBuffer;
				while ( (*pcTmp) ) {
					swsOut.push_back( (*pcTmp++) );
				}
					
				++I;
				continue;
			}
			for ( size_t J = 0; J < sLen; ++J ) {
				swsOut.push_back( static_cast<uint16_t>(_swsInput[I++]) );
			}
		}
		return swsOut;
	}

	// Escapes standard characters.
	std::vector<uint32_t> CUtilities::EscapeStandard( const std::vector<uint32_t> &_swsInput, bool _bKeepNewline ) {
		//CSecureWString swsConv = CUtilities::Utf32StringToWString( _swsInput.data(), _swsInput.size() );
		size_t sLen;
		
		static struct {
			wchar_t wcSrc;
			wchar_t wcDst;
		} const sTable[] = {
			{ L'\'', L'\'' },
			{ L'"', L'"' },
			{ L'\\', L'\\' },
			{ L'\a', L'a' },
			{ L'\b', L'b' },
			{ L'\f', L'f' },
			{ L'\n', L'n' },
			{ L'\r', L'r' },
			{ L'\t', L't' },
			{ L'\v', L'v' },
		};
		std::vector<uint32_t> vOutput;
		for ( size_t I = 0; I < _swsInput.size();  ) {
			uint32_t ui32This = NextUtf32Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			if ( sLen == 1 && MX_UTF_INVALID != ui32This ) {
				bool bFound = false;
				for ( size_t J = 0; J < MX_ELEMENTS( sTable ) && !bFound; ++J ) {
					if ( _swsInput[I] == sTable[J].wcSrc ) {
						vOutput.push_back( L'\\' );
						vOutput.push_back( sTable[J].wcDst );
						bFound = true;
						if ( _bKeepNewline && _swsInput[I] == L'\n' ) {
							vOutput.push_back( L'\r' );
							vOutput.push_back( L'\n' );
						}
						continue;
					}
				}
				if ( bFound ) { ++I; continue; }

				if ( ui32This == 0 ) {
					vOutput.push_back( L'\\' );
					vOutput.push_back( L'0' );
					 ++I;
					continue;
				}
			}
				
			uint32_t ui32ThisLen;
			uint32_t ui32Conv = Utf32ToUtf16( ui32This, ui32ThisLen );
			WORD wTypeData[2]/*, wType3Data[2]*/;
			::GetStringTypeW( CT_CTYPE1,
				reinterpret_cast<LPCWCH>(&ui32Conv),
				ui32ThisLen,
				wTypeData );
			/*::GetStringTypeW( CT_CTYPE3,
				reinterpret_cast<LPCWCH>(&ui32Conv),
				ui32ThisLen,
				wType3Data );*/

			if ( (wTypeData[0] & C1_CNTRL) ||
				/*!::iswprint( static_cast<wint_t>(ui32Conv) ) ||*/
				!(wTypeData[0] & C1_DEFINED) ) {
				char szBuffer[16];
				::sprintf_s( szBuffer, "\\U%.8X", static_cast<uint32_t>(_swsInput[I]) );
				char * pcTmp = szBuffer;
				while ( (*pcTmp) ) {
					vOutput.push_back( (*pcTmp++) );
				}
					
				++I;
				continue;
			}
			for ( size_t J = 0; J < sLen; ++J ) {
				vOutput.push_back( static_cast<uint32_t>(_swsInput[I++]) );
			}
		}
		return vOutput;
	}

	// Creates a string from a string in the form of an array of hex bytes.
	CSecureString CUtilities::FromHexString( const CSecureWString &_swsInput ) {
		CSecureString ssTmp;
		CSecureString ssBuffer;
		for ( size_t I = 0; I < _swsInput.size(); ++I ) {
			if ( ee::ValidHex( _swsInput[I] ) ) {
				ssBuffer.push_back( _swsInput[I] );
			}
			else {
				if ( ssBuffer.size() ) {
					size_t sEaten = 0;
					for ( size_t J = 0; J < ssBuffer.size(); J += sEaten ) {
						uint64_t uiVal = ee::StoULL( &ssBuffer[J], 16, &sEaten, 0xFF );
						ssTmp.push_back( static_cast<uint8_t>(uiVal) );
					}
					ssBuffer.clear();
				}
			}
		}
		if ( ssBuffer.size() ) {
			size_t sEaten = 0;
			for ( size_t J = 0; J < ssBuffer.size(); J += sEaten ) {
				uint64_t uiVal = ee::StoULL( &ssBuffer[J], 16, &sEaten, 0xFF );
				ssTmp.push_back( static_cast<uint8_t>(uiVal) );
			}
		}
		return ssTmp;
	}

	// Strips whitespace from the beginning and end of the string.
	std::string & CUtilities::Strip( std::string &_sIn, bool _bIsUtf8 ) {
		if ( _bIsUtf8 ) {
			bool bFoundNonWhitespace = false;
			const char * pcSrc = _sIn.c_str();
			size_t sTrailIdx = 0;

			for ( size_t I = 0; I < _sIn.size(); ) {
				size_t sThisSize = 0;
				uint32_t ui32This = NextUtf8Char( &pcSrc[I], _sIn.size() - I, &sThisSize );
				uint32_t uiTempLen;
				if ( ui32This != MX_UTF_INVALID && std::iswspace( static_cast<wint_t>(Utf32ToUtf16( ui32This, uiTempLen )) ) ) {
					if ( !bFoundNonWhitespace ) {
						for ( size_t J = 0; J < sThisSize; ++J ) { _sIn.erase( _sIn.begin() ); }
						continue;
					}
				}
				else {
					// Character is either invalid or is not whitespace.
					sTrailIdx = I + sThisSize;
				}
				I += sThisSize;
				bFoundNonWhitespace = true;
			}
			while ( _sIn.size() > sTrailIdx ) { _sIn.pop_back(); }
			return _sIn;
		}
		while ( _sIn.size() && ::isspace( _sIn.c_str()[_sIn.size()-1] ) ) { _sIn.pop_back(); }
		while ( _sIn.size() && ::isspace( _sIn.c_str()[0] ) ) { _sIn.erase( _sIn.begin() ); }

		return _sIn;
	}

	// Upper-cases a string.
	CSecureWString CUtilities::ToUpper( const CSecureWString &_swsInput ) {
		CSecureWString swsRet;
		size_t sLen = 0;
		for ( size_t I = 0; I < _swsInput.size(); I += sLen ) {
			uint32_t uiConverted = NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = std::towupper( Utf32ToUtf16( uiConverted, uiTempLen ) );
			}
			for ( size_t J = 0; J < uiTempLen; ++J ) {
				swsRet.push_back( static_cast<uint16_t>(uiConverted) );
				uiConverted >>= 16;
			}
		}
		return swsRet;
	}

	// Lower-cases a string.
	CSecureWString CUtilities::ToLower( const CSecureWString &_swsInput ) {
		CSecureWString swsRet;
		size_t sLen = 0;
		for ( size_t I = 0; I < _swsInput.size(); I += sLen ) {
			uint32_t uiConverted = NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = std::towlower( Utf32ToUtf16( uiConverted, uiTempLen ) );
			}
			for ( size_t J = 0; J < uiTempLen; ++J ) {
				swsRet.push_back( static_cast<uint16_t>(uiConverted) );
				uiConverted >>= 16;
			}
		}
		return swsRet;
	}

	// Converts Hiragana characters to Katakana.
	CSecureWString CUtilities::ToKatakana( const CSecureWString &_swsInput ) {
		CSecureWString swsRet;
		size_t sLen = 0;
		for ( size_t I = 0; I < _swsInput.size(); I += sLen ) {
			uint32_t uiConverted = NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = HiraganaToKatakana( Utf32ToUtf16( uiConverted, uiTempLen ) );
			}
			for ( size_t J = 0; J < uiTempLen; ++J ) {
				swsRet.push_back( static_cast<uint16_t>(uiConverted) );
				uiConverted >>= 16;
			}
		}
		return swsRet;
	}

	// Converts Katakana characters to Hiragana.
	CSecureWString CUtilities::ToHiragana( const CSecureWString &_swsInput ) {
		CSecureWString swsRet;
		size_t sLen = 0;
		for ( size_t I = 0; I < _swsInput.size(); I += sLen ) {
			uint32_t uiConverted = NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = KatakanaToHiragana( Utf32ToUtf16( uiConverted, uiTempLen ) );
			}
			for ( size_t J = 0; J < uiTempLen; ++J ) {
				swsRet.push_back( static_cast<uint16_t>(uiConverted) );
				uiConverted >>= 16;
			}
		}
		return swsRet;
	}

	// Converts from full-width characters to half-width.
	CSecureWString CUtilities::ToHalfwidth( const CSecureWString &_swsInput ) {
		CSecureWString swsRet;
		size_t sLen = 0;
		for ( size_t I = 0; I < _swsInput.size(); I += sLen ) {
			uint32_t uiConverted = NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = FullWidthToByte( Utf32ToUtf16( uiConverted, uiTempLen ) );
			}
			for ( size_t J = 0; J < uiTempLen; ++J ) {
				swsRet.push_back( static_cast<uint16_t>(uiConverted) );
				uiConverted >>= 16;
			}
		}
		return swsRet;
	}

	// Converts several common numerics to the numerics in ASCII range.
	CSecureWString CUtilities::ToAsciiNumerics( const CSecureWString &_swsInput ) {
		CSecureWString swsRet;
		size_t sLen = 0;
		for ( size_t I = 0; I < _swsInput.size(); I += sLen ) {
			uint32_t uiConverted = NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = NumericToByte( Utf32ToUtf16( uiConverted, uiTempLen ) );
			}
			for ( size_t J = 0; J < uiTempLen; ++J ) {
				swsRet.push_back( static_cast<uint16_t>(uiConverted) );
				uiConverted >>= 16;
			}
		}
		return swsRet;
	}

	// Windows resource type to string.
	BOOL CUtilities::ResourceTypeToString( uint64_t _uiId, CHAR * _pcRet ) {
		
		struct {
			uint32_t ui32Type;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ MX_RT_CURSOR, _T_3587AD08_CURSOR, _LEN_3587AD08 },
			{ MX_RT_BITMAP, _T_1895992A_BITMAP, _LEN_1895992A },
			{ MX_RT_ICON, _T_5301C46F_ICON, _LEN_5301C46F },
			{ MX_RT_MENU, _T_4B90D727_MENU, _LEN_4B90D727 },
			{ MX_RT_DIALOG, _T_B5991FE4_DIALOG, _LEN_B5991FE4 },
			{ MX_RT_STRING, _T_6E46752F_STRING, _LEN_6E46752F },
			{ MX_RT_FONTDIR, _T_3458B8D1_FONTDIR, _LEN_3458B8D1 },
			{ MX_RT_FONT, _T_E601E566_FONT, _LEN_E601E566 },
			{ MX_RT_ACCELERATOR, _T_ABDAFA74_ACCELERATOR, _LEN_ABDAFA74 },
			{ MX_RT_RCDATA, _T_55F5EB0D_RCDATA, _LEN_55F5EB0D },
			{ MX_RT_MESSAGETABLE, _T_97AB2D94_MESSAGETABLE, _LEN_97AB2D94 },
			{ MX_RT_GROUP_CURSOR, _T_4C76C2D2_GROUP_CURSOR, _LEN_4C76C2D2 },
			{ MX_RT_GROUP_ICON, _T_86904090_GROUP_ICON, _LEN_86904090 },
			{ MX_RT_VERSION, _T_80592DD9_VERSION, _LEN_80592DD9 },
			{ MX_RT_DLGINCLUDE, _T_BDA1B9A4_DLGINCLUDE, _LEN_BDA1B9A4 },
			{ MX_RT_PLUGPLAY, _T_4C67C2AD_PLUGPLAY, _LEN_4C67C2AD },
			{ MX_RT_VXD, _T_94A3D9C5_VXD, _LEN_94A3D9C5 },
			{ MX_RT_ANICURSOR, _T_4621EF34_ANICURSOR, _LEN_4621EF34 },
			{ MX_RT_ANIICON, _T_6DE4EBE4_ANIICON, _LEN_6DE4EBE4 },
			{ MX_RT_HTML, _T_2EEC1551_HTML, _LEN_2EEC1551 },
			{ MX_RT_MANIFEST, _T_60C9F40C_MANIFEST, _LEN_60C9F40C },
		};

		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ); ++I ) {
			if ( aTable[I].ui32Type == _uiId ) {
				if ( !Options.bShortenEnumNames ) {
					CStringDecoder::Decode( _T_B57AF7F9_RT_, _LEN_B57AF7F9, _pcRet );
					_pcRet += _LEN_B57AF7F9;
				}
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, _pcRet );
				return TRUE;
			}
		}
		return FALSE;
	}

	// Windows resource type to string.
	BOOL CUtilities::ResourceTypeToString( uint64_t _uiId, std::string &_sString ) {
		CHAR szBuffer[64];
		if ( ResourceTypeToString( _uiId, szBuffer ) ) {
			_sString += szBuffer;
			return TRUE;
		}
		return FALSE;
	}

	// Adds a \ to the end of a string if it does not already have one.
	std::string & CUtilities::FinishPath( std::string &_sString ) {
		if ( !_sString.size() || _sString[_sString.size()-1] != '\\' ) {
			_sString.push_back( '\\' );
		}
		return _sString;
	}

	// Adds a \ to the end of a string if it does not already have one.
	std::wstring & CUtilities::FinishPath( std::wstring &_sString ) {
		if ( !_sString.size() || _sString[_sString.size()-1] != L'\\' ) {
			_sString.push_back( L'\\' );
		}
		return _sString;
	}

	// Generates a string of random characters.
	CHAR * CUtilities::RandomString( CHAR * _pcBuffer, SIZE_T _sSize ) {
		if ( !_pcBuffer || !_sSize ) { return _pcBuffer; }
		std::random_device rdRand;
		std::mt19937 mGen( rdRand() );
		std::uniform_int_distribution<> uidDist( 'a', 'z' );
		for ( SIZE_T I = 0; I < _sSize; ++I ) {
			_pcBuffer[I] = uidDist( mGen );
		}
		_pcBuffer[_sSize-1] = '\0';
		return _pcBuffer;
	}

	// Generates a string of random characters.
	WCHAR * CUtilities::RandomString( WCHAR * _pwcBuffer, SIZE_T _sSize ) {
		if ( !_pwcBuffer || !_sSize ) { return _pwcBuffer; }
		std::random_device rdRand;
		std::mt19937 mGen( rdRand() );
		std::uniform_int_distribution<> uidDistLower( L'a', L'z' );
		std::uniform_int_distribution<> uidDistUpper( L'A', L'Z' );
		std::uniform_int_distribution<> uidBool( 0, 3 );
		for ( SIZE_T I = 0; I < _sSize; ++I ) {
			if ( uidBool( mGen ) == 0 ) {
				_pwcBuffer[I] = uidDistUpper( mGen );
			}
			else {
				_pwcBuffer[I] = uidDistLower( mGen );
			}
		}
		_pwcBuffer[_sSize-1] = L'\0';
		return _pwcBuffer;
	}

	// Gets the number of elements in DataTypeInfo.
	size_t CUtilities::DataTypeInfoLen() {
		return MX_ELEMENTS( DataTypeInfo );
	}

	// Prints an ee::CExpEvalContainer::EE_RESULT value and appends it to _sString, then returns _sString.
	std::wstring & CUtilities::PrintExpResult( const ee::CExpEvalContainer::EE_RESULT &_rResult, std::wstring &_sString, int32_t _iDblSciPrec, MX_DECODING _dDecoder ) {
		CSecureWString wsRes = PrintExpResult( _rResult, _iDblSciPrec, _dDecoder );
		_sString.append( wsRes );
		return _sString;
	}

	// Prints an ee::CExpEvalContainer::EE_RESULT value.
	std::wstring CUtilities::PrintExpResult( const ee::CExpEvalContainer::EE_RESULT &_rResult, uint64_t _ui64Data, MX_DECODING _dDecoder ) {
		switch ( _dDecoder ) {
			case CUtilities::MX_D_EXTENDED : {
				return PrintExpResultExtended( _rResult, _ui64Data );
			}
			case CUtilities::MX_D_TIME : {
				return PrintExpResultDataTime( _rResult, _ui64Data );
			}
			default : {
				return PrintExpResultStandard( _rResult, _ui64Data );
			}
		}
	}

	// Gets the current time in microseconds.
	uint64_t CUtilities::CurTimeInMicros() {
		static LARGE_INTEGER liFreq = { 0 };
		if ( !liFreq.QuadPart ) {
			::QueryPerformanceFrequency( reinterpret_cast<LARGE_INTEGER *>(&liFreq) );
		}
		LARGE_INTEGER liInt;
		::QueryPerformanceCounter( &liInt );
		return (liInt.QuadPart * 1000000ULL) / liFreq.QuadPart;
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
	void CUtilities::ResolveAllEscapes( const std::string &_sInput, std::vector<uint32_t> &_vOutput ) {
		size_t sLen = 1;
		for ( size_t I = 0; I < _sInput.size() && sLen; ) {
			uint64_t uiVal = ResolveEscape( &_sInput.c_str()[I], _sInput.size() - I, sLen, false );
			do {
				_vOutput.push_back( uiVal );
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
	void CUtilities::ResolveAllEscapes( const std::string &_sInput, std::string &_sOutput, bool _bIsUtf8 ) {
		size_t sLen = 1;
		for ( size_t I = 0; I < _sInput.size() && sLen; I += sLen ) {
			uint64_t uiNext = _bIsUtf8 ? NextUtf8Char( &_sInput[I], _sInput.size() - I, &sLen ) : (sLen = 1, static_cast<uint8_t>(_sInput[I]));
			if ( sLen == 1 ) {
				// It was just a regular character.  Could be an escape.
				uiNext = ResolveEscape( &_sInput[I], _sInput.size() - I, sLen, false );
			}
			do {
				uint32_t ui32Len;
				uint32_t ui32BackToUtf8 = _bIsUtf8 ? Utf32ToUtf8( static_cast<uint32_t>(uiNext), ui32Len ) : (ui32Len = CountSetBytes( uiNext ), uiNext);
				if ( ui32BackToUtf8 == MX_UTF_INVALID ) {
					ui32Len = 1;
					ui32BackToUtf8 = uiNext;
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
	uint64_t CUtilities::ResolveEscape( const char * _pcInput, size_t _sLen, size_t &_sCharLen, bool _bIncludeHtml ) {
		if ( !_sLen ) { _sCharLen = 0; return 0; }
		if ( _sLen == 1 ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
		// There are at least 2 characters so an escape is possible.
		if ( _sLen >= 2 && (*_pcInput) == '&' && _bIncludeHtml ) {
			size_t sLen;
			uint32_t uiTmp = ee::EscapeHtml( _pcInput + 1, _sLen - 1, sLen );
			if ( sLen ) {
				_sCharLen = sLen + 1;
				return uiTmp;
			}
			// ee::EscapeHtml() only handles &#????; and &#x????; encodings.
			size_t sCount = ee::CountAlphanumeric( _pcInput + 1, _sLen - 1, 0 );
			size_t sCodeLen;
			uint64_t uiCode = CHtml::GetCode( _pcInput + 1, sCount );
			if ( uiCode != MX_UTF_INVALID ) {
				_sCharLen = sCount + 1;
				_pcInput += _sCharLen;
				_sLen -= _sCharLen;
				if ( _sLen && (*_pcInput) == ';' ) {
					// The ';' is nestled against the name as it should be.  Eat it.
					++_sCharLen;
				}
				return uiCode;
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
				uint32_t ui32Temp = ee::EscapeX( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; }
				return ui32Temp;
			}
			case 'u' : {
				// Takes \uNNNN and \uNNNN\uNNNN.
				uint32_t ui32Temp = ee::EscapeUnicodeWide4WithSurrogatePairs( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; }
				return ui32Temp;
			}
			case 'U' : {
				uint32_t ui32Temp = ee::EscapeUnicode8( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; }
				return ui32Temp;
			}
			case 'N' : {
				uint32_t ui32Temp = ee::EspaceNamedUnicode( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; }
				return ui32Temp;
			}
			default : {
				if ( _pcInput[1] >= '0' && _pcInput[1] <= '7' ) {
					uint32_t ui32Temp = ee::EscapeOctal( &_pcInput[1], _sLen, _sCharLen );
					++_sCharLen;	// Eat the \.
					return ui32Temp;
				}
				else {
					for ( size_t J = 0; sEscapes[J].cEscape != '\0'; J++ ) {
						if ( _pcInput[1] == sEscapes[J].cEscape ) {
							++_sCharLen;
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
	void CUtilities::ResolveAllHtmlXmlEscapes( const std::string &_sInput, std::string &_sOutput, bool _bIsUtf8 ) {
		size_t sLen = 1;
		for ( size_t I = 0; I < _sInput.size() && sLen; I += sLen ) {
			uint64_t uiNext = _bIsUtf8 ? NextUtf8Char( &_sInput[I], _sInput.size() - I, &sLen ) : (sLen = 1, static_cast<uint8_t>(_sInput[I]));
			if ( sLen == 1 && uiNext == '&' ) {
				// It was just a regular character.  Could be an escape.
				uiNext = ResolveEscape( &_sInput[I], _sInput.size() - I, sLen, true );
			}
			do {
				uint32_t ui32Len;
				uint32_t ui32BackToUtf8 = _bIsUtf8 ? Utf32ToUtf8( static_cast<uint32_t>(uiNext), ui32Len ) : (ui32Len = CountSetBytes( static_cast<uint32_t>(uiNext) ), uiNext);
				if ( ui32BackToUtf8 == MX_UTF_INVALID ) {
					ui32Len = 1;
					ui32BackToUtf8 = uiNext;
				}
				for ( uint32_t J = 0; J < ui32Len; ++J ) {
					_sOutput.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
					ui32BackToUtf8 >>= 8;
				}
				uiNext >>= 32ULL;
			} while ( uiNext );
		}
	}

	// Converts a string in the form of sequential bytes in the given base to an array stored as a string.
	CSecureString CUtilities::NumberStringToString( const std::string &_sInput, int _iBase, uint64_t _ui64MaxSingleValue, MX_DATA_TYPES _dtTargetType,
		std::vector<uint8_t> * _pvMeta ) {
		CSecureString ssRet;
		const char * pcInput = _sInput.c_str();
		size_t sLen = _sInput.size();
		bool bIsFloat = DataTypeIsFloat( _dtTargetType );
		size_t sSize = DataTypeSize( _dtTargetType );
		for ( size_t I = 0; I < sLen; ) {
			while ( I < sLen && ::isspace( _sInput[I] ) ) { ++I; }
			if ( _pvMeta ) {
				if ( _sInput[I] == '*' || _sInput[I] == '?' ) {
					for ( size_t J = 0; J < sSize; ++J ) { ssRet.push_back( 0 ); }
					_pvMeta->push_back( _sInput[I] );
					I += 1;
					continue;
				}
			}
			size_t sEaten = 0;
			MX_DATA_TYPE dtVal;
			if ( bIsFloat ) {				
				switch ( _dtTargetType ) {
					case MX_DT_FLOAT16 : {
						dtVal.u.UInt16 = CFloat16( ee::AtoF( &_sInput[I], &sEaten ) ).RawValue();
						break;
					}
					case MX_DT_FLOAT : {
						dtVal.u.Float32 = ee::AtoF( &_sInput[I], &sEaten );
						break;
					}
					case MX_DT_DOUBLE : {
						dtVal.u.Float64 = ee::AtoF( &_sInput[I], &sEaten );
						break;
					}
				}
				if ( !sEaten ) { break; }
			}
			else {
				dtVal.u.UInt64 = ee::StoULL( &_sInput[I], _iBase, &sEaten, _ui64MaxSingleValue );
				if ( !sEaten ) { break; }
			}
			if ( _pvMeta ) { _pvMeta->push_back( 0 ); }
			for ( size_t J = 0; J < sSize; ++J ) {
				ssRet.push_back( static_cast<uint8_t>(dtVal.u.UInt64) );
				dtVal.u.UInt64 >>= 8;
			}
			I += sEaten;
		}
		return ssRet;
	}

	// Converts a string in the form of sequential bytes in variable formats to an array stored as a string.
	CSecureString CUtilities::NumberStringToString_MultiFormat( const std::string &_sInput, int _iBase, MX_DATA_TYPES _dtTargetType,
		std::vector<uint8_t> * _pvMeta ) {
		CSecureString ssRet;
		const char * pcInput = _sInput.c_str();
		size_t sLen = _sInput.size();
		MX_DATA_TYPES dtDefaultType = _dtTargetType;
#define MX_SKIP_WS		while ( I < sLen && ::isspace( _sInput[I] ) ) { ++I; }
		for ( size_t I = 0; I < sLen; ) {
			MX_SKIP_WS;

			if ( I < sLen ) {
				bool bUnsigned = false;
				if ( _sInput[I] == 'u' ) {
					bUnsigned = true;
					++I;
					if ( I >= sLen ) { break; }
				}
				switch ( ::tolower( _sInput[I] ) ) {
					case 'i' : {
						++I;
						if ( I >= sLen ) { break; }
						dtDefaultType = bUnsigned ? MX_DT_UINT32 : MX_DT_INT32;
						if ( _sInput[I] == '8' ) {
							// u/int8.
							dtDefaultType = bUnsigned ? MX_DT_UINT8 : MX_DT_INT8;
							++I;
							MX_SKIP_WS;
							break;
						}
						if ( _sInput[I] == '1' ) {
							// u/int1.
							if ( I >= sLen ) { break; }
							if ( _sInput[I+1] == '6' ) {
								// u/int16.
								dtDefaultType = bUnsigned ? MX_DT_UINT16 : MX_DT_INT16;
								I += 2;
								MX_SKIP_WS;
								break;
							}
						}
						if ( _sInput[I] == '3' ) {
							// u/int3.
							if ( I >= sLen ) { break; }
							if ( _sInput[I+1] == '2' ) {
								// u/int32.
								dtDefaultType = bUnsigned ? MX_DT_UINT32 : MX_DT_INT32;
								I += 2;
								MX_SKIP_WS;
								break;
							}
						}
						if ( _sInput[I] == '6' ) {
							// u/int6.
							if ( I >= sLen ) { break; }
							if ( _sInput[I+1] == '4' ) {
								// u/int64.
								dtDefaultType = bUnsigned ? MX_DT_UINT64 : MX_DT_INT64;
								I += 2;
								MX_SKIP_WS;
								break;
							}
						}
						break;
					}
					case 'f' : {
						++I;
						dtDefaultType = MX_DT_FLOAT;
						if ( _sInput[I] == '1' ) {
							// f1.
							if ( I >= sLen ) { break; }
							if ( _sInput[I+1] == '6' ) {
								// float16.
								dtDefaultType = MX_DT_FLOAT16;
								I += 2;
							}
						}
						if ( _sInput[I] == '3' ) {
							// f3.
							if ( I >= sLen ) { break; }
							if ( _sInput[I+1] == '2' ) {
								// f32.
								dtDefaultType = MX_DT_FLOAT;
								I += 2;
							}
						}
						if ( _sInput[I] == '6' ) {
							// f6.
							if ( I >= sLen ) { break; }
							if ( _sInput[I+1] == '4' ) {
								// f64.
								dtDefaultType = MX_DT_DOUBLE;
								I += 2;
							}
						}
						MX_SKIP_WS;
						break;
					}
					case 'd' : {
						++I;
						dtDefaultType = MX_DT_DOUBLE;
						MX_SKIP_WS;
						break;
					}
				}
			}

			size_t sSize = DataTypeSize( dtDefaultType );
			bool bIsFloat = DataTypeIsFloat( dtDefaultType );
			uint64_t uiMax = static_cast<uint64_t>(-1);
			if ( !bIsFloat ) {
				uiMax = 0;
				for ( size_t I = sSize; I--; ) {
					uiMax <<= 8;
					uiMax |= 0xFF;
				}
			}
			if ( _pvMeta ) {
				if ( _sInput[I] == '*' || _sInput[I] == '?' ) {
					//for ( size_t J = 0; J < sSize; ++J ) { ssRet.push_back( 0 ); }
					_pvMeta->push_back( (_sInput[I] == '*' ? MX_WF_ANY : MX_WF_ANY_CHAR) | dtDefaultType );
					I += 1;
					continue;
				}
			}
			size_t sEaten = 0;
			MX_DATA_TYPE dtVal;
			if ( bIsFloat ) {				
				switch ( dtDefaultType ) {
					case MX_DT_FLOAT16 : {
						dtVal.u.UInt16 = CFloat16( ee::AtoF( &_sInput[I], &sEaten ) ).RawValue();
						break;
					}
					case MX_DT_FLOAT : {
						dtVal.u.Float32 = ee::AtoF( &_sInput[I], &sEaten );
						break;
					}
					case MX_DT_DOUBLE : {
						dtVal.u.Float64 = ee::AtoF( &_sInput[I], &sEaten );
						break;
					}
				}
				if ( !sEaten ) { break; }
			}
			else {
				dtVal.u.UInt64 = ee::StoULL( &_sInput[I], _iBase, &sEaten, uiMax );
				if ( !sEaten ) { break; }
			}
			if ( _pvMeta ) { _pvMeta->push_back( dtDefaultType ); }
			for ( size_t J = 0; J < sSize; ++J ) {
				ssRet.push_back( static_cast<uint8_t>(dtVal.u.UInt64) );
				dtVal.u.UInt64 >>= 8;
			}
			I += sEaten;
		}
		return ssRet;
#undef MX_SKIP_WS
	}

	// Creates a number string given an array of data.
	CSecureString CUtilities::PrimitiveArrayToStringA( const void * _pvData, size_t _sLenInBytes, MX_DATA_TYPES _dtTargetType, uint32_t _uiNumDigits, int32_t _iSigDigits ) {
		CSecureString ssResult;
		size_t sCharSize = DataTypeSize( _dtTargetType );
		const uint8_t * pui8Data = static_cast<const uint8_t *>(_pvData);
		for ( size_t I = 0; I < _sLenInBytes; I += sCharSize ) {
			if ( ssResult.size() ) { 
				ssResult.push_back( ' ' );
			}
			DataTypeToString( pui8Data + I, _dtTargetType, ssResult, _uiNumDigits, _iSigDigits );
		}
		return ssResult;
	}

	// Creates a number string given an array of data.
	CSecureWString CUtilities::PrimitiveArrayToStringW( const void * _pvData, size_t _sLenInBytes, MX_DATA_TYPES _dtTargetType, uint32_t _uiNumDigits, int32_t _iSigDigits ) {
		CSecureWString ssResult;
		size_t sCharSize = DataTypeSize( _dtTargetType );
		const uint8_t * pui8Data = static_cast<const uint8_t *>(_pvData);
		for ( size_t I = 0; I < _sLenInBytes; I += sCharSize ) {
			if ( ssResult.size() ) { 
				ssResult.push_back( ' ' );
			}
			DataTypeToString( pui8Data + I, _dtTargetType, ssResult, _uiNumDigits, _iSigDigits );
		}
		return ssResult;
	}

	// Converts Katakana characters to Hiragana across a UTF-8 string.
	CSecureString CUtilities::KatakanaToHiragana( const std::string &_sString ) {
		CSecureString ssTemp;
		size_t I = 0;
		size_t sSize;
		do {
			uint32_t ui32This = NextUtf8Char( &_sString[I], _sString.size() - I, &sSize );
			uint32_t ui32Len;
			uint32_t ui32Converted = KatakanaToHiragana( Utf32ToUtf16( ui32This, ui32Len ) );
			size_t sConvertedSize;
			ui32This = Utf32ToUtf8( NextUtf16Char( reinterpret_cast<const wchar_t *>(&ui32Converted), sizeof( ui32Converted ) / sizeof( wchar_t ), &sConvertedSize ), ui32Len );

			for ( uint32_t J = 0; J < ui32Len; ++J ) {
				ssTemp.push_back( static_cast<wchar_t>(ui32This & 0xFFU) );
				ui32Converted >>= 8;
			}
			I += sSize;
		} while ( I < _sString.size() );
		return ssTemp;
	}

	// Converts Katakana characters to Hiragana across a UTF-16 string.
	CSecureWString CUtilities::KatakanaToHiragana( const std::wstring &_wsString ) {
		CSecureWString swsTemp;
		size_t I = 0;
		size_t sSize;
		do {
			uint32_t ui32This = NextUtf16Char( &_wsString[I], _wsString.size() - I, &sSize );
			uint32_t ui32Len;
			uint32_t ui32Converted = KatakanaToHiragana( Utf32ToUtf16( ui32This, ui32Len ) );
			uint32_t ui32Converted2 = KatakanaToHiragana( ui32This );
			size_t sConvertedSize;
			ui32This = Utf32ToUtf16( NextUtf16Char( reinterpret_cast<const wchar_t *>(&ui32Converted), sizeof( ui32Converted ) / sizeof( wchar_t ), &sConvertedSize ), ui32Len );

			for ( uint32_t J = 0; J < ui32Len; ++J ) {
				swsTemp.push_back( static_cast<wchar_t>(ui32This & 0xFFFFU) );
				ui32Converted >>= 16;
			}
			I += sSize;
		} while ( I < _wsString.size() );
		return swsTemp;
	}

	// Breaks an input string into an array of strings.
	std::vector<CSecureString> CUtilities::StringToLines( const std::string &_sInput ) {
		std::vector<CSecureString> vLines;
		CSecureString ssTemp;
		size_t sLen = 1;
		for ( size_t I = 0; I < _sInput.size() && sLen; I += sLen ) {
			uint32_t uiNext = NextUtf8Char( &_sInput[I], _sInput.size() - I, &sLen );
			if ( sLen == 1 ) {
				// It was just a regular character.  Could be an escape.
				uiNext = ResolveEscape( &_sInput[I], _sInput.size() - I, sLen, false );
			}
			uint32_t ui32Len;
			uint32_t ui32BackToUtf8 = Utf32ToUtf8( uiNext, ui32Len );
			if ( ui32BackToUtf8 == MX_UTF_INVALID ) {
				ui32Len = 1;
				ui32BackToUtf8 = uiNext;
			}
			if ( ui32BackToUtf8 == '\r' ) { continue; }
			if ( ui32BackToUtf8 == '\n' ) {
				if ( ssTemp.size() ) {
					vLines.push_back( ssTemp );
					ssTemp.clear();
				}
				continue;
			}
			for ( uint32_t J = 0; J < ui32Len; ++J ) {
				ssTemp.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
				ui32BackToUtf8 >>= 8;
			}
		}
		if ( ssTemp.size() ) {
			vLines.push_back( ssTemp );
		}
		return vLines;
	}

	// Creates meta data for a given series of 8-bit ANSI characters.  The flags determine what metadata to generate.
	bool CUtilities::BuildStringDatabaseAnsi( const uint8_t * _puiData, size_t _sBytes,
		uint32_t _uiFlags, uint32_t _uiFlagsEx,
		std::vector<WORD> &_wC1Props, std::vector<WORD> &_wC2Props, std::vector<WORD> &_wC3Props ) {
		const struct {
			bool					bNeedsType;
			DWORD					dwFlags;
			std::vector<WORD>	  * pvBuffer;
		}
		bArray[] = {
			{ NeedsType1( _uiFlags, _uiFlagsEx ), CT_CTYPE1, &_wC1Props },
			{ NeedsType2( _uiFlags ), CT_CTYPE2, &_wC2Props },
			{ NeedsType3( _uiFlags ), CT_CTYPE3, &_wC3Props },
		};
		for ( size_t I = 0; I < MX_ELEMENTS( bArray ); ++I ) {
			if ( bArray[I].bNeedsType ) {
				if ( bArray[I].pvBuffer->size() < _sBytes ) {
					try { bArray[I].pvBuffer->resize( _sBytes ); }
					catch ( const std::bad_alloc /*& _eE*/ ) { return false; }
				}
				for ( size_t J = 0; J < _sBytes; ++J ) {
					uint32_t ui32This = _puiData[J];
					int iRet = ::MultiByteToWideChar( CP_ACP, MB_ERR_INVALID_CHARS, reinterpret_cast<LPCCH>(&_puiData[J]), 1, reinterpret_cast<LPWSTR>(&ui32This), sizeof( ui32This ) / sizeof( WCHAR ) );
					//const int iRet = 1;
					if ( iRet ) {
						::GetStringTypeW( bArray[I].dwFlags, reinterpret_cast<LPCWCH>(&ui32This),
							iRet,
							&(*bArray[I].pvBuffer)[J] );
					}
					else {
						(*bArray[I].pvBuffer)[J] = 0;
					}
				}
			}
		}

		return true;
	}

	// Creates meta data for a given series of UTF-8 characters.  The flags determine what metadata to generate.
	bool CUtilities::BuildStringDatabaseUtf8( const uint8_t * _puiData, size_t _sBytes,
		uint32_t _uiFlags, uint32_t _uiFlagsEx,
		std::vector<WORD> &_wC1Props, std::vector<WORD> &_wC2Props, std::vector<WORD> &_wC3Props ) {

		const struct {
			bool					bNeedsType;
			DWORD					dwFlags;
			std::vector<WORD>	  * pvBuffer;
		}
		bArray[] = {
			{ NeedsType1( _uiFlags, _uiFlagsEx ), CT_CTYPE1, &_wC1Props },
			{ NeedsType2( _uiFlags ), CT_CTYPE2, &_wC2Props },
			{ NeedsType3( _uiFlags ), CT_CTYPE3, &_wC3Props },
		};
		for ( size_t I = 0; I < MX_ELEMENTS( bArray ); ++I ) {
			if ( bArray[I].bNeedsType ) {
				if ( bArray[I].pvBuffer->size() < _sBytes ) {
					try { bArray[I].pvBuffer->resize( _sBytes ); }
					catch ( const std::bad_alloc /*& _eE*/ ) { return false; }
				}
				for ( size_t J = 0; J < _sBytes; ++J ) {
					size_t sLen = 1;
					uint32_t ui32This = NextUtf8Char( reinterpret_cast<const char *>(&_puiData[J]), _sBytes - J, &sLen );
					/*if ( ui32This == MX_UTF_INVALID ) {
						ui32This = _puiData[J];
						sLen = 1;
					}*/
					uint32_t ui32Len;
					uint32_t ui32Converted = Utf32ToUtf16( ui32This, ui32Len );
					::GetStringTypeW( bArray[I].dwFlags, reinterpret_cast<LPCWCH>(&ui32Converted),
						std::min( static_cast<int>(ui32Len), static_cast<int>(_sBytes - J) ),
						&(*bArray[I].pvBuffer)[J] );
				}
			}
		}

		return true;
	}

	// Creates meta data for a given buffer of of UTF-16 characters.  The flags determine what metadata to generate.
	bool CUtilities::BuildStringDatabaseUtf16( const uint8_t * _puiData, size_t _sBytes,
		uint32_t _uiFlags, uint32_t _uiFlagsEx,
		std::vector<WORD> &_wC1Props, std::vector<WORD> &_wC2Props, std::vector<WORD> &_wC3Props ) {
		const struct {
			bool					bNeedsType;
			DWORD					dwFlags;
			std::vector<WORD>	  * pvBuffer;
		}
		bArray[] = {
			{ NeedsType1( _uiFlags, _uiFlagsEx ), CT_CTYPE1, &_wC1Props },
			{ NeedsType2( _uiFlags ), CT_CTYPE2, &_wC2Props },
			{ NeedsType3( _uiFlags ), CT_CTYPE3, &_wC3Props },
		};
		for ( size_t I = 0; I < MX_ELEMENTS( bArray ); ++I ) {
			if ( bArray[I].bNeedsType ) {
				if ( bArray[I].pvBuffer->size() < _sBytes ) {
					try { bArray[I].pvBuffer->resize( _sBytes ); }
					catch ( const std::bad_alloc /*& _eE*/ ) { return false; }
				}
				for ( size_t J = 0; J < _sBytes; ++J ) {
					size_t sLen = 1;
					uint32_t ui32This = NextUtf16Char( reinterpret_cast<const wchar_t *>(&_puiData[J]), _sBytes - J, &sLen );
					/*if ( ui32This == MX_UTF_INVALID ) {
						ui32This = _puiData[J];
						sLen = 1;
					}*/
					uint32_t ui32Len;
					uint32_t ui32Converted = Utf32ToUtf16( ui32This, ui32Len );
					::GetStringTypeW( bArray[I].dwFlags, reinterpret_cast<LPCWCH>(&ui32Converted),
						std::min( static_cast<int>(ui32Len), static_cast<int>(_sBytes - J) ),
						&(*bArray[I].pvBuffer)[J] );
				}
			}
		}

		return true;
	}

	// Creates meta data for a given buffer of of UTF-32 characters.  The flags determine what metadata to generate.
	bool CUtilities::BuildStringDatabaseUtf32( const uint8_t * _puiData, size_t _sBytes,
		uint32_t _uiFlags, uint32_t _uiFlagsEx,
		std::vector<WORD> &_wC1Props, std::vector<WORD> &_wC2Props, std::vector<WORD> &_wC3Props ) {
		const struct {
			bool					bNeedsType;
			DWORD					dwFlags;
			std::vector<WORD>	  * pvBuffer;
		}
		bArray[] = {
			{ NeedsType1( _uiFlags, _uiFlagsEx ), CT_CTYPE1, &_wC1Props },
			{ NeedsType2( _uiFlags ), CT_CTYPE2, &_wC2Props },
			{ NeedsType3( _uiFlags ), CT_CTYPE3, &_wC3Props },
		};
		for ( size_t I = 0; I < MX_ELEMENTS( bArray ); ++I ) {
			if ( bArray[I].bNeedsType ) {
				if ( bArray[I].pvBuffer->size() < _sBytes ) {
					try { bArray[I].pvBuffer->resize( _sBytes ); }
					catch ( const std::bad_alloc /*& _eE*/ ) { return false; }
				}
				for ( size_t J = 0; J < _sBytes; ++J ) {
					size_t sLen = 1;
					uint32_t ui32This = NextUtf32Char( reinterpret_cast<const uint32_t *>(&_puiData[J]), _sBytes - J, &sLen );
					/*if ( ui32This == MX_UTF_INVALID ) {
						ui32This = _puiData[J];
						sLen = 1;
					}*/
					uint32_t ui32Len;
					uint32_t ui32Converted = Utf32ToUtf16( ui32This, ui32Len );
					::GetStringTypeW( bArray[I].dwFlags, reinterpret_cast<LPCWCH>(&ui32Converted),
						std::min( static_cast<int>(ui32Len), static_cast<int>(_sBytes - J) ),
						&(*bArray[I].pvBuffer)[J] );
				}
			}
		}

		return true;
	}
	
	// Composes the given data into something searchable.
	bool CUtilities::ComposeDataIntoSearchableStringAnsi( const uint8_t * _puiData, size_t _sBytes,
		uint32_t _uiFlags, uint32_t _uiFlagsEx,
		const std::vector<WORD> &_wC1Props,
		const std::vector<WORD> &_wC3Props,
		std::vector<MX_CHAR_DATA> &_vData,
		bool /*_bAligned*/,
		bool /*_bByteSwapped*/ ) {
		if ( _vData.size() < _sBytes ) {
			try { _vData.resize( _sBytes ); }
			catch ( const std::bad_alloc /*& _eE*/ ) { return false; }
		}

		for ( size_t I = 0; I < _sBytes; ++I ) {
			_vData[I].ui8UftLen = 1;
			uint32_t ui32Converted = _puiData[I];
			if ( (_uiFlags & MX_SSF_LINGUISTIC_IGNOREDIACRITIC) && (_wC3Props[I] & C3_DIACRITIC) ) {
				//uint32_t ui32This;
				WCHAR szBuffer[8];
				int iRet = ::MultiByteToWideChar( CP_ACP, MB_ERR_INVALID_CHARS | MB_COMPOSITE, reinterpret_cast<LPCCH>(&ui32Converted), 1, szBuffer, MX_ELEMENTS( szBuffer ) );
				if ( iRet ) {
					ui32Converted = szBuffer[0];
				}
			}

			/*if ( _uiFlags & MX_SSF_NORM_IGNOREKANATYPE ) {
				ui32Converted = CUtilities::KatakanaToHiragana( ui32Converted );
			}
			if ( _uiFlags & MX_SSF_NORM_IGNOREWIDTH ) {
				ui32Converted = CUtilities::FullWidthToByte( ui32Converted );
			}*/
			if ( _uiFlags & MX_SSF_NORM_IGNORESYMBOLS ) {
				if ( _wC1Props[I] & C1_PUNCT || _wC3Props[I] & C3_SYMBOL ) {
					if ( ui32Converted != '?' && ui32Converted != '*' || !(_uiFlagsEx & MX_SSF_WILDCARD) ) {
						ui32Converted = '.';
					}
				}
			}

			if ( _uiFlags & MX_SSF_LINGUISTIC_IGNORECASE ) {
				ui32Converted = ::_mbctoupper( ui32Converted );
			}
			else if ( _uiFlags & MX_SSF_NORM_IGNORECASE ) {
				ui32Converted = std::towupper( ui32Converted );
			}
			_vData[I].ui32SearchChar = ui32Converted;
		}
		return true;
	}

	// Composes the given data into something searchable.
	bool CUtilities::ComposeDataIntoSearchableStringUtf8( const uint8_t * _puiData, size_t _sBytes,
		uint32_t _uiFlags, uint32_t _uiFlagsEx,
		const std::vector<WORD> &_wC1Props,
		const std::vector<WORD> &_wC3Props,
		std::vector<MX_CHAR_DATA> &_vData,
		bool /*_bAligned*/,
		bool /*_bByteSwapped*/ ) {
		if ( _vData.size() < _sBytes ) {
			try { _vData.resize( _sBytes ); }
			catch ( const std::bad_alloc /*& _eE*/ ) { return false; }
		}

		for ( size_t I = 0; I < _sBytes; ++I ) {
			size_t sLen = 1;
			uint32_t ui32Temp = NextUtf8Char( reinterpret_cast<const char *>(&_puiData[I]), _sBytes - I, &sLen );
			_vData[I].ui8UftLen = static_cast<uint8_t>(sLen);
			if ( ui32Temp != MX_UTF_INVALID ) {
				uint32_t ui32Len;
				uint32_t ui32Converted = Utf32ToUtf16( ui32Temp, ui32Len );
				if ( (_uiFlags & MX_SSF_LINGUISTIC_IGNOREDIACRITIC) && (_wC3Props[I] & C3_DIACRITIC) ) {
					wchar_t szBuffer[32];
					int iNorm = ::NormalizeString( NormalizationKD, reinterpret_cast<LPCWCH>(&ui32Converted), ui32Len, szBuffer, MX_ELEMENTS( szBuffer ) );
					if ( iNorm > 0 ) {
						ui32Converted = szBuffer[0];
						ui32Len = 1;
					}
				}

				if ( _uiFlags & MX_SSF_NORM_IGNOREKANATYPE ) {
					ui32Converted = CUtilities::KatakanaToHiragana( ui32Converted );
				}
				if ( _uiFlags & MX_SSF_NORM_IGNOREWIDTH ) {
					ui32Converted = CUtilities::FullWidthToByte( ui32Converted );
				}
				if ( _uiFlags & MX_SSF_NORM_IGNORESYMBOLS ) {
					if ( _wC1Props[I] & C1_PUNCT || _wC3Props[I] & C3_SYMBOL ) {
						if ( ui32Converted != '?' && ui32Converted != '*' || !(_uiFlagsEx & MX_SSF_WILDCARD) ) {
							ui32Converted = '.';
						}
					}
				}

				if ( _uiFlags & MX_SSF_LINGUISTIC_IGNORECASE ) {
					ui32Converted = CUtilities::WideToUpper( ui32Converted );
				}
				else if ( _uiFlags & MX_SSF_NORM_IGNORECASE ) {
					ui32Converted = std::towupper( ui32Converted );
				}
				_vData[I].ui32SearchChar = NextUtf16Char( reinterpret_cast<const wchar_t *>(&ui32Converted), 2 );
			}
			else {
				_vData[I].ui32SearchChar = ui32Temp;
			}
		}
		return true;
	}

	// Composes the given data into something searchable.
	bool CUtilities::ComposeDataIntoSearchableStringUtf16( const uint8_t * _puiData, size_t _sBytes,
		uint32_t _uiFlags, uint32_t _uiFlagsEx,
		const std::vector<WORD> &_wC1Props,
		const std::vector<WORD> &_wC3Props,
		std::vector<MX_CHAR_DATA> &_vData,
		bool _bAligned,
		bool _bByteSwapped ) {
		//size_t sFinalSize = _bAligned ? (_sBytes >> 1) : _sBytes;
		size_t sFinalSize = _sBytes;
		if ( _vData.size() < sFinalSize ) {
			try { _vData.resize( sFinalSize ); }
			catch ( const std::bad_alloc /*& _eE*/ ) { return false; }
		}

		size_t sSize = _bAligned ? sizeof( uint16_t ) : 1;
		for ( size_t I = 0; I < _sBytes; I += sSize ) {
			size_t sIdx = I;// >> 1;
			size_t sLen = 1;
			uint32_t ui32Temp;
			if ( _bByteSwapped ) {
				wchar_t szByteSwapped[2] =  { 0 };
				size_t sLeft = I - _sBytes;
				szByteSwapped[0] = ::_byteswap_ushort( reinterpret_cast<const wchar_t *>(&_puiData[sIdx])[0] );
				if ( sLeft >= 2 ) {
					szByteSwapped[1] = ::_byteswap_ushort( reinterpret_cast<const wchar_t *>(&_puiData[sIdx])[1] );
				}
				ui32Temp = NextUtf16Char( szByteSwapped, 2, &sLen );
			}
			else {
				ui32Temp = NextUtf16Char( reinterpret_cast<const wchar_t *>(&_puiData[sIdx]), _sBytes - sIdx, &sLen );
			}
			_vData[sIdx].ui8UftLen = static_cast<uint8_t>(sLen * sizeof( wchar_t ));
			if ( ui32Temp != MX_UTF_INVALID ) {
				uint32_t ui32Len;
				uint32_t ui32Converted = Utf32ToUtf16( ui32Temp, ui32Len );
				if ( (_uiFlags & MX_SSF_LINGUISTIC_IGNOREDIACRITIC) && (_wC3Props[sIdx] & C3_DIACRITIC) ) {
					wchar_t szBuffer[32];
					int iNorm = ::NormalizeString( NormalizationKD, reinterpret_cast<LPCWCH>(&ui32Converted), ui32Len, szBuffer, MX_ELEMENTS( szBuffer ) );
					if ( iNorm > 0 ) {
						ui32Converted = szBuffer[0];
						ui32Len = 1;
					}
				}

				if ( _uiFlags & MX_SSF_NORM_IGNOREKANATYPE ) {
					ui32Converted = CUtilities::KatakanaToHiragana( ui32Converted );
				}
				if ( _uiFlags & MX_SSF_NORM_IGNOREWIDTH ) {
					ui32Converted = CUtilities::FullWidthToByte( ui32Converted );
				}
				if ( _uiFlags & MX_SSF_NORM_IGNORESYMBOLS ) {
					if ( _wC1Props[sIdx] & C1_PUNCT || _wC3Props[sIdx] & C3_SYMBOL ) {
						if ( ui32Converted != '?' && ui32Converted != '*' || !(_uiFlagsEx & MX_SSF_WILDCARD) ) {
							ui32Converted = '.';
						}
					}
				}

				if ( _uiFlags & MX_SSF_LINGUISTIC_IGNORECASE ) {
					ui32Converted = CUtilities::WideToUpper( ui32Converted );
				}
				else if ( _uiFlags & MX_SSF_NORM_IGNORECASE ) {
					ui32Converted = std::towupper( ui32Converted );
				}
				_vData[sIdx].ui32SearchChar = NextUtf16Char( reinterpret_cast<const wchar_t *>(&ui32Converted), 2 );
			}
			else {
				_vData[sIdx].ui32SearchChar = ui32Temp;
			}
		}
		return true;
	}

	// Composes the given data into something searchable.
	bool CUtilities::ComposeDataIntoSearchableStringUtf32( const uint8_t * _puiData, size_t _sBytes,
		uint32_t _uiFlags, uint32_t _uiFlagsEx,
		const std::vector<WORD> &_wC1Props,
		const std::vector<WORD> &_wC3Props,
		std::vector<MX_CHAR_DATA> &_vData,
		bool _bAligned,
		bool _bByteSwapped ) {
		//size_t sFinalSize = _bAligned ? (_sBytes >> 2) : _sBytes;
		size_t sFinalSize = _sBytes;
		if ( _vData.size() < sFinalSize ) {
			try { _vData.resize( sFinalSize ); }
			catch ( const std::bad_alloc /*& _eE*/ ) { return false; }
		}

		size_t sSize = _bAligned ? sizeof( uint32_t ) : 1;
		for ( size_t I = 0; I < _sBytes; I += sSize ) {
			size_t sIdx = I;// >> 2;
			size_t sLen = 1;
			uint32_t ui32Char = _bByteSwapped ?
				::_byteswap_ulong( (*reinterpret_cast<const uint32_t *>(&_puiData[sIdx])) ) :
				(*reinterpret_cast<const uint32_t *>(&_puiData[sIdx]));
			uint32_t ui32Temp = NextUtf32Char( &ui32Char, _sBytes - sIdx, &sLen );
			_vData[sIdx].ui8UftLen = static_cast<uint8_t>(sLen * sizeof( uint32_t ));
			if ( ui32Temp != MX_UTF_INVALID ) {
				uint32_t ui32Len;
				uint32_t ui32Converted = Utf32ToUtf16( ui32Temp, ui32Len );
				if ( (_uiFlags & MX_SSF_LINGUISTIC_IGNOREDIACRITIC) && (_wC3Props[sIdx] & C3_DIACRITIC) ) {
					wchar_t szBuffer[32];
					int iNorm = ::NormalizeString( NormalizationKD, reinterpret_cast<LPCWCH>(&ui32Converted), ui32Len, szBuffer, MX_ELEMENTS( szBuffer ) );
					if ( iNorm > 0 ) {
						ui32Converted = szBuffer[0];
						ui32Len = 1;
					}
				}

				if ( _uiFlags & MX_SSF_NORM_IGNOREKANATYPE ) {
					ui32Converted = CUtilities::KatakanaToHiragana( ui32Converted );
				}
				if ( _uiFlags & MX_SSF_NORM_IGNOREWIDTH ) {
					ui32Converted = CUtilities::FullWidthToByte( ui32Converted );
				}
				if ( _uiFlags & MX_SSF_NORM_IGNORESYMBOLS ) {
					if ( _wC1Props[sIdx] & C1_PUNCT || _wC3Props[sIdx] & C3_SYMBOL ) {
						if ( ui32Converted != '?' && ui32Converted != '*' || !(_uiFlagsEx & MX_SSF_WILDCARD) ) {
							ui32Converted = '.';
						}
					}
				}

				if ( _uiFlags & MX_SSF_LINGUISTIC_IGNORECASE ) {
					ui32Converted = CUtilities::WideToUpper( ui32Converted );
				}
				else if ( _uiFlags & MX_SSF_NORM_IGNORECASE ) {
					ui32Converted = std::towupper( ui32Converted );
				}
				_vData[sIdx].ui32SearchChar = NextUtf16Char( reinterpret_cast<const wchar_t *>(&ui32Converted), 2 );
			}
			else {
				_vData[sIdx].ui32SearchChar = ui32Temp;
			}
		}
		return true;
	}

	// Prints the number of GUI objects.
	void CUtilities::PrintTotalGuiObjects( DWORD /*_dwFlags*/ ) {
		/*char szBuffer[64];
		::sprintf( szBuffer, "PrintTotalGuiObjects: %u.\r\n", ::GetGuiResources( ::GetCurrentProcess(), _dwFlags ) );
		::OutputDebugStringA( szBuffer );*/
	}

	// Prints an ee::CExpEvalContainer::EE_RESULT value.
	std::wstring CUtilities::PrintExpResultStandard( const ee::CExpEvalContainer::EE_RESULT &_rResult, uint64_t _ui64Data, bool _bClosingPar ) {
		CSecureWString swString;
		switch ( _rResult.ncType ) {
			case ee::EE_NC_SIGNED : {
				ToSigned( _rResult.u.i64Val, swString );
				swString.push_back( L' ' );
				swString.push_back( L'(' );
				ToHex( _rResult.u.i64Val, swString, 1 );
				if ( _rResult.u.i64Val < 0 ) {
					swString.push_back( L',' );
					swString.push_back( L' ' );
					swString.push_back( L'-' );
					ToHex( -_rResult.u.i64Val, swString, 1 );
				}
				if ( _bClosingPar ) {
					swString.push_back( L')' );
				}
				break;
			}
			case ee::EE_NC_UNSIGNED : {
				ToUnsigned( _rResult.u.ui64Val, swString );
				swString.push_back( L' ' );
				swString.push_back( L'(' );
				ToHex( _rResult.u.ui64Val, swString, 1 );
				if ( _bClosingPar ) {
					swString.push_back( L')' );
				}
				break;
			}
			case ee::EE_NC_FLOATING : {
				ToDouble( _rResult.u.dVal, swString, static_cast<int32_t>(_ui64Data) );
				swString.push_back( L' ' );
				swString.push_back( L'(' );
				// As float hex.
				FLOAT fVal = static_cast<FLOAT>(_rResult.u.dVal);
				uint32_t ui32FloatVal = (*reinterpret_cast<uint32_t *>(&fVal));
				ToHex( ui32FloatVal, swString, 8 );

				swString.push_back( L',' );
				swString.push_back( L' ' );
				ToHex( _rResult.u.i64Val, swString, 16 );

				if ( _bClosingPar ) {
					swString.push_back( L')' );
				}
				break;
			}
			default : {
				swString.append( _DEC_WS_3424431C_Invalid );
			}
		}
		return swString;
	}

	// Prints an ee::CExpEvalContainer::EE_RESULT value with extended print-outs.
	std::wstring CUtilities::PrintExpResultExtended( const ee::CExpEvalContainer::EE_RESULT &_rResult, uint64_t _ui64Data, bool _bClosingPar ) {
		std::wstring wsTemp = PrintExpResultStandard( _rResult, _ui64Data, false );
		wsTemp.push_back( L',' );
		wsTemp.push_back( L' ' );

		// 32-bit float in binary.
		if ( _rResult.ncType == ee::EE_NC_FLOATING ) {
				union {
					float			fVal;
					uint32_t		ui32Val;
				} uTemp;
				uTemp.fVal = static_cast<float>(_rResult.u.dVal);

				ToBinary( uTemp.ui32Val, wsTemp, 32 );

				wsTemp.push_back( L',' );
				wsTemp.push_back( L' ' );
		}

		// Get the number of bits to display.
		uint32_t ui32Bits = _rResult.ncType == ee::EE_NC_FLOATING ? 64 : 1;
		if ( _rResult.ncType == ee::EE_NC_SIGNED ) {
			if ( _rResult.u.i64Val >= SCHAR_MIN && _rResult.u.i64Val <= SCHAR_MAX ) {
				ui32Bits = CHAR_BIT;
			}
			else if ( _rResult.u.i64Val >= SHRT_MIN && _rResult.u.i64Val <= SHRT_MAX ) {
				ui32Bits = 16;
			}
			else if ( _rResult.u.i64Val >= INT_MIN && _rResult.u.i64Val <= INT_MAX ) {
				ui32Bits = 32;
			}
			else {
				ui32Bits = 64;
			}
		}
		else if ( _rResult.ncType == ee::EE_NC_UNSIGNED ) {
			if ( _rResult.u.ui64Val < (1ULL << 8ULL) ) {
				ui32Bits = 8;
			}
			else if ( _rResult.u.ui64Val < (1ULL << 16ULL) ) {
				ui32Bits = 16;
			}
			else if ( _rResult.u.ui64Val < (1ULL << 32ULL) ) {
				ui32Bits = 32;
			}
			else {
				ui32Bits = 64;
			}
		}

		// Integer or 64-bit float to binary.
		ToBinary( _rResult.u.ui64Val, wsTemp, ui32Bits );


		if ( _rResult.ncType == ee::EE_NC_SIGNED && _rResult.u.i64Val < 0 ) {
			wsTemp.push_back( L',' );
			wsTemp.push_back( L' ' );
			wsTemp.push_back( L'-' );
			// Inverted-sign version.
			ToBinary( -_rResult.u.ui64Val, wsTemp, ui32Bits );
		}
		if ( _bClosingPar ) {
			wsTemp.push_back( L')' );
		}
		return wsTemp;
	}

	// Prints an ee::CExpEvalContainer::EE_RESULT value as a date/time.
	std::wstring CUtilities::PrintExpResultDataTime( const ee::CExpEvalContainer::EE_RESULT &_rResult, uint64_t _ui64Data, bool _bClosingPar ) {
		std::wstring wsTemp;
		// Microseconds can be nabbed from floating-point values.  Get the fractional part if it exists.
		double dFrac = 0.0;
		if ( _rResult.ncType == ee::EE_NC_FLOATING ) {
			double dInt = 0.0;
			dFrac = std::modf( _rResult.u.dVal, &dInt );
		}
		ee::CExpEvalContainer::EE_RESULT rConv = ee::CExpEvalContainer::ConvertResult( _rResult, ee::EE_NC_UNSIGNED );

		FILETIME ftFileTime = {
			static_cast<DWORD>(rConv.u.ui64Val),
			static_cast<DWORD>(rConv.u.ui64Val >> 32ULL)
		};

		// Manual breakdown.  Time value assumed to be seconds.

		uint32_t uiSeconds =		static_cast<uint32_t>(rConv.u.ui64Val % 60 );
		uint32_t uiMinutes =		static_cast<uint32_t>(rConv.u.ui64Val / 60 % 60 );
		uint32_t uiHours =			static_cast<uint32_t>(rConv.u.ui64Val / 60 / 60 % 24 );
		uint32_t uiDays =			static_cast<uint32_t>(rConv.u.ui64Val / 60 / 60 / 24 % 365 );
		//uint32_t uiWeeks =			static_cast<uint32_t>(rConv.u.ui64Val / 1000ULL / 60 / 60 / 24 / 7 % 52 );
		uint32_t uiYears =			static_cast<uint32_t>(rConv.u.ui64Val / 60 / 60 / 24 / 365 );

		SYSTEMTIME stTime;
		const char * pcTemp = std::ctime( reinterpret_cast<const time_t * const>(&rConv.u.ui64Val) );

		if ( pcTemp ) {
			wsTemp = StringToWString( pcTemp );
		}
		else {
			wsTemp.append( _DEC_WS_3424431C_Invalid );
		}

		wsTemp.push_back( L',' );
		wsTemp.push_back( L' ' );

		
		WCHAR szBuffer[256];
		::swprintf_s( szBuffer, MX_ELEMENTS( szBuffer ), L"%uy:%ud:%.2uh:%.2um:", uiYears, uiDays, uiHours, uiMinutes );
		wsTemp.append( szBuffer );
		ToDouble( uiSeconds + dFrac, wsTemp );
		wsTemp.push_back( L's' );

		if ( !::FileTimeToSystemTime( &ftFileTime, &stTime ) ) {
			wsTemp.append( _DEC_WS_3424431C_Invalid );
		}
		else {
			WCHAR szBuffer[256];
			//::swprintf_s( szBuffer, MX_ELEMENTS( szBuffer ), L"%.2000f", _dValue );
		}

		/*if ( _bClosingPar ) {
			wsTemp.push_back( L')' );
		}*/
		return wsTemp;
	}

}	// namespace mx

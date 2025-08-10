#include "MXUtilities.h"
#include "../CodePages/MXCodePages.h"
#include "../Html/MXHtml.h"
#include "../MemHack/MXProcess.h"
#include "../Options/MXOptions.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"

#include <EEExpEval.h>
#include <Float16/EEFloat16.h>
#include <Object/EEObject.h>

#include <algorithm>
#include <cmath>
#include <codecvt>
#include <ctime>
#include <cuchar>
#include <locale>

namespace mx {

	// Options.
	CUtilities::MX_UTIL_OPTIONS CUtilities::Options = {
		FALSE,							// bUse0xForHex
		FALSE,							// bUse0oForOct
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
		{ MX_DT_FLOAT,		_T_LEN_C9A55E95_float,		_T_LEN_08647191_Float,				_T_LEN_93C565BD___3_4028235e_38,										sizeof( float ),		__alignof( float ),		TRUE,		TRUE },
		{ MX_DT_FLOAT16,	_T_LEN_C8DB4BDD_float16,	_T_LEN_5B68EE1F_Half,				_T_LEN_56F7FDCA___65_504,												sizeof( uint16_t ),		__alignof( uint16_t ),	TRUE,		TRUE },
		{ MX_DT_DOUBLE,		_T_LEN_DAE7F2EF_double,		_T_LEN_DD4BF7D9_Double,				_T_LEN_5D5EE229___1_7976931348623157e_308,								sizeof( double ),		__alignof( double ),	TRUE,		TRUE },
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

	/**
	 * Convert a single byte to hex.  _pcString must be at least 3 characters long.
	 * 
	 * \param _bIn The 8-bit value to convert to a 2-character (plus NULL) string.
	 * \param _pcString The retun string, which must be long enough to contain the 2 characters and final NULL.
	 * \param _bLower If FALSE, capital letters are used for characters A-F.
	 **/
	VOID CUtilities::ByteToHex( BYTE _bIn, CHAR * _pcString, BOOL _bLower ) {
		BYTE bTemp = (_bIn >> 4);
#define MX_HEX_TO_CHAR( VAL )		((VAL) < 10 ? ('0' + (VAL)) : ((_bLower ? 'a' : 'A') + ((VAL) - 10)))
		_pcString[0] = MX_HEX_TO_CHAR( bTemp );
		bTemp = (_bIn & 0xF);
		_pcString[1] = MX_HEX_TO_CHAR( bTemp );
		_pcString[2] = '\0';
	}

	/**
	 * Convert a single byte to hex.
	 * 
	 * \param _bIn The 8-bit value to convert to a 2-character string.
	 * \param _sString The retun string.
	 * \param _bLower If FALSE, capital letters are used for characters A-F.
	 **/
	VOID CUtilities::ByteToHex( BYTE _bIn, std::string &_sString, BOOL _bLower ) {
		BYTE bTemp = (_bIn >> 4);
		
		_sString.push_back( MX_HEX_TO_CHAR( bTemp ) );
		bTemp = (_bIn & 0xF);
		_sString.push_back( MX_HEX_TO_CHAR( bTemp ) );
#undef MX_HEX_TO_CHAR
	}
	
	/**
	 * Convert a byte array to a hex string.  _pcString must be twice as long as the input, plus 1 character for the NULL.
	 * 
	 * \param _pvIn The input array of bytes.
	 * \param _sLen The number of bytes to which _pvIn points.
	 * \param _pcString The retun string, which must be long enough to contain the 2 characters and final NULL.
	 * \param _bLower If FALSE, capital letters are used for characters A-F.
	 **/
	VOID CUtilities::BytesToHex( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		for ( size_t I = 0; I < _sLen; ++I, ++bIn, _pcString += 2 ) {
			ByteToHex( (*bIn), _pcString, _bLower );
		}
	}

	/**
	 * Convert a byte array to a hex string.
	 * 
	 * \param _pvIn The input array of bytes.
	 * \param _sLen The number of bytes to which _pvIn points.
	 * \param _sString The retun string.
	 * \param _bLower If FALSE, capital letters are used for characters A-F.
	 **/
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
			{ MX_IMAGE_FILE_MACHINE_UNKNOWN, _T_926359DD_UNKNOWN, _LEN_926359DD }, // 0
			{ MX_IMAGE_FILE_MACHINE_AM33, _T_3115FAF1_AM33, _LEN_3115FAF1 }, // 1
			{ MX_IMAGE_FILE_MACHINE_AMD64, _T_15F446D8_AMD64, _LEN_15F446D8 }, // 2
			{ MX_IMAGE_FILE_MACHINE_ARM, _T_0EF93C1E_ARM, _LEN_0EF93C1E }, // 3
			{ MX_IMAGE_FILE_MACHINE_ARMNT, _T_B80BA838_ARMNT, _LEN_B80BA838 }, // 4
			{ MX_IMAGE_FILE_MACHINE_ARM64, _T_125F3A9E_ARM64, _LEN_125F3A9E }, // 5
			{ MX_IMAGE_FILE_MACHINE_EBC, _T_A48AAB94_EBC, _LEN_A48AAB94 }, // 6
			{ MX_IMAGE_FILE_MACHINE_I386, _T_3950BA00_I386, _LEN_3950BA00 }, // 7
			{ MX_IMAGE_FILE_MACHINE_IA64, _T_1EA84A9C_IA64, _LEN_1EA84A9C }, // 8
			{ MX_IMAGE_FILE_MACHINE_M32R, _T_0602609C_M32R, _LEN_0602609C }, // 9
			{ MX_IMAGE_FILE_MACHINE_MIPS16, _T_FBF78F49_MIPS16, _LEN_FBF78F49 }, // 10
			{ MX_IMAGE_FILE_MACHINE_MIPSFPU, _T_A4E1F6B9_MIPSFPU, _LEN_A4E1F6B9 }, // 11
			{ MX_IMAGE_FILE_MACHINE_MIPSFPU16, _T_29E766ED_MIPSFPU16, _LEN_29E766ED }, // 12
			{ MX_IMAGE_FILE_MACHINE_POWERPC, _T_5C188204_POWERPC, _LEN_5C188204 }, // 13
			{ MX_IMAGE_FILE_MACHINE_POWERPCFP, _T_EB4BD098_POWERPCFP, _LEN_EB4BD098 }, // 14
			{ MX_IMAGE_FILE_MACHINE_R4000, _T_264CE78F_R4000, _LEN_264CE78F }, // 15
			{ MX_IMAGE_FILE_MACHINE_SH3, _T_16CBEDE0_SH3, _LEN_16CBEDE0 }, // 16
			{ MX_IMAGE_FILE_MACHINE_SH3DSP, _T_3CABE542_SH3DSP, _LEN_3CABE542 }, // 17
			{ MX_IMAGE_FILE_MACHINE_SH4, _T_88AF7843_SH4, _LEN_88AF7843 }, // 18
			{ MX_IMAGE_FILE_MACHINE_SH5, _T_FFA848D5_SH5, _LEN_FFA848D5 }, // 19
			{ MX_IMAGE_FILE_MACHINE_THUMB, _T_E263136B_THUMB, _LEN_E263136B }, // 20
			{ MX_IMAGE_FILE_MACHINE_WCEMIPSV2, _T_26FFB052_WCEMIPSV2, _LEN_26FFB052 }, // 21
			{ MX_IMAGE_FILE_MACHINE_R3000, _T_BB9BDF36_R3000, _LEN_BB9BDF36 }, // 22
			{ MX_IMAGE_FILE_MACHINE_R10000, _T_411A8207_R10000, _LEN_411A8207 }, // 23
			{ MX_IMAGE_FILE_MACHINE_ALPHA, _T_27B4FBDA_ALPHA, _LEN_27B4FBDA }, // 24
			{ MX_IMAGE_FILE_MACHINE_SH3E, _T_74A87307_SH3E, _LEN_74A87307 }, // 25
			{ MX_IMAGE_FILE_MACHINE_ALPHA64, _T_B8C9568A_ALPHA64, _LEN_B8C9568A }, // 26
			{ MX_IMAGE_FILE_MACHINE_AXP64, _T_691CEAD9_AXP64, _LEN_691CEAD9 }, // 27
			{ MX_IMAGE_FILE_MACHINE_TRICORE, _T_220F36F9_TRICORE, _LEN_220F36F9 }, // 28
			{ MX_IMAGE_FILE_MACHINE_CEF, _T_9F2CB56E_CEF, _LEN_9F2CB56E }, // 29
			{ MX_IMAGE_FILE_MACHINE_CEE, _T_0625E4D4_CEE, _LEN_0625E4D4 }, // 30
		};

		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ); ++I ) {
			if ( aTable[I].ui32Type == _uiType ) {
				std::string sString;
				CHAR szBuffer[_T_MAX_LEN];
				if ( !Options.bShortenEnumNames ) {
					CStringDecoder::Decode( _T_LEN_6300952B_IMAGE_FILE_MACHINE_, szBuffer );
					sString += szBuffer;
				}
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, szBuffer );
				sString += szBuffer;
				std::strncpy( _pcRet, sString.c_str(), std::min( sString.size() + 1, static_cast<size_t>(_T_MAX_LEN) ) );
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
			{ MX_IMAGE_FILE_RELOCS_STRIPPED, _T_A2ECB695_RELOCS_STRIPPED, _LEN_A2ECB695 }, // 0
			{ MX_IMAGE_FILE_EXECUTABLE_IMAGE, _T_B0E0EEFC_EXECUTABLE_IMAGE, _LEN_B0E0EEFC }, // 1
			{ MX_IMAGE_FILE_LINE_NUMS_STRIPPED, _T_39BF7F5F_LINE_NUMS_STRIPPED, _LEN_39BF7F5F }, // 2
			{ MX_IMAGE_FILE_LOCAL_SYMS_STRIPPED, _T_FF7E7C79_LOCAL_SYMS_STRIPPED, _LEN_FF7E7C79 }, // 3
			{ MX_IMAGE_FILE_AGGRESIVE_WS_TRIM, _T_69F6A8E1_AGGRESIVE_WS_TRIM, _LEN_69F6A8E1 }, // 4
			{ MX_IMAGE_FILE_LARGE_ADDRESS_AWARE, _T_033403FD_LARGE_ADDRESS_AWARE, _LEN_033403FD }, // 5
			{ MX_IMAGE_FILE_BYTES_REVERSED_LO, _T_31E9B75E_BYTES_REVERSED_LO, _LEN_31E9B75E }, // 6
			{ MX_IMAGE_FILE_32BIT_MACHINE, _T_B5CAEC4C_32BIT_MACHINE, _LEN_B5CAEC4C }, // 7
			{ MX_IMAGE_FILE_DEBUG_STRIPPED, _T_6EA85C7C_DEBUG_STRIPPED, _LEN_6EA85C7C }, // 8
			{ MX_IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, _T_23B2E2A2_REMOVABLE_RUN_FROM_SWAP, _LEN_23B2E2A2 }, // 9
			{ MX_IMAGE_FILE_NET_RUN_FROM_SWAP, _T_C6F084C4_NET_RUN_FROM_SWAP, _LEN_C6F084C4 }, // 10
			{ MX_IMAGE_FILE_SYSTEM, _T_39B5D60D_SYSTEM, _LEN_39B5D60D }, // 11
			{ MX_IMAGE_FILE_DLL, _T_AB74F1BC_DLL, _LEN_AB74F1BC }, // 12
			{ MX_IMAGE_FILE_UP_SYSTEM_ONLY, _T_E326A29D_UP_SYSTEM_ONLY, _LEN_E326A29D }, // 13
			{ MX_IMAGE_FILE_BYTES_REVERSED_HI, _T_BCE6D76F_BYTES_REVERSED_HI, _LEN_BCE6D76F }, // 14
		};
		BOOL bAdded = FALSE;
		for ( size_t J = 0; J < sizeof( aTable ) / sizeof( aTable[0] ); ++J ) {
			if ( _uiVal & aTable[J].ui32Type ) {
				if ( bAdded ) {
					_sString.append( " | " );
				}
				CHAR szBuffer[_T_MAX_LEN];
				if ( !Options.bShortenEnumNames ) {
					CStringDecoder::Decode( _T_LEN_92691F05_IMAGE_FILE_, szBuffer );
					_sString += szBuffer;
				}
				CStringDecoder::Decode( aTable[J].pcName, aTable[J].ui32StrLen, szBuffer );
				_sString.append( szBuffer );
				::ZeroMemory( szBuffer, _T_MAX_LEN );
				bAdded = TRUE;
				_uiVal = _uiVal & ~aTable[J].ui32Type;
			}
		}
		if ( !bAdded || _uiVal ) {
			if ( _sString.size() ) {
				_sString.append( " | " );
			}
			ToHex( _uiVal, _sString, 0 );
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
			{ MX_IMAGE_NT_OPTIONAL_HDR32_MAGIC, _T_86508279_NT_OPTIONAL_HDR32_MAGIC, _LEN_86508279 }, // 0
			{ MX_IMAGE_NT_OPTIONAL_HDR64_MAGIC, _T_AD60B927_NT_OPTIONAL_HDR64_MAGIC, _LEN_AD60B927 }, // 1
			{ MX_IMAGE_ROM_OPTIONAL_HDR_MAGIC, _T_B3FE7447_ROM_OPTIONAL_HDR_MAGIC, _LEN_B3FE7447 }, // 2

		};

		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ); ++I ) {
			if ( aTable[I].ui32Type == _uiType ) {
				std::string sString;
				CHAR szBuffer[_T_MAX_LEN];
				if ( !Options.bShortenEnumNames ) {
					CStringDecoder::Decode( _T_LEN_195135C7_IMAGE_, szBuffer );
					sString += szBuffer;
				}
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, szBuffer );
				sString += szBuffer;
				std::strncpy( _pcRet, sString.c_str(), std::min( sString.size() + 1, static_cast<size_t>(_T_MAX_LEN) ) );
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
			{ MX_IMAGE_SUBSYSTEM_UNKNOWN, _T_926359DD_UNKNOWN, _LEN_926359DD }, // 0
			{ MX_IMAGE_SUBSYSTEM_NATIVE, _T_3070DA31_NATIVE, _LEN_3070DA31 }, // 1
			{ MX_IMAGE_SUBSYSTEM_WINDOWS_GUI, _T_11261DF3_WINDOWS_GUI, _LEN_11261DF3 }, // 2
			{ MX_IMAGE_SUBSYSTEM_WINDOWS_CUI, _T_162FB52F_WINDOWS_CUI, _LEN_162FB52F }, // 3
			{ MX_IMAGE_SUBSYSTEM_OS2_CUI, _T_57AA3719_OS2_CUI, _LEN_57AA3719 }, // 4
			{ MX_IMAGE_SUBSYSTEM_POSIX_CUI, _T_7F91D555_POSIX_CUI, _LEN_7F91D555 }, // 5
			{ MX_IMAGE_SUBSYSTEM_NATIVE_WINDOWS, _T_443D2053_NATIVE_WINDOWS, _LEN_443D2053 }, // 6
			{ MX_IMAGE_SUBSYSTEM_WINDOWS_CE_GUI, _T_56B8EBDC_WINDOWS_CE_GUI, _LEN_56B8EBDC }, // 7
			{ MX_IMAGE_SUBSYSTEM_EFI_APPLICATION, _T_A1FCDAAD_EFI_APPLICATION, _LEN_A1FCDAAD }, // 8
			{ MX_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER, _T_41EE3B4A_EFI_BOOT_SERVICE_DRIVER, _LEN_41EE3B4A }, // 9
			{ MX_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER, _T_803115B6_EFI_RUNTIME_DRIVER, _LEN_803115B6 }, // 10
			{ MX_IMAGE_SUBSYSTEM_EFI_ROM, _T_8467DD24_EFI_ROM, _LEN_8467DD24 }, // 11
			{ MX_IMAGE_SUBSYSTEM_XBOX, _T_4EB22BF3_XBOX, _LEN_4EB22BF3 }, // 12
			{ MX_IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION, _T_F8272229_WINDOWS_BOOT_APPLICATION, _LEN_F8272229 }, // 13
		};

		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ); ++I ) {
			if ( aTable[I].ui32Type == _uiType ) {
				std::string sString;
				CHAR szBuffer[_T_MAX_LEN];
				if ( !Options.bShortenEnumNames ) {
					CStringDecoder::Decode( _T_LEN_13E57F2A_IMAGE_REL_BASED_, szBuffer );
					sString += szBuffer;
				}
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, szBuffer );
				sString += szBuffer;
				std::strncpy( _pcRet, sString.c_str(), std::min( sString.size() + 1, static_cast<size_t>(_T_MAX_LEN) ) );
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
			{ MX_IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA, _T_D1BE0763_HIGH_ENTROPY_VA, _LEN_D1BE0763 }, // 0
			{ MX_IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE, _T_85388291_DYNAMIC_BASE, _LEN_85388291 }, // 1
			{ MX_IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY, _T_1701D7FA_FORCE_INTEGRITY, _LEN_1701D7FA }, // 2
			{ MX_IMAGE_DLLCHARACTERISTICS_NX_COMPAT, _T_70CE4FB5_NX_COMPAT, _LEN_70CE4FB5 }, // 3
			{ MX_IMAGE_DLLCHARACTERISTICS_NO_ISOLATION, _T_745B7C8D_NO_ISOLATION, _LEN_745B7C8D }, // 4
			{ MX_IMAGE_DLLCHARACTERISTICS_NO_SEH, _T_56AEF8D8_NO_SEH, _LEN_56AEF8D8 }, // 5
			{ MX_IMAGE_DLLCHARACTERISTICS_NO_BIND, _T_9C08F002_NO_BIND, _LEN_9C08F002 }, // 6
			{ MX_IMAGE_DLLCHARACTERISTICS_APPCONTAINER, _T_74D080C6_APPCONTAINER, _LEN_74D080C6 }, // 7
			{ MX_IMAGE_DLLCHARACTERISTICS_WDM_DRIVER, _T_0C65F3D2_WDM_DRIVER, _LEN_0C65F3D2 }, // 8
			{ MX_IMAGE_DLLCHARACTERISTICS_GUARD_CF, _T_863DC96B_GUARD_CF, _LEN_863DC96B }, // 9
			{ MX_IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE, _T_217EB338_TERMINAL_SERVER_AWARE, _LEN_217EB338 }, // 10
		};
		BOOL bAdded = FALSE;
		for ( size_t J = 0; J < sizeof( aTable ) / sizeof( aTable[0] ); ++J ) {
			if ( _uiVal & aTable[J].ui32Type ) {
				if ( bAdded ) {
					_sString.append( " | " );
				}
				CHAR szBuffer[_T_MAX_LEN];
				if ( !Options.bShortenEnumNames ) {
					CStringDecoder::Decode( _T_LEN_E43E25C7_IMAGE_DLLCHARACTERISTICS_, szBuffer );
					_sString += szBuffer;
				}
				CStringDecoder::Decode( aTable[J].pcName, aTable[J].ui32StrLen, szBuffer );
				_sString.append( szBuffer );
				bAdded = TRUE;
				_uiVal = _uiVal & ~aTable[J].ui32Type;
			}
		}
		if ( !bAdded || _uiVal ) {
			if ( _sString.size() ) {
				_sString.append( " | " );
			}
			ToHex( _uiVal, _sString, 0 );
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
			{ MX_IMAGE_SCN_TYPE_REG, MX_IMAGE_SCN_TYPE_REG, _T_077EDBA6_TYPE_REG, _LEN_077EDBA6 }, // 0
			{ MX_IMAGE_SCN_TYPE_DSECT, MX_IMAGE_SCN_TYPE_DSECT, _T_86813296_TYPE_DSECT, _LEN_86813296 }, // 1
			{ MX_IMAGE_SCN_TYPE_NOLOAD, MX_IMAGE_SCN_TYPE_NOLOAD, _T_FD75D458_TYPE_NOLOAD, _LEN_FD75D458 }, // 2
			{ MX_IMAGE_SCN_TYPE_GROUP, MX_IMAGE_SCN_TYPE_GROUP, _T_6FFFDB3B_TYPE_GROUP, _LEN_6FFFDB3B }, // 3
			{ MX_IMAGE_SCN_TYPE_NO_PAD, MX_IMAGE_SCN_TYPE_NO_PAD, _T_A8A3C864_TYPE_NO_PAD, _LEN_A8A3C864 }, // 4
			{ MX_IMAGE_SCN_TYPE_COPY, MX_IMAGE_SCN_TYPE_COPY, _T_1FB64F61_TYPE_COPY, _LEN_1FB64F61 }, // 5
			{ MX_IMAGE_SCN_CNT_CODE, MX_IMAGE_SCN_CNT_CODE, _T_9B9C0488_CNT_CODE, _LEN_9B9C0488 }, // 6
			{ MX_IMAGE_SCN_CNT_INITIALIZED_DATA, MX_IMAGE_SCN_CNT_INITIALIZED_DATA, _T_A7750BF0_CNT_INITIALIZED_DATA, _LEN_A7750BF0 }, // 7
			{ MX_IMAGE_SCN_CNT_UNINITIALIZED_DATA, MX_IMAGE_SCN_CNT_UNINITIALIZED_DATA, _T_6E16BC60_CNT_UNINITIALIZED_DATA, _LEN_6E16BC60 }, // 8
			{ MX_IMAGE_SCN_LNK_OTHER, MX_IMAGE_SCN_LNK_OTHER, _T_B142AF4E_LNK_OTHER, _LEN_B142AF4E }, // 9
			{ MX_IMAGE_SCN_LNK_INFO, MX_IMAGE_SCN_LNK_INFO, _T_0CA0A7A5_LNK_INFO, _LEN_0CA0A7A5 }, // 10
			{ MX_IMAGE_SCN_TYPE_OVER, MX_IMAGE_SCN_TYPE_OVER, _T_E6F057BA_TYPE_OVER, _LEN_E6F057BA }, // 11
			{ MX_IMAGE_SCN_LNK_REMOVE, MX_IMAGE_SCN_LNK_REMOVE, _T_F98C6B3D_LNK_REMOVE, _LEN_F98C6B3D }, // 12
			{ MX_IMAGE_SCN_LNK_COMDAT, MX_IMAGE_SCN_LNK_COMDAT, _T_18D0E927_LNK_COMDAT, _LEN_18D0E927 }, // 13
			{ MX_IMAGE_SCN_MEM_PROTECTED, MX_IMAGE_SCN_MEM_PROTECTED, _T_C6639C69_MEM_PROTECTED, _LEN_C6639C69 }, // 14
			{ MX_IMAGE_SCN_NO_DEFER_SPEC_EXC, MX_IMAGE_SCN_NO_DEFER_SPEC_EXC, _T_FFC6CCF0_NO_DEFER_SPEC_EXC, _LEN_FFC6CCF0 }, // 15
			{ MX_IMAGE_SCN_GPREL, MX_IMAGE_SCN_GPREL, _T_7AA030C3_GPREL, _LEN_7AA030C3 }, // 16
			{ MX_IMAGE_SCN_MEM_FARDATA, MX_IMAGE_SCN_MEM_FARDATA, _T_B63DE830_MEM_FARDATA, _LEN_B63DE830 }, // 17
			{ MX_IMAGE_SCN_MEM_SYSHEAP, MX_IMAGE_SCN_MEM_SYSHEAP, _T_41E4239D_MEM_SYSHEAP, _LEN_41E4239D }, // 18
			{ MX_IMAGE_SCN_MEM_PURGEABLE, MX_IMAGE_SCN_MEM_PURGEABLE, _T_5B8A84DD_MEM_PURGEABLE, _LEN_5B8A84DD }, // 19
			{ MX_IMAGE_SCN_MEM_16BIT, MX_IMAGE_SCN_MEM_16BIT, _T_070F9CA8_MEM_16BIT, _LEN_070F9CA8 }, // 20
			{ MX_IMAGE_SCN_MEM_LOCKED, MX_IMAGE_SCN_MEM_LOCKED, _T_D02642F8_MEM_LOCKED, _LEN_D02642F8 }, // 21
			{ MX_IMAGE_SCN_MEM_PRELOAD, MX_IMAGE_SCN_MEM_PRELOAD, _T_6D446D1D_MEM_PRELOAD, _LEN_6D446D1D }, // 22
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_1BYTES, _T_725E1F96_ALIGN_1BYTES, _LEN_725E1F96 }, // 23
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_2BYTES, _T_F4CA6D38_ALIGN_2BYTES, _LEN_F4CA6D38 }, // 24
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_4BYTES, _T_22938E25_ALIGN_4BYTES, _LEN_22938E25 }, // 25
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_8BYTES, _T_55514E5E_ALIGN_8BYTES, _LEN_55514E5E }, // 26
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_16BYTES, _T_4DEBCAA4_ALIGN_16BYTES, _LEN_4DEBCAA4 }, // 27
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_32BYTES, _T_41E5999B_ALIGN_32BYTES, _LEN_41E5999B }, // 28
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_64BYTES, _T_C5845521_ALIGN_64BYTES, _LEN_C5845521 }, // 29
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_128BYTES, _T_4B466B92_ALIGN_128BYTES, _LEN_4B466B92 }, // 30
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_256BYTES, _T_3A64338F_ALIGN_256BYTES, _LEN_3A64338F }, // 31
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_512BYTES, _T_5F7F5C93_ALIGN_512BYTES, _LEN_5F7F5C93 }, // 32
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_1024BYTES, _T_E122EFF1_ALIGN_1024BYTES, _LEN_E122EFF1 }, // 33
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_2048BYTES, _T_CCBD2675_ALIGN_2048BYTES, _LEN_CCBD2675 }, // 34
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_4096BYTES, _T_E47A5F4F_ALIGN_4096BYTES, _LEN_E47A5F4F }, // 35
			{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_8192BYTES, _T_5575EED3_ALIGN_8192BYTES, _LEN_5575EED3 }, // 36
			//{ MX_IMAGE_SCN_ALIGN_MASK, MX_IMAGE_SCN_ALIGN_MASK, _T_FA85A454_ALIGN_MASK, _LEN_FA85A454 }, // 37
			{ MX_IMAGE_SCN_LNK_NRELOC_OVFL, MX_IMAGE_SCN_LNK_NRELOC_OVFL, _T_E67FB715_LNK_NRELOC_OVFL, _LEN_E67FB715 }, // 38
			{ MX_IMAGE_SCN_MEM_DISCARDABLE, MX_IMAGE_SCN_MEM_DISCARDABLE, _T_7C04A340_MEM_DISCARDABLE, _LEN_7C04A340 }, // 39
			{ MX_IMAGE_SCN_MEM_NOT_CACHED, MX_IMAGE_SCN_MEM_NOT_CACHED, _T_A0B1C0D7_MEM_NOT_CACHED, _LEN_A0B1C0D7 }, // 40
			{ MX_IMAGE_SCN_MEM_NOT_PAGED, MX_IMAGE_SCN_MEM_NOT_PAGED, _T_56A8D4D7_MEM_NOT_PAGED, _LEN_56A8D4D7 }, // 41
			{ MX_IMAGE_SCN_MEM_SHARED, MX_IMAGE_SCN_MEM_SHARED, _T_29787ED2_MEM_SHARED, _LEN_29787ED2 }, // 42
			{ MX_IMAGE_SCN_MEM_EXECUTE, MX_IMAGE_SCN_MEM_EXECUTE, _T_F0591D60_MEM_EXECUTE, _LEN_F0591D60 }, // 43
			{ MX_IMAGE_SCN_MEM_READ, MX_IMAGE_SCN_MEM_READ, _T_478A60EC_MEM_READ, _LEN_478A60EC }, // 44
			{ MX_IMAGE_SCN_MEM_WRITE, MX_IMAGE_SCN_MEM_WRITE, _T_3CB0DD1E_MEM_WRITE, _LEN_3CB0DD1E }, // 45
		};

		BOOL bAdded = FALSE;
		for ( size_t J = 1; J < sizeof( aTable ) / sizeof( aTable[0] ); ++J ) {
			if ( (_uiVal & aTable[J].ui32Mask) == aTable[J].ui32Type ) {
				size_t stIdx = J;
				if ( bAdded ) {
					_sString.append( " | " );
				}
				else if ( !_uiVal ) {
					// _uiVal has not been modified and is 0, so it started as 0 (MX_IMAGE_SCN_TYPE_REG).
					stIdx = 0;
				}
				CHAR szBuffer[_T_MAX_LEN];
				if ( !Options.bShortenEnumNames ) {
					CStringDecoder::Decode( _T_LEN_11FF5736_IMAGE_SCN_, szBuffer );
					_sString += szBuffer;
				}
				CStringDecoder::Decode( aTable[stIdx].pcName, aTable[stIdx].ui32StrLen, szBuffer );
				_sString.append( szBuffer );
				if ( !stIdx ) { return _sString.c_str(); }
				bAdded = TRUE;
				_uiVal = _uiVal & ~aTable[J].ui32Type;
			}
		}
		if ( !bAdded || _uiVal ) {
			if ( _sString.size() ) {
				_sString.append( " | " );
			}
			ToHex( _uiVal, _sString, 0 );
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
					CStringDecoder::Decode( _T_LEN_13E57F2A_IMAGE_REL_BASED_, szBuffer );
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
					CStringDecoder::Decode( _T_LEN_CD613405_PROCESS_, szBuffer );
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

	// Creates an SW_ string.
	const CHAR * CUtilities::ShowWindowToString( int _iVal, std::string &_sString, BOOL _bShort ) {
		_bShort = (_bShort == -1) ? Options.bShortenEnumNames : _bShort;
		struct {
			int iType;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ SW_HIDE, _T_LEN_920C269F_HIDE },
			{ SW_NORMAL, _T_LEN_D17DAF62_NORMAL },
			{ SW_SHOWMINIMIZED, _T_LEN_6DA4DADD_SHOWMINIMIZED },
			{ SW_SHOWMAXIMIZED, _T_LEN_BA4F689C_SHOWMAXIMIZED },
			{ SW_SHOWNOACTIVATE, _T_LEN_EA02892C_SHOWNOACTIVATE },
			{ SW_SHOW, _T_LEN_049B34B5_SHOW },
			{ SW_MINIMIZE, _T_LEN_364F5FB5_MINIMIZE },
			{ SW_SHOWMINNOACTIVE, _T_LEN_059A7C09_SHOWMINNOACTIVE },
			{ SW_SHOWNA, _T_LEN_96FEABA1_SHOWNA },
			{ SW_RESTORE, _T_LEN_B892D1E5_RESTORE },
			{ SW_SHOWDEFAULT, _T_LEN_18095A2F_SHOWDEFAULT },
			{ SW_FORCEMINIMIZE, _T_LEN_C2DC3EA7_FORCEMINIMIZE },
		};
		for ( size_t I = 0; I < MX_ELEMENTS( aTable ); ++I ) {
			if ( _iVal == aTable[I].iType ) {
				CHAR szBuffer[_T_MAX_LEN];
				if ( !_bShort ) {
					CStringDecoder::Decode( _T_LEN_9F95CE0D_SW_, szBuffer );
					_sString += szBuffer;
				}
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, szBuffer );
				_sString += szBuffer;
				break;
			}
		}
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
					std::sprintf( szNumber, "%I64u ", _uiSize / aTable[I].uiSize );
				}
				else {
					std::sprintf( szNumber, aTable[I].pcFormat, static_cast<DOUBLE>(_uiSize) / static_cast<DOUBLE>(aTable[I].uiSize) );
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
		std::sprintf( szNumber, "%I64u ", _uiSize );
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
			std::strftime( szTemp, sizeof( szTemp ),"%d-%m-%Y %I:%M:%S %Z", ptTimeInfo );
			std::snprintf( _pcRet, _sLen, "%s (%I64u)", szTemp, _uiTime );
		}
		else {
			std::snprintf( _pcRet, _sLen, "%I64u", _uiTime );
		}
		if ( _sLen ) {
			_pcRet[_sLen-1] = '\0';
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
			std::strftime( szTemp, sizeof( szTemp ),"%d-%m-%Y %I:%M:%S %Z", ptTimeInfo );
			std::snprintf( szTemp2, MX_ELEMENTS( szTemp2 ), "%s (%I64u)", szTemp, _uiTime );
			szTemp2[MX_ELEMENTS(szTemp2)-1] = '\0';
			_sString = szTemp2;
		}
		else {
			CHAR szTemp2[128];
			std::snprintf( szTemp2, MX_ELEMENTS( szTemp2 ), "%I64u", _uiTime );
			_sString = szTemp2;
		}
		return _sString.c_str();
	}

	// Creates a hexadecimal string.  Returns the internal buffer, which means the result must be copied as it will be overwritten when the next function that uses the internal buffer is called.
	const CHAR * CUtilities::ToHex( uint64_t _uiValue, uint32_t _uiNumDigits, bool _bIncludePrefix ) {
		return ToHex( _uiValue, m_szTemp, MX_ELEMENTS( m_szTemp ), _uiNumDigits, _bIncludePrefix );
	}
	
	// Creates a hexadecimal string.
	const CHAR * CUtilities::ToHex( uint64_t _uiValue, CHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits, bool _bIncludePrefix ) {
		_uiNumDigits = std::max( _uiNumDigits, 1U );
		CHAR szFormat[32];
		if ( _bIncludePrefix ) {
			if ( Options.bUse0xForHex ) {
				std::snprintf( szFormat, MX_ELEMENTS( szFormat ), "0x%%.%uI64X", _uiNumDigits );
			}
			else {
				std::snprintf( szFormat, MX_ELEMENTS( szFormat ), "%%.%uI64Xh", _uiNumDigits );
			}
		}
		else {
			std::snprintf( szFormat, MX_ELEMENTS( szFormat ), "%%.%uI64X", _uiNumDigits );
		}
		std::snprintf( _pcRet, _sLen, szFormat, _uiValue );
		return _pcRet;
	}

	// Creates a hexadecimal string.
	const CHAR * CUtilities::ToHex( uint64_t _uiValue, std::string &_sString, uint32_t _uiNumDigits, bool _bIncludePrefix ) {
		CHAR szTemp[32];
		ToHex( _uiValue, szTemp, MX_ELEMENTS( szTemp ), _uiNumDigits );

		_sString += szTemp;
		return _sString.c_str();
	}

	// Creates a hexadecimal string.
	const WCHAR * CUtilities::ToHex( uint64_t _uiValue, WCHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits, bool _bIncludePrefix ) {
		_uiNumDigits = std::max( _uiNumDigits, 1U );
		WCHAR szFormat[32];
		if ( _bIncludePrefix ) {
			if ( Options.bUse0xForHex ) {
				std::swprintf( szFormat, MX_ELEMENTS( szFormat ), L"0x%%.%uI64X", _uiNumDigits );
			}
			else {
				std::swprintf( szFormat, MX_ELEMENTS( szFormat ), L"%%.%uI64Xh", _uiNumDigits );
			}
		}
		else {
			std::swprintf( szFormat, MX_ELEMENTS( szFormat ), L"%%.%uI64X", _uiNumDigits );
		}
		std::swprintf( _pcRet, _sLen, szFormat, _uiValue );
		return _pcRet;
	}

	// Creates a hexadecimal string.
	const WCHAR * CUtilities::ToHex( uint64_t _uiValue, std::wstring &_sString, uint32_t _uiNumDigits, bool _bIncludePrefix ) {
		WCHAR szTemp[32];
		ToHex( _uiValue, szTemp, MX_ELEMENTS( szTemp ), _uiNumDigits, _bIncludePrefix );

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

	// Creates an octadecimal (\0[0-7]{1-22}) string from an integer value.
	const CHAR * CUtilities::ToOct( uint64_t _uiValue, std::string &_sString, uint32_t _uiNumDigits ) {
		_uiNumDigits = EE_MAX_( _uiNumDigits, 1 );
		// Determine max number of bits set in the number.
		uint64_t uiCopy = _uiValue;
		uint32_t uiTotalDigits = 0;
		while ( uiCopy ) {
			++uiTotalDigits;
			uiCopy >>= 3;
		}

		_uiNumDigits = EE_MAX_( _uiNumDigits, uiTotalDigits );
		_sString.push_back( '0' );
		if ( Options.bUse0oForOct ) {
			_sString.push_back( 'o' );
		}
		for ( uint64_t I = _uiNumDigits; I--; ) {
			_sString.push_back( ((_uiValue >> (I * 3)) & 0x7) + '0' );
		}
		return _sString.c_str();
	}

	// Creates an octadecimal (\0[0-7]{1-22}) string from an integer value.
	const WCHAR * CUtilities::ToOct( uint64_t _uiValue, std::wstring &_sString, uint32_t _uiNumDigits ) {
		_uiNumDigits = EE_MAX_( _uiNumDigits, 1 );
		// Determine max number of bits set in the number.
		uint64_t uiCopy = _uiValue;
		uint32_t uiTotalDigits = 0;
		while ( uiCopy ) {
			++uiTotalDigits;
			uiCopy >>= 3;
		}

		_uiNumDigits = EE_MAX_( _uiNumDigits, uiTotalDigits );
		_sString.push_back( L'0' );
		if ( Options.bUse0oForOct ) {
			_sString.push_back( 'o' );
		}
		for ( uint64_t I = _uiNumDigits; I--; ) {
			_sString.push_back( ((_uiValue >> (I * 3)) & 0x7) + L'0' );
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
		std::snprintf( szFormat, MX_ELEMENTS( szFormat ), "%%.%uI64u", _uiNumDigits );
		std::snprintf( _pcRet, _sLen, szFormat, _uiValue );
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
		std::swprintf( szFormat, MX_ELEMENTS( szFormat ), L"%%.%uI64u", _uiNumDigits );
		std::swprintf( _pcRet, _sLen, szFormat, _uiValue );
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
		std::snprintf( szFormat, MX_ELEMENTS( szFormat ), "%%.%uI64d", _uiNumDigits );
		std::snprintf( _pcRet, _sLen, szFormat, _iValue );
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
		std::swprintf( szFormat, MX_ELEMENTS( szFormat ), L"%%.%uI64d", _uiNumDigits );
		std::swprintf( _pcRet, _sLen, szFormat, _iValue );
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
			iLen = std::snprintf( pcBuffer, sLen, "%.2000f", _dValue );
		}
		else if ( _iSigDigits < 0 ) {
			CHAR szFormat[32];
			std::snprintf( szFormat, MX_ELEMENTS( szFormat ), "%%.%ug", -_iSigDigits );
			iLen = std::snprintf( pcBuffer, sLen, szFormat, _dValue );
		}
		else {
			CHAR szFormat[32];
			std::snprintf( szFormat, MX_ELEMENTS( szFormat ), "%%.%ue", _iSigDigits );
			iLen = std::snprintf( pcBuffer, sLen, szFormat, _dValue );
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
			iLen = std::swprintf( pcBuffer, sLen, L"%.2000f", _dValue );
		}
		else if ( _iSigDigits < 0 ) {
			WCHAR szFormat[32];
			std::swprintf( szFormat, MX_ELEMENTS( szFormat ), L"%%.%ug", -_iSigDigits );
			iLen = std::swprintf( pcBuffer, sLen, szFormat, _dValue );
		}
		else {
			WCHAR szFormat[32];
			std::swprintf( szFormat, MX_ELEMENTS( szFormat ), L"%%.%ue", _iSigDigits );
			iLen = std::swprintf( pcBuffer, sLen, szFormat, _dValue );
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

	// Creates a string with the given data interpreted as a given type.
	const WCHAR * CUtilities::ToDataTypeString( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtType, std::wstring &_sString,
		bool _bMustPrintNumber ) {
		ee::CExpEvalContainer::EE_RESULT eCopy;
		//ee::CExpEvalContainer::ConvertResult
		switch ( _dtType ) {
			case MX_DT_FLOAT16 : {
				eCopy = DetailedConvertResult( _rRes, _dtType );
				if ( _bMustPrintNumber && std::isinf( ee::CFloat16( eCopy.u.dVal ).Value() ) ) {
					_sString += ee::CFloat16( eCopy.u.dVal ).Value() > 0.0 ? _DEC_WS_90D8D134_as_float16_0x7C00_ :
						_DEC_WS_3C64313D__as_float16_0x7C00_;
				}
				else if ( _bMustPrintNumber && std::isnan( ee::CFloat16( eCopy.u.dVal ).Value() ) ) {
					_sString += _DEC_WS_B5B38EE8_as_float16_0x7E00_;
				}
				else {
					CUtilities::ToDouble( ee::CFloat16( eCopy.u.dVal ).Value(), _sString );
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

	// Creates a string with the given data interpreted as a given type.
	const WCHAR * CUtilities::ToDataTypeString( const uint8_t * _pui8Value, CUtilities::MX_DATA_TYPES _dtType, std::wstring &_sString,
		bool _bMustPrintNumber ) {
		ee::CExpEvalContainer::EE_RESULT eVal;
		switch ( _dtType ) {
#define MX_CONV( CASE, EE_TYPE, VAL, TYPE )												\
	case CASE : {																		\
		eVal.ncType = ee::EE_TYPE;														\
		eVal.u.VAL = (*reinterpret_cast<const TYPE *>(_pui8Value));						\
		break;																			\
	}
			MX_CONV( MX_DT_INT8, EE_NC_SIGNED, i64Val, int8_t )
			MX_CONV( MX_DT_INT16, EE_NC_SIGNED, i64Val, int16_t )
			MX_CONV( MX_DT_INT32, EE_NC_SIGNED, i64Val, int32_t )
			MX_CONV( MX_DT_INT64, EE_NC_SIGNED, i64Val, int64_t )
			MX_CONV( MX_DT_UINT8, EE_NC_UNSIGNED, ui64Val, uint8_t )
			MX_CONV( MX_DT_UINT16, EE_NC_UNSIGNED, ui64Val, uint16_t )
			MX_CONV( MX_DT_UINT32, EE_NC_UNSIGNED, ui64Val, uint32_t )
			MX_CONV( MX_DT_UINT64, EE_NC_UNSIGNED, ui64Val, uint64_t )
			MX_CONV( MX_DT_FLOAT, EE_NC_FLOATING, dVal, float )
			MX_CONV( MX_DT_DOUBLE, EE_NC_FLOATING, dVal, double )
			case MX_DT_FLOAT16 : {
				eVal.ncType = ee::EE_NC_FLOATING;
				eVal.u.dVal = ee::CFloat16( _pui8Value ).Value();
				break;
			}
			default : {
				return _sString.c_str();
			}
		}
		return ToDataTypeString( eVal, _dtType, _sString, _bMustPrintNumber );
#undef MX_CONV
	}

	// Returns -1 if the given result cast to the given type is -inf, 1 if it is +inf, otherwise 0.
	int32_t CUtilities::DataTypeIsInf( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtType ) {
		ee::CExpEvalContainer::EE_RESULT eCopy;
		switch ( _dtType ) {
			case MX_DT_FLOAT16 : {
				eCopy = ee::CExpEvalContainer::ConvertResult( _rRes, ee::EE_NC_FLOATING );
				if ( std::isinf( ee::CFloat16( eCopy.u.dVal ).Value() ) ) {
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
				return std::isnan( ee::CFloat16( eCopy.u.dVal ).Value() );
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
					return _dtType.u.Float64 == ee::CFloat16::Max();
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
	const CHAR * CUtilities::PrintDataType( std::string &_sString, CUtilities::MX_DATA_TYPES _dtType, DWORD _dwOptions, bool _bClearStr ) {
		if ( _dwOptions == DWINVALID ) {
			_dwOptions = Options.dwDataTypeOptions;
		}
		if ( _bClearStr ) {
			_sString.clear();
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
	const WCHAR * CUtilities::PrintDataType( std::wstring &_sString, CUtilities::MX_DATA_TYPES _dtType, DWORD _dwOptions, bool _bClearStr ) {
		if ( _dwOptions == DWINVALID ) {
			_dwOptions = Options.dwDataTypeOptions;
		}
		if ( _bClearStr ) {
			_sString.clear();
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
				ToDouble( ee::CFloat16( _pdtData->u.UInt16 ).Value(), _sRet, _iSigDigits );
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
				int iLen = std::snprintf( szBuffer, MX_ELEMENTS( szBuffer ), "%.11I64X", _pdtData->u.Pointer64 );
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
				ToDouble( ee::CFloat16( _pdtData->u.UInt16 ).Value(), _swsRet, _iSigDigits );
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
				int iLen = std::swprintf( szBuffer, MX_ELEMENTS( szBuffer ), L"%.11I64X", _pdtData->u.Pointer64 );
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
		MX_CASE( MX_DT_FLOAT16, ee::CFloat16, double, MEMBER, double, dVal )								\
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

	// MX_SUB_EVAL_TYPES value to a string.
	const WCHAR * CUtilities::SubEvaluationTypeToString( CUtilities::MX_SUB_EVAL_TYPES _setType, std::wstring &_sString ) {
		static const struct {
			const char *				pcName;
			size_t						sLen;
			uint32_t					ui32Id;
		} aData[] = {
			{ _T_LEN_396582B1_Exact_Value,			MX_SET_EXACT },
			{ _T_LEN_82405865_Not_Equal_To,			MX_SET_NOT_EQUAL_TO },
			{ _T_LEN_7B476D38_Increased,			MX_SET_INCREASED },
			{ _T_LEN_D7629639_Decreased,			MX_SET_DECREASED },
			{ _T_LEN_1BB1B671_In_Range,				MX_SET_RANGE },
			{ _T_LEN_474E42C3_Greater_Than,			MX_SET_GREATER_THAN },
			{ _T_LEN_C8A1ADFC_Lower_Than,			MX_SET_LESS_THAN },
			{ _T_LEN_49B80325_Same_as_Before,		MX_SET_SAME_AS_BEFORE },
			{ _T_LEN_C74C8767_Different_from_Before,MX_SET_DIFFERENT_FROM_BEFORE },
			{ _T_LEN_95C86580_Changed_By,			MX_SET_CHANGED_BY },
			{ _T_LEN_3D6AAA8C_Changed_By____,		MX_SET_CHANGED_BY_PERCENT },
			{ _T_LEN_27A3B5FF_Changed_By__Range_,	MX_SET_CHANGED_BY_RANGE },
			{ _T_LEN_CC66FC29_Changed_By__Range___,	MX_SET_CHANGED_BY_RANGE_PERCENT },
			{ _T_LEN_7BD6DC75_Same_as_Original,		MX_SET_SAME_AS_ORIGINAL },
			{ _T_LEN_8558CA08_Quick_Expression,		MX_SET_QUICK_EXP },
		};

		for ( size_t I = MX_ELEMENTS( aData ); I--; ) {
			if ( aData[I].ui32Id == _setType ) {
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
				dtRet.u.Float64 = ee::CFloat16( eCopy.u.dVal ).Value();
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

	// Converts from UTF-8 to any single-byte code page.
	CSecureString CUtilities::ToCodePage( const std::string &_sIn, UINT _uiCodePage, bool * _pbError ) {
		if ( _pbError ) { (*_pbError) = false; }
		CSecureWString swsUtf16 = ee::CExpEval::StringToWString( _sIn );
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
		return ee::CExpEval::WStringToString( swsTmp );
	}

	// Maps a UTF-16 (wide character) string to a new character string. The new character string is not necessarily from a multibyte character set.  Call within a try/catch block.
	CSecureString CUtilities::WideCharToMultiByte( UINT _uiCodePage,
		DWORD _dwFlags,
		const std::wstring &_wsString,
		LPCCH _lpDefaultChar,
		LPBOOL _lpUsedDefaultChar,
		DWORD * _pdwLastError ) {
		if ( _pdwLastError ) { (*_pdwLastError) = ERROR_SUCCESS; }
		if ( !_wsString.size() ) { return CSecureString(); }
		
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
					uint32_t ui32This = ee::CExpEval::NextUtf16Char( &_wsString[I], _wsString.size() - I, &sLen );
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
					uint32_t ui32This = ee::CExpEval::NextUtf16Char( &_wsString[I], _wsString.size() - I, &sLen );
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
		if ( !_sString.size() ) { return CSecureWString(); }

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
					uint32_t ui32This = ee::CExpEval::Utf32ToUtf16( (*_puiSrc++), uiLen );

					if ( ui32This == MX_UTF_INVALID ) {
						if ( _dwFlags & MB_ERR_INVALID_CHARS ) {
							if ( _pdwLastError ) { (*_pdwLastError) = ERROR_NO_UNICODE_TRANSLATION; }
							return CSecureWString();
						}
						ui32This = ee::CExpEval::Utf32ToUtf16( 0xFFFD, uiLen );
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
					uint32_t ui32This = ee::CExpEval::Utf32ToUtf16( ::_byteswap_ulong( (*_puiSrc++) ), uiLen );

					if ( ui32This == MX_UTF_INVALID ) {
						if ( _dwFlags & MB_ERR_INVALID_CHARS ) {
							if ( _pdwLastError ) { (*_pdwLastError) = ERROR_NO_UNICODE_TRANSLATION; }
							return CSecureWString();
						}
						ui32This = ee::CExpEval::Utf32ToUtf16( 0xFFFD, uiLen );
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
			uint32_t ui32This = ee::CExpEval::NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
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
					std::sprintf( szBuffer, "\\u%.4X", static_cast<uint16_t>(_swsInput[I]) );
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
		CSecureWString swsConv = ee::CExpEval::Utf32StringToWString( _swsInput.data(), _swsInput.size() );
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
			uint32_t ui32This = ee::CExpEval::NextUtf32Char( &_swsInput[I], _swsInput.size() - I, &sLen );
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
				uint32_t ui32Conv = ee::CExpEval::Utf32ToUtf16( ui32This, ui32ThisLen );
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
					std::sprintf( szBuffer, "\\U%.8X", static_cast<uint32_t>(_swsInput[I]) );
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
			std::sprintf( szBuffer, "%.2X", static_cast<uint8_t>(_ssInput[I]) );
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

			uint32_t ui32This = ee::CExpEval::NextUtf8Char( &_ssInput[I], _ssInput.size() - I, &sLen );
			if ( sLen == 1 ) {
				// A single character can be used for all formats.
				ssTmp.push_back( '\\' );
				ssTmp.push_back( 'x' );
				char szBuffer[8];
				std::sprintf( szBuffer, "%.2X", static_cast<uint8_t>(_ssInput[I]) );
				ssTmp.push_back( szBuffer[0] );
				ssTmp.push_back( szBuffer[1] );
				if ( _bKeepNewline && _ssInput[I] == '\n' ) {
					ssTmp.push_back( '\r' );
					ssTmp.push_back( '\n' );
				}
				continue;
			}
			uint32_t ui32ThisLen = 0;
			uint32_t ui32ThisUtf16 = ee::CExpEval::Utf32ToUtf16( ui32This, ui32ThisLen );
			if ( ui32ThisLen == 1 ) {
				// To preserve the UTF data, a \uNNNN must be used.
				ssTmp.push_back( '\\' );
				ssTmp.push_back( 'u' );
				char szBuffer[8];
				std::sprintf( szBuffer, "%.4X", static_cast<uint16_t>(ui32ThisUtf16) );
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
			std::sprintf( szBuffer, "%.8X", ui32This );
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
			
			char szBuffer[8];
			if ( _swsInput[I] < 128 ) {
				// This works because we know that this can't be followed by a number, as it will only be followed
				//	by the end of the string or \.
				swsTmp.push_back( 'x' );
				std::sprintf( szBuffer, "%.2X", static_cast<uint8_t>(_swsInput[I]) );
			}
			else {
				swsTmp.push_back( 'u' );
				std::sprintf( szBuffer, "%.4X", static_cast<uint16_t>(_swsInput[I]) );
			}
			swsTmp.push_back( szBuffer[0] );
			swsTmp.push_back( szBuffer[1] );
			if ( _swsInput[I] >= 128 ) {
				swsTmp.push_back( szBuffer[2] );
				swsTmp.push_back( szBuffer[3] );
			}
			
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
			char szBuffer[16];
			if ( _swsInput[I] < 128 ) {
				// This works because we know that this can't be followed by a number, as it will only be followed
				//	by the end of the string or \.
				swsTmp.push_back( 'x' );
				std::sprintf( szBuffer, "%.2X", static_cast<uint8_t>(_swsInput[I]) );
			}
			else {
				swsTmp.push_back( 'U' );
				std::sprintf( szBuffer, "%.8X", static_cast<uint32_t>(_swsInput[I]) );
			}
			swsTmp.push_back( szBuffer[0] );
			swsTmp.push_back( szBuffer[1] );
			if ( _swsInput[I] >= 128 ) {
				swsTmp.push_back( szBuffer[2] );
				swsTmp.push_back( szBuffer[3] );
				swsTmp.push_back( szBuffer[4] );
				swsTmp.push_back( szBuffer[5] );
				swsTmp.push_back( szBuffer[6] );
				swsTmp.push_back( szBuffer[7] );
			}
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
			char cSrc;
			char cDst;
		} const sTable[] = {
			{ '\'', '\'' },
			{ '"', '"' },
			{ '\\', '\\' },
			{ '\a', 'a' },
			{ '\b', 'b' },
			{ '\f', 'f' },
			{ '\n', 'n' },
			{ '\r', 'r' },
			{ '\t', 't' },
			{ '\v', 'v' },
		};
		CSecureString vOutput;
		for ( size_t I = 0; I < _ssInput.size();  ) {
			uint32_t ui32This = ee::CExpEval::NextUtf8Char( &_ssInput[I], _ssInput.size() - I, &sLen );
			if ( sLen == 1 && MX_UTF_INVALID != ui32This ) {
				bool bFound = false;
				for ( size_t J = 0; J < MX_ELEMENTS( sTable ) && !bFound; ++J ) {
					if ( _ssInput[I] == sTable[J].cSrc ) {
						vOutput.push_back( '\\' );
						vOutput.push_back( sTable[J].cDst );
						bFound = true;
						if ( _bKeepNewline && _ssInput[I] == '\n' ) {
							vOutput.push_back( '\r' );
							vOutput.push_back( '\n' );
						}
						continue;
					}
				}
				if ( bFound ) { ++I; continue; }
			}
			uint32_t ui32ThisLen;
			uint32_t ui32Conv = ee::CExpEval::Utf32ToUtf16( ui32This, ui32ThisLen );
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
				if ( ui32ThisLen == 1 ) {
					std::sprintf( szBuffer, "\\u%.4X", static_cast<uint16_t>(ui32Conv) );
				}
				else {
					std::sprintf( szBuffer, "\\U%.8X", ui32This );
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
			uint32_t ui32This = ee::CExpEval::NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
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
			}
			uint32_t ui32ThisLen;
			uint32_t ui32Conv = ee::CExpEval::Utf32ToUtf16( ui32This, ui32ThisLen );
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
				if ( ui32ThisLen == 1 ) {
					std::sprintf( szBuffer, "\\u%.4X", static_cast<uint16_t>(_swsInput[I]) );
				}
				else {
					std::sprintf( szBuffer, "\\U%.8X", ui32This );
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
			uint32_t ui32This = ee::CExpEval::NextUtf32Char( &_swsInput[I], _swsInput.size() - I, &sLen );
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
			}
				
			uint32_t ui32ThisLen;
			uint32_t ui32Conv = ee::CExpEval::Utf32ToUtf16( ui32This, ui32ThisLen );
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
				std::sprintf( szBuffer, "\\U%.8X", static_cast<uint32_t>(_swsInput[I]) );
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

	// Escapes standard characters in an ASCII string.
	CSecureString CUtilities::EscapeStandardAscii( const CSecureString &_ssInput, bool _bKeepNewline ) {
		size_t sLen;
		
		static struct {
			char cSrc;
			char cDst;
		} const sTable[] = {
			{ '\'', '\'' },
			{ '"', '"' },
			{ '\\', '\\' },
			{ '\a', 'a' },
			{ '\b', 'b' },
			{ '\f', 'f' },
			{ '\n', 'n' },
			{ '\r', 'r' },
			{ '\t', 't' },
			{ '\v', 'v' },
		};
		CSecureString vOutput;
		for ( size_t I = 0; I < _ssInput.size(); ++I ) {
			bool bFound = false;
			for ( size_t J = 0; J < MX_ELEMENTS( sTable ); ++J ) {
				if ( _ssInput[I] == sTable[J].cSrc ) {
					bFound = true;
					vOutput.push_back( '\\' );
					vOutput.push_back( sTable[J].cDst );
					if ( _bKeepNewline && _ssInput[I] == '\n' ) {
						vOutput.push_back( '\r' );
						vOutput.push_back( '\n' );
					}
					break;
				}
			}
			if ( !bFound ) {
				vOutput.push_back( _ssInput[I] );
			}
		}
		return vOutput;
	}

	// Escapes all non-JSON string characters.
	CSecureWString CUtilities::EscapeNonJson( const CSecureWString &_swsInput, bool _bKeepNewline ) {
		size_t sLen;
		
		static struct {
			wchar_t wcSrc;
			wchar_t wcDst;
		} const sTable[] = {
			{ L'"', L'"' },
			{ L'\\', L'\\' },
			{ L'\b', L'b' },
			{ L'\f', L'f' },
			{ L'\n', L'n' },
			{ L'\r', L'r' },
			{ L'\t', L't' },
			{ L'/', L'/' },
		};
		CSecureWString swsOut;

		for ( size_t I = 0; I < _swsInput.size();  ) {
			uint32_t ui32This = ee::CExpEval::NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
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
			}

			uint32_t ui32ThisLen;
			uint32_t ui32Conv = ee::CExpEval::Utf32ToUtf16( ui32This, ui32ThisLen );
			if ( ui32Conv >= L' ' && ui32Conv <= L'z' ) {
				swsOut.push_back( wchar_t( ui32Conv ) );
				++I;
				continue;
			}

			char szBuffer[16];
			if ( ui32ThisLen == 1 ) {
				std::sprintf( szBuffer, "\\u%.4X", static_cast<uint16_t>(_swsInput[I]) );
			}
			else {
				std::sprintf( szBuffer, "\\U%.8X", ui32This );
			}
			char * pcTmp = szBuffer;
			while ( (*pcTmp) ) {
				swsOut.push_back( (*pcTmp++) );
			}

			++I;
		}
		return swsOut;
	}

	// Creates a string from a string in the form of an array of hex bytes.
	CSecureString CUtilities::FromHexString( const CSecureWString &_swsInput ) {
		CSecureString ssTmp;
		CSecureString ssBuffer;
		for ( size_t I = 0; I < _swsInput.size(); ++I ) {
			if ( ee::CExpEval::ValidHex( _swsInput[I] ) ) {
				ssBuffer.push_back( _swsInput[I] );
			}
			else {
				if ( ssBuffer.size() ) {
					size_t sEaten = 0;
					for ( size_t J = 0; J < ssBuffer.size(); J += sEaten ) {
						uint64_t uiVal = ee::CExpEval::StoULL( &ssBuffer[J], 16, &sEaten, 0xFF );
						ssTmp.push_back( static_cast<uint8_t>(uiVal) );
					}
					ssBuffer.clear();
				}
			}
		}
		if ( ssBuffer.size() ) {
			size_t sEaten = 0;
			for ( size_t J = 0; J < ssBuffer.size(); J += sEaten ) {
				uint64_t uiVal = ee::CExpEval::StoULL( &ssBuffer[J], 16, &sEaten, 0xFF );
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
				uint32_t ui32This = ee::CExpEval::NextUtf8Char( &pcSrc[I], _sIn.size() - I, &sThisSize );
				uint32_t uiTempLen;
				if ( ui32This != MX_UTF_INVALID && std::iswspace( static_cast<wint_t>(ee::CExpEval::Utf32ToUtf16( ui32This, uiTempLen )) ) ) {
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
		while ( _sIn.size() && ee::CExpEval::IsWhiteSpace( _sIn.c_str()[_sIn.size()-1] ) ) { _sIn.pop_back(); }
		while ( _sIn.size() && ee::CExpEval::IsWhiteSpace( _sIn.c_str()[0] ) ) { _sIn.erase( _sIn.begin() ); }

		return _sIn;
	}

	// Upper-cases an ASCII string.
	std::string CUtilities::ToUpper( const std::string &_sInput ) {
		std::string sRet;
		for ( size_t I = 0; I < _sInput.size(); ++I ) {
			sRet.push_back( std::toupper( _sInput[I] ) );
		}
		return sRet;
	}

	// Lower-cases an ASCII string.
	std::string CUtilities::ToLower( const std::string &_sInput ) {
		std::string sRet;
		for ( size_t I = 0; I < _sInput.size(); ++I ) {
			sRet.push_back( std::tolower( _sInput[I] ) );
		}
		return sRet;
	}

	// Upper-cases a string.
	CSecureWString CUtilities::ToUpper( const CSecureWString &_swsInput ) {
		CSecureWString swsRet;
		size_t sLen = 0;
		for ( size_t I = 0; I < _swsInput.size(); I += sLen ) {
			uint32_t uiConverted = ee::CExpEval::NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = std::towupper( ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen ) );
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
			uint32_t uiConverted = ee::CExpEval::NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = std::towlower( ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen ) );
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
			uint32_t uiConverted = ee::CExpEval::NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = HiraganaToKatakana( ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen ) );
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
			uint32_t uiConverted = ee::CExpEval::NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = KatakanaToHiragana( ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen ) );
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
			uint32_t uiConverted = ee::CExpEval::NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = FullWidthToByte( ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen ) );
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
			uint32_t uiConverted = ee::CExpEval::NextUtf16Char( &_swsInput[I], _swsInput.size() - I, &sLen );
			uint32_t uiTempLen;
			uiConverted = ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen );
			if ( uiTempLen == 1 ) {
				uiConverted = NumericToByte( ee::CExpEval::Utf32ToUtf16( uiConverted, uiTempLen ) );
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

	/**
	 * Creates a string with _cReplaceMe replaced with _cWithMe inside _s16String.
	 *
	 * \param _s16String The string in which replacements are to be made.
	 * \param _cReplaceMe The character to replace.
	 * \param _cWithMe The character with which to replace _cReplaceMe.
	 * \return Returns the new string with the given replacements made.
	 */
	std::u16string CUtilities::Replace( const std::u16string &_s16String, char16_t _cReplaceMe, char16_t _cWithMe ) {
		std::u16string s16Copy = _s16String;
		auto aFound = s16Copy.find( _cReplaceMe );
		while ( aFound != std::string::npos ) {
			s16Copy[aFound] = _cWithMe;
			aFound = s16Copy.find( _cReplaceMe, aFound + 1 );
		}
		return s16Copy;
	}

	/**
	 * Creates a string with _wcReplaceMe replaced with _wcWithMe inside _wsString.
	 *
	 * \param _wsString The string in which replacements are to be made.
	 * \param _wcReplaceMe The character to replace.
	 * \param _wcWithMe The character with which to replace _cReplaceMe.
	 * \return Returns the new string with the given replacements made.
	 */
	std::wstring CUtilities::Replace( const std::wstring &_wsString, wchar_t _wcReplaceMe, wchar_t _wcWithMe ) {
		std::wstring wsCopy = _wsString;
		auto aFound = wsCopy.find( _wcReplaceMe );
		while ( aFound != std::string::npos ) {
			wsCopy[aFound] = _wcWithMe;
			aFound = wsCopy.find( _wcReplaceMe, aFound + 1 );
		}
		return wsCopy;
	}

	/**
	 * Gets the current time in microseconds.
	 * 
	 * \return Returns the current time in microseconds.
	 **/
	uint64_t CUtilities::CurTimeInMicros() {
		static LARGE_INTEGER liFreq = { 0 };
		if ( !liFreq.QuadPart ) {
			::QueryPerformanceFrequency( reinterpret_cast<LARGE_INTEGER *>(&liFreq) );
		}
		LARGE_INTEGER liInt;
		::QueryPerformanceCounter( &liInt );
		return (liInt.QuadPart * 1000000ULL) / liFreq.QuadPart;
	}

	/**
	 * Gets the extension from a file path.
	 *
	 * \param _s16Path The file path whose extension is to be obtained.
	 * \return Returns a string containing the file extension.
	 */
	std::u16string CUtilities::GetFileExtension( const std::u16string &_s16Path ) {
		std::u16string s16File = GetFileName( _s16Path );
		std::string::size_type stFound = s16File.rfind( u'.' );
		if ( stFound == std::string::npos ) { return std::u16string(); }
		return s16File.substr( stFound + 1 );
	}

	/**
	 * Gets the extension from a file path.
	 *
	 * \param _wsPath The file path whose extension is to be obtained.
	 * \return Returns a string containing the file extension.
	 */
	std::wstring CUtilities::GetFileExtension( const std::wstring &_wsPath ) {
		std::wstring wsFile = GetFileName( _wsPath );
		std::string::size_type stFound = wsFile.rfind( L'.' );
		if ( stFound == std::string::npos ) { return std::wstring(); }
		return wsFile.substr( stFound + 1 );
	}

	/**
	 * Removes the extension from a file path.
	 *
	 * \param _s16Path The file path whose extension is to be removed.
	 * \return Returns a string containing the file mname without the extension.
	 */
	std::u16string CUtilities::NoExtension( const std::u16string &_s16Path ) {
		std::u16string s16File = GetFileName( _s16Path );
		std::string::size_type stFound = s16File.rfind( u'.' );
		if ( stFound == std::string::npos ) { return std::u16string(); }
		return s16File.substr( 0, stFound );
	}

	/**
	 * Removes the extension from a file path.
	 *
	 * \param _wsPath The file path whose extension is to be removed.
	 * \return Returns a string containing the file mname without the extension.
	 */
	std::wstring CUtilities::NoExtension( const std::wstring &_wsPath ) {
		std::wstring wsFile = GetFileName( _wsPath );
		std::string::size_type stFound = wsFile.rfind( L'.' );
		if ( stFound == std::string::npos ) { return std::wstring(); }
		return wsFile.substr( 0, stFound );
	}

	/**
	 * Gets the file name from a file path.
	 *
	 * \param _s16Path The file path whose name is to be obtained.
	 * \return Returns a string containing the file name.
	 */
	std::u16string CUtilities::GetFileName( const std::u16string &_s16Path ) {
		// If the last character is } then it is a file inside a ZIP.
		if ( _s16Path.size() && _s16Path[_s16Path.size()-1] == u'}' ) {
			std::string::size_type stFound = _s16Path.rfind( u'{' );
			std::u16string s16File = _s16Path.substr( stFound + 1 );
			s16File.pop_back();
			return s16File;
		}
		std::u16string s16Normalized = Replace( _s16Path, u'/', u'\\' );
		std::string::size_type stFound = s16Normalized.rfind( u'\\' );
		std::u16string s16File = s16Normalized.substr( stFound + 1 );

		return s16File;
	}

	/**
	 * Gets the file name from a file path.
	 *
	 * \param _wsPath The file path whose name is to be obtained.
	 * \return Returns a string containing the file name.
	 */
	std::wstring CUtilities::GetFileName( const std::wstring &_wsPath ) {
		// If the last character is } then it is a file inside a ZIP.
		if ( _wsPath.size() && _wsPath[_wsPath.size()-1] == L'}' ) {
			std::string::size_type stFound = _wsPath.rfind( L'{' );
			std::wstring wsFile = _wsPath.substr( stFound + 1 );
			wsFile.pop_back();
			return wsFile;
		}
		std::wstring wsNormalized = Replace( _wsPath, L'/', L'\\' );
		std::string::size_type stFound = wsNormalized.rfind( L'\\' );
		std::wstring wsFile = wsNormalized.substr( stFound + 1 );

		return wsFile;
	}

	/**
	 * Gets the file path without the file name
	 *
	 * \param _s16Path The file path whose path is to be obtained.
	 * \return Returns a string containing the file path.
	 */
	std::u16string CUtilities::GetFilePath( const std::u16string &_s16Path ) {
		if ( _s16Path.size() ) {
			std::u16string s16Normalized = Replace( _s16Path, u'/', u'\\' );
			std::string::size_type stFound = s16Normalized.rfind( u'\\' );
			if ( stFound >= s16Normalized.size() ) { return std::u16string(); }
			std::u16string s16File = s16Normalized.substr( 0, stFound + 1 );
			return s16File;
		}
		return std::u16string();
	}

	/**
	 * Gets the file path without the file name
	 *
	 * \param _wsPath The file path whose path is to be obtained.
	 * \return Returns a string containing the file path.
	 */
	std::wstring CUtilities::GetFilePath( const std::wstring &_wsPath ) {
		if ( _wsPath.size() ) {
			std::wstring wsNormalized = Replace( _wsPath, L'/', L'\\' );
			std::string::size_type stFound = wsNormalized.rfind( L'\\' );
			if ( stFound >= wsNormalized.size() ) { return std::wstring(); }
			std::wstring wsFile = wsNormalized.substr( 0, stFound + 1 );
			return wsFile;
		}
		return std::wstring();
	}

	/**
	 * Inserts a number of random characters into a string.  Returns _wsString.
	 * 
	 * \param _wsString The string to update.
	 * \param _stTotal Number of insertions to make.
	 * \param _pwcTable The table of characters that can be inserted.
	 * \param _stTableSize The number of characters in _pwcTable.
	 * \return Returns the updated input, _wsString.
	 **/
	std::wstring CUtilities::InsertRandomChars( std::wstring &_wsString, size_t _stTotal, const wchar_t * _pwcTable, size_t _stTableSize ) {
		if ( !_wsString.size() ) { return _wsString; }
		std::random_device rdRand;
		std::mt19937 mGen( rdRand() );
		
		std::uniform_int_distribution<> uidDistTable( 0, static_cast<int>(_stTableSize) - 1 );
		for ( auto I = _stTotal; I--; ) {
			std::uniform_int_distribution<> uidDistSrc( 0, static_cast<int>(_wsString.size()) - 1 );
			_wsString.insert( _wsString.begin() + uidDistSrc( mGen ), _pwcTable[uidDistTable(mGen)] );
		}


		return _wsString;
	}

	/**
	 * Replaces a number of random characters in a string with characters randomly chosen from _pwcTable.  Returns _wsString.
	 * 
	 * \param _wsString The string to update.
	 * \param _stTotal Number of replacements to make.
	 * \param _pwcTable The table of characters that can be inserted.
	 * \param _stTableSize The number of characters in _pwcTable.
	 * \return Returns the updated input, _wsString.
	 **/
	std::wstring CUtilities::ReplaceRandomChars( std::wstring &_wsString, size_t _stTotal, const wchar_t * _pwcTable, size_t _stTableSize ) {
		if ( !_wsString.size() ) { return _wsString; }
		std::random_device rdRand;
		std::mt19937 mGen( rdRand() );
		
		std::uniform_int_distribution<> uidDistTable( 0, static_cast<int>(_stTableSize) - 1 );
		std::uniform_int_distribution<> uidDistSrc( 0, static_cast<int>(_wsString.size()) - 1 );
		for ( auto I = _stTotal; I--; ) {
			_wsString.data()[uidDistSrc(mGen)] = _pwcTable[uidDistTable(mGen)];
		}


		return _wsString;
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
			uint64_t uiNext = _bIsUtf8 ? ee::CExpEval::NextUtf8Char( &_sInput[I], _sInput.size() - I, &sLen ) : (sLen = 1, static_cast<uint8_t>(_sInput[I]));
			if ( sLen == 1 ) {
				// It was just a regular character.  Could be an escape.
				uiNext = ee::CExpEval::ResolveEscape( &_sInput[I], _sInput.size() - I, sLen, false );
			}
			do {
				uint32_t ui32Len;
				uint32_t ui32BackToUtf8 = _bIsUtf8 ? ee::CExpEval::Utf32ToUtf8( static_cast<uint32_t>(uiNext), ui32Len ) : (ui32Len = ee::CExpEval::CountSetBytes( uiNext ), uiNext);
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
			uint64_t uiTmp = ee::CExpEval::EscapeHtml( _pcInput + 1, _sLen - 1, sLen );
			if ( sLen ) {
				_sCharLen = sLen + 1;
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
				uint32_t ui32Temp = ee::CExpEval::EscapeX( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; }
				return ui32Temp;
			}
			case 'u' : {
				// Takes \uNNNN and \uNNNN\uNNNN.
				uint32_t ui32Temp = ee::CExpEval::EscapeUnicodeWide4WithSurrogatePairs( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; }
				return ui32Temp;
			}
			case 'U' : {
				uint32_t ui32Temp = ee::CExpEval::EscapeUnicode8( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; }
				return ui32Temp;
			}
			case 'N' : {
				uint32_t ui32Temp = ee::CExpEval::EscapeNamedUnicode( &_pcInput[1], _sLen - 1, _sCharLen );
				if ( !_sCharLen ) { _sCharLen = 1; return static_cast<uint8_t>(*_pcInput); }
				else { ++_sCharLen; }
				return ui32Temp;
			}
			default : {
				if ( _pcInput[1] >= '0' && _pcInput[1] <= '7' ) {
					uint32_t ui32Temp = ee::CExpEval::EscapeOctal( &_pcInput[1], _sLen, _sCharLen );
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
			uint64_t uiNext = _bIsUtf8 ? ee::CExpEval::NextUtf8Char( &_sInput[I], _sInput.size() - I, &sLen ) : (sLen = 1, static_cast<uint8_t>(_sInput[I]));
			if ( sLen == 1 && uiNext == '&' ) {
				// It was just a regular character.  Could be an escape.
				uiNext = ee::CExpEval::ResolveEscape( &_sInput[I], _sInput.size() - I, sLen, true );
			}
			do {
				uint32_t ui32Len;
				uint32_t ui32BackToUtf8 = _bIsUtf8 ? ee::CExpEval::Utf32ToUtf8( static_cast<uint32_t>(uiNext), ui32Len ) : (ui32Len = ee::CExpEval::CountSetBytes( static_cast<uint32_t>(uiNext) ), uiNext);
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
			while ( I < sLen && ee::CExpEval::IsWhiteSpace( _sInput[I] ) ) { ++I; }
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
						dtVal.u.UInt16 = ee::CFloat16( ee::CExpEval::AtoF( &_sInput[I], &sEaten ) ).RawValue();
						break;
					}
					case MX_DT_FLOAT : {
						dtVal.u.Float32 = ee::CExpEval::AtoF( &_sInput[I], &sEaten );
						break;
					}
					case MX_DT_DOUBLE : {
						dtVal.u.Float64 = ee::CExpEval::AtoF( &_sInput[I], &sEaten );
						break;
					}
				}
				if ( !sEaten ) { break; }
			}
			else {
				if ( _iBase <= 0 ) {
					if ( (sLen - I) >= (2 + 1) ) {
						if ( _sInput[I] == '0' && _sInput[I+1] == 'x' ) {
							I += 2;
							dtVal.u.UInt64 = ee::CExpEval::StoULL( &_sInput[I], 10, &sEaten, _ui64MaxSingleValue );
						}
						else if ( _sInput[I] == '0' && _sInput[I+1] == 'o' ) {
							I += 2;
							dtVal.u.UInt64 = ee::CExpEval::StoULL( &_sInput[I], 8, &sEaten, _ui64MaxSingleValue );
						}
						else if ( _sInput[I] == '0' && _sInput[I+1] == 'b' ) {
							I += 2;
							dtVal.u.UInt64 = ee::CExpEval::StoULL( &_sInput[I], 2, &sEaten, _ui64MaxSingleValue );
						}
						else if ( _sInput[I] == '0' ) {
							I += 1;
							dtVal.u.UInt64 = ee::CExpEval::StoULL( &_sInput[I], 8, &sEaten, _ui64MaxSingleValue );
						}
						else {
							dtVal.u.UInt64 = ee::CExpEval::StoULL( &_sInput[I], 10, &sEaten, _ui64MaxSingleValue );
						}
					}
					else if ( (sLen - I) >= (1 + 1) && _sInput[I] == '0' ) {
						I += 1;
						dtVal.u.UInt64 = ee::CExpEval::StoULL( &_sInput[I], 8, &sEaten, _ui64MaxSingleValue );
					}
					else {
						dtVal.u.UInt64 = ee::CExpEval::StoULL( &_sInput[I], 10, &sEaten, _ui64MaxSingleValue );
					}
				}
				else {
					dtVal.u.UInt64 = ee::CExpEval::StoULL( &_sInput[I], _iBase, &sEaten, _ui64MaxSingleValue );
				}
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
#define MX_SKIP_WS		while ( I < sLen && (ee::CExpEval::IsWhiteSpace( _sInput[I] ) || _sInput[I] == ',') ) { ++I; }
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
						dtVal.u.UInt16 = ee::CFloat16( ee::CExpEval::AtoF( &_sInput[I], &sEaten ) ).RawValue();
						break;
					}
					case MX_DT_FLOAT : {
						dtVal.u.Float32 = ee::CExpEval::AtoF( &_sInput[I], &sEaten );
						break;
					}
					case MX_DT_DOUBLE : {
						dtVal.u.Float64 = ee::CExpEval::AtoF( &_sInput[I], &sEaten );
						break;
					}
				}
				if ( !sEaten ) { break; }
			}
			else {
				dtVal.u.UInt64 = ee::CExpEval::StoULL( &_sInput[I], _iBase, &sEaten, uiMax );
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

	// Takes a string and converts X number of elements into either a single byte array (if contiguous) or into an array of arrays.  Returns the number of items converted.
	uint32_t CUtilities::WStringToArrayBytes( std::vector<std::vector<uint8_t>> &_vDst, const CSecureWString &_swsString, MX_DATA_TYPES _dtTargetType, int _iBase, bool _bContiguous, CSecureWString &_swsError ) {
		try {
			// We support full-width Japanese characters.  Convert them to normal characters.
			CSecureString ssTmp;
			for ( size_t I = 0; I < _swsString.size(); ++I ) {
				auto ui32Char = FullWidthToByte( _swsString[I] );
				if ( ui32Char <= 0xFFFF ) {
					if ( std::iswprint( wint_t( ui32Char ) ) ) {
						ssTmp.push_back( char( ui32Char ) );
					}
					else if ( std::iswspace( wint_t( ui32Char ) ) ) {
						ssTmp.push_back( ' ' );
					}
				}
			}

			bool bError;
			auto vLines = ee::CExpEval::Tokenize( ssTmp, ' ', false, &bError );
			if ( bError ) {
				_swsError = _DEC_WS_F39F91A5_Out_of_memory_;
				return 0;
			}
			if ( _bContiguous ) {
				_vDst.resize( 1 );
			}
			else {
				_vDst.resize( vLines.size() );
			}

			uint32_t ui32Idx = 0;
			for ( size_t I = 0; I < vLines.size(); ++I ) {
				auto ssResult = NumberStringToString( vLines[I], _iBase, UINT64_MAX, _dtTargetType, nullptr );
				if ( !ssResult.size() ) {
					_swsError = _DEC_WS_CEE34699_Invalid_input__;
					_swsError += ee::CExpEval::ToUtf16<CSecureWString>( vLines[I] );
					return 0;
				}

				if ( _bContiguous ) {
					for ( size_t J = 0; J < ssResult.size(); ++J ) {
						_vDst[0].push_back( ssResult[J] );
					}
				}
				else {
					for ( size_t J = 0; J < ssResult.size(); ++J ) {
						_vDst[I].push_back( ssResult[J] );
					}
				}
				++ui32Idx;
			}
			
			return ui32Idx;
		}
		catch ( ... ) {
			_swsError = _DEC_WS_F39F91A5_Out_of_memory_;
			return 0;
		}
	}

	// Converts Katakana characters to Hiragana across a UTF-8 string.
	CSecureString CUtilities::KatakanaToHiragana( const std::string &_sString ) {
		CSecureString ssTemp;
		size_t I = 0;
		size_t sSize;
		do {
			uint32_t ui32This = ee::CExpEval::NextUtf8Char( &_sString[I], _sString.size() - I, &sSize );
			uint32_t ui32Len;
			uint32_t ui32Converted = KatakanaToHiragana( ee::CExpEval::Utf32ToUtf16( ui32This, ui32Len ) );
			size_t sConvertedSize;
			ui32This = ee::CExpEval::Utf32ToUtf8( ee::CExpEval::NextUtf16Char( reinterpret_cast<const wchar_t *>(&ui32Converted), sizeof( ui32Converted ) / sizeof( wchar_t ), &sConvertedSize ), ui32Len );

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
			uint32_t ui32This = ee::CExpEval::NextUtf16Char( &_wsString[I], _wsString.size() - I, &sSize );
			uint32_t ui32Len;
			uint32_t ui32Converted = KatakanaToHiragana( ee::CExpEval::Utf32ToUtf16( ui32This, ui32Len ) );
			uint32_t ui32Converted2 = KatakanaToHiragana( ui32This );
			size_t sConvertedSize;
			ui32This = ee::CExpEval::Utf32ToUtf16( ee::CExpEval::NextUtf16Char( reinterpret_cast<const wchar_t *>(&ui32Converted), sizeof( ui32Converted ) / sizeof( wchar_t ), &sConvertedSize ), ui32Len );

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
			uint32_t uiNext = ee::CExpEval::NextUtf8Char( &_sInput[I], _sInput.size() - I, &sLen );
			if ( sLen == 1 ) {
				// It was just a regular character.  Could be an escape.
				uiNext = ResolveEscape( &_sInput[I], _sInput.size() - I, sLen, false );
			}
			uint32_t ui32Len;
			uint32_t ui32BackToUtf8 = ee::CExpEval::Utf32ToUtf8( uiNext, ui32Len );
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
					uint32_t ui32This = ee::CExpEval::NextUtf8Char( reinterpret_cast<const char *>(&_puiData[J]), _sBytes - J, &sLen );
					/*if ( ui32This == MX_UTF_INVALID ) {
						ui32This = _puiData[J];
						sLen = 1;
					}*/
					uint32_t ui32Len;
					uint32_t ui32Converted = ee::CExpEval::Utf32ToUtf16( ui32This, ui32Len );
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
					uint32_t ui32This = ee::CExpEval::NextUtf16Char( reinterpret_cast<const wchar_t *>(&_puiData[J]), _sBytes - J, &sLen );
					/*if ( ui32This == MX_UTF_INVALID ) {
						ui32This = _puiData[J];
						sLen = 1;
					}*/
					uint32_t ui32Len;
					uint32_t ui32Converted = ee::CExpEval::Utf32ToUtf16( ui32This, ui32Len );
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
					uint32_t ui32This = ee::CExpEval::NextUtf32Char( reinterpret_cast<const uint32_t *>(&_puiData[J]), _sBytes - J, &sLen );
					/*if ( ui32This == MX_UTF_INVALID ) {
						ui32This = _puiData[J];
						sLen = 1;
					}*/
					uint32_t ui32Len;
					uint32_t ui32Converted = ee::CExpEval::Utf32ToUtf16( ui32This, ui32Len );
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
			uint32_t ui32Temp = ee::CExpEval::NextUtf8Char( reinterpret_cast<const char *>(&_puiData[I]), _sBytes - I, &sLen );
			_vData[I].ui8UftLen = static_cast<uint8_t>(sLen);
			if ( ui32Temp != MX_UTF_INVALID ) {
				uint32_t ui32Len;
				uint32_t ui32Converted = ee::CExpEval::Utf32ToUtf16( ui32Temp, ui32Len );
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
				_vData[I].ui32SearchChar = ee::CExpEval::NextUtf16Char( reinterpret_cast<const wchar_t *>(&ui32Converted), 2 );
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
				ui32Temp = ee::CExpEval::NextUtf16Char( szByteSwapped, 2, &sLen );
			}
			else {
				ui32Temp = ee::CExpEval::NextUtf16Char( reinterpret_cast<const wchar_t *>(&_puiData[sIdx]), _sBytes - sIdx, &sLen );
			}
			_vData[sIdx].ui8UftLen = static_cast<uint8_t>(sLen * sizeof( wchar_t ));
			if ( ui32Temp != MX_UTF_INVALID ) {
				uint32_t ui32Len;
				uint32_t ui32Converted = ee::CExpEval::Utf32ToUtf16( ui32Temp, ui32Len );
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
				_vData[sIdx].ui32SearchChar = ee::CExpEval::NextUtf16Char( reinterpret_cast<const wchar_t *>(&ui32Converted), 2 );
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
			uint32_t ui32Temp = ee::CExpEval::NextUtf32Char( &ui32Char, _sBytes - sIdx, &sLen );
			_vData[sIdx].ui8UftLen = static_cast<uint8_t>(sLen * sizeof( uint32_t ));
			if ( ui32Temp != MX_UTF_INVALID ) {
				uint32_t ui32Len;
				uint32_t ui32Converted = ee::CExpEval::Utf32ToUtf16( ui32Temp, ui32Len );
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
				_vData[sIdx].ui32SearchChar = ee::CExpEval::NextUtf16Char( reinterpret_cast<const wchar_t *>(&ui32Converted), 2 );
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
		std::sprintf( szBuffer, "PrintTotalGuiObjects: %u.\r\n", ::GetGuiResources( ::GetCurrentProcess(), _dwFlags ) );
		::OutputDebugStringA( szBuffer );*/
	}

	// The From and To address ranges.
	void CUtilities::FillAddressRangeComboBoxes( lsw::CComboBox * _pcbFrom, lsw::CComboBox * _pcbTo, const void * _poOptions ) {
		const MX_OPTIONS & oOptions = (*reinterpret_cast<const MX_OPTIONS *>(_poOptions));
		lsw::CComboBox * pcbCombo = _pcbFrom;
		if ( pcbCombo ) {
			for ( size_t I = 0; I < oOptions.vFromHistory.size(); ++I ) {
				pcbCombo->AddString( oOptions.vFromHistory[I].c_str() );
			}
			if ( oOptions.wsFromText.size() == 0 ) {
				uintptr_t uiptrTemp = reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMinimumApplicationAddress);
				std::string sTemp = CUtilities::ToHex( uiptrTemp, 4 );
				pcbCombo->SetTextA( sTemp.c_str() );
			}
			else {
				pcbCombo->SetTextW( oOptions.wsFromText.c_str() );
			}
			pcbCombo->SetTreatAsHex( TRUE );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		pcbCombo = _pcbTo;
		if ( pcbCombo ) {
			for ( size_t I = 0; I < oOptions.vToHistory.size(); ++I ) {
				pcbCombo->AddString( oOptions.vToHistory[I].c_str() );
			}
			if ( oOptions.wsToText.size() == 0 ) {
				uintptr_t uiptrTemp = reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMaximumApplicationAddress);
				std::string sTemp = CUtilities::ToHex( uiptrTemp + 1ULL, 4 );
				pcbCombo->SetTextA( sTemp.c_str() );
			}
			else {
				pcbCombo->SetTextW( oOptions.wsToText.c_str() );
			}
			pcbCombo->SetTreatAsHex( TRUE );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}
	}

	// Verifies the address-range combo boxes.  A return of 0 indicates success, a return of 1 indicates the From box had errors, and 2 indicates the To box had errors.
	uint8_t CUtilities::VerifyAddressRangeComboBoxes( lsw::CComboBox * _pcbFrom, lsw::CComboBox * _pcbTo ) {
		BOOL bIsValid;
		ee::CExpEvalContainer::EE_RESULT rRes;
		// Starting address.
		lsw::CComboBox * pcbCombo = _pcbFrom;
		if ( !pcbCombo || !pcbCombo->GetTextAsExpression( rRes, ee::CObject::EE_TF_NONE, &bIsValid ) ) {
			return 1;
		}
		//_spParmsIfValid.ui64AddressFrom = CUtilities::ExpEvalResultToDataType( rRes, CUtilities::MX_DT_UINT64 ).u.UInt64;
		// Ending search address.
		pcbCombo = _pcbTo;
		if ( !pcbCombo || !pcbCombo->GetTextAsExpression( rRes, ee::CObject::EE_TF_NONE, &bIsValid ) ) {
			return 2;
		}
		//_spParmsIfValid.ui64AddressTo = CUtilities::ExpEvalResultToDataType( rRes, CUtilities::MX_DT_UINT64 ).u.UInt64;

		return 0;
	}

	// Updates the options with the given range-address combo boxes.
	void CUtilities::UpdateAddressRangeComboBoxes( lsw::CComboBox * _pcbFrom, lsw::CComboBox * _pcbTo, void * _poOptions ) {
		MX_OPTIONS & oOptions = (*reinterpret_cast<MX_OPTIONS *>(_poOptions));

		lsw::CComboBox * pcbCombo = _pcbFrom;
		oOptions.wsFromText = pcbCombo->GetTextW();
		if ( oOptions.wsFromText.size() ) {
			auto aFound = std::find( oOptions.vFromHistory.begin(), oOptions.vFromHistory.end(), oOptions.wsFromText );
			if ( aFound != oOptions.vFromHistory.end() ) {
				oOptions.vFromHistory.erase( aFound );
			}
			oOptions.vFromHistory.insert( oOptions.vFromHistory.begin(), oOptions.wsFromText );
		}
		pcbCombo = _pcbTo;
		if ( pcbCombo ) {
			oOptions.wsToText = pcbCombo->GetTextW();
			if ( oOptions.wsToText.size() ) {
				auto aFound = std::find( oOptions.vToHistory.begin(), oOptions.vToHistory.end(), oOptions.wsToText );
				if ( aFound != oOptions.vToHistory.end() ) {
					oOptions.vToHistory.erase( aFound );
				}
				oOptions.vToHistory.insert( oOptions.vToHistory.begin(), oOptions.wsToText );
			}
		}
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
		if ( (_rResult.ncType) == ee::EE_NC_FLOATING ) {
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
		uint32_t ui32Bits = (_rResult.ncType) == ee::EE_NC_FLOATING ? 64 : 1;
		if ( (_rResult.ncType) == ee::EE_NC_SIGNED ) {
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
		else if ( (_rResult.ncType) == ee::EE_NC_UNSIGNED ) {
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


		if ( (_rResult.ncType) == ee::EE_NC_SIGNED && _rResult.u.i64Val < 0 ) {
			wsTemp.push_back( L',' );
			wsTemp.push_back( L' ' );
			wsTemp.push_back( L'-' );
			// Inverted-sign version.
			ToBinary( -_rResult.u.ui64Val, wsTemp, ui32Bits );
		}



		// 32-bit float in octal.
		wsTemp.push_back( L',' );
		wsTemp.push_back( L' ' );
		if ( (_rResult.ncType) == ee::EE_NC_FLOATING ) {
			union {
				float			fVal;
				uint32_t		ui32Val;
			} uTemp;
			uTemp.fVal = static_cast<float>(_rResult.u.dVal);

			ToOct( uTemp.ui32Val, wsTemp, 11 );

			wsTemp.push_back( L',' );
			wsTemp.push_back( L' ' );
		}

		ui32Bits = (_rResult.ncType) == ee::EE_NC_FLOATING ? 23 : 1;
		ToOct( _rResult.u.ui64Val, wsTemp, ui32Bits );
		if ( (_rResult.ncType) == ee::EE_NC_SIGNED && _rResult.u.i64Val < 0 ) {
			wsTemp.push_back( L',' );
			wsTemp.push_back( L' ' );
			wsTemp.push_back( L'-' );
			// Inverted-sign version.
			ToOct( -_rResult.u.ui64Val, wsTemp, ui32Bits );
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
		if ( (_rResult.ncType) == ee::EE_NC_FLOATING ) {
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
			wsTemp = ee::CExpEval::StringToWString( pcTemp );
		}
		else {
			wsTemp.append( _DEC_WS_3424431C_Invalid );
		}

		wsTemp.push_back( L',' );
		wsTemp.push_back( L' ' );

		
		WCHAR szBuffer[256];
		std::swprintf( szBuffer, MX_ELEMENTS( szBuffer ), L"%uy:%ud:%.2uh:%.2um:", uiYears, uiDays, uiHours, uiMinutes );
		wsTemp.append( szBuffer );
		ToDouble( uiSeconds + dFrac, wsTemp );
		wsTemp.push_back( L's' );

		if ( !::FileTimeToSystemTime( &ftFileTime, &stTime ) ) {
			wsTemp.append( _DEC_WS_3424431C_Invalid );
		}
		else {
			WCHAR szBuffer[256];
			//std::swprintf( szBuffer, MX_ELEMENTS( szBuffer ), L"%.2000f", _dValue );
		}

		/*if ( _bClosingPar ) {
			wsTemp.push_back( L')' );
		}*/
		return wsTemp;
	}

	/**
	 * Fills a combo box with the given array of MX_COMBO_ENTRY structures.
	 *
	 * \param _pwComboBox The combo box to fill.
	 * \param _pceEntries The array of combo-box entries.
	 * \param _stTotal The total number of entries to which _pceEntries points.
	 * \param _lpDefaultSelect The default selection.
	 * \param _lpSelectBackup The backup selection in case the default selection isn't in the list.
	 * \return Returns true if _pwComboBox is not nullptr, it is of type CComboBox, and all entries were added.
	 */
	bool CUtilities::FillComboBox( lsw::CWidget * _pwComboBox, const MX_COMBO_ENTRY * _pceEntries, size_t _stTotal, LPARAM _lpDefaultSelect, LPARAM _lpSelectBackup ) {
		if ( nullptr == _pwComboBox ) { return false; }
		if ( !_pwComboBox->IsComboBox() ) { return false; }

		lsw::CComboBox * pcbBox = static_cast<lsw::CComboBox *>(_pwComboBox);
		pcbBox->ResetContent();
		for ( size_t I = 0; I < _stTotal; ++I ) {
			INT iIdx = pcbBox->AddString( _pceEntries[I].pwcName.c_str() );
			if ( CB_ERR == iIdx ) { return false; }
			if ( CB_ERR == pcbBox->SetItemData( iIdx, _pceEntries[I].lpParm ) ) { return false; }
		}
		if ( CB_ERR == pcbBox->SetCurSelByItemData( _lpDefaultSelect ) ) {
			pcbBox->SetCurSelByItemData( _lpSelectBackup );
		}
		pcbBox->AutoSetMinListWidth();

		return true;
	}

	/**
	 * Fills a combo box with Edit Dialog data types.
	 * 
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if _pwComboBox is not nullptr, it is of type CComboBox, and all entries were added.
	 **/
	bool CUtilities::FillComboWithEditTypes( lsw::CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		std::wstring wsTmp;
		MX_COMBO_ENTRY ceEnries[] = {
			//pwcName																																			lpParm
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_INT8, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),											LPARAM( MX_DT_INT8 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_UINT8, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_UINT8 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_INT16, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_INT16 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_UINT16, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_UINT16 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_INT32, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_INT32 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_UINT32, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_UINT32 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_INT64, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_INT64 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_UINT64, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_UINT64 ),					},

			{ CSecureWString( PrintDataType( wsTmp, MX_DT_FLOAT16, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_FLOAT16 ),				},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_FLOAT, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_FLOAT ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_DOUBLE, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_DOUBLE ),					},

			//{ (Options.dwDataTypeOptions & MX_DTO_CODENAMES) ? _DEC_WS_301499E2_void__ : _DEC_WS_FDB95134_Pointer,												LPARAM( MX_DT_VOID ),					},
			//{ _DEC_WS_FDB95134_Pointer,																															LPARAM( MX_DT_VOID ),					},
			{ _DEC_WS_9912B79F_String,																															LPARAM( MX_DT_STRING ),					},
		};
		return FillComboBox( _pwComboBox, ceEnries, MX_ELEMENTS( ceEnries ), _lpDefaultSelect, -1 );
	}

	/**
	 * Fills a combo box with lock types.
	 * 
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if _pwComboBox is not nullptr, it is of type CComboBox, and all entries were added.
	 **/
	bool CUtilities::FillComboBoxWithLocktypes( lsw::CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		MX_COMBO_ENTRY ceEnries[] = {
			//pwcName																																			lpParm
			{ _DEC_WS_396582B1_Exact_Value,																														LPARAM( MX_LT_EXACT ),					},
			{ _DEC_WS_BF6E62BD_No_Lower_Than,																													LPARAM( MX_LT_NO_LOWER_THAN ),			},
			{ _DEC_WS_A5F79469_No_Greater_Than,																													LPARAM( MX_LT_NO_GREATER_THAN ),		},
			{ _DEC_WS_5246754D_Range,																															LPARAM( MX_LT_RANGE ),					},
		};
		return FillComboBox( _pwComboBox, ceEnries, MX_ELEMENTS( ceEnries ), _lpDefaultSelect, -1 );
	}

	/**
	 * Fills the combo box with string types.
	 * 
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if _pwComboBox is not nullptr, it is of type CComboBox, and all entries were added.
	 **/
	//bool CUtilities::FillComboBoxWithStringTypes( lsw::CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
	//	std::wstring wsTmp;
	//	MX_COMBO_ENTRY ceEnries[] = {
	//		//pwcName																																			lpParm
	//		{ _DEC_WS_468B510E_Machine_Code_Page,																												LPARAM( MX_ST_CODE_PAGE ),				},
	//		{ _DEC_WS_0E813C50_UTF_8,																															LPARAM( MX_ST_UTF8 ),					},
	//		{ _DEC_WS_A71F1195_UTF_16,																															LPARAM( MX_ST_UTF16_LE ),				},
	//		{ _DEC_WS_26FC5333_UTF_16_BE,																														LPARAM( MX_ST_UTF16_BE ),				},
	//		{ _DEC_WS_9244B70E_UTF_32,																															LPARAM( MX_ST_UTF32_LE ),				},
	//		{ _DEC_WS_D35E9704_UTF_32_BE,																														LPARAM( MX_ST_UTF32_BE ),				},
	//		//{ _DEC_WS_F22813AD_Custom,																															LPARAM( MX_ST_SPECIAL ),				},
	//	};
	//	return FillComboBox( _pwComboBox, ceEnries, MX_ELEMENTS( ceEnries ), _lpDefaultSelect, -1 );
	//}

	/**
	 * Fills the combo box with standard data types.
	 * 
	 * \param _pwComboBox The combo box to fill.
	 * \param _lpDefaultSelect The default selection.
	 * \return Returns true if _pwComboBox is not nullptr, it is of type CComboBox, and all entries were added.
	 **/
	bool CUtilities::FillComboBoxWithStdDataTypes( lsw::CWidget * _pwComboBox, LPARAM _lpDefaultSelect ) {
		std::wstring wsTmp;
		MX_COMBO_ENTRY ceEnries[] = {
			//pwcName																																			lpParm
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_INT8, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),											LPARAM( MX_DT_INT8 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_UINT8, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_UINT8 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_INT16, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_INT16 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_UINT16, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_UINT16 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_INT32, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_INT32 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_UINT32, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_UINT32 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_INT64, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_INT64 ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_UINT64, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_UINT64 ),					},

			{ CSecureWString( PrintDataType( wsTmp, MX_DT_FLOAT16, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_FLOAT16 ),				},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_FLOAT, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_FLOAT ),					},
			{ CSecureWString( PrintDataType( wsTmp, MX_DT_DOUBLE, Options.dwDataTypeOptions & MX_DTO_CODENAMES, true ) ),										LPARAM( MX_DT_DOUBLE ),					},
		};
		return FillComboBox( _pwComboBox, ceEnries, MX_ELEMENTS( ceEnries ), _lpDefaultSelect, -1 );
	}

}	// namespace mx

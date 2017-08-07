#include "MXUtilities.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"

namespace mx {

	// Convert a single byte to hex.  _pcString must be at least 3 characters long.
	VOID ByteToHex( BYTE _bIn, CHAR * _pcString, BOOL _bLower ) {
		static const CHAR * pszHex = { "0123456789ABCDEF" };
		static const CHAR * pszHexLower = { "0123456789abcdef" };
		BYTE bTemp = (_bIn >> 4);
		_pcString[0] = _bLower ? pszHexLower[bTemp] : pszHex[bTemp];
		bTemp = (_bIn & 0xF);
		_pcString[1] = _bLower ? pszHexLower[bTemp] : pszHex[bTemp];
		_pcString[2] = '\0';
	}

	// Convert a single byte to hex.
	VOID ByteToHex( BYTE _bIn, std::string &_sString, BOOL _bLower ) {
		static const CHAR * pszHex = { "0123456789ABCDEF" };
		static const CHAR * pszHexLower = { "0123456789abcdef" };
		BYTE bTemp = (_bIn >> 4);
		
		_sString.push_back( _bLower ? pszHexLower[bTemp] : pszHex[bTemp] );
		bTemp = (_bIn & 0xF);
		_sString.push_back( _bLower ? pszHexLower[bTemp] : pszHex[bTemp] );
	}
	
	// Convert a byte array to a hex string.  _pcString must be twice as long as the input, plus 1 character for the NULL.
	VOID BytesToHex( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		for ( size_t I = 0; I < _sLen; ++I, ++bIn, _pcString += 2 ) {
			ByteToHex( (*bIn), _pcString, _bLower );
		}
	}

	// Convert a byte array to a hex string.
	VOID BytesToHex( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		for ( size_t I = 0; I < _sLen; ++I, ++bIn ) {
			ByteToHex( (*bIn), _sString, _bLower );
		}
	}

	// Convert a byte array to a hex string.  _pcString must be 3 times as long as the input.
	VOID BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		for ( size_t I = 0; I < _sLen; ++I, ++bIn ) {
			ByteToHex( (*bIn), _pcString, _bLower );
			_pcString += 2;
			(*_pcString++) = ' ';
		}
		(*_pcString) = '\0';
	}

	// Convert a byte array to a hex string.
	VOID BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		size_t sEnd = _sLen - 1;
		for ( size_t I = 0; I < _sLen; ++I, ++bIn ) {
			ByteToHex( (*bIn), _sString, _bLower );
			if ( I != sEnd ) {
				_sString.push_back( ' ' );
			}
		}
	}

	// Is the given character printable?  If not, it should be printed as a space or question mark.
	BOOL ByteIsPrintable( BYTE _bIn, BOOL _bPrintExtended ) {
		if ( _bIn < 32 || _bIn == 127 ) { return FALSE; }
		return _bIn < 128 || _bPrintExtended;
	}

	// Convert a single byte to a printable character.  If _bPrintExtended is TRUE, characters above 127 are kept.
	CHAR ByteToPrintable( BYTE _bIn, BOOL _bPrintExtended, BOOL _bUseQuestionMarks ) {
		CHAR cDefault = _bUseQuestionMarks ? '?' : ' ';
		return ByteIsPrintable( _bIn, _bPrintExtended ) ? _bIn : cDefault;
	}

	// Converts a byte to a C-string value.  _pcString must be at least 5 characters long.  Returns the number of characters written.
	DWORD ByteToCString( BYTE _bIn, CHAR * _pcString, BOOL _bLower ) {
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
				if ( !ByteIsPrintable( _bIn, TRUE ) ) {
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
	DWORD ByteToCString( BYTE _bIn, std::string &_sString, BOOL _bLower ) {
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
				if ( !ByteIsPrintable( _bIn, TRUE ) ) {
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
	DWORD BytesToCString( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		DWORD dwTotal = 0;
		for ( size_t I = 0; I < _sLen; ++I, ++bIn ) {
			DWORD dwLen = ByteToCString( (*bIn), _pcString, _bLower );
			_pcString += dwLen;
			dwTotal += dwLen;
		}
		return dwTotal;
	}

	// Convert a byte array to a C string.  Returns the number of characters written.
	DWORD BytesToCString( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower ) {
		const BYTE * bIn = static_cast<const BYTE *>(_pvIn);
		DWORD dwTotal = 0;
		for ( size_t I = 0; I < _sLen; ++I, ++bIn ) {
			DWORD dwLen = ByteToCString( (*bIn), _sString, _bLower );
			dwTotal += dwLen;
		}
		return dwTotal;
	}

	// Converts a MX_MACHINE_TYPES enum to its string value.  _pcRet should be at least 32 characters long.
	VOID MachineTypeToString( uint32_t _uiType, CHAR * _pcRet ) {
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
				return;
			}
		}
		std::strcpy( _pcRet, "Unknown" );
	}

	VOID PeCharacteristicsToString( uint32_t _uiVal, std::string &_sString ) {
		struct {
			uint32_t ui32Type;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
			{ MX_IMAGE_FILE_RELOCS_STRIPPED, _T_A8CF4CB1_IMAGE_FILE_RELOCS_STRIPPED, _LEN_A8CF4CB1 },
			{ MX_IMAGE_FILE_EXECUTABLE_IMAGE, _T_8CE929D7_IMAGE_FILE_EXECUTABLE_IMAGE, _LEN_8CE929D7 },
			{ MX_IMAGE_FILE_LINE_NUMS_STRIPPED, _T_4ACFE92C_IMAGE_FILE_LINE_NUMS_STRIPPED, _LEN_4ACFE92C },
			{ MX_IMAGE_FILE_AGGRESIVE_WS_TRIM, _T_C5765866_IMAGE_FILE_AGGRESIVE_WS_TRIM, _LEN_C5765866 },
			{ MX_IMAGE_FILE_LARGE_ADDRESS_AWARE, _T_CA4B53ED_IMAGE_FILE_LARGE_ADDRESS_AWARE, _LEN_CA4B53ED },
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
				ZeroMemory( szBuffer, _T_MAX_LEN );
				bAdded = TRUE;
			}
		}
		if ( !bAdded ) {
			_sString.append( "No Flags Set" );
		}
	}

	// Converts a MX_PE_MAGIC_FLAGS enum to its string value.  _pcRet should be at least 32 characters long.
	VOID PeMagicTypeToString( uint32_t _uiType, CHAR * _pcRet ) {
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
				return;
			}
		}
		std::strcpy( _pcRet, "Unknown" );
	}

	// Converts a MX_PE_WINDOWS_SUBSYSTEM enum to its string value.  _pcRet should be at least 32 characters long.
	VOID PeSubSystemTypeToString( uint32_t _uiType, CHAR * _pcRet ) {
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
				return;
			}
		}
		std::strcpy( _pcRet, "Unknown" );
	}

	// Creates a string with all of the MX_PE_DLL_CHARACTERISTICS bit flags set in a given value.
	VOID PeDllCharacteristicsToString( uint32_t _uiVal, std::string &_sString ) {
		struct {
			uint32_t ui32Type;
			const CHAR * pcName;
			uint32_t ui32StrLen;
		}
		static const aTable[] = {
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
	}

	// Gets a Windows version string based on the given major and minor versions.
	VOID WindowsVersion( uint32_t _uiMajor, uint32_t _uiMinor, std::string &_sString ) {
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
				ZeroMemory( szBuffer, _T_MAX_LEN );
				return;
			}
		}
		_sString.append( "Unknown" );
	}

	// Creates a string that best represents the given size.
	VOID SizeString( uint64_t _uiSize, std::string &_sString ) {
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
				ZeroMemory( szBuffer, _T_MAX_LEN );
				return;
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
		ZeroMemory( szBuffer, _T_MAX_LEN );
	}

}	// namespace mx
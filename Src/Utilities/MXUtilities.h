#pragma once

#include <LSWWin.h>
#include "../PE/MXPeStructs.h"

#include <EEExpEvalContainer.h>
#include <string>

// == Macros.
#ifndef MX_COUNT_OF
#define MX_COUNT_OF( x )				((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
#define MX_ELEMENTS( x )				MX_COUNT_OF( x )
#endif	// #ifndef MX_COUNT_OF


namespace mx {

	class CUtilities {
	public :
		// == Enumerations.
		enum MX_RESOURCE_TYPES {
			MX_RT_CURSOR				= 1,
			MX_RT_BITMAP				= 2,
			MX_RT_ICON					= 3,
			MX_RT_MENU					= 4,
			MX_RT_DIALOG				= 5,
			MX_RT_STRING				= 6,
			MX_RT_FONTDIR				= 7,
			MX_RT_FONT					= 8,
			MX_RT_ACCELERATOR			= 9,
			MX_RT_RCDATA				= 10,
			MX_RT_MESSAGETABLE			= 11,

			MX_DIFFERENCE				= 11,
			MX_RT_GROUP_CURSOR			= MX_RT_CURSOR + MX_DIFFERENCE,
			MX_RT_GROUP_ICON			= MX_RT_ICON + MX_DIFFERENCE,
			MX_RT_VERSION				= 16,
			MX_RT_DLGINCLUDE			= 17,
			MX_RT_PLUGPLAY				= 19,
			MX_RT_VXD					= 20,
			MX_RT_ANICURSOR				= 21,
			MX_RT_ANIICON				= 22,
			MX_RT_HTML					= 23,
			MX_RT_MANIFEST				= 24,
		};

		// Data types.
		enum MX_DATA_TYPES {
			MX_DT_INT8,
			MX_DT_UINT8,
			MX_DT_INT16,
			MX_DT_UINT16,
			MX_DT_INT32,
			MX_DT_UINT32,
			MX_DT_INT64,
			MX_DT_UINT64,
			MX_DT_FLOAT,
			MX_DT_DOUBLE,
			MX_DT_VOID,
		};

		// Data-type options.
		enum MX_DATA_TYPE_OPTIONS : DWORD {
			MX_DTO_CODENAMES			= (1 << 0),					// Unsigned Short vs. uint16_t
			MX_DTO_SHOWRANGES			= (1 << 1),					// "uint16_t" vs. "uint16_t (0-65,535)"
			MX_DTO_SHOWSIZES			= (1 << 2),					// "uint16_t" vs. "uint16_t (2 bytes)"
			MX_DTO_DEFAULT				= MX_DTO_CODENAMES | MX_DTO_SHOWRANGES | MX_DTO_SHOWSIZES,
		};

		// Main search types.
		enum MX_SEARCH_TYPES {
			MX_ST_DATATYPE_SEARCH,
			MX_ST_STRING_SEARCH,
			MX_ST_EXP_SEARCH,
			MX_ST_POINTER_SEARCH,
			MX_ST_GROUP_SEARCH,
		};

		// Evaluation types.
		enum MX_EVAL_TYPES {
			MX_ET_EXACT,
			MX_ET_NOT_EQUAL_TO,
			MX_ET_GREATER_THAN,
			MX_ET_LESS_THAN,
			MX_ET_RANGE,
			MX_ET_UNKNOWN,
		};

		// == Types.
		// Various options.
		struct MX_UTIL_OPTIONS {
			BOOL						bUse0xForHex;				// 0xOOOO vs. OOOOh
			BOOL						bShortenEnumNames;			// IMAGE_SCN_MEM_LOCKED vs. LOCKED
			DWORD						dwDataTypeOptions;			// MX_DATA_TYPE_OPTIONS
		};

		// Data-type information.
		struct MX_DATA_TYPE_INFO {
			MX_DATA_TYPES				dtType;

			const CHAR *				pcCodeName;
			size_t						sCodeNameLen;

			const CHAR *				pcBasicName;
			size_t						sBasicNameLen;

			const CHAR *				pcRange;
			size_t						sRangeLen;

			DWORD						dwSize;
			BOOL						bIsFloat;
		};

		// == Functions.
		// Convert a single byte to hex.  _pcString must be at least 3 characters long.
		static VOID						ByteToHex( BYTE _bIn, CHAR * _pcString, BOOL _bLower = FALSE );

		// Convert a single byte to hex.
		static VOID						ByteToHex( BYTE _bIn, std::string &_sString, BOOL _bLower = FALSE );

		// Convert a byte array to a hex string.  _pcString must be twice as long as the input, plus 1 character for the NULL.
		static VOID						BytesToHex( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower = FALSE );

		// Convert a byte array to a hex string.
		static VOID						BytesToHex( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower = FALSE );

		// Convert a byte array to a hex string.  _pcString must be 3 times as long as the input.
		static VOID						BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower = FALSE );

		// Convert a byte array to a hex string.
		static VOID						BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower = FALSE );

		// Is the given character printable?  If not, it should be printed as a space or question mark.
		static BOOL						ByteIsPrintable( BYTE _bIn, BOOL _bPrintExtended = TRUE );

		// Convert a single byte to a printable character.  If _bPrintExtended is TRUE, characters above 127 are kept.
		static CHAR						ByteToPrintable( BYTE _bIn, BOOL _bPrintExtended = TRUE, BOOL _bUseQuestionMarks = FALSE );

		// Converts a byte to a C-string value.  _pcString must be at least 5 characters long.  Returns the number of characters written.
		static DWORD					ByteToCString( BYTE _bIn, CHAR * _pcString, BOOL _bLower = FALSE );

		// Converts a byte to a C-string value.  Returns the number of characters written.
		static DWORD					ByteToCString( BYTE _bIn, std::string &_sString, BOOL _bLower = FALSE );

		// Convert a byte array to a C string.  Returns the number of characters written.
		static DWORD					BytesToCString( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower = FALSE );

		// Convert a byte array to a C string.  Returns the number of characters written.
		static DWORD					BytesToCString( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower = FALSE );

		// Converts a MX_MACHINE_TYPES enum to its string value.  _pcRet should be at least 32 characters long.
		static const CHAR *				MachineTypeToString( uint32_t _uiType, CHAR * _pcRet );

		// Creates a string with all of the MX_PE_CHARACTERISTICS bit flags set in a given value.
		static const CHAR *				PeCharacteristicsToString( uint32_t _uiVal, std::string &_sString );

		// Converts a MX_PE_MAGIC_FLAGS enum to its string value.  _pcRet should be at least 32 characters long.
		static const CHAR *				PeMagicTypeToString( uint32_t _uiType, CHAR * _pcRet );

		// Converts a MX_PE_WINDOWS_SUBSYSTEM enum to its string value.  _pcRet should be at least 32 characters long.
		static const CHAR *				PeSubSystemTypeToString( uint32_t _uiType, CHAR * _pcRet );

		// Creates a string with all of the MX_PE_DLL_CHARACTERISTICS bit flags set in a given value.
		static const CHAR *				PeDllCharacteristicsToString( uint32_t _uiVal, std::string &_sString );

		// Creates a string with all of the MX_PE_SECTION_FLAGS bit flags set in a given value.
		static const CHAR *				PeSectionFlagsToString( uint32_t _uiVal, std::string &_sString );

		// Gets a Windows version string based on the given major and minor versions.
		static const CHAR *				WindowsVersion( uint32_t _uiMajor, uint32_t _uiMinor, std::string &_sString );

		// Creates an IMAGE_REL_BASED_* string.
		static const CHAR *				PeRelocBaseToString( uint32_t _uiVal, std::string &_sString );

		// Creates a PROCESS_ string.
		static const CHAR *				OpenProcessFlagToString( uint32_t _uiVal, std::string &_sString, BOOL _bShort = -1 );

		// Creates a string that best represents the given size.
		static const CHAR *				SizeString( uint64_t _uiSize, std::string &_sString );

		// Creates a date string.
		static const CHAR *				CreateDateString( uint64_t _uiTime, CHAR * _pcRet, size_t _sLen );

		// Creates a date string.
		static const CHAR *				CreateDateString( uint64_t _uiTime, std::string &_sString );

		// Creates a hexadecimal string.  Returns the internal buffer, which means the result must be copied as it will be overwritten when the next function that uses the internal buffer is called.
		static const CHAR *				ToHex( uint64_t _uiValue, uint32_t _uiNumDigits );

		// Creates a hexadecimal string.
		static const CHAR *				ToHex( uint64_t _uiValue, CHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits );

		// Creates a hexadecimal string.
		static const CHAR *				ToHex( uint64_t _uiValue, std::string &_sString, uint32_t _uiNumDigits );

		// Creates a hexadecimal string.
		static const WCHAR *			ToHex( uint64_t _uiValue, WCHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits );

		// Creates a hexadecimal string.
		static const WCHAR *			ToHex( uint64_t _uiValue, std::wstring &_sString, uint32_t _uiNumDigits );

		// Creates an unsigned integer string.  Returns the internal buffer, which means the result must be copied as it will be overwritten when the next function that uses the internal buffer is called.
		static const CHAR *				ToUnsigned( uint64_t _uiValue, uint32_t _uiNumDigits = 0 );

		// Creates an unsigned integer string.
		static const CHAR *				ToUnsigned( uint64_t _uiValue, CHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits = 0 );

		// Creates an unsigned integer string.
		static const CHAR *				ToUnsigned( uint64_t _uiValue, std::string &_sString, uint32_t _uiNumDigits = 0 );

		// Creates an unsigned integer string.
		static const WCHAR *			ToUnsigned( uint64_t _uiValue, WCHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits = 0 );

		// Creates an unsigned integer string.
		static const WCHAR *			ToUnsigned( uint64_t _uiValue, std::wstring &_sString, uint32_t _uiNumDigits = 0 );

		// Creates a signed integer string.
		static const CHAR *				ToSigned( int64_t _iValue, CHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits = 0 );

		// Creates a signed integer string.
		static const CHAR *				ToSigned( int64_t _iValue, std::string &_sString, uint32_t _uiNumDigits = 0 );

		// Creates an signed integer string.
		static const WCHAR *			ToSigned( int64_t _iValue, WCHAR * _pcRet, size_t _sLen, uint32_t _uiNumDigits = 0 );

		// Creates an signed integer string.
		static const WCHAR *			ToSigned( int64_t _iValue, std::wstring &_sString, uint32_t _uiNumDigits = 0 );

		// Creates a double string.
		static const CHAR *				ToDouble( double _dValue, std::string &_sString, int32_t _iSigDigits = -1 );

		// Creates a double string.
		static const WCHAR *			ToDouble( double _dValue, std::wstring &_sString, int32_t _iSigDigits = -1 );

		// Gets the size of a data type.
		static DWORD					DataTypeSize( CUtilities::MX_DATA_TYPES _dtType );

		// Is the data type a float type?
		static BOOL						DataTypeIsFloat( CUtilities::MX_DATA_TYPES _dtType );

		// Gets the range of a data type as a string.
		static const CHAR *				DataTypeRange( CUtilities::MX_DATA_TYPES _dtType, std::string &_sString );

		// Gets the range of a data type as a string.
		static const WCHAR *			DataTypeRange( CUtilities::MX_DATA_TYPES _dtType, std::wstring &_sString );

		// Prints the size of the given data type as a string.
		static const CHAR *				DataTypeSize( CUtilities::MX_DATA_TYPES _dtType, std::string &_sString );

		// Prints the size of the given data type as a string.
		static const WCHAR *			DataTypeSize( CUtilities::MX_DATA_TYPES _dtType, std::wstring &_sString );

		// Prints a data type given the options.
		static const CHAR *				PrintDataType( std::string &_sString, CUtilities::MX_DATA_TYPES _dtType, DWORD _dwOptions = static_cast<DWORD>(-1) );

		// Prints a data type given the options.
		static const WCHAR *			PrintDataType( std::wstring &_sString, CUtilities::MX_DATA_TYPES _dtType, DWORD _dwOptions = static_cast<DWORD>(-1) );

		// MX_SEARCH_TYPES value to a string.
		static const WCHAR *			SearchTypeToString( CUtilities::MX_SEARCH_TYPES _stType, std::wstring &_sString );

		// MX_EVAL_TYPES value to a string.
		static const WCHAR *			EvaluationTypeToString( CUtilities::MX_EVAL_TYPES _etType, std::wstring &_sString );

		// Clears the internal temporary buffer (as a security measure).
		static VOID						ClearInternalBuffer();

		// Converts a wstring to a UTF-8 string.
		static std::string				WStringToString( const std::wstring &_wsIn );

		// Converts a UTF-8 string to wstring (UTF-16).
		static std::wstring				StringToWString( const std::string &_sIn );

		// Windows resource type to string.
		static BOOL						ResourceTypeToString( uint64_t _uiId, CHAR * _pcRet );

		// Windows resource type to string.
		static BOOL						ResourceTypeToString( uint64_t _uiId, std::string &_sString );

		// Adds a \ to the end of a string if it does not already have one.
		static std::string &			FinishPath( std::string &_sString );

		// Adds a \ to the end of a string if it does not already have one.
		static std::wstring &			FinishPath( std::wstring &_sString );

		// Generates a string of random characters.
		static CHAR *					RandomString( CHAR * _pcBuffer, SIZE_T _sSize );

		// Generates a string of random characters.
		static WCHAR *					RandomString( WCHAR * _pwcBuffer, SIZE_T _sSize );

		// Gets the number of elements in DataTypeInfo.
		static size_t					DataTypeInfoLen();

		// Prints an ee::CExpEvalContainer::EE_RESULT value.
		static std::string &			PrintExpResult( const ee::CExpEvalContainer::EE_RESULT &_rResult, std::string &_sString, int32_t _iDblSciPrec = -1 );


		// == Members.
		// Options.
		static MX_UTIL_OPTIONS			Options;

		// Data-type information.
		static MX_DATA_TYPE_INFO		DataTypeInfo[];

	protected :
		// == Members.
		// Internal buffer for temporary strings.
		static CHAR						m_szTemp[128];
	};

	

}	// namespace mx

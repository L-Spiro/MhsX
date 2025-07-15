#pragma once

#include "../MXMhsX.h"
#include "../PE/MXPeStructs.h"
#include "../Strings/MXSecureString.h"
#include "../Strings/MXSecureWString.h"
#include "../System/MXSystem.h"

#include <ComboBox/LSWComboBox.h>
#include <CriticalSection/LSWCriticalSection.h>
#include <EEExpEvalContainer.h>
#include <clocale>
#include <cwctype>
#include <fenv.h>
#include <filesystem>
#include <mbstring.h>
#include <random>
#include <string>


// == Macros.
#ifndef MX_COUNT_OF
#define MX_COUNT_OF( x )				((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
#define MX_ELEMENTS( x )				MX_COUNT_OF( x )
#endif	// #ifndef MX_COUNT_OF

#define MX_UTF_INVALID					~static_cast<uint32_t>(0)

// For printing purposes, extra digits included.  Not representative of their real significant digits.
#define MX_FLOAT10_SIG_DIG				3
#define MX_FLOAT11_SIG_DIG				4
#define MX_FLOAT14_SIG_DIG				5
#define MX_FLOAT16_SIG_DIG				6
#define MX_FLOAT24_SIG_DIG				8
#define MX_FLOAT32_SIG_DIG				9
#define MX_FLOAT64_SIG_DIG				19

// Fast std::memcpy() functions for known-sized small copies.
#define memcpy_2( DEST, SRC )			(*reinterpret_cast<uint16_t *>(DEST)) = (*reinterpret_cast<uint16_t *>(SRC))
#define memcpy_3( DEST, SRC )			(*reinterpret_cast<uint16_t *>(DEST)) = (*reinterpret_cast<uint16_t *>(SRC)); reinterpret_cast<uint8_t *>(DEST)[2] = reinterpret_cast<uint8_t *>(SRC)[2]
#define memcpy_4( DEST, SRC )			(*reinterpret_cast<uint32_t *>(DEST)) = (*reinterpret_cast<uint32_t *>(SRC))
#define memcpy_5( DEST, SRC )			(*reinterpret_cast<uint32_t *>(DEST)) = (*reinterpret_cast<uint32_t *>(SRC)); reinterpret_cast<uint8_t *>(DEST)[4] = reinterpret_cast<uint8_t *>(SRC)[4]
#define memcpy_6( DEST, SRC )			(*reinterpret_cast<uint32_t *>(DEST)) = (*reinterpret_cast<uint32_t *>(SRC)); reinterpret_cast<uint16_t *>(DEST)[2] = reinterpret_cast<uint16_t *>(SRC)[2]
#define memcpy_7( DEST, SRC )			(*reinterpret_cast<uint32_t *>(DEST)) = (*reinterpret_cast<uint32_t *>(SRC)); reinterpret_cast<uint16_t *>(DEST)[2] = reinterpret_cast<uint16_t *>(SRC)[2]; reinterpret_cast<uint8_t *>(DEST)[6] = reinterpret_cast<uint8_t *>(SRC)[6]
#define memcpy_8( DEST, SRC )			(*reinterpret_cast<uint64_t *>(DEST)) = (*reinterpret_cast<uint64_t *>(SRC))

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

		// Types of values.
		enum MX_VALUE_TYPE {
			MX_VT_DATA_TYPE,							// An MX_DATA_TYPES value.
			MX_VT_STRING,								// A string value.  String is of an MX_STRING_TYPES type.
			MX_VT_POINTER,								// A pointer to a value.  Can point to an MX_DATA_TYPES type.
			MX_VT_BLOB,									// Any generic blob of data.  Individual elements can be of MX_DATA_TYPES types.
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
			MX_DT_FLOAT16,
			MX_DT_DOUBLE,
			MX_DT_VOID,

			MX_DT_MASK					= 0xF
		};

		// String types.
		enum MX_STRING_TYPES {
			MX_ST_CODE_PAGE,							// A regular string in a given codepage.
			MX_ST_UTF8,									// A UTF-8 string.
			MX_ST_UTF16_LE,								// A Little-Endian UTF-16 string.
			MX_ST_UTF16_BE,								// A Big-Endian UTF-16 string.
			MX_ST_UTF32_LE,								// A Little-Endian UTF-32 string.
			MX_ST_UTF32_BE,								// A Big-Endian UTF-32 string.
		};

		// Wildcard flags.  Adjust values so that they can be combined with MX_DATA_TYPES if more values are added to MX_DATA_TYPES.
		enum MX_WILDCARD_FLAGS {
			MX_WF_NONE					= 0,
			MX_WF_ANY_CHAR				= (1 << 5),		// ?
			MX_WF_ANY					= (1 << 6),		// *

			MX_WF_MASK					= MX_WF_ANY_CHAR | MX_WF_ANY
		};

		// Data-type options.
		enum MX_DATA_TYPE_OPTIONS : DWORD {
			MX_DTO_CODENAMES			= (1 << 0),		// Unsigned Short vs. uint16_t
			MX_DTO_SHOWRANGES			= (1 << 1),		// "uint16_t" vs. "uint16_t (0-65,535)"
			MX_DTO_SHOWSIZES			= (1 << 2),		// "uint16_t" vs. "uint16_t (2 bytes)"
			MX_DTO_DEFAULT				= MX_DTO_CODENAMES | MX_DTO_SHOWRANGES | MX_DTO_SHOWSIZES,
		};

		// Expression search ?? meanings.
		enum MX_EXP_SEARCH_QQ_MEANING {
			MX_ESQM_ADDRESS,			// Accessed using dereferencing operator (u32[<ADDR>]).
			MX_ESQM_VALUE,				// Value at the current address.
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
			MX_ET_GREATER_THAN,
			MX_ET_LESS_THAN,
			MX_ET_RANGE,
			MX_ET_UNKNOWN,
			MX_ET_QUICK_EXP,
		};

		// Subsearch evaluation types.
		enum MX_SUB_EVAL_TYPES {
			MX_SET_EXACT,
			MX_SET_NOT_EQUAL_TO,
			MX_SET_INCREASED,
			MX_SET_DECREASED,
			MX_SET_RANGE,
			MX_SET_GREATER_THAN,
			MX_SET_LESS_THAN,
			MX_SET_SAME_AS_BEFORE,
			MX_SET_DIFFERENT_FROM_BEFORE,
			MX_SET_CHANGED_BY,
			MX_SET_CHANGED_BY_PERCENT,
			MX_SET_CHANGED_BY_RANGE,
			MX_SET_CHANGED_BY_RANGE_PERCENT,
			MX_SET_SAME_AS_ORIGINAL,
			MX_SET_QUICK_EXP,
		};

		// String search types.
		enum MX_STRING_SEARCH_TYPES {
			MX_SST_ASCII,				// Search string not encoded.
			MX_SST_UTF8,				// Search string is encoded in UTF-8.
			MX_SST_UTF16,				// Search string is encoded in UTF-16.
			MX_SST_UTF16_BE,			// Search string is encoded in UTF-16 BE.
			MX_SST_UTF32,				// Search string is encoded in UTF-32.
			MX_SST_UTF32_BE,			// Search string is encoded in UTF-32 BE.
			MX_SST_UTFALL,				// Search string is encoded in any UTF format.
			MX_SST_RAW,					// Search for the raw string as given.
			MX_SST_REGEX,				// RegEx search.
		};

		// String search flags.
		enum MX_STRING_SEARCH_FLAGS : uint32_t {
			MX_SSF_LINGUISTIC_IGNORECASE							= LINGUISTIC_IGNORECASE,
			MX_SSF_LINGUISTIC_IGNOREDIACRITIC						= LINGUISTIC_IGNOREDIACRITIC,
			MX_SSF_NORM_IGNORECASE									= NORM_IGNORECASE,
			MX_SSF_NORM_IGNOREKANATYPE								= NORM_IGNOREKANATYPE,
			MX_SSF_NORM_IGNORENONSPACE								= NORM_IGNORENONSPACE,
			MX_SSF_NORM_IGNORESYMBOLS								= NORM_IGNORESYMBOLS,
			MX_SSF_NORM_IGNOREWIDTH									= NORM_IGNOREWIDTH,
		};

		// Extra search flags.
		enum MX_STRING_SEARCH_FLAGS_EX : uint32_t {
			MX_SSF_WHOLE_WORD										= (1 << 0),
			MX_SSF_WILDCARD											= (1 << 1),
		};

		// RegEx options.
		enum MX_REGEX_OPTIONS : uint32_t {
			MX_RO_SINGLE_LINE										= (1 << 0),
			MX_RO_MULTI_LINE										= (1 << 1),
			MX_RO_EXTENDED											= (1 << 2),
			MX_RO_FIND_LONGEST										= (1 << 3),
			MX_RO_NEGATE_SINGE_LINE									= (1 << 4),
			MX_RO_IGNORE_CASE										= (1 << 5),
		};

		// RegEx flavors.
		enum MX_REGEX_FLAVORS : uint32_t {
			MX_RF_ASIS,
			MX_RF_POSIX_BASIC,
			MX_RF_POSIX_EXTENDED,
			MX_RF_EMACS,
			MX_RF_GREP,
			MX_RF_GNU_REGEX,
			MX_RF_JAVA,
			MX_RF_PERL,
			MX_RF_PERL_NG,
			MX_RF_RUBY,
			MX_RF_ONIGURUMA,
		};

		// RegEx encodings.
		enum MX_REGEX_ENCODING : uint32_t {
			MX_RE_ASCII,
			MX_RE_UTF8,
			MX_RE_UTF16_BE,
			MX_RE_UTF16_LE,
			MX_RE_UTF32_BE,
			MX_RE_UTF32_LE,
			MX_RE_ISO_8859_1,
			MX_RE_ISO_8859_2,
			MX_RE_ISO_8859_3,
			MX_RE_ISO_8859_4,
			MX_RE_ISO_8859_5,
			MX_RE_ISO_8859_6,
			MX_RE_ISO_8859_7,
			MX_RE_ISO_8859_8,
			MX_RE_ISO_8859_9,
			MX_RE_ISO_8859_10,
			MX_RE_ISO_8859_11,
			MX_RE_ISO_8859_13,
			MX_RE_ISO_8859_14,
			MX_RE_ISO_8859_15,
			MX_RE_ISO_8859_16,
			MX_RE_EUC_JP,
			MX_RE_EUC_TW,
			MX_RE_EUC_KR,
			MX_RE_EUC_CN,
			MX_RE_S_JIS,
			MX_RE_KOI_8,
			MX_RE_KOI8_R,
			MX_RE_WINDOWS_1251,
			MX_RE_BIG_5,
			MX_RE_GB_18030,
		};

		// Wildcard usage.
		enum MX_WILDCARD_USAGE : uint32_t {
			MX_WU_NONE,
			MX_WU_FULL,
			MW_WU_SOME
		};

		// Decoding methods.
		enum MX_DECODING {
			MX_D_STANDARD,
			MX_D_EXTENDED,
			MX_D_TIME,
		};

		// Byteswapping options.
		enum MX_BYTESWAP : uint32_t {
			MX_BS_NONE,
			MX_BS_2BYTE,
			MX_BS_4BYTE,
			MX_BS_8BYTE,
			MX_BS_BY_TYPE,
		};


		// == Types.
		// Various options.
		struct MX_UTIL_OPTIONS {
			BOOL						bUse0xForHex;				// 0xOOOO vs. OOOOh
			BOOL						bUse0oForOct;				// 0oOOOO vs. 0OOOO
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
			DWORD						dwAlign;
			BOOL						bIsFloat;
			BOOL						bIsSigned;
		};

		// Any data type.
		struct MX_DATA_TYPE {
			union {
				int8_t					Int8;
				int16_t					Int16;
				int32_t					Int32;
				int64_t					Int64;
				uint8_t					UInt8;
				uint16_t				UInt16;
				uint32_t				UInt32;
				uint64_t				UInt64;
				float					Float32;
				float					Float;
				double					Float64;
				double					Double;
				uint32_t				Pointer32;
				uint64_t				Pointer64;
			}							u;
			MX_DATA_TYPES				dtType;
		};

		// A helper for reliably pausing and unpausing a process.
		struct MX_PROCESS_PAUSE_HELPER {
			MX_PROCESS_PAUSE_HELPER( class CProcess * _ppProc );
			~MX_PROCESS_PAUSE_HELPER();

			LONG						lResult;

		protected :
			class CProcess *			ppProc;
		};

		// Data about a character for a string search.
		struct MX_CHAR_DATA {
			uint32_t					ui32SearchChar;				// The raw UTF-32 version of the character, which may be 
																	//	a capitalization, change in punctuation, etc. from the
																	//	original.
			uint8_t						ui8UftLen;					// Number of characters in this character as UTF-*.
		};

		// Boyor Moore data.
		struct MX_BOYOR_MOORE {
			std::vector<int32_t>		vBadShifts;
		};

		/** Temporarily setting the floating-point rounding mode. */
		struct MX_FEROUNDMODE {
			MX_FEROUNDMODE( int _iNewMode ) :
				iPrevMode( ::fegetround() ) {
				::fesetround( _iNewMode );
			}
			~MX_FEROUNDMODE() {
				::fesetround( iPrevMode );
			}

			int							iPrevMode;					/**< The previous rounding mode. */
		};


		// == Functions.
		/**
		 * Convert a single byte to hex.  _pcString must be at least 3 characters long.
		 * 
		 * \param _bIn The 8-bit value to convert to a 2-character (plus NULL) string.
		 * \param _pcString The retun string, which must be long enough to contain the 2 characters and final NULL.
		 * \param _bLower If FALSE, capital letters are used for characters A-F.
		 **/
		static VOID						ByteToHex( BYTE _bIn, CHAR * _pcString, BOOL _bLower = FALSE );

		/**
		 * Convert a single byte to hex.
		 * 
		 * \param _bIn The 8-bit value to convert to a 2-character string.
		 * \param _sString The retun string.
		 * \param _bLower If FALSE, capital letters are used for characters A-F.
		 **/
		static VOID						ByteToHex( BYTE _bIn, std::string &_sString, BOOL _bLower = FALSE );

		/**
		 * Convert a byte array to a hex string.  _pcString must be twice as long as the input, plus 1 character for the NULL.
		 * 
		 * \param _pvIn The input array of bytes.
		 * \param _sLen The number of bytes to which _pvIn points.
		 * \param _pcString The retun string, which must be long enough to contain the 2 characters and final NULL.
		 * \param _bLower If FALSE, capital letters are used for characters A-F.
		 **/
		static VOID						BytesToHex( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower = FALSE );

		/**
		 * Convert a byte array to a hex string.
		 * 
		 * \param _pvIn The input array of bytes.
		 * \param _sLen The number of bytes to which _pvIn points.
		 * \param _sString The retun string.
		 * \param _bLower If FALSE, capital letters are used for characters A-F.
		 **/
		static VOID						BytesToHex( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower = FALSE );

		// Convert a byte array to a hex string.  _pcString must be 3 times as long as the input.
		static VOID						BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower = FALSE );

		// Convert a byte array to a hex string.
		static VOID						BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower = FALSE );

		// Convert a byte array to a hex string.
		static VOID						BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, std::string &_sString, const uint8_t * _puiMeta, size_t _sMetaLen, size_t _sMetaCharLen, BOOL _bLower = FALSE );

		// Convert a byte array to a hex string.
		static VOID						BytesToHexWithSpaces_MultiFormat( const VOID * _pvIn, size_t _sLen, std::string &_sString, const uint8_t * _puiMeta, size_t _sMetaLen, BOOL _bLower = FALSE );

		// Is the given character printable?  If not, it should be printed as a space or question mark.
		static BOOL						ByteIsPrintable( BYTE _bIn, BOOL _bPrintExtended = TRUE );

		// Convert a single byte to a printable character.  If _bPrintExtended is TRUE, characters above 127 are kept.
		static CHAR						ByteToPrintable( BYTE _bIn, BOOL _bPrintExtended = TRUE, BOOL _bUseQuestionMarks = FALSE );

		// Converts a byte to a C-string value.  _pcString must be at least 5 characters long.  Returns the number of characters written.
		static DWORD					ByteToCString( BYTE _bIn, CHAR * _pcString, BOOL _bLower = FALSE, BOOL _bExtended = TRUE );

		// Converts a byte to a C-string value.  Returns the number of characters written.
		static DWORD					ByteToCString( BYTE _bIn, std::string &_sString, BOOL _bLower = FALSE, BOOL _bExtended = TRUE );

		// Convert a byte array to a C string.  Returns the number of characters written.
		static DWORD					BytesToCString( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower = FALSE, BOOL _bExtended = TRUE );

		// Convert a byte array to a C string.  Returns the number of characters written.
		static DWORD					BytesToCString( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower = FALSE, BOOL _bExtended = TRUE );

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

		// Creates an SW_ string.
		static const CHAR *				ShowWindowToString( int _iVal, std::string &_sString, BOOL _bShort = -1 );

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

		// Creates a binary (0bxxxx) string from an integer value.
		static const CHAR *				ToBinary( uint64_t _uiValue, std::string &_sString, uint32_t _uiNumDigits );

		// Creates a binary (0bxxxx) string from an integer value.
		static const WCHAR *			ToBinary( uint64_t _uiValue, std::wstring &_sString, uint32_t _uiNumDigits );

		// Creates an octadecimal (\[0-7]{1-3}) string from an integer value.
		static const CHAR *				ToOct( uint64_t _uiValue, std::string &_sString, uint32_t _uiNumDigits );

		// Creates an octadecimal (\[0-7]{1-3}) string from an integer value.
		static const WCHAR *			ToOct( uint64_t _uiValue, std::wstring &_sString, uint32_t _uiNumDigits );

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
		static const CHAR *				ToDouble( double _dValue, std::string &_sString, int32_t _iSigDigits = 0 );

		// Creates a double string.
		static const WCHAR *			ToDouble( double _dValue, std::wstring &_sString, int32_t _iSigDigits = 0 );

		// Determines if any byte of the 4 in the given value are 0.
		static inline bool				AnyBytesAre0( uint32_t _uVal ) {
			return (((_uVal) - 0x01010101) & ~(_uVal) & 0x80808080) != 0;
		}

		// Determines if any byte of the 8 in the given value are 0.
		static inline bool				AnyBytesAre0( uint64_t _uVal ) {
			return (((_uVal) - 0x0101010101010101ULL) & ~(_uVal) & 0x8080808080808080ULL) != 0;
		}

		// Resizes a vector with the ability to return a bool to indicate success.
		template <typename  _tType>
		static inline bool				Resize( std::vector<_tType> &_vVector, size_t _sSize ) {
			try { _vVector.resize( _sSize ); }
			catch ( const std::bad_alloc /*& _eE*/ ) { return false;  }
			return true;
		}

		// Creates a string with the given data interpreted as a given type.
		static const WCHAR *			ToDataTypeString( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtType, std::wstring &_sString,
			bool _bMustPrintNumber = false );

		// Returns -1 if the given result cast to the given type is -inf, 1 if it is +inf, otherwise 0.
		static int32_t					DataTypeIsInf( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtType );

		// Returns true if the given result cast to the given type is nan.
		static bool						DataTypeIsNan( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtType );

		// Gets the size of a data type.
		static DWORD					DataTypeSize( CUtilities::MX_DATA_TYPES _dtType );

		// Gets the alignment of a data type.
		static DWORD					DataTypeAlign( CUtilities::MX_DATA_TYPES _dtType );

		// Is the data type a float type?
		static BOOL						DataTypeIsFloat( CUtilities::MX_DATA_TYPES _dtType );

		// Is the data type signed?
		static BOOL						DataTypeIsSigned( CUtilities::MX_DATA_TYPES _dtType );

		// Is the data type the max value for the given type?
		static BOOL						DataTypeIsMax( CUtilities::MX_DATA_TYPE _dtType );

		// Is the data type the max value for the given type?
		static BOOL						DataTypeIsMin( CUtilities::MX_DATA_TYPE _dtType );

		// Gets the range of a data type as a string.
		static const CHAR *				DataTypeRange( CUtilities::MX_DATA_TYPES _dtType, std::string &_sString );

		// Gets the range of a data type as a string.
		static const WCHAR *			DataTypeRange( CUtilities::MX_DATA_TYPES _dtType, std::wstring &_sString );

		// Prints the size of the given data type as a string.
		static const CHAR *				DataTypeSize( CUtilities::MX_DATA_TYPES _dtType, std::string &_sString );

		// Prints the size of the given data type as a string.
		static const WCHAR *			DataTypeSize( CUtilities::MX_DATA_TYPES _dtType, std::wstring &_sString );

		// Prints a data type given the options.
		static const CHAR *				PrintDataType( std::string &_sString, CUtilities::MX_DATA_TYPES _dtType, DWORD _dwOptions = DWINVALID );

		// Prints a data type given the options.
		static const WCHAR *			PrintDataType( std::wstring &_sString, CUtilities::MX_DATA_TYPES _dtType, DWORD _dwOptions = DWINVALID );

		// Converts a given value to a string given the type.
		static CSecureString &			DataTypeToString( const void * _pvData, CUtilities::MX_DATA_TYPES _dtType, CSecureString &_sRet, uint32_t _uiNumDigits = 0, int32_t _iSigDigits = 0 );

		// Converts a given value to a string given the type.
		static CSecureWString &			DataTypeToString( const void * _pvData, CUtilities::MX_DATA_TYPES _dtType, CSecureWString &_swsRet, uint32_t _uiNumDigits = 0, int32_t _iSigDigits = 0 );

		// Performs a more detailed conversion of a result to one of our data types.
		static ee::CExpEvalContainer::EE_RESULT
										DetailedConvertResult( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtType );

		// Converts a MX_REGEX_ENCODING value to an actual code page.
		static UINT						RegexCodePageToCodePage( MX_REGEX_ENCODING _reEncoding );

		// MX_SEARCH_TYPES value to a string.
		static const WCHAR *			SearchTypeToString( CUtilities::MX_SEARCH_TYPES _stType, std::wstring &_sString );

		// MX_EVAL_TYPES value to a string.
		static const WCHAR *			EvaluationTypeToString( CUtilities::MX_EVAL_TYPES _etType, std::wstring &_sString );

		// MX_SUB_EVAL_TYPES value to a string.
		static const WCHAR *			SubEvaluationTypeToString( CUtilities::MX_SUB_EVAL_TYPES _setType, std::wstring &_sString );

		// Converts an ee::CExpEvalContainer::EE_RESULT to a CUtilities::MX_DATA_TYPE with the given data type.
		static CUtilities::MX_DATA_TYPE	ExpEvalResultToDataType( const ee::CExpEvalContainer::EE_RESULT &_rRes, CUtilities::MX_DATA_TYPES _dtDataType );

		// Clears the internal temporary buffer (as a security measure).
		static VOID						ClearInternalBuffer();

		// Converts from UTF-8 to any single-byte code page.
		static CSecureString			ToCodePage( const std::string &_sIn, UINT _uiCodePage, bool * _pbError = nullptr );

		// Converts from any single-byte code page to UTF-8.
		static CSecureString			FromCodePage( const std::string &_sIn, UINT _uiCodePage, bool * _pbError = nullptr );

		// Maps a UTF-16 (wide character) string to a new character string. The new character string is not necessarily from a multibyte character set.
		static CSecureString			WideCharToMultiByte( UINT _uiCodePage,
			DWORD _dwFlags,
			const std::wstring &_wsString,
			LPCCH _lpDefaultChar,
			LPBOOL _lpUsedDefaultChar,
			DWORD * _pdwLastError = nullptr );

		// Maps a character string to a UTF-16 (wide character) string. The character string is not necessarily from a multibyte character set.
		static CSecureWString			MultiByteToWideChar( UINT _uiCodePage,
			DWORD _dwFlags,
			const std::string &_sString,
			DWORD * _pdwLastError = nullptr
		);

		// Determines if _dwFlags must be 0 for a call to MultiByteToWideChar().
		static bool						MultiByteWideChar_FlagsMustBe0( UINT _uiCodePage );

		// Normalizes characters of a text string according to Unicode 4.0 TR#15.
		static CSecureWString			NormalizeString( NORM_FORM _nfNormForm,
			const std::wstring &_wsString,
			DWORD * _pdwLastError = nullptr );

		// Adds escapes to only NULL characters.
		static CSecureWString			EscapeNulOnly( const CSecureWString &_swsInput, bool _bIncludeBackSlash );

		// Adds escapes to all unprintable characters.
		static CSecureWString			EscapeUnprintable( const CSecureWString &_swsInput, bool _bIncludeBackSlash, bool _bKeepNewline );

		// Adds escapes to all unprintable characters.
		static std::vector<uint32_t>	EscapeUnprintable( const std::vector<uint32_t> &_swsInput, bool _bIncludeBackSlash, bool _bKeepNewline );

		// Escapes all characters non-UTF.
		static CSecureString			EscapeAllNonUtf( const CSecureString &_ssInput, bool _bKeepNewline );

		// Escapes all characters.
		static CSecureString			EscapeAllW( const CSecureString &_ssInput, bool _bKeepNewline );

		// Escapes all characters.
		static CSecureWString			EscapeAllW( const CSecureWString &_swsInput, bool _bKeepNewline );

		// Escapes all characters.
		static std::vector<uint32_t>	EscapeAllW( const std::vector<uint32_t> &_swsInput, bool _bKeepNewline );

		// Escapes standard characters.
		static CSecureString			EscapeStandard( const CSecureString &_ssInput, bool _bKeepNewline );

		// Escapes standard characters.
		static CSecureWString			EscapeStandard( const CSecureWString &_swsInput, bool _bKeepNewline );

		// Escapes standard characters.
		static std::vector<uint32_t>	EscapeStandard( const std::vector<uint32_t> &_swsInput, bool _bKeepNewline );

		// Escapes standard characters in an ASCII string.
		static CSecureString			EscapeStandardAscii( const CSecureString &_ssInput, bool _bKeepNewline );

		// Escapes all non-JSON string characters.
		static CSecureWString			EscapeNonJson( const CSecureWString &_swsInput, bool _bKeepNewline );

		// Creates a string from a string in the form of an array of hex bytes.
		static CSecureString			FromHexString( const CSecureWString &_swsInput );

		// Strips whitespace from the beginning and end of the string.
		static std::string &			Strip( std::string &_sIn, bool _bIsUtf8 );

		// Upper-cases an ASCII string.
		static std::string				ToUpper( const std::string &_sInput );

		// Lower-cases an ASCII string.
		static std::string				ToLower( const std::string &_sInput );

		// Upper-cases a string.
		static CSecureWString			ToUpper( const CSecureWString &_swsInput );

		// Lower-cases a string.
		static CSecureWString			ToLower( const CSecureWString &_swsInput );

		// Converts Hiragana characters to Katakana.
		static CSecureWString			ToKatakana( const CSecureWString &_swsInput );

		// Converts Katakana characters to Hiragana.
		static CSecureWString			ToHiragana( const CSecureWString &_swsInput );

		// Converts from full-width characters to half-width.
		static CSecureWString			ToHalfwidth( const CSecureWString &_swsInput );

		// Converts several common numerics to the numerics in ASCII range.
		static CSecureWString			ToAsciiNumerics( const CSecureWString &_swsInput );

		// Windows resource type to string.
		static BOOL						ResourceTypeToString( uint64_t _uiId, CHAR * _pcRet );

		// Windows resource type to string.
		static BOOL						ResourceTypeToString( uint64_t _uiId, std::string &_sString );

		// Adds a \ to the end of a string if it does not already have one.
		template <typename _tType>
		static _tType &					FinishPath( _tType &_sString ) {
			if ( !_sString.size() || _sString[_sString.size()-1] != _tType::value_type( '\\' ) ) {
				_sString.push_back( _tType::value_type( '\\' ) );
			}
			return _sString;
		}

		// Generates a string of random characters.
		template <typename _tType>
		static _tType *					RandomString( _tType * _pcBuffer, SIZE_T _sSize ) {
			if ( !_pcBuffer || !_sSize ) { return _pcBuffer; }
			std::random_device rdRand;
			std::mt19937 mGen( rdRand() );
			std::uniform_int_distribution<> uidDistLower( _tType( 'a' ), _tType( 'z' ) );
			std::uniform_int_distribution<> uidDistUpper( _tType( 'A' ), _tType( 'Z' ) );
			std::uniform_int_distribution<> uidBool( 0, 3 );
			for ( SIZE_T I = 0; I < _sSize - 1; ++I ) {
				if ( uidBool( mGen ) == 0 ) {
					_pcBuffer[I] = _tType( uidDistUpper( mGen ) );
				}
				else {
					_pcBuffer[I] = _tType( uidDistLower( mGen ) );
				}
			}
			_pcBuffer[_sSize-1] = _tType( '\0' );
			return _pcBuffer;
		}

		// Gets the number of elements in DataTypeInfo.
		static size_t					DataTypeInfoLen();

		// Prints an ee::CExpEvalContainer::EE_RESULT value and appends it to _sString, then returns _sString.
		static std::wstring &			PrintExpResult( const ee::CExpEvalContainer::EE_RESULT &_rResult, std::wstring &_sString, int32_t _iDblSciPrec = -1, MX_DECODING _dDecoder = MX_D_STANDARD );

		// Prints an ee::CExpEvalContainer::EE_RESULT value.
		static std::wstring				PrintExpResult( const ee::CExpEvalContainer::EE_RESULT &_rResult, uint64_t _ui64Data, MX_DECODING _dDecoder = MX_D_STANDARD );

		/**
		 * Creates a string with _cReplaceMe replaced with _cWithMe inside _s16String.
		 *
		 * \param _s16String The string in which replacements are to be made.
		 * \param _cReplaceMe The character to replace.
		 * \param _cWithMe The character with which to replace _cReplaceMe.
		 * \return Returns the new string with the given replacements made.
		 */
		static std::u16string			Replace( const std::u16string &_s16String, char16_t _cReplaceMe, char16_t _cWithMe );

		/**
		 * Creates a string with _wcReplaceMe replaced with _wcWithMe inside _wsString.
		 *
		 * \param _wsString The string in which replacements are to be made.
		 * \param _wcReplaceMe The character to replace.
		 * \param _wcWithMe The character with which to replace _cReplaceMe.
		 * \return Returns the new string with the given replacements made.
		 */
		static std::wstring				Replace( const std::wstring &_wsString, wchar_t _wcReplaceMe, wchar_t _wcWithMe );

		/**
		 * Gets the current time in microseconds.
		 * 
		 * \return Returns the current time in microseconds.
		 **/
		static uint64_t					CurTimeInMicros();

		/**
		 * Gets the extension from a file path.
		 *
		 * \param _s16Path The file path whose extension is to be obtained.
		 * \return Returns a string containing the file extension.
		 */
		static std::u16string			GetFileExtension( const std::u16string &_s16Path );

		/**
		 * Gets the extension from a file path.
		 *
		 * \param _wsPath The file path whose extension is to be obtained.
		 * \return Returns a string containing the file extension.
		 */
		static std::wstring				GetFileExtension( const std::wstring &_wsPath );

		/**
		 * Removes the extension from a file path.
		 *
		 * \param _s16Path The file path whose extension is to be removed.
		 * \return Returns a string containing the file mname without the extension.
		 */
		static std::u16string			NoExtension( const std::u16string &_s16Path );

		/**
		 * Removes the extension from a file path.
		 *
		 * \param _wsPath The file path whose extension is to be removed.
		 * \return Returns a string containing the file mname without the extension.
		 */
		static std::wstring				NoExtension( const std::wstring &_wsPath );

		/**
		 * Gets the file name from a file path.
		 *
		 * \param _s16Path The file path whose name is to be obtained.
		 * \return Returns a string containing the file name.
		 */
		static std::u16string			GetFileName( const std::u16string &_s16Path );

		/**
		 * Gets the file name from a file path.
		 *
		 * \param _wsPath The file path whose name is to be obtained.
		 * \return Returns a string containing the file name.
		 */
		static std::wstring				GetFileName( const std::wstring &_wsPath );

		/**
		 * Gets the file path without the file name
		 *
		 * \param _s16Path The file path whose path is to be obtained.
		 * \return Returns a string containing the file path.
		 */
		static std::u16string			GetFilePath( const std::u16string &_s16Path );

		/**
		 * Gets the file path without the file name
		 *
		 * \param _wsPath The file path whose path is to be obtained.
		 * \return Returns a string containing the file path.
		 */
		static std::wstring				GetFilePath( const std::wstring &_wsPath );

		/**
		 * Inserts a number of random characters into a string.  Returns _wsString.
		 * 
		 * \param _wsString The string to update.
		 * \param _stTotal Number of insertions to make.
		 * \param _pwcTable The table of characters that can be inserted.
		 * \param _stTableSize The number of characters in _pwcTable.
		 * \return Returns the updated input, _wsString.
		 **/
		static std::wstring				InsertRandomChars( std::wstring &_wsString, size_t _stTotal, const wchar_t * _pwcTable, size_t _stTableSize );

		/**
		 * Replaces a number of random characters in a string with characters randomly chosen from _pwcTable.  Returns _wsString.
		 * 
		 * \param _wsString The string to update.
		 * \param _stTotal Number of replacements to make.
		 * \param _pwcTable The table of characters that can be inserted.
		 * \param _stTableSize The number of characters in _pwcTable.
		 * \return Returns the updated input, _wsString.
		 **/
		static std::wstring				ReplaceRandomChars( std::wstring &_wsString, size_t _stTotal, const wchar_t * _pwcTable, size_t _stTableSize );

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
		static void						ResolveAllEscapes( const std::string &_sInput, std::vector<uint32_t> &_vOutput );

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
		static void						ResolveAllEscapes( const std::string &_sInput, std::string &_sOutput, bool _bIsUtf8 );

		// Resolves a single escape character, or returns the first input character if not an escape character.
		static uint64_t					ResolveEscape( const char * _pcInput, size_t _sLen, size_t &_sCharLen, bool _bIncludeHtml );

		// Resolves HTML/XML characters.
		// &#nnnn;
		// &#xhhhh;
		static void						ResolveAllHtmlXmlEscapes( const std::string &_sInput, std::string &_sOutput, bool _bIsUtf8 );

		// Converts a string in the form of sequential bytes in the given base to an array stored as a string.
		static CSecureString			NumberStringToString( const std::string &_sInput, int _iBase, uint64_t _ui64MaxSingleValue, MX_DATA_TYPES _dtTargetType,
			std::vector<uint8_t> * _pvMeta );

		// Converts a string in the form of sequential bytes in variable formats to an array stored as a string.
		static CSecureString			NumberStringToString_MultiFormat( const std::string &_sInput, int _iBase, MX_DATA_TYPES _dtTargetType,
			std::vector<uint8_t> * _pvMeta );

		// Creates a number string given an array of data.
		static CSecureString			PrimitiveArrayToStringA( const void * _pvData, size_t _sLenInBytes, MX_DATA_TYPES _dtTargetType, uint32_t _uiNumDigits = 0, int32_t _iSigDigits = 0 );

		// Creates a number string given an array of data.
		static CSecureWString			PrimitiveArrayToStringW( const void * _pvData, size_t _sLenInBytes, MX_DATA_TYPES _dtTargetType, uint32_t _uiNumDigits = 0, int32_t _iSigDigits = 0 );

		// Converts Katakana characters to Hiragana or returns the original input character.
		static uint32_t					KatakanaToHiragana( uint32_t _uiChar ) {
			//return ::towctrans( _uiChar, ::wctrans( "tojhira" ) );
			if ( _uiChar >= 0x30A1 && _uiChar <= 0x30F6 ) {
				return _uiChar - (0x30A0 - 0x3040);
			}
			return _uiChar;
		}

		// Converts Hiragana characters to Katakana or returns the original input character.
		static uint32_t					HiraganaToKatakana( uint32_t _uiChar ) {
			if ( _uiChar >= 0x3041 && _uiChar <= 0x3096 ) {
				return _uiChar + (0x30A0 - 0x3040);
			}
			return _uiChar;
		}

		// Converts a full-width UTF-16 alphanumeric character to a single-byte UTF-8 character.
		static uint32_t					FullWidthToByte( uint32_t _uiChar ) {
			if ( _uiChar >= 0xFF21 && _uiChar <= 0xFF3A ) {
				return _uiChar - (0xFF21 - u8'A');
			}
			if ( _uiChar >= 0xFF41 && _uiChar <= 0xFF5A ) {
				return _uiChar - (0xFF41 - u8'a');
			}
			if ( _uiChar >= 0xFF10 && _uiChar <= 0xFF19 ) {
				return _uiChar - (0xFF10 - u8'0');
			}
			switch ( _uiChar ) {
				//case 0x2018 : { return 0x91 /*'‘'*/; }	// 0080
				//case 0x2019 : { return 0x92 /*'’'*/; }	// 0080
				//case 0x201C : { return 0x93 /*'“'*/; }	// 0080
				//case 0x201D : { return 0x94 /*'”'*/; }	// 0080
				
				case 0x2018 : { return u8'\''; }	// 0080
				case 0x2019 : { return u8'\''; }	// 0080
				case 0x201C : { return u8'"'; }	// 0080
				case 0x201D : { return u8'"'; }	// 0080

				case 0x3000 : { return u8' '; }	// 0080
				//case 0x30FB : { return u8'·'; }	// 0090
				case 0x30FB : { return u8'.'; }	// 0090
				case 0xFF01 : { return u8'!'; }	// 0080
				case 0xFF02 : { return u8'"'; }	// 0080
				case 0xFF03 : { return u8'#'; }	// 0080
				case 0xFF04 : { return u8'$'; }	// 0488
				case 0xFF05 : { return u8'%'; }	// 0080
				case 0xFF06 : { return u8'&'; }	// 0080
				case 0xFF07 : { return u8'\''; }// 0480
				case 0xFF08 : { return u8'('; }	// 0080
				case 0xFF09 : { return u8')'; }	// 0080
				case 0xFF0A : { return u8'*'; }	// 0080
				case 0xFF0B : { return u8'+'; }	// 0088
				case 0xFF0C : { return u8','; }	// 0080
				case 0xFF0D : { return u8'-'; }	// 0480
				case 0xFF0E : { return u8'.'; }	// 0080
				case 0xFF0F : { return u8'/'; }	// 0480
				case 0xFF1A : { return u8':'; }	// 0080
				case 0xFF1B : { return u8';'; }	// 0080
				case 0xFF1C : { return u8'<'; }	// 0088
				case 0xFF1D : { return u8'='; }	// 0488
				case 0xFF1E : { return u8'>'; }	// 0088
				case 0xFF1F : { return u8'?'; }	// 0088
				case 0xFF20 : { return u8'@'; }	// 0480
				case 0xFF3B : { return u8'['; }	// 0080
				case 0xFF3C : { return u8'\\'; }// 0480
				case 0xFF3D : { return u8']'; }	// 0080
				case 0xFF3E : { return u8'^'; }	// 048A
				case 0xFF3F : { return u8'_'; }	// 0080
				case 0xFF40 : { return u8'`'; }	// 048A
				case 0xFF5B : { return u8'{'; }	// 0080
				case 0xFF5C : { return u8'|'; }	// 0088
				case 0xFF5D : { return u8'}'; }	// 0080
				case 0xFF5E : { return u8'~'; }	// 0488

				// Manually sourced.
				/*case 0x3002 : { return u8'.'; }
				case 0xFF61 : { return u8'.'; }
				case 0xFE12 : { return u8'.'; }*/
			}
			return _uiChar;
		}

		// Converts a UTF-16 numeric character to a single-byte UTF-8 character.
		static uint32_t					NumericToByte( uint32_t _uiChar ) {
			if ( _uiChar >= 0xFF10 && _uiChar <= 0xFF19 ) {
				return _uiChar - (0xFF10 - u8'0');
			}
			if ( _uiChar >= 0x2080 && _uiChar <= 0x2089 ) {
				return _uiChar - (0x2080 - u8'0');
			}
			switch ( _uiChar ) {
				//case 0x2070 : { return 0xB0 /*'°'*/; }	// 0003
				//case 0x2070 : { return u8'°'; }	// 0003
				case 0x2074 : { return u8'4'; }	// 0003
				case 0x2075 : { return u8'5'; }	// 0003
				case 0x2076 : { return u8'6'; }	// 0003
				case 0x2077 : { return u8'7'; }	// 0003
				case 0x2078 : { return u8'8'; }	// 0003
			}
			return _uiChar;
		}

		// Converts a UTF-16 value to upper-case.
		static uint32_t					WideToUpper( uint32_t _uiChar ) {
			return _uiChar <= 0xFF ? ::_mbctoupper( _uiChar ) : std::towupper( _uiChar );
		}

		// Converts a UTF-16 value to lower-case.
		static uint32_t					WideToLower( uint32_t _uiChar ) {
			return _uiChar <= 0xFF ? _mbctolower( _uiChar ) : std::towlower( _uiChar );
		}

		// Converts Katakana characters to Hiragana across a UTF-8 string.
		static CSecureString			KatakanaToHiragana( const std::string &_sString );

		// Converts Katakana characters to Hiragana across a UTF-16 string.
		static CSecureWString			KatakanaToHiragana( const std::wstring &_wsString );

		// Breaks an input string into an array of strings.
		static std::vector<CSecureString>
										StringToLines( const std::string &_sInput );

		// Creates meta data for a given series of 8-bit ANSI characters.  The flags determine what metadata to generate.
		static bool						BuildStringDatabaseAnsi( const uint8_t * _puiData, size_t _sBytes,
			uint32_t _uiFlags, uint32_t _uiFlagsEx,
			std::vector<WORD> &_wC1Props, std::vector<WORD> &_wC2Props, std::vector<WORD> &_wC3Props );

		// Creates meta data for a given series of UTF-8 characters.  The flags determine what metadata to generate.
		static bool						BuildStringDatabaseUtf8( const uint8_t * _puiData, size_t _sBytes,
			uint32_t _uiFlags, uint32_t _uiFlagsEx,
			std::vector<WORD> &_wC1Props, std::vector<WORD> &_wC2Props, std::vector<WORD> &_wC3Props );

		// Creates meta data for a given buffer of of UTF-16 characters.  The flags determine what metadata to generate.
		static bool						BuildStringDatabaseUtf16( const uint8_t * _puiData, size_t _sBytes,
			uint32_t _uiFlags, uint32_t _uiFlagsEx,
			std::vector<WORD> &_wC1Props, std::vector<WORD> &_wC2Props, std::vector<WORD> &_wC3Props );

		// Creates meta data for a given buffer of of UTF-32 characters.  The flags determine what metadata to generate.
		static bool						BuildStringDatabaseUtf32( const uint8_t * _puiData, size_t _sBytes,
			uint32_t _uiFlags, uint32_t _uiFlagsEx,
			std::vector<WORD> &_wC1Props, std::vector<WORD> &_wC2Props, std::vector<WORD> &_wC3Props );

		// Given a set of search flags, determines if CT_CTYPE1 information should be generated for a data set.
		static bool						NeedsType1( uint32_t uiFlags, uint32_t _uiFlagsEx ) {
			return (uiFlags & MX_SSF_NORM_IGNORESYMBOLS) ||		// C1_PUNCT to set all punctuation marks to a period.
				(_uiFlagsEx & MX_SSF_WHOLE_WORD)
				;
		}

		// Given a set of search flags, determines if CT_CTYPE2 information should be generated for a data set.
		static bool						NeedsType2( uint32_t uiFlags ) {
			return false;
		}

		// Given a set of search flags, determines if CT_CTYPE3 information should be generated for a data set.
		static bool						NeedsType3( uint32_t uiFlags ) {
			return (uiFlags & MX_SSF_LINGUISTIC_IGNOREDIACRITIC) ||			// For C3_DIACRITIC.
				(uiFlags & MX_SSF_NORM_IGNORESYMBOLS) ||					// C3_SYMBOL to set all symbols to a period.
				(uiFlags & MX_SSF_NORM_IGNORENONSPACE)						// For C3_NONSPACING.
				;
		}

		// Composes the given data into something searchable.
		static bool						ComposeDataIntoSearchableStringAnsi( const uint8_t * _puiData, size_t _sBytes,
			uint32_t _uiFlags, uint32_t _uiFlagsEx,
			const std::vector<WORD> &_wC1Props,
			const std::vector<WORD> &_wC3Props,
			std::vector<MX_CHAR_DATA> &_vData,
			bool _bAligned,
			bool _bByteSwapped );

		// Composes the given data into something searchable.
		static bool						ComposeDataIntoSearchableStringUtf8( const uint8_t * _puiData, size_t _sBytes,
			uint32_t _uiFlags, uint32_t _uiFlagsEx,
			const std::vector<WORD> &_wC1Props,
			const std::vector<WORD> &_wC3Props,
			std::vector<MX_CHAR_DATA> &_vData,
			bool _bAligned,
			bool _bByteSwapped );

		// Composes the given data into something searchable.
		static bool						ComposeDataIntoSearchableStringUtf16( const uint8_t * _puiData, size_t _sBytes,
			uint32_t _uiFlags, uint32_t _uiFlagsEx,
			const std::vector<WORD> &_wC1Props,
			const std::vector<WORD> &_wC3Props,
			std::vector<MX_CHAR_DATA> &_vData,
			bool _bAligned,
			bool _bByteSwapped );

		// Composes the given data into something searchable.
		static bool						ComposeDataIntoSearchableStringUtf32( const uint8_t * _puiData, size_t _sBytes,
			uint32_t _uiFlags, uint32_t _uiFlagsEx,
			const std::vector<WORD> &_wC1Props,
			const std::vector<WORD> &_wC3Props,
			std::vector<MX_CHAR_DATA> &_vData,
			bool _bAligned,
			bool _bByteSwapped );

		// Prints the number of GUI objects.
		static void						PrintTotalGuiObjects( DWORD _dwFlags );

		// Moves the selected items in a vector up (closer to 0) by 1.
		template <typename _tType>
		static void						MoveElementsUp( std::vector<_tType> &_vElements, const std::vector<size_t> &_vSelection ) {
			size_t sLowestIdx = -1;
			for ( size_t I = 0; I < _vSelection.size(); ++I ) {
				if ( _vSelection[I] < _vElements.size() ) {
					if ( static_cast<intptr_t>(_vSelection[I]) > static_cast<intptr_t>(sLowestIdx) && _vSelection[I] > 0 ) {
						std::swap( _vElements[_vSelection[I]-1], _vElements[_vSelection[I]] );
						sLowestIdx = _vSelection[I];
					}
					else {
						sLowestIdx = _vSelection[I] + 1;
					}
				}
			}
		}

		// Moves the selected items in a vector down (away from 0) by 1.
		template <typename _tType>
		static void						MoveElementsDown( std::vector<_tType> &_vElements, const std::vector<size_t> &_vSelection ) {
			size_t sHighestIdx = _vSelection.size();
			for ( size_t I = _vSelection.size(); I--; ) {
				if ( _vSelection[I] < _vElements.size() ) {
					if ( _vSelection[I] < sHighestIdx && _vSelection[I] < _vElements.size() - 1 ) {
						std::swap( _vElements[_vSelection[I]], _vElements[_vSelection[I]+1] );
						sHighestIdx = _vSelection[I];
					}
					else {
						sHighestIdx = _vSelection[I] - 1;
					}
				}
			}
		}

		// The From and To address ranges.
		static void						FillAddressRangeComboBoxes( lsw::CComboBox * _pcbFrom, lsw::CComboBox * _pcbTo, const void * _poOptions );

		// Verifies the address-range combo boxes.  A return of 0 indicates success, a return of 1 indicates the From box had errors, and 2 indicates the To box had errors.
		static uint8_t					VerifyAddressRangeComboBoxes( lsw::CComboBox * _pcbFrom, lsw::CComboBox * _pcbTo );

		// Updates the options with the given range-address combo boxes.
		static void						UpdateAddressRangeComboBoxes( lsw::CComboBox * _pcbFrom, lsw::CComboBox * _pcbTo, void * _poOptions );
		

		// == Members.
		// Options.
		static MX_UTIL_OPTIONS			Options;

		// Data-type information.
		static MX_DATA_TYPE_INFO		DataTypeInfo[];

	protected :
		// == Members.
		// Internal buffer for temporary strings.
		static CHAR						m_szTemp[128];

		// Buffer for printing a float.
		static wchar_t					m_szFloatPrintBuffer[2000+309+3];

		// The critical section.
		static lsw::CCriticalSection	m_csCrit;


		// == Functions.
		// Prints an ee::CExpEvalContainer::EE_RESULT value.
		static std::wstring				PrintExpResultStandard( const ee::CExpEvalContainer::EE_RESULT &_rResult, uint64_t _ui64Data, bool _bClosingPar = true );

		// Prints an ee::CExpEvalContainer::EE_RESULT value with extended print-outs.
		static std::wstring				PrintExpResultExtended( const ee::CExpEvalContainer::EE_RESULT &_rResult, uint64_t _ui64Data, bool _bClosingPar = true );

		// Prints an ee::CExpEvalContainer::EE_RESULT value as a date/time.
		static std::wstring				PrintExpResultDataTime( const ee::CExpEvalContainer::EE_RESULT &_rResult, uint64_t _ui64Data, bool _bClosingPar = true );
	};

}	// namespace mx

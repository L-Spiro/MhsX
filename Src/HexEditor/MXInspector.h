#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXSecureWString.h"

#include <EEExpEvalContainer.h>

#include <cstdint>
#include <string>
#include <vector>

namespace mx {

	/**
	 * Converts the bytes at the caret into the data types shown in the Inspector, and the text typed into the Inspector back into
	 *	bytes.
	 **/
	class CInspector {
	public :
		// == Enumerations.
		/** The data types the Inspector can show.  The values are stable (they identify the types in saved settings). */
		enum MX_INSPECTOR_TYPE : uint32_t {
			MX_IT_BINARY,																		/**< 8 bits shown as binary digits. */
			MX_IT_INT8,																			/**< Signed Byte. */
			MX_IT_UINT8,																		/**< Unsigned Byte. */
			MX_IT_INT16,																		/**< Signed Short. */
			MX_IT_UINT16,																		/**< Unsigned Short. */
			MX_IT_INT32,																		/**< Signed Int. */
			MX_IT_UINT32,																		/**< Unsigned Int. */
			MX_IT_INT64,																		/**< Signed Int64. */
			MX_IT_UINT64,																		/**< Unsigned Int64. */
			MX_IT_FLOAT,																		/**< 32-bit float. */
			MX_IT_DOUBLE,																		/**< 64-bit float. */
			MX_IT_FLOAT16,																		/**< 16-bit float. */
			MX_IT_VECTOR4,																		/**< 4 32-bit floats (X, Y, Z, W). */
			MX_IT_STRING,																		/**< Null-terminated ASCII string (up to MX_MAX_STRING_CHARS characters). */
			MX_IT_STRING_UTF16,																	/**< Null-terminated UTF-16 string, in the byte order of the view. */
			MX_IT_STRING_UTF8,																	/**< Null-terminated UTF-8 string. */
			MX_IT_DOSDATE,																		/**< 16-bit MS-DOS date. */
			MX_IT_DOSTIME,																		/**< 16-bit MS-DOS time. */
			MX_IT_FILETIME,																		/**< 64-bit count of 100-nanosecond intervals since 1601 (UTC). */
			MX_IT_OLETIME,																		/**< 64-bit float count of days since December 30, 1899. */
			MX_IT_TIME_T,																		/**< 32-bit signed count of seconds since 1970 (UTC). */
			MX_IT_TIME64_T,																		/**< 64-bit signed count of seconds since 1970 (UTC). */
			MX_IT_GUID,																			/**< 16-byte GUID. */
			MX_IT_BFLOAT16,																		/**< 16-bit brain float (the top 16 bits of a 32-bit float). */

			MX_IT_TOTAL
		};

		/** How integers are shown in the Value column. */
		enum MX_NUMBER_FORMAT : uint32_t {
			MX_NF_DECIMAL,																		/**< Decimal. */
			MX_NF_HEX,																			/**< Hexadecimal, in the hex style of the settings. */
			MX_NF_OCTAL,																		/**< Octal. */
			MX_NF_BINARY,																		/**< Binary. */
		};

		/** How hexadecimal numbers are written. */
		enum MX_HEX_STYLE : uint32_t {
			MX_HS_SUFFIX_H,																		/**< FFh. */
			MX_HS_PREFIX_0X,																	/**< 0xFF. */
			MX_HS_NONE,																			/**< FF. */
		};


		// == Types.
		/** A row of the Inspector. */
		struct MX_ROW_SETTING {
			MX_INSPECTOR_TYPE												itType = MX_IT_BINARY;	/**< The data type. */
			bool															bShown = true;			/**< Is the row shown? */
		};

		/** Inspector settings. */
		struct MX_SETTINGS {
			std::vector<MX_ROW_SETTING>										vRows;					/**< The rows, in the order shown.  Every type appears once. */
			MX_NUMBER_FORMAT												nfFormat = MX_NF_DECIMAL;	/**< How integers are shown. */
			MX_HEX_STYLE													hsHexStyle = MX_HS_SUFFIX_H;	/**< How hexadecimal numbers are written. */
			std::wstring													wsDateFormat;			/**< The date format (Windows date picture).  Empty = the user's short date. */
			std::wstring													wsTimeFormat;			/**< The time format (Windows time picture).  Empty = the user's time format. */
			bool															bAllowDocking = true;	/**< Can the Inspector be docked? */
		};

		/** The data being inspected. */
		struct MX_DATA {
			const uint8_t *													pui8Data = nullptr;		/**< The bytes starting at the inspected address. */
			size_t															sSize = 0;				/**< The number of bytes at pui8Data. */
			bool															bBigEndian = false;		/**< Are values big-endian? */
		};


		// == Functions.
		/**
		 * Gets the default settings.
		 *
		 * \param _sSettings Receives the default settings.
		 **/
		static void															DefaultSettings( MX_SETTINGS &_sSettings );

		/**
		 * Makes sure that every type appears exactly once in a set of settings (types that are missing are added at the end, hidden,
		 *	and invalid or repeated types are removed).
		 *
		 * \param _sSettings The settings to fix.
		 **/
		static void															FixSettings( MX_SETTINGS &_sSettings );

		/**
		 * Gets the name of a type, as shown in the Type column.
		 *
		 * \param _itType The type.
		 * \return Returns the name of the type.
		 **/
		static CSecureWString												TypeName( MX_INSPECTOR_TYPE _itType );

		/**
		 * Gets the size of a type in bytes.
		 *
		 * \param _itType The type.
		 * \return Returns the size of the type, or 0 for strings (their sizes depend on the data).
		 **/
		static size_t														TypeSize( MX_INSPECTOR_TYPE _itType );

		/**
		 * Determines whether a type is a string (its size depends on the data, and editing it can change the size of the data).
		 *
		 * \param _itType The type.
		 * \return Returns true if the type is a string.
		 **/
		static bool															IsString( MX_INSPECTOR_TYPE _itType ) {
			return _itType == MX_IT_STRING || _itType == MX_IT_STRING_UTF16 || _itType == MX_IT_STRING_UTF8;
		}

		/**
		 * Determines whether a type is an integer whose display follows the Column Display Format.
		 *
		 * \param _itType The type.
		 * \return Returns true if the type is a signed or unsigned integer.
		 **/
		static bool															IsInteger( MX_INSPECTOR_TYPE _itType ) {
			return _itType >= MX_IT_INT8 && _itType <= MX_IT_UINT64;
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
		static bool															Format( MX_INSPECTOR_TYPE _itType, const MX_DATA &_dData, const MX_SETTINGS &_sSettings,
			std::wstring &_wsText, size_t &_sBytes );

		/**
		 * Gets the value of an integer type as an unsigned number (used to offer "Go to Address").
		 *
		 * \param _itType The type.
		 * \param _dData The data.
		 * \param _ui64Value Receives the value.
		 * \return Returns true if the type is an integer type (including Binary), there is enough data, and the value is not negative.
		 **/
		static bool															UnsignedValue( MX_INSPECTOR_TYPE _itType, const MX_DATA &_dData, uint64_t &_ui64Value );

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
		static bool															Parse( MX_INSPECTOR_TYPE _itType, const std::wstring &_wsText, bool _bBigEndian,
			const MX_SETTINGS &_sSettings, std::vector<uint8_t> &_vBytes, CSecureWString &_swsError );

		/**
		 * Formats an unsigned number in a given number format.
		 *
		 * \param _ui64Value The value.
		 * \param _nfFormat The number format.
		 * \param _hsHexStyle How hexadecimal numbers are written.
		 * \param _wsText Receives the text (appended).
		 * \param _ui32Digits The fewest digits to write (hexadecimal and binary are padded with 0's).
		 **/
		static void															FormatUnsigned( uint64_t _ui64Value, MX_NUMBER_FORMAT _nfFormat, MX_HEX_STYLE _hsHexStyle, std::wstring &_wsText,
			uint32_t _ui32Digits = 1 );


		// == Members.
		/** The most characters shown for a string. */
		static constexpr size_t												MX_MAX_STRING_CHARS = 256;


		/**
		 * Converts a 32-bit float to a 16-bit brain float (rounded to nearest, ties to even).
		 *
		 * \param _fValue The value to convert.
		 * \return Returns the bits of the brain float.
		 **/
		static uint16_t														FloatToBFloat16( float _fValue );


	protected :
		// == Members.
		/** Milliseconds from January 1, 1601 to December 30, 1899 (the start of OLE dates). */
		static constexpr int64_t											MX_OLE_EPOCH_MS = 9435312000000LL;
		/** Seconds from January 1, 1601 to January 1, 1970 (the start of time_t). */
		static constexpr int64_t											MX_UNIX_EPOCH_SECONDS = 11644473600LL;
		/** The most seconds a FILETIME can count. */
		static constexpr int64_t											MX_MAX_FILETIME_SECONDS = 922337203685LL;


		// == Functions.
		/**
		 * Reads a value of up to 8 bytes in the given byte order.
		 *
		 * \param _pui8Data The bytes.
		 * \param _sSize The number of bytes to read (1 to 8).
		 * \param _bBigEndian Are the bytes big-endian?
		 * \return Returns the value.
		 **/
		static uint64_t														ReadUnsigned( const uint8_t * _pui8Data, size_t _sSize, bool _bBigEndian );

		/**
		 * Appends a value of up to 8 bytes in the given byte order.
		 *
		 * \param _ui64Value The value.
		 * \param _sSize The number of bytes to write (1 to 8).
		 * \param _bBigEndian Are the bytes big-endian?
		 * \param _vBytes Receives the bytes (appended).
		 **/
		static void															WriteUnsigned( uint64_t _ui64Value, size_t _sSize, bool _bBigEndian, std::vector<uint8_t> &_vBytes );

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
		static bool															FormatDateTime( const SYSTEMTIME &_stTime, bool _bDate, bool _bTime, const MX_SETTINGS &_sSettings,
			std::wstring &_wsText );

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
		static bool															ParseDateTime( const std::wstring &_wsText, bool _bDate, bool _bTime, const MX_SETTINGS &_sSettings,
			SYSTEMTIME &_stTime );

		/**
		 * Converts a FILETIME value in UTC to local time.
		 *
		 * \param _ui64FileTime The FILETIME value.
		 * \param _stLocal Receives the local time.
		 * \return Returns false if the value is out of range.
		 **/
		static bool															FileTimeToLocal( uint64_t _ui64FileTime, SYSTEMTIME &_stLocal );

		/**
		 * Converts a local time to a FILETIME value in UTC.
		 *
		 * \param _stLocal The local time.
		 * \param _ui64FileTime Receives the FILETIME value.
		 * \return Returns false if the time is invalid.
		 **/
		static bool															LocalToFileTime( const SYSTEMTIME &_stLocal, uint64_t &_ui64FileTime );

		/**
		 * Evaluates text as an expression.
		 *
		 * \param _wsText The text.
		 * \param _bHex Are numbers without a prefix or suffix hexadecimal?
		 * \param _rResult Receives the result.
		 * \return Returns true if the text is a valid expression with a numeric result.
		 **/
		static bool															Evaluate( const std::wstring &_wsText, bool _bHex, ee::CExpEvalContainer::EE_RESULT &_rResult );

		/**
		 * Converts a character to the form shown in the Value column (control characters are shown as dots).
		 *
		 * \param _ui32Char The character (UTF-32).
		 * \param _wsText Receives the character (appended).
		 **/
		static void															AppendShownChar( uint32_t _ui32Char, std::wstring &_wsText );
	};


	/**
	 * Supplies the Inspector with data and carries out what is done in it.
	 **/
	class CInspectorHost {
	public :
		// == Types.
		/** The data at the inspected address. */
		struct MX_INSPECTOR_SOURCE {
			uint64_t														ui64Address = 0;		/**< The inspected address (selection start or caret), relative to the data. */
			uint64_t														ui64StartAddress = 0;	/**< The custom starting address (added to addresses shown). */
			uint64_t														ui64Size = 0;			/**< The size of the data. */
			std::vector<uint8_t>											vBytes;					/**< The bytes starting at ui64Address. */
			bool															bBigEndian = false;		/**< Are values big-endian? */
			bool															bReadOnly = true;		/**< Is the data read-only? */
		};


		// == Functions.
		/**
		 * Gets the data at the inspected address of the current view.
		 *
		 * \param _isSource Receives the data.
		 * \param _sMaxBytes The most bytes to read.
		 * \return Returns false if there is no data to inspect.
		 **/
		virtual bool														InspectorSource( MX_INSPECTOR_SOURCE &_isSource, size_t _sMaxBytes ) = 0;

		/**
		 * Writes a value typed into the Inspector.
		 *
		 * \param _ui64Address The address of the value, relative to the data.
		 * \param _ui64OldSize The number of bytes the value currently uses.
		 * \param _vBytes The new bytes.
		 * \param _bString If true, the value is a string: in Insert mode the old bytes are replaced by the new bytes (the size of the data
		 *	can change); in Overwrite mode, if the new string is shorter, 0's are written over the rest of the old one.  Otherwise the
		 *	new bytes are written over the old ones.
		 * \param _swsStatus Receives a message for the status bar.
		 * \return Returns true if the value was written.
		 **/
		virtual bool														InspectorWrite( uint64_t _ui64Address, uint64_t _ui64OldSize, const std::vector<uint8_t> &_vBytes,
			bool _bString, CSecureWString &_swsStatus ) = 0;

		/**
		 * Moves the caret of the current view to an address.
		 *
		 * \param _ui64Address The address, relative to the data.
		 **/
		virtual void														InspectorGoTo( uint64_t _ui64Address ) = 0;

		/**
		 * Selects bytes in the current view (the bytes of a value clicked in the Inspector).  The start of the selection (the
		 *	inspected address) does not change.
		 *
		 * \param _ui64Address The address of the first byte, relative to the data.
		 * \param _ui64Size The number of bytes.
		 **/
		virtual void														InspectorSelect( uint64_t _ui64Address, uint64_t _ui64Size ) = 0;

		/**
		 * Shows a message in the status bar.
		 *
		 * \param _swsText The message.
		 * \param _bWarning If true, the message is a warning.
		 **/
		virtual void														InspectorStatus( const CSecureWString &_swsText, bool _bWarning ) = 0;

		/**
		 * Gets the Inspector settings.
		 *
		 * \return Returns the Inspector settings.
		 **/
		virtual CInspector::MX_SETTINGS &									InspectorSettings() = 0;
	};

}	// namespace mx

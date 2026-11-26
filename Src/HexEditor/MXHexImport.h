#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXSecureWString.h"

#include <cstdint>
#include <vector>


namespace mx {

	/**
	 * Converts text produced by Export Hex (or by other tools) back into bytes (File -> Import Hex...).
	 **/
	class CHexImport {
	public :
		// == Enumerations.
		/** The kinds of text that can be imported. */
		enum MX_IMPORT_TYPE {
			MX_IT_AUTO,												/**< Decided from the contents. */
			MX_IT_HEX_TEXT,											/**< Pairs of hexadecimal digits ("0x" prefixes and any separators are allowed). */
			MX_IT_DECIMAL_TEXT,										/**< Decimal numbers from 0 to 255. */
			MX_IT_BINARY_TEXT,										/**< Groups of up to 8 binary digits. */
			MX_IT_SOURCE_CODE,										/**< A C, C++, Java, or Python array of byte values. */
			MX_IT_INTEL_HEX,										/**< Intel HEX records (8-, 16-, or 32-bit). */
			MX_IT_MOTOROLA,											/**< Motorola S-records (S19, S28, or S37). */
			MX_IT_BASE64,											/**< Base64. */
			MX_IT_UUENCODING,										/**< A uuencoded file. */
		};


		// == Types.
		/** The result of an import. */
		struct MX_IMPORT_RESULT {
			std::vector<uint8_t>									vData;												/**< The imported bytes. */
			uint64_t												ui64StartAddress = 0;								/**< The address of vData[0] (Intel HEX and Motorola files whose leading blank area was skipped). */
			MX_IMPORT_TYPE											itType = MX_IT_AUTO;								/**< The kind of text that was imported. */
		};

		/** Import settings. */
		struct MX_IMPORT_OPTIONS {
			uint8_t													ui8FillByte = 0;									/**< The value of bytes skipped by Intel HEX and Motorola files. */
			bool													bAutoStartAddress = true;							/**< Intel HEX and Motorola files skip their leading blank area and start at their first data byte. */
			bool													bWordAddresses = false;								/**< Intel HEX and Motorola addresses are in words (multiplied by 2). */
		};


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
		static bool													Import( const uint8_t * _pui8Text, size_t _sSize, MX_IMPORT_TYPE _itType, const MX_IMPORT_OPTIONS &_ioOptions,
			MX_IMPORT_RESULT &_irResult, CSecureWString &_swsError );

		/**
		 * Decides what kind of text something is.
		 *
		 * \param _pui8Text The text.
		 * \param _sSize The number of bytes at _pui8Text.
		 * \return Returns the kind of text (MX_IT_HEX_TEXT if nothing more specific fits).
		 **/
		static MX_IMPORT_TYPE										Detect( const uint8_t * _pui8Text, size_t _sSize );


	protected :
		// == Types.
		/** A run of bytes at an address (Intel HEX and Motorola). */
		struct MX_CHUNK {
			uint64_t												ui64Addr = 0;										/**< The address of the first byte. */
			std::vector<uint8_t>									vBytes;												/**< The bytes. */
		};


		// == Functions.
		/**
		 * Gets the value of a hexadecimal digit.
		 *
		 * \param _ui8Char The character.
		 * \return Returns the value, or -1 if the character is not a hexadecimal digit.
		 **/
		static inline int32_t										HexVal( uint8_t _ui8Char ) {
			if ( _ui8Char >= '0' && _ui8Char <= '9' ) { return _ui8Char - '0'; }
			if ( _ui8Char >= 'A' && _ui8Char <= 'F' ) { return _ui8Char - 'A' + 10; }
			if ( _ui8Char >= 'a' && _ui8Char <= 'f' ) { return _ui8Char - 'a' + 10; }
			return -1;
		}

		/**
		 * Splits text into lines (without their line endings).
		 *
		 * \param _pui8Text The text.
		 * \param _sSize The number of bytes at _pui8Text.
		 * \param _vLines Receives the start and length of each line.
		 **/
		static void													SplitLines( const uint8_t * _pui8Text, size_t _sSize, std::vector<std::pair<size_t, size_t>> &_vLines );

		/**
		 * Imports pairs of hexadecimal digits.
		 *
		 * \param _pui8Text The text.
		 * \param _sSize The number of bytes at _pui8Text.
		 * \param _vData Receives the bytes.
		 * \return Returns false on a memory failure.
		 **/
		static bool													ImportHexText( const uint8_t * _pui8Text, size_t _sSize, std::vector<uint8_t> &_vData );

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
		static bool													ImportNumbers( const uint8_t * _pui8Text, size_t _sSize, uint32_t _ui32Base, std::vector<uint8_t> &_vData, size_t &_sBadLine );

		/**
		 * Imports the values of a C, C++, Java, or Python array.
		 *
		 * \param _pui8Text The text.
		 * \param _sSize The number of bytes at _pui8Text.
		 * \param _vData Receives the bytes.
		 * \param _sBadLine Receives the line (from 1) of the first invalid value on failure, or 0 for a memory failure.
		 * \return Returns false on failure.
		 **/
		static bool													ImportSourceCode( const uint8_t * _pui8Text, size_t _sSize, std::vector<uint8_t> &_vData, size_t &_sBadLine );

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
		static bool													ImportRecords( const uint8_t * _pui8Text, size_t _sSize, bool _bIntel, const MX_IMPORT_OPTIONS &_ioOptions,
			MX_IMPORT_RESULT &_irResult, CSecureWString &_swsError );

		/**
		 * Imports Base64.
		 *
		 * \param _pui8Text The text.
		 * \param _sSize The number of bytes at _pui8Text.
		 * \param _vData Receives the bytes.
		 * \param _sBadLine Receives the line (from 1) of the first invalid character on failure, or 0 for a memory failure.
		 * \return Returns false on failure.
		 **/
		static bool													ImportBase64( const uint8_t * _pui8Text, size_t _sSize, std::vector<uint8_t> &_vData, size_t &_sBadLine );

		/**
		 * Imports a uuencoded file.
		 *
		 * \param _pui8Text The text.
		 * \param _sSize The number of bytes at _pui8Text.
		 * \param _vData Receives the bytes.
		 * \param _sBadLine Receives the line (from 1) of the first invalid line on failure, or 0 for a memory failure.
		 * \return Returns false on failure.
		 **/
		static bool													ImportUuEncoding( const uint8_t * _pui8Text, size_t _sSize, std::vector<uint8_t> &_vData, size_t &_sBadLine );

		/**
		 * Makes the "invalid at line N" or "checksum is wrong at line N" error.
		 *
		 * \param _swsPrefix The start of the message.
		 * \param _sLine The line (from 1), or 0 for a memory failure.
		 * \param _swsError Receives the message.
		 **/
		static void													LineError( const CSecureWString &_swsPrefix, size_t _sLine, CSecureWString &_swsError );
	};

}	// namespace mx

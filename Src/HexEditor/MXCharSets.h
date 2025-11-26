#pragma once

#include "../MXMhsX.h"
#include "../CodePages/MXCodePages.h"
#include "../Utilities/MXUtilities.h"

#include <EEExpEval.h>

namespace mx {

	class CCharSets {
	public :
		// == Enumerations.
		/** The supported character sets. */
		enum MX_CHAR_SETS {
			MX_CS_ASCII,
			MX_CS_ASCII_ANSI,
			MX_CS_ASCII_OEM,
			MX_CS_EBCDIC,
			MX_CS_UNICODE,
			MX_CS_UTF8,
			MX_CS_MACINTOSH,
			MX_CS_ARABIC_WINDOWS,
			MX_CS_ARABIC_ISO,
			MX_CS_BALTIC_WINDOWS,
			MX_CS_BALTIC_ISO,
			MX_CS_CHINESE_SIMPLIFIED,
			MX_CS_CHINESE_TRADITIONAL,
			MX_CS_CYRILLIC_WINDOWS,
			MX_CS_CYRILLIC_KOI8_R,
			MX_CS_CYRILLIC_KOI8_U,
			MX_CS_CYRILLIC_ISO,
			MX_CS_EASTERN_EUROPE_WINDOWS,
			MX_CS_EASTERN_EUROPE_ISO,
			MX_CS_GREEK_WINDOWS,
			MX_CS_GREEK_ISO,
			MX_CS_HEBREW_WINDOWS,
			MX_CS_HEBREW_ISO,
			MX_CS_JAPANESE_JIS,
			MX_CS_JAPANESE_EUC_JP,
			MX_CS_KOREAN,
			MX_CS_THAI,
			MX_CS_TURKISH_WINDWOS,
			MX_CS_TURKISH_ISO,
			MX_CS_VIETNAMESE,
		};

		
		// == Types.
		/** A function for determining if the given character is displayable in the character set.  Returns 0 (not displyable) or the size of the character. */
		typedef uint32_t (*												PfDisaplyableChar)( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar );
		/** A character set. */
		struct MX_CHAR_SET {
			const char *												pcName = nullptr;								/** Name of the character set. Appears in the menu. */
			size_t														sNameLen = 0;									/** Length of the name. */
			const char *												pcStatusBar = nullptr;							/** Status-bar indicator. */
			size_t														sStatusLen = 0;									/** Status-bar indicator length. */
			PfDisaplyableChar											pfDisplayable = nullptr;						/** Function for determining if a byte or byte sequence can be displayed in the hex editor. */
			uint32_t													ui32Code = CCodePages::MX_Windows_1252;			/** Code page. */
			uint32_t													ui32Id = 0;										/** ID. */
			bool														bHideOver127 = true;							/** Hide values over 127. */
			bool														bTopMenu = false;								/** Show in the top-level menu. */
			bool														bSinglePage = true;								/** The whole character set is a single 8-bit page. */
		};


		// == Functions.
		// Gets the total number of presets.
		static size_t													Total();


		// == Display Functions.
		/**
		 * Determines if a character is displayable in ASCII/ANSI.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													AsciiANSIDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 32 || ui8Char == 127 ||
				ui8Char == 129 || ui8Char == 141 || ui8Char == 143 || ui8Char == 144 ||
				ui8Char == 157 || ui8Char == 173 ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcAsciiAnsi[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in ASCII/OEM.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													AsciiOemDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char == 0 ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcAsciiOEM[ui8Char];
			return 1;
		}

		/**
		 * Determines if a character is displayable in EBCDIC.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													EbcdicDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x40 || ui8Char == 0xCA || ui8Char == 0xFF ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcEbcdic[ui8Char-0x40];
			return 1;
		}

		/**
		 * Determines if a character is displayable in UTF-16.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													Utf16Display( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			if MX_UNLIKELY( _sSize < 2 ) { _wcChar = L'.'; return 0; }
			_wcChar = (*reinterpret_cast<const wchar_t *>(_pui8Char));

			if MX_UNLIKELY( _wcChar >= 0xD7FC && _wcChar <= 0xF8FF ) { _wcChar = L'.'; return 0; }
			// Reject noncharacters.
			if MX_UNLIKELY( _wcChar == 0xFFFE || _wcChar == 0xFFFF ) { _wcChar = L'.'; return 0; }
			if MX_UNLIKELY( _wcChar >= 0xFDD0 && _wcChar <= 0xFDEF ) { _wcChar = L'.'; return 0; }
			if MX_UNLIKELY( _wcChar >= 0xFFEF && _wcChar <= 0xFFFB ) { _wcChar = L'.'; return 0; }
			if MX_UNLIKELY( _wcChar >= 0xFFFD ) { _wcChar = L'.'; return 0; }

			WORD wType1{}, wType3{};
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE1, &_wcChar, 1, &wType1 ) ) { _wcChar = L'.'; return 0; }
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE3, &_wcChar, 1, &wType3 ) ) { _wcChar = L'.'; return 0; }

			// Controls are unprintable.
			if MX_UNLIKELY( (wType1 & C1_CNTRL) || !(wType1 & C1_DEFINED) ) { _wcChar = L'.'; return 0; }
			// Combining marks on their own are unprintable (Mn/Mc).
			if MX_UNLIKELY( wType3 & (C3_HIGHSURROGATE | C3_LOWSURROGATE | C3_NONSPACING | C3_DIACRITIC | C3_VOWELMARK) ) { _wcChar = L'.'; return 0; }

			return 2;
		}

		/**
		 * Determines if a character is displayable in UTF-8.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													Utf8Display( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			size_t sChars;
			uint32_t ui32Char = ee::CExpEval::NextUtf8Char( reinterpret_cast<const char *>(_pui8Char), _sSize, &sChars );
			if ( ui32Char == EE_UTF_INVALID ) {
				_wcChar = L'.'; return 1;
			}
			uint32_t ui32Utf16Chars;
			uint32_t ui32Utf16Char = ee::CExpEval::Utf32ToUtf16( ui32Char, ui32Utf16Chars );
			if ( ui32Utf16Chars == 2 ) { _wcChar = L'.'; return 1; }

			if ( !Utf16Display( reinterpret_cast<const uint8_t *>(&ui32Utf16Char), 2, _wcChar ) ) { return 1; }

			//if ( !std::isprint( _wcChar ) ) { _wcChar = L'.'; return 1; }

			return uint32_t( sChars );
		}

		/**
		 * Determines if a character is displayable in Macintosh.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													MacintoshDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F || ui8Char == 0xF0 ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcMac[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Arabic Windows.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													ArabicWindowsDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F || ui8Char == 0x9D || ui8Char == 0x9E || ui8Char == 0xAD ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcArabicWindows[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Arabic ISO.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													ArabicIsoDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 ||
				(ui8Char >= 0x7F && ui8Char <= 0x9F) ||
				(ui8Char >= 0xA1 && ui8Char <= 0xA3) ||
				(ui8Char >= 0xA5 && ui8Char <= 0xAB) ||
				(ui8Char >= 0xAD && ui8Char <= 0xBA) ||
				(ui8Char >= 0xBC && ui8Char <= 0xBE) ||
				ui8Char == 0xC0 ||
				(ui8Char >= 0xD8 && ui8Char <= 0xDF) ||
				ui8Char >= 0xF3 ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcArabicIso[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Baltic Windows.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													BalticWindowsDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F ||
				ui8Char == 0x81 || ui8Char == 0x83 || ui8Char == 0x88 || ui8Char == 0x8A || ui8Char == 0x8C ||
				ui8Char == 0x90 || ui8Char == 0x98 || ui8Char == 0x9A || ui8Char == 0x9C || ui8Char == 0x9F ||
				ui8Char == 0xA1 || ui8Char == 0xA5 || ui8Char == 0xAD ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcBalticWindows[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Baltic ISO.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													BalticIsoDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0xAD ||
				(ui8Char >= 0x7F && ui8Char <= 0x9F) ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcBalticIso[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Chinese (Simplified).
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													ChineseSimplifiedDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto sLen = CUtilities::Gb18030CharLen( _pui8Char, _sSize );
			if ( !sLen ) { _wcChar = L'.'; return 1; }
			if ( sLen == 1 ) {
				if MX_UNLIKELY( (*_pui8Char) < 0x7F && (*_pui8Char) >= 0x20 ) {
					_wcChar = (*_pui8Char);
					return 1;
				}
				_wcChar = L'.';
				return 0;
			}

			DWORD dwErr;

			auto wswTmp = CUtilities::MultiByteToWideChar( CCodePages::MX_GB18030, MB_ERR_INVALID_CHARS, reinterpret_cast<const char *>(_pui8Char), sLen, &dwErr );
			if ( dwErr != ERROR_SUCCESS || !wswTmp.size() ) { _wcChar = L'.'; return sLen; }
			_wcChar = wswTmp[0];

			WORD wType1{}, wType3{};
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE1, &_wcChar, 1, &wType1 ) ) { _wcChar = L'.'; return sLen; }
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE3, &_wcChar, 1, &wType3 ) ) { _wcChar = L'.'; return sLen; }

			// Controls are unprintable.
			if MX_UNLIKELY( (wType1 & C1_CNTRL) || !(wType1 & C1_DEFINED) ) { _wcChar = L'.'; return sLen; }
			// Combining marks on their own are unprintable (Mn/Mc).
			if MX_UNLIKELY( wType3 & (C3_HIGHSURROGATE | C3_LOWSURROGATE | C3_NONSPACING | C3_DIACRITIC | C3_VOWELMARK) ) { _wcChar = L'.'; return sLen; }

			return sLen;
		}

		/**
		 * Determines if a character is displayable in Chinese (Traditional).
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													ChineseTraditionalDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto sLen = CUtilities::Cp950CharLen( _pui8Char, _sSize );
			if ( !sLen ) { _wcChar = L'.'; return 1; }
			if ( sLen == 1 ) {
				if MX_UNLIKELY( (*_pui8Char) < 0x7F && (*_pui8Char) >= 0x20 ) {
					_wcChar = (*_pui8Char);
					return 1;
				}
				_wcChar = L'.';
				return 1;
			}
			DWORD dwErr;

			auto wswTmp = CUtilities::MultiByteToWideChar( CCodePages::MX_big5, MB_ERR_INVALID_CHARS, reinterpret_cast<const char *>(_pui8Char), sLen, &dwErr );
			if ( dwErr != ERROR_SUCCESS || !wswTmp.size() ) { _wcChar = L'.'; return sLen; }
			_wcChar = wswTmp[0];

			WORD wType1{}, wType3{};
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE1, &_wcChar, 1, &wType1 ) ) { _wcChar = L'.'; return sLen; }
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE3, &_wcChar, 1, &wType3 ) ) { _wcChar = L'.'; return sLen; }

			// Controls are unprintable.
			if MX_UNLIKELY( (wType1 & C1_CNTRL) || !(wType1 & C1_DEFINED) ) { _wcChar = L'.'; return sLen; }
			// Combining marks on their own are unprintable (Mn/Mc).
			if MX_UNLIKELY( wType3 & (C3_HIGHSURROGATE | C3_LOWSURROGATE | C3_NONSPACING | C3_DIACRITIC | C3_VOWELMARK) ) { _wcChar = L'.'; return sLen; }

			return sLen;
		}

		/**
		 * Determines if a character is displayable in Cyrillic Windows.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													CyrillicWindowsDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F ||
				ui8Char == 0x98 || ui8Char == 0xAD ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcCyrillicWindows[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Cyrillic KOI8-R.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													CyrillicKoi8RDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcCyrillicKoi8R[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Cyrillic KOI8-U.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													CyrillicKoi8UDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcCyrillicKoi8U[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Cyrillic ISO.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													CyrillicIsoDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0xAD ||
			(ui8Char >= 0x7F && ui8Char <= 0x9F) ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcCyrillicIso[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Eastern Europe (Windows).
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													EasternEuropeWindowsDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F || ui8Char == 0x81 || ui8Char == 0x83 || ui8Char == 0x88 || ui8Char == 0x90 || ui8Char == 0x98 || ui8Char == 0xAD ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcEasternEuropeWindows[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Eastern Europe ISO.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													EasternEuropeIsoDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0xAD ||
			(ui8Char >= 0x7F && ui8Char <= 0x9F) ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcEasternEuropeISO[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Greek Windows.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													GreekWindowsDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F ||
				ui8Char == 0x81 || ui8Char == 0x88 || ui8Char == 0x8A || ui8Char == 0x8C || ui8Char == 0x8D || ui8Char == 0x8E || ui8Char == 0x8F ||
				ui8Char == 0x90 || ui8Char == 0x98 || ui8Char == 0x9A || ui8Char == 0x9C || ui8Char == 0x9D || ui8Char == 0x9E || ui8Char == 0x9F ||
				ui8Char == 0xAA || ui8Char == 0xAD ||
				ui8Char == 0xD2 ||
				ui8Char == 0xFF ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcGreekWindows[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Greek ISO.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													GreekIsoDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || (ui8Char >= 0x7F && ui8Char <= 0x9F) ||
				ui8Char == 0xA4 || ui8Char == 0xA5 || ui8Char == 0xAA || ui8Char == 0xAD || ui8Char == 0xAE ||
				ui8Char == 0xD2 ||
				ui8Char == 0xFF ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcGreekIso[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Hebrew Windows.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													HebrewWindowsDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F ||
				ui8Char == 0x81 || ui8Char == 0x8A || ui8Char == 0x8C || ui8Char == 0x8D || ui8Char == 0x8E || ui8Char == 0x8F ||
				ui8Char == 0x90 || ui8Char == 0x9A || ui8Char == 0x9C || ui8Char == 0x9D || ui8Char == 0x9E || ui8Char == 0x9F ||
				ui8Char == 0xAD ||
				ui8Char == 0xCA ||
				(ui8Char >= 0xD9 && ui8Char <= 0xDF) ||
				(ui8Char >= 0xFB && ui8Char <= 0xFF) ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcHebrewWindows[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Hebrew ISO.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													HebrewIsoDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || (ui8Char >= 0x7F && ui8Char <= 0x9F) ||
				(ui8Char >= 0xBF && ui8Char <= 0xDE) ||
				ui8Char == 0xA1 || ui8Char == 0xAD ||
				ui8Char >= 0xFB ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcHebrewIso[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Japanese Shift JIS.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													JapaneseShiftJisDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto sLen = CUtilities::Cp932CharLen( _pui8Char, _sSize );
			if ( !sLen ) { _wcChar = L'.'; return 1; }
			if ( sLen == 1 ) {
				if MX_UNLIKELY( (*_pui8Char) < 0x20 ) { _wcChar = L'.'; return 1; }
				if MX_UNLIKELY( (*_pui8Char) >= 0x7F && (*_pui8Char) <= 0xA0 ) { _wcChar = L'.'; return 1; }
				if MX_UNLIKELY( (*_pui8Char) >= 0xE0 ) { _wcChar = L'.'; return 1; }
			}
			DWORD dwErr;

			auto wswTmp = CUtilities::MultiByteToWideChar( CCodePages::MX_shift_jis, MB_ERR_INVALID_CHARS, reinterpret_cast<const char *>(_pui8Char), sLen, &dwErr );
			if ( dwErr != ERROR_SUCCESS || !wswTmp.size() ) { _wcChar = L'.'; return sLen; }
			_wcChar = wswTmp[0];

			WORD wType1{}, wType3{};
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE1, &_wcChar, 1, &wType1 ) ) { _wcChar = L'.'; return sLen; }
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE3, &_wcChar, 1, &wType3 ) ) { _wcChar = L'.'; return sLen; }

			// Controls are unprintable.
			if MX_UNLIKELY( (wType1 & C1_CNTRL) || !(wType1 & C1_DEFINED) ) { _wcChar = L'.'; return sLen; }
			// Combining marks on their own are unprintable (Mn/Mc).
			if MX_UNLIKELY( wType3 & (C3_HIGHSURROGATE | C3_LOWSURROGATE | C3_NONSPACING | C3_DIACRITIC | C3_VOWELMARK) ) { _wcChar = L'.'; return sLen; }

			return sLen;
		}

		/**
		 * Determines if a character is displayable in Japanese EUC.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													JapaneseEucDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto sLen = CUtilities::Cp20932CharLen( _pui8Char, _sSize );
			if ( !sLen ) { _wcChar = L'.'; return 1; }
			if ( sLen == 1 ) {
				if MX_UNLIKELY( (*_pui8Char) < 0x7F && (*_pui8Char) >= 0x20 ) {
					_wcChar = (*_pui8Char);
					return 1;
				}
				_wcChar = L'.';
				return 1;
			}
			DWORD dwErr;

			auto wswTmp = CUtilities::MultiByteToWideChar( CCodePages::MX_EUC_JP, MB_ERR_INVALID_CHARS, reinterpret_cast<const char *>(_pui8Char), sLen, &dwErr );
			if ( dwErr != ERROR_SUCCESS || !wswTmp.size() ) { _wcChar = L'.'; return sLen; }
			_wcChar = wswTmp[0];

			WORD wType1{}, wType3{};
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE1, &_wcChar, 1, &wType1 ) ) { _wcChar = L'.'; return sLen; }
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE3, &_wcChar, 1, &wType3 ) ) { _wcChar = L'.'; return sLen; }

			// Controls are unprintable.
			if MX_UNLIKELY( (wType1 & C1_CNTRL) || !(wType1 & C1_DEFINED) ) { _wcChar = L'.'; return sLen; }
			// Combining marks on their own are unprintable (Mn/Mc).
			if MX_UNLIKELY( wType3 & (C3_HIGHSURROGATE | C3_LOWSURROGATE | C3_NONSPACING | C3_DIACRITIC | C3_VOWELMARK) ) { _wcChar = L'.'; return sLen; }

			return sLen;
		}

		/**
		 * Determines if a character is displayable in Korean EUC.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													KoreanEucDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto sLen = CUtilities::Cp51949CharLen( _pui8Char, _sSize );
			if ( !sLen ) { _wcChar = L'.'; return 1; }
			if ( sLen == 1 ) {
				if MX_UNLIKELY( (*_pui8Char) < 0x7F && (*_pui8Char) >= 0x20 ) {
					_wcChar = (*_pui8Char);
					return 1;
				}
				_wcChar = L'.';
				return 1;
			}
			DWORD dwErr;

			auto wswTmp = CUtilities::MultiByteToWideChar( CCodePages::MX_euc_kr, MB_ERR_INVALID_CHARS, reinterpret_cast<const char *>(_pui8Char), sLen, &dwErr );
			if ( dwErr != ERROR_SUCCESS || !wswTmp.size() ) { _wcChar = L'.'; return sLen; }
			_wcChar = wswTmp[0];

			WORD wType1{}, wType3{};
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE1, &_wcChar, 1, &wType1 ) ) { _wcChar = L'.'; return sLen; }
			if MX_UNLIKELY( !::GetStringTypeW( CT_CTYPE3, &_wcChar, 1, &wType3 ) ) { _wcChar = L'.'; return sLen; }

			// Controls are unprintable.
			if MX_UNLIKELY( (wType1 & C1_CNTRL) || !(wType1 & C1_DEFINED) ) { _wcChar = L'.'; return sLen; }
			// Combining marks on their own are unprintable (Mn/Mc).
			if MX_UNLIKELY( wType3 & (C3_HIGHSURROGATE | C3_LOWSURROGATE | C3_NONSPACING | C3_DIACRITIC | C3_VOWELMARK) ) { _wcChar = L'.'; return sLen; }

			return sLen;
		}

		/**
		 * Determines if a character is displayable in Thai.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													ThaiDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F ||
				ui8Char == 0x81 || ui8Char == 0x82 || ui8Char == 0x83 || ui8Char == 0x84 || ui8Char == 0x86 || ui8Char == 0x87 || ui8Char == 0x88 || ui8Char == 0x89 || ui8Char == 0x8A || ui8Char == 0x8B || ui8Char == 0x8C || ui8Char == 0x8D || ui8Char == 0x8E || ui8Char == 0x8F ||
				ui8Char == 0x90 || (ui8Char >= 0x98 && ui8Char <= 0x9F) ||
				ui8Char == 0xA0 ||
				(ui8Char >= 0xDB && ui8Char <= 0xDE) ||
				(ui8Char >= 0xFC && ui8Char <= 0xFF) ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcThai[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Turkish Windows.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													TurkishWindowsDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F ||
				ui8Char == 0x81 || ui8Char == 0x8D || ui8Char == 0x8E || ui8Char == 0x8F ||
				ui8Char == 0x90 || ui8Char == 0x9D || ui8Char == 0x9E ||
				ui8Char == 0xAD ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcTurkishWindows[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Turkish ISO.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													TurkishIsoDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0xAD ||
			(ui8Char >= 0x7F && ui8Char <= 0x9F) ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcTurkishISO[ui8Char-0x20];
			return 1;
		}

		/**
		 * Determines if a character is displayable in Vietnamese.
		 * 
		 * \param _pui8Char The character buffer to examine.
		 * \param _sSize The length of the bugger to which _pui8Char points.
		 * \param _wcChar The actual character to display.
		 * \return Returns 0 if not displayable, otherwise the number of bytes that make up the character.  _wcChar is filled with the character to print.
		 **/
		static uint32_t													VietnameseDisplay( const uint8_t * _pui8Char, size_t _sSize, wchar_t &_wcChar ) {
			auto ui8Char = (*_pui8Char);
			if MX_UNLIKELY( !_sSize || ui8Char < 0x20 || ui8Char == 0x7F ||
			ui8Char == 0x81 || ui8Char == 0x8A || ui8Char == 0x8D || ui8Char == 0x8E || ui8Char == 0x8F ||
			ui8Char == 0x90 || ui8Char == 0x9A || ui8Char == 0x9D || ui8Char == 0x9E ||
			ui8Char == 0xAD ) { _wcChar = L'.'; return 0; }

			_wcChar = m_wcTurkishISO[ui8Char-0x20];
			return 1;
		}


		// == Members.
		static const MX_CHAR_SET										m_csSets[];										/** The predefined character sets. */
		static const wchar_t											m_wcAsciiAnsi[];								/** ASCII/ANSI characters. */
		static const wchar_t											m_wcAsciiOEM[];									/** ASCII/OEM characters. */
		static const wchar_t											m_wcEbcdic[];									/** EBCDIC characters. */
		static const wchar_t											m_wcMac[];										/** Macintosh characters. */
		static const wchar_t											m_wcArabicWindows[];							/** Arabic Windows characters. */
		static const wchar_t											m_wcArabicIso[];								/** Arabic Iso characters. */
		static const wchar_t											m_wcBalticWindows[];							/** Baltic Windows characters. */
		static const wchar_t											m_wcBalticIso[];								/** Baltic ISO characters. */
		static const wchar_t											m_wcCyrillicWindows[];							/** Cyrillic Windows characters. */
		static const wchar_t											m_wcCyrillicKoi8R[];							/** Cyrillic KOI8-R characters. */
		static const wchar_t											m_wcCyrillicKoi8U[];							/** Cyrillic KOI8-U characters. */
		static const wchar_t											m_wcCyrillicIso[];								/** Cyrillic ISO characters. */
		static const wchar_t											m_wcEasternEuropeWindows[];						/** Eastern Europe Windows characters. */
		static const wchar_t											m_wcEasternEuropeISO[];							/** Eastern Europe ISO characters. */
		static const wchar_t											m_wcGreekWindows[];								/** Greek Windows characters. */
		static const wchar_t											m_wcGreekIso[];									/** Greek ISO characters. */
		static const wchar_t											m_wcHebrewWindows[];							/** Hebrew Windows characters. */
		static const wchar_t											m_wcHebrewIso[];								/** Hebrew ISO characters. */
		static const wchar_t											m_wcThai[];										/** Thai characters. */
		static const wchar_t											m_wcTurkishWindows[];							/** Turkish Windows characters. */
		static const wchar_t											m_wcTurkishISO[];								/** Turkish ISO characters. */
		static const wchar_t											m_wcVietnamese[];								/** Vietnamese characters. */


	};

}	// namespace mx

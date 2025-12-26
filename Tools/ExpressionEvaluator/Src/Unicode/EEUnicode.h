#pragma once

#include "../EEExpEval.h"

//#define EE_GEN_TABLE

namespace ee {

	class CUnicode {
	public :

		// == Enums.
		/**
		 * \brief Special constants used by the Unicode nickname table.
		 */
		enum : uint32_t {
			EE_INVALID				= 0xFFFFFFFF
		};

		// == Types.
		/**
		 * \brief Entry in the nickname-to-Unicode lookup table.
		 *
		 * Each entry pairs an ASCII nickname with its Unicode code point and the
		 * nickname length to avoid repeated strlen() calls during lookups.
		 */
		struct EE_NICKS {
			const char *			pcName;									/**< \brief Nickname (ASCII/UTF-8 bytes), not null-terminated assumptions are implementation-defined. */
			uint32_t				uiVal;									/**< \brief Unicode code point value for the nickname. */
			uint32_t				uiLen;									/**< \brief Length of \c pcName in bytes. */
		};


		// == Functions.
		/**
		 * \brief Gets the number of nickname entries available.
		 *
		 * \return Returns the total number of nickname entries in the internal table.
		 */
		static size_t				TotalNames();

		/**
		 * \brief Gets a nickname by index as a UTF-8 string.
		 *
		 * \param _sRet Receives the nickname text.
		 * \param _sIndex Index of the nickname to retrieve.
		 * \return Returns \c _sRet.
		 */
		static std::string &		GetName( std::string &_sRet, size_t _sIndex );

		/**
		 * \brief Gets a nickname by index as a wide string.
		 *
		 * \param _sRet Receives the nickname text.
		 * \param _sIndex Index of the nickname to retrieve.
		 * \return Returns \c _sRet.
		 */
		static std::wstring &		GetName( std::wstring &_sRet, size_t _sIndex );

		/**
		 * \brief Gets the Unicode code point for a nickname entry by index.
		 *
		 * \param _sIndex Index of the nickname entry to query.
		 * \return Returns the Unicode code point for the entry, or \c EE_INVALID if
		 * \c _sIndex is out of range.
		 */
		static uint32_t				GetUnicodeNum( size_t _sIndex ) { return _sIndex < TotalNames() ?
			m_nNames[_sIndex].uiVal :
			EE_INVALID;
		}

		/**
		 * \brief Gets the length, in bytes, of a nickname entry by index.
		 *
		 * \param _sIndex Index of the nickname entry to query.
		 * \return Returns the nickname length in bytes, or 0 if \c _sIndex is out of range.
		 */
		static size_t				GetNameLength( size_t _sIndex ) { return _sIndex < TotalNames() ?
			m_nNames[_sIndex].uiLen :
			0;
		}

		/**
		 * \brief Looks up a Unicode code point by nickname (case-sensitive).
		 *
		 * \param _pcName Pointer to the nickname text to look up.
		 * \param _sLen Length of \c _pcName in bytes. If 0, the implementation may
		 * compute the length (e.g. via strlen()).
		 * \return Returns the corresponding Unicode code point, or \c EE_INVALID if
		 * the nickname is not found.
		 */
		static uint32_t				GetCode( const char * _pcName, size_t _sLen = 0 );

#ifdef EE_GEN_TABLE
		/**
		 * \brief Generates internal nickname lookup tables.
		 *
		 * This is intended for offline/tooling builds when regenerating the compiled
		 * nickname table.
		 */
		static void					MakeNickNameTable();
#endif	// #if EE_GEN_TABLE



	protected :
		// == Members.
		/** \brief Nickname table mapping names to Unicode code points. */
		static EE_NICKS				m_nNames[];


		// == Functions.
		/**
		 * \brief Encrypts a string into an internal, lookup-friendly form.
		 *
		 * Used to transform nickname text for faster comparisons (e.g. hashing or
		 * normalization specific to the table generator/lookup).
		 *
		 * \param _pcString Pointer to the input string bytes.
		 * \param _sLen Length of \c _pcString in bytes.
		 * \param _sResult Receives the encrypted/transformed output.
		 */
		static void					Encrypt( const char * _pcString, size_t _sLen, std::string &_sResult );

	};
}

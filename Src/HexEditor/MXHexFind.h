#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXSecureWString.h"
#include "MXCharSets.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>


namespace mx {

	/**
	 * The Hex Editor's Find (Find -> Find...).  Turns what was typed into a search (text in any character set, hex bytes, or numbers;
	 *	optionally with wildcards or a regular expression) and finds matches in a data source.
	 **/
	class CHexFind {
	public :
		CHexFind();
		~CHexFind();


		// == Enumerations.
		/** What is being searched for. */
		enum MX_FIND_TYPE : uint32_t {
			MX_FT_TEXT,												/**< Text in the character set of the view. */
			MX_FT_HEX,												/**< Hex bytes. */
			MX_FT_ASCII,											/**< An ASCII (ANSI) string. */
			MX_FT_UNICODE,											/**< A UTF-16 string (in the byte order of the view). */
			MX_FT_EBCDIC,											/**< An EBCDIC string. */
			MX_FT_INT8,												/**< A signed byte. */
			MX_FT_UINT8,											/**< An unsigned byte. */
			MX_FT_INT16,											/**< A signed short. */
			MX_FT_UINT16,											/**< An unsigned short. */
			MX_FT_INT32,											/**< A signed int. */
			MX_FT_UINT32,											/**< An unsigned int. */
			MX_FT_INT64,											/**< A signed int64. */
			MX_FT_UINT64,											/**< An unsigned int64. */
			MX_FT_FLOAT,											/**< A float. */
			MX_FT_DOUBLE,											/**< A double. */

			MX_FT_TOTAL
		};


		// == Types.
		/** The settings of a search (the Find dialog). */
		struct MX_FIND_PARMS {
			CSecureWString											swsQuery;										/**< What was typed. */
			MX_FIND_TYPE											ftType = MX_FT_HEX;								/**< What it is. */
			bool													bFindAll = false;								/**< Enter finds all occurrences (otherwise the next one). */
			bool													bMatchCase = false;								/**< Text must match in case. */
			bool													bWholeWord = false;								/**< Text must not be part of a longer word. */
			bool													bRegex = false;									/**< The text is a regular expression. */
			bool													bWildcards = false;								/**< '?' and '*' are wildcards. */
			bool													bMultipleRanges = false;						/**< Find All keeps the results of earlier searches. */
			bool													bUp = false;									/**< Enter searches backward. */
			bool													bLockToSelection = false;						/**< Searches only within ui64LockStart and ui64LockSize. */
			bool													bIncremental = true;							/**< Searches as the text is typed. */
			bool													bWrap = true;									/**< Searching past the end continues at the start (or the reverse). */
			bool													bTypeSpecifiers = true;							/**< A ",h", ",i32", etc. suffix sets the type. */
			bool													bHideAfterSearch = true;						/**< Find All closes the Find dialog. */
			bool													bQuickFind = true;								/**< Find Next/Previous searches for the selected bytes. */
			bool													bCustomColor = false;							/**< Find All results use crFore and crBack. */
			bool													bCustomFore = false;							/**< crFore is used (otherwise the text keeps its color). */
			COLORREF												crFore = RGB( 0x00, 0x00, 0x00 );				/**< The custom text color. */
			COLORREF												crBack = RGB( 0xE0, 0xE6, 0xFF );				/**< The custom background color. */
			bool													bLimit = true;									/**< Find All stops at ui64Limit occurrences. */
			uint64_t												ui64Limit = 10000000ULL;						/**< The most occurrences Find All finds. */
			uint32_t												ui32MaxWildcard = 24;							/**< The most bytes a '*' matches. */
			double													dTolerance = 0.0;								/**< Floats and doubles match within this much of the value. */

			// Filled in by the Hex Editor.
			CCharSets::MX_CHAR_SETS									csCharSet = CCharSets::MX_CS_ASCII;				/**< The character set of the view (for MX_FT_TEXT). */
			bool													bBigEndian = false;								/**< The byte order of the view. */
			uint64_t												ui64LockStart = 0;								/**< The start of the locked range. */
			uint64_t												ui64LockSize = 0;								/**< The size of the locked range. */
		};

		/** A match. */
		struct MX_FIND_MATCH {
			uint64_t												ui64Addr = 0;									/**< The address of the first byte. */
			uint64_t												ui64Size = 0;									/**< The number of bytes. */
		};

		/** Reads data: returns false if the bytes cannot be read.  _vDst receives _sSize bytes. */
		typedef std::function<bool ( uint64_t _ui64Addr, size_t _sSize, std::vector<uint8_t> &_vDst )>
																	CReader;

		/** Reports progress (0 to 1).  Returns false to stop. */
		typedef std::function<bool ( double _dProgress )>			CProgress;


		// == Functions.
		/**
		 * Prepares a search.  Applies a type specifier (",h", ",i32", etc.) if they are allowed, converts the text, and checks it.
		 *
		 * \param _fpParms The search.
		 * \param _swsError Receives a description of the problem on failure.
		 * \return Returns true if the search can be done.
		 **/
		bool														Compile( const MX_FIND_PARMS &_fpParms, CSecureWString &_swsError );

		/**
		 * Gets the type that was searched for (after a type specifier).
		 *
		 * \return Returns the type of the compiled search.
		 **/
		inline MX_FIND_TYPE											Type() const { return m_ftType; }

		/**
		 * Finds the next or previous match.
		 *
		 * \param _rReader Reads the data.
		 * \param _ui64RangeStart The start of the range searched.
		 * \param _ui64RangeEnd The end of the range searched.
		 * \param _ui64From Searching forward, matches start at or after this; backward, they start before it.
		 * \param _bUp Searches backward.
		 * \param _bWrap Continues from the other end of the range.
		 * \param _fmMatch Receives the match.
		 * \param _pProgress Optional progress callback.
		 * \param _bWrapped Receives true if the match was found after wrapping.
		 * \return Returns true if a match was found.
		 **/
		bool														FindNext( const CReader &_rReader, uint64_t _ui64RangeStart, uint64_t _ui64RangeEnd,
			uint64_t _ui64From, bool _bUp, bool _bWrap, MX_FIND_MATCH &_fmMatch, const CProgress &_pProgress, bool &_bWrapped );

		/**
		 * Finds every match in a range.
		 *
		 * \param _rReader Reads the data.
		 * \param _ui64RangeStart The start of the range searched.
		 * \param _ui64RangeEnd The end of the range searched.
		 * \param _ui64Limit The most matches to find (0 = no limit).
		 * \param _vMatches Receives the matches, in order.
		 * \param _bTooMany Receives true if the limit was reached.
		 * \param _pProgress Optional progress callback.
		 * \return Returns false if the search was stopped or memory ran out.
		 **/
		bool														FindAll( const CReader &_rReader, uint64_t _ui64RangeStart, uint64_t _ui64RangeEnd,
			uint64_t _ui64Limit, std::vector<MX_FIND_MATCH> &_vMatches, bool &_bTooMany, const CProgress &_pProgress );

		/**
		 * Gets the name of a type ("Hex Bytes (h)", etc.).
		 *
		 * \param _ftType The type.
		 * \return Returns the name.
		 **/
		static CSecureWString										TypeName( MX_FIND_TYPE _ftType );

		/**
		 * Splits a type specifier (",h", ",i32", etc.) from the end of a query.
		 *
		 * \param _swsQuery The query.  The specifier is removed if found.
		 * \param _ftType Receives the type if a specifier was found.
		 * \return Returns true if a specifier was found.
		 **/
		static bool													SplitTypeSpecifier( CSecureWString &_swsQuery, MX_FIND_TYPE &_ftType );

		/**
		 * Determines whether a type is text (wildcards, case, whole words, and regular expressions apply).
		 *
		 * \param _ftType The type.
		 * \return Returns true for the string types.
		 **/
		static inline bool											IsText( MX_FIND_TYPE _ftType ) { return _ftType <= MX_FT_EBCDIC && _ftType != MX_FT_HEX; }


	protected :
		// == Enumerations.
		/** How the data is matched. */
		enum MX_MATCH_KIND {
			MX_MK_PATTERN,											/**< A byte pattern (with alternatives, masks, and wildcards). */
			MX_MK_FLOAT,											/**< A float within a tolerance. */
			MX_MK_DOUBLE,											/**< A double within a tolerance. */
			MX_MK_REGEX,											/**< A regular expression. */
		};


		// == Types.
		/** One element of a byte pattern. */
		struct MX_ELEMENT {
			uint8_t													ui8Val0 = 0;									/**< A value the byte can have. */
			uint8_t													ui8Val1 = 0;									/**< Another value the byte can have (the other case). */
			uint8_t													ui8Mask = 0xFF;									/**< The bits that are compared (0 for "any byte"). */
			bool													bStar = false;									/**< Matches 0 to m_ui32MaxWildcard bytes. */
		};

		/** The regular expression (hidden so that Oniguruma is only included in the .cpp). */
		struct MX_REGEX;


		// == Members.
		/** How the data is matched. */
		MX_MATCH_KIND												m_mkKind = MX_MK_PATTERN;
		/** The type searched for. */
		MX_FIND_TYPE												m_ftType = MX_FT_HEX;
		/** The byte pattern. */
		std::vector<MX_ELEMENT>										m_vPattern;
		/** The number of bytes the pattern matches at least. */
		size_t														m_sMinLen = 0;
		/** The number of bytes the pattern matches at most. */
		size_t														m_sMaxLen = 0;
		/** The most bytes a '*' matches. */
		uint32_t													m_ui32MaxWildcard = 24;
		/** The value of a float or double search. */
		double														m_dValue = 0.0;
		/** The tolerance of a float or double search. */
		double														m_dTolerance = 0.0;
		/** Text must not be part of a longer word. */
		bool														m_bWholeWord = false;
		/** The size of a character for whole-word checks (1 or 2). */
		uint32_t													m_ui32CharSize = 1;
		/** Whether the data is big-endian. */
		bool														m_bBigEndian = false;
		/** Whether the text is EBCDIC (for whole-word checks). */
		bool														m_bEbcdic = false;
		/** The regular expression. */
		std::unique_ptr<MX_REGEX>									m_prRegex;


		// == Functions.
		/**
		 * Finds every match that starts in a range of a buffer.
		 *
		 * \param _pui8Data The buffer.
		 * \param _sSize The number of bytes in the buffer.
		 * \param _sBegin Matches must start at or after this offset (earlier bytes are only there for whole-word checks).
		 * \param _sStartEnd Matches must start before this offset (later bytes are only there to complete matches).
		 * \param _bAtStart The buffer starts where the searched range starts.
		 * \param _bAtEnd The buffer ends where the searched range ends.
		 * \param _bOverlap Matches may overlap (otherwise the search continues after each match).
		 * \param _fFound Called for each match (offset, size); returns false to stop.
		 * \return Returns false if _fFound stopped the search.
		 **/
		bool														ScanBuffer( const uint8_t * _pui8Data, size_t _sSize, size_t _sBegin, size_t _sStartEnd, bool _bAtStart, bool _bAtEnd,
			bool _bOverlap, const std::function<bool ( size_t, size_t )> &_fFound ) const;

		/**
		 * Finds the matches that start in part of a range, reading the data in chunks.
		 *
		 * \param _rReader Reads the data.
		 * \param _ui64RangeStart The start of the searched range (no data before it is used).
		 * \param _ui64RangeEnd The end of the searched range (no data after it is used).
		 * \param _ui64From Matches start at or after this.
		 * \param _ui64To Matches start before this.
		 * \param _bOverlap Matches may overlap.
		 * \param _fFound Called for each match (address, size), in order; returns false to stop.
		 * \param _pProgress Optional progress callback.
		 * \param _bStopped Receives true if the progress callback stopped the search.
		 * \return Returns false if data could not be read or the search was stopped.
		 **/
		bool														ScanRange( const CReader &_rReader, uint64_t _ui64RangeStart, uint64_t _ui64RangeEnd, uint64_t _ui64From, uint64_t _ui64To,
			bool _bOverlap, const std::function<bool ( uint64_t, uint64_t )> &_fFound, const CProgress &_pProgress, bool &_bStopped ) const;

		/**
		 * Finds the last match that starts in part of a range.
		 *
		 * \param _rReader Reads the data.
		 * \param _ui64RangeStart The start of the searched range.
		 * \param _ui64RangeEnd The end of the searched range.
		 * \param _ui64From Matches start at or after this.
		 * \param _ui64To Matches start before this.
		 * \param _fmMatch Receives the match.
		 * \param _pProgress Optional progress callback.
		 * \param _bStopped Receives true if the progress callback stopped the search.
		 * \return Returns true if a match was found.
		 **/
		bool														FindLast( const CReader &_rReader, uint64_t _ui64RangeStart, uint64_t _ui64RangeEnd, uint64_t _ui64From, uint64_t _ui64To,
			MX_FIND_MATCH &_fmMatch, const CProgress &_pProgress, bool &_bStopped ) const;

		/**
		 * Matches the pattern at a position.
		 *
		 * \param _pui8Data The data.
		 * \param _sSize The number of bytes from _pui8Data.
		 * \param _sElement The first element of the pattern to match.
		 * \param _sLen Receives the number of bytes matched.
		 * \return Returns true if the pattern matches.
		 **/
		bool														MatchAt( const uint8_t * _pui8Data, size_t _sSize, size_t _sElement, size_t &_sLen ) const;

		/**
		 * Determines whether a match is a whole word.
		 *
		 * \param _pui8Data The buffer.
		 * \param _sSize The number of bytes in the buffer.
		 * \param _sOff The offset of the match.
		 * \param _sLen The length of the match.
		 * \param _bAtStart The buffer starts where the searched range starts.
		 * \param _bAtEnd The buffer ends where the searched range ends.
		 * \return Returns true if no word character is next to either end.
		 **/
		bool														IsWholeWord( const uint8_t * _pui8Data, size_t _sSize, size_t _sOff, size_t _sLen, bool _bAtStart, bool _bAtEnd ) const;

		/**
		 * Determines whether the character at a position is part of a word (a letter, digit, or underscore).
		 *
		 * \param _pui8Char The character.
		 * \return Returns true for word characters.
		 **/
		bool														IsWordChar( const uint8_t * _pui8Char ) const;

		/**
		 * Gets how many bytes past the end of a chunk must be read so that matches starting in the chunk are complete.
		 *
		 * \return Returns the overlap in bytes.
		 **/
		size_t														Overlap() const;

		/**
		 * Converts text to bytes in a character set.
		 *
		 * \param _wsText The text.
		 * \param _ftType MX_FT_TEXT, MX_FT_ASCII, MX_FT_UNICODE, or MX_FT_EBCDIC.
		 * \param _csCharSet The character set of the view (for MX_FT_TEXT).
		 * \param _bBigEndian The byte order (for UTF-16).
		 * \param _vBytes Receives the bytes.
		 * \return Returns false if a character cannot be represented.
		 **/
		static bool													Encode( const std::wstring &_wsText, MX_FIND_TYPE _ftType, CCharSets::MX_CHAR_SETS _csCharSet, bool _bBigEndian,
			std::vector<uint8_t> &_vBytes );

		/**
		 * Builds the pattern for text.
		 *
		 * \param _fpParms The search.
		 * \param _wsText The text (without a type specifier).
		 * \param _swsError Receives a description of the problem on failure.
		 * \return Returns true if the pattern was built.
		 **/
		bool														CompileText( const MX_FIND_PARMS &_fpParms, const std::wstring &_wsText, CSecureWString &_swsError );

		/**
		 * Builds the pattern for hex bytes.
		 *
		 * \param _wsText The text (without a type specifier).
		 * \param _bWildcards '?' and '*' are wildcards.
		 * \param _swsError Receives a description of the problem on failure.
		 * \return Returns true if the pattern was built.
		 **/
		bool														CompileHex( const std::wstring &_wsText, bool _bWildcards, CSecureWString &_swsError );

		/**
		 * Builds the pattern (or value) for a number.
		 *
		 * \param _fpParms The search.
		 * \param _wsText The text (without a type specifier).
		 * \param _swsError Receives a description of the problem on failure.
		 * \return Returns true if the pattern was built.
		 **/
		bool														CompileNumber( const MX_FIND_PARMS &_fpParms, const std::wstring &_wsText, CSecureWString &_swsError );

		/**
		 * Prepares a regular expression.
		 *
		 * \param _fpParms The search.
		 * \param _wsText The expression (without a type specifier).
		 * \param _swsError Receives a description of the problem on failure.
		 * \return Returns true if the expression is valid.
		 **/
		bool														CompileRegex( const MX_FIND_PARMS &_fpParms, const std::wstring &_wsText, CSecureWString &_swsError );

		/**
		 * Updates m_sMinLen and m_sMaxLen from the pattern.
		 **/
		void														UpdateLengths();
	};

}	// namespace mx

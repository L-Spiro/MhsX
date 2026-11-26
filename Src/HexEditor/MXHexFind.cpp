#include "MXHexFind.h"
#include "../RegEx/Oniguruma/oniguruma.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cwctype>


namespace mx {

	/** The size of the chunks read while searching. */
	#define MX_HF_CHUNK												(4ULL * 1024ULL * 1024ULL)
	/** How far past a chunk a regular expression can see. */
	#define MX_HF_REGEX_OVERLAP										(64ULL * 1024ULL)

	/** The regular expression. */
	struct CHexFind::MX_REGEX {
		~MX_REGEX() {
			if ( prReg ) { ::onig_free( prReg ); }
			if ( porRegion ) { ::onig_region_free( porRegion, 1 ); }
		}


		// == Members.
		/** The compiled expression. */
		::regex_t *													prReg = nullptr;
		/** The match region. */
		::OnigRegion *												porRegion = nullptr;
	};

	CHexFind::CHexFind() {
	}
	CHexFind::~CHexFind() {
	}

	// == Functions.
	/**
	 * Prepares a search.  Applies a type specifier (",h", ",i32", etc.) if they are allowed, converts the text, and checks it.
	 *
	 * \param _fpParms The search.
	 * \param _swsError Receives a description of the problem on failure.
	 * \return Returns true if the search can be done.
	 **/
	bool CHexFind::Compile( const MX_FIND_PARMS &_fpParms, CSecureWString &_swsError ) {
		m_vPattern.clear();
		m_prRegex.reset();
		m_sMinLen = m_sMaxLen = 0;
		m_mkKind = MX_MK_PATTERN;
		m_ui32MaxWildcard = std::max<uint32_t>( _fpParms.ui32MaxWildcard, 1 );
		m_bBigEndian = _fpParms.bBigEndian;
		m_bWholeWord = false;
		m_ui32CharSize = 1;
		m_bEbcdic = false;
		try {
			CSecureWString swsQuery = _fpParms.swsQuery;
			m_ftType = _fpParms.ftType;
			if ( _fpParms.bTypeSpecifiers ) { SplitTypeSpecifier( swsQuery, m_ftType ); }
			if ( swsQuery.empty() ) {
				_swsError = _DEC_WS_C84B5768_Nothing_to_find_;
				return false;
			}

			if ( IsText( m_ftType ) ) {
				// The size of a character and the encoding of the text, for whole-word checks.
				const bool bUnicode = m_ftType == MX_FT_UNICODE || (m_ftType == MX_FT_TEXT && _fpParms.csCharSet == CCharSets::MX_CS_UNICODE);
				m_ui32CharSize = bUnicode ? 2 : 1;
				m_bEbcdic = m_ftType == MX_FT_EBCDIC || (m_ftType == MX_FT_TEXT && _fpParms.csCharSet == CCharSets::MX_CS_EBCDIC);
				m_bWholeWord = _fpParms.bWholeWord;
				if ( _fpParms.bRegex ) { return CompileRegex( _fpParms, swsQuery, _swsError ); }
				return CompileText( _fpParms, swsQuery, _swsError );
			}
			if ( _fpParms.bRegex ) {
				_swsError = _DEC_WS_6A35309F_Regular_expressions_can_only_be_used_to_find_text_in_ASCII__ANSI__UTF_8__or_Unicode_;
				return false;
			}
			if ( m_ftType == MX_FT_HEX ) { return CompileHex( swsQuery, _fpParms.bWildcards, _swsError ); }
			return CompileNumber( _fpParms, swsQuery, _swsError );
		}
		catch ( ... ) {
			_swsError = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

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
	bool CHexFind::FindNext( const CReader &_rReader, uint64_t _ui64RangeStart, uint64_t _ui64RangeEnd,
		uint64_t _ui64From, bool _bUp, bool _bWrap, MX_FIND_MATCH &_fmMatch, const CProgress &_pProgress, bool &_bWrapped ) {
		_bWrapped = false;
		if ( _ui64RangeEnd <= _ui64RangeStart ) { return false; }
		_ui64From = std::clamp( _ui64From, _ui64RangeStart, _ui64RangeEnd );
		bool bStopped = false;
		if ( !_bUp ) {
			bool bFound = false;
			auto fFirst = [&]( uint64_t _ui64Addr, uint64_t _ui64Size ) {
				_fmMatch.ui64Addr = _ui64Addr;
				_fmMatch.ui64Size = _ui64Size;
				bFound = true;
				return false;
			};
			ScanRange( _rReader, _ui64RangeStart, _ui64RangeEnd, _ui64From, _ui64RangeEnd, true, fFirst, _pProgress, bStopped );
			if ( bFound || bStopped || !_bWrap || _ui64From == _ui64RangeStart ) { return bFound; }
			ScanRange( _rReader, _ui64RangeStart, _ui64RangeEnd, _ui64RangeStart, _ui64From, true, fFirst, _pProgress, bStopped );
			_bWrapped = bFound;
			return bFound;
		}

		if ( FindLast( _rReader, _ui64RangeStart, _ui64RangeEnd, _ui64RangeStart, _ui64From, _fmMatch, _pProgress, bStopped ) ) { return true; }
		if ( bStopped || !_bWrap || _ui64From == _ui64RangeEnd ) { return false; }
		_bWrapped = FindLast( _rReader, _ui64RangeStart, _ui64RangeEnd, _ui64From, _ui64RangeEnd, _fmMatch, _pProgress, bStopped );
		return _bWrapped;
	}

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
	bool CHexFind::FindAll( const CReader &_rReader, uint64_t _ui64RangeStart, uint64_t _ui64RangeEnd,
		uint64_t _ui64Limit, std::vector<MX_FIND_MATCH> &_vMatches, bool &_bTooMany, const CProgress &_pProgress ) {
		_bTooMany = false;
		_vMatches.clear();
		if ( _ui64RangeEnd <= _ui64RangeStart ) { return true; }
		bool bMemory = true;
		auto fAll = [&]( uint64_t _ui64Addr, uint64_t _ui64Size ) {
			try {
				MX_FIND_MATCH fmMatch;
				fmMatch.ui64Addr = _ui64Addr;
				fmMatch.ui64Size = _ui64Size;
				_vMatches.push_back( fmMatch );
			}
			catch ( ... ) {
				bMemory = false;
				return false;
			}
			if ( _ui64Limit && _vMatches.size() >= _ui64Limit ) {
				_bTooMany = true;
				return false;
			}
			return true;
		};
		bool bStopped = false;
		bool bRead = ScanRange( _rReader, _ui64RangeStart, _ui64RangeEnd, _ui64RangeStart, _ui64RangeEnd, false, fAll, _pProgress, bStopped );
		return bMemory && !bStopped && (bRead || _bTooMany);
	}

	/**
	 * Gets the name of a type ("Hex Bytes (h)", etc.).
	 *
	 * \param _ftType The type.
	 * \return Returns the name.
	 **/
	CSecureWString CHexFind::TypeName( MX_FIND_TYPE _ftType ) {
		switch ( _ftType ) {
			case MX_FT_TEXT : { return _DEC_WS_196E144C_Text__t_; }
			case MX_FT_HEX : { return _DEC_WS_975A3F6B_Hex_Bytes__h_; }
			case MX_FT_ASCII : { return _DEC_WS_613C6678_ASCII_String__a_; }
			case MX_FT_UNICODE : { return _DEC_WS_2054C6D6_Unicode_String__u_; }
			case MX_FT_EBCDIC : { return _DEC_WS_6BBCF9F0_EBCDIC_String__e_; }
			case MX_FT_INT8 : { return _DEC_WS_8ECBDC3C_Signed_Byte__i8_; }
			case MX_FT_UINT8 : { return _DEC_WS_385BC563_Unsigned_Byte__ui8_; }
			case MX_FT_INT16 : { return _DEC_WS_B6C0C351_Signed_Short__i16_; }
			case MX_FT_UINT16 : { return _DEC_WS_56848A27_Unsigned_Short__ui16_; }
			case MX_FT_INT32 : { return _DEC_WS_AD69D679_Signed_Int__i32_; }
			case MX_FT_UINT32 : { return _DEC_WS_B3F2692E_Unsigned_Int__ui32_; }
			case MX_FT_INT64 : { return _DEC_WS_07B73E9C_Signed_Int64__i64_; }
			case MX_FT_UINT64 : { return _DEC_WS_1ACADB07_Unsigned_Int64__ui64_; }
			case MX_FT_FLOAT : { return _DEC_WS_99B89100_Float__f_; }
			case MX_FT_DOUBLE : { return _DEC_WS_85B0D384_Double__lf_; }
			default : { return CSecureWString(); }
		}
	}

	/**
	 * Splits a type specifier (",h", ",i32", etc.) from the end of a query.
	 *
	 * \param _swsQuery The query.  The specifier is removed if found.
	 * \param _ftType Receives the type if a specifier was found.
	 * \return Returns true if a specifier was found.
	 **/
	bool CHexFind::SplitTypeSpecifier( CSecureWString &_swsQuery, MX_FIND_TYPE &_ftType ) {
		const size_t sComma = _swsQuery.rfind( L',' );
		if ( sComma == std::wstring::npos ) { return false; }
		// The specifier, trimmed and in lower case.
		std::wstring wsSpec;
		for ( size_t I = sComma + 1; I < _swsQuery.size(); ++I ) {
			if ( !std::iswspace( _swsQuery[I] ) ) { wsSpec.push_back( wchar_t( std::towlower( _swsQuery[I] ) ) ); }
		}
		if ( wsSpec.empty() ) { return false; }
		// The specifiers are the letters in parentheses at the ends of the type names.
		for ( uint32_t I = 0; I < MX_FT_TOTAL; ++I ) {
			const CSecureWString swsName = TypeName( MX_FIND_TYPE( I ) );
			const size_t sOpen = swsName.rfind( L'(' );
			const size_t sClose = swsName.rfind( L')' );
			if ( sOpen == std::wstring::npos || sClose == std::wstring::npos || sClose <= sOpen ) { continue; }
			if ( swsName.compare( sOpen + 1, sClose - sOpen - 1, wsSpec ) == 0 ) {
				_ftType = MX_FIND_TYPE( I );
				size_t sEnd = sComma;
				while ( sEnd && std::iswspace( _swsQuery[sEnd-1] ) ) { --sEnd; }
				_swsQuery.resize( sEnd );
				return true;
			}
		}
		return false;
	}

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
	bool CHexFind::ScanBuffer( const uint8_t * _pui8Data, size_t _sSize, size_t _sBegin, size_t _sStartEnd, bool _bAtStart, bool _bAtEnd,
		bool _bOverlap, const std::function<bool ( size_t, size_t )> &_fFound ) const {
		_sStartEnd = std::min( _sStartEnd, _sSize );
		switch ( m_mkKind ) {
			case MX_MK_PATTERN : {
				if ( m_vPattern.empty() ) { return true; }
				// Bytes that must be at the start of a match can be found quickly.
				const MX_ELEMENT & eFirst = m_vPattern[0];
				const bool bQuickFirst = !eFirst.bStar && eFirst.ui8Mask == 0xFF && eFirst.ui8Val0 == eFirst.ui8Val1;
				for ( size_t I = _sBegin; I < _sStartEnd; ) {
					if ( bQuickFirst ) {
						const void * pvNext = std::memchr( _pui8Data + I, eFirst.ui8Val0, _sStartEnd - I );
						if ( !pvNext ) { break; }
						I = static_cast<size_t>(static_cast<const uint8_t *>(pvNext) - _pui8Data);
					}
					size_t sLen = 0;
					if ( MatchAt( _pui8Data + I, _sSize - I, 0, sLen ) && sLen &&
						(!m_bWholeWord || IsWholeWord( _pui8Data, _sSize, I, sLen, _bAtStart, _bAtEnd )) ) {
						if ( !_fFound( I, sLen ) ) { return false; }
						I += _bOverlap ? 1 : sLen;
						continue;
					}
					++I;
				}
				return true;
			}
			case MX_MK_FLOAT : {}									MX_FALLTHROUGH
			case MX_MK_DOUBLE : {
				const size_t sValSize = (m_mkKind == MX_MK_FLOAT) ? sizeof( float ) : sizeof( double );
				for ( size_t I = _sBegin; I < _sStartEnd && I + sValSize <= _sSize; ++I ) {
					uint8_t ui8Bytes[8];
					for ( size_t J = 0; J < sValSize; ++J ) {
						ui8Bytes[J] = _pui8Data[I+(m_bBigEndian ? (sValSize - 1 - J) : J)];
					}
					double dVal;
					if ( sValSize == sizeof( float ) ) {
						float fVal;
						std::memcpy( &fVal, ui8Bytes, sizeof( fVal ) );
						dVal = fVal;
					}
					else { std::memcpy( &dVal, ui8Bytes, sizeof( dVal ) ); }
					if ( !std::isfinite( dVal ) ) { continue; }
					if ( std::fabs( dVal - m_dValue ) <= m_dTolerance ) {
						if ( !_fFound( I, sValSize ) ) { return false; }
						if ( !_bOverlap ) { I += sValSize - 1; }
					}
				}
				return true;
			}
			case MX_MK_REGEX : {
				if ( !m_prRegex || !m_prRegex->prReg ) { return true; }
				const OnigUChar * pucStart = reinterpret_cast<const OnigUChar *>(_pui8Data);
				const OnigUChar * pucEnd = pucStart + _sSize;
				for ( size_t I = _sBegin; I < _sStartEnd; ) {
					const int iPos = ::onig_search( m_prRegex->prReg, pucStart, pucEnd, pucStart + I, pucStart + _sStartEnd,
						m_prRegex->porRegion, ONIG_OPTION_NONE );
					if ( iPos < 0 || size_t( iPos ) >= _sStartEnd ) { break; }
					const size_t sLen = size_t( m_prRegex->porRegion->end[0] - m_prRegex->porRegion->beg[0] );
					if ( sLen && (!m_bWholeWord || IsWholeWord( _pui8Data, _sSize, size_t( iPos ), sLen, _bAtStart, _bAtEnd )) ) {
						if ( !_fFound( size_t( iPos ), sLen ) ) { return false; }
						I = size_t( iPos ) + (_bOverlap ? m_ui32CharSize : sLen);
					}
					else {
						// Empty matches and partial words are skipped.
						I = size_t( iPos ) + m_ui32CharSize;
					}
				}
				return true;
			}
		}
		return true;
	}

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
	bool CHexFind::ScanRange( const CReader &_rReader, uint64_t _ui64RangeStart, uint64_t _ui64RangeEnd, uint64_t _ui64From, uint64_t _ui64To,
		bool _bOverlap, const std::function<bool ( uint64_t, uint64_t )> &_fFound, const CProgress &_pProgress, bool &_bStopped ) const {
		_bStopped = false;
		_ui64To = std::min( _ui64To, _ui64RangeEnd );
		if ( _ui64From >= _ui64To ) { return true; }
		std::vector<uint8_t> vBuffer;
		const uint64_t ui64Overlap = Overlap();
		uint64_t ui64Next = _ui64From;					// Without overlapping, the next match must start here or later.
		for ( uint64_t ui64Pos = _ui64From; ui64Pos < _ui64To; ) {
			const uint64_t ui64Take = std::min<uint64_t>( MX_HF_CHUNK, _ui64To - ui64Pos );
			const uint64_t ui64Before = std::min<uint64_t>( m_ui32CharSize, ui64Pos - _ui64RangeStart );
			const uint64_t ui64After = std::min<uint64_t>( ui64Overlap + m_ui32CharSize, _ui64RangeEnd - (ui64Pos + ui64Take) );
			const uint64_t ui64Read = ui64Before + ui64Take + ui64After;
			if ( !_rReader( ui64Pos - ui64Before, size_t( ui64Read ), vBuffer ) || vBuffer.size() < ui64Read ) { return false; }

			const uint64_t ui64Base = ui64Pos - ui64Before;
			const size_t sBegin = size_t( std::max( ui64Next, ui64Pos ) - ui64Base );
			bool bContinue = true;
			ScanBuffer( vBuffer.data(), size_t( ui64Read ), sBegin, size_t( ui64Before + ui64Take ),
				ui64Base == _ui64RangeStart, ui64Base + ui64Read == _ui64RangeEnd, _bOverlap,
				[&]( size_t _sOff, size_t _sLen ) {
					const uint64_t ui64Addr = ui64Base + _sOff;
					ui64Next = ui64Addr + (_bOverlap ? 1 : _sLen);
					bContinue = _fFound( ui64Addr, _sLen );
					return bContinue;
				} );
			if ( !bContinue ) { return true; }
			ui64Pos += ui64Take;
			if ( _pProgress && !_pProgress( double( ui64Pos - _ui64From ) / double( _ui64To - _ui64From ) ) ) {
				_bStopped = true;
				return false;
			}
		}
		return true;
	}

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
	bool CHexFind::FindLast( const CReader &_rReader, uint64_t _ui64RangeStart, uint64_t _ui64RangeEnd, uint64_t _ui64From, uint64_t _ui64To,
		MX_FIND_MATCH &_fmMatch, const CProgress &_pProgress, bool &_bStopped ) const {
		_bStopped = false;
		_ui64To = std::min( _ui64To, _ui64RangeEnd );
		if ( _ui64From >= _ui64To ) { return false; }
		// Chunks are searched from the end; the last match in the first chunk that has one is the answer.
		for ( uint64_t ui64End = _ui64To; ui64End > _ui64From; ) {
			const uint64_t ui64Start = ui64End - std::min<uint64_t>( MX_HF_CHUNK, ui64End - _ui64From );
			bool bFound = false;
			bool bStopped = false;
			ScanRange( _rReader, _ui64RangeStart, _ui64RangeEnd, ui64Start, ui64End, true,
				[&]( uint64_t _ui64Addr, uint64_t _ui64Size ) {
					_fmMatch.ui64Addr = _ui64Addr;
					_fmMatch.ui64Size = _ui64Size;
					bFound = true;
					return true;
				}, nullptr, bStopped );
			if ( bFound ) { return true; }
			ui64End = ui64Start;
			if ( _pProgress && !_pProgress( double( _ui64To - ui64End ) / double( _ui64To - _ui64From ) ) ) {
				_bStopped = true;
				return false;
			}
		}
		return false;
	}

	/**
	 * Matches the pattern at a position.
	 *
	 * \param _pui8Data The data.
	 * \param _sSize The number of bytes from _pui8Data.
	 * \param _sElement The first element of the pattern to match.
	 * \param _sLen Receives the number of bytes matched.
	 * \return Returns true if the pattern matches.
	 **/
	bool CHexFind::MatchAt( const uint8_t * _pui8Data, size_t _sSize, size_t _sElement, size_t &_sLen ) const {
		size_t sUsed = 0;
		for ( ; _sElement < m_vPattern.size(); ++_sElement ) {
			const MX_ELEMENT & eThis = m_vPattern[_sElement];
			if ( eThis.bStar ) {
				// As few bytes as possible.
				for ( size_t J = 0; J <= m_ui32MaxWildcard && sUsed + J <= _sSize; ++J ) {
					size_t sRest = 0;
					if ( MatchAt( _pui8Data + sUsed + J, _sSize - sUsed - J, _sElement + 1, sRest ) ) {
						_sLen = sUsed + J + sRest;
						return true;
					}
				}
				return false;
			}
			if ( sUsed >= _sSize ) { return false; }
			const uint8_t ui8Byte = _pui8Data[sUsed] & eThis.ui8Mask;
			if ( ui8Byte != (eThis.ui8Val0 & eThis.ui8Mask) && ui8Byte != (eThis.ui8Val1 & eThis.ui8Mask) ) { return false; }
			++sUsed;
		}
		_sLen = sUsed;
		return true;
	}

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
	bool CHexFind::IsWholeWord( const uint8_t * _pui8Data, size_t _sSize, size_t _sOff, size_t _sLen, bool /*_bAtStart*/, bool /*_bAtEnd*/ ) const {
		// Outside the buffer is outside the searched range (ScanRange() reads a character on each side when there is one).
		if ( _sOff >= m_ui32CharSize && IsWordChar( _pui8Data + _sOff - m_ui32CharSize ) ) { return false; }
		if ( _sOff + _sLen + m_ui32CharSize <= _sSize && IsWordChar( _pui8Data + _sOff + _sLen ) ) { return false; }
		return true;
	}

	/**
	 * Determines whether the character at a position is part of a word (a letter, digit, or underscore).
	 *
	 * \param _pui8Char The character.
	 * \return Returns true for word characters.
	 **/
	bool CHexFind::IsWordChar( const uint8_t * _pui8Char ) const {
		wchar_t wcChar;
		if ( m_ui32CharSize == 2 ) {
			wcChar = m_bBigEndian ? wchar_t( (_pui8Char[0] << 8) | _pui8Char[1] ) : wchar_t( (_pui8Char[1] << 8) | _pui8Char[0] );
		}
		else if ( m_bEbcdic ) {
			if ( (*_pui8Char) < 0x40 ) { return false; }
			wcChar = CCharSets::m_wcEbcdic[(*_pui8Char)-0x40];
		}
		else {
			// Bytes over 127 are letters in most code pages and parts of letters in UTF-8.
			if ( (*_pui8Char) >= 0x80 ) { return true; }
			wcChar = wchar_t( (*_pui8Char) );
		}
		return wcChar == L'_' || ::IsCharAlphaNumericW( wcChar );
	}

	/**
	 * Gets how many bytes past the end of a chunk must be read so that matches starting in the chunk are complete.
	 *
	 * \return Returns the overlap in bytes.
	 **/
	size_t CHexFind::Overlap() const {
		switch ( m_mkKind ) {
			case MX_MK_PATTERN : { return m_sMaxLen ? m_sMaxLen - 1 : 0; }
			case MX_MK_FLOAT : { return sizeof( float ) - 1; }
			case MX_MK_DOUBLE : { return sizeof( double ) - 1; }
			case MX_MK_REGEX : { return size_t( MX_HF_REGEX_OVERLAP ); }
		}
		return 0;
	}

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
	bool CHexFind::Encode( const std::wstring &_wsText, MX_FIND_TYPE _ftType, CCharSets::MX_CHAR_SETS _csCharSet, bool _bBigEndian,
		std::vector<uint8_t> &_vBytes ) {
		_vBytes.clear();
		if ( _ftType == MX_FT_TEXT ) {
			switch ( _csCharSet ) {
				case CCharSets::MX_CS_UNICODE : { _ftType = MX_FT_UNICODE; break; }
				case CCharSets::MX_CS_EBCDIC : { _ftType = MX_FT_EBCDIC; break; }
				default : {}
			}
		}
		switch ( _ftType ) {
			case MX_FT_UNICODE : {
				for ( wchar_t wcChar : _wsText ) {
					const uint8_t ui8Lo = uint8_t( wcChar & 0xFF ), ui8Hi = uint8_t( (wcChar >> 8) & 0xFF );
					_vBytes.push_back( _bBigEndian ? ui8Hi : ui8Lo );
					_vBytes.push_back( _bBigEndian ? ui8Lo : ui8Hi );
				}
				return true;
			}
			case MX_FT_EBCDIC : {
				for ( wchar_t wcChar : _wsText ) {
					bool bFound = false;
					for ( uint32_t I = 0; I < 0xC0; ++I ) {
						if ( CCharSets::m_wcEbcdic[I] == wcChar && wcChar != L'.' ) {
							_vBytes.push_back( uint8_t( 0x40 + I ) );
							bFound = true;
							break;
						}
					}
					if ( !bFound && wcChar == L'.' ) {
						// '.' is also what unprintable characters show as; the real one is 0x4B.
						_vBytes.push_back( 0x4B );
						bFound = true;
					}
					if ( !bFound ) { return false; }
				}
				return true;
			}
			default : {
				UINT uiCodePage = 1252;
				if ( _ftType == MX_FT_TEXT && size_t( _csCharSet ) < CCharSets::Total() ) {
					uiCodePage = UINT( CCharSets::m_csSets[_csCharSet].ui32Code );
				}
				if ( _wsText.empty() ) { return true; }
				const bool bUtf = uiCodePage == CP_UTF8 || uiCodePage == CP_UTF7;
				BOOL bDefault = FALSE;
				const int iLen = ::WideCharToMultiByte( uiCodePage, bUtf ? 0 : WC_NO_BEST_FIT_CHARS, _wsText.c_str(), int( _wsText.size() ),
					nullptr, 0, nullptr, bUtf ? nullptr : &bDefault );
				if ( iLen <= 0 || bDefault ) { return false; }
				_vBytes.resize( size_t( iLen ) );
				::WideCharToMultiByte( uiCodePage, bUtf ? 0 : WC_NO_BEST_FIT_CHARS, _wsText.c_str(), int( _wsText.size() ),
					reinterpret_cast<LPSTR>(_vBytes.data()), iLen, nullptr, bUtf ? nullptr : &bDefault );
				return !bDefault;
			}
		}
	}

	/**
	 * Builds the pattern for text.
	 *
	 * \param _fpParms The search.
	 * \param _wsText The text (without a type specifier).
	 * \param _swsError Receives a description of the problem on failure.
	 * \return Returns true if the pattern was built.
	 **/
	bool CHexFind::CompileText( const MX_FIND_PARMS &_fpParms, const std::wstring &_wsText, CSecureWString &_swsError ) {
		std::vector<uint8_t> vExact, vLower, vUpper;
		for ( size_t I = 0; I < _wsText.size(); ) {
			wchar_t wcChar = _wsText[I];
			if ( _fpParms.bWildcards ) {
				if ( wcChar == L'\\' && I + 1 < _wsText.size() && (_wsText[I+1] == L'*' || _wsText[I+1] == L'?' || _wsText[I+1] == L'\\') ) {
					// An escaped literal.
					wcChar = _wsText[++I];
				}
				else if ( wcChar == L'*' ) {
					MX_ELEMENT eStar;
					eStar.bStar = true;
					m_vPattern.push_back( eStar );
					++I;
					continue;
				}
				else if ( wcChar == L'?' ) {
					// Any one character.
					MX_ELEMENT eAny;
					eAny.ui8Mask = 0;
					for ( uint32_t J = 0; J < m_ui32CharSize; ++J ) { m_vPattern.push_back( eAny ); }
					++I;
					continue;
				}
			}

			// A character (both halves of a surrogate pair together).
			std::wstring wsChar( 1, wcChar );
			if ( wcChar >= 0xD800 && wcChar <= 0xDBFF && I + 1 < _wsText.size() && _wsText[I+1] >= 0xDC00 && _wsText[I+1] <= 0xDFFF ) {
				wsChar.push_back( _wsText[++I] );
			}
			++I;
			if ( !Encode( wsChar, m_ftType, _fpParms.csCharSet, _fpParms.bBigEndian, vExact ) || vExact.empty() ) {
				_swsError = _DEC_WS_E67818BF_The_text_cannot_be_represented_in_the_character_set_;
				return false;
			}
			bool bCases = false;
			if ( !_fpParms.bMatchCase && wsChar.size() == 1 ) {
				// The Windows case mappings cover all of Unicode (towlower()/towupper() only cover ASCII in the "C" locale).
				std::wstring wsLower( wsChar ), wsUpper( wsChar );
				::CharLowerBuffW( wsLower.data(), DWORD( wsLower.size() ) );
				::CharUpperBuffW( wsUpper.data(), DWORD( wsUpper.size() ) );
				// Both cases are used when they have the same length (true for nearly all letters).
				bCases = Encode( wsLower, m_ftType, _fpParms.csCharSet, _fpParms.bBigEndian, vLower ) &&
					Encode( wsUpper, m_ftType, _fpParms.csCharSet, _fpParms.bBigEndian, vUpper ) &&
					vLower.size() == vUpper.size() && vLower.size() == vExact.size();
			}
			for ( size_t J = 0; J < vExact.size(); ++J ) {
				MX_ELEMENT eByte;
				eByte.ui8Val0 = bCases ? vLower[J] : vExact[J];
				eByte.ui8Val1 = bCases ? vUpper[J] : vExact[J];
				m_vPattern.push_back( eByte );
			}
		}
		UpdateLengths();
		if ( !m_sMinLen ) {
			// Wildcards alone.
			_swsError = _DEC_WS_C84B5768_Nothing_to_find_;
			return false;
		}
		return true;
	}

	/**
	 * Builds the pattern for hex bytes.
	 *
	 * \param _wsText The text (without a type specifier).
	 * \param _bWildcards '?' and '*' are wildcards.
	 * \param _swsError Receives a description of the problem on failure.
	 * \return Returns true if the pattern was built.
	 **/
	bool CHexFind::CompileHex( const std::wstring &_wsText, bool _bWildcards, CSecureWString &_swsError ) {
		// Values from 0 to 15 are digits; 16 is '?'.
		std::vector<uint8_t> vNibbles;
		auto fFlush = [&]() {
			if ( vNibbles.size() & 1 ) { vNibbles.insert( vNibbles.begin(), uint8_t( 0 ) ); }
			for ( size_t I = 0; I < vNibbles.size(); I += 2 ) {
				MX_ELEMENT eByte;
				const uint8_t ui8Hi = vNibbles[I], ui8Lo = vNibbles[I+1];
				eByte.ui8Mask = uint8_t( ((ui8Hi == 16) ? 0x00 : 0xF0) | ((ui8Lo == 16) ? 0x00 : 0x0F) );
				eByte.ui8Val0 = eByte.ui8Val1 = uint8_t( (((ui8Hi == 16) ? 0 : ui8Hi) << 4) | ((ui8Lo == 16) ? 0 : ui8Lo) );
				m_vPattern.push_back( eByte );
			}
			vNibbles.clear();
		};
		for ( size_t I = 0; I < _wsText.size(); ++I ) {
			const wchar_t wcChar = _wsText[I];
			if ( std::iswspace( wcChar ) ) {
				fFlush();
				continue;
			}
			// "0x" and "\x" prefixes.
			if ( vNibbles.empty() && (wcChar == L'0' || wcChar == L'\\') && I + 1 < _wsText.size() &&
				(_wsText[I+1] == L'x' || _wsText[I+1] == L'X') ) {
				++I;
				continue;
			}
			if ( _bWildcards && wcChar == L'*' ) {
				fFlush();
				MX_ELEMENT eStar;
				eStar.bStar = true;
				m_vPattern.push_back( eStar );
				continue;
			}
			if ( _bWildcards && wcChar == L'?' ) {
				vNibbles.push_back( 16 );
				continue;
			}
			uint8_t ui8Val;
			if ( wcChar >= L'0' && wcChar <= L'9' ) { ui8Val = uint8_t( wcChar - L'0' ); }
			else if ( wcChar >= L'A' && wcChar <= L'F' ) { ui8Val = uint8_t( wcChar - L'A' + 10 ); }
			else if ( wcChar >= L'a' && wcChar <= L'f' ) { ui8Val = uint8_t( wcChar - L'a' + 10 ); }
			else {
				_swsError = _DEC_WS_F53636B9_The_hex_bytes_are_not_valid_;
				return false;
			}
			vNibbles.push_back( ui8Val );
		}
		fFlush();
		UpdateLengths();
		if ( !m_sMinLen ) {
			_swsError = _DEC_WS_C84B5768_Nothing_to_find_;
			return false;
		}
		return true;
	}

	/**
	 * Builds the pattern (or value) for a number.
	 *
	 * \param _fpParms The search.
	 * \param _wsText The text (without a type specifier).
	 * \param _swsError Receives a description of the problem on failure.
	 * \return Returns true if the pattern was built.
	 **/
	bool CHexFind::CompileNumber( const MX_FIND_PARMS &_fpParms, const std::wstring &_wsText, CSecureWString &_swsError ) {
		// Trimmed.
		size_t sStart = 0, sEnd = _wsText.size();
		while ( sStart < sEnd && std::iswspace( _wsText[sStart] ) ) { ++sStart; }
		while ( sEnd > sStart && std::iswspace( _wsText[sEnd-1] ) ) { --sEnd; }
		const std::wstring wsNum = _wsText.substr( sStart, sEnd - sStart );
		if ( wsNum.empty() ) {
			_swsError = _DEC_WS_C84B5768_Nothing_to_find_;
			return false;
		}

		if ( m_ftType == MX_FT_FLOAT || m_ftType == MX_FT_DOUBLE ) {
			wchar_t * pwcEnd = nullptr;
			const double dVal = std::wcstod( wsNum.c_str(), &pwcEnd );
			if ( !pwcEnd || (*pwcEnd) != L'\0' || !std::isfinite( dVal ) ) {
				_swsError = _DEC_WS_8C348BE9_The_number_is_not_valid_;
				return false;
			}
			if ( !std::isfinite( _fpParms.dTolerance ) ) {
				_swsError = _DEC_WS_055C5BB0_The_tolerance_is_not_valid_;
				return false;
			}
			m_mkKind = (m_ftType == MX_FT_FLOAT) ? MX_MK_FLOAT : MX_MK_DOUBLE;
			// A float search compares against the value as a float.
			m_dValue = (m_ftType == MX_FT_FLOAT) ? double( float( dVal ) ) : dVal;
			m_dTolerance = std::fabs( _fpParms.dTolerance );
			m_sMinLen = m_sMaxLen = (m_ftType == MX_FT_FLOAT) ? sizeof( float ) : sizeof( double );
			return true;
		}

		// Integers: decimal or 0x hexadecimal, with an optional sign.
		size_t sPos = 0;
		bool bNeg = false;
		if ( wsNum[sPos] == L'-' || wsNum[sPos] == L'+' ) { bNeg = wsNum[sPos++] == L'-'; }
		uint32_t ui32Base = 10;
		if ( sPos + 1 < wsNum.size() && wsNum[sPos] == L'0' && (wsNum[sPos+1] == L'x' || wsNum[sPos+1] == L'X') ) {
			ui32Base = 16;
			sPos += 2;
		}
		if ( sPos >= wsNum.size() ) {
			_swsError = _DEC_WS_8C348BE9_The_number_is_not_valid_;
			return false;
		}
		uint64_t ui64Val = 0;
		for ( ; sPos < wsNum.size(); ++sPos ) {
			const wchar_t wcChar = wsNum[sPos];
			uint32_t ui32Digit;
			if ( wcChar >= L'0' && wcChar <= L'9' ) { ui32Digit = uint32_t( wcChar - L'0' ); }
			else if ( ui32Base == 16 && wcChar >= L'A' && wcChar <= L'F' ) { ui32Digit = uint32_t( wcChar - L'A' + 10 ); }
			else if ( ui32Base == 16 && wcChar >= L'a' && wcChar <= L'f' ) { ui32Digit = uint32_t( wcChar - L'a' + 10 ); }
			else {
				_swsError = _DEC_WS_8C348BE9_The_number_is_not_valid_;
				return false;
			}
			if ( ui64Val > (UINT64_MAX - ui32Digit) / ui32Base ) {
				_swsError = _DEC_WS_58735361_The_number_is_out_of_range_for_the_type_;
				return false;
			}
			ui64Val = ui64Val * ui32Base + ui32Digit;
		}

		uint32_t ui32Bytes = 1;
		switch ( m_ftType ) {
			case MX_FT_INT16 : {}									MX_FALLTHROUGH
			case MX_FT_UINT16 : { ui32Bytes = 2; break; }
			case MX_FT_INT32 : {}									MX_FALLTHROUGH
			case MX_FT_UINT32 : { ui32Bytes = 4; break; }
			case MX_FT_INT64 : {}									MX_FALLTHROUGH
			case MX_FT_UINT64 : { ui32Bytes = 8; break; }
			default : {}
		}
		// Anything that fits the size is allowed (from the most negative signed value to the largest unsigned value).
		const uint64_t ui64Max = (ui32Bytes == 8) ? UINT64_MAX : ((1ULL << (ui32Bytes * 8)) - 1ULL);
		const uint64_t ui64MaxNeg = 1ULL << (ui32Bytes * 8 - 1);
		if ( bNeg ? (ui64Val > ui64MaxNeg) : (ui64Val > ui64Max) ) {
			_swsError = _DEC_WS_58735361_The_number_is_out_of_range_for_the_type_;
			return false;
		}
		if ( bNeg ) { ui64Val = uint64_t( 0 ) - ui64Val; }
		for ( uint32_t I = 0; I < ui32Bytes; ++I ) {
			const uint32_t ui32Shift = (_fpParms.bBigEndian ? (ui32Bytes - 1 - I) : I) * 8;
			MX_ELEMENT eByte;
			eByte.ui8Val0 = eByte.ui8Val1 = uint8_t( (ui64Val >> ui32Shift) & 0xFF );
			m_vPattern.push_back( eByte );
		}
		UpdateLengths();
		return true;
	}

	/**
	 * Prepares a regular expression.
	 *
	 * \param _fpParms The search.
	 * \param _wsText The expression (without a type specifier).
	 * \param _swsError Receives a description of the problem on failure.
	 * \return Returns true if the expression is valid.
	 **/
	bool CHexFind::CompileRegex( const MX_FIND_PARMS &_fpParms, const std::wstring &_wsText, CSecureWString &_swsError ) {
		// The encoding of the data.
		CCharSets::MX_CHAR_SETS csSet = CCharSets::MX_CS_ASCII_ANSI;
		if ( m_ftType == MX_FT_TEXT ) { csSet = _fpParms.csCharSet; }
		else if ( m_ftType == MX_FT_UNICODE ) { csSet = CCharSets::MX_CS_UNICODE; }
		else if ( m_ftType == MX_FT_EBCDIC ) { csSet = CCharSets::MX_CS_EBCDIC; }

		::OnigEncodingType * poetEncoding = nullptr;
		std::vector<uint8_t> vPattern;
		if ( csSet == CCharSets::MX_CS_UNICODE ) {
			poetEncoding = _fpParms.bBigEndian ? ONIG_ENCODING_UTF16_BE : ONIG_ENCODING_UTF16_LE;
			Encode( _wsText, MX_FT_UNICODE, csSet, _fpParms.bBigEndian, vPattern );
		}
		else if ( csSet == CCharSets::MX_CS_UTF8 ) {
			poetEncoding = ONIG_ENCODING_UTF8;
			if ( !Encode( _wsText, MX_FT_TEXT, csSet, false, vPattern ) ) {
				_swsError = _DEC_WS_E67818BF_The_text_cannot_be_represented_in_the_character_set_;
				return false;
			}
		}
		else if ( csSet != CCharSets::MX_CS_EBCDIC && size_t( csSet ) < CCharSets::Total() && CCharSets::m_csSets[csSet].bSinglePage ) {
			// Single-byte code pages are matched byte for byte.
			poetEncoding = ONIG_ENCODING_ISO_8859_1;
			if ( !Encode( _wsText, (m_ftType == MX_FT_TEXT) ? MX_FT_TEXT : MX_FT_ASCII, csSet, false, vPattern ) ) {
				_swsError = _DEC_WS_E67818BF_The_text_cannot_be_represented_in_the_character_set_;
				return false;
			}
		}
		else {
			_swsError = _DEC_WS_6A35309F_Regular_expressions_can_only_be_used_to_find_text_in_ASCII__ANSI__UTF_8__or_Unicode_;
			return false;
		}
		if ( vPattern.empty() ) {
			_swsError = _DEC_WS_C84B5768_Nothing_to_find_;
			return false;
		}

		std::unique_ptr<MX_REGEX> prRegex = std::make_unique<MX_REGEX>();
		::OnigErrorInfo oeiInfo = {};
		const int iRes = ::onig_new( &prRegex->prReg, vPattern.data(), vPattern.data() + vPattern.size(),
			_fpParms.bMatchCase ? ONIG_OPTION_NONE : ONIG_OPTION_IGNORECASE, poetEncoding, ONIG_SYNTAX_DEFAULT, &oeiInfo );
		if ( iRes != ONIG_NORMAL ) {
			prRegex->prReg = nullptr;
			OnigUChar ucMsg[ONIG_MAX_ERROR_MESSAGE_LEN] = {};
			::onig_error_code_to_str( ucMsg, iRes, &oeiInfo );
			_swsError = _DEC_WS_206A812D_The_regular_expression_is_not_valid__;
			for ( size_t I = 0; I < ONIG_MAX_ERROR_MESSAGE_LEN && ucMsg[I]; ++I ) { _swsError.push_back( wchar_t( ucMsg[I] ) ); }
			return false;
		}
		prRegex->porRegion = ::onig_region_new();
		if ( !prRegex->porRegion ) {
			_swsError = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
		m_prRegex = std::move( prRegex );
		m_mkKind = MX_MK_REGEX;
		m_sMinLen = 1;
		m_sMaxLen = size_t( MX_HF_REGEX_OVERLAP );
		return true;
	}

	/**
	 * Updates m_sMinLen and m_sMaxLen from the pattern.
	 **/
	void CHexFind::UpdateLengths() {
		m_sMinLen = m_sMaxLen = 0;
		for ( const auto & eThis : m_vPattern ) {
			if ( eThis.bStar ) { m_sMaxLen += m_ui32MaxWildcard; }
			else {
				++m_sMinLen;
				++m_sMaxLen;
			}
		}
	}

}	// namespace mx

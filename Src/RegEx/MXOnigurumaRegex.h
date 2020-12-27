#pragma once

#include "Oniguruma/oniguruma.h"

namespace mx {

	class COnigurumaRegex {
	public :
		COnigurumaRegex( const OnigUChar * _pucPatternStart, size_t _sPatternLen, ::OnigOptionType _ootOptions, ::OnigEncodingType * _poetEncoding = ONIG_ENCODING_UTF8, ::OnigSyntaxType * _postSyntax = ONIG_SYNTAX_DEFAULT ) {
			m_iResult = ::onig_new( &m_prReg, _pucPatternStart,
			_pucPatternStart + _sPatternLen,
			_ootOptions,
			_poetEncoding,
			//ONIG_ENCODING_UTF16_LE,
			_postSyntax,
			&m_oeiInfo );
		}
		~COnigurumaRegex() {
			::onig_free( m_prReg );
			m_prReg = nullptr;
			m_iResult = ONIG_NORMAL;
		}


		// == Functions.
		// Checks the given buffer for a match.  Returns >= 0 on success, < 0 (ONIG_MISMATCH) on failure.
		inline int				Match( const UChar * _pucPatternStart, size_t _sPatternLen, size_t _sAt, OnigRegion * _orRegion, OnigOptionType _ootOption );

		// Gets the result.
		int						Get() { return m_iResult; }
		
		// Gets the error information.
		const ::OnigErrorInfo &	Error() { return m_oeiInfo; }

		// Gets the regex pointer.
		const ::regex_t *		Regex() const { return m_prReg; }


	protected :
		// == Members.
		// The regex object.
		::regex_t *				m_prReg;
		// The error result.
		::OnigErrorInfo			m_oeiInfo;
		// The result.
		int						m_iResult;
	};


	// == Functions.
	// Checks the given buffer for a match.  Returns >= 0 on success, < 0 (ONIG_MISMATCH) on failure.
	inline int COnigurumaRegex::Match( const UChar * _pucPatternStart, size_t _sPatternLen, size_t _sAt, OnigRegion * _orRegion, OnigOptionType _ootOption ) {
		return ::onig_match( m_prReg,
			_pucPatternStart,
			_pucPatternStart + _sPatternLen,
			_pucPatternStart + _sAt,
			_orRegion, _ootOption );
	}

}	// namespace mx

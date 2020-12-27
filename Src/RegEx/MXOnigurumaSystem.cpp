#include "MXOnigurumaSystem.h"


namespace mx {

	// == Members.
	// Number of instances.
	LONG COnigurumaSystem::m_lLiveCount = 0;

	// == Functions.
	// Converts from MHS regex options to Oniguruma's.
	OnigOptionType COnigurumaSystem::RegexOptionsToLibrary( uint32_t _uiFLags ) {
		const struct {
			uint32_t				uiMhsFlag;
			uint32_t				uiOniFlag;
		} sTable[] = {
			{ CUtilities::MX_RO_SINGLE_LINE,		ONIG_OPTION_SINGLELINE },
			{ CUtilities::MX_RO_MULTI_LINE,			ONIG_OPTION_MULTILINE },
			{ CUtilities::MX_RO_EXTENDED,			ONIG_OPTION_EXTEND },
			{ CUtilities::MX_RO_FIND_LONGEST,		ONIG_OPTION_FIND_LONGEST },
			{ CUtilities::MX_RO_NEGATE_SINGE_LINE,	ONIG_OPTION_NEGATE_SINGLELINE },
			{ CUtilities::MX_RO_IGNORE_CASE,		ONIG_OPTION_IGNORECASE },
		};
		unsigned int uiRet = ONIG_OPTION_DONT_CAPTURE_GROUP | ONIG_OPTION_FIND_NOT_EMPTY;
		for ( size_t I = MX_ELEMENTS( sTable ); I--; ) {
			if ( _uiFLags & sTable[I].uiMhsFlag ) {
				uiRet |= sTable[I].uiOniFlag;
			}
		}
		return uiRet;
	}

	// Converts from MHX regex flavors to Oniguruma's.
	::OnigSyntaxType * COnigurumaSystem::RegexFlavorToLibrary( CUtilities::MX_REGEX_FLAVORS _rfFlavor ) {
		const struct {
			CUtilities::MX_REGEX_FLAVORS			rfMhsFlavor;
			::OnigSyntaxType *						postSyntax;
		} sTable[] = {
			{ CUtilities::MX_RF_ASIS,				ONIG_SYNTAX_ASIS },
			{ CUtilities::MX_RF_POSIX_BASIC,		ONIG_SYNTAX_POSIX_BASIC },
			{ CUtilities::MX_RF_POSIX_EXTENDED,		ONIG_SYNTAX_POSIX_EXTENDED },
			{ CUtilities::MX_RF_EMACS,				ONIG_SYNTAX_EMACS },
			{ CUtilities::MX_RF_GREP,				ONIG_SYNTAX_GREP },
			{ CUtilities::MX_RF_GNU_REGEX,			ONIG_SYNTAX_GNU_REGEX },
			{ CUtilities::MX_RF_JAVA,				ONIG_SYNTAX_JAVA },
			{ CUtilities::MX_RF_PERL,				ONIG_SYNTAX_PERL },
			{ CUtilities::MX_RF_PERL_NG,			ONIG_SYNTAX_PERL_NG },
			{ CUtilities::MX_RF_RUBY,				ONIG_SYNTAX_RUBY },
			{ CUtilities::MX_RF_ONIGURUMA,			ONIG_SYNTAX_ONIGURUMA },
		};
		unsigned int uiRet = ONIG_OPTION_DONT_CAPTURE_GROUP | ONIG_OPTION_FIND_NOT_EMPTY;
		for ( size_t I = MX_ELEMENTS( sTable ); I--; ) {
			if ( _rfFlavor == sTable[I].rfMhsFlavor ) { return sTable[I].postSyntax; }
		}
		return ONIG_SYNTAX_ONIGURUMA;
	}

	// Converts from MHS regex encodings to Oniguruma's.
	::OnigEncodingType * COnigurumaSystem::RegexEncodingToLibrary( CUtilities::MX_REGEX_ENCODING _reEncoding ) {
		const struct {
			CUtilities::MX_REGEX_ENCODING			reMhsEncoding;
			::OnigEncodingType *					poetEncoding;
		} sTable[] = {
			{ CUtilities::MX_RE_ASCII,				ONIG_ENCODING_ASCII },
			{ CUtilities::MX_RE_UTF8,				ONIG_ENCODING_UTF8 },
			{ CUtilities::MX_RE_UTF16_BE,			ONIG_ENCODING_UTF16_BE },
			{ CUtilities::MX_RE_UTF16_LE,			ONIG_ENCODING_UTF16_LE },
			{ CUtilities::MX_RE_UTF32_BE,			ONIG_ENCODING_UTF32_BE },
			{ CUtilities::MX_RE_UTF32_LE,			ONIG_ENCODING_UTF32_LE },
			{ CUtilities::MX_RE_ISO_8859_1,			ONIG_ENCODING_ISO_8859_1 },
			{ CUtilities::MX_RE_ISO_8859_2,			ONIG_ENCODING_ISO_8859_2 },
			{ CUtilities::MX_RE_ISO_8859_3,			ONIG_ENCODING_ISO_8859_3 },
			{ CUtilities::MX_RE_ISO_8859_4,			ONIG_ENCODING_ISO_8859_4 },
			{ CUtilities::MX_RE_ISO_8859_5,			ONIG_ENCODING_ISO_8859_5 },
			{ CUtilities::MX_RE_ISO_8859_6,			ONIG_ENCODING_ISO_8859_6 },
			{ CUtilities::MX_RE_ISO_8859_7,			ONIG_ENCODING_ISO_8859_7 },
			{ CUtilities::MX_RE_ISO_8859_8,			ONIG_ENCODING_ISO_8859_8 },
			{ CUtilities::MX_RE_ISO_8859_9,			ONIG_ENCODING_ISO_8859_9 },
			{ CUtilities::MX_RE_ISO_8859_10,		ONIG_ENCODING_ISO_8859_10 },
			{ CUtilities::MX_RE_ISO_8859_11,		ONIG_ENCODING_ISO_8859_11 },
			{ CUtilities::MX_RE_ISO_8859_13,		ONIG_ENCODING_ISO_8859_13 },
			{ CUtilities::MX_RE_ISO_8859_14,		ONIG_ENCODING_ISO_8859_14 },
			{ CUtilities::MX_RE_ISO_8859_15,		ONIG_ENCODING_ISO_8859_15 },
			{ CUtilities::MX_RE_ISO_8859_16,		ONIG_ENCODING_ISO_8859_16 },
			{ CUtilities::MX_RE_EUC_JP,				ONIG_ENCODING_EUC_JP },
			{ CUtilities::MX_RE_EUC_TW,				ONIG_ENCODING_EUC_TW },
			{ CUtilities::MX_RE_EUC_KR,				ONIG_ENCODING_EUC_KR },
			{ CUtilities::MX_RE_EUC_CN,				ONIG_ENCODING_EUC_CN },
			{ CUtilities::MX_RE_S_JIS,				ONIG_ENCODING_SJIS },
			{ CUtilities::MX_RE_KOI_8,				ONIG_ENCODING_KOI8 },
			{ CUtilities::MX_RE_KOI8_R,				ONIG_ENCODING_KOI8_R },
			{ CUtilities::MX_RE_WINDOWS_1251,		ONIG_ENCODING_CP1251 },
			{ CUtilities::MX_RE_BIG_5,				ONIG_ENCODING_BIG5 },
			{ CUtilities::MX_RE_GB_18030,			ONIG_ENCODING_GB18030 },
		};
		unsigned int uiRet = ONIG_OPTION_DONT_CAPTURE_GROUP | ONIG_OPTION_FIND_NOT_EMPTY;
		for ( size_t I = MX_ELEMENTS( sTable ); I--; ) {
			if ( _reEncoding == sTable[I].reMhsEncoding ) {
				return sTable[I].poetEncoding;
			}
		}
		return ONIG_ENCODING_UTF8;
	}

}	// namespace mx

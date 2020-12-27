#pragma once

#include "../MXMhsX.h"
#include "../Utilities/MXUtilities.h"
#include "Oniguruma/oniguruma.h"

namespace mx {

	class COnigurumaSystem {
	public :
		COnigurumaSystem() {
			if ( ::InterlockedIncrement( &m_lLiveCount ) == 1 ) {
				::OnigEncoding oeEncodings[] = {
					ONIG_ENCODING_ASCII,
					ONIG_ENCODING_ISO_8859_1,
					ONIG_ENCODING_ISO_8859_2,
					ONIG_ENCODING_ISO_8859_3,
					ONIG_ENCODING_ISO_8859_4,
					ONIG_ENCODING_ISO_8859_5,
					ONIG_ENCODING_ISO_8859_6,
					ONIG_ENCODING_ISO_8859_7,
					ONIG_ENCODING_ISO_8859_8,
					ONIG_ENCODING_ISO_8859_9,
					ONIG_ENCODING_ISO_8859_10,
					ONIG_ENCODING_ISO_8859_11,
					ONIG_ENCODING_ISO_8859_13,
					ONIG_ENCODING_ISO_8859_14,
					ONIG_ENCODING_ISO_8859_15,
					ONIG_ENCODING_ISO_8859_16,
					ONIG_ENCODING_UTF8,
					ONIG_ENCODING_UTF16_BE,
					ONIG_ENCODING_UTF16_LE,
					ONIG_ENCODING_UTF32_BE,
					ONIG_ENCODING_UTF32_LE,
					ONIG_ENCODING_EUC_JP,
					ONIG_ENCODING_EUC_TW,
					ONIG_ENCODING_EUC_KR,
					ONIG_ENCODING_EUC_CN,
					ONIG_ENCODING_SJIS,
					ONIG_ENCODING_KOI8,
					ONIG_ENCODING_KOI8_R,
					ONIG_ENCODING_CP1251,
					ONIG_ENCODING_BIG5,
					ONIG_ENCODING_GB18030,
				};
				::onig_initialize( oeEncodings, MX_ELEMENTS( oeEncodings ) );
			}
		}
		~COnigurumaSystem() {
			if ( ::InterlockedDecrement( &m_lLiveCount ) == 0 ) {
				::onig_end();
			}
		}


		// == Types.
		// 


		// == Functions.
		// Converts from MHS regex options to Oniguruma's.
		static OnigOptionType			RegexOptionsToLibrary( uint32_t _uiFLags );

		// Converts from MHX regex flavors to Oniguruma's.
		static ::OnigSyntaxType *			RegexFlavorToLibrary( CUtilities::MX_REGEX_FLAVORS _rfFlavor );

		// Converts from MHS regex encodings to Oniguruma's.
		static ::OnigEncodingType *		RegexEncodingToLibrary( CUtilities::MX_REGEX_ENCODING _reEncoding );

	protected :
		// == Members.
		// Number of instances.
		static LONG						m_lLiveCount;
	};

}	// namespace mx

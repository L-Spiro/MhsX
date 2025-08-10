#include "MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"

namespace mx {

	// Decodes to CSecureString, creating a new object.
	CSecureString CStringDecoder::DecodeToString( const char * _pcIn, size_t _sInLen ) {
		CSecureString sTemp;
		try {
			sTemp.resize( _sInLen );
			Decode( _pcIn, _sInLen, const_cast<char *>(sTemp.c_str()) );
		}
		catch ( ... ) {}
		return sTemp;
	}

	// Decodes to CSecureWString, creating a new object.
	CSecureWString CStringDecoder::DecodeToWString( const char * _pcIn, size_t _sInLen ) {
		try {
			CSecureString sTemp = DecodeToString( _pcIn, _sInLen );
			return ee::CExpEval::StringToWString( sTemp );
		}
		catch ( ... ) { return CSecureWString(); }
	}

}	// namespace mx

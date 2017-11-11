#include "MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"

namespace mx {

	// Decodes to std::string, creating a new object.
	std::string CStringDecoder::DecodeToString( const char * _pcIn, size_t _sInLen ) {
		std::string sTemp;
		sTemp.resize( _sInLen );
		Decode( _pcIn, _sInLen, const_cast<char *>(sTemp.c_str()) );
		return sTemp;
	}

	// Decodes to std::wstring, creating a new object.
	std::wstring CStringDecoder::DecodeToWString( const char * _pcIn, size_t _sInLen ) {
		std::string sTemp = DecodeToString( _pcIn, _sInLen );
		return CUtilities::StringToWString( sTemp );
	}

}	// namespace mx

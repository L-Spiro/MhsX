#pragma once

#include <cstring>
#include "MXStringMacros.h"

namespace mx {

	class CStringDecoder {
	public :
		// == Functions.
		// Decodes the given string into the given target.  Can cause buffer overflow, so do be careful.
		//	_pcIn should be one of the _T_* macros in MXStringMacros.h.
		//	_sInLen should be one of the _LEN_* macros in MXStringMacros.h.
		//	_tOut should be at least _T_MAX_LEN characters long.
		template <typename _tOutType>
		static _tOutType *		Decode( const char * _pcIn, size_t _sInLen, _tOutType * _tOut ) {
			// See MXStringMacros.h for the key.
			static const char * pcKey = "x20en7r90872ne9";
			static size_t sLen = std::strlen( pcKey );
			_tOutType * ptRet = _tOut;
			for ( size_t I = 0; I < _sInLen; ++I ) {
				(*_tOut++) = static_cast<char>(_pcIn[I] + pcKey[I%sLen]);
			}
			(*_tOut) = static_cast<_tOutType>('\0');
			return ptRet;
		}
	};

}	// namespace mx
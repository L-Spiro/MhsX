#pragma once

#include "MXStringMacros.h"
#include <cstring>
#include <string>

#define MX_MAKE_WCHAR2( STRING )			L ## STRING
#define MX_MAKE_WCHAR( STRING )				MX_MAKE_WCHAR2( STRING )

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
				(*_tOut++) = static_cast<_tOutType>(_pcIn[I] + pcKey[I%sLen]);
			}
			(*_tOut) = static_cast<_tOutType>('\0');
			return ptRet;
		}

		// Decodes to an std::string or std::wstring object.
		template <typename _tOutType>
		static _tOutType &		Decode( const char * _pcIn, size_t _sInLen, _tOutType &_otRet ) {
			_otRet.resize( _sInLen );
			Decode( _pcIn, _sInLen, const_cast<_tOutType::value_type *>(_otRet.c_str()) );
			return _otRet;
		}

		// Decodes to std::string, creating a new object.
		static std::string		DecodeToString( const char * _pcIn, size_t _sInLen ) {
			std::string sTemp;
			return Decode( _pcIn, _sInLen, sTemp );
		}

		// Decodes to std::wstring, creating a new object.
		static std::wstring		DecodeToWString( const char * _pcIn, size_t _sInLen ) {
			std::wstring sTemp;
			return Decode( _pcIn, _sInLen, sTemp );
		}
	};

}	// namespace mx
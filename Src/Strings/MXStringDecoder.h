#pragma once

#include "MXSecureString.h"
#include "MXSecureWString.h"
#include "MXStringMacros.h"
#include <cstring>
#include <string>

#if 0
#define MX_MAKE_WCHAR2( STRING )			L ## STRING
#define MX_MAKE_WCHAR( STRING )				MX_MAKE_WCHAR2( STRING )
#else
#define MX_MAKE_WCHAR( STRING )				reinterpret_cast<const WCHAR *>(STRING)
#endif	// #if 0

namespace mx {

	class CStringDecoder {
	public :
		// == Functions.
		// Decodes the given string into the given target.  Can cause buffer overflow, so do be careful.
		//	_pcIn should be one of the _T_* macros in MXStringMacros.h.
		//	_sInLen should be one of the _LEN_* macros in MXStringMacros.h.
		//	_tOut should be at least _T_MAX_LEN characters long.
		template <typename _tOutType>
		static _tOutType *					Decode( const char * _pcIn, size_t _sInLen, _tOutType * _tOut ) {
			// See MXStringMacros.h for the key.
			// ANTICHEAT: Change the key in EncodeStrings.bat and run it to re-encode all strings in the project.
			//	This changes how they are stored in the .EXE and at run-time, making it impossible to detect us via "known string" searches.
			//	The key can be any length and contain any values but \0 (terminating null).
			static const char * pcKey = MX_STRING_ENCRYPT_KEY;
			static size_t sLen = std::strlen( pcKey );
			_tOutType * ptRet = _tOut;
			for ( size_t I = 0; I < _sInLen; ++I ) {
				(*_tOut++) = static_cast<_tOutType>(_pcIn[I] + pcKey[I%sLen]);
			}
			(*_tOut) = static_cast<_tOutType>('\0');
			return ptRet;
		}

		// Decodes to a CSecureString.
		static CSecureString &				Decode( const char * _pcIn, size_t _sInLen, CSecureString &_otRet ) {
			_otRet.resize( _sInLen );
			Decode( _pcIn, _sInLen, const_cast<char *>(_otRet.c_str()) );
			return _otRet;
		}

		// Decodes to an CSecureString or CSecureWString object.
		/*template <typename _tOutType>
		static _tOutType &					Decode( const char * _pcIn, size_t _sInLen, _tOutType &_otRet ) {
			_otRet.resize( _sInLen );
			Decode( _pcIn, _sInLen, const_cast<_tOutType::value_type *>(_otRet.c_str()) );
			return _otRet;
		}*/

		// Decodes to CSecureString, creating a new object.
		static CSecureString				DecodeToString( const char * _pcIn, size_t _sInLen );

		// Decodes to CSecureWString, creating a new object.
		static CSecureWString				DecodeToWString( const char * _pcIn, size_t _sInLen );
	};

}	// namespace mx
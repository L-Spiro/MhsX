#pragma once
#include <string>

namespace mx {

	class CSecureWString : public std::wstring {
	public :
		CSecureWString() {}
		CSecureWString( const CSecureWString &_sOther ) :
			std::wstring( _sOther ) {
		}
		CSecureWString( const std::wstring &_sOther ) :
			std::wstring( _sOther ) {
		}
		CSecureWString( const std::wstring &_sOther, size_t _sPos, size_t _sLen = std::wstring::npos ) :
			std::wstring( _sOther, _sPos, _sLen ) {
		}
		CSecureWString( const wchar_t * _pcOther ) :
			std::wstring( _pcOther ) {
		}
		CSecureWString( const wchar_t * _pcOther, size_t _sLen ) :
			std::wstring( _pcOther, _sLen ) {
		}
		CSecureWString( size_t _sLen, wchar_t _cChar ) :
			std::wstring( _sLen, _cChar ) {
		}
		CSecureWString( std::wstring && _sOther ) /*noexcept*/ :
			std::wstring( _sOther ) {
		}
		~CSecureWString() {
			for ( std::wstring::size_type I = 0; I < size(); ++I ) {
				(*this)[0] = 0;
			}
		}


		// == Functions.
		// Securely clear the buffer.
		void						SecureClear() {
			for ( std::wstring::size_type I = 0; I < size(); ++I ) {
				(*this)[0] = 0;
			}
			(*this).clear();
		}
	};

}	// namespace mx

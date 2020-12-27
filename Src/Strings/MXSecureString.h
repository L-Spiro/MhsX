#pragma once
#include <string>

namespace mx {

	class CSecureString : public std::string {
	public :
		CSecureString() {}
		CSecureString( const CSecureString &_sOther ) :
			std::string( _sOther ) {
		}
		CSecureString( const std::string &_sOther ) :
			std::string( _sOther ) {
		}
		CSecureString( const std::string &_sOther, size_t _sPos, size_t _sLen = std::string::npos ) :
			std::string( _sOther, _sPos, _sLen ) {
		}
		CSecureString( const char * _pcOther ) :
			std::string( _pcOther ) {
		}
		CSecureString( const char * _pcOther, size_t _sLen ) :
			std::string( _pcOther, _sLen ) {
		}
		CSecureString( size_t _sLen, char _cChar ) :
			std::string( _sLen, _cChar ) {
		}
		CSecureString( std::string && _sOther ) /*noexcept*/ :
			std::string( _sOther ) {
		}
		~CSecureString() {
			for ( std::string::size_type I = 0; I < size(); ++I ) {
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

#include "LSWHelpers.h"

namespace lsw {

	// == Members.
	#include "LSWMessages.h"		// m_mMessages[].

	// Converts a message to a string.
	std::string & CHelpers::WindowsMessageToString( WORD _wMessage, std::string &_sRet, bool _bOnlyFirst ) {
#ifdef _DEBUG
		bool bFound = false;
		for ( size_t I = 0; I < sizeof( m_mMessages ) / sizeof( m_mMessages[0] ); ++I ) {
			if ( m_mMessages[I].uiId == _wMessage ) {
				if ( bFound ) {
					_sRet += ", ";
				}
				bFound =  true;
				_sRet += m_mMessages[I].pcName;
				if ( !_bOnlyFirst ) { break; }
			}
		}
		if ( !bFound ) {
			char szBuffer[32];
			std::sprintf( szBuffer, "%.4X", _wMessage );
			_sRet += szBuffer;
		}
#endif	// #ifdef _DEBUG
		return _sRet;
	}

}	// namespace lsw

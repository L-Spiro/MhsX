#include "MXPeObject.h"
#include "../Files/MXFile.h"

namespace mx {

	CPeObject::CPeObject() {
		::ZeroMemory( &m_dhDosHeader, sizeof( m_dhDosHeader ) );
		::ZeroMemory( &m_chHeader, sizeof( m_chHeader ) );
		::ZeroMemory( &m_woWinOpt, sizeof( m_woWinOpt ) );
		::ZeroMemory( &m_cwpWin32, sizeof( m_cwpWin32 ) );
		::ZeroMemory( &m_cwpWin64, sizeof( m_cwpWin64 ) );
	}
	CPeObject::~CPeObject() {
	}

	// == Functions.
	BOOL CPeObject::LoadImageFromFile( const wchar_t * _pwcPath ) {
		CFile fFile;
		if ( !fFile.OpenFile( _pwcPath, TRUE ) ) { return FALSE; }
		if ( fFile.Read( &m_dhDosHeader, sizeof( m_dhDosHeader ) ) != sizeof( m_dhDosHeader ) ) { return FALSE; }
		if ( m_dhDosHeader.Signature != 0x5A4D ) { return FALSE; }

		// Actual DOS block.
		m_vDosStub.resize( m_dhDosHeader.PeOffset - sizeof( m_dhDosHeader ) );
		if ( !m_vDosStub.size() ) { return FALSE; }
		if ( fFile.Read( &m_vDosStub[0], m_vDosStub.size() ) != m_vDosStub.size() ) { return FALSE; }

		// PE header.
		if ( fFile.Read( &m_chHeader, sizeof( m_chHeader ) ) != sizeof( m_chHeader ) ) { return FALSE; }
		if ( (*reinterpret_cast<uint32_t *>(m_chHeader.PeSignature)) != 0x00004550 ) { return FALSE; }

		// Optional header.
		if ( fFile.Read( &m_woWinOpt, sizeof( m_woWinOpt ) ) != sizeof( m_woWinOpt ) ) { return FALSE; }

		if ( m_woWinOpt.Magic == 0x10B ) {
			if ( fFile.Read( &m_cwpWin32, sizeof( m_cwpWin32 ) ) != sizeof( m_cwpWin32 ) ) { return FALSE; }
		}
		else if ( m_woWinOpt.Magic == 0x20B ) {
			if ( fFile.Read( &m_cwpWin64, sizeof( m_cwpWin64 ) ) != sizeof( m_cwpWin64 ) ) { return FALSE; }
		}
		else {
			return FALSE;
		}
		return TRUE;
	}

}	// namespace mx

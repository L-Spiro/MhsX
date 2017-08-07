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

		uint32_t uiDatDirSize = 0;
		if ( m_woWinOpt.Magic == MX_IMAGE_NT_OPTIONAL_HDR32_MAGIC ) {
			if ( fFile.Read( &m_cwpWin32, sizeof( m_cwpWin32 ) ) != sizeof( m_cwpWin32 ) ) { return FALSE; }
			uiDatDirSize = m_cwpWin32.NumberOfRvaAndSizes;
			m_uiImageBase = m_cwpWin32.ImageBase;
		}
		else if ( m_woWinOpt.Magic == MX_IMAGE_NT_OPTIONAL_HDR64_MAGIC ) {
			if ( fFile.Read( &m_cwpWin64, sizeof( m_cwpWin64 ) ) != sizeof( m_cwpWin64 ) ) { return FALSE; }
			uiDatDirSize = m_cwpWin64.NumberOfRvaAndSizes;
			m_uiImageBase = m_cwpWin64.ImageBase;
		}
		else {
			return FALSE;
		}

		if ( uiDatDirSize > MX_IMAGE_NUMBEROF_DIRECTORY_ENTRIES ) {
			return FALSE;
		}

		m_vDataDirectories.resize( uiDatDirSize );
		for ( uint32_t I = 0; I < uiDatDirSize; ++I ) {
			if ( fFile.Read( &m_vDataDirectories[I], sizeof( MX_DATA_DIRECTORY ) ) != sizeof( MX_DATA_DIRECTORY ) ) { return FALSE; }
		}
		
		m_vSections.resize( CoffHeader().NumberOfSections );
		for ( size_t I = 0; I < m_vSections.size(); ++I ) {
			if ( fFile.Read( &m_vSections[I], sizeof( MX_IMAGE_SECTION_HEADER ) ) != sizeof( MX_IMAGE_SECTION_HEADER ) ) { return FALSE; }
		}

		//for ( uint32_t I = 0; I < m_cwpWin32.NumberOfRvaAndSizes
		return TRUE;
	}

}	// namespace mx

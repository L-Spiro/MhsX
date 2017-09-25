#include "MXPeObject.h"
#include "../Utilities/MXUtilities.h"

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
		m_wsPath = _pwcPath;
		CFile fFile;
		if ( !fFile.OpenFile( _pwcPath, TRUE ) ) { return FALSE; }
		return LoadImageFromMemory( fFile );
	}

	BOOL CPeObject::LoadImageFromMemory( CFile &_fFile ) {
		m_uiDosHeaderOffset = static_cast<uint32_t>(_fFile.MovePointerBy( 0 ));
		if ( _fFile.Read( &m_dhDosHeader, sizeof( m_dhDosHeader ) ) != sizeof( m_dhDosHeader ) ) { return FALSE; }
		if ( m_dhDosHeader.Signature != 0x5A4D ) { return FALSE; }
		

		// Actual DOS block.
		m_vDosStub.resize( m_dhDosHeader.PeOffset - sizeof( m_dhDosHeader ) );
		if ( !m_vDosStub.size() ) { return FALSE; }
		m_uiDosStubOffset = static_cast<uint32_t>(_fFile.MovePointerBy( 0 ));
		if ( _fFile.Read( &m_vDosStub[0], m_vDosStub.size() ) != m_vDosStub.size() ) { return FALSE; }

		// PE header.
		m_uiCoffHeaderOffset = static_cast<uint32_t>(_fFile.MovePointerBy( 0 ));
		if ( _fFile.Read( &m_chHeader, sizeof( m_chHeader ) ) != sizeof( m_chHeader ) ) { return FALSE; }
		if ( (*reinterpret_cast<uint32_t *>(m_chHeader.PeSignature)) != 0x00004550 ) { return FALSE; }

		// Optional header.
		m_uiWinOptHeaderOffset = static_cast<uint32_t>(_fFile.MovePointerBy( 0 ));
		if ( _fFile.Read( &m_woWinOpt, sizeof( m_woWinOpt ) ) != sizeof( m_woWinOpt ) ) { return FALSE; }

		uint32_t uiDatDirSize = 0;
		if ( m_woWinOpt.Magic == MX_IMAGE_NT_OPTIONAL_HDR32_MAGIC ) {
			m_uiWin6432HeaderOffset = static_cast<uint32_t>(_fFile.MovePointerBy( 0 ));
			if ( _fFile.Read( &m_cwpWin32, sizeof( m_cwpWin32 ) ) != sizeof( m_cwpWin32 ) ) { return FALSE; }
			uiDatDirSize = m_cwpWin32.NumberOfRvaAndSizes;
			m_uiImageBase = m_cwpWin32.ImageBase;
		}
		else if ( m_woWinOpt.Magic == MX_IMAGE_NT_OPTIONAL_HDR64_MAGIC ) {
			m_uiWin6432HeaderOffset = static_cast<uint32_t>(_fFile.MovePointerBy( 0 ));
			if ( _fFile.Read( &m_cwpWin64, sizeof( m_cwpWin64 ) ) != sizeof( m_cwpWin64 ) ) { return FALSE; }
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
		m_uiDataDirsOffset = static_cast<uint32_t>(_fFile.MovePointerBy( 0 ));
		for ( uint32_t I = 0; I < uiDatDirSize; ++I ) {
			if ( _fFile.Read( &m_vDataDirectories[I], sizeof( MX_DATA_DIRECTORY ) ) != sizeof( MX_DATA_DIRECTORY ) ) { return FALSE; }
		}
		
		m_vSections.resize( CoffHeader().NumberOfSections );
		m_uiSectionsOffset = static_cast<uint32_t>(_fFile.MovePointerBy( 0 ));
		for ( size_t I = 0; I < m_vSections.size(); ++I ) {
			if ( _fFile.Read( &m_vSections[I], sizeof( MX_IMAGE_SECTION_HEADER ) ) != sizeof( MX_IMAGE_SECTION_HEADER ) ) { return FALSE; }
			if ( !m_vSections[I].SizeOfRawData ) { return FALSE; }
		}
		uint64_t ui64Pointer = _fFile.MovePointerBy( 0 );

		m_vSectionData.resize( CoffHeader().NumberOfSections );
		for ( size_t I = 0; I < m_vSectionData.size(); ++I ) {
			if ( !_fFile.MovePointerTo( m_vSections[I].PointerToRawData ) ) { return FALSE; }
			m_vSectionData[I].ui64FileOffset = m_vSections[I].PointerToRawData;
			m_vSectionData[I].ui64RelocAddress = m_vSections[I].VirtualAddress;
			m_vSectionData[I].vData.resize( m_vSections[I].SizeOfRawData );
			if ( _fFile.Read( &m_vSectionData[I].vData[0], m_vSections[I].SizeOfRawData ) != m_vSections[I].SizeOfRawData ) { return FALSE; }
		}

		m_piedExportDesc = nullptr;
		m_uiExportOffset = 0;
		if ( HasExportDesc() ) {
			uint32_t uiOffset;
			uint32_t uiIndex = RelocAddrToRelocIndexAndOffset( m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress, uiOffset );
			if ( uiIndex == MX_ERROR ) { return FALSE; }
			// Add until we get a fully 0'ed MX_IMAGE_EXPORT_DIRECTORY.
			// Can't read past the chunk.
			if ( uiOffset + sizeof( MX_IMAGE_EXPORT_DIRECTORY ) > m_vSectionData[uiIndex].vData.size() ) { return FALSE; }
			// Just point into the chunk.
			m_piedExportDesc = reinterpret_cast<MX_IMAGE_EXPORT_DIRECTORY *>(&m_vSectionData[uiIndex].vData[uiOffset]);
			m_uiExportOffset = static_cast<uint32_t>(m_vSectionData[uiIndex].ui64FileOffset + uiOffset);
		}

		// Import table.
		m_uiImportOffset = 0;
		if ( HasImportDesc() ) {
			if ( m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_IMPORT].Size % sizeof( MX_IMAGE_IMPORT_DESCRIPTOR ) != 0 ) {
				return FALSE;
			}
			uint32_t uiMax = m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_IMPORT].Size / sizeof( MX_IMAGE_IMPORT_DESCRIPTOR );
			uint32_t uiOffset;
			uint32_t uiIndex = RelocAddrToRelocIndexAndOffset( m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress, uiOffset );
			if ( uiIndex == MX_ERROR ) { return FALSE; }
			// Add until we get a fully 0'ed MX_IMAGE_IMPORT_DESCRIPTOR.
			do {
				// Can't read past the chunk.
				if ( uiOffset + sizeof( MX_IMAGE_IMPORT_DESCRIPTOR ) > m_vSectionData[uiIndex].vData.size() ) { return FALSE; }
				// Just point into the chunk.
				MX_IMAGE_IMPORT_DESCRIPTOR * piidTemp = reinterpret_cast<MX_IMAGE_IMPORT_DESCRIPTOR *>(&m_vSectionData[uiIndex].vData[uiOffset]);
				if ( !m_uiImportOffset ) {
					m_uiImportOffset = static_cast<uint32_t>(m_vSectionData[uiIndex].ui64FileOffset + uiOffset);
				}
				m_vImportDesc.push_back( piidTemp );
				// Check for all being NULL.
				if ( piidTemp->Characteristics == 0 &&
					piidTemp->FirstThunk == 0 &&
					piidTemp->ForwarderChain == 0 &&
					piidTemp->Name == 0 &&
					piidTemp->TimeDateStamp == 0 ) { break; }
				uiOffset += sizeof( MX_IMAGE_IMPORT_DESCRIPTOR );
			} while ( TRUE );
			if ( m_vImportDesc.size() > uiMax ) { return FALSE; }
		}
		
		// Resources.
		if ( HasResourceDesc() ) {
			uint32_t uiOffset;
			uint32_t uiIndex = RelocAddrToRelocIndexAndOffset( m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress, uiOffset );
			if ( uiIndex == MX_ERROR ) { return FALSE; }
			// Add until we get a fully 0'ed MX_IMAGE_DIRECTORY_ENTRY_RESOURCE.
			// Can't read past the chunk.
			if ( uiOffset + sizeof( MX_IMAGE_RESOURCE_DIRECTORY ) > m_vSectionData[uiIndex].vData.size() ) { return FALSE; }
			// Just point into the chunk.
			m_pirdResourceDir = reinterpret_cast<MX_IMAGE_RESOURCE_DIRECTORY *>(&m_vSectionData[uiIndex].vData[uiOffset]);
/*			std::vector<MX_IMAGE_RESOURCE_DIRECTORY_ENTRY> vTemp;
			MX_IMAGE_RESOURCE_DIRECTORY_ENTRY * pirdeEntry = reinterpret_cast<MX_IMAGE_RESOURCE_DIRECTORY_ENTRY *>(&m_vSectionData[uiIndex].vData[uiOffset+sizeof( MX_IMAGE_RESOURCE_DIRECTORY )]);
			for ( uint32_t I = 0; I < m_pirdResourceDir->NumberOfIdEntries + m_pirdResourceDir->NumberOfNamedEntries; ++I ) {
				vTemp.push_back( pirdeEntry[I] );
			}
			*/
			m_uiResourceOffset = static_cast<uint32_t>(m_vSectionData[uiIndex].ui64FileOffset + uiOffset);
		}
		
		// Relocations.
		// m_uiRelocOffset
		if ( HasRelocDesc() ) {
			uint32_t uiOffset;
			uint32_t uiIndex = RelocAddrToRelocIndexAndOffset( m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress, uiOffset );
			if ( uiIndex == MX_ERROR ) { return FALSE; }
			
			m_uiRelocOffset = static_cast<uint32_t>(m_vSectionData[uiIndex].ui64FileOffset + uiOffset);
			while ( uiOffset < m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_BASERELOC].Size ) {
				MX_BASE_RELOC brReloc;
				brReloc.ui64FileOffset = m_vSectionData[uiIndex].ui64FileOffset + uiOffset;
				
				brReloc.pibrReloc = reinterpret_cast<MX_IMAGE_BASE_RELOCATION *>(&m_vSectionData[uiIndex].vData[uiOffset]);
				brReloc.uiTotal = (brReloc.pibrReloc->SizeOfBlock - sizeof( MX_IMAGE_BASE_RELOCATION )) / sizeof( uint16_t );
				brReloc.puiOffsets = reinterpret_cast<uint16_t *>(reinterpret_cast<uint8_t *>(brReloc.pibrReloc) + sizeof( MX_IMAGE_BASE_RELOCATION ));
	
				m_vBaseRelocations.push_back( brReloc );

				uiOffset += sizeof( MX_IMAGE_BASE_RELOCATION ) + (sizeof( uint16_t ) * brReloc.uiTotal);
			}
		}

		//for ( uint32_t I = 0; I < m_cwpWin32.NumberOfRvaAndSizes
		return TRUE;
	}

	uint64_t CPeObject::FileOffsetToRelocAddress( uint64_t _uiFileAddr ) const {
		for ( size_t I = 0; I < m_vSectionData.size(); ++I ) {
			uint64_t uiEnd = m_vSectionData[I].ui64FileOffset + m_vSectionData[I].vData.size();
			if ( _uiFileAddr >= m_vSectionData[I].ui64FileOffset && _uiFileAddr < uiEnd ) {
				return _uiFileAddr - m_vSectionData[I].ui64FileOffset + m_vSectionData[I].ui64RelocAddress;
			}
		}
		// Probably below the relocation sections.
		return _uiFileAddr;
	}

	uint64_t CPeObject::RelocAddressToFileOffset( uint64_t _uiRelocAddr ) const {
		for ( size_t I = 0; I < m_vSectionData.size(); ++I ) {
			uint64_t uiEnd = m_vSectionData[I].ui64RelocAddress + m_vSectionData[I].vData.size();
			if ( _uiRelocAddr >= m_vSectionData[I].ui64RelocAddress && _uiRelocAddr < uiEnd ) {
				return _uiRelocAddr - m_vSectionData[I].ui64RelocAddress + m_vSectionData[I].ui64FileOffset;
			}
		}
		return _uiRelocAddr;
	}

	uint64_t CPeObject::NameOfSectionByRelocAddress( uint64_t _uiRelocAddr, std::string &_sReturn ) const {
		for ( size_t I = 0; I < m_vSectionData.size(); ++I ) {
			uint64_t uiEnd = m_vSectionData[I].ui64RelocAddress + m_vSectionData[I].vData.size();
			if ( _uiRelocAddr >= m_vSectionData[I].ui64RelocAddress && _uiRelocAddr < uiEnd ) {
				for ( size_t J = 0; J < sizeof( Sections()[I].Name ); ++J ) {
					if ( !Sections()[I].Name[J] ) { return _uiRelocAddr - m_vSectionData[I].ui64RelocAddress; }
					_sReturn.push_back( Sections()[I].Name[J] );
				}
				return _uiRelocAddr - m_vSectionData[I].ui64RelocAddress;
			}
		}
		return 0;
	}

	VOID CPeObject::ImportDllNameByIndex( uint32_t _ui32Index, std::string &_sReturn ) const {
		if ( _ui32Index >= m_vImportDesc.size() ) { return; }
		uint32_t ui32Name = m_vImportDesc[_ui32Index]->Name;
		uint32_t uiOffset;
		uint32_t uiIndex = RelocAddrToRelocIndexAndOffset( ui32Name, uiOffset );
		if ( uiIndex != MX_ERROR ) {
			while ( uiOffset < SectionData()[uiIndex].vData.size() ) {
				CHAR cTemp = SectionData()[uiIndex].vData[uiOffset++];
				if ( !cTemp ) { break; }
				_sReturn.push_back( cTemp );
			}
		}
	}

	VOID CPeObject::GetExportDllName( std::string &_sReturn ) const {
		if ( HasExportDesc() && ExportDescriptor() ) {
			GetString( ExportDescriptor()->Name, _sReturn );
		}
	}

	VOID CPeObject::GetString( uint64_t _uiRelocAddr, std::string &_sReturn ) const {
		CHAR cChar;
		do {
			if ( ReadRelocBytes( _uiRelocAddr++, &cChar, sizeof( cChar ) ) != sizeof( cChar ) ) { return; }
			if ( !cChar ) { return; }
			_sReturn.push_back( cChar );
		} while ( cChar );
	}

	VOID CPeObject::GetSizedWideString( uint64_t _uiRelocAddr, std::string &_sReturn ) const {
		WCHAR wChar;
		uint16_t uiLen;
		std::wstring wTemp;
		if ( ReadRelocBytes( _uiRelocAddr, &uiLen, sizeof( uiLen ) ) != sizeof( uiLen ) ) { return; }
		_uiRelocAddr += sizeof( uiLen );

		for ( uint16_t I = 0; I < uiLen; ++I ) {
			if ( ReadRelocBytes( _uiRelocAddr, &wChar, sizeof( wChar ) ) != sizeof( wChar ) ) { return; }
			//if ( !wChar ) { return; }
			wTemp.push_back( wChar );
			_uiRelocAddr += sizeof( wChar );
		}
		_sReturn += CUtilities::WStringToString( wTemp );
	}

	uint64_t CPeObject::ReadRelocBytes( uint64_t _uiAddr, VOID * _pvDst, uint64_t _uiTotal ) const {
		uint32_t uiOffset;
		uint32_t uiIndex = RelocAddrToRelocIndexAndOffset( _uiAddr, uiOffset );
		if ( uiIndex == MX_ERROR ) { return 0; }
		uint64_t uiEnd = m_vSectionData[uiIndex].ui64RelocAddress + m_vSectionData[uiIndex].vData.size();
		uint64_t uiReadEnd = _uiAddr + _uiTotal;
		uiReadEnd = min( uiReadEnd, uiEnd );
		_uiTotal = uiReadEnd - _uiAddr;
		std::memcpy( _pvDst, &m_vSectionData[uiIndex].vData[uiOffset], _uiTotal );
		return _uiTotal;
	}

	uint32_t CPeObject::RelocAddrToRelocIndexAndOffset( uint64_t _uiAddr, uint32_t &_uiOffset ) const {
		for ( size_t I = 0; I < m_vSectionData.size(); ++I ) {
			uint64_t uiEnd = m_vSectionData[I].ui64RelocAddress + m_vSectionData[I].vData.size();
			if ( _uiAddr >= m_vSectionData[I].ui64RelocAddress && _uiAddr < uiEnd ) {
				_uiOffset = _uiAddr - m_vSectionData[I].ui64RelocAddress;
				return I;
			}
		}
		return MX_ERROR;
	}

	VOID CPeObject::GetExports( std::vector<CPeObject::MX_EXPORT> &_vReturn ) const {
		if ( HasExportDesc() && ExportDescriptor() ) {
			uint32_t uiTotal = max( ExportDescriptor()->NumberOfFunctions, ExportDescriptor()->NumberOfNames );
			_vReturn.reserve( uiTotal );
			uint32_t uiFuncArray, uiNameArray, uiOrdinArray;
			for ( uint32_t I = 0; I < uiTotal; ++I ) {
				MX_EXPORT eExport;
				eExport.uiAddress = 0;
				eExport.uiOrdinal = 0;
				eExport.ui64FileOffsetAddr = RelocAddressToFileOffset( ExportDescriptor()->AddressOfFunctions + I * 4 );
				eExport.ui64FileOffsetName = 0;
				eExport.ui64FileOffsetOrd = RelocAddressToFileOffset( ExportDescriptor()->AddressOfNameOrdinals + I * 2 );

				uint16_t ui16AddressIndex = 0;
				if ( I < ExportDescriptor()->NumberOfNames ) {
					if ( ReadRelocBytes( ExportDescriptor()->AddressOfNames + I * 4, &uiNameArray, sizeof( uiNameArray ) ) != sizeof( uiNameArray ) ) { return; }
					GetString( uiNameArray, eExport.sName );
					eExport.ui64FileOffsetName = RelocAddressToFileOffset( uiNameArray );

					if ( ReadRelocBytes( ExportDescriptor()->AddressOfNameOrdinals + I * 2, &eExport.uiOrdinal, sizeof( eExport.uiOrdinal ) ) != sizeof( eExport.uiOrdinal ) ) { return; }
					ui16AddressIndex = eExport.uiOrdinal;
					eExport.uiOrdinal += ExportDescriptor()->Base;
				}
				else {
					GetExportDllName( eExport.sName );
					eExport.sName.push_back( '.' );
					ui16AddressIndex = I;
					eExport.uiOrdinal = I + ExportDescriptor()->Base;
					CUtilities::ToUnsigned( eExport.uiOrdinal, eExport.sName );
				}

				if ( ui16AddressIndex < ExportDescriptor()->NumberOfFunctions ) {
					if ( ReadRelocBytes( ExportDescriptor()->AddressOfFunctions + ui16AddressIndex * 4, &eExport.uiAddress, sizeof( uint32_t ) ) != sizeof( uint32_t ) ) { return; }
					eExport.uiAddress += ImageBase();
				}
				_vReturn.push_back( eExport );
			}
		}
	}

	VOID CPeObject::GetImportsFromImportDescByIndex( uint32_t _ui32Index, std::vector<CPeObject::MX_IMPORT> &_vReturn ) const {
#define MX_READ( ADDR, RET )	if ( ReadRelocBytes( ADDR, &RET, sizeof( RET ) ) != sizeof( RET ) ) { return; }
		const uint32_t ui32ReadSize = Is64() ? sizeof( uint64_t ) : sizeof( uint32_t );
		if ( _ui32Index < m_vImportDesc.size() ) {
			uint32_t uiTable = m_vImportDesc[_ui32Index]->FirstThunk;
			uint64_t uiPointer = 1;
			while ( uiPointer ) {
				uiPointer = 0;
				if ( ReadRelocBytes( uiTable, &uiPointer, ui32ReadSize ) == ui32ReadSize ) {
					CPeObject::MX_IMPORT iImport;
					BOOL bInFile = RelocAddressIsInFile( uiPointer );
					if ( bInFile ) {
						iImport.ui64FileOffset = RelocAddressToFileOffset( uiPointer );
						// Read the oridinal.
						MX_READ( uiPointer, iImport.uiOrdinal );
						uiPointer += sizeof( iImport.uiOrdinal );
						CHAR cTemp;
						MX_READ( uiPointer++, cTemp );
						while ( cTemp ) {
							iImport.sName.push_back( cTemp );
							MX_READ( uiPointer++, cTemp );
						}
						iImport.bBound = FALSE;
						if ( iImport.uiOrdinal == 0 && iImport.sName.size() == 0 ) { return; }
						_vReturn.push_back( iImport );
					}
					else {
						if ( !uiPointer ) { return; }
						// A bound address (address is hardcoded to be the address where a DLL will be loaded and have the desired function).
						iImport.ui64FileOffset = RelocAddressToFileOffset( uiTable );

						if ( m_vImportDesc[_ui32Index]->TimeDateStamp == 0 ) {
							iImport.uiOrdinal = uiPointer & 0xFFFF;
							ImportDllNameByIndex( _ui32Index, iImport.sName );
							iImport.sName.push_back( '.' );
							CUtilities::ToUnsigned( iImport.uiOrdinal, iImport.sName );
							iImport.bBound = TRUE;	// Tells it how to print the raw bytes.
						}
						else {
							CHAR szBuffer[32];
							iImport.uiOrdinal = 0;
							std::sprintf( szBuffer, "<%s>", CUtilities::ToHex( uiPointer, Is64() ? 16 : 8 ) );
							iImport.sName = szBuffer;
							iImport.bBound = TRUE;
						}
						_vReturn.push_back( iImport );
					}
				}
				else { return; }
				uiTable += ui32ReadSize;
			}
		}
#undef MX_READ
	}

	// Returns the address without modification or 0.
	uint64_t CPeObject::GetExportAddress( const char * _pcExport ) const {
		if ( HasExportDesc() && ExportDescriptor() ) {
			std::string sExport = _pcExport;
			uint32_t uiTotal = ExportDescriptor()->NumberOfNames;
			uint32_t uiNameArray;
			for ( uint32_t I = 0; I < uiTotal; ++I ) {
				if ( ReadRelocBytes( ExportDescriptor()->AddressOfNames + I * 4, &uiNameArray, sizeof( uiNameArray ) ) != sizeof( uiNameArray ) ) { return 0; }
				std::string sName;
				GetString( uiNameArray, sName );
				if ( sName == sExport ) {
					uint16_t ui16AddressIndex = 0;
					if ( ReadRelocBytes( ExportDescriptor()->AddressOfNameOrdinals + I * 2, &ui16AddressIndex, sizeof( ui16AddressIndex ) ) != sizeof( ui16AddressIndex ) ) { return 0; }
					if ( ui16AddressIndex < ExportDescriptor()->NumberOfFunctions ) {
						uint32_t uiAddress;
						if ( ReadRelocBytes( ExportDescriptor()->AddressOfFunctions + ui16AddressIndex * 4, &uiAddress, sizeof( uiAddress ) ) != sizeof( uiAddress ) ) { return 0; }
						return uiAddress;
					}
					// Address not found associated with it.
					return 0;
				}
			}
		}
		return 0;
	}

	VOID CPeObject::RelocAddressToStringwithSection( uint64_t _uiRelocAddr, std::string &_sReturn ) const {
		CHAR szBuffer[128];
		if ( !_uiRelocAddr ) {
			CUtilities::ToHex( _uiRelocAddr, szBuffer, MX_ELEMENTS( szBuffer ), 8 );
		}
		else {
			CHAR szTemp[32];
			std::sprintf( szBuffer, "%s (%s after loading)",
				CUtilities::ToHex( _uiRelocAddr, 8 ),
				CUtilities::ToHex( _uiRelocAddr + ImageBase(), szTemp, MX_ELEMENTS( szTemp ), 0 ) );
		}
		_sReturn += szBuffer;
		if ( _uiRelocAddr ) {
			std::string sTemp;
			uint64_t uiOffset = NameOfSectionByRelocAddress( _uiRelocAddr, sTemp );
			if ( sTemp.size() ) {
				CHAR szTemp[32];
				std::sprintf( szTemp, " ([\"%s\"+%s])", sTemp.c_str(), CUtilities::ToHex( uiOffset, 0 ) );

				_sReturn += szTemp;
			}
		}
	}

	// _uiAddr is given as a RAM address, and this determines if the given address falls within the memory this file would use if properly loaded to memory and expanded starting from its image base.
	BOOL CPeObject::AddressIsInLoadedFile( uint64_t _uiAddr ) const {
		if ( _uiAddr < ImageBase() ) { return FALSE; }
		uint64_t uiEndAddress = ImageBase();
		for ( size_t I = 0; I < SectionData().size(); ++I ) {
			uiEndAddress = max( uiEndAddress, SectionData()[I].ui64RelocAddress + SectionData()[I].vData.size() + ImageBase() );
		}
		return _uiAddr < uiEndAddress;
	}

	// _uiAddr is a file offset.
	BOOL CPeObject::RelocAddressIsInFile( uint64_t _uiAddr ) const {
		uint64_t uiEndAddress = 0;
		for ( size_t I = 0; I < SectionData().size(); ++I ) {
			uiEndAddress = max( uiEndAddress, SectionData()[I].ui64RelocAddress + SectionData()[I].vData.size() + ImageBase() );
		}
		return _uiAddr < uiEndAddress;
	}

}	// namespace mx

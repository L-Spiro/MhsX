#include "MXPeTreeModel.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <stdlib.h>


#define MX_PRINT_FILE_OFFSET( OFF, TYPE )												\
	CHAR szOffset[32];																	\
	std::sprintf( szOffset, "%s (%I64u)", CUtilities::ToHex( OFF, 8 ), OFF );			\
	OFF += sizeof( TYPE )

namespace mx {

	CPeTreeModel::CPeTreeModel( const void * _pvData, QObject * _poParent )
		: CTreeModel( _pvData, _poParent ) {
	}
	CPeTreeModel::CPeTreeModel( const QList<QVariant> &_lRootData, const void * _pvData, QObject * _poParent )
		: CTreeModel( _lRootData, _pvData, _poParent ) {
	}
	CPeTreeModel::~CPeTreeModel() {
	}

	// == Functions.
	// Create the tree given a mx::CPeObject object.
	BOOL CPeTreeModel::CreateTree( const mx::CPeObject &_poPeObject ) {
#define MX_PART_TO_STRING( PART )					\
	CHAR szBuffer[sizeof( PART )*2+1];				\
	CUtilities::BytesToHex( &PART, sizeof( PART ), szBuffer )

		{
			QList<QVariant> lValues;
			//MX_PART_TO_STRING( _poPeObject.DosHeader() );
			lValues << "MS-DOS Header" << "" << "" << "" << "The MS-DOS header.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			AddDosHeader( ptviTemp, _poPeObject.DosHeader(), _poPeObject.DosHeaderOffset() );
		}
		{
			QList<QVariant> lValues;
			//std::string sTemp;
			//CUtilities::BytesToHex( &_poPeObject.DosStub()[0], sizeof( _poPeObject.WinHeader() ), sTemp );
			lValues << "DOS Stub" << "" << "" << "" << "The DOS stub.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			AddDosStub( ptviTemp, _poPeObject.DosStub(), _poPeObject.DosStubOffset() );
		}
		{
			QList<QVariant> lValues;
			//MX_PART_TO_STRING( _poPeObject.CoffHeader() );
			lValues << "COFF Header" << "" << "" << "" << "The Portable Executable signature followed by the COFF header.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			AddCoffHeader( ptviTemp, _poPeObject.CoffHeader(), _poPeObject.CoffHeaderOffset() );
		}
		{
			CTreeViewItem * ptviTemp = nullptr;
			if ( _poPeObject.Is64() ) {
				
				{
					QList<QVariant> lValues;
					//CHAR szBuffer[(sizeof( _poPeObject.WinHeader() )+sizeof( _poPeObject.Win64Header() ))*2+1];
					//CUtilities::BytesToHex( &_poPeObject.WinHeader(), sizeof( _poPeObject.WinHeader() ), szBuffer );
					//CUtilities::BytesToHex( &_poPeObject.Win64Header(), sizeof( _poPeObject.Win64Header() ), &szBuffer[sizeof( _poPeObject.WinHeader() )] );
					lValues << "PE 64 Header" << "" << "" << "" << "The 64-bit PE header.";
					ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
					m_ptviRootItem->AppendChild( ptviTemp );
					AddOptHeader( ptviTemp, _poPeObject.WinHeader(), _poPeObject, _poPeObject.ImageBase(), _poPeObject.WinHeaderOffset() );
					AddPe64Header( ptviTemp, _poPeObject.Win64Header(), _poPeObject.WinXHeaderOffset() );
				}
			}
			else {
				{
					QList<QVariant> lValues;
					//CHAR szBuffer[(sizeof( _poPeObject.WinHeader() )+sizeof( _poPeObject.Win32Header() ))*2+1];
					//CUtilities::BytesToHex( &_poPeObject.WinHeader(), sizeof( _poPeObject.WinHeader() ), szBuffer );
					//CUtilities::BytesToHex( &_poPeObject.Win32Header(), sizeof( _poPeObject.Win32Header() ), &szBuffer[sizeof( _poPeObject.WinHeader() )] );
					lValues << "PE 32 Header" << "" << "" << "" << "The 32-bit PE header.";
					ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
					m_ptviRootItem->AppendChild( ptviTemp );
					AddOptHeader( ptviTemp, _poPeObject.WinHeader(), _poPeObject, _poPeObject.ImageBase(), _poPeObject.WinHeaderOffset() );
					AddPe32Header( ptviTemp, _poPeObject.Win32Header(), _poPeObject.WinXHeaderOffset() );
				}
			}
			AddDataDirectories( ptviTemp, _poPeObject.DataDirectory(), _poPeObject, _poPeObject.ImageBase(), _poPeObject.DataDirsOffset() );
		}
		{
			QList<QVariant> lValues;
			lValues << "Section Headers" << "" << "" << "" << "Section headers.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			AddSectionHeaders( ptviTemp, _poPeObject.Sections(), _poPeObject.ImageBase(), _poPeObject.SectionsOffset() );
		}
		
		AddExports( m_ptviRootItem, _poPeObject );

		if ( _poPeObject.HasImportDesc() ) {
			QList<QVariant> lValues;
			lValues << "Import Descriptions" << "" << "" << "" << "Import descriptors.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			uint32_t ui32Offset = _poPeObject.ImportDescriptorOffset();
			for ( size_t I = 0; I < _poPeObject.ImportDescriptor().size(); ++I ) {
				QList<QVariant> lTemp;
				CHAR szTemp[48];
				std::sprintf( szTemp, "IMAGE_IMPORT_DESCRIPTOR[%u]", static_cast<uint32_t>(I) );
				std::string sDllName;
				_poPeObject.ImportDllNameByIndex( I, sDllName );
				lTemp << szTemp << sDllName.c_str() << "" << "" << "";
				CTreeViewItem * ptviTemp2 = new CTreeViewItem( lTemp, ptviTemp );
				ptviTemp->AppendChild( ptviTemp2 );
				AddImportDesc( ptviTemp2, (*_poPeObject.ImportDescriptor()[I]), _poPeObject, ui32Offset );
				ui32Offset += sizeof( MX_IMAGE_IMPORT_DESCRIPTOR );
			}
		}

		if ( _poPeObject.HasResourceDesc() && _poPeObject.ResourceDescriptor() ) {
			QList<QVariant> lValues;
			//MX_PART_TO_STRING( _poPeObject.CoffHeader() );
			lValues << "Resource Description" << "" << "" << "" << "Resource descriptor.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			AddResourceDesc( ptviTemp, (*_poPeObject.ResourceDescriptor()), _poPeObject, _poPeObject.ResourceDescriptorOffset() );
		}

		AddExportedFunctions( m_ptviRootItem, _poPeObject );
		AddImportedFuncs( m_ptviRootItem, _poPeObject );

		if ( _poPeObject.HasResourceDesc() && _poPeObject.ResourceDescriptor() ) {
			QList<QVariant> lValues;
			//MX_PART_TO_STRING( _poPeObject.CoffHeader() );
			lValues << "Resources" << "" << "" << "" << "Resources.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			AddResourceTree( ptviTemp, _poPeObject );
		} 
		

		return TRUE;
	}

	// Decodes a DOS header to a given parent.
	VOID CPeTreeModel::AddDosHeader( CTreeViewItem * _ptviParent, const MX_DOS_HEADER &_dhHeader, uint64_t _uiStructOffset ) {
		CHAR szBuffer[MX_PE_DOS_PAD*12];
		CHAR szBufferRaw[MX_PE_DOS_PAD*12];
		
		{
			CUtilities::BytesToCString( &_dhHeader.Signature, sizeof( _dhHeader.Signature ), szBuffer );
			CUtilities::BytesToHexWithSpaces( &_dhHeader.Signature, sizeof( _dhHeader.Signature ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.Signature );
			QList<QVariant> lValues;
			lValues << "Signature" << szBuffer << szOffset << szBufferRaw << "Signature MZ (Mark Zbikowski).";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.BytesInLastBlock );
			CUtilities::BytesToHex( &_dhHeader.BytesInLastBlock, sizeof( _dhHeader.BytesInLastBlock ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.BytesInLastBlock );
			QList<QVariant> lValues;
			lValues << "BytesInLastBlock" << szBuffer << szOffset << szBufferRaw << "The number of bytes in the last block of the program that are actually used. If this value is zero, then the entire last block is used (effectively 512).";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.BlocksInFile );
			CUtilities::BytesToHex( &_dhHeader.BlocksInFile, sizeof( _dhHeader.BlocksInFile ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.BlocksInFile );
			QList<QVariant> lValues;
			lValues << "BlocksInFile" << szBuffer << szOffset << szBufferRaw << "Number of blocks in the file that are part of the EXE file. If BytesInLastBlock is non-zero, only that much of the last block is used.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.NumRelocs );
			CUtilities::BytesToHex( &_dhHeader.NumRelocs, sizeof( _dhHeader.NumRelocs ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.NumRelocs );
			QList<QVariant> lValues;
			lValues << "NumRelocs" << szBuffer << szOffset << szBufferRaw << "Number of relocation entries stored after the header. May be zero.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.HeaderParagraphs );
			CUtilities::BytesToHex( &_dhHeader.HeaderParagraphs, sizeof( _dhHeader.HeaderParagraphs ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.HeaderParagraphs );
			QList<QVariant> lValues;
			lValues << "HeaderParagraphs" << szBuffer << szOffset << szBufferRaw << "Number of paragraphs in the header.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.MinExtraParagraphs );
			CUtilities::BytesToHex( &_dhHeader.MinExtraParagraphs, sizeof( _dhHeader.MinExtraParagraphs ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.MinExtraParagraphs );
			QList<QVariant> lValues;
			lValues << "MinExtraParagraphs" << szBuffer << szOffset << szBufferRaw << "Number of paragraphs of additional memory that the program will need. This is the equivalent of the BSS size in a Unix program.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.MaxExtraParagraphs );
			CUtilities::BytesToHex( &_dhHeader.MaxExtraParagraphs, sizeof( _dhHeader.MaxExtraParagraphs ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.MaxExtraParagraphs );
			QList<QVariant> lValues;
			lValues << "MaxExtraParagraphs" << szBuffer << szOffset << szBufferRaw << "Maximum number of paragraphs of additional memory.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.Ss );
			CUtilities::BytesToHex( &_dhHeader.Ss, sizeof( _dhHeader.Ss ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.Ss );
			QList<QVariant> lValues;
			lValues << "StackSegment" << szBuffer << szOffset << szBufferRaw << "Relative value of the stack segment. This value is added to the segment at which the program was loaded, and the result is used to initialize the SS register.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.Sp );
			CUtilities::BytesToHex( &_dhHeader.Sp, sizeof( _dhHeader.Sp ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.Sp );
			QList<QVariant> lValues;
			lValues << "SP" << szBuffer << szOffset << szBufferRaw << "Initial value of the SP register.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.Checksum );
			CUtilities::BytesToHex( &_dhHeader.Checksum, sizeof( _dhHeader.Checksum ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.Checksum );
			QList<QVariant> lValues;
			lValues << "Checksum" << szBuffer << szOffset << szBufferRaw << "Word checksum. If set properly, the 16-bit sum of all words in the file should be zero. Usually this is not set.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.Ip );
			CUtilities::BytesToHex( &_dhHeader.Ip, sizeof( _dhHeader.Ip ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.Ip );
			QList<QVariant> lValues;
			lValues << "IP" << szBuffer << szOffset << szBufferRaw << "Initial value of the IP register.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.Cs );
			CUtilities::BytesToHex( &_dhHeader.Cs, sizeof( _dhHeader.Cs ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.Cs );
			QList<QVariant> lValues;
			lValues << "CS" << szBuffer << szOffset << szBufferRaw << "Initial value of the CS register, relative to the segment where the program was loaded.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.OverlayNumber );
			CUtilities::BytesToHex( &_dhHeader.OverlayNumber, sizeof( _dhHeader.OverlayNumber ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.OverlayNumber );
			QList<QVariant> lValues;
			lValues << "OverlayNumber" << szBuffer << szOffset << szBufferRaw << "Overlay number. A value of 0 implies this is the main program.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CUtilities::BytesToCString( &_dhHeader.Padding, sizeof( _dhHeader.Padding ), szBuffer );
			CUtilities::BytesToHexWithSpaces( &_dhHeader.Padding, sizeof( _dhHeader.Padding ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.Padding );
			QList<QVariant> lValues;
			lValues << "Padding" << szBuffer << szOffset << szBufferRaw << "Padding.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.PeOffset );
			CUtilities::BytesToHex( &_dhHeader.PeOffset, sizeof( _dhHeader.PeOffset ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.PeOffset );
			QList<QVariant> lValues;
			lValues << "PeOffset" << szBuffer << szOffset << szBufferRaw << "The offset from the start of the file to the relocation pointer table.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.OverlayNum );
			CUtilities::BytesToHex( &_dhHeader.OverlayNum, sizeof( _dhHeader.OverlayNum ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _dhHeader.OverlayNum );
			QList<QVariant> lValues;
			lValues << "OverlayNum" << szBuffer << szOffset << szBufferRaw << "Overlays are sections of a program that remain on disk until the program actually requires them.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes a DOS stub to a given parent.
	VOID CPeTreeModel::AddDosStub( CTreeViewItem * _ptviParent, const std::vector<uint8_t> &_vStub, uint64_t _uiStructOffset ) {
		std::string sVal;
		std::string sRaw;
		{
			CUtilities::BytesToCString( &_vStub[0], _vStub.size(), sVal );
			CUtilities::BytesToHexWithSpaces( &_vStub[0], _vStub.size(), sRaw );
			CHAR szOffset[32];
			std::sprintf( szOffset, "%s (%I64u)", CUtilities::ToHex( _uiStructOffset, 8 ), _uiStructOffset );
			QList<QVariant> lValues;
			lValues << "Stub" << sVal.c_str() << szOffset << sRaw.c_str() << "DOS stub.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes a COFF header to a given parent.
	VOID CPeTreeModel::AddCoffHeader( CTreeViewItem * _ptviParent, const MX_COFF_HEADER &_chHeader, uint64_t _uiStructOffset ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];
		{
			CUtilities::BytesToCString( &_chHeader.PeSignature, sizeof( _chHeader.PeSignature ), szBuffer );
			CUtilities::BytesToHexWithSpaces( &_chHeader.PeSignature, sizeof( _chHeader.PeSignature ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _chHeader.PeSignature );
			QList<QVariant> lValues;
			lValues << "PeSignature" << szBuffer << szOffset << szBufferRaw << "The PE header.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[64];
			CUtilities::MachineTypeToString( _chHeader.Machine, szTemp );
			std::sprintf( szBuffer, "%s (%u)", szTemp, _chHeader.Machine );
			CUtilities::BytesToHex( &_chHeader.Machine, sizeof( _chHeader.Machine ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _chHeader.Machine );
			QList<QVariant> lValues;
			lValues << "Machine" << szBuffer << szOffset << szBufferRaw << "Determines for what machine the file was compiled.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _chHeader.NumberOfSections );
			CUtilities::BytesToHex( &_chHeader.NumberOfSections, sizeof( _chHeader.NumberOfSections ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _chHeader.NumberOfSections );
			QList<QVariant> lValues;
			lValues << "NumberOfSections" << szBuffer << szOffset << szBufferRaw << "Number of sections that are described at the end of the PE headers.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CUtilities::CreateDateString( _chHeader.TimeDateStamp, szBuffer, sizeof( szBuffer ) );
			CUtilities::BytesToHex( &_chHeader.TimeDateStamp, sizeof( _chHeader.TimeDateStamp ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _chHeader.TimeDateStamp );
			QList<QVariant> lValues;
			lValues << "TimeDateStamp" << szBuffer << szOffset << szBufferRaw << "32-bit time at which this header was generated (DD-MM-YYYY HH-MM-SS).";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _chHeader.PointerToSymbolTable );
			CUtilities::BytesToHex( &_chHeader.PointerToSymbolTable, sizeof( _chHeader.PointerToSymbolTable ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _chHeader.PointerToSymbolTable );
			QList<QVariant> lValues;
			lValues << "PointerToSymbolTable" << szBuffer << szOffset << szBufferRaw << "The offset of the symbol table, in bytes, or zero if no COFF symbol table exists.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _chHeader.NumberOfSymbols );
			CUtilities::BytesToHex( &_chHeader.NumberOfSymbols, sizeof( _chHeader.NumberOfSymbols ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _chHeader.NumberOfSymbols );
			QList<QVariant> lValues;
			lValues << "NumberOfSymbols" << szBuffer << szOffset << szBufferRaw << "The number of symbols in the symbol table.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _chHeader.SizeOfOptionalHeader );
			CUtilities::BytesToHex( &_chHeader.SizeOfOptionalHeader, sizeof( _chHeader.SizeOfOptionalHeader ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _chHeader.SizeOfOptionalHeader );
			QList<QVariant> lValues;
			lValues << "SizeOfOptionalHeader" << szBuffer << szOffset << szBufferRaw << "The size of the optional header, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[48];
			std::sprintf( szTemp, " (%u, %s)", _chHeader.Characteristics, CUtilities::ToHex( _chHeader.Characteristics, 4 ) );
			std::string sVal;
			CUtilities::PeCharacteristicsToString( _chHeader.Characteristics, sVal );
			sVal.append( szTemp );
			CUtilities::BytesToHex( &_chHeader.Characteristics, sizeof( _chHeader.Characteristics ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _chHeader.Characteristics );
			QList<QVariant> lValues;
			lValues << "Characteristics" << sVal.c_str() << szOffset << szBufferRaw << "The characteristics of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes a PE header to a given parent.
	VOID CPeTreeModel::AddOptHeader( CTreeViewItem * _ptviParent, const MX_COFF_WINDOW_OPT &_cwoHeader, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];
		{
			CHAR szTemp[64];
			CUtilities::PeMagicTypeToString( _cwoHeader.Magic, szTemp );
			std::sprintf( szBuffer, "%s (%u)", szTemp, _cwoHeader.Magic );
			CUtilities::BytesToHex( &_cwoHeader.Magic, sizeof( _cwoHeader.Magic ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwoHeader.Magic );
			QList<QVariant> lValues;
			lValues << "Magic" << szBuffer << szOffset << szBufferRaw << "The state of the image file.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwoHeader.MajorLinkerVersion );
			CUtilities::BytesToHex( &_cwoHeader.MajorLinkerVersion, sizeof( _cwoHeader.MajorLinkerVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwoHeader.MajorLinkerVersion );
			QList<QVariant> lValues;
			lValues << "MajorLinkerVersion" << szBuffer << szOffset << szBufferRaw << "The major version number of the linker.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwoHeader.MinorLinkerVersion );
			CUtilities::BytesToHex( &_cwoHeader.MinorLinkerVersion, sizeof( _cwoHeader.MinorLinkerVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwoHeader.MinorLinkerVersion );
			QList<QVariant> lValues;
			lValues << "MinorLinkerVersion" << szBuffer << szOffset << szBufferRaw << "The minor version number of the linker.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwoHeader.SizeOfCode );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwoHeader.SizeOfCode, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwoHeader.SizeOfCode, sizeof( _cwoHeader.SizeOfCode ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwoHeader.SizeOfCode );
			QList<QVariant> lValues;
			lValues << "SizeOfCode" << sTemp.c_str() << szOffset << szBufferRaw << "The size of the code section, in bytes, or the sum of all such sections if there are multiple code sections.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwoHeader.SizeOfInitializedData );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwoHeader.SizeOfInitializedData, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwoHeader.SizeOfInitializedData, sizeof( _cwoHeader.SizeOfInitializedData ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwoHeader.SizeOfInitializedData );
			QList<QVariant> lValues;
			lValues << "SizeOfInitializedData" << sTemp.c_str() << szOffset << szBufferRaw << "The size of the initialized data section, in bytes, or the sum of all such sections if there are multiple initialized data sections.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwoHeader.SizeOfUninitializedData );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwoHeader.SizeOfUninitializedData, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwoHeader.SizeOfUninitializedData, sizeof( _cwoHeader.SizeOfUninitializedData ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwoHeader.SizeOfUninitializedData );
			QList<QVariant> lValues;
			lValues << "SizeOfUninitializedData" << sTemp.c_str() << szOffset << szBufferRaw << "The size of the uninitialized data section, in bytes, or the sum of all such sections if there are multiple uninitialized data sections.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sAddr;
			_poPeObject.RelocAddressToStringwithSection( _cwoHeader.AddressOfEntryPoint, sAddr );
			CUtilities::BytesToHex( &_cwoHeader.AddressOfEntryPoint, sizeof( _cwoHeader.AddressOfEntryPoint ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwoHeader.AddressOfEntryPoint );
			QList<QVariant> lValues;
			lValues << "AddressOfEntryPoint" << sAddr.c_str() << szOffset << szBufferRaw << "A pointer to the entry point function, relative to the image base address.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CUtilities::ToHex( _cwoHeader.BaseOfCode, szBuffer, MX_ELEMENTS( szBuffer ), 8 );
			CUtilities::BytesToHex( &_cwoHeader.BaseOfCode, sizeof( _cwoHeader.BaseOfCode ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwoHeader.BaseOfCode );
			QList<QVariant> lValues;
			lValues << "BaseOfCode" << szBuffer << szOffset << szBufferRaw << "A pointer to the beginning of the code section, relative to the image base.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes a PE 32 header to a given parent.
	VOID CPeTreeModel::AddPe32Header( CTreeViewItem * _ptviParent, const MX_COFF_WINDOWS_PE32 &_cwpHeader, uint64_t _uiStructOffset ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];
		{
			CUtilities::ToHex( _cwpHeader.BaseOfData, szBuffer, MX_ELEMENTS( szBuffer ), 8 );
			CUtilities::BytesToHex( &_cwpHeader.BaseOfData, sizeof( _cwpHeader.BaseOfData ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.BaseOfData );
			QList<QVariant> lValues;
			lValues << "BaseOfData" << szBuffer << szOffset << szBufferRaw << "A pointer to the beginning of the data section, relative to the image base.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CUtilities::ToHex( _cwpHeader.ImageBase, szBuffer, MX_ELEMENTS( szBuffer ), 8 );
			CUtilities::BytesToHex( &_cwpHeader.ImageBase, sizeof( _cwpHeader.ImageBase ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.ImageBase );
			QList<QVariant> lValues;
			lValues << "ImageBase" << szBuffer << szOffset << szBufferRaw << "The preferred address of the first byte of the image when it is loaded in memory.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.SectionAlignment );
			CUtilities::BytesToHex( &_cwpHeader.SectionAlignment, sizeof( _cwpHeader.SectionAlignment ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SectionAlignment );
			QList<QVariant> lValues;
			lValues << "SectionAlignment" << szBuffer << szOffset << szBufferRaw << "The alignment of sections loaded in memory, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.FileAlignment );
			CUtilities::BytesToHex( &_cwpHeader.FileAlignment, sizeof( _cwpHeader.FileAlignment ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.FileAlignment );
			QList<QVariant> lValues;
			lValues << "FileAlignment" << szBuffer << szOffset << szBufferRaw << "The alignment of the raw data of sections in the image file, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[32];
			std::sprintf( szTemp, "%u", _cwpHeader.MajorOperatingSystemVersion );
			std::string sTemp;
			sTemp.append( szTemp );
			std::sprintf( szTemp, " (%u.%u = ", _cwpHeader.MajorOperatingSystemVersion, _cwpHeader.MinorOperatingSystemVersion );
			sTemp.append( szTemp );
			CUtilities::WindowsVersion( _cwpHeader.MajorOperatingSystemVersion, _cwpHeader.MinorOperatingSystemVersion, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.MajorOperatingSystemVersion, sizeof( _cwpHeader.MajorOperatingSystemVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MajorOperatingSystemVersion );
			QList<QVariant> lValues;
			lValues << "MajorOperatingSystemVersion" << sTemp.c_str() << szOffset << szBufferRaw << "The major version number of the required operating system.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorOperatingSystemVersion );
			CUtilities::BytesToHex( &_cwpHeader.MinorOperatingSystemVersion, sizeof( _cwpHeader.MinorOperatingSystemVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MinorOperatingSystemVersion );
			QList<QVariant> lValues;
			lValues << "MinorOperatingSystemVersion" << szBuffer << szOffset << szBufferRaw << "The minor version number of the required operating system.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MajorImageVersion );
			CUtilities::BytesToHex( &_cwpHeader.MajorImageVersion, sizeof( _cwpHeader.MajorImageVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MajorImageVersion );
			QList<QVariant> lValues;
			lValues << "MajorImageVersion" << szBuffer << szOffset << szBufferRaw << "The major version number of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorImageVersion );
			CUtilities::BytesToHex( &_cwpHeader.MinorImageVersion, sizeof( _cwpHeader.MinorImageVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MinorImageVersion );
			QList<QVariant> lValues;
			lValues << "MinorImageVersion" << szBuffer << szOffset << szBufferRaw << "The minor version number of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MajorSubsystemVersion );
			CUtilities::BytesToHex( &_cwpHeader.MajorSubsystemVersion, sizeof( _cwpHeader.MajorSubsystemVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MajorSubsystemVersion );
			QList<QVariant> lValues;
			lValues << "MajorSubsystemVersion" << szBuffer << szOffset << szBufferRaw << "The major version number of the subsystem.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorSubsystemVersion );
			CUtilities::BytesToHex( &_cwpHeader.MinorSubsystemVersion, sizeof( _cwpHeader.MinorSubsystemVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MinorSubsystemVersion );
			QList<QVariant> lValues;
			lValues << "MinorSubsystemVersion" << szBuffer << szOffset << szBufferRaw << "The minor version number of the subsystem.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.Win32VersionValue );
			CUtilities::BytesToHex( &_cwpHeader.Win32VersionValue, sizeof( _cwpHeader.Win32VersionValue ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.Win32VersionValue );
			QList<QVariant> lValues;
			lValues << "Win32VersionValue" << szBuffer << szOffset << szBufferRaw << "This member is reserved and must be 0.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfImage );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfImage, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfImage, sizeof( _cwpHeader.SizeOfImage ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfImage );
			QList<QVariant> lValues;
			lValues << "SizeOfImage" << sTemp.c_str() << szOffset << szBufferRaw << "The size of the image, in bytes, including all headers.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfHeaders );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfHeaders, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfHeaders, sizeof( _cwpHeader.SizeOfHeaders ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfHeaders );
			QList<QVariant> lValues;
			lValues << "SizeOfHeaders" << sTemp.c_str() << szOffset << szBufferRaw << "The combined size of the e_lfanew member of IMAGE_DOS_HEADER, 4-byte signature, size of IMAGE_FILE_HEADER, size of optional header, and the size of all section headers rounded to a multiple of the value specified in the FileAlignment member.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.CheckSum );
			CUtilities::BytesToHex( &_cwpHeader.CheckSum, sizeof( _cwpHeader.CheckSum ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.CheckSum );
			QList<QVariant> lValues;
			lValues << "CheckSum" << szBuffer << szOffset << szBufferRaw << "The image file checksum.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[64];
			CUtilities::PeSubSystemTypeToString( _cwpHeader.Subsystem, szTemp );
			std::sprintf( szBuffer, "%s (%u)", szTemp, _cwpHeader.Subsystem );
			CUtilities::BytesToHex( &_cwpHeader.Subsystem, sizeof( _cwpHeader.Subsystem ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.Subsystem );
			QList<QVariant> lValues;
			lValues << "Subsystem" << szBuffer << szOffset << szBufferRaw << "The subsystem required to run this image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[48];
			std::sprintf( szTemp, " (%u, %s)", _cwpHeader.DllCharacteristics, CUtilities::ToHex( _cwpHeader.DllCharacteristics, 4 ) );
			std::string sVal;
			CUtilities::PeDllCharacteristicsToString( _cwpHeader.DllCharacteristics, sVal );
			sVal.append( szTemp );
			CUtilities::BytesToHex( &_cwpHeader.DllCharacteristics, sizeof( _cwpHeader.DllCharacteristics ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.DllCharacteristics );
			QList<QVariant> lValues;
			lValues << "DllCharacteristics" << sVal.c_str() << szOffset << szBufferRaw << "The DLL characteristics of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfStackReserve );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfStackReserve, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfStackReserve, sizeof( _cwpHeader.SizeOfStackReserve ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfStackReserve );
			QList<QVariant> lValues;
			lValues << "SizeOfStackReserve" << sTemp.c_str() << szOffset << szBufferRaw << "The number of bytes to reserve for the stack.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfStackCommit );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfStackCommit, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfStackCommit, sizeof( _cwpHeader.SizeOfStackCommit ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfStackCommit );
			QList<QVariant> lValues;
			lValues << "SizeOfStackCommit" << sTemp.c_str() << szOffset << szBufferRaw << "The number of bytes to commit for the stack.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfHeapReserve );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfHeapReserve, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfHeapReserve, sizeof( _cwpHeader.SizeOfHeapReserve ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfHeapReserve );
			QList<QVariant> lValues;
			lValues << "SizeOfHeapReserve" << sTemp.c_str() << szOffset << szBufferRaw << "The number of bytes to reserve for the local heap.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfHeapCommit );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfHeapCommit, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfHeapCommit, sizeof( _cwpHeader.SizeOfHeapCommit ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfHeapCommit );
			QList<QVariant> lValues;
			lValues << "SizeOfHeapCommit" << sTemp.c_str() << szOffset << szBufferRaw << "The number of bytes to commit for the local heap.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.LoaderFlags );
			CUtilities::BytesToHex( &_cwpHeader.LoaderFlags, sizeof( _cwpHeader.LoaderFlags ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.LoaderFlags );
			QList<QVariant> lValues;
			lValues << "LoaderFlags" << szBuffer << szOffset << szBufferRaw << "Obsolete. If bit 0 is set, a breakpoint is invoked before starting the process.  If bit 1 is set, a debugger is invoked on the process after it is loaded.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.NumberOfRvaAndSizes );
			CUtilities::BytesToHex( &_cwpHeader.NumberOfRvaAndSizes, sizeof( _cwpHeader.NumberOfRvaAndSizes ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.NumberOfRvaAndSizes );
			QList<QVariant> lValues;
			lValues << "NumberOfRvaAndSizes" << szBuffer << szOffset << szBufferRaw << "The number of directory entries in the remainder of the optional header. Each entry describes a location and size.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes a PE 64 header to a given parent.
	VOID CPeTreeModel::AddPe64Header( CTreeViewItem * _ptviParent, const MX_COFF_WINDOWS_PE64 &_cwpHeader, uint64_t _uiStructOffset ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];
		{
			std::sprintf( szBuffer, "%s", CUtilities::ToHex( _cwpHeader.ImageBase, 16 ) );
			CUtilities::BytesToHex( &_cwpHeader.ImageBase, sizeof( _cwpHeader.ImageBase ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.ImageBase );
			QList<QVariant> lValues;
			lValues << "ImageBase" << szBuffer << szOffset << szBufferRaw << "The preferred address of the first byte of the image when it is loaded in memory.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.SectionAlignment );
			CUtilities::BytesToHex( &_cwpHeader.SectionAlignment, sizeof( _cwpHeader.SectionAlignment ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SectionAlignment );
			QList<QVariant> lValues;
			lValues << "SectionAlignment" << szBuffer << szOffset << szBufferRaw << "The alignment of sections loaded in memory, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.FileAlignment );
			CUtilities::BytesToHex( &_cwpHeader.FileAlignment, sizeof( _cwpHeader.FileAlignment ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.FileAlignment );
			QList<QVariant> lValues;
			lValues << "FileAlignment" << szBuffer << szOffset << szBufferRaw << "The alignment of the raw data of sections in the image file, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[32];
			std::sprintf( szTemp, "%u", _cwpHeader.MajorOperatingSystemVersion );
			std::string sTemp;
			sTemp.append( szTemp );
			std::sprintf( szTemp, " (%u.%u = ", _cwpHeader.MajorOperatingSystemVersion, _cwpHeader.MinorOperatingSystemVersion );
			sTemp.append( szTemp );
			CUtilities::WindowsVersion( _cwpHeader.MajorOperatingSystemVersion, _cwpHeader.MinorOperatingSystemVersion, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.MajorOperatingSystemVersion, sizeof( _cwpHeader.MajorOperatingSystemVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MajorOperatingSystemVersion );
			QList<QVariant> lValues;
			lValues << "MajorOperatingSystemVersion" << sTemp.c_str() << szOffset << szBufferRaw << "The major version number of the required operating system.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorOperatingSystemVersion );
			CUtilities::BytesToHex( &_cwpHeader.MinorOperatingSystemVersion, sizeof( _cwpHeader.MinorOperatingSystemVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MinorOperatingSystemVersion );
			QList<QVariant> lValues;
			lValues << "MinorOperatingSystemVersion" << szBuffer << szOffset << szBufferRaw << "The minor version number of the required operating system.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MajorImageVersion );
			CUtilities::BytesToHex( &_cwpHeader.MajorImageVersion, sizeof( _cwpHeader.MajorImageVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MajorImageVersion );
			QList<QVariant> lValues;
			lValues << "MajorImageVersion" << szBuffer << szOffset << szBufferRaw << "The major version number of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorImageVersion );
			CUtilities::BytesToHex( &_cwpHeader.MinorImageVersion, sizeof( _cwpHeader.MinorImageVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MinorImageVersion );
			QList<QVariant> lValues;
			lValues << "MinorImageVersion" << szBuffer << szOffset << szBufferRaw << "The minor version number of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MajorSubsystemVersion );
			CUtilities::BytesToHex( &_cwpHeader.MajorSubsystemVersion, sizeof( _cwpHeader.MajorSubsystemVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MajorSubsystemVersion );
			QList<QVariant> lValues;
			lValues << "MajorSubsystemVersion" << szBuffer << szOffset << szBufferRaw << "The major version number of the subsystem.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorSubsystemVersion );
			CUtilities::BytesToHex( &_cwpHeader.MinorSubsystemVersion, sizeof( _cwpHeader.MinorSubsystemVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.MinorSubsystemVersion );
			QList<QVariant> lValues;
			lValues << "MinorSubsystemVersion" << szBuffer << szOffset << szBufferRaw << "The minor version number of the subsystem.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.Win32VersionValue );
			CUtilities::BytesToHex( &_cwpHeader.Win32VersionValue, sizeof( _cwpHeader.Win32VersionValue ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.Win32VersionValue );
			QList<QVariant> lValues;
			lValues << "Win32VersionValue" << szBuffer << szOffset << szBufferRaw << "This member is reserved and must be 0.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfImage );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfImage, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfImage, sizeof( _cwpHeader.SizeOfImage ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfImage );
			QList<QVariant> lValues;
			lValues << "SizeOfImage" << sTemp.c_str() << szOffset << szBufferRaw << "The size of the image, in bytes, including all headers.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfHeaders );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfHeaders, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfHeaders, sizeof( _cwpHeader.SizeOfHeaders ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfHeaders );
			QList<QVariant> lValues;
			lValues << "SizeOfHeaders" << sTemp.c_str() << szOffset << szBufferRaw << "The combined size of the e_lfanew member of IMAGE_DOS_HEADER, 4-byte signature, size of IMAGE_FILE_HEADER, size of optional header, and the size of all section headers rounded to a multiple of the value specified in the FileAlignment member.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.CheckSum );
			CUtilities::BytesToHex( &_cwpHeader.CheckSum, sizeof( _cwpHeader.CheckSum ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.CheckSum );
			QList<QVariant> lValues;
			lValues << "CheckSum" << szBuffer << szOffset << szBufferRaw << "The image file checksum.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[64];
			CUtilities::PeSubSystemTypeToString( _cwpHeader.Subsystem, szTemp );
			std::sprintf( szBuffer, "%s (%u)", szTemp, _cwpHeader.Subsystem );
			CUtilities::BytesToHex( &_cwpHeader.Subsystem, sizeof( _cwpHeader.Subsystem ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.Subsystem );
			QList<QVariant> lValues;
			lValues << "Subsystem" << szBuffer << szOffset << szBufferRaw << "The subsystem required to run this image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[48];
			std::sprintf( szTemp, " (%u, %s)", _cwpHeader.DllCharacteristics, CUtilities::ToHex( _cwpHeader.DllCharacteristics, 4 ) );
			std::string sVal;
			CUtilities::PeDllCharacteristicsToString( _cwpHeader.DllCharacteristics, sVal );
			sVal.append( szTemp );
			CUtilities::BytesToHex( &_cwpHeader.DllCharacteristics, sizeof( _cwpHeader.DllCharacteristics ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.DllCharacteristics );
			QList<QVariant> lValues;
			lValues << "DllCharacteristics" << sVal.c_str() << szOffset << szBufferRaw << "The DLL characteristics of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfStackReserve );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfStackReserve, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfStackReserve, sizeof( _cwpHeader.SizeOfStackReserve ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfStackReserve );
			QList<QVariant> lValues;
			lValues << "SizeOfStackReserve" << sTemp.c_str() << szOffset << szBufferRaw << "The number of bytes to reserve for the stack.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfStackCommit );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfStackCommit, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfStackCommit, sizeof( _cwpHeader.SizeOfStackCommit ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfStackCommit );
			QList<QVariant> lValues;
			lValues << "SizeOfStackCommit" << sTemp.c_str() << szOffset << szBufferRaw << "The number of bytes to commit for the stack.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfHeapReserve );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfHeapReserve, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfHeapReserve, sizeof( _cwpHeader.SizeOfHeapReserve ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfHeapReserve );
			QList<QVariant> lValues;
			lValues << "SizeOfHeapReserve" << sTemp.c_str() << szOffset << szBufferRaw << "The number of bytes to reserve for the local heap.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfHeapCommit );
			sTemp.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfHeapCommit, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_cwpHeader.SizeOfHeapCommit, sizeof( _cwpHeader.SizeOfHeapCommit ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.SizeOfHeapCommit );
			QList<QVariant> lValues;
			lValues << "SizeOfHeapCommit" << sTemp.c_str() << szOffset << szBufferRaw << "The number of bytes to commit for the local heap.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.LoaderFlags );
			CUtilities::BytesToHex( &_cwpHeader.LoaderFlags, sizeof( _cwpHeader.LoaderFlags ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.LoaderFlags );
			QList<QVariant> lValues;
			lValues << "LoaderFlags" << szBuffer << szOffset << szBufferRaw << "Obsolete. If bit 0 is set, a breakpoint is invoked before starting the process.  If bit 1 is set, a debugger is invoked on the process after it is loaded.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.NumberOfRvaAndSizes );
			CUtilities::BytesToHex( &_cwpHeader.NumberOfRvaAndSizes, sizeof( _cwpHeader.NumberOfRvaAndSizes ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _cwpHeader.NumberOfRvaAndSizes );
			QList<QVariant> lValues;
			lValues << "NumberOfRvaAndSizes" << szBuffer << szOffset << szBufferRaw << "The number of directory entries in the remainder of the optional header. Each entry describes a location and size.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes an array of data directories.
	VOID CPeTreeModel::AddDataDirectories( CTreeViewItem * _ptviParent, const std::vector<MX_DATA_DIRECTORY> &_vDataDirs, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset ) {
		struct {
			const CHAR * pcName;
			uint32_t ui32StrLen;

			const CHAR * pcDescName;
			uint32_t ui32DescStrLen;
		}
		static const aTable[MX_IMAGE_NUMBEROF_DIRECTORY_ENTRIES] = {
			{ _T_B4C7FDE2_DataDirectory_IMAGE_DIRECTORY_ENTRY_EXPORT_, _LEN_B4C7FDE2, _T_E00180C9_Export_directory_, _LEN_E00180C9 },
			{ _T_207C04BE_DataDirectory_IMAGE_DIRECTORY_ENTRY_IMPORT_, _LEN_207C04BE, _T_E9670B10_Import_directory_, _LEN_E9670B10 },
			{ _T_E94D3397_DataDirectory_IMAGE_DIRECTORY_ENTRY_RESOURCE_, _LEN_E94D3397, _T_43D2366B_Resource_directory_, _LEN_43D2366B },
			{ _T_352498F1_DataDirectory_IMAGE_DIRECTORY_ENTRY_EXCEPTION_, _LEN_352498F1, _T_7E617328_Exception_directory_, _LEN_7E617328 },
			{ _T_F1837EC1_DataDirectory_IMAGE_DIRECTORY_ENTRY_SECURITY_, _LEN_F1837EC1, _T_E318A398_Security_directory_, _LEN_E318A398 },
			{ _T_3918D7CF_DataDirectory_IMAGE_DIRECTORY_ENTRY_BASERELOC_, _LEN_3918D7CF, _T_3C63033E_Base_relocation_table_, _LEN_3C63033E },
			{ _T_35CDC213_DataDirectory_IMAGE_DIRECTORY_ENTRY_DEBUG_, _LEN_35CDC213, _T_FC6D4DB9_Debug_directory_, _LEN_FC6D4DB9 },
			{ _T_B9A5B87A_DataDirectory_IMAGE_DIRECTORY_ENTRY_ARCHITECTURE_, _LEN_B9A5B87A, _T_51010B52_Architecture_specific_data_, _LEN_51010B52 },
			{ _T_11FA74D4_DataDirectory_IMAGE_DIRECTORY_ENTRY_GLOBALPTR_, _LEN_11FA74D4, _T_2F9F4C35_Global_pointer_register_relative_virtual_address_, _LEN_2F9F4C35 },
			{ _T_0A097252_DataDirectory_IMAGE_DIRECTORY_ENTRY_TLS_, _LEN_0A097252, _T_D218D979_Thread_local_storage_directory_, _LEN_D218D979 },
			{ _T_5F596E7F_DataDirectory_IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG_, _LEN_5F596E7F, _T_5FC3427C_Load_configuration_directory_, _LEN_5FC3427C },
			{ _T_15F78447_DataDirectory_IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT_, _LEN_15F78447, _T_462CE6FA_Bound_import_directory_, _LEN_462CE6FA },
			{ _T_EFE3F884_DataDirectory_IMAGE_DIRECTORY_ENTRY_IAT_, _LEN_EFE3F884, _T_ADA7D54A_Import_address_table_, _LEN_ADA7D54A },
			{ _T_261AB71E_DataDirectory_IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT_, _LEN_261AB71E, _T_493F6470_Delay_import_table_, _LEN_493F6470 },
			{ _T_716D9916_DataDirectory_IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR_, _LEN_716D9916, _T_E80FA376_COM_descriptor_table_, _LEN_E80FA376 },
			{ _T_B8592F2A_DataDirectory_15_, _LEN_B8592F2A, _T_F9026196_Reserved_, _LEN_F9026196 },
		};

		CHAR szBuffer[_T_MAX_LEN];
		CHAR szDescBuffer[_T_MAX_LEN];
		CHAR szTemp[64];
		for ( size_t I = 0; I < _vDataDirs.size(); ++I ) {
			CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32StrLen, szBuffer );
			CStringDecoder::Decode( aTable[I].pcDescName, aTable[I].ui32DescStrLen, szDescBuffer );
			std::sprintf( szTemp, "VirtualAddress=%s, Size=%u", CUtilities::ToHex( _vDataDirs[I].VirtualAddress, 8 ), _vDataDirs[I].Size );
			CHAR szOffset[32];
			std::sprintf( szOffset, "%s (%I64u)", CUtilities::ToHex( _uiStructOffset, 8 ), _uiStructOffset );
			QList<QVariant> lValues;
			lValues << szBuffer << szTemp << szOffset << "" << szDescBuffer;
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
			if ( _vDataDirs[I].VirtualAddress ) {
				AddDataDirectory( ptviTemp, _vDataDirs[I], _poPeObject, _uiImageBase, _uiStructOffset );
			}
			_uiStructOffset += sizeof( MX_DATA_DIRECTORY );
		}
	}

	// Decodes a data directory.
	VOID CPeTreeModel::AddDataDirectory( CTreeViewItem * _ptviParent, const MX_DATA_DIRECTORY &_ddDir, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];

		{
			std::string sAddr;
			_poPeObject.RelocAddressToStringwithSection( _ddDir.VirtualAddress, sAddr );
			CUtilities::BytesToHex( &_ddDir.VirtualAddress, sizeof( _ddDir.VirtualAddress ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ddDir.VirtualAddress );
			QList<QVariant> lValues;
			lValues << "VirtualAddress" << sAddr.c_str() << szOffset << szBufferRaw << "";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			if ( !_ddDir.Size ) {
				std::sprintf( szBuffer, "%u", _ddDir.Size );
			}
			else {
				std::sprintf( szBuffer, "%u (", _ddDir.Size );
			}
			sTemp.append( szBuffer );
			if ( _ddDir.Size ) {
				CUtilities::SizeString( _ddDir.Size, sTemp );
				sTemp.push_back( ')' );
			}
			CUtilities::BytesToHex( &_ddDir.Size, sizeof( _ddDir.Size ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ddDir.Size );
			QList<QVariant> lValues;
			lValues << "Size" << sTemp.c_str() << szOffset << szBufferRaw << "";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes an array of MX_IMAGE_SECTION_HEADER objects.
	VOID CPeTreeModel::AddSectionHeaders( CTreeViewItem * _ptviParent, const std::vector<MX_IMAGE_SECTION_HEADER> &_vHeaders, uint64_t _uiImageBase, uint64_t _uiStructOffset ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];

		for ( size_t I = 0; I < _vHeaders.size(); ++I ) {
			CHAR szTemp[16];
			ZeroMemory( szTemp, sizeof( szTemp ) );
			std::memcpy( szTemp, _vHeaders[I].Name, 8 );
			QList<QVariant> lValues;
			lValues << szTemp << "" << "" << "" << "";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
			AddSectionHeader( ptviTemp, _vHeaders[I], _uiImageBase, _uiStructOffset );
			_uiStructOffset += sizeof( MX_IMAGE_SECTION_HEADER );
		}
	}

	// Decodes an MX_IMAGE_SECTION_HEADER object.
	VOID CPeTreeModel::AddSectionHeader( CTreeViewItem * _ptviParent, const MX_IMAGE_SECTION_HEADER &_ishHeader, uint64_t _uiImageBase, uint64_t _uiStructOffset ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];

		{
			CHAR szTemp[16];
			ZeroMemory( szTemp, sizeof( szTemp ) );
			std::memcpy( szTemp, _ishHeader.Name, 8 );
			CUtilities::BytesToHexWithSpaces( &_ishHeader.Name, sizeof( _ishHeader.Name ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ishHeader.Name );
			QList<QVariant> lValues;
			lValues << "Name" << szTemp << szOffset << szBufferRaw << "Name of the section.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%s (%u)", CUtilities::ToHex( _ishHeader.Misc.PhysicalAddress, 8 ), _ishHeader.Misc.PhysicalAddress );
			CUtilities::BytesToHex( &_ishHeader.Misc.PhysicalAddress, sizeof( _ishHeader.Misc.PhysicalAddress ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ishHeader.Misc );
			QList<QVariant> lValues;
			lValues << "PhysicalAddress/VirtualSize" << szBuffer << szOffset << szBufferRaw << "A physical address or the total size of the section when loaded into memory, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTempOff[32];
			std::sprintf( szBuffer, "%s (%s after loading)", CUtilities::ToHex( _ishHeader.VirtualAddress, 8 ),
				CUtilities::ToHex( _ishHeader.VirtualAddress + _uiImageBase, szTempOff, MX_ELEMENTS( szTempOff ), 0 ) );
			CUtilities::BytesToHex( &_ishHeader.VirtualAddress, sizeof( _ishHeader.VirtualAddress ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ishHeader.VirtualAddress );
			QList<QVariant> lValues;
			lValues << "VirtualAddress" << szBuffer << szOffset << szBufferRaw << "The address of the first byte of the section when loaded into memory, relative to the image base.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u (", _ishHeader.SizeOfRawData, _ishHeader.SizeOfRawData );
			std::string sTemp;
			sTemp.append( szBuffer );
			CUtilities::SizeString( _ishHeader.SizeOfRawData, sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_ishHeader.SizeOfRawData, sizeof( _ishHeader.SizeOfRawData ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ishHeader.SizeOfRawData );
			QList<QVariant> lValues;
			lValues << "SizeOfRawData" << sTemp.c_str() << szOffset << szBufferRaw << "The size of the initialized data on disk, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%s", CUtilities::ToHex( _ishHeader.PointerToRawData, 8 ) );
			CUtilities::BytesToHex( &_ishHeader.PointerToRawData, sizeof( _ishHeader.PointerToRawData ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ishHeader.PointerToRawData );
			QList<QVariant> lValues;
			lValues << "PointerToRawData" << szBuffer << szOffset << szBufferRaw << "A file pointer to the first page within the COFF file.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%s", CUtilities::ToHex( _ishHeader.PointerToRelocations, 8 ) );
			CUtilities::BytesToHex( &_ishHeader.PointerToRelocations, sizeof( _ishHeader.PointerToRelocations ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ishHeader.PointerToRelocations );
			QList<QVariant> lValues;
			lValues << "PointerToRelocations" << szBuffer << szOffset << szBufferRaw << "A file pointer to the beginning of the relocation entries for the section.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%s", CUtilities::ToHex( _ishHeader.PointerToLinenumbers, 8 ) );
			CUtilities::BytesToHex( &_ishHeader.PointerToLinenumbers, sizeof( _ishHeader.PointerToLinenumbers ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ishHeader.PointerToLinenumbers );
			QList<QVariant> lValues;
			lValues << "PointerToLinenumbers" << szBuffer << szOffset << szBufferRaw << "A file pointer to the beginning of the line-number entries for the section.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _ishHeader.NumberOfRelocations );
			CUtilities::BytesToHex( &_ishHeader.NumberOfRelocations, sizeof( _ishHeader.NumberOfRelocations ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ishHeader.NumberOfRelocations );
			QList<QVariant> lValues;
			lValues << "NumberOfRelocations" << szBuffer << szOffset << szBufferRaw << "The number of relocation entries for the section.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _ishHeader.NumberOfLinenumbers );
			CUtilities::BytesToHex( &_ishHeader.NumberOfLinenumbers, sizeof( _ishHeader.NumberOfLinenumbers ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ishHeader.NumberOfLinenumbers );
			QList<QVariant> lValues;
			lValues << "NumberOfLinenumbers" << szBuffer << szOffset << szBufferRaw << "The number of line-number entries for the section.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[48];
			std::sprintf( szTemp, " (%u, %s)", _ishHeader.Characteristics, CUtilities::ToHex( _ishHeader.Characteristics, 4 ) );
			std::string sVal;
			CUtilities::PeSectionFlagsToString( _ishHeader.Characteristics, sVal );
			sVal.append( szTemp );
			CUtilities::BytesToHex( &_ishHeader.Characteristics, sizeof( _ishHeader.Characteristics ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ishHeader.Characteristics );
			QList<QVariant> lValues;
			lValues << "Characteristics" << sVal.c_str() << szOffset << szBufferRaw << "The characteristics of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes exports.
	VOID CPeTreeModel::AddExports( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject ) {
		if ( _poPeObject.ExportDescriptor() ) {
			QList<QVariant> lValues;
			lValues << "Export Description" << "" << "" << "" << "Export descriptor.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			AddExportDesc( ptviTemp, _poPeObject );
		}
	}

	// Decodes an MX_IMAGE_EXPORT_DIRECTORY object.
	VOID CPeTreeModel::AddExportDesc( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];

		uint64_t uiOffset = _poPeObject.ExportDescriptorOffset();
		{
			std::sprintf( szBuffer, "%u", _poPeObject.ExportDescriptor()->Characteristics );
			CUtilities::BytesToHex( &_poPeObject.ExportDescriptor()->Characteristics, sizeof( _poPeObject.ExportDescriptor()->Characteristics ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, _poPeObject.ExportDescriptor()->Characteristics );
			QList<QVariant> lValues;
			lValues << "Characteristics" << szBuffer << szOffset << szBufferRaw << "Unused.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CUtilities::CreateDateString( _poPeObject.ExportDescriptor()->TimeDateStamp, szBuffer, sizeof( szBuffer ) );
			CUtilities::BytesToHex( &_poPeObject.ExportDescriptor()->TimeDateStamp, sizeof( _poPeObject.ExportDescriptor()->TimeDateStamp ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, _poPeObject.ExportDescriptor()->TimeDateStamp );
			QList<QVariant> lValues;
			lValues << "TimeDateStamp" << szBuffer << szOffset << szBufferRaw << "The time/date stamp indicating when this file was created.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _poPeObject.ExportDescriptor()->MajorVersion );
			CUtilities::BytesToHex( &_poPeObject.ExportDescriptor()->MajorVersion, sizeof( _poPeObject.ExportDescriptor()->MajorVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, _poPeObject.ExportDescriptor()->MajorVersion );
			QList<QVariant> lValues;
			lValues << "MajorVersion" << szBuffer << szOffset << szBufferRaw << "Unused.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _poPeObject.ExportDescriptor()->MinorVersion );
			CUtilities::BytesToHex( &_poPeObject.ExportDescriptor()->MinorVersion, sizeof( _poPeObject.ExportDescriptor()->MinorVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, _poPeObject.ExportDescriptor()->MinorVersion );
			QList<QVariant> lValues;
			lValues << "MinorVersion" << szBuffer << szOffset << szBufferRaw << "Unused.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%s (", CUtilities::ToHex( _poPeObject.ExportDescriptor()->Name, 8 ) );
			std::string sTemp = szBuffer;
			_poPeObject.GetExportDllName( sTemp );
			sTemp.push_back( ')' );
			CUtilities::BytesToHex( &_poPeObject.ExportDescriptor()->Name, sizeof( _poPeObject.ExportDescriptor()->Name ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, _poPeObject.ExportDescriptor()->Name );
			QList<QVariant> lValues;
			lValues << "Name" << sTemp.c_str() << szOffset << szBufferRaw << "Name of this DLL.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _poPeObject.ExportDescriptor()->Base );
			CUtilities::BytesToHex( &_poPeObject.ExportDescriptor()->Base, sizeof( _poPeObject.ExportDescriptor()->Base ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, _poPeObject.ExportDescriptor()->Base );
			QList<QVariant> lValues;
			lValues << "Base" << szBuffer << szOffset << szBufferRaw << "The starting ordinal number for exported functions.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _poPeObject.ExportDescriptor()->NumberOfFunctions );
			CUtilities::BytesToHex( &_poPeObject.ExportDescriptor()->NumberOfFunctions, sizeof( _poPeObject.ExportDescriptor()->NumberOfFunctions ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, _poPeObject.ExportDescriptor()->NumberOfFunctions );
			QList<QVariant> lValues;
			lValues << "NumberOfFunctions" << szBuffer << szOffset << szBufferRaw << "The number of elements in the AddressOfFunctions array.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _poPeObject.ExportDescriptor()->NumberOfNames );
			CUtilities::BytesToHex( &_poPeObject.ExportDescriptor()->NumberOfNames, sizeof( _poPeObject.ExportDescriptor()->NumberOfNames ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, _poPeObject.ExportDescriptor()->NumberOfNames );
			QList<QVariant> lValues;
			lValues << "NumberOfNames" << szBuffer << szOffset << szBufferRaw << "The number of elements in the AddressOfNames array.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sAddr;
			_poPeObject.RelocAddressToStringwithSection( _poPeObject.ExportDescriptor()->AddressOfFunctions, sAddr );
			CUtilities::BytesToHex( &_poPeObject.ExportDescriptor()->AddressOfFunctions, sizeof( _poPeObject.ExportDescriptor()->AddressOfFunctions ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, _poPeObject.ExportDescriptor()->AddressOfFunctions );
			QList<QVariant> lValues;
			lValues << "AddressOfFunctions" << sAddr.c_str() << szOffset << szBufferRaw << "Points to an array of function addresses.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sAddr;
			_poPeObject.RelocAddressToStringwithSection( _poPeObject.ExportDescriptor()->AddressOfNames, sAddr );
			CUtilities::BytesToHex( &_poPeObject.ExportDescriptor()->AddressOfNames, sizeof( _poPeObject.ExportDescriptor()->AddressOfNames ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, _poPeObject.ExportDescriptor()->AddressOfNames );
			QList<QVariant> lValues;
			lValues << "AddressOfNames" << sAddr.c_str() << szOffset << szBufferRaw << "Points to an array of function names.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sAddr;
			_poPeObject.RelocAddressToStringwithSection( _poPeObject.ExportDescriptor()->AddressOfNameOrdinals, sAddr );
			CUtilities::BytesToHex( &_poPeObject.ExportDescriptor()->AddressOfNameOrdinals, sizeof( _poPeObject.ExportDescriptor()->AddressOfNameOrdinals ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, _poPeObject.ExportDescriptor()->AddressOfNameOrdinals );
			QList<QVariant> lValues;
			lValues << "AddressOfNameOrdinals" << sAddr.c_str() << szOffset << szBufferRaw << "Points to an array of function ordinals.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes exported functions.
	VOID CPeTreeModel::AddExportedFunctions( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject ) {
		std::vector<CPeObject::MX_EXPORT> vList;
		_poPeObject.GetExports( vList );

		if ( vList.size() ) {
			QList<QVariant> lValues;
			lValues << "Exports" << "" << "" << "" << "Exported functions.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );

			AddExportedFunctions( ptviTemp, _poPeObject, vList );
		}
	}

	// Decodes exported functions.
	VOID CPeTreeModel::AddExportedFunctions( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject, const std::vector<CPeObject::MX_EXPORT> &_vList ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];
		for ( size_t I = 0; I < _vList.size(); ++I ) {
			std::string sBuffer;
			CUtilities::ToHex( _vList[I].uiAddress, sBuffer, _poPeObject.Is64() ? 16 : 8 );
			sBuffer.append( ", " );
			CUtilities::ToUnsigned( _vList[I].uiOrdinal, sBuffer );
			sBuffer.append( ", " );
			sBuffer += _vList[I].sName;

			std::string sRaw;
			CUtilities::BytesToHex( &_vList[I].uiAddress, sizeof( _vList[I].uiAddress ), sRaw );
			sRaw.push_back( ' ' );
			CUtilities::BytesToHex( &_vList[I].uiOrdinal, sizeof( _vList[I].uiOrdinal ), sRaw );
			sRaw.push_back( ' ' );
			CUtilities::BytesToHexWithSpaces( _vList[I].sName.c_str(), _vList[I].sName.size(), sRaw );
			CHAR szHex0[32], szU0[32];
			CHAR szHex1[32], szU1[32];
			CHAR szHex2[32], szU2[32];
			CHAR szOffset[128];
			::sprintf_s( szOffset, MX_ELEMENTS( szOffset ), "%s (%s), %s (%s), %s (%s)",
				CUtilities::ToHex( _vList[I].ui64FileOffsetAddr, szHex0, MX_ELEMENTS( szHex0 ), 8 ), CUtilities::ToUnsigned( _vList[I].ui64FileOffsetAddr, szU0, MX_ELEMENTS( szU0 ) ),
				CUtilities::ToHex( _vList[I].ui64FileOffsetOrd, szHex1, MX_ELEMENTS( szHex1 ), 8 ), CUtilities::ToUnsigned( _vList[I].ui64FileOffsetOrd, szU1, MX_ELEMENTS( szU1 ) ),
				CUtilities::ToHex( _vList[I].ui64FileOffsetName, szHex2, MX_ELEMENTS( szHex2 ), 8 ), CUtilities::ToUnsigned( _vList[I].ui64FileOffsetName, szU2, MX_ELEMENTS( szU2 ) ) );
			QList<QVariant> lValues;
			lValues << "Address, Ordinal, Name" << sBuffer.c_str() << szOffset << sRaw.c_str() << "";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes an MX_IMAGE_IMPORT_DESCRIPTOR object.
	VOID CPeTreeModel::AddImportDesc( CTreeViewItem * _ptviParent, const MX_IMAGE_IMPORT_DESCRIPTOR &_iidDesc, const mx::CPeObject &_poPeObject, uint64_t _uiStructOffset ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];
		{
			CHAR szTempOff1[32], szTempOff2[32];
			std::sprintf( szBuffer, "%s (file offset %s)",
				CUtilities::ToHex( _iidDesc.Characteristics, szTempOff1, MX_ELEMENTS( szTempOff1 ), 8 ),
				CUtilities::ToHex( _poPeObject.RelocAddressToFileOffset( _iidDesc.Characteristics ), szTempOff2, MX_ELEMENTS( szTempOff2 ), 8 ) );
			CUtilities::BytesToHex( &_iidDesc.Characteristics, sizeof( _iidDesc.Characteristics ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _iidDesc.Characteristics );
			QList<QVariant> lValues;
			lValues << "Characteristics" << szBuffer << szOffset << szBufferRaw << "A pointer to a table of imported functions from this library.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CUtilities::CreateDateString( _iidDesc.TimeDateStamp, szBuffer, sizeof( szBuffer ) );
			CUtilities::BytesToHex( &_iidDesc.TimeDateStamp, sizeof( _iidDesc.TimeDateStamp ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _iidDesc.TimeDateStamp );
			QList<QVariant> lValues;
			lValues << "TimeDateStamp" << szBuffer << szOffset << szBufferRaw << "32-bit time at which the file was generated (DD-MM-YYYY HH-MM-SS).";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%s", CUtilities::ToHex( _iidDesc.ForwarderChain, 8 ) );
			CUtilities::BytesToHex( &_iidDesc.ForwarderChain, sizeof( _iidDesc.ForwarderChain ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _iidDesc.ForwarderChain );
			QList<QVariant> lValues;
			lValues << "ForwarderChain" << szBuffer << szOffset << szBufferRaw << "Index into FirstThunk of a forwarded function or 0xFFFFFFFF.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			uint32_t uiOffset;
			uint32_t uiIndex = _poPeObject.RelocAddrToRelocIndexAndOffset( _iidDesc.Name, uiOffset );
			std::string sFinal;
			if ( uiIndex <= _poPeObject.SectionData().size() ) {
				std::string sTemp;
				while ( uiOffset < _poPeObject.SectionData()[uiIndex].vData.size() ) {
					CHAR cTemp = _poPeObject.SectionData()[uiIndex].vData[uiOffset++];
					if ( !cTemp ) { break; }
					sTemp.push_back( cTemp );
				}
				std::sprintf( szBuffer, "%s (", CUtilities::ToHex( _iidDesc.Name, 8 ) );
				sFinal = szBuffer;
				sFinal.append( sTemp );
				sFinal.push_back( ')' );
			}
			else {
				std::sprintf( szBuffer, "%s", CUtilities::ToHex( _iidDesc.Name, 8 ) );
				sFinal = szBuffer;
			}
			
			CUtilities::BytesToHex( &_iidDesc.Name, sizeof( _iidDesc.Name ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _iidDesc.Name );
			QList<QVariant> lValues;
			lValues << "Name" << sFinal.c_str() << szOffset << szBufferRaw << "Relative virtual address to the name of this DLL.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTempOff[32];
			std::sprintf( szBuffer, "%s (file offset %s)",
				CUtilities::ToHex( _iidDesc.FirstThunk, 8 ),
				CUtilities::ToHex( _poPeObject.RelocAddressToFileOffset( _iidDesc.FirstThunk ), szTempOff, MX_ELEMENTS( szTempOff ), 8 ) );
			CUtilities::BytesToHex( &_iidDesc.FirstThunk, sizeof( _iidDesc.FirstThunk ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _iidDesc.FirstThunk );
			QList<QVariant> lValues;
			lValues << "FirstThunk" << szBuffer << szOffset << szBufferRaw << "Relative virtual address to an IMAGE_THUNK_DATA union.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes all imports.
	VOID CPeTreeModel::AddImportedFuncs( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject ) {
		if ( _poPeObject.ImportDescriptor().size() > 1 ) {
			QList<QVariant> lValues;
			//MX_PART_TO_STRING( _poPeObject.DosHeader() );
			lValues << "Imports" << "" << "" << "" << "Function imports.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
			for ( size_t I = 0; I < _poPeObject.ImportDescriptor().size() - 1; ++I ) {
				AddImportedFuncs( ptviTemp, I, _poPeObject );
			}
		}
	}

	// Decodes imports from a given import directory.
	VOID CPeTreeModel::AddImportedFuncs( CTreeViewItem * _ptviParent, uint32_t _uiImportDir, const mx::CPeObject &_poPeObject ) {
		if ( _uiImportDir == _poPeObject.ImportDescriptor().size() - 1 ) { return; }
		{
			std::string sDllName;
			_poPeObject.ImportDllNameByIndex( _uiImportDir, sDllName );
			QList<QVariant> lValues;
			//MX_PART_TO_STRING( _poPeObject.DosHeader() );
			lValues << sDllName.c_str() << "" << "" << "" << "";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );

			std::vector<CPeObject::MX_IMPORT> vImports;
			_poPeObject.GetImportsFromImportDescByIndex( _uiImportDir, vImports );
			for ( size_t I = 0; I < vImports.size(); ++I ) {
				AddImport( ptviTemp, vImports[I], _poPeObject );
			}
		}
	}

	// Decodes an import.
	VOID CPeTreeModel::AddImport( CTreeViewItem * _ptviParent, const CPeObject::MX_IMPORT &_iImport, const mx::CPeObject &_poPeObject ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];

		uint64_t uiOffset = _iImport.ui64FileOffset;
		{
			
			std::string sBytes;
			
			if ( _iImport.bBound ) {
				uint8_t uiBuf[32];
				uint32_t uiSize = _poPeObject.Is64() ? sizeof( uint64_t ) : sizeof( uint32_t );
				_poPeObject.ReadRelocBytes( _poPeObject.FileOffsetToRelocAddress( _iImport.ui64FileOffset ), uiBuf, uiSize );
				CUtilities::BytesToHex( uiBuf, uiSize, sBytes );
			}
			else {
				CUtilities::BytesToHex( &_iImport.uiOrdinal, sizeof( _iImport.uiOrdinal ), sBytes );
				sBytes += ' ';
				CUtilities::BytesToHexWithSpaces( _iImport.sName.c_str(), _iImport.sName.size(), sBytes );
			}

			std::sprintf( szBuffer, "%s", CUtilities::ToHex( _iImport.uiOrdinal, 4 ) );
			std::string sName = szBuffer + std::string( ", " ) + _iImport.sName;
			MX_PRINT_FILE_OFFSET( uiOffset, _iImport.uiOrdinal );
			QList<QVariant> lValues;
			lValues << "Ordinal, Name" << sName.c_str() << szOffset << sBytes.c_str() << "";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes an MX_IMAGE_IMPORT_DESCRIPTOR object.
	VOID CPeTreeModel::AddResourceDesc( CTreeViewItem * _ptviParent, const MX_IMAGE_RESOURCE_DIRECTORY &_irdDesc, const mx::CPeObject &_poPeObject, uint64_t _uiStructOffset ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];

		{
			std::sprintf( szBuffer, "%u", _irdDesc.Characteristics );
			CUtilities::BytesToHex( &_irdDesc.Characteristics, sizeof( _irdDesc.Characteristics ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _irdDesc.Characteristics );
			QList<QVariant> lValues;
			lValues << "Characteristics" << szBuffer << szOffset << szBufferRaw << "Unused.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CUtilities::CreateDateString( _irdDesc.TimeDateStamp, szBuffer, sizeof( szBuffer ) );
			CUtilities::BytesToHex( &_irdDesc.TimeDateStamp, sizeof( _irdDesc.TimeDateStamp ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _irdDesc.TimeDateStamp );
			QList<QVariant> lValues;
			lValues << "TimeDateStamp" << szBuffer << szOffset << szBufferRaw << "32-bit time at which this file was generated (DD-MM-YYYY HH-MM-SS).";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _irdDesc.MajorVersion );
			CUtilities::BytesToHex( &_irdDesc.MajorVersion, sizeof( _irdDesc.MajorVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _irdDesc.MajorVersion );
			QList<QVariant> lValues;
			lValues << "MajorVersion" << szBuffer << szOffset << szBufferRaw << "Major version of the resource.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _irdDesc.MinorVersion );
			CUtilities::BytesToHex( &_irdDesc.MinorVersion, sizeof( _irdDesc.MinorVersion ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _irdDesc.MinorVersion );
			QList<QVariant> lValues;
			lValues << "MinorVersion" << szBuffer << szOffset << szBufferRaw << "Minor version of the resource.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _irdDesc.NumberOfNamedEntries );
			CUtilities::BytesToHex( &_irdDesc.NumberOfNamedEntries, sizeof( _irdDesc.NumberOfNamedEntries ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _irdDesc.NumberOfNamedEntries );
			QList<QVariant> lValues;
			lValues << "NumberOfNamedEntries" << szBuffer << szOffset << szBufferRaw << "Number of entries with names.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _irdDesc.NumberOfIdEntries );
			CUtilities::BytesToHex( &_irdDesc.NumberOfIdEntries, sizeof( _irdDesc.NumberOfIdEntries ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _irdDesc.NumberOfIdEntries );
			QList<QVariant> lValues;
			lValues << "NumberOfIdEntries" << szBuffer << szOffset << szBufferRaw << "Number of entries with ID values.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes the resource tree.
	VOID CPeTreeModel::AddResourceTree( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject ) {
		if ( !_poPeObject.ResourceDescriptor() ) { return; }
		uint32_t uiOffset;
		uint32_t uiIndex = _poPeObject.RelocAddrToRelocIndexAndOffset( _poPeObject.DataDirectory()[MX_IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress, uiOffset );
		AddResourceTree( _ptviParent, _poPeObject.ResourceDescriptor(), uiIndex, _poPeObject );
		/*
		uint32_t uiTotal = _poPeObject.ResourceDescriptor()->NumberOfNamedEntries + _poPeObject.ResourceDescriptor()->NumberOfIdEntries;
		
		const MX_IMAGE_RESOURCE_DIRECTORY_ENTRY * pirdeEntry = reinterpret_cast<const MX_IMAGE_RESOURCE_DIRECTORY_ENTRY *>(&_poPeObject.SectionData()[uiIndex].vData[uiOffset+sizeof( MX_IMAGE_RESOURCE_DIRECTORY )]);
		size_t sEndAddr = reinterpret_cast<size_t>(&_poPeObject.SectionData()[uiIndex].vData[0]) + _poPeObject.SectionData()[uiIndex].vData.size();
		for ( uint32_t I = 0; I < uiTotal; ++I, ++pirdeEntry ) {
			// Check bounds.
			if ( reinterpret_cast<size_t>(pirdeEntry + 1) > sEndAddr ) {
				// Trying to read past the section.  No way to continue.
				break;
			}
			AddResourceTree( _ptviParent, pirdeEntry, uiIndex, _poPeObject );
		}*/
	}

	// Decodes the resource tree.
	VOID CPeTreeModel::AddResourceTree( CTreeViewItem * _ptviParent, const MX_IMAGE_RESOURCE_DIRECTORY * _pirdDesc, uint32_t _uiSectionIndex, const mx::CPeObject &_poPeObject ) {
		uint32_t uiTotal = _pirdDesc->NumberOfNamedEntries + _pirdDesc->NumberOfIdEntries;

		const MX_IMAGE_RESOURCE_DIRECTORY_ENTRY * pirdeEntry = reinterpret_cast<const MX_IMAGE_RESOURCE_DIRECTORY_ENTRY *>(_pirdDesc + 1);
		size_t sEndAddr = reinterpret_cast<size_t>(&_poPeObject.SectionData()[_uiSectionIndex].vData[0]) + _poPeObject.SectionData()[_uiSectionIndex].vData.size();
		for ( uint32_t I = 0; I < uiTotal; ++I, ++pirdeEntry ) {
			// Check bounds.
			if ( reinterpret_cast<size_t>(pirdeEntry + 1) > sEndAddr ) {
				// Trying to read past the section.  No way to continue.
				break;
			}
			AddResourceTree( _ptviParent, pirdeEntry, _uiSectionIndex, _poPeObject );
		}
	}

	// Decodes the resource tree.
	VOID CPeTreeModel::AddResourceTree( CTreeViewItem * _ptviParent, const MX_IMAGE_RESOURCE_DIRECTORY_ENTRY * _pirdeEntry, uint32_t _uiSectionIndex, const mx::CPeObject &_poPeObject ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];

		uint64_t uiOffset = reinterpret_cast<size_t>(_pirdeEntry) - reinterpret_cast<size_t>(&_poPeObject.SectionData()[_uiSectionIndex].vData[0]);
		uiOffset += _poPeObject.SectionData()[_uiSectionIndex].ui64FileOffset;

		CTreeViewItem * ptviTemp;
		{
			std::string sName;
			if ( _pirdeEntry->NameId & 0x80000000 ) {
				uiOffset = (_pirdeEntry->NameId & 0x7FFFFFFF) + _poPeObject.SectionData()[_uiSectionIndex].ui64FileOffset;
				_poPeObject.GetSizedWideString( (_pirdeEntry->NameId & 0x7FFFFFFF) + _poPeObject.SectionData()[_uiSectionIndex].ui64RelocAddress, sName );
			}
			else {
				uint16_t uiId = _pirdeEntry->NameId & 0xFFFF;
				if ( !CUtilities::ResourceTypeToString( uiId, sName ) ) {
					CUtilities::ToUnsigned( uiId, sName );
				}
				else {
					sName += " (";
					CUtilities::ToUnsigned( uiId, sName );
					sName += ')';
				}
			}
			// Decode name or ID.
			//std::sprintf( szBuffer, "%u", _irdDesc.Characteristics );
			CUtilities::BytesToHex( _pirdeEntry, sizeof( MX_IMAGE_RESOURCE_DIRECTORY_ENTRY ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( uiOffset, MX_IMAGE_RESOURCE_DIRECTORY_ENTRY );
			QList<QVariant> lValues;
			lValues << "NameId" << sName.c_str() << szOffset << "" << ((_pirdeEntry->NameId & 0x80000000) ? "Name." : "ID.");
			ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}

		if ( (_pirdeEntry->Data & 0x80000000) ) {
			// Points to an MX_IMAGE_RESOURCE_DIRECTORY.
			size_t sOffset = _pirdeEntry->Data & 0x7FFFFFFF;
			if ( sOffset + sizeof( MX_IMAGE_RESOURCE_DIRECTORY ) <= _poPeObject.SectionData()[_uiSectionIndex].vData.size() ) {
				const MX_IMAGE_RESOURCE_DIRECTORY * pirdResDir = reinterpret_cast<const MX_IMAGE_RESOURCE_DIRECTORY *>(&_poPeObject.SectionData()[_uiSectionIndex].vData[sOffset]);
				//AddResourceDesc( ptviTemp, (*pirdResDir), _poPeObject, sOffset + _poPeObject.SectionData()[_uiSectionIndex].ui64FileOffset );
				AddResourceTree( ptviTemp, pirdResDir, _uiSectionIndex, _poPeObject );
			}
		}
		else {
			size_t sOffset = _pirdeEntry->Data;
			if ( sOffset + sizeof( IMAGE_RESOURCE_DATA_ENTRY ) <= _poPeObject.SectionData()[_uiSectionIndex].vData.size() ) {
				const IMAGE_RESOURCE_DATA_ENTRY * pirdResDir = reinterpret_cast<const IMAGE_RESOURCE_DATA_ENTRY *>(&_poPeObject.SectionData()[_uiSectionIndex].vData[sOffset]);
				//AddResourceDesc( ptviTemp, (*pirdResDir), _poPeObject, sOffset + _poPeObject.SectionData()[_uiSectionIndex].ui64FileOffset );
				//AddResourceTree( ptviTemp, pirdResDir, _uiSectionIndex, _poPeObject );
				uiOffset = _poPeObject.SectionData()[_uiSectionIndex].ui64FileOffset + sOffset;
				{
					std::string sTemp;
					_poPeObject.RelocAddressToStringwithSection( pirdResDir->Data, sTemp );
					//std::sprintf( szBuffer, "%u", pirdResDir->Data );
					CUtilities::BytesToHex( _pirdeEntry, sizeof( pirdResDir->Data ), szBufferRaw );
					MX_PRINT_FILE_OFFSET( uiOffset, pirdResDir->Data );
					QList<QVariant> lValues;
					lValues << "Data" << sTemp.c_str() << szOffset << "" << "Data pointer.";
					CTreeViewItem * ptviTemp2 = new CTreeViewItem( lValues, ptviTemp );
					ptviTemp->AppendChild( ptviTemp2 );
				}
				{
					std::sprintf( szBuffer, "%u", pirdResDir->Size );
					CUtilities::BytesToHex( _pirdeEntry, sizeof( pirdResDir->Size ), szBufferRaw );
					MX_PRINT_FILE_OFFSET( uiOffset, pirdResDir->Size );
					QList<QVariant> lValues;
					lValues << "Size" << szBuffer << szOffset << "" << "Size of the resource.";
					CTreeViewItem * ptviTemp2 = new CTreeViewItem( lValues, ptviTemp );
					ptviTemp->AppendChild( ptviTemp2 );
				}
				{
					std::sprintf( szBuffer, "%u", pirdResDir->CodePage );
					CUtilities::BytesToHex( _pirdeEntry, sizeof( pirdResDir->CodePage ), szBufferRaw );
					MX_PRINT_FILE_OFFSET( uiOffset, pirdResDir->CodePage );
					QList<QVariant> lValues;
					lValues << "CodePage" << szBuffer << szOffset << "" << "Code page.";
					CTreeViewItem * ptviTemp2 = new CTreeViewItem( lValues, ptviTemp );
					ptviTemp->AppendChild( ptviTemp2 );
				}
				{
					std::sprintf( szBuffer, "%u", pirdResDir->Reserved );
					CUtilities::BytesToHex( _pirdeEntry, sizeof( pirdResDir->Reserved ), szBufferRaw );
					MX_PRINT_FILE_OFFSET( uiOffset, pirdResDir->Reserved );
					QList<QVariant> lValues;
					lValues << "Reserved" << szBuffer << szOffset << "" << "Reserved.";
					CTreeViewItem * ptviTemp2 = new CTreeViewItem( lValues, ptviTemp );
					ptviTemp->AppendChild( ptviTemp2 );
				}
			}
		}
	}

}	// namespace mx
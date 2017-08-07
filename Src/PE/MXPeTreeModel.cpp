#include "MXPeTreeModel.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <ctime>
#include <stdlib.h>


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
	BytesToHex( &PART, sizeof( PART ), szBuffer )

		{
			QList<QVariant> lValues;
			MX_PART_TO_STRING( _poPeObject.DosHeader() );
			lValues << "MS-DOS Header" << "" << szBuffer << "The MS-DOS header";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			AddDosHeader( ptviTemp, _poPeObject.DosHeader() );
		}
		{
			QList<QVariant> lValues;
			std::string sTemp;
			BytesToHex( &_poPeObject.DosStub()[0], sizeof( _poPeObject.WinHeader() ), sTemp );
			lValues << "DOS Stub" << "" << sTemp.c_str() << "The DOS stub";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			AddDosStub( ptviTemp, _poPeObject.DosStub() );
		}
		{
			QList<QVariant> lValues;
			MX_PART_TO_STRING( _poPeObject.CoffHeader() );
			lValues << "COFF Header" << "" << szBuffer << "The Portable Executable signature followed by the COFF header.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			AddCoffHeader( ptviTemp, _poPeObject.CoffHeader() );
		}
		{
			CTreeViewItem * ptviTemp = nullptr;
			if ( _poPeObject.Is64() ) {
				
				{
					QList<QVariant> lValues;
					CHAR szBuffer[(sizeof( _poPeObject.WinHeader() )+sizeof( _poPeObject.Win64Header() ))*2+1];
					BytesToHex( &_poPeObject.WinHeader(), sizeof( _poPeObject.WinHeader() ), szBuffer );
					BytesToHex( &_poPeObject.Win64Header(), sizeof( _poPeObject.Win64Header() ), &szBuffer[sizeof( _poPeObject.WinHeader() )] );
					lValues << "PE 64 Header" << "" << szBuffer << "The 64-bit PE header.";
					ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
					m_ptviRootItem->AppendChild( ptviTemp );
					AddOptHeader( ptviTemp, _poPeObject.WinHeader(), _poPeObject.ImageBase() );
					AddPe64Header( ptviTemp, _poPeObject.Win64Header() );
				}
			}
			else {
				{
					QList<QVariant> lValues;
					CHAR szBuffer[(sizeof( _poPeObject.WinHeader() )+sizeof( _poPeObject.Win32Header() ))*2+1];
					BytesToHex( &_poPeObject.WinHeader(), sizeof( _poPeObject.WinHeader() ), szBuffer );
					BytesToHex( &_poPeObject.Win32Header(), sizeof( _poPeObject.Win32Header() ), &szBuffer[sizeof( _poPeObject.WinHeader() )] );
					lValues << "PE 32 Header" << "" << szBuffer << "The 32-bit PE header.";
					ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
					m_ptviRootItem->AppendChild( ptviTemp );
					AddOptHeader( ptviTemp, _poPeObject.WinHeader(), _poPeObject.ImageBase() );
					AddPe32Header( ptviTemp, _poPeObject.Win32Header() );
				}
			}
			AddDataDirectories( ptviTemp, _poPeObject.DataDirectory(), _poPeObject.ImageBase() );
		}

		{
			QList<QVariant> lValues;
			lValues << "Section Headers" << "" << "" << "Section headers";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, m_ptviRootItem );
			m_ptviRootItem->AppendChild( ptviTemp );
			AddSectionHeaders( ptviTemp, _poPeObject.Sections(), _poPeObject.ImageBase() );
		}
		

		return TRUE;
	}

	// Decodes a DOS header to a given parent.
	VOID CPeTreeModel::AddDosHeader( CTreeViewItem * _ptviParent, const MX_DOS_HEADER &_dhHeader ) {
		CHAR szBuffer[MX_PE_DOS_PAD*12];
		CHAR szBufferRaw[MX_PE_DOS_PAD*12];
		{
			BytesToCString( &_dhHeader.Signature, sizeof( _dhHeader.Signature ), szBuffer );
			BytesToHex( &_dhHeader.Signature, sizeof( _dhHeader.Signature ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Signature" << szBuffer << szBufferRaw << "Signature MZ (Mark Zbikowski).";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.BytesInLastBlock );
			BytesToHex( &_dhHeader.BytesInLastBlock, sizeof( _dhHeader.BytesInLastBlock ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "BytesInLastBlock" << szBuffer << szBufferRaw << "The number of bytes in the last block of the program that are actually used. If this value is zero, then the entire last block is used (effectively 512).";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.BlocksInFile );
			BytesToHex( &_dhHeader.BlocksInFile, sizeof( _dhHeader.BlocksInFile ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "BlocksInFile" << szBuffer << szBufferRaw << "Number of blocks in the file that are part of the EXE file. If BytesInLastBlock is non-zero, only that much of the last block is used.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.NumRelocs );
			BytesToHex( &_dhHeader.NumRelocs, sizeof( _dhHeader.NumRelocs ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "NumRelocs" << szBuffer << szBufferRaw << "Number of relocation entries stored after the header. May be zero.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.HeaderParagraphs );
			BytesToHex( &_dhHeader.HeaderParagraphs, sizeof( _dhHeader.HeaderParagraphs ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "HeaderParagraphs" << szBuffer << szBufferRaw << "Number of paragraphs in the header.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.MinExtraParagraphs );
			BytesToHex( &_dhHeader.MinExtraParagraphs, sizeof( _dhHeader.MinExtraParagraphs ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MinExtraParagraphs" << szBuffer << szBufferRaw << "Number of paragraphs of additional memory that the program will need. This is the equivalent of the BSS size in a Unix program.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.MaxExtraParagraphs );
			BytesToHex( &_dhHeader.MaxExtraParagraphs, sizeof( _dhHeader.MaxExtraParagraphs ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MaxExtraParagraphs" << szBuffer << szBufferRaw << "Maximum number of paragraphs of additional memory.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.Ss );
			BytesToHex( &_dhHeader.Ss, sizeof( _dhHeader.Ss ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "StackSegment" << szBuffer << szBufferRaw << "Relative value of the stack segment. This value is added to the segment at which the program was loaded, and the result is used to initialize the SS register.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.Sp );
			BytesToHex( &_dhHeader.Sp, sizeof( _dhHeader.Sp ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SP" << szBuffer << szBufferRaw << "Initial value of the SP register.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.Checksum );
			BytesToHex( &_dhHeader.Checksum, sizeof( _dhHeader.Checksum ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Checksum" << szBuffer << szBufferRaw << "Word checksum. If set properly, the 16-bit sum of all words in the file should be zero. Usually this is not set.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.Ip );
			BytesToHex( &_dhHeader.Ip, sizeof( _dhHeader.Ip ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "IP" << szBuffer << szBufferRaw << "Initial value of the IP register.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.Cs );
			BytesToHex( &_dhHeader.Cs, sizeof( _dhHeader.Cs ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "CS" << szBuffer << szBufferRaw << "Initial value of the CS register, relative to the segment where the program was loaded.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.OverlayNumber );
			BytesToHex( &_dhHeader.OverlayNumber, sizeof( _dhHeader.OverlayNumber ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "OverlayNumber" << szBuffer << szBufferRaw << "Overlay number. A value of 0 implies this is the main program.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			BytesToCString( &_dhHeader.Padding, sizeof( _dhHeader.Padding ), szBuffer );
			BytesToHexWithSpaces( &_dhHeader.Padding, sizeof( _dhHeader.Padding ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Padding" << szBuffer << szBufferRaw << "Padding.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.PeOffset );
			BytesToHex( &_dhHeader.PeOffset, sizeof( _dhHeader.PeOffset ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "PeOffset" << szBuffer << szBufferRaw << "The offset from the start of the file to the relocation pointer table.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _dhHeader.OverlayNum );
			BytesToHex( &_dhHeader.OverlayNum, sizeof( _dhHeader.OverlayNum ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "OverlayNum" << szBuffer << szBufferRaw << "Overlays are sections of a program that remain on disk until the program actually requires them.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes a DOS stub to a given parent.
	VOID CPeTreeModel::AddDosStub( CTreeViewItem * _ptviParent, const std::vector<uint8_t> &_vStub ) {
		std::string sVal;
		std::string sRaw;
		{
			BytesToCString( &_vStub[0], _vStub.size(), sVal );
			BytesToHexWithSpaces( &_vStub[0], _vStub.size(), sRaw );
			QList<QVariant> lValues;
			lValues << "Stub" << sVal.c_str() << sRaw.c_str() << "DOS stub.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes a COFF header to a given parent.
	VOID CPeTreeModel::AddCoffHeader( CTreeViewItem * _ptviParent, const MX_COFF_HEADER &_chHeader ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];
		{
			BytesToCString( &_chHeader.PeSignature, sizeof( _chHeader.PeSignature ), szBuffer );
			BytesToHex( &_chHeader.PeSignature, sizeof( _chHeader.PeSignature ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "PeSignature" << szBuffer << szBufferRaw << "The PE header.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[64];
			MachineTypeToString( _chHeader.Machine, szTemp );
			std::sprintf( szBuffer, "%s (%u)", szTemp, _chHeader.Machine );
			BytesToHex( &_chHeader.Machine, sizeof( _chHeader.Machine ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Machine" << szBuffer << szBufferRaw << "Determines for what machine the file was compiled.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _chHeader.NumberOfSections );
			BytesToHex( &_chHeader.NumberOfSections, sizeof( _chHeader.NumberOfSections ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "NumberOfSections" << szBuffer << szBufferRaw << "Number of sections that are described at the end of the PE headers.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			time_t tRawTime = _chHeader.TimeDateStamp;
			tm * ptTimeInfo;
			CHAR szTemp[128];
			ptTimeInfo = std::localtime( &tRawTime );
			std::strftime( szTemp, sizeof( szTemp ),"%d-%m-%Y %I:%M:%S %Z", ptTimeInfo);

			std::sprintf( szBuffer, "%s (%u)", szTemp, _chHeader.TimeDateStamp );
			BytesToHex( &_chHeader.TimeDateStamp, sizeof( _chHeader.TimeDateStamp ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "TimeDateStamp" << szBuffer << szBufferRaw << "32-bit time at which this header was generated (DD-MM-YYYY HH-MM-SS).";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _chHeader.PointerToSymbolTable );
			BytesToHex( &_chHeader.PointerToSymbolTable, sizeof( _chHeader.PointerToSymbolTable ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "PointerToSymbolTable" << szBuffer << szBufferRaw << "The offset of the symbol table, in bytes, or zero if no COFF symbol table exists.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _chHeader.NumberOfSymbols );
			BytesToHex( &_chHeader.NumberOfSymbols, sizeof( _chHeader.NumberOfSymbols ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "NumberOfSymbols" << szBuffer << szBufferRaw << "The number of symbols in the symbol table.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _chHeader.SizeOfOptionalHeader );
			BytesToHex( &_chHeader.SizeOfOptionalHeader, sizeof( _chHeader.SizeOfOptionalHeader ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfOptionalHeader" << szBuffer << szBufferRaw << "The size of the optional header, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[48];
			std::sprintf( szTemp, " (%u, 0x%.4X)", _chHeader.Characteristics, _chHeader.Characteristics );
			std::string sVal;
			PeCharacteristicsToString( _chHeader.Characteristics, sVal );
			sVal.append( szTemp );
			BytesToHex( &_chHeader.Characteristics, sizeof( _chHeader.Characteristics ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Characteristics" << sVal.c_str() << szBufferRaw << "The characteristics of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes a PE header to a given parent.
	VOID CPeTreeModel::AddOptHeader( CTreeViewItem * _ptviParent, const MX_COFF_WINDOW_OPT &_cwoHeader, uint64_t _uiImageBase ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];
		{
			CHAR szTemp[64];
			PeMagicTypeToString( _cwoHeader.Magic, szTemp );
			std::sprintf( szBuffer, "%s (%u)", szTemp, _cwoHeader.Magic );
			BytesToHex( &_cwoHeader.Magic, sizeof( _cwoHeader.Magic ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Magic" << szBuffer << szBufferRaw << "The state of the image file.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwoHeader.MajorLinkerVersion );
			BytesToHex( &_cwoHeader.MajorLinkerVersion, sizeof( _cwoHeader.MajorLinkerVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MajorLinkerVersion" << szBuffer << szBufferRaw << "The major version number of the linker.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwoHeader.MinorLinkerVersion );
			BytesToHex( &_cwoHeader.MinorLinkerVersion, sizeof( _cwoHeader.MinorLinkerVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MinorLinkerVersion" << szBuffer << szBufferRaw << "The minor version number of the linker.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwoHeader.SizeOfCode );
			sTemp.append( szBuffer );
			SizeString( _cwoHeader.SizeOfCode, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwoHeader.SizeOfCode, sizeof( _cwoHeader.SizeOfCode ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfCode" << sTemp.c_str() << szBufferRaw << "The size of the code section, in bytes, or the sum of all such sections if there are multiple code sections.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwoHeader.SizeOfInitializedData );
			sTemp.append( szBuffer );
			SizeString( _cwoHeader.SizeOfInitializedData, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwoHeader.SizeOfInitializedData, sizeof( _cwoHeader.SizeOfInitializedData ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfInitializedData" << sTemp.c_str() << szBufferRaw << "The size of the initialized data section, in bytes, or the sum of all such sections if there are multiple initialized data sections.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwoHeader.SizeOfUninitializedData );
			sTemp.append( szBuffer );
			SizeString( _cwoHeader.SizeOfUninitializedData, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwoHeader.SizeOfUninitializedData, sizeof( _cwoHeader.SizeOfUninitializedData ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfUninitializedData" << sTemp.c_str() << szBufferRaw << "The size of the uninitialized data section, in bytes, or the sum of all such sections if there are multiple uninitialized data sections.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			if ( _cwoHeader.AddressOfEntryPoint ) {
				std::sprintf( szBuffer, "0x%.8X (0x%I64X after loading)", _cwoHeader.AddressOfEntryPoint, _cwoHeader.AddressOfEntryPoint + _uiImageBase );
			}
			else {
				std::sprintf( szBuffer, "0x%.8X", _cwoHeader.AddressOfEntryPoint );
			}
			BytesToHex( &_cwoHeader.AddressOfEntryPoint, sizeof( _cwoHeader.AddressOfEntryPoint ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "AddressOfEntryPoint" << szBuffer << szBufferRaw << "A pointer to the entry point function, relative to the image base address.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "0x%.8X", _cwoHeader.BaseOfCode );
			BytesToHex( &_cwoHeader.BaseOfCode, sizeof( _cwoHeader.BaseOfCode ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "BaseOfCode" << szBuffer << szBufferRaw << "A pointer to the beginning of the code section, relative to the image base.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes a PE 32 header to a given parent.
	VOID CPeTreeModel::AddPe32Header( CTreeViewItem * _ptviParent, const MX_COFF_WINDOWS_PE32 &_cwpHeader ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];
		{
			std::sprintf( szBuffer, "0x%.8X", _cwpHeader.BaseOfData );
			BytesToHex( &_cwpHeader.BaseOfData, sizeof( _cwpHeader.BaseOfData ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "BaseOfData" << szBuffer << szBufferRaw << "A pointer to the beginning of the data section, relative to the image base.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "0x%.8X", _cwpHeader.ImageBase );
			BytesToHex( &_cwpHeader.ImageBase, sizeof( _cwpHeader.ImageBase ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "ImageBase" << szBuffer << szBufferRaw << "The preferred address of the first byte of the image when it is loaded in memory.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.SectionAlignment );
			BytesToHex( &_cwpHeader.SectionAlignment, sizeof( _cwpHeader.SectionAlignment ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SectionAlignment" << szBuffer << szBufferRaw << "The alignment of sections loaded in memory, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.FileAlignment );
			BytesToHex( &_cwpHeader.FileAlignment, sizeof( _cwpHeader.FileAlignment ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "FileAlignment" << szBuffer << szBufferRaw << "The alignment of the raw data of sections in the image file, in bytes.";
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
			WindowsVersion( _cwpHeader.MajorOperatingSystemVersion, _cwpHeader.MinorOperatingSystemVersion, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.MajorOperatingSystemVersion, sizeof( _cwpHeader.MajorOperatingSystemVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MajorOperatingSystemVersion" << sTemp.c_str() << szBufferRaw << "The major version number of the required operating system.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorOperatingSystemVersion );
			BytesToHex( &_cwpHeader.MinorOperatingSystemVersion, sizeof( _cwpHeader.MinorOperatingSystemVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MinorOperatingSystemVersion" << szBuffer << szBufferRaw << "The minor version number of the required operating system.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MajorImageVersion );
			BytesToHex( &_cwpHeader.MajorImageVersion, sizeof( _cwpHeader.MajorImageVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MajorImageVersion" << szBuffer << szBufferRaw << "The major version number of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorImageVersion );
			BytesToHex( &_cwpHeader.MinorImageVersion, sizeof( _cwpHeader.MinorImageVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MinorImageVersion" << szBuffer << szBufferRaw << "The minor version number of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MajorSubsystemVersion );
			BytesToHex( &_cwpHeader.MajorSubsystemVersion, sizeof( _cwpHeader.MajorSubsystemVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MajorSubsystemVersion" << szBuffer << szBufferRaw << "The major version number of the subsystem.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorSubsystemVersion );
			BytesToHex( &_cwpHeader.MinorSubsystemVersion, sizeof( _cwpHeader.MinorSubsystemVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MinorSubsystemVersion" << szBuffer << szBufferRaw << "The minor version number of the subsystem.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.Win32VersionValue );
			BytesToHex( &_cwpHeader.Win32VersionValue, sizeof( _cwpHeader.Win32VersionValue ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Win32VersionValue" << szBuffer << szBufferRaw << "This member is reserved and must be 0.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfImage );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfImage, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfImage, sizeof( _cwpHeader.SizeOfImage ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfImage" << sTemp.c_str() << szBufferRaw << "The size of the image, in bytes, including all headers.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfHeaders );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfHeaders, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfHeaders, sizeof( _cwpHeader.SizeOfHeaders ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfHeaders" << sTemp.c_str() << szBufferRaw << "The combined size of the e_lfanew member of IMAGE_DOS_HEADER, 4-byte signature, size of IMAGE_FILE_HEADER, size of optional header, and the size of all section headers rounded to a multiple of the value specified in the FileAlignment member.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.CheckSum );
			BytesToHex( &_cwpHeader.CheckSum, sizeof( _cwpHeader.CheckSum ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "CheckSum" << szBuffer << szBufferRaw << "The image file checksum.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[64];
			PeSubSystemTypeToString( _cwpHeader.Subsystem, szTemp );
			std::sprintf( szBuffer, "%s (%u)", szTemp, _cwpHeader.Subsystem );
			BytesToHex( &_cwpHeader.Subsystem, sizeof( _cwpHeader.Subsystem ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Subsystem" << szBuffer << szBufferRaw << "The subsystem required to run this image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[48];
			std::sprintf( szTemp, " (%u, 0x%.4X)", _cwpHeader.DllCharacteristics, _cwpHeader.DllCharacteristics );
			std::string sVal;
			PeCharacteristicsToString( _cwpHeader.DllCharacteristics, sVal );
			sVal.append( szTemp );
			BytesToHex( &_cwpHeader.DllCharacteristics, sizeof( _cwpHeader.DllCharacteristics ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "DllCharacteristics" << sVal.c_str() << szBufferRaw << "The DLL characteristics of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfStackReserve );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfStackReserve, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfStackReserve, sizeof( _cwpHeader.SizeOfStackReserve ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfStackReserve" << sTemp.c_str() << szBufferRaw << "The number of bytes to reserve for the stack.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfStackCommit );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfStackCommit, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfStackCommit, sizeof( _cwpHeader.SizeOfStackCommit ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfStackCommit" << sTemp.c_str() << szBufferRaw << "The number of bytes to commit for the stack.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfHeapReserve );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfHeapReserve, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfHeapReserve, sizeof( _cwpHeader.SizeOfHeapReserve ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfHeapReserve" << sTemp.c_str() << szBufferRaw << "The number of bytes to reserve for the local heap.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfHeapCommit );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfHeapCommit, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfHeapCommit, sizeof( _cwpHeader.SizeOfHeapCommit ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfHeapCommit" << sTemp.c_str() << szBufferRaw << "The number of bytes to commit for the local heap.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.LoaderFlags );
			BytesToHex( &_cwpHeader.LoaderFlags, sizeof( _cwpHeader.LoaderFlags ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "LoaderFlags" << szBuffer << szBufferRaw << "Obsolete. If bit 0 is set, a breakpoint is invoked before starting the process.  If bit 1 is set, a debugger is invoked on the process after it is loaded.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.NumberOfRvaAndSizes );
			BytesToHex( &_cwpHeader.NumberOfRvaAndSizes, sizeof( _cwpHeader.NumberOfRvaAndSizes ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "NumberOfRvaAndSizes" << szBuffer << szBufferRaw << "The number of directory entries in the remainder of the optional header. Each entry describes a location and size.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes a PE 64 header to a given parent.
	VOID CPeTreeModel::AddPe64Header( CTreeViewItem * _ptviParent, const MX_COFF_WINDOWS_PE64 &_cwpHeader ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];
		{
			std::sprintf( szBuffer, "0x%.16I64X", _cwpHeader.ImageBase );
			BytesToHex( &_cwpHeader.ImageBase, sizeof( _cwpHeader.ImageBase ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "ImageBase" << szBuffer << szBufferRaw << "The preferred address of the first byte of the image when it is loaded in memory.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.SectionAlignment );
			BytesToHex( &_cwpHeader.SectionAlignment, sizeof( _cwpHeader.SectionAlignment ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SectionAlignment" << szBuffer << szBufferRaw << "The alignment of sections loaded in memory, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.FileAlignment );
			BytesToHex( &_cwpHeader.FileAlignment, sizeof( _cwpHeader.FileAlignment ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "FileAlignment" << szBuffer << szBufferRaw << "The alignment of the raw data of sections in the image file, in bytes.";
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
			WindowsVersion( _cwpHeader.MajorOperatingSystemVersion, _cwpHeader.MinorOperatingSystemVersion, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.MajorOperatingSystemVersion, sizeof( _cwpHeader.MajorOperatingSystemVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MajorOperatingSystemVersion" << sTemp.c_str() << szBufferRaw << "The major version number of the required operating system.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorOperatingSystemVersion );
			BytesToHex( &_cwpHeader.MinorOperatingSystemVersion, sizeof( _cwpHeader.MinorOperatingSystemVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MinorOperatingSystemVersion" << szBuffer << szBufferRaw << "The minor version number of the required operating system.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MajorImageVersion );
			BytesToHex( &_cwpHeader.MajorImageVersion, sizeof( _cwpHeader.MajorImageVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MajorImageVersion" << szBuffer << szBufferRaw << "The major version number of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorImageVersion );
			BytesToHex( &_cwpHeader.MinorImageVersion, sizeof( _cwpHeader.MinorImageVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MinorImageVersion" << szBuffer << szBufferRaw << "The minor version number of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MajorSubsystemVersion );
			BytesToHex( &_cwpHeader.MajorSubsystemVersion, sizeof( _cwpHeader.MajorSubsystemVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MajorSubsystemVersion" << szBuffer << szBufferRaw << "The major version number of the subsystem.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.MinorSubsystemVersion );
			BytesToHex( &_cwpHeader.MinorSubsystemVersion, sizeof( _cwpHeader.MinorSubsystemVersion ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "MinorSubsystemVersion" << szBuffer << szBufferRaw << "The minor version number of the subsystem.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.Win32VersionValue );
			BytesToHex( &_cwpHeader.Win32VersionValue, sizeof( _cwpHeader.Win32VersionValue ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Win32VersionValue" << szBuffer << szBufferRaw << "This member is reserved and must be 0.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfImage );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfImage, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfImage, sizeof( _cwpHeader.SizeOfImage ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfImage" << sTemp.c_str() << szBufferRaw << "The size of the image, in bytes, including all headers.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfHeaders );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfHeaders, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfHeaders, sizeof( _cwpHeader.SizeOfHeaders ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfHeaders" << sTemp.c_str() << szBufferRaw << "The combined size of the e_lfanew member of IMAGE_DOS_HEADER, 4-byte signature, size of IMAGE_FILE_HEADER, size of optional header, and the size of all section headers rounded to a multiple of the value specified in the FileAlignment member.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.CheckSum );
			BytesToHex( &_cwpHeader.CheckSum, sizeof( _cwpHeader.CheckSum ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "CheckSum" << szBuffer << szBufferRaw << "The image file checksum.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[64];
			PeSubSystemTypeToString( _cwpHeader.Subsystem, szTemp );
			std::sprintf( szBuffer, "%s (%u)", szTemp, _cwpHeader.Subsystem );
			BytesToHex( &_cwpHeader.Subsystem, sizeof( _cwpHeader.Subsystem ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Subsystem" << szBuffer << szBufferRaw << "The subsystem required to run this image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			CHAR szTemp[48];
			std::sprintf( szTemp, " (%u, 0x%.4X)", _cwpHeader.DllCharacteristics, _cwpHeader.DllCharacteristics );
			std::string sVal;
			PeCharacteristicsToString( _cwpHeader.DllCharacteristics, sVal );
			sVal.append( szTemp );
			BytesToHex( &_cwpHeader.DllCharacteristics, sizeof( _cwpHeader.DllCharacteristics ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "DllCharacteristics" << sVal.c_str() << szBufferRaw << "The DLL characteristics of the image.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfStackReserve );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfStackReserve, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfStackReserve, sizeof( _cwpHeader.SizeOfStackReserve ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfStackReserve" << sTemp.c_str() << szBufferRaw << "The number of bytes to reserve for the stack.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfStackCommit );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfStackCommit, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfStackCommit, sizeof( _cwpHeader.SizeOfStackCommit ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfStackCommit" << sTemp.c_str() << szBufferRaw << "The number of bytes to commit for the stack.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfHeapReserve );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfHeapReserve, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfHeapReserve, sizeof( _cwpHeader.SizeOfHeapReserve ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfHeapReserve" << sTemp.c_str() << szBufferRaw << "The number of bytes to reserve for the local heap.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::string sTemp;
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfHeapCommit );
			sTemp.append( szBuffer );
			SizeString( _cwpHeader.SizeOfHeapCommit, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_cwpHeader.SizeOfHeapCommit, sizeof( _cwpHeader.SizeOfHeapCommit ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfHeapCommit" << sTemp.c_str() << szBufferRaw << "The number of bytes to commit for the local heap.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.LoaderFlags );
			BytesToHex( &_cwpHeader.LoaderFlags, sizeof( _cwpHeader.LoaderFlags ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "LoaderFlags" << szBuffer << szBufferRaw << "Obsolete. If bit 0 is set, a breakpoint is invoked before starting the process.  If bit 1 is set, a debugger is invoked on the process after it is loaded.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _cwpHeader.NumberOfRvaAndSizes );
			BytesToHex( &_cwpHeader.NumberOfRvaAndSizes, sizeof( _cwpHeader.NumberOfRvaAndSizes ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "NumberOfRvaAndSizes" << szBuffer << szBufferRaw << "The number of directory entries in the remainder of the optional header. Each entry describes a location and size.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes an array of data directories.
	VOID CPeTreeModel::AddDataDirectories( CTreeViewItem * _ptviParent, const std::vector<MX_DATA_DIRECTORY> &_vDataDirs, uint64_t _uiImageBase ) {
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
			std::sprintf( szTemp, "VirtualAddress=0x%.8X, Size=%u", _vDataDirs[I].VirtualAddress, _vDataDirs[I].Size );
			QList<QVariant> lValues;
			lValues << szBuffer << szTemp << "" << szDescBuffer;
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
			if ( _vDataDirs[I].VirtualAddress ) {
				AddDataDirectory( ptviTemp, _vDataDirs[I], _uiImageBase );
			}
		}
	}

	// Decodes a data directory.
	VOID CPeTreeModel::AddDataDirectory( CTreeViewItem * _ptviParent, const MX_DATA_DIRECTORY &_ddDir, uint64_t _uiImageBase ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];

		{
			if ( !_ddDir.VirtualAddress ) {
				std::sprintf( szBuffer, "0x%.8X", _ddDir.VirtualAddress );
			}
			else {
				std::sprintf( szBuffer, "0x%.8X (0x%I64X after loading)", _ddDir.VirtualAddress, _ddDir.VirtualAddress + _uiImageBase );
			}
			BytesToHex( &_ddDir.VirtualAddress, sizeof( _ddDir.VirtualAddress ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "VirtualAddress" << szBuffer << szBufferRaw << "";
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
				SizeString( _ddDir.Size, sTemp );
				sTemp.push_back( ')' );
			}
			BytesToHex( &_ddDir.Size, sizeof( _ddDir.Size ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Size" << sTemp.c_str() << szBufferRaw << "";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

	// Decodes an array of MX_IMAGE_SECTION_HEADER objects.
	VOID CPeTreeModel::AddSectionHeaders( CTreeViewItem * _ptviParent, const std::vector<MX_IMAGE_SECTION_HEADER> &_vHeaders, uint64_t _uiImageBase ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];

		for ( size_t I = 0; I < _vHeaders.size(); ++I ) {
			CHAR szTemp[16];
			ZeroMemory( szTemp, sizeof( szTemp ) );
			std::memcpy( szTemp, _vHeaders[I].Name, 8 );
			QList<QVariant> lValues;
			lValues << szTemp << "" << "" << "";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
			AddSectionHeader( ptviTemp, _vHeaders[I], _uiImageBase );
		}
	}

	// Decodes an MX_IMAGE_SECTION_HEADER object.
	VOID CPeTreeModel::AddSectionHeader( CTreeViewItem * _ptviParent, const MX_IMAGE_SECTION_HEADER &_ishHeader, uint64_t _uiImageBase ) {
		CHAR szBuffer[128];
		CHAR szBufferRaw[64];

		{
			CHAR szTemp[16];
			ZeroMemory( szTemp, sizeof( szTemp ) );
			std::memcpy( szTemp, _ishHeader.Name, 8 );
			BytesToHex( &_ishHeader.Name, sizeof( _ishHeader.Name ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Name" << szTemp << szBufferRaw << "Name of the section.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "0x%.8X (%u)", _ishHeader.Misc.PhysicalAddress, _ishHeader.Misc.PhysicalAddress );
			BytesToHex( &_ishHeader.Misc.PhysicalAddress, sizeof( _ishHeader.Misc.PhysicalAddress ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "Misc.PhysicalAddress/VirtualSize" << szBuffer << szBufferRaw << "A physical address or the total size of the section when loaded into memory, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "0x%.8X (0x%I64X after loading)", _ishHeader.VirtualAddress, _ishHeader.VirtualAddress + _uiImageBase );
			BytesToHex( &_ishHeader.VirtualAddress, sizeof( _ishHeader.VirtualAddress ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "VirtualAddress" << szBuffer << szBufferRaw << "The address of the first byte of the section when loaded into memory, relative to the image base.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u (", _ishHeader.SizeOfRawData, _ishHeader.SizeOfRawData );
			std::string sTemp;
			sTemp.append( szBuffer );
			SizeString( _ishHeader.SizeOfRawData, sTemp );
			sTemp.push_back( ')' );
			BytesToHex( &_ishHeader.SizeOfRawData, sizeof( _ishHeader.SizeOfRawData ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "SizeOfRawData" << sTemp.c_str() << szBufferRaw << "The size of the initialized data on disk, in bytes.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "0x%.8X", _ishHeader.PointerToRawData );
			BytesToHex( &_ishHeader.PointerToRawData, sizeof( _ishHeader.PointerToRawData ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "PointerToRawData" << szBuffer << szBufferRaw << "A file pointer to the first page within the COFF file.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "0x%.8X", _ishHeader.PointerToRelocations );
			BytesToHex( &_ishHeader.PointerToRelocations, sizeof( _ishHeader.PointerToRelocations ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "PointerToRelocations" << szBuffer << szBufferRaw << "A file pointer to the beginning of the relocation entries for the section.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "0x%.8X", _ishHeader.PointerToLinenumbers );
			BytesToHex( &_ishHeader.PointerToLinenumbers, sizeof( _ishHeader.PointerToLinenumbers ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "PointerToLinenumbers" << szBuffer << szBufferRaw << "A file pointer to the beginning of the line-number entries for the section.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _ishHeader.NumberOfRelocations );
			BytesToHex( &_ishHeader.NumberOfRelocations, sizeof( _ishHeader.NumberOfRelocations ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "NumberOfRelocations" << szBuffer << szBufferRaw << "The number of relocation entries for the section.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
		{
			std::sprintf( szBuffer, "%u", _ishHeader.NumberOfLinenumbers );
			BytesToHex( &_ishHeader.NumberOfLinenumbers, sizeof( _ishHeader.NumberOfLinenumbers ), szBufferRaw );
			QList<QVariant> lValues;
			lValues << "NumberOfLinenumbers" << szBuffer << szBufferRaw << "The number of line-number entries for the section.";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
		}
	}

}	// namespace mx
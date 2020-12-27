#pragma once

#include "../Files/MXFile.h"
#include <LSWWin.h>
#include "MXPeStructs.h"
#include <vector>

namespace mx {

	class CPeObject {
	public :
		CPeObject();
		~CPeObject();

		// == Enumerations.
		enum : uint32_t {
			MX_ERROR						= 0xFFFFFFFF,
		};


		// == Types.
		// A section of data in the file, including to/from relocation offsets.
		struct MX_SECTION_DATA {
			uint64_t						ui64FileOffset;				// Address in the file where this data lives.
			uint64_t						ui64RelocAddress;			// Address (from 0x00000000) where this data has been moved.  This address + ImageBase() = address if loaded to RAM.
			std::vector<uint8_t>			vData;						// The raw data in this section.
		};

		// An export.
		struct MX_EXPORT {
			uint16_t						uiOrdinal;					// Ordinal.
			std::string						sName;						// Name of the function.
			uint64_t						uiAddress;					// Address of the export.
			uint64_t						ui64FileOffsetOrd;			// File address of the ordinal.
			uint64_t						ui64FileOffsetName;			// File address of the name.
			uint64_t						ui64FileOffsetAddr;			// File address of the address.

		};

		// An import.
		struct MX_IMPORT {
			uint16_t						uiOrdinal;					// Ordinal.
			std::string						sName;						// Name of the function.
			uint64_t						ui64FileOffset;				// File offset where this import was found.
			BOOL							bBound;						// Bound to a static address?
		};

		// A base relocation.
		struct MX_BASE_RELOC {
			MX_IMAGE_BASE_RELOCATION *		pibrReloc;					// Pointer to the start of the set of relocations.
			uint32_t						uiTotal;					// Total relocations.
			uint16_t *						puiOffsets;					// Pointer to an array of relocations.
			uint64_t						ui64FileOffset;				// File offset where this relocation was found.
		};

		// An extracted resource with context.
		struct MX_EXTRACTED_RESOURCE {
			std::vector<MX_IMAGE_RESOURCE_DIRECTORY_ENTRY>
											vDirEntries;				// The whole directory tree leading up to this resource.
			MX_IMAGE_RESOURCE_DATA_ENTRY	irdeDataEntry;				// The data for this resource.
			std::vector<uint8_t>			vResourceData;				// The actual resource bytes.
		};


		// == Functions.
		BOOL								LoadImageFromFile( const wchar_t * _pwcPath );
		BOOL								LoadImageFromMemory( CFile &_fFile );

		BOOL								Is64() const { return m_woWinOpt.Magic == MX_IMAGE_NT_OPTIONAL_HDR64_MAGIC; }
		uint32_t							NumberOfRvaAndSizes() const { return static_cast<uint32_t>(m_vDataDirectories.size()); }
		uint64_t							ImageBase() const { return m_uiImageBase; }
		uint64_t							FileOffsetToRelocAddress( uint64_t _uiFileAddr ) const;	// File offset to a file offset that accounts for relocations of sections.  Add ImageBase() to this to get the RAM address.
		uint64_t							RelocAddressToFileOffset( uint64_t _uiRelocAddr ) const;
		uint64_t							NameOfSectionByRelocAddress( uint64_t _uiRelocAddr, std::string &_sReturn ) const; // Returns the offset into the section.
		BOOL								HasImportDesc() const { return m_vDataDirectories.size() > MX_IMAGE_DIRECTORY_ENTRY_IMPORT && m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress && m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_IMPORT].Size; }
		BOOL								HasExportDesc() const { return m_vDataDirectories.size() > MX_IMAGE_DIRECTORY_ENTRY_EXPORT && m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress && m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_EXPORT].Size; }
		BOOL								HasResourceDesc() const { return m_vDataDirectories.size() > MX_IMAGE_DIRECTORY_ENTRY_RESOURCE && m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress && m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_RESOURCE].Size; }
		BOOL								HasRelocDesc() const { return m_vDataDirectories.size() > MX_IMAGE_DIRECTORY_ENTRY_BASERELOC && m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress && m_vDataDirectories[MX_IMAGE_DIRECTORY_ENTRY_BASERELOC].Size; }
		VOID								ImportDllNameByIndex( uint32_t _ui32Index, std::string &_sReturn ) const;
		VOID								GetExportDllName( std::string &_sReturn ) const;
		VOID								GetString( uint64_t _uiRelocAddr, std::string &_sReturn ) const;
		VOID								GetSizedWideString( uint64_t _uiRelocAddr, std::string &_sReturn ) const;
		const MX_DOS_HEADER &				DosHeader() const { return m_dhDosHeader; }
		const std::vector<uint8_t> &		DosStub() const { return m_vDosStub; }
		const MX_COFF_HEADER &				CoffHeader() const { return m_chHeader; }
		const MX_COFF_WINDOW_OPT &			WinHeader() const { return m_woWinOpt; }
		const MX_COFF_WINDOWS_PE32 &		Win32Header() const { return m_cwpWin32; }
		const MX_COFF_WINDOWS_PE64 &		Win64Header() const { return m_cwpWin64; }
		const std::vector<MX_DATA_DIRECTORY> &
											DataDirectory() const { return m_vDataDirectories; }
		const std::vector<MX_IMAGE_SECTION_HEADER> &
											Sections() const { return m_vSections; }
		const std::vector<MX_SECTION_DATA> &SectionData() const { return m_vSectionData; }
		const MX_IMAGE_EXPORT_DIRECTORY *	ExportDescriptor() const { return m_piedExportDesc; }
		const std::vector<MX_IMAGE_IMPORT_DESCRIPTOR *> &
											ImportDescriptor() const { return m_vImportDesc; }
		const MX_IMAGE_RESOURCE_DIRECTORY *	ResourceDescriptor() const { return m_pirdResourceDir; }
		const std::vector<CPeObject::MX_BASE_RELOC> &
											BaseRelocs() const { return m_vBaseRelocations; }
		uint64_t							ReadRelocBytes( uint64_t _uiAddr, VOID * _pvDst, uint64_t _uiTotal ) const; // Expects _uiAddr to be translated from a file offset via FileOffsetToRelocAddress();
		uint32_t							RelocAddrToRelocIndexAndOffset( uint64_t _uiAddr, uint32_t &_uiOffset ) const; // Expects _uiAddr to be translated from a file offset via FileOffsetToRelocAddress();

		VOID								GetExports( std::vector<CPeObject::MX_EXPORT> &_vReturn ) const;
		VOID								GetImportsFromImportDescByIndex( uint32_t _ui32Index, std::vector<CPeObject::MX_IMPORT> &_vReturn ) const;
		
		// Returns the address without modification or 0.
		uint64_t							GetExportAddress( const char * _pcExport ) const;

		uint32_t							DosHeaderOffset() const { return m_uiDosHeaderOffset; }
		uint32_t							DosStubOffset() const { return m_uiDosStubOffset; }
		uint32_t							CoffHeaderOffset() const { return m_uiCoffHeaderOffset; }
		uint32_t							WinHeaderOffset() const { return m_uiWinOptHeaderOffset; }
		uint32_t							WinXHeaderOffset() const { return m_uiWin6432HeaderOffset; }
		uint32_t							DataDirsOffset() const { return m_uiDataDirsOffset; }
		uint32_t							SectionsOffset() const { return m_uiSectionsOffset; }
		uint32_t							ExportDescriptorOffset() const { return m_uiExportOffset; }
		uint32_t							ImportDescriptorOffset() const { return m_uiImportOffset; }
		uint32_t							ResourceDescriptorOffset() const { return m_uiResourceOffset; }

		VOID								RelocAddressToStringwithSection( uint64_t _uiRelocAddr, std::string &_sReturn ) const;

		// _uiAddr is given as a RAM address, and this determines if the given address falls within the memory this file would use if properly loaded to memory and expanded starting from its image base.
		BOOL								AddressIsInLoadedFile( uint64_t _uiAddr ) const;
		// _uiAddr is a file offset.
		BOOL								RelocAddressIsInFile( uint64_t _uiAddr ) const;

		const std::wstring &				GetPath() const { return m_wsPath; }
		VOID								SetPath( const wchar_t * _pwcPath ) { m_wsPath = _pwcPath ? _pwcPath : L""; }	// _pwcPath can be NULL.

		// Fills a vector with a resource.
		bool								GetResource( const MX_IMAGE_RESOURCE_DATA_ENTRY * _irdeResource, std::vector<uint8_t> &_vResult ) const;

		// Gathers all resources.
		bool								GetAllResources( std::vector<MX_EXTRACTED_RESOURCE> &_vResult ) const;

	protected :
		// == Members.
		MX_DOS_HEADER						m_dhDosHeader;
		std::vector<uint8_t>				m_vDosStub;
		MX_COFF_HEADER						m_chHeader;
		MX_COFF_WINDOW_OPT					m_woWinOpt;
		MX_COFF_WINDOWS_PE32				m_cwpWin32;
		MX_COFF_WINDOWS_PE64				m_cwpWin64;
		std::vector<MX_DATA_DIRECTORY>		m_vDataDirectories;
		std::vector<MX_IMAGE_SECTION_HEADER>m_vSections;
		std::vector<MX_SECTION_DATA>		m_vSectionData;
		MX_IMAGE_EXPORT_DIRECTORY *			m_piedExportDesc;
		std::vector<MX_IMAGE_IMPORT_DESCRIPTOR *>
											m_vImportDesc;
		MX_IMAGE_RESOURCE_DIRECTORY *		m_pirdResourceDir;
		std::vector<MX_BASE_RELOC>			m_vBaseRelocations;

		uint64_t							m_uiImageBase;

		uint32_t							m_uiDosHeaderOffset;
		uint32_t							m_uiDosStubOffset;
		uint32_t							m_uiCoffHeaderOffset;
		uint32_t							m_uiWinOptHeaderOffset;
		uint32_t							m_uiWin6432HeaderOffset;
		uint32_t							m_uiDataDirsOffset;
		uint32_t							m_uiSectionsOffset;
		uint32_t							m_uiExportOffset;
		uint32_t							m_uiImportOffset;
		uint32_t							m_uiResourceOffset;
		uint32_t							m_uiRelocOffset;

		std::wstring						m_wsPath;


		// == Functions.
		// Recursively advances down a directory while gathering resources.
		bool								GetAllResources( std::vector<MX_EXTRACTED_RESOURCE> &_vResult,
			const MX_IMAGE_RESOURCE_DIRECTORY * _pirdDesc, uint32_t _uiSectionIndex,
			std::vector<MX_IMAGE_RESOURCE_DIRECTORY_ENTRY> &_vDirStack ) const;

		// Recursively advances down a directory while gathering resources.
		bool								GetAllResources( std::vector<MX_EXTRACTED_RESOURCE> &_vResult,
			const MX_IMAGE_RESOURCE_DIRECTORY_ENTRY * _pirdeEntry, uint32_t _uiSectionIndex,
			std::vector<MX_IMAGE_RESOURCE_DIRECTORY_ENTRY> &_vDirStack ) const;

	};

}	// namespace mx

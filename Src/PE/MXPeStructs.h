#pragma once

#include <cstdint>

namespace mx {

	enum MX_PE_SIZES {
		MX_PE_DOS_PAD									= 0x20,
		MX_IMAGE_NUMBEROF_DIRECTORY_ENTRIES				= 16,
		MX_IMAGE_SIZEOF_SHORT_NAME						= 8,
	};

	enum MX_MACHINE_TYPES {
		MX_IMAGE_FILE_MACHINE_UNKNOWN					= 0x0,
		MX_IMAGE_FILE_MACHINE_AM33						= 0x13,
		MX_IMAGE_FILE_MACHINE_AMD64						= 0x8664,
		MX_IMAGE_FILE_MACHINE_ARM						= 0x1C0,
		MX_IMAGE_FILE_MACHINE_ARMNT						= 0x1C4,
		MX_IMAGE_FILE_MACHINE_ARM64						= 0xAA64,
		MX_IMAGE_FILE_MACHINE_EBC						= 0xEBC,
		MX_IMAGE_FILE_MACHINE_I386						= 0x14C,
		MX_IMAGE_FILE_MACHINE_IA64						= 0x200,
		MX_IMAGE_FILE_MACHINE_M32R						= 0x9041,
		MX_IMAGE_FILE_MACHINE_MIPS16					= 0x266,
		MX_IMAGE_FILE_MACHINE_MIPSFPU					= 0x366,
		MX_IMAGE_FILE_MACHINE_MIPSFPU16					= 0x466,
		MX_IMAGE_FILE_MACHINE_POWERPC					= 0x1F0,
		MX_IMAGE_FILE_MACHINE_POWERPCFP					= 0x1F1,
		MX_IMAGE_FILE_MACHINE_R4000						= 0x166,
		MX_IMAGE_FILE_MACHINE_SH3						= 0x1A2,
		MX_IMAGE_FILE_MACHINE_SH3DSP					= 0x1A3,
		MX_IMAGE_FILE_MACHINE_SH4						= 0x1A6,
		MX_IMAGE_FILE_MACHINE_SH5						= 0x1A8,
		MX_IMAGE_FILE_MACHINE_THUMB						= 0x1C2,
		MX_IMAGE_FILE_MACHINE_WCEMIPSV2					= 0x169,
		MX_IMAGE_FILE_MACHINE_R3000						= 0x162,
		MX_IMAGE_FILE_MACHINE_R10000					= 0x168,
		MX_IMAGE_FILE_MACHINE_ALPHA						= 0x184,
		MX_IMAGE_FILE_MACHINE_SH3E						= 0x1a4,
		MX_IMAGE_FILE_MACHINE_ALPHA64					= 0x284,
		MX_IMAGE_FILE_MACHINE_AXP64						= MX_IMAGE_FILE_MACHINE_ALPHA64,
		MX_IMAGE_FILE_MACHINE_TRICORE					= 0x520,
		MX_IMAGE_FILE_MACHINE_CEF						= 0xCEF,
		MX_IMAGE_FILE_MACHINE_CEE						= 0xC0EE,
	};

	enum MX_PE_CHARACTERISTICS {
		MX_IMAGE_FILE_RELOCS_STRIPPED					= 0x0001,
		MX_IMAGE_FILE_EXECUTABLE_IMAGE					= 0x0002,
		MX_IMAGE_FILE_LINE_NUMS_STRIPPED				= 0x0004,
		MX_IMAGE_FILE_LOCAL_SYMS_STRIPPED				= 0x0008,
		MX_IMAGE_FILE_AGGRESIVE_WS_TRIM					= 0x0010,
		MX_IMAGE_FILE_LARGE_ADDRESS_AWARE				= 0x0020,
		MX_IMAGE_FILE_BYTES_REVERSED_LO					= 0x0080,
		MX_IMAGE_FILE_32BIT_MACHINE						= 0x0100,
		MX_IMAGE_FILE_DEBUG_STRIPPED					= 0x0200,
		MX_IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP			= 0x0400,
		MX_IMAGE_FILE_NET_RUN_FROM_SWAP					= 0x0800,
		MX_IMAGE_FILE_SYSTEM							= 0x1000,
		MX_IMAGE_FILE_DLL								= 0x2000,
		MX_IMAGE_FILE_UP_SYSTEM_ONLY					= 0x4000,
		MX_IMAGE_FILE_BYTES_REVERSED_HI					= 0x8000,
	};

	enum MX_PE_WINDOWS_SUBSYSTEM {
		MX_IMAGE_SUBSYSTEM_UNKNOWN						= 0,
		MX_IMAGE_SUBSYSTEM_NATIVE						= 1,
		MX_IMAGE_SUBSYSTEM_WINDOWS_GUI					= 2,
		MX_IMAGE_SUBSYSTEM_WINDOWS_CUI					= 3,
		MX_IMAGE_SUBSYSTEM_OS2_CUI						= 5,
		MX_IMAGE_SUBSYSTEM_POSIX_CUI					= 7,
		MX_IMAGE_SUBSYSTEM_NATIVE_WINDOWS				= 8,
		MX_IMAGE_SUBSYSTEM_WINDOWS_CE_GUI				= 9,
		MX_IMAGE_SUBSYSTEM_EFI_APPLICATION				= 10,
		MX_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER		= 11,
		MX_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER			= 12,
		MX_IMAGE_SUBSYSTEM_EFI_ROM						= 13,
		MX_IMAGE_SUBSYSTEM_XBOX							= 14,
		MX_IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION		= 16,
	};

	enum MX_PE_DLL_CHARACTERISTICS {
		MX_IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA		= 0x0020,
		MX_IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE		= 0x0040,
		MX_IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY		= 0x0080,
		MX_IMAGE_DLLCHARACTERISTICS_NX_COMPAT			= 0x0100,
		MX_IMAGE_DLLCHARACTERISTICS_NO_ISOLATION		= 0x0200,
		MX_IMAGE_DLLCHARACTERISTICS_NO_SEH				= 0x0400,
		MX_IMAGE_DLLCHARACTERISTICS_NO_BIND				= 0x0800,
		MX_IMAGE_DLLCHARACTERISTICS_APPCONTAINER		= 0x1000,
		MX_IMAGE_DLLCHARACTERISTICS_WDM_DRIVER			= 0x2000,
		MX_IMAGE_DLLCHARACTERISTICS_GUARD_CF			= 0x4000,
		MX_IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE	= 0x8000
	};

	enum MX_PE_MAGIC_FLAGS {
		MX_IMAGE_NT_OPTIONAL_HDR32_MAGIC				= 0x10B,
		MX_IMAGE_NT_OPTIONAL_HDR64_MAGIC				= 0x20B,
		MX_IMAGE_ROM_OPTIONAL_HDR_MAGIC					= 0x107,
	};

	enum MX_IMAGE_DIRECTORY_ENTRY {
		MX_IMAGE_DIRECTORY_ENTRY_EXPORT					= 0,
		MX_IMAGE_DIRECTORY_ENTRY_IMPORT					= 1,
		MX_IMAGE_DIRECTORY_ENTRY_RESOURCE				= 2,
		MX_IMAGE_DIRECTORY_ENTRY_EXCEPTION				= 3,
		MX_IMAGE_DIRECTORY_ENTRY_SECURITY				= 4,
		MX_IMAGE_DIRECTORY_ENTRY_BASERELOC				= 5,
		MX_IMAGE_DIRECTORY_ENTRY_DEBUG					= 6,
		MX_IMAGE_DIRECTORY_ENTRY_COPYRIGHT				= 7,
		MX_IMAGE_DIRECTORY_ENTRY_ARCHITECTURE			= 7,
		MX_IMAGE_DIRECTORY_ENTRY_GLOBALPTR				= 8,
		MX_IMAGE_DIRECTORY_ENTRY_TLS					= 9,
		MX_IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG			= 10,
		MX_IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT			= 11,
		MX_IMAGE_DIRECTORY_ENTRY_IAT					= 12,
		MX_IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT			= 13,
		MX_IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR			= 14,
	};

	enum MX_PE_SECTION_FLAGS : uint32_t {
		MX_IMAGE_SCN_TYPE_REG							= 0x00000000,
		MX_IMAGE_SCN_TYPE_DSECT							= 0x00000001,
		MX_IMAGE_SCN_TYPE_NOLOAD						= 0x00000002,
		MX_IMAGE_SCN_TYPE_GROUP							= 0x00000004,
		MX_IMAGE_SCN_TYPE_NO_PAD						= 0x00000008,
		MX_IMAGE_SCN_TYPE_COPY							= 0x00000010,

		MX_IMAGE_SCN_CNT_CODE							= 0x00000020,
		MX_IMAGE_SCN_CNT_INITIALIZED_DATA				= 0x00000040,
		MX_IMAGE_SCN_CNT_UNINITIALIZED_DATA				= 0x00000080,

		MX_IMAGE_SCN_LNK_OTHER							= 0x00000100,
		MX_IMAGE_SCN_LNK_INFO							= 0x00000200,
		MX_IMAGE_SCN_TYPE_OVER							= 0x00000400,
		MX_IMAGE_SCN_LNK_REMOVE							= 0x00000800,
		MX_IMAGE_SCN_LNK_COMDAT							= 0x00001000,

		MX_IMAGE_SCN_MEM_PROTECTED						= 0x00004000,
		MX_IMAGE_SCN_NO_DEFER_SPEC_EXC					= 0x00004000,
		MX_IMAGE_SCN_GPREL								= 0x00008000,
		MX_IMAGE_SCN_MEM_FARDATA						= 0x00008000,
		MX_IMAGE_SCN_MEM_SYSHEAP						= 0x00010000,
		MX_IMAGE_SCN_MEM_PURGEABLE						= 0x00020000,
		MX_IMAGE_SCN_MEM_16BIT							= 0x00020000,
		MX_IMAGE_SCN_MEM_LOCKED							= 0x00040000,
		MX_IMAGE_SCN_MEM_PRELOAD						= 0x00080000,

		MX_IMAGE_SCN_ALIGN_1BYTES						= 0x00100000,
		MX_IMAGE_SCN_ALIGN_2BYTES						= 0x00200000,
		MX_IMAGE_SCN_ALIGN_4BYTES						= 0x00300000,
		MX_IMAGE_SCN_ALIGN_8BYTES						= 0x00400000,
		MX_IMAGE_SCN_ALIGN_16BYTES						= 0x00500000,
		MX_IMAGE_SCN_ALIGN_32BYTES						= 0x00600000,
		MX_IMAGE_SCN_ALIGN_64BYTES						= 0x00700000,
		MX_IMAGE_SCN_ALIGN_128BYTES						= 0x00800000,
		MX_IMAGE_SCN_ALIGN_256BYTES						= 0x00900000,
		MX_IMAGE_SCN_ALIGN_512BYTES						= 0x00A00000,
		MX_IMAGE_SCN_ALIGN_1024BYTES					= 0x00B00000,
		MX_IMAGE_SCN_ALIGN_2048BYTES					= 0x00C00000,
		MX_IMAGE_SCN_ALIGN_4096BYTES					= 0x00D00000,
		MX_IMAGE_SCN_ALIGN_8192BYTES					= 0x00E00000,

		MX_IMAGE_SCN_ALIGN_MASK							= 0x00F00000,

		MX_IMAGE_SCN_LNK_NRELOC_OVFL					= 0x01000000,
		MX_IMAGE_SCN_MEM_DISCARDABLE					= 0x02000000,
		MX_IMAGE_SCN_MEM_NOT_CACHED						= 0x04000000,
		MX_IMAGE_SCN_MEM_NOT_PAGED						= 0x08000000,
		MX_IMAGE_SCN_MEM_SHARED							= 0x10000000,
		MX_IMAGE_SCN_MEM_EXECUTE						= 0x20000000,
		MX_IMAGE_SCN_MEM_READ							= 0x40000000,
		MX_IMAGE_SCN_MEM_WRITE							= 0x80000000,
	};

	struct MX_DOS_HEADER {
		uint16_t									Signature;
		uint16_t									BytesInLastBlock;
		uint16_t									BlocksInFile;
		uint16_t									NumRelocs;
		uint16_t									HeaderParagraphs;
		uint16_t									MinExtraParagraphs;
		uint16_t									MaxExtraParagraphs;
		uint16_t									Ss;
		uint16_t									Sp;
		uint16_t									Checksum;
		uint16_t									Ip;
		uint16_t									Cs;
		uint16_t									RelocTableOffset;
		uint16_t									OverlayNumber;
		uint8_t										Padding[MX_PE_DOS_PAD];
		uint16_t									PeOffset;
		uint16_t									OverlayNum;
	};

	struct MX_COFF_HEADER {
		char										PeSignature[4];
		uint16_t									Machine;
		uint16_t									NumberOfSections;
		uint32_t									TimeDateStamp;
		uint32_t									PointerToSymbolTable;
		uint32_t									NumberOfSymbols;
		uint16_t									SizeOfOptionalHeader;
		uint16_t									Characteristics;
	};

	struct MX_COFF_WINDOW_OPT {
		uint16_t									Magic;
		uint8_t										MajorLinkerVersion;
		uint8_t										MinorLinkerVersion;
		uint32_t									SizeOfCode;
		uint32_t									SizeOfInitializedData;
		uint32_t									SizeOfUninitializedData;
		uint32_t									AddressOfEntryPoint;
		uint32_t									BaseOfCode;
	};
	
	struct MX_COFF_WINDOWS_PE32 {
		uint32_t									BaseOfData;
		uint32_t									ImageBase;
		uint32_t									SectionAlignment;
		uint32_t									FileAlignment;
		uint16_t									MajorOperatingSystemVersion;
		uint16_t									MinorOperatingSystemVersion;
		uint16_t									MajorImageVersion;
		uint16_t									MinorImageVersion;
		uint16_t									MajorSubsystemVersion;
		uint16_t									MinorSubsystemVersion;
		uint32_t									Win32VersionValue;
		uint32_t									SizeOfImage;
		uint32_t									SizeOfHeaders;
		uint32_t									CheckSum;
		uint16_t									Subsystem;
		uint16_t									DllCharacteristics;
		uint32_t									SizeOfStackReserve;
		uint32_t									SizeOfStackCommit;
		uint32_t									SizeOfHeapReserve;
		uint32_t									SizeOfHeapCommit;
		uint32_t									LoaderFlags;
		uint32_t									NumberOfRvaAndSizes;
	};

	struct MX_COFF_WINDOWS_PE64 {
		uint64_t									ImageBase;
		uint32_t									SectionAlignment;
		uint32_t									FileAlignment;
		uint16_t									MajorOperatingSystemVersion;
		uint16_t									MinorOperatingSystemVersion;
		uint16_t									MajorImageVersion;
		uint16_t									MinorImageVersion;
		uint16_t									MajorSubsystemVersion;
		uint16_t									MinorSubsystemVersion;
		uint32_t									Win32VersionValue;
		uint32_t									SizeOfImage;
		uint32_t									SizeOfHeaders;
		uint32_t									CheckSum;
		uint16_t									Subsystem;
		uint16_t									DllCharacteristics;
		uint64_t									SizeOfStackReserve;
		uint64_t									SizeOfStackCommit;
		uint64_t									SizeOfHeapReserve;
		uint64_t									SizeOfHeapCommit;
		uint32_t									LoaderFlags;
		uint32_t									NumberOfRvaAndSizes;
	};

	struct MX_DATA_DIRECTORY {
		uint32_t									VirtualAddress;
		uint32_t									Size;
	};

	struct MX_IMAGE_SECTION_HEADER {
		uint8_t Name[MX_IMAGE_SIZEOF_SHORT_NAME];
		union {
			uint32_t PhysicalAddress;
			uint32_t VirtualSize;
		} Misc;
		uint32_t VirtualAddress;
		uint32_t SizeOfRawData;
		uint32_t PointerToRawData;
		uint32_t PointerToRelocations;
		uint32_t PointerToLinenumbers;
		uint16_t NumberOfRelocations;
		uint16_t NumberOfLinenumbers;
		uint32_t Characteristics;
	};

	struct MX_IMAGE_IMPORT_DESCRIPTOR {
		union {
			uint32_t Characteristics;
			uint32_t OriginalFirstThunk;
		};
		uint32_t TimeDateStamp;
		uint32_t ForwarderChain;
		uint32_t Name;
		uint32_t FirstThunk;
	};

	struct MX_IMAGE_IMPORT_BY_NAME {
		uint16_t Hint;
		char Name[1];	// NULL terminated, variable length.
	};

	struct MX_IMAGE_EXPORT_DIRECTORY {
        uint32_t Characteristics;
        uint32_t TimeDateStamp;
        uint16_t MajorVersion;
        uint16_t MinorVersion;
        uint32_t Name;
        uint32_t Base;
        uint32_t NumberOfFunctions;
        uint32_t NumberOfNames;
        uint32_t AddressOfFunctions;
        uint32_t AddressOfNames;
        uint32_t AddressOfNameOrdinals;
    };

	struct MX_IMAGE_RESOURCE_DIRECTORY {
		uint32_t Characteristics;
		uint32_t TimeDateStamp;
		uint16_t MajorVersion;
		uint16_t MinorVersion;
		uint16_t NumberOfNamedEntries;
		uint16_t NumberOfIdEntries;
	};

	struct MX_IMAGE_RESOURCE_DIRECTORY_ENTRY {
		uint32_t NameId;
		uint32_t Data;
	};

	struct IMAGE_RESOURCE_DATA_ENTRY {
		uint32_t Data;
		uint32_t Size;
		uint32_t CodePage;
		uint32_t Reserved;
	};


}	// namespace mx

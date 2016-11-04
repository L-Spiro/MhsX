#pragma once

#include <cstdint>

namespace mx {

	enum MX_PE_SIZES {
		MX_PE_DOS_PAD								= 0x20,
	};

	enum MX_MACHINE_TYPES {
		IMAGE_FILE_MACHINE_UNKNOWN					= 0x0,
		IMAGE_FILE_MACHINE_AM33						= 0x13,
		IMAGE_FILE_MACHINE_AMD64					= 0x8664,
		IMAGE_FILE_MACHINE_ARM						= 0x1C0,
		IMAGE_FILE_MACHINE_ARMNT					= 0x1C4,
		IMAGE_FILE_MACHINE_ARM64					= 0xAA64,
		IMAGE_FILE_MACHINE_EBC						= 0xEBC,
		IMAGE_FILE_MACHINE_I386						= 0x14C,
		IMAGE_FILE_MACHINE_IA64						= 0x200,
		IMAGE_FILE_MACHINE_M32R						= 0x9041,
		IMAGE_FILE_MACHINE_MIPS16					= 0x266,
		IMAGE_FILE_MACHINE_MIPSFPU					= 0x366,
		IMAGE_FILE_MACHINE_MIPSFPU16				= 0x466,
		IMAGE_FILE_MACHINE_POWERPC					= 0x1F0,
		IMAGE_FILE_MACHINE_POWERPCFP				= 0x1F1,
		IMAGE_FILE_MACHINE_R4000					= 0x166,
		IMAGE_FILE_MACHINE_SH3						= 0x1A2,
		IMAGE_FILE_MACHINE_SH3DSP					= 0x1A3,
		IMAGE_FILE_MACHINE_SH4						= 0x1A6,
		IMAGE_FILE_MACHINE_SH5						= 0x1A8,
		IMAGE_FILE_MACHINE_THUMB					= 0x1C2,
		IMAGE_FILE_MACHINE_WCEMIPSV2				= 0x169,
	};

	enum MX_PE_CHARACTERISTICS {
		IMAGE_FILE_RELOCS_STRIPPED					= 0x0001,
		IMAGE_FILE_EXECUTABLE_IMAGE					= 0x0002,
		IMAGE_FILE_LINE_NUMS_STRIPPED				= 0x0004,
		IMAGE_FILE_LOCAL_SYMS_STRIPPED				= 0x0008,
		IMAGE_FILE_AGGRESSIVE_WS_TRIM				= 0x0010,
		IMAGE_FILE_LARGE_ADDRESS_AWARE				= 0x0020,
		IMAGE_FILE_BYTES_REVERSED_LO				= 0x0080,
		IMAGE_FILE_32BIT_MACHINE					= 0x0100,
		IMAGE_FILE_DEBUG_STRIPPED					= 0x0200,
		IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP			= 0x0400,
		IMAGE_FILE_NET_RUN_FROM_SWAP				= 0x0800,
		IMAGE_FILE_SYSTEM							= 0x1000,
		IMAGE_FILE_DLL								= 0x2000,
		IMAGE_FILE_UP_SYSTEM_ONLY					= 0x4000,
		IMAGE_FILE_BYTES_REVERSED_HI				= 0x8000,
	};

	enum MX_PE_WINDOWS_SUBSYSTEM {
		IMAGE_SUBSYSTEM_UNKNOWN						= 0,
		IMAGE_SUBSYSTEM_NATIVE						= 1,
		IMAGE_SUBSYSTEM_WINDOWS_GUI					= 2,
		IMAGE_SUBSYSTEM_WINDOWS_CUI					= 3,
		IMAGE_SUBSYSTEM_OS2_CUI						= 5,
		IMAGE_SUBSYSTEM_POSIX_CUI					= 7,
		IMAGE_SUBSYSTEM_NATIVE_WINDOWS				= 8,
		IMAGE_SUBSYSTEM_WINDOWS_CE_GUI				= 9,
		IMAGE_SUBSYSTEM_EFI_APPLICATION				= 10,
		IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER		= 11,
		IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER			= 12,
		IMAGE_SUBSYSTEM_EFI_ROM						= 13,
		IMAGE_SUBSYSTEM_XBOX						= 14,
		IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION	= 16,
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
		uint16_t									Pad;
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

}	// namespace mx

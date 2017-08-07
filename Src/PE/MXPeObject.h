#pragma once

#include "MXPeStructs.h"
#include <vector>
#include <Windows.h>

namespace mx {

	class CPeObject {
	public :
		CPeObject();
		~CPeObject();

		// == Enumerations.
		enum {
			MX_
		};


		// == Functions.
		BOOL								LoadImageFromFile( const wchar_t * _pwcPath );
		BOOL								Is64() const { return m_woWinOpt.Magic == MX_IMAGE_NT_OPTIONAL_HDR64_MAGIC; }
		uint32_t							NumberOfRvaAndSizes() const { return static_cast<uint32_t>(m_vDataDirectories.size()); }
		uint32_t							ImageBase() const { return m_uiImageBase; }
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
		uint32_t							m_uiImageBase;

	};

}	// namespace mx

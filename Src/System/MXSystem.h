#pragma once

#include <Windows.h>

// == Macros.
#define COUNT_OF(x)						((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

namespace mx {

	class CSystem {
	public :
		// == Functions.
		// Initialize the system.
		static VOID						InitSystem();

		// Is this WoW64?
		static BOOL						IsWow64Process() { return m_bIsWow; }

		// System information.
		static const SYSTEM_INFO &		GetSystemInfo() { return m_siSystemInfo; }


	protected :
		// == Members.
		// Is the WoW64?
		static BOOL						m_bIsWow;

		// System information.
		static SYSTEM_INFO				m_siSystemInfo;
	};

}	// namespace mx

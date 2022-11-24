#pragma once

#include "../MXMhsX.h"
#include <string>

namespace mx {

	struct MX_PROGRAM {
		// The program menu name.
		std::wstring						wsMenuName;
		// The program path.
		std::wstring						wsPath;
		// The arguments.
		std::wstring						wsArgs;
		// The working directory.
		std::wstring						wsWorkingDir;
		// The Show command.
		INT									iShowCmd = SW_SHOWNORMAL;
		// Is visible.
		bool								bVisible = true;
	};

}	// namespace mx

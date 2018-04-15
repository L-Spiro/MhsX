#pragma once

#include "../MXMhsX.h"

namespace mx {

	enum MX_OPEN_PROC {
		MX_OP_SHOW_PATH					= (1 << 0),
		MX_OP_SHOW_WINDOWS				= (1 << 1),
		MX_OP_SHOW_CHILDWINDOWS			= (1 << 2),
		MX_OP_SHOW_PARENT				= (1 << 3),
		MX_OP_SHOW_X86					= (1 << 4),
		MX_OP_SHOW_ALL					= MX_OP_SHOW_PATH | MX_OP_SHOW_WINDOWS | MX_OP_SHOW_CHILDWINDOWS | MX_OP_SHOW_PARENT | MX_OP_SHOW_X86,
	};

	struct MX_OPTIONS {
		// General.
		DWORD							dwFoundAddressRefresh;			// 10
		DWORD							dwMainRefresh;					// 10
		DWORD							dwLockedRefresh;				// 1000
		DWORD							dwExpressionRefresh;			// 100
		BOOL							bDataTypesAsCodeNames;			// TRUE
		BOOL							bDataTypeSizes;					// TRUE
		BOOL							bDataTypeRanges;				// TRUE

		// Open-process settings.
		DWORD							dwOpenProc;						// MX_OP_SHOW_ALL

		// Search settings.
		BOOL							bMemImage;						// TRUE
		BOOL							bMemPrivate;					// TRUE
		BOOL							bMemMapped;						// FALSE
		INT								iThreadPriority;				// THREAD_PRIORITY_NORMAL
		DWORD							dwBufferSize;					// 5 * 1024 * 1024
		BOOL							bPreCache;						// TRUE
		BOOL							bPostFlush;						// TRUE
		BOOL							bPauseTarget;					// FALSE
		BOOL							bUseEpsilon;					// TRUE
		BOOL							bSmartEpsilon;					// TRUE
		DOUBLE							dEpsilon;						// FLT_EPSILON (1.19209290E-07F)
		BOOL							bAligned;						// TRUE
		BOOL							bSameAsOriginal;				// TRUE
	};

}	// namespace mx

#pragma once

#include "../MXMhsX.h"

namespace mx {

	struct MX_OPTIONS {
		// General.
		DWORD							dwFoundAddressRefresh;			// 10
		DWORD							dwMainRefresh;					// 10
		DWORD							dwLockedRefresh;				// 1000
		DWORD							dwExpressionRefresh;			// 100
		BOOL							bDataTypesAsCodeNames;			// TRUE
		BOOL							bDataTypeSizes;					// TRUE
		BOOL							bDataTypeRanges;				// TRUE

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
	};

}	// namespace mx

#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXSecureWString.h"
#include "../Utilities/MXUtilities.h"
#include <vector>

namespace mx {

	enum MX_OPEN_PROC {
		MX_OP_SHOW_PATH					= (1 << 0),
		MX_OP_SHOW_WINDOWS				= (1 << 1),
		MX_OP_SHOW_CHILDWINDOWS			= (1 << 2),
		MX_OP_SHOW_PARENT				= (1 << 3),
		MX_OP_SHOW_X86					= (1 << 4),
		MX_OP_SHOW_ALL					= MX_OP_SHOW_PATH | MX_OP_SHOW_WINDOWS | MX_OP_SHOW_CHILDWINDOWS | MX_OP_SHOW_PARENT | MX_OP_SHOW_X86,

		MX_OP_TITLE_BAR_PROC_NAME		= (1 << 5),
		MX_OP_TITLE_BAR_PROC_ID			= (1 << 6),
		MX_OP_TITLE_BAR_X86_FLAG		= (1 << 7),
		MX_OP_TITLE_BAR_RANDOM			= (1 << 8),
		MX_OP_TITLE_BAR_ALL				= (MX_OP_TITLE_BAR_PROC_NAME | MX_OP_TITLE_BAR_PROC_ID | MX_OP_TITLE_BAR_X86_FLAG | MX_OP_TITLE_BAR_RANDOM),
	};

	struct MX_OPTIONS {
		class CMemHack *				pmhMemHackObj;
		// General.
		DWORD							dwFoundAddressRefresh			= 10;
		DWORD							dwMainRefresh					= 10;
		DWORD							dwLockedRefresh					= 1000;
		DWORD							dwExpressionRefresh				= 100;
		BOOL							bDataTypesAsCodeNames			= TRUE;
		BOOL							bDataTypeSizes					= TRUE;
		BOOL							bDataTypeRanges					= TRUE;
		BOOL							bUse0x							= TRUE;
		BOOL							bUse0o							= FALSE;
		BOOL							bShortEnums						= TRUE;

		// Open-process settings.
		DWORD							dwOpenProc						= (MX_OP_SHOW_ALL | MX_OP_TITLE_BAR_ALL) & ~MX_OP_TITLE_BAR_RANDOM;

		// Search settings.
		BOOL							bMemImage						= TRUE;
		BOOL							bMemPrivate						= TRUE;
		BOOL							bMemMapped						= TRUE;
		INT								iThreadPriority					= THREAD_PRIORITY_NORMAL;
		UINT64							ui64BufferSize					= 4 * 1024 * 1024;
		BOOL							bPreCache						= TRUE;
		BOOL							bPostFlush						= TRUE;
		BOOL							bPauseTarget					= FALSE;
		BOOL							bUseEpsilon						= TRUE;
		BOOL							bSmartEpsilon					= TRUE;
		DOUBLE							dEpsilon						= EE_FLOAT16_EPSILON;			// (0.0009765625)
		UINT32							ui32Alignment					= alignof( uint32_t );
		BOOL							bAligned						= TRUE;
		BOOL							bSameAsOriginal					= TRUE;

		BOOL							bMatchCase						= FALSE;
		BOOL							bWholeWord						= FALSE;
		BOOL							bIsHex							= FALSE;
		BOOL							bResolveEscapes					= TRUE;
		BOOL							bWildcard						= FALSE;
		BOOL							bLingIgnoreCase					= FALSE;
		BOOL							bLingIgnoreDiacritic			= FALSE;
		BOOL							bIgnoreKana						= FALSE;
		BOOL							bIgnoreNoSpace					= FALSE;
		BOOL							bIgnoreSymbols					= FALSE;
		BOOL							bIgnoreWidth					= FALSE;

		BOOL							bRegexSingleLine				= FALSE;
		BOOL							bRegexMultiLine					= FALSE;
		BOOL							bRegexExtended					= TRUE;
		BOOL							bRegexFindLongest				= FALSE;
		BOOL							bRegexNegateSingleLine			= TRUE;

		UINT32							uiRegexFlavor					= CUtilities::MX_RF_ONIGURUMA;
		UINT32							bsByteswap						= CUtilities::MX_BS_NONE;

		CSecureWString					wsFromText;
		CSecureWString					wsToText;
		std::vector<CSecureWString>		vFromHistory;
		std::vector<CSecureWString>		vToHistory;

		

		CSecureWString					wsLastFoundAddressSaveFile;
		CSecureWString					wsLastFoundAddressSaveDirectory;

		std::vector<CSecureWString>		vEditDescriptionHistory;
		std::vector<CSecureWString>		vEditCurValHistory;
		std::vector<CSecureWString>		vEditArrayLengthHistory;
		std::vector<CSecureWString>		vEditArrayStrideHistory;
		std::vector<CSecureWString>		vEditLockedLeftHistory;
		std::vector<CSecureWString>		vEditLockedRightHistory;

		std::vector<CSecureWString>		vAddValDescriptionHistory;
		std::vector<CSecureWString>		vAddValAddrHistory;
	};

	struct MX_FLOAT_STUDIO_OPTIONS {
		int								iFloatStudioRoundMode			= FE_TONEAREST;
		CSecureWString					wsFormatText;
		std::vector<CSecureWString>		vFloatStudioFormatHistory;
	};

}	// namespace mx

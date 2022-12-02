#pragma once

#include "../MXMhsX.h"

namespace mx {

	// Opcode groups.
	enum MX_OPCODE_GROUPS : uint64_t {
		MX_OG_ARITH			= (1ULL << 0ULL),
		MX_OG_BINARY		= (1ULL << 1ULL),
		MX_OG_BIT			= (1ULL << 2ULL),
		MX_OG_BRANCH		= (1ULL << 3ULL),
		MX_OG_BREAK			= (1ULL << 4ULL),
		MX_OG_CACHECT		= (1ULL << 5ULL),
		MX_OG_COMPAR		= (1ULL << 6ULL),
		MX_OG_COND			= (1ULL << 7ULL),
		MX_OG_CONTROL		= (1ULL << 8ULL),
		MX_OG_CONVER		= (1ULL << 9ULL),
		MX_OG_DATAMOV		= (1ULL << 10ULL),
		MX_OG_DECIMAL		= (1ULL << 11ULL),
		MX_OG_FETCH			= (1ULL << 12ULL),
		MX_OG_FLGCTRL		= (1ULL << 13ULL),
		MX_OG_GEN			= (1ULL << 14ULL),
		MX_OG_INOUT			= (1ULL << 15ULL),
		MX_OG_LDCONST		= (1ULL << 16ULL),
		MX_OG_LOGICAL		= (1ULL << 17ULL),
		MX_OG_MXCSRSM		= (1ULL << 18ULL),
		MX_OG_OBSOL			= (1ULL << 19ULL),
		MX_OG_ORDER			= (1ULL << 20ULL),
		MX_OG_PCKSCLR		= (1ULL << 21ULL),
		MX_OG_PCKSP			= (1ULL << 22ULL),
		MX_OG_PREFIX		= (1ULL << 23ULL),
		MX_OG_SEGREG		= (1ULL << 24ULL),
		MX_OG_SHFTROT		= (1ULL << 25ULL),
		MX_OG_SHIFT			= (1ULL << 26ULL),
		MX_OG_SHUNPCK		= (1ULL << 27ULL),
		MX_OG_SIMDFP		= (1ULL << 28ULL),
		MX_OG_SIMDINT		= (1ULL << 29ULL),
		MX_OG_SM			= (1ULL << 30ULL),
		MX_OG_STACK			= (1ULL << 31ULL),
		MX_OG_STRING		= (1ULL << 32ULL),
		MX_OG_STRTXT		= (1ULL << 33ULL),
		MX_OG_SYNC			= (1ULL << 34ULL),
		MX_OG_SYSTEM		= (1ULL << 35ULL),
		MX_OG_TRANS			= (1ULL << 36ULL),
		MX_OG_UNPACK		= (1ULL << 37ULL),
		MX_OG_X87FPU		= (1ULL << 38ULL),
		
	};

	// Instruction extensions.
	enum MX_INSTR_EXT {
		MX_IE_NONE,
		MX_IE_MMX,
		MX_IE_SMX,
		MX_IE_SSE1,
		MX_IE_SSE2,
		MX_IE_SSE3,
		MX_IE_SSE41,
		MX_IE_SSE42,
		MX_IE_SSSE3,
		MX_IE_VMX,
	};

	// Addressing modes.
	enum MX_ADDRESS_MODES {
		MX_AM_NONE,
		MX_AM_JMF,
		MX_AM_BAX,
		MX_AM_BBX,
		MX_AM_BDX,
		MX_AM_CRX,
		MX_AM_DRX,
		MX_AM_MRG,
		MX_AM_MF4,
		MX_AM_RST,
		MX_AM_FLG,
		MX_AM_REG,
		MX_AM_RR4,
		MX_AM_IMM,
		MX_AM_JOW,
		MX_AM_MRM,
		MX_AM_MR8,
		MX_AM_IMA,
		MX_AM_RMX,
		MX_AM_MRQ,
		MX_AM_MRGE,
		MX_AM_SGM,
		MX_AM_SGM2,
		MX_AM_SGM30,
		MX_AM_SGM33,
		MX_AM_STK,
		MX_AM_TRX,
		MX_AM_RMXE,
		MX_AM_RRXE,
		MX_AM_MXMM,
		MX_AM_MSI,
		MX_AM_MDI,
		MX_AM_RCM,
	};

}	// namespace mx

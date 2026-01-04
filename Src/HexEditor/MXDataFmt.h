#pragma once


namespace mx {

	/** Data display for the left and right columns. */
	enum MX_DATA_FMT {
		MX_DF_HEX,								/**< "00 FF 2A" */
		MX_DF_DEC,								/**< "000 255 042" */
		MX_DF_OCT,								/**< "000 377 052" */
		MX_DF_BIN,								/**< "00000000" */
		MX_DF_CHAR,								/**< Textual: printable byte or '.' */
		MX_DF_UINT8,							/**< "  0 255  42" */
		MX_DF_INT8,								/**< "  0  -1  42" */
		MX_DF_UINT16,							/**< "    0 65535    42" */
		MX_DF_INT16,							/**< "    0    -1    42" */
		MX_DF_UINT32,							/**< "         0 4294967295         42" */
		MX_DF_INT32,							/**< "         0         -1         42" */
		MX_DF_UINT64,							/**< "                   0 18446744073709551615                   42" */
		MX_DF_INT64,							/**< "                   0                   -1                   42" */
		MX_DF_POINTER16,						/**< "FFFF" */
		MX_DF_POINTER32,						/**< "FFFFFFFF" */
		MX_DF_POINTER64,						/**< "FFFFFFFFFFFFFFFF" */
		MX_DF_FLOAT10,							/**< "0 3.125 -42" */
		MX_DF_FLOAT11,							/**< "0 3.125 -42" */
		MX_DF_BFLOAT16,							/**< "0 3.141 -42" */
		MX_DF_FLOAT16,							/**< "0 3.1406 -42" */
		MX_DF_FLOAT32,							/**< "0 3.1415925 -42" */
		MX_DF_FLOAT64,							/**< "0 3.1415926535897931 -42" */

		MX_DF_PCM8,								/**< "0 -1 0.1647" */
		MX_DF_PCM16,							/**< "0 -1 0.1647" */
		MX_DF_PCM24,							/**< "0 -1 0.1647" */
		MX_DF_PCM32,							/**< "0 -1 0.1647" */
	};
	
}	// namespace mx

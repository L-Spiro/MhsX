#pragma once

#include "MXCharSets.h"
#include "MXDataFmt.h"


namespace mx {

	// == Enumerations.
	enum MX_COPY_AS {
		MX_CA_BINARY,											// Raw bytes.
		MX_CA_HEX_TEXT,											// FF FE 4C 00 53 00 4E 00 5F 00 45 00 5F 00 45 00
		MX_CA_DECIMAL_TEXT,										// 255 254 76  0   83  0   78  0   95  0   69  0   95  0   69  0  
		MX_CA_BINARY_TEXT,										// 11111111 11111110 01001100 00000000 01010011 00000000 01001110 00000000 01011111 00000000 01000101 00000000 01011111 00000000 01000101 00000000
		MX_CA_C_CODE,											// unsigned char hexData[106350] = {
																// 	0xFF, 0xFE, 0x4C, 0x00, 0x53, 0x00, 0x4E, 0x00, 0x5F, 0x00, 0x45, 0x00, 0x5F, 0x00, 0x45, 0x00,
		MX_CA_JAVA_CODE,										// byte hexData[] = {
																//    (byte)0xFF, (byte)0xFE, (byte)0x4C, (byte)0x00, (byte)0x53, (byte)0x00, (byte)0x4E, (byte)0x00, (byte)0x5F, (byte)0x00, (byte)0x45, (byte)0x00, (byte)0x5F, (byte)0x00, (byte)0x45, (byte)0x00,
		MX_CA_PYTHON_CODE,										// buffer = b''.join([
																//     b'\xFF\xFE\x4C\x00\x53\x00\x4E\x00\x5F\x00\x45\x00\x5F\x00\x45\x00',
		MX_CA_INTEL_8,											// :10000000FFFE4C0053004E005F0045005F004500BE
		MX_CA_INTEL_16,											// :020000020000FC
																// :10000000FFFE4C0053004E005F0045005F004500BE
		MX_CA_INTEL_32,											// :020000040000FA
																// :10000000FFFE4C0053004E005F0045005F004500BE
		MX_CA_MOTOROLA_S19,										// S00600004844521B
																// S1130000FFFE4C0053004E005F0045005F004500BA
		MX_CA_MOTOROLA_S28,										// S00600004844521B
																// S214000000FFFE4C0053004E005F0045005F004500B9
		MX_CA_MOTOROLA_S37,										// S00600004844521B
																// S31500000000FFFE4C0053004E005F0045005F004500B8
		MX_CA_HEX_EDITOR_AREAS,									// 0:0000  FF FE 4C 00 53 00 4E 00 5F 00 45 00 5F 00 45 00   LSN_E_E 
		MX_CA_WEB_PAGE,											// Version:1.0
																// StartHTML:0000000105
																// EndHTML:0000000290
																// StartFragment:0000000121
																// EndFragment:0000000270
																// <html>
																// <body>
																// <pre>
																// 0:0000  FF FE 4C 00 53 00 4E 00 5F 00 45 00 5F 00 45 00   LSN_E_E 
																// 0:0010  4E 00 55 00 4D 00 28 00 20 00 4C 00 53 00 4E 00  NUM( LSN 
																// </pre>
																// </body>
																// </html>
		MX_CA_RICH_TEXT,										// {\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fmodern\fprq1\fcharset0 Courier New;}}
																// \viewkind4\uc1\pard\f0\fs20 0:0000  FF FE 4C 00 53 00 4E 00 5F 00 45 00 5F 00 45 00   LSN_E_E \par
																// 0:0010  4E 00 55 00 4D 00 28 00 20 00 4C 00 53 00 4E 00  NUM( LSN \par
		MX_CA_BASE64,											// //5MAFMATgBfAEUAXwBFAE4AVQBNACgAIABMAFMATgA=
		MX_CA_UUENCODING,										// begin 644 LSNErrorEnum UTF16 LE.inl
																// @__Y,`%,`3@!?`$4`7P!%`$X`50!-`"@`(`!,`%,`3@``
																// `
																// end

		MX_CA_UINT8,											// 255 254 76 0 83 0 78 0 95 0 69 0 95 0 69 0
		MX_CA_INT8,												// -1 -2 76 0 83 0 78 0 95 0 69 0 95 0 69 0
		MX_CA_UINT16,											// …
		MX_CA_INT16,
		MX_CA_UINT32,
		MX_CA_INT32,
		MX_CA_UINT64,
		MX_CA_INT64,
		MX_CA_P16,
		MX_CA_P32,
		MX_CA_P64,

		MX_CA_FLOAT10,
		MX_CA_FLOAT11,
		MX_CA_FLOAT16,
		MX_CA_FLOAT32,
		MX_CA_FLOAT64,
		MX_CA_BFLOAT,

		MX_CA_PCM8,
		MX_CA_PCM16,
		MX_CA_PCM24,
		MX_CA_PCM32,

	};
	

	// == Types.
	struct MX_COPYAS {
		MX_COPY_AS				caCopyFmt		= MX_CA_BINARY;
		MX_DATA_FMT				dfLeft			= MX_DF_HEX;
		MX_DATA_FMT				dfRight			= MX_DF_CHAR;
		bool					bBigEndian		= false;
		CCharSets::MX_CHAR_SETS	csCharSet		= CCharSets::MX_CS_ASCII;
	};
}	// namespace mx

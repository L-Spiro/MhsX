#include "MXCodePages.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <cstdint>

namespace mx {

	// == Members.
	// The internal list of code pages.
	const CCodePages::MX_CODE_PAGES CCodePages::m_cpPages[] {
		MX_IBM037,
		MX_IBM437,
		MX_IBM500,
		MX_ASMO_708,
		MX_DOS_720,
		MX_ibm737,
		MX_ibm775,
		MX_ibm850,
		MX_ibm852,
		MX_IBM855,
		MX_ibm857,
		MX_IBM00858,
		MX_IBM860,
		MX_ibm861,
		MX_DOS_862,
		MX_IBM863,
		MX_IBM864,
		MX_IBM865,
		MX_cp866,
		MX_ibm869,
		MX_IBM870,
		MX_windows_874,
		MX_cp875,
		MX_shift_jis,
		MX_gb2312,
		MX_ks_c_5601_1987,
		MX_big5,
		MX_IBM1026,
		MX_IBM01047,
		MX_IBM01140,
		MX_IBM01141,
		MX_IBM01142,
		MX_IBM01143,
		MX_IBM01144,
		MX_IBM01145,
		MX_IBM01146,
		MX_IBM01147,
		MX_IBM01148,
		MX_IBM01149,
		MX_utf_16,
		MX_utf_16BE,
		MX_windows_1250,
		MX_windows_1251,
		MX_Windows_1252,
		MX_windows_1253,
		MX_windows_1254,
		MX_windows_1255,
		MX_windows_1256,
		MX_windows_1257,
		MX_windows_1258,
		MX_Johab,
		MX_macintosh,
		MX_x_mac_japanese,
		MX_x_mac_chinesetrad,
		MX_x_mac_korean,
		MX_x_mac_arabic,
		MX_x_mac_hebrew,
		MX_x_mac_greek,
		MX_x_mac_cyrillic,
		MX_x_mac_chinesesimp,
		MX_x_mac_romanian,
		MX_x_mac_ukrainian,
		MX_x_mac_thai,
		MX_x_mac_ce,
		MX_x_mac_icelandic,
		MX_x_mac_turkish,
		MX_x_mac_croatian,
		MX_utf_32,
		MX_utf_32BE,
		MX_x_Chinese_CNS,
		MX_x_cp20001,
		MX_x_Chinese_Eten,
		MX_x_cp20003,
		MX_x_cp20004,
		MX_x_cp20005,
		MX_x_IA5,
		MX_x_IA5_German,
		MX_x_IA5_Swedish,
		MX_x_IA5_Norwegian,
		MX_us_ascii,
		MX_x_cp20261,
		MX_x_cp20269,
		MX_IBM273,
		MX_IBM277,
		MX_IBM278,
		MX_IBM280,
		MX_IBM284,
		MX_IBM285,
		MX_IBM290,
		MX_IBM297,
		MX_IBM420,
		MX_IBM423,
		MX_IBM424,
		MX_x_EBCDIC_KoreanExtended,
		MX_IBM_Thai,
		MX_koi8_r,
		MX_IBM871,
		MX_IBM880,
		MX_IBM905,
		MX_IBM00924,
		MX_EUC_JP,
		MX_x_cp20936,
		MX_x_cp20949,
		MX_cp1025,
		MX_koi8_u,
		MX_iso_8859_1,
		MX_iso_8859_2,
		MX_iso_8859_3,
		MX_iso_8859_4,
		MX_iso_8859_5,
		MX_iso_8859_6,
		MX_iso_8859_7,
		MX_iso_8859_8,
		MX_iso_8859_9,
		MX_iso_8859_13,
		MX_iso_8859_15,
		MX_x_Europa,
		MX_iso_8859_8_i,
		MX_iso_2022_jp,
		MX_csISO2022JP,
		MX_iso_2022_jp_,
		MX_iso_2022_kr,
		MX_x_cp50227,
		MX_euc_jp,
		MX_EUC_CN,
		MX_euc_kr,
		MX_hz_gb_2312,
		MX_GB18030,
		MX_x_iscii_de,
		MX_x_iscii_be,
		MX_x_iscii_ta,
		MX_x_iscii_te,
		MX_x_iscii_as,
		MX_x_iscii_or,
		MX_x_iscii_ka,
		MX_x_iscii_ma,
		MX_x_iscii_gu,
		MX_x_iscii_pa,
		MX_utf_7,
		MX_utf_8,
	};

	// The critical section.
	lsw::CCriticalSection CCodePages::m_csCrit;

	// The set of code pages.
	std::set<UINT> CCodePages::m_sSystemCodePages;

	// == Functions.
	// Decodes a code page into a text value and description.  _pcValue and _pcDesc should be _T_MAX_LEN characters long.
	BOOL CCodePages::CodePageToString( uint32_t _uiCode, CHAR * _pcValue, CHAR * _pcDesc ) {
		// 2 tables because that is just easier to do with the tools.  Tables generated with MhsXStrings.exe.
		// TODO: Add unnamed codes from https://msdn.microsoft.com/en-us/library/windows/desktop/dd317756(v=vs.85).aspx
		struct {
			MX_CODE_PAGES cpCode;
			const CHAR * pcName;
			uint32_t ui32NameLen;
		}
		static const aTable[] = {
			{ MX_IBM037, _T_BA655461_IBM037, _LEN_BA655461 }, // 0
			{ MX_IBM437, _T_BD6CFCBD_IBM437, _LEN_BD6CFCBD }, // 1
			{ MX_IBM500, _T_09E750EA_IBM500, _LEN_09E750EA }, // 2
			{ MX_ASMO_708, _T_7F753281_ASMO_708, _LEN_7F753281 }, // 3
			{ MX_DOS_720, _T_1440639D_DOS_720, _LEN_1440639D }, // 4
			{ MX_ibm737, _T_D975C7E8_ibm737, _LEN_D975C7E8 }, // 5
			{ MX_ibm775, _T_531763C0_ibm775, _LEN_531763C0 }, // 6
			{ MX_ibm850, _T_1A17B2F0_ibm850, _LEN_1A17B2F0 }, // 7
			{ MX_ibm852, _T_F419D3DC_ibm852, _LEN_F419D3DC }, // 8
			{ MX_IBM855, _T_0C22C373_IBM855, _LEN_0C22C373 }, // 9
			{ MX_ibm857, _T_84732753_ibm857, _LEN_84732753 }, // 10
			{ MX_IBM00858, _T_C2E3F56A_IBM00858, _LEN_C2E3F56A }, // 11
			{ MX_IBM860, _T_5765643F_IBM860, _LEN_5765643F }, // 12
			{ MX_ibm861, _T_463DD1A5_ibm861, _LEN_463DD1A5 }, // 13
			{ MX_DOS_862, _T_957E8088_DOS_862, _LEN_957E8088 }, // 14
			{ MX_IBM863, _T_CE6C3585_IBM863, _LEN_CE6C3585 }, // 15
			{ MX_IBM864, _T_5008A026_IBM864, _LEN_5008A026 }, // 16
			{ MX_IBM865, _T_270F90B0_IBM865, _LEN_270F90B0 }, // 17
			{ MX_cp866, _T_1F51EAD8_cp866, _LEN_1F51EAD8 }, // 18
			{ MX_ibm869, _T_48E65997_ibm869, _LEN_48E65997 }, // 19
			{ MX_IBM870, _T_4E7E557E_IBM870, _LEN_4E7E557E }, // 20
			{ MX_windows_874, _T_4D542171_windows_874, _LEN_4D542171 }, // 21
			{ MX_cp875, _T_9F438A23_cp875, _LEN_9F438A23 }, // 22
			{ MX_shift_jis, _T_4BE552A4_shift_jis, _LEN_4BE552A4 }, // 23
			{ MX_gb2312, _T_354487BD_gb2312, _LEN_354487BD }, // 24
			{ MX_ks_c_5601_1987, _T_6FB9286F_ks_c_5601_1987, _LEN_6FB9286F }, // 25
			{ MX_big5, _T_8B622978_big5, _LEN_8B622978 }, // 26
			{ MX_IBM1026, _T_E03A63F7_IBM1026, _LEN_E03A63F7 }, // 27
			{ MX_IBM01047, _T_FDE8EF67_IBM01047, _LEN_FDE8EF67 }, // 28
			{ MX_IBM01140, _T_624E10F3_IBM01140, _LEN_624E10F3 }, // 29
			{ MX_IBM01141, _T_15492065_IBM01141, _LEN_15492065 }, // 30
			{ MX_IBM01142, _T_8C4071DF_IBM01142, _LEN_8C4071DF }, // 31
			{ MX_IBM01143, _T_FB474149_IBM01143, _LEN_FB474149 }, // 32
			{ MX_IBM01144, _T_6523D4EA_IBM01144, _LEN_6523D4EA }, // 33
			{ MX_IBM01145, _T_1224E47C_IBM01145, _LEN_1224E47C }, // 34
			{ MX_IBM01146, _T_8B2DB5C6_IBM01146, _LEN_8B2DB5C6 }, // 35
			{ MX_IBM01147, _T_FC2A8550_IBM01147, _LEN_FC2A8550 }, // 36
			{ MX_IBM01148, _T_6C9598C1_IBM01148, _LEN_6C9598C1 }, // 37
			{ MX_IBM01149, _T_1B92A857_IBM01149, _LEN_1B92A857 }, // 38
			{ MX_utf_16, _T_C1409499_utf_16, _LEN_C1409499 }, // 39
			{ MX_utf_16BE, _T_D2C12D29_utf_16BE, _LEN_D2C12D29 }, // 40
			{ MX_windows_1250, _T_B14C768A_windows_1250, _LEN_B14C768A }, // 41
			{ MX_windows_1251, _T_C64B461C_windows_1251, _LEN_C64B461C }, // 42
			{ MX_Windows_1252, _T_4773CC5E_Windows_1252, _LEN_4773CC5E }, // 43
			{ MX_windows_1253, _T_28452730_windows_1253, _LEN_28452730 }, // 44
			{ MX_windows_1254, _T_B621B293_windows_1254, _LEN_B621B293 }, // 45
			{ MX_windows_1255, _T_C1268205_windows_1255, _LEN_C1268205 }, // 46
			{ MX_windows_1256, _T_582FD3BF_windows_1256, _LEN_582FD3BF }, // 47
			{ MX_windows_1257, _T_2F28E329_windows_1257, _LEN_2F28E329 }, // 48
			{ MX_windows_1258, _T_BF97FEB8_windows_1258, _LEN_BF97FEB8 }, // 49
			{ MX_Johab, _T_2EBA90AA_Johab, _LEN_2EBA90AA }, // 50
			{ MX_macintosh, _T_046E6B37_macintosh, _LEN_046E6B37 }, // 51
			{ MX_x_mac_japanese, _T_FDA39FA1_x_mac_japanese, _LEN_FDA39FA1 }, // 52
			{ MX_x_mac_chinesetrad, _T_7127DDCB_x_mac_chinesetrad, _LEN_7127DDCB }, // 53
			{ MX_x_mac_korean, _T_AB037EF6_x_mac_korean, _LEN_AB037EF6 }, // 54
			{ MX_x_mac_arabic, _T_636316E2_x_mac_arabic, _LEN_636316E2 }, // 55
			{ MX_x_mac_hebrew, _T_2E509B57_x_mac_hebrew, _LEN_2E509B57 }, // 56
			{ MX_x_mac_greek, _T_4316E1CF_x_mac_greek, _LEN_4316E1CF }, // 57
			{ MX_x_mac_cyrillic, _T_37F31003_x_mac_cyrillic, _LEN_37F31003 }, // 58
			{ MX_x_mac_chinesesimp, _T_4AEC3292_x_mac_chinesesimp, _LEN_4AEC3292 }, // 59
			{ MX_x_mac_romanian, _T_5BB68F18_x_mac_romanian, _LEN_5BB68F18 }, // 60
			{ MX_x_mac_ukrainian, _T_BCFEABC1_x_mac_ukrainian, _LEN_BCFEABC1 }, // 61
			{ MX_x_mac_thai, _T_4EA582DD_x_mac_thai, _LEN_4EA582DD }, // 62
			{ MX_x_mac_ce, _T_CECB6F84_x_mac_ce, _LEN_CECB6F84 }, // 63
			{ MX_x_mac_icelandic, _T_B5C89EA8_x_mac_icelandic, _LEN_B5C89EA8 }, // 64
			{ MX_x_mac_turkish, _T_E765D5FD_x_mac_turkish, _LEN_E765D5FD }, // 65
			{ MX_x_mac_croatian, _T_9FC25553_x_mac_croatian, _LEN_9FC25553 }, // 66
			{ MX_utf_32, _T_F41B3202_utf_32, _LEN_F41B3202 }, // 67
			{ MX_utf_32BE, _T_7FC14D7E_utf_32BE, _LEN_7FC14D7E }, // 68
			{ MX_x_Chinese_CNS, _T_68D04FCE_x_Chinese_CNS, _LEN_68D04FCE }, // 69
			{ MX_x_cp20001, _T_FF449943_x_cp20001, _LEN_FF449943 }, // 70
			{ MX_x_Chinese_Eten, _T_813732C5_x_Chinese_Eten, _LEN_813732C5 }, // 71
			{ MX_x_cp20003, _T_114AF86F_x_cp20003, _LEN_114AF86F }, // 72
			{ MX_x_cp20004, _T_8F2E6DCC_x_cp20004, _LEN_8F2E6DCC }, // 73
			{ MX_x_cp20005, _T_F8295D5A_x_cp20005, _LEN_F8295D5A }, // 74
			{ MX_x_IA5, _T_9D4227D8_x_IA5, _LEN_9D4227D8 }, // 75
			{ MX_x_IA5_German, _T_C8C029EA_x_IA5_German, _LEN_C8C029EA }, // 76
			{ MX_x_IA5_Swedish, _T_5244EF59_x_IA5_Swedish, _LEN_5244EF59 }, // 77
			{ MX_x_IA5_Norwegian, _T_025A55C5_x_IA5_Norwegian, _LEN_025A55C5 }, // 78
			{ MX_us_ascii, _T_F038B127_us_ascii, _LEN_F038B127 }, // 79
			{ MX_x_cp20261, _T_AA9AEAAB_x_cp20261, _LEN_AA9AEAAB }, // 80
			{ MX_x_cp20269, _T_A4416299_x_cp20269, _LEN_A4416299 }, // 81
			{ MX_IBM273, _T_DAE08112_IBM273, _LEN_DAE08112 }, // 82
			{ MX_IBM277, _T_DD8D450B_IBM277, _LEN_DD8D450B }, // 83
			{ MX_IBM278, _T_4D32589A_IBM278, _LEN_4D32589A }, // 84
			{ MX_IBM280, _T_C471CC67_IBM280, _LEN_C471CC67 }, // 85
			{ MX_IBM284, _T_C31C087E_IBM284, _LEN_C31C087E }, // 86
			{ MX_IBM285, _T_B41B38E8_IBM285, _LEN_B41B38E8 }, // 87
			{ MX_IBM290, _T_DD6AFD26_IBM290, _LEN_DD6AFD26 }, // 88
			{ MX_IBM297, _T_430E6885_IBM297, _LEN_430E6885 }, // 89
			{ MX_IBM420, _T_3A13585F_IBM420, _LEN_3A13585F }, // 90
			{ MX_IBM423, _T_A31A09E5_IBM423, _LEN_A31A09E5 }, // 91
			{ MX_IBM424, _T_3D7E9C46_IBM424, _LEN_3D7E9C46 }, // 92
			{ MX_x_EBCDIC_KoreanExtended, _T_458E66C1_x_EBCDIC_KoreanExtended, _LEN_458E66C1 }, // 93
			{ MX_IBM_Thai, _T_40D0118F_IBM_Thai, _LEN_40D0118F }, // 94
			{ MX_koi8_r, _T_3AE4F8F8_koi8_r, _LEN_3AE4F8F8 }, // 95
			{ MX_IBM871, _T_397965E8_IBM871, _LEN_397965E8 }, // 96
			{ MX_IBM880, _T_C9E649B1_IBM880, _LEN_C9E649B1 }, // 97
			{ MX_IBM905, _T_70975D01_IBM905, _LEN_70975D01 }, // 98
			{ MX_IBM00924, _T_85D645B1_IBM00924, _LEN_85D645B1 }, // 99
			{ MX_EUC_JP, _T_C66DAEDC_EUC_JP, _LEN_C66DAEDC }, // 100
			{ MX_x_cp20936, _T_45DC64AC_x_cp20936, _LEN_45DC64AC }, // 101
			{ MX_x_cp20949, _T_9A22EFFA_x_cp20949, _LEN_9A22EFFA }, // 102
			{ MX_cp1025, _T_114FD69E_cp1025, _LEN_114FD69E }, // 103
			{ MX_koi8_u, _T_A4806D5B_koi8_u, _LEN_A4806D5B }, // 104
			{ MX_iso_8859_1, _T_4434A849_iso_8859_1, _LEN_4434A849 }, // 105
			{ MX_iso_8859_2, _T_DD3DF9F3_iso_8859_2, _LEN_DD3DF9F3 }, // 106
			{ MX_iso_8859_3, _T_AA3AC965_iso_8859_3, _LEN_AA3AC965 }, // 107
			{ MX_iso_8859_4, _T_345E5CC6_iso_8859_4, _LEN_345E5CC6 }, // 108
			{ MX_iso_8859_5, _T_43596C50_iso_8859_5, _LEN_43596C50 }, // 109
			{ MX_iso_8859_6, _T_DA503DEA_iso_8859_6, _LEN_DA503DEA }, // 110
			{ MX_iso_8859_7, _T_AD570D7C_iso_8859_7, _LEN_AD570D7C }, // 111
			{ MX_iso_8859_8, _T_3DE810ED_iso_8859_8, _LEN_3DE810ED }, // 112
			{ MX_iso_8859_9, _T_4AEF207B_iso_8859_9, _LEN_4AEF207B }, // 113
			{ MX_iso_8859_13, _T_62964307_iso_8859_13, _LEN_62964307 }, // 114
			{ MX_iso_8859_15, _T_8BF5E632_iso_8859_15, _LEN_8BF5E632 }, // 115
			{ MX_x_Europa, _T_4D6D3542_x_Europa, _LEN_4D6D3542 }, // 116
			{ MX_iso_8859_8_i, _T_A3FA31C1_iso_8859_8_i, _LEN_A3FA31C1 }, // 117
			{ MX_iso_2022_jp, _T_F90E5180_iso_2022_jp, _LEN_F90E5180 }, // 118
			{ MX_csISO2022JP, _T_50B7D228_csISO2022JP, _LEN_50B7D228 }, // 119
			{ MX_iso_2022_jp_, _T_F90E5180_iso_2022_jp, _LEN_F90E5180 }, // 120
			{ MX_iso_2022_kr, _T_0E1B01ED_iso_2022_kr, _LEN_0E1B01ED }, // 121
			{ MX_x_cp50227, _T_95B5568A_x_cp50227, _LEN_95B5568A }, // 122
			{ MX_euc_jp, _T_0ED82FBA_euc_jp, _LEN_0ED82FBA }, // 123
			{ MX_EUC_CN, _T_EDA028F6_EUC_CN, _LEN_EDA028F6 }, // 124
			{ MX_euc_kr, _T_F9CD7FD7_euc_kr, _LEN_F9CD7FD7 }, // 125
			{ MX_hz_gb_2312, _T_66BD1860_hz_gb_2312, _LEN_66BD1860 }, // 126
			{ MX_GB18030, _T_0157CE9A_GB18030, _LEN_0157CE9A }, // 127
			{ MX_x_iscii_de, _T_373E874E_x_iscii_de, _LEN_373E874E }, // 128
			{ MX_x_iscii_be, _T_616420C8_x_iscii_be, _LEN_616420C8 }, // 129
			{ MX_x_iscii_ta, _T_7A915106_x_iscii_ta, _LEN_7A915106 }, // 130
			{ MX_x_iscii_te, _T_7DFC951F_x_iscii_te, _LEN_7DFC951F }, // 131
			{ MX_x_iscii_as, _T_BE9DC65A_x_iscii_as, _LEN_BE9DC65A }, // 132
			{ MX_x_iscii_or, _T_5719DB42_x_iscii_or, _LEN_5719DB42 }, // 133
			{ MX_x_iscii_ka, _T_B7CB5F98_x_iscii_ka, _LEN_B7CB5F98 }, // 134
			{ MX_x_iscii_ma, _T_E191F81E_x_iscii_ma, _LEN_E191F81E }, // 135
			{ MX_x_iscii_gu, _T_01A4C4E9_x_iscii_gu, _LEN_01A4C4E9 }, // 136
			{ MX_x_iscii_pa, _T_1EFD9402_x_iscii_pa, _LEN_1EFD9402 }, // 137
			{ MX_utf_7, _T_C780E71B_utf_7, _LEN_C780E71B }, // 138
			{ MX_utf_8, _T_573FFA8A_utf_8, _LEN_573FFA8A }, // 139

		};

		struct {
			const CHAR * pcName;
			uint32_t ui32NameLen;
		}
		static const aDesc[] = {
			{ _T_3636B8C7_IBM_EBCDIC__US_Canada_, _LEN_3636B8C7 }, // 0
			{ _T_F624CB1C_OEM_United_States, _LEN_F624CB1C }, // 1
			{ _T_0D6DB3BF_IBM_EBCDIC__International_, _LEN_0D6DB3BF }, // 2
			{ _T_C5EADF38_Arabic__ASMO_708_, _LEN_C5EADF38 }, // 3
			{ _T_059E0FD9_Arabic__DOS_, _LEN_059E0FD9 }, // 4
			{ _T_2B8F6891_Greek__DOS_, _LEN_2B8F6891 }, // 5
			{ _T_6F30CFA8_Baltic__DOS_, _LEN_6F30CFA8 }, // 6
			{ _T_97510E52_Western_European__DOS_, _LEN_97510E52 }, // 7
			{ _T_3876CB7D_Central_European__DOS_, _LEN_3876CB7D }, // 8
			{ _T_2B1B2603_OEM_Cyrillic, _LEN_2B1B2603 }, // 9
			{ _T_D528A8A5_Turkish__DOS_, _LEN_D528A8A5 }, // 10
			{ _T_37A4A7AA_OEM_Multilingual_Latin_I, _LEN_37A4A7AA }, // 11
			{ _T_31529C73_Portuguese__DOS_, _LEN_31529C73 }, // 12
			{ _T_C5EA3F27_Icelandic__DOS_, _LEN_C5EA3F27 }, // 13
			{ _T_9C33B0E0_Hebrew__DOS_, _LEN_9C33B0E0 }, // 14
			{ _T_14D59C63_French_Canadian__DOS_, _LEN_14D59C63 }, // 15
			{ _T_55EBBE42_Arabic__864_, _LEN_55EBBE42 }, // 16
			{ _T_ADEB5A05_Nordic__DOS_, _LEN_ADEB5A05 }, // 17
			{ _T_7C15E36E_Cyrillic__DOS_, _LEN_7C15E36E }, // 18
			{ _T_C1991C02_Greek__Modern__DOS_, _LEN_C1991C02 }, // 19
			{ _T_B6733C9A_IBM_EBCDIC__Multilingual_Latin_2_, _LEN_B6733C9A }, // 20
			{ _T_3E2CF854_Thai__Windows_, _LEN_3E2CF854 }, // 21
			{ _T_CD781417_IBM_EBCDIC__Greek_Modern_, _LEN_CD781417 }, // 22
			{ _T_46C5A30E_Japanese__Shift_JIS_, _LEN_46C5A30E }, // 23
			{ _T_D4FB56EF_Chinese_Simplified__GB2312_, _LEN_D4FB56EF }, // 24
			{ _T_96F9CCF5_Korean, _LEN_96F9CCF5 }, // 25
			{ _T_17D4B6A5_Chinese_Traditional__Big5_, _LEN_17D4B6A5 }, // 26
			{ _T_F267F1F3_IBM_EBCDIC__Turkish_Latin_5_, _LEN_F267F1F3 }, // 27
			{ _T_B3A44696_IBM_Latin_1, _LEN_B3A44696 }, // 28
			{ _T_4CEA3B4F_IBM_EBCDIC__US_Canada_Euro_, _LEN_4CEA3B4F }, // 29
			{ _T_23C491D5_IBM_EBCDIC__Germany_Euro_, _LEN_23C491D5 }, // 30
			{ _T_2E3E88CE_IBM_EBCDIC__Denmark_Norway_Euro_, _LEN_2E3E88CE }, // 31
			{ _T_24122638_IBM_EBCDIC__Finland_Sweden_Euro_, _LEN_24122638 }, // 32
			{ _T_3D413EF9_IBM_EBCDIC__Italy_Euro_, _LEN_3D413EF9 }, // 33
			{ _T_926AAEE8_IBM_EBCDIC__Spain_Euro_, _LEN_926AAEE8 }, // 34
			{ _T_A5A2B650_IBM_EBCDIC__UK_Euro_, _LEN_A5A2B650 }, // 35
			{ _T_1028C286_IBM_EBCDIC__France_Euro_, _LEN_1028C286 }, // 36
			{ _T_0E123929_IBM_EBCDIC__International_Euro_, _LEN_0E123929 }, // 37
			{ _T_2D8D0EA8_IBM_EBCDIC__Icelandic_Euro_, _LEN_2D8D0EA8 }, // 38
			{ _T_1C0C8DFC_Unicode, _LEN_1C0C8DFC }, // 39
			{ _T_ECDCEB6A_Unicode__Big_Endian_, _LEN_ECDCEB6A }, // 40
			{ _T_4609212E_Central_European__Windows_, _LEN_4609212E }, // 41
			{ _T_0D9A73EA_Cyrillic__Windows_, _LEN_0D9A73EA }, // 42
			{ _T_B590C6F1_Western_European__Windows_, _LEN_B590C6F1 }, // 43
			{ _T_405E27BD_Greek__Windows_, _LEN_405E27BD }, // 44
			{ _T_15FB2B3E_Turkish__Windows_, _LEN_15FB2B3E }, // 45
			{ _T_0D324357_Hebrew__Windows_, _LEN_0D324357 }, // 46
			{ _T_DC69A415_Arabic__Windows_, _LEN_DC69A415 }, // 47
			{ _T_03A6EC8D_Baltic__Windows_, _LEN_03A6EC8D }, // 48
			{ _T_5A8E141B_Vietnamese__Windows_, _LEN_5A8E141B }, // 49
			{ _T_A7B49950_Korean__Johab_, _LEN_A7B49950 }, // 50
			{ _T_07CC1CC1_Western_European__Mac_, _LEN_07CC1CC1 }, // 51
			{ _T_DCDF7A6F_Japanese__Mac_, _LEN_DCDF7A6F }, // 52
			{ _T_4A7F4E34_Chinese_Traditional__Mac_, _LEN_4A7F4E34 }, // 53
			{ _T_009C51B0_Korean__Mac_, _LEN_009C51B0 }, // 54
			{ _T_95031D4A_Arabic__Mac_, _LEN_95031D4A }, // 55
			{ _T_0CAEA273_Hebrew__Mac_, _LEN_0CAEA273 }, // 56
			{ _T_BB127A02_Greek__Mac_, _LEN_BB127A02 }, // 57
			{ _T_EC88F1FD_Cyrillic__Mac_, _LEN_EC88F1FD }, // 58
			{ _T_375D858B_Chinese_Simplified__Mac_, _LEN_375D858B }, // 59
			{ _T_3BC8292F_Romanian__Mac_, _LEN_3BC8292F }, // 60
			{ _T_C9DCD822_Ukrainian__Mac_, _LEN_C9DCD822 }, // 61
			{ _T_6CB524E7_Thai__Mac_, _LEN_6CB524E7 }, // 62
			{ _T_A8EBD9EE_Central_European__Mac_, _LEN_A8EBD9EE }, // 63
			{ _T_55772DB4_Icelandic__Mac_, _LEN_55772DB4 }, // 64
			{ _T_45B5BA36_Turkish__Mac_, _LEN_45B5BA36 }, // 65
			{ _T_EFE9BD2F_Croatian__Mac_, _LEN_EFE9BD2F }, // 66
			{ _T_AFD526D0_Unicode__UTF_32_, _LEN_AFD526D0 }, // 67
			{ _T_91E0D7DB_Unicode__UTF_32_Big_Endian_, _LEN_91E0D7DB }, // 68
			{ _T_46F70E29_Chinese_Traditional__CNS_, _LEN_46F70E29 }, // 69
			{ _T_6CD14E90_TCA_Taiwan, _LEN_6CD14E90 }, // 70
			{ _T_5D4CB506_Chinese_Traditional__Eten_, _LEN_5D4CB506 }, // 71
			{ _T_4649662F_IBM5550_Taiwan, _LEN_4649662F }, // 72
			{ _T_5EB4791F_TeleText_Taiwan, _LEN_5EB4791F }, // 73
			{ _T_772418F4_Wang_Taiwan, _LEN_772418F4 }, // 74
			{ _T_5C0237A4_Western_European__IA5_, _LEN_5C0237A4 }, // 75
			{ _T_072FCDC8_German__IA5_, _LEN_072FCDC8 }, // 76
			{ _T_F9F58351_Swedish__IA5_, _LEN_F9F58351 }, // 77
			{ _T_0402B4FC_Norwegian__IA5_, _LEN_0402B4FC }, // 78
			{ _T_31A7285D_US_ASCII, _LEN_31A7285D }, // 79
			{ _T_8F3ABD4C_T_61, _LEN_8F3ABD4C }, // 80
			{ _T_34BE1FD4_ISO_6937, _LEN_34BE1FD4 }, // 81
			{ _T_DE08332B_IBM_EBCDIC__Germany_, _LEN_DE08332B }, // 82
			{ _T_4DDB51A6_IBM_EBCDIC__Denmark_Norway_, _LEN_4DDB51A6 }, // 83
			{ _T_CD4C7193_IBM_EBCDIC__Finland_Sweden_, _LEN_CD4C7193 }, // 84
			{ _T_E0308980_IBM_EBCDIC__Italy_, _LEN_E0308980 }, // 85
			{ _T_B4B548C0_IBM_EBCDIC__Spain_, _LEN_B4B548C0 }, // 86
			{ _T_99D5E525_IBM_EBCDIC__UK_, _LEN_99D5E525 }, // 87
			{ _T_E99C6585_IBM_EBCDIC__Japanese_katakana_, _LEN_E99C6585 }, // 88
			{ _T_BA8DB998_IBM_EBCDIC__France_, _LEN_BA8DB998 }, // 89
			{ _T_6E311AFE_IBM_EBCDIC__Arabic_, _LEN_6E311AFE }, // 90
			{ _T_AD3DC15B_IBM_EBCDIC__Greek_, _LEN_AD3DC15B }, // 91
			{ _T_D5776E70_IBM_EBCDIC__Hebrew_, _LEN_D5776E70 }, // 92
			{ _T_478989FA_IBM_EBCDIC__Korean_Extended_, _LEN_478989FA }, // 93
			{ _T_7EA9F0A0_IBM_EBCDIC__Thai_, _LEN_7EA9F0A0 }, // 94
			{ _T_6B91C6B9_Cyrillic__KOI8_R_, _LEN_6B91C6B9 }, // 95
			{ _T_EB2888BD_IBM_EBCDIC__Icelandic_, _LEN_EB2888BD }, // 96
			{ _T_FF7D2C88_IBM_EBCDIC__Cyrillic_Russian_, _LEN_FF7D2C88 }, // 97
			{ _T_46315751_IBM_EBCDIC__Turkish_, _LEN_46315751 }, // 98
			{ _T_B3A44696_IBM_Latin_1, _LEN_B3A44696 }, // 99
			{ _T_F6F8025C_Japanese__JIS_0208_1990_and_0212_1990_, _LEN_F6F8025C }, // 100
			{ _T_A39F12FD_Chinese_Simplified__GB2312_80_, _LEN_A39F12FD }, // 101
			{ _T_4C791D7E_Korean_Wansung, _LEN_4C791D7E }, // 102
			{ _T_D3B6297E_IBM_EBCDIC__Cyrillic_Serbian_Bulgarian_, _LEN_D3B6297E }, // 103
			{ _T_24D0507E_Cyrillic__KOI8_U_, _LEN_24D0507E }, // 104
			{ _T_9670D1C6_Western_European__ISO_, _LEN_9670D1C6 }, // 105
			{ _T_395714E9_Central_European__ISO_, _LEN_395714E9 }, // 106
			{ _T_4749EADA_Latin_3__ISO_, _LEN_4749EADA }, // 107
			{ _T_6E11103C_Baltic__ISO_, _LEN_6E11103C }, // 108
			{ _T_7D343CFA_Cyrillic__ISO_, _LEN_7D343CFA }, // 109
			{ _T_04BFD04D_Arabic__ISO_, _LEN_04BFD04D }, // 110
			{ _T_2AAEB705_Greek__ISO_, _LEN_2AAEB705 }, // 111
			{ _T_E22B30D6_Hebrew__ISO_Visual_, _LEN_E22B30D6 }, // 112
			{ _T_D4097731_Turkish__ISO_, _LEN_D4097731 }, // 113
			{ _T_447191CE_Estonian__ISO_, _LEN_447191CE }, // 114
			{ _T_E339B594_Latin_9__ISO_, _LEN_E339B594 }, // 115
			{ _T_485299F1_Europa, _LEN_485299F1 }, // 116
			{ _T_63C1E97F_Hebrew__ISO_Logical_, _LEN_63C1E97F }, // 117
			{ _T_A810637D_Japanese__JIS_, _LEN_A810637D }, // 118
			{ _T_DA96AADA_Japanese__JIS_Allow_1_byte_Kana_, _LEN_DA96AADA }, // 119
			{ _T_BD28A51B_Japanese__JIS_Allow_1_byte_Kana___SO_SI_, _LEN_BD28A51B }, // 120
			{ _T_91209CB7_Korean__ISO_, _LEN_91209CB7 }, // 121
			{ _T_849FD716_Chinese_Simplified__ISO_2022_, _LEN_849FD716 }, // 122
			{ _T_AF8D3B6E_Japanese__EUC_, _LEN_AF8D3B6E }, // 123
			{ _T_440FC48A_Chinese_Simplified__EUC_, _LEN_440FC48A }, // 124
			{ _T_73CE10B1_Korean__EUC_, _LEN_73CE10B1 }, // 125
			{ _T_26A48EB9_Chinese_Simplified__HZ_, _LEN_26A48EB9 }, // 126
			{ _T_73EB94AB_Chinese_Simplified__GB18030_, _LEN_73EB94AB }, // 127
			{ _T_3AB85660_ISCII_Devanagari, _LEN_3AB85660 }, // 128
			{ _T_67A671DC_ISCII_Bengali, _LEN_67A671DC }, // 129
			{ _T_6D6E24D6_ISCII_Tamil, _LEN_6D6E24D6 }, // 130
			{ _T_26443181_ISCII_Telugu, _LEN_26443181 }, // 131
			{ _T_2F7B76CA_ISCII_Assamese, _LEN_2F7B76CA }, // 132
			{ _T_0B88BC04_ISCII_Oriya, _LEN_0B88BC04 }, // 133
			{ _T_D2A53BA9_ISCII_Kannada, _LEN_D2A53BA9 }, // 134
			{ _T_BAB70C89_ISCII_Malayalam, _LEN_BAB70C89 }, // 135
			{ _T_E7992CDD_ISCII_Gujarati, _LEN_E7992CDD }, // 136
			{ _T_F8D80BF3_ISCII_Punjabi, _LEN_F8D80BF3 }, // 137
			{ _T_3E5EE04E_Unicode__UTF_7_, _LEN_3E5EE04E }, // 138
			{ _T_B9C6FC81_Unicode__UTF_8_, _LEN_B9C6FC81 }, // 139
		};

		for ( size_t I = 0; I < sizeof( aTable ) / sizeof( aTable[0] ); ++I ) {
			if ( aTable[I].cpCode == _uiCode ) {
				CStringDecoder::Decode( aTable[I].pcName, aTable[I].ui32NameLen, _pcValue );
				CStringDecoder::Decode( aDesc[I].pcName, aDesc[I].ui32NameLen, _pcDesc );
				return TRUE;
			}
		}
		return FALSE;
	}

	// Decodes a code page into a text value and description.
	BOOL CCodePages::CodePageToString( uint32_t _uiCode, CSecureString &_sValue, CSecureString &_sDesc ) {
		CHAR szValue[_T_MAX_LEN], szDesc[_T_MAX_LEN];
		if ( CodePageToString( _uiCode, szValue, szDesc ) ) {
			_sValue += szValue;
			_sDesc += szDesc;
			return TRUE;
		}
		return FALSE;
	}

	// Decodes a code page into a text value and description.
	BOOL CCodePages::CodePageToString( uint32_t _ui32Code, CSecureWString &_sValue, CSecureWString &_sDesc ) {
		CHAR szValue[_T_MAX_LEN], szDesc[_T_MAX_LEN];
		if ( CodePageToString( _ui32Code, szValue, szDesc ) ) {
			_sValue += CUtilities::StringToWString( szValue );
			_sDesc += CUtilities::StringToWString( szDesc );
			return TRUE;
		}
		return FALSE;
	}

	// Gets a code page identifier by index.
	CCodePages::MX_CODE_PAGES CCodePages::CodePageByIndex( uint32_t _ui32Idx ) {
		if ( _ui32Idx >= TotalCodePages() ) { return static_cast<CCodePages::MX_CODE_PAGES>(-1); }
		return m_cpPages[_ui32Idx];
	}

	// Gets the total number of code pages.
	size_t CCodePages::TotalCodePages() {
		return sizeof( m_cpPages ) / sizeof( m_cpPages[0] );
	}

	// Gets all the system code pages, sorted by code page.
	std::vector<CCodePages::MX_CODE_PAGE> & CCodePages::GetSystemCodePages( std::vector<CCodePages::MX_CODE_PAGE> &_vRet, bool _bIncludeExtendedUtf ) {
		LSW_ENT_CRIT( m_csCrit );
		if ( !m_sSystemCodePages.size() ) {
			::EnumSystemCodePagesW( CCodePages::EnumCodePagesProc, CP_SUPPORTED );
		}
		for ( auto I = m_sSystemCodePages.begin(); I != m_sSystemCodePages.end(); ++I ) {
			CPINFOEXW cpInfo = { 0 };
			if ( ::GetCPInfoExW( (*I), 0, &cpInfo ) ) {
				_vRet.push_back( { cpInfo.CodePage, cpInfo.CodePageName } );
			}
		}
		if ( _bIncludeExtendedUtf ) {
			if ( !m_sSystemCodePages.count( MX_utf_16 ) ) {
				_vRet.push_back( { MX_utf_16, _DEC_WS_99793C4E_1200__UTF_16_LE_ } );
			}
			if ( !m_sSystemCodePages.count( MX_utf_16BE ) ) {
				_vRet.push_back( { MX_utf_16BE, _DEC_WS_4E71C8C1_1201__UTF_16_BE_ } );
			}
			if ( !m_sSystemCodePages.count( MX_utf_32 ) ) {
				_vRet.push_back( { MX_utf_32, _DEC_WS_3B7FE4CA_12000__UTF_32_LE_ } );
			}
			if ( !m_sSystemCodePages.count( MX_utf_32BE ) ) {
				_vRet.push_back( { MX_utf_32BE, _DEC_WS_EC771045_12001__UTF_32_BE_ } );
			}
		}
		return _vRet;
		/*std::map<UINT, CSecureWString> mMap;
		CPINFOEXW cpInfo = { 0 };
		for ( uint64_t I = 0; I <= 0xFFFF; ++I ) {
			if ( ::IsValidCodePage( static_cast<UINT>(I) ) ) {
				if ( ::GetCPInfoExW( static_cast<UINT>(I), 0, &cpInfo ) ) {
					mMap[cpInfo.CodePage] = cpInfo.CodePageName;
				}
			}
		}
		for ( auto I = mMap.begin(); I != mMap.end(); ++I ) {
			_vRet.push_back( { I->first, I->second } );
		}
		return _vRet;*/
	}

	// Gets the system default Windows ANSI code page.
	UINT CCodePages::GetSystemDefaultAnsiCodePage() {
		CPINFOEXW cpInfo = { 0 };
		if ( ::GetCPInfoExW( CP_ACP, 0, &cpInfo ) ) {
			return cpInfo.CodePage;
		}
		return static_cast<UINT>(-1);
	}

	// Gathers code pages.
	BOOL CALLBACK CCodePages::EnumCodePagesProc( LPWSTR _lpCodePageString ) {
		LSW_ENT_CRIT( m_csCrit );
		UINT uiPage = ::_wtoi( _lpCodePageString );
		if ( ::IsValidCodePage( uiPage ) ) {
			if ( !m_sSystemCodePages.count( uiPage ) ) {
				m_sSystemCodePages.insert( uiPage );
			}
		}
		return TRUE;
	}

}	// namespace mx

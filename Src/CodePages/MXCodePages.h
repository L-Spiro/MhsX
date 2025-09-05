#pragma once

#include <LSWWin.h>
#include "../Strings/MXSecureString.h"
#include "../Strings/MXSecureWString.h"
#include <CriticalSection/LSWCriticalSection.h>

#include <set>
#include <vector>

namespace mx {

	class CCodePages {
	public :
		// == Enumerations.
		enum MX_CODE_PAGES {
			MX_IBM037 = 37,
			MX_IBM437 = 437,
			MX_IBM500 = 500,
			MX_ASMO_708 = 708,
			MX_DOS_720 = 720,
			MX_ibm737 = 737,
			MX_ibm775 = 775,
			MX_ibm850 = 850,
			MX_ibm852 = 852,
			MX_IBM855 = 855,
			MX_ibm857 = 857,
			MX_IBM00858 = 858,
			MX_IBM860 = 860,
			MX_ibm861 = 861,
			MX_DOS_862 = 862,
			MX_IBM863 = 863,
			MX_IBM864 = 864,
			MX_IBM865 = 865,
			MX_cp866 = 866,
			MX_ibm869 = 869,
			MX_IBM870 = 870,
			MX_windows_874 = 874,
			MX_cp875 = 875,
			MX_shift_jis = 932,
			MX_gb2312 = 936,
			MX_ks_c_5601_1987 = 949,
			MX_big5 = 950,
			MX_IBM1026 = 1026,
			MX_IBM01047 = 1047,
			MX_IBM01140 = 1140,
			MX_IBM01141 = 1141,
			MX_IBM01142 = 1142,
			MX_IBM01143 = 1143,
			MX_IBM01144 = 1144,
			MX_IBM01145 = 1145,
			MX_IBM01146 = 1146,
			MX_IBM01147 = 1147,
			MX_IBM01148 = 1148,
			MX_IBM01149 = 1149,
			MX_utf_16 = 1200,
			MX_utf_16BE = 1201,
			MX_windows_1250 = 1250,
			MX_windows_1251 = 1251,
			MX_Windows_1252 = 1252,
			MX_windows_1253 = 1253,
			MX_windows_1254 = 1254,
			MX_windows_1255 = 1255,
			MX_windows_1256 = 1256,
			MX_windows_1257 = 1257,
			MX_windows_1258 = 1258,
			MX_Johab = 1361,
			MX_macintosh = 10000,
			MX_x_mac_japanese = 10001,
			MX_x_mac_chinesetrad = 10002,
			MX_x_mac_korean = 10003,
			MX_x_mac_arabic = 10004,
			MX_x_mac_hebrew = 10005,
			MX_x_mac_greek = 10006,
			MX_x_mac_cyrillic = 10007,
			MX_x_mac_chinesesimp = 10008,
			MX_x_mac_romanian = 10010,
			MX_x_mac_ukrainian = 10017,
			MX_x_mac_thai = 10021,
			MX_x_mac_ce = 10029,
			MX_x_mac_icelandic = 10079,
			MX_x_mac_turkish = 10081,
			MX_x_mac_croatian = 10082,
			MX_utf_32 = 12000,
			MX_utf_32BE = 12001,
			MX_x_Chinese_CNS = 20000,
			MX_x_cp20001 = 20001,
			MX_x_Chinese_Eten = 20002,
			MX_x_cp20003 = 20003,
			MX_x_cp20004 = 20004,
			MX_x_cp20005 = 20005,
			MX_x_IA5 = 20105,
			MX_x_IA5_German = 20106,
			MX_x_IA5_Swedish = 20107,
			MX_x_IA5_Norwegian = 20108,
			MX_us_ascii = 20127,
			MX_x_cp20261 = 20261,
			MX_x_cp20269 = 20269,
			MX_IBM273 = 20273,
			MX_IBM277 = 20277,
			MX_IBM278 = 20278,
			MX_IBM280 = 20280,
			MX_IBM284 = 20284,
			MX_IBM285 = 20285,
			MX_IBM290 = 20290,
			MX_IBM297 = 20297,
			MX_IBM420 = 20420,
			MX_IBM423 = 20423,
			MX_IBM424 = 20424,
			MX_x_EBCDIC_KoreanExtended = 20833,
			MX_IBM_Thai = 20838,
			MX_koi8_r = 20866,
			MX_IBM871 = 20871,
			MX_IBM880 = 20880,
			MX_IBM905 = 20905,
			MX_IBM00924 = 20924,
			MX_EUC_JP = 20932,
			MX_x_cp20936 = 20936,
			MX_x_cp20949 = 20949,
			MX_cp1025 = 21025,
			MX_koi8_u = 21866,
			MX_iso_8859_1 = 28591,
			MX_iso_8859_2 = 28592,
			MX_iso_8859_3 = 28593,
			MX_iso_8859_4 = 28594,
			MX_iso_8859_5 = 28595,
			MX_iso_8859_6 = 28596,
			MX_iso_8859_7 = 28597,
			MX_iso_8859_8 = 28598,
			MX_iso_8859_9 = 28599,
			MX_iso_8859_13 = 28603,
			MX_iso_8859_15 = 28605,
			MX_x_Europa = 29001,
			MX_iso_8859_8_i = 38598,
			MX_iso_2022_jp = 50220,
			MX_csISO2022JP = 50221,
			MX_iso_2022_jp_ = 50222,
			MX_iso_2022_kr = 50225,
			MX_x_cp50227 = 50227,
			MX_euc_jp = 51932,
			MX_EUC_CN = 51936,
			MX_euc_kr = 51949,
			MX_hz_gb_2312 = 52936,
			MX_GB18030 = 54936,
			MX_x_iscii_de = 57002,
			MX_x_iscii_be = 57003,
			MX_x_iscii_ta = 57004,
			MX_x_iscii_te = 57005,
			MX_x_iscii_as = 57006,
			MX_x_iscii_or = 57007,
			MX_x_iscii_ka = 57008,
			MX_x_iscii_ma = 57009,
			MX_x_iscii_gu = 57010,
			MX_x_iscii_pa = 57011,
			MX_utf_7 = 65000,
			MX_utf_8 = 65001,
		};


		// == Types.
		// A codepade and its name.
		struct MX_CODE_PAGE {
			UINT						uiCodePage;
			CSecureWString				swsName;
		};


		// == Functions.
		// Decodes a code page into a text value and description.  _pcValue and _pcDesc should be _T_MAX_LEN characters long.
		static BOOL						CodePageToString( uint32_t _ui32Code, CHAR * _pcValue, CHAR * _pcDesc );

		// Decodes a code page into a text value and description.
		static BOOL						CodePageToString( uint32_t _ui32Code, CSecureString &_sValue, CSecureString &_sDesc );

		// Decodes a code page into a text value and description.
		static BOOL						CodePageToString( uint32_t _ui32Code, CSecureWString &_sValue, CSecureWString &_sDesc );

		// Gets a code page identifier by index.
		static MX_CODE_PAGES			CodePageByIndex( uint32_t _ui32Idx );

		// Gets the total number of code pages.
		static size_t					TotalCodePages();

		// Gets all the system code pages, sorted by code page.
		static std::vector<CCodePages::MX_CODE_PAGE> &
										GetSystemCodePages( std::vector<CCodePages::MX_CODE_PAGE> &_vRet, bool _bIncludeExtendedUtf );

		// Gets the system default Windows ANSI code page.
		static UINT						GetSystemDefaultAnsiCodePage();

		// Is the given code page supported?
		static bool						Supported( UINT _uiCodePage );

	protected :
		// == Members.
		// The internal list of code pages.
		static const MX_CODE_PAGES		m_cpPages[];

		// The critical section.
		static lsw::CCriticalSection	m_csCrit;

		// The set of code pages.
		static std::set<UINT>			m_sSystemCodePages;


		// == Functions.
		// Gathers code pages.
		static BOOL CALLBACK			EnumCodePagesProc( LPWSTR _lpCodePageString );
	};

}	// namespace mx

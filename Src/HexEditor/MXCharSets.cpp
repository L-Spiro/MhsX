#include "MXCharSets.h"
#include "../Utilities/MXUtilities.h"


namespace mx {

	// == Members.
	const CCharSets::MX_CHAR_SET CCharSets::m_csSets[] = {										/** The predefined character sets. */
		//pcName										pcStatusBar								pfDisplayable								ui32Code							ui32Id	bHideOver127	bTopMenu
		{ _T_LEN_807BA8DC__ASCII,						_T_LEN_266E1849_ASCII,					AsciiANSIDisplay,							CCodePages::MX_Windows_1252,		0,		true,			false },
		{ _T_LEN_470FBFA1_ASCII_A_NSI,					_T_LEN_E67EB72D_ANSI,					AsciiANSIDisplay,							CCodePages::MX_Windows_1252,		1,		false,			true },
		{ _T_LEN_4A16753C_ASCII__OEM,					_T_LEN_01E49582_OEM,					AsciiOemDisplay,							CCodePages::MX_IBM437,				2,		false,			false },
		{ _T_LEN_7CFB7587__EBCDIC,						_T_LEN_A48AAB94_EBC,					EbcdicDisplay,								CCodePages::MX_IBM037,				3,		true,			false },
		{ _T_LEN_DB5BE63A__Unicode,						_T_LEN_F4CCAEF6_UNI,					Utf16Display,								CCodePages::MX_utf_16,				4,		false,			true },
		{ _T_LEN_4524B804_UTF__8,						_T_LEN_3803F6FD_UTF8,					AsciiANSIDisplay,							CCodePages::MX_utf_8,				5,		false,			true },
		{ _T_LEN_C15FE6DB__Macintosh,					_T_LEN_81B4A9EF_MAC,					MacintoshDisplay,							CCodePages::MX_macintosh,			6,		false,			false },
		{ _T_LEN_70AA9B0F__Arabic__Windows_,			_T_LEN_074F7035_ARA,					ArabicWindowsDisplay,						CCodePages::MX_windows_1256,		7,		false,			false },
		{ _T_LEN_9A84344E_Arabic___ISO_,				_T_LEN_9CC705E1_ARA_I,					ArabicIsoDisplay,							CCodePages::MX_iso_8859_6,			8,		false,			false },
		{ _T_LEN_AF65D397__Baltic__Windows_,			_T_LEN_1A57F343_BAL,					BalticWindowsDisplay,						CCodePages::MX_windows_1257,		9,		false,			false },
		{ _T_LEN_57BDB155_Ba_ltic__ISO_,				_T_LEN_91131413_BAL_I,					BalticIsoDisplay,							CCodePages::MX_iso_8859_13,			10,		false,			false },
		{ _T_LEN_0E858F99_Chinese__Si_mplified_,		_T_LEN_0424AE16_CH_S,					AsciiANSIDisplay,							CCodePages::MX_GB18030,				11,		false,			false },
		{ _T_LEN_5C9EB386__Chinese__Traditional_,		_T_LEN_9A403BB5_CH_T,					AsciiANSIDisplay,							CCodePages::MX_big5,				12,		false,			false },
		{ _T_LEN_B322F1B2_Cy_rillic__Windows_,			_T_LEN_63813C4E_CYR,					CyrillicWindowsDisplay,						CCodePages::MX_windows_1251,		13,		false,			false },
		{ _T_LEN_DA02E6A6_Cyrillic__K_OI8_R_,			_T_LEN_A5030545_CYR_R,					CyrillicKoi8RDisplay,						CCodePages::MX_koi8_r,				14,		false,			false },
		{ _T_LEN_46567AD6_Cyrillic__KOI_8_U_,			_T_LEN_3B6790E6_CYR_U,					CyrillicKoi8UDisplay,						CCodePages::MX_koi8_u,				15,		false,			false },
		{ _T_LEN_A8A4B69A_C_yrillic__ISO_,				_T_LEN_2F66CCA9_CYR_I,					CyrillicIsoDisplay,							CCodePages::MX_iso_8859_5,			16,		false,			false },
		{ _T_LEN_0B5B5DE3__Eastern_Europe__Windows_,	_T_LEN_CBB90FF0_EUR,					EasternEuropeWindowsDisplay,				CCodePages::MX_windows_1250,		17,		false,			false },
		{ _T_LEN_832DEC00_Eastern_Euro_pe__ISO_,		_T_LEN_EAF086B1_EUR_I,					EasternEuropeIsoDisplay,					CCodePages::MX_iso_8859_2,			18,		false,			false },
		{ _T_LEN_3F987D96__Greek__Windows_,				_T_LEN_04AFC89E_GRE,					GreekWindowsDisplay,						CCodePages::MX_windows_1253,		19,		false,			false },
		{ _T_LEN_9A8AEB42_Greek__I_SO_,					_T_LEN_148E589D_GRE_I,					GreekIsoDisplay,							CCodePages::MX_iso_8859_7,			20,		false,			false },
		{ _T_LEN_A1F17C4D__Hebrew__Windows_,			_T_LEN_94149E96_HEB,					AsciiANSIDisplay,							CCodePages::MX_windows_1255,		21,		false,			false },
		{ _T_LEN_E9850994_Hebre_w__ISO_,				_T_LEN_5E5FB6EF_HEB_I,					AsciiANSIDisplay,							CCodePages::MX_iso_8859_8,			22,		false,			false },
		{ _T_LEN_EAF36EB9__Japanese__Shift_JIS_,		_T_LEN_0045FEB4_JAP,					AsciiANSIDisplay,							CCodePages::MX_shift_jis,			23,		false,			false },
		{ _T_LEN_14A26E2D_Japa_nese__EUC_JP_,			_T_LEN_BDE949ED_JAP_E,					AsciiANSIDisplay,							CCodePages::MX_EUC_JP,				24,		false,			false },
		{ _T_LEN_163ABE8E__Korean,						_T_LEN_710AD821_KOR,					AsciiANSIDisplay,							CCodePages::MX_euc_kr,				24,		false,			false },
		{ _T_LEN_C280ACCE__Thai,						_T_LEN_FDC01CE1_THAI,					AsciiANSIDisplay,							CCodePages::MX_windows_874,			25,		false,			false },
		{ _T_LEN_9401B004_T_urkish__Windows_,			_T_LEN_66DF0C7C_TURK,					AsciiANSIDisplay,							CCodePages::MX_windows_1254,		26,		false,			false },
		{ _T_LEN_B51B4222_Turkish__IS_O_,				_T_LEN_E4CFAD2A_TURK_I,					AsciiANSIDisplay,							CCodePages::MX_iso_8859_9,			27,		false,			false },
		{ _T_LEN_E97EC64F__Vietnamese,					_T_LEN_51611780_VIET,					AsciiANSIDisplay,							CCodePages::MX_windows_1258,		28,		false,			false },
	};

	const wchar_t CCharSets::m_wcAsciiAnsi[] = {								/** ASCII/ANSI characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u20AC',	L'\u0081',	L'\u201A',	L'\u0192',	L'\u201E',	L'\u2026',	L'\u2020',	L'\u2021',	L'\u02C6',	L'\u2030',	L'\u0160',	L'\u2039',	L'\u0152',	L'\u008D',	L'\u017D',	L'\u008F',	// 0x80
		L'\u0090',	L'\u2018',	L'\u2019',	L'\u201C',	L'\u201D',	L'\u2022',	L'\u2013',	L'\u2014',	L'\u02DC',	L'\u2122',	L'\u0161',	L'\u203A',	L'\u0153',	L'\u009D',	L'\u017E',	L'\u0178',	// 0x90
		L'\u00A0',	L'\u00A1',	L'\u00A2',	L'\u00A3',	L'\u00A4',	L'\u00A5',	L'\u00A6',	L'\u00A7',	L'\u00A8',	L'\u00A9',	L'\u00AA',	L'\u00AB',	L'\u00AC',	L'\u00AD',	L'\u00AE',	L'\u00AF',	// 0xA0
		L'\u00B0',	L'\u00B1',	L'\u00B2',	L'\u00B3',	L'\u00B4',	L'\u00B5',	L'\u00B6',	L'\u00B7',	L'\u00B8',	L'\u00B9',	L'\u00BA',	L'\u00BB',	L'\u00BC',	L'\u00BD',	L'\u00BE',	L'\u00BF',	// 0xB0
		L'\u00C0',	L'\u00C1',	L'\u00C2',	L'\u00C3',	L'\u00C4',	L'\u00C5',	L'\u00C6',	L'\u00C7',	L'\u00C8',	L'\u00C9',	L'\u00CA',	L'\u00CB',	L'\u00CC',	L'\u00CD',	L'\u00CE',	L'\u00CF',	// 0xC0
		L'\u00D0',	L'\u00D1',	L'\u00D2',	L'\u00D3',	L'\u00D4',	L'\u00D5',	L'\u00D6',	L'\u00D7',	L'\u00D8',	L'\u00D9',	L'\u00DA',	L'\u00DB',	L'\u00DC',	L'\u00DD',	L'\u00DE',	L'\u00DF',	// 0xD0
		L'\u00E0',	L'\u00E1',	L'\u00E2',	L'\u00E3',	L'\u00E4',	L'\u00E5',	L'\u00E6',	L'\u00E7',	L'\u00E8',	L'\u00E9',	L'\u00EA',	L'\u00EB',	L'\u00EC',	L'\u00ED',	L'\u00EE',	L'\u00EF',	// 0xE0
		L'\u00F0',	L'\u00F1',	L'\u00F2',	L'\u00F3',	L'\u00F4',	L'\u00F5',	L'\u00F6',	L'\u00F7',	L'\u00F8',	L'\u00F9',	L'\u00FA',	L'\u00FB',	L'\u00FC',	L'\u00FD',	L'\u00FE',	L'\u00FF',	// 0xF0
	};

	const wchar_t CCharSets::m_wcAsciiOEM[] = {									/** ASCII/OEM characters. */
		L'\x00',	L'\u263A',	L'\u263B',	L'\u2665',	L'\u2666',	L'\u2663',	L'\u2660',	L'\u2022',	L'\u25D8',	L'\u25CB',	L'\u25D9',	L'\u2642',	L'\u2640',	L'\u266A',	L'\u266B',	L'\u263C',	// 0x00
		L'\u25BA',	L'\u25C4',	L'\u2195',	L'\u203C',	L'\u00B6',	L'\u00A7',	L'\u25AC',	L'\u21A8',	L'\u2191',	L'\u2193',	L'\u2192',	L'\u2190',	L'\u221F',	L'\u2194',	L'\u25B2',	L'\u25BC',	// 0x10
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\u2302',	// 0x70
		L'\u00C7',	L'\u00FC',	L'\u00E9',	L'\u00E2',	L'\u00E4',	L'\u00E0',	L'\u00E5',	L'\u00E7',	L'\u00EA',	L'\u00EB',	L'\u00E8',	L'\u00EF',	L'\u00EE',	L'\u00EC',	L'\u00C4',	L'\u00C5',	// 0x80
		L'\u00C9',	L'\u00E6',	L'\u00C6',	L'\u00F4',	L'\u00F6',	L'\u00F2',	L'\u00FB',	L'\u00F9',	L'\u00FF',	L'\u00D6',	L'\u00DC',	L'\u00A2',	L'\u00A3',	L'\u00A5',	L'\u20A7',	L'\u0192',	// 0x90
		L'\u00E1',	L'\u00ED',	L'\u00F3',	L'\u00FA',	L'\u00F1',	L'\u00D1',	L'\u00AA',	L'\u00BA',	L'\u00BF',	L'\u2310',	L'\u00AC',	L'\u00BD',	L'\u00BC',	L'\u00A1',	L'\u00AB',	L'\u00BB',	// 0xA0
		L'\u2591',	L'\u2592',	L'\u2593',	L'\u2502',	L'\u2524',	L'\u2561',	L'\u2562',	L'\u2556',	L'\u2555',	L'\u2563',	L'\u2551',	L'\u2557',	L'\u255D',	L'\u255C',	L'\u255B',	L'\u2510',	// 0xB0
		L'\u2514',	L'\u2534',	L'\u252C',	L'\u251C',	L'\u2500',	L'\u253C',	L'\u255E',	L'\u255F',	L'\u255A',	L'\u2554',	L'\u2569',	L'\u2566',	L'\u2560',	L'\u2550',	L'\u256C',	L'\u2567',	// 0xC0
		L'\u2568',	L'\u2564',	L'\u2565',	L'\u2559',	L'\u2558',	L'\u2552',	L'\u2553',	L'\u256B',	L'\u256A',	L'\u2518',	L'\u250C',	L'\u2588',	L'\u2584',	L'\u258C',	L'\u2590',	L'\u2580',	// 0xD0
		L'\u03B1',	L'\u00DF',	L'\u0393',	L'\u03C0',	L'\u03A3',	L'\u03C3',	L'\u00B5',	L'\u03C4',	L'\u03A6',	L'\u0398',	L'\u03A9',	L'\u03B4',	L'\u221E',	L'\u03C6',	L'\u03B5',	L'\u2229',	// 0xE0
		L'\u2261',	L'\u00B1',	L'\u2265',	L'\u2264',	L'\u2320',	L'\u2321',	L'\u00F7',	L'\u2248',	L'\u00B0',	L'\u2219',	L'\u00B7',	L'\u221A',	L'\u207F',	L'\u00B2',	L'\u25A0',	L'\u00A0',	// 0xF0
	};

	const wchar_t CCharSets::m_wcEbcdic[] = {									/** EBCDIC characters. */
		L'\x20',	L'\u00A0',	L'\u00E2',	L'\u00E4',	L'\u00E0',	L'\u00E1',	L'\u00E3',	L'\u00E5',	L'\u00E7',	L'\u00F1',	L'\u00A2',	L'\x2E',	L'\x3C',	L'\x28',	L'\x2B',	L'\x7C',	// 0x40
		L'\x26',	L'\u00E9',	L'\u00EA',	L'\u00EB',	L'\u00E8',	L'\u00ED',	L'\u00EE',	L'\u00EF',	L'\u00EC',	L'\u00DF',	L'\x21',	L'\x24',	L'\x2A',	L'\x29',	L'\x3B',	L'\u00AC',	// 0x50
		L'\x2D',	L'\x2F',	L'\u00C2',	L'\u00C4',	L'\u00C0',	L'\u00C1',	L'\u00C3',	L'\u00C5',	L'\u00C7',	L'\u00D1',	L'\u00A6',	L'\x2C',	L'\x25',	L'\x5F',	L'\x3E',	L'\x3F',	// 0x60
		L'\u00F8',	L'\u00C9',	L'\u00CA',	L'\u00CB',	L'\u00C8',	L'\u00CD',	L'\u00CE',	L'\u00CF',	L'\u00CC',	L'\x60',	L'\x3A',	L'\x23',	L'\x40',	L'\x27',	L'\x3D',	L'\x22',	// 0x70
		L'\u00D8',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\u00AB',	L'\u00BB',	L'\u00F0',	L'\u00FD',	L'\u00FE',	L'\u00B1',	// 0x80
		L'\u00B0',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	L'\x70',	L'\x71',	L'\x72',	L'\u00AA',	L'\u00BA',	L'\u00E6',	L'\u00B8',	L'\u00C6',	L'\u00A4',	// 0x90
		L'\u00B5',	L'\x7E',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\u00A1',	L'\u00BF',	L'\u00D0',	L'\u00DD',	L'\u00DE',	L'\u00AE',	// 0xA0
		L'\x5E',	L'\u00A3',	L'\u00A5',	L'\u00B7',	L'\u00A9',	L'\u00A7',	L'\u00B6',	L'\u00BC',	L'\u00BD',	L'\u00BE',	L'\x5B',	L'\x5D',	L'\u00AF',	L'\u00A8',	L'\u00B4',	L'\u00D7',	// 0xB0
		L'\x7B',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x2E',	L'\u00F4',	L'\u00F6',	L'\u00F2',	L'\u00F3',	L'\u00F5',	// 0xC0
		L'\x7D',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	L'\x50',	L'\x51',	L'\x52',	L'\u00B9',	L'\u00FB',	L'\u00FC',	L'\u00F9',	L'\u00FA',	L'\u00FF',	// 0xD0
		L'\x5C',	L'\u00F7',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\u00B2',	L'\u00D4',	L'\u00D6',	L'\u00D2',	L'\u00D3',	L'\u00D5',	// 0xE0
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\u00B3',	L'\u00DB',	L'\u00DC',	L'\u00D9',	L'\u00DA',	L'\u009F',	// 0xF0
	};

	const wchar_t CCharSets::m_wcMac[] = {										/** Macintosh characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u00C4',	L'\u00C5',	L'\u00C7',	L'\u00C9',	L'\u00D1',	L'\u00D6',	L'\u00DC',	L'\u00E1',	L'\u00E0',	L'\u00E2',	L'\u00E4',	L'\u00E3',	L'\u00E5',	L'\u00E7',	L'\u00E9',	L'\u00E8',	// 0x80
		L'\u00EA',	L'\u00EB',	L'\u00ED',	L'\u00EC',	L'\u00EE',	L'\u00EF',	L'\u00F1',	L'\u00F3',	L'\u00F2',	L'\u00F4',	L'\u00F6',	L'\u00F5',	L'\u00FA',	L'\u00F9',	L'\u00FB',	L'\u00FC',	// 0x90
		L'\u2020',	L'\u00B0',	L'\u00A2',	L'\u00A3',	L'\u00A7',	L'\u2022',	L'\u00B6',	L'\u00DF',	L'\u00AE',	L'\u00A9',	L'\u2122',	L'\u00B4',	L'\u00A8',	L'\u2260',	L'\u00C6',	L'\u00D8',	// 0xA0
		L'\u221E',	L'\u00B1',	L'\u2264',	L'\u2265',	L'\u00A5',	L'\u00B5',	L'\u2202',	L'\u2211',	L'\u220F',	L'\u03C0',	L'\u222B',	L'\u00AA',	L'\u00BA',	L'\u2126',	L'\u00E6',	L'\u00F8',	// 0xB0
		L'\u00BF',	L'\u00A1',	L'\u00AC',	L'\u221A',	L'\u0192',	L'\u2248',	L'\u2206',	L'\u00AB',	L'\u00BB',	L'\u2026',	L'\u00A0',	L'\u00C0',	L'\u00C3',	L'\u00D5',	L'\u0152',	L'\u0153',	// 0xC0
		L'\u2013',	L'\u2014',	L'\u201C',	L'\u201D',	L'\u2018',	L'\u2019',	L'\u00F7',	L'\u25CA',	L'\u00FF',	L'\u0178',	L'\u2044',	L'\u20AC',	L'\u2039',	L'\u203A',	L'\uFB01',	L'\uFB02',	// 0xD0
		L'\u2021',	L'\u00B7',	L'\u201A',	L'\u201E',	L'\u2030',	L'\u00C2',	L'\u00CA',	L'\u00C1',	L'\u00CB',	L'\u00C8',	L'\u00CD',	L'\u00CE',	L'\u00CF',	L'\u00CC',	L'\u00D3',	L'\u00D4',	// 0xE0
		L'\uF8FF',	L'\u00D2',	L'\u00DA',	L'\u00DB',	L'\u00D9',	L'\u0131',	L'\u02C6',	L'\u02DC',	L'\u00AF',	L'\u02D8',	L'\u02D9',	L'\u02DA',	L'\u00B8',	L'\u02DD',	L'\u02DB',	L'\u02C7',	// 0xF0
	};

	const wchar_t CCharSets::m_wcArabicWindows[] = {							/** Arabic Windows characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u20AC',	L'\u067E',	L'\u201A',	L'\u0192',	L'\u201E',	L'\u2026',	L'\u2020',	L'\u2021',	L'\u02C6',	L'\u2030',	L'\u0679',	L'\u2039',	L'\u0152',	L'\u0686',	L'\u0698',	L'\u0688',	// 0x80
		L'\u06AF',	L'\u2018',	L'\u2019',	L'\u201C',	L'\u201D',	L'\u2022',	L'\u2013',	L'\u2014',	L'\u06A9',	L'\u2122',	L'\u0691',	L'\u203A',	L'\u0153',	L'\u200C',	L'\u200D',	L'\u06BA',	// 0x90
		L'\u00A0',	L'\u060C',	L'\u00A2',	L'\u00A3',	L'\u00A4',	L'\u00A5',	L'\u00A6',	L'\u00A7',	L'\u00A8',	L'\u00A9',	L'\u06BE',	L'\u00AB',	L'\u00AC',	L'\u00AD',	L'\u00AE',	L'\u00AF',	// 0xA0
		L'\u00B0',	L'\u00B1',	L'\u00B2',	L'\u00B3',	L'\u00B4',	L'\u00B5',	L'\u00B6',	L'\u00B7',	L'\u00B8',	L'\u00B9',	L'\u061B',	L'\u00BB',	L'\u00BC',	L'\u00BD',	L'\u00BE',	L'\u061F',	// 0xB0
		L'\u06C1',	L'\u0621',	L'\u0622',	L'\u0623',	L'\u0624',	L'\u0625',	L'\u0626',	L'\u0627',	L'\u0628',	L'\u0629',	L'\u062A',	L'\u062B',	L'\u062C',	L'\u062D',	L'\u062E',	L'\u062F',	// 0xC0
		L'\u0630',	L'\u0631',	L'\u0632',	L'\u0633',	L'\u0634',	L'\u0635',	L'\u0636',	L'\u00D7',	L'\u0637',	L'\u0638',	L'\u0639',	L'\u063A',	L'\u0640',	L'\u0641',	L'\u0642',	L'\u0643',	// 0xD0
		L'\u00E0',	L'\u0644',	L'\u00E2',	L'\u0645',	L'\u0646',	L'\u0647',	L'\u0648',	L'\u00E7',	L'\u00E8',	L'\u00E9',	L'\u00EA',	L'\u00EB',	L'\u0649',	L'\u064A',	L'\u00EE',	L'\u00EF',	// 0xE0
		L'\u064B',	L'\u064C',	L'\u064D',	L'\u064E',	L'\u00F4',	L'\u064F',	L'\u0650',	L'\u00F7',	L'\u0651',	L'\u00F9',	L'\u0652',	L'\u00FB',	L'\u00FC',	L'\u200E',	L'\u200F',	L'\u06D2',	// 0xF0
	};

	const wchar_t CCharSets::m_wcArabicIso[] = {								/** Arabic Iso characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u0080',	L'\u0081',	L'\u0082',	L'\u0083',	L'\u0084',	L'\u0085',	L'\u0086',	L'\u0087',	L'\u0088',	L'\u0089',	L'\u008A',	L'\u008B',	L'\u008C',	L'\u008D',	L'\u008E',	L'\u008F',	// 0x80
		L'\u0090',	L'\u0091',	L'\u0092',	L'\u0093',	L'\u0094',	L'\u0095',	L'\u0096',	L'\u0097',	L'\u0098',	L'\u0099',	L'\u009A',	L'\u009B',	L'\u009C',	L'\u009D',	L'\u009E',	L'\u009F',	// 0x90
		L'\u00A0',	L'\uF7C8',	L'\uF7C9',	L'\uF7CA',	L'\u00A4',	L'\uF7CB',	L'\uF7CC',	L'\uF7CD',	L'\uF7CE',	L'\uF7CF',	L'\uF7D0',	L'\uF7D1',	L'\u060C',	L'\u00AD',	L'\uF7D2',	L'\uF7D3',	// 0xA0
		L'\uF7D4',	L'\uF7D5',	L'\uF7D6',	L'\uF7D7',	L'\uF7D8',	L'\uF7D9',	L'\uF7DA',	L'\uF7DB',	L'\uF7DC',	L'\uF7DD',	L'\uF7DE',	L'\u061B',	L'\uF7DF',	L'\uF7E0',	L'\uF7E1',	L'\u061F',	// 0xB0
		L'\uF7E2',	L'\u0621',	L'\u0622',	L'\u0623',	L'\u0624',	L'\u0625',	L'\u0626',	L'\u0627',	L'\u0628',	L'\u0629',	L'\u062A',	L'\u062B',	L'\u062C',	L'\u062D',	L'\u062E',	L'\u062F',	// 0xC0
		L'\u0630',	L'\u0631',	L'\u0632',	L'\u0633',	L'\u0634',	L'\u0635',	L'\u0636',	L'\u0637',	L'\u0638',	L'\u0639',	L'\u063A',	L'\uF7E3',	L'\uF7E4',	L'\uF7E5',	L'\uF7E6',	L'\uF7E7',	// 0xD0
		L'\u0640',	L'\u0641',	L'\u0642',	L'\u0643',	L'\u0644',	L'\u0645',	L'\u0646',	L'\u0647',	L'\u0648',	L'\u0649',	L'\u064A',	L'\u064B',	L'\u064C',	L'\u064D',	L'\u064E',	L'\u064F',	// 0xE0
		L'\u0650',	L'\u0651',	L'\u0652',	L'\uF7E8',	L'\uF7E9',	L'\uF7EA',	L'\uF7EB',	L'\uF7EC',	L'\uF7ED',	L'\uF7EE',	L'\uF7EF',	L'\uF7F0',	L'\uF7F1',	L'\uF7F2',	L'\uF7F3',	L'\uF7F4',	// 0xF0
	};

	const wchar_t CCharSets::m_wcBalticWindows[] = {							/** Baltic Windows characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u20AC',	L'\u0081',	L'\u201A',	L'\u0083',	L'\u201E',	L'\u2026',	L'\u2020',	L'\u2021',	L'\u0088',	L'\u2030',	L'\u008A',	L'\u2039',	L'\u008C',	L'\u00A8',	L'\u02C7',	L'\u00B8',	// 0x80
		L'\u0090',	L'\u2018',	L'\u2019',	L'\u201C',	L'\u201D',	L'\u2022',	L'\u2013',	L'\u2014',	L'\u0098',	L'\u2122',	L'\u009A',	L'\u203A',	L'\u009C',	L'\u00AF',	L'\u02DB',	L'\u009F',	// 0x90
		L'\u00A0',	L'\uF8FC',	L'\u00A2',	L'\u00A3',	L'\u00A4',	L'\uF8FD',	L'\u00A6',	L'\u00A7',	L'\u00D8',	L'\u00A9',	L'\u0156',	L'\u00AB',	L'\u00AC',	L'\u00AD',	L'\u00AE',	L'\u00C6',	// 0xA0
		L'\u00B0',	L'\u00B1',	L'\u00B2',	L'\u00B3',	L'\u00B4',	L'\u00B5',	L'\u00B6',	L'\u00B7',	L'\u00F8',	L'\u00B9',	L'\u0157',	L'\u00BB',	L'\u00BC',	L'\u00BD',	L'\u00BE',	L'\u00E6',	// 0xB0
		L'\u0104',	L'\u012E',	L'\u0100',	L'\u0106',	L'\u00C4',	L'\u00C5',	L'\u0118',	L'\u0112',	L'\u010C',	L'\u00C9',	L'\u0179',	L'\u0116',	L'\u0122',	L'\u0136',	L'\u012A',	L'\u013B',	// 0xC0
		L'\u0160',	L'\u0143',	L'\u0145',	L'\u00D3',	L'\u014C',	L'\u00D5',	L'\u00D6',	L'\u00D7',	L'\u0172',	L'\u0141',	L'\u015A',	L'\u016A',	L'\u00DC',	L'\u017B',	L'\u017D',	L'\u00DF',	// 0xD0
		L'\u0105',	L'\u012F',	L'\u0101',	L'\u0107',	L'\u00E4',	L'\u00E5',	L'\u0119',	L'\u0113',	L'\u010D',	L'\u00E9',	L'\u017A',	L'\u0117',	L'\u0123',	L'\u0137',	L'\u012B',	L'\u013C',	// 0xE0
		L'\u0161',	L'\u0144',	L'\u0146',	L'\u00F3',	L'\u014D',	L'\u00F5',	L'\u00F6',	L'\u00F7',	L'\u0173',	L'\u0142',	L'\u015B',	L'\u016B',	L'\u00FC',	L'\u017C',	L'\u017E',	L'\u02D9',	// 0xF0
	};

	const wchar_t CCharSets::m_wcBalticIso[] = {								/** Baltic ISO characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u0080',	L'\u0081',	L'\u0082',	L'\u0083',	L'\u0084',	L'\u0085',	L'\u0086',	L'\u0087',	L'\u0088',	L'\u0089',	L'\u008A',	L'\u008B',	L'\u008C',	L'\u008D',	L'\u008E',	L'\u008F',	// 0x80
		L'\u0090',	L'\u0091',	L'\u0092',	L'\u0093',	L'\u0094',	L'\u0095',	L'\u0096',	L'\u0097',	L'\u0098',	L'\u0099',	L'\u009A',	L'\u009B',	L'\u009C',	L'\u009D',	L'\u009E',	L'\u009F',	// 0x90
		L'\u00A0',	L'\u201D',	L'\u00A2',	L'\u00A3',	L'\u00A4',	L'\u201E',	L'\u00A6',	L'\u00A7',	L'\u00D8',	L'\u00A9',	L'\u0156',	L'\u00AB',	L'\u00AC',	L'\u00AD',	L'\u00AE',	L'\u00C6',	// 0xA0
		L'\u00B0',	L'\u00B1',	L'\u00B2',	L'\u00B3',	L'\u201C',	L'\u00B5',	L'\u00B6',	L'\u00B7',	L'\u00F8',	L'\u00B9',	L'\u0157',	L'\u00BB',	L'\u00BC',	L'\u00BD',	L'\u00BE',	L'\u00E6',	// 0xB0
		L'\u0104',	L'\u012E',	L'\u0100',	L'\u0106',	L'\u00C4',	L'\u00C5',	L'\u0118',	L'\u0112',	L'\u010C',	L'\u00C9',	L'\u0179',	L'\u0116',	L'\u0122',	L'\u0136',	L'\u012A',	L'\u013B',	// 0xC0
		L'\u0160',	L'\u0143',	L'\u0145',	L'\u00D3',	L'\u014C',	L'\u00D5',	L'\u00D6',	L'\u00D7',	L'\u0172',	L'\u0141',	L'\u015A',	L'\u016A',	L'\u00DC',	L'\u017B',	L'\u017D',	L'\u00DF',	// 0xD0
		L'\u0105',	L'\u012F',	L'\u0101',	L'\u0107',	L'\u00E4',	L'\u00E5',	L'\u0119',	L'\u0113',	L'\u010D',	L'\u00E9',	L'\u017A',	L'\u0117',	L'\u0123',	L'\u0137',	L'\u012B',	L'\u013C',	// 0xE0
		L'\u0161',	L'\u0144',	L'\u0146',	L'\u00F3',	L'\u014D',	L'\u00F5',	L'\u00F6',	L'\u00F7',	L'\u0173',	L'\u0142',	L'\u015B',	L'\u016B',	L'\u00FC',	L'\u017C',	L'\u017E',	L'\u2019',	// 0xF0
	};

	const wchar_t CCharSets::m_wcCyrillicWindows[] = {							/** Cyrillic Windows characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u0402',	L'\u0403',	L'\u201A',	L'\u0453',	L'\u201E',	L'\u2026',	L'\u2020',	L'\u2021',	L'\u20AC',	L'\u2030',	L'\u0409',	L'\u2039',	L'\u040A',	L'\u040C',	L'\u040B',	L'\u040F',	// 0x80
		L'\u0452',	L'\u2018',	L'\u2019',	L'\u201C',	L'\u201D',	L'\u2022',	L'\u2013',	L'\u2014',	L'\u0098',	L'\u2122',	L'\u0459',	L'\u203A',	L'\u045A',	L'\u045C',	L'\u045B',	L'\u045F',	// 0x90
		L'\u00A0',	L'\u040E',	L'\u045E',	L'\u0408',	L'\u00A4',	L'\u0490',	L'\u00A6',	L'\u00A7',	L'\u0401',	L'\u00A9',	L'\u0404',	L'\u00AB',	L'\u00AC',	L'\u00AD',	L'\u00AE',	L'\u0407',	// 0xA0
		L'\u00B0',	L'\u00B1',	L'\u0406',	L'\u0456',	L'\u0491',	L'\u00B5',	L'\u00B6',	L'\u00B7',	L'\u0451',	L'\u2116',	L'\u0454',	L'\u00BB',	L'\u0458',	L'\u0405',	L'\u0455',	L'\u0457',	// 0xB0
		L'\u0410',	L'\u0411',	L'\u0412',	L'\u0413',	L'\u0414',	L'\u0415',	L'\u0416',	L'\u0417',	L'\u0418',	L'\u0419',	L'\u041A',	L'\u041B',	L'\u041C',	L'\u041D',	L'\u041E',	L'\u041F',	// 0xC0
		L'\u0420',	L'\u0421',	L'\u0422',	L'\u0423',	L'\u0424',	L'\u0425',	L'\u0426',	L'\u0427',	L'\u0428',	L'\u0429',	L'\u042A',	L'\u042B',	L'\u042C',	L'\u042D',	L'\u042E',	L'\u042F',	// 0xD0
		L'\u0430',	L'\u0431',	L'\u0432',	L'\u0433',	L'\u0434',	L'\u0435',	L'\u0436',	L'\u0437',	L'\u0438',	L'\u0439',	L'\u043A',	L'\u043B',	L'\u043C',	L'\u043D',	L'\u043E',	L'\u043F',	// 0xE0
		L'\u0440',	L'\u0441',	L'\u0442',	L'\u0443',	L'\u0444',	L'\u0445',	L'\u0446',	L'\u0447',	L'\u0448',	L'\u0449',	L'\u044A',	L'\u044B',	L'\u044C',	L'\u044D',	L'\u044E',	L'\u044F',	// 0xF0
	};

	const wchar_t CCharSets::m_wcCyrillicKoi8R[] = {							/** Cyrillic KOI8 characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u2500',	L'\u2502',	L'\u250C',	L'\u2510',	L'\u2514',	L'\u2518',	L'\u251C',	L'\u2524',	L'\u252C',	L'\u2534',	L'\u253C',	L'\u2580',	L'\u2584',	L'\u2588',	L'\u258C',	L'\u2590',	// 0x80
		L'\u2591',	L'\u2592',	L'\u2593',	L'\u2320',	L'\u25A0',	L'\u2219',	L'\u221A',	L'\u2248',	L'\u2264',	L'\u2265',	L'\u00A0',	L'\u2321',	L'\u00B0',	L'\u00B2',	L'\u00B7',	L'\u00F7',	// 0x90
		L'\u2550',	L'\u2551',	L'\u2552',	L'\u0451',	L'\u2553',	L'\u2554',	L'\u2555',	L'\u2556',	L'\u2557',	L'\u2558',	L'\u2559',	L'\u255A',	L'\u255B',	L'\u255C',	L'\u255D',	L'\u255E',	// 0xA0
		L'\u255F',	L'\u2560',	L'\u2561',	L'\u0401',	L'\u2562',	L'\u2563',	L'\u2564',	L'\u2565',	L'\u2566',	L'\u2567',	L'\u2568',	L'\u2569',	L'\u256A',	L'\u256B',	L'\u256C',	L'\u00A9',	// 0xB0
		L'\u044E',	L'\u0430',	L'\u0431',	L'\u0446',	L'\u0434',	L'\u0435',	L'\u0444',	L'\u0433',	L'\u0445',	L'\u0438',	L'\u0439',	L'\u043A',	L'\u043B',	L'\u043C',	L'\u043D',	L'\u043E',	// 0xC0
		L'\u043F',	L'\u044F',	L'\u0440',	L'\u0441',	L'\u0442',	L'\u0443',	L'\u0436',	L'\u0432',	L'\u044C',	L'\u044B',	L'\u0437',	L'\u0448',	L'\u044D',	L'\u0449',	L'\u0447',	L'\u044A',	// 0xD0
		L'\u042E',	L'\u0410',	L'\u0411',	L'\u0426',	L'\u0414',	L'\u0415',	L'\u0424',	L'\u0413',	L'\u0425',	L'\u0418',	L'\u0419',	L'\u041A',	L'\u041B',	L'\u041C',	L'\u041D',	L'\u041E',	// 0xE0
		L'\u041F',	L'\u042F',	L'\u0420',	L'\u0421',	L'\u0422',	L'\u0423',	L'\u0416',	L'\u0412',	L'\u042C',	L'\u042B',	L'\u0417',	L'\u0428',	L'\u042D',	L'\u0429',	L'\u0427',	L'\u042A',	// 0xF0
	};

	const wchar_t CCharSets::m_wcCyrillicKoi8U[] = {							/** Cyrillic KOI8-U characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u2500',	L'\u2502',	L'\u250C',	L'\u2510',	L'\u2514',	L'\u2518',	L'\u251C',	L'\u2524',	L'\u252C',	L'\u2534',	L'\u253C',	L'\u2580',	L'\u2584',	L'\u2588',	L'\u258C',	L'\u2590',	// 0x80
		L'\u2591',	L'\u2592',	L'\u2593',	L'\u2320',	L'\u25A0',	L'\u2219',	L'\u221A',	L'\u2248',	L'\u2264',	L'\u2265',	L'\u00A0',	L'\u2321',	L'\u00B0',	L'\u00B2',	L'\u00B7',	L'\u00F7',	// 0x90
		L'\u2550',	L'\u2551',	L'\u2552',	L'\u0451',	L'\u0454',	L'\u2554',	L'\u0456',	L'\u0457',	L'\u2557',	L'\u2558',	L'\u2559',	L'\u255A',	L'\u255B',	L'\u0491',	L'\u045E',	L'\u255E',	// 0xA0
		L'\u255F',	L'\u2560',	L'\u2561',	L'\u0401',	L'\u0404',	L'\u2563',	L'\u0406',	L'\u0407',	L'\u2566',	L'\u2567',	L'\u2568',	L'\u2569',	L'\u256A',	L'\u0490',	L'\u040E',	L'\u00A9',	// 0xB0
		L'\u044E',	L'\u0430',	L'\u0431',	L'\u0446',	L'\u0434',	L'\u0435',	L'\u0444',	L'\u0433',	L'\u0445',	L'\u0438',	L'\u0439',	L'\u043A',	L'\u043B',	L'\u043C',	L'\u043D',	L'\u043E',	// 0xC0
		L'\u043F',	L'\u044F',	L'\u0440',	L'\u0441',	L'\u0442',	L'\u0443',	L'\u0436',	L'\u0432',	L'\u044C',	L'\u044B',	L'\u0437',	L'\u0448',	L'\u044D',	L'\u0449',	L'\u0447',	L'\u044A',	// 0xD0
		L'\u042E',	L'\u0410',	L'\u0411',	L'\u0426',	L'\u0414',	L'\u0415',	L'\u0424',	L'\u0413',	L'\u0425',	L'\u0418',	L'\u0419',	L'\u041A',	L'\u041B',	L'\u041C',	L'\u041D',	L'\u041E',	// 0xE0
		L'\u041F',	L'\u042F',	L'\u0420',	L'\u0421',	L'\u0422',	L'\u0423',	L'\u0416',	L'\u0412',	L'\u042C',	L'\u042B',	L'\u0417',	L'\u0428',	L'\u042D',	L'\u0429',	L'\u0427',	L'\u042A',	// 0xF0
	};

	const wchar_t CCharSets::m_wcCyrillicIso[] = {								/** Cyrillic ISO characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0xF0
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0xF0
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0xF0
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0xF0
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0xF0
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0xF0
		L'\u0080',	L'\u0081',	L'\u0082',	L'\u0083',	L'\u0084',	L'\u0085',	L'\u0086',	L'\u0087',	L'\u0088',	L'\u0089',	L'\u008A',	L'\u008B',	L'\u008C',	L'\u008D',	L'\u008E',	L'\u008F',	// 0xF0
		L'\u0090',	L'\u0091',	L'\u0092',	L'\u0093',	L'\u0094',	L'\u0095',	L'\u0096',	L'\u0097',	L'\u0098',	L'\u0099',	L'\u009A',	L'\u009B',	L'\u009C',	L'\u009D',	L'\u009E',	L'\u009F',	// 0xF0
		L'\u00A0',	L'\u0401',	L'\u0402',	L'\u0403',	L'\u0404',	L'\u0405',	L'\u0406',	L'\u0407',	L'\u0408',	L'\u0409',	L'\u040A',	L'\u040B',	L'\u040C',	L'\u00AD',	L'\u040E',	L'\u040F',	// 0xF0
		L'\u0410',	L'\u0411',	L'\u0412',	L'\u0413',	L'\u0414',	L'\u0415',	L'\u0416',	L'\u0417',	L'\u0418',	L'\u0419',	L'\u041A',	L'\u041B',	L'\u041C',	L'\u041D',	L'\u041E',	L'\u041F',	// 0xF0
		L'\u0420',	L'\u0421',	L'\u0422',	L'\u0423',	L'\u0424',	L'\u0425',	L'\u0426',	L'\u0427',	L'\u0428',	L'\u0429',	L'\u042A',	L'\u042B',	L'\u042C',	L'\u042D',	L'\u042E',	L'\u042F',	// 0xF0
		L'\u0430',	L'\u0431',	L'\u0432',	L'\u0433',	L'\u0434',	L'\u0435',	L'\u0436',	L'\u0437',	L'\u0438',	L'\u0439',	L'\u043A',	L'\u043B',	L'\u043C',	L'\u043D',	L'\u043E',	L'\u043F',	// 0xF0
		L'\u0440',	L'\u0441',	L'\u0442',	L'\u0443',	L'\u0444',	L'\u0445',	L'\u0446',	L'\u0447',	L'\u0448',	L'\u0449',	L'\u044A',	L'\u044B',	L'\u044C',	L'\u044D',	L'\u044E',	L'\u044F',	// 0xF0
		L'\u2116',	L'\u0451',	L'\u0452',	L'\u0453',	L'\u0454',	L'\u0455',	L'\u0456',	L'\u0457',	L'\u0458',	L'\u0459',	L'\u045A',	L'\u045B',	L'\u045C',	L'\u00A7',	L'\u045E',	L'\u045F',	// 0xF0
	};

	const wchar_t CCharSets::m_wcEasternEuropeWindows[] = {						/** Eastern Europe Windows characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u20AC',	L'\u0081',	L'\u201A',	L'\u0083',	L'\u201E',	L'\u2026',	L'\u2020',	L'\u2021',	L'\u0088',	L'\u2030',	L'\u0160',	L'\u2039',	L'\u015A',	L'\u0164',	L'\u017D',	L'\u0179',	// 0x80
		L'\u0090',	L'\u2018',	L'\u2019',	L'\u201C',	L'\u201D',	L'\u2022',	L'\u2013',	L'\u2014',	L'\u0098',	L'\u2122',	L'\u0161',	L'\u203A',	L'\u015B',	L'\u0165',	L'\u017E',	L'\u017A',	// 0x90
		L'\u00A0',	L'\u02C7',	L'\u02D8',	L'\u0141',	L'\u00A4',	L'\u0104',	L'\u00A6',	L'\u00A7',	L'\u00A8',	L'\u00A9',	L'\u015E',	L'\u00AB',	L'\u00AC',	L'\u00AD',	L'\u00AE',	L'\u017B',	// 0xA0
		L'\u00B0',	L'\u00B1',	L'\u02DB',	L'\u0142',	L'\u00B4',	L'\u00B5',	L'\u00B6',	L'\u00B7',	L'\u00B8',	L'\u0105',	L'\u015F',	L'\u00BB',	L'\u013D',	L'\u02DD',	L'\u013E',	L'\u017C',	// 0xB0
		L'\u0154',	L'\u00C1',	L'\u00C2',	L'\u0102',	L'\u00C4',	L'\u0139',	L'\u0106',	L'\u00C7',	L'\u010C',	L'\u00C9',	L'\u0118',	L'\u00CB',	L'\u011A',	L'\u00CD',	L'\u00CE',	L'\u010E',	// 0xC0
		L'\u0110',	L'\u0143',	L'\u0147',	L'\u00D3',	L'\u00D4',	L'\u0150',	L'\u00D6',	L'\u00D7',	L'\u0158',	L'\u016E',	L'\u00DA',	L'\u0170',	L'\u00DC',	L'\u00DD',	L'\u0162',	L'\u00DF',	// 0xD0
		L'\u0155',	L'\u00E1',	L'\u00E2',	L'\u0103',	L'\u00E4',	L'\u013A',	L'\u0107',	L'\u00E7',	L'\u010D',	L'\u00E9',	L'\u0119',	L'\u00EB',	L'\u011B',	L'\u00ED',	L'\u00EE',	L'\u010F',	// 0xE0
		L'\u0111',	L'\u0144',	L'\u0148',	L'\u00F3',	L'\u00F4',	L'\u0151',	L'\u00F6',	L'\u00F7',	L'\u0159',	L'\u016F',	L'\u00FA',	L'\u0171',	L'\u00FC',	L'\u00FD',	L'\u0163',	L'\u02D9',	// 0xF0
	};

	const wchar_t CCharSets::m_wcEasternEuropeISO[] = {							/** Eastern Europe ISO characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u0080',	L'\u0081',	L'\u0082',	L'\u0083',	L'\u0084',	L'\u0085',	L'\u0086',	L'\u0087',	L'\u0088',	L'\u0089',	L'\u008A',	L'\u008B',	L'\u008C',	L'\u008D',	L'\u008E',	L'\u008F',	// 0x80
		L'\u0090',	L'\u0091',	L'\u0092',	L'\u0093',	L'\u0094',	L'\u0095',	L'\u0096',	L'\u0097',	L'\u0098',	L'\u0099',	L'\u009A',	L'\u009B',	L'\u009C',	L'\u009D',	L'\u009E',	L'\u009F',	// 0x90
		L'\u00A0',	L'\u0104',	L'\u02D8',	L'\u0141',	L'\u00A4',	L'\u013D',	L'\u015A',	L'\u00A7',	L'\u00A8',	L'\u0160',	L'\u015E',	L'\u0164',	L'\u0179',	L'\u00AD',	L'\u017D',	L'\u017B',	// 0xA0
		L'\u00B0',	L'\u0105',	L'\u02DB',	L'\u0142',	L'\u00B4',	L'\u013E',	L'\u015B',	L'\u02C7',	L'\u00B8',	L'\u0161',	L'\u015F',	L'\u0165',	L'\u017A',	L'\u02DD',	L'\u017E',	L'\u017C',	// 0xB0
		L'\u0154',	L'\u00C1',	L'\u00C2',	L'\u0102',	L'\u00C4',	L'\u0139',	L'\u0106',	L'\u00C7',	L'\u010C',	L'\u00C9',	L'\u0118',	L'\u00CB',	L'\u011A',	L'\u00CD',	L'\u00CE',	L'\u010E',	// 0xC0
		L'\u0110',	L'\u0143',	L'\u0147',	L'\u00D3',	L'\u00D4',	L'\u0150',	L'\u00D6',	L'\u00D7',	L'\u0158',	L'\u016E',	L'\u00DA',	L'\u0170',	L'\u00DC',	L'\u00DD',	L'\u0162',	L'\u00DF',	// 0xD0
		L'\u0155',	L'\u00E1',	L'\u00E2',	L'\u0103',	L'\u00E4',	L'\u013A',	L'\u0107',	L'\u00E7',	L'\u010D',	L'\u00E9',	L'\u0119',	L'\u00EB',	L'\u011B',	L'\u00ED',	L'\u00EE',	L'\u010F',	// 0xE0
		L'\u0111',	L'\u0144',	L'\u0148',	L'\u00F3',	L'\u00F4',	L'\u0151',	L'\u00F6',	L'\u00F7',	L'\u0159',	L'\u016F',	L'\u00FA',	L'\u0171',	L'\u00FC',	L'\u00FD',	L'\u0163',	L'\u02D9',	// 0xF0
	};

	const wchar_t CCharSets::m_wcGreekWindows[] = {								/** Greek Windows characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u20AC',	L'\u0081',	L'\u201A',	L'\u0192',	L'\u201E',	L'\u2026',	L'\u2020',	L'\u2021',	L'\u0088',	L'\u2030',	L'\u008A',	L'\u2039',	L'\u008C',	L'\u008D',	L'\u008E',	L'\u008F',	// 0x80
		L'\u0090',	L'\u2018',	L'\u2019',	L'\u201C',	L'\u201D',	L'\u2022',	L'\u2013',	L'\u2014',	L'\u0098',	L'\u2122',	L'\u009A',	L'\u203A',	L'\u009C',	L'\u009D',	L'\u009E',	L'\u009F',	// 0x90
		L'\u00A0',	L'\u0385',	L'\u0386',	L'\u00A3',	L'\u00A4',	L'\u00A5',	L'\u00A6',	L'\u00A7',	L'\u00A8',	L'\u00A9',	L'\uF8F9',	L'\u00AB',	L'\u00AC',	L'\u00AD',	L'\u00AE',	L'\u2015',	// 0xA0
		L'\u00B0',	L'\u00B1',	L'\u00B2',	L'\u00B3',	L'\u0384',	L'\u00B5',	L'\u00B6',	L'\u00B7',	L'\u0388',	L'\u0389',	L'\u038A',	L'\u00BB',	L'\u038C',	L'\u00BD',	L'\u038E',	L'\u038F',	// 0xB0
		L'\u0390',	L'\u0391',	L'\u0392',	L'\u0393',	L'\u0394',	L'\u0395',	L'\u0396',	L'\u0397',	L'\u0398',	L'\u0399',	L'\u039A',	L'\u039B',	L'\u039C',	L'\u039D',	L'\u039E',	L'\u039F',	// 0xC0
		L'\u03A0',	L'\u03A1',	L'\uF8FA',	L'\u03A3',	L'\u03A4',	L'\u03A5',	L'\u03A6',	L'\u03A7',	L'\u03A8',	L'\u03A9',	L'\u03AA',	L'\u03AB',	L'\u03AC',	L'\u03AD',	L'\u03AE',	L'\u03AF',	// 0xD0
		L'\u03B0',	L'\u03B1',	L'\u03B2',	L'\u03B3',	L'\u03B4',	L'\u03B5',	L'\u03B6',	L'\u03B7',	L'\u03B8',	L'\u03B9',	L'\u03BA',	L'\u03BB',	L'\u03BC',	L'\u03BD',	L'\u03BE',	L'\u03BF',	// 0xE0
		L'\u03C0',	L'\u03C1',	L'\u03C2',	L'\u03C3',	L'\u03C4',	L'\u03C5',	L'\u03C6',	L'\u03C7',	L'\u03C8',	L'\u03C9',	L'\u03CA',	L'\u03CB',	L'\u03CC',	L'\u03CD',	L'\u03CE',	L'\uF8FB',	// 0xF0
	};

	const wchar_t CCharSets::m_wcGreekIso[] = {									/** Greek ISO characters. */
		L'\x20',	L'\x21',	L'\x22',	L'\x23',	L'\x24',	L'\x25',	L'\x26',	L'\x27',	L'\x28',	L'\x29',	L'\x2A',	L'\x2B',	L'\x2C',	L'\x2D',	L'\x2E',	L'\x2F',	// 0x20
		L'\x30',	L'\x31',	L'\x32',	L'\x33',	L'\x34',	L'\x35',	L'\x36',	L'\x37',	L'\x38',	L'\x39',	L'\x3A',	L'\x3B',	L'\x3C',	L'\x3D',	L'\x3E',	L'\x3F',	// 0x30
		L'\x40',	L'\x41',	L'\x42',	L'\x43',	L'\x44',	L'\x45',	L'\x46',	L'\x47',	L'\x48',	L'\x49',	L'\x4A',	L'\x4B',	L'\x4C',	L'\x4D',	L'\x4E',	L'\x4F',	// 0x40
		L'\x50',	L'\x51',	L'\x52',	L'\x53',	L'\x54',	L'\x55',	L'\x56',	L'\x57',	L'\x58',	L'\x59',	L'\x5A',	L'\x5B',	L'\x5C',	L'\x5D',	L'\x5E',	L'\x5F',	// 0x50
		L'\x60',	L'\x61',	L'\x62',	L'\x63',	L'\x64',	L'\x65',	L'\x66',	L'\x67',	L'\x68',	L'\x69',	L'\x6A',	L'\x6B',	L'\x6C',	L'\x6D',	L'\x6E',	L'\x6F',	// 0x60
		L'\x70',	L'\x71',	L'\x72',	L'\x73',	L'\x74',	L'\x75',	L'\x76',	L'\x77',	L'\x78',	L'\x79',	L'\x7A',	L'\x7B',	L'\x7C',	L'\x7D',	L'\x7E',	L'\x7F',	// 0x70
		L'\u0080',	L'\u0081',	L'\u0082',	L'\u0083',	L'\u0084',	L'\u0085',	L'\u0086',	L'\u0087',	L'\u0088',	L'\u0089',	L'\u008A',	L'\u008B',	L'\u008C',	L'\u008D',	L'\u008E',	L'\u008F',	// 0x80
		L'\u0090',	L'\u0091',	L'\u0092',	L'\u0093',	L'\u0094',	L'\u0095',	L'\u0096',	L'\u0097',	L'\u0098',	L'\u0099',	L'\u009A',	L'\u009B',	L'\u009C',	L'\u009D',	L'\u009E',	L'\u009F',	// 0x90
		L'\u00A0',	L'\u02BD',	L'\u02BC',	L'\u00A3',	L'\uF7C2',	L'\uF7C3',	L'\u00A6',	L'\u00A7',	L'\u00A8',	L'\u00A9',	L'\uF7C4',	L'\u00AB',	L'\u00AC',	L'\u00AD',	L'\uF7C5',	L'\u2015',	// 0xA0
		L'\u00B0',	L'\u00B1',	L'\u00B2',	L'\u00B3',	L'\u0384',	L'\u0385',	L'\u0386',	L'\u00B7',	L'\u0388',	L'\u0389',	L'\u038A',	L'\u00BB',	L'\u038C',	L'\u00BD',	L'\u038E',	L'\u038F',	// 0xB0
		L'\u0390',	L'\u0391',	L'\u0392',	L'\u0393',	L'\u0394',	L'\u0395',	L'\u0396',	L'\u0397',	L'\u0398',	L'\u0399',	L'\u039A',	L'\u039B',	L'\u039C',	L'\u039D',	L'\u039E',	L'\u039F',	// 0xC0
		L'\u03A0',	L'\u03A1',	L'\uF7C6',	L'\u03A3',	L'\u03A4',	L'\u03A5',	L'\u03A6',	L'\u03A7',	L'\u03A8',	L'\u03A9',	L'\u03AA',	L'\u03AB',	L'\u03AC',	L'\u03AD',	L'\u03AE',	L'\u03AF',	// 0xD0
		L'\u03B0',	L'\u03B1',	L'\u03B2',	L'\u03B3',	L'\u03B4',	L'\u03B5',	L'\u03B6',	L'\u03B7',	L'\u03B8',	L'\u03B9',	L'\u03BA',	L'\u03BB',	L'\u03BC',	L'\u03BD',	L'\u03BE',	L'\u03BF',	// 0xE0
		L'\u03C0',	L'\u03C1',	L'\u03C2',	L'\u03C3',	L'\u03C4',	L'\u03C5',	L'\u03C6',	L'\u03C7',	L'\u03C8',	L'\u03C9',	L'\u03CA',	L'\u03CB',	L'\u03CC',	L'\u03CD',	L'\u03CE',	L'\uF7C7',	// 0xF0
	};


	// == Functions.
	// Gets the total number of presets.
	size_t CCharSets::Total() { return MX_ELEMENTS( m_csSets ); }

}	// namespace mx

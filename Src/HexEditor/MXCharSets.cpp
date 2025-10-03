#include "MXCharSets.h"
#include "../Utilities/MXUtilities.h"


namespace mx {

	// == Members.
	const CCharSets::MX_CHAR_SET CCharSets::m_csSets[] = {										/** The predefined character sets. */
		//pcName										pcStatusBar								ui32Code							ui32Id	bHideOver127	bTopMenu
		{ _T_LEN_807BA8DC__ASCII,						_T_LEN_266E1849_ASCII,					CCodePages::MX_Windows_1252,		0,		true,			false },
		{ _T_LEN_470FBFA1_ASCII_A_NSI,					_T_LEN_E67EB72D_ANSI,					CCodePages::MX_Windows_1252,		1,		false,			true },
		{ _T_LEN_4A16753C_ASCII__OEM,					_T_LEN_01E49582_OEM,					CCodePages::MX_IBM437,				2,		false,			false },
		{ _T_LEN_7CFB7587__EBCDIC,						_T_LEN_A48AAB94_EBC,					CCodePages::MX_IBM037,				3,		true,			false },
		{ _T_LEN_DB5BE63A__Unicode,						_T_LEN_F4CCAEF6_UNI,					CCodePages::MX_utf_16,				4,		false,			true },
		{ _T_LEN_4524B804_UTF__8,						_T_LEN_3803F6FD_UTF8,					CCodePages::MX_utf_8,				5,		false,			true },
		{ _T_LEN_C15FE6DB__Macintosh,					_T_LEN_81B4A9EF_MAC,					CCodePages::MX_macintosh,			6,		false,			false },
		{ _T_LEN_70AA9B0F__Arabic__Windows_,			_T_LEN_074F7035_ARA,					CCodePages::MX_windows_1256,		7,		false,			false },
		{ _T_LEN_9A84344E_Arabic___ISO_,				_T_LEN_9CC705E1_ARA_I,					CCodePages::MX_iso_8859_6,			8,		false,			false },
		{ _T_LEN_AF65D397__Baltic__Windows_,			_T_LEN_1A57F343_BAL,					CCodePages::MX_windows_1257,		9,		false,			false },
		{ _T_LEN_57BDB155_Ba_ltic__ISO_,				_T_LEN_91131413_BAL_I,					CCodePages::MX_iso_8859_13,			10,		false,			false },
		{ _T_LEN_0E858F99_Chinese__Si_mplified_,		_T_LEN_0424AE16_CH_S,					CCodePages::MX_GB18030,				11,		false,			false },
		{ _T_LEN_5C9EB386__Chinese__Traditional_,		_T_LEN_9A403BB5_CH_T,					CCodePages::MX_big5,				12,		false,			false },
		{ _T_LEN_B322F1B2_Cy_rillic__Windows_,			_T_LEN_63813C4E_CYR,					CCodePages::MX_windows_1251,		13,		false,			false },
		{ _T_LEN_DA02E6A6_Cyrillic__K_OI8_R_,			_T_LEN_A5030545_CYR_R,					CCodePages::MX_koi8_r,				14,		false,			false },
		{ _T_LEN_46567AD6_Cyrillic__KOI_8_U_,			_T_LEN_3B6790E6_CYR_U,					CCodePages::MX_koi8_u,				15,		false,			false },
		{ _T_LEN_A8A4B69A_C_yrillic__ISO_,				_T_LEN_2F66CCA9_CYR_I,					CCodePages::MX_iso_8859_5,			16,		false,			false },
		{ _T_LEN_0B5B5DE3__Eastern_Europe__Windows_,	_T_LEN_CBB90FF0_EUR,					CCodePages::MX_windows_1250,		17,		false,			false },
		{ _T_LEN_832DEC00_Eastern_Euro_pe__ISO_,		_T_LEN_EAF086B1_EUR_I,					CCodePages::MX_iso_8859_2,			18,		false,			false },
		{ _T_LEN_3F987D96__Greek__Windows_,				_T_LEN_04AFC89E_GRE,					CCodePages::MX_windows_1253,		19,		false,			false },
		{ _T_LEN_9A8AEB42_Greek__I_SO_,					_T_LEN_148E589D_GRE_I,					CCodePages::MX_iso_8859_7,			20,		false,			false },
		{ _T_LEN_A1F17C4D__Hebrew__Windows_,			_T_LEN_94149E96_HEB,					CCodePages::MX_windows_1255,		21,		false,			false },
		{ _T_LEN_E9850994_Hebre_w__ISO_,				_T_LEN_5E5FB6EF_HEB_I,					CCodePages::MX_iso_8859_8,			22,		false,			false },
		{ _T_LEN_EAF36EB9__Japanese__Shift_JIS_,		_T_LEN_0045FEB4_JAP,					CCodePages::MX_shift_jis,			23,		false,			false },
		{ _T_LEN_14A26E2D_Japa_nese__EUC_JP_,			_T_LEN_BDE949ED_JAP_E,					CCodePages::MX_EUC_JP,				24,		false,			false },
		{ _T_LEN_163ABE8E__Korean,						_T_LEN_710AD821_KOR,					CCodePages::MX_euc_kr,				24,		false,			false },
		{ _T_LEN_C280ACCE__Thai,						_T_LEN_FDC01CE1_THAI,					CCodePages::MX_windows_874,			25,		false,			false },
		{ _T_LEN_9401B004_T_urkish__Windows_,			_T_LEN_66DF0C7C_TURK,					CCodePages::MX_windows_1254,		26,		false,			false },
		{ _T_LEN_B51B4222_Turkish__IS_O_,				_T_LEN_E4CFAD2A_TURK_I,					CCodePages::MX_iso_8859_9,			27,		false,			false },
		{ _T_LEN_E97EC64F__Vietnamese,					_T_LEN_51611780_VIET,					CCodePages::MX_windows_1258,		28,		false,			false },
	};

	// == Functions.
	// Gets the total number of presets.
	size_t CCharSets::Total() { return MX_ELEMENTS( m_csSets ); }

}	// namespace mx

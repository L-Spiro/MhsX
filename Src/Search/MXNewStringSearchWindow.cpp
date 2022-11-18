#include "MXNewStringSearchWindow.h"
#include "../Float16/MXFloat16.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXNewStringSearchLayout.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../RegEx/MXOnigurumaRegex.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"
#include <algorithm>
#include <Base/LSWBase.h>
#include <Button/LSWButton.h>
#include <CheckButton/LSWCheckButton.h>
#include <ComboBox/LSWComboBox.h>
#include <Static/LSWStatic.h>

#include <set>


namespace mx {

	// == Members.
	CNewStringSearchWindow::MX_SEARCH_WIN_DATA CNewStringSearchWindow::m_swdData = {
		MX_ST_UTF8,								// stSearchFormat
		CUtilities::MX_DT_UINT8,				// dtArrayCharType
		CUtilities::MX_RE_UTF8,					// reRegexEncoding
		CUtilities::MX_RF_ONIGURUMA,			// rfRegexFlavor
		0,										// uiUtfHelperIdx
		0,										// uiRegexHelperIdx
	};

	CNewStringSearchWindow::CNewStringSearchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmhMemHack( reinterpret_cast<CMemHack *>(_ui64Data) ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CNewStringSearchWindow::InitDialog() {
		CComboBox * pcbCombo;
		{
			pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TYPE_COMBO ));
			m_lOriginalTypeWidth = 0;
			if ( pcbCombo ) {
				static const struct {
					const char *					pcName;
					size_t							sLen;
					MX_STRING_TYPE					stType;
				} sCombo[] = {
					{ _T_LEN_468B510E_Machine_Code_Page,	MX_ST_ANSI },
					{ _T_LEN_0E813C50_UTF_8,				MX_ST_UTF8 },
					{ _T_LEN_A71F1195_UTF_16,				MX_ST_UTF16 },
					{ _T_LEN_26FC5333_UTF_16_BE,			MX_ST_UTF16_BE },
					{ _T_LEN_9244B70E_UTF_32,				MX_ST_UTF32 },
					{ _T_LEN_D35E9704_UTF_32_BE,			MX_ST_UTF32_BE },
					{ _T_LEN_FD384D18_UTF__,				MX_ST_UTFANY },
					{ _T_LEN_CF8E9225_Regular_Expression,	MX_ST_REGEX },
					{ _T_LEN_751205D5_Byte_Array,			MX_ST_BYTE_ARRAY },
				};
				for ( size_t I = 0; I < MX_ELEMENTS( sCombo ); ++I ) {
					INT iIndex = pcbCombo->AddString( CStringDecoder::DecodeToString( sCombo[I].pcName, sCombo[I].sLen ).c_str() );
					pcbCombo->SetItemData( iIndex, sCombo[I].stType );
				}
				pcbCombo->AutoSetMinListWidth();

				pcbCombo->SetCurSelByItemData( m_swdData.stSearchFormat );
				m_lOriginalTypeWidth = pcbCombo->ClientRect().Width();
			}
		}


		{
			pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_REGEX_FLAVOR_COMBO ));
			if ( pcbCombo ) {
				static const struct {
					const char *							pcName;
					size_t									sLen;
					CUtilities::MX_REGEX_FLAVORS			rfType;
				} sCombo[] = {
					{ _T_LEN_2968C625_Asis,					CUtilities::MX_RF_ASIS },
					{ _T_LEN_81899B3C_POSIX_Basic,			CUtilities::MX_RF_POSIX_BASIC },
					{ _T_LEN_1B76210D_POSIX_Extended,		CUtilities::MX_RF_POSIX_EXTENDED },
					{ _T_LEN_30473635_eMacs,				CUtilities::MX_RF_EMACS },
					{ _T_LEN_984F4246_grep,					CUtilities::MX_RF_GREP },
					{ _T_LEN_534B494A_GNU_Regex,			CUtilities::MX_RF_GNU_REGEX },
					{ _T_LEN_DF2849EC_Java,					CUtilities::MX_RF_JAVA },
					{ _T_LEN_FD58EF72_Perl,					CUtilities::MX_RF_PERL },
					{ _T_LEN_B785ED5A_Perl___Named_Groups,	CUtilities::MX_RF_PERL_NG },
					{ _T_LEN_6C687233_Ruby,					CUtilities::MX_RF_RUBY },
					{ _T_LEN_6144B512_Oniguruma,			CUtilities::MX_RF_ONIGURUMA },
				};
				for ( size_t I = 0; I < MX_ELEMENTS( sCombo ); ++I ) {
					INT iIndex = pcbCombo->AddString( CStringDecoder::DecodeToString( sCombo[I].pcName, sCombo[I].sLen ).c_str() );
					pcbCombo->SetItemData( iIndex, sCombo[I].rfType );
				}
				pcbCombo->AutoSetMinListWidth();

				pcbCombo->SetCurSelByItemData( m_swdData.rfRegexFlavor );
			}
		}


		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_CHAR_TYPE_COMBO ));
		if ( pcbCombo ) {
			static const CUtilities::MX_DATA_TYPES dtTypes[] = {
				CUtilities::MX_DT_INT8,
				CUtilities::MX_DT_UINT8,
				CUtilities::MX_DT_INT16,
				CUtilities::MX_DT_UINT16,
				CUtilities::MX_DT_INT32,
				CUtilities::MX_DT_UINT32,
				CUtilities::MX_DT_INT64,
				CUtilities::MX_DT_UINT64,
				CUtilities::MX_DT_FLOAT16,
				CUtilities::MX_DT_FLOAT,
				CUtilities::MX_DT_DOUBLE,
				CUtilities::MX_DT_VOID
			};
			for ( size_t I = 0; I < MX_ELEMENTS( dtTypes ); ++I ) {
				if ( dtTypes[I] == CUtilities::MX_DT_VOID ) {
					INT iIndex = pcbCombo->AddString( _DEC_WS_28D8D027_Mixed.c_str() );
					pcbCombo->SetItemData( iIndex, dtTypes[I] );
				}
				else {
					CSecureWString sTemp;
					CUtilities::PrintDataType( sTemp, dtTypes[I] );
					INT iIndex = pcbCombo->AddString( sTemp.c_str() );
					pcbCombo->SetItemData( iIndex, dtTypes[I] );
				}
			}
			pcbCombo->AutoSetMinListWidth();

			pcbCombo->SetCurSelByItemData( m_swdData.dtArrayCharType );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_UTF_HELPER_COMBO ));
		if ( pcbCombo ) {
			pcbCombo->InitStorage( ee::CUnicode::TotalNames(), ee::CUnicode::TotalNames() * 17 );
			for ( size_t I = 0; I < ee::CUnicode::TotalNames(); I++ ) {
				std::wstring sTemp;
				ee::CUnicode::GetName( sTemp, I );
				sTemp.push_back( L':' );
				sTemp.push_back( L' ' );
				uint32_t uiLen;
				uint32_t uiConverted = ee::CExpEval::Utf32ToUtf16( ee::CUnicode::GetUnicodeNum( I ), uiLen );
				for ( uint32_t J = 0; J < uiLen; ++J ) {
					sTemp.push_back( static_cast<uint16_t>(uiConverted) );
					uiConverted >>= 16;
				}
				INT iIndex = pcbCombo->AddString( sTemp.c_str() );
				pcbCombo->SetItemData( iIndex, I );
			}
			pcbCombo->AutoSetMinListWidth();

			pcbCombo->SetCurSelByItemData( m_swdData.uiUtfHelperIdx );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_REGEX_ENCODING_COMBO ));
		if ( pcbCombo ) {
			static const struct {
				const char *					pcName;
				size_t							sLen;
				CUtilities::MX_REGEX_ENCODING	reEncoding;
			} sCombo[] = {
				{ _T_LEN_266E1849_ASCII,											CUtilities::MX_RE_ASCII },
				{ _T_LEN_0E813C50_UTF_8,											CUtilities::MX_RE_UTF8 },
				{ _T_LEN_26FC5333_UTF_16_BE,										CUtilities::MX_RE_UTF16_BE },
				{ _T_LEN_A71F1195_UTF_16,											CUtilities::MX_RE_UTF16_LE },
				{ _T_LEN_D35E9704_UTF_32_BE,										CUtilities::MX_RE_UTF32_BE },
				{ _T_LEN_9244B70E_UTF_32,											CUtilities::MX_RE_UTF32_LE },
				{ _T_LEN_4A2B64E8_ISO_8859_1__Latin_1__Western_European_,			CUtilities::MX_RE_ISO_8859_1 },
				{ _T_LEN_762137F0_ISO_8859_2__Latin_2__Central_European_,			CUtilities::MX_RE_ISO_8859_2 },
				{ _T_LEN_67696996_ISO_8859_3__Latin_3__South_European_,				CUtilities::MX_RE_ISO_8859_3 },
				{ _T_LEN_918DCE6F_ISO_8859_4__Latin_4__North_European_,				CUtilities::MX_RE_ISO_8859_4 },
				{ _T_LEN_97F4EA6D_ISO_8859_5__Latin_Cyrillic_,						CUtilities::MX_RE_ISO_8859_5 },
				{ _T_LEN_5C684CC3_ISO_8859_6__Latin_Arabic_,						CUtilities::MX_RE_ISO_8859_6 },
				{ _T_LEN_95FCC49F_ISO_8859_7__Latin_Greek_,							CUtilities::MX_RE_ISO_8859_7 },
				{ _T_LEN_C7ACCEE6_ISO_8859_8__Latin_Hebrew_,						CUtilities::MX_RE_ISO_8859_8 },
				{ _T_LEN_B5C41944_ISO_8859_9__Latin_5__Turkish_,					CUtilities::MX_RE_ISO_8859_9 },
				{ _T_LEN_3CD377C1_ISO_8859_10__Latin_6__Nordic_,					CUtilities::MX_RE_ISO_8859_10 },
				{ _T_LEN_692656F3_ISO_8859_11__Latin_Thai_,							CUtilities::MX_RE_ISO_8859_11 },
				{ _T_LEN_6502C877_ISO_8859_13__Latin_7__Baltic_Rim_,				CUtilities::MX_RE_ISO_8859_13 },
				{ _T_LEN_84F7764A_ISO_8859_14__Latin_8__Celtic_,					CUtilities::MX_RE_ISO_8859_14 },
				{ _T_LEN_62416340_ISO_8859_15__Latin_9_,							CUtilities::MX_RE_ISO_8859_15 },
				{ _T_LEN_B8F1F96A_ISO_8859_16__Latin_10__South_Eastern_European_,	CUtilities::MX_RE_ISO_8859_16 },
				{ _T_LEN_C66DAEDC_EUC_JP,											CUtilities::MX_RE_EUC_JP },
				{ _T_LEN_8C4804A0_EUC_TW,											CUtilities::MX_RE_EUC_TW },
				{ _T_LEN_3178FEB1_EUC_KR,											CUtilities::MX_RE_EUC_KR },
				{ _T_LEN_EDA028F6_EUC_CN,											CUtilities::MX_RE_EUC_CN },
				{ _T_LEN_6CEF2247_Shift_JIS,										CUtilities::MX_RE_S_JIS },
				{ _T_LEN_5D158210_KOI_8,											CUtilities::MX_RE_KOI_8 },
				{ _T_LEN_67D55D3C_KOI8_R,											CUtilities::MX_RE_KOI8_R },
				{ _T_LEN_DE7A9DE4_Windows_1251,										CUtilities::MX_RE_WINDOWS_1251 },
				{ _T_LEN_993E07D4_Big_5,											CUtilities::MX_RE_BIG_5 },
				{ _T_LEN_CD45848F_GB_18030,											CUtilities::MX_RE_GB_18030 },
			};
			for ( size_t I = 0; I < MX_ELEMENTS( sCombo ); ++I ) {
				INT iIndex = pcbCombo->AddString( CStringDecoder::DecodeToString( sCombo[I].pcName, sCombo[I].sLen ).c_str() );
				pcbCombo->SetItemData( iIndex, sCombo[I].reEncoding );
			}
			pcbCombo->AutoSetMinListWidth();

			pcbCombo->SetCurSelByItemData( m_swdData.reRegexEncoding );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_REGEX_HELPER_COMBO ));
		if ( pcbCombo ) {
			static const struct {
				const char *					pcName;
				size_t							sLen;
				//CUtilities::MX_REGEX_ENCODING	reEncoding;
			} sCombo[] = {
				{ _T_LEN_1E9998F8_Syntax_____Escape__enable_or_disable_meta_character__ },
				{ _T_LEN_73B59F8B_Syntax____Alternation_ },
				{ _T_LEN_C474B857_Syntax________Group_ },
				{ _T_LEN_97286653_Syntax________Character_class_ },
				{ _T_LEN_E89C1E7C_Characters____t_Horizontal_tab__0x09__ },
				{ _T_LEN_98E6A75C_Characters____v_Vertical_tab__0x0B__ },
				{ _T_LEN_DB4DBA7B_Characters____n_New_line__0x0A__ },
				{ _T_LEN_FC8E4B8D_Characters____r_Carriage_return__0x0D__ },
				{ _T_LEN_9A02D931_Characters____b_Backspace__0x08__ },
				{ _T_LEN_80C68932_Characters____f_Form_feed__0x0C__ },
				{ _T_LEN_B7C9BCFF_Characters____a_Bell__0x07__ },
				{ _T_LEN_53C040D9_Characters____e_Escape__0x1B__ },
				{ _T_LEN_04758C2B_Characters____nnn_Octal_character_ },
				{ _T_LEN_B8E16D6C_Characters____xHH_Hex_character_ },
				{ _T_LEN_2C219293_Characters____x_7HHHHHHH___1_8_digits__Hex_character_ },
				{ _T_LEN_DC2297EC_Characters____o_17777777777___1_11_digits__Octal_character_ },
				{ _T_LEN_C3AD5DA9_Characters____uHHHH_Hex_character_ },
				{ _T_LEN_C8D8391D_Characters____cx_Control_character_ },
				{ _T_LEN_3F21A2EB_Characters____C_x_Control_character_ },
				{ _T_LEN_650EEF78_Characters____M_x_Meta__x_0x80__ },
				{ _T_LEN_6704B682_Characters____M___C_x_Meta_control_char_ },
				{ _T_LEN_78411CED_Character_Types____Any_character__except_newline__ },
				{ _T_LEN_F94A6D6F_Character_Types____w_Word_character__Not_Unicode__alphanumeric__________and_multibyte_char__Unicode__General_Category______Letter_Mark_Number_Connector_Punctuation__ },
				{ _T_LEN_6FC9760A_Character_Types____W_Non_word_character_ },
				{ _T_LEN_4B2860CE_Character_Types____s_Whitespace_character__Not_Unicode____t____n____v____f____r____x20__Unicode_case__U_0009__U_000A__U_000B__U_000C__U_000D__U_0085_NEL_______Line_Separator_____Paragraph_Separator_____Space_Separator_ },
				{ _T_LEN_8CD76527_Character_Types____S_Non_whitespace_character_ },
				{ _T_LEN_4D38C486_Character_Types____d_Decimal_digit_character_ },
				{ _T_LEN_CD49677F_Character_Types____D_Non_decimal_digit_character_ },
				{ _T_LEN_FC2BBBED_Character_Types____h_Hexadecimal_digit_character__0_9a_fA_F__ },
				{ _T_LEN_59068FEB_Character_Types____H_Non_hexadecimal_digit_character_ },
				{ _T_LEN_D9635A7F_Character_Types____R_General_newline_______r__n____or___n___v___f___r___Unicode_case_______r__n____or___n___v___f___r_or_U_0085__U_2028__U_2029_ },
				{ _T_LEN_3D358534_Character_Types____N_Negative_new_line_____m___ },
				{ _T_LEN_658774D1_Character_Types____O_True____any_char_______m___ },
				{ _T_LEN_0A868224_Character_Types____X_Text_segment____X__________O_____Y__O___ },
				{ _T_LEN_8487C3CF_Character_Types____p_alnum__Alphanumeric_character_ },
				{ _T_LEN_533C75BE_Character_Types____p_alpha__Alphabetic_character_ },
				{ _T_LEN_BDA468F5_Character_Types____p_blank____t____x20_ },
				{ _T_LEN_CF2DC677_Character_Types____p_cntrl__Control_character_ },
				{ _T_LEN_8DBB2834_Character_Types____p_digit__0_9_ },
				{ _T_LEN_B30E7F0C_Character_Types____p_graph__Includes_all_multibyte_encoded_characters_ },
				{ _T_LEN_9CD26A62_Character_Types____p_lower__Lower_case_character_ },
				{ _T_LEN_0827433F_Character_Types____p_print__Includes_all_multibyte_encoded_characters_ },
				{ _T_LEN_E09CB6D8_Character_Types____p_punct__Punctuation_character_ },
				{ _T_LEN_836C2382_Character_Types____p_space____t____n____v____f____r____x20_ },
				{ _T_LEN_3ADC2673_Character_Types____p_upper__Upper_case_character_ },
				{ _T_LEN_FAF8CE3B_Character_Types____p_xdigit__0_9__a_f__A_F_ },
				{ _T_LEN_25B1D8DC_Character_Types____p_word__Alphanumeric__________and_multibyte_characters_ },
				{ _T_LEN_BE67B794_Character_Types____p_ascii__Code_value___0___127__ },
				{ _T_LEN_B1E73782_Character_Types____p_hiragana__Hiragana_characters_ },
				{ _T_LEN_2ECD5D16_Character_Types____p_katakana__Katakana_characters_ },
				{ _T_LEN_FBE718BA_Character_Types____p__property_name__Property_negation_ },
				{ _T_LEN_D8FC6352_Character_Types____P_property_name__Property_negation_ },
				{ _T_LEN_09D6A54A_Quantifier__Greedy_____1_or_0_times_ },
				{ _T_LEN_7AEC71A3_Quantifier__Greedy_____0_or_more_times_ },
				{ _T_LEN_3BD656D3_Quantifier__Greedy_____1_or_more_times_ },
				{ _T_LEN_B18057D0_Quantifier__Greedy____n_m___n____m__At_least_n_but_no_more_than_m_times_ },
				{ _T_LEN_791B2373_Quantifier__Greedy____n___At_least_n_times_ },
				{ _T_LEN_2F3102E6_Quantifier__Greedy_____n__At_least_0_but_no_more_than_n_times___0_n___ },
				{ _T_LEN_C258C722_Quantifier__Greedy____n__n_times_ },
				{ _T_LEN_0DF5199A_Quantifier__Reluctant______0_or_1_times_ },
				{ _T_LEN_9505114F_Quantifier__Reluctant______0_or_more_times_ },
				{ _T_LEN_EC8F0086_Quantifier__Reluctant______1_or_more_times_ },
				{ _T_LEN_FF0B5D1E_Quantifier__Reluctant____n_m____n____m__At_least_n_but_not_more_than_m_times_ },
				{ _T_LEN_14674217_Quantifier__Reluctant____n____At_least_n_times_ },
				{ _T_LEN_57B23102_Quantifier__Reluctant_____n___At_least_0_but_no_more_than_n_times______0_n____ },
				{ _T_LEN_E8DBD2A2_Quantifier__Possessive______1_or_0_times_ },
				{ _T_LEN_2443E899_Quantifier__Possessive______0_or_more_times_ },
				{ _T_LEN_5DC9F950_Quantifier__Possessive______1_or_more_times_ },
				{ _T_LEN_EFC73A28_Anchors____Beginning_of_the_line_ },
				{ _T_LEN_4B2D627A_Anchors____End_of_the_line_ },
				{ _T_LEN_AF7BCF69_Anchors____b_Word_boundary_ },
				{ _T_LEN_67DE38EA_Anchors____B_Non_word_boundary_ },
				{ _T_LEN_CC7F033B_Anchors____A_Beginning_of_the_string_ },
				{ _T_LEN_416B0D49_Anchors____Z_End_of_the_string__or_before_newline_at_the_end_ },
				{ _T_LEN_ED7E8C8C_Anchors____z_End_of_the_string_ },
				{ _T_LEN_E1EC9285_Anchors____y_Text_segment_boundary___Unicode_only__ },
				{ _T_LEN_F387A4C7_Anchors____Y_Text_segment_non_boundary___Unicode_only__ },
				{ _T_LEN_49A6189C_Character_Class_______Negative_class__lowest_precedence__ },
				{ _T_LEN_B295E2EF_Character_Class__x_y_Range_from_x_to_y_ },
				{ _T_LEN_06B7B91D_Character_Class________Set__character_class_in_character_class__ },
				{ _T_LEN_D40C5E20_Character_Class__________Intersection__low_precedence__only_higher_than____ },
				{ _T_LEN_ABD9DAA3_Character_Class_____alnum____Alphanumeric_character__Unicode_case__Letter___Mark___Decimal_Number_ },
				{ _T_LEN_288C8F1B_Character_Class_____alpha____Alphabetic_character__Unicode_case__Letter___Mark_ },
				{ _T_LEN_749D363D_Character_Class_____ascii____Code_value___0___127___Unicode_case__0000___007F_ },
				{ _T_LEN_3E9FA80A_Character_Class_____blank______t____x20__Unicode_case__Space_Separator___0009_ },
				{ _T_LEN_3DF6DFD3_Character_Class_____cntrl____Control_character__Unicode_case__Control___Format___Unassigned___Private_Use___Surrogate_ },
				{ _T_LEN_9CA4AD05_Character_Class_____digit____0_9__Unicode_case__Decimal_Number_ },
				{ _T_LEN_3BF519B8_Character_Class_____graph____Includes_all_multibyte_encoded_characters__Unicode_case______space________Control_____Unassigned_____Surrogate_ },
				{ _T_LEN_0A86F75D_Character_Class_____lower____Lower_case_character__Unicode_case__Lowercase_Letter_ },
				{ _T_LEN_0D4437B6_Character_Class_____print____Includes_all_multibyte_encoded_characters__Unicode_case_____graph_________space____ },
				{ _T_LEN_B5282475_Character_Class_____punct____Punctuation_character__Unicode_case__Connector_Punctuation___Dash_Punctuation___Close_Punctuation___Final_Punctuation___Initial_Punctuation___Other_Punctuation___Open_Punctuation_ },
				{ _T_LEN_C8195CEE_Character_Class_____space______t____n____v____f____r____x20__Unicode_case__Space_Separator___Line_Separator___Paragraph_Separator___U_0009___U_000A___U_000B___U_000C___U_000D___U_0085_ },
				{ _T_LEN_D80EB15F_Character_Class_____upper____Upper_case_character__Unicode_case__Uppercase_Letter_ },
				{ _T_LEN_F522B169_Character_Class_____xdigit____0_9__a_f__A_F__Unicode_case__U_0030___U_0039___U_0041___U_0046___U_0061___U_0066__0_9__a_f__A_F__ },
				{ _T_LEN_581DC2FE_Character_Class_____word____Alphanumeric__________and_multibyte_characters__Unicode_case__Letter___Mark___Decimal_Number___Connector_Punctuation_ },
				{ _T_LEN_9D000D59_Extended_Groups__________Comment_ },
				{ _T_LEN_CC727843_Extended_Groups____imxWDSPy_imxWDSP_subexp__Option_on_off_for_subexp_ },
				{ _T_LEN_BA259B4D_Extended_Groups____imxWDSPy_imxWDSP__Isolated_option_ },
				{ _T_LEN_B70731CD_Extended_Groups_____subexp__Non_capturing_group_ },
				{ _T_LEN_6B99F9DE_Extended_Groups___subexp__Capturing_group_ },
				{ _T_LEN_AA8B920D_Extended_Groups_____subexp__Look_ahead_ },
				{ _T_LEN_E027A046_Extended_Groups_____subexp__Negative_look_ahead_ },
				{ _T_LEN_112033AB_Extended_Groups______subexp__Look_behind_ },
				{ _T_LEN_02B331D3_Extended_Groups______subexp__Negative_look_behind_ },
				{ _T_LEN_7C80D082_Extended_Groups_____subexp__Atomic_group_ },
				{ _T_LEN_D87A4D1D_Extended_Groups_____name_subexp______name_subexp__Defines_named_group_ },

			};
			for ( size_t I = 0; I < MX_ELEMENTS( sCombo ); ++I ) {
				INT iIndex = pcbCombo->AddString( CStringDecoder::DecodeToWString( sCombo[I].pcName, sCombo[I].sLen ).c_str() );
				pcbCombo->SetItemData( iIndex, I );
			}
			pcbCombo->AutoSetMinListWidth();

			pcbCombo->SetCurSelByItemData( m_swdData.uiRegexHelperIdx );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TO_FIND_COMBO ));
		if ( pcbCombo ) {
			for ( size_t I = 0; I < m_swdData.vSearchTextHistory.size(); ++I ) {
				pcbCombo->AddString( m_swdData.vSearchTextHistory[I].c_str() );
			}
			pcbCombo->SetTextW( m_swdData.wsSearchText.c_str() );
			pcbCombo->SetFocus();

			pcbCombo->AutoSetMinListWidth();
		}

		if ( m_pmhMemHack ) {
			CCheckButton * pcbCheck;
#define MX_CHECK( ID, VAL )																						\
	pcbCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::ID ));							\
	if ( pcbCheck ) { pcbCheck->CheckButton( m_pmhMemHack->Options().VAL ? BST_CHECKED : BST_UNCHECKED ); }

			MX_CHECK( MX_SSI_ALIGNED_CHECK, bAligned );
			MX_CHECK( MX_SSI_IGNORE_CASE_CHECK, bMatchCase );
			MX_CHECK( MX_SSI_WHOLE_WORD_CHECK, bWholeWord );
			MX_CHECK( MX_SSI_HEX_CHECK, bIsHex );
			MX_CHECK( MX_SSI_ESCAPES_CHECK, bResolveEscapes );
			MX_CHECK( MX_SSI_WILDCARD_CHECK, bWildcard );
			MX_CHECK( MX_SSI_LINGUISTIC_IGNORECASE_CHECK, bLingIgnoreCase );
			MX_CHECK( MX_SSI_LINGUISTIC_IGNOREDIACRITIC_CHECK, bLingIgnoreDiacritic );
			MX_CHECK( MX_SSI_NORM_IGNOREKANATYPE_CHECK, bIgnoreKana );
			MX_CHECK( MX_SSI_NORM_IGNORENONSPACE_CHECK, bIgnoreNoSpace );
			MX_CHECK( MX_SSI_NORM_IGNORESYMBOLS_CHECK, bIgnoreSymbols );
			MX_CHECK( MX_SSI_NORM_IGNOREWIDTH_CHECK, bIgnoreWidth );

			MX_CHECK( MX_SSI_REGEX_SINGLELINE, bRegexSingleLine );
			MX_CHECK( MX_SSI_REGEX_MULTILINE, bRegexMultiLine );
			MX_CHECK( MX_SSI_REGEX_EXTENDED, bRegexExtended );
			MX_CHECK( MX_SSI_REGEX_FIND_LONGEST, bRegexFindLongest );
			MX_CHECK( MX_SSI_REGEX_NEGATE_SINGLELINE, bRegexNegateSingleLine );
#undef MX_CHECK

			pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_FROM_COMBO ));
			if ( pcbCombo ) {
				for ( size_t I = 0; I < m_pmhMemHack->Options().vFromHistory.size(); ++I ) {
					pcbCombo->AddString( m_pmhMemHack->Options().vFromHistory[I].c_str() );
				}
				if ( m_pmhMemHack->Options().wsFromText.size() == 0 ) {
					uintptr_t uiptrTemp = reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMinimumApplicationAddress);
					std::string sTemp = CUtilities::ToHex( uiptrTemp, 4 );
					pcbCombo->SetTextA( sTemp.c_str() );
				}
				else {
					pcbCombo->SetTextW( m_pmhMemHack->Options().wsFromText.c_str() );
				}
				pcbCombo->SetTreatAsHex( TRUE );
				pcbCombo->SetFocus();

				pcbCombo->AutoSetMinListWidth();
			}

			pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_TO_COMBO ));
			if ( pcbCombo ) {
				for ( size_t I = 0; I < m_pmhMemHack->Options().vToHistory.size(); ++I ) {
					pcbCombo->AddString( m_pmhMemHack->Options().vToHistory[I].c_str() );
				}
				if ( m_pmhMemHack->Options().wsToText.size() == 0 ) {
					uintptr_t uiptrTemp = reinterpret_cast<uintptr_t>(mx::CSystem::GetSystemInfo().lpMaximumApplicationAddress);
					std::string sTemp = CUtilities::ToHex( uiptrTemp + 1ULL, 4 );
					pcbCombo->SetTextA( sTemp.c_str() );
				}
				else {
					pcbCombo->SetTextW( m_pmhMemHack->Options().wsToText.c_str() );
				}
				pcbCombo->SetTreatAsHex( TRUE );
				pcbCombo->SetFocus();

				pcbCombo->AutoSetMinListWidth();
			}
		}

		CButton * pbButton = static_cast<CButton *>(FindChild( CNewStringSearchLayout::MX_SSI_GENERAL_SEARCH_OPTIONS_BUTTON ));
		if ( pbButton ) {
			pbButton->SetEnabled( m_pmhMemHack != nullptr );
		}

		

		UpdateDialog();
		return CMainWindow::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CNewStringSearchWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CNewStringSearchLayout::MX_SSI_CANCEL : {
				return Close();
			}
			case CNewStringSearchLayout::MX_SSI_UTF_HELPER_COMBO : {
				if ( /*_wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE*/_wCtrlCode == CBN_SELENDOK ) {
					CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( _Id ));
					if ( pcbCombo ) {
						size_t sIdx = static_cast<size_t>(pcbCombo->GetCurSelItemData());
						std::wstring sTemp;
						sTemp.push_back( L'\\' );
						sTemp.push_back( L'N' );
						sTemp.push_back( L'{' );
						ee::CUnicode::GetName( sTemp, sIdx );
						sTemp.push_back( L'}' );

						CComboBox * pcbEdit = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TO_FIND_COMBO ));
						if ( pcbEdit ) {
							pcbEdit->ReplaceSel( true, sTemp.c_str() );
						}
					}
					UpdateDialog();
				}
				break;
			}
			case CNewStringSearchLayout::MX_SSI_REGEX_FLAVOR_COMBO : {}
			case CNewStringSearchLayout::MX_SSI_STRING_TYPE_COMBO : {}
			case CNewStringSearchLayout::MX_SSI_CHAR_TYPE_COMBO : {}
			case CNewStringSearchLayout::MX_SSI_REGEX_ENCODING_COMBO : {}
			case CNewStringSearchLayout::MX_SSI_STRING_TO_FIND_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CNewStringSearchLayout::MX_SSI_GENERAL_SEARCH_OPTIONS_BUTTON : {
				CMhsMainWindow * pmmwMain = static_cast<CMhsMainWindow *>(FindParent( mx::CMainWindowLayout::MX_MWI_MAINWINDOW ));
				if ( pmmwMain ) {
					pmmwMain->ShowOptions( pmmwMain->SearchOptionsPage() );
					UpdateDialog();
				}
				break;
			}
			case CNewStringSearchLayout::MX_SSI_REGEX_EXTENDED : {}
			case CNewStringSearchLayout::MX_SSI_REGEX_FIND_LONGEST : {}
			//case CNewStringSearchLayout::MX_SSI_REGEX_IGNORE_EMPTY : {}
			case CNewStringSearchLayout::MX_SSI_REGEX_MULTILINE : {}
			case CNewStringSearchLayout::MX_SSI_REGEX_NEGATE_SINGLELINE : {}
			case CNewStringSearchLayout::MX_SSI_REGEX_SINGLELINE : {}
			case CNewStringSearchLayout::MX_SSI_LINGUISTIC_IGNORECASE_CHECK : {}
			case CNewStringSearchLayout::MX_SSI_LINGUISTIC_IGNOREDIACRITIC_CHECK : {}
			case CNewStringSearchLayout::MX_SSI_NORM_IGNOREKANATYPE_CHECK : {}
			case CNewStringSearchLayout::MX_SSI_NORM_IGNORENONSPACE_CHECK : {}
			case CNewStringSearchLayout::MX_SSI_NORM_IGNORESYMBOLS_CHECK : {}
			case CNewStringSearchLayout::MX_SSI_NORM_IGNOREWIDTH_CHECK : {}
			case CNewStringSearchLayout::MX_SSI_IGNORE_CASE_CHECK : {}
			case CNewStringSearchLayout::MX_SSI_WHOLE_WORD_CHECK : {}
			case CNewStringSearchLayout::MX_SSI_HEX_CHECK : {}
			case CNewStringSearchLayout::MX_SSI_ESCAPES_CHECK : {}
			case CNewStringSearchLayout::MX_SSI_WILDCARD_CHECK : {}
			case CNewStringSearchLayout::MX_SSI_ALIGNED_CHECK : {
				if ( _wCtrlCode == STN_CLICKED ) {
					UpdateDialog();
				}
				break;
			}
			case CNewStringSearchLayout::MX_SSI_OK : {
				CSecureWString wsError;
				CSearcher::MX_SEARCH_PARMS spParms;
				if ( !DialogContentsAreValid( wsError, spParms ) ) {
					lsw::CBase::MessageBoxError( Wnd(), wsError.c_str() );
					break;
				}
				SaveDialogData( m_swdData );
				CMhsMainWindow * pmmwMain = static_cast<CMhsMainWindow *>(FindParent( mx::CMainWindowLayout::MX_MWI_MAINWINDOW ));
				if ( pmmwMain ) {
					pmmwMain->Search( &spParms, this, true, nullptr );
				}
				return Close();
			}
		}
		return CMainWindow::Command( _wCtrlCode, _Id, _pwSrc );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CNewStringSearchWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

	// Saves the current dialog values to an MX_SEARCH_WIN_DATA structure.
	void CNewStringSearchWindow::SaveDialogData( MX_SEARCH_WIN_DATA &_swdData ) {
		CComboBox * pcbCombo;
		if ( m_pmhMemHack ) {
			MX_OPTIONS oOpts = m_pmhMemHack->Options();
			CCheckButton * pcbCheck;
#define MX_SAVE_CHECK( ID, VAL )														\
	pcbCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::ID ));	\
	if ( pcbCheck ) { oOpts.VAL = pcbCheck->IsChecked(); }
			MX_SAVE_CHECK( MX_SSI_ALIGNED_CHECK, bAligned );
			MX_SAVE_CHECK( MX_SSI_IGNORE_CASE_CHECK, bMatchCase );
			MX_SAVE_CHECK( MX_SSI_WHOLE_WORD_CHECK, bWholeWord );
			MX_SAVE_CHECK( MX_SSI_HEX_CHECK, bIsHex );
			MX_SAVE_CHECK( MX_SSI_ESCAPES_CHECK, bResolveEscapes );
			MX_SAVE_CHECK( MX_SSI_WILDCARD_CHECK, bWildcard );
			MX_SAVE_CHECK( MX_SSI_LINGUISTIC_IGNORECASE_CHECK, bLingIgnoreCase );
			MX_SAVE_CHECK( MX_SSI_LINGUISTIC_IGNOREDIACRITIC_CHECK, bLingIgnoreDiacritic );
			MX_SAVE_CHECK( MX_SSI_NORM_IGNOREKANATYPE_CHECK, bIgnoreKana );
			MX_SAVE_CHECK( MX_SSI_NORM_IGNORENONSPACE_CHECK, bIgnoreNoSpace );
			MX_SAVE_CHECK( MX_SSI_NORM_IGNORESYMBOLS_CHECK, bIgnoreSymbols );
			MX_SAVE_CHECK( MX_SSI_NORM_IGNOREWIDTH_CHECK, bIgnoreWidth );
			MX_SAVE_CHECK( MX_SSI_REGEX_SINGLELINE, bRegexSingleLine );
			MX_SAVE_CHECK( MX_SSI_REGEX_MULTILINE, bRegexMultiLine );
			MX_SAVE_CHECK( MX_SSI_REGEX_EXTENDED, bRegexExtended );
			MX_SAVE_CHECK( MX_SSI_REGEX_FIND_LONGEST, bRegexFindLongest );
			MX_SAVE_CHECK( MX_SSI_REGEX_NEGATE_SINGLELINE, bRegexNegateSingleLine );
#undef MX_SAVE_CHECK

			pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_FROM_COMBO ));
			if ( pcbCombo ) {
				oOpts.wsFromText = pcbCombo->GetTextW();
				if ( oOpts.wsFromText.size() ) {
					auto aFound = std::find( oOpts.vFromHistory.begin(), oOpts.vFromHistory.end(), oOpts.wsFromText );
					if ( aFound != oOpts.vFromHistory.end() ) {
						oOpts.vFromHistory.erase( aFound );
					}
					oOpts.vFromHistory.insert( oOpts.vFromHistory.begin(), oOpts.wsFromText );
				}
			}
			pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_TO_COMBO ));
			if ( pcbCombo ) {
				oOpts.wsToText = pcbCombo->GetTextW();
				if ( oOpts.wsToText.size() ) {
					auto aFound = std::find( oOpts.vToHistory.begin(), oOpts.vToHistory.end(), oOpts.wsToText );
					if ( aFound != oOpts.vToHistory.end() ) {
						oOpts.vToHistory.erase( aFound );
					}
					oOpts.vToHistory.insert( oOpts.vToHistory.begin(), oOpts.wsToText );
				}
			}

			m_pmhMemHack->SetOptions( oOpts );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TYPE_COMBO ));
		if ( pcbCombo ) {
			_swdData.stSearchFormat = static_cast<MX_STRING_TYPE>(pcbCombo->GetCurSelItemData());
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_REGEX_FLAVOR_COMBO ));
		if ( pcbCombo ) {
			_swdData.rfRegexFlavor = static_cast<CUtilities::MX_REGEX_FLAVORS>(pcbCombo->GetCurSelItemData());
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_CHAR_TYPE_COMBO ));
		if ( pcbCombo ) {
			_swdData.dtArrayCharType = static_cast<CUtilities::MX_DATA_TYPES>(pcbCombo->GetCurSelItemData());
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_REGEX_ENCODING_COMBO ));
		if ( pcbCombo ) {
			_swdData.reRegexEncoding = static_cast<CUtilities::MX_REGEX_ENCODING>(pcbCombo->GetCurSelItemData());
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_UTF_HELPER_COMBO ));
		if ( pcbCombo ) {
			_swdData.uiUtfHelperIdx = static_cast<uint32_t>(pcbCombo->GetCurSelItemData());
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_REGEX_HELPER_COMBO ));
		if ( pcbCombo ) {
			_swdData.uiRegexHelperIdx = static_cast<uint32_t>(pcbCombo->GetCurSelItemData());
		}

		


		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TO_FIND_COMBO ));
		if ( pcbCombo ) {
			_swdData.wsSearchText = pcbCombo->GetTextW();
			if ( _swdData.wsSearchText.size() ) {
				auto aFound = std::find( _swdData.vSearchTextHistory.begin(), _swdData.vSearchTextHistory.end(), _swdData.wsSearchText );
				if ( aFound != _swdData.vSearchTextHistory.end() ) {
					_swdData.vSearchTextHistory.erase( aFound );
				}
				_swdData.vSearchTextHistory.insert( _swdData.vSearchTextHistory.begin(), _swdData.wsSearchText );
			}
		}
	}

	// Validates the dialog box.
	bool CNewStringSearchWindow::DialogContentsAreValid( CSecureWString &_wsError, CSearcher::MX_SEARCH_PARMS &_spParmsIfValid ) {
		_spParmsIfValid.stType = CUtilities::MX_ST_STRING_SEARCH;
		_spParmsIfValid.pbAbort = nullptr;
		_spParmsIfValid.iThreadPriority = THREAD_PRIORITY_NORMAL;



		//std::vector<uint8_t> vFinalBytes;
		if ( !GetFinalSearchBytes( _spParmsIfValid.vStringSearchValue, _spParmsIfValid.vStringMetaData, &_wsError, _spParmsIfValid.wuWildcardUsage ) ) {
			_wsError = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		if ( _spParmsIfValid.wuWildcardUsage == CUtilities::MX_WU_FULL ) {
			_wsError = _DEC_WS_AA343191_The_search_string_must_not_be_entirely_wildcards_;
			return false;
		}
		if ( !_spParmsIfValid.vStringSearchValue.size() ) {
			_wsError = _DEC_WS_3E3C1533_No_string_provided_;
			return false;
		}

		// String encoding.
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TYPE_COMBO ));
		if ( !pcbCombo ) {
			_wsError = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		switch ( pcbCombo->GetCurSelItemData() ) {
			case MX_ST_BYTE_ARRAY : {
				_spParmsIfValid.sstSearchType = CUtilities::MX_SST_RAW;
				break;
			}
			case MX_ST_REGEX : {
				_spParmsIfValid.sstSearchType = CUtilities::MX_SST_REGEX;

				CComboBox * pcbRegexCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_REGEX_FLAVOR_COMBO ));
				if ( !pcbRegexCombo ) {
					_wsError = _DEC_WS_1468A1DF_Internal_error_;
					return false;
				}
				_spParmsIfValid.rfRegexFlavor = static_cast<CUtilities::MX_REGEX_FLAVORS>(pcbRegexCombo->GetCurSelItemData());

				break;
			}
			case MX_ST_ANSI : {
				_spParmsIfValid.sstSearchType = CUtilities::MX_SST_ASCII;
				break;
			}
			case MX_ST_UTF8 : {
				_spParmsIfValid.sstSearchType = CUtilities::MX_SST_UTF8;
				break;
			}
			case MX_ST_UTF16 : {
				_spParmsIfValid.sstSearchType = CUtilities::MX_SST_UTF16;
				break;
			}
			case MX_ST_UTF16_BE : {
				_spParmsIfValid.sstSearchType = CUtilities::MX_SST_UTF16_BE;
				break;
			}
			case MX_ST_UTF32 : {
				_spParmsIfValid.sstSearchType = CUtilities::MX_SST_UTF32;
				break;
			}
			case MX_ST_UTF32_BE : {
				_spParmsIfValid.sstSearchType = CUtilities::MX_SST_UTF32_BE;
				break;
			}
			case MX_ST_UTFANY : {
				_spParmsIfValid.sstSearchType = CUtilities::MX_SST_UTFALL;
				break;
			}
			default : {
				_wsError = _DEC_WS_1468A1DF_Internal_error_;
				return false;
			}
			
		}

		// Array type.
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_CHAR_TYPE_COMBO ));
		if ( !pcbCombo ) {
			_wsError = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		_spParmsIfValid.dtStringCharType = static_cast<CUtilities::MX_DATA_TYPES>(pcbCombo->GetCurSelItemData());

		// Regex encoding.
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_REGEX_ENCODING_COMBO ));
		if ( !pcbCombo ) {
			_wsError = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		_spParmsIfValid.reRegexEncoding = static_cast<CUtilities::MX_REGEX_ENCODING>(pcbCombo->GetCurSelItemData());

		// Starting address.
		BOOL bIsValid;
		ee::CExpEvalContainer::EE_RESULT rRes;
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_FROM_COMBO ));
		if ( !pcbCombo || !pcbCombo->GetTextAsExpression( rRes, &bIsValid ) ) {
			_wsError = _DEC_WS_F94C43F0_The_specified_search_range_is_invalid_;
			return false;
		}
		_spParmsIfValid.ui64AddressFrom = CUtilities::ExpEvalResultToDataType( rRes, CUtilities::MX_DT_UINT64 ).u.UInt64;
		// Ending search address.
		pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_TO_COMBO ));
		if ( !pcbCombo || !pcbCombo->GetTextAsExpression( rRes, &bIsValid ) ) {
			_wsError = _DEC_WS_F94C43F0_The_specified_search_range_is_invalid_;
			return false;
		}
		_spParmsIfValid.ui64AddressTo = CUtilities::ExpEvalResultToDataType( rRes, CUtilities::MX_DT_UINT64 ).u.UInt64;


		CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_ALIGNED_CHECK ));
		if ( pcbCheck ) {
			_spParmsIfValid.bAligned = pcbCheck->IsChecked();
		}
		else { _spParmsIfValid.bAligned = false; }

		{
			const struct {
				WORD		wCheckId;
				uint32_t	uiFlag;
			} sFlags[] = {
				{ CNewStringSearchLayout::MX_SSI_IGNORE_CASE_CHECK,					CUtilities::MX_SSF_NORM_IGNORECASE },
				{ CNewStringSearchLayout::MX_SSI_LINGUISTIC_IGNORECASE_CHECK,		CUtilities::MX_SSF_LINGUISTIC_IGNORECASE },
				{ CNewStringSearchLayout::MX_SSI_LINGUISTIC_IGNOREDIACRITIC_CHECK,	CUtilities::MX_SSF_LINGUISTIC_IGNOREDIACRITIC },
				{ CNewStringSearchLayout::MX_SSI_NORM_IGNOREKANATYPE_CHECK,			CUtilities::MX_SSF_NORM_IGNOREKANATYPE },
				{ CNewStringSearchLayout::MX_SSI_NORM_IGNORENONSPACE_CHECK,			CUtilities::MX_SSF_NORM_IGNORENONSPACE },
				{ CNewStringSearchLayout::MX_SSI_NORM_IGNORESYMBOLS_CHECK,			CUtilities::MX_SSF_NORM_IGNORESYMBOLS },
				{ CNewStringSearchLayout::MX_SSI_NORM_IGNOREWIDTH_CHECK,			CUtilities::MX_SSF_NORM_IGNOREWIDTH },
			};
			_spParmsIfValid.ui32StringSearchFlags = 0;
			for ( size_t I = 0; I < MX_ELEMENTS( sFlags ); ++I ) {
				pcbCheck = static_cast<CCheckButton *>(FindChild( sFlags[I].wCheckId ));
				if ( pcbCheck ) {
					if ( pcbCheck->IsChecked() ) {
						_spParmsIfValid.ui32StringSearchFlags |= sFlags[I].uiFlag;
					}
					else {
						_spParmsIfValid.ui32StringSearchFlags &= ~sFlags[I].uiFlag;
					}
				}
				else {
					_spParmsIfValid.ui32StringSearchFlags &= ~sFlags[I].uiFlag;
				}
			}
		}

		{
			const struct {
				WORD		wCheckId;
				uint32_t	uiFlag;
			} sFlagsEx[] = {
				{ CNewStringSearchLayout::MX_SSI_WHOLE_WORD_CHECK,					CUtilities::MX_SSF_WHOLE_WORD },
				{ CNewStringSearchLayout::MX_SSI_WILDCARD_CHECK,					CUtilities::MX_SSF_WILDCARD },
			};
			_spParmsIfValid.ui32StringSearchFlagsEx = 0;
			for ( size_t I = 0; I < MX_ELEMENTS( sFlagsEx ); ++I ) {
				pcbCheck = static_cast<CCheckButton *>(FindChild( sFlagsEx[I].wCheckId ));
				if ( pcbCheck ) {
					if ( pcbCheck->IsChecked() ) {
						_spParmsIfValid.ui32StringSearchFlagsEx |= sFlagsEx[I].uiFlag;
					}
					else {
						_spParmsIfValid.ui32StringSearchFlagsEx &= ~sFlagsEx[I].uiFlag;
					}
				}
				else {
					_spParmsIfValid.ui32StringSearchFlagsEx &= ~sFlagsEx[I].uiFlag;
				}
			}
		}

		{
			const struct {
				WORD		wCheckId;
				uint32_t	uiFlag;
			} sFlags[] = {
				{ CNewStringSearchLayout::MX_SSI_REGEX_SINGLELINE,					CUtilities::MX_RO_SINGLE_LINE },
				{ CNewStringSearchLayout::MX_SSI_REGEX_MULTILINE,					CUtilities::MX_RO_MULTI_LINE },
				{ CNewStringSearchLayout::MX_SSI_REGEX_EXTENDED,					CUtilities::MX_RO_EXTENDED },
				{ CNewStringSearchLayout::MX_SSI_REGEX_FIND_LONGEST,				CUtilities::MX_RO_FIND_LONGEST },
				{ CNewStringSearchLayout::MX_SSI_REGEX_NEGATE_SINGLELINE,			CUtilities::MX_RO_NEGATE_SINGE_LINE },
				{ CNewStringSearchLayout::MX_SSI_IGNORE_CASE_CHECK,					CUtilities::MX_RO_IGNORE_CASE },
			};
			_spParmsIfValid.ui32RegexFlags = 0;
			for ( size_t I = 0; I < MX_ELEMENTS( sFlags ); ++I ) {
				pcbCheck = static_cast<CCheckButton *>(FindChild( sFlags[I].wCheckId ));
				if ( pcbCheck ) {
					if ( pcbCheck->IsChecked() ) { _spParmsIfValid.ui32RegexFlags |= sFlags[I].uiFlag; }
					else { _spParmsIfValid.ui32RegexFlags &= ~sFlags[I].uiFlag; }
				}
				else { _spParmsIfValid.ui32RegexFlags &= ~sFlags[I].uiFlag; }
			}
		}

		if ( _spParmsIfValid.sstSearchType == CUtilities::MX_SST_REGEX ) {
			{
				COnigurumaRegex orRegex( _spParmsIfValid.vStringSearchValue.data(), _spParmsIfValid.vStringSearchValue.size(),
					COnigurumaSystem::RegexOptionsToLibrary( _spParmsIfValid.ui32RegexFlags ),
					COnigurumaSystem::RegexEncodingToLibrary( _spParmsIfValid.reRegexEncoding ),
					COnigurumaSystem::RegexFlavorToLibrary( _spParmsIfValid.rfRegexFlavor ) );
				if ( orRegex.Get() != ONIG_NORMAL ) {
					char szBuffer[ONIG_MAX_ERROR_MESSAGE_LEN];
					::onig_error_code_to_str( reinterpret_cast<OnigUChar*>(szBuffer), orRegex.Get(), &orRegex.Error() );
					char szError[ONIG_MAX_ERROR_MESSAGE_LEN+32];
					std::sprintf( szError, _DEC_S_CC6C71BB_Regex_Error___s.c_str(), szBuffer );
					_wsError = ee::CExpEval::StringToWString( szError );
					return false;
				}
			}
		}

		return true;
	}

	// Updates the dialog.
	void CNewStringSearchWindow::UpdateDialog() {
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TYPE_COMBO ));
		CComboBox * pcbStringToFindCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TO_FIND_COMBO ));
		CStatic * peCharType = static_cast<CStatic *>(FindChild( CNewStringSearchLayout::MX_SSI_CHAR_TYPE_LABEL ));
		CComboBox * pcbCharTypeCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_CHAR_TYPE_COMBO ));
		CEdit * peEdit = static_cast<CEdit *>(FindChild( CNewStringSearchLayout::MX_SSI_PREVIEW_EDIT ));
		CEdit * peEdit2 = static_cast<CEdit *>(FindChild( CNewStringSearchLayout::MX_SSI_PREVIEW_HEX_EDIT ));
		CCheckButton * pcbHexCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_HEX_CHECK ));
		CCheckButton * pcbEscapesCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_ESCAPES_CHECK ));

		CStatic * psRegexEncodingStatic = static_cast<CStatic *>(FindChild( CNewStringSearchLayout::MX_SSI_REGEX_ENCODING_LABEL ));
		CComboBox * pcbRegexEncodingCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_REGEX_ENCODING_COMBO ));

		if ( pcbCombo && pcbStringToFindCombo &&
			peCharType &&
			pcbHexCheck && pcbEscapesCheck && peEdit && peEdit2 && pcbCharTypeCombo &&
			pcbRegexEncodingCombo && psRegexEncodingStatic ) {
			CUtilities::MX_DATA_TYPES dtArrayType = static_cast<CUtilities::MX_DATA_TYPES>(pcbCharTypeCombo->GetCurSelItemData());
			uint32_t uiStringType = pcbCombo->GetCurSelItemData();
			pcbEscapesCheck->SetEnabled( uiStringType != MX_ST_BYTE_ARRAY );
			if ( uiStringType == MX_ST_BYTE_ARRAY ) {
				LSW_RECT rRect = pcbCombo->ClientRect();
				rRect = rRect.ClientToScreen( pcbCombo->Wnd() ).ScreenToClient( Wnd() );
				LSW_RECT rInvalidateRect = rRect;
				LSW_RECT rCharTypeRect = peCharType->ClientRect();
				rCharTypeRect = rCharTypeRect.ClientToScreen( peCharType->Wnd() ).ScreenToClient( Wnd() );
				rRect.SetWidth( rCharTypeRect.left - rRect.left - 2 );
				::MoveWindow( pcbCombo->Wnd(), rRect.left, rRect.top, rRect.Width(), rRect.Height(), FALSE );
				::InvalidateRect( Wnd(), &rInvalidateRect, TRUE );
				pcbHexCheck->SetEnabled( !CUtilities::DataTypeIsFloat( dtArrayType ) );

				CStatic * peByteArrayHelper = static_cast<CStatic *>(FindChild( CNewStringSearchLayout::MX_SSI_BYTE_HELPER_LABEL ));
				if ( peByteArrayHelper ) {
					switch ( dtArrayType ) {
						case CUtilities::MX_DT_VOID : {
							peByteArrayHelper->SetTextW( _DEC_WS_21FF525C__u_i_8_16_32_64_____unsigned__int__f_16_32_64____float__d___f64__r_nExample__ui8_0x64_0___i16__200_____f_1_0.c_str() );
							break;
						}
						default : {
							if ( CUtilities::DataTypeIsFloat( dtArrayType ) ) {
								peByteArrayHelper->SetTextW( _DEC_WS_350A4967_All_inputs_interpreted_as_floating_point_.c_str() );
							}
							else {
								if ( pcbHexCheck->IsChecked() ) {
									peByteArrayHelper->SetTextW( _DEC_WS_710C03BD_All_inputs_interpreted_in_base_16_.c_str() );
								}
								else {
									peByteArrayHelper->SetTextW( _DEC_WS_6952586D__digits____base_10__0x_digits____base_16__0b_digits____base_2__0_digits____base_8__r_nExample__90_0x6C_0b0101_072____0x20____0b11110000.c_str() );
								}
							}
						}
					}
				}
			}
			else if ( uiStringType == MX_ST_REGEX ) {
				LSW_RECT rRect = pcbCombo->ClientRect();
				rRect = rRect.ClientToScreen( pcbCombo->Wnd() ).ScreenToClient( Wnd() );
				LSW_RECT rInvalidateRect = rRect;
				LSW_RECT rCharTypeRect = psRegexEncodingStatic->ClientRect();
				rCharTypeRect = rCharTypeRect.ClientToScreen( psRegexEncodingStatic->Wnd() ).ScreenToClient( Wnd() );
				rRect.SetWidth( rCharTypeRect.left - rRect.left - 2 );
				::MoveWindow( pcbCombo->Wnd(), rRect.left, rRect.top, rRect.Width(), rRect.Height(), FALSE );
				::InvalidateRect( Wnd(), &rInvalidateRect, TRUE );
				pcbHexCheck->SetEnabled( FALSE );
			}
			else {
				LSW_RECT rRect = pcbCombo->ClientRect();
				rRect = rRect.ClientToScreen( pcbCombo->Wnd() ).ScreenToClient( Wnd() );
				rRect.SetWidth( m_lOriginalTypeWidth );
				::MoveWindow( pcbCombo->Wnd(), rRect.left, rRect.top, rRect.Width(), rRect.Height(), FALSE );
				pcbHexCheck->SetEnabled( FALSE );
			}
			peCharType->SetVisible( uiStringType == MX_ST_BYTE_ARRAY );
			pcbCharTypeCombo->SetVisible( uiStringType == MX_ST_BYTE_ARRAY );
			pcbRegexEncodingCombo->SetVisible( uiStringType == MX_ST_REGEX );
			psRegexEncodingStatic->SetVisible( uiStringType == MX_ST_REGEX );

			CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_WILDCARD_CHECK ));
			if ( pcbCheck ) {
				pcbCheck->SetEnabled( uiStringType != MX_ST_BYTE_ARRAY && uiStringType != MX_ST_REGEX );
			}

			
#define MX_CHECK_UTF( ID, CHECK )																		\
	{																									\
		CCheckButton * pcbTemp = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::ID ));	\
		if ( pcbTemp ) { pcbTemp->SetEnabled( CHECK ); }												\
	}
			MX_CHECK_UTF( MX_SSI_ALIGNED_CHECK, uiStringType != MX_ST_REGEX );
			MX_CHECK_UTF( MX_SSI_IGNORE_CASE_CHECK, IsString( uiStringType ) )
			MX_CHECK_UTF( MX_SSI_WHOLE_WORD_CHECK, IsString( uiStringType ) )
			MX_CHECK_UTF( MX_SSI_LINGUISTIC_IGNORECASE_CHECK, IsString( uiStringType ) )
			MX_CHECK_UTF( MX_SSI_LINGUISTIC_IGNOREDIACRITIC_CHECK, IsString( uiStringType ) )
			MX_CHECK_UTF( MX_SSI_NORM_IGNOREKANATYPE_CHECK, IsString( uiStringType ) && uiStringType != MX_ST_ANSI )
			MX_CHECK_UTF( MX_SSI_NORM_IGNORENONSPACE_CHECK, IsString( uiStringType ) )
			MX_CHECK_UTF( MX_SSI_NORM_IGNORESYMBOLS_CHECK, IsString( uiStringType ) )
			MX_CHECK_UTF( MX_SSI_NORM_IGNOREWIDTH_CHECK, IsString( uiStringType ) && uiStringType != MX_ST_ANSI )

			MX_CHECK_UTF( MX_SSI_ALIGNED_CHECK, (uiStringType != MX_ST_BYTE_ARRAY || dtArrayType != CUtilities::MX_DT_VOID) && uiStringType != MX_ST_REGEX )
#undef MX_CHECK_UTF

#define MX_TOGGLE_VIS( ID, CHECK )																		\
	{																									\
		CWidget * pcbTemp = static_cast<CWidget *>(FindChild( CNewStringSearchLayout::ID ));			\
		if ( pcbTemp ) { pcbTemp->SetVisible( CHECK ); }												\
	}
			MX_TOGGLE_VIS( MX_SSI_LINGUISTIC_IGNORECASE_CHECK, uiStringType != MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_REGEX_SINGLELINE, uiStringType == MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_LINGUISTIC_IGNOREDIACRITIC_CHECK, uiStringType != MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_REGEX_MULTILINE, uiStringType == MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_NORM_IGNOREKANATYPE_CHECK, uiStringType != MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_REGEX_EXTENDED, uiStringType == MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_NORM_IGNORENONSPACE_CHECK, uiStringType != MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_REGEX_FIND_LONGEST, uiStringType == MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_NORM_IGNORESYMBOLS_CHECK, uiStringType != MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_NORM_IGNOREWIDTH_CHECK, uiStringType != MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_REGEX_NEGATE_SINGLELINE, uiStringType == MX_ST_REGEX );

			MX_TOGGLE_VIS( MX_SSI_PREVIEW_LABEL, uiStringType != MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_REGEX_FLAVOR_LABEL, uiStringType == MX_ST_REGEX );

			MX_TOGGLE_VIS( MX_SSI_PREVIEW_EDIT, uiStringType != MX_ST_REGEX );
			MX_TOGGLE_VIS( MX_SSI_REGEX_FLAVOR_COMBO, uiStringType == MX_ST_REGEX );

			MX_TOGGLE_VIS( MX_SSI_UTF_HELPER_LABEL, uiStringType != MX_ST_REGEX && uiStringType != MX_ST_BYTE_ARRAY );
			MX_TOGGLE_VIS( MX_SSI_UTF_HELPER_COMBO, uiStringType != MX_ST_REGEX && uiStringType != MX_ST_BYTE_ARRAY );

			MX_TOGGLE_VIS( MX_SSI_REGEX_HELPER_LABEL, uiStringType == MX_ST_REGEX && uiStringType != MX_ST_BYTE_ARRAY );
			MX_TOGGLE_VIS( MX_SSI_REGEX_HELPER_COMBO, uiStringType == MX_ST_REGEX && uiStringType != MX_ST_BYTE_ARRAY );
			
			MX_TOGGLE_VIS( MX_SSI_BYTE_HELPER_LABEL, uiStringType == MX_ST_BYTE_ARRAY );
#undef MX_TOGGLE_VIS
			

			std::vector<uint8_t> vFinalBytes, vFinalMeta;
			CUtilities::MX_WILDCARD_USAGE wuWildcard;
			if ( GetFinalSearchBytes( vFinalBytes, vFinalMeta, nullptr, wuWildcard ) ) {
				// Convert to UTF-16 for display.
				CSecureWString swsPreview;
				switch ( uiStringType ) {
					case MX_ST_ANSI : {
						CSecureString ssPreview;
						if ( vFinalBytes.size() ) {
							CSecureString ssTmp = CUtilities::FromCodePage( CSecureString( reinterpret_cast<const CSecureString::value_type *>(vFinalBytes.data()), vFinalBytes.size() / sizeof( CSecureString::value_type ) ),
								CP_ACP );
							CUtilities::BytesToCString( &ssTmp[0], ssTmp.size(), ssPreview, false, true );
						}
						swsPreview = ee::CExpEval::StringToWString( ssPreview );
						break;
					}
					case MX_ST_UTFANY : {}
					case MX_ST_UTF8 : {
						if ( vFinalBytes.size() ) {
							swsPreview = ee::CExpEval::StringToWString( reinterpret_cast<const char *>(&vFinalBytes[0]), vFinalBytes.size() );
						}
						break;
					}
					case MX_ST_UTF16_BE : {}
					case MX_ST_UTF16 : {
						if ( vFinalBytes.size() ) {
							const wchar_t * puiIn = reinterpret_cast<const wchar_t *>(&vFinalBytes[0]);
							size_t sLen = (vFinalBytes.size() * sizeof( vFinalBytes[0] )) / sizeof( uint16_t );
							for ( size_t I = 0; I < sLen; ++I ) {
								swsPreview.push_back( puiIn[I] );
							}
						}
						break;
					}
					case MX_ST_UTF32_BE : {}
					case MX_ST_UTF32 : {
						if ( vFinalBytes.size() ) {
							size_t sLen = (vFinalBytes.size() * sizeof( vFinalBytes[0] )) / sizeof( uint32_t );
							swsPreview = ee::CExpEval::Utf32StringToWString( reinterpret_cast<const uint32_t *>(&vFinalBytes[0]), sLen );
						}
						break;
					}
					case MX_ST_REGEX : {
						for ( size_t I = 0; I < vFinalBytes.size(); ++I ) {
							swsPreview.push_back( vFinalBytes[I] );
						}
						break;
					}
					case MX_ST_BYTE_ARRAY : {
						if ( vFinalBytes.size() ) {
#define MX_CASE( CASE, TYPE, FUNC )															\
	case CUtilities::CASE : {																\
		const TYPE * puiIn = reinterpret_cast<const TYPE *>(&vFinalBytes[0]);				\
		size_t sLen = (vFinalBytes.size() * sizeof( vFinalBytes[0] )) / sizeof( TYPE );		\
		for ( size_t I = 0; I < sLen; ++I ) {												\
			if ( swsPreview.size() ) { swsPreview.push_back( L' ' ); }						\
			switch ( vFinalMeta[I] ) {														\
				case '*' : { swsPreview.push_back( L'*' ); break; }							\
				case '?' : { swsPreview.push_back( L'?' ); break; }							\
				default : { CUtilities::FUNC( puiIn[I], swsPreview ); }						\
			}																				\
		}																					\
		break;																				\
	}

#define MX_CASE_PAIR( TYPE )		MX_CASE( MX_DT_INT ## TYPE, int ## TYPE ## _t, ToSigned ) MX_CASE( MX_DT_UINT ## TYPE, uint ## TYPE ## _t, ToUnsigned )


							switch ( dtArrayType ) {
								MX_CASE_PAIR( 8 )
								MX_CASE_PAIR( 16 )
								MX_CASE_PAIR( 32 )
								MX_CASE_PAIR( 64 )
								case CUtilities::MX_DT_FLOAT16 : {
									const uint16_t * pui16Array = reinterpret_cast<const uint16_t *>(&vFinalBytes[0]);

									size_t sLen = (vFinalBytes.size() * sizeof( vFinalBytes[0] )) / sizeof( CFloat16 );
									for ( size_t I = 0; I < sLen; ++I ) {
										if ( swsPreview.size() ) { swsPreview.push_back( L' ' ); }
										switch ( vFinalMeta[I] ) {
											case '*' : {
												swsPreview.push_back( L'*' );
												break;
											}
											case '?' : {
												swsPreview.push_back( L'?' );
												break;
											}
											default : {
												CUtilities::ToDouble( CFloat16( pui16Array[I] ).Value(), swsPreview, -MX_FLOAT16_SIG_DIG );
											}
										}
									}
									break;
								}
								case CUtilities::MX_DT_FLOAT : {
									const float * pfIn = reinterpret_cast<const float *>(&vFinalBytes[0]);
									size_t sLen = (vFinalBytes.size() * sizeof( vFinalBytes[0] )) / sizeof( float );
									for ( size_t I = 0; I < sLen; ++I ) {
										if ( swsPreview.size() ) { swsPreview.push_back( L' ' ); }
										switch ( vFinalMeta[I] ) {
											case '*' : {
												swsPreview.push_back( L'*' );
												break;
											}
											case '?' : {
												swsPreview.push_back( L'?' );
												break;
											}
											default : {
												CUtilities::ToDouble( pfIn[I], swsPreview, -MX_FLOAT32_SIG_DIG );
											}
										}
									}
									break;
								}
								case CUtilities::MX_DT_DOUBLE : {
									const double * pfIn = reinterpret_cast<const double *>(&vFinalBytes[0]);
									size_t sLen = (vFinalBytes.size() * sizeof( vFinalBytes[0] )) / sizeof( double );
									for ( size_t I = 0; I < sLen; ++I ) {
										if ( swsPreview.size() ) { swsPreview.push_back( L' ' ); }
										switch ( vFinalMeta[I] ) {
											case '*' : {
												swsPreview.push_back( L'*' );
												break;
											}
											case '?' : {
												swsPreview.push_back( L'?' );
												break;
											}
											default : {
												CUtilities::ToDouble( pfIn[I], swsPreview, -MX_FLOAT64_SIG_DIG );
											}
										}
									}
									break;
								}
								case CUtilities::MX_DT_VOID : {
									const uint8_t * pfIn = reinterpret_cast<const uint8_t *>(&vFinalBytes[0]);
									for ( size_t I = 0; I < vFinalMeta.size(); ++I ) {
										if ( swsPreview.size() ) { swsPreview.push_back( L' ' ); }
										switch ( vFinalMeta[I] & CUtilities::MX_WF_MASK ) {
											case CUtilities::MX_WF_ANY_CHAR : {
												swsPreview.push_back( L'?' );
												break;
											}
											case CUtilities::MX_WF_ANY : {
												swsPreview.push_back( L'*' );
												break;
											}
											default : {
												switch ( vFinalMeta[I] & CUtilities::MX_DT_MASK ) {

#define MX_CASE_INT( TYPE )																				\
	case CUtilities::MX_DT_INT ## TYPE : {																\
		CUtilities::ToSigned( (*reinterpret_cast<const int ## TYPE ## _t *>(pfIn)), swsPreview );		\
		pfIn += sizeof( int ## TYPE ## _t );															\
		break;																							\
	}																									\
	case CUtilities::MX_DT_UINT ## TYPE : {																\
		CUtilities::ToUnsigned( (*reinterpret_cast<const uint ## TYPE ## _t *>(pfIn)), swsPreview );	\
		pfIn += sizeof( uint ## TYPE ## _t );															\
		break;																							\
	}

													MX_CASE_INT( 8 )
													MX_CASE_INT( 16 )
													MX_CASE_INT( 32 )
													MX_CASE_INT( 64 )
													case CUtilities::MX_DT_FLOAT16 : {
														CUtilities::ToDouble( CFloat16( pfIn ).Value(), swsPreview, -MX_FLOAT16_SIG_DIG );
														pfIn += sizeof( CFloat16 );
														break;
													}
													case CUtilities::MX_DT_FLOAT : {
														CUtilities::ToDouble( (*reinterpret_cast<const float *>(pfIn)), swsPreview, -MX_FLOAT32_SIG_DIG );
														pfIn += sizeof( float );
														break;
													}
													case CUtilities::MX_DT_DOUBLE : {
														CUtilities::ToDouble( (*reinterpret_cast<const double *>(pfIn)), swsPreview, -MX_FLOAT64_SIG_DIG );
														pfIn += sizeof( double );
														break;
													}
#undef MX_CASE_INT
												}
											}
										}
									}
									break;
								}
							}
						}
#undef MX_CASE_PAIR
#undef MX_CASE
						break;
					}
				}
				
				peEdit->SetTextW( swsPreview.c_str() );

				CSecureString ssPreview;
				if ( vFinalBytes.size() ) {
					if ( uiStringType == MX_ST_BYTE_ARRAY && vFinalMeta.size() ) {
						if ( dtArrayType == CUtilities::MX_DT_VOID ) {
							CUtilities::BytesToHexWithSpaces_MultiFormat( &vFinalBytes[0], vFinalBytes.size(), ssPreview, vFinalMeta.data(), vFinalMeta.size(), false );
						}
						else {
							CUtilities::BytesToHexWithSpaces( &vFinalBytes[0], vFinalBytes.size(), ssPreview, vFinalMeta.data(), vFinalMeta.size(), CUtilities::DataTypeSize( dtArrayType ), false );
						}
					}
					else if ( uiStringType == MX_ST_UTFANY ) {
						const uint32_t uiStrTypes[] = {
							MX_ST_UTF16,
							MX_ST_UTF16_BE,
							MX_ST_UTF32,
							MX_ST_UTF32_BE,
						};
						std::set<std::vector<uint8_t>> sVariants;
						sVariants.insert( vFinalBytes );
						for ( size_t I = 0; I < MX_ELEMENTS( uiStrTypes ); ++I ) {
							std::vector<uint8_t> vTmp, vTmpMeta;
							CUtilities::MX_WILDCARD_USAGE wuTmp;
							if ( GetFinalSearchBytes( uiStrTypes[I],
								vTmp, vTmpMeta, nullptr, wuTmp ) ) {
								if ( !sVariants.count( vTmp ) ) {
									sVariants.insert( vTmp );
								}
							}
						}
						//swsPreview = CUtilities::StringToWString( reinterpret_cast<const char *>(&vFinalBytes[0]), vFinalBytes.size() );
						//CUtilities::BytesToHexWithSpaces( &vFinalBytes[0], vFinalBytes.size(), ssPreview, false );
						for ( auto I = sVariants.begin(); I != sVariants.end(); ++I ) {
							if ( ssPreview.size() ) {
								ssPreview.push_back( ',' );
								ssPreview.push_back( ' ' );
							}
							CUtilities::BytesToHexWithSpaces( I->data(), I->size(), ssPreview, false );
						}
					}
					else {
						CUtilities::BytesToHexWithSpaces( &vFinalBytes[0], vFinalBytes.size(), ssPreview, false );
					}
				}
				peEdit2->SetTextW( ee::CExpEval::StringToWString( ssPreview ).c_str() );
			}
		}
	}

	// Gets the final search bytes.
	bool CNewStringSearchWindow::GetFinalSearchBytes( std::vector<uint8_t> &_vResult, std::vector<uint8_t> &_vMeta, CSecureWString * _pswsError, CUtilities::MX_WILDCARD_USAGE &_wuWildcardUsage ) {
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TYPE_COMBO ));
		CComboBox * pcbStringToFindCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TO_FIND_COMBO ));
		CComboBox * pcbCharTypeCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_CHAR_TYPE_COMBO ));
		CEdit * peEdit = static_cast<CEdit *>(FindChild( CNewStringSearchLayout::MX_SSI_PREVIEW_EDIT ));
		CEdit * peEdit2 = static_cast<CEdit *>(FindChild( CNewStringSearchLayout::MX_SSI_PREVIEW_HEX_EDIT ));
		CCheckButton * pcbHexCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_HEX_CHECK ));
		CCheckButton * pcbEscapesCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_ESCAPES_CHECK ));
		CCheckButton * pcbWildCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_WILDCARD_CHECK ));
		CCheckButton * pcbWholeWordOnly = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_WHOLE_WORD_CHECK ));
		if ( pcbCombo ) {
			return GetFinalSearchBytes( pcbCombo->GetCurSelItemData(),
				_vResult, _vMeta, _pswsError, _wuWildcardUsage );
		}
		return false;
	}

	// Gets the final search bytes for a given string type.
	bool CNewStringSearchWindow::GetFinalSearchBytes( uint32_t _uiStringType,
		std::vector<uint8_t> &_vResult, std::vector<uint8_t> &_vMeta, CSecureWString * _pswsError, CUtilities::MX_WILDCARD_USAGE &_wuWildcardUsage ) {
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TYPE_COMBO ));
		CComboBox * pcbStringToFindCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_STRING_TO_FIND_COMBO ));
		CComboBox * pcbCharTypeCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_CHAR_TYPE_COMBO ));
		CEdit * peEdit = static_cast<CEdit *>(FindChild( CNewStringSearchLayout::MX_SSI_PREVIEW_EDIT ));
		CEdit * peEdit2 = static_cast<CEdit *>(FindChild( CNewStringSearchLayout::MX_SSI_PREVIEW_HEX_EDIT ));
		CCheckButton * pcbHexCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_HEX_CHECK ));
		CCheckButton * pcbEscapesCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_ESCAPES_CHECK ));
		CCheckButton * pcbWildCheck = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_WILDCARD_CHECK ));
		CCheckButton * pcbWholeWordOnly = static_cast<CCheckButton *>(FindChild( CNewStringSearchLayout::MX_SSI_WHOLE_WORD_CHECK ));
		if ( pcbCombo && pcbStringToFindCombo && pcbHexCheck && pcbEscapesCheck && pcbWildCheck && peEdit && peEdit2 && pcbWholeWordOnly ) {
			CSecureString sUtf;
			switch ( _uiStringType ) {
				case MX_ST_ANSI : {}
				case MX_ST_REGEX : {}
				case MX_ST_UTFANY : {}
				case MX_ST_UTF8 : {}
				case MX_ST_UTF16_BE : {}
				case MX_ST_UTF32_BE : {}
				case MX_ST_UTF16 : {}
				case MX_ST_UTF32 : {
					sUtf = ee::CExpEval::ToUtf8( pcbStringToFindCombo->GetTextW() );
					break;
				}
				case MX_ST_BYTE_ARRAY : {
					CUtilities::MX_DATA_TYPES dtCharType = pcbCharTypeCombo ? static_cast<CUtilities::MX_DATA_TYPES>(pcbCharTypeCombo->GetCurSelItemData()) : CUtilities::MX_DT_UINT8;
					if ( (dtCharType & CUtilities::MX_DT_MASK) == CUtilities::MX_DT_VOID ) {
						// Base 0 defaults to base 10 but allows the string to override individual numbers with x (base-16), 0 (base-8), and b (base-2).
						std::string sTmp = pcbStringToFindCombo->GetTextA();
						sUtf = CUtilities::NumberStringToString_MultiFormat( CUtilities::Strip( sTmp, true ), pcbHexCheck->IsChecked() ? 16 : 0,
							CUtilities::MX_DT_UINT32,
							&_vMeta );
					}
					else {
						uint64_t uiMax = static_cast<uint64_t>(-1);
						if ( !CUtilities::DataTypeIsFloat( dtCharType ) ) {
							uiMax = 0;
							for ( size_t I = CUtilities::DataTypeSize( dtCharType ); I--; ) {
								uiMax <<= 8;
								uiMax |= 0xFF;
							}
						}
						// Base 0 defaults to base 10 but allows the string to override individual numbers with x (base-16), 0 (base-8), and b (base-2).
						std::string sTmp = pcbStringToFindCombo->GetTextA();
						sUtf = CUtilities::NumberStringToString( CUtilities::Strip( sTmp, true ), pcbHexCheck->IsChecked() ? 16 : 0,
							uiMax, dtCharType,
							&_vMeta );
					}
					break;
				}
			}

			_wuWildcardUsage = CUtilities::MX_WU_NONE;
			switch ( _uiStringType ) {
				case MX_ST_ANSI : {}
				case MX_ST_UTFANY : {}
				case MX_ST_UTF8 : {}
				case MX_ST_UTF16 : {}
				case MX_ST_UTF16_BE : {}
				case MX_ST_UTF32_BE : {}
				case MX_ST_UTF32 : {
					if ( pcbWildCheck->IsChecked() ) {
						_wuWildcardUsage = ClassifyWildcardUseInUtf8String( sUtf );
					}
					break;
				}
				case MX_ST_BYTE_ARRAY : {
					_wuWildcardUsage = ClassifyWildcardUseInRawString( _vMeta );
					break;
				}
			}
			

			CSecureString sResolved;
			if ( pcbEscapesCheck->IsChecked() && _uiStringType != MX_ST_BYTE_ARRAY ) {
				CUtilities::ResolveAllEscapes( sUtf, sResolved, true );
			}
			else {
				sResolved = sUtf;
			}


			bool bByteSwapped = false;
			if ( _uiStringType == MX_ST_ANSI ) {
				sResolved = CUtilities::ToCodePage( sResolved, CP_ACP );
			}
			else {
				if ( _uiStringType == MX_ST_REGEX && sResolved.size() ) {
					if ( pcbWholeWordOnly->IsChecked() ) {
						sResolved.insert( sResolved.begin(), '(' );
						sResolved.push_back( ')' );
						sResolved.insert( sResolved.begin(), 'b' );
						sResolved.push_back( '\\' );
						sResolved.insert( sResolved.begin(), '\\' );
						sResolved.push_back( 'b' );
					}
					pcbCombo = static_cast<CComboBox *>(FindChild( CNewStringSearchLayout::MX_SSI_REGEX_ENCODING_COMBO ));
					if ( pcbCombo ) {
						CUtilities::MX_REGEX_ENCODING reRegexEncoding = static_cast<CUtilities::MX_REGEX_ENCODING>(pcbCombo->GetCurSelItemData());
						::OnigEncodingType * poetEncoding = COnigurumaSystem::RegexEncodingToLibrary( reRegexEncoding );
						if ( poetEncoding == ONIG_ENCODING_UTF16_LE ) {
							_uiStringType = MX_ST_UTF16;
						}
						else if ( poetEncoding == ONIG_ENCODING_UTF32_LE ) {
							_uiStringType = MX_ST_UTF32;
						}
						else if ( poetEncoding == ONIG_ENCODING_UTF16_BE ) {
							_uiStringType = MX_ST_UTF16;
							bByteSwapped = true;
						}
						else if ( poetEncoding == ONIG_ENCODING_UTF32_BE ) {
							_uiStringType = MX_ST_UTF32;
							bByteSwapped = true;
						}
						else {
							sResolved = CUtilities::ToCodePage( sResolved, CUtilities::RegexCodePageToCodePage( reRegexEncoding ) );
						}

					}
				}
			}


			switch ( _uiStringType ) {
				case MX_ST_BYTE_ARRAY : {}
				case MX_ST_UTFANY : {}
				case MX_ST_UTF8 : {}
				case MX_ST_REGEX : {}
				case MX_ST_ANSI : {
					// Byte-by-byte conversion.
					for ( size_t I = 0; I < sResolved.size(); ++I ) {
						_vResult.push_back( sResolved[I] );
					}
					return true;
				}
				case MX_ST_UTF16_BE : { bByteSwapped = true; }
				case MX_ST_UTF16 : {
					// Byte-by-two-byte conversion.
					CSecureWString swsUtf16 = ee::CExpEval::ToUtf16( sResolved );
					if ( bByteSwapped ) {
						for ( size_t I = 0; I < swsUtf16.size(); ++I ) {
							_vResult.push_back( (swsUtf16[I] >> 8) & 0xFF );
							_vResult.push_back( swsUtf16[I] & 0xFF );
						}
					}
					else {
						for ( size_t I = 0; I < swsUtf16.size(); ++I ) {
							_vResult.push_back( swsUtf16[I] & 0xFF );
							_vResult.push_back( (swsUtf16[I] >> 8) & 0xFF );
						}
					}
					return true;
				}
				case MX_ST_UTF32_BE : { bByteSwapped = true; }
				case MX_ST_UTF32 : {
					// Byte-by-four-byte conversion.
					std::u32string u32ExpandedResult = ee::CExpEval::ToUtf32( sResolved );
					if ( bByteSwapped ) {
						for ( size_t I = 0; I < u32ExpandedResult.size(); ++I ) {
							_vResult.push_back( (u32ExpandedResult[I] >> 24) & 0xFF );
							_vResult.push_back( (u32ExpandedResult[I] >> 16) & 0xFF );
							_vResult.push_back( (u32ExpandedResult[I] >> 8) & 0xFF );
							_vResult.push_back( u32ExpandedResult[I] & 0xFF );							
						}
					}
					else {
						for ( size_t I = 0; I < u32ExpandedResult.size(); ++I ) {
							_vResult.push_back( u32ExpandedResult[I] & 0xFF );
							_vResult.push_back( (u32ExpandedResult[I] >> 8) & 0xFF );
							_vResult.push_back( (u32ExpandedResult[I] >> 16) & 0xFF );
							_vResult.push_back( (u32ExpandedResult[I] >> 24) & 0xFF );
						}
					}
					return true;
				}
			}
		}
		return false;
	}

	// Classifies the wildcard usage in a string.
	CUtilities::MX_WILDCARD_USAGE CNewStringSearchWindow::ClassifyWildcardUseInUtf8String( const CSecureString &_ssString ) {
		size_t sWildcardCount = 0;
		size_t sCharCount = 0, sChars = 0;
		for ( size_t I = 0; I < _ssString.size(); I += sChars ) {
			uint32_t ui32Next = ee::CExpEval::NextUtf8Char( &_ssString[I], _ssString.size() - I, &sChars );
			if ( ui32Next != MX_UTF_INVALID ) {
				++sCharCount;
				if ( ui32Next == '*' || ui32Next == '?' ) { ++sWildcardCount; }
			}
		}
		if ( !sWildcardCount ) { return CUtilities::MX_WU_NONE; }
		if ( sWildcardCount == sCharCount ) { return CUtilities::MX_WU_FULL; }
		return CUtilities::MW_WU_SOME;
	}

	// Classifies the wildcard usage in a raw string.
	CUtilities::MX_WILDCARD_USAGE CNewStringSearchWindow::ClassifyWildcardUseInRawString( const std::vector<uint8_t> &_vMeta ) {
		size_t sWildcardCount = 0;
		size_t sCharCount = 0;
		for ( size_t I = 0; I < _vMeta.size(); ++I ) {
			++sCharCount;
			if ( _vMeta[I] == '*' || _vMeta[I] == '?' ) { ++sWildcardCount; }
		}
		if ( !sWildcardCount ) { return CUtilities::MX_WU_NONE; }
		if ( sWildcardCount == sCharCount ) { return CUtilities::MX_WU_FULL; }
		return CUtilities::MW_WU_SOME;
	}

}	// namespace mx

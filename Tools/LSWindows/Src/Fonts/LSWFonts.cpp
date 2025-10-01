#include "LSWFonts.h"
#include "../Base/LSWBase.h"
#include "../Helpers/LSWHelpers.h"

#include <vector>

namespace lsw {

	// == Private Statics.
	// 1-block scripts.
	static constexpr CFonts::LSW_UC_RANGE g_lrGreek[]			= { { 0x0370, 0x03FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrCyrillic[]		= { { 0x0400, 0x04FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrArmenian[]		= { { 0x0530, 0x058F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrHebrew[]			= { { 0x0590, 0x05FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrArabic[]			= { { 0x0600, 0x06FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrSyriac[]			= { { 0x0700, 0x074F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrThaana[]			= { { 0x0780, 0x07BF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrDevanagari[]		= { { 0x0900, 0x097F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrBengali[]			= { { 0x0980, 0x09FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrGurmukhi[]		= { { 0x0A00, 0x0A7F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrGujarati[]		= { { 0x0A80, 0x0AFF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrOriya[]			= { { 0x0B00, 0x0B7F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrTamil[]			= { { 0x0B80, 0x0BFF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrTelugu[]			= { { 0x0C00, 0x0C7F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrKannada[]			= { { 0x0C80, 0x0CFF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrMalayalam[]		= { { 0x0D00, 0x0D7F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrSinhala[]			= { { 0x0D80, 0x0DFF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrThai[]			= { { 0x0E00, 0x0E7F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrLao[]				= { { 0x0E80, 0x0EFF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrTibetan[]			= { { 0x0F00, 0x0FFF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrMyanmar[]			= { { 0x1000, 0x109F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrGeorgian[]		= { { 0x10A0, 0x10FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrKhmer[]			= { { 0x1780, 0x17FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrKhmerSym[]		= { { 0x19E0, 0x19FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrOgham[]			= { { 0x1680, 0x169F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrRunic[]			= { { 0x16A0, 0x16FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrNko[]				= { { 0x07C0, 0x07FF } };

	// Latin & extensions commonly needed.
	static constexpr CFonts::LSW_UC_RANGE g_lrLatinCore[]		= { { 0x0000, 0x00FF }, {0x0100, 0x017F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrCombining[]		= { { 0x0300, 0x036F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrLatExtAdd[]		= { { 0x1E00, 0x1EFF } }; // includes Vietnamese letters (1EA0–1EFF subset)

	// Japanese components.
	static constexpr CFonts::LSW_UC_RANGE g_lrHiragana[]		= { { 0x3040, 0x309F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrKatakana[]		= { { 0x30A0, 0x30FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrKanaExtA[]		= { { 0x1B100,0x1B12F } }; // optional (U+1B100..)
	// Korean components.
	static constexpr CFonts::LSW_UC_RANGE g_lrHangulJamo[]		= { { 0x1100, 0x11FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrHangulSyll[]		= { { 0xAC00, 0xD7AF } };

	// Han (shared by CJK).
	static constexpr CFonts::LSW_UC_RANGE g_lrCjkSymbols[]		= { { 0x3000, 0x303F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrCjkUnified[]		= { { 0x4E00, 0x9FFF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrCjkExtA[]			= { { 0x3400, 0x4DBF } };
	// Bopomofo (Traditional Chinese/Taiwan).
	static constexpr CFonts::LSW_UC_RANGE g_lrBopomofo[]		= { { 0x3100, 0x312F } };
	static constexpr CFonts::LSW_UC_RANGE g_lrBopomofoExt[]		= { { 0x31A0, 0x31BF } };

	// Symbol-ish blocks (heuristic).
	static constexpr CFonts::LSW_UC_RANGE g_lrArrows[]			= { { 0x2190, 0x21FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrGeomShapes[]		= { { 0x25A0, 0x25FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrMiscSymbols[]		= { { 0x2600, 0x26FF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrDingbats[]		= { { 0x2700, 0x27BF } };
	static constexpr CFonts::LSW_UC_RANGE g_lrPrivateUse[]		= { { 0xF000, 0xF8FF } };


	static const CFonts::LSW_WRITING_SYSTEM g_wsList[] = {
		// pwszName					pwszSample																							prRanges			ui32CountRanges	pfSpecial
		{ L"Any",					L"\x41\x61\x42\x62\x7A\x5A",													/*AaBbzZ*/			nullptr,			0,				nullptr },
		{ L"Latin",					L"\x41\x61\u00C3\u00E1\x5A\x7A",												/*AaÃáZz*/			g_lrLatinCore,		2,				nullptr },
		{ L"Greek",					L"\u0393\u03B1\u03A9\u03C9",													/*ΓαΩω*/			g_lrGreek,			1,				nullptr },
		{ L"Cyrillic",				L"\u0414\u0434\u0436\u044F",													/*Дджя*/			g_lrCyrillic,		1,				nullptr },
		{ L"Armenian",				L"\u053F\u054F\u056F\u057F",													/*ԿՏկտ*/			g_lrArmenian,		1,				nullptr },
		{ L"Hebrew",				L"\u05D0\u05D1\u05D2\u05D3",													/*אבגדאבגד*/		g_lrHebrew,			1,				nullptr },
		{ L"Arabic",				L"\u0623\u0628\u062C\u062F\u064A\u0629\x20\u0639\u0631\u0628\u064A\u0629",		/*أبجدية عربية*/	g_lrArabic,			1,				nullptr },
		{ L"Syriac",				L"\u0715\u0725\u0716\u0726",													/*ܕܥܖܦ*/				g_lrSyriac,			1,				nullptr },
		{ L"Thaana",				L"\u0784\u0794\u078C\u078D",													/*ބޔތލ*/			g_lrThaana,			1,				nullptr },
		{ L"Devanagari",			L"\u0905\u0915\u0925\u0935",													/*अकथव*/			g_lrDevanagari,		1,				nullptr },
		{ L"Bengali",				L"\u0986\u0996\u09A6\u09B6",													/*আখদশ*/			g_lrBengali,		1,				nullptr },
		{ L"Gurmukhi",				L"\u0A05\u0A15\u0A25\u0A35",													/*ਅਕਥਵ*/			g_lrGurmukhi,		1,				nullptr },
		{ L"Gujarati",				L"\u0A85\u0A95\u0AA5\u0AB5",													/*અકથવ*/			g_lrGujarati,		1,				nullptr },
		{ L"Oriya",					L"\u0B06\u0B16\u0B2B\u0B36",													/*ଆଖଫଶ*/			g_lrOriya,			1,				nullptr },
		{ L"Tamil",					L"\u0B89\u0B99\u0BA9\u0BB9",													/*உஙனஹ*/		g_lrTamil,			1,				nullptr },
		{ L"Telugu",				L"\u0C05\u0C15\u0C25\u0C35",													/*అకథవ*/			g_lrTelugu,			1,				nullptr },
		{ L"Kannada",				L"\u0C85\u0C95\u0CA5\u0CB5",													/*ಅಕಥವ*/			g_lrKannada,		1,				nullptr },
		{ L"Malayalam",				L"\u0D05\u0D15\u0D25\u0D35",													/*അകഥവ*/		g_lrMalayalam,		1,				nullptr },
		{ L"Sinhala",				L"\u0D90\u0DA0\u0DB0\u0DC0",													/*ඐචධව*/			g_lrSinhala,		1,				nullptr },
		{ L"Thai",					L"\u0E02\u0E12\u0E22\u0E32",													/*ขฒยา*/				g_lrThai,			1,				nullptr },
		{ L"Lao",					L"\u0E8D\u0E9D\u0EAD\u0EBD",													/*ຍຝອຽ*/			g_lrLao,			1,				nullptr },
		{ L"Tibetan",				L"\u0F00\u0F01\u0F02\u0F03",													/*ༀ༁༂༃*/				g_lrTibetan,		1,				nullptr },
		{ L"Myanmar",				L"\u1000\u1001\u1002\u1003",													/*ကခဂဃ*/			g_lrMyanmar,		1,				nullptr },
		{ L"Georgian",				L"\u10A0\u10B0\u10C0\u10D0",													/*ႠႰჀა*/			g_lrGeorgian,		1,				nullptr },
		{ L"Khmer",					L"\u1780\u1790\u17B0\u17C0",													/*កថឰៀ*/			g_lrKhmer,			1,				nullptr },
		{ L"Simplified Chinese",	L"\u4E2D\u6587\u8303\u4F8B",													/*中文范例*/			nullptr,			0,				CFonts::Check_SimplifiedChinese },
		{ L"Traditional Chinese",	L"\u4E2D\u6587\u7BC4\u4F8B",													/*中文範例*/			nullptr,			0,				CFonts::Check_TraditionalChinese },
		{ L"Japanese",				L"\u30B5\u30F3\u30D7\u30EB\u3067\u3059",										/*サンプルです*/		nullptr,			0,				CFonts::Check_Japanese },
		{ L"Korean",				L"\uAC00\uAC11\uAC1A\uAC2F",													/*가갑갚갯*/			nullptr,			0,				CFonts::Check_Korean },
		{ L"Vietnamese",			L"\u1ED7\u1ED9\u1ED1\u1ED3",													/*ỗộốồ*/			nullptr,			0,				CFonts::Check_Vietnamese },
		{ L"Symbol",				L"\x41\x61\x42\x62\x7A\x5A",													/*AaBbzZ*/			nullptr,			0,				CFonts::Check_Symbol },
		{ L"Other",					L"\x41\x61\x42\x62\x7A\x5A",													/*AaBbzZ*/			nullptr,			0,				nullptr },
		{ L"Ogham",					L"\u1681\u1682\u1683\u1684",													/*ᚁᚂᚃᚄ*/				g_lrOgham,			1,				nullptr },
		{ L"Runic",					L"\u16A0\u16A1\u16A2\u16A3",													/*ᚠᚡᚢᚣ*/			g_lrRunic,			1,				nullptr },
		{ L"N’ko",					L"\u07CA\u07CB\u07CC\u07CD",													/*ߊߋߌߍ*/				g_lrNko,			1,				nullptr },
	};

	static constexpr uint32_t g_ui32Count = static_cast<uint32_t>(LSW_ELEMENTS( g_wsList ));

	static_assert(
		static_cast<uint32_t>(CFonts::LSW_WS__COUNT) ==
		static_cast<uint32_t>(LSW_ELEMENTS( g_wsList )),
		"LSW_WS_ID does not match g_wsList size/order."
	);

	// == Functions.
	/**
	 * \brief FONTENUMPROCW-compatible callback used by FamilyIsOther() to scan faces within a family.
	 *
	 * The callback checks each enumerated face with FaceIsOther(). If any face is classified as "Other",
	 * the context flag is set and enumeration is terminated.
	 *
	 * \param _plfEnum Pointer to LOGFONTW for the enumerated face (ENUMLOGFONTEXW-compatible).
	 * \param _ptmMetrics Pointer to TEXTMETRICW for the enumerated face (may actually be NEWTEXTMETRICEXW for TrueType).
	 * \param _dwType Font type flags (bitwise OR of TRUETYPE_FONTTYPE/RASTER_FONTTYPE/DEVICE_FONTTYPE). Unused here.
	 * \param _lpParam LPARAM pointing to an lsw::CFonts::LSW_CTX instance that receives the result flag.
	 * \return Returns 0 to stop enumeration when a match is found; otherwise returns nonzero to continue.
	 */
	int CALLBACK CFonts::EnumFamilyIsOther_Proc(
		const LOGFONTW * _plfEnum,
		const TEXTMETRICW * /*_ptmMetrics*/,
		DWORD /*_dwType*/,
		LPARAM _lpParam ) {
		bool * pbCtx = reinterpret_cast<bool *>(_lpParam);
		if ( pbCtx == nullptr || _plfEnum == nullptr ) {
			return 1; // continue safely if inputs are unexpected
		}

		// FaceIsOther() must be declared elsewhere in this namespace:
		// bool FaceIsOther( const LOGFONTW & _lfFace );
		if ( FaceIsOther( (*_plfEnum) ) ) {
			(*pbCtx) = true;
			return 0; // stop early: we found at least one "Other" face in this family
		}
		return 1; // keep scanning other faces in the same family
	}

	/**
	 * FONTENUMPROCW-compatible callback used by EnumSizesForFace().
	 * 
	 * Captures whether a TrueType face exists and collects raster strike sizes.
	 * \param _plfEnum Pointer to LOGFONTW (the enumerated face).
	 * \param _ptmEnum Pointer to TEXTMETRICW (actually NEWTEXTMETRICW-compatible).
	 * \param _dwType Font type flags (TRUETYPE_FONTTYPE/RASTER_FONTTYPE).
	 * \param _lpParam LPARAM pointing to an LSW_ENUM_SIZES_CTX.
	 * \return Returns nonzero to continue enumeration; zero to stop.
	 */
	int CALLBACK CFonts::EnumSizesProc( const LOGFONTW * /*_plfEnum*/,
		const TEXTMETRICW * _ptmEnum,
		DWORD _dwType,
		LPARAM _lpParam ) {
		LSW_ENUM_SIZES_CTX * pCtx = reinterpret_cast<LSW_ENUM_SIZES_CTX *>(_lpParam);
		const NEWTEXTMETRICW * pNtm = reinterpret_cast<const NEWTEXTMETRICW *>(_ptmEnum);

		if ( (_dwType & TRUETYPE_FONTTYPE) != 0 ) {
			(*(pCtx->pbIsTrueType)) = true;
			return 0;	// stop early; scalable sizes will be used
		}

		if ( (_dwType & RASTER_FONTTYPE) != 0 ) {
			int32_t i32Px = pNtm->tmHeight - pNtm->tmInternalLeading;
			int32_t i32Pt = ::MulDiv( i32Px, 72, static_cast<int32_t>(pCtx->uDpi) );
			if ( i32Pt >= 4 && i32Pt <= 256 ) {
				pCtx->psPoints->insert( i32Pt );
			}
		}
		return 1;
	}

	/**
	 * \brief FONTENUMPROCW-compatible callback used by EnumAllFamilies().
	 *
	 * Collects per-face LOGFONT and metrics into a vector.
	 *
	 * \param _plfEnum Pointer to LOGFONTW for the enumerated face (actually ENUMLOGFONTEXW-compatible).
	 * \param _ptmEnum Pointer to TEXTMETRICW. For TrueType faces this actually points to NEWTEXTMETRICEXW.
	 * \param _dwType  Font type flags (bitwise OR of TRUETYPE_FONTTYPE, RASTER_FONTTYPE, DEVICE_FONTTYPE).
	 * \param _lpParam LPARAM pointing to CSystemFonts.
	 * \return Returns nonzero to continue enumeration; zero to stop.
	 */
	int CALLBACK CFonts::EnumFamilies_Proc( const LOGFONTW * _plfEnum,
		const TEXTMETRICW * _ptmEnum,
		DWORD _dwType,
		LPARAM _lpParam ) {
		LSW_FAMILY_ENUM_CTX * pvOut = reinterpret_cast<LSW_FAMILY_ENUM_CTX *>(_lpParam);

		if ( pvOut == nullptr || pvOut->psfFonts == nullptr || _plfEnum == nullptr || _ptmEnum == nullptr ) { return 1; }

		try {
			LSW_SYSTEM_FONTS sSfTmp{};
			sSfTmp.lfLogFont = (*_plfEnum);

			// Safely populate NEWTEXTMETRICW regardless of source type.
			if ( (_dwType & TRUETYPE_FONTTYPE) != 0 ) {
				// For TrueType, _ptmEnum is actually NEWTEXTMETRICEXW*.
				const NEWTEXTMETRICEXW * pntmEx = reinterpret_cast<const NEWTEXTMETRICEXW *>(_ptmEnum);
				sSfTmp.ntmMetrics = pntmEx->ntmTm;
			}
			else {
				// For raster/device fonts, _ptmEnum is TEXTMETRICW*.
				const TEXTMETRICW * ptmW = _ptmEnum;
				std::memset( &sSfTmp.ntmMetrics, 0, sizeof( sSfTmp.ntmMetrics ) );
				std::memcpy( &sSfTmp.ntmMetrics, ptmW, sizeof( TEXTMETRICW ) );
			}

			if ( !IsVerticalAtFace( sSfTmp.lfLogFont.lfFaceName ) ) {
				if ( pvOut->wiWritingId == LSW_WS_ANY ||
					((pvOut->wiWritingId == LSW_WS_OTHER && FaceIsOther( (*_plfEnum) )) ||
					(pvOut->wiWritingId != LSW_WS_OTHER && FaceSupportsWritingSystem( (*_plfEnum), WritingSystem( pvOut->wiWritingId ) ))) ) {
					pvOut->psfFonts->insert( sSfTmp );
				}
			}
		}
		catch ( ... ) { return 0; }

		return 1;
	}

	/**
	 * \brief FONTENUMPROCW-compatible callback used by EnumStylesForFamily().
	 *
	 * Extracts the user-visible style name (from ENUMLOGFONTEXW::elfStyle when available,
	 * otherwise derives a reasonable label), plus the numeric weight and italic flag,
	 * then inserts a record into the provided CFontStyles set.
	 *
	 * \param _plfEnum Pointer to LOGFONTW (actually ENUMLOGFONTEXW).
	 * \param _ptmEnum Pointer to TEXTMETRICW (for TrueType, actually NEWTEXTMETRICEXW).
	 * \param _dwType  Font type flags (bitwise OR of TRUETYPE_FONTTYPE/RASTER_FONTTYPE/DEVICE_FONTTYPE).
	 * \param _lpParam LPARAM pointing to an CFontStyles.
	 * \return Returns nonzero to continue enumeration; zero to stop.
	 */
	int CALLBACK CFonts::EnumStyles_Proc( const LOGFONTW * _plfEnum,
		const TEXTMETRICW * _ptmEnum,
		DWORD _dwType,
		LPARAM _lpParam ) {
		CFontStyles * psCtx = reinterpret_cast<CFontStyles *>(_lpParam);
		if ( psCtx == nullptr || _plfEnum == nullptr || _ptmEnum == nullptr ) { return 1; }

		const ENUMLOGFONTEXW * pelfElf = reinterpret_cast<const ENUMLOGFONTEXW *>(_plfEnum);

		// Numeric weight from the correct source.
		LONG lWeight = 0;
		if ( (_dwType & TRUETYPE_FONTTYPE) != 0 ) {
			const NEWTEXTMETRICEXW * pNtmEx = reinterpret_cast<const NEWTEXTMETRICEXW *>(_ptmEnum);
			lWeight = static_cast<LONG>(pNtmEx->ntmTm.tmWeight);
		}
		else {
			lWeight = static_cast<LONG>(_ptmEnum->tmWeight);
		}

		// Style name: prefer elfStyle; if empty, derive from metrics.
		std::wstring wsStyle = pelfElf->elfStyle;
		if ( wsStyle.empty() ) {
			bool bItalic = (_plfEnum->lfItalic != 0);
			
			if ( (lWeight == 0 || lWeight == FW_NORMAL) && bItalic ) {
				wsStyle = L"Italic";
			}
			else {
				wsStyle = WeightToString( lWeight );
				if ( bItalic ) {
					wsStyle += L' ';
					wsStyle += L"Italic";
				}
			}
		}
		

		BYTE bItalic = _plfEnum->lfItalic;

		LSW_FONT_STYLE fsStyle {};
		fsStyle.wsStyleName = wsStyle;
		fsStyle.lWeight = lWeight;
		fsStyle.bItalic = bItalic;

		try {
			psCtx->insert( fsStyle );
		}
		catch ( ... ) { return 0; }

		return 1;		// continue enumeration
	}

	/**
	 * \brief FONTENUMPROCW-compatible callback used by EnumSizesForFace().
	 *
	 * Captures whether a TrueType face exists and collects raster strike sizes.
	 *
	 * \param _plfEnum Pointer to LOGFONTW for the enumerated face.
	 * \param _ptmEnum Pointer to TEXTMETRICW (for TrueType, actually NEWTEXTMETRICEXW-compatible).
	 * \param _dwType  Font type flags (bitwise OR of TRUETYPE_FONTTYPE/RASTER_FONTTYPE/DEVICE_FONTTYPE).
	 * \param _lpParam LPARAM pointing to an LSW_ENUM_SIZES_CTX.
	 * \return Returns nonzero to continue enumeration; zero to stop.
	 */
	int CALLBACK CFonts::EnumSizes_Proc( const LOGFONTW * /*_plfEnum*/,
		const TEXTMETRICW * _ptmEnum,
		DWORD _dwType,
		LPARAM _lpParam ) {
		LSW_ENUM_SIZES_CTX * psCtx = reinterpret_cast<LSW_ENUM_SIZES_CTX *>(_lpParam);
		if ( psCtx == nullptr || _ptmEnum == nullptr ) {
			return 1;
		}

		// If any TrueType face exists for this style, mark scalable and stop early.
		if ( (_dwType & TRUETYPE_FONTTYPE) != 0 ) {
			(*(psCtx->pbIsTrueType)) = true;
			return 0;
		}

		// For raster/device faces, collect discrete strike sizes.
		if ( (_dwType & RASTER_FONTTYPE) != 0 || (_dwType & DEVICE_FONTTYPE) != 0 ) {
			int32_t i32Px = static_cast<int32_t>(_ptmEnum->tmHeight - _ptmEnum->tmInternalLeading);
			int32_t i32Pt = ::MulDiv( i32Px, 72, static_cast<int32_t>(psCtx->uDpi) );
			if ( i32Pt >= 4 && i32Pt <= 256 ) {
				psCtx->psPoints->insert( static_cast<uint32_t>(i32Pt) );
			}
		}
		return 1;
	}

	/**
	 * \brief Enumerate all faces (Family + Style) on the system for a given writing style.
	 * 
	 * \param _uDpi		DPI used to convert pixel heights to points for raster faces.
	 * \param _wiId		The writing style for which to gather fonts.
	 * \return Returns an array of system fonts.
	 */
	CFonts::CSystemFonts CFonts::EnumAllFaces( uint32_t _uDpi, LSW_WS_ID _wiId ) {
		LOGFONTW lfQuery {};
		lfQuery.lfCharSet = DEFAULT_CHARSET;
		lfQuery.lfFaceName[0] = L'\0';

		CSystemFonts sfCtx;
		LSW_FAMILY_ENUM_CTX fecCtx = { &sfCtx, _uDpi, _wiId };

		HDC hDc = ::GetDC( NULL );
		if ( hDc ) {
			::EnumFontFamiliesExW( hDc, &lfQuery, EnumFamilies_Proc, reinterpret_cast<LPARAM>(&fecCtx), 0 );
			::ReleaseDC( NULL, hDc );
		}

		for ( auto & I : sfCtx ) {
			EnumStylesForFamily( I.lfLogFont.lfFaceName, const_cast<CFontStyles &>(I.fsStyles) );
			for ( auto & J : I.fsStyles ) {
				EnumSizesForFace( I.lfLogFont.lfFaceName, const_cast<LSW_FONT_STYLE &>(J), _uDpi );
			}
		}

		return sfCtx;
	}

	/**
	 * \brief Enumerates full style names for a given family using ENUMLOGFONTEXW.
	 * 
	 * \param _wsFamily The family whose styles to enumerate.
	 * \param _vOutStyles Receives unique style names as reported by GDI (e.g., L"Regular", L"Light", L"Light Italic", L"Black").
	 */
	VOID CFonts::EnumStylesForFamily( const std::wstring &_wsFamily, std::vector<std::wstring> &_vOutStyles ) {
		_vOutStyles.clear();

		LOGFONTW lfQuery {};
		::wcsncpy_s( lfQuery.lfFaceName, _wsFamily.c_str(), _TRUNCATE );
		lfQuery.lfCharSet = DEFAULT_CHARSET;

		std::unordered_set<std::wstring, LSW_CI_HASH, LSW_CI_EQ> usNames;
		LSW_ENUM_STYLES_CTX sCtx { &usNames };

		HDC hDc = ::GetDC( NULL );
		::EnumFontFamiliesExW( hDc, &lfQuery, EnumStylesProc, reinterpret_cast<LPARAM>(&sCtx), 0 );
		::ReleaseDC( NULL, hDc );


		if ( usNames.empty() ) {
			usNames.insert( L"Regular" );
		}

		// Stable order: put “Regular” first, then others alphabetically.
		if ( usNames.find( L"Regular" ) != usNames.end() ) {
			_vOutStyles.push_back( L"Regular" );
		}
		std::vector<std::wstring> vTmp;
		for ( const std::wstring & wsThis : usNames ) {
			if ( ::_wcsicmp( wsThis.c_str(), L"Regular" ) != 0 ) {
				vTmp.push_back( wsThis );
			}
		}
		std::sort( vTmp.begin(), vTmp.end(), []( const std::wstring & _wsA, const std::wstring & _wsB ) {
			return ::_wcsicmp( _wsA.c_str(), _wsB.c_str() ) < 0;
		} );
		for ( const std::wstring & wsThis : vTmp ) {
			_vOutStyles.push_back( wsThis );
		}
	}

	/**
	 * \brief Enumerates full style names for a given family using ENUMLOGFONTEXW.
	 * 
	 * \param _wcFaceName The family whose styles to enumerate.
	 * \param _sOutStyles Receives the styles for the given font family.
	 */
	VOID CFonts::EnumStylesForFamily( const WCHAR _wcFaceName[LF_FACESIZE], CFontStyles &_sOutStyles ) {
		_sOutStyles.clear();

		LOGFONTW lfQuery{};
		lfQuery.lfCharSet = DEFAULT_CHARSET;
		::wcsncpy_s( lfQuery.lfFaceName, _wcFaceName, _TRUNCATE );

		HDC hDc = ::GetDC( NULL );
		if ( hDc ) {
			::EnumFontFamiliesExW( hDc, &lfQuery, EnumStyles_Proc, reinterpret_cast<LPARAM>(&_sOutStyles), 0 );
			::ReleaseDC( NULL, hDc );
		}

		if ( _sOutStyles.empty() ) {
			LSW_FONT_STYLE fsDefault{};
			fsDefault.wsStyleName = L"Regular";
			fsDefault.lWeight = FW_NORMAL;
			fsDefault.bItalic = 0;
			_sOutStyles.insert( fsDefault );
		}

	}

	/**
	 * \brief Enumerate available point sizes for a given family + style (face) and add them to the style record.
	 *
	 * If any TrueType/OpenType face exists for the requested style, a standard scalable set is used.
	 * Otherwise, the function gathers discrete raster strike sizes from the system and stores them.
	 *
	 * \param _wcFaceName The family name to query (LF_FACESIZE-terminated string).
	 * \param _fsStyle    In/out style record. Uses _fsStyle.lWeight/_fsStyle.bItalic to select the face;
	 *                    populates _fsStyle.sSizes with available point sizes.
	 * \param _uDpi       DPI used to convert pixel heights to points for raster faces.
	 */
	VOID CFonts::EnumSizesForFace( const WCHAR _wcFaceName[LF_FACESIZE], LSW_FONT_STYLE & _fsStyle, uint32_t _uDpi ) {
		// Reset output.
		_fsStyle.sSizes.clear();

		// Build the LOGFONTW query for this specific style (weight/italic).
		LOGFONTW lfQuery{};
		::wcsncpy_s( lfQuery.lfFaceName, _wcFaceName, _TRUNCATE );
		lfQuery.lfCharSet = DEFAULT_CHARSET;
		lfQuery.lfWeight  = (_fsStyle.lWeight > 0) ? _fsStyle.lWeight : FW_DONTCARE;
		lfQuery.lfItalic  = _fsStyle.bItalic;

		bool bTrueType = false;
		LSW_ENUM_SIZES_CTX sCtx{ &bTrueType, &_fsStyle.sSizes, _uDpi };

		HDC hDc = ::GetDC( NULL );
		if ( hDc ) {
			::EnumFontFamiliesExW( hDc, &lfQuery, EnumSizes_Proc, reinterpret_cast<LPARAM>( &sCtx ), 0 );
			::ReleaseDC( NULL, hDc );
		}

		// If scalable, populate a standard size list.
		if ( bTrueType ) {
			static const uint32_t kuPts[] = { 6,7,8,9,10,11,12,14,16,18,20,22,24,26,28,36,48,72 };
			for ( size_t I = 0; I < sizeof( kuPts ) / sizeof( kuPts[0] ); ++I ) {
				_fsStyle.sSizes.insert( kuPts[I] );
			}
		}
		else {
			// For raster-only faces, ensure we have at least one sane default (e.g., 10 pt) if none were found.
			if ( _fsStyle.sSizes.empty() ) {
				_fsStyle.sSizes.insert( 10 );
			}
		}
	}

	/**
	 * Gets a writing system by enumeration.
	 * 
	 * \param _wiId The ID of the writing system to get.
	 * \return Returns a constant reference to the requested writing system.
	 **/
	const CFonts::LSW_WRITING_SYSTEM & CFonts::WritingSystem( LSW_WS_ID _wiId ) {
		return g_wsList[_wiId];
	}

	/**
	 * Gets the total number of writing systems.
	 * 
	 * \return Returns the total number of writing systems.
	 **/
	size_t CFonts::WritingSystems() {
		return LSW_ELEMENTS( g_wsList );
	}

	/**
	 * \brief Determines if a specific LOGFONT face supports a writing system.
	 * 
	 *        Internally creates the HFONT, selects it into a memory DC, and calls GetFontUnicodeRanges().
	 * \param _lfFace LOGFONTW describing the face to check (lfHeight is ignored and may be 0).
	 * \param _wsSystem Writing system definition to test.
	 * \return Returns true if the face covers all required Unicode blocks for _wsSystem (or always true for "Any").
	 */
	bool CFonts::FaceSupportsWritingSystem( const LOGFONTW &_lfFace, const LSW_WRITING_SYSTEM &_wsSystem ) {
		// Create font at default height for coverage query.
		LSW_FONT fFace;
		LOGFONTW lfTmp = _lfFace;
		lfTmp.lfHeight = 0;

		if ( !fFace.CreateFontIndirectW( &lfTmp ) ) {
			return false;
		}

		HDC hDcLocal = ::CreateCompatibleDC( NULL );
		if ( !hDcLocal ) {
			return false;
		}

		bool bResult = false;
		{
			LSW_SELECTOBJECT oSel( hDcLocal, fFace.hFont );
			DWORD dwBytes = ::GetFontUnicodeRanges( hDcLocal, NULL );
			if ( dwBytes == 0 ) {
				::DeleteDC( hDcLocal );
				return false;
			}

			std::vector<BYTE> vBuffer( dwBytes );
			GLYPHSET * pgs = reinterpret_cast<GLYPHSET *>(vBuffer.data());
			if ( ::GetFontUnicodeRanges( hDcLocal, pgs ) == 0 ) {
				::DeleteDC( hDcLocal );
				return false;
			}

			const bool bHasRanges   = (_wsSystem.ui32CountRanges != 0 && _wsSystem.prRanges != nullptr);
			const bool bHasSpecial  = (_wsSystem.pfSpecial != nullptr);
			const bool bNoCriteria  = (!bHasRanges && !bHasSpecial);

			bool bRangesOk = true;
			if ( bHasRanges ) {
				for ( uint32_t I = 0; I < _wsSystem.ui32CountRanges; ++I ) {
					const LSW_UC_RANGE & rR = _wsSystem.prRanges[I];
					if ( !GlyphSetHasRange( pgs, rR.ui32Start, rR.ui32End ) ) {
						bRangesOk = false;
						break;
					}
				}
			}

			bool bSpecialOk = true;
			if ( bHasSpecial ) {
				// Signature assumed: bool (*pfSpecial)(HDC, const GLYPHSET *)
				bSpecialOk = _wsSystem.pfSpecial( hDcLocal, pgs );
			}

			bResult = bNoCriteria || ( bRangesOk && bSpecialOk );
		}

		::DeleteDC( hDcLocal );
		return bResult;
	}

	/**
	 * \brief Returns true if a face matches none of the known writing systems except "Other".
	 * 
	 * \param _lfFace Face to test.
	 * \return Returns true if the face is not classified by any non-Other system.
	 */
	bool CFonts::FaceIsOther( const LOGFONTW &_lfFace ) {
		for ( uint32_t I = 0; I < g_ui32Count; ++I ) {
			if ( _wcsicmp( g_wsList[I].pwszName, L"Other" ) == 0 ) {
				continue;
			}
			if ( FaceSupportsWritingSystem( _lfFace, g_wsList[I] ) ) {
				return false;
			}
		}
		return true;
	}

	/**
	 * \brief Convert a GDI tmWeight value into a human-readable name.
	 *
	 * Exact matches return the canonical names corresponding to:
	 * FW_DONTCARE, FW_THIN, FW_EXTRALIGHT, FW_LIGHT, FW_NORMAL, FW_MEDIUM,
	 * FW_SEMIBOLD, FW_BOLD, FW_EXTRABOLD, FW_HEAVY.
	 *
	 * Non-standard values return the name of the *nearest* canonical bucket.
	 * Ties prefer the heavier bucket (e.g., 350 → "Light", 550 → "Semibold").
	 *
	 * \param _i32TmWeight The tmWeight value (usually 0..1000).
	 * \return Returns a display string such as L"Normal", L"Semibold", etc.
	 */
	std::wstring CFonts::WeightToString( int32_t _i32TmWeight ) {
		struct LSW_WEIGHT_NAME {
			int32_t         i32Weight;
			const wchar_t * pwszName;
		};
		static const LSW_WEIGHT_NAME g_wLookup[] = {
			{ FW_DONTCARE,   L"Regular"		},
			{ FW_THIN,       L"Thin"		},
			{ FW_EXTRALIGHT, L"Extra Light"	},
			{ FW_LIGHT,      L"Light"		},
			{ FW_NORMAL,     L"Regular"		},
			{ FW_MEDIUM,     L"Medium"		},
			{ FW_SEMIBOLD,   L"Semibold"	},
			{ FW_BOLD,       L"Bold"		},
			{ FW_EXTRABOLD,  L"Extra Bold"	},
			{ FW_HEAVY,      L"Heavy"		},
		};

		// Fast path: exact match.
		for ( size_t I = 0; I < sizeof( g_wLookup ) / sizeof( g_wLookup[0] ); ++I ) {
			if ( _i32TmWeight == g_wLookup[I].i32Weight ) {
				return g_wLookup[I].pwszName;
			}
		}

		// Otherwise snap to nearest canonical bucket (ties -> heavier).
		int32_t i32BestIdx = 0;
		int32_t i32BestDelta = std::numeric_limits<int32_t>::max();

		for ( size_t I = 0; I < sizeof( g_wLookup ) / sizeof( g_wLookup[0] ); ++I ) {
			int32_t i32Delta = std::abs( _i32TmWeight - g_wLookup[I].i32Weight );
			if ( i32Delta < i32BestDelta ) {
				i32BestDelta = i32Delta;
				i32BestIdx = static_cast<int32_t>( I );
			}
			else if ( i32Delta == i32BestDelta ) {
				// Prefer heavier bucket on ties.
				if ( g_wLookup[I].i32Weight > g_wLookup[i32BestIdx].i32Weight ) {
					i32BestIdx = static_cast<int32_t>( I );
				}
			}
		}
		return g_wLookup[i32BestIdx].pwszName;
	}

	/** Japanese: require Kana (Hiragana or Katakana); Han alone is not sufficient. */
	bool CFonts::Check_Japanese( HDC /*_hDc*/, const GLYPHSET * _pgsSet ) {
		return GsHasAny( _pgsSet, g_lrHiragana, 1 ) || GsHasAny( _pgsSet, g_lrKatakana, 1 );
	}

	/** Korean: Hangul Jamo or Hangul Syllables. */
	bool CFonts::Check_Korean( HDC /*_hDc*/, const GLYPHSET * _pgsSet ) {
		return GsHasAny( _pgsSet, g_lrHangulSyll, 1 ) || GsHasAny( _pgsSet, g_lrHangulJamo, 1 );
	}

	/** Traditional Chinese: Han + Bopomofo. */
	bool CFonts::Check_TraditionalChinese( HDC /*_hDc*/, const GLYPHSET * _pgsSet ) {
		bool bHan = GsHasAny( _pgsSet, g_lrCjkUnified, 1 ) || GsHasAny( _pgsSet, g_lrCjkExtA, 1 );
		bool bBopo = GsHasAny( _pgsSet, g_lrBopomofo, 1 ) || GsHasAny( _pgsSet, g_lrBopomofoExt, 1 );
		return bHan && bBopo;
	}

	/** Simplified Chinese: Han but not Japanese/Korean/Traditional heuristics. */
	bool CFonts::Check_SimplifiedChinese( HDC /*_hDc*/, const GLYPHSET * _pgsSet ) {
		bool bHan = GsHasAny( _pgsSet, g_lrCjkUnified, 1 ) || GsHasAny( _pgsSet, g_lrCjkExtA, 1 );
		if ( !bHan ) { return false; }
		if ( Check_TraditionalChinese( NULL, _pgsSet ) ) { return false; }	// Has Bopomofo → treat as Traditional.
		if ( Check_Japanese( NULL, _pgsSet ) ) { return false; }			// Has Kana → probably Japanese.
		if ( Check_Korean( NULL, _pgsSet ) ) { return false; }				// Has Hangul → Korean.
		return true;
	}

	/** Vietnamese: Latin core + Vietnamese diacritics (1EA0–1EFF) or combining marks. */
	bool CFonts::Check_Vietnamese( HDC /*_hDc*/, const GLYPHSET * _pgsSet ) {
		bool bLatin = GsHasAll( _pgsSet, g_lrLatinCore, 2 );
		bool bVn = GsHasAny( _pgsSet, g_lrLatExtAdd, 1 ) || GsHasAny( _pgsSet, g_lrCombining, 1 );
		return bLatin && bVn;
	}

	/** Symbol: Symbol charset or significant symbol ranges while lacking alphabetic scripts. */
	bool CFonts::Check_Symbol( HDC _hDc, const GLYPHSET * _pgsSet ) {
		TEXTMETRICW tmMetrics {};
		::GetTextMetricsW( _hDc, &tmMetrics );
		if ( tmMetrics.tmCharSet == SYMBOL_CHARSET ) { return true; }
		bool bSym =
			GsHasAny( _pgsSet, g_lrDingbats, 1 ) ||
			GsHasAny( _pgsSet, g_lrMiscSymbols, 1 ) ||
			GsHasAny( _pgsSet, g_lrGeomShapes, 1 ) ||
			GsHasAny( _pgsSet, g_lrArrows, 1 ) ||
			GsHasAny( _pgsSet, g_lrPrivateUse, 1 );
		// If it also has common alphabetic scripts, don't call it "Symbol".
		bool bAlphabetic =
			GsHasAny( _pgsSet, g_lrLatinCore, 2 ) ||
			GsHasAny( _pgsSet, g_lrGreek, 1 ) ||
			GsHasAny( _pgsSet, g_lrCyrillic, 1 );
		return bSym && !bAlphabetic;
	}

}	// namespace lsw

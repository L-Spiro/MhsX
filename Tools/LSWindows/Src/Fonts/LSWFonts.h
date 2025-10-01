#pragma once

#include "../LSWWin.h"

#include <cstdint>
#include <set>
#include <string>
#include <unordered_set>


namespace lsw {

	class CFonts {
		/**
		 * \brief Case-insensitive hash functor for std::wstring.
		 *
		 * Converts the input string to lowercase (using ::towlower on each wchar_t)
		 * and hashes the result with std::hash<std::wstring>. Intended for use with
		 * unordered containers that require case-insensitive keys.
		 */
		struct LSW_CI_HASH {
			/**
			 * \brief Compute a case-insensitive hash for a wide string.
			 * 
			 * \param _wsVal Reference to the input wide string to hash.
			 * \return Returns the hash value of the lowercase-transformed input.
			 */
			size_t							operator() ( const std::wstring &_wsVal ) const {
				std::wstring wsTmp = _wsVal;
				for ( wchar_t & wCh : wsTmp ) { wCh = ::towlower( wCh ); }
				return std::hash<std::wstring>{}( wsTmp );
			}
		};

		/**
		 * Case-insensitive equality functor for std::wstring.
		 *
		 * Compares two wide strings for equality ignoring case by lowercasing each
		 * character with ::towlower and performing a position-by-position comparison.
		 */
		struct LSW_CI_EQ {
			/**
			 * \brief Compare two wide strings for case-insensitive equality.
			 * 
			 * \param _a Reference to the first wide string.
			 * \param _b Reference to the second wide string.
			 * \return Returns true if the strings have the same length and the same
			 *         characters when compared case-insensitively; otherwise false.
			 */
			bool							operator() ( const std::wstring & _a, const std::wstring & _b ) const {
				if ( _a.size() != _b.size() ) { return false; }
				for ( size_t I = 0; I < _a.size(); ++I ) {
					if ( ::towlower( _a[I] ) != ::towlower( _b[I] ) ) { return false; }
				}
				return true;
			}
		};

		/** Context passed via lParam for the styles callback. */
		struct LSW_ENUM_STYLES_CTX {
			std::unordered_set<std::wstring, LSW_CI_HASH, LSW_CI_EQ> *
											pusNames;
		};

		/** \brief Context used by the sizes enumeration callback. */
		struct LSW_ENUM_SIZES_CTX {
			bool *							pbIsTrueType;					/** Set to TRUE if a TrueType/OpenType face is encountered. */
			std::set<uint32_t> *			psPoints;						/** Destination set for discrete raster point sizes. */
			uint32_t						uDpi;							/** DPI for pixel→point conversion. */
		};

	public :
		// == Enumerations.
		enum LSW_WS_ID : uint32_t {
			LSW_WS_ANY = 0,						// Any
			LSW_WS_LATIN,						// Latin
			LSW_WS_GREEK,						// Greek
			LSW_WS_CYRILLIC,					// Cyrillic
			LSW_WS_ARMENIAN,					// Armenian
			LSW_WS_HEBREW,						// Hebrew
			LSW_WS_ARABIC,						// Arabic
			LSW_WS_SYRIAC,						// Syriac
			LSW_WS_THAANA,						// Thaana
			LSW_WS_DEVANAGARI,					// Devanagari
			LSW_WS_BENGALI,						// Bengali
			LSW_WS_GURMUKHI,					// Gurmukhi
			LSW_WS_GUJARATI,					// Gujarati
			LSW_WS_ORIYA,						// Oriya
			LSW_WS_TAMIL,						// Tamil
			LSW_WS_TELUGU,						// Telugu
			LSW_WS_KANNADA,						// Kannada
			LSW_WS_MALAYALAM,					// Malayalam
			LSW_WS_SINHALA,						// Sinhala
			LSW_WS_THAI,						// Thai
			LSW_WS_LAO,							// Lao
			LSW_WS_TIBETAN,						// Tibetan
			LSW_WS_MYANMAR,						// Myanmar
			LSW_WS_GEORGIAN,					// Georgian
			LSW_WS_KHMER,						// Khmer
			LSW_WS_SIMPLIFIED_CHINESE,			// Simplified Chinese
			LSW_WS_TRADITIONAL_CHINESE,			// Traditional Chinese
			LSW_WS_JAPANESE,					// Japanese
			LSW_WS_KOREAN,						// Korean
			LSW_WS_VIETNAMESE,					// Vietnamese
			LSW_WS_SYMBOL,						// Symbol
			LSW_WS_OTHER,						// Other
			LSW_WS_OGHAM,						// Ogham
			LSW_WS_RUNIC,						// Runic
			LSW_WS_NKO,							// N’ko

			LSW_WS__COUNT						// Count/sentinel
		};


		// == Types.
		/**
		 * \brief A Unicode block range.
		 */
		struct LSW_UC_RANGE {
			uint32_t						ui32Start;						/**< Inclusive start code point. */
			uint32_t						ui32End;						/**< Inclusive end code point. */
		};

		/** Optional special checker for a writing system. */
		typedef bool (*						PfWsCheck)( HDC _hDc, const GLYPHSET * _pgsGlyphs );

		/** Writing system described by Unicode ranges and/or a special checker. */
		struct LSW_WRITING_SYSTEM {
			const wchar_t *					pwszName;						/**< Display name (e.g., L"Greek"). */
			const wchar_t *					pwszSample;						/**< Sample text. */
			const LSW_UC_RANGE *			prRanges;						/**< Pointer to an array of Unicode ranges (may be nullptr). */
			uint32_t						ui32CountRanges;				/**< Number of ranges in prRanges. 0 means "Any" unless pfSpecial is used. */
			PfWsCheck						pfSpecial;						/**< Optional special predicate. If non-null, it must return true for a face to match. */
		};

		/** Font style. */
		struct LSW_FONT_STYLE {
			std::wstring					wsStyleName;					/**< The name of the style. */
			std::set<uint32_t>				sSizes;							/**, The possible sizes for this font style. */
			LONG							lWeight = 0;					/**< The actual weight. */
			BYTE							bItalic = 0;					/**< The italic setting. */
		};

		/**
		 * \brief Case-insensitive strict-weak ordering for LSW_FONT_STYLE.
		 *
		 * Primary key:  case-insensitive style name.
		 * Secondary:    weight (lighter → heavier).
		 * Tertiary:     italic flag (non-italic → italic).
		 * Final tie:    case-sensitive style name to stabilize total order.
		 */
		struct LSW_FONT_STYLE_LESS {
			/**
			 * \brief Compare two LSW_FONT_STYLE values.
			 * \param _fsA Left operand.
			 * \param _fsB Right operand.
			 * \return Returns true if _fsA should be ordered before _fsB.
			 */
			bool							operator()	( const LSW_FONT_STYLE & _fsA, const LSW_FONT_STYLE & _fsB ) const {
				// Case-insensitive compare of style names.
				int iCmp = ::_wcsicmp( _fsA.wsStyleName.c_str(), _fsB.wsStyleName.c_str() );
				if ( iCmp != 0 ) { return iCmp < 0; }

				// Weight (ascending).
				if ( _fsA.lWeight != _fsB.lWeight ) { return _fsA.lWeight < _fsB.lWeight; }

				// Italic (false before true).
				if ( _fsA.bItalic != _fsB.bItalic ) { return _fsA.bItalic < _fsB.bItalic; }

				// Stabilize ordering if everything else matched (rare).
				return _fsA.wsStyleName < _fsB.wsStyleName;
			}
		};

		/** List of styles. */
		typedef std::set<LSW_FONT_STYLE, LSW_FONT_STYLE_LESS>
											CFontStyles;

		/** Fonts on a system. */
		struct LSW_SYSTEM_FONTS {
			LOGFONTW						lfLogFont;						/**< Contains information about the logical attributes of the font. */
			NEWTEXTMETRICW					ntmMetrics;						/**< Contains information about the physical attributes of the font. */
			CFontStyles						fsStyles;						/**< Contains all the valid styles for a font. */
		};

		/**
		 * \brief Case-insensitive lexicographic ordering for LSW_SYSTEM_FONTS.
		 *
		 * Order by family (lfFaceName, case-insensitive), then key LOGFONT members, then key metrics.
		 * This produces a stable, duplicate-free set across TrueType and raster faces.
		 */
		struct LSW_SYSTEM_FONTS_LESS {
			/**
			 * \param _sfA Left operand.
			 * \param _sfB Right operand.
			 * \return Returns true if _sfA is ordered before _sfB.
			 */
			bool							operator() ( const LSW_SYSTEM_FONTS & _sfA, const LSW_SYSTEM_FONTS & _sfB ) const {
				// 1) Family name (case-insensitive).
				int iCmp = ::_wcsicmp( _sfA.lfLogFont.lfFaceName, _sfB.lfLogFont.lfFaceName );
				return iCmp != 0 ? iCmp < 0 : false;
				//if ( iCmp != 0 ) { return iCmp < 0; }

				// 2) LOGFONT style-defining fields.
				//if ( _sfA.lfLogFont.lfWeight       != _sfB.lfLogFont.lfWeight       ) { return _sfA.lfLogFont.lfWeight       < _sfB.lfLogFont.lfWeight; }
				//if ( _sfA.lfLogFont.lfItalic       != _sfB.lfLogFont.lfItalic       ) { return _sfA.lfLogFont.lfItalic       < _sfB.lfLogFont.lfItalic; }
				//if ( _sfA.lfLogFont.lfUnderline    != _sfB.lfLogFont.lfUnderline    ) { return _sfA.lfLogFont.lfUnderline    < _sfB.lfLogFont.lfUnderline; }
				//if ( _sfA.lfLogFont.lfStrikeOut    != _sfB.lfLogFont.lfStrikeOut    ) { return _sfA.lfLogFont.lfStrikeOut    < _sfB.lfLogFont.lfStrikeOut; }

				//// 3) Charset / pitch / quality (affect face identity).
				//if ( _sfA.lfLogFont.lfCharSet      != _sfB.lfLogFont.lfCharSet      ) { return _sfA.lfLogFont.lfCharSet      < _sfB.lfLogFont.lfCharSet; }
				//if ( _sfA.lfLogFont.lfOutPrecision != _sfB.lfLogFont.lfOutPrecision ) { return _sfA.lfLogFont.lfOutPrecision < _sfB.lfLogFont.lfOutPrecision; }
				//if ( _sfA.lfLogFont.lfClipPrecision!= _sfB.lfLogFont.lfClipPrecision) { return _sfA.lfLogFont.lfClipPrecision< _sfB.lfLogFont.lfClipPrecision; }
				//if ( _sfA.lfLogFont.lfQuality      != _sfB.lfLogFont.lfQuality      ) { return _sfA.lfLogFont.lfQuality      < _sfB.lfLogFont.lfQuality; }
				//if ( _sfA.lfLogFont.lfPitchAndFamily!=_sfB.lfLogFont.lfPitchAndFamily) { return _sfA.lfLogFont.lfPitchAndFamily < _sfB.lfLogFont.lfPitchAndFamily; }

				//// 4) As tie-breakers, width/height/esc/orient (do not define style, but stabilize order).
				//if ( _sfA.lfLogFont.lfHeight       != _sfB.lfLogFont.lfHeight       ) { return _sfA.lfLogFont.lfHeight       < _sfB.lfLogFont.lfHeight; }
				//if ( _sfA.lfLogFont.lfWidth        != _sfB.lfLogFont.lfWidth        ) { return _sfA.lfLogFont.lfWidth        < _sfB.lfLogFont.lfWidth; }
				//if ( _sfA.lfLogFont.lfEscapement   != _sfB.lfLogFont.lfEscapement   ) { return _sfA.lfLogFont.lfEscapement   < _sfB.lfLogFont.lfEscapement; }
				//if ( _sfA.lfLogFont.lfOrientation  != _sfB.lfLogFont.lfOrientation  ) { return _sfA.lfLogFont.lfOrientation  < _sfB.lfLogFont.lfOrientation; }

				//// 5) Key text metrics (weight/height/style flags) for further disambiguation.
				//if ( _sfA.ntmMetrics.tmWeight      != _sfB.ntmMetrics.tmWeight      ) { return _sfA.ntmMetrics.tmWeight      < _sfB.ntmMetrics.tmWeight; }
				//if ( _sfA.ntmMetrics.tmHeight      != _sfB.ntmMetrics.tmHeight      ) { return _sfA.ntmMetrics.tmHeight      < _sfB.ntmMetrics.tmHeight; }
				//if ( _sfA.ntmMetrics.tmItalic      != _sfB.ntmMetrics.tmItalic      ) { return _sfA.ntmMetrics.tmItalic      < _sfB.ntmMetrics.tmItalic; }
				//if ( _sfA.ntmMetrics.tmUnderlined  != _sfB.ntmMetrics.tmUnderlined  ) { return _sfA.ntmMetrics.tmUnderlined  < _sfB.ntmMetrics.tmUnderlined; }
				//if ( _sfA.ntmMetrics.tmStruckOut   != _sfB.ntmMetrics.tmStruckOut   ) { return _sfA.ntmMetrics.tmStruckOut   < _sfB.ntmMetrics.tmStruckOut; }

				//// 6) Last resort: compare remaining bytes to stabilize total order.
				//return std::memcmp( &_sfA, &_sfB, sizeof( LSW_SYSTEM_FONTS ) ) < 0;
			}
		};

		

		/** List of system fonts. */
		typedef std::set<LSW_SYSTEM_FONTS, LSW_SYSTEM_FONTS_LESS>
											CSystemFonts;

		/** Family-enumeration context. */
		struct LSW_FAMILY_ENUM_CTX {
			CSystemFonts *					psfFonts;
			uint32_t						ui32Dpi;
			LSW_WS_ID						wiWritingId;
		};

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
		static int CALLBACK					EnumFamilyIsOther_Proc(
			const LOGFONTW * _plfEnum,
			const TEXTMETRICW * /*_ptmMetrics*/,
			DWORD /*_dwType*/,
			LPARAM _lpParam );

		/** FONTENUMPROCW-compatible callback to collect style names. */
		static int CALLBACK					EnumStylesProc(
			const LOGFONTW * _plfEnum,
			const TEXTMETRICW * _ptmEnum,
			DWORD /*_dwType*/,
			LPARAM _lpParam ) {
			LSW_ENUM_STYLES_CTX * pCtx = reinterpret_cast<LSW_ENUM_STYLES_CTX *>(_lpParam);

			const ENUMLOGFONTEXW * pElf = reinterpret_cast<const ENUMLOGFONTEXW *>(_plfEnum);
			/*const NEWTEXTMETRICEXW * pNtmEx = reinterpret_cast<const NEWTEXTMETRICEXW *>(_ptmEnum);
			(void)pNtmEx;*/

			std::wstring wsStyle = pElf->elfStyle;	// Preferred: actual UI style name.
			if ( wsStyle.empty() ) {
				const NEWTEXTMETRICW * pNtm = reinterpret_cast<const NEWTEXTMETRICW *>(_ptmEnum);
				bool bItalic = (_plfEnum->lfItalic != 0);
				bool bBold = pNtm->tmWeight >= FW_SEMIBOLD;
				if ( bBold && bItalic ) { wsStyle = L"Bold Italic"; }
				else if ( bBold ) { wsStyle = L"Bold"; }
				else if ( bItalic ) { wsStyle = L"Italic"; }
				else { wsStyle = L"Regular"; }
			}

			pCtx->pusNames->insert( wsStyle );
			return 1;
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
		static int CALLBACK					EnumSizesProc( const LOGFONTW * /*_plfEnum*/,
			const TEXTMETRICW * _ptmEnum,
			DWORD _dwType,
			LPARAM _lpParam );

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
		static int CALLBACK					EnumFamilies_Proc( const LOGFONTW * _plfEnum,
			const TEXTMETRICW * _ptmEnum,
			DWORD _dwType,
			LPARAM _lpParam );

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
		static int CALLBACK					EnumStyles_Proc( const LOGFONTW * _plfEnum,
			const TEXTMETRICW * _ptmEnum,
			DWORD _dwType,
			LPARAM _lpParam );

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
		static int CALLBACK					EnumSizes_Proc( const LOGFONTW * /*_plfEnum*/,
			const TEXTMETRICW * _ptmEnum,
			DWORD _dwType,
			LPARAM _lpParam );

		/**
		 * \brief Enumerate all faces (Family + Style) on the system for a given writing style.
		 * 
		 * \param _uDpi		DPI used to convert pixel heights to points for raster faces.
		 * \param _wiId		The writing style for which to gather fonts.
		 * \return Returns an array of system fonts.
		 */
		static CSystemFonts					EnumAllFaces( uint32_t _uDpi, LSW_WS_ID _wiId = LSW_WS_ANY );

		/**
		 * \brief Enumerates full style names for a given family using ENUMLOGFONTEXW.
		 * 
		 * \param _wsFamily The family whose styles to enumerate.
		 * \param _vOutStyles Receives unique style names as reported by GDI (e.g., L"Regular", L"Light", L"Light Italic", L"Black").
		 */
		static VOID							EnumStylesForFamily( const std::wstring &_wsFamily, std::vector<std::wstring> &_vOutStyles );

		/**
		 * \brief Enumerates full style names for a given family using ENUMLOGFONTEXW.
		 * 
		 * \param _wcFaceName The family whose styles to enumerate.
		 * \param _sOutStyles Receives the styles for the given font family.
		 */
		static VOID							EnumStylesForFamily( const WCHAR _wcFaceName[LF_FACESIZE], CFontStyles &_sOutStyles );

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
		static VOID							EnumSizesForFace( const WCHAR _wcFaceName[LF_FACESIZE], LSW_FONT_STYLE &_fsStyle, uint32_t _uDpi );

		/**
		 * \brief Tests whether a GLYPHSET fully covers the Unicode range [ _ui32A, _ui32B ] (inclusive).
		 * 
		 * \param _pgsGlyphs Pointer to a GLYPHSET returned by GetFontUnicodeRanges().
		 * \param _ui32A Inclusive start code point.
		 * \param _ui32B Inclusive end code point.
		 * \return Returns true if the glyph set contains every code point in the interval.
		 */
		static bool							GlyphSetHasRange( const GLYPHSET * _pgsGlyphs, uint32_t _ui32A, uint32_t _ui32B ) {
			if ( _pgsGlyphs == nullptr ) { return false; }
			for ( DWORD I = 0; I < _pgsGlyphs->cRanges; ++I ) {
				const WCRANGE & rRange = _pgsGlyphs->ranges[I];
				uint32_t ui32Start = rRange.wcLow;
				uint32_t ui32End   = rRange.wcLow + rRange.cGlyphs - 1;
				if ( ui32End < _ui32A ) { continue; }
				if ( ui32Start > _ui32B ) { break; }
				if ( ui32Start <= _ui32A && ui32End >= _ui32B ) { return true; }
				if ( ui32Start >= _ui32A && ui32Start < _ui32B ) { return true; }
			}
			return false;
		}

		/**
		 * \brief Returns a numeric rank for a style name to sort by weight then italic.
		 * 
		 * \param _wsStyle Style name.
		 * \return Returns an increasing integer rank (lower first).
		 */
		static inline int32_t				RankStyle( const std::wstring & _wsStyle ) {
			std::wstring wsTmp = _wsStyle; for ( wchar_t & w : wsTmp ) { w = ::towlower( w ); }
			int32_t i32Base =
				wsTmp.find( L"thin" )		!= std::wstring::npos ? 100 :
				wsTmp.find( L"extralight" )	!= std::wstring::npos || wsTmp.find( L"ultralight" )	!= std::wstring::npos ? 200 :
				wsTmp.find( L"semilight" )	!= std::wstring::npos || wsTmp.find( L"book" )			!= std::wstring::npos ? 350 :
				wsTmp.find( L"light" )		!= std::wstring::npos ? 300 :
				wsTmp.find( L"regular" )	!= std::wstring::npos || wsTmp.find( L"normal" )		!= std::wstring::npos ? 400 :
				wsTmp.find( L"medium" )		!= std::wstring::npos ? 500 :
				wsTmp.find( L"semibold" )	!= std::wstring::npos || wsTmp.find( L"demi" )			!= std::wstring::npos ? 600 :
				wsTmp.find( L"bold" )		!= std::wstring::npos ? 700 :
				wsTmp.find( L"extrabold" )	!= std::wstring::npos || wsTmp.find( L"ultrabold" )		!= std::wstring::npos ? 800 :
				wsTmp.find( L"black" )		!= std::wstring::npos || wsTmp.find( L"heavy" )			!= std::wstring::npos ? 900 : 400;
			bool bItalic = wsTmp.find( L"italic" ) != std::wstring::npos;
			return i32Base + (bItalic ? 1 : 0);
		}

		/**
		 * \brief Converts a style label (e.g., L"Light", L"Black", L"SemiBold Italic") into LOGFONT weight/italic.
		 * 
		 * \param _wsStyle Style string from the styles list.
		 * \param _lfInOut LOGFONT to modify: sets lfWeight and lfItalic; preserves other fields.
		 */
		static inline VOID					ApplyStyleToLogFont( const std::wstring &_wsStyle, LOGFONTW &_lfInOut ) {
			std::wstring wsTmp = _wsStyle;
			for ( wchar_t & wCh : wsTmp ) { wCh = ::towlower( wCh ); }

			// Weight buckets.
			int32_t i32Weight = FW_NORMAL;
			if ( wsTmp.find( L"thin" ) != std::wstring::npos )																	{ i32Weight = 100; }
			else if ( wsTmp.find( L"extralight" ) != std::wstring::npos || wsTmp.find( L"ultralight" ) != std::wstring::npos )	{ i32Weight = 200; }
			else if ( wsTmp.find( L"light" ) != std::wstring::npos )															{ i32Weight = 300; }
			else if ( wsTmp.find( L"semilight" ) != std::wstring::npos )														{ i32Weight = 350; }
			else if ( wsTmp.find( L"book" ) != std::wstring::npos )																{ i32Weight = 350; }
			else if ( wsTmp.find( L"regular" ) != std::wstring::npos || wsTmp.find( L"normal" ) != std::wstring::npos )			{ i32Weight = 400; }
			else if ( wsTmp.find( L"medium" ) != std::wstring::npos )															{ i32Weight = 500; }
			else if ( wsTmp.find( L"demi" ) != std::wstring::npos || wsTmp.find( L"semibold" ) != std::wstring::npos )			{ i32Weight = 600; }
			else if ( wsTmp.find( L"bold" ) != std::wstring::npos )																{ i32Weight = 700; }
			else if ( wsTmp.find( L"extrabold" ) != std::wstring::npos || wsTmp.find( L"ultrabold" ) != std::wstring::npos )	{ i32Weight = 800; }
			else if ( wsTmp.find( L"black" ) != std::wstring::npos || wsTmp.find( L"heavy" ) != std::wstring::npos )			{ i32Weight = 900; }

			BYTE bItalic = (wsTmp.find( L"italic" ) != std::wstring::npos) ? 1 : 0;

			_lfInOut.lfWeight = i32Weight;
			_lfInOut.lfItalic = bItalic;
		}

		/**
		 * \brief Creates a preview HFONT using the exact style string.
		 * 
		 * \param _wsFamily Family name.
		 * \param _wsStyle Exact style name (e.g., L"Light Italic").
		 * \param _fPointSize Point size.
		 * \param _bUnderline Underline toggle.
		 * \param _bStrikeout Strikeout toggle.
		 * \param _uDpi DPI for point→pixel conversion.
		 * \return Returns an HFONT the caller owns (wrap with LSW_FONT or delete later).
		 */
		static inline HFONT					CreatePreviewHfont( const std::wstring &_wsFamily, const std::wstring &_wsStyle, float _fPointSize, bool _bUnderline, bool _bStrikeout, uint32_t _uDpi ) {
			LOGFONTW lf{};
			::wcsncpy_s( lf.lfFaceName, _wsFamily.c_str(), _TRUNCATE );
			lf.lfCharSet = DEFAULT_CHARSET;
			ApplyStyleToLogFont( _wsStyle, lf );
			lf.lfUnderline = _bUnderline ? TRUE : FALSE;
			lf.lfStrikeOut = _bStrikeout ? TRUE : FALSE;
			lf.lfHeight = -::MulDiv( static_cast<int32_t>(_fPointSize + 0.5f), static_cast<int32_t>(_uDpi), 72 );
			return ::CreateFontIndirectW( &lf );
		}

		/**
		 * \brief Determines if a specific LOGFONT face supports a writing system.
		 * 
		 *        Internally creates the HFONT, selects it into a memory DC, and calls GetFontUnicodeRanges().
		 * \param _lfFace LOGFONTW describing the face to check (lfHeight is ignored and may be 0).
		 * \param _wsSystem Writing system definition to test.
		 * \return Returns true if the face covers all required Unicode blocks for _wsSystem (or always true for "Any").
		 */
		static bool							FaceSupportsWritingSystem( const LOGFONTW &_lfFace, const LSW_WRITING_SYSTEM &_wsSystem );

		/**
		 * Gets a writing system by enumeration.
		 * 
		 * \param _wiId The ID of the writing system to get.
		 * \return Returns a constant reference to the requested writing system.
		 **/
		static const LSW_WRITING_SYSTEM &	WritingSystem( LSW_WS_ID _wiId );

		/**
		 * Gets the total number of writing systems.
		 * 
		 * \return Returns the total number of writing systems.
		 **/
		static size_t						WritingSystems();

		/**
		 * \brief Returns true if a face matches none of the known writing systems except "Other".
		 * 
		 * \param _lfFace Face to test.
		 * \return Returns true if the face is not classified by any non-Other system.
		 */
		static bool							FaceIsOther( const LOGFONTW &_lfFace );

		/** Family predicate for "Other". */
		static inline bool					FamilyIsOther( const wchar_t * _pwszFamily ) {
			if ( _pwszFamily == nullptr || _pwszFamily[0] == L'\0' ) { return false; }

			LOGFONTW lfQuery {};
			lfQuery.lfCharSet = DEFAULT_CHARSET;
			::wcsncpy_s( lfQuery.lfFaceName, _pwszFamily, _TRUNCATE );

			bool bIsIt = false;

			HDC hDc = ::GetDC( NULL );
			if ( hDc ) {
				::EnumFontFamiliesExW(
					hDc,
					&lfQuery,
					EnumFamilyIsOther_Proc,
					reinterpret_cast<LPARAM>(&bIsIt),
					0 );
				::ReleaseDC( NULL, hDc );
			}
			return bIsIt;
		}

		/**
		 * \brief Returns TRUE if the face name represents a vertical (@-prefixed) GDI face.
		 * 
		 * \param _pwszName Null-terminated face/family name.
		 * \return TRUE if the first character is L'@'.
		 */
		static inline BOOL					IsVerticalAtFace( const wchar_t * _pwszName ) {
			if ( _pwszName == nullptr ) { return FALSE; }
			return (_pwszName[0] == L'@') ? TRUE : FALSE;
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
		static std::wstring					WeightToString( int32_t _i32TmWeight );

		/** Helper: test if any of the given ranges exist in the glyph set. */
		static inline bool					GsHasAny( const GLYPHSET * _pgsSet, const LSW_UC_RANGE * _prRange, uint32_t _ui32N ) {
			if ( _ui32N == 0 || _pgsSet == nullptr ) { return false; }
			for ( uint32_t I = 0; I < _ui32N; ++I ) {
				if ( GlyphSetHasRange( _pgsSet, _prRange[I].ui32Start, _prRange[I].ui32End ) ) {
					return true;
				}
			}
			return false;
		}

		/** Helper: test if all of the given ranges exist in the glyph set. */
		static inline bool					GsHasAll( const GLYPHSET * _pgsSet, const LSW_UC_RANGE * _prRange, uint32_t _ui32N ) {
			if ( _pgsSet == nullptr ) { return false; }
			for ( uint32_t I = 0; I < _ui32N; ++I ) {
				if ( !GlyphSetHasRange( _pgsSet, _prRange[I].ui32Start, _prRange[I].ui32End ) ) {
					return false;
				}
			}
			return true;
		}

		/** Japanese: require Kana (Hiragana or Katakana); Han alone is not sufficient. */
		static bool							Check_Japanese( HDC /*_hDc*/, const GLYPHSET * _pgsSet );

		/** Korean: Hangul Jamo or Hangul Syllables. */
		static bool							Check_Korean( HDC /*_hDc*/, const GLYPHSET * _pgsSet ) ;

		/** Traditional Chinese: Han + Bopomofo. */
		static bool							Check_TraditionalChinese( HDC /*_hDc*/, const GLYPHSET * _pgsSet );

		/** Simplified Chinese: Han but not Japanese/Korean/Traditional heuristics. */
		static bool							Check_SimplifiedChinese( HDC /*_hDc*/, const GLYPHSET * _pgsSet );

		/** Vietnamese: Latin core + Vietnamese diacritics (1EA0–1EFF) or combining marks. */
		static bool							Check_Vietnamese( HDC /*_hDc*/, const GLYPHSET * _pgsSet );

		/** Symbol: Symbol charset or significant symbol ranges while lacking alphabetic scripts. */
		static bool							Check_Symbol( HDC _hDc, const GLYPHSET * _pgsSet );

	};

}	// namespace lsw

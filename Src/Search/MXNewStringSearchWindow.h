#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXMemHack.h"
#include "../Options/MXOptions.h"
#include "../Search/MXSearcher.h"
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>
#include <string>

using namespace lsw;

namespace mx {

	class CNewStringSearchWindow : public lsw::CMainWindow {
	public :
		CNewStringSearchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Enumerations.
		enum MX_STRING_TYPE {
			MX_ST_ANSI,
			MX_ST_UTF8,
			MX_ST_UTF16,
			MX_ST_UTF16_BE,
			MX_ST_UTF32,
			MX_ST_UTF32_BE,
			MX_ST_UTFANY,
			MX_ST_REGEX,
			MX_ST_BYTE_ARRAY,
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// WM_CLOSE.
		virtual LSW_HANDLED					Close();

		// Translate a child's tooltip text.
		virtual std::wstring				TranslateTooltip( const std::string &_sText ) { 
			return CStringDecoder::DecodeToWString( _sText.c_str(), _sText.size() );
		}


	protected :
		// == Types.
		struct MX_SEARCH_WIN_DATA {
			MX_STRING_TYPE					stSearchFormat;
			CUtilities::MX_DATA_TYPES		dtArrayCharType;
			CUtilities::MX_REGEX_ENCODING	reRegexEncoding;
			CUtilities::MX_REGEX_FLAVORS	rfRegexFlavor;					// CUtilities::MX_RF_GNU_REGEX
			uint32_t						uiUtfHelperIdx;
			uint32_t						uiRegexHelperIdx;
			std::wstring					wsSearchText;
			std::vector<std::wstring>		vSearchTextHistory;

			std::wstring					wsFromText;
			std::wstring					wsToText;
			std::vector<std::wstring>		vFromHistory;
			std::vector<std::wstring>		vToHistory;
		};


		// == Members.
		// The CMemHack object.
		CMemHack *							m_pmhMemHack;
		// Original width of the Type: combo box.
		LONG								m_lOriginalTypeWidth;
		//Search data for restoring values into the dialog upon reload.
		static MX_SEARCH_WIN_DATA			m_swdData;


		// == Functions.
		// Saves the current dialog values to an MX_SEARCH_WIN_DATA structure.
		void								SaveDialogData( MX_SEARCH_WIN_DATA &_swdData );

		// Validates the dialog box.
		bool								DialogContentsAreValid( CSecureWString &_wsError, CSearcher::MX_SEARCH_PARMS &_spParmsIfValid );

		// Updates the dialog.
		void								UpdateDialog();

		// Gets the final search bytes.
		bool								GetFinalSearchBytes( std::vector<uint8_t> &_vResult, std::vector<uint8_t> &_vMeta, CSecureWString * _pswsError, CUtilities::MX_WILDCARD_USAGE &_wuWildcardUsage );

		// Gets the final search bytes for a given string type.
		bool								GetFinalSearchBytes( uint32_t _uiStringType,
			std::vector<uint8_t> &_vResult, std::vector<uint8_t> &_vMeta, CSecureWString * _pswsError, CUtilities::MX_WILDCARD_USAGE &_wuWildcardUsage );

		// Is the given type a UTF format?
		static bool							IsUtf( uint32_t _uiType ) {
			switch ( _uiType ) {
				case MX_ST_UTF8 : {}
				case MX_ST_UTF16 : {}
				case MX_ST_UTF32 : {}
				case MX_ST_UTF16_BE : {}
				case MX_ST_UTF32_BE : {}
				case MX_ST_UTFANY : { return true; }
				default : { return false; }
			}
		}

		// Is the given type a string type?
		static bool							IsString( uint32_t _uiType ) {
			switch ( _uiType ) {
				case MX_ST_ANSI : {}
				case MX_ST_REGEX : { return true; }
				default : { return IsUtf( _uiType ); }
			}
		}

		// Classifies the wildcard usage in a string.
		static CUtilities::MX_WILDCARD_USAGE
											ClassifyWildcardUseInUtf8String( const CSecureString &_ssString );

		// Classifies the wildcard usage in a raw string.
		static CUtilities::MX_WILDCARD_USAGE
											ClassifyWildcardUseInRawString( const std::vector<uint8_t> &_vMeta );
	};

}	// namespace mx


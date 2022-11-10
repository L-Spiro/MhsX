#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXMemHack.h"
#include "../Options/MXOptions.h"
#include "../Search/MXSearcher.h"
#include <EEExpEvalContainer.h>
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>
#include <string>

using namespace lsw;

namespace mx {

	class CNewExpressionSearchWindow : public lsw::CMainWindow {
	public :
		CNewExpressionSearchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


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
		struct MX_EXP_SEARCH_WIN_DATA {
			CUtilities::MX_DATA_TYPES		dtDataType;
			CUtilities::MX_EXP_SEARCH_QQ_MEANING
											esqmEvalType;
			bool							bDynamic;
			CSecureString					ssExpression;
			CSecureWString					wsFixedSizeText;
			std::vector<CSecureWString>		vFixedSizeHistory;

			CSecureWString					wsAlignmentText;
			std::vector<CSecureWString>		vAlignmentHistory;

			CSecureWString					wsDummyText;
			std::vector<CSecureWString>		vDummyHistory;
		};


		// == Members.
		//MX_OPTIONS *						m_poOptions;
		CMemHack *							m_pmhMemHack;

		static MX_EXP_SEARCH_WIN_DATA		m_eswdData;


		// == Functions.
		// Saves the current dialog values to an MX_EXP_SEARCH_WIN_DATA structure.
		void								SaveDialogData( MX_EXP_SEARCH_WIN_DATA &_eswdData );

		// Validates the dialog box.
		bool								DialogContentsAreValid( CSecureWString &_wsError, CSearcher::MX_SEARCH_PARMS &_spParmsIfValid );

		// Updates the dialog.
		void								UpdateDialog();

		// Verifies that the expression is valid and returns descriptive texts.
		bool								VerifyExpression( CSecureString &_sExpression, CSecureWString &_wsResult, CSecureWString &_wsHelp, CUtilities::MX_EXP_SEARCH_QQ_MEANING _esqmEvalType, CUtilities::MX_DATA_TYPES _dtDataType );

		// WM_CTLCOLORSTATIC.
		virtual LSW_HANDLED					CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush );

		// User-variable function handler.
		static bool __stdcall 				UserVarHandler( uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );
	};

}	// namespace mx


#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXMemHack.h"
#include "../Options/MXOptions.h"
#include "../Search/MXSearcher.h"
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	class CStandardSubsearchWindow : public lsw::CMainWindow {
	public :
		CStandardSubsearchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


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
			uint32_t						uiEvalType;

			std::wstring					wsLValText;
			std::wstring					wsRValText;
			std::wstring					wsExpText;
			std::vector<std::wstring>		vLValHistory;
			std::vector<std::wstring>		vRValHistory;

			std::vector<std::wstring>		vExpHistory;

			BOOL							bInvertResults;
		};

		// == Members.
		//MX_OPTIONS *						m_poOptions;
		CMemHack *							m_pmhMemHack;
		// Was the last manually typed expression null?
		bool								m_bLastTypeExpressionWasEmpty;

		static MX_SEARCH_WIN_DATA			m_swdData;


		// == Functions.
		// Saves the current dialog values to an MX_SEARCH_WIN_DATA structure.
		void								SaveDialogData( MX_SEARCH_WIN_DATA &_swdData );

		// Validates the dialog box.
		bool								DialogContentsAreValid( CSecureWString &_wsError, CSearcher::MX_SUBSEARCH_PARMS &_spParmsIfValid );

		// Verifies that the expression is valid and returns descriptive texts.
		bool								VerifyExpression( CSecureString &_sExpression, CSecureWString &_wsResult, CSecureWString &_wsHelp, CUtilities::MX_DATA_TYPES _dtDataType );

		// Updates the dialog.
		void								UpdateDialog();

		// Creates an expression for searching for exact values.
		void								CreateExactValueExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for not-equal-to values.
		void								CreateNotEqualToExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for increased values.
		void								CreateIncreasedExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &/*_rLVal*/, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for decreased values.
		void								CreateDecreasedExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &/*_rLVal*/, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for ranged values.
		void								CreateRangeExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal,
			const ee::CExpEvalContainer::EE_RESULT &_rRVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for greater-than values.
		void								CreateGreaterThanExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for less-than values.
		void								CreateLessThanExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for same-as-before values.
		void								CreateSameAsBeforeExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &/*_rLVal*/, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for same-as-before values.
		void								CreateDifferentFromBeforeExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &/*_rLVal*/, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for changed-by values.
		void								CreateChangedByExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for changed-by-percent values.
		void								CreateChangedByPercentExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for changed-by-range values.
		void								CreateChangedByRangeExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal,
			const ee::CExpEvalContainer::EE_RESULT &_rRVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );

		// Creates an expression for searching for changed-by-range-percent values.
		void								CreateChangedByRangePercentExpression( CSecureString &_sExpression, const ee::CExpEvalContainer::EE_RESULT &_rLVal,
			const ee::CExpEvalContainer::EE_RESULT &_rRVal, CUtilities::MX_DATA_TYPES _dtDataType, bool _bInv );
	};

}	// namespace mx

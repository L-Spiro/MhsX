#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXStringDecoder.h"
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	class CFloatingPointStudioWindow : public lsw::CMainWindow {
	public :
		CFloatingPointStudioWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CFloatingPointStudioWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_OPENFILE,

			MX_I_TOTAL,
		};

		// Presets.
		enum MX_PRESETS {
			MX_P_FLOAT32,
			MX_P_FLOAT64,
			MX_P_FLOAT16,
			MX_P_6_BIT,
			MX_P_8_BIT,
			MX_P_10_BIT,
			MX_P_11_BIT,
			MX_P_AMD_9511,
			MX_P_CUSTOM,
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// WM_COMMAND from menu.
		virtual LSW_HANDLED					MenuCommand( WORD _Id ) { return Command( 0, _Id, nullptr ); }

		// WM_CLOSE.
		virtual LSW_HANDLED					Close();

		// WM_ERASEBKGND.
		virtual LSW_HANDLED					EraseBkgnd( HDC _hDc );

		// Translate a child's tooltip text.
		virtual std::wstring				TranslateTooltip( const std::string &_sText ) { 
			return CStringDecoder::DecodeToWString( _sText.c_str(), _sText.size() );
		}

		// Prepares to create the window.  Creates the atom if necessary.
		static void							PrepareFloatingPointStudio();


	protected :
		// == Types.
		// The preset database.
		struct MX_PRESET {
			const char *					pcEncryptedName;
			size_t							sNameLen;

			bool							bSign;
			bool							bImpBit;
			uint8_t							ui8ExpBits;
			uint8_t							ui8ManBits;

			MX_PRESETS						pId;
		};

		// For gathering the current parameters for the float.
		struct MX_CUR_SETTINGS {
			bool							bSign;
			bool							bImpBit;
			uint8_t							ui8ExpBits;
			uint8_t							ui8ManBits;
		};

		// == Members.
		// The widget of the last input.
		CWidget *							m_pwLastInput;
		// Are we updating inputs?
		bool								m_bUpdatingInputs;

		// The main window class.
		static ATOM							m_aAtom;
		// The database of float types.
		static MX_PRESET					m_pPresets[];


		// == Functions.
		// Gathers the floating-point settings.
		bool								GatherSettings( MX_CUR_SETTINGS &_csSettings );

		// Updates the inputs based on the current settings and a value, skipping m_pwLastInput.
		void								UpdateInputs( const ee::CFloatX &_fVal, const MX_CUR_SETTINGS &_csSettings );

		// Updates the inputs based on whatever was last.
		void								UpdateByLast();

		// Updates the inputs based on the expression input.
		void								UpdateDefault( CWidget * _pwWidget );

		// Updates the inputs based on the hex input.
		void								UpdateHex( CWidget * _pwWidget );

		// Updates the inputs based on the binary input.
		void								UpdateBinary( CWidget * _pwWidget );

		// Updates the inputs based on the checkboxes.
		void								UpdateChecks();

		// Updates the check boxes.
		void								UpdateChecks( const ee::CFloatX &_fVal, const MX_CUR_SETTINGS &_csSettings );

		// Updates the check boxes (only enable/disable).
		void								UpdateChecks( const MX_CUR_SETTINGS &_csSettings );

		// Updates the limit edits.
		void								UpdateLimits( const MX_CUR_SETTINGS &_csSettings );

		// Updates the Misc. section.
		void								UpdateMisc( const ee::CFloatX &_fVal, const MX_CUR_SETTINGS &_csSettings );

		// WM_CTLCOLORSTATIC.
		virtual LSW_HANDLED					CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush );

		// Updates the preset list selection or text.
		void								UpdatePreset( const MX_CUR_SETTINGS &_csSettings );
	};

}	// namespace mx

#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXStringDecoder.h"
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	class CTemplateWindow : public lsw::CMainWindow {
	public :
		CTemplateWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CTemplateWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_OPENFILE,

			MX_I_TOTAL,
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
		static void							PrepareTemplate();


	protected :
		// == Members.
		// The main window class.
		static ATOM							m_aAtom;
		
	};

}	// namespace mx

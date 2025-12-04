#pragma once

#include "../MXMhsX.h"
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	class CStringTheoryWindow : public lsw::CMainWindow {
		public :
		CStringTheoryWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CStringTheoryWindow();


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// WM_GETMINMAXINFO.
		virtual LSW_HANDLED					GetMinMaxInfo( MINMAXINFO * _pmmiInfo );

		// Sets the text in the main input box.
		void								SetText( const wchar_t * _pwcTet );

		// Updates the dialog.
		void								UpdateDialog();

		/**
		 * Handles WM_ERASEBKGND.
		 * \brief Allows custom background erasing.
		 *
		 * \param _hDc Device context provided for erasing.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					EraseBkgnd( HDC /*_hDc*/ ) { return LSW_H_CONTINUE; }

		// Prepares to create the window.  Creates the atom if necessary.
		static void							PrepareStringTheory();


	protected :
		// == Members.
		// The main window class.
		static ATOM							m_aAtom;


		// == Functions.
		// WM_CTLCOLORSTATIC.
		virtual LSW_HANDLED					CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush );

	};

}	// namespace mx

#pragma once
#include "../MXMhsX.h"
#include "../Layouts/MXFindLayout.h"

#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	/**
	 * The modeless dialog shown while a long Hex Editor search runs.  The search keeps running; Cancel (or closing the dialog) asks
	 *	it to stop.  The Hex Editor destroys the dialog when the search ends.
	 **/
	class CFindProgressDialog : public lsw::CMainWindow {
	public :
		CFindProgressDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CFindProgressDialog();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.  Sets the range of the progress bar.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing.
		 **/
		virtual LSW_HANDLED											InitDialog();

		/**
		 * Handles WM_CLOSE.  Asks the search to stop (the dialog is destroyed by the Hex Editor when it has).
		 *
		 * \return Returns LSW_H_HANDLED.
		 **/
		virtual LSW_HANDLED											Close();

		/**
		 * Handles WM_COMMAND.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
		 * \param _wId The identifier for the command or control.
		 * \param _pwSrc The source control for control notifications; otherwise nullptr.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED											Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Handles WM_COMMAND from a menu (Escape).
		 *
		 * \param _wId The menu command identifier.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED											MenuCommand( WORD _wId );

		/**
		 * Shows how far the search has gone.
		 *
		 * \param _dProgress The progress, from 0 to 1.
		 **/
		void														SetProgress( double _dProgress );


	protected :
		// == Members.
		/** The parameters. */
		CFindLayout::MX_FIND_PROGRESS_PARMS *						m_pfppParms;


		// == Functions.
		/**
		 * Asks the search to stop.
		 **/
		void														Cancel();


	private :
		/** The parent object. */
		typedef lsw::CMainWindow									CParent;
	};

}	// namespace mx

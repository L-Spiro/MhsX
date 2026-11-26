#pragma once
#include "../MXMhsX.h"
#include "../Layouts/MXFindLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	/**
	 * The Hex Editor's Find dialog (Find -> Find...).  Holds every Find setting; the searching is done by the Hex Editor through
	 *	CFindLayout::MX_FIND_DIALOG_PARMS::fAction while the dialog stays open.
	 **/
	class CFindDialog : public lsw::CMainWindow {
	public :
		CFindDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CFindDialog();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.  Fills in the settings.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing.
		 **/
		virtual LSW_HANDLED											InitDialog();

		/**
		 * Handles WM_CLOSE.  Keeps the settings and closes the dialog.
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
		 * Handles WM_COMMAND from a menu (Enter and Escape).
		 *
		 * \param _wId The menu command identifier.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED											MenuCommand( WORD _wId );


	protected :
		// == Members.
		/** The parameters. */
		CFindLayout::MX_FIND_DIALOG_PARMS *							m_pfdpParms;
		/** Set while the controls are being filled in (so that changes do not start searches). */
		bool														m_bFilling = false;


		// == Functions.
		/**
		 * Does a search.
		 *
		 * \param _faAction What to do.
		 **/
		void														DoAction( CFindLayout::MX_FIND_ACTION _faAction );

		/**
		 * Copies the settings from the controls to the parameters.
		 *
		 * \param _swsError Receives a description of the problem on failure.
		 * \return Returns true if every setting is valid.
		 **/
		bool														Gather( CSecureWString &_swsError );

		/**
		 * Adds the search text to the top of the history.
		 *
		 * \param _swsText The text.
		 **/
		void														AddHistory( const CSecureWString &_swsText );

		/**
		 * Enables the options that apply to the selected type and settings and disables the rest.
		 **/
		void														UpdateControls();

		/**
		 * Gets the type selected in the list.
		 *
		 * \return Returns the type.
		 **/
		CHexFind::MX_FIND_TYPE										SelectedType();

		/**
		 * Shows text under the search text.
		 *
		 * \param _swsText The text.
		 **/
		void														SetStatus( const CSecureWString &_swsText );

		/**
		 * Determines whether a radio button or check box is checked.
		 *
		 * \param _wId The ID of the control.
		 * \return Returns true if the control exists and is checked.
		 **/
		bool														Checked( WORD _wId );

		/**
		 * Checks or unchecks a radio button or check box.
		 *
		 * \param _wId The ID of the control.
		 * \param _bCheck Whether to check it.
		 **/
		void														SetChecked( WORD _wId, bool _bCheck );

		/**
		 * Enables or disables a control.
		 *
		 * \param _wId The ID of the control.
		 * \param _bEnable If true, the control is enabled.
		 **/
		void														Enable( WORD _wId, bool _bEnable );

		/**
		 * Reads a whole number from an edit control.
		 *
		 * \param _wId The ID of the control.
		 * \param _ui64Val Receives the number.
		 * \return Returns true if the control holds a valid number.
		 **/
		bool														ReadNumber( WORD _wId, uint64_t &_ui64Val );


	private :
		/** The parent object. */
		typedef lsw::CMainWindow									CParent;
	};

}	// namespace mx

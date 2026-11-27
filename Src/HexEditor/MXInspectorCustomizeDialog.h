#pragma once

#include "../MXMhsX.h"
#include "../Layouts/MXInspectorLayout.h"

#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	/**
	 * The Customize dialog of the Inspector: which data types are shown and in what order, how hexadecimal numbers are written,
	 *	and the date and time formats.
	 **/
	class CInspectorCustomizeDialog : public lsw::CMainWindow {
	public :
		CInspectorCustomizeDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CInspectorCustomizeDialog();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.  Fills in the controls from the settings.
		 *
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED													InitDialog();

		/**
		 * Handles WM_CLOSE.  Closes the dialog as Cancel.
		 *
		 * \return Returns LSW_H_HANDLED.
		 **/
		virtual LSW_HANDLED													Close();

		/**
		 * Handles WM_COMMAND.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
		 * \param _wId The identifier for the command or control.
		 * \param _pwSrc The source control for control notifications; otherwise nullptr.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED													Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Handles WM_COMMAND from a menu (Escape).
		 *
		 * \param _wId The menu command identifier.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED													MenuCommand( WORD _wId );


	protected :
		// == Members.
		/** The parameters. */
		CInspectorLayout::MX_CUSTOMIZE_PARMS *								m_pcpParms;
		/** The settings being edited. */
		CInspector::MX_SETTINGS												m_sSettings;


		// == Functions.
		/**
		 * Fills the list of data types from m_sSettings.
		 *
		 * \param _iSelect The row to select.
		 **/
		void																FillList( int _iSelect );

		/**
		 * Reads the check boxes of the list into m_sSettings.
		 **/
		void																ReadChecks();

		/**
		 * Fills in the formats from m_sSettings.
		 **/
		void																FillFormats();

		/**
		 * Gets the selected row of the list.
		 *
		 * \return Returns the selected row, or -1.
		 **/
		int																	SelectedRow();

		/**
		 * Moves the selected data type up or down.
		 *
		 * \param _iBy -1 to move it up, 1 to move it down.
		 **/
		void																MoveSelected( int _iBy );

		/**
		 * Gets the text of a combo box (what is typed in its edit box).
		 *
		 * \param _wId The combo box.
		 * \return Returns the text.
		 **/
		std::wstring														ComboText( WORD _wId );


	private :
		/** The parent class. */
		typedef lsw::CMainWindow											CParent;
	};

}	// namespace mx

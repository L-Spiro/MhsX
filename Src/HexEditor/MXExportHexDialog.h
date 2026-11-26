#pragma once
#include "../MXMhsX.h"
#include "../Layouts/MXExportHexLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	/**
	 * The Export Hex dialog (File -> Export Hex...).  Chooses the file to write, the format, the bytes per row, and the options
	 *	that apply to the format chosen (the others are disabled).  Changing the format changes the extension of the file.
	 **/
	class CExportHexDialog : public lsw::CMainWindow {
	public :
		CExportHexDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CExportHexDialog();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.  Fills in the defaults.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing.
		 **/
		virtual LSW_HANDLED											InitDialog();

		/**
		 * Handles WM_CLOSE.  Closes the dialog as Cancel.
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


	protected :
		// == Members.
		/** The parameters. */
		CExportHexLayout::MX_EXPORT_HEX_PARMS *						m_pehpParms;


		// == Functions.
		/**
		 * Gets the index of the export type selected in the list.
		 *
		 * \return Returns the index into CExportHexLayout::Types().
		 **/
		uint32_t													SelectedType();

		/**
		 * Enables the options that apply to the selected export type and disables the rest.
		 **/
		void														UpdateOptions();

		/**
		 * Gives the file in the Export File box the extension of the selected export type.
		 **/
		void														UpdateExtension();

		/**
		 * Asks for the file to write (the Browse button).
		 **/
		void														Browse();

		/**
		 * Checks what was entered and fills in the parameters.
		 *
		 * \return Returns true if everything is valid.
		 **/
		bool														Finish();

		/**
		 * Determines whether a radio button or check box is checked.
		 *
		 * \param _wId The ID of the control.
		 * \return Returns true if the control exists and is checked.
		 **/
		bool														Checked( WORD _wId );

		/**
		 * Enables or disables a control.
		 *
		 * \param _wId The ID of the control.
		 * \param _bEnable If true, the control is enabled.
		 **/
		void														Enable( WORD _wId, bool _bEnable );


	private :
		/** The parent object. */
		typedef lsw::CMainWindow									CParent;
	};

}	// namespace mx

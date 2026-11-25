#pragma once
#include "../MXMhsX.h"
#include "../Layouts/MXSelectRangeLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	/**
	 * The Select Range dialog (Select -> Select Range...).  Selects a range of bytes in the Hex Editor given its start address and
	 *	either its size or its (inclusive) end address.  Both can be expressions.
	 **/
	class CSelectRangeDialog : public lsw::CMainWindow {
	public :
		CSelectRangeDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CSelectRangeDialog();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.  Fills in the default start address and size.
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
		 * Handles WM_COMMAND.  OK checks and returns the values; Cancel closes the dialog; the radio buttons change the meaning of the
		 *	second value (converting it).
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
		CSelectRangeLayout::MX_SELECT_RANGE_PARMS *					m_psrpParms;
		/** True while the second value is an end address. */
		bool														m_bEnd = false;


		// == Functions.
		/**
		 * Evaluates the expression in an edit control as an unsigned number, showing an error if it is not one.
		 *
		 * \param _wId The edit control.
		 * \param _ui64Value Receives the value.
		 * \param _bShowErrors If true, errors are shown in a message box and the control is given the focus.
		 * \return Returns true if the text is an unsigned number.
		 **/
		bool														GetValue( WORD _wId, uint64_t &_ui64Value, bool _bShowErrors );

		/**
		 * Puts a number in an edit control, in hexadecimal or decimal.
		 *
		 * \param _wId The edit control.
		 * \param _ui64Value The value.
		 **/
		void														SetValue( WORD _wId, uint64_t _ui64Value );

		/**
		 * Switches the second value between a size and an end address, converting what has been entered if it can be read.
		 *
		 * \param _bEnd If true, the second value becomes the (inclusive) end address.
		 **/
		void														SetEndMode( bool _bEnd );


	private :
		/** The parent object. */
		typedef lsw::CMainWindow									CParent;
	};

}	// namespace mx

#pragma once
#include "../MXMhsX.h"
#include "../Layouts/MXEditBytesLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	/**
	 * The Insert Bytes, Overwrite Bytes, and Set File Size dialogs (Edit -> Insert/Overwrite).  Insert Bytes and Overwrite Bytes take
	 *	a start address, a size or an (inclusive) end address, and a byte value; Set File Size takes the new size and the value of
	 *	any bytes added.  Numbers can be expressions, entered in hexadecimal or decimal.  The byte value can be entered in hexadecimal,
	 *	decimal, or as a character, and the three are kept in sync.
	 **/
	class CEditBytesDialog : public lsw::CMainWindow {
	public :
		CEditBytesDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CEditBytesDialog();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.  Fills in the default values.
		 *
		 * \return Returns LSW_H_HANDLED (the focus is set here).
		 **/
		virtual LSW_HANDLED											InitDialog();

		/**
		 * Handles WM_CLOSE.  Closes the dialog as Cancel.
		 *
		 * \return Returns LSW_H_HANDLED.
		 **/
		virtual LSW_HANDLED											Close();

		/**
		 * Handles WM_COMMAND.  OK checks and returns the values; Cancel closes the dialog; the radio buttons convert what has been
		 *	entered; the byte-value edits update each other.
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
		CEditBytesLayout::MX_EDIT_BYTES_PARMS *						m_pebpParms;
		/** True while the second value is an end address. */
		bool														m_bEnd = false;
		/** True while numbers are entered in hexadecimal. */
		bool														m_bHex = true;
		/** True while the byte-value edits are being updated (so that they do not update each other again). */
		bool														m_bSyncing = false;


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

		/**
		 * Switches numbers between hexadecimal and decimal, converting what has been entered if it can be read.
		 *
		 * \param _bHex If true, numbers are entered in hexadecimal.
		 **/
		void														SetHexMode( bool _bHex );

		/**
		 * Reads the byte value from one of its edits.
		 *
		 * \param _wId The edit (hexadecimal, decimal, or character).
		 * \param _ui8Value Receives the value.
		 * \return Returns true if the edit holds a byte value.
		 **/
		bool														ReadByteValue( WORD _wId, uint8_t &_ui8Value ) const;

		/**
		 * Puts the byte value into its edits, except the one being typed into.
		 *
		 * \param _ui8Value The value.
		 * \param _wSkip The edit not to change, or 0.
		 **/
		void														WriteByteValue( uint8_t _ui8Value, WORD _wSkip );

		/**
		 * Shows an error and gives a control the focus.
		 *
		 * \param _wsMsg The message.
		 * \param _wId The control.
		 **/
		void														ShowError( const std::wstring &_wsMsg, WORD _wId );


	private :
		/** The parent object. */
		typedef lsw::CMainWindow									CParent;
	};

}	// namespace mx

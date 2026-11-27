#pragma once
#include "../MXMhsX.h"
#include "../Layouts/MXPasteSpecialLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

#include <string>

using namespace lsw;

namespace mx {

	/**
	 * The Paste Special dialog (Edit -> Paste Special...).  Lists the formats on the Windows clipboard; Paste (or a double-click)
	 *	reads the raw bytes of the chosen format.  Formats held as GDI objects (bitmaps, metafiles, palettes) have no bytes to paste
	 *	and are not listed.
	 **/
	class CPasteSpecialDialog : public lsw::CMainWindow {
	public :
		CPasteSpecialDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CPasteSpecialDialog();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.  Lists the clipboard formats.
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
		 * Handles WM_COMMAND.  Paste (or a double-click on a format) reads the format; Cancel closes the dialog.
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
		 * Gets the name of a clipboard format.
		 *
		 * \param _uiFormat The format.
		 * \return Returns the name of the format.
		 **/
		static std::wstring											FormatName( UINT _uiFormat );

		/**
		 * Determines whether a clipboard format is held as a GDI object or another handle that is not global memory.
		 *
		 * \param _uiFormat The format.
		 * \return Returns true if the format has no bytes to read.
		 **/
		static bool													IsHandleFormat( UINT _uiFormat );

		/**
		 * Reads the bytes of a clipboard format.  Text formats end at their terminators.
		 *
		 * \param _hWnd The window that opens the clipboard.
		 * \param _uiFormat The format.
		 * \param _vBytes Receives the bytes.
		 * \return Returns true if the format was read.
		 **/
		static bool													ReadFormat( HWND _hWnd, UINT _uiFormat, std::vector<uint8_t> &_vBytes );


	protected :
		// == Members.
		/** The parameters. */
		CPasteSpecialLayout::MX_PASTE_SPECIAL_PARMS *				m_ppspParms;


		// == Functions.
		/**
		 * Reads the selected format and closes the dialog.
		 **/
		void														PasteSelected();


	private :
		/** The parent object. */
		typedef lsw::CMainWindow									CParent;
	};

}	// namespace mx

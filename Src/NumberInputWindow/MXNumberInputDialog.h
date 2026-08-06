#pragma once

#include "../MXMhsX.h"
#include "../Layouts/MXNumberInputLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	class CNumberInputDialog : public lsw::CMainWindow {
	public :
		CNumberInputDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CNumberInputDialog();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.
		 * \brief Initializes a dialog after creation and before it is shown.
		 *
		 * Use to set control defaults, attach data bindings, and position the dialog.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
		 */
		virtual LSW_HANDLED											InitDialog();

		/**
		 * Handles WM_CLOSE.
		 * \brief Requests window closure and gives a chance to veto or prompt the user.
		 *
		 * Use to confirm unsaved changes or to cancel the close. Post WM_DESTROY if approved.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing; return LSW_H_HANDLED to stop it.
		 */
		virtual LSW_HANDLED											Close();

		/**
		 * Handles WM_COMMAND dispatch.
		 * \brief Routes command notifications from menus, accelerators, or child controls.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
		 * \param _wId The identifier for the command or control.
		 * \param _pwSrc The source control for control notifications; otherwise nullptr.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED											Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ );

		/**
		 * Handles WM_COMMAND from a menu.
		 * \brief Invoked for menu command selections.
		 *
		 * \param _wId The menu command identifier.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED											MenuCommand( WORD _wId );


	protected :
		// == Members.
		// A pointer to the options.
		CNumberInputLayout::MX_NUMBER_DIALOG_PARMS *				m_pndpParms;
		// The parent object.
		typedef lsw::CMainWindow									CParent;
	};

}	// namespace mx

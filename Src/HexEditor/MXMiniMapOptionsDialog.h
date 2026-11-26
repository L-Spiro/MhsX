#pragma once
#include "../MXMhsX.h"
#include "../Layouts/MXMiniMapOptionsLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

using namespace lsw;

namespace mx {

	/**
	 * The Mini Map Options dialog (View -> Mini Map -> Mini Map Options..., or right-clicking the Mini Map).  Changes are shown in the
	 *	hex editor as they are made; Cancel (or Escape) puts back the options the dialog started with.
	 **/
	class CMiniMapOptionsDialog : public lsw::CMainWindow {
	public :
		CMiniMapOptionsDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CMiniMapOptionsDialog();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.  Fills in the controls from the current options.
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
		 * Handles WM_COMMAND.  Any change to a control is shown in the hex editor right away; OK keeps the changes, Cancel undoes them,
		 *	Hide hides the Mini Map, and Reset puts back the default options.
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
		 * Handles WM_HSCROLL.  Moving a slider updates its value label and the hex editor.
		 *
		 * \param _uScrollPos The current position of the scroll box if the scroll type is SB_THUMBPOSITION or SB_THUMBTRACK.
		 * \param _uScrollType The type of scroll request.
		 * \param _pwWidget The control sending the message (a slider), or nullptr.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED											HScroll( USHORT _uScrollPos, USHORT _uScrollType, CWidget * _pwWidget );


	protected :
		// == Members.
		/** The parameters. */
		CMiniMapOptionsLayout::MX_MINI_MAP_OPTIONS_PARMS *			m_pmmopParms;
		/** The options when the dialog was opened (put back by Cancel). */
		CMiniMap::MX_MINI_MAP										m_mmOriginal;
		/** True once the controls have been filled in (notifications sent while filling them are ignored). */
		bool														m_bReady = false;


		// == Functions.
		/**
		 * Fills in the controls from a set of options.
		 *
		 * \param _mmOptions The options to show.
		 **/
		void														SetControls( const CMiniMap::MX_MINI_MAP &_mmOptions );

		/**
		 * Reads the options from the controls.  Values that cannot be read are kept from the options the dialog was given.
		 *
		 * \param _mmOptions Receives the options.
		 **/
		void														GetControls( CMiniMap::MX_MINI_MAP &_mmOptions ) const;

		/**
		 * Enables or disables the controls whose use depends on other controls (the palette, Bytes Per Line, and Auto Snap).
		 **/
		void														UpdateEnabled();

		/**
		 * Updates the value labels beside the sliders.
		 **/
		void														UpdateValueLabels();

		/**
		 * Reads the options from the controls and shows them in the hex editor.
		 **/
		void														Apply();

		/**
		 * Puts back the original options and closes the dialog.
		 **/
		void														Cancel();

		/**
		 * Enables or disables a control.
		 *
		 * \param _wId The control to enable or disable.
		 * \param _bEnable If true, the control is enabled.
		 **/
		void														EnableCtrl( WORD _wId, bool _bEnable );

		/**
		 * Reads an unsigned number from an edit control.
		 *
		 * \param _wId The edit control.
		 * \param _ui32Value Receives the value if it can be read.
		 * \return Returns true if the edit holds a number greater than 0.
		 **/
		bool														GetUInt( WORD _wId, uint32_t &_ui32Value ) const;


	private :
		/** The parent object. */
		typedef lsw::CMainWindow									CParent;
	};

}	// namespace mx

#include "MXFindProgressDialog.h"

#include <algorithm>
#include <CommCtrl.h>


namespace mx {

	CFindProgressDialog::CFindProgressDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pfppParms( reinterpret_cast<CFindLayout::MX_FIND_PROGRESS_PARMS *>(_ui64Data) ) {
	}
	CFindProgressDialog::~CFindProgressDialog() {
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.  Sets the range of the progress bar.
	 *
	 * \return Returns LSW_H_CONTINUE to allow default processing.
	 **/
	CWidget::LSW_HANDLED CFindProgressDialog::InitDialog() {
		auto pwBar = FindChild( CFindLayout::MX_FI_PROGRESS_BAR );
		if ( pwBar ) { ::SendMessageW( pwBar->Wnd(), PBM_SETRANGE32, 0, 1000 ); }
		return CParent::InitDialog();
	}

	/**
	 * Handles WM_CLOSE.  Asks the search to stop (the dialog is destroyed by the Hex Editor when it has).
	 *
	 * \return Returns LSW_H_HANDLED.
	 **/
	CWidget::LSW_HANDLED CFindProgressDialog::Close() {
		Cancel();
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
	 * \param _wId The identifier for the command or control.
	 * \param _pwSrc The source control for control notifications; otherwise nullptr.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CFindProgressDialog::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case IDCANCEL : {}										MX_FALLTHROUGH
			case CFindLayout::MX_FI_PROGRESS_CANCEL : {
				Cancel();
				return LSW_H_HANDLED;
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_COMMAND from a menu (Escape).
	 *
	 * \param _wId The menu command identifier.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CFindProgressDialog::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {}										MX_FALLTHROUGH
			case CFindLayout::MX_FI_PROGRESS_CANCEL : {
				Cancel();
				return LSW_H_HANDLED;
			}
		}
		return CParent::MenuCommand( _wId );
	}

	/**
	 * Shows how far the search has gone.
	 *
	 * \param _dProgress The progress, from 0 to 1.
	 **/
	void CFindProgressDialog::SetProgress( double _dProgress ) {
		auto pwBar = FindChild( CFindLayout::MX_FI_PROGRESS_BAR );
		if ( pwBar ) {
			::SendMessageW( pwBar->Wnd(), PBM_SETPOS, static_cast<WPARAM>(std::clamp( _dProgress, 0.0, 1.0 ) * 1000.0), 0 );
		}
	}

	/**
	 * Asks the search to stop.
	 **/
	void CFindProgressDialog::Cancel() {
		if ( m_pfppParms && m_pfppParms->pbCancel ) { (*m_pfppParms->pbCancel) = true; }
		// The button cannot be pressed again while the search finishes its current step.
		auto pwCancel = FindChild( CFindLayout::MX_FI_PROGRESS_CANCEL );
		if ( pwCancel ) { ::EnableWindow( pwCancel->Wnd(), FALSE ); }
	}

}	// namespace mx

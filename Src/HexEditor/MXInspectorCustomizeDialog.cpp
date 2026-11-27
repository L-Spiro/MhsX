#include "MXInspectorCustomizeDialog.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"

#include <ComboBox/LSWComboBox.h>
#include <ListView/LSWListView.h>

#include <CommCtrl.h>


namespace mx {

	CInspectorCustomizeDialog::CInspectorCustomizeDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pcpParms( reinterpret_cast<CInspectorLayout::MX_CUSTOMIZE_PARMS *>(_ui64Data) ) {
		try {
			if ( m_pcpParms ) { m_sSettings = m_pcpParms->sSettings; }
			else { CInspector::DefaultSettings( m_sSettings ); }
			CInspector::FixSettings( m_sSettings );
		}
		catch ( ... ) {}
	}
	CInspectorCustomizeDialog::~CInspectorCustomizeDialog() {
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.  Fills in the controls from the settings.
	 *
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CInspectorCustomizeDialog::InitDialog() {
		CParent::InitDialog();
		try {
			auto plvList = static_cast<CListView *>(FindChild( CInspectorLayout::MX_ICI_TYPES_LIST ));
			if ( plvList ) {
				const DWORD dwExStyle = LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER;
				::SendMessageW( plvList->Wnd(), LVM_SETEXTENDEDLISTVIEWSTYLE, dwExStyle, dwExStyle );
				const INT iCol = plvList->AddColumn( _DEC_WS_2CECF817_Type.c_str() );
				RECT rClient = {};
				::GetClientRect( plvList->Wnd(), &rClient );
				plvList->SetColumnWidth( iCol, std::max<INT>( rClient.right - rClient.left - ::GetSystemMetrics( SM_CXVSCROLL ), 50 ) );
			}

			auto pcbDate = static_cast<CComboBox *>(FindChild( CInspectorLayout::MX_ICI_DATE_COMBO ));
			if ( pcbDate ) {
				const struct {
					const char *											pcText;
					size_t													sLen;
				} aFormats[] = {
					{ _T_LEN_B27B5A4D_MM_dd_yyyy },
					{ _T_LEN_55E76E1E_dd_MM_yyyy },
					{ _T_LEN_76CD7848_yyyy_MM_dd },
					{ _T_LEN_C6070C02_d_MMM_yyyy },
					{ _T_LEN_9FD1DA43_MMM_d__yyyy },
					{ _T_LEN_A0D23EEB_dddd__MMMM_d__yyyy },
				};
				for ( const auto & fThis : aFormats ) {
					pcbDate->AddString( mx::CStringDecoder::DecodeToWString( fThis.pcText, fThis.sLen ).c_str() );
				}
			}
			auto pcbTime = static_cast<CComboBox *>(FindChild( CInspectorLayout::MX_ICI_TIME_COMBO ));
			if ( pcbTime ) {
				const struct {
					const char *											pcText;
					size_t													sLen;
				} aFormats[] = {
					{ _T_LEN_0153EF33_HH_mm_ss },
					{ _T_LEN_7D450060_h_mm_ss_tt },
					{ _T_LEN_3AADFC0E_HH_mm },
					{ _T_LEN_0C4C2A22_h_mm_tt },
				};
				for ( const auto & fThis : aFormats ) {
					pcbTime->AddString( mx::CStringDecoder::DecodeToWString( fThis.pcText, fThis.sLen ).c_str() );
				}
			}
		}
		catch ( ... ) {}

		FillList( 0 );
		FillFormats();

		auto pwList = FindChild( CInspectorLayout::MX_ICI_TYPES_LIST );
		if ( pwList ) { pwList->SetFocus(); }
		// The focus was set here.
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_CLOSE.  Closes the dialog as Cancel.
	 *
	 * \return Returns LSW_H_HANDLED.
	 **/
	CWidget::LSW_HANDLED CInspectorCustomizeDialog::Close() {
		::EndDialog( Wnd(), FALSE );
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
	CWidget::LSW_HANDLED CInspectorCustomizeDialog::Command( WORD /*_wCtrlCode*/, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case CInspectorLayout::MX_ICI_MOVE_UP : {
				MoveSelected( -1 );
				break;
			}
			case CInspectorLayout::MX_ICI_MOVE_DOWN : {
				MoveSelected( 1 );
				break;
			}
			case CInspectorLayout::MX_ICI_RESET : {
				try {
					const bool bAllowDocking = m_sSettings.bAllowDocking;
					CInspector::DefaultSettings( m_sSettings );
					m_sSettings.bAllowDocking = bAllowDocking;
				}
				catch ( ... ) {}
				FillList( 0 );
				FillFormats();
				break;
			}
			case IDOK : {}												MX_FALLTHROUGH
			case CInspectorLayout::MX_ICI_OK : {
				ReadChecks();
				try {
					// The formats must work (an empty format uses the user's format from Windows).
					SYSTEMTIME stTest = {};
					stTest.wYear = 2001;
					stTest.wMonth = 2;
					stTest.wDay = 3;
					stTest.wHour = 16;
					stTest.wMinute = 5;
					stTest.wSecond = 6;
					wchar_t wszTest[128];
					const std::wstring wsDate = ComboText( CInspectorLayout::MX_ICI_DATE_COMBO );
					if ( wsDate.size() && ::GetDateFormatEx( LOCALE_NAME_USER_DEFAULT, 0, &stTest, wsDate.c_str(), wszTest, static_cast<int>(std::size( wszTest )), nullptr ) <= 0 ) {
						::MessageBeep( MB_ICONWARNING );
						auto pwDate = FindChild( CInspectorLayout::MX_ICI_DATE_COMBO );
						if ( pwDate ) { pwDate->SetFocus(); }
						break;
					}
					const std::wstring wsTime = ComboText( CInspectorLayout::MX_ICI_TIME_COMBO );
					if ( wsTime.size() && ::GetTimeFormatEx( LOCALE_NAME_USER_DEFAULT, 0, &stTest, wsTime.c_str(), wszTest, static_cast<int>(std::size( wszTest )) ) <= 0 ) {
						::MessageBeep( MB_ICONWARNING );
						auto pwTime = FindChild( CInspectorLayout::MX_ICI_TIME_COMBO );
						if ( pwTime ) { pwTime->SetFocus(); }
						break;
					}
					m_sSettings.wsDateFormat = wsDate;
					m_sSettings.wsTimeFormat = wsTime;
					if ( m_pcpParms ) { m_pcpParms->sSettings = m_sSettings; }
				}
				catch ( ... ) {
					::EndDialog( Wnd(), FALSE );
					break;
				}
				::EndDialog( Wnd(), TRUE );
				break;
			}
			case IDCANCEL : {}											MX_FALLTHROUGH
			case CInspectorLayout::MX_ICI_CANCEL : {
				::EndDialog( Wnd(), FALSE );
				break;
			}
		}
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND from a menu (Escape).
	 *
	 * \param _wId The menu command identifier.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CInspectorCustomizeDialog::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {
				::EndDialog( Wnd(), FALSE );
				return LSW_H_HANDLED;
			}
			case IDOK : {
				return Command( 0, CInspectorLayout::MX_ICI_OK, nullptr );
			}
		}
		return CParent::MenuCommand( _wId );
	}

	/**
	 * Fills the list of data types from m_sSettings.
	 *
	 * \param _iSelect The row to select.
	 **/
	void CInspectorCustomizeDialog::FillList( int _iSelect ) {
		auto plvList = static_cast<CListView *>(FindChild( CInspectorLayout::MX_ICI_TYPES_LIST ));
		if ( !plvList ) { return; }
		try {
			plvList->SetRedraw( false );
			plvList->DeleteAll();
			for ( size_t I = 0; I < m_sSettings.vRows.size(); ++I ) {
				const INT iIdx = plvList->InsertItem( CInspector::TypeName( m_sSettings.vRows[I].itType ).c_str(), static_cast<LPARAM>(m_sSettings.vRows[I].itType) );
				if ( iIdx >= 0 ) { ListView_SetCheckState( plvList->Wnd(), iIdx, m_sSettings.vRows[I].bShown ? TRUE : FALSE ); }
			}
			if ( _iSelect >= 0 && size_t( _iSelect ) < m_sSettings.vRows.size() ) {
				ListView_SetItemState( plvList->Wnd(), _iSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
				::SendMessageW( plvList->Wnd(), LVM_ENSUREVISIBLE, static_cast<WPARAM>(_iSelect), FALSE );
			}
		}
		catch ( ... ) {}
		plvList->SetRedraw( true );
	}

	/**
	 * Reads the check boxes of the list into m_sSettings.
	 **/
	void CInspectorCustomizeDialog::ReadChecks() {
		auto plvList = static_cast<CListView *>(FindChild( CInspectorLayout::MX_ICI_TYPES_LIST ));
		if ( !plvList ) { return; }
		const int iCount = static_cast<int>(::SendMessageW( plvList->Wnd(), LVM_GETITEMCOUNT, 0, 0 ));
		for ( int I = 0; I < iCount && size_t( I ) < m_sSettings.vRows.size(); ++I ) {
			m_sSettings.vRows[I].bShown = ListView_GetCheckState( plvList->Wnd(), I ) != FALSE;
		}
	}

	/**
	 * Fills in the formats from m_sSettings.
	 **/
	void CInspectorCustomizeDialog::FillFormats() {
		auto pwDate = FindChild( CInspectorLayout::MX_ICI_DATE_COMBO );
		if ( pwDate ) { ::SetWindowTextW( pwDate->Wnd(), m_sSettings.wsDateFormat.c_str() ); }
		auto pwTime = FindChild( CInspectorLayout::MX_ICI_TIME_COMBO );
		if ( pwTime ) { ::SetWindowTextW( pwTime->Wnd(), m_sSettings.wsTimeFormat.c_str() ); }
	}

	/**
	 * Gets the selected row of the list.
	 *
	 * \return Returns the selected row, or -1.
	 **/
	int CInspectorCustomizeDialog::SelectedRow() {
		auto plvList = static_cast<CListView *>(FindChild( CInspectorLayout::MX_ICI_TYPES_LIST ));
		if ( !plvList ) { return -1; }
		return static_cast<int>(::SendMessageW( plvList->Wnd(), LVM_GETNEXTITEM, static_cast<WPARAM>(-1), MAKELPARAM( LVNI_SELECTED, 0 ) ));
	}

	/**
	 * Moves the selected data type up or down.
	 *
	 * \param _iBy -1 to move it up, 1 to move it down.
	 **/
	void CInspectorCustomizeDialog::MoveSelected( int _iBy ) {
		ReadChecks();
		const int iSel = SelectedRow();
		const int iTo = iSel + _iBy;
		if ( iSel < 0 || iTo < 0 || size_t( iSel ) >= m_sSettings.vRows.size() || size_t( iTo ) >= m_sSettings.vRows.size() ) { return; }
		std::swap( m_sSettings.vRows[iSel], m_sSettings.vRows[iTo] );
		FillList( iTo );
		auto pwList = FindChild( CInspectorLayout::MX_ICI_TYPES_LIST );
		if ( pwList ) { pwList->SetFocus(); }
	}

	/**
	 * Gets the text of a combo box (what is typed in its edit box).
	 *
	 * \param _wId The combo box.
	 * \return Returns the text.
	 **/
	std::wstring CInspectorCustomizeDialog::ComboText( WORD _wId ) {
		auto pwCombo = FindChild( _wId );
		if ( !pwCombo ) { return std::wstring(); }
		const int iLen = ::GetWindowTextLengthW( pwCombo->Wnd() );
		if ( iLen <= 0 ) { return std::wstring(); }
		std::wstring wsText( size_t( iLen ) + 1, L'\0' );
		const int iGot = ::GetWindowTextW( pwCombo->Wnd(), wsText.data(), iLen + 1 );
		wsText.resize( size_t( std::max( iGot, 0 ) ) );
		return wsText;
	}

}	// namespace mx

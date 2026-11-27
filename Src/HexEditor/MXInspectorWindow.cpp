#include "MXInspectorWindow.h"
#include "../Layouts/MXInspectorLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Layouts/MXUiIds.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"

#include <Base/LSWBase.h>
#include <Helpers/LSWHelpers.h>
#include <EEExpEval.h>

#include <CommCtrl.h>
#include <windowsx.h>


namespace mx {

	/** The ID of the list view's subclass. */
	static constexpr UINT_PTR							MX_INSPECTOR_LIST_SUBCLASS = 0x4D58494C;
	/** The ID of the edit box's subclass. */
	static constexpr UINT_PTR							MX_INSPECTOR_EDIT_SUBCLASS = 0x4D584945;
	/** How often the values are refreshed (milliseconds). */
	static constexpr UINT								MX_INSPECTOR_REFRESH_MS = 100;
	/** The most bytes read for the values (enough for the longest string). */
	static constexpr size_t								MX_INSPECTOR_READ_BYTES = CInspector::MX_MAX_STRING_CHARS * 4 + 8;

	/** WPARAM values of MX_IM_EDIT_DONE. */
	enum MX_EDIT_DONE : WPARAM {
		MX_ED_CANCEL									= 0,				/**< Escape: cancel and return the focus to the table. */
		MX_ED_COMMIT									= 1,				/**< Enter: write the value. */
		MX_ED_FOCUS_LOST								= 2,				/**< The focus went elsewhere: cancel and leave the focus where it went. */
	};

	CInspectorWindow::CInspectorWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CDockable( _wlLayout.AddStyle( WS_CLIPCHILDREN ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}
	CInspectorWindow::~CInspectorWindow() {
		m_tRefresh.Stop();
		if ( m_hEdit ) {
			::RemoveWindowSubclass( m_hEdit, EditSubclassProc, MX_INSPECTOR_EDIT_SUBCLASS );
			::DestroyWindow( m_hEdit );
			m_hEdit = NULL;
		}
		CListView * plvList = ListView();
		if ( plvList && plvList->Wnd() ) {
			::RemoveWindowSubclass( plvList->Wnd(), ListSubclassProc, MX_INSPECTOR_LIST_SUBCLASS );
		}
		m_pihHost = nullptr;
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.  Sets up the table and starts refreshing it.
	 *
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CInspectorWindow::InitDialog() {
		CListView * plvList = ListView();
		if ( plvList ) {
			const DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP;
			::SendMessageW( plvList->Wnd(), LVM_SETEXTENDEDLISTVIEWSTYLE, dwExStyle, dwExStyle );

			int iTypeWidth = plvList->GetStringWidth( _DEC_WS_2CECF817_Type.c_str() );
			for ( uint32_t I = 0; I < CInspector::MX_IT_TOTAL; ++I ) {
				iTypeWidth = std::max( iTypeWidth, plvList->GetStringWidth( CInspector::TypeName( static_cast<CInspector::MX_INSPECTOR_TYPE>(I) ).c_str() ) );
			}
			const INT iType = plvList->AddColumn( _DEC_WS_2CECF817_Type.c_str() );
			plvList->SetColumnWidth( iType, iTypeWidth + 20 );
			const INT iValue = plvList->AddColumn( _DEC_WS_DCB67730_Value.c_str() );
			plvList->SetColumnWidth( iValue, 200 );
			plvList->SetRightColumnSnap( TRUE );

			::SetWindowSubclass( plvList->Wnd(), ListSubclassProc, MX_INSPECTOR_LIST_SUBCLASS, reinterpret_cast<DWORD_PTR>(this) );
		}

		Rebuild();
		m_tRefresh.Start( Wnd(), MX_TI_INSPECTOR_REFRESH, MX_INSPECTOR_REFRESH_MS, nullptr );
		ForceSizeUpdate();
		return CParent::InitDialog();
	}

	/**
	 * Handles WM_CONTEXTMENU.  Shows the Inspector menu.
	 *
	 * \param _pwControl The control that was right-clicked.
	 * \param _iX The horizontal position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
	 * \param _iY The vertical position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CInspectorWindow::ContextMenu( CWidget * /*_pwControl*/, INT _iX, INT _iY ) {
		CListView * plvList = ListView();
		if ( !plvList ) { return LSW_H_CONTINUE; }
		EndEdit( false );

		int iRow = SelectedRow();
		m_iContextColumn = 1;
		POINT ptMenu = { _iX, _iY };
		if ( _iX == -1 && _iY == -1 ) {
			RECT rItem = {};
			if ( iRow >= 0 && ListView_GetItemRect( plvList->Wnd(), iRow, &rItem, LVIR_BOUNDS ) ) {
				ptMenu = { rItem.left + 16, rItem.bottom };
			}
			else {
				ptMenu = { 0, 0 };
			}
			::ClientToScreen( plvList->Wnd(), &ptMenu );
		}
		else {
			LVHITTESTINFO htiHit = {};
			htiHit.pt = ptMenu;
			::ScreenToClient( plvList->Wnd(), &htiHit.pt );
			if ( ListView_SubItemHitTest( plvList->Wnd(), &htiHit ) >= 0 && htiHit.iItem >= 0 ) {
				iRow = htiHit.iItem;
				m_iContextColumn = htiHit.iSubItem ? 1 : 0;
				ListView_SetItemState( plvList->Wnd(), iRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
			}
		}
		const bool bRow = iRow >= 0 && size_t( iRow ) < m_vRows.size();
		CInspector::MX_SETTINGS sDefault;
		CInspector::MX_SETTINGS & sSettings = m_pihHost ? m_pihHost->InspectorSettings() : sDefault;

		enum : DWORD {
			MX_MENU_MAIN = 1,
			MX_MENU_COPY,
			MX_MENU_FORMAT,
			MX_ITEM_COPY = 200,
			MX_ITEM_FORMAT,
		};
		LSW_MENU_ITEM miMain[] = {
			//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	lpwcText, stTextLen							bSkip	aHotkey
			{ FALSE,		MX_IC_COPY_ROW,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CF3CA396_Copy__Row, _LEN_CF3CA396 ),								FALSE,	{ static_cast<BYTE>(FVIRTKEY | FCONTROL), 'C', MX_IC_COPY_ROW } },
			{ FALSE,		MX_ITEM_COPY,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_446FE619__Copy, _LEN_446FE619 ),									FALSE },
			{ TRUE,			0,							FALSE,		FALSE,		FALSE,		nullptr,  0,									FALSE },
			{ FALSE,		MX_IC_EXPORT_CSV,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_378CD84F__Export_CSV___, _LEN_378CD84F ),							FALSE },
			{ TRUE,			0,							FALSE,		FALSE,		FALSE,		nullptr,  0,									FALSE },
			{ FALSE,		MX_ITEM_FORMAT,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_BDC4BDF7_Column__Display_Format, _LEN_BDC4BDF7 ),				FALSE },
			{ TRUE,			0,							FALSE,		FALSE,		FALSE,		nullptr,  0,									FALSE },
			{ FALSE,		MX_IC_CUSTOMIZE,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_1AE033AB_C_ustomize___, _LEN_1AE033AB ),								FALSE },
			{ TRUE,			0,							FALSE,		FALSE,		FALSE,		nullptr,  0,									FALSE },
			{ FALSE,		MX_IC_ALLOW_DOCKING,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_721536E4_Allow_Doc_king, _LEN_721536E4 ),							FALSE },
		};
		LSW_MENU_ITEM miCopy[] = {
			//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	lpwcText, stTextLen							bSkip
			{ FALSE,		MX_IC_COPY_TYPE,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_850B0AB2__Type, _LEN_850B0AB2 ),									FALSE },
			{ FALSE,		MX_IC_COPY_VALUE,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_7AA3C7A5__Value, _LEN_7AA3C7A5 ),									FALSE },
			{ TRUE,			0,							FALSE,		FALSE,		FALSE,		nullptr,  0,									FALSE },
			{ FALSE,		MX_IC_COPY_ROW,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CF3CA396_Copy__Row, _LEN_CF3CA396 ),								FALSE },
			{ FALSE,		MX_IC_COPY_COLUMN,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_4ADC8137_Copy_C_olumn, _LEN_4ADC8137 ),							FALSE },
			{ FALSE,		MX_IC_COPY_ALL,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_70142490_Copy__All, _LEN_70142490 ),								FALSE },
		};
		LSW_MENU_ITEM miFormat[] = {
			//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	lpwcText, stTextLen							bSkip
			{ FALSE,		MX_IC_FORMAT_HEX,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9CC0956A__Hex, _LEN_9CC0956A ),									FALSE },
			{ FALSE,		MX_IC_FORMAT_DECIMAL,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5217E1C9__Decimal, _LEN_5217E1C9 ),								FALSE },
			{ FALSE,		MX_IC_FORMAT_OCTAL,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F81D67E4__Octal, _LEN_F81D67E4 ),									FALSE },
			{ FALSE,		MX_IC_FORMAT_BINARY,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_418F9E48__Binary, _LEN_418F9E48 ),									FALSE },
		};
		const LSW_MENU_LAYOUT miMenus[] = {
			{ MX_MENU_MAIN, 0, 0, std::size( miMain ), miMain },
			{ MX_MENU_COPY, MX_MENU_MAIN, MX_ITEM_COPY, std::size( miCopy ), miCopy },
			{ MX_MENU_FORMAT, MX_MENU_MAIN, MX_ITEM_FORMAT, std::size( miFormat ), miFormat },
		};
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		HMENU hMenu = plmLayout ? plmLayout->CreateDisplayPopupMenu( miMenus, std::size( miMenus ) ) : NULL;
		if ( !hMenu ) { return LSW_H_HANDLED; }

		// States.
		for ( UINT uiId : { UINT( MX_IC_COPY_ROW ), UINT( MX_IC_COPY_TYPE ), UINT( MX_IC_COPY_VALUE ) } ) {
			::EnableMenuItem( hMenu, uiId, MF_BYCOMMAND | (bRow ? MF_ENABLED : MF_GRAYED) );
		}
		const UINT uiFormat = sSettings.nfFormat == CInspector::MX_NF_HEX ? MX_IC_FORMAT_HEX :
			sSettings.nfFormat == CInspector::MX_NF_OCTAL ? MX_IC_FORMAT_OCTAL :
			sSettings.nfFormat == CInspector::MX_NF_BINARY ? MX_IC_FORMAT_BINARY : MX_IC_FORMAT_DECIMAL;
		::CheckMenuRadioItem( hMenu, MX_IC_FORMAT_HEX, MX_IC_FORMAT_BINARY, uiFormat, MF_BYCOMMAND );
		::CheckMenuItem( hMenu, MX_IC_ALLOW_DOCKING, MF_BYCOMMAND | (sSettings.bAllowDocking ? MF_CHECKED : MF_UNCHECKED) );

		// "Go to Address" for an integer that is an address in the data.
		uint64_t ui64GoTo = 0;
		bool bGoTo = false;
		if ( bRow && m_bSourceValid ) {
			CInspector::MX_DATA dData;
			dData.pui8Data = m_isSource.vBytes.data();
			dData.sSize = m_isSource.vBytes.size();
			dData.bBigEndian = m_isSource.bBigEndian;
			uint64_t ui64Value;
			if ( CInspector::UnsignedValue( m_vRows[iRow].itType, dData, ui64Value ) &&
				ui64Value >= m_isSource.ui64StartAddress && ui64Value - m_isSource.ui64StartAddress <= m_isSource.ui64Size ) {
				ui64GoTo = ui64Value - m_isSource.ui64StartAddress;
				bGoTo = true;
				try {
					std::wstring wsText = _DEC_WS_6B4613A5__Go_to_Address_;
					CInspector::FormatUnsigned( ui64Value, CInspector::MX_NF_HEX, CInspector::HexStyle(), wsText );
					::InsertMenuW( hMenu, 0, MF_BYPOSITION | MF_STRING, MX_IC_GO_TO_ADDRESS, wsText.c_str() );
					::InsertMenuW( hMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr );
				}
				catch ( ... ) { bGoTo = false; }
			}
		}

		const UINT uiCmd = static_cast<UINT>(::TrackPopupMenu( hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY, ptMenu.x, ptMenu.y, 0, Wnd(), nullptr ));
		::DestroyMenu( hMenu );

		try {
			switch ( uiCmd ) {
				case MX_IC_COPY_ROW : {
					CopyRow();
					break;
				}
				case MX_IC_COPY_TYPE : {}						MX_FALLTHROUGH
				case MX_IC_COPY_VALUE : {
					if ( bRow ) { SetClipboardText( CellText( iRow, uiCmd == MX_IC_COPY_TYPE ? 0 : 1 ) ); }
					break;
				}
				case MX_IC_COPY_COLUMN : {}						MX_FALLTHROUGH
				case MX_IC_COPY_ALL : {
					std::wstring wsText;
					if ( uiCmd == MX_IC_COPY_ALL ) {
						wsText = _DEC_WS_2CECF817_Type;
						wsText.push_back( L'\t' );
						wsText += _DEC_WS_DCB67730_Value;
					}
					for ( int I = 0; I < int( m_vRows.size() ); ++I ) {
						if ( wsText.size() || I ) {
							wsText.push_back( L'\r' );
							wsText.push_back( L'\n' );
						}
						if ( uiCmd == MX_IC_COPY_ALL ) {
							wsText += CellText( I, 0 );
							wsText.push_back( L'\t' );
							wsText += CellText( I, 1 );
						}
						else {
							wsText += CellText( I, m_iContextColumn );
						}
					}
					SetClipboardText( wsText );
					break;
				}
				case MX_IC_EXPORT_CSV : {
					ExportCsv();
					break;
				}
				case MX_IC_FORMAT_HEX : {}						MX_FALLTHROUGH
				case MX_IC_FORMAT_DECIMAL : {}					MX_FALLTHROUGH
				case MX_IC_FORMAT_OCTAL : {}					MX_FALLTHROUGH
				case MX_IC_FORMAT_BINARY : {
					sSettings.nfFormat = uiCmd == MX_IC_FORMAT_HEX ? CInspector::MX_NF_HEX :
						uiCmd == MX_IC_FORMAT_OCTAL ? CInspector::MX_NF_OCTAL :
						uiCmd == MX_IC_FORMAT_BINARY ? CInspector::MX_NF_BINARY : CInspector::MX_NF_DECIMAL;
					Refresh();
					break;
				}
				case MX_IC_CUSTOMIZE : {
					Customize();
					break;
				}
				case MX_IC_ALLOW_DOCKING : {
					sSettings.bAllowDocking = !sSettings.bAllowDocking;
					SetAllowDocking( sSettings.bAllowDocking );
					break;
				}
				case MX_IC_GO_TO_ADDRESS : {
					if ( bGoTo && m_pihHost ) { m_pihHost->InspectorGoTo( ui64GoTo ); }
					break;
				}
			}
		}
		catch ( ... ) {}
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_TIMER.  Refreshes the values.
	 *
	 * \param _uiptrId The timer identifier.
	 * \param _tpProc The timer callback, if any.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CInspectorWindow::Timer( UINT_PTR _uiptrId, TIMERPROC /*_tpProc*/ ) {
		if ( _uiptrId == MX_TI_INSPECTOR_REFRESH ) {
			Refresh();
			return LSW_H_HANDLED;
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles the private messages sent by the in-place edit box (commit and cancel).
	 *
	 * \param _uMsg The message.
	 * \param _wParam The message's WPARAM.
	 * \param _lParam The message's LPARAM.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CInspectorWindow::CustomPrivateMsg( UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		if ( _uMsg == MX_IM_EDIT_DONE ) {
			if ( !m_hEdit || static_cast<uint32_t>(_lParam) != m_ui32EditId ) { return LSW_H_HANDLED; }
			if ( _wParam == MX_ED_FOCUS_LOST ) {
				if ( m_hEdit ) {
					HWND hEdit = m_hEdit;
					m_hEdit = NULL;
					const int iRow = m_iEditRow;
					m_iEditRow = -1;
					::RemoveWindowSubclass( hEdit, EditSubclassProc, MX_INSPECTOR_EDIT_SUBCLASS );
					::DestroyWindow( hEdit );
					CListView * plvList = ListView();
					if ( plvList && iRow >= 0 && size_t( iRow ) < m_vRows.size() ) { plvList->SetItemText( iRow, 1, m_vRows[iRow].wsValue.c_str() ); }
				}
			}
			else {
				EndEdit( _wParam == MX_ED_COMMIT );
			}
			return LSW_H_HANDLED;
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * The WM_SIZE handler.
	 *
	 * \param _wParam The type of resizing requested.
	 * \param _lWidth The new width of the client area.
	 * \param _lHeight The new height of the client area.
	 * \return Returns a LSW_HANDLED enumeration.
	 **/
	CWidget::LSW_HANDLED CInspectorWindow::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		if ( m_hEdit ) { EndEdit( false ); }
		return CParent::Size( _wParam, _lWidth, _lHeight );
	}

	/**
	 * Sets the object that supplies the data and carries out edits.  The rows are rebuilt from its settings.
	 *
	 * \param _pihHost The host.
	 **/
	void CInspectorWindow::SetHost( CInspectorHost * _pihHost ) {
		m_pihHost = _pihHost;
		if ( m_pihHost ) { SetAllowDocking( m_pihHost->InspectorSettings().bAllowDocking ); }
		Rebuild();
	}

	/**
	 * Rebuilds the rows from the settings (after they change).
	 **/
	void CInspectorWindow::Rebuild() {
		EndEdit( false );
		CListView * plvList = ListView();
		if ( !plvList ) { return; }
		try {
			const int iSel = SelectedRow();
			const CInspector::MX_INSPECTOR_TYPE itSel = (iSel >= 0 && size_t( iSel ) < m_vRows.size()) ? m_vRows[iSel].itType : CInspector::MX_IT_TOTAL;

			plvList->SetRedraw( false );
			plvList->DeleteAll();
			m_vRows.clear();
			if ( m_pihHost ) {
				CInspector::MX_SETTINGS & sSettings = m_pihHost->InspectorSettings();
				CInspector::FixSettings( sSettings );
				for ( const auto & rsRow : sSettings.vRows ) {
					if ( !rsRow.bShown ) { continue; }
					MX_ROW rRow;
					rRow.itType = rsRow.itType;
					const INT iIdx = plvList->InsertItem( CInspector::TypeName( rsRow.itType ).c_str(), static_cast<LPARAM>(rsRow.itType) );
					if ( iIdx < 0 ) { continue; }
					m_vRows.push_back( rRow );
					if ( rsRow.itType == itSel ) {
						ListView_SetItemState( plvList->Wnd(), iIdx, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
					}
				}
			}
			plvList->SetRedraw( true );
		}
		catch ( ... ) {
			plvList->SetRedraw( true );
		}
		Refresh();
	}

	/**
	 * Refreshes the values from the data at the inspected address.  Does nothing while the Inspector is hidden.
	 **/
	void CInspectorWindow::Refresh() {
		if ( !m_pihHost || !Wnd() || !::IsWindowVisible( Wnd() ) ) { return; }
		CListView * plvList = ListView();
		if ( !plvList ) { return; }
		try {
			CInspectorHost::MX_INSPECTOR_SOURCE isSource;
			const bool bValid = m_pihHost->InspectorSource( isSource, MX_INSPECTOR_READ_BYTES );

			// A value being edited belongs to the address where editing began.
			if ( m_hEdit && (!bValid || isSource.ui64Address != m_ui64EditAddress) ) { EndEdit( false ); }

			const CInspector::MX_SETTINGS & sSettings = m_pihHost->InspectorSettings();
			CInspector::MX_DATA dData;
			dData.pui8Data = isSource.vBytes.data();
			dData.sSize = isSource.vBytes.size();
			dData.bBigEndian = isSource.bBigEndian;
			std::wstring wsText;
			for ( size_t I = 0; I < m_vRows.size(); ++I ) {
				MX_ROW & rRow = m_vRows[I];
				size_t sBytes = 0;
				const bool bOk = bValid && dData.pui8Data && CInspector::Format( rRow.itType, dData, sSettings, wsText, sBytes );
				if ( !bOk ) {
					wsText.clear();
					sBytes = 0;
				}
				rRow.bValid = bOk;
				rRow.sBytes = sBytes;
				if ( wsText != rRow.wsValue ) {
					rRow.wsValue = wsText;
					if ( int( I ) != m_iEditRow ) { plvList->SetItemText( int( I ), 1, rRow.wsValue.c_str() ); }
				}
			}
			m_isSource = std::move( isSource );
			m_bSourceValid = bValid;
		}
		catch ( ... ) {}
	}

	/**
	 * Determines whether the keyboard focus is in the Inspector (so that Ctrl+C copies from it rather than from the hex editor).
	 *
	 * \return Returns true if the Inspector or one of its children has the focus.
	 **/
	bool CInspectorWindow::HasFocus() const {
		HWND hFocus = ::GetFocus();
		return hFocus && Wnd() && (hFocus == Wnd() || ::IsChild( Wnd(), hFocus ));
	}

	/**
	 * Copies the selected row ("Type<Tab>Value") to the clipboard.
	 **/
	void CInspectorWindow::CopyRow() {
		const int iRow = SelectedRow();
		if ( iRow < 0 ) { return; }
		try {
			std::wstring wsText = CellText( iRow, 0 );
			wsText.push_back( L'\t' );
			wsText += CellText( iRow, 1 );
			SetClipboardText( wsText );
		}
		catch ( ... ) {}
	}

	/**
	 * Gets the list view.
	 *
	 * \return Returns the list view, or nullptr.
	 **/
	CListView * CInspectorWindow::ListView() {
		return static_cast<CListView *>(FindChild( CInspectorLayout::MX_II_LIST ));
	}

	/**
	 * Gets the selected row.
	 *
	 * \return Returns the index of the selected row, or -1.
	 **/
	int CInspectorWindow::SelectedRow() {
		CListView * plvList = ListView();
		if ( !plvList ) { return -1; }
		const int iRow = static_cast<int>(::SendMessageW( plvList->Wnd(), LVM_GETNEXTITEM, static_cast<WPARAM>(-1), MAKELPARAM( LVNI_SELECTED, 0 ) ));
		return (iRow >= 0 && size_t( iRow ) < m_vRows.size()) ? iRow : -1;
	}

	/**
	 * Gets the text of a cell as shown.
	 *
	 * \param _iRow The row.
	 * \param _iColumn The column (0 = Type, 1 = Value).
	 * \return Returns the text of the cell.
	 **/
	std::wstring CInspectorWindow::CellText( int _iRow, int _iColumn ) {
		if ( _iRow < 0 || size_t( _iRow ) >= m_vRows.size() ) { return std::wstring(); }
		if ( _iColumn == 0 ) { return std::wstring( CInspector::TypeName( m_vRows[_iRow].itType ).c_str() ); }
		return m_vRows[_iRow].wsValue;
	}

	/**
	 * Begins editing the value of a row in place.
	 *
	 * \param _iRow The row.
	 **/
	void CInspectorWindow::BeginEdit( int _iRow ) {
		EndEdit( false );
		CListView * plvList = ListView();
		if ( !plvList || !m_pihHost || _iRow < 0 || size_t( _iRow ) >= m_vRows.size() ) { return; }
		Refresh();
		if ( !m_bSourceValid ) { return; }
		if ( m_isSource.bReadOnly ) {
			Status( _DEC_WS_15A738F1_The_data_is_read_only_, true );
			::MessageBeep( MB_ICONWARNING );
			return;
		}
		const MX_ROW & rRow = m_vRows[_iRow];
		const size_t sSize = CInspector::TypeSize( rRow.itType );
		if ( sSize && m_isSource.vBytes.size() < sSize ) {
			Status( _DEC_WS_F6281A5B_There_is_not_enough_data_for_this_type_here_, true );
			::MessageBeep( MB_ICONWARNING );
			return;
		}

		::SendMessageW( plvList->Wnd(), LVM_ENSUREVISIBLE, static_cast<WPARAM>(_iRow), FALSE );
		RECT rCell = {};
		rCell.top = 1;
		rCell.left = LVIR_LABEL;
		if ( !::SendMessageW( plvList->Wnd(), LVM_GETSUBITEMRECT, static_cast<WPARAM>(_iRow), reinterpret_cast<LPARAM>(&rCell) ) ) { return; }
		RECT rClient = {};
		::GetClientRect( plvList->Wnd(), &rClient );
		rCell.right = std::max( rCell.right, rCell.left + 40 );
		rCell.right = std::min( rCell.right, rClient.right );

		HWND hEdit = ::CreateWindowExW( 0, WC_EDITW, rRow.wsValue.c_str(),
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
			rCell.left, rCell.top, rCell.right - rCell.left, rCell.bottom - rCell.top,
			plvList->Wnd(), NULL, lsw::CBase::GetThisHandle(), nullptr );
		if ( !hEdit ) { return; }
		::SendMessageW( hEdit, WM_SETFONT, ::SendMessageW( plvList->Wnd(), WM_GETFONT, 0, 0 ), FALSE );
		::SendMessageW( hEdit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM( 2, 2 ) );
		if ( !::SetWindowSubclass( hEdit, EditSubclassProc, MX_INSPECTOR_EDIT_SUBCLASS, reinterpret_cast<DWORD_PTR>(this) ) ) {
			::DestroyWindow( hEdit );
			return;
		}
		m_hEdit = hEdit;
		++m_ui32EditId;
		m_iEditRow = _iRow;
		m_wsEditOriginal = rRow.wsValue;
		m_ui64EditAddress = m_isSource.ui64Address;
		m_sEditBytes = rRow.sBytes;
		::SendMessageW( hEdit, EM_SETSEL, 0, -1 );
		::SetFocus( hEdit );
	}

	/**
	 * Ends editing in place.
	 *
	 * \param _bCommit If true, the value typed is written.
	 **/
	void CInspectorWindow::EndEdit( bool _bCommit ) {
		if ( !m_hEdit ) { return; }
		HWND hEdit = m_hEdit;
		m_hEdit = NULL;
		const int iRow = m_iEditRow;
		m_iEditRow = -1;

		std::wstring wsText;
		try {
			const int iLen = ::GetWindowTextLengthW( hEdit );
			if ( iLen > 0 ) {
				wsText.resize( size_t( iLen ) + 1 );
				const int iGot = ::GetWindowTextW( hEdit, wsText.data(), iLen + 1 );
				wsText.resize( size_t( std::max( iGot, 0 ) ) );
			}
		}
		catch ( ... ) { _bCommit = false; }

		const bool bHadFocus = ::GetFocus() == hEdit;
		::RemoveWindowSubclass( hEdit, EditSubclassProc, MX_INSPECTOR_EDIT_SUBCLASS );
		::DestroyWindow( hEdit );
		CListView * plvList = ListView();
		if ( plvList ) {
			if ( iRow >= 0 && size_t( iRow ) < m_vRows.size() ) { plvList->SetItemText( iRow, 1, m_vRows[iRow].wsValue.c_str() ); }
			if ( bHadFocus ) { ::SetFocus( plvList->Wnd() ); }
		}

		if ( !_bCommit || !m_pihHost || iRow < 0 || size_t( iRow ) >= m_vRows.size() ) { return; }
		// Nothing is written unless the value was changed (values are not rewritten just because they were shown, such as floats
		//	rounded for display or strings with characters shown as dots).
		if ( wsText == m_wsEditOriginal ) { return; }

		try {
			const CInspector::MX_INSPECTOR_TYPE itType = m_vRows[iRow].itType;
			CInspectorHost::MX_INSPECTOR_SOURCE isNow;
			if ( !m_pihHost->InspectorSource( isNow, 1 ) || isNow.ui64Address != m_ui64EditAddress ) {
				Status( _DEC_WS_DB22B597_The_caret_moved__nothing_was_written_, true );
				return;
			}

			std::vector<uint8_t> vBytes;
			CSecureWString swsStatus;
			if ( !CInspector::Parse( itType, wsText, isNow.bBigEndian, m_pihHost->InspectorSettings(), vBytes, swsStatus ) ) {
				Status( swsStatus, true );
				::MessageBeep( MB_ICONWARNING );
				// Back to the edit box with what was typed, so that it can be corrected.
				BeginEdit( iRow );
				if ( m_hEdit ) {
					::SetWindowTextW( m_hEdit, wsText.c_str() );
					::SendMessageW( m_hEdit, EM_SETSEL, 0, -1 );
					m_wsEditOriginal = m_vRows[iRow].wsValue;
				}
				return;
			}
			const bool bString = CInspector::IsString( itType );
			if ( m_pihHost->InspectorWrite( m_ui64EditAddress, bString ? m_sEditBytes : vBytes.size(), vBytes, bString, swsStatus ) ) {
				std::wstring wsDone = CInspector::TypeName( itType ).c_str();
				wsDone += _DEC_WS_17DAA2FD__written_;
				Status( CSecureWString( wsDone ), false );
			}
			else {
				Status( swsStatus, true );
				::MessageBeep( MB_ICONWARNING );
			}
			Refresh();
		}
		catch ( ... ) {}
	}

	/**
	 * Selects the bytes of a row's value in the hex editor.
	 *
	 * \param _iRow The row.
	 **/
	void CInspectorWindow::SelectInHexEditor( int _iRow ) {
		if ( !m_pihHost || _iRow < 0 || size_t( _iRow ) >= m_vRows.size() || !m_bSourceValid ) { return; }
		const MX_ROW & rRow = m_vRows[_iRow];
		size_t sBytes = CInspector::TypeSize( rRow.itType );
		if ( !sBytes ) { sBytes = rRow.sBytes; }
		if ( !sBytes || m_isSource.vBytes.size() < sBytes ) { return; }
		m_pihHost->InspectorSelect( m_isSource.ui64Address, sBytes );
	}

	/**
	 * Puts text on the clipboard.
	 *
	 * \param _wsText The text.
	 * \return Returns true if the text was put on the clipboard.
	 **/
	bool CInspectorWindow::SetClipboardText( const std::wstring &_wsText ) {
		lsw::LSW_CLIPBOARD cbClipboard( Wnd(), true );
		return cbClipboard.IsOpen() && cbClipboard.SetText( _wsText.c_str(), _wsText.size() );
	}

	/**
	 * Exports the table to a CSV file (UTF-8, comma-separated).
	 **/
	void CInspectorWindow::ExportCsv() {
		try {
			// The CSV text: every cell in quotes, with quotes doubled.
			auto AddCell = []( std::wstring &_wsCsv, const std::wstring &_wsCell ) {
				_wsCsv.push_back( L'"' );
				for ( wchar_t wcThis : _wsCell ) {
					if ( wcThis == L'"' ) { _wsCsv.push_back( L'"' ); }
					_wsCsv.push_back( wcThis );
				}
				_wsCsv.push_back( L'"' );
			};
			std::wstring wsCsv;
			AddCell( wsCsv, std::wstring( _DEC_WS_2CECF817_Type.c_str() ) );
			wsCsv.push_back( L',' );
			AddCell( wsCsv, std::wstring( _DEC_WS_DCB67730_Value.c_str() ) );
			wsCsv.push_back( L'\r' );
			wsCsv.push_back( L'\n' );
			for ( int I = 0; I < int( m_vRows.size() ); ++I ) {
				AddCell( wsCsv, CellText( I, 0 ) );
				wsCsv.push_back( L',' );
				AddCell( wsCsv, CellText( I, 1 ) );
				wsCsv.push_back( L'\r' );
				wsCsv.push_back( L'\n' );
			}

			const CSecureWString swsFilter = _DEC_WS_ACE47C26_CSV_Files____csv__0__csv_0All_Files_______0____0_0;
			const CSecureWString swsExt = _DEC_WS_08A45257_csv;
			std::vector<wchar_t> vPath( 32 * 1024 );
			OPENFILENAMEW ofnSave = { sizeof( ofnSave ) };
			ofnSave.hwndOwner = Wnd();
			ofnSave.lpstrFilter = swsFilter.c_str();
			ofnSave.lpstrFile = vPath.data();
			ofnSave.nMaxFile = static_cast<DWORD>(vPath.size());
			ofnSave.lpstrDefExt = swsExt.c_str();
			ofnSave.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_ENABLESIZING;
			if ( !::GetSaveFileNameW( &ofnSave ) ) { return; }

			const std::string sUtf8 = ee::CExpEval::ToUtf8( wsCsv );
			HANDLE hFile = ::CreateFileW( vPath.data(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
			if ( hFile == INVALID_HANDLE_VALUE ) {
				Status( _DEC_WS_38247799_Could_not_create_the_CSV_file_, true );
				return;
			}
			DWORD dwWritten = 0;
			const bool bOk = ::WriteFile( hFile, sUtf8.data(), static_cast<DWORD>(sUtf8.size()), &dwWritten, nullptr ) && dwWritten == sUtf8.size();
			::CloseHandle( hFile );
			if ( !bOk ) {
				Status( _DEC_WS_64F720ED_Could_not_write_the_CSV_file_, true );
				return;
			}
			std::wstring wsDone = _DEC_WS_1695187A_Exported__;
			wsDone += vPath.data();
			Status( CSecureWString( wsDone ), false );
		}
		catch ( ... ) {
			Status( _DEC_WS_F39F91A5_Out_of_memory_, true );
		}
	}

	/**
	 * Shows the Customize dialog and applies the settings chosen.
	 **/
	void CInspectorWindow::Customize() {
		if ( !m_pihHost ) { return; }
		EndEdit( false );
		try {
			CInspectorLayout::MX_CUSTOMIZE_PARMS cpParms;
			cpParms.sSettings = m_pihHost->InspectorSettings();
			if ( CInspectorLayout::CreateCustomizeDialog( this, cpParms ) ) {
				CInspector::MX_SETTINGS & sSettings = m_pihHost->InspectorSettings();
				const bool bAllowDocking = sSettings.bAllowDocking;
				sSettings = cpParms.sSettings;
				sSettings.bAllowDocking = bAllowDocking;
				Rebuild();
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Shows a message in the status bar of the host.
	 *
	 * \param _swsText The message.
	 * \param _bWarning If true, the message is a warning.
	 **/
	void CInspectorWindow::Status( const CSecureWString &_swsText, bool _bWarning ) {
		if ( m_pihHost ) { m_pihHost->InspectorStatus( _swsText, _bWarning ); }
	}

	/**
	 * The subclass procedure of the list view (clicks and keys that edit and copy).
	 *
	 * \param _hWnd The list view.
	 * \param _uMsg The message.
	 * \param _wParam The message's WPARAM.
	 * \param _lParam The message's LPARAM.
	 * \param _uIdSubclass The subclass ID.
	 * \param _dwRefData The CInspectorWindow.
	 * \return Returns the result of the message.
	 **/
	LRESULT CALLBACK CInspectorWindow::ListSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR /*_uIdSubclass*/, DWORD_PTR _dwRefData ) {
		CInspectorWindow * piwThis = reinterpret_cast<CInspectorWindow *>(_dwRefData);
		switch ( _uMsg ) {
			case WM_LBUTTONDOWN : {
				const int iSelBefore = piwThis->SelectedRow();
				const bool bWasEditing = piwThis->m_hEdit != NULL;
				LVHITTESTINFO htiHit = {};
				htiHit.pt = { GET_X_LPARAM( _lParam ), GET_Y_LPARAM( _lParam ) };
				const int iHit = ListView_SubItemHitTest( _hWnd, &htiHit );
				const LRESULT lrRet = ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam );
				if ( !bWasEditing && iHit >= 0 && iHit == iSelBefore && htiHit.iSubItem == 1 && !(_wParam & (MK_SHIFT | MK_CONTROL)) &&
					::GetCapture() != _hWnd ) {
					piwThis->BeginEdit( iHit );
				}
				else if ( iHit >= 0 && iHit != iSelBefore && !(_wParam & (MK_SHIFT | MK_CONTROL)) ) {
					piwThis->SelectInHexEditor( iHit );
				}
				return lrRet;
			}
			case WM_LBUTTONDBLCLK : {
				const LRESULT lrRet = ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam );
				LVHITTESTINFO htiHit = {};
				htiHit.pt = { GET_X_LPARAM( _lParam ), GET_Y_LPARAM( _lParam ) };
				const int iHit = ListView_SubItemHitTest( _hWnd, &htiHit );
				if ( iHit >= 0 ) { piwThis->BeginEdit( iHit ); }
				return lrRet;
			}
			case WM_KEYDOWN : {
				if ( _wParam == VK_RETURN || _wParam == VK_F2 ) {
					piwThis->BeginEdit( piwThis->SelectedRow() );
					return 0;
				}
				if ( _wParam == 'C' && (::GetKeyState( VK_CONTROL ) & 0x8000) ) {
					piwThis->CopyRow();
					return 0;
				}
				break;
			}
			case WM_CHAR : {
				// No beep for Enter.
				if ( _wParam == L'\r' ) { return 0; }
				break;
			}
			case WM_VSCROLL : {}								MX_FALLTHROUGH
			case WM_HSCROLL : {}								MX_FALLTHROUGH
			case WM_MOUSEWHEEL : {
				if ( piwThis->m_hEdit ) { piwThis->EndEdit( false ); }
				break;
			}
			case WM_NCDESTROY : {
				::RemoveWindowSubclass( _hWnd, ListSubclassProc, MX_INSPECTOR_LIST_SUBCLASS );
				break;
			}
		}
		return ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam );
	}

	/**
	 * The subclass procedure of the in-place edit box (Enter writes, Escape cancels, losing the focus cancels).
	 *
	 * \param _hWnd The edit box.
	 * \param _uMsg The message.
	 * \param _wParam The message's WPARAM.
	 * \param _lParam The message's LPARAM.
	 * \param _uIdSubclass The subclass ID.
	 * \param _dwRefData The CInspectorWindow.
	 * \return Returns the result of the message.
	 **/
	LRESULT CALLBACK CInspectorWindow::EditSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR /*_uIdSubclass*/, DWORD_PTR _dwRefData ) {
		CInspectorWindow * piwThis = reinterpret_cast<CInspectorWindow *>(_dwRefData);
		switch ( _uMsg ) {
			case WM_GETDLGCODE : {
				return ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam ) | DLGC_WANTALLKEYS;
			}
			case WM_KEYDOWN : {
				// The edit box is closed outside of its own window procedure.
				if ( _wParam == VK_RETURN ) {
					if ( piwThis->m_hEdit == _hWnd ) { ::PostMessageW( piwThis->Wnd(), MX_IM_EDIT_DONE, MX_ED_COMMIT, static_cast<LPARAM>(piwThis->m_ui32EditId) ); }
					return 0;
				}
				if ( _wParam == VK_ESCAPE ) {
					if ( piwThis->m_hEdit == _hWnd ) { ::PostMessageW( piwThis->Wnd(), MX_IM_EDIT_DONE, MX_ED_CANCEL, static_cast<LPARAM>(piwThis->m_ui32EditId) ); }
					return 0;
				}
				break;
			}
			case WM_CHAR : {
				// No beeps for Enter and Escape.
				if ( _wParam == L'\r' || _wParam == 0x1B ) { return 0; }
				break;
			}
			case WM_KILLFOCUS : {
				if ( piwThis->m_hEdit == _hWnd ) { ::PostMessageW( piwThis->Wnd(), MX_IM_EDIT_DONE, MX_ED_FOCUS_LOST, static_cast<LPARAM>(piwThis->m_ui32EditId) ); }
				break;
			}
			case WM_NCDESTROY : {
				::RemoveWindowSubclass( _hWnd, EditSubclassProc, MX_INSPECTOR_EDIT_SUBCLASS );
				break;
			}
		}
		return ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam );
	}

}	// namespace mx
